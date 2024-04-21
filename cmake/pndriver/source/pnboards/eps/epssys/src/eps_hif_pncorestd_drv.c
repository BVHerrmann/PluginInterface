/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_hif_pncorestd_drv.c                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF PnCoreStd Drv                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20056
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include <psi_inc.h>

/* EPS includes */
#include <eps_sys.h>               /* Types / Prototypes / Fucns */
#include <eps_trc.h>               /* Tracing                    */
#include <eps_locks.h>             /* EPS Locks                  */
#include <eps_hif_drv_if.h>        /* HIF-Driver Interface       */
#include <eps_hif_pncorestd_drv.h> /* Own Header                 */
#include <eps_cp_mem.h>
#include <eps_tasks.h>

#include <eps_rtos.h>              /* OS interface               */
#include <PnCore_Inc.h>            /* PnCore Interface           */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
#include <eps_adonis_isr_drv.h>
#endif

#define EPS_HIF_PNCORESTD_DRV_MAX_INSTANCES 1

#define EPS_HIF_PNCORESTD_DRV_SIG_ISR   (SIGRTMAX-5)
#define EPS_HIF_PNCORESTD_DRV_SIG_STOP	(SIGRTMAX-6)

LSA_VOID   eps_hif_pncorestd_drv_uninstall       (LSA_VOID);
LSA_UINT16 eps_hif_pncorestd_drv_open            (EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_TYPE* ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
LSA_UINT16 eps_hif_pncorestd_drv_close           (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_pncorestd_drv_enable_isr      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif);
LSA_UINT16 eps_hif_pncorestd_drv_disable_isr     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_pncorestd_drv_send_hif_ir     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_pncorestd_drv_ack_hif_ir      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_UINT16 eps_hif_pncorestd_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
LSA_VOID   eps_hif_pncorestd_drv_dmacpy          (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength);
LSA_BOOL   eps_hif_pncorestd_drv_dma_pend_free   (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree);
LSA_VOID   eps_hif_pncorestd_drv_get_params      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar);

typedef struct
{
	LSA_BOOL bUsed;
	LSA_BOOL bIsrEnabled;
	LSA_UINT32 hOwnHandle;
	LSA_UINT32 hIsrThread;
	LSA_UINT16 hHif;
	LSA_UINT8* pIcuBase;
	uPNCORE_INT_PAR IntParIcuHost;
	uPNCORE_INT_PAR IntParIcuAsic;
	int IcuFd;
	LSA_UINT32 uCntSendIsr;
	LSA_UINT32 uCntIsrs;
	LSA_UINT32 uCntIsrAcks;
	LSA_UINT32 uCntIsrReenables;
	LSA_UINT32 uCntDmaCpys;
	EPS_HIF_DRV_OPENPAR_TYPE InPar;
	EPS_HIF_DRV_HW_TYPE sHw;
#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	EPS_ADONIS_ISR_DRV_HIF_IOCTL_ARG_TYPE IsrDrvArgs;
#endif
} EPS_HIF_PNCORESTD_DRV_INST_TYPE, *EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE;

typedef struct
{
	LSA_UINT16 hEnterExit;
	LSA_BOOL   bInit;
	LSA_UINT32 uCntInterfaceInstalls;

	EPS_HIF_PNCORESTD_DRV_INST_TYPE Inst[EPS_HIF_PNCORESTD_DRV_MAX_INSTANCES];
} EPS_HIF_PNCORESTD_DRV_TYPE, *EPS_HIF_PNCORESTD_DRV_PTR_TYPE;

EPS_HIF_PNCORESTD_DRV_TYPE g_EpsHifPnCoreStdDrv;
EPS_HIF_PNCORESTD_DRV_PTR_TYPE g_pEpsHifPnCoreStdDrv = LSA_NULL;

/**
 * This is the ISR thread for the PNCore driver. This function has an endless loop that checks for events.
 * 
 * @see hif_interrupt   - this function is called whenever an HIF interrupt occurs.
 * 
 * @param [in] uParam   - not used
 * @param [in] pArgs    - casted to instance handle
 * @return LSA_VOID
 */
LSA_VOID eps_hif_pncorestd_hif_isr_thread(LSA_UINT32 uParam, LSA_VOID* pArgs)
{    
	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = (EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE)pArgs;

	sigset_t isr_event;
	int info;

	LSA_UNUSED_ARG(uParam);
	
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);

	sigemptyset(&isr_event);
	sigaddset(&isr_event, EPS_HIF_PNCORESTD_DRV_SIG_ISR); /* Isr Signal  */
	sigaddset(&isr_event, EPS_HIF_PNCORESTD_DRV_SIG_STOP); /* Kill Signal */

	while(1)
	{	
		sigwait(&isr_event, &info); /* Wait on signal */

		if(info == EPS_HIF_PNCORESTD_DRV_SIG_ISR)
		{
			EPS_ASSERT(pInst->bUsed == LSA_TRUE);
			EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

			#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_POLL)
			if(fnPnCore_CheckIntIrr(pInst->pIcuBase, &pInst->IntParIcuAsic))

			{
			#endif

				pInst->uCntIsrs++;

				hif_interrupt(pInst->hHif);

			#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_POLL)
			}
			#endif
		}
		else
		{
			break;
		}
	}
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_pncorestd_drv_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsHifPnCoreStdDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_pncorestd_drv_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsHifPnCoreStdDrv->hEnterExit, LSA_FALSE);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_pncorestd_drv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsHifPnCoreStdDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_pncorestd_drv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsHifPnCoreStdDrv->hEnterExit);
}

/** 
 * Allocates a instance of the HIF PNCore driver.
 * 
 * @param LSA_VOID
 * @return EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE - handle to instance
 */
static EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE eps_hif_pncorestd_drv_alloc_instance(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;

	for(i=0; i<EPS_HIF_PNCORESTD_DRV_MAX_INSTANCES; i++)
	{
		pInst = &g_pEpsHifPnCoreStdDrv->Inst[i];

		if(pInst->bUsed == LSA_FALSE)
		{
			pInst->hOwnHandle       = i;
			pInst->bIsrEnabled      = LSA_FALSE;
			pInst->uCntDmaCpys      = 0;
			pInst->uCntIsrAcks      = 0;
			pInst->uCntIsrs         = 0;
			pInst->uCntIsrReenables = 0;
			pInst->uCntSendIsr      = 0;
			pInst->bUsed = LSA_TRUE;

			return pInst;
		}
	}

	return pInst;
}

/**
 * Releases an instance.
 * @param [in] pInst - instance to be freed
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_pncorestd_drv_free_instance(EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst)
{
	EPS_ASSERT(pInst != LSA_NULL);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	pInst->bUsed = LSA_FALSE;
}

/**
 * Returns the instance.
 * 
 * @param [in] pHwInstIn                        - handle to instance
 * @return EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE  - the pointer the struct of the instance
 */
static EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE eps_hif_pncorestd_drv_get_inst (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UINT32 index = (LSA_UINT32)pHwInstIn->hHandle;
	
	return &g_pEpsHifPnCoreStdDrv->Inst[index];
}

/**
 * Installs the HIF PNCore  Driver by calling eps_hif_drv_if_register. Does a function mapping from the API defined in eps_hif_drv_if.
 * 
 * The HIF PNCore Driver is a interrupt driven HIF driver that uses the PNCore API provided by the PNDevDriver. 
 * The driver requires interrupt registration in the system, this is done using the eps_adonis_isr_drv module.
 * 
 
 * 
 * Set the HIF Driver capabilites
 *    bUseIpcInterrupts     := FALSE
 *    bUsePnCoreInterrupts  := TRUE
 *    bUseRemoteMem         := FALSE
 *    bUseDma               := FALSE
 *    bUseShortMode         := FALSE
 *    bUseSharedMemMode     := TRUE
 * 
 * @see eps_adonis_isr_drv_install  - This module implements the kernel mode functionality (register interrupt line...)
 * @see eps_hif_drv_if_register     - this function is used to register the driver implementation to the EPS HIF Drv IF.
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_hif_pncorestd_drv_install (LSA_VOID)
{
	EPS_HIF_DRV_IF_TYPE sHifDrvIf;
	EPS_HIF_DRV_CFG_TYPE sCfg;

	//Init Internals

	eps_memset(&g_EpsHifPnCoreStdDrv, 0, sizeof(g_EpsHifPnCoreStdDrv));
	g_pEpsHifPnCoreStdDrv = &g_EpsHifPnCoreStdDrv;

	eps_hif_pncorestd_drv_init_critical_section();

	eps_memset(&sHifDrvIf, 0, sizeof(sHifDrvIf));

	//Init Interface
	sHifDrvIf.open      = eps_hif_pncorestd_drv_open;
	sHifDrvIf.close     = eps_hif_pncorestd_drv_close;
	sHifDrvIf.uninstall = eps_hif_pncorestd_drv_uninstall;

	sHifDrvIf.AckIsr        = eps_hif_pncorestd_drv_ack_hif_ir;
	sHifDrvIf.DisableIsr    = eps_hif_pncorestd_drv_disable_isr;
	sHifDrvIf.EnableIsr     = eps_hif_pncorestd_drv_enable_isr;
	sHifDrvIf.ReenableIsr   = eps_hif_pncorestd_drv_reenable_hif_ir;
	sHifDrvIf.SendIsr       = eps_hif_pncorestd_drv_send_hif_ir;
	sHifDrvIf.DmaCpy        = eps_hif_pncorestd_drv_dmacpy;
	sHifDrvIf.DmaPendFree   = eps_hif_pncorestd_drv_dma_pend_free;
	sHifDrvIf.GetParams     = eps_hif_pncorestd_drv_get_params;

	g_pEpsHifPnCoreStdDrv->bInit = LSA_TRUE;

	eps_hif_drv_init_config(&sCfg);

	sCfg.bUseSharedMemMode    = LSA_TRUE;
	sCfg.bUsePnCoreInterrupts = LSA_TRUE;

	g_pEpsHifPnCoreStdDrv->uCntInterfaceInstalls++;
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);

	sCfg.bUseRemoteMem = LSA_TRUE;

	g_pEpsHifPnCoreStdDrv->uCntInterfaceInstalls++;
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
}

/**
 * Uninstalls the driver by setting bInit to FALSE and deleting the pointer.
 * Releases locks.
 * 
 * @see eps_hif_drv_if_undo_init - calls this function
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hif_pncorestd_drv_uninstall (LSA_VOID)
{
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->uCntInterfaceInstalls > 0);

	g_pEpsHifPnCoreStdDrv->uCntInterfaceInstalls--;

	if(g_pEpsHifPnCoreStdDrv->uCntInterfaceInstalls == 0)
	{
		eps_hif_pncorestd_drv_undo_init_critical_section();

		g_pEpsHifPnCoreStdDrv->bInit = LSA_FALSE;

		g_pEpsHifPnCoreStdDrv = LSA_NULL;
	}
}

/**
 * eps_hif_drv_if_open implementation of the EPS HIF PNCore Drv.
 *
 * @see eps_hif_drv_if_open         - calls this function
 * @see eps_adonis_isr_drv_install  - implements the kernel mode functionality used by EPS HIF PNCore driver
 *  
 * @param [in] pInPars              - generic in structure for all EPS HIF drivers         
 * @param [out] ppHwInstOut         - handle to the instance
 * @param [in] hd_id                - 0 = LD, 1...4 = HD
 * @param [in] bUpper               - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_open (EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst;
	ePNCORE_ASIC eAsic = ePNCORE_ASIC_INVALID;

	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(pInPars != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_pncorestd_drv_open - Using EPS HIF PNCORE Drv. hd_id=%d, isUpper=%d", hd_id, bUpper);

	eps_hif_pncorestd_drv_enter();

	pInst = eps_hif_pncorestd_drv_alloc_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	pInst->InPar       = *pInPars;
	pInst->sHw.hd_id   = hd_id;
	pInst->sHw.bUpper  = bUpper;
	pInst->sHw.hHandle = pInst->hOwnHandle;

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
	eAsic = ePNCORE_ASIC_SOC1;
	pInst->pIcuBase = (LSA_UINT8*)0x1D200000;
#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)
	eAsic = ePNCORE_ASIC_ERTEC200P;
	pInst->pIcuBase = (LSA_UINT8*)0x10600000;
#elif (EPS_PLF == EPS_PLF_PNIP_A53)
    eAsic = ePNCORE_ASIC_HERA;
    pInst->pIcuBase = (LSA_UINT8*)0xA0400000;   //TODO hera; mk5656: Pointer auf Interface A
#elif (EPS_PLF == EPS_PLF_ERTEC400_ARM9)
	eAsic = ePNCORE_ASIC_ERTEC400;
	pInst->pIcuBase = (LSA_UINT8*)0x10000000;
#elif (EPS_PLF == EPS_PLF_ERTEC200_ARM9)
	eAsic = ePNCORE_ASIC_ERTEC200;
	pInst->pIcuBase = (LSA_UINT8*)0x10000000;
#endif

    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	pInst->IcuFd = open(EPS_ADONIS_ISR_DRV_NAME, O_RDWR);	
	EPS_ASSERT(pInst->IcuFd != -1);
	#endif

	if(!fnPnCore_GetIntPar(eAsic, ePNCORE_ICU_HOST, &pInst->IntParIcuHost))
	{
		EPS_FATAL();
	}
	if(!fnPnCore_GetIntPar(eAsic, ePNCORE_ICU_ASIC, &pInst->IntParIcuAsic))
	{
		EPS_FATAL();
	}

	*ppHwInstOut = &pInst->sHw;

	eps_hif_pncorestd_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function closes the instance of the HIF PNCore driver 
 * 
 * @see eps_hif_drv_if_close  - calls this function
 * 
 * @param [in] pHwInstIn      - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_close (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
    
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_hif_pncorestd_drv_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	close(pInst->IcuFd);
	#endif

	eps_hif_pncorestd_drv_free_instance(pInst);

	eps_hif_pncorestd_drv_exit();

	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_pncorestd_drv_close - done");

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function enable interrupts using the PNCore API.
 * 
 * @see eps_hif_drv_if_enable_isr - calls this function
 * @see eps_adonis_isr_drv_ioctl    - This Kernel Mode function is called here.
 * 
 * @param [in] pHwInstIn  - handle to the instance
 * @param [i] hHif        - handle to HIF
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_enable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif)
{
    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	LSA_INT retVal;
	#endif

	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
	
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_hif_pncorestd_drv_enter();

	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

	pInst->hHif = hHif;
	pInst->bIsrEnabled = LSA_TRUE;

	pInst->hIsrThread = eps_tasks_start_thread("EPS_PNCOREHIFISR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, eps_hif_pncorestd_hif_isr_thread, 0, pInst, eRUN_ON_1ST_CORE );

	fnPnCore_UnmaskInt(pInst->pIcuBase, &pInst->IntParIcuAsic);

    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	pInst->IsrDrvArgs.hThread = (pthread_t)eps_tasks_get_os_handle(pInst->hIsrThread);
	pInst->IsrDrvArgs.sig = EPS_HIF_PNCORESTD_DRV_SIG_ISR;
	retVal = ioctl(pInst->IcuFd ,EPS_ADONIS_ISR_DRV_ICU_HIF_ENABLE, (LSA_VOID*)&pInst->IsrDrvArgs);		
	EPS_ASSERT(retVal == 0);
	#endif

	eps_hif_pncorestd_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function disables interrupts using the PNCore API.
 * 
 * @see eps_hif_drv_if_disable_isr  - calls this function
 * @see eps_adonis_isr_drv_ioctl    - This Kernel Mode function is called here.
 * 
 * @param [in] pHwInstIn            - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_disable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	LSA_INT retVal;
	#endif
	
	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;

	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_hif_pncorestd_drv_enter();

    #if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_IR_KERNELMODE)
	retVal = ioctl(pInst->IcuFd ,EPS_ADONIS_ISR_DRV_ICU_HIF_DISABLE, LSA_NULL);		
	EPS_ASSERT(retVal == 0);
	#endif

	fnPnCore_MaskInt(pInst->pIcuBase, &pInst->IntParIcuAsic);

	//(LaM)ToDo Send signal to isr thread

	eps_hif_pncorestd_drv_exit();

	pInst->bIsrEnabled = LSA_FALSE;

	return EPS_HIF_DRV_RET_OK;
}

/**
 * Sends an interrupt using the PNCore API.
 * 
 * @see eps_hif_drv_if_send_isr - calls this function
 * 
 * @param [in] pHwInstIn        - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_send_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
    
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	pInst->uCntSendIsr++;

	fnPnCore_SetInt(pInst->pIcuBase, &pInst->IntParIcuAsic);

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function acknowledges interrupts using the PNCore API.
 * 
 * @see eps_hif_drv_if_ack_isr  - calls this function
 * 
 * @param [in] pHwInstIn        - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
LSA_UINT16 eps_hif_pncorestd_drv_ack_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
    
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_hif_pncorestd_drv_enter();

	pInst->uCntIsrAcks++;

	#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_POLL)
	if(fnPnCore_CheckIntIrr(pInst->pIcuBase, &pInst->IntParIcuAsic))
	{
	#endif

		fnPnCore_AckInt(pInst->pIcuBase, &pInst->IntParIcuAsic);		
		fnPnCore_SetEoi(pInst->pIcuBase, &pInst->IntParIcuAsic, 100);
				
	#if (EPS_ISR_MODE_HIF_PNCORE == EPS_ISR_MODE_POLL)
	}
	#endif	
	
	eps_hif_pncorestd_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function reenables interrupts using the PNCore API.
 * 
 * @see eps_hif_drv_if_reenable_isr - calls this function
 * 
 * @param [in] pHwInstIn            - handle to the instance
 * @return
 */
LSA_UINT16 eps_hif_pncorestd_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
    
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_hif_pncorestd_drv_enter();

	pInst->uCntIsrReenables++;

	if(pInst->bIsrEnabled == LSA_TRUE)
	{
		fnPnCore_UnmaskInt(pInst->pIcuBase, &pInst->IntParIcuAsic);
	}

	eps_hif_pncorestd_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * Copies data from host to shared-mem without swapping using memcpy.
 * 
 * @see eps_hif_drv_if_dma_copy       - calls this function
 * @param [in] pHwInstIn              - handle to instance
 * @param [in] ppData                 - not used
 * @param [in] uLength                - not used
 * @return LSA_VOID
 */
LSA_VOID eps_hif_pncorestd_drv_dmacpy (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* const pSrc, LSA_UINT32 uLength)
{
    EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;
    LSA_UINT8* pSrcData;
    LSA_UINT8* pDstData;
    LSA_UINT32* pCopyFinsihedFlag;
    
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);
    
    pSrcData = pSrc;
    pCopyFinsihedFlag = (LSA_UINT32*)(pDst);
    pDstData = (LSA_UINT8*)((((LSA_UINT8*)(pDst)) + 4));

    // invalidate the copy status flag
    *pCopyFinsihedFlag = 0;
    // copy data
    eps_memcpy(pDstData, pSrcData, uLength);
    // set copy status flag
    *pCopyFinsihedFlag = HIF_DMA_FINISHED_FLAG;

	pInst->uCntDmaCpys++;

	/* This is the non dma variant => no HW support */
}


/** 
 * This function pends a free for allocated buffer which is in use by dma.
 * Since no dma is used for copy in EPS HIF PNCore Drv this function will always return LSA_FALSE.
 * 
 * @see eps_hif_drv_if_dma_pend_free - calls this function
 * @param [in] pHwInstIn             - handle to the instance
 * @param [in] pBuffer               - pointer to the buffer to be freed
 * @param [out] ppReadyForFree       - pointer to where dma writes HIF_DMA_FREE_BUF_FLAG, 
 *                                     when buffer is not longer in use by dma.
 * @return #LSA_FALSE                - no dma is used, buffer can be freed immediate, 
 *                                     do not pend a free return LSA_FALSE.
 */
LSA_BOOL eps_hif_pncorestd_drv_dma_pend_free (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(pBuffer);

    EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    // No dma is used
    *ppReadyForFree = LSA_NULL;
    return LSA_FALSE;
}

/**
 * GetParams implementation of the EPS HIF PNCore Drv.
 * Implements eps_hif_drv_if_get_params
 * 
 * The EPS HIF PNCore driver uses a shared memory and has access to interrupt mechanisms provided by the PNCore IF.
 * 
 * @see eps_hif_drv_if_get_params
 * 
 * @param [in] pHwInstIn            - handle to instance
 * @param [out] pPar                - casted to #HIF_GET_PAR_PTR_TYPE
 * @return LSA_VOID
 */
LSA_VOID eps_hif_pncorestd_drv_get_params (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar)
{
	HIF_GET_PAR_PTR_TYPE pHifParams = (HIF_GET_PAR_PTR_TYPE) pPar;
	EPS_HIF_PNCORESTD_DRV_INST_PTR_TYPE pInst = LSA_NULL;

	eps_hif_pncorestd_drv_enter();

	EPS_ASSERT(g_pEpsHifPnCoreStdDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifPnCoreStdDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHifParams != LSA_NULL);

	pInst = eps_hif_pncorestd_drv_get_inst(pHwInstIn);

	eps_memset(pHifParams, 0, sizeof(*pHifParams));

	pHifParams->Mode = pInst->InPar.Mode;

	pHifParams->if_pars.hif_lower_device = pInst->InPar.if_pars.hif_lower_device;

	pHifParams->if_pars.SharedMem.bUseTimerPoll = LSA_TRUE;
    pHifParams->if_pars.SharedMem.SendIrqMode = pInst->InPar.if_pars.SendIrqMode;
    pHifParams->if_pars.SharedMem.uNsWaitBeforeSendIrq = pInst->InPar.if_pars.uNsWaitBeforeSendIrq;

	pHifParams->if_pars.SharedMem.Local.Base = pInst->InPar.if_pars.SharedMem.Local.Base;
	pHifParams->if_pars.SharedMem.Local.Size = pInst->InPar.if_pars.SharedMem.Local.Size;
	pHifParams->if_pars.SharedMem.Remote.Base = pInst->InPar.if_pars.SharedMem.Remote.Base;
	pHifParams->if_pars.SharedMem.Remote.Size = pInst->InPar.if_pars.SharedMem.Remote.Size;

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_pncorestd_drv_get_params - Using EPS HIF PNCORE Drv. hd_id=%d, isUpper=%d", pInst->sHw.hd_id, pInst->sHw.bUpper); 
	
	eps_hif_pncorestd_drv_exit();
}

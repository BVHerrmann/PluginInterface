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
/*  F i l e               &F: eps_hif_ipcsiedma_drv.c                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS HIF Universal Drv                                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20058
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

#include <psi_inc.h>

/* EPS includes */
#include <eps_sys.h>                  /* Types / Prototypes / Fucns */
#include <eps_trc.h>                  /* Tracing                    */
#include <eps_locks.h>                /* EPS Locks                  */
#include <eps_hif_drv_if.h>           /* HIF-Driver Interface       */
#include <eps_hif_ipcsiedma_drv.h>    /* Own Header                 */
#include <eps_plf.h>                  /* Platform include           */
#include <eps_ipc_drv_if.h>           /* EPS IPC Interface          */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
#include <eps_gdma.h>                 /* GDMA Hw funcs              */
#endif

#if (EPS_PLF == EPS_PLF_PNIP_A53)
#include <eps_xgdma.h>                /* XGDMA Hw funcs             */
#endif

#define EPS_HIF_IPCSIEDMA_DRV_MAX_INSTANCES 10

static LSA_VOID   eps_hif_ipcsiedma_drv_uninstall       (LSA_VOID);
static LSA_UINT16 eps_hif_ipcsiedma_drv_open            (struct eps_hif_drv_openpar_tag const* pInPars, EPS_HIF_DRV_HW_PTR_TYPE* ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
static LSA_UINT16 eps_hif_ipcsiedma_drv_close           (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_ipcsiedma_drv_enable_isr      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif);
static LSA_UINT16 eps_hif_ipcsiedma_drv_disable_isr     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_ipcsiedma_drv_send_hif_ir     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_ipcsiedma_drv_ack_hif_ir      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_ipcsiedma_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_VOID   eps_hif_ipcsiedma_drv_dma_cpy         (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength);
static LSA_BOOL   eps_hif_ipcsiedma_drv_dma_pend_free   (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree);
static LSA_VOID   eps_hif_ipcsiedma_drv_get_params      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar);

typedef struct
{
    LSA_BOOL   bUsed;
    LSA_BOOL   bIsrEnabled;
    LSA_BOOL   bUseDma;
    LSA_BOOL   bAckPending;
    LSA_UINT32 hOwnHandle;
    LSA_UINT32 uCntSendIsr;
    LSA_UINT32 uCntIsrAcks;
    LSA_UINT32 uCntIsrReenables;
    LSA_UINT32 uCntDmaCpys;
    LSA_UINT32 uCntIsrs;
#if (EPS_PLF == EPS_PLF_SOC_MIPS)
    LSA_VOID*  hGdmaInst;
    LSA_UINT32 hGdmaTransfer;
#endif
    struct
    {
        LSA_UINT32 uSize;
    } HifWriteBuffer;
    struct
    {
        LSA_UINT32 hRx;
        LSA_UINT32 hTx;
    } Ipc;
    EPS_HIF_DRV_OPENPAR_TYPE InPar;
    EPS_HIF_DRV_HW_TYPE sHw;
    HIF_HANDLE hHif;
} EPS_HIF_IPCSIEDMA_DRV_INST_TYPE, *EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE;

typedef struct
{
    LSA_UINT16 hEnterExit;
    LSA_BOOL   bInit;
    LSA_UINT32 uCntRegistered;
    EPS_HIF_IPCSIEDMA_DRV_INST_TYPE Inst[EPS_HIF_IPCSIEDMA_DRV_MAX_INSTANCES];
} EPS_HIF_IPCSIEDMA_DRV_TYPE, *EPS_HIF_IPCSIEDMA_DRV_PTR_TYPE;

static EPS_HIF_IPCSIEDMA_DRV_TYPE g_EpsHifIpcSieDmaDrv;
static EPS_HIF_IPCSIEDMA_DRV_PTR_TYPE g_pEpsHifIpcSieDmaDrv = LSA_NULL;

/**
 * This is the ISR thread for the IPC SIEDMA driver. This function is called by EPS IPC whenever an interrupt occurs.
 * 
 * @see hif_interrupt   - this function is called whenever an HIF interrupt occurs.
 * 
 * @param [in] uParam   - not used
 * @param [in] pArgs    - casted to instance handle
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_hif_isr_thread(LSA_UINT32 uParam, LSA_VOID * pArgs)
{    
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE const pInst = (EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE)pArgs;
    
    LSA_UNUSED_ARG(uParam);

    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(pInst->bUsed == LSA_TRUE);
    EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

    pInst->uCntIsrs++;

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "eps_hif_ipcsiedma_hif_isr_thread(): uCntIsrs=%u, uCntIsrAcks=%u, bAckPending=%u", pInst->uCntIsrs, pInst->uCntIsrAcks, pInst->bAckPending);
    
    // only send new hif_interrupt if last one was acknowled
    if(pInst->bAckPending != LSA_TRUE)
    {
        pInst->bAckPending = LSA_TRUE;
        hif_interrupt(pInst->hHif);
    }
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_undo_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
    retVal = eps_free_critical_section(g_pEpsHifIpcSieDmaDrv->hEnterExit);
    EPS_ASSERT( LSA_RET_OK == retVal );
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
    retVal = eps_alloc_critical_section(&g_pEpsHifIpcSieDmaDrv->hEnterExit, LSA_FALSE);
    EPS_ASSERT( LSA_RET_OK == retVal );
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_enter(LSA_VOID)
{
    eps_enter_critical_section(g_pEpsHifIpcSieDmaDrv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_exit(LSA_VOID)
{
    eps_exit_critical_section(g_pEpsHifIpcSieDmaDrv->hEnterExit);
}

/** 
 * Allocates a instance of the HIF IPC SIEDMA Driver.
 * 
 * @param LSA_VOID
 * @return EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE - handle to instance
 */
static EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE eps_hif_ipcsiedma_drv_alloc_instance(LSA_VOID)
{
    LSA_UINT32                          i;
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst = LSA_NULL;

    for(i=0; i<EPS_HIF_IPCSIEDMA_DRV_MAX_INSTANCES; i++)
    {
        pInst = &g_pEpsHifIpcSieDmaDrv->Inst[i];

        if(pInst->bUsed == LSA_FALSE)
        {
            pInst->hOwnHandle        = i;
            pInst->bIsrEnabled       = LSA_FALSE;
            pInst->bAckPending       = LSA_FALSE;
            pInst->uCntDmaCpys       = 0;
            pInst->uCntIsrAcks       = 0;
            pInst->uCntIsrReenables  = 0;
            pInst->uCntSendIsr       = 0;
            pInst->uCntIsrs          = 0;

            pInst->Ipc.hRx           = 0;
            pInst->Ipc.hTx           = 0;

            pInst->bUsed = LSA_TRUE;
            
            return pInst;
        }
    }

    return pInst;
}

/**
 * Releases an instance
 * @param [in] pInst - instance to be freed
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_free_instance(EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE const pInst)
{
    EPS_ASSERT(pInst != LSA_NULL);

    EPS_ASSERT(pInst->bUsed == LSA_TRUE);
    pInst->bUsed = LSA_FALSE;
}

/**
 * Returns the instance.
 * 
 * @param [in] pHwInstIn                        - handle to instance
 * @return EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE  - the pointer the struct of the instance
 */
static EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE eps_hif_ipcsiedma_drv_get_inst (EPS_HIF_DRV_HW_CONST_PTR_TYPE const pHwInstIn)
{
    LSA_UINT32 const index = (LSA_UINT32)pHwInstIn->hHandle;
    
    return &g_pEpsHifIpcSieDmaDrv->Inst[index];
}

/**
 * Installs the HIF IPC SIEDMA Driver by calling eps_hif_drv_if_register. Does a function mapping from the API defined in eps_hif_drv_if.
 * 
 * IPC    - Inter Processor Communication
 * SIEDMA - Siemens Direct Memory Access (intellectual property)
 * 
 * This driver supports the SOC1 GDMA, the ERTEC400 DMA and HERA XGDMA.
 * 
 * The IPC SIEDMA Driver is a interrupt driven HIF driver that uses DMA IPs on the ASICs. 
 * This driver uses the module eps_ipc_drv_if to implement the IPC functionality.
 * This driver also uses the module eps_gdma/eps_xgdma to implement the DMA functionality.
 * 
 * Set the HIF Driver capabilites
 *    bUseIpcInterrupts     := TRUE
 *    bUsePnCoreInterrupts  := FALSE
 *    bUseRemoteMem         := TRUE/FALSE
 *    bUseDma               := FALSE/TRUE
 *    bUseShortMode         := FALSE
 *    bUseSharedMemMode     := TRUE
 * 
 * if bUseDma is FALSE:
 *                      - Only the IPC part of this driver is active.
 *                      - RQB is serialized directly in transfer memory -> no dma copy!
 *                      - HIF only uses this driver for sending/receiving IRQs
 *              
 * 
 * @see eps_hif_drv_if_register     - this function is used to register the driver implementation to the EPS HIF Drv IF.
 * @see eps_ipc_install             - EPS IPC is used to provide the software interrupt
 * @see eps_gmda_init               - EPS GDMA configures and starts the GDMA
 * @see eps_xgmda_init              - EPS XGDMA configures and initializes the XGDMA
 * 
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_hif_ipcsiedma_drv_install (LSA_VOID)
{
    EPS_HIF_DRV_IF_TYPE sHifDrvIf;
    EPS_HIF_DRV_CFG_TYPE sCfg;

    //Init Internals

    eps_memset(&g_EpsHifIpcSieDmaDrv, 0, sizeof(g_EpsHifIpcSieDmaDrv));
    g_pEpsHifIpcSieDmaDrv = &g_EpsHifIpcSieDmaDrv;

    eps_hif_ipcsiedma_drv_init_critical_section();

    eps_memset(&sHifDrvIf, 0, sizeof(sHifDrvIf));

    //Init Interface
    sHifDrvIf.open      = eps_hif_ipcsiedma_drv_open;
    sHifDrvIf.close     = eps_hif_ipcsiedma_drv_close;
    sHifDrvIf.uninstall = eps_hif_ipcsiedma_drv_uninstall;

    sHifDrvIf.AckIsr        = eps_hif_ipcsiedma_drv_ack_hif_ir;
    sHifDrvIf.DisableIsr    = eps_hif_ipcsiedma_drv_disable_isr;
    sHifDrvIf.EnableIsr     = eps_hif_ipcsiedma_drv_enable_isr;
    sHifDrvIf.ReenableIsr   = eps_hif_ipcsiedma_drv_reenable_hif_ir;
    sHifDrvIf.SendIsr       = eps_hif_ipcsiedma_drv_send_hif_ir;
    sHifDrvIf.DmaCpy        = eps_hif_ipcsiedma_drv_dma_cpy;
    sHifDrvIf.DmaPendFree   = eps_hif_ipcsiedma_drv_dma_pend_free;
    sHifDrvIf.GetParams     = eps_hif_ipcsiedma_drv_get_params;

    g_pEpsHifIpcSieDmaDrv->bInit = LSA_TRUE;

    eps_hif_drv_init_config(&sCfg);

    sCfg.bUseIpcInterrupts    = LSA_TRUE;
    sCfg.bUseSharedMemMode    = LSA_TRUE;

    //Register driver without dma usage and without remote mem... (Soc1 Basic)
	sCfg.bUseDma			  = LSA_FALSE;
    eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
    g_pEpsHifIpcSieDmaDrv->uCntRegistered = 1;

	sCfg.bUseRemoteMem = LSA_TRUE;
	//Register driver without dma usage and with remote mem... (Soc1 Advanced)
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
	g_pEpsHifIpcSieDmaDrv->uCntRegistered++;

	//Register driver with dma usage and with remote mem... (Hera Basic and Advanced)
	sCfg.bUseDma = LSA_TRUE;
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
	g_pEpsHifIpcSieDmaDrv->uCntRegistered++;
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
static LSA_VOID eps_hif_ipcsiedma_drv_uninstall (LSA_VOID)
{
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->uCntRegistered > 0);
    
    g_pEpsHifIpcSieDmaDrv->uCntRegistered--;
    
    if(g_pEpsHifIpcSieDmaDrv->uCntRegistered == 0)
    {
	    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
	
	    eps_hif_ipcsiedma_drv_undo_init_critical_section();
	
	    g_pEpsHifIpcSieDmaDrv->bInit = LSA_FALSE;
	
	    g_pEpsHifIpcSieDmaDrv = LSA_NULL;
    }
}

/**
 * eps_hif_drv_if_open implementation of the EPS HIF IPC SIEDMA Drv.
 *
 * @see eps_hif_drv_if_open
 *
 * @param [in] pInPars               - generic in structure for all EPS HIF drivers
 * @param [out] ppHwInstOut          - handle to the instance
 * @param [in] hd_id                 - 0 = LD, 1...4 = HD
 * @param [in] bUpper                - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_open (EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;

    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(pInPars != LSA_NULL);
    EPS_ASSERT(ppHwInstOut != LSA_NULL);

    EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_ipcsiedma_drv_open - Using EPS HIF IPCSIEDMA Drv. hd_id=%d, isUpper=%d", hd_id, bUpper);

    eps_hif_ipcsiedma_drv_enter();

    pInst = eps_hif_ipcsiedma_drv_alloc_instance();
    EPS_ASSERT(pInst != LSA_NULL);

    pInst->InPar       = *pInPars;
    pInst->sHw.hd_id   = hd_id;
    pInst->sHw.bUpper  = bUpper;
    pInst->sHw.hHandle = pInst->hOwnHandle;

    pInst->Ipc.hRx = pInPars->drv_type_specific.pars.ipc.hIpcRx;
    pInst->Ipc.hTx = pInPars->drv_type_specific.pars.ipc.hIpcTx;

    *ppHwInstOut = &pInst->sHw;

    eps_hif_ipcsiedma_drv_exit();

    return EPS_HIF_DRV_RET_OK;
}

//z0031au DOCU
/**
 * This function closes the instance of the HIF IPC SIEDMA driver 
 * 
 * @see eps_hif_drv_if_close  - calls this function
 * @see eps_ipc_rx_free       - used by this function
 * @see eps_ipc_tx_free       - used by this function
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_close (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT( pInst != LSA_NULL );

    eps_hif_ipcsiedma_drv_enter();

    EPS_ASSERT(pInst->bUsed == LSA_TRUE);
    EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

    #if (EPS_PLF == EPS_PLF_SOC_MIPS)
    eps_gdma_undo_init(pInst->hGdmaInst);
    #endif

    #if (EPS_PLF == EPS_PLF_PNIP_A53)
    eps_xgdma_undo_init();
    #endif

    eps_ipc_drv_if_rx_free(pInst->Ipc.hRx, pHwInstIn->hd_id);
    eps_ipc_drv_if_tx_free(pInst->Ipc.hTx, pHwInstIn->hd_id);

    eps_hif_ipcsiedma_drv_free_instance(pInst);

    eps_hif_ipcsiedma_drv_exit();

	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_ipcsiedma_drv_close - done");

	return EPS_HIF_DRV_RET_OK;
}

/**
 * This function enable interrupts using the EPS_IPC implementation
 * 
 * @see eps_hif_drv_if_enable_isr - calls this function
 * @see eps_ipc_rx_enable         - used by this function
 * 
 * @param [in] pHwInstIn - handle to the instance
 * @param [i] hHif      - handle to HIF
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_enable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    eps_hif_ipcsiedma_drv_enter();

    EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

    pInst->hHif = hHif;
    pInst->bIsrEnabled = LSA_TRUE;

    eps_ipc_drv_if_rx_enable(pInst->Ipc.hRx, pHwInstIn->hd_id, eps_hif_ipcsiedma_hif_isr_thread, 0, pInst);

    eps_hif_ipcsiedma_drv_exit();

    return EPS_HIF_DRV_RET_OK;
}

/**
 * This function disables interrupts using the EPS_IPC implementation.
 * 
 * @see eps_hif_drv_if_disable_isr  - calls this function
 * @see eps_ipc_rx_disable          - used by this function
 * 
 * @param [in] pHwInstIn            - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_disable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    eps_hif_ipcsiedma_drv_enter();

    EPS_ASSERT(pInst->bIsrEnabled == LSA_TRUE);

    eps_ipc_drv_if_rx_disable(pInst->Ipc.hRx, pHwInstIn->hd_id);

    pInst->bIsrEnabled = LSA_FALSE;

    eps_hif_ipcsiedma_drv_exit();

    return EPS_HIF_DRV_RET_OK;
}

/**
 * Sends an interrupt by writing an value to the APIC of the other core.
 * 
 * @see eps_hif_drv_if_send_isr - calls this function
 * @see eps_ipc_tx_send         - used by this function
 * 
 * @param [in] pHwInstIn        - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_send_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    pInst->uCntSendIsr++;

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_hif_ipcsiedma_drv_send_hif_ir(): uCntSendIsr=%u", pInst->uCntSendIsr);
    
    eps_ipc_drv_if_tx_send(pInst->Ipc.hTx, pHwInstIn->hd_id);

    return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only does statistics since software interrupts can not be acknowledged.
 * 
 * @see eps_hif_drv_if_ack_isr - calls this function
 * 
 * @param [in] pHwInstIn        - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_ack_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    pInst->uCntIsrAcks++;
    // Acknowledge Interrupt -> allow next interrupt to execute hif_interrupt
    pInst->bAckPending = LSA_FALSE;

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"eps_hif_ipcsiedma_drv_ack_hif_ir uCntIsrAcks=%d, bAckPending=%d", pInst->uCntIsrAcks, pInst->bAckPending);

    /* IPC SW Interrupts don't need Ack */

    return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only does statistics.
 * 
 * @see eps_hif_drv_if_reenable_isr - calls this function
 * @param [in] pHwInstIn            - handle to the instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_ipcsiedma_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    pInst->uCntIsrReenables++;

    /* Reenable not needed in IPC Variant, because it doesn't use IRTE Interrupts */

    return EPS_HIF_DRV_RET_OK;
}

/** 
 * Copies data from host to shared-mem without swapping.
 * If driver supports dma it is used for better performance otherwise memcpy is used.
 * 
 * @see eps_hif_drv_if_dma_copy - calls this function
 * @param [in] pHwInstIn - handle to the instance
 * @param [in] pDst      - pointer to the destination
 * @param [in] pSrc      - pointer to the source
 * @param [in] uLength   - length of the data
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_dma_cpy (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength)
{
#if (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_SOC_MIPS)
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
#endif

    LSA_UINT8  * pSrcData;
    LSA_UINT8  * pDstData;
    LSA_UINT32 * pCopyFinsihedFlag;

    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);
   
    pSrcData = pSrc;
    pCopyFinsihedFlag = (LSA_UINT32*)(pDst);// - pInst->HifWriteBuffer.pBase);
    pDstData = (LSA_UINT8*)((((LSA_UINT8*)(pDst)) + 4));// - pInst->HifWriteBuffer.pBase);

    // invalidate the copy status flag
    *pCopyFinsihedFlag = 0;

#if (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_SOC_MIPS)
	pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);

    if(pInst->bUseDma && uLength > 100) // TODO measure overhead for xgdma/gdma 
    {
        pInst->uCntDmaCpys++;

        #if (EPS_PLF == EPS_PLF_PNIP_A53)
        // copy data with xgdma and append dma finished flag.
        eps_xgdma_hif_memcpy(pDstData, pSrcData, uLength, pCopyFinsihedFlag, HIF_DMA_FINISHED_FLAG);
        #elif (EPS_PLF == EPS_PLF_SOC_MIPS)
        eps_gdma_single_hif_transfer(pInst->hGdmaInst, pInst->hGdmaTransfer, (LSA_UINT32)pSrcData, (LSA_UINT32)pDstData, uLength); //TODO not tested without shadow memory yet!
        *pCopyFinsihedFlag = HIF_DMA_FINISHED_FLAG; // TODO implement in eps_gdma
        #endif
    }
    else /* DMA not used use Memcpy */
#endif   /* On x86 (and others) Memcpy is used */
    {
        // copy data
        eps_memcpy(pDstData, pSrcData, uLength);
        // set copy status flag
        *pCopyFinsihedFlag = HIF_DMA_FINISHED_FLAG;
    }
}

/** 
 * This function pends a free for allocated buffer which is in use by dma.
 * If no dma is used for copy this function will always return LSA_FALSE.
 * 
 * @see eps_hif_drv_if_dma_pend_free - calls this function
 * @param [in] pHwInstIn             - handle to the instance
 * @param [in] pBuffer               - pointer to the buffer to be freed
 * @param [out] ppReadyForFree       - pointer to where dma writes HIF_DMA_FREE_BUF_FLAG, 
 *                                     when buffer is not longer in use by dma.
 * @return #LSA_FALSE                - DMA did not pend a free for buffer, buffer is not in use by dma.
 *                                     If no hif dma transfers are pending or no dma is used,
 *                                     buffer can be freed immediate, do not pend a free return LSA_FALSE.           
 * @return #LSA_TRUE                 - DMA pended a free for buffer.
 */
static LSA_BOOL eps_hif_ipcsiedma_drv_dma_pend_free (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree)
{
#if (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_SOC_MIPS)
	EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE pInst;
#endif

    LSA_UNUSED_ARG(pBuffer);
    
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);
    
#if (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_SOC_MIPS)
	pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
	
	if(pInst->bUseDma)
    {
        #if (EPS_PLF == EPS_PLF_PNIP_A53)
        return eps_xgdma_hif_pend_free(pBuffer, ppReadyForFree, HIF_DMA_FREE_BUF_FLAG);
        #elif (EPS_PLF == EPS_PLF_SOC_MIPS)
        *ppReadyForFree = LSA_NULL;
        return LSA_FALSE; // TODO implement in eps_gdma
        #endif
    }
    else /* DMA not used, buffer was already copied with Memcpy */
#endif   /* On x86 (and others) Memcpy was used for copy */
    {
        // No dma is used buffer can be freed immediate.
        *ppReadyForFree = LSA_NULL;
        return LSA_FALSE;
    }
}

/**
 * GetParams implementation of the EPS HIF IPC SIEDMA Drv.
 *
 * The EPS HIF IPC SIEDMA driver uses EPS IPC for interprocessor software interrupts. Also, the DMA of the ASICs (e.g. SOC1 GDMA, ERTEC400 DMA) is used.
 *
 * @see eps_hif_drv_if_get_params - calls this function
 *
 * @param [in] pHwInstIn          - handle to instance
 * @param [out] pPar              - casted to #HIF_GET_PAR_PTR_TYPE
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_ipcsiedma_drv_get_params (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar)
{
    HIF_GET_PAR_PTR_TYPE                const pHifParams = (HIF_GET_PAR_PTR_TYPE)pPar;
    LSA_UINT32                                IpcHeaderLength;
    LSA_UINT32                                uRxPhyAddress;
    LSA_UINT32                                uRxMsgVal;
    LSA_UINT16                                ret_val;
    EPS_HIF_IPCSIEDMA_DRV_INST_PTR_TYPE       pInst;

    eps_hif_ipcsiedma_drv_enter();

    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifIpcSieDmaDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHifParams != LSA_NULL);

    pInst = eps_hif_ipcsiedma_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst != LSA_NULL);

    eps_memset(pHifParams, 0, sizeof(*pHifParams));
    
    pInst->bUseDma              = pInst->InPar.drv_type_specific.pars.siegdma.bUseDma;
    pInst->HifWriteBuffer.uSize = pInst->InPar.if_pars.SharedMem.Remote.Size;
    
    if(pInst->bUseDma)
    {
        /* Create GDMA Instance [start] */
        #if (EPS_PLF == EPS_PLF_SOC_MIPS)
        pInst->hGdmaInst = LSA_NULL;
        pInst->hGdmaInst = eps_gdma_init((LSA_UINT8*)0x1E100000, (LSA_UINT8*)0x1E100000, (LSA_UINT8*)0x00000000, (LSA_UINT8*)0x00000000, pInst->InPar.if_pars.SharedMem.Remote.Base, (LSA_UINT8*)pInst->InPar.drv_type_specific.pars.siegdma.RemoteShmPhysBaseAhb); /* Soc1 Onboard: Translation is needed because of MTU usage */
        
        pInst->hGdmaTransfer = eps_gdma_single_hif_transfer_init(pInst->hGdmaInst, pInst->sHw.hd_id, pInst->sHw.bUpper);
        #elif (EPS_PLF == EPS_PLF_PNIP_A53)
        ret_val = eps_xgdma_init(0xA0800000);
        EPS_ASSERT(ret_val == LSA_RET_OK);
        #else
        /* On x86 and others Memcpy is used */
        #endif
        /* Create GDMA Instance [end] */
    }

    pHifParams->Mode = pInst->InPar.Mode;

    pHifParams->if_pars.hif_lower_device = pInst->InPar.if_pars.hif_lower_device;

    pHifParams->if_pars.SharedMem.bUseTimerPoll = LSA_TRUE; /* We do need a timer because we are using the IPC Interrupts and maybe some of them are missing?*/
    pHifParams->if_pars.SharedMem.SendIrqMode = pInst->InPar.if_pars.SendIrqMode;
    pHifParams->if_pars.SharedMem.uNsWaitBeforeSendIrq = pInst->InPar.if_pars.uNsWaitBeforeSendIrq;

    if(pInst->InPar.drv_type_specific.pars.ipc.bUsehIpcRx == LSA_FALSE)
    {
        ret_val = eps_ipc_drv_if_rx_alloc(&pInst->Ipc.hRx, &uRxPhyAddress, &uRxMsgVal, pHwInstIn->hd_id);
        EPS_ASSERT(ret_val == EPS_IPC_RET_OK);
    }

    IpcHeaderLength = eps_ipc_drv_if_sizeof_shm_data();
    eps_ipc_drv_if_rx_set_shm (pInst->Ipc.hRx, pHwInstIn->hd_id, pInst->InPar.if_pars.SharedMem.Local.Base, pInst->InPar.if_pars.SharedMem.Remote.Base, IpcHeaderLength);

    if(pInst->InPar.drv_type_specific.pars.ipc.bUsehIpcTx == LSA_FALSE)
    {
        ret_val = eps_ipc_drv_if_tx_alloc_from_local_shm(&pInst->Ipc.hTx, pInst->InPar.if_pars.SharedMem.Local.Base, IpcHeaderLength, pHwInstIn->hd_id);
        EPS_ASSERT(ret_val == EPS_IPC_RET_OK);
    }

    eps_ipc_drv_if_tx_set_shm (pInst->Ipc.hTx, pHwInstIn->hd_id, pInst->InPar.if_pars.SharedMem.Local.Base, pInst->InPar.if_pars.SharedMem.Remote.Base, IpcHeaderLength);

    pHifParams->if_pars.SharedMem.Local.Base  = pInst->InPar.if_pars.SharedMem.Local.Base  + IpcHeaderLength;
    pHifParams->if_pars.SharedMem.Local.Size  = pInst->InPar.if_pars.SharedMem.Local.Size  - IpcHeaderLength;
    pHifParams->if_pars.SharedMem.Remote.Base = pInst->InPar.if_pars.SharedMem.Remote.Base + IpcHeaderLength;
    pHifParams->if_pars.SharedMem.Remote.Size = pInst->InPar.if_pars.SharedMem.Remote.Size - IpcHeaderLength;

    EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_ipcsiedma_drv_get_params - Using EPS HIF IPCSIEDMA Drv. hd_id=%d, isUpper=%d", pInst->sHw.hd_id, pInst->sHw.bUpper);

    eps_hif_ipcsiedma_drv_exit();
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

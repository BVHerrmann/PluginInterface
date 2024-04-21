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
/*  F i l e               &F: eps_hif_universal_drv.c                   :F&  */
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

#define LTRC_ACT_MODUL_ID  20059
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* EPS includes */
#include <eps_sys.h>               /* Types / Prototypes / Fucns */
#include <eps_trc.h>               /* Tracing                    */
#include <eps_locks.h>             /* EPS Locks                  */
#include <eps_hif_drv_if.h>        /* HIF-Driver Interface       */
#include <eps_hif_universal_drv.h> /* Own Header                 */

#if ( EPS_CFG_USE_HIF == 1 )


#define EPS_HIF_UNIVERSAL_DRV_MAX_INSTANCES 10

static LSA_VOID   eps_hif_universal_drv_uninstall       (LSA_VOID);
static LSA_UINT16 eps_hif_universal_drv_open            (struct eps_hif_drv_openpar_tag const* pInPars, EPS_HIF_DRV_HW_PTR_TYPE* ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper);
static LSA_UINT16 eps_hif_universal_drv_close           (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_universal_drv_enable_isr      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif);
static LSA_UINT16 eps_hif_universal_drv_disable_isr     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_universal_drv_send_hif_ir     (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_universal_drv_ack_hif_ir      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16 eps_hif_universal_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_VOID   eps_hif_universal_drv_dmacpy          (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength);
static LSA_BOOL   eps_hif_universal_drv_dma_pend_free   (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree);
static LSA_VOID   eps_hif_universal_drv_get_params      (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar);

typedef struct
{
	LSA_BOOL   bUsed;               ///< Instance in use?
	LSA_BOOL   bIsrEnabled;         ///< ISRs enabled?
	LSA_UINT32 hOwnHandle;          ///< handle to itself
	LSA_UINT32 uCntSendIsr;         ///< statistics
	LSA_UINT32 uCntIsrAcks;         ///< statistics
	LSA_UINT32 uCntIsrReenables;    ///< statistics
	LSA_UINT32 uCntDmaCpys;         ///< statistics
	EPS_HIF_DRV_OPENPAR_TYPE InPar; ///< generic in structure for all EPS HIF Drivers
	EPS_HIF_DRV_HW_TYPE sHw;        ///< generic struct to see where the driver is used (LD/HD - Upper/Lower)
} EPS_HIF_UNIVERSAL_DRV_INST_TYPE, *EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE;

typedef struct
{
	LSA_UINT16 hEnterExit;
	LSA_BOOL   bInit;
	LSA_UINT32 uCntInterfaceInstalls;
	EPS_HIF_UNIVERSAL_DRV_INST_TYPE Inst[EPS_HIF_UNIVERSAL_DRV_MAX_INSTANCES];
} EPS_HIF_UNIVERSAL_DRV_TYPE, *EPS_HIF_UNIVERSAL_DRV_PTR_TYPE;

static EPS_HIF_UNIVERSAL_DRV_TYPE g_EpsHifUniversalDrv;
static EPS_HIF_UNIVERSAL_DRV_PTR_TYPE g_pEpsHifUniversalDrv = LSA_NULL;

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_universal_drv_undo_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
	retVal = eps_free_critical_section(g_pEpsHifUniversalDrv->hEnterExit);
    EPS_ASSERT( LSA_RET_OK == retVal );
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_universal_drv_init_critical_section(LSA_VOID)
{
    LSA_UINT16 retVal;
	retVal = eps_alloc_critical_section(&g_pEpsHifUniversalDrv->hEnterExit, LSA_FALSE);
    EPS_ASSERT( LSA_RET_OK == retVal );
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_universal_drv_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsHifUniversalDrv->hEnterExit);
}

/**
 * Lock implementation
 * @param LSA_VOID
 * @return
 */
static LSA_VOID eps_hif_universal_drv_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsHifUniversalDrv->hEnterExit);
}

/** 
 * Allocates a instance of the HIF Universal Driver.
 * 
 * @param LSA_VOID
 * @return EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE - handle to instance
 */
static EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE eps_hif_universal_drv_alloc_instance(LSA_VOID)
{
	LSA_UINT32 i;
	EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst = LSA_NULL;

	for(i=0; i<EPS_HIF_UNIVERSAL_DRV_MAX_INSTANCES; i++)
	{
		pInst = &g_pEpsHifUniversalDrv->Inst[i];

		if(pInst->bUsed == LSA_FALSE)
		{
			pInst->hOwnHandle       = i;
			pInst->bIsrEnabled      = LSA_FALSE;
			pInst->uCntDmaCpys      = 0;
			pInst->uCntIsrAcks      = 0;
			pInst->uCntIsrReenables = 0;
			pInst->uCntSendIsr      = 0;
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
static LSA_VOID eps_hif_universal_drv_free_instance(EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst)
{
	EPS_ASSERT(pInst != LSA_NULL);

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	pInst->bUsed = LSA_FALSE;
}

/**
 * Returns the instance.
 * 
 * @param [in] pHwInstIn                        - handle to instance
 * @return EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE  - the pointer the struct of the instance
 */
static EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE eps_hif_universal_drv_get_inst (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UINT32 index = (LSA_UINT32)pHwInstIn->hHandle;
	
	return &g_pEpsHifUniversalDrv->Inst[index];
}

/**
 * Installs the HIF Universal Driver by calling eps_hif_drv_if_register. Does a function mapping from the API defined in eps_hif_drv_if.
 * 
 * The Universal Driver is a polling mode driver.
 * 
 * Set the HIF Driver capabilites
 *    bUseIpcInterrupts     := FALSE
 *    bUsePnCoreInterrupts  := FALSE
 *    bUseRemoteMem         := TRUE/FALSE
 *    bUseDma               := FALSE
 *    bUseShortMode         := FALSE
 *    bUseSharedMemMode     := TRUE
 * 
 * @see eps_hif_drv_if_register - This function is called to register the HIF Driver implementation into the EPS.
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_hif_universal_drv_install (LSA_VOID)
{
	EPS_HIF_DRV_IF_TYPE sHifDrvIf;
	EPS_HIF_DRV_CFG_TYPE sCfg;

	//Init Internals
	eps_memset(&g_EpsHifUniversalDrv, 0, sizeof(g_EpsHifUniversalDrv));
	g_pEpsHifUniversalDrv = &g_EpsHifUniversalDrv;

	eps_hif_universal_drv_init_critical_section();

	eps_memset(&sHifDrvIf, 0, sizeof(sHifDrvIf));

	//Init Interface
	sHifDrvIf.open      = eps_hif_universal_drv_open;
	sHifDrvIf.close     = eps_hif_universal_drv_close;
	sHifDrvIf.uninstall = eps_hif_universal_drv_uninstall;

	sHifDrvIf.AckIsr        = eps_hif_universal_drv_ack_hif_ir;
	sHifDrvIf.DisableIsr    = eps_hif_universal_drv_disable_isr;
	sHifDrvIf.EnableIsr     = eps_hif_universal_drv_enable_isr;
	sHifDrvIf.ReenableIsr   = eps_hif_universal_drv_reenable_hif_ir;
	sHifDrvIf.SendIsr       = eps_hif_universal_drv_send_hif_ir;
	sHifDrvIf.DmaCpy        = eps_hif_universal_drv_dmacpy;
	sHifDrvIf.DmaPendFree   = eps_hif_universal_drv_dma_pend_free;
	sHifDrvIf.GetParams     = eps_hif_universal_drv_get_params;

	g_pEpsHifUniversalDrv->bInit = LSA_TRUE;


	eps_hif_drv_init_config(&sCfg);

	sCfg.bUseSharedMemMode = LSA_TRUE;

	g_pEpsHifUniversalDrv->uCntInterfaceInstalls++;
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);

	sCfg.bUseRemoteMem = LSA_TRUE;

	g_pEpsHifUniversalDrv->uCntInterfaceInstalls++;
	eps_hif_drv_if_register(&sHifDrvIf, &sCfg);
}

/**
 * Releases an instance of the HIF universal driver.
 * 
 * @see eps_hif_drv_if_undo_init - calls this function
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_universal_drv_uninstall (LSA_VOID)
{
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(g_pEpsHifUniversalDrv->uCntInterfaceInstalls > 0);

	g_pEpsHifUniversalDrv->uCntInterfaceInstalls--;

	if(g_pEpsHifUniversalDrv->uCntInterfaceInstalls == 0)
	{
	    eps_hif_universal_drv_undo_init_critical_section();

	    g_pEpsHifUniversalDrv->bInit = LSA_FALSE;

	    g_pEpsHifUniversalDrv = LSA_NULL;
	}
}

/**
 * eps_hif_drv_if_open implementation of the EPS HIF Universal Drv.
 *
 * @see eps_hif_drv_if_open          - calls this function
 *
 * @param [in] pInPars               - generic in structure for all EPS HIF drivers
 * @param [out] ppHwInstOut          - handle to the instance
 * @param [in] hd_id                 - 0 = LD, 1...4 = HD
 * @param [in] bUpper                - 1 = Upper, 0 = Lower
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_open (EPS_HIF_DRV_OPENPAR_CONST_PTR_TYPE pInPars, EPS_HIF_DRV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id, LSA_BOOL bUpper)
{
	EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(pInPars != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_universal_drv_open - Using EPS HIF Universal Mode Drv. hd_id=%d, isUpper=%d", hd_id, bUpper);

	eps_hif_universal_drv_enter();

	pInst = eps_hif_universal_drv_alloc_instance();
	EPS_ASSERT(pInst != LSA_NULL);

	pInst->InPar       = *pInPars;
	pInst->sHw.hd_id   = hd_id;
	pInst->sHw.bUpper  = bUpper;
	pInst->sHw.hHandle = pInst->hOwnHandle;

	*ppHwInstOut = &pInst->sHw;

	eps_hif_universal_drv_exit();

	return EPS_HIF_DRV_RET_OK;
}

/**
 * Closes the HIF driver.
 * 
 * @see eps_hif_drv_if_close        - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_close (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
    
    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);

	eps_hif_universal_drv_enter();

	EPS_ASSERT(pInst->bUsed == LSA_TRUE);
	EPS_ASSERT(pInst->bIsrEnabled == LSA_FALSE);

	eps_hif_universal_drv_free_instance(pInst);

	eps_hif_universal_drv_exit();
	
	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_universal_drv_close - done");

	return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only sets bIsrEnabled to true.
 * The Universal Driver is a polling mode driver!!!
 *  
 * @see eps_hif_drv_if_enable_isr - calls this function
 * 
 * @param [in] pHwInstIn               - handle to instance
 * @param [in] hHif                    - handle for HIF
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_enable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 hHif)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	LSA_UNUSED_ARG(hHif);

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
    
    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

	pInst->bIsrEnabled = LSA_TRUE;

	/* This is the non isr variant => no HW support */

	return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only sets bIsrEnabled to false.
 * The Universal Driver is a polling mode driver!!!
 *  
 * @see eps_hif_drv_if_disable_isr   - calls this function
 * 
 * @param [in] pHwInstIn             - handle to instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_disable_isr (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
    
    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

	pInst->bIsrEnabled = LSA_FALSE;

	/* This is the non isr variant => no HW support */

	return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only does statistics
 *  
 * @see eps_hif_drv_if_send_isr       - calls this function
 * 
 * @param [in] pHwInstIn              - handle to instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_send_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;
	
    EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
    
    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

	pInst->uCntSendIsr++;

	/* This is the non isr variant => no HW support */

	return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only does statistics.
 * @see eps_hif_drv_if_ack_isr      - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_ack_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);

    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

	pInst->uCntIsrAcks++;

	/* This is the non isr variant => no HW support */

	return EPS_HIF_DRV_RET_OK;
}

/**
 * The API requires the implementation of this function. This function only does statistics.
 * 
 * @see eps_hif_drv_if_reenable_isr - calls this function
 * @param pHwInstIn                 - handle to instance
 * @return #EPS_HIF_DRV_RET_OK
 */
static LSA_UINT16 eps_hif_universal_drv_reenable_hif_ir (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
    
    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

	pInst->uCntIsrReenables++;

	/* This is the non isr variant => no HW support */

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
static LSA_VOID eps_hif_universal_drv_dmacpy (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pDst, LSA_VOID* pSrc, LSA_UINT32 uLength)
{
    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;
    LSA_UINT8* pSrcData;
    LSA_UINT8* pDstData;
    LSA_UINT32* pCopyFinsihedFlag;

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);	

    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT( LSA_NULL != pInst );

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
 * Since no dma is used for copy in EPS HIF Universal Drv this function will always return LSA_FALSE.
 * 
 * @see eps_hif_drv_if_dma_pend_free - calls this function
 * @param [in] pHwInstIn             - handle to the instance
 * @param [in] pBuffer               - pointer to the buffer to be freed
 * @param [out] ppReadyForFree       - pointer to where dma writes HIF_DMA_FREE_BUF_FLAG, 
 *                                     when buffer is not longer in use by dma.
 * @return #LSA_FALSE                - no dma is used, buffer can be freed immediate, 
 *                                     do not pend a free return LSA_FALSE.
 */
static LSA_BOOL eps_hif_universal_drv_dma_pend_free (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(pBuffer);

    EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
    EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
    EPS_ASSERT(pHwInstIn != LSA_NULL);

    // No dma is used
    *ppReadyForFree = LSA_NULL;
    return LSA_FALSE;
}

/**
 * GetParams implementation of the EPS HIF Universal Drv.
 * 
 * The EPS HIF Universal driver is a polling mode driver that works on a shared memory.
 * 
 * @see eps_hif_drv_if_get_params - calls this function
 * 
 * @param [in] pHwInstIn - handle to instance
 * @param [out] pPar      - casted to #HIF_GET_PAR_PTR_TYPE
 * @return LSA_VOID
 */
static LSA_VOID eps_hif_universal_drv_get_params (EPS_HIF_DRV_HW_CONST_PTR_TYPE pHwInstIn, LSA_VOID* pPar)
{
	HIF_GET_PAR_PTR_TYPE pHifParams = (HIF_GET_PAR_PTR_TYPE) pPar;

    EPS_HIF_UNIVERSAL_DRV_INST_PTR_TYPE pInst;

	eps_hif_universal_drv_enter();

	EPS_ASSERT(g_pEpsHifUniversalDrv != LSA_NULL);
	EPS_ASSERT(g_pEpsHifUniversalDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHifParams != LSA_NULL);

    pInst = eps_hif_universal_drv_get_inst(pHwInstIn);
    EPS_ASSERT(pInst);
    
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
	
	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_hif_universal_drv_get_params - Using EPS HIF Universal Drv. hd_id=%d, isUpper=%d", pInst->sHw.hd_id, pInst->sHw.bUpper); 
	    
	eps_hif_universal_drv_exit();
}

#endif // ( EPS_CFG_USE_HIF == 1 )

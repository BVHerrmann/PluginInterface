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
/*  F i l e               &F: eps_xgdma.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation interface for xgdma                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20070
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include <eps_sys.h>					/* Types / Prototypes / Fucns    */

#if (EPS_PLF == EPS_PLF_PNIP_A53)

#include <eps_trc.h>					/* Tracing						 */
#include "eps_caching.h"
#include "eps_register.h"               /* Register access, EPS_CAST_TO_VOID_PTR */

/*----------------------------------------------------------------------------*/
/* Defines                                                                    */
/*----------------------------------------------------------------------------*/

/********* Module test ************/

//#define XGDMA_TEST
//#define XGDMA_USE_LOCKS   

/*********************/

#define EPS_XGDMA_COUNT_JOBS			            1	
#define EPS_XGDMA_COUNT_TRANSFER_RECORDS			100			// per job - max 2048
#define EPS_XGDMA_COUNT_MAX_FREE_PENDING            (101)       // HIF_BUFFER_ADMIN_LIST_ENTRIES + 1


#define EPS_HERA_XGDMA_LIST_RAM_OFFSET				0x2200

/* XGDMA registers		*/
#define EPS_HERA_XGDMA_REG__JOB_CTRL(job_nr)		(/*0x0000 +*/ (job_nr * 4)) // Prevent lint warning 835, but keep it as comment to make clear that it starts with offset 0.
#define EPS_HERA_XGDMA_REG__JOB_CTRL_SET(job_nr)	(0x0100 + (job_nr * 4))
#define EPS_HERA_XGDMA_REG__JOB_CTRL_CLR(job_nr)	(0x0200 + (job_nr * 4))
#define EPS_HERA_XGDMA_REG__JOB_EXT(job_nr)         (0x0600 + (job_nr * 4))
//#define EPS_HERA_XGDMA_REG__AXI_CTRL				0x0300
#define EPS_HERA_XGDMA_REG__JOB_TRIG				0x0400
#define EPS_HERA_XGDMA_REG__JOB_INTERRUPTED			0x0404
#define EPS_HERA_XGDMA_REG__JOB_STATUS				0x0408
#define EPS_HERA_XGDMA_REG__JOB_FIN					0x040C
//#define EPS_HERA_XGDMA_REG__TCOUNT					0x0500
//#define EPS_HERA_XGDMA_REG__TCTRL					0x0504

//#define EPS_HERA_XGDMA_REG__IP_VER					0x0800
//#define EPS_HERA_XGDMA_REG__IP_DEV					0x0804
#define EPS_HERA_XGDMA_REG__ACCESS_ERR				0x0808
#define EPS_HERA_XGDMA_REG__JOB_DONE_INT			0x080C
#define EPS_HERA_XGDMA_REG__JOB_DONE_MASK			0x0810
#define EPS_HERA_XGDMA_REG__JOB_DONE_ACK			0x0814
#define EPS_HERA_XGDMA_REG__JOB_ERR_INT				0x0818
#define EPS_HERA_XGDMA_REG__JOB_ERR_NUM				0x081C

#if (EPS_PLF == EPS_PLF_PNIP_A53)
#define EPS_XGDMA_MEM_BARRIER()                     asm("isb")
#define EPS_XGDMA_WB_CACHE(srcCpy, sizeCpy)         eps_cache_wb(srcCpy, sizeCpy)
#else
#define EPS_XGDMA_MEM_BARRIER()
#define EPS_XGDMA_WB_CACHE(srcCpy, sizeCpy)
#endif

/*----------------------------------------------------------------------------*/
/* Data Types                                                                 */
/*----------------------------------------------------------------------------*/
typedef struct {
	volatile LSA_UINT32* 		pXGdmaJobTriggerd;
	volatile LSA_UINT32* 		pXGdmaJobInterrupted;
	volatile LSA_UINT32* 		pXGdmaJobStatus;
	volatile LSA_UINT32* 		pXGdmaJobFinished;
	volatile LSA_UINT32* 		pXGdmaAccessError;
	volatile LSA_UINT32* 		pXGdmaJobDoneInt;
	volatile LSA_UINT32* 		pXGdmaJobDoneMask;
	volatile LSA_UINT32* 		pXGdmaJobDoneAck;
	volatile LSA_UINT32* 		pXGdmaJobErrInt;
	volatile LSA_UINT32* 		pXGdmaJobErrNum;
} EPS_HERA_XGDMA_JOB_CTRL_TYPE;

typedef struct {
	LSA_BOOL					isUsed;

	volatile LSA_UINT32*		transferInfo;
	volatile LSA_UINT32* 		srcAddress;
	volatile LSA_UINT32*		dstAddress;
	volatile LSA_UINT32*		ctrlInfo;
} EPS_HERA_XGDMA_TRANSFER_CTRL_TYPE, *EPS_HERA_XGDMA_TRANSFER_CTRL_PTR_TYPE;

typedef struct {
	LSA_BOOL						isUsed;
	LSA_UINT32						jobNr;

    volatile LSA_UINT32*            pXGdmaJobCtrl;
	volatile LSA_UINT32*			pXGdmaJobCtrlSet;
	volatile LSA_UINT32*			pXGdmaJobCtrlClr;
	volatile LSA_UINT32*			pXGdmaJobExt;

    EPS_HERA_XGDMA_TRANSFER_CTRL_TYPE	transferRecord[EPS_XGDMA_COUNT_TRANSFER_RECORDS];
    LSA_UINT32                      nextTransferRecord;
} EPS_XGDMA_JOB_MGM_TYPE, *EPS_XGDMA_JOB_MGM_PTR_TYPE;

typedef struct {
    volatile LSA_UINT32         ReadyForFree;
    LSA_UINT32*                 pBuffer;
    LSA_UINT32                  valueFlag;
} EPS_XGDMA_FREE_PEND_TYPE;

typedef struct {
    LSA_UINT32                  next_free_entry;
    LSA_UINT32                  first_used_entry;
    EPS_XGDMA_FREE_PEND_TYPE    list[EPS_XGDMA_COUNT_MAX_FREE_PENDING];
} EPS_XGDMA_FREE_PEND_LIST_TYPE;



typedef struct {
	LSA_BOOL						isInitialized;

	LSA_UINT8*						pEpsXGDMAReg;
	LSA_UINT8*						pEpsXGDMARam;

	EPS_XGDMA_JOB_MGM_TYPE			job[EPS_XGDMA_COUNT_JOBS];
	EPS_HERA_XGDMA_JOB_CTRL_TYPE	job_ctrl;
	EPS_XGDMA_FREE_PEND_LIST_TYPE   pending_frees;
#ifdef XGDMA_USE_LOCKS
	LSA_UINT16                      lock;               // Lock
#endif
} EPS_XGDMA_MGM_TYPE;

/*----------------------------------------------------------------------------*/
/* Data                                                                       */
/*----------------------------------------------------------------------------*/

static EPS_XGDMA_MGM_TYPE g_EpsXGDMAMgmStore; 

static EPS_XGDMA_JOB_MGM_PTR_TYPE pHifJob;

/*----------------------------------------------------------------------------*/
/* Implementation                                                             */
/*----------------------------------------------------------------------------*/
#ifdef XGDMA_USE_LOCKS
/**
* Lock implementation
*/
static LSA_VOID eps_xgdma_lock(LSA_VOID)
{
    eps_enter_critical_section(g_EpsXGDMAMgmStore.lock);
}

/**
* Lock Implementation
*/
static LSA_VOID eps_xgdma_unlock(LSA_VOID)
{
    eps_exit_critical_section(g_EpsXGDMAMgmStore.lock);
}

#endif

/**
* @brief  Pends a free for allocated buffer which is in use by xgdma.
*
* Link a free pending to the hif xgdma copy job.
*
* @param [in] pBuffer           pointer to the buffer to be freed
* @param [in] valueFlag         valueFlag XGDMA should write, when buffer can be freed.
* @param [out] ppReadyForFree   pointer to where xgdma will write valueFlag, 
*                               when buffer is not longer in use by dma.
*/
static LSA_VOID eps_xgdma_hif_add_pending_free(LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree, LSA_UINT32 valueFlag)
{
    // Check if the actual entry is free
    EPS_ASSERT(g_EpsXGDMAMgmStore.pending_frees.list[g_EpsXGDMAMgmStore.pending_frees.next_free_entry].ReadyForFree == 0x0);
    
    g_EpsXGDMAMgmStore.pending_frees.list[g_EpsXGDMAMgmStore.pending_frees.next_free_entry].pBuffer = pBuffer;
    g_EpsXGDMAMgmStore.pending_frees.list[g_EpsXGDMAMgmStore.pending_frees.next_free_entry].valueFlag = valueFlag;
    g_EpsXGDMAMgmStore.pending_frees.list[g_EpsXGDMAMgmStore.pending_frees.next_free_entry].ReadyForFree = ~valueFlag;

    *ppReadyForFree = (LSA_VOID_PTR_TYPE) &g_EpsXGDMAMgmStore.pending_frees.list[g_EpsXGDMAMgmStore.pending_frees.next_free_entry].ReadyForFree;
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_xgdma_hif_add_pending_free() - Pend Free for pBuffer: 0x%08x", pBuffer);

    g_EpsXGDMAMgmStore.pending_frees.next_free_entry++;
    g_EpsXGDMAMgmStore.pending_frees.next_free_entry %= EPS_XGDMA_COUNT_MAX_FREE_PENDING;
}

/**
* @brief  Updates pending_free list if no hif xgdma job is running.
*/
static LSA_VOID eps_xgdma_hif_update_pending_frees(LSA_UINT32 JobsTriggered)
{
    LSA_UINT32 entry;
    
    if((JobsTriggered & (1U << pHifJob->jobNr)) == 0)
    {
        // next_free_entry overrun?
        if(g_EpsXGDMAMgmStore.pending_frees.first_used_entry > g_EpsXGDMAMgmStore.pending_frees.next_free_entry)
        {
            // write free flag for last indexes until overrun
            for ( entry = g_EpsXGDMAMgmStore.pending_frees.first_used_entry; entry < EPS_XGDMA_COUNT_MAX_FREE_PENDING; entry++ )
            {
                g_EpsXGDMAMgmStore.pending_frees.list[entry].ReadyForFree = g_EpsXGDMAMgmStore.pending_frees.list[entry].valueFlag;
                g_EpsXGDMAMgmStore.pending_frees.list[entry].valueFlag = 0x0;
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_xgdma_hif_update_pending_frees() - Set ReadyForFree for pBuffer: 0x%08x", g_EpsXGDMAMgmStore.pending_frees.list[entry].pBuffer);
            }
            g_EpsXGDMAMgmStore.pending_frees.first_used_entry = 0x0; // last indexes are free now start from 0
        }
        // write free flag for all used entries
        for ( entry = g_EpsXGDMAMgmStore.pending_frees.first_used_entry; entry < g_EpsXGDMAMgmStore.pending_frees.next_free_entry; entry++ )
        {
            g_EpsXGDMAMgmStore.pending_frees.list[entry].ReadyForFree = g_EpsXGDMAMgmStore.pending_frees.list[entry].valueFlag;
            g_EpsXGDMAMgmStore.pending_frees.list[entry].valueFlag = 0x0;
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_xgdma_hif_update_pending_frees() - Set ReadyForFree for pBuffer: 0x%08x", g_EpsXGDMAMgmStore.pending_frees.list[entry].pBuffer);
        }
        g_EpsXGDMAMgmStore.pending_frees.first_used_entry = g_EpsXGDMAMgmStore.pending_frees.next_free_entry;
    }
}

/**
* @brief Initializes the xgdma interface
*
* Initialize the xgdma interface inclusive hardware module.
*
* @param [in] pXGdmaRegBase					XGDMA register base address
*
*/
LSA_RESULT eps_xgdma_init(LSA_UINT32 xGdmaRegBase)
{   
    LSA_RESPONSE_TYPE retVal = LSA_RET_OK;
	LSA_UINT32 i, j;

	EPS_ASSERT(g_EpsXGDMAMgmStore.isInitialized == LSA_FALSE);
	EPS_ASSERT(xGdmaRegBase != 0);

#ifdef XGDMA_USE_LOCKS
    // Init Locks
    retVal = eps_alloc_critical_section(&g_EpsXGDMAMgmStore.lock, LSA_TRUE);
    EPS_ASSERT(retVal == LSA_RET_OK);
#endif
    
	g_EpsXGDMAMgmStore.pEpsXGDMAReg     = (LSA_UINT8*)xGdmaRegBase;
	g_EpsXGDMAMgmStore.pEpsXGDMARam     = (LSA_UINT8*)(xGdmaRegBase + EPS_HERA_XGDMA_LIST_RAM_OFFSET);

	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobTriggerd =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_TRIG);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobInterrupted =	(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_INTERRUPTED);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobStatus =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_STATUS);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobFinished =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_FIN);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaAccessError =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__ACCESS_ERR);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobDoneInt =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_DONE_INT);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobDoneMask =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_DONE_MASK);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobDoneAck =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_DONE_ACK);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobErrInt =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_ERR_INT);
	g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobErrNum =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_ERR_NUM);
	
	// initialize job registers
	for (i = 0; i < EPS_XGDMA_COUNT_JOBS; i++)
	{
		g_EpsXGDMAMgmStore.job[i].isUsed = LSA_FALSE;
		g_EpsXGDMAMgmStore.job[i].jobNr = i;

        g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrl =    	(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_CTRL(i));
		g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrlSet =	(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_CTRL_SET(i));
		g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrlClr =	(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_CTRL_CLR(i));
		g_EpsXGDMAMgmStore.job[i].pXGdmaJobExt =		(volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMAReg + EPS_HERA_XGDMA_REG__JOB_EXT(i));

		// set first Transfer record address
		*(g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrl) = (EPS_XGDMA_COUNT_TRANSFER_RECORDS * i) << 21;

		// initialize transfer record registers
        for (j = 0; j < EPS_XGDMA_COUNT_TRANSFER_RECORDS; j++)
        {
            g_EpsXGDMAMgmStore.job[i].transferRecord[j].isUsed = LSA_FALSE;
            g_EpsXGDMAMgmStore.job[i].transferRecord[j].transferInfo =   (volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMARam + ( /*0 +*/ (16 * j)) + (16 * EPS_XGDMA_COUNT_TRANSFER_RECORDS * i)); // prevent lint warning by commenting out '0 +'
            g_EpsXGDMAMgmStore.job[i].transferRecord[j].srcAddress =     (volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMARam + ( 4 +     (16 * j)) + (16 * EPS_XGDMA_COUNT_TRANSFER_RECORDS * i));
            g_EpsXGDMAMgmStore.job[i].transferRecord[j].dstAddress =     (volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMARam + ( 8 +     (16 * j)) + (16 * EPS_XGDMA_COUNT_TRANSFER_RECORDS * i));
            g_EpsXGDMAMgmStore.job[i].transferRecord[j].ctrlInfo =       (volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(g_EpsXGDMAMgmStore.pEpsXGDMARam + (12 +     (16 * j)) + (16 * EPS_XGDMA_COUNT_TRANSFER_RECORDS * i));
        }

        g_EpsXGDMAMgmStore.job[i].nextTransferRecord = 0;

        // set JobEnable
        *(g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrlSet) = 0x00000002;
	}
	
	g_EpsXGDMAMgmStore.pending_frees.next_free_entry = 0x0;
	g_EpsXGDMAMgmStore.pending_frees.first_used_entry = 0x0;
	// initialize pending_free list
    for (i = 0; i < EPS_XGDMA_COUNT_MAX_FREE_PENDING; i++)
    {
        g_EpsXGDMAMgmStore.pending_frees.list[i].pBuffer = LSA_NULL;
        g_EpsXGDMAMgmStore.pending_frees.list[i].ReadyForFree = 0x0;
        g_EpsXGDMAMgmStore.pending_frees.list[i].valueFlag = 0x0;
    }

	// reset access error register
	*(g_EpsXGDMAMgmStore.job_ctrl.pXGdmaAccessError) = 0; 

    pHifJob = &g_EpsXGDMAMgmStore.job[0];

	g_EpsXGDMAMgmStore.isInitialized = LSA_TRUE;

	return retVal;
}

/**
* @brief Undoes initialization of the xgdma interface
*
* Undoes initializazion of the xgdma interface.
*
*
*/
LSA_VOID eps_xgdma_undo_init( LSA_VOID )
{
    LSA_UINT32 i;
    volatile LSA_UINT32* pJobTriggered = g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobTriggerd;

	EPS_ASSERT(g_EpsXGDMAMgmStore.isInitialized == LSA_TRUE);

    // wait until all hif xgdma transfers are finished.
    while((*pJobTriggered & (1U << pHifJob->jobNr)) != 0)
    {
        ; // idle wait
    }
    // issue a memory barrier command -> make sure job is finished before further execution 
    EPS_XGDMA_MEM_BARRIER();
    
    // undo initialize job registers
    for (i = 0; i < EPS_XGDMA_COUNT_JOBS; i++)
    {
        // reset JobEnable
        *(g_EpsXGDMAMgmStore.job[i].pXGdmaJobCtrlClr) = 0x00000002;
    }
    
    eps_xgdma_hif_update_pending_frees(*pJobTriggered);

	g_EpsXGDMAMgmStore.isInitialized = LSA_FALSE;
#ifdef XGDMA_USE_LOCKS
    eps_free_critical_section(g_EpsXGDMAMgmStore.lock);
#endif
}

/**
* @brief Copy data and finished flag with xgdma
*
* Link two transfer records to the hif xgdma copy job.
*
* @param [in] destCpy             Destination address to copy to
* @param [in] srcCpy              Source address to copy from
* @param [in] sizeCpy             Size in byte to copy
* @param [in] destFlag            Destination address of finished flag
* @param [in] valueFlag           Flag to copy as finished flag
*/
LSA_VOID eps_xgdma_hif_memcpy(LSA_VOID* destCpy, LSA_VOID* srcCpy, LSA_UINT32 sizeCpy, LSA_VOID* destFlag, LSA_UINT32 valueFlag)
{
    LSA_UINT32 cpyTr, flagTr, lastCpyTr;
    LSA_UINT32 srcAhb, dstAhb, flagAhb;
    LSA_UINT32 waitCount;
    EPS_HERA_XGDMA_TRANSFER_CTRL_PTR_TYPE pTransferRecordCopy;
    EPS_HERA_XGDMA_TRANSFER_CTRL_PTR_TYPE pTransferRecordFlag;

    volatile LSA_UINT32* pJobTriggered = g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobTriggerd;

#ifdef XGDMA_TEST
    void* srcCpyOrig = (void *) 0xE0800000;
    /// Save buffer before write back
    eps_memcpy((LSA_UINT8*)srcCpyOrig, (LSA_UINT8*)srcCpy, sizeCpy);
//    /// invalidate src
//    eps_memset(srcCpy, 0xCC, sizeCpy);
    /// invalidate dst
    eps_memset(destCpy, 0xFF, sizeCpy);
#endif
    
    EPS_ASSERT(g_EpsXGDMAMgmStore.isInitialized == LSA_TRUE);
    
    // nothing to do here
    if (sizeCpy == 0)
    {
        return;
    }

#ifdef XGDMA_USE_LOCKS
    eps_xgdma_lock();
#endif
    
    EPS_XGDMA_WB_CACHE(srcCpy, sizeCpy);

#ifdef XGDMA_TEST
    { 
        LSA_INT memCmpRetVal = eps_cache_cmp_uncached(srcCpy, sizeCpy); 
        if(memCmpRetVal != 0) \
        { 
            EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - eps_cache_cmp_uncached() returned=%d, Src ptr: 0x%x, size: 0x%x", memCmpRetVal, srcCpy, sizeCpy);
        }
    }
#endif
    
    srcAhb = (LSA_UINT32)srcCpy;
    dstAhb = (LSA_UINT32)destCpy;
    flagAhb = (LSA_UINT32)destFlag;
    
    cpyTr = pHifJob->nextTransferRecord;
    flagTr = (cpyTr + 1) % EPS_XGDMA_COUNT_TRANSFER_RECORDS;

    // load transfer records for the job
    pTransferRecordCopy = &pHifJob->transferRecord[cpyTr];
    pTransferRecordFlag = &pHifJob->transferRecord[flagTr];
    {
        *(pTransferRecordCopy->srcAddress) = srcAhb;
        *(pTransferRecordCopy->dstAddress) = dstAhb;
        *(pTransferRecordCopy->ctrlInfo) =   sizeCpy;
        /* TODO optimize XGDMA Transfer 
        *(pTransferRecordCopy->ctrlInfo) =   sizeCpy     |
                                            (0x1u << 18) | // PostedWR
                                            (0x1u << 19) | // Streaming_Enable
                                            (0x3u << 22) | // DST_Burst_Mode
                                            (0x3u << 25) | // SRC_Burst_Mode
                                            (0x3u << 28) | // DST_Data_Size
                                            (0x3u << 30);  // SRC_Data_Size
         */

#ifdef XGDMA_TEST
        // first record is last record, do not copy magic value with xgdma, set magic value after test of xgdma instead
        *(pTransferRecordCopy->transferInfo) = (0x1u << 31); // set is LastRecord
#else
        // concatenate next transfer record with finished flag
        *(pTransferRecordCopy->transferInfo) = flagTr;

        *(pTransferRecordFlag->srcAddress) = valueFlag;
        *(pTransferRecordFlag->dstAddress) = flagAhb;
        *(pTransferRecordFlag->ctrlInfo) = 4; // 4 byte flag
        *(pTransferRecordFlag->transferInfo) =  (0x02U << 12) |                      // direct data transfer mode
                                                (0x1U << 31);                       // set is LastRecord
#endif
        
        // issue a memory barrier command -> transferrecords are written before further execution
        EPS_XGDMA_MEM_BARRIER();
    }
    
    {
        // wait until xgdma job is free for use again
        waitCount = 0;
        while((*pJobTriggered & (1U << pHifJob->jobNr)) != 0)
        {
            waitCount++;
            if(waitCount == 1)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "eps_xgdma_hif_memcpy() - wait until job is finished");
            }
        }
        // issue a memory barrier command -> make sure job is finished before further execution 
        EPS_XGDMA_MEM_BARRIER();
        
        // hif xgdma job finished, update free flags
        eps_xgdma_hif_update_pending_frees(*pJobTriggered);
        if(waitCount > 0)
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "eps_xgdma_hif_memcpy() waited 0x%08x for dma job", waitCount);
            // last transfer record is finished flag, the transfer before last is last copy transfer -> cpyTr - 2
            lastCpyTr = (EPS_XGDMA_COUNT_TRANSFER_RECORDS + cpyTr - 2) % EPS_XGDMA_COUNT_TRANSFER_RECORDS;
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "eps_xgdma_hif_memcpy() dma job transferd bytecount is 0x%08x", pHifJob->transferRecord[lastCpyTr].ctrlInfo);
        }
    }
    
    // retrigger job with new transfer record
    *(pHifJob->pXGdmaJobCtrl) = 0x00000003 | (cpyTr << 21);
    
    // issue a memory barrier command -> make sure job was triggered before further execution
    EPS_XGDMA_MEM_BARRIER();

	// increment for both records cpy and flag.
    pHifJob->nextTransferRecord+=2;
    pHifJob->nextTransferRecord %= EPS_XGDMA_COUNT_TRANSFER_RECORDS;

#ifdef XGDMA_TEST
    // wait until job is finished
    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_xgdma_hif_memcpy() - wait until job is finished - Dst ptr: 0x%x, Src ptr: 0x%x, size: 0x%x", destCpy, srcCpy, sizeCpy);
    while (((*g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobFinished) & (1U << pHifJob->jobNr)) == 0x00000000);
    
    // issue a memory barrier command ->  make sure job is finished before further execution 
    EPS_XGDMA_MEM_BARRIER();
    
    {
        /// compare srcCpy cached with orig copy
        if (0 != eps_memcmp((LSA_UINT8*)srcCpy, (LSA_UINT8*)srcCpyOrig, sizeCpy))
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - cached data not valid");
        }
        /// compare srcCpy cached with uncached
        if (0 != eps_cache_cmp_uncached(srcCpy, sizeCpy))
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - cached!=uncached data");
        }
        /// compare tranfered memory from xgdma with orig copy
        if (0 != eps_memcmp((LSA_UINT8*)destCpy, (LSA_UINT8*)srcCpyOrig, sizeCpy))
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - xgdma data not valid");
            EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - Dst ptr: 0x%x, Src ptr: 0x%x, size: 0x%x", destCpy, srcCpy, sizeCpy);
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - flag Dst: 0x%08x, value: 0x%08x", *(LSA_UINT32*)destFlag, valueFlag);
            
            {
                LSA_UINT8* pCpyEnd  = ((LSA_UINT8*)srcCpy + sizeCpy);
                
                while((LSA_UINT32*)srcCpy < ((LSA_UINT32*)pCpyEnd))
                {
                    if(*(LSA_UINT32*)destCpy != *(LSA_UINT32*)srcCpy)
                    {
                        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - Dst ptr: 0x%x, Src ptr: 0x%x", destCpy, srcCpy);
                        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_xgdma_hif_memcpy() - Dst: 0x%08x, Src: 0x%08x", *(LSA_UINT32*)destCpy, *(LSA_UINT32*)srcCpy);
                    }
                    destCpy = ((LSA_UINT32*)destCpy + 1);
                    srcCpy = ((LSA_UINT32*)srcCpy + 1);
                }
            }
    //        EPS_FATAL("Xgdma Data not transferred correctly");
        }
        else
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_xgdma_hif_memcpy() - data transferred correctly.");
        }
    }

    // set magicvalue after compare
    *(LSA_UINT32*)destFlag = valueFlag;
#endif
    
#ifdef XGDMA_USE_LOCKS
    eps_xgdma_unlock();
#endif
}

/**
* @brief  Pends a free for allocated buffer which is in use by xgdma. Otherwise returns LSA_FALSE.
*
* @param [in] pBuffer           pointer to the buffer to be freed
* @param [in] valueFlag         valueFlag XGDMA should write, when buffer can be freed.
* @param [out] ppReadyForFree   pointer to where xgdma will write valueFlag, 
*                               when buffer is not longer in use by dma.
* @return #LSA_FALSE            XGDMA did not pend a free for buffer, buffer is not in use by dma.        
* @return #LSA_TRUE             XGDMA pended a free for buffer.
*/
LSA_BOOL eps_xgdma_hif_pend_free(LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree, LSA_UINT32 valueFlag)
{  
    LSA_BOOL free_pended = LSA_FALSE;
    volatile LSA_UINT32* pJobTriggered = g_EpsXGDMAMgmStore.job_ctrl.pXGdmaJobTriggerd;
    
    EPS_ASSERT(g_EpsXGDMAMgmStore.isInitialized == LSA_TRUE);

#ifdef XGDMA_USE_LOCKS
    eps_xgdma_lock();
#endif
    
    // is hif xgdma job still running?
    if((*pJobTriggered & (1U << pHifJob->jobNr)) != 0)
    {
        // hif xgdma job still running, set valueFlag, when job is finished.
        eps_xgdma_hif_add_pending_free(pBuffer, ppReadyForFree, valueFlag);
        free_pended = LSA_TRUE;
    }
    else
    {
        // hif xgdma job finished, update free flags.
        eps_xgdma_hif_update_pending_frees(*pJobTriggered);
        // buffer not used by xgdma return LSA_FALSE.
        free_pended = LSA_FALSE;
    }
    
#ifdef XGDMA_USE_LOCKS
    eps_xgdma_unlock();
#endif
    
    return free_pended;
}

#endif

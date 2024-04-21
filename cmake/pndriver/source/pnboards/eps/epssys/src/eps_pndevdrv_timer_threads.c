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
/*  F i l e               &F: eps_pndevdrv_timer_threads.c              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Interface Adaption                                       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20064
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Fucns */
#include <eps_rtos.h> 

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*-------------------------------------------------------------------------*/

#include <precomp.h>
#include <PnDev_Driver_Inc.h>       /* PnDevDrv Interface                       */

#include <eps_tasks.h>              /* EPS TASK API                             */
#include <eps_cp_hw.h>              /* EPS CP PSI adaption                      */
#include <eps_trc.h>                /* Tracing                                  */
#include <eps_pn_drv_if.h>          /* PN Device Driver Interface               */
#include <eps_pndevdrv.h>           /* PNDEV-Driver Interface implementation    */
#include <eps_shm_file.h>           /* EPS Shared Memory File Interface         */

#include "eps_pndevdrv_timer_threads.h"
#include "eps_pndevdrv_helper.h"
#include "eps_register.h"

// for HighPerformance adaptive timer (1msec)
#define EPS_PNDEVDRV_CFG_HPTIMER_MIN_CBF_START_DELAY_NS     ((LSA_UINT64)500000)    // 0,5ms // minimum delay between 2 Cbf executions of high performance timer
#define EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_START_DELAY_NS     ((LSA_UINT64)2500000)   // 2,5ms // maximum delay between 2 Cbf executions of high performance timer
#define EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_DURATION_NS        ((LSA_UINT64)1000000)   // 1ms   // maximum duration Cbf should last when using high performance timer
// for adaptive timer (32msec)
#define EPS_PNDEVDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS       ((LSA_UINT64)500000)    // 0,5ms // minimum delay between 2 Cbf executions
#define EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS       ((LSA_UINT64)2500000)   // 2,5ms // maximum delay between 2 Cbf executions
#define EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_DURATION_NS          ((LSA_UINT64)1000000)   // 1ms   // maximum duration Cbf should last when using adaptive timer
//#define EPS_PNDEVDRV_CFG_TIMER_TICK_VALUE_MS                ((LSA_UINT32)1)         // 1 ms delay between two timer ticks

typedef struct eps_pndevdrv_threads_store_tag
{
	uPNDEV_DRIVER_DESC *pDrvHandle;
    LSA_BOOL bInit;
    struct
	{
		EPS_PNDEVDRV_POLL_TYPE PnGatheredIsr;
		EPS_PNDEVDRV_POLL_TYPE PnNrtIsr;
	} poll;
	EPS_APP_FILE_TAG_TYPE    file_tag;
	LSA_BOOL                 bFileAllocated;
} EPS_PNDEVDRV_TIMER_THREADS_STORE_TYPE, *EPS_PNDEVDRV_TIMER_THREADS_STORE_PTR_TYPE;

static EPS_PNDEVDRV_TIMER_THREADS_STORE_TYPE         g_EpsPnDevDrvTimerThreads;
static EPS_PNDEVDRV_TIMER_THREADS_STORE_PTR_TYPE     g_pEpsPnDevDrvTimerThreads = LSA_NULL;

static LSA_UINT16   eps_pndevdrv_register_board_for_polling(EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread);
static LSA_UINT16   eps_pndevdrv_unregister_board_for_polling(EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, LSA_BOOL *bKillThread);
static LSA_VOID     eps_pndevdrv_pn_poll_isr_thread(LSA_UINT32 uParam, void *arg);
static LSA_UINT16   eps_pndevdrv_create_poll_thread(LSA_CHAR const* cName, EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard);

/**
 * Initializes the global structure g_EpsPnDevDrvTimerThreads and assigns the pointer g_pEpsPnDevDrvTimerThreads.
 * Note: Do not access g_EpsPnDevDrvTimerThreads directly, only access g_pEpsPnDevDrvTimerThreads or use the functions to access members of the structure.
 *
 * @see eps_pndevdrv_install - Calls this function
*/
LSA_VOID eps_pndevdrv_timer_threads_init(uPNDEV_DRIVER_DESC *pDrvHandle)
{
    eps_memset(&g_EpsPnDevDrvTimerThreads, 0, sizeof(g_EpsPnDevDrvTimerThreads));

	g_pEpsPnDevDrvTimerThreads = &g_EpsPnDevDrvTimerThreads;
	g_pEpsPnDevDrvTimerThreads->pDrvHandle = pDrvHandle;
	eps_memset(&g_pEpsPnDevDrvTimerThreads->file_tag, 0, sizeof(g_pEpsPnDevDrvTimerThreads->file_tag));
	g_pEpsPnDevDrvTimerThreads->bFileAllocated = LSA_FALSE;
    g_pEpsPnDevDrvTimerThreads->bInit = LSA_TRUE;
    
}

/**
 * Deletes the pointer of g_pEpsPnDevDrvTimerThreads to g_EpsPnDevDrvTimerThreads
 *
 * @see eps_pndevdrv_uninstall - Calls this function
*/
LSA_VOID eps_pndevdrv_timer_threads_undo_init(LSA_VOID)
{
	g_pEpsPnDevDrvTimerThreads->bInit = LSA_FALSE;
	g_pEpsPnDevDrvTimerThreads->pDrvHandle = LSA_NULL;
	g_pEpsPnDevDrvTimerThreads = LSA_NULL;
}

#if 0 //not used yet
/**
 * polling isr, which will be called periodically
 * 
 * @param uParam    Parameterstructure (unused)
 * @param arg       ptr to corresponding structure, containing board and isr parameter
 * @return                  
 */
static LSA_VOID eps_pndevdrv_timer_poll_isr_thread(LSA_UINT32 uParam, LSA_VOID* pArgs)
{
    EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pArgs;

    LSA_UNUSED_ARG(uParam);

    pBoard->TimerCtrl.sTimerCbf.pCbf(pBoard->TimerCtrl.sTimerCbf.uParam, pBoard->TimerCtrl.sTimerCbf.pArgs);

    return;
}

/**
 * create periodically poll thread for nrt purpose
 * 
 * @param pBoard 		ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK, upon successful execution		
 */
static LSA_UINT16 eps_pndevdrv_create_timer_polling_thread(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	pBoard->TimerCtrl.hThread = eps_tasks_start_us_poll_thread("EPS_PNDEVTMRPOLL", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, eps_pndevdrv_timer_poll_isr_thread, 0, (LSA_VOID*)pBoard, eRUN_ON_1ST_CORE );
	EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);
	#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
	eps_tasks_group_thread_add ( pBoard->TimerCtrl.hThread, EPS_TGROUP_USER );
	#endif
	return EPS_PNDEV_RET_OK;
}
#endif

/**
 * create periodically poll thread for gathered method
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_create_pn_gathered_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrvTimerThreads->poll.PnGatheredIsr;

	retVal = eps_pndevdrv_register_board_for_polling(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread)
	{
		retVal = eps_pndevdrv_create_poll_thread("EPS_PNDEVGPISR", pPollIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
            LSA_UINT16 retValUnregister;
            retValUnregister = eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bCreateThread);
            EPS_ASSERT(EPS_PNDEV_RET_OK == retValUnregister);
		}
	}

	return retVal;
}

/**
 * register board for interrupt polling
 * 
 * @param [in] pPollIsr			ptr to PollIsr parameter
 * @param [in] pBoard 			ptr to corresponding board structure
 * @param [in] uIntSrc			enum with IntSrc information, (here EPS_PNDEV_ISR_PN_GATHERED or EPS_PNDEV_ISR_PN_NRT)
 * @param [in] bCreateThread	True if Thread should be created
 * @return 					    EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pndevdrv_register_board_for_polling(EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{
	LSA_UINT32 i;

	*bCreateThread = LSA_FALSE;

	if(pPollIsr->uCntUsers >= EPS_CFG_PNDEVDRV_MAX_BOARDS)
	{
		return EPS_PNDEV_RET_ERR; /* No More Resources */
	}

	for(i=0; i<EPS_CFG_PNDEVDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
		}
	}

	for(i=0; i<EPS_CFG_PNDEVDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == LSA_NULL)
		{
			pPollIsr->pBoard[i]  = (EPS_PNDEVDRV_BOARD_PTR_TYPE) pBoard;  /* Register Board for polling */
			pPollIsr->uIntSrc[i] = uIntSrc;
			break;
		}
	}

	pPollIsr->uCntUsers++;

	if(pPollIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}

/**
 * create periodically poll thread
 * 
 * @param [in] cName			internal thread name
 * @param [in] pPollIsr 		ptr to corresponding structure for isr parameter
 * @param [in] pBoard 		    ptr to corresponding board structure
 * @return EPS_PNDEV_RET_OK					
 */
static LSA_UINT16 eps_pndevdrv_create_poll_thread(LSA_CHAR const* cName, EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_create_poll_thread() - starting poll thread - Name: %s", cName);

    switch (pBoard->pDeviceInfo->eBoard)
    {
        // HighPerformance adaptive poll-thread (1msec)
        case ePNDEV_BOARD_I210:
        case ePNDEV_BOARD_I210SFP:
        case ePNDEV_BOARD_I82574:
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        case ePNDEV_BOARD_TI_AM5728:
        case ePNDEV_BOARD_IX1000:
        {
            pPollIsr->hThread = eps_tasks_start_us_poll_thread(cName, EPS_POSIX_THREAD_PRIORITY_HIGH_PERFORMANCE, eSchedFifo, 1000, eps_pndevdrv_pn_poll_isr_thread, 0, (LSA_VOID*)pPollIsr, eRUN_ON_1ST_CORE );   
            break;
        }

        // normal poll-thread (1msec)
        case ePNDEV_BOARD_CP1616:
        case ePNDEV_BOARD_EB200:
        case ePNDEV_BOARD_DB_EB200_PCIE:
        case ePNDEV_BOARD_DB_SOC1_PCI:
        case ePNDEV_BOARD_DB_SOC1_PCIE:
		case ePNDEV_BOARD_CP1625:
        case ePNDEV_BOARD_FPGA1_ERTEC200P:
        case ePNDEV_BOARD_EB200P:
        case ePNDEV_BOARD_DB_EB400_PCIE:
		case ePNDEV_BOARD_FPGA1_HERA:
        {
            pPollIsr->hThread = eps_tasks_start_poll_thread(cName, EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, 1, eps_pndevdrv_pn_poll_isr_thread, 0, (LSA_VOID*)pPollIsr, eRUN_ON_1ST_CORE );   
            break;
        }

        // unknown board
        case ePNDEV_BOARD_INVALID:
        default:
        {
            EPS_FATAL("eps_pndevdrv_create_poll_thread(): Unknown Board-Type!!!");
            break;
        }	  
    }

	EPS_ASSERT(pPollIsr->hThread != 0);

	pPollIsr->bRunning = LSA_TRUE;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS  
	eps_tasks_group_thread_add( pPollIsr->hThread, EPS_TGROUP_USER );
#endif

	return EPS_PNDEV_RET_OK;
}

/**
 * kill periodically poll thread
 * 
 * @param [in] cName			internal thread name
 * @param [in] pPollIsr 		ptr to corresponding structure for isr parameter
 * @param [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK				
 */
static LSA_UINT16 eps_pndevdrv_kill_poll_thread(LSA_CHAR* cName, EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UNUSED_ARG(pBoard);
	LSA_UNUSED_ARG(cName);
	EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_kill_poll_thread() - stopping poll thread - Name: %s", cName);
	pPollIsr->bRunning = LSA_FALSE;

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS 
	eps_tasks_group_thread_remove ( pPollIsr->hThread );
#endif

	eps_tasks_stop_poll_thread(pPollIsr->hThread);

	return EPS_PNDEV_RET_OK;
}

/**
 * kill periodically poll thread for gathered method
 * 
 * @param [in] pBoard 	                ptr to corresponding board structure
 * @return LSA_TRUE	EPS_PNDEV_RET_OK,   upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_kill_pn_gathered_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrvTimerThreads->poll.PnGatheredIsr;
	
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_kill_pn_gathered_polling_thread() - stopping gathered poll thread");
	retVal = eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

	if(bKillThread)
	{
        LSA_UINT16 retValKill;
        retValKill = eps_pndevdrv_kill_poll_thread("EPS_PNDEVGPISR", pPollIsr, pBoard);
        EPS_ASSERT(LSA_RET_OK == retValKill);
	}

	return retVal;
}

/**
 * create periodically poll thread for nrt purpose
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_create_pn_nrt_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrvTimerThreads->poll.PnNrtIsr;
	
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_create_pn_nrt_polling_thread() - creating nrt poll thread");
	retVal = eps_pndevdrv_register_board_for_polling(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_NRT, &bCreateThread);

	if(bCreateThread)
	{
		retVal = eps_pndevdrv_create_poll_thread("EPS_PNDEVNRTPISR", pPollIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
            LSA_UINT16 retValUnregister;
            retValUnregister = eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bCreateThread);
            EPS_ASSERT(LSA_RET_OK == retValUnregister);
		}
	}

	return retVal;
}

/**
 * kill periodically poll thread for nrt purpose
 * 
 * @param [in] pBoard 		        ptr to corresponding board structure
 * @return EPS_PNDEV_RET_OK         upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_kill_pn_nrt_polling_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = &g_pEpsPnDevDrvTimerThreads->poll.PnNrtIsr;
	
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_kill_pn_nrt_polling_thread() - stopping nrt poll thread");
	retVal = eps_pndevdrv_unregister_board_for_polling(pPollIsr, pBoard, &bKillThread);

	if(bKillThread)
	{
        LSA_UINT16 retValKill;
        retValKill = eps_pndevdrv_kill_poll_thread("EPS_PNDEVNRTPISR", pPollIsr, pBoard);
        EPS_ASSERT(LSA_RET_OK == retValKill);
	}

	return retVal;
}



/**
 * unregister board for interrupt polling
 * 
 * @param   [in] pPollIsr			    ptr to PollIsr parameter
 * @param   [in] pBoard 			    ptr to corresponding board structure
 * @param   [in] bKillThread		    True if Thread should be killed
 * @return 	EPS_PNDEV_RET_OK            upon successful execution	
 * @return  EPS_PNDEV_RET_ERR           Board not found
 */
static LSA_UINT16 eps_pndevdrv_unregister_board_for_polling(EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	LSA_UINT32 i;

	*bKillThread = LSA_FALSE;

	if(pPollIsr->uCntUsers == 0)
	{
		return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
	}

	for(i=0; i<EPS_CFG_PNDEVDRV_MAX_BOARDS; i++)
	{
		if(pPollIsr->pBoard[i] == pBoard)
		{
			pPollIsr->pBoard[i] = LSA_NULL;

			pPollIsr->uCntUsers--;
			if(pPollIsr->uCntUsers == 0)
			{
				*bKillThread = LSA_TRUE;
			}

			return EPS_PNDEV_RET_OK; /* Board polling already enabled */
		}
	}

	return EPS_PNDEV_RET_ERR; /* Board not found */
}

/**
 * polling isr, which will be called periodically
 * 
 * @param [in] uParam	Parameterstructure (unused)
 * @param [in] arg 		ptr to corresponding structure, containing board and isr parameter
 * @return 					
 */
static LSA_VOID eps_pndevdrv_pn_poll_isr_thread(LSA_UINT32 uParam, void *arg)
{
	EPS_PNDEVDRV_POLL_PTR_TYPE pPollIsr = (EPS_PNDEVDRV_POLL_PTR_TYPE)arg;
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT32 i;

	LSA_UNUSED_ARG(uParam);

	for(i=0; i<EPS_CFG_PNDEVDRV_MAX_BOARDS; i++)
	{
		pBoard = pPollIsr->pBoard[i];

		if(pBoard != LSA_NULL)
		{
			switch(pPollIsr->uIntSrc[i])
			{
				case EPS_PNDEV_ISR_PN_GATHERED:
					if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
					}
					else
					{
						EPS_FATAL("No callbackfunction for gathered interrupt registered in pndevdrv polling isr"); /* Unexpected */
					}
					break;
				case EPS_PNDEV_ISR_PN_NRT:
					if(pBoard->sIsrPnNrt.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnNrt.pCbf(pBoard->sIsrPnNrt.uParam, pBoard->sIsrPnNrt.pArgs);
					}
					else
					{
					  EPS_FATAL("No callbackfunction for nrt interrupt registered in pndevdrv polling isr"); /* Unexpected */
					}
					break;
				default: EPS_FATAL("Unknown interrupt source in pndevdrv polling isr");
					break;
                    //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
			}
		}
	}
}

/**
 * start timer intern, for eddi SiiExtTimerMode
 * 
 * @param   [in] pHwInstIn 		        pointer to structure with hardware configuration
 * @param   [in] pCbf				    callback function, as argument for ISR
 * @return 	EPS_PNDEV_RET_OK            upon successful execution
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Not supported for this board type
 */
static LSA_UINT16 eps_pndevdrv_timer_ctrl_start_intern(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_CONST_PTR_TYPE pCbf)
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16 result = EPS_PNDEV_RET_ERR;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

	if(pBoard->sHw.EpsBoardInfo.edd_type == LSA_COMP_ID_EDDI)
	{
		if	(	(pBoard->sHw.EpsBoardInfo.board_type == EPS_PNDEV_BOARD_SOC1_PCI)			
			|| 	(pBoard->sHw.EpsBoardInfo.board_type == EPS_PNDEV_BOARD_SOC1_PCIE)
			||	(pBoard->sHw.EpsBoardInfo.board_type == EPS_PNDEV_BOARD_CP1625))
		{	
			pBoard->TimerCtrl.hThread=eps_tasks_start_us_poll_thread("EPS_PNDEXTTMR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, pCbf->pCbf, pCbf->uParam, pCbf->pArgs, eRUN_ON_1ST_CORE );
			EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);

			result = EPS_PNDEV_RET_OK;
		}
		else
		{
			EPS_FATAL("Can't activate poll thread for non IRTEREV7DEV boards");
		}
	}
	else
	{
		result = EPS_PNDEV_RET_UNSUPPORTED;
	}

	return result;
}

/**
 * stop timer intern, for eddi SiiExtTimerMode
 * 
 * @param [in] pHwInstIn 		        pointer to structure with hardware configuration
 * @return EPS_PNDEV_RET_OK             upon successful execution	
 * @return EPS_PNDEV_RET_UNSUPPORTED    not supported for this board type
 */
LSA_UINT16 eps_pndevdrv_timer_ctrl_stop_intern(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	pBoard->TimerCtrl.bRunning = LSA_FALSE;

	if (pBoard->sHw.EpsBoardInfo.edd_type == LSA_COMP_ID_EDDI)
	{
		eps_tasks_stop_poll_thread(pBoard->TimerCtrl.hThread);
	}
	else
	{
		result = EPS_PNDEV_RET_UNSUPPORTED;
	}

	return result;
}

/**
 * start timer
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @param [in] pCbf				ptr to Callbackfunction
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16  eps_pndevdrv_timer_ctrl_start(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard;
	LSA_UINT16                  result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pCbf != LSA_NULL);

	eps_pndevdrv_enter();

	pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

	pBoard->TimerCtrl.sTimerCbf = *pCbf;

	if(pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0)
	{
		result = eps_pndevdrv_timer_ctrl_start_intern(pHwInstIn, pCbf);
		EPS_ASSERT(result == EPS_PNDEV_RET_OK);
	}

	pBoard->TimerCtrl.bRunning = LSA_TRUE;

	eps_pndevdrv_exit();

	return result;
	//lint --e(818) Pointer parameter 'pCbf' (line 559) could be declared as pointing to const - API requires it to be non const
}

/**
 * stop timer
 * 
 * @param [in] pHwInstIn 		pointer to structure with hardware configuration
 * @return 	EPS_PNDEV_RET_OK	
 */
LSA_UINT16  eps_pndevdrv_timer_ctrl_stop(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UINT16 result;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pndevdrv_enter();

	result = eps_pndevdrv_timer_ctrl_stop_intern(pHwInstIn);

	eps_pndevdrv_exit();

	return result;
}

/**
 * Callbackfunction which is called after receiving tick event from pndevdrv kernel timer
 * 
 * @see eps_pndev_if_timer_tick - function is called to trigger the real cbf. Also, this function checks for overruns and underruns. A underrun should never occur!
 *
 * @param [in] hDeviceIn		Handle of Device
 * @param [in] eEventClassIn	Enume with different EventClasses (only tick and sync tick event are received)
 * @param [in] pParTmrIn        pointer to timer parameter
 * @return
 */
static LSA_VOID __stdcall eps_pndevdrv_hptimer_cbf(const HANDLE hDeviceIn, const ePNDEV_EVENT_CLASS sEventIn, const LSA_VOID_PTR_TYPE pParTmrIn)
{
	EPS_PNDEVDRV_TMR_PAR_PTR_TYPE   pPnDevTmrPar = (EPS_PNDEVDRV_TMR_PAR_PTR_TYPE) pParTmrIn;
	EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar      = (EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE) &pPnDevTmrPar->TmrPar;

	LSA_UNUSED_ARG(hDeviceIn);

	// Attention:
	//	- it is not allowed to call any service of PnDev_Driver here!

	// call actual timer Cbf if tick is not too early.
	eps_pndev_if_timer_tick(pTmrPar);

	// do additional timer tick event statistics
	switch	(sEventIn)
	{
		case ePNDEV_EVENT_CLASS_TICK:
		case ePNDEV_EVENT_CLASS_SYNC_TICK:
		{
			pPnDevTmrPar->uTicksToSyncCount++;

			if(sEventIn == ePNDEV_EVENT_CLASS_SYNC_TICK)
			{
				pPnDevTmrPar->uSyncCount++;
				if(pPnDevTmrPar->uTicksToSyncCount != PNDEV_EVENT_SYNC_TICK)
				{
					pPnDevTmrPar->uSyncErrCount++;
				}
				pPnDevTmrPar->uTicksToSyncCount = 0;
			}

			break;
		}

        default: 
            break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}
}

/**
* Callback function that is called by the systimer of EPS. This function calls eps_pndev_if_timer_tick() that handles overruns / underruns.
* Checks the current time as well as the last time this function was called. Since we do an oversampling, we do nothing if the uTickUnderrunMarkNs is not reached.
*
* @see eps_pndev_if_timer_tick - function is called to trigger the real cbf. Also, this function checks for overruns and underruns. A underrun should never occur!
*
* @param [in] hDeviceIn		Handle of Device
* @param [in] eEventClassIn	Enume with different EventClasses (only tick and sync tick event are received)
* @param [in] pParTmrIn        pointer to timer parameter
*/
static LSA_VOID __stdcall eps_pndevdrv_timer_cbf(const HANDLE hDeviceIn, const ePNDEV_EVENT_CLASS sEventIn, const LSA_VOID_PTR_TYPE pParTmrIn)
{
	EPS_PNDEVDRV_TMR_PAR_PTR_TYPE   pPnDevTmrPar = (EPS_PNDEVDRV_TMR_PAR_PTR_TYPE) pParTmrIn;
	EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE pTmrPar      = (EPS_PNDEV_TMR_TICK_PAR_PTR_TYPE) &pPnDevTmrPar->TmrPar;
	LSA_UINT64                      uStartTimeNs;
	LSA_UINT64                      uDiffTimeNs;

	LSA_UNUSED_ARG(hDeviceIn);
    LSA_UNUSED_ARG(sEventIn);

    // read current StartTime
    uStartTimeNs = eps_get_ticks_100ns();
    // calc diff to last call to see if we need to call eps_pndev_if_timer_tick or skip this tick
	uDiffTimeNs = (uStartTimeNs - pTmrPar->uTickCbfLastCallTimeNs) * 100;

	if	(uDiffTimeNs > pTmrPar->uTickUnderrunMarkNs)
		// tick is not too early.
	{
		// call actual timer Cbf if tick is not too early.
		eps_pndev_if_timer_tick(pTmrPar);
	}
    else
    {
        // skip this tick. Since we do a oversampling, this path is called way more often then the if path.
    }
}

/**
 * open pndevdrv timer for accurate callbacks on kernel timer tick events
 * 
 * @param	[in] cName				internal timer name
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @param   [in] pTmrPar		    ptr to EPS_PNDEVDRV_TMR_PAR_PTR_TYPE structure (includes ptr to Cbf and is used for adaptive timer tick handling and statistics)
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
static LSA_UINT16 eps_pndevdrv_open_timer(LSA_CHAR* cName, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_PNDEVDRV_TMR_PAR_PTR_TYPE pTmrPar, LSA_BOOL bHPTimer)
{
    uPNDEV_OPEN_TIMER_IN    uOpenTimerIn;
    ePNDEV_RESULT           drvResult;

	EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_timer() - starting timer - Name: %s", cName);
	LSA_UNUSED_ARG (cName);

    EPS_ASSERT(pBoard->bUsed == LSA_TRUE);
	EPS_ASSERT(pTmrPar != LSA_NULL);

    eps_memset(&uOpenTimerIn, 0, sizeof(uOpenTimerIn));
	uOpenTimerIn.uDevice.hHandle			= pBoard->OpenDevice.uDevice.hHandle;
    
	uOpenTimerIn.uThreadKTimer.lPrio		= PNDEV_THREAD_PRIO_TIME_CRITICAL;
	uOpenTimerIn.uCbfParUser.pPtr			= (void *)pTmrPar;
	
	if(bHPTimer)
	{
		uOpenTimerIn.uCbfTick.pPtr			= eps_pndevdrv_hptimer_cbf;
	}
	else
	{
		uOpenTimerIn.uCbfTick.pPtr			= eps_pndevdrv_timer_cbf;
	}

    // open pndevdrv timer -> start timerthread for calling eps_pndevdrv_timer_cbf_intern every 1 ms (after receiving tick event from kernel timer)
	drvResult = g_pEpsPnDevDrvTimerThreads->pDrvHandle->uIf.pFnOpenTimer(g_pEpsPnDevDrvTimerThreads->pDrvHandle, sizeof(uPNDEV_OPEN_TIMER_IN),sizeof(uPNDEV_OPEN_TIMER_OUT), &uOpenTimerIn, &pTmrPar->OpenTmr);


    EPS_ASSERT(drvResult == ePNDEV_OK);

    return EPS_PNDEV_RET_OK;
}

/**
 * close pndevdrv timer
 * 
 * @param	[in] cName				internal timer name
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @param   [in] pTmrPar		    ptr to EPS_PNDEVDRV_TMR_PAR_PTR_TYPE structure (includes uPNDEV_OPEN_TIMER_OUT structure, which is needed for closing timer)
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
static LSA_UINT16 eps_pndevdrv_close_timer(LSA_CHAR* cName, EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard, EPS_PNDEVDRV_TMR_PAR_PTR_TYPE pTmrPar)
{
    uPNDEV_CLOSE_TIMER_IN   uCloseTimerIn;
	uPNDEV_CLOSE_TIMER_OUT	uCloseTimerOut;
    ePNDEV_RESULT           drvResult;

	EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_close_timer() - stopping timer - Name: %s", cName);
	LSA_UNUSED_ARG (cName);

    EPS_ASSERT(pBoard->bUsed == LSA_TRUE);

    eps_memset(&uCloseTimerIn, 0, sizeof(uCloseTimerIn));
    uCloseTimerIn.uTimer.hHandle = pTmrPar->OpenTmr.uTimer.hHandle;

    // close pndevdrv timer -> stop timerthread and unregister it from receiving tick events from kernel timer
    drvResult = g_pEpsPnDevDrvTimerThreads->pDrvHandle->uIf.pFnCloseTimer(g_pEpsPnDevDrvTimerThreads->pDrvHandle, sizeof(uPNDEV_CLOSE_TIMER_IN),sizeof(uPNDEV_CLOSE_TIMER_OUT), &uCloseTimerIn, &uCloseTimerOut);

    EPS_ASSERT(drvResult == ePNDEV_OK);

	eps_memset(&pTmrPar->OpenTmr, 0, sizeof(uPNDEV_CLOSE_TIMER_OUT));

	// show statistics
	EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_close_timer: Ticks: %u, SyncTicks: %u, SyncTickErrors: %u, TickOverruns: %u, TickUnderruns: %u, CbfOverruns: %u", 
						pTmrPar->TmrPar.uTickCount, pTmrPar->uSyncCount, pTmrPar->uSyncErrCount, 
						pTmrPar->TmrPar.uTickOverrunCount, pTmrPar->TmrPar.uTickUnderrunCount,pTmrPar->TmrPar.uTickCbfOverrunCount);

	eps_memset(pTmrPar, 0, sizeof(EPS_PNDEVDRV_TMR_PAR_TYPE));

    return EPS_PNDEV_RET_OK;
}

/**
 * open pndevdrv timer for 1 ms sIsrPnGathered callbacks
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_open_pn_gathered_timer(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_PNDEVDRV_TMR_PAR_PTR_TYPE   pTmrPar  = &pBoard->TmrParPnGathered;
	LSA_BOOL                        bHPTimer = LSA_FALSE;

	eps_memset(pTmrPar, 0, sizeof(EPS_PNDEVDRV_TMR_PAR_TYPE));
	pTmrPar->TmrPar.pTickCbf				= &pBoard->sIsrPnGathered;

	switch (pBoard->pDeviceInfo->eBoard)
    {
        // HighPerformance adaptive timer (1msec)
        case ePNDEV_BOARD_I210:
        case ePNDEV_BOARD_I210SFP:
        case ePNDEV_BOARD_I82574:
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        case ePNDEV_BOARD_TI_AM5728:
        case ePNDEV_BOARD_IX1000:
        {
			pTmrPar->TmrPar.uTickCbfOverrunMarkNs	= EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_DURATION_NS;
			pTmrPar->TmrPar.uTickUnderrunMarkNs		= EPS_PNDEVDRV_CFG_HPTIMER_MIN_CBF_START_DELAY_NS;
			pTmrPar->TmrPar.uTickOverrunMarkNs		= EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_START_DELAY_NS;
			bHPTimer 								= LSA_TRUE;
            break;
        }

        // adaptive timer (32msec)
        case ePNDEV_BOARD_CP1616:
        case ePNDEV_BOARD_EB200:
        case ePNDEV_BOARD_DB_EB200_PCIE:
        case ePNDEV_BOARD_DB_SOC1_PCI:
        case ePNDEV_BOARD_DB_SOC1_PCIE:
		case ePNDEV_BOARD_CP1625:
        case ePNDEV_BOARD_FPGA1_ERTEC200P:
        case ePNDEV_BOARD_EB200P:
        case ePNDEV_BOARD_DB_EB400_PCIE:
		case ePNDEV_BOARD_FPGA1_HERA:
        {
			pTmrPar->TmrPar.uTickCbfOverrunMarkNs	= EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_DURATION_NS;
			pTmrPar->TmrPar.uTickUnderrunMarkNs		= EPS_PNDEVDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS;
			pTmrPar->TmrPar.uTickOverrunMarkNs		= EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS;
			break;
        }

        // unknown board
        case ePNDEV_BOARD_INVALID:
        default:
        {
            EPS_FATAL("eps_pndevdrv_open_pn_gathered_timer(): Unknown Board-Type!!!");
            break;
        }	  
    }

	return eps_pndevdrv_open_timer("EPS_PNDEVGPISR", pBoard, pTmrPar, bHPTimer);
}

/**
 * close pndevdrv timer for 1 ms sIsrPnGathered callbacks
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_close_pn_gathered_timer(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	return eps_pndevdrv_close_timer("EPS_PNDEVGPISR", pBoard, &pBoard->TmrParPnGathered);
}

/**
 * open pndevdrv timer for 1 ms sIsrPnNrt callbacks
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_open_pn_nrt_timer(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	EPS_PNDEVDRV_TMR_PAR_PTR_TYPE   pTmrPar  = &pBoard->TmrParPnNrt;
	LSA_BOOL                        bHPTimer = LSA_FALSE;

	eps_memset(pTmrPar, 0, sizeof(EPS_PNDEVDRV_TMR_PAR_TYPE));
	pTmrPar->TmrPar.pTickCbf				= &pBoard->sIsrPnNrt;

	switch (pBoard->pDeviceInfo->eBoard)
    {
        // HighPerformance adaptive timer (1msec)
        case ePNDEV_BOARD_I210:
        case ePNDEV_BOARD_I210SFP:
        case ePNDEV_BOARD_I82574:
        case ePNDEV_BOARD_MICREL_KSZ8841:
        case ePNDEV_BOARD_MICREL_KSZ8842:
        case ePNDEV_BOARD_TI_AM5728:
        case ePNDEV_BOARD_IX1000:
        {
			pTmrPar->TmrPar.uTickCbfOverrunMarkNs	= EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_DURATION_NS;
			pTmrPar->TmrPar.uTickUnderrunMarkNs		= EPS_PNDEVDRV_CFG_HPTIMER_MIN_CBF_START_DELAY_NS;
			pTmrPar->TmrPar.uTickOverrunMarkNs		= EPS_PNDEVDRV_CFG_HPTIMER_MAX_CBF_START_DELAY_NS;
			bHPTimer 								= LSA_TRUE;
            break;
        }

        // adaptive timer (32msec)
        case ePNDEV_BOARD_CP1616:
        case ePNDEV_BOARD_EB200:
        case ePNDEV_BOARD_DB_EB200_PCIE:
        case ePNDEV_BOARD_DB_SOC1_PCI:
        case ePNDEV_BOARD_DB_SOC1_PCIE:
		case ePNDEV_BOARD_CP1625:
        case ePNDEV_BOARD_FPGA1_ERTEC200P:
        case ePNDEV_BOARD_EB200P:
        case ePNDEV_BOARD_DB_EB400_PCIE:
		case ePNDEV_BOARD_FPGA1_HERA:
        {
			pTmrPar->TmrPar.uTickCbfOverrunMarkNs	= EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_DURATION_NS;
			pTmrPar->TmrPar.uTickUnderrunMarkNs		= EPS_PNDEVDRV_CFG_TIMER_MIN_CBF_START_DELAY_NS;
			pTmrPar->TmrPar.uTickOverrunMarkNs		= EPS_PNDEVDRV_CFG_TIMER_MAX_CBF_START_DELAY_NS;
			break;
        }

        // unknown board
        case ePNDEV_BOARD_INVALID:
        default:
        {
            EPS_FATAL("eps_pndevdrv_open_pn_nrt_timer(): Unknown Board-Type!!!");
            break;
        }	  
    }

	return eps_pndevdrv_open_timer("EPS_PNDEVNRTPISR", pBoard, pTmrPar, bHPTimer);
}

/**
 * close pndevdrv timer for 1 ms sIsrPnNrt callbacks
 * 
 * @param   [in] pBoard 		    ptr to corresponding board structure
 * @return 	EPS_PNDEV_RET_OK,       upon successful execution		
 */
LSA_UINT16 eps_pndevdrv_close_pn_nrt_timer(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
	return eps_pndevdrv_close_timer("EPS_PNDEVNRTPISR", pBoard, &pBoard->TmrParPnNrt);
}

/**
* Poll thread for handling file transfers via shared memory
*
* @param uParam NULL
* @param arg Board descriptor in EPS_PNDEVDRV_BOARD_TYPE format
*/
static LSA_VOID eps_pndevdrv_file_transfer_poll_thread(LSA_UINT32 uParam, void *arg)
{
    LSA_UNUSED_ARG(uParam);

    EPS_PNDEVDRV_BOARD_PTR_TYPE     pBoard;
    EPS_SHM_FILE_TRANSFER_PTR_TYPE  pFileTransfer;
    LSA_UINT8*                      pFileData = LSA_NULL;

    pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)arg;
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr != 0);

    pFileTransfer = (EPS_SHM_FILE_TRANSFER_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr);

    if (pFileTransfer != 0)
    {

        if ((g_pEpsPnDevDrvTimerThreads->bFileAllocated == LSA_TRUE)
            && (pFileTransfer->rsp.bTransferDone == 1)
            )
        {
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_file_transfer_poll_thread() - Firmware download finished");

            EPS_APP_FREE_FILE((EPS_APP_FILE_TAG_TYPE*)&g_pEpsPnDevDrvTimerThreads->file_tag);
            g_pEpsPnDevDrvTimerThreads->bFileAllocated = LSA_FALSE;
            pFileTransfer->rsp.bTransferDoneCnf = 1;
        }

        if (pFileTransfer->req.bReqStart == 1)
        {
            pFileTransfer->rsp.uActPacket = 0;
            pFileTransfer->rsp.bRspErr = 0;
            pFileTransfer->rsp.uActPacketSize = 0;
            pFileTransfer->rsp.uFileSize = 0;

            if (g_pEpsPnDevDrvTimerThreads->bFileAllocated == LSA_FALSE)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_file_transfer_poll_thread() - Start firmware download");

                g_pEpsPnDevDrvTimerThreads->file_tag.pndevBoard = (EPS_PNDEV_BOARD_TYPE) pFileTransfer->req.pndevBoard;
                g_pEpsPnDevDrvTimerThreads->file_tag.pndevBoardDetail = (EPS_EDDP_BOARD_TYPE)pFileTransfer->req.pndevBoardDetail;
                g_pEpsPnDevDrvTimerThreads->file_tag.isHD = (LSA_BOOL)pFileTransfer->req.isHD;
                g_pEpsPnDevDrvTimerThreads->file_tag.FileType = (EPS_APP_FILE_TAG_ENUM) pFileTransfer->req.FileType;

                EPS_APP_ALLOC_FILE((EPS_APP_FILE_TAG_TYPE*)&g_pEpsPnDevDrvTimerThreads->file_tag);
                g_pEpsPnDevDrvTimerThreads->bFileAllocated = LSA_TRUE;
            }

            if (g_pEpsPnDevDrvTimerThreads->file_tag.pFileData && g_pEpsPnDevDrvTimerThreads->file_tag.uFileSize > 0)
            {
                LSA_UINT32 uOffset = pFileTransfer->req.uReqPacketRead * sizeof(pFileTransfer->rsp.uData);

                pFileTransfer->rsp.uFileSize = g_pEpsPnDevDrvTimerThreads->file_tag.uFileSize;

                if (uOffset < pFileTransfer->rsp.uFileSize)
                {
                    pFileTransfer->rsp.uActPacketSize = pFileTransfer->rsp.uFileSize - uOffset; // for last packet n

                    if (pFileTransfer->rsp.uActPacketSize > sizeof(pFileTransfer->rsp.uData)) //correct for chunks 0..n-1
                    {
                        pFileTransfer->rsp.uActPacketSize = sizeof(pFileTransfer->rsp.uData);
                    }

                    pFileData = g_pEpsPnDevDrvTimerThreads->file_tag.pFileData + (long)uOffset;
                    eps_memcpy(pFileTransfer->rsp.uData, pFileData, pFileTransfer->rsp.uActPacketSize);
                    pFileTransfer->rsp.uActPacket = pFileTransfer->req.uReqPacketRead;
                }
                else
                {
                    pFileTransfer->rsp.bRspErr = 1; // Tried to read beyond file
                }
            }
            else
            {
                pFileTransfer->rsp.bRspErr = 1;
            }

            pFileTransfer->req.bReqStart = 0;
            pFileTransfer->rsp.bRspDone = 1;
        }
    }
}

/**
* Start poll thread for handling file transfers via shared memory
* 
* @see eps_pndevdrv_file_transfer_poll_thread
* 
* @param pBoard Board descriptor
* @return EPS_PNDEV_RET_OK Thread started successfully
*/
LSA_UINT16 eps_pndevdrv_create_file_transfer_poll_thread(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard)
{
    pBoard->fileTransferPollThreadHandle = eps_tasks_start_poll_thread("FILE_TRANS_POLL", EPS_POSIX_THREAD_PRIORITY_NORMAL, eSchedFifo, 100 /*ms*/, eps_pndevdrv_file_transfer_poll_thread, 0, pBoard, eRUN_ON_1ST_CORE);
    EPS_ASSERT(pBoard->fileTransferPollThreadHandle != 0);

#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
    eps_tasks_group_thread_add(pBoard->fileTransferPollThreadHandle, EPS_TGROUP_USER);
#endif
    return EPS_PNDEV_RET_OK;
}

/**
*
* @see eps_pndevdrv_kill_poll_thread
*
* @param pBoard Board descriptor
* @return EPS_PNDEV_RET_OK Thread stopped successfully
*/
LSA_VOID eps_pndevdrv_kill_file_transfer_poll_thread(EPS_PNDEVDRV_BOARD_CONST_PTR_TYPE pBoard)
{
#ifndef EPS_CFG_DO_NOT_USE_TGROUPS
    eps_tasks_group_thread_remove(pBoard->fileTransferPollThreadHandle);
#endif

    eps_tasks_stop_poll_thread(pBoard->fileTransferPollThreadHandle);
}

#endif //((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

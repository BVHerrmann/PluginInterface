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
/*  F i l e               &F: eps_pn_soc1_drv.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN SOC1 Driver Interface Adaption                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20035
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>			/* Types / Prototypes / Funcs               */ 
#include <eps_trc.h>			/* Tracing                                  */
#include <eps_rtos.h>			/* OS for Thread Api                        */
#include <eps_tasks.h>          /* EPS TASK API                             */
#include <eps_locks.h>          /* EPS Locks                                */
#include <eps_mem.h>            /* EPS local mem                            */
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */
#include <eps_hw_soc.h>         /* EPS Soc1 hw adaption                     */
#include <eps_pn_drv_if.h>		/* PN Device Driver Interface               */
#include <eps_pn_soc1_drv.h>    /* Driver Interface implementation          */	
#include <eps_shm_if.h>         /* Shared Memory Interface                  */
#include <eps_pncore.h>         /* PnCore Driver Include                    */
#include <eps_app.h>            /* EPS Application Api                      */
#include <eps_register.h>       /* Register access macros                   */
#include <eps_pndrvif.h>
#include <eps_plf.h>

#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
#include <irte_drv.h>
#include <s12i_tlb_config.h>    /* Soc1 offset cached/uncached              */
#endif

/* EDD / HW Includes */
#include <edd_inc.h>
#include <eddi_sys_reg_soc12.h>
#include <eps_statistics.h>     /* EPS Statistics module                    */

#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
#include <sys/eventfd.h>    //kernel-user events
#include <sys/ioctl.h>
#include <linux_irte_drv.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>

// These signals are not defined within linux, so we have to do it by our own
// it should be done in eps_internal_cfg.h
// but in this version of EPS this file is not existing, but if we have it, move it their
// in eps_tasks.c are SIGRT defined too
// move it to eps_internal_cfg.h too and make them work together!
#define SIGRT0  SIGRTMAX-1
#define SIGRT1  SIGRTMAX-2

#endif

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_SOC_TIMERTOP_1TICK_NS    40UL  /* each tick is 40ns */
#define EPS_SOC_TIMER_NR              0

#define EPS_BOARD_TYPE_INVALID      0x0000
#define EPS_BOARD_TYPE_DB_SOC1_PCI  0x403e
#define EPS_BOARD_TYPE_DB_SOC1_PCIE 0x408a
#define EPS_BOARD_TYPE_CP1625       0x4060

#define EPS_PN_SOC1_COUNT_MAC_IF      1
#define EPS_PN_SOC1_COUNT_MAC_PORTS   3

LSA_CHAR*    pEpsSoc1Isr_GAT = "EPS_SOC1GAT_ISR";
LSA_CHAR*    pEpsSoc1Isr_ISO = "EPS_SOC1ISO_ISR";

#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
extern EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw;

/* - Defines from Linker File --------------------------------------------------------------------- */
extern LSA_UINT32 __NRT_RAM_START__;
extern LSA_UINT32 __NRT_RAM_END__;
extern LSA_UINT32 __PNCORE_SHM_START__;
#endif

#ifdef EPS_STATISTICS_ISO_TRANSFEREND
    #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
        #define EPS_STATS_ID_TRANSFEREND_INIT   0xFFFFFFFF
        static LSA_UINT32  stats_id_TransferEnd = EPS_STATS_ID_TRANSFEREND_INIT;
    #endif
#endif // EPS_STATISTICS_ISO_TRANSFEREND

/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_VOID   *eps_pn_soc1_isr_thread              (void *arg);
static LSA_VOID   *eps_pn_soc1_iso_isr_thread          (void *arg);
static LSA_UINT16  eps_pn_soc1_enable_interrupt        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_soc1_disable_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_pn_soc1_set_gpio                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_soc1_clear_gpio              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_soc1_timer_ctrl_start        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_soc1_timer_ctrl_stop         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_soc1_read_trace_data	        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pn_soc1_write_trace_data        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pn_soc1_save_dump               (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_soc1_enable_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_soc1_disable_hw_interrupt    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_soc1_read_hw_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_pn_soc1_write_sync_time_lower   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);

static LSA_UINT16  eps_pn_soc1_open                    (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16  eps_pn_soc1_close                   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);

/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_pn_soc1_board_tag;

typedef struct eps_soc1_isr_tag
{
	LSA_BOOL                        bRunning;
	LSA_BOOL                        bThreadAlive;
	pthread_t                       hThread;
	void *                          pStack;
	EPS_PNDEV_INTERRUPT_DESC_TYPE   uIntSrc;
	struct eps_pn_soc1_board_tag  * pBoard;
	EPS_PN_DRV_ARGS_TYPE            PnDrvArgs;
} EPS_PN_SOC1_DRV_ISR_TYPE, *EPS_PN_SOC1_DRV_ISR_PTR_TYPE;

typedef struct eps_pn_soc1_board_tag
{
	LSA_BOOL                    bUsed;
	EPS_PNDEV_HW_TYPE           sHw;
	LSA_UINT32                  uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE     sIsrIsochronous;
	EPS_PNDEV_CALLBACK_TYPE     sIsrPnGathered;
	EPS_SYS_TYPE                sysDev;

    #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
	int PnFd;
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
		int     evtfd;		// std interrupt event filedescriptor for kernel to user event mechanism
		int     evtfd_iso;  // iso interrupt event filedescriptor for kernel to user event mechanism
	#endif  
	#endif

	struct
	{
		EPS_PNDEV_CALLBACK_TYPE sTimerCbf;
		LSA_UINT32              hThread;
		LSA_BOOL                bRunning;
	} TimerCtrl;
	EPS_PN_SOC1_DRV_ISR_TYPE    IsochronousIsr;
	EPS_PN_SOC1_DRV_ISR_TYPE    PnGatheredIsr;
} EPS_PN_SOC1_DRV_BOARD_TYPE, *EPS_PN_SOC1_DRV_BOARD_PTR_TYPE;
								
typedef struct eps_pn_soc1_store_tag
{
	LSA_BOOL                    bInit;
	LSA_UINT16                  hEnterExit;
	EPS_PN_SOC1_DRV_BOARD_TYPE  board;
} EPS_PN_SOC1_DRV_STORE_TYPE, *EPS_PN_SOC1_DRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_PN_SOC1_DRV_STORE_TYPE      g_EpsPnSoc1Drv;
static EPS_PN_SOC1_DRV_STORE_PTR_TYPE  g_pEpsPnSoc1Drv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#if ((EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL) && (EPS_PLF != EPS_PLF_LINUX_SOC1))
static LSA_UINT16 eps_wait_pn_drv_event(EPS_PN_DRV_EVENT_PTR_TYPE pEvent)
{
	int info;
	sigwait(&pEvent->event, &info);

	return info;
}
#endif	

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsPnSoc1Drv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsPnSoc1Drv->hEnterExit, LSA_FALSE);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsPnSoc1Drv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsPnSoc1Drv->hEnterExit);
}

/**
 * This function links the function pointer required by the EPS PN DEV IF to internal functions.
 *
 * This function is called by eps_pn_soc1_open
 *
 * Note that this function may only be called once since only one HD runs on the SOC1 MIPS.
 * eps_pn_soc1_free_board must be called before a new instance can be allocated.
 *
 * @see eps_pn_soc1_open        - calls this function
 * @see eps_pn_dev_open         - calls eps_pn_soc1_open which calls this function
 * @see eps_pn_soc1_free_board  - corresponding free function
 *
 * @param LSA_VOID
 * @return Pointer to the board structure.
 */
static EPS_PN_SOC1_DRV_BOARD_PTR_TYPE eps_pn_soc1_alloc_board(LSA_VOID)
{
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE const pBoard = &g_pEpsPnSoc1Drv->board;

    // free board entry available?
	if (!pBoard->bUsed)
	{
		pBoard->sHw.hDevice			    = (LSA_VOID*)pBoard;
		pBoard->sHw.EnableIsr		    = eps_pn_soc1_enable_interrupt;
		pBoard->sHw.DisableIsr		    = eps_pn_soc1_disable_interrupt;
		pBoard->sHw.EnableHwIr          = eps_pn_soc1_enable_hw_interrupt;
        pBoard->sHw.DisableHwIr         = eps_pn_soc1_disable_hw_interrupt;
        pBoard->sHw.ReadHwIr            = eps_pn_soc1_read_hw_interrupt;
		pBoard->sHw.SetGpio			    = eps_pn_soc1_set_gpio;
		pBoard->sHw.ClearGpio		    = eps_pn_soc1_clear_gpio;
		pBoard->sHw.TimerCtrlStart	    = eps_pn_soc1_timer_ctrl_start;
		pBoard->sHw.TimerCtrlStop 	    = eps_pn_soc1_timer_ctrl_stop;
		pBoard->sHw.ReadTraceData	    = eps_pn_soc1_read_trace_data;
		pBoard->sHw.WriteTraceData	    = eps_pn_soc1_write_trace_data;
        pBoard->sHw.WriteSyncTimeLower	= eps_pn_soc1_write_sync_time_lower;
		pBoard->sHw.SaveDump		    = eps_pn_soc1_save_dump;
		pBoard->bUsed				    = LSA_TRUE;

		return pBoard;
	}

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_alloc_board(): no free board entry available");

	return LSA_NULL;
}

/**
 * This function releases the board by setting bUsed to false. A new alloc can be called.
 *
 * @see eps_pn_soc1_close        - calls this function
 * @see eps_pn_dev_close         - calls eps_pn_soc1_close which calls this function
 * @see eps_pn_soc1_alloc_board  - corresponding alloc function
 *
 * @param [in] pBoard                - pointer to hardware instance
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_free_board(
    EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

static LSA_UINT16 eps_pn_soc1_register_board_for_isr(
    EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr, 
    EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard, 
    EPS_PNDEV_INTERRUPT_DESC_TYPE   uIntSrc, 
    LSA_BOOL *                      bCreateThread)
{	
	*bCreateThread = LSA_FALSE;

	if(pIsr->pBoard == pBoard)
	{
		return EPS_PNDEV_RET_ERR; /* Board Isr already enabled */
	}
	
	if(pIsr->pBoard == LSA_NULL)
	{
		pIsr->pBoard  = pBoard;  /* Register Board for interrupt */
		pIsr->uIntSrc = uIntSrc;		
	}

	if (!pIsr->bRunning)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}
	 
static LSA_UINT16 eps_pn_soc1_unregister_board_for_isr(
    EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr, 
    EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard, 
    LSA_BOOL *                      bKillThread)
{
    LSA_UNUSED_ARG(pBoard);
    
	pIsr->pBoard = LSA_NULL;
    *bKillThread = LSA_TRUE;

	return EPS_PNDEV_RET_OK;
}

/**
 * interrupt thread for SOC1 
 * runs as long pIsr->bRunning is LSA_TRUE 
 *
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_soc1_isr_thread(void *arg)
{
	volatile EPS_PN_SOC1_DRV_ISR_PTR_TYPE   pIsr = (EPS_PN_SOC1_DRV_ISR_PTR_TYPE)arg;
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE          pBoard;
	int                                     info;
	sigset_t                                init_done_event;

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	LSA_UINT64 	event_nr = 0;
	ssize_t 	temp = 0;
	#else
    #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
    LSA_UINT32                  event_nr=0;	
    EPS_PN_DRV_EVENT_PTR_TYPE   drv_event = &pIsr->PnDrvArgs.evnt;
	#endif
	#endif
		
		
	sigemptyset(&init_done_event);
	sigaddset(&init_done_event, SIGRT0);

	pIsr->bThreadAlive = LSA_TRUE;

	/* Wait until init done */	
	sigwait(&init_done_event, &info);	

	/* Enter polling loop */
	while (pIsr->bRunning)
	{
		#if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_POLL)
        eps_tasks_sleep(1);
		#else
		
		#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
		temp = read(g_pEpsPnSoc1Drv->board.evtfd, &event_nr, sizeof(LSA_UINT64));
		#else
        event_nr = eps_wait_pn_drv_event(drv_event);
		#endif

        if (event_nr == 1)
        {
		#endif
        pBoard = pIsr->pBoard;

		if(pBoard != LSA_NULL)
		{
            switch(pIsr->uIntSrc)
			{
				case EPS_PNDEV_ISR_PN_GATHERED:
                {
					if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
					}
					else
					{
                        EPS_FATAL("eps_pn_soc1_isr_thread(): no callbackfunction set in SOC1 gathered interrupt");
					}

					break;
                }

				default:
				    EPS_FATAL("eps_pn_soc1_isr_thread(): undefined interrupt source");
					break;
			}
		}
        #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
		}
		#endif	
	}

	pIsr->bThreadAlive = LSA_FALSE;

	return NULL;
}

/**
 * ISO interrupt thread for SOC1 
 * runs as long pIsr->bRunning is LSA_TRUE 
 *
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_soc1_iso_isr_thread(void *arg)
{
	volatile EPS_PN_SOC1_DRV_ISR_PTR_TYPE   pIsr = (EPS_PN_SOC1_DRV_ISR_PTR_TYPE)arg;
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE          pBoard;
	int                                     info;
	sigset_t                                init_done_event;

    EPS_ASSERT(pIsr != LSA_NULL);

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	LSA_UINT64 	event_nr = 0;
	ssize_t 	temp = 0;
	#else
	#if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
    LSA_UINT32                  event_nr  = 0;
    EPS_PN_DRV_EVENT_PTR_TYPE   drv_event = &pIsr->PnDrvArgs.evnt;
	#endif
    #endif


	sigemptyset(&init_done_event);
	sigaddset(&init_done_event, SIGRT0);

	pIsr->bThreadAlive = LSA_TRUE;

	/* Wait until init done */	
	sigwait(&init_done_event, &info);	

	pBoard = pIsr->pBoard;
    EPS_ASSERT(pBoard != LSA_NULL);

	/* Enter polling loop */
	while (pIsr->bRunning)
	{

		#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
		temp = read(g_pEpsPnSoc1Drv->board.evtfd_iso, &event_nr, sizeof(LSA_UINT64));
		#else
		event_nr = eps_wait_pn_drv_event(drv_event);
		#endif

		if (event_nr == 1)
		{
            switch (pIsr->uIntSrc)
		    {
			    case EPS_PNDEV_ISR_ISOCHRONOUS:
                {
                    if (pBoard->sIsrIsochronous.pCbf != LSA_NULL)
                    {
                        #ifdef EPS_STATISTICS_ISO_TRANSFEREND                    
                        EPS_ASSERT(stats_id_TransferEnd != EPS_STATS_ID_TRANSFEREND_INIT);
                        eps_statistics_capture_end_value(stats_id_TransferEnd);
                        #endif

                        pBoard->sIsrIsochronous.pCbf(pBoard->sIsrIsochronous.uParam, pBoard->sIsrIsochronous.pArgs);

                        #ifdef EPS_STATISTICS_ISO_TRANSFEREND                    
                        EPS_ASSERT(stats_id_TransferEnd != EPS_STATS_ID_TRANSFEREND_INIT);
                        eps_statistics_capture_start_value(stats_id_TransferEnd);
                        #endif
                    }
                    else
                    {
                        EPS_FATAL("eps_pn_soc1_iso_isr_thread(): no callbackfunction set in SOC1 iso interrupt");
                    }

				    break;
                }

			    default:
				    EPS_FATAL("eps_pn_soc1_iso_isr_thread(): undefined interrupt source");
				    break;
		    }
		}
	}

	pIsr->bThreadAlive = LSA_FALSE;

	return NULL;
}

static LSA_UINT16 eps_pn_soc1_create_isr_thread(LSA_CHAR* cName, EPS_PN_SOC1_DRV_ISR_PTR_TYPE pIsr, EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
    LSA_UNUSED_ARG(pBoard);
    
	pthread_attr_t      th_attr; // Attributes
	pthread_attr_t    * p_th_attr = &th_attr;
	struct sched_param  th_param; // Scheduling parameters
	int                 ret_val;

	/* Init thread attributes */
	ret_val = EPS_POSIX_PTHREAD_ATTR_INIT(p_th_attr);
	EPS_ASSERT(ret_val == 0);

	/* Enable explicitly to set individual scheduling parameters */
	EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED(p_th_attr, PTHREAD_EXPLICIT_SCHED);

	// Set scheduling policy to FIFO
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(p_th_attr, SCHED_FIFO);
	EPS_ASSERT(ret_val == 0);

	/* Set stack and stack size */
	pIsr->pStack = eps_mem_alloc( 64000, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT(pIsr->pStack != LSA_NULL);

	EPS_POSIX_PTHREAD_ATTR_SETSTACK(p_th_attr, pIsr->pStack, 64000);

	/* Set priority */
    if (cName == pEpsSoc1Isr_GAT)
    {
		th_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
    }
    else if (cName == pEpsSoc1Isr_ISO)
    {
		#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
        th_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_ISO;
		#else
		th_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
		#endif
    }

	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM(p_th_attr, &th_param);
	EPS_ASSERT(ret_val == 0);

	/* Set name */
    #if (EPS_PLF != EPS_PLF_LINUX_SOC1)
    ret_val = EPS_POSIX_PTHREAD_ATTR_SETNAME(p_th_attr, cName);
    #endif
	EPS_ASSERT(ret_val == 0);

	pIsr->bRunning = LSA_TRUE;

	/* Create Thread */
	if (cName == pEpsSoc1Isr_GAT)
	{
	ret_val = EPS_POSIX_PTHREAD_CREATE(&pIsr->hThread, &th_attr, eps_pn_soc1_isr_thread, (LSA_VOID*)pIsr);
	}
	else if (cName == pEpsSoc1Isr_ISO)
	{
        ret_val = EPS_POSIX_PTHREAD_CREATE(&pIsr->hThread, &th_attr, eps_pn_soc1_iso_isr_thread, (LSA_VOID*)pIsr);
	}
	else
	{
	    EPS_FATAL("eps_pn_soc1_create_isr_thread(): unknown cName");
	}

	EPS_ASSERT(ret_val == 0);

    /* Set name */
    #if (EPS_PLF == EPS_PLF_LINUX_SOC1)
    ret_val = EPS_POSIX_PTHREAD_ATTR_SETNAME(pIsr->hThread, cName);
    #endif

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_soc1_kill_isr_thread(LSA_CHAR* cName, EPS_PN_SOC1_DRV_ISR_PTR_TYPE pIsr, EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_VOID  * th_ret;
	LSA_UINT16  ret_val;

    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	LSA_UINT64  event_nr = 2;
    ssize_t     temp = 0;
    #endif

	LSA_UNUSED_ARG(pBoard);

	pIsr->bRunning = LSA_FALSE;

    if (cName == pEpsSoc1Isr_GAT)
    {
        #if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_USERMODE)
        #if(EPS_PLF != EPS_PLF_LINUX_SOC1)
	    EPS_POSIX_PTHREAD_KILL(pIsr->hThread, SIGRT1);
        #else	    
	    temp = write(g_pEpsPnSoc1Drv->board.evtfd, &event_nr, sizeof(LSA_UINT64));	    
        #endif
	    #endif
    }
    else if (cName == pEpsSoc1Isr_ISO)
    {
        #if(EPS_PLF != EPS_PLF_LINUX_SOC1)
        EPS_POSIX_PTHREAD_KILL(pIsr->hThread, SIGRT1);
        #else        
        temp = write(g_pEpsPnSoc1Drv->board.evtfd_iso, &event_nr, sizeof(LSA_UINT64));        
        #endif
    }
    else
    {
        EPS_FATAL("eps_pn_soc1_kill_isr_thread(): unknown cName");
    }

	EPS_POSIX_PTHREAD_JOIN(pIsr->hThread, &th_ret); /* Wait until Thread is down */

	// free stack mem
	ret_val = eps_mem_free(pIsr->pStack, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
	EPS_ASSERT(ret_val == EPS_PNDEV_RET_OK);
    pIsr->pStack = LSA_NULL;
	
	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_soc1_create_pn_gathered_isr_thread(EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bCreateThread;
	EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_soc1_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if (bCreateThread)
	{
		retVal = eps_pn_soc1_create_isr_thread(pEpsSoc1Isr_GAT, pIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_pn_soc1_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_pn_soc1_kill_pn_gathered_isr_thread(EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bKillThread;
	EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_soc1_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if (bKillThread)
	{
	    eps_pn_soc1_kill_isr_thread(pEpsSoc1Isr_GAT, pIsr, pBoard);
	}

	return retVal;
}

/**
 * Create the ISO-ISR-Thread
 * @param pBoard pointer to board instance
 * @return #EPS_PNDEV_RET_OK
 * @return anything else
 */
static LSA_UINT16 eps_pn_soc1_create_iso_isr_thread(EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bCreateThread;
	EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr;

	EPS_ASSERT(pBoard != LSA_NULL);

	pIsr = &pBoard->IsochronousIsr;

	retVal = eps_pn_soc1_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_ISOCHRONOUS, &bCreateThread);

	if (bCreateThread)
	{
		retVal = eps_pn_soc1_create_isr_thread(pEpsSoc1Isr_ISO, pIsr, pBoard);

		if (retVal != EPS_PNDEV_RET_OK)
		{
		    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_create_iso_isr_thread(): call failed -> eps_pn_soc1_create_isr_thread()");
			eps_pn_soc1_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}
	else
	{
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_create_iso_isr_thread(): call failed -> eps_pn_soc1_register_board_for_isr()");
	}

	return retVal;
}

/**
 * Kill the ISO-ISR-Thread
 * @param pBoard pointer to board instance
 * @return #EPS_PNDEV_RET_OK
 * @return anything else
 */
static LSA_UINT16 eps_pn_soc1_kill_iso_isr_thread(EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bKillThread;
	EPS_PN_SOC1_DRV_ISR_PTR_TYPE    pIsr;

    EPS_ASSERT(pBoard != LSA_NULL);

    pIsr = &pBoard->IsochronousIsr;

    retVal = eps_pn_soc1_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if (bKillThread)
	{
	    eps_pn_soc1_kill_isr_thread(pEpsSoc1Isr_ISO, pIsr, pBoard);
	}
	else
	{
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_kill_iso_isr_thread(): call failed -> eps_pn_soc1_unregister_board_for_isr()");
	}

	return retVal;
}


/**
 * get type of SOC1 board
 * 
 * @param pBoard pointer to board instance
 * @return #EPS_BOARD_TYPE_DB_SOC1_PCI
 * @return #EPS_BOARD_TYPE_DB_SOC1_PCIE
 * @return anything else
 */
static LSA_UINT32 eps_pn_soc1_get_board_type(EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard)
{
    LSA_UNUSED_ARG(pBoard);
    
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	return EPS_BOARD_TYPE_CP1625;
	#else
	LSA_UINT16 uDeviceId = 0;

	/* The DB_SOC1_PCI and DB_SOC1_PCIE are evaluated by the AhbPci-BridgeRegister Byte 2+3 = Device ID */
	/* DB_SOC1_PCI = Device Id 0x403e and DB_SOC1_PCIE = Device Id 0x408a */

	uDeviceId = (LSA_UINT16)(*((LSA_UINT32*)(0x1d500000 /* SOC1 PCI Bridge Base address */))>>16);

	switch(uDeviceId)
	{
		case EPS_BOARD_TYPE_DB_SOC1_PCI:
		case EPS_BOARD_TYPE_DB_SOC1_PCIE:
		case EPS_BOARD_TYPE_CP1625:
		  return uDeviceId; /* value should be EPS_BOARD_TYPE_DB_SOC1_PCI or EPS_BOARD_TYPE_DB_SOC1_PCIE */
			break;
		default:
			EPS_FATAL("eps_pn_soc1_get_board_type(): Unknown SOC1 board type.");
			break;
	}

  	return EPS_BOARD_TYPE_INVALID;
	#endif	
}

/**
 * Undo initialization by setting bInit to FALSE.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_soc1_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsPnSoc1Drv->bInit);

    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    close(g_pEpsPnSoc1Drv->board.PnFd);
    #else
    #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
	/* IRTE Driver Uninstall */
	if(irte_drv_uninstall() != EPS_PN_DRV_RET_OK)
	{
	    EPS_FATAL("eps_pn_soc1_uninstall(): Uninstalling SOC1 driver failed");
	}
    #endif
    #endif

	eps_pn_soc1_undo_init_critical_section();

	g_pEpsPnSoc1Drv->bInit = LSA_FALSE;

	g_pEpsPnSoc1Drv = LSA_NULL;
}

/**
 * Installing SOC1 driver. The SOC1 PN Dev Driver implements the eps_pndev_if.
 *
 * The SOC1 driver requires the irte_drv for interrupt integration.
 * 
 * Note that this module is implemented to run on the LD / HD firmware that runs on the MIPS core on the SOC1 with the adonis operating system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pn_soc1_open
 *  eps_pndev_if_close     -> eps_pn_soc1_close
 *  eps_pndev_if_uninstall -> eps_pn_soc1_uninstall
 *
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * @see irte_drv_install      - Interrupt integration.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_soc1_drv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnSoc1DrvIf;	

	eps_memset(&g_EpsPnSoc1Drv, 0, sizeof(g_EpsPnSoc1Drv));
	g_pEpsPnSoc1Drv = &g_EpsPnSoc1Drv;

	g_pEpsPnSoc1Drv->bInit = LSA_TRUE;

	eps_pn_soc1_init_critical_section();	

	//Init Interface
	sPnSoc1DrvIf.open      = eps_pn_soc1_open;
	sPnSoc1DrvIf.close     = eps_pn_soc1_close;
	sPnSoc1DrvIf.uninstall = eps_pn_soc1_uninstall;

	eps_pndev_if_register(&sPnSoc1DrvIf);

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	g_pEpsPnSoc1Drv->board.PnFd = open(LINUX_IRTE_DRV_NAME, O_RDWR);
	EPS_ASSERT(g_pEpsPnSoc1Drv->board.PnFd != -1);
    #else
    #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
    /* IRTE Driver Setup (TopLevel - ICU) */
    if(irte_drv_install() != EPS_PN_DRV_RET_OK)
    {
        EPS_FATAL("eps_pn_soc1_drv_install(): Installing SOC1 driver failed");
    }
    g_pEpsPnSoc1Drv->board.PnFd = open(IRTE_DRV_NAME, O_RDWR);
    EPS_ASSERT(g_pEpsPnSoc1Drv->board.PnFd != -1);			
	#endif
	#endif
}

/**
 * open SOC1 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_open. 
 * The function may only be called once since there is only one HD.
 * 
 * @see eps_pndev_if_open             - calls this function
 * 
 * @param [in] pLocation              - location descriptor (PCI Location or MAC Address)
 * @param [in] pOption                - options (debug, download of firmware). Note that not all PN Dev implementations support this.
 * @param [in] ppHwInstOut            - out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @param [in] hd_id                  - hd number. 0 = LD, 1...4 = HD instance.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_soc1_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
    LSA_UNUSED_ARG(pOption);
    
    EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard;	
    LSA_UINT16                      result = EPS_PNDEV_RET_OK;
    EPS_MAC_PTR_TYPE                pMac = LSA_NULL;
    LSA_UINT16                      macIdx, portIdx;	
    LSA_UINT32                      ahb_virt_base = 0x10000000;

    EPS_APP_MAC_ADDR_ARR            MacAddressesApp;
    LSA_UINT16                      loopCnt = 0;
    
    /*----------------------------------------------------------*/

	EPS_ASSERT(g_pEpsPnSoc1Drv->bInit);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_pn_soc1_enter();

	pBoard = eps_pn_soc1_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */
	
	eps_memset( &pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo) );
	
	// set board type
	switch ( eps_pn_soc1_get_board_type(pBoard) ) // Type of board
	{
		case EPS_BOARD_TYPE_DB_SOC1_PCI: // SOC1 board PCI
		{
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCI;
		}
		break;
		case EPS_BOARD_TYPE_DB_SOC1_PCIE: // SOC1 board PCIe
		{
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCIE;
		}
		break;
        case EPS_BOARD_TYPE_CP1625: // CP1625 PCIe
        {
            pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_CP1625;
        }
        break;
        default:
            EPS_FATAL("eps_pn_soc1_open(): Undefinded SOC1 board type when opening board.");
		break;
	}
	
	pBoard->sysDev.hd_nr        = hd_id;
	pBoard->sysDev.pnio_if_nr   = 0;	// don't care
	pBoard->sysDev.edd_comp_id  = LSA_COMP_ID_EDDI;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle  = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type       = LSA_COMP_ID_EDDI;

	// Init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param( &pBoard->sHw.EpsBoardInfo );

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_IRTEREV7;			
	pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_SOC;	
	
	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	/*------------------------*/	

	pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

	// IRTE settings
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr, 0x0D200000 + ahb_virt_base, 0x200000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)0x0D200000 + ahb_virt_base;
	#endif
	pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = 0x1D200000;
	pBoard->sHw.EpsBoardInfo.eddi.irte.size     = 0x1D3FFFFF - pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr;

	// KRAM settings
	//	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = ;
	//	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = ;
	//	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = ;

	// SDRAM settings (for NRT)
	// UT5507 - Changed mapping of NRT
    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    // the address of the NRT memory has to be set via a define in eps_internal_cfg.h
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr, 0x0EE08000, 0xB00000);
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = 0x4EE08000;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = 0x00B00000;
    #else
    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)((LSA_UINT32)__NRT_RAM_START__);
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)__NRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = (LSA_UINT32)__NRT_RAM_END__ - (LSA_UINT32)__NRT_RAM_START__; /* nrt pool size */
    #endif
    
    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_soc1_open(): sdSize=0x%x, phyAddr=0x%x", pBoard->sHw.EpsBoardInfo.eddi.sdram.size, pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);
    
    // SDRAM cached
    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
    pBoard->sHw.EpsBoardInfo.eddi.sdram_cached.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr - EPS_ADONIS_SDRAM_CACHED_UNCACHED_OFFSET;
    pBoard->sHw.EpsBoardInfo.eddi.sdram_cached.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr;
    pBoard->sHw.EpsBoardInfo.eddi.sdram_cached.size     = pBoard->sHw.EpsBoardInfo.eddi.sdram.size;
    #endif // PSI_CFG_USE_NRT_CACHE_SYNC

    // Shared Mem settings - addresses are not available here
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr      = LSA_NULL;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr      = 0;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size          = 0;

	// APB-Peripherals SCRB
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr, 0x0FB00000 + ahb_virt_base, 0x100000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)0x0fb00000 + ahb_virt_base;
	#endif
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr = 0x1fb00000;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     = 0x1fbFFFFF - pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr;

	// APB-Peripherals TIMER
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr, 0x0F900000 + ahb_virt_base, 0x100000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr = (LSA_UINT8*)0x0F900000 + ahb_virt_base;
	#endif
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr = 0x1F900000;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size     = 0x1F9FFFFF - pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr;

	// APB-Peripherals GPIO
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr, 0x0FA00000 + ahb_virt_base, 0x100000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr = (LSA_UINT8*)0x0FA00000 + ahb_virt_base;
	#endif
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr = 0x1FA00000;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size     = 0x1FAFFFFF - pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr;

	// IOCC settings
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr, 0x0D600000 + ahb_virt_base, 0x200000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr = (LSA_UINT8*)0x0D600000 + ahb_virt_base;
	#endif
	pBoard->sHw.EpsBoardInfo.eddi.iocc.phy_addr = 0x1D600000;
	pBoard->sHw.EpsBoardInfo.eddi.iocc.size     = 0x1D7FFFFF - pBoard->sHw.EpsBoardInfo.eddi.iocc.phy_addr; // PAEA-RAM size

	// I2C setting
	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    EPS_PLF_MAP_MEMORY(&pBoard->sHw.EpsBoardInfo.eddi.i2c_base_ptr, 0x0EB00000 + ahb_virt_base, 0x80000);
	#else
	pBoard->sHw.EpsBoardInfo.eddi.i2c_base_ptr = (LSA_UINT8*)0x0EB00000 + ahb_virt_base;
	#endif

	// PAEA settings
	pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr + + U_IOCC_PA_EA_DIRECT_start;
	pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.iocc.phy_addr + U_IOCC_PA_EA_DIRECT_start;
	pBoard->sHw.EpsBoardInfo.eddi.paea.size     = (U_IOCC_PA_EA_DIRECT_end - U_IOCC_PA_EA_DIRECT_start)/4 + 1; // RAM size
	
	pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll        = LSA_TRUE;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 156;  // use ISO_OUT4
	pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr  = 94;   // use ISO_IN0

	// Set POF, PHY-Led and PLL functions
	pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct   = eps_set_pllport_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct    = EPS_SOC1_LED_BLINK_BEGIN; /* Done by EDDI */
	pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct      = EPS_SOC1_LED_BLINK_END; /* Done by EDDI */
	pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct = EPS_SOC1_LED_BLINK_SET_MODE; /* Done by EDDI */
	pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct        = eps_pof_set_led_SOC;

	// Set I2C functions
	pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz = eps_i2c_set_scl_low_highz_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz = eps_i2c_set_sda_low_highz_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read          = eps_i2c_sda_read_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select            = eps_i2c_select_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset    = eps_i2c_ll_read_offset_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset   = eps_i2c_ll_write_offset_SOC;

	// SII settings (EDDI runs on EPC environment, ExtTimer exists)
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector      = EDDI_SII_IRQ_SP;
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber        = EDDI_SII_IRQ_0;
	
    #if defined ( EDDI_CFG_SII_EXTTIMER_MODE_ON )
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 1000UL;    //0, 250, 500, 1000 in us   
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 0UL;    //0, 250, 500, 1000 in us
    #endif

    // setup CRT memory pools
    eps_soc_FillCrtMemParams(&pBoard->sHw.EpsBoardInfo, &pBoard->sHw.EpsBoardInfo.eddi.shared_mem);

	// setup NRT memory pools
	eps_soc_FillNrtMemParams(&pBoard->sHw.EpsBoardInfo);

	// setup PI (IOCC is used, with full size
	// Note: we use the full size of IOCC, real size is calculated in PSI
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddi.paea.size;

	// setup HIF buffer => HIF HD on same processor
	pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.size     = 0; 
	
    // SRD API Memory is located in pncore shared memory, see eps_pncore.c
    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	// TODO Pncore shared memory base address??
    #else
	pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr = g_pEpsPlfShmHw->sSrdApi.pBase;
	pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr = (LSA_UINT32)__PNCORE_SHM_START__ + g_pEpsPlfShmHw->sSrdApi.uOffset;
	pBoard->sHw.EpsBoardInfo.srd_api_mem.size     = g_pEpsPlfShmHw->sSrdApi.uSize;
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                        "eps_pn_soc1_open(hd_id(%u)): srd_api_mem: base_ptr=0x%x phy_addr=0x%x size=0x%x",
                        hd_id,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.size);
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDI_GSHAREDMEM_TYPE));
    #endif
		
	/*------------------------*/
	
    // -- MAC address handling START ----------------------------------

	// prepare the MAC-array
	eps_memset(&MacAddressesApp, 0, sizeof(MacAddressesApp));

	// for linux soc1 standalone this part is deactivated because we cannot get the mac addresses from anywhere
	// application has to give them to us
	#if ( (EPS_PLF != EPS_PLF_LINUX_SOC1 ) || ( (EPS_PLF == EPS_PLF_LINUX_SOC1 ) && ( EPS_CFG_USE_HIF_LD == 1 ) ) )
	{
		EPS_PNCORE_MAC_ADDR_TYPE        MacAddressesCore;

		eps_memset(&MacAddressesCore, 0, sizeof(MacAddressesCore));

		// Interface part
		// First we get the MAC address from the hardware
		eps_pncore_get_mac_addr(&MacAddressesCore);
		EPS_ASSERT(MacAddressesCore.lCtrMacAdr > 0);

		// we have to copy the given MAC addresses to our structure which will be passed to the application
		for (loopCnt = 0; loopCnt < (EPS_PN_SOC1_COUNT_MAC_IF + EPS_PN_SOC1_COUNT_MAC_PORTS); loopCnt++)
		{
			MacAddressesApp.lArrayMacAdr[loopCnt][0] = MacAddressesCore.lArrayMacAdr[loopCnt][0];
			MacAddressesApp.lArrayMacAdr[loopCnt][1] = MacAddressesCore.lArrayMacAdr[loopCnt][1];
			MacAddressesApp.lArrayMacAdr[loopCnt][2] = MacAddressesCore.lArrayMacAdr[loopCnt][2];
			MacAddressesApp.lArrayMacAdr[loopCnt][3] = MacAddressesCore.lArrayMacAdr[loopCnt][3];
			MacAddressesApp.lArrayMacAdr[loopCnt][4] = MacAddressesCore.lArrayMacAdr[loopCnt][4];
			MacAddressesApp.lArrayMacAdr[loopCnt][5] = MacAddressesCore.lArrayMacAdr[loopCnt][5];
		}
	}
	#endif

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PN_SOC1_COUNT_MAC_IF + EPS_PN_SOC1_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PN_SOC1_COUNT_MAC_IF + EPS_PN_SOC1_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PN_SOC1_COUNT_MAC_IF + EPS_PN_SOC1_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    // Use MAC from driver
    pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[0];
    pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    
    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_SOC1_COUNT_MAC_PORTS;

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PN_SOC1_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// Set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

	///*------------------------------------------------*/
	
	// Prepare the board specific HW functions and GPIO numbers
	// Note used for PHY LED, PLL and LEDs
	switch ( eps_pn_soc1_get_board_type(pBoard) ) // Type of board
	{
		case EPS_BOARD_TYPE_DB_SOC1_PCI: // SOC1 board PCI
		{
 			// setup portmapping (1:2, 2:3, 3:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;

			pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 1;
		}
		break;

		case EPS_BOARD_TYPE_DB_SOC1_PCIE: // SOC1 board PCIe
		{
			// setup portmapping (=1:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

			pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;
		}
		break;
		
        case EPS_BOARD_TYPE_CP1625: // CP1625 PCIe
        {
            // setup portmapping
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

            pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
            pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;
        }
        break;

	    default:
            EPS_FATAL("eps_pn_soc1_open(): Undefinded SOC1 board type when opening board.");
		break;
	}

	///*------------------------------------------------*/

	if(result == EPS_PNDEV_RET_OK)
	{
		*ppHwInstOut = &pBoard->sHw;

		eps_init_hw_SOC(hd_id);
	}
	else
	{
		eps_pn_soc1_free_board(pBoard);
	}

    //Init SCRB and GPIOs for SoC1 standalone use
    //copied from PnDevDrv PnDev_Util.c fnDrvUtil_InitPhySoc1()
    #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
    #define PNDEV_SOC1_SCRB_REG__IRTE_CTRL      0x150
    #define PNDEV_SOC1_SCRB_REG__ENABLE_PN_PB   0xA00
    #define PNDEV_SOC1_GPIO_REG__PORT_MODE0_L   0x18
    #define PNDEV_SOC1_GPIO_REG__PORT_MODE0_H   0x1c
    #define PNDEV_SOC1_GPIO_REG__PORT_MODE1_L   0x38
    #define PNDEV_SOC1_GPIO_REG__PORT_MODE1_H   0x3c

    volatile LSA_UINT32*    pIrteCtrl = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr + PNDEV_SOC1_SCRB_REG__IRTE_CTRL);
    volatile LSA_UINT32*    pPnPb = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr + PNDEV_SOC1_SCRB_REG__ENABLE_PN_PB);
    volatile LSA_UINT32*    pGpioRegPortMode0_L = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr + PNDEV_SOC1_GPIO_REG__PORT_MODE0_L);
    volatile LSA_UINT32*    pGpioRegPortMode0_H = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr + PNDEV_SOC1_GPIO_REG__PORT_MODE0_H);
    volatile LSA_UINT32*    pGpioRegPortMode1_L = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr + PNDEV_SOC1_GPIO_REG__PORT_MODE1_L);
    volatile LSA_UINT32*    pGpioRegPortMode1_H = (volatile LSA_UINT32*)(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr + PNDEV_SOC1_GPIO_REG__PORT_MODE1_H);

    // PHY clock isn't generated by external oscillator
    // enable PHY clock
    *pPnPb &= 0x7;

    // set interface type of PHYs (IRTE channel 0/1/2 in MII mode)
    //  - MII:  4Bit interface to PHY, 0,25*clock=25MHz, clock by MAC (easier, but more lines required)
    //  - RMII: 2Bit interface to PHY, 0,50*clock=50MHz, clock by PHY
    *pIrteCtrl = 0x7;

    // connect PHYs by MII
    {
        // set AltFct1 of GPIO9:8 / GPIO3:0
        //  - PortMode0_L:  AltFct of GPIO15:0 can be set
        //                  only the output needs to be set, because all inputs gets the signal parallel
        //                  xxxx xxxx xxxx 1010_xxxx xxxx 1010 1010
        //  Signal          GPIO        Fct(=AltFct+1)      Output      Input
        //  MII1_TXD0       0           2                   x
        //  MII1_TXD1       1           2                   x
        //  MII1_TXD2       2           2                   x
        //  MII1_TXD3       3           2                   x
        //  MII1_RXD0       4                                           x
        //  MII1_RXD1       5                                           x
        //  MII1_RXD2       6                                           x
        //  MII1_RXD3       7                                           x
        //  MII1_TX_ER      8           2                   x
        //  MII1_TX_EN      9           2                   x
        //  MII1_TX_CLK     10                                          x
        //  MII1_RX_CLK     11                                          x
        //  MII1_COL        12                                          x
        //  MII1_RX_ER      13                                          x
        //  MII1_CRS        14                                          x
        //  MII1_RX_DV      15                                          x
        *pGpioRegPortMode0_L |= 0x000a00aa;

        // set AltFct1 of GPIO25:24 / GPIO19:16
        //  - PortMode0_H:  AltFct of GPIO31:16 can be set
        //                  only the output needs to be set, because all inputs gets the signal parallel
        //                  xxxx xxxx xxxx 1010_xxxx xxxx 1010 1010
        //  Signal          GPIO        Fct(=AltFct+1)      Output      Input
        //  MII2_TXD0       16          2                   x
        //  MII2_TXD1       17          2                   x
        //  MII2_TXD2       18          2                   x
        //  MII2_TXD3       19          2                   x
        //  MII2_RXD0       20                                          x
        //  MII2_RXD1       21                                          x
        //  MII2_RXD2       22                                          x
        //  MII2_RXD3       23                                          x
        //  MII2_TX_ER      24          2                   x
        //  MII2_TX_EN      25          2                   x
        //  MII2_TX_CLK     26                                          x
        //  MII2_RX_CLK     27                                          x
        //  MII2_COL        28                                          x
        //  MII2_RX_ER      29                                          x
        //  MII2_CRS        30                                          x
        //  MII2_RX_DV      31                                          x
        *pGpioRegPortMode0_H |= 0x000a00aa;

        // set AltFct1 of GPIO41:40 / GPIO35:32
        //  - PortMode1_L:  AltFct of GPIO47:32 can be set
        //                  only the output needs to be set, because all inputs gets the signal parallel
        //                  xxxx xxxx xxxx 1010_xxxx xxxx 1010 1010
        //  Signal          GPIO        Fct(=AltFct+1)      Output      Input
        //  MII3_TXD0       32          2                   x
        //  MII3_TXD1       33          2                   x
        //  MII3_TXD2       34          2                   x
        //  MII3_TXD3       35          2                   x
        //  MII3_RXD0       36                                          x
        //  MII3_RXD1       37                                          x
        //  MII3_RXD2       38                                          x
        //  MII3_RXD3       39                                          x
        //  MII3_TX_ER      40          2                   x
        //  MII3_TX_EN      41          2                   x
        //  MII3_TX_CLK     42                                          x
        //  MII3_RX_CLK     43                                          x
        //  MII3_COL        44                                          x
        //  MII3_RX_ER      45                                          x
        //  MII3_CRS        46                                          x
        //  MII3_RX_DV      47                                          x
        *pGpioRegPortMode1_L |= 0x000a00aa;
    }

    // connect SMI (Serial Management Interface)
    //  - MDC: clock
    //  - MDIO control of ManagementData
    {
        // set AltFct0 of GPIO50:48
        //  - PortMode1_H:  AltFct of GPIO63:48 can be set
        //                  xxxx xxxx xxxx xxxx_xxxx xxxx xx01 0101
        //  Signal          GPIO        Fct(=AltFct+1)      Output      Input
        //  MII_MD_DAT      48          1                   x
        //  MII_MD_CLK      49          1                   x
        //  RES_PHY_N       50          1                   x
        *pGpioRegPortMode1_H |= 0x00000015;
    }

    //CP1625: NSC DualPHY need min 167ms Post Power Up Stabilization time
    eps_tasks_sleep(200);

#endif





	eps_pn_soc1_exit();

	return result;
}

/**
 * Close the SOC1 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance to close.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_soc1_close (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{	
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	//EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_soc1_enter();

	pBoard = (EPS_PN_SOC1_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);	

	eps_undo_init_hw_SOC(pBoard->sysDev.hd_nr);

	#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr,				0x200000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr,				0xB00000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr,		0x100000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr,	0x100000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr,		0x100000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr,				0x200000);
	EPS_PLF_UNMAP_MEMORY(pBoard->sHw.EpsBoardInfo.eddi.i2c_base_ptr,					0x080000);
	#endif

	eps_pn_soc1_free_board(pBoard);

	eps_pn_soc1_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * Enabling interrupts for SOC1. This function implements the EPS PN DEV API function EnableIsr
 * 
 * @param [in] pHwInstIn   - pointer to hardware instance
 * @param [in] pInterrupt  - type of interrupt can be:
 * - #EPS_PNDEV_ISR_PN_GATHERED
 * - #EPS_PNDEV_ISR_PN_NRT
 * - #EPS_PNDEV_ISR_ISOCHRONOUS
 * @param [in] pCbf  callbackfunction with parameters
 * 
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED when unknown interrupt type
 */
static LSA_UINT16 eps_pn_soc1_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16                      result = EPS_PNDEV_RET_OK;
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PN_SOC1_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	eps_pn_soc1_enter();	

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_soc1_enable_interrupt(): pInterrupt=%u", (*pInterrupt));

	switch (*pInterrupt)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
        {
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			result = eps_pn_soc1_create_pn_gathered_isr_thread(pBoard);

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			else
			{
				#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
				tgroup_thread_add((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), pBoard->PnGatheredIsr.hThread);
				#endif

                #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
					int retVal;

					pBoard->evtfd = eventfd(0, EFD_SEMAPHORE);
					EPS_ASSERT(pBoard->evtfd != -1);

					retVal = ioctl(pBoard->PnFd, LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_ENABLE, pBoard->evtfd);
					EPS_ASSERT(retVal == 0);
                #else                    
                #if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_USERMODE)
                    int retVal;
                    pBoard->PnGatheredIsr.PnDrvArgs.evnt.th_id = pBoard->PnGatheredIsr.hThread;
                    retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_ENABLE, &pBoard->PnGatheredIsr.PnDrvArgs.evnt);
                    EPS_ASSERT(retVal == 0);
                    #endif
                #endif
			}

			EPS_POSIX_PTHREAD_KILL(pBoard->PnGatheredIsr.hThread, SIGRT0);	/* PN NRT ISR Thread */

			break;
        }

		case EPS_PNDEV_ISR_INTERRUPT:
        {    
            #if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
            int retVal;
            retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_IRQ_ICU_ENABLE_KERNEL, pCbf);
            EPS_ASSERT(retVal == 0);               
            #else
            result = EPS_PNDEV_RET_UNSUPPORTED;
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_enable_interrupt(): not supported with configured EPS_ISR_MODE");
            #endif

            break;
        }

		case EPS_PNDEV_ISR_POLLINTERRUPT:
        {
            EPS_FATAL("eps_pn_soc1_enable_interrupt(): EPS_PNDEV_ISR_POLLINTERRUPT is not implemented");
            break;
        }

		case EPS_PNDEV_ISR_CPU:
        {
            EPS_FATAL("eps_pn_soc1_enable_interrupt(): EPS_PNDEV_ISR_CPU is not implemented");
            break;
        }

        #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
		case EPS_PNDEV_ISR_ISOCHRONOUS:
        {
            int         retVal;
            #ifdef EPS_STATISTICS_ISO_TRANSFEREND                    
            LSA_UINT32  retVal_stats;
            #endif

            if (pBoard->sIsrIsochronous.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_enable_interrupt(): sIsrIsochronous.pCbf is not NULL");
				break;
			}

			pBoard->sIsrIsochronous = *pCbf;

            result = eps_pn_soc1_create_iso_isr_thread(pBoard);

			if (result != EPS_PNDEV_RET_OK)
			{
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_enable_interrupt(): call failed -> eps_pn_soc1_create_iso_isr_thread()");

				pBoard->sIsrIsochronous.pCbf = LSA_NULL;
			}
            else
            {
                #ifdef EPS_STATISTICS_ISO_TRANSFEREND
                // first allocation must be done before activating
                // because it is possibly that the first interrupt will be come immediately and calls the "eps_pn_soc1_iso_isr_thread()"
                EPS_ASSERT(stats_id_TransferEnd == EPS_STATS_ID_TRANSFEREND_INIT);
                retVal_stats = eps_statistics_alloc_stats(&stats_id_TransferEnd, "ISO_TransferEnd");
                EPS_ASSERT(retVal_stats == LSA_RET_OK);
                eps_statistics_capture_start_value(stats_id_TransferEnd);
                #endif // EPS_STATISTICS_ISO_TRANSFEREND

				#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
				pBoard->evtfd_iso = eventfd(0, EFD_SEMAPHORE);
				EPS_ASSERT(pBoard->evtfd_iso != -1);

				retVal = ioctl(pBoard->PnFd, LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_ENABLE, pBoard->evtfd_iso);
				EPS_ASSERT(retVal == 0);
				#else     
                pBoard->IsochronousIsr.PnDrvArgs.evnt.th_id = pBoard->IsochronousIsr.hThread;
                retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_ISOCHRONOUS_IRQ_ICU_ENABLE, &pBoard->IsochronousIsr.PnDrvArgs.evnt);
                EPS_ASSERT(retVal == 0);
				#endif
            }

			EPS_POSIX_PTHREAD_KILL(pBoard->IsochronousIsr.hThread, SIGRT0);	/* ISO ISR Thread */

            break;
        }
        #endif // EPS_ISR_MODE_SOC1

		default:
		{
			result = EPS_PNDEV_RET_UNSUPPORTED;
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_enable_interrupt(): unsupported pInterrupt=%u", (*pInterrupt));
			break;
		}
	}

    if (result == EPS_PNDEV_RET_OK)
    {
        pBoard->uCountIsrEnabled++;
    }

	eps_pn_soc1_exit();

	return result;
}

/**
 * Disabling interrupts for SOC1
 * 
 * @see eps_disable_pnio_event      - calls this function
 * @see eps_disable_iso_interrupt   - calls this function
 * 
 * @param [in] pHwInstIn            - pointer to hardware instance
 * @param [in] pInterrupt           - type of interrupt can be:
 *                                      - #EPS_PNDEV_ISR_PN_GATHERED
 *                                      - #EPS_PNDEV_ISR_PN_NRT
 *                                      - #EPS_PNDEV_ISR_ISOCHRONOUS
 * @return #EPS_PNDEV_RET_UNSUPPORTED - interrupt type not supported
 * @return #EPS_PNDEV_RET_ERR         - an error occured
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_soc1_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	LSA_UINT16                      result = EPS_PNDEV_RET_OK;	
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);	

	pBoard = (EPS_PN_SOC1_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	eps_pn_soc1_enter();

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_soc1_disable_interrupt(): pInterrupt=%u", (*pInterrupt));

	switch (*pInterrupt)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
        {
			if(pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

            #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
        	int retVal;
			retVal = ioctl(pBoard->PnFd, LINUX_IRTE_DRV_IOCTL_GATH_IRQ_ICU_DISABLE, NULL);
			EPS_ASSERT(retVal == 0);
            #else
            #if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_USERMODE)
                int retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_DISABLE, &pBoard->PnGatheredIsr.PnDrvArgs.evnt);
                EPS_ASSERT(retVal == 0);
			    #endif
			#endif
			
			#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
			result = tgroup_thread_remove((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), pBoard->PnGatheredIsr.hThread);
			EPS_ASSERT(result==0);
			#endif

			result = eps_pn_soc1_kill_pn_gathered_isr_thread(pBoard);

            #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
            //cleanup event filedescriptor
            if (pBoard->evtfd != -1)
            {
                close(pBoard->evtfd);
                pBoard->evtfd = -1;
            }
            #endif


			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
        }

		case EPS_PNDEV_ISR_INTERRUPT:
        {            
            #if (EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
            int retVal;
            retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_IRQ_ICU_DISABLE_KERNEL, 0);
            EPS_ASSERT(retVal == 0);
            #else
            result = EPS_PNDEV_RET_UNSUPPORTED;
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_disable_interrupt(): not supported with configured EPS_ISR_MODE");
            #endif

            break;
        }

		case EPS_PNDEV_ISR_POLLINTERRUPT:
        {            
            EPS_FATAL("eps_pn_soc1_disable_interrupt(): disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented");            
            break;
        }

		case EPS_PNDEV_ISR_CPU:
        {            
            EPS_FATAL("eps_pn_soc1_disable_interrupt(): disabling EPS_PNDEV_ISR_CPU is not implemented");            
            break;
        }

        #if (EPS_ISR_MODE_SOC1 != EPS_ISR_MODE_POLL)
		case EPS_PNDEV_ISR_ISOCHRONOUS:
        {
            int         retVal;
            #ifdef EPS_STATISTICS_ISO_TRANSFEREND                    
            LSA_UINT32  retVal_stats;
            #endif

			if (pBoard->sIsrIsochronous.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_disable_interrupt(): sIsrIsochronous.pCbf is NULL");
				break;
			}

			#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
			retVal = ioctl(pBoard->PnFd, LINUX_IRTE_DRV_IOCTL_ISOCHRONOUS_IRQ_ICU_DISABLE, NULL);
			EPS_ASSERT(retVal == 0);
			#else
            retVal = ioctl(pBoard->PnFd, EPS_PN_DRV_ISOCHRONOUS_IRQ_ICU_DISABLE, &pBoard->IsochronousIsr.PnDrvArgs.evnt);
            EPS_ASSERT(retVal == 0);
			#endif
            
            #ifdef EPS_STATISTICS_ISO_TRANSFEREND
            EPS_ASSERT(stats_id_TransferEnd != EPS_STATS_ID_TRANSFEREND_INIT);
            eps_statistics_capture_end_value(stats_id_TransferEnd);
            retVal_stats = eps_statistics_free_stats(stats_id_TransferEnd);
            EPS_ASSERT(retVal_stats == LSA_RET_OK);
            stats_id_TransferEnd = EPS_STATS_ID_TRANSFEREND_INIT;
            #endif // EPS_STATISTICS_ISO_TRANSFEREND

			result = eps_pn_soc1_kill_iso_isr_thread(pBoard);

            #if(EPS_PLF == EPS_PLF_LINUX_SOC1)
            //cleanup event filedescriptor
            if (pBoard->evtfd_iso != -1)
            {
                close(pBoard->evtfd_iso);
                pBoard->evtfd_iso = -1;
            }
            #endif

			if (result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrIsochronous.pCbf = LSA_NULL;
			}
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_disable_interrupt(): call failed -> eps_pn_soc1_kill_iso_isr_thread()");
            }

            break;
        }
        #endif // EPS_ISR_MODE_SOC1

		default:
        {
            result = EPS_PNDEV_RET_UNSUPPORTED;
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_soc1_disable_interrupt(): unsupported pInterrupt=%u", (*pInterrupt));
            break;
        }
    }

    if (result == EPS_PNDEV_RET_OK)
    {
        pBoard->uCountIsrEnabled--;
    }

	eps_pn_soc1_exit();

	return result;
}

/**
 * Sets GPIO pins of the SOC1 Board. 
 * 
 * Not used currently.
 * 
 * @param [in] pHwInstIn - pointer to hardware instance
 * @param [in] gpio      - GPIO PIN
 *  possible values: 
 *    #EPS_PNDEV_DEBUG_GPIO_1
 *    #EPS_PNDEV_DEBUG_GPIO_2
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED  - GPIO typer not supported
 */
static LSA_UINT16  eps_pn_soc1_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    
	/* ToDo */
	return EPS_PNDEV_RET_OK;
}

/**
 * Sets GPIO pins of the SOC1 Board.
 * 
 * Not used currently.
 * 
 * @param [in] pHwInstIn - pointer to hardware instance
 * @param [in] gpio      - GPIO PIN
 *  possible values: 
 *    #EPS_PNDEV_DEBUG_GPIO_1
 *    #EPS_PNDEV_DEBUG_GPIO_2
 *
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED  - GPIO typer not supported
 */
static LSA_UINT16  eps_pn_soc1_clear_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    
	/* ToDo */
	return EPS_PNDEV_RET_OK;
}

/**
 * Starts timer control. This is used for the ext timer mode of the EDDI.
 * 
 * @see eps_eddi_exttimer_start - calls this function
 *  
 * @param [in] pHwInstIn        - pointer to hardware instance 
 * @param [in] pCbf             - callback function to call
 * 
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_soc1_timer_ctrl_start (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard  = LSA_NULL;
	LSA_UINT16                      result  = EPS_PNDEV_RET_OK;
    
    #if(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
	LSA_UINT32                      ret_val = 0;
    #endif

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pCbf != LSA_NULL);

	eps_pn_soc1_enter();

	pBoard = (EPS_PN_SOC1_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(!pBoard->TimerCtrl.bRunning);

	pBoard->TimerCtrl.sTimerCbf = *pCbf;

    #if(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_USERMODE)
	if(pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0)
	{
		pBoard->TimerCtrl.hThread=eps_tasks_start_us_poll_thread("EPS_SOC1EXTTMR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, pCbf->pCbf, pCbf->uParam, pCbf->pArgs, eRUN_ON_1ST_CORE );
		EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);

		#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
		eps_tasks_group_thread_add ( pBoard->TimerCtrl.hThread, EPS_TGROUP_NRT );
		#endif
	}
    #elif(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
	
	EPS_ASSERT(pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0);	
	pCbf->lSII_ExtTimerInterval_us = pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval;
	
	// activate the kernel mode timer
	ret_val = ioctl(pBoard->PnFd, EPS_PN_DRV_IRQ_START_EXTTIMER, pCbf);
	EPS_ASSERT ( ret_val == 0 );
	
    #else
	EPS_FATAL("eps_pn_soc1_timer_ctrl_start() - unsupported EPS_ISR_MODE");
    #endif

	pBoard->TimerCtrl.bRunning = LSA_TRUE;

	eps_pn_soc1_exit();

	return result;
}

/**
 * Stop timer control. This is used for the ext timer mode of the EDDI.
 * 
 * @see eps_eddi_exttimer_stop - calls this function
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16  eps_pn_soc1_timer_ctrl_stop (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	EPS_PN_SOC1_DRV_BOARD_PTR_TYPE  pBoard  = LSA_NULL;
	LSA_UINT16                      result  = EPS_PNDEV_RET_OK;
	
    #if(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
	LSA_UINT32                      ret_val = 0;
    #endif

	pBoard = (EPS_PN_SOC1_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning);

	eps_pn_soc1_enter();

	EPS_ASSERT(pBoard->bUsed);
	
    #if(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_USERMODE)

	#if(EPS_PLF != EPS_PLF_LINUX_SOC1)
	eps_tasks_group_thread_remove(pBoard->TimerCtrl.hThread);
	#endif
	
	eps_tasks_stop_poll_thread(pBoard->TimerCtrl.hThread);
	
    #elif(EPS_ISR_MODE_SOC1 == EPS_ISR_MODE_IR_KERNELMODE)
	
	// stop the kernel mode timer
	ret_val = ioctl(pBoard->PnFd, EPS_PN_DRV_IRQ_STOP_EXTTIMER, 0);
	EPS_ASSERT ( ret_val == 0 );
	
    #else
    EPS_FATAL("eps_pn_soc1_timer_ctrl_stop() - unsupported EPS_ISR_MODE");
    #endif
	
	pBoard->TimerCtrl.bRunning = LSA_FALSE;	

	eps_pn_soc1_exit();

	return result;
}

/**
 * read SOC1 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_READ_LOWER_CPU       - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written to (destination)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_soc1_read_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
    EPS_FATAL("eps_pn_soc1_read_trace_data(): for SOC1 not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * write SOC1 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_WRITE_LOWER_CPU      - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written (source)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_soc1_write_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("eps_pn_soc1_write_trace_data(): for SOC1 not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_soc1_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(lowerCpuId);
	EPS_FATAL("eps_pn_soc1_write_sync_time_lower(): for SOC1 not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * save dump file in fatal
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16  eps_pn_soc1_save_dump (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Enable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_soc1_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("eps_pn_soc1_enable_hw_interrupt(): in SOC1 not supported in this driver"); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_soc1_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("eps_pn_soc1_disable_hw_interrupt(): in SOC1 not supported in this driver"); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_soc1_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("eps_pn_soc1_read_hw_interrupt(): in SOC1 not supported in this driver"); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

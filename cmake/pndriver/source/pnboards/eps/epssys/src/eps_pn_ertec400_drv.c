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
/*  F i l e               &F: eps_pn_ertec400_drv.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN ERTEC400 Driver Interface Adaption                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20034
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>              /* Types / Prototypes / Fucns               */
#include <eps_trc.h>			  /* Tracing                                  */
#include <eps_rtos.h>			  /* OS for Thread Api                        */
#include <eps_tasks.h>            /* EPS TASK API                             */
#include <eps_locks.h>            /* EPS Locks                                */
#include <eps_mem.h>              /* EPS local memory                         */
#include <eps_cp_hw.h>            /* EPS CP PSI adaption                      */
#include <eps_hw_ertec400.h>      /* EPS HW Ertec200 adaption                 */
#include <eps_pn_drv_if.h>		  /* PN Device Driver Interface               */
#include <eps_pn_ertec400_drv.h>  /* PNDEV-Driver Interface implementation    */
#include <eps_shm_if.h>           /* Shared Memory Interface                  */
#include <eps_pncore.h>           /* PnCore Driver Include                    */
#include <eps_app.h>              /* EPS Application Api                      */
#include <eps_register.h>         /* Register access macros                   */
#include <eps_plf.h>              /* g_EpsPlfAdonis declaration               */
#include <eps_adonis_isr_drv.h>
#include <irte_drv.h>
#include <eps_pndrvif.h>
/* EDD / HW Includes */
#include <edd_inc.h>

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_ERTEC_TIMERTOP_1TICK_NS    40UL  /* prescaler is set that each tick is 40ns (25MHz) */
#define EPS_ERTEC_TIMER_NR              0

#define EPS_ERTEC400PCIE_VENDORDEVICEID             0x408C110A
#define EPS_ERTEC400PCIE_VENDORDEVICEID_REGISTER    0x80000040

#define EPS_PN_ERTEC400_COUNT_MAC_IF      1
#define EPS_PN_ERTEC400_COUNT_MAC_PORTS   4

extern EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw;
/* - Defines from Linker File --------------------------------------------------------------------- */
extern LSA_UINT32 __IRTE_START__;
extern LSA_UINT32 __IRTE_END__;
extern LSA_UINT32 __NRT_RAM_START__;
extern LSA_UINT32 __NRT_RAM_END__;
extern LSA_UINT32 __SHARED_MEM_START__;
extern LSA_UINT32 __SHARED_MEM_END__;
extern LSA_UINT32 __PNCORE_SHM_START__;

/* - Function Forward Declaration ----------------------------------------------------------------- */


static LSA_VOID   *eps_pn_ertec400_isr_thread               (void *arg);
static LSA_UINT16  eps_pn_ertec400_enable_interrupt         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_ertec400_disable_interrupt        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_pn_ertec400_set_gpio                 (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_ertec400_clear_gpio               (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_ertec400_timer_ctrl_start         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_ertec400_timer_ctrl_stop          (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec400_read_trace_data		    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pn_ertec400_write_trace_data	        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pn_ertec400_write_sync_time_lower    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);
static LSA_UINT16  eps_pn_ertec400_save_dump                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec400_enable_hw_interrupt      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec400_disable_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec400_read_hw_interrupt        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);

static LSA_UINT16  eps_pn_ertec400_open                     (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16  eps_pn_ertec400_close                    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_pn_ertec400_board_tag;

typedef struct eps_ertec400_isr_tag
{
	LSA_BOOL   bRunning;
	LSA_BOOL   bThreadAlive;
	pthread_t  hThread;
	void *     pStack;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc;
	struct eps_pn_ertec400_board_tag *pBoard;
	EPS_PN_DRV_ARGS_TYPE PnDrvArgs;
} EPS_PN_ERTEC400_DRV_ISR_TYPE, *EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE;

typedef struct eps_pn_ertec400_board_tag
{	
	LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;
	/* EPS_PNDEV_CALLBACK_TYPE sIsrIsochronous; */
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_SYS_TYPE sysDev;

	#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_KERNELMODE)
	int PnFd;
	#endif

	struct
	{
		EPS_PNDEV_CALLBACK_TYPE sTimerCbf;
		LSA_UINT32 hThread;
		LSA_BOOL bRunning;
	} TimerCtrl;
	EPS_PN_ERTEC400_DRV_ISR_TYPE PnGatheredIsr;
} EPS_PN_ERTEC400_DRV_BOARD_TYPE, *EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE;

typedef struct eps_pn_ertec400_store_tag
{
	LSA_BOOL 					   bInit;
	LSA_UINT16 					   hEnterExit;
	EPS_PN_ERTEC400_DRV_BOARD_TYPE board;
} EPS_PN_ERTEC400_DRV_STORE_TYPE, *EPS_PN_ERTEC400_DRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_PN_ERTEC400_DRV_STORE_TYPE g_EpsPnErtec400Drv;
static EPS_PN_ERTEC400_DRV_STORE_PTR_TYPE g_pEpsPnErtec400Drv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
static LSA_UINT16 eps_wait_pn_drv_event(EPS_PN_DRV_EVENT_PTR_TYPE pEvent)
{
	int info;
	sigwait(&pEvent->event, &info);

	return info;
}

static LSA_UINT16 eps_wait_timer_drv_event(EPS_TIMER_DRV_EVENT_PTR_TYPE pEvent)
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
static LSA_VOID eps_pn_eb400_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsPnErtec400Drv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec400_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsPnErtec400Drv->hEnterExit, LSA_FALSE);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec400_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsPnErtec400Drv->hEnterExit);
}

/**
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec400_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsPnErtec400Drv->hEnterExit);
}

/**
 * This function links the function pointer required by the EPS PN DEV IF to internal functions.
 *
 * This function is called by eps_pn_ertec400_open
 *
 * Note that this function may only be called once since only one HD runs on the ERTEC400 ARM9.
 * eps_pn_ertec400_free_board must be called before a new instance can be allocated.
 *
 * @see eps_pn_ertec400_open        - calls this function
 * @see eps_pn_dev_open             - calls eps_pn_ertec400_open which calls this function
 * @see eps_pn_ertec400_free_board  - corresponding free function
 *
 * @param LSA_VOID
 * @return Pointer to the board structure.
 */
static EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE eps_pn_ertec400_alloc_board(LSA_VOID)
{
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE const pBoard = &g_pEpsPnErtec400Drv->board;

	if //free board entry available?
	   (!pBoard->bUsed)
  	{
    	pBoard->sHw.hDevice			    = (LSA_VOID*)pBoard;
    	pBoard->sHw.EnableIsr		    = eps_pn_ertec400_enable_interrupt;
    	pBoard->sHw.DisableIsr		    = eps_pn_ertec400_disable_interrupt;
        pBoard->sHw.EnableHwIr          = eps_pn_ertec400_enable_hw_interrupt;
        pBoard->sHw.DisableHwIr         = eps_pn_ertec400_disable_hw_interrupt;
        pBoard->sHw.ReadHwIr            = eps_pn_ertec400_read_hw_interrupt;
    	pBoard->sHw.SetGpio			    = eps_pn_ertec400_set_gpio;
    	pBoard->sHw.ClearGpio		    = eps_pn_ertec400_clear_gpio;
    	pBoard->sHw.TimerCtrlStart	    = eps_pn_ertec400_timer_ctrl_start;
    	pBoard->sHw.TimerCtrlStop	    = eps_pn_ertec400_timer_ctrl_stop;
    	pBoard->sHw.ReadTraceData	    = eps_pn_ertec400_read_trace_data;
    	pBoard->sHw.WriteTraceData	    = eps_pn_ertec400_write_trace_data;
    	pBoard->sHw.WriteSyncTimeLower	= eps_pn_ertec400_write_sync_time_lower;
    	pBoard->sHw.SaveDump		    = eps_pn_ertec400_save_dump;
    	pBoard->bUsed           	    = LSA_TRUE;

		return pBoard;
	}	

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_ertec400_alloc_board: no free board entry available");

	return LSA_NULL;
}

/**
 * This function releases the board by setting bUsed to false. A new alloc can be called.
 *
 * @see eps_pn_ertec400_close        - calls this function
 * @see eps_pn_dev_close             - calls eps_pn_ertec400_close which calls this function
 * @see eps_pn_ertec400_alloc_board  - corresponding alloc function
 *
 * @param [in] pBoard                - pointer to hardware instance
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec400_free_board(EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

static LSA_UINT16 eps_pn_ertec400_register_board_for_isr(EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr, EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{	
	*bCreateThread = LSA_FALSE;

	if(pIsr->pBoard == pBoard)
	{
		return EPS_PNDEV_RET_ERR; /* Board interrupt already enabled */
	}
	
	if(pIsr->pBoard == LSA_NULL)
	{
		pIsr->pBoard  = pBoard;  /* Register Board for interrupt */
		pIsr->uIntSrc = uIntSrc;
	}

	if(pIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_ertec400_unregister_board_for_isr(EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr, EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	pIsr->pBoard = LSA_NULL;
	*bKillThread = LSA_TRUE;
	return EPS_PNDEV_RET_OK;
}

/**
 * interrupt thread for ERTEC400
 * runs as long as pIsr->bRunning is LSA_TRUE
 *
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_ertec400_isr_thread(void *arg)
{
	volatile EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr = (EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE)arg;
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard;
	int info;
	sigset_t init_done_event;

	#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
		LSA_UINT32 event_nr=0;
		EPS_PN_DRV_EVENT_PTR_TYPE drv_event = &pIsr->PnDrvArgs.evnt;
	#endif
		
	sigemptyset(&init_done_event);
	sigaddset(&init_done_event, SIGRT0);
		
	pIsr->bThreadAlive = LSA_TRUE;

	/* Wait until init done */
	sigwait(&init_done_event, &info);

	/* Enter polling loop */
	while (pIsr->bRunning == LSA_TRUE)
	{
		#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_POLL) || (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_KERNELMODE)
			eps_tasks_sleep(1);
		#else
			event_nr = eps_wait_pn_drv_event(drv_event);

			if (event_nr == 1)
			{
		#endif
		pBoard = pIsr->pBoard;

		if(pBoard != LSA_NULL)
		{
			switch(pIsr->uIntSrc)
			{
				case EPS_PNDEV_ISR_PN_GATHERED:
					if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
					}
					else
					{
            			EPS_FATAL("No callbackfunction set in ERTEC400 gathered interrupt");
					}
					break;
				default: EPS_FATAL("ERTEC400 interrupt: undefined interruptsource");
					break;
				}
			}
		#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
		}
		#endif	
	}

	pIsr->bThreadAlive = LSA_FALSE;

	return NULL;
}

static LSA_UINT16 eps_pn_ertec400_create_isr_thread(LSA_CHAR* cName, EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr, EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard)
{
	pthread_attr_t th_attr; // Attributes
	pthread_attr_t *p_th_attr = &th_attr;
	struct sched_param th_param; // Scheduling parameters
	int ret_val;

	/* Init thread attributes */
	ret_val = EPS_POSIX_PTHREAD_ATTR_INIT(p_th_attr);
	EPS_ASSERT(ret_val == 0);

	/* Enable explicitly to set individual scheduling parameters */
	EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED(p_th_attr, PTHREAD_EXPLICIT_SCHED);

	/* Set stack and stack size */
	pIsr->pStack = eps_mem_alloc( 64000, LSA_COMP_ID_PNBOARDS,  PSI_MTYPE_IGNORE );
	EPS_ASSERT(pIsr->pStack != LSA_NULL);

	EPS_POSIX_PTHREAD_ATTR_SETSTACK(p_th_attr, pIsr->pStack, 64000);

	/* Set priority */
	th_param.sched_priority = EPS_POSIX_THREAD_PRIORITY_HIGH;
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM(p_th_attr, &th_param);
	EPS_ASSERT(ret_val == 0);

	/* Set scheduling policy to FIFO */
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(p_th_attr, SCHED_FIFO);
	EPS_ASSERT(ret_val == 0);

	/* Set name */
	ret_val = pthread_attr_setname(p_th_attr, cName);
	EPS_ASSERT(ret_val == 0);

	pIsr->bRunning = LSA_TRUE;

	/* Create Thread */
	ret_val = EPS_POSIX_PTHREAD_CREATE(&pIsr->hThread, &th_attr, eps_pn_ertec400_isr_thread, (LSA_VOID*)pIsr);

	EPS_ASSERT(ret_val == 0);

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_ertec400_kill_isr_thread(LSA_CHAR* cName, EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr, EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_VOID* th_ret;
	LSA_UINT16 ret_val;

	LSA_UNUSED_ARG(cName);
	LSA_UNUSED_ARG(pBoard);

	pIsr->bRunning = LSA_FALSE;

	#if(EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
	EPS_POSIX_PTHREAD_KILL(pIsr->hThread, SIGRT1);
	#endif

	EPS_POSIX_PTHREAD_JOIN(pIsr->hThread, &th_ret); /* Wait until Thread is down */

    // free stack mem
    ret_val = eps_mem_free(pIsr->pStack, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
    EPS_ASSERT(ret_val == EPS_PNDEV_RET_OK);
    
	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_ertec400_create_pn_gathered_isr_thread(EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_ertec400_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread)
	{
		retVal = eps_pn_ertec400_create_isr_thread("EPS_EB400GATPOISR", pIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_pn_ertec400_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_pn_ertec400_kill_pn_gathered_isr_thread(EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PN_ERTEC400_DRV_ISR_PTR_TYPE pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_ertec400_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if(bKillThread)
	{
		eps_ertec400_kill_isr_thread("EPS_EB400GATPOISR", pIsr, pBoard);
	}

	return retVal;
}

/**
 * Undo initialization by setting bInit to FALSE.
 *
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsPnErtec400Drv->bInit == LSA_TRUE);

	eps_pn_eb400_undo_init_critical_section();

	g_pEpsPnErtec400Drv->bInit = LSA_FALSE;

	g_pEpsPnErtec400Drv = LSA_NULL;
}

/**
 * Installing ERTEC400 driver. The ERTEC400 PN Dev Driver implements the eps_pndev_if.
 *
 * The ERTEC400 driver requires the irte_drv for interrupt integration.
 * 
 * Note that this module is implemented to run on the LD / HD firmware that runs on the ARM9 core on the ERTEC400 with the adonis operating system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pn_ertec400_open
 *  eps_pndev_if_close     -> eps_pn_ertec400_close
 *  eps_pndev_if_uninstall -> eps_pn_ertec400_uninstall
 *
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * @see irte_drv_install      - Interrupt integration.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_ertec400_drv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnErtec400DrvIf;

	eps_memset(&g_EpsPnErtec400Drv, 0, sizeof(g_EpsPnErtec400Drv));
	g_pEpsPnErtec400Drv = &g_EpsPnErtec400Drv;

	g_pEpsPnErtec400Drv->bInit = LSA_TRUE;

	eps_pn_ertec400_init_critical_section();

	//Init Interface
	sPnErtec400DrvIf.open  = eps_pn_ertec400_open;
	sPnErtec400DrvIf.close = eps_pn_ertec400_close;
	sPnErtec400DrvIf.uninstall = eps_pn_eb200_uninstall;

	eps_pndev_if_register(&sPnErtec400DrvIf);

	#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_KERNELMODE)
		/* IRTE Driver Setup (TopLevel - ICU) */
		if(irte_drv_install() != EPS_PN_DRV_RET_OK)
		{
		  EPS_FATAL("Installing ERTEC400 driver failed");
		}
		/*if(timertop_drv_install() != EPS_TIMER_DRV_RET_OK)
		{
			EPS_FATAL(0);
		}*/
		g_pEpsPnErtec400Drv->board.PnFd = open(IRTE_DRV_NAME, O_RDWR);
		EPS_ASSERT(g_pEpsPnErtec400Drv->board.PnFd != -1);

		g_pEpsPnErtec400Drv->board.TimerCtrl.TimerFd = g_EpsPlfAdonis.fdEpsAdonisIsr;
		EPS_ASSERT(g_pEpsPnErtec400Drv->board.TimerCtrl.TimerFd >= 0);
		
	#endif
}

/**
 * open ERTEC400 board.
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
static LSA_UINT16 eps_pn_ertec400_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
    EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard;
    LSA_UINT16                         result = EPS_PNDEV_RET_OK;
    EPS_MAC_PTR_TYPE                   pMac = LSA_NULL;
    LSA_UINT16                         macIdx, portIdx;
    LSA_UINT32                         vendDevID = 0;
    
    EPS_PNCORE_MAC_ADDR_TYPE           MacAddressesCore;
    EPS_APP_MAC_ADDR_ARR               MacAddressesApp;
    LSA_UINT16                         loopCnt = 0;
    
    /*----------------------------------------------------------*/

    EPS_ASSERT(g_pEpsPnErtec400Drv->bInit == LSA_TRUE);
    EPS_ASSERT(pLocation   != LSA_NULL);
    EPS_ASSERT(ppHwInstOut != LSA_NULL);

    eps_pn_ertec400_enter();

    pBoard = eps_pn_ertec400_alloc_board();
    EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */
  
    eps_memset( &pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo) );

    // read Vendor and Device ID to differ between CP1616 and EB400PCIe
    vendDevID = EPS_REG32_READ(EPS_ERTEC400PCIE_VENDORDEVICEID_REGISTER);

    if (vendDevID != EPS_ERTEC400PCIE_VENDORDEVICEID) // Vendor and Device ID of EB400PCIe
        // not EB400PCIe
    {
	    pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_CP1616;
        pBoard->sHw.EpsBoardInfo.eddi.BC5221_MCModeSet = 1; //CP1616 has BC5521 PHY transceivers with MEDIA_CONV# pin strapped to 0
    }
    else
        // it is EB400PCIe
    {
	    pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB400_PCIE;
    }

    pBoard->sysDev.hd_nr        = hd_id;
    pBoard->sysDev.pnio_if_nr   = 0;	// don't care
    pBoard->sysDev.edd_comp_id  = LSA_COMP_ID_EDDI;

    pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
    pBoard->sHw.EpsBoardInfo.edd_type            = LSA_COMP_ID_EDDI;

    // Init User to HW port mapping (used PSI GET HD PARAMS)
    eps_hw_init_board_port_param( &pBoard->sHw.EpsBoardInfo );

    pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_400;

    // initialization of MediaType
    eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

    /*------------------------*/
    // IRTE settings
    pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)((LSA_UINT32)__IRTE_START__);
    pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = (LSA_UINT32)__IRTE_START__;
    pBoard->sHw.EpsBoardInfo.eddi.irte.size     = (LSA_UINT32)__IRTE_END__ - (LSA_UINT32)__IRTE_START__; /* irt size */

    // KRAM settings
	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr + 0x100000;
	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr + 0x100000;
	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = 0x30000;

    // SDRAM settings (for NRT)
    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)((LSA_UINT32)__NRT_RAM_START__);
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)__NRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = (LSA_UINT32)__NRT_RAM_END__ - (LSA_UINT32)__NRT_RAM_START__; /* nrt pool size */

    // Shared Mem settings
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr = (LSA_UINT32)__SHARED_MEM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size     = (LSA_UINT32)__SHARED_MEM_END__ - (LSA_UINT32)__SHARED_MEM_START__; /* shared memory size */

    // APB-Peripherals SCRB
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)0x40002600;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr = 0x40002600;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     = 0xA3;

	// APB-Peripherals TIMER
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr = (LSA_UINT8*)0x40002000;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr = 0x40002000;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size     = 0xFF;

    // APB-Peripherals GPIO
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr = (LSA_UINT8*)0x40002500;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr = 0x40002500;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size     = 0xFF;

    // Set default CycleBaseFactor
    pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

    // Prepare the board specific HW functions and GPIO numbers
    // Note used for PHY LED, PLL and LEDs
	pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll        = LSA_TRUE;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 25;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr  = 25;

    // Set POF, PHY-Led and PLL functions
    pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct   = eps_set_pllport_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct    = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct      = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct        = eps_pof_set_led_ERTEC400;

    // Set I2C functions
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz = eps_i2c_set_scl_low_highz_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz = eps_i2c_set_sda_low_highz_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read          = eps_i2c_sda_read_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select            = eps_i2c_select_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset    = eps_i2c_ll_read_offset_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset   = eps_i2c_ll_write_offset_ERTEC400;

	// SII settings (EDDI runs on EPC environment, ExtTimer exists)
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector      = EDDI_SII_IRQ_SP;
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber        = EDDI_SII_IRQ_0;

    #if defined ( EDDI_CFG_SII_EXTTIMER_MODE_ON )
	pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 1000UL;	//0, 250, 500, 1000 in us
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 0UL;    //0, 250, 500, 1000 in us
    #endif

    if (vendDevID != EPS_ERTEC400PCIE_VENDORDEVICEID) // Vendor and Device ID of EB400PCIe
        // not EB400PCIe
    {    
		// setup portmapping (=1:1)
		pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
		pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;
	
		pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
		pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;
	
		pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
		pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;
	
		pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 3;
		pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 4;
    }
    else
        // it is EB400PCIe
    {    
        // setup portmapping (= 1:4, 2:3, 3:2, 4:1)
        pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 3;
        pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 4;
    
        pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
        pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;
    
        pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 1;
        pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 2;
    
        pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 0;
        pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 1;
    }

    // setup DEV and NRT memory pools
    eps_ertec400_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

    // Prepare process image settings (KRAM is used)
    // Note: we use the full size of KRAM, real size is calculated in PSI
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddi.kram.size;

    // setup HIF buffer => HIF HD on same processor
	pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.size     = 0;
    
    // SRD API Memory is located in pncore shared memory, see eps_pncore.c
    pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr = g_pEpsPlfShmHw->sSrdApi.pBase;
    pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr = (LSA_UINT32)__PNCORE_SHM_START__ + g_pEpsPlfShmHw->sSrdApi.uOffset;
    pBoard->sHw.EpsBoardInfo.srd_api_mem.size     = g_pEpsPlfShmHw->sSrdApi.uSize;
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                        "eps_pn_ertec400_open(hd_id(%u)): srd_api_mem: base_ptr=0x%x phy_addr=0x%x size=0x%x",
                        hd_id,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.size);
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDI_GSHAREDMEM_TYPE));
		
	/*------------------------*/
	
    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array
    eps_memset(&MacAddressesCore, 0, sizeof(MacAddressesCore));
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));
    
    // Interface part
    // First we get the MAC address from the hardware
    eps_pncore_get_mac_addr(&MacAddressesCore);
    EPS_ASSERT(MacAddressesCore.lCtrMacAdr > 0);

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_ERTEC400_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC400_COUNT_MAC_IF + EPS_PN_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = MacAddressesCore.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = MacAddressesCore.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = MacAddressesCore.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = MacAddressesCore.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = MacAddressesCore.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = MacAddressesCore.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PN_ERTEC400_COUNT_MAC_IF + EPS_PN_ERTEC400_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC400_COUNT_MAC_IF + EPS_PN_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
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
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC400_COUNT_MAC_IF + EPS_PN_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
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
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_ERTEC400_COUNT_MAC_PORTS;

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PN_ERTEC400_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// Set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

	///*------------------------------------------------*/
	
	if (result == EPS_PNDEV_RET_OK)
	{
		*ppHwInstOut = &pBoard->sHw;

		eps_init_hw_ertec400(hd_id);
	}
	else
	{
		eps_pn_ertec400_free_board(pBoard);
	}

	eps_pn_ertec400_exit();

	return result;
}

/**
 * Close the ERTEC400 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance to close.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_ertec400_close( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	//EPS_PNDEV_INTERRUPT_DESC_TYPE    IsrDesc;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_ertec400_enter();

	pBoard = (EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	eps_undo_init_hw_ertec400(pBoard->sysDev.hd_nr);

	eps_pn_ertec400_free_board(pBoard);

	eps_pn_ertec400_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * Enabling interrupts for ERTEC400. This function implements the EPS PN DEV API function EnableIsr
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
static LSA_UINT16 eps_pn_ertec400_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	eps_pn_ertec400_enter();

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
      		///@todo IRT for ERTEC400 not implemented
      		EPS_FATAL("enabling ISR for isochronous mode not implemented yet for ERTEC400");
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			result = eps_pn_ertec400_create_pn_gathered_isr_thread(pBoard);

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			else
			{
				//tgroup_thread_add((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), pBoard->PnGatheredIsr.hThread);

    			#if (EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
                {
	                LSA_INT ret_val;

				    pBoard->PnGatheredIsr.PnDrvArgs.evnt.th_id = pBoard->PnGatheredIsr.hThread;

				    ret_val = ioctl(pBoard->PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_ENABLE, &pBoard->PnGatheredIsr.PnDrvArgs.evnt);
	                EPS_ASSERT(ret_val == 0);
                }
			#endif
			}

			EPS_POSIX_PTHREAD_KILL(pBoard->PnGatheredIsr.hThread, SIGRT0);	/* PN NRT ISR Thread */

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_INTERRUPT is not implemented in ertec400 drv");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in ertec400 drv");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("EPS_PNDEV_ISR_CPU is not implemented in ertec400 drv");            
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	eps_pn_ertec400_exit();

	return result;
}

/**
 * Disabling interrupts for ERTEC400
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
static LSA_UINT16 eps_pn_ertec400_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	eps_pn_ertec400_enter();

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
      		///@todo IRT for ERTEC200 not implemented
      		EPS_FATAL("disabling ISR for isochronous mode not implemented yet for ERTEC200");
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if(EPS_ISR_MODE_EB400 == EPS_ISR_MODE_IR_USERMODE)
            {
	            LSA_INT ret_val;

			    ret_val = ioctl(pBoard->PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_DISABLE, &pBoard->PnGatheredIsr.PnDrvArgs.evnt);
	            EPS_ASSERT(ret_val == 0);
            }
			#endif

			//result = tgroup_thread_remove((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), pBoard->PnGatheredIsr.hThread);
			//EPS_ASSERT(result==0);

			result = eps_pn_ertec400_kill_pn_gathered_isr_thread(pBoard);

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_INTERRUPT is not implemented in ertec400 drv");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in ertec400 drv");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("disabling EPS_PNDEV_ISR_CPU is not implemented in ertec400 drv");            
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	eps_pn_ertec400_exit();

	return result;
}

/**
 * Sets GPIO pins of the CP1616 / DB_ERTEC400_PCIe Board. 
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
static LSA_UINT16  eps_pn_ertec400_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	/* ToDo */
	return EPS_PNDEV_RET_OK;
}

/**
 * Sets GPIO pins of the CP1616 / DB_ERTEC400_PCIe Board. 
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
static LSA_UINT16  eps_pn_ertec400_clear_gpio( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio )
{
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
static LSA_UINT16 eps_pn_ertec400_timer_ctrl_start( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf )
{
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pCbf != LSA_NULL);

	eps_pn_ertec400_enter();

	pBoard = (EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

	pBoard->TimerCtrl.sTimerCbf = *pCbf;

	if(pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0)
	{
		pBoard->TimerCtrl.hThread=eps_tasks_start_us_poll_thread("EPS_EB400EXTTMR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, pCbf->pCbf, pCbf->uParam, pCbf->pArgs, eRUN_ON_1ST_CORE );
		EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);

		eps_tasks_group_thread_add ( pBoard->TimerCtrl.hThread, EPS_TGROUP_NRT );
	}

	pBoard->TimerCtrl.bRunning = LSA_TRUE;

	eps_pn_ertec400_exit();

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
static LSA_UINT16  eps_pn_ertec400_timer_ctrl_stop( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
{
	EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_PN_ERTEC400_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_TRUE);

	eps_pn_ertec400_enter();

	EPS_ASSERT(pBoard->bUsed);
	
	eps_tasks_stop_poll_thread(pBoard->TimerCtrl.hThread);
	
	pBoard->TimerCtrl.bRunning = LSA_FALSE;	

	eps_pn_ertec400_exit();

	return result;
}

/**
 * Read ERTEC400 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_READ_LOWER_CPU       - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written to (destination)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec400_read_trace_data( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8 * ptr, LSA_UINT32 size )
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(offset);
    LSA_UNUSED_ARG(ptr);
    LSA_UNUSED_ARG(size);
    EPS_FATAL("read trace data for ERTEC400 not supported");
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Write ERTEC400 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_WRITE_LOWER_CPU      - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written (source)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec400_write_trace_data( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size )
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(offset);
    LSA_UNUSED_ARG(ptr);
    LSA_UNUSED_ARG(size);
    EPS_FATAL("write trace data for ERTEC400 not supported");
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec400_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(lowerCpuId);
	EPS_FATAL("Writing lower sync time not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * save dump file in fatal
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16  eps_pn_ertec400_save_dump( EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn )
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
static LSA_UINT16  eps_pn_ertec400_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in ERTEC400 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_ertec400_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in ERTEC400 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_ertec400_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in ERTEC400 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}


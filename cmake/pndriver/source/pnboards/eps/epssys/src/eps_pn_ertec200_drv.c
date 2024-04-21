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
/*  F i l e               &F: eps_pn_ertec200_drv.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN ERTEC200 Driver Interface Adaption                                */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20033
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>              /* Types / Prototypes / Fucns               */ 
#include <eps_trc.h>              /* Tracing                                  */
#include <eps_rtos.h>			  /* OS for Thread Api                        */
#include <eps_tasks.h>            /* EPS TASK API                             */
#include <eps_locks.h>            /* EPS Locks                                */
#include <eps_cp_hw.h>            /* EPS CP PSI adaption                      */
#include <eps_hw_ertec200.h>      /* EPS HW Ertec200 adaption                 */
#include <eps_pn_drv_if.h>		  /* PN Device Driver Interface               */
#include <eps_pn_ertec200_drv.h>  /* Driver Interface implementation          */
#include <eps_shm_if.h>           /* Shared Memory Interface                  */
#include <eps_app.h>              /* EPS Application Api                      */
#include <eps_pncore.h>           /* PnCore Driver Include                    */
#include <eps_register.h>         /* Register access macros                   */
#include <irte_drv.h>
#include <eps_adonis_isr_drv.h>
#include <eps_pndrvif.h>

/* EDD / HW Includes */
#include <edd_inc.h>

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_ERTEC_TIMERTOP_1TICK_NS    40UL  /* prescaler is set that each tick is 40ns (25MHz) */
#define EPS_ERTEC_TIMER_NR              0

#define EPS_PN_ERTEC200_COUNT_MAC_IF    1
#define EPS_PN_ERTEC200_COUNT_MAC_PORTS 2

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

static LSA_VOID   *eps_pn_ertec200_isr_thread          (void *arg);
static LSA_UINT16  eps_pn_ertec200_enable_interrupt        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_ertec200_disable_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_pn_ertec200_set_gpio                (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_ertec200_clear_gpio              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_ertec200_timer_ctrl_start        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_ertec200_timer_ctrl_stop         (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec200_read_trace_data		    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pn_ertec200_write_trace_data        (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pn_ertec200_save_dump               (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec200_enable_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec200_disable_hw_interrupt    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_ertec200_read_hw_interrupt       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_pn_ertec200_write_sync_time_lower   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);

static LSA_UINT16 eps_pn_ertec200_open                     (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16 eps_pn_ertec200_close                    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_pn_ertec200_board_tag;

typedef struct eps_ertec200_isr_tag
{
	LSA_BOOL   bRunning;
	LSA_BOOL   bThreadAlive;
	pthread_t  hThread;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc;
	struct eps_pn_ertec200_board_tag *pBoard;
} EPS_PN_ERTEC200_DRV_ISR_TYPE, *EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE;

typedef struct eps_pn_ertec200_board_tag
{	
	LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE sIsrIsochronous;
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_SYS_TYPE sysDev;
  struct
  {
    EPS_PNDEV_CALLBACK_TYPE sTimerCbf;
		LSA_UINT32 hThread;
		LSA_BOOL bRunning;
  } TimerCtrl;

} EPS_PN_ERTEC200_DRV_BOARD_TYPE, *EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE;
								
typedef struct eps_pn_ertec200_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_PN_ERTEC200_DRV_BOARD_TYPE board;
	struct
	{
		EPS_PN_ERTEC200_DRV_ISR_TYPE PnGatheredIsr;
	} poll;
} EPS_PN_ERTEC200_DRV_STORE_TYPE, *EPS_PN_ERTEC200_DRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_PN_ERTEC200_DRV_STORE_TYPE g_EpsPnErtec200Drv;
static EPS_PN_ERTEC200_DRV_STORE_PTR_TYPE g_pEpsPnErtec200Drv = LSA_NULL;

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsPnErtec200Drv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec200_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsPnErtec200Drv->hEnterExit, LSA_FALSE);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec200_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsPnErtec200Drv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec200_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsPnErtec200Drv->hEnterExit);
}

/**
 * This function links the function pointer required by the EPS PN DEV IF to internal functions.
 * 
 * This function is called by eps_pn_ertec200_open
 * 
 * Note that this function may only be called once since only one HD runs on the ERTEC200 ARM9.
 * eps_pn_ertec200_free_board must be called before a new instance can be allocated.
 * 
 * @see eps_pn_ertec200_open        - calls this function
 * @see eps_pn_dev_open             - calls eps_pn_ertec200_open which calls this function
 * @see eps_pn_ertec200_free_board  - corresponding free function
 * 
 * @param LSA_VOID
 * @return Pointer to the board structure.
 */
static EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE eps_pn_ertec200_alloc_board(LSA_VOID)
{
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE const pBoard = &g_pEpsPnErtec200Drv->board;

 	if //free board entry available?
	   (!pBoard->bUsed)
	{
		pBoard->sHw.hDevice			    = (LSA_VOID*)pBoard;
		pBoard->sHw.EnableIsr		    = eps_pn_ertec200_enable_interrupt;
		pBoard->sHw.DisableIsr		    = eps_pn_ertec200_disable_interrupt;
		pBoard->sHw.EnableHwIr          = eps_pn_ertec200_enable_hw_interrupt;
        pBoard->sHw.DisableHwIr         = eps_pn_ertec200_disable_hw_interrupt;
        pBoard->sHw.ReadHwIr            = eps_pn_ertec200_read_hw_interrupt;
		pBoard->sHw.SetGpio			    = eps_pn_ertec200_set_gpio;
		pBoard->sHw.ClearGpio		    = eps_pn_ertec200_clear_gpio;
		pBoard->sHw.TimerCtrlStart	    = eps_pn_ertec200_timer_ctrl_start;
		pBoard->sHw.TimerCtrlStop	    = eps_pn_ertec200_timer_ctrl_stop;
		pBoard->sHw.ReadTraceData	    = eps_pn_ertec200_read_trace_data;
		pBoard->sHw.WriteTraceData	    = eps_pn_ertec200_write_trace_data;
		pBoard->sHw.WriteSyncTimeLower	= eps_pn_ertec200_write_sync_time_lower;
		pBoard->sHw.SaveDump		    = eps_pn_ertec200_save_dump;
		pBoard->bUsed				    = LSA_TRUE;

		return pBoard;
	}	

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_ertec200_alloc_board: no free board entry available");

	return LSA_NULL;
}

/**
 * This function releases the board by setting bUsed to false. A new alloc can be called.
 * 
 * @see eps_pn_ertec200_close        - calls this function
 * @see eps_pn_dev_close             - calls eps_pn_ertec200_close which calls this function
 * @see eps_pn_ertec200_alloc_board  - corresponding alloc function
 * 
 * @param [in] pBoard             - pointer to hardware instance
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_ertec200_free_board(EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

static LSA_UINT16 eps_pn_ertec200_register_board_for_isr(EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr, EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
{	
	*bCreateThread = LSA_FALSE;

	if(pPollIsr->pBoard == pBoard)
	{
		return EPS_PNDEV_RET_ERR; /* Board polling already enabled */
	}
	
	if(pPollIsr->pBoard == LSA_NULL)
	{
		pPollIsr->pBoard  = pBoard;  /* Register Board for polling */
		pPollIsr->uIntSrc = uIntSrc;		
	}

	if(pPollIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}
	 
static LSA_UINT16 eps_pn_ertec200_unregister_board_for_isr(EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pIsr, EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
		{
	pIsr->pBoard = LSA_NULL;
			*bKillThread = LSA_TRUE;
	return EPS_PNDEV_RET_OK;
		}

/**
 * interrupt thread for ERTEC200 
 * runs as long pIsr->bRunning is LSA_TRUE 
 * 
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_ertec200_isr_thread(void *arg)
{
	volatile EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr = (EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE)arg;
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard;

	pPollIsr->bThreadAlive = LSA_TRUE;

	/* Enter polling loop */
	while (pPollIsr->bRunning == LSA_TRUE)
	{
		pBoard = pPollIsr->pBoard;

		if(pBoard != LSA_NULL)
		{
			switch(pPollIsr->uIntSrc)
			{
				case EPS_PNDEV_ISR_PN_GATHERED:
					if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
					{
						pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
					}
					else
					{
					  EPS_FATAL("No callbackfunction set in ERTEC200 gathered interrupt");
					}
					break;
				default: EPS_FATAL("ERTEC400 interrupt: undefined interruptsource");
					break;
			}
		}
		
		eps_tasks_sleep(1);
	}

	pPollIsr->bThreadAlive = LSA_FALSE;

	return NULL;
}

static LSA_UINT16 eps_pn_ertec200_create_poll_thread(LSA_CHAR* cName, EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr, EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard)
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
	ret_val = EPS_POSIX_PTHREAD_ATTR_SETSTACKSIZE(p_th_attr, 51200); /* 50 KB Stacksize */
	EPS_ASSERT(ret_val == 0);

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

	pPollIsr->bRunning = LSA_TRUE;

	/* Create Thread */
	ret_val = EPS_POSIX_PTHREAD_CREATE(&pPollIsr->hThread, &th_attr, eps_pn_ertec200_isr_thread, (LSA_VOID*)pPollIsr);

	EPS_ASSERT(ret_val == 0);

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_ertec200_kill_poll_thread(LSA_CHAR* cName, EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr, EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_VOID* th_ret;

	pPollIsr->bRunning = LSA_FALSE;

	EPS_POSIX_PTHREAD_JOIN(pPollIsr->hThread ,&th_ret); /* Wait until Thread is down */

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_ertec200_create_pn_gathered_polling_thread(EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr = &g_pEpsPnErtec200Drv->poll.PnGatheredIsr;

	retVal = eps_pn_ertec200_register_board_for_isr(pPollIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_pn_ertec200_create_poll_thread("EPS_EB200GATPOISR", pPollIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_pn_ertec200_unregister_board_for_isr(pPollIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_pn_ertec200_kill_pn_gathered_polling_thread(EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PN_ERTEC200_DRV_ISR_PTR_TYPE pPollIsr = &g_pEpsPnErtec200Drv->poll.PnGatheredIsr;

	retVal = eps_pn_ertec200_unregister_board_for_isr(pPollIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_ertec200_kill_poll_thread("EPS_EB200GATPOISR", pPollIsr, pBoard);
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
	EPS_ASSERT(g_pEpsPnErtec200Drv->bInit == LSA_TRUE);

	eps_pn_eb200_undo_init_critical_section();

	g_pEpsPnErtec200Drv->bInit = LSA_FALSE;

	g_pEpsPnErtec200Drv = LSA_NULL;
}

/**
 * Installing ERTEC200 driver. The ERTEC200 PN Dev Driver implements the eps_pndev_if.
 * 
 * Note that this module is implemented to run on the LD / HD firmware that runs on the ARM9 core on the ERTEC200 with the adonis operating system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pn_ertec200_open
 *  eps_pndev_if_close     -> eps_pn_ertec200_close
 *  eps_pndev_if_uninstall -> eps_pn_ertec200_uninstall
 * 
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_ertec200_drv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnErtec200DrvIf;	

	eps_memset(&g_EpsPnErtec200Drv, 0, sizeof(g_EpsPnErtec200Drv));
	g_pEpsPnErtec200Drv = &g_EpsPnErtec200Drv;

	g_pEpsPnErtec200Drv->bInit = LSA_TRUE;

	eps_pn_ertec200_init_critical_section();	

	//Init Interface
	sPnErtec200DrvIf.open  = eps_pn_ertec200_open;
	sPnErtec200DrvIf.close = eps_pn_ertec200_close;
	sPnErtec200DrvIf.uninstall = eps_pn_eb200_uninstall;

	eps_pndev_if_register(&sPnErtec200DrvIf);
}

/**
 * open ERTEC200 board.
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
static LSA_UINT16 eps_pn_ertec200_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard;	
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
	EPS_MAC_PTR_TYPE pMac = LSA_NULL;
	LSA_UINT16 macIdx, portIdx;	
	
    EPS_PNCORE_MAC_ADDR_TYPE MacAddressesCore;
    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt = 0;
    
    /*----------------------------------------------------------*/

	EPS_ASSERT(g_pEpsPnErtec200Drv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_pn_ertec200_enter();

	pBoard = eps_pn_ertec200_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */
	
	eps_memset( &pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo) );
	
	pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200;
	
	pBoard->sysDev.hd_nr = hd_id;
	pBoard->sysDev.pnio_if_nr = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type            = LSA_COMP_ID_EDDI;
	pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_200;	

	// Init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param( &pBoard->sHw.EpsBoardInfo );

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200;			
	
	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	/*------------------------*/	
	// IRTE settings
#if defined (__ghs__)
    pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)((LSA_UINT32)&__IRTE_START__);
    pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = (LSA_UINT32)&__IRTE_START__;
    pBoard->sHw.EpsBoardInfo.eddi.irte.size     = (LSA_UINT32)&__IRTE_END__ - (LSA_UINT32)&__IRTE_START__; /* irt size */
#else
    pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)((LSA_UINT32)__IRTE_START__);
    pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = (LSA_UINT32)__IRTE_START__;
    pBoard->sHw.EpsBoardInfo.eddi.irte.size     = (LSA_UINT32)__IRTE_END__ - (LSA_UINT32)__IRTE_START__; /* irt size */
#endif

    // KRAM settings
	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr + 0x100000;
	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr + 0x100000;
	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = 0x10000; /* 64 kB */

    // SDRAM settings (for NRT)
#if defined (__ghs__)
	pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)((LSA_UINT32)&__NRT_RAM_START__);
	pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)&__NRT_RAM_START__;
	pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = (LSA_UINT32)&__NRT_RAM_END__ - (LSA_UINT32)&__NRT_RAM_START__; /* nrt pool size */
#else
    pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)((LSA_UINT32)__NRT_RAM_START__);
    pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)__NRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = (LSA_UINT32)__NRT_RAM_END__ - (LSA_UINT32)__NRT_RAM_START__; /* nrt pool size */
#endif
    
    // Shared Mem settings
#if defined (__ghs__)
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr = (LSA_UINT8*)((LSA_UINT32)&__SHARED_MEM_START__);
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr = (LSA_UINT32)&__SHARED_MEM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size     = (LSA_UINT32)&__SHARED_MEM_END__ - (LSA_UINT32)&__SHARED_MEM_START__; /* shared memory size */
#else
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr = (LSA_UINT8*)((LSA_UINT32)__SHARED_MEM_START__);
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr = (LSA_UINT32)__SHARED_MEM_START__;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size     = (LSA_UINT32)__SHARED_MEM_END__ - (LSA_UINT32)__SHARED_MEM_START__; /* shared memory size */
#endif
    // APB-Peripherals SCRB
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)0x40002600;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr = 0x40002600;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     = 0xA4; /* 164 Byte - EB200 Handbook */

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
    pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct   = eps_set_pllport_ERTEC200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct    = eps_phy_led_blink_begin_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct      = eps_phy_led_blink_end_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct = eps_phy_led_blink_set_mode_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct        = eps_pof_set_led_EB200;

    // Set I2C functions
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz = eps_i2c_set_scl_low_highz_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz = eps_i2c_set_sda_low_highz_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read          = eps_i2c_sda_read_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select            = eps_i2c_select_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset    = eps_i2c_ll_read_offset_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset   = eps_i2c_ll_write_offset_EB200;

	// setup portmapping (=1:1)
	pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
	pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 	pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
	pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;
	// SII settings (EDDI runs on EPC environment, ExtTimer exists)
    pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector      = EDDI_SII_IRQ_SP;
    pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber        = EDDI_SII_IRQ_0;

    #if defined ( EDDI_CFG_SII_EXTTIMER_MODE_ON )
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 1000UL;	//0, 250, 500, 1000 in us	
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval = 0UL;    //0, 250, 500, 1000 in us
    #endif

    // setup DEV and NRT memory pools
    eps_ertec200_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

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
#if defined (__ghs__)
    pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr = (LSA_UINT32)&__PNCORE_SHM_START__ + g_pEpsPlfShmHw->sSrdApi.uOffset;
#else
    pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr = (LSA_UINT32)__PNCORE_SHM_START__ + g_pEpsPlfShmHw->sSrdApi.uOffset;
#endif
    pBoard->sHw.EpsBoardInfo.srd_api_mem.size     = g_pEpsPlfShmHw->sSrdApi.uSize;
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                        "eps_pn_ertec200_open(hd_id(%u)): srd_api_mem: base_ptr=0x%x phy_addr=0x%x size=0x%x",
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
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_ERTEC200_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC200_COUNT_MAC_IF + EPS_PN_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = MacAddressesCore.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = MacAddressesCore.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = MacAddressesCore.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = MacAddressesCore.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = MacAddressesCore.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = MacAddressesCore.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PN_ERTEC200_COUNT_MAC_IF + EPS_PN_ERTEC200_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC200_COUNT_MAC_IF + EPS_PN_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
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
    for(loopCnt = 0; loopCnt < (EPS_PN_ERTEC200_COUNT_MAC_IF + EPS_PN_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
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
    pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[0];
    pBoard->sHw.EpsBoardInfo.if_mac = *pMac;       
    
    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_ERTEC200_COUNT_MAC_PORTS;

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PN_ERTEC200_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// Set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

	///*------------------------------------------------*/
	
	if(result == EPS_PNDEV_RET_OK)
	{
		*ppHwInstOut = &pBoard->sHw;

		eps_init_hw_ertec200(hd_id);
	}
	else
	{
		eps_pn_ertec200_free_board(pBoard);
	}

	eps_pn_ertec200_exit();

	return result;
}

/**
 * Close the ERTEC200 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance to close.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_ertec200_close (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{	
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_ertec200_enter();

	pBoard = (EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);	

	eps_undo_init_hw_ertec200(pBoard->sysDev.hd_nr);

	eps_pn_ertec200_free_board(pBoard);

	eps_pn_ertec200_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * Enabling interrupts for ERTEC200. This function implements the EPS PN DEV API function EnableIsr
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
LSA_UINT16 eps_pn_ertec200_enable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16 result = EPS_PNDEV_RET_OK;
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	eps_pn_ertec200_enter();	

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_ISOCHRONOUS:
		    ///@todo IRT for ERTEC200 not implemented
		    EPS_FATAL("enabling ISR for isochronous mode not implemented yet for ERTEC200");
			break;
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			result = eps_pn_ertec200_create_pn_gathered_polling_thread(pBoard);

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			else
			{
				tgroup_thread_add((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), g_pEpsPnErtec200Drv->poll.PnGatheredIsr.hThread);
			}

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_INTERRUPT is not implemented in ertec200 drv");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in ertec200 drv");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("EPS_PNDEV_ISR_CPU is not implemented in ertec200 drv");            
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	eps_pn_ertec200_exit();

	return result;
}

/**
 * Disabling interrupts for ERTEC200
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
static LSA_UINT16 eps_pn_ertec200_disable_interrupt (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;	

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);	

	pBoard = (EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	eps_pn_ertec200_enter();

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

			result = tgroup_thread_remove((*(tgroup_t*)eps_tasks_get_group_os_handle(EPS_TGROUP_USER)), g_pEpsPnErtec200Drv->poll.PnGatheredIsr.hThread);
			EPS_ASSERT(result==0);
			
			result = eps_pn_ertec200_kill_pn_gathered_polling_thread(pBoard);

			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
        case EPS_PNDEV_ISR_INTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_INTERRUPT is not implemented in ertec200 drv");            
            break;
        case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in ertec200 drv");            
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("disabling EPS_PNDEV_ISR_CPU is not implemented in ertec200 drv");            
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	eps_pn_ertec200_exit();

	return result;
}

/**
 * Sets GPIO pins of the EB200. 
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
static LSA_UINT16  eps_pn_ertec200_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	/* ToDo */
	return EPS_PNDEV_RET_OK;
}

/**
 * Resets GPIO pins of the EB200. 
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
static LSA_UINT16  eps_pn_ertec200_clear_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
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
static LSA_UINT16 eps_pn_ertec200_timer_ctrl_start (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pCbf != LSA_NULL);

	eps_pn_ertec200_enter();

	pBoard = (EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_FALSE);

	pBoard->TimerCtrl.sTimerCbf = *pCbf;

	if(pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval != 0)
	{
		pBoard->TimerCtrl.hThread=eps_tasks_start_us_poll_thread("EPS_EB200EXTTMR", EPS_POSIX_THREAD_PRIORITY_HIGH, eSchedFifo, pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval, pCbf->pCbf, pCbf->uParam, pCbf->pArgs, eRUN_ON_1ST_CORE );
		EPS_ASSERT(pBoard->TimerCtrl.hThread != 0);
		
		eps_tasks_group_thread_add ( pBoard->TimerCtrl.hThread, EPS_TGROUP_NRT );
	}

	pBoard->TimerCtrl.bRunning = LSA_TRUE;

	eps_pn_ertec200_exit();

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
static LSA_UINT16  eps_pn_ertec200_timer_ctrl_stop (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16 result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_PN_ERTEC200_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);
	EPS_ASSERT(pBoard->TimerCtrl.bRunning == LSA_TRUE);

	eps_pn_ertec200_enter();

	EPS_ASSERT(pBoard->bUsed);
	
	eps_tasks_stop_poll_thread(pBoard->TimerCtrl.hThread);
	
	pBoard->TimerCtrl.bRunning = LSA_FALSE;	

	eps_pn_ertec200_exit();

	return result;
}

/**
 * Read ERTEC200 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_READ_LOWER_CPU       - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written to (destination)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec200_read_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
    EPS_FATAL("read trace data for ERTEC200 not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Write ERTEC200 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_WRITE_LOWER_CPU      - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written (source)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec200_write_trace_data (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
    EPS_FATAL("write trace data for ERTEC200 not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}


/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_ertec200_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
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
static LSA_UINT16  eps_pn_ertec200_save_dump (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
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
static LSA_UINT16  eps_pn_ertec200_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in ERTEC200 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_ertec200_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in ERTEC200 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_ertec200_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in ERTEC200 not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

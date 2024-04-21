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
/*  F i l e               &F: eps_pn_eb200p_drv.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN EB200P Interface Adaption                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20032
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */

#include <eps_sys.h>			/* Types / Prototypes / Fucns               */ 
#include <eps_trc.h>			/* Tracing                                  */
#include <eps_rtos.h>			/* OS for Thread Api                        */
#include <eps_tasks.h>          /* EPS TASK API                             */
#include <eps_mem.h>            /* EPS Local Memory                         */
#include <eps_locks.h>          /* EPS Locks                                */
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>		/* PN Device Driver Interface               */
#include <eps_pn_eb200p_drv.h>  /* Eb200p-Driver Interface implementation   */
#include <eps_shm_if.h>         /* Shared Memory Interface                  */
#include <eps_pncore.h>         /* PnCore Driver Include                    */
#include <eps_pndrvif.h>        /* EPS Adonis Pnip Driver If                */
#include <pnip_drv.h>           /* EPS Adonis Pnip Driver                   */
#include <eps_register.h>       /* EPS Register access macros               */
#include <eps_hw_pnip.h>        /* EPS HW Ertec200p adaption                */
#include <eps_app.h>            /* EPS Application Api                      */

/* - Local Defines -------------------------------------------------------------------------------- */

#define EPS_PN_EB200P_COUNT_MAC_IF      1
#define EPS_PN_EB200P_COUNT_MAC_PORTS   2

extern EPS_SHM_HW_PTR_TYPE g_pEpsPlfShmHw;
/* - Defines from Linker File --------------------------------------------------------------------- */
extern LSA_UINT32 __NRT_CRT_RAM_START__;
extern LSA_UINT32 __NRT_CRT_RAM_END__;
extern LSA_UINT32 __PNCORE_SHM_START__;

/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_VOID   *eps_pn_eb200p_isr_thread            (void *arg);
static LSA_UINT16  eps_pn_eb200p_enable_interrupt      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_eb200p_disable_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_pn_eb200p_set_gpio              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_eb200p_clear_gpio            (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_eb200p_timer_ctrl_start      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_eb200p_timer_ctrl_stop       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_eb200p_read_trace_data       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pn_eb200p_write_trace_data	    (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pn_eb200p_save_dump             (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_eb200p_enable_hw_interrupt   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_eb200p_disable_hw_interrupt  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_eb200p_read_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_pn_eb200p_write_sync_time_lower (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);

static LSA_UINT16 eps_pn_eb200p_open                   (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16 eps_pn_eb200p_close                  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
/* - Typedefs ------------------------------------------------------------------------------------- */

typedef struct eps_pn_eb200p_board_tag
{
	LSA_BOOL bUsed;
	EPS_PNDEV_HW_TYPE sHw;
	LSA_UINT32 uCountIsrEnabled;	
	EPS_PNDEV_CALLBACK_TYPE sIsrIsochronous;
	EPS_PNDEV_CALLBACK_TYPE sIsrPnGathered;
	EPS_PNDEV_CALLBACK_TYPE sIsrPnNrt;
	EPS_SYS_TYPE sysDev;
} EPS_PN_EB200P_DRV_BOARD_TYPE, *EPS_PN_EB200P_DRV_BOARD_PTR_TYPE;
								
typedef struct eps_eb200p_isr_tag
{
	LSA_BOOL   bRunning;
	LSA_BOOL   bThreadAlive;
	LSA_UINT32 uCntUsers;
	pthread_t  hThread;
	void *     pStack;
	EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc;
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard;
	EPS_PN_DRV_ARGS_TYPE PnDrvArgs;
} EPS_PN_EB200P_DRV_ISR_TYPE, *EPS_PN_EB200P_DRV_ISR_PTR_TYPE;

typedef struct eps_pn_eb200p_store_tag
{
	LSA_BOOL bInit;
	LSA_UINT16 hEnterExit;
	EPS_PN_EB200P_DRV_BOARD_TYPE board;
	struct
	{
		EPS_PN_EB200P_DRV_ISR_TYPE PnGatheredIsr;
		EPS_PN_EB200P_DRV_ISR_TYPE PnNrtIsr;
		int PnFd;
	} isr;
} EPS_PN_EB200P_DRV_STORE_TYPE, *EPS_PN_EB200P_DRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
static EPS_PN_EB200P_DRV_STORE_TYPE     g_EpsPnEb200pDrv;
static EPS_PN_EB200P_DRV_STORE_PTR_TYPE g_pEpsPnEb200pDrv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
static LSA_UINT16 eps_wait_pn_drv_event(EPS_PN_DRV_EVENT_PTR_TYPE pEvent)
{
	int info;
	sigwait(&pEvent->event, &info);

	return info;
}
#endif	

/**
 * Get board variant of eb200p board
 * 
 * @param [in] uPnipBaseAddress - baseaddress of PNIP
 * @return #EPS_PNDEV_BOARD_EB200P
 * @return #EPS_PNDEV_BOARD_FPGA_ERTEC200P
 * @return #EPS_PNDEV_BOARD_EB200P on error
 */
static EPS_PNDEV_BOARD_TYPE eps_pn_eb200p_pnip_get_variant(LSA_UINT32 uPnipBaseAddress)
{
	/* Check Variant based on IP_Development Register 20:19 */
	switch((EPS_REG32_READ(uPnipBaseAddress + 4 /* IP_Development Register Offset = 4 */) & 0x180000) >> 19)
	{
		case 0x00: /* Target Asic */
			return EPS_PNDEV_BOARD_EB200P;
			break;
		case 0x01: /* Target FPGA */
			return EPS_PNDEV_BOARD_FPGA_ERTEC200P;
			break;
		default:
			EPS_FATAL("Unknown EB200P board type");
			break;
	}

	return EPS_PNDEV_BOARD_EB200P;
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_undo_init_critical_section(LSA_VOID)
{
	eps_free_critical_section(g_pEpsPnEb200pDrv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsPnEb200pDrv->hEnterExit, LSA_FALSE);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsPnEb200pDrv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsPnEb200pDrv->hEnterExit);
}

/**
 * This function links the function pointer required by the EPS PN DEV IF to internal functions.
 * 
 * This function is called by eps_pn_eb200p_open
 * 
 * Note that this function may only be called once since only one HD runs on the EB200P.
 * eps_pn_eb200p_free_board must be called before a new instance can be allocated.
 * 
 * @see eps_pn_eb200p_open        - calls this function
 * @see eps_pn_dev_open           - calls eps_pn_eb200p_open which calls this function
 * @see eps_pn_eb200p_free_board  - corresponding free function
 * 
 * @param LSA_VOID
 * @return Pointer to the board structure.
 */
static EPS_PN_EB200P_DRV_BOARD_PTR_TYPE eps_pn_eb200p_alloc_board(LSA_VOID)
{
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE const pBoard = &g_pEpsPnEb200pDrv->board;

	if //free board entry available?
	   (!pBoard->bUsed)
	{
		pBoard->sHw.hDevice             = (LSA_VOID*)pBoard;
		pBoard->sHw.EnableIsr           = eps_pn_eb200p_enable_interrupt;
		pBoard->sHw.DisableIsr          = eps_pn_eb200p_disable_interrupt;
		pBoard->sHw.EnableHwIr          = eps_pn_eb200p_enable_hw_interrupt;
		pBoard->sHw.DisableHwIr         = eps_pn_eb200p_disable_hw_interrupt;
		pBoard->sHw.ReadHwIr            = eps_pn_eb200p_read_hw_interrupt;
		pBoard->sHw.SetGpio             = eps_pn_eb200p_set_gpio;
		pBoard->sHw.ClearGpio           = eps_pn_eb200p_clear_gpio;
		pBoard->sHw.TimerCtrlStart      = eps_pn_eb200p_timer_ctrl_start;
		pBoard->sHw.TimerCtrlStop       = eps_pn_eb200p_timer_ctrl_stop;
		pBoard->sHw.ReadTraceData       = eps_pn_eb200p_read_trace_data;
		pBoard->sHw.WriteTraceData      = eps_pn_eb200p_write_trace_data;
		pBoard->sHw.WriteSyncTimeLower  = eps_pn_eb200p_write_sync_time_lower;
		pBoard->sHw.SaveDump	        = eps_pn_eb200p_save_dump;
		pBoard->bUsed                   = LSA_TRUE;

		return pBoard;
	}

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_eb200p_alloc_board(): no free board entry available");

	return LSA_NULL;
}

/**
 * This function releases the board by setting bUsed to false. A new alloc can be called.
 * 
 * @see eps_pn_eb200p_close       - calls this function
 * @see eps_pn_dev_close          - calls eps_pn_eb200p_close which calls this function
 * @see eps_pn_eb200p_alloc_board - corresponding alloc function
 * 
 * @param [in] pBoard             - pointer to hardware instance
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_free_board(EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

static LSA_UINT16 eps_pn_eb200p_register_board_for_isr(EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr, EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_INTERRUPT_DESC_TYPE uIntSrc, LSA_BOOL *bCreateThread)
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

	pIsr->uCntUsers++;

	if(pIsr->bRunning == LSA_FALSE)
	{
		/* Create Thread */
		*bCreateThread = LSA_TRUE;
	}

	return EPS_PNDEV_RET_OK;
}
	 
static LSA_UINT16 eps_pn_eb200p_unregister_board_for_isr(EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr, EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard, LSA_BOOL *bKillThread)
{
	*bKillThread = LSA_FALSE;

	if(pIsr->uCntUsers == 0)
	{
		return EPS_PNDEV_RET_ERR; /* ??? Somethings wrong! */
	}

	if(pIsr->pBoard == pBoard)
	{
		pIsr->pBoard = LSA_NULL;

		pIsr->uCntUsers--;
		if(pIsr->uCntUsers == 0)
		{
			*bKillThread = LSA_TRUE;
		}

		return EPS_PNDEV_RET_OK; /* Board polling already enabled */
	}
	
	return EPS_PNDEV_RET_ERR; /* Board not found */
}

/**
 * interrupt thread for EB200P
 * runs as long pIsr->bRunning is LSA_TRUE 
 *
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_eb200p_isr_thread(void *arg)
{
	volatile EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr = (EPS_PN_EB200P_DRV_ISR_PTR_TYPE)arg;
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE        pBoard;
	sigset_t                                init_done_event;
	int                                     info;
	#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
	LSA_UINT32                              event_nr = 0;
	#endif

	sigemptyset(&init_done_event);
	sigaddset(&init_done_event, SIGRT0);

	pIsr->bThreadAlive = LSA_TRUE;

	/* Wait until init done */	
	sigwait(&init_done_event, &info);

	/* Enter polling loop */
	while (pIsr->bRunning)
	{
		#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_POLL) || (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_KERNELMODE)
		eps_tasks_sleep(1);
		#else
		event_nr = eps_wait_pn_drv_event(&pIsr->PnDrvArgs.evnt);

		if(event_nr == 1)
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
							EPS_FATAL("No callbackfunction set in EB200P gathered interrupt");
						}
						break;
					case EPS_PNDEV_ISR_PN_NRT:
						if(pBoard->sIsrPnNrt.pCbf != LSA_NULL)
						{
							pBoard->sIsrPnNrt.pCbf(pBoard->sIsrPnNrt.uParam, pBoard->sIsrPnNrt.pArgs);
						}
						else
						{
							EPS_FATAL("No callbackfunction set in EB200P NRT interrupt");
						}
						break;
					default: EPS_FATAL("EB200P interrupt: undefined interruptsource");
						break;
				}
			}
		#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
	    }
	    else
        {
	        break;
        }
		#endif
	}

	pIsr->bThreadAlive = LSA_FALSE;

	return NULL;
}

static LSA_UINT16 eps_pn_eb200p_create_isr_thread(LSA_CHAR* cName, EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr, EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
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
	pIsr->pStack = eps_mem_alloc(64000, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
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
	ret_val = EPS_POSIX_PTHREAD_CREATE(&pIsr->hThread, &th_attr, eps_pn_eb200p_isr_thread, (LSA_VOID*)pIsr);

	EPS_ASSERT(ret_val == 0);

	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_eb200p_kill_isr_thread(LSA_CHAR* cName, EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr, EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_VOID* th_ret;
	LSA_UINT16 ret_val;

	LSA_UNUSED_ARG(cName);
	LSA_UNUSED_ARG(pBoard);

	pIsr->bRunning = LSA_FALSE;

	#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
	EPS_POSIX_PTHREAD_KILL(pIsr->hThread, SIGRT1);	/* exit PN Gathered Thread */
	#endif

	EPS_POSIX_PTHREAD_JOIN(pIsr->hThread, &th_ret); /* Wait until Thread is down */

    // free stack mem
    ret_val = eps_mem_free(pIsr->pStack, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
    EPS_ASSERT(ret_val == EPS_PNDEV_RET_OK);
    
	return EPS_PNDEV_RET_OK;
}

static LSA_UINT16 eps_pn_eb200p_create_pn_gathered_isr_thread(EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr = &g_pEpsPnEb200pDrv->isr.PnGatheredIsr;

	retVal = eps_pn_eb200p_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_pn_eb200p_create_isr_thread("EPS_E200P_PNGAT_ISR", pIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_pn_eb200p_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_pn_eb200p_kill_pn_gathered_isr_thread(EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr = &g_pEpsPnEb200pDrv->isr.PnGatheredIsr;

	retVal = eps_pn_eb200p_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_eb200p_kill_isr_thread("EPS_E200P_PNGAT_ISR", pIsr, pBoard);
	}

	return retVal;
}

static LSA_UINT16 eps_pn_eb200p_create_pn_nrt_isr_thread(EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bCreateThread;
	EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr = &g_pEpsPnEb200pDrv->isr.PnNrtIsr;

	retVal = eps_pn_eb200p_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_PN_NRT, &bCreateThread);

	if(bCreateThread == LSA_TRUE)
	{
		retVal = eps_pn_eb200p_create_isr_thread("EPS_E200P_PNNRT_ISR", pIsr, pBoard);
		if(retVal != EPS_PNDEV_RET_OK)
		{
			eps_pn_eb200p_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}

	return retVal;
}

static LSA_UINT16 eps_pn_eb200p_kill_pn_nrt_isr_thread(EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard)
{
	LSA_UINT16 retVal;
	LSA_BOOL bKillThread;
	EPS_PN_EB200P_DRV_ISR_PTR_TYPE pIsr = &g_pEpsPnEb200pDrv->isr.PnNrtIsr;

	retVal = eps_pn_eb200p_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if(bKillThread == LSA_TRUE)
	{
		eps_eb200p_kill_isr_thread("EPS_E200P_PNNRT_ISR", pIsr, pBoard);
	}

	return retVal;
}

/**
 * Undo initialization by setting bInit to FALSE.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_eb200p_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsPnEb200pDrv->bInit == LSA_TRUE);

	eps_pn_eb200p_undo_init_critical_section();

	g_pEpsPnEb200pDrv->bInit = LSA_FALSE;

	g_pEpsPnEb200pDrv = LSA_NULL;
}

/**
 * Installing EB200P driver. The EB200P PN Dev Driver implements the eps_pndev_if.
 * 
 * The EB200P driver requires the pnip_drv.
 * 
 * Note that this module is implemented to run on the LD / HD firmware that runs on the ARM9 core on the EB200P with the adonis operating system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pn_eb200p_open
 *  eps_pndev_if_close     -> eps_pn_eb200p_close
 *  eps_pndev_if_uninstall -> eps_pn_eb200p_uninstall
 * 
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * @see pnip_drv_install      - Interrupt integration.
 * 
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_eb200p_drv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE sPnEb200pDrvIf;	

	eps_memset(&g_EpsPnEb200pDrv, 0, sizeof(g_EpsPnEb200pDrv));
	g_pEpsPnEb200pDrv = &g_EpsPnEb200pDrv;

	g_pEpsPnEb200pDrv->bInit = LSA_TRUE;

	eps_pn_eb200p_init_critical_section();	

	//Init Interface
	sPnEb200pDrvIf.open      = eps_pn_eb200p_open;
	sPnEb200pDrvIf.close     = eps_pn_eb200p_close;
	sPnEb200pDrvIf.uninstall = eps_pn_eb200p_uninstall;

	// Register the PN Dev driver implementation to the EPS PN Dev IF
	eps_pndev_if_register(&sPnEb200pDrvIf);

	/* PN-IP Driver Setup (TopLevel - ICU) */
	if(pnip_drv_install() != EPS_PN_DRV_RET_OK)
	{
		EPS_FATAL("Installing EB200P driver failed");
	}

	g_pEpsPnEb200pDrv->isr.PnFd = open(PNIP_DRV_NAME, O_RDWR);
	EPS_ASSERT(g_pEpsPnEb200pDrv->isr.PnFd != -1);
}

/**
 * open EB200P board.
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
 * @return #EPS_PNDEV_RET_UNSUPPORTED - This is neither a EB200P with the ERTEC200P asic nor the FPGA Board with a supported bitstrom
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_eb200p_open(EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id)
{
	LSA_UINT16                       result  = EPS_PNDEV_RET_OK;
	LSA_UINT32                       nrtSize = 0;
	LSA_UINT32                       crtSize = 0;
	EPS_MAC_PTR_TYPE                 pMac;	
	LSA_UINT16                       macIdx, portIdx;
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard;

	EPS_PNCORE_MAC_ADDR_TYPE         MacAddressesCore;
	EPS_APP_MAC_ADDR_ARR             MacAddressesApp;
	LSA_UINT16                       loopCnt = 0;

	EPS_ASSERT(g_pEpsPnEb200pDrv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);

	eps_pn_eb200p_enter();

	pBoard = eps_pn_eb200p_alloc_board();
	EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200P;

	pBoard->sysDev.hd_nr       = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDP;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type      = LSA_COMP_ID_EDDP;

	// Init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200P;

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	// EDDP CRT App (Transfer End) settings
	pBoard->sHw.EpsBoardInfo.eddp.appl_timer_mode   = EDDP_APPL_TIMER_CFG_MODE_TRANSFER_END; // EDDP CRT APP Transfer End Interrupt not used

	pBoard->sHw.EpsBoardInfo.eddp.icu_location      = EDDP_LOCATION_LOCAL;

    // ERTEC200P only supports EDDP_HW_IF_A. pLocation->eInterfaceSelector is irrelevant.
    // EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
    pBoard->sHw.EpsBoardInfo.eddp.hw_interface      = EDDP_HW_IF_A;

	/* PNIP settings */
	pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr     = (LSA_UINT8*)0x10600000UL;
	pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr     = 0x10600000UL;
	pBoard->sHw.EpsBoardInfo.eddp.pnip.size         = 0x00200000UL;

    /* calculate memory ranges in SDRAM based on avaliable SDRAM */
    eps_hw_e200p_calculateSizeInSDRAM(((LSA_UINT32)__NRT_CRT_RAM_END__ - (LSA_UINT32)__NRT_CRT_RAM_START__)/*4MB*/, &nrtSize, &crtSize);

    /* SDRam settings for NRT */
    pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr = (LSA_UINT8*)((LSA_UINT32)__NRT_CRT_RAM_START__);
    pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr = (LSA_UINT32)__NRT_CRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size     = nrtSize;

    /* SDRam settings for CRT (used for ext. CRT memory pool) */
    pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr = (LSA_UINT8*)((LSA_UINT32)__NRT_CRT_RAM_START__) + nrtSize;
    pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr = (LSA_UINT32)__NRT_CRT_RAM_START__ + nrtSize;
    pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size     = crtSize;

	/* PERIF ram settings (used for EDDP PERIF setup) */
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr = (LSA_UINT8*)0x10800000UL;    // user adress mapped from AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.phy_addr = 0x10800000UL;                // physical adress to AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size     = 0x00010000UL;                // size of PERIF

	/* K32 ram settings (used for K32 DLL attach) */
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.base_ptr = (LSA_UINT8*)0x04000000UL;
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.phy_addr = 0x04000000UL;
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.size     = 0x00020000UL;

	/* APB-Peripherals SCRB */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.base_ptr = (LSA_UINT8*)0x4000F000UL;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.phy_addr = 0x4000F000UL;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.size     = 0x00000100UL;

	/* APB-Peripherals PERIF */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr = (LSA_UINT8*)0x40000000UL;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.phy_addr = 0x40000000UL;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.size     = 0x00008000UL;

	/* setup DEV and NRT memory pools */
    eps_eb200p_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

	/* Prepare process image settings (KRAM is used) */
	/* Note: we use the full size of KRAM, real size is calculated in PSI */
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;

	/* setup HIF buffer => HIF HD on same processor */
	pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.hif_mem.size     = 0;
	
	/* SRD API Memory is located in pncore shared memory, see eps_pncore.c */
    pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr = g_pEpsPlfShmHw->sSrdApi.pBase;
    pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr = (LSA_UINT32)__PNCORE_SHM_START__ + g_pEpsPlfShmHw->sSrdApi.uOffset;
    pBoard->sHw.EpsBoardInfo.srd_api_mem.size     = g_pEpsPlfShmHw->sSrdApi.uSize;
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH,
                        "eps_pn_eb200p_open(hd_id(%u)): srd_api_mem: base_ptr=0x%x phy_addr=0x%x size=0x%x",
                        hd_id,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr,
                        pBoard->sHw.EpsBoardInfo.srd_api_mem.size);
    EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDP_GSHAREDMEM_TYPE));

	/* Set POF LED function (board specific) */
	pBoard->sHw.EpsBoardInfo.eddp.pof_led_fct = LSA_NULL; /* ToDo! */

	switch (eps_pn_eb200p_pnip_get_variant((LSA_UINT32)pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr))
	{
		case EPS_PNDEV_BOARD_EB200P:
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200P;
			break;
		case EPS_PNDEV_BOARD_FPGA_ERTEC200P:
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_FPGA_ERTEC200P;
			break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	/* Board Type specific setup */
	switch (pBoard->sHw.EpsBoardInfo.board_type)
	{
		case EPS_PNDEV_BOARD_FPGA_ERTEC200P:
			pBoard->sHw.EpsBoardInfo.eddp.board_type                     = EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1;
			pBoard->sHw.EpsBoardInfo.eddp.hw_type                        = EDDP_HW_ERTEC200P_FPGA;
			pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio     = 4;        // CRT Transfer End 4:1 reduction ratio
			pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos = LSA_TRUE; //(LaM) ToDo CRT Transfer End Isr Offset Correction
			pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value  = 0;        //(LaM) ToDo CRT Transfer End Isr Offset Correction

			// setup portmapping (1:2, 2:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

 			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 1;
			break;
		case EPS_PNDEV_BOARD_EB200P:
			pBoard->sHw.EpsBoardInfo.eddp.board_type                     = EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1;
			pBoard->sHw.EpsBoardInfo.eddp.hw_type                        = EDDP_HW_ERTEC200P;
			pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio     = 1;        // CRT Transfer End 1:1 reduction ratio
			pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos = LSA_TRUE; //(LaM) ToDo CRT Transfer End Isr Offset Correction
			pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value  = 0;        //(LaM) ToDo CRT Transfer End Isr Offset Correction

			// setup portmapping (=1:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;
			break;
		default:
			EPS_FATAL("eps_pn_eb200p_open(): Undefinded EB200P board type when opening board.");
			break;
	}

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array
    eps_memset(&MacAddressesCore, 0, sizeof(MacAddressesCore));
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));
    
    // Interface part
    // First we get the MAC address from the hardware
    eps_pncore_get_mac_addr(&MacAddressesCore);
    EPS_ASSERT(MacAddressesCore.lCtrMacAdr > 0);

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_EB200P_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PN_EB200P_COUNT_MAC_IF + EPS_PN_EB200P_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = MacAddressesCore.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = MacAddressesCore.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = MacAddressesCore.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = MacAddressesCore.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = MacAddressesCore.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = MacAddressesCore.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PN_EB200P_COUNT_MAC_IF + EPS_PN_EB200P_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PN_EB200P_COUNT_MAC_IF + EPS_PN_EB200P_COUNT_MAC_PORTS); loopCnt++)
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
    for(loopCnt = 0; loopCnt < (EPS_PN_EB200P_COUNT_MAC_IF + EPS_PN_EB200P_COUNT_MAC_PORTS); loopCnt++)
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
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PN_EB200P_COUNT_MAC_PORTS;

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PN_EB200P_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// Set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddp.is_valid = LSA_TRUE;

	eps_hw_e200p_init(pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr);

	if (result == EPS_PNDEV_RET_OK)
	{
		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
		eps_pn_eb200p_free_board(pBoard);
	}

	eps_pn_eb200p_exit();

	return result;
}

/**
 * Close the EB200P board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param [in] pHwInstIn            - handle to instance to close.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_eb200p_close(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	eps_pn_eb200p_enter();

	pBoard = (EPS_PN_EB200P_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

	eps_pn_eb200p_free_board(pBoard);

	eps_pn_eb200p_exit();

	return EPS_PNDEV_RET_OK;
}

/**
 * Enabling interrupts for EB200P. This function implements the EPS PN DEV API function EnableIsr
 * 
 * @see eps_enable_pnio_event                       - calls this function
 * @see eps_enable_iso_interrupt                    - calls this function
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
static LSA_UINT16 eps_pn_eb200p_enable_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UINT16                       result = EPS_PNDEV_RET_OK;
    #if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE) || (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_KERNELMODE)
	LSA_UINT16                       ret_val;
    #endif
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	EPS_PN_DRV_IOCTL_ARG_TYPE        ioctl_args;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	pBoard = (EPS_PN_EB200P_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);

	eps_pn_eb200p_enter();	

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnGathered = *pCbf;

			result = eps_pn_eb200p_create_pn_gathered_isr_thread(pBoard);

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
			#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
			else
			{
				g_pEpsPnEb200pDrv->isr.PnGatheredIsr.PnDrvArgs.evnt.th_id = g_pEpsPnEb200pDrv->isr.PnGatheredIsr.hThread;

				ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_ENABLE, &g_pEpsPnEb200pDrv->isr.PnGatheredIsr.PnDrvArgs.evnt);
				EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);
			}
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				EPS_POSIX_PTHREAD_KILL(g_pEpsPnEb200pDrv->isr.PnGatheredIsr.hThread, SIGRT0);	/* PN NRT ISR Thread */
			}

			break;
		case EPS_PNDEV_ISR_PN_NRT:
			if(pBoard->sIsrPnNrt.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrPnNrt = *pCbf;

			result = eps_pn_eb200p_create_pn_nrt_isr_thread(pBoard);

			if(result != EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnNrt.pCbf = LSA_NULL;
			}
			#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
			else
			{
				g_pEpsPnEb200pDrv->isr.PnNrtIsr.PnDrvArgs.evnt.th_id = g_pEpsPnEb200pDrv->isr.PnNrtIsr.hThread;

				ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_NRT_IOC_IRQ_ICU_ENABLE, &g_pEpsPnEb200pDrv->isr.PnNrtIsr.PnDrvArgs.evnt);
				EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);
			}
			#endif

			if(result == EPS_PNDEV_RET_OK)
			{
				EPS_POSIX_PTHREAD_KILL(g_pEpsPnEb200pDrv->isr.PnNrtIsr.hThread, SIGRT0);	/* PN NRT ISR Thread */
			}

			break;

		case EPS_PNDEV_ISR_ISOCHRONOUS:
			if(pBoard->sIsrIsochronous.pCbf != LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			pBoard->sIsrIsochronous = *pCbf;

			eps_hw_e200p_enable_isochronous_interrupt(pHwInstIn->EpsBoardInfo.eddp.pnip.base_ptr, pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, LSA_TRUE);

			ioctl_args.crt_app_enable.func_ptr = (EPS_PN_DRV_CBF_TYPE)(pCbf->pCbf);
			ioctl_args.crt_app_enable.param    = pCbf->uParam;
			ioctl_args.crt_app_enable.args     = pCbf->pArgs;

			ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_CRT_APP_ENABLE, &ioctl_args);
			EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);

			break;
        case EPS_PNDEV_ISR_INTERRUPT:

            #if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_KERNELMODE)

            //IFA_PN_IRQ(0)
            pCbf->uParam = 0;
            ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_IRQ_ICU_ENABLE_KERNEL, pCbf);
            EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);

            //IFA_PN_IRQ(2)
            pCbf->uParam = 2;
            ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_IRQ_ICU_ENABLE_KERNEL, pCbf);
            EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);

            #endif

            break;
		case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in eb200p drv");
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("EPS_PNDEV_ISR_CPU is not implemented in eb200p drv");
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
			break;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled++;
	}

	eps_pn_eb200p_exit();

	return result;
}

/**
 * Disabling interrupts for EB200P
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
static LSA_UINT16 eps_pn_eb200p_disable_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt)
{
	LSA_UINT16                       ret_val;
	EPS_PN_EB200P_DRV_BOARD_PTR_TYPE pBoard = LSA_NULL;
	LSA_UINT16                       result = EPS_PNDEV_RET_OK;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);	

	pBoard = (EPS_PN_EB200P_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;
	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

	eps_pn_eb200p_enter();

	switch(*pInterrupt)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
			if(pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
			ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_GATH_IOC_IRQ_ICU_DISABLE, 0);
			EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);
			#endif

			result = eps_pn_eb200p_kill_pn_gathered_isr_thread(pBoard);			
			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}

			break;
		case EPS_PNDEV_ISR_PN_NRT:
			/* In PCI Mode only 1 real interrupt is available which is used for Isochrnous Mode */
			/* Kill isr threads                                                                 */
			if(pBoard->sIsrPnNrt.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			#if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_USERMODE)
			ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_NRT_IOC_IRQ_ICU_DISABLE, 0);
            EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);
			#endif

			result = eps_pn_eb200p_kill_pn_nrt_isr_thread(pBoard);		
			if(result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnNrt.pCbf = LSA_NULL;
			}

			break;
		case EPS_PNDEV_ISR_ISOCHRONOUS:
			if(pBoard->sIsrIsochronous.pCbf == LSA_NULL)
			{
				result = EPS_PNDEV_RET_ERR;
				break;
			}			

			ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_CRT_APP_DISABLE, 0);
            EPS_ASSERT(ret_val == EPS_PN_DRV_RET_OK);

			pBoard->sIsrIsochronous.pCbf = LSA_NULL;

			break;
        case EPS_PNDEV_ISR_INTERRUPT:

            #if (EPS_ISR_MODE_EB200P == EPS_ISR_MODE_IR_KERNELMODE)
            ret_val = ioctl(g_pEpsPnEb200pDrv->isr.PnFd, EPS_PN_DRV_IRQ_ICU_DISABLE_KERNEL, 0);
            EPS_ASSERT(ret_val == 0);
            #endif

            break;
		case EPS_PNDEV_ISR_POLLINTERRUPT:
            EPS_FATAL("disabling EPS_PNDEV_ISR_POLLINTERRUPT is not implemented in eb200p drv");
            break;
        case EPS_PNDEV_ISR_CPU:
            EPS_FATAL("disabling EPS_PNDEV_ISR_CPU is not implemented in eb200p drv");
            break;
		default:
			result = EPS_PNDEV_RET_UNSUPPORTED;
	}

	if(result == EPS_PNDEV_RET_OK)
	{
		pBoard->uCountIsrEnabled--;
	}

	eps_pn_eb200p_exit();

	return result;
}

/**
 * Sets GPIO pins of the EB200P. 
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
static LSA_UINT16  eps_pn_eb200p_set_gpio(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	switch(gpio)
	{
		case EPS_PNDEV_DEBUG_GPIO_1:
			eps_hw_e200p_set_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_1);
			break;
		case EPS_PNDEV_DEBUG_GPIO_2:
			eps_hw_e200p_set_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_2);
			break;
		default:
			return EPS_PNDEV_RET_UNSUPPORTED;
	}
	return EPS_PNDEV_RET_OK;
}

/**
 * Resets GPIO pins of the EB200P. 
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
static LSA_UINT16  eps_pn_eb200p_clear_gpio(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	switch(gpio)
	{
		case EPS_PNDEV_DEBUG_GPIO_1:
			eps_hw_e200p_clear_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_1);
			break;
		case EPS_PNDEV_DEBUG_GPIO_2:
			eps_hw_e200p_clear_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_2);
			break;
		default:
			return EPS_PNDEV_RET_UNSUPPORTED;
	}
	return EPS_PNDEV_RET_OK;
}

/**
 * Starts timer control     - NOT SUPPORTED IN THIS DRIVER
 *  
 * @param [in] pHwInstIn    - pointer to hardware instance 
 * @param [in] pCbf         - callback function to call
 * 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16  eps_pn_eb200p_timer_ctrl_start(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf)
{
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(pCbf);
	EPS_FATAL("Start timer control in EB200P not supported in this driver."); 

	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Stop timer control - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16  eps_pn_eb200p_timer_ctrl_stop(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
	LSA_UNUSED_ARG(pHwInstIn);
  EPS_FATAL("Stop timer control in EB200P not supported in this driver."); 

	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read eb200p trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_READ_LOWER_CPU       - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written to (destination)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_eb200p_read_trace_data(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("read trace data for eb200p not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * write eb200p trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_WRITE_LOWER_CPU      - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @param [in] offset              - offset inside the trace buffer for the data to write 
 * @param [in] ptr                 - pointer to data to be written (source)
 * @param [in] size                - size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_eb200p_write_trace_data(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(offset);
	LSA_UNUSED_ARG(ptr);
	LSA_UNUSED_ARG(size);
	EPS_FATAL("write trace data for eb200p not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn - handle to lower instance. Not used
 * @param [in] lowerCpuId - handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_eb200p_write_sync_time_lower(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId)
{	
	LSA_UNUSED_ARG(pHwInstIn);
	LSA_UNUSED_ARG(lowerCpuId);
	EPS_FATAL("write lower sync time not supported");
	return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * save dump file in fatal
 * 
 * @param [in] pHwInstIn           - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_eb200p_save_dump(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
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
static LSA_UINT16 eps_pn_eb200p_enable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Enable HW Interrupt in EB200P not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16 eps_pn_eb200p_disable_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("Disable HW Interrupt in EB200P not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn       - pointer to hardware instance 
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16 eps_pn_eb200p_read_hw_interrupt(EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("Read HW Interrupt in EB200P not supported in this driver."); 

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

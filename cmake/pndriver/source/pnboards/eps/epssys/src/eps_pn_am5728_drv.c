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
/*  F i l e               &F: eps_pn_am5728_drv.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PN AM5728 Interface Adaption                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20044
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>			/* Types / Prototypes / Funcs               */ 
#include <eps_trc.h>			/* Tracing                                  */
#include <eps_rtos.h>			/* OS for Thread Api                        */
#include <eps_tasks.h>          /* EPS TASK API                             */
#include <eps_mem.h>            /* EPS Local Memory                         */
#include <eps_locks.h>          /* EPS Locks                                */
#include <eps_cp_hw.h>          /* EPS CP PSI adaption                      */
#include <eps_pn_drv_if.h>		/* PN Device Driver Interface               */
#include <eps_pn_am5728_drv.h>  /* AM5728-Driver Interface implementation   */
#include <eps_hw_edds.h>        /* EDDS_LL                                  */
#include <eps_shm_if.h>         /* Shared Memory Interface                  */
#include <eps_pncore.h>         /* PnCore Driver Include                    */
#include <eps_app.h>            /* EPS Application Api                      */
#include <eps_pndrvif.h>
#include <am5728_drv.h>         /* AM5728-Driver in kernel-mode             */

/* - Local Defines -------------------------------------------------------------------------------- */
// from linker script
extern LSA_UINT32 __NRT_RAM_START__;
extern LSA_UINT32 __NRT_RAM_END__;

/* - Function Forward Declaration ----------------------------------------------------------------- */

static LSA_VOID   *eps_pn_am5728_isr_thread            (void *arg);
static LSA_UINT16  eps_pn_am5728_enable_interrupt      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_am5728_disable_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE pInterrupt);
static LSA_UINT16  eps_pn_am5728_enable_hw_interrupt   (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_am5728_disable_hw_interrupt  (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_am5728_read_hw_interrupt     (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32* interrupts);
static LSA_UINT16  eps_pn_am5728_set_gpio              (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_am5728_clear_gpio            (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio);
static LSA_UINT16  eps_pn_am5728_timer_ctrl_start      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_CALLBACK_PTR_TYPE pCbf);
static LSA_UINT16  eps_pn_am5728_timer_ctrl_stop       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);
static LSA_UINT16  eps_pn_am5728_read_trace_data       (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT32 offset, LSA_UINT8* ptr, LSA_UINT32 size);
static LSA_UINT16  eps_pn_am5728_write_trace_data      (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT8 const* ptr, LSA_UINT32 offset, LSA_UINT32 size);
static LSA_UINT16  eps_pn_am5728_write_sync_time_lower (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, LSA_UINT16 lowerCpuId);
static LSA_UINT16  eps_pn_am5728_save_dump             (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);

static LSA_UINT16  eps_pn_am5728_open                  (EPS_PNDEV_LOCATION_CONST_PTR_TYPE pLocation, EPS_PNDEV_OPEN_OPTION_PTR_TYPE pOption, EPS_PNDEV_HW_PTR_PTR_TYPE ppHwInstOut, LSA_UINT16 hd_id);
static LSA_UINT16  eps_pn_am5728_close                 (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn);

/* - Typedefs ------------------------------------------------------------------------------------- */

struct eps_pn_am5728_board_tag;

typedef struct eps_am5728_isr_tag
{
    LSA_BOOL                            bRunning;
    LSA_BOOL                            bThreadAlive;
    pthread_t                           hThread;
    void *                              pStack;
    EPS_PNDEV_INTERRUPT_DESC_TYPE       uIntSrc;
    struct eps_pn_am5728_board_tag    * pBoard;
} EPS_PN_AM5728_DRV_ISR_TYPE, *EPS_PN_AM5728_DRV_ISR_PTR_TYPE;

typedef struct eps_pn_am5728_board_tag
{
    LSA_BOOL                    bUsed;
	EPS_PNDEV_HW_TYPE           sHw;
    LSA_UINT32                  uCountIsrEnabled;    
    EPS_PNDEV_CALLBACK_TYPE     sIsrPnGathered;
    EPS_SYS_TYPE                sysDev;
    int                         PnFd;
    EPS_PN_AM5728_DRV_ISR_TYPE  PnGatheredIsr;
	EPS_EDDS_TI_LL_TYPE         uEddsTI;
} EPS_PN_AM5728_DRV_BOARD_TYPE, *EPS_PN_AM5728_DRV_BOARD_PTR_TYPE;

typedef struct eps_pn_am5728_store_tag
{
    LSA_BOOL                        bInit;
    LSA_UINT16                      hEnterExit;
	EPS_PN_AM5728_DRV_BOARD_TYPE    board;
} EPS_PN_AM5728_DRV_STORE_TYPE, *EPS_PN_AM5728_DRV_STORE_PTR_TYPE;

/* - Global Data ---------------------------------------------------------------------------------- */
EPS_PN_AM5728_DRV_STORE_TYPE        g_EpsPnAm5728Drv;
EPS_PN_AM5728_DRV_STORE_PTR_TYPE    g_pEpsPnAm5728Drv = LSA_NULL;

/* - Source --------------------------------------------------------------------------------------- */

/** 
 * Lock implementation.
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_am5728_init_critical_section(LSA_VOID)
{
	eps_alloc_critical_section(&g_pEpsPnAm5728Drv->hEnterExit, LSA_FALSE);

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_am5728_init_critical_section(): hEnterExit=%u/0x%x", 
        g_pEpsPnAm5728Drv->hEnterExit, g_pEpsPnAm5728Drv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_am5728_undo_init_critical_section(LSA_VOID)
{
    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_am5728_undo_init_critical_section(): hEnterExit=%u/0x%x", 
        g_pEpsPnAm5728Drv->hEnterExit, g_pEpsPnAm5728Drv->hEnterExit);

    eps_free_critical_section(g_pEpsPnAm5728Drv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_am5728_enter(LSA_VOID)
{
	eps_enter_critical_section(g_pEpsPnAm5728Drv->hEnterExit);
}

/** 
 * Lock implementation.
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_am5728_exit(LSA_VOID)
{
	eps_exit_critical_section(g_pEpsPnAm5728Drv->hEnterExit);
}

/**
 * This function links the function pointer required by the EPS PN DEV IF to internal functions.
 * 
 * This function is called by eps_pn_am5728_open
 * 
 * Note that this function may only be called once since only one HD runs on the am5728.
 * eps_pn_am5728_free_board must be called before a new instance can be allocated.
 * 
 * @see eps_pn_am5728_open          - calls this function
 * @see eps_pn_dev_open             - calls eps_pn_am5728_open which calls this function
 * @see eps_pn_am5728_free_board    - corresponding free function
 * 
 * @param LSA_VOID
 * @return Pointer to the board structure.
 */
static EPS_PN_AM5728_DRV_BOARD_PTR_TYPE eps_pn_am5728_alloc_board(LSA_VOID)
{
	EPS_PN_AM5728_DRV_BOARD_PTR_TYPE const pBoard = &g_pEpsPnAm5728Drv->board;

    // is free board entry available ?
	if (!pBoard->bUsed)
	{
		pBoard->sHw.hDevice             = (LSA_VOID*)pBoard;
		pBoard->sHw.EnableIsr           = eps_pn_am5728_enable_interrupt;
		pBoard->sHw.DisableIsr          = eps_pn_am5728_disable_interrupt;
		pBoard->sHw.EnableHwIr          = eps_pn_am5728_enable_hw_interrupt;
		pBoard->sHw.DisableHwIr         = eps_pn_am5728_disable_hw_interrupt;
        pBoard->sHw.ReadHwIr            = eps_pn_am5728_read_hw_interrupt;
        pBoard->sHw.SetGpio             = eps_pn_am5728_set_gpio;
        pBoard->sHw.ClearGpio           = eps_pn_am5728_clear_gpio;
        pBoard->sHw.TimerCtrlStart      = eps_pn_am5728_timer_ctrl_start;
        pBoard->sHw.TimerCtrlStop       = eps_pn_am5728_timer_ctrl_stop;
        pBoard->sHw.ReadTraceData       = eps_pn_am5728_read_trace_data;
        pBoard->sHw.WriteTraceData      = eps_pn_am5728_write_trace_data;
        pBoard->sHw.WriteSyncTimeLower  = eps_pn_am5728_write_sync_time_lower;
        pBoard->sHw.SaveDump            = eps_pn_am5728_save_dump;
        pBoard->bUsed                   = LSA_TRUE;

		return pBoard;
	}

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_alloc_board(): no free board entry available");

	return LSA_NULL;
}

/**
 * This function releases the board by setting bUsed to false. A new alloc can be called.
 * 
 * @see eps_pn_am5728_close         - calls this function
 * @see eps_pn_dev_close            - calls eps_pn_am5728_close which calls this function
 * @see eps_pn_am5728_alloc_board   - corresponding alloc function
 * 
 * @param [in] pBoard   : pointer to hardware instance
 * @return LSA_VOID
 */
static LSA_VOID eps_pn_am5728_free_board(
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard)
{
	EPS_ASSERT(pBoard != LSA_NULL);

	EPS_ASSERT(pBoard->bUsed);
	pBoard->bUsed = LSA_FALSE;
}

/**
 * Register board for ISR
 * 
 * @param [in] pIsr             : pointer to eps_am5728_isr_tag
 * @param [in] pBoard           : pointer to eps_pn_am5728_board_tag
 * @param [in] uIntSrc          : interrupt source from EPS_PNDEV_INTERRUPT_DESC_TYPE
 * @param [in] bCreateThread    : address of bCreateThread
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_register_board_for_isr(
    EPS_PN_AM5728_DRV_ISR_PTR_TYPE      pIsr, 
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard, 
    EPS_PNDEV_INTERRUPT_DESC_TYPE       uIntSrc, 
    LSA_BOOL *                          bCreateThread)
{
	*bCreateThread = LSA_FALSE;

	if (pIsr->pBoard == pBoard)
	{
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_register_board_for_isr(): Board Isr already enabled");
		return EPS_PNDEV_RET_ERR; /* Board Isr already enabled */
	}
	
	if (pIsr->pBoard == LSA_NULL)
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

/**
 * Unregister board from ISR
 * 
 * @param [in] pIsr             : pointer to eps_am5728_isr_tag
 * @param [in] pBoard           : pointer to eps_pn_am5728_board_tag
 * @param [in] bKillThread      : address of bKillThread
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_unregister_board_for_isr(
    EPS_PN_AM5728_DRV_ISR_PTR_TYPE      pIsr, 
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard, 
    LSA_BOOL *                          bKillThread)
{
    LSA_UNUSED_ARG(pBoard);
    
    pIsr->pBoard = LSA_NULL;
    *bKillThread = LSA_TRUE;

	return EPS_PNDEV_RET_OK;
}

/**
 * Interrupt thread for AM5728 
 * runs as long pIsr->bRunning is LSA_TRUE 
 *
 * @param arg Argument of interrupt thread
 */
static LSA_VOID *eps_pn_am5728_isr_thread(void *arg)
{
    volatile EPS_PN_AM5728_DRV_ISR_PTR_TYPE     pIsr = (EPS_PN_AM5728_DRV_ISR_PTR_TYPE)arg;
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE            pBoard;
    int                                         info;
    sigset_t                                    init_done_event;

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
        #if (EPS_ISR_MODE_AM5728 == EPS_ISR_MODE_POLL)
        eps_tasks_sleep(1);

        switch (pIsr->uIntSrc)
        {
            case EPS_PNDEV_ISR_PN_GATHERED:
            {
                if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
                {
                    pBoard->sIsrPnGathered.pCbf(pBoard->sIsrPnGathered.uParam, pBoard->sIsrPnGathered.pArgs);
                }
                else
                {
                    EPS_FATAL("eps_pn_am5728_isr_thread(): no callbackfunction set in AM5728 interrupt");
                }

                break;
            }

            default:
            {
                EPS_FATAL("eps_pn_am5728_isr_thread(): undefined interrupt source");
                break;
            }
        }
        #else
        #error "eps_pn_am5728_isr_thread() is not supported with ISR_MODE = Interrupt"
        #endif
    }

    pIsr->bThreadAlive = LSA_FALSE;

    return NULL;
}

/**
 * Create ISR thread for AM5728 board
 * 
 * @param [in] cName        : thread name as char string
 * @param [in] pIsr         : pointer to eps_am5728_isr_tag
 * @param [in] pBoard       : pointer to eps_pn_am5728_board_tag
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_create_isr_thread(
    LSA_CHAR                          * cName, 
    EPS_PN_AM5728_DRV_ISR_PTR_TYPE      pIsr, 
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard)
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

	/* Set stack and stack size */
	pIsr->pStack = eps_mem_alloc( 64000, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
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
	ret_val = EPS_POSIX_PTHREAD_CREATE(&pIsr->hThread, &th_attr, eps_pn_am5728_isr_thread, (LSA_VOID*)pIsr);
	EPS_ASSERT(ret_val == 0);

	return EPS_PNDEV_RET_OK;
}

/**
 * Kill ISR thread for AM5728 board
 * 
 * @param [in] cName        : thread name as char string
 * @param [in] pIsr         : pointer to eps_am5728_isr_tag
 * @param [in] pBoard       : pointer to eps_pn_am5728_board_tag
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_kill_isr_thread(
    LSA_CHAR                          * cName, 
    EPS_PN_AM5728_DRV_ISR_PTR_TYPE      pIsr, 
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard)
{
    LSA_VOID  * th_ret;
    LSA_UINT16  ret_val;

    LSA_UNUSED_ARG(cName);
    LSA_UNUSED_ARG(pBoard);

    pIsr->bRunning = LSA_FALSE;

    #if (EPS_ISR_MODE_AM5728 != EPS_ISR_MODE_POLL)
    EPS_POSIX_PTHREAD_KILL(pIsr->hThread, SIGRT1);
    #endif

    EPS_POSIX_PTHREAD_JOIN(pIsr->hThread, &th_ret); /* Wait until Thread is down */

    // free stack mem
    ret_val = eps_mem_free(pIsr->pStack, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE);
    EPS_ASSERT(ret_val == EPS_PNDEV_RET_OK);
    pIsr->pStack = LSA_NULL;

    return EPS_PNDEV_RET_OK;
}

/**
 * Create ISR_PN_GATHERED for AM5728 board
 * 
 * @param [in] pBoard       : pointer to eps_pn_am5728_board_tag
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_create_pn_gathered_isr_thread(
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bCreateThread;
	EPS_PN_AM5728_DRV_ISR_PTR_TYPE  pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_am5728_register_board_for_isr(pIsr, pBoard, EPS_PNDEV_ISR_PN_GATHERED, &bCreateThread);

	if (bCreateThread)
	{
		retVal = eps_pn_am5728_create_isr_thread("EPS_AM5728_ISR", pIsr, pBoard);

		if (retVal != EPS_PNDEV_RET_OK)
		{
            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_create_pn_gathered_isr_thread(): call failed -> eps_pn_am5728_create_isr_thread()");
            eps_pn_am5728_unregister_board_for_isr(pIsr, pBoard, &bCreateThread);
		}
	}
	else
	{
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_create_pn_gathered_isr_thread(): call failed -> eps_pn_am5728_register_board_for_isr()");
	}

	return retVal;
}

/**
 * Kill ISR_PN_GATHERED for AM5728 board
 * 
 * @param [in] pBoard       : pointer to eps_pn_am5728_board_tag
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_kill_pn_gathered_isr_thread(
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard)
{
	LSA_UINT16                      retVal;
	LSA_BOOL                        bKillThread;
	EPS_PN_AM5728_DRV_ISR_PTR_TYPE  pIsr = &pBoard->PnGatheredIsr;

	retVal = eps_pn_am5728_unregister_board_for_isr(pIsr, pBoard, &bKillThread);

	if (bKillThread)
	{
		eps_pn_am5728_kill_isr_thread("EPS_AM5728_ISR", pIsr, pBoard);
	}
	else
	{
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_kill_pn_gathered_isr_thread(): call failed -> eps_pn_am5728_unregister_board_for_isr()");
	}

	return retVal;
}

/**
 * Undo initialization by setting bInit to FALSE.
 * 
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_am5728_uninstall(LSA_VOID)
{
	EPS_ASSERT(g_pEpsPnAm5728Drv->bInit == LSA_TRUE);

    if (am5728_drv_uninstall() != EPS_PN_DRV_RET_OK)
    {
        EPS_FATAL("eps_pn_am5728_uninstall(): Uninstalling AM5728 driver failed");
    }

	eps_pn_am5728_undo_init_critical_section();

	g_pEpsPnAm5728Drv->bInit = LSA_FALSE;

	g_pEpsPnAm5728Drv = LSA_NULL;
}

/**
 * Installing AM5728 driver. The AM5728 PN Dev Driver implements the eps_pndev_if.
 * 
 * Note that this module is implemented to run on the LD / HD firmware that runs on the ARM-Cortex-A15 of AM5728 board with the adonis operating system.
 * 
 * This function links the functions required by the EPS PN Dev IF to internal functions
 *  eps_pndev_if_open      -> eps_pn_am5728_open
 *  eps_pndev_if_close     -> eps_pn_am5728_close
 *  eps_pndev_if_uninstall -> eps_pn_am5728_uninstall
 * 
 * @see eps_pndev_if_register - this function is called to register the PN Dev implementation into the EPS PN Dev IF.
 * @see pnip_drv_install      - Interrupt integration.
 * 
 * @param   : LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_pn_am5728_drv_install(LSA_VOID)
{
	EPS_PNDEV_IF_TYPE   sPnAm5728DrvIf;

	eps_memset(&g_EpsPnAm5728Drv, 0, sizeof(g_EpsPnAm5728Drv));
	g_pEpsPnAm5728Drv = &g_EpsPnAm5728Drv;

	g_pEpsPnAm5728Drv->bInit = LSA_TRUE;

    eps_pn_am5728_init_critical_section();

	//Init Interface
	sPnAm5728DrvIf.open      = eps_pn_am5728_open;
	sPnAm5728DrvIf.close     = eps_pn_am5728_close;
	sPnAm5728DrvIf.uninstall = eps_pn_am5728_uninstall;

	// Register the PN Dev driver implementation to the EPS PN Dev IF
	eps_pndev_if_register(&sPnAm5728DrvIf);

    if (am5728_drv_install() != EPS_PN_DRV_RET_OK)
    {
        EPS_FATAL("eps_pn_am5728_drv_install(): Installing AM5728 driver failed");
    }
    g_pEpsPnAm5728Drv->board.PnFd = open(AM5728_DRV_NAME, O_RDWR);
    EPS_ASSERT(g_pEpsPnAm5728Drv->board.PnFd != -1);
}

/**
 * open AM5728 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_open. 
 * The function may only be called once since there is only one HD.
 * 
 * @see eps_pndev_if_open             - calls this function
 * 
 * @param [in] pLocation    : location descriptor (PCI Location or MAC Address)
 * @param [in] pOption      : options (debug, download of firmware). Note that not all PN Dev implementations support this.
 * @param [in] ppHwInstOut  : out structure with function pointers to access PNBoard functionality. Also contains info about the board.
 * @param [in] hd_id        : hd number. 0 = LD, 1...4 = HD instance.
 * @return #EPS_PNDEV_RET_UNSUPPORTED - This is no AM5728 board
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_open(
    EPS_PNDEV_LOCATION_CONST_PTR_TYPE       pLocation, 
    EPS_PNDEV_OPEN_OPTION_PTR_TYPE          pOption, 
    EPS_PNDEV_HW_PTR_PTR_TYPE               ppHwInstOut, 
    LSA_UINT16                              hd_id)
{
	LSA_UINT16                          result = EPS_PNDEV_RET_OK;
    LSA_INT                             ret_val;
    LSA_UINT8                         * pMapRegBase;
	EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard;

    EPS_PNCORE_MAC_ADDR_TYPE    MacAddresses;

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_am5728_open(): hd_id=%u", hd_id);

    /*--------------------------------------------------------------------------------*/

    EPS_ASSERT(g_pEpsPnAm5728Drv->bInit == LSA_TRUE);
	EPS_ASSERT(pLocation   != LSA_NULL);
	EPS_ASSERT(ppHwInstOut != LSA_NULL);
    EPS_ASSERT((hd_id >= 1) && (hd_id <= PSI_CFG_MAX_IF_CNT));

    eps_pn_am5728_enter();

    pBoard = eps_pn_am5728_alloc_board();
    EPS_ASSERT(pBoard != LSA_NULL); /* No more resources */

    /*--------------------------------------------------------------------------------*/
    // get MAC addresses
    eps_memset(&MacAddresses, 0, sizeof(MacAddresses));    
    eps_pncore_get_mac_addr(&MacAddresses);
    EPS_ASSERT(MacAddresses.lCtrMacAdr > 0);

    /*--------------------------------------------------------------------------------*/
    /* Mapping for base address of GMAC-SW register memory */
    // go to kernel-mode to save the base address (physical address)
    ret_val = ioctl( g_pEpsPnAm5728Drv->board.PnFd, 
                     EPS_AM5728_DRV_IOCTL_SRV_MMAP_REG_BASE_ADR, 
                     (LSA_VOID*)EPS_PN_AM5728_GMACSW_PHY_BASE_PTR );
    EPS_ASSERT(ret_val == 0);

    // map the physical address in kernel-mode to virtual address in user-mode
    pMapRegBase = (LSA_UINT8*) mmap( NULL, 
                                     EPS_PN_AM5728_GMACSW_SIZE, 
                                     PROT_READ | PROT_WRITE, 
                                     MAP_SHARED, 
                                     g_pEpsPnAm5728Drv->board.PnFd, 
                                     0 );

    if (pMapRegBase == MAP_FAILED)
    {
        EPS_FATAL("eps_pn_am5728_open(): mapping of GMAC-SW RegisterBaseAdr failed");
    }

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_am5728_open(): GmacSw.base_ptr=0x%x", pMapRegBase);

    /*--------------------------------------------------------------------------------*/
    // open TI LL
    result = eps_tiLL_open(&pBoard->uEddsTI, 
                           &pBoard->sHw, 
                           &pBoard->sysDev, 
                           pMapRegBase, 
                           hd_id, 
                           EPS_TI_PCI_VENDOR_ID, 
                           EPS_TI_PCI_DEVICE_ID_AM5728, 
                           (EPS_MAC_PTR_TYPE)&MacAddresses.lArrayMacAdr[0]);

    /*--------------------------------------------------------------------------------*/
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.base_ptr    = pMapRegBase;
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.phy_addr    = EPS_PN_AM5728_GMACSW_PHY_BASE_PTR;
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.size        = EPS_PN_AM5728_GMACSW_SIZE;

    /* SDRam settings for NRT - base address is from lk file */
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)__NRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = (LSA_UINT32)__NRT_RAM_START__;
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = (LSA_UINT32)__NRT_RAM_END__ - (LSA_UINT32)__NRT_RAM_START__; /* shared memory size */

    // setup DEV and NRT memory pools
    eps_am5728_gmac_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

    /*--------------------------------------------------------------------------------*/
	if (result == EPS_PNDEV_RET_OK)
	{
		*ppHwInstOut = &pBoard->sHw;
	}
	else
	{
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_open(): open failed");

		eps_pn_am5728_free_board(pBoard);
	}

    eps_pn_am5728_exit();

    return result;
}

/**
 * Close the AM5728 board.
 * 
 * This function implements the eps_pndev_if function eps_pndev_if_close.
 * 
 * @see eps_pndev_if_close          - calls this function
 * 
 * @param [in] pHwInstIn    : handle to instance to close.
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_close(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn)
{	
    int                                 ret_val;
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard = LSA_NULL;

	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_am5728_close()");

	eps_pn_am5728_enter();

    pBoard = (EPS_PN_AM5728_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled == 0);

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_pn_am5728_close(): GmacSw.base_ptr=0x%x", 
        pBoard->sHw.EpsBoardInfo.edds.GmacSw.base_ptr);

	/* Unmapping for base address of GMAC-SW register memory */
	ret_val = munmap( (void*)pBoard->sHw.EpsBoardInfo.edds.GmacSw.base_ptr, pBoard->sHw.EpsBoardInfo.edds.GmacSw.size );
    if (ret_val != 0)
    {
        EPS_FATAL("eps_pn_am5728_close(): unmapping of GMAC-SW RegisterBaseAdr failed");
    }
    /* delete all addresses from open */ 
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.base_ptr    = 0;
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.phy_addr    = 0;
    pBoard->sHw.EpsBoardInfo.edds.GmacSw.size        = 0;
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = 0;
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = 0;
    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = 0;

    eps_pn_am5728_free_board(pBoard);

    eps_pn_am5728_exit();

    return EPS_PNDEV_RET_OK;
}

/**
 * Enabling interrupts for AM5728. This function implements the EPS PN DEV API function EnableIsr
 * 
 * @see eps_enable_pnio_event                       - calls this function
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @param [in] pInterrupt   : type of interrupt can be:
 *                             - #EPS_PNDEV_ISR_PN_GATHERED
 * @param [in] pCbf         : callbackfunction with parameters
 * 
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED when unknown interrupt type
 */
static LSA_UINT16 eps_pn_am5728_enable_interrupt(
    EPS_PNDEV_HW_CONST_PTR_TYPE               pHwInstIn, 
    EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE   pInterrupt, 
    EPS_PNDEV_CALLBACK_PTR_TYPE               pCbf)
{
	LSA_UINT16                          result = EPS_PNDEV_RET_OK;
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard = LSA_NULL;

    EPS_ASSERT(pHwInstIn != LSA_NULL);
    EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

    pBoard = (EPS_PN_AM5728_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

    EPS_ASSERT(pBoard->bUsed);

    eps_pn_am5728_enter();

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_am5728_enable_interrupt(): pInterrupt=%u", (*pInterrupt));

    switch (*pInterrupt)
    {
        case EPS_PNDEV_ISR_PN_GATHERED:
        {
            if (pBoard->sIsrPnGathered.pCbf != LSA_NULL)
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_enable_interrupt(): sIsrPnGathered.pCbf is not NULL");
                result = EPS_PNDEV_RET_ERR;
                break;
            }

            pBoard->sIsrPnGathered = *pCbf;

			result = eps_pn_am5728_create_pn_gathered_isr_thread(pBoard);

            if (result == EPS_PNDEV_RET_OK)
            {
                EPS_POSIX_PTHREAD_KILL(pBoard->PnGatheredIsr.hThread, SIGRT0);
            }
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_enable_interrupt(): call failed -> eps_pn_am5728_create_pn_gathered_isr_thread()");
                pBoard->sIsrPnGathered.pCbf = LSA_NULL;
            }

            break;
        }

        default:
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_enable_interrupt(): unsupported pInterrupt=%u", (*pInterrupt));
            result = EPS_PNDEV_RET_UNSUPPORTED;
            break;
        }
    }

    if (result == EPS_PNDEV_RET_OK)
    {
        pBoard->uCountIsrEnabled++;
    }

    eps_pn_am5728_exit();

    return result;
}

/**
 * Disabling interrupts for AM5728
 * 
 * @see eps_disable_pnio_event      - calls this function
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @param [in] pInterrupt   : type of interrupt can be:
 *                             - #EPS_PNDEV_ISR_PN_GATHERED
 * @return #EPS_PNDEV_RET_UNSUPPORTED - interrupt type not supported
 * @return #EPS_PNDEV_RET_ERR         - an error occured
 * @return #EPS_PNDEV_RET_OK
 */
static LSA_UINT16 eps_pn_am5728_disable_interrupt(
    EPS_PNDEV_HW_CONST_PTR_TYPE         pHwInstIn, 
    EPS_PNDEV_INTERRUPT_DESC_CONST_PTR_TYPE   pInterrupt)
{
    EPS_PN_AM5728_DRV_BOARD_PTR_TYPE    pBoard = LSA_NULL;
    LSA_UINT16                          result = EPS_PNDEV_RET_OK;

	pBoard = (EPS_PN_AM5728_DRV_BOARD_PTR_TYPE)pHwInstIn->hDevice;

	EPS_ASSERT(pBoard->bUsed);
	EPS_ASSERT(pBoard->uCountIsrEnabled != 0);

    eps_pn_am5728_enter();

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pn_am5728_disable_interrupt(): pInterrupt=%u", (*pInterrupt));

	switch (*pInterrupt)
	{
		case EPS_PNDEV_ISR_PN_GATHERED:
        {
			if (pBoard->sIsrPnGathered.pCbf == LSA_NULL)
			{
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_disable_interrupt(): sIsrPnGathered.pCbf is NULL");
				result = EPS_PNDEV_RET_ERR;
				break;
			}

			result = eps_pn_am5728_kill_pn_gathered_isr_thread(pBoard);

			if (result == EPS_PNDEV_RET_OK)
			{
				pBoard->sIsrPnGathered.pCbf = LSA_NULL;
			}
            else
            {
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_disable_interrupt(): call failed -> eps_pn_am5728_kill_pn_gathered_isr_thread()");
            }

            break;
        }

        default:
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_pn_am5728_disable_interrupt(): unsupported pInterrupt=%u", (*pInterrupt));
            result = EPS_PNDEV_RET_UNSUPPORTED;
            break;
        }
    }

    if (result == EPS_PNDEV_RET_OK)
    {
        pBoard->uCountIsrEnabled--;
    }

    eps_pn_am5728_exit();

    return result;
}

/**
 * Enable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_enable_hw_interrupt(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("eps_pn_am5728_enable_hw_interrupt(): for AM5728 not supported in this driver");

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Disable HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */             
static LSA_UINT16 eps_pn_am5728_disable_hw_interrupt(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("eps_pn_am5728_disable_hw_interrupt(): for AM5728 not supported in this driver");

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Read HW Interrupt - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_read_hw_interrupt(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn, 
    LSA_UINT32            *       interrupts)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(interrupts);
    EPS_FATAL("eps_pn_am5728_read_hw_interrupt(): for AM5728 not supported in this driver");

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Sets GPIO pins of the AM5728 Board. 
 * 
 * Not used currently.
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @param [in] gpio         : GPIO PIN
 *  possible values: 
 *    #EPS_PNDEV_DEBUG_GPIO_1
 *    #EPS_PNDEV_DEBUG_GPIO_2
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED  - GPIO typer not supported
 */
static LSA_UINT16 eps_pn_am5728_set_gpio(
    EPS_PNDEV_HW_CONST_PTR_TYPE     pHwInstIn, 
    EPS_PNDEV_GPIO_DESC_TYPE        gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    
    /* ToDo */
    return EPS_PNDEV_RET_OK;
}

/**
 * Sets GPIO pins of the AM5728 Board.
 * 
 * Not used currently.
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @param [in] gpio         : GPIO PIN
 *  possible values: 
 *    #EPS_PNDEV_DEBUG_GPIO_1
 *    #EPS_PNDEV_DEBUG_GPIO_2
 *
 * @return #EPS_PNDEV_RET_OK
 * @return #EPS_PNDEV_RET_UNSUPPORTED  - GPIO typer not supported
 */
static LSA_UINT16 eps_pn_am5728_clear_gpio(
    EPS_PNDEV_HW_CONST_PTR_TYPE     pHwInstIn, 
    EPS_PNDEV_GPIO_DESC_TYPE        gpio)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(gpio);
    
    /* ToDo */
    return EPS_PNDEV_RET_OK;
}

/**
 * Starts timer control     - NOT SUPPORTED IN THIS DRIVER
 *  
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @param [in] pCbf         : callback function to call
 * 
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_timer_ctrl_start(
    EPS_PNDEV_HW_CONST_PTR_TYPE     pHwInstIn, 
    EPS_PNDEV_CALLBACK_PTR_TYPE     pCbf)
{
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(pCbf);
    EPS_FATAL("eps_pn_am5728_timer_ctrl_start(): for AM5728 not supported in this driver");

    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Stop timer control - NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTE
 */
static LSA_UINT16 eps_pn_am5728_timer_ctrl_stop(
    EPS_PNDEV_HW_CONST_PTR_TYPE    pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    EPS_FATAL("eps_pn_am5728_timer_ctrl_stop(): for AM5728 not supported in this driver");
    
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * read AM5728 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_READ_LOWER_CPU       - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn        : pointer to hardware instance 
 * @param [in] offset           : offset inside the trace buffer for the data to write 
 * @param [in] ptr              : pointer to data to be written to (destination)
 * @param [in] size             : size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_read_trace_data(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn, 
    LSA_UINT32                    offset, 
    LSA_UINT8                   * ptr, 
    LSA_UINT32                    size)
{   
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(offset);
    LSA_UNUSED_ARG(ptr);
    LSA_UNUSED_ARG(size);
    EPS_FATAL("eps_pn_am5728_read_trace_data(): for AM5728 not supported in this driver");
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * write AM5728 trace data - NOT SUPPORTED IN THIS DRIVER
 * 
 * @see PNTRC_WRITE_LOWER_CPU      - calls this function (not in HD firmwares)
 * 
 * @param [in] pHwInstIn        : pointer to hardware instance 
 * @param [in] offset           : offset inside the trace buffer for the data to write 
 * @param [in] ptr              : pointer to data to be written to (destination)
 * @param [in] size             : size of the data to be written
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_write_trace_data(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn, 
    LSA_UINT8             const * ptr, 
    LSA_UINT32                    offset, 
    LSA_UINT32                    size)
{   
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(offset);
    LSA_UNUSED_ARG(ptr);
    LSA_UNUSED_ARG(size);
    EPS_FATAL("eps_pn_am5728_write_trace_data(): for AM5728 not supported in this driver");
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * Reads the time of a lower instance and traces it. NOT SUPPORTED IN THIS DRIVER
 * 
 * @param [in] pHwInstIn    : handle to lower instance. Not used
 * @param [in] lowerCpuId   : handle to lower CPU. Used in trace macro.
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_write_sync_time_lower(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn, 
    LSA_UINT16              lowerCpuId)
{   
    LSA_UNUSED_ARG(pHwInstIn);
    LSA_UNUSED_ARG(lowerCpuId);
    EPS_FATAL("eps_pn_am5728_write_sync_time_lower(): for AM5728 not supported in this driver");
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/**
 * save dump file in fatal
 * 
 * @param [in] pHwInstIn    : pointer to hardware instance
 * @return #EPS_PNDEV_RET_UNSUPPORTED
 */
static LSA_UINT16 eps_pn_am5728_save_dump(
    EPS_PNDEV_HW_CONST_PTR_TYPE   pHwInstIn)
{
    LSA_UNUSED_ARG(pHwInstIn);
    return EPS_PNDEV_RET_UNSUPPORTED;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

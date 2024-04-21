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
/*  F i l e               &F: eps_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS System Startup + Error Handling                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20001
#define EPS_MODULE_ID      20001

#define EPS_SYS /* see lsa_cfg.h */

#include <eps_plf.h>                /* EPS Platform Interface		    */
#include <eps_rtos.h>               /* RTOS Interface                   */
#include <eps_sys.h>                /* Types / Prototypes               */
#include <eps_cp_hw.h>              /* CP PSI adation                   */
#include <eps_cp_mem.h>             /* CP Mem Pool adaption             */
#include <eps_locks.h>              /* EPS Locks                        */
#include <eps_events.h>             /* EPS Events                       */
#include <eps_msg.h>                /* EPS Msg                          */
//#include <eps_mem.h>              /* Local Memory Handling            */
#include <eps_trc.h>                /* Tracing                          */
#include <eps_tasks.h>              /* EPS Threads                      */
#include <eps_shm_if.h>             /* Shared Memory Interface          */
#if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
#include <eps_trace_timer_if.h>     /* EPS Trace Timer IF               */
#endif
#include <eps_app.h>                /* EPS Application Interface				*/
#include <eps_timer.h>              /* Timer System Interface					*/
#include <eps_pn_drv_if.h>          /* EPS PN Driver Interface					*/
#include <eps_hif_drv_if.h>	        /* EPS HIF Driver Interface					*/
#include <eps_ipc_drv_if.h>	        /* EPS IPC Driver Interface					*/

#if (EPS_PLF != EPS_PLF_LINUX_SOC1)
#include "eps_pndevdrv_boards.h"    /* EPS PNDevDrv board specific functions    */
#endif

#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
#if ( PSI_CFG_USE_PNTRC == 1 )
#include <pntrc_inc.h>     /* PNTRC Interface */
#endif
#endif

#if ( EPS_CFG_USE_STATISTICS == 1 )
#include <eps_statistics.h>         /* EPS Statistics module            */
#include <eps_statistics_stack.h>   /* EPS Statistics module            */
#endif
//#include <eps_usr.h>
#include <stdio.h>                  /* for vsprintf */
#include "eps_register.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#ifdef _TEST
#include "epsTests.h"
#if (EPS_PLF == EPS_PLF_WINDOWS_X86)
#include "testapp.h"
#endif
#endif
EPS_FILE_SYSTEM_EXTENSION(EPS_MODULE_ID)

/*------------------------------------------------------------------------------
//	Forwarded decl
//----------------------------------------------------------------------------*/

typedef struct
{
    LSA_VOID *_user_arg;
    EPS_LOWER_DONE_FCT _user_func;

} /*EPS_RQB_USER_ID_TYPE, */ *EPS_RQB_USER_ID_PTR_TYPE;

static LSA_VOID eps_system_request_lower_done(LSA_VOID* pRQBData);
static LSA_VOID eps_system_request_lower(LSA_VOID_PTR_TYPE pRQBData, LSA_VOID_PTR_TYPE pInst, EPS_LOWER_DONE_FCT pFct, LSA_UINT16 mbox_id, PSI_REQUEST_FCT req_fct);

/*------------------------------------------------------------------------------
//	Module Globals
//----------------------------------------------------------------------------*/
static EPS_SHM_HW_PTR_TYPE g_pEpsShmHw = LSA_NULL;

// for fatal in fatal
static EPS_SHM_FATAL_PAR_TYPE   g_Eps_FatalInFatal;
static LSA_BOOL                 g_Eps_TraceInFatalDone = LSA_FALSE;
static LSA_UINT16               g_Eps_fatal_in_fatal_enter_exit_id = EPS_LOCK_ID_INVALID;

//lint --esym(457, g_Eps_TraceInFatalDone)
//lint --esym(457, g_Eps_FatalInFatal)

/*------------------------------------------------------------------------------
 // Advanced Reboot Globals
 //----------------------------------------------------------------------------*/

#ifndef EPS_SUPPRESS_PRINTF
#define EPS_IMCEADRV_PRINTF
#endif

/*------------------------------------------------------------------------------
//	Set / Get EPS-State
//----------------------------------------------------------------------------*/
static LSA_VOID eps_set_state( EPS_STATE_TYPE State )
{
    EPS_ASSERT(g_pEpsData != LSA_NULL);
    g_pEpsData->eState = State;
}

static EPS_STATE_TYPE eps_get_state( LSA_VOID )
{
    EPS_ASSERT(g_pEpsData != LSA_NULL);
    return (g_pEpsData->eState);
}

/*------------------------------------------------------------------------------
//	Get runlevel of LD part
//----------------------------------------------------------------------------*/

PSI_LD_RUNS_ON_TYPE eps_get_ld_runs_on( LSA_VOID )
{
    #if defined HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT
    return PSI_LD_RUNS_ON_BASIC;
    #else
    EPS_ASSERT( g_pEpsData->ldRunsOnType < PSI_LD_RUNS_ON_MAX );
    return g_pEpsData->ldRunsOnType;
    #endif
}

/*------------------------------------------------------------------------------
//	Software Fatal Handling
//----------------------------------------------------------------------------*/

LSA_VOID eps_store_fatal_info( EPS_SHM_HW_CONST_PTR_TYPE pEpsShmHw, EPS_SHM_FATAL_PAR_CONST_PTR_TYPE pFatal )
{
    //Exported function, suppress lint warnings for "could be declared static"
    //lint -esym(765, eps_store_fatal_info)

    EPS_SHM_FATAL_STORE_PTR_TYPE pFatalStore;
    LSA_UINT32                   i;

    if( ( pEpsShmHw != LSA_NULL ) &&
        ( pEpsShmHw->sFatal.pBase != LSA_NULL ) &&
        ( pEpsShmHw->sFatal.uSize >= sizeof(EPS_SHM_FATAL_STORE_TYPE) ) )
    {
        if (pFatal != LSA_NULL)
        {
            pFatalStore = (EPS_SHM_FATAL_STORE_PTR_TYPE) EPS_CAST_TO_VOID_PTR(pEpsShmHw->sFatal.pBase);

            if (pFatalStore->bUsed != 1)
            {
                pFatalStore->exit_code_org = pFatal->exit_code_org;
                pFatalStore->uLine 		   = pFatal->uLine;

                if (pFatal->pMsg != LSA_NULL)
                    for (i = 0; (i < 200) && (pFatal->pMsg[i]) != 0; i++)
                        pFatalStore->sMsg[i] = pFatal->pMsg[i];

                if (pFatal->pFile != LSA_NULL)
                    for (i = 0; (i < 200) && (pFatal->pFile[i]) != 0; i++)
                        pFatalStore->sFile[i] = pFatal->pFile[i];

                if (pFatal->pComp != LSA_NULL)
                    for (i = 0; (i < 200) && (pFatal->pComp[i]) != 0; i++)
                        pFatalStore->sComp[i] = pFatal->pComp[i];

                if (pFatal->pFunc != LSA_NULL)
                    for (i = 0; (i < 200) && (pFatal->pFunc[i]) != 0; i++)
                        pFatalStore->sFunc[i] = pFatal->pFunc[i];

                pFatalStore->uLsaFatalLen = pFatal->uLsaFatalLen;

                if (pFatal->pLsaFatal != LSA_NULL)
                {
                    pFatalStore->lsa_fatal.lsa_component_id     = pFatal->pLsaFatal->lsa_component_id;
                    pFatalStore->lsa_fatal.module_id            = pFatal->pLsaFatal->module_id;
                    pFatalStore->lsa_fatal.line                 = pFatal->pLsaFatal->line;
                    pFatalStore->lsa_fatal.error_data_length    = pFatal->pLsaFatal->error_data_length;
                    pFatalStore->lsa_fatal.error_code[0]        = pFatal->pLsaFatal->error_code[0];
                    pFatalStore->lsa_fatal.error_code[1]        = pFatal->pLsaFatal->error_code[1];
                    pFatalStore->lsa_fatal.error_code[2]        = pFatal->pLsaFatal->error_code[2];
                    pFatalStore->lsa_fatal.error_code[3]        = pFatal->pLsaFatal->error_code[3];

                    if (pFatal->pComp != LSA_NULL)
                    {
                        pFatalStore->lsa_fatal.error_data_ptr   = &(pFatalStore->lsa_fatal_error_data[0]);
                        for (i = 0; (i < pFatal->pLsaFatal->error_data_length) && (i < EPS_ERROR_DATA_BUF_LENGTH); i++)
                        {
                            pFatalStore->lsa_fatal_error_data[i] = ((LSA_UINT8*)pFatal->pLsaFatal->error_data_ptr)[i];
                        }
                    }
                    else
                    {
                        pFatalStore->lsa_fatal.error_data_ptr   = LSA_NULL;
                    }
                }

                pFatalStore->uSize = sizeof(EPS_SHM_FATAL_STORE_TYPE);
                pFatalStore->bUsed = 1;
            }
        }
        else
        {
            for (i = 0; i < sizeof(EPS_SHM_FATAL_STORE_TYPE); i++)
            {
                pEpsShmHw->sFatal.pBase[i] = 0;
            }
        }
    }
}

/**
    eps_fatal_with_reason()

    exit_code_org: EPS_EXIT_CODE_LSA_FATAL: The following parameters are valid: comp, length, lsa_fatal_ptr
                   else:                    The following parameters are valid: file, line, func, str

    @param exit_code_org:    original exit code that causes the error
    @param exit_code_signal: signal exit code for signaling to EPS_APP_KILL_EPS() (in some cases the original exit code is mapped to another signal exit code)
*/
LSA_VOID eps_fatal_with_reason(
    EPS_TYPE_EXIT_CODE      const exit_code_org, 
    EPS_TYPE_EXIT_CODE      const exit_code_signal, 
    LSA_CHAR              * comp, 
    LSA_UINT16              length, 
    LSA_FATAL_ERROR_TYPE  * const lsa_fatal_ptr, 
    LSA_CHAR              * file, 
    LSA_INT                 line, 
    const LSA_CHAR        * func, 
    LSA_CHAR              * str)
{
    #ifdef _TEST
    if (exit_code_signal == EPS_EXIT_CODE_EPS_EXCEPTION)
    {
        #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
        googleTestFatal();
        return;
        #endif
    }
    else if ((exit_code_signal == EPS_EXIT_CODE_EPS_FATAL) || (exit_code_signal == EPS_EXIT_CODE_LSA_FATAL))
    {
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Msg(%s)", str);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): File(%s)", file);
        EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Function(%s)", func);
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Line(%d)", line);
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, " << eps_fatal_with_reason(): EPS-State(%u)", eps_get_state());
        epsFatalCallback(exit_code_org, exit_code_signal);
        return;
    }
    #endif //_TEST

    #if ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
    printf("\nFATAL");
    printf("\nFile: %s", file);
    printf("\nLine: %d", line);
    printf("\nFunc: %s", func);
    printf("\nStr : %s", str);
    #endif

    if (g_pEpsData == LSA_NULL)
    {
        //EPS_FATAL was called after eps_undo_init was called. unrecoverable error. Notify Application and call KILL_EPS.
        EPS_APP_FATAL_HOOK(0);

        EPS_APP_KILL_EPS(0); //Let EPS die here!
        return;
    }

    //is already allocated ?
    if (g_Eps_fatal_in_fatal_enter_exit_id != EPS_LOCK_ID_INVALID)
    {
        //to avoid the simultaneous call of "eps_fatal" from different threads
        #if defined(EPS_USE_RTOS_ADONIS)
        // in adonis: check we are not in interrupt
        if (adn_get_system_state() != ADN_SYS_STATE_IRQ)
        #endif 
        {
            eps_enter_critical_section(g_Eps_fatal_in_fatal_enter_exit_id);
        }
    }

    if (!g_pEpsData->bEpsInFatal) //FATAL already handled, usecase FATAL in FATAL?
    {
        EPS_SHM_FATAL_PAR_TYPE  sFatal;        
		#if (EPS_PLF != EPS_PLF_LINUX_SOC1)
		LSA_UINT8               hd_id;
		EPS_PNDEV_HW_PTR_TYPE   ppHwInst;
		#endif

        g_pEpsData->bEpsInFatal = LSA_TRUE; //do not use trace calls before this instruction!

        /* Store information, if */
        /* - shared mem is available and */
        /* - the eps shared mem interface is already initialized or not yet uninitialized */
        if ((g_pEpsShmHw != LSA_NULL) && (g_pEpsData->bEpsShmIfInitialized))
        {
            eps_memset(&sFatal, 0, sizeof(sFatal));
            eps_memset(&g_Eps_FatalInFatal, 0, sizeof(g_Eps_FatalInFatal));

            sFatal.exit_code_org        = exit_code_org;
            g_Eps_FatalInFatal.exit_code_org = exit_code_org;

            switch (exit_code_org)
            {
                case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
                {
                    sFatal.pComp        = comp;
                    sFatal.uLsaFatalLen = length;
                    sFatal.pLsaFatal    = lsa_fatal_ptr;

                    g_Eps_FatalInFatal.pComp        = comp;
                    g_Eps_FatalInFatal.uLsaFatalLen = length;
                    g_Eps_FatalInFatal.pLsaFatal    = lsa_fatal_ptr;

                    break;
                }
                default: //all other exit_codes/callers
                {
                    sFatal.pFile = file;
                    sFatal.uLine = (LSA_UINT32)line;
                    sFatal.pFunc = func;
                    sFatal.pMsg  = str;

                    g_Eps_FatalInFatal.pFile = file;
                    g_Eps_FatalInFatal.uLine = (LSA_UINT32)line;
                    g_Eps_FatalInFatal.pFunc = func;
                    g_Eps_FatalInFatal.pMsg  = str;
                }
            }

            eps_store_fatal_info(g_pEpsShmHw, &sFatal);

            (LSA_VOID)g_pEpsShmHw->FatalError(g_pEpsShmHw, sFatal);
        }

        EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, " >> eps_fatal_with_reason(): exit_code_org(%u) exit_code_signal(%u) EPS-State(%u)", exit_code_org, exit_code_signal, eps_get_state());

        eps_tasks_fatal();

		
        //if the eps device interface is already initialized or not yet uninitialized

		#if (EPS_PLF != EPS_PLF_LINUX_SOC1) // not for linux on soc1 because there is no pndevdrv
        if (g_pEpsData->bEpsDevIfInitialized)
        {
            for ( hd_id = 0; hd_id < (PSI_CFG_MAX_IF_CNT+1); hd_id++ )
            {
                if ((eps_pndev_if_get(hd_id, &ppHwInst) != EPS_PNDEV_RET_DEV_NOT_FOUND) && (g_pEpsData->uEpsOpen.write_dumpfiles_on_fatal[hd_id]))
                {
                    EPS_PNDEV_INTERRUPT_DESC_TYPE   IsrDesc;
					EPS_PNDEVDRV_BOARD_PTR_TYPE     pBoard;

                    //save dump
                    (LSA_VOID)ppHwInst->SaveDump(ppHwInst);
                    
                    pBoard = (EPS_PNDEVDRV_BOARD_PTR_TYPE)ppHwInst->hDevice;

                    if (0 != pBoard->uCountIsrEnabled)
                    {
                        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_fatal_with_reason(): disabling interrupts for hd_id(%u)", hd_id);

                        //disable group interrupt
                        IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;

                        (LSA_VOID)ppHwInst->DisableIsr(ppHwInst, &IsrDesc);

                        if (   (ppHwInst->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
                            || (ppHwInst->asic_type == EPS_PNDEV_ASIC_HERA) )
                        {
                            //disable single interrupt
                            IsrDesc = EPS_PNDEV_ISR_PN_NRT;
                            (LSA_VOID)ppHwInst->DisableIsr(ppHwInst, &IsrDesc);
                        }
                    }
                }
            }
        }
		#endif

        switch (exit_code_org)
        {
            case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
            {
                EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: Comp(%s)", comp);
                if (lsa_fatal_ptr != LSA_NULL)
                {
                    EPS_SYSTEM_TRACE_07(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: CompId(%u) ModuleId(%u) Line(%u) ErrCode[0](%u) [1](%u) [2](%u) [3](%u)", lsa_fatal_ptr->lsa_component_id, lsa_fatal_ptr->module_id, lsa_fatal_ptr->line, lsa_fatal_ptr->error_code[0], lsa_fatal_ptr->error_code[1], lsa_fatal_ptr->error_code[2], lsa_fatal_ptr->error_code[3]);
                }
                else
                {
                    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: lsa_fatal_ptr == NULL!");
                }
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, " << eps_fatal_with_reason(): EPS-State(%u)", eps_get_state());
                break;
            }
            default: //all other exit_codes/callers
            {
                EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Msg(%s)", str);
                EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): File(%s)", file);
                EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Function(%s)", func);
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Line(%d)", line);
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, " << eps_fatal_with_reason(): EPS-State(%u)", eps_get_state());
            }
        }

        g_Eps_TraceInFatalDone = LSA_TRUE;

        (LSA_VOID)pntrc_fatal_occurred(); //From now on, no Traces will be recorded!

        EPS_APP_FATAL_HOOK(exit_code_signal);
        EPS_APP_KILL_EPS(exit_code_signal); //Let EPS die here!
    }
    else
    {
        //FATAL in FATAL occured!

        if (!g_Eps_TraceInFatalDone)
        {
            g_Eps_TraceInFatalDone = LSA_TRUE;

            EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): FATAL in FATAL occured - first FATAL see below");

            switch (g_Eps_FatalInFatal.exit_code_org)
            {
                case EPS_EXIT_CODE_LSA_FATAL: //caller = PSI_FATAL_ERROR()
                {
                    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: Comp(%s)", g_Eps_FatalInFatal.pComp);
                    if (g_Eps_FatalInFatal.pLsaFatal != LSA_NULL)
                    {
                        EPS_SYSTEM_TRACE_07(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: CompId(%u) ModuleId(%u) Line(%u) ErrCode[0](%u) [1](%u) [2](%u) [3](%u)",
                            g_Eps_FatalInFatal.pLsaFatal->lsa_component_id,
                            g_Eps_FatalInFatal.pLsaFatal->module_id,
                            g_Eps_FatalInFatal.pLsaFatal->line,
                            g_Eps_FatalInFatal.pLsaFatal->error_code[0],
                            g_Eps_FatalInFatal.pLsaFatal->error_code[1],
                            g_Eps_FatalInFatal.pLsaFatal->error_code[2],
                            g_Eps_FatalInFatal.pLsaFatal->error_code[3]);
                    }
                    else
                    {
                        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): LSA_FATAL: sFatalInFatal.pLsaFatal == NULL!");
                    }
                    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, " << eps_fatal_with_reason(): EPS-State(%u)", eps_get_state());
                    break;
                }
                default: //all other exit_codes/callers
                {
                    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Msg(%s)", g_Eps_FatalInFatal.pMsg);
                    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): File(%s)", g_Eps_FatalInFatal.pFile);
                    EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Function(%s)", g_Eps_FatalInFatal.pFunc);
                    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_fatal_with_reason(): Line(%d)", g_Eps_FatalInFatal.uLine);
                    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, " << eps_fatal_with_reason(): EPS-State(%u)", eps_get_state());
                }
            }

            (LSA_VOID)pntrc_fatal_occurred(); //From now on, no Traces will be recorded!
        }

        EPS_APP_FATAL_HOOK(exit_code_signal);
        EPS_APP_KILL_EPS(exit_code_signal); //Let EPS die here!
    }
}

/*------------------------------------------------------------------------------
//	Init / Startup
//----------------------------------------------------------------------------*/
#if ( EPS_CFG_USE_HIF == 1)
static LSA_VOID eps_startup_done( HIF_HANDLE hH )
{
    LSA_UINT16 retVal;
    EPS_ASSERT(g_pEpsData != LSA_NULL);
    
    /* Signalize FW Ready and Check Driver State */
    retVal = g_pEpsShmHw->AppReady(g_pEpsShmHw, &hH);
    EPS_ASSERT(EPS_SHM_RET_OK == retVal);
}
#endif // ( EPS_CFG_USE_HIF == 1)

#if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
static LSA_VOID eps_hif_hd_lower_open_done( LSA_VOID * pArg, HIF_RQB_PTR_TYPE pRQB )
{
    LSA_UINT16 * phDoneEvent = (LSA_UINT16 *)pArg;
    LSA_UINT16   rc;

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT(g_pEpsData  != LSA_NULL);
    EPS_ASSERT(pRQB        != LSA_NULL);

    EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
    EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_HD_LOWER_OPEN);

    g_pEpsData->hLowerHD = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(rc == LSA_RET_OK);

    eps_startup_done(g_pEpsData->hLowerHD); /* System ready for open! - Send Signal to PnDevDrv */

    eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_hd_lower_close_done( LSA_VOID * arg, HIF_RQB_PTR_TYPE pRQB )
{
    LSA_UINT16 rc;
    LSA_UINT16* phDoneEvent = (LSA_UINT16*)arg;

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT(pRQB     != LSA_NULL);

    EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
    EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_HD_LOWER_CLOSE);

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(rc == LSA_RET_OK);

    eps_set_event(*phDoneEvent);
}

static LSA_VOID eps_hif_hd_lower_open( EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16 * phDoneEvent )
{
    HIF_RQB_PTR_TYPE pRQB;
    PSI_HIF_SYS_PTR_TYPE pSysLowerHD;

    EPS_ASSERT(pEpsData != LSA_NULL);

    /* Create HIF LD Lower Device */
    pSysLowerHD = &pEpsData->SysLowerHD;

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB);
    EPS_ASSERT(pRQB != LSA_NULL);

    HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_HD_LOWER_OPEN );
    HIF_RQB_SET_HANDLE ( pRQB, 0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

    pSysLowerHD->hd_nr         = 1;
    pSysLowerHD->shortcut_mode = LSA_FALSE;
    pSysLowerHD->is_upper      = LSA_FALSE;

    pRQB->args.dev_lower_open.hSysDev = pSysLowerHD;
    pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; /* out from HIF */
    pRQB->args.dev_lower_open.Cbf     = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_hd_lower_open_done, PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system);
}

LSA_VOID eps_hif_hd_lower_close(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
    HIF_RQB_PTR_TYPE pRQB;

    EPS_ASSERT(pEpsData != LSA_NULL);

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB);
    EPS_ASSERT(pRQB != LSA_NULL);

    HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_HD_LOWER_CLOSE);
    HIF_RQB_SET_HANDLE ( pRQB, 0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

    pRQB->args.dev_close.hH  = pEpsData->hLowerHD;
    pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_hd_lower_close_done, PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_system);
}

#else // (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)

#if ( EPS_CFG_USE_HIF == 1)
LSA_VOID eps_hif_ld_upper_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
       LSA_UINT16 rc;
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

    EPS_ASSERT( pRQB != 0 );
    EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == HIF_OK );
    EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == HIF_OPC_LD_UPPER_OPEN );

    g_pEpsData->hUpperLD = pRQB->args.dev_ld_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT( rc == LSA_RET_OK );

    // Signal done
    eps_set_event( *phDoneEvent );
}

static LSA_VOID eps_hif_ld_lower_open_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
    LSA_UINT16 rc;

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT(g_pEpsData  != LSA_NULL);
    EPS_ASSERT(pRQB        != LSA_NULL);

    EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
    EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_LD_LOWER_OPEN);

    g_pEpsData->hLowerLD = pRQB->args.dev_lower_open.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(rc == LSA_RET_OK);

    eps_startup_done(g_pEpsData->hLowerLD);   /* System ready for open! - Send Signal to PnDevDrv */

    eps_set_event(*phDoneEvent);
}

#if ( (EPS_PLF != EPS_PLF_LINUX_SOC1 ) || ( (EPS_PLF == EPS_PLF_LINUX_SOC1 ) && ( PSI_CFG_USE_LD_COMP == 1 )  ) ) 
static LSA_VOID eps_hif_ld_lower_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
    HIF_RQB_PTR_TYPE     pRQB;
    PSI_HIF_SYS_PTR_TYPE pSysLowerLD;

    EPS_ASSERT(pEpsData != LSA_NULL);

    /* Create HIF LD Lower Device */
    pSysLowerLD = &pEpsData->SysLowerLD;

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB);
    EPS_ASSERT(pRQB != LSA_NULL);

    HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_LD_LOWER_OPEN );
    HIF_RQB_SET_HANDLE ( pRQB, 0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

    pSysLowerLD->hd_nr = 0;

    if (PSI_LD_RUNS_ON_LIGHT == pEpsData->ldRunsOnType)
    {
        pSysLowerLD->shortcut_mode = LSA_TRUE;
    }
    else
    {
        pSysLowerLD->shortcut_mode = LSA_FALSE;
    }

    pSysLowerLD->ldRunsOnType = pEpsData->ldRunsOnType;

    pSysLowerLD->is_upper     = LSA_FALSE;

    pRQB->args.dev_lower_open.hSysDev = pSysLowerLD;
    pRQB->args.dev_lower_open.hH      = PSI_INVALID_HANDLE; /* out from HIF */
    pRQB->args.dev_lower_open.Cbf     = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_lower_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}
#endif

static LSA_VOID eps_hif_ld_lower_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
    LSA_UINT16 rc;

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT(pRQB        != LSA_NULL);

    EPS_ASSERT(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK);
    EPS_ASSERT(HIF_RQB_GET_OPCODE(pRQB)   == HIF_OPC_LD_LOWER_CLOSE);

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(rc == LSA_RET_OK);

    eps_set_event(*phDoneEvent);
}

#if ( (EPS_PLF != EPS_PLF_LINUX_SOC1 ) || ( (EPS_PLF == EPS_PLF_LINUX_SOC1 ) && ( EPS_CFG_USE_HIF_LD == 1 ) ) )
static LSA_VOID eps_hif_ld_lower_close(EPS_DATA_CONST_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
    HIF_RQB_PTR_TYPE pRQB;

    EPS_ASSERT(pEpsData != LSA_NULL);

	HIF_ALLOC_LOCAL_MEM((LSA_VOID_PTR_TYPE*)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB);
    EPS_ASSERT(pRQB != LSA_NULL);

    HIF_RQB_SET_OPCODE ( pRQB, HIF_OPC_LD_LOWER_CLOSE);
    HIF_RQB_SET_HANDLE ( pRQB, 0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF );

    pRQB->args.dev_close.hH  = pEpsData->hLowerLD;
    pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_lower_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}
#endif

#ifdef EPS_LOCAL_PN_USER
// Close HIF LD upper connection
static LSA_VOID eps_hif_ld_upper_close_done(LSA_VOID* pArg, HIF_RQB_PTR_TYPE pRQB)
{
       LSA_UINT16 rc;
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

    EPS_ASSERT( pRQB != 0 );
    EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == HIF_OK );
    EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == HIF_OPC_LD_UPPER_CLOSE );

    g_pEpsData->hUpperLD = pRQB->args.dev_close.hH;

	HIF_FREE_LOCAL_MEM( &rc, pRQB, LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT( rc == LSA_RET_OK );

    // Signal done
    eps_set_event( *phDoneEvent );
}

#if ( EPS_PLF != EPS_PLF_LINUX_SOC1 )
static LSA_VOID eps_hif_ld_upper_close(EPS_DATA_CONST_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
    HIF_RQB_PTR_TYPE pRQB;

	HIF_ALLOC_LOCAL_MEM((void**)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(pRQB);

    HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_LD_UPPER_CLOSE );
    HIF_RQB_SET_HANDLE( pRQB,  0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF);

    pRQB->args.dev_close.hH =  pEpsData->hUpperLD;

    pRQB->args.dev_close.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_upper_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}
#endif
#endif
#endif // ( EPS_CFG_USE_HIF == 1)

#if ((defined EPS_CFG_USE_EDDI) && (EPS_CFG_USE_EDDI == 1))
/* Function will provide io mem pointer to application for eddi soc1 boards*/
static LSA_VOID eps_ld_upper_set_io_mem(EPS_HD_IO_OUTPUT_CONST_PTR_TYPE io_param_out, LSA_UINT16 hd_id)
{
    EPS_BOARD_INFO_PTR_CONST_TYPE p_board_info;
    EPS_SYS_PTR_TYPE sys_handle;
    EPS_ASSERT((hd_id > 0) && (hd_id <= PSI_CFG_MAX_IF_CNT));


    if (PSI_LD_RUNS_ON_ADVANCED == g_EpsData.ldRunsOnType)
    {
        //There is no accessible io memory for the application
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_set_io_mem(): no io memory access possible in advanced variants");
        return;
    }
    else if(PSI_LD_RUNS_ON_BASIC == g_EpsData.ldRunsOnType)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_ld_upper_set_io_mem(): get io mem pointer for hd(0)");
        //When LD runs on BasicFW, there is only one HD possible and values are stored in ld storage "0"
        p_board_info = eps_hw_get_board_store(0);
    }
    else if(PSI_LD_RUNS_ON_LIGHT == g_EpsData.ldRunsOnType)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_ld_upper_set_io_mem(): get io mem pointer for hd(%ud)", hd_id);
        p_board_info = eps_hw_get_board_store(hd_id);
        sys_handle = p_board_info->hd_sys_handle;
        EPS_ASSERT(sys_handle->hd_nr == hd_id); // sanity check
    }
    else
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_ld_upper_set_io_mem(): invalid ldRunsOnType(%ud)", g_EpsData.ldRunsOnType);
        return;
    }


    switch (p_board_info->board_type)
    {
    //Note: for Ertec200 and Ertec400: there is the real size of io_mem.size in hd_out.edd.eddi.io_mem_size. It's manipulated by eddi ins hd firmware!

    case EPS_PNDEV_BOARD_SOC1_PCI:
    case EPS_PNDEV_BOARD_SOC1_PCIE:
    case EPS_PNDEV_BOARD_CP1625:

        io_param_out->io_mem.pBase          = p_board_info->crt_slow_mem.base_ptr;
        io_param_out->io_mem.uPhyAddr       = p_board_info->crt_slow_mem.phy_addr;
        io_param_out->io_mem.lSize          = p_board_info->crt_slow_mem.size;

        io_param_out->io_iocc.pBase         = p_board_info->eddi.iocc.base_ptr;
        io_param_out->io_iocc.uPhyAddr      = p_board_info->eddi.iocc.phy_addr;
        io_param_out->io_iocc.lSize         = p_board_info->eddi.iocc.size;

        io_param_out->srd_api_mem.pBase     = p_board_info->srd_api_mem.base_ptr;
        io_param_out->srd_api_mem.uPhyAddr  = p_board_info->srd_api_mem.phy_addr;
        io_param_out->srd_api_mem.lSize     = p_board_info->srd_api_mem.size;

        io_param_out->irte_base.pBase       = p_board_info->eddi.irte.base_ptr;
        io_param_out->irte_base.uPhyAddr    = p_board_info->eddi.irte.phy_addr;
        io_param_out->irte_base.lSize       = p_board_info->eddi.irte.size;

        break;
    default:
        break;
    }
}
#endif

static LSA_VOID eps_ld_upper_open_done( LSA_VOID * pArg, PSI_UPPER_RQB_PTR_TYPE pRQB )
{
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;
    EPS_OPEN_TYPE * pEpsLdOpenOut = &g_pEpsData->uEpsOpen;
    LSA_RESULT retVal;
    LSA_UINT16 rc;
    EPS_SYS_TYPE dummy_sys = {0};
    PSI_UPPER_LD_OPEN_PTR_TYPE pOpen;
    LSA_UINT32 nrOfHD;

    LSA_UINT16 hd_idx;

    LSA_UNUSED_ARG(dummy_sys);

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT( pRQB != 0 );
    
    EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == PSI_OPC_LD_OPEN_DEVICE );
    
    retVal = PSI_RQB_GET_RESPONSE( pRQB );
    if( PSI_OK != retVal )
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_ld_upper_open_done(): ERROR during PSI startup. Details: See trace.");
        pEpsLdOpenOut->retVal = LSA_RSP_ERR_PARAM;
    }
    else
    {
        pEpsLdOpenOut->retVal = LSA_RSP_OK;
    }
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "< eps_ld_upper_open_done(): PSI LD open done");

    pOpen = &((PSI_UPPER_RQB_PTR_TYPE)pRQB)->args.ld_open;

    // Update all INI stores with the calculated values for each PNIO-IF
    nrOfHD = pOpen->hd_count;

    for (hd_idx = 0; hd_idx < nrOfHD; hd_idx++)
    {
        pEpsLdOpenOut->hd_out[hd_idx] = pOpen->hd_out[hd_idx];
        pEpsLdOpenOut->hd_args[hd_idx] = pOpen->hd_args[hd_idx];

        #if ((defined EPS_CFG_USE_EDDI) && (EPS_CFG_USE_EDDI == 1))
        // io_memory to application is only for eddi provided
        if (pEpsLdOpenOut->retVal == LSA_RSP_OK)
        {
            eps_ld_upper_set_io_mem(&pEpsLdOpenOut->io_param_out[hd_idx], pOpen->hd_args[hd_idx].hd_id);
        }
        #endif
    }

    pEpsLdOpenOut->ld_out.supported_comps[PSI_DNS_COMP_IDX]    = pOpen->ld_out.supported_comps[PSI_DNS_COMP_IDX];

    PSI_FREE_LOCAL_MEM(&rc, pRQB, &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
    EPS_ASSERT (rc == LSA_RET_OK);


    eps_set_event(*phDoneEvent);
}

void eps_ld_upper_open(EPS_OPEN_CONST_PTR_TYPE pEpsOpen, LSA_UINT16* phDoneEvent)
{
    LSA_UINT16 iSockDetails;
    LSA_UINT16 idx;

    PSI_UPPER_RQB_PTR_TYPE     pRQB;
    PSI_UPPER_LD_OPEN_PTR_TYPE pOpen;

    LSA_USER_ID_TYPE    dummy_id;
    
    EPS_SYS_TYPE    dummy_sys;
    dummy_id.uvar32 = 0;
    
    g_pEpsData->uEpsOpen = *pEpsOpen;

    PSI_ALLOC_LOCAL_MEM(((void**)&pRQB), dummy_id, sizeof(*pRQB), &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
    EPS_ASSERT(pRQB != NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_open(): Calling PSI with OPC PSI_OPC_LD_OPEN_DEVICE");

    PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_LD_OPEN_DEVICE );
    PSI_RQB_SET_HANDLE( pRQB, 0 );
    PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

    pOpen = &pRQB->args.ld_open;
#if ( EPS_CFG_USE_HIF == 1 )
    pOpen->hH       = g_pEpsData->hUpperLD;
#endif
    pOpen->hd_count = g_pEpsData->hdCount;
    pOpen->im_args  = g_pEpsData->imInput;
    pOpen->ld_in.ld_runs_on = g_pEpsData->ldRunsOnType;

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_open(): hd_count = %d", pOpen->hd_count);

    for( iSockDetails = 0; iSockDetails < PSI_CFG_MAX_SOCK_APP_CHANNELS; iSockDetails++)
    {
        pOpen->sock_args.sock_app_ch_details[iSockDetails] = g_pEpsData->uEpsOpen.sock_args.sock_app_ch_details[iSockDetails];
    }

    for (iSockDetails = 0; iSockDetails < PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS; iSockDetails++)
    {
        pOpen->sock_args.sock_sockapp_ch_details[iSockDetails] = g_pEpsData->uEpsOpen.sock_args.sock_sockapp_ch_details[iSockDetails];
    }

    for ( idx = 0; idx < g_pEpsData->hdCount; idx++ )
    {
        pOpen->hd_args[idx] = g_pEpsData->hdInput[idx];
    }

    pOpen->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if (eps_get_ld_runs_on() != PSI_LD_RUNS_ON_LIGHT)
    {
        // Post thread to HIF
        eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post Thread directly
        eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_open_done, PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_system);
    #endif
    }
    LSA_UNUSED_ARG(dummy_sys);
}

static LSA_VOID eps_ld_upper_close_done(LSA_VOID* pArg, PSI_UPPER_RQB_PTR_TYPE pRQB)
{
    LSA_UINT16 rc;
    EPS_SYS_TYPE dummy_sys = {0};
    LSA_UINT16 *phDoneEvent = (LSA_UINT16*)pArg;

    LSA_UNUSED_ARG(dummy_sys);

    EPS_ASSERT(phDoneEvent != LSA_NULL);
    EPS_ASSERT( pRQB != 0 );
    EPS_ASSERT( PSI_RQB_GET_RESPONSE( pRQB ) == PSI_OK );
    EPS_ASSERT( PSI_RQB_GET_OPCODE( pRQB )   == PSI_OPC_LD_CLOSE_DEVICE );

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "< eps_ld_upper_close_done(): PSI LD close done");

    PSI_FREE_LOCAL_MEM(&rc, pRQB, &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
    EPS_ASSERT (rc == LSA_RET_OK);

    eps_set_event(*phDoneEvent);
}

void eps_ld_upper_close(EPS_CLOSE_CONST_PTR_TYPE pEpsClose, LSA_UINT16* phDoneEvent)
{
    PSI_UPPER_RQB_PTR_TYPE      pRQB;
    PSI_UPPER_LD_CLOSE_PTR_TYPE pClose;

    LSA_USER_ID_TYPE   dummy_id;
    EPS_SYS_TYPE   dummy_sys;
    dummy_id.uvar32 = 0;
    
    g_pEpsData->uEpsClose = *pEpsClose;

    PSI_ALLOC_LOCAL_MEM(((void**)&pRQB), dummy_id, sizeof(*pRQB), &dummy_sys, LSA_COMP_ID_PSI, PSI_MTYPE_UPPER_RQB );
    EPS_ASSERT(pRQB);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_ld_upper_close(): calling PSI with OPC PSI_OPC_LD_CLOSE_DEVICE");

    PSI_RQB_SET_OPCODE( pRQB, PSI_OPC_LD_CLOSE_DEVICE );
    PSI_RQB_SET_HANDLE( pRQB, 0 );
    PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_PSI );

    pClose = &pRQB->args.ld_close;
#if ( EPS_CFG_USE_HIF == 1 )
    pClose->hH                         = g_pEpsData->hUpperLD;
#endif
    pClose->psi_request_upper_done_ptr = (PSI_UPPER_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if (eps_get_ld_runs_on() != PSI_LD_RUNS_ON_LIGHT)
    {
        // Post thread to HIF
        eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_close_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post Thread directly
        eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_ld_upper_close_done, PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_system);
    #endif
    }
    LSA_UNUSED_ARG(dummy_sys);
}

#if ( EPS_CFG_USE_HIF == 1)
LSA_VOID eps_hif_ld_upper_open(EPS_DATA_PTR_TYPE pEpsData, LSA_UINT16* phDoneEvent)
{
    HIF_RQB_PTR_TYPE    pRQB;

    LSA_UINT16          hdCount = pEpsData->hdCount;
    LSA_BOOL            shortcutMode;
    LSA_UINT16          idx;

    EPS_ASSERT((hdCount > 0 ) && ( hdCount <= PSI_CFG_MAX_IF_CNT ));

	HIF_ALLOC_LOCAL_MEM((void**)&pRQB, sizeof(*pRQB), LSA_COMP_ID_PNBOARDS, HIF_MEM_TYPE_RQB );
    EPS_ASSERT(pRQB);

    HIF_RQB_SET_OPCODE( pRQB,  HIF_OPC_LD_UPPER_OPEN );
    HIF_RQB_SET_HANDLE( pRQB,  0 );
    HIF_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_HIF);

    // Prepare the hSysDev for HIF LD upper
    if (PSI_LD_RUNS_ON_LIGHT == g_pEpsData->ldRunsOnType)
    {
        shortcutMode = LSA_TRUE;
    }
    else // the LD is located on a firmware, e.g. Advanced or Basic
    {
        shortcutMode = LSA_FALSE;
    }

    // Prepare the upper hSysDev
    pEpsData->SysUpperLD.hd_nr = 0;
    pEpsData->SysUpperLD.is_upper   = LSA_TRUE;

    if (shortcutMode)
    {
        // Note: for upper no location information is necessary
        pEpsData->SysUpperLD.shortcut_mode    = shortcutMode;
        pEpsData->SysUpperLD.hif_lower_handle = pEpsData->hLowerLD;
    }
    else
    {
        // LD upper connection must be opened over driver, so location info must be added
        pEpsData->SysUpperLD.hd_sys_id.hd_location = g_pEpsData->hdInput[0].hd_location;
    }

    pEpsData->SysUpperLD.ldRunsOnType = g_pEpsData->ldRunsOnType;

    pRQB->args.dev_ld_open.hSysDev  = &pEpsData->SysUpperLD;
    pRQB->args.dev_ld_open.hd_count = pEpsData->hdCount;
    pRQB->args.dev_ld_open.hH       = PSI_INVALID_HANDLE; // out from HIF

    for ( idx = 0; idx < pEpsData->hdCount; idx++ )
    {
        PSI_HD_INPUT_TYPE hdInp = pEpsData->hdInput[idx]; // m_pHwIniStore->GetHdEntry( idx+1 );

        pRQB->args.dev_ld_open.hd_args[idx].hd_id = hdInp.hd_id;
    }

    pRQB->args.dev_ld_open.Cbf = (HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE)eps_system_request_lower_done;

    eps_system_request_lower((LSA_VOID_PTR_TYPE)pRQB, (LSA_VOID_PTR_TYPE)phDoneEvent, (EPS_LOWER_DONE_FCT)eps_hif_ld_upper_open_done, PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system);
}
#endif // ( EPS_CFG_USE_HIF == 1)
#endif  // else (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)

static LSA_VOID eps_system_request_lower_done(LSA_VOID* pRQBData)
{
    struct eps_header *pRQB = (struct eps_header*)pRQBData;
    EPS_RQB_USER_ID_PTR_TYPE pEpsUserId;
    LSA_UINT16 retVal;

    EPS_ASSERT(pRQB != LSA_NULL);

    pEpsUserId = (EPS_RQB_USER_ID_PTR_TYPE) LSA_RQB_GET_USER_ID_PTR(pRQB);

    EPS_ASSERT(pEpsUserId != LSA_NULL);
    EPS_ASSERT(pEpsUserId->_user_func != LSA_NULL);

    pEpsUserId->_user_func(pEpsUserId->_user_arg, pRQBData);

    retVal = eps_mem_free(pEpsUserId, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);

    EPS_ASSERT( LSA_RET_OK == retVal );
    
}

static LSA_VOID eps_system_request_lower(LSA_VOID_PTR_TYPE pRQBData, LSA_VOID_PTR_TYPE pArg, EPS_LOWER_DONE_FCT pFct, LSA_UINT16 mbox_id, PSI_REQUEST_FCT req_fct)
{
    struct eps_header *pRQB = (struct eps_header*)pRQBData;
    EPS_RQB_USER_ID_PTR_TYPE pEpsUserId;

    /* Send Request to HIF System*/

    EPS_ASSERT(pRQB != LSA_NULL);
    EPS_ASSERT(pFct != LSA_NULL);

    pEpsUserId = (EPS_RQB_USER_ID_PTR_TYPE) eps_mem_alloc(sizeof(*pEpsUserId), LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);

    EPS_ASSERT(pEpsUserId != LSA_NULL);

    LSA_RQB_SET_USER_ID_PTR(pRQB, pEpsUserId);

    pEpsUserId->_user_arg  = pArg;
    pEpsUserId->_user_func = pFct;

    psi_request_start( mbox_id, req_fct, pRQBData );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_remove_boards( LSA_VOID )
{
    LSA_INT board;

    // Remove all added PN boards
    for ( board = PSI_CFG_MAX_IF_CNT; board >= 0; board-- )
    {
        eps_hw_close_hd((LSA_UINT16)board);
    }
}

/**
 * initialize the EPS Framework. Calls the OUT-Macro EPS_APP_INIT that the application must provide.
 * @param hSys SystemHandle, not used right now in adonis system
*/
LSA_VOID eps_init( LSA_VOID_PTR_TYPE hSys )
{
    #if ( PSI_CFG_USE_PNTRC == 1 ) /* Setup PSI PNTRC Init Data */
    PNTRC_INIT_TYPE     pntrc_init_struct;
    #endif

    EPS_SHM_OPEN_OPTION_TYPE    sShmOpenOption;
    LSA_UINT16                  retVal;

    g_pEpsData = &g_EpsData;

    eps_memset(g_pEpsData, 0, sizeof(EPS_DATA_TYPE));

    EPS_ASSERT( EPS_STATE_RESET_DONE == eps_get_state() );
    eps_set_state(EPS_STATE_INIT_RUNNING);

    g_pEpsData->bEpsInFatal             = LSA_FALSE;
    g_pEpsData->bBlockingPermitted      = LSA_TRUE;
    g_pEpsData->bShutdown               = LSA_FALSE;
    g_pEpsData->bRunning                = LSA_TRUE;
    g_pEpsData->hSysUpper               = hSys;
    g_pEpsData->bEpsShmIfInitialized    = LSA_FALSE;
    g_pEpsData->bEpsDevIfInitialized    = LSA_FALSE;

    eps_locks_init();   /* Init Eps Critical Section Module */

    #if ( EPS_CFG_USE_HIF == 1 )
    #if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))) // ipc_drv not usable in linux
    eps_ipc_drv_if_init();      /* Init IPC Driver Interface */
    #endif //#if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1)))
    eps_hif_drv_if_init();      /* Init HIF Driver Interface */
    #endif

    eps_pndev_if_init();        /* Init Eps Network Card Driver Interface */

    #if ( EPS_PLF == EPS_PLF_ARM_CORTEX_A15 )
    eps_trace_timer_init(); // init Trace Timer Interface
    #elif ( EPS_PLF == EPS_PLF_PNIP_A53 )
    // future use when HERA Trace Timer is implemented: eps_trace_timer_init(); // init Trace Timer Interface
    #endif

    eps_hw_init();  /* Init HW boards (handles reachable comm boards) */

    EPS_APP_INIT(hSys, &g_pEpsData->HwInfo);   	/* Call the App to initialize the SHM IF Driver and HIF Drivers */

    #if ( EPS_CFG_USE_STATISTICS == 1 )
    eps_statistics_init();  /* Init statistics modules */
    #if ( EPS_PLF != EPS_PLF_WINDOWS_X86 )
    eps_statistics_stack_init();
    #endif
    #endif

    retVal = eps_timer_init_tgroup0();      /* Init LSA Timer System Thread Group 0 */
    EPS_ASSERT(retVal == LSA_RSP_OK);
    retVal = eps_timer_init_tgroup1();      /* Init LSA Timer System Thread Group 1 */
    EPS_ASSERT(retVal == LSA_RSP_OK);

    // at the moment not available for Basic-Variants
	#if ( EPS_PLF == EPS_PLF_WINDOWS_X86 || EPS_PLF == EPS_PLF_ADONIS_X86 || EPS_PLF == EPS_PLF_LINUX_X86 || EPS_PLF == EPS_PLF_LINUX_IOT2000 )
    eps_timer_init_sys_timer(); /* Init System Timer Interface */
    eps_msg_init();             /* Init Msg System */
    #endif

    /* Open Shared Memory Interface */
    eps_memset(&sShmOpenOption, 0, sizeof(sShmOpenOption));
    sShmOpenOption.bEnableWatchdog = LSA_TRUE;
    #if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
    sShmOpenOption.uHdNr = 1;
    #else
    sShmOpenOption.uHdNr = 0;
    #endif

    retVal = eps_shm_if_open(hSys, &sShmOpenOption, &g_pEpsShmHw); /* Open the EPS SHM IF implementation instance using the EPS SHM API */
    EPS_ASSERT(retVal == EPS_SHM_RET_OK);
    EPS_ASSERT(g_pEpsShmHw != LSA_NULL);
    EPS_PLF_REGISTER_SHM_IF(g_pEpsShmHw);

    eps_store_fatal_info(g_pEpsShmHw, LSA_NULL); /* Init Fatal Buffer */

    if (g_pEpsShmHw->bShmHalt)
    {
        // use fatal hook to halt on startup when shared memory debugging boards
        EPS_APP_HALT();
    }

    /* Init Local Memory Management with standard and fast mem pools and configure eps user pools */
    retVal = eps_mem_init( (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(g_pEpsData->HwInfo.LocalMemPool.pBase), 
                           g_pEpsData->HwInfo.LocalMemPool.lSize, 
                           (LSA_UINT32*)EPS_CAST_TO_VOID_PTR(g_pEpsData->HwInfo.FastMemPool.pBase), 
                           g_pEpsData->HwInfo.FastMemPool.lSize );
    EPS_ASSERT(retVal == LSA_RSP_OK);

    eps_events_init();      /* Init Events */

    retVal = psi_init();    /* init PSI (LD/HD/Mailbox) / must be done before eps_tasks_init() */
    EPS_ASSERT(retVal == LSA_RET_OK);
    
    eps_cp_mem_init();      /* Init Communication (HIF-/NRT-/PI-) Memory Management */

    eps_tasks_init();       /* Init PNIO threads and Thread System */

    /* Init PNTRC */
    #if ( PSI_CFG_USE_PNTRC == 1 ) /* Setup PSI PNTRC Init Data */
    pntrc_init_struct.bIsTopMostCPU       = g_pEpsShmHw->sPntrcParam.bIsTopMostCPU;
    pntrc_init_struct.bUseInitLogLevels   = g_pEpsShmHw->sPntrcParam.bUseInitLogLevels;
    pntrc_init_struct.pInitLogLevels      = g_pEpsShmHw->sPntrcParam.pInitLogLevels;
    pntrc_init_struct.Size                = g_pEpsShmHw->sPntrc1.uSize;
    pntrc_init_struct.Ptr		          = (LSA_UINT8*)g_pEpsShmHw->sPntrc1.pBase;

    retVal = pntrc_init(&pntrc_init_struct);
    EPS_ASSERT(retVal == LSA_RET_OK);
    
    pntrc_startstop(1, g_pEpsShmHw);
    #endif

    g_Eps_TraceInFatalDone = LSA_FALSE;
    retVal = eps_alloc_critical_section_prio_protected(&g_Eps_fatal_in_fatal_enter_exit_id, LSA_TRUE); // recursive lock
    EPS_ASSERT(retVal == LSA_RET_OK);

    #if ( PSI_CFG_USE_EPS_RQBS == 1 )
    retVal = eps_init_rqb_interface();
    EPS_ASSERT(EPS_OK == retVal);
    #endif

    #if ( EPS_CFG_USE_STATISTICS == 1 )
    #if ( EPS_PLF != EPS_PLF_WINDOWS_X86 )
    eps_statistics_stack_cyclic_start(500);  // Thread every 500 msec
    #endif
    eps_statistics_trace_cyclic_start(500);  // Thread every 500 msec
    #endif

    // BUG - Timer only works after 2nd call - workaround: Call it once at startup
    {
        struct timespec timerStart;
        int retValTime;
        retValTime = EPS_POSIX_CLOCK_GETTIME( CLOCK_REALTIME, &timerStart );
        LSA_UNUSED_ARG(retValTime);
    }

    eps_set_state(EPS_STATE_INIT_DONE);

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<-- eps_init(): EPS-State=%u", eps_get_state());
}

/*
   eps open()

   pEpsOpen

   pCbf:        If a function ptr was given -> asynchron, else it was waiting -> synchron

   EPS variant:
    * PSI_LD_RUNS_ON_LIGHT        - PCIOX_Light, PCIOX with HD@FW, PNDriver. Used to set short circuit mode
    * PSI_LD_RUNS_ON_ADVANCED     - PCIOX_Advanced, Used to open a LD Upper IF to the Advanced FW using VDD
    * PSI_LD_RUNS_ON_BASIC        - PCIOX_Basic, Used to open an LD Upper IF to the Basic FW
*/

LSA_VOID eps_open(
    EPS_OPEN_PTR_TYPE                   pEpsOpen, 
    EPS_UPPER_CALLBACK_FCT_PTR_TYPE     pCbf, 
    LSA_UINT16                          TimeOutSec)
{
    LSA_UINT16  retVal;
    LSA_UINT16  eps_startup_proceed_event;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "--> eps_open()");

    EPS_ASSERT( (EPS_STATE_INIT_DONE == eps_get_state()) || (EPS_STATE_CLOSE_DONE == eps_get_state()) );
    eps_set_state(EPS_STATE_OPEN_CALLED);

    #ifdef EPS_LOCAL_PN_USER
    {
        LSA_UINT16  idx;

        EPS_ASSERT(pEpsOpen->ldRunsOnType != PSI_LD_RUNS_ON_UNKNOWN);
        EPS_ASSERT(pEpsOpen->ldRunsOnType < PSI_LD_RUNS_ON_MAX);

        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): ldRunsOnType=%d (1-Light, 2-Advanced, 3-Basic), EPS-State=%u", pEpsOpen->ldRunsOnType, eps_get_state());

        #if (defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT))
        if (pEpsOpen->ldRunsOnType != PSI_LD_RUNS_ON_LIGHT)
        {
            EPS_FATAL("eps_open(): In HSM is only light (ldRunsOnType = PSI_LD_RUNS_ON_LIGHT) allowed!!!");
        }
        #endif

        g_pEpsData->ldRunsOnType    = pEpsOpen->ldRunsOnType;
        g_pEpsData->bDebug          = pEpsOpen->fw_debug_activated;
        g_pEpsData->bShmDebug       = pEpsOpen->shm_debug_activated;
        g_pEpsData->bShmHalt	    = pEpsOpen->shm_halt_activated;

        EPS_ASSERT((pEpsOpen->hd_count > 0 ) && ( pEpsOpen->hd_count <= PSI_CFG_MAX_IF_CNT ));
        g_pEpsData->hdCount = (LSA_UINT16)pEpsOpen->hd_count;

        for ( idx = 0; idx < g_pEpsData->hdCount; idx++ )
        {
            #if (defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT) || defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT))
            if (pEpsOpen->hd_args[idx].hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
                EPS_FATAL("eps_open(): In HSM <hd_runs_on_level_ld> == NO is NOT allowed!!!");
            }
            #endif

            if ((pEpsOpen->ldRunsOnType == PSI_LD_RUNS_ON_BASIC) && (pEpsOpen->hd_args[idx].hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO))
            {
                EPS_FATAL("eps_open(): invalid combination of <ldRunsOnType=3> and <hd_runs_on_level_ld=0>");
            }

            g_pEpsData->hdInput[idx] = pEpsOpen->hd_args[idx]; // save HD input
        }

        g_pEpsData->imInput = pEpsOpen->imInput;
    }
    #endif

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): called with TimeOutSec value = %d (0=Infinite), EPS-State=%u", TimeOutSec, eps_get_state());

    //Install drivers -> Call to EPS APP IF (e.g. pcIOX, PNDriver, TestApp)
    EPS_APP_INSTALL_DRV_OPEN(pEpsOpen->ldRunsOnType);
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): EPS_APP_INSTALL_DRV_OPEN done");

    //Get all Devices
    retVal = eps_shm_update_device_list();
    LSA_UNUSED_ARG(retVal);

    /* Init Interrupt Handler init (eps_isr) - no init function required */

    retVal = eps_alloc_event(&eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);

    // *** 1. OPEN HIF LD LOWER -> Enable HIF receive ****************************************************
    #if ( EPS_CFG_USE_HIF == 1)
    #if (( EPS_CFG_USE_HIF_LD == 1 ) || ( EPS_CFG_USE_HIF_HD == 1 ))
    eps_set_state(EPS_STATE_OPEN_DOING_HIF_LOWER_OPEN);
    #if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
    #if ( EPS_CFG_USE_HIF_HD == 1 )
    eps_hif_hd_lower_open(g_pEpsData, &eps_startup_proceed_event);      /* Init HIF HD Lower Device */
    #endif //( EPS_CFG_USE_HIF_HD == 1 )
    #else
    #if ( EPS_CFG_USE_HIF_LD == 1 )
    eps_hif_ld_lower_open(g_pEpsData, &eps_startup_proceed_event);      /* Init HIF LD Lower Device */
    #endif //( EPS_CFG_USE_HIF_LD == 1 )
    #endif // (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
    retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
    eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): HIF HD/LD Lower open done, EPS-State=%u", eps_get_state());
    #endif //(( EPS_CFG_USE_HIF_LD == 1 ) || ( EPS_CFG_USE_HIF_HD == 1 ))
    #endif // ( EPS_CFG_USE_HIF == 1)
    // *************************************************************************************************

    // *** 2. OPEN HIF LD UPPER ************************************************************************
    #ifdef EPS_LOCAL_PN_USER
    #if ( EPS_CFG_USE_HIF == 1)
    #if ( EPS_CFG_USE_HIF_LD == 1 )
    eps_set_state(EPS_STATE_OPEN_DOING_HIF_UPPER_OPEN);
    eps_hif_ld_upper_open(g_pEpsData, &eps_startup_proceed_event);
    retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
    eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): HIF LD Upper open done, EPS-State=%u", eps_get_state());
    #endif //( EPS_CFG_USE_HIF_LD == 1 )
    #endif // ( EPS_CFG_USE_HIF == 1)
    #endif
    // **************************************************************************************************

    // *** 3. OPEN LD UPPER *****************************************************************************
    #ifdef EPS_LOCAL_PN_USER
    eps_set_state(EPS_STATE_OPEN_DOING_PSI_OPEN);
    eps_ld_upper_open(pEpsOpen, &eps_startup_proceed_event);
    retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
    eps_memcpy(pEpsOpen, &g_pEpsData->uEpsOpen, sizeof(EPS_OPEN_TYPE));
    eps_reset_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    
    // *** 4. Check if PN Stack was opened successfully
    if (LSA_RET_OK != pEpsOpen->retVal)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): ERROR during PSI startup. All the actions from eps_open are undone now:");
 
        // *** 4.1. CLOSE HIF LD UPPER ***********************************************************************
        #if ( EPS_CFG_USE_HIF_LD == 1 )
        eps_set_state(EPS_STATE_CLOSE_DOING_HIF_UPPER_CLOSE);
        eps_hif_ld_upper_close(g_pEpsData, &eps_startup_proceed_event);
        retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
        eps_reset_event(eps_startup_proceed_event);
        EPS_ASSERT(retVal == LSA_RET_OK);
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): HIF LD Upper close done, EPS-State=%u", eps_get_state());
        #endif // ( EPS_CFG_USE_HIF_LD == 1 )
        // *************************************************************************************************

        // *** 4.2. CLOSE HIF LD LOWER -> Disable HIF receive ************************************************
        #if (( EPS_CFG_USE_HIF_LD == 1 ) || ( EPS_CFG_USE_HIF_HD == 1 ))
        eps_set_state(EPS_STATE_CLOSE_DOING_HIF_LOWER_CLOSE);
        #if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
        #if ( EPS_CFG_USE_HIF_HD == 1 )
        eps_hif_hd_lower_close(g_pEpsData, eps_startup_proceed_event);      /* Undo Init HIF HD Lower Device */
        #endif // ( EPS_CFG_USE_HIF_HD == 1 )
        #else
        #if ( EPS_CFG_USE_HIF_LD == 1 )
        eps_hif_ld_lower_close(g_pEpsData, &eps_startup_proceed_event);      /* Undo Init HIF LD Lower Device */
        #endif // ( EPS_CFG_USE_HIF_LD == 1 )
        #endif // (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
        retVal = eps_wait_event(eps_startup_proceed_event, TimeOutSec);
        eps_reset_event(eps_startup_proceed_event);
        EPS_ASSERT(retVal == LSA_RET_OK);
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): HIF LD/HD Lower close done, EPS-State=%u", eps_get_state());
        #endif
        // *************************************************************************************************

        eps_set_state(EPS_STATE_CLOSE_DOING_REMOVE_BOARDS);
        eps_remove_boards();
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): removed all boards, EPS-State=%u", eps_get_state());
    }
    else
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): LD Upper open done, EPS-State=%u", eps_get_state());
        eps_set_state(EPS_STATE_RUNNING);
    }
    #else
    eps_set_state(EPS_STATE_RUNNING);
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open() - EPS now running, EPS-State=%u", eps_get_state());    
    #endif

    retVal = eps_free_event(eps_startup_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);

    #if ( EPS_PLF == EPS_PLF_SOC_MIPS )
    #if ( EPS_TGROUP_STATISTICS == EPS_YES )
    eps_tgroup_quota_statistics_startstop(LSA_TRUE);
    #endif
    #endif

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<-- eps_open()");

    #ifdef EPS_LOCAL_PN_USER
    if (pCbf) //If pEpsLdOpen == LSA_NULL then do nothing
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_open(): calling application, EPS-State=%u", eps_get_state());
        //Asynchron
        ((EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(pCbf))(&g_pEpsData->uEpsOpen,  g_pEpsData->uEpsOpen.pUsrHandle);
    }
    #else
    LSA_UNUSED_ARG(pCbf);
    #endif
}

LSA_VOID eps_close(
    EPS_CLOSE_PTR_TYPE                  pEpsClose, 
    EPS_UPPER_CALLBACK_FCT_PTR_TYPE     pCbf, 
    LSA_UINT16                          TimeOutSec)
{
    LSA_UINT16                          retVal;
    EPS_UPPER_CALLBACK_FCT_PTR_TYPE     pEpsLdClose = (EPS_UPPER_CALLBACK_FCT_PTR_TYPE)(pCbf);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "--> eps_close()");

    EPS_ASSERT( EPS_STATE_RUNNING == eps_get_state() );

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): called with TimeOutSec value = %d (0=Infinite), EPS-State=%u", TimeOutSec, eps_get_state());

    retVal = eps_alloc_event(&pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);

    // *** 1. CLOSE LD UPPER ***************************************************************************
    #ifdef EPS_LOCAL_PN_USER
    eps_set_state(EPS_STATE_CLOSE_DOING_PSI_CLOSE);
    eps_ld_upper_close(pEpsClose, &pEpsClose->eps_proceed_event);
    retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
    eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): PSI LD Upper close done, EPS-State=%u", eps_get_state());
    #endif

    // *** 2. CLOSE HIF LD UPPER ***********************************************************************
    #ifdef EPS_LOCAL_PN_USER
    #if ( EPS_CFG_USE_HIF_LD == 1 )
    eps_set_state(EPS_STATE_CLOSE_DOING_HIF_UPPER_CLOSE);
    eps_hif_ld_upper_close(g_pEpsData, &pEpsClose->eps_proceed_event);
    retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
    eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): HIF LD Upper close done, EPS-State=%u", eps_get_state());
    #endif // ( EPS_CFG_USE_HIF_LD == 1 )
    #endif
    // *************************************************************************************************

    // *** 3. CLOSE HIF LD LOWER -> Disable HIF receive ************************************************
    #if (( EPS_CFG_USE_HIF_LD == 1 ) || ( EPS_CFG_USE_HIF_HD == 1 ))
    eps_set_state(EPS_STATE_CLOSE_DOING_HIF_LOWER_CLOSE);
    #if (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
    #if ( EPS_CFG_USE_HIF_HD == 1 )
    eps_hif_hd_lower_close(g_pEpsData, &pEpsClose->eps_proceed_event);      /* Undo Init HIF HD Lower Device */
    #endif // ( EPS_CFG_USE_HIF_HD == 1 )
    #else
    #if ( EPS_CFG_USE_HIF_LD == 1 )
    eps_hif_ld_lower_close(g_pEpsData, &pEpsClose->eps_proceed_event);      /* Undo Init HIF LD Lower Device */
    #endif // ( EPS_CFG_USE_HIF_LD == 1 )
    #endif // (PSI_CFG_USE_LD_COMP == 0 && PSI_CFG_USE_HD_COMP == 1)
    retVal = eps_wait_event(pEpsClose->eps_proceed_event, TimeOutSec);
    eps_reset_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): HIF LD/HD Lower close done, EPS-State=%u", eps_get_state());
    #endif
    // *************************************************************************************************

    eps_set_state(EPS_STATE_CLOSE_DOING_REMOVE_BOARDS);
    eps_remove_boards();
    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): removed all boards, EPS-State=%u", eps_get_state());

    retVal = eps_free_event(pEpsClose->eps_proceed_event);
    EPS_ASSERT(retVal == LSA_RET_OK);

    #if ( EPS_PLF == EPS_PLF_SOC_MIPS )
    #if ( EPS_TGROUP_STATISTICS == EPS_YES )
    eps_tgroup_quota_statistics_startstop(LSA_FALSE);
    #endif
    #endif

    EPS_APP_UNINSTALL_DRV_CLOSE(g_pEpsData->hSysUpper); /* eg. uninstall pndevdrv interfaces */

    eps_set_state(EPS_STATE_CLOSE_DONE);

    if (pEpsLdClose)
    {
        //Asynchron
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_close(): calling Application, EPS-State=%u", eps_get_state());
        pEpsLdClose(pEpsClose, g_pEpsData->uEpsClose.pUsrHandle);
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<-- eps_close()");
}

LSA_VOID eps_undo_init( LSA_VOID )
{
    LSA_BOOL    bResult;
    LSA_UINT16  retVal;

    EPS_ASSERT(g_pEpsData != LSA_NULL);

    EPS_ASSERT( (EPS_STATE_CLOSE_DONE == eps_get_state()) || (EPS_STATE_INIT_DONE == eps_get_state()) );
    eps_set_state(EPS_STATE_UNDO_INIT_RUNNING);

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_undo_init(): stopping system, EPS-State=%u", eps_get_state());

    g_pEpsData->bShutdown = LSA_TRUE;

    EPS_ASSERT(g_pEpsShmHw != LSA_NULL);
    retVal = g_pEpsShmHw->AppShutdown(g_pEpsShmHw);
    EPS_ASSERT(EPS_SHM_RET_OK == retVal);

    #if ( EPS_CFG_USE_STATISTICS == 1 )
    #if ( EPS_PLF != EPS_PLF_WINDOWS_X86 )
    eps_statistics_stack_cyclic_stop();
    #else
	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_undo_init(): eps_statistics_stack - stack statistics not available in windows.");
    #endif
    eps_statistics_trace_cyclic_stop();
    #endif

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_undo_init(): trace system is now inactive for this instance");
    #if ( PSI_CFG_USE_PNTRC == 1 )
    pntrc_startstop(0, LSA_NULL);
    #endif

    #if ( PSI_CFG_USE_EPS_RQBS == 1 )
    retVal = eps_undo_init_rqb_interface();
    EPS_ASSERT(EPS_OK == retVal);
    #endif

    retVal = psi_undo_init();           /* undo init PSI (LD/HD/Mailbox) => will end threads / must be done before eps_tasks_undo_init() */
    EPS_ASSERT(LSA_RET_OK == retVal);
    
    eps_tasks_undo_init();              /* check if all threads are down? */

    eps_cp_mem_undo_init();             /* close cp memory pools */
    bResult = eps_mem_check_pool();     /* check if there are still allocated blocks. Must be done before pntrc_undo_init since there are important traces */
    LSA_UNUSED_ARG(bResult);
    
    #if ( PSI_CFG_USE_PNTRC == 1 )
    retVal = pntrc_undo_init();
    EPS_ASSERT(LSA_RET_OK == retVal);
    #endif

    retVal = eps_free_critical_section(g_Eps_fatal_in_fatal_enter_exit_id);
    EPS_ASSERT(retVal == LSA_RET_OK);
    g_Eps_fatal_in_fatal_enter_exit_id = EPS_LOCK_ID_INVALID;

    /* Close drivers => needs to be done now as they might use timers/events/locks/allocs */
    #if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))) // ipc_drv not usable in linux
    eps_ipc_drv_if_close_all();
    #endif //#if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1)))

    retVal = eps_shm_if_close(g_pEpsShmHw);
    EPS_ASSERT(EPS_SHM_RET_OK == retVal);
    
    eps_shm_if_undo_init();

    #if ( EPS_PLF == EPS_PLF_ARM_CORTEX_A15 )
    eps_trace_timer_undo_init(); // remove Trace Timer Interface
    #elif ( EPS_PLF == EPS_PLF_PNIP_A53 )
    // future use when HERA Trace Timer is implemented: eps_trace_timer_undo_init(); // remove Trace Timer
    #endif

    #if (EPS_CFG_USE_HIF == 1)
    eps_hif_drv_if_undo_init();
    
    #if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1))) // ipc_drv not usable in linux
    eps_ipc_drv_if_undo_init();
    #endif //#if !(defined(_TEST) && ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_SOC1)))

    #endif

    // at the moment not available for Basic-Variants
	#if (EPS_PLF == EPS_PLF_WINDOWS_X86 || EPS_PLF == EPS_PLF_ADONIS_X86 || EPS_PLF == EPS_PLF_LINUX_X86 || EPS_PLF == EPS_PLF_LINUX_IOT2000)
    eps_msg_undo_init();                /* No more msgs */
    eps_timer_undo_init_sys_timer();    /* No more system timer */
    #endif

    eps_events_undo_init();             /* No more events */
    eps_hw_undo_init();                 /* No more allocs */
    eps_timer_undo_init_tgroup0();      /* No more timers Thread Group 0 */
    eps_timer_undo_init_tgroup1();      /* No more timers Thread Group 1 */
   
    #if ( EPS_CFG_USE_STATISTICS == 1 )
    eps_statistics_undo_init();
    #if ( EPS_PLF != EPS_PLF_WINDOWS_X86 )
    eps_statistics_stack_undo_init();
    #endif
    #endif

    eps_mem_undo_init();        /* No more allocs */

    EPS_APP_UNDO_INIT(g_pEpsData->hSysUpper, &g_pEpsData->HwInfo);

    eps_pndev_if_undo_init();   /* Undo init Eps Network Card Driver Interface */

    eps_locks_undo_init();      /* No more locks */

    eps_set_state(EPS_STATE_UNDO_INIT_DONE);

    g_pEpsData->bRunning = LSA_FALSE;

    g_pEpsData = LSA_NULL;
}

#if (EPS_PLF == EPS_PLF_ADONIS_X86)
#define EPS_ADVANCED_REBOOT_STACK_SIZE 8192
// thread vars
typedef struct _eps_advanced_reboot
{
    LSA_UINT32      lThIdAdvRebootShutdownStackAdr[EPS_ADVANCED_REBOOT_STACK_SIZE];
    LSA_UINT32      lThIdAdvRebootMainStackAdr[EPS_ADVANCED_REBOOT_STACK_SIZE];
    pthread_t       lThIdAdvRebootShutdown;
    pthread_t       lThIdAdvRebootMain;

    LSA_UINT32      bGlobIsEPSReboot;
} EPS_ADVANCED_REBOOT_TYPE;
static EPS_ADVANCED_REBOOT_TYPE g_EpsAdvancedReboot = {{0},{0},0,0,0};

/**
 * creates a new thread in adonis
 * uses directly the POSIX API, no EPS API available !
 *
 * @param lStackSizeIn
 * @param lPriorityIn
 * @param sThreadNameIn
 * @param sThreadNumberArgument
 * @param pThreadFunction
 * @param lStackAdrIn
 * @return
 */
static pthread_t eps_advanced_reboot_create_thread( LSA_UINT32        lStackSizeIn,
                                                    LSA_UINT32        lPriorityIn,
                                                    const _TCHAR*     sThreadNameIn,
                                                    _TCHAR            sThreadNumberArgument,
                                                    void*             pThreadFunction,
                                                    LSA_UINT32*       lStackAdrIn )
{
    // thread var
    pthread_attr_t      uThAttributes;
    struct sched_param  uThParams;
    pthread_t           lThId         = 0;
    pthread_t           lResult       = 0;
    UINT32              lThResult     = 0;
    _TCHAR              sThArgument   = { 0 };

    // set attributes to default
    lThResult = EPS_POSIX_PTHREAD_ATTR_INIT(&uThAttributes);

    if  (0 != lThResult)
    // default is set
    {
        // error
    }
    else
    {
        // enable setting individual params
        EPS_POSIX_PTHREAD_ATTR_SETINHERITSCHED( &uThAttributes, PTHREAD_EXPLICIT_SCHED);

        // stack
        lThResult = EPS_POSIX_PTHREAD_ATTR_SETSTACK (&uThAttributes, (void*) lStackAdrIn, lStackSizeIn*4); // 32k

        if  (0 != lThResult)
        // default is set
        {
            // error
        }
        else
        {
            // set prio
            uThParams.sched_priority = lPriorityIn;

            // set attributes and params
            lThResult = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPARAM( &uThAttributes, &uThParams);

            if  (0 != lThResult)
            // default is set
            {
                // error
            }
            else
            {
                // set scheduling policy to RoundRobin
                lThResult = EPS_POSIX_PTHREAD_ATTR_SETSCHEDPOLICY(&uThAttributes, SCHED_RR);

                if  (0 != lThResult)
                // default is set
                {
                    // error
                }
                else
                {
                    // set thread name
                    lThResult = pthread_attr_setname( &uThAttributes, sThreadNameIn);

                    if  (0 != lThResult)
                    // default is set
                    {
                        // error
                    }
                    else
                    {
                        // set detach state
                        lThResult = pthread_attr_setdetachstate(&uThAttributes, PTHREAD_INHERIT_SCHED);

                        if  (0 != lThResult)
                        // default is set
                        {
                            // error
                        }
                        else
                        {
                            // set start argument
                            sThArgument = sThreadNumberArgument;

                            // create thread
                            lThResult = EPS_POSIX_PTHREAD_CREATE( &lThId, &uThAttributes, pThreadFunction, &sThArgument);

                            if  (0 != lThResult)
                            // default is set
                            {
                                // error
                                #ifdef EPS_IMCEADRV_PRINTF
                                printf("\nERROR - Thread not created: %s\n", sThreadNameIn);
                                #endif
                            }
                            else
                            {
                                #ifdef EPS_IMCEADRV_PRINTF
                                printf("\nThread created: %s", sThreadNameIn);
                                #endif

                                lResult = lThId;
                            }
                        }
                    }
                }
            }
        }
    }

    return lResult;
}

/**
 * starts the eps
 */
static LSA_VOID eps_advanced_reboot_startup( LSA_VOID )
{
    EPS_OPEN_TYPE uOpen;

    EPS_ASSERT(LSA_TRUE == g_EpsAdvancedReboot.bGlobIsEPSReboot);

    PSI_MEMSET(&uOpen, 0, sizeof(EPS_OPEN_TYPE));

    uOpen.hd_count      = 1;
    uOpen.pUsrHandle    = LSA_NULL;
    uOpen.ldRunsOnType  = PSI_LD_RUNS_ON_UNKNOWN;

    /* EPS entry point */
    eps_init(LSA_NULL ); /* hSys and ldRunsOnType are not used in Adonis Targets*/
    eps_open(&uOpen, LSA_NULL, PSI_EVENT_NO_TIMEOUT); /*pCbf are not used in Adonis Targets*/

    // reset the reboot flag
    g_EpsAdvancedReboot.bGlobIsEPSReboot = LSA_FALSE;
}

/**
 * second part of the shutdown - is called by eps_advanced_reboot_shutdown()
 *
 * does the eps_undo_init()
 * afterwards create a new thread for startup
 */
static LSA_VOID eps_advanced_reboot_shutdown_cbf( LSA_VOID )
{
    #ifdef EPS_IMCEADRV_PRINTF
    printf("\neps_advanced_reboot_cbf() - eps_close() done");
    #endif
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_advanced_reboot_cbf() - eps_close() done");

    eps_undo_init();
    #ifdef EPS_IMCEADRV_PRINTF
    printf("\neps_advanced_reboot_cbf() - eps_undo_init() done");
    #endif
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_advanced_reboot_cbf() - eps_undo_init() done");

    // starting the main thread again to start the EPS
    g_EpsAdvancedReboot.lThIdAdvRebootMain = eps_advanced_reboot_create_thread(     EPS_ADVANCED_REBOOT_STACK_SIZE,
                                                                RT_P1,
                                                                "EPS_REBOOT_UP",
                                                                '0',
                                                                eps_advanced_reboot_startup,
                                                                g_EpsAdvancedReboot.lThIdAdvRebootMainStackAdr);

    #ifdef EPS_IMCEADRV_PRINTF
    printf("\n\n########################## EPS ADN REBOOT - DONE ##########################");
    printf("\n###########################################################################\n");
    #endif
}

/**
 * first part of the eps shutdown - eps_close()
 */
static LSA_VOID eps_advanced_reboot_shutdown( LSA_VOID )
{
    EPS_CLOSE_TYPE uClose;
    eps_close( &uClose, (EPS_UPPER_CALLBACK_FCT_PTR_TYPE)eps_advanced_reboot_shutdown_cbf, PSI_EVENT_NO_TIMEOUT);
}

/**
 * initialize the reboot of Adonis FW
 */
LSA_VOID eps_advanced_init_reboot ( LSA_VOID )
{
    // set the reboot flag
    EPS_ASSERT(LSA_FALSE == g_EpsAdvancedReboot.bGlobIsEPSReboot);
    g_EpsAdvancedReboot.bGlobIsEPSReboot = LSA_TRUE;

    // memset the stacks
    eps_memset(g_EpsAdvancedReboot.lThIdAdvRebootShutdownStackAdr,  0, EPS_ADVANCED_REBOOT_STACK_SIZE);
    eps_memset(g_EpsAdvancedReboot.lThIdAdvRebootMainStackAdr,      0, EPS_ADVANCED_REBOOT_STACK_SIZE);

    #ifdef EPS_IMCEADRV_PRINTF
    printf("\n###########################################################################");
    printf("\n############################# EPS ADN REBOOT ##############################\n");
    #endif

    // starting the thread to shutdown the EPS
    g_EpsAdvancedReboot.lThIdAdvRebootShutdown = eps_advanced_reboot_create_thread( EPS_ADVANCED_REBOOT_STACK_SIZE,
                                                                RT_P1,
                                                                "EPS_REBOOT_DOWN",
                                                                '0',
                                                                eps_advanced_reboot_shutdown,
                                                                g_EpsAdvancedReboot.lThIdAdvRebootShutdownStackAdr);
}

/**
 * returns the global variable bGlobIsEPSReboot to check if reboot is active
 *
 * @return bGlobIsEPSReboot
 */
LSA_UINT32 eps_advanced_reboot_is_active( LSA_VOID )
{
    return g_EpsAdvancedReboot.bGlobIsEPSReboot;
}
#endif

#ifdef EPS_LOCAL_PN_USER
/*---------------------------------------------------------------------------*/
/* Methodes from EPS user core to PN Stack messaging                         */
/*---------------------------------------------------------------------------*/

LSA_VOID eps_user_system_request_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct )
{
    /* add the user instance and CBF in extended RQB Header */
    /* note: information is used for addressing response receiver */
    EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE*)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
    EPS_ASSERT( p_inst  != 0 );
    EPS_ASSERT( p_fct   != 0 );

    EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
    EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if ((eps_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (eps_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
    {
        // Post message to HIF LD
        psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_system, rqb_ptr );
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post message to PSI LD
        psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_system, rqb_ptr );
    #endif
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_open_channel_lower( LSA_VOID_PTR_TYPE  rqb_ptr, LSA_VOID_PTR_TYPE  p_inst, EPS_LOWER_DONE_FCT  p_fct  )
{
    /* add the user instance and CBF in extended RQB Header */
    /* note: information is used for addressing response receiver */
    EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
    EPS_ASSERT( p_inst  != 0 );
    EPS_ASSERT( p_fct   != 0 );

    g_pEpsData->iOpenUserChannels++;

    EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
    EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if ((eps_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (eps_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
    {
        // Post message to HIF LD
        psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_open_channel, rqb_ptr );
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post message to PSI LD
        psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_open_channel, rqb_ptr );
    #endif
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_close_channel_lower( LSA_VOID_PTR_TYPE rqb_ptr, LSA_VOID_PTR_TYPE p_inst, EPS_LOWER_DONE_FCT p_fct )
{
    /* add the user instance and CBF in extended RQB Header */
    /* note: information is used for addressing response receiver */
    EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
    EPS_ASSERT( p_inst  != 0 );
    EPS_ASSERT( p_fct   != 0 );

    EPS_ASSERT(g_pEpsData->iOpenUserChannels != 0);
    g_pEpsData->iOpenUserChannels--;

    EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
    EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if ((eps_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (eps_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
    {
        // Post message to HIF LD
        psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_close_channel, rqb_ptr );
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post message to PSI LD
        psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_close_channel, rqb_ptr );
    #endif
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID eps_user_request_lower( LSA_VOID_PTR_TYPE rqb_ptr, LSA_VOID_PTR_TYPE p_inst, EPS_LOWER_DONE_FCT p_fct )
{
    /* add the user instance and CBF in extended RQB Header */
    /* note: information is used for addressing response receiver */
    EPS_RQB_TYPE * pRQB = (EPS_RQB_TYPE *)rqb_ptr;

    EPS_ASSERT( rqb_ptr != 0 );
    EPS_ASSERT( p_inst  != 0 );
    EPS_ASSERT( p_fct   != 0 );

    EPS_RQB_SET_USER_INST_PTR( pRQB, p_inst );
    EPS_RQB_SET_USER_CBF_PTR( pRQB, p_fct );

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_user_request_lower(): pRQB(0x%08x) p_inst(0x%08x) p_fct(0x%08x)", rqb_ptr, p_inst, p_fct);

    #if ( EPS_CFG_USE_HIF_LD == 1 )
    if ((eps_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (eps_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
    {
        // Post message to HIF LD
        psi_request_start( PSI_MBX_ID_HIF_LD, (PSI_REQUEST_FCT)hif_ld_u_request, rqb_ptr );
    }
    else
    #endif
    {
    #if (PSI_CFG_USE_LD_COMP == 1)
        // Post message to PSI LD
        psi_request_start( PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_ld_request, rqb_ptr );
    #endif
    }
}
#endif

LSA_UINT32 eps_get_ticks_1ms(LSA_VOID)
{   
    // CLOCK_REALTIME/gettimeofday() should not be used to measure time, use CLOCK_MONOTONIC instead. -> eps_get_ticks_100ns() uses CLOCK_MONOTONIC.
    // Calculate from 100 ns to 1 milliseconds ticks, don't lose percision here -> divide with UIN64 type!
    LSA_UINT64 const ticks_1ms = eps_get_ticks_100ns() / 10000;
    
    return (LSA_UINT32)ticks_1ms;
}

LSA_UINT64 eps_get_ticks_100ns(LSA_VOID)
{
    LSA_UINT64 ticks_100ns;

#if (EPS_PLF == EPS_PLF_SOC_MIPS)
    LSA_UINT32 val_l, val_h;
    // read TTU-timer   -> 56 bit timer
    val_l = *((uint32_t*)(0x16000000 + 0x20));      ///< read low  32 bit (offset 0x20)
    val_h = *((uint32_t*)(0x16000000 + 0x28));      ///< read high 24 bit (offset 0x28)
    ticks_100ns = ((((LSA_UINT64)val_h) << 32) | ((LSA_UINT64)val_l & 0xFFFFFFFF)) * 10;
#else
    struct timespec my_time;
    int             ret_val;

    // CLOCK_REALTIME should not be used to measure time, use CLOCK_MONOTONIC instead.
    ret_val = EPS_POSIX_CLOCK_GETTIME(CLOCK_MONOTONIC, &my_time);
    EPS_ASSERT(0 == ret_val);

    ticks_100ns = ((LSA_UINT64)my_time.tv_sec * 10000000ULL);
    ticks_100ns += ((LSA_UINT64)(LSA_INT64)my_time.tv_nsec / 100ULL);
#endif

    return ticks_100ns;
}

/*------------------------------------------------------------------------------
//	stdlib
//----------------------------------------------------------------------------*/
LSA_INT eps_sprintf( LSA_UINT8 * buffer, const char * format, ... )
{
    LSA_INT rc;
    //lint -save -e10 -e40 -e534 va_list and va_start unknown to lint (VS2015 adaptation not done for current lint version)
    va_list argptr;

    va_start(argptr, format);
    rc = vsprintf((char*)buffer, format, argptr);
    va_end(argptr);
    //lint -restore
    return rc;
}

LSA_INT eps_vsprintf( LSA_UINT8 * buffer, const char * format, va_list argptr )
{
    LSA_INT rc;
    rc = vsprintf((char*)buffer, format, argptr);
    return rc;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

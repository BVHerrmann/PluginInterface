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
/*  F i l e               &F: eps_isr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Interrupt adaption                                                   */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20007
#define EPS_MODULE_ID      20007

/* EPS includes */
#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "eps_trc.h"
#include "eps_tasks.h"
#include "eps_timer.h"
#include "eps_msg.h"

/* EPS Driver If includes */
#include "eps_pn_drv_if.h" /* Pn Device Driver Interface */

#if (PSI_CFG_USE_EDDS == 1)
/* Defines */
//#define EPS_CFG_EDDS_SCHEDULER_MEASURE_ON
#define EPS_CFG_EDDS_TIMER_HIGH_MS  1       // 1ms
#define EPS_CFG_EDDS_TIMER_LOW_MS   100     // 100ms

typedef enum _EPS_EDDS_SCHEDULER_TRIGGER_MODE
{
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INVALID        = 0,
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TIMED          = 1,
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INTERMEDIATE   = 2,
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TRIGGER        = 3,
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_RETRIGGER      = 4,
    eEPS_EDDS_SCHEDULER_TRIGGER_MODE_STOP           = 5
}EPS_EDDS_SCHEDULER_TRIGGER_MODE;

typedef enum _EPS_EDDS_SCHEDULER_CALL_MODE
{
    eEPS_EDDS_SCHEDULER_CALL_MODE_INVALID       = 0x0000, 
    eEPS_EDDS_SCHEDULER_CALL_MODE_TIMED         = 0x0001,
    eEPS_EDDS_SCHEDULER_CALL_MODE_INTERMEDIATE  = 0x0010,
    eEPS_EDDS_SCHEDULER_CALL_MODE_TRIGGER       = 0x0100,
    eEPS_EDDS_SCHEDULER_CALL_MODE_STOP          = 0x1000
}EPS_EDDS_SCHEDULER_CALL_MODE;
//lint -esym(751, EPS_EDDS_SCHEDULER_CALL_MODE) local typedef not referenced, but it is in the embedded variants
//lint -esym(749, _EPS_EDDS_SCHEDULER_CALL_MODE::eEPS_EDDS_SCHEDULER_CALL_MODE_INVALID) local enumeration constant not referenced - used for error detection (memset value == 0)

#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_INVALID                0x0000
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TIMED                  0x0001
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_TIMED                  0x0101
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_INTERMEDIATE           0x0110
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_INTERMEDIATE           0x0010
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TRIGGER                0x0100
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TIMED_INTERMEDIATE     0x0011
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_TIMED_INTERMEDIATE     0x0111
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_STOP                   0x1000
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_STOP                   0x1001
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION3_STOP                   0x1010
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION4_STOP                   0x1011
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION5_STOP                   0x1100
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION6_STOP                   0x1101
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION7_STOP                   0x1110
#define EPS_EDDS_SCHEDULER_CALL_MODE_OPTION8_STOP                   0x1111

#endif //PSI_CFG_USE_EDDS

#if (PSI_CFG_USE_HD_COMP == 1)
/* General PN Gathered Interrupt Callback Function */
static LSA_VOID eps_pn_interrupt(
    LSA_UINT32   uParam,
    LSA_VOID   * pArgs )
{
    EPS_SYS_PTR_TYPE const pSys    = (EPS_SYS_PTR_TYPE)pArgs;
    LSA_UINT32             uIntSrc = uParam;

    #if (PSI_CFG_USE_EDDP == 1)
    PSI_EDD_HDDB            hddb;
    EPS_PNDEV_HW_PTR_TYPE   pBoard;
    LSA_UINT16              retVal;

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

    // do the PNIP things here (usually done in user mode thread)
    // only for single interrupt (MUX)
    if (uIntSrc == 2)
    {
        if (   (pBoard->EpsBoardInfo.board_type == EPS_PNDEV_BOARD_EB200P)
            || (pBoard->EpsBoardInfo.board_type == EPS_PNDEV_BOARD_FPGA_ERTEC200P))
        {
            psi_hd_get_edd_handle(pSys->hd_nr, &hddb);
            eddp_SetIRQ_WaitTime_to_MAX((EDDP_HANDLE)hddb, uIntSrc); 
        }
    }
    #endif //PSI_CFG_USE_EDDP

    /* Call psi interrupt for hd_id with interrupt source */    
    psi_hd_interrupt(pSys->hd_nr, uIntSrc);
}
#endif //PSI_CFG_USE_HD_COMP

/* General EDDS Interrupt Poll Callback Function */
#if (PSI_CFG_USE_EDDS == 1)
static LSA_VOID eps_pn_interrupt_edds_poll(
    LSA_UINT32   uParam,
    LSA_VOID   * pArgs )
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)pArgs;
    LSA_BOOL               bRetVal;

    LSA_UNUSED_ARG(uParam);

    /* Call psi interrupt for hd_id with interrupt source */
    bRetVal = psi_hd_isr_poll(pSys->hd_nr);
    EPS_ASSERT(!bRetVal); // TODO: PSI API returns a strange LSA_FALSE here
}

// description
static LSA_VOID eps_edds_sys_timer_cbf(
    union sigval sig )
{
    LSA_UINT8                             sendBuffer[EPS_MSG_MAX_MSG_SIZE] = {0};
    EPS_EDDS_SCHEDULER_TRIGGER_MODE       eTriggerMode;
    EPS_PNDEV_HW_PTR_TYPE                 pBoard;
    EPS_SYS_PTR_TYPE                const pSys = (EPS_SYS_PTR_TYPE)sig.sival_ptr;
    LSA_UINT16                            retVal;

    EPS_ASSERT(pSys);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

    // put the trigger mode into the send buffer
    eTriggerMode  = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TIMED;
    sendBuffer[0] = (LSA_UINT8)eTriggerMode;
    sendBuffer[1] = (LSA_UINT8)pSys->hd_nr;

    // and send it to the active message queue
    eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqActive, &sendBuffer[0]);
}

/* Helper Function to translate the messages */
static LSA_UINT16 eps_edds_translate_trigger_mode(
    EPS_EDDS_SCHEDULER_TRIGGER_MODE const eTriggerMode,
    LSA_UINT16                            lCallModeInOut,
    LSA_UINT16                      const hd_nr )
{
    switch (eTriggerMode)
    {
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TIMED:
        {
            lCallModeInOut |= eEPS_EDDS_SCHEDULER_CALL_MODE_TIMED;
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "hd(%u) eps_edds_scheduler_thread(): edds_scheduler() - registered timed", hd_nr );
            break;
        }
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INTERMEDIATE:
        {
            lCallModeInOut |= eEPS_EDDS_SCHEDULER_CALL_MODE_INTERMEDIATE;
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "hd(%u) eps_edds_scheduler_thread(): edds_scheduler() - registered intermediate", hd_nr );                
            break;    
        }
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TRIGGER:
        {
            lCallModeInOut |= eEPS_EDDS_SCHEDULER_CALL_MODE_TRIGGER;
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "hd(%u) eps_edds_scheduler_thread(): edds_scheduler() - registered trigger", hd_nr );
            break;    
        }
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_RETRIGGER:
        {
            lCallModeInOut |= eEPS_EDDS_SCHEDULER_CALL_MODE_TRIGGER;
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "hd(%u) eps_edds_scheduler_thread(): edds_scheduler() - registered retrigger", hd_nr );
            break;   
        }
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_STOP:
        {
            lCallModeInOut |= eEPS_EDDS_SCHEDULER_CALL_MODE_STOP;
            EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "hd(%u) eps_edds_scheduler_thread(): edds_scheduler() - registered stop", hd_nr );
            break;    
        }
        case eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INVALID:
        {
            EPS_FATAL("Unknown edds_scheduler eTriggerMode call");
            break;
        }
        default: break;
    }
    
    return lCallModeInOut;
}

/* EDDS Interrupt Callback Function */
static LSA_VOID eps_edds_interrupt(
    LSA_UINT32   uParam,
    LSA_VOID   * pArgs )
{
    LSA_UINT8                             sendBuffer[EPS_MSG_MAX_MSG_SIZE] = {0};
    EPS_EDDS_SCHEDULER_TRIGGER_MODE       eTriggerMode;
    EPS_PNDEV_HW_PTR_TYPE                 pBoard;
    EPS_SYS_PTR_TYPE                const pSys = (EPS_SYS_PTR_TYPE)pArgs;
    LSA_UINT16                            retVal;

    LSA_UNUSED_ARG(uParam);

    EPS_ASSERT(pSys);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

    // put the trigger mode into the send buffer
    eTriggerMode  = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INTERMEDIATE;
    sendBuffer[0] = (LSA_UINT8)eTriggerMode;
    sendBuffer[1] = (LSA_UINT8)pSys->hd_nr;

    // and send it to the active message queue
    eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqActive, &sendBuffer[0]);
}

// description
static LSA_VOID eps_edds_scheduler_thread(
    LSA_UINT32   uParam,
    LSA_VOID   * pArgs )
{
    LSA_UINT8                       receiveMessage[EPS_MSG_MAX_MSG_SIZE]    = {0};
    LSA_UINT16                      hd_nr                                   = 0;
    PSI_EDD_HDDB                    pHDDB                                   = LSA_NULL;
    LSA_UINT32                      msgq_id                                 = (LSA_UINT32)pArgs;    
    EPS_EDDS_SCHEDULER_TRIGGER_MODE eTriggerMode                            = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_INVALID;
    LSA_UINT16                      lCallMode                               = 0;
    #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
    LSA_UINT64                      uStartTime;
    LSA_UINT64                      uEndTime;
    LSA_UINT64                      uDiffTime;
    #endif
    EPS_PNDEV_HW_PTR_TYPE           pBoard;
    LSA_BOOL                        bIsRunning                              = LSA_TRUE;
    LSA_UINT16                      retVal;
    LSA_UINT32                      lOccuredInts;
    LSA_UNUSED_ARG(uParam);  

    EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_scheduler_thread(): thread started, listening on message queue (%u)", msgq_id);

    // endless listening for messages
    while (bIsRunning)
    {
        lCallMode = 0;
        
        // get next message
        eps_msg_receive((LSA_UINT16)msgq_id, &receiveMessage[0]);

        // get the relevant data
        eTriggerMode    = (EPS_EDDS_SCHEDULER_TRIGGER_MODE)receiveMessage[0];
        hd_nr           = (LSA_UINT16)receiveMessage[1];
          
        // get edd_hDDB and pBoard
        psi_hd_get_edd_handle(hd_nr, &pHDDB);
        retVal = eps_pndev_if_get(hd_nr, &pBoard);
        EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

        // translate the message
        lCallMode = eps_edds_translate_trigger_mode(eTriggerMode, lCallMode, hd_nr);
        
        // flush the message queue
        while( (eps_msg_tryreceive((LSA_UINT16)msgq_id, &receiveMessage[0])) == 0 )
                // there are more msg availabe
        {
            // NOTE: this cannot be a endless loop
            // the only EPS_EDDS_SCHEDULER_TRIGGER_MODE who can force an endless loop is the interrupt
            // but the interrupts are disabled when they occure the first time
            // so no interrupt message overflow occures
            
            // get the relevant data
            eTriggerMode = (EPS_EDDS_SCHEDULER_TRIGGER_MODE)receiveMessage[0];

            // translate the message
            lCallMode = eps_edds_translate_trigger_mode(eTriggerMode, lCallMode, hd_nr);
        }
        
        // ----------------------------------------------------------------------
        // now do the scheduler call
        // ----------------------------------------------------------------------
        
        switch (lCallMode)
        {
            // Timed
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TIMED:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_TIMED:
            {
                #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                uStartTime = eps_get_ticks_100ns() * (LSA_UINT64)100;
                #endif

                edds_scheduler(pHDDB, EDDS_SCHEDULER_CYCLIC_CALL);

                #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                uEndTime  = eps_get_ticks_100ns() * (LSA_UINT64)100;
                uDiffTime = uEndTime - uStartTime;

                EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_edds_scheduler_thread(): hd(%u) edds_scheduler() - call by timed (0x%04x) - %u ns runtime",
                    hd_nr, lCallMode, uDiffTime );
                #endif

                break;
            }

            // Intermediate
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_INTERMEDIATE:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_INTERMEDIATE:
            {
                lOccuredInts = 0;

                EPS_ASSERT(pBoard->ReadHwIr != LSA_NULL);
                retVal = (*pBoard->ReadHwIr)(pBoard, &lOccuredInts);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

                if (lOccuredInts)
                {
                    #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                    uStartTime = eps_get_ticks_100ns() * (LSA_UINT64)100;
                    #endif

                    edds_scheduler(pHDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);                

                    #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                    uEndTime  = eps_get_ticks_100ns() * (LSA_UINT64)100;
                    uDiffTime = uEndTime - uStartTime;

                    EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_edds_scheduler_thread(): hd(%u) edds_scheduler() - call by intermediate (0x%04x) - %u ns runtime",
                        hd_nr, lCallMode, uDiffTime );
                    #endif
                }

                //enable interrupts at the board again -> PnDevDrv deactivates them
                EPS_ASSERT(pBoard->EnableHwIr != LSA_NULL);
                retVal = (*pBoard->EnableHwIr)(pBoard);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                break;
            }

            // Trigger
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TRIGGER:
            {
                #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                uStartTime = eps_get_ticks_100ns() * (LSA_UINT64)100;
                #endif

                edds_scheduler(pHDDB, EDDS_SCHEDULER_INTERMEDIATE_CALL);

                #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                uEndTime  = eps_get_ticks_100ns() * (LSA_UINT64)100;
                uDiffTime = uEndTime - uStartTime;

                EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_edds_scheduler_thread(): hd(%u) edds_scheduler() - call by trigger (0x%04x) - %u ns runtime",
                    hd_nr, lCallMode, uDiffTime );
                #endif

                break;
            }

            // Timed + Intermediate
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_TIMED_INTERMEDIATE:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_TIMED_INTERMEDIATE:
            {
                lOccuredInts = 0;

                EPS_ASSERT(pBoard->ReadHwIr != LSA_NULL);
                retVal = (*pBoard->ReadHwIr)(pBoard, &lOccuredInts);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                if (lOccuredInts)
                {
                    #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                    uStartTime = eps_get_ticks_100ns() * (LSA_UINT64)100;
                    #endif

                    edds_scheduler(pHDDB, EDDS_SCHEDULER_CYCLIC_CALL);

                    #if defined(EPS_CFG_EDDS_SCHEDULER_MEASURE_ON)
                    uEndTime  = eps_get_ticks_100ns() * (LSA_UINT64)100;
                    uDiffTime = uEndTime - uStartTime;

                    EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE, "eps_edds_scheduler_thread(): hd(%u) edds_scheduler() - call by timed + intermediate (0x%04x) - %u ns runtime", 
                        hd_nr, lCallMode, uDiffTime );
                    #endif
                }

                //enable interrupts at the board again -> PnDevDrv deactivates them
                EPS_ASSERT(pBoard->EnableHwIr != LSA_NULL);
                retVal = (*pBoard->EnableHwIr)(pBoard);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                break;
            }

            // STOP
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION2_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION3_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION4_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION5_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION6_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION7_STOP:
            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION8_STOP:
            {
                EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_scheduler_thread(): hd(%u) edds_scheduler() - STOP (0x%04x)",
                    hd_nr, lCallMode );
                bIsRunning = LSA_FALSE;
                break;
            }

            case EPS_EDDS_SCHEDULER_CALL_MODE_OPTION1_INVALID:
            {
                EPS_FATAL("Unknown edds_scheduler eTriggerMode call");
                break;
            }

            default: break;
        }
    }
}
#endif

#ifdef PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON
/* General EDDI Exttimer Callback Function */
static LSA_VOID eps_eddi_exttimer_interrupt(
    LSA_UINT32   uParam,
    LSA_VOID   * pArgs )
{
    EPS_SYS_PTR_TYPE const pSys = (EPS_SYS_PTR_TYPE)pArgs;

    LSA_UNUSED_ARG(uParam);

    /* Call psi interrupt for hd_id with interrupt source */
    psi_hd_eddi_exttimer_interrupt(pSys->hd_nr);
}

/**
 * Starts the Exttimer Mode
 * 
 * A timer is started, calling a call-back-function, when it expired.
 * 
 * @param hSysDev
 */
LSA_VOID eps_eddi_exttimer_start(
    PSI_SYS_HANDLE hSysDev )
{
#ifdef PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON
    EPS_PNDEV_CALLBACK_TYPE       Cbf;
    EPS_PNDEV_HW_PTR_TYPE         pBoard;
    LSA_UINT16                    retVal;
    EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDI);
    EPS_ASSERT(pBoard->TimerCtrlStart != LSA_NULL);

    Cbf.pArgs  = (LSA_VOID*)pSys;
    Cbf.uParam = 0;
    Cbf.pCbf   = eps_eddi_exttimer_interrupt;
    retVal = pBoard->TimerCtrlStart(pBoard, &Cbf);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
#else
    LSA_UNUSED_ARG(hSysDev);
    EPS_FATAL("Exttimer Mode not supported in this configuration.");
#endif
}

/**
 * Stopping the Exttimer Mode
 *  
 * @param hSysDev
 */
LSA_VOID eps_eddi_exttimer_stop(
    PSI_SYS_HANDLE hSysDev )
{
#ifdef PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON
    EPS_PNDEV_HW_PTR_TYPE       pBoard;
    LSA_UINT16                  retVal;
    EPS_SYS_PTR_TYPE      const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->TimerCtrlStop != LSA_NULL);

    retVal = pBoard->TimerCtrlStop(pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
#else
    LSA_UNUSED_ARG(hSysDev);
    EPS_FATAL("Exttimer Mode not supported in this configuration.");
#endif
}
#endif //PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON

#if (PSI_CFG_USE_HD_COMP == 1)
/**
 * Enables Interrupt for specified board.
 *
 * For a HD (pSys->hd_nr) an interrupt is enabled via Systemadaption. 
 *
 * @param hSysDev
 * @return
 */
LSA_VOID eps_enable_pnio_event(
    PSI_SYS_HANDLE const hSysDev )
{
    EPS_PNDEV_HW_PTR_TYPE               pBoard;
    EPS_PNDEV_CALLBACK_TYPE             Cbf;
    EPS_PNDEV_INTERRUPT_DESC_TYPE       IsrDesc;
    LSA_UINT16                          retVal;
    EPS_SYS_PTR_TYPE              const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_enable_pnio_event(): edd_comp_id(%#x) asic_type(%u) IrMode(%u)", 
        pSys->edd_comp_id, pBoard->asic_type, pBoard->IrMode);

    if (pSys->edd_comp_id != LSA_COMP_ID_EDDS)
    {	
        switch (pBoard->asic_type)
        {
            case EPS_PNDEV_ASIC_IRTEREV7:
            {
                #if (EPS_PLF == EPS_PLF_ADONIS_X86)
                // in Advanced no Interrupts are activated --> model of the CPU 1517
                // they are just using ExtTimerMode
                #else
                Cbf.pArgs  = (LSA_VOID*)pSys;
                Cbf.uParam = 0;
                Cbf.pCbf   = eps_pn_interrupt;
                
                #if (EPS_PLF == EPS_PLF_SOC_MIPS) // in Basic variants we enable the real kernel interrupts
                IsrDesc    = EPS_PNDEV_ISR_INTERRUPT;
                #else
                IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
                #endif
                
                retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                #endif
                break;
            }

            #if ((EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
            case EPS_PNDEV_ASIC_HERA:
            case EPS_PNDEV_ASIC_ERTEC200P:
            {
                Cbf.pArgs   = (LSA_VOID*)pSys;
                Cbf.uParam  = 0;                                        // IFA_PN_IRQ(0) AND IFA_PN_IRQ(2) are used 
                Cbf.pCbf    = eps_pn_interrupt;
                IsrDesc     = EPS_PNDEV_ISR_INTERRUPT;
                retVal      = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                break;
            }
            #endif

            default:
            {
                
                Cbf.pArgs  = (LSA_VOID*)pSys;
                Cbf.uParam = 0; /* group interrupt */
                Cbf.pCbf   = eps_pn_interrupt;
                IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
                retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_enable_pnio_event(): IsrDesc=%d", IsrDesc);
                
                if	(   (pBoard->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
                    ||  (pBoard->asic_type == EPS_PNDEV_ASIC_HERA))
                {
                    Cbf.pArgs  = (LSA_VOID*)pSys;
                    Cbf.uParam = 2; /* single interrupt */
                    Cbf.pCbf   = eps_pn_interrupt;
                    IsrDesc    = EPS_PNDEV_ISR_PN_NRT;
                    retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                }
            }
            //lint --e(788) enum constant not used within defaulted switch - we only need EPS_PNDEV_ASIC_IRTEREV7 here, rest is handled in default case
        }
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_enable_pnio_event(): IsrDesc=%d", IsrDesc);
    }
    else // LSA_COMP_ID_EDDS
    {
        #if (PSI_CFG_USE_EDDS == 1)
        switch (pBoard->IrMode)
        {
            case EPS_PNDEV_IR_MODE_POLL:
            {
                Cbf.pArgs  = (LSA_VOID*)pSys;
                Cbf.uParam = 0;
                Cbf.pCbf   = eps_pn_interrupt_edds_poll;
                IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
                retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_enable_pnio_event(): EPS_PNDEV_IR_MODE_POLL");
                break;
            }

            case EPS_PNDEV_IR_MODE_INTERRUPT:
            {
                EPS_FATAL("eps_enable_pnio_event(): EDDS InterruptMode stand-alone not allowed");
                break;
            }

            case EPS_PNDEV_IR_MODE_POLLINTERRUPT:
            {
                /* Enable both - Polling and Interrupt */

                Cbf.pArgs  = (LSA_VOID*)pSys;
                Cbf.uParam = 0;
                Cbf.pCbf   = eps_pn_interrupt_edds_poll;
                IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
                retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

                Cbf.pArgs  = (LSA_VOID*)pSys;
                Cbf.uParam = 0;
                Cbf.pCbf   = eps_pn_interrupt;
                IsrDesc    = EPS_PNDEV_ISR_INTERRUPT;
                retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

                EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_enable_pnio_event(): EPS_PNDEV_IR_MODE_POLLINTERRUPT");
                break;
            }

            case EPS_PNDEV_IR_MODE_CPU:
            {
                // set the default startup values
                pBoard->EpsBoardInfo.edds.bFillActive      = LSA_FALSE;
                pBoard->EpsBoardInfo.edds.bHsyncModeActive = LSA_FALSE;

                // allocate the message queues
                retVal = eps_msg_alloc_msgq(&pBoard->EpsBoardInfo.edds.hMsgqHigh);
                EPS_ASSERT(LSA_RET_OK == retVal);
                retVal = eps_msg_alloc_msgq(&pBoard->EpsBoardInfo.edds.hMsgqLow);
                EPS_ASSERT(LSA_RET_OK == retVal);

                // select message queue low by default
                pBoard->EpsBoardInfo.edds.hMsgqActive = pBoard->EpsBoardInfo.edds.hMsgqLow;

                // start the 2 threads for the EDDS
                pBoard->EpsBoardInfo.edds.hThreadHigh = eps_tasks_start_thread("EDDS_SCHED_HIGH", 
                                                                               EPS_POSIX_THREAD_PRIORITY_ABOVE_NORMAL, 
                                                                               eSchedFifo, 
                                                                               eps_edds_scheduler_thread,
                                                                               0, 
                                                                               (LSA_VOID*)(LSA_UINT32)pBoard->EpsBoardInfo.edds.hMsgqHigh, 
                                                                               eRUN_ON_1ST_CORE);
                pBoard->EpsBoardInfo.edds.hThreadLow  = eps_tasks_start_thread("EDDS_SCHED_LOW",
                                                                               EPS_POSIX_THREAD_PRIORITY_NORMAL,
                                                                               eSchedFifo,
                                                                               eps_edds_scheduler_thread,
                                                                               0,
                                                                               (LSA_VOID*)(LSA_UINT32)pBoard->EpsBoardInfo.edds.hMsgqLow,
                                                                               eRUN_ON_1ST_CORE);

                // start the system timer
                if (pBoard->EpsBoardInfo.edds.bFillActive) // Fill==1?
                {
                    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_enable_pnio_event(): EDDS - EPS_PNDEV_IR_MODE_CPU - TIMER_1MS - hd_nr(%u) bFillActive(%u) bHsyncModeActive(%u)",
                        pSys->hd_nr, pBoard->EpsBoardInfo.edds.bFillActive, pBoard->EpsBoardInfo.edds.bHsyncModeActive);

                    retVal = eps_timer_allocate_sys_timer(&pBoard->EpsBoardInfo.edds.hTimer,
                                                          "EDDS_SYS_TIMER",
                                                          EPS_CFG_EDDS_TIMER_HIGH_MS,
                                                          eps_edds_sys_timer_cbf,
                                                          (LSA_VOID*)hSysDev);
                    EPS_ASSERT(LSA_RET_OK == retVal);
                }
                else // Fill==0
                {
                    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_enable_pnio_event(): EDDS - EPS_PNDEV_IR_MODE_CPU - TIMER_100MS - hd_nr(%u) bFillActive(%u) bHsyncModeActive(%u)",
                        pSys->hd_nr, pBoard->EpsBoardInfo.edds.bFillActive, pBoard->EpsBoardInfo.edds.bHsyncModeActive);

                    retVal = eps_timer_allocate_sys_timer(&pBoard->EpsBoardInfo.edds.hTimer,
                                                          "EDDS_SYS_TIMER",
                                                          EPS_CFG_EDDS_TIMER_LOW_MS,
                                                          eps_edds_sys_timer_cbf,
                                                          (LSA_VOID*)hSysDev);
                    EPS_ASSERT(LSA_RET_OK == retVal);
                }

                // enable interrupts
                if (!pBoard->EpsBoardInfo.edds.bFillActive) // Fill==0?
                {
                    Cbf.pArgs  = (LSA_VOID*)pSys;
                    Cbf.uParam = 0;
                    Cbf.pCbf   = eps_edds_interrupt;
                    IsrDesc    = EPS_PNDEV_ISR_CPU;
                    retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                }

                break;
            }

            case EPS_PNDEV_IR_MODE_UNDEFINED:
            default:
            {
                EPS_FATAL("eps_enable_pnio_event(): EDDS undefined InterruptMode");
            }
        }
        #else
        EPS_FATAL("eps_enable_pnio_event(): EDDS expected but not configurated for Interrupts");
        #endif
    }
}

LSA_VOID eps_disable_pnio_event(
    PSI_SYS_HANDLE const hSysDev )
{
    EPS_PNDEV_HW_PTR_TYPE               pBoard;
    EPS_PNDEV_INTERRUPT_DESC_TYPE       IsrDesc;
    LSA_UINT16                          retVal;
    EPS_SYS_PTR_TYPE              const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_disable_pnio_event(): edd_comp_id(%#x) asic_type(%u) IrMode(%u)",
        pSys->edd_comp_id, pBoard->asic_type, pBoard->IrMode);

    if (pSys->edd_comp_id != LSA_COMP_ID_EDDS)
    {
        switch (pBoard->asic_type)
        {
            case EPS_PNDEV_ASIC_IRTEREV7:
            {
                #if (EPS_PLF == EPS_PLF_ADONIS_X86)
                // in Advanced no Interrupts are activated --> model of the CPU 1517
                // they are just using ExtTimerMode
                #else    

                #if(EPS_PLF == EPS_PLF_SOC_MIPS) // in Basic variants we enable the real kernel interrupts
                IsrDesc    = EPS_PNDEV_ISR_INTERRUPT;
                #else
                IsrDesc    = EPS_PNDEV_ISR_PN_GATHERED;
                #endif
                
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                #endif
                break;
            }

            #if ((EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_PNIP_ARM9))
            case EPS_PNDEV_ASIC_HERA:
            case EPS_PNDEV_ASIC_ERTEC200P:
            {
                IsrDesc = EPS_PNDEV_ISR_INTERRUPT;
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                break;
            }
            #endif

            default:
            {
                /* group interrupt */
                IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                
                if  (   (pBoard->asic_type == EPS_PNDEV_ASIC_ERTEC200P)
                     || (pBoard->asic_type == EPS_PNDEV_ASIC_HERA))
                {
                    /* single interrupt */
                    IsrDesc = EPS_PNDEV_ISR_PN_NRT;
                    retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                }
            }
            //lint --e(788) enum constant not used within defaulted switch - we only need EPS_PNDEV_ASIC_IRTEREV7, EPS_PNDEV_ASIC_HERA, EPS_PNDEV_ASIC_ERTEC200P here, rest is handled in default case
        }
    }
    else //LSA_COMP_ID_EDDS
    {
        #if (PSI_CFG_USE_EDDS == 1)
        switch (pBoard->IrMode)
        {
            case EPS_PNDEV_IR_MODE_POLL:
            {
                IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                break;
            }

            case EPS_PNDEV_IR_MODE_INTERRUPT:
            {
                EPS_FATAL("eps_disable_pnio_event(): EDDS InterruptMode stand-alone not allowed");
                break;
            }

            case EPS_PNDEV_IR_MODE_POLLINTERRUPT:
            {
                IsrDesc = EPS_PNDEV_ISR_PN_GATHERED;
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    
                IsrDesc = EPS_PNDEV_ISR_INTERRUPT;
                retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    
                break;
            }

            case EPS_PNDEV_IR_MODE_CPU:
            {
                LSA_UINT8                       sendBuffer[EPS_MSG_MAX_MSG_SIZE] = {0};
                EPS_EDDS_SCHEDULER_TRIGGER_MODE eTriggerMode;
                LSA_RESPONSE_TYPE               retValMsg;

                // disable interrupts
                if (!pBoard->EpsBoardInfo.edds.bFillActive) // Fill==0?
                {
                    IsrDesc = EPS_PNDEV_ISR_CPU;
                    retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
                }

                // disable system timer
                retVal = eps_timer_free_sys_timer(pBoard->EpsBoardInfo.edds.hTimer);
                EPS_ASSERT(LSA_RET_OK == retVal);

                // put the quit mode into the send buffer
                eTriggerMode  = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_STOP;
                sendBuffer[0] = (LSA_UINT8)eTriggerMode;
                sendBuffer[1] = (LSA_UINT8)pSys->hd_nr;

                // and send it to the active message queues
                eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqHigh, &sendBuffer[0]);
                eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqLow, &sendBuffer[0]);

                // delete threads
                eps_tasks_stop_thread(pBoard->EpsBoardInfo.edds.hThreadHigh);
                eps_tasks_stop_thread(pBoard->EpsBoardInfo.edds.hThreadLow);
                
                // delete message queues
                retValMsg = eps_msg_free_msgq(pBoard->EpsBoardInfo.edds.hMsgqHigh);
                EPS_ASSERT(LSA_RET_OK == retValMsg);
                retValMsg = eps_msg_free_msgq(pBoard->EpsBoardInfo.edds.hMsgqLow);
                EPS_ASSERT(LSA_RET_OK == retValMsg);

                EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_disable_pnio_event(): hd_nr(%d): EPS_PNDEV_IR_MODE_CPU", pSys->hd_nr);                
                break;
            }
            case EPS_PNDEV_IR_MODE_UNDEFINED:
            default:
            {
                EPS_FATAL("eps_disable_pnio_event(): EDDS undefined InterruptMode");
            }
        }
        #else
        EPS_FATAL("eps_disable_pnio_event(): EDDS expected but not configurated for Interrupts");
        #endif
    }
}
#endif //PSI_CFG_USE_HD_COMP

// argument bHsyncModeActive is only valid at startup
LSA_BOOL eps_edds_notify_fill_settings(
    PSI_SYS_HANDLE const hSysDev,
    LSA_BOOL       const bFillActive,
    LSA_BOOL       const bIOConfigured,
    LSA_BOOL       const bHsyncModeActive )
{
    EPS_PNDEV_HW_PTR_TYPE       pBoard;
    EPS_SYS_PTR_TYPE      const pSys    = (EPS_SYS_PTR_TYPE)hSysDev;
    LSA_BOOL                    bRetVal = LSA_TRUE; // default: edds_scheduler() is NOT called directly by edds_request()
    LSA_UINT16                  retVal;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

    if (pSys->edd_comp_id != LSA_COMP_ID_EDDS)
    {
        LSA_UNUSED_ARG(bFillActive);
        LSA_UNUSED_ARG(bIOConfigured);
        LSA_UNUSED_ARG(bHsyncModeActive);
        EPS_FATAL("eps_edds_notify_fill_settings(): invalid edd_comp_id");
    }
    else // LSA_COMP_ID_EDDS
    {
        #if (PSI_CFG_USE_EDDS == 1)
        EPS_PNDEV_INTERRUPT_DESC_TYPE IsrDesc;

        switch (pBoard->IrMode)
        {
            case EPS_PNDEV_IR_MODE_CPU:
            {
                LSA_RESPONSE_TYPE retValSysTimer;

                EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_notify_fill_settings(): hd_nr(%u): arguments: bFillActive(%u) bIOConfigured(%u) bHsyncModeActive(%u)", pSys->hd_nr, bFillActive, bIOConfigured, bHsyncModeActive);

                // change message queue, interrupts and system timer
                if (bHsyncModeActive) // HSYNC mode requested at startup?
                {
                    // HSYNC mode was not activated yet and is now activated

                    // select message queue high
                    pBoard->EpsBoardInfo.edds.hMsgqActive = pBoard->EpsBoardInfo.edds.hMsgqHigh;

                    // change value
                    pBoard->EpsBoardInfo.edds.bHsyncModeActive = bHsyncModeActive;

                    // setting for edds_scheduler() whether called directly by edds_request():
                    // edds_scheduler() should be called directly.
                    // Removes the trigger call in adaption.
                    bRetVal = LSA_FALSE; // direct call
                }
                else if (pBoard->EpsBoardInfo.edds.bHsyncModeActive) // HSYNC mode active?
                {
                    // nothing todo

                    // setting for edds_scheduler() whether called directly by edds_request():
                    // edds_scheduler() should be called directly.
                    // Removes the trigger call in adaption.
                    bRetVal = LSA_FALSE; // direct call
                }
                else
                {
                    if ((!pBoard->EpsBoardInfo.edds.bFillActive) && bFillActive)
                    {
                        // Fill was not activated yet and is now activated

                        // select message queue high
                        pBoard->EpsBoardInfo.edds.hMsgqActive = pBoard->EpsBoardInfo.edds.hMsgqHigh;

                        // disable interrupts
                        IsrDesc = EPS_PNDEV_ISR_CPU;
                        retVal  = pBoard->DisableIsr(pBoard, &IsrDesc);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

                        // change timer
                        retValSysTimer = eps_timer_change_sys_timer(pBoard->EpsBoardInfo.edds.hTimer, EPS_CFG_EDDS_TIMER_HIGH_MS);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retValSysTimer);

                        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_notify_fill_settings(): change to TIMER_1MS");

                        // change value
                        pBoard->EpsBoardInfo.edds.bFillActive = bFillActive;
                    }
                    else if (pBoard->EpsBoardInfo.edds.bFillActive && (!bFillActive))
                    {
                        EPS_PNDEV_CALLBACK_TYPE Cbf;

                        // Fill was activated yet and is now not activated

                        // select message queue low
                        pBoard->EpsBoardInfo.edds.hMsgqActive = pBoard->EpsBoardInfo.edds.hMsgqLow;

                        // enable interrupts
                        Cbf.pArgs  = (LSA_VOID*)pSys;
                        Cbf.uParam = 0;
                        Cbf.pCbf   = eps_edds_interrupt;
                        IsrDesc    = EPS_PNDEV_ISR_CPU;
                        retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

                        // change timer
                        retValSysTimer = eps_timer_change_sys_timer(pBoard->EpsBoardInfo.edds.hTimer, EPS_CFG_EDDS_TIMER_LOW_MS);
                        EPS_ASSERT(EPS_PNDEV_RET_OK == retValSysTimer);

                        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_notify_fill_settings(): change to TIMER_100MS");

                        // change value
                        pBoard->EpsBoardInfo.edds.bFillActive = bFillActive;
                    }

                    // setting for edds_scheduler() whether called directly by edds_request():
                    if (!pBoard->EpsBoardInfo.edds.bFillActive) // Fill==0?
                    {
                        // NRT mode

                        // If NOT FillActive, then edds_scheduler() should be called directly.
                        // Removes the trigger call in adaption.
                        bRetVal = LSA_FALSE; // direct call
                    }
                    else // Fill==1
                    {
                        // IO/NRT mode

                        // If FillActive, then edds_scheduler() should NOT be called directly.
                        // bRetVal = LSA_TRUE; // bRetVal is already preset with this value
                    }
                }

                EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_edds_notify_fill_settings(): hd_nr(%u): new values: bFillActive(%u) bHsyncModeActive(%u) bRetval(scheduler_direct_call)(%u)", pSys->hd_nr, pBoard->EpsBoardInfo.edds.bFillActive, pBoard->EpsBoardInfo.edds.bHsyncModeActive, bRetVal);
                break;
            }
            case EPS_PNDEV_IR_MODE_POLL:
            case EPS_PNDEV_IR_MODE_INTERRUPT:
            case EPS_PNDEV_IR_MODE_POLLINTERRUPT:
            {
                // do nothing
                break;
            }
            case EPS_PNDEV_IR_MODE_UNDEFINED:
            default:
            {
                EPS_FATAL("eps_edds_notify_fill_settings(): invalid IR mode");
            }
        }
        #else
        EPS_FATAL("eps_edds_notify_fill_settings(): EDDS expected but not configurated for Interrupts");
        #endif
    }

    return bRetVal;
}

LSA_VOID eps_edds_do_notify_scheduler(
    PSI_SYS_HANDLE const hSysDev )
{
    EPS_PNDEV_HW_PTR_TYPE       pBoard;
    EPS_SYS_PTR_TYPE      const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    LSA_UINT16                  retVal;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

    if (pSys->edd_comp_id != LSA_COMP_ID_EDDS)
    {
        EPS_FATAL("eps_edds_do_notify_scheduler(): invalid edd_comp_id");
    }
    else // LSA_COMP_ID_EDDS
    {
        #if (PSI_CFG_USE_EDDS == 1)
        switch (pBoard->IrMode)
        {
            case EPS_PNDEV_IR_MODE_CPU:
            {
                if (pBoard->EpsBoardInfo.edds.bFillActive) // Fill==1?
                {
                    // do nothing: wait for next cyclic trigger

                    #if defined(UNUSED)
                    LSA_UINT8                       sendBuffer[EPS_MSG_MAX_MSG_SIZE] = {0};
                    EPS_EDDS_SCHEDULER_TRIGGER_MODE eTriggerMode;

                    // put the trigger mode into the send buffer
                    eTriggerMode  = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_TRIGGER;
                    sendBuffer[0] = (LSA_UINT8)eTriggerMode;
                    sendBuffer[1] = (LSA_UINT8)pSys->hd_nr;

                    // and send it to the active message queue
                    eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqActive, &sendBuffer[0]);
                    #endif
                }
                else // Fill==0
                {
                    EPS_FATAL("eps_edds_do_notify_scheduler(): not allowed in non Fill mode");
                }

                break;
            }

            case EPS_PNDEV_IR_MODE_POLL:
            case EPS_PNDEV_IR_MODE_INTERRUPT:
            case EPS_PNDEV_IR_MODE_POLLINTERRUPT:
            {
                // do nothing: wait for next cyclic trigger
                break;
            }

            case EPS_PNDEV_IR_MODE_UNDEFINED:
            default:
            {
                EPS_FATAL("eps_edds_do_notify_scheduler(): invalid IR mode");
            }
        }
        #else
        EPS_FATAL("eps_edds_do_notify_scheduler(): EDDS expected but not configurated for Interrupts");
        #endif
    }
}

LSA_VOID eps_edds_do_retrigger_scheduler(
    PSI_SYS_HANDLE const hSysDev )
{
    EPS_PNDEV_HW_PTR_TYPE       pBoard;
    EPS_SYS_PTR_TYPE      const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
    LSA_UINT16                  retVal;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

    if (pSys->edd_comp_id != LSA_COMP_ID_EDDS)
    {
        EPS_FATAL("eps_edds_do_retrigger_scheduler(): invalid edd_comp_id");
    }
    else //LSA_COMP_ID_EDDS
    {
        #if (PSI_CFG_USE_EDDS == 1)
        switch (pBoard->IrMode)
        {
            case EPS_PNDEV_IR_MODE_CPU:
            {
                LSA_UINT8                       sendBuffer[EPS_MSG_MAX_MSG_SIZE] = {0};
                EPS_EDDS_SCHEDULER_TRIGGER_MODE eTriggerMode;

                // put the trigger mode into the send buffer
                eTriggerMode  = eEPS_EDDS_SCHEDULER_TRIGGER_MODE_RETRIGGER;
                sendBuffer[0] = (LSA_UINT8)eTriggerMode;
                sendBuffer[1] = (LSA_UINT8)pSys->hd_nr;

                // and send it to the active message queue
                eps_msg_send(pBoard->EpsBoardInfo.edds.hMsgqActive, &sendBuffer[0]);

                break;
            }

            case EPS_PNDEV_IR_MODE_POLL:
            case EPS_PNDEV_IR_MODE_INTERRUPT:
            case EPS_PNDEV_IR_MODE_POLLINTERRUPT:
            {
                // do nothing
                break;
            }

            case EPS_PNDEV_IR_MODE_UNDEFINED:
            default:
            {
                EPS_FATAL("eps_edds_do_retrigger_scheduler(): invalid IR mode");
            }
        }
        #else
        EPS_FATAL("eps_edds_do_retrigger_scheduler(): EDDS expected but not configurated for Interrupts");
        #endif
    }
}

/** enable iso interrupt
* 
* this function enables board interrupts for iso mode.
*
* @param [IN]  \c hSysDev     systemhandle for device and interface
* @param [IN]  \c func_ptr    callback function pointer
* @param [IN]  \c param       parameter
* @param [IN]  \c args        argument
*
* @return 
*  - \c EPS_PNDEV_RET_OK      Interrupt enabled successful
*  - \c ..                    Enable interrupts not possible
*/
LSA_RESULT eps_enable_iso_interrupt(
    PSI_SYS_HANDLE hSysDev,
    LSA_VOID(*func_ptr)(LSA_INT param, LSA_VOID* args), LSA_INT32 param, LSA_VOID* args )
{
    EPS_PNDEV_HW_PTR_TYPE               pBoard;
    EPS_PNDEV_CALLBACK_TYPE             Cbf;
    EPS_PNDEV_INTERRUPT_DESC_TYPE       IsrDesc;
    LSA_UINT16                          retVal;
    EPS_SYS_PTR_TYPE              const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);

    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->EnableIsr != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_enable_iso_interrupt()");

    Cbf.pArgs  = args;
    Cbf.uParam = (LSA_UINT32)param;
    Cbf.pCbf   = (EPS_PNDEV_ISR_CBF)func_ptr;
    IsrDesc    = EPS_PNDEV_ISR_ISOCHRONOUS;
    retVal     = pBoard->EnableIsr(pBoard, &IsrDesc, &Cbf);

    if (retVal != EPS_PNDEV_RET_OK)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_enable_iso_interrupt(): Enable isr error (%u)", retVal);
    }

    return ((LSA_RESULT)retVal);
}

/** disable iso interrupt
* 
* this function disables board interrupts for iso mode.
*
* @param [IN]  \c hSysDev     systemhandle for device and interface
*
* @return 
*  - \c EPS_PNDEV_RET_OK      Interrupts disabled successful
*  - \c ..                    Disable interrupts not possible
*/
LSA_RESULT eps_disable_iso_interrupt(
    PSI_SYS_HANDLE hSysDev )
{
    EPS_PNDEV_HW_PTR_TYPE               pBoard;
    EPS_PNDEV_INTERRUPT_DESC_TYPE       IsrDesc;
    LSA_UINT16                          retVal;
    EPS_SYS_PTR_TYPE              const pSys = (EPS_SYS_PTR_TYPE)hSysDev;

    EPS_ASSERT(hSysDev);

    retVal = eps_pndev_if_get(pSys->hd_nr, &pBoard);
    EPS_ASSERT(EPS_PNDEV_RET_OK == retVal);
    EPS_ASSERT(pBoard != LSA_NULL);
    EPS_ASSERT(pBoard->DisableIsr != LSA_NULL);

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_disable_iso_interrupt()");

    IsrDesc = EPS_PNDEV_ISR_ISOCHRONOUS;
    retVal = pBoard->DisableIsr(pBoard, &IsrDesc);

    if (retVal != EPS_PNDEV_RET_OK)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_disable_iso_interrupt(): Disable isr error (%u)", retVal);
    }

    return ((LSA_RESULT)retVal);
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

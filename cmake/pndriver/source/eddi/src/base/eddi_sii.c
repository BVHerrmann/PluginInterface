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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_sii.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-SII (standard interrupt integration)        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ext.h"

#if defined (EDDI_CFG_SII_POLLING_MODE)
#include "eddi_isr_nrt.h"
#include "eddi_nrt_rx.h"
#endif

#define EDDI_MODULE_ID     M_ID_EDDI_SII
#define LTRC_ACT_MODUL_ID  20

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

/* external functions */

/*===========================================================================*/
/*                          internal defines                                 */
/*===========================================================================*/

//special test-mode:
//define EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_WITH_PNDEVDRV     //New-Cycle-Sync-Test-Mode used together with PNDEVDRV

/*===========================================================================*/
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_WITH_PNDEVDRV)
#if !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#error "Error in SII-Compiler-Switches: EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_WITH_PNDEVDRV only allowed with EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE!"
#endif
#endif
/*===========================================================================*/

//Internal Defines - do not change!
//Channel A0 always active!
#define IRQ_CHA0_SND_DONE_SELECTOR      IRQ_CHA0_SND_DONE
#define IRQ_CHA0_RCV_DONE_SELECTOR      IRQ_CHA0_RCV_DONE
//Channel A1 not supported!
//Channel B0 always active!
#define IRQ_CHB0_SND_DONE_SELECTOR      IRQ_CHB0_SND_DONE
#define IRQ_CHB0_RCV_DONE_SELECTOR      IRQ_CHB0_RCV_DONE
//Channel B1 not supported!
#if defined (EDDI_CFG_REV5)
#define IRQ_TIME_STAMP_SELECTOR         IRQ_TIME_STAMP
#else
#define IRQ_TIME_STAMP_SELECTOR         0   //interrupt remains masked!
#endif
#if defined (EDDI_CFG_REV5)
#define IRQ_NO_FREE_FCW_SELECTOR        IRQ_NO_FREE_FCW
#define IRQ_NO_FREE_DB_SELECTOR         IRQ_NO_FREE_DB
#elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define IRQ_NO_FREE_FCW_SELECTOR        0   //interrupt remains masked!
#define IRQ_NO_FREE_DB_SELECTOR         0   //interrupt remains masked!
#endif
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define IRQ_CLK_CY_INT_USER_SELECTOR    0
#else
#define IRQ_CLK_CY_INT_USER_SELECTOR    IRQ_CLK_CY_INT
#endif

//interrupt masks: 0=masked, 1=unmasked

//Defines for interrupt masks for global interrupts (interrupts that are not in another group)
#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
#define EDDI_SII_IRT_GROUP_MASK_GLOBAL_IRTE         0
#else
#define EDDI_SII_IRT_GROUP_MASK_GLOBAL_IRTE         (IRQ_NEW_CYCL)
#endif
#define EDDI_SII_NRT_GROUP_MASK_GLOBAL_IRTE         0

//Defines for interrupt masks for interrupt-group ISR
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_IRT_GROUP_MASK_PRIO0_ISR_IRTE      0
#define EDDI_SII_NRT_GROUP_MASK_PRIO0_ISR_IRTE      0
#else
#define EDDI_SII_IRT_GROUP_MASK_PRIO0_ISR_IRTE      0
#define EDDI_SII_NRT_GROUP_MASK_PRIO0_ISR_IRTE      (EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR|IRQ_CHA0_SND_DONE_SELECTOR|IRQ_CHB0_SND_DONE_SELECTOR|\
                                                     IRQ_TIME_STAMP_SELECTOR|IRQ_HOL_OVERFLOW)
#endif

//Defines for interrupt masks for interrupt-group AUX
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE      0
#define EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE      (EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR|IRQ_CHA0_SND_DONE_SELECTOR|IRQ_CHB0_SND_DONE_SELECTOR|\
                                                     IRQ_TIME_STAMP_SELECTOR|IRQ_HOL_OVERFLOW)
#else
#define EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE      0
#define EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE      0
#endif

//Defines for interrupt masks for interrupt-group ORG
#define EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE      0
#define EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE      (IRQ_CHB0_RCV_DONE_SELECTOR)

//Defines for interrupt masks for interrupt-group REST
#define EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE     (IRQ_CRT_SND_IRQ1|IRQ_TRC_FIFO_ERR|IRQ_TCW_DONE|IRQ_ACW_LIST_TERM|IRQ_SRT_CYCLE_SKIPPED|IRQ_SCORE_BOARD_CHANGED)
#define EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE     (IRQ_LINK_STATE_CHANGE|IRQ_COMMAND_DONE /*|IRQ_LOW_WATER_MARK*/)

//Defines for interrupt masks for interrupt-group NRT_LOW
#define EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE  0
#define EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE  (IRQ_CHA0_RCV_DONE_SELECTOR)

//Defines for interrupt masks for NRT-Done interrupts (special handling in New-Cycle-Sync-Mode)
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE  (IRQ_CHA0_SND_DONE_SELECTOR|IRQ_CHB0_SND_DONE_SELECTOR|\
                                                     IRQ_CHA0_RCV_DONE_SELECTOR|IRQ_CHB0_RCV_DONE_SELECTOR)
#else
#define EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE  0
#endif

//Defines for interrupt masks for possible USER interrupts
#define EDDI_SII_IRT_GROUP_MASK_USER_IRTE           (IRQ_START_OPERATE|IRQ_IRT_TRANS_END|IRQ_DMA_IN|IRQ_DMA_OUT|IRQ_IN_FAULT|IRQ_OUT_FAULT|\
                                                     IRQ_APL_CLK|IRQ_CLK_CY_INT_USER_SELECTOR|IRQ_CLK_CY|IRQ_COMP1|IRQ_COMP2|IRQ_COMP3|IRQ_COMP4|IRQ_COMP5|\
                                                     IRQ_RX_OUT_OF_SYNC|IRQ_TX_OUT_OF_SYNC|IRQ_PLL_EXT_IN|IRQ_READY_FOR_DMA_IN|IRQ_DMA_OUT_DONE)
#define EDDI_SII_NRT_GROUP_MASK_USER_IRTE           (IRQ_HP|IRQ_SP)

//Defines for interrupt masks for fatal interrupts
#define EDDI_SII_IRT_GROUP_MASK_FATAL_IRTE          (IRQ_CRT_RCV_IRQ1|IRQ_CRT_RCV_IRQ2|IRQ_CRT_LIST_FATAL_ERR|IRQ_ISRT_FAULT|IRQ_SCORE_BOARD_SKIPPED)
#define EDDI_SII_NRT_GROUP_MASK_FATAL_IRTE          (IRQ_NRT_LIST_FATAL_ERR|IRQ_TRIGGER_UNIT|IRQ_HOST_ACC_ERR|\
                                                     IRQ_SRT_FCW_EMPTY|IRQ_NO_FREE_FCW_SELECTOR|IRQ_NO_FREE_DB_SELECTOR|IRQ_NO_FDB_ENTRY)

//Defines for HW interrupt masks for New-Cycle-Sync-Mode (for IRTE-IMRs)
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_IRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE (EDDI_SII_IRT_GROUP_MASK_GLOBAL_IRTE|EDDI_SII_IRT_GROUP_MASK_FATAL_IRTE|EDDI_SII_IRT_GROUP_MASK_USER_IRTE|IRQ_CLK_CY_INT)
#define EDDI_SII_NRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE (EDDI_SII_NRT_GROUP_MASK_GLOBAL_IRTE|EDDI_SII_NRT_GROUP_MASK_FATAL_IRTE|EDDI_SII_NRT_GROUP_MASK_USER_IRTE)
#endif

//Defines for SW interrupt masks (for SII-variables)
#define EDDI_SII_IRT_SW_MASK_INIT_IRTE      (EDDI_SII_IRT_GROUP_MASK_GLOBAL_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_PRIO0_ISR_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE|\
                                             EDDI_SII_IRT_GROUP_MASK_FATAL_IRTE)
#define EDDI_SII_NRT_SW_MASK_INIT_IRTE      (EDDI_SII_NRT_GROUP_MASK_GLOBAL_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_PRIO0_ISR_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE|\
                                             EDDI_SII_NRT_GROUP_MASK_FATAL_IRTE)

//Defines for NewCycle reductions
#if defined (EDDI_CFG_SII_POLLING_MODE)
#define EDDI_SII_NEW_CYCLE_REDUCED_SCALING_10NS     (400000UL - 4UL)    //4ms scaling. do not change reduction of 4!
#endif

//selects DO macros for interrupt restarting in task (all these macros must have the same argument-list!)
#if defined (EDDI_CFG_SII_FLEX_MODE) || defined (EDDI_CFG_SII_STD_MODE)
#if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_SELECT_DO_MACRO_NAME_AUX       EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP
#endif //EDDI_CFG_SII_ADD_PRIO1_AUX
#define EDDI_SII_SELECT_DO_MACRO_NAME_ORG       EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP
#define EDDI_SII_SELECT_DO_MACRO_NAME_NRT_LOW   EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP
#elif !defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_SELECT_DO_MACRO_NAME_AUX       EDDI_DO_EV_INTERRUPT_PRIO1_AUX
#endif //EDDI_CFG_SII_ADD_PRIO1_AUX
#define EDDI_SII_SELECT_DO_MACRO_NAME_ORG       EDDI_DO_EV_INTERRUPT_PRIO2_ORG
#define EDDI_SII_SELECT_DO_MACRO_NAME_NRT_LOW   EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW
#endif //EDDI_CFG_SII_INT_RESTART_MODE_LOOP
#endif //EDDI_CFG_SII_FLEX_MODE || EDDI_CFG_SII_STD_MODE


/*===========================================================================*/
/*                          internal data                                    */
/*===========================================================================*/

#if defined (EDDI_SII_DEBUG_MODE_TRACES)
LSA_UINT32          EDDI_SII_Critical_Ctr;
#endif

/*===========================================================================*/
/*                          internal prototypes                              */
/*===========================================================================*/

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR    EDDI_SIISetNormalIsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptDummy( EDDI_DDB_TYPE  *  const  pDDB,
                                                                 LSA_BOOL          const  bExtTimerInt );

static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptWork( EDDI_DDB_TYPE  *  const  pDDB,
                                                                LSA_BOOL          const  bExtTimerInt );
#else
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptDummy( EDDI_DDB_TYPE  *  const  pDDB );

static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptWork( EDDI_DDB_TYPE  *  const  pDDB );
#endif

#if defined (EDDI_CFG_SII_POLLING_MODE)
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptLowWork( EDDI_DDB_TYPE  *  const  pDDB );

static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptLowDummy( EDDI_DDB_TYPE  *  const  pDDB );
#endif

#if defined (EDDI_CFG_SYSRED_2PROC)
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SII_SysRedPollingWork( EDDI_DDB_TYPE  *  const  pDDB );

static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SII_SysRedPollingDummy( EDDI_DDB_TYPE  *  const  pDDB );
#endif


/*===========================================================================*/
/*                          internal macros                                  */
/*===========================================================================*/

//  EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp)
//  EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT(pSIIComp)
//  EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp)
//  EDDI_SII_CALL_INTERRUPT_EVENT(pDDB, EDDIIntSource)
//  EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRTCheckBitIRTE, IRT_IrqIRTE, EDDIIntSource)
//  EDDI_SII_EXE_EDDI_NRT_INT(pDDB, NRTCheckBitIRTE, NRT_IrqIRTE, EDDIIntSource)
//  EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
//  EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
//  EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
//  EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
//  EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE)
//  EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR(IRTFatalGroupMask, NRTFatalGroupMask)
//  EDDI_SII_START_DO_MACRO_IN_ISR(pRQB, pGroupMarkerRqb, SII_DO_MACRO_NAME)
//  EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR(IRTGroupMaskIRTE, NRTGroupMaskIRTE)
//  EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR_NRT(IRTGroupMaskIRTE, NRTGroupMaskIRTE)
//  EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE, pGroupMarkerRqb, SII_DO_MACRO_NAME)
//  EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE, pGroupMarkerRqb, SII_DO_MACRO_NAME)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE)
//  EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP(pRQB)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, IntGroupIndex)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, IntGroupIndex)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)
//  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT_RX_LIMIT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex, NRT_RX_Limit_MaskIRTE)


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_MODIFY_IRTE_MASK_REGISTERS()            */
/*                                                                         */
/* D e s c r i p t i o n: modifies IRTE-mask-registers conditionally       */
/*                        - only used for EDDI-interrupts                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_POLLING_MODE)
//modifications of IRTE-mask-registers are not necessary (IMRs are static)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp)
#elif defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE) && !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_WITH_PNDEVDRV) //New-Cycle-Sync-Mode
//modifications of IRTE-mask-registers are not necessary (IMRs are static) (in new-cycle-sync-mode only 1 EDDI-interrupt is masked/unmasked otherwise)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp)
#elif defined (EDDI_CFG_SII_FLEX_MODE)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp)\
{\
    if ((pSIIComp)->SII_Mode == EDDI_SII_STANDARD_MODE)\
    {\
        EDDI_SII_IO_x32((pSIIComp)->IMR_IRT_Adr) = (pSIIComp)->IRT_Mask_IRTE;\
        EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE;\
    }\
}
#else //Standard-Mode or New-Cycle-Sync-Test-Mode (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_WITH_PNDEVDRV)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp)\
{\
    EDDI_SII_IO_x32((pSIIComp)->IMR_IRT_Adr) = (pSIIComp)->IRT_Mask_IRTE;\
    EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE;\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT()*/
/*                                                                         */
/* D e s c r i p t i o n: modifies IRTE-mask-registers conditionally       */
/*                        - only used for EDDI-interrupts                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_STD_MODE)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT(pSIIComp)\
{\
    /* only NRT interrupts are considered */\
    /* macro is only allowed on task-level (because PNDEVDRV) */\
    EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE;\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER()       */
/*                                                                         */
/* D e s c r i p t i o n: modifies IRTE-mask-registers conditionally       */
/*                        - only used for USER-interrupts                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_POLLING_MODE)
//modifications of IRTE-mask-registers are not necessary (IMRs are static)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp)
#elif defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp)\
{\
    EDDI_SII_IO_x32((pSIIComp)->IMR_IRT_Adr) = (pSIIComp)->IRT_Mask_IRTE & (LSA_UINT32)EDDI_SII_IRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE;\
    EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE & (LSA_UINT32)EDDI_SII_NRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE;\
}
#elif defined (EDDI_CFG_SII_FLEX_MODE)
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp)\
{\
    if ((pSIIComp)->SII_Mode == EDDI_SII_STANDARD_MODE)\
    {\
        EDDI_SII_IO_x32((pSIIComp)->IMR_IRT_Adr) = (pSIIComp)->IRT_Mask_IRTE;\
        EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE;\
    }\
}
#else //Standard-Mode
#define EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp)\
{\
    EDDI_SII_IO_x32((pSIIComp)->IMR_IRT_Adr) = (pSIIComp)->IRT_Mask_IRTE;\
    EDDI_SII_IO_x32((pSIIComp)->IMR_NRT_Adr) = (pSIIComp)->NRT_Mask_IRTE;\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_CALL_INTERRUPT_EVENT()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_CALL_INTERRUPT_EVENT(pDDB, EDDIIntSource)\
{\
    EDDI_ISR_T  *  const  pIsr = &(pDDB)->ISR_Table[EDDIIntSource];\
    \
    EDDI_LOWER_TRACE_01((pDDB)->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_CALL_INTERRUPT_EVENT->EDDI-Int-Source:0x%X", (EDDIIntSource));\
    \
    pIsr->fct((pDDB), pIsr->para_1);\
}
#else
#define EDDI_SII_CALL_INTERRUPT_EVENT(pDDB, EDDIIntSource)\
{\
    EDDI_ISR_T  *  const  pIsr = &(pDDB)->ISR_Table[EDDIIntSource];\
    \
    pIsr->fct((pDDB), pIsr->para_1);\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_EXE_EDDI_IRT_INT()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     IRTCheckBitIRTE: interrupt-bit to check          */
/*                        IRT_IrqIRTE:     sum of pending group-interrupts */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#define EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRTCheckBitIRTE, IRT_IrqIRTE, EDDIIntSource)\
{\
    if ((IRTCheckBitIRTE) & (IRT_IrqIRTE)) /*lint -e774 -e941*/\
    {\
        EDDI_SII_CALL_INTERRUPT_EVENT((pDDB), (EDDIIntSource));\
    }\
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_EXE_EDDI_NRT_INT()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     NRTCheckBitIRTE: interrupt-bit to check          */
/*                        NRT_IrqIRTE:     sum of pending group-interrupts */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#define EDDI_SII_EXE_EDDI_NRT_INT(pDDB, NRTCheckBitIRTE, NRT_IrqIRTE, EDDIIntSource)\
{\
    if ((NRTCheckBitIRTE) & (NRT_IrqIRTE)) /*lint -e774 -e941*/\
    {\
        EDDI_SII_CALL_INTERRUPT_EVENT((pDDB), (EDDIIntSource));\
    }\
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR()        */
/*                                                                         */
/* D e s c r i p t i o n: => interrupt-group is masked                     */
/*                        => DO-macro is started                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR->SPECIAL_TS_TRACE, Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#elif defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR->Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#else
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR()      */
/*                                                                         */
/* D e s c r i p t i o n: => RQB is busy                                   */
/*                        => interrupt-group remain masked                 */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR->SPECIAL_TS_TRACE, Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#elif defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR->Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#else
#define EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK()       */
/*                                                                         */
/* D e s c r i p t i o n: => interrupt-group remain masked                 */
/*                        the following variables from caller are used:    */
/*                        - pRQB                                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK->SPECIAL_TS_TRACE, Int-Group-Index:0x%X interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
    \
    if (((IRTGroupIrqIRTE) | (NRTGroupIrqIRTE)) == 0)\
    {\
        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK, SPECIAL_TS_TRACE, no interrupt error");\
        EDDI_Excp("EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK, no interrupt error", EDDI_FATAL_ERR_EXCP, (IntGroupIndex), pRQB);\
        return;\
    }\
}
#elif defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK->Int-Group-Index:0x%X interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
    \
    if (((IRTGroupIrqIRTE) | (NRTGroupIrqIRTE)) == 0)\
    {\
        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK, no interrupt error");\
        EDDI_Excp("EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK, no interrupt error", EDDI_FATAL_ERR_EXCP, (IntGroupIndex), pRQB);\
        return;\
    }\
}
#else
#define EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK()     */
/*                                                                         */
/* D e s c r i p t i o n: => interrupt-group remain masked                 */
/*                        => DO-macro is restarted                         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK->SPECIAL_TS_TRACE, Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#elif (!defined (EDDI_CFG_SII_POLLING_MODE) && !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE))
#if defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK->Int-Group-Index:0x%X new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupIrqIRTE), (NRTGroupIrqIRTE));\
}
#else
#define EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK(IntGroupIndex, IRTGroupIrqIRTE, NRTGroupIrqIRTE)
#endif
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK()       */
/*                                                                         */
/* D e s c r i p t i o n: => interrupt-group is unmasked again             */
/*                        => DO-macro is stopped                           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK->SPECIAL_TS_TRACE, Int-Group-Index:0x%X unmasking interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupMaskIRTE), (NRTGroupMaskIRTE));\
}
#elif defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK->Int-Group-Index:0x%X unmasking interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", (IntGroupIndex), (IRTGroupMaskIRTE), (NRTGroupMaskIRTE));\
}
#else
#define EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE)
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR()          */
/*                                                                         */
/* D e s c r i p t i o n: the following variables from caller are used:    */
/*                        - IRT_IrqIRTE                                    */
/*                        - NRT_IrqIRTE                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_REV5)
#define EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR(IRTFatalGroupMask, NRTFatalGroupMask)\
{\
    LSA_UINT32  const  GroupIRTFatal_IrqIRTE = IRT_IrqIRTE & (LSA_UINT32)(IRTFatalGroupMask);\
    LSA_UINT32  const  GroupNRTFatal_IrqIRTE = NRT_IrqIRTE & (LSA_UINT32)(NRTFatalGroupMask);\
    \
    if (GroupIRTFatal_IrqIRTE | GroupNRTFatal_IrqIRTE)\
    {\
        EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);\
        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, unexpected fatal Interrupt occured on Rev5");\
        EDDI_Excp("EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, unexpected fatal Interrupt occured on Rev5", EDDI_FATAL_ERR_EXCP,\
                  GroupIRTFatal_IrqIRTE, GroupNRTFatal_IrqIRTE);\
        return;\
    }\
}
#elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR(IRTFatalGroupMask, NRTFatalGroupMask)\
{\
    LSA_UINT32  const  GroupIRTFatal_IrqIRTE = IRT_IrqIRTE & (LSA_UINT32)(IRTFatalGroupMask);\
    LSA_UINT32  const  GroupNRTFatal_IrqIRTE = NRT_IrqIRTE & (LSA_UINT32)(NRTFatalGroupMask);\
    \
    if (GroupIRTFatal_IrqIRTE | GroupNRTFatal_IrqIRTE)\
    {\
        EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);\
        if (GroupIRTFatal_IrqIRTE & IRQ_CRT_LIST_FATAL_ERR)\
        {\
            EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, IRT-List-Error:0x%X occured", IO_R32(FATAL_IRT_ERROR));\
        }\
        if (GroupNRTFatal_IrqIRTE & IRQ_NRT_LIST_FATAL_ERR)\
        {\
            EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, NRT-List-Error:0x%X occured", IO_R32(FATAL_NRT_ERROR));\
        }\
        \
        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, unexpected fatal Interrupt occured");\
        EDDI_Excp("EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR, unexpected fatal Interrupt occured", EDDI_FATAL_ERR_EXCP,\
                  GroupIRTFatal_IrqIRTE, GroupNRTFatal_IrqIRTE);\
        return;\
    }\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_START_DO_MACRO_IN_ISR()                 */
/*                                                                         */
/* D e s c r i p t i o n: sets marker for starting DO-Macro later or       */
/*                        calls DO-Macro immediately                       */
/*                                                                         */
/* A r g u m e n t s:     pGroupMarkerRqb is used conditionally!           */
/*                        SII_DO_MACRO_NAME is used conditionally!         */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
#define EDDI_SII_START_DO_MACRO_IN_ISR(pRQB, pGroupMarkerRqb, SII_DO_MACRO_NAME)\
{\
    /* set marker for starting DO-Macro later => context-switch */\
    (pGroupMarkerRqb) = (pRQB);\
}
#elif defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
#define EDDI_SII_START_DO_MACRO_IN_ISR(pRQB, pGroupMarkerRqb, SII_DO_MACRO_NAME)\
{\
    /* call DO-Macro immediately => context-switch */\
    SII_DO_MACRO_NAME(pRQB);\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR()           */
/*                        EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR_NRT()       */
/*                                                                         */
/* D e s c r i p t i o n: masks interrupt-group in ISR.                    */
/*                        this macro always runs under lock-protection!    */
/*                        IRTGroupMaskIRTE and NRTGroupMaskIRTE are CONST! */
/*                        the following variables from caller are used:    */
/*                        - IRT_ChangeMaskIRTE                             */
/*                        - NRT_ChangeMaskIRTE                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR(IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    /* mask all group-interrupts (via SII-variables) (nrt-done-interrupts are always masked in New-Cycle-Sync-Mode) */\
    IRT_ChangeMaskIRTE |= (LSA_UINT32)(IRTGroupMaskIRTE);\
    NRT_ChangeMaskIRTE |= ((LSA_UINT32)(NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE));\
}
#elif !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR(IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    /* mask all group-interrupts (via SII-variables) */\
    IRT_ChangeMaskIRTE |= (LSA_UINT32)(IRTGroupMaskIRTE);\
    NRT_ChangeMaskIRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR_NRT(IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    /* mask all group-interrupts (via SII-variables) (nrt-done-interrupts are always masked in New-Cycle-Sync-Mode) */\
    NRT_ChangeMaskIRTE |= ((LSA_UINT32)(NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE));\
}
#elif !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR_NRT(IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    /* mask all group-interrupts (via SII-variables) */\
    NRT_ChangeMaskIRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR()          */
/*                        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT()      */
/*                                                                         */
/* D e s c r i p t i o n: checks interrupt-group in ISR and initiates      */
/*                        a context-switch if necessary.                   */
/*                        this macro always runs under lock-protection!    */
/*                        IRTGroupMaskIRTE and NRTGroupMaskIRTE are CONST! */
/*                        the following variables from caller are used:    */
/*                        - IRT_IrqIRTE                                    */
/*                        - NRT_IrqIRTE                                    */
/*                        - pSIIComp                                       */
/*                                                                         */
/* A r g u m e n t s:     pGroupMarkerRqb is used conditionally!           */
/*                        SII_DO_MACRO_NAME is used conditionally!         */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#define EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE, pGroupMarkerRqb, SII_DO_MACRO_NAME)\
{\
    register  EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB            = &pSIIComp->INTGroup[IntGroupIndex].IntRQB;\
    register  EDDI_SII_VOLATILE  LSA_UINT32              const  IRTGroupIrqIRTE = IRT_IrqIRTE & (LSA_UINT32)(IRTGroupMaskIRTE)/*lint -e941*/;\
    register  EDDI_SII_VOLATILE  LSA_UINT32              const  NRTGroupIrqIRTE = NRT_IrqIRTE & (LSA_UINT32)(NRTGroupMaskIRTE)/*lint -e941*/;\
    \
    /* both IRT and NRT interrupts are considered */\
    \
    if (IRTGroupIrqIRTE | NRTGroupIrqIRTE) /*lint -e941*/\
    {\
        if (pRQB->internal_context == EDDI_SII_RQB_FREE)\
        {\
            EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR((IntGroupIndex), IRTGroupIrqIRTE, NRTGroupIrqIRTE);\
            EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR(IRTGroupMaskIRTE, NRTGroupMaskIRTE);\
            /* modifications of IRTE-mask-registers are done later conditionally! */\
            /* store pending group-interrupts in RQB */\
            pRQB->internal_context_1    = IRTGroupIrqIRTE;\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            /* mark RQB as busy */\
            pRQB->internal_context = EDDI_SII_RQB_BUSY;\
            /* start DO-Macro */\
            EDDI_SII_START_DO_MACRO_IN_ISR(pRQB, pGroupMarkerRqb, SII_DO_MACRO_NAME);\
        }\
        else /* RQB is busy */\
        {\
            EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR((IntGroupIndex), IRTGroupIrqIRTE, NRTGroupIrqIRTE);\
            /* optimization: manipulate pending interrupts in busy RQB (perhaps interrupts are processed within current cycle) */\
            /*               only relevant for SW-generated interrupt-events! */\
            /* attention:    concurrent accesses to RQB! */\
            pRQB->internal_context_1    |= IRTGroupIrqIRTE;\
            pRQB->internal_context_Prio |= NRTGroupIrqIRTE;\
        }\
    }\
}
/*---------------------- end [subroutine] ---------------------------------*/

#define EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(IntGroupIndex, IRTGroupMaskIRTE, NRTGroupMaskIRTE, pGroupMarkerRqb, SII_DO_MACRO_NAME)\
{\
    register  EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB            = &pSIIComp->INTGroup[IntGroupIndex].IntRQB;\
    register  EDDI_SII_VOLATILE  LSA_UINT32              const  NRTGroupIrqIRTE = NRT_IrqIRTE & (LSA_UINT32)(NRTGroupMaskIRTE);\
    \
    /* only NRT interrupts are considered */\
    \
    if (NRTGroupIrqIRTE)\
    {\
        if (pRQB->internal_context == EDDI_SII_RQB_FREE)\
        {\
            EDDI_SII_INTERRUPT_GROUP_STARTED_IN_ISR((IntGroupIndex), 0, NRTGroupIrqIRTE);\
            EDDI_SII_MASK_INTERRUPT_GROUP_IN_ISR_NRT(0, NRTGroupMaskIRTE);\
            /* modifications of IRTE-mask-registers are done later conditionally! */\
            /* store pending group-interrupts in RQB */\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            /* mark RQB as busy */\
            pRQB->internal_context = EDDI_SII_RQB_BUSY;\
            /* start DO-Macro */\
            EDDI_SII_START_DO_MACRO_IN_ISR(pRQB, pGroupMarkerRqb, SII_DO_MACRO_NAME);\
        }\
        else /* RQB is busy */\
        {\
            EDDI_SII_INTERRUPT_GROUP_RETRIGGER_IN_ISR((IntGroupIndex), 0, NRTGroupIrqIRTE);\
            /* optimization: manipulate pending interrupts in busy RQB (perhaps interrupts are processed within current cycle) */\
            /*               only relevant for SW-generated interrupt-events! */\
            /* attention:    concurrent accesses to RQB! */\
            pRQB->internal_context_Prio |= NRTGroupIrqIRTE;\
        }\
    }\
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT()     */
/*                        EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR()         */
/*                                                                         */
/* D e s c r i p t i o n: checks interrupt-events in ISR and executes them */
/*                        we use different functions and special macros    */
/*                        here to give the compiler the chance to optimize */
/*                        away all unused interrupt checks.                */
/*                        the following variables from caller are used:    */
/*                        - IRT_IrqIRTE                                    */
/*                        - NRT_IrqIRTE                                    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE)\
{\
    register  LSA_UINT32  const  NRTGroupIrqIRTE = NRT_IrqIRTE & (LSA_UINT32)(NRTGroupMaskIRTE);\
    \
    /* only NRT interrupts are considered */\
    \
    if (NRTGroupIrqIRTE)\
    {\
        /* check single interrupt-bits */\
        /* execute pending interrupts  */\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_TIME_STAMP,                   NRTGroupIrqIRTE, EDDI_INT_Time_Stamp);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA1_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHB0);\
        /* EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR must be processed after TX-Done-Interrupts (1.empty 2.reload DMACW rings) */\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR, NRTGroupIrqIRTE, EDDI_SW_INT_Reload_NRTSendLists);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_HOL_OVERFLOW,                 NRTGroupIrqIRTE, EDDI_INT_Overflow_HOL);\
    }\
}
#endif //EDDI_CFG_SII_ADD_PRIO1_AUX
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP()  */
/*                                                                         */
/* D e s c r i p t i o n: sets marker in interrupt-restart-mode-loop for   */
/*                        "further group interrupts pending"               */
/*                        the following variables from caller are used:    */
/*                        - bFurtherGroupInterruptsPending                 */
/*                                                                         */
/* A r g u m e n t s:     pRQB is not used!                                */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
#define EDDI_SII_SET_MARKER_FOR_INT_RESTART_MODE_LOOP(pRQB)\
{\
    /* set marker for "further group interrupts pending" */\
    bFurtherGroupInterruptsPending = LSA_TRUE;\
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n: EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1()           */
/*                  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT()       */
/*                  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2()           */
/*                  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT()       */
/*                  EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT_RX_LIMIT()*/
/*                                                                         */
/* D e s c r i p t i o n: checks interrupt-events in TASK and executes them*/
/*                        we use different functions and special macros    */
/*                        here to give the compiler the chance to optimize */
/*                        away all unused interrupt checks.                */
/*                        IRTGroupMaskIRTE and NRTGroupMaskIRTE are CONST! */
/*                        the following variables from caller are used:    */
/*                        - pRQB                                           */
/*                                                                         */
/* A r g u m e n t s:     SII_HANDLE_DO_MACRO_NAME only relevant in        */
/*                        SII-FLEX-MODE and SII-STD-MODE                   */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, IntGroupIndex)\
{\
    register  EDDI_SII_VOLATILE  LSA_UINT32            IRTGroupIrqIRTE;\
    register  EDDI_SII_VOLATILE  LSA_UINT32            NRTGroupIrqIRTE;\
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;\
    \
    if /* stopping of interrupts not requested? */\
       (!pSIIComp->bStopInterrupts)\
    {\
        \
        /* both IRT and NRT interrupts are considered */\
        \
        /* read stored group-interrupts (only interrupts of this group are stored) */\
        IRTGroupIrqIRTE = pRQB->internal_context_1    & (LSA_UINT32)(IRTGroupMaskIRTE); /* & because of optimization */\
        NRTGroupIrqIRTE = pRQB->internal_context_Prio & (LSA_UINT32)(NRTGroupMaskIRTE); /* & because of optimization */\
        \
        EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK((IntGroupIndex), IRTGroupIrqIRTE, NRTGroupIrqIRTE);\
        \
        /* check single interrupt-bits */\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_CRT_SND_IRQ1,                 IRTGroupIrqIRTE, EDDI_SW_INT_NewCycleReduced);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA0_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHA0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA1_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHA1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB0_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHB0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB1_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHB1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_TIME_STAMP,                   NRTGroupIrqIRTE, EDDI_INT_Time_Stamp);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA1_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHB0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB1_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHB1);\
        /* EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR must be processed after TX-Done-Interrupts (1.empty 2.reload DMACW rings) */\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR, NRTGroupIrqIRTE, EDDI_SW_INT_Reload_NRTSendLists);\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_TRC_FIFO_ERR,                 IRTGroupIrqIRTE, EDDI_INT_FifoData_Lost);\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_TCW_DONE,                     IRTGroupIrqIRTE, EDDI_INT_TCW_Done);\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_ACW_LIST_TERM,                IRTGroupIrqIRTE, EDDI_INT_ACW_List_Term);\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_SRT_CYCLE_SKIPPED,            IRTGroupIrqIRTE, EDDI_INT_SRT_Cycle_Skipped);\
        EDDI_SII_EXE_EDDI_IRT_INT(pDDB, IRQ_SCORE_BOARD_CHANGED,          IRTGroupIrqIRTE, EDDI_INT_PS_Scoreboard_Changed);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_LINK_STATE_CHANGE,            NRTGroupIrqIRTE, EDDI_INT_Link_Status_Change);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_COMMAND_DONE,                 NRTGroupIrqIRTE, EDDI_INT_Instruction_Done);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_HOL_OVERFLOW,                 NRTGroupIrqIRTE, EDDI_INT_Overflow_HOL);\
        /*EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_LOW_WATER_MARK,               NRTGroupIrqIRTE, EDDI_INT_Underflow_NRT);*/\
    }

#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, IntGroupIndex)\
{\
    register  EDDI_SII_VOLATILE  LSA_UINT32            NRTGroupIrqIRTE;\
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;\
    \
    if /* stopping of interrupts not requested? */\
       (!pSIIComp->bStopInterrupts)\
    {\
        /* only NRT interrupts are considered */\
        \
        /* read stored group-interrupts (only interrupts of this group are stored) */\
        NRTGroupIrqIRTE = pRQB->internal_context_Prio & (LSA_UINT32)(NRTGroupMaskIRTE); /* & because of optimization */\
        \
        EDDI_SII_INTERRUPT_GROUP_STARTED_IN_TASK((IntGroupIndex), 0, NRTGroupIrqIRTE);\
        \
        /* check single interrupt-bits */\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA0_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHA0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA1_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHA1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB0_RCV_DONE,                NRTGroupIrqIRTE, EDDI_INT_Rcv_CHB0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_TIME_STAMP,                   NRTGroupIrqIRTE, EDDI_INT_Time_Stamp);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA0);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHA1_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHA1);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_CHB0_SND_DONE,                NRTGroupIrqIRTE, EDDI_INT_Snd_CHB0);\
        /* EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR must be processed after TX-Done-Interrupts (1.empty 2.reload DMACW rings) */\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR, NRTGroupIrqIRTE, EDDI_SW_INT_Reload_NRTSendLists);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_LINK_STATE_CHANGE,            NRTGroupIrqIRTE, EDDI_INT_Link_Status_Change);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_COMMAND_DONE,                 NRTGroupIrqIRTE, EDDI_INT_Instruction_Done);\
        EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_HOL_OVERFLOW,                 NRTGroupIrqIRTE, EDDI_INT_Overflow_HOL);\
        /*EDDI_SII_EXE_EDDI_NRT_INT(pDDB, IRQ_LOW_WATER_MARK,               NRTGroupIrqIRTE, EDDI_INT_Underflow_NRT);*/\
    }

#if defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
        }\
    }\
    else /* stopping of interrupts not requested */\
    {\
        /* unmask all group-interrupts (nrt-done-interrupts are always masked in new-cycle-sync-mode) */\
        pSIIComp->IRT_Mask_IRTE |=  (LSA_UINT32)(IRTGroupMaskIRTE);\
        pSIIComp->NRT_Mask_IRTE |= ((LSA_UINT32)(NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE));\
        /* no modifications of IRTE-mask-registers! */\
        /* mark RQB as free */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
        EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), (IRTGroupMaskIRTE), ((NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE)));\
    }\
}
#elif defined (EDDI_CFG_SII_FLEX_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
        }\
    }\
    else if (pSIIComp->SII_Mode == EDDI_SII_POLLING_MODE)\
    {\
        /* unmask all group-interrupts */\
        pSIIComp->IRT_Mask_IRTE |= (LSA_UINT32)(IRTGroupMaskIRTE);\
        pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
        /* no modifications of IRTE-mask-registers! */\
        /* mark RQB as free */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
        EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), (IRTGroupMaskIRTE), (NRTGroupMaskIRTE));\
    }\
    else /* Standard-Mode */\
    {\
        /* check new group-interrupts (group-interrupts are masked here) */\
        IRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr) & (LSA_UINT32)(IRTGroupMaskIRTE);\
        NRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & (LSA_UINT32)(NRTGroupMaskIRTE);\
        \
        if /* new group-interrupts pending? */\
           (IRTGroupIrqIRTE | NRTGroupIrqIRTE)\
        {\
            /* group-interrupts remain masked */\
            /* acknowledge pending group-interrupts */\
            EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = IRTGroupIrqIRTE;\
            EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = NRTGroupIrqIRTE;\
            /* store new pending group-interrupts in RQB*/\
            /* attention: concurrent accesses to RQB! */\
            pRQB->internal_context_1    = IRTGroupIrqIRTE;\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            /* RQB remains busy */\
            EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK((IntGroupIndex), IRTGroupIrqIRTE, NRTGroupIrqIRTE);\
            /* handle DO-Macro: e.g. call DO-Macro */\
            SII_HANDLE_DO_MACRO_NAME(pRQB);\
        }\
        else /* no new group-interrupts pending */\
        {\
            /* unmask all group-interrupts */\
            pSIIComp->IRT_Mask_IRTE |= (LSA_UINT32)(IRTGroupMaskIRTE);\
            pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
            /* modify IRTE-mask-registers */\
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp);\
            /* mark RQB as free */\
            pRQB->internal_context = EDDI_SII_RQB_FREE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), (IRTGroupMaskIRTE), (NRTGroupMaskIRTE));\
        }\
    }\
}
#elif defined (EDDI_CFG_SII_STD_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
        }\
    }\
    else /* stopping of interrupts not requested */\
    {\
        /* check new group-interrupts (group-interrupts are masked here) */\
        IRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr) & (LSA_UINT32)(IRTGroupMaskIRTE);\
        NRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & (LSA_UINT32)(NRTGroupMaskIRTE);\
        \
        if /* new group-interrupts pending? */\
           (IRTGroupIrqIRTE | NRTGroupIrqIRTE)\
        {\
            /* group-interrupts remain masked */\
            /* acknowledge pending group-interrupts */\
            EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = IRTGroupIrqIRTE;\
            EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = NRTGroupIrqIRTE;\
            /* store new pending group-interrupts in RQB */\
            /* attention: concurrent accesses to RQB! */\
            pRQB->internal_context_1    = IRTGroupIrqIRTE;\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            /* RQB remains busy */\
            EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK((IntGroupIndex), IRTGroupIrqIRTE, NRTGroupIrqIRTE);\
            /* handle DO-Macro: e.g. call DO-Macro */\
            SII_HANDLE_DO_MACRO_NAME(pRQB);\
        }\
        else /* no new group-interrupts pending */\
        {\
            /* unmask all group-interrupts */\
            pSIIComp->IRT_Mask_IRTE |= (LSA_UINT32)(IRTGroupMaskIRTE);\
            pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
            /* modify IRTE-mask-registers */\
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp);\
            /* mark RQB as free */\
            pRQB->internal_context = EDDI_SII_RQB_FREE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2);\
            EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), (IRTGroupMaskIRTE), (NRTGroupMaskIRTE));\
        }\
    }\
}
#endif

#if defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    /* only NRT interrupts are considered */\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)(pDDB->Glob.pCloseDevicePendingRQB->pParam);\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
        }\
    }\
    else /* stopping of interrupts not requested */\
    {\
        /* unmask all group-interrupts (nrt-done-interrupts are always masked in new-cycle-sync-mode) */\
        pSIIComp->NRT_Mask_IRTE |= ((LSA_UINT32)(NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE));\
        /* no modifications of IRTE-mask-registers! */\
        /* mark RQB as free */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
        EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, ((NRTGroupMaskIRTE) & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE)));\
    }\
}
#elif defined (EDDI_CFG_SII_FLEX_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    /* only NRT interrupts are considered */\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
        }\
    }\
    else if (pSIIComp->SII_Mode == EDDI_SII_POLLING_MODE)\
    {\
        /* unmask all group-interrupts */\
        pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
        /* no modifications of IRTE-mask-registers! */\
        /* mark RQB as free */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
        EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, (NRTGroupMaskIRTE));\
    }\
    else /* Standard-Mode */\
    {\
        /* check new group-interrupts (group-interrupts are masked here) */\
        NRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & (LSA_UINT32)(NRTGroupMaskIRTE);\
        \
        if /* new group-interrupts pending? */\
           (NRTGroupIrqIRTE)\
        {\
            /* group-interrupts remain masked */\
            /* acknowledge pending group-interrupts */\
            EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = NRTGroupIrqIRTE;\
            /* store new pending group-interrupts in RQB */\
            /* attention: concurrent accesses to RQB! */\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            /* RQB remains busy */\
            EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK((IntGroupIndex), 0, NRTGroupIrqIRTE);\
            /* handle DO-Macro: e.g. call DO-Macro */\
            SII_HANDLE_DO_MACRO_NAME(pRQB);\
        }\
        else /* no new group-interrupts pending */\
        {\
            /* unmask all group-interrupts */\
            pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
            /* modify IRTE-mask-registers */\
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT(pSIIComp);\
            /* mark RQB as free */\
            pRQB->internal_context = EDDI_SII_RQB_FREE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, (NRTGroupMaskIRTE));\
        }\
    }\
}
#elif defined (EDDI_CFG_SII_STD_MODE)
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex)\
    \
    /* only NRT interrupts are considered */\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)(pDDB->Glob.pCloseDevicePendingRQB->pParam);\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
        }\
    }\
    else /* stopping of interrupts not requested */\
    {\
        /* check new group-interrupts (group-interrupts are masked here) */\
        NRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & (LSA_UINT32)(NRTGroupMaskIRTE);\
        \
        if /* new group-interrupts pending? */\
           (NRTGroupIrqIRTE)\
        {\
            /* group-interrupts remain masked */\
            /* acknowledge pending group-interrupts */\
            EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = NRTGroupIrqIRTE;\
            /* store new pending group-interrupts in RQB */\
            /* attention: concurrent accesses to RQB! */\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            /* RQB remains busy */\
            EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK((IntGroupIndex), 0, NRTGroupIrqIRTE);\
            /* handle DO-Macro: e.g. call DO-Macro */\
            SII_HANDLE_DO_MACRO_NAME(pRQB);\
        }\
        else /* no new group-interrupts pending */\
        {\
            /* unmask all group-interrupts */\
            pSIIComp->NRT_Mask_IRTE |= (LSA_UINT32)(NRTGroupMaskIRTE);\
            /* modify IRTE-mask-registers */\
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT(pSIIComp);\
            /* mark RQB as free */\
            pRQB->internal_context = EDDI_SII_RQB_FREE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT);\
            EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, (NRTGroupMaskIRTE));\
        }\
    }\
}
#endif

#if defined (EDDI_CFG_SII_STD_MODE)
#if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) || defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
//alternative macro for Standard-Mode with NRT-RX-Limitation/Timeslicing
//ATTENTION: only NRT-RX-Done-Interrupts are considered => only tasks are supported that process NRT-RX-Done-Interrupts exclusively!
#define EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT_RX_LIMIT(pDDB, IRTGroupMaskIRTE, NRTGroupMaskIRTE, SII_HANDLE_DO_MACRO_NAME, IntGroupIndex, NRT_RX_Limit_MaskIRTE)\
    \
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
    if /* stopping of interrupts requested? */\
       (pSIIComp->bStopInterrupts)\
    {\
        /* mark RQB as free ==> stop SII-RQB */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        /* check if EDDI already waits for internal pending RQBs */\
        if (pDDB->Glob.pCloseDevicePendingRQB)\
        {\
            EDDI_UPPER_DDB_REL_PTR_TYPE  const  pCloseDevicePendingRqbParam = (EDDI_UPPER_DDB_REL_PTR_TYPE)pDDB->Glob.pCloseDevicePendingRQB->pParam;\
            \
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
            if (!(0 == pCloseDevicePendingRqbParam->Cbf))\
            {\
                /* no polling used */\
                EDDI_DeviceClosePart2(pDDB, pDDB->Glob.pCloseDevicePendingRQB); /* try to close device now. if successful, the cbf is called. */\
            }\
        }\
        else\
        {\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
        }\
    }\
    else if /* further relevant group-interrupts possible? (NRT_RX_Limit_MaskIRTE is dynamically changed!) */\
            ((NRT_RX_Limit_MaskIRTE) != (NRTGroupMaskIRTE))\
    {\
        EDDI_SII_VOLATILE  LSA_UINT32  const  NRT_RX_Limit_Negative_MaskIRTE = (LSA_UINT32)(~(NRT_RX_Limit_MaskIRTE));\
        \
        /* check new NRT-group-interrupts with the exception of active RX-Limit-Interrupts (group-interrupts are masked here) */\
        NRTGroupIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & (LSA_UINT32)(NRTGroupMaskIRTE) & NRT_RX_Limit_Negative_MaskIRTE;\
        \
        if /* new NRT-group-interrupts with the exception of active RX-Limit-Interrupts pending? */\
           (NRTGroupIrqIRTE)\
        {\
            /* group-interrupts remain masked */\
            /* acknowledge pending NRT-group-interrupts not necessary at NRT-RX-Done-interrupts! */\
            /* store new pending NRT-group-interrupts in RQB */\
            /* attention: concurrent accesses to RQB! */\
            pRQB->internal_context_Prio = NRTGroupIrqIRTE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
            /* RQB remains busy */\
            EDDI_SII_INTERRUPT_GROUP_RESTARTED_IN_TASK((IntGroupIndex), 0, NRTGroupIrqIRTE);\
            /* handle DO-Macro: e.g. call DO-Macro */\
            SII_HANDLE_DO_MACRO_NAME(pRQB);\
        }\
        else /* no new group-interrupts pending */\
        {\
            /* unmask all NRT-group-interrupts with the exception of active RX-Limit-Interrupts */\
            pSIIComp->NRT_Mask_IRTE |= ((LSA_UINT32)(NRTGroupMaskIRTE) & NRT_RX_Limit_Negative_MaskIRTE);\
            /* modify IRTE-mask-registers */\
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_IN_TASK_NRT(pSIIComp);\
            /* mark RQB as free */\
            pRQB->internal_context = EDDI_SII_RQB_FREE;\
            EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
            EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, (NRTGroupMaskIRTE));\
        }\
    }\
    else /* no further relevant group-interrupts possible (= complete interrupt-group is in NRT-RX-overload-state!) */\
    {\
        /* mark RQB as free */\
        pRQB->internal_context = EDDI_SII_RQB_FREE;\
        EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT);\
        EDDI_SII_INTERRUPT_GROUP_STOPPED_IN_TASK((IntGroupIndex), 0, (NRTGroupMaskIRTE));\
    }\
}
#endif
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIInit()                                   */
/*                                                                         */
/* D e s c r i p t i o n: configurates interrupt-processing                */
/*                        - interrupts are deactivated here!               */
/*                        - all old interrupts are acknowledged here!      */
/*                        - called at the end of EDDI_DeviceOpen()         */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    register  LSA_UINT32                               Ctr;
    register  LSA_UINT32                        const  IRTE_BaseAdr = pDDB->IRTE_SWI_BaseAdr_32Bit; //base of IRTE-register (for 32bit-access)
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp     = &pDDB->SII;
    register  EDD_SERVICE                              DoRqbService;

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInit->");

    for (Ctr = 0; Ctr < (LSA_UINT32)EDDI_SII_INT_GROUP_CNT; Ctr++)
    {
        SII_INT_GROUP_TYPE      *  const  pINTGroup = &pSIIComp->INTGroup[Ctr];
        EDD_UPPER_RQB_PTR_TYPE     const  pRQB      = &pINTGroup->IntRQB;

        //init RQB for context-switch via DO-macros
        EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_REQUEST);
        switch (Ctr)
        {
            case 0:  DoRqbService = (EDD_SERVICE)EDDI_SRV_EV_INTERRUPT_PRIO4_NRT_LOW; break;
            case 1:  DoRqbService = (EDD_SERVICE)EDDI_SRV_EV_INTERRUPT_PRIO3_REST;    break;
            case 2:  DoRqbService = (EDD_SERVICE)EDDI_SRV_EV_INTERRUPT_PRIO2_ORG;     break;
            #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
            case 3:  DoRqbService = (EDD_SERVICE)EDDI_SRV_EV_INTERRUPT_PRIO1_AUX;     break;
            #endif
            default:
            {
                EDDI_Excp("EDDI_SIIInit, invalid Ctr occurs during initialization of RQBs!", EDDI_FATAL_ERR_EXCP, Ctr, 0);
                return;
            }
        }
        EDD_RQB_SET_SERVICE(pRQB, DoRqbService);
        pRQB->internal_context      = EDDI_SII_RQB_FREE;            //RQB.internal_context     : contains marker: RQB free or busy
        pRQB->internal_context_1    = 0;                            //RQB.internal_context_1   : contains stored/pending IRT-interrupts of this interrupt-group in IRTE format (= little endian)
        pRQB->internal_context_Prio = 0;                            //RQB.internal_context_Prio: contains stored/pending NRT-interrupts of this interrupt-group in IRTE format (= little endian)
        pRQB->pParam                = (EDD_UPPER_MEM_PTR_TYPE)pDDB; //RQB.pParam               : contains pDDB
    }

    //SII_IrqSelector was already checked before
    switch (pSIIComp->SII_IrqSelector)
    {
        case EDDI_SII_IRQ_SP:
        {
            switch (pSIIComp->SII_IrqNumber)
            {
                case EDDI_SII_IRQ_0:
                {
                    //IRQ SP0
                    pSIIComp->IMode_Adr             = IRTE_BaseAdr + SP_IRQ_MODE;
                    pSIIComp->IR_IRT_Adr            = IRTE_BaseAdr + SP_IRQ0_IRT;
                    pSIIComp->IR_NRT_Adr            = IRTE_BaseAdr + SP_IRQ0_NRT;
                    pSIIComp->IRR_IRT_Adr           = IRTE_BaseAdr + SP_IRR_IRT;
                    pSIIComp->IRR_NRT_Adr           = IRTE_BaseAdr + SP_IRR_NRT;
                    pSIIComp->IAR_IRT_Adr           = IRTE_BaseAdr + SP_IRQ_ACK_IRT;
                    pSIIComp->IAR_NRT_Adr           = IRTE_BaseAdr + SP_IRQ_ACK_NRT;
                    pSIIComp->IMR_IRT_Adr           = IRTE_BaseAdr + SP_IRQ0_MASK_IRT;
                    pSIIComp->IMR_NRT_Adr           = IRTE_BaseAdr + SP_IRQ0_MASK_NRT;
                    pSIIComp->EOI_Adr               = IRTE_BaseAdr + SP_EOI_IRQ0;
                    pSIIComp->IMode_Int_Ack_Mask    = IRTE_INTERRUPT_MODE_BIT__R0;
                    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
                    pSIIComp->IMR_IRT_Opposite_Adr  = IRTE_BaseAdr + SP_IRQ1_MASK_IRT;
                    pSIIComp->IMR_NRT_Opposite_Adr  = IRTE_BaseAdr + SP_IRQ1_MASK_NRT;
                    #endif
                    break;
                }
                case EDDI_SII_IRQ_1:
                {
                    //IRQ SP1
                    pSIIComp->IMode_Adr             = IRTE_BaseAdr + SP_IRQ_MODE;
                    pSIIComp->IR_IRT_Adr            = IRTE_BaseAdr + SP_IRQ1_IRT;
                    pSIIComp->IR_NRT_Adr            = IRTE_BaseAdr + SP_IRQ1_NRT;
                    pSIIComp->IRR_IRT_Adr           = IRTE_BaseAdr + SP_IRR_IRT;
                    pSIIComp->IRR_NRT_Adr           = IRTE_BaseAdr + SP_IRR_NRT;
                    pSIIComp->IAR_IRT_Adr           = IRTE_BaseAdr + SP_IRQ_ACK_IRT;
                    pSIIComp->IAR_NRT_Adr           = IRTE_BaseAdr + SP_IRQ_ACK_NRT;
                    pSIIComp->IMR_IRT_Adr           = IRTE_BaseAdr + SP_IRQ1_MASK_IRT;
                    pSIIComp->IMR_NRT_Adr           = IRTE_BaseAdr + SP_IRQ1_MASK_NRT;
                    pSIIComp->EOI_Adr               = IRTE_BaseAdr + SP_EOI_IRQ1;
                    pSIIComp->IMode_Int_Ack_Mask    = IRTE_INTERRUPT_MODE_BIT__R1;
                    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
                    pSIIComp->IMR_IRT_Opposite_Adr  = IRTE_BaseAdr + SP_IRQ0_MASK_IRT;
                    pSIIComp->IMR_NRT_Opposite_Adr  = IRTE_BaseAdr + SP_IRQ0_MASK_NRT;
                    #endif
                    break;
                }
                default:
                {
                    EDDI_Excp("EDDI_SIIInit, invalid SII_IrqNumber:", EDDI_FATAL_ERR_EXCP, pSIIComp->SII_IrqNumber, 0);
                    return;
                }
            }
            break;
        }
        case EDDI_SII_IRQ_HP:
        {
            switch (pSIIComp->SII_IrqNumber)
            {
                case EDDI_SII_IRQ_0:
                {
                    //IRQ HP0
                    pSIIComp->IMode_Adr             = IRTE_BaseAdr + HP_IRQ_MODE;
                    pSIIComp->IR_IRT_Adr            = IRTE_BaseAdr + HP_IRQ0_IRT;
                    pSIIComp->IR_NRT_Adr            = IRTE_BaseAdr + HP_IRQ0_NRT;
                    pSIIComp->IRR_IRT_Adr           = IRTE_BaseAdr + HP_IRR_IRT;
                    pSIIComp->IRR_NRT_Adr           = IRTE_BaseAdr + HP_IRR_NRT;
                    pSIIComp->IAR_IRT_Adr           = IRTE_BaseAdr + HP_IRQ_ACK_IRT;
                    pSIIComp->IAR_NRT_Adr           = IRTE_BaseAdr + HP_IRQ_ACK_NRT;
                    pSIIComp->IMR_IRT_Adr           = IRTE_BaseAdr + HP_IRQ0_MASK_IRT;
                    pSIIComp->IMR_NRT_Adr           = IRTE_BaseAdr + HP_IRQ0_MASK_NRT;
                    pSIIComp->EOI_Adr               = IRTE_BaseAdr + HP_EOI_IRQ0;
                    pSIIComp->IMode_Int_Ack_Mask    = IRTE_INTERRUPT_MODE_BIT__R0;
                    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
                    pSIIComp->IMR_IRT_Opposite_Adr  = IRTE_BaseAdr + HP_IRQ1_MASK_IRT;
                    pSIIComp->IMR_NRT_Opposite_Adr  = IRTE_BaseAdr + HP_IRQ1_MASK_NRT;
                    #endif
                    break;
                }
                case EDDI_SII_IRQ_1:
                {
                    //IRQ HP1
                    pSIIComp->IMode_Adr             = IRTE_BaseAdr + HP_IRQ_MODE;
                    pSIIComp->IR_IRT_Adr            = IRTE_BaseAdr + HP_IRQ1_IRT;
                    pSIIComp->IR_NRT_Adr            = IRTE_BaseAdr + HP_IRQ1_NRT;
                    pSIIComp->IRR_IRT_Adr           = IRTE_BaseAdr + HP_IRR_IRT;
                    pSIIComp->IRR_NRT_Adr           = IRTE_BaseAdr + HP_IRR_NRT;
                    pSIIComp->IAR_IRT_Adr           = IRTE_BaseAdr + HP_IRQ_ACK_IRT;
                    pSIIComp->IAR_NRT_Adr           = IRTE_BaseAdr + HP_IRQ_ACK_NRT;
                    pSIIComp->IMR_IRT_Adr           = IRTE_BaseAdr + HP_IRQ1_MASK_IRT;
                    pSIIComp->IMR_NRT_Adr           = IRTE_BaseAdr + HP_IRQ1_MASK_NRT;
                    pSIIComp->EOI_Adr               = IRTE_BaseAdr + HP_EOI_IRQ1;
                    pSIIComp->IMode_Int_Ack_Mask    = IRTE_INTERRUPT_MODE_BIT__R1;
                    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
                    pSIIComp->IMR_IRT_Opposite_Adr  = IRTE_BaseAdr + HP_IRQ0_MASK_IRT;
                    pSIIComp->IMR_NRT_Opposite_Adr  = IRTE_BaseAdr + HP_IRQ0_MASK_NRT;
                    #endif
                    break;
                }
                default:
                {
                    EDDI_Excp("EDDI_SIIInit, invalid SII_IrqNumber:", EDDI_FATAL_ERR_EXCP, pSIIComp->SII_IrqNumber, 0);
                    return;
                }
            }
            break;
        }
        default:
        {
            EDDI_Excp("EDDI_SIIInit, invalid SII_IrqSelector:", EDDI_FATAL_ERR_EXCP, pSIIComp->SII_IrqSelector, 0);
            return;
        }
    }

    //init global parameters
    pSIIComp->IRT_Mask_IRTE                         = 0;
    pSIIComp->NRT_Mask_IRTE                         = 0;
    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE  = EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE;
    pSIIComp->HWClkCycleIntDividerForNRTCheck       = 0;
    #endif
    #if defined (EDDI_SII_DEBUG_MODE_TRACES) || defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES) || defined (EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES)
    pSIIComp->RegularInterruptCnt                   = 0;
    pSIIComp->EmptyInterruptCnt                     = 0;
    #endif
    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_SII_Critical_Ctr                           = 0;
    #endif
    #if defined (EDDI_CFG_REV5)
    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    pSIIComp->TwoTSIntWithSameTimeCnt               = 0;
    #endif
    #endif

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    //nothing todo!
    #else //Standard-Mode, Flex-Mode or New-Cycle-Sync-Mode
    #if defined (EDDI_CFG_SII_FLEX_MODE)
    if (pSIIComp->SII_Mode == EDDI_SII_STANDARD_MODE)
    #endif
    {
        //reset IRTE-mask-register: mask all interrupts by HW (all EDDI- and USER-interrupts)
        EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr) = 0;
        EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Adr) = 0;
    }
    #endif //EDDI_CFG_SII_POLLING_MODE

    //acknowledge all old interrupts (EDDI + USER)
    EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = (LSA_UINT32)0xFFFFFFFF;
    EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = (LSA_UINT32)0xFFFFFFFF;

	//generate EOI for security in IRTE
    EDDI_SII_IO_W32(pSIIComp->EOI_Adr, 0xFUL);

    //call LSA output macro
    EDDI_SII_USER_INTERRUPT_HANDLING_STARTED(pDDB->hDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIStartInterrupts()                        */
/*                                                                         */
/* D e s c r i p t i o n: starts interrupt-processing                      */
/*                        - interrupts are activated here!                 */
/*                        - called at the end of EDDI_DeviceSetup()        */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIStartInterrupts( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIStartInterrupts->");

    //check IRTE-interrupt-mode-register
    if (EDDI_SII_IO_x32(pSIIComp->IMode_Adr) & pSIIComp->IMode_Int_Ack_Mask)
    {
        EDDI_Excp("EDDI_SIIStartInterrupts, invalid IRTE-interrupt-mode-register (read-only-mode forbidden)!", EDDI_FATAL_ERR_EXCP, EDDI_SII_IO_x32(pSIIComp->IMode_Adr), pSIIComp->IMode_Int_Ack_Mask);
        return;
    }

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIISetNormalIsr->set device granular interrupt-function-pointer to normal interrupt-function");
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_START_INTS);

    EDDI_SIISetNormalIsr(pDDB);

    //unmask all selected EDDI-interrupts for SW
    pSIIComp->IRT_Mask_IRTE |=  EDDI_SII_IRT_SW_MASK_INIT_IRTE;
    pSIIComp->NRT_Mask_IRTE |= (EDDI_SII_NRT_SW_MASK_INIT_IRTE & (~EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE));

    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    if //ExtTimerMode on?
       (pSIIComp->SII_ExtTimerInterval)
    {
        //mask NewCycleInterrupt for SW
        pSIIComp->IRT_Mask_IRTE &= (~IRQ_NEW_CYCL);
    }
    else //ExtTimerMode off
    {
        //unmask NewCycleInterrupt for SW
        pSIIComp->IRT_Mask_IRTE |= IRQ_NEW_CYCL;
    }
    #endif

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    //nothing todo!
    #elif defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    {
        //initialize/update IRTE-mask-register: unmask interrupts by HW
        EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr) = pSIIComp->IRT_Mask_IRTE & (LSA_UINT32)EDDI_SII_IRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE;
        EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Adr) = pSIIComp->NRT_Mask_IRTE & (LSA_UINT32)EDDI_SII_NRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE;
    }
    #else //Standard-Mode or Flex-Mode
    #if defined (EDDI_CFG_SII_FLEX_MODE)
    if (pSIIComp->SII_Mode == EDDI_SII_STANDARD_MODE)
    #endif
    {
        //initialize/update IRTE-mask-register: unmask interrupts by HW
        EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr) = pSIIComp->IRT_Mask_IRTE;
        EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Adr) = pSIIComp->NRT_Mask_IRTE;
    }
    #endif //EDDI_CFG_SII_POLLING_MODE

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_START_INTS);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIStopInterrupts()                         */
/*                                                                         */
/* D e s c r i p t i o n: stops interrupt-processing                       */
/*                        - interrupts are deactivated here!               */
/*                        - called within EDDI_DeviceClose()               */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIStopInterrupts( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIStopInterrupts->");

    //call LSA output macro
    EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED(pDDB->hDDB);

    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIISetDummyIsr->set device granular interrupt-function-pointer to dummy-interrupt-function");
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_STOP_INTS);

    pSIIComp->bStopInterrupts = LSA_TRUE; //bStopInterrupts is reset in EDDI_CloseDDB() when nulling DDB!

    EDDI_SIISetDummyIsr(pDDB);

    //mask all interrupts for SW
    pSIIComp->IRT_Mask_IRTE = 0;
    pSIIComp->NRT_Mask_IRTE = 0;

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    //nothing todo!
    #else //Standard-Mode, Flex-Mode or New-Cycle-Sync-Mode
    #if defined (EDDI_CFG_SII_FLEX_MODE)
    if (pSIIComp->SII_Mode == EDDI_SII_STANDARD_MODE)
    #endif
    {
        //reset IRTE-mask-register: mask all interrupts by HW (all EDDI- and USER-interrupts)
        EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr) = 0;
        EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Adr) = 0;
    }
    #endif //EDDI_CFG_SII_POLLING_MODE

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_STOP_INTS);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIISetDummyIsr()                            */
/*                                                                         */
/* D e s c r i p t i o n: set device granular interrupt-function-pointer   */
/*                        to dummy-interrupt-function                      */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIISetDummyIsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    pDDB->SII.interrupt_fct_ptr = (EDDI_INTERRUPT_FCT)EDDI_SIIInterruptDummy;

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    pDDB->SII.interrupt_low_fct_ptr = (EDDI_LOW_INTERRUPT_FCT)EDDI_SIIInterruptLowDummy;
    #endif

    #if defined (EDDI_CFG_SYSRED_2PROC)
    pDDB->SII.polling_sysred_fct_ptr = (EDDI_SYSRED_POLLING_FCT)EDDI_SII_SysRedPollingDummy;
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIISetNormalIsr()                           */
/*                                                                         */
/* D e s c r i p t i o n: set device granular interrupt-function-pointer   */
/*                        to normal interrupt-function                     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIISetNormalIsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    pDDB->SII.interrupt_fct_ptr = (EDDI_INTERRUPT_FCT)EDDI_SIIInterruptWork;

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    pDDB->SII.interrupt_low_fct_ptr = (EDDI_LOW_INTERRUPT_FCT)EDDI_SIIInterruptLowWork;
    #endif

    #if defined (EDDI_CFG_SYSRED_2PROC)
    pDDB->SII.polling_sysred_fct_ptr = (EDDI_SYSRED_POLLING_FCT)EDDI_SII_SysRedPollingWork;
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)

/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIGetNewCycleReductionFactors()            */
/*                                                                         */
/* D e s c r i p t i o n: calculates NewCycle-reduction-factors for        */
/*                        reduced new-cycle-interrupt-event and            */
/*                        for NRT-interrupt-check                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactors( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      LSA_UINT32               const  CycleBaseFactor,
                                                                      LSA_UINT32               const  CycleLength_10ns,
                                                                      LSA_UINT32               const  CycleLength_us )
{
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp                = &pDDB->SII;
    LSA_UINT32                               NewCycleReductionFactor = (LSA_UINT32)((EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS * 100000UL) / CycleLength_10ns);
    LSA_UINT32                               NewCycleReductionFactorForNRTCheck;
    LSA_UINT32                               HWDividerForNRTCheck; //for Apl-Clk-Unit-signal "Clock-Cycle-Int"

    LSA_UNUSED_ARG(CycleLength_us);

    //calculates SW-reduction-factor for reduced new-cycle-interrupt-event
    if (NewCycleReductionFactor == 0)
    {
        NewCycleReductionFactor = 1UL;
    }

    #if ((EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 1000) || (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 500) || (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 250))
    //calculates SW- or HW-reduction-factor for NRT-interrupt-check in New-Cycle-Sync-Mode
    NewCycleReductionFactorForNRTCheck = (LSA_UINT32)(((EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US * 100UL) + (CycleLength_10ns / 2UL)) / CycleLength_10ns);

    if (NewCycleReductionFactorForNRTCheck > 1UL)
    {
        //Cycle length is <=1ms ==> use SW-reduction
        //deactivate HW-trigger (Apl-Clk-Unit)
        HWDividerForNRTCheck = 0;

        #if defined (EDDI_SII_DEBUG_MODE_TRACES)
        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIGetNewCycleReductionFactors, SII-New-Cycle-Sync-Mode, use SW-reduction");
        #endif
    }
    else
    {
        //Cycle length is >1ms ==> use Apl-Clk-Unit
        #if (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 1000)
            //Try to keep pollcycle between 750us and 1375us
            #define EDDI_SII_NEW_CYCLE_SCALING_FOR_NRT_CHECK_SECF_IN_US     500UL
        #elif (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 500)
            //Try to keep pollcycle between 375us and 625us
            #define EDDI_SII_NEW_CYCLE_SCALING_FOR_NRT_CHECK_SECF_IN_US     250UL
        #elif (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US == 250)
            //Try to keep pollcycle between 250us and 375us
            #define EDDI_SII_NEW_CYCLE_SCALING_FOR_NRT_CHECK_SECF_IN_US     0
        #endif
        LSA_UINT32  const  DivLocal = ((CycleBaseFactor * EDDI_CRT_CYCLE_LENGTH_GRANULARITY)+(EDDI_SII_NEW_CYCLE_SCALING_FOR_NRT_CHECK_SECF_IN_US*100UL))/(EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US*100UL);

        HWDividerForNRTCheck = (CycleBaseFactor + DivLocal - 1UL) / DivLocal;

        //deactivate SW-reduction
        NewCycleReductionFactorForNRTCheck = 0;

        #if defined (EDDI_SII_DEBUG_MODE_TRACES)
        EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIGetNewCycleReductionFactors, SII-New-Cycle-Sync-Mode, use HW-trigger (Apl-Clk-Unit), CBF:%d DivLocal:%d HWDividerForNRTCheck:%d PollCycle:%d",
                            CycleBaseFactor, DivLocal, HWDividerForNRTCheck, (HWDividerForNRTCheck * EDDI_CRT_CYCLE_LENGTH_GRANULARITY * 10UL));
        #endif
    }
    #else
    #error "Error in SII-Compiler-Switches: For EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_US only the values 1000, 500 or 250 are allowed!"
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIGetNewCycleReductionFactors, SII-New-Cycle-Sync-Mode, CycleBaseFactor:0x%X NewCycleReductionFactor:0x%X NewCycleReductionFactorForNRTCheck:0x%X HWDividerForNRTCheck:0x%X",
                        CycleBaseFactor, NewCycleReductionFactor, NewCycleReductionFactorForNRTCheck, HWDividerForNRTCheck);
    #endif

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_GET_NEWCYCLE_REDUCTION);

    //modify interrupt-masks for HW-trigger (Apl-Clk-Unit)
    if (HWDividerForNRTCheck)
    {
        //unmask clock-cycle-int-interrupt for SW
        pSIIComp->IRT_Mask_IRTE |= IRQ_CLK_CY_INT;
    }
    else
    {
        //mask clock-cycle-int-interrupt for SW
        pSIIComp->IRT_Mask_IRTE &= (~IRQ_CLK_CY_INT);
    }

    //modify IRTE-mask-register for IRT
    EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr) = pSIIComp->IRT_Mask_IRTE & (LSA_UINT32)EDDI_SII_IRT_NEW_CYCLE_SYNC_GROUP_MASK_IRTE;

    //acknowledge old clock-cycle-int-interrupt
    EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = IRQ_CLK_CY_INT;

    pDDB->NewCycleReductionFactor                = NewCycleReductionFactor;            //OUT
    pSIIComp->NewCycleReductionFactorForNRTCheck = NewCycleReductionFactorForNRTCheck; //OUT
    pSIIComp->HWClkCycleIntDividerForNRTCheck    = HWDividerForNRTCheck;               //OUT

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_GET_NEWCYCLE_REDUCTION);
}
/*---------------------- end [subroutine] ---------------------------------*/

#elif defined (EDDI_CFG_SII_POLLING_MODE)

/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIGetNewCycleReductionFactor()             */
/*                                                                         */
/* D e s c r i p t i o n: calculates NewCycle-reduction-factor for         */
/*                        reduced new-cycle-interrupt-event                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactor( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  CycleLength_10ns )
{
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp                = &pDDB->SII;
    LSA_UINT32                        const  NewCycleReductionFactor = (LSA_UINT32)((EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS * 100000UL) / CycleLength_10ns);

    //initialize variables
    pSIIComp->LastTimeStamp               = 0;
    pSIIComp->RestTimeNewCycleReduced10ns = 0;

    if (NewCycleReductionFactor == 0)
    {
        return 1UL;
    }

    return NewCycleReductionFactor;
}
/*---------------------- end [subroutine] ---------------------------------*/

#else

/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIGetNewCycleReductionFactor()             */
/*                                                                         */
/* D e s c r i p t i o n: calculates NewCycle-reduction-factor for         */
/*                        reduced new-cycle-interrupt-event                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
//Attention: calculation of reduction has to be for the same target time (here: 4ms) as in EDDI_SIIGetExtTimerReductionFactor!
//           Otherwise the formulas for calculating SWSB times will not be correct any more.
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactor( LSA_UINT32  const  CycleLength_10ns )
{
    LSA_UINT32  const  NewCycleReductionFactor = (LSA_UINT32)((EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS * 100000UL) / CycleLength_10ns);

    if (NewCycleReductionFactor == 0)
    {
        return 1UL;
    }

    return NewCycleReductionFactor;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIGetExtTimerReductionFactor()             */
/*                                                                         */
/* D e s c r i p t i o n: calculates ExtTimer-reduction-factor for         */
/*                        reduced new-cycle-interrupt-event                */
/*                        - pDDB->SII.SII_ExtTimerInterval != 0 is         */
/*                          checked before!                                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
//Attention: calculation of reduction has to be for the same target time (here: 4ms) as in EDDI_SIIGetNewCycleReductionFactor!
//           Otherwise the formulas for calculating SWSB times will not be correct any more.
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetExtTimerReductionFactor( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  const  ExtTimerReductionFactor = (LSA_UINT32)((EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS * 1000UL) / pDDB->SII.SII_ExtTimerInterval);

    return ExtTimerReductionFactor;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX

#endif //EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt()                                 */
/*                                                                         */
/* D e s c r i p t i o n: global SII-interrupt-function for USER           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt( const  EDDI_HANDLE  const  hDDB )
{
    register  EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    EDDI_ENTER_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);

    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    pDDB->SII.interrupt_fct_ptr(pDDB, LSA_FALSE /*bExtTimerInt*/);
    #else
    pDDB->SII.interrupt_fct_ptr(pDDB);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIInterruptDummy()                         */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptDummy( EDDI_DDB_TYPE  *  const  pDDB,
                                                                 LSA_BOOL          const  bExtTimerInt )
#else
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptDummy( EDDI_DDB_TYPE  *  const  pDDB )
#endif
{
    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptDummy->");
    #endif

    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    {
        EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

        if //SII already initialized?
           (pSIIComp->IAR_IRT_Adr)
        {
            //acknowledge clock-cycle-int-interrupt during EDDI startup (to avoid empty-interrupts)
            EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = IRQ_CLK_CY_INT;

            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptDummy, acknowledge clock-cycle-int-interrupt in NewCycleSyncMode");
            #endif
        }
    }
    #endif

    EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    LSA_UNUSED_ARG(bExtTimerInt); //satisfy lint!
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIInterruptWork()                          */
/*                                                                         */
/* D e s c r i p t i o n: SII-function for ISR                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptWork( EDDI_DDB_TYPE  *  const  pDDB,
                                                                LSA_BOOL          const  bExtTimerInt )
#else
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptWork( EDDI_DDB_TYPE  *  const  pDDB )
#endif
{
    register  EDDI_SII_VOLATILE  LSA_UINT32            IRT_IrqIRTE;
    register  EDDI_SII_VOLATILE  LSA_UINT32            NRT_IrqIRTE;
    EDDI_SII_VOLATILE  LSA_UINT32                      AllIRT_IrqIRTE;
    EDDI_SII_VOLATILE  LSA_UINT32                      AllNRT_IrqIRTE;
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp        = &pDDB->SII;
    #if !defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
	#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
	EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE	       pAuxRqb         = 0;
	#endif
	EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE	       pOrgRqb         = 0;
	EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE	       pRestRqb        = 0;
	EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE	       pNrtLowRqb      = 0;
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork->");
    #endif

	//read pending EDDI-interrupts via IRR
    AllIRT_IrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr);
    AllNRT_IrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr);
    IRT_IrqIRTE    = (AllIRT_IrqIRTE & pSIIComp->IRT_Mask_IRTE) & (~EDDI_SII_IRT_GROUP_MASK_USER_IRTE);
    NRT_IrqIRTE    = (AllNRT_IrqIRTE & pSIIComp->NRT_Mask_IRTE) & (~EDDI_SII_NRT_GROUP_MASK_USER_IRTE);

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_08(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIInterruptWork, START, SPECIAL_TS_TRACE, IRT_IrqIRTE(SW-unmasked-EDDI-Ints):0x%X IRT_IRR:0x%X Var IRT_Mask_IRTE:0x%X NRT_IrqIRTE(SW-unmasked-EDDI-Ints):0x%X NRT_IRR:0x%X Var NRT_Mask_IRTE:0x%X ClockCntVal:0x%X CycleLength_us:%d",
                        IRT_IrqIRTE, AllIRT_IrqIRTE, pSIIComp->IRT_Mask_IRTE, NRT_IrqIRTE, AllNRT_IrqIRTE, pSIIComp->NRT_Mask_IRTE, IO_R32(CLK_COUNT_VALUE), pDDB->CycCount.CycleLength_us);
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_07(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, IRT_IrqIRTE(SW-unmasked-EDDI-Ints):0x%X IRT_IRR:0x%X Var IRT_Mask_IRTE:0x%X NRT_IrqIRTE(SW-unmasked-EDDI-Ints):0x%X NRT_IRR:0x%X Var NRT_Mask_IRTE:0x%X CycleLength_us:%d",
                        IRT_IrqIRTE, AllIRT_IrqIRTE, pSIIComp->IRT_Mask_IRTE, NRT_IrqIRTE, AllNRT_IrqIRTE, pSIIComp->NRT_Mask_IRTE, pDDB->CycCount.CycleLength_us);
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
    {
        EDDI_SII_VOLATILE  LSA_UINT32  ImrIRTE;
        EDDI_SII_VOLATILE  LSA_UINT32  ImrOppositeIRTE;

        //read IRT-IMRs
        ImrIRTE         = EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Adr);
        ImrOppositeIRTE = EDDI_SII_IO_x32(pSIIComp->IMR_IRT_Opposite_Adr);

        if //same IRT-interrupts are unmasked both at IRQ0 and IRQ1?
           (ImrIRTE & ImrOppositeIRTE)
        {
            EDDI_Excp("EDDI_SIIInterruptWork, IRT IMR user handling error!", EDDI_FATAL_ERR_EXCP, ImrIRTE, ImrOppositeIRTE);
            EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);
            return;
        }

        //read NRT-IMRs
        ImrIRTE         = EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Adr);
        ImrOppositeIRTE = EDDI_SII_IO_x32(pSIIComp->IMR_NRT_Opposite_Adr);

        if //same NRT-interrupts are unmasked both at IRQ0 and IRQ1?
           (ImrIRTE & ImrOppositeIRTE)
        {
            EDDI_Excp("EDDI_SIIInterruptWork, NRT IMR user handling error!", EDDI_FATAL_ERR_EXCP, ImrIRTE, ImrOppositeIRTE);
            EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);
            return;
        }
    }
    #endif //EDDI_SII_DEBUG_MODE_CHECKS

    if //any SW-unmasked EDDI-interrupt pending?
       (IRT_IrqIRTE | NRT_IrqIRTE
        #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
        | bExtTimerInt
        #endif
       )
    {
        #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
        LSA_UINT32                               IrqCyclicTrigger;
        #endif
        //local variables for interrupt-mask-changes
        register  EDDI_SII_VOLATILE  LSA_UINT32  IRT_ChangeMaskIRTE = 0;
        register  EDDI_SII_VOLATILE  LSA_UINT32  NRT_ChangeMaskIRTE = 0;

        //acknowledge all pending interrupts
        EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = IRT_IrqIRTE;
        EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = NRT_IrqIRTE;

        //check fatal-interrupt-processing here
        EDDI_SII_CHECK_INTERRUPT_FATAL_IN_ISR(EDDI_SII_IRT_GROUP_MASK_FATAL_IRTE, EDDI_SII_NRT_GROUP_MASK_FATAL_IRTE);

        #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON)
        if //ExtTimerInterrupt pending?
           (bExtTimerInt)
        #elif defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
        if //ExtTimerMode on?
           (pSIIComp->SII_ExtTimerInterval)
        {
            IrqCyclicTrigger = (LSA_UINT32)bExtTimerInt;
        }
        else //ExtTimerMode off
        {
            IrqCyclicTrigger = IRT_IrqIRTE & IRQ_NEW_CYCL;
        }

        if //ExtTimerInterrupt or New-Cycle-interrupt pending?
           (IrqCyclicTrigger)
        #else
        if //New-Cycle-interrupt pending?
           (IRT_IrqIRTE & IRQ_NEW_CYCL)
        #endif
        {
            //****************************************************************** 
            #if defined (EDDI_CFG_SII_POLLING_MODE)
            //****************************************************************** 
            //POLLING MODE: NewCycleReduced is based on the check of time passed
            //No NRT timeslice emulation!
            LSA_UINT32  const  CurrentTimeStamp               = IO_R32(CLK_COUNT_BEGIN_VALUE);
            #if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
            LSA_UINT32  const  DeltaTime10ns                  = CurrentTimeStamp - pSIIComp->LastTimeStamp;
            #elif defined (EDDI_CFG_REV7)
            LSA_UINT32  const  DeltaTime10ns                  = (CurrentTimeStamp - pSIIComp->LastTimeStamp) / 10UL;
            #endif
            LSA_INT32   const  TmpRestTimeNewCycleReduced10ns = pSIIComp->RestTimeNewCycleReduced10ns - (LSA_INT32)DeltaTime10ns; //count down

            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NewCycleReduced checked in SII-Polling-Mode, CurrentTimeStamp:0x%X LastTimeStamp:0x%X RestTimeNewCycleReduced10ns:0x%X",
                                CurrentTimeStamp, pSIIComp->LastTimeStamp, pSIIComp->RestTimeNewCycleReduced10ns);
            #endif

            //store current timestamp
            pSIIComp->LastTimeStamp = CurrentTimeStamp;

            if //time for NewCycleReduced reached?
               (TmpRestTimeNewCycleReduced10ns <= 0)
            {
                #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NewCycleReduced occured in SII-Polling-Mode");
                #endif

                //execute actions in every reduced NewCycle (4ms)
                {
                    //trigger reduced new-cycle-execution
                    //generate IRTE-irt-interrupt "IRQ_CRT_SND_IRQ1" by SW for executing reduced new-cycle in REST-task.
                    //do not change without checking EDDI_NewCycleReduced() => IRQ_CRT_SND_IRQ1!
                    IRT_IrqIRTE |= IRQ_CRT_SND_IRQ1;
                    EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr) = IRQ_CRT_SND_IRQ1;

                    //trigger nrt-tx-reload-execution
                    #if defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
                    #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                    //generate IRTE-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" by SW for executing nrt-tx-reload in AUX-task. 
                    //do not change without checking EDDI_NRTReloadAllSendLists() => EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR!
                    NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) = EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    #elif !defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                    //generate SW-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" for executing nrt-tx-reload in this ISR
                    NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    #endif //EDDI_CFG_SII_ADD_PRIO1_AUX
                    #endif //EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION
                }

                //restart timeout - wait for next NewCycle
                pSIIComp->RestTimeNewCycleReduced10ns = (LSA_INT32)EDDI_SII_NEW_CYCLE_REDUCED_SCALING_10NS;
            }
            else //time for NewCycleReduced not yet reached
            {
                //wait for next NewCycle
                pSIIComp->RestTimeNewCycleReduced10ns = TmpRestTimeNewCycleReduced10ns;
            }
            //****************************************************************** 
            #elif !defined (EDDI_CFG_SII_POLLING_MODE)
            //****************************************************************** 
            //OTHER MODES: NewCycleReduced is based on reduction counters
            //****************************************************************** 
            #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON)
            //****************************************************************** 
            //read and increment reduction-counter for reduced NewCycle
            register  LSA_UINT32  TmpNewCycleReductionCtr = pDDB->ExtTimerReductionCtr + 1UL;

            if //new-cycle-reduction-factor reached?
               (TmpNewCycleReductionCtr >= pDDB->ExtTimerReductionFactor)
            //****************************************************************** 
            #elif defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
            //****************************************************************** 
            register  LSA_UINT32  TmpNewCycleReductionCtr;
            LSA_UINT32         *  pSelectReductionCtr;
            LSA_UINT32         *  pSelectReductionFactor;

            if //ExtTimerMode on?
               (pSIIComp->SII_ExtTimerInterval)
            {
                //read and increment reduction-counter for reduced NewCycle
                TmpNewCycleReductionCtr = pDDB->ExtTimerReductionCtr + 1UL;
                pSelectReductionCtr     = &(pDDB->ExtTimerReductionCtr);
                pSelectReductionFactor  = &(pDDB->ExtTimerReductionFactor);
            }
            else //ExtTimerMode off
            {
                //read and increment reduction-counter for reduced NewCycle
                TmpNewCycleReductionCtr = pDDB->NewCycleReductionCtr + 1UL;
                pSelectReductionCtr     = &(pDDB->NewCycleReductionCtr);
                pSelectReductionFactor  = &(pDDB->NewCycleReductionFactor);
            }

            if //new-cycle-reduction-factor reached?
               (TmpNewCycleReductionCtr >= *pSelectReductionFactor)
            //****************************************************************** 
            #else //ExtTimerMode off
            //****************************************************************** 
            //read and increment reduction-counter for reduced NewCycle
            register  LSA_UINT32  TmpNewCycleReductionCtr = pDDB->NewCycleReductionCtr + 1UL;

            if //new-cycle-reduction-factor reached?
               (TmpNewCycleReductionCtr >= pDDB->NewCycleReductionFactor)
            #endif
            {
                //execute actions in every reduced NewCycle (4ms)
                {
                    //trigger reduced new-cycle-execution
                    //generate IRTE-irt-interrupt "IRQ_CRT_SND_IRQ1" by SW for executing reduced new-cycle in REST-task.
                    //do not change without checking EDDI_NewCycleReduced() => IRQ_CRT_SND_IRQ1!
                    IRT_IrqIRTE |= IRQ_CRT_SND_IRQ1;
                    EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr) = IRQ_CRT_SND_IRQ1;

                    //trigger nrt-tx-reload-execution
                    #if defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
                    #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                    //generate IRTE-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" by SW for executing nrt-tx-reload in AUX-task. 
                    //do not change without checking EDDI_NRTReloadAllSendLists() => EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR!
                    NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) = EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    #elif !defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                    //generate SW-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" for executing nrt-tx-reload in this ISR
                    NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                    #endif
                    #endif

                    //NRT-RX-timeslice-emulation in reduced NewCycle (4ms)
                    {
                        #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) && (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION == 0)
                        if //NRT-RX-overload currently present?
                           (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitA0)
                        {
                            //signal NRT-RX-Done-interrupt to EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR()
                            //-NRT-RX-Done-interrupt is always pending in overload-state!
                            NRT_IrqIRTE |= (LSA_UINT32)IRQ_CHA0_RCV_DONE;
                            //reset selected NRT-RX-Done-interrupt-bit in RX-Limit-Mask
                            pSIIComp->NRT_RX_Limit_NRT_LOW_MaskIRTE &= (~((LSA_UINT32)IRQ_CHA0_RCV_DONE));
                            //ACK for pending NRT-RX-Done-interrupt is executed later (in RX-receive-loop)!
                            pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitA0 = 0;
                            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                            EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NRT-RX-Limitation/Timeslicing stopped for CHA0 in reduced NewCycle (4ms), NRT-RX-Done-Interrupt is pending");
                            #endif
                        }
                        #endif
                        #if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION) && (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION == 0)
                        if //NRT-RX-overload currently present?
                           (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitB0)
                        {
                            //signal NRT-RX-Done-interrupt to EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR()
                            //-NRT-RX-Done-interrupt is always pending in overload-state!
                            NRT_IrqIRTE |= (LSA_UINT32)IRQ_CHB0_RCV_DONE;
                            //reset selected NRT-RX-Done-interrupt-bit in RX-Limit-Mask
                            pSIIComp->NRT_RX_Limit_ORG_MaskIRTE &= (~((LSA_UINT32)IRQ_CHB0_RCV_DONE));
                            //ACK for pending NRT-RX-Done-interrupt is executed later (in RX-receive-loop)!
                            pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitB0 = 0;
                            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                            EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NRT-RX-Limitation/Timeslicing stopped for CHB0 in reduced NewCycle (4ms), NRT-RX-Done-Interrupt is pending");
                            #endif
                        }
                        #endif
                    } //end of NRT-RX-timeslice-emulation in reduced NewCycle (4ms)
                } //end of actions in every reduced NewCycle

                TmpNewCycleReductionCtr = 0;
            }

            //writeback reduction-counter
            #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON)
            pDDB->ExtTimerReductionCtr = TmpNewCycleReductionCtr;
            #elif defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
            *pSelectReductionCtr = TmpNewCycleReductionCtr;
            #else
            pDDB->NewCycleReductionCtr = TmpNewCycleReductionCtr;
            #endif
            #endif //EDDI_CFG_SII_POLLING_MODE

            #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
            if //SW-reduction is used? => clock-cycle-int-interrupt is masked?
               (pSIIComp->NewCycleReductionFactorForNRTCheck)
            {
                //read and increment reduction-counter for NRT-check in New-Cycle-Sync-Mode
                register  LSA_UINT32  TmpNewCycleReductionCtrForNRTCheck = pSIIComp->NewCycleReductionCtrForNRTCheck + 1UL;

                if //new-cycle-reduction-factor for NRT-check reached?
                   (TmpNewCycleReductionCtrForNRTCheck >= pSIIComp->NewCycleReductionFactorForNRTCheck)
                {
                    //calculate unmasked pending NRT-Done-interrupts (TX-Done + RX-Done)
                    LSA_UINT32  const  NRTDone_IrqIRTE = AllNRT_IrqIRTE & pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE;

                    //signal NRT-Done-interrupts to EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR()/EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT() or EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR().
                    //NRT-Done-interrupts (TX-Done + RX-Done) remain stored in IRR if RQB is currently not available.
                    NRT_IrqIRTE |= NRTDone_IrqIRTE;

                    //no ACK executed for pending NRT-Done-interrupts so far!

                    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                    EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, SII-New-Cycle-Sync-Mode with SW-reduction occured, NRT-Done-Interrupts are checked, AllNRTDone_IrqIRTE:0x%X NRTDone_IrqIRTE:0x%X NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE:0x%X CycleCntVal:%i",
                                        (AllNRT_IrqIRTE & EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE), NRTDone_IrqIRTE, pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE, IO_R32(CLK_COUNT_VALUE));
                    #endif

                    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
                    //strict mask-check
                    if (pSIIComp->NRT_Mask_IRTE & EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE)
                    {
                        EDDI_Excp("EDDI_SIIInterruptWork, Error: range of pSIIComp->NRT_Mask_IRTE", EDDI_FATAL_ERR_EXCP,
                                  pSIIComp->NRT_Mask_IRTE, 0);
                        EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);
                        return;
                    }
                    #endif

                    TmpNewCycleReductionCtrForNRTCheck = 0;
                }

                //writeback reduction-counter for NRT-check in New-Cycle-Sync-Mode
                pSIIComp->NewCycleReductionCtrForNRTCheck = TmpNewCycleReductionCtrForNRTCheck;
            }
            #endif //EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE

            //NRT-RX-timeslice-emulation in NewCycle
            {
                #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) && (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION > 0)
                if //NRT-RX-overload currently present?
                   (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitA0)
                {
                    pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitA0--;

                    if //NRT-RX-Done-interrupt may be checked again?
                       (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitA0 == (LSA_UINT32)0)
                    {
                        //signal NRT-RX-Done-interrupt to EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR()
                        //-NRT-RX-Done-interrupt is always pending in overload-state!
                        NRT_IrqIRTE |= (LSA_UINT32)IRQ_CHA0_RCV_DONE;
                        //reset selected NRT-RX-Done-interrupt-bit in RX-Limit-Mask
                        pSIIComp->NRT_RX_Limit_NRT_LOW_MaskIRTE &= (~((LSA_UINT32)IRQ_CHA0_RCV_DONE));
                        //ACK for pending NRT-RX-Done-interrupt is executed later (in RX-receive-loop)!
                        #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NRT-RX-Limitation/Timeslicing stopped for CHA0 in NewCycle, NRT-RX-Done-Interrupt is pending");
                        #endif
                    }
                }
                #endif
                #if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION) && (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION > 0)
                if //NRT-RX-overload currently present?
                   (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitB0)
                {
                    pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitB0--;

                    if //NRT-RX-Done-interrupt may be checked again?
                       (pSIIComp->NewCycleReductionCtrForNRTCheckRxLimitB0 == (LSA_UINT32)0)
                    {
                        //signal NRT-RX-Done-interrupt to EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR()
                        //-NRT-RX-Done-interrupt is always pending in overload-state!
                        NRT_IrqIRTE |= (LSA_UINT32)IRQ_CHB0_RCV_DONE;
                        //reset selected NRT-RX-Done-interrupt-bit in RX-Limit-Mask
                        pSIIComp->NRT_RX_Limit_ORG_MaskIRTE &= (~((LSA_UINT32)IRQ_CHB0_RCV_DONE));
                        //ACK for pending NRT-RX-Done-interrupt is executed later (in RX-receive-loop)!
                        #if defined (EDDI_SII_DEBUG_MODE_TRACES)
                        EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, NRT-RX-Limitation/Timeslicing stopped for CHB0 in NewCycle, NRT-RX-Done-Interrupt is pending");
                        #endif
                    }
                }
                #endif
            } //end of NRT-RX-timeslice-emulation in NewCycle

            //actions in every NewCycle
            {
                #if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)
                //ApplSyncSharedMode not allowed with ExtTimerMode! This is already checked before!
                EDDI_HANDLE  const  hDDB = (EDDI_HANDLE)pDDB;
                //call SII-output-macro
                EDDI_SII_APPLSYNC_NEWCYCLE(hDDB);
                #endif
                #if !defined (EDDI_CFG_SII_NRT_TX_RELOAD_REDUCTION)
                #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                //generate IRTE-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" by SW for executing nrt-tx-reload in AUX-task. do not change without checking EDDI_NRTReloadAllSendLists() => EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR!
                NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) = EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                #elif !defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
                //generate SW-nrt-interrupt "EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR" for executing nrt-tx-reload within this ISR
                NRT_IrqIRTE |= EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR;
                #endif
                #endif
            }
        } //end of: New-Cycle-interrupt pending/extTimer interrupt pending

        #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
        //if HW-trigger (Apl-Clk-Unit) is used? => clock-cycle-int-interrupt is unmasked?
        if //clock-cycle-int-interrupt pending?
           (IRT_IrqIRTE & IRQ_CLK_CY_INT)
        {
            //calculate unmasked pending NRT-Done-interrupts (TX-Done + RX-Done)
            LSA_UINT32  const  NRTDone_IrqIRTE = AllNRT_IrqIRTE & pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE;

            //signal NRT-Done-interrupts to EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR()/EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT() or EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR().
            //NRT-Done-interrupts (TX-Done + RX-Done) remain stored in IRR if RQB is currently not available.
            NRT_IrqIRTE |= NRTDone_IrqIRTE;

            //no ACK executed for pending NRT-Done-interrupts so far!

            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, SII-New-Cycle-Sync-Mode with HW-trigger occured, NRT-Done-Interrupts are checked, AllNRTDone_IrqIRTE:0x%X NRTDone_IrqIRTE:0x%X NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE:0x%X CycleCntVal:%i",
                                (AllNRT_IrqIRTE & EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE), NRTDone_IrqIRTE, pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE, IO_R32(CLK_COUNT_VALUE));
            #endif

            #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
            //strict mask-check
            if (pSIIComp->NRT_Mask_IRTE & EDDI_SII_NRT_GROUP_MASK_SPEC_NRT_DONE_IRTE)
            {
                EDDI_Excp("EDDI_SIIInterruptWork, Error: range of pSIIComp->NRT_Mask_IRTE", EDDI_FATAL_ERR_EXCP, pSIIComp->NRT_Mask_IRTE, 0);
                EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);
                return;
            }
            #endif
        }
        #endif //EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE

        //check interrupt groups and initiate context-switching if necessary
        #if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
        #if (EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE == 0)
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX,     0,                                          EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE,     pAuxRqb,    EDDI_DO_EV_INTERRUPT_PRIO1_AUX);
        #else
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX,         EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE,     EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE,     pAuxRqb,    EDDI_DO_EV_INTERRUPT_PRIO1_AUX);
        #endif
        #endif
        #if (EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE == 0)
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG,     0,                                          EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE,     pOrgRqb,    EDDI_DO_EV_INTERRUPT_PRIO2_ORG);
        #else
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG,         EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE,     EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE,     pOrgRqb,    EDDI_DO_EV_INTERRUPT_PRIO2_ORG);
        #endif
        #if (EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE == 0)
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(EDDI_SII_INT_GROUP_INDEX_PRIO3_REST,    0,                                          EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE,    pRestRqb,   EDDI_DO_EV_INTERRUPT_PRIO3_REST);
        #else
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(EDDI_SII_INT_GROUP_INDEX_PRIO3_REST,        EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE,    EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE,    pRestRqb,   EDDI_DO_EV_INTERRUPT_PRIO3_REST);
        #endif
        #if (EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE == 0)
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR_NRT(EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW, 0,                                          EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, pNrtLowRqb, EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW);
        #else
        EDDI_SII_CHECK_INTERRUPT_GROUP_IN_ISR(EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW,     EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, pNrtLowRqb, EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW);
        #endif

        //check if modifications in SW-interrupt-masks is necessary
        if (IRT_ChangeMaskIRTE | NRT_ChangeMaskIRTE)
        {
            #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
            EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIInterruptWork, SPECIAL_TS_TRACE, mask new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", IRT_ChangeMaskIRTE, NRT_ChangeMaskIRTE);
            #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, mask new interrupts IRT_IRTE:0x%X NRT_IRTE:0x%X", IRT_ChangeMaskIRTE, NRT_ChangeMaskIRTE);
            #endif

            //mask all selected interrupts for SW
            pSIIComp->IRT_Mask_IRTE &= (~IRT_ChangeMaskIRTE);
            pSIIComp->NRT_Mask_IRTE &= (~NRT_ChangeMaskIRTE);

            #if defined (EDDI_CFG_SII_NO_PNDEVDRV_SUPPORT)
            //modify IRTE-mask-registers conditionally for HW:
            // - in Polling-Mode (EDDI_CFG_SII_POLLING_MODE) modifications of IRTE-mask-registers are not necessary (no interrupts are statically unmasked)
            // - in New-Cycle-Sync-Mode (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE) modifications of IRTE-mask-registers are not necessary here (only a few interrupts are statically unmasked, 1 dynamic sii-interrupt left, which is masked/unmasked otherwise)
            // - otherwise IRTE-mask-registers are modified
            EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp);
            #endif
        }

        #if defined (EDDI_SII_DEBUG_MODE_TRACES) || defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES) || defined (EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES)
        pSIIComp->RegularInterruptCnt++;
        #endif
    }
    #if defined (EDDI_SII_DEBUG_MODE_TRACES) || defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES) || defined (EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES)
    else //no EDDI-interrupt pending
    {
        pSIIComp->EmptyInterruptCnt++;

        #if defined (EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES)
        if //any USER-interrupt is unmasked?
           ((pSIIComp->IRT_Mask_IRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE) | (pSIIComp->NRT_Mask_IRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE))
        {
            EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, no EDDI-interrupt pending, but USER-interrupts unmasked, RegularInterruptCnt:0x%X EmptyInterruptCnt:0x%X unmasked IRT-USER-interrupts:0x%X unmasked NRT-USER-interrupts:0x%X",
                                pSIIComp->RegularInterruptCnt, pSIIComp->EmptyInterruptCnt, (pSIIComp->IRT_Mask_IRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE), (pSIIComp->NRT_Mask_IRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE));
        }
        else //no USER-interrupt is unmasked
        {
            EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, no EDDI-interrupt pending, no USER-interrupt unmasked, RegularInterruptCnt:0x%X EmptyInterruptCnt:0x%X",
                                pSIIComp->RegularInterruptCnt, pSIIComp->EmptyInterruptCnt);
        }
        #endif
    }
    #endif

    #if !defined (EDDI_CFG_SII_NO_PNDEVDRV_SUPPORT)
    //modify IRTE-mask-registers conditionally for HW:
    // - in Polling-Mode (EDDI_CFG_SII_POLLING_MODE) modifications of IRTE-mask-registers are not necessary (no interrupts are statically unmasked)
    // - in New-Cycle-Sync-Mode (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE) modifications of IRTE-mask-registers are not necessary here (only a few interrupts are statically unmasked, 1 dynamic sii-interrupt left, which is masked/unmasked otherwise)
    // - otherwise IRTE-mask-registers are modified
    // - a writeback of the IRTE-mask-registers is always necessary with kernel-mode-driver (PNDEVDRV)!
    EDDI_SII_MODIFY_IRTE_MASK_REGISTERS(pSIIComp);
    #endif

    EDDI_EXIT_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);

    #if !defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
    //check interrupt-processing here (in ISR)
    #if (EDDI_SII_IRT_GROUP_MASK_PRIO0_ISR_IRTE == 0)
    EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO0_ISR_IRTE);
    #else    
    EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_ISR(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO0_ISR_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO0_ISR_IRTE);
    #endif
    #endif

    #if !defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
    //call DO-Macros for context-switch
	#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
	if (pAuxRqb)
	{
        EDDI_DO_EV_INTERRUPT_PRIO1_AUX(pAuxRqb);
	}
	#endif
	if (pOrgRqb)
	{
		EDDI_DO_EV_INTERRUPT_PRIO2_ORG(pOrgRqb);
	}
	if (pRestRqb)
	{
		EDDI_DO_EV_INTERRUPT_PRIO3_REST(pRestRqb);
	}
	if (pNrtLowRqb)
	{
		EDDI_DO_EV_INTERRUPT_PRIO4_NRT_LOW(pNrtLowRqb);
	}
    #endif

    #if defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
    {
        //set EOI here in IRTE interrupt controller

        #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIInterruptWork, EDDI_CFG_SII_USE_SPECIAL_EOI:0x%X", (LSA_UINT32)EDDI_CFG_SII_USE_SPECIAL_EOI);
        #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork, EDDI_CFG_SII_USE_SPECIAL_EOI:0x%X", (LSA_UINT32)EDDI_CFG_SII_USE_SPECIAL_EOI);
        #endif

        //generate EOI
        EDDI_SII_IO_W32(pSIIComp->EOI_Adr, EDDI_CFG_SII_USE_SPECIAL_EOI);

        if (pSIIComp->SII_IrqSelector == EDDI_SII_IRQ_HP)
        {
            //dummy-read to finish (pci)-write-access!
            EDDI_SII_VOLATILE  LSA_UINT32  const  Dummy = EDDI_SII_IO_x32(pSIIComp->EOI_Adr);

            LSA_UNUSED_ARG(Dummy); //satisfy lint!
        }
    }
    #elif !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
    //EOI is executed external!
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIInterruptWork, END, SPECIAL_TS_TRACE, RegularInterruptCnt:0x%X EmptyInterruptCnt:0x%X Var IRT_Mask_IRTE:0x%X Var NRT_Mask_IRTE:0x%X ClockCntVal:0x%X", pSIIComp->RegularInterruptCnt, pSIIComp->EmptyInterruptCnt, pSIIComp->IRT_Mask_IRTE, pSIIComp->NRT_Mask_IRTE, IO_R32(CLK_COUNT_VALUE));
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptWork<-RegularInterruptCnt:0x%X EmptyInterruptCnt:0x%X Var IRT_Mask_IRTE:0x%X Var NRT_Mask_IRTE:0x%X", pSIIComp->RegularInterruptCnt, pSIIComp->EmptyInterruptCnt, pSIIComp->IRT_Mask_IRTE, pSIIComp->NRT_Mask_IRTE);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
/***************************************************************************/
/* F u n c t i o n:       eddi_ExtTimerInterrupt()                         */
/*                                                                         */
/* D e s c r i p t i o n: global SII-interrupt-function for USER           */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_ExtTimerInterrupt( const  EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    //plausible SII_ExtTimerInterval
    if (pDDB->SII.SII_ExtTimerInterval == 0)
    {
        EDDI_Excp("eddi_ExtTimerInterrupt, SII_EXTTIMER_MODE_FLEX, invalid SII_ExtTimerInterval == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    #endif

    EDDI_ENTER_SII_ISR_CRITICAL_S(EDDI_SII_CALLER_ID_ISR);

    pDDB->SII.interrupt_fct_ptr(pDDB, LSA_TRUE /*bExtTimerInt*/);

    #ifdef EDDI_CFG_SYSRED_2PROC
        pDDB->SII.polling_sysred_fct_ptr(pDDB);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/
#else
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_ExtTimerInterrupt( const  EDDI_HANDLE  const  hDDB )
{
  LSA_UNUSED_ARG(hDDB);
}
#endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX


#if defined (EDDI_CFG_SYSRED_2PROC)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_SysRedPollingWork()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SII_SysRedPollingWork( EDDI_DDB_TYPE  *  const  pDDB )
{
    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_SysRedPollingWork->");
    #endif

    if (pDDB->pGSharedRAM->ChangeCount != pDDB->CRT.ProviderList.LastSetDS_ChangeCount)
    {
        if ((pDDB->CRT.ProviderList.SysRedPoll.bUsed) || (pDDB->Glob.pCloseDevicePendingRQB))
        {// if still in use  OR  all channels are closed do not send rqb
            return;
        }

        pDDB->CRT.ProviderList.SysRedPoll.bUsed = LSA_TRUE;

        EDDI_DO_SYSRED_POLL(pDDB->hSysDev, pDDB->hDDB, &pDDB->CRT.ProviderList.SysRedPoll.IntRQB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SII_SysRedPollingDummy()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SII_SysRedPollingDummy( EDDI_DDB_TYPE  *  const  pDDB )
{
    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SII_SysRedPollingDummy->");
    #endif

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif // EDDI_CFG_SYSRED_2PROC


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIEvPrio1Aux()                             */
/*                                                                         */
/* D e s c r i p t i o n: executes EDDI-event within eddi_request() after  */
/*                        context-switch by DO-macro                       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio1Aux( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    register  EDDI_DDB_TYPE  *  const  pDDB = (EDDI_DDB_TYPE *)(pRQB->pParam);

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIEvPrio1Aux, START, SPECIAL_TS_TRACE, ClockCntVal:0x%X", IO_R32(CLK_COUNT_VALUE));
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio1Aux->");
    #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
    {
        LSA_BOOL  bFurtherGroupInterruptsPending;

        do
        {
            bFurtherGroupInterruptsPending = LSA_FALSE;
    #endif

            #if (EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE == 0)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX);
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_AUX, EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX);
            #else
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX);
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO1_AUX_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_AUX, EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX);
            #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio1Aux, option EDDI_CFG_SII_INT_RESTART_MODE_LOOP used, bFurtherGroupInterruptsPending:0x%X", bFurtherGroupInterruptsPending);
            #endif
        }
        while (bFurtherGroupInterruptsPending);
    }
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SIIEvPrio1Aux, END, SPECIAL_TS_TRACE, ClockCntVal:0x%X", IO_R32(CLK_COUNT_VALUE));
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio1Aux<-");
    #endif
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIEvPrio2Org()                             */
/*                                                                         */
/* D e s c r i p t i o n: executes EDDI-event within eddi_request() after  */
/*                        context-switch by DO-macro                       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio2Org( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    register  EDDI_DDB_TYPE  *  const  pDDB = (EDDI_DDB_TYPE *)(pRQB->pParam);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio2Org->");
    #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
    {
        LSA_BOOL  bFurtherGroupInterruptsPending;

        do
        {
            bFurtherGroupInterruptsPending = LSA_FALSE;
    #endif

            #if (EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE == 0)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG);
            #else
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG);
            #endif
            #if (EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE == 0) && defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT_RX_LIMIT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_ORG, EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG, pDDB->SII.NRT_RX_Limit_ORG_MaskIRTE);
            #elif (EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE == 0)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_ORG, EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG);
            #else
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO2_ORG_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_ORG, EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG);
            #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio2Org, option EDDI_CFG_SII_INT_RESTART_MODE_LOOP used, bFurtherGroupInterruptsPending:0x%X", bFurtherGroupInterruptsPending);
            #endif
        }
        while (bFurtherGroupInterruptsPending);
    }
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio2Org<-");
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIEvPrio3Rest()                            */
/*                                                                         */
/* D e s c r i p t i o n: executes EDDI-event within eddi_request() after  */
/*                        context-switch by DO-macro                       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio3Rest( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    register  EDDI_DDB_TYPE  *  const  pDDB = (EDDI_DDB_TYPE *)(pRQB->pParam);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio3Rest->");
    #endif

    EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO3_REST);
    EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO3_REST_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO3_REST_IRTE, EDDI_DO_EV_INTERRUPT_PRIO3_REST, EDDI_SII_INT_GROUP_INDEX_PRIO3_REST);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio3Rest<-");
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIEvPrio4NRTLow()                          */
/*                                                                         */
/* D e s c r i p t i o n: executes EDDI-event within eddi_request() after  */
/*                        context-switch by DO-macro                       */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio4NRTLow( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    register  EDDI_DDB_TYPE  *  const  pDDB = (EDDI_DDB_TYPE *)(pRQB->pParam);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio4NRTLow->");
    #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
    {
        LSA_BOOL  bFurtherGroupInterruptsPending;

        do
        {
            bFurtherGroupInterruptsPending = LSA_FALSE;
    #endif

            #if (EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE == 0)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW);
            #else
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M1(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW);
            #endif
            #if (EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE == 0) && defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT_RX_LIMIT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_NRT_LOW, EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW, pDDB->SII.NRT_RX_Limit_NRT_LOW_MaskIRTE);
            #elif (EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE == 0)
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2_NRT(pDDB, 0, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_NRT_LOW, EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW);
            #else
            EDDI_SII_CHECK_INTERRUPT_EVENTS_IN_TASK_M2(pDDB, EDDI_SII_IRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_NRT_GROUP_MASK_PRIO4_NRT_LOW_IRTE, EDDI_SII_SELECT_DO_MACRO_NAME_NRT_LOW, EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW);
            #endif

    #if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
            #if defined (EDDI_SII_DEBUG_MODE_TRACES)
            EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio4NRTLow, option EDDI_CFG_SII_INT_RESTART_MODE_LOOP used, bFurtherGroupInterruptsPending:0x%X", bFurtherGroupInterruptsPending);
            #endif
        }
        while (bFurtherGroupInterruptsPending);
    }
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIEvPrio4NRTLow<-");
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_CFG_SII_POLLING_MODE)

/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_low()                             */
/*                                                                         */
/* D e s c r i p t i o n: specific SII-interrupt-function for USER         */
/*                        for low-context-polling.                         */
/*                        e.g. this function can be used in an IDLE-task.  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_low( const  EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;

    pDDB->SII.interrupt_low_fct_ptr(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIInterruptLowWork()                       */
/*                                                                         */
/* D e s c r i p t i o n: specific SII-interrupt-function for USER         */
/*                        for low-context-polling                          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptLowWork( EDDI_DDB_TYPE  *  const  pDDB )
{
    volatile  LSA_UINT32                     AllNRT_IrqIRTE;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptLowWork->");
    #endif

	//read pending EDDI-NRT-interrupts via IRR
    AllNRT_IrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptLowWork, NRT_IRR:0x%X Var NRT_Mask_IRTE:0x%X",
                        AllNRT_IrqIRTE, pSIIComp->NRT_Mask_IRTE);
    #endif

    if (AllNRT_IrqIRTE & IRQ_CHA0_SND_DONE_SELECTOR)
    {
        #if (!defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS) || defined (EDDI_CFG_SII_ADD_PRIO1_AUX))
        EDDI_INTNRTTxDone(pDDB, EDDI_NRT_CHA_IF_0); //lock-macros conditionally inside
        #else
        EDDI_NRT_CHX_SS_IF_TYPE  *  const  pIF = &pDDB->NRT.IF[EDDI_NRT_CHA_IF_0];

        pIF->Tx.LockFct(pIF);
        EDDI_INTNRTTxDone(pDDB, EDDI_NRT_CHA_IF_0); //lock-macros conditionally inside
        pIF->Tx.UnLockFct(pIF);
        #endif
    }
    if (AllNRT_IrqIRTE & IRQ_CHA0_RCV_DONE_SELECTOR)
    {
        EDDI_NRTRxDoneInt(pDDB, EDDI_NRT_CHA_IF_0); //lock-macros always inside
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SIIInterruptLowDummy()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInterruptLowDummy( EDDI_DDB_TYPE  *  const  pDDB )
{
    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIIInterruptLowDummy->");
    #endif

    LSA_UNUSED_ARG(pDDB); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_SII_POLLING_MODE


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_set_eoi()                         */
/*                                                                         */
/* D e s c r i p t i o n: Set EOI in IRTE interrupt controller             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_set_eoi( const  EDDI_HANDLE  const  hDDB,
	                                                     LSA_UINT32          const  EOIInactiveTime )
{
    register  EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB     = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    register  EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    //check EOIInactiveTime
    #if defined (EDDI_SII_MAX_EOI_INACTIVE_TIME)
    if (EOIInactiveTime > (LSA_UINT32)EDDI_SII_MAX_EOI_INACTIVE_TIME)
    {
        EDDI_Excp("eddi_interrupt_set_eoi, Error: range of EOIInactiveTime", EDDI_FATAL_ERR_EXCP,
                  EOIInactiveTime, (LSA_UINT32)EDDI_SII_MAX_EOI_INACTIVE_TIME);
        return;
    }
    #else
    if (EOIInactiveTime > pSIIComp->SII_MaxEoiInactiveTime)
    {
        EDDI_Excp("eddi_interrupt_set_eoi, Error: range of EOIInactiveTime", EDDI_FATAL_ERR_EXCP,
                  EOIInactiveTime, pSIIComp->SII_MaxEoiInactiveTime);
        return;
    }
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_interrupt_set_eoi->EOIInactiveTime:0x%X", EOIInactiveTime);
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_interrupt_set_eoi->EOIInactiveTime:0x%X", EOIInactiveTime);
    #endif

    //generate EOI
    EDDI_SII_IO_W32(pSIIComp->EOI_Adr, EOIInactiveTime);

    if (pSIIComp->SII_IrqSelector == EDDI_SII_IRQ_HP)
    {
        //dummy-read to finish (pci)-write-access!
        EDDI_SII_VOLATILE  LSA_UINT32  const  Dummy = EDDI_SII_IO_x32(pSIIComp->EOI_Adr);

        LSA_UNUSED_ARG(Dummy); //satisfy lint!
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_mask_user_int()                   */
/*                                                                         */
/* D e s c r i p t i o n: input-function for User-Int-Share-Mode only:     */
/*                        - only modifies selected USER-interrupts         */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT:   EDD_STS_OK                         */
/*                                      EDD_STS_ERR_PARAM                  */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_mask_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                 LSA_UINT32          const  IRTIrqIRTE,
                                                                 LSA_UINT32          const  NRTIrqIRTE )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB            = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp        = &pDDB->SII;
    LSA_UINT32                        const  UserIRTMaskIRTE = IRTIrqIRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE;
    LSA_UINT32                        const  UserNRTMaskIRTE = NRTIrqIRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE;

    if //a non-USER-interrupt is requested?
       ((IRTIrqIRTE & (~EDDI_SII_IRT_GROUP_MASK_USER_IRTE)) | (NRTIrqIRTE & (~EDDI_SII_NRT_GROUP_MASK_USER_IRTE)))
    {
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_MASK_USER_INTS);

    //mask all selected USER-interrupts for SW
    pSIIComp->IRT_Mask_IRTE &= (~UserIRTMaskIRTE);
    pSIIComp->NRT_Mask_IRTE &= (~UserNRTMaskIRTE);

    //modify IRTE-mask-registers conditionally for HW
    EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp);

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_MASK_USER_INTS);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_unmask_user_int()                 */
/*                                                                         */
/* D e s c r i p t i o n: input-function for User-Int-Share-Mode only:     */
/*                        - only modifies selected USER-interrupts         */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT:   EDD_STS_OK                         */
/*                                      EDD_STS_ERR_PARAM                  */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_unmask_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                   LSA_UINT32          const  IRTIrqIRTE,
                                                                   LSA_UINT32          const  NRTIrqIRTE )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB            = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp        = &pDDB->SII;
    LSA_UINT32                        const  UserIRTMaskIRTE = IRTIrqIRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE;
    LSA_UINT32                        const  UserNRTMaskIRTE = NRTIrqIRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE;

    if //a non-USER-interrupt is requested?
       ((IRTIrqIRTE & (~EDDI_SII_IRT_GROUP_MASK_USER_IRTE)) | (NRTIrqIRTE & (~EDDI_SII_NRT_GROUP_MASK_USER_IRTE)))
    {
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_UNMASK_USER_INTS);

    //unmask all selected USER-interrupts for SW
    pSIIComp->IRT_Mask_IRTE |= UserIRTMaskIRTE;
    pSIIComp->NRT_Mask_IRTE |= UserNRTMaskIRTE;

    //modify IRTE-mask-registers conditionally for HW
    EDDI_SII_MODIFY_IRTE_MASK_REGISTERS_USER(pSIIComp);

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_UNMASK_USER_INTS);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_ack_user_int()                    */
/*                                                                         */
/* D e s c r i p t i o n: input-function for User-Int-Share-Mode only:     */
/*                        - only acknowledge selected USER-interrupts      */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT:   EDD_STS_OK                         */
/*                                      EDD_STS_ERR_PARAM                  */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_ack_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                LSA_UINT32          const  IRTIrqIRTE,
                                                                LSA_UINT32          const  NRTIrqIRTE )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB           = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp       = &pDDB->SII;
    LSA_UINT32                        const  UserIRTIrqIRTE = IRTIrqIRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE;
    LSA_UINT32                        const  UserNRTIrqIRTE = NRTIrqIRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE;

    if //a non-USER-interrupt is requested?
       ((IRTIrqIRTE & (~EDDI_SII_IRT_GROUP_MASK_USER_IRTE)) | (NRTIrqIRTE & (~EDDI_SII_NRT_GROUP_MASK_USER_IRTE)))
    {
        return EDD_STS_ERR_PARAM;
    }

    //acknowledge all selected USER-interrupts
    EDDI_SII_IO_x32(pSIIComp->IAR_IRT_Adr) = UserIRTIrqIRTE;
    EDDI_SII_IO_x32(pSIIComp->IAR_NRT_Adr) = UserNRTIrqIRTE;

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_interrupt_ack_user_int->SPECIAL_TS_TRACE, IRT_IrqIRTE(ACK-User-Ints):0x%X NRT_IrqIRTE(ACK-User-Ints):0x%X", UserIRTIrqIRTE, UserNRTIrqIRTE);
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_interrupt_ack_user_int->IRT_IrqIRTE(ACK-User-Ints):0x%X NRT_IrqIRTE(ACK-User-Ints):0x%X", UserIRTIrqIRTE, UserNRTIrqIRTE);
    #endif

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_get_user_int()                    */
/*                                                                         */
/* D e s c r i p t i o n: input-function for User-Int-Share-Mode only:     */
/*                        - only reads unmasked USER-interrupts            */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT:   EDD_STS_OK                         */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_get_user_int( const  EDDI_HANDLE     const  hDDB,
                                                                LSA_UINT32          *  const  pIRTIrqIRTE,
                                                                LSA_UINT32          *  const  pNRTIrqIRTE )
{
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB     = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_GET_USER_INTS);

    //reads unmasked USER-interrupts
    *pIRTIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_IRT_Adr) & pSIIComp->IRT_Mask_IRTE & EDDI_SII_IRT_GROUP_MASK_USER_IRTE;
    *pNRTIrqIRTE = EDDI_SII_IO_x32(pSIIComp->IRR_NRT_Adr) & pSIIComp->NRT_Mask_IRTE & EDDI_SII_NRT_GROUP_MASK_USER_IRTE;

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_GET_USER_INTS);

    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "eddi_interrupt_get_user_int->SPECIAL_TS_TRACE, IRT_IrqIRTE(SW-unmasked-User-Ints):0x%X NRT_IrqIRTE(SW-unmasked-User-Ints):0x%X", *pIRTIrqIRTE, *pNRTIrqIRTE);
    #elif defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_interrupt_get_user_int->IRT_IrqIRTE(SW-unmasked-User-Ints):0x%X NRT_IrqIRTE(SW-unmasked-User-Ints):0x%X", *pIRTIrqIRTE, *pNRTIrqIRTE);
    #endif

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_set_nrt_rx_int()                  */
/*                                                                         */
/* D e s c r i p t i o n: Enable or disable selected NRT RX Interrupt:     */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:     hDDB:            Device handle                   */
/*                        NRTChannel:      EDDI_NRT_CHANEL_A_IF_0,         */
/*                                         EDDI_NRT_CHANEL_B_IF_0,         */
/*                        bEnableNrtRxInt: LSA_TRUE  = enable  interrupt   */
/*                                         LSA_FALSE = disable interrupt   */
/*                                                                         */
/* Return Value:          LSA_RESULT:      EDD_STS_OK                      */
/*                                         EDD_STS_ERR_PARAM               */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_set_nrt_rx_int( const  EDDI_HANDLE  const  hDDB,
                                                                  LSA_UINT32          const  NRTChannel,
                                                                  LSA_UINT8           const  bEnableNrtRxInt )
{
    LSA_RESULT                               Result   = EDD_STS_OK;
    EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB     = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE  const  pSIIComp = &pDDB->SII;

    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_SET_NRT_RX_INT);

    if //NRT RX Interrupt must be enabled?
       (EDDI_SII_ENABLE_NRT_RX_INT == bEnableNrtRxInt)
    {
        switch (NRTChannel)
        {
            case EDDI_NRT_CHANEL_A_IF_0: //A0
            {
                pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE |= IRQ_CHA0_RCV_DONE_SELECTOR;
                break;
            }
            case EDDI_NRT_CHANEL_B_IF_0: //B0
            {
                pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE |= IRQ_CHB0_RCV_DONE_SELECTOR;
                break;
            }

            default:
            {
                Result = EDD_STS_ERR_PARAM;
            }
        }
    }
    else if (EDDI_SII_DISABLE_NRT_RX_INT == bEnableNrtRxInt) //NRT RX Interrupt must be disabled
    {
        switch (NRTChannel)
        {
            case EDDI_NRT_CHANEL_A_IF_0: //A0
            {
                pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE &= (~IRQ_CHA0_RCV_DONE_SELECTOR);
                break;
            }
            case EDDI_NRT_CHANEL_B_IF_0: //B0
            {
                pSIIComp->NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE &= (~IRQ_CHB0_RCV_DONE_SELECTOR);
                break;
            }
            default:
            {
                Result = EDD_STS_ERR_PARAM;
            }
        }
    }
    else
    {   
        Result = EDD_STS_ERR_PARAM;
    }

    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_SET_NRT_RX_INT);

    #if defined (EDDI_SII_DEBUG_MODE_TRACES)
    EDDI_LOWER_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "eddi_interrupt_set_nrt_rx_int->bEnableNrtRxInt:0x%X NRTChannel:0x%X Result:0x%X", bEnableNrtRxInt, NRTChannel, Result);
    #endif

    return Result;
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/



/*****************************************************************************/
/*  end of file eddi_sii.c                                                   */
/*****************************************************************************/


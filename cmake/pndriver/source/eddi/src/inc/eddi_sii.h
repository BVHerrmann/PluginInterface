#ifndef EDDI_SII_H              //reinclude-protection
#define EDDI_SII_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_sii.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDDI-SII (standard interrupt integration)        */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*****************************************************************************/

/*===========================================================================*/
/*                          internal defines                                 */
/*===========================================================================*/

//internal compilerswitches
#undef EDDI_SII_DEBUG_MODE_TRACES               //enables full lsa-traces in SII
#undef EDDI_SII_DEBUG_MODE_CHECKS               //enables additional security-checks in SII
#undef EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES    //enables special lsa-traces in SII for REV5-Timestamp-Tests (SII-Standard-Mode, EDDI_CFG_SII_ADD_PRIO1_AUX)
#undef EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES     //enables empty-interrupt-lsa-traces in SII (counts empty EDDI-interrupts, but USER-interrupts can exist in USER-Int-Share-mode!)

#define EDDI_SII_VOLATILE   /* defined empty! */

//Defines for number of interrupt groups
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_INT_GROUP_CNT                  4
#else
#define EDDI_SII_INT_GROUP_CNT                  3
#endif

#define EDDI_SII_INT_GROUP_INDEX_PRIO4_NRT_LOW  0   //lowest prio
#define EDDI_SII_INT_GROUP_INDEX_PRIO3_REST     1
#define EDDI_SII_INT_GROUP_INDEX_PRIO2_ORG      2
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SII_INT_GROUP_INDEX_PRIO1_AUX      3   //highest prio
#endif

#if !defined (EDDI_CFG_SII_POLLING_MODE) && !defined (EDDI_CFG_SII_FLEX_MODE) && !defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_CFG_SII_STD_MODE
#endif

#define EDDI_IRQ_SLL_RELOAD_NRT_SELECTOR            IRQ_CHB1_SND_DONE

#define EDDI_SII_NEW_CYCLE_REDUCED_SCALING_1MS      4UL                 //4ms scaling

//Defines for DO-RQBs
#define EDDI_SII_RQB_FREE                           (LSA_VOID *)0   //for RQB-parameter internal_context
#define EDDI_SII_RQB_BUSY                           (LSA_VOID *)1   //for RQB-parameter internal_context

//Defines for EOI inactive time
#if defined (EDDI_CFG_REV5)
#define EDDI_SII_MAX_EOI_INACTIVE_TIME_REV5         0xF         //for IRTE EOI register
#endif
#if defined (EDDI_CFG_REV6)
#define EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_SP  0xF         //for IRTE EOI register
#define EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_HP  0xFFFF      //for IRTE EOI register
//temporarily_disabled_lint -esym(755, EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_SP)
//temporarily_disabled_lint -esym(755, EDDI_SII_MAX_EOI_INACTIVE_TIME_REV6_IRQ_HP)
#endif
#if defined (EDDI_CFG_REV7)
#define EDDI_SII_MAX_EOI_INACTIVE_TIME_REV7         0xFFFF      //for IRTE EOI register
#endif
#if defined (EDDI_CFG_REV5)
#define EDDI_SII_MAX_EOI_INACTIVE_TIME              EDDI_SII_MAX_EOI_INACTIVE_TIME_REV5     //for IRTE EOI register
#elif defined (EDDI_CFG_REV6)
//EDDI_SII_MAX_EOI_INACTIVE_TIME depending on IRQ_SP or IRQ_HP
#elif defined (EDDI_CFG_REV7)
#define EDDI_SII_MAX_EOI_INACTIVE_TIME              EDDI_SII_MAX_EOI_INACTIVE_TIME_REV7     //for IRTE EOI register
#endif

//check range of EOI inactive time
#if defined (EDDI_SII_MAX_EOI_INACTIVE_TIME)
#if defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
#if (EDDI_CFG_SII_USE_SPECIAL_EOI > EDDI_SII_MAX_EOI_INACTIVE_TIME)
#error "EDDI_CFG_SII_USE_SPECIAL_EOI defined wrong!"
#endif
#endif
#endif

//Defines for interrupt events = EDDIIntSource (SII specific)   ATTENTION: do not change!
//IRTE IRT HW-Interrupts:
//0x00: reserved!
#define EDDI_INT_StartOp                        0x01UL          /* Bit 1 Trigger for synchronous applications */
#define EDDI_INT_IRT_TransfEnd                  0x02UL          /* Bit 2 Cycle synchronisation */
#define EDDI_INT_DMAIn                          0x03UL          /* Bit 3  */
#define EDDI_INT_DMAOut                         0x04UL          /* Bit 4  */
#define EDDI_INT_InFault                        0x05UL          /* Bit 5  */
#define EDDI_INT_OutFault                       0x06UL          /* Bit 6  */
#define EDDI_INT1_rcv_IRT_SRT                   0x07UL          /* Bit 7  */
#define EDDI_INT2_rcv_IRT_SRT                   0x08UL          /* Bit 8  */
#define EDDI_INT1_snd_IRT_SRT                   0x09UL          /* Bit 9  */
#define EDDI_INT2_snd_IRT_SRT                   0x0aUL          /* Bit 10 */
#define EDDI_INT_fatal_List_Err                 0x0bUL          /* Bit 11 */
#define EDDI_INT_AplClk                         0x0cUL          /* Bit 12 */
#define EDDI_INT_Clkcy_Int                      0x0dUL          /* Bit 13 */
#define EDDI_INT_Clkcy                          0x0eUL          /* Bit 14 */
#define EDDI_INT_FifoData_Lost                  0x0fUL          /* Bit 15 */
#define EDDI_INT_TCW_Done                       0x10UL          /* Bit 16 IRTTrace ready */
#define EDDI_INT_Comp_1                         0x11UL          /* Bit 17 */
#define EDDI_INT_Comp_2                         0x12UL          /* Bit 18 */
#define EDDI_INT_Comp_3                         0x13UL          /* Bit 19 */
#define EDDI_INT_Comp_4                         0x14UL          /* Bit 20 */
#define EDDI_INT_Comp_5                         0x15UL          /* Bit 21 */
#define EDDI_INT_ACW_List_Term                  0x16UL          /* Bit 22 */
#define EDDI_INT_iSRT_DurationFault             0x17UL          /* Bit 23 */
#define EDDI_INT_Out_of_Sync_Rcv                0x18UL          /* Bit 24 */
#define EDDI_INT_Out_of_Sync_Snd                0x19UL          /* Bit 25 */
#define EDDI_INT_PS_Scoreboard_Skipped          0x1aUL          /* Bit 26 */
#define EDDI_INT_PLL_ext_IN                     0x1bUL          /* Bit 27 */
#define EDDI_INT_SRT_Cycle_Skipped              0x1cUL          /* Bit 28 */
#define EDDI_INT_ReadyForDMAIn                  0x1dUL          /* Bit 29 Event to applikation */
#define EDDI_INT_DMAOutDone                     0x1eUL          /* Bit 30 Event to applikation */
#define EDDI_INT_PS_Scoreboard_Changed          0x1fUL          /* Bit 31 Scoreboard INT */

//IRTE NRT HW-Interrupts:
#define EDDI_INT_Snd_CHA0                       0x20UL          /* Bit 0  Send */
#define EDDI_INT_Rcv_CHA0                       0x21UL          /* Bit 1  Receive */
#define EDDI_INT_Snd_CHA1                       0x22UL          /* Bit 2  Send */
#define EDDI_INT_Rcv_CHA1                       0x23UL          /* Bit 3  Receive */
#define EDDI_INT_Snd_CHB0                       0x24UL          /* Bit 4  Send */
#define EDDI_INT_Rcv_CHB0                       0x25UL          /* Bit 5  Receive */
#define EDDI_INT_Snd_CHB1                       0x26UL          /* Bit 6  Send */
#define EDDI_INT_Rcv_CHB1                       0x27UL          /* Bit 7  Receive */
#define EDDI_INT_Fatal_NRTList_Err              0x28UL          /* Bit 8  */
#define EDDI_INT_Link_Status_Change             0x29UL          /* Bit 9  Link State Change */
#define EDDI_INT_Trigger_Unit                   0x2aUL          /* Bit 10 */
#define EDDI_INT_ClockCnt_Wrap                  0x2bUL          /* Bit 11 Only Rev7 */
#define EDDI_INT_HP                             0x2cUL          /* Bit 12 */
#define EDDI_INT_SP                             0x2dUL          /* Bit 13 */
#define EDDI_INT_Host_Access_Err                0x2eUL          /* Bit 14 */
#define EDDI_INT_Time_Stamp                     0x2fUL          /* Bit 15 */
#define EDDI_INT_Instruction_Done               0x30UL          /* Bit 16 */
#define EDDI_INT_Overflow_HOL                   0x31UL          /* Bit 17 */
#define EDDI_INT_Underflow_NRT                  0x32UL          /* Bit 18 Low Watermark */
#define EDDI_INT_SRT_FCW_empty                  0x33UL          /* Bit 19 */
#define EDDI_INT_NRT_FCW_empty                  0x34UL          /* Bit 20 */
#define EDDI_INT_NRT_DB_empty                   0x35UL          /* Bit 21 */
#define EDDI_INT_no_Table_Entry                 0x36UL          /* Bit 22 */
#define EDDI_INT_a                              0x37UL          /* Bit 23 */
#define EDDI_INT_b                              0x38UL          /* Bit 24 */
#define EDDI_INT_c                              0x39UL          /* Bit 25 */
#define EDDI_INT_d                              0x3aUL          /* Bit 26 */
#define EDDI_INT_e                              0x3bUL          /* Bit 27 */
#define EDDI_INT_f                              0x3cUL          /* Bit 28 */
#define EDDI_INT_g                              0x3dUL          /* Bit 29 */
#define EDDI_INT_h                              0x3eUL          /* Bit 30 */
#define EDDI_INT_i                              0x3fUL          /* Bit 31 */

//EDDI Software Interrupts:
#define EDDI_SW_INT_NewCycleReduced             0x40UL          /* NewCycle reduced */
#define EDDI_SW_INT_Reload_NRTSendLists         0x41UL          /* Reload all NRT send lists */

//#define EDDI_SW_INT_RToverUDP                 0x44UL          /* Software solution for RToverUDP -> not implemented! */

//pay attention to EDDI_MAX_ISR = 0x45!

/*===========================================================================*/
/*                          internal types                                   */
/*===========================================================================*/

//struct _EDDI_DDB_TYPE;

#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
typedef  LSA_VOID  ( EDD_UPPER_IN_FCT_ATTR  *  EDDI_INTERRUPT_FCT )( struct _EDDI_DDB_TYPE  *  const  pDDB,
                                                                     LSA_BOOL                  const  bExtTimerInt );
#else
typedef  LSA_VOID  ( EDD_UPPER_IN_FCT_ATTR  *  EDDI_INTERRUPT_FCT )( struct _EDDI_DDB_TYPE  *  const  pDDB );
#endif
#if defined (EDDI_CFG_SII_POLLING_MODE)
typedef  LSA_VOID  ( EDD_UPPER_IN_FCT_ATTR  *  EDDI_LOW_INTERRUPT_FCT )( struct _EDDI_DDB_TYPE  *  const  pDDB);
#endif
#if defined (EDDI_CFG_SYSRED_2PROC)
typedef  LSA_VOID  ( EDD_UPPER_IN_FCT_ATTR  *  EDDI_SYSRED_POLLING_FCT )( struct _EDDI_DDB_TYPE  *  const  pDDB);
#endif

typedef struct _SII_INT_GROUP_TYPE
{
    EDD_RQB_TYPE            IntRQB;      //static RQB for context-switch via DO-macro

} SII_INT_GROUP_TYPE;

typedef struct _EDDI_DDB_COMP_SII_TYPE
{
    EDDI_INTERRUPT_FCT      interrupt_fct_ptr;      //device granular function-pointer to SII-interrupt-function
    #if defined (EDDI_CFG_SII_POLLING_MODE)
    EDDI_LOW_INTERRUPT_FCT  interrupt_low_fct_ptr;  //device granular function-pointer to SII-interrupt-function for low-context-polling
    #endif
    #if defined (EDDI_CFG_SYSRED_2PROC)
    EDDI_SYSRED_POLLING_FCT polling_sysred_fct_ptr; //device granular function-pointer to SII-sysred-polling-function
    #endif

    //data for interrupt groups
    SII_INT_GROUP_TYPE      INTGroup[EDDI_SII_INT_GROUP_CNT]; //index 0 = lowest prio = PRIO4-NRT-LOW

    //IRTE register addresses
    volatile  LSA_UINT32    IMode_Adr;      //interrupt mode register
    volatile  LSA_UINT32    IR_IRT_Adr;     //interrupt register for IRT
    volatile  LSA_UINT32    IR_NRT_Adr;     //interrupt register for NRT
    volatile  LSA_UINT32    IRR_IRT_Adr;    //interrupt request register for IRT
    volatile  LSA_UINT32    IRR_NRT_Adr;    //interrupt request register for NRT
    volatile  LSA_UINT32    IAR_IRT_Adr;    //interrupt acknowledge register for IRT
    volatile  LSA_UINT32    IAR_NRT_Adr;    //interrupt acknowledge register for NRT
    volatile  LSA_UINT32    IMR_IRT_Adr;    //interrupt mask register for IRT
    volatile  LSA_UINT32    IMR_NRT_Adr;    //interrupt mask register for NRT
    volatile  LSA_UINT32    EOI_Adr;        //end of interrupt register

    LSA_BOOL                bStopInterrupts;        //boolean for stopping interrupts for this device (reset when nulling DDB)

    #if defined (EDDI_SII_DEBUG_MODE_CHECKS)
    LSA_UINT32              IMR_IRT_Opposite_Adr;   //interrupt mask register for IRT of the opposite interrupt-line (IRQ0 - IRQ1)
    LSA_UINT32              IMR_NRT_Opposite_Adr;   //interrupt mask register for NRT of the opposite interrupt-line (IRQ0 - IRQ1)
    #endif

    //dynamic global interrupt masks
    volatile  LSA_UINT32    IRT_Mask_IRTE;          //global interrupt mask for IRT in IRTE format (= little endian). Contains EDDI- and User-interrupts!
    volatile  LSA_UINT32    NRT_Mask_IRTE;          //global interrupt mask for NRT in IRTE format (= little endian). Contains EDDI- and User-interrupts!

    LSA_UINT32              IMode_Int_Ack_Mask;     //mask for interrupt-acknowledge-mode-bit in IRTE-interrupt-mode-register in IRTE format (= little endian)

    //SII configuration parameters
    LSA_UINT32              SII_IrqSelector;        //EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
    LSA_UINT32              SII_IrqNumber;          //EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    LSA_UINT32              SII_ExtTimerInterval;   //0, 250, 500, 1000 in us
    #endif
    #if defined (EDDI_CFG_SII_FLEX_MODE)
    LSA_UINT32              SII_Mode;               //EDDI_SII_STANDARD_MODE or EDDI_SII_POLLING_MODE
    #endif

    #if !defined (EDDI_SII_MAX_EOI_INACTIVE_TIME) && !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
    LSA_UINT32              SII_MaxEoiInactiveTime; //used for dynamic MaxEoiInactiveTime
    #endif

    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    LSA_UINT32              NRT_Group_Mask_Spec_NRT_Rx_Done_IRTE;       //dynamic interrupt-mask-status of NRT-RX-Done-interrupts (changeable with eddi_interrupt_set_nrt_rx_int())
    LSA_UINT32              HWClkCycleIntDividerForNRTCheck;            //0 => HW-trigger not used => SW-reduction is used instead
    #endif

    //NRT-RX-timeslice-emulation
    #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) || defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
    volatile  LSA_UINT32    NewCycleReductionCtrForNRTCheckRxLimitA0;   //reduction counter for NRT-RX-Limitation/Timeslicing of NRT-Channel A0
    volatile  LSA_UINT32    NewCycleReductionCtrForNRTCheckRxLimitB0;   //reduction counter for NRT-RX-Limitation/Timeslicing of NRT-Channel B0
    #if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
    volatile  LSA_UINT32    NRT_RX_Limit_ORG_MaskIRTE;                  //variable for dynamic interrupt-mask for ORG-Task
    #endif
    #if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
    volatile  LSA_UINT32    NRT_RX_Limit_NRT_LOW_MaskIRTE;              //variable for dynamic interrupt-mask for NRT-LOW-Task
    #endif
    #endif

    #if defined (EDDI_SII_DEBUG_MODE_TRACES) || defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES) || defined (EDDI_SII_DEBUG_MODE_EMPTY_INT_TRACES)
    //statistic-counter
    LSA_UINT32              RegularInterruptCnt;                //counts regular EDDI-interrupts (without empty EDDI-interrupts)
    LSA_UINT32              EmptyInterruptCnt;                  //counts empty EDDI-interrupts, but USER-interrupts can exist in USER-Int-Share-mode!
    #endif

    #if defined (EDDI_CFG_REV5)
    #if defined (EDDI_SII_DEBUG_MODE_SPECIAL_TS_TRACES)
    LSA_UINT32              TwoTSIntWithSameTimeCnt;            //counts sequent (2) timestamp-interrupts with identical send-timestamps
    #endif
    #endif

    #if defined (EDDI_CFG_SII_POLLING_MODE)
    LSA_INT32               RestTimeNewCycleReduced10ns;        //rest time for NewCycleReduced (is counted down)
    LSA_UINT32              LastTimeStamp;                      //stores timestamp of last NewCycle
    #elif defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    LSA_UINT32              NewCycleReductionFactorForNRTCheck; //reduction factor for NRT-check in New-Cycle-Sync-Mode
    LSA_UINT32              NewCycleReductionCtrForNRTCheck;    //reduction counter for NRT-check in New-Cycle-Sync-Mode
    #endif

} EDDI_DDB_COMP_SII_TYPE;

typedef struct _EDDI_DDB_COMP_SII_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_SII_PTR_TYPE;

/*===========================================================================*/
/*                          internal data                                    */
/*===========================================================================*/

#if defined (EDDI_SII_DEBUG_MODE_TRACES)
extern  LSA_UINT32          EDDI_SII_Critical_Ctr;
#endif

/*===========================================================================*/
/*                          internal macros                                  */
/*===========================================================================*/

//the following macro contains no endianess-swapping
#define EDDI_SII_IO_x32(io_adr)      (*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)(io_adr))
//the following macros contain endianess-swapping
#define EDDI_SII_IO_W32(io_adr, val) (*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)(io_adr) = EDDI_HOST2IRTE32(val))
//#define EDDI_SII_IO_R32(io_adr)    EDDI_IRTE2HOST32((*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)(io_adr)))

#if defined (EDDI_SII_DEBUG_MODE_TRACES)
//CallerIds for ENTER-CRITICAL-macros
#define EDDI_SII_CALLER_ID_START_INTS               ((LSA_UINT32) 0x1)
#define EDDI_SII_CALLER_ID_STOP_INTS                ((LSA_UINT32) 0x2)
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_CALLER_ID_GET_NEWCYCLE_REDUCTION   ((LSA_UINT32) 0x3)
#endif
#if !defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
#define EDDI_SII_CALLER_ID_ISR                      ((LSA_UINT32) 0x4)
#endif
#define EDDI_SII_CALLER_ID_TASK_M2                  ((LSA_UINT32) 0x5)
#if defined (EDDI_CFG_SII_STD_MODE)
#define EDDI_SII_CALLER_ID_TASK_M2_NRT              ((LSA_UINT32) 0x6)
#endif
#if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION) || defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
#define EDDI_SII_CALLER_ID_TASK_M2_NRT_RX_LIMIT     ((LSA_UINT32) 0x7)
#define EDDI_SII_CALLER_ID_TASK_RX_DONE_LOOP        ((LSA_UINT32) 0x8)
#endif
#define EDDI_SII_CALLER_ID_DEVICE_CLOSE_P2          ((LSA_UINT32) 0x9)
#define EDDI_SII_CALLER_ID_CLOSE_DDB                ((LSA_UINT32) 0xA)
#define EDDI_SII_CALLER_ID_MASK_USER_INTS           ((LSA_UINT32) 0xB)
#define EDDI_SII_CALLER_ID_UNMASK_USER_INTS         ((LSA_UINT32) 0xC)
#define EDDI_SII_CALLER_ID_GET_USER_INTS            ((LSA_UINT32) 0xD)
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#define EDDI_SII_CALLER_ID_SET_NRT_RX_INT           ((LSA_UINT32) 0xE)
#endif
#endif

#if defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_ENTER_SII_CRITICAL_S(CallerId_)\
{\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_ENTER_SII_CRITICAL_S, before, CallerId:0x%X", (CallerId_));\
    EDDI_ENTER_SII_CRITICAL();\
    if (EDDI_SII_Critical_Ctr)\
    {\
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_SII_CRITICAL_S, critical counter error, EDDI_SII_Critical_Ctr:0x%X", EDDI_SII_Critical_Ctr);\
        EDDI_Excp("EDDI_ENTER_SII_CRITICAL_S, critical counter error", EDDI_FATAL_ERR_EXCP, EDDI_SII_Critical_Ctr, 0);\
    }\
    else\
    {\
        EDDI_SII_Critical_Ctr++;\
    }\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_ENTER_SII_CRITICAL_S, afterwards, CallerId:0x%X", (CallerId_));\
}
#define EDDI_EXIT_SII_CRITICAL_S(CallerId_)\
{\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EXIT_SII_CRITICAL_S, before, CallerId:0x%X", (CallerId_));\
    if (EDDI_SII_Critical_Ctr != 1UL)\
    {\
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_SII_CRITICAL_S, critical counter error, EDDI_SII_Critical_Ctr:0x%X", EDDI_SII_Critical_Ctr);\
        EDDI_Excp("EDDI_EXIT_SII_CRITICAL_S, critical counter error", EDDI_FATAL_ERR_EXCP, EDDI_SII_Critical_Ctr, 0);\
    }\
    else\
    {\
        EDDI_SII_Critical_Ctr--;\
    }\
    EDDI_EXIT_SII_CRITICAL();\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EXIT_SII_CRITICAL_S, afterwards, CallerId:0x%X", (CallerId_));\
}
#else
#define EDDI_ENTER_SII_CRITICAL_S(CallerId_)    EDDI_ENTER_SII_CRITICAL()
#define EDDI_EXIT_SII_CRITICAL_S(CallerId_)     EDDI_EXIT_SII_CRITICAL()
#endif

#if defined (EDDI_CFG_SII_REMOVE_ISR_LOCKS)
#define EDDI_ENTER_SII_ISR_CRITICAL_S(CallerId_)
#define EDDI_EXIT_SII_ISR_CRITICAL_S(CallerId_)
#elif defined (EDDI_SII_DEBUG_MODE_TRACES)
#define EDDI_ENTER_SII_ISR_CRITICAL_S(CallerId_)\
{\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_ENTER_SII_ISR_CRITICAL_S, before, CallerId:0x%X", (CallerId_));\
    EDDI_ENTER_SII_CRITICAL();\
    if (EDDI_SII_Critical_Ctr)\
    {\
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_ENTER_SII_ISR_CRITICAL_S, critical counter error, EDDI_SII_Critical_Ctr:0x%X", EDDI_SII_Critical_Ctr);\
        EDDI_Excp("EDDI_ENTER_SII_ISR_CRITICAL_S, critical counter error", EDDI_FATAL_ERR_EXCP, EDDI_SII_Critical_Ctr, 0);\
    }\
    else\
    {\
        EDDI_SII_Critical_Ctr++;\
    }\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_ENTER_SII_ISR_CRITICAL_S, afterwards, CallerId:0x%X", (CallerId_));\
}
#define EDDI_EXIT_SII_ISR_CRITICAL_S(CallerId_)\
{\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EXIT_SII_ISR_CRITICAL_S, before, CallerId:0x%X", (CallerId_));\
    if (EDDI_SII_Critical_Ctr != 1UL)\
    {\
        EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_EXIT_SII_ISR_CRITICAL_S, critical counter error, EDDI_SII_Critical_Ctr:0x%X", EDDI_SII_Critical_Ctr);\
        EDDI_Excp("EDDI_EXIT_SII_ISR_CRITICAL_S, critical counter error", EDDI_FATAL_ERR_EXCP, EDDI_SII_Critical_Ctr, 0);\
    }\
    else\
    {\
        EDDI_SII_Critical_Ctr--;\
    }\
    EDDI_EXIT_SII_CRITICAL();\
    EDDI_LOWER_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_EXIT_SII_ISR_CRITICAL_S, afterwards, CallerId:0x%X", (CallerId_));\
}
#else
#define EDDI_ENTER_SII_ISR_CRITICAL_S(CallerId_)    EDDI_ENTER_SII_CRITICAL()
#define EDDI_EXIT_SII_ISR_CRITICAL_S(CallerId_)     EDDI_EXIT_SII_CRITICAL()
#endif

/*===========================================================================*/
/*                          internal prototypes                              */
/*===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIInit( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIStartInterrupts( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIStopInterrupts( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIISetDummyIsr( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactors( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      LSA_UINT32               const  CycleBaseFactor,
                                                                      LSA_UINT32               const  CycleLength_10ns,
                                                                      LSA_UINT32               const  CycleLength_us );
#elif defined (EDDI_CFG_SII_POLLING_MODE)
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactor( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  CycleLength_10ns );
#else
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetNewCycleReductionFactor( LSA_UINT32  const  CycleLength_10ns );
#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
LSA_UINT32  EDD_UPPER_IN_FCT_ATTR  EDDI_SIIGetExtTimerReductionFactor( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );
#endif
#endif

#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio1Aux( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB );
#endif

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio2Org( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio3Rest( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SIIEvPrio4NRTLow( EDDI_SII_VOLATILE  EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SII_H


/*****************************************************************************/
/*  end of file eddi_sii.h                                                   */
/*****************************************************************************/

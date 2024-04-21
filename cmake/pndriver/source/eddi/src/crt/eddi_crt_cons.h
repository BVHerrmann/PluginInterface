#ifndef EDDI_CRT_CONS_H         //reinclude-protection
#define EDDI_CRT_CONS_H

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
/*  F i l e               &F: eddi_crt_cons.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* CRT-                                                                      */
/* Defines constants, types, macros and prototyping for prefix.              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

// States for EDD-Consumer
#define EDDI_CRT_CONS_STS_NOTUSED               0
#define EDDI_CRT_CONS_STS_RESERVED              1
#define EDDI_CRT_CONS_STS_INACTIVE              2
#define EDDI_CRT_CONS_STS_ACTIVE                3

// InitValue for Redundanz-Byte in APDU-Status for Consumers
#define EDDI_SER_RED_STATUS_INIT_VALUE          0x0F

// Waiting time before the PRIMARY-Event can be indicated
#define EDDI_CRT_CONS_PRIM_WAIT_TIME_NS         3000       //3us

// Bugfix for IRTE issue:
// In case of receiving valid frames, IRTE checks the Partial_DataLen first and generates an IRTFatalListError
// A Partial_DataLen between 36 and 39 is forbidden now
#if !defined (EDDI_CFG_REV5) /* REV5 does not support partdata access */
#define EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_LO  36
#define EDDI_CRT_CONS_PARTIAL_DATALEN_FORBIDDEN_HI  39
#endif

/*===========================================================================*/

// Contains all Param-Data for adding a Conumer
typedef struct _EDDI_CRT_CONSUMER_PARAM_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE           pKRAMDataBuffer;   // Pointer of IN-Data in KRAM

    LSA_UINT32                          ui32SBTimerAdr;    // "pointer" to the related TimerScoreboard-Entry (ASIC-sight)
    EDDI_SER10_TIMER_SCORB_TYPE         *pSBTimerEntry;

    LSA_UINT16                          FrameId;
    EDD_MAC_ADR_TYPE                    SrcMAC;
    LSA_UINT16                          CycleReductionRatio;
    LSA_UINT16                          CyclePhase;
    LSA_UINT16                          DataHoldFactor;    // DataHoldFactor same as given over RQB-Params
    LSA_BOOL                            bKRAMDataBufferValid;
    
    #if defined (EDDI_CFG_REV7) || defined (EDDI_CFG_REV6)
    LSA_UINT16                          DataHoldStatus;    /*!< DataHoldStatus same as given over RQB-Params*/
    LSA_UINT32                          DHReloadVal;       /*!< WDReloadVal as used in ACW/FCW */
    #endif
    LSA_UINT32                          WDReloadVal;       // WDReloadVal as used in ACW/FCW
    LSA_UINT32                          TSBEntry_RR;       // Reductionratio for use in Timer-SB          
    // (Calculation is HW-Revision-dependend !)

    #if defined (EDDI_CFG_REV7)
    LSA_UINT32                          SCRR_ReloadValue;
    #endif

    //LSA_UINT32                          MessageId;        // Needed to Connect to the related ICC Info (for RT-Class3 only)
    LSA_UINT8                           ListType;           // ACW or FCW ?
    LSA_UINT8                           ConsumerType;
    LSA_UINT8                           ImageMode;          // Defines OK ??

    LSA_UINT8                           SFPosition;         //PosT (DG only)
    LSA_UINT16                          DataLen;            // DataLen (without APDU !!) as given in RQB-Params
    LSA_UINT32                          DataOffset;
    LSA_UINT32                          SFOffset;           //Offset of DG.C_SDU relative to the beginning of the packframe
    LSA_UINT16                          Partial_DataOffset; //PartialData from Rev6 on
    LSA_UINT16                          Partial_DataLen;
    LSA_UINT32                          BufferProperties;

    EDDI_RT_TYPE                        RT_Type;
    EDDI_IP_TYPE                        SrcIP;

} EDDI_CRT_CONSUMER_PARAM_TYPE;

typedef struct _EDDI_CRT_CONSUMER_PARAM_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_CRT_CONSUMER_PARAM_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* CRT-CONSUMER-Trace                                                        */
/*---------------------------------------------------------------------------*/
#if defined (EDDI_CFG_CONSTRACE_DEPTH)
typedef enum _EDDI_CRT_CONSTRACE_STATES_TYPE
{
    //EDDI_CONSTRACE_STATE_ADD,
    //EDDI_CONSTRACE_STATE_ACT,
    EDDI_CONSTRACE_STATE_DEACT,
    EDDI_CONSTRACE_STATE_SETTOUK,
    //EDDI_CONSTRACE_STATE_REMOVE,
    EDDI_CONSTRACE_STATE_EVAL,
    EDDI_CONSTRACE_STATE_UKTO_Again,
    EDDI_CONSTRACE_STATE_UKTO_Miss,
    EDDI_CONSTRACE_STATE_DSChanged,
    #if defined (EDDI_CFG_USE_SW_RPS)
    EDDI_CONSTRACE_STATE_UKCOUNTDOWN1,
    EDDI_CONSTRACE_STATE_UKCOUNTDOWN2,
    EDDI_CONSTRACE_STATE_UKCOUNTDOWNEARLY,
    EDDI_CONSTRACE_STATE_UKTIMEDOUT,
    EDDI_CONSTRACE_STATE_SWTRIG,
    EDDI_CONSTRACE_STATE_NOAgainIfNotUk,
    EDDI_CONSTRACE_STATE_NONoAgainPS0to1,
    EDDI_CONSTRACE_STATE_INVPS1to0,
    EDDI_CONSTRACE_STATE_INVDV0to1,
    EDDI_CONSTRACE_STATE_INVNoEvent,
    EDDI_CONSTRACE_STATE_VALPS1to0,
    EDDI_CONSTRACE_STATE_VALDS1to0,
    EDDI_CONSTRACE_STATE_VALPS0to1,
    EDDI_CONSTRACE_STATE_VALNoEvent,
    EDDI_CONSTRACE_STATE_UKTO_0xF,
    EDDI_CONSTRACE_STATE_AGAIN0xF,
    EDDI_CONSTRACE_STATE_PendingEvent
    #else
    EDDI_CONSTRACE_STATE_OKMiss,
    EDDI_CONSTRACE_STATE_MSAgain
    #endif
} EDDI_CRT_CONSTRACE_STATES_TYPE;

typedef struct _EDDI_CRT_CONSTRACE_TRACEENTRY_TYPE
{
    LSA_UINT32                    clock_cnt_val;
    LSA_UINT32                    apdustatus;
    LSA_UINT32                    detail1;
    LSA_UINT32                    detail2;
    EDDI_CRT_CONSTRACE_STATES_TYPE state;
} EDDI_CRT_CONSTRACE_TRACEENTRY_TYPE;

typedef struct _EDDI_CRT_CONSTRACE_TRACE_TYPE
{
    LSA_UINT16  next_index;
    EDDI_CRT_CONSTRACE_TRACEENTRY_TYPE trace[EDDI_CFG_CONSTRACE_DEPTH];
} EDDI_CRT_CONSTRACE_TRACE_TYPE;

typedef struct _EDDI_CRT_CONSTRACE_TABLE_TYPE
{
    LSA_UINT16    frame_id[1024];       //index is the consumer-id
    #if defined (EDDI_CFG_CONSTRACE_ADVANCED)
    EDDI_CRT_CONSTRACE_TRACE_TYPE trace_acw[1024];
    EDDI_CRT_CONSTRACE_TRACE_TYPE trace_fcw[1024];
    #endif
} EDDI_CRT_CONSUMER_TRACE_TABLE;

#define EDDI_CRT_CONSTRACE_INIT(pCons_) {(pCons_)->trace.next_index=0;}

#if defined (EDDI_CFG_CONSTRACE_ADVANCED)
#define EDDI_CRT_CONSTRACE_TRACE_ADV(pCons_, apdustatus_, detail1_, detail2_, state_) {                          \
EDDI_CRT_CONSTRACE_TRACEENTRY_TYPE  * pTraceEntry; \
LSA_UINT16 * pNextEntry; \
LSA_UINT16 const LocalFrameID = (pCons_)->LowerParams.FrameId; \
  \
    if (LocalFrameID < 1024) \
    { \
        if ((pCons_)->LowerParams.ConsumerType != EDDI_RTC3_CONSUMER)  \
        { \
            pNextEntry = &(pDDB->CRT.pConsTable->trace_acw[LocalFrameID].next_index); \
            pTraceEntry = &(pDDB->CRT.pConsTable->trace_acw[LocalFrameID].trace[*pNextEntry]); \
        } \
        else  \
        { \
            pNextEntry = &(pDDB->CRT.pConsTable->trace_fcw[LocalFrameID].next_index); \
            pTraceEntry = &(pDDB->CRT.pConsTable->trace_fcw[LocalFrameID].trace[*pNextEntry]); \
        } \
        pTraceEntry->clock_cnt_val  = IO_R32(CLK_COUNT_VALUE);                                                  \
        pTraceEntry->apdustatus     = (LSA_UINT32)(apdustatus_);                                                \
        pTraceEntry->detail1        = (LSA_UINT32)(detail1_);                                                   \
        pTraceEntry->detail2        = (LSA_UINT32)(detail2_);                                                   \
        pTraceEntry->state          = (state_);                                                                 \
        \
        *pNextEntry = *pNextEntry + 1;                                                                           \
        if (*pNextEntry >= EDDI_CFG_CONSTRACE_DEPTH) {*pNextEntry = 0;}            \
    } \
}
#else
#define EDDI_CRT_CONSTRACE_TRACE_ADV(pCons_, apdustatus_, detail1_, detail2_, state_)
#endif //defined (EDDI_CFG_CONSTRACE_ADVANCED)

#define EDDI_CRT_CONSTRACE_TRACE(pCons_, apdustatus_, detail1_, detail2_, state_) {                          \
EDDI_CRT_CONSTRACE_TRACEENTRY_TYPE  * const pTraceentry = &((pCons_)->trace.trace[(pCons_)->trace.next_index]); \
pTraceentry->clock_cnt_val  = IO_R32(CLK_COUNT_VALUE);                                                  \
pTraceentry->apdustatus     = (LSA_UINT32)(apdustatus_);                                                \
pTraceentry->detail1        = (LSA_UINT32)(detail1_);                                                   \
pTraceentry->detail2        = (LSA_UINT32)(detail2_);                                                   \
pTraceentry->state          = (state_);                                                                 \
\
(pCons_)->trace.next_index++;                                                                           \
if ((pCons_)->trace.next_index >= EDDI_CFG_CONSTRACE_DEPTH) {(pCons_)->trace.next_index = 0;}            \
      \
    EDDI_CRT_CONSTRACE_TRACE_ADV(pCons_, apdustatus_, detail1_, detail2_, state_);  \
}


#if defined (EDDI_CFG_CONSTRACE_ADVANCED)
#define EDDI_CRT_CONSTRACE_CLEAR_TABLE_ADV(pDDB_) {                                                \
LSA_UINT16 frame_id;                                                                                     \
for (frame_id=0;frame_id<1024;frame_id++)                              \
{                                                                                                       \
  (pDDB_)->CRT.pConsTable->trace_acw[frame_id].next_index = 0;                                                  \
  (pDDB_)->CRT.pConsTable->trace_fcw[frame_id].next_index = 0;                                                  \
}                                                                                                       \
}
#else
#define EDDI_CRT_CONSTRACE_CLEAR_TABLE_ADV(pDDB_)
#endif

#define EDDI_CRT_CONSTRACE_ALLOC_TABLE(pDDB_) {                                                              \
LSA_UINT16 cons_id;                                                                                     \
\
EDDI_AllocLocalMem((void**)&((pDDB_)->CRT.pConsTable), sizeof(EDDI_CRT_CONSUMER_TRACE_TABLE));          \
\
if (EDDI_NULL_PTR == (pDDB_)->CRT.pConsTable)                                                           \
{                                                                                                       \
/* traces in macros are not supported (tracescanner) */                                                 \
EDDI_Excp("EDDI_CRTCreateConsumerListSB, ERROR while allocating Consumer-Array", EDDI_FATAL_ERR_EXCP, 0, 0); \
}                                                                                                       \
\
for (cons_id=0;cons_id<1024;cons_id++)                                                                  \
{                                                                                                       \
(pDDB_)->CRT.pConsTable->frame_id[cons_id] = 0xFFFF;                                                    \
}                                                                                                       \
  \
EDDI_CRT_CONSTRACE_CLEAR_TABLE_ADV(pDDB_);  \
}

#define EDDI_CRT_CONSTRACE_DEALLOC_TABLE(pDDB_) {                                                       \
LSA_UINT16  u16Status;                                                                                  \
\
if (!((pDDB_)->CRT.pConsTable == EDDI_NULL_PTR))                                                        \
{                                                                                                       \
EDDI_FREE_LOCAL_MEM(&u16Status, (pDDB_)->CRT.pConsTable);                                               \
\
if (u16Status != EDD_STS_OK)                                                                            \
{                                                                                                       \
/* traces in macros are not supported (tracescanner) */                                                 \
EDDI_Excp("EDDI_CRT_CONSTRACE_DEALLOC_TABLE", EDDI_FATAL_ERR_EXCP, u16Status, 0);                       \
}                                                                                                       \
}                                                                                                       \
}

#define EDDI_CRT_CONSTRACE_SET_TABLE_ENTRY(pDDB_, cons_id_, frame_id_) {                                \
if ( (cons_id_) < (sizeof(EDDI_CRT_CONSUMER_TRACE_TABLE)/sizeof(LSA_UINT16)) )                          \
{                                                                                                       \
(pDDB_)->CRT.pConsTable->frame_id[cons_id_] = (frame_id_);                                              \
}                                                                                                       \
}

#else

#define EDDI_CRT_CONSTRACE_INIT(pCons_)

#define EDDI_CRT_CONSTRACE_TRACE(pCons_, apdustatus_, detail1_, detail2_, state_)

#define EDDI_CRT_CONSTRACE_ALLOC_TABLE(pDDB_)

#define EDDI_CRT_CONSTRACE_DEALLOC_TABLE(pDDB_)

#define EDDI_CRT_CONSTRACE_SET_TABLE_ENTRY(pDDB_, cons_id_, frame_id_)

#endif //defined (EDDI_CFG_CONSTRACE_DEPTH)


/*---------------------------------------------------------------------------*/
/* CRT-CONSUMER-Structure                                                    */
/*---------------------------------------------------------------------------*/
typedef enum _EDDI_CRT_CONS_UK_STATUS
{
    EDDI_CRT_CONS_UK_OFF,         // UnknownMode is OFF
    #if defined (EDDI_CFG_USE_SW_RPS)
    EDDI_CRT_CONS_UK_TIMED_OUT,   // UnknownMode is Reaching the Timeout --> an Indication will generated
    #endif
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_CRT_CONS_UK_PENDING,      // DG only: DG is waiting for PF-AGAIN
    #endif
    EDDI_CRT_CONS_UK_COUNTING    // UnknownMode is ON and Counting Down
} EDDI_CRT_CONS_UK_STATUS;

// Some of the following consumer-related member-variables are stored
// in this separate and small struct to optimize caching
// for the SW-Solution:

typedef union _EDDI_SB_LINK
{
    struct _EDDI_CRT_CONS_SB_TYPE    *pNext;  // Chain elements for phase tree:  DO NOT MOVE !!!
    E_TREE_LIST                       TreeList;

} EDDI_SB_LINK;

#if defined (EDDI_CFG_DFP_ON)
typedef struct _EDDI_DFP_SB_LINK_TYPE
{
    struct _EDDI_DFP_SB_ENTRY_TYPE  * pNext;            //ptr to next sb-element 
    struct _EDDI_DFP_SB_ENTRY_TYPE  * pPrev;            //ptr to previous sb-element
     
} EDDI_DFP_SB_LINK_TYPE;

typedef struct _EDDI_DFP_SB_ENTRY_TYPE
{
    EDDI_DFP_SB_LINK_TYPE           Link; 
    struct _EDDI_CRT_CONSUMER_TYPE  * pConsumer;      //ptr to consumer
    LSA_UINT8                       * pSFDataStatus;  //ptr to DG.APDU.SFDataStatus
    LSA_UINT8                       * pSFCycleCtr;    //ptr to DG.APDU.SFCycleCtr
    LSA_UINT8                       WDTReloadValue;   //Reloadvalue, taken from SFIOCRProperties.RestartFactorForDistributedWD
    LSA_UINT8                       WDTValue;         //WDTimer, counting down
    LSA_UINT8                       LastDS;           //last stored datastatus for ds_changed detection
    LSA_BOOL                        bInWDTList;       //DG is part of WDT list 
    LSA_BOOL                        bPSO;             //ProducerSwitchedOn. LSA_TRUE/FALSE: last state was "AGAIN"/"MISS" (initial state)
    LSA_UINT32                      SBEntryValue;    
} EDDI_DFP_SB_ENTRY_TYPE;

typedef struct _EDDI_DFP_SB_ENTRY_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DFP_SB_ENTRY_PTR_TYPE;
#endif

typedef struct _EDDI_CRT_CONS_SB_TYPE
{
    EDDI_SB_LINK                     Link;
    EDD_UPPER_MEM_U8_PTR_TYPE        pCopyKRAMConsDataStatus;  // Optimization to access the DataStatus faster
    LSA_UINT32                       Status;           // State of the EDD-Consumer-StateMachine.
    EDDI_CRT_CONS_UK_STATUS          UkStatus;         // State of the UnknownState-Substate
    #if defined (EDDI_CFG_USE_SW_RPS)
    LSA_INT32                        UkCountdown;      // Countdown for UnknownMode
    LSA_BOOL                         Invalid;
    #endif
    EDDI_SER_DATA_STATUS_TYPE        OldDataStatus;    // Old DataStatus to figure out which of the DataStatus-Bits had changed
    EDDI_SER_DATA_STATUS_TYPE        CurrentDataStatus;    // 

    LSA_UINT16                       ConsumerId;       // Unique ID in a Consumer-List, Needed to Control and to Remove a Consumer
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_DFP_SB_ENTRY_TYPE           SBEntryDFP;
    #endif
} EDDI_CRT_CONS_SB_TYPE;

//typedef EDDI_CRT_CONS_SB_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_CRT_CONS_SB_PTR_TYPE;
#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)

typedef enum _EDDI_SB_RTO_EV_TYPE
{
    EDDI_SB_RTO_EV_CTRL_DEACT,
    EDDI_SB_RTO_EV_CTRL_ACT,
    EDDI_SB_RTO_EV_CTRL_ACT_UK,
    EDDI_SB_RTO_EV_FRAME,
    EDDI_SB_RTO_EV_TIMEOUT

} EDDI_SB_RTO_EV_TYPE;

struct _EDDI_CRT_CONSUMER_TYPE;

typedef  LSA_VOID ( EDDI_LOCAL_FCT_ATTR   *EDDI_SB_RTO_FCT) ( const EDDI_SB_RTO_EV_TYPE                Event,
                                                              struct _EDDI_CRT_CONSUMER_TYPE  * const  pConsumer, 
                                                              EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB   );
typedef struct _EDDI_CRT_UDP_CONS_TYPE
{
    EDDI_DEV_MEM_U8_PTR_TYPE       pFrameBuffer_2;
    EDDI_DEV_MEM_U8_PTR_TYPE       pFrameBuffer_3;

    EDDI_DEV_MEM_U8_PTR_TYPE       pData;
    EDDI_DEV_MEM_U8_PTR_TYPE       pUser;
    EDDI_DEV_MEM_U8_PTR_TYPE       pNext;
    EDDI_DEV_MEM_U8_PTR_TYPE       pFree;

    //  LSA_UINT32                    FrameReceived;
    LSA_UINT32                     FramesMissed;
    LSA_UINT32                     Network_Ip_TotalLength;

    LSA_UINT32                     FrameLength;
    LSA_UINT32                     OffsetAPDU;
    LSA_UINT32                     OffsetRTData;
    EDDI_CRT_DATA_APDU_STATUS     *pAPDU;

    EDDI_SER_DATA_STATUS_TYPE      PrevDataStatus;    // Previous DataStatus to figure out which of the DataStatus-Bits had changed

    EDDI_SB_RTO_FCT                StateFct;
    EDDI_LOCAL_DDB_PTR_TYPE        pDDB;

} EDDI_CRT_UDP_CONS_TYPE;

//temporarily_disabled_lint -esym(768, _EDDI_TREE_LINK::TreeList)  not referenced

typedef union _EDDI_TREE_LINK
{
    struct _EDDI_CRT_CONSUMER_TYPE  *  pNext;  // Chain elements for phase tree:  DO NOT MOVE !!!
    E_TREE_LIST                        TreeList;

} EDDI_TREE_LINK;

#endif

#define EDDI_CRT_EVENT_TYPE    LSA_UINT32


#define EDDI_CONS_ACTIVE_NONE               0x0000
#define EDDI_CONS_ACTIVE_RTC3_MAIN          0x0001
#define EDDI_CONS_ACTIVE_AUX_MAIN           0x0002
#define EDDI_CONS_ACTIVE_CLASS12_MAIN       0x0100


typedef enum _EDDI_CONS_IRTTOP_SM_STATE_TYPE
{
    EDDI_CONS_STATE_PASSIVE,
    EDDI_CONS_STATE_WF_CLASS3_RX_NO_LEGACY,
    EDDI_CONS_STATE_WF_CLASS3_RX_LEGACY,
    EDDI_CONS_STATE_WF_IRDATA,
    EDDI_CONS_STATE_ACTIVE
} EDDI_CONS_IRTTOP_SM_STATE_TYPE;


typedef enum _EDDI_CONS_IRTTOP_SM_TRIGGER_TYPE
{
    EDDI_CONS_SM_TRIGGER_ACTIVATE_NO_IRDATA,   //activating without existing IRDATA
    EDDI_CONS_SM_TRIGGER_ACTIVATE_WITH_IRDATA, //activating with existing IRDATA
    EDDI_CONS_SM_TRIGGER_PASSIVATE,            //passivating main consumer
    EDDI_CONS_SM_TRIGGER_AGAIN_GREEN,          //Again indication (AUX)
    EDDI_CONS_SM_TRIGGER_AGAIN_RED,            //Again indication (Main)
    EDDI_CONS_SM_TRIGGER_MISS_GREEN,           //Miss-event (AUX)
    EDDI_CONS_SM_TRIGGER_MISS_RED              //Miss-event (Main)
} EDDI_CONS_IRTTOP_SM_TRIGGER_TYPE;


typedef struct _EDDI_CONS_IRTTOP_TYPE
{
    EDDI_CONS_IRTTOP_SM_STATE_TYPE     ConsState;
    struct _EDDI_CRT_CONSUMER_TYPE   * pAscConsumer;
    LSA_UINT16                         ConsActiveMask;
    LSA_UINT8                          HwPortIndex;
} EDDI_CONS_IRTTOP_TYPE;



typedef enum _EDDI_CONS_SCOREBOARD_SM_TRIGGER_TYPE
{
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_ACTIVATE,   //Activating event
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_PASSIVATE,   //Passivating event
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_MISS_EVENT,   //Miss event
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_DGMISS_EVENT,   //DG experiences MISS because its PF is passive again during Set2UK 
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_EVENT,   //Again event
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_AGAIN_HW_EVENT,   //Again event, generated by hw (prod state changed)
    EDDI_CONS_SM_SCOREBOARD_TRIGGER_SET_TO_UNKNOWN
} EDDI_CONS_SCOREBOARD_SM_TRIGGER_TYPE;


typedef enum _EDDI_CONS_SCOREBOARD_SM_STATE_TYPE
{
    EDDI_CONS_SCOREBOARD_STATE_PASSIVE,
    EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_INIT,
    EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED,
    EDDI_CONS_SCOREBOARD_STATE_MISS_NOT_STOPPED_UK,
    EDDI_CONS_SCOREBOARD_STATE_AGAIN,
    EDDI_CONS_SCOREBOARD_STATE_UNKNOWN,
    EDDI_CONS_SCOREBOARD_STATE_MISS
} EDDI_CONS_SCOREBOARD_SM_STATE_TYPE;


typedef enum _EDDI_CRT_CONS_HWWD
{
    EDDI_CRT_CONS_HWWD_CLEAR,
    EDDI_CRT_CONS_HWWD_LOAD,
    EDDI_CRT_CONS_HWWD_UNTOUCH
} EDDI_CRT_CONS_HWWD;


typedef struct _EDDI_CRT_CONSUMER_TYPE
{
    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    EDDI_TREE_LINK                     Link;
    EDDI_CRT_UDP_CONS_TYPE             Udp;
    struct _EDDI_CRT_CONSUMER_TYPE  *  pNext;
    #endif

    LSA_UINT16                         ConsumerId;       // Unique ID in a Consumer-List, Needed to Control and to Remove a Consumer
    LSA_UINT32                         UpperUserId;      // Needed in TimerScoreboard-Indications
    EDDI_CONS_SCOREBOARD_SM_STATE_TYPE ScoreBoardStatus; // Status for the Connection
    EDD_UPPER_MEM_U8_PTR_TYPE          pUserDataBuffer;  // Pointer to DataBuffer (can be in KRAM or User-RAM)
    LSA_UINT16                         Properties;
    EDDI_CONS_IRTTOP_TYPE              IRTtopCtrl;      

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    LSA_BOOL                           Locked;           // Describes if ConsumerBuffer is locked
    EDD_UPPER_RQB_PTR_TYPE             pRQB_PendingRemove;
    #endif

    EDDI_CRT_EVENT_TYPE                PendingIndEvent;  // Current pending Indication-Event for this Consumer
    LSA_UINT16                         PendingCycleCnt;  // Frame CycleCounter of Indication-Event
    LSA_UINT8                          PendingTransferStatus;
    LSA_BOOL                           bAUXTriggeredMiss;
    LSA_BOOL                           bHasSeenMiss;     // SetToUnknown on a passive consumer ...
                                                         // bHasSeenMiss is set     --> MISS
                                                         // bHasSeenMiss is not set --> MISS_NOT_STOPPED
    LSA_BOOL                           bHasSeenAgain;    // for RTC1/2-Consumer
                                                         //     if set     --> a valid consumer frame has been received once
                                                         //     if not set --> haven't received a valid frame since initialization
    LSA_UINT32                         EventStatus;      // Current accumulated status of the consumer-events
    LSA_UINT32                         PrimaryEventStartTime_ticks;  //Time when the PRIMARY-Event was detected 
    EDDI_CRT_CONSUMER_PARAM_TYPE       LowerParams;      // Contains all Data for the lower structures

    EDDI_IRT_FRM_HANDLER_PTR_TYPE      pFrmHandler;      // Pointer to FrameHandler-Element
    LSA_BOOL                           bRedundantFrameID;// TRUE if Redundant Consumer is available
    
    EDDI_CCW_CTRL_PTR_TYPE             pLowerCtrlACW;    // LowerLevel-Control-Structure for ACW-Entries

    EDDI_CRT_CONS_SB_TYPE           *  pSB;              // Optimierung SB Workauround
     
    #if defined (EDDI_CFG_REV7)
    LSA_BOOL                           usingPAEA_Ram;    //!< If TRUE: using IOC storing the user data in the PAEARam; else 3-Buffer-Interface is used
    #endif

    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    EDDI_CRT_CONSTRACE_TRACE_TYPE      trace;
    #endif

    LSA_BOOL                           bEnableAutoStop;    // TRUE: Autostop enabled on MISS
    LSA_BOOL                           bFocusAutoStop;     // TRUE: Focus only after 1st again indication, else FALSE
    EDDI_CRT_PROVIDER_TYPE           * pNextAutoStopProv;  // Shows at 1st provider

    #if defined (EDDI_CFG_DFP_ON)
    struct _EDDI_PRM_SF_DSCR_TYPE    * pSubFrmHandler;     // Pointer to SubFrameHandler-Element, in a PF: pointer to PF-Handler element
    LSA_UINT8                          DGActiveCtr;        // Ctr for active DGs 
    LSA_UINT8                          DGPassiveCtr;       // Ctr for passive DGs
    LSA_BOOL                           bIsPFConsumer;      // LSA_TRUE: Consumer references a packframe
    LSA_UINT16                         PFConsumerId;       // ConsumerID of the dedicated packframe
    #endif
} EDDI_CRT_CONSUMER_TYPE;
typedef       struct _EDDI_CRT_CONSUMER_TYPE EDDI_LOCAL_MEM_ATTR *       EDDI_CRT_CONSUMER_PTR_TYPE;
typedef const struct _EDDI_CRT_CONSUMER_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_CONST_CRT_CONSUMER_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* CRT-CONSUMER-List-Structure                                               */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_CRT_CONSRTC12EVENTLIST_TYPE
{
    EDDI_CRT_CONSUMER_TYPE      * pEntry;
    LSA_UINT32                    SBEntryValue;
} EDDI_CRT_CONSRTC12EVENTLIST_TYPE;

typedef struct _EDDI_CRT_CONSUMER_LIST_TYPE
{
    LSA_UINT32                         MaxEntries;           // number of available entries
    LSA_UINT32                         LastIndexRTC123;      // = index of currently highest valid entry
    LSA_UINT32                         LastIndexDFP;         // = index of currently highest valid entry
    LSA_UINT32                         UsedEntriesRTC123;    // number of currently valid entries RTC12
    LSA_UINT32                         UsedEntriesDFP;       // number of currently valid entries RTC3 and DFP
    LSA_UINT32                         ActiveConsRTC12;      // number of active RTC12 cons., includes AUX cons.
    LSA_UINT32                         ActiveConsRTC3;       // number of active RTC3 cons., excludes DFP cons., includes packframes, redundant cons. count as 1
    EDDI_CRT_CONSUMER_TYPE           * pEntry;               // pointer to array of consumers
    EDDI_CRT_CONS_SB_TYPE            * pSBEntry;             // separated from pEntry due to SB optimization
    EDDI_CRT_CONSRTC12EVENTLIST_TYPE * pConsRTC12EventID;    // pointer to list of consumers having a SB event.
    LSA_UINT32                         UsedACWs;

} EDDI_CRT_CONSUMER_LIST_TYPE;

typedef EDDI_CRT_CONSUMER_LIST_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_CRT_CONSUMER_LIST_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_CONS_H


/*****************************************************************************/
/*  end of file eddi_crt_cons.h                                              */
/*****************************************************************************/

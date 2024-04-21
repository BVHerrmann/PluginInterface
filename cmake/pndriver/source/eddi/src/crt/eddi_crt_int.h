#ifndef EDDI_CRT_INT_H          //reinclude-protection
#define EDDI_CRT_INT_H

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
/*  F i l e               &F: eddi_crt_int.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  CRT-                                                                     */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* CRT handle (channel) management-structures                                */
/*---------------------------------------------------------------------------*/

/******* Definitions of CYCLES and TICKS *************************************
   TICK:  One tick is the period of the used ASIC, and the granularity of
          CLK_COUNT_VALUE: Rev5/6 10ns, Rev7 1ns
   CYCLE: The counters defining the cycle-length run on all revisions with a
          granularity of 10ns. The cycle-length is a multiple of 31250ns
*/
#define EDDI_CRT_CYCLE_LENGTH_GRANULARITY       3125    // [Value in 10ns Ticks]
#define EDDI_CRT_CYCLE_LENGTH_GRANULARITY_NS    31250   // [Value in ns]
// Needed to set the correct Cyclelength and CycleEntity
// According to PNIO, CycleLength must be a multiple
// of 31,25 us =>  n * 3125 [in 10ns Ticks]


// invalid value
#define EDDI_CRT_INVALID_VALUE_8                0xFF
#define EDDI_CRT_INVALID_VALUE_16               0xFFFF
#define EDDI_CRT_INVALID_VALUE_32               0xFFFFFFFFUL
#define EDDI_CRT_INVALID_VALUE_64               0xFFFFFFFFFFFFFFFFULL


#if defined(EDDI_CFG_REV7)
#define EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS 10   //granularity of CLK_COUNT_VALUE and CLK_COUNT_BEGIN_VALUE
#define EDDI_CRT_NS_PER_CLK_COUNT_TICK      1    
#else
#define EDDI_CRT_CYCLE_COUNT_TICKS_PER_10NS 1    //granularity of CLK_COUNT_VALUE and CLK_COUNT_BEGIN_VALUE
#define EDDI_CRT_NS_PER_CLK_COUNT_TICK      10    
#endif

/************************************************************************
 * Event handling
 *
 ************************************************************************/
#define EDDI_CSRT_CONS_EVENT_NEGATION               EDD_CSRT_CONS_EVENT_NEGATION    //clear events instead of setting them
#define EDDI_CSRT_CONS_EVENT_RESET_EVENT_HANDLING   0xFFFFFFFFUL                    //clear all pending events and rebuild EventStatus
#define EDDI_CSRT_CONS_EVENT_CLEAR_PENDING_EVENTS   0xEFFFFFFFUL                    //clear all pending events and leave EventStatus untouched

#define EDD_CSRT_DSTAT_BITNR_STATE            0
#define EDD_CSRT_DSTAT_BITNR_REDUNDANCY       1

/*
#define EDD_CSRT_DSTAT_BITNR_DATA_VALID       2
#define EDD_CSRT_DSTAT_BITNR_3                3
#define EDD_CSRT_DSTAT_BITNR_STOP_RUN         4
#define EDD_CSRT_DSTAT_BITNR_STATION_FAILURE  5
#define EDD_CSRT_DSTAT_BITNR_6                6
#define EDD_CSRT_DSTAT_BITNR_IGNORE           7
*/
#define EDD_CSRT_DSTAT_BIT_STATE_BACKUP       0
#define EDD_CSRT_DSTAT_BIT_STATE_PRIMARY      1

#define EDDI_CSRT_SET_DATASTATUS_STATE(DataStatus_, NewState_)    {(DataStatus_) = ((DataStatus_) & ~EDD_CSRT_DSTAT_BIT_STATE) | ((NewState_) << EDD_CSRT_DSTAT_BITNR_STATE);/*lint -e941*/}
#define EDDI_CSRT_KEEP_DATASTATUS(DataStatus_, pAPDU_, KeepMask_) {(DataStatus_) = ((DataStatus_) & ~(KeepMask_)) | ((pAPDU_)->Detail.DataStatus & (KeepMask_));}

/************************************************************************
 * Remote-Provider-Surveillance
 *
 ************************************************************************/

typedef enum _EDDI_CRT_RPS_STATES                               // States of the Remote-Provider-Surveillance (RPS)
{
    EDDI_CRT_RPS_STS_NOT_AVAILABLE,                             // TimerScoreboard is not needed (ConsumerCnt == 0)
    EDDI_CRT_RPS_STS_SB_STOPPED,                                // TimerScoreboard is stopped.
    EDDI_CRT_RPS_STS_SB_STARTED_AND_WAITING_FOR_SB_INT,         // The Remote-Provider-Surveillance is waiting for new Scoreboard entries.
    EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED,               // We have new Scoreboard entries, but no free indication-ressource is available to inform the user.
    EDDI_CRT_RPS_STS_INDICATION_RESSOURCE_NEEDED_AND_STOPPED    // We have new Scoreboard entries, but no free indication-ressource is available to inform the user AND the Stae was STOPPED before
} EDDI_CRT_RPS_STATES;

typedef enum _EDDI_CRT_RPS_EVENT
{
    EDDI_CRT_RPS_EVENT_START,                    // Starts the Remote-Provider-Surveillance
    EDDI_CRT_RPS_EVENT_STOP,                     // Stops the Remote-Provider-Surveillance
    EDDI_CRT_RPS_EVENT_SB_CHANGED,               // SB changed
    EDDI_CRT_RPS_EVENT_NEW_IND_RESOURCE,         // Inform that new IndicationRessource is available
    EDDI_CRT_RPS_EVENT_NEEDED_IND_SENT_TO_USER,  // Send needed Indication to user 
    EDDI_CRT_RPS_EVENT_NO_RESSOURCE_AVAILABLE    // Inform if no free indication-ressource is available 
} EDDI_CRT_RPS_EVENT;

/*---------------------------------------------------------------------------*/
/* CRT_SCORE_BOARD_LIST_TYPE-Structure                                       */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_CRT_SCORE_BOARD_LIST_TYPE
{
    EDDI_SER10_TIMER_SCORB_TYPE            *pDevSBTimerBase;
    EDDI_SER10_PROV_SCORB_PTR_TYPE         pDevSBChangeList[2];
    LSA_UINT32                             DevSBChangeListSizeInUint32; 
} EDDI_CRT_SCORE_BOARD_LIST_TYPE;

typedef struct _EDDI_CRT_RPS_TYPE    // Contains all Variables handling the Remote-Provider-Surveillance
{
    EDDI_CRT_RPS_STATES                 Status;             // Status of Remote-Provider-Surveillance

    //LSA_UINT32                         PendingEventsCount; // Counts all pending indication-events of Consumers
    LSA_BOOL                            PendingEvents;      //events occured during an asynchronous service
    LSA_UINT32                          MaxPendingDGCons;    //Highest DG-ConsID where an event is pending
    // support of HW-TimerScoreboard
    EDDI_CRT_SCORE_BOARD_LIST_TYPE      SBList;
    #if defined (EDDI_CFG_DFP_ON)
    struct _EDDI_DFP_SB_LINK_TYPE       DFPSB;
    LSA_UINT16                          DFPSWSBTimerID;
    #endif
} EDDI_CRT_RPS_TYPE;

//typedef struct _EDDI_CRT_RPS_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_CRT_RPS_PTR_TYPE;

typedef struct _EDDI_DDB_COMP_CRT_POOL_TYPE
{
    LSA_UINT32                        AcwProvider;
    LSA_UINT32                        AcwConsumer;
    LSA_UINT32                        CCWCTRL;
    LSA_UINT32                        ApduAuxConsumer;
    #if defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
    LSA_UINT32                        IOCWRef;
    #endif
    #if defined (EDDI_CFG_REV7)
    EDDI_MEM_BUF_EL_H                * pMemHeaderStateBuffer;
    #endif

} EDDI_DDB_COMP_CRT_POOL_TYPE;

#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
typedef struct EDDI_CRT_XCHANGE_TYPE
{
    EDDI_QUEUE_TYPE                    RQBQueue;                 //Queue for deferred Xchange-Buffer-Requests
    EDDI_QUEUE_ELEMENT_PTR_TYPE        pFinishBorder;            //Pointer to a Queue-Element of ProvXchangeQueue,
                                                                 //which will be delayed until next Cycle.

} EDDI_CRT_XCHANGE_TYPE;
#endif

typedef struct _EDDI_RTCLASS2_TYPE
{
    LSA_UINT32                         Status;                   //Status of ERTEC-Global BandWidth-Reservation.
    LSA_UINT32                         ReservedIntervalBegin;    //Begin of ERTEC-Global BandWidth-Reservation.
                                                                 //(not supported yet)
    LSA_UINT32                         ReservedIntervalEnd;      //End of ERTEC-Global BandWidth-Reservation.

    EDDI_SER10_ACW_SND_TYPE            *pRootAcw;

    EDDI_LOCAL_EOL_PTR_TYPE            pEOL;                     //local Pointer to EOL

} EDDI_RTCLASS2_TYPE;

typedef struct _EDDI_CRT_CONTEXT_PROV_REM_TYPE
{
    LSA_BOOL                           InUse;
    EDDI_CRT_PROVIDER_PTR_TYPE         pProvider;

} EDDI_CRT_CONTEXT_PROV_REM_TYPE;

typedef struct _EDDI_CRT_CONTEXT_PROV_CH_PHASE_TYPE
{
    LSA_BOOL                           InUse;
    EDDI_SER_CCW_PTR_TYPE              pOldAcw;

} EDDI_CRT_CONTEXT_PROV_CH_PHASE_TYPE;

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
typedef LSA_VOID ( EDDI_LOCAL_FCT_ATTR * INI_RCW_FCT)( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                       EDDI_LOCAL_RCW_RED_PTR_TYPE  const  pRCW,
                                                       LSA_UINT32                   const  PhaseCnt,
                                                       LSA_UINT32                   const  SendClock,
                                                       LSA_UINT32                   const  ReductionRate,
                                                       LSA_UINT32                   const  SendClockShift);
#endif

typedef struct _EDDI_CRT_META_INFO_TYPE
{
    LSA_UINT32                AcwProviderCnt;
    LSA_UINT32                FcwProviderCnt;
    LSA_UINT32                AcwConsumerCnt;
    LSA_UINT32                FcwConsumerCnt;
    LSA_UINT32                RTC123IDStart;    //1st ConsID used for RTC123
    LSA_UINT32                RTC123IDEnd;      //(last-1) ConsID used for RTC123
    LSA_UINT32                DFPIDStart;       //first ConsID used for DFP
    LSA_UINT32                DFPIDEnd;         //(last-1) ConsID used for DFP
    LSA_UINT32                RTC12IDStartProv; //1st ProvID used for RTC12
    LSA_UINT32                RTC12IDEndProv;   //(last-1) ProvID used for RTC12
    LSA_UINT32                RTC3IDStartProv;  //1st ProvID used for RTC3
    LSA_UINT32                RTC3IDEndProv;    //(last-1) ProvID used for RTC3
    LSA_UINT32                DFPIDStartProv;   //first ProvID used for DFP
    LSA_UINT32                DFPIDEndProv;     //(last-1) ProvID used for DFP

} EDDI_CRT_META_INFO_TYPE;

#if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
typedef struct _EDDI_CONS_QUEUE_TYPE
{
    EDDI_CRT_CONSUMER_TYPE     *pBottom;
    EDDI_CRT_CONSUMER_TYPE     *pTop;

    LSA_UINT32                  Cnt;

} EDDI_CONS_QUEUE_TYPE;

typedef struct _EDDI_PROV_QUEUE_TYPE
{
    EDDI_CRT_PROVIDER_TYPE     *pBottom;
    EDDI_CRT_PROVIDER_TYPE     *pTop;

    LSA_UINT32                  Cnt;

} EDDI_PROV_QUEUE_TYPE;
#endif

#if defined (EDDI_CFG_REV7)
/**
 *  Stores all IOC/PAEA-RAM related parameters.
 */
typedef struct _EDDI_CRT_PAEA_TYPE
{
    /**< absolute address were user related application Data can be stored.
          If value != null, PAEA-Mode is switched on */
    LSA_UINT32                      PAEA_BaseAdr;

    /**< Each VectorEntry holds a pointer to an APDU memory slot in the KRAM.
         This reference is needed to associate a ConsumerID with its APDU-status
         during a XCHANGE_BUF operation. @see EDDI_MemGetApduBuffer */
    LSA_UINT32                      *pConsumerMirrorArray;

    /**< Length of pConsumerMirrorArray, to check buffer-overflow */
    LSA_UINT32                      ConsumerMirrorArrayLen;

} EDDI_CRT_PAEA_TYPE;
#endif

/*---------------------------------------------------------------------------*/
/* CRT-COMP-Structure                                                        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_DDB_COMP_CRT_TYPE
{
    LSA_UINT32                         UsrHandleCnt;           // Number of handles using
    // CRT. (only 1 allowed)
    LSA_UINT16                         CycleBaseFactor;

    EDDI_LOCAL_HDB_PTR_TYPE            pHDB;

    EDDI_CRT_INI_TYPE                  CfgPara;                // All Initialization-Params
    EDDI_CRT_SYNC_IMAGE_TYPE           SyncImage;              // Parameters needed for the Synchron-Images  

    EDDI_CRT_META_INFO_TYPE            MetaInfo;

    EDDI_CRT_PROVIDER_LIST_TYPE        ProviderList;          // the CRT-Provider-List
    EDDI_CRT_CONSUMER_LIST_TYPE        ConsumerList;          // the CRT-Consumer-List
    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    EDDI_CRT_CONSUMER_TRACE_TABLE    * pConsTable;            // table of consumer-frame-ids for easier debugging
    #endif
    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    EDDI_CONS_QUEUE_TYPE               UdpConsQueue;
    EDDI_PROV_QUEUE_TYPE               UdpProvQueue;
    #endif
    // This Struct can store:
    // - A defered SENT Indication, wich will be fired in the EDDI_CRTRequest-Function
    //   In future When SER_ProviderAdd will work asynchronously this struct will be obsolete.

    // - A defered AGAIN/MISS - Indication triggered by the ProviderCtrl-SetToUnknownState-Function.
    //   It will be fired in the EDDI_CRTRequest-Function.

    LSA_UINT32                         PendingProvEventsCount;// Count of ProviderEvents to be sent

    EDDI_QUEUE_TYPE                    FreeIndicationQueue;   // Indication-Auftrags-Queue

    EDDI_CRT_RPS_TYPE                  Rps;                   // Struct to handle Remote-Provider-Surveillance

    EDDI_SET_DMA_CTRL_TYPE             DMACtrl;

    EDDI_TREE_TYPE                     SrtRedTree;            // ReductionTree for SRT

    EDDI_IRT_REDUCTION_AX_TYPE         IRTRedAx;

    EDDI_DDB_COMP_CRT_POOL_TYPE        hPool;

    #if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)
    EDDI_CRT_XCHANGE_TYPE              XChangeHandling;
    #endif
    #if defined (EDDI_CFG_USE_SW_RPS)
    E_TREE                           * pSBTree;
    #endif

    EDDI_RTCLASS2_TYPE                 RTClass2Interval;

    EDDI_CRT_CONTEXT_PROV_REM_TYPE     ContextProvRemove;     // stores lowLevel-Context to Handle asynchronous
    // removing of SRT-Provider

    EDDI_CRT_CONTEXT_PROV_CH_PHASE_TYPE  ContextProvChPhase;  // stores lowLevel-Context to Handle asynchronous
    // ChangePhase-Command of SRT-Provider

    EDDI_CRT_PHASE_TX_MACHINE          PhaseTx[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine CRT-Phases of TX-Ports
    EDDI_CRT_PHASE_RX_MACHINE          PhaseRx[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine CRT-Phases of RX-Ports

    EDDI_CYCLE_COUNTER_TYPE            SrtFcwEmptyCycleCounter;// Will be set on SrtFcwEmpty-Interrupt
    // Needed to combine multiple SrtFcwEmpty-
    // Interrupts inbetween one Cycle.

    LSA_UINT32                        KRAMSize_xRT_Trees;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    INI_RCW_FCT                       SERIniRcwFct;
    #endif

    #if defined (EDDI_CFG_REV7)
    EDDI_CRT_PAEA_TYPE                PAEA_Para;      //!< stores IOC/PAEA-RAM related Parameters
    #endif

} EDDI_DDB_COMP_CRT_TYPE;

typedef struct _EDDI_DDB_COMP_CRT_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_INT_H


/*****************************************************************************/
/*  end of file eddi_crt_int.h                                               */
/*****************************************************************************/

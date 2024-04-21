#ifndef EDDI_SYNC_TYP_H         //reinclude-protection
#define EDDI_SYNC_TYP_H

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
/*  F i l e               &F: eddi_sync_typ.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  19.02.08    JS    moved EDDI_IRT_MAX_TREE_ELEMENTS to eddi_sync_typ.h    */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* SYNC DDB_COMP-Structure                                                   */
/*---------------------------------------------------------------------------*/

//temporarily_disabled_lint -esym(756, _EDDI_SYNC_IRT_META_INFO_TYPE*)
//temporarily_disabled_lint -esym(768, _EDDI_SYNC_IRT_META_INFO_TYPE*)
//temporarily_disabled_lint -esym(756, EDDI_SYNC_IRT_META_INFO_TYPE*)
//temporarily_disabled_lint -esym(528, SyncSnd_*)
//temporarily_disabled_lint -esym(759, SyncSnd_*)
//temporarily_disabled_lint -esym(765, SyncSnd_*)
//temporarily_disabled_lint -esym(714, SyncSnd_*)
//temporarily_disabled_lint -esym(768, _EDDI_SYNC_SND_MACHINE)
//temporarily_disabled_lint -esym(750, EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_END)

//temporarily_disabled_lint -esym(714, SyncRcv_*)
//temporarily_disabled_lint -esym(765, EDDI_SyncRcvTrigger)
//temporarily_disabled_lint -esym(759, EDDI_SyncRcvTrigger)

//temporarily_disabled_lint -esym(755, EDDI_DIAG_SOURCE_)

//temporarily_disabled_lint -esym(961, EDDI_NTOHL)

//temporarily_disabled_lint -esym(765, SyncTimer*)
//temporarily_disabled_lint -esym(714, SyncTimer*)
//temporarily_disabled_lint -esym(759, SyncTimer*)
//temporarily_disabled_lint -esym(757, SyncTimer*)
//temporarily_disabled_lint -esym(528, SyncTimer*)

//temporarily_disabled_lint -esym(768, _EDDI_SYNC_RCV_TYPE*)
//temporarily_disabled_lint -esym(768, _EDDI_DDB_COMP_SYNC_TYPE*)
//temporarily_disabled_lint -esym(768, _PROF_EDDI_INT_STATS_INFO*)

//temporarily_disabled_lint -esym(757, EDDI_SERIrtForwardingDeactivate)

//temporarily_disabled_lint -esym(757, EDDI_SyncIrFindFrameHandlerForSyncProvider)
//temporarily_disabled_lint -esym(759, EDDI_SyncIrFindFrameHandlerForSyncProvider)
//temporarily_disabled_lint -esym(765, EDDI_SyncIrFindFrameHandlerForSyncProvider)
//temporarily_disabled_lint -esym(757, EDDI_SyncIrFindFrameHandlerForSyncConsumer)
//temporarily_disabled_lint -esym(759, EDDI_SyncIrFindFrameHandlerForSyncConsumer)
//temporarily_disabled_lint -esym(765, EDDI_SyncIrFindFrameHandlerForSyncConsumer)

//temporarily_disabled_lint -esym(759, EDDI_SyncPortStmsTransitionBegin)
//temporarily_disabled_lint -esym(765, EDDI_SyncPortStmsTransitionBegin)

//temporarily_disabled_lint -esym(759, EDDI_SyncRcvRTDoneHook)
//temporarily_disabled_lint -esym(765, EDDI_SyncRcvRTDoneHook)
//temporarily_disabled_lint -esym(755, M_ID_SYNC_CALC)

//temporarily_disabled_lint -esym(768, _EDDI_SYNC_PTCP_PAYLOAD_TYPE*)
//temporarily_disabled_lint -esym(768, _EDDI_SYNC_FRAME_BUFFER_TYPE*)

//temporarily_disabled_lint -esym(755, _EDDI_PRM_RECORD_FRAME_DETAILS_TYPE*)
//temporarily_disabled_lint -esym(755, _EDDI_PRM_RECORD_FRAME_DETAILS_BIT*)
//temporarily_disabled_lint -esym(755, EDDI_IMAGE_MODE_STAND_ALONE)
//temporarily_disabled_lint -esym(755, EDDI_SYNC_PTCP_CLOCK_ROLE_PRM*)
//temporarily_disabled_lint -esym(755, EDDI_DS_BIT2_DATA_VALID)
//temporarily_disabled_lint -esym(755, EDDI_ERR_INV_FrameDetails)
//temporarily_disabled_lint -esym(755, EDDI_CALC_SYNC_DIAG_ENTRY_CNT)
//temporarily_disabled_lint -esym(755, EDDI_ERR_INV_PDIRData_PDSyncData_eddiPrmCheckConsistency)
//temporarily_disabled_lint -esym(755, EDDI_ERR_INV_PortID_NotInterface_ReadPDSyncTraceRTCData)
//temporarily_disabled_lint -esym(755, EDDI_ERR_INV_SyncProperties_PDIRPDSyncConsistency)
//temporarily_disabled_lint -esym(755, EDDI_ERR_INV_SyncProperties_Role)
//temporarily_disabled_lint -esym(755, EDDI_PRM_LENGTH_PD_SYNC_TRACE_RTC_DATA)
//temporarily_disabled_lint -esym(755, EDDI_PRM_LENGTH_SYNC_DIAG_ENTRY)
//temporarily_disabled_lint -esym(755, EDDI_SYNC_PTCP_CLOCK_ROLE_PRM)
//temporarily_disabled_lint -esym(755, EDDI_SYNC_PTCP_CLOCK_ROLE_SEC)

//temporarily_disabled_lint -esym(769, _EDDI_10NS_TIMER_ENUM*)
//temporarily_disabled_lint -esym(768, _EDDI_DDB_INT_STAT_TYPE*)

//temporarily_disabled_lint -esym(759, EDDI_SERXchangeBuffer)
//temporarily_disabled_lint -esym(765, EDDI_SERXchangeBuffer)
//temporarily_disabled_lint -esym(714, EDDI_SERXchangeBuffer)
//temporarily_disabled_lint -esym(756, EDDI_SER_DRIFT_COR_TYPE)
//temporarily_disabled_lint -esym(714, EDDI_SyncIrFindFrameHandlerForSyncProvider)
//temporarily_disabled_lint -esym(714, EDDI_SyncIrFindFrameHandlerForSyncConsumer)

//temporarily_disabled_lint -esym(759, EDDI_SERConnectCwToBranch)
//temporarily_disabled_lint -esym(765, EDDI_SERConnectCwToBranch)

//temporarily_disabled_lint -esym(768, _EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE*)

#define EDDI_SYNC_MAX_NUMBER_OF_FRAME_ELEMENTS 512UL
#define EDDI_SYNC_MAX_IR_FRAMEDATA_LENGTH_EXT  (84UL + (EDDI_SYNC_MAX_NUMBER_OF_FRAME_ELEMENTS * 20UL) + EDDI_PRM_MAX_PDIR_BEGIN_END_DATA_LENGTH)

#define EDDI_SYNC_MAX_SYNCDATA_LENGTH          (312)

#define EDDI_SYNC_MAX_REDUNDANT_ELEMENTS       (2UL)

typedef struct EDDI_SYNC_MEM_POOL_TYPE
{
    LSA_UINT32              hIrtSyncSnd;        // SingleShotSnd-Element for  buffered Sync-Sender
    LSA_UINT32              hIrtProviderFcw;    // Contains KRAM-Ressources for IRTProvider-FCWs
    LSA_UINT32              hIrtConsumerFcw;    // Contains KRAM-Ressources for IrtConsumer-FCWs
    // and forwarding IrtConsumer-FCWs
    LSA_UINT32              hIrtForwarderFcw;   // Contains KRAM-Ressources for IrtForwarder-FCWs
    LSA_UINT32              hSyncFrameBuffer;   // Contains KRAM-Ressources for IrtSyncFrames
    // (TxBuffer for IRTSyncMaster or
    //  RxBuffer for IRTSyncSlave)

} EDDI_SYNC_MEM_POOL_TYPE;

/***********************************************************************************/
/* FRAME_HANDLER binds all FCWRx FCWTx entry to one frame                          */
/***********************************************************************************/

#define EDDI_IRT_MAX_TREE_ELEMENTS  400

typedef enum _EDDI_SYNC_IRT_FRM_HANDLER_ENUM
{
    EDDI_SYNC_IRT_INVALID_HANDLER, // HandlerType is not valid
    EDDI_SYNC_IRT_PROVIDER,        // also used for IRTSyncMaster-Entries
    EDDI_SYNC_IRT_CONSUMER,        // also used for IRTSyncSlaves-Entries
    EDDI_SYNC_IRT_FWD_CONSUMER,    // also used for IRTSyncSlaves-Entries
    EDDI_SYNC_IRT_FORWARDER

} EDDI_SYNC_IRT_FRM_HANDLER_ENUM;

typedef struct _EDDI_IRT_FRM_HANDLER_TYPE
{
    EDDI_QUEUE_ELEMENT_TYPE              QueueLink;             // Contains Queuing-Links
    struct _EDDI_IRT_FRM_HANDLER_TYPE  * pFrmHandlerRedundant;
    
    EDDI_SYNC_IRT_FRM_HANDLER_ENUM       HandlerType;           // Type of Handler (Provider, Consumer etc.)

    LSA_BOOL                             UsedByUpper;           // LSA_TRUE:  This FrameHandler has an upper

    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  pIrFrameDataElem[EDDI_MAX_IRTE_PORT_CNT];
                                                                // Pointer to Element in IrRecordByteStream
                                                                // --> RawData needed to get all infos for one or more FCWs
                                                                // Der Index hat nichts mit dem UsrPortIndex zu tun
                                                                // 1.  IrFrameDataElem -> 1. Eintrag

    LSA_UINT16                           FrameId;               // FrameId
    EDDI_TREE_ELEM_PTR_TYPE              pIRTRcvEl[EDDI_MAX_IRTE_PORT_CNT];
    EDDI_TREE_ELEM_PTR_TYPE              pIRTSndEl[EDDI_MAX_IRTE_PORT_CNT];

} EDDI_IRT_FRM_HANDLER_TYPE;

typedef struct _EDDI_IRT_FRM_HANDLER_TYPE EDDI_LOCAL_MEM_ATTR       *       EDDI_IRT_FRM_HANDLER_PTR_TYPE;
typedef struct _EDDI_IRT_FRM_HANDLER_TYPE EDDI_LOCAL_MEM_ATTR const * EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE;

// Controls the SyncSend - mechanisms
typedef struct _EDDI_SYNC_SND_TYPE
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE   pFrmHandler;                        // FrameHandler (PROVIDER !) for buffered SyncSender
    LSA_UINT32                      ExpectedFrameLenWithApdu;           // Length of SyncPDU without FU on Network
    // Contains [EthernetHeader + Payload + APDU]
    // Typic: 94 Bytes

    EDDI_SYNC_SND_MACHINE           BufferdSndStm[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine[HwPortIndex] for buffered SyncSender

} EDDI_SYNC_SND_TYPE;

typedef struct _EDDI_SYNC_RCV_TYPE
{
    EDDI_IRT_FRM_HANDLER_PTR_TYPE   pFrmHandler;                        // FrameHandler (CONSUMER !) for buffered SyncReceiver
    LSA_UINT32                      ExpectedFrameLenWithApdu;           // Length of SyncPDU without FU on Network
    // Contains [EthernetHeader + Payload + APDU]
    // Typic: 94 Bytes

    EDDI_SYNC_RCV_MACHINE           BufferdRcvStm[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine[HwPortIndex] for buffered SyncReceiver

} EDDI_SYNC_RCV_TYPE;

typedef struct _EDDI_SYNC_IRT_META_INFO_TYPE
{
    LSA_UINT32                      UsrCnt;         // End   of the last  Frame passing this port

} EDDI_SYNC_IRT_META_INFO_TYPE;


//BitMasks used with FreezeResetMask in EDDI_SYNC_PORT_DATA
#define EDDI_SYNC_FREEZE_RESET_FOR_PRM       0x1UL  // Flag to Freeze Reset for PRM_COMMIT
#define EDDI_SYNC_FREEZE_RESET_FOR_SENDCLK   0x2UL  // Flag to Freeze Reset for SendclockChange

// Contains port specific input data for all port state machines:
typedef struct _EDDI_SYNC_PORT_DATA
{
    LSA_BOOL      doRtClass3Reset;  //Set if IRT_CLASS3_PORT_STMS must reach the OFF-State
    LSA_UINT32    FreezeResetMask;  //Set if ResetState shall be frozen for some time. e.g for FCW-Listchanges during PRM_COMMIT

    LSA_BOOL      isMauTypeOk;
    LSA_BOOL      isPortStateOk;
    LSA_BOOL      RTSync_isLinedelayPresent;
    LSA_BOOL      RTSync_isBoundaryIngress[2];  /* ingress (in) boundary */
    LSA_BOOL      RTSync_isBoundaryEgress[2];   /* egress (out) boundary */
    LSA_UINT32    RTSync_TopoState;
    LSA_UINT32    RtClass2_TopoState;
    LSA_UINT32    RtClass3_TopoState;
    LSA_UINT32    RtClass3_PDEVTopoState;
    LSA_UINT32    RtClass3_RemoteState;

} EDDI_SYNC_PORT_DATA;

// Contains all input data for all port state machines:
typedef struct _EDDI_SYNC_PORT_MACHINES_INPUT
{
    LSA_BOOL             isRateValid;
    LSA_BOOL             isLocalSyncOk;
    LSA_BOOL             RtClass2_isReservedIntervalPresent;
    EDDI_SYNC_PORT_DATA  PortData[EDDI_MAX_IRTE_PORT_CNT];

} EDDI_SYNC_PORT_MACHINES_INPUT;

#if defined (EDDI_RED_PHASE_SHIFT_ON)

typedef LSA_VOID  (EDDI_LOCAL_FCT_ATTR  *  EDDI_SYNC_TRANSITION_END_FCT_TYPE)(EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#define EDDI_SYNC_PORT_STM_MAX_TRANSITION_TIMEOUT_10NS  (100 * 1000 * 100) //Maximum allowed time for a SOL transition: 100ms (formerly: 12ms)

typedef struct _EDDI_SYNC_PORT_MACHINES_TRANSITION
{
    EDDI_SYNC_TRANSITION_END_FCT_TYPE   PrmTransitionEndCbf;      // PRM-Cbf to be called after End of Transition
    EDDI_SYNC_TRANSITION_END_FCT_TYPE   SndClkChTransitionEndCbf; // SendClockChange-Cbf
    LSA_BOOL                            bTransitionRunning;       // TRUE if one or more IRT_PORT_STMs have a running SOL-Transition
    LSA_INT32                           Timeout10ns;              // This Timeout will count down in EDDI_NewCycleReduced()
    LSA_UINT32                          LastTimeStamp;            // Stores timestamp of last count down

} EDDI_SYNC_PORT_MACHINES_TRANSITION;

#endif //end of EDDI_RED_PHASE_SHIFT_ON

// Container for all port Statemachines:
typedef struct _EDDI_SYNC_PORT_MACHINES
{
    EDDI_SYNC_PORT_MACHINES_INPUT       Input;                                    // Input Data for the logic
    //EDDI_RTCLASS2_TYPE                RTClass2Interval;                         // Paramters for the Underlying HW Parameters
    EDDI_RT_CLASS2_MACHINE              RtClass2_Machine[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine for RtClass2 Activity
    EDDI_RT_CLASS3_MACHINE              RtClass3_Machine[EDDI_MAX_IRTE_PORT_CNT]; // Statemachine for RtClass3 Activity
    EDDI_SYNC_FWD_MACHINE               SyncFwd_Machine [EDDI_MAX_IRTE_PORT_CNT]; // Statemachines for SyncForwarding one for each HwRxPort

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_SYNC_PORT_MACHINES_TRANSITION Transition;
    #endif

} EDDI_SYNC_PORT_MACHINES;

typedef struct _EDDI_SYNC_PORT_DATA EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_SYNC_PORT_DATA_PTR_TYPE;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++
//  Sync-Component
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define EDDI_SYNC_IDX_CNT           2   /* number ob rcv array elements */

typedef struct _EDDI_DDB_COMP_SYNC_TYPE
{                                   
    LSA_UINT32                      UsrHandleCnt;       // Number of handles using

    EDDI_LOCAL_HDB_PTR_TYPE         pHDB;               // HDB of current user

    LSA_BOOL                        SyncActivity;       // Describes global RTC-Sync-Activity
    LSA_BOOL                        IrtActivity;        // Describes global HW-IRT-Activity (RTC3 is projected?)

    LSA_UINT32                      SyncFrmHandlerCnt;  // Count of planned PtcpFrmHandlers:
    //   1: No   System-Redundancy
    //   2: With System-Redundancy

    LSA_UINT16                      SyncRole;               // Role Found in SyncProperties:
    // SYNC_PROP_ROLE_CLOCK_MASTER
    // SYNC_PROP_ROLE_EXTERNAL_SYNC

    LSA_UINT16                      SyncClass;              // SyncClass needed for Best Master Algo
    // Primary or Secondary

    EDDI_SYNC_SND_TYPE              Snd;                    // Contains ressources for RTSync-Send functionality
    EDDI_SYNC_RCV_TYPE              Rcv[EDDI_SYNC_IDX_CNT]; // Responsible for Receiving Sync and Announceframes
    // Index 0: Primary
    // Index 1: Secondary

    LSA_INT32                       GetParamsDriftIntervalNs;   // Only use for GetParams-Service !!

    LSA_UINT32                      n_IrtSyncSndCnt;
    LSA_UINT32                      n_IrtProviderFcwCnt;
    LSA_UINT32                      n_IrtConsumerFcwCnt;
    LSA_UINT32                      n_IrtForwarderFcwCnt;
    LSA_UINT32                      n_SyncFrameBuffer;
    LSA_UINT32                      Size_SyncFrameBuffer;

    LSA_UINT32                      FrameHandlerCnt;

    EDDI_SYNC_MEM_POOL_TYPE         MemPool;                // Contains KRAM-Ressources for IRT and RTSync Elements


    EDDI_SYNC_PORT_MACHINES         PortMachines;           // Contains all port state machines

    EDDI_NRT_RX_USER_TYPE           RxUser;                 // User-Receive queue (from EDD_SRV_SYNC_RECV)

    LSA_BOOL                        bSyncInRedActive;

} EDDI_DDB_COMP_SYNC_TYPE;

typedef struct _EDDI_DDB_COMP_SYNC_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_TYP_H


/*****************************************************************************/
/*  end of file eddi_sync_typ.h                                              */
/*****************************************************************************/

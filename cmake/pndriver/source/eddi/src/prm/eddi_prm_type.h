#ifndef EDDI_PRM_TYPE_H         //reinclude-protection
#define EDDI_PRM_TYPE_H

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
/*  F i l e               &F: eddi_prm_type.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Version of prefix                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  08.01.08    JS    Added MRP-Records                                      */
/*                                                                           */
/*****************************************************************************/

#define EDDI_CALC_SYNC_DIAG_ENTRY_CNT(_SyncDiagEntryCnt,_MAX_ENTRY) ( _SyncDiagEntryCnt = ( (_SyncDiagEntryCnt > _MAX_ENTRY) ? _MAX_ENTRY : _SyncDiagEntryCnt ) )

typedef enum _EDDI_PRM_DIAG_PDNC_EVENT
{
    EDDI_PRM_EV_GOOD,
    EDDI_PRM_EV_REQUIRED,
    EDDI_PRM_EV_DEMANDED,
    EDDI_PRM_EV_ERROR,
    EDDI_PRM_EV_START

} EDDI_PRM_DIAG_PDNC_EVENT;

/*  PK:RTSync
typedef enum _EDDI_PRM_DIAG_PDSYNC_EVENT
{
  EDDI_PRM_EV_SYNC_OK,
  EDDI_PRM_EV_OUT_OF_BOUNDARY,
  EDDI_PRM_EV_WRONG_IRDATAID,
  EDDI_PRM_EV_WRONG_PTCPSUBDOMAINID,
  EDDI_PRM_EV_NO_MESSAGE_RECEIVED,
  EDDI_PRM_EV_WRONG_IRDATA_PTCPSUBDOMAIN_ID,
  EDDI_PRM_EV_START_PDSYNC

} EDDI_PRM_DIAG_PDSYNC_EVENT;
*/

typedef enum _EDDI_PRM_STATE
{
    EDDI_PRM_NOT_VALID,
    EDDI_PRM_WRITE_DONE,
    EDDI_PRM_VALID

} EDDI_PRM_STATE;

typedef LSA_RESULT ( EDDI_LOCAL_FCT_ATTR *PRM_STATE_FCT)( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                          EDDI_LOCAL_HDB_PTR_TYPE  const pHDB,
                                                          EDD_SERVICE             const Service );

typedef struct _EDDI_PRM_PDNC_DATA_CHECK  * EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE;

typedef LSA_VOID   ( EDDI_LOCAL_FCT_ATTR *PRM_STATE_DIAG_PDNC_FCT)( EDDI_PRM_PDNC_DATA_CHECK_PTR_TYPE   const pPDNC,
                                                                    EDDI_PRM_DIAG_PDNC_EVENT          const DiagEv,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE           const pDDB);

typedef struct _EDDI_PRM_PDSYNC_DATA  *  EDDI_PRM_PDSYNC_DATA_PTR_TYPE;

typedef struct _EDDI_PRM_PDSCF_DATA_CHECK
{
    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE    RecordSet_A;
    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE    RecordSet_B;

    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE    *pA;
    EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE    *pB;

    EDDI_PRM_STATE                           State_A;
    EDDI_PRM_STATE                           State_B;

    LSA_UINT16                               edd_port_id;

    LSA_UINT16                               SlotNumber;
    LSA_UINT16                               SubSlotNumber;

    LSA_UINT32                               RecordSize;

} EDDI_PRM_PDSCF_DATA_CHECK;

typedef struct _EDDI_PRM_PDTRACE_UNIT_CONTROL
{
    EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE     RecordSet_A[EDD_CFG_MAX_PORT_CNT]; /* user view */
    EDDI_PRM_STATE                                State_A    [EDD_CFG_MAX_PORT_CNT]; /* user view */

} EDDI_PRM_PDTRACE_UNIT_CONTROL;

typedef struct _EDDI_PRM_PDCONTROL_PLL
{
    EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE     RecordSet_A;
    EDDI_PRM_STATE                         State_A;
    LSA_BOOL                               PDControlPllActive; /* determine synchronize signal active */

} EDDI_PRM_PDCONTROL_PLL;

typedef struct _EDDI_PRM_CHECK_DROP_BUDGET
{
    LSA_UINT32    ErrorDropBudget;
    LSA_UINT32    DoCheckError;

    LSA_UINT32    MaintenanceDemandedDropBudget;
    LSA_UINT32    DoCheckDemanded;

    LSA_UINT32    MaintenanceRequiredDropBudget;
    LSA_UINT32    DoCheckRequired;

    LSA_UINT32    DoCheck;

} EDDI_PRM_CHECK_DROP_BUDGET;

typedef struct _EDDI_PRM_PDNC_DATA_CHECK
{
    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE   RecordSet_A;
    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE   RecordSet_B;

    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE   *pA;
    EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE   *pB;
    
    PRM_STATE_DIAG_PDNC_FCT                DiagStateFct;

    EDDI_PRM_STATE                         State_A;
    EDDI_PRM_STATE                         State_B;

    EDDI_PRM_DIAG_PDNC_EVENT               Last_Event_Appears;
    EDDI_PRM_DIAG_PDNC_EVENT               Last_Event_Disappears;

    EDDI_PRM_CHECK_DROP_BUDGET             DropBudget_A;
    EDDI_PRM_CHECK_DROP_BUDGET             DropBudget_B;

    LSA_UINT32                             StartValue_HOL[EDDI_MAX_IRTE_PORT_CNT];
    LSA_UINT32                             StartValue_DROP[EDD_CFG_MAX_PORT_CNT];
    LSA_UINT32                             DiffAct;

    LSA_UINT16                             SlotNumber;
    LSA_UINT16                             SubSlotNumber;

    LSA_BOOL                               bPDNCChanged;

    LSA_UINT16                             edd_port_id;

} EDDI_PRM_PDNC_DATA_CHECK;

typedef struct _EDDI_PORT_DATA_ADJUST_RECORD_SET_TYPE
{
    LSA_BOOL                                     MulticastBoundaryPresent;
    LSA_BOOL                                     MAUTypePresent;
    LSA_BOOL                                     PortStatePresent;
    LSA_BOOL                                     DomainBoundaryPresent;
    LSA_BOOL                                     DCPBoundaryPresent;

    LSA_UINT8                                    LinkStat;
    LSA_UINT16                                   SlotNumber;
    LSA_UINT16                                   SubSlotNumber;
    LSA_UINT32                                   MulticastBoundary;
    LSA_UINT16                                   MulticastBoundaryAP;
    LSA_UINT32                                   DomainBoundaryIngress;
    LSA_UINT32                                   DomainBoundaryEgress;
    LSA_UINT16                                   DomainBoundaryAP;
    LSA_UINT32                                   DCPBoundary;
    LSA_UINT16                                   DCPBoundaryAP;

    LSA_UINT16                                   MAUType;
    LSA_UINT16                                   MAUTypeAP;
    LSA_UINT16                                   PortState;
    LSA_UINT16                                   PortStateAP;

    EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE  PDPortDataAdjust_Max_Record_Net;
    LSA_UINT32                                   record_data_length;

} EDDI_PORT_DATA_ADJUST_RECORD_SET_TYPE;

typedef struct _EDDI_PORT_DATA_ADJUST_RECORD_SET_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_PORT_DATA_ADJUST_RECORD_SET_PTR_TYPE;

typedef struct _EDDI_PRM_PDPORT_DATA_ADJUST
{
    EDDI_PORT_DATA_ADJUST_RECORD_SET_TYPE RecordSet_A[EDD_CFG_MAX_PORT_CNT]; //user view
    EDDI_PORT_DATA_ADJUST_RECORD_SET_TYPE RecordSet_B[EDD_CFG_MAX_PORT_CNT]; //user view

    EDDI_PRM_STATE                        RecordState_A[EDD_CFG_MAX_PORT_CNT]; //user view
    EDDI_PRM_STATE                        RecordState_B[EDD_CFG_MAX_PORT_CNT]; //user view

} EDDI_PRM_PDPORT_DATA_ADJUST;

typedef struct _EDDI_PRM_PDPORT_DATA_ADJUST EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_PDPORT_MRP_DATA_ADJUST_RECORD_PTR_TYPE;

typedef struct _EDDI_IRT_META_DATA_TYPE
{
    LSA_UINT32                            IrtPortUsrTxCnt[EDD_CFG_MAX_PORT_CNT];    //Nr of providers+forwarders per port
    LSA_UINT32                            IrtAllPortUsrTxCnt;                       //Nr of providers+forwarders of all ports
    
    LSA_UINT32                            IrtPortUsrRxCnt[EDD_CFG_MAX_PORT_CNT];    //Nr of consumers per port
    LSA_UINT32                            IrtAllPortUsrRxCnt;                       //Nr of providers+forwarders of all ports
 
    LSA_UINT32                            IrtPortSyncTxCnt[EDDI_MAX_IRTE_PORT_CNT]; //Counts available SyncFrames for each IRT-Send-Port
    LSA_UINT32                            IrtPortSyncRxCnt[EDDI_MAX_IRTE_PORT_CNT]; //Counts available SyncFrames for each IRT-Receive-Port

    LSA_UINT32                            GlobalTxAcwStartTimeMac10Ns; //StartTime of global Root-ACW (over all ports); Time is already corrected!
    LSA_UINT32                            GlobalTxAcwEndTimeMac10Ns;   //EndTime   of global ACW-EOL  (over all ports); Time is already corrected!

    LSA_UINT32                            n_IrtSyncSndCnt;      //Number of EDDI_SYNC_IRT_PROVIDER for SYNC
    LSA_UINT32                            n_IrtProviderFcwCnt;  //Number of EDDI_SYNC_IRT_PROVIDER without SYNC
    LSA_UINT32                            n_IrtConsumerFcwCnt;  //Number of EDDI_SYNC_IRT_CONSUMER or EDDI_SYNC_IRT_FWD_CONSUMER
    LSA_UINT32                            n_IrtForwarderFcwCnt; //Number of EDDI_SYNC_IRT_FORWARDER

} EDDI_IRT_META_DATA_TYPE;

/* --------------------------------------------------------------------------*/
/* Management for PDIRBeginEndData                                           */
/*                                                                           */
/* For every cylc phase (16) a Begin/End time is defined. The value is       */
/* referenced by the Assignement. So more than one Phase may refere to the   */
/* same Assignment.                                                          */
/*                                                                           */
/* --------------------------------------------------------------------------*/

#define EDDI_IRT_MAX_BEGIN_END_ASSIGNMENTS  5  /* maximum number of Assignments supported by EDD */
#define EDDI_IRT_MAX_PHASE_ASSIGNMENTS      16 /* value according to standard (IEEE)             */
#define EDDI_IRT_MAX_PERIOD_TIME            0x3D08FF

typedef struct _EDDI_IRT_BEGIN_END_TYPE
{
    /* from record */
    LSA_UINT32  RedOrangePeriodBegin;  /* period time in ns. 0 = not present */
    LSA_UINT32  OrangePeriodBegin;     /* period time in ns. 0 = not present */
    LSA_UINT32  GreenPeriodBegin;      /* period time in ns. 0 = not present */

} EDDI_IRT_BEGIN_END_TYPE;

typedef struct _EDDI_IRT_TXRX_BEGIN_END_TYPE
{
    EDDI_IRT_BEGIN_END_TYPE             Tx;
    EDDI_IRT_BEGIN_END_TYPE             Rx;

} EDDI_IRT_TXRX_BEGIN_END_TYPE;

#define EDDI_IRT_BEGIN_END_OFFSET_Tx_RedOrangePeriodBegin               0
#define EDDI_IRT_BEGIN_END_OFFSET_Tx_OrangePeriodBegin                  4  
#define EDDI_IRT_BEGIN_END_OFFSET_Tx_GreenPeriodBegin                   8
#define EDDI_IRT_BEGIN_END_OFFSET_Rx_RedOrangePeriodBegin              12
#define EDDI_IRT_BEGIN_END_OFFSET_Rx_OrangePeriodBegin                 16
#define EDDI_IRT_BEGIN_END_OFFSET_Rx_GreenPeriodBegin                  20

typedef struct _EDDI_IRT_PHASE_ASSIGNMENT_TYPE
{
    /* from PRM-Record */
    LSA_UINT8                          RedOrangePeriodBeginAssignment;  /* 0..(NumberOfAssignments-1) */
    LSA_UINT8                          OrangePeriodBeginAssignment;     /* 0..(NumberOfAssignments-1) */
    LSA_UINT8                          GreenPeriodBeginAssignment;      /* 0..(NumberOfAssignments-1) */

    LSA_UINT32                         Type; /* Type of Phase. derived data from validation.  */

    /* Each Phase Begin/End is part of a "Group" which have the same Begin/End values         */
    /* 0    : no group is assigned. the timevalue is 0 (implizit 0).                          */
    /* 1..xx: If a Group is assigned. Time is hold within "Groups". 1 => index 0 in "Groups". */

    /* -------------------------------------------------------------------------------------- */
    /* Only valid with Type = EDDI_IRT_PHASE_TYPE_RED !!                                      */
    /* -------------------------------------------------------------------------------------- */
    LSA_UINT32                         RedOrangePeriodBeginGroup;  /* 0, 1..EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT  */
    LSA_UINT32                         RedPeriodEndGroup;          /* 1..EDDI_IRT_MAX_RED_ORANGE_END_CNT       */

    /* -------------------------------------------------------------------------------------- */
    /* Only valid with Type = EDDI_IRT_PHASE_TYPE_ORANGE !!                                   */
    /* and with tx! (we have no orange with rx!)                                              */
    /* -------------------------------------------------------------------------------------- */
    LSA_UINT32                         OrangePeriodEndGroup;    /* 1..EDDI_IRT_MAX_ORANGE_END_CNT    */

} EDDI_IRT_PHASE_ASSIGNMENT_TYPE;

/* Type: (note: Rx never has a ORANGE Phase!) */
#define EDDI_IRT_PHASE_TYPE_NONE           0  /* no reserved period is defined    */
#define EDDI_IRT_PHASE_TYPE_ORANGE         1  /* ORANGE period is defined         */
#define EDDI_IRT_PHASE_TYPE_RED            2  /* RED period is defined            */

typedef struct _EDDI_IRT_TXRX_PHASE_ASSIGNMENT_TYPE
{
    EDDI_IRT_PHASE_ASSIGNMENT_TYPE      Tx;
    EDDI_IRT_PHASE_ASSIGNMENT_TYPE      Rx;

} EDDI_IRT_TXRX_PHASE_ASSIGNMENT_TYPE;

#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_RedOrangePeriodBeginAssignment               1
#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_OrangePeriodBeginAssignment                  1  
#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Tx_GreenPeriodBeginAssignment                   0
#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_RedOrangePeriodBeginAssignment               3
#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_OrangePeriodBeginAssignment                  3
#define EDDI_IRT_TXRX_PHASE_ASSIGNMENT_OFFSET_Rx_GreenPeriodBeginAssignment                   2


/* HW-Limitation of ERTEC:                                                          */
/* We only support a number of different Begin/End values over all Phases           */
/* per Port and rx/tx direction.                                                    */
/* These values will be collected from the Assignments and stored within the Groups */
/* Each Phase Begin/End belongs to one GroupEntry, or none if Time = 0              */

#define  EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT  1                      /* only 1 (inkl 0)     Starttime supported */
#define  EDDI_IRT_MAX_RED_ORANGE_END_CNT    EDDI_FCW_TX_EOL_CNT    /* EDDI_FCW_TX_EOL_CNT Endtimes  supported */
#define  EDDI_IRT_MAX_ORANGE_END_CNT        1                      /* only 1              Endtime   supported */
/*                    for all Ports!       */

typedef struct _EDDI_IRT_TXRX_GROUP_TYPE
{                                                                    /* Begin/End Times of each Group in ns  */
    /* ATTENTION: Each Array only contains  */
    /* different values and no 0!           */
    /* Tx: */
    LSA_UINT32         TxRedOrangePeriodBegin[EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT];   /* RedOrangePeriodBegin     from Assignment.  */
    LSA_UINT32         TxRedOrangePeriodEnd[EDDI_IRT_MAX_RED_ORANGE_END_CNT];       /* RedPeriodEnd             from Assignment.  */
    LSA_UINT32         TxOrangePeriodEnd[EDDI_IRT_MAX_ORANGE_END_CNT];              /* OrangePeriodEnd          from Assignment.  */
    /* Rx: Never a ORANGE phase present!    */
    LSA_UINT32         RxRedOrangePeriodBegin[EDDI_IRT_MAX_RED_ORANGE_BEGIN_CNT];   /* RedOrangePeriodBegin     from Assignment.  */
    LSA_UINT32         RxRedOrangePeriodEnd[EDDI_IRT_MAX_RED_ORANGE_END_CNT];       /* RedPeriodEnd             from Assignment.  */

    LSA_UINT32         TxRedOrangePeriodBeginGroupCnt;                              /* number of valid entries in TxRedOrangePeriodBegin[]  */
    LSA_UINT32         TxRedPeriodEndGroupCnt;                                      /* number of valid entries in TxRedOrangePeriodEnd[]    */
    LSA_UINT32         TxOrangePeriodEndGroupCnt;                                   /* number of valid entries in TxOrangePeriodEnd[] */

    LSA_UINT32         RxRedOrangePeriodBeginGroupCnt;                              /* number of valid entries in RxRedOrangePeriodBegin[]  */
    LSA_UINT32         RxRedPeriodEndGroupCnt;                                      /* number of valid entries in RxRedOrangePeriodEnd[]    */

    LSA_UINT32         TxRedPeriodEndMax;                                           /* max value in TxRedOrangePeriodEnd[]                  */
    LSA_UINT32         RxRedPeriodEndMax;                                           /* max value in RxRedOrangePeriodEnd[]                  */
    LSA_UINT32         TxRedPeriodEndMaxIndex;                                      /* index of max value in TxRedOrangePeriodEnd[]         */

    LSA_UINT32         TxRedOrangePeriodBeginMin;                                   /* min value of TxRedOrangePeriodBegin (incl. 0)        */
    LSA_UINT32         RxRedOrangePeriodBeginMin;                                   /* min value in RxRedOrangePeriodBegin (incl. 0)        */

    LSA_BOOL           TxRedOrangePeriodBegin0Present;                              /* is TxRedOrangePeriodBegin Time 0 present for Port?   */
    LSA_BOOL           RxRedOrangePeriodBegin0Present;                              /* is RxRedOrangePeriodBegin Time 0 present for Port?   */

} EDDI_IRT_TXRX_GROUP_TYPE;

typedef struct _EDDI_IRT_BEGIN_END_DATA_TYPE
{
    LSA_BOOL                            bPresent;               //Assignments present?
    LSA_BOOL                            bAtLeastRedPresent;     //Assignments red phase present?
    LSA_BOOL                            bAtLeastOrangePresent;  //Assignments orange phase present?

    LSA_UINT32                          NumberOfAssignments;    //Number of present Assignments 1..EDDI_IRT_MAX_BEGIN_END_ASSIGNMENTS
    EDDI_IRT_TXRX_BEGIN_END_TYPE        Assignment[EDDI_IRT_MAX_BEGIN_END_ASSIGNMENTS];
    LSA_UINT32                          NumberOfPhases;         //Number of present PhaseAssignment 1..EDDI_IRT_MAX_PHASE_ASSIGNMENTS
    EDDI_IRT_TXRX_PHASE_ASSIGNMENT_TYPE PhaseAssignment[EDDI_IRT_MAX_PHASE_ASSIGNMENTS];
    EDDI_IRT_TXRX_GROUP_TYPE            Groups;                 //Filled within Validation. Metadata
    LSA_UINT32                          EndLocalNsRx;           //Starttime of the last local frame received
    LSA_UINT32                          EndLocalNsTx;           //Starttime of the last local frame sent
    LSA_UINT32                          LastLocalFrameLenRx;    //Length of the last local frame received
    LSA_UINT32                          LastLocalFrameLenTx;    //Length of the last local frame sent

} EDDI_IRT_BEGIN_END_DATA_TYPE;

typedef struct _EDDI_IRT_RED_GUARD_TYPE
{
    LSA_UINT16                          StartOfRedFrameID;
    LSA_UINT16                          EndOfRedFrameID;

} EDDI_IRT_RED_GUARD_TYPE;

#define EDDI_IRT_RED_GUARD_MIN_RED_FRAME_ID  0x0080
#define EDDI_IRT_RED_GUARD_MAX_RED_FRAME_ID  0x7FFF

/* --------------------------------------------------------------------------*/
/* FragmentationMode                                                         */
/* --------------------------------------------------------------------------*/
typedef struct _EDDI_PRM_NRT_FRAG
{
    LSA_UINT32                          FragSize;               //0, 128, 256 (0 = TX fragmentation disabled)
    LSA_UINT32                          FragUsrPortIndex;       //only valid with TX fragmentation enabled!
    LSA_UINT32                          FragHwPortIndex;        //only valid with TX fragmentation enabled!
    LSA_UINT32                          ActivePortCnt;          //always valid!

} EDDI_PRM_NRT_FRAG;

/* --------------------------------------------------------------------------*/
/* EDDI_PRM_PDIR_DATA                                                        */
/* --------------------------------------------------------------------------*/

typedef struct _EDDI_PRM_RECORD_IRT_TYPE
{
    LSA_BOOL                                        bGlobalDataExtended;
    LSA_UINT32                                      PortCnt_Extended;

    EDDI_PRM_RECORD_PDIR_DATA_EXT_PTR_TYPE          pNetWorkPDIRDataRecord;
    EDDI_PRM_RECORD_PDIR_DATA_EXT_PTR_TYPE          pLocalPDIRDataRecord; // Note: PDIRBeginEnd is not converted to local!
    
    LSA_UINT32                                      PDIRDataRecordActLen;

    EDDI_IRT_META_DATA_TYPE                         MetaData;

    LSA_BOOL                                        bBeginEndDataPresent; // LSA_TRUE: PDIRBeginEndData Record/Data present

    EDDI_IRT_RED_GUARD_TYPE                         RedGuard;             // RedGuard from PDIRBeginEndData Record

    EDDI_IRT_BEGIN_END_DATA_TYPE                    BeginEndData[EDD_CFG_MAX_PORT_CNT]; //Index = UsrPortIndex

    LSA_UINT32                                      BeginEndIRTType;      // Total IRT Type over all Ports if bBeginEndDataPresent
                                                                          // EDDI_IRT_PHASE_TYPE_ORANGE or EDDI_IRT_PHASE_TYPE_RED
                                                                          // EDDI_IRT_PHASE_TYPE_NONE

    EDDI_QUEUE_TYPE                                 FrameHandlerQueue;    // Links all used FrameHandler-Elements.
                                                                          // A single FrameHandler-Element describes all
                                                                          // Parameters and all FCWs related to a IRTFrame.

    LSA_UINT32                                      hFrameHandler;        // Contains Ressources for all FramesHandlers

    // LSA_UINT32                                   PLLWindow10ns;        // Needed to calculate MetaDatas (MinTime and MaxTimes)
    //                                                                    // Value of PLLWindow10ns will be received via related sync record.

    EDDI_PRM_NRT_FRAG                               NrtFrag;

    LSA_UINT32                                      NumberOfFrameDataElements;
} EDDI_PRM_RECORD_IRT_TYPE;

typedef struct _EDDI_PRM_RECORD_IRT_TYPE EDDI_LOCAL_MEM_ATTR        *       EDDI_PRM_RECORD_IRT_PTR_TYPE;
typedef struct _EDDI_PRM_RECORD_IRT_TYPE EDDI_LOCAL_MEM_ATTR const  * EDDI_CONST_PRM_RECORD_IRT_PTR_TYPE;



/* --------------------------------------------------------------------------*/
/* EDDI_PRM_PDIR_SUBFRAME_DATA                                               */
/* --------------------------------------------------------------------------*/
#if defined (EDDI_CFG_DFP_ON)

typedef struct _EDDI_PRM_RECORD_IRT_SUBFRAME_TYPE
{

    EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE     pNetWorkPDirSubframeDataRecord;
    EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE     pLocalPDirSubframeDataRecord;

    LSA_UINT32                                      PDirSubFrameDataRecordActLen;

    struct _EDDI_PRM_PF_DSCR_TYPE                 * pFirstPackFrame;                  // 1st pointer of the PackFrm 
    
    LSA_UINT32                                      hPackFrameHandler;                // Contains Ressources for all Subframe FramesHandlers
    LSA_UINT32                                      hSubFrameHandler;                 // Contains Ressources for all Subframe FramesHandlers

} EDDI_PRM_RECORD_IRT_SUBFRAME_TYPE;

typedef struct _EDDI_PRM_RECORD_IRT_SUBFRAME_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE;

#endif

/* --------------------------------------------------------------------------*/
/* Management for IRData Validation. Used on PRM to validate the parameters  */
/* used to build up a phase tree with sorted frame elemtents to verify start */
/* and end-times of frames for overlapping and borders.                      */
/* --------------------------------------------------------------------------*/

#define PRM_RT_CLASS3_GAP_NS      1120 /* The minimal GAP between two RT_CLASS3 Frames is 1120ns */

typedef struct _EDDI_PRM_IRT_NODE_LIST_TYPE
{
    EDDI_QUEUE_TYPE                         Queue;            /* Queue of EDDI_PRM_IRT_ELEM_TYPE   */
    LSA_UINT32                              StartTimeNs;      /* Starttime of List in ns          */
    LSA_UINT32                              EndTimeNsWithGap; /* Endime    of List in ns with Gap */

} EDDI_PRM_IRT_NODE_LIST_TYPE;

typedef struct _EDDI_PRM_IRT_NODE_LIST_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_IRT_NODE_LIST_PTR_TYPE;

typedef struct _EDDI_PRM_IRT_NODE_TYPE
{
    EDDI_PRM_IRT_NODE_LIST_TYPE             Tx; /* Tx-List */
    EDDI_PRM_IRT_NODE_LIST_TYPE             Rx; /* Rx-List */

} EDDI_PRM_IRT_NODE_TYPE;

typedef struct _EDDI_PRM_IRT_ELEM_TYPE
{
    EDDI_QUEUE_ELEMENT_TYPE                 QueueLink;       /* Queuing of Elements. MUST BE FIRST ELEMENT ! */
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     pFrmData;        /* related FrameData      */
    LSA_UINT32                              StartTimeNs;     /* StartTime in ns        */
    LSA_UINT32                              EndTimeNsWithGap;/* EndTime in ns with Gap */

} EDDI_PRM_IRT_ELEM_TYPE;

typedef struct _EDDI_PRM_IRT_ELEM_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_IRT_ELEM_PTR_TYPE;

#define EDDI_PRM_RECORD_IRT_VAL_NODE_CNT             ((EDD_CSRT_CYCLE_REDUCTION_RATIO_RTC3_MAX * 2)-1)

typedef struct _EDDI_PRM_RECORD_IRT_VALIDATA_TYPE
{
    /* Array of Node-Elements. Size depends */
    /* of Max-supported reduction ratio     */
    /* The array holds the list of elements */
    /* per reduction and phase              */
    /* Index: ((Reduction-1) + (Phase-1))   */
    EDDI_PRM_IRT_NODE_TYPE                  NodeArray[EDD_CFG_MAX_PORT_CNT][EDDI_PRM_RECORD_IRT_VAL_NODE_CNT];

    /* Array of Tree-Elements used to queue */
    /* within NodeArray.                    */
    EDDI_PRM_IRT_ELEM_TYPE                  ElemArray[EDDI_IRT_MAX_TREE_ELEMENTS];
    LSA_UINT32                              ElemFreeCnt;  /* Number of free Elements */
    LSA_UINT32                              ElemUsedCnt;  /* Number of used Elements */

} EDDI_PRM_RECORD_IRT_VALIDATA_TYPE;

typedef EDDI_PRM_RECORD_IRT_VALIDATA_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE;

typedef struct _EDDI_PRM_PDIR_DATA
{
    EDDI_PRM_RECORD_IRT_TYPE                RecordSet_A;
    EDDI_PRM_RECORD_IRT_TYPE                RecordSet_B;

    EDDI_PRM_RECORD_IRT_PTR_TYPE            pRecordSet_A;
    EDDI_PRM_RECORD_IRT_PTR_TYPE            pRecordSet_B;

    EDDI_PRM_STATE                          State_A;
    EDDI_PRM_STATE                          State_B;

    LSA_UINT16                              edd_port_id;

    EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE   pValidate;  /* For IRData validation */

} EDDI_PRM_PDIR_DATA;

typedef struct _EDDI_PRM_PDIR_DATA EDDI_LOCAL_MEM_ATTR * EDDI_PRM_PDIR_DATA_PTR_TYPE;


#if defined (EDDI_CFG_DFP_ON)
/* --------------------------------------------------------------------------*/
/* EDDI_PRM_PDSYNC_DATA                                                        */
/* --------------------------------------------------------------------------*/
typedef struct _EDDI_PRM_PDIR_SUBFRAME_DATA
{
    EDDI_PRM_RECORD_IRT_SUBFRAME_TYPE       RecordSet_A;
    EDDI_PRM_RECORD_IRT_SUBFRAME_TYPE       RecordSet_B;

    EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE   pRecordSet_A;
    EDDI_PRM_RECORD_IRT_SUBFRAME_PTR_TYPE   pRecordSet_B;

    EDDI_PRM_STATE                          State_A;
    EDDI_PRM_STATE                          State_B;

    LSA_UINT16                              edd_port_id;

    LSA_UINT16                              SlotNumber;
    LSA_UINT16                              SubSlotNumber;

    EDDI_PRM_RECORD_IRT_VALIDATA_PTR_TYPE   pValidate;  /* For IRData validation */

	LSA_UINT8*								pPDIRSubframeDataTemp; /* temporary storage for PDIRSubframeData record */
	LSA_UINT8*								pPDIRSubframeDataNetworkFormatTemp; /* temporary storage for PDIRSubframeData record (network format) */
} EDDI_PRM_PDIR_SUBFRAME_DATA;

typedef struct _EDDI_PRM_PDIR_SUBFRAME_DATA EDDI_LOCAL_MEM_ATTR * EDDI_PRM_PDIR_SUBFRAME_DATA_PTR_TYPE;
#endif

/* --------------------------------------------------------------------------*/
/* EDDI_PRM_PDSYNC_DATA                                                      */
/* --------------------------------------------------------------------------*/

typedef struct _EDDI_PRM_RECORD_SYNC_TYPE
{
    LSA_UINT32                            PaddingCount;
    LSA_UINT32                            PDSyncDataLength_All;
    LSA_UINT32                            PDSyncDataLength_PTCPSubdomainNameLength;

    EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  pNetWorkPDSyncDataRecord;
    EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE  pLocalPDSyncDataRecord;

} EDDI_PRM_RECORD_SYNC_TYPE;

typedef struct _EDDI_PRM_RECORD_SYNC_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_RECORD_SYNC_PTR_TYPE;

typedef struct _EDDI_PRM_PDSYNC_DATA
{
    EDDI_PRM_RECORD_SYNC_TYPE         RecordSet_A;
    EDDI_PRM_RECORD_SYNC_TYPE         RecordSet_B;

    EDDI_PRM_RECORD_SYNC_PTR_TYPE     pRecordSet_A;
    EDDI_PRM_RECORD_SYNC_PTR_TYPE     pRecordSet_B;

    //PRM_STATE_DIAG_PDSYNC_FCT        DiagStateFct;   PK: RTSync

    EDDI_PRM_STATE                    State_A;
    EDDI_PRM_STATE                    State_B;

    //EDDI_PRM_DIAG_PDSYNC_EVENT        Last_Event_Appears;    PK: RTSync
    //EDDI_PRM_DIAG_PDSYNC_EVENT        Last_Event_Disappears;   PK: RTSync

    //EDDI_PRM_DIAG_PDSYNC_EVENT        Appear_Event_Execute;   PK: RTSync

    LSA_BOOL                          bPDSyncChanged;

    LSA_UINT16                        SlotNumber;
    LSA_UINT16                        SubSlotNumber;

    LSA_UINT16                        edd_port_id;

} EDDI_PRM_PDSYNC_DATA;

#if defined (EDDI_RED_PHASE_SHIFT_ON)
typedef struct _EDDI_PRM_TRANSITION_TYPE
{
    LSA_BOOL                     bTransitionRunning;    //Set to LSA_TRUE if a PRM_COMMIT-Transition is running

    // all new Requests are queues in RQB-Queue
    // Request are stored only if a State-Transition is running
    EDDI_RQB_QUEUE_TYPE          RqbQueue;              //Queue to store pending Requests
    EDD_UPPER_RQB_PTR_TYPE       pRunningRQB;           //Running Request of current Transition;

} EDDI_PRM_TRANSITION_TYPE;

#endif

/* --------------------------------------------------------------------------*/
/* PDPortMrpDataAjust                                                        */
/* --------------------------------------------------------------------------*/
/* Note: We only support WRITE ! and only check for Record to be written (no validation!) */

typedef struct _EDDI_PRM_PDPORT_MRP_DATA_ADJUST
{
    EDDI_PRM_STATE    RecordState_A[EDD_CFG_MAX_PORT_CNT]; // user view
    EDDI_PRM_STATE    RecordState_B[EDD_CFG_MAX_PORT_CNT]; // user view
    LSA_BOOL          AtLeastTwoRecordsPresentB;           // TRUE if min. 2 records are present
    /* Record set B on END. helper var  */

} EDDI_PRM_PDPORT_MRP_DATA_ADJUST;

/* --------------------------------------------------------------------------*/
/* PDInterfaceMrpDataAdjust                                                  */
/* --------------------------------------------------------------------------*/
typedef struct _EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST
{
    EDDI_PRM_STATE                        RecordState_A; //user view
    EDDI_PRM_STATE                        RecordState_B; //user view

} EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST;

typedef struct _EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_RECORD_PTR_TYPE;

/* --------------------------------------------------------------------------*/
/* PDSetDefaultPortStates                                                    */
/* --------------------------------------------------------------------------*/
/* Note: We only support WRITE. This is a privat EDD record!                 */

typedef struct _EDDI_PRM_RECORD_SET_PORT_STATES_TYPE
{
    LSA_BOOL                             PortStatePresent;  /* is PortState present? */
    LSA_UINT8                            PortState;

} EDDI_PRM_RECORD_SET_PORT_STATES_TYPE;

/* PortState : */
/* #define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_DISABLE   0x01 */
/* #define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_BLOCKING  0x02 */

typedef struct _EDDI_PRM_PDSET_DEFAULT_PORT_STATES
{
    EDDI_PRM_STATE                        RecordState_A; //user view
    EDDI_PRM_STATE                        RecordState_B; //user view
    EDDI_PRM_RECORD_SET_PORT_STATES_TYPE  RecordSet_A[EDD_CFG_MAX_PORT_CNT];
    EDDI_PRM_RECORD_SET_PORT_STATES_TYPE  RecordSet_B[EDD_CFG_MAX_PORT_CNT];

} EDDI_PRM_PDSET_DEFAULT_PORT_STATES;
/* --------------------------------------------------------------------------*/

/* --------------------------------------------------------------------------*/
/* PDIRApplicationData                                                       */
/* --------------------------------------------------------------------------*/
typedef struct _EDDI_PRM_PDIR_APPLICATION_DATA_TYPE
{
    LSA_UINT16                           ConsumerCnt;  
    LSA_UINT16                           ConsumerIODataLength;
    LSA_UINT16                           Reserved;
    EDD_RESERVED_INTERVAL_IF_RED_TYPE    TransferEndValues;
    LSA_BOOL                             bCalculatedTransferEndValues;
} EDDI_PRM_PDIR_APPLICATION_DATA_TYPE;

typedef struct _EDDI_PRM_PDIR_APPLICATION_DATA_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_PDIR_APPLICATION_DATA_PTR_TYPE;

typedef struct _EDDI_PRM_PDIR_APPLICATION_DATA
{
    EDDI_PRM_STATE                          State_A; //user view
    EDDI_PRM_STATE                          State_B; //user view
    EDDI_PRM_PDIR_APPLICATION_DATA_TYPE     RecordSet_A;
    EDDI_PRM_PDIR_APPLICATION_DATA_TYPE     RecordSet_B;

    EDDI_PRM_PDIR_APPLICATION_DATA_PTR_TYPE pRecordSet_A;
    EDDI_PRM_PDIR_APPLICATION_DATA_PTR_TYPE pRecordSet_B;

} EDDI_PRM_PDIR_APPLICATION_DATA;

/* --------------------------------------------------------------------------*/
/* PDNRT_LOAD_LIMITATION                                                     */
/* --------------------------------------------------------------------------*/
typedef struct _EDDI_PRM_PDNRT_LOAD_LIMITATION_TYPE
{
    LSA_BOOL                                           LoadLimitationActive;
    LSA_BOOL                                           IO_Configuration;
    EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE  RecordSet;
} EDDI_PRM_PDNRT_LOAD_LIMITATION_TYPE;

typedef struct _EDDI_PRM_PDNRT_LOAD_LIMITATION_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_PRM_PDNRT_LOAD_LIMITATION_PTR_TYPE;


typedef struct _EDDI_PRM_PDNRT_LOAD_LIMITATION
{
    EDDI_PRM_STATE                          State_A; //user view
    EDDI_PRM_STATE                          State_B; //user view

    EDDI_PRM_PDNRT_LOAD_LIMITATION_TYPE     RecordSet_A;
    EDDI_PRM_PDNRT_LOAD_LIMITATION_TYPE     RecordSet_B;

    EDDI_PRM_PDNRT_LOAD_LIMITATION_PTR_TYPE pRecordSet_A;
    EDDI_PRM_PDNRT_LOAD_LIMITATION_PTR_TYPE pRecordSet_B;

} EDDI_PRM_PDNRT_LOAD_LIMITATION;
/* --------------------------------------------------------------------------*/


typedef struct _EDDI_PRM_DETAIL_ERR
{
    LSA_BOOL        ErrValid;
    LSA_UINT8       ErrFault;
    LSA_UINT16      ErrPortID;
    LSA_UINT32      ErrIndex;
    LSA_UINT32      ErrOffset;
 
    /* special offsets for consistency checks */
    LSA_UINT32      EDDI_PRM_ADJUST_PORT_STATE_Offset[EDD_CFG_MAX_PORT_CNT];
//    LSA_UINT32      EDDI_PRM_PDSYNC_DATA_Record_Offset;
    LSA_UINT32      EDDI_PRM_RECORD_PDSYNC_DATA_ReservedIntervalEnd_OFFSET;
    LSA_UINT32      EDDI_PRM_RECORD_PDSYNC_DATA_PTCP_MasterPriority1_OFFSET;
    LSA_UINT32      EDDI_PRM_RECORD_PDSYNC_DATA_SyncProperties_OFFSET;

} EDDI_PRM_DETAIL_ERR;

typedef struct _EDDI_PRM_DETAIL_ERR         EDDI_LOCAL_MEM_ATTR *    EDDI_PRM_DETAIL_ERR_PTR_TYPE;

/* --------------------------------------------------------------------------*/


typedef struct _EDDI_PRM_TYPE
{
    PRM_STATE_FCT                           StateFct;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_PRM_TRANSITION_TYPE                Transition;
    #endif

    LSA_BOOL                                bFirstCommitExecuted;// set to TRUE on first PRM-COMMIT. False on Startup.
    LSA_BOOL                                bSyncIrDataChanged;  // Needed during PRM_COMMIT-Transition: LSA_TRUE if Change in IRDATA or PDSYNCDATA occured.

    PRM_STATE_DIAG_PDNC_FCT                 PDNCStateDiagFctTable[0x5];
    //PRM_STATE_DIAG_PDSYNC_FCT             PDSYNC_StateDiagFctTable[0x7];  PK: RTSync

    EDDI_PRM_PDNC_DATA_CHECK                PDNCDataCheck;
    EDDI_PRM_PDSYNC_DATA                    PDSyncData;
    EDDI_PRM_PDIR_DATA                      PDIRData;
    #if defined (EDDI_CFG_DFP_ON)
    EDDI_PRM_PDIR_SUBFRAME_DATA             PDIRSubFrameData;
    #endif
    
    EDDI_PRM_PDPORT_DATA_ADJUST             PDPortDataAdjust;
    EDDI_PRM_PDSCF_DATA_CHECK               PDSCFDataCheck;
    EDDI_PRM_PDTRACE_UNIT_CONTROL           PDTraceUnitControl;
    EDDI_PRM_PDCONTROL_PLL                  PDControlPLL;
    EDDI_PRM_PDPORT_MRP_DATA_ADJUST         PDPortMrpDataAdjust;
    EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST    PDInterfaceMrpDataAdjust;
    EDDI_PRM_PDSET_DEFAULT_PORT_STATES      PDSetDefaultPortStates;
    EDDI_PRM_PDIR_APPLICATION_DATA          PDirApplicationData;
    EDDI_PRM_PDNRT_LOAD_LIMITATION          PDNrtLoadLimitation;

    EDDI_PRM_DETAIL_ERR                     PrmDetailErr;
    /* Change Media Type */
    LSA_UINT8                               PortparamsNotApplicable[EDD_CFG_MAX_PORT_CNT];
    LSA_UINT8                               ApplyDefaultPortparams[EDD_CFG_MAX_PORT_CNT];

    LSA_BOOL                                DoFireIndication_Interface;
    LSA_BOOL                                DoFireIndication_Ports[EDD_CFG_MAX_PORT_CNT];      // only UserPort-View
                                      
    EDDI_RQB_QUEUE_TYPE                     IndQueue_Interface;
    EDDI_RQB_QUEUE_TYPE                     IndQueue_Ports[EDD_CFG_MAX_PORT_CNT];              // only UserPort-View
                                      
    LSA_TIMER_ID_TYPE                       TimerID;
                                      
    LSA_BOOL                                bPendingCommit;         // TRUE if the running PRMRequest is a COMMIT-Request
    EDD_UPPER_RQB_PTR_TYPE                  pRQB_PendingCommit;     // pending IndicationRQB during Commit
                                      
                                      
    LSA_BOOL                                bPendingRessource;
    EDD_UPPER_RQB_PTR_TYPE                  pRQB_PendingRessource;
                                      
    LSA_BOOL                                bCommitExecuted;        /* PRM-COMMIT already executed,
                                                                       will be checked if the user tries to call
                                                                       illegally EDDI_SRV_SWITCH_SET_PORT_SPEEDMODE
                                                                       on a running PNIO-Device after parameterisation. */
                                      
    LSA_UINT32                              HandleCnt;              /* Number of Handles(only 1 allowed) */
                                      
    LSA_BOOL                                LesserCheckQuality;     /* Flag from CDB.                    */
                                      
    EDDI_DDB_COMP_TRACE_TYPE                TRACE;

} EDDI_PRM_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_TYPE_H


/*****************************************************************************/
/*  end of file eddi_prm_type.h                                              */
/*****************************************************************************/

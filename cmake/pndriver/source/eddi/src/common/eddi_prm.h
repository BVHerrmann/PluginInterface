#ifndef EDDI_PRM_H              //reinclude-protection
#define EDDI_PRM_H

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
/*  F i l e               &F: eddi_prm.h                                :F&  */
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
/*  26.03.08    JS    added PDSetDefaultPortStates                           */
/*  06.02.15    TH    refactor AdjustPortState to AdjustLinkState            */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  defines                                                                  */
/*****************************************************************************/

/*======================================================================================*/
/*                                   types                                              */
/*======================================================================================*/


typedef enum EDDI_PORT_MODULE_SM_STATE_TYPE_
{
    EDDI_PORT_MODULE_SM_STATE_PULLED,
    EDDI_PORT_MODULE_SM_STATE_PLUGGING,
    EDDI_PORT_MODULE_SM_STATE_PLUGGED
} EDDI_PORT_MODULE_SM_STATE_TYPE;


typedef enum EDDI_PRM_PORT_MODULE_SM_TRIGGER_TYPE_
{
    EDDI_PRM_PORT_MODULE_SM_TRIGGER_PULL            = 0,
    EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_PREPARE    = 1,
    EDDI_PRM_PORT_MODULE_SM_TRIGGER_PLUG_COMMIT     = 2
} EDDI_PRM_PORT_MODULE_SM_TRIGGER_TYPE;

typedef struct _EDDI_PRM_PORT_MODULE_SM_TYPE
{
    EDDI_PORT_MODULE_SM_STATE_TYPE              State;
} EDDI_PRM_PORT_MODULE_SM_TYPE;

/*****************************************************************************/
/*  record index                                                             */
/*****************************************************************************/

/*** Meaning of index                         value of record  ****/

/************************* norm record *****************************/

/* PDNCDataCheck */
#define EDDI_PRM_INDEX_PDNC_DATA_CHECK              0x8070

/* PDIRData */
#define EDDI_PRM_INDEX_PDIR_DATA                    0x802C

/* PDSyncData (PTCPoverRTC) */
#define EDDI_PRM_INDEX_PDSYNC_DATA                  0x802D

/* PDPortDataAdjust */
#define EDDI_PRM_INDEX_PDPORT_DATA_ADJUST           0x802F

/* PDInterfaceMrpDataAdjust */
#define EDDI_PRM_INDEX_PDINTERFACE_MRP_DATA_ADJUST  0x8052

/* PDPortMrpDataAdjust */
#define EDDI_PRM_INDEX_PDPORT_MRP_DATA_ADJUST       0x8053

/* PDIRSubframeData */
#define EDDI_PRM_INDEX_PDIR_SUBFRAME_DATA           0x8020

/* Reserved (legacy V1.x)*/
#define EDDI_PRM_INDEX_PDIR_RESERVED_LEGACY_V1_X    0x802E

/* PDPortStatistic  */
#define EDDI_PRM_INDEX_PDPORT_STATISTIC             0x8072

/************************* user specific  record 0xB000 - 0xBFFF ***/

/* PDPLLExtData_RTC */
#define EDDI_PRM_INDEX_PDCONTROL_PLL                0xB050

/* PDTraceUnitControl */
#define EDDI_PRM_INDEX_PDTRACEUNIT_CONTROL          0xB060

/* PDTraceUnitData */
#define EDDI_PRM_INDEX_PDTRACE_UNIT_DATA            0xB061


/* PDSCFDataCheck */
#define EDDI_PRM_INDEX_PDSCF_DATA_CHECK             0x10000

/* PDSetDefaultPortStates */
#define EDDI_PRM_INDEX_PDSET_DEFAULT_PORT_STATES    0x10001

/* PDIRApplicationData  */
#define EDDI_PRM_INDEX_PDIR_APPLICATION_DATA        0x10002

/* PDNRTFeedInLoadLimitation */
#define EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION 0x10003

/*****************************************************************************/
/*  typedefs                                                                 */
/*****************************************************************************/

#include "pnio_pck1_on.h"
/*###########################################################################*/
/*  PACKED STRUCTS                                                           */
/*###########################################################################*/

/*****************************************************************************/
/*  Default error values                                                     */
/*****************************************************************************/


#define EDDI_PRM_ERR_INVALID_OFFSET                     0xFFFFFFFF


/*****************************************************************************/
/*  block header                                                             */
/*****************************************************************************/

#define EDDI_PRM_BLOCKHEADER_OFFSET_BlockType           0
#define EDDI_PRM_BLOCKHEADER_OFFSET_BlockLength         2
#define EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionHigh    4
#define EDDI_PRM_BLOCKHEADER_OFFSET_BlockVersionLow     5
#define EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_1          6
#define EDDI_PRM_BLOCKHEADER_OFFSET_Padding1_2          7

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_HEADER_TYPE
{
    /* 00 */
    LSA_UINT16          BlockType;
    /* 02 */
    LSA_UINT16          BlockLength;    /* without Type/Length field */
    /* 04 */
    LSA_UINT8           BlockVersionHigh;
    /* 05 */
    LSA_UINT8           BlockVersionLow;
    /* 06 */
    LSA_UINT8           Padding1_1;
    /* 07 */
    LSA_UINT8           Padding1_2;

    /* 08 Length */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_HEADER_TYPE EDDI_PRM_RECORD_HEADER_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_STATISTIC_RECORD_HEADER_TYPE
{
    /* 00 */
    LSA_UINT16          BlockType;
    /* 02 */
    LSA_UINT16          BlockLength;    /* without Type/Length field */
                                        /* 04 */
    LSA_UINT8           BlockVersionHigh;
    /* 05 */
    LSA_UINT8           BlockVersionLow;
    /* 06 */
    LSA_UINT16          CounterStatus;

    /* 08 Length */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_STATISTIC_RECORD_HEADER_TYPE EDDI_PRM_STATISTIC_RECORD_HEADER_TYPE;

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDNC_DATA_CHECK, record_index:0x8070                      */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_NCDropBudgetType
{
    /* 00 */
    LSA_UINT32          Data;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_NCDropBudgetType EDDI_NCDropBudgetType;

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE   Header;

    /* 08 */
    EDDI_NCDropBudgetType         MaintenanceRequiredDropBudget;
    /* 12 */
    EDDI_NCDropBudgetType         MaintenanceDemandedDropBudget;
    /* 16 */
    EDDI_NCDropBudgetType         ErrorDropBudget;

    /* 20 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE  EDDI_PRM_RECORD_PDNC_DATA_CHECK_TYPE;


#define EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceRequiredDropBudget         8
#define EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_MaintenanceDemandedDropBudget        12
#define EDDI_PRM_RECORD_PDNC_DATA_CHECK_OFFSET_ErrorDropBudget                      16

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDIR_DATA, record_index:0x802C                            */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_MAX_PORT_DELAY
{
    /* 00 */
    LSA_UINT32          Tx;
    /* 04 */
    LSA_UINT32          Rx;
    /* 08 */
    // PDIRGlobalData V1.2
    LSA_UINT32          MaxLineRxDelay;
    /* 12 */
    LSA_UINT32          YellowTime;
    /* 16 */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_MAX_PORT_DELAY EDDI_PRM_RECORD_MAX_PORT_DELAY;

#define EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Tx                            0
#define EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_Rx                            4
#define EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_MaxLineRxDelay                8
#define EDDI_PRM_RECORD_MAX_PORT_DELAY_OFFSET_YellowTime                    12

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE      BlockHeader;
    /* 08 */
    LSA_UINT8                        IRDataID[16];
    /* 24 */
    LSA_UINT32                       MaxBridgeDelay;
    /* 28 */
    LSA_UINT32                       NumberOfPorts;
    /* 32 */
    EDDI_PRM_RECORD_MAX_PORT_DELAY   MaxPortDelay[EDD_CFG_MAX_PORT_CNT]; //has to be able to store the biggest possible record!

    /* 68 Length */

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE;

#define EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_MaxBridgeDelay          24
#define EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSET_NumberOfPorts           28
#define EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETBASE_MaxPortDelay        32
#define EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_OFFSETSIZE_MaxPortDelay        sizeof(EDDI_PRM_RECORD_MAX_PORT_DELAY)


PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_FRAME_DETAILS_TYPE
{
    LSA_UINT8                           Byte;

} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_FRAME_DETAILS_TYPE EDDI_PRM_RECORD_FRAME_DETAILS_TYPE;

//FrameData
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_FRAME_DATA_TYPE
{
    /* 00 */
    LSA_UINT32                         FrameSendOffset;
    /* 04 */
    LSA_UINT16                         DataLength;
    /* 06 */
    LSA_UINT16                         ReductionRatio;
    /* 08 */
    LSA_UINT16                         Phase;
    /* 10 */
    LSA_UINT16                         FrameID;
    /* 12 */
    LSA_UINT16                         Ethertype;
    /* 14 */
    LSA_UINT8                          UsrRxPort_0_4;            //       0: Local Port;
    // 1,2,3,4: Port1..Port4
    /* 15 */
    EDDI_PRM_RECORD_FRAME_DETAILS_TYPE FrameDetails;
    /* 16 */
    LSA_UINT8                          NumberOfTxPortGroups;
    /* 17 */
    LSA_UINT8                          UsrTxPortGroupArray[1];   // For EDD Only 1 allowed
    /* 18 */
    LSA_UINT8                          Reserved[2];

    /* 20 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_FRAME_DATA_TYPE EDDI_PRM_RECORD_FRAME_DATA_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_FRAME_DATA_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE;

#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameSendOffset           0
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_DataLength                4
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_ReductionRatio            6
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_Phase                     8
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameID                  10
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_Ethertype                12
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_UsrRxPort_0_4            14
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameDetails             15
//#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_FrameDetails_???              
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_NumberOfTxPortGroups     16
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_UsrTxPortGroupArray      17
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_Reserved_1               18
#define EDDI_PRM_RECORD_FRAME_DATA_OFFSET_Reserved_2               19


//PDIRFrameData
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE      BlockHeader;
    /* 08 */
    LSA_UINT32                       FrameDataProperties;   //only for BlockVersionLow = 1
    /* 12 + n * 20*/
    EDDI_PRM_RECORD_FRAME_DATA_TYPE  PDIRFrameData[1];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE;

#define EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties                    8
#define EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties_ForwardingMode    11
#define EDDI_PRM_RECORD_PDIR_FRAME_DATA_OFFSET_FrameDataProperties_FragmentationMode 11

//PDIRData_Ext_Max
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE                BlockHeader;
    /* 08 */
    LSA_UINT16                                 SlotNumber;
    /* 10 */
    LSA_UINT16                                 SubslotNumber;
    /* 12 */
    EDDI_PRM_RECORD_PDIR_GLOBAL_DATA_EXT_TYPE  PDIRGlobalDataExt; /* 68 Length */
    /* 80 */
    EDDI_PRM_RECORD_PDIR_FRAME_DATA_TYPE       PDIRFrameData;     /* 08 + n * 20 Length */
    /* xx */  /* after PDIRFrameData comes PDIRBeginEndData! no static structure possible ! */
    /* 88 + n * 20 Length, Max n = 512 + PRDIRBeginEndData */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDIR_DATA_EXT_TYPE  EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDIR_DATA_EXT_PTR_TYPE;

#define EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SlotNumber      8
#define EDDI_PRM_RECORD_PDIR_DATA_EXT_OFFSET_SubslotNumber  10

//PDIRData_Ext_Max
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PD_PORT_STATISTIC
{
    /* 00 */
    EDDI_PRM_STATISTIC_RECORD_HEADER_TYPE  BlockHeader;
    /* 08 */
    LSA_UINT32  ifInOctets;
    /* 12 */
    LSA_UINT32  ifOutOctets;
    /* 16 */
    LSA_UINT32  ifInDiscards;
    /* 20 */
    LSA_UINT32  ifOutDiscards;
    /* 24 */
    LSA_UINT32  ifInErrors;
    /* 28 */
    LSA_UINT32  ifOutErrors;
    /* 32 */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PD_PORT_STATISTIC EDDI_PRM_RECORD_PD_PORT_STATISTIC;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PD_PORT_STATISTIC  EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PD_PORT_STATISTIC_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/*   PDIRBeginEndData                                                        */
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT16          RedGuard.StartOfRedFrameID                    2    */
/*    LSA_UINT16          RedGuard.EndOfRedFrameID                      2    */
/*    LSA_UINT32          NumberOfPorts                                 4    */
/*                                                                           */
/*    [NumberOfPorts]                                                        */
/*      LSA_UINT32        NumberOfAssignments                                */
/*      [NumberOfAssignments]                                                */
/*       {                                                                   */
/*        LSA_UINT32      RedOrangePeriodBegin                               */
/*        LSA_UINT32      OrangePeriodBegin                                  */
/*        LSA_UINT32      GreenPeriodBegin      (1)                          */
/*       } TxBeginEndAssignment                                              */
/*                                                                           */
/*       {                                                                   */
/*        LSA_UINT32      RedOrangePeriodBegin                               */
/*        LSA_UINT32      OrangePeriodBegin                                  */
/*        LSA_UINT32      GreenPeriodBegin      (1)                          */
/*       } RxBeginEndAssignment                                              */
/*      ]                                                                    */
/*                                                                           */
/*      LSA_UINT32        NumberOfPhases                                     */
/*      [NumberOfPhases]                                                     */
/*        LSA_UINT16      TxPhaseAssignment                                  */
/*        LSA_UINT16      RxPhaseAssignment                                  */
/*      ]                                                                    */
/*    ]                                                                      */
/*                                                                           */
/* (1) actually renamed to GreenPeriodBegin whithin IEC spec after           */
/*     implementation                                                        */
/*---------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE          BlockHeader;
    /* 08 */
    LSA_UINT16                           RedGuardStartOfRedFrameID;
    /* 10 */
    LSA_UINT16                           RedGuardEndOfRedFrameID;
    /* 12 */
    LSA_UINT32                           NumberOfPorts;

    /* from now on its no static structure anymore ! */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_TYPE;

#define EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_RedGuardStartOfRedFrameID           8
#define EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_RedGuardEndOfRedFrameID            10
#define EDDI_PRM_RECORD_PDIR_BEGIN_END_DATA_OFFSET_NumberOfPorts                      12

/* Maximum size : 8 + 4 + 4 + EDDI_MAX_IRTE_PORT_CNT * [ 4 + MAXASSIGNMENTS * [12*2] + 4 + MAXPHASES * 4 ] = 1820 */
#define EDDI_PRM_MAX_PDIR_BEGIN_END_DATA_LENGTH   1840

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDSYNC_DATA, record_index:0x8020                          */
/*****************************************************************************/

/*
6.2.10.4  Coding of the field SyncProperties
This field shall be coded according to 3.7.3.4 and the individual bits shall have the following meaning:
Bit 0 - 1: SyncProperties.Role This field shall be coded with the values according to Table 422.

Table 422 - SyncProperties.Role
Value (hexadecimal) Meaning Usage
0x00  Reserved
0x01  External sync Clock or Time Slave
0x02  Internal sync Clock or Time Master
0x03  Reserved
*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_PROPERTIES_TYPE
{
    LSA_UINT16                        Value16;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PROPERTIES_TYPE EDDI_PRM_RECORD_PROPERTIES_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_PDSYNC_DATA_TYPE
{
    /*  00 */
    EDDI_PRM_RECORD_HEADER_TYPE     BlockHeader;
    /*  08 */
    LSA_UINT8                       PTPSubDomainID[16];
    /*  24 */
    LSA_UINT32                      ReservedIntervalBegin;
    /*  28 */
    LSA_UINT32                      ReservedIntervalEnd;
    /*  32 */
    LSA_UINT32                      PLLWindow;
    /*  36 */
    LSA_UINT32                      SyncSendFactor;
    /*  40 */
    LSA_UINT16                      SendClockFactor;
    /*  42 */
    LSA_UINT16                      PTCPTimeoutFactor; // recognition timeout for missing sync frames
    /*  44 */
    LSA_UINT16                      PTCPTakeoverTimeoutFactor;
    /*  46 */
    LSA_UINT16                      PTCPMasterStartupTime;
    /*  48 */
    EDDI_PRM_RECORD_PROPERTIES_TYPE SyncProperties;
    /*  50 */
    LSA_UINT8                       PTCP_MasterPriority1;
    /*  51 */                                                        
    LSA_UINT8                       PTCP_MasterPriority2;
    /*  52 */
    LSA_UINT8                       PTCPLengthSubdomainName;
    /*  53 */
    LSA_UINT8                       PTCPSubdomainName[240];
    /* 293 */
    LSA_UINT8                       Padding_1[3];

    /* 296 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDSYNC_DATA_TYPE EDDI_PRM_RECORD_PDSYNC_DATA_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_PDSYNC_DATA_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDSYNC_DATA_PTR_TYPE;


#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTPSubDomainID                    8
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_ReservedIntervalBegin            24
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_ReservedIntervalEnd              28
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PLLWindow                        32
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncSendFactor                   36
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SendClockFactor                  40
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPTimeoutFactor                42
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPTakeoverTimeoutFactor        44
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPMasterStartupTime            46
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_SyncProperties                   48
//#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_       
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCP_MasterPriority1             50
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCP_MasterPriority2             51
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPLengthSubdomainName          52
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_PTCPSubdomainName                53
#define EDDI_PRM_RECORD_PDSYNC_DATA_OFFSET_Padding_1                       293

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDPORT_DATA_ADJUST, record_index:0x802F                   */
/*****************************************************************************/

// AdjustMulticastBoundary
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /*  08 */
    LSA_UINT16                  Reserved;
    /*  10 */
    LSA_UINT16                  AdjustProperties;
    /*  12 */
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_TYPE;

#define EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_OFFSET_Reserved              10
#define EDDI_PRM_RECORD_ADJUST_PREAMBLE_LENGTH_OFFSET_AdjustProperties      12

// AdjustMulticastBoundary
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT32                  MulticastBoundary;
    /* 12 */
    LSA_UINT16                  AdjustProperties;
    /* 14 */
    LSA_UINT8                   Padding1_1;
    /* 15 */
    LSA_UINT8                   Padding1_2;

    /* 16 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_TYPE;

#define EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_MulticastBoundary           8
#define EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_AdjustProperties           12
#define EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_Padding1_1                 14
#define EDDI_PRM_RECORD_ADJUST_MULTICAST_BOUNDARY_OFFSET_Padding1_2                 15


// AdjustDomainBoundary
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT32                  DomainBoundaryIngress;
    /* 12 */
    LSA_UINT32                  DomainBoundaryEgress;
    /* 14 */
    LSA_UINT16                  AdjustProperties;
    /* 18 */
    LSA_UINT8                   Padding1_1;
    /* 19 */
    LSA_UINT8                   Padding1_2;

    /* 20 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_TYPE;

#define EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_DomainBoundaryIngress          8
#define EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_DomainBoundaryEgress          12
#define EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_AdjustProperties              14
#define EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_Padding1_1                    18
#define EDDI_PRM_RECORD_ADJUST_DOMAIN_BOUNDARY_OFFSET_Padding1_2                    19


// AdjustLinkState
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT16                  PortState;
    /* 10 */
    LSA_UINT16                  AdjustProperties;

    /* 12 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE EDDI_PRM_RECORD_ADJUST_LINK_STATE_TYPE;

#define EDDI_PRM_RECORD_ADJUST_PORT_STATE_OFFSET_PortState                       8
#define EDDI_PRM_RECORD_ADJUST_PORT_STATE_OFFSET_AdjustProperties               10


// AdjustMAUType
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT16                  MAUType;
    /* 10 */
    LSA_UINT16                  AdjustProperties;

    /* 12 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE EDDI_PRM_RECORD_ADJUST_MAU_TYPE_TYPE;

#define EDDI_PRM_RECORD_ADJUST_MAU_TYPE_OFFSET_MAUType                   8
#define EDDI_PRM_RECORD_ADJUST_MAU_TYPE_OFFSET_AdjustProperties         10

// AdjustDCPBoundary
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT32                  DCPBoundary;
    /* 12 */
    LSA_UINT16                  AdjustProperties;
    /* 14 */
    LSA_UINT8                   Padding1_1;
    /* 15 */
    LSA_UINT8                   Padding1_2;

    /* 16 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_TYPE;

#define EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_DCPBoundary               8
#define EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_AdjustProperties         12
#define EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_Padding1_1               14
#define EDDI_PRM_RECORD_ADJUST_DCP_BOUNDARY_OFFSET_Padding1_2               15


//PDPortDataAdjust, maximum length
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT16                  SlotNummer;
    /* 10 */
    LSA_UINT16                  SubslotNummer;

    /* 12 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader1; /* DomainBoundary    */
    /* 20 */
    LSA_UINT8                   Dummy1[12];
    /* 32 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader2; /* MulitcastBoundary */
    /* 40 */
    LSA_UINT8                   Dummy2[8];
    /* 48 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader3; /* PortState         */
    /* 56 */
    LSA_UINT8                   Dummy3[4];
    /* 60 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader4; /* PeerToPeerBoundary */
    /* 68 */
    LSA_UINT8                   Dummy4[8];
    /* 76 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader5; /* DcpBoundary */
    /* 84 */
    LSA_UINT8                   Dummy5[8];
    /* 92 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader6; /* PreambleLength */
    /* 100 */
    LSA_UINT8                   Dummy6[4];
    /* 104 */
                                              /* AdjustMAUTypeExtension is not supported */
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_TYPE EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_PTR_TYPE;

#define EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SlotNummer      8
#define EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_SubSlotNummer   10
#define EDDI_PRM_RECORD_PDPORT_DATA_ADJUST_MAX_OFFSET_BlockHeader1    12

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDCONTROL_PLL, record_index:0xB050                        */
/*****************************************************************************/

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT16                  PLL_Mode;
    /* 10 */
    LSA_UINT16                  Reserved;

    /* 12 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE   EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDCONTROL_PLL_TYPE_PTR_TYPE;

#define EDDI_PRM_RECORD_PDCONTROL_PLL_OFFSET_PLL_Mode       8

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDTRACEUNIT_CONTROL, record_index:0xB060                  */
/*****************************************************************************/

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT32                  DiagnosticEnable;
    /* 12 */
    LSA_UINT32                  Reserved;

    /* 16 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_TYPE   EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_PTR_TYPE;

#define EDDI_PRM_RECORD_PDTRACE_UNIT_CONTROL_OFFSET_DiagnosticEnable    8

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDTraceUnitRTCData, record_index:0xB061                   */
/*****************************************************************************/

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_DIAGNOSTIC_ENTRY_TYPE
{
    /* 00 */
    LSA_UINT8                       PortID;
    /* 01 */
    LSA_UINT8                       Reserved_1;
    /* 02 */
    LSA_UINT16                      FrameID;
    /* 04 */
    LSA_UINT32                      DiagnosticState;
    /* 08 */
    LSA_UINT32                      TimeNS_SrcIPAddr;
    /* 12 */
    LSA_UINT16                      CycleNumber;
    /* 14 */
    LSA_UINT8                       SrcMACAddr[EDD_MAC_ADDR_SIZE];
    /* 20 */
    LSA_UINT32                      Reserved_2;

    /* 24 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_DIAGNOSTIC_ENTRY_TYPE EDDI_PRM_RECORD_DIAGNOSTIC_ENTRY_TYPE;

#define  EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA                  64UL

PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE              BlockHeader;
    /* 08 */
    LSA_UINT16                               HardwareType;
    /* 10 */
    LSA_UINT16                               HardwareBufferCnt;
    /* 12 */
    LSA_UINT16                               LostDiagEntryCnt;
    /* 14 */
    LSA_UINT16                               RecvDiagEntryCnt;

    /* 16 */
    EDDI_PRM_RECORD_DIAGNOSTIC_ENTRY_TYPE    DiagEntry[EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA];

    /* 16 + n * 24 Length, Max n = 64 */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE EDDI_PRM_RECORD_PDTRACE_UNIT_DATA_TYPE;

/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDSCF_DATA_CHECK, record_index:0x10000                    */
/*****************************************************************************/

// PDSCFDataCheck
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE
{
    /* 00 */
    LSA_UINT16          BlockType;
    /* 02 */
    LSA_UINT16          BlockLength;
    /* 04 */
    LSA_UINT8           BlockVersionHigh;
    /* 05 */
    LSA_UINT8           BlockVersionLow;
    /* 06 */
    LSA_UINT16          SendClockFactor;
    /* 08 */
    LSA_UINT16          SendClockProperties;
    /* 10 */
    LSA_UINT16          Reserved;
    /* 12 */
    LSA_UINT8           Data[32];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE EDDI_PRM_RECORD_PDSCF_DATA_CHECK_TYPE;


#define EDDI_PRM_RECORD_PDSCF_DATA_CHECK_OFFSET_SendClockFactor          6
#define EDDI_PRM_RECORD_PDSCF_DATA_CHECK_OFFSET_SendClockProperties      8
#define EDDI_PRM_RECORD_PDSCF_DATA_CHECK_OFFSET_Reserved                10
#define EDDI_PRM_RECORD_PDSCF_DATA_CHECK_OFFSET_Data                    12


/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDSET_DEFAULT_PORT_STATES, record_index:0x10001           */
/*****************************************************************************/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    (                                                                      */
/*      LSA_UINT8           PortId                                      1    */
/*      LSA_UINT8           PortState                                   1    */
/*      LSA_UINT8           Padding                                     1    */
/*      LSA_UINT8           Padding                                     1    */
/*    )*                                                                     */
/*                                                                           */
/* PortID: 1..255 (a valid PortId)                                           */
/* each PortId must be present Port and only appears once within record!     */
/* Multiple PortId settings may be present within record.                    */
/*                                                                           */
/*****************************************************************************/

/* defines for PortState */
#define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_DISABLE   0x01
#define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_BLOCKING  0x02

#define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_OFFSET_PortID            0
#define EDDI_PRM_RECORD_PD_SET_DEFAULT_PORT_STATES_OFFSET_PortState         1


/*****************************************************************************/
/*  EDDI_PRM_INDEX_PDNRT_FEEDIN_LOAD_LIMITATION, record_index:0x10003        */
/*****************************************************************************/
/*                                                                           */
/*    BlockHeader                                                       6    */
/*    LSA_UINT8           Padding                                       1    */
/*    LSA_UINT8           Padding                                       1    */
/*    (                                                                      */
/*      LSA_UINT8           LoadLimitationActive                        1    */
/*      LSA_UINT8           IO_Configured                               1    */
/*      LSA_UINT16          Padding                                     2    */
/*    )*                                                                     */
/*                                                                           */
/* LoadLimitationActive :  0: Inactive, 1: Active                            */
/* IO_Configured        :  0: No,       1: Yes                               */
/*                                                                           */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_TYPE BlockHeader;
    /* 08 */
    LSA_UINT8                   LoadLimitationActive;
    /* 09 */
    LSA_UINT8                   IO_Configured;
    /* 10 */
    LSA_UINT16                  Reserved;

    /* 12 Length */

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE   EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_TYPE_PTR_TYPE;

#define EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_LoadLimitationActive   8
#define EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_IO_Configured          9
#define EDDI_PRM_RECORD_PDNRT_FEEDIN_LOAD_LIMITATION_OFFSET_Reserved               10

/* DFP */
/*****************************************************************************/
/*  block header without padding                                             */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE
{
    /* 00 */
    LSA_UINT16          BlockType;
    /* 02 */
    LSA_UINT16          BlockLength;    /* without Type/Length field */
    /* 04 */
    LSA_UINT8           BlockVersionHigh;
    /* 05 */
    LSA_UINT8           BlockVersionLow;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE;

#if defined (EDDI_CFG_DFP_ON)
/*****************************************************************************/
/*  SubframeBlock                                                            */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE   BlockHeader;
    /* 06 */
    LSA_UINT16	                                  FrameID; 
    /* 08 */
    LSA_UINT32	                                  SFIOCRProperties;
    /* 12 + n * (-> max 64) */
    LSA_UINT32                                    SubframeData[1];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_TYPE EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_TYPE   EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_PTR_TYPE;


#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_FrameID                      6
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DistributedWatchDogFactor    7
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DFP_MODE                     9
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_DFP_RED_PATH_LAYOUT          8
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_START_FIRST_SubframeData    12
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_SubframeData_Position        3
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_OFFSET_SubframeData_Length          2


/*****************************************************************************/
/*  PDIRSubframeData                                                         */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE  BlockHeader;
    /* 06 */
    LSA_UINT16                       	         NumberOfSubframeBlocks;  
    /* 08 + (n * m) */
    EDDI_PRM_RECORD_PDIR_SUBFRAME_BLOCK_TYPE     SubframeBlock[1];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_TYPE   EDDI_LOCAL_MEM_ATTR *  EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_PTR_TYPE;


#define EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_OFFSET_NumberOfSubframeBlocks       6
#define EDDI_PRM_RECORD_PDIR_SUBFRAME_DATA_OFFSET_START_FIRST_SubframeBlock    8

#endif //defined (EDDI_CFG_DFP_ON)

/*****************************************************************************/
/*  PDIRSubframeData                                                         */
/*****************************************************************************/
PNIO_PACKED_ATTRIBUTE_PRE  struct _EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE
{
    /* 00 */
    EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_TYPE  BlockHeader;
    /* 06 */
    LSA_UINT16                       	         ConsumerCnt;  
    /* 08 */
    LSA_UINT16                                   ConsumerIODataLength;
    /* 10 */
    LSA_UINT16                                   Reserved;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_TYPE;

#define EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_OFFSET_ConsumerCnt                 6
#define EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_OFFSET_ConsumerIODataLength        8
#define EDDI_PRM_RECORD_PDIR_APPLICATION_DATA_OFFSET_Reserved                   10

#include "pnio_pck_off.h"

/*###########################################################################*/
/*  UNPACKED STRUCTS                                                         */
/*###########################################################################*/
typedef struct _EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE
{
    LSA_UINT16          BlockType;
    LSA_UINT16          BlockLength;    /* without Type/Length field */
    LSA_UINT8           BlockVersionHigh;
    LSA_UINT8           BlockVersionLow;
} EDDI_PRM_RECORD_HEADER_WITHOUT_PADDING_UNPACKED_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_H


/*****************************************************************************/
/*  end of file eddi_prm.h                                                   */
/*****************************************************************************/

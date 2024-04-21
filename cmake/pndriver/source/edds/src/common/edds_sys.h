#ifndef EDDS_SYS_H                  /* ----- reinclude-protection ----- */
#define EDDS_SYS_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: edds_sys.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*  Note: All output macros are in edd_out.h                                 */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

/**@note AP01167690, CR: RTOverUDP ablehnen */
#ifdef EDDS_XRT_OVER_UDP_INCLUDE
#warning actually RToverUDP is not supported in PNIO stack - EDDS will undefine EDDS_XRT_OVER_UDP_INCLUDE!
#undef EDDS_XRT_OVER_UDP_INCLUDE
#endif

#if defined(EDDS_CFG_MAX_MC_MAC_CNT) && (EDDS_CFG_MAX_MC_MAC_CNT < 15)
#error EDDS_CFG_MAX_MC_MAC_CNT must be at least set to 15.
#endif

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* FrameID ranges                                                            */
/*---------------------------------------------------------------------------*/

#define EDDS_PTCP_SYNC_START			0x0080
#define EDDS_PTCP_SYNC_END				0x0081

#define EDDS_PTCP_DELAY_START			0xFF40
#define EDDS_PTCP_DELAY_END				0xFF43

#define EDDS_PTCP_ANNO_START			0xFF00
#define EDDS_PTCP_ANNO_END				0xFF01

#define EDDS_PTCP_SYNC_FOLLOWUP_START	0x0020
#define EDDS_PTCP_SYNC_FOLLOWUP_END		0x0021

#define EDDS_PTCP_FOLLOWUP_START		0xFF20
#define EDDS_PTCP_FOLLOWUP_END			0xFF21




/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for EDDS_FatalError())                             */
/*---------------------------------------------------------------------------*/

#define EDDS_FATAL_ERROR_TYPE LSA_UINT32

#define EDDS_FATAL_ERR_HANDLE_CLOSE    (EDDS_FATAL_ERROR_TYPE)  0x01
#define EDDS_FATAL_ERR_HANDLE_REL      (EDDS_FATAL_ERROR_TYPE)  0x02
#define EDDS_FATAL_ERR_DEV_INFO_CLOSE  (EDDS_FATAL_ERROR_TYPE)  0x03
#define EDDS_FATAL_ERR_CLOSE_DDB       (EDDS_FATAL_ERROR_TYPE)  0x04
#define EDDS_FATAL_ERR_NULL_PTR        (EDDS_FATAL_ERROR_TYPE)  0x05
#define EDDS_FATAL_ERR_MEM_FREE        (EDDS_FATAL_ERROR_TYPE)  0x06
#define EDDS_FATAL_ERR_LL              (EDDS_FATAL_ERROR_TYPE)  0x07
#define EDDS_FATAL_ERR_RELEASE_PATH_INFO (EDDS_FATAL_ERROR_TYPE) 0x08
#define EDDS_FATAL_ERR_ISR             (EDDS_FATAL_ERROR_TYPE)  0x09
#define EDDS_FATAL_ERR_SEND            (EDDS_FATAL_ERROR_TYPE)  0x0A
#define EDDS_FATAL_ERR_TIMEOUT_SEND    (EDDS_FATAL_ERROR_TYPE)  0x0B
#define EDDS_FATAL_ERR_TIMER_START     (EDDS_FATAL_ERROR_TYPE)  0x0C
#define EDDS_FATAL_ERR_INCONSISTENCE   (EDDS_FATAL_ERROR_TYPE)  0x0D
#define EDDS_FATAL_ERR_CHANNEL_USE     (EDDS_FATAL_ERROR_TYPE)  0x0E
#define EDDS_FATAL_ERR_TIMER_RESOURCE  (EDDS_FATAL_ERROR_TYPE)  0x0F
#define EDDS_FATAL_ERR_TX_RESOURCES    (EDDS_FATAL_ERROR_TYPE)  0x10
#define EDDS_FATAL_ERR_REENTRANCE      (EDDS_FATAL_ERROR_TYPE)  0x11
#define EDDS_FATAL_ERR_ENDIANES        (EDDS_FATAL_ERROR_TYPE)  0x12

/*---------------------------------------------------------------------------*/
/* intermediate call control flags (used in edds_scheduler())               */
/*---------------------------------------------------------------------------*/

#define EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART1              0x00000001U
#define EDDS_SCHEDULER_BITFIELD_NRT_SEND_PART2              0x00000002U
#define EDDS_SCHEDULER_BITFIELD_NRT_RCV                     0x00000004U
#define EDDS_SCHEDULER_BITFIELD_SRT                         0x00000008U
#define EDDS_SCHEDULER_BITFIELD_CYCLIC_TASKS                0x00000010U
#define EDDS_SCHEDULER_BITFIELD_RESET_FILL                  0x00000020U
#define EDDS_SCHEDULER_BITFIELD_HIGHPRIOR_RQB               0x00000040U


/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

#define EDDS_HANDLE            LSA_VOID EDDS_LOCAL_MEM_ATTR *
/* for device-handle   */

#define EDDS_DETAIL_PTR_TYPE               /* pointer to EDDS_DETAIL */ \
struct edds_detail_tag      EDDS_SYSTEM_MEM_ATTR *

#define EDDS_FATAL_ERROR_PTR_TYPE         /* pointer to LSA_FATAL_ERROR */ \
struct lsa_fatal_error_tag  EDDS_LOCAL_MEM_ATTR *


/*===========================================================================*/

#define EDDS_MAX_PORT_CNT      EDD_CFG_MAX_PORT_CNT
#define EDDS_MAX_SCHEDULER_CYCLE_IO   64  /* milliseconds */
#define EDDS_MAX_SCHEDULER_CYCLE_NRT  100 /* milliseconds */

#define EDDS_DEFAULT_CONSUMER_COUNT   64

/*---------------------------------------------------------------------------*/
/* Service within RQB for EDD_OPC_SYSTEM                                     */
/*                                                                           */
/* Bits 08..15 used for decoding of service by EDDS. Do not change!          */
/*                                                                           */
/* Range 0x0000..0x0FFF reserved for Device-Services                         */
/*---------------------------------------------------------------------------*/

/*=====  EDD_OPC_SYSTEM:  System-Services (0x0100..0x01FF) ===*/
#define EDDSP_SRV_DEV_TYPE                              (EDD_SERVICE)   0x0100

#define EDDS_SRV_DEV_OPEN                               (EDD_SERVICE)   0x0100
#define EDDS_SRV_DEV_SETUP                              (EDD_SERVICE)   0x0101
#define EDDS_SRV_DEV_SHUTDOWN                           (EDD_SERVICE)   0x0102
#define EDDS_SRV_DEV_CLOSE                              (EDD_SERVICE)   0x0103
#define EDDS_SRV_DEV_TIMEOUT                            (EDD_SERVICE)   0x0121
#define EDDS_SRV_DEV_TRIGGER                            (EDD_SERVICE)   0x0122

/*---------------------------------------------------------------------------*/
/* Additional status within RQBs �                                           */
/* ERR-Codes from Offset 0x2F..0x3E reserved for EDDS specific systemfunctions*/
/*---------------------------------------------------------------------------*/

/* EDDS OK-Code (MaxOffset = 0x3F) */
/* LSA_RSP_OK_OFFSET = 0x40       */

/* EDDS ERR-Codes (MaxOffset = 0x3E)*/
/* LSA_RSP_ERR_OFFSET = 0xC0       */

/*---------------------------------------------------------------------------*/
/* Defines for output functions                                              */
/*---------------------------------------------------------------------------*/
#ifdef EDDS_CFG_CSRT_CONSUMER_CYCLE_JITTER_CHECK
#define EDDS_CSRT_CYCLE_JITTER_CHECK_DONT_DO_CONS_CHECK  0
#define EDDS_CSRT_CYCLE_JITTER_CHECK_DO_CONS_CHECK       1
#endif

/*---------------------------------------------------------------------------*/
/* RQB- service-specific paramteter blocks, pParam points to.                */
/*      Opcode and Service specify the Paramblock                            */
/*                                                                           */
/*   RQB-Header                                                              */
/*  +------------+                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  |            |                                                           */
/*  +------------+ service-       e.g. EDDS_UPPER_DEV_OPEN_PTR_TYPE          */
/*  |   pParam   | ------------->+-----------+                               */
/*  +------------+ specific      |           |                               */
/*                               | +------------+                            */
/*                               | |            |                            */
/*                               | | +-------------+                         */
/*                               | | |             |                         */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/
/*======================= Device open/setup services ========================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* switch- device management structure                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* MaxMRP_Instances           : Number of MRP instances supported for this (out)                        */
/*                              interface. This parameter is configured                                 */
/*                              within EDD startup.                                                     */
/*                              0: none. MRP not supported.                                             */
/*                              1.. EDD_CFG_MAX_MRP_INSTANCE_CNT                                        */
/* MaxMRPInterconn_Instances  : Number of MRP interconnection instances supported for this (out)        */
/*                              interface. This parameter is configured                                 */
/*                              within EDD startup.                                                     */
/*                              0: none. MRP interconnection/RSTP+ not supported.                       */
/*                              1.. EDD_CFG_MAX_MRP_IN_INSTANCE_CNT                                     */
/*                              NOTE: If RSTP+ shall be supported, it must not be 0.                    */
/* MRPDefaultRoleInstance0    : If MRP is supported but no MRP             (out)                        */
/*                              parameterization is present it is assumed                               */
/*                              that only one MRP instance is present.                                  */
/*                              In this case this parameter defines the MRP                             */
/*                              role for MRP instance 0.                                                */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/* MRPSupportedRole           : This parameter defines the possible MRP    (out)                        */
/*                              roles if only one MRP instance is present                               */
/*                              within parameterization (PRM).                                          */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/*                              Values can be ORed!                                                     */
/* SupportedMRPInterconnRole  : This parameter defines the possible MRP    (out)                        */
/*                              interconnection roles if only one MRP instance is present               */
/*                              within parameterization (PRM).                                          */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/*                              Values can be ORed!                                                     */
/* MRPSupportedMultipleRole   : This parameter defines the possible MRP    (out)                        */
/*                              roles if multiple  MRP instance are present                             */
/*                              within parameterization (PRM).                                          */
/*                              The parameter is configured within EDD on                               */
/*                              startup and only forwarded to the user (MRP)                            */
/*                              Values can be ORed!                                                     */
/* MRAEnableLegacyMode        : MRA2.0 support: This parameter is only                                  */
/*                              forwarded to the user (MRP)                                             */
/* MRPRingPort          : For each port of the interface this parameter defines if the port is          */
/*                        a ring port used with MRP and if this port is a default ring port on startup  */
/*                          -> possible defines for this parameter see <edd_usr.h>                      */
/* SupportsMRPInterconnPortConfig : For each port of the interface this parameter defines if the port   */
/*                                  may be an interconnection port                                      */
/*                                  -> possible defines for this parameter see <edd_usr.h>              */
typedef struct _EDDS_DPB_SWI_TYPE
{
    LSA_UINT8                       MaxMRP_Instances;
    LSA_UINT8                       MaxMRPInterconn_Instances;
    LSA_UINT8                       MRPDefaultRoleInstance0;
    LSA_UINT8                       MRPSupportedRole;
    LSA_UINT8                       SupportedMRPInterconnRole;
    LSA_UINT8                       MRPSupportedMultipleRole;
    LSA_UINT8                       MRAEnableLegacyMode;
    LSA_UINT8                       MRPRingPort[EDDS_MAX_PORT_CNT];
    LSA_UINT8                       SupportsMRPInterconnPortConfig[EDDS_MAX_PORT_CNT];
} EDDS_DPB_SWI_TYPE;

typedef EDDS_DPB_SWI_TYPE   EDD_UPPER_MEM_ATTR *   EDDS_UPPER_DPB_SWI_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Number of parameterset for NRT-Only or CRT/NRT                            */
/*---------------------------------------------------------------------------*/

#define EDDS_DPB_PARAMETERSET_NRT_MODE 0
#define EDDS_DPB_PARAMETERSET_CRT_MODE 1

#define EDDS_DPB_PARAMETERSET_COUNT    2

/*===========================================================================*/
/*=========================  INI COMPONENTS  ================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_OPEN                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DPB_NRT_PARAMETER_SET_TYPE
{
    LSA_UINT32              RxBufferCntARPIP;     /* number of rx-buffers to alloc for NRT ARP/IP*/
    LSA_UINT32              RxBufferCntASRTOther; /* number of rx-buffers to alloc for ASRT other*/
    LSA_UINT32              RxBufferCntASRTAlarm; /* number of rx-buffers to alloc for ASRT alarm*/
    LSA_UINT32              RxBufferCntOther;     /* number of rx-buffers to alloc for NRT Other */

} EDDS_DPB_NRT_PARAMETER_SET_TYPE;

typedef EDDS_DPB_NRT_PARAMETER_SET_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DPB_NRT_PARAMETER_SET_PTR_TYPE;


typedef struct _EDDS_DPB_NRT_TYPE
{
    // different parameterset (NRT only, NRT and CRT, ...)
    EDDS_DPB_NRT_PARAMETER_SET_TYPE  parameterSet[EDDS_DPB_PARAMETERSET_COUNT];

    LSA_UINT8               RxFilterUDP_Unicast;
    LSA_UINT8               RxFilterUDP_Broadcast;

    LSA_UINT8               FeedInLoadLimitationActive; /* defines the default behaviour of the NRT Feed-In Load Limitation */
    LSA_UINT8               IO_Configured;              /* defines the default behaviour of IO_Configured */
    LSA_UINT32              FeedInLoadLimitationNRTPacketsMS; /* limiting NRT send : maximum packets per millisecond (0 == unlimited) */
} EDDS_DPB_NRT_TYPE;

typedef struct _EDDS_DPB_NRT_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DPB_NRT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_COMP_SRT_INI               */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* type for managing the primary-AR ID                                       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS ON                                                        */
/*---------------------------------------------------------------------------*/
#include "pnio_pck2_on.h"

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDS_CSRT_SRD_ARSET_TYPE
{

    LSA_UINT16      PrimaryIDs[EDD_CFG_MAX_NR_ARSETS];

} PNIO_PACKED_ATTRIBUTE_POST;


typedef struct _EDDS_CSRT_SRD_ARSET_TYPE       EDDS_CSRT_SRD_ARSET_TYPE;
typedef struct _EDDS_CSRT_SRD_ARSET_TYPE   *   EDDS_CSRT_SRD_ARSET_TYPE_PTR;

#include "pnio_pck_off.h"
/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS OFF                                                       */
/*---------------------------------------------------------------------------*/

/* cyclic SRT init-Structure */

/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS ON                                                        */
/*---------------------------------------------------------------------------*/
#include "pnio_pck4_on.h"

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE
{
    LSA_UINT16                  ProviderCrCnt;       // amount of providers that are configured
    LSA_UINT16                  ConsumerCrCnt;       // amount of consumers that are configured
    LSA_UINT32                  pProviderCrTableOffset; // offset in Byte from pointer to pIOBufferManagement
    LSA_UINT32                  pConsumerCrTableOffset; // offset in Byte from pointer to pIOBufferManagement
    EDDS_CSRT_SRD_ARSET_TYPE    PrimaryARIDSetTable;    // Table to store primary ARID for an ARSet
} PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE       EDDS_CSRT_MGM_INFOBLOCK_TYPE;
typedef struct _EDDS_CSRT_MGM_INFOBLOCK_TYPE   *   EDDS_CSRT_MGM_INFOBLOCK_TYPE_PTR;

#include "pnio_pck_off.h"
/*---------------------------------------------------------------------------*/
/*  PACKED STRUCTS OFF                                                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DPB_CSRT_TYPE
{
    LSA_UINT16              CycleBaseFactor;
    LSA_UINT16              ConsumerFrameIDBaseClass2;
    LSA_UINT16              ConsumerCntClass2;
    LSA_UINT16              ConsumerFrameIDBaseClass1;
    LSA_UINT16              ConsumerCntClass1;
    LSA_UINT16              ProviderCnt;                   /* XRT+XRToverUDP */
    LSA_UINT16              ProviderCycleDropCnt;
    EDD_COMMON_MEM_PTR_TYPE hIOBufferManagement;           /* CRT IOBuffer handler mgm */
                                                           /* in shared memory         */
} EDDS_DPB_CSRT_TYPE;

typedef struct _EDDS_DPB_CSRT_TYPE EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DPB_CSRT_PTR_TYPE;

/* ConsumerFrameIDBase has to be >= EDD_SRT_FRAMEID_CSRT_START and           */
/* ConsumerFrameIDBase+ConsumerCnt has to be <= EDD_SRT_FRAMEID_CSRT_STOP    */

/* CycleBaseFactor                                                           */
/* Base factor for calculation of base cycle time (=CycleBaseFactor* 31.25us)*/
/* For calculation of Cycle-Time: 250...4000us                               */
/* We currently only support 32,64,128 (=1000us,2000us,4000us Cylce-Time)    */

/* ConsumerFrameIDBaseClass2                                                 */
/* Start of FrameID-Range valid for RT-Class 2 Consumers                     */
/* Range: EDD_SRT_FRAMEID_CSRT_START1 ..  EDD_SRT_FRAMEID_CSRT_STOP1         */
/* Note: Shall be used from Class 2 FrameIDs                                 */

/* ConsumerCntClass2                                                         */
/* Max RT-Class2 Consumers supported. (SRT)                                  */
/* Range: 1..(EDD_SRT_FRAMEID_CSRT_STOP1-ConsumerFrameIDBase1+1)             */

/* ConsumerFrameIDBaseClass1                                                 */
/* Start of FrameID-Range valid for Class1 Consumers                         */
/* Range: EDD_SRT_FRAMEID_CSRT_START2  ..  EDD_SRT_FRAMEID_CSRT_STOP2        */
/* Note: Shall be used from Class 1 FrameIDs                                 */

/* ConsumerCntClass1                                                         */
/* Max RT-Class1 Consumers supported (SRT and xRToverUDP)                    */
/* Range: 1..(EDD_SRT_FRAMEID_CSRT_STOP2-ConsumerFrameIDBase2+1)             */

/* ProviderCnt                                                               */
/* Max Provider supported.                                                   */
/* Range: 1..EDDS_SRT_PROVIDER_CNT_MAX                                       */

#define EDDS_SRT_PROVIDER_CNT_MAX               0x7FFF   /* < 0xFFFF ! */

/* ProviderCycleDropCnt                                                      */
/* Number of provider cycles to drop on machine overload (if we can not send */
/* all providers within cycle.                                               */
/* 0: no drops                                                               */

#define EDDS_SRT_PROVIDER_CYCLE_DROP_MAX        255

/* ProviderCylcePhaseMaxByteCnt 1)                                           */
/* Max number of i/o-bytes for all providers within one cycle                */
/* ProviderCylcePhaseMaxCnt 1)                                               */
/* Max number of providers within one cycle.                                 */
/* 1) used with EDD_SRV_SRT_PROVIDER_ADD                                     */
/* ProviderCylcePhaseMaxByteCntHard 2)                                       */
/* Max number of i/o-bytes for all providers within one cycle. hard-limit    */
/* ProviderCylcePhaseMaxCntHard 2)                                           */
/* Max number of providers within one cycle. hard-limit for                  */
/* 2) used with EDD_SRV_SRT_PROVIDER_CHANGE_PHASE.                           */


/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_OPEN                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DPB_FEATURE_TYPE
{
    LSA_UINT8     AdditionalForwardingRulesSupported;       /* special support for HSYN MAC */
    LSA_UINT8     ApplicationExist;                         /* support HSYNC application    */
    LSA_UINT8     ClusterIPSupport;                         /* ClusterIP supported for MC   */
    LSA_UINT8     MRPInterconnFwdRulesSupported;            /* MRP Interconnection fwd rules support */
    LSA_UINT8     MRPInterconnOriginatorSupported;          /* MRP as originator for frames with MRP interconnection MC destination */
} EDDS_DPB_FEATURE_TYPE;

typedef struct _EDDS_DPB_TYPE
{
    /* Global parameters ---*/
    EDDS_SYS_HANDLE hSysDev;                    /* system handle        */
    LSA_UINT32      InterfaceID;                /* Interface ID         */
    LSA_UINT32      TraceIdx;            	    /* Interface ID         */
    LSA_UINT32      RxBufferCnt;                /* number of Rx-Buffers [In/Out] */
    LSA_UINT32      TxBufferCnt;                /* number of Tx-Buffers [In/Out] */
    LSA_UINT8       ZeroCopyInterface;          /* use ZeroCopyInterface */
    LSA_UINT8       schedulerRecurringTaskMS;   /* reduction of recurring task execution in scheduler (aimed execution in MS) */
    LSA_UINT16      schedulerCycleIO;           /* scheduler cycle in milliseconds for IO mode */
    LSA_UINT16      schedulerCycleNRT;          /* scheduler cycle in milliseconds for NRT mode */

    LSA_UINT32      RxLimitationPackets_IO;  /* limiting receive : maximum processed packets per scheduler call in IO mode (0 == unlimited) */
    LSA_UINT32      RxLimitationPackets_NRT; /* limiting receive : maximum processed packets per scheduler call in NRT mode (0 == unlimited) */

    LSA_VOID_PTR_TYPE		    pLLFunctionTable;
    LSA_VOID_PTR_TYPE 		    pLLManagementData;
    /* HW-dependend --------*/
    LSA_VOID_PTR_TYPE		    pLLParameter;

    /* MaxPortCntOfAllEDD :    Number of all interfaces of all EDDs         (in) */
    /*                         max: EDD_CFG_MAX_INTERFACE_CNT                    */
    LSA_UINT16                  MaxPortCntOfAllEDD;

    /* MaxInterfaceCntOfAllEDD Number of all ports of all EDDs              (in) */
    /*                         max: EDD_CFG_MAX_PORT_CNT_ALL_INTERFACE           */
    LSA_UINT16                  MaxInterfaceCntOfAllEDD;

    EDDS_DPB_SWI_TYPE           SWI;

    EDDS_DPB_NRT_TYPE           NRT;
    EDDS_DPB_CSRT_TYPE          CSRT;

    EDDS_DPB_FEATURE_TYPE       FeatureSupport;

} EDDS_DPB_TYPE;

typedef EDDS_DPB_TYPE  EDD_UPPER_MEM_ATTR       *    EDDS_UPPER_DPB_PTR_TYPE;
typedef EDDS_DPB_TYPE  EDD_UPPER_MEM_ATTR const *    EDDS_UPPER_DPB_PTR_TO_CONST_TYPE;

#define EDDS_DPB_DO_NOT_USE_ZERO_COPY_IF       0
#define EDDS_DPB_USE_ZERO_COPY_IF              1

/*--------------------------------------------------------------------------*/
/* Optimizations for zero-copy-interface:                                   */
/*--------------------------------------------------------------------------*/
/* RxBufferSize and RxBufferOffset are used for allocating receive-buffers  */
/* inside edds. RxBufferSize is the size of the whole buffer allocated.     */
/* RxBufferOffset is the offset from start where the frame will be put.     */
/* This is also the pointer which will be returned to the user when a frame */
/* is returned! (so there is RxBufferOffset free room before this address!) */
/* Note: RxBufferSize-RxBufferOffset >= EDD_FRAME_BUFFER_LENGTH             */
/* Note: When deallocating rx-buffers you must subtract RxBufferOffset from */
/*       the frame-buffer pointer!                                          */
/* Note: The edds does not allocate or frees any transmitbuffers exchanged  */
/*       whith the user.                                                    */
/* Note: RxBufferOffset must be a multiple of the rx-buffer-alignment       */
/*       (i.e. a multiple of (EDDS_RX_FRAME_BUFFER_ALIGN+1)                 */
/*--------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_INI_TYPE
{
    EDDS_HANDLE             hDDB;
    EDDS_UPPER_DPB_PTR_TYPE pDPB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_INI_TYPE;

typedef EDDS_RQB_DDB_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_INI_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*=================  SETUP/SHUTDOWN/CLOSE DEVICE  ===========================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_SETUP                      */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_DSB_TYPE
{
    EDD_MAC_ADR_TYPE    MACAddress[EDD_CFG_MAX_PORT_CNT+1];
} EDDS_DSB_TYPE;

typedef struct _EDDS_DSB_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DSB_PTR_TYPE;


/* for use with EDD_SRV_SETUP_PHY         */

/* PHYPower */
#define EDDS_PHY_POWER_ON        0  /* Switch the Phy on, -> Link-Ability    */
#define EDDS_PHY_POWER_OFF       1  /* Switch the Phy off -> No Link-Ability */
#define EDDS_PHY_POWER_UNCHANGED 2  /* No Changes on Phy                     */
/* for use with EDD_SRV_SETUP_PHY        */

typedef struct _EDDS_RQB_DDB_SETUP_TYPE
{
    EDDS_HANDLE             hDDB;
    EDDS_UPPER_DSB_PTR_TYPE pDSB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_SETUP_TYPE;

typedef struct _EDDS_RQB_DDB_SETUP_TYPE EDD_UPPER_MEM_ATTR *   EDDS_UPPER_DDB_SETUP_PTR_TYPE;


/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_SHUTDOWN                   */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_SHUTDOWN_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_SHUTDOWN_TYPE;

typedef struct _EDDS_RQB_DDB_SHUTDOWN_TYPE EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_CLOSE                      */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DDB_REL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
    (EDD_UPPER_RQB_PTR_TYPE pRQB);
} EDDS_RQB_DDB_REL_TYPE;

typedef struct _EDDS_RQB_DDB_REL_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DDB_REL_PTR_TYPE;

/*===========================================================================*/
/*===========================================================================*/


/*===========================================================================*/
/*======================= INTERNAL & SERVICES   =============================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Filled and used internal!!!                                               */
/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDS_SRV_DEV_INTERRUPT                  */
/*                          Service: EDDS_SRV_DEV_TIMEOUT                    */
/*                          Service: EDDS_SRV_DEV_TRIGGER                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_RQB_DEV_INTERNAL_TYPE EDD_UPPER_MEM_ATTR *    EDDS_UPPER_DEV_INTERNAL_PTR_TYPE;

typedef struct _EDDS_RQB_DEV_INTERNAL_TYPE
{
    EDDS_HANDLE             hDDB;
    LSA_UINT32              ID;     /* Sub-ID                      */
    LSA_UINT32              Param;  /* service-dependend parameter */
} EDDS_RQB_DEV_INTERNAL_TYPE;

/* EDDS_SRV_DEV_TRIGGER                     */
/* IDs:                                     */

#define EDDS_TRIGGER_PROV_INDICATION        0      /* trigger a provider indicat.  */
#define EDDS_TRIGGER_CONS_INDICATION        1      /* trigger a consumer indicat.  */
#define EDDS_TRIGGER_GEN_REQUEST_PROCESSED  2      /* trigger from scheduler: general request processed */
#define EDDS_TRIGGER_SCHEDULER              3      /* trigger scheduler from RQB context: only in NRT mode allowed */
#define EDDS_TRIGGER_NRT_CANCEL_PART2       4      /* Trigger NRT-Cancel Part 2    */
#define EDDS_TRIGGER_SCHEDULER_EVENTS_DONE  5      /* schdeduler events done */
#define EDDS_TRIGGER_FILTER_ARP_FRAME       6      /* trigger from scheduler: Filter ARP Frame in RQB Context*/
#define EDDS_TRIGGER_FILTER_DCP_FRAME       7      /* trigger from scheduler: Filter DCP Frame in RQB Context*/
#define EDDS_TRIGGER_FILTER_DCP_HELLO_FRAME 8      /* trigger from scheduler: Filter DCP_HELLO Frame in RQB Context*/

/* 32 events could be signaled to RQB context */
#define EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__NOTHING             0x00000000U
/* do EDDS_DiagCycleRequest (fired from scheduler) */
#define EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__PRM_DIAG_CYCLE      0x00000001U
/* indicate a phy event to rqb context */
#define EDDS_TRIGGER_SCHEDULER_EVENTS_DONE__LINK_CHANGED        0x00000002U

/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by EDDS_GET_PATH_INFO)                          */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* CDB- Parameter for NRT                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CDB_NRT_TYPE
{
    LSA_UINT32  FrameFilter;        /* defines NRT-Frames to be receive for  */
    /* this channel                          */
} EDDS_CDB_NRT_TYPE;

/* defines for FrameFilter. Can be ORed. see edd_usr.h, EDD_SRV_NRT_RECV */

typedef struct _EDDS_CDB_NRT_TYPE  EDD_UPPER_MEM_ATTR *    EDDS_UPPER_CDB_NRT_PTR_TYPE;


/*---------------------------------------------------------------------------*/
/* CDB- Parameter for PRM                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_CDB_PRM_TYPE
{
    LSA_UINT8  PortDataAdjustLesserCheckQuality;
} EDDS_CDB_PRM_TYPE;

typedef struct _EDDS_CDB_PRM_TYPE EDD_UPPER_MEM_ATTR *EDDS_UPPER_CDB_PRM_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* PortDataAdjustLesserCheckQuality:                                         */
/*                                                                           */
/* LSA_TRUE:   Do a lesser strict check (see table below)                    */
/* LSA_FALSE:  No special handling.                                          */
/*                                                                           */
/* PDPortDataAdjust checking rules:                                          */
/*                                                                           */
/* --------+--------------------+----------------------+-------------------- */
/*  PRM    |     IEC61158       |  LesserCheckQuality  |     Local-Flag      */
/* --------+--------------------+----------------------+-------------------- */
/*  WRITE  | PortState = OFF    |             PortState = OFF                */
/*         |                    |                 10 FD/HD                   */
/*         |      100 FD        |                100 FD/HD                   */
/*         |     1000 FD        |               1000 FD/HD                   */
/*         |    10000 FD        |              10000 FD/HD                   */
/*         |     Autoneg 1)     |                 Autoneg 1)                 */
/* --------+--------------------+----------------------+-------------------- */
/*  END    | At least one port has to be set to        |  no restrictions    */
/*         | Autoneg, 100 FD, 1000 FD or 10000 FD      |                     */
/* --------+-------------------------------------------+-------------------- */
/*                                                                           */
/* IEC61158           : Checking rule according to IEC61158.                 */
/* LesserCheckQuality : Rule if PortDataAdjustLesserCheckQuality is set      */
/* Local-Flag         : Rule if "Local"  flag set with PRM-request by CMPD.  */
/*                                                                           */
/*  1) Autoneg is used if no MAUType or PortState is present for this port.  */
/*     This is the default setting.                                          */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in EDDS_GET_PATH_INFO)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct _EDDS_COMP_USE_TYPE
{
    LSA_UINT8    UseNRT;          /* EDD_CDB_CHANNEL_USE_ON: NRT-Component used by channel */
    LSA_UINT8    UseCSRT;         /* EDD_CDB_CHANNEL_USE_ON: cSRT-Component used by channel */
    LSA_UINT8    UsePRM;          /* EDD_CDB_CHANNEL_USE_ON: PRM-Component used by channel */
} EDDS_COMP_USE_TYPE;

typedef struct edds_detail_tag
{
    EDDS_COMP_USE_TYPE          UsedComp;
    EDDS_HANDLE                 hDDB;
    LSA_UINT8                   InsertSrcMAC; /* !=0 : edds inserts src-mac in */
    /*       send frame             */
    EDDS_UPPER_CDB_NRT_PTR_TYPE pNRT;
    EDDS_UPPER_CDB_PRM_PTR_TYPE pPRM;
} EDDS_CDB_TYPE;

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                              external functions                           */
/*===========================================================================*/

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  edds_init
 *
 * function:       initialize EDDS
 *
 * parameters:     ....
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_init(LSA_VOID);

/*=============================================================================
 * function name:  edds_undo_init
 *
 * function:       undo the initialization of EDDS
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/

LSA_RESULT  EDDS_SYSTEM_IN_FCT_ATTR edds_undo_init(LSA_VOID);

/*=============================================================================
 * function name:  edds_system
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDDS_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_system(EDD_UPPER_RQB_PTR_TYPE upper_rqb_ptr);


/*=============================================================================
 * function name:  edds_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDD_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (EDD_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of edds
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_open_channel(EDD_UPPER_RQB_PTR_TYPE pRQB);

/*=============================================================================
 * function name:  edds_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDDS_OPC_CLOS_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_close_channel(EDD_UPPER_RQB_PTR_TYPE pRQB);


/*=============================================================================
 * function name:  edds_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edds
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDDS_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/

LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_request(EDD_UPPER_RQB_PTR_TYPE upper_rqb_ptr);


/*=============================================================================
 * function name:  edds_scheduler
 *
 * function:       EDDS scheduler called cyclically from system adaption (1ms)
 *
 * parameters:     EDDS_HANDLE          hDDB
 *                 LSA_UINT8            intermediateCall
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#define EDDS_SCHEDULER_CYCLIC_CALL          0
#define EDDS_SCHEDULER_INTERMEDIATE_CALL    1
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  edds_scheduler( EDDS_HANDLE const hDDB, LSA_UINT8 const intermediateCall);


/*=============================================================================
 * function name:  edds_SRTProviderGetCycleCounter
 *
 * function:       Get SRT-provider CycleCounter (31.25us ticks)
 *
 * parameters:     EDDS_HANDLE                 hDDB
 *                 EDDS_CYCLE_COUNTER_PTR_TYPE pCycleCounter
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
typedef struct _EDDS_CYCLE_COUNTER_TYPE
{
    LSA_UINT32      High;
    LSA_UINT32      Low;
} EDDS_CYCLE_COUNTER_TYPE;

typedef struct _EDDS_CYCLE_COUNTER_TYPE EDD_UPPER_MEM_ATTR * EDDS_CYCLE_COUNTER_PTR_TYPE;

LSA_RESULT EDDS_SYSTEM_IN_FCT_ATTR edds_SRTProviderGetCycleCounter(
    EDDS_HANDLE                     hDDB,
    EDDS_CYCLE_COUNTER_PTR_TYPE     pCycleCounter);

/*=============================================================================
 * function name:  edds_InitDPBWithDefaults
 *
 * function:       Write default settings into pDPB
 *
 * parameters:     EDDS_UPPER_DPB_PTR_TYPE      pDPB
 *
 * return value:   EDD_RSP
 *===========================================================================*/
EDD_RSP EDDS_LOCAL_FCT_ATTR edds_InitDPBWithDefaults(
    EDDS_UPPER_DPB_PTR_TYPE     pDPB);


/*=============================================================================
 * function name:  edds_InitDSBWithDefaults
 *
 * function:       Write default settings into pDSB
 *
 * parameters:     EDDS_UPPER_DSB_PTR_TYPE      pDSB
 *
 * return value:   EDD_RSP
 *===========================================================================*/
EDD_RSP EDDS_LOCAL_FCT_ATTR edds_InitDSBWithDefaults(
    EDDS_UPPER_DSB_PTR_TYPE     pDSB);

#ifdef EDDS_CFG_SYSRED_API_SUPPORT
/*=============================================================================
*
*  function name:    edds_ProviderGetPrimaryAR
*
*  Description:     Get the primary AR for a specific
*                   ARSet ( determined by ARSetID)
* parameters:       EDDS_HANDLE              hIOBuffer
*                   LSA_UINT16     const     ARSetID
*                   LSA_UINT16    *const     pPrimaryARID
*------------------------------------------------------------------------------
*
* return value:     EDD_STS_OK
*                   EDD_STS_ERR_PARAM
*============================================================================*/
LSA_RESULT edds_ProviderGetPrimaryAR (
    EDDS_HANDLE                         hDDB,
    LSA_UINT16                 const    ARSetID,
    LSA_UINT16                *const    pPrimaryARID);

#endif /*DDS_CFG_SYSRED_API_SUPPORT*/

/*---------------------------------------------------------------------------*/
/* this structure is  used for internal request initiated by interrupt or    */
/* timeout-handler. pParam in pRQB points to EDDS_RQB_DEV_INTERNAL_TYPE      */
/* InUse is set if the RQB is underway (from Int-Handler to edds_system(). if */
/* it arrives edds_system() InUse is reset to LSA_FALSE. So only one RQB is  */
/* used for every event.                                                     */
/*---------------------------------------------------------------------------*/

typedef volatile struct _EDDS_INTERNAL_REQUEST_STRUCT
{
    LSA_BOOL                    InUse;          /* LSA_TRUE if in rqb-queue. Internal parameter, not part of EDDS API! */
    EDD_UPPER_RQB_PTR_TYPE      pRQB;
} EDDS_INTERNAL_REQUEST_TYPE;

typedef EDDS_INTERNAL_REQUEST_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_INTERNAL_REQUEST_PTR_TYPE;

/*===========================================================================*/
/* Capabilities                                                              */
/*===========================================================================*/
typedef struct _EDDS_LL_CAPS_TYPE
{
    LSA_UINT32                         MaxFrameSize;
    LSA_UINT32                         HWFeatures;
    LSA_UINT32                         PortCnt;
} EDDS_LL_CAPS_TYPE;

typedef struct _EDDS_LL_CAPS_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE;
/*-----------------------------------------------------------------------*/
/* MaxFrameSize         Size in byte of maximum frame for send/receive   */
/*                                                                       */
/* HWFeatures                                                            */
#define EDDS_LL_CAPS_HWF_NONE                            (0)
#define EDDS_LL_CAPS_HWF_CRC_GENERATE                    (1/*<<0*/)
#define EDDS_LL_CAPS_HWF_CRC_DECODE                      (1<<1)
#define EDDS_LL_CAPS_HWF_IPV4_CHK_GENERATE               (1<<2)
#define EDDS_LL_CAPS_HWF_IPV4_CHK_DECODE                 (1<<3)
#define EDDS_LL_CAPS_HWF_TCPV4_CHK_GENERATE              (1<<4)
#define EDDS_LL_CAPS_HWF_TCPV4_CHK_DECODE                (1<<5)
#define EDDS_LL_CAPS_HWF_UDPV4_CHK_GENERATE              (1<<6)
#define EDDS_LL_CAPS_HWF_UDPV4_CHK_DECODE                (1<<7)
#define EDDS_LL_CAPS_HWF_TCPV6_CHK_GENERATE              (1<<8)
#define EDDS_LL_CAPS_HWF_TCPV6_CHK_DECODE                (1<<9)
#define EDDS_LL_CAPS_HWF_UDPV6_CHK_GENERATE              (1<<10)
#define EDDS_LL_CAPS_HWF_UDPV6_CHK_DECODE                (1<<11)
#define EDDS_LL_CAPS_HWF_CUT_THROUGH_SUPPORT             (1<<12)
#define EDDS_LL_CAPS_HWF_EXACT_MAC_FILTER                (1<<13)
#define EDDS_LL_CAPS_HWF_PHY_POWER_OFF                   (1<<14)
#define EDDS_LL_CAPS_HWF_SET_MC_BOUNDARIES               (1<<15)
#define EDDS_LL_CAPS_HWF_MCMAC_SRCPORT_SUPPORT           (1<<16)
#define EDDS_LL_CAPS_HWF_CLUSTER_IP_SUPPORT              (1<<17)
#define EDDS_LL_CAPS_HWF_HSYNC_FORWARDER                 (1<<18)
#define EDDS_LL_CAPS_HWF_HSYNC_APPLICATION               (1<<19)
#define EDDS_LL_CAPS_HWF_MRP_INTERCONN_FWD_RULES         (1<<20)
#define EDDS_LL_CAPS_HWF_MRP_INTERCONN_ORIGINATOR        (1<<21)
/*                                                                       */
/* PortCnt              Number of ports on controller; if PortCnt >1,    */
/*                      controller is a switch.                          */
/*                                                                       */
/*-----------------------------------------------------------------------*/

/*===========================================================================*/
/* LL (initial) hw parameter set                                             */
/*===========================================================================*/
typedef struct _EDDS_LL_HW_PARAM_TYPE
{
    LSA_UINT32    PortCnt;                          /* IN:     PortCnt                   */
    EDD_MAC_ADR_TYPE  MACAddress;                   /* OUT: Station MAC address.              */
    EDD_MAC_ADR_TYPE  MACAddressPort[EDDS_MAX_PORT_CNT];/*OUT: Port MAC addresses            */
    LSA_UINT8     PHYPower[EDDS_MAX_PORT_CNT];      /* OUT: Configured PhyPower       */
    LSA_UINT8     LinkSpeedMode[EDDS_MAX_PORT_CNT]; /* OUT: Configured LinkSpeedMode  */
    LSA_UINT8     IsWireless[EDDS_MAX_PORT_CNT];    /* OUT: Configured IsWireless     */
} EDDS_LL_HW_PARAM_TYPE;

typedef EDDS_LL_HW_PARAM_TYPE EDDS_LOWER_MEM_ATTR *         EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE;
typedef EDDS_LL_HW_PARAM_TYPE EDDS_LOWER_MEM_ATTR const *   EDDS_LOCAL_LL_HW_PARAM_PTR_TO_CONST_TYPE;

/* ----------------------------------------------------------------- */
/* PHYPower          : Phy power status of each port                 */
/* LinkSpeedMode     : Link speed mode of each port                  */
/* IsWireless        : Are ports wireless (wireless port:=LSA_TRUE)? */
/* ----------------------------------------------------------------- */

/*===========================================================================*/
/* Statistics                                                                */
/*===========================================================================*/

typedef struct _EDDS_STATISTICS_TYPE
{
	/* RFC2863_COUNTER */
	LSA_UINT32      RequestedCounters;
	/* RFC2863_COUNTER - End */
    LSA_UINT32      SupportedCounters;
    LSA_UINT32      InOctets;
    LSA_UINT32      InUcastPkts;
    LSA_UINT32      InNUcastPkts;
    LSA_UINT32      InDiscards;
    LSA_UINT32      InErrors;
    LSA_UINT32      InUnknownProtos;
    LSA_UINT32      OutOctets;
    LSA_UINT32      OutUcastPkts;
    LSA_UINT32      OutNUcastPkts;
    LSA_UINT32      OutDiscards;
    LSA_UINT32      OutErrors;
    LSA_UINT32      OutQLen;
	/* RFC2863_COUNTER */
    LSA_UINT32      InMulticastPkts;
    LSA_UINT32      InBroadcastPkts;
    LSA_UINT32      OutMulticastPkts;
    LSA_UINT32      OutBroadcastPkts;
    LSA_UINT64      InHCOctets;
    LSA_UINT64      InHCUcastPkts;
    LSA_UINT64      InHCMulticastPkts;
    LSA_UINT64      InHCBroadcastPkts;
    LSA_UINT64      OutHCOctets;
    LSA_UINT64      OutHCUcastPkts;
    LSA_UINT64      OutHCMulticastPkts;
    LSA_UINT64      OutHCBroadcastPkts;
	/* RFC2863_COUNTER - End */
} EDDS_STATISTICS_TYPE;

typedef struct _EDDS_STATISTICS_TYPE EDDS_LOCAL_MEM_ATTR * EDDS_LOCAL_STATISTICS_PTR_TYPE;

typedef struct EDDS_PHY_LINK_STATUS_STRUCT
{
    LSA_UINT32          Link;
    LSA_UINT32          Speed;
    LSA_UINT32          Duplexity;
    LSA_UINT16          MAUType;
    LSA_UINT8           MediaType;
    LSA_UINT8           IsPOF;
    LSA_UINT8           LinkSpeedModeConfigured;
    LSA_UINT32          AutonegCapAdvertised;
    LSA_UINT32          RealPortTxDelay;
    LSA_UINT32          RealPortRxDelay;
    LSA_UINT32          MaxPortTxDelay;
    LSA_UINT32          MaxPortRxDelay;
    LSA_UINT8           IsWireless;
    LSA_UINT8           IsMDIX;
} EDDS_PHY_LINK_STATUS_TYPE;

typedef EDDS_PHY_LINK_STATUS_TYPE EDDS_LOWER_MEM_ATTR       * EDDS_PHY_LINK_STATUS_PTR_TYPE;
typedef EDDS_PHY_LINK_STATUS_TYPE EDDS_LOWER_MEM_ATTR const * EDDS_PHY_LINK_STATUS_PTR_CONST_TYPE;
/* ----------------------------------------------------------------------------------------------------------*/
/*      Link                    : OUT: PHY register containing link information                              */
/*                                IN : Link status (EDD_LINK_UP, EDD_LINK_DOWN)                              */
/*      Speed                   : OUT: PHY register containing speed information                             */
/*                                IN : Current speed status (e.g. EDD_LINK_SPEED_100)                        */
/*      Duplexity               : OUT: PHY register containing duplexity information                         */
/*                                IN : Current duplexity status ( EDD_LINK_MODE_FULL, EDD_LINK_MODE_HALF)    */
/*      MAUType                 : Current MAUType                                                            */
/*      MediaType               : Current MediaType                                                          */
/*      LinkSpeedModeConfigured : Link speed and link mode configured in the phy, it usually corresponds     */
/*                                to the input parameter below Speed, Duplexity and Autoneg, just in case    */
/*                                of POF media, no auto-negotiation is supported, therefore                  */
/*                                pLinkSpeedModeConfigured refers to no autoneg modes                        */
/*      AutonegCapAdvertised    : Current link capabilites which are advertised by transceiver               */
/*      RealPortTxDelay         : real Transmit port delay in nano seconds (ns)                              */
/*      RealPortRxDelay         : real Receive port delay in nano seconds (ns)                               */
/*      MaxPortTxDelay          : maximum Transmit port delay in nano seconds (ns) - only for checking in Prm*/
/*      MaxPortRxDelay          : maximum Receive port delay in nano seconds (ns) - only for checking in Prm */
/*      IsWireless              : Is this port a wireless port? (EDD_PORT_IS_WIRELESS / EDD_PORT_IS_NOT_WIRELESS)    */
/*      IsMDIX                  : Is this a "Crossed Port" ? (EDD_PORT_MDIX_ENABLED / EDD_PORT_MDIX_DISABLED)        */
/*      IsPOF                   : Is this a "POF Port" ? (EDD_PORT_OPTICALTYPE_ISPOF / EDD_PORT_OPTICALTYPE_ISNONPOF)*/
/* ---------------------------- ---------------------------------------------------------------------------- */

/* OBJECT IDENTITIES for MAU types */

#define EDDS_LL_MAUTYPE_AUI			1
//  DESCRIPTION "no internal MAU, view from AUI"
//  REFERENCE   "[IEEE802.3], Section 7"

#define EDDS_LL_MAUTYPE_10BASE5		2
//	DESCRIPTION "thick coax MAU"
//	REFERENCE   "[IEEE802.3], Section 7"

#define EDDS_LL_MAUTYPE_FOIRL			3
//  DESCRIPTION "FOIRL MAU"
//  REFERENCE   "[IEEE802.3], Section 9.9"

#define EDDS_LL_MAUTYPE_10BASE2		4
//  DESCRIPTION "thin coax MAU"
//  REFERENCE   "[IEEE802.3], Section 10"

#define EDDS_LL_MAUTYPE_10BASET		5
//  DESCRIPTION "UTP MAU.
//              Note that it is strongly recommended that
//              agents return either dot3MauType10BaseTHD or
//              dot3MauType10BaseTFD if the duplex mode is
//              known.  However, management applications should
//              be prepared to receive this MAU type value from
//              older agent implementations."
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASEFP 		6
//  DESCRIPTION "passive fiber MAU"
//  REFERENCE   "[IEEE802.3], Section 16"

#define EDDS_LL_MAUTYPE_10BASEFB		7
//  DESCRIPTION "sync fiber MAU"
//  REFERENCE   "[IEEE802.3], Section 17"

#define EDDS_LL_MAUTYPE_10BASEFL		8
//  DESCRIPTION "async fiber MAU.
//              Note that it is strongly recommended that
//              agents return either dot3MauType10BaseFLHD or
//              dot3MauType10BaseFLFD if the duplex mode is
//              known.  However, management applications should
//              be prepared to receive this MAU type value from
//              older agent implementations."
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_10BROAD36		9
//  DESCRIPTION "broadband DTE MAU.
//              Note that 10BROAD36 MAUs can be attached to
//              interfaces but not to repeaters."
//  REFERENCE   "[IEEE802.3], Section 11"

//------ new since RFC 1515:

#define EDDS_LL_MAUTYPE_10BASETHD		10
//  DESCRIPTION "UTP MAU, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASETFD		11
//  DESCRIPTION "UTP MAU, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 14"

#define EDDS_LL_MAUTYPE_10BASEFLHD		12
//  DESCRIPTION "async fiber MAU, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_10BASEFLFD		13
//  DESCRIPTION "async fiber MAU, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 18"

#define EDDS_LL_MAUTYPE_100BASET4		14
//  DESCRIPTION "4 pair category 3 UTP"
//  REFERENCE   "[IEEE802.3], Section 23"

#define EDDS_LL_MAUTYPE_100BASETXHD	15
//  DESCRIPTION "2 pair category 5 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 25"

#define EDDS_LL_MAUTYPE_100BASETXFD	16
//  DESCRIPTION "2 pair category 5 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 25"

#define EDDS_LL_MAUTYPE_100BASEFXHD	17
//  DESCRIPTION "X fiber over PMT, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 26"

#define EDDS_LL_MAUTYPE_100BASEFXFD	18
//  DESCRIPTION "X fiber over PMT, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 26"

#define EDDS_LL_MAUTYPE_100BASET2HD	19
//  DESCRIPTION "2 pair category 3 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 32"

#define EDDS_LL_MAUTYPE_100BASET2FD	20
//  DESCRIPTION "2 pair category 3 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 32"

//------ new since RFC 2239:

#define EDDS_LL_MAUTYPE_1000BASEXHD	21
//  DESCRIPTION "PCS/PMA, unknown PMD, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 36"

#define EDDS_LL_MAUTYPE_1000BASEXFD	22
//  DESCRIPTION "PCS/PMA, unknown PMD, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 36"

#define EDDS_LL_MAUTYPE_1000BASELXHD	23
//  DESCRIPTION "Fiber over long-wavelength laser, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASELXFD	24
//  DESCRIPTION "Fiber over long-wavelength laser, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASESXHD	25
//  DESCRIPTION "Fiber over short-wavelength laser, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASESXFD	26
//  DESCRIPTION "Fiber over short-wavelength laser, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 38"

#define EDDS_LL_MAUTYPE_1000BASECXHD	27
//  DESCRIPTION "Copper over 150-Ohm balanced cable, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 39"

#define EDDS_LL_MAUTYPE_1000BASECXFD	28
//  DESCRIPTION "Copper over 150-Ohm balanced cable, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 39"

#define EDDS_LL_MAUTYPE_1000BASETHD	29
//  DESCRIPTION "Four-pair Category 5 UTP, half duplex mode"
//  REFERENCE   "[IEEE802.3], Section 40"

#define EDDS_LL_MAUTYPE_1000BASETFD	30
//  DESCRIPTION "Four-pair Category 5 UTP, full duplex mode"
//  REFERENCE   "[IEEE802.3], Section 40"

//------ new since RFC 2668:

#define EDDS_LL_MAUTYPE_10GIGBASEX		31
//  DESCRIPTION "X PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 48"

#define EDDS_LL_MAUTYPE_10GIGBASELX4	32
//  DESCRIPTION "X fiber over WWDM optics"
//  REFERENCE   "[IEEE802.3], Section 53"

#define EDDS_LL_MAUTYPE_10GIGBASER		33
//  DESCRIPTION "R PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 49"

#define EDDS_LL_MAUTYPE_10GIGBASEER	34
//  DESCRIPTION "R fiber over 1550 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASELR	35
//  DESCRIPTION "R fiber over 1310 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASESR	36
//  DESCRIPTION "R fiber over 850 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASEW		37
//  DESCRIPTION "W PCS/PMA, unknown PMD."
//  REFERENCE   "[IEEE802.3], Section 49 and 50"

#define EDDS_LL_MAUTYPE_10GIGBASEEW	38
//  DESCRIPTION "W fiber over 1550 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASELW	39
//  DESCRIPTION "W fiber over 1310 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

#define EDDS_LL_MAUTYPE_10GIGBASESW	40
//  DESCRIPTION "W fiber over 850 nm optics"
//  REFERENCE   "[IEEE802.3], Section 52"

//------ new since RFC 3636:

#define EDDS_LL_MAUTYPE_10GIGBASECX4	41
//  DESCRIPTION "X copper over 8 pair 100-Ohm balanced cable"
//  REFERENCE   "[IEEE802.3], Section 54"

#define EDDS_LL_MAUTYPE_2BASETL		42
//  DESCRIPTION "Voice grade UTP copper, up to 2700m, optional PAF"
//  REFERENCE   "[IEEE802.3], Sections 61 and 63"

#define EDDS_LL_MAUTYPE_10BASETS 43
//  DESCRIPTION "Voice grade UTP copper, up to 750m, optional PAF"
//  REFERENCE   "[IEEE802.3], Sections 61 and 62"

#define EDDS_LL_MAUTYPE_100BASEBX10D	44
//  DESCRIPTION "One single-mode fiber OLT, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"


#define EDDS_LL_MAUTYPE_100BASEBX10U	45
//  DESCRIPTION "One single-mode fiber ONU, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"

#define EDDS_LL_MAUTYPE_100BASELX10	46
//  DESCRIPTION "Two single-mode fibers, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 58"

#define EDDS_LL_MAUTYPE_1000BASEBX10D	47
//  DESCRIPTION "One single-mode fiber OLT, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASEBX10U	48
//  DESCRIPTION "One single-mode fiber ONU, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASELX10	49
//  DESCRIPTION "Two sigle-mode fiber, long wavelength, 10km"
//  REFERENCE   "[IEEE802.3], Section 59"

#define EDDS_LL_MAUTYPE_1000BASEPX10D	50
//  DESCRIPTION "One single-mode fiber EPON OLT, 10km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX10U	51
//  DESCRIPTION "One single-mode fiber EPON ONU, 10km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX20D	52
//  DESCRIPTION "One single-mode fiber EPON OLT, 20km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_1000BASEPX20U	53
//  DESCRIPTION "One single-mode fiber EPON ONU, 20km"
//  REFERENCE   "[IEEE802.3], Section 60"

#define EDDS_LL_MAUTYPE_10GBASET		54
//  DESCRIPTION "Four-pair Category 6A or better, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 55"

#define EDDS_LL_MAUTYPE_10GBASELRM		55
//  DESCRIPTION "R multimode fiber over 1310 nm optics"
//  REFERENCE   "IEEE Std 802.3, Clause 68"

#define EDDS_LL_MAUTYPE_1000BASEKX		56
//  DESCRIPTION "X backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 70"

#define EDDS_LL_MAUTYPE_10GBASEKX4		57
//  DESCRIPTION "4 lane X backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 71"

#define EDDS_LL_MAUTYPE_10GBASEKR		58
//  DESCRIPTION "R backplane, full duplex mode only"
//  REFERENCE   "IEEE Std 802.3, Clause 72"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD1	59
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting low
//               power budget (PRX10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD2	60
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting
//              medium power budget (PRX20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXD3	61
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON OLT, supporting high
//              power budget (PRX30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU1	62
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting low
//              power budget (PRX10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU2	63
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting
//              medium power budget (PRX20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10G1GBASEPRXU3	64
//  DESCRIPTION "One single-mode fiber asymmetric-rate EPON ONU, supporting high
//              power budget (PRX30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD1		65
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting low
//              power budget (PR10)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD2		66
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting
//              medium power budget (PR20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRD3		67
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON OLT, supporting high
//              power budget (PR30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRU1		68
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON ONU, supporting
//              low and medium power budget (PR10 and PR20)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_10GBASEPRU3		69
//  DESCRIPTION "One single-mode fiber symmetric-rate EPON ONU, supporting high
//              power budget (PR30)"
//  REFERENCE   "IEEE Std 802.3, Clause 75"

#define EDDS_LL_MAUTYPE_40GBASEKR4		70
//    DESCRIPTION "40GBASE-R PCS/PMA over an electrical
//                 backplane"
//    REFERENCE   "IEEE Std 802.3, Clause 84"

#define EDDS_LL_MAUTYPE_40GBASECR4		71
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 lane shielded
//                 copper balanced cable"
//    REFERENCE   "IEEE Std 802.3, Clause 85"

#define EDDS_LL_MAUTYPE_40GBASESR4		72
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 lane multimode
//                 fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 86"

#define EDDS_LL_MAUTYPE_40GBASEFR		73
//    DESCRIPTION "40GBASE-R PCS/PMA over single mode
//                 fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 89"

#define EDDS_LL_MAUTYPE_40GBASELR4		74
//    DESCRIPTION "40GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 87"

#define EDDS_LL_MAUTYPE_100GBASECR10	75
//    DESCRIPTION "100GBASE-R PCS/PMA over 10 lane
//                 shielded copper balanced cable"
//    REFERENCE   "IEEE Std 802.3, Clause 85"

#define EDDS_LL_MAUTYPE_100GBASESR10	76
//    DESCRIPTION "100GBASE-R PCS/PMA over 10 lane
//                 multimode fiber"
//    REFERENCE   "IEEE Std 802.3, Clause 86"

#define EDDS_LL_MAUTYPE_100GBASELR4		77
//    DESCRIPTION "100GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber, long reach"
//    REFERENCE   "IEEE Std 802.3, Clause 88"

#define EDDS_LL_MAUTYPE_100GBASEER4		78
//    DESCRIPTION "100GBASE-R PCS/PMA over 4 WDM lane
//                 single mode fiber PMD, extended reach"
//    REFERENCE   "IEEE Std 802.3, Clause 88"

/*****************************************************************************/
/*                                                                           */
/* Output-functions are in edd_out.h                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  end of file EDDS_SYS.H                                                   */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_SYS_H */

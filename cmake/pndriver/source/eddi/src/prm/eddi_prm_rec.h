#ifndef EDDI_PRM_REC_H          //reinclude-protection
#define EDDI_PRM_REC_H

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
/*  F i l e               &F: eddi_prm_rec.h                            :F&  */
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
/*  06.02.15    TH    refactor ADJUST_PORT_STATE to ADJUST_LINK_STATE        */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  defines                                                                  */
/*****************************************************************************/

#define EDDI_RPM_INDEX_RANGE_B_LOW                                      0xB000   // lower range 
#define EDDI_RPM_INDEX_RANGE_B_HIGH                                     0xBFFF   // higher range 

/*****************************************************************************/
/*  block type                                                               */
/*****************************************************************************/

#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST                         (EDDI_NTOHS(0x0202))
#define  EDDI_PRM_BLOCKTYPE_PD_SYNC_DATA                                (EDDI_NTOHS(0x0203))
#define  EDDI_PRM_BLOCKTYPE_PDIR_HEADER_DATA                            0x0205
#define  EDDI_PRM_BLOCKTYPE_PDIR_GLOBAL_DATA                            0x0206
#define  EDDI_PRM_BLOCKTYPE_PDIR_FRAME_DATA                             0x0207
#define  EDDI_PRM_BLOCKTYPE_PDIR_BEGIN_END_DATA                         0x0208
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY         (EDDI_NTOHS(0x0209))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MAU_TYPE                (EDDI_NTOHS(0x020E))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_MULTICAST_BOUNDARY      (EDDI_NTOHS(0x0210))
#define  EDDI_PRM_BLOCKTYPE_PD_INTERFACE_MRP_DATA_ADJUST                (EDDI_NTOHS(0x0211))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_LINK_STATE              (EDDI_NTOHS(0x021B))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PEER_TP_PEER_BOUNDARY   (EDDI_NTOHS(0x0224))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_DCP_BOUNDARY            (EDDI_NTOHS(0x0225))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH         (EDDI_NTOHS(0x0226))
#define  EDDI_PRM_BLOCKTYPE_PDNC_DATA_CHECK                             (EDDI_NTOHS(0x0230))
#define  EDDI_PRM_BLOCKTYPE_PD_CONTROL_PLL                              (EDDI_NTOHS(0xB050))
#define  EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_CONTROL                       (EDDI_NTOHS(0xB060))
#define  EDDI_PRM_BLOCKTYPE_PD_TRACE_UNIT_DATA                          (EDDI_NTOHS(0xB061))
#define  EDDI_PRM_BLOCKTYPE_PDSCF_DATA_CHECK                            (EDDI_CONST_NTOHS(F0,00))
#define  EDDI_PRM_BLOCKTYPE_PD_SET_DEFAULT_PORT_STATES                  (EDDI_CONST_NTOHS(F0,01))
#define  EDDI_PRM_BLOCKTYPE_PD_PORT_STATISTIC                           0x0251
#define  EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_DATA                          0x022A
#define  EDDI_PRM_BLOCKTYPE_PDIR_SUBFRAME_BLOCK                         0x022B
#define  EDDI_PRM_BLOCKTYPE_PDIR_APPLICATION_DATA                       0xF002
#define  EDDI_PRM_BLOCKTYPE_PDNRT_FILL                                  (EDDI_NTOHS(0xF004))

/*****************************************************************************/
/*  block version                                                            */
/*****************************************************************************/

#define  EDDI_PRM_BLOCK_VERSION_HIGH                                     0x01              /* BlockVersionHigh - Standard             */
#define  EDDI_PRM_BLOCK_VERSION_LOW                                      0x00              /* BlockVersionLow  - Standard             */
#define  EDDI_PRM_STATISTIC_BLOCK_VERSION_LOW                            0x01              /* BlockVersionLow  - Statistic Counter    */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V10                 0x00              /* BlockVersionLow  - PDIRFrameData V1.0   */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_FRAME_DATA_V11                 0x01              /* BlockVersionLow  - PDIRFrameData V1.1   */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_DATA                           0x01              /* BlockVersionLow  - PDIRData             */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA                    0x01              /* BlockVersionLow  - PDIRGlobalData       */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_IR_GLOBAL_DATA_V12                0x02              /* BlockVersionLow  - PDIRGlobalData       */
#define  EDDI_PRM_BLOCK_VERSION_LOW_PD_SYNC_DATA                         0x02              /* BlockVersionLow  - PDSyncData           */
#define  EDDI_PRM_BLOCK_VERSION_LOW_ADJUST_DOMAIN_BOUNDARY               0x01              /* BlockVersionLow  - AdjustDomainBoundary */

/*****************************************************************************/
/*  PDNCDataCheck                                                            */
/*****************************************************************************/

#define  EDDI_PRM_DEF_MASK_DROP_BUDGET_VALUE                             0x7FFFFFFFUL      /* Value */
#define  EDDI_PRM_DEF_MASK_DROP_BUDGET_CHECK                             0x80000000UL      /* Check - Bit */

/*****************************************************************************/
/*  PDControlPLL                                                             */
/*****************************************************************************/

#define  EDDI_PRM_PDIRDATA_LENGTH_GLOBAL_EXT_WITHOUT_PORT                8UL               /* 8 */

#define  EDDI_PRM_xRT_TAG                                                0x8892            /* RT -Tag */

#define  EDDI_PRM_MAX_BRIDGE_DELAY                                       0x3B9AC9FF        /* max. BridgeDelay */
#define  EDDI_PRM_MAX_PORT_RX_DELAY                                      0x3B9AC9FF        /* max. MaxPortRxDelay */
#define  EDDI_PRM_MAX_PORT_TX_DELAY                                      0x3B9AC9FF        /* max. MaxPortTxDelay */

#if defined (EDDI_CFG_REV5)
#define  EDDI_PRM_IRTE_IP_BUG_REV5_DEBUG_VERSION                         0x0               /* REV5 IRTE-IP (Bug) */
#elif defined (EDDI_CFG_REV6)
#define  EDDI_PRM_IRTE_IP_BUG_REV6_DEBUG_VERSION                         0x1               /* REV6 IRTE-IP (Bug) */
#endif

/*****************************************************************************/
/*  PDSyncTraceRTCData - diag sources                                        */
/*****************************************************************************/

//#define  EDDI_DIAG_SOURCE_EXTPLL                                       0UL               /* ExtPllOffset */
//#define  EDDI_DIAG_SOURCE_OFFSET                                       1UL               /* LocalOffset */
//#define  EDDI_DIAG_SOURCE_ADJUST                                       2UL               /* Adjust */
//#define  EDDI_DIAG_SOURCE_DRIFT                                        3UL               /* Drift */
//#define  EDDI_DIAG_SOURCE_DELAY                                        4UL               /* LineDelay and PortId */
//#define  EDDI_DIAG_SOURCE_SET                                          5UL               /* Hard Setting */

/*****************************************************************************/
/*  PDSyncData                                                               */
/*****************************************************************************/

#define  EDDI_PRM_PDSYNCDATA_PTCP_SUBDOMAIN_NAME_LENGTH_MAX             240UL             /* max. Length of PTCPSubdomainName */
#define  EDDI_PRM_PDSYNCDATA_PTCP_TIMEOUT_FACTOR_MAX                    0x200UL           /* PTCP_TimeoutFactor = PTCP_TimeoutInterval / CycleLength  */
/* Example: Redundanter Syncmaster light needs PTCP_TimeoutInterval of 192 ms  */
/*          ==>  max. PTCP_TimeoutFactor = 192ms / 250us = 768                 */

#define  EDDI_PRM_PDSYNCDATA_SYNC_ID_MAX                                0x1FUL            /* max. SyncID */
#define  EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MIN                       0x00000001UL      /* min. SendclockFactor */
#define  EDDI_PRM_PDSYNCDATA_SENDCLOCK_FACTOR_MAX                       0x00000080UL      /* max. SendClockFactor */

/* PTCP_MasterPriority1*/
#define  EDDI_PRM_PDSYNCDATA_PTCP_SLAVE                                 0x00              /* Primary Slave    */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER                        0x01              /* Primary Master*/
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER                      0x02              /* Secondary Master*/

#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_1                0x09              /* Primary Master Level 1 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_2                0x11              /* Primary Master Level 2 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_3                0x19              /* Primary Master Level 3 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_4                0x21              /* Primary Master Level 4 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_5                0x29              /* Primary Master Level 5 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_6                0x31              /* Primary Master Level 6 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_PRIMARY_MASTER_LEVEL_7                0x39              /* Primary Master Level 7 */

#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_1              0x0A              /* Secondary Master Level 1 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_2              0x12              /* Secondary Master Level 2 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_3              0x1A              /* Secondary Master Level 3 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_4              0x22              /* Secondary Master Level 4 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_5              0x2A              /* Secondary Master Level 5 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_6              0x32              /* Secondary Master Level 6 */
#define  EDDI_PRM_PDSYNCDATA_PTCP_SECONDARY_MASTER_LEVEL_7              0x3A              /* Secondary Master Level 7 */

/* PTCP_MasterPriority2*/
#define  EDDI_PRM_PDSYNCDATA_PTCP_MASTERPRIORITY2_DEFAULT               0xFF              /* Default MasterPriority2*/

/* PTCPTakeoverTimeoutFactor*/
#define  EDDI_PRM_PDSYNCDATA_PTCP_TAKEOVER_TIMEOUT_FACTOR_MAX           0x200             /* max. PTCPTakeoverTimeoutFactor*/

/* PTCPMasterStartupTime*/
#define  EDDI_PRM_PDSYNCDATA_PTCP_MASTER_STARTUP_TIME_MAX               0x12C             /* max. PTCPMasterStartupTime*/

#define  S_PRM_RECORD_PROPERTIES_BIT__ROLE                              EDDI_BIT_MASK_PARA( 1, 0)
#define  S_PRM_RECORD_PROPERTIES_BIT__SYNC_ID                           EDDI_BIT_MASK_PARA(12, 8)

/*****************************************************************************/
/*  PDPortDataAdjust                                                         */
/*****************************************************************************/

#define EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_EMPTY                        12UL    /* 12 */
#define EDDI_PRM_PDPORT_DATA_ADJUST_LENGTH_MAX                          104UL   /* 104 */

#define EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PORTE_STATE_DOWN              0x0002

/*****************************************************************************/
/*  PDInterfaceMrpDataAdjust                                                 */
/*****************************************************************************/

#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_LENGTH_EMPTY               12UL    /* 12 */
#if defined (EDDI_CFG_BIG_ENDIAN)
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_DISABLED              0x0000U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_CLIENT                0x0001U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_MANAGER               0x0002U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_AUTOMANAGER           0x0004U
#else
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_DISABLED              0x0000U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_CLIENT                0x0100U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_MANAGER               0x0200U
#define EDDI_PRM_PDINTERFACE_MRP_DATA_ADJUST_ROLE_AUTOMANAGER           0x0400U
#endif

/*****************************************************************************/
/*  PDTraceUnitControl                                                       */
/*****************************************************************************/

#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_DATA                         0x00000001
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_TIME                         0x00000002
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_SUBSTITUTE                   0x00000004
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_LATE                     0x00000008
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NRT                      0x00000010
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_MISSING                  0x00000020
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_SRT                      0x00000040
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_UNEXPECTED               0x00000080
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_CRC                      0x00000100
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_DATA_LOST                0x00000200
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_LENGTH                   0x00000400
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_ACW                   0x00000800
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_TABLE_ENTRY           0x00001000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_BUFFER_OVERFLOW          0x00002000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_NO_ACW                   0x00004000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_NO_DATA                  0x00008000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_LATE                     0x00010000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_IP_HEADER_FRAME_LENGTH_ERR    0x00020000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_UDP_HEADER_FRAME_LENGTH_ERR   0x00040000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_IP_HEADER_CS_ERR              0x00080000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_IP_HEADER_SRC_IP_ERR          0x00100000
#define EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_ENABLE_UDP_HEADER_SRC_PORT_ERR       0x00200000

//value automatically set by EDDI if no record is written
#define EDDI_PRM_PDTRACE_UNIT_DEFAULT_SETUP (  EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_SUBSTITUTE      \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_LATE         \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NRT          \
                                             + /* EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_MISSING */     \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_SRT         \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_UNEXPECTED   \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_CRC          \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_DATA_LOST     \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_LENGTH       \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_ACW        \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_NO_TABLE_ENTRY    \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_RCV_ERR_BUFFER_OVERFLOW      \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_SRT_RCV_ERR_NO_ACW        \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_NO_DATA       \
                                             + EDDI_PRM_PDTRACE_UNIT_CONTROL_DIAG_IRT_SND_ERR_LATE )

/*****************************************************************************/
/*  data length, block length                                                */
/*****************************************************************************/
#define  EDDI_PRM_BLOCK_WITHOUT_LENGTH                          4UL /* sizeof(BlockType) + sizeof(BlockLength)*/

#define  EDDI_PRM_LENGTH_PDNC_DATA_CHECK                        20UL
#define  EDDI_PRM_H_BL_PDNC_DATA_CHECK                          EDDI_CONST_NTOHS(00,10)   //16                                                       

#if defined (EDDI_CFG_TRACE_MODE) && ((EDDI_CFG_TRACE_MODE == 1) || (EDDI_CFG_TRACE_MODE == 2)) //LSA-Trace
#define  EDDI_PRM_H_BL_PDPORT_DATA_ADJUST                       EDDI_CONST_NTOHS(00,30)   //48
#endif

#define  EDDI_PRM_LENGTH_PDSYNC_DATA_MAX                        296UL
#define  EDDI_PRM_LENGTH_PDSYNC_DATA_FIX                        53UL

#define  EDDI_PRM_LENGTH_PDIR_GLOBAL_DATA                       24UL
#define  EDDI_PRM_H_BL_PDIR_GLOBAL_DATA                         EDDI_CONST_NTOHS(00,14)   //20

#define  EDDI_PRM_H_BL_PD_PORT_DTATA_ADJUST_MULTICAST_BOUNDARY  EDDI_CONST_NTOHS(00,0C)   //12
#define  EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DOMAIN_BOUNDARY      EDDI_CONST_NTOHS(00,10)   //16
#define  EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_DCP_BOUNDARY         EDDI_CONST_NTOHS(00,0C)   //12
#define  EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_PREAMBLE_LENGTH      EDDI_CONST_NTOHS(00,08)   //8
#define  EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_LINK_STATE           EDDI_CONST_NTOHS(00,08)   //8
#define  EDDI_PRM_H_BL_PD_PORT_DATA_ADJUST_MAU_TYPE             EDDI_CONST_NTOHS(00,08)   //8

#define  EDDI_PRM_LENGTH_FRAME_DATA                             20UL

#define  EDDI_PRM_LENGTH_PDIR_HEADER_DATA                       12UL

#define  EDDI_PRM_LENGTH_PD_TRACE_UNIT_CONTROL                  16UL
#define  EDDI_PRM_H_BL_PD_TRACE_UNIT_CONTROL                    EDDI_CONST_NTOHS(00,0C)   //12
#define  EDDI_PRM_HEADER_BL_PD_TRACE_UNIT_CONTROL               12

#define  EDDI_PRM_LENGTH_PD_TRACE_UNIT_DATA_ENTRY               24UL
#define  EDDI_PRM_ENTRY_PD_TRACE_UNIT_DATA_EMPTY                16UL
#define  EDDI_PRM_LENGTH_PD_TRACE_UNIT_DATA                     (EDDI_PRM_ENTRY_PD_TRACE_UNIT_DATA_EMPTY + (EDDI_PRM_LENGTH_PD_TRACE_UNIT_DATA_ENTRY * EDDI_PRM_MAX_ENTRY_PDTRACE_UNIT_DATA))

#define  EDDI_PRM_LENGTH_PDSCF_DATA_MIN_CHECK                   12UL
#define  EDDI_PRM_LENGTH_PDSCF_DATA_MAX_CHECK                   44UL

#define  EDDI_PRM_H_BL_PDSCF_DATA_CHECK                         EDDI_CONST_NTOHS(00,04)   //4

#define  EDDI_PRM_LENGTH_PD_CONTROL_PLL                         12UL
#define  EDDI_PRM_H_BL_PD_CONTROL_PLL                           EDDI_CONST_NTOHS(00,08)   //8
#define  EDDI_PRM_HEADER_BL_PD_CONTROL_PLL                      8   
                                                               
#define  EDDI_PRM_LENGTH_SYNC_DIAG_ENTRY                        80UL
#define  EDDI_PRM_LENGTH_PD_SYNC_TRACE_RTC_DATA                 (EDDI_PRM_LENGTH_SYNC_DIAG_ENTRY * EDDI_PRM_MAX_ENTRY_SyncTraceRTCData)

#define  EDDI_PRM_PDPORT_STATISTIC_RECORD_LENGTH                32

#define  EDDI_PRM_PDNRT_FILL_RECORD_LENGTH                      12UL
#define  EDDI_PRM_HEADER_BL_PDNRT_FILL                          8   

/*****************************************************************************/
/*  PDIRData                                                                 */
/*****************************************************************************/
#define EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_128B                  (128 * 80) + (22 * 80) //12000 ns
#define EDDI_PRM_PDIR_MAX_GREEN_BANDWIDTH_256B                  (256 * 80) + (22 * 80) //34240 ns

/*****************************************************************************/
/*  typedefs                                                                 */
/*****************************************************************************/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PRM_REC_H


/*****************************************************************************/
/*  end of file eddi_prm_rec.h                                               */
/*****************************************************************************/

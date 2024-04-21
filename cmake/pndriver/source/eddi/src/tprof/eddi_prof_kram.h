#ifndef EDDI_PROF_KRAM_H        //reinclude-protection
#define EDDI_PROF_KRAM_H

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
/*  F i l e               &F: eddi_prof_kram.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  23.10.07    JS    reduced reserved field from PROF_KRAM_INFO.            */
/*                    PROF_KRAM_SIZE changed from 900 to 320                 */
/*****************************************************************************/

#define PROF_MODE_NOT_ACTIVATED  0x44556677

typedef struct _PROF_EDDI_SERCMD_INFO
{
    LSA_UINT32                    FCodeMaxDelay[0x17+1];        //Max. synchronous command-runtime in 10ns-units
    LSA_UINT32                    FCodeCurDelay[0x17+1];        //Cur. synchronous command-runtime in 10ns-units
    LSA_UINT32                    FCodeMaxDelayAsync[0x17+1];  //Max. asynchronous command-runtime in 10ns-units
    LSA_UINT32                    FCodeCurDelayAsync[0x17+1];  //Cur. asynchronous command-runtime in 10ns-units
    //PROF_EDDI_SERCMD_ENTRY      entry[PROF_MAX_SERCMD_ENTRIES];

} PROF_EDDI_SERCMD_INFO;

typedef struct _PROF_EDDI_EXCP_INFO
{
    /* Offset */
    /* 000 */
    LSA_UINT32       Line;
    /* 004 */
    LSA_UINT32       ModuleID;
    /* 008 */
    LSA_UINT32       Error;
    /* 012 */
    LSA_UINT32       DW_0;
    /* 016 */
    LSA_UINT32       DW_1;
    /* 020 */
    LSA_UINT32       acw_base_adr;
    /* 024 */

} PROF_EDDI_EXCP_INFO;

typedef struct _EDDI_KRAM_RES_POOL_TYPE
{
    LSA_UINT32       acw_snd;
    LSA_UINT32       acw_rcv;

    LSA_UINT32       irt_noop;
    LSA_UINT32       irt_eol;

    LSA_UINT32       irt_port_start_time_tx_block;  //used for EDDI_RED_PHASE_SHIFT_ON
    LSA_UINT32       irt_port_start_time_rx_block;  //used for EDDI_RED_PHASE_SHIFT_ON

    LSA_UINT32       fcw_snd;
    LSA_UINT32       fcw_rcv;
    LSA_UINT32       fcw_rcv_forwarder;

    LSA_UINT32       sync_frame;

    LSA_UINT32       bcw_mod;
    LSA_UINT32       rcw;
    LSA_UINT32       srt_noop;
    LSA_UINT32       srt_eol_and_root_acw;

} EDDI_KRAM_RES_POOL_TYPE;

typedef struct _EDDI_PROF_KRAM_RES_TYPE             //ATTENTION: DO NOT CHANGE STRUCTURE!
{
    LSA_UINT32                  UsedBytes;

    LSA_UINT32                  FreeBytes;
    LSA_UINT32                  ProcessImage;
    LSA_UINT32                  PABorderLine;

    LSA_UINT32                  DCP_FilterStringBufLen;   //length in bytes
    LSA_UINT32                  DCP_FilterTableLen;       //length in bytes
    LSA_UINT32                  ArpFilterTableLen;        //length in bytes
    LSA_UINT32                  ArpDcpCCWLen;             //length in bytes

    LSA_UINT32                  TimeMasterListLen;        //length in bytes

    LSA_UINT32                  AcwHead;

    LSA_UINT32                  NrtCTRL;
    LSA_UINT32                  UCMCTable;
    LSA_UINT32                  MCTable;
    LSA_UINT32                  VlanTable;
    LSA_UINT32                  Statistic;
    LSA_UINT32                  SrtFcw;
    LSA_UINT32                  ScoreBoard;
    LSA_UINT32                  IrtBase;

    LSA_UINT32                  Nrt_DB_FCW;
    LSA_UINT32                  Trace;
    LSA_UINT32                  TraceTCW;

    EDDI_KRAM_RES_POOL_TYPE     pool;

} EDDI_PROF_KRAM_RES_TYPE;

typedef struct _EDDI_PROF_NRAM_RES_TYPE
{
    LSA_UINT32                  NRT_SDRAM;
    LSA_UINT32                  NRT_SHARED_MEM;

    LSA_UINT32                  LocalMem;

} EDDI_PROF_NRAM_RES_TYPE;

#define EDDI_MAX_CNT_CYCLE_LOST_ARRAY   32

//temporarily_disabled_lint -esym(768, _PROF_EDDI_INT_STATS_INFO::reserved*)
typedef struct _PROF_EDDI_INT_STATS_INFO
{
    /* Offset */
    /* 000 */
    LSA_UINT32           outOfSyncTimestamp;      /*!< Clock count at which IRT Out Of Sync was detected. */
    /* 004 */
    LSA_UINT32           outOfSyncReason;         /*!< Reason code for last Out Of Sync. */

    /*!< Ring-buffer with the timestamps of the last XXX NewCycle calls. */
    /* 008 */
    LSA_UINT32           newCycleHistoryBuffer[EDDI_MAX_CNT_CYCLE_LOST_ARRAY];
    /*!< Head of the Queue newCycleHistoryBuffer[]. */
    /* 136 */
    LSA_UINT8            firstBufferElem;

    /*!< Padding to 4Byte alignment (not referenced). */
    /* 137 */
    LSA_UINT8            reserved;
    /* 138 */
    LSA_UINT8            reserved_1;
    /* 139 */
    LSA_UINT8            reserved_2;
    /* 140 */

} PROF_EDDI_INT_STATS_INFO;

typedef struct _PROF_EDDI_PARA_INFO
{
    /* Offset */
    /* 000 */
    LSA_UINT32                      PHY_Basic_Control_Register[EDDI_MAX_IRTE_PORT_CNT];
    /* 016 */
    LSA_UINT8                       PHY_LinkStatus[EDDI_MAX_IRTE_PORT_CNT];
    /* 020 */
    LSA_UINT8                       PHY_LinkMode[EDDI_MAX_IRTE_PORT_CNT];
    /* 024 */
    LSA_UINT8                       PHY_LinkSpeed[EDDI_MAX_IRTE_PORT_CNT];
    /* 028 */

} PROF_EDDI_PARA_INFO;

/**
 * Image of the structure EDDI_DPB_TYPE & EDDI_KRAM_TYPE.
 * Needs to be altered if there are any changes.
 * Used to prevent misalignment on different systems.
 * Size 15 Byte.
 */
//temporarily_disabled_lint -esym(768, _PROF_EDDI_DPB_INFO::dummy0)
//temporarily_disabled_lint -esym(768, _PROF_EDDI_DPB_INFO::dummy1)
//temporarily_disabled_lint -esym(768, _PROF_EDDI_DPB_INFO::dummy2)
//temporarily_disabled_lint -esym(768, _PROF_EDDI_DPB_INFO::dummy3)
//temporarily_disabled_lint -esym(768, _PROF_EDDI_DPB_INFO::dummy4)
typedef struct _PROF_EDDI_DPB_INFO
{
    /* Offset */
    /* 000 */
    LSA_UINT32            InterfaceID;
    /* 004 */
    LSA_UINT32            dummy0;
    /* 008 */
    LSA_UINT32            IRTE_SWI_BaseAdr;
    /* 012 */
    LSA_UINT32            IRTE_SWI_BaseAdr_LBU_16Bit;
    /* 016 */
    LSA_UINT32            BOARD_SDRAM_BaseAddr;
    /* 020 */
    LSA_UINT32            dummy1;
    /* 024 */
    LSA_UINT32            dummy2;
    /* 028 */
    LSA_UINT32            dummy3;
    /* 032 */
    LSA_UINT32            dummy4;
    /* 036 */
    LSA_UINT32            ERTECx00_SCRB_BaseAddr;
    /* 040 */
    LSA_UINT32            PAEA_BaseAdr;
    LSA_UINT32            KRam_offset_ProcessImageEnd;
    /* 048 */
    LSA_UINT32            KRam_size_reserved_for_eddi;
    /* 052 */
    LSA_UINT32            EnableReset;
    /* 056 */

} PROF_EDDI_DPB_INFO;

/**
 * Pendant to the LSA_VERSION-struct. Introduced due to Padding-topic
 * @see LSA_VERSION
 */
#define EDDI_LSA_PREFIX_SIZE             16
typedef struct _PROF_EDDI_LSA_VERS
{
    /* Offset */
    /* 000 */
    LSA_UINT8   lsa_prefix[EDDI_LSA_PREFIX_SIZE];
    /* 016 */
    LSA_UINT16  lsa_component_id;
    /* 018 */
    LSA_UINT8   kind;
    /* 019 */
    LSA_UINT8   version;
    /* 020 */
    LSA_UINT8   distribution;
    /* 021 */
    LSA_UINT8   fix;
    /* 022 */
    LSA_UINT8   hotfix;
    /* 023 */
    LSA_UINT8   project_number;
    /* 024 */
    LSA_UINT8   increment;
    /* 025 */
    LSA_UINT8   integration_counter;
    /* 026 */
    LSA_UINT8   gen_counter;

    /*!< Padding to 4-Byte border (not referenced) */
    /* 027 */
    LSA_UINT8   reserved;
    /* 028 */

} PROF_EDDI_LSA_VERS;

#if defined (EDDI_SER_CMD_STAT)
#define PROF_KRAM_SIZE_SERCMD                          384     /*!< Size in Bytes of the SERCMD substructure. */
#else
#define PROF_KRAM_SIZE_SERCMD                          0
#endif
#define PROF_KRAM_SIZE                                 320     /*!< Size in Bytes of the PROF_KRAM_INFO structure. */
#define PROF_KRAM_OFFSET_PROF_EDDI_DPB_INFO            12 
#define PROF_KRAM_OFFSET_PROF_EDDI_EXCP_INFO           68 
#define PROF_KRAM_OFFSET_PROF_EDDI_PARA_INFO           92 
#define PROF_KRAM_OFFSET_PROF_EDDI_INT_STATS_INFO      120
#define PROF_KRAM_OFFSET_PROF_EDDI_LSA_VERS            260
#define PROF_KRAM_OFFSET_SWI_AGEPOLL_TIME              288
#define PROF_KRAM_OFFSET_TICKS_TILL_PHYRESET           292
#define PROF_KRAM_OFFSET_SER_CMD_SIZE                  296

//////////////////////////////////////////////////////////////////////////
//
//  ATTENTION .. ATTENTION .. ATTENTION .. ATTENTION .. ATTENTION ..
//
//  Most compilers behave different when it comes to placing structs/arrays
//  in structs. As we need a consistent IRTE memory dump there have to be
//  fixed offset boundaries for each sub-object. Therefore a few rules
//  should be obeyed:
//
//  1. Due to the fact that some compilers set structs and arrays only to
//     4Byte addresses the layout should look like this:
//
//     OFFSET  TYPE         NAME
//     -------------------------
//     0x0000  LSA_UINT32   x
//     0x0004  LSA_UINT32   y[10]
//
//     Don't do something like this:
//
//     OFFSET  TYPE         NAME
//     -------------------------
//     0x0000  LSA_UINT16   x
//     0x0002  LSA_UINT32   y[10]
//
//     In the latter case consider a redesign of the structure, or fill the
//     offset-gap with a placeholder like this:
//
//     OFFSET  TYPE         NAME
//     -------------------------
//     0x0000  LSA_UINT16   x
//     0x0002  LSA_UINT16   placeholder
//     0x0004  LSA_UINT32   y[10]
//
//     Remember that the same applies to structs.
//
//  2. Some compilers fill arrays and structs with padding bytes to reach
//     a 4-Byte boarder at the end. This packed Bytes can be placed in the
//     scope of the actual element just as well on the upper-level. This
//     depends on the compiler and can't be predicted.
//     Therefore each struct should be a multiple of 4Byte. If you have
//     something like this:
//
//     OFFSET  TYPE         NAME
//     -------------------------
//     0x0000  LSA_UINT32   x
//     0x0004  struct       structure
//
//     The struct structure should look like:
//
//     OFFSET  TYPE         NAME
//     -------------------------
//     0x0000  LSA_UINT16   x
//     0x0002  LSA_UINT8    z
//     0x0003  LSA_UINT8    placeholder   <-- included to reach 4Byte boarder
//
//  3. Avoid the usage of LSA_BOOL due to the fact that it can't be
//     determined which size it has.
//
//////////////////////////////////////////////////////////////////////////

//temporarily_disabled_lint -esym(768, _PROF_KRAM_INFO::reserved_0)
typedef struct _PROF_KRAM_INFO
{
    /* Offset */
    #if defined (EDDI_SER_CMD_STAT)
    /* -384*/ PROF_EDDI_SERCMD_INFO         SerCmdInfo;
    #endif
    /* 000 */ 
    LSA_UINT32                              mode;
    /* 004 */
    LSA_UINT32                              offset;
    /* 008 */
    LSA_UINT32                              ram_type;
    /* 012 */
    PROF_EDDI_DPB_INFO                      DPB;
    /* 068 */
    PROF_EDDI_EXCP_INFO                     Excp;
    /* 092 */
    PROF_EDDI_PARA_INFO                     info;
    /* 120 */
    PROF_EDDI_INT_STATS_INFO                intStats;
    /* 260 */
    PROF_EDDI_LSA_VERS                      eddi_lsa_vers;
    /* 288 */
    LSA_UINT16                              Swi_AgePollTime;
    /* 290 */
    LSA_UINT16                              reserved_0;
    /* 292 */
    LSA_UINT32                              Ticks_till_PhyReset;
    /* 296 */
    LSA_UINT32                              SerCmdSize;
    /*!< Padding-field to reach PROF_KRAM_SIZE Border (not referenced). */
    /* 300 */
    LSA_UINT32                              reserved_1[5];
    /* 320  == PROF_KRAM_SIZE */

} PROF_KRAM_INFO;

#define PROF_INVALID_INDEX    0xaaaa

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PROF_KRAM_H


/*****************************************************************************/
/*  end of file eddi_prof_kram.h                                             */
/*****************************************************************************/

#ifndef EDDI_SER_TYP_H          //reinclude-protection
#define EDDI_SER_TYP_H

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
/*  F i l e               &F: eddi_ser_typ.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:   Switch-ASIC-Structs                             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*****************************************************************************/

#include "pnio_pck4_on.h"
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_UINT64_TYPE
{
    LSA_UINT32   U32_0;
    LSA_UINT32   U32_1;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_UINT64_TYPE EDDI_SER10_UINT64_TYPE;

#include "pnio_pck_off.h"

#include "eddi_ser_acw_fcw.h"

#if defined (EDDI_CFG_REV5)
#include "eddi_ser_typ_r5.h"
#elif defined (EDDI_CFG_REV6)
#include "eddi_ser_typ_r6.h"
#elif defined (EDDI_CFG_REV7)
#include "eddi_ser_typ_r7.h"
#else
#error EDDI_CFG_ERROR: Revision not defined !
#endif

/*===========================================================================*/
/*       Global-Device-Defines                                               */
/*===========================================================================*/

//+---------------------------------------------------------------------------
//|  defines for IRT-Timing
//+---------------------------------------------------------------------------

#define EDDI_MIN_FCW_START_TIME_NS   5000 // 5 usec, earliest possible FCW send/receive time, used for checks
#define EDDI_ROOT_ACW_LEAD_TIME_NS   2500 // 2,5 usec, leadtime for Root-ACW in ACW-Tx-List

//PreFrameSendDelay [ns]: Delay-Time between Transfer-Unit and Tx-Timestamp-Unit
#if defined (EDDI_CFG_ERTEC_200)
//Transfer-Unit and Tx-Timestamp-Unit have different HW-positions.
#define EDDI_PREFRAME_SEND_DELAY_100MBIT_NS  1030UL
#elif defined (EDDI_CFG_ERTEC_400)
//Transfer-Unit and Tx-Timestamp-Unit are located at the same HW-position.
#define EDDI_PREFRAME_SEND_DELAY_100MBIT_NS  40UL
#elif defined (EDDI_CFG_SOC)
//Transfer-Unit and Tx-Timestamp-Unit have different HW-positions.
#define EDDI_PREFRAME_SEND_DELAY_100MBIT_NS  1030UL
#endif

//+---------------------------------------------------------------------------
//|  defines for Control-Words
//+---------------------------------------------------------------------------

#define SER10_NULL_PTR              0x001FFFFFUL

#define SER10_NULL_PTR_SWAP         EDDI_PASTE_32(00,1F,FF,FF)

#define SER10_NULL_PTR_12_0         0x00001FFFL
#define SER10_NULL_PTR_20_13        0x000000FFL

#define SER10_NULL_PTR_HELP         (0x001FFFFFUL << 11UL)  //Help-Pointer for CW_Link, do not change!

//#define SER10_PTR_VALID           0x80000000L

#define SER10_PTR_VALID_SWAP        EDDI_PASTE_32(80,00,00,00)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
// Needed for RCW-Tree using FCODE_INSERT_ACW_TX FCODE_REMOVE_ACW_TX Commands
#define SER10_PTR_LIST_HEAD_MARKER  (1UL << 21UL)
#endif

#define SER10_NRT_DB_SIZE           0x80

// Opcodes for IRTE-ControlWords
#define SER_IRT_DATA                0x00
//#define SER_SRT_DATA              0x01
//#define SER_NRT_DATA              0x02
#define SER_ACW_TX                  0x04
#define SER_ACW_RX                  0x05

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define SER_RCW                     0x06
#endif

#define SER_NOOP                    0x07
#define SER_LMW                     0x08
//#define SER_BMW                   0x09
#if defined (EDDI_CFG_REV5)
#define SER_BCW_MOD                 0x0A
#endif

#define SER_SOC_ID                  0x0B
#define SER_SOL                     0x0C
//#define SER_BCW_ID                0x0D
#define SER_EOC_ID                  0x0E
#define SER_EOL                     0x0F

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
// Not a real HW-Opcode !
// This "virtual" Opcode is used to specify RCW-Phase-List-Header
// in EDDI_TREE_ELEM->Opc
#define SER_RCW_PHS                0x10
#endif

// SER Statistic Port spezific mem space
#define SER_STATISTIC_PORT_SIZE     128
#define SER_STATISTIC_BASE_OFFSET   256

#include "pnio_pck4_on.h"
/*===========================================================================*/
/*========================= EDDI_SER_NRT_FCW_TYPE ===========================*/
/*===========================================================================*/
#define EDDI_SER_NRT_LL0_FCW_BIT__Opc                        EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER_NRT_LL0_FCW_BIT__RcvPort                    EDDI_BIT_MASK_PARA( 7, 4)
#define EDDI_SER_NRT_LL0_FCW_BIT__Valid                      EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_NRT_LL0_FCW_BIT__Reserved1                  EDDI_BIT_MASK_PARA(12, 9)
#define EDDI_SER_NRT_LL0_FCW_BIT__FrameId                    EDDI_BIT_MASK_PARA(28,13)
#define EDDI_SER_NRT_LL0_FCW_BIT__Len_2_0                    EDDI_BIT_MASK_PARA(31,29)

#define EDDI_SER_NRT_LL0_FCW_BIT2__Len_10_3                  EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER_NRT_LL0_FCW_BIT2__Reserved2                 EDDI_BIT_MASK_PARA(10, 8)
#define EDDI_SER_NRT_LL0_FCW_BIT2__pNext                     EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_LL0_FCW_TYPE
{
    EDDI_SER10_UINT64_TYPE    Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_LL0_FCW_TYPE EDDI_SER_NRT_LL0_FCW_TYPE;

#define EDDI_SER_NRT_LL1_FCW_BIT__pDB                        EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER_NRT_LL1_FCW_BIT__Reserved1                  EDDI_BIT_MASK_PARA(22,21)
#define EDDI_SER_NRT_LL1_FCW_BIT__Tagged                     EDDI_BIT_MASK_PARA(23,23)
#define EDDI_SER_NRT_LL1_FCW_BIT__TagStripped                EDDI_BIT_MASK_PARA(24,24)
#define EDDI_SER_NRT_LL1_FCW_BIT__Reserved2                  EDDI_BIT_MASK_PARA(25,25)
#define EDDI_SER_NRT_LL1_FCW_BIT__NRTType                    EDDI_BIT_MASK_PARA(26,26)
#define EDDI_SER_NRT_LL1_FCW_BIT__PrecTime                   EDDI_BIT_MASK_PARA(27,27)
#define EDDI_SER_NRT_LL1_FCW_BIT__CntPort                    EDDI_BIT_MASK_PARA(31,28)

#define EDDI_SER_NRT_LL1_FCW_BIT2__Time                      EDDI_BIT_MASK_PARA(31, 0)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_LL1_FCW_TYPE
{
    EDDI_SER10_UINT64_TYPE     Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_LL1_FCW_TYPE EDDI_SER_NRT_LL1_FCW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_FCW_TYPE
{
    EDDI_SER_NRT_LL0_FCW_TYPE  Hw0;
    EDDI_SER_NRT_LL1_FCW_TYPE  Hw1;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_FCW_TYPE EDDI_SER_NRT_FCW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_NRT_FCW_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_NRT_FCW_PTR_TYPE;

/*===========================================================================*/

/*===========================================================================*/
/*===================== EDDI_SER_NRT_FREE_CCW_TYPE ==========================*/
/*===========================================================================*/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_CCW_TYPE
{
    LSA_UINT32                    Top;
    LSA_UINT32                    Bottom;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_CCW_TYPE EDDI_SER_NRT_CCW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_NRT_CCW_ENTRY_TYPE
{
    EDDI_SER10_UINT64_TYPE        Value;
    EDDI_SER_NRT_CCW_TYPE         p;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_NRT_CCW_ENTRY_TYPE EDDI_SER_NRT_CCW_ENTRY_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_NRT_FREE_CCW_TYPE
{
    EDDI_SER_NRT_CCW_ENTRY_TYPE   NRTFcw;
    EDDI_SER_NRT_CCW_ENTRY_TYPE   NRTDb;
    EDDI_SER_NRT_CCW_ENTRY_TYPE   SRTFcw;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_NRT_FREE_CCW_TYPE EDDI_SER_NRT_FREE_CCW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_NRT_FREE_CCW_TYPE EDDI_LOWER_MEM_ATTR * EDDI_SER_NRT_FREE_CCW_PTR_TYPE;

/*===========================================================================*/
/*===================== EDDI_SER_UCMC_TYPE ==================================*/
/*===========================================================================*/
#define SER_FDB_DEFAULT_PRIO 0x0
#define SER_FDB_PRIO3        0x1
#define SER_FDB_ORG_PRIO     0x2
#define SER_FDB_FLUSS_PRIO   0x3

typedef enum _EDDI_SER_SWI_MODE_TYPE
{
    SER_SWI_CONFIG_MODE,
    SER_SWI_RUN_MODE

} EDDI_SER_SWI_MODE_TYPE;

typedef enum _EDDI_SER_FDB_SEARCH_TYPE
{
    FDB_SEARCH_VALID_STATIC_ENTRY,
    FDB_SEARCH_VALID_DYN_ENTRY,
    FDB_SEARCH_FREE_ENTRY,
    FDB_SEARCH_VALID_ENTRY

} EDDI_SER_FDB_SEARCH_TYPE;

#define EDDI_SER_UCMC_BIT__DestPort                          EDDI_BIT_MASK_PARA( 5, 0)
#if defined (EDDI_CFG_REV7)
#define EDDI_SER_UCMC_BIT__Reserved_0                        EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_UCMC_BIT__BLBit                             EDDI_BIT_MASK_PARA( 7, 7)
#else
#define EDDI_SER_UCMC_BIT__Reserved_0                        EDDI_BIT_MASK_PARA( 7, 6)
#endif

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_UCMC_BIT__SyncBit                           EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_UCMC_BIT__LS_FU                             EDDI_BIT_MASK_PARA( 9, 9)
#else
#define EDDI_SER_UCMC_BIT__Reserved_1                        EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_UCMC_BIT__Ls                                EDDI_BIT_MASK_PARA( 9, 9)
#endif

#define EDDI_SER_UCMC_BIT__Type                              EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_UCMC_BIT__Valid                             EDDI_BIT_MASK_PARA(11,11)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_UCMC_BIT__Age_DCP                           EDDI_BIT_MASK_PARA(12,12)
#else
#define EDDI_SER_UCMC_BIT__Age                               EDDI_BIT_MASK_PARA(12,12)
#endif

#define EDDI_SER_UCMC_BIT__Prio                              EDDI_BIT_MASK_PARA(14,13)
#define EDDI_SER_UCMC_BIT__PauseFilter                       EDDI_BIT_MASK_PARA(15,15)
#define EDDI_SER_UCMC_BIT__MacAdr0                           EDDI_BIT_MASK_PARA(23,16)
#define EDDI_SER_UCMC_BIT__MacAdr1                           EDDI_BIT_MASK_PARA(31,24)

#define EDDI_SER_UCMC_BIT2__MacAdr2                          EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER_UCMC_BIT2__MacAdr3                          EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_SER_UCMC_BIT2__MacAdr4                          EDDI_BIT_MASK_PARA(23,16)
#define EDDI_SER_UCMC_BIT2__MacAdr5                          EDDI_BIT_MASK_PARA(31,24)

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER_UCMC_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;
    LSA_UINT8               Byte[8];  

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER_UCMC_TYPE EDDI_SER_UCMC_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_UCMC_TYPE   EDDI_LOWER_MEM_ATTR * EDDI_SER_UCMC_PTR_TYPE;

/*===========================================================================*/
/*======================== EDDI_SER_STATS_TYPE ==============================*/
/*===========================================================================*/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_STATS_TYPE
{
    LSA_UINT32  RxGoodByte;  /* good bytes received       */
    LSA_UINT32  RxTotalByte; /* total bytes received       */
    LSA_UINT32  TxTotalByte; /* total bytes send        */
    LSA_UINT32  TxFrame;  /* total frames send       */
    LSA_UINT32  RxUni;   /* Unicast received        */
    LSA_UINT32  RxUnUni;  /* Unknown Unicast received      */
    LSA_UINT32  RxMul;   /* Multicast received       */
    LSA_UINT32  RxUnMul;  /* Unknown Multicast received     */
    LSA_UINT32  RxBroad;  /* Broadcast received       */
    LSA_UINT32  TxUni;   /* Unicast send         */
    LSA_UINT32  TxMul;   /* Multicast send        */
    LSA_UINT32  TxBroad;  /* Broadcast send        */
    LSA_UINT32  Size_0_64;  /* Frames of size less than 64 bytes   */
    LSA_UINT32  Size_64;  /* Frames of size 64 byte      */
    LSA_UINT32  Size_65_127; /* Frames of size 56 to 127 bytes    */
    LSA_UINT32  Size_128_255; /* Frames of size 128 to 255 bytes    */
    LSA_UINT32  Size_256_511; /* Frames of size 256 to 511 bytes    */
    LSA_UINT32  Size_512_1023; /* Frames of size 512 to 1023 bytes    */
    LSA_UINT32  Size_1024_MAX; /* Frames of size 1024 to max size bytes  */
    LSA_UINT32  RxGoodFrame; /* Good frames received       */
    LSA_UINT32  RxTotalFrame; /* Total frames received      */
    LSA_UINT32  RxUnderFrame_64;/* Received frames under 64 byte with crc error */
    LSA_UINT32  RxOverflow;  /* Received frames overflow      */
    LSA_UINT32  RxOversize;  /* Received total frames > 1536     */
    LSA_UINT32  ErrCrcAlign; /* CRC and alignment errors      */
    LSA_UINT32  FcwDbDrop;  /* Dropped FCW and DB frames     */
    LSA_UINT32  TxCollision; /* Transmit collisions       */
    LSA_UINT32  HolBndCros;  /* HOL boundary crossed       */
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_UINT32  CcDiffSummary;  /*   Summary clock-counter difference     */
    LSA_UINT32  CcDelaySummary; /*   Summary clock-counter delay     */
    LSA_UINT32  CcCount;      /*   Number of accumulated values     */
    #endif

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_STATS_TYPE EDDI_SER_STATS_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_STATS_TYPE    EDDI_LOWER_MEM_ATTR  * EDDI_SER_STATS_PTR_TYPE;
/*===========================================================================*/
/*===========================================================================*/

#if defined (EDDI_CFG_USE_SW_RPS)
// All used Bits in TimerSB
#define SER_DATA_STATUS_USED_BITS ((LSA_UINT8)(EDD_CSRT_DSTAT_BIT_STATE             | \
                                               EDD_CSRT_DSTAT_BIT_REDUNDANCY        | \
                                               EDD_CSRT_DSTAT_BIT_DATA_VALID        | \
                                               EDD_CSRT_DSTAT_BIT_STOP_RUN          | \
                                               EDD_CSRT_DSTAT_BIT_STATION_FAILURE       ))
#endif

/*===========================================================================*/
/*===================== EDDI_SER_ACW_HEAD_TYPE ==============================*/
/*===========================================================================*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_ACW_HEAD_TYPE
{
    LSA_UINT32              Head[2];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_ACW_HEAD_TYPE EDDI_SER_ACW_HEAD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_ACW_HEAD_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_SER_ACW_HEAD_PTR_TYPE;

/*===========================================================================*/
/*========================== EDDI_SER10_CCW_TYPE ============================*/
/*===========================================================================*/

// NOOP
#define EDDI_SER10_NOOP_BIT__Opc                             EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER10_NOOP_BIT__NotUsd1                         EDDI_BIT_MASK_PARA(31, 4)

#define EDDI_SER10_NOOP_BIT2__NotUsd2                        EDDI_BIT_MASK_PARA(10, 0)
#define EDDI_SER10_NOOP_BIT2__pNext                          EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_NOOP_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_NOOP_TYPE EDDI_SER10_NOOP_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_NOOP_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_NOOP_PTR_TYPE;

/*===========================================================================*/

// EOL
#define EDDI_SER10_EOL_BIT__Opc                              EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER10_EOL_BIT__NotUsd1                          EDDI_BIT_MASK_PARA( 9, 4)
#define EDDI_SER10_EOL_BIT__Time_21_0                        EDDI_BIT_MASK_PARA(31,10)

#define EDDI_SER10_EOL_BIT2__Time_31_22                      EDDI_BIT_MASK_PARA( 9, 0)
#define EDDI_SER10_EOL_BIT2__NotUsd2                         EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER10_EOL_BIT2__pNext                           EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_EOL_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_EOL_TYPE EDDI_SER10_EOL_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_EOL_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_EOL_PTR_TYPE;
typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_EOL_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER_EOL_PTR_TYPE;

#if defined (EDDI_RED_PHASE_SHIFT_ON)
// SOL
#define EDDI_SER10_SOL_BIT__Opc                              EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER10_SOL_BIT__NotUsd1                          EDDI_BIT_MASK_PARA( 9, 4)
#define EDDI_SER10_SOL_BIT__Time_21_0                        EDDI_BIT_MASK_PARA(31,10)

#define EDDI_SER10_SOL_BIT2__Time_31_22                      EDDI_BIT_MASK_PARA( 9, 0)
#define EDDI_SER10_SOL_BIT2__NotUsd2                         EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER10_SOL_BIT2__pNext                           EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_SOL_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_SOL_TYPE EDDI_SER10_SOL_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_SOL_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_SOL_PTR_TYPE;
#endif //EDDI_RED_PHASE_SHIFT_ON

// BCW (BCW_MOD)
#define EDDI_SER10_BCW_MOD_BIT__Opc                          EDDI_BIT_MASK_PARA( 3, 0)
#define EDDI_SER10_BCW_MOD_BIT__ModMask                      EDDI_BIT_MASK_PARA(18, 4)
#define EDDI_SER10_BCW_MOD_BIT__pBranch_12_0                 EDDI_BIT_MASK_PARA(31,19)

#define EDDI_SER10_BCW_MOD_BIT2__pBranch_20_13               EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_BCW_MOD_BIT2__NotUsd                      EDDI_BIT_MASK_PARA(10, 8)
#define EDDI_SER10_BCW_MOD_BIT2__pNext                       EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_BCW_MOD_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_BCW_MOD_TYPE EDDI_SER10_BCW_MOD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_BCW_MOD_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_BCW_MOD_PTR_TYPE;

// LMW (Link Modifier Word)
#define EDDI_SER10_LMW_BIT__Opc                              EDDI_BIT_MASK_PARA(  3, 0)
#define EDDI_SER10_LMW_BIT__NotUsd1                          EDDI_BIT_MASK_PARA( 18, 4)
#define EDDI_SER10_LMW_BIT__pBranch_1                        EDDI_BIT_MASK_PARA( 20,20)
#define EDDI_SER10_LMW_BIT__pBranch_12_0                     EDDI_BIT_MASK_PARA( 31,19)

#define EDDI_SER10_LMW_BIT__pBranch_20_13                    EDDI_BIT_MASK_PARA(  7, 0)
#define EDDI_SER10_LMW_BIT__NotUsd2                          EDDI_BIT_MASK_PARA( 10, 8)
#define EDDI_SER10_LMW_BIT__pNext                            EDDI_BIT_MASK_PARA( 31,11)

#define EDDI_SER10_LMW_BIT__pAddr                            EDDI_BIT_MASK_PARA( 20, 0)
#define EDDI_SER10_LMW_BIT__NotUsd3                          EDDI_BIT_MASK_PARA( 31,21)

#define EDDI_SER10_LMW_BIT__NotUsd4                          EDDI_BIT_MASK_PARA( 31, 0)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL0_LMW_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL0_LMW_TYPE EDDI_SER10_LL0_LMW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL1_LMW_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL1_LMW_TYPE EDDI_SER10_LL1_LMW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LMW_TYPE
{
    EDDI_SER10_LL0_LMW_TYPE  Hw0;
    EDDI_SER10_LL1_LMW_TYPE  Hw1;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LMW_TYPE EDDI_SER10_LMW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_LMW_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_LMW_PTR_TYPE;

// General Kram-HeadElement
#define EDDI_SER10_LIST_HEAD_BIT__pNext                     EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LIST_HEAD_BIT__NotUsd                    EDDI_BIT_MASK_PARA(31,21)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LIST_HEAD_TYPE
{
    LSA_UINT32                Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LIST_HEAD_TYPE EDDI_SER10_LIST_HEAD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_LIST_HEAD_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_LIST_HEAD_PTR_TYPE;

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

// RCW_RED (MainElement of Reduction Control Word)
//   Contains Infos over Reduction-Rate and points to next RCW

#define EDDI_SER10_RCW_RED_BIT__Opc                         EDDI_BIT_MASK_PARA( 3, 0)

#if defined (EDDI_CFG_REV6)
#define EDDI_SER10_RCW_RED_BIT__NotUsd1                     EDDI_BIT_MASK_PARA( 7, 4)
#else
#define EDDI_SER10_RCW_RED_BIT__Udp                         EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER10_RCW_RED_BIT__NotUsd1                     EDDI_BIT_MASK_PARA( 7, 5)
#endif

#define EDDI_SER10_RCW_RED_BIT__SendClockShift              EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_SER10_RCW_RED_BIT__ReductionRate               EDDI_BIT_MASK_PARA(23,16)
#define EDDI_SER10_RCW_RED_BIT__SendClock                   EDDI_BIT_MASK_PARA(31,24)

#define EDDI_SER10_RCW_RED_BIT2__NotUsd2                    EDDI_BIT_MASK_PARA(10, 0)
#define EDDI_SER10_RCW_RED_BIT2__pNext                      EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_RCW_RED_TYPE
{
    EDDI_SER10_UINT64_TYPE  Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_RCW_RED_TYPE EDDI_SER10_RCW_RED_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_RCW_RED_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_RCW_RED_PTR_TYPE;


// RCW_PHS (PhaseElement of Reduction Control Word)
//   Contains Infos over related phase and points to first FCW/ACW of this phase

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_RCW_PHS_TYPE
{
    LSA_UINT32                        Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_RCW_PHS_TYPE EDDI_SER10_RCW_PHS_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_RCW_PHS_TYPE  EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_RCW_PHS_PTR_TYPE;

#endif // if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

/*===========================================================================*/

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Special type for SyncSnd. This combined struct is used to build a
// "single-shot-snd-fcw" used in sync_snd.c
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_SINGLE_SHOT_SND_TYPE
{
    EDDI_SER10_NOOP_TYPE               Noop;
    EDDI_SER10_FCW_SND_TYPE            FcwSnd;
    EDDI_SER10_LMW_TYPE                Lmw;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_SINGLE_SHOT_SND_TYPE EDDI_SER10_SINGLE_SHOT_SND_TYPE;

#if defined (EDDI_RED_PHASE_SHIFT_ON)
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Combined type for shifting of red TX-phase.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE
{
    EDDI_SER10_LMW_TYPE                Lmw;
    EDDI_SER10_FCW_SND_TYPE            FcwSnd;
    EDDI_SER10_SOL_TYPE                Sol;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_IRT_PORT_START_TIME_TX_BLOCK_TYPE  EDDI_LOWER_MEM_ATTR  *  EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Combined type for shifting of red RX-phase.
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE
{
    EDDI_SER10_LMW_TYPE                Lmw;
    EDDI_SER10_FCW_RCV_TYPE            FcwRcv;
    EDDI_SER10_SOL_TYPE                Sol;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE  EDDI_SER10_IRT_PORT_START_TIME_RX_BLOCK_TYPE  EDDI_LOWER_MEM_ATTR  *  EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE;
#endif //EDDI_RED_PHASE_SHIFT_ON

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER10_CCW_TYPE
{
    EDDI_SER10_LIST_HEAD_TYPE          ListHead;
    EDDI_SER10_NOOP_TYPE               Noop;
    EDDI_SER10_EOL_TYPE                Eol;
    EDDI_SER10_BCW_MOD_TYPE            BcwMod;
    EDDI_SER10_FCW_ACW_TYPE            FcwAcw;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_SER10_RCW_RED_TYPE            RcwRed;
    EDDI_SER10_RCW_PHS_TYPE            RcwPhs;
    #endif

    //  EDDI_SER10_EOC_TYPE              Eoc;
    //  EDDI_SER10_SOL_TYPE              Sol;
    //  EDDI_SER10_SOC_TYPE              Soc;
    //  EDDI_SER10_BCW_ID_TYPE           BcwId;
    EDDI_SER10_LMW_TYPE                Lmw;
    //  EDDI_SER10_BMW_TYPE              Bmw;

    EDDI_SER10_SINGLE_SHOT_SND_TYPE    SingleShotSnd;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER10_CCW_TYPE EDDI_SER10_CCW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE  EDDI_SER10_CCW_TYPE  EDDI_LOWER_MEM_ATTR  *  EDDI_SER_CCW_PTR_TYPE;
/*===========================================================================*/

/*===========================================================================*/
/*====================== END OF EDDI_SER10_CCW_TYPE =========================*/
/*===========================================================================*/

/*===========================================================================*/
/*====================== EDDI_SER_SCORE_BOARD_TYPES =========================*/
/*===========================================================================*/

//Timer-Score-Board
//#define SER10_CNT_TIMER_SCOREB_ENTRIES_64BIT 4

//Producer-Score-Board

#define SER10_SB_SIZE_ENTRY_BIT              4
//#define SER10_CNT_PROD_SCOREB_ENTRIES_64BIT  16
//#define SER10_CNT_PROD_SCOREB_ENTRIES_32BIT  8

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_PROV_SCORB_TYPE
{
    LSA_UINT32                 LValue[2];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_PROV_SCORB_TYPE EDDI_SER10_PROV_SCORB_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_PROV_SCORB_TYPE   EDDI_LOWER_MEM_ATTR * EDDI_SER10_PROV_SCORB_PTR_TYPE;

/*===========================================================================*/
/*=================== END OF EDDI_SER_SCORE_BOARD_TYPES =====================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_CYCLEID_DATA_TYPE ======================*/
/*===========================================================================*/

#define EDDI_SER_CYCLEID_DATA_BIT__CycleId                   EDDI_BIT_MASK_PARA( 5, 0)
#define EDDI_SER_CYCLEID_DATA_BIT__EndApp                    EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_CYCLEID_DATA_BIT__StartApp                  EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_SER_CYCLEID_DATA_BIT__Unused                    EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_CYCLEID_DATA_BIT__Mask_Comp_1to5            EDDI_BIT_MASK_PARA(13, 9)
#define EDDI_SER_CYCLEID_DATA_BIT__Res1                      EDDI_BIT_MASK_PARA(31,14)

/*===========================================================================*/

/*===========================================================================*/
/*====================== EDDI_SER10_IRT_SND_RCV_HEAD_TYPE ===================*/
/*===========================================================================*/

// One Pair of IRT-ListHeader (Part of the KRAM-based IrtBaseList)
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_IRT_SND_RCV_HEAD_TYPE
{
    LSA_UINT32     Rcv;
    LSA_UINT32     Snd;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_IRT_SND_RCV_HEAD_TYPE EDDI_SER_IRT_SND_RCV_HEAD_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER_IRT_BASE_LIST_TYPE
{
    EDDI_SER_IRT_SND_RCV_HEAD_TYPE  Head[EDDI_MAX_IRTE_PORT_CNT];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER_IRT_BASE_LIST_TYPE EDDI_SER_IRT_BASE_LIST_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER_IRT_BASE_LIST_TYPE   EDDI_LOWER_MEM_ATTR * EDDI_SER_IRT_BASE_LIST_PTR_TYPE;
/*===========================================================================*/

/*===========================================================================*/
/*============================= EDDI_SER10_TCW_TYPE =========================*/
/*===========================================================================*/

#define EDDI_SER10_L0_TCW_BIT__Reserved1                   EDDI_BIT_MASK_PARA( 5, 0)
#define EDDI_SER10_L0_TCW_BIT__Tcw_F_BIT                   EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER10_L0_TCW_BIT__Tcw_D_BIT                   EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_SER10_L0_TCW_BIT__NrOfDiag                    EDDI_BIT_MASK_PARA(23, 8)
#define EDDI_SER10_L0_TCW_BIT__Reserved2                   EDDI_BIT_MASK_PARA(31,24)

#define EDDI_SER10_L0_TDE_BIT__DQ                          EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER10_L0_TDE_BIT__DiagnoseStatus              EDDI_BIT_MASK_PARA(15, 1)
#define EDDI_SER10_L0_TDE_BIT__FrameID                     EDDI_BIT_MASK_PARA(31, 16)
#define EDDI_SER10_L3_TDE_BIT__Port                        EDDI_BIT_MASK_PARA(19, 17)
#define EDDI_SER10_L3_TDE_BIT__CycleNumber                 EDDI_BIT_MASK_PARA(31, 20)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_L0_TCW_TYPE
{
    LSA_UINT32            Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_L0_TCW_TYPE EDDI_SER10_L0_TCW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_L1_TCW_TYPE
{
    LSA_UINT32   DestAdr;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_L1_TCW_TYPE EDDI_SER10_L1_TCW_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_TCW_TYPE
{
    EDDI_SER10_L0_TCW_TYPE  L0;
    EDDI_SER10_L1_TCW_TYPE  L1;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_TCW_TYPE EDDI_SER10_TCW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_TCW_TYPE     EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_TCW_PTR_TYPE;
/*===========================================================================*/
/*===========================================================================*/
#include "pnio_pck_off.h"

/*===========================================================================*/
/*========================= EDDI_SER_ETH_PORT_X_TYPE ========================*/
/*===========================================================================*/

/*=======================================*/
/*====== EDDI_SER_MIN_PREAMBLE_TYPE =====*/
/*=======================================*/
#define EDDI_SER_MIN_PREAMBLE_BIT__MinPreamble                EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER_MIN_PREAMBLE_BIT__Res0                       EDDI_BIT_MASK_PARA(31, 8)

/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_IRT_DIAG_ENABLE_TYPE ======*/
/*===================================*/
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvData                        EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvTime                        EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvSubstitute                  EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__SRTRcvErrLate                     EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrNRT                      EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrMissing                  EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErraSRT                     EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrUnexpected               EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrCRC                      EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrDataLost                 EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrLength                   EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrNoACW                    EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_IRT_DIAG_ENABLE_BIT__RRTErrNoTableEntry                EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTRcvErrBufOvfl                  EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__SRTRcvErrNoACW                    EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTSndErrNoData                   EDDI_BIT_MASK_PARA(15,15)

#define EDDI_SER_IRT_DIAG_ENABLE_BIT__IRTSndErrLate                     EDDI_BIT_MASK_PARA(16,16)
#define EDDI_SER_IRT_DIAG_ENABLE_BIT__Res1                              EDDI_BIT_MASK_PARA(31,17)

/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_NRT_CTRL_TYPE ======*/
/*===================================*/
#define EDDI_SER_NRT_CTRL_BIT__Baudrate                        EDDI_BIT_MASK_PARA( 0, 0)

#define EDDI_SER_NRT_CTRL_BIT__FullDuplex                      EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_NRT_CTRL_BIT__EnCutThrough                    EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_NRT_CTRL_BIT__EnSRTPriority                   EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_SER_NRT_CTRL_BIT__EnLearning                      EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_NRT_CTRL_BIT__MCLearningEn                    EDDI_BIT_MASK_PARA( 5, 5)

#define EDDI_SER_NRT_CTRL_BIT__EnPadding                       EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_NRT_CTRL_BIT__IngressFilter                   EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_NRT_CTRL_BIT__EnCheckSA                       EDDI_BIT_MASK_PARA( 8, 8)

#define EDDI_SER_NRT_CTRL_BIT__EnISRT                          EDDI_BIT_MASK_PARA( 9, 9)

#define EDDI_SER_NRT_CTRL_BIT__EnRedundance                    EDDI_BIT_MASK_PARA(10,10)

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define EDDI_SER_NRT_CTRL_BIT__EnMonitorCyclic                 EDDI_BIT_MASK_PARA(11,11)
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

#define EDDI_SER_NRT_CTRL_BIT__EnShortFrame                    EDDI_BIT_MASK_PARA(12,12)

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define EDDI_SER_NRT_CTRL_BIT__EnMonitorAcyclic                EDDI_BIT_MASK_PARA(13,13)
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

#if defined (EDDI_CFG_REV5)
#define EDDI_SER_NRT_CTRL_BIT__Res0                            EDDI_BIT_MASK_PARA(15,14)
#elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_NRT_CTRL_BIT__Add_Line_Delay                  EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_NRT_CTRL_BIT__Sync_Redundance_Enable          EDDI_BIT_MASK_PARA(15,15)
#endif

#define EDDI_SER_NRT_CTRL_BIT__Res1                            EDDI_BIT_MASK_PARA(31,16)

/*===========================================================================*/

/*=======================================*/
/*====== EDDI_SER_DEF_VLAN_TAG_TYPE ======*/
/*=======================================*/
#define EDDI_SER_DEF_VLAN_TAG_BIT__VLANAdr          EDDI_BIT_MASK_PARA(11, 0)
#define EDDI_SER_DEF_VLAN_TAG_BIT__Reserved         EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_DEF_VLAN_TAG_BIT__DefVLANPrio      EDDI_BIT_MASK_PARA(15,13)
#define EDDI_SER_DEF_VLAN_TAG_BIT__Res1             EDDI_BIT_MASK_PARA(31,16)

/*===========================================================================*/
/*======================== EDDI_SER_CYCL_SYNC_CTRL_TYPE =====================*/
/*===========================================================================*/

/*====================================*/
/*====== EDDI_SER_PHASE_COR_TYPE ======*/
/*====================================*/
#define EDDI_SER_PHASE_COR_BIT__CntCycl             EDDI_BIT_MASK_PARA(30,  0)
#define EDDI_SER_PHASE_COR_BIT__Inkrement           EDDI_BIT_MASK_PARA(31, 31)

/*===========================================================================*/
/*========================= EDDI_DEV_NRT_CTRL_TYPE ==========================*/
/*===========================================================================*/

/*======================================*/
/*====== EDDI_SER_NRT_MAP_CHx_TYPE ======*/
/*======================================*/
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio0_TAG_1_2     EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio1_TAG_0_3     EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio2_TAG_4_5     EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio3_TAG_6_7     EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_NRT_MAP_CHx_BIT__Prio4_aSRTLow     EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio5_cSRT        EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio6_aSRTHigh    EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio7_iSRT        EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_NRT_MAP_CHx_BIT__Prio8_ORG         EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_NRT_MAP_CHx_BIT__Prio9_Fluss       EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_NRT_MAP_CHx_BIT__Res0              EDDI_BIT_MASK_PARA(15,10)

#define EDDI_SER_NRT_MAP_CHx_BIT__Res1              EDDI_BIT_MASK_PARA(31,16)

#define EDDI_CHx_MAP_ALL_TO_IF0                     0x000003FF

/*=========================================*/
/*====== EDDI_SER_NRT_ENABLE_CHx_TYPE ======*/
/*=========================================*/
/*===========================================================================*/

/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_IRT_CTRL_TYPE ==========================*/
/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_IRT_CTRL_TYPE ======*/
/*===================================*/
/*================================= ==========================================*/

/*=====================================*/
/*====== EDDI_SER_IMAGE_MODE_TYPE ======*/
/*=====================================*/
#define EDDI_SER_IMAGE_MODE_BIT__BufMode            EDDI_BIT_MASK_PARA( 1, 0)
#define EDDI_SER_IMAGE_MODE_BIT__SyncAPI            EDDI_BIT_MASK_PARA( 3, 2)
#define EDDI_SER_IMAGE_MODE_BIT__EnStandBy          EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_IMAGE_MODE_BIT__EnAsyncDMA         EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_IMAGE_MODE_BIT__Res0               EDDI_BIT_MASK_PARA(31, 6)

/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_PHY_SMI_PORT_X =========================*/
/*===========================================================================*/

//SMI for PORT_X
/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_PHY_CMD_TYPE =======*/
/*===================================*/
#define EDDI_SER_PHY_CMD_BIT__PhyAddr                   EDDI_BIT_MASK_PARA( 4, 0)
#define EDDI_SER_PHY_CMD_BIT__Enable                    EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_PHY_CMD_BIT__PreampSup                 EDDI_BIT_MASK_PARA( 6, 6)

#define EDDI_SER_PHY_CMD_BIT__Res0                      EDDI_BIT_MASK_PARA(31, 7)

/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

//PHY
/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_MD_CA_TYPE =========*/
/*===================================*/
#define EDDI_SER_MD_CA_BIT__Address                       EDDI_BIT_MASK_PARA( 4, 0)
#define EDDI_SER_MD_CA_BIT__PhyAddr                       EDDI_BIT_MASK_PARA( 9, 5)
#define EDDI_SER_MD_CA_BIT__Write                         EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_MD_CA_BIT__Busy                          EDDI_BIT_MASK_PARA(11,11)
#define EDDI_SER_MD_CA_BIT__PreamSupress                  EDDI_BIT_MASK_PARA(12,12)

#define EDDI_SER_MD_CA_BIT__Res0                          EDDI_BIT_MASK_PARA(31,13)

/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_MAC_CTRL_TYPE ======*/
/*===================================*/
#define EDDI_SER_MAC_CTRL_BIT__HaltSndRcv                                   EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_MAC_CTRL_BIT__HaltImmed                                    EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_MAC_CTRL_BIT__SoftReset                                    EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_MAC_CTRL_BIT__FullDuplex                                   EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_SER_MAC_CTRL_BIT__MacLoop                                      EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_MAC_CTRL_BIT__ConMode                                      EDDI_BIT_MASK_PARA( 6, 5)

#define EDDI_SER_MAC_CTRL_BIT__Loop_10                                      EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_MAC_CTRL_BIT__LinkChanged                                  EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_MAC_CTRL_BIT__Reserved0                                    EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_MAC_CTRL_BIT__MissedRoll                                   EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_MAC_CTRL_BIT__Reserved1                                    EDDI_BIT_MASK_PARA(12,11)
#define EDDI_SER_MAC_CTRL_BIT__EnableMiss                                   EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_MAC_CTRL_BIT__Reserved2                                    EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_MAC_CTRL_BIT__LinkStat_10                                  EDDI_BIT_MASK_PARA(15,15)

#define EDDI_SER_MAC_CTRL_BIT__Res0                                         EDDI_BIT_MASK_PARA(31,16)

/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_ARC_CTRL_TYPE ======*/
/*===================================*/
#define EDDI_SER_ARC_CTRL_BIT__StationAcpt                                 EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_ARC_CTRL_BIT__GroupAcpt                                   EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_ARC_CTRL_BIT__BroadAcpt                                   EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_ARC_CTRL_BIT__NegativARC                                  EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_ARC_CTRL_BIT__CompEnable                                  EDDI_BIT_MASK_PARA( 4, 4)

#define EDDI_SER_ARC_CTRL_BIT__Res0                                        EDDI_BIT_MASK_PARA(31, 5)

/*===========================================================================*/

/*=====================================*/
/*====== EDDI_SER_TRANS_CTRL_TYPE ======*/
/*=====================================*/
#define EDDI_SER_TRANS_CTRL_BIT__TransEn                                      EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_TRANS_CTRL_BIT__TransHalt                                    EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_TRANS_CTRL_BIT__SupprPadd                                    EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_TRANS_CTRL_BIT__SupprCRC                                     EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_TRANS_CTRL_BIT__FastBackOff                                  EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_TRANS_CTRL_BIT__NoExcessive                                  EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_TRANS_CTRL_BIT__SendPause                                    EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_TRANS_CTRL_BIT__MII_10_Mode                                  EDDI_BIT_MASK_PARA( 7, 7)


#define EDDI_SER_TRANS_CTRL_BIT__FIFOUnderr                                   EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_TRANS_CTRL_BIT__ExcessiveDef                                 EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_TRANS_CTRL_BIT__LostCarrier                                  EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_TRANS_CTRL_BIT__ExcessColl                                   EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_TRANS_CTRL_BIT__LateColl                                     EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_TRANS_CTRL_BIT__TransParity                                  EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_TRANS_CTRL_BIT__Complete                                     EDDI_BIT_MASK_PARA(14,14)

#define EDDI_SER_TRANS_CTRL_BIT__Res0                                         EDDI_BIT_MASK_PARA(31,15)

/*===================================*/
/*====== EDDI_SER_RCV_CTRL_TYPE ======*/
/*===================================*/
#define EDDI_SER_RCV_CTRL_BIT__RcvEnable                                   EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_RCV_CTRL_BIT__RcvHalt                                     EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_RCV_CTRL_BIT__LongEnable                                  EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_RCV_CTRL_BIT__ShortEnable                                 EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_RCV_CTRL_BIT__CRCStrip                                    EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_RCV_CTRL_BIT__PassCtrl                                    EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_RCV_CTRL_BIT__IngnoreCRCVal                               EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_RCV_CTRL_BIT__Reserved0                                   EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_RCV_CTRL_BIT__AlignErr                                    EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_RCV_CTRL_BIT__CRCErr                                      EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_RCV_CTRL_BIT__OverflowErr                                 EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_RCV_CTRL_BIT__LongErr                                     EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_RCV_CTRL_BIT__Reserved1                                   EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_RCV_CTRL_BIT__ParityErr                                   EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_RCV_CTRL_BIT__Good                                        EDDI_BIT_MASK_PARA(14,14)

#define EDDI_SER_RCV_CTRL_BIT__Res0                                        EDDI_BIT_MASK_PARA(31,15)

/*===========================================================================*/

/*===========================================*/
/*====== EDDI_SER_NRT_DEFAULT_CTRL_TYPE ======*/
/*===========================================*/
#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__CHA                 EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__CHB                 EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__DstP0               EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__DstP1               EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__DstP2               EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__DstP3               EDDI_BIT_MASK_PARA( 5, 5)

#define EDDI_SER_NRT_DEFAULT_CTRL_BIT__Res0                EDDI_BIT_MASK_PARA(31, 6)

/*===========================================================================*/

/*==================================*/
/*====== EDDI_SER_FC_MASK_TYPE ======*/
/*==================================*/
/*===========================================================================*/

/*=================================================*/
/*====== EDDI_SER_COMMAND_RESET_TX_QUEUE_TYPE ======*/
/*=================================================*/

#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Qu_Type                              EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Qu_No                                EDDI_BIT_MASK_PARA( 4, 1)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Res                                  EDDI_BIT_MASK_PARA( 8, 5)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_0                               EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_1                               EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_2                               EDDI_BIT_MASK_PARA(11,11)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_3                               EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_4                               EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_5                               EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_6                               EDDI_BIT_MASK_PARA(15,15)

#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_7                               EDDI_BIT_MASK_PARA(16,16)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_8                               EDDI_BIT_MASK_PARA(17,17)
#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Prio_9                               EDDI_BIT_MASK_PARA(18,18)

#define EDDI_SER_COMMAND_RESET_TX_QUEUE_BIT__Res0                                 EDDI_BIT_MASK_PARA(31,19)

/*===========================================================================*/

/*=======================================*/
/*====== EDDI_SER_PRIM_COMMAND_TYPE ======*/
/*=======================================*/
#define EDDI_SER_PRIM_COMMAND_BIT__Para                                   EDDI_BIT_MASK_PARA(21, 0)
#define EDDI_SER_PRIM_COMMAND_BIT__UsrId                                  EDDI_BIT_MASK_PARA(24,22)
#define EDDI_SER_PRIM_COMMAND_BIT__FCode                                  EDDI_BIT_MASK_PARA(29,25)
#define EDDI_SER_PRIM_COMMAND_BIT__Reserved                               EDDI_BIT_MASK_PARA(30,30)
#define EDDI_SER_PRIM_COMMAND_BIT__Active                                 EDDI_BIT_MASK_PARA(31,31)

/*===========================================================================*/

/*======================================*/
/*====== EDDI_SER_SEC_COMMAND_TYPE ======*/
/*======================================*/
#define EDDI_SER_SEC_COMMAND_BIT__Para                                   EDDI_BIT_MASK_PARA(30, 0)
#define EDDI_SER_SEC_COMMAND_BIT__Active                                 EDDI_BIT_MASK_PARA(31,31)

/*===========================================================================*/

/*=======================================*/
/*====== EDDI_SER_CONF_COMMAND_TYPE ======*/
/*=======================================*/
#define EDDI_SER_CONF_COMMAND_BIT__Result                                 EDDI_BIT_MASK_PARA(21, 0)
#define EDDI_SER_CONF_COMMAND_BIT__UsrId                                  EDDI_BIT_MASK_PARA(24,22)
#define EDDI_SER_CONF_COMMAND_BIT__FCode                                  EDDI_BIT_MASK_PARA(29,25)
#define EDDI_SER_CONF_COMMAND_BIT__Conf                                   EDDI_BIT_MASK_PARA(30,30)
#define EDDI_SER_CONF_COMMAND_BIT__Error                                  EDDI_BIT_MASK_PARA(31,31)

/*===========================================================================*/

/*====================================*/
/*====== EDDI_SER_PORT_CTRL_TYPE ======*/
/*====================================*/
#define EDDI_SER_PORT_CTRL_BIT__DisableClosedP0       EDDI_BIT_MASK_PARA( 1, 0)
#define EDDI_SER_PORT_CTRL_BIT__DisableP0             EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_PORT_CTRL_BIT__ClosedP0              EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_PORT_CTRL_BIT__ResP0                 EDDI_BIT_MASK_PARA( 3, 2)

#define EDDI_SER_PORT_CTRL_BIT__DisableClosedP1       EDDI_BIT_MASK_PARA( 5, 4)
#define EDDI_SER_PORT_CTRL_BIT__DisableP1             EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_PORT_CTRL_BIT__ClosedP1              EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_PORT_CTRL_BIT__ResP1                 EDDI_BIT_MASK_PARA( 7, 6)

#define EDDI_SER_PORT_CTRL_BIT__DisableClosedP2       EDDI_BIT_MASK_PARA( 9, 8)
#define EDDI_SER_PORT_CTRL_BIT__DisableP2             EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_PORT_CTRL_BIT__ClosedP2              EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_PORT_CTRL_BIT__ResP2                 EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_PORT_CTRL_BIT__DisableClosedP3       EDDI_BIT_MASK_PARA(13,12)
#define EDDI_SER_PORT_CTRL_BIT__DisableP3             EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_PORT_CTRL_BIT__ClosedP3              EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_PORT_CTRL_BIT__ResP3                 EDDI_BIT_MASK_PARA(15,14)

#define EDDI_SER_PORT_CTRL_BIT__Res0                  EDDI_BIT_MASK_PARA(31,16)

/*====================================*/
/*====== EDDI_SER_PROD_CTRL_TYPE ======*/
/*====================================*/
#define EDDI_SER_PROD_CTRL_BIT__EnIRT                           EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_PROD_CTRL_BIT__EnSRT                           EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_PROD_CTRL_BIT__EnSCOREBChange                  EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_PROD_CTRL_BIT__StartPSAging                    EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_SER_PROD_CTRL_BIT__PsSCOREBSelect                  EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_PROD_CTRL_BIT__Res0                            EDDI_BIT_MASK_PARA(31, 5)

/*===========================================================================*/

/*====================================*/
/*====== EDDI_SER_PS_ENTRY_TYPE  ======*/
/*====================================*/
/*
#define EDDI_SER_PS_ENTRY_MASK__ProdState            EDDI_PASTE_32( 0, 0, 0, 1)
#define EDDI_SER_PS_ENTRY_MASK__ProdStateCh          EDDI_PASTE_32( 0, 0, 0, 2)
#define EDDI_SER_PS_ENTRY_MASK__DataStateCh          EDDI_PASTE_32( 0, 0, 0, 4)

#define EDDI_PS_BIT__ProdState(Reg)                  (Reg & EDDI_SER_PS_ENTRY_MASK__ProdState)
#define EDDI_PS_BIT__ProdStateCh(Reg)                (Reg & EDDI_SER_PS_ENTRY_MASK__ProdStateCh)
#define EDDI_PS_BIT__DataStateCh(Reg)                (Reg & EDDI_SER_PS_ENTRY_MASK__DataStateCh)
*/
#define EDDI_PS_COND__MISS(Reg)                      (2 == (Reg & 3))
#define EDDI_PS_COND__AGAIN(Reg)                     (3 == (Reg & 3))
#define EDDI_PS_BIT__ProdState(Reg)                  (Reg & 0x00000001UL)
#define EDDI_PS_BIT__ProdStateCh(Reg)                (Reg & 0x00000002UL)
#define EDDI_PS_BIT__DataStateCh(Reg)                (Reg & 0x00000004UL)
#if defined (EDDI_CFG_REV7) || defined (EDDI_CFG_REV6)
#define EDDI_PS_BIT__DataHoldExpired(Reg)            (Reg & 0x00000008UL)
#endif
#define EDDI_PS_BIT__ProdDataStateCh(Reg)            (Reg & 0x00000006UL)

#define EDDI_SER_PS_ENTRY_BIT__ProdState                                  EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_PS_ENTRY_BIT__ProdStateCh                                EDDI_BIT_MASK_PARA( 1, 0)
#define EDDI_SER_PS_ENTRY_BIT__DataStateCh                                EDDI_BIT_MASK_PARA( 2, 2)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_PS_ENTRY_BIT__DataHoldExpired                            EDDI_BIT_MASK_PARA( 3, 3)
#else
#define EDDI_SER_PS_ENTRY_BIT__Reserved                                   EDDI_BIT_MASK_PARA( 3, 3)
#endif

#define EDDI_SER_PS_ENTRY_BIT__Reserved_1                                 EDDI_BIT_MASK_PARA( 7, 4)

#define EDDI_SER_PS_ENTRY_BIT__Res2                                       EDDI_BIT_MASK_PARA(31, 8)

#define EDDI_DS_BIT__State_backup0_primary1(Reg)                  (Reg & 0x01)
#define EDDI_DS_BIT__Redundancy(Reg)                              (Reg & 0x02)
#define EDDI_DS_BIT__DataValid(Reg)                               (Reg & 0x04)
#define EDDI_DS_BIT__Bit_3(Reg)                                   (Reg & 0x08)

#define EDDI_DS_BIT__Stop0_Run1(Reg)                              (Reg & 0x10)
#define EDDI_DS_BIT__StationFailure(Reg)                          (Reg & 0x20)
#define EDDI_DS_BIT__Bit_6(Reg)                                   (Reg & 0x40)
#define EDDI_DS_BIT__Bit_Ignore(Reg)                              (Reg & 0x80)


#define EDDI_SET_DS_BIT__State_backup0_primary1(pReg, value)                      \
/*lint --e(941) */  (*pReg  = (LSA_UINT8)((*pReg & ~0x01) | ( ((value) << 0) & 0x01 )))

#define EDDI_SET_DS_BIT__Redundancy(pReg, value)                      \
/*lint --e(941) */  (*pReg  = (LSA_UINT8)((*pReg & ~0x02) | ( ((value) << 1) & 0x02 )))

#define EDDI_SET_DS_BIT__DataValid(pReg, value)                      \
/*lint --e(941) */  (*pReg  = (LSA_UINT8)((*pReg & ~0x04) | ( ((value) << 2) & 0x04 )))

#define EDDI_SET_DS_BIT__Bit_3(pReg, value)                      \
/*lint --e(941) */ (*pReg  = (LSA_UINT8)((*pReg & ~0x08) | ( ((value) << 3) & 0x08 )))

#define EDDI_SET_DS_BIT__Stop0_Run1(pReg, value)                      \
/*lint --e(941) */ (*pReg  = (LSA_UINT8)((*pReg & ~0x10) | ( ((value) << 4) & 0x10 )))

#define EDDI_SET_DS_BIT__StationFailure(pReg, value)                      \
/*lint --e(941) */ (*pReg  = (LSA_UINT8)((*pReg & ~0x20) | ( ((value) << 5) & 0x20 )))


#define EDDI_DS_BIT2_DATA_VALID    0x04

typedef LSA_UINT8 EDDI_SER_DATA_STATUS_TYPE;

/*=======================================*/
/*====== EDDI_SER_NRT_API_CTRL_TYPE ======*/
/*=======================================*/
#define EDDI_SER_NRT_API_CTRL_BIT__S_R                EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_NRT_API_CTRL_BIT__CHA0               EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_NRT_API_CTRL_BIT__CHA1               EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_NRT_API_CTRL_BIT__CHB0               EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_SER_NRT_API_CTRL_BIT__CHB1               EDDI_BIT_MASK_PARA( 4, 4)

/*===========================================================================*/

/*===================================*/
/*====== EDDI_SER_SWI_CTRL_TYPE ======*/
/*===================================*/
#define EDDI_SER_SWI_CTRL_BIT__SwitchEnable               EDDI_BIT_MASK_PARA( 0, 0)
//#define EDDI_SER_SWI_CTRL_BIT__SoftReset                  EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_SWI_CTRL_BIT__PHY_SMICtrl                EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_SWI_CTRL_BIT__StatiCtrl                  EDDI_BIT_MASK_PARA( 3, 3)

//#define EDDI_SER_SWI_CTRL_BIT__TRCCtrl                    EDDI_BIT_MASK_PARA( 4, 4)
//#define EDDI_SER_SWI_CTRL_BIT__KRAMCtrl                   EDDI_BIT_MASK_PARA( 5, 5)
//#define EDDI_SER_SWI_CTRL_BIT__SRTCtrl                    EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_SWI_CTRL_BIT__IRTCtrl                    EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_SWI_CTRL_BIT__NRTCtrl                    EDDI_BIT_MASK_PARA( 8, 8)
//#define EDDI_SER_SWI_CTRL_BIT__TRIGUnit                   EDDI_BIT_MASK_PARA( 9, 9)
//#define EDDI_SER_SWI_CTRL_BIT__IRQCtrl                    EDDI_BIT_MASK_PARA(10,10)
//#define EDDI_SER_SWI_CTRL_BIT__CYCCtrl                    EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_SWI_CTRL_BIT__Port0                      EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_SWI_CTRL_BIT__Port1                      EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_SWI_CTRL_BIT__Port2                      EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_SWI_CTRL_BIT__Port3                      EDDI_BIT_MASK_PARA(15,15)

/*===========================================================================*/

/*========================================*/
/*====== EDDI_SER_MIN_IDLE_TIME_TYPE ======*/
/*========================================*/
#define EDDI_SER_MIN_IDLE_TIME_BIT__MinIdleTime10MBit             EDDI_BIT_MASK_PARA( 9, 0)
#define EDDI_SER_MIN_IDLE_TIME_BIT__Reserved                      EDDI_BIT_MASK_PARA(15,10)
#define EDDI_SER_MIN_IDLE_TIME_BIT__MinIdleTime100MBit            EDDI_BIT_MASK_PARA(25,16)
#define EDDI_SER_MIN_IDLE_TIME_BIT__Res0                          EDDI_BIT_MASK_PARA(31,26)

/*===========================================================================*/

/*==========================================*/
/*====== EDDI_SER_MAC_EMPTY_COUNT_TYPE ======*/
/*==========================================*/
#define EDDI_SER_MAC_EMPTY_COUNT_BIT__MacEmpty10MBit                 EDDI_BIT_MASK_PARA( 5, 0)
#define EDDI_SER_MAC_EMPTY_COUNT_BIT__Reserved                       EDDI_BIT_MASK_PARA( 7, 6)
#define EDDI_SER_MAC_EMPTY_COUNT_BIT__MacEmpty100MBit                EDDI_BIT_MASK_PARA(13, 8)
#define EDDI_SER_MAC_EMPTY_COUNT_BIT__Res0                           EDDI_BIT_MASK_PARA(31,14)

/*===========================================================================*/

/*=========================================*/
/*====== EDDI_SER_NRT_TRANS_CTRL_TYPE ======*/
/*=========================================*/
#define EDDI_SER_NRT_TRANS_CTRL_BIT__TagMode                EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_NRT_TRANS_CTRL_BIT__NRT_CT_Delay           EDDI_BIT_MASK_PARA( 6, 1)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_ARP_Filter      EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_SER_NRT_TRANS_CTRL_BIT__Enable_DCP_Filter      EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_NRT_TRANS_CTRL_BIT__b_2Port_CT_Mode        EDDI_BIT_MASK_PARA( 9, 9)
#else
#define EDDI_SER_NRT_TRANS_CTRL_BIT__Res400                 EDDI_BIT_MASK_PARA(10, 7)
#endif

#define EDDI_SER_NRT_TRANS_CTRL_BIT__Res0                   EDDI_BIT_MASK_PARA(31,11)

/*===========================================================================*/

/*===========================================*/
/*====== EDDI_SER_NRT_SAFETY_MARGIN_TYPE ======*/
/*===========================================*/
#define EDDI_SER_NRT_SAFETY_MARGIN_BIT__Bd10          EDDI_BIT_MASK_PARA(15, 0)
#define EDDI_SER_NRT_SAFETY_MARGIN_BIT__Bd100         EDDI_BIT_MASK_PARA(31,16)

/*===========================================================================*/

#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)

/*=======================================*/
/*====== EDDI_SER_MONITOR_CTRL_TYPE ======*/
/*=======================================*/
#define EDDI_SER_MONITOR_CTRL_BIT__P0                 EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_MONITOR_CTRL_BIT__P1                 EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_MONITOR_CTRL_BIT__P2                 EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_MONITOR_CTRL_BIT__P3                 EDDI_BIT_MASK_PARA( 5, 5)

#define EDDI_SER_MONITOR_CTRL_BIT__Res0               EDDI_BIT_MASK_PARA(31, 6)

/*===========================================================================*/

#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

/*=========================================*/
/*====== EDDI_SER_STAT_BASE_TYPE ==========*/
/*=========================================*/

/*============================================*/
/*====== EDDI_SER_SWITCH_SETUP_TYPE ==========*/
/*============================================*/
#define EDDI_SER_SWITCH_SETUP_BIT__NRT_API_Access               EDDI_BIT_MASK_PARA( 0, 0)

#define EDDI_SER_SWITCH_SETUP_BIT__MACChargMode                 EDDI_BIT_MASK_PARA( 1, 1)

#define EDDI_SER_SWITCH_SETUP_BIT__IRTDMAAcces                  EDDI_BIT_MASK_PARA( 2, 2)

#define EDDI_SER_SWITCH_SETUP_BIT__NRTChxTxMode                 EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_SWITCH_SETUP_BIT__NRTChxRxMode                 EDDI_BIT_MASK_PARA( 4, 4)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER_SWITCH_SETUP_BIT__NRTDischargMode              EDDI_BIT_MASK_PARA( 5, 5)

//#define EDDI_SER_SWITCH_SETUP_BIT__SRTAPIChargeMode           EDDI_BIT_MASK_PARA( 6, 6)

#define EDDI_SER_SWITCH_SETUP_BIT__Confirmation_SndTime         EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_SWITCH_SETUP_BIT__Max_TxStat_Valuation         EDDI_BIT_MASK_PARA( 8, 8)

#define EDDI_SER_SWITCH_SETUP_BIT__Ignore_Substitute_Frame      EDDI_BIT_MASK_PARA( 9, 9)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER_SWITCH_SETUP_BIT__RToverUDP_Enable             EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_SWITCH_SETUP_BIT__NRT_API_DELAY_Enable         EDDI_BIT_MASK_PARA(11,11)
#define EDDI_SER_SWITCH_SETUP_BIT__Confirmation_WR_Done         EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_SWITCH_SETUP_BIT__Extended_Delay               EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_SWITCH_SETUP_BIT__Res0                         EDDI_BIT_MASK_PARA(31,14)
#else
#define EDDI_SER_SWITCH_SETUP_BIT__res_6                        EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_SWITCH_SETUP_BIT__Res0                         EDDI_BIT_MASK_PARA(31,11)
#endif

#else  // REV 5

#define EDDI_SER_SWITCH_SETUP_BIT__Res400                       EDDI_BIT_MASK_PARA(10, 5)
#define EDDI_SER_SWITCH_SETUP_BIT__Res0                         EDDI_BIT_MASK_PARA(31,11)

#endif

/*===========================================================================*/

/*==================================*/
/*====== EDDI_SER_VERSION_TYPE =====*/
/*==================================*/
#define EDDI_SER_VERSION_BIT__Debug               EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER_VERSION_BIT__Version             EDDI_BIT_MASK_PARA(15,  8)
#define EDDI_SER_VERSION_BIT__MacroRevision       EDDI_BIT_MASK_PARA(23,16)
#define EDDI_SER_VERSION_BIT__Variante            EDDI_BIT_MASK_PARA(27, 24)
#define EDDI_SER_VERSION_BIT__Location            EDDI_BIT_MASK_PARA(31, 28)

/*=====================================*/
/*====== EDDI_SER_SWI_STATUS_TYPE =====*/
/*=====================================*/
#define EDDI_SER_SWI_STATUS_BIT__SWIStop                    EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_SER_SWI_STATUS_BIT__KonsStop                   EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_SER_SWI_STATUS_BIT__PHY_SMIStop                EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_SER_SWI_STATUS_BIT__StatiStop                  EDDI_BIT_MASK_PARA( 3, 3)

#define EDDI_SER_SWI_STATUS_BIT__TRCStop                    EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_SER_SWI_STATUS_BIT__KRAMStop                   EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_SER_SWI_STATUS_BIT__SRTStop                    EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_SER_SWI_STATUS_BIT__IRTStop                    EDDI_BIT_MASK_PARA( 7, 7)

#define EDDI_SER_SWI_STATUS_BIT__NRTStop                    EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_SER_SWI_STATUS_BIT__TRIGGERStop                EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_SER_SWI_STATUS_BIT__IRQStop                    EDDI_BIT_MASK_PARA(10,10)
#define EDDI_SER_SWI_STATUS_BIT__CYCStop                    EDDI_BIT_MASK_PARA(11,11)

#define EDDI_SER_SWI_STATUS_BIT__Port0Stop                  EDDI_BIT_MASK_PARA(12,12)
#define EDDI_SER_SWI_STATUS_BIT__Port1Stop                  EDDI_BIT_MASK_PARA(13,13)
#define EDDI_SER_SWI_STATUS_BIT__Port2Stop                  EDDI_BIT_MASK_PARA(14,14)
#define EDDI_SER_SWI_STATUS_BIT__Port3Stop                  EDDI_BIT_MASK_PARA(15,15)

/*===========================================================================*/

/*===========================================================================*/
/*==================== END OF EDDI_SER_STAT_CTRL ============================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Predefines for FCW-Control-Structure                                      */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_CCW_CTRL_HEAD_TYPE  EDDI_LOCAL_MEM_ATTR *  EDDI_CCW_CTRL_HEAD_PTR_TYPE;

typedef struct _EDDI_CCW_CTRL_TYPE  EDDI_LOCAL_MEM_ATTR *  EDDI_CCW_CTRL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// FCW-Control-Structure
//  - Represents a lower FCW-, ACW-, or EOL-Entry in KRAM
//
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_CCW_CTRL_TYPE
{
    EDDI_CCW_CTRL_PTR_TYPE        pNext;    // Pointer to next     SW-Control-Structure
    EDDI_CCW_CTRL_PTR_TYPE        pPrev;    // Pointer to previous SW-Control-Structure

    LSA_UINT16                    Opc;      // Opcode of lower KRAM-Control-Word
    LSA_UINT16                    FrameId;  //
    //  LSA_UINT16                   Port;
    //  LSA_UINT32                   Time;     // Time needed for FCWs and EOLs
    LSA_UINT16                    Size;     // Opcode of lower KRAM-Control-Word
    EDDI_SER10_FCW_ACW_PTR_TYPE   pKramCW;  // Pointer to KRAM-ControlWord
    LSA_UINT16                    DataLen;  // Datalen
    EDDI_CCW_CTRL_HEAD_PTR_TYPE   pCCWHead;

} EDDI_CCW_CTRL_TYPE;

/*---------------------------------------------------------------------------*/
/* FCW-Head-Control-Structure                                                */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_CCW_CTRL_HEAD_TYPE
{
    EDDI_CCW_CTRL_PTR_TYPE      pBottom;        //1st SW-Control-Element of List
    EDDI_CCW_CTRL_PTR_TYPE      pTop;           //2nd SW-Control-Element of List
    LSA_UINT32 *                pKramListHead;  //Pointer to an Entry in the FCW- or ACW-Header-Array

} EDDI_CCW_CTRL_HEAD_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TYP_H


/*****************************************************************************/
/*  end of file eddi_ser_typ.h                                               */
/*****************************************************************************/

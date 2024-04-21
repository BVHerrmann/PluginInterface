#ifndef EDDI_SER_ACW_FCW_H      //reinclude-protection
#define EDDI_SER_ACW_FCW_H

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
/*  F i l e               &F: eddi_ser_acw_fcw.h                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                           Switch-ASIC-Structs                             */
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

/*===========================================================================*/
/*=============================== FCW / ACW =================================*/
/*===========================================================================*/

#if defined (EDDI_CFG_REV7)
//#define IO_GET_CLK_COUNT_VALUE_10NS   (IO_R32(CLK_COUNT_VALUE) / 10);
#define IO_GET_CLK_COUNT_VALUE_10NS     eddi_getClockCount10ns(IO_R32(CLK_COUNT_VALUE))
#else
#define IO_GET_CLK_COUNT_VALUE_10NS     IO_R32(CLK_COUNT_VALUE)
#endif

// Needed to selelct ACW/FCW-rcv-irq
typedef enum _EDDI_SER10_IRQ_QUALIFIER_ENUM
{
    EDDI_SER10_IRQ_QUALIFIER_RCV1 = 0,
    EDDI_SER10_IRQ_QUALIFIER_RCV2 = 1,
    EDDI_SER10_IRQ_QUALIFIER_NO   = 2

} EDDI_SER10_IRQ_QUALIFIER_ENUM;

//  LL0  ----------------------------------------------
#define EDDI_SER10_LL0_COMMON_BIT__Opc                EDDI_BIT_MASK_PARA( 3, 0)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL0_COMMON_BIT__DstSrcPort         EDDI_BIT_MASK_PARA( 9, 4)
#else
#define EDDI_SER10_LL0_COMMON_BIT__DstSrcPort         EDDI_BIT_MASK_PARA( 7, 4)
#define EDDI_SER10_LL0_COMMON_BIT__reserved           EDDI_BIT_MASK_PARA( 9, 8)
#endif

#define EDDI_SER10_LL0_COMMON_BIT__IdxEthType         EDDI_BIT_MASK_PARA(11,10)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL0_COMMON_BIT__UDP                EDDI_BIT_MASK_PARA(12,12)
#else
#define EDDI_SER10_LL0_COMMON_BIT__Local_dont_use     EDDI_BIT_MASK_PARA(12,12)
#endif

#define EDDI_SER10_LL0_COMMON_BIT__FrameId            EDDI_BIT_MASK_PARA(28,13)
#define EDDI_SER10_LL0_COMMON_BIT__Len_2_0            EDDI_BIT_MASK_PARA(31,29)

#define EDDI_SER10_LL0_COMMON_BIT2__Len_10_3          EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_LL0_COMMON_BIT2__AppStatSel        EDDI_BIT_MASK_PARA(10, 8)
#define EDDI_SER10_LL0_COMMON_BIT2__pNext             EDDI_BIT_MASK_PARA(31,11)

//  LL0  ----------------------------------------------


#define EDDI_SER10_LL0_FCW_RCV_BIT__Opc                EDDI_BIT_MASK_PARA( 3, 0)

#define EDDI_SER10_LL0_FCW_RCV_BIT__DstSrcPort         EDDI_BIT_MASK_PARA( 7, 4)
#define EDDI_SER10_LL0_FCW_RCV_BIT__reserved           EDDI_BIT_MASK_PARA( 9, 8)

#define EDDI_SER10_LL0_FCW_RCV_BIT__IdxEthType         EDDI_BIT_MASK_PARA(11,10)

#define EDDI_SER10_LL0_FCW_RCV_BIT__Local              EDDI_BIT_MASK_PARA(12,12)

#define EDDI_SER10_LL0_FCW_RCV_BIT__FrameId            EDDI_BIT_MASK_PARA(28,13)
#define EDDI_SER10_LL0_FCW_RCV_BIT__Len_2_0            EDDI_BIT_MASK_PARA(31,29)

#define EDDI_SER10_LL0_FCW_RCV_BIT2__Len_10_3          EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_LL0_FCW_RCV_BIT2__AppStatSel        EDDI_BIT_MASK_PARA(10, 8)
#define EDDI_SER10_LL0_FCW_RCV_BIT2__pNext             EDDI_BIT_MASK_PARA(31,11)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL0_COMMON_TYPE
{
    EDDI_SER10_UINT64_TYPE         Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL0_COMMON_TYPE EDDI_SER10_LL0_COMMON_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL0_FCW_RCV_TYPE
{
    EDDI_SER10_UINT64_TYPE          Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL0_FCW_RCV_TYPE EDDI_SER10_LL0_FCW_RCV_TYPE;

/*=========== End LL0 =======================================================*/


/* -------------- LL1   ---------------*/
#define EDDI_SER10_LL1_ACW_RCV_BIT__pDB0            EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL1_ACW_RCV_BIT__Int2            EDDI_BIT_MASK_PARA(21,21)
#define EDDI_SER10_LL1_ACW_RCV_BIT__Int1            EDDI_BIT_MASK_PARA(22,22)
#define EDDI_SER10_LL1_ACW_RCV_BIT__CSrcAdr         EDDI_BIT_MASK_PARA(23,23)

#if ( defined(EDDI_CFG_REV7) )
#define EDDI_SER10_LL1_ACW_RCV_BIT__EXT             EDDI_BIT_MASK_PARA(24,24)
#define EDDI_SER10_LL1_ACW_RCV_BIT__GN_Rcv          EDDI_BIT_MASK_PARA(25,25)
#else
#define EDDI_SER10_LL1_ACW_RCV_BIT__ Reserved_0     EDDI_BIT_MASK_PARA(26,24)
#endif

#if ( defined(EDDI_CFG_REV6) || defined(EDDI_CFG_REV7) )
#define EDDI_SER10_LL1_ACW_RCV_BIT__IIIB            EDDI_BIT_MASK_PARA(27,27)
#else
#define EDDI_SER10_LL1_ACW_RCV_BIT__r5_IIIB         EDDI_BIT_MASK_PARA(27,27)
#endif

#define EDDI_SER10_LL1_ACW_RCV_BIT__Sync            EDDI_BIT_MASK_PARA(28,28)
#define EDDI_SER10_LL1_ACW_RCV_BIT__Async           EDDI_BIT_MASK_PARA(29,29)
#define EDDI_SER10_LL1_ACW_RCV_BIT__EnDiag          EDDI_BIT_MASK_PARA(30,30)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_ACW_RCV_BIT__RMA_Rcv         EDDI_BIT_MASK_PARA(31,31)
#else
#define EDDI_SER10_LL1_ACW_RCV_BIT__Reserved_1      EDDI_BIT_MASK_PARA(31,31)
#endif



#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_ACW_RCV_BIT2__PartDataStart        EDDI_BIT_MASK_PARA(10, 0)
#define EDDI_SER10_LL1_ACW_RCV_BIT2__Reserved_2           EDDI_BIT_MASK_PARA(15,11)
#define EDDI_SER10_LL1_ACW_RCV_BIT2__PartDataLength       EDDI_BIT_MASK_PARA(26,16)
#define EDDI_SER10_LL1_ACW_RCV_BIT2__Reserved_3           EDDI_BIT_MASK_PARA(30,27)
#define EDDI_SER10_LL1_ACW_RCV_BIT2__PD                   EDDI_BIT_MASK_PARA(31,31)
#else
#define EDDI_SER10_LL1_ACW_RCV_BIT2__r5_reserved          EDDI_BIT_MASK_PARA(31, 0)
#endif

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL1_ACW_RCV_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL1_ACW_RCV_TYPE EDDI_SER10_LL1_ACW_RCV_TYPE;

#define EDDI_SER10_LL1_FCW_RCV_BIT__pDB0                       EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL1_FCW_RCV_BIT__Int2                       EDDI_BIT_MASK_PARA(21,21)
#define EDDI_SER10_LL1_FCW_RCV_BIT__Int1                       EDDI_BIT_MASK_PARA(22,22)
#define EDDI_SER10_LL1_FCW_RCV_BIT__CSrcAdr                    EDDI_BIT_MASK_PARA(23,23)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_FCW_RCV_BIT__EXT                        EDDI_BIT_MASK_PARA(24,24)
#define EDDI_SER10_LL1_FCW_RCV_BIT__PD                         EDDI_BIT_MASK_PARA(25,25)
#define EDDI_SER10_LL1_FCW_RCV_BIT__T                          EDDI_BIT_MASK_PARA(26,26)
#define EDDI_SER10_LL1_FCW_RCV_BIT__Reserved_must_be_0         EDDI_BIT_MASK_PARA(27,27)
#else
#define EDDI_SER10_LL1_FCW_RCV_BIT__Reserved_must_be_0         EDDI_BIT_MASK_PARA(27,24)
#endif

#define EDDI_SER10_LL1_FCW_RCV_BIT__Sync              EDDI_BIT_MASK_PARA(28,28)
#define EDDI_SER10_LL1_FCW_RCV_BIT__Async             EDDI_BIT_MASK_PARA(29,29)
#define EDDI_SER10_LL1_FCW_RCV_BIT__EnDiag            EDDI_BIT_MASK_PARA(30,30)
#define EDDI_SER10_LL1_FCW_RCV_BIT__Reserved_2        EDDI_BIT_MASK_PARA(31,31)

#define EDDI_SER10_LL1_FCW_RCV_BIT2__Time             EDDI_BIT_MASK_PARA(31, 0)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL1_FCW_RCV_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL1_FCW_RCV_TYPE EDDI_SER10_LL1_FCW_RCV_TYPE;

#define EDDI_SER10_LL1_ACW_SND_BIT__pDB0                       EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL1_ACW_SND_BIT__Int2                       EDDI_BIT_MASK_PARA(21,21)
#define EDDI_SER10_LL1_ACW_SND_BIT__Int1                       EDDI_BIT_MASK_PARA(22,22)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_ACW_SND_BIT__GN_Snd                     EDDI_BIT_MASK_PARA(23,23)
#define EDDI_SER10_LL1_ACW_SND_BIT__EXT                        EDDI_BIT_MASK_PARA(24,24)
#else
#define EDDI_SER10_LL1_ACW_SND_BIT__Reserved_0                 EDDI_BIT_MASK_PARA(24,23)
#endif

#define EDDI_SER10_LL1_ACW_SND_BIT__Raw                        EDDI_BIT_MASK_PARA(25,25)
#define EDDI_SER10_LL1_ACW_SND_BIT__Tag                        EDDI_BIT_MASK_PARA(26,26)

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_ACW_SND_BIT__IIIB                       EDDI_BIT_MASK_PARA(27,27)
#else
#define EDDI_SER10_LL1_ACW_SND_BIT__r5_IIIB                    EDDI_BIT_MASK_PARA(27,27)
#endif

#define EDDI_SER10_LL1_ACW_SND_BIT__Sync                       EDDI_BIT_MASK_PARA(28,28)
#define EDDI_SER10_LL1_ACW_SND_BIT__Async                      EDDI_BIT_MASK_PARA(29,29)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_ACW_SND_BIT__RMA_Snd                    EDDI_BIT_MASK_PARA(30,30)
#else
#define EDDI_SER10_LL1_ACW_SND_BIT__Reserved_1                 EDDI_BIT_MASK_PARA(30,30)
#endif

#define EDDI_SER10_LL1_ACW_SND_BIT__DelayCtr                   EDDI_BIT_MASK_PARA(31,31)

#define EDDI_SER10_LL1_ACW_SND_BIT2__Time                      EDDI_BIT_MASK_PARA(31, 0)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL1_ACW_SND_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL1_ACW_SND_TYPE EDDI_SER10_LL1_ACW_SND_TYPE;

#define EDDI_SER10_LL1_FCW_SND_BIT__pDB0                EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL1_FCW_SND_BIT__Int2                EDDI_BIT_MASK_PARA(21,21)
#define EDDI_SER10_LL1_FCW_SND_BIT__Int1                EDDI_BIT_MASK_PARA(22,22)

#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL1_FCW_SND_BIT__CS                  EDDI_BIT_MASK_PARA(23,23)
#define EDDI_SER10_LL1_FCW_SND_BIT__EXT                 EDDI_BIT_MASK_PARA(24,24)
#define EDDI_SER10_LL1_FCW_SND_BIT__T                   EDDI_BIT_MASK_PARA(26,26)
#define EDDI_SER10_LL1_FCW_SND_BIT__PAD                 EDDI_BIT_MASK_PARA(27,27)
#else
#define EDDI_SER10_LL1_FCW_SND_BIT__Reserved_0          EDDI_BIT_MASK_PARA(27,23)
#endif

#define EDDI_SER10_LL1_FCW_SND_BIT__Sync                EDDI_BIT_MASK_PARA(28,28)
#define EDDI_SER10_LL1_FCW_SND_BIT__Async               EDDI_BIT_MASK_PARA(29,29)
#define EDDI_SER10_LL1_FCW_SND_BIT__GlobDest            EDDI_BIT_MASK_PARA(30,30)
#define EDDI_SER10_LL1_FCW_SND_BIT__DelayCtr            EDDI_BIT_MASK_PARA(31,31)

#define EDDI_SER10_LL1_FCW_SND_BIT2__Time               EDDI_BIT_MASK_PARA(31, 0)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL1_FCW_SND_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL1_FCW_SND_TYPE EDDI_SER10_LL1_FCW_SND_TYPE;

/*=========== End LL1 =======================================================*/


/* -------------- LL 2  ---------------*/
#define EDDI_SER10_LL2_COMMON_BIT__MacAdr0                  EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_LL2_COMMON_BIT__MacAdr1                  EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_SER10_LL2_COMMON_BIT__MacAdr2                  EDDI_BIT_MASK_PARA(23,16)
#define EDDI_SER10_LL2_COMMON_BIT__MacAdr3                  EDDI_BIT_MASK_PARA(31,24)

#define EDDI_SER10_LL2_COMMON_BIT2__MacAdr4                  EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_LL2_COMMON_BIT2__MacAdr5                  EDDI_BIT_MASK_PARA(15, 8)
#define EDDI_SER10_LL2_COMMON_BIT2__Vlan                     EDDI_BIT_MASK_PARA(31,16)


PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL2_COMMON_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL2_COMMON_TYPE EDDI_SER10_LL2_COMMON_TYPE;

/*=========== End LL2 =======================================================*/


/* -------------- LL 3  ---------------*/
#if defined (EDDI_CFG_REV7)
#define EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB                        EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL3_xCW_RCV_BIT__Reserved1                       EDDI_BIT_MASK_PARA(21,21)

#define EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus                        EDDI_BIT_MASK_PARA(22,22)
#define EDDI_SER10_LL3_xCW_RCV_BIT__Again                           EDDI_BIT_MASK_PARA(23,23)

#define EDDI_SER10_LL3_xCW_RCV_BIT__WDReloadVal                     EDDI_BIT_MASK_PARA(31,24)

#define EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal                    EDDI_BIT_MASK_PARA( 7, 0)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataStart                  EDDI_BIT_MASK_PARA(18, 8)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__Reserved2                      EDDI_BIT_MASK_PARA(19,19)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__PartDataLength                 EDDI_BIT_MASK_PARA(30,20)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__Reserved3                      EDDI_BIT_MASK_PARA(31,31)

#else
#define EDDI_SER10_LL3_xCW_RCV_BIT__pTimerSB                        EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LL3_xCW_RCV_BIT__Reserved1                       EDDI_BIT_MASK_PARA(23,21)

#if defined (EDDI_CFG_REV5)
#define EDDI_SER10_LL3_xCW_RCV_BIT__r5_res0                         EDDI_BIT_MASK_PARA(25,24)
#else
#define EDDI_SER10_LL3_xCW_RCV_BIT__DHStatus                        EDDI_BIT_MASK_PARA(24,24)
#define EDDI_SER10_LL3_xCW_RCV_BIT__Again                           EDDI_BIT_MASK_PARA(25,25)
#endif

#define EDDI_SER10_LL3_xCW_RCV_BIT__Reserved2                       EDDI_BIT_MASK_PARA(31,26)

#define EDDI_SER10_LL3_xCW_RCV_BIT2__WDReloadVal                    EDDI_BIT_MASK_PARA(12, 0)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__Reserved3                      EDDI_BIT_MASK_PARA(15,13)

#if defined (EDDI_CFG_REV5)
#define EDDI_SER10_LL3_xCW_RCV_BIT2__r5_DHReloadVal                 EDDI_BIT_MASK_PARA(28,16)
#else
#define EDDI_SER10_LL3_xCW_RCV_BIT2__DHReloadVal                    EDDI_BIT_MASK_PARA(28,16)
#endif

#define EDDI_SER10_LL3_xCW_RCV_BIT2__Reserved4                      EDDI_BIT_MASK_PARA(31,29)

#endif

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LL3_xCW_RCV_TYPE
{
    EDDI_SER10_UINT64_TYPE      Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LL3_xCW_RCV_TYPE EDDI_SER10_LL3_xCW_RCV_TYPE;

/*=========== End LL 3 =======================================================*/


/* -------------- LL 3 ,4, 5 ---------------*/

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

// Defines for 3-Buffer-Descriptor used in FundDBx
#define SER_FUND_FREE   0x00   // --> Buffer free available

#if defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
#define EDDI_IO_XCW_OFFSET_FOLLOWS     0x80000001UL         //acw-, fcw-offset is to follow
#define EDDI_IO_OFFSET_INVALID         0x80000003UL         //offset is invalid, either fcw-offset is to follow, or acw-offset
#define EDDI_IO_OFFSET_IS_ACW          0x80000000UL         //offset points to an acw
#define EDDI_IO_OFFSET_IS_FCW          0x40000000UL         //offset points to an fcw
#endif

#define EDDI_SER10_LLx_3B_IF_BIT__pDB1                           EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LLx_3B_IF_BIT__Reserved1                      EDDI_BIT_MASK_PARA(23,21)
#define EDDI_SER10_LLx_3B_IF_BIT__FundDB0                        EDDI_BIT_MASK_PARA(25,24)
#define EDDI_SER10_LLx_3B_IF_BIT__FundDB1                        EDDI_BIT_MASK_PARA(27,26)
#define EDDI_SER10_LLx_3B_IF_BIT__FundDB2                        EDDI_BIT_MASK_PARA(29,28)
#define EDDI_SER10_LLx_3B_IF_BIT__Reserved2                      EDDI_BIT_MASK_PARA(31,30)

#define EDDI_SER10_LLx_3B_IF_BIT2__pDB2                          EDDI_BIT_MASK_PARA(20, 0)
#define EDDI_SER10_LLx_3B_IF_BIT2__Reserved3                     EDDI_BIT_MASK_PARA(31,21)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LLx_3B_IF_TYPE
{
    EDDI_SER10_UINT64_TYPE        Value;
}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LLx_3B_IF_TYPE EDDI_SER10_LLx_3B_IF_TYPE;

#if defined (EDDI_CFG_REV7)
/* UDP */
#define EDDI_SER10_LLx_UDP_ACW_RCV_BIT__SourceIPAddr                               EDDI_BIT_MASK_PARA(31, 0)

#define EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__UDPSourcePort                             EDDI_BIT_MASK_PARA(15, 0)
#define EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__Reserved0                                 EDDI_BIT_MASK_PARA(29,16)
#define EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__CheckSrcIPAddr                            EDDI_BIT_MASK_PARA(31,30)
#define EDDI_SER10_LLx_UDP_ACW_RCV_BIT2__CheckSrcUDPPort                           EDDI_BIT_MASK_PARA(31,31)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LLx_UDP_ACW_RCV_TYPE
{
    EDDI_SER10_UINT64_TYPE           Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LLx_UDP_ACW_RCV_TYPE EDDI_SER10_LLx_UDP_ACW_RCV_TYPE;

#define EDDI_SER10_LLx_UDP_ACW_SND_BIT__DestIPAddr                                EDDI_BIT_MASK_PARA(31, 0)

#define EDDI_SER10_LLx_UDP_ACW_SND_BIT2__UDPDestPort                              EDDI_BIT_MASK_PARA(15, 0)
#define EDDI_SER10_LLx_UDP_ACW_SND_BIT2__Reserved0                                EDDI_BIT_MASK_PARA(25,16)
#define EDDI_SER10_LLx_UDP_ACW_SND_BIT2__IP_Select                                EDDI_BIT_MASK_PARA(27,26)
#define EDDI_SER10_LLx_UDP_ACW_SND_BIT2__UP_Select                                EDDI_BIT_MASK_PARA(29,28)
#define EDDI_SER10_LLx_UDP_ACW_SND_BIT2__Reserved1                                EDDI_BIT_MASK_PARA(31,30)

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_LLx_UDP_ACW_SND_TYPE
{
    EDDI_SER10_UINT64_TYPE           Value;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_LLx_UDP_ACW_SND_TYPE EDDI_SER10_LLx_UDP_ACW_SND_TYPE;

#endif

#endif

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_FCW_RCV_FORWARDER_TYPE
{
    EDDI_SER10_LL0_FCW_RCV_TYPE   Hw0;
    EDDI_SER10_LL1_FCW_RCV_TYPE   Hw1;
    //EDDI_SER10_LL2_COMMON_TYPE    Hw2;
    //EDDI_SER10_LL3_xCW_RCV_TYPE   Hw3;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_FCW_RCV_FORWARDER_TYPE EDDI_SER10_FCW_RCV_FORWARDER_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_FCW_RCV_TYPE
{
    EDDI_SER10_LL0_FCW_RCV_TYPE   Hw0;
    EDDI_SER10_LL1_FCW_RCV_TYPE   Hw1;
    EDDI_SER10_LL2_COMMON_TYPE    Hw2;
    EDDI_SER10_LL3_xCW_RCV_TYPE   Hw3;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_FCW_RCV_TYPE EDDI_SER10_FCW_RCV_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_ACW_RCV_TYPE
{
    EDDI_SER10_LL0_COMMON_TYPE    Hw0;
    EDDI_SER10_LL1_ACW_RCV_TYPE   Hw1;
    EDDI_SER10_LL2_COMMON_TYPE    Hw2;

    EDDI_SER10_LL3_xCW_RCV_TYPE   Hw3;

    #if defined (EDDI_CFG_REV7)
    EDDI_SER10_LLx_UDP_ACW_RCV_TYPE    Hw4;
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_SER10_LLx_3B_IF_TYPE          Hw45;
    #endif

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_ACW_RCV_TYPE EDDI_SER10_ACW_RCV_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_ACW_SND_TYPE
{
    EDDI_SER10_LL0_COMMON_TYPE      Hw0;
    EDDI_SER10_LL1_ACW_SND_TYPE     Hw1;
    EDDI_SER10_LL2_COMMON_TYPE      Hw2;

    #if defined (EDDI_CFG_REV7)
    EDDI_SER10_LLx_UDP_ACW_SND_TYPE  Hw3;
    #endif

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_SER10_LLx_3B_IF_TYPE       Hw34;
    #endif

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_ACW_SND_TYPE EDDI_SER10_ACW_SND_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_FCW_SND_TYPE
{
    EDDI_SER10_LL0_COMMON_TYPE    Hw0;
    EDDI_SER10_LL1_FCW_SND_TYPE   Hw1;
    EDDI_SER10_LL2_COMMON_TYPE    Hw2;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_FCW_SND_TYPE EDDI_SER10_FCW_SND_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SER10_FCW_ACW_TYPE
{
    EDDI_SER10_LL0_COMMON_TYPE             Common;

    EDDI_SER10_FCW_SND_TYPE                FcwSnd;
    EDDI_SER10_FCW_RCV_TYPE                FcwRcv;
    EDDI_SER10_FCW_RCV_FORWARDER_TYPE      FcwRcvForwarder;

    EDDI_SER10_ACW_SND_TYPE                AcwSnd;
    EDDI_SER10_ACW_RCV_TYPE                AcwRcv;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_SER10_FCW_ACW_TYPE EDDI_SER10_FCW_ACW_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE EDDI_SER10_FCW_ACW_TYPE  EDDI_LOWER_MEM_ATTR * EDDI_SER10_FCW_ACW_PTR_TYPE; //SER_DEV_FCW_PTR_TYPE;

// SRT-FCWs
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SER10_SRT_FCW_TYPE
{
    EDDI_SER10_LL0_COMMON_TYPE   Common;
    EDDI_SER10_UINT64_TYPE       Hw1;
    EDDI_SER10_UINT64_TYPE       Hw2;
    #if defined (EDDI_CFG_REV7)
    EDDI_SER10_UINT64_TYPE       Hw3;
    #endif

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SER10_SRT_FCW_TYPE EDDI_SER10_SRT_FCW_TYPE;

#include "pnio_pck_off.h"

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_ACW_FCW_H


/*****************************************************************************/
/*  end of file eddi_ser_acw_fcw.h                                           */
/*****************************************************************************/

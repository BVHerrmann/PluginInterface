#ifndef EDDI_NRT_INC_H          //reinclude-protection
#define EDDI_NRT_INC_H

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
/*  F i l e               &F: eddi_nrt_inc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  NRT-Defines constants, types, macros and prototyping for prefix.         */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* General Ethernet Header:                                                  */
/*                                                                           */
/*+   6   6   2     2     2                                                 +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+ |DA |SA |VLAN|VLAN |Type |                                              +*/
/*+ |   |   |TYPE| ID  |LEN  |                                              +*/
/*+ |   |   |    |     |     |                                              +*/
/*+ +---+---+----+-----+-----+                                              +*/
/*+          \--------/                                                     +*/
/*+           optional                                                      +*/
/*---------------------------------------------------------------------------*/

#define EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN             14
#define EDDI_FRAME_HEADER_SIZE_WITH_VLAN                18

#define EDDI_FRAME_LEN_TYPE_WORD_OFFSET                 6
#define EDDI_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET       8

/* if VLAN Tag is present */
#define EDDI_FRAME_VLAN_TYPE_WORD_OFFSET                EDDI_FRAME_LEN_TYPE_WORD_OFFSET
#define EDDI_FRAME_VLAN_ID_WORD_OFFSET                  7

#if defined (EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
#define EDDI_NRT_FRAME_BUFFER_MIN_LENGTH  64       /* min-size for sw-padding */
#endif

/*---------------------------------------------------------------------------*/
/* Frameformat for xRT over Ethernet                                         */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ mandatory on send      */
/* 12/16 | 2 |TypeLenxRT                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 | 2 |FrameID                              |                         */
/* 16/20 |   |Data                                 |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

#define EDDI_FRAME_XRT_FRAME_ID_WORD_OFFSET             7     /* without VLAN */
#define EDDI_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET   9

/*---------------------------------------------------------------------------*/
/* DCP via xRT-Format                                                        */
/*                                                                           */
/*+   6   6   2     2     2     2    1    1    4   2    2                   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+---+-----+---------...+   +*/
/*+ |DA |SA |VLAN|VLAN |Type |Frame|Serv|Serv|xid|len|resp |            |   +*/
/*+ |   |   |TYPE| ID  |LEN  | ID  | ID |type|   |   |delay| TLV-Fields |   +*/
/*+ |   |   |    |     |     |     |    |    |   |   |     |            |   +*/
/*+ +---+---+----+-----+-----+-----+----+----+---+---|-----+---------...+   +*/
/*+          \--------/                            |        \-----v--../    +*/
/*+           optional                             +--------------+         +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+  TLV-Field:                                                             +*/
/*+                                                                         +*/
/*+        2             2                                                  +*/
/*+ +--------------+-----------+--------------..+                           +*/
/*+ |      Type    |           |                |                           +*/
/*+ +---1---+--1---+   Length  |     Value      |                           +*/
/*+ | Option|SubOpt|           |                |                           +*/
/*+ +-------+------+-----------+--------------..+                           +*/
/*+                      |      \-------v----../                            +*/
/*+                      +--------------+                                   +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/

/* Offset values without VLAN-Tag: */
/* Important: All LEN-Field has to be WORD-algined. If not, access has to be */
/*            changed within coding!                                         */

#define EDDI_FRAME_DCP_FRAME_ID_OFFSET              14

//#define EDDI_FRAME_DCP_SERVICE_ID_OFFSET          16
//#define EDDI_FRAME_DCP_SERVICE_TYPE_OFFSET        17
//#define EDDI_FRAME_DCP_XID_OFFSET                 18
//#define EDDI_FRAME_DCP_RESP_DELAY_OFFSET          22
#define EDDI_FRAME_DCP_LEN_OFFSET                   24
#define EDDI_FRAME_DCP_TLV_OFFSET                   26

#define EDDI_FRAME_DCP_MC_HEADER_SIZE               26  //all fields till TLV
#define EDDI_FRAME_DCP_UC_HEADER_SIZE               26  //all fields till TLV

/* defines for SERVICE_ID:   */

//#define EDDI_DCP_SERVICE_ID_IDENTIFY_REQ          5

/* defines for SERVICE_TYPE: */

//#define EDDI_DCP_SERVICE_TYPE_MC_REQUEST_MSK      0x3
//#define EDDI_DCP_SERVICE_TYPE_MC_REQUEST          0x3

/* TLV offsets (from start of a TLV) */
#define EDDI_DCP_TLV_TYPE_OFFSET                    0
#define EDDI_DCP_TLV_LENGTH_OFFSET                  2
#define EDDI_DCP_TLV_VALUE_OFFSET                   4

#define EDDI_DCP_TLV_TLSIZE                         4
#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
#define EDDI_DCP_TLV_RES_BLOCKINFO_SIZE             2
#endif

/* NameOfStation-TLV: */
#if !defined (EDDI_CFG_DISABLE_DCP_HELLO_FILTER)
#define EDDI_DCP_DEVICE_PROPERTIES_OPTION           2   //option
#define EDDI_DCP_SUBOPTION_NAME_OF_STATION          2   //suboption
#endif

/*---------------------------------------------------------------------------*/
/* defines for SNAP-Type Frames                                              */
/*                                                                           */
/*+   6   6   2     1     1     1        5                                  +*/
/*+ +---+---+----+-----+-----+-----+-------------+-------                   +*/
/*+ |DA |SA |LEN |DSAP |SSAP |CTRL |   SNAP ID   | Data                     +*/
/*+ |   |   |    |     |     |     +--3---+---2--+                          +*/
/*+ |   |   |    |     |     |     | OUI  | Type |                          +*/
/*+ +---+---+----+-----+-----+-----+------+------+-------                   +*/
/*+                                                                         +*/
/*+            DSAP   SSAP     CTRL  OUI                          Type      +*/
/*+ DCP:       AAh    AAh      3h    8 0 6 (Siemens)              01FDh     +*/
/*+ Time       AAh    AAh      3h    8 0 6 (Siemens)              0100h     +*/
/*+ SINEC FWL  AAh    AAh      3h    8 0 6 (Siemens)              0110h     +*/
/*+            FDh    FDh      3h    8 0 6 (Siemens)              0110h     +*/
/*+ STDBY      AAh    AAh      3h    8 0 6 (Siemens)              01FBh     +*/
/*+ HSR        AAh    AAh      3h    0 8 0 63 (R. Hirschmann)     0800h     +*/
/*+                                                                         +*/
/*+                                                                         +*/
/*+ Note that optionally a VLAN-Tag may be present after LEN!!              +*/
/*+                                                                         +*/
/*---------------------------------------------------------------------------*/

/*
LITTLE: mit Microsoft und GNU und NRK Compiler
=======

       lb                  hb                   LB                  HB

|-|-|-|-|-|-|-|-|   |-|-|-|-|-|-|-|-|    |-|-|-|-|-|-|-|-|   |-|-|-|-|-|-|-|-|
 7 . . . . . . 0     F . . . . . . 8     17 . . . . . . 10   1F . . . . . . 18
 7 . . . . . . 0    15 . . . . . . 8     23 . . . . . . 16   31 . . . . . . 24


|-|-|-|-| |-|-|-|-|   |-|-|-|-| |-|-|-|-|    |-|-|-|-| |-|-|-|-|   |-|-|-|-| |-|-|-|-|
31    28  27    24    23    20  19    16     15    12  11     8     7     4   3     0

*/

#define  BM_S_DMACW_OwnerHW_29                      EDDI_PASTE_32(20,00,00,00)
#define  BM_S_DMACW_LocalCH_15                      EDDI_PASTE_32(00,00,80,00)

#if !defined (EDDI_CFG_REV5)
#define  BM_S_DMACW_SND_LearningDisable_23          EDDI_PASTE_32(00,80,00,00)
//#define  BM_S_DMACW_SND_MulticastLearningDisable_24  EDDI_PASTE_32(00,00,00,01)
#endif

#define  BM_S_DMACW_SND_RawMode_11                  EDDI_PASTE_32(00,00,08,00)

#if defined (EDDI_CFG_FRAG_ON)
#if !defined (EDDI_CFG_REV5)
#define  BM_S_DMACW_Send_Response_25                EDDI_PASTE_32(02,00,00,00)
#endif
#endif

#define  BM_S_DMACW_SND_PTFrame_26                  EDDI_PASTE_32(04,00,00,00)
#define  BM_S_DMACW_SND_SRTPrio_30                  EDDI_PASTE_32(40,00,00,00)

#define  BM_M_DMACW_FrameLength_0_10                EDDI_PASTE_32(00,00,07,FF)

#define  BM_M_DMACW_RCV_BufferLength_16_24          EDDI_PASTE_32(01,FF,00,00)
#define  BM_M_DMACW_RCV_BufferLength_SHIFT          16

#define  BM_M_DMACW_Port_12_13                      EDDI_PASTE_32(00,00,30,00)
#define  BM_M_DMACW_Port_SHIFT                      12

#define  BM_M_DMACW_Status_27_28                    EDDI_PASTE_32(18,00,00,00)
#define  BM_M_DMACW_Status_27_28_SHIFT_GET          27

#define  BM_M_NRT_LL0_DB_BIT__pNext_08_28           EDDI_PASTE_32(1F,FF,FF,00)
#define  BM_M_NRT_LL0_DB_BIT__pNext_SHIFT           8

#define  BM_M_NRT_LL0_FCW_BIT2__pNext_11_31         EDDI_PASTE_32(FF,FF,F8,00)
#define  BM_M_NRT_LL0_FCW_BIT2__pNext_SHIFT         11

#define  BM_M_NRT_NOOP_BIT2__pNext_11_31            EDDI_PASTE_32(FF,FF,F8,00)
#define  BM_M_NRT_NOOP_BIT2__pNext_SHIFT            11

//defines for NRT channel mapping:
//0 = map to NRT interface 1
//1 = map to NRT interface 0
#define EDDI_NRT_RCV_CHA_MAP_A0_ALONE   (  (1UL<<EDDI_NRT_PRIO_0_TAG_1_2)\
                                         | (1UL<<EDDI_NRT_PRIO_1_TAG_0_3)\
                                         | (1UL<<EDDI_NRT_PRIO_2_TAG_4_5)\
                                         | (1UL<<EDDI_NRT_PRIO_3_TAG_6_7)\
                                         | (1UL<<EDDI_NRT_PRIO_4_aSRTLow)\
                                         | (1UL<<EDDI_NRT_PRIO_5_cSRT)\
                                         | (1UL<<EDDI_NRT_PRIO_6_aSRTHigh)\
                                         | (1UL<<EDDI_NRT_PRIO_9_Fluss))
#define EDDI_NRT_RCV_CHA_MAP_B0_ALONE   (  (1UL<<EDDI_NRT_PRIO_3_TAG_6_7)\
                                         | (1UL<<EDDI_NRT_PRIO_7_iSRT)\
                                         | (1UL<<EDDI_NRT_PRIO_8_ORG))

/* macros */

#define NRT_GET_Rcv_Port(target, s32)  \
EDDI_GET_xxx(target, s32, BM_M_DMACW_Port_12_13, BM_M_DMACW_Port_SHIFT)

#define NRT_GET_Status(target, s32)  \
EDDI_GET_xxx(target, s32, BM_M_DMACW_Status_27_28, BM_M_DMACW_Status_27_28_SHIFT_GET)

#define NRT_GET_FrameLength(target, s32) \
EDDI_GET_xxx(target, s32, BM_M_DMACW_FrameLength_0_10, 0)

#define NRT_SET_SND_Port(target, port)  \
EDDI_SET_xxx(target, port, BM_M_DMACW_Port_12_13, BM_M_DMACW_Port_SHIFT)

#define NRT_SET_BufferLength(target, length)  \
EDDI_SET_xxx(target, length, BM_M_DMACW_RCV_BufferLength_16_24, BM_M_DMACW_RCV_BufferLength_SHIFT)

#define NRT_SET_FrameLength(target, length)  \
EDDI_SET_xxx(target, length, BM_M_DMACW_FrameLength_0_10, 0)

#define NRT_SET_NRT_LL0_DB_BIT__pNext(target, value)  \
EDDI_SET_0_xxx(target, value, BM_M_NRT_LL0_DB_BIT__pNext_08_28, BM_M_NRT_LL0_DB_BIT__pNext_SHIFT)

#define NRT_SET_NRT_LL0_FCW_BIT2__pNext(target, value)  \
EDDI_SET_0_xxx(target, value, BM_M_NRT_LL0_FCW_BIT2__pNext_11_31, BM_M_NRT_LL0_FCW_BIT2__pNext_SHIFT)

#define NRT_SET_NOOP_BIT2__pNext(target, value)  \
EDDI_SET_xxx(target, value, BM_M_NRT_NOOP_BIT2__pNext_11_31, BM_M_NRT_NOOP_BIT2__pNext_SHIFT)

#if defined (EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT)
#define EDDI_NRT_ENABLE_NRTRXTX_CHANNEL(Reg_, Val_)         \
{                                                           \
    PCIWriteCtr--;                                          \
    if (0 == PCIWriteCtr)                                   \
    {                                                       \
        PCIWriteCtr = EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT;      \
        IO_x32((Reg_)) = (LSA_UINT32)(Val_);                \
    }                                                       \
}
#else
#define EDDI_NRT_ENABLE_NRTRX_CHANNEL (Reg_, Val_)  {IO_x32((Reg_)) = (LSA_UINT32)(Val_);}
#endif //(EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT)

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_NRT_INC_H


/*****************************************************************************/
/*  end of file eddi_nrt_inc.h                                               */
/*****************************************************************************/

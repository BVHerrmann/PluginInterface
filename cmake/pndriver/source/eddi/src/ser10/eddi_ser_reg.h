#ifndef EDDI_SER_REG_H          //reinclude-protection
#define EDDI_SER_REG_H

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
/*  F i l e               &F: eddi_ser_reg.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                       internal Register of SER10                          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 D E F I N E                               */
/*===========================================================================*/

/*===============================*/
/*====== BASE-ADR-REGISTER ======*/
/*===============================*/

/*===========================================================================*/
/*===========================================================================*/

/*============================*/
/*====== FRAME-ID-SPACE ======*/
/*============================*/

//#define  MAX_NRT_TIME_FRAME_ID  0x007F
//#define  MAX_IRT_TIME_FRAME_ID  0x00FF
//#define  MAX_IRT_FRAME_ID       0xFBFF

#define IO_START     (pDDB->IRTE_SWI_BaseAdr_32Bit)

//the following macro contains no endianess-swapping
#define IO_x32(offset) (*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((IO_START) + (offset)))
//the following macros contain endianess-swapping
#define IO_R32(offset) EDDI_IRTE2HOST32((*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((IO_START) + (offset))))
#define IO_W32(offset, val) EDDI_HOST2IRTE32x(*(volatile EDDI_DEV_MEM_U32_PTR_TYPE)((IO_START) + (offset)), val)

#if defined (EDDI_CFG_LITTLE_ENDIAN)
#define EDDI_GET_xxx(target, s32, BitMask, ShiftValue)              \
target = (s32 & BitMask) >> ShiftValue;

#define EDDI_SET_xxx(target, val, BitMask, ShiftValue)              \
/*lint --e(941)  Result 0*/  (target = (target & ~(BitMask)) | ((val << ShiftValue) & BitMask))

#define EDDI_SET_0_xxx(target, val, BitMask, ShiftValue)            \
/*lint --e(941)  Result 0*/  (target = ((val << ShiftValue) & BitMask))

#else //EDDI_CFG_BIG_ENDIAN

#define EDDI_GET_xxx(target, s32, BitMask, ShiftValue)              \
{                                                                   \
LSA_UINT32  const  stmp32 = (s32) & BitMask;                        \
target = EDDI_SWAP_32(stmp32) >> ShiftValue;                        \
}

#define EDDI_SET_xxx(target, val, BitMask, ShiftValue)              \
{                                                                   \
LSA_UINT32  stmp32 = /*lint --e(941) Result 0*/ val << ShiftValue;  \
stmp32 = EDDI_SWAP_32(stmp32);                                      \
target = (target & ~(BitMask)) | (stmp32 & BitMask);                \
}

#define EDDI_SET_0_xxx(target, val, BitMask, ShiftValue)            \
{                                                                   \
LSA_UINT32  stmp32 = /*lint --e(941) Result 0*/ val << ShiftValue;  \
stmp32 = EDDI_SWAP_32(stmp32);                                      \
target = (stmp32 & BitMask);                                        \
}

#endif

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_ETH_PORT_X_TYPE ========================*/
/*===========================================================================*/

/*===============================*/
/*====== EDDI_SER_PORT_CTRL ======*/
/*===============================*/
//#define  ETH_RESET_RCV_CTRL   0x00000002  // 1-Bit, synchron Reset for RCV-Control
//#define  ETH_RESET_CT_CTRL     0x00000004  // 1-Bit, synchron Reset for IRTE-CT-Control
//#define  ETH_RESET_DB_CTRL   0x00000008  // 1-Bit, synchron Reset for IRTE/SRT-RCV-DB-Control
//#define  ETH_RESET_NRT_RCV_CTRL  0x00000010  // 1-Bit, synchron Reset for NRT-RCV-Control
//#define  ETH_RESET_TEST_RCV_CTRL  0x00000020  // 1-Bit, synchron Reset for RCV-TEST-Control
//#define  ETH_RESET_SRT_RCV_CTRL  0x00000040  // 1-Bit, synchron Reset for SRT-SND-Control
//#define  ETH_RESET_IRT_SND_CTRL  0x00000080  // 1-Bit, synchron Reset for IRT-SND-Control
//#define  ETH_RESET_NRT_SND_CTRL  0x00000100  // 1-Bit, synchron Reset for NRT-SND-Control
//#define  ETH_RESET_TEST_SND_CTRL  0x00000200  // 1-Bit, synchron Reset for TEST-SND-Control
//#define  ETH_RESET_SND_CTRl   0x00000400  // 1-Bit, synchron Reset for SND-Control
//#define  ETH_RESET_DIAG_CTRL   0x00000800  // 1-Bit, synchron Reset for DIAG-Control

/*==============================*/
/*====== EDDI_SER_IRT_DIAG ======*/
/*==============================*/
/* Receive-Unit */
//#define  ETH_DIAG_IRT_RCV   0x00000001  // 1-Bit, Diag-Entry for receiving a IRT-Frame
//#define  ETH_DIAG_TIME_RCV   0x00000002  // 1-Bit, Diag-Entry for receiving a TIME-Frame
//#define  ETH_DIAG_REPLACE_RCV  0x00000004  // 1-Bit, Diag-Entry for receiving a REPLACE-Frame
//#define  ETH_DIAG_SRT_RCV   0x00000008  // 1-Bit, Diag-Entry for receiving a SRT-Frame
//#define  ETH_DIAG_NRT_RCV   0x00000010  // 1-Bit, Diag-Entry for receiving a NRT-Frame
//#define  ETH_DIAG_NO_IRT_RCV   0x00000020  // 1-Bit, Diag-Entry for receiving no IRT-Frame
//#define  ETH_DIAG_IRT_ERR_RCV  0x00000040  // 1-Bit, Diag-Entry for receiving a IRT-Frame smaller 18Bytes
//#define  ETH_DIAG_FRAME_ID_ERR  0x00000080  // 1-Bit, Diag-Entry for receiving a Frame with wrong FrameId
//#define  ETH_DIAG_IRT_CRC_ERR  0x00000100  // 1-Bit, Diag-Entry for receiving a Frame with CRC-Errror
//#define  ETH_DIAG_BUF_OVFL_ERR  0x00000200  // 1-Bit, Diag-Entry by Buffer-Overflow
//#define  ETH_DIAG_LENGTH_ERR   0x00000400  // 1-Bit, Diag-Entry by Length-Error
//#define  ETH_DIAG_NO_ACW_ERR   0x00000800  // 1-Bit, Diag-Entry if no ACW for Time-Frame
//#define  ETH_DIAG_ACW_TIME_OUT_ERR 0x00001000  // 1-Bit, Diag-Entry for no ACW
//#define  ETH_DIAG_BUF_OVLOAD_ERR  0x00002000  // 1-Bit, Diag-Entry for Receive-Buffer-Overrun
//#define  ETH_DIAG_NO_ACW_NRT_ERR  0x00004000  // 1-Bit, Diag-Entry no ACW in NRT-Phase
/* Send-Unit*/
//#define  ETH_DIAG_REPLACE_SENT   0x00002000  // 1-Bit, Diag-Entry for sending Substitute-Frame
//#define  ETH_DIAG_TIME_ERR   0x00004000  // 1-Bit, Diag-Entry by Time over

/*===================================*/
/*====== EDDI_SER_NRT_CTRL_TYPE ======*/
/*===================================*/
//#define  ETH_10MBIT_MODE      0x00000001  // 1-Bit, 1: 10-MBit-Modus, 0 100-MBit-Modus
//#define  ETH_FULL_DUPLEX      0x00000002  // 1-Bit, 1: Fullduplex, 0: Halfduplex
//#define  ETH_CUT_THROUG_ENABLE    0x00000004  // 1-Bit, 1: Cut-Through 0: Store & Forward
//#define  ETH_SRT_PRIORITY_ENABLE    0x00000008  // 1-Bit, Frame with Type 0x8892 1: SRT-Priority 0: Default
//#define  ETH_LEARNING_ENABLE     0x00000010  // 1-Bit, 1: Learning  0: no Learning
//#define  ETH_MC_LEARNING_ENABLE    0x00000020  // 1-Bit, 1: Learning with Src-MAC-Adr all Frames
//    0: Learning only wit Src-MAC-Adr from Unicast-Frames
//#define  ETH_PADDING       0x00000040  // 1-Bit, 1: Padding from Frames <64 Bytes, 0: No Padding
//#define  ETH_INGRESS_FILTERING    0x00000080  // 1-Bit, 1: Receive only Frames with VLAN-Adr by Tag-Mode
//    0: Receive all Frames
//#define  ETH_CHECK_SA_ENABLE     0x00000100  // 1-Bit, 1: Filter Frames which SA is not included in the MAC-Adr-Table
//#define  ETH_ISRT_ENABLE      0x00000200  // 1-Bit, 1: Reserve ISRT-Time
//    0: No ISRT-Time
//#define  ETH_REDUNDANCE_ENABLE    0x00000400  // 1-Bit, 1: Enable Monitoring cRT-Frames
//    0: Disable Monitoring cRT-Frames
//#define  ETH_MONITORING_CYCLIC_ENABLE  0x00000800  // 1-Bit, 1: Enable Monitoring
//    0: Disable Monitoring
//#define  ETH_NOCHECK_FRAMELENGTH    0x00001000  // 1-Bit, 0: filter Frames with Length < 64 Bytes
//    1: no check of Frane-Lenght
//#define  ETH_MONITORING_ACYCLIC_ENABLE  0x00002000  // 1-Bit, 1: Enable Monitoring acyclic Frames
//    0: Disable Monitoring acyclic Frames

//PORT 0
/*===========================================================================*/

/*======================================*/
/*====== EDDI_SER_NRT_MAP_CHx_TYPE ======*/
/*======================================*/

//#define  NRT_PRIO_0     0x00000001  // 1-Bit, Receive Prio 0
//#define  NRT_PRIO_1     0x00000002  // 1-Bit, Receive Prio 1
//#define  NRT_PRIO_2     0x00000004  // 1-Bit, Receive Prio 2
//#define  NRT_PRIO_3     0x00000008  // 1-Bit, Receive Prio 3
//#define  NRT_PRIO_4     0x00000010  // 1-Bit, Receive Prio 4
//#define  NRT_PRIO_5     0x00000020  // 1-Bit, Receive Prio 5
//#define  NRT_PRIO_6     0x00000040  // 1-Bit, Receive Prio 6
//#define  NRT_PRIO_7     0x00000080  // 1-Bit, Receive Prio 7
//#define  NRT_PRIO_8     0x00000100  // 1-Bit, Receive Prio 8
//#define  NRT_PRIO_9     0x00000200  // 1-Bit, Receive Prio 9


/*=========================================*/
/*====== EDDI_SER_NRT_ENABLE_CHx_TYPE ======*/
/*=========================================*/

#define  NRT_CHANNEL_ENABLE_BIT   EDDI_PASTE_32(00,00,00,01)   // 1-Bit, 1: Set, 0: Reset
#define  NRT_CHANNEL_DISABLE_BIT  EDDI_PASTE_32(00,00,00,00)   // 1-Bit, 1: Set, 0: Reset
#define  NRT_TX_ENABLE            EDDI_PASTE_32(00,00,00,02)   // 1-Bit, Open/Close Send-Channel
#define  NRT_RX_ENABLE            EDDI_PASTE_32(00,00,00,04)   // 1-Bit, Open/Close Receive-Channel


/*===========================================================================*/


/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_IRT_CTRL_TYPE ==========================*/
/*===========================================================================*/

/*=========================*/
/*====== IO_IRT_CTRL ======*/
/*=========================*/

/*
#define  IRT_SND_PORT0_ENABLE  0x00000001  // 1-Bit, enable IRT-SND-P0
#define  IRT_RCV_PORT0_ENABLE  0x00000002  // 1-Bit, enable IRT-RCV-P0
#define  IRT_SND_PORT1_ENABLE  0x00000004  // 1-Bit, enable IRT-SND-P1
#define  IRT_RCV_PORT1_ENABLE  0x00000008  // 1-Bit, enable IRT-RCV-P1
#define  IRT_SND_PORT2_ENABLE  0x00000010  // 1-Bit, enable IRT-SND-P2
#define  IRT_RCV_PORT2_ENABLE  0x00000020  // 1-Bit, enable IRT-RCV-P2
#define  IRT_SND_PORT3_ENABLE  0x00000040  // 1-Bit, enable IRT-SND-P3
#define  IRT_RCV_PORT3_ENABLE  0x00000080  // 1-Bit, enable IRT-RCV-P3
*/

/*===========================================================================*/

/*===========================*/
/*====== IO_IMAGE_MODE ======*/
/*===========================*/

//#define  SER_SYNC_IMAGE_NO_MODE       0x0   // no Mode
#if defined(EDDI_CFG_REV7)
#define  SER_SYNC_IMAGE_UNBUF_MODE    0x1   // sync unbuffered
#endif
#define  SER_SYNC_IMAGE_BUF_MODE      0x2   // sync buffered
//#define  SER_SYNC_IMAGE_CHBUF_MODE    0x3   // sync double buffered

//#define     IMAGE_SYNC_API_MIN_TIME_IRT  0x0   // minimum (time compare, IRT-time)
//#define     IMAGE_SYNC_API_MIN_TIME_ISRT 0x1   // minimum (time compare, ISRT-time)
//#define     IMAGE_SYNC_API_TIME_COMPARE  0x2   // time compare
//#define     IMAGE_SYNC_API_TIME_COMPARE2 0x3   // time compare

//#define  IMAGE_ENABLE_STAND_BY   0x1   // with StandBy-Master
//#define  IMAGE_ENABLE_ASYNC_DMA   0x1   // with Quit for DMA



/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_SER_PHY_SMI_PORT_X =========================*/
/*===========================================================================*/
//#define  SMI_PHY_CON     0x00000000
//#define  SMI_PHY_STAT    0x00000001
//#define  SMI_PHY_IDENT0    0x00000002
//#define  SMI_PHY_IDENT1    0x00000003
//#define  SMI_PHY_AN_AV    0x00000004
//#define  SMI_PHY_AN_LP    0x00000005
//#define  SMI_PHY_AN_EX    0x00000006
//#define  SMI_PHY_AN_NPT    0x00000007
//#define  SMI_PHY_AN_LPRN    0x00000008
//#define  SMI_PHY_100CON    0x00000009
//#define  SMI_PHY_100STAT    0x0000000A

//#define  SMI_PHY_AMD_DIAG   0x00000012

//#define  SMI_PHY_AMD_DIAG_DPLX  0x00000800

/*===================================*/
/*====== EDDI_SER_SMI_PHY_CMD_TYPE ===*/
/*===================================*/
//#define  PHY_CMD_PHY_ADDR   0x00000001  // 5-Bit, PHY Adresse
//#define  PHY_CMD_ENABLE    0x00000020  // 1-Bit, Enable
//#define  PHY_CMD_PREAMP_SUP   0x00000040  // 1-Bit, Praemble Suppress

/*===================================*/
/*====== EDDI_SER_SMI_PHY_STAT_TYPE ==*/
/*===================================*/
//#define  PHY_STAT_EXT_CAP   0x00000001  // 1-Bit, Extended Capability
//#define  PHY_STAT_JAB_DET   0x00000002  // 1-Bit, Jabber Detect
//#define  PHY_STAT_LINK_STAT   0x00000004  // 1-Bit, Link Status
//#define  PHY_STAT_AUTO_NEG_ABL  0x00000008  // 1-Bit, Auto Negotiation Ability
//#define  PHY_STAT_REM_FAULT   0x00000010  // 1-Bit, Remote Fault
//#define  PHY_STAT_AUTO_NEG_COM  0x00000020  // 1-Bit, Auto Negotiation Complete
//#define  PHY_STAT_MF_PRE_SUP   0x00000040  // 1-Bit, MF Preamble Suppression
//#define  PHY_STAT_EXT_STAT   0x00000100  // 1-Bit, Extended Status
//#define  PHY_STAT_BASE100_HD   0x00000200  // 1-Bit, 100 Base T2 Half Duplex
//#define  PHY_STAT_BASE100_FD   0x00000400  // 1-Bit, 100 Base T2 Full Duplex
//#define  PHY_STAT_M10_HD    0x00000800  // 1-Bit, 10Mb/s Half Duplex
//#define  PHY_STAT_M10_FD    0x00001000  // 1-Bit, 10Mb/s Full Duplex
//#define  PHY_STAT_BASE100_X_HD  0x00002000  // 1-Bit, 100 Base X Half Duplex
//#define  PHY_STAT_BASE100_X_DD  0x00004000  // 1-Bit, 100 Base X Full Duplex
//#define  PHY_STAT_BASE100_T4   0x00008000  // 1-Bit, 100 Base T4

/*===================================*/
/*====== EDDI_SER_MAC_CTRL_TYPE ======*/
/*===================================*/
//#define  MAC_HALT_SND_RCV   0x00000001  // 1-Bit, stop Send & Recieve
//#define  MAC_HALT_SND_RCV_IMMED  0x00000002  // 1-Bit, stop Send & Recieve immediate
//#define  MAC_SOFT_RESET    0x00000004  // 1-Bit, reset all Ethernet-MAC
//#define  MAC_FULL_DUPLEX    0x00000008  // 1-Bit, 0:Half-Duplex, 1: Full-Duplex
//#define  MAC_LOOP        0x00000010  // 1-Bit, Loopback for sendet data
//#define     MAC_CON_MODE    0x00000020  // 2-Bit, Connection-Mode
//    00: Automatic
//    01: force 10-MBits/s
//    10: force MII
//#define  MAC_LOOP_10     0x00000080  // 1-Bit, set Loop by MAC of 10-MBits/s
//#define  MAC_LINK_CHANGED   0x00000100  // 1-Bit, set interrupt by link change
//#define  MAC_MISSED_ROLL    0x00000400  // 1-Bit, overflow of the Missed Error Counter
//#define  MAC_ENABLE_MISS    0x00002000  // 1-Bit, enable interrupt if MissedRoll == 1
//#define  MAC_LINK_STAT_10   0x00008000  // 1-Bit, status of the 10-MBit-Connection

/*===================================*/
/*====== EDDI_SER_ARC_CTRL_TYPE ======*/
/*===================================*/
//#define  ARC_STATION_ACCEPT   0x00000001  // 1-Bit, receive UniCast-Frame
//#define  ARC_GROUP_ACCEPT   0x00000002  // 1-Bit, receive MultiCast-Frame
//#define  ARC_BROAD_ACCEPT    0x00000004  // 1-Bit, receive BroadCast-Frame
//#define  ARC_FRAME_ACCEPT   0x00000008  // 1-Bit, 0: accept all Frames which known from ARC
//    1: accept all Frames which not known from ARC
//#define  ARC_COMPARE_ENABLE   0x00000010  // 1-Bit, enable compare of the Destinationaddress


/*=====================================*/
/*====== EDDI_SER_TRANS_CTRL_TYPE ======*/
/*=====================================*/
//#define  TRANS_ENABLE    0x00000001  // 1-Bit, enable Transmit
//#define  TRANS_HALT     0x00000002  // 1-Bit, stop Transmit
//#define  TRANS_SUPPR_PADDING   0x00000004  // 1-Bit, no padding for Frams smaller 64 Bytes
//#define  TRANS_SUPPR_CRC    0x00000008  // 1-Bit, no CRC append
//#define  TRANS_FAST_BACK_OFF_TIMER 0x00000010  // 1-Bit, use faster Back-off-Timer
//#define  TRANS_NO_EXESSIVE   0x00000020   // 1-Bit, no test of Exessive Deferral
//#define  TRANS_SEND_PAUSE   0x00000040  // 1-Bit, send Pause-Frame
//#define  TRANS_MII_10_MODE   0x00000080  // 1-Bit, activate SQE-Test in MII-10MBits/s-Mode
//enable interrupt
//#define  TRANS_IRQ_FIFO_UNDERRUN  0x00000100  // 1-Bit, enable Interrupt for MAC-Transmit-FIFO-Underrun
//#define  TRANS_IRQ_EXCE_DEFERAL  0x00000200  // 1-Bit, enable Interrupt for max. Deferral-Time
//#define  TRANS_IRQ_LOST_CARRIER  0x00000400  // 1-Bit, enable Interrupt for Carrier-Sense-Error
//#define  TRANS_IRQ_EXCESSIVE_COLL    0x00000800  // 1-Bit, enable Interrupt for more then 16 Collsions in Half-Duplex-Mode
//#define  TRANS_IRQ_LATE_COLLISION 0x00001000  // 1-Bit, enable Interrupt for Half-Duplex-Mode by collisions after 512TBit
//#define  TRANS_IRQ_TRANS_PARITY  0x00002000  // 1-Bit, enable Interrupt by Parity-Error in MAC-Transmit-FIFO
//#define  TRANS_IRQ_COMPLETE      0x00004000  // 1-Bit, enable Interrupt when Frame send

/*======================================*/
/*====== EDDI_SER_TRANS_STATE_TYPE ======*/
/*======================================*/
//#define     TRANS_COLL_CNT    0x00000001  // 1-Bit, number of collisions while transmitting one telegram
//#define  TRANS_EXCE_COLL_CNT   0x00000010  // 1-Bit, during Transmit more then 16 collisions
//#define  TRANS_DEFER     0x00000020  // 1-Bit, Frame is waiting for send
//#define  TRANS_PAUSED    0x00000040  // 1-Bit, waiting while pause
//#define  TRANS_IRQ_ON_TRANS   0x00000080  // 1-Bit, IRQ while sending a Frame
//#define  TRANS_UNDERRUN    0x00000100  // 1-Bit, MAC-Transmit-FIFO-Underrun
//#define  TRANS_EXCESS_DEFER   0x00000200  // 1-Bit, max Deferral-Time passed
//#define  TRANS_LOST_CARRIER   0x00000400  // 1-Bit, Carrier-Sense-Error
//#define  TRANS_10        0x00000800  // 1-Bit, Frame was received with 10MBits/s
//#define  TRANS_LATE_COLLISION  0x00001000  // 1-Bit, collision in Half-Duplex-Mode after 512TBit
//#define  TRANS_PARITY    0x00002000  // 1-Bit, send stoped by TransEn = 0 or TransHalt = 1
//#define  TRANS_COMPLETION   0x00004000  // 1_Bit, MAC-Unit has finished or cancelled transmitting a telegram
//#define  TRANS_TRANS_HALTED   0x00008000  // 1-Bit, 0:transmit enable ; 1:transmit halt request
//#define  TRANS_SIGNAL_QUALITY  0x00010000  // 1-Bit, could not receive Heart-Beat-Signal on the End
//#define  TRANS_MULTICAST_TRANS  0x00020000  // 1-Bit, MAC sent Multicast-Frame
//#define  TRANS_BROADCAST_TRANS  0x00040000  // 1-Bit, MAC sent Broadcast-Frame
//#define  TRANS_VLAN_TAGGED   0x00080000  // 1-Bit, MAC sent Frame with VLAN-Tagging
//#define  TRANS_MAC_CTRL_FRAME  0x00100000  // 1-Bit, MAC sent MAC-Control-Frame
//#define  TRANS_PAUSE     0x00200000  // 1-Bit, MAC sent PAUSE-Frame
//#define  TRANS_HOST_NO_RESP    0x00400000  // 1-Bit, ???

/*===================================*/
/*====== EDDI_SER_RCV_CTRL_TYPE ======*/
/*===================================*/
//#define  RCV_ENABLE     0x00000001  // 1-Bit, switch on/off Receive-Unit
//#define  RCV_HALT     0x00000002  // 1-Bit, stop Receive-Unit
//#define  RCV_LONG_ENABLE    0x00000004  // 1-Bit, receive Frames longer as 1518 Bytes
//#define  RCV_SHORT_ENABLE   0x00000008  // 1-Bit, recieve Frames shorter 63 Bytes
//#define  RCV_CRC_STRIP    0x00000010  // 1-Bit, by receiving Frame strip CRC
//#define  RCV_PASS_CTRL    0x00000020  // 1-Bit, Control-Frame on MAC-Interface
//#define  RCV_CRC_CHECK_OFF   0x00000040  // 1-Bit, no CRC-Check
//enable Interrupts
//#define  RCV_IRQ_ALIGN_ENABLE  0x00000100  // 1-Bit, enable interrupt by Alignment-Error
//#define  RCV_IRQ_CRC_ERROR_ENABLE 0x00000200  // 1-Bit, enable interrupt by CRC-Error
//#define  RCV_IRQ_OVERFLOW_ENABLE  0x00000400  // 1-Bit, enable interrupt by MAC-Receive-FIFO-Overflow
//#define  RCV_IRQ_LONG_ERROR_ENABLE 0x00000800  // 1-Bit, enable interrupt by Long-Frame
//#define  RCV_IRQ_PARITY_ENABLE  0x00002000  // 1-Bit, enable interrupt by Parity-Error
//#define  RCV_IRQ_GOOD_ENABLE   0x00004000  // 1-Bit, enable intterupt by receiving a Good-Frame

/*====================================*/
/*====== EDDI_SER_RCV_STATE_TYPE ======*/
/*====================================*/
//#define  RCV_LENGTH_ERROR   0x00000010  // 1-Bit, received Frame with wrong Length
//#define  RCV_FRAME_CTRL    0x00000020  // 1-Bit, received MAC-Control-Frame
//#define  RCV_IRQ_RCV     0x00000040  // 1-Bit, received a Frame
//#define  RCV_10      0x00000080  // 1-Bit, recieve Frame with 10MBits/s
//#define  RCV_ALIGN_ERROR    0x00000100  // 1-Bit, received Frame with Alignment-Error
//#define  RCV_CRC_ERROR    0x00000200  // 1-Bit, received Frame with CRC-Error
//#define  RCV_OVERFLOW_ERROR   0x00000400  // 1-Bit, MAC-Receive-FIFO-Overflow
//#define  RCV_LONG_ERROR    0x00000800  // 1-Bit, Frame-Length greater 1518 Bytes
//#define  RCV_PARITY_ERROR   0x00002000  // 1-Bit, MAC-Receive-Parity-Error
//#define  RCV_GOOD_RCV    0x00004000  // 1-Bit, received Frame wihtout Error
//#define  RCV_HALT_STAT    0x00008000  // 1-Bit, reception was stopped by RecvEn=0 & RecvHalt=1
//#define  RCV_MULTICAST_RCV   0x00020000  // 1-Bit, received MultiCast-Frame
//#define  RCV_BROADCAST_RCV   0x00040000  // 1-Bit, received BroadCast-Frame
//#define  RCV_VLAN_RCV    0x00080000  // 1-Bit, received Vlan-Frame
//#define  RCV_PAUSE_REV    0x00100000  // 1-Bit, received Pause-Frame
//#define  RCV_ARC_STATE    0x00200000  // 4-Bit, state of the ARC-unit
//#define  RCV_ARC_ENT     0x02000000  // 5-Bit, Offset-Address of ARC-Memeory

/*================================*/
/*====== EDDI_SER_MD_MC_TYPE ======*/
/*================================*/
//#define  MD_MC_PHY_REG_ADDR   0x00000001  // 5-Bit, PHY-Register-Address
//#define  MD_MC_PHY_ADDR    0x00000020  // 5-Bit, PHY-Address
//#define  MD_MC_WRITE        0x00000400  // 1-Bit, 0: read PHY-Register; 1: write PHY-Register
//#define  MD_MC_BUSY     0x00000800  // 1-Bit, 0: no read or write possible
//    1: after set -> read/write possible
//#define  MD_MC_NO_PREAMPLE   0x00001000  // 1-Bit, 1: do not send Preamble to PHY

/*====================================*/
/*====== EDDI_SER_PROM_CTRL_TYPE ======*/
/*====================================*/
//#define  PROM_ADDR     0x00000001  // 6-Bit, PROM-Address
//#define  PROM_OPCODE     0x00000200  // 2-Bit, 10: read
//    01: write
//    00: enable or disable read/write
//    11: clear
//#define  PROM_BEGIN_OPERATION  0x00000800  // 1-Bit, set by begin of an Operation

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================= EDDI_DEV_KRAM_CTRL_TYPE ===========================*/
/*===========================================================================*/

/*===========================================*/
/*===== EDDI_SER_NRT_DEFAULT_CTRL_TYPE ======*/
/*===========================================*/

#define  NRT_DEF_CTRL_CHA      0x00000001  // 1-Bit, Forward NRT-Frame to CHA
#define  NRT_DEF_CTRL_CHB      0x00000002  // 1-Bit, Forward NRT-Frame to CHB
//#define  NRT_DEF_CTRL_DST_P0   0x00000004  // 1-Bit, Forward NRT-Frame to P0
//#define  NRT_DEF_CTRL_DST_P1   0x00000008  // 1-Bit, Forward NRT-Frame to P1
//#define  NRT_DEF_CTRL_DST_P2   0x00000010  // 1-Bit, Forward NRT-Frame to P2
//#define  NRT_DEF_CTRL_DST_P3   0x00000020  // 1-Bit, Forward NRT-Frame to P3
/*========================================*/
/*===== EDDI_SER_NRT_API_CLEAR_TYPE ======*/
/*========================================*/

#define  COMMAND_ACTIVATE         0x1    // progress Command
#define  PREV_PTR_IS_LIST_HEADER  0x200000  // Used in context of Command "InsertIntoAcwSendList" and "
// "RemoveFromAcwSendList" if the pointers is the Listheader.
// Needed to Mark one of the two pointers in a 64-Bit-ACWListHeader

#define  FCODE_ENABLE_ACW_RX_LIST    (LSA_UINT16)0x0000   // Enable ACW-Receive-List
//#define  FCODE_DISALBE_ACW_RX_LIST (LSA_UINT16) 0x0001   // Disable ACW-Receive-List
#define  FCODE_ENABLE_ACW_TX_LIST     (LSA_UINT16)0x0002   // Enable ACW-Send-List
#define  FCODE_DISABLE_ACW_TX_LIST    (LSA_UINT16)0x0003   // Disable ACW-Send-List
#define  FCODE_INSERT_ACW_TX          (LSA_UINT16)0x0004   // Insert Send-ACW
#define  FCODE_REMOVE_ACW_TX          (LSA_UINT16)0x0005   // Remove Send-ACW
#define  FCODE_INSERT_ACW_RX          (LSA_UINT16)0x0006   // Insert Receive-ACW
#define  FCODE_REMOVE_ACW_RX          (LSA_UINT16)0x0007   // Remove Receive-ACW
#define  FCODE_AGE_FDB                (LSA_UINT16)0x0008   // Age Address in UC-MC-Address-Table
#define  FCODE_RESERVE_ADR_ENTRY      (LSA_UINT16)0x0009   // Reserve Address Entry
#define  FCODE_RESET_TX_QUEUE         (LSA_UINT16)0x000A   // Reset Send Queue
#define  FCODE_CLEAR_FDB              (LSA_UINT16)0x000B   // Clear UCMC-Address-Table
#define  FCODE_CLEAR_FDB_ENTRY        (LSA_UINT16)0x000C   // Clear UCMC-Address-Table Entry

#define  FCODE_ENABLE_DATA_IMAGE      (LSA_UINT16)0x000D   // Enable Data Image
#define  FCODE_DISABLE_DATA_IMAGE     (LSA_UINT16)0x000E   // Disable Data Image

//#define  FCODE_INSERT_FCW_TX          (LSA_UINT16)0x000F   // Insert Send-FCW
//#define  FCODE_REMOVE_FCW_TX          (LSA_UINT16)0x0010   // Remove Send-FCW
//#define  FCODE_INSERT_FCW_RX          (LSA_UINT16)0x0011   // Insert Receive-FCW
//#define  FCODE_REMOVE_FCW_RX          (LSA_UINT16)0x0012   // Remove Receive-FCW

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#define  FCODE_RELOAD_SB_ENTRY        (LSA_UINT16)0x0013   // reload TimerScoreBoard-Entry
#endif

#if defined (EDDI_CFG_REV7)
#define  FCODE_MOVE_RCV_ACW_DB        (LSA_UINT16)0x0017
#endif
//#define  FCODE_CLEAR_REDUNDANCE_STATE             0x0016   // Clear the consumer redundance state consistent

//#define   FCODE_HIGHEST_COMMAND_NR    (LSA_UINT16)0x0017  //highest command-nr available (for statistic)

/*====================================*/
/*===== EDDI_SER_PORT_CTRL_TYPE ======*/
/*====================================*/

//#define  PORT_CTRL_SET    0x00000001  // Bit 0: Command
//#define  PORT_CTRL_PAUSE_P0   0x00000002  // Bit 1: PausP0
//#define  PORT_CTRL_PAUSE_P1   0x00000004  // Bit 2: PausP1
//#define  PORT_CTRL_PAUSE_P2   0x00000008  // Bit 3: PausP2
//#define  PORT_CTRL_PAUSE_P3   0x00000010  // Bit 4: PausP3

/*=======================================*/
/*===== EDDI_SER_NRT_API_CTRL_TYPE ======*/
/*=======================================*/

#define  NRT_API_CTRL_RESET   0x00000001  // 1-Bit, Bit 0: open NRT-API-Port for Bit(0..3)
//   Bit 1: close NRT-API-Port for Bit(0..3)
#define  NRT_API_CTRL_CHA0   0x00000002  // 1-Bit, CHA0
#define  NRT_API_CTRL_CHA1   0x00000004  // 1-Bit, CHA1
#define  NRT_API_CTRL_CHB0   0x00000008  // 1-Bit, CHB0
#define  NRT_API_CTRL_CHB1   0x00000010  // 1-Bit, CHB1

/*===========================================================================*/

/*=========================*/
/*====== IO_TRC_MODE ======*/
/*=========================*/

//#define  TRC_RESET     0x00000001  // 1-Bit, reset Trace-Unit
//#define  TRC_STOP     0x00000002  // 1-Bit, stop Trace-Unit
#define  TRC_START          EDDI_PASTE_32(00,00,00,04)  // 1-Bit, start Trace-Unit
#define  TRC_BUF_RING_MODE  EDDI_PASTE_32(00,00,00,08)  // 1-Bit, Trace-Buffer in Ring-Mode
#define  TRC_EXECUTE        EDDI_PASTE_32(00,00,01,00)  // 1-Bit, Trace-Unit execute

/*===========================================================================*/

/*============================*/
/*====== IO_IRT_CYCL_ID ======*/
/*============================*/

#define  PARA_EOF_APP     0x00000040  // 1-Bit, END-of-Application
#define  PARA_START_APP     0x00000080  // 1-Bit, START-of-Application

//#define  PARA_START_COMMUNICATION  0x00000100  // 1-Bit, START-of-Communication

#define  PARA_ENABLE_COMP1_IRQ   0x00000200  // 1-Bit, Enable IRQ for COMP1
#define  PARA_ENABLE_COMP2_IRQ   0x00000400  // 1-Bit, Enable IRQ for COMP2
#define  PARA_ENABLE_COMP3_IRQ   0x00000800  // 1-Bit, Enable IRQ for COMP3
#define  PARA_ENABLE_COMP4_IRQ   0x00001000  // 1-Bit, Enable IRQ for COMP4
#define  PARA_ENABLE_COMP5_IRQ   0x00002000  // 1-Bit, Enable IRQ for COMP5

/*=========================*/
/*====== IO_SWI_CTRL ======*/
/*=========================*/

#define  PARA_SWI_ENABLE          0x00000001  // 1-Bit, enable Switch-Makro
//#define  PARA_SOFT_RESET          0x00000002  // 1-Bit, Soft-Reset
#define  PARA_PHY_SMI_CTRL_ENABLE 0x00000004  // 1-Bit, enable PHY/SMI-Control
//JM #define  PARA_STATI_CTRL_ENABLE   0x00000008  // 1-Bit, enable Statistic-Control
#define  PARA_TRACE_CTRL_ENABLE   0x00000010  // 1-Bit, enable IRT-TRACE-Control
#define  PARA_KRAM_CTRL_ENABLE    0x00000020  // 1-Bit, enable KRAM-Control
#define  PARA_SRT_CTRL_ENABLE     0x00000040  // 1-Bit, enable SRT-Control
#define  PARA_IRT_CTRL_ENABLE     0x00000080  // 1-Bit, enable IRT-Control
#define  PARA_NRT_CTRL_ENABLE     0x00000100  // 1-Bit, enable NRT-Control
//#define  PARQ_TRIG_UNIT_ENABLE  0x00000200  // 1-Bit, enable TRIGGER-UNIT
#define  PARA_IRQ_CTRL_ENABLE     0x00000400  // 1-Bit, enable IRQ-Control
#define  PARA_CYC_CTRL_ENABLE     0x00000800  // 1-Bit, eanble CYCL-SYNC-Control
#define  PARA_PORT0_ENABLE        0x00001000  // 1-Bit, enable Port0
#define  PARA_PORT1_ENABLE        0x00002000  // 1-Bit, enable Port1
#define  PARA_PORT2_ENABLE        0x00004000  // 1-Bit, enable Port2
#define  PARA_PORT3_ENABLE        0x00008000  // 1-Bit, enable Port3

/*===============================*/
/*====== IO_NRT_TRANS_CTRL ======*/
/*===============================*/
// #define  PARA_TAG_MODE    0x00000001  // 1-Bit, 0: check Tag

/*==================================*/
/*====== IO_SC_BUS_FAULT_INFO ======*/
/*==================================*/

//#define  BUS_READ_ERROR    0x00000001  // 1-Bit, 1:Read-Error; 0:Write-Error
//#define  BUS_BURST_ERROR    0x00000002  // 1-Bit, 1:one Acces-Error; 0: Burst-Error
//#define  BUS_ADRESS_ERROR   0x00000004  // 1-Bit, Adress-Error
//#define  BUS_SLAVE_ERROR    0x00000008  // 1-Bit, not used
//#define  BUS_BEN_ERROR    0x00000010  // 1-Bit, Byte-Enable-Error
//#define  BUS_BEN      0x00000020  // 1-Bit, Byte-Enable ???

/*=================================*/
/*====== IO_K_BUS_FAULT_INFO ======*/
/*=================================*/

//#define   BUS_Port0_ERROR   0x00000001  // 1-Bit, 1: Port0
//#define   BUS_Port1_ERROR   0x00000002  // 1-Bit, 1: Port1
//#define   BUS_Port2_ERROR   0x00000004  // 1-Bit, 1: Port2
//#define   BUS_Port3_ERROR   0x00000008  // 1-Bit, 1: Port3
//#define   BUS_Port4_ERROR   0x00000010  // 1-Bit, 1: Port4
//#define   BUS_Port5_ERROR   0x00000020  // 1-Bit, 1: Port5
//#define   BUS_Port6_ERROR   0x00000040  // 1-Bit, 1: Port6
//#define   BUS_Port7_ERROR   0x00000080  // 1-Bit, 1: Port7
//#define   BUS_DMAUnit_ERROR  0x00000100  // 1-Bit, 1: DMA-Unit
//#define   BUS_NRTCtrl_ERROR  0x00000200  // 1-Bit, 1: NRT-Control
//#define   BUS_IRTCtrl_ERROR  0x00000400  // 1-Bit, 1: IRT-Control
//#define   BUS_SRTCtrl_ERROR  0x00000800  // 1-Bit, 1: SRT-Control
//#define   BUS_KRAMCtrl_ERROR  0x00001000  // 1-Bit, 1: KRAM-Control
//#define   BUS_TRACECtr_ERROR  0x00002000  // 1-Bit, 1: TRACE-Control
//#define   BUS_STATCtrl_ERROR  0x00004000  // 1-Bit, 1: STATISTIC-Control
//#define   BUS_CKBus_ERROR   0x00008000  // 1-Bit, 1: CK-Buskoppler
//#define   BUS_Access    0x00800000  // 1-Bit, 1: Read, 0: Write
//#define   BEN_0     0x01000000  // 1-Bit, Byte-Enaable-Signal BEN_0
//#define   BEN_1     0x02000000  // 1-Bit, Byte-Enaable-Signal BEN_1
//#define   BEN_2     0x04000000  // 1-Bit, Byte-Enaable-Signal BEN_2
//#define   BEN_3     0x08000000  // 1-Bit, Byte-Enaable-Signal BEN_3
//#define   BEN_4     0x10000000  // 1-Bit, Byte-Enaable-Signal BEN_4
//#define   BEN_5     0x20000000  // 1-Bit, Byte-Enaable-Signal BEN_5
//#define   BEN_6     0x40000000  // 1-Bit, Byte-Enaable-Signal BEN_6
//#define   BEN_7     0x80000000  // 1-Bit, Byte-Enaable-Signal BEN_7

/*===========================================================================*/

/*============================*/
/*====== IO_CLK_COMMAND ======*/
/*============================*/

#define  START_CYCL_CLK               0x00000001  // 1-Bit
#define  START_APP_CLK                0x00000002  // 1-Bit
#define  ENABLE_MULTICYCLE_APL_CLOCKS 0x00000004  // 1-Bit

/*============================*/
/*==== Min Preamble 100Mbit ==*/
/*============================*/

#define MIN_PREAMBLE_100MBIT          0x8U //Minimum 1 byte preamble

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_REG_H


/*****************************************************************************/
/*  end of file eddi_ser_reg.h                                               */
/*****************************************************************************/

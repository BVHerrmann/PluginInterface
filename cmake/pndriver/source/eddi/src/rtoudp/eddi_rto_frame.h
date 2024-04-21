#ifndef EDDI_RTO_FRAME_H        //reinclude-protection
#define EDDI_RTO_FRAME_H

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
/*  F i l e               &F: eddi_rto_frame.h                          :F&  */
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
/*                                                                           */
/*****************************************************************************/


/*---------------------------------------------------------------------------*/
/* Frameformat for cyclic SRT over UDP                                       */
/*                                                                           */
/* Offset|Len|Fields with VLAN                     |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*     0 | 6 |DST-MAC                              |                         */
/*     6 | 6 |SRC-MAC                              |                         */
/*    12 | 2 |TypeLenVLAN                          |\ optional on recv       */
/*    14 | 2 |VLAN-ID                              |/ not inserted on send   */
/* 12/16 | 2 |IP-Type                              |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 14/18 |20 |IPHeader                             |  we only accept 20 Byte */
/* ------+---+-------------------------------------+-------------------------*/
/* 34/38 | 8 |UDP-Header                           |                         */
/* ------+---+-------------------------------------+-------------------------*/
/* 42/46 | 2 |FrameID                              |                         */
/* 44/48 |   |I/O-Data                             |                         */
/*       |   |:                                    |                         */
/*       |   |:                                    |  xRT-Data               */
/*       |   |:                                    |                         */
/* ------+---+-------------------------------------+                         */
/*       | 2 |CycleCounter                         |                         */
/*       | 1 |DataStatus                           |                         */
/*       | 1 |TransferStatus                       |                         */
/* ------+---+-------------------------------------+-------------------------*/
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* size of framecomponents before and after I/O-Data */
/* (excluding VLAN-Tag!)                             */


/*---------------------------------------------------------------------------*/
/* VLAN - Tag fields                                                         */
/* Network-Format:                                                           */
/*                                                                           */
/*               8 7 6    5    4 3 2 1                                       */
/*              +--------------------+                                       */
/*   octet 1    | Prio | CFI |  VID  |                                       */
/*              +--------------------+                                       */
/*   octet 2    |       VID          |                                       */
/*              +--------------------+                                       */
/*                                                                           */
/*  15.13 12  11         ...         0                                       */
/*  +----+---+-----------------------+                                       */
/*  |Prio|CFI|         VID           |                                       */
/*  +----+---+-----------------------+                                       */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/* Bits 0..11 */

#define EDDI_SRT_VLAN_ID_MSK            0x0FFF
//#define EDDI_SRT_VLAN_ID_SHIFT        0

/* Bit  12    */

//#define EDDI_SRT_VLAN_CFI_MSK         0x1000
//#define EDDI_SRT_VLAN_CFI_SHIFT       12

/* Bits 13..15*/

//#define EDDI_SRT_VLAN_PRIO_MSK        0xE000
#define EDDI_SRT_VLAN_PRIO_SHIFT        13

//#define EDDI_SRT_VLAN_PRIO_ASRT_HIGH  6
#define EDDI_SRT_VLAN_PRIO_CSRT         6
//#define EDDI_SRT_VLAN_PRIO_ASRT_LOW   5

/* xSRT-Header */
#define EDDI_UDP_FRAME_SET_VLAN_TAG(pBuffer) \
(((EDD_UPPER_MEM_U16_PTR_TYPE )(void *)pBuffer)[EDDI_FRAME_VLAN_TYPE_WORD_OFFSET])

#define EDDI_UDP_FRAME_SET_VLAN_ID(pBuffer) \
(((EDD_UPPER_MEM_U16_PTR_TYPE )(void *)pBuffer)[EDDI_FRAME_VLAN_ID_WORD_OFFSET])

#define EDDI_UDP_FRAME_SET_TYPELEN(pBuffer) \
(((EDD_UPPER_MEM_U16_PTR_TYPE )(void *)pBuffer)[EDDI_FRAME_LEN_TYPE_WITH_VLAN_WORD_OFFSET])

//#define EDDI_UDP_FRAME_SET_FRAMEID(pBuffer) (((EDD_UPPER_MEM_U16_PTR_TYPE )(void *)pBuffer)[EDDI_FRAME_XRT_FRAME_ID_WITH_VLAN_WORD_OFFSET])

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_RTO_FRAME_H


/*****************************************************************************/
/*  end of file eddi_rto_frame.h                                             */
/*****************************************************************************/

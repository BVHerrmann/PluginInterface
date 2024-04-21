#ifndef EDDI_SWI_PTP_H          //reinclude-protection
#define EDDI_SWI_PTP_H

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
/*  F i l e               &F: eddi_swi_ptp.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SWITCH                                                                   */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

#define  EDDI_MAC_BROADCAST               { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }

#define  EDDI_MAC_PROFINET                { 0x01, 0x0E, 0xCF, 0x00, 0x00, 0x00 }
#define  EDDI_MAC_PROFINET_FF             { 0x03, 0x00, 0x00, 0x00, 0x00, 0x00 }
/* IEC - PNIO */
#define  EDDI_MAC_RT_3_DA_BEGIN           { 0x01, 0x0E, 0xCF, 0x00, 0x01, 0x01 }
#define  EDDI_MAC_RT_3_INVALID_DA_BEGIN   { 0x01, 0x0E, 0xCF, 0x00, 0x01, 0x02 }
#define  EDDI_MAC_RT_2_QUER_BEGIN         { 0x01, 0x0E, 0xCF, 0x00, 0x02, 0x00 }
#define  EDDI_MAC_PTP_ANNOUNCE_BEGIN      { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x00 }
#define  EDDI_MAC_TIME_ANNOUNCE_BEGIN     { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x01 }
#define  EDDI_MAC_PTP_SYNC_WITH_FU_BEGIN  { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x20 }
#define  EDDI_MAC_TIME_SYNC_WITH_FU_BEGIN { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x21 }
#define  EDDI_MAC_PTP_FOLLOW_UP_BEGIN     { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x40 }
#define  EDDI_MAC_TIME_FOLLOW_UP_BEGIN    { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x41 }
#define  EDDI_MAC_PTP_SYNC_BEGIN          { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x80 }
#define  EDDI_MAC_TIME_SYNC_BEGIN         { 0x01, 0x0E, 0xCF, 0x00, 0x04, 0x81 }
/* IEC - MRP 1 */
#define  EDDI_MAC_MRP_1_BEGIN             { 0x01, 0x15, 0x4E, 0x00, 0x00, 0x01 }      //when changing check all locations with MAC_MRP_1_LEN!
/* IEC - MRP 2 */
#define  EDDI_MAC_MRP_2_BEGIN             { 0x01, 0x15, 0x4E, 0x00, 0x00, 0x02 }      //when changing check all locations with MAC_MRP_2_LEN!
/* IEC - MRP IC 1 */
#define  EDDI_MAC_MRP_IC_1_BEGIN          { 0x01, 0x15, 0x4E, 0x00, 0x00, 0x03 }      
/* IEC - MRP IC 2 */
#define  EDDI_MAC_MRP_IC_2_BEGIN          { 0x01, 0x15, 0x4E, 0x00, 0x00, 0x04 }      
/* IEC - HSYNC */
#define  EDDI_MAC_HSYNC_BEGIN             { 0x01, 0x0E, 0xCF, 0x00, 0x05, 0x00 }
/* IEEE Reserved addresses - 1 00-00-00 - 00-00-0D */
#define  EDDI_MAC_IEEE_RESERVED_BEGIN_1   { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x00 }
/* IEEE Reserved addresses - 1 00-00-01 - 00-00-0D */
#define  EDDI_MAC_IEEE_RESERVED_BEGIN_1_1 { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x01 }
/* PTP (Delay) and LLDP        00-00-0E            */
#define  EDDI_MAC_LLDP_PTP_DELAY_BEGIN    { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E }
/* IEEE Reserved addresses - 2 00-00-0F - 00-00-10 */
#define  EDDI_MAC_IEEE_RESERVED_BEGIN_2   { 0x01, 0x80, 0xC2, 0x00, 0x00, 0x0F }
/* HSR 1 */
#define  EDDI_MAC_HSR_1_BEGIN             { 0x01, 0x80, 0x63, 0x07, 0x00, 0x00 }      //when changing check all locations with MAC_HSR_1_LEN!
/* HSR 2 */
#define  EDDI_MAC_HSR_2_BEGIN             { 0x01, 0x80, 0x63, 0x07, 0x00, 0x01 }      //when changing check all locations with MAC_HSR_2_LEN!
/* HSR STBY 1 */
#define  EDDI_MAC_STBY_1_BEGIN            { 0x00, 0x80, 0x63, 0x07, 0x10, 0x00 }
/* HSR STBY 2 */
#define  EDDI_MAC_STBY_2_BEGIN            { 0x09, 0x00, 0x06, 0x20, 0xCB, 0xBC }
/* DCP addresses 01-0E-CF-00-00-00 - 01-0E-CF-00-00-01 */
#define  EDDI_MAC_DCP_BEGIN               { 0x01, 0x0E, 0xCF, 0x00, 0x00, 0x00 }
/* VRRP MAC base address for CARP */    
#define  EDDI_MAC_CARP_BEGIN              { 0x00, 0x00, 0x5E, 0x00, 0x01, 0x01 }
/* VRRP multicast MAC base address for CARP*/
#define  EDDI_MAC_CARP_MC_BEGIN           { 0x01, 0x00, 0x5E, 0x00, 0x00, 0x12 }
#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SWI_PTP_H


/*****************************************************************************/
/*  end of file eddi_swi_ptp.h                                               */
/*****************************************************************************/

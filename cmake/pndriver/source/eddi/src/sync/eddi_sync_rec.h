#ifndef EDDI_SYNC_REC_H         //reinclude-protection
#define EDDI_SYNC_REC_H

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
/*  F i l e               &F: eddi_sync_rec.h                           :F&  */
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
/*  25.10.07    JS    removed outdated properties defines.                   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                         Bit-Masks for TxPortGroupArray[0]                 */
/*===========================================================================*/
// Local Port
#define EDDI_SYNC_TXPORT_LOCAL      ((LSA_UINT8)0x01)
// Calculate all supported External TxPortBits depending on pDDB->PM.PortMap.PortCnt:
#define EDDI_SYNC_TXPORTS_EXTERNAL  \
( (LSA_UINT8)(  (((LSA_UINT32)0x02 << pDDB->PM.PortMap.PortCnt) - 1) & (~EDDI_SYNC_TXPORT_LOCAL)  ) )

/*===========================================================================*/
/*                         Bitfield for FrameDetails                         */
/*===========================================================================*/

// Bit 0 - 1: FrameDetails.SyncFrame
// This field shall be coded with the values according to Table 77.
// Table 268 - FrameDetails.SyncFrame
// Value (hexadecimal)  Meaning
#define SYNC_FRAME_DETAILS_NO_SYNC_FRAME           0x00 // no sync frame
#define SYNC_FRAME_DETAILS_PRIMARY_SYNC_FRAME      0x01 // primary sync frame
//Zur Zeit nicht unterstuetzt
#define SYNC_FRAME_DETAILS_SECONDARY_SYNC_FRAME    0x02 // secondary sync frame

// Bit 2 - 3: FrameDetails. MeaningFrameSendOffset
// This field shall be coded with the values according to Table 78.
// Table 269 - FrameDetails. MeaningFrameSendOffset
// Value (hexadecimal)  Meaning
#define SYNC_FRAME_DETAILS_RX_TX_TIME      0x00 // Field FrameSendOffset specifies the point 
// of time for receiving or transmittimg a frame
#define SYNC_FRAME_DETAILS_IRT_PHASE_BEGIN 0x01 // Field FrameSendOffset specifies the beginning 
// of the RT Class 3 interval within a phase
#define SYNC_FRAME_DETAILS_IRT_PHASE_END   0x02 // Field FrameSendOffset specifies the ending 
// of the RT Class 3 interval within a phase

/*===========================================================================*/
/*                         Bitfield for SyncProperties                       */
/*===========================================================================*/

// Function                               ClockRole           Sec.Available  SyncClass          Hex-Value
//                                        (Bit0..1)           (Bit2)        (Bit8-15)
// Klassischer RTC-SyncSlave              1 (Clock slave)     0 (no)         0 (not rele-vant)  0x0001
// Redundanter RTC-SyncSlave              1 (Clock slave)     1 (yes)        0 (not rele-vant)  0x0005
// Klassischer RTC-SyncMaster             2 (Clock master)    0 (no)         0 (not rele-vant)  0x0002
// Redundanter RSM light CLASS_PRIMARY    2 (Clock master)    1 (yes)        1 (Primary)        0x0106
// Redundanter RSM light CLASS_SECONDARY  2 (Clock master)    1 (yes)        2 (Second.)        0x0206

// Bit 0 - 1: Role
// This field shall be coded with the values according to Table 81.
// Value (hexadecimal)  Meaning
//#define SYNC_PROP_ROLE_LOCAL_SYNC             0x00  // local sync
#define SYNC_PROP_ROLE_EXTERNAL_SYNC            0x01  // external sync.: Role Clock Slave
#define SYNC_PROP_ROLE_CLOCK_MASTER             0x02  // Clock Master

// Bit 2 - 7 : reserved
// Bit 8 - 12: SyncID
// Bit 13- 15: reserved

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_REC_H


/*****************************************************************************/
/*  end of file eddi_sync_rec.h                                              */
/*****************************************************************************/

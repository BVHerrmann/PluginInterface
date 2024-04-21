#ifndef EDDI_SYNC_FRM_H         //reinclude-protection
#define EDDI_SYNC_FRM_H

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
/*  F i l e               &F: eddi_sync_frm.h                           :F&  */
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

#include "pnio_pck1_on.h"

#define EDDI_SYNC_FRAME_HEADER_SIZE      16UL  // [DestMac to FrameID] (No VLAN-Tags !!)
#define EDDI_SYNC_FRAME_PAYLOAD_SIZE     74UL //  [Cc_snd to PTCP_End]
#define EDDI_SYNC_FRAME_APDU_SIZE         4UL  // [APDU-Status]

// Nr of bytes that have to be provisioned in KRAM for a Syncframe.
#define EDDI_SYNC_FRAME_KRAMDATA_SIZE   ( EDDI_SYNC_FRAME_PAYLOAD_SIZE + EDDI_SYNC_FRAME_APDU_SIZE )

// Values PTCP_ClockRole
#define EDDI_SYNC_PTCP_CLOCK_ROLE_PRM 1
#define EDDI_SYNC_PTCP_CLOCK_ROLE_SEC 2

/*---------------------------------------------------------------------------*/
/* SYNC FRAME Structure                                                      */
/*---------------------------------------------------------------------------*/
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SYNC_PTCP_PAYLOAD_TYPE // 104 Byte
{/* Offset */

    //Block: PTCP_Header         20 Byte
    /* 00 */
    LSA_UINT32                    Cc_snd;
    /* 04 */
    LSA_UINT32                    Cc_rcv;
    /* 08 */
    LSA_UINT32                    Cc_DelayInTicks;
    LSA_UINT8                     Buffer[EDDI_SYNC_FRAME_PAYLOAD_SIZE];

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SYNC_PTCP_PAYLOAD_TYPE EDDI_SYNC_PTCP_PAYLOAD_TYPE;

PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_CRT_DATA_APDU_STATUS
{
    LSA_UINT32                          Block;          // used to make fast copies
    PNIO_PACKED_ATTRIBUTE_PRE struct _Detail
    {
        LSA_UINT16                      CycleCnt;
        EDDI_SER_DATA_STATUS_TYPE       DataStatus;
        LSA_UINT8                       TransferStatus; // TransferStatus
    } PNIO_PACKED_ATTRIBUTE_POST Detail;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef union _EDDI_CRT_DATA_APDU_STATUS EDDI_CRT_DATA_APDU_STATUS;

#if defined (EDDI_CFG_REV7)
/**
 *  If using IOC (SOC) the APDU is followed
 *  by a pointer to the application-data
 *  residing in the PAEA-RAM.
 */
PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_CRT_PAEA_APDU_STATUS
{
    EDDI_CRT_DATA_APDU_STATUS    APDU_Status;
    LSA_UINT32                   pPAEA_Data;

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_CRT_PAEA_APDU_STATUS EDDI_CRT_PAEA_APDU_STATUS;

#endif

/*---------------------------------------------------------------------------
/  SYNC EDDI_SYNC_FRAME_BUFFER_TYPE-Structure
/    (as the ERTEC stores it in KRAM !)
/  - All data elements of the IRT Syncframe
/    beginning from Cc_Snd
/    until the end of PTCP-Payload (incl.)
/  - ATTENTION: Contrary to the standard the APDU is before the payload
/    (IRTE speciality)  !
/----------------------------------------------------------------------------*/

PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SYNC_FRAME_BUFFER_TYPE    // 108 Byte
{
    EDDI_CRT_DATA_APDU_STATUS     Apdu;           //   4 Byte
    EDDI_SYNC_PTCP_PAYLOAD_TYPE   PTCP_Payload;   // 104 Byte

}  PNIO_PACKED_ATTRIBUTE_POST;

typedef struct _EDDI_SYNC_FRAME_BUFFER_TYPE EDDI_SYNC_FRAME_BUFFER_TYPE;

// #define   EDDI_APDU_OFFSET_CYCLE_CNT         0UL
// #define   EDDI_APDU_OFFSET_DATA_STATUS       2UL
#define   EDDI_APDU_OFFSET_REDUNDANCY_STATUS  3UL

#include "pnio_pck_off.h"

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_FRM_H


/*****************************************************************************/
/*  end of file eddi_sync_frm.h                                              */
/*****************************************************************************/

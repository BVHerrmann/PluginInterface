#ifndef EDDI_SYNC_RCV_H         //reinclude-protection
#define EDDI_SYNC_RCV_H

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
/*  F i l e               &F: eddi_sync_rcv.h                           :F&  */
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

// ATTENTION: This enum is related to FunctionTable CrtPhaseTxFctTable !!
typedef enum _EDDI_SYNC_RCV_STATE
{
    EDDI_SYNC_RCV_STS_OFF       = 0,  // Not Active (not paramtetrized)
    EDDI_SYNC_RCV_STS_ON        = 1   // next KRAM-Rcv will be copied to EDDI_SRV_SYNC_RCV-Buffer
    // if there are Ressources available.
    // On every NewCycleIRQ KramBuffer-APDU will checked if a new frame has arrived.
    // If so Kram Doublebuffer will be exchanged.

} EDDI_SYNC_RCV_STATE;

typedef enum _EDDI_SYNC_RCV_EVENT
{
    EDDI_SYNC_RCV_EVT_ACTIVATE      = 0,   //
    EDDI_SYNC_RCV_EVT_NEW_CYCLE     = 1,   //
    EDDI_SYNC_RCV_EVT_DEACTIVATE    = 2    //

} EDDI_SYNC_RCV_EVENT;

typedef struct _EDDI_SYNC_RCV_MACHINE
{
    EDDI_SYNC_RCV_STATE                State;
    LSA_UINT16                        HwPortIndex;

    /* LSA_UINT8                       * pHelpBuffer[2];    // [DirectionIdx]
                                                            // Pointer 4 Bytes before FrameBuffer, needed to
                                                            // avoid conflicts between APDU-Status and CheckPattern.*/

    EDDI_SYNC_FRAME_BUFFER_TYPE      * pFrameBuffer[2];      // [DoubleBuffer] Pointer to Kram-Buffer

    EDD_UPPER_MEM_U8_PTR_TYPE         pRedState[2];         // [DoubleBuffer] Shortcut to Redundancy-Status of APDU.
    // Indicates whether there has arrived a new SyncFrame.

    LSA_UINT32                        CurrentBufIdx;        // Index of current KRAM buffer active for receiving
    // Will be changed periodicaly between 0 and 1 by STM
    // (see pCopySrc)

    LSA_UINT32                        CopyLength;           // Length of Byte to be copied (SyncPayload only,
    // without Header, without APDU.

    EDDI_SER10_CCW_TYPE              * pRcvCw;               // Referenz to CtrlWord

    /*
    LSA_UINT32                        SyncRcvPort[2];       // Stores the SyncRcvPort for each Buffer to get correct

    LSA_UINT8                         HwRxPortMaskAllowed;  */

} EDDI_SYNC_RCV_MACHINE;

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvInitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvDeinitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvNewCycleCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_RCV_H


/*****************************************************************************/
/*  end of file eddi_sync_rcv.h                                              */
/*****************************************************************************/

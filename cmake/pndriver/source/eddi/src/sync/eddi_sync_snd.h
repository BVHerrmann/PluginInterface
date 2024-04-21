#ifndef EDDI_SYNC_SND_H         //reinclude-protection
#define EDDI_SYNC_SND_H

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
/*  F i l e               &F: eddi_sync_snd.h                           :F&  */
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
typedef enum _EDDI_SYNC_SND_STATE
{
    EDDI_SYNC_SND_STS_OFF       = 0,  //
    EDDI_SYNC_SND_STS_EMPTY     = 1,  //
    EDDI_SYNC_SND_STS_LOADED    = 2   //

} EDDI_SYNC_SND_STATE;

typedef enum _EDDI_SYNC_SND_EVENT
{
    EDDI_SYNC_SND_EVT_ACTIVATE      = 0,   //
    EDDI_SYNC_SND_EVT_NEW_CYCLE     = 1,   //
    EDDI_SYNC_SND_EVT_NEW_BUFFER    = 2,   //
    EDDI_SYNC_SND_EVT_DEACTIVATE    = 3    //

} EDDI_SYNC_SND_EVENT;

typedef struct _EDDI_SYNC_SND_MACHINE
{
    EDDI_SYNC_SND_STATE              State;
    LSA_UINT16                       HwPortIndex;
    EDDI_RQB_QUEUE_TYPE              RqbQueue;
    EDD_UPPER_RQB_PTR_TYPE           pCurrentRqb;
    LSA_UINT32                       AgingTimeout;

    EDDI_SER10_SINGLE_SHOT_SND_TYPE * pSingleShotCw; // SingleShotCw-Struct in KRAM
    EDDI_SYNC_FRAME_BUFFER_TYPE     * pFrameBuffer;  // Pointer to Kram-Buffer

    EDD_UPPER_MEM_U8_PTR_TYPE        pCopyDest;  //  CopyDestination in Kram (first byte after APDU)
    LSA_UINT32                       CopyLength; //  Length of Byte to be copied (SyncPayload only,
    //  without Header, without APDU.

} EDDI_SYNC_SND_MACHINE;

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndInitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndDeinitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndTrigger( EDDI_SYNC_SND_MACHINE    *  const  pMachine,
                                                  EDDI_SYNC_SND_EVENT         const  Event,
                                                  EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndNewCycleCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYNC_SND_H


/*****************************************************************************/
/*  end of file eddi_sync_snd.h                                              */
/*****************************************************************************/

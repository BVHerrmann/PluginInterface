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
/*  F i l e               &F: eddi_sync_rcv.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  14.07.07    JS    Added EDDI_SyncRcvRecv(), eddi_nrt_usr.h               */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
//#include "eddi_nrt_usr.h"
#include "eddi_sync_ir.h"
#include "eddi_pool.h"
#include "eddi_crt_check.h"
//#include "eddi_irt_ext.h"
//#include "eddi_swi_ext.h"
#include "eddi_ser_ext.h"
//#include "eddi_ser_cmd.h"
#include "eddi_sync_usr.h"
#include "eddi_lock.h"
//#include "eddi_profile.h"
#include "eddi_nrt_inc.h"

#define EDDI_MODULE_ID     M_ID_SYNC_RCV
#define LTRC_ACT_MODUL_ID  134

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvInitFrameBuffer( EDDI_LOCAL_DDB_PTR_TYPE       const pDDB, 
                                                                    EDDI_SYNC_FRAME_BUFFER_TYPE * const pFrameBuffer );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvInit( EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                         EDDI_SYNC_RCV_TYPE                  * const pRcv,
                                                         LSA_UINT32                            const HwPortIndex,
                                                         EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE   const pFrmHandler,
                                                         EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE   const pIrFrameDataElem );


static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvDeinit( EDDI_LOCAL_DDB_PTR_TYPE         const pDDB,
                                                           EDDI_SYNC_RCV_TYPE      const * const pRcv,
                                                           EDDI_SYNC_RCV_MACHINE         * const pMachine );


static LSA_VOID  EDDI_LOCAL_FCT_ATTR   EDDI_SyncRcv_EDDI_NRTRecv( EDD_UPPER_RQB_PTR_TYPE const pRQB );

// Functions for the SyncRcv - State-Machine
typedef LSA_VOID (EDDI_LOCAL_FCT_ATTR  *EDDI_SYNC_RCV_FCT)( EDDI_SYNC_RCV_TYPE      * const pRcv,
                                                            EDDI_SYNC_RCV_MACHINE   * const pMachine,
                                                            EDDI_SYNC_RCV_EVENT       const Event,
                                                            EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvRTDoneHook( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                            EDDI_SYNC_RCV_TYPE      * const pRcv,
                                                            LSA_UINT32                const HwPortIndex );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvStsOff( EDDI_SYNC_RCV_TYPE        * const pRcv,
                                                          EDDI_SYNC_RCV_MACHINE     * const pMachine,
                                                          EDDI_SYNC_RCV_EVENT         const Event,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvStsOn( EDDI_SYNC_RCV_TYPE        * const pRcv,
                                                          EDDI_SYNC_RCV_MACHINE     * const pMachine,
                                                          EDDI_SYNC_RCV_EVENT         const Event,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncRcvTrigger( EDDI_SYNC_RCV_TYPE      * const pRcv,
                                                  EDDI_SYNC_RCV_MACHINE   * const pMachine,
                                                  EDDI_SYNC_RCV_EVENT       const Event,
                                                  EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

// ATTENTION: This FunctionTable is related to enum EDDI_SYNC_RCV_STATE !!
static EDDI_SYNC_RCV_FCT SyncRcvFctTable[] =
{
    EDDI_SyncRcvStsOff,
    EDDI_SyncRcvStsOn
};


/***************************************************************************
* F u n c t i o n:       EDDI_SyncRcvInitAll()
*
* D e s c r i p t i o n:
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvInitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_RCV_TYPE  *  pRcv;
    LSA_UINT32             i, j, iDataElem;
    LSA_BOOL               Primary;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncRcvInitAll->");

    //loop over all (2) recv-handler (for primary and secondary)

    for (j=0; j<EDDI_SYNC_IDX_CNT; j++)
    {
        pRcv = &pDDB->SYNC.Rcv[j];

        // Reset all STMs
        for (i = 0; i < EDDI_MAX_IRTE_PORT_CNT; i++)
        {
            pRcv->BufferdRcvStm[i].State = EDDI_SYNC_RCV_STS_OFF;
        }

        if (j == 0)
        {
            Primary = LSA_TRUE;
        }
        else
        {
            Primary = LSA_FALSE;
        }

        //Find Consumer for RTSync
        pRcv->pFrmHandler = EDDI_SyncIrFindFrameHandlerForSyncConsumer(pDDB, Primary, pDDB->PRM.PDIRData.pRecordSet_A);

        if (pRcv->pFrmHandler != LSA_NULL)
        {
            if (   (EDDI_SYNC_IRT_CONSUMER     != pRcv->pFrmHandler->HandlerType)
                && (EDDI_SYNC_IRT_FWD_CONSUMER != pRcv->pFrmHandler->HandlerType))
            {
                EDDI_Excp("EDDI_SyncRcvInitAll, Wrong HandlerType:", EDDI_FATAL_ERR_EXCP, pRcv->pFrmHandler->HandlerType, 0);
                return;
            }

            pRcv->ExpectedFrameLenWithApdu = pRcv->pFrmHandler->pIrFrameDataElem[0]->DataLength  +
                                             EDDI_SYNC_FRAME_HEADER_SIZE                 +
                                             sizeof(EDDI_CRT_DATA_APDU_STATUS);

            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SyncRcvInitAll, Framehandler found, Primary:0x%X FrameHandler:0x%X",
                               Primary, ((LSA_UINT32)pRcv->pFrmHandler));

            //Init all STMs
            for (iDataElem = 0; iDataElem < pDDB->PM.PortMap.PortCnt; iDataElem++)
            {
                EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pIrFrameDataElem = pRcv->pFrmHandler->pIrFrameDataElem[iDataElem];
                LSA_UINT32                                  HwPortIndex;

                if (EDDI_NULL_PTR == pIrFrameDataElem)
                {
                    if (iDataElem == 0)
                    {
                        //At least one DataElement must exist.
                        EDDI_Excp("EDDI_SyncRcvInitAll, EDDI_NULL_PTR == pIrFrameDataElem", EDDI_FATAL_ERR_EXCP, 0, 0);
                        return;
                    }
                }
                else
                {
                    HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pIrFrameDataElem->UsrRxPort_0_4);

                    EDDI_SyncRcvInit(pDDB, pRcv, HwPortIndex, pRcv->pFrmHandler, pIrFrameDataElem);
                }
            }

            pRcv->pFrmHandler->UsedByUpper = LSA_TRUE;
            pDDB->SYNC.bSyncInRedActive = LSA_TRUE; //at least one port needs the SIR statemachines
            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                               "EDDI_SyncRcvInitAll, FrameHandler USED: FrameId:0x%X pFrmHandler:0x%X", 
                               pRcv->pFrmHandler->FrameId, (LSA_UINT32)pRcv->pFrmHandler);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncRcvInit()
 *
 *  Description: Prepares all FCWs for buffered SyncRcv
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvInit( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                         EDDI_SYNC_RCV_TYPE                   *  const  pRcv,
                                                         LSA_UINT32                              const  HwPortIndex,
                                                         EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE     const  pFrmHandler,
                                                         EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     const  pIrFrameDataElem )
{
    EDDI_DDB_COMP_SYNC_TYPE       *  const  pSyncComp = &pDDB->SYNC;
    EDDI_SYNC_RCV_MACHINE         *  const  pMachine  = &pRcv->BufferdRcvStm[HwPortIndex];
    LSA_UINT32                              BufferIdx;
    EDDI_CRT_CONSUMER_PARAM_TYPE            LowerParams;

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncRcvInit->State:0x%X HwPortIndex:0x%X", 
                       pMachine->State, HwPortIndex);

    EDDI_ENTER_SYNC_S();

    pMachine->State = EDDI_SYNC_RCV_STS_OFF;

    pMachine->HwPortIndex = (LSA_UINT16)HwPortIndex;

    // On ERTEC400 we need to store the Rx-Port of Sync-Telegram to handle
    // LineDelay-Deviations in Sync-Error-Calculation

    if (   (pIrFrameDataElem->UsrRxPort_0_4 == 0)
        || (pIrFrameDataElem->UsrRxPort_0_4 > pDDB->PM.PortMap.PortCnt))
    {
        EDDI_Excp("EDDI_SyncRcvInit, Invalid RxPort for SyncSlave", EDDI_FATAL_ERR_EXCP, pIrFrameDataElem->UsrRxPort_0_4, 0);
        return;
    }

    // Init DoubleBuffers
    for (BufferIdx = 0; BufferIdx < 2; BufferIdx++ )
    {
        //DELETE Build Data-Buffer;
        /* EDDI_MemGetPoolBuffer(pSyncComp->MemPool.hSyncFrameBuffer, (void * *)&pRcv->pHelpBuffer[i]);
        pRcv->pFrameBuffer[i] = (EDDI_SYNC_FRAME_BUFFER_TYPE *)(void *)(pRcv->pHelpBuffer[i] + 4UL);
                                                            // We need 4 Bytes more in front of our FrameBuffer
                                                            // to handle CHECK_PATTERN-Problem of Memory-Administration
                                                            // (CHECK_PATTERN would overwrite DataStatus.CtrlBit in ReceiveBuffer
                                                            //  after freeing receive buffer )
        */

        EDDI_MemGetPoolBuffer(pSyncComp->MemPool.hSyncFrameBuffer, (void * *)&pMachine->pFrameBuffer[BufferIdx]);

        EDDI_SyncRcvInitFrameBuffer(pDDB, pMachine->pFrameBuffer[BufferIdx]);

        pMachine->pRedState[BufferIdx] = ((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer[BufferIdx]) + EDDI_APDU_OFFSET_REDUNDANCY_STATUS;
    }

    EDDI_EXIT_SYNC_S();

    if (   (EDDI_SYNC_IRT_CONSUMER     != pFrmHandler->HandlerType)
        && (EDDI_SYNC_IRT_FWD_CONSUMER != pFrmHandler->HandlerType))
    {
        EDDI_Excp("IRTSyncReceiverAdd, Wrong HandlerType:", EDDI_FATAL_ERR_EXCP, pFrmHandler->HandlerType, 0);
        return;
    }

    if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) != EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
    {
        EDDI_Excp("IRTSyncReceiverAdd, Wrong FrameId:", EDDI_FATAL_ERR_EXCP, pFrmHandler->FrameId, 0);
        return;
    }

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Add lower FcwReceiver
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    EDDI_MEMSET(&LowerParams, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_CRT_CONSUMER_PARAM_TYPE));

    LowerParams.ListType              = EDDI_LIST_TYPE_SYNC;
    LowerParams.CyclePhase            = pIrFrameDataElem->Phase;
    LowerParams.CycleReductionRatio   = pIrFrameDataElem->ReductionRatio;
    LowerParams.DataHoldFactor        = 0;
    LowerParams.DataLen               = pIrFrameDataElem->DataLength;
    LowerParams.DataOffset            = 0;
    LowerParams.FrameId               = EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId);
    LowerParams.ImageMode             = (LSA_UINT8)EDDI_IMAGE_MODE_STAND_ALONE;// Special ImageMode (== 0x03) to avoid
    // influence of global DataValid State !)
    LowerParams.pKRAMDataBuffer       = (EDDI_DEV_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer[0];
    LowerParams.pSBTimerEntry         = 0;
    LowerParams.ui32SBTimerAdr        = SER10_NULL_PTR;
    LowerParams.WDReloadVal           = 0;
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LowerParams.DHReloadVal           = 0;
    #endif
    LowerParams.RT_Type               = EDDI_RT_TYPE_NORMAL;

    // --> FCW-Entry is already inserted.
    // --> Only modify DataBuffer-specific parameters (Local-Bit, TimerSB, etc.)
    // --> Local Receiving is still switched off!
    EDDI_SERIrtConsumerBufferAdd(pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW,
                                 &LowerParams,
                                 (EDDI_DEV_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer[0],
                                 EDDI_SER10_IRQ_QUALIFIER_NO,
                                 pDDB);

    //Switch on local Receiving
    EDDI_RedTreeIrtSetRxLocalActivity(pFrmHandler->pIRTRcvEl[HwPortIndex], LSA_TRUE);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  Set Meta Infos
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // Prepare copy information for later use to fasten up buffer-exchange-routine

    pMachine->pRcvCw        = (EDDI_SER10_CCW_TYPE *)(void *)pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW;

    pMachine->CopyLength    = pRcv->ExpectedFrameLenWithApdu - (sizeof(EDDI_CRT_DATA_APDU_STATUS) + EDDI_SYNC_FRAME_HEADER_SIZE);

    pMachine->CurrentBufIdx = 0;

    pMachine->State         = EDDI_SYNC_RCV_STS_ON; 
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvInitFrameBuffer()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvInitFrameBuffer( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB, 
                                                                    EDDI_SYNC_FRAME_BUFFER_TYPE  *  const  pFrameBuffer )
{
    EDDI_CRT_DATA_APDU_STATUS  Apdu;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvInitFrameBuffer->");

    EDDI_MEMSET(pFrameBuffer, (LSA_UINT8)0, EDDI_SYNC_FRAME_KRAMDATA_SIZE);

    Apdu.Block = 0;
    EDDI_SET_DS_BIT__DataValid(&Apdu.Detail.DataStatus, 0);
    EDDI_SET_DS_BIT__Bit_3(&Apdu.Detail.DataStatus, 1); //Set DataCtrl-Bit --> allow receiving

    Apdu.Detail.TransferStatus = EDDI_SER_RED_STATUS_INIT_VALUE;

    ((EDDI_CRT_DATA_APDU_STATUS *)(void *)pFrameBuffer)->Block = Apdu.Block;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SyncRcvDeinitAll()
*
* D e s c r i p t i o n:
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvDeinitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_RCV_TYPE  *  pRcv;
    LSA_UINT32             UsrPortIndex;
    LSA_UINT32             j;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncRcvDeinitAll->");

    /* loop over all (2) recv-handler (for primary and secondary).*/
    for (j=0; j<EDDI_SYNC_IDX_CNT; j++)
    {
        pRcv = &pDDB->pLocal_SYNC->Rcv[j];

        if (pRcv->pFrmHandler != LSA_NULL)
        {
            for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
            {
                LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                EDDI_SyncRcvDeinit(pDDB,pRcv,&pRcv->BufferdRcvStm[HwPortIndex]);
            }

            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,
                               "EDDI_SyncRcvDeinitAll, FrameHandler FREED: FrameId:0x%X pFrmHandler:0x%X", 
                               pRcv->pFrmHandler->FrameId, (LSA_UINT32)pRcv->pFrmHandler);

            pRcv->pFrmHandler->UsedByUpper = LSA_FALSE;
            pRcv->pFrmHandler              = LSA_NULL;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncRcvDeinit()
 *
 *  Description: Disables all FCWs for buffered SyncRcv
 *               - a pure RTC3-Forwarder doesn't exist at RTSync!
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvDeinit( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                           EDDI_SYNC_RCV_TYPE      const *  const  pRcv,
                                                           EDDI_SYNC_RCV_MACHINE         *  const  pMachine )
{
    EDDI_DDB_COMP_SYNC_TYPE  *  const  pSyncComp = &pDDB->SYNC;
    LSA_UINT32                         BufferIdx;

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncRcvDeinit->State:0x%X", 
                       pMachine->State);

    EDDI_ENTER_SYNC_S();

    if (pMachine->State == EDDI_SYNC_RCV_STS_OFF)
    {
        //nothing to do
    }
    else
    {
        if (pMachine->pRcvCw != LSA_NULL)
        {
            //Switch off local Receiving
            EDDI_RedTreeIrtSetRxLocalActivity(pRcv->pFrmHandler->pIRTRcvEl[pMachine->HwPortIndex], LSA_FALSE);

            EDDI_SERIrtConsumerBufferRemove(pMachine->pRcvCw, pDDB);
        }

        for (BufferIdx = 0; BufferIdx < 2; BufferIdx++)
        {
            if (pMachine->pFrameBuffer[BufferIdx])
            {
                EDDI_MemFreePoolBuffer(pSyncComp->MemPool.hSyncFrameBuffer, (void *)pMachine->pFrameBuffer[BufferIdx]);
            }
        }
        pMachine->State = EDDI_SYNC_RCV_STS_OFF;
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SyncRcvRecv()
*
* D e s c r i p t i o n: Receive a Sync-Frame via buffered or queued interface
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvRecv( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                 EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_NRT_RECV_PTR_TYPE  const  pParam = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;
    LSA_RESULT                          Status;

    EDDI_SYNC_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncRcvRecv->");

    Status = EDD_STS_OK;

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        /* invalid parameters */
        Status = EDD_STS_ERR_PARAM;
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
    else
    {
        EDDI_SyncRcv_EDDI_NRTRecv(pRQB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvRTDoneHook()                         */
/*                                                                         */
/* D e s c r i p t i o n: Called after a buffered Sync-frame was           */
/*                        received. Check Sync-RECV-Queue and indicate     */
/*                        receiption if one is queued.                     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvRTDoneHook( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                               EDDI_SYNC_RCV_TYPE       *  const  pRcv,
                                                               LSA_UINT32                  const  HwPortIndex )
{
    EDD_RQB_TYPE                        *  pRQB;
    EDD_UPPER_NRT_RECV_PTR_TYPE            pRQBRcv;
    EDDI_NRT_RX_USER_TYPE               *  pRxUser;
    EDDI_LOCAL_HDB_PTR_TYPE                pHDB;
    EDDI_SYNC_RCV_MACHINE        *  const  pRcvStm = &pRcv->BufferdRcvStm[HwPortIndex];
    EDDI_NRT_CHX_SS_IF_TYPE      *  const  pIF     = &pDDB->NRT.IF[EDDI_NRT_CHB_IF_0];
    LSA_UINT32                             RxTime;
    LSA_INT16                              RepeatCtr;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvRTDoneHook->");

    pIF->Rx.LockFct(pIF);

    /* get Pointer to Rx-Management within SYNC-management */
    pRxUser = &pDDB->SYNC.RxUser;

    pRQB = pRxUser->pFirstRQB;

    if //a Recv-RQB is present?
       (!LSA_HOST_PTR_ARE_EQUAL(pRQB, LSA_NULL))
    {
        pIF->Rx.Queued_RxCount--;

        #if defined (EDDI_DEACT_NRT_CHANNEL_AT_NO_RX_RESOURCE)
        if //last queued RQB within NRT channel?
           (pIF->Rx.Queued_RxCount == 0)
        {
            #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
            if (pDDB->pLocal_RTOUDP->EnableRcv_CHA0 == 0)
            #endif
            {
                //disable NRT channel
                IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_DISABLE;
            }
        }
        #endif

        //remove RQB from queue
        pRxUser->pFirstRQB = (EDD_RQB_TYPE *)EDD_RQB_GET_NEXT_RQB_PTR(pRxUser->pFirstRQB);

        pRQBRcv = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;

        //Fill RQB. tbd.

        pRQBRcv->IOCount          = (LSA_UINT16)pRcv->ExpectedFrameLenWithApdu;
        pRQBRcv->UserDataOffset   = EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN;
        pRQBRcv->UserDataLength   = (LSA_UINT16)(pRQBRcv->IOCount - pRQBRcv->UserDataOffset);

        RxTime                    = EDDI_NTOHL(pRcvStm->pFrameBuffer[pRcvStm->CurrentBufIdx]->PTCP_Payload.Cc_rcv); //cc_rcv is in BIG_ENDIAN!

        pRQBRcv->FrameFilter      = EDD_NRT_FRAME_PTCP_SYNC;
        pRQBRcv->FrameFilterInfo  = 0;

        pRQBRcv->PortID           = (LSA_UINT16)pDDB->PM.HWPortIndex_to_UsrPortID[HwPortIndex];

        EDDI_SyncFwdStmsGetSwFwd(pDDB, LSA_TRUE, LSA_FALSE /*bSyncID1*/, pRQBRcv->PortID, pRQBRcv->doSwFwd);

        EDDI_MEMSET(pRQBRcv->pBuffer, (LSA_UINT8)0,  EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN);

        //Set Ethertype
        //EDDI_PRM_xRT_TAG

        //Set FrameId
        *(pRQBRcv->pBuffer + EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN + 0) = 0x00;
        *(pRQBRcv->pBuffer + EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN + 1) = 0x80;

        RepeatCtr = 1;
        do
        {
            //Copy Payload
            EDDI_MEMCOPY(pRQBRcv->pBuffer + EDDI_FRAME_HEADER_SIZE_WITHOUT_VLAN + 2,
                         (LSA_UINT8 *)(void *)(pRcvStm->pFrameBuffer[pRcvStm->CurrentBufIdx]) + sizeof(EDDI_CRT_DATA_APDU_STATUS),
                         (LSA_UINT)pRcvStm->CopyLength);

            pRQBRcv->RxTime = EDDI_NTOHL(pRcvStm->pFrameBuffer[pRcvStm->CurrentBufIdx]->PTCP_Payload.Cc_rcv); // cc_rcv is in BIG_ENDIAN !!

            //if cc_rcv now is different from cc_rcv read before, then we "overtook" IRTE with copying
            if (RxTime == pRQBRcv->RxTime)
            {
                RepeatCtr = -1;   //Proceed directly
            }
            else if (RepeatCtr != 0)
            {
                //different timestamps for the 1st time => read again
                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_WARN, "EDDI_SyncRcvRTDoneHook, cc_rcv different 1st time. PreCopy cc_rcv:0x%X PostCopy cc_rcv:0x%X", 
                                   RxTime, pRQBRcv->RxTime);
                RxTime = pRQBRcv->RxTime;
                RepeatCtr--;  //1 more round
            }
            else
            {
                //different timestamps for the 2nd time => ERROR, proceed and risk loosing sync
                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SyncRcvRTDoneHook, cc_rcv different 2nd time. PreCopy cc_rcv:0x%X PostCopy cc_rcv:0x%X", 
                                   RxTime, pRQBRcv->RxTime);
                RepeatCtr = -1; //Proceed with errors
            }
        }
        while (RepeatCtr >= 0);

        EDD_RQB_SET_RESPONSE(pRQB, EDD_STS_OK);

        pRxUser->RxQueueReqCnt--;
        pRQBRcv->RequestCnt = pRxUser->RxQueueReqCnt;
        
        pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context;

        if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL))
        {
            EDDI_Excp("EDDI_SyncRcvRTDoneHook", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
            return; //not expected! prevents lint warning
        }

        EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

        if (!(0 == pHDB->Cbf))
        {
            EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);
            pHDB->RxCount--;
        }
        else
        {
            EDDI_Excp("EDDI_SyncRcvRTDoneHook", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
        }
    }

    pIF->Rx.UnLockFct(pIF);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvNRTDoneHook()                        */
/*                                                                         */
/* D e s c r i p t i o n: Function called from NRT-RecvDone just before    */
/*                        calling user-cbf. RQB already filled with NRT    */
/*                        stuff.                                           */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvNRTDoneHook( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                        LSA_BOOL                 const  bSyncID1)
{
    EDD_UPPER_NRT_RECV_PTR_TYPE  const  pParam = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvNRTDoneHook->");

    /* FrameFilterInfo contains the FrameID of the received frame!           */
    /* Depending on the FrameID we fill up the DoSwFwd field within RQB      */
    /* With Sync/Fu for SyncID0  the EDD may forward by hardware.            */
    /* With Announce for SyncID0 the EDD the user shall never forward        */
    /* With all other SyncIds the EDD does currently not forward at all      */

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncRcvNRTDoneHook, FrameFilterInfo:0x%X", pParam->FrameFilterInfo);

    if (   (pParam->FrameFilterInfo == EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_START)
        || (pParam->FrameFilterInfo == EDDI_SRT_FRAMEID_PTCP_SYNC_W_FU_START)
        || (pParam->FrameFilterInfo == EDDI_SRT_FRAMEID_PTCP_SYNC_FU_START))
    {
        /* For Sync/FU for SyncID0 get forwarding info */
        EDDI_SyncFwdStmsGetSwFwd(pDDB, LSA_FALSE, bSyncID1, pParam->PortID, pParam->doSwFwd);
    }
    else
    {
        LSA_UINT32  DestUsrPortId;

        for (DestUsrPortId = 0; DestUsrPortId < pDDB->PM.PortMap.PortCnt; DestUsrPortId++)
        {
            if (pParam->FrameFilterInfo == EDD_SRT_FRAMEID_PTCP_ANNO_START)
            {
                /* Announce frame for SyncID 0 will always be forwarded by EDD !   */
                /* May be extended to ALL Announce frame in the future!            */
                pParam->doSwFwd[DestUsrPortId] = EDD_NRT_RECV_DO_NOT_FWD_BY_UPPER_LAYER;

                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvNRTDoneHook, Announce for SyncID0. doSwFwd[0x%X] = 0x%X", 
                                   DestUsrPortId, LSA_FALSE);
            }
            else
            {
                /* all other frames will not be handled by hardware and shall be   */
                /* forwarded by GSY.                                               */
                pParam->doSwFwd[DestUsrPortId] = EDD_NRT_RECV_DO_FWD_BY_UPPER_LAYER;

                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvNRTDoneHook, SyncID <> 0. doSwFwd[0x%X] = 0x%X", 
                                   DestUsrPortId, LSA_TRUE);
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcv_EDDI_NRTRecv()                      */
/*                                                                         */
/* D e s c r i p t i o n: Handle EDD_SRV_SYNC_RECV requests.               */
/*                        Queue the request within SYNC-Management and     */
/*                        put to NRT-Filter-management.                    */
/*                        Locked under IF locking                          */
/*                                                                         */
/*                        Important: The SYNC-channel must also have a     */
/*                                   NRT-Channel and use Interface B0!     */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcv_EDDI_NRTRecv( EDD_UPPER_RQB_PTR_TYPE  const  pRQB )
{
    EDDI_LOCAL_HDB_PTR_TYPE             pHDB;
    EDD_UPPER_NRT_RECV_PTR_TYPE  const  pRQBRcv     = (EDD_UPPER_NRT_RECV_PTR_TYPE)pRQB->pParam;
    EDD_UPPER_MEM_U8_PTR_TYPE    const  pDataBuffer = pRQBRcv->pBuffer;
    EDDI_LOCAL_DDB_PTR_TYPE             pDDB;
    EDDI_NRT_CHX_SS_IF_TYPE          *  pIF;
    EDDI_NRT_RX_USER_TYPE            *  pRxUser;

    pHDB = EDDI_NRTLockIFRx(pRQB);

    if (pHDB == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SyncRcv_EDDI_NRTRecv, No valid handle found!", EDDI_FATAL_ERR_NULL_PTR, 0, 0);
        return;
    }

    EDDI_SYNC_TRACE_00(pHDB->pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncRcv_EDDI_NRTRecv->");

    pIF  = pHDB->pIF;
    pDDB = pHDB->pDDB;

    //initialize IOCount (RQ 1839107, Task 1839257)
    pRQBRcv->IOCount = 0;

    //get Pointer to Rx-Management within SYNC-management
    pRxUser = &pDDB->SYNC.RxUser;

    if (   ((LSA_UINT32)(void *)pDataBuffer < pIF->LimitDown_BufferAddr)
        || ((LSA_UINT32)(void *)pDataBuffer > pIF->LimitUp_BufferAddr))
    {
        EDDI_Excp("EDDI_SyncRcv_EDDI_NRTRecv, Data Buffer Address wrong", EDDI_FATAL_ERR_EXCP, pDataBuffer, 0);
        pIF->Rx.UnLockFct(pIF);
        return;
    }

    if (((LSA_UINT32)(void *)pDataBuffer) & EDDI_RX_FRAME_BUFFER_ALIGN)
    {
        EDDI_Excp("EDDI_SyncRcv_EDDI_NRTRecv, Data Buffer Alignment wrong (see EDDI_RX_FRAME_BUFFER_ALIGN)", EDDI_FATAL_ERR_EXCP, pDataBuffer, 0);
        pIF->Rx.UnLockFct(pIF);
        return;
    }

    if (pIF->Rx.Dscr.Cnt)
    {
        //ATTENTION: do not change!
        #if defined (EDDI_ENABLE_NRT_CACHE_SYNC)
        //invalidate nrt-cache
        EDDI_SYNC_CACHE_NRT_RECEIVE(pDataBuffer, EDDI_FRAME_BUFFER_LENGTH);
        #endif

        //put RQB to end of list
        EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

        if (pRxUser->pFirstRQB == EDDI_NULL_PTR)
        {
            pRxUser->pFirstRQB = pRQB;
        }
        else
        {
            EDD_RQB_SET_NEXT_RQB_PTR(pRxUser->pLastRQB, pRQB);
        }

        pRxUser->pLastRQB = pRQB;

        pHDB->RxCount++;
        pRxUser->RxQueueReqCnt++;

        if (pIF->Rx.Queued_RxCount == 0)
        {
            //enable nrt-channel for receiving
            IO_x32(pIF->Value_for_IO_NRT_ENABLE) = (LSA_UINT32)(NRT_CHANNEL_ENABLE_BIT | NRT_RX_ENABLE);
            //enable NRT-SS-QUEUE (for sending and receiving)
            IO_x32(SS_QUEUE_DISABLE) = pIF->Value_for_NRT_Queue_ENABLE;
        }

        pIF->Rx.Queued_RxCount++;
        pIF->Rx.UnLockFct(pIF);
        return; //Request is queued
    }

    EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_RxInUse_SEE_EDD_RQB_CMP_NRT_INI_TYPE);
    pIF->Rx.UnLockFct(pIF);
    EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_CHANNEL_USE);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvTrigger()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvTrigger( EDDI_SYNC_RCV_TYPE       *  const  pRcv,
                                                    EDDI_SYNC_RCV_MACHINE    *  const  pMachine,
                                                    EDDI_SYNC_RCV_EVENT         const  Event,
                                                    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_SYNC_RCV_STATE  const  OldState = pMachine->State;

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvTrigger->State:0x%X Event:0x%X", OldState, Event);

    SyncRcvFctTable[OldState](pRcv, pMachine, Event, pDDB);

    if (OldState != pMachine->State)
    {
        EDDI_SYNC_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncRcvTrigger, HwPortIndex:0x%X OldState:0x%X Event:0x%X NewState:0x%X", 
                           pMachine->HwPortIndex, OldState, Event, pMachine->State);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvStsOff()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvStsOff( EDDI_SYNC_RCV_TYPE       *  const  pRcv,
                                                           EDDI_SYNC_RCV_MACHINE    *  const  pMachine,
                                                           EDDI_SYNC_RCV_EVENT         const  Event,
                                                           EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    LSA_UNUSED_ARG(pRcv); //satisfy lint!
    LSA_UNUSED_ARG(pDDB); //satisfy lint!

    switch (Event)
    {
        case EDDI_SYNC_RCV_EVT_ACTIVATE:
        {
            //TODO: Prepare Buffer
            //TODO: Activate STM: pMachine->State = EDDI_SYNC_RCV_STATE_ON;
        }
        break;

        case EDDI_SYNC_RCV_EVT_NEW_CYCLE:
        {
            //nothing to do
        }
        break;

        case EDDI_SYNC_RCV_EVT_DEACTIVATE:
        {
            //nothing to do
        }
        break;

        default:
            EDDI_Excp("EDDI_SyncRcvStsOff, ERROR unknown Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvStsOn()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvStsOn( EDDI_SYNC_RCV_TYPE       *  const  pRcv,
                                                          EDDI_SYNC_RCV_MACHINE    *  const  pMachine,
                                                          EDDI_SYNC_RCV_EVENT         const  Event,
                                                          EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    switch (Event)
    {
        case EDDI_SYNC_RCV_EVT_ACTIVATE:
        {
        }
        break;

        case EDDI_SYNC_RCV_EVT_NEW_CYCLE:
        {
            if (*(pMachine->pRedState[pMachine->CurrentBufIdx]) == EDDI_SER_RED_STATUS_INIT_VALUE)
            {
                //no new frame received --> wait for next cycle
            }
            else
            {
                LSA_UINT32  const  NewBufferIdx = (pMachine->CurrentBufIdx + 1UL) % 2UL;

                EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncRcvStsOn, New buffered RTSync frame received (0x%X,%d)",
                                   (LSA_UINT32)pMachine, pMachine->HwPortIndex);

                //prepare new Buffer
                *(pMachine->pRedState[NewBufferIdx]) = EDDI_SER_RED_STATUS_INIT_VALUE;

                //Change BufferPtr
                EDDI_SERXchangeBuffer(pDDB,
                                      pMachine->pRcvCw,
                                      (EDDI_LOCAL_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer[NewBufferIdx]);

                //Wait 10us
                EDDI_WAIT_10_NS(pDDB->hSysDev, 1000UL);

                //Do NrtRcv-Cbf (copies KramBuffer to UsrBuffer and calls upper cbf)
                EDDI_SyncRcvRTDoneHook(pDDB, pRcv, pMachine->HwPortIndex);

                //Idx - swapping must be done AFTER EDDI_SyncRcvRTDoneHook !!
                pMachine->CurrentBufIdx = NewBufferIdx;
            }
        }
        break;

        case EDDI_SYNC_RCV_EVT_DEACTIVATE:
        {
            //TODO: Deactivate STM: pMachine->State = EDDI_SYNC_RCV_STATE_OFF;
        }
        break;

        default:
        {
            EDDI_Excp("EDDI_SyncRcvStsOn, ERROR unknown Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncRcvNewCycleCbf()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncRcvNewCycleCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  UsrPortId;

    for (UsrPortId = 1; UsrPortId <= pDDB->PM.PortMap.PortCnt; UsrPortId++)
    {
        LSA_UINT32             const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_SYNC_RCV_TYPE         *  pRcv;
        EDDI_SYNC_RCV_MACHINE      *  pRcvStm;
        LSA_UINT32                    Ctr;

        for (Ctr=0; Ctr<EDDI_SYNC_IDX_CNT; Ctr++) //for primary and secondary..
        {
            pRcv    = &pDDB->pLocal_SYNC->Rcv[Ctr];
            pRcvStm = &pRcv->BufferdRcvStm[HwPortIndex];

            if (pRcvStm->State == EDDI_SYNC_RCV_STS_ON)
            {
                EDDI_SyncRcvTrigger(pRcv, pRcvStm, EDDI_SYNC_RCV_EVT_NEW_CYCLE, pDDB);
                //maybe optimized to...
                //EDDI_SyncRcvStsOn(pRcv, pRcvStm, EDDI_SYNC_RCV_EVT_NEW_CYCLE, pDDB);
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_rcv.c                                              */
/*****************************************************************************/


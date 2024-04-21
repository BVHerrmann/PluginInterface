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
/*  F i l e               &F: eddi_sync_snd.c                           :F&  */
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
/*  23.10.07    JS    added LineDelayInTicksHardware with buffered send      */
/*  07.12.07    JS    FrameID defines moved to eddi_sync_usr.h               */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*
 *                                 includes                                  *
 *===========================================================================*/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_nrt_usr.h"
#include "eddi_sync_usr.h"
#include "eddi_lock.h"
#include "eddi_swi_ext.h"
#include "eddi_sync_ir.h"
#include "eddi_pool.h"
#include "eddi_ser_ext.h"

#define EDDI_MODULE_ID     M_ID_SYNC_SND
#define LTRC_ACT_MODUL_ID  136

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndInit( EDDI_LOCAL_DDB_PTR_TYPE              const pDDB,
                                                      EDDI_SYNC_SND_MACHINE              * const pMachine,
                                                      LSA_UINT32                           const HwPortIndex,
                                                      EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const pFrmHandler );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SyncSndDeinit( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB,
                                                        EDDI_SYNC_SND_MACHINE   * const pMachine );

// Typedef of FunctionPtr
typedef LSA_VOID (EDDI_LOCAL_FCT_ATTR  *EDDI_SYNC_SND_FCT)( EDDI_SYNC_SND_MACHINE   * const pMachine,
                                                            EDDI_SYNC_SND_EVENT       const Event,
                                                            EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

// Functions for the SyncSnd - State-Machine

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SyncSndStsOff( EDDI_SYNC_SND_MACHINE   * const pMachine,
                                                          EDDI_SYNC_SND_EVENT       const Event,
                                                          EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SyncSndStsEmpty( EDDI_SYNC_SND_MACHINE   * const pMachine,
                                                            EDDI_SYNC_SND_EVENT       const Event,
                                                            EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_SyncSndStsLoaded( EDDI_SYNC_SND_MACHINE   * const pMachine,
                                                             EDDI_SYNC_SND_EVENT       const Event,
                                                             EDDI_LOCAL_DDB_PTR_TYPE   const pDDB );

//ATTENTION: This FunctionTable is related to enum EDDI_SYNC_SND_STATE !!
static EDDI_SYNC_SND_FCT SyncSndFctTable[] =
{
    EDDI_SyncSndStsOff,
    EDDI_SyncSndStsEmpty,
    EDDI_SyncSndStsLoaded
};

static LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndSendBuffered( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                 LSA_UINT16              const PortID );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndInitFrameBuffer( EDDI_LOCAL_DDB_PTR_TYPE       const pDDB, 
                                                                    EDDI_SYNC_FRAME_BUFFER_TYPE * const pFrameBuffer );

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndGetLineDelayInTicksHardware( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                                                LSA_UINT32              const PortID );

#if defined (EDDI_CFG_REV5)
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSnd_SyncFrameSubCCDELField( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                                            EDDI_SYNC_PTCP_PAYLOAD_TYPE     * pFrame,
                                                                            LSA_UINT32                  const DelayInTicks );
#endif


#if defined (EDDI_CFG_REV5)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncSnd_SyncFrameCCDELCorrection()          */
/*                                                                         */
/* D e s c r i p t i o n: Modifies cc_del field within send-buffer         */
/*                        with actual linedelay setting of port if we      */
/*                        have a sync without FollowUp to send.            */
/*                                                                         */
/*                        Function is only called at service               */
/*                        EDD_SRV_SYNC_SEND.                               */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSnd_SyncFrameCCDELCorrection( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      EDD_UPPER_RQB_PTR_TYPE   const  pRQB )
{
    LSA_UINT32                          LineDelayInTicksHardware;
    EDD_UPPER_NRT_SEND_PTR_TYPE  const  pRQBSnd = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;

    if (   (pRQBSnd->FrameID >= EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_START)
        && (pRQBSnd->FrameID <= EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_END))
    {
        /* get linedelay of port */
        LineDelayInTicksHardware = EDDI_SyncSndGetLineDelayInTicksHardware(pDDB, pRQBSnd->PortID);

        /* sub from cc_del field */
        EDDI_SyncSnd_SyncFrameSubCCDELField(pDDB, (EDDI_SYNC_PTCP_PAYLOAD_TYPE *)(void*)(pRQBSnd->pBuffer + EDDI_SYNC_FRAME_HEADER_SIZE),
                                            LineDelayInTicksHardware);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


#if defined (EDDI_CFG_REV5)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncSnd_SyncFrameSubCCDELField()            */
/*                                                                         */
/* D e s c r i p t i o n: Get linedelay set within Hardware for Port       */
/*                        with PortID in ticks                             */
/*                                                                         */
/* A r g u m e n t s:     pFrame: Pointer to Frame buffer starting         */
/*                                behind FrameID                           */
/*                                (EDDI_SYNC_PTCP_PAYLOAD_TYPE *)          */
/*                                                                         */
/*                        DelayInTicks:                                    */
/*                                Delay to be substracted from current     */
/*                                ccdelay within Framebuffer               */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSnd_SyncFrameSubCCDELField( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                            EDDI_SYNC_PTCP_PAYLOAD_TYPE      *  pFrame,
                                                                            LSA_UINT32                   const  DelayInTicks )
{
    if (DelayInTicks)
    {
        LSA_UINT32  Cc_DelayInTicks;

        Cc_DelayInTicks         = EDDI_NTOHL(pFrame->Cc_DelayInTicks);
        Cc_DelayInTicks        -= DelayInTicks;
        pFrame->Cc_DelayInTicks = EDDI_HTONL(Cc_DelayInTicks);

        EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSnd_SyncFrameSubCCDELField, :0x%X :0x%X",
                           DelayInTicks, Cc_DelayInTicks);
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncSndGetLineDelayInTicksHardware()        */
/*                                                                         */
/* D e s c r i p t i o n: Get linedelay set within Hardware for Port       */
/*                        with PortID in ticks                             */
/*                                                                         */
/* A r g u m e n t s:     PortID: = 1..x                                   */
/*                                                                         */
/* Return Value:          LineDelayInTicksHardware                         */
/*                                                                         */
/***************************************************************************/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndGetLineDelayInTicksHardware( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                  LSA_UINT32               const  PortID )
{
    LSA_UINT32  HwPortIndex = 0;

    /* get the actual linedelay of Port and return it within Send-RQB */
    /* With ERTEC400 this is the linedelay added on send.             */

    if ((PortID > EDD_PORT_ID_AUTO) && (PortID <= pDDB->PM.PortMap.PortCnt))
    {
        HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[PortID];
    }
    else
    {
        EDDI_Excp("EDDI_SyncSndGetLineDelayInTicksHardware, ERROR unexpected PortID:", EDDI_FATAL_ERR_EXCP, PortID, 0);
        return 0;
    }

    return (pDDB->pLocal_SWITCH->LinkIndExtPara[HwPortIndex].LineDelayInTicksHardware);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:       EDDI_SyncSndInitAll()
 *
 *  Description:
 *                  Prepares all FCWs for buffered SyncSnd
 *  Arguments:
 *
 *
 *  Return:         LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndInitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_SND_TYPE  *  const  pSnd = &pDDB->SYNC.Snd;
    LSA_UINT32                    UsrPortIndex, i;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSndInitAll->");

    // Reset all STMs
    for (i = 0; i < EDDI_MAX_IRTE_PORT_CNT; i++)
    {
        pSnd->BufferdSndStm[i].State = EDDI_SYNC_SND_STS_OFF;
    }

    // FindProvider for RTSync
    // Note: We should only have one Provider! (primary or secondary)
    pSnd->pFrmHandler = EDDI_SyncIrFindFrameHandlerForSyncProvider(pDDB, pDDB->PRM.PDIRData.pRecordSet_A);

    if (pSnd->pFrmHandler != LSA_NULL)
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        pSnd->ExpectedFrameLenWithApdu =   pSnd->pFrmHandler->pIrFrameDataElem[0]->DataLength  
                                         + EDDI_SYNC_FRAME_HEADER_SIZE               
                                         + sizeof(EDDI_CRT_DATA_APDU_STATUS);

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            EDDI_SyncSndInit(pDDB, &pSnd->BufferdSndStm[HwPortIndex], HwPortIndex, pSnd->pFrmHandler);
        }

        pDDB->SYNC.bSyncInRedActive = LSA_TRUE; //at least one port needs the SIR statemachines
        pSnd->pFrmHandler->UsedByUpper = LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:       EDDI_SyncSndDeinitAll()
 *
 *  Description:
 *                  Deactivates all SyncSend-Machines
 *  Arguments:
 *
 *  Return:         void
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndDeinitAll( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SYNC_SND_TYPE  *  const  pSnd = &pDDB->SYNC.Snd;
    LSA_UINT32                    UsrPortIndex;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSndDeinitAll->");

    if (pSnd->pFrmHandler != LSA_NULL)
    {
        LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

            EDDI_SyncSndDeinit(pDDB, &pSnd->BufferdSndStm[HwPortIndex]);
        }

        pSnd->pFrmHandler->UsedByUpper = LSA_FALSE;
        pSnd->pFrmHandler              = LSA_NULL;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:       EDDI_SyncSndInit()
 *
 *  Description:
 *                  Deactivates a single portspecific SyncSend-Machine
 *  Arguments:
 *
 *
 *  Return:         LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndInit( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                         EDDI_SYNC_SND_MACHINE             *  const  pMachine,
                                                         LSA_UINT32                           const  HwPortIndex,
                                                         EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler )
{
    EDDI_DDB_COMP_SYNC_TYPE  *  const  pSyncComp    = &pDDB->SYNC;
    EDD_MAC_ADR_TYPE                   SyncDestAddr = EDDI_SwiPNGetPTPSYNCDA(pDDB);

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSndInit->State:0x%X HwPortIndex:0x%X", pMachine->State, HwPortIndex);

    EDDI_ENTER_SYNC_S();

    EDDI_InitQueue(pDDB, &pMachine->RqbQueue);

    EDDI_MemGetPoolBuffer(pSyncComp->MemPool.hSyncFrameBuffer, (void * *)&(pMachine->pFrameBuffer));
    EDDI_SyncSndInitFrameBuffer(pDDB, pMachine->pFrameBuffer);

    pMachine->State       = EDDI_SYNC_SND_STS_OFF;
    pMachine->HwPortIndex = (LSA_UINT16)HwPortIndex;

    if (pFrmHandler->pIRTSndEl[HwPortIndex] == LSA_NULL)
    {
        //planned FCW available
        pMachine->pSingleShotCw = LSA_NULL;
    }
    else
    {
        pMachine->pSingleShotCw = (EDDI_SER10_SINGLE_SHOT_SND_TYPE *)(void *)pFrmHandler->pIRTSndEl[HwPortIndex]->pCW;

        EDDI_SERSingleShotSndIniTake2(pMachine->pSingleShotCw,
                                      (EDDI_DEV_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer,
                                      &SyncDestAddr,
                                      pDDB);

        // Prepare copy information for later use to fasten up buffer-exchange-routine
        pMachine->pCopyDest  = ((EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pMachine->pFrameBuffer) + sizeof(EDDI_CRT_DATA_APDU_STATUS);
        pMachine->CopyLength = pDDB->SYNC.Snd.ExpectedFrameLenWithApdu - (sizeof(EDDI_CRT_DATA_APDU_STATUS) + EDDI_SYNC_FRAME_HEADER_SIZE);
    }

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SyncSndInitFrameBuffer()                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndInitFrameBuffer( EDDI_LOCAL_DDB_PTR_TYPE         const  pDDB, 
                                                                    EDDI_SYNC_FRAME_BUFFER_TYPE  *  const  pFrameBuffer )
{
    EDDI_CRT_DATA_APDU_STATUS  Apdu;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncSndInitFrameBuffer->");

    EDDI_MEMSET(pFrameBuffer, (LSA_UINT8)0, EDDI_SYNC_FRAME_KRAMDATA_SIZE);

    Apdu.Block = 0;
    EDDI_SET_DS_BIT__DataValid(&Apdu.Detail.DataStatus, 1);

    ((EDDI_CRT_DATA_APDU_STATUS *)(void *)pFrameBuffer)->Block = Apdu.Block;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncSndDeinit()
 *
 *  Description: Disables all FCWs for buffered SyncSnd
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 *
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndDeinit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                           EDDI_SYNC_SND_MACHINE    *  const  pMachine )
{
    EDDI_DDB_COMP_SYNC_TYPE  *  const  pSyncComp = &pDDB->SYNC;
    EDD_UPPER_RQB_PTR_TYPE             pRQB;

    EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSndDeinit->State:0x%X", pMachine->State);

    EDDI_ENTER_SYNC_S();

    if (pMachine->pSingleShotCw != LSA_NULL)
    {
        if (EDDI_SYNC_SND_STS_OFF != pMachine->State)
        {
            LSA_UINT32  pNextAsic;

            EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SyncSndDeinit. Manual execute LMW. 0x%X 0x%X", 0, 0);
        
            //possibly the LMW did not have a chance to be executed (either UNSHIFT or list got switched off)
            //==> manually do job of LMW
            //NOOP.CWLink = LMW.CWLink
            pNextAsic = EDDI_GetBitField32(pMachine->pSingleShotCw->Lmw.Hw0.Value.U32_1, EDDI_SER10_LMW_BIT__pNext);
            EDDI_SetBitField32(&pMachine->pSingleShotCw->Noop.Value.U32_1, EDDI_SER10_NOOP_BIT2__pNext, pNextAsic);
        }
    }

    if (pMachine->pFrameBuffer)
    {
        EDDI_MemFreePoolBuffer(pSyncComp->MemPool.hSyncFrameBuffer, (void *)pMachine->pFrameBuffer);
    }

    if (pMachine->pCurrentRqb != LSA_NULL)
    {
        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pMachine->pCurrentRqb->internal_context,
                           pMachine->pCurrentRqb,
                           EDD_STS_OK_CANCEL);
        pMachine->pCurrentRqb = LSA_NULL;
    }

    //Flush Send-Queue
    for (;;)
    {
        pRQB = EDDI_RemoveFromQueue(pDDB, &pMachine->RqbQueue);

        if (!pRQB)
        {
            break;
        }

        EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK_CANCEL);
    }

    pMachine->State = EDDI_SYNC_SND_STS_OFF;

    EDDI_EXIT_SYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncSndTrigger()
 *
 *  Description:
 *
 *  Arguments:
 *
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndTrigger( EDDI_SYNC_SND_MACHINE    *  const  pMachine,
                                                    EDDI_SYNC_SND_EVENT         const  Event,
                                                    EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    EDDI_SYNC_SND_STATE  OldState;

    EDDI_SYNC_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_SyncSndTrigger->State:0x%X Event:0x%X", pMachine->State, Event);

    OldState = pMachine->State;

    SyncSndFctTable[pMachine->State](pMachine, Event, pDDB);

    if (OldState != pMachine->State)
    {
        EDDI_SYNC_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncSndTrigger, HwPortIndex:0x%X OldState:0x%X Event:0x%X NewState:0x%X", 
                           pMachine->HwPortIndex, OldState, Event, pMachine->State);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncSndStsOff()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndStsOff( EDDI_SYNC_SND_MACHINE    *  const  pMachine,
                                                           EDDI_SYNC_SND_EVENT         const  Event,
                                                           EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    switch (Event)
    {
        case EDDI_SYNC_SND_EVT_ACTIVATE:
        {
            //insert Single-Shot-Sender
            if (pMachine->pSingleShotCw == LSA_NULL)
            {
                EDDI_Excp("EDDI_SyncSndStsOff, case EDDI_SYNC_SND_EVT_ACTIVATE, pMachine->pSingleShotCw == LSA_NULL", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }

            pMachine->State = EDDI_SYNC_SND_STS_EMPTY;
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_CYCLE:
        {
            //nothing todo
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_BUFFER:
        {
            if (pMachine->pSingleShotCw == LSA_NULL)
            {
                EDDI_Excp("EDDI_SyncSndStsOff, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pMachine->pSingleShotCw == LSA_NULL", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }

            pMachine->pCurrentRqb = EDDI_RemoveFromQueue(pDDB, &pMachine->RqbQueue);

            if (pMachine->pCurrentRqb == 0)
            {
                EDDI_Excp("EDDI_SyncSndStsOff, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pRQB == 0", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }
            else
            {
                EDD_UPPER_MEM_U8_PTR_TYPE           pSrc;
                #if defined (EDDI_CFG_REV5)
                LSA_UINT32                          LineDelayInTicksHardware;
                #endif
                EDD_UPPER_NRT_SEND_PTR_TYPE  const  pParam = (EDD_UPPER_NRT_SEND_PTR_TYPE)pMachine->pCurrentRqb->pParam;

                if (   (LSA_HOST_PTR_ARE_EQUAL(pParam->pBuffer, LSA_NULL))
                    || (pParam->Length < ( pDDB->SYNC.Snd.ExpectedFrameLenWithApdu - sizeof(EDDI_CRT_DATA_APDU_STATUS))))
                {
                    EDDI_Excp("EDDI_SyncSndStsOff, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pParam->pBuffer: pParam->Length:", EDDI_FATAL_ERR_ISR, pParam->pBuffer, pParam->Length);
                    return;
                }

                //Copy Buffer (without HEADER without APDU)
                pSrc = pParam->pBuffer + EDDI_SYNC_FRAME_HEADER_SIZE;
                EDDI_MemCopy(pMachine->pCopyDest, pSrc, pMachine->CopyLength);

                #if defined (EDDI_CFG_REV5)
                /* get current linedelay set within Hardware. This linedelay will be  */
                /* added by ERTEC400 on send (which is wrong), so we substract it     */
                /* within cc_del field to compensate this..                           */

                //get linedelay of port
                LineDelayInTicksHardware = EDDI_SyncSndGetLineDelayInTicksHardware(pDDB,pParam->PortID);

                //sub from cc_del field
                EDDI_SyncSnd_SyncFrameSubCCDELField(pDDB, (EDDI_SYNC_PTCP_PAYLOAD_TYPE *)(void*)pMachine->pCopyDest, LineDelayInTicksHardware);
                #endif

                //Load SingleShot - Element
                EDDI_SERSingleShotSndLoad(pMachine->pSingleShotCw, pDDB);

                pMachine->AgingTimeout = 2;

                pMachine->State        = EDDI_SYNC_SND_STS_LOADED;
            }
        }
        break;

        case EDDI_SYNC_SND_EVT_DEACTIVATE:
        {
            //nothing to do
        }
        break;

        default:
            EDDI_Excp("EDDI_SyncSndStsOff, ERROR unknown Event", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncSndStsEmpty()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndStsEmpty( EDDI_SYNC_SND_MACHINE    *  const  pMachine,
                                                             EDDI_SYNC_SND_EVENT         const  Event,
                                                             EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    switch (Event)
    {
        case EDDI_SYNC_SND_EVT_ACTIVATE:
        {
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_CYCLE:
        {
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_BUFFER:
        {
            if (pMachine->pSingleShotCw == LSA_NULL)
            {
                EDDI_Excp("EDDI_SyncSndStsEmpty, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pMachine->pSingleShotCw == LSA_NULL", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }

            pMachine->pCurrentRqb = EDDI_RemoveFromQueue(pDDB, &pMachine->RqbQueue);

            if (pMachine->pCurrentRqb == 0)
            {
                EDDI_Excp("EDDI_SyncSndStsEmpty, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pRQB == 0", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }
            else
            {
                EDD_UPPER_MEM_U8_PTR_TYPE           pSrc;
                #if defined (EDDI_CFG_REV5)
                LSA_UINT32                          LineDelayInTicksHardware;
                #endif
                EDD_UPPER_NRT_SEND_PTR_TYPE  const  pParam = (EDD_UPPER_NRT_SEND_PTR_TYPE)pMachine->pCurrentRqb->pParam;

                if (   (LSA_HOST_PTR_ARE_EQUAL (pParam->pBuffer, LSA_NULL))
                    || (pParam->Length < ( pDDB->SYNC.Snd.ExpectedFrameLenWithApdu - sizeof(EDDI_CRT_DATA_APDU_STATUS))))
                {
                    EDDI_Excp("EDDI_SyncSndStsEmpty, case EDDI_SYNC_SND_EVT_NEW_BUFFER, pParam->pBuffer: pParam->Length:", EDDI_FATAL_ERR_ISR, pParam->pBuffer, pParam->Length);
                    return;
                }

                // Copy Buffer (without HEADER without APDU
                pSrc = pParam->pBuffer + EDDI_SYNC_FRAME_HEADER_SIZE;
                EDDI_MemCopy(pMachine->pCopyDest, pSrc, pMachine->CopyLength);

                #if defined (EDDI_CFG_REV5)
                /* get current linedelay set within Hardware. This linedelay will be  */
                /* added by ERTEC400 on send (which is wrong), so we substract it     */
                /* within cc_del field to compensate this..                           */

                //get linedelay of port
                LineDelayInTicksHardware = EDDI_SyncSndGetLineDelayInTicksHardware(pDDB,pParam->PortID);

                //sub from cc_del field
                EDDI_SyncSnd_SyncFrameSubCCDELField(pDDB, (EDDI_SYNC_PTCP_PAYLOAD_TYPE *)(void*)pMachine->pCopyDest, LineDelayInTicksHardware);
                #endif

                //Load SingleShot - Element
                EDDI_SERSingleShotSndLoad(pMachine->pSingleShotCw, pDDB);

                pMachine->AgingTimeout = 2UL;

                pMachine->State        = EDDI_SYNC_SND_STS_LOADED;
            }
        }
        break;

        case EDDI_SYNC_SND_EVT_DEACTIVATE:
        {
            //nothing to do
        }
        break;

        default:
            EDDI_Excp("EDDI_SyncSndStsEmpty, ERROR unknown Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_SyncSndStsLoaded()
 *
 *  Description:
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndStsLoaded( EDDI_SYNC_SND_MACHINE    *  const  pMachine,
                                                              EDDI_SYNC_SND_EVENT         const  Event,
                                                              EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB )
{
    switch (Event)
    {
        case EDDI_SYNC_SND_EVT_ACTIVATE:
        {
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_CYCLE:
        {
            if (pMachine->pCurrentRqb == 0)
            {
                EDDI_Excp("EDDI_SyncSndStsLoaded, case EDDI_SYNC_SND_EVT_NEW_CYCLE, pRQB == 0", EDDI_FATAL_ERR_ISR, 0, 0);
                return;
            }

            pMachine->AgingTimeout --;

            if (pMachine->AgingTimeout != 0)
            {
                //No status change
                //Keep waiting for next cycle
            }
            else
            {
                EDDI_LOCAL_HDB_PTR_TYPE      pHDB;
                EDD_UPPER_NRT_SEND_PTR_TYPE  pParam;
                #if defined (EDDI_CFG_REV5)
                LSA_UINT32                   LineDelayInTicksHardware;
                #endif

                pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(pMachine->pCurrentRqb->internal_context);

                /* get the actual linedelay of Port and return it within Send-RQB */
                /* With ERTEC400 this is the linedelay added on send.             */

                pParam = (EDD_UPPER_NRT_SEND_PTR_TYPE)pMachine->pCurrentRqb->pParam;
                pParam->LineDelayInTicksHardware = EDDI_SyncSndGetLineDelayInTicksHardware(pDDB,pParam->PortID);

                #if defined (EDDI_CFG_REV5)
                LineDelayInTicksHardware = pParam->LineDelayInTicksHardware;
                #endif

                EDDI_RequestFinish(pHDB, pMachine->pCurrentRqb, EDD_STS_OK);

                pMachine->pCurrentRqb = EDDI_RemoveFromQueue(pDDB, &pMachine->RqbQueue);

                if (pMachine->pCurrentRqb == 0)
                {
                    pMachine->State = EDDI_SYNC_SND_STS_EMPTY;
                }
                else
                {
                    EDD_UPPER_MEM_U8_PTR_TYPE  pSrc;

                    pParam = (EDD_UPPER_NRT_SEND_PTR_TYPE)pMachine->pCurrentRqb->pParam;

                    if (   (LSA_HOST_PTR_ARE_EQUAL(pParam->pBuffer, LSA_NULL))
                        || (pParam->Length != (pDDB->SYNC.Snd.ExpectedFrameLenWithApdu - sizeof(EDDI_CRT_DATA_APDU_STATUS))))
                    {
                        EDDI_Excp("EDDI_SyncSndStsLoaded, case EDDI_SYNC_SND_EVT_NEW_CYCLE, pParam->pBuffer: pParam->Length:", EDDI_FATAL_ERR_ISR, pParam->pBuffer, pParam->Length);
                        return;
                    }

                    //Copy Buffer (without HEADER without APDU
                    pSrc = pParam->pBuffer + EDDI_SYNC_FRAME_HEADER_SIZE;
                    EDDI_MemCopy(pMachine->pCopyDest, pSrc, pMachine->CopyLength);

                    #if defined (EDDI_CFG_REV5)
                    //sub linedelay of hardware from cc_del field
                    EDDI_SyncSnd_SyncFrameSubCCDELField(pDDB, (EDDI_SYNC_PTCP_PAYLOAD_TYPE *)(void*)pMachine->pCopyDest, LineDelayInTicksHardware);
                    #endif

                    //Load SingleShot - Element
                    EDDI_SERSingleShotSndLoad(pMachine->pSingleShotCw, pDDB);

                    pMachine->AgingTimeout = 2;

                    pMachine->State = EDDI_SYNC_SND_STS_LOADED;
                }
            }
        }
        break;

        case EDDI_SYNC_SND_EVT_NEW_BUFFER:
        {
            //nothing to do
        }
        break;

        case EDDI_SYNC_SND_EVT_DEACTIVATE:
        {
            //nothing to do
        }
        break;

        default:
            EDDI_Excp("EDDI_SyncSndStsLoaded, ERROR unknown Event: State:", EDDI_FATAL_ERR_ISR, Event, pMachine->State);
            return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SyncSndSend()
*
* D e s c r i p t i o n: Depending on the delivered FrameId this function
*                        calls either the NRT_Send()-Function
*                        or the SendBuffered()-Function
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndSend( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                 EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                 EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB )
{
    EDD_UPPER_NRT_SEND_PTR_TYPE  pParam;
    LSA_RESULT                   Status;

    EDDI_SYNC_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SyncSndSend->");

    Status = EDD_STS_OK;

    pParam = (EDD_UPPER_NRT_SEND_PTR_TYPE)pRQB->pParam;

    //tbd: NullPtr-Check kann entfallen, da dies bereits in EDDI_NRTReq and EDDI_RESTReq geprüft wird.

    if (LSA_HOST_PTR_ARE_EQUAL(pParam, LSA_NULL))
    {
        //invalid parameters
        Status = EDD_STS_ERR_PARAM;
        EDDI_RequestFinish(pHDB, pRQB, Status);
    }
    else
    {
        LSA_UINT32         HwPortIndex;
        LSA_RESULT  const  Result = EDDI_GenChangeUsrPortID2HwPortIndex(pParam->PortID, pDDB, &HwPortIndex);

        pParam->LineDelayInTicksHardware = 0;  /* default: none */

        if (EDD_STS_OK != Result)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INVALID_PortID);
            EDDI_RequestFinish(pHDB, pRQB, EDD_STS_ERR_PARAM);
        }

        /* Note: We use TxTimePresent to signal NRT that we want a timestamp */
        /*       TxTimePresent is checked within NRT.                        */

        else if (   (pParam->FrameID >= EDDI_SRT_FRAMEID_PTCP_SYNC_W_FU_START)
                 && (pParam->FrameID <= EDDI_SRT_FRAMEID_PTCP_SYNC_W_FU_END))
        {
            /* PTCP-RTSyncPDU with follow up  (0x0020-0x0021) (needs a Timestamp) */

            pParam->TxTimePresent = EDD_SYNC_SEND_TX_TIME_PRESENT; /* this also signals NRT to do a timestamp         */
            EDDI_NRTSendTimestamp(pRQB);      /* On Error this function calls EDDIReqestFinish() */
        }
        else if (   (pParam->FrameID >= EDDI_SRT_FRAMEID_PTCP_SYNC_FU_START)
                 && (pParam->FrameID <= EDDI_SRT_FRAMEID_PTCP_SYNC_FU_END))
        {
            /* PTCP-FollowUpPDU               (0xFF20-0xFF21) */
            pParam->TxTimePresent = EDD_SYNC_SEND_TX_TIME_NOT_PRESENT; /* this also signals NRT to not timestamp          */
            EDDI_NRTSendUsr(pRQB);             /* On Error this function calls EDDIReqestFinish() */
        }
        else if (   (pParam->FrameID >= EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_START)
                 && (pParam->FrameID <= EDDI_SRT_FRAMEID_PTCP_SYNC_WO_FU_END))
        {
            /* PTCP-RTSyncPDU (0x0080-0x0081) */
            if (   (EDDI_SyncIrHasIrtPortSyncSender    (pDDB, pDDB->PM.UsrPortID_to_HWPort_0_3[pParam->PortID]))
                && (EDDI_SyncPortStmsIsIrtSyncSndActive(pDDB, pDDB->PM.UsrPortID_to_HWPort_0_3[pParam->PortID])))
            {
                pParam->TxTimePresent = EDD_SYNC_SEND_TX_TIME_NOT_PRESENT;
                Status = EDDI_SyncSndSendBuffered(pRQB, pDDB, pParam->PortID);
                //EDDI_RequestFinish(pHDB, pRQB, Status); //tbd Request Finish must be moved from here to NewCycle-Context.
            }
            else
            {
                pParam->TxTimePresent = EDD_SYNC_SEND_TX_TIME_NOT_PRESENT; /* this also signals NRT to not timestamp          */
                EDDI_NRTSendUsr(pRQB);             /* On Error this function calls EDDIReqestFinish() */
            }
        }
        else
        {
            EDDI_SYNC_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,"EDDI_SyncSndSend, Invalid FrameID:0x%X", pParam->FrameID);
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_FrameID);
            Status = EDD_STS_ERR_PARAM;
            EDDI_RequestFinish(pHDB, pRQB, Status);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SyncSndNewCycleCbf()
*
* D e s c r i p t i o n: Triggered by NewCycleIRQ
*                        calls all Statemachines waiting for newcycle event
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndNewCycleCbf( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32         UsrPortId;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    for (UsrPortId = 1; UsrPortId <= PortMapCnt; UsrPortId++)
    {
        LSA_UINT32                const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[UsrPortId];
        EDDI_SYNC_SND_MACHINE  *  const  pSndStm     = &pDDB->pLocal_SYNC->Snd.BufferdSndStm[HwPortIndex];

        if (pSndStm->State == EDDI_SYNC_SND_STS_LOADED)
        {
            EDDI_SyncSndTrigger(pSndStm, EDDI_SYNC_SND_EVT_NEW_CYCLE, pDDB);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************
* F u n c t i o n:       EDDI_SyncSndSendBuffered()
*
* D e s c r i p t i o n:
*
* A r g u m e n t s:
*
* Return Value:          LSA_RESULT
*
***************************************************************************/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SyncSndSendBuffered( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                   EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT16               const  PortID )
{
    //TODO: ENTER / EXIT
    LSA_UINT32                const  HwPortIndex = pDDB->PM.UsrPortID_to_HWPort_0_3[PortID];
    EDDI_SYNC_SND_MACHINE  *  const  pSndStm     = &pDDB->pLocal_SYNC->Snd.BufferdSndStm[HwPortIndex];

    EDDI_AddToQueueEnd(pDDB, &pSndStm->RqbQueue, pRQB);

    EDDI_SyncSndTrigger(pSndStm, EDDI_SYNC_SND_EVT_NEW_BUFFER, pDDB);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_sync_snd.c                                              */
/*****************************************************************************/

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
/*  F i l e               &F: eddi_irt_usr.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  IRT (isochron real time) for EDDI.               */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.02.04    ZR    workaround to test old irt with new crt interface      */
/*                    (for addProvider)                                      */
/*  01.03.04    ZR    new DataPuffer handling; use the provided offset       */
/*                    new interface for SERIniProviderFcw                    */
/*  05.03.04    ZR    provide parameter for CRT Provider table               */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ser_ext.h"
#include "eddi_irt_ext.h"
#include "eddi_sync_ir.h"
#include "eddi_crt_ext.h"

#define EDDI_MODULE_ID     M_ID_IRT_USR
#define LTRC_ACT_MODUL_ID  120

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_IRTInitFcwLists()                      +*/
/*+  Input/Output          :    EDD_UPPER_RQB_PTR_TYPE     pRQB             +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE    Service: EDDI_SRV_COMP_SRT_INI                         +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_CMP_IRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE    Service:    EDDI_SRV_COMP_IRT_INI                      +*/
/*+   LSA_RESULT    Status: EDD_STS_OK                                      +*/
/*+                         EDD_STS_ERR_SEQUENCE                            +*/
/*+                         EDDI_STS_ERR_RESOURCES                          +*/
/*+                         EDD_STS_ERR_PARAM                               +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTInitFcwLists( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                               UsrPortIndex;
    EDDI_LOCAL_DDB_COMP_IRT_PTR_TYPE  const  pIRTComp = &pDDB->IRT;
    //EDDI_MEM_BUF_EL_H                   *  pMemHeader;
    LSA_UINT32                               HwPortIndex;
    LSA_UINT32                        const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTInitFcwLists->");

    pIRTComp->pKramIrtBase = &pDDB->pKramFixMem->IrtBaseList;
    pDDB->KramRes.IrtBase  = sizeof(EDDI_SER_IRT_BASE_LIST_TYPE);

    //see following for-loop EDDI_MemSet(pIRTComp->pKramIrtBase, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER_IRT_BASE_LIST_TYPE));
    //already 0 EDDI_MemSet(&pIRTComp->CtrlHeads,   (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_IRT_MGMT_TYPE) );

    //init KRAM IrtBaseList
    //We also have to initialize FCW-Header of unused HwPorts.
    for (HwPortIndex = 0; HwPortIndex < pDDB->PM.HwTypeMaxPortCnt; HwPortIndex++)
    {
        pIRTComp->pKramIrtBase->Head[HwPortIndex].Rcv = SER10_NULL_PTR_SWAP;
        pIRTComp->pKramIrtBase->Head[HwPortIndex].Snd = SER10_NULL_PTR_SWAP;
    }

    //init SW IrtHeader-List
    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        pIRTComp->CtrlHeads.CtrlHeadSnd[HwPortIndex].pKramListHead = &(pIRTComp->pKramIrtBase->Head[HwPortIndex].Snd);
        pIRTComp->CtrlHeads.CtrlHeadRcv[HwPortIndex].pKramListHead = &(pIRTComp->pKramIrtBase->Head[HwPortIndex].Rcv);
        pIRTComp->CtrlHeads.CtrlHeadRcv[HwPortIndex].pTop          = EDDI_NULL_PTR;

        #if defined (EDDI_RED_PHASE_SHIFT_ON)
        pDDB->pLocal_CRT->PhaseTx[HwPortIndex].pFcwListHead = pIRTComp->CtrlHeads.CtrlHeadSnd[HwPortIndex].pKramListHead;
        pDDB->pLocal_CRT->PhaseRx[HwPortIndex].pFcwListHead = pIRTComp->CtrlHeads.CtrlHeadRcv[HwPortIndex].pKramListHead;
        #else
        {
            EDDI_SER_CCW_PTR_TYPE  pRootCw;

            pRootCw = pDDB->CRT.IRTRedAx.IrtRedTreeTx[HwPortIndex].pRootCw;
            EDDI_SERConnectCwToHeader(pDDB, (EDDI_LOCAL_LIST_HEAD_PTR_TYPE)pIRTComp->CtrlHeads.CtrlHeadSnd[HwPortIndex].pKramListHead, pRootCw);

            pRootCw = pDDB->CRT.IRTRedAx.IrtRedTreeRx[HwPortIndex].pRootCw;
            EDDI_SERConnectCwToHeader(pDDB, (EDDI_LOCAL_LIST_HEAD_PTR_TYPE)pIRTComp->CtrlHeads.CtrlHeadRcv[HwPortIndex].pKramListHead, pRootCw);
        }
        #endif
    }

    //init IRT-Ctrl-Register (Portspecific IRT-Activity)
    IO_W32(IRT_CTRL_BASE_ADR, DEV_kram_adr_to_asic_register(pIRTComp->pKramIrtBase, pDDB));
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*======================  IRT-Provider-Consumer-Functions  ==================*/
/*===========================================================================*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IRTProviderAdd()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTProviderAdd( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE const  pFrmHandler,
                                                    EDD_MAC_ADR_TYPE                    const  DstMacAdr,
                                                    EDDI_DEV_MEM_U8_PTR_TYPE            const  pFrameBuffer,
                                                    LSA_UINT8                           const  ImageMode,
                                                    LSA_UINT32                       *  const  ppFCW,
                                                    LSA_BOOL                            const  bXCW_DBInitialized)
{
    LSA_UINT32         iDataElem;
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTProviderAdd->");

    //Build FCW-Entries for all IrFrameDataElements and all TxPorts
    for (iDataElem = 0; iDataElem < PortMapCnt; iDataElem++)
    {
        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pFrmHandler->pIrFrameDataElem[iDataElem];

        if (EDDI_NULL_PTR == pFrmData)
        {
            continue;
        }

        for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
        {
            // UsrPortIndex ist Absicht -> Vergleich auf User Sicht in pFrmData
            if (EDDI_IRTIsTxPortSet(pFrmData, PortMapCnt, UsrPortIndex))
            {
                LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                //return pointer to FCW (needed by io-functions)
                *ppFCW = (LSA_UINT32)(pFrmHandler->pIRTSndEl[HwPortIndex]->pCW);

                //ini FCW-Struct
                EDDI_SERIrtProviderBufferAdd(pFrmHandler->pIRTSndEl[HwPortIndex]->pCW,
                                             ImageMode,
                                             pFrameBuffer,
                                             &DstMacAdr,
                                             pDDB,
                                             bXCW_DBInitialized);
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IRTProviderRemove()
 *
 * function:      Remove a provider for IRT-communication
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTProviderRemove( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                       EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE const  pFrmHandler )
{
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTProviderRemove->");

    if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
    {
        EDDI_Excp("EDDI_IRTProviderRemove, pFrmHandler->FrameId == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3", EDDI_FATAL_ERR_EXCP, pFrmHandler->FrameId, 0);
        return;
    }

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        if (pFrmHandler->pIRTSndEl[HwPortIndex])
        {
            EDDI_RedTreeIrtSetTxActivity(pFrmHandler->pIRTSndEl[HwPortIndex], LSA_FALSE);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IRTConsumerAdd()
 *
 * function:      create structs für IRT-Communication
 *                - Function is not used for a pure RTC3-Forwarder!
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTConsumerAdd( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler,
                                                    EDDI_CRT_CONSUMER_PARAM_PTR_TYPE     const  pLowerParams,
                                                    EDD_UPPER_MEM_U8_PTR_TYPE            const  pKramDataBufferRed )
{
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pFrmData;
    LSA_UINT32                                  iDataElem;
    EDD_UPPER_MEM_U8_PTR_TYPE                   pKRAMDataBuffer;
    EDDI_SER10_IRQ_QUALIFIER_ENUM               IrqQualifier;
    LSA_UINT32                                  HwPortIndex;
    LSA_UINT32                           const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTConsumerAdd->");

    //A Redundant Consumer has more than one FCW !
    for (iDataElem = 0; iDataElem < PortMapCnt; iDataElem++)
    {
        pFrmData = pFrmHandler->pIrFrameDataElem[iDataElem];

        if (EDDI_NULL_PTR == pFrmData)
        {
            if (iDataElem == 0)
            {
                //At least one DataElement must exist.
                EDDI_Excp("EDDI_IRTConsumerAdd, EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
            }
            return;
        }

        if (   (EDDI_SYNC_IRT_CONSUMER     != pFrmHandler->HandlerType)
            && (EDDI_SYNC_IRT_FWD_CONSUMER != pFrmHandler->HandlerType))
        {
            EDDI_Excp("EDDI_IRTConsumerAdd, Wrong HandlerType", EDDI_FATAL_ERR_EXCP, pFrmHandler->HandlerType, 0);
            return;
        }

        //Prepare BufferLocation
        if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
        {
            //Special SyncSlave - DoubleBuffer
            switch (iDataElem)
            {
                case 0:
                    pKRAMDataBuffer = pLowerParams->pKRAMDataBuffer;
                    IrqQualifier    = EDDI_SER10_IRQ_QUALIFIER_NO;
                    break;

                case 1:
                    pKRAMDataBuffer = pKramDataBufferRed;
                    IrqQualifier    = EDDI_SER10_IRQ_QUALIFIER_NO;
                    break;

                default:
                    EDDI_Excp("EDDI_IRTConsumerAdd, iDataElem >= 2 (max 2 RX-FCWs allowed)", EDDI_FATAL_ERR_EXCP, 0, 0);
                    return;
            }
        }
        else
        {
            pKRAMDataBuffer = pLowerParams->pKRAMDataBuffer;
            IrqQualifier    = EDDI_SER10_IRQ_QUALIFIER_NO;
        }

        HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4);

        // --> FCW-Entry is already inserted.
        // --> Only modify DataBuffer-specific parameters (Local-Bit, TimerSB, etc.)
        // --> Local Receiving is still switched off!
        EDDI_SERIrtConsumerBufferAdd(pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW,
                                     pLowerParams,
                                     pKRAMDataBuffer,
                                     IrqQualifier,
                                     pDDB);

        //SB has to be enabled before enabling local receive of first consumer(IRT only). IRTE-RQ 1594728
        if (EDDI_CRTRpsIsStopped(&pDDB->CRT.Rps))
        {
            EDDI_CRTRpsTrigger(&pDDB->CRT.Rps, pDDB, EDDI_CRT_RPS_EVENT_START);
        }

        //Switch on local Receiving
        EDDI_RedTreeIrtSetRxLocalActivity(pFrmHandler->pIRTRcvEl[HwPortIndex], LSA_TRUE);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IRTConsumerRemove()
 *
 * function:      Remove a consumer for IRT-Communication
 *                - Function is not used for a pure RTC3-Forwarder!
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTConsumerRemove( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                       EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE  const  pFrmHandler )
{
    LSA_UINT32                                  iDataElem;
    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE         pFrmData;
    LSA_UINT32                                  HwPortIndex;
    LSA_UINT32                           const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTConsumerRemove->");

    //A Redundant Consumer has more than one FCW!
    for (iDataElem = 0; iDataElem < PortMapCnt; iDataElem++)
    {
        pFrmData = pFrmHandler->pIrFrameDataElem[iDataElem];

        if (EDDI_NULL_PTR == pFrmData)
        {
            if (iDataElem == 0)
            {
                // At least one DataElement must exist.
                EDDI_Excp("EDDI_IRTConsumerRemove, EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
            }
            return;
        }

        HwPortIndex = EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4);

        //Switch off local Receiving
        EDDI_RedTreeIrtSetRxLocalActivity(pFrmHandler->pIRTRcvEl[HwPortIndex], LSA_FALSE);

        if (!pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW)
        {
            EDDI_Excp("EDDI_IRTConsumerRemove, !pFCW", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_SERIrtConsumerBufferRemove(pFrmHandler->pIRTRcvEl[HwPortIndex]->pCW, pDDB);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IRTFcwAdd()
 *
 * function:      add - active   IRT-Forwarder
 *                    - inactive IRT-Consumer
 *                    - inactive IRT-Provider
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTFcwAdd( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                               EDDI_IRT_FRM_HANDLER_PTR_TYPE const  pFrmHandler )
{
    LSA_UINT32                           iDataElem;
    LSA_UINT32                           HwTxPortMask;
    LSA_UINT32                           HwRxPortIndex;
    EDDI_SER10_CCW_TYPE                  CCWLocal;
    LSA_UINT32                           UsrTxPortMask, mask, UsrPortIndex;
    EDDI_TREE_PTR_TYPE                   pTree;
    EDDI_TREE_ELEM_PTR_TYPE              pTreeElem;
    SER_IRT_INI_FCW_TYPE                 IniFCW;
    LSA_UINT32                    const  PortMapCnt = pDDB->PM.PortMap.PortCnt;
    EDDI_PRM_RECORD_IRT_PTR_TYPE  const  pRecordSet = pDDB->PRM.PDIRData.pRecordSet_A;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTFcwAdd->");

    for (iDataElem = 0; iDataElem < PortMapCnt; iDataElem++)
    {
        EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pFrmData = pFrmHandler->pIrFrameDataElem[iDataElem]; //this is a frame-record-structure with valid host-swapping! -> IniFCW.
        //Optimization possible: PRM builds "IniFCW" immediately. => EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE no longer necessary!
        LSA_UINT32                                  FcwTime;

        if (EDDI_NULL_PTR == pFrmData)
        {
            if (iDataElem == 0)
            {
                //At least one DataElement must exist.
                EDDI_Excp("EDDI_IRTFcwAdd, EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
            }
            return;
        }

        EDDI_MemSet(&IniFCW, (LSA_UINT8)0, (LSA_UINT32)sizeof(SER_IRT_INI_FCW_TYPE)); //do not change!

        //Common Ini-Params
        IniFCW.FrameID = EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId);
        IniFCW.DataLen = pFrmData->DataLength;

        FcwTime = pFrmData->FrameSendOffset;

        if (FcwTime > EDDI_PREFRAME_SEND_DELAY_100MBIT_NS)
        {
            IniFCW.Time = EDDI_IRT_CONVERT_FRAME_SEND_OFFSET(FcwTime) / 10;
        }
        else
        {
            //IniFCW.Time = 0; //already reset!
        }

        switch (pFrmHandler->HandlerType)
        {
            case EDDI_SYNC_IRT_FORWARDER:
            case EDDI_SYNC_IRT_FWD_CONSUMER:
            {
                //Get HwRxPort 0..3 of current pFrmData
                HwRxPortIndex = (LSA_UINT8)EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4);

                //Get TxPortMask 1|2|4|8
                UsrTxPortMask = pFrmData->UsrTxPortGroupArray[0] >> 1;
                HwTxPortMask  = 0;
                mask          = 1;

                if  /* not reception from local port */
                    (pFrmData->UsrRxPort_0_4)
                {
                    if  /* time for local reception > time already stored at this port */
                        (pFrmData->FrameSendOffset > pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].EndLocalNsRx)
                    {
                        pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].EndLocalNsRx        = pFrmData->FrameSendOffset;
                        pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].LastLocalFrameLenRx = pFrmData->DataLength;
                    }
                }
                
                for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
                {
                    if (UsrTxPortMask & mask)
                    {
                        HwTxPortMask |= EDDI_PmGetMask0108(pDDB, UsrPortIndex);
                    }
                    mask = mask << 1;
                }

                EDDI_MemSet(&CCWLocal, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_FCW_RCV_TYPE)); //do not change! EDDI_SER10_FCW_RCV_TYPE is necessary for EDDI_SYNC_IRT_FWD_CONSUMER!

                //ini FCW-Struct
                //Forwarders start with DstPorts disabled, except the SYNC-Forwarder
                EDDI_SERIniForwarderFcw(&IniFCW, (EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3 == pFrmHandler->FrameId)?HwTxPortMask:0, &CCWLocal, pDDB);

                //Add Fcw and Store reference to TreeElement
                pTree     = &pDDB->CRT.IRTRedAx.IrtRedTreeRx[HwRxPortIndex];
                pTreeElem = EDDI_RedTreeIrtAddCw(pDDB, pTree, pFrmHandler, pFrmData, IniFCW.Time, &CCWLocal);
                if (pTreeElem)
                {
                    pTreeElem->HwTxPortMask = HwTxPortMask;  //store only, forwarding is activated later on transition to RTC3PSM_RUN
                }
                pFrmHandler->pIRTRcvEl[HwRxPortIndex] = pTreeElem;
            }
            break;

            case EDDI_SYNC_IRT_CONSUMER:
            {
                //Here we have a pure Consumer (without Forwarding)!

                //Get HwRxPort 0..3 of current pFrmData
                HwRxPortIndex = (LSA_UINT8)EDDI_PmUsrPortIDToHwPort03(pDDB, pFrmData->UsrRxPort_0_4);

                if  /* not reception from local port */
                    (pFrmData->UsrRxPort_0_4)
                {
                    if  /* time for local reception > time already stored at this port */
                        (pFrmData->FrameSendOffset > pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].EndLocalNsRx)
                    {
                        pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].EndLocalNsRx        = pFrmData->FrameSendOffset;
                        pRecordSet->BeginEndData[pFrmData->UsrRxPort_0_4 - 1].LastLocalFrameLenRx = pFrmData->DataLength;
                    }
                }
                
                EDDI_MemSet(&CCWLocal, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_FCW_RCV_TYPE)); //do not change!

                //ini FCW-Struct
                EDDI_SERIniSleepingConsumerFcw(&IniFCW, (LSA_UINT16)EDDI_CRT_PORT_L, &CCWLocal, pDDB);

                //Add Fcw and Store reference to TreeElement
                pTree     = &pDDB->CRT.IRTRedAx.IrtRedTreeRx[HwRxPortIndex];
                pTreeElem = EDDI_RedTreeIrtAddCw(pDDB, pTree, pFrmHandler, pFrmData, IniFCW.Time, &CCWLocal);
                if (pTreeElem)
                {
                    pTreeElem->HwTxPortMask = 0;
                }
                pFrmHandler->pIRTRcvEl[HwRxPortIndex] = pTreeElem;
            }
            break;

            case EDDI_SYNC_IRT_PROVIDER: 
            {
                for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
                {
                    //UsrPortIndex ist Absicht -> Vergleich auf User Sicht in pFrmData
                    if (EDDI_IRTIsTxPortSet(pFrmData, PortMapCnt, UsrPortIndex))
                    {
                        LSA_UINT32  const  HwPortTxIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

                        if (IniFCW.FrameID == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
                        {
                            EDDI_MemSet(&CCWLocal, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_SINGLE_SHOT_SND_TYPE)); //do not change!

                            //ini FCW-Struct
                            EDDI_SERSingleShotSndIni(&IniFCW, &CCWLocal, pDDB);
                        }
                        else
                        {
                            EDDI_MemSet(&CCWLocal, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER10_FCW_SND_TYPE)); //do not change!

                            //ini FCW-Struct
                            EDDI_SERIniSleepingProviderFcw(&IniFCW, &CCWLocal, pDDB);
                            
                          if  /* time for local reception > time already stored at this port */
                              (pFrmData->FrameSendOffset > pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsTx)
                          {
                              pRecordSet->BeginEndData[UsrPortIndex].EndLocalNsTx        = pFrmData->FrameSendOffset;
                              pRecordSet->BeginEndData[UsrPortIndex].LastLocalFrameLenTx = pFrmData->DataLength;
                          }
                        }

                        //Add Fcw and Store reference to TreeElement
                        pTree     = &pDDB->CRT.IRTRedAx.IrtRedTreeTx[HwPortTxIndex];
                        pTreeElem = EDDI_RedTreeIrtAddCw(pDDB, pTree, pFrmHandler, pFrmData, IniFCW.Time, &CCWLocal);
                        if (pTreeElem)
                        {
                            pTreeElem->HwTxPortMask = 0;
                        }
                        pFrmHandler->pIRTSndEl[HwPortTxIndex] = pTreeElem;
                    }
                }
            }
            break;

            case EDDI_SYNC_IRT_INVALID_HANDLER:
            default:
            {
                EDDI_Excp("EDDI_IRTFcwAdd, EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IRTFcwRemove()
 *
 * function:      Remove a forwarder for IRT-Communication
 *
 * parameters:
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IRTFcwRemove( EDDI_LOCAL_DDB_PTR_TYPE       const  pDDB,
                                                  EDDI_IRT_FRM_HANDLER_PTR_TYPE const  pFrmHandler )
{
    LSA_UINT32         UsrPortIndex;
    LSA_UINT32         iRemoved = 0;
    LSA_UINT32  const  PortMapCnt = pDDB->PM.PortMap.PortCnt;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_IRTFcwRemove->");

    for (UsrPortIndex = 0; UsrPortIndex < PortMapCnt; UsrPortIndex++)
    {
        LSA_UINT32  const  HwPortIndex = EDDI_PmGetHwPortIndex(pDDB, UsrPortIndex);

        switch (pFrmHandler->HandlerType)
        {
            case EDDI_SYNC_IRT_FORWARDER:
            case EDDI_SYNC_IRT_FWD_CONSUMER:
            case EDDI_SYNC_IRT_CONSUMER:
            {
                EDDI_TREE_ELEM_PTR_TYPE  const  pIRTRcvEl = pFrmHandler->pIRTRcvEl[HwPortIndex];

                if (pIRTRcvEl)
                {
                    EDDI_RedTreeIrtRemoveCw(&pDDB->CRT.IRTRedAx.IrtRedTreeRx[HwPortIndex],
                                            (EDDI_TREE_ELEM_PTR_TYPE)(void *)pIRTRcvEl,
                                            pDDB);

                    pFrmHandler->pIRTRcvEl[HwPortIndex] = EDDI_NULL_PTR;
                    iRemoved++;
                }
            }
            break;

            case EDDI_SYNC_IRT_PROVIDER:
            {
                EDDI_TREE_ELEM_PTR_TYPE  const  pIRTSndEl = pFrmHandler->pIRTSndEl[HwPortIndex];

                if (pIRTSndEl)
                {
                    EDDI_RedTreeIrtRemoveCw(&pDDB->CRT.IRTRedAx.IrtRedTreeTx[HwPortIndex],
                                            (EDDI_TREE_ELEM_PTR_TYPE)(void *)pIRTSndEl,
                                            pDDB);

                    pFrmHandler->pIRTSndEl[HwPortIndex] = EDDI_NULL_PTR;
                    iRemoved++;
                }
            }
            break;

            case EDDI_SYNC_IRT_INVALID_HANDLER:
            default:
            {
                EDDI_Excp("EDDI_IRTFcwRemove, EDDI_NULL_PTR, pFrmData", EDDI_FATAL_ERR_EXCP, 0, 0);
                return;
            }
        }
    }

    if (iRemoved == 0)
    {
        // At least one DataElement must have been removed.
        EDDI_Excp("EDDI_IRTFcwRemove, iRemoved == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_IRTIsTxPortSet()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_IRTIsTxPortSet( EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE  const  pIrFrameDataElem,
                                                    LSA_UINT32                           const  PortCnt,
                                                    LSA_UINT32                           const  UsrPortIndex )
{
    LSA_UINT8  const  TestMask = (LSA_UINT8)((LSA_UINT32)1 << (UsrPortIndex + 1));

    if (UsrPortIndex >= PortCnt)
    {
        EDDI_Excp("EDDI_IRTIsTxPortSet, UsrPortIndex > pDDB->PM.PortMap.PortCnt", EDDI_FATAL_ERR_EXCP, UsrPortIndex, 0);
        return LSA_FALSE;
    }

    if (pIrFrameDataElem->UsrTxPortGroupArray[0] & TestMask)
    {
        return LSA_TRUE;
    }

    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_irt_usr.c                                               */
/*****************************************************************************/

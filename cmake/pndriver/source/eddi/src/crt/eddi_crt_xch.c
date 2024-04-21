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
/*  F i l e               &F: eddi_crt_xch.c                            :F&  */
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
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_CRT_XCH
#define LTRC_ACT_MODUL_ID  112

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if !defined (EDDI_CFG_ERTEC_400)
#if defined (M_ID_CRT_XCH) //satisfy lint!
#endif
#endif

#if defined (EDDI_CFG_ERTEC_400) || defined (EDDI_CFG_REV7)

#include "eddi_dev.h"
#if defined (EDDI_CFG_ERTEC_400)
    #include "eddi_ser_ext.h"
#endif
#include "eddi_crt_ext.h"

#include "eddi_crt_check.h"
#include "eddi_crt_xch.h"

#if defined (EDDI_CFG_REV7)
#include "eddi_pool.h"
#include "eddi_ser_cmd.h"
#endif

#if defined (EDDI_CFG_ERTEC_400)
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTConsumerCopyAndChangeBuffer( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                                         EDDI_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                         EDDI_LOCAL_MEM_U8_PTR_TYPE const pNewBuffer,
                                                                         LSA_BOOL                   const bCopyData );
#endif

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_CRTProviderListAddToXChangeQueue( EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                                           EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

#if defined (EDDI_CFG_REV7)
static LSA_VOID EDDI_LOCAL_FCT_ATTR CRTConsumerCopyAndChangePaeaBuffer( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                                        EDDI_CRT_CONSUMER_PTR_TYPE const pConsumer,
                                                                        EDDI_LOCAL_MEM_U8_PTR_TYPE const pNewUserBuffer );
#endif


#if defined (EDDI_CFG_REV7)
//////////////////////////////////////////////////////////////////////////
///
/// Used to move APDU and UserData in KRAM and PAEA.
/// This is supported by the SOC via CommandInterface. Primary + Secondary
/// Command are:
///
/// PRIM_CMD =>  RCV_ACW
/// SEC_CMD  =>  +-------------+
///              | StateBuffer |
///              |  (KRAM)     |
///              | +---------+ |           PAEA
///              | |  APDU   | |       |   ...    |
///              | +---------+ |       +----------+
///              | | PAEA_PTR|-+------>|   User   |
///              | +---------+ |       |   Data   |
///              +-------------+       +----------+
///                                    |   ...    |
///
/// @param pDDB containing the RCV_ACW
/// @param pConsumer for the Consumer-ID
/// @param pNewUserBuffer relative address to the new UserData in PAEA-RAM
///
//////////////////////////////////////////////////////////////////////////
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  CRTConsumerCopyAndChangePaeaBuffer( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                           EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                           EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pNewUserBuffer )
{
    LSA_UINT32                   status;
    EDDI_CRT_PAEA_APDU_STATUS  * pNewStateBuffer;
    EDDI_CRT_PAEA_APDU_STATUS  * pOldStateBuffer;
    LSA_UINT32                   dev_pAcw;
    LSA_UINT32                   dev_pApdu;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "CRTConsumerCopyAndChangePaeaBuffer->");
 
    if (!pConsumer->usingPAEA_Ram)
    {
        EDDI_Excp("CRTConsumerCopyAndChangePaeaBuffer, No using PAEA-RAM!", EDDI_FATAL_ERR_PPARAM, 0, 0);
        return;
    }

    if (   (EDDI_NULL_PTR == pConsumer->pLowerCtrlACW) 
        && ((EDD_DATAOFFSET_INVALID == pConsumer->LowerParams.DataOffset) || (EDD_DATAOFFSET_UNDEFINED == pConsumer->LowerParams.DataOffset)))
    {
        EDDI_Excp("CRTConsumerCopyAndChangePaeaBuffer, No ACW was setuped!", EDDI_FATAL_ERR_PPARAM, 0, 0);
        return;
    }

    pOldStateBuffer = (EDDI_CRT_PAEA_APDU_STATUS*)(*(pDDB->CRT.PAEA_Para.pConsumerMirrorArray + pConsumer->ConsumerId));

    status = EDDI_MemGetApduBuffer(pConsumer->ConsumerId, pDDB, &pNewStateBuffer, EDDI_MEM_APDU_CONSUMER_BUFFER);
    if (EDD_STS_OK != status)
    {
        EDDI_Excp("CRTConsumerCopyAndChangePaeaBuffer, Run out of APDU-Space!", EDDI_FATAL_ERR_PPARAM, 0, 0);
        return;
    }

    pNewStateBuffer->pPAEA_Data = EDDI_SWAP_32((LSA_UINT32)pNewUserBuffer);
    pConsumer->LowerParams.pKRAMDataBuffer = (EDD_UPPER_MEM_U8_PTR_TYPE)(void *)pNewStateBuffer;

    if (EDDI_CRT_CONS_STS_ACTIVE == pConsumer->pSB->Status)
    {
        dev_pAcw   = DEV_kram_adr_to_asic_register((LSA_VOID *)pConsumer->pLowerCtrlACW->pKramCW, pDDB);
        dev_pApdu  = DEV_kram_adr_to_asic_register((LSA_VOID *)pNewStateBuffer, pDDB);
        EDDI_SERCommand(FCODE_MOVE_RCV_ACW_DB, dev_pAcw, dev_pApdu, 0, pDDB);
    } 
    else if (EDDI_CRT_CONS_STS_INACTIVE == pConsumer->pSB->Status)
    {         
        LSA_UINT8 *  pSrc = (LSA_UINT8 *)(pDDB->CRT.PAEA_Para.PAEA_BaseAdr + (LSA_UINT32)(pOldStateBuffer->pPAEA_Data << 2)); 
        LSA_UINT8 *  pDst = (LSA_UINT8 *)(pDDB->CRT.PAEA_Para.PAEA_BaseAdr + (LSA_UINT32)(pNewStateBuffer->pPAEA_Data << 2));        
        
        //If no ACW was setuped then copy only the pointers
        if (EDDI_NULL_PTR == pConsumer->pLowerCtrlACW)
        {
            pDst = pSrc;            
        }
        else
        {       
            LSA_UINT32  const  Len = pConsumer->LowerParams.Partial_DataLen?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;
            LSA_UINT32         i;  
            
            /*1. Copy APDU-Status*/
            pNewStateBuffer->APDU_Status.Block = pOldStateBuffer->APDU_Status.Block;
            
            /*2. Copy PAEA Buffer*/
            for (i = 0; i < Len; i++)
            {
                *pDst = *pSrc;     
                pDst += sizeof(LSA_UINT32);
                pSrc += sizeof(LSA_UINT32);  
            }

            /*3. Insert new State-Buffer-Pointer into ACW*/
            EDDI_SetBitField32(&pConsumer->pLowerCtrlACW->pKramCW->AcwRcv.Hw1.Value.U32_0, EDDI_SER10_LL1_ACW_RCV_BIT__pDB0,
                                DEV_kram_adr_to_asic_register(pNewStateBuffer, pDDB));                     /* 21 Bit */     
        }  
    }

    // Invalidate old APDU-slot by setting it to 0xFF
    EDDI_MemSet(pOldStateBuffer, (LSA_UINT8)0xFF, (LSA_UINT32)sizeof(EDDI_CRT_PAEA_APDU_STATUS));
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/******************************************************************************
 *  Function:   EDDI_CRTConsumerListXchangeBuf()
 *
 *  Description:Changes the Buffer of a SRT-Consumer
 *
 *  Arguments:  pRQB
 *              pHDB
 *
 *  Return:     EDD_STS_OK if succeeded,
 *              EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerListXchangeBuf( EDD_UPPER_RQB_PTR_TYPE  const  pRQB,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDD_UPPER_CSRT_CONSUMER_XCHANGE_BUF_PTR_TYPE  const  pParam = (EDD_UPPER_CSRT_CONSUMER_XCHANGE_BUF_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_CRT_CONSUMER_PTR_TYPE                           pConsumer;
    LSA_RESULT                                           Status;
    #if !defined (EDDI_CFG_REV7)
    EDDI_LOCAL_MEM_U8_PTR_TYPE                            pNewBuffer;
    #endif

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTConsumerListXchangeBuf");

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+                            Check all Paramters
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (EDD_STS_OK != EDDI_CRTConsumerListGetEntry(pDDB, &pConsumer,
                                                   &pDDB->CRT.ConsumerList,
                                                   pParam->ConsumerID))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListXchangeBuf ERROR: Consumer not in List; pParam->ConsumerID, - -> pParam->ConsumerID:0x%X",
                          pParam->ConsumerID);
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pConsumer->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTConsumerListXchangeBuf ERROR: UDP Consumer not supported; pParam->ConsumerID, - -> pParam->ConsumerID:0x%X",
                          pParam->ConsumerID);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    Status = EDDI_CRTConsumerXchangeCheckRQB(pParam, pConsumer, pDDB);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    #if defined (EDDI_CFG_REV7)
    //PAEA data size does not matter; always 8-Byte
    CRTConsumerCopyAndChangePaeaBuffer(pDDB, pConsumer, (EDDI_LOCAL_MEM_U8_PTR_TYPE)(pParam->NewOffset));
    #else
    {
        LSA_UINT16  const  KramDatalen = (pConsumer->LowerParams.Partial_DataLen)?pConsumer->LowerParams.Partial_DataLen:pConsumer->LowerParams.DataLen;
    
        EDDI_CRTCheckAddRemoveMirror(pDDB, KramDatalen, pConsumer->LowerParams.DataOffset,
                                     pConsumer->LowerParams.ListType, LSA_FALSE, CRT_MIRROR_REMOVE);

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        //+                            XCHANGE BUFFERS
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        pNewBuffer = pDDB->pKRam + pParam->NewOffset;

        EDDI_CRTConsumerCopyAndChangeBuffer(pDDB, pConsumer, pNewBuffer, pParam->CopyUserData);

        // update CONSUMER->LowerParams and BufferPtr
        pConsumer->LowerParams.DataOffset       = pParam->NewOffset;
        pConsumer->LowerParams.pKRAMDataBuffer  = pNewBuffer;
        pConsumer->pUserDataBuffer              = pNewBuffer;
        pConsumer->pSB->pCopyKRAMConsDataStatus = pNewBuffer + 2; // direct BytePointer to DataStatus !!

        EDDI_CRTCheckAddRemoveMirror(pDDB, KramDatalen, pConsumer->LowerParams.DataOffset,
                                     pConsumer->LowerParams.ListType, LSA_FALSE, CRT_MIRROR_ADD);
    }
    #endif

    //return new APDUStatusOffset
    pParam->APDUStatusOffset = (LSA_UINT32)pConsumer->LowerParams.pKRAMDataBuffer - (LSA_UINT32)pDDB->pKRam;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_ERTEC_400)
/******************************************************************************
 *  Function:    EDDI_CRTConsumerCopyAndChangeBuffer()
 *
 *  Description: Makes a Copy from Old Databuffer to new.
 *
 *
 *  Arguments:  
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTConsumerCopyAndChangeBuffer( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                            EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer,
                                                                            EDDI_LOCAL_MEM_U8_PTR_TYPE  const  pNewBuffer,
                                                                            LSA_BOOL                    const  bCopyData )
{
    EDDI_CRT_DATA_APDU_STATUS  APDUbefore,      //APDU Status before memcopy
                               APDUafter;       //APDU Status after memcopy
    LSA_UINT32                 OldDataCtrlBit;  //indicates, if DataCtrl was enabled. If so
                                                //new Frames could arrive during BufferChange.
                                                //--> additional operations are needed to safely handle this case.

    APDUbefore.Block = ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer))->Block;
    OldDataCtrlBit   = EDDI_DS_BIT__Bit_3(APDUbefore.Detail.DataStatus);

    if (OldDataCtrlBit)
    {
        // 1) deactivate DataCtrl-Bit !
        EDDI_CRTDataImage(FCODE_DISABLE_DATA_IMAGE, pConsumer, pDDB);

        // 2) Wait for 6us (for safety, if frame was just received)
        EDDI_WAIT_10_NS(pDDB->hSysDev, 600UL);

        // Update storage of current APDU-Status
        APDUbefore.Block = ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer))->Block;
    }

    // 3a) Memcopy Data if wished
    if (bCopyData)
    {
        EDDI_MEMCOPY_SRT_CONSUMER_XCHANGE_BUF(pNewBuffer + (sizeof(EDDI_CRT_DATA_APDU_STATUS)),
                                              pConsumer->LowerParams.pKRAMDataBuffer + (sizeof(EDDI_CRT_DATA_APDU_STATUS)),
                                              (LSA_UINT)pConsumer->LowerParams.DataLen);
    }

    if (OldDataCtrlBit)
    {
        EDDI_CRT_DATA_APDU_STATUS  APDUnew;   //APDU Status for the new Buffer

        APDUnew.Block = APDUbefore.Block;
        // Reactivate Data-Ctrl-Bit
        //    APDUnew.Detail.DataStatus.xBit.xBit_3 = 1; // Prepare new APDU as activated !
        APDUnew.Detail.DataStatus |= 0x08; // Prepare new APDU as activated !

        ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pNewBuffer))->Block = APDUnew.Block;
    }

    // 4) Change BufferPtr
    EDDI_SERXchangeBuffer(pDDB, (EDDI_SER_CCW_PTR_TYPE)(void*)(pConsumer->pLowerCtrlACW->pKramCW), pNewBuffer);

    // 5) Get APDU Status of old Buffer
    APDUafter.Block = ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pConsumer->LowerParams.pKRAMDataBuffer))->Block;

    if (OldDataCtrlBit)
    {
        // Check if new APDU-Status-Data has arrived during Memcopy
        if (APDUbefore.Detail.CycleCnt != APDUafter.Detail.CycleCnt)
        {
            EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_CRTConsumerCopyAndChangeBuffer > WARNING: One Frame may be lost.");
        }
    }
    else
    {
        ((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pNewBuffer))->Block = APDUafter.Block;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/******************************************************************************
 *  Function:    EDDI_CRTProviderListXchangeBuf()
 *
 *  Description: Changes the Buffer of a SRT-Provider
 *               Before this Request can be finished, it must be kept for at least
 *               one Cycle in the EDDI.
 *               This is necesary to garanty that the old Buffer
 *               wont be used any more.
 *
 *  Arguments:
 *
 *  Return:      EDD_STS_OK if succeeded,
 *               EDD_STS_ERR_PARAM else.
 */
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListXchangeBuf( EDD_UPPER_RQB_PTR_TYPE      const  pRQB,
                                                                 EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                 LSA_BOOL                 *  const  pIndicate )
{
    EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE  const  pParam = (EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    EDDI_CRT_PROVIDER_PTR_TYPE                           pProvider;
    LSA_RESULT                                           Status;
    #if defined (EDDI_CFG_REV7)
    EDDI_CRT_PAEA_APDU_STATUS                         *  pProviderAPDU;
    LSA_UINT32                                           kram_apdu_pos;
    #else
    EDDI_LOCAL_MEM_U8_PTR_TYPE                           pNewBuffer;
    #endif

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderListXchangeBuf->");

    *pIndicate = LSA_TRUE;

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+                            Check all Parameters
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if (EDD_STS_OK != EDDI_CRTProviderListGetEntry(pDDB, &pProvider,
                                                   &pDDB->CRT.ProviderList,
                                                   pParam->ProviderID))
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderListXchangeBuf, ERROR: Provider not in List ; pParam->ProviderID, - -> pParam->ProviderID:0x%X",
                          pParam->ProviderID);
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_XRT_OVER_UDP_SOFTWARE)
    if (pProvider->LowerParams.RT_Type == EDDI_RT_TYPE_UDP)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR,
                          "EDDI_CRTProviderListXchangeBuf, ERROR: UDP Provider not supported; pParam->ConsumerID, - -> pParam->ConsumerID:0x%X",
                          pParam->ProviderID);
        return EDD_STS_ERR_PARAM;
    }
    #endif

    Status = EDDI_CRTProviderXchangeCheckRQB(pParam, pProvider, pDDB);
    if (Status != EDD_STS_OK)
    {
        return Status;
    }

    #if defined (EDDI_CFG_REV7)
    kram_apdu_pos = pParam->ProviderID * sizeof(EDDI_CRT_PAEA_APDU_STATUS);
    pProviderAPDU = (EDDI_CRT_PAEA_APDU_STATUS *)((void *)(pDDB->pKRamStateBuffer + kram_apdu_pos));

    //Check the old PAEA-Pointer and alter it, if it is different from new one
    if (EDDI_IRTE2HOST32(pProviderAPDU->pPAEA_Data) == pParam->NewOffset)
    {
        EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_CRTProviderListXchangeBuf, ERROR: old PAEA-Offset == new PAEA-Offset, abort XCHANGE_BUFF");
        EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_INV_DataOffset);
        return EDD_STS_ERR_PARAM;
    }

    pProviderAPDU->pPAEA_Data = EDDI_HOST2IRTE32(pParam->NewOffset);

    //update PROVIDER->LowerParams and BufferPtr
    pProvider->LowerParams.DataOffset = pParam->NewOffset;

    #else
    EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                 pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_REMOVE);

    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //+                            XCHANGE BUFFERS
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    pNewBuffer = pDDB->pKRam + pParam->NewOffset;

    // Copy APDU-Status
    *((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pNewBuffer)) = *((EDDI_CRT_DATA_APDU_STATUS *)(void *)(pProvider->LowerParams.pKRAMDataBuffer));

    // Change Buffer Pointer in ACW
    EDDI_SERXchangeBuffer(pDDB, pProvider->pLowerCtrlACW->pCW, pNewBuffer);

    // update PROVIDER->LowerParams and BufferPtr
    pProvider->LowerParams.DataOffset      = pParam->NewOffset;
    pProvider->LowerParams.pKRAMDataBuffer = pNewBuffer;
    pProvider->pUserDataBuffer             = pNewBuffer;
    #endif

    // Add RQB to Provider-Xchange-Queue --> Finishing this RQB will be done in context of NewCycle-Interrupt.
    EDDI_CRTProviderListAddToXChangeQueue(pRQB, pDDB);
    pProvider->IsXChangingBuffer = LSA_TRUE;

    #if defined (EDDI_CFG_ERTEC_400)
    EDDI_CRTCheckAddRemoveMirror(pDDB, pProvider->LowerParams.DataLen, pProvider->LowerParams.DataOffset,
                                 pProvider->LowerParams.ListType, LSA_TRUE, CRT_MIRROR_ADD);
    #endif

    *pIndicate = LSA_FALSE;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTProviderListAddToXChangeQueue()
 *
 *  Description: Adds a new Entry in the RQB-Queue for the XChanges of ProviderBuffers
 *               And activates the NewCycle-Trigger if necesary.
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProviderListAddToXChangeQueue( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                              EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    //pDDB->pLocal_CRT->RQBQueue
    EDDI_CRT_XCHANGE_TYPE  *  const  pXChangeHandling = &pDDB->pLocal_CRT->XChangeHandling;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProviderListAddToXChangeQueue->");

    //If the First Element was added --> Register for NewCycle-Interrupt
    //Registering for Drop events is not needed (DropEvent is not used)

    EDDIQueueAddToBegin(&pXChangeHandling->RQBQueue, (EDDI_QUEUE_ELEMENT_PTR_TYPE)(void *)pRQB);

    if (pXChangeHandling->RQBQueue.Count == 1)
    {
        //neutralize the finish Border
        pXChangeHandling->pFinishBorder = EDDI_NULL_PTR;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/******************************************************************************
 *  Function:    EDDI_CRTTriggerBufferXchange()
 *
 *  Description: This Function will be triggered by a NewCycle-Interrupt
 *               and handles
 *
 *                                 pFinishBorder ( Will be moved to Begin
 *                                  |              of Queue every NewCycle)
 *                                  |
 *   RQBQueue:  * - W - W - W - W - F - F - F - x
 *              |                               |
 *              |                              Old XchangeRQBs will be finished
 *              |                              from here until pFinishBorder
 *              |
 *              New XchangeRQBs will be added to Begin of Queue
 *
 *
 *  Arguments:
 *
 *  Return:      LSA_VOID
 */
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTTriggerBufferXchange( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_QUEUE_ELEMENT_PTR_TYPE                             pCurrentRQB;
    LSA_UINT32                                              safetyCounter;
    EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE            pParam;
    EDDI_CRT_PROVIDER_PTR_TYPE                              pProvider;
    EDDI_CRT_XCHANGE_TYPE                         *  const  pXChangeHandling = &pDDB->pLocal_CRT->XChangeHandling;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTTriggerBufferXchange->");

    if (!(pXChangeHandling->pFinishBorder == EDDI_NULL_PTR))
    { // OK We have some RQBs to Finish
        pCurrentRQB = pXChangeHandling->RQBQueue.pLast;

        safetyCounter = 0;
        while (!(pCurrentRQB == EDDI_NULL_PTR))
        {
            if ((safetyCounter++) > 1000)
            {
                EDDI_Excp("EDDI_CRTTriggerBufferXchange, List Corrupt!", EDDI_FATAL_ERR_PPARAM, 0, 0);
                return;
            }

            pCurrentRQB = EDDI_QueueRemoveFromEnd(&pXChangeHandling->RQBQueue);
            if (pCurrentRQB == EDDI_NULL_PTR)
            {
                EDDI_Excp("EDDI_CRTTriggerBufferXchange, RQB expected but NullPtr received!", EDDI_FATAL_ERR_PPARAM, 0, 0);
                return;
            }

            // Update Provider-Info.
            pParam    = (EDD_UPPER_CSRT_PROVIDER_XCHANGE_BUF_PTR_TYPE)EDD_RQB_GET_PPARAM(((EDD_UPPER_RQB_PTR_TYPE)(void *)pCurrentRQB));
            pProvider = EDDI_NULL_PTR;
            if (EDD_STS_OK != EDDI_CRTProviderListGetEntry(pDDB, &pProvider,
                                                           &pDDB->CRT.ProviderList,
                                                           pParam->ProviderID))
            {
                EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE,
                                  "EDDI_CRTTriggerBufferXchange, WARNING: Provider was removed during BufferXChange; pParam->ProviderID, - -> pParam->ProviderID:0x%X",
                                  pParam->ProviderID);
                // Never mind if no more Provider was found: maybe it was removed.
            }
            else
            {
                pProvider->IsXChangingBuffer = LSA_FALSE;
            }

            EDDI_RequestFinish(pDDB->pLocal_CRT->pHDB, (EDD_UPPER_RQB_PTR_TYPE)(LSA_VOID *)pCurrentRQB, EDD_STS_OK);

            if (pCurrentRQB == pXChangeHandling->pFinishBorder)
            {
                // Border Reached --> No more RQBs to send within this Cycle
                break;
            }
        }
    }

    // Move Finish-Border to Begin of Queue (if no more Entry is there it will be automatically EDDI_NULL_PTR)
    pXChangeHandling->pFinishBorder = pXChangeHandling->RQBQueue.pFirst;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif


/*****************************************************************************/
/*  end of file eddi_crt_xch.c                                               */
/*****************************************************************************/

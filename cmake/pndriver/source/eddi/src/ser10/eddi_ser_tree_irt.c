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
/*  F i l e               &F: eddi_ser_tree_irt.c                       :F&  */
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
/*  25.10.07    JS    added support of secondary RT-Sync master              */
/*                    (use EDDI_SyncIrFrameHandlerGetRealFrameID())          */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ser_ext.h"
#include "eddi_sync_ir.h"

#include "eddi_pool.h"

#define EDDI_MODULE_ID     M_ID_SER_TREE_IRT
#define LTRC_ACT_MODUL_ID  308

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:        EDDI_RedTreeIrtAddCw()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtAddCw( EDDI_LOCAL_DDB_PTR_TYPE                 const  pDDB,
                                                                    EDDI_TREE_PTR_TYPE                      const  pTree,
                                                                    EDDI_CONST_IRT_FRM_HANDLER_PTR_TYPE     const  pFrmHandler,
                                                                    EDDI_PRM_RECORD_FRAME_DATA_PTR_TYPE     const  pFrmData,
                                                                    LSA_UINT32                              const  FcwTimeNs,
                                                                    EDDI_SER10_CCW_TYPE                  *  const  pFcw )
{
    EDDI_TREE_ELEM_PTR_TYPE  pNewElem;
    EDDI_TREE_ELEM_PTR_TYPE  pPrevElem;
    #if defined (EDDI_CFG_REV5)
    EDDI_TREE_ELEM_PTR_TYPE  pNextElem;
    #endif
    LSA_UINT32              Size;
    LSA_UINT32              hPoolCCW;
    LSA_UINT32              Reduction;
    LSA_UINT32              Phase;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "RedTreeIrtAddFcw->");

    //get free TreeElem
    pNewElem = EDDI_RedTreePopTxRxElem(pTree);

    //Prepare Params

    Reduction = (LSA_UINT32)pFrmData->ReductionRatio;
    Phase     = (LSA_UINT32)pFrmData->Phase;

    pNewElem->Sub.TxRx.Time    = FcwTimeNs;
    pNewElem->Sub.TxRx.DataLen = pFrmData->DataLength;

    switch (pFrmHandler->HandlerType)
    {
        case EDDI_SYNC_IRT_PROVIDER:
        {
            if (EDDI_SyncIrFrameHandlerGetRealFrameID(pFrmHandler->FrameId) == EDD_SRT_FRAMEID_PTCP_SYNC_RT_CLASS3)
            {
                hPoolCCW = pDDB->SYNC.MemPool.hIrtSyncSnd;
            }
            else
            {
                hPoolCCW = pDDB->SYNC.MemPool.hIrtProviderFcw;
            }

            break;
        }

        case EDDI_SYNC_IRT_FWD_CONSUMER:
        case EDDI_SYNC_IRT_CONSUMER:
        {
            hPoolCCW = pDDB->SYNC.MemPool.hIrtConsumerFcw;
            break;
        }

        case EDDI_SYNC_IRT_FORWARDER:
        {
            hPoolCCW = pDDB->SYNC.MemPool.hIrtForwarderFcw;
            break;
        }

        case EDDI_SYNC_IRT_INVALID_HANDLER:
        default:
        {
            EDDI_Excp("EDDI_RedTreeIrtAddCw HandlerType", EDDI_FATAL_ERR_EXCP, pFrmHandler->HandlerType, 0);
            return EDDI_NULL_PTR;
        }
    }

    //Get Size of PoolBuffer-Element
    Size = EDDI_MemGetPoolBuffersize(hPoolCCW);

    /*Check if we have enough pool resources for IRT_FORWARDER. If not */
    /*use CONSUMER (there must be enough room because we checked this  */
    /*on prm-commit!                                                   */

    if (   (pFrmHandler->HandlerType == EDDI_SYNC_IRT_FORWARDER)
        && (!EDDI_MemCheckPoolFreeBuffer(hPoolCCW)))
    {
        EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_RedTreeIrtAddCw, No more FORWARDER FCW. Using CONSUMER FCW.");
        //for Irt-Forwarder we also can use the pool hIrtConsumerFcw.
        hPoolCCW = pDDB->SYNC.MemPool.hIrtConsumerFcw;
        //ATTENTION with the Size: must be the same as of an hIrtForwarderFcw - Element !!
    }

    //get New CW from KRAM
    EDDI_MemGetPoolBuffer(hPoolCCW, (void * *)&pNewElem->pCW);

    if (pNewElem->pCW == EDDI_NULL_PTR)
    {
        EDDI_Excp("RedTreeIrtAddFcw", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDDI_NULL_PTR;
    }

    //Copy local CCW into device CCW.
    DEV32_MEMCOPY(&pNewElem->pCW->FcwAcw, pFcw, Size);

    //Remember pooltype for later remove
    pNewElem->hPoolCCW = hPoolCCW;

    //Search Previous of new Element
    pPrevElem = EDDI_RedTreeGetPrevElemByTime(pTree, (LSA_UINT16)Reduction, (LSA_UINT16)Phase, pNewElem->Sub.TxRx.Time);

    #if defined (EDDI_CFG_REV5)
    //do not change location!
    pNextElem = pPrevElem->pNextElem;
    #endif

    EDDI_RedTreeChainElem(pPrevElem, pNewElem);

    #if defined (EDDI_CFG_REV5)
    pNewElem->bCwBranchLinking = LSA_FALSE;
    EDDI_SERInsertCwDyn(pDDB, pPrevElem->pCW, pNextElem->pCW, pNewElem->pCW, pPrevElem->bCwBranchLinking);
    #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pPrevElem->Opc == SER_RCW_PHS)
    {
        //Special Treatment for RCW_PHS_List
        EDDI_SERInsertCwToRcwPhase(pDDB, &pPrevElem->pCW->ListHead, pNewElem->pCW);
    }
    else
    {
        EDDI_SERInsertCw(pDDB, pPrevElem->pCW, pNewElem->pCW);
    }
    #endif

    return pNewElem;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeIrtRemoveCw()                        */
/*                                                                         */
/* D e s c r i p t i o n: only used for FCWs!                              */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtRemoveCw( EDDI_TREE_PTR_TYPE       const  pTree,
                                                        EDDI_TREE_ELEM_PTR_TYPE  const  pTreeElem,
                                                        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_TREE_ELEM_PTR_TYPE  const  pPrevElem = pTreeElem->pPrevElem;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeIrtRemoveCw->");

    //ATTENTION: Productive FCW-List must completely be inactive!

    #if !defined (EDDI_CFG_REV5)
    LSA_UNUSED_ARG(pDDB); //satisfy lint
    #endif

    if (!pPrevElem)
    {
        EDDI_Excp("EDDI_RedTreeIrtRemoveCw", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    #if defined (EDDI_CFG_REV5)
    {
        EDDI_TREE_ELEM_PTR_TYPE  const  pNextElem = pTreeElem->pNextElem;

        EDDI_SERDisconnectCwDyn(pDDB, pPrevElem->pCW, pNextElem->pCW, pTreeElem->pCW, pPrevElem->bCwBranchLinking);
    }
    #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pPrevElem->Opc == SER_RCW_PHS)
    {
        //Special Treatment for RCW_PHS_List
        EDDI_SERDisconnectCwFromRcwPhase(&pPrevElem->pCW->ListHead, pTreeElem->pCW);
    }
    else
    {
        EDDI_SERDisconnectCw(pPrevElem->pCW, pTreeElem->pCW);
    }
    #endif

    EDDI_MemFreePoolBuffer(pTreeElem->hPoolCCW, (EDDI_DEV_MEM_PTR_TYPE)pTreeElem->pCW);

    EDDI_RedTreeUnChainElem(pTreeElem);

    //free TreeElem
    EDDI_RedTreePushTxRxElem(pTree, pTreeElem);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_RedTreeIrtSetTxActivity()
*
* function:      activate/deactivate a TX-FCW completely
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtSetTxActivity( EDDI_TREE_ELEM_PTR_TYPE  const  pTreeElem,
                                                             LSA_BOOL                 const  bActivate )
{
    //Function is used for FCW-Provider

    if (bActivate)
    {
        EDDI_SetBitField32(&pTreeElem->pCW->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_IRT_DATA);
    }
    else
    {
        EDDI_SetBitField32(&pTreeElem->pCW->FcwAcw.Common.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_RedTreeIrtSetRxLocalActivity()
*
* function:      activate/deactivate local receiving in a RX-FCW
*                ATTENTION: manipulating the FCW-opcode (NOOP<->IRT) is not allowed because of IRTE-R5-R7-Problem!
*
* parameters:
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtSetRxLocalActivity( EDDI_TREE_ELEM_PTR_TYPE  const  pTreeElem,
                                                                  LSA_BOOL                 const  bActivate )
{
    //Function is used for FCW-Consumer

    if (bActivate)
    {
        EDDI_SetBitField32(&pTreeElem->pCW->FcwAcw.FcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 1);
    }
    else
    {
        EDDI_SetBitField32(&pTreeElem->pCW->FcwAcw.FcwRcv.Hw0.Value.U32_0, EDDI_SER10_LL0_FCW_RCV_BIT__Local, 0);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeIrtWaitForDeadline()                 */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtWaitForDeadline( LSA_UINT32               const  DeadLine,
                                                               EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  WaitingTime;
    LSA_UINT32  CurCycleTime;

    CurCycleTime = IO_R32(CYCL_TIME_VALUE);

    // Wait until Deadline element is reached
    if (CurCycleTime < (DeadLine + 50))
    {
        WaitingTime = (DeadLine + 50) - CurCycleTime;
        EDDI_WAIT_10_NS(pDDB->hSysDev, WaitingTime);
    }

    pDDB->IRT.DebugRemoveCycleTime = CurCycleTime;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser_tree_irt.c                                          */
/*****************************************************************************/




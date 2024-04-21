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
/*  F i l e               &F: eddi_ser_tree_usr.c                       :F&  */
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
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_ser_ext.h"
#include "eddi_ser_cmd.h"

#include "eddi_pool.h"
#include "eddi_crt_check.h"

#define EDDI_MODULE_ID     M_ID_SER_TREE_USR
#define LTRC_ACT_MODUL_ID  306

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeSrtProviderAdd()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderAdd( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                             EDDI_CRT_PROVIDER_PARAM_TYPE  *  const  pLowerParams,
                                                             EDDI_TREE_ELEM_PTR_TYPE       *  const  ppElemHandle,
                                                             EDDI_SER_CCW_PTR_TYPE            const  pCWStored )
{
    EDDI_TREE_ELEM_PTR_TYPE         pNewElem;
    EDDI_TREE_ELEM_PTR_TYPE         pPrevElem;
    EDDI_TREE_PTR_TYPE       const  pTree = &pDDB->CRT.SrtRedTree;
    LSA_UINT32                      asic_pPrevCW;
    LSA_UINT32                      asic_pNewCW;
    EDDI_SER10_FCW_ACW_TYPE         FcwAcwLocal;
    LSA_UINT32                      Size;
    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    LSA_UINT32               const  NewSequenceNr = (LSA_UINT16)(pLowerParams->CyclePhaseSequence - 1);
    #endif

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeSrtProviderAdd->");

    //Check Params
    if (   (!EDDI_RedIsBinaryValue((LSA_UINT32)pLowerParams->CycleReductionRatio))
        || (pLowerParams->CycleReductionRatio >  pTree->MaxReduction)
        || (pLowerParams->CyclePhase          >  pLowerParams->CycleReductionRatio)
        || (pLowerParams->CyclePhase          == 0))
    {
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //Search Previous of new Element
    pPrevElem = EDDI_RedTreeGetPrevElem(pDDB, pTree, pLowerParams->CycleReductionRatio, pLowerParams->CyclePhase, pLowerParams->CyclePhaseSequence);

    if (pPrevElem == EDDI_NULL_PTR)
    {
        return EDD_STS_ERR_PARAM;
    }
    #endif

    //get free TreeElem
    pNewElem = EDDI_RedTreePopTxRxElem(pTree);

    if (pCWStored)
    {
        //reuse stored, initialized element
        pNewElem->pCW = pCWStored;
    }
    else
    {
        //get New CW from KRAM
        EDDI_MemGetPoolBuffer(pDDB->CRT.hPool.AcwProvider, (void * *)&pNewElem->pCW);

        if (pNewElem->pCW == EDDI_NULL_PTR)
        {
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_RedTreeSrtProviderAdd out of memory");
            EDDI_Excp("RedInitACWTree5", EDDI_FATAL_ERR_EXCP, 0, 0);
            return EDD_STS_ERR_RESOURCE;
        }

        pDDB->pLocal_CRT->ProviderList.UsedACWs++;

        //ini ACW
        EDDI_SERIniProviderACW(&FcwAcwLocal, pLowerParams, LSA_FALSE /*bActive*/, LSA_FALSE /*bInitBuf*/, pDDB);

        Size = EDDI_MemGetPoolBuffersize(pDDB->CRT.hPool.AcwProvider);

        //Copy local CCW into device CCW
        DEV32_MEMCOPY(&pNewElem->pCW->FcwAcw, &FcwAcwLocal, Size);
    }
    
    #if !defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //Search Previous of new Element
    pPrevElem = EDDI_RedTreeGetPrevElem(pDDB, pTree, pLowerParams->CycleReductionRatio, pLowerParams->CyclePhase, pLowerParams->CyclePhaseSequence);
    #endif

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    pNewElem->SequenceNr = NewSequenceNr;
    #endif

    EDDI_RedTreeChainElem(pPrevElem, pNewElem);

    asic_pPrevCW = DEV_kram_adr_to_asic_register(pPrevElem->pCW, pDDB);
    asic_pNewCW  = DEV_kram_adr_to_asic_register(pNewElem->pCW, pDDB);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pPrevElem->Opc == SER_RCW_PHS)
    {
        //Special Treatment for RCW_PHS_List
        asic_pPrevCW |= SER10_PTR_LIST_HEAD_MARKER; // Bit 21: AcwPositionMarker
    }
    #endif

    EDDI_SERCommand(FCODE_INSERT_ACW_TX, asic_pPrevCW, asic_pNewCW, 0, pDDB);

    //Update LoadValue
    //allow overplaning for AUX-providers
    if (EDDI_RTC3_AUX_PROVIDER != pLowerParams->ProviderType)
    {
        EDDI_RedTreeUpdatePathLoad(pTree, pLowerParams->CycleReductionRatio, pLowerParams->CyclePhase, (LSA_INT32)pLowerParams->DataLen) ;
    }

    //set the return Value
    *ppElemHandle = pNewElem;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_RedTreeSrtProviderSetActivity()
*
* function:      ini the ACW for IRT/SRT-Communication
*
* parameters:    
*
* return value:  LSA_VOID
*
*===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderSetActivity( EDDI_TREE_ELEM_PTR_TYPE  const  pLowerCtrlACW,
                                                                   LSA_BOOL                 const  bActivate )
{
    EDDI_SER10_LL0_COMMON_TYPE  LocalHw0;

    LocalHw0.Value.U32_0 = pLowerCtrlACW->pCW->FcwAcw.Common.Value.U32_0;

    //Modify part of ACW locally
    if (bActivate)
    {
        EDDI_SetBitField32(&LocalHw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_ACW_TX);
    }
    else
    {
        EDDI_SetBitField32(&LocalHw0.Value.U32_0, EDDI_SER10_LL0_COMMON_BIT__Opc, SER_NOOP);
    }

    //Write back part (4Byte) of ACW to KRAM
    pLowerCtrlACW->pCW->FcwAcw.Common.Value.U32_0 = LocalHw0.Value.U32_0;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if !defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeSrtProviderChPhasePart1()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
//temporarily_disabled_lint -save -esym(613,pPrevElemOfNewPhase) Possible use of null pointer!
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderChPhasePart1( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                      EDD_UPPER_RQB_PTR_TYPE           const  pRQB,
                                                                      EDDI_CRT_PROVIDER_PTR_TYPE       const  pProvider,
                                                                      LSA_UINT16                       const  NewCyclePhase,
                                                                      LSA_UINT32                       const  NewCyclePhaseSequence,
                                                                      LSA_BOOL                      *  const  pIndicate )
{
    // Setup of a new ACW:
    // ******************
    // 1) reuse data from old one except: NewCyclePhase, (NewCyclePhaseSequence)
    // 2) NextPtr to new successor
    // 3) Connect new predecessor with new ACW
    // 4) Predecessor of old ACW is now pointing to succesor of old ACW
    // 5) reconnect TreeElement
    // 6) free old ACW
    // 7) update bandwidth information

    EDDI_CRT_PROVIDER_PARAM_TYPE  *  const  pLowerParams = &pProvider->LowerParams;
    LSA_BOOL                         const  bActive = (pProvider->Status == EDDI_CRT_PROV_STS_ACTIVE) ? LSA_TRUE : LSA_FALSE;
    EDDI_TREE_ELEM_PTR_TYPE          const  pElemHandle = pProvider->pLowerCtrlACW;

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //conditions here: NewCyclePhase         != old CyclePhase
    //                 NewCyclePhaseSequence == old CyclePhaseSequence == EDDI_TREE_GET_LAST_OF_SEQ
    #endif

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeSrtProviderChPhasePart1");
    *pIndicate = LSA_TRUE;

    //Check Params
    if (   (NewCyclePhase >  pLowerParams->CycleReductionRatio)
        || (NewCyclePhase == 0))
    {
        return EDD_STS_ERR_PARAM;
    }

   
    if (!bActive)
    {
        //set parameters only, will be considered when provider gets activated 
        pLowerParams->CyclePhase = NewCyclePhase;  
        pLowerParams->CyclePhaseSequence = NewCyclePhaseSequence;  
    }
    else
    {
        EDDI_TREE_PTR_TYPE       const  pTree = &pDDB->CRT.SrtRedTree;
        EDDI_TREE_ELEM_PTR_TYPE  const  pPrevElemOfOldPhase = pElemHandle->pPrevElem;
        EDDI_TREE_ELEM_PTR_TYPE         pPrevElemOfNewPhase;
        LSA_UINT32                      asic_pNextOfOldPhaseCW;
        LSA_UINT32                      asic_pNextOfNewPhaseCW;
        LSA_UINT32                      asic_pNewCW;
        LSA_UINT32                      asic_pPrevCW;
        LSA_UINT32                      asic_pPrevOfOldPhaseCW;
        EDDI_SER10_FCW_ACW_TYPE         FcwAcwLocal;
        EDDI_SER_CCW_PTR_TYPE    const  pOldAcw = pElemHandle->pCW;
        EDDI_SER_CCW_PTR_TYPE           pNextAcw = pElemHandle->pNextElem->pCW;

        if (   !(pElemHandle) 
            || !(pProvider->pCWStored) )
        {
            EDDI_CRT_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_RedTreeSrtProviderChPhasePart1, pLowerCtrlACW (0x%X)/pCWStored(0x%X)=0 but status of ProvID/FrameID 0x%X/0x%X is ACTIVE", 
                pElemHandle, pProvider->pCWStored, pProvider->ProviderId, pLowerParams->FrameId);
            EDDI_Excp("EDDI_RedTreeSrtProviderChPhasePart1, pLowerCtrlACW/pCWStored(=0 but status is ACTIVE", EDDI_FATAL_ERR_EXCP, pElemHandle, pProvider->pCWStored);
            return EDD_STS_ERR_EXCP;
        }

        if (pNextAcw == 0) //gibt es das überhaupt?
        {
            asic_pNextOfOldPhaseCW = SER10_NULL_PTR;
        }
        else
        {
            asic_pNextOfOldPhaseCW = DEV_kram_adr_to_asic_register(pNextAcw, pDDB);
        }

        //Get PrevElemOfNewPhase
        pPrevElemOfNewPhase = EDDI_RedTreeGetPrevElem(pDDB, pTree,
                                                      pLowerParams->CycleReductionRatio,
                                                      NewCyclePhase,
                                                      NewCyclePhaseSequence);

        #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE_NOT_NECESSARY) //not necessary because only NewCyclePhaseSequence = EDDI_TREE_GET_LAST_OF_SEQ allowed!
        if (pPrevElemOfNewPhase == EDDI_NULL_PTR)                    //in this case the return-value == EDDI_NULL_PTR is not possible!
        {
            return EDD_STS_ERR_PARAM;
        }
        #endif

        #if !defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
        //Check if Element is already on right place
        if (   ((NewCyclePhaseSequence == EDDI_TREE_GET_LAST_OF_SEQ) && (pPrevElemOfNewPhase == pElemHandle))
               || ((NewCyclePhaseSequence != EDDI_TREE_GET_LAST_OF_SEQ) && (pPrevElemOfNewPhase == pPrevElemOfOldPhase)))
        {
            //nothing has to be changed because Element and its CW are already on right place
            return EDD_STS_OK;
        }

        if (pPrevElemOfNewPhase == pElemHandle)
        {
            //nothing has to be changed because Element and its CW are already on right place
            return EDD_STS_OK;
        }
        #endif

        //Get New CW from KRAM
        EDDI_MemGetPoolBuffer(pDDB->CRT.hPool.AcwProvider, (void * *)&pElemHandle->pCW);

        if (pElemHandle->pCW == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_RedTreeSrtProviderChPhasePart1", EDDI_FATAL_ERR_EXCP, 0, 0);
            return EDD_STS_ERR_RESOURCE;
        }

        //Update Old Pathload
        //allow overplaning for AUX-providers
        if  (EDDI_RTC3_AUX_PROVIDER != pLowerParams->ProviderType)
        {
            EDDI_RedTreeUpdatePathLoad(pTree,
                                       pLowerParams->CycleReductionRatio,
                                       pLowerParams->CyclePhase,
                                       (-1L) * pLowerParams->DataLen);
        }

        //Change PhaseInfo of LowerParams
        pLowerParams->CyclePhase         = NewCyclePhase;
        #if !defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
        pLowerParams->CyclePhaseSequence = NewCyclePhaseSequence;
        #endif

        //reconnect TreeElement TX 
        EDDI_RedTreeUnChainElem(pElemHandle);
        EDDI_RedTreeChainElem(pPrevElemOfNewPhase, pElemHandle);

        //Ini ACW
        EDDI_SERIniProviderACW(&FcwAcwLocal, pLowerParams, bActive, LSA_TRUE /*bInitBuf*/, pDDB);

        pNextAcw = pElemHandle->pNextElem->pCW;

        if (pNextAcw == 0) 
        {
            //Last ACW-Element in List needs a special "end-pointer"
            asic_pNextOfNewPhaseCW = SER10_NULL_PTR;
        }
        else
        {
            asic_pNextOfNewPhaseCW = DEV_kram_adr_to_asic_register(pNextAcw, pDDB);
        }

        asic_pNewCW = DEV_kram_adr_to_asic_register(pElemHandle->pCW, pDDB);

        //Set pNext of local CCW
        EDDI_SetBitField32(&FcwAcwLocal.Common.Value.U32_1, EDDI_SER10_LL0_COMMON_BIT2__pNext, asic_pNextOfNewPhaseCW);

        //Copy local CCW into device CCW
        DEV32_MEMCOPY(&pElemHandle->pCW->FcwAcw, &FcwAcwLocal, EDDI_MemGetPoolBuffersize(pDDB->CRT.hPool.AcwProvider));

        //Change Previous ACW of New Phase --> chains new ACW
        //(void)SERSet_asic_pNext(pPrevElemOfNewPhase->pCW, asic_pNewCW);
        asic_pPrevCW = DEV_kram_adr_to_asic_register(pPrevElemOfNewPhase->pCW, pDDB);

        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        if (pPrevElemOfNewPhase->Opc == SER_RCW_PHS)
        {
            //Special Treatment for RCW_PHS_List
            asic_pPrevCW |= SER10_PTR_LIST_HEAD_MARKER; //Bit 21: AcwPositionMarker
        }
        #endif

        EDDI_SERCommand(FCODE_INSERT_ACW_TX, asic_pPrevCW, asic_pNewCW, 0, pDDB);

        //Change Previous ACW of Old Phase --> unchains old ACW
        //(void)SERSet_asic_pNext(pPrevElemOfOldPhase->pCW, asic_pNextOfOldPhaseCW);

        asic_pPrevOfOldPhaseCW = DEV_kram_adr_to_asic_register(pPrevElemOfOldPhase->pCW, pDDB);

        #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
        if (pPrevElemOfOldPhase->Opc == SER_RCW_PHS)
        {
            // Special Treatment for RCW_PHS_List
            asic_pPrevOfOldPhaseCW |= SER10_PTR_LIST_HEAD_MARKER; //Bit 21: AcwPositionMarker
        }
        #endif

        EDDI_SERAsyncCmd(FCODE_REMOVE_ACW_TX,
                         asic_pPrevOfOldPhaseCW,
                         asic_pNextOfOldPhaseCW,
                         pDDB,
                         pRQB, 
                         LSA_TRUE /*bLock*/);

        //Store Context for Part2
        pDDB->CRT.ContextProvChPhase.InUse   = LSA_TRUE;
        pDDB->CRT.ContextProvChPhase.pOldAcw = pOldAcw;

        //Update New Pathload
        //allow overplaning for AUX-providers
        if  (EDDI_RTC3_AUX_PROVIDER != pLowerParams->ProviderType)
        {
            EDDI_RedTreeUpdatePathLoad(pTree,
                                       pLowerParams->CycleReductionRatio,
                                       NewCyclePhase,
                                       (LSA_INT32)pLowerParams->DataLen);
        }

        //Update ACWptr for passivate
        pProvider->pCWStored = pProvider->pLowerCtrlACW->pCW;

        *pIndicate = LSA_FALSE; //EddReequestFinish will be called asynchronously (in CmdDone-Context)
    }

    return EDD_STS_OK;
}
//temporarily_disabled_lint -restore
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeSrtProviderChPhasePart2()            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderChPhasePart2( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeSrtProviderChPhasePart2->");

    //Restore Context
    if (!(pDDB->CRT.ContextProvChPhase.InUse) ||
        (pDDB->CRT.ContextProvChPhase.pOldAcw == 0))
    {
        EDDI_Excp("EDDI_RedTreeSrtProviderChPhasePart2, no valid ContextProvChPhase found", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    //Recycle old ACW
    EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.AcwProvider, (void *)pDDB->CRT.ContextProvChPhase.pOldAcw);

    //CleanUp Context
    pDDB->CRT.ContextProvChPhase.InUse   = LSA_FALSE;
    pDDB->CRT.ContextProvChPhase.pOldAcw = 0;

    EDDI_RequestFinish((EDDI_LOCAL_HDB_PTR_TYPE)pRQB->internal_context, pRQB, EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //!defined(EDDI_INTCFG_PROV_BUFFER_IF_3BSW)


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeSrtProviderACWRemovePart1()          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderACWRemovePart1( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                      EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                                      EDDI_TREE_ELEM_PTR_TYPE  const  pDelElemHandle )
{
    LSA_UINT32  asic_pPrevCW;
    LSA_UINT32  asic_pDelCW;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeSrtProviderACWRemovePart1->");

    asic_pPrevCW = DEV_kram_adr_to_asic_register(pDelElemHandle->pPrevElem->pCW, pDDB);
    asic_pDelCW  = DEV_kram_adr_to_asic_register(pDelElemHandle->pCW, pDDB);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (pDelElemHandle->pPrevElem->Opc == SER_RCW_PHS)
    {
        //Special Treatment for RCW_PHS_List
        asic_pPrevCW |= SER10_PTR_LIST_HEAD_MARKER; //Bit 21: AcwPositionMarker
    }
    #endif

    EDDI_SERAsyncCmd(FCODE_REMOVE_ACW_TX, asic_pPrevCW, asic_pDelCW, pDDB, pRQB, LSA_TRUE /*bLock*/);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeSrtProviderRemovePart2()             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeSrtProviderRemovePart2( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                   EDDI_TREE_ELEM_PTR_TYPE                 pDelElemHandle,
                                                             const EDDI_CRT_PROVIDER_PARAM_TYPE  *  const  pLowerParams,
                                                                   LSA_BOOL                         const  bFreeACW)
{
    EDDI_TREE_TYPE  *  const  pTree = &pDDB->CRT.SrtRedTree;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeSrtProviderRemovePart2->");

    if (bFreeACW)
    {
        EDDI_MemFreePoolBuffer(pDDB->CRT.hPool.AcwProvider, (EDDI_DEV_MEM_PTR_TYPE)(pDelElemHandle->pCW));
    }

    EDDI_RedTreeUnChainElem(pDelElemHandle);

    //free TreeElem
    EDDI_RedTreePushTxRxElem(pTree, pDelElemHandle);

    //allow overplaning for AUX-providers
    if (EDDI_RTC3_AUX_PROVIDER != pLowerParams->ProviderType)
    {
        EDDI_RedTreeUpdatePathLoad(pTree,
                                   pLowerParams->CycleReductionRatio,
                                   pLowerParams->CyclePhase,
                                   (-1L) * pLowerParams->DataLen);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_ser_tree_usr.c                                          */
/*****************************************************************************/




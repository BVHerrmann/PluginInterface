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
/*  F i l e               &F: eddi_ser_tree.c                           :F&  */
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

#define EDDI_MODULE_ID     M_ID_SER_TREE
#define LTRC_ACT_MODUL_ID  304

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define EDDI_TREE_INVALID_ELEM_PTR ((EDDI_TREE_ELEM_PTR_TYPE)0xFFFFFFFFUL)  //this pointer must be used as a return value after an exception has been thrown!

static EDDI_TREE_ELEM_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_RedTreeGetBranchHead( EDDI_TREE_PTR_TYPE const pTree,
                                                                              LSA_UINT16         const CycleReduction,
                                                                              LSA_UINT16         const CyclePhase );

static EDDI_TREE_ELEM_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_RedTreeGetBranchTail( EDDI_TREE_PTR_TYPE const pTree,
                                                                              LSA_UINT16         const CycleReduction,
                                                                              LSA_UINT16         const CyclePhase );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeCloseBasicElements( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                       EDDI_TREE_PTR_TYPE           const  pTree );


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                 EDDI_TREE_TYPE           *  const  pTree,
                                                 EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                 LSA_UINT32                  const  CycleBaseFactor,
                                                 LSA_UINT32                  const  CfgMaxReduction,
                                                 LSA_UINT32                  const  TxRxElementCnt )
{
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeInit");

    #if defined (EDDI_CFG_REV5)
    EDDI_RedTreeBcwModInit(pDDB, pTree, RtClassType, CycleBaseFactor, CfgMaxReduction, TxRxElementCnt); 
    #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
   
    EDDI_RedTreeRcwInit(pDDB, pTree, RtClassType, CycleBaseFactor, CfgMaxReduction, TxRxElementCnt);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeBuild()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBuild( EDDI_DDB_TYPE                   *  const  pDDB,
                                                  EDDI_TREE_TYPE                  *  const  pTree,
                                                  LSA_UINT32                         const  CycleBaseFactor,
                                                  EDDI_SER_CCW_PTR_TYPE              const  pRootCw,
                                                  EDDI_LOCAL_EOL_PTR_TYPE  const  *  const  ppEOLn )
{
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBuild");

    #if defined (EDDI_CFG_REV5)
    RedTreeBcwModBuild(pDDB,
                       pTree,
                       CycleBaseFactor,
                       pRootCw,
                       ppEOLn);
    #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_RedTreeRcwBuild(pDDB,
                         pTree,
                         CycleBaseFactor,
                         pRootCw,
                         ppEOLn);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeClose( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                  EDDI_TREE_TYPE            *  const  pTree )
{
    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeClose");

    EDDI_RedTreeCloseBasicElements(pDDB, pTree);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_RedTreeRcwClose(pDDB, pTree);
    #else
    EDDI_RedTreeBcwModClose(pDDB, pTree);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeCloseBasicElements( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       EDDI_TREE_PTR_TYPE       const  pTree )
{
    LSA_UINT16  ret16_val;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeCloseBasicElements->");

    EDDI_FREE_LOCAL_MEM(&ret16_val, pTree->pHeadElemArray);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_RedTreeCloseBasicElements, pTree->pHeadElemArray", EDDI_FATAL_ERR_EXCP, ret16_val, pDDB);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret16_val, pTree->pTailElemArray);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_RedTreeCloseBasicElements, pTree->pTailElemArray", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret16_val, pTree->pTxRxElemArray);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_RedTreeCloseBasicElements, pTree->pTxRxElemArray", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
        return;
    }

    EDDI_FREE_LOCAL_MEM(&ret16_val, pTree->pPathLoadArray);
    if (ret16_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_RedTreeCloseBasicElements, pTree->pPathLoadArray", EDDI_FATAL_ERR_EXCP, ret16_val, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeChainElem()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeChainElem( EDDI_TREE_ELEM_PTR_TYPE  const  pPrevElem,
                                                      EDDI_TREE_ELEM_PTR_TYPE  const  pNewElem )
{
    if (   (pPrevElem == EDDI_NULL_PTR)
        || (pNewElem == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_RedTreeChainElem, Error with EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    else
    {
        EDDI_TREE_ELEM_PTR_TYPE  const  pNextElem = pPrevElem->pNextElem; //remember next Element

        pPrevElem->pNextElem = pNewElem;
        pNewElem->pPrevElem  = pPrevElem;

        pNewElem->pNextElem  = pNextElem;

        if (pNextElem != EDDI_NULL_PTR)
        {
            pNextElem->pPrevElem = pNewElem;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeUnChainElem()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeUnChainElem( EDDI_TREE_ELEM_PTR_TYPE  const  pActElem )
{
    if (   (pActElem            == EDDI_NULL_PTR)
        || (pActElem->pPrevElem == EDDI_NULL_PTR)
        || (pActElem->pNextElem == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_RedTreeUnChainElem, Error with EDDI_NULL_PTR", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pActElem->pPrevElem->pNextElem = pActElem->pNextElem;
    pActElem->pNextElem->pPrevElem = pActElem->pPrevElem;

    pActElem->pNextElem = EDDI_NULL_PTR;
    pActElem->pPrevElem = EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetBranchHead( EDDI_TREE_PTR_TYPE  const  pTree,
                                                                                 LSA_UINT16          const  CycleReduction,
                                                                                 LSA_UINT16          const  CyclePhase )
{
    LSA_UINT32  index;

    if ((CycleReduction < 1)                      ||
        (CycleReduction > pTree->MaxReduction)    ||
        (CyclePhase == 0 )                        ||
        (CyclePhase > CycleReduction ))
    {
        EDDI_Excp("EDDI_RedTreeGetBranchHead", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDDI_TREE_INVALID_ELEM_PTR;
    }

    index = RedGetQuickBinaryIndex(CycleReduction, CyclePhase);

    return (&pTree->pHeadElemArray[index]);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeGetBranchTail()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetBranchTail( EDDI_TREE_PTR_TYPE  const  pTree,
                                                                                        LSA_UINT16          const  CycleReduction,
                                                                                        LSA_UINT16          const  CyclePhase )
{
    LSA_UINT32  Index;

    if (   (CycleReduction < 1)
        || (CycleReduction > pTree->MaxReduction)
        || (CyclePhase == 0)
        || (CyclePhase > CycleReduction))
    {
        EDDI_Excp("EDDI_RedTreeGetBranchTail", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDDI_TREE_INVALID_ELEM_PTR;
    }

    Index = RedGetQuickBinaryIndex(CycleReduction, CyclePhase);

    return &pTree->pTailElemArray[Index];
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreePopTxRxElem()                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreePopTxRxElem( EDDI_TREE_PTR_TYPE  const  pTree )
{
    EDDI_TREE_ELEM_PTR_TYPE  pCurrentElem;
    LSA_UINT32               Ctr;

    if (pTree->TxRxElemCountUsed < pTree->TxRxElemCountMax)
    {
        for (Ctr = 0; Ctr < pTree->TxRxElemCountMax; Ctr++)
        {
            //to speed up searchtime we start searching at Element Nr == TxRxElemCountUsed
            pCurrentElem = &pTree->pTxRxElemArray[(Ctr + pTree->TxRxElemCountUsed) % pTree->TxRxElemCountMax];
            if (pCurrentElem->Opc == EDDI_TREE_ELEM_OPC_FREE)
            {
                pCurrentElem->Opc = EDDI_TREE_ELEM_OPC_USED;
                pTree->TxRxElemCountUsed++;
                return pCurrentElem;
            }
        }
    }

    EDDI_Excp("EDDI_RedTreePopTxRxElem failed", EDDI_FATAL_ERR_EXCP, 0, 0);
    return EDDI_TREE_INVALID_ELEM_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreePushTxRxElem( EDDI_TREE_PTR_TYPE       const  pTree,
                                                         EDDI_TREE_ELEM_PTR_TYPE  const  pCurrentElem )
{
    if (pTree->TxRxElemCountUsed)
    {
        EDDI_MemSet(pCurrentElem, (LSA_UINT8)0UL, (LSA_UINT32)sizeof(EDDI_TREE_ELEM_TYPE));
        pCurrentElem->Opc = EDDI_TREE_ELEM_OPC_FREE;
        pTree->TxRxElemCountUsed --;
    }
    else
    {
        EDDI_Excp("EDDI_RedTreePushTxRxElem failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeGetPrevElem()                        */
/*                                                                         */
/* D e s c r i p t i o n: search for right insert-position in ACW-TX-list  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetPrevElem( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       EDDI_TREE_PTR_TYPE       const  pTree,
                                                                       LSA_UINT16               const  CycleReductionRatio,
                                                                       LSA_UINT16               const  CyclePhase,
                                                                       LSA_UINT32               const  CyclePhaseSequence )
{
    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    if (CyclePhaseSequence == EDDI_TREE_GET_LAST_OF_SEQ)
    {
        //get Tail of related Branch
        EDDI_TREE_ELEM_PTR_TYPE  const  pTailElem = EDDI_RedTreeGetBranchTail(pTree, CycleReductionRatio, CyclePhase);

        //search last Element in PhaseBranch (get Element before Tail)
        LSA_UNUSED_ARG(pDDB);
        return pTailElem->pPrevElem;
    }
    else
    {
        //get Head of related Branch
        EDDI_TREE_ELEM_PTR_TYPE         pPrevElem      = EDDI_RedTreeGetBranchHead(pTree, CycleReductionRatio, CyclePhase);
        LSA_UINT32               const  NewSequenceNr  = (LSA_UINT32)(CyclePhaseSequence - 1);
        LSA_UINT32                      LastSequenceNr = (LSA_UINT32)EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
        LSA_UINT32                      NextSequenceNr;
        EDDI_TREE_ELEM_PTR_TYPE         pNextElem;

        for (;;)
        {
            pNextElem      = pPrevElem->pNextElem;
            NextSequenceNr = pNextElem->SequenceNr;

            //ATTENTION: all Tail-Elements must have the SequenceNr = 0xFFFFFFFF!

            //all elements are lined up with ascending SequenceNr.
            //search the element with the next higher SequenceNr.
            if (NewSequenceNr < NextSequenceNr)
            {
                if (NewSequenceNr == LastSequenceNr)
                {
                    //SeqNr already exists, add AFTER the last frame with the same SeqNr
                    EDDI_FUNCTION_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_RedTreeGetPrevElem, new SequenceNr 0x%X already exist, adding before SequenceNr 0x%X",
                                           NewSequenceNr, NextSequenceNr);
                }

                return pPrevElem;
            }

            LastSequenceNr = NextSequenceNr;

            //move one step forward
            pPrevElem = pNextElem;
        }
    }
    #else
    EDDI_TREE_ELEM_PTR_TYPE  pPrevElem;
    EDDI_TREE_ELEM_PTR_TYPE  pTailElem;
    LSA_UINT32               Ctr;

    //get Tail of related Branch
    pTailElem = EDDI_RedTreeGetBranchTail(pTree, CycleReductionRatio, CyclePhase);

    if (CyclePhaseSequence == EDDI_TREE_GET_LAST_OF_SEQ)
    {
        //search last Element in PhaseBranch (get Element before Tail)
        return pTailElem->pPrevElem;
    }

    //get Head of related Branch
    pPrevElem = EDDI_RedTreeGetBranchHead(pTree, CycleReductionRatio, CyclePhase);

    //search Element with specified SequenceNr in PhaseBranch
    for (Ctr = 1; Ctr < CyclePhaseSequence; Ctr++)
    {
        if (pPrevElem->pNextElem == pTailElem)
        {
            break;
        }
        pPrevElem = pPrevElem->pNextElem; //move one step forward
    }
    LSA_UNUSED_ARG(pDDB);
    return pPrevElem;
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetPrevElemByTime( EDDI_TREE_PTR_TYPE  const  pTree,
                                                                             LSA_UINT16          const  CycleReductionRatio,
                                                                             LSA_UINT16          const  CyclePhase,
                                                                             LSA_UINT32          const  Time_10ns )
{
    EDDI_TREE_ELEM_PTR_TYPE  pPrevElem;
    EDDI_TREE_ELEM_PTR_TYPE  pTailElem;
    LSA_UINT32               Ctr;

    //get Tail of related Branch
    pTailElem = EDDI_RedTreeGetBranchTail(pTree, CycleReductionRatio, CyclePhase);

    //search Element with specified SequenceNr in PhaseBranch
    pPrevElem = EDDI_RedTreeGetBranchHead(pTree, CycleReductionRatio, CyclePhase);

    for (Ctr = 1; Ctr <= 1000; Ctr++)
    {
        if (Ctr == 1000)
        {
            EDDI_Excp("EDDI_RedTreeGetPrevElemByTime only supported for EDDI_TREE_REDUCTION_MODE_BINARY; ReductionMode,-",
                      EDDI_FATAL_ERR_EXCP, CycleReductionRatio, CyclePhase);
            return pPrevElem;
        }

        if (pPrevElem->pNextElem == pTailElem)
        {
            //End of Branch reached
            break;
        }

        if (pPrevElem->pNextElem->Sub.TxRx.Time > Time_10ns)
        {
            //Time of next Element too big
            break;
        }

        pPrevElem = pPrevElem->pNextElem; //move one step forward
    }

    return pPrevElem;
}
/*---------------------- end [subroutine] ---------------------------------*/


/**************************************************************************
* F u n c t i o n:       EDDI_RedTreeUpdatePathLoad()
*
* D e s c r i p t i o n: Updates the Load-Info of all Pathes which belong
*                        to the (Sub-)Branch which is specified by
*                        CycleReductionRatio and CyclePhase.
*
* A r g u m e n t s:
*
* Return Value:          LSA_VOID
*
***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeUpdatePathLoad( EDDI_TREE_PTR_TYPE  const  pTree,
                                                           LSA_UINT16          const  CycleReductionRatio,
                                                           LSA_UINT16          const  CyclePhase,
                                                           LSA_INT32           const  DeltaLoad )
{
    LSA_UINT32                    Ctr;
    EDDI_TREE_PATH_LOAD_PTR_TYPE  pEntry;

    if (pTree->ReductionMode != EDDI_TREE_REDUCTION_MODE_BINARY)
    {
        EDDI_Excp("EDDI_RedTreeUpdatePathLoad only supported for EDDI_TREE_REDUCTION_MODE_BINARY; ReductionMode,-", EDDI_FATAL_ERR_EXCP, pTree->ReductionMode, 0);
        return;
    }

    /*
    Algorithm for calculating the indices of PathLoadInfos to be updated
    Example: Branch 2.1, pTree->MaxReduction = 8;

    CycleRedRatio = 2
    Phase         = 1
    MaxReduction  = 8

    for (Ctr = Phase - 1; Ctr < MaxReduction; Ctr+=CycleRedRatio)
    {
       PathLoadInfo[Ctr] += DataLen;
    }
    --> Contains indeces: 0 (=Phase 1)
                          2 (=Phase 3)
                          4 (=Phase 5)
                          7 (=Phase 7)
    */

    for (Ctr = CyclePhase - 1; Ctr < pTree->MaxReduction; Ctr += CycleReductionRatio)
    {
        pEntry = &pTree->pPathLoadArray[Ctr];

        pEntry->ByteCounter = (LSA_UINT32)((LSA_INT32)(pEntry->ByteCounter) + DeltaLoad);

        if (DeltaLoad > 0)
        {
            pEntry->ProvCounter++;
            pEntry->ByteCounter += EDDI_CRT_PROVIDER_HEADER_OVERHEAD; 
        }
        else
        {
            pEntry->ProvCounter--;
            pEntry->ByteCounter -= EDDI_CRT_PROVIDER_HEADER_OVERHEAD; 
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n: EDDI_RedTreeGetMaxPathLoad()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:    Returns the Maximum Load of all Pathes which are using */
/*                  the Branch with the specified by CycleReductionRatio   */
/*                  and CyclePhase.                                        */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetMaxPathLoad( EDDI_TREE_PTR_TYPE  const  pTree,
                                                           LSA_UINT16          const  CycleReductionRatio,
                                                           LSA_UINT16          const  CyclePhase,
                                                           LSA_UINT32              *  pMaxByteCounter,  //OUT
                                                           LSA_UINT32              *  pMaxProvCounter ) //OUT
{
    LSA_UINT32  MaxByteCounter = 0;
    LSA_UINT32  MaxProvCounter = 0;
    LSA_UINT32  Ctr;

    if (pTree->ReductionMode != EDDI_TREE_REDUCTION_MODE_BINARY)
    {
        EDDI_Excp("EDDI_RedTreeGetMaxPathLoad only supported for EDDI_TREE_REDUCTION_MODE_BINARY; ReductionMode,-", EDDI_FATAL_ERR_EXCP, pTree->ReductionMode, 0);
        return;
    }

    for (Ctr = CyclePhase - 1; Ctr < pTree->MaxReduction; Ctr += CycleReductionRatio)
    {
        if (pTree->pPathLoadArray[Ctr].ByteCounter > MaxByteCounter)
        {
            MaxByteCounter = pTree->pPathLoadArray[Ctr].ByteCounter;
        }

        if (pTree->pPathLoadArray[Ctr].ProvCounter > MaxProvCounter)
        {
            MaxProvCounter = pTree->pPathLoadArray[Ctr].ProvCounter;
        }
    }

    *pMaxByteCounter = MaxByteCounter + EDDI_CRT_PROVIDER_HEADER_OVERHEAD; //take overhead of next frame into account
    *pMaxProvCounter = MaxProvCounter;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedIsBinaryValue()                          */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_BOOL                                         */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_RedIsBinaryValue( LSA_UINT32  const  Value )
{
    LSA_UINT32  Ctr;

    for (Ctr = 0; Ctr < 32UL; Ctr++)
    {
        if (((LSA_UINT32)((LSA_UINT32)1 << Ctr)) == Value)
        {
            return LSA_TRUE;
        }
    }
    return LSA_FALSE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeIrtInsertTxEol()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:     CyclePhase > 0!                                  */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeIrtInsertTxEol( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                           EDDI_TREE_TYPE           *  const  pTxTree,
                                                           EDDI_LOCAL_EOL_PTR_TYPE     const  pEOL,
                                                           LSA_UINT16                  const  CyclePhase )
{
    LSA_UINT32               const  MaxReduction = pTxTree->MaxReduction;
    EDDI_TREE_ELEM_PTR_TYPE  const  pTailElem    = EDDI_RedTreeGetBranchTail(pTxTree, (LSA_UINT16)MaxReduction, CyclePhase);

    #if defined (EDDI_CFG_REV5)
    {
        EDDI_TREE_ELEM_PTR_TYPE  const  pTailPrevElem = pTailElem->pPrevElem;

        //previous element in BCW-Tree is a Root-NOOP or BCW
        EDDI_SERConnectCwToNextDyn(pDDB, pTailPrevElem->pCW, (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pEOL, pTailPrevElem->bCwBranchLinking);
    }
    #elif defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    if (MaxReduction == 1)
    {
        //previous element in RCW-Tree is a Root-NOOP
        EDDI_SERConnectCwToNext(pDDB, pTailElem->pPrevElem->pCW, (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pEOL);
    }
    else //MaxReduction > 1
    {
        //previous element in RCW-Tree is a RCW
        EDDI_SERConnectCwToRcwPhase(pDDB, (EDDI_LOCAL_LIST_HEAD_PTR_TYPE)(LSA_VOID *)pTailElem->pPrevElem->pCW, (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pEOL);
    }
    #endif

    pTailElem->pCW = (EDDI_SER_CCW_PTR_TYPE)(void *)pEOL;
    if (pEOL != EDDI_NULL_PTR)
    {
        pTailElem->Opc = SER_EOL;
    }
    else
    {
        pTailElem->Opc = EDDI_TREE_ELEM_OPC_EMPTY;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBuildBranch( LSA_UINT32                  const  HeadOpc,
                                                        EDDI_TREE_ELEM_PTR_TYPE     const  pHeadElem,
                                                        EDDI_SER_CCW_PTR_TYPE       const  pHeadCw,
                                                        EDDI_TREE_ELEM_PTR_TYPE     const  pTailElem,
                                                        EDDI_SER_CCW_PTR_TYPE       const  pTailCw,
                                                        EDDI_DDB_TYPE            *  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBuildBranch");

    /*if( (pHeadElem->pCW       != 0) || (pTailElem->pCW       != 0) ||
          (pHeadElem->pPrevElem != 0) || (pTailElem->pPrevElem != 0) ||
          (pHeadElem->pNextElem != 0) || (pTailElem->pNextElem != 0) )
      {
        EDDI_Excp("EDDI_RedTreeBuildBranch EDDI_TREE_ELEMENT already in Use ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, pTailElem->pCW);
        return;
      } */

    if ((pHeadElem->pCW != 0) || (pTailElem->pCW != 0))
    {
        EDDI_Excp("EDDI_RedTreeBuildBranch, EDDI_TREE_ELEMENT already in Use ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, pTailElem->pCW);
        return;
    }

    /*
    if (pHeadCw == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeBuildBranch pHeadCw == EDDI_NULL_PTR ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, 0);
        return;
    }
    */

    //Init Elements
    pHeadElem->Opc = HeadOpc;
    pHeadElem->pCW = pHeadCw;

    pTailElem->Opc = SER_RCW;
    pTailElem->pCW = pTailCw;

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //special initialization of all Tail-Elements necessary for EDDI_RedTreeGetPrevElem()!
    pTailElem->SequenceNr = (LSA_UINT32)EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
    #endif

    //Connect Elements
    EDDI_RedTreeChainElem(pHeadElem, pTailElem);

    //Connect CWs
    if (pHeadElem->Opc == SER_RCW_PHS)
    {
        EDDI_SERConnectCwToRcwPhase(pDDB, &pHeadElem->pCW->ListHead, pTailElem->pCW);
    }
    else
    {
        EDDI_SERConnectCwToNext(pDDB, pHeadElem->pCW, pTailElem->pCW);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeBuildBranchRCWPHS()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBuildBranchRCWPHS( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              EDDI_TREE_ELEM_PTR_TYPE  const  pHeadElem,
                                                              EDDI_SER_CCW_PTR_TYPE    const  pHeadCw,
                                                              EDDI_TREE_ELEM_PTR_TYPE  const  pTailElem )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBuildBranchRCWPHS->");

    /*if( (pHeadElem->pCW       != 0) || (pTailElem->pCW       != 0) ||
          (pHeadElem->pPrevElem != 0) || (pTailElem->pPrevElem != 0) ||
          (pHeadElem->pNextElem != 0) || (pTailElem->pNextElem != 0) )
      {
        EDDI_Excp("EDDI_RedTreeBuildBranchRCWPHS, EDDI_TREE_ELEMENT already in Use ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, pTailElem->pCW);
        return;
      } */

    /*if ((pHeadElem->pCW != 0) || (pTailElem->pCW != 0))
      {
        EDDI_Excp("EDDI_RedTreeBuildBranchRCWPHS, EDDI_TREE_ELEMENT already in Use ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, pTailElem->pCW);
        return;
      }

      if (pHeadCw == EDDI_NULL_PTR)
      {
        EDDI_Excp("EDDI_RedTreeBuildBranchRCWPHS, pHeadCw == EDDI_NULL_PTR ", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW, 0);
        return;
      }  */

    //Init Elements:
    pHeadElem->Opc = SER_RCW_PHS;
    pHeadElem->pCW = pHeadCw;

    pTailElem->Opc = SER_RCW;
    pTailElem->pCW = EDDI_NULL_PTR;

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //special initialization of all Tail-Elements necessary for EDDI_RedTreeGetPrevElem()!
    pTailElem->SequenceNr = (LSA_UINT32)EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
    #endif

    //Connect Elements
    EDDI_RedTreeChainElem(pHeadElem, pTailElem);

    //Connect CWs -> nicht notwendig mit 0xFFFFFFFF initialisiert
    /* EDDI_SERConnectCwToRcwPhase( pDDB, &pHeadElem->pCW->ListHead, pTailElem->pCW);
      if (pHeadElem->pCW->ListHead.Value != 0xFFFFFFFFUL)
      {
        EDDI_Excp("EDDI_RedTreeBuildBranchRCWPHS, pHeadElem->pCW->ListHead.Value", EDDI_FATAL_ERR_EXCP, pHeadElem->pCW->ListHead.Value, 0);

      }
      pHeadElem->pCW->ListHead.Value = EDDI_HOST2IRTE32(SER10_NULL_PTR);  */
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_REV6 || EDDI_CFG_REV7


/*****************************************************************************/
/*  end of file eddi_ser_tree.c                                              */
/*****************************************************************************/




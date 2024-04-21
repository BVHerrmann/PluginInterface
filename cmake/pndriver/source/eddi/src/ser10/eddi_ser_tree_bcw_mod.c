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
/*  F i l e               &F: eddi_ser_tree_bcw_mod.c                   :F&  */
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

#if defined (M_ID_SER_TREE_BCW_MOD) //satisfy lint!
#endif

#include "eddi_int.h"
   
#define EDDI_MODULE_ID     M_ID_SER_TREE_BCW_MOD
#define LTRC_ACT_MODUL_ID  307

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (EDDI_CFG_REV5)

#include "eddi_dev.h"
#include "eddi_ser_ext.h"

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR RedTreeGetModMask( LSA_UINT32 const CycleBaseFactor,
                                                         LSA_UINT32 const CurrentReduction );

static EDDI_TREE_ELEM_PTR_TYPE EDDI_LOCAL_FCT_ATTR EDDI_RedTreeInitBranchWithBCWMod( EDDI_DDB_TYPE           * const pDDB,
                                                                                     EDDI_TREE_PTR_TYPE        const pTree,
                                                                                     EDDI_TREE_ELEM_PTR_TYPE   const pParentElement,
                                                                                     LSA_UINT32                const CurrentReduction,
                                                                                     LSA_UINT32                const CurrentPhase,
                                                                                     LSA_UINT32                const CycleBaseFactor );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeInitBasicElements( EDDI_LOCAL_DDB_PTR_TYPE   const pDDB, 
                                                                   EDDI_TREE_TYPE          * const pTree,
                                                                   LSA_UINT32                const BranchCnt,
                                                                   LSA_UINT32                const CntTxRxElements,
                                                                   LSA_BOOL                  const bCreatePathload );


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBcwModInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_TREE_TYPE           *  const  pTree,
                                                       EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                       LSA_UINT32                  const  CycleBaseFactor,
                                                       LSA_UINT32                  const  CfgMaxReduction,
                                                       LSA_UINT32                  const  TxRxElementCnt )
{
    LSA_UINT32  MaxReduction;
    LSA_UINT32  CntNOOPs;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBcwModInit->");

    if (!EDDI_RedIsBinaryValue((LSA_UINT32)CycleBaseFactor))
    {
        //Special case for MC: a non-binary CycleBaseFactor doesnt allow a SRT-Tree with BCW-MODS
        //                     -> Set MaxReduction = 1 !!
        MaxReduction = 1;
    }
    else if ((CycleBaseFactor * CfgMaxReduction) > 0x8000) //CycleMask BitPos 18..4-> 2^15
    {
        //Ab CycleBaseFactor = 128
        //To avoid overload of ModMasks in BCW_MOD-Entries we must limit the MaxReduction
        MaxReduction = 0x8000 / CycleBaseFactor;
    }
    else
    {
        //OK: we can use the needed Reduction
        MaxReduction = CfgMaxReduction;
    }

    if (   (MaxReduction == 0)
        || (MaxReduction  > EDDI_TREE_MAX_BINARY_REDUCTION))
    {
        EDDI_Excp("EDDI_RedTreeBcwModInit, MaxReduction iswrong", EDDI_FATAL_ERR_EXCP, MaxReduction, pDDB);
        return;
    }

    //build a binary tree: part 1
    pTree->RtClassType   = RtClassType;
    pTree->ReductionMode = EDDI_TREE_REDUCTION_MODE_BINARY;
    pTree->MaxReduction  = MaxReduction;

    //We must use fixed Values of BranchCnt, CntBCWs, CntNOOPs here, to provide
    //enough memory for Worstcase-Allocations!
    pTree->BcwMod.BranchCnt = (CfgMaxReduction * 2) - 1; // (MaxReduction * 2 - 1);
    pTree->BcwMod.CntBCWs   = (CfgMaxReduction - 1);     // (MaxReduction - 1);
    if (RtClassType != EDDI_TREE_RT_CLASS_ACW_TX) //EDDI_TREE_RT_CLASS_FCW_RX || EDDI_TREE_RT_CLASS_FCW_TX
    {
        CntNOOPs = 0;
    }
    else
    {
        CntNOOPs = CfgMaxReduction - 1;     // (MaxReduction - 1);
    }
    pTree->BcwMod.CntNOOPs = CntNOOPs;

    pTree->BcwMod.SizeBCWs  = sizeof(EDDI_SER10_BCW_MOD_TYPE) * pTree->BcwMod.CntBCWs;
    pTree->BcwMod.SizeNOOPs = sizeof(EDDI_SER10_NOOP_TYPE)    * CntNOOPs;

    pTree->BcwMod.TxRxElementCnt = TxRxElementCnt;

    pTree->KramSize = pTree->BcwMod.SizeBCWs + pTree->BcwMod.SizeNOOPs;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:         RedTreeBcwModBuild()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:            LSA_VOID                                       */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  RedTreeBcwModBuild( EDDI_DDB_TYPE                   *  const  pDDB,
                                                   EDDI_TREE_TYPE                  *  const  pTree,
                                                   LSA_UINT32                         const  CycleBaseFactor,
                                                   EDDI_SER_CCW_PTR_TYPE              const  pRootCw,
                                                   EDDI_LOCAL_EOL_PTR_TYPE  const  *  const  ppEOLn )
{
    LSA_UINT32  Ret;
    LSA_UINT32  EOLCount, EOLCountMax;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "RedTreeBcwModBuild");

    //build a binary tree: part 2
    pTree->pRootCw = pRootCw;  //Only Store Reference to RootCw (RootCw must already have been allocated outside this function)

    pTree->pEOLMax = EDDI_NULL_PTR;

    switch (pTree->RtClassType)
    {
        case EDDI_TREE_RT_CLASS_FCW_TX:
        {
            EOLCountMax = EDDI_FCW_TX_EOL_CNT;
            //pEOLMax is set later in EDDI_SyncIrInitAllTimeElements()!
            break;
        }
        case EDDI_TREE_RT_CLASS_FCW_RX:
        case EDDI_TREE_RT_CLASS_ACW_TX:
        default:
        {
            EOLCountMax = 1;
            break;
        }
    }

    //init valid EOLs
    for (EOLCount = 0; EOLCount < EOLCountMax; EOLCount++)
    {
        pTree->pEOLn[EOLCount] = ppEOLn[EOLCount];  //Only Store Reference to EOLn (EOLn must already have been allocated outside this function)
    }

    //reset invalid EOLs
    for (; EOLCount < EDDI_FCW_TX_EOL_CNT; EOLCount++)
    {
        pTree->pEOLn[EOLCount] = EDDI_NULL_PTR;
    }

    EDDI_RedTreeInitBasicElements(pDDB, pTree, pTree->BcwMod.BranchCnt, pTree->BcwMod.TxRxElementCnt, LSA_TRUE);

    //Allocate Buffer in KRAM for BCW_MOD-Entries
    if (pTree->BcwMod.CntBCWs)
    {
        Ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pTree->BcwMod.pBCWArrayMemHeader, pTree->BcwMod.SizeBCWs);

        if (Ret != EDDI_MEM_RET_OK)
        {
            if (Ret == EDDI_MEM_RET_NO_FREE_MEMORY)
            {
                EDDI_Excp("RedTreeBcwModBuild5 - Not enough free KRAM !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->BcwMod.SizeBCWs);
                return;
            }
            else
            {
                EDDI_Excp("RedTreeBcwModBuild5 - EDDI_MEMGetBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->BcwMod.SizeBCWs);
                return;
            }
        }

        pDDB->KramRes.pool.bcw_mod += pTree->BcwMod.SizeBCWs;
        pTree->BcwMod.pBCWArray     = (EDDI_LOCAL_BCW_MOD_PTR_TYPE)(void *)pTree->BcwMod.pBCWArrayMemHeader->pKRam;

        //EDDI_MemSet(pTree->BcwMod.pBCWArray, (LSA_UINT8)0, Size);
    }
    else
    {
        pTree->BcwMod.pBCWArray = EDDI_NULL_PTR;
    }

    //Allocate Buffer in KRAM for NOOP-Entries
    if (pTree->BcwMod.CntNOOPs)
    {
        Ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pTree->BcwMod.pNOOPArrayMemHeader, pTree->BcwMod.SizeNOOPs);

        if (Ret != EDDI_MEM_RET_OK)
        {
            if (Ret == EDDI_MEM_RET_NO_FREE_MEMORY)
            {
                EDDI_Excp("RedTreeBcwModBuild6 - Not enough free KRAM !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->BcwMod.SizeNOOPs);
                return;
            }
            else
            {
                EDDI_Excp("RedTreeBcwModBuild6 - EDDI_MEMGetBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->BcwMod.SizeNOOPs);
                return;
            }
        }

        pTree->BcwMod.pNOOPArray = (EDDI_LOCAL_NOOP_PTR_TYPE)(void *)pTree->BcwMod.pNOOPArrayMemHeader->pKRam;

        //EDDI_MemSet(pTree->BcwMod.pNOOPArray, (LSA_UINT8)0, Size);

        pDDB->KramRes.pool.srt_noop = pTree->BcwMod.SizeNOOPs;
    }
    else
    {
        pTree->BcwMod.pNOOPArray = EDDI_NULL_PTR;
    }

    //Now recursively init and connect all Branch-Control-Structures and BCW_MOD-Entries
    pTree->pRootElem = EDDI_RedTreeInitBranchWithBCWMod(pDDB, pTree, EDDI_NULL_PTR, 1, 1, CycleBaseFactor);

    //Finally connect SerAcwHeader to first Entry in KRAM-Tree
    //But this will be done in Device-Setup-Call
    //with function EDDI_SERConnectCwToHeader(pDDB, pDDB->Glob.LLHandle.pACWDevBase->pTx, pTree->pRootElem->pCW);
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBcwModClose( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                        EDDI_TREE_TYPE            *  const  pTree )
{
    LSA_UINT32  Ret;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBcwModClose->");

    if (pTree->BcwMod.pBCWArrayMemHeader != EDDI_NULL_PTR)
    {
        Ret = EDDI_MEMFreeBuffer(pTree->BcwMod.pBCWArrayMemHeader);
        if (Ret != EDDI_MEM_RET_OK)
        {
            EDDI_Excp("EDDI_RedTreeBcwModClose, pTree->pBCWArray", EDDI_FATAL_ERR_EXCP, Ret, pDDB);
            return;
        }
        pTree->BcwMod.pBCWArrayMemHeader = EDDI_NULL_PTR;
    }

    if (pTree->BcwMod.pNOOPArrayMemHeader != EDDI_NULL_PTR)
    {
        Ret = EDDI_MEMFreeBuffer(pTree->BcwMod.pNOOPArrayMemHeader);
        if (Ret != EDDI_MEM_RET_OK)
        {
            EDDI_Excp("EDDI_RedTreeBcwModClose, pTree->pNOOPArray", EDDI_FATAL_ERR_EXCP, Ret, 0);
            return;
        }
        pTree->BcwMod.pNOOPArrayMemHeader = EDDI_NULL_PTR;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n: used for the following queues: FCW-TX            */
/*                                                       FCW-RX            */
/*                                                       ACW-TX            */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          pHeadElem (Begin of current branch)              */
/*                                                                         */
/***************************************************************************/
static  EDDI_TREE_ELEM_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeInitBranchWithBCWMod( EDDI_DDB_TYPE            *  const  pDDB,
                                                                                        EDDI_TREE_PTR_TYPE          const  pTree,
                                                                                        EDDI_TREE_ELEM_PTR_TYPE     const  pParentElem,
                                                                                        LSA_UINT32                  const  CurrentReduction,
                                                                                        LSA_UINT32                  const  CurrentPhase,
                                                                                        LSA_UINT32                  const  CycleBaseFactor )
{
    LSA_UINT32                   const  QuickIndex    = RedGetQuickBinaryIndex(CurrentReduction, CurrentPhase);
    LSA_UINT32                   const  NextReduction = CurrentReduction * 2;
    LSA_UINT32                          CurrentModMask;
    EDDI_LOCAL_BCW_MOD_PTR_TYPE         pCurrentBCW;
    EDDI_TREE_ELEM_PTR_TYPE             pHeadElem;  //Begin of current branch
    EDDI_TREE_ELEM_PTR_TYPE             pTailElem;  //End of current branch

    //get free Head-Element
    pHeadElem = &pTree->pHeadElemArray[QuickIndex];

    //Special treatment of pHeadElem to handle asymetrical use of NOOPs and RootCw in Trees
    if (CurrentReduction == 1)
    {
        //Start of Tree -> insert RootCw

        //Get RootCw
        pHeadElem->pCW = pTree->pRootCw;
        if (pTree->RtClassType == EDDI_TREE_RT_CLASS_ACW_TX)
        {
            pHeadElem->Opc = SER_ACW_TX; //Dummy-ACW
        }
        else //EDDI_TREE_RT_CLASS_FCW_RX || EDDI_TREE_RT_CLASS_FCW_TX
        {
            pHeadElem->Opc = SER_NOOP;
        }
        //pHeadElem->bCwBranchLinking = LSA_FALSE;  //already set!
    }
    else if (CurrentPhase <= (CurrentReduction / 2))
    {
        //Coming from pBranch

        if (pParentElem->Opc != SER_BCW_MOD)
        {
            EDDI_Excp("EDDI_RedTreeInitBranchWithBCWMod, pParentElem->Opc != SER_BCW_MOD", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, CurrentReduction);
            return 0;
        }

        if (pTree->RtClassType != EDDI_TREE_RT_CLASS_ACW_TX) //EDDI_TREE_RT_CLASS_FCW_RX || EDDI_TREE_RT_CLASS_FCW_TX
        {
            //Coming from pBranch -> no additional NOOP used
            //                    -> use Parent BCW instead
            //                    -> intelligent flexible connect-functions necessary!
            //                    -> no dynamic tree-entries allowed (Cw-pBranch-access is not consistent)!

            pHeadElem->pCW              = pParentElem->pCW;
            pHeadElem->Opc              = SER_BCW_MOD;
            pHeadElem->bCwBranchLinking = LSA_TRUE;
        }
        else //EDDI_TREE_RT_CLASS_ACW_TX
        {
            //Coming from pBranch -> insert an additional NOOP for linking!

            LSA_UINT32               const  QuickIndexForNoop = RedGetQuickBinaryIndex((CurrentReduction / 2UL), CurrentPhase);
            EDDI_LOCAL_NOOP_PTR_TYPE  const  pNoop             = &pTree->BcwMod.pNOOPArray[QuickIndexForNoop];

            pHeadElem->pCW                = (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pNoop;
            pHeadElem->Opc                = SER_NOOP;
            //pHeadElem->bCwBranchLinking = LSA_FALSE;  //already set!

            //initialize NOOP
            pNoop->Value.U32_0 = EDDI_HOST2IRTE32(SER_NOOP); //regular: EDDI_SERIniNOOP(pDDB, pNoop);
            pNoop->Value.U32_1 = EDDI_HOST2IRTE32(SER10_NULL_PTR_HELP);
        }
    }
    else
    {
        //Coming from pNext -> no NOOP needed
        //                  -> use Parent BCW instead

        if (pParentElem->Opc != SER_BCW_MOD)
        {
            EDDI_Excp("EDDI_RedTreeInitBranchWithBCWMod, pParentElem->Opc != SER_BCW_MOD", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, CurrentReduction);
            return 0;
        }

        pHeadElem->pCW                = pParentElem->pCW;
        pHeadElem->Opc                = SER_BCW_MOD;
        //pHeadElem->bCwBranchLinking = LSA_FALSE;  //already set!
    }

    pHeadElem->pPrevElem = pParentElem;

    pHeadElem->Sub.TxRx.Time    = 0;
    pHeadElem->Sub.TxRx.DataLen = 0;

    pTailElem = &pTree->pTailElemArray[QuickIndex];

    #if defined (EDDI_FIXED_ACW_PROVIDER_SEQUENCE)
    //special initialization of all Tail-Elements necessary for EDDI_RedTreeGetPrevElem()!
    pTailElem->SequenceNr = (LSA_UINT32)EDD_CYCLE_PHASE_SEQUENCE_UNDEFINED;
    #endif

    EDDI_RedTreeChainElem(pHeadElem, pTailElem);

    if (CurrentReduction >= pTree->MaxReduction)
    {
        switch (pTree->RtClassType)
        {
            case EDDI_TREE_RT_CLASS_FCW_TX:
            {
                //Connection with EOL is made later!
                //All Link-Pointers in pHeadElem->pCW (Root-NOOP or BCW) are already set to EDDI_NULL_PTR!
                pTailElem->Opc = EDDI_TREE_ELEM_OPC_EMPTY;
                //pTailElem->pCW = EDDI_NULL_PTR;  //already set!
                break;
            }
            case EDDI_TREE_RT_CLASS_FCW_RX: //Optimization possible by removing Dummy-EOL (pay attention when changes in red-shift-list are made!)
            case EDDI_TREE_RT_CLASS_ACW_TX:
            {
                EDDI_SER_CCW_PTR_TYPE  const  pEOL = (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pTree->pEOLn[0];

                //pHeadElem->pCW is a Root-NOOP or BCW (or Root-ACW?)
                EDDI_SERConnectCwToNextDyn(pDDB, pHeadElem->pCW, pEOL, pHeadElem->bCwBranchLinking);
                pTailElem->Opc = SER_EOL;
                pTailElem->pCW = pEOL;
                break;
            }
            default:
                break;
        }
        //pTailElem->bCwBranchLinking = LSA_FALSE;  //parameter is not used because Tail-Element is not connected!
        return pHeadElem;
    }

    //Add new Branches BCW Einrichten

    CurrentModMask = RedTreeGetModMask(CycleBaseFactor, CurrentReduction);

    //select a free BCW
    pCurrentBCW = &pTree->BcwMod.pBCWArray[QuickIndex];

    SERIniBCWMOD(pDDB, pCurrentBCW, CurrentModMask);

    EDDI_SERConnectCwToNextDyn(pDDB, pHeadElem->pCW, (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pCurrentBCW, pHeadElem->bCwBranchLinking);

    pTailElem->pCW = (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pCurrentBCW;
    pTailElem->Opc = SER_BCW_MOD;
    //pTailElem->bCwBranchLinking = LSA_FALSE;  //parameter is not used because Tail-Element is not connected!

    //init SubBranch 1 and connect
    pTailElem->Sub.Branch.pBranchedElem = EDDI_RedTreeInitBranchWithBCWMod(pDDB,
                                                                           pTree,
                                                                           pTailElem,
                                                                           NextReduction,
                                                                           CurrentPhase,
                                                                           CycleBaseFactor);

    if (pTree->RtClassType == EDDI_TREE_RT_CLASS_ACW_TX)
    {
        //connect NOOP with pBranch of lower BCWMod
        EDDI_SERConnectCwToBranch(pDDB,
                                  (EDDI_SER_CCW_PTR_TYPE)(LSA_VOID *)pCurrentBCW,
                                  pTailElem->Sub.Branch.pBranchedElem->pCW);
    }

    //init SubBranch 2 and connect
    pTailElem->pNextElem = EDDI_RedTreeInitBranchWithBCWMod(pDDB,
                                                            pTree,
                                                            pTailElem,
                                                            NextReduction,
                                                            (CurrentPhase + CurrentReduction),
                                                            CycleBaseFactor);

    return pHeadElem;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeUpdateBCWMods()                      */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeUpdateBCWMods( EDDI_DDB_TYPE   *  const  pDDB,
                                                          EDDI_TREE_TYPE  *  const  pTree,
                                                          LSA_UINT16         const  CurrentReduction,
                                                          LSA_UINT16         const  CurrentPhase,
                                                          LSA_UINT16         const  CycleBaseFactor )
{
    LSA_UINT16                   const  NextReduction = (LSA_UINT16)(CurrentReduction * 2);
    LSA_UINT32                   const  QuickIndex    = RedGetQuickBinaryIndex(CurrentReduction, CurrentPhase);
    LSA_UINT32                          CurrentModMask;
    EDDI_LOCAL_BCW_MOD_PTR_TYPE         pCurrentBCW;

    if (CurrentReduction >= pTree->MaxReduction)
    {
        //break-condition
        return;
    }

    CurrentModMask = RedTreeGetModMask(CycleBaseFactor, CurrentReduction);

    if ((CycleBaseFactor * CurrentReduction) > 0x8000) // Maximum of ModMask reached
    {
        EDDI_Excp("EDDI_RedTreeUpdateBCWMods, CurrentModMask Overflow", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, CurrentReduction);
        return;
    }

    //select BCW to be modified
    pCurrentBCW = &pTree->BcwMod.pBCWArray[QuickIndex];

    SERAlterBCWMOD(pDDB, pCurrentBCW, CurrentModMask);

    //go to SubBranch 1
    EDDI_RedTreeUpdateBCWMods(pDDB,
                              pTree,
                              NextReduction,
                              CurrentPhase,
                              CycleBaseFactor);

    //go to SubBranch 2
    EDDI_RedTreeUpdateBCWMods(pDDB,
                              pTree,
                              NextReduction,
                              (LSA_UINT16)(CurrentPhase + CurrentReduction),
                              CycleBaseFactor);
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
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  RedTreeGetModMask( LSA_UINT32  const  CycleBaseFactor,
                                                            LSA_UINT32  const  CurrentReduction )
{
    if ((CycleBaseFactor * CurrentReduction) > 0x8000) // Maximum of ModMask reached
    {
        EDDI_Excp("RedTreeGetModMask, CurrentModMask Overflow", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, CurrentReduction);
        return 0;
    }

    // ModuloMask depends on CycleBaseFactor Reduction and Phase;
    // CycleBasefactor and Reduction must both have value of n^2 !

    // Example: ( "<<" stands for a leftshift depending on CycleBasefactor )
    //   Reduction       1    |      2          |             4                 |
    //   Phase           1    |   1     2       |   1      2      3      4      |
    //   ~ModMask(Binär) 1<<  |  10<<   11<<    |  100<<  101<<  110<<  111<<   |

    return  (0x7FFF & (~(CycleBaseFactor * CurrentReduction)));
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeInitBasicElements()                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeInitBasicElements( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                      EDDI_TREE_TYPE           *  const  pTree,
                                                                      LSA_UINT32                  const  BranchCnt,
                                                                      LSA_UINT32                  const  CntTxRxElements,
                                                                      LSA_BOOL                    const  bCreatePathload )
{
    LSA_UINT32  Size, Ctr;

    EDDI_FUNCTION_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeInitBasicElements->");

    Size = sizeof(EDDI_TREE_ELEM_TYPE) * BranchCnt;
    EDDI_AllocLocalMem((void * *)&pTree->pHeadElemArray, Size);

    if (pTree->pHeadElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeInitBasicElements, EDDI_AllocLocalMem 1 failed", EDDI_FATAL_ERR_EXCP, pDDB, 0);
        return; //EDD_STS_ERR_RESOURCE;
    }

    EDDI_MemSet(pTree->pHeadElemArray, (LSA_UINT8)0, Size);

    Size = sizeof(EDDI_TREE_ELEM_TYPE) * BranchCnt;
    EDDI_AllocLocalMem((void * *)&pTree->pTailElemArray, Size);

    if (pTree->pTailElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeInitBasicElements, EDDI_AllocLocalMem 2 failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return; //EDD_STS_ERR_RESOURCE;
    }

    EDDI_MemSet(pTree->pTailElemArray, (LSA_UINT8)0, Size);

    //Alloc Buffer for Dynamic TREE-Elements for ACWs
    Size = sizeof(EDDI_TREE_ELEM_TYPE) * CntTxRxElements;

    EDDI_AllocLocalMem((void * *)&pTree->pTxRxElemArray, Size);

    if (pTree->pTxRxElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeInitBasicElements, EDDI_AllocLocalMem 3 failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return; //EDD_STS_ERR_RESOURCE;
    }

    EDDI_MemSet(pTree->pTxRxElemArray, (LSA_UINT8)0, Size);

    for (Ctr = 0; Ctr < CntTxRxElements; Ctr++)
    {
        pTree->pTxRxElemArray[Ctr].Opc = EDDI_TREE_ELEM_OPC_FREE; //mark entry as free
    }

    pTree->TxRxElemCountMax  = CntTxRxElements;
    pTree->TxRxElemCountUsed = 0;

    //Alloc Buffer for PathLoad-Information if needed
    if (bCreatePathload)
    {
        Size = sizeof(EDDI_TREE_PATH_LOAD_TYPE) * pTree->MaxReduction;
        EDDI_AllocLocalMem((void * *)&pTree->pPathLoadArray, Size);

        if (pTree->pPathLoadArray == EDDI_NULL_PTR)
        {
            EDDI_Excp("EDDI_RedTreeInitBasicElements, EDDI_AllocLocalMem 4 failed", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        EDDI_MemSet(pTree->pPathLoadArray, (LSA_UINT8)0, Size);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_REV5


/*****************************************************************************/
/*  end of file eddi_ser_tree_bcw_mod.c                                      */
/*****************************************************************************/




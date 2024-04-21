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
/*  F i l e               &F: eddi_ser_tree_rcw.c                       :F&  */
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
/*  26.03.08    UL    EDDI_RedTreeUpdateRCWHeads: changing the               */
/*                    sendclockfactor of IRT-RCWs is done simply by          */
/*                    inserting the new scf into the RCW                     */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_SER_TREE_RCW
#define LTRC_ACT_MODUL_ID  305

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_SER_TREE_RCW) //satisfy lint!
#endif

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)

#include "eddi_dev.h"
#include "eddi_ser_ext.h"

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_CalcLog2( LSA_UINT32 Arg );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeGetRcwDataByLayerNr( const EDDI_TREE_TYPE * const pTree,
                                                                           LSA_UINT32       const CycleBaseFactor,
                                                                           LSA_UINT32       const Layer,
                                                                           LSA_UINT32     * const pSendClock,
                                                                           LSA_UINT32     * const pReductionRate,
                                                                           LSA_UINT32     * const pShiftInterval,
                                                                           LSA_UINT32     * const pRcwsPerLayer );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_RedTreeBuildAllLayers( EDDI_TREE_TYPE * const pTree,
                                                                LSA_UINT32       const CycleBaseFactor,
                                                                EDDI_DDB_TYPE  * const pDDB );


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
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_CalcLog2( LSA_UINT32  Arg )
{
    LSA_UINT32  lg2;

    for (lg2 = 0; lg2 < 32; lg2++)
    {
        Arg = Arg >> 1;  // Build SquareRoot
        if (Arg == 0)
        {
            break;
        }
    }
    return lg2;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeGetRcwDataByLayerNr( const EDDI_TREE_TYPE  *  const  pTree,
                                                                              LSA_UINT32         const  CycleBaseFactor,
                                                                              LSA_UINT32         const  Layer,
                                                                              LSA_UINT32      *  const  pSendClock,
                                                                              LSA_UINT32      *  const  pReductionRate,
                                                                              LSA_UINT32      *  const  pShiftInterval,
                                                                              LSA_UINT32      *  const  pRcwsPerLayer )
{
    LSA_UINT32  SendInterval; //Sender-Interval of Current Layer in CycleUnits

    SendInterval = CycleBaseFactor * ((LSA_UINT32)1 << Layer);

    if (SendInterval > (EDDI_TREE_RCW_MAX_PHASES_PER_RCW * EDDI_TREE_RCW_MAX_PHASES_PER_RCW))
    {
        EDDI_Excp("EDDI_RedTreeGetRcwDataByLayerNr SendInterval too big; SendInterval ", EDDI_FATAL_ERR_EXCP, SendInterval, 0);
        return;
    }

    if (Layer == 0)
    {
        // Special Treatment of Layer 0 (No RCW needed)
        *pSendClock     = 0;
        *pReductionRate = 0;
        *pRcwsPerLayer  = 0;
        *pShiftInterval = 0;
        return;
    }

    //+++++++++++++++++++++
    // Calc SendClock
    //+++++++++++++++++++++
    *pSendClock = (LSA_UINT8)EDDI_MIN(CycleBaseFactor, EDDI_TREE_RCW_MAX_PHASES_PER_RCW);

    //+++++++++++++++++++++
    //Calc RcwsPerLayer and ShiftInterval
    //+++++++++++++++++++++
    if (Layer <= pTree->Rcw.LastLayer_SingleRcw)
    {
        *pSendClock     = CycleBaseFactor;
        *pReductionRate = (SendInterval / *pSendClock);
        *pRcwsPerLayer  = 1;
        *pShiftInterval = CycleBaseFactor;
    }
    else if (Layer <= pTree->Rcw.LastLayer_MultiRcw_NoLimit)
    {
        *pSendClock     = (CycleBaseFactor << (Layer - pTree->Rcw.LastLayer_SingleRcw));
        *pReductionRate = EDDI_TREE_RCW_MAX_PHASES_PER_RCW;
        *pRcwsPerLayer  = ((LSA_UINT32)1 << (Layer - pTree->Rcw.LastLayer_SingleRcw));
        *pShiftInterval = CycleBaseFactor;
    }
    else
    {
        *pSendClock     = (CycleBaseFactor << (Layer - pTree->Rcw.LastLayer_SingleRcw));
        *pReductionRate = EDDI_TREE_RCW_MAX_PHASES_PER_RCW;
        *pRcwsPerLayer  = EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER / EDDI_TREE_RCW_MAX_PHASES_PER_RCW;
        *pShiftInterval = ((LSA_UINT32)((LSA_UINT32)1 << Layer) / EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER);
    }

    if (*pReductionRate == 0)
    {
        EDDI_Excp("EDDI_RedTreeGetRcwDataByLayerNr *pReductionRate == 0; SendInterval ", EDDI_FATAL_ERR_EXCP, SendInterval, 0);
        return;
    }
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
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_RedRcwGetQuickBranchIndex( const EDDI_TREE_TYPE  *  const  pTree,
                                                                               LSA_UINT32         const  CycleReduction,
                                                                               LSA_UINT32         const  CyclePhase,
                                                                               LSA_UINT32         const  Layer )
{
    LSA_UINT32  Index, PhaseInterval;

    // We dont have to search the whole tree for our branch-Ptr!
    // Thanks to the compact arrangement of the tree
    // we just calculate a quick shortcut to the searched Branch:

    if (Layer <= pTree->Rcw.LastLayer_MultiRcw_NoLimit)
    {
        Index = (CycleReduction - 1) + (CyclePhase - 1);
    }
    else
    {
        LSA_UINT32  Position;
        // Special Calculation for Limited Phases needed !
        // Check if Phase exists
        PhaseInterval = CycleReduction / EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER;

        if ((CyclePhase - 1) & PhaseInterval)
        {
            EDDI_Excp("RedRcwGetQuickIndexForBranch CyclePhase not supported; CyclePhase, CycleReduction", EDDI_FATAL_ERR_EXCP, CyclePhase, CycleReduction);
            return 0;
        }

        if (PhaseInterval == 0)
        {
            EDDI_Excp("RedRcwGetQuickIndexForBranch (PhaseInterval == 0); CyclePhase, CycleReduction", EDDI_FATAL_ERR_EXCP, CyclePhase, CycleReduction);
            return 0;
        }

        Position = (CyclePhase - 1) / PhaseInterval;

        Index    = (pTree->Rcw.LastReduction_MultiRcw_NoLimit - 1)          +  // All Indices of "unlimited" TreeBranches
                   ( ( (Layer - pTree->Rcw.LastLayer_MultiRcw_NoLimit) - 1)    //
                     * EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER )            +   // All Indices of "limited" Treebranches since
                   (Position );                                                // Position if Element in current Layer.
    }

    return Index;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeBuildAllLayers()                     */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeBuildAllLayers( EDDI_TREE_TYPE  *  const  pTree,
                                                                   LSA_UINT32         const  CycleBaseFactor,
                                                                   EDDI_DDB_TYPE   *  const  pDDB )
{
    LSA_UINT32                   Layer;
    EDDI_LOCAL_RCW_RED_PTR_TYPE  pRCW;
    //EDDI_TREE_ELEM_PTR_TYPE    pHeadElem;
    //EDDI_TREE_ELEM_PTR_TYPE    pTailElem;
    EDDI_LOCAL_MEM_U8_PTR_TYPE   pCurMem;
    EDDI_SER_CCW_PTR_TYPE        pLastCw;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeBuildAllLayers");

    pLastCw = pTree->pRootCw;
    pCurMem = pTree->Rcw.pRcwBlock;

    for (Layer = 0; Layer <= pTree->Rcw.MaxLayer; Layer++)
    {
        LSA_UINT32  CycleReduction, CyclePhase;
        LSA_UINT32  PhasesPerRcw, CurRcwNr;
        LSA_UINT32  SendClock     = 0;
        LSA_UINT32  RcwsPerLayer  = 0;
        LSA_UINT32  ReductionRate = 0;
        LSA_UINT32  ShiftInterval = 0;
        LSA_UINT32  QuickIndex, Ctr, ShiftInterval_div_CycleBaseFactor;

        CycleReduction = ((LSA_UINT32) 1 << Layer);

        EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "******************  EDDI_RedTreeBuildAllLayers  Layer:0x%X", Layer);

        // Calculate RCW-Params depending on Layer
        EDDI_RedTreeGetRcwDataByLayerNr(pTree, CycleBaseFactor, Layer, &SendClock, &ReductionRate, &ShiftInterval, &RcwsPerLayer);

        PhasesPerRcw = ReductionRate;

        if (Layer == 0)
        {
            CyclePhase = 1;
            QuickIndex = EDDI_RedRcwGetQuickBranchIndex(pTree, CycleReduction, CyclePhase, Layer);

            // Build Branch
            // Attention: In this Branch
            // the TailElement intentionaly is pointing to the first (not yet initialized !) RCW.
            EDDI_RedTreeBuildBranch(SER_ACW_TX,
                                    &pTree->pHeadElemArray[QuickIndex],
                                    pLastCw,
                                    &pTree->pTailElemArray[QuickIndex],
                                    (EDDI_SER_CCW_PTR_TYPE)(void *)pTree->Rcw.pRcwBlock,
                                    pDDB);

            // pLastCw will not be changed here !!
        }
        else
        {
            LSA_UINT32  const  Size       = sizeof(EDDI_SER10_RCW_RED_TYPE) + (PhasesPerRcw * sizeof(EDDI_SER10_RCW_PHS_TYPE));
            LSA_UINT32  const  DeltaPhase = (CycleReduction / PhasesPerRcw); // Distance between Phases of *current* RCW

            ShiftInterval_div_CycleBaseFactor = (ShiftInterval / CycleBaseFactor);

            for (CurRcwNr = 0; CurRcwNr < RcwsPerLayer; CurRcwNr++)
            {
                EDDI_LOCAL_RCW_PHS_PTR_TYPE  pRcwPhsArray;
                LSA_UINT32                  FirstPhase;

                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_RedTreeBuildAllLayers ***** CurRcwNr:0x%X", CurRcwNr );

                pRCW         = (EDDI_LOCAL_RCW_RED_PTR_TYPE)(void *)pCurMem;
                pRcwPhsArray = (EDDI_LOCAL_RCW_PHS_PTR_TYPE)(void *)(pCurMem + sizeof(EDDI_SER10_RCW_RED_TYPE));

                pCurMem += Size;

                //Build KRAM - RCW
                pDDB->CRT.SERIniRcwFct(pDDB, pRCW, PhasesPerRcw, SendClock, ReductionRate, (CurRcwNr * ShiftInterval));

                //Connect RCW-Head-to predecessor
                EDDI_SERConnectCwToNext(pDDB, pLastCw, (EDDI_SER_CCW_PTR_TYPE)(void *)pRCW);

                //Build Control - Layer of this RCW
                //--> Build all Phase-Branches of this RCW
                FirstPhase = (ShiftInterval_div_CycleBaseFactor * CurRcwNr) + 1;

                for (Ctr = 0; Ctr < PhasesPerRcw; Ctr++)
                {
                    //loop is executed approximately 1000 times
                    CyclePhase = (FirstPhase + (Ctr * DeltaPhase));
                    QuickIndex = EDDI_RedRcwGetQuickBranchIndex(pTree, CycleReduction, CyclePhase, Layer);

                    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_RedTreeBuildAllLayers->CyclePhase:0x%X QuickIndex:0x%X", CyclePhase, QuickIndex);

                    //EDDI_RedTreeBuildBranch
                    EDDI_RedTreeBuildBranchRCWPHS(pDDB, &pTree->pHeadElemArray[QuickIndex],
                                                  (EDDI_SER_CCW_PTR_TYPE)(void *)&pRcwPhsArray[Ctr],
                                                  &pTree->pTailElemArray[QuickIndex]);
                }
                pLastCw = (EDDI_SER_CCW_PTR_TYPE)(void *)pRCW;
            }
        }
    }

    if (pTree->RtClassType != EDDI_TREE_RT_CLASS_FCW_TX) //EDDI_TREE_RT_CLASS_FCW_RX || EDDI_TREE_RT_CLASS_ACW_TX
    {
        //Finally connect last RCW to EOL
        EDDI_SERConnectCwToNext(pDDB, pLastCw, (EDDI_SER_CCW_PTR_TYPE)(void *)pTree->pEOLn[0]);
    }
    else //EDDI_TREE_RT_CLASS_FCW_TX
    {
        //Finally connect last RCW to NULL
        EDDI_SERConnectCwToNext(pDDB, pLastCw, (EDDI_SER_CCW_PTR_TYPE)(void *)EDDI_NULL_PTR);
    }
    pTree->pRootElem = &pTree->pHeadElemArray[0];
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeRcwInit()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeRcwInit( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                    EDDI_TREE_TYPE           *  const  pTree,
                                                    EDDI_TREE_RT_CLASS_TYPE     const  RtClassType,
                                                    LSA_UINT32                  const  CycleBaseFactor,
                                                    LSA_UINT32                  const  CfgMaxReduction,
                                                    LSA_UINT32                  const  ProviderCnt )
{
    LSA_UINT32  Size;
    LSA_UINT32  MaxReduction;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeRcwInit");

    //General Config-Checks
    #if (EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER < EDDI_TREE_RCW_MAX_PHASES_PER_RCW)
    #error "EDDI_RedTreeRcwInit: EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER < EDDI_TREE_RCW_MAX_PHASES_PER_RCW"
    #endif

    if (CycleBaseFactor > EDDI_TREE_RCW_MAX_PHASES_PER_RCW)
    {
        EDDI_Excp("EDDI_RedTreeRcwInit CycleBaseFactor > 128 (not allowed for RCW-Tree)", EDDI_FATAL_ERR_EXCP, CycleBaseFactor, EDDI_TREE_RCW_MAX_PHASES_PER_RCW);
        return;
    }

    if (!EDDI_RedIsBinaryValue(CycleBaseFactor))
    {
        LSA_UINT32  Exp2, OddMaxReduction;

        OddMaxReduction = (128 * 128) / CycleBaseFactor;
        //Calculate biggest 2^n - value wich is smaller than MaxReduction:
        Exp2         = EDDI_CalcLog2(OddMaxReduction);
        MaxReduction = (LSA_UINT32)1 << Exp2;
    }
    else
    {
        //MaxReduction is limited by:
        //- EDDI_TREE_MAX_BINARY_REDUCTION and
        //- RCW-Member-Values: ReductionRatio <= 128, SendClock <=128)
        MaxReduction = (128 * 128) / CycleBaseFactor;
    }
    MaxReduction = EDDI_MIN(CfgMaxReduction, MaxReduction);

    //build a binary tree: part 1
    pTree->RtClassType   = RtClassType;
    pTree->ReductionMode = EDDI_TREE_REDUCTION_MODE_BINARY;
    pTree->MaxReduction  = MaxReduction;

    //Calculate Counts of the TreeLayers
    pTree->Rcw.MaxLayer = EDDI_CalcLog2(MaxReduction);
    if (CycleBaseFactor <= EDDI_TREE_RCW_MAX_PHASES_PER_RCW)
    {
        pTree->Rcw.LastLayer_SingleRcw = EDDI_CalcLog2(EDDI_MIN(MaxReduction, EDDI_TREE_RCW_MAX_PHASES_PER_RCW));
    }
    else
    {
        pTree->Rcw.LastLayer_SingleRcw = 0;
    }

    pTree->Rcw.LastLayer_MultiRcw_NoLimit     = EDDI_CalcLog2(EDDI_MIN(MaxReduction, EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER));
    pTree->Rcw.LastReduction_MultiRcw_NoLimit =               EDDI_MIN(MaxReduction, EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER);

    //Calculate Counts of RCW-Ressources
    pTree->Rcw.CntRcws     = pTree->Rcw.LastLayer_SingleRcw +                                                                                                          // TreeBottom - Layers
                             ( ((1UL << ( pTree->Rcw.LastLayer_MultiRcw_NoLimit - pTree->Rcw.LastLayer_SingleRcw )) - 1UL) * 2UL )+                                       // TreeMiddle - Layers
                             ( ( pTree->Rcw.MaxLayer - pTree->Rcw.LastLayer_MultiRcw_NoLimit ) *
                               ( EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER / EDDI_TREE_RCW_MAX_PHASES_PER_RCW) );// TreeTop    - Layers

    pTree->Rcw.CntBranches = 1UL +                                                                                                                                     // Layer0 Branch
                             ( ((1UL << pTree->Rcw.LastLayer_MultiRcw_NoLimit) - 1UL) * 2 ) +                                                                              // TreeBottom - and TreeMiddle - Layers
                             ( (pTree->Rcw.MaxLayer - pTree->Rcw.LastLayer_MultiRcw_NoLimit) *
                               EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER);                                  // TreeTop    - Layers

    EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_RedTreeRcwInit  CntRcws:0x%X CntBranches:0x%X", pTree->Rcw.CntRcws, pTree->Rcw.CntBranches);

    //Calculate Size of KRAM-Memory for RCW-Elements-Entries
    Size = (sizeof(EDDI_SER10_RCW_RED_TYPE) * pTree->Rcw.CntRcws) +
           (sizeof(EDDI_SER10_RCW_PHS_TYPE) * (pTree->Rcw.CntBranches - 1));  // Attention: RCW-Branches are reduced by 1
    // because Root-Branch has no RCW

    pTree->Rcw.RcwBlockSize = Size;
    pTree->KramSize         = Size;

    pTree->TxRxElemCountMax = ProviderCnt;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeRcwBuild()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeRcwBuild( EDDI_DDB_TYPE                   *  const  pDDB,
                                                     EDDI_TREE_TYPE                  *  const  pTree,
                                                     LSA_UINT32                         const  CycleBaseFactor,
                                                     EDDI_SER_CCW_PTR_TYPE              const  pRootCw,
                                                     EDDI_LOCAL_EOL_PTR_TYPE  const  *  const  ppEOLn )
{
    LSA_UINT32  Size;
    LSA_UINT32  Ctr, Ret;
    LSA_UINT32  EOLCount, EOLCountMax;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeRcwBuild->");

    //build a binary tree: part 2
    pTree->pRootCw = pRootCw;  // Only Store Reference to RootCw (RootCw must already have been allocated outside this function)

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

    Ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pTree->Rcw.pRcwBlockMemHeader, pTree->Rcw.RcwBlockSize);

    if (Ret != EDDI_MEM_RET_OK)
    {
        if (Ret == EDDI_MEM_RET_NO_FREE_MEMORY)
        {
            EDDI_Excp("EDDI_RedTreeRcwBuild, 5 - Not enough free KRAM !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->Rcw.RcwBlockSize);
            return;
        }
        else
        {
            EDDI_Excp("EDDI_RedTreeRcwBuild, 5 - EDDI_MEMGetBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, Ret, pTree->Rcw.RcwBlockSize);
            return;
        }
    }

    pDDB->KramRes.pool.rcw += pTree->Rcw.RcwBlockSize;
    pTree->Rcw.pRcwBlock    = pTree->Rcw.pRcwBlockMemHeader->pKRam;

    //JM EDDI_MemSet(pTree->Rcw.pRcwBlock, (LSA_UINT8)0, Size);

    Size = sizeof(EDDI_TREE_ELEM_TYPE) * pTree->Rcw.CntBranches;
    EDDI_AllocLocalMem( (void * *)&pTree->pHeadElemArray, Size);

    if (pTree->pHeadElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeRcwBuild, 6 EDDI_ALLOC_LOCAL_MEM failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    EDDI_MemSet(pTree->pHeadElemArray, (LSA_UINT8)0, Size);

    Size = sizeof(EDDI_TREE_ELEM_TYPE) * pTree->Rcw.CntBranches;
    EDDI_AllocLocalMem( (void * *)&pTree->pTailElemArray, Size);

    if (pTree->pTailElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeRcwBuild, 7 EDDI_ALLOC_LOCAL_MEM failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    EDDI_MemSet(pTree->pTailElemArray, (LSA_UINT8)0, Size);

    // Alloc Buffer for Dynamic TREE-Elements for ACWs
    Size = sizeof(EDDI_TREE_ELEM_TYPE) * pTree->TxRxElemCountMax;

    EDDI_AllocLocalMem((void * *)&pTree->pTxRxElemArray, Size);

    if (pTree->pTxRxElemArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeRcwBuild, 8 EDDI_ALLOC_LOCAL_MEM failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    //JM EDDI_MemSet(pTree->pTxRxElemArray, (LSA_UINT8)0, Size);

    for (Ctr = 0; Ctr < pTree->TxRxElemCountMax; Ctr++)
    {
        pTree->pTxRxElemArray[Ctr].Opc = EDDI_TREE_ELEM_OPC_FREE; //Mark entry as free
    }

    pTree->TxRxElemCountUsed = 0;

    //Alloc Buffer for PathLoad-Information
    Size = sizeof(EDDI_TREE_PATH_LOAD_TYPE) * pTree->MaxReduction;
    EDDI_AllocLocalMem((void * *)&pTree->pPathLoadArray, Size);

    if (pTree->pPathLoadArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_RedTreeRcwBuild, 9 EDDI_ALLOC_LOCAL_MEM failed", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    EDDI_MemSet(pTree->pPathLoadArray, (LSA_UINT8)0, Size);

    EDDI_RedTreeBuildAllLayers(pTree, CycleBaseFactor, pDDB);

    //Finally connect SerAcwHeader to first Entry in KRAM-Tree
    //But this will be done in Device-Setup-Call
    //with function EDDI_SERConnectCwToHeader(pDDB, pDDB->Glob.LLHandle.pACWDevBase->pTx, pTree->pRootElem->pCW);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeUpdateRCWHeads()                     */
/*                                                                         */
/* D e s c r i p t i o n: Steps through all RCW-Headers and updates its    */
/*                        Sendclock-Parameter.                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeUpdateRCWHeads( EDDI_DDB_TYPE   *  const  pDDB,
                                                     const EDDI_TREE_TYPE  *  const  pTree,
                                                           LSA_UINT16         const  CycleBaseFactor )
{
    LSA_UINT32                   Ctr;
    LSA_UINT32                   QuickIndex;
    EDDI_LOCAL_RCW_RED_PTR_TYPE  pRCW;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeUpdateRCWHeads->");

    //Get First RCW
    QuickIndex = EDDI_RedRcwGetQuickBranchIndex(pTree, 1, 1, 0);

    pRCW = (EDDI_LOCAL_RCW_RED_PTR_TYPE)pTree->pTailElemArray[QuickIndex].pCW;

    if //no change?
       (EDDI_SERGetRcwSendClock(pDDB, pRCW) == CycleBaseFactor)
    {
        //CycleBaseFactor == OldSendClock --> No changes needed
        return;
    }

    //Now step through all RCWs and change its SendClock-Parameter.
    for (Ctr = 0; (pRCW != EDDI_NULL_PTR) && (Ctr < 1000); Ctr++)
    {
        //Update CurrentRcw and get next RCWs
        pRCW = EDDI_SERUpdateRcwSendClock(pDDB, pRCW, (LSA_UINT8)CycleBaseFactor);
    }

    if (Ctr == 1000UL)
    {
        EDDI_Excp("EDDI_RedTreeUpdateRCWHeads, Overflow Parsing RCWs", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RedTreeRcwClose()                           */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RedTreeRcwClose(       EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     const EDDI_TREE_TYPE  *        const  pTree )
{
    LSA_UINT32  Ret;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RedTreeRcwClose->");

    Ret = EDDI_MEMFreeBuffer(pTree->Rcw.pRcwBlockMemHeader);
    if (Ret != EDDI_MEM_RET_OK)
    {
        EDDI_Excp("EDDI_RedTreeRcwClose, pTree->Rcw.pRcwBlockMemHeader", EDDI_FATAL_ERR_EXCP, Ret, 0);
        return;
    }
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //EDDI_CFG_REV6 || EDDI_CFG_REV7


/*****************************************************************************/
/*  end of file eddi_ser_tree_rcw.c                                          */
/*****************************************************************************/




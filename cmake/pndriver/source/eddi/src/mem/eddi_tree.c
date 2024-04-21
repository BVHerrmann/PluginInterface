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
/*  F i l e               &F: eddi_tree.c                               :F&  */
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
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_EDDI_TREE
#define LTRC_ACT_MODUL_ID  26

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_EDDI_TREE) //satisfy lint!
#endif

#if defined (EDDI_CFG_USE_SW_RPS) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)

#include "eddi_dev.h"

static E_TREE_NODE *  EDDI_LOCAL_FCT_ATTR EDDI_TreeIni_element( LSA_UINT32    const Reduction,
                                                                LSA_UINT32    const Phase,
                                                                E_TREE      * const pTree,
                                                                E_TREE_NODE * const pRootNode );

static E_TREE_NODE  * EDDI_LOCAL_FCT_ATTR EDDI_TreeGetNewElement( E_TREE     * const pTree,
                                                                  LSA_UINT32   const CurrentReduction,
                                                                  LSA_UINT32   const CurrentPhase );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TQueueAddToEnd( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                            E_TREE_NODE               *  const  pNode,
                                                            E_TREE_LIST               *  const  pEntry );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TQueueRemove( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                          E_TREE_NODE               *  const  pNode,
                                                          E_TREE_LIST               *  const  pEntry );

static LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_TreeGetQuickBinaryIndex( LSA_UINT32 const CycleReduction,
                                                                    LSA_UINT32 const CyclePhase );


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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TreeIni( LSA_UINT32        const  MaxReduction,
                                             E_TREE      *  *  const  ppTree )
{
    E_TREE_NODE  *  const  pRootNode = (E_TREE_NODE *)0;

    EDDI_AllocLocalMem((void * *)ppTree, (sizeof(E_TREE_NODE) * MaxReduction * 2) + sizeof(E_TREE));

    if (*ppTree == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_TreeIni", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_MemSet(*ppTree, (LSA_UINT8)0, (LSA_UINT32)sizeof(E_TREE));

    (*ppTree)->MaxReduction         = MaxReduction;
    (*ppTree)->ActMaxPathDepthIndex = -1;

    // Phase beginnt immer bei 1
    (void)EDDI_TreeIni_element(1UL, 1UL, *ppTree, pRootNode);
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
static  E_TREE_NODE  *  EDDI_LOCAL_FCT_ATTR  EDDI_TreeIni_element( LSA_UINT32      const  Reduction,
                                                                   LSA_UINT32      const  Phase,
                                                                   E_TREE       *  const  pTree,
                                                                   E_TREE_NODE  *  const  pRootNode )
{
    E_TREE_NODE  *  pNewNode;

    if (Reduction > pTree->MaxReduction)
    {
        return (E_TREE_NODE *)0;
    }

    pNewNode = EDDI_TreeGetNewElement(pTree, Reduction, Phase);

    if (!pNewNode)
    {
        EDDI_Excp("EDDI_TreeIni_element", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    { //temporarily_disabled_lint --e{661}
        switch (Reduction)
        {
            case   1:
                pNewNode->PathDepthIndex = 0;
                break;
            case   2:
                pNewNode->PathDepthIndex = 1;
                break;
            case   4:
                pNewNode->PathDepthIndex = 2;
                break;
            case   8:
                pNewNode->PathDepthIndex = 3;
                break;
            case  16:
                pNewNode->PathDepthIndex = 4;
                break;
            case  32:
                pNewNode->PathDepthIndex = 5;
                break;
            case  64:
                pNewNode->PathDepthIndex = 6;
                break;
            case 128:
                pNewNode->PathDepthIndex = 7;
                break;
            case 256:
                pNewNode->PathDepthIndex = 8;
                break;
            case 512:
                pNewNode->PathDepthIndex = 9;
                break;
            default:
            {
                EDDI_Excp("EDDI_TreeIni_element, ActMaxReduction", EDDI_FATAL_ERR_EXCP, Reduction, 0);
                return (E_TREE_NODE *)0;
            }
        }

        pNewNode->phase     = Phase;
        pNewNode->reduction = Reduction;
        pNewNode->pRootNode = pRootNode;
        pNewNode->pTree     = pTree;

        pNewNode->pNodes_attached[0] = EDDI_TreeIni_element(Reduction * 2, Phase,             pTree, pNewNode);
        pNewNode->pNodes_attached[1] = EDDI_TreeIni_element(Reduction * 2, Phase + Reduction, pTree, pNewNode);
    }

    return pNewNode;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_USE_SW_RPS) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TreeFree( E_TREE  *  const  pTree )
{
    LSA_UINT16  ret_val;

    EDDI_FREE_LOCAL_MEM(&ret_val, pTree);
    if (ret_val != LSA_RET_OK)
    {
        EDDI_Excp("EDDI_TreeFree", EDDI_FATAL_ERR_EXCP, ret_val, 0);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


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
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_TreeGetQuickBinaryIndex( LSA_UINT32  const  CycleReduction,
                                                                       LSA_UINT32  const  CyclePhase )
{
    return ((CycleReduction - 1) + (CyclePhase - 1));
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
void  EDDI_LOCAL_FCT_ATTR  EDDI_TreeAdd( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                         E_TREE                    *  const  pTree,
                                         LSA_UINT32                   const  Reduction,
                                         LSA_UINT32                   const  Phase,
                                         E_TREE_LIST               *  const  pEntry )
{
    LSA_UINT32      index;
    E_TREE_NODE  *  pNode;

    if (Reduction > pTree->MaxReduction)
    {
        EDDI_Excp("EDDI_TreeAdd, Reduction", EDDI_FATAL_ERR_EXCP, Reduction, pTree->MaxReduction);
        return;
    }

    if (Phase > Reduction)
    {
        EDDI_Excp("EDDI_TreeAdd, Phase", EDDI_FATAL_ERR_EXCP, Reduction, Phase);
        return;
    }

    index = EDDI_TreeGetQuickBinaryIndex(Reduction, Phase);

    pNode = &pTree->BaseNode[index];

    pTree->CntEntriesPerDepth[pNode->PathDepthIndex]++;

    if (pTree->ActMaxPathDepthIndex < pNode->PathDepthIndex)
    {
        pTree->ActMaxPathDepthIndex = pNode->PathDepthIndex;
    }

    pEntry->Phase     = Phase;
    pEntry->Reduction = Reduction;
    pEntry->pTree     = pTree;

    EDDI_TQueueAddToEnd(pDDB, pNode, pEntry);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_TreeRemove()                                */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TreeRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                E_TREE_LIST              *  const  pEntry )
{
    LSA_UINT32      index;
    E_TREE_NODE  *  pNode;

    index = EDDI_TreeGetQuickBinaryIndex(pEntry->Reduction, pEntry->Phase);

    pNode = &(pEntry->pTree)->BaseNode[index];

    EDDI_TQueueRemove(pDDB, pNode, pEntry);

    switch (pEntry->pTree->CntEntriesPerDepth[pNode->PathDepthIndex])
    {
        case 0:
        {
            EDDI_Excp("EDDI_TreeRemove - cEntriesPerDepth", EDDI_FATAL_ERR_EXCP, pEntry->Reduction, pEntry->Phase);
            return;
        }

        case 1:
        {
            if (pEntry->pTree->ActMaxPathDepthIndex == pNode->PathDepthIndex)
            {
                pEntry->pTree->ActMaxPathDepthIndex--;
            }
            break;
        }

        default:
            break;
    }

    pEntry->pTree->CntEntriesPerDepth[pNode->PathDepthIndex]--;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TreeGetPathList( E_TREE  *  const  pTree )
{
    LSA_UINT32      next;
    LSA_INT32       i;
    E_TREE_NODE  *  pNode;

    pTree->CntActPath = 0;

    pNode = &pTree->BaseNode[0];

    for (i = 0; i <= pTree->ActMaxPathDepthIndex; i++)
    {
        if (pNode->pTop)
        {
            pTree->ActPath[pTree->CntActPath].pList      = pNode->pTop;
            pTree->ActPath[pTree->CntActPath].CntEntries = pNode->CntListEntries;
            pTree->CntActPath++;
        }

        next = pNode->CntNextAttached & 0x01;

        pNode->CntNextAttached++; // Weg umschalten fuer nächsten Durchlauf

        pNode = pNode->pNodes_attached[next];
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
static  E_TREE_NODE  *  EDDI_LOCAL_FCT_ATTR  EDDI_TreeGetNewElement( E_TREE      *  const  pTree,
                                                                     LSA_UINT32     const  CurrentReduction,
                                                                     LSA_UINT32     const  CurrentPhase )
{
    E_TREE_NODE  *  pNode;
    LSA_UINT32      QuickIndex;

    QuickIndex = EDDI_TreeGetQuickBinaryIndex(CurrentReduction, CurrentPhase);

    if (QuickIndex >= (pTree->MaxReduction * 2))
    {
        EDDI_Excp("EDDI_TreeGetNewElement", EDDI_FATAL_ERR_EXCP, QuickIndex, 0);
        return (E_TREE_NODE *)0;
    }

    { //temporarily_disabled_lint --e{661} --e{662}
        pNode = &pTree->BaseNode[QuickIndex];

        pNode->CntListEntries = 0;
        pNode->pTop           = (E_TREE_LIST *)0;
        pNode->pBottom        = (E_TREE_LIST *)0;

        pNode->pNodes_attached[0] = (E_TREE_NODE *)0;
        pNode->pNodes_attached[1] = (E_TREE_NODE *)0;

        pNode->CntNextAttached = 0;
        pNode->pRootNode       = (E_TREE_NODE *)0;
    }

    return pNode;
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TQueueAddToEnd( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                            E_TREE_NODE              *  const  pNode,
                                                            E_TREE_LIST              *  const  pEntry )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TQueueAddToEnd->");

    pEntry->pNext = (E_TREE_LIST *)0;
    pEntry->pPrev = (E_TREE_LIST *)0;

    if (pNode->pTop == (E_TREE_LIST *)0)
    {
        pNode->pTop = pEntry;
    }
    else
    {
        (pNode->pBottom)->pNext = pEntry;
        pEntry->pPrev           = pNode->pBottom;
    }

    pNode->pBottom = pEntry;

    pNode->CntListEntries++;
    LSA_UNUSED_ARG(pDDB);
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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TQueueRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                          E_TREE_NODE              *  const  pNode,
                                                          E_TREE_LIST              *  const  pEntry )
{
    E_TREE_LIST  *  pAct;
    LSA_UINT32      i;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_TQueueRemove->");

    pAct = pNode->pTop;

    for (i = 0; i < pNode->CntListEntries; i++)
    {
        if (pEntry == pAct)
        {
            //found!
            if (i == 0)
            {
                //unchain first element
                pNode->pTop = (pNode->pTop)->pNext;
                if (pNode->pTop)
                {
                    (pNode->pTop)->pPrev = (E_TREE_LIST *)0;
                }
            }
            else
            {
                //unchain intermediate or last element
                (pAct->pPrev)->pNext = pAct->pNext;
                if (pAct->pNext == (E_TREE_LIST *)0)
                {
                    //has been last element!
                    pNode->pBottom = pAct->pPrev;
                }
                else
                {
                    (pAct->pNext)->pPrev = pAct->pPrev;
                }
            }

            pNode->CntListEntries--;
            return;
        }

        pAct = pAct->pNext;
    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_Excp("EDDI_TQueueRemove", EDDI_FATAL_ERR_EXCP, pNode->CntListEntries, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/****************************************************************************/
/*  end of file eddi_tree.c                                                 */
/****************************************************************************/


#ifndef EDDI_TREE_H             //reinclude-protection
#define EDDI_TREE_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
extern "C"
{
#endif

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
/*  F i l e               &F: eddi_tree.h                               :F&  */
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
/*                                                                           */
/*****************************************************************************/

typedef struct _E_TREE_LIST
{
    struct _E_TREE_LIST    *pNext;
    struct _E_TREE_LIST    *pPrev;
    struct _E_TREE         *pTree;
    LSA_UINT32             Reduction;
    LSA_UINT32             Phase;

} E_TREE_LIST;

#if defined (EDDI_CFG_USE_SW_RPS) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)

typedef struct _E_TREE_NODE
{
    struct _E_TREE_NODE    *pNodes_attached[2];
    LSA_UINT32             CntNextAttached;

    E_TREE_LIST            *pTop;
    E_TREE_LIST            *pBottom;
    LSA_UINT32             CntListEntries;
    LSA_INT32              PathDepthIndex;
    struct _E_TREE_NODE    *pRootNode;

    LSA_UINT32             reduction;
    LSA_UINT32             phase;
    struct _E_TREE         *pTree;

} E_TREE_NODE;

#define EDDI_TREE_MAX_DEPTH    10

typedef struct _E_PATH
{
    LSA_UINT32             CntEntries;
    void                   *pList;

} E_PATH;

typedef struct _E_TREE
{
    LSA_UINT32             MaxReduction;

    LSA_INT32              ActMaxPathDepthIndex;
    LSA_UINT32             CntEntriesPerDepth[EDDI_TREE_MAX_DEPTH];

    E_PATH                 ActPath[EDDI_TREE_MAX_DEPTH];
    LSA_UINT32             CntActPath;

    //has to be last element!
    E_TREE_NODE            BaseNode[1];

} E_TREE;

void EDDI_LOCAL_FCT_ATTR EDDI_TreeIni( LSA_UINT32     const MaxReduction,
                                       E_TREE     * * const ppTree );

void EDDI_LOCAL_FCT_ATTR EDDI_TreeFree( E_TREE * const pTree );

void  EDDI_LOCAL_FCT_ATTR  EDDI_TreeAdd( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                         E_TREE                    *  const  pTree,
                                         LSA_UINT32                   const  Reduction,
                                         LSA_UINT32                   const  Phase,
                                         E_TREE_LIST               *  const  pEntry );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TreeRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                E_TREE_LIST              *  const  pEntry );

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_TreeGetPathList( E_TREE * const pTree );

#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_TREE_H


/*****************************************************************************/
/*  end of file eddi_tree.h                                                  */
/*****************************************************************************/

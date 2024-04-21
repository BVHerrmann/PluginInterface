#ifndef EDDI_SER_TREE_BCW_MOD_H //reinclude-protection
#define EDDI_SER_TREE_BCW_MOD_H

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
/*  F i l e               &F: eddi_ser_tree_bcw_mod.h                   :F&  */
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
/*  Date        Who   What   Needed for Reduction-Trees                      */
/*                                                                           */
/*****************************************************************************/

// +++++++++++++++++++++++++++
// BCW-MOD-Specific Part:
// +++++++++++++++++++++++++++
typedef struct _EDDI_TREE_BCW_MOD_SPECIFIC_TYPE
{
    EDDI_LOCAL_BCW_MOD_PTR_TYPE       pBCWArray;       // local Pointer to the Array of BCWs
    EDDI_LOCAL_NOOP_PTR_TYPE          pNOOPArray;      // local Pointer to the Array of NOOPs

    EDDI_MEM_BUF_EL_H                *pBCWArrayMemHeader;
    LSA_UINT32                        SizeBCWs;

    EDDI_MEM_BUF_EL_H                *pNOOPArrayMemHeader;
    LSA_UINT32                        SizeNOOPs;

    LSA_UINT32                        CntNOOPs;
    LSA_UINT32                        CntBCWs;
    LSA_UINT32                        BranchCnt;

    LSA_UINT32                        TxRxElementCnt;

} EDDI_TREE_BCW_MOD_SPECIFIC_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TREE_BCW_MOD_H


/*****************************************************************************/
/*  end of file eddi_ser_tree_bcw_mod.h                                      */
/*****************************************************************************/

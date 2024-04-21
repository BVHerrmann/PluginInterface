#ifndef EDDI_SER_TREE_RCW_H     //reinclude-protection
#define EDDI_SER_TREE_RCW_H

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
/*  F i l e               &F: eddi_ser_tree_rcw.h                       :F&  */
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

#define EDDI_TREE_RCW_MAX_PHASES_PER_RCW                     128UL  // Maximum of allowed Phases per RCW 
// (induced by HW)

#define EDDI_TREE_RCW_LIMITED_PHASES_PER_LAYER               512UL  // Limit of supported Phases per TreeLayer

// +++++++++++++++++++++++++++
// RCW-Specific Part:
// +++++++++++++++++++++++++++
typedef struct _EDDI_TREE_RCW_SPECIFIC_TYPE
{
    EDDI_MEM_BUF_EL_H                 *pRcwBlockMemHeader;
    EDDI_LOCAL_MEM_U8_PTR_TYPE         pRcwBlock;
    LSA_UINT32                        RcwBlockSize;

    LSA_UINT32                        CntBranches;
    LSA_UINT32                        CntRcws;

    // Relation Between LayerNr and Reduction: Reduction = 2^LayerNr
    // Layer:     0,  1,  2, ... MaxLayer
    // Reduction: 1,  2,  4, ... MaxReduction = 2^MaxLayer

    LSA_UINT32                        LastLayer_SingleRcw;            // Last Layer of Tree with a single RCW.
    LSA_UINT32                        LastLayer_MultiRcw_NoLimit;     // Last Layer of Tree with multiple RCWs without Phase-Limitation.
    LSA_UINT32                        MaxLayer;                       // MaxLayer.

    LSA_UINT32                        LastReduction_MultiRcw_NoLimit; // Last Reduction of Tree with multiple RCWs without Phase-Limitation.

} EDDI_TREE_RCW_SPECIFIC_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_TREE_RCW_H


/*****************************************************************************/
/*  end of file eddi_ser_tree_rcw.h                                          */
/*****************************************************************************/

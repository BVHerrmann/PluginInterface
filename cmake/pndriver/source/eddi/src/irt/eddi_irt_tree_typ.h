#ifndef EDDI_IRT_TREE_TYP_H     //reinclude-protection
#define EDDI_IRT_TREE_TYP_H

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
/*  F i l e               &F: eddi_irt_tree_typ.h                       :F&  */
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

//new irt-reduction-structure
typedef struct _EDDI_IRT_REDUCTION_AX_TYPE
{
    EDDI_MEM_BUF_EL_H                               *   pEOLMemHeader;
    EDDI_SER_EOL_PTR_TYPE                               pEOL;
    LSA_UINT32                                          KramSizeEOL;

    EDDI_MEM_BUF_EL_H                               *   pNOOPMemHeader;
    EDDI_LOCAL_NOOP_PTR_TYPE                            pNOOPBase;
    LSA_UINT32                                          KramSizeNOOP;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    EDDI_MEM_BUF_EL_H                               *   pIrtPortStartTimeTxBlockMemHeader;
    EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE      pIrtPortStartTimeTxBlock;
    LSA_UINT32                                          KramSizeIrtPortStartTimeTxBlock;

    EDDI_MEM_BUF_EL_H                               *   pIrtPortStartTimeRxBlockMemHeader;
    EDDI_SER_IRT_PORT_START_TIME_RX_BLOCK_PTR_TYPE      pIrtPortStartTimeRxBlock;
    LSA_UINT32                                          KramSizeIrtPortStartTimeRxBlock;
    #endif

    EDDI_TREE_TYPE                                      IrtRedTreeTx[EDDI_MAX_IRTE_PORT_CNT];  // ReductionTrees for IRT-TX
                                                                                               // Index = HwPortIndex!
    EDDI_TREE_TYPE                                      IrtRedTreeRx[EDDI_MAX_IRTE_PORT_CNT];  // ReductionTrees for IRT-RX
                                                                                               // Index = HwPortIndex!

} EDDI_IRT_REDUCTION_AX_TYPE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IRT_TREE_TYP_H


/*****************************************************************************/
/*  end of file eddi_irt_tree_typ.h                                          */
/*****************************************************************************/

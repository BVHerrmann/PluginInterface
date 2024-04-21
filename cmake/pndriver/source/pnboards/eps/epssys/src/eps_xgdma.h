#ifndef EPS_XGDMA_H_
#define EPS_XGDMA_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_xgdma.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS XGDMA Driver                                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

#if (EPS_PLF == EPS_PLF_PNIP_A53)
LSA_RESULT  eps_xgdma_init(LSA_UINT32 xGdmaRegBase);
LSA_VOID	eps_xgdma_undo_init(LSA_VOID);

LSA_VOID    eps_xgdma_hif_memcpy(LSA_VOID* destCpy, LSA_VOID* srcCpy, LSA_UINT32 byteCount, LSA_VOID* destFlag, LSA_UINT32 valueFlag);
LSA_BOOL    eps_xgdma_hif_pend_free(LSA_VOID* pBuffer, LSA_VOID_PTR_TYPE *ppReadyForFree, LSA_UINT32 valueFlag);
#endif


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* EPS_XGDMA_H_ */

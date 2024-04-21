#ifndef EPS_GDMA_H_
#define EPS_GDMA_H_

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
/*  F i l e               &F: eps_gdma.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS GDMA Driver                                                          */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_VOID* eps_gdma_init(LSA_UINT8* pGdmaRegBase, LSA_UINT8* pGdmaRegBasePhys, LSA_UINT8* pRamVirtBaseSrc, LSA_UINT8* pRamPhysBaseSrc, LSA_UINT8* pRamVirtBaseDst, LSA_UINT8* pRamPhysBaseDst);
LSA_VOID   eps_gdma_undo_init(LSA_VOID* hGdma);
LSA_UINT32 eps_gdma_single_hif_transfer_init(LSA_VOID* hGdma, LSA_UINT16 uHdNr, LSA_BOOL bUpper);
LSA_VOID   eps_gdma_single_hif_transfer(LSA_VOID* hGdma, LSA_UINT32 hTransfer, LSA_UINT32 uSourceAddr, LSA_UINT32 uDestAddr, LSA_UINT32 uByteCount);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* EPS_GDMA_H_ */
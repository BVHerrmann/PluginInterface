#ifndef EPS_MEMPOOL_H_
#define EPS_MEMPOOL_H_

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
/*  F i l e               &F: eps_mempool.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Toolbox for creating memory pools                                    */
/*  - currently mem3 is used as memory managment system                      */
/*****************************************************************************/

/*****************************************************************************/
/*  typedefines                                                              */
/*****************************************************************************/

typedef LSA_VOID *  (*pMemSet_fp)                (LSA_VOID *, LSA_INT, LSA_UINT); /* MEM3_POOL_MEMSET_FUNCTION              */

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_RESPONSE_TYPE eps_mempool_create(LSA_UINT16* pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_fp mem_set);
LSA_VOID_PTR_TYPE eps_mempool_alloc(LSA_UINT16 pool_id, LSA_UINT32 size);
LSA_UINT16        eps_mempool_free(LSA_UINT16 pool_id, LSA_VOID_PTR_TYPE ptr);
LSA_RESPONSE_TYPE eps_mempool_delete(LSA_UINT16 pool_id);

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_MEMPOOL_H_ */

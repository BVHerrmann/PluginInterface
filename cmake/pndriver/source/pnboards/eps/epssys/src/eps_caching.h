#ifndef EPS_CACHING_H_
#define EPS_CACHING_H_

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
/*  F i l e               &F: eps_caching.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Cache                                                                */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef struct eps_cache_mem_info_tag
{
    LSA_VOID * address;
    LSA_UINT32 len;
} EPS_CACHE_MEM_INFO_TYPE, *EPS_CACHE_MEM_INFO_PTR_TYPE;
    
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

/* Basic initialisation */
LSA_VOID    eps_cache_install       (LSA_VOID);
LSA_VOID    eps_cache_uninstall     (LSA_VOID);

/* Cache synchronisation */
LSA_VOID    eps_cache_sync          (LSA_VOID * address, LSA_UINT32 len);
LSA_VOID    eps_cache_inv           (LSA_VOID * address, LSA_UINT32 len);
LSA_VOID    eps_cache_wb            (LSA_VOID * address, LSA_UINT32 len);
LSA_VOID    eps_cache_wb_inv        (LSA_VOID * address, LSA_UINT32 len);
LSA_INT		eps_cache_cmp_uncached  (LSA_VOID * address, LSA_UINT32 len);


#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_CACHING_H_ */

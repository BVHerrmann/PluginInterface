#ifndef PSI_INC_MEM_H               /* ----- reinclude-protection ----- */
#define PSI_INC_MEM_H

#ifdef __cplusplus              /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: psi_inc_mem.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Psi Alloc mem macros                                                     */
/*                                                                           */
/*****************************************************************************/

/*- inline implementation --------------------------------------------------*/

LSA_VOID_PTR_TYPE eps_mem_alloc_internal(LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_UINT16 line, LSA_CHAR* file);
LSA_UINT16 eps_mem_free(LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type);

//#include "eps_mem.h"
    /**
    * Inline implementation of PSI_ALLOC_LOCAL_MEM as a macro. We use this macro implementation since we want to use the eps statistics module.
    * With the macro we may use __line__ and __file__ to get the actual source file and line of code where the allocation happens.
    * If a real function is used, we would only get the line of code within psi_cfg.c in the function PSI_ALLOC_LOCAL_MEM.
    */
#define PSI_ALLOC_LOCAL_MEM(mem_ptr_ptr, user_id, length, sys_ptr, comp_id, mem_type) \
    {\
    LSA_UNUSED_ARG(user_id); \
    LSA_UNUSED_ARG(sys_ptr); \
    *mem_ptr_ptr = eps_mem_alloc_internal(length, comp_id, mem_type, (LSA_UINT16) __LINE__, (LSA_CHAR*) __FILE__); \
    }


    /*===========================================================================*/
    /*                         Local memory                                      */
    /*===========================================================================*/

#define PSI_FREE_LOCAL_MEM(ret_val_ptr, mem_ptr, sys_ptr, comp_id, mem_type) \
    { \
    LSA_UNUSED_ARG(sys_ptr); \
    *ret_val_ptr = eps_mem_free(mem_ptr, comp_id, mem_type); \
    }



#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif   /* of PSI_INC_MEM_H */

#ifndef EPS_MEM_H_
#define EPS_MEM_H_

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
/*  F i l e               &F: eps_mem.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Memory                                                               */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_LSA_COMP_ID_LAST      PSI_COMP_ID_LAST              /* Max Comp Id used by Mem Statistic */
#define EPS_LSA_COMP_ID_SUM       PSI_COMP_ID_LAST+1            /* Last Element of the Mem Statistics Array; used to accumulate all values */

#define EPS_LSA_COUNT_MEM_TYPES   PSI_MTYPE_MAX                 /* Max Mem Type */
#define EPS_LSA_MEM_TYPES_SUM     EPS_LSA_COUNT_MEM_TYPES+1     /* Last Element of the Mem Statistics Array; used to accumulate all values */

#define EPS_MEM_MAX_USER_POOLS    5                             /* 5 userpools, that eps user (APP) may use with tool functions. Adapt Enums in EPS_CP_POOL_TYPE.
                                                                   Important: Update POOL_NUM in mem3cfg.h whenever this define is changed */

/*****************************************************************************/
/*  Enums                                                                    */
/*****************************************************************************/
typedef enum eps_cp_pool_type_enum {    // pool types for each HD
    POOL_UNKNOWN               = 0x00,  // reserved for invalid
    EPS_LOCAL_MEMORY           = 0x01,  // reserved pool id for local mem (see eps_mem.c)
    EPS_FAST_MEMORY            = 0x02,  // reserved pool id for fast emm (see eps_mem.c)
    //HIF_POOL                 = 0x03,  // HIF pool => no longer required 
    DEV_POOL                   = 0x04,  // DEV pool
    NRT_TX_POOL                = 0x05,  // NRT TX pool
    NRT_RX_POOL                = 0x06,  // NRT RX pool
    NRT_POOL_DMACW_DEFAULT     = 0x07,  // NRT pool
    NRT_POOL_TX_LOW_FRAG       = 0x08,  // NRT pool
    NRT_POOL_TX_MID_FRAG       = 0x09,  // NRT pool
    NRT_POOL_TX_HIGH_FRAG      = 0x0A,  // NRT pool
    NRT_POOL_TX_MGMTLOW_FRAG   = 0x0B,  // NRT pool
    NRT_POOL_TX_MGMTHIGH_FRAG  = 0x0C,  // NRT pool
    PI_POOL                    = 0x0D,  // PI pool
    TCIP_PAGE_POOL             = 0x0E,  // TCIP memory pool (used by TCIP_ALLOC_LOCAL_MEM())
    OBSD_PAGE_POOL             = 0x0F,  //lint -esym(749, eps_cp_pool_type_enum::OBSD_PAGE_POOL) TCIP OpenBSD Page memory pool (used by PSI_OBSD_ALLOC_MEM()) - only for OBSD variants
    CRT_SLOW_POOL              = 0x10,  // CRT slow pool
    POOL_NR_MAX,                         // !!! IMPORTANT !!!: Update POOL_NUM in mem3cfg.h whenever this enum is modified.

    // User pools. They may be created and deleted from the Application using EPS
    EPS_USER_POOL1             = ((POOL_NR_MAX) * (1 /*LD*/ + 4 /*HDs*/)) + 1, // first user pool of EPS_MEM_MAX_USER_POOLS
    EPS_USER_POOL2             = ((POOL_NR_MAX) * (1 /*LD*/ + 4 /*HDs*/)) + 2, // first user pool of EPS_MEM_MAX_USER_POOLS
    EPS_USER_POOL3             = ((POOL_NR_MAX) * (1 /*LD*/ + 4 /*HDs*/)) + 3, // first user pool of EPS_MEM_MAX_USER_POOLS
    EPS_USER_POOL4             = ((POOL_NR_MAX) * (1 /*LD*/ + 4 /*HDs*/)) + 4, // first user pool of EPS_MEM_MAX_USER_POOLS
    EPS_USER_POOL5             = ((POOL_NR_MAX) * (1 /*LD*/ + 4 /*HDs*/)) + 5  // first user pool of EPS_MEM_MAX_USER_POOLS
} EPS_CP_POOL_TYPE;

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef struct eps_mem_statistic_prefix_tag *EPS_MEM_STATISTIC_PREFIX_PTR_TYPE;

typedef struct eps_mem_statistic_prefix_tag
{
	LSA_UINT32 magic;                       // magic value, check for memory overwrite
	LSA_UINT32 comp_id;                     // storage of the component id, see lsa_sys.h
	LSA_UINT32 mem_type;                    // storage of the memory type, see psi_sys.h
	LSA_INT   pool;                         // number of the pool
	LSA_UINT32 size;                        // size of the block
	LSA_UINT32 isPrefix;                    // 1: prefix, 0: postfix
    LSA_UINT32 line;                        // linenumber where the block was allocated
    LSA_CHAR*  file;                        // pointer to a string with the filename
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE next; // pointer to the next entry
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prev; // pointer to the previous entry
} EPS_MEM_STATISTIC_PREFIX_TYPE;

typedef struct
{
	LSA_UINT32  uAllocCount;                // number of all allocs (not subtracted if free is called)
	LSA_UINT32  uActAllocCount;             // current number of allocated blocks
	LSA_UINT32  uActAllocSize;              // size of allocated blocks in byte
	LSA_UINT32  uMaxAllocSize;              // maximum size of allocated blocks since begin of statistic
	LSA_UINT32  uMinAllocSizeElement;       // size of the smallest block
	LSA_UINT32  uMaxAllocSizeElement;       // size of the biggest block
	LSA_UINT32  uSumAllocSize;              // sum of all allcs in byte (not subtracted if free is called)
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE first;
} EPS_MEM_STATISTIC_TYPE, *EPS_MEM_STATISTIC_PTR_TYPE;
/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_RESPONSE_TYPE eps_mem_init ( LSA_UINT32* pStdMemBase, LSA_UINT32 uStdMemSize, LSA_UINT32* pFastMemBase, LSA_UINT32 uFastMemSize );
LSA_VOID          eps_mem_undo_init ( LSA_VOID );    

LSA_VOID_PTR_TYPE eps_mem_alloc_from_pool( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool, LSA_UINT16 line, LSA_CHAR* file );
LSA_UINT16        eps_mem_free_from_pool ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool );

LSA_VOID_PTR_TYPE eps_mem_alloc_internal( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_UINT16 line, LSA_CHAR* file );

/**
* This is a macro definition extended to eps_mem_alloc_internal. 
* Function signature is           LSA_VOID_PTR_TYPE eps_mem_alloc( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type)
* Extended function signature is  LSA_VOID_PTR_TYPE eps_mem_alloc_internal( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_UINT16 line, LSA_CHAR* file )
*/
#define eps_mem_alloc(size_ , lsa_comp_id_ , mem_type_) \
    eps_mem_alloc_internal(size_, lsa_comp_id_, mem_type_, (LSA_UINT16) __LINE__, (LSA_CHAR*) __FILE__)

LSA_UINT16        eps_mem_free ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type );

LSA_VOID_PTR_TYPE eps_fast_mem_alloc ( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type );
LSA_UINT16        eps_fast_mem_free ( LSA_VOID_PTR_TYPE ptr );

LSA_VOID          eps_mem_lock  ( LSA_VOID );
LSA_VOID          eps_mem_unlock( LSA_VOID );

LSA_BOOL          eps_mem_verify_buffer (LSA_VOID_PTR_TYPE pBuf);
LSA_BOOL          eps_mem_check_pool ();

EPS_MEM_STATISTIC_TYPE eps_mem_statistics_get_single_statistic(LSA_UINT16 compId, LSA_UINT16 memType);
LSA_VOID          eps_mem_statistics_print_pool_usage( void );

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_MEM_H_ */

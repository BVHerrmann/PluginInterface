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
/*  F i l e               &F: eps_cp_mem.c                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation for NRT / PI / HIF  CP memory management using MEM3       */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20003
#define EPS_MODULE_ID      20003

#include "eps_sys.h"
#include "eps_cp_mem.h"
#include "eps_trc.h"   // Tracing
#include "mem3if.h"    // MEM3 Interface
#include <eps_mem.h>       /* Memory If               */

/*---------------------------------------------------------------------------*/
/* Defines                                                                   */
/*---------------------------------------------------------------------------*/

#define EPS_MAX_BOARD_NR    (PSI_CFG_MAX_IF_CNT)
#define EPS_MEM_USAGE_MEM3_FFT2BOUND    0
#define EPS_MEM_USAGE_MEM3_UNIVERSAL3   1
#define EPS_MEM_USAGE_HIF_INTERNAL      2

#if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
#include "hif_mem_malloc.h"
#define EPS_DEV_MEM_USAGE       EPS_MEM_USAGE_HIF_INTERNAL
#define EPS_NRT_MEM_USAGE       EPS_MEM_USAGE_HIF_INTERNAL
#define EPS_CRT_SLOW_MEM_USAGE  EPS_MEM_USAGE_MEM3_FFT2BOUND
#else
#define EPS_DEV_MEM_USAGE       EPS_MEM_USAGE_MEM3_FFT2BOUND
#define EPS_NRT_MEM_USAGE       EPS_MEM_USAGE_MEM3_FFT2BOUND
#define EPS_CRT_SLOW_MEM_USAGE  EPS_MEM_USAGE_MEM3_FFT2BOUND
#endif

#if (EPS_DEV_MEM_USAGE != EPS_MEM_USAGE_MEM3_FFT2BOUND) && (EPS_DEV_MEM_USAGE != EPS_MEM_USAGE_MEM3_UNIVERSAL3) && (EPS_DEV_MEM_USAGE != EPS_MEM_USAGE_HIF_INTERNAL )
#error "Invalid config for EPS_DEV_MEM_USAGE"
#endif

#if (EPS_NRT_MEM_USAGE != EPS_MEM_USAGE_MEM3_FFT2BOUND) && (EPS_NRT_MEM_USAGE != EPS_MEM_USAGE_MEM3_UNIVERSAL3) && (EPS_NRT_MEM_USAGE != EPS_MEM_USAGE_HIF_INTERNAL )
#error "Invalid config for EPS_NRT_MEM_USAGE"
#endif
/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/

#define EPS_MEM_ALIGN4(x)   (((x) + 3) & ~3) /* 4 Byte Alignment Macro */

/*---------------------------------------------------------------------------*/
/* Types                                                                     */
/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
typedef struct eps_cp_mem_pool_tag
{
	LSA_UINT8* base_ptr;           // Base address of memory block
	LSA_UINT32 size;               // Size of memory block [bytes]
	LSA_UINT8* help_mem_ptr ;      // pointer to helper memory for data pool
	LSA_INT    pool_nr;            // Creation number for data pool or alias nr
	LSA_INT    alias_nr;           // Alias number for data pool (used only for Fft2Bound pools)
} EPS_CP_MEM_POOL_TYPE, *EPS_CP_MEM_POOL_PTR_TYPE;
typedef EPS_CP_MEM_POOL_TYPE const* EPS_CP_MEM_POOL_CONST_PTR_TYPE;

/*---------------------------------------------------------------------------------*/
/* Note: board_nr = 0 is used for LD pools (only HIF POOL is allowed with this nr) */
typedef struct eps_cp_mem_instance_tag  // type for CP memory instance
{
	LSA_BOOL             is_running;                                    // Initialized yes/no
	EPS_CP_MEM_POOL_TYPE dev_mem[EPS_MAX_BOARD_NR+1];                   // DEV memory pools
	EPS_CP_MEM_POOL_TYPE nrt_tx_mem[EPS_MAX_BOARD_NR+1];                // NRT TX memory pools (NRT buffer)
	EPS_CP_MEM_POOL_TYPE nrt_rx_mem[EPS_MAX_BOARD_NR+1];                // NRT RX memory pools (NRT buffer)
	EPS_CP_MEM_POOL_TYPE nrt_dmacw_default_mem[EPS_MAX_BOARD_NR+1];     // NRT memory pools (NRT buffer for DMACWs)
	EPS_CP_MEM_POOL_TYPE nrt_tx_low_frag_mem[EPS_MAX_BOARD_NR+1];       // NRT memory pools (NRT buffer for TX fragmentation prio low, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mid_frag_mem[EPS_MAX_BOARD_NR+1];       // NRT memory pools (NRT buffer for TX fragmentation prio mid, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_high_frag_mem[EPS_MAX_BOARD_NR+1];      // NRT memory pools (NRT buffer for TX fragmentation prio high, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mgmtlow_frag_mem[EPS_MAX_BOARD_NR+1];   // NRT memory pools (NRT buffer for TX fragmentation prio MGMTLow, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE nrt_tx_mgmthigh_frag_mem[EPS_MAX_BOARD_NR+1];  // NRT memory pools (NRT buffer for TX fragmentation prio MGMTHigh, used only by EDDI SOC)
	EPS_CP_MEM_POOL_TYPE pi_mem[EPS_MAX_BOARD_NR+1];                    // ProcessImage memory pool (Provider/Consumer)
	EPS_CP_MEM_POOL_TYPE tcip_mem;                                      // TCIP memory pool (used by TCIP_ALLOC_LOCAL_MEM())
	EPS_CP_MEM_POOL_TYPE obsd_mem;                                      // TCIP OpenBSD Page memory pool (used by PSI_OBSD_ALLOC_MEM())
	EPS_CP_MEM_POOL_TYPE crt_slow_mem[EPS_MAX_BOARD_NR+1];              // CRT slow memory pools
} EPS_CP_MEM_INSTANCE_TYPE;

static EPS_CP_MEM_INSTANCE_TYPE g_Eps_pools = {0}; // the CP mem instance for max CPs

#if (EPS_PLF != EPS_PLF_SOC_MIPS)
/* Device memory pool not used for SOC1 board */
static LSA_UINT8 * eps_cp_mem_dev_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );
static LSA_VOID eps_cp_mem_dev_free_by_poolptr( LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );
#endif

static LSA_UINT8 * eps_cp_mem_nrt_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id, EPS_CP_POOL_TYPE eps_cp_pool_type );
static LSA_VOID eps_cp_mem_nrt_free_by_poolptr( LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );

#if (EPS_CFG_USE_IOH == 1)
/* CRT-Pool is only used in IOH (LD component) */
static LSA_UINT8 * eps_cp_mem_crt_slow_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );
static LSA_VOID eps_cp_mem_crt_slow_free_by_poolptr( LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id );
#endif

/*----------------------------------------------------------------------------*/
/* This is to create an valid and defined return address for an alloc(0)      */
/*----------------------------------------------------------------------------*/
static  LSA_UINT8              eps_cp_mem_zero_size_ptr_var = 0;
#define EPS_CP_MEM_ZERO_SIZE_PTR (&eps_cp_mem_zero_size_ptr_var)

/*----------------------------------------------------------------------------*/
/**
 * @brief Intializes a pool struture
 * @param [inout] pCpMem pointer to the pool struture
 * @return
 */
static LSA_VOID eps_cp_mem_init_pool_struct(EPS_CP_MEM_POOL_PTR_TYPE const pCpMem)
{
	pCpMem->base_ptr     = 0;
	pCpMem->size         = 0;
	pCpMem->help_mem_ptr = 0;
	pCpMem->pool_nr      = POOL_UNKNOWN;
	pCpMem->alias_nr     = POOL_UNKNOWN;
}

/*----------------------------------------------------------------------------*/
/**
 * Initializes the EPS CP mem module
 * @see eps_cp_mem_init_pool_struct - Calls eps_cp_mem_init_pool_struct for each pool type
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_cp_mem_init(LSA_VOID)
{
	LSA_UINT16 idx;

	for ( idx = 0; idx <= EPS_MAX_BOARD_NR; idx++ ) // Execute for all CPs
	{
		// initialize instance pool information for NRT for each possible PN Board
		eps_cp_mem_init_pool_struct( &g_Eps_pools.dev_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_rx_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_dmacw_default_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_low_frag_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_mid_frag_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_high_frag_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_mgmtlow_frag_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.nrt_tx_mgmthigh_frag_mem[idx] );
		eps_cp_mem_init_pool_struct( &g_Eps_pools.crt_slow_mem[idx] );

		// initialize instance pool information for PI for each possible PN Board
		eps_cp_mem_init_pool_struct( &g_Eps_pools.pi_mem[idx] );
	}

	// initialize instance pool information for TCIP
	eps_cp_mem_init_pool_struct( &g_Eps_pools.tcip_mem );

	// initialize instance pool information for TCIP OpenBSD
	eps_cp_mem_init_pool_struct( &g_Eps_pools.obsd_mem );

	g_Eps_pools.is_running = LSA_TRUE;

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_init(): Mem pool init done, max boards(%u)",
		EPS_MAX_BOARD_NR );
}

/*----------------------------------------------------------------------------*/
/**
 * Undoes the initialization for the mempools.
 * Checks if the pools have been initialized
 * @param LSA_VOID
 * @return
 */
LSA_VOID eps_cp_mem_undo_init(LSA_VOID)
{
	LSA_UINT16 idx;

    EPS_ASSERT(LSA_TRUE == g_Eps_pools.is_running);
	// All pools freed?

	for ( idx = 0 ; idx <= EPS_MAX_BOARD_NR ; idx++ ) // Check for all CPs
	{
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.dev_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_rx_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_dmacw_default_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_low_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_mid_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_high_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_mgmtlow_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.nrt_tx_mgmthigh_frag_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.crt_slow_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.pi_mem[idx].pool_nr );
		EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.pi_mem[idx].alias_nr );
	}

	EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.tcip_mem.pool_nr );
	EPS_ASSERT( POOL_UNKNOWN == g_Eps_pools.obsd_mem.pool_nr );

	g_Eps_pools.is_running = LSA_FALSE;

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_undo_init(): Mem pool cleanup done, max boards(%u)", 
		EPS_MAX_BOARD_NR );
}

/*----------------------------------------------------------------------------*/
#if(EPS_PLF != EPS_PLF_SOC_MIPS)
/**
 * Create the pool for PN board (CP) with number 
 * The pool is created by checking the define EPS_DEV_MEM_USAGE
 * - EPS_MEM_USAGE_MEM3_FFT2BOUND  - Uses mem3 with Fit First to bound (generic, see mem3 docu)
 * - EPS_MEM_USAGE_MEM3_UNIVERSAL3 - Uses mem3 with Universal3 (generic, see mem3 docu)
 * - EPS_MEM_USAGE_HIF_INTERNAL    - Uses hif_mem_alloc (generic)
 * 
 * @param [in] board_nr - used as index
 * @param [in] base_ptr - base pointer to the memory pool
 * @param [in] size     - size of the pool
 * @return pool handle from mem3
 */
LSA_INT eps_cp_mem_create_dev_pool(LSA_UINT16 board_nr, LSA_UINT8 * base_ptr, LSA_UINT32 size)
{
	LSA_INT  pool_id;
    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	ORDHW    gran_size, fftThreshold, helperSize;
	eMemType mem3pooltype;

	// create MEM3 pool for DEV memory (SDRAM)
	// Note: we use the PN Board number as handle for the DEV pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;
    #endif

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( base_ptr != 0 );
	EPS_ASSERT( size > 0 );

	pool_ptr = &g_Eps_pools.dev_mem[board_nr];

	// the pool number for creation is set based on HD-Nr and type of pool
	// Note: necessary to handle multiple pools of same type in MEM3
	// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
	pool_id = DEV_POOL + (board_nr*POOL_NR_MAX);

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND)
	mem3pooltype = Fft2Bound;
	gran_size    = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;
    #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	mem3pooltype = Universal3;
	gran_size    = 1024; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 1024;
    #endif

    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	helperSize = PoolHelperSize3( size, gran_size, mem3pooltype, LSA_TRUE, 8 );
	pool_ptr->help_mem_ptr = (LSA_UINT8 *)eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_dev_pool(): boardNr(%u) pool helpPtr(0x%08x) helperSize(%#x)",
		board_nr,
		pool_ptr->help_mem_ptr,
		helperSize );

	// Note: assignment is necessary because ptr are modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, eps_memset );   // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE ); // enable overwrite check
	SetPoolAlignment( pool_id, 8 );         // 64 bit alignment (see eddi_dev.c)

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( &pBaseMem, &pHelpMem, size, helperSize, gran_size, fftThreshold, mem3pooltype, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );
    #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	pool_ptr->help_mem_ptr = hif_mem_malloc_init( base_ptr, size, NULL );
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_dev_pool(): boardNr(%u) pool_id(%u) MEM3 pool_nr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->base_ptr, size );

	return ( (LSA_INT)board_nr );
}

/**
 * Deletes the dev_mem for the given board_nr.
 * @param [in] board_nr         - used as index
 * @param [in] pool_handle      - handle to the pool (@see eps_cp_mem_create_dev_pool)
 * @return
 */
LSA_VOID eps_cp_mem_delete_dev_pool(LSA_UINT16 board_nr, LSA_INT pool_handle)
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

#if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	int result ;
    LSA_UINT16 retVal;
#endif

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( pool_handle == board_nr );

	pool_ptr = &g_Eps_pools.dev_mem[board_nr];

    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_dev_pool(): board_nr(%u) pool_nr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 ) // all memory blocks freed ?
	{
		result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

        // check for overwrite
        if ( !eps_mem_verify_buffer( pool_ptr->help_mem_ptr ) )
        {
            // buffer is not ok!
            EPS_FATAL( "eps_cp_mem_delete_dev_pool(): MEM3 DEV pool, but check for overwrite is not ok" );
        }

        retVal = eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
        EPS_ASSERT( LSA_RET_OK == retVal );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// Not all buffers freed		
		EPS_FATAL( "eps_cp_mem_delete_dev_pool(): MEM3 DEV pool, but not all DEV buffers deleted" );
	}
    #elif (EPS_DEV_MEM_USAGE == EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	hif_mem_malloc_undo_init( pool_ptr->help_mem_ptr );

	// reset instance pool information
    eps_cp_mem_init_pool_struct( pool_ptr );
    #endif
}

/**
 * Allocates a memory block from the given pool
 * @param [in] size             - Size of the block
 * @param [in] pool_handle      - Handle to the pool
 * @param [in] lsa_comp_id      - component id (see lsa_sys.h)
 * @throws EPS_ASSERT if pool_handle is invalid
 * 
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_dev_alloc(LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id)
{
	LSA_VOID_PTR_TYPE           ptr;
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.dev_mem[pool_handle];

    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	ptr = eps_cp_mem_dev_alloc_by_poolptr( size, pool_handle, pool_ptr, lsa_comp_id );

	return ptr;
}

/**
 * Frees a memory block
 * @param [in] ptr              - Pointer to the memory that is freed 
 * @param [in] pool_handle      - Handle to the pool
 * @param [in] lsa_comp_id      - component id (see lsa_sys)
 * @throws EPS_ASSERT if the pool handle is invalid
 * @throws EPS_ASSERT if the pointer is nullT
 * @return
 */
LSA_VOID eps_cp_mem_dev_free(LSA_UINT8 * ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id)
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.dev_mem[pool_handle];

    #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif
	eps_cp_mem_dev_free_by_poolptr( ptr, pool_handle, pool_ptr, lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates memory using the underlying memory management by either calling mem3 or hif_mem_alloc (@see eps_cp_mem_create_dev_pool)
 * If the allocation fails, a trace entry is added. This function will not call EPS_FATAL if the allocation failes 
 * -> the LSA component shall handle failed allocs by themselves
 * @param [in] size             - Size of the block
 * @param [in] pool_handle      - Handle to the pool
 * @param [in] pool_ptr         - Pointer to the management struture
 * @param [in] lsa_comp_id      - component id (see lsa_sys.h)
 * @throws EPS_ASSERT if pool_handle is invalid
 * @throws EPS_FATAL if the size is 0 (Alloc(0) is undefined)
 * @return Pointer to the memory block
 */
static LSA_UINT8 * eps_cp_mem_dev_alloc_by_poolptr(LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id)
{
	LSA_VOID_PTR_TYPE ptr;

	LSA_UNUSED_ARG( lsa_comp_id );

    EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;
        
        EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_dev_alloc_by_poolptr: Alloc(0) - size(%u) pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
            size, pool_handle, pool_ptr, lsa_comp_id );        
    }
    else
    {  
        #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        ptr = Mem3palloc( size, pool_ptr->pool_nr );
        #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        ptr = hif_mem_malloc( size, pool_ptr->help_mem_ptr );
        #endif
    
        // Do NOT call EPS_FATAL here or check with EPS_ASSERT when allocation fails
        if (ptr == LSA_NULL)
        {	
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_dev_alloc_by_poolptr(): Allocation returns 0-ptr!!! size(%u)!!! pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
                size, pool_handle, pool_ptr, lsa_comp_id );
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees a memory block using the underlying memory management by either calling mem3 or hif_mem_alloc (@see eps_cp_mem_create_dev_pool)
 * @param [in] ptr              - pointer to the memory block
 * @param [in] pool_handle      - Handle to the pool
 * @param [in] pool_ptr         - Pointer to the management struture
 * @param [in] lsa_comp_id      - component id (see lsa_sys.h)
 * @return
 */
static LSA_VOID eps_cp_mem_dev_free_by_poolptr(LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id)
{
	LSA_UNUSED_ARG( lsa_comp_id );

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

	if(ptr == EPS_CP_MEM_ZERO_SIZE_PTR)
	{
        EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_dev_free_by_poolptr: free(0) - pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
            pool_handle, pool_ptr, lsa_comp_id );   
	}
	else
	{
        #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        #elif (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        hif_mem_free( ptr, pool_ptr->help_mem_ptr );
        #endif
    
        #if (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_DEV_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
        {
            Mem3free( ptr );
        }
        else
        {
            // free of an invalid mem block --> dump statistic and break		
            EPS_FATAL( "eps_cp_mem_dev_free_by_poolptr(): MEM3 DEV memory free failed" );
        }
        #endif
	}
}
#endif //(EPS_PLF != EPS_PLF_SOC_MIPS)

/**
 * Creates the memory pool for the acyclic communication (Non Real Time). 
 * This pool is used by the EDD and all LSA-Components that want to sent acyclic frames (zero copy interface)
 * 
 * @param [in] board_nr         - used as index
 * @param [in] base_ptr         - base pointer to the pool
 * @param [in] size             - size of the pool
 * @param [in] cp_mem_nrt_type  - memory type, see defines to PSI_HD_NRT_XXX
 * @throws EPS_ASSERT if the pool allocation failes
 * @throws EPS_ASSERT if the size is 0
 * @throws EPS_ASSERT if the base pointer is NULL
 * @throws EPS_ASSERT if the board index is invalid
 * @return the pool handle
 * @todo: board_nr is returned instead of pool_ptr->pool_nr
 */
LSA_INT eps_cp_mem_create_nrt_pool( LSA_UINT16 board_nr, LSA_UINT8 * base_ptr, LSA_UINT32 size, LSA_UINT8 cp_mem_nrt_type )
{
	LSA_INT  pool_id;
    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	ORDHW    gran_size, fftThreshold, helperSize;
	eMemType mem3pooltype;

	// create MEM3 pool for NRT memory (Shared mem or SDRAM)
	// Note: we use the PN Board number as handle for the NRT pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;
    #endif

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( base_ptr != 0 );
	EPS_ASSERT( size > 0 );

	switch (cp_mem_nrt_type)
	{
		case PSI_HD_NRT_TX_MEM:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mem[board_nr];

			// the pool number for creation is set based on HD-Nr and type of pool.
			// Note: necessary to handle multiple pools of same type in MEM3
			// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4,5,6 for HD with HD-NR=1, ...
			pool_id = NRT_TX_POOL + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_RX_MEM:
		{
			pool_ptr = &g_Eps_pools.nrt_rx_mem[board_nr];

			// the pool number for creation is set based on HD-Nr and type of pool.
			// Note: necessary to handle multiple pools of same type in MEM3
			// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4,5,6 for HD with HD-NR=1, ...
			pool_id = NRT_RX_POOL + (board_nr*POOL_NR_MAX);
			break;
		}
        #if (PSI_CFG_USE_EDDI == 1)
		case PSI_HD_NRT_EDDI_DMACW_DEFAULT:
		{
			pool_ptr = &g_Eps_pools.nrt_dmacw_default_mem[board_nr];
			pool_id  = NRT_POOL_DMACW_DEFAULT + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_EDDI_TX_LOW_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_low_frag_mem[board_nr];
			pool_id  = NRT_POOL_TX_LOW_FRAG + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MID_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mid_frag_mem[board_nr];
			pool_id  = NRT_POOL_TX_MID_FRAG + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_EDDI_TX_HIGH_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_high_frag_mem[board_nr];
			pool_id  = NRT_POOL_TX_HIGH_FRAG + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mgmtlow_frag_mem[board_nr];
			pool_id  = NRT_POOL_TX_MGMTLOW_FRAG + (board_nr*POOL_NR_MAX);
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mgmthigh_frag_mem[board_nr];
			pool_id  = NRT_POOL_TX_MGMTHIGH_FRAG + (board_nr*POOL_NR_MAX);
			break;
		}
        #endif // PSI_CFG_USE_EDDI
		default:
		{
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_FATAL,
                "eps_cp_mem_create_nrt_pool(): invalid cp_mem_nrt_type(%u) board_nr(%u) base_ptr(0x%08x) size(%u)",
                cp_mem_nrt_type, board_nr, base_ptr, size );
            EPS_FATAL( "eps_cp_mem_create_nrt_pool(): invalid cp_mem_nrt_type" );

            //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
            return 0;
		}
	}

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND)
	mem3pooltype = Fft2Bound;
	gran_size    = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	mem3pooltype = Universal3;
	gran_size    = 1024; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 1024;
    #endif

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	helperSize = PoolHelperSize3( size, gran_size, mem3pooltype, LSA_TRUE, 8 );
	pool_ptr->help_mem_ptr = (LSA_UINT8 *)eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_nrt_pool(): boardNr(%u) pool helpPtr(0x%08x) helperSize(%#x)",
		board_nr,
		pool_ptr->help_mem_ptr,
		helperSize );

	// Note: assignment is necessary because ptr are modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, eps_memset );   // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE ); // enable overwrite check
	SetPoolAlignment( pool_id, 8 );         // 64 bit alignment (see eddi_dev.c)

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( &pBaseMem, &pHelpMem, size, helperSize, gran_size, fftThreshold, mem3pooltype, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	pool_ptr->help_mem_ptr = hif_mem_malloc_init( base_ptr, size, NULL );
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_nrt_pool(): boardNr(%u) pool_id(%u) MEM3 pool_nr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->base_ptr, size );

	return ( (LSA_INT)board_nr );
}

/*----------------------------------------------------------------------------*/
/**
 * Deletes the NRT pool (@see eps_cp_mem_create_nrt_pool)
 * @param [in] board_nr         - used as index
 * @param [in] pool_handle      - handle from eps_cp_mem_create_nrt_pool
 * @param [in] cp_mem_nrt_type  - PSI_HD_NRT_XXX
 * @todo Same todo as eps_cp_mem_create_nrt_pool -> board_nr is used as pool handle, additionally, there is the check EPS_ASSERT( pool_handle == board_nr );
 * @return
 */
LSA_VOID eps_cp_mem_delete_nrt_pool( LSA_UINT16 board_nr, LSA_INT pool_handle, LSA_UINT8 cp_mem_nrt_type )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

#if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	int result;
    LSA_UINT16 retVal;
#endif

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( pool_handle == board_nr );

	switch (cp_mem_nrt_type)
	{
		case PSI_HD_NRT_TX_MEM:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_RX_MEM:
		{
			pool_ptr = &g_Eps_pools.nrt_rx_mem[board_nr];
			break;
		}
        #if (PSI_CFG_USE_EDDI == 1)
		case PSI_HD_NRT_EDDI_DMACW_DEFAULT:
		{
			pool_ptr = &g_Eps_pools.nrt_dmacw_default_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_EDDI_TX_LOW_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_low_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MID_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mid_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_EDDI_TX_HIGH_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_high_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MGMTLOW_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mgmtlow_frag_mem[board_nr];
			break;
		}
		case PSI_HD_NRT_EDDI_TX_MGMTHIGH_FRAG:
		{
			pool_ptr = &g_Eps_pools.nrt_tx_mgmthigh_frag_mem[board_nr];
			break;
		}
        #endif // PSI_CFG_USE_EDDI
		default:
		{
            EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
                "eps_cp_mem_delete_nrt_pool(): invalid cp_mem_nrt_type(%u) board_nr(%u) pool_handle(%u)",
                cp_mem_nrt_type, board_nr, pool_handle );
            EPS_FATAL( "eps_cp_mem_delete_nrt_pool(): invalid cp_mem_nrt_type" );

            //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
            return;
		}
	}

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_nrt_pool(): board_nr(%u) pool_nr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 ) // all memory blocks freed ?
	{
		result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

        // check for overwrite
        if ( !eps_mem_verify_buffer( pool_ptr->help_mem_ptr ) )
        {
            // buffer is not ok!
            EPS_FATAL( "eps_cp_mem_delete_nrt_pool(): MEM3 NRT pool, but check for overwrite is not ok" );
            //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
            return;
        }

		retVal = eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
        EPS_ASSERT( LSA_RET_OK == retVal );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// Not all buffers freed		
		EPS_FATAL( "eps_cp_mem_delete_nrt_pool(): MEM3 NRT pool, but not all NRT buffers deleted" );
	}
    #elif (EPS_NRT_MEM_USAGE == EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	hif_mem_malloc_undo_init( pool_ptr->help_mem_ptr );

	// reset instance pool information
    eps_cp_mem_init_pool_struct( pool_ptr );
#endif
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates tranceive memory for NRT
 * @param [in] size             - size of the requested memory block
 * @param [in] pool_handle      - handle to the pool
 * @param [in] lsa_comp_id      - component ID (see lsa_sys)
 * @return pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_nrt_tx_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE           ptr;
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.nrt_tx_mem[pool_handle];

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	ptr = eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, pool_ptr, lsa_comp_id, NRT_TX_POOL );

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees tranceive memory for NRT
 * @param [in] ptr             - pointer to the memory
 * @param [in] pool_handle     - pool handle
 * @param [in] lsa_comp_id     - component ID (see lsa_sys.h)
 * @return
 */
LSA_VOID eps_cp_mem_nrt_tx_free( LSA_UINT8 * ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.nrt_tx_mem[pool_handle];

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif
	eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, pool_ptr, lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates receive memory for NRT
 * @param [in] size             - size of the requested memory block
 * @param [in] pool_handle      - handle to the pool
 * @param [in] lsa_comp_id      - component ID (see lsa_sys)
 * @return pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_nrt_rx_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE           ptr;
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.nrt_rx_mem[pool_handle];

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	ptr = eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, pool_ptr, lsa_comp_id, NRT_RX_POOL );

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees receive memory for NRT
 * @param [in] ptr             - pointer to the memory
 * @param [in] pool_handle     - pool handle
 * @param [in] lsa_comp_id     - component ID (see lsa_sys.h)
 * @return
 */
LSA_VOID eps_cp_mem_nrt_rx_free( LSA_UINT8 * ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.nrt_rx_mem[pool_handle];

    #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif
	eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, pool_ptr, lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * Generic function to allocate NRT memory. Calls are redirected to mem3 or hif_mem
 * @param [in] size             - size of the requested memory block
 * @param [in] pool_handle      - handle to the pool
 * @param [in] lsa_comp_id      - component ID (see lsa_sys)
 * @param eps_cp_pool_type      - @see eps_cp_pool_type_enum
 * @return pointer to the memory block
 */
static LSA_UINT8 * eps_cp_mem_nrt_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id, EPS_CP_POOL_TYPE eps_cp_pool_type )
{
	LSA_VOID_PTR_TYPE ptr;

    EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;
        
        EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_nrt_alloc_by_poolptr(): Alloc(0) - size(%u) pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u) eps_cp_pool_type(0x%X)",
            size, pool_handle, pool_ptr, lsa_comp_id, eps_cp_pool_type );
    }
    else
    {
        #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        ptr = Mem3palloc( size, pool_ptr->pool_nr );
        #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        ptr = hif_mem_malloc( size, pool_ptr->help_mem_ptr );
        #endif
    
        if (ptr == LSA_NULL)
        {
            EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_nrt_alloc_by_poolptr(): Allocation returns 0-ptr!!! size(%u)!!! pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u) eps_cp_pool_type(0x%X)",
                size, pool_handle, pool_ptr, lsa_comp_id, eps_cp_pool_type );
            LSA_UNUSED_ARG( lsa_comp_id );
            LSA_UNUSED_ARG( eps_cp_pool_type );
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Generic function to free NRT memory. Calls are redirected to mem3 or hif_mem.
 * @param [in] ptr             - pointer to the memory
 * @param [in] pool_handle     - pool handle
 * @param [in] pool_ptr        - pointer to the pool management struture
 * @param [in] lsa_comp_id     - component ID (see lsa_sys.h)
 * @return
 */
static LSA_VOID eps_cp_mem_nrt_free_by_poolptr( LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_UNUSED_ARG( lsa_comp_id );

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );
	
	if(ptr == EPS_CP_MEM_ZERO_SIZE_PTR)
	{
        EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_nrt_free_by_poolptr(): free(0) - pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
            pool_handle, pool_ptr, lsa_comp_id );
	}
	else
	{
        #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        #elif (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        hif_mem_free( ptr, pool_ptr->help_mem_ptr );
        #endif
    
        #if (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_NRT_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
        {
            Mem3free( ptr );
        }
        else
        {
            // free of an invalid mem block --> dump statistic and break
            EPS_FATAL( "eps_cp_mem_nrt_free_by_poolptr(): MEM3 NRT memory free failed" );
        }
        #endif
	}
}

#if (PSI_CFG_USE_EDDI == 1)

/*----------------------------------------------------------------------------*/
// no inline function because of compiler dependencies!
#define EPS_CP_MEM_GET_POOL_HANDLE(pool_handle_, hSysDev_)          \
    LSA_INT                pool_handle_;                            \
    EPS_SYS_PTR_TYPE const pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev_;    \
	EPS_ASSERT( pSysPtr != 0 );                                     \
	EPS_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDI );         \
    pool_handle_ = pSysPtr->hd_nr;

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - For Direct Memory Access IP
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_dmacw_default_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_dmacw_default_mem[pool_handle], lsa_comp_id, NRT_POOL_DMACW_DEFAULT );
}

/*----------------------------------------------------------------------------*/
/**
 * Frees the memory allocated by eps_cp_mem_eddi_nrt_dmacw_default_alloc
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_dmacw_default_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_dmacw_default_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Rx Memory
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_rx_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_rx_mem[pool_handle], lsa_comp_id, NRT_RX_POOL );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific free function
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_rx_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_rx_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Tx Memory
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_mem[pool_handle], lsa_comp_id, NRT_TX_POOL );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_eddi_nrt_tx_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fragmentation memory for low prio frames
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_low_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_low_frag_mem[pool_handle], lsa_comp_id, NRT_POOL_TX_LOW_FRAG );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_eddi_nrt_tx_low_frag_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_low_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fragmentation memory for medium prio frames
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_mid_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_mid_frag_mem[pool_handle], lsa_comp_id, NRT_POOL_TX_MID_FRAG );
}

/*----------------------------------------------------------------------------*/
LSA_VOID eps_cp_mem_eddi_nrt_tx_mid_frag_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_mid_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fragmentation memory for high prio frames
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_high_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_high_frag_mem[pool_handle], lsa_comp_id, NRT_POOL_TX_HIGH_FRAG );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific free function
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_tx_high_frag_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_high_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fragmentation memory for management frames (low)
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_mgmtlow_frag_mem[pool_handle], lsa_comp_id, NRT_POOL_TX_MGMTLOW_FRAG );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific free function
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_tx_mgmtlow_frag_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_mgmtlow_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fragmentation memory for management frames (high)
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    return eps_cp_mem_nrt_alloc_by_poolptr( size, pool_handle, &g_Eps_pools.nrt_tx_mgmthigh_frag_mem[pool_handle], lsa_comp_id, NRT_POOL_TX_MGMTHIGH_FRAG );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific free function
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_tx_mgmthigh_frag_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
    eps_cp_mem_nrt_free_by_poolptr( ptr, pool_handle, &g_Eps_pools.nrt_tx_mgmthigh_frag_mem[pool_handle], lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific allocation functions - Fatal storage memory
 * @param [in] size          - size of the block
 * @param [in] hSysDev       - system handle
 * @param [in] lsa_comp_id   - component id
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_eddi_nrt_fatal_alloc( LSA_UINT32 size, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
		"eps_cp_mem_eddi_nrt_fatal_alloc(): pool_handle(%u) size(0x%08x) lsa_comp_id(%u)", 
		pool_handle, size, lsa_comp_id );
    return 0;
}

/*----------------------------------------------------------------------------*/
/**
 * EDDI specific free function
 * @param [in] ptr          - pointer to the memory block
 * @param [in] hSysDev      - system handle
 * @param [in] lsa_comp_id  - component id
 * @return
 */
LSA_VOID eps_cp_mem_eddi_nrt_fatal_free( LSA_UINT8 * ptr, LSA_SYS_PTR_TYPE hSysDev, LSA_COMP_ID_TYPE lsa_comp_id )
{
    EPS_CP_MEM_GET_POOL_HANDLE( pool_handle, hSysDev );
	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
		"eps_cp_mem_eddi_nrt_fatal_free(): pool_handle(%u) ptr(0x%08x) lsa_comp_id(%u)", 
		pool_handle, ptr, lsa_comp_id );
}

#endif // PSI_CFG_USE_EDDI

#if (EPS_CFG_USE_IOH == 1)
/*----------------------------------------------------------------------------*/
/* MEM3 Pool Functions for PI memory                                          */
/*----------------------------------------------------------------------------*/
/**
 * Creates the Process Image memory pool used for the cyclic communication.
 * Mem3 is used as memory management unit with the strategy Fft2Bound.
 * The PI pool is preset with the value "0". 
 * 
 * @param [in] board_nr     - used as index
 * @param [in] base_ptr     - base pointer of the pool
 * @param [in] size         - size of the pool
 * @param [in] gran_size    - alignment of the allocated blocks
 * @param [in] p_memset_fct - this function is called as memset function
 * @return The pool handle
 */
LSA_INT eps_cp_mem_create_pi_pool( LSA_UINT16     board_nr,
                                   LSA_UINT8 *    base_ptr,
								   LSA_UINT32     size,
								   LSA_UINT32     gran_size,
								   EPS_MEMSET_FCT p_memset_fct ) // special memset function (i.e. on SOC1 boards using PAEA)
{
	LSA_INT pool_id;
	ORDHW   fftThreshold, helperSize;

	// create MEM3 pool for PI memory (KRAM, IOCC, ...)
	// Note: the pool is a Fft2Bound pool with created one for provider and an alias for consumer
	//       we use the PN Board number as handle for the PI pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;

	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.pi_mem[board_nr];

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( base_ptr != 0 );
	EPS_ASSERT( size     >  0 );
	EPS_ASSERT( p_memset_fct != LSA_NULL );

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

	fftThreshold = 0; // Don't care (see MEM3 docu)

	helperSize = PoolHelperSize3( size, gran_size, Fft2Bound, LSA_FALSE, 4 );

	pool_ptr->help_mem_ptr = eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_pi_pool(): board_nr(%u) mem helpPtr(0x%08x) helpSize(%#x) gran_size(%#x)", 
		board_nr, pool_ptr->help_mem_ptr, helperSize, gran_size );

	// Note: assigment is necessary because ptr is modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	// the pool number for creation is set based on HD-Nr and type of pool
	// Note: necessary to handle multiple pools of same type in MEM3
	// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
	pool_id = PI_POOL + (board_nr*POOL_NR_MAX);

	SetPresetVal( pool_id, 0 ); // for PI pool set to 0

	SetMemSetFunc( pool_id, p_memset_fct );  // set special memset function for MEM3 (i.e. on SOC1 boards using PAEA)
	SetCheckOverwrite( pool_id, LSA_FALSE ); // disable overwrite check
	SetPoolAlignment( pool_id, 4 );          // DWord alignment

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( &pBaseMem, &pHelpMem, size, helperSize, gran_size, fftThreshold, Fft2Bound, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );

	// Set the alias for the consumer pool
	pool_ptr->alias_nr = PoolAliasId( pool_ptr->pool_nr ); // AliasID for consumer pool
	EPS_ASSERT( PoolValid( pool_ptr->alias_nr ) );

	EPS_SYSTEM_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_pi_pool(): board_nr(%u) pool_id(%u) provider_pool_nr(%u) consumer_pool_nr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->alias_nr, pool_ptr->base_ptr, size );

	return ( (LSA_INT)board_nr );
}

/*----------------------------------------------------------------------------*/
/**
 * Deletes the PI pool.
 * @param [in] board_nr     - used as index
 * @param [in] pool_handle  - handle to the pool
 * @throws EPS_ASSERT if the pool pointer is invalid
 * @throws EPS_FATAL if there are still buffers in use
 * @return
 */
LSA_VOID eps_cp_mem_delete_pi_pool( LSA_UINT16 board_nr, LSA_INT pool_handle )
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( pool_handle == board_nr );

    pool_ptr = &g_Eps_pools.pi_mem[pool_handle];

	EPS_ASSERT( pool_ptr->pool_nr  != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_pi_pool(): board_nr(%u) pool_nr(%u) alias_nr(%u) helpPtr(0x%08x)", 
		board_nr, pool_ptr->pool_nr, pool_ptr->alias_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool and the helper memory
	if (( BufNumUsedInPool( pool_ptr->pool_nr  ) == 0 ) &&
		( BufNumUsedInPool( pool_ptr->alias_nr ) == 0 ))
	{
		int result = DeletePool( pool_ptr->pool_nr );
        LSA_UINT16 retVal;
		EPS_ASSERT( result == 0 );

		retVal = eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
        EPS_ASSERT( LSA_RET_OK == retVal );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// Not all buffers freed --> dump statistics and break		
		EPS_FATAL( "eps_cp_mem_delete_pi_pool(): MEM3 not all PI buffers deleted" );
	}
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates memory from the PI pool
 * @param [in] size             - size of the memory block
 * @param [in] is_provider      - LSA_TRUE: Memory used for provider, LSA_FALSE: Memory used for consumer
 * @param [in] pool_handle      - handle to the pool
 * @param [in] lsa_comp_id      - component id
 * @return
 */
LSA_UINT8 * eps_cp_mem_pi_alloc( LSA_UINT32       size,
                                 LSA_BOOL         is_provider,
								 LSA_INT          pool_handle,
								 LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_INT                     pool_nr;
	LSA_UINT8                  *ptr;
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	LSA_UNUSED_ARG( lsa_comp_id );

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.pi_mem[pool_handle];

	// Alloc process_image memory from Pool (provider = pool_nr, consumer = alias_nr)
	pool_nr = ( is_provider ) ? pool_ptr->pool_nr : pool_ptr->alias_nr;
	EPS_ASSERT( pool_nr != POOL_UNKNOWN );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;
        
        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_pi_alloc(): Alloc(0) - size(%u) pool_nr(%u)",
            size, pool_nr );
    }
    else
    {
        ptr = Mem3palloc( size, pool_nr );

        if ( ptr != 0 ) // malloc ok ?
        {
            LSA_UINT32 offset;

            LSA_UINT32 allocSize = GetBufSize( ptr );

            // eps_mem_add_statistic( ptr, allocSize, __FILE__, __LINE__, lsa_comp_id, PSI_MTYPE_PI_MEM );
            EPS_ASSERT(ptr >= pool_ptr->base_ptr);
            offset = (LSA_UINT32) (ptr - pool_ptr->base_ptr);

            EPS_SYSTEM_TRACE_07( 0, LSA_TRACE_LEVEL_CHAT,
                "eps_cp_mem_pi_alloc(): pool_handle(%u) ptr(0x%08x) is_provider(%u) offset(%#x) size(%#x) alloc Size(%#x) pool_nr(%u)", 
                pool_handle, ptr, is_provider, offset, size, allocSize, pool_nr );
        }
        else
        {
            // failed
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_pi_alloc(): alloc failed, pool_handle(%u) is_provider(%u) size(%#x) pool_nr(%u)", 
                pool_handle, is_provider, size, pool_nr );
		
            // IOH handles the case to get 0-ptr. NO FATAL!!!
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees a pointer from the PI pool
 * @param [in] p_buffer         - pointer to be freed
 * @param [in] pool_handle      - handle to the pool
 * @param [in] lsa_comp_id      - component id
 * @throws EPS_FATAL if the pointer is not from the given block
 * @return
 */
LSA_VOID eps_cp_mem_pi_free( LSA_UINT8 *      p_buffer,
                             LSA_INT          pool_handle,
							 LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	LSA_UNUSED_ARG(lsa_comp_id);

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.pi_mem[pool_handle];

    EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );

	// eps_mem_remove_statistic( p_buffer, lsa_comp_id, PSI_MTYPE_PI_MEM );

    if (p_buffer == EPS_CP_MEM_ZERO_SIZE_PTR)
    {
        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_pi_free(): free(0) - pool_ptr(0x%08x) pool_nr(%u)",
            pool_ptr, pool_ptr->pool_nr );
    }
    else
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,
            "eps_cp_mem_pi_free(): ptr(0x%08x)", 
            p_buffer );
	
        if ( GetBufSize( p_buffer ) != 0 ) // allocated buffer ?
        {
            Mem3free( p_buffer );
        }
        else
        {
            // it looks like, we have an invalid mem block		
            EPS_FATAL( "eps_cp_mem_pi_free(): MEM3 PI memory free failed" );
        }
    }
}
#endif //(EPS_CFG_USE_IOH == 1)

/*----------------------------------------------------------------------------*/
/* MEM3 Pool Functions for TCIP memory                                        */
/*----------------------------------------------------------------------------*/
#if ( PSI_CFG_USE_TCIP == 1 )
/**
 * Creates a pool used by the TCIP component. 
 * Uses mem3 with the strategy Fft2Bound.
 * @param [in] base_ptr - base pointer of the pool
 * @param [in] size     - size of the pool
 * @return
 */
LSA_VOID eps_cp_mem_create_tcip_pool( LSA_UINT8  * const base_ptr, // Base Ptr of TCIP memory
                                      LSA_UINT32   const size )    // Size of TCIP memory [Bytes]
{
    LSA_INT                  const pool_id      = TCIP_PAGE_POOL;
	LSA_UINT32               const gran_size    = 0x20; // 32 Byte packages (a full frame fits without waste)
	LSA_UINT32               const fftThreshold = 0;
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr     = &g_Eps_pools.tcip_mem;
	LSA_UINT32                     helperSize;
	LSA_VOID                     * pHelpMem;
	LSA_VOID                     * pBaseMem;

	EPS_ASSERT( base_ptr );
	EPS_ASSERT( size );

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

	helperSize = PoolHelperSize3( size, gran_size, Fft2Bound, LSA_TRUE, 8 );
	helperSize = EPS_MEM_ALIGN4( helperSize );
	EPS_ASSERT( helperSize < size );

	pool_ptr->help_mem_ptr = eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE,
	    "eps_cp_mem_create_tcip_pool(): MEM3: base_ptr(0x%08x) help_mem_ptr(0x%08x) size(%#x) helperSize(%#x) gran_size(%#x) pool-size(%#x)",
	    base_ptr, pool_ptr->help_mem_ptr, size, helperSize, gran_size, size );

	// Attention: assigment is necessary because ptr is modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, eps_memset );   // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE ); // enable overwrite check
	SetPoolAlignment( pool_id, 8 );         // DWord alignment

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( (LSA_VOID**)&pBaseMem, (LSA_VOID**)&pHelpMem, size, helperSize, gran_size, fftThreshold, Fft2Bound, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
	    "eps_cp_mem_create_tcip_pool(): pool_id(%u) pool_nr(%u)",
	    pool_id, pool_ptr->pool_nr );
}

/*----------------------------------------------------------------------------*/
/** 
 * Deletes the pool for TCIP
 * @return
 */
LSA_VOID eps_cp_mem_delete_tcip_pool( LSA_VOID )
{
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.tcip_mem;

	EPS_ASSERT( pool_ptr->pool_nr  == TCIP_PAGE_POOL );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
	    "eps_cp_mem_delete_tcip_pool(): pool_nr(%u) help_mem_ptr(0x%08x)",
	    pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the MEM3 memory pool and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 )
	{
		int result = DeletePool( pool_ptr->pool_nr );
        LSA_UINT16 retVal;
		EPS_ASSERT( result == 0 );

		retVal = eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
        EPS_ASSERT( LSA_RET_OK == retVal );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// not all buffers freed => dump statistics and break
		EPS_FATAL( "eps_cp_mem_delete_tcip_pool(): not all MEM3 TCIP buffers deleted" );
	}
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates memory from the TCIP pool
 * @param [in] size - size of the buffer
 * @return Pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_tcip_alloc( LSA_UINT32 size )
{
	LSA_UINT8                    * ptr;
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.tcip_mem;
	LSA_INT                  const pool_nr  = pool_ptr->pool_nr;

	EPS_ASSERT( pool_nr == TCIP_PAGE_POOL );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;

        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_tcip_alloc(): Alloc(0) - size(%u) pool_nr(%u)",
            size, pool_nr );
    }
    else
    {
        ptr = (LSA_UINT8 *)Mem3palloc( size, pool_nr );

        if ( ptr != 0 ) // alloc ok ?
        {
            LSA_UINT32 allocSize = GetBufSize( ptr );
            LSA_UINT32 offset;
            EPS_ASSERT( ptr >= pool_ptr->base_ptr );
            offset = (LSA_UINT32) (ptr - pool_ptr->base_ptr);

            EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT,
                "eps_cp_mem_tcip_alloc(): ptr(0x%08x) offset(%#x) size(%#x) alloc Size(%#x) pool_nr(%u)",
                ptr, offset, size, allocSize, pool_nr );
        }
        else
        {
            // failed
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_tcip_alloc(): alloc failed, size(%#x) pool_nr(%u)",
                size, pool_nr );
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees memory from the TCIP pool
 * @param [in] p_buffer - pointer to the memory block
 * @throws EPS_FATAL if the block is not part of the TCIP pool
 * @return
 */
LSA_VOID eps_cp_mem_tcip_free( LSA_UINT8 * p_buffer )
{
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.tcip_mem;

	EPS_ASSERT( pool_ptr->pool_nr == TCIP_PAGE_POOL );

    if (p_buffer == EPS_CP_MEM_ZERO_SIZE_PTR)
    {
        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_tcip_free(): free(0) - pool_ptr(0x%08x) pool_nr(%u)",
            pool_ptr, pool_ptr->pool_nr );
    }
    else
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,
            "eps_cp_mem_tcip_free(): ptr(0x%08x)", 
            p_buffer );

        if ( GetBufSize( p_buffer ) != 0 ) // allocated buffer ?
        {
            Mem3free( p_buffer );
        }
        else
        {
            // it looks like, we have an invalid mem block
            EPS_FATAL( "eps_cp_mem_tcip_free(): MEM3 memory free failed" );
        }
    }
}

#endif // PSI_CFG_USE_TCIP

/*----------------------------------------------------------------------------*/
/* MEM3 Pool Functions for OpenBSD Page memory                                */
/*----------------------------------------------------------------------------*/
#if ( PSI_CFG_TCIP_STACK_OPEN_BSD == 1 )
/**
 * Creates a pool used by the OpenBSD stack inside the TCIP component. 
 * Uses mem3 with the strategy Fft2Bound. 
 * The blocks have an aligment of 4096 Byte. 
 * Note: Check overwrite is disabled!
 * 
 * @param [in] base_ptr - base pointer of the pool
 * @param [in] size     - size of the pool
 * @return
 */
LSA_VOID eps_cp_mem_create_obsd_pool( LSA_UINT8  * const base_ptr, // Base Ptr of OpenBSD memory
									  LSA_UINT32   const size )    // Size of OpenBSD memory [Bytes]
{
	LSA_INT                  const pool_id   = OBSD_PAGE_POOL;
	LSA_UINT32               const gran_size = 0x1000UL;
    LSA_UINT32                     helperSize;
	LSA_UINT8 *                    pHelpMem;
	LSA_UINT8 *                    pBaseMem;
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr  = &g_Eps_pools.obsd_mem;

	EPS_ASSERT( base_ptr );
	EPS_ASSERT( size );

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

	helperSize = PoolHelperSize3( size, gran_size, Fft2Bound, LSA_FALSE, gran_size );
	helperSize = EPS_MEM_ALIGN4( helperSize );
	EPS_ASSERT( helperSize < size );

	pool_ptr->help_mem_ptr = eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE,
	    "eps_cp_mem_create_obsd_pool(): MEM3: base_ptr(0x%08x) help_mem_ptr(0x%08x) size(%#x) helperSize(%#x) gran_size(%#x) pool-size(%#x)",
	    base_ptr, pool_ptr->help_mem_ptr, size, helperSize, gran_size, size );

	// Note: assigment is necessary because ptr is modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetPresetVal( pool_id, 0 );              // set to 0 for OBSD-Pool
	SetMemSetFunc( pool_id, eps_memset );    // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_FALSE ); // disable overwrite check for OBSD-Pool
	SetPoolAlignment( pool_id, gran_size );

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( (LSA_VOID**)&pBaseMem, (LSA_VOID**)&pHelpMem, size, helperSize, gran_size, 0, Fft2Bound, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_obsd_pool(): pool_id(%u) pool_nr(%u)",
		pool_id, pool_ptr->pool_nr );
}

/*----------------------------------------------------------------------------*/
/** 
 * Deletes the pool for OpenBSD
 * @return
 */
LSA_VOID eps_cp_mem_delete_obsd_pool( LSA_VOID )
{
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.obsd_mem;

	EPS_ASSERT( pool_ptr->pool_nr  == OBSD_PAGE_POOL );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_obsd_pool(): pool_nr(%u) helpPtr(0x%08x)",
		pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the MEM3 memory pool and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 )
	{
		int result = DeletePool( pool_ptr->pool_nr );
		EPS_ASSERT( result == 0 );

		eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// not all buffers freed => dump statistics and break
		EPS_FATAL( "eps_cp_mem_delete_obsd_pool(): not all MEM3 OBSD buffers deleted" );
	}
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates a memory block from the OBSD pool.
 * Note: Blocks are 4k aligned (no check done here).
 * @param [in] size - size of the block
 * @return Pointer to the memory pool
 */
LSA_UINT8 * eps_cp_mem_obsd_alloc( LSA_UINT32 size )
{
	LSA_UINT8                    * ptr;
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.obsd_mem;
	LSA_INT                  const pool_nr  = pool_ptr->pool_nr;

	EPS_ASSERT( pool_nr == OBSD_PAGE_POOL );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;

        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_obsd_alloc(): Alloc(0) - size(%u) pool_nr(%u)",
            size, pool_nr );
    }
    else
    {
        ptr = (LSA_UINT8 *)Mem3palloc( size, pool_nr );

        if ( ptr != 0 ) // alloc ok ?
        {
            LSA_UINT32 allocSize = GetBufSize( ptr );
            LSA_UINT32 offset    = (LSA_UINT32) (ptr - pool_ptr->base_ptr);

            EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT,
                "eps_cp_mem_obsd_alloc(): ptr(0x%08x) offset(%#x) size(%#x) alloc Size(%#x) pool_nr(%u)",
                ptr, offset, size, allocSize, pool_nr );
        }
        else
        {
            // failed
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_obsd_alloc(): alloc failed, size(%#x) pool_nr(%u)",
                size, pool_nr );
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees a buffer from the OBSD pool
 * @param [in] p_buffer - pointer to the buffer
 * @throws EPS_FATAL if the block is not part of the pool
 * @return
 */
LSA_VOID eps_cp_mem_obsd_free( LSA_UINT8 * p_buffer )
{
	EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.obsd_mem;

	EPS_ASSERT( pool_ptr->pool_nr == OBSD_PAGE_POOL );

    if (p_buffer == EPS_CP_MEM_ZERO_SIZE_PTR)
    {
        EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "eps_cp_mem_obsd_free(): free(0) - pool_ptr(0x%08x) pool_nr(%u)",
            pool_ptr, pool_ptr->pool_nr );
    }
    else
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT,
            "eps_cp_mem_obsd_free(): ptr(0x%08x)", 
            p_buffer );
	
        if ( GetBufSize( p_buffer ) != 0 ) // allocated buffer ?
        {
            Mem3free( p_buffer );
        }
        else
        {
            // it looks like, we have an invalid mem block
            EPS_FATAL( "eps_cp_mem_obsd_free(): MEM3 OBSD memory free failed" );
        }
    }
}

#endif // PSI_CFG_TCIP_STACK_OPEN_BSD

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#if (EPS_CFG_USE_IOH == 1) //used for IOH only
/** 
 * memset function for paea of SOC1 
 * 
 * Sets len bytes in memory at dst to val. 
 * 
 * @param [in] hd_nr - number of hardwaredevice
 * @param [in] dst   - startadress of memory
 * @param [in] val   - value the memory is set to
 * @param [in] len   - number of bytes to be changed
 * @return dst
 */
static LSA_VOID * eps_cp_mem_hd_paea_memset( LSA_UINT16 hd_nr, LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len )
{
	EPS_ASSERT( hd_nr && (hd_nr <= EPS_MAX_BOARD_NR) );

    #if ((PSI_CFG_USE_EDDI == 1) && defined (PSI_CFG_EDDI_CFG_SOC)) // Only for SOC1
    if (dst != 0) // buffer allocated ?
    {
        EPS_CP_MEM_POOL_PTR_TYPE const pool_ptr = &g_Eps_pools.pi_mem[hd_nr]; // using base of HD
        LSA_UINT8 *                    base_ptr;
        size_t                         buffer_len = len;
        LSA_UINT8 *                    buffer_ptr;

        base_ptr = pool_ptr->base_ptr;
        EPS_ASSERT(base_ptr != LSA_NULL);

        // calc direct access addr (offset in steps of 4)
        buffer_ptr = (((LSA_UINT32)((LSA_UINT8 *)dst - base_ptr)) << 2U/* * 4 */) + base_ptr;

        // Note: Setting is done over direct access address
        for (; buffer_len != 0; buffer_len--)
        {
            *buffer_ptr = (LSA_UINT8)val;
            buffer_ptr += 4;    //direct access mode: PAEA ram is accessed in steps of 4
        }
	}
    #else
	LSA_UNUSED_ARG( dst );
	LSA_UNUSED_ARG( val );
	LSA_UNUSED_ARG( len );
	EPS_FATAL( "eps_cp_mem_hd_paea_memset(): function can only be used with SOC1 Board." );
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    #endif

	return dst;
}

/*----------------------------------------------------------------------------*/
/**
 * HD specific memset function. @see eps_cp_mem_hd_paea_memset
 * @param [in] dst   - startadress of memory
 * @param [in] val   - value the memory is set to
 * @param [in] len   - number of bytes to be changed
 * @return dst
 */
LSA_VOID * eps_cp_mem_hd1_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len ) // according to MEM3 type pMemSet_ft
{
	return ( eps_cp_mem_hd_paea_memset( 1, dst, val, len ) );
}

/*----------------------------------------------------------------------------*/
/**
 * HD specific memset function. @see eps_cp_mem_hd_paea_memset
 * @param [in] dst   - startadress of memory
 * @param [in] val   - value the memory is set to
 * @param [in] len   - number of bytes to be changed
 * @return dst
 */
LSA_VOID * eps_cp_mem_hd2_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len ) // according to MEM3 type pMemSet_ft
{
	return ( eps_cp_mem_hd_paea_memset( 2, dst, val, len ) );
}

/*----------------------------------------------------------------------------*/
/**
 * HD specific memset function. @see eps_cp_mem_hd_paea_memset
 * @param [in] dst   - startadress of memory
 * @param [in] val   - value the memory is set to
 * @param [in] len   - number of bytes to be changed
 * @return dst
 */
LSA_VOID * eps_cp_mem_hd3_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len ) // according to MEM3 type pMemSet_ft
{
	return ( eps_cp_mem_hd_paea_memset( 3, dst, val, len ) );
}

/*----------------------------------------------------------------------------*/
/**
 * HD specific memset function. @see eps_cp_mem_hd_paea_memset
 * @param [in] dst   - startadress of memory
 * @param [in] val   - value the memory is set to
 * @param [in] len   - number of bytes to be changed
 * @return dst
 */
LSA_VOID * eps_cp_mem_hd4_paea_memset( LSA_VOID_PTR_TYPE dst, LSA_INT val, size_t len ) // according to MEM3 type pMemSet_ft
{
	return ( eps_cp_mem_hd_paea_memset( 4, dst, val, len ) );
}
#endif //(EPS_CFG_USE_IOH == 1)
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
#if (EPS_CFG_USE_IOH == 1) //used in IOH only

/*----------------------------------------------------------------------------*/
/**
 * Creates a pool for non time critical cyclical communication. 
 * @param [in] board_nr - used as index
 * @param [in] base_ptr - base pointer to the index
 * @param [in] size     - size of the pool
 * @param [in] cp_mem_crt_slow - assumed to be PSI_HD_CRT_SLOW_MEM
 */
LSA_INT eps_cp_mem_create_crt_slow_pool( LSA_UINT16 board_nr, LSA_UINT8 * base_ptr, LSA_UINT32 size, LSA_UINT8 cp_mem_crt_slow )
{
	LSA_INT  pool_id;
    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	ORDHW    gran_size, fftThreshold, helperSize;
	eMemType mem3pooltype;

	// create MEM3 pool for CRT slow memory (SDRAM)
	// Note: we use the PN Board number as handle for the CRT slow pool
	LSA_VOID_PTR_TYPE pHelpMem;
	LSA_VOID_PTR_TYPE pBaseMem;
    #endif

	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( base_ptr != 0 );
	EPS_ASSERT( size > 0 );

	switch (cp_mem_crt_slow)
	{
		case PSI_HD_CRT_SLOW_MEM:
		{
			pool_ptr = &g_Eps_pools.crt_slow_mem[board_nr];

			// the pool number for creation is set based on HD-Nr and type of pool
			// Note: necessary to handle multiple pools of same type in MEM3
			// i.e.: Pool 1,2,3 for LD with HD-NR=0, Pool 4, 5,6 for HD with HD-NR=1,....
			pool_id = CRT_SLOW_POOL + (board_nr*POOL_NR_MAX);
			break;
		}
		default:
		{
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_FATAL,
                "eps_cp_mem_create_crt_slow_pool(): invalid cp_mem_crt_slow(%u) board_nr(%u) base_ptr(0x%08x) size(%u)",
                cp_mem_crt_slow, board_nr, base_ptr, size );
            EPS_FATAL( "eps_cp_mem_create_crt_slow_pool(): invalid cp_mem_crt_slow" );

            //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
            return 0;
		}
	}

	EPS_ASSERT( pool_ptr->pool_nr  == POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->alias_nr == POOL_UNKNOWN );

	pool_ptr->base_ptr = base_ptr;
	pool_ptr->size     = size;

    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND)
	mem3pooltype = Fft2Bound;
	gran_size    = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;
    #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	mem3pooltype = Universal3;
	gran_size    = 1024; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 1024;
    #endif

    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	helperSize = PoolHelperSize3( size, gran_size, mem3pooltype, LSA_TRUE, 8 );
	pool_ptr->help_mem_ptr = (LSA_UINT8 *)eps_mem_alloc( helperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_crt_slow_pool(): boardNr(%u) pool helpPtr(0x%08x) helperSize(%#x)",
		board_nr,
		pool_ptr->help_mem_ptr,
		helperSize );

	// Note: assignment is necessary because ptr are modified in MEM3!
	pHelpMem = pool_ptr->help_mem_ptr;
	pBaseMem = pool_ptr->base_ptr;

	SetMemSetFunc( pool_id, eps_memset );   // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE ); // enable overwrite check
	SetPoolAlignment( pool_id, 64 );         // 64 byte alignment (see eddi_dev.c)

	// create MEM3 pool
	pool_ptr->pool_nr = CreatePool( &pBaseMem, &pHelpMem, size, helperSize, gran_size, fftThreshold, mem3pooltype, pool_id );

	EPS_ASSERT( pool_ptr->pool_nr > 0 );
	EPS_ASSERT( PoolValid( pool_ptr->pool_nr ) );
    #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	pool_ptr->help_mem_ptr = hif_mem_malloc_init( base_ptr, size, NULL );
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	EPS_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_create_crt_slow_pool(): boardNr(%u) pool_id(%u) MEM3 pool_nr(%u) basePtr(0x%08x) baseSize(%#x)",
		board_nr, pool_id, pool_ptr->pool_nr, pool_ptr->base_ptr, size );

	return ( (LSA_INT)board_nr );
}

/*----------------------------------------------------------------------------*/
/**
 * Deletes the slow pool
 * @param [in] board_nr         - used as index
 * @param [in] pool_handle      - handle to the pool
 * @param [in] cp_mem_crt_slow  - assumed to be PSI_HD_CRT_SLOW_MEM
 */
LSA_VOID eps_cp_mem_delete_crt_slow_pool( LSA_UINT16 board_nr, LSA_INT pool_handle, LSA_UINT8 cp_mem_crt_slow )
{
	EPS_CP_MEM_POOL_PTR_TYPE pool_ptr;

	EPS_ASSERT( (board_nr > 0) && (board_nr <= EPS_MAX_BOARD_NR) );
	EPS_ASSERT( pool_handle == board_nr );

	switch (cp_mem_crt_slow)
	{
		case PSI_HD_CRT_SLOW_MEM:
		{
			pool_ptr = &g_Eps_pools.crt_slow_mem[board_nr];
			break;
		}
		default:
		{
            EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_FATAL,
                "eps_cp_mem_delete_crt_slow_pool(): invalid cp_mem_crt_slow(%u) board_nr(%u) pool_handle(%u)",
                cp_mem_crt_slow, board_nr, pool_handle );
            EPS_FATAL( "eps_cp_mem_delete_crt_slow_pool(): invalid cp_mem_crt_slow" );

            //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
            return;
		}
	}

    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
	EPS_ASSERT( pool_ptr->help_mem_ptr );

	EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
		"eps_cp_mem_delete_crt_slow_pool(): board_nr(%u) pool_nr(%u) helpPtr(0x%08x)",
		board_nr, pool_ptr->pool_nr, pool_ptr->help_mem_ptr );

	// delete the memory pool and the helper memory
	if ( BufNumUsedInPool( pool_ptr->pool_nr ) == 0 ) // all memory blocks freed ?
	{
		int result = DeletePool( pool_ptr->pool_nr );
        LSA_UINT16 retVal;
		EPS_ASSERT( result == 0 );

        // check for overwrite
        if ( !eps_mem_verify_buffer( pool_ptr->help_mem_ptr ) )
        {
            // buffer is not ok!
            EPS_FATAL( "eps_cp_mem_delete_crt_slow_pool(): MEM3 CRT slow pool, but check for overwrite is not ok" );
        }

		retVal = eps_mem_free( pool_ptr->help_mem_ptr, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_IGNORE );
        EPS_ASSERT( LSA_RET_OK == retVal );

		// reset instance pool information
        eps_cp_mem_init_pool_struct( pool_ptr );
	}
	else
	{
		// Not all buffers freed		
		EPS_FATAL( "eps_cp_mem_delete_crt_slow_pool(): MEM3 CRT slow pool, but not all CRT slow buffers deleted" );
	}
    #elif (EPS_CRT_SLOW_MEM_USAGE == EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
	
	hif_mem_malloc_undo_init( pool_ptr->help_mem_ptr );
	
	// reset instance pool information
    eps_cp_mem_init_pool_struct( pool_ptr );
    #endif
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates a block from the slow mem.
 * @param [in] size         - size of the block
 * @param [in] pool_handle  - pool handle
 * @param [in] lsa_comp_id  - component id
 * @returns pointer to the memory block
 */
LSA_UINT8 * eps_cp_mem_crt_slow_alloc( LSA_UINT32 size, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE           ptr;
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.crt_slow_mem[pool_handle];

    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif

	ptr = eps_cp_mem_crt_slow_alloc_by_poolptr( size, pool_handle, pool_ptr, lsa_comp_id );

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees memory from the slow pool.
 * @param [in] ptr          - pointer to be freed
 * @param [in] pool_handle  - handle to the pool
 * @param [in] lsa_comp_id  - component id
 */
LSA_VOID eps_cp_mem_crt_slow_free( LSA_UINT8 * ptr, LSA_INT pool_handle, LSA_COMP_ID_TYPE lsa_comp_id )
{
	EPS_CP_MEM_POOL_PTR_TYPE    pool_ptr;

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    pool_ptr = &g_Eps_pools.crt_slow_mem[pool_handle];

    #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
	EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
    #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
	EPS_ASSERT( pool_ptr->help_mem_ptr );
    #endif
	eps_cp_mem_crt_slow_free_by_poolptr( ptr, pool_handle, pool_ptr, lsa_comp_id );
}

/*----------------------------------------------------------------------------*/
/**
 * Allocates a memory block by providing a pool pointer
 * @param [in] size         - size of the block
 * @param [in] pool_handle  - handle to the pool
 * @param [in] pool_ptr     - pointer to the pool
 * @param [in] lsa_comp_id  - component id
 */
static LSA_UINT8 * eps_cp_mem_crt_slow_alloc_by_poolptr( LSA_UINT32 size, LSA_INT pool_handle, EPS_CP_MEM_POOL_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_VOID_PTR_TYPE ptr;

	LSA_UNUSED_ARG( lsa_comp_id );

    EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    if (size == 0)
    {
        ptr = EPS_CP_MEM_ZERO_SIZE_PTR;

        EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_crt_slow_alloc_by_poolptr(): Alloc(0) - size(%u) pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
            size, pool_handle, pool_ptr, lsa_comp_id );        
    }
    else
    {
        #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        ptr = Mem3palloc( size, pool_ptr->pool_nr );
        #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        ptr = hif_mem_malloc( size, pool_ptr->help_mem_ptr );
        #endif

        if (ptr == LSA_NULL)
        {
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_cp_mem_crt_slow_alloc_by_poolptr(): Allocation returns 0-ptr!!! size(%u)!!! pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
                size, pool_handle, pool_ptr, lsa_comp_id );
        }
    }

	return ptr;
}

/*----------------------------------------------------------------------------*/
/**
 * Frees a block from the slow mem
 * @param [in] ptr          - pointer to be freed
 * @param [in] pool_handle  - handle to the pool
 * @param [in] pool_ptr     - pointer to the management struture of the pool
 * @param [in] comp_id      - component id
 */
static LSA_VOID eps_cp_mem_crt_slow_free_by_poolptr( LSA_UINT8 * ptr, LSA_INT pool_handle, EPS_CP_MEM_POOL_CONST_PTR_TYPE pool_ptr, LSA_COMP_ID_TYPE lsa_comp_id )
{
	LSA_UNUSED_ARG( lsa_comp_id );

	EPS_ASSERT( ptr != 0 );
	EPS_ASSERT( (pool_handle > 0) && (pool_handle <= EPS_MAX_BOARD_NR) );

    if (ptr == EPS_CP_MEM_ZERO_SIZE_PTR)
    {
        EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_cp_mem_crt_slow_free_by_poolptr(): free(0) - pool_handle(%u) pool_ptr(0x%08x) lsa_comp_id(%u)",
            pool_handle, pool_ptr, lsa_comp_id );   
    }
    else
    {
        #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        EPS_ASSERT( pool_ptr->pool_nr != POOL_UNKNOWN );
        #elif (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_HIF_INTERNAL)
        EPS_ASSERT( pool_ptr->help_mem_ptr );
        hif_mem_free( ptr, pool_ptr->help_mem_ptr );
        #endif

        #if (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_FFT2BOUND) || (EPS_CRT_SLOW_MEM_USAGE==EPS_MEM_USAGE_MEM3_UNIVERSAL3)
        if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
        {
            Mem3free( ptr );
        }
        else
        {
            // free of an invalid mem block --> dump statistic and break		
            EPS_FATAL( "eps_cp_mem_crt_slow_free_by_poolptr(): MEM3 CRT slow memory free failed" );
        }
        #endif
    }
}

#endif //(EPS_CFG_USE_IOH == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

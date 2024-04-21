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
/*  F i l e               &F: eps_mem.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation interface for MEM3 memory management                      */
/*  including local and fast memory pools                                    */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20008
#define EPS_MODULE_ID      20008

/*----------------------------------------------------------------------------*/
/* includes                                                                   */
/*----------------------------------------------------------------------------*/
#include "eps_sys.h" // Global EPS Header
#include "eps_locks.h"
#include <stddef.h>  // OBSD_ITGR -- shifted from pcpnio_sys.h, otherwise collision with OpenBSD types
#include "mem3if.h"  // MEM3 Interface
#include "eps_mempool.h" //mempool tool functions
#include <eps_trc.h> // Tracing
#include "eps_register.h"
#include <eps_mem.h>       /* Memory If               */

/*----------------------------------------------------------------------------*/
/* Defines                                                                    */
/*----------------------------------------------------------------------------*/
#define EPS_MEM_ALIGN4(x) (((x) + 3) & ~3) /* 4 Byte Alignment Macro */
#define EPS_MEM_MAGIC_VALUE     0x12344321

/*----------------------------------------------------------------------------*/
/* Structures                                                                 */
/*----------------------------------------------------------------------------*/
typedef struct eps_mem_block_tag
{
	LSA_BOOL          bInitialized;  // LSA_TRUE: Memory pool is initialized. LSA_FALSE: Memory pool is not initialized.
	LSA_UINT32        uSize;         // Size of memory block [bytes]
	LSA_VOID_PTR_TYPE pBase;         // Base address of memory block
	LSA_VOID_PTR_TYPE pHelperMem;    // pointer to helper memory for data pool
	LSA_UINT32        uHelperSize;   // Size of helper memory
	LSA_INT           pool_nr;       // Creation number for data pool or alias nr
	LSA_UINT16        pool_id;       // 
} EPS_MEM_BLOCK_TYPE, *EPS_MEM_BLOCK_PTR_TYPE;

typedef struct eps_mem_instance_tag
{
	LSA_BOOL            bInitialized;           // LSA_TRUE: Module is initialized. LSA_FALSE: Module is not initialized
	LSA_UINT16          hMemLock;               // Lock used to allocate memory. This lock is used by mem3
#ifdef EPS_USE_MEM_STATISTIC
    LSA_UINT16          hMemStatsLock;          // Lock to protect the statistics structure as well as the allocaion of memory.
#endif
	EPS_MEM_BLOCK_TYPE  sStdMem;                // Infos for normal memory  (heap)
	EPS_MEM_BLOCK_TYPE  sFastMem;               // Infos for fast memory
    EPS_MEM_BLOCK_TYPE  sUserMem[5];            // Infos for fast memory
} EPS_MEM_INSTANCE_TYPE, *EPS_MEM_INSTANCE_PTR_TYPE;

static EPS_MEM_INSTANCE_TYPE     g_EpsMem;      // global management structure
static EPS_MEM_INSTANCE_PTR_TYPE g_pEpsMem;     // pointer to global management stucture

/*----------------------------------------------------------------------------*/
/* Forward declarations                                                       */
/*----------------------------------------------------------------------------*/
static LSA_VOID eps_mempool_undo_init( EPS_MEM_BLOCK_PTR_TYPE pMem );
static LSA_RESPONSE_TYPE eps_mempool_init(LSA_UINT16 pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_ft mem_set, EPS_MEM_BLOCK_PTR_TYPE pMem);

/*----------------------------------------------------------------------------*/
/* This is to create an valid and defined return address for an alloc(0)      */
/*----------------------------------------------------------------------------*/
static  LSA_UINT16           eps_mem_zero_size_ptr_var = 0;
#define EPS_MEM_ZERO_SIZE_PTR (&eps_mem_zero_size_ptr_var)

/*----------------------------------------------------------------------------*/
/* Special implementation if EPS_USE_MEM_STATISTIC is used                    */
/*----------------------------------------------------------------------------*/

#ifdef EPS_USE_MEM_STATISTIC

/**
* Lock implementation
*/
static LSA_VOID eps_mem_statistics_lock( LSA_VOID )
{
	eps_enter_critical_section(g_pEpsMem->hMemStatsLock);
}

/**
* Lock implementation
*/
static LSA_VOID eps_mem_statistics_unlock( LSA_VOID )
{
	eps_exit_critical_section(g_pEpsMem->hMemStatsLock);
}

// Global management structure for memory. The last elements are used to sum up all previous elements.
EPS_MEM_STATISTIC_TYPE g_EpsMemStat[EPS_LSA_COMP_ID_SUM+1][EPS_LSA_COUNT_MEM_TYPES + 1] = { 0 }; /* +1 => sum */

/**
 * Returns a single statistics value from the array.

 * @param [in] compId  Component ID (valid range: 0...EPS_LSA_COMP_ID_SUM)
 * @param [in] memType Memory Type  (valid range: 0...EPS_LSA_COUNT_MEM_TYPES)
 * Note: 
 * - The last element EPS_LSA_COUNT_MEM_TYPES sums up all memory allocations for a the Component
 * - The last element EPS_LSA_COMP_ID_SUM sums up all memory allocations that were done for the pool
 * @return EPS_MEM_STATISTIC_TYPE - the requested element
 * @throws EPS_ASSERT if compId or memType are out of range
 */
EPS_MEM_STATISTIC_TYPE eps_mem_statistics_get_single_statistic(LSA_UINT16 compId, LSA_UINT16 memType)
{
	// range checks
	EPS_ASSERT(compId  < EPS_LSA_COMP_ID_SUM + 1);
	EPS_ASSERT(memType < EPS_LSA_COUNT_MEM_TYPES + 1);
	return g_EpsMemStat[compId][memType];
	
}
/**
* @brief Allocates memory using additional statistics functions. Allocation is done using mem3
* 
* Features: 
* - Statistics of all allocated blocks (number of all allocated blocks, byte sum of all allocated blocks etc.)
* - Storage of the location (filename and linenumber) of each allocation using a linked list
* - Statistics are done for each LSA Component and its memory type
* - CheckOverwrite and CheckUnderWrite
* - Check if a eps_mem_alloc matches a eps_mem_free (checks are done for lsa_comp_id and mem_type), @see eps_mem_free_with_statistic 
* - Check if there are still allocated blocks at shutdown, @see eps_mem_check_pool_by_statistics
* 
* @param [in] size         - size of the block
* @param [in] file         - filename as string using __FILE__
* @param [in] line         - linenumber within the file using __LINE__
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] pool_name    - not used in statistics, but required for mem3
* 
* @return Pointer to the memory block with the given size
* @return LSA_NULL if no memory is left in the pool
*/
static LSA_VOID_PTR_TYPE eps_mem_alloc_with_statistic( LSA_UINT32 size , LSA_CHAR* file, LSA_UINT16 line, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type , LSA_INT pool_name)
{
	LSA_UINT32 align4 = 0;
	LSA_UINT8* ptr;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prefix;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE postfix;

	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE* ppFirst;

	EPS_ASSERT(lsa_comp_id <= EPS_LSA_COMP_ID_LAST);    /* Comp Id must be within range  */
	EPS_ASSERT(mem_type    <  EPS_LSA_COUNT_MEM_TYPES); /* Mem Type must be within range */

	if(size%4)
    {
		align4 = (4-size%4);
    }

	ptr = (LSA_UINT8*) Mem3palloc( sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4 + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) , pool_name );
	
    if (ptr == LSA_NULL)
	{
        EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
            "eps_mem_alloc_with_statistic(): Allocation returns 0-ptr!!! size(%u) lsa_comp_id(%u) mem_type(%u) pool_name(%d)",
            size, lsa_comp_id, mem_type, pool_name );
        return LSA_NULL;
	}

	prefix  = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE)EPS_CAST_TO_VOID_PTR(ptr);
	postfix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE)EPS_CAST_TO_VOID_PTR(ptr + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4);

	prefix->comp_id  = lsa_comp_id;
	prefix->mem_type = mem_type;
	prefix->pool     = pool_name;
	prefix->size     = size;
	prefix->magic    = EPS_MEM_MAGIC_VALUE;
    prefix->line     = line;
    prefix->file     = file;
	prefix->isPrefix = 1;

	*postfix = *prefix;
	postfix->isPrefix = 0;


	// update field uActAllocSize
    g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize							+= size;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize			+= size;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocSize					+= size;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize	+= size;

	// update field uAllocCount
	g_EpsMemStat[lsa_comp_id][mem_type].uAllocCount++;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uAllocCount++;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uAllocCount++;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uAllocCount++;

	// update field uActAllocCount
	g_EpsMemStat[lsa_comp_id][mem_type].uActAllocCount++;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount++;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocCount++;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount++;

	// update field uSumAllocSize
	g_EpsMemStat[lsa_comp_id][mem_type].uSumAllocSize							+= size;
	g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uSumAllocSize			+= size;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uSumAllocSize					+= size;
	g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uSumAllocSize	+= size;


	// update field uMinAllocSizeElement
	// minimum for this component and memtype
	if (g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement == 0)
	{
		g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement = size;
	}
	else if (g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement > size)
	{
		g_EpsMemStat[lsa_comp_id][mem_type].uMinAllocSizeElement = size;
	}

	// minimum for this component over all memtypes
	if (g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement == 0)
	{
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement = size;
	}
	else if (g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement > size)
	{
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement = size;
	}

	// minimum for this memtype over all components
	if(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMinAllocSizeElement == 0)
    {
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMinAllocSizeElement = size;
    }
	else if(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMinAllocSizeElement > size)
    {
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMinAllocSizeElement = size;
    }
	// minimum over all components and memtypes
	if (g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement == 0)
	{
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement = size;
	}
	else if (g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement > size)
	{
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMinAllocSizeElement = size;
	}

	// Update field uMaxAllocSizeElement
	// Maximum for this component and this memtype
	if (g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSizeElement < size)
	{
		g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSizeElement = size;
	}
	// maximum for this memtype over all components
	if(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMaxAllocSizeElement < size)
    {
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMaxAllocSizeElement = size;
    }
	// maximum for this memtype over all components
	if (g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSizeElement < size)
	{
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSizeElement = size;
	}
	// maximum over all components and memtypes
	if (g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSizeElement < size)
	{
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSizeElement = size;
	}

	// Update field uMaxAllocSize
	// maximum for this component and this memtype
	if (g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize > g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSize)
	{
		g_EpsMemStat[lsa_comp_id][mem_type].uMaxAllocSize = g_EpsMemStat[lsa_comp_id][mem_type].uActAllocSize;
	}
	// maximum for this memtype over all components
	if(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocSize > g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMaxAllocSize)
    {
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uMaxAllocSize = g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocSize;
    }
	// maximum over all memtypes for this component
	if (g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize > g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize)
	{
		g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize = g_EpsMemStat[lsa_comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize;
	}
	// maximum over all components and all memtypes
	if(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize > g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize)
    {
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize = g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize;
    }


	/* do enqueuing */
	ppFirst = &g_EpsMemStat[lsa_comp_id][mem_type].first;

	prefix->next = *ppFirst;
	prefix->prev = LSA_NULL;
	if(*ppFirst != LSA_NULL)
    {
		(*ppFirst)->prev = prefix;
    }
	*ppFirst = prefix;
	postfix->next = prefix->next;
	postfix->prev = prefix->prev;

	return (LSA_VOID_PTR_TYPE)(ptr+sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE));
}

/**
* @brief Frees memory and update memory statistic. Freeing of the memory is done using mem3
 * 
* @param [in] ptr          - pointer to the memory block that shall be freed
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] pool_name    - not used in statistics, but required for mem3
* 
* @return #LSA_RET_OK           - pointer was successfully freed
* @return LSA_RET_ERR_RESOURCE  - an error occured:  This pointer is unknown to the memory management.
 */
static LSA_UINT16 eps_mem_free_with_statistic ( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 comp_id, LSA_UINT32 mem_type, LSA_INT pool )
{
	LSA_UINT32 size;
	LSA_UINT32 align4;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE prefix;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE postfix;
	
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE *ppFirst;
	EPS_MEM_STATISTIC_PREFIX_PTR_TYPE cprefix;

	EPS_ASSERT(ptr);

	ptr = (LSA_UINT8*)ptr - sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE);
	prefix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE)(ptr);

	size = GetBufSize( ptr );

	if ( size > 0 )
	{
		EPS_ASSERT(size > sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE));

		EPS_ASSERT(prefix->comp_id  <= EPS_LSA_COMP_ID_LAST);     /* Comp Id must be within range  */
		EPS_ASSERT(prefix->mem_type <  EPS_LSA_COUNT_MEM_TYPES);  /* Mem Type must be within range */

		size = prefix->size;

		align4 = 0;
		if(prefix->size%4)
		{
			align4 = (4-prefix->size%4);
		}
		postfix = (EPS_MEM_STATISTIC_PREFIX_PTR_TYPE)EPS_CAST_TO_VOID_PTR((LSA_UINT8*)ptr + sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE) + size + align4);

		/* prefix / postfix check => check overwrites */
		if(		(prefix->mem_type != mem_type)
			||	(prefix->pool     != pool)
			||	(prefix->magic    != EPS_MEM_MAGIC_VALUE)
			||	(prefix->comp_id  != comp_id)
			||	(prefix->magic    != postfix->magic)
			||	(prefix->comp_id  != postfix->comp_id)
			||	(prefix->mem_type != postfix->mem_type)
			||	(prefix->pool     != postfix->pool)
			||	(prefix->size     != postfix->size)
			)
		{
			/* Memory block that is freed does not match to the block that was allocated. Trace all infos */
			/* Infoblock 1: Infos from call vs infos from allocation */
			EPS_SYSTEM_TRACE_07(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_free_with_statistic: Error while freeing block. Expected: prefix->mem_type = %d, prefix->pool = %d, prefix->comp_id = %d, prefix->size = %d Real: mem_type = %d, pool = %d, comp_id = %d",
														 prefix->mem_type, 
														 prefix->pool, 
														 prefix->comp_id, 
														 prefix->size,
														 mem_type,
														 pool,
														 comp_id);

			/* Infoblock 2: Infos from memory overwrite check */
			EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_free_with_statistic: Error while freeing block. CheckOverwrite: prefix->magic = %d, postfix->magic = %d , postfix->comp_id = %d, postfix->mem_type = %d, postfix->pool = %d, postfix->size %d",
												prefix->magic, 
												postfix->magic,
												postfix->comp_id,
												postfix->mem_type,
												postfix->pool,
												postfix->size);
			
			/* Infoblock 3: Location in sourcecode with linenumber */
			EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_free_with_statistic: allocated block in file %s", prefix->file);
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_free_with_statistic: line = %d", prefix->line);

			/* Don't throw a FATAL here, some products may want to ignore this? */
            //lint --e(506) Constant value Boolean - This is intended. In the final version, EPS_ASSERT may be defined as an empty macro. In a debug version, EPS_ASSERT(0) throws a FATAL
			//lint --e(774) Boolean within 'if' always evaluates to True - see above
            EPS_ASSERT(0);
		}

		/* Memory statistic checks */
		EPS_ASSERT(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[comp_id][mem_type].uActAllocCount > 0);
		EPS_ASSERT(g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount > 0);

	    EPS_ASSERT(g_EpsMemStat[comp_id][mem_type].uActAllocSize >= size);
		EPS_ASSERT(g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize >= size);

		EPS_ASSERT(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocSize >= size);
		EPS_ASSERT(g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize >= size);

		/* Update Memory Statistic */

		// Update field uActAllocSize
		g_EpsMemStat[comp_id][mem_type].uActAllocSize								-= size;
		g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize				-= size;
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocSize					-= size;
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocSize	-= size;

		// update field uActAllocCount
		g_EpsMemStat[comp_id][mem_type].uActAllocCount--;
		g_EpsMemStat[comp_id][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount--;
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][mem_type].uActAllocCount--;
		g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount--;
		

		/* do dequeuing */
		ppFirst = &g_EpsMemStat[comp_id][mem_type].first;

		cprefix = *ppFirst;
		while(cprefix != prefix && (cprefix != 0))
        {
			cprefix = cprefix->next;
        }
		EPS_ASSERT(cprefix == prefix); /* not found */
		if(cprefix->prev != LSA_NULL)
        {
			cprefix->prev->next = cprefix->next;
        }
		else
        {
			*ppFirst = cprefix->next;
        }
		if(cprefix->next != LSA_NULL)
        {
			cprefix->next->prev = cprefix->prev;
        }

		Mem3free( ptr );
		return LSA_RET_OK;
	}

	EPS_FATAL( "Size of memory to be freed must be greater then zero" );
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	return LSA_RET_ERR_RESOURCE;
}
/**
* @brief Checks the memory statistics and searches for allocated blocks.
* 
* If there are still allocated blocks, we search the linked list for each allocated block and trace the info.
*
* @return LSA_TRUE - no blocks in use
* @return LSA_FALSE - there are still blocks in use. Detailled infos -> see trace
*/
static LSA_BOOL eps_mem_check_pool_by_statistics( void )
{
    // indizes to access g_EpsMemStat; valid range: i: 0...EPS_LSA_COMP_ID_SUM-1; k:0...EPS_LSA_COUNT_MEM_TYPES-1 
    LSA_UINT16 i, k;
    LSA_BOOL bNoBlocksInUse = LSA_TRUE;

    // first, check the last entry since the statistics module sums up all allocated blocks there. 
    if (g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount != 0)
    {
        /* we found a block that is still allocated */
        bNoBlocksInUse = LSA_FALSE;

        // deeper inspection for each component. Skip the last element since statistics for all components are stored there.
        for ( i = 0; i < EPS_LSA_COMP_ID_SUM; i++)
        {
            /* sum of all allocs is located at the last element */
            if(g_EpsMemStat[i][EPS_LSA_COUNT_MEM_TYPES].uActAllocCount == 0)
            {
                continue; // this component has no open allocs
            }
            else
            {
                // deeper inspection for each memtype
                // look through all memtypes. Skip the last element since it's sums up all previous elements
                for ( k = 0; k < EPS_LSA_COUNT_MEM_TYPES; k++)
                {
                    if(g_EpsMemStat[i][k].uActAllocCount != 0)
                    {
                        EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_check_pool_by_statistics: LSA_COMP_ID = %d has (%d) allocated blocks for memtype = %d ",i, g_EpsMemStat[i][k].uActAllocCount, k);            
                        // find the evil block by iterating through the linked list 
                        {
                            EPS_MEM_STATISTIC_PREFIX_PTR_TYPE helper = g_EpsMemStat[i][k].first;
							// walk through the linked list of allocated blocks for this memory type and trace all allocated blocks with their filename and linenumber
                            while(helper != LSA_NULL)
                            {   
                                EPS_SYSTEM_TRACE_STRING(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_check_pool_by_statistics: allocated block in file %s", helper->file);
                                EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"eps_mem_check_pool_by_statistics: line = %d. Size of block = %d", helper->line, helper->size);
                                helper = helper->next;
                            }    
                        }
                    }
                }
            }
        }
    }
    else
    {
        // no blocks in use
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_mem_check_pool_by_statistics: No blocks in use");
    }
    return bNoBlocksInUse;
}

/**
* @brief Checks the memory statistics and searches for allocated blocks.
* 
* If there are still allocated blocks, we search the linked list for each allocated block and trace the info.
*
* @return LSA_TRUE - no blocks in use
* @return LSA_FALSE - there are still blocks in use. Detailled infos -> see trace
*/
LSA_VOID eps_mem_statistics_print_pool_usage( void )
{
    // indizes to access g_EpsMemStat; valid range: i: 0...EPS_LSA_COMP_ID_SUM-1; k:0...EPS_LSA_COUNT_MEM_TYPES-1 
    LSA_UINT16 i;
    LSA_UINT64 percent;
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "****************************************");
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Memory usage statistics:");
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "   Local Memory:");
    
    percent = ((LSA_UINT64)g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize * 1000) / (LSA_UINT64)g_pEpsMem->sStdMem.uSize;
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "      Available: 0x%8x, Used: 0x%8x, Usage in percent: %2d.%01d", g_pEpsMem->sStdMem.uSize, g_EpsMemStat[EPS_LSA_COMP_ID_SUM][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize, percent/10, percent%10);
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "****************************************");
    
    // deeper inspection for each component. Skip the last element since statistics for all components are stored there.
    for ( i = 0; i < EPS_LSA_COMP_ID_SUM; i++)
    {
        percent = ((LSA_UINT64)g_EpsMemStat[i][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize * 1000) / (LSA_UINT64)g_pEpsMem->sStdMem.uSize;
        
        /* sum of all allocs is located at the last element */
        EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "   Comp ID %2d: Used: 0x%8x, Usage in percent: %2d.%01d", i, g_EpsMemStat[i][EPS_LSA_COUNT_MEM_TYPES].uMaxAllocSize, percent/10, percent%10);
    }
    
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "****************************************");
}
#endif // EPS_USE_MEM_STATISTIC

/*----------------------------------------------------------------------------*/
/* Implementation                                                             */
/*----------------------------------------------------------------------------*/

/**
* @brief Initializes the memory interface of EPS
* 
* EPS expects a big memory block and builds a memory allocation management around it. All calls to eps_mem_alloc will use memory from this block (pStdMemBase).
* EPS will check at eps_mem_undo_init if they are still allocated blocks and will go into FATAL if this is the case.
* 
* Currently, EPS uses the component mem3 for memory management.
* 
* @param [in] pStdMemBase    base pointer to location of the local memory
* @param [in] uStdMemSize    size of the local memory
* @param [in] pFastMemBase   base pointer to location of the fast memory
* @param [in] uFastMemSize   size of the fast memory
* 
* @return #LSA_RET_OK Pool was successfully created
*/
LSA_RESPONSE_TYPE eps_mem_init( LSA_UINT32* pStdMemBase, LSA_UINT32 uStdMemSize, LSA_UINT32* pFastMemBase, LSA_UINT32 uFastMemSize )
{
	LSA_RESPONSE_TYPE retVal;
    LSA_UINT8         userPoolCount;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, ">> eps_mem_init()");
    
    // Initialize Global Mem Structure
	eps_memset( &g_EpsMem, 0 , sizeof(EPS_MEM_INSTANCE_TYPE) );
	g_pEpsMem = &g_EpsMem;

	uStdMemSize  = uStdMemSize - uStdMemSize%4;
	uFastMemSize = uFastMemSize - uFastMemSize%4;

	// Init Locks
	retVal = eps_alloc_critical_section_prio_protected(&g_pEpsMem->hMemLock, LSA_TRUE); 
	EPS_ASSERT(retVal == LSA_RET_OK);

    #ifdef EPS_USE_MEM_STATISTIC
    retVal = eps_alloc_critical_section(&g_pEpsMem->hMemStatsLock, LSA_TRUE);
	EPS_ASSERT(retVal == LSA_RET_OK);
    #endif

    // Configure EPS User mem pools - no pools are created
    for (userPoolCount = 0; userPoolCount < EPS_MEM_MAX_USER_POOLS; userPoolCount++)
    {
        g_pEpsMem->sUserMem[userPoolCount].bInitialized = LSA_FALSE;
        g_pEpsMem->sUserMem[userPoolCount].pool_id      = EPS_USER_POOL1 + userPoolCount; // first Pool gets id EPS_USER_POOL1, secound pool gets EPS_USER_POOL1 + 1 
    }

	// Create Fast Mem pool
	if( (pFastMemBase == LSA_NULL) &&
		(uFastMemSize != 0) )
	{
		uStdMemSize -= uFastMemSize;
		pFastMemBase = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR((LSA_UINT8*)pStdMemBase + uStdMemSize);
	}

	retVal = eps_mempool_init(EPS_FAST_MEMORY, pFastMemBase, uFastMemSize, 8 /*DWord alignment*/, eps_memset, &g_pEpsMem->sFastMem);
	EPS_ASSERT(retVal == LSA_RET_OK);

	// Create Local Mem pool
	EPS_ASSERT( pStdMemBase != 0 );
	EPS_ASSERT( uStdMemSize != 0 );
	
	retVal = eps_mempool_init(EPS_LOCAL_MEMORY, pStdMemBase, uStdMemSize, 8 /*DWord alignment*/, eps_memset, &g_pEpsMem->sStdMem);
	EPS_ASSERT( retVal == LSA_RET_OK );

	g_pEpsMem->bInitialized = LSA_TRUE;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "<< eps_mem_init()");

	return retVal;
}

/**
* @brief uninitializes the management for local memory 
* 
* This function checks for allocated blocks at shutdown by ASSERT. If there are any allocated blocks, the ASSERT will trigger.
*
*/
LSA_VOID eps_mem_undo_init( LSA_VOID )
{
    LSA_RESPONSE_TYPE retVal;
    LSA_UINT8         userPoolCount;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, ">> eps_mem_undo_init()");

	EPS_ASSERT( g_pEpsMem != LSA_NULL );
	EPS_ASSERT( g_pEpsMem->bInitialized == LSA_TRUE );

	eps_mempool_undo_init(&g_pEpsMem->sFastMem);

	eps_mempool_undo_init(&g_pEpsMem->sStdMem);

    //Check if user has deleted all pools
    for (userPoolCount = 0; userPoolCount < EPS_MEM_MAX_USER_POOLS; userPoolCount++)
    {
        if (g_pEpsMem->sUserMem[userPoolCount].bInitialized == LSA_TRUE)
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "eps_mem_undo_init(): eps user mem pool(%u) is not deleted!", userPoolCount);
            EPS_FATAL("eps_mem_undo_init() : not all eps user mem pools are deleted!");
        }
    }

	retVal = eps_free_critical_section(g_pEpsMem->hMemLock);
    EPS_ASSERT( LSA_RET_OK == retVal );
    #ifdef EPS_USE_MEM_STATISTIC
    retVal = eps_free_critical_section(g_pEpsMem->hMemStatsLock);
    EPS_ASSERT( LSA_RET_OK == retVal );
    #endif
	g_pEpsMem->bInitialized = LSA_FALSE;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "<< eps_mem_undo_init()");
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
* Lock implementation
*/
LSA_VOID eps_mem_lock( LSA_VOID )
{
	eps_enter_critical_section(g_pEpsMem->hMemLock);
}

/**
* Lock Implementation
*/
LSA_VOID eps_mem_unlock( LSA_VOID )
{
	eps_exit_critical_section(g_pEpsMem->hMemLock);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
* @brief Creates a memory pool using mem3
*
* @param [out] pool_id - pool identifier, that is used for alloc and free
* @param [in] pBase    - basepointer to the block where the memory pool is located
* @param [in] uSize    - size of the pool
* @param [in] uAlign   - alignment of the pool
* @param [in] mem_set  - function that should be used for memset actions by mem3
* @return #LSA_RET_OK           - pool was successfully created
* @return #LSA_RET_ERR_RESOURCE - no free poolhandle in eps was found. Maybe you have to increase EPS_MEM_MAX_USER_POOLS
* @return #LSA_RET_ERR_PARAM    - pool could not be created due to parameter errors. See log file traces.
*/
LSA_RESPONSE_TYPE eps_mempool_create(LSA_UINT16* pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_fp mem_set)
{
    //Exported function, suppress lint warnings for "could be declared static"
    //lint -esym(765, eps_mempool_create)

    LSA_UINT8           userPoolCount;
    LSA_RESPONSE_TYPE   retvalCreate;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, ">> eps_mempool_create()");

    //validate inputs
    EPS_ASSERT(pool_id != LSA_NULL);
    EPS_ASSERT(pBase != LSA_NULL);
    EPS_ASSERT(mem_set != LSA_NULL);

    //find free mem pool storage
    for(userPoolCount = 0; userPoolCount < EPS_MEM_MAX_USER_POOLS; userPoolCount++)
    {
        if (g_pEpsMem->sUserMem[userPoolCount].bInitialized == LSA_FALSE)
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_mempool_create(): found free user pool (%u)", userPoolCount);
            break;
        }
    }

    if(userPoolCount == EPS_MEM_MAX_USER_POOLS)
    {
        // no unitizialized pool found
        *pool_id = POOL_UNKNOWN;
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_mempool_create(): could not create pool. %u out of %u user pools are in use.", userPoolCount, EPS_MEM_MAX_USER_POOLS);
        return LSA_RET_ERR_RESOURCE;
    }

    {
        //create the userpool: the helperpool must be located on local mem, because the pool may be on inaccessable hardware memory.
        LSA_UINT32 granSize, fftThreshold;
        LSA_VOID *pHelpMem;
        LSA_VOID *pBaseMem;
        EPS_MEM_BLOCK_PTR_TYPE pMem = &(g_pEpsMem->sUserMem[userPoolCount]);

        EPS_ASSERT(pMem != LSA_NULL);
        EPS_ASSERT(uSize != 0);
        EPS_ASSERT(pMem->bInitialized == LSA_FALSE);

        pMem->pBase = pBase;
        pMem->pool_id = g_pEpsMem->sUserMem[userPoolCount].pool_id;
        pMem->pHelperMem = LSA_NULL;
        pMem->uHelperSize = 0;
        pMem->pool_nr = -1;

        EPS_ASSERT(pMem->pool_id != POOL_UNKNOWN);

        granSize = 0x08;    // for KRAM use 8
        fftThreshold = 0x0; // not defined

        pMem->uHelperSize = PoolHelperSize3(uSize, granSize, Fft2Bound, LSA_FALSE, uAlign);
        pMem->uHelperSize = EPS_MEM_ALIGN4(pMem->uHelperSize);

        EPS_ASSERT(pMem->uHelperSize < uSize);

        pMem->pHelperMem = eps_mem_alloc(pMem->uHelperSize, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);    //allocate in local memory
        pMem->uSize = uSize;

        // Note: assignment is necessary because ptr are modified in MEM3!
        pHelpMem = pMem->pHelperMem;
        pBaseMem = pMem->pBase;

        SetMemSetFunc(pMem->pool_id, (pMemSet_ft)mem_set);  // set special memset function for MEM3
        SetCheckOverwrite(pMem->pool_id, LSA_FALSE);        // disable overwrite check
        SetPoolAlignment(pMem->pool_id, uAlign);            // 8: DWord alignment

        /// create MEM3 pool
        pMem->pool_nr = CreatePool(&pBaseMem, &pHelpMem, pMem->uSize, pMem->uHelperSize, granSize, fftThreshold, Fft2Bound, pMem->pool_id);

        EPS_ASSERT(pMem->pool_nr >= 0);
        EPS_ASSERT(PoolValid(pMem->pool_nr));

        pMem->bInitialized = LSA_TRUE;

        retvalCreate = LSA_RET_OK;
    }

    if (retvalCreate != LSA_RET_OK)
    {
        *pool_id = POOL_UNKNOWN;
        EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "eps_mempool_create(): eps_mempool_init failed with (%u) for user pool (%u), eps_pool_id(%u)",
            retvalCreate, userPoolCount, g_pEpsMem->sUserMem[userPoolCount].pool_id);
    }
    else
    {
        *pool_id = userPoolCount + 1; // 0 would be invalid, so add 1
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_mempool_create(): user pool (%u) created with pool_id(%u)", userPoolCount, (LSA_UINT16) *pool_id);
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<< eps_mempool_create()");
    return retvalCreate;
}

/**
* @brief Deletes the pool given by the management info in pMem. Resets all values in pMem.
*
* @param [in] pool_id   - pool_id that was received by eps_mempool_create
* @return #LSA_RET_ERR_PARAM - invalid pool_id was given
*/
LSA_RESPONSE_TYPE eps_mempool_delete(LSA_UINT16 pool_id)
{
    //Exported function, suppress lint warnings for "could be declared static"
    //lint -esym(765, eps_mempool_delete)

    LSA_UINT8           userPoolCount = (LSA_UINT8)(pool_id - 1); // map pool_id to array index: index goes from 0 to EPS_MEM_MAX_USER_POOLS - 1
    LSA_RESPONSE_TYPE   retvalDelete;
    LSA_UINT8*          freeHelperMem;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, ">> eps_mempool_delete()");

    EPS_ASSERT((pool_id > 0) && (pool_id <= EPS_MEM_MAX_USER_POOLS)); //valid pool_ids: 1 to EPS_MEM_MAX_USER_POOLS,

    //find mem pool storage
    if (g_pEpsMem->sUserMem[userPoolCount].bInitialized == LSA_TRUE)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "eps_mempool_delete(): delete user pool (%u)", userPoolCount);

        //delete the pool
        freeHelperMem = g_pEpsMem->sUserMem[userPoolCount].pHelperMem; // store pointer to free after undo_init already has set it to null
        eps_mempool_undo_init(&g_pEpsMem->sUserMem[userPoolCount]);
        eps_mem_free(freeHelperMem, LSA_COMP_ID_PNBOARDS, PSI_MTYPE_LOCAL_MEM);
        retvalDelete = LSA_RET_OK;
    }
    else
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_mempool_delete(): user pool (%u) not initialized", userPoolCount);
        retvalDelete = LSA_RET_ERR_PARAM;
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<< eps_mempool_delete()");
    return retvalDelete;
}

/**
* @brief Allocates memory using a pool_id. Allocation is done using mem3.
*
* If there is not enough memory left in the pool, LSA_NULL is returned and a trace is generated.
*
* @param [in] size      - size of the block
* @param [in] pool_id   - pool_id that was received by eps_mempool_create
*
* @return #Pointer      - to the memory block with the given size
* @return #LSA_NULL     - if no memory is left in the pool or size is 0
*/
LSA_VOID_PTR_TYPE eps_mempool_alloc(LSA_UINT16 pool_id, LSA_UINT32 size)
{
    //Exported function, suppress lint warnings for "could be declared static"
    //lint -esym(765, eps_mempool_alloc)

    LSA_UINT8           userPoolCount = (LSA_UINT8)(pool_id - 1); // map pool_id to array index: index goes from 0 to EPS_MEM_MAX_USER_POOLS - 1
    LSA_UINT16          pool_nr;
    LSA_VOID_PTR_TYPE   ptr = LSA_NULL;

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, ">> eps_mempool_alloc(): size(%u) pool(%d)", size, pool_id);

    EPS_ASSERT((pool_id > 0) && (pool_id <= EPS_MEM_MAX_USER_POOLS)); //valid pool_ids: 1 to EPS_MEM_MAX_USER_POOLS,

    // check if size is 0. Alloc(0) will return a defined address
    if (size == 0)
    {
        EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "eps_mempool_alloc(): size(%u) pool(%d) - alloc(0)", size, pool_id);
        ptr = EPS_MEM_ZERO_SIZE_PTR;
    }
    else
    {
        //find mem pool storage
        if (g_pEpsMem->sUserMem[userPoolCount].bInitialized == LSA_TRUE)
        {
            pool_nr = (LSA_UINT16) g_pEpsMem->sUserMem[userPoolCount].pool_nr;
            ptr = Mem3palloc(size, pool_nr);

            // memory allocation failed?
            if (ptr == LSA_NULL)
            {
                EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "eps_mempool_alloc(): Allocation returns 0-ptr!!! size(%u) pool(%d)", size, pool_id);
            }
        }
        else
        {
            EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "eps_mempool_alloc(): mem pool (%u) not initialized", pool_id);
        }
    }

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<< eps_mempool_alloc()");
    return ptr;
}

/**
* @brief Frees memory. Freeing of the memory is done using mem3
*
* @param [in] ptr       - pointer to the memory block that shall be freed
* @param [in] pool_id   - pool_id that was received by eps_mempool_create
*
* @return #LSA_RET_OK           - pointer was successfully freed
* @return #LSA_RET_ERR_RESOURCE - an error occured:  This pointer is unknown to the memory management.
*/
LSA_UINT16 eps_mempool_free(LSA_UINT16 pool_id, LSA_VOID_PTR_TYPE ptr)
{
    //Exported function, suppress lint warnings for "could be declared static"
    //lint -esym(765, eps_mempool_free)

    // Handle the alloc(0) address
    if(ptr == EPS_MEM_ZERO_SIZE_PTR)
    {
        EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "eps_mempool_free(): pool(%d) - free(0)", pool_id);
        return LSA_RET_OK;
    }
    else
    {    
        EPS_ASSERT(ptr);
        if (GetBufSize(ptr) > 0) // we have a valid buffer ?
        {
            Mem3free(ptr);
            return LSA_RET_OK;
        }
        return LSA_RET_ERR_RESOURCE;
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
* @brief Initializes a memory pool. 
* 
* This functions sets up the parameters for mem3.
*
* @param [in] pool_id - pool identifier
* @param [in] pBase   - basepointer to the block where the memory pool is located
* @param [in] uSize   - size of the pool
* @param [out] pMem   - pointer to the management structure. This function fills the management structure
* @return #LSA_RET_OK - pool was successfully created
*/
static LSA_RESPONSE_TYPE eps_mempool_init( LSA_UINT16 pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_ft mem_set, EPS_MEM_BLOCK_PTR_TYPE pMem )
{
	LSA_UINT32 granSize, fftThreshold;
	LSA_VOID *pHelpMem;
	LSA_VOID *pBaseMem;

	EPS_ASSERT( pBase != LSA_NULL );
	EPS_ASSERT( pMem  != LSA_NULL );
	EPS_ASSERT( uSize != 0 );
	EPS_ASSERT( pMem->bInitialized == LSA_FALSE );

	pMem->pBase       = pBase;
	pMem->pool_id     = pool_id;
	pMem->pHelperMem  = LSA_NULL;
	pMem->uHelperSize = 0;
	pMem->pool_nr     = -1;

	EPS_ASSERT(pMem->pool_id != POOL_UNKNOWN);

	granSize     = 0x20; // 32 Byte packages (A full frame fits without waste)
	fftThreshold = 0x0;

	pMem->uHelperSize = PoolHelperSize3( uSize, granSize, Fft2Bound, LSA_TRUE, 8 );
	pMem->uHelperSize = EPS_MEM_ALIGN4( pMem->uHelperSize );

	EPS_ASSERT( pMem->uHelperSize < uSize ); 

	pMem->pHelperMem = (LSA_UINT32*)EPS_CAST_TO_VOID_PTR((LSA_UINT8*)pBase + uSize - pMem->uHelperSize);
	pMem->uSize = uSize - pMem->uHelperSize;

	// Note: assignment is necessary because ptr are modified in MEM3!
	pHelpMem = pMem->pHelperMem;
	pBaseMem = pMem->pBase;

	SetMemSetFunc( pool_id, mem_set);   // set standard memset function for MEM3
	SetCheckOverwrite( pool_id, LSA_TRUE ); // enable overwrite check
	SetPoolAlignment( pool_id, uAlign);         // 8: DWord alignment

	// create MEM3 pool
	pMem->pool_nr = CreatePool( &pBaseMem, &pHelpMem, pMem->uSize, pMem->uHelperSize, granSize, fftThreshold, Fft2Bound, pMem->pool_id );

	EPS_ASSERT( pMem->pool_nr >= 0 );
	EPS_ASSERT( PoolValid( pMem->pool_nr ) );

	pMem->bInitialized = LSA_TRUE;

	return LSA_RET_OK;
}
/**
* @brief Deletes the pool given by the management info in pMem. Resets all values in pMem.
* 
* @param [inout] pointer to the management structure.
*/
static LSA_VOID eps_mempool_undo_init( EPS_MEM_BLOCK_PTR_TYPE pMem )
{
	int result;
	int BufNumUsed;

	// delete the memory pool, and the helper memory
	EPS_ASSERT( pMem != LSA_NULL );
	EPS_ASSERT( pMem->bInitialized == LSA_TRUE );

	BufNumUsed = BufNumUsedInPool( pMem->pool_nr );

	EPS_ASSERT( BufNumUsed == 0 ); /* Are there any buffers not freed? */
	
	result = DeletePool( pMem->pool_nr );
	EPS_ASSERT( result == 0 );
		
	pMem->pHelperMem   = 0;
	pMem->pool_nr      = -1;
	pMem->pBase        = 0;
	pMem->uSize        = 0;
	pMem->bInitialized = LSA_FALSE;
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
* @brief Allocates memory using from a pool. Allocation is done using mem3
* If the define EPS_USE_MEM_STATISTIC is set, the function eps_mem_alloc_with_statistic is used. @see eps_mem_alloc_with_statistic for more infos.
* Otherwise, the memory allocation is passed to mem3.
* 
* Throws a FATAL if a component wants to allocate memory with the size of 0 (undefined behaviour)
* If there is not enough memory left in the pool, LSA_NULL is returned and a trace is generated.
* 
* @param [in] size         - size of the block
* @param [in] file         - filename as string using __FILE__
* @param [in] line         - linenumber within the file using __LINE__
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] pool_name    - not used in statistics, but required for mem3
* 
* @return Pointer to the memory block with the given size
* @return LSA_NULL if no memory is left in the pool
*/
LSA_VOID_PTR_TYPE eps_mem_alloc_from_pool( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool, LSA_UINT16 line, LSA_CHAR* file )
{
	LSA_VOID_PTR_TYPE ptr;

    // check if size is 0. Alloc(0) will return a defined ZERO_SIZE_PTR address
    if (size == 0)
    {
        ptr = EPS_MEM_ZERO_SIZE_PTR;
        
        EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_mem_alloc_from_pool(): Alloc(0) - size(%u) lsa_comp_id(%u) mem_type(%u) pool(%d)",
            size, lsa_comp_id, mem_type, pool );
    }
    else
    {
        #ifdef EPS_USE_MEM_STATISTIC
        eps_mem_statistics_lock(); // use a separate lock for statistics. Note: eps_mem_lock is not possible here (nested locks!)
        ptr = eps_mem_alloc_with_statistic(size, file, line, lsa_comp_id, mem_type, pool);
        eps_mem_statistics_unlock();
        #else
        LSA_UNUSED_ARG(line);
        LSA_UNUSED_ARG(file);
    
        ptr = Mem3palloc( size, pool );
        #endif
    
        // memory allocation failed?
        if (ptr == LSA_NULL)
        {
            EPS_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_ERROR,
                "eps_mem_alloc_from_pool(): Allocation returns 0-ptr!!! size(%u) lsa_comp_id(%u) mem_type(%u) pool(%d)",
                size, lsa_comp_id, mem_type, pool );
        }
    }

	return ptr;
}

/**
* @brief Frees memory. Freeing of the memory is done using mem3
* 
* @param [in] ptr          - pointer to the memory block that shall be freed
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] pool_name    - not used in statistics, but required for mem3
* 
* @return #LSA_RET_OK           - pointer was successfully freed
* @return LSA_RET_ERR_RESOURCE  - an error occured:  This pointer is unknown to the memory management.
*/
LSA_UINT16 eps_mem_free_from_pool( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_INT pool )
{  
    // We handle the alloc(0) ptr and do nothing if it is free'd
    if(ptr == EPS_MEM_ZERO_SIZE_PTR)
    {
        // we just say everything is okay
        EPS_SYSTEM_TRACE_03( 0, LSA_TRACE_LEVEL_NOTE,
            "eps_mem_free_from_pool(): free(0) - lsa_comp_id(%u) mem_type(%u) pool(%d)",
            lsa_comp_id, mem_type, pool );
        
        return LSA_RET_OK;
    }
    else
    {	
        #ifdef EPS_USE_MEM_STATISTIC
        LSA_UINT16 retVal;
        eps_mem_statistics_lock();    // use a separate lock for statistics. Note: eps_mem_lock is not possible here (nested locks!)
        retVal = eps_mem_free_with_statistic( ptr, lsa_comp_id, mem_type, pool );
        EPS_ASSERT( LSA_RET_OK == retVal );
        eps_mem_statistics_unlock();
        return LSA_RET_OK;
        #else
    
        EPS_ASSERT( ptr );
        if ( GetBufSize( ptr ) > 0 ) // we have a valid buffer ?
        {
            Mem3free( ptr );
            return LSA_RET_OK;
        }
        return LSA_RET_ERR_RESOURCE;
        #endif
    }
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/**
* @brief Allocates a memory block with the given size from the internal memory pool that was provided by eps_mem_init
* 
* Note: This is an extended version of eps_mem_alloc that adds the linenumber as well as the filename. eps_mem_alloc is extended to this function using a macro defintion. 
* 
* @param [in] size         - size of the block
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] line         - linenumber within the file using __LINE__. This info is used if EPS_USE_MEM_STATISTIC is set
* @param [in] file         - filename as string using __FILE__. This info is used if EPS_USE_MEM_STATISTIC is set
*/
LSA_VOID_PTR_TYPE eps_mem_alloc_internal( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type, LSA_UINT16 line, LSA_CHAR* file )
{
	return eps_mem_alloc_from_pool(size, lsa_comp_id, mem_type, g_pEpsMem->sStdMem.pool_id, line, file);
}

/**
* @brief Frees the given memory block
* 
* @param [in] ptr          - pointer to the memory block that shall be freed
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
* @param [in] pool_name    - not used in statistics, but required for mem3
* @return #LSA_RET_OK      - the memory was freed successfully
* @return #LSA_RET_ERR     - The memory block is unknown to the memory management
*/
LSA_UINT16 eps_mem_free( LSA_VOID_PTR_TYPE ptr, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type )
{
	return eps_mem_free_from_pool(ptr, lsa_comp_id, mem_type, g_pEpsMem->sStdMem.pool_id);
}

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/**
* @brief Allocates a memory block with the given size from the internal fast memory pool that was provided by eps_mem_init
* 
* @param [in] size         - size of the block
* @param [in] lsa_comp_id  - comp_id, see lsa_sys.h and psi_sys.h
* @param [in] mem_type     - memory type, see psi_sys.h
*/
LSA_VOID_PTR_TYPE eps_fast_mem_alloc( LSA_UINT32 size, LSA_UINT32 lsa_comp_id, LSA_UINT32 mem_type )
{
	LSA_UNUSED_ARG(mem_type); /* Mem Types not supported for fast mem */
	LSA_UNUSED_ARG(lsa_comp_id);

	return eps_mem_alloc_from_pool(size, 0 /* Don't Check CompId */, 0, g_pEpsMem->sFastMem.pool_id, (LSA_UINT16) __LINE__ /* currently no mapping from original caller */, __FILE__ /* currently no mapping from original caller */);
}

/**
* @brief Frees the given memory block
* 
* @param [in] ptr          - pointer to the memory block that shall be freed
*/
LSA_UINT16 eps_fast_mem_free( LSA_VOID_PTR_TYPE ptr )
{
	return eps_mem_free_from_pool(ptr, 0 /* Don't Check CompId */, 0, g_pEpsMem->sFastMem.pool_id);
}

/**
* @brief Checks if the given memory pointer is overwritten at the end.
* 
* Note: This function uses the functionality provided by mem3. This function only checks if the pattern at the end of the block was overwritten.
* 
* @param [in] pBuf  - Pointer to the buffer that needs to be checked
* @return LSA_TRUE  - Buffer is valid (not overwritten at the end)
* @return LSA_FALSE - Buffer is corrupt. The magic pattern was overwritten at the end.
*/
LSA_BOOL eps_mem_verify_buffer (LSA_VOID_PTR_TYPE pBuf)
{
// If the memory statistics are used the eps_mem _alloc doesn't return the pointer which mem3 actually returned. Therefore it is necessary that we recalculate the original mem3 pointer, to use VerifyMem3Buf.
#ifdef EPS_USE_MEM_STATISTIC
	return VerifyMem3Buf((LSA_UINT8*)pBuf - sizeof(EPS_MEM_STATISTIC_PREFIX_TYPE));
#else
	return VerifyMem3Buf(pBuf);
#endif
}

/**
* @brief Checks if there are still memory blocks allocated in the local memory pool.
* If the define EPS_USE_MEM_STATISTIC is set, information are added in the trace with the file + linenumber where the memory block was allocated.
* @see eps_mem_check_pool_by_statistics - detailled checks are done here.
* 
* @return LSA_TRUE  - there are no more allocated blocks
* @return LSA_FALSE - there is at least one allocated block left.
*/
LSA_BOOL eps_mem_check_pool ()
{  
    #ifdef EPS_USE_MEM_STATISTIC
    return eps_mem_check_pool_by_statistics();
    #else
    int BufNumUsed = 0;
    BufNumUsed = BufNumUsedInPool( g_pEpsMem->sStdMem.pool_nr );

    if( BufNumUsed == 0 )
    {
        EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_mem_check_pool(): Check successful. There are no more blocks in use");
        return LSA_TRUE;
    }
    else
    {
        EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE_HIGH,"eps_mem_check_pool(): There are still (%d) blocks in use", BufNumUsed );
        return LSA_FALSE;
    }
    #endif
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

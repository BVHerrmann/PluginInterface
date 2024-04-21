/*****************************************************************************/
/*    Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.    */
/*****************************************************************************/
/*    This program is protected by German copyright law and international    */
/*    treaties. The use of this software including but not limited to its    */
/*    Source Code is subject to restrictions as agreed in the license        */
/*    agreement between you and Siemens.                                     */
/*    Copying or distribution is not allowed unless expressly permitted      */
/*    according to your license agreement with Siemens.                      */
/*****************************************************************************/
/*                                                                           */
/*    P r o j e c t         &P: PROFINET IO Runtime Software          :P&    */
/*                                                                           */
/*    P a c k a g e         &W: PROFINET IO Runtime Software          :W&    */
/*                                                                           */
/*    C o m p o n e n t     &C: HIF (Host Interface)                  :C&    */
/*                                                                           */
/*    F i l e               &F: hif_mem_malloc.c                          :F&    */
/*                                                                           */
/*    V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15     :V&    */
/*                                                                           */
/*    D a t e    (YYYY-MM-DD) &D: 2019-08-05                          :D&    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*    D e s c r i p t i o n :                                                */
/*                                                                           */
/*    Implements heap management                                             */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID    37
#define HIF_MODULE_ID        LTRC_ACT_MODUL_ID


#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>

#include "hif_int.h"
#include "hif_mem_malloc.h"
#include "hif_list.h"

#if ( HIF_CFG_USE_HIF == 1 )

/**********************************************************************
 * Types
 *********************************************************************/
/**
 * Enumeration of states of a memory block.
 *
 * @see #HIF_MEM_MALLOC_BLOCK_DESC_TYPE
 */
typedef enum hif_mem_malloc_state
{
    HIF_MALLOC_USED= 0x02ed02ed,             ///< Memory block is currently used
    HIF_MALLOC_FREE= 0x0fee0fee              ///< Memory block is currently free
} HIF_MEM_MALLOC_STATE_TYPE;

/**
 * Structure to store information about free or allocated memory blocks.
 *
 * @see hif_mem_malloc_data_ptr->c
 */
typedef struct hif_mem_malloc_block_desc
{
    HIF_LIST_TYPE allocated_blocks;         ///< Used for catenation of allocated blocks (used AND free)
    HIF_LIST_TYPE used_free_blocks;         ///< Catenation for used/free list
    HIF_MEM_MALLOC_STATE_TYPE state;        ///< State of the block (free/used)
    LSA_UINT32 size;                        ///< Size of the memory block (including the block descriptor)
} HIF_MEM_MALLOC_BLOCK_DESC_TYPE, *HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE;

/**
 * Type to encapsulate memory manegment for one hif heap pool needed by hif_mem_malloc
 */
typedef struct hif_mem_malloc
{
    /**
     * Mutex used to protect the memory management variables
     */
    LSA_UINT16 enter_exit_id;
    /**
     * List of allocated blocks (used AND free) managed by the operating system.
     *
     * On startup this list contains only one free block. If a block of memory
     * is requested, this block is split up into two blocks:
     *    - The first is marked as used and gets the requested size
     *    - The second is marked as free and gets the remaining memory.
     * The second block may then be split up by further requests.
     *
     * If block are freed by calling #hif_mem_free(), the block is marked as free and
     * the adjacent blocks are tested if they are also free. If two adjacent
     * blocks are free, then they are merged into one big block.
     *
     * If all previously allocated blocks are freed, then there will be only
     * one big free block in this list, like at the startup of the system.
     */
    HIF_LIST_TYPE allocated_blocks;
    /**
     * List of free blocks. If a new block is requested via malloc, only this
     * list of free resources has to be searched.
     */
    HIF_LIST_TYPE free_blocks;
    LSA_UINT32 used_heap;       ///< currently used heap in bytes
    LSA_UINT32 configured_heap; ///< configured heap size in bytes
    LSA_UINT32 max_used_heap;   ///< 'high water mark'; maximum of used heap so far in bytes
    LSA_UINT32 blocks_used;     ///< currently used blocks
    LSA_UINT32 blocks_free;     ///< currently free blocks
    LSA_UINT32 malloc_calls;    ///< number of calls to hif_mem_malloc() and hif_mem_calloc()
    LSA_UINT32 free_calls;      ///< number of calls to hif_mem_free()
    LSA_UINT32 realloc_calls;   ///< number of calls to hif_mem_realloc()
} HIF_MEM_MALLOC_TYPE, *HIF_MEM_MALLOC_PTR_TYPE;

/// Size of the block descriptor aligned to the specified alignment
#define HIF_MEM_MALLOC_BLOCK_DESC_SIZE (((sizeof(HIF_MEM_MALLOC_BLOCK_DESC_TYPE)+(HIF_CFG_MEM_MALLOC_ALIGN)-1) / HIF_CFG_MEM_MALLOC_ALIGN) * HIF_CFG_MEM_MALLOC_ALIGN)

/**********************************************************************
 * External variables
 *********************************************************************/


/**********************************************************************
 * Global variables
 *********************************************************************/
#if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
HIF_MEM_MALLOC_TYPE hif_mem_malloc_pools[HIF_CFG_MEM_MALLOC_POOL_MAX] = {{0}}; // init with zeros to mark all pools as free 
#endif
/**********************************************************************
 * Functions
 *********************************************************************/

/**
 * \brief Init heap for hif_mem_malloc
 * 
 * @param [in] heap_begin Pointer to begin of heap
 * @param [in] heap_size Size of heap
 * @param [in] hif_mem_malloc_helper_ptr Pointer for memory management data of pool (if LSA_NULL) begin of heap will be used
 * 
 * @return Pointer to memory management data of pool
 */
LSA_VOID_PTR_TYPE hif_mem_malloc_init(  LSA_UINT8* heap_begin,
                                        LSA_UINT32 heap_size,
                                        LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    LSA_UINT8 *begin;
    LSA_UINT32 size;
    LSA_UINT32 remainder;
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE big_free_block; // the one big block of free memory available after initialization
    LSA_UINT16 rsp;
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr;
    
    if(hif_mem_malloc_helper_ptr != LSA_NULL)
    {
        // use given helper pointer for memory management data of pool
        hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
        begin = heap_begin;
        size = heap_size;
    }
    else
    {
        hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE)HIF_CAST_TO_VOID_PTR(heap_begin);
        size = heap_size;
        // make sure hif_mem_malloc_data_ptr fits the requested alignment
        remainder = (LSA_UINT32)hif_mem_malloc_data_ptr % HIF_CFG_MEM_MALLOC_ALIGN;
        if(remainder != 0)
        {
            hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE)HIF_CAST_TO_VOID_PTR(heap_begin + HIF_CFG_MEM_MALLOC_ALIGN - remainder);
            HIF_ASSERT(size > HIF_CFG_MEM_MALLOC_ALIGN);
            size -= HIF_CFG_MEM_MALLOC_ALIGN;
        }
		begin = (LSA_UINT8*)hif_mem_malloc_data_ptr + sizeof(HIF_MEM_MALLOC_TYPE);
		HIF_ASSERT(size > sizeof(HIF_MEM_MALLOC_TYPE));
		size -= sizeof(HIF_MEM_MALLOC_TYPE);
    }
    
    HIF_ALLOC_REENTRANCE_LOCK(&rsp, &hif_mem_malloc_data_ptr->enter_exit_id );
    HIF_ASSERT(rsp == LSA_RET_OK);
    
    // make sure begin and size fits the requested alignment
    remainder = (LSA_UINT32)begin % HIF_CFG_MEM_MALLOC_ALIGN;
    if(remainder != 0)
    {
        begin += HIF_CFG_MEM_MALLOC_ALIGN - remainder;
        HIF_ASSERT(size > HIF_CFG_MEM_MALLOC_ALIGN);
        size -= HIF_CFG_MEM_MALLOC_ALIGN;
    }

    // initialize the heap memory if HIF is configured to do so
#if (defined HIF_MEM_MALLOC_CLEAR)
    {
        unsigned long i;
        LSA_UINT32 *heap_word=(LSA_UINT32*)begin;
        for (i=0; i<(size/sizeof(LSA_UINT32)); i++)
        {
            heap_word[i]=HIF_MEM_MALLOC_CLEAR_PATTERN;
        }
    }
#endif // #if (defined HIF_MEM_MALLOC_CLEAR)
    // the calculated pool begin and size has to be inside the requested memory 
    HIF_ASSERT(begin + size <= heap_begin + heap_size);
    
    // Initialize the big free block. This block takes the whole memory available.
    big_free_block=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)HIF_CAST_TO_VOID_PTR(begin);
    big_free_block->size=size;
    big_free_block->state=HIF_MALLOC_FREE;
    HIF_INIT_LIST(&big_free_block->allocated_blocks);
    HIF_INIT_LIST(&big_free_block->used_free_blocks);

    // initialize the list of allocated blocks (used AND free)
    HIF_INIT_LIST(&hif_mem_malloc_data_ptr->allocated_blocks);
    // initialize the list of free blocks
    HIF_INIT_LIST(&hif_mem_malloc_data_ptr->free_blocks);

    hif_mem_malloc_data_ptr->used_heap=0;
    hif_mem_malloc_data_ptr->configured_heap=size;
    hif_mem_malloc_data_ptr->blocks_free=1;
    hif_mem_malloc_data_ptr->blocks_used=0;
    hif_mem_malloc_data_ptr->free_calls=0;
    hif_mem_malloc_data_ptr->malloc_calls=0;
    hif_mem_malloc_data_ptr->realloc_calls=0;
    hif_mem_malloc_data_ptr->max_used_heap=0;

    // add the only big free block available to the list of allocated blocks...
    hif_list_add(&big_free_block->allocated_blocks, &hif_mem_malloc_data_ptr->allocated_blocks);
    // ...and to the list of free blocks
    hif_list_add(&big_free_block->used_free_blocks, &hif_mem_malloc_data_ptr->free_blocks);
    
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Initialized memory pool: 0x%x begin: 0x%x size: 0x%x.", hif_mem_malloc_helper_ptr, begin, size);

    return hif_mem_malloc_data_ptr;
}


/**
 * Undo init heap for hif_mem_malloc
 * 
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 */
LSA_VOID hif_mem_malloc_undo_init(LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    LSA_UINT16 rsp;
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    HIF_FREE_REENTRANCE_LOCK(&rsp, hif_mem_malloc_data_ptr->enter_exit_id);
    HIF_ASSERT(rsp == LSA_RET_OK);
    HIF_MEMSET(hif_mem_malloc_data_ptr, 0, sizeof(HIF_MEM_MALLOC_TYPE));
    
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Uninitialized memory pool: 0x%x.", hif_mem_malloc_helper_ptr);
}

/**
 * Create hif heap pool
 * 
 * @param [in] begin Pointer to begin of poll
 * @param [in] size Desired size of pool
 *
 * @return Pool number
 */
LSA_INT hif_mem_malloc_create_pool(LSA_UINT8* begin, LSA_UINT32 size)
{
    LSA_INT pool_nr = -1;
    
#if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
    LSA_INT pool_idx;
    
    HIF_ENTER();
    
    for ( pool_idx = 0; pool_idx < HIF_CFG_MEM_MALLOC_POOL_MAX; pool_idx++ )
    {
        if ( 0 == hif_mem_malloc_pools[pool_idx].configured_heap )
        {
            hif_mem_malloc_init(begin, size, &hif_mem_malloc_pools[pool_idx]);
            pool_nr = pool_idx + 1;
            break;
        }
    }
    
    HIF_EXIT();
#else
    LSA_UNUSED_ARG( begin );
    LSA_UNUSED_ARG( size );
#endif
    
    return pool_nr;
}

/**
 * Delete hif heap pool
 * 
 * @param [in] pool_nr Pool number created on hif_mem_malloc_create_pool
 */
LSA_VOID hif_mem_malloc_delete_pool(LSA_INT pool_nr)
{
    HIF_ASSERT(pool_nr > 0 && pool_nr <= HIF_CFG_MEM_MALLOC_POOL_MAX);
    #if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
    HIF_ENTER();
    hif_mem_malloc_undo_init(&hif_mem_malloc_pools[pool_nr-1]);
    HIF_EXIT();
    #else
    LSA_UNUSED_ARG(pool_nr);
    #endif
}

/**
 * Extend the HIF heap by a memory block that starts at \c address
 * and has a size of \c size bytes.
 * 
 * @param [in] address Start address of the memory block to extend the heap
 * @param [in] size Size of the memory block to extend the heap in bytes.
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 * 
 * @return 0 upon successful completion, otherwise
 *        - #EINVAL if lenght of the given memory block is to short    
 */
LSA_INT hif_mem_malloc_heap_extend(LSA_VOID_PTR_TYPE address, LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    LSA_UINT8 *heap_extend_block_begin;
    LSA_UINT32 heap_extend_block_size;
    LSA_UINT32 remainder;
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE heap_extend_block; // the one big block to extend the heap
    
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    heap_extend_block_begin=(LSA_UINT8*)address;
    heap_extend_block_size=size;
    // make sure begin and size fits the requested alignment
    remainder = (LSA_UINT32)heap_extend_block_begin % HIF_CFG_MEM_MALLOC_ALIGN;
    if (remainder != 0)
    {
        heap_extend_block_begin += HIF_CFG_MEM_MALLOC_ALIGN - remainder;
        heap_extend_block_size -= HIF_CFG_MEM_MALLOC_ALIGN;
    }

    if (heap_extend_block_size<HIF_MEM_MALLOC_BLOCK_DESC_SIZE)
    {
        return EINVAL;
    }
    
    // initialize the heap extend memory if HIF is configured to do so
#if (defined HIF_MEM_MALLOC_CLEAR)
    {
        unsigned long i;
        LSA_UINT32 *heap_word=(LSA_UINT32*)heap_extend_block_begin;
        for (i=0; i<(heap_extend_block_size/sizeof(LSA_UINT32)); i++)
        {
            heap_word[i]=HIF_MEM_MALLOC_CLEAR_PATTERN;
        }
    }
#endif // #if (defined HIF_MEM_MALLOC_CLEAR)
    
    // Initialize the big free block. This block takes the whole additional memory available.
    heap_extend_block=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)HIF_CAST_TO_VOID_PTR(heap_extend_block_begin);
    heap_extend_block->size=heap_extend_block_size;
    heap_extend_block->state=HIF_MALLOC_FREE;
    HIF_INIT_LIST(&heap_extend_block->allocated_blocks);
    HIF_INIT_LIST(&heap_extend_block->used_free_blocks);

    // protect memory management data structures
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    hif_mem_malloc_data_ptr->configured_heap += heap_extend_block_size;
    hif_mem_malloc_data_ptr->blocks_free += 1;

    // add the big free block to the list of allocated blocks...
    hif_list_add(&heap_extend_block->allocated_blocks, &hif_mem_malloc_data_ptr->allocated_blocks);
    // ...and to the list of free blocks
    hif_list_add(&heap_extend_block->used_free_blocks, &hif_mem_malloc_data_ptr->free_blocks);

    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);
    
    return 0;
}

/**
 * Reduce the HIF heap by the one free block of memory that starts at \c address.
 * 
 * @param [in] address Start address of the memory block that should be removed from the heap
 * @param [in] size Size of the memory block that was used for the call to hif_mem_malloc_heap_extend().
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 * 
 * @return 0 upon successful completion, otherwise
 *        - #EINVAL if the given address does not point to an HIF_MEM_MALLOC_BLOCK_DESC_TYPE
 *        in the state HIF_MALLOC_FREE or if the size of the block does not equal
 *        the size passed as argument
 */
LSA_INT hif_mem_malloc_heap_reduce(LSA_VOID_PTR_TYPE address, LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    LSA_INT rv;
    LSA_UINT32 remainder;
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE heap_extend_block; // the one big block to extend the heap
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    // calculate address of first block descriptor within the block (it is aligned to HIF_CFG_MEM_MALLOC_ALIGN)
    remainder = (LSA_UINT32)address % HIF_CFG_MEM_MALLOC_ALIGN;
    if (remainder != 0)
    {
        address = (LSA_VOID_PTR_TYPE)(((LSA_UINT32)address)+HIF_CFG_MEM_MALLOC_ALIGN - remainder);
        size -= HIF_CFG_MEM_MALLOC_ALIGN;
    }
    
    // block that was used to extend the heap
    heap_extend_block=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)address;

    // protect memory management data structures
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);
    
    if ((heap_extend_block->state == HIF_MALLOC_FREE) &&
        (heap_extend_block->size == size))
    {
        // add the big free block to the list of allocated blocks...
        hif_list_del_init(&heap_extend_block->allocated_blocks);
        // ...and to the list of free blocks
        hif_list_del_init(&heap_extend_block->used_free_blocks);
        
        hif_mem_malloc_data_ptr->configured_heap -= heap_extend_block->size;
        hif_mem_malloc_data_ptr->blocks_free -= 1;
        
        rv = 0;
    }
    else
    {
        // could not remove the memory block from the HIF heap,
        // since it is either not free or has a different size as specified
        rv = EINVAL;
    }

    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);
    
    return rv;
}

/**
 * Deallocates the memory pointed to by \c ptr.
 * \c ptr has to be a pointer to a memory allocated by hif_mem_malloc() or    hif_mem_calloc().
 * If ptr is not a pointer to a previously allocated memory block, no action is taken.
 *
 * @param [in] ptr Pointer to a memory block previously allocated by hif_mem_malloc() or hif_mem_calloc()
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 */
LSA_VOID hif_mem_free(LSA_VOID_PTR_TYPE ptr, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE block_desc, predecessor, successor;
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;

    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

#if (defined HIF_MEM_MALLOC_VERIFY)
    // check if the pool is still valid
    if( 0 != hif_mem_malloc_verify(hif_mem_malloc_data_ptr))
    {
        // something went wrong. Overwrites are not accepted by concept -> throw FATAL
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "Mem free failed for ptr = 0x%x, hif_mem_malloc_helper_ptr = 0x%x.",ptr, hif_mem_malloc_helper_ptr);
        HIF_FATAL(0);
    }
#endif

    if (ptr==LSA_NULL)
    {
        return;
    }

    // protect memory management data structures
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    // the block descriptor is stored in front of each block
    block_desc=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)(((LSA_UINT32)ptr)-HIF_MEM_MALLOC_BLOCK_DESC_SIZE);

    // test if the end of the list was reached or the block is already free
    if (block_desc->state != HIF_MALLOC_USED)
    {
        // if so, no block for the given pointer could be found
        HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);
        // enter error state

        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_mem_free() - block_desc->state (%d) != HIF_MALLOC_USED", block_desc->state);
        HIF_FATAL(0);
        //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
        return;
    }

    hif_mem_malloc_data_ptr->free_calls++;

    hif_mem_malloc_data_ptr->used_heap-=block_desc->size;

    block_desc->state=HIF_MALLOC_FREE;
    
    // ... and add it the list of free blocks
    hif_list_add(&block_desc->used_free_blocks, &hif_mem_malloc_data_ptr->free_blocks);

    hif_mem_malloc_data_ptr->blocks_free++;
    hif_mem_malloc_data_ptr->blocks_used--;

    predecessor=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)block_desc->allocated_blocks.prev;
    // make sure predecessor is not the head of the list and try to join the freed block with its predecessor
    if (((LSA_VOID_PTR_TYPE)predecessor != (LSA_VOID_PTR_TYPE)&hif_mem_malloc_data_ptr->allocated_blocks)
            && (predecessor->state == HIF_MALLOC_FREE)
            && (((LSA_UINT32)&predecessor->allocated_blocks + predecessor->size) == (LSA_UINT32)&block_desc->allocated_blocks)) // there must be no gap between the two blocks, if they shall be joined
    {
        // join block_desc and its predecessor
        predecessor->size += block_desc->size;

        // remove the current block from the list of allocated blocks ...
        hif_list_del(&block_desc->allocated_blocks);
        // remove the current block from the list of free blocks
        hif_list_del(&block_desc->used_free_blocks);
        block_desc=predecessor;
        hif_mem_malloc_data_ptr->blocks_free--;
    }

    // make sure successor is not the head of the list and try to join the freed block with its successor
    successor=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)block_desc->allocated_blocks.next;
    if (((LSA_VOID_PTR_TYPE)successor != (LSA_VOID_PTR_TYPE)&hif_mem_malloc_data_ptr->allocated_blocks)
            && (successor->state == HIF_MALLOC_FREE)
            && (((LSA_UINT32)&block_desc->allocated_blocks + block_desc->size) == (LSA_UINT32)&successor->allocated_blocks)) // there must be no gap between the two blocks, if they shall be joined
    {
        // join block_desc with its successor
        block_desc->size += successor->size;
        // remove the successor from the list of allocated blocks ...
        hif_list_del(&successor->allocated_blocks);
        // remove the former successor from the list of free blocks
        hif_list_del(&successor->used_free_blocks);
        hif_mem_malloc_data_ptr->blocks_free--;
    }

    //trace-point: "Deallocate memory object."
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "Deallocate memory object 0x%x.",ptr);

    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

#if (defined HIF_MEM_MALLOC_VERIFY)
    hif_mem_malloc_verify(hif_mem_malloc_data_ptr);
#endif

    return;
}

#if (defined HIF_MEM_MALLOC_VERIFY)
/**
 * Tests whether the block with the given address and size is valid. It is tested whether
 * the given block intersects any other memory block previously returned by malloc.
 * If an invalid block was found, the system is taken into error state by calling #hif_fatal
 *
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 * @return 0 if the block is valid. Otherwise the error state is entered
 */
LSA_INT hif_mem_malloc_verify(LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    LSA_UINT32 prev_block_begin=0;
    LSA_UINT32 current_block_begin;
    LSA_UINT32 current_block_end;

    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE block_desc;
    
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;

    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    // iterate over all memory blocks
    block_desc=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)hif_mem_malloc_data_ptr->allocated_blocks.next;
    while (block_desc != (HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)&hif_mem_malloc_data_ptr->allocated_blocks)    // end of list reached?
    {
            current_block_begin=(LSA_UINT32)block_desc; // begin of the current block to compare the new one with
            current_block_end=current_block_begin+block_desc->size; // end of the current block to compare the new one with
            if (block_desc->state!=HIF_MALLOC_FREE
                && block_desc->state!=HIF_MALLOC_USED)
            {
            // enter error state
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_mem_malloc_verify() - BLock descriptor state (0x%x) is not valid", block_desc->state);
                HIF_FATAL(0);
            }
            prev_block_begin=current_block_begin;
            // get next block
            block_desc=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)block_desc->allocated_blocks.next;
    }

    // the block was valid --> return 0
    return 0;
}

/**
 * Tests whether the block with the given address is valid. It is tested whether
 * the given block intersects any other memory block previously returned by malloc.
 * If an invalid block was found, the system is taken into error state by calling #hif_fatal
 * This function is used if the memory was alloced using hif_mem_malloc_from_pool
 *
 * @param [in] ptr Pointer to an allocated memory section
 * @param [in] pool_nr Number of the pool to verify
 * @return 0 if the block is valid. Otherwise the error state is entered
 */
LSA_INT hif_mem_malloc_verify_by_pool(LSA_VOID_PTR_TYPE ptr, LSA_INT pool_nr)
{
    LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr = LSA_NULL;
    LSA_UNUSED_ARG(ptr);
#if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
    if(pool_nr > 0 && pool_nr <= HIF_CFG_MEM_MALLOC_POOL_MAX)
    {
        hif_mem_malloc_helper_ptr = &hif_mem_malloc_pools[pool_nr-1];
    }
    else
    {
         HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_mem_malloc_verify_by_pool FATAL - pool unknown. pool_nr = %d", pool_nr);
         HIF_FATAL(0);
    }
#else
	LSA_UNUSED_ARG(pool_nr);
#endif
    return hif_mem_malloc_verify( hif_mem_malloc_helper_ptr );
}

#endif // (defined HIF_MEM_MALLOC_VERIFY)

/**
 * Allocates a new memory block of the given size.
 * The list of allocated and free memory blocks (#hif_mem_malloc_data_ptr->allocated_blocks) is searched for a free memory block of the given size (first fit).
 * If found and if resources for an additional block descriptor (#HIF_MEM_MALLOC_BLOCK_DESC_TYPE) are available, the block found is split up into
 * two blocks.
 *    # The first block will get the requested size and is marked is used
 *    # The second block gets the remaining memory is marked as free.
 *
 * If no resources for an additional block descriptor are available, the allocated blocks are searched for a block that fits best for the
 * requested size (best fit). This block is marked as used and returned. Note that the block will most of the time be greater than the
 * requested size.
 *
 * @see #hif_mem_malloc_data_ptr->allocated_blocks
 * @see #hif_mem_malloc_data_ptr->free_blocks
 *
 * @param [in] size The size of the memory block to be allocated.
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 *
 * @return Upon successful completion a pointer to the allocated memory is returned.
 *        If \c size is 0, \c LSA_NULL is returned. If no memory is available, \c LSA_NULL is returned
 *        and #errno is set to #ENOMEM
 */
LSA_VOID_PTR_TYPE hif_mem_malloc(LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE block_desc; // used to iterate over the lists
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE suitable_block_desc; // holds the block that will fulfil the memory request
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE new_free_block; // When a free block is split up into a used and a new free block, this variable holds the new free block
    LSA_UINT32 fit_size;
    LSA_UINT32 org_size=size;
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

#if (defined HIF_MEM_MALLOC_VERIFY)
    hif_mem_malloc_verify(hif_mem_malloc_data_ptr);
#endif

    // It makes no sense to allocate memory of length 0
    if (size == 0)
    {
        return LSA_NULL;
    }

    // increase the block size in order to be able to store a pointer to the block descriptor
    size+=HIF_MEM_MALLOC_BLOCK_DESC_SIZE;
    if (org_size>size) // overflow?
    {
        errno = ENOMEM;
        return LSA_NULL;
    }

    // make sure the size fits the requested alignment
    if ((size % HIF_CFG_MEM_MALLOC_ALIGN) != 0)
    {
        fit_size = (size + HIF_CFG_MEM_MALLOC_ALIGN) - (size % HIF_CFG_MEM_MALLOC_ALIGN);

        // detect overflow
        if (fit_size < size)
        {
            errno = ENOMEM;
            return LSA_NULL;
        }
        size = fit_size;
    }

    // searching / manipulating the memory management data structures has to be protected
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    // find a block of which has at least the requested size
    suitable_block_desc=LSA_NULL;
    block_desc=HIF_LIST_STRUCT_BASE(HIF_MEM_MALLOC_BLOCK_DESC_TYPE, used_free_blocks, hif_mem_malloc_data_ptr->free_blocks.next);
    while (!HIF_LIST_END(block_desc, used_free_blocks, hif_mem_malloc_data_ptr->free_blocks)) // end of list?
    {
        if (block_desc->size >= size)
        {
            suitable_block_desc=block_desc;
            break;
        }
        else
        {
            block_desc=HIF_LIST_STRUCT_BASE(HIF_MEM_MALLOC_BLOCK_DESC_TYPE, used_free_blocks, block_desc->used_free_blocks.next);
        }
    }

    // no free memory was found
    if (suitable_block_desc == LSA_NULL)
    {
        HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

        //trace-point: "Error from malloc: No free memory was found."
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH,"Error from hif_mem_malloc: No free memory was found. Needed size is 0x%x", size);

        errno = ENOMEM;
        return LSA_NULL;
    }

    hif_mem_malloc_data_ptr->malloc_calls++;
#if (defined HIF_MEM_MALLOC_VERIFY)
    hif_mem_malloc_verify(hif_mem_malloc_data_ptr);
#endif

    // The found suitable block will be returned, so mark it as used
    suitable_block_desc->state=HIF_MALLOC_USED;
    // remove the block from the list of free blocks...
    hif_list_del(&suitable_block_desc->used_free_blocks);
    hif_mem_malloc_data_ptr->blocks_free--;
    hif_mem_malloc_data_ptr->blocks_used++;
    // Allocate a new free block?
    if ((suitable_block_desc->size - size) > HIF_MEM_MALLOC_BLOCK_DESC_SIZE)
    {
        // calculate address of the block descriptor of the new free block
        new_free_block=(HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE)((LSA_UINT32)suitable_block_desc + size);
        HIF_INIT_LIST(&new_free_block->allocated_blocks); // initialize list elements
        HIF_INIT_LIST(&new_free_block->used_free_blocks);
        // mark the new block as free
        new_free_block->state=HIF_MALLOC_FREE;
        new_free_block->size=suitable_block_desc->size - size;

        // the found suitable block will get the requested size
        suitable_block_desc->size=size;
        // enqueue the block of free memory AFTER the the block of used memory
        hif_list_add(&new_free_block->allocated_blocks, &suitable_block_desc->allocated_blocks);
        // add the new free block to the list of free blocks
        hif_list_add(&new_free_block->used_free_blocks, &hif_mem_malloc_data_ptr->free_blocks);
        hif_mem_malloc_data_ptr->blocks_free++;
    }

    //trace-point: "Allocates memory block at with size."
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "Allocates memory block at 0x%x with size %d.",((LSA_UINT32)suitable_block_desc+HIF_MEM_MALLOC_BLOCK_DESC_SIZE),size);

    hif_mem_malloc_data_ptr->used_heap+=suitable_block_desc->size;
    if (hif_mem_malloc_data_ptr->used_heap>hif_mem_malloc_data_ptr->max_used_heap)
    {
        hif_mem_malloc_data_ptr->max_used_heap=hif_mem_malloc_data_ptr->used_heap;
    }

    // release the spinlock protecting the memory management data structures
    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

#if (defined HIF_MEM_MALLOC_VERIFY)
    hif_mem_malloc_verify(hif_mem_malloc_data_ptr);
#endif

    // return the block increased by the space needed for the pointer to the block descriptor
    return (LSA_VOID_PTR_TYPE)((LSA_UINT32)suitable_block_desc+HIF_MEM_MALLOC_BLOCK_DESC_SIZE);
}

/**
 * Allocates a new memory block of the given size from hif heap pool.
 *
 * @param [in] size Desired size of the memory section
 * @param [in] pool_nr Number of pool to allocate from
 *
 * @return Pointer to allocated memory, LSA_NULL if no memory is available
 */
LSA_VOID_PTR_TYPE hif_mem_malloc_from_pool(LSA_UINT32 size, LSA_INT pool_nr)
{
    LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr = LSA_NULL;
#if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
    if(pool_nr > 0 && pool_nr <= HIF_CFG_MEM_MALLOC_POOL_MAX)
    {
        hif_mem_malloc_helper_ptr = &hif_mem_malloc_pools[pool_nr-1];
    }
#else
    LSA_UNUSED_ARG( pool_nr );
#endif
    return hif_mem_malloc(size, hif_mem_malloc_helper_ptr);
}

/**
 * Deallocates the memory pointed to by \c ptr from hif heap pool.
 *
 * @param [in] ptr Pointer to allocated memory
 * @param [in] pool_nr Number of pool to free memory from
 */
LSA_VOID hif_mem_free_from_pool(LSA_VOID_PTR_TYPE ptr, LSA_INT pool_nr)
{
    LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr = LSA_NULL;
#if (HIF_CFG_MEM_MALLOC_POOL_MAX > 0)
    if(pool_nr > 0 && pool_nr <= HIF_CFG_MEM_MALLOC_POOL_MAX)
    {
        hif_mem_malloc_helper_ptr = &hif_mem_malloc_pools[pool_nr-1];
    }
#else
    LSA_UNUSED_ARG( pool_nr );
#endif
    hif_mem_free(ptr, hif_mem_malloc_helper_ptr);
}

/**
 * Stores in the location of \c heap_info the information about the
 * current state of the HIF heap.
 * Information includes
 *    - the configured heap size in bytes
 *    - the currently used heap in bytes
 *    - the maximum heap usage so far in bytes
 *    - current number of used blocks
 *    - current number of free blocks
 *    - number of calls to malloc
 *    - number of calls to free
 *    - number of calls to realloc
 *
 * @see hif_mem_malloc_get_heap_fragmentation()
 *
 * @param [in] heap_info pointer to #HIF_MEM_MALLOC_HEAP_INFO_TYPE where the current heap status is stored to
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 *
 * @return 0 upon successful completion
 */
LSA_INT hif_mem_malloc_get_heap_info(HIF_MEM_MALLOC_HEAP_INFO_PTR_TYPE heap_info, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{        
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    // lock heap in order to get consistent information
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    heap_info->blocks_free=hif_mem_malloc_data_ptr->blocks_free;
    heap_info->blocks_used=hif_mem_malloc_data_ptr->blocks_used;
    heap_info->free_calls=hif_mem_malloc_data_ptr->free_calls;
    heap_info->malloc_calls=hif_mem_malloc_data_ptr->malloc_calls;
    heap_info->max_used=hif_mem_malloc_data_ptr->max_used_heap;
    heap_info->realloc_calls=hif_mem_malloc_data_ptr->realloc_calls;
    heap_info->size=hif_mem_malloc_data_ptr->configured_heap;
    heap_info->used=hif_mem_malloc_data_ptr->used_heap;

    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    return 0;
}

/**
 * Calculates and returns in the location of \c fragmentation the heap fragmentation as a <b>floating point</b> value
 * in the range of 0<=fragmentation<1.
 * Fragmentation value means the following:
 *    - 0: No fragmentation, there is only one free block available that covers the whole free heap (best case)
 *    - close to 1: There are a lot of very small free blocks in the system (worst case)
 *
 * The heap fragmentation is calculated as follows:
 * Given: nf=number of free blocks on the heap
 *                sf(i)=size of the free block number i (i=[1..nf])
 *
 *    fragmentation = 1 - (sum[i=1;nf](sf(i) ^ 2) / sum[i=1;nf](sf(i)) ^ 2)
 *
 * In words: 1 minus the sum of the squares of all free blocks sizes divided by the square
 *                     of the sum of all free blocks sizes.
 *
 * \attention The heap calculation makes use of floating point variables! The thread that calls this function may be marked as
 *        'floating point user' which may increase thread switching times from/to the thread, depending on the floating point configuration
 *        of your product
 *
 * @see hif_mem_malloc_get_heap_info
 *
 * @param [in] fragmentation Pointer to a location where the fragmentation should be stored
 * @param [in] hif_mem_malloc_helper_ptr Pointer to memory management data of pool
 *
 * @return 0 upon successful completion otherwise
 *    - #EINVAL If there is no free memory available and the calculation would result in a devision by zero
 */
LSA_INT hif_mem_malloc_get_heap_fragmentation(double *fragmentation, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr)
{
    HIF_MEM_MALLOC_BLOCK_DESC_PTR_TYPE block_desc;
    double sum_of_squares=0.0;
    double sum=0.0;
    double sumSq;
    LSA_INT rv=0;
    HIF_MEM_MALLOC_PTR_TYPE hif_mem_malloc_data_ptr = (HIF_MEM_MALLOC_PTR_TYPE) hif_mem_malloc_helper_ptr;
    
    HIF_ASSERT(HIF_IS_NOT_NULL(hif_mem_malloc_data_ptr));

    // lock heap in order to get consistent information
    HIF_ENTER_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    // calculate fragmentation
    // iterate over all free blocks
    block_desc=HIF_LIST_STRUCT_BASE(HIF_MEM_MALLOC_BLOCK_DESC_TYPE, used_free_blocks, hif_mem_malloc_data_ptr->free_blocks.next);
    while (!HIF_LIST_END(block_desc, used_free_blocks, hif_mem_malloc_data_ptr->free_blocks)) // end of list?
    {
        sum+=(double)block_desc->size;
        sum_of_squares+=((double)block_desc->size*block_desc->size);
        block_desc=HIF_LIST_STRUCT_BASE(HIF_MEM_MALLOC_BLOCK_DESC_TYPE, used_free_blocks, block_desc->used_free_blocks.next);
    }

    HIF_EXIT_REENTRANCE_LOCK(hif_mem_malloc_data_ptr->enter_exit_id);

    sumSq = sum*sum;

    if (sumSq == 0)
    {
        *fragmentation = 0;
        rv = EINVAL;
    }
    else
    {
        //lint --e(795) Conceivable divison by 0. Divisor check for 0, see if clause above.
        *fragmentation = 1.0 - (sum_of_squares / sumSq);
    }

    return rv;
}

#endif // ( HIF_CFG_USE_HIF == 1 )

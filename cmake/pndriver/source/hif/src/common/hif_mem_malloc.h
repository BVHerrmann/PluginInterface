#ifndef HIF_MEM_MALLOC_H_
#define HIF_MEM_MALLOC_H_

#include <stdint.h>
#include <stddef.h>

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_mem_malloc.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Declarations for heap management                                         */
/*                                                                           */
/*****************************************************************************/

#if ( HIF_CFG_USE_HIF == 1 )

/**
 * Structure used to store information about the current status of the HIF heap.
 *
 * @see #hif_mem_malloc_get_heap_info
 */
typedef struct hif_mem_malloc_heap_info
{
    LSA_UINT32 size;          ///< configured size of the heap in bytes
    LSA_UINT32 used;          ///< currently used heap in bytes
    LSA_UINT32 max_used;      ///< "high water mark"; maximum heap usage so far in bytes
    LSA_UINT32 blocks_used;   ///< current number of used blocks
    LSA_UINT32 blocks_free;   ///< current number of free blocks
    LSA_UINT32 malloc_calls;  ///< number of successful calls to malloc/calloc
    LSA_UINT32 free_calls;    ///< number of successful calls to free
    LSA_UINT32 realloc_calls; ///< number of successful calls to realloc
} HIF_MEM_MALLOC_HEAP_INFO_TYPE, *HIF_MEM_MALLOC_HEAP_INFO_PTR_TYPE;

/**
 * Stores in the location of \c heap_info the information about the
 * current state of the HIF heap.
 * Information includes
 *  - the configured heap size in bytes
 *  - the currently used heap in bytes
 *  - the maximum heap usage so far in bytes
 *  - current number of used blocks
 *  - current number of free blocks
 *  - number of calls to malloc/calloc
 *  - number of calls to free
 *  - number of calls to realloc
 *
 * @see hif_mem_malloc_get_heap_fragmentation()
 *
 * @param heap_info pointer to #HIF_MEM_MALLOC_HEAP_INFO_TYPE where the current heap status is stored to
 *
 * @return 0 upon successful completion
 */
extern LSA_INT hif_mem_malloc_get_heap_info(HIF_MEM_MALLOC_HEAP_INFO_PTR_TYPE heap_info, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Calculates and returns in the location of \c fragmentation the heap fragmentation as a <b>floating point</b> value
 * in the range of 0<=fragmentation<1.
 * Fragmentation value means the following:
 *  - 0: No fragmentation, there is only one free block available that covers the whole free heap (best case)
 *  - close to 1: There are a lot of very small free blocks in the system (worst case)
 *
 * The heap fragmentation is calculated as follows:
 * Given: nf=number of free blocks on the heap
 *        sf(i)=size of the free block number i (i=[1..nf])
 *
 *  fragmentation = 1 - (sum[i=1;nf](sf(i) ^ 2) / sum[i=1;nf](sf(i)) ^ 2)
 *
 * In words: 1 minus the sum of the squares of all free blocks sizes divided by the square
 *           of the sum of all free blocks sizes.
 *
 * \attention The heap calculation makes use of floating point variables! The thread that calls this function may be marked as
 *    'floating point user' which may increase thread switching times from/to the thread, depending on the floating point configuration
 *    of your product
 *
 * @see hif_mem_malloc_get_heap_info
 *
 * @param fragmentation Pointer to a location where the fragmentation should be stored
 *
 * @return 0 upon successful completion otherwise
 *  - #EINVAL If there is no free memory available and the calculation would result in a devision by zero
 */
extern LSA_INT hif_mem_malloc_get_heap_fragmentation(double *fragmentation, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Extend the HIF heap by a memory block that starts at \c address
 * and has a size of \c size bytes.
 *
 * @param address Start address of the memory block to extend the heap
 * @param size Size of the memory block to extend the heap in bytes.
 *
 * @return 0 upon successful completion, otherwise
 *    - #EINVAL if lenght of the given memory block is to short
 */
extern LSA_INT hif_mem_malloc_heap_extend(LSA_VOID_PTR_TYPE address, LSA_UINT32 length, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Reduce the HIF heap by the one free block of memory that starts at \c address.
 *
 * @param address Start address of the memory block that should be removed from the heap
 * @param size Size of the memory block that was used for the call to hif_mem_malloc_heap_extend().
 *
 * @return 0 upon successful completion, otherwise
 *    - #EINVAL if the given address does not point to an HIF_MEM_MALLOC_BLOCK_DESC_TYPE
 *    in the state HIF_MALLOC_FREE or if the size of the block does not equal
 *    the size passed as argument
 */
extern LSA_INT hif_mem_malloc_heap_reduce(LSA_VOID_PTR_TYPE address, LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);


/**
 * Allocates a new memory block of the given size.
 * The list of allocated and free memory blocks (#hif_mem_malloc_data.allocated_blocks) is searched for a free memory block of the given size (first fit).
 * If found and if resources for an additional block descriptor (#HIF_MEM_MALLOC_BLOCK_DESC_TYPE) are available, the block found is split up into
 * two blocks.
 *  # The first block will get the requested size and is marked is used
 *  # The second block gets the remaining memory is marked as free.
 *
 * If no resources for an additional block descriptor are available, the allocated blocks are searched for a block that fits best for the
 * requested size (best fit). This block is marked as used and returned. Note that the block will most of the time be greater than the
 * requested size.
 *
 * @see #hif_mem_malloc_data.allocated_blocks
 * @see #hif_mem_malloc_data.free_blocks
 *
 * @param size The size of the memory block to be allocated.
 * @return Upon successful completion a pointer to the allocated memory is returned.
 *    If \c size is 0, \c LSA_NULL is returned. If no memory is available, \c LSA_NULL is returned
 *    and #errno is set to #ENOMEM
 */
extern LSA_VOID_PTR_TYPE hif_mem_malloc(LSA_UINT32 size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Deallocates the memory pointed to by \c ptr.
 * \c ptr has to be a pointer to a memory allocated by hif_mem_malloc() or  hif_mem_calloc().
 * If ptr is not a pointer to a previously allocated memory block, no action is taken.
 *
 * @param ptr Pointer to a memory block previously allocated by hif_mem_malloc() or hif_mem_calloc()
 */
extern LSA_VOID hif_mem_free(LSA_VOID_PTR_TYPE ptr, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);



/**
 * Tests whether the block with the given address and size is valid. It is tested whether
 * the given block intersects any other memory block previously returned by malloc.
 * If an invalid block was found, the system is taken into error state by calling #hif_fatal
 *
 * @return 0 if the block is valid. Otherwise the error state is entered
 */
extern LSA_INT hif_mem_malloc_verify(LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Tests whether the block with the given address and size is valid. It is tested whether
 * the given block intersects any other memory block previously returned by malloc.
 * If an invalid block was found, the system is taken into error state by calling #hif_fatal
 * This function is used if the memory was alloced using hif_mem_malloc_from_pool
 *
 * @param *pool_nr
 * @return 0 if the block is valid. Otherwise the error state is entered
 */
extern LSA_INT hif_mem_malloc_verify_by_pool(LSA_VOID_PTR_TYPE ptr, LSA_INT pool_nr);

/**
 * Init and Undo Init heap for hif_mem_malloc
 */
extern LSA_VOID_PTR_TYPE hif_mem_malloc_init(LSA_UINT8* heap_begin, LSA_UINT32 heap_size, LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);
extern LSA_VOID hif_mem_malloc_undo_init(LSA_VOID_PTR_TYPE hif_mem_malloc_helper_ptr);

/**
 * Create and Delete hif heap pool
 */
extern LSA_INT hif_mem_malloc_create_pool(LSA_UINT8* begin, LSA_UINT32 size);
extern LSA_VOID hif_mem_malloc_delete_pool(LSA_INT pool_nr);

/**
 * Allocates a new memory block of the given size from hif heap pool.
 */
extern LSA_VOID_PTR_TYPE hif_mem_malloc_from_pool(LSA_UINT32 size, LSA_INT pool_nr);
/**
 * Deallocates the memory pointed to by \c ptr from hif heap pool.
 */
extern LSA_VOID hif_mem_free_from_pool(LSA_VOID_PTR_TYPE ptr, LSA_INT pool_nr);

//#define HIF_MEM_MALLOC_VERIFY
//#define HIF_MEM_MALLOC_CLEAR
//#define HIF_MEM_MALLOC_CLEAR_PATTERN    0x0

/*----------------------------------------------------------------------------*/
#endif // ( HIF_CFG_USE_HIF == 1 )

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*        */
/*****************************************************************************/
#endif  /* of HIF_MEM_MALLOC_H */

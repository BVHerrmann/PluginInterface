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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: clsDmm.h                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsDmm"
//
//****************************************************************************/

#ifndef __clsDmm_h__
#define __clsDmm_h__

	//########################################################################
	//  Defines
	//########################################################################

	//------------------------------------------------------------------------
	//	configuration
	//------------------------------------------------------------------------

	#define	DMM_CLASS_UI
	#define	DMM_DEBUG
//	#define DMM_FILL_FREE_MEM

	//------------------------------------------------------------------------
	#define DMM_IFA_DATA_ATTR
	#define DMM_IFA_CODE_ATTR
	#define DMM_MEM_DATA_ATTR

	#define DMM_DISABLE_FURTHER_REQUESTS__
	#define DMM_ENABLE_FURTHER_REQUESTS__
	
	//------------------------------------------------------------------------
	#ifdef DMM_FILL_FREE_MEM
		#define DMM_FILL_INIT_MEM_VALUE		(LSA_UINT8) 0x88
		#define DMM_FILL_FREE_MEM_VALUE		(LSA_UINT8) 0xAA
	#endif
	
	//------------------------------------------------------------------------
	enum eDMM_ID
	{
		eDMM_QUEUE_ID_SHARED_HOST_SDRAM = 0,									// SharedHostSdram
		eDMM_QUEUE_ID_SDRAM				= 1,									// external AsicSdram
		eDMM_BLOCK_COUNT				= 2										// must be the last element -> no extra define necessary for DMM_BLOCK_COUNT
	};

	//------------------------------------------------------------------------
	//	internal
	//------------------------------------------------------------------------

    #ifndef NULL
	    #define NULL					0
    #endif
	#define DMM_SIZE_HEADER_BLOCK	(LSA_UINT16) (sizeof(uDMM_HEADER_BLOCK))
	#define DMM_SIZE_FREE_BLOCK		(LSA_UINT16) (sizeof(uDMM_FREE_BLOCK))
	
	//------------------------------------------------------------------------
	#if	(   (defined (__cplusplus))     \
			&& (defined (DMM_CLASS_UI)))
		#define dmm_fct_name__(name)	\
			clsDmm::name
	#else
		#define dmm_fct_name__(name)	\
			name
	#endif
	
	//------------------------------------------------------------------------
	#ifdef DMM_DEBUG
		#define DMM_DEBUG_SIZE_ARRAY 100
	#endif
	
	//------------------------------------------------------------------------
	typedef enum _eDMM_BOOL
	{
		eDMM_FALSE	= 0,
		eDMM_TRUE	= 1
	}
	eDMM_BOOL;
	
	//------------------------------------------------------------------------
	typedef enum _eDMM_DEBUG
	{
		DMM_DEBUG_INVALID	= 0,
		DMM_DEBUG_ALLOC		= 1,
		DMM_DEBUG_FREE		= 2
	}
	eDMM_DEBUG;
	
	//------------------------------------------------------------------------
	typedef enum _eDMM_ERROR
	{
		eDMM_ERROR_INVALID							= 0,
		eDMM_ERROR_QUEUE_NOT_INITIALIZED			= 1,
		eDMM_ERROR_QUEUE_ALREADY_INITIALIZED		= 2,
		eDMM_ERROR_QUEUE_BUFFER_TOO_SMALL			= 3,
		eDMM_ERROR_NOT_ENOUGH_MEMORY				= 4,
		eDMM_ERROR_NO_BLOCK_FOUND					= 5,
		eDMM_ERROR_INVALID_QUEUE_ID					= 6,
		eDMM_ERROR_INVALID_USER_LENGTH				= 7,
		eDMM_ERROR_INCONSISTENT_FREE_BLOCK_QUEUE	= 8,
		eDMM_ERROR_NULL_PTR							= 9,
		eDMM_ERROR_QUEUE_NOT_FOUND					= 10,
		eDMM_ERROR_BLOCK_NOT_ALIGNED				= 11,
		eDMM_ERROR_BLOCK_NOT_ALLOCATED				= 12,
		eDMM_ERROR_BLOCK_TOO_SMALL					= 13
	}
	eDMM_ERROR;

	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uDMM_INIT_BLOCK_QUEUE
	{
		LSA_UINT32	lQueueId;
		LSA_UINT32	lAdrStart;
		LSA_UINT32	lLen;
		LSA_UINT32	lAlignment;													// 4, 8, 16, 32
		LSA_UINT32	lLenPrebuffer;												// reserved area before UserPtr
	}
	uDMM_INIT_BLOCK_QUEUE;

	//------------------------------------------------------------------------
	typedef struct _uDMM_HEADER_BLOCK											// = header of an allocated block
	{		
		LSA_UINT32	lLenThisBlock;												// 4 Byte (must be able to store nearly the complete QueueBufferSize if less memory is allocated)
		LSA_UINT32	lLenPrevBlock;												// 4 Byte (must be able to store nearly the complete QueueBufferSize if less memory is allocated)
		LSA_UINT8	eAllocated;													// 1 Byte, eDMM_BOOL
		LSA_UINT8	eLastBlockPhy;												// 1 Byte, eDMM_BOOL (physical FirstBlock is signed by lLenPrevBlock=0)
	}
	uDMM_HEADER_BLOCK;

	//------------------------------------------------------------------------
	typedef struct _uDMM_FREE_BLOCK												// a FreeBlock has the same header like an allocated block + ChainPtr
	{
		uDMM_HEADER_BLOCK							uHeader;
		struct _uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pNext;
		struct _uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pPrev;
	}
	uDMM_FREE_BLOCK;

	//------------------------------------------------------------------------
	typedef struct _uDMM_QUEUE_BUFFER
	{
		LSA_UINT32	lAdrStart;
		LSA_UINT32	lAdrEnd;
		LSA_UINT32	lLen;
	}
	uDMM_QUEUE_BUFFER;

	//------------------------------------------------------------------------
	typedef struct _uDMM_QUEUE
	{
		uDMM_FREE_BLOCK		uRoot;												// at begin of structure -> easier for debugging
		uDMM_QUEUE_BUFFER	uBuffer;
		LSA_UINT32      	lAlignment;
		LSA_UINT32			lLenHeaderAligned;
		LSA_UINT32			lLenPrebufferAligned;
		LSA_UINT32			lLenFreeBlockAligned;
		LSA_UINT32       	lSizeFree;
		LSA_UINT32       	lSizeFreeMin;										// for debug purpose
		LSA_UINT32       	lCtrAllocated;										// for debug purpose

	#ifdef DMM_DEBUG
		uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR* pAllocated[DMM_DEBUG_SIZE_ARRAY];
	#endif
	}
	uDMM_QUEUE;

	//########################################################################
	//  Class
	//########################################################################

	class clsDmm
	{
	public:

	//========================================================================
	//  PUBLIC-attributes
	//========================================================================

	//========================================================================
	//  PUBLIC-inline-methods
	//========================================================================

	//========================================================================
	//  PUBLIC-methods
	//========================================================================

	clsDmm(	void);
	~clsDmm(void);
	
	//------------------------------------------------------------------------
	LSA_VOID					DMM_IFA_CODE_ATTR	dmm_startup(			LSA_VOID);
	LSA_BOOL					DMM_IFA_CODE_ATTR	dmm_init_queue(			uDMM_INIT_BLOCK_QUEUE		uInitBlockIn,
																			eDMM_ERROR*					pErrorOut);
	LSA_VOID DMM_MEM_DATA_ATTR*	DMM_IFA_CODE_ATTR   dmm_alloc_mem(			LSA_UINT32					lLenUserIn,
																			LSA_UINT32					lQueueIdIn,
																			eDMM_ERROR*					pErrorOut);
	LSA_VOID DMM_MEM_DATA_ATTR*	DMM_IFA_CODE_ATTR	dmm_realloc_mem(		LSA_VOID DMM_MEM_DATA_ATTR* pUserIn,
																			LSA_UINT32					lLenUserIn,
																			eDMM_ERROR*					pErrorOut);
	LSA_BOOL					DMM_IFA_CODE_ATTR	dmm_free_mem(			LSA_VOID DMM_MEM_DATA_ATTR*	pUserIn,
																			eDMM_ERROR*					pErrorOut);
	LSA_VOID					DMM_IFA_CODE_ATTR	dmm_get_info_queue(		LSA_UINT32					lQueueIdIn,
																			LSA_UINT32*					lSizeFreeOut,
																			LSA_UINT32*					lSizeFreeMinOut,
																			LSA_UINT32*					lCtrAllocatedOut);

	private:

	//========================================================================
	//  PRIVATE-attributes
	//========================================================================

	uDMM_QUEUE DMM_IFA_DATA_ATTR m_lArrayQueue[eDMM_BLOCK_COUNT];

	//========================================================================
	//  PRIVATE-inline-methods
	//========================================================================

	LSA_VOID DMM_IFA_CODE_ATTR dmm_insert_block_at_start_of_free_block_queue(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pFreeBlockIn,
																				uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pQueueIn)
	{
		// insert FreeBlock at QueueStart

		// block.prev = &list
		pFreeBlockIn->pPrev = pQueueIn;

		// block.next = list.next
		pFreeBlockIn->pNext = pQueueIn->pNext;

		// list.next.prev = &block
		pQueueIn->pNext->pPrev = pFreeBlockIn;

		// list.next = &block
		pQueueIn->pNext = pFreeBlockIn;
	}

	//------------------------------------------------------------------------
	LSA_BOOL DMM_IFA_CODE_ATTR dmm_remove_block_from_free_block_queue(uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR* pFreeBlockIn)
	{
	LSA_BOOL bResult = LSA_FALSE;

		if	(	(pFreeBlockIn->pNext		!= NULL)
			&&	(pFreeBlockIn->pPrev		!= NULL)
			&&	(pFreeBlockIn->pPrev->pNext	== pFreeBlockIn))
			// valid chain
		{
			// success
			bResult = LSA_TRUE;

			// block.prev.next = block.next
			pFreeBlockIn->pPrev->pNext = pFreeBlockIn->pNext;

			// block.next.prev = block.prev
			pFreeBlockIn->pNext->pPrev = pFreeBlockIn->pPrev;

			// update allocated FreeBlock n
			pFreeBlockIn->pNext	= NULL;
			pFreeBlockIn->pPrev = NULL;
		}

		return(bResult);
	}

	//------------------------------------------------------------------------
	LSA_BOOL DMM_IFA_CODE_ATTR dmm_value_aligned(	LSA_UINT64	lValueIn,
													LSA_UINT64	lAlignmentIn)
	{
	LSA_BOOL bResult = LSA_FALSE;

		if	((lValueIn & (lAlignmentIn-1)) == 0)
			// value aligned (look if relevant bits are already 0)
		{
			// example:
			//	- Value=8, Alignment=8
			//	- 1000 & 0111 = 0 -> aligned, nothing to do

			// success
			bResult = LSA_TRUE;
		}

		return(bResult);
	}

	//------------------------------------------------------------------------
	LSA_UINT32 DMM_IFA_CODE_ATTR dmm_get_value_aligned(	LSA_UINT32	lValueIn,
														LSA_UINT32	lAlignmentIn,
														LSA_BOOL	bIncreaseIn)
	{
	LSA_UINT32 lResult = 0;

		if	(dmm_value_aligned(lValueIn, lAlignmentIn))
			// value already aligned
		{
			lResult = lValueIn;
		}
		else
		{
			// value not aligned

			if	(bIncreaseIn)
				// value should be increased to a multiple of alignment
			{
				// example:
				//	- Value=6, Alignment=8
				//	- 0110 & 0111 != 0 -> lResult = (6+8) & ~7 = 14 & ~7 = 1110 & 1000 = 1000

				// add alignment, clear relevant bits
				lResult = (lValueIn + lAlignmentIn) & (~(lAlignmentIn-1));
			}
			else
			{
				// value should be decreased to a multiple of alignment

				// example:
				//	- Value=10, Alignment=8
				//	- 1010 & 0111 != 0 -> lResult = 10 & ~7 = 10 & ~7 = 1010 & 1000 = 1000

				// clear relevant bits
				lResult = lValueIn & (~(lAlignmentIn-1));
			}
		}

		return(lResult);
	}

	//------------------------------------------------------------------------
	LSA_VOID DMM_IFA_CODE_ATTR dmm_copy_bytes_ss(	LSA_UINT8 DMM_IFA_DATA_ATTR*	pDestIn,
													LSA_UINT8 DMM_IFA_DATA_ATTR*	pSrcIn, 
													LSA_UINT32						lLenDataIn)
	{
		while	(lLenDataIn > 0)
				// data available
		{
			*pDestIn = *pSrcIn;
			pDestIn++;
			pSrcIn++;

			lLenDataIn--;
		}
	}

	//------------------------------------------------------------------------
	LSA_VOID DMM_IFA_CODE_ATTR dmm_merge_blocks(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pBlockFirstIn,
													uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pBlockLastIn,
													uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pSuccessorIn,
													LSA_UINT32							lLenAddIn)
	{
		// first block will be the start of the new merged block

		// update BufferLength of merged block
		pBlockFirstIn->uHeader.lLenThisBlock += lLenAddIn;

		// update state 'PhysicalLastBlock' of merged block
		//	- first block inherits state of last block
		pBlockFirstIn->uHeader.eLastBlockPhy = pBlockLastIn->uHeader.eLastBlockPhy;

		// update successor of merged block
		{
			if	(pSuccessorIn != NULL)
				// successor exist
			{
				// update information about predecessor
				pSuccessorIn->uHeader.lLenPrevBlock = pBlockFirstIn->uHeader.lLenThisBlock;
			}
		}
	}

	//------------------------------------------------------------------------
	LSA_VOID DMM_IFA_CODE_ATTR dmm_fill_mem(LSA_UINT8 DMM_MEM_DATA_ATTR*	pStartIn,
											LSA_UINT32						lLenIn,
											LSA_UINT8						lValueIn)
	{
		for	(LSA_UINT32 i = 0; i < lLenIn; i++)
		{
			*pStartIn = lValueIn;

			pStartIn++;
		}
	}

	//------------------------------------------------------------------------
	LSA_BOOL DMM_IFA_CODE_ATTR dmm_get_info_block(	LSA_VOID DMM_MEM_DATA_ATTR*			pUserIn,
													uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR**	pPtrMemOut,
													LSA_UINT32*							pQueueIdOut,
													eDMM_ERROR*							pErrorOut)
	{
	LSA_BOOL bResult = LSA_FALSE;

		// preset OutputParameter
		*pPtrMemOut		= NULL;
		*pQueueIdOut	= 0;
		*pErrorOut		= eDMM_ERROR_INVALID;

		if	(pUserIn == NULL)
			// invalid ptr
		{
			// error
			*pErrorOut = eDMM_ERROR_NULL_PTR;
		}
		else
		{
		LSA_BOOL	bResultQueue	= LSA_FALSE;
		LSA_UINT32	lQueueId		= 0;

			// search for queue from which block was allocated
			{
				for	(lQueueId = 0; lQueueId < eDMM_BLOCK_COUNT; lQueueId++)
					// all queues
				{
					if	(	((LSA_UINT64) pUserIn	>= m_lArrayQueue[lQueueId].uBuffer.lAdrStart)
						&&	((LSA_UINT64) pUserIn	<= m_lArrayQueue[lQueueId].uBuffer.lAdrEnd))
						// block in AddressRange of this QueueBuffer
					{
					uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR* pMem = NULL;

						// get ptr to header (= original BlockStartPtr)
						pMem = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
							(((LSA_UINT8 DMM_MEM_DATA_ATTR*) pUserIn)
								- m_lArrayQueue[lQueueId].lLenPrebufferAligned
								- m_lArrayQueue[lQueueId].lLenHeaderAligned);

						if (!dmm_value_aligned((LSA_UINT64)pMem, (LSA_UINT64)m_lArrayQueue[lQueueId].lAlignment))

							// block not aligned
						{
							// error
							*pErrorOut = eDMM_ERROR_BLOCK_NOT_ALIGNED;
						}
						else if (pMem->uHeader.eAllocated != eDMM_TRUE)
							// block not allocated
						{
							// error
							*pErrorOut = eDMM_ERROR_BLOCK_NOT_ALLOCATED;
						}
						else if (pMem->uHeader.lLenThisBlock < m_lArrayQueue[lQueueId].lLenFreeBlockAligned)
							// BlockLength < MinSize of FreeBlock
						{
							// error
							*pErrorOut = eDMM_ERROR_BLOCK_TOO_SMALL;
						}
						else
						{
							// success
							bResult = LSA_TRUE;

							// return
							*pPtrMemOut = pMem;
							*pQueueIdOut = lQueueId;
						}

						// success
						bResultQueue = LSA_TRUE;
						
						// leave loop
						break;
					}
				}
			}

			if	(bResultQueue == LSA_FALSE)
				// error
			{
				// error
				*pErrorOut = eDMM_ERROR_QUEUE_NOT_FOUND;
			}
		}

		return(bResult);
	}

	//========================================================================
	//  PRIVATE-methods
	//========================================================================

	LSA_BOOL DMM_IFA_CODE_ATTR	dmm_free_mem_queue(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMemIn,
													LSA_UINT32							lQueueIdIn,
													eDMM_ERROR*							pErrorOut);
															
	//------------------------------------------------------------------------
	#ifdef DMM_DEBUG
		LSA_VOID	DMM_IFA_CODE_ATTR   dmm_update_debug_info(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMemIn,
																LSA_UINT32							lSelectionIn, 
																LSA_UINT32							lQueueIdIn);
	#endif
	};

#endif

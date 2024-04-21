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
/*  F i l e               &F: clsDmm.cpp                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsDmm" (based on DMM of Peter Heinze)
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//	Variables
//########################################################################
//------------------------------------------------------------------------

//########################################################################
//	PUBLIC
//########################################################################

#ifdef DMM_CLASS_UI

//************************************************************************
//  D e s c r i p t i o n :
//
//	constructor
//************************************************************************

clsDmm::clsDmm(void)
{
	// preset
	memset(	this,
			0,
			sizeof(clsDmm));
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

clsDmm::~clsDmm(void)
{
}

#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	startup DMM
//************************************************************************

LSA_VOID DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_startup)(LSA_VOID)
{
	for	(LSA_UINT32 i = 0; i < eDMM_BLOCK_COUNT; i++)
		// all queues
	{
		m_lArrayQueue[i].uBuffer.lAdrStart		= 0;
		m_lArrayQueue[i].uBuffer.lAdrEnd		= 0;
		m_lArrayQueue[i].uBuffer.lLen			= 0;

		m_lArrayQueue[i].lAlignment				= 0;
		m_lArrayQueue[i].lLenHeaderAligned		= 0;
		m_lArrayQueue[i].lLenPrebufferAligned	= 0;
		m_lArrayQueue[i].lLenFreeBlockAligned	= 0;

		m_lArrayQueue[i].lSizeFree				= 0;
		m_lArrayQueue[i].lSizeFreeMin			= 0;
		m_lArrayQueue[i].lCtrAllocated			= 0;

		m_lArrayQueue[i].uRoot.uHeader.lLenThisBlock	= 0;
		m_lArrayQueue[i].uRoot.uHeader.lLenPrevBlock	= 0;
		m_lArrayQueue[i].uRoot.uHeader.eAllocated		= eDMM_FALSE;
		m_lArrayQueue[i].uRoot.uHeader.eLastBlockPhy	= eDMM_FALSE;
		m_lArrayQueue[i].uRoot.pNext					= NULL;
		m_lArrayQueue[i].uRoot.pPrev					= NULL;

		#ifdef DMM_DEBUG
		{
			for	(LSA_UINT32 k = 0;	k <	DMM_DEBUG_SIZE_ARRAY; k++)
			{
				m_lArrayQueue[i].pAllocated[k] = NULL;
			}
		}
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	initialize Queue
//************************************************************************

LSA_BOOL DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_init_queue)(	uDMM_INIT_BLOCK_QUEUE	uInitBlockIn,
															eDMM_ERROR*				pErrorOut)
{
LSA_BOOL	bResult			= LSA_FALSE;
LSA_UINT32	lAdrStart		= 0;
LSA_UINT32	lLenBuffer		= 0;
LSA_UINT32	lQueueId		= 0;
LSA_UINT32	lAlignment		= 0;
LSA_UINT32	lLenPrebuffer	= 0;

	// preset OutputParameter
	*pErrorOut = eDMM_ERROR_INVALID;

	// initialize variables
	lQueueId		= uInitBlockIn.lQueueId;
	lAdrStart		= uInitBlockIn.lAdrStart;
	lLenBuffer		= uInitBlockIn.lLen;
	lAlignment		= uInitBlockIn.lAlignment;
	lLenPrebuffer	= uInitBlockIn.lLenPrebuffer;

	if	(m_lArrayQueue[lQueueId].uBuffer.lLen != 0)
		// Queue already initialized
	{
		// error
		*pErrorOut = eDMM_ERROR_QUEUE_ALREADY_INITIALIZED;
	}
	else
	{
	LSA_UINT32 lAdrStartOld = 0;

		// store current StartAdr
		lAdrStartOld = lAdrStart;

		// align StartAdr
		//	- if not aligned then value should be increased
		lAdrStart = dmm_get_value_aligned(lAdrStart, lAlignment, LSA_TRUE);

		// update QueueBufferLength
		lLenBuffer -= (lAdrStart - lAdrStartOld);

		// align QueueBufferLength
		//	- if not aligned then value should be decreased
		lLenBuffer = dmm_get_value_aligned(lLenBuffer, lAlignment, LSA_FALSE);

		// there must be place for at least 1 FreeBlock

		if	(lLenBuffer < m_lArrayQueue[lQueueId].lLenFreeBlockAligned)
			// QueueBufferLength < MinSize of FreeBlock
		{
			// error
			*pErrorOut = eDMM_ERROR_QUEUE_BUFFER_TOO_SMALL;
		}
		else
		{
			// disable DmmRequests
			DMM_DISABLE_FURTHER_REQUESTS__;

			// initialize QueueManagement
			{
				m_lArrayQueue[lQueueId].uBuffer.lAdrStart	= lAdrStart;
				m_lArrayQueue[lQueueId].uBuffer.lAdrEnd		= lAdrStart	+ lLenBuffer - 1;
				m_lArrayQueue[lQueueId].uBuffer.lLen		= lLenBuffer;
				m_lArrayQueue[lQueueId].lAlignment			= lAlignment;

				// Note:
				//	- if FreeBlockAdr is aligned then HeaderLength and PreBuffer must be aligned too. Only then pUser is aligned!

				// align HeaderLength and PreBuffer
				//	- if not aligned then value should be increased
				m_lArrayQueue[lQueueId].lLenHeaderAligned		= dmm_get_value_aligned(DMM_SIZE_HEADER_BLOCK,	lAlignment, LSA_TRUE);
				m_lArrayQueue[lQueueId].lLenPrebufferAligned	= dmm_get_value_aligned(lLenPrebuffer,			lAlignment, LSA_TRUE);
				m_lArrayQueue[lQueueId].lLenFreeBlockAligned	= dmm_get_value_aligned(DMM_SIZE_FREE_BLOCK,	lAlignment, LSA_TRUE);

				m_lArrayQueue[lQueueId].lSizeFree		= lLenBuffer;
				m_lArrayQueue[lQueueId].lSizeFreeMin	= lLenBuffer;
				m_lArrayQueue[lQueueId].lCtrAllocated	= 0;

				// initialize root of FreeBlockQueue
				m_lArrayQueue[lQueueId].uRoot.uHeader.lLenThisBlock	= 0;
				m_lArrayQueue[lQueueId].uRoot.uHeader.lLenPrevBlock	= 0;
				m_lArrayQueue[lQueueId].uRoot.uHeader.eAllocated	= eDMM_FALSE;
				m_lArrayQueue[lQueueId].uRoot.uHeader.eLastBlockPhy	= eDMM_FALSE;
				m_lArrayQueue[lQueueId].uRoot.pNext					= (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*) (UINT64) lAdrStart;
				m_lArrayQueue[lQueueId].uRoot.pPrev					= (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*) (UINT64) lAdrStart;
			}

			#ifdef DMM_FILL_FREE_MEM
			{
				// fill complete QueueBuffer
				//	- must be before initialization of first FreeBlock (= QueueBuffer)!
				dmm_fill_mem(	(LSA_UINT8 DMM_MEM_DATA_ATTR*) lAdrStart,
								lLenBuffer,
								DMM_FILL_INIT_MEM_VALUE);
			}
			#endif

			// initialize first FreeBlock (= QueueBuffer got by user)
			m_lArrayQueue[lQueueId].uRoot.pNext->uHeader.lLenThisBlock	= lLenBuffer;
			m_lArrayQueue[lQueueId].uRoot.pNext->uHeader.lLenPrevBlock	= 0;									// sign for PhysicalFirstBlock of QueueBuffer
			m_lArrayQueue[lQueueId].uRoot.pNext->uHeader.eAllocated		= eDMM_FALSE;
			m_lArrayQueue[lQueueId].uRoot.pNext->uHeader.eLastBlockPhy	= eDMM_TRUE;							// sign for PhysicalLastBlock of QueueBuffer
			m_lArrayQueue[lQueueId].uRoot.pNext->pNext					= &m_lArrayQueue[lQueueId].uRoot;		// root
			m_lArrayQueue[lQueueId].uRoot.pNext->pPrev					= &m_lArrayQueue[lQueueId].uRoot;		// root

			// success
			bResult = LSA_TRUE;

			// enable DmmRequests
			DMM_ENABLE_FURTHER_REQUESTS__;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	allocate memory
//************************************************************************

LSA_VOID DMM_MEM_DATA_ATTR* DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_alloc_mem)(LSA_UINT32	lLenUserIn,
																			LSA_UINT32	lQueueIdIn,
																			eDMM_ERROR*	pErrorOut)
{
LSA_VOID DMM_MEM_DATA_ATTR* pResult = NULL;

	// preset OutputParameter
	*pErrorOut = eDMM_ERROR_INVALID;

	if	(lLenUserIn == 0)
		// invalid UserLength
	{
		// error
		*pErrorOut = eDMM_ERROR_INVALID_USER_LENGTH;
	}
	else if	(lQueueIdIn >= eDMM_BLOCK_COUNT)
			// invalid QueueId
	{
		// error
		*pErrorOut = eDMM_ERROR_INVALID_QUEUE_ID;
	}
	else if	(m_lArrayQueue[lQueueIdIn].uBuffer.lLen == 0)
			// Queue not initialized
	{
		// error
		*pErrorOut = eDMM_ERROR_QUEUE_NOT_INITIALIZED;
	}
	else
	{
	LSA_UINT32 lLenAligned = 0;

		// disable DmmRequests
		DMM_DISABLE_FURTHER_REQUESTS__;

		// align UserLength
		//	- if not aligned then value should be increased
		lLenAligned = dmm_get_value_aligned(lLenUserIn, m_lArrayQueue[lQueueIdIn].lAlignment, LSA_TRUE);

		// compute complete necessary memory (aligned Header + aligned Prebuffer + aligned UserLength)
		lLenAligned += (m_lArrayQueue[lQueueIdIn].lLenHeaderAligned + m_lArrayQueue[lQueueIdIn].lLenPrebufferAligned);

		// ensure that block can be reinserted to FreeBlockQueue at dmm_free_mem()
		{
			if (lLenAligned < m_lArrayQueue[lQueueIdIn].lLenFreeBlockAligned)
				// length < MinSize of FreeBlock
			{
				// set length to MinSize of FreeBlock
				lLenAligned = m_lArrayQueue[lQueueIdIn].lLenFreeBlockAligned;
			}
		}

		if	(lLenAligned > m_lArrayQueue[lQueueIdIn].lSizeFree)
			// not enough memory
		{
			// error
			*pErrorOut = eDMM_ERROR_NOT_ENOUGH_MEMORY;
		}
		else
		{
		uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMem	= NULL;
		uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pRoot	= NULL;

			// get root of FreeBlockQueue
			pRoot = &m_lArrayQueue[lQueueIdIn].uRoot;

			// search for block with desired size at FreeBlockQueue
			{
				// start with first FreeBlock
				pMem = pRoot->pNext;

				while	(	(pMem != pRoot)
						&&	(pMem->uHeader.lLenThisBlock < lLenAligned))
						//		root not reached
						// AND	current FreeBlock too small
				{
					// try next block
					pMem = pMem->pNext;
				}
			}

			if	(pMem == pRoot)
				// no block found
			{
				// error
				*pErrorOut = eDMM_ERROR_NO_BLOCK_FOUND;
			}
			else
			{
				if	(!dmm_remove_block_from_free_block_queue(pMem))
					// error at removing block from FreeBlockQueue
				{
					// error
					*pErrorOut = eDMM_ERROR_INCONSISTENT_FREE_BLOCK_QUEUE;
				}
				else
				{
				LSA_UINT32 lSizeUnused = 0;

					// mark block as allocated
					pMem->uHeader.eAllocated = eDMM_TRUE;

					// compute rest size of allocated block which is not demanded by user
					lSizeUnused = pMem->uHeader.lLenThisBlock - lLenAligned;

					// reinsert unused memory to FreeBlockQueue if possible
					{
						if	(lSizeUnused > m_lArrayQueue[lQueueIdIn].lLenFreeBlockAligned)
							// UnusedSize > MinSize of FreeBlock
						{
						uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pNext		= NULL;
						uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pNextNext	= NULL;

							// compute StartAdr of new FreeBlock
							pNext = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
									((LSA_UINT8 DMM_MEM_DATA_ATTR*) pMem + lLenAligned);

							// initialize new FreeBlock
							{
								pNext->uHeader.lLenThisBlock	= lSizeUnused;
								pNext->uHeader.lLenPrevBlock	= lLenAligned;
								pNext->uHeader.eAllocated		= eDMM_FALSE;

								// update state 'PhysicalLastBlock' of new FreeBlock
								//	- if allocated block (= current FreeBlock) was PhysicalLastBlock then new FreeBlock will now be the PhysicalLastBlock
								//		-> new FreeBlock inherits state of allocated block
								pNext->uHeader.eLastBlockPhy = pMem->uHeader.eLastBlockPhy;
							}

							// update successor of new FreeBlock if exist
							{
								if	(pMem->uHeader.eLastBlockPhy != eDMM_TRUE)
									// allocated block was not PhysicalLastBlock
								{
									// compute successor of new FreeBlock
									//	- original length of allocated block (= current FreeBlock) was not changed up to now
									pNextNext = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
												((LSA_UINT8 DMM_MEM_DATA_ATTR*) pMem + pMem->uHeader.lLenThisBlock);

									// update successor of new FreeBlock
									pNextNext->uHeader.lLenPrevBlock = lSizeUnused;
								}
							}

							// update allocated block (= current FreeBlock)
							//	- if block was not split then these parameters are still up to date
							{
								pMem->uHeader.lLenThisBlock = lLenAligned;

								//  if allocated block (= current FreeBlock) was the PhysicalFirstBlock then it	will be	still the PhysicalFirstBlock
								//	-> don't change

								// update state 'PhysicalLastBlock' of allocated block
								//	- allocated block (= current FreeBlock) was split
								//		-> surely not PhysicalLastBlock
								pMem->uHeader.eLastBlockPhy = eDMM_FALSE;
							}

							// insert FreeBlock at start of FreeBufferQueue
							dmm_insert_block_at_start_of_free_block_queue(pNext, pRoot);
						}
					}

					// update FreeSize
					m_lArrayQueue[lQueueIdIn].lSizeFree -= lLenAligned;

					// update minimal FreeSize of Queue
					if (m_lArrayQueue[lQueueIdIn].lSizeFree < m_lArrayQueue[lQueueIdIn].lSizeFreeMin)
					{
						m_lArrayQueue[lQueueIdIn].lSizeFreeMin = m_lArrayQueue[lQueueIdIn].lSizeFree;
					}	 

					// update number of allocated blocks
					m_lArrayQueue[lQueueIdIn].lCtrAllocated++;

					// return UserPtr (= OffsetPtr -> hide BlockHeader and PreBuffer)
					pResult	= ((LSA_UINT8 DMM_MEM_DATA_ATTR*) pMem) 
							+ m_lArrayQueue[lQueueIdIn].lLenHeaderAligned
							+ m_lArrayQueue[lQueueIdIn].lLenPrebufferAligned;

					#ifdef DMM_DEBUG
					{
						// update trace of allocated blocks (original BlockStartPtr)
						dmm_update_debug_info(pMem, DMM_DEBUG_ALLOC, lQueueIdIn);
					}
					#endif
				}
			}
		}

		// enable DmmRequests
		DMM_ENABLE_FURTHER_REQUESTS__;
	}

	return(pResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	reallocate memory (get new block with new size but same content)
//************************************************************************

LSA_VOID DMM_MEM_DATA_ATTR* DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_realloc_mem)(	LSA_VOID DMM_MEM_DATA_ATTR* pUserIn,		// UserPtr = OffsetPtr
																				LSA_UINT32					lLenUserIn,
																				eDMM_ERROR*					pErrorOut)
{
LSA_VOID DMM_MEM_DATA_ATTR*			pResult		= NULL;
uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMem		= NULL;
LSA_UINT32							lQueueId	= 0;

	// preset OutputParameter
	*pErrorOut = eDMM_ERROR_INVALID;

	if	(dmm_get_info_block(pUserIn,
							&pMem,
							&lQueueId,
							pErrorOut))
		// getting BlockInfo ok
	{
	LSA_UINT32 lLenUserOld = 0;

		// get UserLength of old block
		lLenUserOld	= pMem->uHeader.lLenThisBlock
					- m_lArrayQueue[lQueueId].lLenPrebufferAligned
					- m_lArrayQueue[lQueueId].lLenHeaderAligned;

		// allocate new block with new desired UserLength
		pResult	= (LSA_VOID DMM_MEM_DATA_ATTR*) dmm_alloc_mem(	lLenUserIn,	
																lQueueId,
																pErrorOut);
		if	(pResult == NULL)
			// error
		{
			// return NullPtr and ErrorCode
		}
		else
		{
		LSA_UINT32 lLenCopy = 0;

			if	(lLenUserIn >= lLenUserOld)
				// bigger block was allocated
			{
				// copy current content
				lLenCopy = lLenUserOld;
			}
			else
			{
				// copy part of current content
				lLenCopy = lLenUserIn;
			}

			// disable DmmRequests
			DMM_DISABLE_FURTHER_REQUESTS__;

			// copy content of old block to new block (may be only a part)
			dmm_copy_bytes_ss(	(LSA_UINT8*) pResult,
								(LSA_UINT8*) pUserIn,
								lLenCopy);

			// enable DmmRequests
			DMM_ENABLE_FURTHER_REQUESTS__;

			if	(!dmm_free_mem_queue(	pMem,
										lQueueId,
										pErrorOut))
				// error at freeing old block (original BlockStartPtr)
			{
			eDMM_ERROR eError = eDMM_ERROR_INVALID;

				// free new block (UserPtr = OffsetPtr)
				//	- don't use pErrorOut, otherwise it will be overwritten!
				dmm_free_mem(	pResult,
								&eError);

				// return NullPtr
				pResult = NULL;
			}
		}
	}

	return(pResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	free memory
//************************************************************************

LSA_BOOL DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_free_mem)(	LSA_VOID DMM_MEM_DATA_ATTR*	pUserIn,		// UserPtr = OffsetPtr
															eDMM_ERROR*					pErrorOut)
{
LSA_BOOL							bResult		= LSA_FALSE;
uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMem		= NULL;
LSA_UINT32							lQueueId	= 0;

	// preset OutputParameter
	*pErrorOut = eDMM_ERROR_INVALID;

	if	(dmm_get_info_block(pUserIn,
							&pMem,
							&lQueueId,
							pErrorOut))
		// getting BlockInfo ok
	{
		// free block (original BlockStartPtr)
		bResult	= dmm_free_mem_queue(	pMem,
										lQueueId,
										pErrorOut);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	get QueueInfo
//************************************************************************

LSA_VOID DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_get_info_queue)(	LSA_UINT32	lQueueIdIn,
																LSA_UINT32*	lSizeFreeOut,
																LSA_UINT32*	lSizeFreeMinOut,
																LSA_UINT32*	lCtrAllocatedOut)
{
	// return
	*lSizeFreeOut		= m_lArrayQueue[lQueueIdIn].lSizeFree;
	*lSizeFreeMinOut	= m_lArrayQueue[lQueueIdIn].lSizeFreeMin;
	*lCtrAllocatedOut	= m_lArrayQueue[lQueueIdIn].lCtrAllocated;
}

//########################################################################
//	PRIVATE
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	free memory of a known queue
//************************************************************************

LSA_BOOL DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_free_mem_queue)(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMemIn,			// original BlockStartPtr
																LSA_UINT32							lQueueIdIn,
																eDMM_ERROR*							pErrorOut)
{
LSA_BOOL							bResult			= LSA_FALSE;
uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pPrev			= NULL;
uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pNext			= NULL;
uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pNextNext		= NULL;
LSA_BOOL							bNextBlockFree	= LSA_FALSE;

	// preset OutputParameter
	*pErrorOut = eDMM_ERROR_INVALID;

	// disable DmmRequests
	DMM_DISABLE_FURTHER_REQUESTS__;

	// get physical blocks n-1/n+1/n+2 if exist (ThisBlock = block n)
	{
		if	(pMemIn->uHeader.lLenPrevBlock != 0)
			// block n-1 exist
		{
			// get block n-1
			pPrev = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
					(((LSA_UINT8 DMM_MEM_DATA_ATTR*) pMemIn) - pMemIn->uHeader.lLenPrevBlock);
		}

		if	(pMemIn->uHeader.eLastBlockPhy != eDMM_TRUE)
			// block n+1 exist
		{
			// get block n+1
			pNext = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
					(((LSA_UINT8 DMM_MEM_DATA_ATTR*) pMemIn) + pMemIn->uHeader.lLenThisBlock);

			if	(pNext->uHeader.eLastBlockPhy != eDMM_TRUE)
				// block n+2 exist
			{
				// get block n+2
				pNextNext = (uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*)
							(((LSA_UINT8 DMM_MEM_DATA_ATTR*) pNext) + pNext->uHeader.lLenThisBlock);
			}
		}
	}

	if	(	(pNext						!= NULL)
		&&	(pNext->uHeader.eAllocated	!= eDMM_TRUE))
		// block n+1 free
	{
		// block n+1 will be merged with block n
		// block n+1 is at FreeBufferQueue

		bNextBlockFree = LSA_TRUE;

		if	(!dmm_remove_block_from_free_block_queue(pNext))
			// error at removing block from FreeBlockQueue
		{
			// error
			*pErrorOut = eDMM_ERROR_INCONSISTENT_FREE_BLOCK_QUEUE;
		}
	}

	if	(*pErrorOut == eDMM_ERROR_INVALID)
		// no error
	{
	uDMM_FREE_BLOCK	DMM_MEM_DATA_ATTR* pRoot = NULL;
    #ifdef DMM_FILL_FREE_MEM
	    LSA_UINT8       DMM_MEM_DATA_ATTR* pFill = NULL;
    #endif
	
		// success
		bResult = LSA_TRUE;

		// get root of FreeBlockQueue
		pRoot = &m_lArrayQueue[lQueueIdIn].uRoot;

		// update FreeSize
		//	- Note: must be before merging block n
		m_lArrayQueue[lQueueIdIn].lSizeFree += (pMemIn->uHeader.lLenThisBlock);

		// update number of allocated blocks
		m_lArrayQueue[lQueueIdIn].lCtrAllocated--;

		// try to merge block with FreeBlock n-1/n+1 (if possible)

		if	(	(pPrev						!= NULL)
			&&	(pPrev->uHeader.eAllocated	!= eDMM_TRUE))
			// block n-1 free
		{
			// block n-1 will be the start of the merged block
			//	- block n-1 already in FreeBufferQueue
			//	- block n-1 already marked as free
        #ifdef DMM_FILL_FREE_MEM
            pFill = (LSA_UINT8 DMM_MEM_DATA_ATTR*) pPrev;
        #endif

			if	(bNextBlockFree)
				// block n+1 free
			{
				// merge block n-1/n/n+1
				dmm_merge_blocks(	pPrev,																// first free block				= block n-1
									pNext,																// last free block				= block n+1
									pNextNext,															// successor of merged block	= block n+2
									(pMemIn->uHeader.lLenThisBlock + pNext->uHeader.lLenThisBlock));	// additional BufferLength added to first free block
			}
			else
			{
				// merge block n-1/n
				dmm_merge_blocks(	pPrev,								// first free block				= block n-1
									pMemIn,								// last free block				= block n
									pNext,								// successor of merged block	= block n+1
									pMemIn->uHeader.lLenThisBlock);		// additional BufferLength added to first free block
			}
		}
		else	// block n-1 not exist OR allocated
		{
			// block n will be the start of the merged block
			//	- block n must be inserted to FreeBufferQueue
			//	- block n must be marked as free
        #ifdef DMM_FILL_FREE_MEM
            pFill = (LSA_UINT8 DMM_MEM_DATA_ATTR*) pMemIn;
        #endif


			if	(bNextBlockFree)
				// block n+1 free
			{
				// merge block n/n+1
				dmm_merge_blocks(	pMemIn,								// first free block				= block n
									pNext,								// last free block				= block n+1
									pNextNext,							// successor of merged block	= block n+2
									pNext->uHeader.lLenThisBlock);		// additional BufferLength added to first free block
			}

			// mark block as free
			pMemIn->uHeader.eAllocated = eDMM_FALSE;

			// insert FreeBuffer at	start of FreeBlockQueue
			dmm_insert_block_at_start_of_free_block_queue(pMemIn, pRoot);
		}

		#ifdef DMM_DEBUG
		{
			// update trace of allocated blocks (original BlockStartPtr)
			dmm_update_debug_info(pMemIn, DMM_DEBUG_FREE, lQueueIdIn);
		}
		#endif

		#ifdef DMM_FILL_FREE_MEM
		{
			dmm_fill_mem(	(pFill + DMM_SIZE_FREE_BLOCK),
							((uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*) pFill)->uHeader.lLenThisBlock - DMM_SIZE_FREE_BLOCK,
							DMM_FILL_FREE_MEM_VALUE);
		}
		#endif
	}

	// enable DmmRequests
	DMM_ENABLE_FURTHER_REQUESTS__;

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	update DebugInfo
//************************************************************************

#ifdef DMM_DEBUG

LSA_VOID DMM_IFA_CODE_ATTR dmm_fct_name__(dmm_update_debug_info)(	uDMM_FREE_BLOCK DMM_MEM_DATA_ATTR*	pMemIn,
																	LSA_UINT32							lModeIn,
																	LSA_UINT32							lQueueIdIn)
{
	if	(lModeIn == DMM_DEBUG_ALLOC)
		// AllocMem
	{
		for	(LSA_UINT32 i = 0; i < DMM_DEBUG_SIZE_ARRAY; i++)
		{
			if	(m_lArrayQueue[lQueueIdIn].pAllocated[i] == NULL)
				// free item found
			{
				// store BlockAdr
				m_lArrayQueue[lQueueIdIn].pAllocated[i] = pMemIn;

				// leave loop
				break;
			}
		}
	}
	else if	(lModeIn == DMM_DEBUG_FREE)
			// FreeMem
	{
		for	(LSA_UINT32 i = 0;	i <	DMM_DEBUG_SIZE_ARRAY;	i++)
		{
			if	(m_lArrayQueue[lQueueIdIn].pAllocated[i] == pMemIn)
				// block found
			{
				// clear BlockAdr
				m_lArrayQueue[lQueueIdIn].pAllocated[i] = NULL;

				// leave loop
				break;
			}
		}
	}
}
#endif

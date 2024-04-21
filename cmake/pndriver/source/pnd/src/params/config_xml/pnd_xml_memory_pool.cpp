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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_xml_memory_pool.cpp                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Generic memory pool implementation. New block is allocated when          */ 
/*  existing one is exhausted. Free operation is done in one step            */
/*  and all allocated memory blocks are freed.                               */
/*                                                                           */
/*****************************************************************************/

#include "pnd_xml_memory_pool.h"
#include "pnd_sys.h"

CPndMemoryPool::CPndMemoryPool(PNIO_VOID_PTR_TYPE (*pAllocate)(PNIO_UINT32), PNIO_VOID(*pFree)(PNIO_VOID*))
{
    m_pUserAllocMethod = pAllocate;
    m_pUserFreeMethod = pFree;
    m_pAvailableMem = PNIO_NULL;
    m_pCurrentBlock = PNIO_NULL;  
}

CPndMemoryPool::~CPndMemoryPool()
{
    freeAll();
}

void CPndMemoryPool::freeAll()
{
    PNIO_INT8* nextBlockToFree;

    while(m_pCurrentBlock != PNIO_NULL)
    {  
        //get next block to free from the beggining of current block
        nextBlockToFree = *((PNIO_INT8**)m_pCurrentBlock);

        if(m_pUserFreeMethod != PNIO_NULL)
        {
            m_pUserFreeMethod(m_pCurrentBlock);
        }
        else
        {
            free(m_pCurrentBlock);
        }
        m_pCurrentBlock = nextBlockToFree;
    }
}

PNIO_INT8* CPndMemoryPool::allocateNewBlock(PNIO_UINT32 size) const
{
    PNIO_INT8* pReturnBlock = PNIO_NULL;

    if(m_pUserAllocMethod != NULL)
    {
        pReturnBlock = (PNIO_INT8*)(m_pUserAllocMethod(size));
    }
    else
    {
        pReturnBlock = (PNIO_INT8*)(malloc(size));
    }
     
    return pReturnBlock;
}

PNIO_VOID_PTR_TYPE CPndMemoryPool::allocateFromPool(PNIO_UINT32 size)
{
    PNIO_INT8* returnMem = PNIO_NULL;

    //Return PNIO_NULL if requested size is larger than maximum allowed size
    if( size > (BLOCK_SIZE-sizeof(m_pCurrentBlock)) )
    {
        returnMem = PNIO_NULL;
    }
    else
    {
        //If there is no enough space in the current block
        if( ( (m_pAvailableMem+size) > (m_pCurrentBlock + BLOCK_SIZE) ) || (m_pCurrentBlock == PNIO_NULL) )
        {
            //allocate new block
            PNIO_INT8* newBlock = allocateNewBlock(BLOCK_SIZE);

            //put old block start address to the beginnig of new block 
            *((PNIO_INT8**)newBlock) = m_pCurrentBlock;

            m_pCurrentBlock = newBlock;
            m_pAvailableMem = m_pCurrentBlock + sizeof(m_pCurrentBlock);
        }

        //allocate from current block
        returnMem = m_pAvailableMem;
        m_pAvailableMem += size;
    }
    
    return returnMem;
}
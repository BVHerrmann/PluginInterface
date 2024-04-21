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
/*  F i l e               &F: pnd_EDDIPiMemManager.cpp               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1204
#define PND_MODULE_ID       1204

#include "pnd_EDDIPiMemManager.h"
#include "pnd_IPndAdapter.h"
#include "pnd_trc.h"
#include "pnd_iodu.h"
#include "pnd_PIPool.h"
#include "pnd_ValueHelper.h"

//#include "eps_cp_mem.h"

CEDDIPiMemManager::CEDDIPiMemManager(IPndAdapter *pndAdapter, IPIPool *piPool) :
m_pPndAdapter(pndAdapter),
m_pPIPool(piPool),
m_PoolHandle(0)
{

}

CEDDIPiMemManager::~CEDDIPiMemManager()
{
    if(m_pPndAdapter != PNIO_NULL)
    {
        delete m_pPndAdapter;
        m_pPndAdapter = PNIO_NULL;
    }
	if (m_pPIPool != PNIO_NULL)
	{
		delete m_pPIPool;
		m_pPIPool = PNIO_NULL;
	}
}
/*lint -e{818} Pointer parameter 'pBufferProperties' could be declared as pointing to const*/
PNIO_UINT32 CEDDIPiMemManager::allocShadowBuffer(PND_BUFFER_PROPERTIES_PTR_TYPE pBufferProperties,
    PNIO_UINT8 *pImage, PNIO_UINT32 allocSize, PND_IODU_BUFFER_PTR_TYPE *ppBufferArrayEntry)
{
    PND_IODU_BUFFER_PTR_TYPE pIODUBuffer;
    PNIO_UINT32 retval = PNIO_OK;

    pIODUBuffer = (PND_IODU_BUFFER_PTR_TYPE)pBufferProperties->buffer_handle;    

    pIODUBuffer->p_shadow_buffer = (PNIO_UINT8*)m_pPndAdapter->memAlloc(allocSize);

    if (pIODUBuffer->p_shadow_buffer != PNIO_NULL)
    {
        pnd_memset(pIODUBuffer->p_shadow_buffer, 0/*init_value*/, allocSize);
        pIODUBuffer->eddi_buffer_handle = pImage;
        pIODUBuffer->shadow_buffer_size = allocSize;
        pIODUBuffer->is_provider = pBufferProperties->is_provider;

        *ppBufferArrayEntry = pIODUBuffer;

        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "CEDDIPiMemManager::allocShadowBuffer(): Shadow buffer alloc done.");
    }
    else
    {
        m_pPndAdapter->memFree(pBufferProperties->buffer_handle);
        m_pPndAdapter->poolFreeMem(m_PoolHandle, pImage);

        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::allocShadowBuffer(): No local memory available (buffer_handle)!");
        retval = PNIO_ERR_NO_RESOURCE;
    }

    return retval;
}

PNIO_UINT32 CEDDIPiMemManager::piAlloc(PND_BUFFER_PROPERTIES_PTR_TYPE pBufferProperties,
    PND_IODU_BUFFER_PTR_TYPE *ppBufferArrayEntry, PNIO_BOOL *pIsBufferNeeded)
{    
    PNIO_UINT8* pImage;
    PNIO_UINT32 allocSize;
    CValueHelper val;
    PNIO_UINT32 retval = PNIO_OK;

    if(pBufferProperties == PNIO_NULL ||
        pIsBufferNeeded == PNIO_NULL || 
        ppBufferArrayEntry == PNIO_NULL)
    {
        retval = PNIO_ERR_PRM_INVALIDARG;
        return retval;
    }
    
    if (pBufferProperties->partial_length != 0)
    {
        // these arguments are not used with EDDI
        pBufferProperties->edd_forward_mode = 0;
        pBufferProperties->edd_ct_outdatastart = 0;

        // RoundUP partialLength to next DWORD boundary (see EDDx docu)
        allocSize = val.AlignSize(pBufferProperties->partial_length, 4);

        // Allocate PI Buffer
        pImage = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_PoolHandle, allocSize);
                
        if (pImage != PNIO_NULL)
        {
            PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "CEDDIPiMemManager::piAlloc(): PI buffer alloc done.");

			m_pPIPool->paea_mem_set(pImage, 0/*init_value*/, allocSize);
			pBufferProperties->edd_data_offset = (LSA_UINT32)pImage - (LSA_UINT32)(m_pPIPool->getPoolBasePtr());
            pBufferProperties->edd_properties = pBufferProperties->is_provider ? 
                EDD_PROV_BUFFER_PROP_IRTE_IMG_ASYNC : EDD_CONS_BUFFER_PROP_IRTE_IMG_ASYNC;
            pBufferProperties->buffer_handle = m_pPndAdapter->memAlloc(sizeof(PND_IODU_BUFFER_TYPE));

            if (pBufferProperties->buffer_handle != PNIO_NULL)
            {
                retval = allocShadowBuffer(pBufferProperties, pImage, allocSize, ppBufferArrayEntry);
            }
            else
            {
                PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::piAlloc(): No local memory available (buffer_handle)!");

                // Free PI Buffer
                m_pPndAdapter->poolFreeMem(m_PoolHandle, pImage);
                retval = PNIO_ERR_NO_RESOURCE;                
            }
        }
        else
        {
            PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::piAlloc(): No PI buffer available!");

            pBufferProperties->edd_data_offset = EDD_DATAOFFSET_INVALID;
            pBufferProperties->edd_properties = pBufferProperties->is_provider ? EDD_PROV_BUFFER_PROP_IRTE_IMG_INVALID : EDD_CONS_BUFFER_PROP_IRTE_IMG_INVALID;
            retval = PNIO_ERR_NO_RESOURCE;
        }
    }
    else
    {
        *pIsBufferNeeded = PNIO_FALSE;
        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "CEDDIPiMemManager::piAlloc(): partial_length == 0 -> No EDDI buffer needed.");
    }

    return retval;
}

PNIO_UINT32 CEDDIPiMemManager::piFree(PND_IODU_BUFFER_PTR_TYPE pBuffer)
{
    PND_IODU_BUFFER_PTR_TYPE pIODUBuffer;
    PNIO_UINT32 retval = PNIO_OK;
    LSA_UINT16 lsaResult = LSA_OK;

    pIODUBuffer = pBuffer;

    if(pIODUBuffer != PNIO_NULL 
        && pIODUBuffer->eddi_buffer_handle != PNIO_NULL 
        && pIODUBuffer->p_shadow_buffer != PNIO_NULL)
    {
        lsaResult = m_pPndAdapter->poolFreeMem(m_PoolHandle, pIODUBuffer->eddi_buffer_handle);
        if(lsaResult != LSA_OK)
        {
            PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::piFree(): error on psiPiFreeMem(), result(%x)", lsaResult);
            retval = PNIO_ERR_INTERNAL;
        }
        else
        {            
            m_pPndAdapter->memFree(pIODUBuffer->p_shadow_buffer);
        }

        m_pPndAdapter->memFree(pIODUBuffer);
    }
    else
    {
        retval = PNIO_ERR_PRM_INVALIDARG;
        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::piFree(): iodu buffer or eddi buffer handle or shadow buffer is NULL!");
    }

    return retval;
}

LSA_RESPONSE_TYPE CEDDIPiMemManager::createPIMemPool(PNIO_UINT32 ioccVirtualAddr)
{
    LSA_RESPONSE_TYPE retval = LSA_RET_OK;
    PNIO_UINT32 piSize = 0;    

    piSize = (PND_IOCC_PA_EA_DIRECT_end - PND_IOCC_PA_EA_DIRECT_start)/4 + 1; // RAM size

    if (ioccVirtualAddr != 0)
    {
		m_pPIPool->Create((PNIO_UINT8*)(ioccVirtualAddr + PND_IOCC_PA_EA_DIRECT_start));
	}
    else
    {
        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CEDDIPiMemManager::createPIMemPool(): IOCC virtual address not set!");
    }

    // Create Pi MemPool
	retval = m_pPndAdapter->createMemPool(&m_PoolHandle, (PNIO_UINT32*)m_pPIPool->getPoolBasePtr(), piSize, 4, CPIPool::paeaMemSet);

    return retval;
}

LSA_RESPONSE_TYPE CEDDIPiMemManager::deletePIMemPool()
{
    LSA_UINT16 retval = LSA_RET_OK;

    retval = m_pPndAdapter->deleteMemPool(m_PoolHandle);

    return retval;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

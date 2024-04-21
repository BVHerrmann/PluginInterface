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
/*  F i l e               &F: pnd_iodu_eddi.cpp                        :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of IODU_EDDI class                                        */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1203
#define PND_MODULE_ID       1203

#include "pnd_iodu_eddi.h"
//#include "pnd_pnstack.h"
#include "pnd_EDDIPiMemManager.h"
#include "pnd_pnio_user_core.h"
#include "pnd_IOCCProcessImageUpdater.h"
#include "pnd_Adapter.h"
#include "pnd_PIPool.h"
#include "pnd_trc.h"
/*lint --e{1746} parameter 'pndHandle' could be made const reference*/
CIODU_EDDI::CIODU_EDDI(PND_HANDLE_TYPE pndHandle, IPndAdapter *adapter) : CIODU(pndHandle, adapter), 
m_pPiMemManager(PNIO_NULL),
m_pProcessImageUpdater(PNIO_NULL)
{
	memset(m_sharedMem, 0, sizeof(m_sharedMem));
}

CIODU_EDDI::~CIODU_EDDI()
{
    LSA_RESPONSE_TYPE retVal = LSA_RET_OK;

    PND_ASSERT(m_pPiMemManager != PNIO_NULL);
    PND_ASSERT(m_pPndAdapter != PNIO_NULL);

    retVal = m_pPiMemManager->deletePIMemPool();
    PND_ASSERT(retVal == LSA_RET_OK);

    deletePiMemManager();
    deleteProcessImageUpdater();
}
/*lint -e{818} Pointer parameter 'params' could be declared as pointing to const*/
PNIO_VOID CIODU_EDDI::initEddi(INIT_PARAMS* params)
{
    IIOCCProcessImageUpdater::INIT_PARAMS initParams;

    LSA_RESPONSE_TYPE retval = LSA_RET_OK;

    PND_ASSERT(m_pPndAdapter != PNIO_NULL);

    m_pProcessImageUpdater = createProcessImageUpdater();
    PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);

    m_pPiMemManager = createPiMemManager();
    PND_ASSERT(m_pPiMemManager != PNIO_NULL);

    retval = m_pPiMemManager->createPIMemPool((PNIO_UINT32)(params->pIoccBase));
    PND_ASSERT(retval == LSA_RET_OK);
    
    initParams.ioccBaseAddress = (PNIO_UINT32)params->pIoccBase;
    initParams.ioccAhbAddress = params->ioccPhyAddr;
    initParams.pIoMemBase = params->pIoMemBase;
    initParams.ioMemPhyAddr = params->ioMemPhyAddr;
    initParams.ioMemSize = params->ioMemSize;
    initParams.hdNr = pnd_handle.hd_nr;

    PNIO_UINT32 result = m_pProcessImageUpdater->init(initParams);
    PND_ASSERT(result == PNIO_OK);    
        
    // eddi buffer request setup
    eddi_SysRed_BufferRequest_Init();
    LSA_UINT16 lsaRetval = m_pPndAdapter->eddiBufferRequestSetup(
        pnd_handle.if_nr,
        pnd_handle.if_nr,
        (LSA_VOID *)(initParams.ioccBaseAddress - 0x400000 + 0x100000),
        (LSA_VOID *)(initParams.ioccBaseAddress - 0x400000),
        &m_sharedMem,
        PNIO_NULL, 0, 0, 0);

    PND_ASSERT(lsaRetval == LSA_RET_OK);
}

PNIO_VOID CIODU_EDDI::allocateLinkLists( PND_RQB_PTR_TYPE pRqb )
{
    PNIO_UINT32 ret;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> CIODU_EDDI::allocateLinkList()");

	PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
    ret = m_pProcessImageUpdater->allocateLinkLists();

    pRqb->args.ll_alloc.pnio_err = ret;

    //return rqb to caller
    m_pPndAdapter->requestStart(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, pRqb);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< CIODU_EDDI::allocateLinkList()");
}

PNIO_VOID CIODU_EDDI::freeLinkLists( PND_RQB_PTR_TYPE pRqb )
{
    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> CIODU_EDDI::freeLinkList()");

	PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
    m_pProcessImageUpdater->freeLinkLists();

    pRqb->args.ll_free.pnio_err = PNIO_OK;

    //return rqb to caller
    m_pPndAdapter->requestStart(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, pRqb);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< CIODU_EDDI::freeLinkList()");
}

PNIO_VOID CIODU_EDDI::PI_Alloc( PND_RQB_PTR_TYPE pRqb )
{
    PNIO_UINT32 ret;
    PNIO_UINT32 piArrIdx;
    PNIO_BOOL isBufferNeeded = PNIO_TRUE;
    PND_BUFFER_PROPERTIES_PTR_TYPE pProviderBuffer = &pRqb->args.pi_alloc.provider_buffer;
    PND_BUFFER_PROPERTIES_PTR_TYPE pConsumerBuffer = &pRqb->args.pi_alloc.consumer_buffer;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> CIODU_EDDI::PI_Alloc()");

    // Find a free slot to store a link to the consumer/provide buffer
    ret = findFreeSlot(PROVIDER, &piArrIdx);

    if(ret == PNIO_OK)
    {
		PND_ASSERT(m_pPiMemManager != PNIO_NULL);
        ret = m_pPiMemManager->piAlloc(pProviderBuffer, 
            &provider_pi_buffer_array[piArrIdx], &isBufferNeeded);

        if(ret == PNIO_OK)
        {
            ret = findFreeSlot(CONSUMER, &piArrIdx);

            if (ret == PNIO_OK)
            {
				PND_ASSERT(m_pPiMemManager != PNIO_NULL);
                ret = m_pPiMemManager->piAlloc(pConsumerBuffer, 
                    &consumer_pi_buffer_array[piArrIdx], &isBufferNeeded);
            }
            else
            {
                PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CIODU_EDDI::PI_Alloc(): Resource error on consumer buffer array! retval(%#x)", ret);
            }
        }
    }
    else
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CIODU_EDDI::PI_Alloc(): Resource error on provider buffer array! retval(%#x)", ret);
    }
    
    //continue with creating IQ table entry and iso consistency object list
    if (ret == PNIO_OK && isBufferNeeded) 
    {
        ret = check_params_IO_addr(pRqb);

        if (ret == PNIO_OK)
        {
            ret = create_IQ_table(pRqb);

            if (ret == PNIO_OK)
            {
                addIsoCnstObjects(pRqb);
            }
        }
    }

    pRqb->args.pi_alloc.pnio_err = ret;

    //return rqb to caller
    m_pPndAdapter->requestStart(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, pRqb);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< CIODU_EDDI::PI_Alloc()");
}

PNIO_VOID CIODU_EDDI::PI_Free( PND_RQB_PTR_TYPE pRqb)
{
    PNIO_UINT32 retval;
    PND_IODU_BUFFER_PTR_TYPE pProviderIODUBuffer = (PND_IODU_BUFFER_PTR_TYPE)pRqb->args.pi_free.consumer_buffer_handle;
    PND_IODU_BUFFER_PTR_TYPE pConsumerIODUBuffer = (PND_IODU_BUFFER_PTR_TYPE)pRqb->args.pi_free.provider_buffer_handle;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> CIODU_EDDI::PI_Free()");

    removeUsedSlot(pProviderIODUBuffer);

    delete_IQ_table_entries(pRqb);
    deleteIsoCnstObjects(pRqb);

    retval = m_pPiMemManager->piFree(pProviderIODUBuffer);

    if(retval == PNIO_OK)
    {
        removeUsedSlot(pConsumerIODUBuffer);

        retval = m_pPiMemManager->piFree(pConsumerIODUBuffer);
    }

    pRqb->args.pi_free.pnio_err = retval;

    //return rqb to caller
    m_pPndAdapter->requestStart(PND_MBX_ID_PNIO_USER_CORE, (PND_REQUEST_FCT)pnd_pnio_user_core_request_done, pRqb);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< CIODU_EDDI::PI_Free()");
}

PNIO_VOID CIODU_EDDI::update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODUBufferHandle)
{
    PNIO_UINT32 result = PNIO_OK;

    //transfer from single shadow buffer to process image
	PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
    result = m_pProcessImageUpdater->transferSingleBuffer(pIODUBufferHandle->p_shadow_buffer, (PNIO_UINT32)pIODUBufferHandle->eddi_buffer_handle,
                                              pIODUBufferHandle->shadow_buffer_size, IIOCCProcessImageUpdater::OUTPUT);

    if (result != PNIO_OK)
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CIODU_EDDI::update_provider_shadow_buffer(): failed with %u", result);
    }
}

PNIO_VOID CIODU_EDDI::update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODUBufferHandle)
{
    PNIO_UINT32 result = PNIO_OK;

    //transfer from process image to single shadow buffer
	PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
    result = m_pProcessImageUpdater->transferSingleBuffer(pIODUBufferHandle->p_shadow_buffer, (PNIO_UINT32)pIODUBufferHandle->eddi_buffer_handle,
                                              pIODUBufferHandle->shadow_buffer_size, IIOCCProcessImageUpdater::INPUT);

    if (result != PNIO_OK)
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CIODU_EDDI::update_consumer_shadow_buffer(): failed with %u", result);
    }
}

PNIO_UINT32 CIODU_EDDI::updateIsoInputs()
{
    PNIO_UINT32 result = PNIO_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> updateIsoInputs");
    PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);

    //Transfer all consistency objects previously added from process image to shadow buffers
    result = m_pProcessImageUpdater->transferCnstObjects(IIOCCProcessImageUpdater::INPUT);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< updateIsoInputs");

    return result;
}

PNIO_UINT32 CIODU_EDDI::updateIsoOutputs()
{
    PNIO_UINT32 result = PNIO_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">> updateIsoOutputs");
    PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);

    //Transfer all consistency objects previously added from shadow buffers to process image 
    result = m_pProcessImageUpdater->transferCnstObjects(IIOCCProcessImageUpdater::OUTPUT);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<< updateIsoOutputs");

    return result;
}

PNIO_VOID CIODU_EDDI::addIsoCnstObjects(PND_RQB_PTR_TYPE pRqb)
{
    PND_BUFFER_PROPERTIES_PTR_TYPE rqbArgsConsumer = &(pRqb->args.pi_alloc.consumer_buffer);
    PND_BUFFER_PROPERTIES_PTR_TYPE rqbArgsProvider = &(pRqb->args.pi_alloc.provider_buffer);
    PND_IODU_BUFFER_PTR_TYPE pIoduBufferConsumer = (PND_IODU_BUFFER_PTR_TYPE)rqbArgsConsumer->buffer_handle;
    PND_IODU_BUFFER_PTR_TYPE pIoduBufferProvider = (PND_IODU_BUFFER_PTR_TYPE)rqbArgsProvider->buffer_handle;
    PND_IO_ADDR_PTR_TYPE pIOAddr = PNIO_NULL;

    for (PNIO_UINT32 i = 0; i<rqbArgsConsumer->number_of_IO_addr; i++)
    {
        pIOAddr = &rqbArgsConsumer->IO_addr_array[i];

        if (pIOAddr->isIsoIOData == LSA_TRUE && pIOAddr->length_IO_data > 0)
        {
			PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
            m_pProcessImageUpdater->addCnstObject(pIoduBufferConsumer->p_shadow_buffer + pIOAddr->offset_IO_data,
                (PNIO_UINT32)(pIoduBufferConsumer->eddi_buffer_handle) + pIOAddr->offset_IO_data,
                pIOAddr->length_IO_data + 1, IIOCCProcessImageUpdater::INPUT);

            m_pProcessImageUpdater->addCnstObject(pIoduBufferProvider->p_shadow_buffer + pIOAddr->offset_IOCS,
                (PNIO_UINT32)(pIoduBufferProvider->eddi_buffer_handle) + pIOAddr->offset_IOCS,
                1, IIOCCProcessImageUpdater::OUTPUT);
        }
    }

    for(PNIO_UINT32 i = 0; i<rqbArgsProvider->number_of_IO_addr; i++)
    {
        pIOAddr = &rqbArgsProvider->IO_addr_array[i];

        if(pIOAddr->isIsoIOData == LSA_TRUE && pIOAddr->length_IO_data > 0)
        {
			PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
            m_pProcessImageUpdater->addCnstObject(pIoduBufferProvider->p_shadow_buffer + pIOAddr->offset_IO_data,
                (PNIO_UINT32)(pIoduBufferProvider->eddi_buffer_handle) + pIOAddr->offset_IO_data,
                pIOAddr->length_IO_data + 1, IIOCCProcessImageUpdater::OUTPUT);

            m_pProcessImageUpdater->addCnstObject(pIoduBufferConsumer->p_shadow_buffer + pIOAddr->offset_IOCS,
                (PNIO_UINT32)(pIoduBufferConsumer->eddi_buffer_handle) + pIOAddr->offset_IOCS,
                1, IIOCCProcessImageUpdater::INPUT);
        }
    }
}

/*lint -e{818} Pointer parameter 'pRqb' could be declared as pointing to const*/
PNIO_VOID CIODU_EDDI::deleteIsoCnstObjects(PND_RQB_PTR_TYPE pRqb)
{
    PND_IODU_BUFFER_PTR_TYPE pIoduBufferConsumer = (PND_IODU_BUFFER_PTR_TYPE)pRqb->args.pi_free.consumer_buffer_handle;
    PND_IODU_BUFFER_PTR_TYPE pIoduBufferProvider = (PND_IODU_BUFFER_PTR_TYPE)pRqb->args.pi_free.provider_buffer_handle;

    if(pIoduBufferConsumer != PNIO_NULL)
    {
		PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
        m_pProcessImageUpdater->removeCnstObjects(pIoduBufferConsumer->p_shadow_buffer, pIoduBufferConsumer->shadow_buffer_size, IIOCCProcessImageUpdater::INPUT);
    }

    if (pIoduBufferProvider != PNIO_NULL)
    {
		PND_ASSERT(m_pProcessImageUpdater != PNIO_NULL);
        m_pProcessImageUpdater->removeCnstObjects(pIoduBufferProvider->p_shadow_buffer, pIoduBufferProvider->shadow_buffer_size, IIOCCProcessImageUpdater::OUTPUT);
    }
}

PNIO_UINT32 CIODU_EDDI::findFreeSlot(BUFFER_TYPE bufferType, PNIO_UINT32 *pIndex) const
{
    PNIO_UINT32 idx;
    PNIO_UINT32 retval = PNIO_OK;

    if (bufferType == PROVIDER)
    {
        /* Find a free spot to store a link to the provider buffer */
        for (idx = 0; idx < PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS; idx++)
        {
            if (provider_pi_buffer_array[idx] == PNIO_NULL)
            {
                *pIndex = idx;
                break;
            }
        }
        if (idx >= PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS)
        {
            retval = PNIO_ERR_NO_RESOURCE;
        }
    }
    else if (bufferType == CONSUMER)
    {
        /* Find a free spot to store a link to the consumer buffer */
        for (idx = 0; idx < PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS; idx++)
        {
            if (consumer_pi_buffer_array[idx] == PNIO_NULL)
            {
                *pIndex = idx;
                break;
            }
        }
        if (idx >= PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS)
        {
            retval = PNIO_ERR_NO_RESOURCE;
        }
    }

    return retval;
}

/*lint -e{818} Pointer parameter 'pIODUBuffer' could be declared as pointing to const*/
PNIO_VOID CIODU_EDDI::removeUsedSlot(const PND_IODU_BUFFER_PTR_TYPE pIODUBuffer)
{
    PNIO_UINT32 idx;

    /* Delete IODU buffer handle from PI buffer handle array */
    for (idx = 0; idx < PND_IODU_CFG_MAX_CONSUMER_PI_BUFFERS; idx++)
    {
        if (consumer_pi_buffer_array[idx] == pIODUBuffer)
        {
            consumer_pi_buffer_array[idx] = LSA_NULL;
            break;
        }
    }
    for (idx = 0; idx < PND_IODU_CFG_MAX_PROVIDER_PI_BUFFERS; idx++)
    {
        if (provider_pi_buffer_array[idx] == pIODUBuffer)
        {
            provider_pi_buffer_array[idx] = LSA_NULL;
            break;
        }
    }
}

IEDDIPiMemManager* CIODU_EDDI::createPiMemManager()
{
	return new CEDDIPiMemManager(new CPndAdapter(), new CPIPool());
}

PNIO_VOID CIODU_EDDI::deletePiMemManager()
{
    if (m_pPiMemManager != PNIO_NULL)
    {
        delete m_pPiMemManager;
        m_pPiMemManager = PNIO_NULL;
    }
}

IIOCCProcessImageUpdater* CIODU_EDDI::createProcessImageUpdater()
{
    return new CIOCCProcessImageUpdater();
}

PNIO_VOID CIODU_EDDI::deleteProcessImageUpdater()
{
    if(m_pProcessImageUpdater != PNIO_NULL)
    {
        delete m_pProcessImageUpdater;
        m_pProcessImageUpdater = PNIO_NULL;
    }
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

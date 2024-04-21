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
/*  F i l e               &F: pnd_IOCCProcessImageUpdater.cpp           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CIOCCProcessImageUpdater class                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   1126
#define PND_MODULE_ID       1126

#include "pnd_IOCCProcessImageUpdater.h"
#include "pnd_Adapter.h"
#include "pnd_trc.h"

#include "eps_cp_hw.h"
#include "eddi_iocc_int.h"
#include "pnd_EDDIPiMemManager.h"

#include <algorithm>

LSA_UINT8* const CIOCCProcessImageUpdater::m_pDummyExtLinkListBase = (LSA_UINT8*)1;
PNIO_UINT8* CIOCCProcessImageUpdater::m_pIntLinkListMemMultipleRw = PNIO_NULL;

CIOCCProcessImageUpdater::CIOCCProcessImageUpdater() : m_initialized(PNIO_FALSE),
m_pPndAdapter(PNIO_NULL),
m_pa_ea_baseAddress(0),
m_pMbxSingleRw(PNIO_NULL),
m_pMbxMultipleRw(PNIO_NULL),
m_ioMemPoolId(0),
m_ioccLocalToTargetAddressOffset(0),
m_pCachedReadBuffer(PNIO_NULL),
m_pIoccHandleSingleRw(PNIO_NULL),
m_pIntLinkListMemSingleRw(PNIO_NULL),
m_intLinkListMemSizeSingleRw(0),
m_pHostBufferSingleRw(PNIO_NULL),
m_pIoccHandleMultipleRw(PNIO_NULL),
m_pIntLinkListMemSizeMultipleRw(0),
m_pLLHandleMultipleWrite(PNIO_NULL),
m_pLLHandleMultipleRead(PNIO_NULL)
{
    memset(&m_initParams, 0, sizeof(m_initParams));  
	m_ioccErrorSingleRw = {};
	m_ioccErrorMultipleRw = {};
    m_ioccUpperHandle.llMempoolId = 0;
}

CIOCCProcessImageUpdater::~CIOCCProcessImageUpdater()
{
    LSA_RESULT lsaResult = LSA_RET_OK;
	
    if(m_ioMemPoolId != POOL_UNKNOWN)
    {
        if (m_pMbxSingleRw != PNIO_NULL)
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pMbxSingleRw);
            PND_ASSERT(lsaResult == LSA_RET_OK);

            m_pMbxSingleRw = PNIO_NULL;
        }

        if(m_pMbxMultipleRw != PNIO_NULL)
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pMbxMultipleRw);
            PND_ASSERT(lsaResult == LSA_RET_OK);

            m_pMbxMultipleRw = PNIO_NULL;
        }

        if (m_pHostBufferSingleRw != PNIO_NULL)
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pHostBufferSingleRw);
            PND_ASSERT(lsaResult == LSA_RET_OK);

            m_pHostBufferSingleRw = PNIO_NULL;
        }
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->deleteMemPool(m_ioMemPoolId);
        PND_ASSERT(lsaResult == LSA_RET_OK);
    }

    if(m_pIoccHandleSingleRw != PNIO_NULL)
    {
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->eddiIOCCShutdown(m_pIoccHandleSingleRw);
        PND_ASSERT(lsaResult == EDD_STS_OK);

        m_pIoccHandleSingleRw = PNIO_NULL;
    }

    if(m_pCachedReadBuffer != PNIO_NULL)
    {
        PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        m_pPndAdapter->memFree(m_pCachedReadBuffer);

        m_pCachedReadBuffer = PNIO_NULL;
    }

    if (m_pIoccHandleMultipleRw != PNIO_NULL)
    {
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->eddiIOCCShutdown(m_pIoccHandleMultipleRw);
        PND_ASSERT(lsaResult == EDD_STS_OK);

        m_pIoccHandleMultipleRw = PNIO_NULL;

		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->deleteMemPool(m_ioccUpperHandle.llMempoolId);
        PND_ASSERT(lsaResult == LSA_RET_OK);
    }
	if(m_pPndAdapter != PNIO_NULL)
	{
		if(m_pPndAdapter->isPndEddiIoccCh1LockAllocated() == PNIO_TRUE)
		{
			m_pPndAdapter->pndEddiFreeIoccCh1Lock();
		}
		if(m_pPndAdapter->isPndEddiIoccCh2LockAllocated() == PNIO_TRUE)
		{
			m_pPndAdapter->pndEddiFreeIoccCh2Lock();
		}
	}
	
    
    deletePndAdapter();
}

PNIO_UINT32 CIOCCProcessImageUpdater::init(INIT_PARAMS& _initParams)
{
    PNIO_UINT32 result = PNIO_OK;
    LSA_RESULT lsaResult = EDD_STS_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::init()");

    m_initParams = _initParams;
    m_pa_ea_baseAddress = m_initParams.ioccBaseAddress + PND_IOCC_PA_EA_DIRECT_start;
    m_pPndAdapter = createPndAdapter();

    lsaResult = m_pPndAdapter->createMemPool(&m_ioMemPoolId, (PNIO_UINT32*)m_initParams.pIoMemBase, m_initParams.ioMemSize, 4, eps_memset);
    PND_ASSERT(lsaResult == LSA_RET_OK);

    m_ioccLocalToTargetAddressOffset = calculateLocalToTargetAddressOffset(m_initParams.pIoMemBase, m_initParams.ioMemPhyAddr);

    m_pCachedReadBuffer = (PNIO_UINT8*)m_pPndAdapter->memAlloc(EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE); 
    PND_ASSERT(m_pCachedReadBuffer != PNIO_NULL);

    m_pMbxSingleRw = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_ioMemPoolId, IOCC_MB_DATA_SIZE);
    PND_ASSERT(m_pMbxSingleRw != PNIO_NULL);

    m_pMbxMultipleRw = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_ioMemPoolId, IOCC_MB_DATA_SIZE);
    PND_ASSERT(m_pMbxMultipleRw != PNIO_NULL);

    m_pHostBufferSingleRw = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_ioMemPoolId, EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE);
    PND_ASSERT(m_pHostBufferSingleRw != PNIO_NULL);

    // 
	m_pPndAdapter->pndEddiAllocIoccCh1Lock();
	m_pPndAdapter->pndEddiAllocIoccCh2Lock();

    lsaResult = m_pPndAdapter->eddiIOCCSetup(&m_pIoccHandleSingleRw,
        &m_ioccUpperHandle.llMempoolId,
        m_initParams.hdNr,
        IOCC_CHANNEL_SINGLE_RW,
        (PNIO_VOID*)(m_initParams.ioccBaseAddress),
        m_initParams.pIoMemBase,
        (PNIO_UINT32)(m_initParams.pIoMemBase) + m_ioccLocalToTargetAddressOffset,
        m_pDummyExtLinkListBase,
        DUMMY_EXT_LINK_LIST_BASE_PHY_ADDR,
        &m_pIntLinkListMemSingleRw,
        &m_intLinkListMemSizeSingleRw,
		0,
		static_cast<PNIO_UINT32>(m_pMbxSingleRw - m_initParams.pIoMemBase),
        &m_ioccErrorSingleRw);

    if (lsaResult == EDD_STS_OK)
    {
        lsaResult = m_pPndAdapter->eddiIOCCSetup(&m_pIoccHandleMultipleRw,
            &m_ioccUpperHandle.llMempoolId,
            m_initParams.hdNr,
            IOCC_CHANNEL_MULTIPLE_RW,
            (PNIO_VOID*)(m_initParams.ioccBaseAddress),
            m_initParams.pIoMemBase,
            (PNIO_UINT32)(m_initParams.pIoMemBase) + m_ioccLocalToTargetAddressOffset,
            m_pDummyExtLinkListBase,
            DUMMY_EXT_LINK_LIST_BASE_PHY_ADDR,
            &m_pIntLinkListMemMultipleRw,
            &m_pIntLinkListMemSizeMultipleRw,
            0,
			static_cast<PNIO_UINT32>(m_pMbxMultipleRw - m_initParams.pIoMemBase),
            &m_ioccErrorMultipleRw);

        if (lsaResult == EDD_STS_OK)
        {
            lsaResult = m_pPndAdapter->createMemPool(&m_ioccUpperHandle.llMempoolId, (PNIO_UINT32*)m_pIntLinkListMemMultipleRw, m_pIntLinkListMemSizeMultipleRw, 4, memSet);
            PND_ASSERT(lsaResult == LSA_RET_OK);
        }
        else
        {
            lsaResult = m_pPndAdapter->eddiIOCCShutdown(m_pIoccHandleSingleRw);
            PND_ASSERT(lsaResult == EDD_STS_OK);
        }
    }

    if (lsaResult == EDD_STS_OK)
    {
        PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CIOCCProcessImageUpdater::init(): initialization done");

        result = PNIO_OK;
        m_initialized = PNIO_TRUE;
    }
    else
    {
        m_pPndAdapter->memFree(m_pCachedReadBuffer);
        m_pCachedReadBuffer = PNIO_NULL;

        lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pMbxSingleRw);
        PND_ASSERT(lsaResult == LSA_RET_OK);
        m_pMbxSingleRw = PNIO_NULL;

        lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pMbxMultipleRw);
        PND_ASSERT(lsaResult == LSA_RET_OK);
        m_pMbxMultipleRw = PNIO_NULL;

        lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, m_pHostBufferSingleRw);
        PND_ASSERT(lsaResult == LSA_RET_OK);
        m_pHostBufferSingleRw = PNIO_NULL;

        lsaResult = m_pPndAdapter->deleteMemPool(m_ioMemPoolId);
        PND_ASSERT(lsaResult == LSA_RET_OK);
		
		m_pPndAdapter->pndEddiFreeIoccCh1Lock();
		m_pPndAdapter->pndEddiFreeIoccCh2Lock();

        deletePndAdapter();

        result = PNIO_ERR_NO_RESOURCE;
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::init()");

    return result;
}

/*lint -e{818} Pointer parameter 'pObject' could be declared as pointing to const*/
PNIO_VOID CIOCCProcessImageUpdater::directRead(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length) const
{
    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::directRead()");

    PND_ASSERT(length <= 4);

    PNIO_UINT32* pSrc = (PNIO_UINT32*)PND_IOCC_PROCESS_IMAGE_UPDATER_CALC_REAL_PAEA_RAM_ADDRESS(m_pa_ea_baseAddress, pa_ea_adr);

    PND_IOCC_DIRECT_ACCESS_TYPE directAccess;
    PNIO_UINT8* pDst = pHostBuf;

    directAccess.as_u32 = *pSrc;

    PNIO_UINT8 j=0;
    switch (length)
    {
    case 4:
        *pDst++ = directAccess.as_u8[j++];
        /* no break */
	//lint -fallthrough
    case 3:
        *pDst++ = directAccess.as_u8[j++];
        /* no break */
	//lint -fallthrough
    case 2:
        *pDst++ = directAccess.as_u8[j++];
        /* no break */
	//lint -fallthrough
    case 1:
        *pDst = directAccess.as_u8[j];
        break;
    default:
        break;
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::directRead()");
}

/*lint -e{818} Pointer parameter 'pObject' could be declared as pointing to const */
PNIO_VOID CIOCCProcessImageUpdater::directWrite(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length) const
{
    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::directWrite()");

    PND_ASSERT(length <= 4);

    PNIO_UINT8* pDst = (PNIO_UINT8*)PND_IOCC_PROCESS_IMAGE_UPDATER_CALC_REAL_PAEA_RAM_ADDRESS(m_pa_ea_baseAddress, pa_ea_adr);
    PNIO_UINT8* pSrc = pHostBuf;
    PND_IOCC_DIRECT_ACCESS_TYPE directAccess;

    switch (length)
    {
    case 4:
        directAccess.as_u8[0] = *pSrc++;
        directAccess.as_u8[1] = *pSrc++;
        directAccess.as_u8[2] = *pSrc++;
        directAccess.as_u8[3] = *pSrc;
        *((PNIO_UINT32*)pDst) = directAccess.as_u32;
        break;
    case 3:
        directAccess.as_u32 = *((PNIO_UINT32*)pDst);
        directAccess.as_u8[0] = *pSrc++;
        directAccess.as_u8[1] = *pSrc++;
        directAccess.as_u8[2] = *pSrc;
        *((PNIO_UINT32*)pDst) = directAccess.as_u32;
        break;
    case 2:
        directAccess.as_u8[0] = *pSrc++;
        directAccess.as_u8[1] = *pSrc;
        *((PNIO_UINT16*)pDst) = directAccess.as_u16[0];
        break;
    case 1:
        *pDst = *pSrc;
        break;
    default:
        break;
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::directWrite()");
}

PNIO_UINT32 CIOCCProcessImageUpdater::transferSingleBuffer(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type)
{
    PNIO_UINT32 byteOffset = pa_ea_adr - m_pa_ea_baseAddress;
    PNIO_UINT32 result = PNIO_OK;
    LSA_RESULT lsaResult = EDD_STS_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::transferSingleBuffer()");

    if (type == INPUT)
    {
        if (length <= PND_IOCC_DIRECT_ACCESS_MAX_LENGTH)
        {
            directRead(pHostBuf, pa_ea_adr, length);
        }
        else
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->eddiIOCCSingleRead(m_pIoccHandleSingleRw, length, byteOffset, (PNIO_UINT8*)m_pHostBufferSingleRw);

            if (lsaResult == EDD_STS_OK || lsaResult == EDD_STS_OK_PENDING)
            {
				PND_ASSERT(m_pHostBufferSingleRw != PNIO_NULL);
                pnd_memcpy(pHostBuf, m_pHostBufferSingleRw, length);
            }
        }
    }
    else if (type == OUTPUT)
    {
        if (length <= PND_IOCC_DIRECT_ACCESS_MAX_LENGTH)
        {
            directWrite(pHostBuf, pa_ea_adr, length);
        }
        else
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->eddiIOCCSingleWrite(m_pIoccHandleSingleRw, length, byteOffset, (PNIO_UINT8*)pHostBuf);
        }
    }
    else
    {
        PND_FATAL("CIOCCProcessImageUpdater::transferSingleBuffer(): Unknown data type");
    }

    if (lsaResult != EDD_STS_OK && lsaResult != EDD_STS_OK_PENDING)
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CIOCCProcessImageUpdater::transferSingleBuffer(): eddi returned error=%u", lsaResult);

        PND_IODU_TRACE_06(0, LSA_TRACE_LEVEL_ERROR, "CIOCCProcessImageUpdater::transferSingleBuffer(): Status=%u, DestAddr=%u, DestLength=%u, DebugReg=%u, Line=%u, ModuleID=%u",
            m_ioccErrorSingleRw.Status, m_ioccErrorSingleRw.DestAddr, m_ioccErrorSingleRw.DestLength, m_ioccErrorSingleRw.DebugReg, m_ioccErrorSingleRw.Line, m_ioccErrorSingleRw.ModuleID);

        result = PNIO_ERR_INTERNAL;
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::transferSingleBuffer()");

    return result;
}

PNIO_UINT32 CIOCCProcessImageUpdater::transferCnstObjects(IO_DATA_TYPE type)
{
    PNIO_UINT32 result = PNIO_OK;
    LSA_RESULT lsaResult = EDD_STS_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::transferCnstObjects()");

    if (type == INPUT)
    {
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->eddiIOCCMultipleRead(&m_linkListHandles[READ][0], m_linkLists[READ].size());
        copyReadCnstData();
    }
    else if (type == OUTPUT)
    {
        copyWriteCnstData();
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->eddiIOCCMultipleWrite(&m_linkListHandles[WRITE][0], m_linkLists[WRITE].size());
    }
    else
    {
        PND_FATAL("Wrong IO data type");
    }

    if ((lsaResult != EDD_STS_OK) && (lsaResult != EDD_STS_OK_PENDING) && (lsaResult != EDD_STS_OK_NO_DATA))
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CIOCCProcessImageUpdater::transferCnstObjects(): eddi returned error = %u", lsaResult);

        PND_IODU_TRACE_06(0, LSA_TRACE_LEVEL_ERROR, "CIOCCProcessImageUpdater::transferCnstObjects(): Status=%u, DestAddr=%u, DestLength=%u, DebugReg=%u, Line=%u, ModuleID=%u",
            m_ioccErrorMultipleRw.Status, m_ioccErrorMultipleRw.DestAddr, m_ioccErrorMultipleRw.DestLength, m_ioccErrorMultipleRw.DebugReg, m_ioccErrorMultipleRw.Line, m_ioccErrorMultipleRw.ModuleID);

        result = PNIO_ERR_INTERNAL;
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::transferCnstObjects()");

    return result;
}

PNIO_VOID CIOCCProcessImageUpdater::addCnstObject(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type)
{
    CONSISTENCY_OBJECT* pNewCnstObj = PNIO_NULL;
    vector<CONSISTENCY_OBJECT*>* pCnstObjects = PNIO_NULL;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::addCnstObject()");

	PND_ASSERT(m_pPndAdapter != PNIO_NULL);

    switch (type)
    {
    case INPUT:
        pCnstObjects = &cnstObjects[READ];
        break;
    case OUTPUT:
        pCnstObjects = &cnstObjects[WRITE];
        break;
    default:
        PND_FATAL("CIOCCProcessImageUpdater::addCnstObject: Unknown consistency object type");
        break;
    }

    pNewCnstObj = (CONSISTENCY_OBJECT*)m_pPndAdapter->memAlloc(sizeof(CONSISTENCY_OBJECT));
    PND_ASSERT(pNewCnstObj != PNIO_NULL);

    pNewCnstObj->pHostBuf = pHostBuf;
    pNewCnstObj->paeaOffset = pa_ea_adr - m_pa_ea_baseAddress;
    pNewCnstObj->length = (PNIO_UINT16)length;

    pCnstObjects->push_back(pNewCnstObj);

	PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::addCnstObject()");
}

PNIO_VOID CIOCCProcessImageUpdater::removeCnstObjects(PNIO_UINT8* pHostBufBase, PNIO_UINT32 bufSize, IO_DATA_TYPE type)
{
    PNIO_UINT32 i = 0;
    PNIO_UINT32 j = 0;
    vector<LINK_LIST*>* pLinkLists = PNIO_NULL;
    vector<EDDI_IOCC_LL_HANDLE_TYPE>* pLinkListHandles = PNIO_NULL;
    LINK_LIST* pLinkList = PNIO_NULL;
    LSA_RESULT lsaResult = EDD_STS_OK;
    PNIO_UINT8* pObjHostBuf = PNIO_NULL;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::removeCnstObject()");

    switch (type)
    {
    case INPUT:
        pLinkLists = &m_linkLists[READ];
        pLinkListHandles = &m_linkListHandles[READ];
        break;
    case OUTPUT:
        pLinkLists = &m_linkLists[WRITE];
        pLinkListHandles = &m_linkListHandles[WRITE];
        break;
    default:
        PND_FATAL("CIOCCProcessImageUpdater::addCnstObject: Unknown consistency object type");
        break;
    }

    for (i = 0; i < pLinkLists->size(); i++)
    {
        pLinkList = (*pLinkLists)[i];

        for (j = 0; j < pLinkList->cnstObjects.size(); )
        {
            pObjHostBuf = pLinkList->cnstObjects[j]->pHostBuf;

            if ( (pObjHostBuf >= pHostBufBase) && (pObjHostBuf < pHostBufBase + bufSize) )
            {                
                if (pLinkListHandles->size() >= i + 1)
                {
					PND_ASSERT(m_pPndAdapter != PNIO_NULL);
                    lsaResult = m_pPndAdapter->eddiIOCCRemoveFromLinkList((*pLinkListHandles)[i], pLinkList->cnstObjects[j]->pHostBuf);
                    PND_ASSERT(lsaResult == EDD_STS_OK);
                }
				PND_ASSERT(m_pPndAdapter != PNIO_NULL);
                m_pPndAdapter->memFree(pLinkList->cnstObjects[j]);
                pLinkList->cnstObjects.erase(pLinkList->cnstObjects.begin() + static_cast<int>(j));
            }
            else
            {
                j++;
            }
        }
    }

	PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::removeCnstObject()");
}

PNIO_UINT32 CIOCCProcessImageUpdater::allocateLinkLists()
{
    PNIO_UINT32 result = PNIO_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::allocateLinkLists()");


    createLinkLists(&m_linkLists[READ], &cnstObjects[READ], EDDI_IOCC_LINKLIST_MODE_READ);
    result = allocateLinkLists(&m_linkLists[READ], &m_linkListHandles[READ]);

    if(result == PNIO_OK)
    {
        createLinkLists(&m_linkLists[WRITE], &cnstObjects[WRITE], EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP);
        result = allocateLinkLists(&m_linkLists[WRITE], &m_linkListHandles[WRITE]);
    }

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::allocateLinkLists()");

    return result;
}

PNIO_BOOL CIOCCProcessImageUpdater::compCnstObjects(const CONSISTENCY_OBJECT* pObj1, const CONSISTENCY_OBJECT* pObj2)
{
    return(pObj1->pHostBuf < pObj2->pHostBuf);
}

PNIO_VOID CIOCCProcessImageUpdater::createLinkLists(vector<LINK_LIST*>* pLinkLists, vector<CONSISTENCY_OBJECT*>* pCnstObjects, PNIO_UINT8 linkListMode)
{    
    PNIO_UINT32 i = 0;
    LINK_LIST* pNewLinkList = PNIO_NULL;
    CONSISTENCY_OBJECT* pCnstObject = PNIO_NULL;
    CONSISTENCY_OBJECT* pLastCnstObject = PNIO_NULL;

	/*lint -e(864) Expression involving variable '*pCnstObjects' possibly depends on order of evaluation*/
    sort(pCnstObjects->begin(), pCnstObjects->end(), compCnstObjects);

    for(i=0; i<pCnstObjects->size(); i++)
    {
        pCnstObject = (*pCnstObjects)[i];

        if ((pLinkLists->empty() == true) || (pLinkLists->back()->length + pCnstObject->length > EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE))
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            pNewLinkList = (LINK_LIST*)m_pPndAdapter->memAlloc(sizeof(LINK_LIST));
            pNewLinkList = new(pNewLinkList)(LINK_LIST);
            pNewLinkList->mode = linkListMode;
            pNewLinkList->length = pCnstObject->length;
            pNewLinkList->pHostBuf = PNIO_NULL;
            pNewLinkList->cnstObjects.push_back(pCnstObject);

            pLinkLists->push_back(pNewLinkList);
            continue;
        }

        pLinkLists->back()->length += (PNIO_UINT16)(pCnstObject->length);

        pLastCnstObject = pLinkLists->back()->cnstObjects.back();
        if(pLastCnstObject->pHostBuf + pLastCnstObject->length == pCnstObject->pHostBuf)
        {
            pLastCnstObject->length += pCnstObject->length;
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            m_pPndAdapter->memFree(pCnstObject);
            pCnstObject = PNIO_NULL;
        }
        else
        {
            pLinkLists->back()->cnstObjects.push_back(pCnstObject);
        }
    }

    pCnstObjects->clear();
}

PNIO_UINT32 CIOCCProcessImageUpdater::allocateLinkLists(vector<LINK_LIST*>* pLinkLists, vector<EDDI_IOCC_LL_HANDLE_TYPE>* linkListHandles)
{
    LSA_RESULT lsaResult = EDD_STS_OK;
    PNIO_UINT32 result = PNIO_OK;    
    LINK_LIST* pLinkList = PNIO_NULL;
    PNIO_UINT8* pMem = PNIO_NULL;
    EDDI_IOCC_LL_HANDLE_TYPE llHandle;
    PNIO_UINT32 i = 0;
    PNIO_UINT32 j = 0;

    for (i = 0; lsaResult == EDD_STS_OK && i < pLinkLists->size(); i++)
    {
        pLinkList = (*pLinkLists)[i];
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
        lsaResult = m_pPndAdapter->eddiIOCCCreateLinkList(m_pIoccHandleMultipleRw,
                                                          &llHandle,
                                                          EDDI_IOCC_LINKLIST_TYPE_INTERNAL,
                                                          pLinkList->mode,
                                                          (PNIO_UINT16)pLinkList->cnstObjects.size());

        if(lsaResult == EDD_STS_OK)
        {
            (*linkListHandles).push_back(llHandle);

            if (pLinkList->mode == EDDI_IOCC_LINKLIST_MODE_READ)
            {
                pLinkList->pHostBuf = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_ioMemPoolId, pLinkList->length);
                PND_ASSERT(pLinkList->pHostBuf != PNIO_NULL);
            }
            else if(pLinkList->mode == EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP)
            {
                pLinkList->pHostBuf = (PNIO_UINT8*)m_pPndAdapter->memAlloc(pLinkList->length);
                PND_ASSERT(pLinkList->pHostBuf != PNIO_NULL);
            }
            else
            {
                pLinkList->pHostBuf = PNIO_NULL;
            }
        }

        
        for (j=0; lsaResult == EDD_STS_OK && j < pLinkList->cnstObjects.size(); j++)
        {
            pMem = (pLinkList->pHostBuf == PNIO_NULL) ? (pLinkList->cnstObjects[j]->pHostBuf) : (pLinkList->pHostBuf);

			PND_ASSERT(m_pPndAdapter != PNIO_NULL);
            lsaResult = m_pPndAdapter->eddiIOCCAppendToLinkList(llHandle,
                                                                pLinkList->cnstObjects[j]->paeaOffset,
                                                                pLinkList->cnstObjects[j]->length,
                                                                pMem,
                                                                pLinkList->cnstObjects[j]->pHostBuf);
        }
    }


    if(lsaResult != EDD_STS_OK)
    {
        PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "EDDI returned error(%u)", lsaResult);
        result = PNIO_ERR_INTERNAL;
    }

    return result;
}

PNIO_VOID CIOCCProcessImageUpdater::freeLinkLists()
{
    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOCCProcessImageUpdater::freeLinkLists()");

    freeLinkLists(&m_linkLists[READ], &m_linkListHandles[READ]);
    freeLinkLists(&m_linkLists[WRITE], &m_linkListHandles[WRITE]);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIOCCProcessImageUpdater::freeLinkLists()");
}

PNIO_VOID CIOCCProcessImageUpdater::freeLinkLists(vector<LINK_LIST*>* pLinkLists, vector<EDDI_IOCC_LL_HANDLE_TYPE>* pLinkListHandles)
{
    LSA_RESULT lsaResult = EDD_STS_OK;
    LINK_LIST* pLinkList = PNIO_NULL;
    EDDI_IOCC_LL_HANDLE_TYPE llHandle = PNIO_NULL;

    while (pLinkLists->empty() == false)
    {
        pLinkList = pLinkLists->back();
        llHandle = pLinkListHandles->back();

        PND_ASSERT(pLinkList->cnstObjects.empty());

        if(pLinkList->pHostBuf != PNIO_NULL)
        {
			PND_ASSERT(m_pPndAdapter != PNIO_NULL);

            if(pLinkList->mode == EDDI_IOCC_LINKLIST_MODE_READ || pLinkList->mode == EDDI_IOCC_LINKLIST_MODE_WRITE_SINGLE)
            {
                lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, pLinkList->pHostBuf);
                PND_ASSERT(lsaResult == LSA_RET_OK);
            }
            else if(pLinkList->mode == EDDI_IOCC_LINKLIST_MODE_WRITE_GROUP)
            {
                m_pPndAdapter->memFree(pLinkList->pHostBuf);
            }
            else
            {
                PND_FATAL("Unknown link list mode");
            }

            pLinkList->pHostBuf = PNIO_NULL;
        }

        pLinkList->~LINK_LIST();
		PND_ASSERT(m_pPndAdapter != PNIO_NULL);
		/*lint -e{449} Pointer variable 'pLinkList' previously deallocated  */
		m_pPndAdapter->memFree(pLinkList);

        lsaResult = m_pPndAdapter->eddiIOCCDeleteLinkList(llHandle);
        PND_ASSERT(lsaResult == EDD_STS_OK);

        pLinkLists->pop_back();
        pLinkListHandles->pop_back();
    }
}

PNIO_VOID CIOCCProcessImageUpdater::memcopyDstAligned(PNIO_UINT8 * const pDst, PNIO_UINT8 * const pSrc, PNIO_UINT32 const DataLength) const
{
    if ((PNIO_UINT32)pSrc & 0x3)
    {
        //src is not aligned :(
        PNIO_UINT32  LenLocal = DataLength;
        PNIO_UINT8 * pDstLocalu8 = pDst; //Always 32bit aligned
        PNIO_UINT8 * pSrcLocalu8 = pSrc;
        while (LenLocal)
        {
			PND_ASSERT(pDstLocalu8 != PNIO_NULL);
            *pDstLocalu8++ = *pSrcLocalu8++;
            LenLocal--;
        }
    }
    else
    {
        //src is aligned
        PNIO_UINT32  LenLocal = DataLength >> 2;
        PNIO_UINT32 * pDstLocalu32 = (PNIO_UINT32 *)((PNIO_VOID *)pDst); //Always 32bit aligned
        PNIO_UINT32 * pSrcLocalu32 = (PNIO_UINT32 *)((PNIO_VOID *)pSrc);

        while (LenLocal)
        {
			PND_ASSERT(pDstLocalu32 != PNIO_NULL);
            *pDstLocalu32++ = *pSrcLocalu32++;
            LenLocal--;
        }

        {
            //Some compilers(e.g. GCC) do not condone casting
            PNIO_UINT8 * pDstLocalu8 = (PNIO_UINT8 *)pDstLocalu32;
            PNIO_UINT8 * pSrcLocalu8 = (PNIO_UINT8 *)pSrcLocalu32;
            switch (DataLength & 0x3)
            {
            case 3:
            {
				PND_ASSERT(pDstLocalu8 != PNIO_NULL);
                *pDstLocalu8++ = *pSrcLocalu8++;
            }
            //lint -fallthrough
            case 2:
            {
				PND_ASSERT(pDstLocalu8 != PNIO_NULL);
                *pDstLocalu8++ = *pSrcLocalu8++;
            }
            //lint -fallthrough
            case 1:
            {
				PND_ASSERT(pDstLocalu8 != PNIO_NULL);
                *pDstLocalu8 = *pSrcLocalu8;
                break;
            }
            default: break;
            }
        }
    }
}

PNIO_VOID CIOCCProcessImageUpdater::copyReadCnstData()
{
    PNIO_UINT32 i = 0;
    PNIO_UINT32 j = 0;
    PNIO_UINT32 byteOffset = 0;
    LINK_LIST* pLL;
    CONSISTENCY_OBJECT* pCnstObj;
    

    for (i = 0; i < m_linkLists[READ].size(); i++)
    {
        pLL = m_linkLists[READ][i];
        byteOffset = 0;

        //uncached memory -> cached memory in 32-bits
        PND_ASSERT(pLL->length <= EDDI_IOCC_MAX_SINGLE_TRANSFER_SIZE);
        memcopyDstAligned(m_pCachedReadBuffer, pLL->pHostBuf, pLL->length);

        for (j = 0; j < pLL->cnstObjects.size(); j++)
        {
            pCnstObj = pLL->cnstObjects[j];
			PND_ASSERT(m_pCachedReadBuffer != PNIO_NULL);
            pnd_memcpy(pCnstObj->pHostBuf, m_pCachedReadBuffer + byteOffset, pCnstObj->length);
            byteOffset += pCnstObj->length;
        }        
    }
}

PNIO_VOID CIOCCProcessImageUpdater::copyWriteCnstData()
{
    PNIO_UINT32 i = 0;
    PNIO_UINT32 j = 0;
    PNIO_UINT32 byteOffset = 0;
    LINK_LIST* pLL;
    CONSISTENCY_OBJECT* pCnstObj;

    for (i = 0; i < m_linkLists[WRITE].size(); i++)
    {
        pLL = m_linkLists[WRITE][i];
        byteOffset = 0;

        for (j = 0; j < pLL->cnstObjects.size(); j++)
        {
            pCnstObj = pLL->cnstObjects[j];

            pnd_memcpy(pLL->pHostBuf + byteOffset, pCnstObj->pHostBuf, pCnstObj->length);
            byteOffset += pCnstObj->length;
        }
    }
}

PNIO_VOID* CIOCCProcessImageUpdater::memSet(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len)
{
    if (dst != 0)
    {
        size_t buffer_len = len;
		PNIO_UINT8* alignedAddr = 0;
		PNIO_UINT8 byteNum = 0;

        PND_ASSERT(m_pIntLinkListMemMultipleRw != LSA_NULL);

		for (buffer_len = 0;  buffer_len < len; buffer_len++)
		{
			alignedAddr = (PNIO_UINT8 *)dst + buffer_len;
			byteNum = static_cast<PNIO_UINT8>((alignedAddr - m_pIntLinkListMemMultipleRw) % 4);
			alignedAddr = alignedAddr - byteNum;

			//write new value to dst	   Read Original val					Set byte value to Zero			Set byte vaue to val
			*(PNIO_UINT32*)alignedAddr = ( *((PNIO_UINT32*)alignedAddr) & (~((0xFFU) << (byteNum * 8))) ) | (val << (byteNum * 8));
		}
    }

    return dst;
}

PNIO_UINT32 CIOCCProcessImageUpdater::calculateLocalToTargetAddressOffset(const PNIO_UINT8* pVirtBaseAddress, PNIO_UINT32 phyBaseAddress)
{
    PNIO_UINT32 offset = 0;
    PNIO_UINT32 ahbAddr = 0;
    PNIO_UINT8* pLocalAddress = PNIO_NULL;
    LSA_UINT16 lsaResult = LSA_RET_OK;

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIODU_EDDI::calculateLocalToTargetAddressOffset()");

	PND_ASSERT(m_pPndAdapter != PNIO_NULL);
    pLocalAddress = (PNIO_UINT8*)m_pPndAdapter->poolAllocMem(m_ioMemPoolId, 256);
    PND_ASSERT(pLocalAddress != PNIO_NULL);

    ahbAddr = (PNIO_UINT32)((PNIO_UINT8*)pLocalAddress - pVirtBaseAddress);
    ahbAddr += phyBaseAddress;

    offset = ahbAddr - (PNIO_UINT32)pLocalAddress;
    PND_IODU_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "CIODU_EDDI::calculateLocalToTargetAddressOffset(): local to asic address offset: 0x%08x", offset);

	PND_ASSERT(m_pPndAdapter != PNIO_NULL);
    lsaResult = m_pPndAdapter->poolFreeMem(m_ioMemPoolId, pLocalAddress);
    PND_ASSERT(lsaResult == LSA_RET_OK);

    PND_IODU_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CIODU_EDDI::calculateLocalToTargetAddressOffset()");

    return offset;
}

IPndAdapter * CIOCCProcessImageUpdater::createPndAdapter()
{
    return new CPndAdapter();
}

void CIOCCProcessImageUpdater::deletePndAdapter()
{
    if(m_pPndAdapter != PNIO_NULL)
    {
        delete m_pPndAdapter;
        m_pPndAdapter = PNIO_NULL;
    }
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

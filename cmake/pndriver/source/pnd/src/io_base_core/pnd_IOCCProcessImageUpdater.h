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
/*  F i l e               &F: pnd_IOCCProcessImageUpdater.h             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Class for managing process image update via IOCC                         */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_IOCCPROCESSIMAGEUPDATER_H
#define PND_IOCCPROCESSIMAGEUPDATER_H

#include "pnd_IIOCCProcessImageUpdater.h"
#include "pnd_IPndAdapter.h"
#include "pniobase.h"
#include <vector>

#define PND_IOCC_PROCESS_IMAGE_UPDATER_GET_LL_VALUE(pa_ea_address, dataLength)                       ( ( ( pa_ea_address ) << 14 ) + ( ( dataLength ) << 3 ) )
#define PND_IOCC_PROCESS_IMAGE_UPDATER_CALC_REAL_PAEA_RAM_ADDRESS(baseVirtAddress, inVirtAddress)    ( ( baseVirtAddress ) + ( ( ( inVirtAddress ) - ( baseVirtAddress ) ) * 4 ) )

using namespace std;

class CIOCCProcessImageUpdater : public IIOCCProcessImageUpdater
{   
public:
    CIOCCProcessImageUpdater();
    virtual ~CIOCCProcessImageUpdater();

    PNIO_UINT32 init(INIT_PARAMS& _initParams);
    PNIO_VOID addCnstObject(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type);
    PNIO_VOID removeCnstObjects(PNIO_UINT8* pHostBufBase, PNIO_UINT32 bufSize, IO_DATA_TYPE type);
    PNIO_UINT32 transferSingleBuffer(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type);
    PNIO_UINT32 transferCnstObjects(IO_DATA_TYPE type);
    PNIO_UINT32 allocateLinkLists();
    PNIO_VOID freeLinkLists();
    
protected:    
    virtual IPndAdapter* createPndAdapter();
    PNIO_VOID deletePndAdapter();

private:        
    typedef struct pnd_consistency_object_tag
    {
        PNIO_UINT8* pHostBuf;
        PNIO_UINT32 paeaOffset;
        PNIO_UINT16 length;
    }CONSISTENCY_OBJECT;

    typedef struct pnd_link_list_object_tag
    {
        PNIO_UINT8 mode;
        PNIO_UINT16 length;
        PNIO_UINT8* pHostBuf;
        vector<CONSISTENCY_OBJECT*> cnstObjects;
    }LINK_LIST;

    typedef union pnd_iocc_direct_access_type_tag
    {
      PNIO_UINT32  as_u32;
      PNIO_UINT16  as_u16[2];
      PNIO_UINT8   as_u8[4];
    }PND_IOCC_DIRECT_ACCESS_TYPE;


    PND_EDDI_IOCC_UPPER_HANDLE m_ioccUpperHandle;
    PNIO_BOOL m_initialized;
    INIT_PARAMS m_initParams;
    IPndAdapter* m_pPndAdapter; 
    PNIO_UINT32 m_pa_ea_baseAddress;
    PNIO_UINT8* m_pMbxSingleRw;
    PNIO_UINT8* m_pMbxMultipleRw;
    PNIO_UINT16 m_ioMemPoolId;
    PNIO_UINT32 m_ioccLocalToTargetAddressOffset;
    PNIO_UINT8* m_pCachedReadBuffer;
    
    EDDI_IOCC_LOWER_HANDLE_TYPE m_pIoccHandleSingleRw;
    EDDI_IOCC_ERROR_TYPE m_ioccErrorSingleRw;
    PNIO_UINT8* m_pIntLinkListMemSingleRw;
    PNIO_UINT32 m_intLinkListMemSizeSingleRw;
    PNIO_UINT8* m_pHostBufferSingleRw;

    EDDI_IOCC_LOWER_HANDLE_TYPE m_pIoccHandleMultipleRw;
    EDDI_IOCC_ERROR_TYPE m_ioccErrorMultipleRw;    
    PNIO_UINT32 m_pIntLinkListMemSizeMultipleRw;
    EDDI_IOCC_LOWER_HANDLE_TYPE m_pLLHandleMultipleWrite;
    EDDI_IOCC_LOWER_HANDLE_TYPE m_pLLHandleMultipleRead;

    PNIO_VOID directRead(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length) const;
    PNIO_VOID directWrite(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length) const;
    PNIO_UINT32 allocateLinkLists(vector<LINK_LIST*>* pLinkLists, vector<EDDI_IOCC_LL_HANDLE_TYPE>* pLinkListHandles);
    PNIO_VOID freeLinkLists(vector<LINK_LIST*>* pLinkLists, vector<EDDI_IOCC_LL_HANDLE_TYPE>* pLinkListHandles);
    PNIO_UINT32 calculateLocalToTargetAddressOffset(const PNIO_UINT8* pVirtBaseAddress, PNIO_UINT32 phyBaseAddress);
    PNIO_VOID memcopyDstAligned(PNIO_UINT8 * const pDst, PNIO_UINT8 * const pSrc, PNIO_UINT32 const dataLength) const;
    PNIO_VOID copyWriteCnstData();
    PNIO_VOID copyReadCnstData();
    PNIO_VOID createLinkLists(vector<LINK_LIST*>* pLinkLists, vector<CONSISTENCY_OBJECT*>* pCnstObjects, PNIO_UINT8 linkListMode);

    static const PNIO_UINT8 IOCC_MB_DATA_SIZE = 4;
    static const PNIO_UINT32 DUMMY_EXT_LINK_LIST_BASE_PHY_ADDR = 1;
    static const PNIO_UINT8 IOCC_CHANNEL_SINGLE_RW = 1;
    static const PNIO_UINT8 IOCC_CHANNEL_MULTIPLE_RW = 2;
    static LSA_UINT8* const m_pDummyExtLinkListBase;
    static PNIO_UINT8* m_pIntLinkListMemMultipleRw;

    vector<LINK_LIST*> m_linkLists[LL_TYPE_MAX];
    vector<CONSISTENCY_OBJECT*> cnstObjects[LL_TYPE_MAX];
    vector<EDDI_IOCC_LL_HANDLE_TYPE> m_linkListHandles[LL_TYPE_MAX];

    static PNIO_VOID* memSet(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len);
    static PNIO_BOOL compCnstObjects(const CONSISTENCY_OBJECT* pObj1, const CONSISTENCY_OBJECT* pObj2);
};

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

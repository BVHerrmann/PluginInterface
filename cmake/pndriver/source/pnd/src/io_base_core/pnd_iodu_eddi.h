#ifndef PND_IODU_EDDI_H
#define PND_IODU_EDDI_H

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
/*  F i l e               &F: pnd_iodu_eddi.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Child class for IODU for SOC1 (EDDI)                                  */
/*****************************************************************************/

#include "pnd_int.h"
#include "pnd_iodu.h"
#include "pnd_IIODU_EDDI.h"
#include "pnd_pnstack.h"
#include "pnd_IIOCCProcessImageUpdater.h"
#include "eddi_sys.h"

class IEDDIPiMemManager;

class CIODU_EDDI : public CIODU, public IIODU_EDDI
{    
public:
    typedef struct
    {
        PNIO_UINT8*  pIoMemBase;
        PNIO_UINT32  ioMemPhyAddr;
        PNIO_UINT32  ioMemSize;
        PNIO_UINT8*  pIoccBase;
        PNIO_UINT32  ioccPhyAddr;
    }INIT_PARAMS;

    CIODU_EDDI(PND_HANDLE_TYPE pndHandle, IPndAdapter *adapter);
    virtual ~CIODU_EDDI();

    // implementation of abstract class methodes
    virtual PNIO_VOID PI_Alloc(PND_RQB_PTR_TYPE  pRqb);
    virtual PNIO_VOID PI_Free(PND_RQB_PTR_TYPE  pRqb);
    
    PNIO_VOID initEddi(INIT_PARAMS* params);
    PNIO_UINT32 updateIsoInputs();
    PNIO_UINT32 updateIsoOutputs();
    PNIO_VOID allocateLinkLists(PND_RQB_PTR_TYPE pRqb);
    PNIO_VOID freeLinkLists(PND_RQB_PTR_TYPE pRqb);

protected:
    virtual IEDDIPiMemManager* createPiMemManager();
    virtual IIOCCProcessImageUpdater* createProcessImageUpdater();

private:
    typedef enum
    {
        PROVIDER,
        CONSUMER
    } BUFFER_TYPE;

    IEDDIPiMemManager *m_pPiMemManager;
    IIOCCProcessImageUpdater* m_pProcessImageUpdater; 
    PNIO_UINT32 m_sharedMem[(sizeof(EDDI_GSHAREDMEM_TYPE) / 4) + 1];    

    virtual PNIO_VOID update_provider_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODUBufferHandle);
    virtual PNIO_VOID update_consumer_shadow_buffer(PND_IODU_BUFFER_PTR_TYPE pIODUBufferHandle);

    PNIO_UINT32 calculateLocalToTargetAddressOffset(PNIO_UINT8* pVirtBaseAddress, PNIO_UINT32 phyBaseAddress);
    PNIO_UINT32 findFreeSlot(BUFFER_TYPE bt, PNIO_UINT32 *pIndex) const;
    PNIO_VOID removeUsedSlot(const PND_IODU_BUFFER_PTR_TYPE pIODUBuffer);
    PNIO_VOID deleteProcessImageUpdater();
    PNIO_VOID deletePiMemManager();
    PNIO_VOID addIsoCnstObjects(PND_RQB_PTR_TYPE pRqb);
    PNIO_VOID deleteIsoCnstObjects(PND_RQB_PTR_TYPE pRqb);
};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_IODU_EDDI_H */
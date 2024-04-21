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
/*  F i l e               &F: pnd_IIOCCProcessImageUpdater.h            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Interface for process image update via IOCC                              */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_IIOCCPROCESSIMAGEUPDATER_H
#define PND_IIOCCPROCESSIMAGEUPDATER_H

#include "pnd_iodu.h"
#include "pniobase.h"

class IIOCCProcessImageUpdater
{
public:
    typedef struct init_params_tag
    {
        PNIO_UINT32     ioccBaseAddress;
        PNIO_UINT32     ioccAhbAddress;
        PNIO_UINT32     ioMemSize;        
        PNIO_UINT32     hdNr;
        PNIO_UINT8*     pIoMemBase;
        PNIO_UINT32     ioMemPhyAddr;
    }INIT_PARAMS;

	typedef enum consistency_object_type_tag
	{
		INPUT   = 0,
		OUTPUT  = 1
	}IO_DATA_TYPE;

    typedef enum consistency_link_list_type_tag
    {
        READ            = 0,
        WRITE           = 1,
        LL_TYPE_MAX     = 2
    }LINK_LIST_TYPE;

    virtual ~IIOCCProcessImageUpdater() {}
    virtual PNIO_UINT32 init(INIT_PARAMS& _initParams) = 0;
	virtual PNIO_VOID addCnstObject(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type) = 0;
    virtual PNIO_VOID removeCnstObjects(PNIO_UINT8* pHostBufBase, PNIO_UINT32 bufSize, IO_DATA_TYPE type) = 0;
    virtual PNIO_UINT32 transferSingleBuffer(PNIO_UINT8* pHostBuf, PNIO_UINT32 pa_ea_adr, PNIO_UINT32 length, IO_DATA_TYPE type) = 0;
    virtual PNIO_UINT32 transferCnstObjects(IO_DATA_TYPE type) = 0;
    virtual PNIO_UINT32 allocateLinkLists() = 0;
    virtual PNIO_VOID freeLinkLists() = 0;
};

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
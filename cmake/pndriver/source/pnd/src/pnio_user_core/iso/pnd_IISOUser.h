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
/*  F i l e               &F: pnd_IISOUser.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface for isochronous mode operations                                */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_IISOUSER_H
#define PND_IISOUSER_H

#include "pnd_UserNode.h"

class IISOUser : public CUserNode
{
public:
    virtual ~IISOUser() {}
    IISOUser() : CUserNode() {}
    IISOUser(PND_HANDLE_PTR_TYPE pHandle, CUserNode* pParent) : CUserNode( pHandle, pParent ) {}

    virtual PNIO_VOID Create(PND_PARAM_ISO_HD_IF_PTR_TYPE params) = 0;
    virtual PNIO_VOID Startup(PNIO_VOID* pRqb) = 0;
    virtual PNIO_UINT32 setOpDone(PNIO_CYCLE_INFO* pCycleInfo) = 0;
};

#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
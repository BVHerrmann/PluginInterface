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
/*  F i l e               &F: pnd_IIODU_EDDI.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface for io data update via EDDI                                    */
/*                                                                           */
/*****************************************************************************/
#ifndef PND_IIODU_EDDI_H
#define PND_IIODU_EDDI_H

#include "pnd_int.h"

class IIODU_EDDI
{
public:
    virtual ~IIODU_EDDI() {}
    virtual PNIO_UINT32 updateIsoInputs() = 0;
    virtual PNIO_UINT32 updateIsoOutputs() = 0;
    virtual PNIO_VOID allocateLinkLists(PND_RQB_PTR_TYPE pRqb) = 0;
    virtual PNIO_VOID freeLinkLists(PND_RQB_PTR_TYPE pRqb) = 0;
};

#endif
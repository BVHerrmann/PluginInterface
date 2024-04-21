#ifndef PND_IEDDIPIMEMMANAGER_H
#define PND_IEDDIPIMEMMANAGER_H

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
/*  F i l e               &F: pnd_IEDDIPiMemManager.h                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*****************************************************************************/

#include "pnd_int.h"
#include "pnd_iodu.h"

class IEDDIPiMemManager
{
public:
    virtual ~IEDDIPiMemManager() {}
    virtual PNIO_UINT32 piAlloc(PND_BUFFER_PROPERTIES_PTR_TYPE pBuffer, PND_IODU_BUFFER_PTR_TYPE *ppBufferArrayEntry, PNIO_BOOL *pIsBufferNeeded) = 0;
    virtual PNIO_UINT32 piFree(PND_IODU_BUFFER_PTR_TYPE pBuffer) = 0;
    virtual LSA_RESPONSE_TYPE createPIMemPool(PNIO_UINT32 ioccVirtualAddr) = 0;
    virtual LSA_RESPONSE_TYPE deletePIMemPool() = 0;
};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_IEDDIPIMEMMANAGER_H */

#ifndef PND_PIPOOL_H
#define PND_PIPOOL_H

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
/*  F i l e               &F: pnd_PIPool.h				                :F&  */
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
#include "pnd_IPIPool.h"

#include "pnd_iodu.h"

class CPIPool : public IPIPool
{
public:
	virtual ~CPIPool();
	PNIO_VOID Create(PNIO_UINT8* basePtr);
	PNIO_VOID paea_mem_set(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len);
	PNIO_UINT8* getPoolBasePtr();

	static PNIO_VOID* paeaMemSet(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len);

private:
	static PNIO_UINT8* m_pPoolBasePtr;

};
/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of PND_PIPOOL_H */

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
/*  F i l e               &F: pnd_PIPool.cpp			                :F&  */
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

#define LTRC_ACT_MODUL_ID   1205
#define PND_MODULE_ID       1205

#include "pnd_PIPool.h"

PNIO_UINT8* CPIPool::m_pPoolBasePtr = PNIO_NULL;

CPIPool::~CPIPool()
{
	m_pPoolBasePtr = PNIO_NULL;
}

PNIO_VOID CPIPool::paea_mem_set(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len)
{
	paeaMemSet(dst, val, len);
}

PNIO_VOID CPIPool::Create(PNIO_UINT8* basePtr)
{
	m_pPoolBasePtr = basePtr;
}

PNIO_UINT8* CPIPool::getPoolBasePtr()
{
	return m_pPoolBasePtr;
}

PNIO_VOID* CPIPool::paeaMemSet(PNIO_VOID_PTR_TYPE dst, LSA_INT val, size_t len)
{
	if (dst != 0) // buffer allocated ?
	{
		size_t                         buffer_len = len;
		PNIO_UINT8 *                   buffer_ptr;

		EPS_ASSERT(m_pPoolBasePtr != LSA_NULL);

		// calc direct access addr (offset in steps of 4)
		buffer_ptr = (((PNIO_UINT32)((PNIO_UINT8 *)dst - m_pPoolBasePtr)) << 2U/* * 4 */) + m_pPoolBasePtr;

		// Note: Setting is done over direct access address
		for (; buffer_len != 0; buffer_len--)
		{
			*buffer_ptr = (PNIO_UINT8)val;
			buffer_ptr += 4;    //direct access mode: PAEA ram is accessed in steps of 4
		}
	}

	return dst;
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

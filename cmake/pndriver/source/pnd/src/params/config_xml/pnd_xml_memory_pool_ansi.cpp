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
/*  F i l e               &F: pnd_xml_memory_pool_ansi.cpp              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Methods for C linkage of memory pool                                     */ 
/*                                                                           */
/*****************************************************************************/
#include "pnd_xml_memory_pool_ansi.h"
#include "pnd_xml_memory_pool.h"
#include "pnd_sys.h"

#ifdef  __cplusplus
  extern "C" {
#endif 

static CPndMemoryPool* pool = PNIO_NULL;

PNIO_VOID_PTR_TYPE pnd_allocateFromPool(PNIO_UINT32 size)
{
    if(pool == PNIO_NULL)
    {
        pool = new CPndMemoryPool(pnd_mem_alloc, pnd_mem_free);
    }

    return pool->allocateFromPool(size);
}

PNIO_VOID pnd_freePool()
{
    if(pool != PNIO_NULL)
    {
        delete pool;
        pool = PNIO_NULL;
    }
}

#ifdef  __cplusplus
}
#endif
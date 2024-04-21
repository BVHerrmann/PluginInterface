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
/*  F i l e               &F: pnd_xml_memory_pool.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Class decleration for memory pool                                        */ 
/*                                                                           */
/*****************************************************************************/
#ifndef PND_XML_MEMORY_POOL_H
#define PND_XML_MEMORY_POOL_H

#include "pniobase.h"

class CPndMemoryPool
{
public:
    CPndMemoryPool(PNIO_VOID_PTR_TYPE (*pAllocate)(PNIO_UINT32)=PNIO_NULL, PNIO_VOID (*pFree)(PNIO_VOID_PTR_TYPE)=PNIO_NULL);
    virtual ~CPndMemoryPool();       
    PNIO_VOID_PTR_TYPE allocateFromPool(PNIO_UINT32 size);    
    PNIO_VOID freeAll();

private:
    static const PNIO_UINT32 BLOCK_SIZE = 65536;

    PNIO_INT8* m_pAvailableMem;
    PNIO_INT8* m_pCurrentBlock;
    PNIO_VOID_PTR_TYPE (*m_pUserAllocMethod)(PNIO_UINT32);
    PNIO_VOID (*m_pUserFreeMethod)(PNIO_VOID_PTR_TYPE);
    PNIO_INT8* allocateNewBlock(PNIO_UINT32 size) const;    
};

#endif
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_pool.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  15.11.07    JS    added EDDI_MEMCheckPoolFreeBuffer                      */
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"

#include "eddi_pool.h"

#define POOL_BUF_FREE   (LSA_UINT32)0x1
#define POOL_BUF_USED   (LSA_UINT32)0x2

//#define  EDDI_POOL_CHECK_ALWAYS  //for debugging
#if defined (EDDI_POOL_CHECK_ALWAYS)
#define EDDI_POOL_EXT_CHECKING
static void  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheck_pool( const LSA_UINT32    PoolHandle );
#else
#define EDDI_MemCheck_pool(PoolHandle)
#endif

typedef struct POOL_BUF_S
{
    LSA_UINT32        BufState;

    LSA_UINT32     *  pBegin;

    #if defined (EDDI_POOL_EXT_CHECKING)
    LSA_UINT32     *  pEnd;
    #endif

} POOL_BUF_T;

typedef struct POOL_STACK_S
{
    LSA_UINT8      *  p;
    LSA_UINT32        PhysAllocIndex;

} POOL_STACK_T;

//temporarily_disabled_lint -esym(754, POOL_VERW_H_S::tmp)

typedef struct POOL_VERW_H_S
{
    LSA_UINT32                    CheckPattern;
    LSA_BOOL                      do_preset_buffer;
    LSA_UINT8                     preset_value;
    LSA_UINT8                     tmp[3];
    LSA_UINT32                    BufferSize;
    LSA_UINT32                    BufferSize_in_dw;
    LSA_UINT32                    MaxBuffer;
    EDDI_ALLOC_LOCAL_MEM_FCT      alloc_local_mem_fct;
    EDDI_FREE_LOCAL_MEM_FCT       free_local_mem_fct;
    EDDI_LOCAL_DDB_PTR_TYPE       pDDB;
    LSA_INT32                     alloc_index;
    LSA_UINT8                  *  pPoolBase;
    LSA_UINT32                    dwPoolBase;
    EDDI_POOL_TYPE_T              PoolType;
    LSA_UINT32                    PoolHandle;
    EDDI_MEM_BUF_EL_H          *  pMemHeader;

    POOL_STACK_T               *  pStack;

    POOL_BUF_T                    buf[1];  // Structure for validation purposes-> last element!

} POOL_VERW_H_T;

#define EDDI_MODULE_ID     M_ID_EDDI_POOL
#define LTRC_ACT_MODUL_ID  25

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/***************************************************************************/
/* F u n c t i o n:       EDDI_MemIniPool()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MemIniPool( const  LSA_UINT32                   MaxBuffer,
                                                  const  LSA_UINT32                   BufferSize,
                                                  const  EDDI_POOL_TYPE_T             PoolType,
                                                  EDDI_ALLOC_LOCAL_MEM_FCT     const  alloc_local_mem_fct,
                                                  EDDI_FREE_LOCAL_MEM_FCT      const  free_local_mem_fct,
                                                  const  LSA_BOOL                     do_preset_buffer,
                                                  const  LSA_UINT8                    preset_value,
                                                  EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                  LSA_UINT32                *  const  pPoolHandle )
{
    LSA_UINT32        i, SizeVerw, SizePool, ret, MaxBufferIndex;
    LSA_UINT16        tmp;
    POOL_VERW_H_T  *  pPoolVerw;
    LSA_UINT8      *  pTmp;
    POOL_BUF_T     *  pBuf;
    POOL_STACK_T   *  pStack;

    if ((MaxBuffer == 0) || (BufferSize == 0))
    {
        EDDI_Excp("EDDI_MemIniPool, Invalid Inputs", EDDI_FATAL_ERR_EXCP, MaxBuffer, BufferSize);
        return 0;
    }

    switch (PoolType)
    {
        case EDDI_POOL_TYPE_KRAM_8BA:  //KRAM-Buffer with 8-Byte-Alignment
        {
            if (BufferSize & 0x7)
            {
                EDDI_Excp("EDDI_MemIniPool, Wrong 8-Byte-Alignment", EDDI_FATAL_ERR_EXCP, BufferSize, 0);
                return 0;
            }
            break;
        }
        case EDDI_POOL_TYPE_KRAM_4BA:  //KRAM-Buffer with 4-Byte-Alignment
        {
            if (BufferSize & 0x3)
            {
                EDDI_Excp("EDDI_MemIniPool, Wrong 4-Byte-Alignment", EDDI_FATAL_ERR_EXCP, BufferSize, 0);
                return 0;
            }
            break;
        }
        case EDDI_POOL_TYPE_LOCAL_MEM: //satisfy lint!
        {
            break;
        }
    }

    SizeVerw = sizeof(POOL_VERW_H_T) + (MaxBuffer * (sizeof(POOL_BUF_T) + sizeof(POOL_STACK_T)));

    alloc_local_mem_fct((void **)&pPoolVerw, SizeVerw);

    if (pPoolVerw == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_MemIniPool pPoolVerw == 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    pPoolVerw->PoolHandle          = (LSA_UINT32)pPoolVerw;
    pPoolVerw->CheckPattern        = EDDI_POOL_CHECK_PAT_DISABLED;
    pPoolVerw->CheckPattern        = EDDI_POOL_CHECK_PAT;
    pPoolVerw->alloc_local_mem_fct = alloc_local_mem_fct;
    pPoolVerw->free_local_mem_fct  = free_local_mem_fct;
    pPoolVerw->pDDB                = pDDB;
    pPoolVerw->do_preset_buffer    = do_preset_buffer;
    pPoolVerw->preset_value        = preset_value;
    pPoolVerw->BufferSize          = BufferSize;
    pPoolVerw->BufferSize_in_dw    = BufferSize / 4;
    pPoolVerw->MaxBuffer           = MaxBuffer;
    pPoolVerw->PoolType            = PoolType;

    //pPoolVerw->tmp[0]  = 0;
    //pPoolVerw->tmp[1]  = 0;
    //pPoolVerw->tmp[2]  = 0;

    SizePool = BufferSize * MaxBuffer;

    switch (PoolType)
    {
        case EDDI_POOL_TYPE_KRAM_8BA:
        case EDDI_POOL_TYPE_KRAM_4BA:
        {
            ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pPoolVerw->pMemHeader, SizePool);

            if (ret != EDDI_MEM_RET_OK)
            {
                pPoolVerw->free_local_mem_fct(&tmp, (void *)pPoolVerw);
                EDDI_Excp("EDDI_MemIniPool, KRAM-Alloc-Fehler", EDDI_FATAL_ERR_EXCP, ret, 0);
                return 0;
            }

            pPoolVerw->pPoolBase = pPoolVerw->pMemHeader->pKRam;
            break;
        }

        case EDDI_POOL_TYPE_LOCAL_MEM:
        {
            alloc_local_mem_fct((void **)&pPoolVerw->pPoolBase, SizePool);

            if (pPoolVerw->pPoolBase == EDDI_NULL_PTR)
            {
                pPoolVerw->free_local_mem_fct(&tmp, (void *)pPoolVerw);
                EDDI_Excp("EDDI_MemIniPool, Local-Mem-Alloc-Fehler", EDDI_FATAL_ERR_EXCP, 0, 0);
                return 0;
            }
            break;
        }

        default:
        {
            EDDI_Excp("EDDI_MemIniPool, Invalid PoolType", EDDI_FATAL_ERR_EXCP, PoolType, 0);
            return 0;
        }
    }

    pPoolVerw->dwPoolBase = (LSA_UINT32)(void *)pPoolVerw->pPoolBase;

    MaxBufferIndex = MaxBuffer - 1;

    {
        pBuf =  pPoolVerw->buf + MaxBufferIndex;    //lint !e416   BV 27/01/2016  // points to end of last buffer
        //  pBuf =  pPoolVerw->buf;
        //  pPoolVerw->pStack = (POOL_STACK_T *)(void *)(pBuf + MaxBuffer);
        pPoolVerw->pStack = (POOL_STACK_T *)(void *)(&pPoolVerw->buf[0] + MaxBuffer);   //lint !e416        BV 27/01/2016
    }

    pStack = pPoolVerw->pStack;

    for (i = 0; i <= MaxBufferIndex; i++)
    {
        //temporarily_disabled_lint --e{661}
        pStack->PhysAllocIndex = MaxBufferIndex - i;
        //    pStack->PhysAllocIndex = i;

        pTmp           = pPoolVerw->pPoolBase + ( pStack->PhysAllocIndex * BufferSize);
        pBuf->BufState = POOL_BUF_FREE;                 //lint !e661    BV 27/01/2016

        pBuf->pBegin   = (LSA_UINT32 *)(void *)pTmp;    //lint !e661    BV 27/01/2016
        *pBuf->pBegin  = EDDI_POOL_CHECK_PAT;           //lint !e661    BV 27/01/2016

        #if defined (EDDI_POOL_EXT_CHECKING)
        pBuf->pEnd     = pBuf->pBegin + (pPoolVerw->BufferSize_in_dw - 1);  //lint !e661    BV 27/01/2016
        *pBuf->pEnd    = EDDI_POOL_CHECK_PAT;                               //lint !e661    BV 27/01/2016
        #endif
        pStack->p = pTmp;

        pBuf--;
        pStack++;
    }

    pPoolVerw->alloc_index  = (LSA_INT32)pPoolVerw->MaxBuffer - 1;
    pPoolVerw->CheckPattern = EDDI_POOL_CHECK_PAT;

    *pPoolHandle = pPoolVerw->PoolHandle;

    return SizePool;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_MemCheckPoolFreeBuffer()                    */
/*                                                                         */
/* D e s c r i p t i o n: Check if a free buffer is available within       */
/*                        pool.                                            */
/*                                                                         */
/* A r g u m e n t s:     PoolHandle :valid PoolHandle                     */
/*                                                                         */
/* Return Value:          LSA_TRUE: Buffer available                       */
/*                        LSA_FALSE:No Buffer available                    */
/*                                                                         */
/***************************************************************************/
LSA_BOOL  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheckPoolFreeBuffer( const  LSA_UINT32  PoolHandle) //IN

{
    POOL_VERW_H_T  *  pPoolVerw;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return LSA_FALSE;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern , 0);
        return LSA_FALSE;
    }

    if (pPoolVerw->alloc_index < 0) /* no more buffers available ? */
    {
        return LSA_FALSE;
    }

    return LSA_TRUE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
void  EDDI_LOCAL_FCT_ATTR  EDDI_MemGetPoolBuffer( const  LSA_UINT32  PoolHandle, //IN
                                                  void   *  *        p )   //OUT
{
    POOL_VERW_H_T  *  pPoolVerw;
    POOL_BUF_T     *  pBuf;
    POOL_STACK_T   *  pStack;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 1", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern , 0);
        return;
    }

    *p = (void *)0;

    if (pPoolVerw->alloc_index < 0)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 2 alloc_index", EDDI_FATAL_ERR_EXCP, PoolHandle, pPoolVerw->MaxBuffer);
        return;
    }

    pStack = &pPoolVerw->pStack[pPoolVerw->alloc_index];
    pPoolVerw->alloc_index--;

    pBuf = &pPoolVerw->buf[pStack->PhysAllocIndex];

    #if defined (EDDI_POOL_EXT_CHECKING)
    if ((*pBuf->pBegin != EDDI_POOL_CHECK_PAT) || (*pBuf->pEnd != EDDI_POOL_CHECK_PAT))
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 3", EDDI_FATAL_ERR_EXCP, pPoolVerw->alloc_index, *pBuf->pEnd);
        return;
    }
    #else
    if (*pBuf->pBegin != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer *pBuf->pBegin != EDDI_POOL_CHECK_PAT", EDDI_FATAL_ERR_EXCP, pPoolVerw->alloc_index, *pBuf->pBegin);
        return;
    }
    #endif

    pBuf->BufState = POOL_BUF_USED;

    *p = pStack->p;
    if (pPoolVerw->do_preset_buffer)
    {
        EDDI_MemSet(pStack->p, pPoolVerw->preset_value, pPoolVerw->BufferSize);
    }

    EDDI_MemCheck_pool(PoolHandle);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
void  EDDI_LOCAL_FCT_ATTR  EDDI_MemClosePool( const  LSA_UINT32  PoolHandle )  //IN
{
    POOL_VERW_H_T  *  pPoolVerw;
    LSA_UINT16        ret, ret0;
    LSA_UINT32        dwRet;

    if (PoolHandle == 0)
    {
        return;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemClosePool 2", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern, 0);
        return;
    }

    pPoolVerw->CheckPattern = EDDI_POOL_CHECK_PAT_DISABLED;

    dwRet = EDDI_MEM_RET_OK;

    switch (pPoolVerw->PoolType)
    {
        case EDDI_POOL_TYPE_KRAM_8BA:
        case EDDI_POOL_TYPE_KRAM_4BA:
        {
            dwRet = EDDI_MEMFreeBuffer(pPoolVerw->pMemHeader);
            break;
        }

        case EDDI_POOL_TYPE_LOCAL_MEM:
        {
            pPoolVerw->free_local_mem_fct(&ret0, pPoolVerw->pPoolBase);
            if (ret0 != LSA_RET_OK)
            {
                EDDI_Excp("EDDI_MemClosePool EDDI_POOL_TYPE_LOCAL_MEM", EDDI_FATAL_ERR_EXCP, ret0, 0);
                return;
            }

            break;
        }

        default:
        {
            EDDI_Excp("EDDI_MemClosePool", EDDI_FATAL_ERR_EXCP, pPoolVerw->PoolType, 0);
            return;
        }
    }

    pPoolVerw->free_local_mem_fct(&ret, pPoolVerw);

    if ((ret != EDD_STS_OK) || (dwRet != EDDI_MEM_RET_OK))
    {
        EDDI_Excp("EDDI_MemClosePool 3", EDDI_FATAL_ERR_EXCP, ret, dwRet);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
void  EDDI_LOCAL_FCT_ATTR  EDDI_MemFreePoolBuffer( const  LSA_UINT32            PoolHandle,  //IN
                                                   void               *  const  p )          //IN
{
    POOL_VERW_H_T  *  pPoolVerw;
    LSA_UINT32        PhysAllocIndex, i;
    LSA_UINT32        dwBuf;
    POOL_BUF_T     *  pBuf;
    POOL_STACK_T   *  pStack;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemFreePoolBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    dwBuf = (LSA_UINT32)p;

    switch (pPoolVerw->PoolType)
    {
        case EDDI_POOL_TYPE_KRAM_8BA:  //KRAM-Buffer with 8-Byte-Alignment
        {
            if (dwBuf & 0x7)
            {
                EDDI_Excp("EDDI_MemFreePoolBuffer, Wrong 8-Byte-Alignment", EDDI_FATAL_ERR_EXCP, pPoolVerw->PoolType, dwBuf);
                return;
            }
            break;
        }
        case EDDI_POOL_TYPE_KRAM_4BA:  //KRAM-Buffer with 4-Byte-Alignment
        {
            if (dwBuf & 0x3)
            {
                EDDI_Excp("EDDI_MemFreePoolBuffer, Wrong 4-Byte-Alignment", EDDI_FATAL_ERR_EXCP, pPoolVerw->PoolType, dwBuf);
                return;
            }
            break;
        }
        case EDDI_POOL_TYPE_LOCAL_MEM: //satisfy lint!
        {
            break;
        }
    }

    if (dwBuf < pPoolVerw->dwPoolBase)
    {
        EDDI_Excp("EDDI_MemFreePoolBuffer - wrong Pool", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    PhysAllocIndex = (dwBuf - pPoolVerw->dwPoolBase) / pPoolVerw->BufferSize;

    if (PhysAllocIndex >= pPoolVerw->MaxBuffer)
    {
        EDDI_Excp("EDDI_MemFreePoolBuffer 3", EDDI_FATAL_ERR_EXCP, PhysAllocIndex, 0);
        return;
    }

    pBuf = &pPoolVerw->buf[PhysAllocIndex];
    if (pBuf->BufState != POOL_BUF_USED)
    {
        EDDI_Excp("EDDI_MemFreePoolBuffer 4", EDDI_FATAL_ERR_EXCP, PhysAllocIndex, pBuf->BufState);
        return;
    }

    pBuf->BufState = POOL_BUF_FREE;

    for (i = 0; i < pPoolVerw->BufferSize_in_dw; i++)
    {
        *(pBuf->pBegin + i) = EDDI_POOL_CHECK_PAT;
    }

    pPoolVerw->alloc_index++;
    pStack = &pPoolVerw->pStack[pPoolVerw->alloc_index];

    pStack->p              = (LSA_UINT8 *)p;
    pStack->PhysAllocIndex = PhysAllocIndex;

    EDDI_MemCheck_pool(PoolHandle);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
void  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheckPoolBuffer( const LSA_UINT32    PoolHandle,   //IN
                                                    void                * const p         )  //IN
{
    POOL_VERW_H_T  *  pPoolVerw;
    LSA_UINT32        PhysAllocIndex;
    LSA_UINT32        dwBuf;
    POOL_BUF_T     *  pBuf;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    dwBuf = (LSA_UINT32)p;

    switch (pPoolVerw->PoolType)
    {
        case EDDI_POOL_TYPE_KRAM_8BA:  //KRAM-Buffer with 8-Byte-Alignment
        {
            if (dwBuf & 0x7)
            {
                EDDI_Excp("EDDI_MemCheckPoolBuffer, Wrong 8-Byte-Alignment", EDDI_FATAL_ERR_EXCP, pPoolVerw->PoolType, dwBuf);
                return;
            }
            break;
        }
        case EDDI_POOL_TYPE_KRAM_4BA:  //KRAM-Buffer with 4-Byte-Alignment
        {
            if (dwBuf & 0x3)
            {
                EDDI_Excp("EDDI_MemCheckPoolBuffer, Wrong 4-Byte-Alignment", EDDI_FATAL_ERR_EXCP, pPoolVerw->PoolType, dwBuf);
                return;
            }
            break;
        }
        case EDDI_POOL_TYPE_LOCAL_MEM: //satisfy lint!
        {
            break;
        }
    }

    if (dwBuf < pPoolVerw->dwPoolBase)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer - wrong Pool", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    PhysAllocIndex = (dwBuf - pPoolVerw->dwPoolBase ) / pPoolVerw->BufferSize;

    if (PhysAllocIndex >= pPoolVerw->MaxBuffer)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer 3", EDDI_FATAL_ERR_EXCP, PhysAllocIndex, 0);
        return;
    }

    pBuf = &pPoolVerw->buf[PhysAllocIndex];
    if (pBuf->BufState != POOL_BUF_USED)
    {
        EDDI_Excp("EDDI_MemCheckPoolBuffer 4", EDDI_FATAL_ERR_EXCP, PhysAllocIndex, pBuf->BufState);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_POOL_CHECK_ALWAYS)
/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
static  void  EDDI_LOCAL_FCT_ATTR  EDDI_MemCheck_pool( const  LSA_UINT32  PoolHandle )  //IN
{
    POOL_VERW_H_T  *  pPoolVerw;
    LSA_INT32         i;
    POOL_STACK_T   *  pStack;
    POOL_BUF_T     *  pBuf;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemCheck_pool PoolHandle", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemCheck_pool 2", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern, 0);
        return;
    }

    for (i = 0; i < pPoolVerw->alloc_index; i++)
    {
        pStack = &pPoolVerw->pStack[i];
        pBuf   = &pPoolVerw->buf[pStack->PhysAllocIndex];

        if ((*pBuf->pBegin != EDDI_POOL_CHECK_PAT) || (*pBuf->pEnd != EDDI_POOL_CHECK_PAT))
        {
            EDDI_Excp("EDDI_MemCheck_pool 3", EDDI_FATAL_ERR_EXCP, pPoolVerw->alloc_index, *pBuf->pEnd);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MemGetPoolBuffersize( const  LSA_UINT32 PoolHandle )//IN
{
    POOL_VERW_H_T  *  pPoolVerw;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    pPoolVerw = (POOL_VERW_H_T   *)(void *)PoolHandle;

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemGetPoolBuffer 1", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern, 0);
        return 0;
    }

    EDDI_MemCheck_pool(PoolHandle);

    return (pPoolVerw->BufferSize);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_INT32  EDDI_LOCAL_FCT_ATTR  EDDI_MemPoolGetFreeBuffer( const  LSA_UINT32  PoolHandle )
{
    POOL_VERW_H_T  *  const  pPoolVerw = (POOL_VERW_H_T *)(void *)PoolHandle;

    if (PoolHandle == 0)
    {
        EDDI_Excp("EDDI_MemPoolGetFreeBuffer 0", EDDI_FATAL_ERR_EXCP, 0, 0);
        return 0;
    }

    if (pPoolVerw->CheckPattern != EDDI_POOL_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MemPoolGetFreeBuffer 1", EDDI_FATAL_ERR_EXCP, pPoolVerw->CheckPattern, 0);
        return 0;
    }

    EDDI_MemCheck_pool(PoolHandle);

    return (pPoolVerw->alloc_index);
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV7)
//////////////////////////////////////////////////////////////////////////
///
/// Checks whether the provided KRAM memory is enough to hold all APDUs
/// for the number of providers+consumers+forwarders.
///
/// @param pDDB for getting consumer and provider related informations
///
/// @return EDD_STS_ERR_NOT_ALLOWED if PAEA-Ram is not used by HW.
/// @return EDD_STS_ERR_RESOURCE if not enough KRAM for APDU + PAEA-pointer (8-Byte)
///         was specified.
/// @return EDD_STS_OK if buffer size matches number of providers/consumers.
///
//////////////////////////////////////////////////////////////////////////
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MemCalcApduBuffer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32                               consumer_entries = 0;
    LSA_UINT32                               provider_entries = 0;
    LSA_UINT32                               APDU_buffer_size = 0;
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp         = &pDDB->CRT;
    LSA_UINT32                               ret;

    if (pCRTComp->PAEA_Para.PAEA_BaseAdr)
    {
        // +1 is needed for XCHANGE_BUF as storage for copying
        consumer_entries = (pCRTComp->MetaInfo.AcwConsumerCnt +
                            pCRTComp->MetaInfo.FcwConsumerCnt +
                            1);                                   //+1 for consumer-xchange

        provider_entries = (pCRTComp->MetaInfo.AcwProviderCnt +
                            pCRTComp->MetaInfo.FcwProviderCnt +
                            1);                                   //+1 for provider-xchange
        //pCRTComp->CfgPara.IRT.ForwarderCnt);

        APDU_buffer_size = (consumer_entries + provider_entries) * sizeof(EDDI_CRT_PAEA_APDU_STATUS);

        //Malloc StateBuffer-area from EDD-KRAM
        ret = EDDI_MEMGetBuffer(pDDB->KRamMemHandle, &pCRTComp->hPool.pMemHeaderStateBuffer, APDU_buffer_size);
        if (ret != EDDI_MEM_RET_OK)
        {
            if (ret == EDDI_MEM_RET_NO_FREE_MEMORY)
            {
                EDDI_Excp("EDDI_MemCalcApduBuffer !!", EDDI_FATAL_ERR_EXCP, ret, 0);
            }
            else
            {
                EDDI_Excp("EDDI_MemCalcApduBuffer - EDDI_MEMGetBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, ret, 0);
            }
            return EDD_STS_ERR_EXCP;
        }
        else
        {
            pDDB->pKRamStateBuffer    = pCRTComp->hPool.pMemHeaderStateBuffer->pKRam;
            pDDB->SizeKRamStateBuffer = APDU_buffer_size;
        }
    }
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MemCloseApduBuffer( EDDI_LOCAL_DDB_PTR_TYPE           const  pDDB,
                                                        EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp )
{
    LSA_UINT32  ret32;
    LSA_UINT16  ret16;

    LSA_UNUSED_ARG(pDDB);

    if (pCRTComp->PAEA_Para.PAEA_BaseAdr)
    {
        //Dealloc StateBuffer-area from EDD-KRAM
        ret32 = EDDI_MEMFreeBuffer(pCRTComp->hPool.pMemHeaderStateBuffer);
        if (ret32 != EDDI_MEM_RET_OK)
        {
            EDDI_Excp("EDDI_MemCloseApduBuffer - EDDI_MEMFreeBuffer see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, ret32, 0);
            return;
        }

        //Dealloc ConsumerMirrorArray
        EDDI_FREELOCALMEM(&ret16, (EDDI_DEV_MEM_PTR_TYPE)(pCRTComp->PAEA_Para.pConsumerMirrorArray));
        if (ret16 != EDD_STS_OK)
        {
            EDDI_Excp("EDDI_MemCloseApduBuffer - EDDI_FREELOCALMEM see eddi_mem.h !!", EDDI_FATAL_ERR_EXCP, (LSA_UINT32)ret16, 0);
            return;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


//////////////////////////////////////////////////////////////////////////
///
/// Initializes an indirection layer for managing the relation
/// between ConsumerID and APDU if using PAEA-RAM.
/// Therefore a vector holding all consumerIds becomes setup. This entries
/// point to the corresponding APDU-block residing in the KRAM.
///
/// @param pDDB holds information about consumers and providers as well as
///        the pConsumerMirrorArray holding the consumerId vector
///
/// @return EDD_STS_ERR_NOT_ALLOWED if called without the usage of PAEA
/// @return EDD_STS_ERR_RESOURCE if no memory for vector could be allocated
/// @return EDD_STS_OK if everything went ok
///
//////////////////////////////////////////////////////////////////////////
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MemIniApduConsumerIndirectionLayer( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp        = &pDDB->CRT;
    LSA_UINT32                        const  numConsumers    = (pCRTComp->MetaInfo.AcwConsumerCnt +
                                                                pCRTComp->MetaInfo.FcwConsumerCnt +
                                                                1);

    if (0 == pCRTComp->PAEA_Para.PAEA_BaseAdr)
    {
        EDDI_Excp("EDDI_MemIniApduConsumerIndirectionLayer, Conflict! Cannot use APDU_BUFFERS without usage of PAEA-RAM!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    EDDI_AllocLocalMem((void * *)&pCRTComp->PAEA_Para.pConsumerMirrorArray, (numConsumers * sizeof(LSA_UINT32)));

    pCRTComp->PAEA_Para.ConsumerMirrorArrayLen = numConsumers;

    if (pCRTComp->PAEA_Para.pConsumerMirrorArray == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_MemIniApduConsumerIndirectionLayer, alloc pCRTComp->PAEA_Para.pConsumerMirrorOffset", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    //Set the whole consumer APDU-area to 0xFF to declare it as unused
    //(including one APDU as XCHANGE_BUF storage)
    EDDI_MemSet((void *)pDDB->pKRamStateBuffer, (LSA_UINT8)0xFF, pDDB->SizeKRamStateBuffer);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


//////////////////////////////////////////////////////////////////////////
///
/// Gives a KRAM-address for an unused APDU memory slot. This only works
/// in combination with the PAEA-Ram where an APDU+PA-pointer structure is
/// requested.
/// If the requested APDU-block belongs to a consumer, a double pointer list
/// is used for supporting @see XCHANGE_BUF_Consumer.
/// <pre>
/// +-----+
/// | ID0 |-------------+
/// +-----+             |
/// | ID1 |             \/     KRAM
/// +-----+          +-------+-------+-------+--------------
/// | ... |          | APDU0 | APDUx | APDUy | ...        //
/// +-----+          +-------+-------+-------+--------------
/// | IDn |
/// +-----+</pre>
///
/// @param consumerId EDD-internal ID number of the provider/consumer. Gives
///        the position in the memory array at the beginning of the KRAM.
/// @param pDDB holding informations about consumer+provider
/// @param **pAPDU_Buffer is an empty structure which points to an valid
///        KRAM position after calling this function.
/// @param mode decides whether a provider or a consumer buffer should be
///        returned.
///
/// @return EDD_STS_ERR_RESOURCE if no further resources could be allocated.
///         Specify more space with "offset_ProcessImageEnd".
/// @return EDD_STS_ERR_MODE if no valid mode was chosen.
/// @return EDD_STS_OK if everything went correct.
///
//////////////////////////////////////////////////////////////////////////
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MemGetApduBuffer( LSA_UINT32                       const  ProvConsId,
                                                        EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                        EDDI_CRT_PAEA_APDU_STATUS  *  *  const  ppAPDU_Buffer,
                                                        EDDI_MEM_APDU_BUFFER_TYPE        const  Mode )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp        = &pDDB->CRT;
    LSA_UINT32                               kram_apdu_pos   = 0;
    LSA_UINT32                               kram_apdu_adr   = 0;
    LSA_UINT32                        const  apduSize        = sizeof(EDDI_CRT_PAEA_APDU_STATUS);
    LSA_UINT32                        const  consumer_offset = (pCRTComp->MetaInfo.AcwProviderCnt +
                                                                pCRTComp->MetaInfo.FcwProviderCnt +
                                                                1) * apduSize;
    LSA_UINT32                        const  numConsumers    = (pCRTComp->MetaInfo.AcwConsumerCnt +
                                                                pCRTComp->MetaInfo.FcwConsumerCnt);

    *ppAPDU_Buffer = LSA_NULL;

    //Get Array slice for APDU from PA-area at the beginning of the KRAM
    if (EDDI_MEM_APDU_PROVIDER_BUFFER == Mode)
    {
        kram_apdu_pos = ProvConsId * apduSize;
        kram_apdu_adr = (LSA_UINT32)(pDDB->pKRamStateBuffer + kram_apdu_pos);

        if (kram_apdu_adr >= (LSA_UINT32)(pDDB->pKRamStateBuffer + consumer_offset))
        {
            EDDI_Excp("Requested PAEA-APDU exceeds the reserved memory space (Wrong Provider Index?). Please check \"offset_ProcessImageEnd\"! APDU Address %x, EDD Startingpoint %x",
                      EDDI_FATAL_ERR_EXCP, kram_apdu_adr, pDDB->pKramFixMem);
            return EDD_STS_ERR_EXCP;
        }

        *ppAPDU_Buffer = (EDDI_CRT_PAEA_APDU_STATUS *)kram_apdu_adr;
    }
    else if (EDDI_MEM_APDU_CONSUMER_BUFFER == Mode)
    {
        LSA_UINT32  apduSlotPos;

        //Search for the first free position in APDU-consumer area
        kram_apdu_adr  = (LSA_UINT32)(consumer_offset + pDDB->pKRamStateBuffer);

        for (apduSlotPos = 0; apduSlotPos < (numConsumers+1); apduSlotPos++)
        {
            //only paea-ptr is used for free-check, apdu-status.data_ctrl has to remain 0!
            if (*(LSA_UINT32 *)(kram_apdu_adr + 4) == EDD_DATAOFFSET_UNDEFINED)
            {
                //Found free Cell, store APDU
                *ppAPDU_Buffer = (EDDI_CRT_PAEA_APDU_STATUS *)(void *)kram_apdu_adr;
                break;
            }
            else
            {
                //Cell not free
                kram_apdu_adr += apduSize;
                continue;
            }
        }

        if (kram_apdu_adr >= ((LSA_UINT32)pDDB->pKRamStateBuffer + pDDB->SizeKRamStateBuffer - apduSize))
        {
            EDDI_Excp("Requested PAEA-APDU exceeds the reserved memory space (Wrong Consumer Index?). Please check \"offset_ProcessImageEnd\"! APDU Address %x, EDD Startingpoint %x",
                      EDDI_FATAL_ERR_EXCP, kram_apdu_adr, pDDB->pKRamStateBuffer);
            return EDD_STS_ERR_EXCP;
        }

        if (LSA_NULL == *ppAPDU_Buffer)
        {
            EDDI_Excp("EDDI_MemGetApduBuffer: Could not find free APDU slot for consumer!", EDDI_FATAL_ERR_EXCP, 0, 0);
            return EDD_STS_ERR_EXCP;
        }

        if (ProvConsId > numConsumers)
        {
            EDDI_Excp("EDDI_MemGetApduBuffer: ProvConsId 0x%X sucseeds number of allocated APDU slots 0x%X", EDDI_FATAL_ERR_EXCP, ProvConsId, numConsumers);
            return EDD_STS_ERR_EXCP;
        }

        //enter apdu-position into ConsumerMirrorArray
        *(pCRTComp->PAEA_Para.pConsumerMirrorArray + ProvConsId) = (LSA_UINT32)kram_apdu_adr;
    }
    else
    {
        EDDI_Excp("Wrong Mode. Valid are only EDDI_MEM_APDU_PROVIDER/CONSUMER_BUFFER", EDDI_FATAL_ERR_EXCP, 0, 0);
        return EDD_STS_ERR_EXCP;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


//////////////////////////////////////////////////////////////////////////
///
/// Marks an already use APDU-slot (in KRAM) for re usage. Needed in
/// conjunction with PAEA and a indirection layer. Called each time a consumer
/// gets removed.
///
/// @param consumerId to which consumer the APDU belongs to
/// @param pDDB holding informations about the consumer and PAEA
///
/// @return EDD_STS_ERR_PARAM if consumerId could not be associated with APDU
/// @return EDD_STS_OK
//////////////////////////////////////////////////////////////////////////
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MemFreeApduBufferCons( LSA_UINT32               const  ProvConsId,
                                                            EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE  const  pCRTComp = &pDDB->CRT;
    EDDI_CRT_PAEA_APDU_STATUS             *  pApduStatus;

    //Validate ConsumerId
    if (ProvConsId > pCRTComp->PAEA_Para.ConsumerMirrorArrayLen)
    {
        EDDI_Excp("EDDI_MemFreeApduBufferCons(): Buffer-Overflow! ProvConsId 0x%X, ConsumerMirrorArrayLen 0x%X",
                  EDDI_FATAL_ERR_EXCP, ProvConsId, pCRTComp->PAEA_Para.ConsumerMirrorArrayLen);
        return EDD_STS_ERR_EXCP;
    }

    pApduStatus = (EDDI_CRT_PAEA_APDU_STATUS *)(*(pCRTComp->PAEA_Para.pConsumerMirrorArray + ProvConsId));

    //Invalidate APDU-slot by setting the paearam-ptr to 0xFF, apdu-status.data_ctrl has to remain 0
    pApduStatus->pPAEA_Data = EDD_DATAOFFSET_UNDEFINED;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //EDDI_CFG_REV7


/****************************************************************************/
/*  end of file eddi_pool.c                                                 */
/****************************************************************************/


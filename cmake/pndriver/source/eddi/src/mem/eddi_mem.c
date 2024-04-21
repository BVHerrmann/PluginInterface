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
/*  F i l e               &F: eddi_mem.c                                :F&  */
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
/*****************************************************************************/

#include "eddi_int.h"

#include "eddi_dev.h"
#include "eddi_pool.h"

#define EDDI_MODULE_ID     M_ID_EDDI_MEM
#define LTRC_ACT_MODUL_ID  24

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#define EDDI_MEM_CHECK_PAT            0xabcd0123UL

typedef struct _EDDI_MEM_BUFFER_EL
{
    EDDI_MEM_BUF_EL_H              Header;

    LSA_UINT32                     KramOffset;

    struct _EDDI_MEM_BUFFER_EL  *  pPrev;
    struct _EDDI_MEM_BUFFER_EL  *  pNext;

    LSA_UINT32                     free_len_to_next_buf;
    EDDI_MEM_VERW_H_T           *  pVerw;    //fix
    LSA_UINT32                     checksum; //only fix
    LSA_UINT32                     buf_len;  //fix

} EDDI_MEM_BUFFER_EL;

#define EDDI_MEM_NULL_H  (EDDI_MEM_BUFFER_EL *)0

static LSA_UINT32 EDDI_MemCheckH( const EDDI_MEM_BUFFER_EL * const h );

static LSA_UINT32 EDDI_MemGetChecksumH( const EDDI_MEM_BUFFER_EL * const h );

static LSA_VOID EDDI_MemCheck( const EDDI_MEM_BUFFER_EL * const h );


/***************************************************************************/
/* F u n c t i o n:       EDDI_MEMIni()                                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MEMIni( const  LSA_UINT32                  MaxBuffer,
                                            const  LSA_UINT32                  Alignment,
                                            const  LSA_UINT32                  KramSize,
                                            LSA_UINT8                *  const  pKRAM,
                                            const  LSA_BOOL                    do_preset_buffer,
                                            EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                            LSA_UINT32                      *  MemHandle )
{
    //LSA_UINT32          SizeVerw;
    EDDI_MEM_VERW_H_T   *  pMemVerw;
    EDDI_MEM_BUFFER_EL  *  pFirstEl;
    #if (0)
    LSA_UINT32          *  pTest; //do not change FSU behaviour
    #endif

    if ((MaxBuffer == 0) || (KramSize == 0))
    {
        EDDI_Excp("EDDI_MEMIni", EDDI_FATAL_ERR_EXCP, MaxBuffer, KramSize);
        return;
    }

    pMemVerw = &pDDB->MemVerw;
    if (pMemVerw->CheckPattern != 0)
    {
        EDDI_Excp("EDDI_MEMIni, Double Call!", EDDI_FATAL_ERR_EXCP, Alignment, 0);
        return;
    }

    *MemHandle = (LSA_UINT32)(void *)pMemVerw;
    pMemVerw->MemHandle = *MemHandle;

    pMemVerw->CheckPattern        = EDDI_MEM_CHECK_PAT;

    pMemVerw->pKRAM               = pKRAM;

    pMemVerw->do_preset_buffer    = do_preset_buffer;
    pMemVerw->preset_value        = 0xFF;
    pMemVerw->KramSize            = KramSize;
    pMemVerw->MaxBuffer           = MaxBuffer;
    pMemVerw->Alignment           = Alignment;

    switch (Alignment)
    {
        case 4:
            pMemVerw->AligMask = 0x03UL;
            break;
        case 8:
            pMemVerw->AligMask = 0x07UL;
            break;

        default:
        {
            EDDI_Excp("EDDI_MEMIni, Alignment", EDDI_FATAL_ERR_EXCP, Alignment, 0);
            return;
        }
    }

    if (pMemVerw->AligMask & (LSA_UINT32)pKRAM)
    {
        EDDI_Excp("EDDI_MEMIni, pMemVerw->AligMask & (LSA_UINT32)pKRAM", EDDI_FATAL_ERR_EXCP, Alignment, pKRAM);
        return;
    }

    (void)EDDI_MemIniPool( MaxBuffer,
                           (LSA_UINT32)sizeof(EDDI_MEM_BUFFER_EL),
                           EDDI_POOL_TYPE_LOCAL_MEM,
                           EDDI_AllocLocalMem,
                           EDDI_FREELOCALMEM,
                           EDDI_MEM_DO_PRESET,
                           (LSA_UINT8)0, // Preset Value
                           pDDB,
                           &pMemVerw->hPool); // OUT

    EDDI_MemGetPoolBuffer(pMemVerw->hPool, (void * *)&pFirstEl);

    pMemVerw->pFirst_el = pFirstEl;
    pMemVerw->pAct_el   = pFirstEl;

    pMemVerw->check_reenter = 0;

    pFirstEl->Header.pKRam         = pKRAM;
    pFirstEl->Header.MemHandle     = pMemVerw->MemHandle;
    pFirstEl->KramOffset           = 0;
    pFirstEl->buf_len              = 0;
    pFirstEl->free_len_to_next_buf = pMemVerw->KramSize;
    pFirstEl->pPrev                = EDDI_MEM_NULL_H;
    pFirstEl->pNext                = EDDI_MEM_NULL_H;
    pFirstEl->pVerw                = pMemVerw;
    pFirstEl->checksum             = EDDI_MemGetChecksumH(pFirstEl);

    #if (0)
    pTest = (LSA_UINT32 *)(void *)pFirstEl->Header.pKRam; //do not change FSU behaviour!
    if ((*pTest != 0xFFFFFFFFUL) || (pDDB->pKramFixMem->BorderLine.dw[0] != 0xFFFFFFFFUL))  // Check for HW stuck at reset state
    #endif
    {
        EDDI_MemSet(pFirstEl->Header.pKRam, pMemVerw->preset_value, pMemVerw->KramSize);
        EDDI_MemSet(pDDB->pKramFixMem, pMemVerw->preset_value, sizeof(EDDI_KRAM_MEM_TYPE_T));
    }

    if (EDDI_LOC_FPGA_XC2_V8000 == pDDB->ERTEC_Version.Location)
    {
        //FPGA-Board's memory does not become reset correctly, thus we have to set it manually
        EDDI_MemSet(pFirstEl->Header.pKRam, pMemVerw->preset_value, pMemVerw->KramSize);
        EDDI_MemSet(pDDB->pKramFixMem, pMemVerw->preset_value, sizeof(EDDI_KRAM_MEM_TYPE_T));
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
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MEMClose( LSA_UINT32  const  MemHandle )
{
    EDDI_MEM_VERW_H_T  *  pMemVerw;

    if (MemHandle == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_MEMClose", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)MemHandle;

    if (pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT)
    {
        EDDI_Excp("EDDI_MEMClose pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT", EDDI_FATAL_ERR_EXCP, pMemVerw->CheckPattern, 0);
        return;
    }

    pMemVerw->CheckPattern = 0;

    EDDI_MemClosePool(pMemVerw->hPool); //IN
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMGetBuffer( const  LSA_UINT32        MemHandle,
                                                    EDDI_MEM_BUF_EL_H   *  *  ppHeader,
                                                    const  LSA_UINT32        len )
{
    EDDI_MEM_VERW_H_T   *  pMemVerw;
    LSA_UINT32            check_reenter, real_len, i, ret;
    EDDI_MEM_BUFFER_EL  *  pEl;
    EDDI_MEM_BUFFER_EL  *  pNewEl;

    if (MemHandle == EDDI_NULL_PTR)
    {
        return EDDI_MEM_RET_INVALID_HANDLE;
    }

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)MemHandle;

    if (pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT)
    {
        return EDDI_MEM_RET_INVALID_CHECK_PAT;
    }

    if (len == 0)
    {
        return EDDI_MEM_RET_MEM_LEN_NULL;
    }

    check_reenter = pMemVerw->check_reenter;

    pMemVerw->check_reenter = 1;

    if (check_reenter)
    {
        return EDDI_MEM_RET_REENTER;
    }

    pEl = pMemVerw->pAct_el;
    *ppHeader = &pEl->Header;

    if (len & pMemVerw->AligMask)
    {
        real_len = (len + pMemVerw->Alignment - 1) & (~pMemVerw->AligMask);
    }
    else
    {
        real_len = len;
    }

    // Special case: The 1st buffer always has buf_len=0 (not allocated)
    // This buffer is never deleted!
    if ((pEl->buf_len == 0) && (pEl->free_len_to_next_buf >= real_len))
    {
        if (pEl->pPrev != EDDI_MEM_NULL_H)
        {
            return EDDI_MEM_RET_INVALID_FIRST_BUF;
        }

        pEl->free_len_to_next_buf = pEl->free_len_to_next_buf - real_len;
        pEl->buf_len              = real_len;

        pMemVerw->count_allocated_buffer++;
        pMemVerw->used_mem      += real_len;
        pMemVerw->check_reenter  = 0;

        EDDI_MemCheck(pEl);
        return EDDI_MEM_RET_OK;
    }

    if (pEl->free_len_to_next_buf < real_len)
    {
        // start search from the beginning
        pEl = pMemVerw->pFirst_el;

        for (i = 0; i < pMemVerw->MaxBuffer; i++)
        {
            ret = EDDI_MemCheckH(pEl);
            if (ret != EDDI_MEM_RET_OK)
            {
                pMemVerw->check_reenter  = 0;
                return ret;
            }

            if (pEl->free_len_to_next_buf >= real_len)
            {
                // found!
                break;
            }

            pEl = pEl->pNext;
            if (pEl == EDDI_MEM_NULL_H)
            {
                pMemVerw->check_reenter  = 0;
                return EDDI_MEM_RET_NO_FREE_MEMORY;
            }
        }  // for

        if (i == pMemVerw->MaxBuffer)
        {
            pMemVerw->check_reenter  = 0;
            return EDDI_MEM_RET_NO_FREE_MEMORY;
        }
    }

    // found -> initialize new buffer
    // allocate new administration element
    EDDI_MemGetPoolBuffer(pMemVerw->hPool, (void * *)ppHeader);  //OUT
    pNewEl = (EDDI_MEM_BUFFER_EL *)(void *)*ppHeader;

    pNewEl->pVerw                = pMemVerw;
    pNewEl->buf_len              = real_len;
    pNewEl->free_len_to_next_buf = pEl->free_len_to_next_buf - real_len;
    pNewEl->KramOffset           = pEl->KramOffset + pEl->buf_len;
    pNewEl->Header.pKRam         = pMemVerw->pKRAM + pNewEl->KramOffset;
    pNewEl->Header.MemHandle     = pMemVerw->MemHandle;

    if (pMemVerw->do_preset_buffer)
    {
        EDDI_MemSet(pNewEl->Header.pKRam, pMemVerw->preset_value, real_len);
    }

    // chain with previous
    pNewEl->pPrev    = pEl;
    pNewEl->pNext    = pEl->pNext;
    pNewEl->checksum = EDDI_MemGetChecksumH(pEl);

    //adapt previous
    pEl->free_len_to_next_buf = 0;
    pEl->pNext                = pNewEl;

    // adapt next element if it exists
    if (pNewEl->pNext != EDDI_MEM_NULL_H)
    {
        (pNewEl->pNext)->pPrev = pNewEl;
    }

    pMemVerw->pAct_el = pNewEl;

    pMemVerw->count_allocated_buffer++;
    pMemVerw->used_mem += real_len;

    pMemVerw->check_reenter = 0;
    EDDI_MemCheck(pNewEl);

    return EDDI_MEM_RET_OK;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMTestBuffer( LSA_UINT32      const  MemHandle,
                                                     LSA_UINT32  const  len )
{
    EDDI_MEM_VERW_H_T   *  pMemVerw;
    LSA_UINT32             check_reenter, real_len, i, ret;
    EDDI_MEM_BUFFER_EL  *  pEl;

    if (MemHandle == EDDI_NULL_PTR)
    {
        return EDDI_MEM_RET_INVALID_HANDLE;
    }

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)MemHandle;

    if (pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT)
    {
        return EDDI_MEM_RET_INVALID_CHECK_PAT;
    }

    if (len == 0)
    {
        return EDDI_MEM_RET_MEM_LEN_NULL;
    }

    check_reenter = pMemVerw->check_reenter;

    pMemVerw->check_reenter = 1;

    if (check_reenter)
    {
        return EDDI_MEM_RET_REENTER;
    }

    pEl = pMemVerw->pAct_el;

    if (len & pMemVerw->AligMask)
    {
        real_len = (len + pMemVerw->Alignment - 1) & (~pMemVerw->AligMask);
    }
    else
    {
        real_len = len;
    }

    // Special case: The 1st buffer always has buf_len=0 (not allocated)
    // This buffer is never deleted!
    if ((pEl->buf_len == 0) && (pEl->free_len_to_next_buf >= real_len))
    {
        if (pEl->pPrev != EDDI_MEM_NULL_H)
        {
            return EDDI_MEM_RET_INVALID_FIRST_BUF;
        }
        pMemVerw->check_reenter  = 0;
        return EDDI_MEM_RET_OK;
    }

    if (pEl->free_len_to_next_buf < real_len)
    {
        // start search from the beginning
        pEl = pMemVerw->pFirst_el;

        for (i = 0; i < pMemVerw->MaxBuffer; i++)
        {
            ret = EDDI_MemCheckH(pEl);
            if (ret != EDDI_MEM_RET_OK)
            {
                pMemVerw->check_reenter  = 0;
                return ret;
            }

            if (pEl->free_len_to_next_buf >= real_len)
            {
                // found!
                pMemVerw->check_reenter  = 0;
                return EDDI_MEM_RET_OK;
            }

            pEl = pEl->pNext;
            if (pEl == EDDI_MEM_NULL_H)
            {
                pMemVerw->check_reenter  = 0;
                return EDDI_MEM_RET_NO_FREE_MEMORY;
            }

        }  // for

        if (i == pMemVerw->MaxBuffer)
        {
            pMemVerw->check_reenter  = 0;
            return EDDI_MEM_RET_NO_FREE_MEMORY;
        }

    }

    pMemVerw->check_reenter = 0;

    return EDDI_MEM_RET_OK;
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
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMFreeBuffer( EDDI_MEM_BUF_EL_H  *  const  pHeader )
{
    EDDI_MEM_VERW_H_T    *pMemVerw;
    LSA_UINT32           check_reenter, ret;
    EDDI_MEM_BUFFER_EL   *pEl;
    EDDI_MEM_BUFFER_EL   *pNext, *pPrev;

    if (!pHeader)
    {
        return EDDI_MEM_RET_ERR_NULL_PTR;
    }

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)pHeader->MemHandle;

    if (pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT)
    {
        return EDDI_MEM_RET_INVALID_CHECK_PAT;
    }

    check_reenter = pMemVerw->check_reenter;

    pMemVerw->check_reenter = 1;

    if (check_reenter)
    {
        return EDDI_MEM_RET_REENTER;
    }

    pEl = (EDDI_MEM_BUFFER_EL *)(void *)pHeader;

    ret = EDDI_MemCheckH(pEl);
    if (ret != EDDI_MEM_RET_OK)
    {
        return ret;
    }

    EDDI_MemCheckPoolBuffer(pMemVerw->hPool, pHeader);

    if (pEl->pVerw->do_preset_buffer)
    {
        EDDI_MemSet(pEl->Header.pKRam, pEl->pVerw->preset_value, pEl->buf_len);
    }

    pMemVerw->count_allocated_buffer--;
    pMemVerw->used_mem -= pEl->buf_len;

    pNext = pEl->pNext;
    pPrev = pEl->pPrev;

    if (pPrev == EDDI_MEM_NULL_H)
    {
        // enqueue 1st buffer -> element is not freed!
        pEl->free_len_to_next_buf = pEl->free_len_to_next_buf + pEl->buf_len;
        pEl->buf_len              = 0;

        pMemVerw->pAct_el       = pEl;
        pMemVerw->check_reenter = 0;
        return EDDI_MEM_RET_OK;
    }

    // allocationptr points to previous element
    pMemVerw->pAct_el = pPrev;

    // enqueue intermediate buffer
    pPrev->pNext = pNext;

    pPrev->free_len_to_next_buf += pEl->buf_len + pEl->free_len_to_next_buf;

    if (pNext == EDDI_MEM_NULL_H)
    {
        // was last buffer in reality
        pMemVerw->check_reenter = 0;
        return EDDI_MEM_RET_OK;
    }

    // was real intermediate buffer
    pNext->pPrev = pPrev;

    EDDI_MemFreePoolBuffer(pMemVerw->hPool, pHeader);

    pMemVerw->check_reenter = 0;
    return EDDI_MEM_RET_OK;
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
/***************************************************************************/
/*
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEM_check_buffer( EDDI_MEM_BUF_EL_H  *  const  pHeader )
{
  EDDI_MEM_VERW_H_T   *  pMemVerw;
  EDDI_MEM_BUFFER_EL  *  pEl;
  LSA_UINT32            ret;

  pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)pHeader->MemHandle;

  if ( pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT )
  {
    return EDDI_MEM_RET_INVALID_CHECK_PAT;
  }

  pEl = (EDDI_MEM_BUFFER_EL *)(void *)pHeader;

  ret = EDDI_MemCheckH(pEl);

  EDDI_MemCheckPoolBuffer( pMemVerw->hPool, pHeader);

  return ret;
} */
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
static  LSA_UINT32  EDDI_MemCheckH( const  EDDI_MEM_BUFFER_EL  *  const  h )
{
    LSA_UINT32                   checksum;
    const  EDDI_MEM_BUFFER_EL  *  prev_h, * next_h;

    if ((LSA_UINT32)h & 0x03)
    {
        return EDDI_MEM_RET_INVALID_POINTER_ALIGN;
    }

    checksum = EDDI_MemGetChecksumH(h);

    if (checksum != h->checksum)
    {
        return EDDI_MEM_RET_MEM_CORRUPT_PREV_OVERWRITE;
    }

    prev_h = h->pPrev;
    next_h = h->pNext;

    if (prev_h && (prev_h->pNext != h))
    {
        return EDDI_MEM_RET_MEM_CORRUPT_PREV_POINTER;
    }

    if (next_h && (next_h->pPrev != h))
    {
        return EDDI_MEM_RET_MEM_CORRUPT_NEXT_POINTER;
    }

    return EDDI_MEM_RET_OK;
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
static  LSA_UINT32  EDDI_MemGetChecksumH( const  EDDI_MEM_BUFFER_EL  *  const  h )
{
    LSA_UINT32  checksum;

    checksum = (LSA_UINT32)(h->pVerw);

    checksum++;

    return checksum;
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
static  LSA_VOID  EDDI_MemCheck( const  EDDI_MEM_BUFFER_EL  *  const  h )
{
    LSA_UINT32     i, len;
    LSA_UINT8   *  p;
    LSA_UINT8      PresetValue;

    if (!h->pVerw->do_preset_buffer)
    {
        return;
    }

    //  len         = h->buf_len + h->free_len_to_next_buf;
    len         = h->buf_len;
    PresetValue = h->pVerw->preset_value;
    p           = h->Header.pKRam;

    for (i = 0; i < len; i++)
    {
        if (*p != PresetValue)
        {
            EDDI_Excp("EDDI_MemCheck - i, *p", EDDI_FATAL_ERR_EXCP, i, *p);
            return;
        }
        p++;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_MEMInfo()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_UINT32                                       */
/*                                                                         */
/***************************************************************************/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MEMInfo( EDDI_MEM_INFO_T  *  const  info )
{
    EDDI_MEM_VERW_H_T   *  pMemVerw;
    LSA_UINT32             ret, i;
    EDDI_MEM_BUFFER_EL  *  pEl;

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)info->MemHandle;

    info->max_free_buffer_size   = 0;
    info->free_mem               = 0;
    info->count_allocated_buffer = 0;
    info->used_mem               = 0;

    if (info->MemHandle == EDDI_NULL_PTR)
    {
        return EDDI_MEM_RET_INVALID_HANDLE;
    }

    pMemVerw = (EDDI_MEM_VERW_H_T *)(void *)info->MemHandle;

    if (pMemVerw->CheckPattern != EDDI_MEM_CHECK_PAT)
    {
        return EDDI_MEM_RET_INVALID_CHECK_PAT;
    }

    pEl = pMemVerw->pFirst_el;

    for (i = 0; i < pMemVerw->count_allocated_buffer; i++)
    {
        if (pEl->buf_len > 0)
        {
            info->count_allocated_buffer++; //first buffer exists always, but can be empty
        }

        info->free_mem += pEl->free_len_to_next_buf;
        info->used_mem += pEl->buf_len;

        if (pEl->free_len_to_next_buf > info->max_free_buffer_size)
        {
            info->max_free_buffer_size = pEl->free_len_to_next_buf;
        }

        pEl = pEl->pNext;

        if (pEl == EDDI_MEM_NULL_H)
        {
            break;
        }

        ret = EDDI_MemCheckH(pEl);
        if (ret != EDDI_MEM_RET_OK)
        {
            return ret;
        }
    }  //for

    if (i == pMemVerw->count_allocated_buffer)
    {
        return EDDI_MEM_RET_MEM_CORRUPT;
    }

    return EDDI_MEM_RET_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/****************************************************************************/
/*  end of file eddi_mem.c                                                  */
/****************************************************************************/


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
/*  F i l e               &F: pnd_eddi_cfg.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  This File is for output function implementations expected from PN Driver */
/*                                                                           */
/*****************************************************************************/

#include "pnd_int.h"
#include "pnd_sys.h" 
#include "eddi_iocc_cfg.h"
#include "eddi_iocc.h"
#include "eps_mempool.h"


#define LTRC_ACT_MODUL_ID   20052
#define PND_MODULE_ID       20052

enum _PND_ENTER_EXIT
{
	EDDI_APPLSYNC_ISO_APPL,
    EDDI_IOCC_CH1,
    EDDI_IOCC_CH2,
	LOCK_MAX
};

static PNIO_UINT16 pnd_enter_exit_id[LOCK_MAX];

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_eddi_alloc_applsync_lock()
{
	LSA_RESPONSE_TYPE result = LSA_RET_OK;
	pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL] = PND_LOCK_ID_INVALID;
	result = pnd_alloc_critical_section_prio_protected( &pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL], PNIO_FALSE);
	PND_ASSERT(result == LSA_RET_OK);
}

/*---------------------------------------------------------------------------*/
LSA_VOID pnd_eddi_free_applsync_lock()
{
	LSA_RESPONSE_TYPE result = LSA_RET_OK;
	result = pnd_free_critical_section( pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL]);
	PND_ASSERT(result == LSA_RET_OK);
	pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL] = PND_LOCK_ID_INVALID;
}

/*---------------------------------------------------------------------------*/
#ifndef EDDI_ENTER_APPLSYNC
PNIO_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_ENTER_APPLSYNC(PNIO_VOID)
{
	pnd_enter_critical_section(pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL]);
}
#else
#error "by design a function!"
#endif
/*---------------------------------------------------------------------------*/
#ifndef EDDI_EXIT_APPLSYNC
PNIO_VOID  EDDI_SYSTEM_OUT_FCT_ATTR  EDDI_EXIT_APPLSYNC(PNIO_VOID)
{
    pnd_exit_critical_section(pnd_enter_exit_id[EDDI_APPLSYNC_ISO_APPL]);
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
PNIO_VOID pnd_eddi_alloc_iocc_ch1_lock()
{
    LSA_RESPONSE_TYPE result = LSA_RET_OK;

    pnd_enter_exit_id[EDDI_IOCC_CH1] = PND_LOCK_ID_INVALID;

    result = pnd_alloc_critical_section(&pnd_enter_exit_id[EDDI_IOCC_CH1], LSA_FALSE);
    PND_ASSERT(result == LSA_RET_OK);
}

/*---------------------------------------------------------------------------*/
PNIO_VOID pnd_eddi_alloc_iocc_ch2_lock()
{
    LSA_RESPONSE_TYPE result = LSA_RET_OK;

    pnd_enter_exit_id[EDDI_IOCC_CH2] = PND_LOCK_ID_INVALID;

    result = pnd_alloc_critical_section(&pnd_enter_exit_id[EDDI_IOCC_CH2], LSA_FALSE);
    PND_ASSERT(result == LSA_RET_OK);
}


/*---------------------------------------------------------------------------*/
PNIO_VOID pnd_eddi_free_iocc_ch1_lock()
{
    LSA_RESPONSE_TYPE result = LSA_RET_OK;

    result = pnd_free_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH1]);
    PND_ASSERT(result == LSA_RET_OK);
    pnd_enter_exit_id[EDDI_IOCC_CH1] = PND_LOCK_ID_INVALID;
}

/*---------------------------------------------------------------------------*/
PNIO_VOID pnd_eddi_free_iocc_ch2_lock()
{
    LSA_RESPONSE_TYPE result = LSA_RET_OK;

    result = pnd_free_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH2]);
    PND_ASSERT(result == LSA_RET_OK);
    pnd_enter_exit_id[EDDI_IOCC_CH2] = PND_LOCK_ID_INVALID;
}

/*---------------------------------------------------------------------------*/
PNIO_BOOL is_pnd_eddi_iocc_ch1_lock_allocated()
{
	return (pnd_enter_exit_id[EDDI_IOCC_CH1] == PND_LOCK_ID_INVALID) ? PNIO_FALSE : PNIO_TRUE;
}

/*---------------------------------------------------------------------------*/
PNIO_BOOL is_pnd_eddi_iocc_ch2_lock_allocated()
{
	return (pnd_enter_exit_id[EDDI_IOCC_CH2] == PND_LOCK_ID_INVALID) ? PNIO_FALSE : PNIO_TRUE;
}

/*---------------------------------------------------------------------------*/
#ifndef EDDI_ENTER_IOCC_CH1
LSA_VOID EDDI_ENTER_IOCC_CH1(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    LSA_UNUSED_ARG(UpperHandle);

    pnd_enter_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH1]);
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_ENTER_IOCC_CH2
LSA_VOID EDDI_ENTER_IOCC_CH2(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    LSA_UNUSED_ARG(UpperHandle);

    pnd_enter_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH2]);
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef EDDI_EXIT_IOCC_CH1
LSA_VOID EDDI_EXIT_IOCC_CH1(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    LSA_UNUSED_ARG(UpperHandle);

    pnd_exit_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH1]);
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_EXIT_IOCC_CH2
LSA_VOID EDDI_EXIT_IOCC_CH2(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle)
{
    LSA_UNUSED_ARG(UpperHandle);

    pnd_exit_critical_section(pnd_enter_exit_id[EDDI_IOCC_CH2]);
}
#else
#error "by design a function!"
#endif


/*---------------------------------------------------------------------------*/
#ifndef EDDI_IOCC_ALLOC_LOCAL_MEM
LSA_VOID EDDI_IOCC_ALLOC_LOCAL_MEM(LSA_VOID                * * ppLocalMem,
                                   LSA_UINT32                  Length)
{
    *ppLocalMem = eps_mem_alloc(Length, LSA_COMP_ID_PND, 0);
    PND_ASSERT(*ppLocalMem != PNIO_NULL);
}
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_IOCC_FREE_LOCAL_MEM
LSA_VOID EDDI_IOCC_FREE_LOCAL_MEM(LSA_UINT16                * pRetVal,
                                  LSA_VOID                  * pLocalMem)
{
    *pRetVal = eps_mem_free(pLocalMem, LSA_COMP_ID_PND, 0);
    PND_ASSERT(*pRetVal == LSA_RET_OK);
}
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_IOCC_ALLOC_LINKLIST_MEM
LSA_VOID EDDI_IOCC_ALLOC_LINKLIST_MEM(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                      LSA_VOID                * * ppLocalMem,
                                      LSA_UINT32                  Length,
                                      LSA_UINT8                   LinkListType)
{
    LSA_UNUSED_ARG(LinkListType);

    PND_EDDI_IOCC_UPPER_HANDLE_PTR_TYPE pHandle = (PND_EDDI_IOCC_UPPER_HANDLE_PTR_TYPE)UpperHandle;

    *ppLocalMem = eps_mempool_alloc(pHandle->llMempoolId, Length);
}
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_IOCC_FREE_LINKLIST_MEM
LSA_VOID EDDI_IOCC_FREE_LINKLIST_MEM(EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle,
                                     LSA_UINT16                * pRetVal,
                                     LSA_VOID                  * pLocalMem,
                                     LSA_UINT8                   LinkListType)
{
    LSA_UNUSED_ARG(LinkListType);

    PND_EDDI_IOCC_UPPER_HANDLE_PTR_TYPE pHandle = (PND_EDDI_IOCC_UPPER_HANDLE_PTR_TYPE)UpperHandle;

    *pRetVal = eps_mempool_free(pHandle->llMempoolId, pLocalMem);
}
#endif

/*---------------------------------------------------------------------------*/
#ifndef EDDI_IOCC_FATAL_ERROR
LSA_VOID  EDDI_IOCC_FATAL_ERROR(LSA_UINT16                     Length,
                                EDDI_IOCC_FATAL_ERROR_PTR_TYPE pError)
{
    EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_LSA_FATAL, EPS_EXIT_CODE_LSA_FATAL, 0, Length, pError, 0, 0, 0, 0);
}
#endif 

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
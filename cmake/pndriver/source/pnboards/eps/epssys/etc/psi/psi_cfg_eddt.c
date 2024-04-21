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
/*  C o m p o n e n t     &C: EPS (Embedded Profinet System)            :C&  */
/*                                                                           */
/*  F i l e               &F: psi_cfg_eddt.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDDT HW system adaption                            */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32006
#define LTRC_ACT_MODUL_ID  32006

#include <eps_sys.h>
#include <psi_inc.h>

#if ( PSI_CFG_USE_EDDT == 1 )
/*----------------------------------------------------------------------------*/

#include <eps_cp_hw.h>
#include <psi_trc.h>
#include <eps_isr.h>
#include <eps_hw_pnip.h>
#include <eddt_inc.h>

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_ALLOC_UPPER_RX_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	// allocate NRT buffer from NRT memory pool
	PSI_NRT_ALLOC_RX_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_ALLOC_UPPER_RX_MEM(): ptr(%08x) size(%u)", 
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_FREE_UPPER_RX_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_FREE_UPPER_RX_MEM(): ptr(%08x)", 
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_ALLOC_UPPER_TX_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	// allocate NRT buffer from NRT memory pool
	PSI_NRT_ALLOC_TX_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_ALLOC_UPPER_TX_MEM(): ptr(%08x) size(%u)", 
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_FREE_UPPER_TX_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_FREE_UPPER_TX_MEM(): ptr(%08x)",
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_ALLOC_UPPER_DEV_MEM
LSA_VOID PSI_EDDT_ALLOC_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_DEV_ALLOC_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_ALLOC_UPPER_DEV_MEM(): ptr(%08x) size(%u)",
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_FREE_UPPER_DEV_MEM
LSA_VOID PSI_EDDT_FREE_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_FREE_UPPER_DEV_MEM(): ptr(%08x)",
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_ALLOC_LOWER_DEV_MEM
LSA_VOID PSI_EDDT_ALLOC_LOWER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_DEV_ALLOC_MEM( lower_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_ALLOC_LOWER_DEV_MEM(): ptr(%08x) size(%u)",
		*lower_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_FREE_LOWER_DEV_MEM
LSA_VOID PSI_EDDT_FREE_LOWER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr )
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDT );

	PSI_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDT_FREE_LOWER_DEV_MEM(): ptr(%08x)",
		lower_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, lower_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDT );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/
#ifndef PSI_EDDT_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDT_SIGNAL_SENDCLOCK_CHANGE(
    PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     CycleBaseFactor,
	LSA_UINT8      Mode )
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	PSI_ASSERT( pSys != 0 );

	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDT );

	p_board = eps_hw_get_board_store( pSys->hd_nr );	

	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddt.is_valid );

	p_board->eddt.cycle_base_factor = CycleBaseFactor;

	LSA_UNUSED_ARG( Mode );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDT

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

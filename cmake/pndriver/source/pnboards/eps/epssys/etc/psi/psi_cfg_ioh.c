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
/*  F i l e               &F: psi_cfg_ioh.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI IOH system adaption                                */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID     32009
#define LTRC_ACT_MODUL_ID 32009

#include "eps_sys.h"

#include "eps_isr.h"
#include "eps_cp_hw.h"

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if (( PSI_CFG_USE_IOH == 1 ) && (PSI_CFG_USE_LD_COMP == 1))
/*----------------------------------------------------------------------------*/
PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*----------------------------------------------------------------------------*/

#ifndef PSI_IOH_ALLOC_FAST_MEM
LSA_VOID PSI_IOH_ALLOC_FAST_MEM(
    LSA_VOID   ** local_mem_ptr_ptr,
    LSA_UINT32    length )
{
	#if  (EPS_PLF == EPS_PLF_PNIP_ARM9)
	*local_mem_ptr_ptr = eps_fast_mem_alloc( length, LSA_COMP_ID_IOH, 0 );
	#else
	LSA_USER_ID_TYPE user_id;
	LSA_SYS_PTR_TYPE sys_ptr = 0;
	
	user_id.uvar32 = 0;
	PSI_ALLOC_LOCAL_MEM( local_mem_ptr_ptr, user_id, length, sys_ptr, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM );
	#endif
}
#else
#error "by design a function!"
#endif

#ifndef PSI_IOH_FREE_FAST_MEM
LSA_VOID PSI_IOH_FREE_FAST_MEM(
    LSA_UINT16 * ret_val_ptr,
    LSA_VOID   * local_mem_ptr )
{
	#if (EPS_PLF == EPS_PLF_PNIP_ARM9)
    *ret_val_ptr = eps_fast_mem_free( local_mem_ptr );
	#else
	LSA_SYS_PTR_TYPE sys_ptr;
	sys_ptr = 0;
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, sys_ptr, LSA_COMP_ID_IOH, PSI_MTYPE_LOCAL_MEM );
	#endif
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_IOH_ENABLE_ISOCHRONOUS
LSA_RESULT PSI_IOH_ENABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE  hSysDev,
	LSA_VOID (*func_ptr)(LSA_INT params, LSA_VOID* args),
	LSA_INT32       param,
	LSA_VOID *      args )
{
	return eps_enable_iso_interrupt( hSysDev, func_ptr, param, args );
}
#else
#error "by design a function!"
#endif

#ifndef PSI_IOH_DISABLE_ISOCHRONOUS
LSA_RESULT PSI_IOH_DISABLE_ISOCHRONOUS(
	PSI_SYS_HANDLE hSysDev )
{
	return eps_disable_iso_interrupt( hSysDev );
}
#else
#error "by design a function!"
#endif


/*----------------------------------------------------------------------------*/

#ifndef PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_IOH_DEV_MEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE         hSysDev,   //device info
	LSA_VOID       const*  p,         //virtual address
    LSA_BOOL               bShared )  //LSA_TRUE: calculated for SharedMEM, LSA_FALSE: calculated for LocalMEM
{
	LSA_UINT32              asic_adr;
	EPS_BOARD_INFO_PTR_TYPE p_board;
    PSI_SYS_PTR_TYPE        pSys = (PSI_SYS_PTR_TYPE)hSysDev;
    PSI_ASSERT(pSys != 0);

    if(( psi_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC ) && pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
        p_board = eps_hw_get_board_store( 0 );
    }
    else
    {
        p_board = eps_hw_get_board_store( pSys->hd_nr );
    }

	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );

    if (bShared)
    {
	    asic_adr = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.shared_mem.base_ptr);
	    asic_adr += p_board->eddi.shared_mem.phy_addr;
    }
    else
    {
	    asic_adr = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.sdram.base_ptr);
	    asic_adr += p_board->eddi.sdram.phy_addr;
    }

	return asic_adr;
}
#else
#error "by design a function!"
#endif

#ifndef PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY
LSA_BOOL PSI_IOH_DEV_IS_IOCC_ACCESSED_LOCALLY(
    PSI_SYS_HANDLE hSysDev,        //device info
    LSA_UINT32     iocc_base_addr )
{
	EPS_BOARD_INFO_PTR_TYPE  p_board;

	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

    if ((psi_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC) && pSys->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
        p_board = eps_hw_get_board_store(0);
    }
    else
    {
        p_board = eps_hw_get_board_store(pSys->hd_nr);
    }

	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );

    if (p_board->eddi.iocc.phy_addr == (iocc_base_addr & ~0xA0000000))
    {
        return LSA_TRUE;
    }
    else
    {
        return LSA_FALSE;
    }
}
#else
#error "by design a function!"
#endif

#endif // (( PSI_CFG_USE_IOH == 1 ) && (PSI_CFG_USE_LD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

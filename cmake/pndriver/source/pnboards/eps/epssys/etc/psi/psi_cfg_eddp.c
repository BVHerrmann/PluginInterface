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
/*  F i l e               &F: psi_cfg_eddp.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDDP HW system adaption                            */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32003
#define LTRC_ACT_MODUL_ID  32003

#include <eps_sys.h>
#include <eps_trc.h>

#if (( PSI_CFG_USE_EDDP == 1 ) && (PSI_CFG_USE_HD_COMP == 1))
/*----------------------------------------------------------------------------*/

#include <eps_cp_hw.h>
#include "eps_register.h"

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*---------------------------------------------------------------------------*/
/* Currently not used
#ifndef PSI_EDDP_WAIT_10_NS
LSA_VOID PSI_EDDP_WAIT_10_NS(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     ticks_10ns)
{
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(ticks_10ns);

    // TBD.
}
#else
#error "by design a function!"
#endif
*/
/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_ALLOC_UPPER_RX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	// allocate NRT buffer from NRT memory pool
	PSI_NRT_ALLOC_RX_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_ALLOC_UPPER_RX_MEM(): ptr(0x%08x) size(%u)", 
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_FREE_UPPER_RX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_RX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16          *ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_FREE_UPPER_RX_MEM(): ptr(0x%08x)", 
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_ALLOC_UPPER_TX_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	// allocate NRT buffer from NRT memory pool
	PSI_NRT_ALLOC_TX_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_ALLOC_UPPER_TX_MEM(): ptr(0x%08x) size(%u)", 
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_FREE_UPPER_TX_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_TX_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_FREE_UPPER_TX_MEM(): ptr(0x%08x)",
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_ALLOC_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_ALLOC_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE * upper_mem_ptr_ptr,
	LSA_UINT32          length)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	PSI_DEV_ALLOC_MEM( upper_mem_ptr_ptr, length, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_02( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_ALLOC_UPPER_DEV_MEM(): ptr(0x%08x) size(%u)",
		*upper_mem_ptr_ptr, length );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_FREE_UPPER_DEV_MEM
LSA_VOID PSI_EDDP_FREE_UPPER_DEV_MEM(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   upper_mem_ptr)
{
	EPS_SYS_PTR_TYPE pSysPtr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSysPtr != 0 );
	PSI_ASSERT( pSysPtr->edd_comp_id == LSA_COMP_ID_EDDP );

	EPS_SYSTEM_TRACE_01( pSysPtr->pnio_if_nr, LSA_TRACE_LEVEL_CHAT,
		"PSI_EDDP_FREE_UPPER_DEV_MEM(): ptr(0x%08x)",
		upper_mem_ptr );

	PSI_DEV_FREE_MEM( ret_val_ptr, upper_mem_ptr, pSysPtr->hd_nr, LSA_COMP_ID_EDDP );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_RX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                pnip_addr;
	LSA_UINT8               * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE   p_board;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( hSysDev != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	base_ptr = (LSA_UINT8*)p_board->eddp.sdram_NRT.base_ptr;

	PSI_ASSERT( (LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr );

	pnip_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32) base_ptr);
	pnip_addr += p_board->eddp.sdram_NRT.phy_addr;
	*pAddr    = pnip_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_TX_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                pnip_addr;
	LSA_UINT8               * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE   p_board;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	base_ptr = (LSA_UINT8*)p_board->eddp.sdram_NRT.base_ptr;

	PSI_ASSERT( (LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr );

	pnip_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32) base_ptr);
	pnip_addr += p_board->eddp.sdram_NRT.phy_addr;
	*pAddr    = pnip_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_DEV_MEM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                pnip_addr;
	LSA_UINT8               * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE   p_board;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	base_ptr = (LSA_UINT8*)p_board->eddp.sdram_NRT.base_ptr;

	PSI_ASSERT( (LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr );

	pnip_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32) base_ptr);
	pnip_addr += p_board->eddp.sdram_NRT.phy_addr;
	*pAddr    = pnip_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_EXTRAM_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                pnip_addr;
	LSA_UINT8               * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE   p_board;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	base_ptr = (LSA_UINT8*)p_board->eddp.sdram_CRT.base_ptr;

	PSI_ASSERT( (LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr );

	pnip_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32) base_ptr);
	pnip_addr += p_board->eddp.sdram_CRT.phy_addr;
	*pAddr    = pnip_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP
LSA_VOID PSI_EDDP_IO_PERIF_ADDR_UPPER_TO_PNIP(
	PSI_SYS_HANDLE      hSysDev,
	LSA_VOID_PTR_TYPE   Ptr,
	LSA_UINT32        * pAddr)
{
	LSA_UINT32                pnip_addr;
	LSA_UINT8               * base_ptr;
	EPS_BOARD_INFO_PTR_TYPE   p_board;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	base_ptr = (LSA_UINT8*)p_board->eddp.perif_ram.base_ptr;

	PSI_ASSERT( (LSA_UINT32)Ptr >= (LSA_UINT32)base_ptr );

	pnip_addr = ((LSA_UINT32)Ptr) - ((LSA_UINT32) base_ptr);
	pnip_addr += p_board->eddp.perif_ram.phy_addr;
	*pAddr    = pnip_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_TEST_WRITE_REG
LSA_VOID PSI_EDDP_TEST_WRITE_REG(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     Reg,
	LSA_UINT32     Value)
{
	// Dummy with PNIP HW
	LSA_UNUSED_ARG( hSysDev );
	LSA_UNUSED_ARG( Reg );
	LSA_UNUSED_ARG( Value );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDP_K32_RESET
LSA_VOID PSI_EDDP_K32_RESET(
	PSI_SYS_HANDLE hSysDev,
	LSA_BOOL       on)
{
	EPS_BOARD_INFO_PTR_TYPE p_board;

	volatile LSA_UINT32 * pAPB_SCRB_ASYN_RES_CTRL_REG;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	PSI_ASSERT( sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	// get pointer to ASYN_RES_CTRL_REG with offset 0x0C in SCRB
	// bit 5: reset K32 core system
    PSI_ASSERT(p_board->eddp.apb_periph_scrb.base_ptr != 0);
	pAPB_SCRB_ASYN_RES_CTRL_REG = (volatile LSA_UINT32*) EPS_CAST_TO_VOID_PTR(p_board->eddp.apb_periph_scrb.base_ptr + 0x0C);
	
	if( on )
	{
		*pAPB_SCRB_ASYN_RES_CTRL_REG |= 0x20ul;
	}
	else
	{
		*pAPB_SCRB_ASYN_RES_CTRL_REG &= ~0x20ul;
	}
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
/*          NEC PHY function prototype for System adaptation                 */
/*===========================================================================*/

#ifndef EDDP_CFG_PHY_NEC_MAU_TYPE_INTERNAL
LSA_VOID PSI_EDDP_PHY_NEC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16            * pMauType)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(Speed);
    LSA_UNUSED_ARG(Duplexity);
    LSA_UNUSED_ARG(*pMauType);
}

LSA_VOID PSI_EDDP_PHY_NEC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32            * pSpeed,
    LSA_UINT32            * pDuplexity )
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(MAUType);
    LSA_UNUSED_ARG(*pSpeed);
    LSA_UNUSED_ARG(*pDuplexity);
}
#endif  // EDDP_CFG_PHY_NEC_MAU_TYPE_INTERNAL

/*---------------------------------------------------------------------------*/

#ifndef EDDP_CFG_PHY_NEC_LED_BLINK_INTERNAL
LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkBegin(
	PSI_EDD_HDDB 	    hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
}

LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(LEDMode);
}

LSA_VOID PSI_EDDP_PHY_NEC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
}
#endif //EDDP_CFG_PHY_NEC_LED_BLINK_INTERNAL

/*===========================================================================*/
/*          NSC PHY function prototype for System adaptation                 */
/*===========================================================================*/

#ifndef EDDP_CFG_PHY_NSC_MAU_TYPE_INTERNAL
LSA_VOID PSI_EDDP_PHY_NSC_GET_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT32              Speed,
    LSA_UINT32              Duplexity,
    LSA_UINT16            * pMauType)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(Speed);
    LSA_UNUSED_ARG(Duplexity);
    LSA_UNUSED_ARG(*pMauType);
}

LSA_VOID PSI_EDDP_PHY_NSC_CHECK_MAU_TYPE(
    PSI_EDD_HDDB            hDDB,
    PSI_SYS_HANDLE          hSysDev,
    LSA_UINT32              HwPortID,
    LSA_UINT16              MAUType,
    LSA_UINT32            * pSpeed,
    LSA_UINT32            * pDuplexity)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(MAUType);
    LSA_UNUSED_ARG(*pSpeed);
    LSA_UNUSED_ARG(*pDuplexity);
}
#endif  // EDDP_CFG_PHY_NSC_MAU_TYPE_INTERNAL

/*---------------------------------------------------------------------------*/

#ifndef EDDP_CFG_PHY_NSC_LED_BLINK_INTERNAL
LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkBegin(
    PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
}

LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkSetMode(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode)
{
    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
    LSA_UNUSED_ARG(LEDMode);
}

LSA_VOID PSI_EDDP_PHY_NSC_LED_BlinkEnd(
	PSI_EDD_HDDB        hDDB,
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          HwPortID)
{

    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(HwPortID);
}
#endif // EDDP_CFG_PHY_NSC_LED_BLINK_INTERNAL

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDP_SIGNAL_SENDCLOCK_CHANGE(
	PSI_SYS_HANDLE      hSysDev,
	LSA_UINT32          CycleBaseFactor,
	LSA_UINT8           Mode)
{
	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	PSI_ASSERT( pSys != 0 );

	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDP );

	p_board = eps_hw_get_board_store( pSys->hd_nr );	

	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	p_board->eddp.cycle_base_factor = CycleBaseFactor;

	LSA_UNUSED_ARG( Mode );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDP_I2C_SELECT
LSA_VOID EDDP_SYSTEM_OUT_FCT_ATTR PSI_EDDP_I2C_SELECT( 
	LSA_UINT8 EDDP_LOCAL_MEM_ATTR * const ret_val_ptr,
	EDDP_SYS_HANDLE                const hSysDev,
	LSA_UINT16                     const PortId,
	LSA_UINT16                     const I2CMuxSelect)
{
	LSA_UNUSED_ARG(hSysDev);
	LSA_UNUSED_ARG(PortId);
	LSA_UNUSED_ARG(I2CMuxSelect);

	*ret_val_ptr = EDD_I2C_DEVICE_ACTIVATED;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDP_K32_RELEASE_RESET_HERA
LSA_VOID EDDP_SYSTEM_OUT_FCT_ATTR PSI_EDDP_K32_RELEASE_RESET_HERA(
	EDDP_SYS_HANDLE const hSysDev)
{
	EPS_BOARD_INFO_PTR_TYPE p_board;

	volatile LSA_UINT32 * pAPB_SCRB_REG;

	EPS_SYS_PTR_TYPE sys_ptr = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( sys_ptr != 0 );

	p_board = eps_hw_get_board_store( sys_ptr->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddp.is_valid );

	pAPB_SCRB_REG = (volatile LSA_UINT32*)EPS_CAST_TO_VOID_PTR(p_board->eddp.apb_periph_sec_scrb.base_ptr + 0x18);
	
    if //Interface A?
       (p_board->eddp.hw_interface == EDDP_HW_IF_A)
    {
         *pAPB_SCRB_REG |= (1<<8); // Keep CR4_A not in HALT state
    }
    else //Interface B
    {
         *pAPB_SCRB_REG |= (1<<16); // Keep CR4_B not in HALT state
    }
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/
#endif // PSI_CFG_USE_EDDP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

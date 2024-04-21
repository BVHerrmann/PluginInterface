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
/*  F i l e               &F: psi_cfg_eddi.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of PSI EDDI HW system adaption.                           */
/*                                                                           */
/*****************************************************************************/

#define PSI_MODULE_ID      32002
#define LTRC_ACT_MODUL_ID  32002

#include <eps_sys.h>

#if (( PSI_CFG_USE_EDDI == 1 ) && (PSI_CFG_USE_HD_COMP == 1))
/*----------------------------------------------------------------------------*/

#include <eps_rtos.h>
#include <eps_cp_hw.h>

#include <eps_trc.h>        /* Tracing              */
#include <eps_plf.h>
#include <eps_app.h>        /* EPS APP prototypes   */

#if (defined (PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_FLEX))
#include "eps_isr.h"
#endif

#if ( PSI_CFG_USE_PNTRC == 1 )
#include <pntrc_inc.h>      /* PNTRC Interface      */
#endif

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#ifndef PSI_EDDI_LL_XPLL_SETPLLMODE_OUT
LSA_VOID PSI_EDDI_LL_XPLL_SETPLLMODE_OUT(
	LSA_RESULT     * result,
	PSI_SYS_HANDLE   hSysDev,
	LSA_UINT32       pIRTE,
	LSA_UINT32       location,
	LSA_UINT16       pllmode )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_SET_PLLPORT_FCT     pll_fct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );

	pll_fct = p_board->eddi.set_pll_port_fct;

	if (( p_board->eddi.has_ext_pll ) &&
		( pll_fct != 0 ))               // EXT_PLL supported ?
	{
		switch ( pllmode )
		{
		case EDDI_PRM_DEF_PLL_MODE_OFF:
			{
				*result = (pll_fct)( pSys->hd_nr, pIRTE, location, pllmode, p_board->eddi.extpll_out_gpio_nr );
			}
			break;
		case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT:
			{
				*result = (pll_fct)( pSys->hd_nr, pIRTE, location, pllmode, p_board->eddi.extpll_out_gpio_nr );
			}
			break;
		case EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN:
			{
				*result = (pll_fct)( pSys->hd_nr, pIRTE, location, pllmode, p_board->eddi.extpll_in_gpio_nr );
			}
			break;
		default:
			{
				*result = EDD_STS_ERR_PARAM;
			}
			break;
		}
	}
	else
	{
		*result = EDD_STS_ERR_NOT_SUPPORTED;
	}
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_BEGIN
LSA_VOID PSI_EDDI_LL_LED_BLINK_BEGIN(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     HwPortIndex,
	LSA_UINT32     PhyTransceiver )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_PHY_LED_BLINK_FCT   start_fct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( HwPortIndex < EDD_CFG_MAX_PORT_CNT );

	// note: only called by EDDI if PHY led function external exists (i.e.: EB200)
	start_fct = p_board->eddi.blink_start_fct;
	PSI_ASSERT( start_fct != 0 );

	( start_fct )( hSysDev, HwPortIndex, PhyTransceiver );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_END
LSA_VOID PSI_EDDI_LL_LED_BLINK_END(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     HwPortIndex,
	LSA_UINT32     PhyTransceiver )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_PHY_LED_BLINK_FCT   end_fct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( HwPortIndex < EDD_CFG_MAX_PORT_CNT );

	// note: only called by EDDI if PHY led function external exists (i.e.: EB200)
	end_fct = p_board->eddi.blink_end_fct;
	PSI_ASSERT( end_fct != 0 );

	( end_fct )( hSysDev, HwPortIndex, PhyTransceiver );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_LED_BLINK_SET_MODE
LSA_VOID PSI_EDDI_LL_LED_BLINK_SET_MODE(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     HwPortIndex,
	LSA_UINT32     PhyTransceiver,
	LSA_UINT16     LEDMode )
{
	EPS_BOARD_INFO_PTR_TYPE    p_board;
	EPS_PHY_LED_BLINK_MODE_FCT set_mode_fct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( HwPortIndex < EDD_CFG_MAX_PORT_CNT );
	PSI_ASSERT( (LEDMode == EDDI_LED_MODE_ON) || (LEDMode == EDDI_LED_MODE_OFF));

	// note: only called by EDDI if PHY led function external exists (i.e.: EB200)
	set_mode_fct = p_board->eddi.blink_set_mode_fct;
	PSI_ASSERT( set_mode_fct != 0 );

	( set_mode_fct )( hSysDev, HwPortIndex, PhyTransceiver, LEDMode );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_LOCAL_TO_ASIC_DMA(
	PSI_SYS_HANDLE         hSysDev,
	LSA_VOID       const*  p,
	LSA_UINT32     const   location )
{
	LSA_UINT32              asic_dma;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	LSA_UNUSED_ARG(location);

	//!!!!!!!!!!!!!!!!!!!
	// Attention: this example assumes that the TCW is located in KRAM.
	// If located elsewhere, the calculation from
	// virtual to AHB-address has to be adapted!
	//!!!!!!!!!!!!!!!!!!!

	asic_dma = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.irte.base_ptr);
	asic_dma += p_board->eddi.irte.phy_addr;

	// KRAM-Address (AHB-Adress)
	return( asic_dma );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_LOCRAM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE         hSysDev,
	LSA_VOID        const* p,
	LSA_UINT32      const  location )
{
	LSA_UINT32              ahb_adr;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	LSA_UNUSED_ARG(location);

    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
	if((LSA_UINT8*)p >= p_board->eddi.sdram_cached.base_ptr &&
	   (LSA_UINT8*)p <= p_board->eddi.sdram_cached.base_ptr + p_board->eddi.sdram_cached.size )
	{
	    ahb_adr = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.sdram_cached.base_ptr);
        ahb_adr += p_board->eddi.sdram_cached.phy_addr;
	}
	else
	#endif // PSI_CFG_USE_NRT_CACHE_SYNC
	{
	    ahb_adr = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.sdram.base_ptr);
	    ahb_adr += p_board->eddi.sdram.phy_addr;
	}
	
	// AHB-Address (IRTE-sight)
	return( ahb_adr );
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_ADR_LOCAL_TO_ASIC(
	PSI_SYS_HANDLE       hSysDev,
	LSA_VOID             const* p,
	LSA_UINT32           const  location )
{
	LSA_UINT32              asic_adr;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	LSA_UNUSED_ARG(location);

	asic_adr = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.shared_mem.base_ptr);
	asic_adr += p_board->eddi.shared_mem.phy_addr;

	return asic_adr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_SHAREDMEM_OFFSET
LSA_UINT32 PSI_EDDI_DEV_SHAREDMEM_OFFSET(
	PSI_SYS_HANDLE         hSysDev,
	LSA_VOID        const* p,
	LSA_UINT32      const  location )
{
	LSA_UINT32              offset;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	LSA_UNUSED_ARG(location);

	offset = (LSA_UINT32)((LSA_UINT8*)p - p_board->eddi.shared_mem.base_ptr);

	return offset;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL
LSA_UINT32 PSI_EDDI_DEV_KRAM_ADR_ASIC_TO_LOCAL(
	PSI_SYS_HANDLE       hSysDev,
	LSA_UINT32     const p,
	LSA_UINT32     const location )
{
	LSA_UINT32              virt_addr;
	EPS_BOARD_INFO_PTR_TYPE p_board;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	LSA_UNUSED_ARG(location);

	virt_addr = (LSA_UINT32)p_board->eddi.irte.base_ptr + p;

	// virtual addr
	return virt_addr;
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE
LSA_VOID PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE(
	PSI_SYS_HANDLE hSysDev,
	LSA_UINT32     CycleBaseFactor,
	LSA_UINT8      Mode )
{
	EPS_SYS_PTR_TYPE        pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	EPS_BOARD_INFO_PTR_TYPE p_board;
	LSA_RESULT				result;
	LSA_UINT32				eddi_location;

    PSI_ASSERT( pSys );

	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );

    p_board = eps_hw_get_board_store( pSys->hd_nr );	

	PSI_ASSERT( p_board );
	PSI_ASSERT( p_board->eddi.is_valid );

    p_board->eddi.cycle_base_factor = CycleBaseFactor;

	switch (p_board->board_type)
	{
		case EPS_PNDEV_BOARD_EB200:
		case EPS_PNDEV_BOARD_EB200_PCIE:
			eddi_location = EDDI_LOC_ERTEC200;
			break;
		case EPS_PNDEV_BOARD_CP1616:
		case EPS_PNDEV_BOARD_EB400_PCIE:
			eddi_location = EDDI_LOC_ERTEC400;
			break;
		case EPS_PNDEV_BOARD_SOC1_PCI:
		case EPS_PNDEV_BOARD_SOC1_PCIE:
		case EPS_PNDEV_BOARD_CP1625:
			eddi_location = EDDI_LOC_SOC1;
			break;

		default:
			EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE called for invalid boardtype(%u)", p_board->board_type);
			return;
	}

	PSI_EDDI_LL_XPLL_SETPLLMODE_OUT(&result, hSysDev, (LSA_UINT32)p_board->eddi.irte.base_ptr, eddi_location, Mode);
	if (result != EDD_STS_OK)
	{
		EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_WARN, "PSI_EDDI_SIGNAL_SENDCLOCK_CHANGE: PSI_EDDI_LL_XPLL_SETPLLMODE_OUT failed with result(%u)", result);
	}
}
#else
#error "by design a function!"
#endif

/*---------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED
LSA_VOID PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED( 
	PSI_EDD_HDDB hDDB )
{
	LSA_UNUSED_ARG( hDDB );

	/* This macro is called from the EDDI during device open.
	 * After this macro is called, we may use the user interrupts (e.g. TransferEnd) by accessing the IRTE-IP.
	 * We have no isochonous application in the EPS currently. We'll only do a trace here.
	 * See EDDI_SYS.doc for further information.
	 */
	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
	        "PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STARTED called");
}
#else
#error "by design a function!"
    #endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED
LSA_VOID PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED( 
    PSI_EDD_HDDB hDDB )
{
    LSA_UNUSED_ARG( hDDB );
    
    /* This macro is called from the EDDI during device close.
     * After this macro is called, we may no longer use the user interrupts (e.g. TransferEnd) by accessing the IRTE-IP.
     * We have no isochonous application in the EPS currently. We'll only do a trace here.
     * See EDDI_SYS.doc for further information.
     */
    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE_HIGH,
            "PSI_EDDI_SII_USER_INTERRUPT_HANDLING_STOPPED called");
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SCL_LOW_HIGHZ
LSA_VOID PSI_EDDI_I2C_SCL_LOW_HIGHZ(
	PSI_SYS_HANDLE       hSysDev,
	LSA_UINT8      const Level )
{
	EPS_BOARD_INFO_PTR_TYPE  p_board;
	EPS_I2C_SET_LOW_HIGH_FCT setFct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	setFct = p_board->eddi.i2c_set_scl_low_highz;
	PSI_ASSERT( setFct != 0 );

	// Execute PN-Board specific I2C handling
	( setFct )( hSysDev, Level );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_LOW_HIGHZ
LSA_VOID PSI_EDDI_I2C_SDA_LOW_HIGHZ(
	PSI_SYS_HANDLE       hSysDev,
    LSA_UINT8      const Level )
{
	EPS_BOARD_INFO_PTR_TYPE  p_board;
	EPS_I2C_SET_LOW_HIGH_FCT setFct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	setFct = p_board->eddi.i2c_set_sda_low_highz;
	PSI_ASSERT( setFct != 0 );

	// Execute PN-Board specific I2C handling
	( setFct )( hSysDev, Level );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SDA_READ
LSA_VOID PSI_EDDI_I2C_SDA_READ(
	PSI_SYS_HANDLE         hSysDev,
	LSA_UINT8      * const value_ptr )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_I2C_SDA_READ_FCT    readFct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );

	readFct = p_board->eddi.i2c_sda_read;
	PSI_ASSERT( readFct != 0 );

	// Execute PN-Board specific I2C handling
	*value_ptr = ( readFct )( hSysDev );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_I2C_SELECT
LSA_VOID PSI_EDDI_I2C_SELECT(
    LSA_UINT8       * const ret_val_ptr,
	EDDI_SYS_HANDLE   const hSysDev,
	LSA_UINT16        const PortId,
	LSA_UINT16        const I2CMuxSelect )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_I2C_SELECT_FCT      i2cSelectFct;

	EPS_SYS_PTR_TYPE pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( (PortId) && (PortId <= p_board->nr_of_ports) );
	PSI_ASSERT( (I2CMuxSelect) && (I2CMuxSelect <= PSI_CFG_MAX_PORT_CNT) );

	i2cSelectFct = p_board->eddi.i2c_select;
	PSI_ASSERT( i2cSelectFct != 0 );

	// Execute PN-Board specific I2C handling
	*ret_val_ptr = (( i2cSelectFct )( hSysDev, PortId, I2CMuxSelect )) ? EDD_I2C_DEVICE_ACTIVATED : EDD_I2C_DEVICE_NOT_ACTIVATED;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_LL_I2C_WRITE_OFFSET_SOC(
	LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
	EDDI_SYS_HANDLE                     const  hSysDev,
	LSA_UINT8                           const  I2CDevAddr,
	LSA_UINT8                           const  I2COffsetCnt,
	LSA_UINT8                           const  I2COffset1,
	LSA_UINT8                           const  I2COffset2,
	LSA_UINT32                          const  Size,
	LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
	EPS_BOARD_INFO_PTR_TYPE       p_board;
	EPS_SYS_PTR_TYPE        const pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( p_board->eddi.i2c_ll_read_offset != 0 );

	// Execute PN-Board specific I2C write handling
	*ret_val_ptr = ( p_board->eddi.i2c_ll_write_offset )( hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_LL_I2C_READ_OFFSET_SOC
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_LL_I2C_READ_OFFSET_SOC( 
	LSA_RESULT  EDDI_LOCAL_MEM_ATTR  *  const  ret_val_ptr,
	EDDI_SYS_HANDLE                     const  hSysDev,
	LSA_UINT8                           const  I2CDevAddr,
	LSA_UINT8                           const  I2COffsetCnt,
	LSA_UINT8                           const  I2COffset1,
	LSA_UINT8                           const  I2COffset2,
	LSA_UINT32                          const  Size,
	LSA_UINT8  EDD_UPPER_MEM_ATTR    *  const  pBuf )
{
	EPS_BOARD_INFO_PTR_TYPE p_board;
	EPS_SYS_PTR_TYPE  const  pSys = (EPS_SYS_PTR_TYPE)hSysDev;
	PSI_ASSERT( pSys != 0 );

	p_board = eps_hw_get_board_store( pSys->hd_nr );
	PSI_ASSERT( p_board != 0 );
	PSI_ASSERT( pSys->edd_comp_id == LSA_COMP_ID_EDDI );
	PSI_ASSERT( p_board->eddi.is_valid );
	PSI_ASSERT( p_board->eddi.i2c_ll_read_offset != 0 );

	// Execute PN-Board specific I2C read handling
	*ret_val_ptr = ( p_board->eddi.i2c_ll_read_offset )( hSysDev, I2CDevAddr, I2COffsetCnt, I2COffset1, I2COffset2, Size, pBuf );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM( 
    PSI_SYS_HANDLE                              hSysDev,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID)
{
    PSI_ASSERT( UserMemID != (EDDI_USERMEMID_TYPE)0 );
    PSI_ASSERT( UserMemID->Alloc != 0 );
    
    *lower_mem_ptr_ptr = UserMemID->Alloc( length, hSysDev, LSA_COMP_ID_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_FREE_DEV_SDRAM_ERTEC_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_FREE_DEV_SDRAM_ERTEC_MEM( 
    PSI_SYS_HANDLE                              hSysDev,
    LSA_UINT16  EDDI_LOCAL_MEM_ATTR *           ret_val_ptr,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID)
{
    PSI_ASSERT( UserMemID != (EDDI_USERMEMID_TYPE)0 );
    PSI_ASSERT( UserMemID->Free != 0 );

    UserMemID->Free( (LSA_UINT8 *)lower_mem_ptr, hSysDev, LSA_COMP_ID_EDD );

    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_ALLOC_DEV_SHARED_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_ALLOC_DEV_SHARED_MEM(
    PSI_SYS_HANDLE                              hSysDev,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr_ptr,
    LSA_UINT32                                  length,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID)
{
    PSI_ASSERT( UserMemID != (EDDI_USERMEMID_TYPE)0 );
    PSI_ASSERT( UserMemID->Alloc != 0 );

    *lower_mem_ptr_ptr = UserMemID->Alloc( length, hSysDev, LSA_COMP_ID_EDD );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PSI_EDDI_FREE_DEV_SHARED_MEM
LSA_VOID EDDI_SYSTEM_OUT_FCT_ATTR PSI_EDDI_FREE_DEV_SHARED_MEM(
    PSI_SYS_HANDLE                              hSysDev,
    LSA_UINT16  EDDI_LOCAL_MEM_ATTR *           ret_val_ptr,
    LSA_VOID_PTR_TYPE *                         lower_mem_ptr,
    PSI_USR_NRTMEM_ID_FKT_CONST_PTR_TYPE        UserMemID)
{
    PSI_ASSERT( UserMemID != (EDDI_USERMEMID_TYPE)0 );
    PSI_ASSERT( UserMemID->Free != 0 );

    UserMemID->Free( (LSA_UINT8 *)lower_mem_ptr, hSysDev, LSA_COMP_ID_EDD );

    *ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif
/*=============================================================================
 * function name:  PSI_EDDI_EXT_TIMER_CMD()
 *
 * function:       initializes the timer-handling for 1 TIMERTOP-Timer
 *===========================================================================*/
LSA_VOID PSI_EDDI_EXT_TIMER_CMD(
    PSI_SYS_HANDLE const hSysDev,
    LSA_UINT8      const Cmd,
    PSI_EDD_HDDB   const hDDB )
{
    #if !defined (PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON) && !defined (PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    LSA_UNUSED_ARG(hSysDev);
    LSA_UNUSED_ARG(Cmd);
    LSA_UNUSED_ARG(hDDB);
    #else
	switch(Cmd)
	{
		case PSI_EDDI_EXTTIMER_CMD_INIT_START:
			eps_eddi_exttimer_start(hSysDev);
			break;
		case PSI_EDDI_EXTTIMER_CMD_STOP:
			eps_eddi_exttimer_stop(hSysDev);
			break;
		default: 
			PSI_FATAL(0);
			break;
	}
    #endif
}

/*----------------------------------------------------------------------------*/
#endif /* PSI_CFG_USE_EDDI */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

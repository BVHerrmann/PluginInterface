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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_cp_hw.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS is a framework to integrate the LSA-modules                          */
/*  of the PN IO runtime stack on an embedded platform.                      */
/*                                                                           */
/*  PN Board specific functions and storage used for EDDx HW adaption        */
/*****************************************************************************/

#define EPS_MODULE_ID      20002
#define LTRC_ACT_MODUL_ID  20002

#include "eps_sys.h"
#include "eps_cp_hw.h"
#include "eps_locks.h"
#include "eps_trc.h"
#include "eps_app.h"

#include "eps_pn_drv_if.h"   /* PN-Driver Interface */

#if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)  
#include "eps_elf_loader.h"
#include "eps_c66x_bootloader.h"  /* EPS c66 bootloader for am5728  */
#endif

EPS_FILE_SYSTEM_EXTENSION(EPS_MODULE_ID)

/*----------------------------------------------------------------------------*/

static EPS_BOARD_INFO_TYPE board_list[PSI_CFG_MAX_IF_CNT+1];

/*----------------------------------------------------------------------------*/

static struct {
	LSA_UINT16               enter_exit;
	EPS_BOARD_INFO_PTR_TYPE  p_list;
} g_Eps_boards;

/*----------------------------------------------------------------------------*/
/** 
 * Initilizes the board storage structure.
 * 
 * @see eps_init        - calls this function.
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hw_init( LSA_VOID )
{
    LSA_RESPONSE_TYPE retVal;
	retVal = eps_alloc_critical_section(&g_Eps_boards.enter_exit, LSA_FALSE);
    EPS_ASSERT( LSA_RET_OK == retVal );
	eps_memset(board_list, 0, sizeof(board_list));

	g_Eps_boards.p_list = board_list;

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_hw_init(): Board HW init done, max boards(%u)", PSI_CFG_MAX_IF_CNT );
}

/*----------------------------------------------------------------------------*/
/**
 * Deletes the board storage. Sets the pointer to invalid.
 * 
 * @see eps_undo_init - calls this function
 * @param LSA_VOID
 * @return LSA_VOID
 */
LSA_VOID eps_hw_undo_init( LSA_VOID )
{
    LSA_RESPONSE_TYPE retVal;
	EPS_ASSERT( g_Eps_boards.p_list ); // Initialized?

	g_Eps_boards.p_list = 0;

	eps_memset(board_list, 0, sizeof(board_list));

	retVal = eps_free_critical_section(g_Eps_boards.enter_exit);

    EPS_ASSERT( LSA_RET_OK == retVal );

	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "eps_hw_undo_init(): Board HW cleanup done, max boards(%u)", PSI_CFG_MAX_IF_CNT );
}

/*----------------------------------------------------------------------------*/
/**
 * Getter function for the edd type of the selected board.
 * 
 * @param [in] board_nr     - 1...4 = HD.
 * 
 * @return LSA_COMP_ID_EDDI - PNBoard with IRTE IP
 * @return LSA_COMP_ID_EDDP - PNBoard with PNIP
 * @return LSA_COMP_ID_EDDS - Standard Ethernet Controller
 */
LSA_UINT16 eps_hw_get_edd_type( LSA_UINT16 board_nr )
{
	LSA_UINT16 edd_type;

	EPS_ASSERT(g_Eps_boards.p_list); // Initialized?
	EPS_ASSERT(board_nr && (board_nr <= PSI_CFG_MAX_IF_CNT));

	eps_enter_critical_section(g_Eps_boards.enter_exit);

	edd_type = g_Eps_boards.p_list[board_nr].edd_type;

	eps_exit_critical_section(g_Eps_boards.enter_exit);

	return edd_type;
}

/*----------------------------------------------------------------------------*/
/**
 * Translate the EPS_BoardType_EDDP to PSI_BoardType_EDDP
 * 
 * @see eps_hw_set_hd_param        - calls this function.
 * @param boardtype BoardType EPS Define
 * @return boardtype as PSI Define
 */
static PSI_EDDP_BOARD_TYPE eps_cp_hw_eps2psi_boardtype_eddp(EPS_EDDP_BOARD_TYPE boardtype)
{
    switch (boardtype)
    {
    case EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1:
        return PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1;
    case EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2:
        return PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2;
    case EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1:
        return PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1;
    case EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2:
        return PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2;
    case EPS_EDDP_BOARD_TYPE_UNKNOWN:
        return PSI_EDDP_BOARD_TYPE_UNKNOWN;
    case EPS_EDDP_BOARD_MAX:
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "eps_cp_hw_eps2psi_boardtype_eddp(): Invalid board type");
        EPS_FATAL("Invalid boardtype" );
    default:
        return PSI_EDDP_BOARD_TYPE_UNKNOWN;
    }
}

/*----------------------------------------------------------------------------*/
/**
 * Initializes the Board Port param structure.
 * 
 * @param [in] p_boardInfo            - the structure that must be initialized.
 * @return LSA_VOID
 */
LSA_VOID eps_hw_init_board_port_param( EPS_BOARD_INFO_PTR_TYPE p_boardInfo )
{
	LSA_UINT32 port_idx;

	// get the detail setting for this PN Board
	EPS_ASSERT(g_Eps_boards.p_list); // Initialized?

	eps_enter_critical_section(g_Eps_boards.enter_exit);

	for ( port_idx = 0; port_idx < (PSI_CFG_MAX_PORT_CNT + 1); port_idx++ )
	{
		p_boardInfo->port_map[port_idx].hw_port_id = EPS_HW_PORT_ID_UNDEFINED;
		p_boardInfo->port_map[port_idx].hw_phy_nr  = EPS_HW_PHY_NR_UNDEFINED;
	}

	eps_exit_critical_section(g_Eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------*/
/**
 *  The function eps_hw_init_board_port_media_type initializes the arrays
 *      port_media_type_by_HwPortId[]  and  b_is_pof_port[]
 *  of given EPS_BOARD_INFO_TYPE.
 * 
 * @param [in/out] p_boardInfo      - Adaption info for each PN board, contains the array of MediaType
 * @param [in]     asic_type        - ERTEC200/400, ERTEC200P, IRTE, ...
 * @param [in]     board_type       - EB200, CP1616, EB200P, SOC1, FPGA, ...
 * @return LSA_VOID
 */
LSA_VOID eps_hw_init_board_port_media_type(
    EPS_BOARD_INFO_PTR_TYPE p_boardInfo,
    EPS_PNDEV_ASIC_TYPE     asic_type,
    EPS_PNDEV_BOARD_TYPE    board_type )
{
    LSA_UINT16 hw_port_id;

    EPS_ASSERT(p_boardInfo != 0);

    eps_enter_critical_section(g_Eps_boards.enter_exit);

    /*------------------------------------------*/
    /* Media Type                               */
    /*------------------------------------------*/
    switch (asic_type)
    {
        case EPS_PNDEV_ASIC_ERTEC200:
        case EPS_PNDEV_ASIC_ERTEC400:
        case EPS_PNDEV_ASIC_IRTEREV7:
        {
            // EDDI
            p_boardInfo->port_media_type_by_HwPortId[0] = PSI_MEDIA_TYPE_UNKNOWN;
            p_boardInfo->b_is_pof_port[0] = 0;

            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->port_media_type_by_HwPortId[hw_port_id] = PSI_MEDIA_TYPE_COPPER;
                p_boardInfo->b_is_pof_port[hw_port_id]               = 0;

                if (   (board_type == EPS_PNDEV_BOARD_SOC1_PCIE)
                    || (board_type == EPS_PNDEV_BOARD_EB400_PCIE))
                {
                    p_boardInfo->port_media_type_by_HwPortId[hw_port_id] = PSI_MEDIA_TYPE_AUTO; // automatic POF port detection
                    p_boardInfo->b_is_pof_port[hw_port_id]               = 0;
                }
            }
        }
        break;

        case EPS_PNDEV_ASIC_ERTEC200P:
        case EPS_PNDEV_ASIC_HERA:
        {
            // EDDP
            p_boardInfo->port_media_type_by_HwPortId[0] = PSI_MEDIA_TYPE_UNKNOWN;
            p_boardInfo->b_is_pof_port[0] = 0;

            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->port_media_type_by_HwPortId[hw_port_id] = PSI_MEDIA_TYPE_AUTO; // automatic POF port detection
                p_boardInfo->b_is_pof_port[hw_port_id]               = 0;
            }
        }
        break;

        case EPS_PNDEV_ASIC_INTEL_XX:
        case EPS_PNDEV_ASIC_KSZ88XX:
        case EPS_PNDEV_ASIC_TI_XX:
        {
            // EDDS
            p_boardInfo->port_media_type_by_HwPortId[0] = PSI_MEDIA_TYPE_UNKNOWN;
            p_boardInfo->b_is_pof_port[0]               = 0;

            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->port_media_type_by_HwPortId[hw_port_id] = PSI_MEDIA_TYPE_COPPER;
                p_boardInfo->b_is_pof_port[hw_port_id]               = 0;
            }
        }
        break;

        default:
        {
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "eps_hw_init_board_port_media_type(): Invalid asic_type(%u) [board_type = (%u)]", asic_type, board_type );
            EPS_FATAL("eps_hw_init_board_port_media_type(): invalid asic_type!!!");
        }
        break;
    }

    /*------------------------------------------*/
    /* PHY Type                                 */
    /*------------------------------------------*/
    p_boardInfo->phy_type[0] = PSI_PHY_TYPE_UNKNOWN;

    switch (board_type)
    {
        case EPS_PNDEV_BOARD_EB200:
        case EPS_PNDEV_BOARD_EB200_PCIE:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_NEC;
            }
        }
        break;

        case EPS_PNDEV_BOARD_CP1616:
        case EPS_PNDEV_BOARD_EB400_PCIE:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_BCM;
            }
        }
        break;

        case EPS_PNDEV_BOARD_SOC1_PCI:
        case EPS_PNDEV_BOARD_SOC1_PCIE:
        case EPS_PNDEV_BOARD_CP1625:	
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_NSC;
            }
        }
        break;

        case EPS_PNDEV_BOARD_FPGA_ERTEC200P:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_NSC;
            }
        }
        break;

        case EPS_PNDEV_BOARD_EB200P:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_NEC;
            }
        }
        break;

        case EPS_PNDEV_BOARD_FPGA_HERA:
        case EPS_PNDEV_BOARD_HERA:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_NSC;
            }
        }
        break;

        case EPS_PNDEV_BOARD_INTEL_XX:
        case EPS_PNDEV_BOARD_IX1000:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_INTEL;
            }
        }
        break;

        case EPS_PNDEV_BOARD_KSZ88XX:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_UNKNOWN;
            }
        }
        break;

        case EPS_PNDEV_BOARD_TI_XX:
        {
            for ( hw_port_id = 1; hw_port_id <= PSI_CFG_MAX_PORT_CNT; hw_port_id++ )
            {
                p_boardInfo->phy_type[hw_port_id] = PSI_PHY_TYPE_BCM;
            }
        }
        break;
        case EPS_PNDEV_BOARD_INVALID:
        {
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "eps_hw_init_board_port_media_type(): Invalid board_type(%u) [asic_type = (%u)]", board_type, asic_type);
            EPS_FATAL("eps_hw_init_board_port_media_type(): invalid board_type!!!");
        }
        break;
        default:
        {
            EPS_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "eps_hw_init_board_port_media_type(): Invalid board_type(%u) [asic_type = (%u)]", board_type, asic_type );
            EPS_FATAL("eps_hw_init_board_port_media_type(): invalid board_type!!!");
        }
        break;
    }

    eps_exit_critical_section(g_Eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------*/
/**
 * Getter function for the board store for a given board.
 * 
 * @param [in] board_nr             - 0 = LD, 1...4 = HD.
 * @return EPS_BOARD_INFO_PTR_TYPE  - Pointer to board storage for the given board_nr.
 */
EPS_BOARD_INFO_PTR_TYPE eps_hw_get_board_store( LSA_UINT16 board_nr )
{
	EPS_BOARD_INFO_PTR_TYPE p_ret;

	// get the detail setting for this PN Board
	EPS_ASSERT(g_Eps_boards.p_list); // Initialized?
	EPS_ASSERT(board_nr <= PSI_CFG_MAX_IF_CNT);

	eps_enter_critical_section(g_Eps_boards.enter_exit);

	p_ret = &g_Eps_boards.p_list[board_nr];

	eps_exit_critical_section(g_Eps_boards.enter_exit);

	return p_ret;
}

/*----------------------------------------------------------------------------*/
/**
 * Overwrites the board storage with the given structure for the given board_nr.
 * 
 * @param [in] board_nr - 1...4 = HD
 * @param [in] p_store  - structure with the board storage info.
 * @return LSA_VOID
 */
LSA_VOID eps_hw_set_board_store( LSA_UINT16 board_nr, EPS_BOARD_INFO_CONST_PTR_TYPE p_store )
{
	// store the detail setting for this PN Board
	EPS_ASSERT(g_Eps_boards.p_list); // Initialized?
	EPS_ASSERT(board_nr <= PSI_CFG_MAX_IF_CNT);

	eps_enter_critical_section(g_Eps_boards.enter_exit);

	if (p_store == LSA_NULL) // Reset?
	{
		eps_memset(&g_Eps_boards.p_list[board_nr], 0, sizeof(EPS_BOARD_INFO_TYPE));
	}
	else
	{
	    g_Eps_boards.p_list[board_nr] = *p_store;
	}

	eps_exit_critical_section(g_Eps_boards.enter_exit);
}

/*----------------------------------------------------------------------------*/
/**
 * Set hd_params for given board_nr
 *
 * @param [in] board_nr       - 0 = LD, 1...4 = HD.
 * @param [in] p_param        - pointer to the structure with the hd_params. Casted to PSI_HD_PARAM_PTR_TYPE
 * @return LSA_VOID
 */
LSA_VOID eps_hw_set_hd_param( LSA_UINT16 board_nr, LSA_VOID_PTR_TYPE p_param )
{
	LSA_UINT16              port_idx;
	EPS_BOARD_INFO_PTR_TYPE p_store;
	PSI_HD_PARAM_PTR_TYPE   p_hd_param = (PSI_HD_PARAM_PTR_TYPE)p_param;

	EPS_ASSERT(g_Eps_boards.p_list); // Initialized?
	EPS_ASSERT(board_nr && (board_nr <= PSI_CFG_MAX_IF_CNT));

	eps_enter_critical_section(g_Eps_boards.enter_exit);

	p_store = &g_Eps_boards.p_list[board_nr];

	p_hd_param->hd_sys_handle = p_store->hd_sys_handle;

	eps_memcpy(p_hd_param->if_mac, p_store->if_mac.mac, PSI_MAC_ADDR_SIZE);

	for (port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++)
	{
		eps_memcpy(p_hd_param->port_mac[port_idx], p_store->port_mac[port_idx+1].mac, PSI_MAC_ADDR_SIZE);
	}

	// set the common used HD settings
	p_hd_param->edd_type              = p_store->edd_type;

	p_hd_param->dev_mem.base_ptr      = p_store->dev_mem.base_ptr;
	p_hd_param->dev_mem.phy_addr      = p_store->dev_mem.phy_addr;
	p_hd_param->dev_mem.size          = p_store->dev_mem.size;

	p_hd_param->nrt_tx_mem.base_ptr   = p_store->nrt_tx_mem.base_ptr;
	p_hd_param->nrt_tx_mem.phy_addr   = p_store->nrt_tx_mem.phy_addr;
	p_hd_param->nrt_tx_mem.size       = p_store->nrt_tx_mem.size;

	p_hd_param->nrt_rx_mem.base_ptr   = p_store->nrt_rx_mem.base_ptr;
	p_hd_param->nrt_rx_mem.phy_addr   = p_store->nrt_rx_mem.phy_addr;
	p_hd_param->nrt_rx_mem.size       = p_store->nrt_rx_mem.size;

	p_hd_param->crt_slow_mem.base_ptr = p_store->crt_slow_mem.base_ptr;
	p_hd_param->crt_slow_mem.phy_addr = p_store->crt_slow_mem.phy_addr;
	p_hd_param->crt_slow_mem.size     = p_store->crt_slow_mem.size;

	p_hd_param->pi_mem.base_ptr       = p_store->pi_mem.base_ptr;
	p_hd_param->pi_mem.phy_addr       = p_store->pi_mem.phy_addr;
	p_hd_param->pi_mem.size           = p_store->pi_mem.size;

	p_hd_param->hif_mem.base_ptr      = p_store->hif_mem.base_ptr;
	p_hd_param->hif_mem.phy_addr      = p_store->hif_mem.phy_addr;
	p_hd_param->hif_mem.size          = p_store->hif_mem.size;

	switch (p_store->edd_type)
	{
	    case LSA_COMP_ID_EDDI:
		{
			EPS_ASSERT(p_store->eddi.is_valid);

			// set the HD specific params for EDDI HD
			p_hd_param->edd.eddi.device_type                          = p_store->eddi.device_type;
            p_hd_param->edd.eddi.BC5221_MCModeSet                     = p_store->eddi.BC5221_MCModeSet;
			p_hd_param->edd.eddi.irte.base_ptr                        = p_store->eddi.irte.base_ptr;
			p_hd_param->edd.eddi.irte.phy_addr                        = p_store->eddi.irte.phy_addr;
			p_hd_param->edd.eddi.irte.size                            = p_store->eddi.irte.size;

			p_hd_param->edd.eddi.kram.base_ptr                        = p_store->eddi.kram.base_ptr;
			p_hd_param->edd.eddi.kram.phy_addr                        = p_store->eddi.kram.phy_addr;
			p_hd_param->edd.eddi.kram.size                            = p_store->eddi.kram.size;

			p_hd_param->edd.eddi.iocc.base_ptr                        = p_store->eddi.iocc.base_ptr;
			p_hd_param->edd.eddi.iocc.phy_addr                        = p_store->eddi.iocc.phy_addr;
			p_hd_param->edd.eddi.iocc.size                            = p_store->eddi.iocc.size;
			
			p_hd_param->edd.eddi.paea.base_ptr                        = p_store->eddi.paea.base_ptr;
			p_hd_param->edd.eddi.paea.phy_addr                        = p_store->eddi.paea.phy_addr;
			p_hd_param->edd.eddi.paea.size                            = p_store->eddi.paea.size;			

			p_hd_param->edd.eddi.sdram.base_ptr                       = p_store->eddi.sdram.base_ptr;
			p_hd_param->edd.eddi.sdram.phy_addr                       = p_store->eddi.sdram.phy_addr;
			p_hd_param->edd.eddi.sdram.size                           = p_store->eddi.sdram.size;

            #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
            p_hd_param->edd.eddi.sdram_cached.base_ptr                = p_store->eddi.sdram_cached.base_ptr;
            p_hd_param->edd.eddi.sdram_cached.phy_addr                = p_store->eddi.sdram_cached.phy_addr;
            p_hd_param->edd.eddi.sdram_cached.size                    = p_store->eddi.sdram_cached.size;
            #endif
            
			p_hd_param->edd.eddi.shared_mem.base_ptr                  = p_store->eddi.shared_mem.base_ptr;
			p_hd_param->edd.eddi.shared_mem.phy_addr                  = p_store->eddi.shared_mem.phy_addr;
			p_hd_param->edd.eddi.shared_mem.size                      = p_store->eddi.shared_mem.size;

			p_hd_param->edd.eddi.apb_periph_scrb.base_ptr             = p_store->eddi.apb_periph_scrb.base_ptr;
			p_hd_param->edd.eddi.apb_periph_scrb.phy_addr             = p_store->eddi.apb_periph_scrb.phy_addr;
			p_hd_param->edd.eddi.apb_periph_scrb.size                 = p_store->eddi.apb_periph_scrb.size;

			p_hd_param->edd.eddi.apb_periph_timer.base_ptr            = p_store->eddi.apb_periph_timer.base_ptr;
			p_hd_param->edd.eddi.apb_periph_timer.phy_addr            = p_store->eddi.apb_periph_timer.phy_addr;
			p_hd_param->edd.eddi.apb_periph_timer.size                = p_store->eddi.apb_periph_timer.size;

			p_hd_param->edd.eddi.g_shared_ram_base_ptr                = p_store->srd_api_mem.base_ptr;
            
            // SII configuration parameters
			p_hd_param->edd.eddi.SII_IrqSelector			          = p_store->eddi.SII_IrqSelector;
			p_hd_param->edd.eddi.SII_IrqNumber			              = p_store->eddi.SII_IrqNumber;
			p_hd_param->edd.eddi.SII_ExtTimerInterval		          = p_store->eddi.SII_ExtTimerInterval;

            p_hd_param->edd.eddi.fragmentation_supported              = p_store->eddi.fragmentation_supported;

            p_hd_param->edd.eddi.nrt_dmacw_default_mem.base_ptr       = p_store->eddi.nrt_mem_dmacw_default.base_ptr;
            p_hd_param->edd.eddi.nrt_dmacw_default_mem.phy_addr       = p_store->eddi.nrt_mem_dmacw_default.phy_addr;
            p_hd_param->edd.eddi.nrt_dmacw_default_mem.size           = p_store->eddi.nrt_mem_dmacw_default.size;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.base_ptr         = p_store->eddi.nrt_mem_tx_low_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.phy_addr         = p_store->eddi.nrt_mem_tx_low_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_low_frag_mem.size             = p_store->eddi.nrt_mem_tx_low_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.base_ptr         = p_store->eddi.nrt_mem_tx_mid_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.phy_addr         = p_store->eddi.nrt_mem_tx_mid_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mid_frag_mem.size             = p_store->eddi.nrt_mem_tx_mid_frag.size;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.base_ptr        = p_store->eddi.nrt_mem_tx_high_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.phy_addr        = p_store->eddi.nrt_mem_tx_high_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_high_frag_mem.size            = p_store->eddi.nrt_mem_tx_high_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.base_ptr     = p_store->eddi.nrt_mem_tx_mgmtlow_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.phy_addr     = p_store->eddi.nrt_mem_tx_mgmtlow_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mgmtlow_frag_mem.size         = p_store->eddi.nrt_mem_tx_mgmtlow_frag.size;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.base_ptr    = p_store->eddi.nrt_mem_tx_mgmthigh_frag.base_ptr;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.phy_addr    = p_store->eddi.nrt_mem_tx_mgmthigh_frag.phy_addr;
            p_hd_param->edd.eddi.nrt_tx_mgmthigh_frag_mem.size        = p_store->eddi.nrt_mem_tx_mgmthigh_frag.size;

            p_hd_param->edd.eddi.user_mem_fcts_dmacw_default          = p_store->eddi.user_mem_fcts_dmacw_default;
            p_hd_param->edd.eddi.user_mem_fcts_rx_default             = p_store->eddi.user_mem_fcts_rx_default;
            p_hd_param->edd.eddi.user_mem_fcts_tx_default             = p_store->eddi.user_mem_fcts_tx_default;
            p_hd_param->edd.eddi.user_mem_fcts_tx_low_frag            = p_store->eddi.user_mem_fcts_tx_low_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mid_frag            = p_store->eddi.user_mem_fcts_tx_mid_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_high_frag           = p_store->eddi.user_mem_fcts_tx_high_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mgmtlow_frag        = p_store->eddi.user_mem_fcts_tx_mgmtlow_frag;
            p_hd_param->edd.eddi.user_mem_fcts_tx_mgmthigh_frag       = p_store->eddi.user_mem_fcts_tx_mgmthigh_frag;

            p_hd_param->edd.eddi.has_ext_pll = p_store->eddi.has_ext_pll;

	        // Port mapping
 		    for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		    {
			    p_hd_param->port_map[port_idx].hw_phy_nr    = p_store->port_map[port_idx].hw_phy_nr;
			    p_hd_param->port_map[port_idx].hw_port_id   = p_store->port_map[port_idx].hw_port_id;
		    }
		}
		break;

	    case LSA_COMP_ID_EDDP:
		{
			EPS_ASSERT(p_store->eddp.is_valid);

			// set the specific HD params for EDDP HD
			p_hd_param->edd.eddp.icu_location = p_store->eddp.icu_location;
			p_hd_param->edd.eddp.hw_type      = p_store->eddp.hw_type;
            p_hd_param->edd.eddp.hw_interface = p_store->eddp.hw_interface;
			p_hd_param->edd.eddp.board_type   = eps_cp_hw_eps2psi_boardtype_eddp(p_store->eddp.board_type);

			p_hd_param->edd.eddp.appl_timer_mode                = p_store->eddp.appl_timer_mode;
			p_hd_param->edd.eddp.appl_timer_reduction_ratio     = p_store->eddp.appl_timer_reduction_ratio;
			p_hd_param->edd.eddp.is_transfer_end_correction_pos = p_store->eddp.is_transfer_end_correction_pos;
			p_hd_param->edd.eddp.transfer_end_correction_value  = p_store->eddp.transfer_end_correction_value;

			p_hd_param->edd.eddp.pnip.base_ptr = p_store->eddp.pnip.base_ptr;
			p_hd_param->edd.eddp.pnip.phy_addr = p_store->eddp.pnip.phy_addr;
			p_hd_param->edd.eddp.pnip.size     = p_store->eddp.pnip.size;

			p_hd_param->edd.eddp.sdram_NRT.base_ptr = p_store->eddp.sdram_NRT.base_ptr;
			p_hd_param->edd.eddp.sdram_NRT.phy_addr = p_store->eddp.sdram_NRT.phy_addr;
			p_hd_param->edd.eddp.sdram_NRT.size     = p_store->eddp.sdram_NRT.size;

			p_hd_param->edd.eddp.sdram_CRT.base_ptr = p_store->eddp.sdram_CRT.base_ptr;
			p_hd_param->edd.eddp.sdram_CRT.phy_addr = p_store->eddp.sdram_CRT.phy_addr;
			p_hd_param->edd.eddp.sdram_CRT.size     = p_store->eddp.sdram_CRT.size;

			p_hd_param->edd.eddp.perif_ram.base_ptr = p_store->eddp.perif_ram.base_ptr;
			p_hd_param->edd.eddp.perif_ram.phy_addr = p_store->eddp.perif_ram.phy_addr;
			p_hd_param->edd.eddp.perif_ram.size     = p_store->eddp.perif_ram.size;

			p_hd_param->edd.eddp.k32_tcm.base_ptr = p_store->eddp.k32_tcm.base_ptr;
			p_hd_param->edd.eddp.k32_tcm.phy_addr = p_store->eddp.k32_tcm.phy_addr;
			p_hd_param->edd.eddp.k32_tcm.size     = p_store->eddp.k32_tcm.size;

			p_hd_param->edd.eddp.k32_Atcm.base_ptr = p_store->eddp.k32_Atcm.base_ptr;
			p_hd_param->edd.eddp.k32_Atcm.phy_addr = p_store->eddp.k32_Atcm.phy_addr;
			p_hd_param->edd.eddp.k32_Atcm.size     = p_store->eddp.k32_Atcm.size;

			p_hd_param->edd.eddp.k32_Btcm.base_ptr = p_store->eddp.k32_Btcm.base_ptr;
			p_hd_param->edd.eddp.k32_Btcm.phy_addr = p_store->eddp.k32_Btcm.phy_addr;
			p_hd_param->edd.eddp.k32_Btcm.size     = p_store->eddp.k32_Btcm.size;

			p_hd_param->edd.eddp.k32_ddr3.base_ptr = p_store->eddp.k32_ddr3.base_ptr;
			p_hd_param->edd.eddp.k32_ddr3.phy_addr = p_store->eddp.k32_ddr3.phy_addr;
			p_hd_param->edd.eddp.k32_ddr3.size     = p_store->eddp.k32_ddr3.size;

			p_hd_param->edd.eddp.apb_periph_scrb.base_ptr = p_store->eddp.apb_periph_scrb.base_ptr;
			p_hd_param->edd.eddp.apb_periph_scrb.phy_addr = p_store->eddp.apb_periph_scrb.phy_addr;
			p_hd_param->edd.eddp.apb_periph_scrb.size     = p_store->eddp.apb_periph_scrb.size;

			p_hd_param->edd.eddp.apb_periph_perif.base_ptr = p_store->eddp.apb_periph_perif.base_ptr;
			p_hd_param->edd.eddp.apb_periph_perif.phy_addr = p_store->eddp.apb_periph_perif.phy_addr;
			p_hd_param->edd.eddp.apb_periph_perif.size     = p_store->eddp.apb_periph_perif.size;
			
            p_hd_param->edd.eddp.g_shared_ram_base_ptr     = p_store->srd_api_mem.base_ptr;

	        // Port mapping
 		    for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		    {
			    p_hd_param->port_map[port_idx].hw_phy_nr	= p_store->port_map[port_idx].hw_phy_nr;
			    p_hd_param->port_map[port_idx].hw_port_id   = p_store->port_map[port_idx].hw_port_id;
		    }
		}
		break;

	    case LSA_COMP_ID_EDDS:
		{
			EPS_ASSERT(p_store->edds.is_valid);

			// set the HD specific params for EDDS HD
			// note: the LL adaption settings are used
			p_hd_param->edd.edds.ll_handle         = p_store->edds.ll_handle;
			p_hd_param->edd.edds.ll_function_table = p_store->edds.ll_function_table;
			p_hd_param->edd.edds.ll_parameter      = p_store->edds.ll_params;

	        // Port mapping
 		    for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		    {
			    p_hd_param->port_map[port_idx].hw_phy_nr    = p_store->port_map[port_idx].hw_phy_nr;
			    p_hd_param->port_map[port_idx].hw_port_id   = p_store->port_map[port_idx].hw_port_id;
		    }
		}
		break;

        #if (PSI_CFG_USE_EDDT == 1)
	    case LSA_COMP_ID_EDDT:
		{
			EPS_ASSERT(p_store->eddt.is_valid);

			// set the specific HD params for EDDT HD
			p_hd_param->edd.eddt.sdram_NRT.base_ptr  = p_store->eddt.sdram_NRT.base_ptr;
			p_hd_param->edd.eddt.sdram_NRT.phy_addr  = p_store->eddt.sdram_NRT.phy_addr;
			p_hd_param->edd.eddt.sdram_NRT.size      = p_store->eddt.sdram_NRT.size;

			p_hd_param->edd.eddt.sdram_CRT.base_ptr  = p_store->eddt.sdram_CRT.base_ptr;
			p_hd_param->edd.eddt.sdram_CRT.phy_addr  = p_store->eddt.sdram_CRT.phy_addr;
			p_hd_param->edd.eddt.sdram_CRT.size      = p_store->eddt.sdram_CRT.size;

			p_hd_param->edd.eddt.pnunit_mem.base_ptr = p_store->eddt.pnunit_mem.base_ptr;
			p_hd_param->edd.eddt.pnunit_mem.phy_addr = p_store->eddt.pnunit_mem.phy_addr;
			p_hd_param->edd.eddt.pnunit_mem.size     = p_store->eddt.pnunit_mem.size;

	        // Port mapping
 		    for (port_idx = 0; port_idx <= PSI_CFG_MAX_PORT_CNT; port_idx++)
		    {
			    p_hd_param->port_map[port_idx].hw_phy_nr	= p_store->port_map[port_idx].hw_phy_nr;
			    p_hd_param->port_map[port_idx].hw_port_id   = p_store->port_map[port_idx].hw_port_id;
		    }
		}
		break;
        #endif

	    default:
		{
			EPS_FATAL("eps_hw_set_hd_param(): invalid EDDx type");
		}
	}

	eps_exit_critical_section(g_Eps_boards.enter_exit);
}

/**
 * Load firmware for coprocessor (TI, HERA, ERTEC200P) and run boot sequence
 * 
 * @param pBoardInfo [in] Board information to determine correct firmware
 * 
 * @return LSA_RET_OK firmware loading and booting was successful, error code otherwise
 */
static LSA_RESULT eps_hw_load_coprocessor( EPS_BOARD_INFO_PTR_TYPE pBoardInfo )
{
    LSA_UNUSED_ARG(pBoardInfo);

#if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
    EPS_APP_FILE_TAG_TYPE file_tag;
    LSA_UINT32            programEntryAddress;

    // retrieve the firmware
    file_tag.pndevBoard = EPS_PNDEV_BOARD_TI_XX;
    file_tag.pndevBoardDetail = EPS_EDDP_BOARD_TYPE_UNKNOWN;
    file_tag.FileType = EPS_APP_FILE_INTERNAL_FW;
    file_tag.isHD = LSA_TRUE;

    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_load_coprocessor() - Download c66x Firmware.");
    EPS_APP_ALLOC_FILE(&file_tag); 

    // load firmware to memory
    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_load_coprocessor() - Load c66x ELF file sections to memory.");  
    if(LSA_RET_OK != eps_elf_loader_load_firmware(file_tag.pFileData, file_tag.uFileSize, &programEntryAddress))
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "eps_hw_load_coprocessor() - Write elf file sections to mem failed.");
        return LSA_RET_ERR_RESOURCE;
    }

    // Free the previous allocated file
    EPS_APP_FREE_FILE(&file_tag); 

    if(EPS_OK != eps_c66x_bootloader_dsp1(programEntryAddress))
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "eps_hw_load_coprocessor() - Booting c66x failed.");
        EPS_FATAL(0);
    }
#endif

    return LSA_RET_OK;
}

/*----------------------------------------------------------------------------
ld_upper_get_params	    FW download
= ld_runs_on_type    
1			            0
2			            0
3			            1

hd_upper_get_params	    FW download
= hd_runs_on_type	
0			            0
1			            1
*/
LSA_VOID eps_hw_connect( LSA_UINT16 hd_id, LSA_BOOL bFWDownload, PSI_HD_SYS_ID_CONST_PTR_TYPE hd_sys_id_ptr )
{
    EPS_BOARD_INFO_TYPE        BoardInfo;
	EPS_PNDEV_OPEN_OPTION_TYPE Option;
	EPS_PNDEV_LOCATION_TYPE    Location;
	EPS_PNDEV_HW_PTR_TYPE      pHwInstOut;
	LSA_UINT16                 retVal;
	EPS_PNDEV_HW_PTR_TYPE 	   pBoard;

    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): hd_id = %d, bFWDownload = %d", hd_id, (LSA_UINT8)bFWDownload);

    eps_memset(&Option, 0, sizeof(Option));
    
	retVal = eps_pndev_if_get(hd_id, &pBoard);
    if (retVal == EPS_PNDEV_RET_OK)
    {
	    EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): already connected");
		return;
    }

	Option.bDebug               = g_pEpsData->bDebug;
	Option.bShmDebug            = g_pEpsData->bShmDebug;
	Option.bShmHaltOnStartup    = g_pEpsData->bShmHalt;
    Option.bUseEpsAppToGetFile  = LSA_TRUE; // use EPS_APP implementation to get the firmware file for EPS_PN_DEV Drivers that support firmware download

	Option.bLoadFw = bFWDownload;

	Location.uBusNr    = hd_sys_id_ptr->hd_location.hd_location.bus_nr;
	Location.uDeviceNr = hd_sys_id_ptr->hd_location.hd_location.device_nr;
	Location.uFctNr    = hd_sys_id_ptr->hd_location.hd_location.function_nr;

    eps_memcpy(Location.Mac, hd_sys_id_ptr->hd_location.hd_mac, sizeof(EPS_PNDEV_MAC_TYPE));

    if (hd_sys_id_ptr->hd_location.hd_selection == PSI_HD_SELECT_WITH_LOCATION)
	{
		Location.eLocationSelector = EPS_PNDEV_LOCATION_PCI;
        EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): PCI location = (%u,%u,%u)", Location.uBusNr, Location.uDeviceNr, Location.uFctNr);
	}
	else
	{
		Location.eLocationSelector = EPS_PNDEV_LOCATION_MAC;
        EPS_SYSTEM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): MAC Address = %2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X", Location.Mac[0], Location.Mac[1], Location.Mac[2], Location.Mac[3], Location.Mac[4], Location.Mac[5]);
	}

    switch (hd_sys_id_ptr->hd_location.hd_interface_selector)
    {
        case PSI_INTERFACE_1:
        {
            Location.eInterfaceSelector = EPS_PNDEV_INTERFACE_1;
            break;
        }
        case PSI_INTERFACE_2:
        {
            Location.eInterfaceSelector = EPS_PNDEV_INTERFACE_2;
            break;
        }
        case PSI_INTERFACE_UNKNOWN:
        default:
        {
            Location.eInterfaceSelector = EPS_PNDEV_INTERFACE_INVALID;
        }
    }

	retVal = eps_pndev_if_open(&Location, &Option, &pHwInstOut, hd_id);
    if (EPS_PNDEV_RET_OK == retVal)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): eps_pndev_open returned with EPS_PNDEV_RET_OK");
    }
    else
    {
        EPS_FATAL_WITH_REASON(EPS_EXIT_CODE_DEVICE_NOT_FOUND, EPS_EXIT_CODE_DEVICE_NOT_FOUND, 0, 0, 0, __FILE__, __LINE__, __FUNCTION__, "eps_hw_connect(): eps_pndev_open returned with EPS_PNDEV_RET_DEV_NOT_FOUND. Please check PCI/MAC address and configuration.");
    }
	EPS_ASSERT(pHwInstOut != LSA_NULL);
	BoardInfo = pHwInstOut->EpsBoardInfo;

	eps_hw_set_board_store(hd_id, &BoardInfo); /* Register Settings at Board Store */

    retVal = eps_hw_load_coprocessor(&BoardInfo);
    if (LSA_RET_OK == retVal)
    {
        EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_connect(): eps_hw_load_coprocessor returned with LSA_RET_OK");
    }
    else
    {
        EPS_FATAL("eps_hw_connect(): eps_hw_load_coprocessor failed");
    }
}

LSA_VOID eps_hw_get_hd_params( LSA_UINT16 hd_id, LSA_UINT8 hd_runs_on_level_ld, PSI_HD_SYS_ID_CONST_PTR_TYPE hd_sys_id_ptr, PSI_HD_PARAM_PTR_TYPE hd_param_ptr )
{
    LSA_BOOL bFWDownload;

    switch (hd_runs_on_level_ld)
    {
        case PSI_HD_RUNS_ON_LEVEL_LD_YES:
            bFWDownload = LSA_FALSE;
            break;
        case PSI_HD_RUNS_ON_LEVEL_LD_NO:
            bFWDownload = LSA_TRUE;
            break;
        default:
            bFWDownload = LSA_FALSE;
            EPS_FATAL("eps_hw_get_hd_params(): invalid hd_runs_on_level_ld");
    }

    eps_hw_connect(hd_id, bFWDownload, hd_sys_id_ptr);
	eps_hw_set_hd_param(hd_id, (LSA_VOID_PTR_TYPE)hd_param_ptr); /* Fill in HD Params needed by PSI */
}

LSA_VOID eps_hw_free_hd_params( LSA_UINT16 hd_id )
{
	LSA_UINT16 retVal;

	retVal = eps_pndev_if_close(hd_id);
	EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
}

LSA_VOID eps_hw_close_hd( LSA_UINT16 hd_id )
{
	EPS_PNDEV_HW_PTR_TYPE pBoard;
	LSA_UINT16            retVal;

	EPS_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_hw_close_hd(): closing hd_id = %u", hd_id);

	retVal = eps_pndev_if_get(hd_id, &pBoard);
	if (retVal == EPS_PNDEV_RET_OK)
	{
		retVal = eps_pndev_if_close(hd_id);
		EPS_ASSERT(retVal == EPS_PNDEV_RET_OK);
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

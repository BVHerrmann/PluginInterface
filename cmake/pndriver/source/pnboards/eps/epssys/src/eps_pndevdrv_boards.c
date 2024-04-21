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
/*  F i l e               &F: eps_pndevdrv_boards.c                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS PNDevDriver Implementation - module for board specific impl.         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20061
#define EPS_MODULE_ID      20061

/* - Includes ------------------------------------------------------------------------------------- */
#include <eps_sys.h>           /* Types / Prototypes / Funcs */

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
/*-------------------------------------------------------------------------*/
//lint -save -e18 -e114
#include <precomp.h>
#include <PnDev_Driver_Inc.h>       /* PnDevDrv Interface                       */
//lint -restore
#include <eps_cp_hw.h>              /* EPS CP PSI adaption                      */
#include <eps_trc.h>                /* Tracing                                  */
#include <eps_pn_drv_if.h>          /* PN Device Driver Interface               */
#include <eps_pndevdrv.h>           /* PNDEV-Driver Interface implementation    */
#if (PSI_CFG_USE_EDDI == 1)
//#include <eps_hw_ertec400.h>        /* Ertec400 hw adaption                     */
#endif
#if (PSI_CFG_USE_EDDP == 1)
#include <eps_hw_pnip.h>            /* Ertec200p HW adaption                    */
#endif
#if (PSI_CFG_USE_EDDS == 1)
//#include <eps_hw_edds.h>		    /* EDDS LL HW adaption 				        */
#endif
#if (PSI_CFG_USE_EDDT == 1)
#include <eps_hw_ti_icss.h>	        /* TI ICSS HW adaption	                    */
#endif
#include <eps_shm_map.h>			/* EPS Shared Memory Adress Map				*/
#include <eps_shm_file.h>           /* EPS Shared Memory File Transfer          */

#include "eps_pndevdrv_boards.h"
#include "eps_pndevdrv_helper.h"
#include "eps_register.h"

#if ( ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
#include "eps_hw_soc.h"
#endif

#if ( ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
#include "eps_hw_ertec200.h"
#endif

#if ( ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86) )
#include "eps_hw_ertec400.h"
#endif

// PAEA settings (light)
#define EPS_IOCC_BASE                                           0
#define EPS_U_IOCC_PA_EA_DIRECT_start                           (0x100000 + EPS_IOCC_BASE) 
#define EPS_U_IOCC_PA_EA_DIRECT_end                             (0x18FFFC + EPS_IOCC_BASE)
#define EPS_IOCC_PAEA_START_SOC1                                EPS_U_IOCC_PA_EA_DIRECT_start
// PNCORE_SHM Memory size is 0x500000, first 0x300000 of direct SDRAM is PNCORE_SHM
// for SOC1 and HERA NRT Memory is located directly after PNCORE_SHM
#define EPS_SDRAM_DIRECT_OFFSET_NRT                             0x300000

#if ( PSI_CFG_USE_EDDI == 1 )
/**
 * init and set board parameters after OpenDevice of PnDevDriver for ERTEC400
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_ertec400( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id )
{
	LSA_UINT16               result = EPS_PNDEV_RET_OK;
	EPS_MAC_PTR_TYPE         pMac;
	LSA_UINT32               macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	EPS_ASSERT(pBoard != LSA_NULL); // no more resources
	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	switch (pBoard->OpenDevice.uHwInfo.eBoard)
	{
	    case ePNDEV_BOARD_CP1616:
		    pBoard->sHw.EpsBoardInfo.board_type 		   = EPS_PNDEV_BOARD_CP1616;
            pBoard->sHw.EpsBoardInfo.eddi.BC5221_MCModeSet = 1; // CP1616 has BC5521 PHY transceivers with MEDIA_CONV# pin strapped to 0
		    break;
	    case ePNDEV_BOARD_DB_EB400_PCIE:
		    pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB400_PCIE;
		    break;
	    default:
		    return EPS_PNDEV_RET_UNSUPPORTED;
		//lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	pBoard->sysDev.hd_nr 	   = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type      = LSA_COMP_ID_EDDI;

	// init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	pBoard->sHw.asic_type 					  = EPS_PNDEV_ASIC_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_400;

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	/*------------------------*/
	// IRTE settings
	pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.irte.size     = pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lSize;

    // KRAM settings
	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = (LSA_UINT8*)(pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.uBase.pPtr + 0x100000); // PCIOX_KRAM_START_ERTEC400 from pciox_hw.h
	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uIrte.lAdrAhb + 0x100000;             // PCIOX_KRAM_START_ERTEC400 from pciox_hw_h
	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = EDDI_KRAM_SIZE_ERTEC400;

    // SDRAM settings (for NRT)

	pBoard->sHw.EpsBoardInfo.eddi.sdram.size    = 0x300000;

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
	// using SDRam mapping delivered by driver
	EPS_ASSERT(pBoard->sHw.EpsBoardInfo.eddi.sdram.size <= pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lSize);

	pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lAdrAhb;
    #else
	if (!pBoard->bBoardFwUsed) // we use host mem for NRT/DMACW if there is no HD Firmware
	{
		EPS_ASSERT(pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr != LSA_NULL);

		pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr;
		pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;
		pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)eps_pndevdrv_calc_phy_pci_host_address(pBoard,
																										  pBoard->OpenDevice.uHwRes.as.uErtec400.uPciMaster,
																							              sizeof(pBoard->OpenDevice.uHwRes.as.uErtec400.uPciMaster) / sizeof(uPNCORE_PCI_HOST_PAR),
																									      (LSA_UINT8*)pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);
	}
	else
	{
		EPS_ASSERT(pBoard->sHw.EpsBoardInfo.eddi.sdram.size <= pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lSize);

		pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.uBase.pPtr;
		pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uDirect.lAdrAhb;
	}
    #endif

    // Shared Mem settings
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lPhyAdr;
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size     = pBoard->OpenDevice.uHwRes.as.uErtec400.uSharedHostSdram.lSize;

    // APB-Peripherals SCRB
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     = pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uScrb.lSize;

	// APB-Peripherals TIMER
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size     = 0;

    // APB-Peripherals GPIO
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size     = pBoard->OpenDevice.uHwRes.as.uErtec400.uApbPer.uGpio.lSize;

    // set default CycleBaseFactor
    pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

	// note: not setting the EDDI spec functions for this runlevel
	//       if EDDI runs not on this level the functions never called otherwise FATAL
    #if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
    // prepare the board specific HW functions and GPIO numbers
    // Note used for PHY LED, PLL and LEDs
	pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll        = LSA_TRUE;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 25;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr  = 25;

    // set POF, PHY-Led and PLL functions
    pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct   = eps_set_pllport_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct    = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct      = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct = LSA_NULL; /* Done by EDDI */
    pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct        = eps_pof_set_led_ERTEC400;

    // set I2C functions
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz = eps_i2c_set_scl_low_highz_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz  = eps_i2c_set_sda_low_highz_ERTEC400;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read          = eps_i2c_sda_read_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select		    = eps_i2c_select_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset    = eps_i2c_ll_read_offset_ERTEC400;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset   = eps_i2c_ll_write_offset_ERTEC400;

	// SII settings (EDDI runs on EPC environment, ExtTimer exists)
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector       = EDDI_SII_IRQ_HP;
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber         = EDDI_SII_IRQ_1;

    #if defined(PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON)
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 1000UL;    //0, 250, 500, 1000 in us
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 0UL;    //0, 250, 500, 1000 in us
    #endif

    // setup DEV and NRT memory pools
    eps_ertec400_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
    #endif

    switch (pBoard->pDeviceInfo->eBoard)
    {
        case ePNDEV_BOARD_CP1616:
        {
            // setup portmapping (=1:1)
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

            pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
            pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

            pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
            pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;

            pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 3;
            pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 4;

            break;
        }
        case ePNDEV_BOARD_DB_EB400_PCIE:
        {
            // setup portmapping (= 1:4, 2:3, 3:2, 4:1)
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 3;
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 4;

            pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
            pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;

            pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 1;
            pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 2;

            pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 0;
            pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 1;

            break;
        }
        default:
        {
            // setup portmapping (=1:1)
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
            pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

            pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
            pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

            pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
            pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;

            pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 3;
            pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 4;

            break;
        }
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
    }

    // prepare process image settings (KRAM is used)
    // Note: we use the full size of KRAM, real size is calculated in PSI
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddi.kram.size;

	if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.uBase.pPtr);

		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); // HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr   = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset);
        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr   = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset;
        pBoard->sHw.EpsBoardInfo.srd_api_mem.size       = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDI_GSHAREDMEM_TYPE));
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uErtec400.uEmifSdram.uFwHif.lSize;
	}

	/*------------------------*/

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC400_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_ERTEC400_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

	// note: not setting the EDDI spec functions for this runlevel
	//       if EDDI runs not on this level the functions never called otherwise FATAL
	#if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_400))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
	eps_init_hw_ertec400(hd_id);
	#endif

	return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for ERTEC200
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_ertec200( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id )
{
	LSA_UINT16               result = EPS_PNDEV_RET_OK;
	EPS_MAC_PTR_TYPE         pMac;
	LSA_UINT32               macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200;

	pBoard->sysDev.hd_nr       = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle    = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type         = LSA_COMP_ID_EDDI;
	pBoard->sHw.EpsBoardInfo.eddi.device_type = EDD_HW_TYPE_USED_ERTEC_200;

	// init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200;

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	/*------------------------*/
	// IRTE settings
	pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.irte.size     = pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lSize;

    // KRAM settings
	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = (LSA_UINT8*)(pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.uBase.pPtr + 0x100000); // PCIOX_KRAM_START_ERTEC200 from pciox_hw.h
	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uIrte.lAdrAhb + 0x100000;             // PCIOX_KRAM_START_ERTEC200 from pciox_hw.h
	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = EDDI_KRAM_SIZE_ERTEC200; /* 64 kB */

    // SDRAM settings (for NRT)
	pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uDirect.lSize;

    // Shared Mem settings
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr = 0; // Doesn't exist // (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.pBase;
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr = 0; // Doesn't exist // pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.lPhyAdr;
	pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size     = 0; // Doesn't exist // pBoard->OpenDevice.uHwRes.as.uErtec200.uSharedHostSdram.lSize;

    // APB-Peripherals SCRB
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     = pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uScrb.lSize; // 164 Byte - EB200 Handbook

    // APB-Peripherals GPIO
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.uBase.pPtr;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size     = pBoard->OpenDevice.uHwRes.as.uErtec200.uApbPer.uGpio.lSize;

    // set default CycleBaseFactor
    pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

    // prepare the board specific HW functions and GPIO numbers
    // Note used for PHY LED, PLL and LEDs
	pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll        = LSA_TRUE;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr = 25;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr  = 25;

	// note: not setting the EDDI spec functions for this runlevel
	//       if EDDI runs not on this level the functions never called otherwise FATAL
    #if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
    // Set POF, PHY-Led and PLL functions
    pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct   = eps_set_pllport_ERTEC200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct    = eps_phy_led_blink_begin_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct      = eps_phy_led_blink_end_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct = eps_phy_led_blink_set_mode_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct        = eps_pof_set_led_EB200;

    // set I2C functions
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz  = eps_i2c_set_scl_low_highz_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz  = eps_i2c_set_sda_low_highz_EB200;
    pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read          = eps_i2c_sda_read_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select		    = eps_i2c_select_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset    = eps_i2c_ll_read_offset_EB200;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset   = eps_i2c_ll_write_offset_EB200;

	// SII settings (EDDI runs on EPC environment, ExtTimer exists)
    pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector       = EDDI_SII_IRQ_HP;
    pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber         = EDDI_SII_IRQ_1;

    #if defined(PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON)
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 1000UL;    //0, 250, 500, 1000 in us
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 0UL;    //0, 250, 500, 1000 in us
    #endif

    // setup DEV and NRT memory pools
    eps_ertec200_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
    #endif

	// setup portmapping (=1:1)
	pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
	pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 	pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
	pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

    // prepare process image settings (KRAM is used)
    // Note: we use the full size of KRAM, real size is calculated in PSI
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddi.kram.size;

	if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.uBase.pPtr);

		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); // HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr   = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset);
        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr   = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset;
        pBoard->sHw.EpsBoardInfo.srd_api_mem.size       = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDI_GSHAREDMEM_TYPE));
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uErtec200.uEmifSdram.uFwHif.lSize;
	}

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF + EPS_PNDEVDRV_ERTEC200_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_ERTEC200_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

    #if (((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_ERTEC_200))) || (EPS_PLF == EPS_PLF_ADONIS_X86))
	eps_init_hw_ertec200(hd_id);
    #endif

	return result;
}
/**
 * init and set board parameters after OpenDevice of PnDevDriver for SOC1
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_soc1( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id )
{
	LSA_UINT16               result = EPS_PNDEV_RET_OK;
	EPS_MAC_PTR_TYPE         pMac;
	LSA_UINT32               macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	pBoard->sysDev.hd_nr 	   = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDI;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle	        = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type		        = LSA_COMP_ID_EDDI;
	pBoard->sHw.EpsBoardInfo.eddi.device_type		= EDD_HW_TYPE_USED_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.cycle_base_factor = 32;

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_IRTEREV7;

	// init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	switch (pBoard->OpenDevice.uHwInfo.eBoard)
	{
		case ePNDEV_BOARD_DB_SOC1_PCI:

			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCI;

 			// setup portmapping (1:2, 2:3, 3:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;

			pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 1;
			break;
		case ePNDEV_BOARD_DB_SOC1_PCIE:

			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_SOC1_PCIE;

			// setup portmapping (=1:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

			pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;
			break;
		case ePNDEV_BOARD_CP1625:

			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_CP1625;

			// setup portmapping
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr	= 0;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr	= 1;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 3;

			break;
		default:
	        return EPS_PNDEV_RET_UNSUPPORTED;
	    //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	        
	}

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	pBoard->sHw.EpsBoardInfo.eddi.irte.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddi.irte.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.irte.size     = pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lSize;

	// KRAM settings (light)
	// Note: for PI Image the IOCC ram is used
	pBoard->sHw.EpsBoardInfo.eddi.kram.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.uBase.pPtr + 0x100000); // PCIOX_KRAM_START_SOC1 from pciox_hw.h
	pBoard->sHw.EpsBoardInfo.eddi.kram.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uIrte.lAdrAhb + 0x100000;                  // PCIOX_KRAM_START_SOC1 from pciox_hw.h
	pBoard->sHw.EpsBoardInfo.eddi.kram.size     = EDDI_KRAM_SIZE_SOC;

    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
	{
		// using the mapped memory from driver for NRT
		LSA_UINT32 sdSize = LSA_MIN( 0x00C00000, (pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lSize - EPS_SDRAM_DIRECT_OFFSET_NRT)); // 12 Mb minimum recommended
		pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.uBase.pPtr + EPS_SDRAM_DIRECT_OFFSET_NRT;
		pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lAdrAhb + EPS_SDRAM_DIRECT_OFFSET_NRT;
		pBoard->sHw.EpsBoardInfo.eddi.sdram.size     = sdSize;
	}
    #else
	// SDRAM settings (for NRT) (light)
	// Assumption: HD runs on basic
	{
		LSA_UINT32 sdSize = LSA_MIN(0x00C00000, (pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lSize - EPS_SDRAM_DIRECT_OFFSET_NRT)); // 12 Mb minimum recommended

		if (!pBoard->bBoardFwUsed) // we use host mem for NRT/DMACW if there is no HD Firmware
		{
			EPS_ASSERT(sdSize <= pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lSize);
			EPS_ASSERT(pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr != LSA_NULL);

			pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*) EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr);
			pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lPhyAdr;
			pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr = (LSA_UINT32)eps_pndevdrv_calc_phy_pci_host_address(pBoard,
																											  pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster,
																											  sizeof(pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster) / sizeof(uPNCORE_PCI_HOST_PAR),
																											  (LSA_UINT8*)pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);
		}
		else
		{
    		pBoard->sHw.EpsBoardInfo.eddi.sdram.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.uBase.pPtr + EPS_SDRAM_DIRECT_OFFSET_NRT);
	    	pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr =			   pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uDirect.lAdrAhb + EPS_SDRAM_DIRECT_OFFSET_NRT;
    	}

		pBoard->sHw.EpsBoardInfo.eddi.sdram.size         = sdSize;
	}
    #endif
    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_soc1(): sdSize=0x%x, phyAddr=0x%x", pBoard->sHw.EpsBoardInfo.eddi.sdram.size, pBoard->sHw.EpsBoardInfo.eddi.sdram.phy_addr);

    // Shared Mem settings
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr      = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.uBase.pPtr;
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr      = (LSA_UINT32)eps_pndevdrv_calc_phy_pci_host_address(pBoard,
                                                                                                                pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster,
                                                                                                                sizeof(pBoard->OpenDevice.uHwRes.as.uSoc1.uPciMaster) / sizeof(uPNCORE_PCI_HOST_PAR),
                                                                                                                (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lPhyAdr);
    pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size          = pBoard->OpenDevice.uHwRes.as.uSoc1.uSharedHostSdram.lSize;
    
    if (pBoard->bBoardFwUsed && 
        pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size > 0)
        // if Basic FW is loaded, it may use some part of it for HIF
    {
        if (pBoard->SharedMemConfig.HIF.HifCfg.bUseSharedMemMode)
        {
            if (pBoard->SharedMemConfig.HIF.HifCfg.bUseRemoteMem)
            {
                EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_soc1(): adjust sharedMem subtract hifShmSize=0x%x, before subtraction sharedSize=0x%x, phyAddr=0x%x", pBoard->SharedMemConfig.HIF.HostRam.uSize, pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size, pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr);
                // adjust sharedMem subtract HIF HostRam size
                pBoard->sHw.EpsBoardInfo.eddi.shared_mem.base_ptr      += pBoard->SharedMemConfig.HIF.HostRam.uSize;
                pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr      += pBoard->SharedMemConfig.HIF.HostRam.uSize;
                pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size          -= pBoard->SharedMemConfig.HIF.HostRam.uSize;
            }
        }
    }
    EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_soc1(): sharedSize=0x%x, phyAddr=0x%x", pBoard->sHw.EpsBoardInfo.eddi.shared_mem.size, pBoard->sHw.EpsBoardInfo.eddi.shared_mem.phy_addr);

	// APB-Peripherals SCRB (light)
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.base_ptr = (LSA_UINT8*)	EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.phy_addr =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_scrb.size     =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uScrb.lSize;

	// APB-Peripherals GPIO (light)
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.base_ptr = (LSA_UINT8*)	EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.phy_addr =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_gpio.size     =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uGpio.lSize;

	// APB-Peripherals TIMER (light)
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.base_ptr  = (LSA_UINT8*)	EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.phy_addr  =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.apb_periph_timer.size      =				pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uTimer.lSize;
	// pBoard->sHw.EpsBoardInfo.eddi.info_exttimer.TimerStarted = LSA_FALSE;

	// IOCC settings (light)
	pBoard->sHw.EpsBoardInfo.eddi.iocc.base_ptr = (LSA_UINT8*)	EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddi.iocc.phy_addr =				pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddi.iocc.size     =				pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lSize;

	// I2C setting
	pBoard->sHw.EpsBoardInfo.eddi.i2c_base_ptr  = (LSA_UINT8*) EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uApbPer.uI2c.uBase.pPtr);
	
	//lint --e(835) A zero has been given as right argument to operator '+'
	pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr = (LSA_UINT8*)	EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.uBase.pPtr + EPS_IOCC_PAEA_START_SOC1);
	//lint --e(835) A zero has been given as right argument to operator '+'
	pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr =				pBoard->OpenDevice.uHwRes.as.uSoc1.uIocc.lAdrAhb + EPS_IOCC_PAEA_START_SOC1;
	//lint --e(835) A zero has been given as right argument to operator '+'
	pBoard->sHw.EpsBoardInfo.eddi.paea.size     =				(EPS_U_IOCC_PA_EA_DIRECT_end - EPS_U_IOCC_PA_EA_DIRECT_start)/4 + 1; // RAM size

	pBoard->sHw.EpsBoardInfo.eddi.has_ext_pll			= LSA_TRUE;
	pBoard->sHw.EpsBoardInfo.eddi.extpll_out_gpio_nr	= 156;  // use ISO_OUT4
	pBoard->sHw.EpsBoardInfo.eddi.extpll_in_gpio_nr		= 94;   // use ISO_IN0

	// note: not setting the EDDI spec functions for this runlevel
	//       if EDDI runs not on this level the functions never called otherwise FATAL
    #if ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC)))
	// Set POF, PHY-Led and PLL functions
	pBoard->sHw.EpsBoardInfo.eddi.set_pll_port_fct		= eps_set_pllport_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.blink_start_fct		= EPS_SOC1_LED_BLINK_BEGIN;
	pBoard->sHw.EpsBoardInfo.eddi.blink_end_fct			= EPS_SOC1_LED_BLINK_END;
	pBoard->sHw.EpsBoardInfo.eddi.blink_set_mode_fct	= EPS_SOC1_LED_BLINK_SET_MODE;
	pBoard->sHw.EpsBoardInfo.eddi.pof_led_fct			= eps_pof_set_led_SOC;

	// set I2C functions
	pBoard->sHw.EpsBoardInfo.eddi.i2c_set_scl_low_highz  = eps_i2c_set_scl_low_highz_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_set_sda_low_highz	= eps_i2c_set_sda_low_highz_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_sda_read			= eps_i2c_sda_read_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_select			= eps_i2c_select_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_read_offset	= eps_i2c_ll_read_offset_SOC;
	pBoard->sHw.EpsBoardInfo.eddi.i2c_ll_write_offset	= eps_i2c_ll_write_offset_SOC;

	// SII settings (EDDI runs on PC environment, no ExtTimer exists)
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqSelector		= EDDI_SII_IRQ_HP;
	pBoard->sHw.EpsBoardInfo.eddi.SII_IrqNumber			= EDDI_SII_IRQ_1;
    #if defined ( PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON )
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 1000UL;	//0, 250, 500, 1000 in us
    #else
    pBoard->sHw.EpsBoardInfo.eddi.SII_ExtTimerInterval  = 0;    	//0, 250, 500, 1000 in us
    #endif

    // setup CRT memory pools
    eps_soc_FillCrtMemParams(&pBoard->sHw.EpsBoardInfo, &pBoard->sHw.EpsBoardInfo.eddi.shared_mem);

    // setup NRT memory pools
    eps_soc_FillNrtMemParams(&pBoard->sHw.EpsBoardInfo);
    #endif //((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC)))

	// setup PI (PAEA is used, with full size)
	// Note: we use the full size of PAEA, real size is calculated in PSI
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddi.paea.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddi.paea.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddi.paea.size;

    if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr);
		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); // HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size     = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr   = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset);
        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr   = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset;
        pBoard->sHw.EpsBoardInfo.srd_api_mem.size       = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDI_GSHAREDMEM_TYPE));
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size     = pBoard->OpenDevice.uHwRes.as.uSoc1.uDdr2Sdram.uFwHif.lSize;
	}
    
    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_SOC1_COUNT_MAC_IF + EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_SOC1_COUNT_MAC_IF + EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_SOC1_COUNT_MAC_IF + EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_SOC1_COUNT_MAC_IF + EPS_PNDEVDRV_SOC1_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_SOC1_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_SOC1_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddi.is_valid = LSA_TRUE;

    #if ((PSI_CFG_USE_EDDI == 1) && (defined(PSI_CFG_EDDI_CFG_SOC)))
	eps_init_hw_SOC(hd_id);
    #endif

	return result;
}
#endif // PSI_CFG_USE_EDDI

#if ( PSI_CFG_USE_EDDP == 1 ) 
/**
 * init and set board parameters after OpenDevice of PnDevDriver for EB200P
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_eb200p(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id)
{
	LSA_UINT16       		 result  = EPS_PNDEV_RET_OK;
	LSA_UINT32       		 nrtSize = 0;
	LSA_UINT32       		 crtSize = 0;
	EPS_MAC_PTR_TYPE 		 pMac;
	LSA_UINT32       		 macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_ERTEC200P;

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	switch (pBoard->OpenDevice.uHwInfo.eBoard)
	{
		case ePNDEV_BOARD_EB200P:
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_EB200P;
			break;
		case ePNDEV_BOARD_FPGA1_ERTEC200P:
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_FPGA_ERTEC200P;
			break;
		default:
			return EPS_PNDEV_RET_UNSUPPORTED;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	pBoard->sysDev.hd_nr 	   = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDP;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type      = LSA_COMP_ID_EDDP;

	// Init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	// EDDP CRT App (Transfer End) settings
	pBoard->sHw.EpsBoardInfo.eddp.appl_timer_mode = EDDP_APPL_TIMER_CFG_MODE_TRANSFER_END; // EDDP CRT APP Transfer End Interrupt not used

	// Board Type specific setup
	switch (pBoard->sHw.EpsBoardInfo.board_type)
	{
		case EPS_PNDEV_BOARD_FPGA_ERTEC200P:
        {
            switch (pBoard->OpenDevice.uHwInfo.eAsicDetail)
            {
		        case ePNDEV_ASIC_DETAIL_PNIP_REV1:
			        pBoard->sHw.EpsBoardInfo.eddp.board_type             = EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1;
			        break;
		        case ePNDEV_ASIC_DETAIL_PNIP_REV2:
			        pBoard->sHw.EpsBoardInfo.eddp.board_type             = EPS_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2;
			        break;
		        default:
			        return EPS_PNDEV_RET_UNSUPPORTED;
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
            }
			pBoard->sHw.EpsBoardInfo.eddp.hw_type                        = EDDP_HW_ERTEC200P_FPGA;
			pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio     = 4;        // CRT Transfer End 4:1 reduction ratio
			pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos = LSA_TRUE; //(LaM) ToDo CRT Transfer End Isr Offset Correction
			pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value  = 0;        //(LaM) ToDo CRT Transfer End Isr Offset Correction

			// setup portmapping (1:2, 2:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 2;

 			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 1;
			break;
        }
		case EPS_PNDEV_BOARD_EB200P:
        {
            switch (pBoard->OpenDevice.uHwInfo.eAsicDetail)
            {
		        case ePNDEV_ASIC_DETAIL_PNIP_REV1:
		            pBoard->sHw.EpsBoardInfo.eddp.board_type             = EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1;
			        break;
		        case ePNDEV_ASIC_DETAIL_PNIP_REV2:
			        pBoard->sHw.EpsBoardInfo.eddp.board_type             = EPS_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2;
			        break;
		        default:
			        return EPS_PNDEV_RET_UNSUPPORTED;
                //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
            }
			pBoard->sHw.EpsBoardInfo.eddp.hw_type                        = EDDP_HW_ERTEC200P;
			pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio     = 1;        // CRT Transfer End 1:1 reduction ratio
			pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos = LSA_TRUE; //(LaM) ToDo CRT Transfer End Isr Offset Correction
			pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value  = 0;        //(LaM) ToDo CRT Transfer End Isr Offset Correction

			// setup portmapping (=1:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;
			break;
        }
		default:
        {
			EPS_FATAL("eps_pndevdrv_open_eb200p(): not supported board type");
        }
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	pBoard->sHw.EpsBoardInfo.eddp.icu_location = EDDP_LOCATION_EXT_HOST;

    // ERTEC200P only supports EDDP_HW_IF_A. pBoard->Location.eInterfaceSelector is irrelevant.
    // EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
    pBoard->sHw.EpsBoardInfo.eddp.hw_interface = EDDP_HW_IF_A;

	// PNIP settings
	pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.pnip.size     = pBoard->OpenDevice.uHwRes.as.uErtec200P.uPnIp.lSize;

	// calculate NRT and CRT memory ranges in SDRAM
	eps_hw_e200p_calculateSizeInSDRAM(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lSize, &nrtSize, &crtSize);

    EPS_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE,
                        "eps_pndevdrv_open_eb200p(): SDRAMSize(0x%X) nrtSize(0x%X) crtSize(0x%X)",
                        pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lSize,
                        nrtSize,
                        crtSize);

	/* SDRam settings for NRT */
	pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size     = nrtSize;

	/* SDRam settings for CRT (used for ext. CRT memory pool) */
	pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.uBase.pPtr + nrtSize);
	pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uDirect.lAdrAhb + nrtSize;
	pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size     = crtSize;

	/* PERIF ram settings (used for EDDP PERIF setup) */
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf.uBase.pPtr);     // user adress mapped from AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf.lAdrAhb;                  // physical adress to AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size     = pBoard->OpenDevice.uHwRes.as.uErtec200P.uPerIf.lSize;                    // size of PERIF

	/* K32 ram settings (used for K32 DLL attach) */
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.size     = pBoard->OpenDevice.uHwRes.as.uErtec200P.uKrisc32Tcm.lSize;

	/* APB-Peripherals SCRB */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.size     = pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uScrb.lSize;

	/* APB-Peripherals PERIF */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.size     = pBoard->OpenDevice.uHwRes.as.uErtec200P.uApbPer.uPerIf.lSize;

    /* setup DEV and NRT memory pools */
    eps_eb200p_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

	/* prepare process image settings (KRAM is used) */
	/* Note: we use the full size of KRAM, real size is calculated in PSI */
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;

	if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr);

		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); //HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset);
        pBoard->sHw.EpsBoardInfo.file_transfer.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset;
        pBoard->sHw.EpsBoardInfo.file_transfer.size     = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].size;

        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr   = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset);
        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr   = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset;
        pBoard->sHw.EpsBoardInfo.srd_api_mem.size       = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDP_GSHAREDMEM_TYPE));
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uErtec200P.uEmcSdram.uFwHif.lSize;
	}

	// set POF LED function (board specific)
	pBoard->sHw.EpsBoardInfo.eddp.pof_led_fct = LSA_NULL;

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_EB200P_COUNT_MAC_IF + EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_EB200P_COUNT_MAC_IF + EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_EB200P_COUNT_MAC_IF + EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_EB200P_COUNT_MAC_IF + EPS_PNDEVDRV_EB200P_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_EB200P_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_EB200P_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddp.is_valid = LSA_TRUE;

	eps_hw_e200p_init(pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr);

	return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for HERA
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_hera( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 hd_id )
{
	LSA_UINT16               result  = EPS_PNDEV_RET_OK;
	LSA_UINT32               nrtSize = 0;
	LSA_UINT32               crtSize = 0;
	EPS_MAC_PTR_TYPE         pMac;
	LSA_UINT32               macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_HERA;

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	switch (pBoard->OpenDevice.uHwInfo.eBoard)
	{
		case ePNDEV_BOARD_FPGA1_HERA:
        {
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_FPGA_HERA;
			break;
        }
		default:
        {
	        return EPS_PNDEV_RET_UNSUPPORTED;
			//lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
        }
	}

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

    pBoard->sysDev.hd_nr 	   = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDP;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type      = LSA_COMP_ID_EDDP;

	// init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	// EDDP CRT App (Transfer End) settings
	pBoard->sHw.EpsBoardInfo.eddp.appl_timer_mode = EDDP_APPL_TIMER_CFG_MODE_TRANSFER_END; // EDDP CRT APP Transfer End Interrupt not used

	// Board Type specific setup
	switch (pBoard->sHw.EpsBoardInfo.board_type)
	{
		case EPS_PNDEV_BOARD_FPGA_HERA:
        {
			pBoard->sHw.EpsBoardInfo.eddp.board_type                     = EPS_EDDP_BOARD_TYPE_UNKNOWN;	// TODO hera; mk5656: TBD for Hera after Eddp FI
			pBoard->sHw.EpsBoardInfo.eddp.hw_type                        = EDDP_HW_HERA_FPGA;
			pBoard->sHw.EpsBoardInfo.eddp.appl_timer_reduction_ratio     = 1;        // CRT Transfer End 1:1 reduction ratio
			pBoard->sHw.EpsBoardInfo.eddp.is_transfer_end_correction_pos = LSA_TRUE; //(LaM) ToDo CRT Transfer End Isr Offset Correction
			pBoard->sHw.EpsBoardInfo.eddp.transfer_end_correction_value  = 0;        //(LaM) ToDo CRT Transfer End Isr Offset Correction

            switch (pBoard->Location.eInterfaceSelector)
            {
                case EPS_PNDEV_INTERFACE_1:
                {
                    // EDDP_HW_IF_A: 4 ports:
			        // setup portmapping (=1:1)
			        pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
			        pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 			        pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
			        pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

 			        pBoard->sHw.EpsBoardInfo.port_map[3].hw_phy_nr  = 2;
			        pBoard->sHw.EpsBoardInfo.port_map[3].hw_port_id = 3;

 			        pBoard->sHw.EpsBoardInfo.port_map[4].hw_phy_nr  = 3;
			        pBoard->sHw.EpsBoardInfo.port_map[4].hw_port_id = 4;

                    break;
                }
                case EPS_PNDEV_INTERFACE_2:
                {
                    // EDDP_HW_IF_B: 2 ports:
			        // setup portmapping (=1:1)
			        pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 0;
			        pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 			        pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 1;
			        pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;

                    break;
                }
                case EPS_PNDEV_INTERFACE_INVALID:
                {
                    EPS_FATAL("eps_pndevdrv_open_hera(): interface selector not valid");
                    break;
                }
                default: break;
            }

			break;
        }
		default:
        {
			EPS_FATAL("eps_pndevdrv_open_hera(): not supported board type");
			//lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
        }
	}

	pBoard->sHw.EpsBoardInfo.eddp.icu_location = EDDP_LOCATION_EXT_HOST;

    // HERA supports EDDP_HW_IF_A and EDDP_HW_IF_B
    switch (pBoard->Location.eInterfaceSelector)
    {
        case EPS_PNDEV_INTERFACE_1:
        {
            // EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
            pBoard->sHw.EpsBoardInfo.eddp.hw_interface = EDDP_HW_IF_A;

		    // PNIP settings for Interface A
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_A.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_A.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.size     = pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_A.lSize;

            break;
        }
        case EPS_PNDEV_INTERFACE_2:
        {
            // EDDP Interface type: EDDP_HW_IF_A, EDDP_HW_IF_B
            pBoard->sHw.EpsBoardInfo.eddp.hw_interface = EDDP_HW_IF_B;

		    // PNIP settings for Interface B
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_B.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_B.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.pnip.size     = pBoard->OpenDevice.uHwRes.as.uHera.uPnIp_B.lSize;		

            break;
        }
        case EPS_PNDEV_INTERFACE_INVALID:
        default:
        {
            EPS_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,
                                "eps_pndevdrv_open_hera(): invalid eInterfaceSelector(%u), hd_id(%u)",
                                pBoard->Location.eInterfaceSelector,
                                hd_id);
			EPS_FATAL("eps_pndevdrv_open_hera(): invalid eInterfaceSelector");
        }
    }

    // check PNIP settings
    if (   (LSA_NULL == pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr)
        || (0        == pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr)
        || (0        == pBoard->sHw.EpsBoardInfo.eddp.pnip.size))
    {
        EPS_SYSTEM_TRACE_05(0, LSA_TRACE_LEVEL_FATAL,
                            "eps_pndevdrv_open_hera(): invalid PNIP settings. eInterfaceSelector(%u) hd_id(%u) pnip_base_ptr(0x%X) pnip_phy_addr(0x%X) pnip_size(0x%X)",
                            pBoard->Location.eInterfaceSelector,
                            hd_id,
                            pBoard->sHw.EpsBoardInfo.eddp.pnip.base_ptr,
                            pBoard->sHw.EpsBoardInfo.eddp.pnip.phy_addr,
                            pBoard->sHw.EpsBoardInfo.eddp.pnip.size);
		EPS_FATAL("eps_pndevdrv_open_hera(): invalid PNIP settings");
		//lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
	    return EPS_PNDEV_RET_ERR;
    }

    #if (EPS_PLF != EPS_PLF_WINDOWS_X86)
    if (!pBoard->bBoardFwUsed) /* We use host mem for NRT if no Firmware is loaded */
    {
        EPS_ASSERT(pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.uBase.pPtr != LSA_NULL);
        // calculate memory ranges in HostMem based on avaliable HostMem
        eps_hw_hera_calculateSizeInSDRAM(pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.lSize, &nrtSize, &crtSize);
        
        /* SDRam settings for NRT */
        pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.uBase.pPtr;
        pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr = 0xE0000000; // PCIe Master Window 0
        pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size     = nrtSize;

        /* SDRam settings for CRT (used for ext. CRT memory pool) */
        pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uHera.uSharedHostSdram.uBase.pPtr + nrtSize;
        pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr = 0xE0000000 + nrtSize; // PCIe Master Window 0 + nrtOffset
        pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size     = crtSize;
    }
    else
    #endif
    {
        // calculate memory ranges in SDRAM based on avaliable SDRAM
	    eps_hw_hera_calculateSizeInSDRAM((pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uDirect.lSize - EPS_SDRAM_DIRECT_OFFSET_NRT), &nrtSize, &crtSize);

    	/* SDRam settings for NRT */
	    pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uDirect.uBase.pPtr + EPS_SDRAM_DIRECT_OFFSET_NRT);
    	pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uDirect.lAdrAhb + EPS_SDRAM_DIRECT_OFFSET_NRT;
    	pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.size     = nrtSize;

    	/* SDRam settings for CRT (used for ext. CRT memory pool) */
	    pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uDirect.uBase.pPtr + EPS_SDRAM_DIRECT_OFFSET_NRT + nrtSize);
    	pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uDirect.lAdrAhb + EPS_SDRAM_DIRECT_OFFSET_NRT + nrtSize;
    	pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size     = crtSize;
    }
    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_HIGH, "eps_pndevdrv_open_hera(): eps_hw_hera_calculateSizeInSDRAM() returned nrtSize=0x%x, crtSize=0x%x, nrtPhyAddr=0x%x, crtPhyAddr=0x%x", 
                                                            nrtSize, crtSize, pBoard->sHw.EpsBoardInfo.eddp.sdram_NRT.phy_addr, pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr);


	/* PERIF ram settings (used for EDDP PERIF setup) */
	/* no PERIF available at Hera */
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.base_ptr = 0;   // user adress mapped from AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.phy_addr = 0;	// physical adress to AHB Adress to PERIF
	pBoard->sHw.EpsBoardInfo.eddp.perif_ram.size     = 0;   // size of perif

	/* K32 ram settings (used for K32 DLL attach) */
	{
		pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.base_ptr = 0;
		pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.phy_addr = 0;
		pBoard->sHw.EpsBoardInfo.eddp.k32_tcm.size     = 0;

        if // Interface A?
           (pBoard->sHw.EpsBoardInfo.eddp.hw_interface == EDDP_HW_IF_A)
        {
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Atcm.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Atcm.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.size     = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Atcm.lSize;

		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Btcm.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Btcm.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.size     = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_A_Btcm.lSize;
        }
        else // Interface B
        {
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Atcm.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Atcm.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Atcm.size     = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Atcm.lSize;

		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Btcm.uBase.pPtr);
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Btcm.lAdrAhb;
		    pBoard->sHw.EpsBoardInfo.eddp.k32_Btcm.size     = pBoard->OpenDevice.uHwRes.as.uHera.uR4_CORE_B_Btcm.lSize;
        }

		pBoard->sHw.EpsBoardInfo.eddp.k32_ddr3.base_ptr     = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uKriscDdr3Sdram.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.eddp.k32_ddr3.phy_addr     = pBoard->OpenDevice.uHwRes.as.uHera.uKriscDdr3Sdram.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.eddp.k32_ddr3.size         = pBoard->OpenDevice.uHwRes.as.uHera.uKriscDdr3Sdram.lSize;
	}

	/* APB-Peripherals SCRB */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uScrb.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_scrb.size     = pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uScrb.lSize;

	/* APB-Peripherals SEC SCRB */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_sec_scrb.base_ptr	= (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uSecScrb.uBase.pPtr);
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_sec_scrb.phy_addr	= pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uSecScrb.lAdrAhb;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_sec_scrb.size		= pBoard->OpenDevice.uHwRes.as.uHera.uApbPer.uSecScrb.lSize;

	/* APB-Peripherals PERIF */
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.eddp.apb_periph_perif.size     = 0;

    /* setup DEV and NRT memory pools */
    eps_hera_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

	/* setup CRT slow buffer */
	pBoard->sHw.EpsBoardInfo.crt_slow_mem.base_ptr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.base_ptr;
	pBoard->sHw.EpsBoardInfo.crt_slow_mem.phy_addr = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.phy_addr;
	pBoard->sHw.EpsBoardInfo.crt_slow_mem.size     = pBoard->sHw.EpsBoardInfo.eddp.sdram_CRT.size;

	/* prepare process image settings */
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = 0;

	if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.uBase.pPtr);

		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); //HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset);
        pBoard->sHw.EpsBoardInfo.file_transfer.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset;
        pBoard->sHw.EpsBoardInfo.file_transfer.size     = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].size;

        pBoard->sHw.EpsBoardInfo.srd_api_mem.base_ptr   = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset);
        pBoard->sHw.EpsBoardInfo.srd_api_mem.phy_addr   = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].offset;
        pBoard->sHw.EpsBoardInfo.srd_api_mem.size       = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_SRD_API_MEM].size;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.srd_api_mem.size >= sizeof(EDDP_GSHAREDMEM_TYPE));
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.uBase.pPtr);
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uHera.uDdr3Sdram.uFwHif.lSize;
	}

	// set POF LED function (board specific)
	pBoard->sHw.EpsBoardInfo.eddp.pof_led_fct = LSA_NULL;

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_HERA_COUNT_MAC_IF + EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_HERA_COUNT_MAC_IF + EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_HERA_COUNT_MAC_IF + EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_HERA_COUNT_MAC_IF + EPS_PNDEVDRV_HERA_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_HERA_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_HERA_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------

	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddp.is_valid = LSA_TRUE;

	return result;
}
#endif // PSI_CFG_USE_EDDP
#if ( PSI_CFG_USE_EDDS == 1 )
#if defined(PSI_EDDS_CFG_HW_INTEL)
/**
 * init and set board parameters after OpenDevice of PnDevDriver for i210 (Intel Springville, copper variant)
 * 
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_i210( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id )
{
	LSA_UINT16 result;

	result = eps_intelLL_open(&pBoard->uEddsIntel,
							  &pBoard->sHw,
							  &pBoard->sysDev,
							  (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr,
							  hd_id,
							  EPS_INTEL_PCI_VENDOR_ID,
						      EPS_INTEL_PCI_DEVICE_ID_I210);

	if // success?
	   (result == EPS_PNDEV_RET_OK)
	{
	    // SDRam settings for NRT
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.uBase.pPtr);
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lPhyAdr;
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lSize;

        // setup DEV and NRT memory pools
        eps_i210_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
	}

	return result;
}

/**
* init and set board parameters after OpenDevice of PnDevDriver for i210 (Intel Springville with SFP module, fiber optics)
*
* @param	[in] pBoard				pointer to current board structure
* @param 	[in] hd_id				index of hd
* @return 	EPS_PNDEV_RET_OK
*/
LSA_UINT16 eps_pndevdrv_open_i210IS( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id )
{
    LSA_UINT16 result;

    result = eps_intelLL_open(&pBoard->uEddsIntel,
                              &pBoard->sHw,
                              &pBoard->sysDev,
                              (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uI210.uBar0.uBase.pPtr,
                              hd_id,
                              EPS_INTEL_PCI_VENDOR_ID,
                              EPS_INTEL_PCI_DEVICE_ID_I210IS);

    if // success?
       (result == EPS_PNDEV_RET_OK)
    {
        // SDRam settings for NRT
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.uBase.pPtr);
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lPhyAdr;
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size = pBoard->OpenDevice.uHwRes.as.uI210.uSharedHostSdram.lSize;

        // setup DEV and NRT memory pools
        eps_i210_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
    }

    return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for i82574 (Intel Hartwell)
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_i82574( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id )
{
    LSA_UINT16 result;

    result = eps_intelLL_open(&pBoard->uEddsIntel,
							  &pBoard->sHw,
							  &pBoard->sysDev,
                              (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uI82574.uBar0.uBase.pPtr,
							  hd_id,
                              EPS_INTEL_PCI_VENDOR_ID,
                              EPS_INTEL_PCI_DEVICE_ID_I82574);

	if // success?
	   (result == EPS_PNDEV_RET_OK)
	{
	    // SDRam settings for NRT
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.uBase.pPtr);
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.lPhyAdr;
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = pBoard->OpenDevice.uHwRes.as.uI82574.uSharedHostSdram.lSize;

        // setup DEV and NRT memory pools
        eps_i82574_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
	}

	return result;
}
#endif // PSI_EDDS_CFG_HW_INTEL

#if defined(PSI_EDDS_CFG_HW_KSZ88XX)
/**
 * init and set board parameters after OpenDevice of PnDevDriver for ksz8841
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_ksz8841( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id )
{
	LSA_UINT16 result;

	result = eps_ksz88xx_open(&pBoard->uEddsKSZ88XX,
							  &pBoard->sHw,
							  &pBoard->sysDev,
							  (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uKSZ8841.uBar0.uBase.pPtr,
							  hd_id,
							  EPS_KSZ88XX_PCI_VENDOR_ID,
							  EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8841);

	if // success?
	   (result == EPS_PNDEV_RET_OK)
	{
	    // SDRam settings for NRT
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.uBase.pPtr);
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.lPhyAdr;
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = pBoard->OpenDevice.uHwRes.as.uKSZ8841.uSharedHostSdram.lSize;

        // setup DEV and NRT memory pools
        eps_ksz8841_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
	}

	return result;
}

/**
 * init and set board parameters after OpenDevice of PnDevDriver for ksz8841
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_ksz8842( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id )
{
	LSA_UINT16 result;

	result = eps_ksz88xx_open(&pBoard->uEddsKSZ88XX,
							  &pBoard->sHw,
							  &pBoard->sysDev,
							  (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uKSZ8842.uBar0.uBase.pPtr,
							  hd_id,
							  EPS_KSZ88XX_PCI_VENDOR_ID,
							  EPS_KSZ88XX_PCI_DEVICE_ID_KSZ8842);

	if // success?
	   (result == EPS_PNDEV_RET_OK)
	{
	    // SDRam settings for NRT
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.uBase.pPtr);
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.lPhyAdr;
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = pBoard->OpenDevice.uHwRes.as.uKSZ8842.uSharedHostSdram.lSize;

        // setup DEV and NRT memory pools
        eps_ksz8842_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
	}

	return result;
}
#endif // PSI_EDDS_CFG_HW_KSZ88XX

#if defined(PSI_EDDS_CFG_HW_TI)
/**
 * init and set board parameters after OpenDevice of PnDevDriver for AM5728 GMAC (TI)
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			 options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_am5728_gmac( EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, EPS_PNDEV_OPEN_OPTION_CONST_PTR_TYPE pOption, LSA_UINT16 const hd_id )
{
	LSA_UINT16 				 result;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;

	result = eps_tiLL_open(&pBoard->uEddsTI,
						   &pBoard->sHw,
						   &pBoard->sysDev,
						   (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uGmacSw.uBase.pPtr,
						   hd_id,
						   EPS_TI_PCI_VENDOR_ID,
						   EPS_TI_PCI_DEVICE_ID_AM5728,
                           (EPS_MAC_PTR_TYPE)pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr); //Surpress lint warning by using Address to array. This equals the first element == &pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[0]

	if // success?
	   (result == EPS_PNDEV_RET_OK)
    {
	    // SDRam settings for NRT: use EmifSdram --> can't use Outbound Region 0 since GMAC is not connected to it via Interconnect (see chapter 14 Connectivity Matrix)
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.uBase.pPtr);
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.lAdrAhb;
	    pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size     = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.lSize;

        // setup DEV and NRT memory pools
        eps_am5728_gmac_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

		if (pOption->bLoadFw)
		{
			// get pointer to adress map which is written to shm in firmware
			pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr);

			// setup HIF buffer
			pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset); //HIF Offset moved up 100kB
			pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
			pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

            pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset);
            pBoard->sHw.EpsBoardInfo.file_transfer.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset;
            pBoard->sHw.EpsBoardInfo.file_transfer.size     = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].size;
		}
		else
		{
			// map HIF buffer to direct shared mem, no memory map is available in this mode
			pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr);
			pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb;
			pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lSize;
		}
    }

    return result;
}
#endif // PSI_EDDS_CFG_HW_TI

#if defined(PSI_EDDS_CFG_HW_IOT2000)
/**
* init and set board parameters after OpenDevice of PnDevDriver for iot2000 (Intel Quark)
*
* @param	[in] pBoard				pointer to current board structure
* @param 	[in] hd_id				index of hd
* @return 	EPS_PNDEV_RET_OK
*/
LSA_UINT16 eps_pndevdrv_open_iot2000(EPS_PNDEVDRV_BOARD_PTR_TYPE pBoard, LSA_UINT16 hd_id)
{
    LSA_UINT16 result = EPS_PNDEV_RET_OK;

    result = eps_iot2000LL_open(&pBoard->uEddsIOT2000,
                                &pBoard->sHw,
                                &pBoard->sysDev,
                                (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uIX1000.uBar0.uBase.pPtr,
                                hd_id,
                                EPS_IX1000_PCI_VENDOR_ID,
                                EPS_IX1000_PCI_DEVICE_ID_IOT2000,
                                (EPS_MAC_PTR_TYPE)pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr); //Surpress lint warning by using Address to array. This equals the first element == &pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[0]

    if (result == EPS_PNDEV_RET_OK)
    {
        // setup NRT buffer (local mem is used)
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.size = pBoard->OpenDevice.uHwRes.as.uIX1000.uSharedHostSdram.lSize;
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr = (LSA_UINT8*)EPS_CAST_TO_VOID_PTR(pBoard->OpenDevice.uHwRes.as.uIX1000.uSharedHostSdram.uBase.pPtr);
        pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.phy_addr = pBoard->OpenDevice.uHwRes.as.uIX1000.uSharedHostSdram.lPhyAdr;
        EPS_ASSERT(pBoard->sHw.EpsBoardInfo.edds.sdram_NRT.base_ptr != LSA_NULL);

		eps_iot2000_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);
    }

    return result;
}
#endif // PSI_EDDS_CFG_HW_IOT2000
#endif // PSI_CFG_USE_EDDS

#if ( PSI_CFG_USE_EDDT == 1 )
/**
 * init and set board parameters after OpenDevice of PnDevDriver for AM5728 ICSS (TI)
 *
 * @param	[in] pBoard				pointer to current board structure
 * @param 	[in] pOption			options, e.g. if firmware is downloaded
 * @param 	[in] hd_id				index of hd
 * @return 	EPS_PNDEV_RET_OK
 */
LSA_UINT16 eps_pndevdrv_open_am5728_icss( EPS_PNDEVDRV_BOARD_PTR_TYPE const pBoard, EPS_PNDEV_OPEN_OPTION_PTR_TYPE const pOption, LSA_UINT16 const hd_id )
{
	LSA_UINT16       		 result  = EPS_PNDEV_RET_OK;
	LSA_UINT32       		 nrtSize = 0;
	LSA_UINT32       		 crtSize = 0;
	EPS_MAC_PTR_TYPE 	     pMac;
	LSA_UINT32       	     macIdx, ifIdx, portIdx;
	EPS_SHM_ADR_MAP_PTR_TYPE pPnDevAdrMap;
    LSA_UINT32               OffsetRamNRT;
    LSA_UINT32               OffsetRamPNUnit;
    LSA_UINT32               LenRamNRT;
    LSA_UINT32               LenRamPNUnit;

    EPS_APP_MAC_ADDR_ARR     MacAddressesApp;
    LSA_UINT16               loopCnt;

	pBoard->sHw.asic_type = EPS_PNDEV_ASIC_TI_XX;

	eps_memset(&pBoard->sHw.EpsBoardInfo, 0, sizeof(pBoard->sHw.EpsBoardInfo));

	switch (pBoard->OpenDevice.uHwInfo.eBoard)
	{
		case ePNDEV_BOARD_TI_AM5728:
			pBoard->sHw.EpsBoardInfo.board_type = EPS_PNDEV_BOARD_TI_XX;
			break;
		default:
			return EPS_PNDEV_RET_UNSUPPORTED;
	}

	// initialization of MediaType
	eps_hw_init_board_port_media_type((EPS_BOARD_INFO_PTR_TYPE)&pBoard->sHw.EpsBoardInfo, pBoard->sHw.asic_type, pBoard->sHw.EpsBoardInfo.board_type);

	pBoard->sysDev.hd_nr 	   = hd_id;
	pBoard->sysDev.pnio_if_nr  = 0;	// don't care
	pBoard->sysDev.edd_comp_id = LSA_COMP_ID_EDDT;

	pBoard->sHw.EpsBoardInfo.hd_sys_handle = &pBoard->sysDev;
	pBoard->sHw.EpsBoardInfo.edd_type      = LSA_COMP_ID_EDDT;

	// init User to HW port mapping (used PSI GET HD PARAMS)
	eps_hw_init_board_port_param(&pBoard->sHw.EpsBoardInfo);

	// Board Type specific setup
	switch (pBoard->sHw.EpsBoardInfo.board_type)
	{
		case EPS_PNDEV_BOARD_TI_XX:
        {
			// setup portmapping (=1:1)
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_phy_nr  = 1;
			pBoard->sHw.EpsBoardInfo.port_map[1].hw_port_id = 1;

 			pBoard->sHw.EpsBoardInfo.port_map[2].hw_phy_nr  = 2;
			pBoard->sHw.EpsBoardInfo.port_map[2].hw_port_id = 2;
			break;
        }
		default:
        {
			EPS_FATAL("eps_pndevdrv_open_am5728_icss(): not supported board type");
        }
	}

    // the PNDEVDRV maps physical memory: pHWres->as.uAM5728.uEmifSdram.uDirect.lAdrAhb = 0x80200000 ( 0x80000000 + offset ) with length: 0x01e00000
    // the used segments are defined by the adonis_eps_am5728 target

    // config NRT MEM:
    //  we use the physical memory from 0x80500000 + (length) for IO_DATA segment (adonis_eps_am5728::nrt)
    //  nrtMemPhy = 0x80500000
    //  so the offset of the mapped memory is: 0x80500000 - 0x8020000
    OffsetRamNRT    = 0x00300000UL;
    LenRamNRT       = 0x01800000UL;
    // config PNUNIT MEM:
    //  we use the physical memory from 0x81d0000 + (length) for PNUNIT_DATA segment (adonis_eps_am5728::c66x_shm)
    //  sharedMemPhy = 0x81d00000
    //  so the offset of the mapped memory is: 0x81D00000 - 0x8020000
    OffsetRamPNUnit = 0x01B00000UL;
    LenRamPNUnit    = 0x00100000UL - 8UL; // handshake between A15 and C66 uses the last 8 bytes

	// calculate NRT and CRT memory ranges in SDRAM
	eps_hw_am5728_icss_calculateSizeInSDRAM(LenRamNRT, &nrtSize, &crtSize);

    EPS_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE,
                        "eps_pndevdrv_open_am5728_icss(): SDRAMSize(0x%X) LenRamNRT(0x%X) nrtSize(0x%X) crtSize(0x%X)",
                        pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.lSize,
                        LenRamNRT,
                        nrtSize,
                        crtSize);

	// SDRam settings for NRT
	pBoard->sHw.EpsBoardInfo.eddt.sdram_NRT.base_ptr  = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.uBase.pPtr + OffsetRamNRT;
	pBoard->sHw.EpsBoardInfo.eddt.sdram_NRT.phy_addr  = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.lAdrAhb + OffsetRamNRT;
	pBoard->sHw.EpsBoardInfo.eddt.sdram_NRT.size      = nrtSize;

	// SDRam settings for CRT
	pBoard->sHw.EpsBoardInfo.eddt.sdram_CRT.base_ptr  = pBoard->sHw.EpsBoardInfo.eddt.sdram_NRT.base_ptr + nrtSize;
	pBoard->sHw.EpsBoardInfo.eddt.sdram_CRT.phy_addr  = pBoard->sHw.EpsBoardInfo.eddt.sdram_NRT.phy_addr + nrtSize;
	pBoard->sHw.EpsBoardInfo.eddt.sdram_CRT.size      = crtSize;

	// PNUNIT ram settings
	pBoard->sHw.EpsBoardInfo.eddt.pnunit_mem.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.uBase.pPtr + OffsetRamPNUnit;
	pBoard->sHw.EpsBoardInfo.eddt.pnunit_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uDirect.lAdrAhb + OffsetRamPNUnit;
	pBoard->sHw.EpsBoardInfo.eddt.pnunit_mem.size     = LenRamPNUnit;

    // setup DEV and NRT memory pools
    eps_am5728_icss_FillDevNrtMemParams(&pBoard->sHw.EpsBoardInfo);

	// prepare process image settings
	pBoard->sHw.EpsBoardInfo.pi_mem.base_ptr = 0;
	pBoard->sHw.EpsBoardInfo.pi_mem.phy_addr = 0;
	pBoard->sHw.EpsBoardInfo.pi_mem.size     = 0;

	if (pOption->bLoadFw)
	{
		// get pointer to adress map which is written to shm in firmware
		pPnDevAdrMap = (EPS_SHM_ADR_MAP_PTR_TYPE)(pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr);

		// setup HIF buffer
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset; // HIF Offset moved up 100kB
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].offset;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_HIF].size;

        pBoard->sHw.EpsBoardInfo.file_transfer.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset;
        pBoard->sHw.EpsBoardInfo.file_transfer.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb + pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].offset;
        pBoard->sHw.EpsBoardInfo.file_transfer.size     = pPnDevAdrMap->adr_map[EPS_SHM_MAP_KEY_FILE].size;
	}
	else
	{
		// map HIF buffer to direct shared mem, no memory map is available in this mode
		pBoard->sHw.EpsBoardInfo.hif_mem.base_ptr = (LSA_UINT8*)pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.uBase.pPtr;
		pBoard->sHw.EpsBoardInfo.hif_mem.phy_addr = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lAdrAhb;
		pBoard->sHw.EpsBoardInfo.hif_mem.size 	  = pBoard->OpenDevice.uHwRes.as.uAM5728.uEmifSdram.uFwHif.lSize;
	}

    // -- MAC address handling START ----------------------------------

    // prepare the MAC-array    
    eps_memset(&MacAddressesApp,  0, sizeof(MacAddressesApp));

    // Port specific setup
    pBoard->sHw.EpsBoardInfo.nr_of_ports = EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS;
    
    // we have to copy the given MAC addresses to our structure which will be passed to the application
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF + EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS); loopCnt++)
    {
        MacAddressesApp.lArrayMacAdr[loopCnt][0] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][0]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][1] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][1]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][2] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][2]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][3] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][3]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][4] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][4]; 
        MacAddressesApp.lArrayMacAdr[loopCnt][5] = pBoard->OpenDevice.uHwRes.uCommon.uRemanentData.lArrayMacAdr[loopCnt][5]; 
    }

    // now give the pre-filled MAC array to application (pcIOX, PNDriver,...)
    EPS_APP_GET_MAC_ADDR(&MacAddressesApp, pBoard->sysDev.hd_nr, (EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF + EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS) );
    
    // we got the array with MAC addresses back and trace it
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF + EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS); loopCnt++)
    {
        EPS_SYSTEM_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE_HIGH, 
                                "EPS_APP_GET_MAC_ADDR: hd_id(%d): MacAddresses[%d]: %02x:%02x:%02x:%02x:%02x:%02x", 
                                pBoard->sysDev.hd_nr, loopCnt,
                                MacAddressesApp.lArrayMacAdr[loopCnt][0],
                                MacAddressesApp.lArrayMacAdr[loopCnt][1],
                                MacAddressesApp.lArrayMacAdr[loopCnt][2],
                                MacAddressesApp.lArrayMacAdr[loopCnt][3],
                                MacAddressesApp.lArrayMacAdr[loopCnt][4],
                                MacAddressesApp.lArrayMacAdr[loopCnt][5]);
    }
    
    // we now check the array with MAC addresses if any of the needed addresses is empty
    for(loopCnt = 0; loopCnt < (EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF + EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_PORTS); loopCnt++)
    {
        if(   MacAddressesApp.lArrayMacAdr[loopCnt][0] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][1] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][2] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][3] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][4] == 0x00
           && MacAddressesApp.lArrayMacAdr[loopCnt][5] == 0x00
          )
        {
            EPS_FATAL("Empty MAC-Address detected");
        }
    }

    // set the interface MAC with the value from the array returned
    for ( ifIdx = 0; ifIdx < EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF; ifIdx++ )
    {
        // Use MAC from driver
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[ifIdx];
        pBoard->sHw.EpsBoardInfo.if_mac = *pMac;
    }

    // set the Port MAC(s) with the value(s) from the array returned
    for ( portIdx = 0; portIdx < pBoard->sHw.EpsBoardInfo.nr_of_ports; portIdx++ )
    {
        macIdx = portIdx + EPS_PNDEVDRV_TI_ICSS_COUNT_MAC_IF;

        // Use next MAC from driver for user port
        pMac = (EPS_MAC_PTR_TYPE)&MacAddressesApp.lArrayMacAdr[macIdx];
        pBoard->sHw.EpsBoardInfo.port_mac[portIdx+1]        = *pMac;
    }

    // -- MAC address handling DONE ----------------------------------
    
	// set valid and store the data for the HD
	pBoard->sHw.EpsBoardInfo.eddt.is_valid = LSA_TRUE;

    return result;
}
#endif // PSI_CFG_USE_EDDT

/**
 * Clear gpio for a given board.
 *
 * Note: Only EB200P boards are currently supported.
 *
 * @param	[in] pHwInstIn		        pointer to structure with hardware configuration
 * @param	[in] gpio			        enum with gpios
 * @return 	EPS_PNDEV_RET_OK            GPIO were set correctly
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
LSA_UINT16 eps_pndevdrv_clear_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	LSA_UNUSED_ARG(gpio);

	switch (pHwInstIn->EpsBoardInfo.board_type)
	{
        #if ( PSI_CFG_USE_EDDP == 1 )
		case EPS_PNDEV_BOARD_EB200P:
			switch (gpio)
			{
				case EPS_PNDEV_DEBUG_GPIO_1:
					eps_hw_e200p_clear_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_1);
					break;
				case EPS_PNDEV_DEBUG_GPIO_2:
					eps_hw_e200p_clear_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_2);
					break;
				default:
					return EPS_PNDEV_RET_UNSUPPORTED;
			}
			break;
        #endif
		default:
			/* do nothing */
			break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	return EPS_PNDEV_RET_OK;
}

/**
 * Set gpio for a given board.
 * Note: Only EB200P boards are currently supported.
 *
 * @param	[in] pHwInstIn		        pointer to structure with hardware configuration
 * @param	[in] gpio			        enum with gpios
 * @return 	EPS_PNDEV_RET_OK            GPIO were set correctly
 * @return  EPS_PNDEV_RET_UNSUPPORTED   Board type is not supported.
 */
LSA_UINT16 eps_pndevdrv_set_gpio (EPS_PNDEV_HW_CONST_PTR_TYPE pHwInstIn, EPS_PNDEV_GPIO_DESC_TYPE gpio)
{
	EPS_ASSERT(pHwInstIn != LSA_NULL);
	EPS_ASSERT(pHwInstIn->hDevice != LSA_NULL);

	LSA_UNUSED_ARG(gpio);

	switch (pHwInstIn->EpsBoardInfo.board_type)
	{
        #if ( PSI_CFG_USE_EDDP == 1 )
		case EPS_PNDEV_BOARD_EB200P:
			switch (gpio)
			{
				case EPS_PNDEV_DEBUG_GPIO_1:
					eps_hw_e200p_set_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_1);
					break;
				case EPS_PNDEV_DEBUG_GPIO_2:
					eps_hw_e200p_set_gpio(pHwInstIn->EpsBoardInfo.eddp.apb_periph_perif.base_ptr, EPS_HW_E200P_DEBUG_GPIO_2);
					break;
				default:
					return EPS_PNDEV_RET_UNSUPPORTED;
			}
			break;
        #endif
		default:
			/* do nothing */
			break;
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
	}

	return EPS_PNDEV_RET_OK;
}
#endif //((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

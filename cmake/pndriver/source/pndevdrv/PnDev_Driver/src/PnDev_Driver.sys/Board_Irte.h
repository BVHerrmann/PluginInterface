/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Board_Irte.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for boards with IRTE (Ertec200/400/Soc1)
//
//****************************************************************************/

#ifndef __Board_Irte_h__
#define __Board_Irte_h__

	//########################################################################
	//  Defines
	//########################################################################

	//========================================================================
	//	CP1616 board / CP1616-Eval board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR0_SIZE			(0)
	#define	BOARD_CP1616__BAR1_SIZE			(1024*64)
	#define	BOARD_CP1616__BAR2_SIZE			(1024*64)
	#define	BOARD_CP1616__BAR3_SIZE			(1024*1024*8)
	#define	BOARD_CP1616__BAR4_SIZE			(1024*1024*16)
	#define	BOARD_CP1616__BAR5_SIZE			(1024*1024*32)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR1_OFFSET_APB_PER	0x00000000

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR2_OFFSET_SRAM	0x00000000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR3_OFFSET_IRTE	0x00000000

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR4_OFFSET_EMIF_ASYNC_BANK0	0x00000000

	#define	BOARD_CP1616__BAR4_SIZE_EMIF_ASYNC_BANK0	(1024*1024*8)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_CP1616__BAR5_OFFSET_EMIF_SDRAM	0x00000000

	#define	BOARD_CP1616__BAR5_SIZE_EMIF_SDRAM		(1024*1024*32)

	//------------------------------------------------------------------------
	//	SRAM
	//------------------------------------------------------------------------
	#define BOARD_CP1616__SRAM_OFFSET_COMMAND_SIGNAL	((1024*8) - 8)			// command/signal of SBL: last 8 bytes of SRAM (0x1ff8)

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 16Bit NOR flash AM29LV641/S29GL064N (1x 8MB):
	//	- used: 1x chip
	//			UsableSize:	8MB
	//			SectorSize:	64kB
	//			MacAdr at start of last sector
	#define	BOARD_CP1616__FLASH_SIZE_USABLE		(1024*1024*8)
	#define	BOARD_CP1616__FLASH_SIZE_SECTOR		(1024*64)
	#define BOARD_CP1616__FLASH_OFFSET_DATA		(0x00800000 - 0x00010000)		// offset=8MB-64k=0x007f_0000

	//========================================================================
	//	DB-EB400-PCIE board
	//========================================================================

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	#define		BOARD_EB400__SDRAM_BASE					(0x20000000) //EB400 integration
	#define		BOARD_EB400__SDRAM_BOOT_FW				(0x20001000)

	#define		BOARD_EB400__SDRAM_OFFSET_ALIVE			(0x400)
	#define		BOARD_EB400__SDRAM_OFFSET_IDENT			(0x404)
	#define		BOARD_EB400__SDRAM_OFFSET_VERSION_MIN	(0x408)
	#define		BOARD_EB400__SDRAM_OFFSET_VERSION_MAJ	(0x40A)
	#define		BOARD_EB400__SDRAM_OFFSET_RES1			(0x40C)
	
	#define		BOARD_EB400__SDRAM_BIN_CMD				(0x420)
	#define		BOARD_EB400__SDRAM_BIN_FW				(0x424)
	#define		BOARD_EB400__SDRAM_BIN_STATUS			(0x428)
	#define		BOARD_EB400__SDRAM_BIN_EXECUTE	 		(0x00000002)


	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 16Bit NOR flash S29GL064N (1x 8MB):
	//	- used: 1x chip
	//			UsableSize:	8MB
	//			SectorSize:	64kB
	//			MacAdr at start of last sector
	#define	BOARD_EB400__FLASH_SIZE_USABLE		(1024*1024*8)
	#define	BOARD_EB400__FLASH_SIZE_SECTOR		(1024*64)
	#define BOARD_EB400__FLASH_OFFSET_DATA		(0x00800000 - 0x00010000)		// offset=8MB-64k=0x007f_0000

	//========================================================================
	//	EB200 board / EB200-PCIe board
	//========================================================================

	//------------------------------------------------------------------------
	//	PCIe Board Register
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	// HOSTIF/XHIF Register:
	//	- addressing by CS-R: only XHIF-PageCfg Register visible
	#define ERTEC200_XHIF_REG_CSR__PAGE0_RANGE		0x00
 	#define ERTEC200_XHIF_REG_CSR__PAGE0_OFFSET		0x04
 	#define ERTEC200_XHIF_REG_CSR__PAGE0_CFG		0x08
 	#define ERTEC200_XHIF_REG_CSR__PAGE1_RANGE		0x10
 	#define ERTEC200_XHIF_REG_CSR__PAGE1_OFFSET		0x14
 	#define ERTEC200_XHIF_REG_CSR__PAGE1_CFG		0x18
 	#define ERTEC200_XHIF_REG_CSR__PAGE2_RANGE		0x20
 	#define ERTEC200_XHIF_REG_CSR__PAGE2_OFFSET		0x24
 	#define ERTEC200_XHIF_REG_CSR__PAGE2_CFG		0x28
 	#define ERTEC200_XHIF_REG_CSR__PAGE3_RANGE		0x30
 	#define ERTEC200_XHIF_REG_CSR__PAGE3_OFFSET		0x34
 	#define ERTEC200_XHIF_REG_CSR__PAGE3_CFG		0x38
 	#define ERTEC200_XHIF_REG_CSR__VERSION_BLOCK0	0x3c
 	#define ERTEC200_XHIF_REG_CSR__PAGE4_RANGE		0x40
 	#define ERTEC200_XHIF_REG_CSR__PAGE4_OFFSET		0x44
 	#define ERTEC200_XHIF_REG_CSR__PAGE4_CFG		0x48
 	#define ERTEC200_XHIF_REG_CSR__PAGE5_RANGE		0x50
 	#define ERTEC200_XHIF_REG_CSR__PAGE5_OFFSET		0x54
 	#define ERTEC200_XHIF_REG_CSR__PAGE5_CFG		0x58
 	#define ERTEC200_XHIF_REG_CSR__PAGE6_RANGE		0x60
 	#define ERTEC200_XHIF_REG_CSR__PAGE6_OFFSET		0x64
 	#define ERTEC200_XHIF_REG_CSR__PAGE6_CFG		0x68
 	#define ERTEC200_XHIF_REG_CSR__PAGE7_RANGE		0x70
 	#define ERTEC200_XHIF_REG_CSR__PAGE7_OFFSET		0x74
 	#define ERTEC200_XHIF_REG_CSR__PAGE7_CFG		0x78
 	#define ERTEC200_XHIF_REG_CSR__VERSION_BLOCK1	0x7c

	typedef enum _eBOARD_EB200__LBU_DYN_PAGE
	{
		eBOARD_EB200__LBU_DYN_PAGE_INVALID			= 0,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_REG			= 1,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM		= 2,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0	= 3,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK1	= 4,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK2	= 5,
		eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK3	= 6
	}
	eBOARD_EB200__LBU_DYN_PAGE;	

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_EB200__BAR0_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR1_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR2_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR3_SIZE	(1024*1024*2)
	#define	BOARD_EB200__BAR4_SIZE	(1024*64)
	#define	BOARD_EB200__BAR5_SIZE	(1024*64)
	
	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	#define	BOARD_EB200_XHIF__BAR0_SIZE			(1024*4)
	#define	BOARD_EB200_XHIF__BAR1_SIZE			(1024*4)
	#define	BOARD_EB200_XHIF__BAR2_SIZE			(1024*1024*16)
	#define	BOARD_EB200_XHIF__BAR3_SIZE			0
	#define	BOARD_EB200_XHIF__BAR4_SIZE			0
	#define	BOARD_EB200_XHIF__BAR5_SIZE			0
	              
	#define	BOARD_EB200_XHIF__PAGE_SIZE_1MB		(1024*1024*1)
	#define	BOARD_EB200_XHIF__PAGE_SIZE_2MB		(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR0_OFFSET_LBU_PAGE0	0x00000000

	#define	BOARD_EB200__BAR0_SIZE_LBU_PAGE0	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR1_OFFSET_LBU_PAGE1	0x00000000

	#define	BOARD_EB200__BAR1_SIZE_LBU_PAGE1	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR2_OFFSET_LBU_PAGE2	0x00000000

	#define	BOARD_EB200__BAR2_SIZE_LBU_PAGE2	(1024*1024*2)
	
	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	// each page can address a maximal window of 2MB
	#define	BOARD_EB200_XHIF__BAR2_OFFSET_PAGE0	0
	#define	BOARD_EB200_XHIF__BAR2_OFFSET_PAGE1	(1024*1024*2)
	#define	BOARD_EB200_XHIF__BAR2_OFFSET_PAGE2	(1024*1024*4)
	#define	BOARD_EB200_XHIF__BAR2_OFFSET_PAGE3	(1024*1024*6)

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR3_OFFSET_LBU_PAGE3	0x00000000

	#define	BOARD_EB200__BAR3_SIZE_LBU_PAGE3	(1024*1024*2)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR4_OFFSET_LBU_REG	0x00000000

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_EB200__BAR5_OFFSET_PCI_BRIDGE_REG	0x00000000

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------
	#define BOARD_EB200_PCI_BRIDGE_REG__MASTER_OFFSET		0x00000000
	#define BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MAJOR	0x00000004
	#define BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MINOR	0x00000008			// introduced May 2013
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR0_CONS			0x00000010
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR1_CONS			0x00000014
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR2_CONS			0x00000018
	#define BOARD_EB200_PCI_BRIDGE_REG__BAR3_CONS			0x0000001c

	#define BOARD_EB200_PCI_BRIDGE_REG__SIZE				0x20

	// Register BARx_CONS
	#define BOARD_EB200_PCI_BRIDGE_BARX_CONS__32BIT	0x00000001

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 1x 8Bit NOR flash AM29LV040B (1x 512kB):
	//	- used: 1x chip
	//			UsableSize:	512kB
	//			SectorSize:	64kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_EB200__FLASH_SIZE_USABLE			(1024*512)
	#define	BOARD_EB200__FLASH_SIZE_SECTOR			(1024*64)
	#define BOARD_EB200__FLASH_OFFSET_DATA			(0x00080000 - 0x00001000)	// page0, offset=512kB-4k=0x0007_f000
	
	//------------------------------------------------------------------------
	//	FLASH-PCIe
	//------------------------------------------------------------------------
	// 1x 16Bit NOR flash STM29W320EB (1x 2MB):
	//	- used: 1x chip
	//			UsableSize:	2MB
	//			SectorSize:	64kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_EB200_PCIE__FLASH_SIZE_USABLE			(1024*1024*2)
	#define	BOARD_EB200_PCIE__FLASH_SIZE_SECTOR			(1024*64)
	#define BOARD_EB200_PCIE__FLASH_OFFSET_DATA			((1024*1024*2) - 0x00001000)	// offset=2MB-4k=0x001f_f000
	
	
	#define BOARD_EB200__FLASH_OFFSET_FW_IDENT		0x00000024					// page0
	#define BOARD_EB200__FLASH_OFFSET_FW_VERSION	0x00000020					// page0

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
#ifdef EB200_NEW_BOOTLOADER
	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED		0						// page0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT		1						// page1

	#define BOARD_EB200__SDRAM_OFFSET_FW				(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0)	// jmp to this offset if semaphore=0x6879_9786
	#define BOARD_EB200__SDRAM_OFFSET_SEMAPHORE			(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 4)
	#define BOARD_EB200__SDRAM_OFFSET_ALIVE				(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 8)	// value will be inverted if changed by user
#else
	#define BOARD_EB200__SDRAM_PAGE_USER_FW				0						// page0, SWAP instruction can select SDRAM (CS) -> Arm9-Adr0 = Sdram-Adr0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED		0						// page0
	#define BOARD_EB200__SDRAM_PAGE_SHARED_DIRECT		1						// page1

	#define BOARD_EB200__SDRAM_PAGE_SHARED_RESERVED_TMP	31						// page31
	#define BOARD_EB200__SDRAM_OFFSET_SEMAPHORE			0x001ffff8
	#define BOARD_EB200__SDRAM_OFFSET_ALIVE				0x001ffffc				// value will be inverted if changed by user
	#define BOARD_EB200__SDRAM_OFFSET_FW				0x00000048				// jmp to this offset if semaphore=0x6879_9786
#endif

	//========================================================================
	//	DB-Soc1-PCI board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR0_SIZE_V2	(1024*8)
	#define	BOARD_SOC1_DEV__BAR1_SIZE_V2	(1024*1)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_V2	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR3_SIZE_V2	(1024*1024*8)
	#define	BOARD_SOC1_DEV__BAR4_SIZE_V2	(1024*1024*32)
	#define	BOARD_SOC1_DEV__BAR5_SIZE_V2	(1024*1024*64)

	#define	BOARD_SOC1_DEV__BAR0_SIZE_V3	(1024*8)
	#define	BOARD_SOC1_DEV__BAR1_SIZE_V3	(1024*1)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_V3	(1024*1024*64)
	#define	BOARD_SOC1_DEV__BAR3_SIZE_V3	(1024*1024*8)
	#define	BOARD_SOC1_DEV__BAR4_SIZE_V3	(1024*1024*32)
	#define	BOARD_SOC1_DEV__BAR5_SIZE_V3	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR0_OFFSET_TPA_RAM	0x00000000

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR1_OFFSET_TRACE_TIMER_UNIT	0x00000000

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0	0x00000000
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS1	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0)
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS2	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1)
	#define	BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS3	(BOARD_SOC1_DEV__BAR2_OFFSET_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1 + BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS2)

	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS0	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS1	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS2	(1024*1024*16)
	#define	BOARD_SOC1_DEV__BAR2_SIZE_LOCAL_BUS3	(1024*1024*16)

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_LOCAL_BUS_REG	(SOC1_AHB_ADR__LOCAL_BUS_REG	- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_IRTE			(SOC1_AHB_ADR__IRTE				- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_PB_COM_RAM		(SOC1_AHB_ADR__PB_COM_RAM		- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_PCI_BRIDGE_REG	(SOC1_AHB_ADR__PCI_BRIDGE_REG	- 0x1d000000)
	#define	BOARD_SOC1_DEV__BAR3_OFFSET_IOCC			(SOC1_AHB_ADR__IOCC				- 0x1d000000)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_MMC_SD			(SOC1_AHB_ADR__MMC_SD		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_GDMA			(SOC1_AHB_ADR__GDMA			- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_GICU			(SOC1_AHB_ADR__GICU			- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_PCI_ICU			(SOC1_AHB_ADR__PCI_ICU		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_ECC_ICU			(SOC1_AHB_ADR__ECC_ICU		- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_DDR2C_REG		(SOC1_AHB_ADR__DDR2C_REG	- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_DSA_SOFT_PLL	(SOC1_AHB_ADR__DSA_SOFT_PLL	- 0x1e000000)
	#define	BOARD_SOC1_DEV__BAR4_OFFSET_APB_PER			(SOC1_AHB_ADR__APB_PER		- 0x1e000000)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_SOC1_DEV__BAR5_OFFSET_DDR2_SDRAM	0x00000000

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 8Bit NAND flash K91G08U0B (2x 128MB):
	//	- used:	1x chip	
	//			UsableSize:		128MB
	//			BlockSize:		128kB
	//			NandPageSize:	2kB
	//			MacAdr at first good of last 8 reserved blocks (1MB)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_USABLE		(1024*1024*128)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_SECTOR		(1024*128)
	#define	BOARD_SOC1_DEV__FLASH_SIZE_PAGE			(1024*2)
	#define BOARD_SOC1_DEV__FLASH_OFFSET_DATA		(0x08000000 - 0x00100000)	// offset=128MB-(8*128k)=0x07f0_0000

	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__FLASH_OFFSET_DATA_REG	0x00						// data
	#define BOARD_SOC1_DEV__FLASH_OFFSET_CMD_REG	0x40						// Command Register
	#define BOARD_SOC1_DEV__FLASH_OFFSET_ADR_REG	0x80						// Address Register

	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__FLASH_SIZE_RSC_DATA		249		
	#define BOARD_SOC1_DEV__FLASH_SIZE_RSC_CHECK	6	

	//------------------------------------------------------------------------
	//	TPA-RAM
	//------------------------------------------------------------------------
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_SEMAPHORE		0x00000000
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_CODE				0x00000004

	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_TYPE		(0x17f0 + 0x00)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MAJOR	(0x17f0 + 0x01)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_MINOR	(0x17f0 + 0x02)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_VERSION_BUILD	(0x17f0 + 0x03)		// 8Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_HW				(0x17f0 + 0x04)		// 16Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_BAR_CFG			(0x17f0 + 0x06)		// 16Bit
	#define BOARD_SOC1_DEV__TPA_OFFSET_SBL_PRODUCT			(0x17f0 + 0x08)		// 8Bit

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//========================================================================
	//	CP1625 board 
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_CP1625__BAR0_SIZE			(1024*8)
	#define	BOARD_CP1625__BAR1_SIZE			(1024*1)
	#define	BOARD_CP1625__BAR2_SIZE			(0)
	#define	BOARD_CP1625__BAR3_SIZE			(1024*1024*8)
	#define	BOARD_CP1625__BAR4_SIZE			(1024*1024*32)
	#define	BOARD_CP1625__BAR5_SIZE			(1024*1024*64)

	//------------------------------------------------------------------------
	//	SSP (Serializer)  / GPIOs
	//------------------------------------------------------------------------

    #define BOARD_CP1625__GPIO_IOCTRL_0          0x00      // Configuration register for General Purpose IOs (31:0) --> 0: GPIOx = output, 1: GPIOx = input
    #define BOARD_CP1625__GPIO_OUT_0             0x04      // Output register for General Purpose IOs (31:0) --> 0: GPIO outputx = 0, 1: GPIO outputx = 1
    #define BOARD_CP1625__GPIO_OUT_SET_0         0x08      // Bit-selective setting of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 1 / Read always returns 0
    #define BOARD_CP1625__GPIO_OUT_CLEAR_0       0x0C      // Bit-selective reset of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 0 / Read always returns 0
    #define BOARD_CP1625__GPIO_RES_DIS_0         0x10      // Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0)
    #define BOARD_CP1625__GPIO_IN_0              0x14      // Input register for General Purpose IOs (31:0)
    #define BOARD_CP1625__GPIO_PORT_MODE_0_L     0x18      // Configuration register for GPIO port (15:0) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
    #define BOARD_CP1625__GPIO_PORT_MODE_0_H     0x1C      // Configuration register for GPIO port (31:16) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
	#define BOARD_CP1625__GPIO_IOCTRL_1          0x20      // Configuration register for General Purpose IOs (63:32) --> 0: GPIOx = output, 1: GPIOx = input	
	#define BOARD_CP1625__GPIO_OUT_1             0x24      // Output register for General Purpose IOs (63:48) --> 0: GPIO outputx = 0, 1: GPIO outputx = 1
	#define BOARD_CP1625__GPIO_OUT_SET_1         0x28      // Bit-selective setting of the output register for General Purpose IOs (63:48) --> For writing
	#define BOARD_CP1625__GPIO_OUT_CLEAR_1       0x2C      // Bit-selective reset of the output register for General Purpose IOs (63:32) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 0 / Read always returns 0
	#define BOARD_CP1625__GPIO_PORT_MODE_1_H     0x3C      // Configuration register for GPIO port (63:48) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"	

    #define BOARD_CP1625__SSP_CR0                0x00      // Control Register 0 [15:0]
    #define BOARD_CP1625__SSP_CR1                0x04      // Control Register 1 [15:0]
    #define BOARD_CP1625__SSP_DR                 0x08      // Data Register r/w  [15:0]
    #define BOARD_CP1625__SSP_SR                 0x0C      // Status Register    [4:0]
    #define BOARD_CP1625__SSP_CPSR               0x10      // Clock prescale register r/w  [7:0]

    /* USER settings */    
	#define BOARD_CP1625__GPIO_PORT_MODE_1_H_SETTINGS       0x01500000	// Reg GPIO_PORT_MODE_1_H -> GPIOs[58:60] alternate function 0 (SPI_3); GPIO[61] gpio function for CS
																		// make sure that bits [20:25] are set to 01

    #define BOARD_CP1625__SSP_CR0_SETTINGS                  0x00001707  // serial clock rate 0x31 (=49), Motorola frame format, SPH/SPO = 0, data width 8 Bit
    #define BOARD_CP1625__SSP_CPSR_SETTINGS                 0x0002      // Clock Prescale Devisor 0x2; results in a 500 kHz clock
    #define BOARD_CP1625__SSP_DUMMY_SEND                    0x00        // dummy byte

	//------------------------------------------------------------------------
	//	SPI FLASH
	//------------------------------------------------------------------------

    /* USER settings */
    #define BOARD_CP1625_SPI_FLASH__SIZE_USABLE             (SPI_FLASH_M25P10A__SIZE - SPI_FLASH_M25P10A__PAGE_SIZE) // 1 MB - 256Bytes using one page for remanent data
    #define BOARD_CP1625_SPI_FLASH__PAGE_DATA				(SPI_FLASH_M25P10A__PAGE_COUNT - 1) // using last page for remanent data
    #define BOARD_CP1625_SPI_FLASH__OFFSET_DATA             ( (SPI_FLASH_M25P10A__SECTOR_COUNT-1) * SPI_FLASH_M25P10A__SECTOR_SIZE) // adress of sector for remanent data

	//------------------------------------------------------------------------
	//	NAND - FLASH
	//------------------------------------------------------------------------
	// 2x 8Bit NAND flash TC58NVG0S3E (2x 128MB):
	//	- used:	1x chip	
	//			UsableSize:		128MB
	//			BlockSize:		128kB
	//			NandPageSize:	2kB

	#define	BOARD_CP1625__NAND_FLASH_SIZE_USABLE		(1024*1024*128)
	#define	BOARD_CP1625__NAND_FLASH_SIZE_SECTOR		(1024*128)
	#define	BOARD_CP1625__NAND_FLASH_SIZE_PAGE			(1024*2)
	#define	BOARD_CP1625__NAND_FLASH_SIZE_SPARE			(64)

	//------------------------------------------------------------------------
	#define BOARD_CP1625__NAND_FLASH_OFFSET_DATA_REG	0x00						// data
	#define BOARD_CP1625__NAND_FLASH_OFFSET_CMD_REG		0x40						// Command Register
	#define BOARD_CP1625__NAND_FLASH_OFFSET_ADR_REG		0x80						// Address Register

#endif

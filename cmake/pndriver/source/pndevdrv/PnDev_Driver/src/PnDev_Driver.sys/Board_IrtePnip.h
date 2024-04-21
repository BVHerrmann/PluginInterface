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
/*  F i l e               &F: Board_IrtePnip.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common declarations for boards with IRTE or PNIP
//
//****************************************************************************/

#ifndef __Board_IrtePnip_h__
#define __Board_IrtePnip_h__

	//########################################################################
	//  Defines
	//########################################################################

	//------------------------------------------------------------------------
	//	Board Register
	//------------------------------------------------------------------------
	#define BOARD_ADMIN_REG__ID						0x00000000
	#define BOARD_ADMIN_REG__REVISION				0x00000004
	#define BOARD_ADMIN_REG__STATUS					0x00000008
	#define BOARD_ADMIN_REG__CONTROL				0x00000020
	#define BOARD_ADMIN_REG__SCRATCH				0x00000024
	#define BOARD_ADMIN_REG__RESET					0x00000028
	#define BOARD_ADMIN_REG__XHIF_CONFIG			0x00000030
	#define BOARD_ADMIN_REG__INT_MASK				0x00000034
	#define BOARD_ADMIN_REG__INT_STATUS_ACK			0x00000038
	#define BOARD_ADMIN_REG__EMC_SDRAM_REFRESH		0x00000100
	#define BOARD_ADMIN_REG__EMC_ASYNC_WAIT_CYCLE	0x00000104
	#define BOARD_ADMIN_REG__EMC_PM					0x00000108
	#define BOARD_ADMIN_REG__EMC_BF					0x0000010c
	#define BOARD_ADMIN_REG__EMC_ASYNC_BANK0		0x00000110
	#define BOARD_ADMIN_REG__EMC_ASYNC_BANK1		0x00000114
	#define BOARD_ADMIN_REG__EMC_ASYNC_BANK2		0x00000118
	#define BOARD_ADMIN_REG__EMC_ASYNC_BANK3		0x0000011c
	#define BOARD_ADMIN_REG__EMC_LPEMR				0x00000120
	#define BOARD_ADMIN_REG__EMC_EXTENDED			0x00000124
	#define BOARD_ADMIN_REG__EMC_SDRAM_CONFIG		0x00000128
	#define BOARD_ADMIN_REG__EMC_RECOVERY			0x0000012c

	//------------------------------------------------------------------------
	// Bits of BoardStatus register
	#define	BOARD_ADMIN_STATUS_RESET		PNDEV_UINT32_SET_BIT_0
	#define	BOARD_ADMIN_STATUS_PLL_LOC		PNDEV_UINT32_SET_BIT_1
	#define	BOARD_ADMIN_STATUS_INT			PNDEV_UINT32_SET_BIT_2
	#define	BOARD_ADMIN_STATUS_DMA_TIMEOUT	PNDEV_UINT32_SET_BIT_3				// PCIe access not finished by Asic -> Timeout
	#define	BOARD_ADMIN_STATUS_MSI			PNDEV_UINT32_SET_BIT_4

	//------------------------------------------------------------------------
	// Bits of BoardControl register
	#define	BOARD_ADMIN_CONTROL_EOI			PNDEV_UINT32_SET_BIT_1
	#define	BOARD_ADMIN_CONTROL_INT_ENABLE	PNDEV_UINT32_SET_BIT_2
	#define	BOARD_ADMIN_CONTROL_PAGE_CS_R	PNDEV_UINT32_SET_BIT_3

	//------------------------------------------------------------------------
	// Bits of IntMask register
	#define	BOARD_ADMIN_INT_MASK_SET_DEMO_INT			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_ADMIN_INT_MASK_ENABLE_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_1
	#define	BOARD_ADMIN_INT_MASK_ENABLE_GPIO_ERTEC200P	PNDEV_UINT32_SET_BIT_2
	#define	BOARD_ADMIN_INT_MASK_ENABLE_DMA				PNDEV_UINT32_SET_BIT_3
	#define	BOARD_ADMIN_INT_MASK_ENABLE_DMA_ERROR		PNDEV_UINT32_SET_BIT_4
	#define	BOARD_ADMIN_INT_MASK_ENABLE_XHIF_HOST		PNDEV_UINT32_SET_BIT_5

	//------------------------------------------------------------------------
	// Bits of IntStatus/Ack register
	#define	BOARD_ADMIN_INT_STATUS_ACK_DEMO_INT			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_ADMIN_INT_STATUS_ACK_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_1
	#define	BOARD_ADMIN_INT_STATUS_ACK_GPIO				PNDEV_UINT32_SET_BIT_2
	#define	BOARD_ADMIN_INT_STATUS_ACK_DMA				PNDEV_UINT32_SET_BIT_3
	#define	BOARD_ADMIN_INT_STATUS_ACK_DMA_ERROR		PNDEV_UINT32_SET_BIT_4
	#define	BOARD_ADMIN_INT_STATUS_ACK_XHIF_HOST		PNDEV_UINT32_SET_BIT_5

	//========================================================================
	//	FPGA1-Xx board
	//========================================================================

	//------------------------------------------------------------------------
	//	Board Register
	//------------------------------------------------------------------------
	#define BOARD_FPGA1_REG__ID					0x00000000
	#define BOARD_FPGA1_REG__REVISION			0x00000004
	#define BOARD_FPGA1_REG__STATUS				0x00000008
	#define BOARD_FPGA1_REG__STREAM_ID			0x0000000c
	#define BOARD_FPGA1_REG__STREAM_VERSION		0x00000010
	#define BOARD_FPGA1_REG__CONTROL			0x00000020
	#define BOARD_FPGA1_REG__SCRATCH			0x00000024
	#define BOARD_FPGA1_REG__RESET				0x00000028
	#define BOARD_FPGA1_REG__BOOT_MODE			0x0000002c
	#define BOARD_FPGA1_REG__CONFIG				0x00000030

	//------------------------------------------------------------------------
	typedef enum _eBOARD_ID
	{
		eBOARD_ID_INVALID						= 0,
		eBOARD_ID_FPGA1_ERTEC200P_AHB_STD		= 0x00000001,
		eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD		= 0x00000002,
		eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200		= 0x00000003,
		eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200	= 0x00000004,
		eBOARD_ID_FPGA1_ERTEC400				= 0x00001000,
		eBOARD_ID_FPGA1_ERTEC200				= 0x00001001,
		eBOARD_ID_FPGA1_SOC1					= 0x00001002,
		eBOARD_ID_EB200P						= 0x00002000,
		eBOARD_ID_EB200_PCIE					= 0x00001f00
	}
	eBOARD_ID;

	//------------------------------------------------------------------------
	// Bits of BoardStatus register
	#define	BOARD_FPGA1_STATUS_RESET			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_FPGA1_STATUS_PLL_LOC			PNDEV_UINT32_SET_BIT_1
	#define	BOARD_FPGA1_STATUS_INT				PNDEV_UINT32_SET_BIT_2
	#define	BOARD_FPGA1_STATUS_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_3			// PCIe access not finished by Asic -> Timeout

	//------------------------------------------------------------------------
	// Bits of BoardControl register
	#define	BOARD_FPGA1_CONTROL_SET_DEMO_INT			PNDEV_UINT32_SET_BIT_0
	#define	BOARD_FPGA1_CONTROL_ENABLE_INT				PNDEV_UINT32_SET_BIT_1
	#define	BOARD_FPGA1_CONTROL_XHIF_BUS_SIZE32			PNDEV_UINT32_SET_BIT_2
	#define	BOARD_FPGA1_CONTROL_PAGE_CS_R				PNDEV_UINT32_SET_BIT_3
	#define	BOARD_FPGA1_CONTROL_RESET_ACCESS_TIMEOUT	PNDEV_UINT32_SET_BIT_4

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 16Bit NOR flash S29GL512P (2x 64MB):
	//	- used: 1x chip, 1MB (PCIe-XHIF: page0 -> 2MB would be possible
	//						  PCIe-AHB:  no pages, but only 1MB (BAR5))
	//			UsableSize:	1MB
	//			SectorSize:	128kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_FPGA1__FLASH_SIZE_USABLE	(1024*1024*1)
	#define	BOARD_FPGA1__FLASH_SIZE_SECTOR	(1024*128)
	#define	BOARD_FPGA1__FLASH_OFFSET_DATA	(0x00100000 - 0x00001000)			// offset=1MB-4k=0x000f_f000

	//========================================================================
	//	EB200P board
	//========================================================================

	//------------------------------------------------------------------------
	//	Board Register
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	FLASH
	//------------------------------------------------------------------------
	// 2x 16Bit NOR flash S29WS128P (2x 16MB):
	//	- used: 2x chip, 2MB
	//			UsableSize:	2MB
	//			SectorSize:	128kB
	//			MacAdr at last 4kB of last sector
	#define	BOARD_EB200P__FLASH_SIZE_USABLE	(1024*1024*2)
	#define	BOARD_EB200P__FLASH_SIZE_SECTOR	(1024*128)
	#define	BOARD_EB200P__FLASH_OFFSET_DATA	(0x00200000 - 0x00001000)			// page15, offset=2MB-4k=0x001f_f000

	//========================================================================
	//	FPGA1-Ertec400 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//========================================================================
	//	FPGA1-Ertec200 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR0_SIZE		(1024*4)
	#define	BOARD_FPGA1_ERTEC200__BAR1_SIZE		(1024*1024*8)
	#define	BOARD_FPGA1_ERTEC200__BAR2_SIZE		(1024*1024*16)
	#define	BOARD_FPGA1_ERTEC200__BAR3_SIZE		(1024*1024*64)
	#define	BOARD_FPGA1_ERTEC200__BAR4_SIZE		(1024*1024*1)
	#define	BOARD_FPGA1_ERTEC200__BAR5_SIZE		(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_ARM_ICU	0x00000000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_IRTE		0x00500000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_EMC_REG	0x00b00000
	#define	BOARD_FPGA1_ERTEC200__BAR2_OFFSET_APB_PER	0x00c00000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR3_OFFSET_EMC_SDRAM	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR3_SIZE_EMC_SDRAM	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR4_OFFSET_EMC_ASYNC_BANK0	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR4_SIZE_EMC_ASYNC_BANK0		(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_ERTEC200__BAR5_OFFSET_EMC_ASYNC_BANK1	0x00000000

	#define	BOARD_FPGA1_ERTEC200__BAR5_SIZE_EMC_ASYNC_BANK1		(1024*1024*1)

	//========================================================================
	//	FPGA1-Soc1 board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//========================================================================
	//	FPGA1-Ertec200P / EB200P board
	//========================================================================

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_FPGA1_ERTEC200P__BAR0_SIZE				(1024*4)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	#define	BOARD_ERTEC200P_XHIF__BAR1_SIZE			(1024*4)
	#define	BOARD_ERTEC200P_XHIF__BAR2_SIZE			(1024*1024*16)
	#define	BOARD_ERTEC200P_XHIF__BAR3_SIZE			0
	#define	BOARD_ERTEC200P_XHIF__BAR4_SIZE			0
	#define	BOARD_ERTEC200P_XHIF__BAR5_SIZE			0

	#define	BOARD_ERTEC200P_XHIF__PAGE_SIZE_1MB		(1024*1024*1)
	#define	BOARD_ERTEC200P_XHIF__PAGE_SIZE_2MB		(1024*1024*2)
	#define	BOARD_ERTEC200P_XHIF__PAGE_OFFSET_TTU	(1024*1024*1)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE			(1024*1024*8)
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_ET200	(1024*1024*16)
	#define	BOARD_ERTEC200P_AHB__BAR2_SIZE			(1024*1024*16)
	#define	BOARD_ERTEC200P_AHB__BAR3_SIZE			(1024*1024*64)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE			(1024*1024*1)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_ET200	(1024*1024*2)
	#define	BOARD_ERTEC200P_AHB__BAR5_SIZE			(1024*1024*1)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_EMC_ASYNC_BANK2		0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM			0x00400000
	#define	BOARD_ERTEC200P_AHB__BAR1_OFFSET_KRISC32_TCM_ET200		0x00800000

	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2			(1024*1024*4)
	#define	BOARD_ERTEC200P_AHB__BAR1_SIZE_EMC_ASYNC_BANK2_ET200	(1024*1024*8)

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	// each page can address a maximal window of 2MB
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE0	0
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE1	(1024*1024*2)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE2	(1024*1024*4)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE3	(1024*1024*6)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE4	(1024*1024*8)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE5	(1024*1024*10)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE6	(1024*1024*12)
	#define	BOARD_ERTEC200P_XHIF__BAR2_OFFSET_PAGE7	(1024*1024*14)

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_ARM_ICU	0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_PNIP		0x00500000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_PERIF		0x00700000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_GDMA		0x00800000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_TTU		0x00900000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_SD_MMC		0x00a00000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_EMC_REG	0x00b00000
	#define	BOARD_ERTEC200P_AHB__BAR2_OFFSET_APB_PER	0x00c00000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR3_OFFSET_EMC_SDRAM	0x00000000

	#define	BOARD_ERTEC200P_AHB__BAR3_SIZE_EMC_SDRAM	(1024*1024*64)

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK0		0x00000000
	#define	BOARD_ERTEC200P_AHB__BAR4_OFFSET_EMC_ASYNC_BANK3_ET200	0x00100000

	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK0			(1024*1024*1)
	#define	BOARD_ERTEC200P_AHB__BAR4_SIZE_EMC_ASYNC_BANK3_ET200	(1024*1)

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:

	//////////////////////////////////////////////////////////////////////////
	// PCIe-AHB:
	#define	BOARD_ERTEC200P_AHB__BAR5_OFFSET_EMC_ASYNC_BANK1	0x00000000

	#define	BOARD_ERTEC200P_AHB__BAR5_SIZE_EMC_ASYNC_BANK1		(1024*1024*1)

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	// PCIe-XHIF:
	#define BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_RESERVED	0				// page0
	#define BOARD_ERTEC200P_XHIF__SDRAM_PAGE_SHARED_DIRECT		1				// page1

	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_FW			(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 0)	// jmp to this offset if semaphore=0x6879_9786
	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_SEMAPHORE	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 4)
	//#define BOARD_ERTEC200P_XHIF__SDRAM_OFFSET_HANDSHAKE	(PNCORE_SHARED_ASIC_SDRAM__OFFSET_BOOT + 8)	// value will be inverted if changed by user
	
	//========================================================================
	//	FPGA1-HERA board
	//========================================================================
	
	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------
	
	// Note:
	//	- only the bars with even number are use, because the bars have a width of 64bit	
	
	#define	BOARD_FPGA1_HERA__BAR0_SIZE				(1024*128)
	#define	BOARD_FPGA1_HERA__BAR1_SIZE				(1024*0)
	#define	BOARD_FPGA1_HERA__BAR2_SIZE				(1024*1024*16)
	#define	BOARD_FPGA1_HERA__BAR3_SIZE				(1024*0)
	#define	BOARD_FPGA1_HERA__BAR4_SIZE				(1024*1024*128)
	#define	BOARD_FPGA1_HERA__BAR5_SIZE				(1024*0)

	#define	BOARD_FPGA1_HERA__BAR0_SIZE_INC12		(1024*1024*1)
	
	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM			0x00000000
	#define	BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_KRISC_DDR3SDRAM	(0x07F00000 + BOARD_FPGA1_HERA_AHB__BAR4_OFFSET_DDR3SDRAM)  // 127MB + 0KB

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------

	// Inc 12
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_XGDMA				0x00800000

	// PNIP_A / PNIP_B
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_PNIP_IFA			0x00400000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_PNIP_IFB			0x00600000	

	// CORE_A Inc 9 - TODO Remove	
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_BTCM_INC9	0x00D00000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_TIMER_INC9	0x00D08000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_VIC_INC9		0x00D0C000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_ATCM_INC9	0x00D10000
	
	// CORE_B Inc 9 - TODE Remove	
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_BTCM_INC9	0x00E00000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_TIMER_INC9	0x00E08000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_VIC_INC9		0x00E0C000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_ATCM_INC9	0x00E10000

	// CORE_A Inc 12
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_BTCM		0x00D00000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_TIMER	0x00D10000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_VIC		0x00D18000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREA_ATCM		0x00D20000

	// CORE_B Inc 12
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_BTCM		0x00E00000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_TIMER	0x00E10000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_VIC		0x00E18000
	#define	BOARD_FPGA1_HERA_AHB__BAR2_OFFSET_R4_COREB_ATCM		0x00E20000
	

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define	BOARD_FPGA1_HERA_AHB__BAR0_OFFSET_APB_PER			0x00000000
	
	//------------------------------------------------------------------------
	//	SSP (Serializer)  / GPIOs
	//------------------------------------------------------------------------

    #define BOARD_FPGA1_HERA__GPIO_IOCTRL_0          0x00      // Configuration register for General Purpose IOs (31:0) --> 0: GPIOx = output, 1: GPIOx = input
    #define BOARD_FPGA1_HERA__GPIO_OUT_0             0x04      // Output register for General Purpose IOs (31:0) --> 0: GPIO outputx = 0, 1: GPIO outputx = 1
    #define BOARD_FPGA1_HERA__GPIO_OUT_SET_0         0x08      // Bit-selective setting of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 1 / Read always returns 0
    #define BOARD_FPGA1_HERA__GPIO_OUT_CLEAR_0       0x0C      // Bit-selective reset of the output register for General Purpose IOs (31:0) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 0 / Read always returns 0
    #define BOARD_FPGA1_HERA__GPIO_RES_DIS_0         0x10      // Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0)
    #define BOARD_FPGA1_HERA__GPIO_IN_0              0x14      // Input register for General Purpose IOs (31:0)
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_L     0x18      // Configuration register for GPIO port (15:0) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_H     0x1C      // Configuration register for GPIO port (31:16) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
	#define BOARD_FPGA1_HERA__GPIO_IOCTRL_1          0x20      // Configuration register for General Purpose IOs (63:32) --> 0: GPIOx = output, 1: GPIOx = input	
	#define BOARD_FPGA1_HERA__GPIO_OUT_1             0x24      // Output register for General Purpose IOs (63:48) --> 0: GPIO outputx = 0, 1: GPIO outputx = 1
	#define BOARD_FPGA1_HERA__GPIO_OUT_SET_1         0x28      // Bit-selective setting of the output register for General Purpose IOs (63:48) --> For writing
	#define BOARD_FPGA1_HERA__GPIO_OUT_CLEAR_1       0x2C      // Bit-selective reset of the output register for General Purpose IOs (63:32) --> For writing: 0: GPIO outputx remains unchanged / 1: GPIO outputx = 0 / Read always returns 0
	#define BOARD_FPGA1_HERA__GPIO_PORT_MODE_1_H     0x3C      // Configuration register for GPIO port (63:48) --> Function assignment: "00=GPIO function" "01=alternate function A" "10=alternate function B" "11=alternate function C"
	
	

    #define BOARD_FPGA1_HERA__SSP_CR0                0x00      // Control Register 0 [15:0]
    #define BOARD_FPGA1_HERA__SSP_CR1                0x04      // Control Register 1 [15:0]
    #define BOARD_FPGA1_HERA__SSP_DR                 0x08      // Data Register r/w  [15:0]
    #define BOARD_FPGA1_HERA__SSP_SR                 0x0C      // Status Register    [4:0]
    #define BOARD_FPGA1_HERA__SSP_CPSR               0x10      // Clock prescale register r/w  [7:0]

    /* USER settings */
    #define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_L_SETTINGS       0x0054      // Reg GPIO_PORT_MODE_0_L -> GPIOs[3:1] alternate function A (SPI_1); GPIO[0] gpio function for CS
	#define BOARD_FPGA1_HERA__GPIO_PORT_MODE_0_H_SETTINGS       0x15000000	// Reg GPIO_PORT_MODE_0_H -> GPIOs[28:30] alternate function A (SPI_3); GPIO[0] gpio function for CS
	#define BOARD_FPGA1_HERA__GPIO_PORT_MODE_1_H_SETTINGS       0x15000		// Reg GPIO_PORT_MODE_0_H -> GPIOs[54:56] alternate function A (SPI_3); GPIO[0] gpio function for CS

    #define BOARD_FPGA1_HERA__SSP_CR0_SETTINGS                  0x00003107  // serial clock rate 0x31 (=49), Motorola frame format, SPH/SPO = 0, data width 8 Bit
    #define BOARD_FPGA1_HERA__SSP_CPSR_SETTINGS                 0x0002      // Clock Prescale Devisor 0x2; results in a 500 kHz clock
    #define BOARD_FPGA1_HERA__SSP_DUMMY_SEND                    0x00        // dummy byte

	//------------------------------------------------------------------------
	//	SPI FLASH
	//------------------------------------------------------------------------

    /* USER settings */
    #define BOARD_FPGA1_HERA_SPI_FLASH__SIZE_USABLE             (SPI_FLASH_N25Q128A__SIZE - SPI_FLASH_N25Q128A__SECTOR_SIZE) // 128 MB - 64kB using one sector for remanent data
    #define BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_DATA             (SPI_FLASH_N25Q128A__SECTOR_COUNT - 1) // using last sector for remanent data
    #define BOARD_FPGA1_HERA_SPI_FLASH__OFFSET_DATA             (BOARD_FPGA1_HERA_SPI_FLASH__SECTOR_DATA * SPI_FLASH_N25Q128A__SECTOR_SIZE) // adress of sector for remanent data

#endif

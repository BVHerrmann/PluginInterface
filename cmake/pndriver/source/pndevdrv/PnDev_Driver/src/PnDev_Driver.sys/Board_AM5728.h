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
/*  F i l e               &F: Board_AM5728.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for boards with AM5728
//
//****************************************************************************/

#ifndef __Board_AM5728_h__
#define __Board_AM5728_h__

	//########################################################################
	//  Defines
	//########################################################################

	#define AM5728_AHB_ADR__L4PER1						0x48000000
    #define AM5728_AHB_ADR__UART3						0x48020000
	#define AM5728_AHB_ADR__TIMER						0x48032000
	#define AM5728_AHB_ADR__APB							0x48051000
	#define AM5728_AHB_ADR__GMAC_SW						0x48484000
	#define AM5728_AHB_ADR__CTRL_MODULE_CORE			0x4A002000
	#define AM5728_AHB_ADR__CM_CORE_AON					0x4A005000 
	#define AM5728_AHB_ADR__CM_CORE						0x4A008000
	#define AM5728_AHB_ADR__DMA_SYSTEM					0x4A056000
	#define AM5728_AHB_ADR__EDMA						0x4A161000
	#define AM5728_AHB_ADR__PRUSS1Config				0x4A175000
	#define AM5728_AHB_ADR__PRUSS1						0x4B200000
	#define AM5728_AHB_ADR__PRUSS2Config				0x4A177000
	#define AM5728_AHB_ADR__PRUSS2						0x4B280000
    #define AM5728_AHB_ADR__QSPI_REG					0x4B300000
	#define AM5728_AHB_ADR__PCIE_SS1					0x51000000
	#define AM5728_AHB_ADR__PCIE_SS2					0x51800000

	#define AM5728_AHB_ADR__EMIF_SDRAM					0x80000000
	#define AM5728_AHB_ADR__QSPI						0x5C000000

	#define AM5728_AHB_SIZE__L4PER1						(1024*8)
	#define AM5728_AHB_SIZE__UART3						(1024*8)
	#define AM5728_AHB_SIZE__TIMER						(1024*56)
	#define AM5728_AHB_SIZE__APB						(1024*140)
	#define AM5728_AHB_SIZE__GMAC_SW					(1024*20)
 	#define AM5728_AHB_SIZE__CTRL_MODULE_CORE			(1024*8)
    #define AM5728_AHB_SIZE__CM_CORE_AON				(1024*8)
    #define AM5728_AHB_SIZE__CM_CORE					(1024*12)
    #define AM5728_AHB_SIZE__DMA_SYSTEM					(1024*8)
    #define AM5728_AHB_SIZE__EDMA						(1024*16)
    #define AM5728_AHB_SIZE__PRUSS1Config				(1024*8)
    #define AM5728_AHB_SIZE__PRUSS1						(1024*512)
	#define AM5728_AHB_SIZE__PRUSS2Config				(1024*8)
	#define AM5728_AHB_SIZE__PRUSS2						(1024*512)
    #define AM5728_AHB_SIZE__QSPI_REG					(1024*1024)
	#define AM5728_AHB_SIZE__PCIE_SS1					(1024*1024*8)
	#define AM5728_AHB_SIZE__PCIE_SS2					(1024*1024*8)
   	#define AM5728_AHB_SIZE__EMIF_SDRAM					(1024*1024*64)
    #define AM5728_AHB_SIZE__QSPI						(1024*1024*64)

	//------------------------------------------------------------------------
	//	APB-Peripherals
	//------------------------------------------------------------------------

 	#define AM5728_APB_PER__OFFSET_GPIO		    0x00000000
 	#define AM5728_APB_PER__OFFSET_I2C1		    0x0001F000
 	#define AM5728_APB_PER__OFFSET_I2C2		    0x00021000
	#define AM5728_APB_PER__OFFSET_I2C3		    0x0000F000
	#define AM5728_APB_PER__OFFSET_UART		    0x00015000

 	#define AM5728_APB_PER__SIZE_GPIO			(1024*7*8)
 	#define AM5728_APB_PER__SIZE_I2C1			(1024*8)
	#define AM5728_APB_PER__SIZE_I2C2			(1024*8)
	#define AM5728_APB_PER__SIZE_I2C3			(1024*8)
    #define AM5728_APB_PER__SIZE_UART			(1024*5*8)

	//------------------------------------------------------------------------
	//	BAR0-5
	//------------------------------------------------------------------------

	#define	BOARD_TI_AM5728__BAR0_SIZE	(1024*1024*64)	
	#define	BOARD_TI_AM5728__BAR1_SIZE	(1024*0)
	#define	BOARD_TI_AM5728__BAR2_SIZE	(1024*1024*64)
	#define	BOARD_TI_AM5728__BAR3_SIZE	(1024*0)
	#define	BOARD_TI_AM5728__BAR4_SIZE	(1024*1024*64)
	#define	BOARD_TI_AM5728__BAR5_SIZE	(1024*0)

	//------------------------------------------------------------------------
	//	BAR0
	//------------------------------------------------------------------------
	#define BOARD_TI_AM5728__BAR0_IDX_FIRST_PART				0
	#define BOARD_TI_AM5728__BAR0_IDX_SECOND_PART				1
	#define BOARD_TI_AM5728__BAR0_OFFSET_FIRST_PART				(0x00000000)
	#define BOARD_TI_AM5728__BAR0_OFFSET_SECOND_PART			(0x02000000)
	#define	BOARD_TI_AM5728__BAR0_SIZE_FIRST_PART				(1024*1024*8)
	#define	BOARD_TI_AM5728__BAR0_SIZE_SECOND_PART				(1024*1024*32)

	#define	BOARD_TI_AM5728__BAR0_FIRST_PART_OFFSET_L4PER1				(0x00000000)
	#define	BOARD_TI_AM5728__BAR0_FIRST_PART_OFFSET_UART3				(0x00020000)
	#define	BOARD_TI_AM5728__BAR0_FIRST_PART_OFFSET_TIMER				(0x00032000)
	#define	BOARD_TI_AM5728__BAR0_FIRST_PART_OFFSET_APB					(0x00051000)
	#define	BOARD_TI_AM5728__BAR0_FIRST_PART_OFFSET_GMAC_SW				(0x00484000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_CTRL_MODULE_CORE	(0x00002000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_CM_CORE_AON		(0x00005000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_CM_CORE			(0x00008000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_DMA_SYSTEM			(0x00056000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_EDMA				(0x00161000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRUSS1Config		(0x00175000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRUSS1				(0x01200000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRUSS2Config		(0x00177000)
	#define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRUSS2				(0x01280000)

    #define	BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_QSPI_REG 			(0x01280000 + 0x80000)

	#define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTCTRL		(0x00E07D00)
	#define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTST			(0x00E07D04)
    #define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WCLR               (0x00E14024)
    #define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WCRR               (0x00E14028)
    #define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WDLY               (0x00E14044)
    #define BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR               (0x00E14048)

    #define BOARD_TI_AM5728__APB_OFFSET_GPIO2_OE							0x4134
    #define BOARD_TI_AM5728__APB_OFFSET_GPIO2_DATAOUT						0x413C

    #define BOARD_TI_AM5728__CM_CORE_OFFSET_CM_L3INSTR_L3_MAIN_2_CLKCTRL    0xE20
    #define BOARD_TI_AM5728__CM_CORE_OFFSET_CM_L3INSTR_L3_INSTR_CLKCTRL     0xE28
	
	//------------------------------------------------------------------------
	//	BAR1
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR2
	//------------------------------------------------------------------------
	#define	BOARD_TI_AM5728__BAR2_OFFSET_EMIF_SDRAM			0x00000000

	//------------------------------------------------------------------------
	//	BAR3
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	BAR4
	//------------------------------------------------------------------------
	#define	BOARD_TI_AM5728__BAR4_OFFSET_QSPI					0x00000000

	//------------------------------------------------------------------------
	//	BAR5
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	//	PCI-Bridge Register
	//------------------------------------------------------------------------
	#define BOARD_TI_AM5728__PCIE_SS1_PL_CONF								(0x700)
	#define BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_INDEX				(0x200 + BOARD_TI_AM5728__PCIE_SS1_PL_CONF)
	#define BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LIMIT			(0x214 + BOARD_TI_AM5728__PCIE_SS1_PL_CONF)
	#define BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_BASE		(0x20C + BOARD_TI_AM5728__PCIE_SS1_PL_CONF)
	#define BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET	    (0x218 + BOARD_TI_AM5728__PCIE_SS1_PL_CONF)

	//------------------------------------------------------------------------
	//	SPI FLASH
	//------------------------------------------------------------------------

    /***************************************************************************************************/
    /* CTRL_MODULE_CORE PAD Register Definitions */

    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A13       0x1474      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A14       0x1478      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A15       0x147C      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A16       0x1480      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A17       0x1484      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_A18       0x1488      // PAD configuration register
    #define BOARD_TI_AM5728__CTRL_CORE_PAD_GPMC_CS2       0x14B8      // PAD configuration register

    #define BOARD_TI_AM5728__CTRL_CORE_PAD_SET_TO_QSPI    0x1         // value to set to activate QSPI pad multiplexing

    
    /***************************************************************************************************/
    /* QSPI Register Definitions */

    #define BOARD_TI_AM5728__QSPI_PID                         0x00    // Revision register
    #define BOARD_TI_AM5728__QSPI_SYSCONFIG                   0x10    // Configuration of the local target state management mode
    #define BOARD_TI_AM5728__QSPI_INTR_STATUS_RAW_SET         0x20    // This register contains raw interrupt status flags
    #define BOARD_TI_AM5728__QSPI_INTR_STATUS_ENABLED_CLEAR   0x24    // This register contains status flags of the enabled interrupts
    #define BOARD_TI_AM5728__QSPI_INTR_ENABLE_SET_REG         0x28    // This register enables the interrupts
    #define BOARD_TI_AM5728__QSPI_INTR_ENABLE_CLEAR_REG       0x2C    // This register disables the interrupts
    #define BOARD_TI_AM5728__QSPI_INTC_EOI_REG                0x30    // Software End-Of-Interrupt
    #define BOARD_TI_AM5728__QSPI_SPI_CLOCK_CNTRL_REG         0x40    // This register controls the external SPI clock generation
    #define BOARD_TI_AM5728__QSPI_SPI_DC_REG                  0x44    // This register controls the different modes for each output chip select
    #define BOARD_TI_AM5728__QSPI_SPI_CMD_REG                 0x48    // This register sets up the SPI command
    #define BOARD_TI_AM5728__QSPI_SPI_STATUS_REG              0x4C    // This register contains indicators to allow the user to monitor the progression of a frame transfer
    #define BOARD_TI_AM5728__QSPI_SPI_DATA_REG                0x50    // Data register for read and write operations
    #define BOARD_TI_AM5728__QSPI_SPI_SETUP0_REG              0x54    // This register contains the read/write command setup for the memory mapped protocol translator (effecting chip select 0 output)
    #define BOARD_TI_AM5728__QSPI_SPI_SETUP1_REG              0x58    // This register contains the read/write command setup for the memory mapped protocol translator (effecting chip select 1 output)
    #define BOARD_TI_AM5728__QSPI_SPI_SETUP2_REG              0x5C    // This register contains the read/write command setup for the memory mapped protocol translator (effecting chip select 2 output)
    #define BOARD_TI_AM5728__QSPI_SPI_SETUP3_REG              0x60    // This register contains the read/write command setup for the memory mapped protocol translator (effecting chip select 3 output)
    #define BOARD_TI_AM5728__QSPI_SPI_SWITCH_REG              0x64    // This register allows initiators to switch control of the SPI core port between the configuration port and the SFI translator
    #define BOARD_TI_AM5728__QSPI_SPI_DATA_REG_1              0x68    // Data register for read and write operations
    #define BOARD_TI_AM5728__QSPI_SPI_DATA_REG_2              0x6C    // Data register for read and write operations
    #define BOARD_TI_AM5728__QSPI_SPI_DATA_REG_3              0x70    // Data register for read and write operations

    /* USER settings */
    #define BOARD_TI_AM5728_SPI_FLASH__SIZE_USABLE            (SPI_FLASH_A25LQ032__SIZE - SPI_FLASH_A25LQ032__SECTOR_SIZE) // 4 MB - 4kB using one sector for remanent data
    #define BOARD_TI_AM5728_SPI_FLASH__SECTOR_DATA            (SPI_FLASH_A25LQ032__SECTOR_COUNT - 1) // using last sector for remanent data
    #define BOARD_TI_AM5728_SPI_FLASH__OFFSET_DATA            (BOARD_TI_AM5728_SPI_FLASH__SECTOR_DATA * SPI_FLASH_A25LQ032__SECTOR_SIZE) // adress of sector for remanent data

    #define BOARD_TI_AM5728__QSPI_CMD_REG_DEFAULT             0x003E0000

    #define BOARD_TI_AM5728__QSPI_DUMMY_SEND                  0x00        // dummy byte

	//------------------------------------------------------------------------
	//	SDRAM
	//------------------------------------------------------------------------
	#define BOARD_TI_AM5728__SDRAM_OFFSET_SBL_VERSION_TYPE	  (0x0404 + 0x00)		// 32Bit
	#define BOARD_TI_AM5728__SDRAM_OFFSET_SBL_VERSION_MAJOR	  (0x0408 + 0x00)		// 16Bit
	#define BOARD_TI_AM5728__SDRAM_OFFSET_SBL_VERSION_MINOR	  (0x0408 + 0x02)		// 16Bit
		
#endif

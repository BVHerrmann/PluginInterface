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
/*  F i l e               &F: Board_IX1000.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for IX1000
//
//****************************************************************************/

#ifndef __Board_IX1000_h__
#define __Board_IX1000_h__

//########################################################################
//  Defines
//########################################################################
//========================================================================
//	Intel
//========================================================================

//========================================================================
//	Intel IX1000 (formerly Quark)
//========================================================================

//------------------------------------------------------------------------
//	PCI Configuration Registers
//------------------------------------------------------------------------

// Version Register (Register 8) (GMAC_REG_8)
#define BOARD_IX1000__VERSION					0x20

// MAC Address0 High/Low Register (Register 16/17) (GMAC_REG_16/17)
#define BOARD_IX1000__MAC0_HIGH					0x40
#define BOARD_IX1000__MAC0_LOW					0x44

// Bus Mode Register (Register 0) (DMA_REG_0)
#define BOARD_IX1000__BUS_MODE					0x1000
#define BOARD_IX1000__BUS_MODE_RESERVED_MASK	0xBFFFFFFF
#define BOARD_IX1000__BUS_MODE_SWR				0

// Status Register (Register 5) (DMA_REG_5)
#define BOARD_IX1000__STATUS					0x1014
#define BOARD_IX1000__STATUS_INT_MASK			0x2801E7FF			
// Interrupt Enable Register (Register 7) (DMA_REG_7)
#define BOARD_IX1000__INTEN						0x101C

// Interrupts enabled with INTEN and read in STATUS Register on IX1000
#define BOARD_IX1000__ICU_VECTOR_TI				(1<<0)		// Transmit Interrupt (TI)
#define BOARD_IX1000__ICU_VECTOR_TPS			(1<<1)		// Transmit Process Stopped (TPS) 1
#define BOARD_IX1000__ICU_VECTOR_TU				(1<<2)		// Transmit Buffer Unavailable (TU) 2
#define BOARD_IX1000__ICU_VECTOR_TJT			(1<<3)		// Transmit Jabber Timeout (TJT) 3
#define BOARD_IX1000__ICU_VECTOR_OVF			(1<<4)		// Receive Overflow (OVF) 4
#define BOARD_IX1000__ICU_VECTOR_UNF			(1<<5)		// Transmit Underflow (UNF) 5
#define BOARD_IX1000__ICU_VECTOR_RI				(1<<6)		// Receive Interrupt (RI)
#define BOARD_IX1000__ICU_VECTOR_RU				(1<<7)		// Receive Buffer Unavailable (RU) 7
#define BOARD_IX1000__ICU_VECTOR_RPS			(1<<8)		// Receive Process Stopped (RPS) 8
#define BOARD_IX1000__ICU_VECTOR_RWT			(1<<9)		// Receive Watchdog Timeout (RWT) 9
#define BOARD_IX1000__ICU_VECTOR_ETI			(1<<10)		// Early Transmit Interrupt (ETI)
// reserved 12:11
#define BOARD_IX1000__ICU_VECTOR_FBI			(1<<13)		// Fatal Bus Error Interrupt (FBI)
#define BOARD_IX1000__ICU_VECTOR_ERI			(1<<14)		// Early Receive Interrupt (ERI)
#define BOARD_IX1000__ICU_VECTOR_AIS			(1<<15)		// Abnormal Interrupt Summary (AIS)
#define BOARD_IX1000__ICU_VECTOR_NIS			(1<<16)		// Normal Interrupt Summary (NIS)
// reserved 31:17 in INTEN but not all of them are reserved in STATUS Register!
// The following bits are not reserved in STATUS Register:
// Received Process State (RS) bits 19:17
// Transmit Process State (TS) bits 22:20
// Error Bits (EB) bits 25:23
// MMC Receive/Transmit Interrupts can be enabled in MMC Receive/Transmit Interrupt Mask Register (MMC_INTR_MASK_RX/MMC_INTR_MASK_TX)
#define BOARD_IX1000__ICU_VECTOR_GMI			(1<<27)		// MAC MMC Interrupt (GMI)
// Timestamp Trigger Interrupt can be enabled in Interrupt Mask Register (Register 15) (GMAC_REG_15)
#define BOARD_IX1000__ICU_VECTOR_TTI			(1<<29)		// Timestamp Trigger Interrupt (TTI)

// Interrupt Register (Register 14) (GMAC_REG_14)
#define BOARD_IX1000__INT_REG					0x38
// Interrupt Mask Register (Register 15) (GMAC_REG_15)
#define BOARD_IX1000__INT_MASK					0x3C
// MMC Receive Interrupt Mask Register (MMC_INTR_MASK_RX)
#define BOARD_IX1000__INT_MASK_MMC_RX			0x010C
// MMC Transmit Interrupt Mask Register (MMC_INTR_MASK_TX)
#define BOARD_IX1000__INT_MASK_MMC_TX			0x0110
// MMC IPC Receive Checksum Offload Interrupt Mask Register (MMC_IPC_INTR_MASK_RX)
#define BOARD_IX1000__INT_MASK_MMC_IPC_RX		0x200

//------------------------------------------------------------------------
//	BAR0-5
//------------------------------------------------------------------------

#define	BOARD_IX1000__BAR0_SIZE	(1024*8)
#define	BOARD_IX1000__BAR1_SIZE	(1024*0)
#define	BOARD_IX1000__BAR2_SIZE	(1024*0)
#define	BOARD_IX1000__BAR3_SIZE	(1024*0)
#define	BOARD_IX1000__BAR4_SIZE	(1024*0)
#define	BOARD_IX1000__BAR5_SIZE	(1024*0)

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

//------------------------------------------------------------------------
//	PCI-Bridge Register
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//	FLASH
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//	SDRAM
//------------------------------------------------------------------------

//########################################################################
//  Structures
//########################################################################

#ifdef PNDEV_DRIVER_SYS // just used by kernel
//########################################################################
//  Function prototypes
//########################################################################

BOOLEAN			fnBoard_IX1000_Reset(               DEVICE_EXTENSION*			pDevExtIn,
                                                    const ePNDEV_RESET_ACTION	eActionIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_IX1000_ResetHw(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_IX1000_InitHw1(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bStopDefaultFwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_IX1000_InitHw2(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bResetHwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_IX1000_VerifyHwAfterReset(  DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN 		fnBoard_IX1000_GetMacAdress(        DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);
#endif

//------------------------------------------------------------------------

#endif

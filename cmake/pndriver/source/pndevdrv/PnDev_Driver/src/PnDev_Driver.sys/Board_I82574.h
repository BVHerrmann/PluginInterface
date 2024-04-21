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
/*  F i l e               &F: Board_I82574.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for I82574
//
//****************************************************************************/

#ifndef __Board_I82574_h__
#define __Board_I82574_h__

//########################################################################
//  Defines
//########################################################################
//========================================================================
//	Intel
//========================================================================

// RAL Receive Address Low
#define BOARD_I82574__RAL		(0x05400+(8*0))

// RAL Receive Address High
#define BOARD_I82574__RAH		(0x05404+(8*0))

//========================================================================
//	Intel I82574 (formerly Hartwell)
//========================================================================

// CTRL Register
#define BOARD_I82574__CTRL						0x0
#define BOARD_I82574__CTRL_RESERVED_ZEROS_MASK	0xDC101B6D
#define BOARD_I82574__CTRL_RESERVED_ONES_MASK	0x00000008
#define BOARD_I82574__CTRL_FD					0
// reserved 1 (set to 0b)
#define BOARD_I82574__CTRL_GIO_MASTER_DISABLE	2
// reserved 3 (set to 1b)
// reserved 4 (set to 0b)
#define BOARD_I82574__CTRL_ASDE					5
#define BOARD_I82574__CTRL_SLU					6
// reserved 7 (set to 0b)
#define BOARD_I82574__CTRL_SPEED_s				8
#define BOARD_I82574__CTRL_SPEED_e				9
// reserved 10 (set to 0b)
#define BOARD_I82574__CTRL_FRCSPD				11
#define BOARD_I82574__CTRL_FRCDPLX				12
// reserved 19:13 (set to 0x0)
#define BOARD_I82574__CTRL_ADVD3WUC				20
// reserved 25:21 (set to 0x0)
#define BOARD_I82574__CTRL_RST					26
#define BOARD_I82574__CTRL_RFCE					27
#define BOARD_I82574__CTRL_TFCE					28
// reserved 29 (set to 0b)
#define BOARD_I82574__CTRL_VME					30
#define BOARD_I82574__CTRL_PHY_RST				31

// Status Register
#define BOARD_I82574__STATUS						0x8
#define BOARD_I82574__STATUS_FD					0
#define BOARD_I82574__STATUS_LU					1
// reserved 3:2
#define BOARD_I82574__STATUS_TXOFF				4
// reserved 5
#define BOARD_I82574__STATUS_SPEED_s				6
#define BOARD_I82574__STATUS_SPEED_e				7
#define BOARD_I82574__STATUS_ASDV_s				8
#define BOARD_I82574__STATUS_ASDV_e				9
#define BOARD_I82574__STATUS_PHYRA				10
// reserved 18:11
#define BOARD_I82574__STATUS_GIO_MASTER_ENABLE 	19
// reserved 31:20

//EEPROM/FLASH Control Register EEC
#define BOARD_I82574__EEC						0x00010
#define BOARD_I82574__EEC_AUTO_RD				9

#define BOARD_I82574__EERD						0x00014
#define BOARD_I82574__EERD_DONE					1

// Interrupts on I82574 board
#define BOARD_I82574__ICU_RESERVED_MASK			0x01F782D7

#define BOARD_I82574__ICU_VECTOR__TXDW			(1<<0)
#define BOARD_I82574__ICU_VECTOR__TXQE			(1<<1)
#define BOARD_I82574__ICU_VECTOR__LSC        	(1<<2)
// reserved 3
#define BOARD_I82574__ICU_VECTOR__RXDMT0     	(1<<4)
// reserved 5
#define BOARD_I82574__ICU_VECTOR__RXO		    (1<<6)
#define BOARD_I82574__ICU_VECTOR__RXT0			(1<<7)
// reserved 8
#define BOARD_I82574__ICU_VECTOR__MDAC			(1<<9)
// reserved 14:10
#define BOARD_I82574__ICU_VECTOR__TXD_LOW		(1<<15)
#define BOARD_I82574__ICU_VECTOR__SRPD			(1<<16)
#define BOARD_I82574__ICU_VECTOR__ACK			(1<<17)
#define BOARD_I82574__ICU_VECTOR__MNG       		(1<<18)
// reserved 19 
#define BOARD_I82574__ICU_VECTOR__RXQ0      		(1<<20)
#define BOARD_I82574__ICU_VECTOR__RXQ1      		(1<<21)
#define BOARD_I82574__ICU_VECTOR__TXQ0      		(1<<22)
#define BOARD_I82574__ICU_VECTOR__TXQ1      		(1<<23)
#define BOARD_I82574__ICU_VECTOR__OTHER 			(1<<24)
// reserved 30:25
// INT_ASSERTED is a non-maskable interrupt 
// it can be enabled in PCI configuration space
#define BOARD_I82574__ICU_VECTOR__INT_ASSERTED  	(1<<31)


#define BOARD_I82574__CTRL_EXT						0x00018
#define BOARD_I82574__CTRL_EXT_RESERVED_MASK		0xB9DAB000
#define BOARD_I82574__CTRL_EXT_DRV_LOAD				(1<<28)


#define BOARD_I82574__EIAC						0x00DC
#define BOARD_I82574__IVAR						0x00E4


//------------------------------------------------------------------------
//	BAR0-5
//------------------------------------------------------------------------

#define	BOARD_I82574__BAR0_SIZE	(1024*128)
#define	BOARD_I82574__BAR1_SIZE	(1024*512)
#define	BOARD_I82574__BAR2_SIZE	(1024*0)
#define	BOARD_I82574__BAR3_SIZE	(1024*16)
#define	BOARD_I82574__BAR4_SIZE	(1024*0)
#define	BOARD_I82574__BAR5_SIZE	(1024*0)

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

BOOLEAN			fnBoard_I82574_Reset(               DEVICE_EXTENSION*			pDevExtIn,
                                                    const ePNDEV_RESET_ACTION	eActionIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I82574_ResetHw(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I82574_InitHw1(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bStopDefaultFwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I82574_InitHw2(             DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bResetHwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I82574_VerifyHwAfterReset(  DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN 		fnBoard_I82574_GetMacAdress(        DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);
#endif

//------------------------------------------------------------------------

#endif

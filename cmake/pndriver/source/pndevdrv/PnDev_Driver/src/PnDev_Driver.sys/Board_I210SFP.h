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
/*  F i l e               &F: Board_I210SFP.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for I210SFP
//
//****************************************************************************/

#ifndef __Board_I210SFP_h__
#define __Board_I210SFP_h__

//########################################################################
//  Defines
//########################################################################
//========================================================================
//	Intel
//========================================================================

// RAL Receive Address Low
#define BOARD_I210SFP__RAL		(0x05400+(8*0))

// RAL Receive Address High
#define BOARD_I210SFP__RAH		(0x05404+(8*0))

//========================================================================
//	Intel I210SFP
//========================================================================

// CTRL Register
#define BOARD_I210SFP__CTRL					    0x0
#define BOARD_I210SFP__CTRL_RESERVED_MASK		0xFCFF1BC5
#define BOARD_I210SFP__CTRL_FD					0
// reserved 1  (set to 0b)
#define BOARD_I210SFP__CTRL_GIO_MASTER_DISABLE	2		
#define BOARD_I210SFP__CTRL_LRST				3
// reserved 4  (set to 0b)
#define BOARD_I210SFP__CTRL_ASDE				5
#define BOARD_I210SFP__CTRL_SLU				    6
#define BOARD_I210SFP__CTRL_ILOS				7
#define BOARD_I210SFP__CTRL_SPEED_s			    8
#define BOARD_I210SFP__CTRL_SPEED_e			    9
// reserved 10  (set to 0b)
#define BOARD_I210SFP__CTRL_FRCSPD				11
#define BOARD_I210SFP__CTRL_FRCDPLX			    12
// reserved 17:13  (set to 0x0)
#define BOARD_I210SFP__CTRL_SDP0_DATA			18
#define BOARD_I210SFP__CTRL_SDP1_DATA			19
#define BOARD_I210SFP__CTRL_ADVD3WUC			20
#define BOARD_I210SFP__CTRL_EN_PHY_PWR_MGMT	    21
#define BOARD_I210SFP__CTRL_SDP0_IODIR			22
#define BOARD_I210SFP__CTRL_SDP1_IODIR			23
// reserved 25:24  (set to 0x0)
#define BOARD_I210SFP__CTRL_RST				    26
#define BOARD_I210SFP__CTRL_RFCE				27
#define BOARD_I210SFP__CTRL_TFCE				28
#define BOARD_I210SFP__CTRL_DEV_RST			    29		
#define BOARD_I210SFP__CTRL_VME				    30
#define BOARD_I210SFP__CTRL_PHY_RST			    31

// Status Register
#define BOARD_I210SFP__STATUS					0x8
#define BOARD_I210SFP__STATUS_FD				0
#define BOARD_I210SFP__STATUS_LU				1
#define BOARD_I210SFP__STATUS_FunctionID_s		2
#define BOARD_I210SFP__STATUS_FunctionID_e		3
#define BOARD_I210SFP__STATUS_TXOFF			    4
#define BOARD_I210SFP__STATUS_TBIMODE			5
#define BOARD_I210SFP__STATUS_SPEED_s			6
#define BOARD_I210SFP__STATUS_SPEED_e			7
#define BOARD_I210SFP__STATUS_ASDV_s			8
#define BOARD_I210SFP__STATUS_ASDV_e			9
// reserved 10
#define BOARD_I210SFP__STATUS_PCI66			    11
#define BOARD_I210SFP__STATUS_BUS64			    12
#define BOARD_I210SFP__STATUS_PCIX_MODE		    13
#define BOARD_I210SFP__STATUS_PCIXSPD_s		    14
#define BOARD_I210SFP__STATUS_PCIXSPD_e		    15
#define BOARD_I210SFP__STATUS_PF_RST_DONE		21		
// reserved 31:16

#define BOARD_I210SFP__EECD					    0x00010
#define BOARD_I210SFP__EDCD_AUTO_RD			    9

// Interrupts on I210SFP board
#define BOARD_I210SFP__ICU_RESERVED_MASK			0x67CC7CD5

#define BOARD_I210SFP__ICU_VECTOR__TXDW			    (1<<0)
// reserved 1	
#define BOARD_I210SFP__ICU_VECTOR__LSC        	 	(1<<2)
// reserved 3
#define BOARD_I210SFP__ICU_VECTOR__RXDMT0     	 	(1<<4)
// reserved 5
#define BOARD_I210SFP__ICU_VECTOR__RX_MISS     	    (1<<6)
#define BOARD_I210SFP__ICU_VECTOR__RXDW			    (1<<7)
// reserved 9:8
#define BOARD_I210SFP__ICU_VECTOR__GPHY			    (1<<10)
#define BOARD_I210SFP__ICU_VECTOR__GPI_SDP0  		(1<<11)
#define BOARD_I210SFP__ICU_VECTOR__GPI_SDP1  		(1<<12)
#define BOARD_I210SFP__ICU_VECTOR__GPI_SDP2  		(1<<13)
#define BOARD_I210SFP__ICU_VECTOR__GPI_SDP3  		(1<<14)
// reserved 15 - kein Wert
// reserved 16:17
#define BOARD_I210SFP__ICU_VECTOR__MNG       		(1<<18)
#define BOARD_I210SFP__ICU_VECTOR__TIME_SYNC 		(1<<19)
// reserved 21:20 
#define BOARD_I210SFP__ICU_VECTOR__FER       		(1<<22)
// reserved 23
#define BOARD_I210SFP__ICU_VECTOR__PCI_EXEPTION 	(1<<24)
#define BOARD_I210SFP__ICU_VECTOR__SCE           	(1<<25)
#define BOARD_I210SFP__ICU_VECTOR__SOFTWARE_WD   	(1<<26)
// reserved 27
// reserved 28 - kein Wert
#define BOARD_I210SFP__ICU_VECTOR__TCP_TIMER     	(1<<29)
#define BOARD_I210SFP__ICU_VECTOR__DRSTA         	(1<<30)
// reserved 31

#define BOARD_I210SFP__CTRL_EXT					    0x00018
#define BOARD_I210SFP__CTRL_EXT_RESERVED_MASK		0x16DFBCCE
#define BOARD_I210SFP__CTRL_EXT_DRV_LOAD			(1<<28)

#define BOARD_I210SFP__MREVID					    0x5B64
#define BOARD_I210SFP__EERD						    0x12014
#define BOARD_I210SFP__EERD_DONE					1u

#define BOARD_I210SFP__GPIE						    0x1514
#define BOARD_I210SFP__EICS						    0x1520
#define BOARD_I210SFP__EIMS						    0x1524
#define BOARD_I210SFP__EIAC						    0x152C
#define BOARD_I210SFP__EIAM						    0x1530
#define BOARD_I210SFP__IVAR_MISC				    0x1740

//------------------------------------------------------------------------
//	BAR0-5
//------------------------------------------------------------------------

#define	BOARD_I210SFP__BAR0_SIZE	(1024*512)
#define	BOARD_I210SFP__BAR1_SIZE	(1024*0)
#define	BOARD_I210SFP__BAR2_SIZE	(1024*0)
#define	BOARD_I210SFP__BAR3_SIZE	(1024*16)
#define	BOARD_I210SFP__BAR4_SIZE	(1024*0)
#define	BOARD_I210SFP__BAR5_SIZE	(1024*0)

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

BOOLEAN			fnBoard_I210SFP_Reset(              DEVICE_EXTENSION*			pDevExtIn,
                                                    const ePNDEV_RESET_ACTION	eActionIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I210SFP_ResetHw(            DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I210SFP_InitHw1(            DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bStopDefaultFwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I210SFP_InitHw2(            DEVICE_EXTENSION*			pDevExtIn,
                                                    const BOOLEAN				bResetHwIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN			fnBoard_I210SFP_VerifyHwAfterReset( DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);

BOOLEAN 		fnBoard_I210SFP_GetMacAdress(       DEVICE_EXTENSION*			pDevExtIn,
                                                    const UINT32				lDimErrorIn,
                                                    _TCHAR*						sErrorOut);
#endif

//------------------------------------------------------------------------

#endif

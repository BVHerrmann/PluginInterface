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
/*  F i l e               &F: Board_Pnip.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for boards with PNIP (Ertec200P)
//
//****************************************************************************/

#ifndef __Board_Pnip_h__
#define __Board_Pnip_h__

	//########################################################################
	//  Defines
	//########################################################################

	typedef enum _eBOARD_ERTEC200P_XHIF__DYN_PAGE
	{
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_INVALID			= 0,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_ARM926_TCM		= 1,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_SD_MMC			= 2,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_REG			= 3,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM		= 4,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0	= 5,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1	= 6,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK2	= 7,
		eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK3	= 8
	}
	eBOARD_ERTEC200P_XHIF__DYN_PAGE;

	//------------------------------------------------------------------------
	// XHIF=on, 32Bit Mode, GPIO95:32 off, XHIF_XRDY is low-active, separate Rd/Wr
	#define	BOARD_ERTEC200P__CONFIG_PINS_XHIF	0x38								// Bit6:3=0111, Bit2:0=user-specific, Fspec.S.251

	// XHIF=off, GPIO95:32 on (all inputs)
	#define	BOARD_ERTEC200P__CONFIG_PINS_AHB	0x48								// Bit6:3=1001, Bit2:0=don't care

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

#endif

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
/*  F i l e               &F: cp1625_flash_bootloader.h                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for the console application (cp1625_flash_bootloader)
//
//****************************************************************************/

#ifndef __cp1625_flash_bootloader_h__
#define __cp1625_flash_bootloader_h__

	#ifdef __cplusplus		// if C++ - compiler: Use C linkage
	extern "C"
	{
	#endif

	extern UINT32 g_SleepTime;

	//########################################################################
	//  Defines
	//########################################################################

	#define	DIM_ARRAY_DEVICE	16

	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uCTR_CBF_ISR
	{
		UINT32	lCtrCbfIsr;
		UINT32	lCtrTest;
	}
	uCTR_CBF_ISR;

	//------------------------------------------------------------------------
	typedef struct _uCTR_CBF_EVENT
	{
		BOOLEAN	bFatal;
	}
	uCTR_CBF_EVENT;

	//########################################################################
	//  Function prototypes
	//########################################################################

	void	fnDrvTest_Main(								void);
	void	fnDrvMenu_Handler(							void);
	BOOLEAN fnDrvMenu_SelectDevice(						void);
	BOOLEAN fnDrvMenu_ProgramFlashCP1625(				void);

	BOOLEAN fnDrvTest_CloseDevice(						void);
	void	fnDrvTest_ShowDriverVersion(				LSA_VERSION_TYPE* 			pVersionIn);
	BOOLEAN	fnDrvTest_DeviceParOk(						const UINT32				lDimErrorIn,
														_TCHAR*						sErrorOut);
	BOOLEAN fnDrvTest_OpenDeviceCommon(					const BOOLEAN				bMasterAppIn,
														const BOOLEAN				bFwDownloadIn,
														const BOOLEAN				bShmDebug,
														const BOOLEAN				bReqHostMemIn,
														const PNDEV_CBF_EVENT		pCbfEventIn,
														const PNDEV_CBF_ISR			pCbfIsrIn,
														const _TCHAR*				sPathDrvBinIn);
	BOOLEAN fnDrvTest_ProgramFlashCP1625(				const ePNDEV_FLASH_ACTION	eFlashActionIn,
														const _TCHAR*				sPathDrvBinIn);
	void	fnDrvTest_ShowError(						const _TCHAR*				sErrorIn);
	void	fnDrvUtil_ShowError(						const HANDLE				hStdoutIn,
														const ePNDEV_STDOUT			eStdoutIn,
														const ePNDEV_RESULT			eResultIn,
														const PNDEV_CHAR*			sErrorIn);
	void	fnDrvUtil_PrintfColor(						const HANDLE				hStdoutIn,
														const ePNDEV_STDOUT			eStdoutIn,
														const UINT32				lColorFontIn,
														const UINT32				lColorBackIn,
														const PNDEV_CHAR*			sFormatIn, ...);
	void			fnDrvTest_InitVarCbfIsr(			void);
	void			fnDrvTest_InitVarCbfEvent(			void);

	void __stdcall	fnDrvTest_CbfEvent(					const HANDLE				hDeviceIn,
														const ePNDEV_EVENT_CLASS	eEventClassIn,
														_TCHAR*						sEventIn,
														const void*					pCbfParUserIn);
	void __stdcall	fnDrvTest_CbfIsr(					const HANDLE				hDeviceIn,
														const BOOLEAN				bDeviceTestIn,
														const ePNDEV_INT_SRC		eIntSrcIn,
														const void*					pCbfParUserIn);

	//------------------------------------------------------------------------
	#ifdef __cplusplus		// if C++ - compiler: End of C linkage
	}
	#endif

#endif

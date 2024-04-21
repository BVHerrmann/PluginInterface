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
/*  F i l e               &F: cp1625_flash_bootloader.c                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for the console application (cp1625_flash_bootloader)
//
//****************************************************************************/

// Note:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//  Structures
//########################################################################

typedef struct _uMENU
{
	_TCHAR*		sTitle;
	const void*	pTableMenu;
}
uMENU;

//------------------------------------------------------------------------
typedef struct _uENTRY_TABLE_MENU
{
	_TCHAR*	sDescription;
	UINT16	lShortKey;
	BOOLEAN	(*pFnAction) (void);
	uMENU*	pNextMenu;													// NULL: exit MenuHandler
}
uENTRY_TABLE_MENU;

//########################################################################
//  Variables
//########################################################################

	UINT32						g_lIndexFixedDevice								= MAXUINT32;
	UINT32						g_lCtrDevice									= 0;
	UINT32						g_lCurDeviceIndex								= MAXUINT32;				// current opened device:	index at ArrayUsedDevice
	ePNDEV_BOARD				g_eCurBoard										= ePNDEV_BOARD_INVALID;		// current selected device:	BoardType
	_TCHAR						g_sCurDevicePciLoc[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};						// current selected device:	PCI location
	uPNDEV_DRIVER_DESC*			g_hPnDev										= NULL;
	HANDLE						g_hStdin										= NULL;
	HANDLE						g_hStdout										= NULL;
	ePNDEV_STDOUT				g_eStdout										= ePNDEV_STDOUT_INVALID;
	uPNDEV_GET_DEVICE_INFO_OUT	g_uArrayDeviceInfo[DIM_ARRAY_DEVICE];
	uPNDEV_OPEN_DEVICE_OUT		g_uArrayUsedDevice[DIM_ARRAY_DEVICE];
	UINT32 						g_SleepTime 									= 1000;
	_TCHAR						g_sPathDrvBin[PNDEV_SIZE_STRING_BUF] 			= {0};

	uCTR_CBF_ISR				g_uCbfIsr;
	uCTR_CBF_EVENT				g_uCbfEvent;

	// necessary for referencing uMenu... at uTableMenu..
	extern uMENU uMenuMain;

//========================================================================
	uENTRY_TABLE_MENU uTableMenuMain[]=
	{
		// description										key		FctPtr							menu after calling FctPtr
		{_TEXT("  (0) Select device"),								'0',	&fnDrvMenu_SelectDevice,		&uMenuMain,},
		{_TEXT("  (1) CP1625 - Flash NAND Bootloader"),				'1',	&fnDrvMenu_ProgramFlashCP1625,	&uMenuMain},
		{_TEXT("  (c) Cancel"),										'c',	NULL,							NULL},
		{NULL, 0, NULL, NULL}	// end of table
	};

//------------------------------------------------------------------------
	uMENU uMenuMain=
	{
		_TEXT("MAIN menu:"), uTableMenuMain
	};

//************************************************************************
//  D e s c r i p t i o n :
//
//  do services of PnDev_Driver
//************************************************************************

void fnDrvTest_Main(void)
{
ePNDEV_STDIN	eStdin = ePNDEV_STDIN_INVALID;
_TCHAR			sError[PNDEV_SIZE_STRING_BUF] = {0};

	// initialize global variables
	{
		memset(	g_uArrayDeviceInfo,
				0,
				(DIM_ARRAY_DEVICE * sizeof(uPNDEV_GET_DEVICE_INFO_OUT)));

		memset(	g_uArrayUsedDevice,
				0,
				(DIM_ARRAY_DEVICE * sizeof(uPNDEV_OPEN_DEVICE_OUT)));

	}

    eStdin    = ePNDEV_STDIN_KEYBOARD;
    g_eStdout = ePNDEV_STDOUT_SCREEN;

    _tcscpy_s(  g_sPathDrvBin,
        _countof(g_sPathDrvBin),
        _TEXT("../../bin"));

  	if	(!fnDrvUtil_RegisterStdStreams(	eStdin,
										g_eStdout,
										_countof(sError),
										&g_hStdin,
										&g_hStdout,
										sError))
    	// error at registering StdStreams (stdin, stdout, stderr)
  	{
    	// error cannot be shown because of missing stdout
    }
	else
	{
		ePNDEV_RESULT eResult = ePNDEV_FATAL;

		// initialize variables of CBF-ISR / CBF-EVENT
		fnDrvTest_InitVarCbfIsr();
		fnDrvTest_InitVarCbfEvent();

		// start up PnDev_Driver
		{
		_TCHAR sPathDll[PNDEV_SIZE_STRING_BUF] 		= {0};
		_TCHAR sSysFileName[PNDEV_SIZE_STRING_BUF]	= {0};

			#if (PNDEV_PLATFORM == PNDEV_UM_PLATFORM_64)
			  // selected platform = x64
			  _tcscpy_s(  sPathDll, _countof(sPathDll), g_sPathDrvBin);
			  _tcscat_s(  sPathDll, _countof(sPathDll), _TEXT("/Linux/PnDev_DriverU64.so"));
			#else
			  // selected platform = Win32
			  _tcscpy_s(  sPathDll, _countof(sPathDll), g_sPathDrvBin);
			  _tcscat_s(  sPathDll, _countof(sPathDll), _TEXT("/Linux/PnDev_DriverU32.so"));
			#endif

			_tcscpy_s(	sSysFileName,
						_countof(sSysFileName),
						_TEXT("PnDev_Driver.sys"));

			if	(fnDrvUtil_CheckInstalledDriver(sSysFileName,
												_countof(sError),
												sError))
				// success
			{
				eResult = fnPnDev_StartUpDriver(sPathDll,
												_countof(sError),
												&g_hPnDev,
												sError);
			}
		}

		if	(eResult != ePNDEV_OK)
			// error
		{
			fnDrvUtil_ShowError(g_hStdout,
								g_eStdout,
								eResult,
								sError);
		}
		else
		{
			// show driver version
			fnDrvTest_ShowDriverVersion(&g_hPnDev->uVersion);

			// start with selecting device as usual in manual mode
			fnDrvMenu_SelectDevice();
			fnDrvMenu_Handler();

			// cleanup
			{
			BOOLEAN	bResultCleanup = FALSE;
			UINT32	i = 0;

				for	(i = 0; i < 10; i++)
					// 10sec
				{
					if	(!fnDrvUtil_CleanUp(g_hPnDev,
											DIM_ARRAY_DEVICE,
											g_uArrayUsedDevice,
											FALSE,						// standard CloseDevice
											_countof(sError),
											sError))
						// error at cleanup
					{
						fnDrvTest_ShowError(sError);
					}
					else
					{
						// success
						bResultCleanup = TRUE;

						// leave loop
						break;
					}

					// wait 1sec
					Sleep(g_SleepTime);
				}

				if	(!bResultCleanup)
					// error
				{
					if	(!fnDrvUtil_CleanUp(g_hPnDev,
											DIM_ARRAY_DEVICE,
											g_uArrayUsedDevice,
											TRUE,						// do CloseDevice anyway
											_countof(sError),
											sError))
						// error at cleanup
					{
						fnDrvTest_ShowError(sError);
					}
					else
					{
						// success
						bResultCleanup = TRUE;
					}
				}

				if	(bResultCleanup)
					// success
				{

					_tprintf(_TEXT("Cleanup done.\n\n"));

					// shut down PnDev_Driver
					eResult = fnPnDev_ShutDownDriver(	g_hPnDev,
														_countof(sError),
														sError);

					if	(eResult != ePNDEV_OK)
						// error
					{
						fnDrvUtil_ShowError(g_hStdout,
											g_eStdout,
											eResult,
											sError);
					}
					else
					{
						// invalidate
						{
							_tcscpy_s(	g_sCurDevicePciLoc,
										_countof(g_sCurDevicePciLoc),
										_TEXT(""));

							g_lCurDeviceIndex	= MAXUINT32;
							g_eCurBoard			= ePNDEV_BOARD_INVALID;
							g_hPnDev			= NULL;
						}
					}
				}
			}
		}


		if	(!fnDrvUtil_UnregisterStdStreams(	g_hStdin,
												g_hStdout,
												_countof(sError),
												sError))
			// error at unregistering StdStreams (stdin, stdout, stderr)
		{
			fnDrvTest_ShowError(sError);
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  menu handler
//************************************************************************

void fnDrvMenu_Handler(void)
{
uMENU* pMenu = NULL;

	// start with main menu
	pMenu = &uMenuMain;

	while	(pMenu != NULL)
			// valid menu
	{
	BOOLEAN				bEntryFound		= FALSE;
	uENTRY_TABLE_MENU*	pEntryMenuTmp	= NULL;

		_tprintf(	_TEXT("\n%s\n\n"),
					pMenu->sTitle);

		_tprintf(_TEXT("====================================================\n"));
		_tprintf(_TEXT(" Selected Device: %s\n"), g_sCurDevicePciLoc);
		_tprintf(_TEXT("====================================================\n"));

		// show all entries of this menu
		{
			pEntryMenuTmp = (uENTRY_TABLE_MENU*) pMenu->pTableMenu;

			while	(pEntryMenuTmp->sDescription != NULL)
					// valid entry
			{
				// show description
				_tprintf(	_TEXT("%s\n"),
							pEntryMenuTmp->sDescription);

				// next entry
				pEntryMenuTmp++;
			}
		}

		_tprintf(_TEXT("====================================================\n"));

		while	(!bEntryFound)
				// entry not found
		{
		char cScan = 0;

			if	(scanf_s("%c", &cScan, 1) == 0)
				// no fields assigned
			{
				fnDrvUtil_PrintfColor(	g_hStdout,
										g_eStdout,
										_RED,
										_BLACK,
										_TEXT("#### Invalid input, please try again (%c)\n"),
										cScan);
			}
			else if (	(cScan == '\r')
					|| 	(cScan == '\n'))
			{
				// do nothing ignore linefeed
			}
			else
			{
				pEntryMenuTmp = (uENTRY_TABLE_MENU*) pMenu->pTableMenu;

				while	(pEntryMenuTmp->sDescription != NULL)
						// valid entry
				{
					if	(pEntryMenuTmp->lShortKey == cScan)
						// entry found
					{
						// success
						bEntryFound = TRUE;

						if	(pEntryMenuTmp->pFnAction != NULL)
							// action exist
						{
							// do action
							pEntryMenuTmp->pFnAction();
						}

						// leave loop
						break;
					}
					else
					{
						// next entry
						pEntryMenuTmp++;
					}
				}

				if	(pEntryMenuTmp->sDescription == NULL)
					// all entries scanned
				{
					fnDrvUtil_PrintfColor(	g_hStdout,
											g_eStdout,
											_RED,
											_BLACK,
											_TEXT("#### Invalid selection, please try again (%c)\n"),
											cScan);
				}
			}
		}

		// show next menu or exit
		pMenu = pEntryMenuTmp->pNextMenu;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  select device
//************************************************************************

BOOLEAN fnDrvMenu_SelectDevice(void)
{
BOOLEAN						bResult		= FALSE;
ePNDEV_RESULT				eResult		= ePNDEV_FATAL;
UINT32						lCtrDevice	= 0;
uPNDEV_GET_DEVICE_INFO_IN	uGetDeviceInfoIn;
_TCHAR						sError[PNDEV_SIZE_STRING_BUF] = {0};

	// set InputParameter
	memset(	&uGetDeviceInfoIn,
			0,
			sizeof(uPNDEV_GET_DEVICE_INFO_IN));

	// get DeviceInfo of all devices
	eResult = (*g_hPnDev->uIf.pFnGetDeviceInfo)(g_hPnDev,
												sizeof(uPNDEV_GET_DEVICE_INFO_IN),
												(sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * DIM_ARRAY_DEVICE),
												DIM_ARRAY_DEVICE,
												_countof(sError),
												&uGetDeviceInfoIn,
												g_uArrayDeviceInfo,
												&lCtrDevice,
												sError);

	if	(eResult != ePNDEV_OK)
		// error
	{
		fnDrvUtil_ShowError(g_hStdout,
							g_eStdout,
							eResult,
							sError);
	}
	else
	{
	_TCHAR	sBoardType  [PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
	_TCHAR	sDeviceState[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
	UINT32	i = 0;

		_tprintf(_TEXT("Service GetDeviceInfo done.\n\n"));

		if	(g_lCtrDevice == 0)
			// first call
		{
			// store number of devices
			g_lCtrDevice = lCtrDevice;
		}
		else if	(lCtrDevice != g_lCtrDevice)
				// number of devices changed
		{
			_tprintf(_TEXT("Number of devices changed!"));

			// Note:
			//	- this should not be a problem because devices are managed by their PciLoc
			//	- at UsedDeviceList for each opened device an own entry is used. Its index hasn't any relationship to an index of DeviceInfoList
		}

		for	(i = 0; i < g_lCtrDevice; i++)
			// all devices
		{
			switch	(g_uArrayDeviceInfo[i].eBoard)
					// BoardType
			{
				case ePNDEV_BOARD_CP1616:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("CP1616 board"));			break;}
				case ePNDEV_BOARD_DB_EB400_PCIE:	{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("DB-EB400-PCIe board"));	break;}
				case ePNDEV_BOARD_EB200:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("EB200 board"));			break;}
				case ePNDEV_BOARD_DB_EB200_PCIE:	{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("DB-EB200-PCIe board"));	break;}
				case ePNDEV_BOARD_DB_SOC1_PCI:		{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("DB-Soc1-PCI board"));		break;}
				case ePNDEV_BOARD_DB_SOC1_PCIE:		{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("DB-Soc1-PCIe board"));		break;}
				case ePNDEV_BOARD_FPGA1_ERTEC200P:	{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("FPGA1-Ertec200P board"));	break;}
				case ePNDEV_BOARD_EB200P:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("EB200P board"));			break;}
				case ePNDEV_BOARD_I210:				{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("I210 board"));				break;}
				case ePNDEV_BOARD_I210SFP:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("I210SFP board"));			break;}
				case ePNDEV_BOARD_I82574:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("I82574 board"));			break;}
				case ePNDEV_BOARD_IX1000:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("IX1000 Galileo board"));	break;}
				case ePNDEV_BOARD_FPGA1_HERA:		{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("FPGA1-Hera board"));		break;}
				case ePNDEV_BOARD_MICREL_KSZ8841:	{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("Micrel KSZ8841 board"));	break;}
				case ePNDEV_BOARD_MICREL_KSZ8842:	{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("Micrel KSZ8842 board"));	break;}
				case ePNDEV_BOARD_TI_AM5728:		{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("TI AM5728 board"));		break;}
				case ePNDEV_BOARD_CP1625:			{_tcscpy_s(	sBoardType, _countof(sBoardType), _TEXT("CP1625 board"));			break; }
				default:
				{
					_tcscpy_s(	sBoardType,
								_countof(sBoardType),
								_TEXT("#### Invalid board!"));

					break;
				}
			}

			if	(g_uArrayDeviceInfo[i].bInUse)
				// device already in use by current application
			{
				_tcscpy_s(	sDeviceState,
							_countof(sDeviceState),
							_TEXT("[in use]"));
			}
			else
			{
				_tcscpy_s(	sDeviceState,
							_countof(sDeviceState),
							_TEXT(""));
			}

			// show device
			{
				// TabSize = 8 char

				if	(_tcslen(sBoardType) < 30)
				{
					_tprintf(	_TEXT("(%u) %s\t%s\t\t%s\t\t0x%x\t\t0x%x\n"),
								i,
								g_uArrayDeviceInfo[i].sPciLoc,
								sBoardType,
								sDeviceState,
								g_uArrayDeviceInfo[i].lVendorId,
								g_uArrayDeviceInfo[i].lDeviceId);
				}
				else
				{
					_tprintf(	_TEXT("(%u) %s\t%s\t%s\n"),
								i,
								g_uArrayDeviceInfo[i].sPciLoc,
								sBoardType,
								sDeviceState);
				}
			}
		}

		if	(g_lCtrDevice != 0)
			// any device exist
		{
		BOOLEAN	bResultSelect	= FALSE;
		UINT32	lScan			= MAXUINT32;
		char	cScan			= 0;

				if	(g_lCtrDevice == 1)
					// only one device exist
				{
					// select this device automatically
					lScan = 0;

					// success
					bResultSelect = TRUE;
				}
				else if (g_lIndexFixedDevice != MAXUINT32)
						// fixed device
				{
					if	(g_lIndexFixedDevice >= g_lCtrDevice)
						// invalid index
					{
						fnDrvUtil_PrintfColor(	g_hStdout,
												g_eStdout,
												_RED,
												_BLACK,
												_TEXT("#### Invalid fixed device index!\n"));
					}
					else
					{
						lScan = g_lIndexFixedDevice;

						// success
						bResultSelect = TRUE;
					}
				}
				else
				{
				BOOLEAN bValidInput = FALSE;

					// select device by console
					_tprintf(_TEXT("(c) Cancel\n"));

					// select device by index
					_tprintf(_TEXT("\nPlease select desired device...\n"));

					while	(!bValidInput)
							// no valid input
					{
						if	(scanf_s("%c", &cScan, 1) == 0)
							// no fields assigned
						{
							fnDrvUtil_PrintfColor(	g_hStdout,
													g_eStdout,
													_RED,
													_BLACK,
													_TEXT("#### Invalid input, please try again (%c)\n"),
													cScan);
						}
						else if (	(cScan == '\r')
								|| 	(cScan == '\n'))
						{
							// do nothing ignore linefeed
						}
						else if	(cScan == 'c')
								// cancel
						{
							// success
							bValidInput = TRUE;
						}
						else
						{
							// convert '0'..'9' to 0..9
							lScan = cScan - '0';

							if	(	(lScan >= g_lCtrDevice)
								||	(lScan >= DIM_ARRAY_DEVICE))
								// invalid device
							{
								fnDrvUtil_PrintfColor(	g_hStdout,
														g_eStdout,
														_RED,
														_BLACK,
														_TEXT("#### Invalid device index, please try again\n"));

							}
							else
							{
								// success
								bValidInput		= TRUE;
								bResultSelect	= TRUE;
							}
						}
					}
				}


			if	(bResultSelect)
				// success
			{
			UINT32 lDeviceIndexTmp = MAXUINT32;

				bResult = TRUE;

				// update CurDevicePciLoc
				_tcscpy_s(	g_sCurDevicePciLoc,
							_countof(g_sCurDevicePciLoc),
							g_uArrayDeviceInfo[lScan].sPciLoc);

				// update CurBoard
				g_eCurBoard = g_uArrayDeviceInfo[lScan].eBoard;

				if	(g_uArrayDeviceInfo[lScan].bInUse)
					// device already in use by current application
				{
					if	(!fnDrvUtil_GetUsedListIndex(	g_sCurDevicePciLoc,
														DIM_ARRAY_DEVICE,
														g_uArrayUsedDevice,
														&lDeviceIndexTmp))
						// error at getting index of opened device at UsedDeviceList
					{
						bResult = FALSE;

						fnDrvUtil_PrintfColor(	g_hStdout,
												g_eStdout,
												_RED,
												_BLACK,
												_TEXT("#### Opened device not registered at UsedDeviceList!\n"));
					}
				}

				// update CurDeviceIndex
				g_lCurDeviceIndex = lDeviceIndexTmp;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  close device
//************************************************************************

BOOLEAN fnDrvTest_CloseDevice(void)
{
BOOLEAN	bResult = FALSE;
_TCHAR	sError[PNDEV_SIZE_STRING_BUF] = {0};

	if	(!fnDrvTest_DeviceParOk(_countof(sError),
								sError))
		// invalid DevicePar
	{
		fnDrvTest_ShowError(sError);
	}
	else
	{
	ePNDEV_RESULT			eResult			= ePNDEV_FATAL;
	BOOLEAN					bResultClose	= FALSE;
	uPNDEV_CLOSE_DEVICE_IN	uCloseDeviceIn;
	uPNDEV_CLOSE_DEVICE_OUT	uCloseDeviceOut;
	UINT32 i = 0;

		// set InputParameter
		{
			memset(	&uCloseDeviceIn,
					0,
					sizeof(uPNDEV_CLOSE_DEVICE_IN));

			// use standard CloseDevice
			uCloseDeviceIn.uDevice.hHandle	= g_uArrayUsedDevice[g_lCurDeviceIndex].uDevice.hHandle;
			uCloseDeviceIn.bDoAnyway		= FALSE;

			_tcscpy_s(	uCloseDeviceIn.sDriverTarget,
						_countof(uCloseDeviceIn.sDriverTarget),
						_TEXT(""));
		}

		for	(i = 0; i < 10; i++)
			// 10sec
		{
			// close device
			eResult = (*g_hPnDev->uIf.pFnCloseDevice)(	g_hPnDev,
														sizeof(uPNDEV_CLOSE_DEVICE_IN),
														sizeof(uPNDEV_CLOSE_DEVICE_OUT),
														&uCloseDeviceIn,
														&uCloseDeviceOut);

			if	(eResult != ePNDEV_OK)
				// error
			{
				fnDrvUtil_ShowError(g_hStdout,
									g_eStdout,
									eResult,
									uCloseDeviceOut.sError);
			}
			else
			{
				// success
				bResultClose = TRUE;

				// leave loop
				break;
			}

			// wait 1sec
			Sleep(g_SleepTime);
		}

		if	(!bResultClose)
			// error
		{
			// do CloseDevice anyway
			uCloseDeviceIn.bDoAnyway = TRUE;

			// close device
			eResult = (*g_hPnDev->uIf.pFnCloseDevice)(	g_hPnDev,
														sizeof(uPNDEV_CLOSE_DEVICE_IN),
														sizeof(uPNDEV_CLOSE_DEVICE_OUT),
														&uCloseDeviceIn,
														&uCloseDeviceOut);

			if	(eResult != ePNDEV_OK)
				// error
			{
				fnDrvUtil_ShowError(g_hStdout,
									g_eStdout,
									eResult,
									uCloseDeviceOut.sError);
			}
			else
			{
				// success
				bResultClose = TRUE;
			}
		}

		if	(bResultClose)
			// success
		{
			_tprintf(_TEXT("Service CloseDevice done.\n\n"));

			if	(!fnDrvUtil_ReleaseListIndex(	g_lCurDeviceIndex,
												DIM_ARRAY_DEVICE,
												g_uArrayUsedDevice,
												_countof(sError),
												sError))
				// error at releasing used index at UsedDeviceList
			{
				fnDrvTest_ShowError(sError);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			// invalidate
			g_lCurDeviceIndex = MAXUINT32;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  program CP1625
//************************************************************************

BOOLEAN fnDrvMenu_ProgramFlashCP1625(void)
{
BOOLEAN	bResult	= FALSE;

	// Step 1: Open the device
	bResult = fnDrvTest_OpenDeviceCommon(	TRUE,					// MasterApp
											FALSE,					// no downloading of UserFw
											FALSE,					// no debugging
											FALSE,					// no HostMem
											&fnDrvTest_CbfEvent,
											&fnDrvTest_CbfIsr,
											g_sPathDrvBin);

	if(bResult)
	{
		// Step 2: Flash the bootloader
		bResult = fnDrvTest_ProgramFlashCP1625(	ePNDEV_FLASH_ACTION_PROGRAM_CP1625,
												g_sPathDrvBin);


		// Step3: Close the device
		bResult = fnDrvTest_CloseDevice();
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  show driver version
//************************************************************************

void fnDrvTest_ShowDriverVersion(LSA_VERSION_TYPE* pVersionIn)
{
	// show driver version in short format
	fnDrvUtil_PrintfColor(	g_hStdout,
							g_eStdout,
							_YELLOW,
							_BLACK,
							_TEXT("\nDetected PnDev_Driver:\t%u.%u.%u.%u\n"),
							pVersionIn->version,
							pVersionIn->distribution,
							pVersionIn->increment,
							pVersionIn->integration_counter);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if current DevicePar ok
//************************************************************************

BOOLEAN	fnDrvTest_DeviceParOk(	const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(g_lCurDeviceIndex >= DIM_ARRAY_DEVICE)
		// invalid DeviceIndex
	{
		// set ErrorString
		_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
		_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Device not opened (invalid DeviceIndex)!"), _TRUNCATE);
	}
	else
	{
		if	(g_uArrayUsedDevice[g_lCurDeviceIndex].uDevice.hHandle == NULL)
			// device not opened
		{
			// set ErrorString
			_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
			_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Device not opened (hDevice=NULL)!"), _TRUNCATE);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  common code of OpenDevice
//************************************************************************

//#define PNDEV_FW_EXAMPLE

BOOLEAN fnDrvTest_OpenDeviceCommon(	const BOOLEAN			bMasterAppIn,
									const BOOLEAN			bFwDownloadIn,
									const BOOLEAN			bShmDebug,
									const BOOLEAN			bReqHostMemIn,
									const PNDEV_CBF_EVENT	pCbfEventIn,
									const PNDEV_CBF_ISR		pCbfIsrIn,
									const _TCHAR*			sPathDrvBinIn)
{
BOOLEAN	bResult = FALSE;

	if	(_tcscmp(g_sCurDevicePciLoc, _TEXT("")) == 0)
		// no device selected
	{
		fnDrvUtil_PrintfColor(	g_hStdout,
								g_eStdout,
								_RED,
								_BLACK,
								_TEXT("#### No device selected!\n"));
	}
	else
	{
	ePNDEV_RESULT	  eResult				= ePNDEV_FATAL;
	UINT32			    lDeviceIndexTmp		= MAXUINT32;
	BOOLEAN			    bFwDownloadExample	= FALSE;
	_TCHAR			    sError[PNDEV_SIZE_STRING_BUF] = {0};

		if	(!fnDrvUtil_GetFreeListIndex(	DIM_ARRAY_DEVICE,
											g_uArrayUsedDevice,
											_countof(sError),
											&lDeviceIndexTmp,
											sError))
			// error at getting first free index at UsedDeviceList
		{
			fnDrvTest_ShowError(sError);
		}
		else
		{
		uPNDEV_OPEN_DEVICE_IN	uOpenDeviceIn;
		uPNDEV_OPEN_DEVICE_OUT*	pOpenDeviceOut = NULL;

			pOpenDeviceOut = &g_uArrayUsedDevice[lDeviceIndexTmp];

			_tprintf(_TEXT("\nService OpenDevice..."));

			// set InputParameter
			{
				memset(	&uOpenDeviceIn,
						0,
						sizeof(uPNDEV_OPEN_DEVICE_IN));

				_tcscpy_s(	uOpenDeviceIn.sPciLoc,
							_countof(uOpenDeviceIn.sPciLoc),
							g_sCurDevicePciLoc);

				uOpenDeviceIn.bMasterApp			= bMasterAppIn;
				uOpenDeviceIn.bReqHostMem			= bReqHostMemIn;
				uOpenDeviceIn.bInitPhy				= TRUE;
				uOpenDeviceIn.uCbfEvent.pPtr		= pCbfEventIn;
				uOpenDeviceIn.uCbfIsr.pPtr			= pCbfIsrIn;
				uOpenDeviceIn.uCbfParUser.pPtr		= pOpenDeviceOut;
				uOpenDeviceIn.lTimeScaleFactor		= 0;
				uOpenDeviceIn.bVerifyEnvironment	= TRUE;

				// set parameter for DllThread
				{
					uOpenDeviceIn.uThreadDll.lPrio		= PNDEV_THREAD_PRIO_HIGH;
					uOpenDeviceIn.uThreadDll.lStackSize	= 0;

					_tcscpy_s(	uOpenDeviceIn.uThreadDll.sName,
								_countof(uOpenDeviceIn.uThreadDll.sName),
								_TEXT("PnDev_DllThread"));
				}

				// set parameter for SysThread
				{
					uOpenDeviceIn.uThreadSys.lPrio		= PNDEV_THREAD_PRIO_NORMAL;
					uOpenDeviceIn.uThreadSys.lStackSize	= 0;

					_tcscpy_s(	uOpenDeviceIn.uThreadSys.sName,
								_countof(uOpenDeviceIn.uThreadSys.sName),
								_TEXT("PnDev_SysThread"));

					uOpenDeviceIn.uThreadSys.lTimeCycle_10msec = 0;
				}

				// set parameter for BootFw
				{
				_TCHAR sPathBootFw[PNDEV_SIZE_STRING_BUF] = {0};

					switch	(g_eCurBoard)
							// BoardType
					{
						case ePNDEV_BOARD_CP1625:		// CP1625 board
						{
							uOpenDeviceIn.uBootFw.eSrc		= ePNDEV_FW_SRC_FILE;
							uOpenDeviceIn.uBootFw.eFwType	= ePNDEV_FW_TYPE_BOOT;

							// set PathBootFw
							_tcscpy_s(	sPathBootFw, _countof(sPathBootFw), sPathDrvBinIn);

							_tcsncat_s(	sPathBootFw, _countof(sPathBootFw), _TEXT("/BootFw_Jmp_Soc1.img"), _TRUNCATE);

							_tcscpy_s(	uOpenDeviceIn.uBootFw.as.uFile.sPath,
										_countof(uOpenDeviceIn.uBootFw.as.uFile.sPath),
										sPathBootFw);

							break;
						}

						default:
						{
							fnDrvUtil_PrintfColor(	g_hStdout,
													g_eStdout,
													_RED,
													_STDBACKGROUND,
													_TEXT("\n#### Selected Hardware not supported (just CP1625)...\n\n"));

							// clear used entry at UsedDeviceList
							{
								if	(!fnDrvUtil_ReleaseListIndex(	lDeviceIndexTmp,
																	DIM_ARRAY_DEVICE,
																	g_uArrayUsedDevice,
																	_countof(sError),
																	sError))
									// error at releasing used index at UsedDeviceList
								{
									fnDrvTest_ShowError(sError);
								}
							}

							return FALSE;

							break;
						}
					}
				}
			}

			// open device
			eResult = (*g_hPnDev->uIf.pFnOpenDevice)(	g_hPnDev,
														sizeof(uPNDEV_OPEN_DEVICE_IN),
														sizeof(uPNDEV_OPEN_DEVICE_OUT),
														&uOpenDeviceIn,
														pOpenDeviceOut);

			if	(eResult != ePNDEV_OK)
				// error
			{
				fnDrvUtil_ShowError(g_hStdout,
									g_eStdout,
									eResult,
									pOpenDeviceOut->sError);

				// clear used entry at UsedDeviceList
				{
					if	(!fnDrvUtil_ReleaseListIndex(	lDeviceIndexTmp,
														DIM_ARRAY_DEVICE,
														g_uArrayUsedDevice,
														_countof(sError),
														sError))
						// error at releasing used index at UsedDeviceList
					{
						fnDrvTest_ShowError(sError);
					}
				}
			}
			else
			{
				// success
				bResult = TRUE;

				// update CurDeviceIndex
				g_lCurDeviceIndex = lDeviceIndexTmp;

				_tprintf(_TEXT("Done.\n"));
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  common code of ProgramFlash - CP1625
//************************************************************************

BOOLEAN fnDrvTest_ProgramFlashCP1625(	const ePNDEV_FLASH_ACTION	eFlashActionIn,
										const _TCHAR*				sPathDrvBinIn)
{
BOOLEAN	bResult = FALSE;
_TCHAR	sError[PNDEV_SIZE_STRING_BUF] = {0};

	UNREFERENCED_PARAMETER(sPathDrvBinIn);

	if	(!fnDrvTest_DeviceParOk(_countof(sError),
								sError))
		// invalid DevicePar
	{
		fnDrvTest_ShowError(sError);
	}
	else
	{
	BOOLEAN	bConfirmAction	= FALSE;
	BOOLEAN	bCancel			= FALSE;

		if	(!bCancel)
			// not canceled
		{
		ePNDEV_RESULT				eResult = ePNDEV_FATAL;
		uPNDEV_PROGRAM_FLASH_IN		uProgramFlashIn;
		uPNDEV_PROGRAM_FLASH_OUT	uProgramFlashOut;
		_TCHAR						sPathSbl[PNDEV_SIZE_STRING_BUF] = { 0 };

			// preset
			bCancel = FALSE;

			// set InputParameter
			{
				memset(	&uProgramFlashIn,
						0,
						sizeof(uPNDEV_PROGRAM_FLASH_IN));

				uProgramFlashIn.uDevice.hHandle	= g_uArrayUsedDevice[g_lCurDeviceIndex].uDevice.hHandle;
				uProgramFlashIn.eFlashAction	= eFlashActionIn;

				if	(eFlashActionIn == ePNDEV_FLASH_ACTION_PROGRAM_CP1625)
					// ProgramDataCP1625
				{
				BOOLEAN	bValidInput		= FALSE;
				char    sPathIn[200]	= { 0 };

					uProgramFlashIn.as.uSbl.eSrc = ePNDEV_FW_SRC_FILE;

					while (!bValidInput)
						// no valid input
					{
						_tprintf(_TEXT("\nPlease enter the filepath ( e.g. /home/user/bootloader.bin ) (max. 200 characters):\n"));

						if (scanf_s("%s", sPathIn, 200) == 0)
							// no fields assigned
						{
							fnDrvUtil_PrintfColor(	g_hStdout,
													g_eStdout,
													_RED,
													_BLACK,
													_TEXT("#### Invalid input, please try again\n"));
						}
						else
						{
						char	cScan	= 0;
						_TCHAR	sPathInW[200] = { 0 };

							if( access(sPathIn, R_OK) != -1)
							{
								fnConvertStringCharToTchar(sPathIn, sPathInW);

								bValidInput = TRUE;
								bCancel		= FALSE;

								uProgramFlashIn.as.uSbl.uFile.lFlashOffset = 0x0; // offset for bootloader is always 0x0
								_tcsncat_s(sPathSbl, _countof(sPathSbl), sPathInW, _TRUNCATE);
							}
							else
							{
								fnDrvUtil_PrintfColor(	g_hStdout,
														g_eStdout,
														_RED,
														_STDBACKGROUND,
														_TEXT("\n#### Given file could not be opened...\n\n"));

								bValidInput = FALSE;
								bCancel		= FALSE;
							}
						}
					}
				}
			}

			if	(!bCancel)
				// success
			{
				_tprintf(_TEXT("\nStarting flash operation, this may take a while...\n\n"));

				// set PathSbl
				_tcscpy_s(	uProgramFlashIn.as.uSbl.uFile.sPath,
							_countof(uProgramFlashIn.as.uSbl.uFile.sPath),
							sPathSbl);

				// store to flash
				eResult = (*g_hPnDev->uIf.pFnProgramFlash)(	g_hPnDev,
															sizeof(uPNDEV_PROGRAM_FLASH_IN),
															sizeof(uPNDEV_PROGRAM_FLASH_OUT),
															&uProgramFlashIn,
															&uProgramFlashOut);

				if	(eResult != ePNDEV_OK)
					// error
				{
					fnDrvUtil_ShowError(g_hStdout,
										g_eStdout,
										eResult,
										uProgramFlashOut.sError);
				}
				else
				{
					// success
					bResult = TRUE;

					_tprintf(_TEXT("Service ProgramFlash CP1625 done.\n"));
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  show error
//************************************************************************

void fnDrvTest_ShowError(const _TCHAR* sErrorIn)
{
	fnDrvUtil_PrintfColor(	g_hStdout,
							g_eStdout,
							_RED,
							_STDBACKGROUND,
							_TEXT("#### %s \n\n"),
							sErrorIn);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  show error
//************************************************************************

void fnDrvUtil_ShowError(	const HANDLE		hStdoutIn,
							const ePNDEV_STDOUT	eStdoutIn,
							const ePNDEV_RESULT	eResultIn,
							const PNDEV_CHAR*	sErrorIn)
{
PNDEV_CHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

	switch	(eResultIn)
			// ErrorCode
	{
		case ePNDEV_FATAL:				{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_FATAL"));				break;}
		case ePNDEV_FATAL_BUF:			{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_FATAL_BUF"));			break;}
		case ePNDEV_ERROR_NOT_FOUND:	{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_ERROR_NOT_FOUND"));	break;}
		case ePNDEV_ERROR_IN_USE:		{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_ERROR_IN_USE"));		break;}
		case ePNDEV_ERROR_MASTER_APP:	{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_ERROR_MASTER_APP"));	break;}
		case ePNDEV_ERROR_CBF:			{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_ERROR_CBF"));			break;}
		case ePNDEV_ERROR_NO_SUPPORT:	{_stprintf_s(sErrorDetail, _countof(sErrorDetail), _TEXT("ePNDEV_ERROR_NO_SUPPORT"));	break;}
		default:
		{
			// set ErrorDetail
			_stprintf_s(sErrorDetail,
						_countof(sErrorDetail),
						_TEXT("Unknown PnDev_Driver error code!"));

			break;
		}
	}

	fnDrvUtil_PrintfColor(	hStdoutIn,
							eStdoutIn,
							_BLUE,
							_BLACK,
							_TEXT("\n#### %s (%s)\n"),
							sErrorIn,
							sErrorDetail);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  print with color
//************************************************************************

void fnDrvUtil_PrintfColor( const HANDLE    hStdoutIn,
              const ePNDEV_STDOUT eStdoutIn,
              const UINT32    lColorFontIn,
              const UINT32    lColorBackIn,
              const PNDEV_CHAR* sFormatIn, ...)
{
  va_list pListArgs;

    // Note:
    //  - printf uses list of arguments
    //  - types of arguments are defined by parameter "format" of printf
    //  - va_arg() gets the next parameter (size must be known!)

    // set pointer to first unknown argument at stack
    va_start( pListArgs, sFormatIn);

    //Set Colors
    printf("\e[%d;%dm", lColorFontIn, lColorBackIn + 10);

    // write formatted output using a pointer to a list of arguments
    vprintf(sFormatIn, pListArgs);

    //Set to std color output
    printf("\e[0m");

    fflush(hStdoutIn);

    // stop (pointer=NULL)
    va_end(pListArgs);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize variables of CBF-ISR
//************************************************************************

void fnDrvTest_InitVarCbfIsr(void)
{
	memset(	&g_uCbfIsr,
			0,
			sizeof(uCTR_CBF_ISR));
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize variables of CBF-EVENT
//************************************************************************

void fnDrvTest_InitVarCbfEvent(void)
{
	memset(	&g_uCbfEvent,
			0,
			sizeof(uCTR_CBF_EVENT));
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for indicating an event
//************************************************************************

void __stdcall fnDrvTest_CbfEvent(	const HANDLE				hDeviceIn,
									const ePNDEV_EVENT_CLASS	eEventClassIn,
									_TCHAR*						sEventIn,
									const void*					pCbfParUserIn)
{
uPNDEV_OPEN_DEVICE_OUT* pCurDevice = NULL;

	// Note:
	//	- it is not allowed to call any service of PnDev_Driver here!

	UNREFERENCED_PARAMETER(hDeviceIn);

	pCurDevice = (uPNDEV_OPEN_DEVICE_OUT*) pCbfParUserIn;

	switch	(eEventClassIn)
			// EventClass
	{
		case ePNDEV_EVENT_CLASS_TEST:
		{
			// test event - do nothing
			break;
		}
		case ePNDEV_EVENT_CLASS_FATAL_FW:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_MAGENTA,
									_BLACK,
									_TEXT("\n[%s] **** [Firmware fatal] CBF_EVENT (Event='%s')\n\n"),
									pCurDevice->sPciLocShort,
									sEventIn);

			// event of class FatalError
			g_uCbfEvent.bFatal = TRUE;

			break;
		}
		case ePNDEV_EVENT_CLASS_FATAL_ASYNC:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_RED,
									_BLACK,
									_TEXT("\n[%s] **** [Driver FatalAsync] CBF_EVENT (Event='%s')\n\n"),
									pCurDevice->sPciLocShort,
									sEventIn);

			// event of class FatalError
			g_uCbfEvent.bFatal = TRUE;

			break;
		}
		case ePNDEV_EVENT_CLASS_WARNING:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_YELLOW,
									_BLACK,
									_TEXT("\n[%s] **** [Driver warning] CBF_EVENT (Event='%s')\n\n"),
									pCurDevice->sPciLocShort,
									sEventIn);

			break;
		}
		case ePNDEV_EVENT_CLASS_REQ_CLOSE:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_YELLOW,
									_BLACK,
									_TEXT("\n[%s] **** [Driver CloseRequest] CBF_EVENT (Event='%s')\n\n"),
									pCurDevice->sPciLocShort,
									sEventIn);

			break;
		}
		case ePNDEV_EVENT_CLASS_DEBUG:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_YELLOW,
									_BLACK,
									_TEXT("\n[%s] **** [Driver debug] CBF_EVENT (Event='%s')\n\n"),
									pCurDevice->sPciLocShort,
									sEventIn);

			break;
		}
		default:
		{
			fnDrvUtil_PrintfColor(	g_hStdout,
									g_eStdout,
									_RED,
									_BLACK,
									_TEXT("\n[%s] #### CBF_ISR Unexpected EventClass!\n"),
									pCurDevice->sPciLocShort);

			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for indicating an interrupt
//************************************************************************

void __stdcall fnDrvTest_CbfIsr(const HANDLE			hDeviceIn,
								const BOOLEAN			bDeviceTestIn,
								const ePNDEV_INT_SRC	eIntSrcIn,
								const void*				pCbfParUserIn)
{
uPNDEV_OPEN_DEVICE_OUT* pCurDevice = NULL;

	// Note:
	//	- it is not allowed to call any service of PnDev_Driver here!

	UNREFERENCED_PARAMETER(hDeviceIn);

	pCurDevice = (uPNDEV_OPEN_DEVICE_OUT*) pCbfParUserIn;

	if	(bDeviceTestIn)
		// DeviceTest
	{
		switch	(eIntSrcIn)
				// IntSrc
		{
			case ePNDEV_INT_SRC_TEST:
			{
				g_uCbfIsr.lCtrTest++;
				break;
			}
			default:
			{
				fnDrvUtil_PrintfColor(	g_hStdout,
										g_eStdout,
										_RED,
										_BLACK,
										_TEXT("\n[%s] #### CBF_ISR [Test] Unexpected IntSrc!\n"),
										pCurDevice->sPciLocShort);

				break;
			}
		}
	}
	else	// StandardMode
	{
		g_uCbfIsr.lCtrCbfIsr++;

		switch	(pCurDevice->uHwInfo.eAsic)
				// AsicType
		{
			default:
			{
				fnDrvUtil_PrintfColor(	g_hStdout,
										g_eStdout,
										_RED,
										_BLACK,
										_TEXT("\n[%s] #### CBF_ISR [Standard] Unexpected AsicType! (%u)\n"),
										pCurDevice->sPciLocShort,
										pCurDevice->uHwInfo.eAsic);

				break;
			}
		}
	}
}

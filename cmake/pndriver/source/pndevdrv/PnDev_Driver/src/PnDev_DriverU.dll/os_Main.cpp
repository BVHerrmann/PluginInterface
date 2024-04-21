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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: os_Main.cpp                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Entry point of DLL (OS specific)
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//  Variables
//########################################################################

static void* g_hDriver = NULL;												// if driver is opened several times here a list of DriverHandles would be necessary

//************************************************************************
//  D e s c r i p t i o n :
//
//  Entry point
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOL APIENTRY DllMain(	HMODULE	hModuleIn,
						ULONG	lReasonForCallIn,
						LPVOID	pReservedIn)
{
BOOL lResult = TRUE;

	UNREFERENCED_PARAMETER(hModuleIn);
	UNREFERENCED_PARAMETER(pReservedIn);

	switch (lReasonForCallIn)
	{
		case DLL_PROCESS_ATTACH:
		{
			// preset
			g_hDriver = NULL;

			break;
		}
		case DLL_THREAD_ATTACH:
		{
			break;
		}
		case DLL_THREAD_DETACH:
		{
			break;
		}
		case DLL_PROCESS_DETACH:
		{
			if	(g_hDriver != NULL)
				// valid DriverHandle
			{
			ePNDEV_RESULT	eResultTmp						= ePNDEV_FATAL;
			_TCHAR			sError[PNDEV_SIZE_STRING_BUF]	= {0};

				clsDataSet::fnSetTrace(_TEXT("PnDev[]: DLL_PROCESS_DETACH - CloseDriver"));

				// Note for Win7:
				//	- at DLL_PROCESS_DETACH entering CriticalSection sometimes failed (waiting forever) if two Apps - using the same device - were stopped by Ctrl-C
				//		-> don't enter CriticalSection at detaching process (no problem because unloading of DLL is the last action)
				//	- SYS: entering CriticalSection is no problem because it is not unloaded
				{
				clsDataSet* pDataSet = NULL;

					pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) g_hDriver)->pDataSet;

					// suppress entering of CriticalSection
					pDataSet->fnSuppressCritSec(TRUE);
				}

				// close PnDev_Driver
				eResultTmp = fnPnDev_CloseDriver(	(uPNDEV_DRIVER_DESC*) g_hDriver,
													_countof(sError),
													sError);

				if	(eResultTmp != ePNDEV_OK)
					// error
				{
					clsDataSet::fnSetTrace(sError);
				}
			}

			break;
		}
		default:
		{
			break;
		}
	}

	return(lResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  set DriverHandle for DllDetach
//
//	Note:
//	- if App is finished by Ctrl-C then service CloseDriver should be executed
//************************************************************************

void fnSetDriverHandleDetach(HANDLE hDriverIn)
{
	g_hDriver = hDriverIn;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	template
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif


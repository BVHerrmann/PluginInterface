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
/*  F i l e               &F: clsDataSet.cpp                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsDataSet" (DataSet of DLL, dependent functions)
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//	PUBLIC
//########################################################################


//########################################################################
//	PRIVATE
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	start up DLL
//************************************************************************

BOOLEAN clsDataSet::fnStartUpDll(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(fnStartUpLockRes(	lDimErrorIn,
							sErrorOut))
		// starting up resources for locking ok
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	shut down DLL
//************************************************************************

BOOLEAN clsDataSet::fnShutDownDll(	HANDLE			hDriverIn,
									const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultEnter	= FALSE;
BOOLEAN bResultLeave	= FALSE;
BOOLEAN bResultClose	= FALSE;

	//------------------------------------------------------------------------
	// enter CriticalSection
	//	- prevent simultaneous calling of services at different threads
	//	- avoid problems if same device is referenced or UsedDeviceList is used
	bResultEnter = fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL_SERVICE,
									lDimErrorIn,
									sErrorOut);
	{
		// preset
		bResultClose = TRUE;

		// close all used devices
		//	- get DeviceHandles by UsedDeviceList
		//	- close devices in the same way as a user would do it
		{
		BOOLEAN			bStop			= FALSE;
		uLIST_HEADER*	pUsedDeviceTmp	= NULL;

			while	(!bStop)
					// not stopped
			{
				// get first element of UsedDeviceList
				pUsedDeviceTmp = (uLIST_HEADER*) fnGetFirstBlkFromList(fnGetPtrListUsedDevice());

				if	(pUsedDeviceTmp == NULL)
					// no (further) UsedDevice exist
				{
					// stop
					bStop = TRUE;
				}
				else
				{
				uPNDEV_CLOSE_DEVICE_IN	uCloseDeviceIn;
				uPNDEV_CLOSE_DEVICE_OUT	uCloseDeviceOut;
				ePNDEV_RESULT			eResultTmp = ePNDEV_FATAL;

					// by service CloseDevice the UsedDevice will be removed from UsedDeviceList

					// set DebugMsg
					{
					_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: Shut down DLL - Close device anyway"), 
									((clsUsedDevice*) pUsedDeviceTmp)->fnGetPciLoc());

						clsDataSet::fnSetTrace(sTraceDetail);
					}

					// set InputParameter
					{
						memset(	&uCloseDeviceIn,
								0,
								sizeof(uPNDEV_CLOSE_DEVICE_IN));

						uCloseDeviceIn.uDevice.hHandle	= pUsedDeviceTmp;
						uCloseDeviceIn.bDoAnyway		= TRUE;				// do CloseDevice anyway

						_tcscpy_s(	uCloseDeviceIn.sDriverTarget,
									_countof(uCloseDeviceIn.sDriverTarget),
									_TEXT(""));
					}

					// close device
					eResultTmp = fnPnDev_CloseDevice(	hDriverIn,
														sizeof(uPNDEV_CLOSE_DEVICE_IN),
														sizeof(uPNDEV_CLOSE_DEVICE_OUT),
														&uCloseDeviceIn,
														&uCloseDeviceOut);

					if	(eResultTmp != ePNDEV_OK)
						// error
					{
						// error
						bResultClose = FALSE;

						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): "),
													uCloseDeviceOut.sError,
													lDimErrorIn,
													sErrorOut);

						// set DebugMsg
						{
						_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

							_stprintf_s(sTraceDetail,
										_countof(sTraceDetail),
										_TEXT("PnDev[%s]: %s"), 
										((clsUsedDevice*) pUsedDeviceTmp)->fnGetPciLoc(),
										sErrorOut);

							clsDataSet::fnSetTrace(sTraceDetail);
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------------------------
	// leave CriticalSection
	bResultLeave = fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL_SERVICE,
									lDimErrorIn,
									sErrorOut);

	if	(	bResultEnter
		&&	bResultLeave)
		// success
	{
		if	(bResultClose)
			// success
		{
			if	(fnShutDownLockRes(	lDimErrorIn,
									sErrorOut))
			// shutting down resources for locking ok
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do entry actions of each service
//************************************************************************

BOOLEAN clsDataSet::fnDoEntryActions(	const UINT32	lSizeInputBufIn,
										const UINT32	lSizeOutputBufIn,
										const UINT32	lMinSizeInputBufIn,
										const UINT32	lMinSizeOutputBufIn,
										const BOOLEAN	bCompareEqualIn,
										void*			pOutputBufIn,
										HANDLE			hDriverIn,
										const UINT32	lDimErrorIn,
										BOOLEAN*		pFatalBufOut,
										_TCHAR*			sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bResultSize	= TRUE;

	// preset OutputParameter
	*pFatalBufOut = FALSE;

	if	(pOutputBufIn != NULL)
		// valid ptr
	{
		// preset OutputParameter of PnDev_Driver
		//	- if real size > required size of uPNDEV_xx_OUT (=MinSize) then user-specific data behind must not be overwritten!
		//		-> MinSize is the maximal size for preset of OutputParameter

		if	(lSizeOutputBufIn <= lMinSizeOutputBufIn)
			// OutputBuf ok or smaller than expected
		{
			memset(	pOutputBufIn,
					0,
					lSizeOutputBufIn);
		}
		else
		{
			memset(	pOutputBufIn,
					0,
					lMinSizeOutputBufIn);
		}
	}

	// Note:
	//	- if possible compare for equal size
	//	- this increases the chance for finding problems if wrong structure is used

	if	(bCompareEqualIn)
		// compare for equal size
	{
		if	(	(lSizeInputBufIn	!= lMinSizeInputBufIn)
			||	(lSizeOutputBufIn	!= lMinSizeOutputBufIn))
		{
			bResultSize = FALSE;
		}
	}
	else
	{
		if	(	(lSizeInputBufIn	!= lMinSizeInputBufIn)
			||	(lSizeOutputBufIn	< lMinSizeOutputBufIn))
		{
			bResultSize = FALSE;
		}
	}

	if	(!bResultSize)
		// error
	{
	UINT32	lNoCharErrorTmp = 0;
	_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF]	= {0};

		// Note:
		//	- sError is the first element of the output structure
		//	- even if the size of the whole output structure would be smaller than PNDEV_SIZE_STRING_BUF a truncated string can always be written to it
		//	- if service returns an error, user is never allowed to use values of other structure elements
		//	  -> it does not matter if memory of other elements would be overwritten by the ErrorString,
		//		 access of ErrorString is only controlled by terminating null, independent of array dimension

		// signal FatalErrorBuf
		*pFatalBufOut = TRUE;

		// compute maximal CharNumber of ErrorString
		lNoCharErrorTmp = lSizeOutputBufIn / sizeof(_TCHAR);

		// set ErrorDetail
		_stprintf_s(sErrorDetail,
					_countof(sErrorDetail),
					_TEXT("(Real IN: %u, OUT: %u / Desired IN: %u, OUT: %u)"),
					lSizeInputBufIn,
					lSizeOutputBufIn,
					lMinSizeInputBufIn,
					lMinSizeOutputBufIn);

		// set ErrorString
		clsString::fnBuildString(	_TEXT("Invalid size of Input- or Output structure!\n"),
									sErrorDetail,
									NULL,
									lNoCharErrorTmp,
									sErrorOut);
	}
	else
	{
		if	(fnGetDriverVerifier() != hDriverIn)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid DriverHandle!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
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
//  check if device is already in use (UsedDeviceList)
//************************************************************************

BOOLEAN clsDataSet::fnDeviceInUse(_TCHAR* sPciLocIn)
{
BOOLEAN			bResult		= FALSE;
uLIST_HEADER*	pList		= NULL;
uLIST_HEADER*	pBlockTmp	= NULL;

	// get UsedDeviceList
	pList = fnGetPtrListUsedDevice();

	// get first block
	pBlockTmp = (uLIST_HEADER*) pList->pNext;

	while	(pBlockTmp != pList)
			// end of list not reached
	{
		if	(_tcscmp(((clsUsedDevice*) pBlockTmp)->fnGetPciLoc(), sPciLocIn) == 0)
			// device with this PciLoc found
		{
			// success
			bResult = TRUE;

			// leave loop
			break;
		}
		else
		{
			// get next block
			pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;
		}
	}

	return(bResult);
}


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
/*  F i l e               &F: os_clsDataSet.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	OS specific code of class "clsDataSet" (DataSet of DLL, dependent functions)
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//	PUBLIC
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	constructor
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
clsDataSet::clsDataSet(	const UINT32	lDimErrorIn,
						_TCHAR*			sErrorOut)
{
	// preset
	memset(	this,
			0,
			sizeof(clsDataSet));

	// initialize UsedDeviceList
	fnInitList(&m_uListUsedDevice);

	// initialize variables
	m_pInterfaceClassGuid = &GUID_PNDEV_IF;

	if	(!fnAllocDeviceInfoList(m_pInterfaceClassGuid,
								lDimErrorIn,
								&m_hDeviceInfoList,
								&m_lCtrDevice,
								sErrorOut))
		// error at allocating DeviceInfoList
	{
		// ErrorString already set
	}
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
clsDataSet::clsDataSet(	const UINT32	lDimErrorIn,
						_TCHAR*			sErrorOut)
{
	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// preset
	memset(	this,
			0,
			sizeof(clsDataSet));

	// initialize UsedDeviceList
	fnInitList(&m_uListUsedDevice);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
clsDataSet::~clsDataSet(void)
{
	// free DeviceInfoList
	fnFreeDeviceInfoList(m_hDeviceInfoList);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
clsDataSet::~clsDataSet(void)
{
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
clsDataSet::~clsDataSet(void)
{
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up SYS
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnStartUpSys(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	return(TRUE);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnStartUpSys(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN			bResult		= FALSE;
adn_drv_func_t	uFctSys	= {	DriverEntry,		// open
							NULL,				// read
							NULL,				// write
							fnEvtDriverUnload,	// close
							fnEvtIoctl,			// ioctl
							NULL};				// mmap

	if	(adn_drv_register(	PNDEV_DRV_NAME,
							&uFctSys) != ADN_OK)
		// error at registering driver
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at registering driver!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// open driver (for reading and writing)
		//	- DriverEntry() is called
		m_hDriverSys = open(    PNDEV_DRV_NAME,
		                        O_RDWR);

		if	((int32_t) m_hDriverSys == -1)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at opening driver!"),
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

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnStartUpSys( const UINT32  lDimErrorIn,
                  	  	  	  	  _TCHAR*     sErrorOut)
{
BOOLEAN bResult = FALSE;

	// open driver (for reading and writing)
	m_hDriverSys = open(PNDEV_DRV_NAME_UM, O_RDWR | O_SYNC);
    if  (m_hDriverSys == -1)
		// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at opening driver!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}
    
	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down SYS
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnShutDownSys(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	return(TRUE);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnShutDownSys(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bResultClose	= FALSE;

	// preset
	bResultClose = TRUE;

	if	((int32_t) m_hDriverSys != -1)
		// valid DriverHandle
	{
		if	(close((int32_t) m_hDriverSys) != ADN_OK)
			// error at closing driver
			//	- fnEvtDriverUnload() is called
		{
			bResultClose = FALSE;

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at closing driver!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
	}

	if	(bResultClose)
		// success
	{
		if	(adn_drv_unregister(PNDEV_DRV_NAME) != ADN_OK)
			// error at unregistering driver
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at unregistering driver!"),
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

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnShutDownSys(	const UINT32  lDimErrorIn,
									_TCHAR*     sErrorOut)
{
BOOLEAN	bResult	= FALSE;

	if	(m_hDriverSys != -1)
		// valid DriverHandle
	{
		if	(close((int32_t) m_hDriverSys) != LINUX_OK)
			// error at closing driver
			//	- fnEvtDriverUnload() is called
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at closing driver!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			bResult = TRUE;
		}
	}

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get number of devices
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetCtrDevice(	const UINT32	lDimErrorIn,
									UINT32*			pCtrDeviceOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	// return
	(*pCtrDeviceOut) = m_lCtrDevice;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetCtrDevice(	const UINT32	lDimErrorIn,
									UINT32*			pCtrDeviceOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN							bResult = FALSE;
uPNDEV_GET_DEVICE_INFO_SYS_IN	uGetDeviceInfoSysIn;
uPNDEV_GET_DEVICE_INFO_SYS_OUT	uGetDeviceInfoSysOut;

	// set InputParameter
	{
		memset(	&uGetDeviceInfoSysIn,
				0,
				sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN));

		// use DeviceIndex 0
		uGetDeviceInfoSysIn.lIndexDevice = 0;
	}

	if	(!fnDoDeviceIoctl(	NULL,										// DriverService (not a DeviceService) -> no DeviceHandle!
							IOCTL_GET_DEVICE_INFO_SYS,
							sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN),
							sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT),
							&uGetDeviceInfoSysIn,
							&uGetDeviceInfoSysOut))
		// error at IoctlGetDeviceInfoSys
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uGetDeviceInfoSysOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;

		// return
		(*pCtrDeviceOut) = uGetDeviceInfoSysOut.lCtrDevice;
	}

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get DeviceInfo of all devices
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDeviceInfoAll(	const UINT32				lDimOutputArrayIn,
										const UINT32				lDimErrorIn,
										uPNDEV_GET_DEVICE_INFO_OUT	uArrayDeviceInfoOut[],		// array=ptr
										_TCHAR*						sErrorOut)
{
BOOLEAN						bResult			= FALSE;
ePNDEV_BOARD				eBoardTmp		= ePNDEV_BOARD_INVALID;
UINT16						lVendorIdTmp	= 0;
UINT16						lDeviceIdTmp	= 0;
_TCHAR						sHardwareIdTmp[PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR						sPciLocTmp    [PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR						sClassNameTmp [PNDEV_SIZE_STRING_BUF] = {0};
UINT32						lBusTmp			= 0;
UINT32						lDeviceTmp		= 0;
UINT32						lFunctionTmp	= 0;
SP_DEVINFO_DATA				uDeviceElement;
uPNDEV_GET_DEVICE_INFO_OUT	(*pDebug)[32] = NULL;
UINT32						i = 0;

	pDebug = (uPNDEV_GET_DEVICE_INFO_OUT (*)[32]) uArrayDeviceInfoOut;

	// preset OutputParameter
	memset(	uArrayDeviceInfoOut,
			0,
			(sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * lDimOutputArrayIn));

	// initialize structure
	uDeviceElement.cbSize = sizeof(SP_DEVINFO_DATA);

	while	(SetupDiEnumDeviceInfo(	m_hDeviceInfoList,
									i,
									&uDeviceElement))
			// getting DeviceInfoElement ok
	{
		if	(!fnGetDevicePropertiesAll(	m_hDeviceInfoList,
										&uDeviceElement,
										_countof(sHardwareIdTmp),
										_countof(sPciLocTmp),
										_countof(sClassNameTmp),
										lDimErrorIn,
										sHardwareIdTmp,
										sPciLocTmp,
										sClassNameTmp,
										&lBusTmp,
										&lDeviceTmp,
										&lFunctionTmp,
										sErrorOut))
			// error at getting all desired DeviceProperties
		{
			// leave loop
			break;
		}
		else
		{
			// get board type
			eBoardTmp = fnGetBoardType(sHardwareIdTmp,
									   &lVendorIdTmp,
									   &lDeviceIdTmp);
	
			if	(eBoardTmp != ePNDEV_BOARD_INVALID)
				// board found
			{
				// return
				{
					uArrayDeviceInfoOut[i].eBoard 		= eBoardTmp;
					uArrayDeviceInfoOut[i].lVendorId 	= lVendorIdTmp;
					uArrayDeviceInfoOut[i].lDeviceId 	= lDeviceIdTmp;

					uArrayDeviceInfoOut[i].lBus 		= (UINT16) lBusTmp;
					uArrayDeviceInfoOut[i].lDevice 		= (UINT16) lDeviceTmp;
					uArrayDeviceInfoOut[i].lFunction 	= (UINT16) lFunctionTmp;
	
					_tcscpy_s(uArrayDeviceInfoOut[i].sPciLoc,		_countof(uArrayDeviceInfoOut[i].sPciLoc),		sPciLocTmp);
					_tcscpy_s(uArrayDeviceInfoOut[i].sClassName,	_countof(uArrayDeviceInfoOut[i].sClassName),	sClassNameTmp);
	
					if	(fnDeviceInUse(sPciLocTmp))
						// device already in use
					{
						uArrayDeviceInfoOut[i].bInUse = TRUE;
					}
				}
			}
	
			// next device
			i++;
		}
	}

	if	(GetLastError() == ERROR_NO_MORE_ITEMS)
		// success
	{
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDeviceInfoAll(	const UINT32				lDimOutputArrayIn,
										const UINT32				lDimErrorIn,
										uPNDEV_GET_DEVICE_INFO_OUT	uArrayDeviceInfoOut[],		// array=ptr
										_TCHAR*						sErrorOut)
{
BOOLEAN						bResult			= FALSE;
UINT32						lCtrDevice		= 0;

	// preset OutputParameter
	memset(	uArrayDeviceInfoOut,
			0,
			(sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * lDimOutputArrayIn));

	if	(fnGetCtrDevice(lDimErrorIn,
						&lCtrDevice,
						sErrorOut))
		// getting number of devices ok
	{
	uPNDEV_GET_DEVICE_INFO_SYS_IN	uGetDeviceInfoSysIn;
	uPNDEV_GET_DEVICE_INFO_SYS_OUT	uGetDeviceInfoSysOut;
	UINT32							i = 0;

		// preset
		bResult = TRUE;

		for	(i = 0; i < lCtrDevice; i++)
			// all devices
		{
			// set InputParameter
			{
				memset(	&uGetDeviceInfoSysIn,
						0,
						sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN));

				// set desired DeviceIndex
				uGetDeviceInfoSysIn.lIndexDevice = i;
			}

			if	(!fnDoDeviceIoctl(	NULL,									// DriverService (not a DeviceService) -> no DeviceHandle!
									IOCTL_GET_DEVICE_INFO_SYS,
									sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN),
									sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT),
									(void*) &uGetDeviceInfoSysIn,
									(void*) &uGetDeviceInfoSysOut))
				// error at IoctlGetDeviceInfoSys
			{
				bResult = FALSE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											uGetDeviceInfoSysOut.sError,
											lDimErrorIn,
											sErrorOut);

				// leave loop
				break;
			}
			else
			{
				// return
				{
					uArrayDeviceInfoOut[i].eBoard 		= uGetDeviceInfoSysOut.eBoard;
					uArrayDeviceInfoOut[i].lVendorId 	= uGetDeviceInfoSysOut.lVendorId;
					uArrayDeviceInfoOut[i].lDeviceId 	= uGetDeviceInfoSysOut.lDeviceId;
					
					uArrayDeviceInfoOut[i].lBus		 	= uGetDeviceInfoSysOut.lBus;
					uArrayDeviceInfoOut[i].lDevice	 	= uGetDeviceInfoSysOut.lDevice;
					uArrayDeviceInfoOut[i].lFunction 	= uGetDeviceInfoSysOut.lFunction;

					_tcscpy_s(	uArrayDeviceInfoOut[i].sPciLoc,
								_countof(uArrayDeviceInfoOut[i].sPciLoc),
								uGetDeviceInfoSysOut.sPciLoc);

					if	(fnDeviceInUse(uGetDeviceInfoSysOut.sPciLoc))
						// device already in use
					{
						uArrayDeviceInfoOut[i].bInUse = TRUE;
					}
				}
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get DeviceHandle of driver
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDeviceHandleSys(	const _TCHAR*		sPciLocIn,
											const UINT32		lDimErrorIn,
											uPNDEV64_HANDLE*	pHandleDeviceOut,
											BOOLEAN*			pErrorDeviceNotFoundOut,
											_TCHAR*				sErrorOut)
{
BOOLEAN	bResult					= FALSE;
_TCHAR	sDevicePath[MAX_PATH]	= {0};

	// preset OutputParameter
	pHandleDeviceOut->hHandle	= NULL;
	*pErrorDeviceNotFoundOut	= FALSE;

	if	(fnGetDevicePath(	m_hDeviceInfoList,
							m_pInterfaceClassGuid,
							sPciLocIn,
							_countof(sDevicePath),
							lDimErrorIn,
							sDevicePath,
							pErrorDeviceNotFoundOut,
							sErrorOut))
		// getting DevicePath ok
	{
	HANDLE hDeviceTmp = INVALID_HANDLE_VALUE;

		// get DriverHandle
		hDeviceTmp = CreateFile(sDevicePath,
								GENERIC_READ | GENERIC_WRITE,				// data can be read from and written to the file, the file ptr can be moved
								FILE_SHARE_READ | FILE_SHARE_WRITE,			// if zero, the object cannot be shared. Subsequent open operations on the object will fail, until the handle is closed.
								NULL,										// ignored, set always to NULL
								OPEN_EXISTING,								// opens the file/device. The function fails if the file/device does not exist
								FILE_FLAG_OVERLAPPED,						// multiple read/write operations pending on a device at a time are allowed
								NULL);										// ignored, set always to NULL

		if	(hDeviceTmp == INVALID_HANDLE_VALUE)
			// error
		{
			// set ErrorString, append error code of GetLastError()
			fnAddLastError(	_TEXT(__FUNCTION__),
							_TEXT("(): "),
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;

			// return
			(*pHandleDeviceOut).hHandle = hDeviceTmp;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDeviceHandleSys(	const _TCHAR*	    sPciLocIn,
											const UINT32	    lDimErrorIn,
											uPNDEV64_HANDLE*	pHandleDeviceOut,
											BOOLEAN*		    pErrorDeviceNotFoundOut,
											_TCHAR*			    sErrorOut)
{
BOOLEAN								bResult = FALSE;
uPNDEV_GET_DEVICE_HANDLE_SYS_IN		uGetDeviceHandleSysIn;
uPNDEV_GET_DEVICE_HANDLE_SYS_OUT	uGetDeviceHandleSysOut;

	// preset OutputParameter
	pHandleDeviceOut->hHandle	= NULL;
	*pErrorDeviceNotFoundOut	= FALSE;

	// set InputParameter
	{
		memset(	&uGetDeviceHandleSysIn,
				0,
				sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_IN));

		_tcscpy_s(	uGetDeviceHandleSysIn.sPciLoc,
					_countof(uGetDeviceHandleSysIn.sPciLoc),
					sPciLocIn);
	}
  
	if	(!fnDoDeviceIoctl(	NULL,									// DriverService (not a DeviceService) -> no DeviceHandle!
							IOCTL_GET_DEVICE_HANDLE_SYS,
							sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_IN),
							sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT),
							(void*) &uGetDeviceHandleSysIn,
							(void*) &uGetDeviceHandleSysOut))
		// error at IoctlGetDeviceHandleSys
	{
		if	(_tcscmp(uGetDeviceHandleSysOut.sError, PNDEV_ERROR_DEVICE_NOT_FOUND) == 0)
			// device not found
		{
			*pErrorDeviceNotFoundOut = TRUE;
		}
		else
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										uGetDeviceHandleSysOut.sError,
										lDimErrorIn,
										sErrorOut);
		}
	}
	else
	{
		// success
		bResult = TRUE;

		// return
		*pHandleDeviceOut = uGetDeviceHandleSysOut.uDeviceSys;
	}

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//   release DeviceHandleSys
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnReleaseDeviceHandleSys(	const uPNDEV64_HANDLE	hDeviceIn,
												const UINT32			lDimErrorIn,
												_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	if	(!CloseHandle(hDeviceIn.hHandle))
		// error
	{
		// set ErrorString, append error code of GetLastError()
		fnAddLastError(	_TEXT(__FUNCTION__),
						_TEXT("(): "),
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnReleaseDeviceHandleSys(	const uPNDEV64_HANDLE	hDeviceIn,
												const UINT32	        lDimErrorIn,
												_TCHAR*			        sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(hDeviceIn);
	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  do DeviceIoControl
//
// Note:
//	- size of pOut is checked by fnDoEntryActions
//		-> pOut contains at least sError (first element of OutputParameter)
//		-> DLL can always return an ErrorString
//		   (but SYS cannot access pOut in all cases!)
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnDoDeviceIoctl(void*			pUsedDeviceIn,
									const UINT32	lIoctlIn,
									const UINT32	lSizeInputBufIn,
									const UINT32	lSizeOutputBufIn,
									void*			pIn,
									void*			pOut)
{
BOOLEAN				bResult			= FALSE;
BOOLEAN				bResultEnter	= FALSE;
BOOLEAN				bResultLeave	= FALSE;
clsUsedDevice*		pUsedDevice		= NULL;
uPNDEV_IOCTL_OUT*	pOutCommon		= NULL;

	pUsedDevice	= (clsUsedDevice*) pUsedDeviceIn;
	pOutCommon	= (uPNDEV_IOCTL_OUT*) pOut;

	//------------------------------------------------------------------------
	// enter CriticalSection
	//	- prevent simultaneous calling of DeviceIoControl
	//	- fnDoDeviceIoctl() is called at services and DllThread
	//	  (if a running DeviceIoControl was interrupted by another DeviceIoControl then lBytesReturn was corrupted!)
	bResultEnter = fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL_IOCTL,
									_countof(pOutCommon->sError),
									pOutCommon->sError);
	{
		ULONG lBytesReturn = 0;

		if (NULL != pUsedDevice)
			// maybe null pointer? (LINT)
		{
			// Note:
			//	- SYS must retrieve OutputBuf which may fail
			//		-> in such cases an error is returned by NTSTATUS (=LastError)

			if (!DeviceIoControl(pUsedDevice->fnGetHandleDeviceSys().hHandle,	// set context, retrieved by SYS at start of IOCTL
				lIoctlIn,
				(void*)pIn,
				lSizeInputBufIn,
				pOut,
				lSizeOutputBufIn,
				&lBytesReturn,
				NULL))
				// error at DeviceIoControl (= value of NTSTATUS at driver)
			{
				// set ErrorString, append error code of GetLastError()
				fnAddLastError(_TEXT(__FUNCTION__),
					_TEXT("(): "),
					_countof(pOutCommon->sError),
					pOutCommon->sError);
			}
			else
			{
				// driver returned STATUS_SUCCESS

				if (lBytesReturn != lSizeOutputBufIn)
					// invalid number of returned bytes
				{
					// set ErrorString
					clsString::fnBuildString(_TEXT(__FUNCTION__),
						_TEXT("(): DeviceIoControl: Invalid number of returned bytes!"),
						NULL,
						_countof(pOutCommon->sError),
						pOutCommon->sError);
				}
				else
				{
					// if other errors occurs then they are returned by pOut->sError

					if (_tcscmp(pOutCommon->sError, _TEXT("")) == 0)
						// no FatalError
					{
						// success
						bResult = TRUE;
					}
				}
			}
		}
		else
		{
			// set ErrorString
			clsString::fnBuildString(_TEXT(__FUNCTION__),
				_TEXT("(): DeviceIoControl: null pointer detected!"),
				NULL,
				_countof(pOutCommon->sError),
				pOutCommon->sError);
		}
	}
	//------------------------------------------------------------------------
	// leave CriticalSection
	bResultLeave = fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL_IOCTL,
									_countof(pOutCommon->sError),
									pOutCommon->sError);

	if	(	!bResultEnter
		||	!bResultLeave)
		// error
	{
		bResult = FALSE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnDoDeviceIoctl(void*			pUsedDeviceIn,			// may be NULL!
									const UINT32	lIoctlIn,
									const UINT32	lSizeInputBufIn,
									const UINT32	lSizeOutputBufIn,
									void*			pIn,
									void*			pOut)
{
BOOLEAN				bResult			= FALSE;
BOOLEAN				bResultEnter	= FALSE;
BOOLEAN				bResultLeave	= FALSE;
clsUsedDevice*		pUsedDevice		= NULL;
uPNDEV_IOCTL_OUT*	pOutCommon		= NULL;

	pUsedDevice	= (clsUsedDevice*) pUsedDeviceIn;
	pOutCommon	= (uPNDEV_IOCTL_OUT*) pOut;

	//------------------------------------------------------------------------
	// enter CriticalSection
	//	- prevent simultaneous calling of DeviceIoControl
	//	- fnDoDeviceIoctl() is called at services and DllThread
	bResultEnter = fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL_IOCTL,
									_countof(pOutCommon->sError),
									pOutCommon->sError);
	{
	uPNDEV_IOCTL_ARG uIoctlArg;

		memset(	&uIoctlArg,
				0,
				sizeof(uPNDEV_IOCTL_ARG));

		// set IoctlArg
		{
			if	(pUsedDevice != NULL)
				// DriverService (not a DeviceService) -> no DeviceHandle!
			{
				uIoctlArg.uDeviceSys = pUsedDevice->fnGetHandleDeviceSys();
			}

			uIoctlArg.lSizeInputBuf		= lSizeInputBufIn;
			uIoctlArg.lSizeOutputBuf	= lSizeOutputBufIn;
			uIoctlArg.uIn.pPtr			= pIn;
			uIoctlArg.uOut.pPtr			= pOut;
		}

		// Note:
		//	- SYS can always access OutputBuf if passed
		//		-> if an error occurs it can nearly always be returned by pOut->sError
		if	(ioctl(	(int32_t) m_hDriverSys,
					lIoctlIn,
					&uIoctlArg) != POSIX_OK)
			// error at ioctl
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at ioctl of driver!"),
										NULL,
										_countof(pOutCommon->sError),
										pOutCommon->sError);
		}
		else
		{
			// if other errors exist then they are returned by pOut->sError

			if	(_tcscmp(pOutCommon->sError, _TEXT("")) == 0)
				// no FatalError
			{
				// success
				bResult = TRUE;
			}
		}
	}
	//------------------------------------------------------------------------
	// leave CriticalSection
	bResultLeave = fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL_IOCTL,
									_countof(pOutCommon->sError),
									pOutCommon->sError);

	if	(	!bResultEnter
		||	!bResultLeave)
		// error
	{
		bResult = FALSE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	start up resources for locking
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnStartUpLockRes(	const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	// initialize CriticalSections
	InitializeCriticalSection(&m_uCritSec_Service);
	InitializeCriticalSection(&m_uCritSec_Ioctl);

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnStartUpLockRes(	const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(fnCreateMutex(	lDimErrorIn,
						&m_uCritSec_Service,
						sErrorOut))
		// creating mutex ok
	{
		if	(fnCreateMutex(	lDimErrorIn,
							&m_uCritSec_Ioctl,
							sErrorOut))
			// creating mutex ok
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	shut down resources for locking
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnShutDownLockRes(	const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	// delete CriticalSections
	DeleteCriticalSection(&m_uCritSec_Service);
	DeleteCriticalSection(&m_uCritSec_Ioctl);

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnShutDownLockRes(	const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(fnDeleteMutex(	&m_uCritSec_Service,
						lDimErrorIn,
						sErrorOut))
		// deleting mutex ok
	{
		if	(fnDeleteMutex(	&m_uCritSec_Ioctl,
							lDimErrorIn,
							sErrorOut))
			// deleting mutex ok
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  enter CriticalSection
//
//	Note:
//	- CriticalSection must be nestable because of calling a service inside a service
//	- after a thread has ownership of a critical section it can make additional calls to fnEnterCritSec() without blocking its execution
//	- this prevents a thread from deadlocking itself while waiting for a critical section that it already owns
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL	eCritSecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(m_bSuppressCritSec)
		// suppress entering of CriticalSection
	{
		// success
		bResult = TRUE;
	}
	else
	{
	HANDLE hCritSec = NULL;

		switch	(eCritSecIn)
				// CritSec
		{
			case ePNDEV_CRIT_SEC_DLL_SERVICE:	{hCritSec = &m_uCritSec_Service;	break;}
			case ePNDEV_CRIT_SEC_DLL_IOCTL:		{hCritSec = &m_uCritSec_Ioctl;		break;}
			default:
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid CriticalSection!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}

		if	(hCritSec != NULL)
			// success
		{
			bResult = TRUE;

			// enter CriticalSection
			EnterCriticalSection((CRITICAL_SECTION*) hCritSec);
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL	eCritSecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(m_bSuppressCritSec)
		// suppress entering of CriticalSection
	{
		// success
		bResult = TRUE;
	}
	else
	{
	HANDLE hCritSec = NULL;

		switch	(eCritSecIn)
				// CritSec
		{
			case ePNDEV_CRIT_SEC_DLL_SERVICE:	{hCritSec = &m_uCritSec_Service;	break;}
			case ePNDEV_CRIT_SEC_DLL_IOCTL:		{hCritSec = &m_uCritSec_Ioctl;		break;}
			default:
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid CriticalSection!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}

		if	(hCritSec != NULL)
			// success
		{
			if	(pthread_mutex_lock((pthread_mutex_t*) hCritSec) != POSIX_OK)
				// error at entering CriticalSection
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at pthread_mutex_lock()!"),
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
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  leave CriticalSection
//
//	Note:
//	- CriticalSection must be nestable because of calling a service inside a service
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL eCritSecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(m_bSuppressCritSec)
		// suppress entering of CriticalSection
	{
		// success
		bResult = TRUE;
	}
	else
	{
	HANDLE hCritSec = NULL;

		switch	(eCritSecIn)
				// CritSec
		{
			case ePNDEV_CRIT_SEC_DLL_SERVICE:	{hCritSec = &m_uCritSec_Service;	break;}
			case ePNDEV_CRIT_SEC_DLL_IOCTL:		{hCritSec = &m_uCritSec_Ioctl;		break;}
			default:
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid CriticalSection!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}

		if	(hCritSec != NULL)
			// success
		{
			// success
			bResult = TRUE;

			// leave CriticalSection
			LeaveCriticalSection((CRITICAL_SECTION*) hCritSec);
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL	eCritSecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(m_bSuppressCritSec)
		// suppress entering of CriticalSection
	{
		// success
		bResult = TRUE;
	}
	else
	{
	HANDLE hCritSec = NULL;

		switch	(eCritSecIn)
				// CritSec
		{
			case ePNDEV_CRIT_SEC_DLL_SERVICE:	{hCritSec = &m_uCritSec_Service;	break;}
			case ePNDEV_CRIT_SEC_DLL_IOCTL:		{hCritSec = &m_uCritSec_Ioctl;		break;}
			default:
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid CriticalSection!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}

		if	(hCritSec != NULL)
			// success
		{
			if	(pthread_mutex_unlock((pthread_mutex_t*) hCritSec) != POSIX_OK)
				// error at leaving CriticalSection
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at pthread_mutex_unlock()!"),
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
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check further version details
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnCheckVersionDetails(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult					= FALSE;
UINT32	lVersionMajor_Dll		= 0;
UINT32	lVersionMinor_Dll		= 0;
UINT32	lVersionBuild_Dll		= 0;
UINT32	lVersionPrivate_Dll		= 0;
UINT32	lVersionMajor_Sys		= 0;
UINT32	lVersionMinor_Sys		= 0;
UINT32	lVersionBuild_Sys		= 0;
UINT32	lVersionPrivate_Sys		= 0;
_TCHAR	sPathDriver[MAX_PATH]	= {0};

	if	(!GetSystemDirectory(	sPathDriver,
								_countof(sPathDriver)))
		// error at getting SystemDirectory
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at GetSystemDirectory()!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	_TCHAR	sPathDll[PNDEV_SIZE_STRING_BUF] = {0};

		#if (PNDEV_PLATFORM == PNDEV_UM_PLATFORM_64)

			// selected platform = x64
			_tcscpy_s(	sPathDll,
						_countof(sPathDll),
						_TEXT("PnDev_DriverU64.dll"));
		#else

			// selected platform = Win32
			_tcscpy_s(	sPathDll,
						_countof(sPathDll),
						_TEXT("PnDev_DriverU32.dll"));
		#endif


		// append SystemDirectory
		_tcsncat_s(	sPathDriver,
					_countof(sPathDriver),
					_TEXT("\\drivers\\PnDev_Driver.sys"),
					_TRUNCATE);

		if	(clsFile::fnGetFileVersion(	sPathDll,
										&lVersionMajor_Dll,
										&lVersionMinor_Dll,
										&lVersionBuild_Dll,
										&lVersionPrivate_Dll,
										lDimErrorIn,
										sErrorOut))
			// getting FileVersion of DLL ok
		{
			if	(clsFile::fnGetFileVersion(	sPathDriver,
											&lVersionMajor_Sys,
											&lVersionMinor_Sys,
											&lVersionBuild_Sys,
											&lVersionPrivate_Sys,
											lDimErrorIn,
											sErrorOut))
				// getting FileVersion of SYS ok
			{
				if	(	(lVersionMajor_Dll		!= lVersionMajor_Sys)
					||	(lVersionMinor_Dll		!= lVersionMinor_Sys)
					)
					// different versions
				{
				_TCHAR	sVersionDll[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
				_TCHAR	sVersionSys[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};

					// set VersionString of DLL/SYS
					{
						_stprintf_s(sVersionDll,	_countof(sVersionDll),	_TEXT("DLL: %u.%u.%u.%u\n"),	lVersionMajor_Dll,
																											lVersionMinor_Dll,
																											lVersionBuild_Dll,
																											lVersionPrivate_Dll);
						_stprintf_s(sVersionSys,	_countof(sVersionSys),	_TEXT("SYS: %u.%u.%u.%u"),		lVersionMajor_Sys,
																											lVersionMinor_Sys,
																											lVersionBuild_Sys,
																											lVersionPrivate_Sys);
					}

					// set ErrorString
					clsString::fnBuildString(	_TEXT("Version mismatch of PnDev_DriverU.dll and PnDev_Driver.sys:\n"),
												sVersionDll,
												sVersionSys,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;
				}
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnCheckVersionDetails(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	// success
	bResult = TRUE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnCheckVersionDetails(  const UINT32  lDimErrorIn,
                      	  	  	  	  	  	  _TCHAR*     sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	return(bResult);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	set ErrorString, append error description of GetLastError()
//	- error codes: see WinError.h
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void clsDataSet::fnAddLastError(const _TCHAR*	sSrc1In,
								const _TCHAR*	sSrc2In,
								const UINT32	lDimResultIn,
								_TCHAR*			sResultOut)
{
errno_t lError = -1;

	// preset OutputParameter
	_tcscpy_s(	sResultOut,
				lDimResultIn,
				_TEXT(""));

	// append strings to OutputString
	if (sSrc1In != NULL)	{lError = _tcsncat_s(sResultOut, lDimResultIn, sSrc1In, _TRUNCATE);}
	if (sSrc2In != NULL)	{lError = _tcsncat_s(sResultOut, lDimResultIn, sSrc2In, _TRUNCATE);}

	// if (lError == STRUNCATE)
	{
		// truncation occurred
	}

	// append LastError
	{
	UINT32	lLastError = 0;
	_TCHAR	sLastError[PNDEV_SIZE_STRING_BUF]				= {0};
	_TCHAR	sLastErrorDescription[PNDEV_SIZE_STRING_BUF]	= {0};

		// Note:
		//	- The actual error number for "_tcserror" is stored in the variable "errno" (returned by GetLastError()).
		//	- The system error messages are accessed through the variable "_sys_errlist", which is an array of messages ordered by error number.
		//	- "_tcserror" accesses the appropriate error message by using the "errno" value as an index to the variable "_sys_errlist".
		//	  => there is no internal allocation of a string buffer which must be released

		// get LastError
		//	- Note: value is not cleared by read
		lLastError = GetLastError();

		// set LastErrorString
		//_ltot_s(lLastError, sLastError, _countof(sLastError), 10);
		_stprintf_s(sLastError,
					_countof(sLastError),
					_TEXT("(Error=0x%x) "),
					lLastError);

		// append LastErrorString
		_tcsncat_s(	sResultOut,
					lDimResultIn,
					sLastError,
					_TRUNCATE);

		if	(lLastError == ERROR_ACCESS_DENIED)
			// error code 'Access is denied'
		{
			// append hint
			_tcsncat_s(	sResultOut,
						lDimResultIn,
						_TEXT("- Did you start application with administration rights?"),
						_TRUNCATE);
		}
		else
		{
			// try to get LastErrorDescription
			lError = _tcserror_s(sLastErrorDescription, _countof(sLastErrorDescription), lLastError);

			if	(lError == ERROR_SUCCESS)
				// success
			{
				// append LastErrorDescription
				_tcsncat_s(	sResultOut,
							lDimResultIn,
							sLastErrorDescription,
							_TRUNCATE);
			}
			else
			{
				switch	(lLastError)
						// error code
				{
					case ERROR_FILE_NOT_FOUND:	{_tcsncat_s(sResultOut, lDimResultIn, _TEXT("File not found"),		_TRUNCATE);	break;}
					case ERROR_PATH_NOT_FOUND:	{_tcsncat_s(sResultOut, lDimResultIn, _TEXT("Path not found"),		_TRUNCATE);	break;}

					default:
					{
						break;
					}
				}
			}
		}
	}
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void clsDataSet::fnAddLastError(const _TCHAR*	sSrc1In,
								const _TCHAR*	sSrc2In,
								const UINT32	lDimResultIn,
								_TCHAR*			sResultOut)
{
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void clsDataSet::fnAddLastError(const _TCHAR* sSrc1In,
								const _TCHAR* sSrc2In,
								const UINT32  lDimResultIn,
								_TCHAR*     sResultOut)
{
	UNREFERENCED_PARAMETER(sSrc1In);
	UNREFERENCED_PARAMETER(sSrc2In);
	UNREFERENCED_PARAMETER(lDimResultIn);
	UNREFERENCED_PARAMETER(sResultOut);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	set string of ListBox showing current action
//	(TextBox wasn't updated very often)
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void clsDataSet::fnSetCurAction(const void*		hListBoxIn,
								const _TCHAR*	sMsgIn)
{
	if	(hListBoxIn != 0)
		// debug desired
	{
	LRESULT lCtrEntries = 0;

		// put string to ListBox
		SendMessage(((HWND) hListBoxIn), LB_ADDSTRING, 0, (LPARAM) sMsgIn);

		// get number of entries
		lCtrEntries = SendMessage(((HWND) hListBoxIn), LB_GETCOUNT, 0, 0);

		if	(	(lCtrEntries != 0)
			&&	(lCtrEntries != LB_ERR))	// error
			// valid number of entries
		{
			if	(_tcscmp(sMsgIn, _TEXT("")) == 0)
				// empty string
			{
				// remove all elements at ListBox
				SendMessage(((HWND) hListBoxIn), LB_RESETCONTENT, (lCtrEntries - 1), 0);
			}
			else
			{
				// select last element at ListBox
				SendMessage(((HWND) hListBoxIn), LB_SETCURSEL, (lCtrEntries - 1), 0);
			}

			if	(lCtrEntries > 1)
				// too many entries
			{
				// remove first entry
				SendMessage(((HWND) hListBoxIn), LB_DELETESTRING, 0, 0);
			}
		}
	}

	// set string of TextBox
	//SetWindowText((HWND) hTextBoxIn , sMsgIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void clsDataSet::fnSetCurAction(const void*		hListBoxIn,
								const _TCHAR*	sMsgIn)
{
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void clsDataSet::fnSetCurAction(const void*   hListBoxIn,
                				const _TCHAR* sMsgIn)
{
	UNREFERENCED_PARAMETER(hListBoxIn);
	UNREFERENCED_PARAMETER(sMsgIn);
}
//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	set trace
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
void clsDataSet::fnSetTrace(const _TCHAR* sTraceIn)
{
	OutputDebugString(sTraceIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
void clsDataSet::fnSetTrace(const _TCHAR* sTraceIn)
{
	fnSetTraceDll(sTraceIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
void clsDataSet::fnSetTrace(const _TCHAR* sTraceIn)
{
	UNREFERENCED_PARAMETER(sTraceIn);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//########################################################################
//	PRIVATE
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate DeviceInfoList
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnAllocDeviceInfoList(	const GUID*		pInterfaceClassGuidIn,	// GUID for a device setup class or a device interface class
											const UINT32	lDimErrorIn,
											HDEVINFO*		pDeviceInfoListOut,
											UINT32*			pCtrDeviceGuidOut,
											_TCHAR*			sErrorOut)
{
BOOLEAN		bResult				= FALSE;
HDEVINFO	hDeviceInfoListTmp	= INVALID_HANDLE_VALUE;

	// preset OutputParameter
	(*pDeviceInfoListOut)	= NULL;
	(*pCtrDeviceGuidOut)	= 0;

	hDeviceInfoListTmp = SetupDiGetClassDevs(	pInterfaceClassGuidIn,
												NULL,
												NULL,
												(DIGCF_DEVICEINTERFACE | DIGCF_PRESENT));

	if	(hDeviceInfoListTmp == INVALID_HANDLE_VALUE)
		// error
	{
		// set ErrorString, append error code of GetLastError()
		fnAddLastError(	_TEXT(__FUNCTION__),
						_TEXT("(): "),
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
	UINT32						lCtrDeviceGuidTmp = 0;
	SP_DEVICE_INTERFACE_DATA	uInterfaceElement;

		// get number of devices with desired GUID

		// initialize structure
		uInterfaceElement.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

		while	(SetupDiEnumDeviceInterfaces(	hDeviceInfoListTmp,
												NULL,
												pInterfaceClassGuidIn,
												lCtrDeviceGuidTmp,			// index
												&uInterfaceElement))
				// getting DeviceInterfaceElement ok
		{
			// next device found
			lCtrDeviceGuidTmp++;
		}

		if	(GetLastError() != ERROR_NO_MORE_ITEMS)
			// not expected error
		{
			// set ErrorString, append error code of GetLastError()
			fnAddLastError(	_TEXT(__FUNCTION__),
							_TEXT("(): "),
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;

			// return
			(*pDeviceInfoListOut)	= hDeviceInfoListTmp;
			(*pCtrDeviceGuidOut)	= lCtrDeviceGuidTmp;
		}
	}

	return(bResult);
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
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  free DeviceInfoList
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnFreeDeviceInfoList(const HDEVINFO hDeviceInfoListIn)
{
BOOLEAN bResult = FALSE;

	if	(hDeviceInfoListIn != NULL)
		// valid handle
	{
		if	(SetupDiDestroyDeviceInfoList(hDeviceInfoListIn))
			// destroying DeviceInfoList ok
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
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
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get DevicePath
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsDataSet::fnGetDevicePath(const HDEVINFO	hDeviceInfoListIn,
									const GUID*		pInterfaceClassGuidIn,
									const _TCHAR*	sPciLocIn,
									const UINT32	lDimDevicePathIn,
									const UINT32	lDimErrorIn,
									_TCHAR*			sDevicePathOut,
									BOOLEAN*		pErrorDeviceNotFoundOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN			bResult			= FALSE;
BOOLEAN			bDeviceFound	= FALSE;
SP_DEVINFO_DATA	uDeviceElement;
_TCHAR			sHardwareIdTmp[PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR			sPciLocTmp    [PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR			sClassNameTmp [PNDEV_SIZE_STRING_BUF] = {0};
UINT32			i = 0;

	// preset OutputParameter
	{
		(*pErrorDeviceNotFoundOut) = FALSE;

		_tcscpy_s(	sDevicePathOut,
					lDimDevicePathIn,
					_TEXT(""));
	}

	// initialize structure
	uDeviceElement.cbSize = sizeof(SP_DEVINFO_DATA);

	while	(SetupDiEnumDeviceInfo(	hDeviceInfoListIn,
									i,
									&uDeviceElement))
			// getting DeviceInfoElement ok
	{
		if	(!fnGetDevicePropertiesAll(	hDeviceInfoListIn,
										&uDeviceElement,
										_countof(sHardwareIdTmp),
										_countof(sPciLocTmp),
										_countof(sClassNameTmp),
										lDimErrorIn,
										sHardwareIdTmp,
										sPciLocTmp,
										sClassNameTmp,
										NULL,
										NULL,
										NULL,
										sErrorOut))
			// error at getting all desired DeviceProperties
		{
			// leave loop
			break;
		}
		else
		{
			if	(_tcscmp(sPciLocTmp, sPciLocIn) == 0)
				// board with desired PciLoc found
			{
				// success
				bDeviceFound = TRUE;

				// leave loop
				break;
			}

			// next device
			i++;
		}
	}

	if	(!bDeviceFound)
		// error
	{
		*pErrorDeviceNotFoundOut = TRUE;
	}
	else
	{
		// verify DeviceElement

		if	(fnGetDevicePropertiesAll(	hDeviceInfoListIn,
										&uDeviceElement,
										_countof(sHardwareIdTmp),
										_countof(sPciLocTmp),
										_countof(sClassNameTmp),
										lDimErrorIn,
										sHardwareIdTmp,
										sPciLocTmp,
										sClassNameTmp,
										NULL,
										NULL,
										NULL,
										sErrorOut))
			// getting all desired DeviceProperties ok
		{
			if	(_tcscmp(sPciLocTmp, sPciLocIn) == 0)
				// desired device
			{
			SP_DEVICE_INTERFACE_DATA uInterfaceElement;

				// SetupDiEnumDeviceInterfaces():
				//	- if second parameter (DeviceElement) is specified (!= NULL) then only interfaces, supported by the specified device, are returned
				//	- exactly one interface must be returned because driver initializes one interface

				// preset
				i = 0;

				// initialize structure
				uInterfaceElement.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);

				while	(SetupDiEnumDeviceInterfaces(	hDeviceInfoListIn,
														&uDeviceElement,			// requested device
														pInterfaceClassGuidIn,		// requested interface
														i,
														&uInterfaceElement))
						// getting DeviceInterfaceElement ok
				{
					i++;
				}

				if	(GetLastError() != ERROR_NO_MORE_ITEMS)
					// not expected error
				{
					// set ErrorString, append error code of GetLastError()
					fnAddLastError(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									lDimErrorIn,
									sErrorOut);
				}
				else if	(i == 0)
						// no interface
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): No interface found!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else if	(i > 1)
						// invalid number of interfaces
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid number of interfaces!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
				ULONG								lSize					= 0;
				PSP_DEVICE_INTERFACE_DETAIL_DATA	pInterfaceElementDetail	= NULL;
				UINT8*								pInterfaceElementDetail8 = NULL;

					//	- first call:	get required buffer size
					//	- second call:	get data

					// get required size for InterfaceElementDetail buffer
					SetupDiGetDeviceInterfaceDetail(hDeviceInfoListIn,
													&uInterfaceElement,
													pInterfaceElementDetail,
													0,							// size of InterfaceElementDetail buffer
													&lSize,
													NULL);						// pDeviceElement, optional buffer to receive information about device that supports requested interface

					if	(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
						// not expected error
					{
						// set ErrorString, append error code of GetLastError()
						fnAddLastError(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// allocate buffer
						pInterfaceElementDetail8 = (UINT8*) new UINT8[lSize];
						pInterfaceElementDetail = (PSP_DEVICE_INTERFACE_DETAIL_DATA) pInterfaceElementDetail8;

						if (pInterfaceElementDetail == NULL)
						{
							// set ErrorString
							clsString::fnBuildString(	_TEXT(__FUNCTION__),
														_TEXT("(): Insufficient memory for InterfaceDetailData!"),
														NULL,
														lDimErrorIn,
														sErrorOut);
						}
						else
						{
							// initialize structures
							uDeviceElement.cbSize			= sizeof(SP_DEVINFO_DATA);
							pInterfaceElementDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);

							if	(!SetupDiGetDeviceInterfaceDetail(	hDeviceInfoListIn,
																	&uInterfaceElement,
																	pInterfaceElementDetail,
																	lSize,
																	NULL,
																	&uDeviceElement))
								// error at getting InterfaceElementDetail
							{
								// set ErrorString, append error code of GetLastError()
								fnAddLastError(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												lDimErrorIn,
												sErrorOut);
							}
							else
							{
								// e.g. DevicePath = "\\?\pci#ven_110a&dev_4073&subsys_00000000&rev_02#4&26198c6b&0&00e0#{70ef9c99-5112-45a3-901c-c69ad2c63f59}"
								_tcscpy_s(	sDevicePathOut,
											lDimDevicePathIn,
											pInterfaceElementDetail->DevicePath);

								// success
								bResult = TRUE;
							}
						}
					}
				}
			}
		}
	}

	return(bResult);
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
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get property of detected device
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDeviceProperty(const HDEVINFO		hDeviceInfoListIn,
										SP_DEVINFO_DATA*	pDeviceElementIn,
										const UINT32		lPropertyIn,			// SPDRP_HARDWAREID, SPDRP_LOCATION_INFORMATION, SPDRP_CLASS
										const UINT32		lDimErrorIn,
										_TCHAR*				sInfoOut,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
TCHAR*	sInfoTmp	= NULL;
UINT8*	sInfoTmp8	= NULL;
ULONG	lSize		= 0;

	// preset OutputParameter
	_tcscpy_s(sErrorOut, lDimErrorIn,	_TEXT(""));

	//	- first call:	get required buffer size
	//	- second call:	get data

	// get required size for property buffer
	SetupDiGetDeviceRegistryProperty(	hDeviceInfoListIn,
										pDeviceElementIn,
										lPropertyIn,
										NULL,
										(PBYTE) sInfoTmp,
										0,
										&lSize);

	if	(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
		// not expected error
	{
		// set ErrorString, append error code of GetLastError()
		fnAddLastError(	_TEXT(__FUNCTION__),
						_TEXT("(): "),
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		// allocate buffer
		sInfoTmp8 = (UINT8*) new UINT8[lSize];
		sInfoTmp = (TCHAR*) sInfoTmp8;

		if	(sInfoTmp == NULL)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Insufficient memory for DeviceProperty!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(!SetupDiGetDeviceRegistryProperty(	hDeviceInfoListIn,
													pDeviceElementIn,
													lPropertyIn,
													NULL,
													(PBYTE) sInfoOut,
													lSize,
													NULL))
				// error at getting RegistryProperty
			{
				// set ErrorString, append error code of GetLastError()
				fnAddLastError(	_TEXT(__FUNCTION__),
								_TEXT("(): "),
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult	= TRUE;
			}

			// free buffer
			delete[] sInfoTmp;
			sInfoTmp = NULL;
		}
	}

	return(bResult);
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
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get all desired device properties
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsDataSet::fnGetDevicePropertiesAll(	const HDEVINFO		hDeviceInfoListIn,
												SP_DEVINFO_DATA*	pDeviceElementIn,
												const UINT32		lDimHardwareIdIn,
												const UINT32		lDimPciLocIn,
												const UINT32		lDimClassNameIn,
												const UINT32		lDimErrorIn,
												_TCHAR*				sHardwareIdOut,
												_TCHAR*				sPciLocOut,
												_TCHAR*				sClassNameOut,
												UINT32*				pBusOut,
												UINT32*				pDeviceOut,
												UINT32*				pFunctionOut,
												_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	_tcscpy_s(sHardwareIdOut, lDimHardwareIdIn,	_TEXT(""));
	_tcscpy_s(sPciLocOut,	  lDimPciLocIn,		_TEXT(""));
	_tcscpy_s(sClassNameOut,  lDimClassNameIn,	_TEXT(""));
	_tcscpy_s(sErrorOut,	  lDimErrorIn,		_TEXT(""));

	if	(fnGetDeviceProperty(	hDeviceInfoListIn,
								pDeviceElementIn,
								SPDRP_HARDWAREID,
								lDimErrorIn,
								sHardwareIdOut,
								sErrorOut))
		// getting HardwareId ok
	{
		if	(fnGetDeviceProperty(	hDeviceInfoListIn,
									pDeviceElementIn,
									SPDRP_CLASS,
									lDimErrorIn,
									sClassNameOut,
									sErrorOut))
			// getting ClassName ok
		{
			if	(!clsString::fnSearchSubString(	_TEXT("PCI"),
												sHardwareIdOut,
												TRUE))
				// non PCI device
			{
				// don't try to get DeviceProperty (otherwise error ERROR_INVALID_DATA at fnGetDeviceProperty())

				// success
				bResult = TRUE;
			}
			else
			{
				if	(fnGetDeviceProperty(	hDeviceInfoListIn,
											pDeviceElementIn,
											SPDRP_LOCATION_INFORMATION,
											lDimErrorIn,
											sPciLocOut,
											sErrorOut))
					// getting PciLoc ok
				{
					if	(	(pBusOut == NULL)
						||	(pDeviceOut == NULL))
					{
						// success
						bResult = TRUE;
					}
					else
					{
						if	(fnGetDeviceProperty(	hDeviceInfoListIn,
													pDeviceElementIn,
													SPDRP_BUSNUMBER,
													lDimErrorIn,
													(_TCHAR*) pBusOut,
													sErrorOut))
							// getting Bus ok
						{
						UINT32 lAddrTmp = 0;
					
							if	(fnGetDeviceProperty(	hDeviceInfoListIn,
														pDeviceElementIn,
														SPDRP_ADDRESS,
														lDimErrorIn,
														(_TCHAR*) pDeviceOut,
														sErrorOut))
								// getting Addr ok
							{
								lAddrTmp		= *pDeviceOut;
								*pDeviceOut 	= (lAddrTmp >> 16) & 0xFFFF;
								*pFunctionOut	= lAddrTmp & 0xFFFF;
							
								// success
								bResult = TRUE;
							}
						}
					}
				}
			}
		}
	}

	return(bResult);
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
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	get board type
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
ePNDEV_BOARD	clsDataSet::fnGetBoardType(	const _TCHAR*		sHardwareIdIn,
								UINT16*				pVendorIdOut,
								UINT16*				pDeviceIdOut)
{
ePNDEV_BOARD eResult = ePNDEV_BOARD_INVALID;

	if	(clsString::fnSearchSubString(	PNDEV_HW_ID_CP1616,
										sHardwareIdIn,
										TRUE))
		// CP1616 board
	{
		eResult 		= ePNDEV_BOARD_CP1616;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x4036;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_DB_EB400_PCIE,
											sHardwareIdIn,
											TRUE))
		// DB-EB400-PCIe board
	{
		eResult 		= ePNDEV_BOARD_DB_EB400_PCIE;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x408c;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_EB200,
											sHardwareIdIn,
											TRUE))
		// EB200 board
	{
		eResult			= ePNDEV_BOARD_EB200;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x4039;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_DB_EB200_PCIE,
											sHardwareIdIn,
											TRUE))
		// EB200 board
	{
		eResult			= ePNDEV_BOARD_DB_EB200_PCIE;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x409a;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_DB_SOC1_PCI,
											sHardwareIdIn,
											TRUE))
		// DB-Soc1-PCI board
	{
		eResult 		= ePNDEV_BOARD_DB_SOC1_PCI;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x403e;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_DB_SOC1_PCIE,
											sHardwareIdIn,
											TRUE))
		// DB-Soc1-PCIe board
	{
		eResult 		= ePNDEV_BOARD_DB_SOC1_PCIE;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x408a;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_FPGA1_ERTEC200P,
											sHardwareIdIn,
											TRUE))
		// FPGA1-Ertec200P board
	{
		eResult			= ePNDEV_BOARD_FPGA1_ERTEC200P;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x406B;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_EB200P,
											sHardwareIdIn,
											TRUE))
		// EB200P board
	{
		eResult 		= ePNDEV_BOARD_EB200P;
		*pVendorIdOut 	= 0x110a;
		*pDeviceIdOut 	= 0x4073;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_I210,
											sHardwareIdIn,
											TRUE))
		// Intel I210 board
	{
		eResult 		= ePNDEV_BOARD_I210;
		*pVendorIdOut 	= 0x8086;
		*pDeviceIdOut 	= 0x1533;
	}
	else if (clsString::fnSearchSubString(	PNDEV_HW_ID_I210SFP,
											sHardwareIdIn,
											TRUE))
		// Intel I210SFP board
	{
		eResult = ePNDEV_BOARD_I210SFP;
		*pVendorIdOut = 0x8086;
		*pDeviceIdOut = 0x1536;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_I82574,
											sHardwareIdIn,
											TRUE))
		// Intel I82574 board
	{
		eResult 		= ePNDEV_BOARD_I82574;
		*pVendorIdOut 	= 0x8086;
		*pDeviceIdOut 	= 0x10D3;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_FPGA1_HERA,
											sHardwareIdIn,
											TRUE))
		// FPGA1-Hera board
	{
		eResult 		= ePNDEV_BOARD_FPGA1_HERA;
		*pVendorIdOut 	= 0x110A;
		*pDeviceIdOut 	= 0x4090;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_MICREL_KSZ8841,
											sHardwareIdIn,
											TRUE))
		// Micrel KSZ8841 board
	{
		eResult 		= ePNDEV_BOARD_MICREL_KSZ8841;
		*pVendorIdOut 	= 0x16C6;
		*pDeviceIdOut 	= 0x8841;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_MICREL_KSZ8842,
											sHardwareIdIn,
											TRUE))
		// Micrel KSZ8842 board
	{
		eResult 		= ePNDEV_BOARD_MICREL_KSZ8842;
		*pVendorIdOut 	= 0x16C6;
		*pDeviceIdOut 	= 0x8842;
	}
	else if	(clsString::fnSearchSubString(	PNDEV_HW_ID_TI_AM5728,
											sHardwareIdIn,
											TRUE))
		// TI AM5728 board
	{
		eResult 		= ePNDEV_BOARD_TI_AM5728;
		*pVendorIdOut 	= 0x104C;
		*pDeviceIdOut 	= 0x8888;
	}
	else if (clsString::fnSearchSubString(	PNDEV_HW_ID_CP1625,
											sHardwareIdIn,
											TRUE))
		// CP1625 board
	{
		eResult = ePNDEV_BOARD_CP1625;
		*pVendorIdOut = 0x110a;
		*pDeviceIdOut = 0x4060;
	}

	return(eResult);
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
//------------------------------------------------------------------------
#endif

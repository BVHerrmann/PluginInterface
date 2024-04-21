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
/*  F i l e               &F: PnDev_Service.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Services of PnDev_Driver
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//  Function prototypes
//########################################################################

//------------------------------------------------------------------------
// os_Main.cpp
//------------------------------------------------------------------------
void fnSetDriverHandleDetach(HANDLE hDriverIn);

//************************************************************************
//  D e s c r i p t i o n :
//
//	open PnDev_Driver
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_OpenDriver(	const UINT32	lDimErrorIn,
													HANDLE*			pHandleDriverOut,
													_TCHAR*			sErrorOut)
{
ePNDEV_RESULT		eResult		= ePNDEV_FATAL;
uPNDEV_DRIVER_DESC*	hDriverDll	= NULL;
clsDataSet*			pDataSet	= NULL;

	// preset OutputParameter
	{
		*pHandleDriverOut = NULL;

		_tcscpy_s(	sErrorOut,
					lDimErrorIn,
					_TEXT(""));
	}

	// allocate/preset DllDataSet
	pDataSet = new clsDataSet(	lDimErrorIn,
								sErrorOut);

	if	(pDataSet == NULL)
		// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Insufficient memory for DllDataSet!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else if	(_tcscmp(sErrorOut, _TEXT("")) != 0)
			// error
	{
		// ErrorString set by constructor of clsDataSet
	}
	else
	{
		// dummy instruction because of Microsoft warning C6211
		*pHandleDriverOut	= pDataSet;
		*pHandleDriverOut	= NULL;

		if	(pDataSet->fnStartUpSys(lDimErrorIn,
									sErrorOut))
			// starting up SYS ok
		{
			if	(pDataSet->fnStartUpDll(lDimErrorIn,
										sErrorOut))
				// starting up DLL ok
			{
				// allocate DriverDescBlock
				hDriverDll = new uPNDEV_DRIVER_DESC;

				if	(hDriverDll == NULL)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Insufficient memory for DriverDescBlock!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
				LSA_UINT16 lResultVersion = 0;

					// preset
					memset(	hDriverDll,
							0,
							sizeof(uPNDEV_DRIVER_DESC));

					// get version of PnDev_Driver
					lResultVersion = fnPnDev_GetVersion(sizeof(LSA_VERSION_TYPE),
														&hDriverDll->uVersion);

					if	(lResultVersion != 0)
						// error
					{
						// set ErrorString
						_tcscpy_s(	sErrorOut, lDimErrorIn, _TEXT(__FUNCTION__));
						_tcsncat_s(	sErrorOut, lDimErrorIn, _TEXT("(): Error at getting version!"), _TRUNCATE);
					}
					else
					{
						// success
						eResult = ePNDEV_OK;

						// set DriverVerifier
						pDataSet->fnSetDriverVerifier(hDriverDll);

						// set DriverHandle for DllDetach
						fnSetDriverHandleDetach(hDriverDll);

						// return
						hDriverDll->pDataSet	= pDataSet;
						*pHandleDriverOut		= hDriverDll;
					}
				}
			}
		}
	}

	if	(eResult != ePNDEV_OK)
		// error
	{
		// free DllDataSet
		delete pDataSet;
		pDataSet = NULL;

		// free DriverDescBlock
		delete hDriverDll;
		hDriverDll = NULL;
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	close PnDev_Driver
//	- Windows:	called automatically at detach of DLL
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_CloseDriver(	HANDLE			hDriverIn,
													const UINT32	lDimErrorIn,
													_TCHAR*			sErrorOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(pDataSet->fnGetDriverVerifier() != hDriverIn)
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
		if	(pDataSet->fnShutDownDll(	hDriverIn,
										lDimErrorIn,
										sErrorOut))
			// shutting down DLL ok
		{
			if	(pDataSet->fnShutDownSys(	lDimErrorIn,
											sErrorOut))
				// shutting down SYS ok
			{
				// success
				eResult = ePNDEV_OK;

				// reset DriverHandle for DllDetach
				fnSetDriverHandleDetach(NULL);
			}
		}

		// clear FctPtr
		memset(	&((uPNDEV_DRIVER_DESC*) hDriverIn)->uIf,
				0,
				sizeof(uPNDEV_FCT_PTR));

		// free DllDataSet
		delete pDataSet;
		pDataSet = NULL;

		// free DriverDescBlock
		delete ((uPNDEV_DRIVER_DESC*) hDriverIn);
		hDriverIn = NULL;
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  fnPnDev_GetVersion(): see pndevdrv_ver.c
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  check version of PnDev_DriverU.h (used by application)
//	and installed driver (Windows: DLL, SYS)
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_CheckVersion(HANDLE			hDriverIn,
													const UINT32	lVersionIn,
													const UINT32	lDistributionIn,
													const UINT32	lIncrementIn,
													const UINT32	lIntegrationCtrIn,
													const UINT32	lDimErrorIn,
													_TCHAR*			sErrorOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;

	// preset OutputParameter
	_tcscpy_s(	sErrorOut,
				lDimErrorIn,
				_TEXT(""));

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(pDataSet->fnGetDriverVerifier() != hDriverIn)
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
		if	(	(lVersionIn			!= PNDEVDRV_VERSION)
			||	(lDistributionIn	!= PNDEVDRV_DISTRIBUTION))
			// version/distribution of used UI-HeaderFile do not match driver
		{
		_TCHAR	sVersionHeaderFile[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
		_TCHAR	sVersionDriver    [PNDEV_SIZE_STRING_BUF_SHORT]	= {0};

			// set VersionString of HeaderFile and driver
			//	- this C-function is placed at UserMode component of driver (Windows: DLL) -> versions of HeaderFile and this component are different
			{
				_stprintf_s(sVersionHeaderFile,	_countof(sVersionHeaderFile),	_TEXT("PnDev_DriverU.h: %u.%u.%u.%u\n"),	lVersionIn,
																															lDistributionIn,
																															lIncrementIn,
																															lIntegrationCtrIn);
				_stprintf_s(sVersionDriver,		_countof(sVersionDriver),		_TEXT("PnDev_DriverU:   %d.%d.%d.%d"),		PNDEVDRV_VERSION,
																															PNDEVDRV_DISTRIBUTION,
																															PNDEVDRV_INCREMENT,
																															PNDEVDRV_INTEGRATION_COUNTER);
			}

			// set ErrorString
			clsString::fnBuildString(	_TEXT("Version mismatch of PnDev_DriverU.h (used by application) and installed PnDev_Driver:\n"),
										sVersionHeaderFile,
										sVersionDriver,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(clsDataSet::fnCheckVersionDetails(	lDimErrorIn,
													sErrorOut))
				// further version details ok
			{
				// success
				eResult = ePNDEV_OK;
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get DeviceInfo of all devices
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_GetDeviceInfo(	HANDLE						hDriverIn,
														const UINT32				lSizeInputBufIn,
														const UINT32				lSizeOutputBufIn,	// size of complete OutputArray
														const UINT32				lDimOutputArrayIn,
														const UINT32				lDimErrorIn,
														uPNDEV_GET_DEVICE_INFO_IN*	pIn,
														uPNDEV_GET_DEVICE_INFO_OUT	uArrayOut[],		// array=ptr
														UINT32*						pCtrDeviceOut,
														_TCHAR*						sErrorOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;

	UNREFERENCED_PARAMETER(pIn);

	// structure of OutputParameter will be preset at fnDoEntryActions()

	// preset remaining OutputParameter
	{
		*pCtrDeviceOut = 0;

		_tcscpy_s(	sErrorOut,
					lDimErrorIn,
					_TEXT(""));
	}

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(pDataSet->fnGetDriverVerifier() != hDriverIn)
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
		if	(	(sErrorOut		== NULL)
			||	(lDimErrorIn	== 0))
			// error
		{
			// no detailed ErrorString possible
		}
		else
		{
		BOOLEAN bFatalBuf = FALSE;

			if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
												lSizeOutputBufIn,
												sizeof(uPNDEV_GET_DEVICE_INFO_IN),
												(sizeof(uPNDEV_GET_DEVICE_INFO_OUT) * lDimOutputArrayIn),
												TRUE,
												uArrayOut,
												hDriverIn,
												lDimErrorIn,
												&bFatalBuf,
												sErrorOut))
				// error at entry actions
			{
				if	(bFatalBuf)
					// FatalErrorBuf
				{
					eResult = ePNDEV_FATAL_BUF;
				}
			}
			else
			{
				if	(pDataSet->fnGetCtrDevice(	lDimErrorIn,
												pCtrDeviceOut,
												sErrorOut))
					// getting number of devices ok
				{
					if	(lDimOutputArrayIn == 0)
						// only number of available devices desired by user
					{
						// success
						eResult = ePNDEV_OK;
					}
					else if	(*pCtrDeviceOut > lDimOutputArrayIn)
							// OutputArray too small
					{
					_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

						eResult = ePNDEV_FATAL_BUF;

						// set ErrorDetail
						_stprintf_s(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT(" (Devices: %u, dimension: %u)"),
									*pCtrDeviceOut,
									lDimOutputArrayIn);

						// set ErrorString
						clsString::fnBuildString(	_TEXT("Dimension of output array too small!\n"),
													sErrorDetail,
													NULL,
													lDimErrorIn,
													sErrorOut);
					}
					else if	(*pCtrDeviceOut == 0)
							// no device found
					{
						eResult = ePNDEV_ERROR_NOT_FOUND;

						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): No device found!"),
													NULL,
													lDimErrorIn,
													sErrorOut);
					}
					else
					{
						if	(pDataSet->fnGetDeviceInfoAll(	lDimOutputArrayIn,
															lDimErrorIn,
															uArrayOut,		// array=ptr
															sErrorOut))
							// getting DeviceInfo of all devices ok
						{
							// success
							eResult = ePNDEV_OK;
						}
					}
				}

				//missing uDriverInfo
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  open device
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_OpenDevice(	HANDLE					hDriverIn,
													const UINT32			lSizeInputBufIn,
													const UINT32			lSizeOutputBufIn,
													uPNDEV_OPEN_DEVICE_IN*	pIn,
													uPNDEV_OPEN_DEVICE_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_OPEN_DEVICE_IN),
										sizeof(uPNDEV_OPEN_DEVICE_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
			if	(pIn->uCbfEvent.pPtr == NULL)
				// invalid CbfEvent
			{
				eResult = ePNDEV_ERROR_CBF;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): CBF_EVENT not set!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else if	(	(pIn->bMasterApp)
					&&	(pIn->uCbfIsr.pPtr == NULL))
					// invalid CbfIsr
			{
				eResult = ePNDEV_ERROR_CBF;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): CBF_ISR not set (Master App)!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else if	(pDataSet->fnDeviceInUse(pIn->sPciLoc))
					// device already in use (UsedDeviceList)
			{
				eResult = ePNDEV_ERROR_IN_USE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Device already opened by this application!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			clsUsedDevice* pUsedDevice = NULL;

				// allocate/preset UsedDeviceObject
				pUsedDevice = new clsUsedDevice();

				if	(pUsedDevice == NULL)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Insufficient memory for UsedDeviceObject!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
				BOOLEAN	        bErrorDeviceNotFound	= FALSE;
				uPNDEV64_HANDLE	hDeviceSys;
				    hDeviceSys.hHandle = NULL;

					// dummy instruction because of Microsoft warning C6211
					pIn->uListBoxCurAction.hHandle	= pUsedDevice;
					pIn->uListBoxCurAction.hHandle	= NULL;

					if	(!pDataSet->fnGetDeviceHandleSys(	pIn->sPciLoc,
															_countof(pOut->sError),
															&hDeviceSys,
															&bErrorDeviceNotFound,
															pOut->sError))
						// error at getting DeviceHandleSys
					{
						if (bErrorDeviceNotFound)
							// board with desired PciLoc not found
						{
							eResult = ePNDEV_ERROR_NOT_FOUND;

							// set ErrorString
							clsString::fnBuildString(	_TEXT(__FUNCTION__),
														_TEXT("(): No device with this PCI location found!"),
														NULL,
														_countof(pOut->sError),
														pOut->sError);
						}

						// free UsedDeviceObject
						delete pUsedDevice;
						pUsedDevice = NULL;
					}
					else
					{
					UINT32 lStackSize = 0;

						// Note:
						//	- now UsedDeviceObject exist
						//		-> service CloseDevice can be used if there is an error

						// set variables of UsedDeviceObject
						pUsedDevice->fnSetVarOpenDevice1(	hDriverIn,
															pDataSet,
															hDeviceSys,
															pIn);

						if	(pIn->uThreadDll.lStackSize == 0)
							// default value desired
						{
							// 16kByte (PTHREAD_STACK_MIN in Linux)
							lStackSize = (1024*16);
						}
						else
						{
							lStackSize = pIn->uThreadDll.lStackSize;
						}

						if	(pUsedDevice->fnStartUpThreadRes(	pIn->uThreadDll.lPrio,
																lStackSize,
																pIn->uThreadDll.sName,
																_countof(pOut->sError),
																pOut->sError))
							// starting up resources of ServiceThread / DllThread ok
						{
						BOOLEAN bResultOpen = FALSE;

							// set internal parameter for SYS
							pIn->uInternal.uThreadIoctl.uEventUinfo.hHandle		= pUsedDevice->fnGetHandleEventUinfo();
							pIn->uInternal.uThreadIoctl.uEventUinfoUm.hHandle	= pUsedDevice->fnGetHandleEventUinfoUm();
							pIn->uInternal.uThreadIoctl.uEventUisr.hHandle		= pUsedDevice->fnGetHandleEventUisr();
							pIn->uInternal.uThreadIoctl.uThreadDll.hHandle		= pUsedDevice->fnGetHandleThreadDll();

							if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
															IOCTL_OPEN_DEVICE,
															sizeof(uPNDEV_OPEN_DEVICE_IN),
															sizeof(uPNDEV_OPEN_DEVICE_OUT),
															pIn,
															pOut))
								// error at IoctlOpenDevice
							{
								if	(_tcscmp(pOut->sError, PNDEV_ERROR_MASTER_APP) == 0)
									// MasterApp already exist
								{
									eResult = ePNDEV_ERROR_MASTER_APP;

									// set ErrorString
									clsString::fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Master App already exist!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);
								}
								else
								{
									// ErrorString already set

									eResult = ePNDEV_FATAL;
								}
							}
							else
							{
							UINT32 i = 0;
							uPNDEV_MAP_MEMORY_IN	uMapMemoryIn;
							uPNDEV_MAP_MEMORY_OUT	uMapMemoryOut;

								// set InputParameter
								{
									memset(	&uMapMemoryIn,
											0,
											sizeof(uPNDEV_MAP_MEMORY_IN));

									memset(	&uMapMemoryOut,
											0,
											sizeof(uPNDEV_MAP_MEMORY_OUT));

									// set ptr to InputParameter
									for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
									{
										uMapMemoryIn.uBarKm[i] = pOut->uInternal.uBarKm[i];
									}

									uMapMemoryIn.uSdramKm 		= pOut->uInternal.uSdramKm;
									uMapMemoryIn.uHostMemKm 	= pOut->uInternal.uHostMemKm;
								}

								// map Memory to Usermode
								eResult = pUsedDevice->fnMapMemory( pUsedDevice,
																	&uMapMemoryIn,
																	&uMapMemoryOut);

								// set ptr to OutputParameter
								for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
								{
									pOut->uInternal.uBarUm[i]= uMapMemoryOut.uBarUm[i] ;
								}

								pOut->uInternal.uSdramUm	= uMapMemoryOut.uSdramUm;
								pOut->uInternal.uHostMemUm 	= uMapMemoryOut.uHostMemUm;

								if	(eResult != ePNDEV_OK)
									// error
								{
									// set error string to pOut
									_tcscpy_s(	pOut->sError,
												_countof(uMapMemoryOut.sError),
												uMapMemoryOut.sError);

									eResult = ePNDEV_FATAL;
								}
								else
								{
								uPNDEV_OPEN_DEVICE_OUT uOutTmp;

									// preset
									eResult = ePNDEV_FATAL;

									memset(	&uOutTmp,
											0,
											sizeof(uPNDEV_OPEN_DEVICE_OUT));

									if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
																	IOCTL_SET_HW_RES_UM,
																	sizeof(uPNDEV_OPEN_DEVICE_OUT),
																	sizeof(uPNDEV_OPEN_DEVICE_OUT),
																	pOut,
																	&uOutTmp))
										// error at IoctlOpenDevice
									{
										// ErrorString already set
										eResult = ePNDEV_FATAL;
										
										// copy sError
										memcpy_s(	pOut,
													sizeof(uPNDEV_OPEN_DEVICE_OUT),
													&uOutTmp,
													sizeof(uPNDEV_OPEN_DEVICE_OUT));
									}
									else
									{
									BOOLEAN bFirstOpenDevice = FALSE;

										// copy OutTmp to real pOut
										memcpy_s(	pOut,
													sizeof(uPNDEV_OPEN_DEVICE_OUT),
													&uOutTmp,
													sizeof(uPNDEV_OPEN_DEVICE_OUT));

										// update StateOpenDevice
										pUsedDevice->m_lStateOpenDevice |= PNDEV_DEVICE_STATE__IOCTL_OPEN_DEVICE_DONE;

										// return
										//	- must be after IoctlOpenDevice because OutputParameter are preset!
										{
											_tcscpy_s(	pOut->sPciLoc,
														_countof(pOut->sPciLoc),
														pIn->sPciLoc);

											pOut->bMasterApp			= pIn->bMasterApp;
											pOut->lCtrFw				= pIn->uUserFw.lCtrFw;
										#if (PNDEV_PLATFORM == PNDEV_UM_PLATFORM_64) //BU_loses_precision
											//64bit
											pOut->uThreadInfo.lIdDll    = (UINT32) (UINT64) pUsedDevice->fnGetHandleThreadDll();
										#else
											//32bit
											pOut->uThreadInfo.lIdDll    = (UINT32) pUsedDevice->fnGetHandleThreadDll();
										#endif
										}

										// store info about first OpenDevice
										bFirstOpenDevice = pOut->uInternal.bFirstOpenDevice;

										if	(pUsedDevice->fnSetVarOpenDevice2(pOut))
											// setting variables of UsedDeviceObject ok
										{
											// update UsedDeviceList
											{
												// put UsedDeviceObject to UsedDeviceList
												fnPutBlkToList(	pDataSet->fnGetPtrListUsedDevice(),
																((uLIST_HEADER*) pUsedDevice));

												// increment CtrDevice
												pDataSet->fnIncrCtrUsedDevice();
											}

											// update StateOpenDevice
											//	- e.g. pUsedDevice->m_uAppSys is needed for CloseDevice
											pUsedDevice->m_lStateOpenDevice |= PNDEV_DEVICE_STATE__ADD_LIST_DONE;

											if	(!bFirstOpenDevice)
												// not first OpenDevice
											{
												// success
												bResultOpen = TRUE;
											}
											else
											{
												if	(pUsedDevice->fnVerifyEnvFirstOpen(	pIn->bVerifyEnvironment,
																						_countof(pOut->sError),
																						pOut->sError))
													// verifying environment at first OpenDevice ok
												{
												BOOLEAN bResultPhy = FALSE;

													// Note:
													//	- first OpenDevice: OpenDevice is locked by SYS for verifying environment and starting Fw at DLL!

													// preset
													bResultPhy = TRUE;

													if	(pIn->bInitPhy)
														// initialize PHY connection
													{
													BOOLEAN bInitPhy		= FALSE;
													BOOLEAN bPhyClockExt	= FALSE;

														switch	(pOut->uHwInfo.eBoardDetail)
																// BoardDetail
														{
															case ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_EXT:	{bInitPhy = TRUE;	bPhyClockExt = TRUE;	break;}
															case ePNDEV_BOARD_DETAIL_SOC1_PHY_CLOCK_INT:	{bInitPhy = TRUE;	bPhyClockExt = FALSE;	break;}
															default:
															{
																break;
															}
														}

														if	(bInitPhy)
															// initialize PHY connection of Soc1
														{
															if	(!fnDrvUtil_InitPhySoc1(pOut->uHwRes.as.uSoc1.uApbPer.uGpio.uBase.pPtr,
																						pOut->uHwRes.as.uSoc1.uApbPer.uScrb.uBase.pPtr,
																						bPhyClockExt,
																						_countof(pOut->sError),
																						pOut->sError))
																// error at initialization of PHY connection of Soc1
															{
																bResultPhy = FALSE;
															}
														}
													}

													if	(bResultPhy)
														// success
													{
														if	(pUsedDevice->fnStartFw(pIn,
																					pOut,
																					_countof(pOut->sError),
																					pOut->sError))
															// starting Fw ok
														{
															// Note:
															//	- only if first OpenDevice is completely done then events may be received by DLL
															//	- there may be problems if a CloseDevice would follow

															if	(pUsedDevice->fnNotifyDllState(	TRUE,					// ready
																								_countof(pOut->sError),
																								pOut->sError))
																// notifying DllState ok

															{
																// success
																bResultOpen = TRUE;
															}
														}
													}
												}
											}
										}
									}
								}
							}

							if	(!bResultOpen)
								// error
							{
							ePNDEV_RESULT			eResultTmp = ePNDEV_FATAL;
							uPNDEV_CLOSE_DEVICE_IN	uCloseDeviceIn;
							uPNDEV_CLOSE_DEVICE_OUT	uCloseDeviceOut;

								// set InputParameter
								{
									memset(	&uCloseDeviceIn,
											0,
											sizeof(uPNDEV_CLOSE_DEVICE_IN));

									uCloseDeviceIn.uDevice.hHandle = pUsedDevice;

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
								if(eResultTmp != ePNDEV_OK)
								    // error
								{
								    eResult = eResultTmp;

                                    // set ErrorString
                                    clsString::fnBuildString(   _TEXT(__FUNCTION__),
                                                                _TEXT("(): Error Closing Device!"),
                                                                NULL,
                                                                _countof(pOut->sError),
                                                                pOut->sError);
								}
								else
								{
									if	(!pUsedDevice->fnUnmapMemory(	(uPNDEV_HW_RES_SINGLE_UM*) &pOut->uInternal.uBarUm[0],
																		&pOut->uInternal.uSdramUm,
																		&pOut->uInternal.uHostMemUm))
										// error
									{
									    eResult = ePNDEV_FATAL;

									    // set ErrorString
									    clsString::fnBuildString(   _TEXT(__FUNCTION__),
									                                _TEXT("(): Error at Release Memory after CloseDevice!"),
									                                NULL,
									                                _countof(pOut->sError),
									                                pOut->sError);
									}
								}
							}
							else
							{
								// success
								eResult = ePNDEV_OK;

								// return
								//	- must be after IoctlOpenDevice because OutputParameter are preset!
								pOut->uDevice.hHandle = pUsedDevice;
							}
						}
					}
				}
			}

			// reset
			{
				memset(	&pIn->uInternal,
						0,
						sizeof(uPNDEV_OPEN_DEVICE_INTERNAL_IN));

				memset(	&pOut->uInternal,
						0,
						sizeof(uPNDEV_OPEN_DEVICE_INTERNAL_OUT));
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get device state
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_GetDeviceState(	HANDLE							hDriverIn,
														const UINT32					lSizeInputBufIn,
														const UINT32					lSizeOutputBufIn,
														uPNDEV_GET_DEVICE_STATE_IN*		pIn,
														uPNDEV_GET_DEVICE_STATE_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_GET_DEVICE_STATE_IN),
										sizeof(uPNDEV_GET_DEVICE_STATE_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			UINT32 lCtrUisrTmp = 0;

				// set internal parameter for SYS
				//	- use 64Bit value because DriverHandle may be a 64Bit handle!
				pIn->uInternal.uAppSys.lUint64 = pUsedDevice->fnGetHandleAppSys();

				// get ctr before getting corresponding ctr of driver for ensuring that ctr of driver >= ctr of DLL
				lCtrUisrTmp = pUsedDevice->fnGetCtrUisr();

				if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
												IOCTL_GET_DEVICE_STATE,
												sizeof(uPNDEV_GET_DEVICE_STATE_IN),
												sizeof(uPNDEV_GET_DEVICE_STATE_OUT),
												pIn,
												pOut))
					// error at IoctlGetDeviceState
				{
					eResult = ePNDEV_FATAL;

					// ErrorString already set
				}
				else
				{
					// success
					eResult = ePNDEV_OK;

					// update OutputParameter of DLL
					//	- must be after calling driver because OutputParameter are cleared at driver!
					{
						pOut->uIntStatistic.lCtrUisr = lCtrUisrTmp;

						// updated by service -> already locked
						_tcscpy_s(	pOut->sPciLoc,
									_countof(pOut->sPciLoc),
									pUsedDevice->fnGetPciLoc());
					}
				}
			}

			// reset
			memset(	&pIn->uInternal,
					0,
					sizeof(uPNDEV_GET_DEVICE_STATE_INTERNAL_IN));
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

#if defined (PNDEV_OS_ADONIS)
//************************************************************************
//  D e s c r i p t i o n :
//
//  get shared host mem addr
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_GetSharedHostMemAddr(HANDLE                          hDriverIn,
															const UINT32                    lSizeInputBufIn,
															const UINT32                    lSizeOutputBufIn,
															uPNDEV_GET_SHARED_HOST_MEM_ADDR_IN*     pIn,
															uPNDEV_GET_SHARED_HOST_MEM_ADDR_OUT*    pOut)
{
ePNDEV_RESULT   eResult     = ePNDEV_FATAL;
clsDataSet*     pDataSet    = NULL;
BOOLEAN         bFatalBuf   = FALSE;

    // fnDoEntryActions():
    //  - structure of OutputParameter is preset
    //  - DriverHandle is verified

    pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

    if  (!pDataSet->fnDoEntryActions(   lSizeInputBufIn,
                                        lSizeOutputBufIn,
                                        sizeof(uPNDEV_GET_SHARED_HOST_MEM_ADDR_IN),
                                        sizeof(uPNDEV_GET_SHARED_HOST_MEM_ADDR_OUT),
                                        TRUE,
                                        pOut,
                                        hDriverIn,
                                        _countof(pOut->sError),
                                        &bFatalBuf,
                                        pOut->sError))
        // error at entry actions
    {
        if  (bFatalBuf)
            // FatalErrorBuf
        {
            eResult = ePNDEV_FATAL_BUF;
        }
    }
    else
    {
    BOOLEAN bResultEnter    = FALSE;
    BOOLEAN bResultLeave    = FALSE;

        //------------------------------------------------------------------------
        // enter CriticalSection
        //  - prevent simultaneous calling of services at different threads
        //  - avoid problems if same device is referenced or UsedDeviceList is used
        bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
                                                _countof(pOut->sError),
                                                pOut->sError);

        if  (!pDataSet->fnDoDeviceIoctl(NULL,
                                        IOCTL_GET_SHARED_HOST_MEM_ADDR,
                                        sizeof(uPNDEV_GET_SHARED_HOST_MEM_ADDR_IN),
                                        sizeof(uPNDEV_GET_SHARED_HOST_MEM_ADDR_OUT),
                                        pIn,
                                        pOut))
            // error at Ioctl
        {
            eResult = ePNDEV_FATAL;

            // ErrorString already set
        }
        else
        {
            // success
            eResult = ePNDEV_OK;
        }

        // reset
        memset( &pIn->uInternal,
                0,
                sizeof(uPNDEV_GET_SHARED_HOST_MEM_ADDR_INTERNAL_IN));

        
        //------------------------------------------------------------------------
        // leave CriticalSection
        bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
                                                _countof(pOut->sError),
                                                pOut->sError);

        if  (   !bResultEnter
            ||  !bResultLeave)
            // error
        {
            eResult = ePNDEV_FATAL;
        }
    }

    return(eResult);
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable/disable interrupts source(s)
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_ChangeInt(	HANDLE					hDriverIn,
													const UINT32			lSizeInputBufIn,
													const UINT32			lSizeOutputBufIn,
													uPNDEV_CHANGE_INT_IN*	pIn,
													uPNDEV_CHANGE_INT_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_CHANGE_INT_IN),
										sizeof(uPNDEV_CHANGE_INT_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				if	(!pUsedDevice->fnGetMasterApp())
					// not MasterApp
				{
					eResult = ePNDEV_ERROR_MASTER_APP;

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Service not supported for a NonMaster App!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else if	(	(pIn->eIntAction != ePNDEV_INT_ACTION_UNMASK)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_MASK_ALL)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__ONCE_UISR)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__CONT_UISR)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__CONT_DPC)
						&&	(pIn->eIntAction != ePNDEV_INT_ACTION_TEST__CONT_ISR))
						// invalid IntAction
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid IntAction!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
				BOOLEAN bResultIntTest = FALSE;

					// preset
					bResultIntTest = TRUE;

					// mask all interrupts at ICU
					//	- consistency of IntAction at driver and DllThread must be ensured!
					pUsedDevice->fnMaskIcu_All(TRUE);	// initialization

					// update IntAction
					//	- must be before pIn->eIntAction may be changed!
					pUsedDevice->fnSetIntAction(pIn->eIntAction);

					if	(pUsedDevice->fnGetIntAction() >= PNDEV_INT_ACTION_TEST_MIN)
						// current IntAction=Test
					{
						// stop IntTest
						pUsedDevice->fnStopIntTest();

						if	(pIn->eIntAction == ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS)
						{
							if	(!pUsedDevice->fnSetEventList(	ePNDEV_EVENT_CLASS_FATAL_ASYNC,
																ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS,
																_countof(pOut->sError),
																pOut->sError))
								// error at simulating FatalAsync
							{
								bResultIntTest = FALSE;
							}
						}
					}

					if	(bResultIntTest)
						// success
					{
						// check unmasking of IRTE/PNIP interrupt if there is no TopLevel-ICU
						{
							switch	(pUsedDevice->fnGetTypeIcu())
									// IcuType
							{
								case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
								{
									if	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTECX00__IRTE_IRQ1])
										// interrupts of IRTE-XIRQ1_HP enabled
									{
										// TopLevel-ICU implemented by software
										pUsedDevice->fnSetIntEnabledIrtePnipIntel(TRUE);
									}

									break;
								}
								case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
								{
									if	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0])
										// interrupts of IRQ0 (combined interrupt) of PN-ICU2 enabled
									{
										// TopLevel-ICU implemented by software
										pUsedDevice->fnSetIntEnabledIrtePnipIntel(TRUE);
									}

									break;
								}
								case ePNDEV_ICU_INTEL:	// no TopLevel-ICU exist
								{
									if	(	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_I210])
										||	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_I210SFP])
										||	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_I82574]))
										// interrupts enabled
									{
										// TopLevel-ICU implemented by software
										pUsedDevice->fnSetIntEnabledIrtePnipIntel(TRUE);
									}

									break;
								}
								case ePNDEV_ICU_IX1000:	// no TopLevel-ICU exist
								{
									if	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_IX1000])
										// interrupts enabled
									{
										// TopLevel-ICU implemented by software
										pUsedDevice->fnSetIntEnabledIrtePnipIntel(TRUE);
									}

									break;
								}
								case ePNDEV_ICU_MICREL:	// no TopLevel-ICU exist
								{
									if	(	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8841])
										||	(pIn->bArrayIntUnmask[ePNDEV_INT_SRC_MICREL_KSZ8842]))
										// interrupts enabled
									{
										// TopLevel-ICU implemented by software
										pUsedDevice->fnSetIntEnabledIrtePnipIntel(TRUE);
									}

									break;
								}
								default:
								{
									break;
								}
							}
						}

						// reset IntStatistic
						pUsedDevice->fnSetCtrUisr(0);

						// set internal parameter for SYS
						//	- use 64Bit value because DriverHandle may be a 64Bit handle!
						pIn->uInternal.uAppSys.lUint64 = pUsedDevice->fnGetHandleAppSys();

						if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
														IOCTL_CHANGE_INT,
														sizeof(uPNDEV_CHANGE_INT_IN),
														sizeof(uPNDEV_CHANGE_INT_OUT),
														pIn,
														pOut))
							// error at IoctlChangeInt
						{
							eResult = ePNDEV_FATAL;

							// ErrorString already set
						}
						else
						{
						BOOLEAN bResultPrio = TRUE;

							if	(pIn->lPrioThread != PNDEV_THREAD_PRIO_UNCHANGED)
								// ThreadPrio should be changed
							{
								if	(!fnSetThreadPrio(	pUsedDevice->fnGetHandleThreadDll(),
														pIn->lPrioThread,
														PNDEV_THREAD_POLICY,
														_countof(pOut->sError),
														pOut->sError))
									// error at setting ThreadPrio
								{
									bResultPrio = FALSE;
								}
							}

							if	(bResultPrio)
								// success
							{
								if	(pUsedDevice->fnGetIntAction() == ePNDEV_INT_ACTION_MASK_ALL)
									// mask all interrupts
								{
									if	(pUsedDevice->fnGetTypeIcu() == ePNDEV_ICU_HERA)
									{
										// mask all interrupts at ICU
										//	 - special case for deletion of shadowBackupArray (m_bEventEnable)
										pUsedDevice->fnMaskIcu_All(FALSE);	// no initialization
									}

									// success
									eResult = ePNDEV_OK;
								}
								else
								{
									if	(pUsedDevice->fnUnmaskIcu_Desired(	pIn,
																			_countof(pOut->sError),
																			pOut->sError))
										// unmasking desired interrupts at TopLevel-ICU ok
									{
										// success
										eResult = ePNDEV_OK;
									}
								}
							}
						}
					}
				}
			}

			// reset
			memset(	&pIn->uInternal,
					0,
					sizeof(uPNDEV_CHANGE_INT_INTERNAL_IN));
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  program flash
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_ProgramFlash(HANDLE						hDriverIn,
													const UINT32				lSizeInputBufIn,
													const UINT32				lSizeOutputBufIn,
													uPNDEV_PROGRAM_FLASH_IN*	pIn,
													uPNDEV_PROGRAM_FLASH_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_PROGRAM_FLASH_IN),
										sizeof(uPNDEV_PROGRAM_FLASH_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			BOOLEAN bResultSbl = TRUE;

				if	(pIn->eFlashAction == ePNDEV_FLASH_ACTION_PROGRAM_SBL)
					// ProgramSbl
				{
				HANDLE hFileFw = NULL;

					// preset
					bResultSbl = FALSE;

					if	(pIn->as.uSbl.eSrc == ePNDEV_FW_SRC_BUF)
						// Fw by HostBuf
					{
						// Fw already at HostBuf

						// success
						bResultSbl = TRUE;
					}
					else	// Fw by file
					{
					UINT32 lSizeFw = 0;

						if	(clsFile::fnOpenFileForRead(pIn->as.uSbl.uFile.sPath,
														_countof(pOut->sError),
														&hFileFw,
														pOut->sError))
							// opening FwFile for reading ok
						{
							if	(clsFile::fnGetFileSize(hFileFw,
														_countof(pOut->sError),
														&lSizeFw,
														pOut->sError))
								// getting size of FwFile ok
							{
								if	(lSizeFw == 0)
									// error
								{
									// set ErrorString
									clsString::fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Invalid FileSize!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);
								}
								else
								{
									if	(lSizeFw > PNDEV_DIM_ARRAY_SBL)
										// Fw too large for HostBuf
									{
										// set ErrorString
										clsString::fnBuildString(	_TEXT(__FUNCTION__),
																	_TEXT("(): Fw too large for HostBuf!"),
																	NULL,
																	_countof(pOut->sError),
																	pOut->sError);
									}
									else
									{
										// clear HostBuf
										memset(	pIn->as.uSbl.lArrayFw,
												0,
												PNDEV_DIM_ARRAY_SBL);

										if	(clsFile::fnReadFile(	hFileFw,
																	pIn->as.uSbl.lArrayFw,
																	lSizeFw,
																	_countof(pOut->sError),
																	pOut->sError))
											// copying FwFile to HostBuf ok
										{
											// success
											bResultSbl = TRUE;
										}
									}
								}
							}
						}
					}
				}
				else if (pIn->eFlashAction == ePNDEV_FLASH_ACTION_PROGRAM_CP1625)
					// we need to access a bigger file
				{
					// kernel will open the file itself - nothing to do here
				}

				if	(bResultSbl)
					// success
				{
					// set internal parameter for SYS
					//	- use 64Bit value because DriverHandle may be a 64Bit handle!
					pIn->uInternal.uAppSys.lUint64 = pUsedDevice->fnGetHandleAppSys();

					if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
													IOCTL_PROGRAM_FLASH,
													sizeof(uPNDEV_PROGRAM_FLASH_IN),
													sizeof(uPNDEV_PROGRAM_FLASH_OUT),
													pIn,
													pOut))
						// error at IoctlProgramFlash
					{
						eResult = ePNDEV_FATAL;

						// ErrorString already set
					}
					else
					{
						// success
						eResult = ePNDEV_OK;
					}
				}

				// reset
				memset(	&pIn->uInternal,
						0,
						sizeof(uPNDEV_PROGRAM_FLASH_INTERNAL_IN));
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  change XHIF page
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_ChangeXhifPage(	HANDLE							hDriverIn,
														const UINT32					lSizeInputBufIn,
														const UINT32					lSizeOutputBufIn,
														uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
														uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_CHANGE_XHIF_PAGE_IN),
										sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				if	(	(pIn->ePage != ePNDEV_XHIF_PAGE_INVALID)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__SD_MMC)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__ARM926_TCM)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__EMC_SDRAM)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK0)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK1)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK2)
					&&	(pIn->ePage != ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK3))
					// invalid XhifPage
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid XHIF page!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
					//	SD-MMC:			PhySize=1MB
					//	EMC SDRAM:		PhySize=256MB, first AHB segment (0..2MB) used by static page
					//	EMC AsyncBank0:	PhySize=64MB
					//	EMC AsyncBank1:	PhySize=64MB
					//	EMC AsyncBank2:	PhySize=64MB
					//	EMC AsyncBank3:	PhySize=64MB

					if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__SD_MMC)
						&&	(pIn->lSegmentAhb	!= 0))
						// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (SD-MMC size = 1MB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__ARM926_TCM)
							&&	(pIn->lSegmentAhb	!= 0))
						// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (ARM926-TCM size = 256kB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage == ePNDEV_XHIF_PAGE_ERTEC200P__EMC_SDRAM)
							&&	(	(pIn->lSegmentAhb	< 1)
								||	(pIn->lSegmentAhb	> 127)))
							// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (EMC SDRAM size = 256MB, first segment in use)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK0)
							&&	(pIn->lSegmentAhb	> 31))
							// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (EMC AsyncBank0 size = 64MB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK1)
							&&	(pIn->lSegmentAhb	> 31))
							// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (EMC AsyncBank1 size = 64MB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK2)
							&&	(pIn->lSegmentAhb	> 31))
							// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (EMC AsyncBank2 size = 64MB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else if	(	(pIn->ePage			== ePNDEV_XHIF_PAGE_ERTEC200P__EMC_ASYNC_BANK3)
							&&	(pIn->lSegmentAhb	> 31))
							// invalid AHB segment
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid AHB segment (EMC AsyncBank3 size = 64MB)!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else
					{
						// set internal parameter for SYS
						//	- use 64Bit value because DriverHandle may be a 64Bit handle!
						pIn->uInternal.uAppSys.lUint64 = pUsedDevice->fnGetHandleAppSys();

						if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
														IOCTL_CHANGE_XHIF_PAGE,
														sizeof(uPNDEV_CHANGE_XHIF_PAGE_IN),
														sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT),
														pIn,
														pOut))
							// error at IoctlChangeXhifPage
						{
							if	(_tcscmp(pOut->sError, PNDEV_ERROR_SUPPORT) == 0)
								// XHIF pages not supported
							{
								eResult = ePNDEV_ERROR_NO_SUPPORT;

								// set ErrorString
								clsString::fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): No hardware support!"),
															NULL,
															_countof(pOut->sError),
															pOut->sError);
							}
							else if	(_tcscmp(pOut->sError, PNDEV_ERROR_IN_USE) == 0)
									// page already in use
							{
								eResult = ePNDEV_ERROR_IN_USE;

								// set ErrorString
								clsString::fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Dynamic XHIF page used by other applications!"),
															NULL,
															_countof(pOut->sError),
															pOut->sError);
							}
							else
							{
								eResult = ePNDEV_FATAL;

								// ErrorString already set
							}
						}
						else
						{
							// success
							eResult = ePNDEV_OK;
						}
					}
				}
			}

			// reset
			memset(	&pIn->uInternal,
					0,
					sizeof(uPNDEV_CHANGE_XHIF_PAGE_INTERNAL_IN));
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  compare PciLocation
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_ComparePciLoc(	HANDLE						hDriverIn,
														const UINT32				lSizeInputBufIn,
														const UINT32				lSizeOutputBufIn,
														uPNDEV_COMPARE_PCI_LOC_IN*	pIn,
														uPNDEV_COMPARE_PCI_LOC_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_COMPARE_PCI_LOC_IN),
										sizeof(uPNDEV_COMPARE_PCI_LOC_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	_TCHAR	sPciBusNo     [PNDEV_SIZE_STRING_BUF_SHORT] = {0};
	_TCHAR	sPciDeviceNo  [PNDEV_SIZE_STRING_BUF_SHORT] = {0};
	_TCHAR	sPciFunctionNo[PNDEV_SIZE_STRING_BUF_SHORT] = {0};
	_TCHAR*	sFirstOccur	= NULL;
	_TCHAR*	sTmp		= NULL;

		// build strings " xxNo," or " xxNo"
		{
			_tcscpy_s(sPciBusNo,		_countof(sPciBusNo),		_TEXT(" "));
			_tcscpy_s(sPciDeviceNo,		_countof(sPciDeviceNo),		_TEXT(" "));
			_tcscpy_s(sPciFunctionNo,	_countof(sPciFunctionNo),	_TEXT(" "));

			// set PciStrings
			//_ltot_s(pIn->lPciBusNo,		&sPciBusNo[1],		(_countof(sPciBusNo)-1),		10);
			//_ltot_s(pIn->lPciDeviceNo,	&sPciDeviceNo[1],	(_countof(sPciDeviceNo)-1),		10);
			//_ltot_s(pIn->lPciFunctionNo,	&sPciFunctionNo[1],	(_countof(sPciFunctionNo)-1),	10);
			_stprintf_s(&sPciBusNo[1],		(_countof(sPciBusNo)-1),		_TEXT("%u"), pIn->lPciBusNo);
			_stprintf_s(&sPciDeviceNo[1],	(_countof(sPciDeviceNo)-1),		_TEXT("%u"), pIn->lPciDeviceNo);
			_stprintf_s(&sPciFunctionNo[1],	(_countof(sPciFunctionNo)-1),	_TEXT("%u"), pIn->lPciFunctionNo);

			// append PciStrings
			_tcsncat_s(sPciBusNo,		_countof(sPciBusNo),	_TEXT(","), _TRUNCATE);
			_tcsncat_s(sPciDeviceNo,	_countof(sPciDeviceNo),	_TEXT(","), _TRUNCATE);
		}

		sTmp = pIn->sPciLoc;

		// try to find BusNo at PciLoc
		sFirstOccur = _tcsstr(	sTmp,
								sPciBusNo);

		if	(sFirstOccur == NULL)
			// SubString not found
		{
			eResult = ePNDEV_ERROR_NOT_FOUND;

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): BusNo not found!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);
		}
		else
		{
			sTmp = sFirstOccur + _tcslen(sPciBusNo);

			// try to find DeviceNo at remaining part of PciLoc
			sFirstOccur = _tcsstr(	sTmp,
									sPciDeviceNo);

			if	(sFirstOccur == NULL)
				// SubString not found
			{
				eResult = ePNDEV_ERROR_NOT_FOUND;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): BusNo found, DeviceNo not found!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				sTmp = sFirstOccur + _tcslen(sPciDeviceNo);

				// try to find FunctionNo at remaining part of PciLoc
				sFirstOccur = _tcsstr(	sTmp,
										sPciFunctionNo);

				if	(sFirstOccur == NULL)
					// SubString not found
				{
					eResult = ePNDEV_ERROR_NOT_FOUND;

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): BusNo found, DeviceNo found, FunctionNo not found!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
					sTmp = sFirstOccur + _tcslen(sPciFunctionNo);

					// success
					eResult = ePNDEV_OK;
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  copy data between HostDataBuffer and external AsicSdram
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_CopyData(HANDLE					hDriverIn,
												const UINT32			lSizeInputBufIn,
												const UINT32			lSizeOutputBufIn,
												uPNDEV_COPY_DATA_IN*	pIn,
												uPNDEV_COPY_DATA_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_COPY_DATA_IN),
										sizeof(uPNDEV_COPY_DATA_OUT),
										FALSE,							// size of output buffer may be larger
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			UINT32	lSizeHostBuf	= 0;
			UINT32	lSizeAsicDest	= 0;
			UINT32	lOffsetAsicMin	= 0;

				// compute size of HostBuffer
				lSizeHostBuf = lSizeOutputBufIn - sizeof(uPNDEV_COPY_DATA_OUT);

				switch	(pIn->eHwRes)
						// HwRes
				{
					case ePNDEV_COPY_HW_RES_SDRAM:
					{
						lSizeAsicDest	= pUsedDevice->fnGetSizeSdramSys();
						lOffsetAsicMin	= PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0;

						break;
					}
					case ePNDEV_COPY_HW_RES_PNIP_IRTE:
					{
						lSizeAsicDest = pUsedDevice->fnGetSizePnipIrte();

						break;
					}
					default:
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Invalid Asic destination!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);

						break;
					}
				}

				if	(lSizeAsicDest != 0)
					// success
				{
				BOOLEAN	bResultWrite	= TRUE;
				BOOLEAN	bResultRead		= TRUE;
				UINT32	i				= 0;
				_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF]	= {0};

					for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
					{
						if	(pIn->uArrayBlockWrite[i].lSize != 0)
							// current WriteBlock exist
						{
							if	(	((pIn->uArrayBlockWrite[i].lOffsetHostDataBuf + pIn->uArrayBlockWrite[i].lSize)	> lSizeHostBuf)
								||	((pIn->uArrayBlockWrite[i].lOffsetAsic + pIn->uArrayBlockWrite[i].lSize)		> lSizeAsicDest)
								||	(pIn->uArrayBlockWrite[i].lOffsetAsic											< lOffsetAsicMin))
								//		current WriteBlock outside of HostDataBuffer
								// OR	current WriteBlock outside of AsicDestination
								// OR	writing to reserved area
							{
								// error
								bResultWrite = FALSE;

								// set ErrorDetail
								_stprintf_s(sErrorDetail,
											_countof(sErrorDetail),
											_TEXT(" (number=%u)"),
											i);

								// set ErrorString
								clsString::fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Invalid WriteBlock!"),
															sErrorDetail,
															_countof(pOut->sError),
															pOut->sError);

								// leave loop
								break;
							}
						}
					}

					if	(bResultWrite)
						// success
					{
						for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
						{
							if	(pIn->uArrayBlockRead[i].lSize != 0)
								// current ReadBlock exist
							{
								if	(	((pIn->uArrayBlockRead[i].lOffsetHostDataBuf + pIn->uArrayBlockRead[i].lSize)	> lSizeHostBuf)
									||	((pIn->uArrayBlockRead[i].lOffsetAsic + pIn->uArrayBlockRead[i].lSize)			> lSizeAsicDest))
									//		current ReadBlock outside of HostDataBuffer
									// OR	current ReadBlock outside of AsicDestination
								{
									// error
									bResultRead = FALSE;

									// set ErrorDetail
									_stprintf_s(sErrorDetail,
												_countof(sErrorDetail),
												_TEXT(" (number=%u)"),
												i);

									// set ErrorString
									clsString::fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Invalid ReadBlock!"),
																sErrorDetail,
																_countof(pOut->sError),
																pOut->sError);

									// leave loop
									break;
								}
							}
						}

						if	(bResultRead)
							// success
						{
							if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
															IOCTL_COPY_DATA,
															sizeof(uPNDEV_COPY_DATA_IN),
															lSizeOutputBufIn,
															pIn,
															pOut))
								// error at IoctlCopyData
							{
								eResult = ePNDEV_FATAL;

								// ErrorString already set
							}
							else
							{
								// success
								eResult = ePNDEV_OK;
							}
						}
					}
				}
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  close device
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_CloseDevice(	HANDLE						hDriverIn,
													const UINT32				lSizeInputBufIn,
													const UINT32				lSizeOutputBufIn,
													uPNDEV_CLOSE_DEVICE_IN*		pIn,
													uPNDEV_CLOSE_DEVICE_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_CLOSE_DEVICE_IN),
										sizeof(uPNDEV_CLOSE_DEVICE_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			BOOLEAN bResultList = FALSE;

				if	(pUsedDevice->fnGetMasterApp())
					// MasterApp
				{
					// at first it must be ensured that all interrupts are disabled!

					// mask all interrupts at ICU
					pUsedDevice->fnMaskIcu_All(FALSE);	// no initialization
				}

				// preset
				bResultList = TRUE;

				if	((pUsedDevice->m_lStateOpenDevice & PNDEV_DEVICE_STATE__ADD_LIST_DONE) != 0)
					// UsedDeviceObject stored at UsedDeviceList
				{
					if	(!fnBlkAtList(	pDataSet->fnGetPtrListUsedDevice(),
										((uLIST_HEADER*) pUsedDevice)))
						// UsedDeviceObject not stored at UsedDeviceList
					{
						bResultList = FALSE;

						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Device not found at UsedDeviceList!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else
					{
						// remove UsedDeviceObject from UsedDeviceList
						fnRemoveBlkFromList(pDataSet->fnGetPtrListUsedDevice(),
											((uLIST_HEADER*) pUsedDevice));

						// decrement CtrDevice
						pDataSet->fnDecrCtrUsedDevice();

						// clear flag
						pUsedDevice->m_lStateOpenDevice &= ~PNDEV_DEVICE_STATE__ADD_LIST_DONE;
					}
				}

				if	(bResultList)
					// success
				{
				BOOLEAN	bResultIoctl = FALSE;

					// preset
					bResultIoctl = TRUE;

					if	((pUsedDevice->m_lStateOpenDevice & PNDEV_DEVICE_STATE__IOCTL_OPEN_DEVICE_DONE) != 0)
						// IoctlOpenDevice done
					{
						// close used timers before closing device
						// make sure all used timers (ktimer tick receivers) removed before closing device 
						if	(!pUsedDevice->fnShutDownUsedTimer(	_countof(pOut->sError),
																pOut->sError))
							// error at closing used timers
						{
							bResultIoctl = FALSE;

							// ErrorString already set
						}
						else
						{
							// set internal parameter for SYS
							//	- use 64Bit value because DriverHandle may be a 64Bit handle!
							pIn->uInternal.uAppSys.lUint64 = pUsedDevice->fnGetHandleAppSys();

							if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
															IOCTL_CLOSE_DEVICE,
															sizeof(uPNDEV_CLOSE_DEVICE_IN),
															sizeof(uPNDEV_CLOSE_DEVICE_OUT),
															pIn,
															pOut))
								// error at IoctlCloseDevice
							{
								bResultIoctl = FALSE;

								// ErrorString already set
							}
							else
							{
								// clear flag
								pUsedDevice->m_lStateOpenDevice &= ~PNDEV_DEVICE_STATE__IOCTL_OPEN_DEVICE_DONE;

								if	(!pUsedDevice->fnUnmapMemory(	(uPNDEV_HW_RES_SINGLE_UM*) &pOut->uInternal.uBarUm[0],
																	&pOut->uInternal.uSdramUm,
																	&pOut->uInternal.uHostMemUm))
									// error
								{
									bResultIoctl = FALSE;

									// set ErrorString
									clsString::fnBuildString(   _TEXT(__FUNCTION__),
																_TEXT("(): Error at Release Memory after CloseDevice!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);
								}
							}
						}
					}

					if	(bResultIoctl)
						// success
					{
						if	(pDataSet->fnReleaseDeviceHandleSys(pUsedDevice->fnGetHandleDeviceSys(),
																_countof(pOut->sError),
																pOut->sError))
							// releasing DeviceHandleSys ok
						{
							// terminate DllThread

							if	(	(pIn->bDoAnyway)
								||	(fnGetThreadHandle() == pUsedDevice->fnGetHandleThreadDll()))
								//		(service called by service CloseDriver (called by user or DllDetach))
								// OR	(service called at DllThread)
							{
								// Note:
								//	- after end of this service DllThread will get EventStop
								//	- DllThread will delete UsedDeviceObject

								if	(pUsedDevice->fnSetEventThreadDll(	ePNDEV_EVENT_THREAD_DLL_STOP,
																		_countof(pOut->sError),
																		pOut->sError))
									// setting EventStop of DllThread ok
								{
									// success
									eResult = ePNDEV_OK;
								}
							}
							else
							{
								// Note:
								//	- DllThread will get EventStopWithCon
								//	- DllThread must not delete UsedDeviceObject because it is needed for EventConStop
								//	- after getting EventConStop it is guaranteed that DllThread is finished
								//		-> UsedDeviceObject can be deleted here (don't delete it in error cases!)

								if	(pUsedDevice->fnSetEventThreadDll(	ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON,
																		_countof(pOut->sError),
																		pOut->sError))
									// setting EventStopWithCon of DllThread ok
								{
								_TCHAR sErrorTmp[PNDEV_SIZE_STRING_BUF]	= {0};

									if	(pUsedDevice->fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE_CON_STOP,
																					_countof(pOut->sError),
																					pOut->sError))
										// wait for SingleEvent EventConStop at ServiceThread ok
									{
										// success
										eResult = ePNDEV_OK;
									}

									// shut down resources of ServiceThread / DllThread
									pUsedDevice->fnShutDownThreadRes(	_countof(sErrorTmp),
																		sErrorTmp);

									// free UsedDeviceObject
									delete pUsedDevice;
									pUsedDevice = NULL;
								}
							}
						}
					}
				}
			}

			// reset
			memset(	&pIn->uInternal,
					0,
					sizeof(uPNDEV_CLOSE_DEVICE_INTERNAL_IN));
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize NRT interface
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_InitNrt(HANDLE					hDriverIn,
												const UINT32			lSizeInputBufIn,
												const UINT32			lSizeOutputBufIn,
												uPNDEV_INIT_NRT_IN*		pIn,
												uPNDEV_INIT_NRT_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;

#if (PNDEV_PLATFORM==PNDEV_UM_PLATFORM_32)
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_INIT_NRT_IN),
										sizeof(uPNDEV_INIT_NRT_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else if	(!pUsedDevice->fnGetMasterApp())
					// not MasterApp
			{
				eResult = ePNDEV_ERROR_MASTER_APP;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Service not supported for a NonMaster App!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			UINT32	lAlignment			= 0;
			UINT32	lSizeDmacw			= 0;
			UINT32	lSizeFrameBuffer	= 0;

				// Ertec200P needs a DMACW alignment of 4
				lAlignment = 4;

				lSizeDmacw			= sizeof(uPNDEV_DMACW);
				lSizeFrameBuffer	= sizeof(uPNDEV_FRAME_BUF);

				if	(	((lSizeDmacw		% lAlignment)	!= 0)
					||	((lSizeFrameBuffer	% lAlignment)	!= 0))
					// not aligned size
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Size of DMACW or FrameBuffer is not a multiple of alignment!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
				UINT32 lCtrNrtRqb = 0;

					if	(pUsedDevice->fnInitDmmNrt(	pIn->uMemNrt,
													pIn->uMemDescTable,
													lAlignment,
													_countof(pOut->sError),
													&lCtrNrtRqb,
													pOut->sError))
						// initialize DMM ok
					{
						// success
						eResult = ePNDEV_OK;

						// activate PHY
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_PHY_CONTROL,
													(pUsedDevice->READ_REG_PNIP(PNIP_REG_PHY_CONTROL) | PNIP_R2_REG_PHY_CONTROL__MSK_PHY_ACTIVATE));

						// don't discard frames
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_FLOWCONTROL_P1, 0);
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_FLOWCONTROL_P2, 0);

						// enable send/rcve
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_MACCONTROL_P1, 0x0101);
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_MACCONTROL_P2, 0x0101);

						// forward all received frames to API and Port1/2
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_UC_DEFAULT_A, 		0x1f);
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_MC_DEFAULT_API,	0x1f);
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_MC_DEFAULT_P1,		0x1f);
						pUsedDevice->WRITE_REG_PNIP(PNIP_REG_MC_DEFAULT_P2,		0x1f);
					}
				}
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

#else
	UNREFERENCED_PARAMETER(hDriverIn);
	UNREFERENCED_PARAMETER(lSizeInputBufIn);
	UNREFERENCED_PARAMETER(lSizeOutputBufIn);
	UNREFERENCED_PARAMETER(pIn);
	UNREFERENCED_PARAMETER(pOut);

#endif // #if (PNDEV_PLATFORM==PNDEV_UM_PLATFORM_32)

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate NRT Rqb
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_AllocNrtRqb(HANDLE						hDriverIn,
													const UINT32				lSizeInputBufIn,
													const UINT32				lSizeOutputBufIn,
													uPNDEV_ALLOC_NRT_RQB_IN*	pIn,
													uPNDEV_ALLOC_NRT_RQB_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_ALLOC_NRT_RQB_IN),
										sizeof(uPNDEV_ALLOC_NRT_RQB_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			uPNDEV_NRT_RQB*	pNrtRqb		= NULL;
			eDMM_ERROR		eErrorDmm	= eDMM_ERROR_INVALID;

				// allocate block
				//	- queue 0
				pNrtRqb = (uPNDEV_NRT_RQB*) pUsedDevice->m_oMemDmm.dmm_alloc_mem(	sizeof(uPNDEV_NRT_RQB),
																					0,
																					&eErrorDmm);

				if	(pNrtRqb == NULL)
					// error
				{
				_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

					// set ErrorDetail
					_stprintf_s(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT(" (ErrorCode=%d)"),
								eErrorDmm);

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): DMM error at dmm_alloc_mem()!"),
												sErrorDetail,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
					// success
					eResult = ePNDEV_OK;

					// return
					pOut->uNrtRqb.pPtr = pNrtRqb;
				}
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  free NRT Rqb
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_FreeNrtRqb(	HANDLE						hDriverIn,
													const UINT32				lSizeInputBufIn,
													const UINT32				lSizeOutputBufIn,
													uPNDEV_FREE_NRT_RQB_IN*		pIn,
													uPNDEV_FREE_NRT_RQB_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_FREE_NRT_RQB_IN),
										sizeof(uPNDEV_FREE_NRT_RQB_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
			LSA_BOOL	bResult		= LSA_FALSE;
			eDMM_ERROR	eErrorDmm	= eDMM_ERROR_INVALID;

				// free block
				bResult = pUsedDevice->m_oMemDmm.dmm_free_mem(	(void*) pIn->uNrtRqb.pPtr,
																&eErrorDmm);

				if	(bResult == LSA_FALSE)
					// error
				{
				_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

					// set ErrorDetail
					_stprintf_s(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT(" (ErrorCode=%d)"),
								eErrorDmm);

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): DMM error at dmm_free_mem()!"),
												sErrorDetail,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
					// success
					eResult = ePNDEV_OK;
				}
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  send this NRT frame
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_SendNrt(HANDLE					hDriverIn,
												const UINT32			lSizeInputBufIn,
												const UINT32			lSizeOutputBufIn,
												uPNDEV_SEND_NRT_IN*		pIn,
												uPNDEV_SEND_NRT_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_SEND_NRT_IN),
										sizeof(uPNDEV_SEND_NRT_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				UNREFERENCED_PARAMETER(lSizeInputBufIn);
				UNREFERENCED_PARAMETER(lSizeOutputBufIn);
				UNREFERENCED_PARAMETER(pIn);
				UNREFERENCED_PARAMETER(pOut);
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  receive any NRT frame
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_RcveNrt(HANDLE					hDriverIn,
												const UINT32			lSizeInputBufIn,
												const UINT32			lSizeOutputBufIn,
												uPNDEV_RCVE_NRT_IN*		pIn,
												uPNDEV_RCVE_NRT_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_RCVE_NRT_IN),
										sizeof(uPNDEV_RCVE_NRT_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				UNREFERENCED_PARAMETER(lSizeInputBufIn);
				UNREFERENCED_PARAMETER(lSizeOutputBufIn);
				UNREFERENCED_PARAMETER(pIn);
				UNREFERENCED_PARAMETER(pOut);
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  change NRT
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT	fnPnDev_ChangeNrt(	HANDLE					hDriverIn,
													const UINT32			lSizeInputBufIn,
													const UINT32			lSizeOutputBufIn,
													uPNDEV_CHANGE_NRT_IN*	pIn,
													uPNDEV_CHANGE_NRT_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_CHANGE_NRT_IN),
										sizeof(uPNDEV_CHANGE_NRT_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				UNREFERENCED_PARAMETER(lSizeInputBufIn);
				UNREFERENCED_PARAMETER(lSizeOutputBufIn);
				UNREFERENCED_PARAMETER(pIn);
				UNREFERENCED_PARAMETER(pOut);
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}


	return(eResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  open timer
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_OpenTimer(	HANDLE					hDriverIn,
													const UINT32			lSizeInputBufIn,
													const UINT32			lSizeOutputBufIn,
													uPNDEV_OPEN_TIMER_IN*	pIn,
													uPNDEV_OPEN_TIMER_OUT*	pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
clsUsedDevice*	pUsedDevice = NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_OPEN_TIMER_IN),
										sizeof(uPNDEV_OPEN_TIMER_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same timer is referenced or UsedTimerList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				if	(pIn->uCbfTick.pPtr == NULL)
						// invalid CbfTick
				{
					eResult = ePNDEV_ERROR_CBF;

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): CBF_Tick not set!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
				clsUsedTimer* pUsedTimer = NULL;

					// allocate/preset UsedTimerObject
					pUsedTimer = new clsUsedTimer();

					if	(pUsedTimer == NULL)
						// error
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): Insufficient memory for UsedTimerObject!"),
													NULL,
													_countof(pOut->sError),
													pOut->sError);
					}
					else
					{
					UINT32 lStackSize = 0;

						// Note:
						//	- now UsedTimerObject exist
						//		-> service CloseTimer can be used if there is an error

						// set variables of UsedTimerObject
						pUsedTimer->fnSetVarOpenTimer1(	hDriverIn,
														pDataSet,
														pUsedDevice,
														pIn);

						if	(pIn->uThreadKTimer.lStackSize == 0)
							// default value desired
						{
							// 16kByte (PTHREAD_STACK_MIN in Linux)
							lStackSize = (1024*16);
						}
						else
						{
							lStackSize = pIn->uThreadKTimer.lStackSize;
						}

						if	(pUsedTimer->fnStartUpTimerRes(	pIn->uThreadKTimer.lPrio,
															lStackSize,
															pIn->uThreadKTimer.sName,
															_countof(pOut->sError),
															pOut->sError))
							// starting up resources of Timerthread ok
						{
						BOOLEAN bResultOpen = FALSE;

							// set internal parameter for SYS
							pIn->uInternal.uKTimerIoctl.uEventUtmrTick.hHandle		= pUsedTimer->fnGetHandleEventUtmrTick();
							pIn->uInternal.uKTimerIoctl.uEventUtmrTickSynch.hHandle	= pUsedTimer->fnGetHandleEventUtmrTickSynch();

							if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
															IOCTL_OPEN_TIMER,
															sizeof(uPNDEV_OPEN_TIMER_IN),
															sizeof(uPNDEV_OPEN_TIMER_OUT),
															pIn,
															pOut))
								// error at IoctlOpenTimer
							{
								eResult = ePNDEV_FATAL;
							}
							else
							{
								// update StateOpenTimer
								pUsedTimer->m_lStateOpenTimer |= PNDEV_TIMER_STATE__IOCTL_OPEN_TIMER_DONE;

								// setting variables of UsedTimerObject
								pUsedTimer->fnSetVarOpenTimer2(pOut);
									
								// update UsedTimerList
								{
									// put UsedTimerObject to UsedTimerList
									fnPutBlkToList(	pUsedDevice->fnGetPtrListUsedTimer(),
													((uLIST_HEADER*) pUsedTimer));

									// increment CtrTimer
									pUsedDevice->fnIncrCtrUsedTimer();
								}

								// update StateOpenTimer
								//	- e.g. pUsedTimer->m_uReceiverSys is needed for CloseTimer
								pUsedTimer->m_lStateOpenTimer |= PNDEV_TIMER_STATE__ADD_LIST_DONE;

								// success
								bResultOpen = TRUE;
							}

							if	(!bResultOpen)
								// error
							{
							ePNDEV_RESULT			eResultTmp = ePNDEV_FATAL;
							uPNDEV_CLOSE_TIMER_IN	uCloseTimerIn;
							uPNDEV_CLOSE_TIMER_OUT	uCloseTimerOut;

								// set InputParameter
								{
									memset(	&uCloseTimerIn,
											0,
											sizeof(uPNDEV_CLOSE_TIMER_IN));

									uCloseTimerIn.uTimer.hHandle = pUsedTimer;
								}

								// close timer
								eResultTmp = fnPnDev_CloseTimer(	hDriverIn,
																	sizeof(uPNDEV_CLOSE_TIMER_IN),
																	sizeof(uPNDEV_CLOSE_TIMER_OUT),
																	&uCloseTimerIn,
																	&uCloseTimerOut);
								if(eResultTmp != ePNDEV_OK)
									// error
								{
									eResult = eResultTmp;

									// set ErrorString
									clsString::fnBuildString(   _TEXT(__FUNCTION__),
																_TEXT("(): Error Closing Timer!"),
																NULL,
																_countof(pOut->sError),
																pOut->sError);
								}
							}
							else
							{
								// success
								eResult = ePNDEV_OK;

								// return
								//	- must be after IoctlOpenTimer because OutputParameter are preset!
								pOut->uTimer.hHandle = pUsedTimer;
							}
						}
					}
				}
			}

			// reset
			{
				memset(	&pIn->uInternal,
						0,
						sizeof(uPNDEV_OPEN_TIMER_INTERNAL_IN));

				memset(	&pOut->uInternal,
						0,
						sizeof(uPNDEV_OPEN_TIMER_INTERNAL_OUT));
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  close timer
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_CloseTimer(	HANDLE						hDriverIn,
													const UINT32				lSizeInputBufIn,
													const UINT32				lSizeOutputBufIn,
													uPNDEV_CLOSE_TIMER_IN*		pIn,
													uPNDEV_CLOSE_TIMER_OUT*		pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
clsUsedTimer*	pUsedTimer = NULL;
clsUsedDevice*	pUsedDevice = NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;



	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_CLOSE_TIMER_IN),
										sizeof(uPNDEV_CLOSE_TIMER_OUT),
										TRUE,
										pOut,
										hDriverIn,
										_countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same Timer is referenced or UsedTimerList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
			// get TimerHandle
			pUsedTimer = (clsUsedTimer*) pIn->uTimer.hHandle;

			if	(	(pUsedTimer	== NULL)
				||	(pUsedTimer	!= pUsedTimer->m_hTimerVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid TimerHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				// get DeviceHandle
				pUsedDevice = pUsedTimer->fnGetUsedDevice();

				if	(	(pUsedDevice	== NULL)
					||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid DeviceHandle!"),
												NULL,
												_countof(pOut->sError),
												pOut->sError);
				}
				else
				{
				BOOLEAN bResultList = FALSE;

					// preset
					bResultList = TRUE;

					if	((pUsedTimer->m_lStateOpenTimer & PNDEV_TIMER_STATE__ADD_LIST_DONE) != 0)
						// UsedTimerObject stored at UsedTimerList
					{
						if	(!fnBlkAtList(	pUsedDevice->fnGetPtrListUsedTimer(),
											((uLIST_HEADER*) pUsedTimer)))
							// UsedTimerObject not stored at UsedTimerList
						{
							bResultList = FALSE;

							// set ErrorString
							clsString::fnBuildString(	_TEXT(__FUNCTION__),
														_TEXT("(): Timer not found at UsedTimerList!"),
														NULL,
														_countof(pOut->sError),
														pOut->sError);
						}
						else
						{
							// remove UsedTimerObject from UsedTimerList
							fnRemoveBlkFromList(pUsedDevice->fnGetPtrListUsedTimer(),
												((uLIST_HEADER*) pUsedTimer));

							// decrement CtrTimer
							pUsedDevice->fnDecrCtrUsedTimer();

							// clear flag
							pUsedTimer->m_lStateOpenTimer &= ~PNDEV_TIMER_STATE__ADD_LIST_DONE;
						}
					}

					if	(bResultList)
						// success
					{
					BOOLEAN	bResultIoctl = FALSE;

						// preset
						bResultIoctl = TRUE;

						if	((pUsedTimer->m_lStateOpenTimer & PNDEV_TIMER_STATE__IOCTL_OPEN_TIMER_DONE) != 0)
							// IoctlOpenTimer done
						{
							// set internal parameter for SYS
							//	- use 64Bit value because DriverHandle may be a 64Bit handle!
							pIn->uInternal.uReceiverSys.lUint64 = pUsedTimer->fnGetHandleReceiverSys();

							if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
															IOCTL_CLOSE_TIMER,
															sizeof(uPNDEV_CLOSE_TIMER_IN),
															sizeof(uPNDEV_CLOSE_TIMER_OUT),
															pIn,
															pOut))
								// error at IoctlCloseTimer
							{
								bResultIoctl = FALSE;

								// ErrorString already set
							}
							else
							{
								// clear flag
								pUsedTimer->m_lStateOpenTimer &= ~PNDEV_TIMER_STATE__IOCTL_OPEN_TIMER_DONE;
							}
						}

						if	(bResultIoctl)
							// success
						{
							// terminate Timer
							{
								// Note:
								//	- Timer will get EventStopWithCon
								//	- after getting EventConStop it is guaranteed that Timer is finished
								//		-> UsedTimerObject can be deleted here (don't delete it in error cases!)

								if	(pUsedTimer->fnSetEventTimer(	ePNDEV_EVENT_TIMER_STOP_WITH_CON,
																	_countof(pOut->sError),
																	pOut->sError))
									// setting EventStopWithCon of Timer ok
								{
								_TCHAR sErrorTmp[PNDEV_SIZE_STRING_BUF]	= {0};

									if	(pUsedTimer->fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE_CON_STOP,
																					_countof(pOut->sError),
																					pOut->sError))
										// wait for SingleEvent EventConStop at ServiceThread ok
									{
										// success
										eResult = ePNDEV_OK;
									}

									// shut down resources of Timerthread
									pUsedTimer->fnShutDownTimerRes(	_countof(sErrorTmp),
																	sErrorTmp);

									// free UsedTimerObject
									delete pUsedTimer;
									pUsedTimer = NULL;
								}
							}
						}
					}
				}
			}

			// reset
			memset(	&pIn->uInternal,
					0,
					sizeof(uPNDEV_CLOSE_TIMER_INTERNAL_IN));
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
// Calls IOCTL for NetProfi Kernel Mode. Corresponding KernelMode function: fnIoctlDoNetProfi
//************************************************************************

extern "C"
PNDEV_DRIVER_API ePNDEV_RESULT fnPnDev_DoNetProfi      (	HANDLE						hDriverIn,
														const UINT32					lSizeInputBufIn,
														const UINT32					lSizeOutputBufIn,
														uPNDEV_NP_DO_NETPROFI_REQ_IN*		pIn,
														uPNDEV_NP_DO_NETPROFI_REQ_OUT*	    pOut)
{
ePNDEV_RESULT	eResult		= ePNDEV_FATAL;
clsDataSet*		pDataSet	= NULL;
BOOLEAN			bFatalBuf	= FALSE;

	// fnDoEntryActions():
	//	- structure of OutputParameter is preset
	//	- DriverHandle is verified

	pDataSet = (clsDataSet*) ((uPNDEV_DRIVER_DESC*) hDriverIn)->pDataSet;

	if	(!pDataSet->fnDoEntryActions(	lSizeInputBufIn,
										lSizeOutputBufIn,
										sizeof(uPNDEV_NP_DO_NETPROFI_REQ_IN),
										sizeof(uPNDEV_NP_DO_NETPROFI_REQ_OUT),
										TRUE,
										pOut,
										hDriverIn,
                                        _countof(pOut->sError),
										&bFatalBuf,
										pOut->sError))
		// error at entry actions
	{
		if	(bFatalBuf)
			// FatalErrorBuf
		{
			eResult = ePNDEV_FATAL_BUF;
		}
	}
	else
	{
	BOOLEAN	bResultEnter	= FALSE;
	BOOLEAN	bResultLeave	= FALSE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		//	- prevent simultaneous calling of services at different threads
		//	- avoid problems if same device is referenced or UsedDeviceList is used
		bResultEnter = pDataSet->fnEnterCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);
		{
		clsUsedDevice* pUsedDevice = NULL;

			// get DeviceHandle
			pUsedDevice = (clsUsedDevice*) pIn->uCommon.uDevice.hHandle;

			if	(	(pUsedDevice	== NULL)
				||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid DeviceHandle!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
			}
			else
			{
				if	(!pDataSet->fnDoDeviceIoctl((void*) pUsedDevice,
												IOCTL_DO_NETPROFI,
                                                sizeof (uPNDEV_NP_DO_NETPROFI_REQ_IN) ,
												sizeof (uPNDEV_NP_DO_NETPROFI_REQ_OUT),
												pIn,
												pOut))
					// error
				{
					eResult = ePNDEV_FATAL;

					// ErrorString already set
				}
				else
				{
					// success
					eResult = ePNDEV_OK;
				}
			}
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		bResultLeave = pDataSet->fnLeaveCritSec(ePNDEV_CRIT_SEC_DLL_SERVICE,
												_countof(pOut->sError),
												pOut->sError);

		if	(	!bResultEnter
			||	!bResultLeave)
			// error
		{
			eResult = ePNDEV_FATAL;
		}
	}

	return(eResult);
}
//////////////////////////////////////////////////////////////////////////

	// example of an exported variable
	//PNDEV_DRIVER_API int lTest = 0;

//************************************************************************
//  D e s c r i p t i o n :
//
//  example of an exported function
//************************************************************************

PNDEV_DRIVER_API int fnPnDev_Test(void)
{
	return(0x1234);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  constructor of exported class
//	- class definition: see PnDev_Driver.h
//************************************************************************
/*
clsPnDev_Driver::clsPnDev_Driver()
{
	return;
}
*/

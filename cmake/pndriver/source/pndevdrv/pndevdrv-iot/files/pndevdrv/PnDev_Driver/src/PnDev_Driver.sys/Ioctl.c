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
/*  F i l e               &F: Ioctl.c                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling of Ioctl requests
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Ioctl.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnDoIoctl)
//	#pragma alloc_text (PAGE, fnIoctlOpenDevice)
//	#pragma alloc_text (PAGE, fnIoctlCloseDevice)
//	#pragma alloc_text (PAGE, fnIoctlResetDevice)
//	#pragma alloc_text (PAGE, fnIoctlGetEventList)
//	#pragma alloc_text (PAGE, fnIoctlSetEventList)
//	#pragma alloc_text (PAGE, fnIoctlChangeInt)
//	#pragma alloc_text (PAGE, fnIoctlGetDeviceState)
//	#pragma alloc_text (PAGE, fnIoctlProgramFlash)
//	#pragma alloc_text (PAGE, fnIoctlChangeXhifPage)
//	#pragma alloc_text (PAGE, fnIoctlCopyData)
//	#pragma alloc_text (PAGE, fnIoctlStartBootFw)
//	#pragma alloc_text (PAGE, fnIoctlStartUserFw)
//	#pragma alloc_text (PAGE, fnIoctlIsUserFwReady)
//	#pragma alloc_text (PAGE, fnIoctlChangeAppLock)
//	#pragma alloc_text (PAGE, fnIoctlNotifyDllState)
//	#pragma alloc_text (PAGE, fnIoctlMapMemory)
//	#pragma alloc_text (PAGE, fnIoctlSetHwResUm)
//	#pragma alloc_text (PAGE, fnIoctlGetIntSource)
//	#pragma alloc_text (PAGE, fnCheckIoctlPar)
//	#pragma alloc_text (PAGE, fnCloseDevice)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  do IoDeviceControl
//************************************************************************

ePNDEV_IOCTL fnDoIoctl(	DEVICE_EXTENSION*	pDevExtIn,						// Attention: pDevExtIn may be NULL if DriverService!
						HANDLE				hOsParIn,
						const UINT32		lIoctlIn,
						const UINT32		lSizeInputBufIn,
						const UINT32		lSizeOutputBufIn,
						void*				pInputBufIn,
						void*				pOutputBufIn)
{
ePNDEV_IOCTL	eResult = ePNDEV_IOCTL_INVALID;

	#ifdef PNDEV_DEVELOPMENT
	char			sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ++++ IOCTL 0x%x",	// pDevExtIn may be NULL
							lIoctlIn);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					FALSE,						// logging
					sTraceDetail);
	#endif

	// check IoctlPar
	eResult = fnCheckIoctlPar(	lIoctlIn,
								lSizeInputBufIn,
								lSizeOutputBufIn);

	if	(eResult == ePNDEV_IOCTL_OK)
		// success
	{
	BOOLEAN	bResultBuf	= FALSE;
	void*	pIn			= NULL;
	void*	pOut		= NULL;

		if	(	(pInputBufIn	!= NULL)
			&&	(pOutputBufIn	!= NULL))
			// valid IoctlBuffer (e.g. Adonis)
		{
			// success
			bResultBuf = TRUE;

			// use passed IoctlBuffer
			pIn		= pInputBufIn;
			pOut	= pOutputBufIn;
		}
		else	// e.g. Windows
		{
			// preset
			eResult = ePNDEV_IOCTL_INVALID;

			// get IoctlBuffer
			eResult = fnGetIoctlBuf(hOsParIn,
									lSizeInputBufIn,
									lSizeOutputBufIn,
									&pIn,
									&pOut);

			if	(eResult == ePNDEV_IOCTL_OK)
				// success
			{
				bResultBuf = TRUE;
			}
		}

		if	(bResultBuf)
			// success
		{
		KIRQL	lIrqlCur = PASSIVE_LEVEL;
		_TCHAR	sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// Note:
			//	- now errors can be returned by pOut->sError

			// preset
			eResult = ePNDEV_IOCTL_ERROR_BY_STRING;

			// Note:
			//	- IOCTLs must not be called at DISPATCH_LEVEL because of some system calls

			if	(!fnCheckIrql(	PASSIVE_LEVEL,
								&lIrqlCur))
				// invalid IRQL
			{
			uPNDEV_IOCTL_OUT* pOutTmp = pOut;

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): IOCTL 0x%x: Invalid Irql (=%u) [%s()]!"),
									lIoctlIn,
									lIrqlCur,
									__FUNCTION__);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								NULL,
								_countof(pOutTmp->sError),
								pOutTmp->sError);
			}
			else
			{
			BOOLEAN bResultService = FALSE;

				if	(pDevExtIn == NULL)
					// DriverService (not a DeviceService) -> no DeviceHandle!
				{
					switch	(lIoctlIn)
							// IoControlCode
					{
						case IOCTL_GET_DEVICE_INFO_SYS:
						{
							// service GetDeviceInfoSys
							bResultService = fnIoctlGetDeviceInfoSys(	(uPNDEV_GET_DEVICE_INFO_SYS_IN*)	pIn,
																		(uPNDEV_GET_DEVICE_INFO_SYS_OUT*)	pOut);

							break;
						}
						case IOCTL_GET_DEVICE_HANDLE_SYS:
						{
							// service GetDeviceHandleSys
							bResultService = fnIoctlGetDeviceHandleSys(	(uPNDEV_GET_DEVICE_HANDLE_SYS_IN*)	pIn,
																		(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*)	pOut);

							break;
						}
						case IOCTL_ALLOC_EVENT_ID:
						{
							// service AllocEventId
							bResultService = fnIoctlAllocEventId(	(uPNDEV_ALLOC_EVENT_ID_IN*)		pIn,
																	(uPNDEV_ALLOC_EVENT_ID_OUT*)	pOut);

							break;
						}
						case IOCTL_FREE_EVENT_ID:
						{
							// service FreeEventId
							bResultService = fnIoctlFreeEventId((uPNDEV_FREE_EVENT_ID_IN*)	pIn,
																(uPNDEV_FREE_EVENT_ID_OUT*)	pOut);

							break;
						}


						default:
						{
							break;
						}
					}

					if	(bResultService)
						// success
					{
						eResult = ePNDEV_IOCTL_OK;
					}
				}
				else
				{
					//------------------------------------------------------------------------
					// enter CriticalSection (serialize IOCTLs for this device)
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_IOCTL);
					{
					    
						if	(pDevExtIn->bIoctlRunning)
							// IOCTL interrupted itself
						{
						uPNDEV_IOCTL_OUT* pOutTmp = pOut;

							// set ErrorDetail
							_RtlStringCchPrintf(sErrorDetail,
												_countof(sErrorDetail),
												_TEXT("(): IOCTL 0x%x: IOCTL interrupted itself!"),
												lIoctlIn);

							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											sErrorDetail,
											NULL,
											_countof(pOutTmp->sError),
											pOutTmp->sError);
						}
						else
						{
                            // set variable
							pDevExtIn->bIoctlRunning = TRUE;
							{
							#ifdef PNDEV_GET_RUNTIME_IOCTL
							uPNDEV_CLOCK uClockStart;

									// get clock
									fnGetClock(&uClockStart);
							#endif

								switch	(lIoctlIn)
										// IoControlCode
								{
									case IOCTL_OPEN_DEVICE:
									{
										// service OpenDevice
										bResultService = fnIoctlOpenDevice(	pDevExtIn,
																			(uPNDEV_OPEN_DEVICE_IN*)	pIn,
																			(uPNDEV_OPEN_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_CLOSE_DEVICE:
									{
										// service CloseDevice
										bResultService = fnIoctlCloseDevice(pDevExtIn,
																			(uPNDEV_CLOSE_DEVICE_IN*)	pIn,
																			(uPNDEV_CLOSE_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_RESET_DEVICE:
									{
										// service ResetDevice
										bResultService = fnIoctlResetDevice(pDevExtIn,
																			(uPNDEV_RESET_DEVICE_IN*)	pIn,
																			(uPNDEV_RESET_DEVICE_OUT*)	pOut);

										break;
									}
									case IOCTL_GET_EVENT_LIST:
									{
										// service GetEventList
										bResultService = fnIoctlGetEventList(	pDevExtIn,
																				(uPNDEV_GET_EVENT_LIST_IN*)		pIn,
																				(uPNDEV_GET_EVENT_LIST_OUT*)	pOut);

										break;
									}
									case IOCTL_SET_EVENT_LIST:
									{
										// service SetEventList
										bResultService = fnIoctlSetEventList(	pDevExtIn,
																				(uPNDEV_SET_EVENT_LIST_IN*)		pIn,
																				(uPNDEV_SET_EVENT_LIST_OUT*)	pOut);

										break;
									}
									case IOCTL_CHANGE_INT:
									{
										// service ChangeInt
										bResultService = fnIoctlChangeInt(	pDevExtIn,
																			(uPNDEV_CHANGE_INT_IN*)		pIn,
																			(uPNDEV_CHANGE_INT_OUT*)	pOut);

										break;
									}
									case IOCTL_GET_DEVICE_STATE:
									{
										// service GetDeviceState
										bResultService = fnIoctlGetDeviceState(	pDevExtIn,
																				(uPNDEV_GET_DEVICE_STATE_IN*)	pIn,
																				(uPNDEV_GET_DEVICE_STATE_OUT*)	pOut);

										break;
									}


									case IOCTL_CHANGE_XHIF_PAGE:
									{
										// service ChangeXhifPage
										bResultService = fnIoctlChangeXhifPage(	pDevExtIn,
																				(uPNDEV_CHANGE_XHIF_PAGE_IN*)	pIn,
																				(uPNDEV_CHANGE_XHIF_PAGE_OUT*)	pOut);

										break;
									}
									case IOCTL_COPY_DATA:
									{
										// service CopyData
										bResultService = fnIoctlCopyData(	pDevExtIn,
																			(uPNDEV_COPY_DATA_IN*)	pIn,
																			(uPNDEV_COPY_DATA_OUT*)	pOut);

										break;
									}
									case IOCTL_START_BOOT_FW:
									{
										// service StartBootFw
										bResultService = fnIoctlStartBootFw(pDevExtIn,
																			(uPNDEV_START_BOOT_FW_IN*)	pIn,
																			(uPNDEV_START_BOOT_FW_OUT*)	pOut);

										break;
									}
									case IOCTL_START_USER_FW:
									{
										// service StartUserFw
										bResultService = fnIoctlStartUserFw(pDevExtIn,
																			(uPNDEV_START_USER_FW_IN*)	pIn,
																			(uPNDEV_START_USER_FW_OUT*)	pOut);

										break;
									}
									case IOCTL_START_USER_FW_LINUX:
									{
										// service StartUserFwLinux
										bResultService = fnIoctlStartUserFwLinux(pDevExtIn,
																				(uPNDEV_START_USER_FW_LINUX_IN*)	pIn,
																				(uPNDEV_START_USER_FW_LINUX_OUT*)	pOut);

										break;
									}
									case IOCTL_CHANGE_APP_LOCK:
									{
										// service ChangeAppLock
										bResultService = fnIoctlChangeAppLock(	pDevExtIn,
																				(uPNDEV_CHANGE_APP_LOCK_IN*)	pIn,
																				(uPNDEV_CHANGE_APP_LOCK_OUT*)	pOut);

										break;
									}
									case IOCTL_NOTIFY_DLL_STATE:
									{
										// service NotifyDllState
										bResultService = fnIoctlNotifyDllState(	pDevExtIn,
																				(uPNDEV_NOTIFY_DLL_STATE_IN*)	pIn,
																				(uPNDEV_NOTIFY_DLL_STATE_OUT*)	pOut);

										break;
									}
                                    case IOCTL_DO_NETPROFI:
		                            {
		                            	// execute service at PASSIVE_LEVEL (BufferedMode)
                                        bResultService =  fnIoctlDoNetProfi(pDevExtIn,
                                        									(uPNDEV_NP_DO_NETPROFI_REQ_IN*) 	pIn,
					                    						            (uPNDEV_NP_DO_NETPROFI_REQ_OUT*)	pOut);

                                        break;
		                            }
    								case IOCTL_MAP_MEMORY:
    								{
    									// service MapMemory
    									bResultService = fnIoctlMapMemory(	pDevExtIn,
    																		(uPNDEV_MAP_MEMORY_IN*)		pIn,
    																		(uPNDEV_MAP_MEMORY_OUT*)	pOut);

    									break;
    								}
    								case IOCTL_SET_HW_RES_UM:
    								{
    									// service MapMemory
    									bResultService = fnIoctlSetHwResUm(	pDevExtIn,
    																		(uPNDEV_OPEN_DEVICE_OUT*)	pIn,
    																		(uPNDEV_OPEN_DEVICE_OUT*)	pOut);

    									break;
    								}
    								case IOCTL_GET_INT_STATE:
    								{
    									// service GetIntState
    									bResultService = fnIoctlGetIntState(pDevExtIn,
																			(uPNDEV_GET_INT_STATE_IN*)	pIn,
																			(uPNDEV_GET_INT_STATE_OUT*)	pOut);

    									break;
    								}
									case IOCTL_IS_USER_FW_READY:
									{
										// service StartUserFw
										bResultService = fnIoctlIsUserFwReady(	pDevExtIn,
																				(uPNDEV_IS_USER_FW_READY_IN*)	pIn,
																				(uPNDEV_IS_USER_FW_READY_OUT*)	pOut);

										break;
									}
									case IOCTL_OPEN_TIMER:
									{
										// service OpenTimer
										bResultService = fnIoctlOpenTimer(	pDevExtIn,
																			(uPNDEV_OPEN_TIMER_IN*)		pIn,
																			(uPNDEV_OPEN_TIMER_OUT*)	pOut);

										break;
									}
									case IOCTL_CLOSE_TIMER:
									{
										// service CloseTimer
										bResultService = fnIoctlCloseTimer(	pDevExtIn,
																			(uPNDEV_CLOSE_TIMER_IN*)	pIn,
																			(uPNDEV_CLOSE_TIMER_OUT*)	pOut);

										break;
									}
									default:
									{
										break;
									}
								}

								if	(bResultService)
									// success
								{
									eResult = ePNDEV_IOCTL_OK;
								}

								#ifdef PNDEV_GET_RUNTIME_IOCTL
								{
								UINT32 lRuntime_Usec = 0;

									// compute Runtime in usec
									lRuntime_Usec = fnComputeRunTimeUsec(&uClockStart);

									// set TraceDetail
									RtlStringCchPrintfA(sTraceDetail,
														_countof(sTraceDetail),
														"PnDev[%s]: ++++ IOCTL 0x%x Runtime=%u usec",
														pDevExtIn->sPciLocShortAscii,
														lIoctlIn,
														lRuntime_Usec);

									// set trace
									fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
												ePNDEV_TRACE_CONTEXT_BASIC,
												FALSE,						// logging
												sTraceDetail);
								}
								#endif
							}
							// reset variable
							pDevExtIn->bIoctlRunning = FALSE;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_IOCTL);
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service OpenDevice
//************************************************************************

BOOLEAN fnIoctlOpenDevice(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_OPEN_DEVICE_IN*	pIn,
							uPNDEV_OPEN_DEVICE_OUT*	pOut)
{
BOOLEAN				bResult				= FALSE;
BOOLEAN				bMasterApp			= FALSE;
UINT32				lCtrFw				= 0;
BOOLEAN				bShmDebugFw			= FALSE;
UINT32				lTimeScaleFactor	= 0;
BOOLEAN				bReqHostMem			= FALSE;
BOOLEAN             bReqSharedHostMem   = FALSE;
uPNDEV_APP*			pApp				= NULL;
uPNDEV_THREAD_SYS   uThreadSys;
uPNDEV_THREAD_IOCTL	uThreadIoctl;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	&uThreadSys,
						sizeof(uPNDEV_THREAD_SYS));

		RtlZeroMemory(	&uThreadIoctl,
						sizeof(uPNDEV_THREAD_IOCTL));

		// copy input data
		{
			bMasterApp			= pIn->bMasterApp;
			lCtrFw				= pIn->uUserFw.lCtrFw;
			bShmDebugFw			= pIn->uUserFw.bShmDebug;
			lTimeScaleFactor	= pIn->lTimeScaleFactor;
            bReqHostMem         = pIn->bReqHostMem;
            bReqSharedHostMem   = pIn->bReqSharedHostMem;


			*((uPNDEV_THREAD_SYS*)   &uThreadSys)   = *((uPNDEV_THREAD_SYS*)   &pIn->uThreadSys);
			*((uPNDEV_THREAD_IOCTL*) &uThreadIoctl) = *((uPNDEV_THREAD_IOCTL*) &pIn->uInternal.uThreadIoctl);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_OPEN_DEVICE_OUT));
	}

	// set ScaleFactor for WaitTimes and TimeoutTimes
	{
		if	(lTimeScaleFactor == 0)
			// default value desired
		{
			// use original TimeValues
			pDevExtIn->lTimeScaleFactor = 1;
		}
		else
		{
			// stretch TimeValues by this factor
			pDevExtIn->lTimeScaleFactor = lTimeScaleFactor;
		}
	}

	// allocate AppObject
	pApp = fnAllocMemNonPaged(	sizeof(uPNDEV_APP),
								TAG('O','p','e','n'));

	if	(pApp == NULL)
		// error
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Insufficient memory for AppObject!"),
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// preset AppObject
		RtlZeroMemory(	pApp,
						sizeof(uPNDEV_APP));

		if 	(fnStartUpThreadResDll(	pDevExtIn,
									pApp,
									&uThreadIoctl,
									_countof(pOut->sError),
									pOut->sError))
			// starting up resources of DllThread ok
		{
		BOOLEAN bResultLock = TRUE;
		BOOLEAN bResulTmp 	= TRUE;

			// update AppList
			{
				//------------------------------------------------------------------------
				// enter CriticalSection
				fnEnterCritSec(	pDevExtIn,
								ePNDEV_CRIT_SEC_SYS_APP_LIST);
				{
					// put AppObject to AppList
					fnPutBlkToList(	&pDevExtIn->uListApp,
									((uLIST_HEADER*) pApp));

					pDevExtIn->lCtrApp++;
				}
				//------------------------------------------------------------------------
				// leave CriticalSection
				fnLeaveCritSec(	pDevExtIn,
								ePNDEV_CRIT_SEC_SYS_APP_LIST);
			}

			if	(pDevExtIn->lCtrApp == 1)
				// first OpenDevice
			{
				// enable BusMaster here
				bResulTmp = fnEnableBusMasterConfigSpace(	pDevExtIn,
															_countof(pOut->sError),
															pOut->sError);
				
				// preset DllState of first OpenDevice
				//	- lock OpenDevice for verifying environment and starting Fw at DLL afterwards
				//	- during this phase there must not be an OpenDevice of any other App
				pDevExtIn->bDllReadyFirstOpenDevice = FALSE;
			}
			else
			{
				if	(	!pDevExtIn->bDllReadyFirstOpenDevice
					&& 	!bShmDebugFw)
					// first OpenDevice not completed at DLL and not opened for shared memory debugging!
				{
					// error
					bResultLock = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): First OpenDevice not completed at DLL!"),
									NULL,
									_countof(pOut->sError),
									pOut->sError);
				}
			}

			if	(	bResultLock 
				&& 	bResulTmp)
				// success
			{
			BOOLEAN bResultMasterApp = TRUE;

				if	(bMasterApp)
					// MasterApp
				{
					if	(pDevExtIn->pMasterApp != NULL)
						// MasterApp already exist
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
						fnBuildString(	PNDEV_ERROR_MASTER_APP,
										NULL,
										NULL,
										_countof(pOut->sError),
										pOut->sError);
					}
					else if (!pDevExtIn->bIntConnected)
							// no LegIntConnected
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
						fnBuildString(	NULL,
										_TEXT("Connected Int for MasterApplicaton required (start Non-MasterApplication)!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else if (pDevExtIn->eBoard == ePNDEV_BOARD_TI_AM5728)
							// TI AM5728 board
					{
						// error
						bResultMasterApp = FALSE;

						// set ErrorString
						fnBuildString(	NULL,
										_TEXT("MasterApp for TI board not valid, because there is no interrupt handling supported (start Non-MasterApplication)!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else
					{
						// book IntHandling
						pDevExtIn->pMasterApp = pApp;
					}
				}

				if	(bResultMasterApp)
					// success
				{
				BOOLEAN bResultCleanHw = TRUE;

					if	(	(bMasterApp)
						||	(lCtrFw != 0))
						//		MasterApp required (EDDx needs clean hardware)
						// OR	starting of UserFw
					{
						if	(pDevExtIn->lCtrApp > 1)
							// not first OpenDevice
							//	- decision cannot be done at DLL because AppCtr is only consistent in driver context (service GetDeviceState returns only a copy!)
						{
							// Hw may not be in ResetState

							// error
							bResultCleanHw = FALSE;

							// set ErrorString
							fnBuildString(	NULL,
											_TEXT("CloseDevice of all other applications required (start of UserFw / MasterApp need a clean hardware by ResetDevice)!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);

							// set event 'ReqClose'
							{
								pDevExtIn->uEvent.bReqClose = TRUE;

								// check EventList
								//	- must be done here:
								//		SysThread is only started if service is ok!
								//		SysThread does not know the application which should be excluded!
								fnCheckEventList(	pDevExtIn,
													pApp,		// App excluded
													FALSE);		// not called by SysThread
							}
						}
					}

					if	(bResultCleanHw)
						// success
					{
						if	(pDevExtIn->lCtrApp == 1)
							// first OpenDevice
						{
							// Note:
							//	- don't try to read RemanentData if current board is not a known PcBoard!

							if	(pDevExtIn->bPcBoard)
								// PcBoard
							{
							_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

								if	(!fnBoard_GetRemanentData(	pDevExtIn,
																_countof(sError),
																sError))
									// error at getting RemanentData
								{
									// Note:
									//	- if there are flash problems OpenDevice cannot solve them
									//		-> returned error is only used for debug purpose

									// set trace by returned ErrorString
									fnSetTraceFctError(	pDevExtIn,
														sError);
								}
							}
						}

						// return RemanentData
						*((uPNCORE_REMANENT_DATA*) &pOut->uHwRes.uCommon.uRemanentData) = *((uPNCORE_REMANENT_DATA*) &pDevExtIn->uRemanentData);

						if	(!fnPnCore_GetIntPar(	pDevExtIn->eAsic,
													ePNCORE_ICU_HOST,
													&pOut->uCoreIf.uIntPar_IcuHost))
							// error at getting parameter of INT_CoreIf (HostCore-ICU)
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at fnPnCore_GetIntPar() [HostCore ICU]!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
						}
						else if	(!fnPnCore_GetIntPar(	pDevExtIn->eAsic,
														ePNCORE_ICU_ASIC,
														&pOut->uCoreIf.uIntPar_IcuAsic))
							// error at getting parameter of INT_CoreIf (AsicCore-ICU)
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at fnPnCore_GetIntPar() [AsicCore ICU]!"),
											NULL,
											_countof(pOut->sError),
											pOut->sError);
						}
						else
						{
							if	(fnBoard_GetHwInfo(	pDevExtIn,
													_countof(pOut->sError),
													&pOut->uHwInfo,
													pOut->sError))
								// getting HwInfo ok
							{
							BOOLEAN	bResultThreadSys	= FALSE;
							UINT64	lThreadId			= 0;

								if	(pDevExtIn->lCtrApp != 1)
									// not first OpenDevice
								{
									// success
									bResultThreadSys = TRUE;
								}
								else
								{
					            UINT32 lStackSize = 0;

									// no UserFw running
									pDevExtIn->bUserFwRunning = FALSE;

									// reset ThreadCtr
									pDevExtIn->lCtrThreadSys = 0;

									// set parameter of SysThread
									{
										if	(uThreadSys.lStackSize == 0)
											// default value desired
										{
											// 8kByte
											lStackSize = (1024*8);
										}
										else
										{
											lStackSize = uThreadSys.lStackSize;
										}

										if	(uThreadSys.lTimeCycle_10msec == 0)
											// default value desired
										{
											// 100msec
											pDevExtIn->lTimeCycleThreadSys_10msec = 10;
										}
										else
										{
											pDevExtIn->lTimeCycleThreadSys_10msec = uThreadSys.lTimeCycle_10msec;
										}
									}

									if	(fnStartUpThreadResSys(	pDevExtIn,
																uThreadSys.lPrio,
                                                                lStackSize,
                                                                uThreadSys.sName,
																_countof(pOut->sError),
																&lThreadId,
																pOut->sError))
										// starting up resources of periodic SysThread ok
									{
										// success
										bResultThreadSys = TRUE;
									}
								}

								if	(bResultThreadSys)
									// success
								{

                                    if(fnCheckSharedHostMemUM(	pApp,
																pDevExtIn,
                                    							bReqHostMem,
                                    							bReqSharedHostMem,
                                    							bMasterApp,
                                    							_countof(pOut->sError),
                                    							pOut->sError))
                                        // success
                                    {
                                    	if	(fnBoard_SetPtrMemMapUM(	pApp,
																		pDevExtIn,
                                    									_countof(pOut->sError),
                                    									&pOut->uInternal,
                                    									pOut->sError))
											// success
                                    	{
                                    		// success
                                    		bResult = TRUE;

                                    		// return
											{
												_RtlStringCchCopy(	pOut->sPciLocShort,
																	_countof(pOut->sPciLocShort),
																	pDevExtIn->sPciLocShort);

												// SblInfo
												*((uPNDEV_SBL_INFO*) &pOut->uSblInfo) = *((uPNDEV_SBL_INFO*) &pDevExtIn->uSblInfo);

												pOut->uThreadInfo.lIdSys		= lThreadId;
												pOut->uInternal.eIcu			= (UINT32) pDevExtIn->eIcu;
												pOut->uInternal.uAppSys.hHandle	= pApp;							// necessary for some services

												if	(pDevExtIn->lCtrApp == 1)
													// first OpenDevice
												{
													// return
													pOut->uInternal.bFirstOpenDevice = TRUE;
												}
											}
											/*
											if	(pDevExtIn->lCtrApp == 1)
												// first OpenDevice
											{
												// green LED=ON, red LED=OFF
												fnBoard_SetLed(	pDevExtIn,
																TRUE,
																FALSE);
											}
											 */
                                    	}
                                    }
								}
							}
						}
					}
				}
			}
		}

		if	(!bResult)
			// error
		{
		_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

			if	(!fnCloseDevice(pDevExtIn,
								pApp,
								_countof(sError),
								sError))
				// error at closing device
			{
				// set trace by returned ErrorString
				fnSetTraceFctError(	pDevExtIn,
									sError);
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service CloseDevice
//************************************************************************

BOOLEAN fnIoctlCloseDevice(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_CLOSE_DEVICE_IN*		pIn,
							uPNDEV_CLOSE_DEVICE_OUT*	pOut)
{
BOOLEAN		bResult		= FALSE;
uPNDEV_APP*	pApp		= NULL;
BOOLEAN		bDoAnyway	= FALSE;
BOOLEAN		bDoClose	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp		= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;
		bDoAnyway	= pIn->bDoAnyway;

		// preset OutputParameter
		//	- necessary because of METHOD_BUFFERED
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CLOSE_DEVICE_OUT));
	}

	// preset
	bDoClose = TRUE;

	if	(pApp->bStartedUserFw)
		// UserFw started by this App
	{
		// code cannot be done at DLL because AppCtr is only consistent in driver context (service GetDeviceState returns only a copy!)

		if	(pDevExtIn->lCtrApp > 1)
			// other App exist
		{
			if	(!bDoAnyway)
				// standard CloseDevice
			{
				bDoClose = FALSE;

				// Note:
				//	- internal ResetDevice is done at last CloseDevice

				// set ErrorString
				fnBuildString(	NULL,
								_TEXT("CloseDevice of all other applications required (stop of UserFw needs a ResetDevice)!"),
								NULL,
								_countof(pOut->sError),
								pOut->sError);

				// set event 'ReqClose'
				{
					pDevExtIn->uEvent.bReqClose = TRUE;

					// check EventList
					//	- must be done here:
					//		SysThread does not know the application which should be excluded!
					fnCheckEventList(	pDevExtIn,
										pApp,		// App excluded
										FALSE);		// not called by SysThread
				}
			}
		}
	}

	if	(bDoClose)
		// success
	{
		// set ptr to InternalOut
		{
		UINT32 i = 0;

			for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
			{
				// set ptr to InternalOut
				pOut->uInternal.uBarUm[i] = pApp->uHwResUm.uBar[i];
			}

			pOut->uInternal.uSdramUm = pApp->uHwResUm.uAsicSdramDirect;

			
			if(pApp->bUsesHostMem)
			{
				pOut->uInternal.uHostMemUm = g_uMemSys.uHostMem[pApp->lUsesHostMemIndex].uHwResUserSpace;
			}
			else if(pApp->bUsesSharedHostMem)
			{
			    pOut->uInternal.uHostMemUm = g_uMemSys.uSharedHostMem[pApp->lUsesSharedHostMemIndex].uHwResUserSpace;
			}
		}

		// close device
		bResult = fnCloseDevice(pDevExtIn,
								pApp,
								_countof(pOut->sError),
								pOut->sError);
/*
		if	(pDevExtIn->lCtrApp == 0)
			// last CloseDevice
		{
			// green LED=OFF, red LED=OFF
			fnBoard_SetLed(	pDevExtIn,
							FALSE,
							FALSE);
		}
*/
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ResetDevice
//************************************************************************

BOOLEAN fnIoctlResetDevice(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_RESET_DEVICE_IN*		pIn,
							uPNDEV_RESET_DEVICE_OUT*	pOut)
{
BOOLEAN		bResult		= FALSE;
BOOLEAN		bResultTmp	= FALSE;
uPNDEV_APP*	pApp		= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_RESET_DEVICE_OUT));
	}

	if	(pDevExtIn->lCtrApp == 1)
		// only one App exist
	{
		// success
		bResultTmp = TRUE;
	}
	else
	{
		if	(pDevExtIn->pMasterApp == pApp)
			// MasterApp
		{
			// success
			bResultTmp = TRUE;
		}
		else
		{
			// set ErrorString
			//	- changed by DLL to ReturnValue=ePNDEV_ERROR_MASTER_APP
			fnBuildString(	PNDEV_ERROR_MASTER_APP,
							NULL,
							NULL,
							_countof(pOut->sError),
							pOut->sError);
		}
	}

	if	(bResultTmp)
		// success
	{
		// reset board
		bResult = (*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																	ePNDEV_RESET_ACTION_REINIT_HW,
																	_countof(pOut->sError),
																	pOut->sError);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetEventList (get events by DLL)
//************************************************************************

BOOLEAN fnIoctlGetEventList(DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_GET_EVENT_LIST_IN*	pIn,
							uPNDEV_GET_EVENT_LIST_OUT*	pOut)
{
BOOLEAN		bResult	= FALSE;
uPNDEV_APP* pApp	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_GET_EVENT_LIST_OUT));
	}

	if	(pApp == NULL)
		// invalid AppObject
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Invalid AppObject!"));

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// success
		bResult = TRUE;

		//------------------------------------------------------------------------
		// enter CriticalSection
		fnEnterCritSec(	pDevExtIn,
						ePNDEV_CRIT_SEC_SYS_APP_LIST);
		{
			// copy events
			pOut->uEvent.lFatalFw		= pApp->uEvent.lFatalFw;
			pOut->uEvent.eFatalAsync 	= pApp->uEvent.eFatalAsync;
			pOut->uEvent.eWarning 		= pApp->uEvent.eWarning;
			pOut->uEvent.bReqClose 		= pApp->uEvent.bReqClose;
			pOut->uEvent.eDebug 		= pApp->uEvent.eDebug;

			// clear events at AppObject
			pApp->uEvent.lFatalFw		= 0;
			pApp->uEvent.eFatalAsync	= ePNDEV_FATAL_ASYNC__INVALID;
			pApp->uEvent.eWarning		= ePNDEV_WARNING__INVALID;
			pApp->uEvent.bReqClose		= FALSE;
			pApp->uEvent.eDebug			= ePNDEV_DEBUG__INVALID;
		}
		//------------------------------------------------------------------------
		// leave CriticalSection
		fnLeaveCritSec(	pDevExtIn,
						ePNDEV_CRIT_SEC_SYS_APP_LIST);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service SetEventList (set events by DLL)
//************************************************************************

BOOLEAN fnIoctlSetEventList(DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_SET_EVENT_LIST_IN*	pIn,
							uPNDEV_SET_EVENT_LIST_OUT*	pOut)
{
BOOLEAN			bResult	= FALSE;
uPNDEV_APP*		pApp	= NULL;
uPNDEV_EVENT	uEvent;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	&uEvent,
						sizeof(uPNDEV_EVENT));

		// copy input data
		{
			pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;

			*((uPNDEV_EVENT*) &uEvent) = *((uPNDEV_EVENT*) &pIn->uEvent);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_SET_EVENT_LIST_OUT));
	}

	if	(pApp == NULL)
		// invalid AppObject
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Invalid AppObject!"));

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// success
		bResult = TRUE;

		if	(uEvent.lFatalFw != 0)
			// event set
		{
			pDevExtIn->uEvent.lFatalFw = uEvent.lFatalFw;
		}

		if	(uEvent.eFatalAsync != ePNDEV_FATAL_ASYNC__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eFatalAsync = uEvent.eFatalAsync;
		}

		if	(uEvent.eWarning != ePNDEV_WARNING__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eWarning = uEvent.eWarning;
		}

		if	(uEvent.bReqClose)
			// event set
		{
			pDevExtIn->uEvent.bReqClose = uEvent.bReqClose;
		}

		if	(uEvent.eDebug != ePNDEV_DEBUG__INVALID)
			// event set
		{
			pDevExtIn->uEvent.eDebug = uEvent.eDebug;
		}

		// check EventList
		//	- do it here at once:
		//		currently only one event of each EventClass can be stored
		fnCheckEventList(	pDevExtIn,
							NULL,		// no App excluded
							FALSE);		// not called by SysThread
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeInt
//************************************************************************

BOOLEAN fnIoctlChangeInt(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_CHANGE_INT_IN*	pIn,
							uPNDEV_CHANGE_INT_OUT*	pOut)
{
BOOLEAN				bResult		= FALSE;
ePNDEV_INT_ACTION	eIntAction	= ePNDEV_INT_ACTION_INVALID;
BOOLEAN				bArrayIntUnmask[PNDEV_DIM_ARRAY_INT_SRC];
uPNDEV_INT_POLARITY	uIntPolarity;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		RtlZeroMemory(	bArrayIntUnmask,
						(sizeof(BOOLEAN) * PNDEV_DIM_ARRAY_INT_SRC));

		RtlZeroMemory(	&uIntPolarity,
						sizeof(uPNDEV_INT_POLARITY));

		// copy input data
		{
			eIntAction	= pIn->eIntAction;

			RtlCopyMemory(	bArrayIntUnmask,
							pIn->bArrayIntUnmask,
							(sizeof(BOOLEAN) * PNDEV_DIM_ARRAY_INT_SRC));

			*((uPNDEV_INT_POLARITY*) &uIntPolarity) = *((uPNDEV_INT_POLARITY*) &pIn->uIntPolarity);
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_INT_OUT));
	}

	// update IntAction
	pDevExtIn->eIntAction = eIntAction;

	// reset IntStatistic
	RtlZeroMemory(	&pDevExtIn->uIntStatistic,
					sizeof(uPNDEV_INT_STATISTIC));

	switch	(pDevExtIn->eIcu)
			// IcuType
	{


        case ePNDEV_ICU_IX1000:	// IX1000
		{
			break;
		}

		default:
		{
			break;
		}
	}

	if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
		// IntAction=Standard
	{
		// ePNDEV_INT_ACTION_UNMASK:
		//	- Asic with TopLevel-ICU:    interrupts will be unmasked by DLL afterwards
		//	- Asic without TopLevel-ICU: interrupts will be forwarded/filtered by DLL dependent on bArrayIntUnmask[]

		// success
		bResult = TRUE;
	}
	else	// IntAction=Test
	{
		if	(fnBoard_StartIntTest(	pDevExtIn,
									_countof(pOut->sError),
									pOut->sError))
			// starting IntTest ok
		{
			// success
			bResult = TRUE;

			// Note:
			//	- if BoardInt:	always unmasked -> ISR should be already called
			//	- else:			interrupt will be unmasked by DLL afterwards -> ISR not called up to now
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetDeviceState
//************************************************************************

BOOLEAN fnIoctlGetDeviceState(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_GET_DEVICE_STATE_IN*		pIn,
								uPNDEV_GET_DEVICE_STATE_OUT*	pOut)
{
BOOLEAN		bResult	= FALSE;
uPNDEV_APP* pApp	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp = (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_GET_DEVICE_STATE_OUT));
	}

	// success
	bResult = TRUE;

	// update OutputParameter of driver
	{
		if	(pDevExtIn->pMasterApp == pApp)
			// MasterApp
		{
			pOut->bMasterApp = TRUE;
		}

		pOut->lCtrApp				= pDevExtIn->lCtrApp;
		pOut->lCtrRef_XhifPage		= pDevExtIn->uUserPageXhif.lCtrRef;
		pOut->bUserFwRunning		= pDevExtIn->bUserFwRunning;
		pOut->uIntStatistic.lCtrDpc	= pDevExtIn->uIntStatistic.lCtrDpc;		// do it before lCtrIsr for ensuring that lCtrIsr>=lCtrDpc
		pOut->uIntStatistic.lCtrIsr	= pDevExtIn->uIntStatistic.lCtrIsr;
	}

	// to do at DLL
	//	- pOut->uIntStatistic.lCtrUisr

	return(bResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ProgramFlash
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeXhifPage
//************************************************************************

BOOLEAN fnIoctlChangeXhifPage(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_CHANGE_XHIF_PAGE_IN*		pIn,
								uPNDEV_CHANGE_XHIF_PAGE_OUT*	pOut)
{
BOOLEAN				bResult		= FALSE;
uPNDEV_APP*			pApp		= NULL;
ePNDEV_XHIF_PAGE	ePage		= ePNDEV_XHIF_PAGE_INVALID;
UINT32				lSegmentAhb	= 0;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp		= (uPNDEV_APP*) pIn->uInternal.uAppSys.hHandle;
		ePage		= pIn->ePage;
		lSegmentAhb	= pIn->lSegmentAhb;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT));
	}

	if	(pDevExtIn->pFnBoard_ChangePageXhif == NULL)
		// no support
	{
		// set ErrorString
		//	- changed by DLL to ReturnValue=ePNDEV_ERROR_SUPPORT
		fnBuildString(	PNDEV_ERROR_SUPPORT,
						NULL,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		// change dynamic UserPage
		bResult = (*((FN_BOARD_CHANGE_PAGE_XHIF) pDevExtIn->pFnBoard_ChangePageXhif))(	pDevExtIn,
																						pApp,
																						ePage,
																						lSegmentAhb,
																						_countof(pOut->sError),
																						&pOut->uHwDesc,
																						&pOut->lCtrRef,
																						pOut->sError);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service CopyData
//************************************************************************

BOOLEAN fnIoctlCopyData(DEVICE_EXTENSION*		pDevExtIn,
						uPNDEV_COPY_DATA_IN*	pIn,
						uPNDEV_COPY_DATA_OUT*	pOut)
{
BOOLEAN			bResult			= FALSE;
volatile UINT8* pHostDataBuf	= NULL;

	// preset OutputParameter
	//	- not necessary because of METHOD_DIRECT (driver OutputParameter (=uPNDEV_COPY_DATA_OUT) are already preset by DLL function fnDoEntryActions() )
	//	- user-specific data behind must not be overwritten!

	pHostDataBuf = ((UINT8*) pOut) + sizeof(uPNDEV_COPY_DATA_OUT);

	if	(!pDevExtIn->bCopyDataSupport)
		// Board don't support CopyData
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): Service CopyData not supported!"),
						NULL,
						_countof(pOut->sError),
						pOut->sError);

	}
	else	// success
	{
		switch	(pIn->eHwRes)
				// HwRes
		{
			case ePNDEV_COPY_HW_RES_SDRAM:
			{
			BOOLEAN			bPagedAccess	= FALSE;
			volatile UINT8*	pAsicSdramSeg0	= NULL;

				// success
				bResult = TRUE;

				// first check if a paged access to AsicSdram required

				//------------------------------------------------------------------------
				// start using SdramPage 0
				fnBoard_StartSdramPage(	pDevExtIn,
										0,
										&bPagedAccess,
										&pAsicSdramSeg0);
				{
					if	(!bPagedAccess)
						// non-paged access to AsicSdram
					{
						// AsicSdramPtr = pAsicSdramSeg0

						// copy data direct
						fnBoard_CopyDataDirect(	pIn,
												pHostDataBuf,
												pAsicSdramSeg0);
					}
				}
				//------------------------------------------------------------------------
				// stop using SdramPage
				fnBoard_StopSdramPage(pDevExtIn);

				if	(bPagedAccess)
					// paged access to AsicSdram
				{
					// copy data paged (AsicSdram)
					fnBoard_CopyDataPaged_Sdram(pDevExtIn,
												pIn,
												pHostDataBuf);
				}

				break;
			}
			case ePNDEV_COPY_HW_RES_PNIP_IRTE:
			{
			volatile UINT8* pPnipIrte = NULL;

				switch	(pDevExtIn->eAsic)
						// AsicType
				{
					case ePNDEV_ASIC_ERTEC400:		{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_ERTEC200:		{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_SOC1:			{pPnipIrte = pDevExtIn->pIrte;	break;}
					case ePNDEV_ASIC_ERTEC200P:		{pPnipIrte = pDevExtIn->pPnIp;	break;}
					default:
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid AsicType!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

						break;
					}
				}

				if	(pPnipIrte != NULL)
					// success
				{
					// success
					bResult = TRUE;

					// copy data direct
					fnBoard_CopyDataDirect(	pIn,
											pHostDataBuf,
											pPnipIrte);
				}

				break;
			}
			default:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid HwRes!"),
								NULL,
								_countof(pOut->sError),
								pOut->sError);

				break;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartBootFw
//************************************************************************

BOOLEAN fnIoctlStartBootFw(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_START_BOOT_FW_IN*	pIn,
							uPNDEV_START_BOOT_FW_OUT*	pOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bResultBoard	= FALSE;


	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_START_BOOT_FW_OUT));
	}

	// preset
	{
	BOOLEAN			bPagedAccess	= FALSE;
	volatile UINT8*	pAsicSdramSeg0	= NULL;

		//------------------------------------------------------------------------
		// start using SdramPage 0
		fnBoard_StartSdramPage(	pDevExtIn,
								0,
								&bPagedAccess,
								&pAsicSdramSeg0);
		{
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
								0xaaffffee);
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_IDENT),
								0xaaffffee);
			WRITE_REG_UINT32(	(pAsicSdramSeg0 + PNDEV_ASIC_SDRAM__OFFSET_VERSION),
								0xaaffffee);
		}
		//------------------------------------------------------------------------
		// stop using SdramPage
		fnBoard_StopSdramPage(pDevExtIn);
	}

	// Note:
	//	- BootFw copied to start of SharedAsicSdram_Indirect0 by DLL

	switch	(pDevExtIn->eBoard)
			// BoardType
	{

		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid BoardType!"),
							NULL,
							_countof(pOut->sError),
							pOut->sError);

			break;
		}
	}

	if	(bResultBoard)
		// success
	{
		// wait 50msec to ensure that startup of BootFw is really finished
		//	- PBL/SBL can only confirm request before jumping to BootFw
		//		-> wait necessary because startup of BootFw may initialized variables, no command before this is done!

		if	(fnDelayThread(	pDevExtIn,
							50,
							_countof(pOut->sError),
							pOut->sError))
			// starting WaitTime of current thread ok
		{
			// verify BootFw
			{
			BOOLEAN			bPagedAccess	= FALSE;
			volatile UINT8*	pAsicSdramSeg0	= NULL;

				//------------------------------------------------------------------------
				// start using SdramPage 0
				fnBoard_StartSdramPage(	pDevExtIn,
										0,
										&bPagedAccess,
										&pAsicSdramSeg0);
				{
					if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
														pAsicSdramSeg0,
														NULL,
														100,						// max. 100msec
														_countof(pOut->sError),
														pOut->sError))
						// checking SignOfLife of BootFw ok
					{
						if	(fnBoard_CheckVersionBootFw(pDevExtIn,
														pAsicSdramSeg0,
														_countof(pOut->sError),
														pOut->sError))
							// checking version of BootFw ok
						{
							// success
							bResult = TRUE;

							// BootFw running
							pDevExtIn->bBootFwRunning = TRUE;
						}
					}
				}
				//------------------------------------------------------------------------
				// stop using SdramPage
				fnBoard_StopSdramPage(pDevExtIn);
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartUserFw
//************************************************************************

BOOLEAN fnIoctlStartUserFw(	DEVICE_EXTENSION*			pDevExtIn,
							uPNDEV_START_USER_FW_IN*	pIn,
							uPNDEV_START_USER_FW_OUT*	pOut)
{
BOOLEAN						bResult							= FALSE;
uPNDEV_APP*					pApp							= NULL;
UINT32						lOffsetFwBuf					= 0;
UINT32						lIndexFw						= 0;
UINT32						lIndexFwMax						= 0;
BOOLEAN						bDebugMode						= FALSE;
BOOLEAN						bShmDebug						= FALSE;
UINT32						lTimeoutFwLoad_10msec			= 0;
UINT32						lTimeoutFwExe_10msec			= 0;
UINT32						lTimeoutFwStartupStd_10msec		= 0;
UINT32						lTimeoutFwStartupDebug_10msec	= 0;
UINT32						lTimeoutDebugger_10msec			= 0;
UINT32						lTimeWaitDebugger_10msec		= 0;
UINT32						lTimeStartupSignOfLife_10msec	= 0;
UINT32						lBatHostAddress					= 0;
UINT32						i								= 0;
BOOLEAN						bPagedAccess					= FALSE;
volatile UINT8*				pAsicSdramSeg0					= NULL;

uPNCORE_STARTUP_PAR_DRIVER	*puStartupParDriver;
uPNDEV_HW_DESC_SDRAM		*puAsicSdram_Org;

	puStartupParDriver = (uPNCORE_STARTUP_PAR_DRIVER*)fnAllocMemNonPaged(sizeof(uPNCORE_STARTUP_PAR_DRIVER), TAG('D', 'a', 't', 'a'));
	puAsicSdram_Org = (uPNDEV_HW_DESC_SDRAM*)fnAllocMemNonPaged(sizeof(uPNDEV_HW_DESC_SDRAM), TAG('D', 'a', 't', 'a'));

    if(!puStartupParDriver)
    {
        // error at kmalloc
    }
    else
    {
        if(!puAsicSdram_Org)
        {
            // error at kmalloc
        }
        else
        {
            RtlZeroMemory(	puStartupParDriver,
							sizeof(uPNCORE_STARTUP_PAR_DRIVER));
		
            RtlZeroMemory(	puAsicSdram_Org,
							sizeof(uPNDEV_HW_DESC_SDRAM));

			// Note:
			//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
			{
				// copy input data
				{
					pApp											= (uPNDEV_APP*) pIn->uAppSys.hHandle;
					lOffsetFwBuf									= pIn->lOffsetFwBuf;
					lIndexFw										= pIn->lIndexFw;
					lIndexFwMax										= pIn->lIndexFwMax;
					bDebugMode										= pIn->bDebugMode;
					bShmDebug										= pIn->bShmDebug;
					lTimeoutFwLoad_10msec							= pIn->lTimeoutFwLoad_10msec;
					lTimeoutFwExe_10msec							= pIn->lTimeoutFwExe_10msec;
					lTimeoutFwStartupStd_10msec						= pIn->lTimeoutFwStartupStd_10msec;
					lTimeoutFwStartupDebug_10msec					= pIn->lTimeoutFwStartupDebug_10msec;
					lTimeoutDebugger_10msec							= pIn->lTimeoutDebugger_10msec;
					lTimeWaitDebugger_10msec						= pIn->lTimeWaitDebugger_10msec;
					lTimeStartupSignOfLife_10msec					= pIn->lTimeStartupSignOfLife_10msec;
		            puStartupParDriver->lTimeUpdateSignOfLife_10msec= pIn->lTimeUpdateSignOfLife_10msec;
		            puStartupParDriver->bShmHaltOnStartup 			= pIn->bShmHaltOnStartup;
					lBatHostAddress									= pIn->lBatHostAddress;
		
					
					// copy PciMasterPar
		            *(&(puStartupParDriver->uPciMaster[0])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[0]);
		            *(&(puStartupParDriver->uPciMaster[1])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[1]);
		            *(&(puStartupParDriver->uPciMaster[2])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[2]);
		
					// copy uSharedHostSdram
		            if(pApp->bUsesHostMem)
		            {
		            	puStartupParDriver->uSharedHostSdram.lPhyAdr 	= g_uMemSys.uHostMem[pApp->lUsesHostMemIndex].lPhyAdr;
		            	puStartupParDriver->uSharedHostSdram.lSize	 	= g_uMemSys.uHostMem[pApp->lUsesHostMemIndex].uHwResUserSpace.lSize;
		            }
		            else if(pApp->bUsesSharedHostMem)
                    {
                        puStartupParDriver->uSharedHostSdram.lPhyAdr    = g_uMemSys.uSharedHostMem[pApp->lUsesSharedHostMemIndex].lPhyAdr;
                        puStartupParDriver->uSharedHostSdram.lSize      = g_uMemSys.uSharedHostMem[pApp->lUsesSharedHostMemIndex].uHwResUserSpace.lSize;
                    }
		
					for	(i=0; i < PNCORE_DIM_ARRAY_USER_DATA; i++)
						// dim array
					{
						// copy FwUserData
		                puStartupParDriver->lArrayUserData[i] = pIn->lArrayUserData[i];
					}
		
		            *((uPNDEV_HW_DESC_SDRAM*) puAsicSdram_Org) = *((uPNDEV_HW_DESC_SDRAM*) &pIn->uAsicSdram_Org);
				}
		
				// preset OutputParameter
				RtlZeroMemory(	pOut,
								sizeof(uPNDEV_START_USER_FW_OUT));
			}
		
			if	(lTimeoutFwLoad_10msec == 0)
				// default value desired
			{
				// 60sec (=60.000msec=6000*10msec)
				lTimeoutFwLoad_10msec = 6000;
			}
		
			if	(lTimeoutFwExe_10msec == 0)
				// default value desired
			{
				// 10sec (=10.000msec=1000*10msec)
				lTimeoutFwExe_10msec = 1000;
			}
		
			if	(lTimeoutFwStartupStd_10msec == 0)
				// default value desired
			{
				// 30sec (=30.000msec=3000*10msec)
				lTimeoutFwStartupStd_10msec = 3000;
			}
		
			if	(lTimeoutFwStartupDebug_10msec == 0)
				// default value desired
			{
				// 300sec (=300.000msec=30000*10msec)
				lTimeoutFwStartupDebug_10msec = 30000;
			}
		
			if	(lTimeoutDebugger_10msec == 0)
				// default value desired
			{
				// 300sec (=300.000msec=30000*10msec)
				lTimeoutDebugger_10msec = 30000;
			}
		
			if	(lTimeWaitDebugger_10msec == 0)
				// default value desired
			{
				// 10sec (=10000msec=1000*10msec)
				lTimeWaitDebugger_10msec = 1000;
			}
		
			if	(lTimeStartupSignOfLife_10msec == 0)
				// default value desired
			{
				// 1sec
				lTimeStartupSignOfLife_10msec = 100;
			}
		
		    if	(puStartupParDriver->lTimeUpdateSignOfLife_10msec == 0)
				// default value desired
			{
				// 5sec
		        puStartupParDriver->lTimeUpdateSignOfLife_10msec = 500;
			}
		
			//------------------------------------------------------------------------
			// start using SdramPage 0
			fnBoard_StartSdramPage(	pDevExtIn,
									0,
									&bPagedAccess,
									&pAsicSdramSeg0);
			{
			BOOLEAN				bResultAckDebugger	= FALSE;
			uPNCORE_CONFIG_PAR*	pConfigPar			= NULL;
		
				pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);
				
				// SharedAsicSdram_Config is already preset by ResetDevice
				
				if (pDevExtIn->eAsic == ePNDEV_ASIC_HERA)
					// Hera, ConfigArea needs Reset
				{
					RtlZeroMemory(	pConfigPar,
									sizeof(uPNCORE_CONFIG_PAR));			
				}
		
				// preset
				bResultAckDebugger = TRUE;
		
				if	(bDebugMode)
					// DebugMode
				{
					if	(lIndexFw == 0)
						// first downloading of UserFw after reset
					{
						if	(!fnBoard_WaitForAckDebugger(	pDevExtIn,
															pConfigPar,
															lTimeoutDebugger_10msec,
															lTimeWaitDebugger_10msec,
															_countof(pOut->sError),
															pOut->sError))
							// error at waiting for ack of debugger
						{
							bResultAckDebugger = FALSE;
						}
					}
				}
		
				if	(bResultAckDebugger)
					// success
				{
					if	(fnBoard_DoElfCmdLoadBootFw(pDevExtIn,
													pAsicSdramSeg0,
													pConfigPar,
													lOffsetFwBuf,
													lIndexFw,
													lIndexFwMax,
													lTimeoutFwLoad_10msec,
													_countof(pOut->sError),
													pOut->sError))
						// doing ElfCmd 'Load' of BootFw ok
					{
					uPNCORE_STARTUP_PAR_DRIVER* pStartupParDriver = NULL;
		
						if	(lIndexFw == lIndexFwMax)
							// MainFw
						{
		                    pStartupParDriver = puStartupParDriver;
		
							// copy BAT for 3.PciMaster
							pConfigPar->lBatHostAddress = lBatHostAddress;
		
							if	(pDevExtIn->eAsic == ePNDEV_ASIC_ERTEC400)
								// Ertec400
							{
								// set BridgeConfigReg
								pConfigPar->lOffsetBridgeConfigReg = PNCORE_ERTEC400_PCI_BRIDGE_REG__ADR;
							}
							else if	(pDevExtIn->eAsic == ePNDEV_ASIC_SOC1)
									// Soc1
							{
								// set BridgeConfigReg
								pConfigPar->lOffsetBridgeConfigReg = PNCORE_SOC1_PCI_BRIDGE_REG__ADR;
							}
		
							// copy RemanentData
		                    *((uPNCORE_REMANENT_DATA*) &(puStartupParDriver->uRemanentData)) = *((uPNCORE_REMANENT_DATA*) &pDevExtIn->uRemanentData);
		
							// set Boardtype
		
		
						}
		
						if	(fnBoard_DoElfCmdExecuteBootFw(	pDevExtIn,
															pAsicSdramSeg0,
															pConfigPar,
															(BOOLEAN)(	bDebugMode 
															||	bShmDebug),
															lIndexFw,
															lIndexFwMax,
															lTimeoutFwExe_10msec,
															lTimeStartupSignOfLife_10msec,
															pStartupParDriver,
															_countof(pOut->sError),
															pOut->sError))
							// doing ElfCmd 'Execute' of BootFw ok							
						{
						UINT32 lTimeoutFwStartup = 0;
		
							if	(	bDebugMode
								||	bShmDebug)
								// DebugMode or ShmDebug
							{
								lTimeoutFwStartup = lTimeoutFwStartupDebug_10msec;
							}
							else
							{
								lTimeoutFwStartup = lTimeoutFwStartupStd_10msec;
							}
		
							if	(lIndexFw < lIndexFwMax)
								// SetupFw
							{
								if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
																	pAsicSdramSeg0,
																	pConfigPar,
																	(lTimeoutFwStartup * 10),
																	_countof(pOut->sError),
																	pOut->sError))
									// checking SignOfLife of BootFw ok
								{
									// success
									bResult = TRUE;
								}
							}
							else	// MainFw
							{
								if	(fnBoard_WaitForStartupParUserFw(	pDevExtIn,
																		pConfigPar,
																		lTimeoutFwStartup,
																		_countof(pOut->sError),
																		pOut->sError))
									// waiting for StartupPar of UserFw ok
								{
									// preset SdramPar with current values
		                            *((uPNDEV_HW_DESC_SDRAM*) &pOut->uAsicSdram_Updated) = *((uPNDEV_HW_DESC_SDRAM*) puAsicSdram_Org);
		
									if	(fnBoard_UpdatePtrHwResUm_Sdram(&pConfigPar->uFw.uStartupPar,
																		_countof(pOut->sError),
																		&pOut->uAsicSdram_Updated,
																		pOut->sError))
										// updating SdramPar ok
									{
		
										if	(!bShmDebug)
										{
										
											if	(fnBoard_WaitForReadyUserFw(pDevExtIn,
																			pConfigPar,
																			lTimeoutFwStartup,
																			_countof(pOut->sError),
																			pOut->sError))
												// waiting for Ready of UserFw ok
											{
												if	(!bDebugMode)
													// not DebugMode
												{
													// start WD
												}
			
												// UserFw running
												//	- this variable activates some specific code of SysThread
												pDevExtIn->bUserFwRunning = TRUE;
			
												// activate watchdog
												//	- Note: partner may check it immediately after setting Ready
												{
													// set SignOfLife of driver
													fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
																			LSA_FALSE);			// called by driver
			
													// signal Ready of driver to Fw
													pConfigPar->uDriver.bReady = TRUE;
												}
			
												// UserFw started by this App
												pApp->bStartedUserFw = TRUE;
			
												// success
												bResult = TRUE;
											}
										}
									}
								}
							}
						}
					}
				}
			}
			//------------------------------------------------------------------------
			// stop using SdramPage
			fnBoard_StopSdramPage(pDevExtIn);
		
			if	(!bResult)
				// error
			{
				// Note:
				//	- if there is an error at downloading of UserFw then service CloseDevice is automatically called in context of service OpenDevice
				//		-> automatic BoardReset at last CloseDevice
				//		-> loaded UserFw will always be stopped
			}

			fnFreeMemNonPaged(puAsicSdram_Org, TAG('D', 'a', 't', 'a'));
        }
		
		fnFreeMemNonPaged(puStartupParDriver, TAG('D', 'a', 't', 'a'));
    }

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartUserFwLinux
//************************************************************************

BOOLEAN fnIoctlStartUserFwLinux(DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_START_USER_FW_LINUX_IN*	pIn,
								uPNDEV_START_USER_FW_LINUX_OUT*	pOut)
{
BOOLEAN						bResult							= FALSE;
uPNDEV_APP*					pApp							= NULL;
UINT32						lOffsetFwBuf					= 0;
UINT32						lIndexFw						= 0;
UINT32						lIndexFwMax						= 0;
BOOLEAN						bDebugMode						= FALSE;
BOOLEAN						bShmDebug						= FALSE;
UINT32						lTimeoutFwLoad_10msec			= 0;
UINT32						lTimeoutFwExe_10msec			= 0;
UINT32						lTimeoutFwStartupStd_10msec		= 0;
UINT32						lTimeoutFwStartupDebug_10msec	= 0;
UINT32						lTimeoutDebugger_10msec			= 0;
UINT32						lTimeWaitDebugger_10msec		= 0;
UINT32						lTimeStartupSignOfLife_10msec	= 0;
UINT32						lBatHostAddress					= 0;
UINT32						i								= 0;
BOOLEAN						bPagedAccess					= FALSE;
volatile UINT8*				pAsicSdramSeg0					= NULL;
uPNCORE_STARTUP_PAR_DRIVER	*puStartupParDriver;
uPNDEV_HW_DESC_SDRAM		*puAsicSdram_Org;

	puStartupParDriver	= (uPNCORE_STARTUP_PAR_DRIVER*)fnAllocMemNonPaged(sizeof(uPNCORE_STARTUP_PAR_DRIVER), TAG('D', 'a', 't', 'a'));
	puAsicSdram_Org		= (uPNDEV_HW_DESC_SDRAM*)fnAllocMemNonPaged(sizeof(uPNDEV_HW_DESC_SDRAM), TAG('D', 'a', 't', 'a'));

    if(!puStartupParDriver)
    {
        // error at kmalloc
    }
    else
    {
        if(!puAsicSdram_Org)
        {
            // error at kmalloc
        }
        else
        {
            RtlZeroMemory(	puStartupParDriver,
							sizeof(uPNCORE_STARTUP_PAR_DRIVER));
		
            RtlZeroMemory(	puAsicSdram_Org,
							sizeof(uPNDEV_HW_DESC_SDRAM));

			// Note:
			//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
			{
				// copy input data
				{
					pApp											= (uPNDEV_APP*) pIn->uAppSys.hHandle;
					lOffsetFwBuf									= pIn->lOffsetFwBuf;
					lIndexFw										= pIn->lIndexFw;
					lIndexFwMax										= pIn->lIndexFwMax;
					bDebugMode										= pIn->bDebugMode;
					bShmDebug										= pIn->bShmDebug;
					lTimeoutFwLoad_10msec							= pIn->lTimeoutFwLoad_10msec;
					lTimeoutFwExe_10msec							= pIn->lTimeoutFwExe_10msec;
					lTimeoutFwStartupStd_10msec						= pIn->lTimeoutFwStartupStd_10msec;
					lTimeoutFwStartupDebug_10msec					= pIn->lTimeoutFwStartupDebug_10msec;
					lTimeoutDebugger_10msec							= pIn->lTimeoutDebugger_10msec;
					lTimeWaitDebugger_10msec						= pIn->lTimeWaitDebugger_10msec;
					lTimeStartupSignOfLife_10msec					= pIn->lTimeStartupSignOfLife_10msec;
		            puStartupParDriver->lTimeUpdateSignOfLife_10msec= pIn->lTimeUpdateSignOfLife_10msec;
		            puStartupParDriver->bShmHaltOnStartup 			= pIn->bShmHaltOnStartup;
					lBatHostAddress									= pIn->lBatHostAddress;
		
					
					// copy PciMasterPar
		            *(&(puStartupParDriver->uPciMaster[0])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[0]);
		            *(&(puStartupParDriver->uPciMaster[1])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[1]);
		            *(&(puStartupParDriver->uPciMaster[2])) = *((uPNCORE_PCI_HOST_PAR*) &pIn->uPciMaster[2]);
		
					// copy uSharedHostSdram
		            if(pApp->bUsesHostMem)
		            {
                        puStartupParDriver->uSharedHostSdram.lPhyAdr 	= g_uMemSys.uHostMem[pApp->lUsesHostMemIndex].lPhyAdr;
                        puStartupParDriver->uSharedHostSdram.lSize	 	= g_uMemSys.uHostMem[pApp->lUsesHostMemIndex].uHwResUserSpace.lSize;
		            }
		            else if(pApp->bUsesSharedHostMem)
                    {
                        puStartupParDriver->uSharedHostSdram.lPhyAdr    = g_uMemSys.uSharedHostMem[pApp->lUsesSharedHostMemIndex].lPhyAdr;
                        puStartupParDriver->uSharedHostSdram.lSize      = g_uMemSys.uSharedHostMem[pApp->lUsesSharedHostMemIndex].uHwResUserSpace.lSize;
                    }
		
					for	(i=0; i < PNCORE_DIM_ARRAY_USER_DATA; i++)
						// dim array
					{
						// copy FwUserData
		                puStartupParDriver->lArrayUserData[i] = pIn->lArrayUserData[i];
					}
		
		            *((uPNDEV_HW_DESC_SDRAM*) puAsicSdram_Org) = *((uPNDEV_HW_DESC_SDRAM*) &pIn->uAsicSdram_Org);
				}
		
				// preset OutputParameter
				RtlZeroMemory(	pOut,
								sizeof(uPNDEV_START_USER_FW_OUT));
			}
		
			if	(lTimeoutFwLoad_10msec == 0)
				// default value desired
			{
				// 60sec (=60.000msec=6000*10msec)
				lTimeoutFwLoad_10msec = 6000;
			}
		
			if	(lTimeoutFwExe_10msec == 0)
				// default value desired
			{
				// 10sec (=10.000msec=1000*10msec)
				lTimeoutFwExe_10msec = 1000;
			}
		
			if	(lTimeoutFwStartupStd_10msec == 0)
				// default value desired
			{
				// 30sec (=30.000msec=3000*10msec)
				lTimeoutFwStartupStd_10msec = 3000;
			}
		
			if	(lTimeoutFwStartupDebug_10msec == 0)
				// default value desired
			{
				// 300sec (=300.000msec=30000*10msec)
				lTimeoutFwStartupDebug_10msec = 30000;
			}
		
			if	(lTimeoutDebugger_10msec == 0)
				// default value desired
			{
				// 300sec (=300.000msec=30000*10msec)
				lTimeoutDebugger_10msec = 30000;
			}
		
			if	(lTimeWaitDebugger_10msec == 0)
				// default value desired
			{
				// 10sec (=10000msec=1000*10msec)
				lTimeWaitDebugger_10msec = 1000;
			}
		
			if	(lTimeStartupSignOfLife_10msec == 0)
				// default value desired
			{
				// 1sec
				lTimeStartupSignOfLife_10msec = 100;
			}
		
		    if	(puStartupParDriver->lTimeUpdateSignOfLife_10msec == 0)
				// default value desired
			{
				// 5sec
		        puStartupParDriver->lTimeUpdateSignOfLife_10msec = 500;
			}
		
			//------------------------------------------------------------------------
			// start using SdramPage 0
			fnBoard_StartSdramPage(	pDevExtIn,
									0,
									&bPagedAccess,
									&pAsicSdramSeg0);
			{
			BOOLEAN						bResultAckDebugger	= FALSE;
			uPNCORE_CONFIG_PAR*			pConfigPar			= NULL;
			uPNCORE_STARTUP_PAR_DRIVER* pStartupParDriver = NULL;
			UINT32						lTimeoutFwStartup = 0;
		
				pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);		
		
				// preset
				bResultAckDebugger = TRUE;		
		
				// MainFw
		        pStartupParDriver = puStartupParDriver;
		
				// copy BAT for 3.PciMaster
				pConfigPar->lBatHostAddress = lBatHostAddress;		

				if	(pDevExtIn->eAsic == ePNDEV_ASIC_SOC1)
						// Soc1
				{
					// set BridgeConfigReg
					pConfigPar->lOffsetBridgeConfigReg = PNCORE_SOC1_PCI_BRIDGE_REG__ADR;
				}
		
				// copy RemanentData
		        *((uPNCORE_REMANENT_DATA*) &(puStartupParDriver->uRemanentData)) = *((uPNCORE_REMANENT_DATA*) &pDevExtIn->uRemanentData);
		
				if	(	bDebugMode
					||	bShmDebug)
					// DebugMode or ShmDebug
				{
					lTimeoutFwStartup = lTimeoutFwStartupDebug_10msec;
				}
				else
				{
					lTimeoutFwStartup = lTimeoutFwStartupStd_10msec;
				}

				// update DriverConfigPar
				//	- must be done before command 'Execute' because Fw needs info at startup (e.g. DebugMode)
				{
					pConfigPar->bDebugMode = bDebugMode;

					if (pStartupParDriver != NULL)
						// valid ptr
					{
						*((uPNCORE_STARTUP_PAR_DRIVER*)&pConfigPar->uDriver.uStartupPar) = *pStartupParDriver;
					}

					// set TimeStartupSignOfLife
					pConfigPar->lTimeStartupSignOfLife_10msec = lTimeStartupSignOfLife_10msec;
				}


				// -----------------------------------------------------------------------
				// the FW is loaded to the RAM and needs to be started

				switch (pDevExtIn->eBoard)
					// BoardType
				{

					default:
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid BoardType!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

						break;
					}
				}

				// -----------------------------------------------------------------------
				// the FW is started, now we wait until it is running

				if	(fnBoard_WaitForStartupParUserFw(	pDevExtIn,
														pConfigPar,
														lTimeoutFwStartup,
														_countof(pOut->sError),
														pOut->sError))
					// waiting for StartupPar of UserFw ok
				{
					// preset SdramPar with current values
		            *((uPNDEV_HW_DESC_SDRAM*) &pOut->uAsicSdram_Updated) = *((uPNDEV_HW_DESC_SDRAM*) puAsicSdram_Org);
		
					if	(fnBoard_UpdatePtrHwResUm_Sdram(&pConfigPar->uFw.uStartupPar,
														_countof(pOut->sError),
														&pOut->uAsicSdram_Updated,
														pOut->sError))
						// updating SdramPar ok
					{
		
						if	(!bShmDebug)
						{
										
							if	(fnBoard_WaitForReadyUserFw(pDevExtIn,
															pConfigPar,
															lTimeoutFwStartup,
															_countof(pOut->sError),
															pOut->sError))
								// waiting for Ready of UserFw ok
							{
								if	(!bDebugMode)
									// not DebugMode
								{
									// start WD
								}
			
								// UserFw running
								//	- this variable activates some specific code of SysThread
								pDevExtIn->bUserFwRunning = TRUE;
			
								// activate watchdog
								//	- Note: partner may check it immediately after setting Ready
								{
									// set SignOfLife of driver
									fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
															LSA_FALSE);			// called by driver
			
									// signal Ready of driver to Fw
									pConfigPar->uDriver.bReady = TRUE;
								}
			
								// UserFw started by this App
								pApp->bStartedUserFw = TRUE;
			
								// reconfigure the GPIOs
							}
						}
					}
				}
			}
			//------------------------------------------------------------------------
			// stop using SdramPage
			fnBoard_StopSdramPage(pDevExtIn);
		
			if	(!bResult)
				// error
			{
				// Note:
				//	- if there is an error at downloading of UserFw then service CloseDevice is automatically called in context of service OpenDevice
				//		-> automatic BoardReset at last CloseDevice
				//		-> loaded UserFw will always be stopped
			}

			fnFreeMemNonPaged(puAsicSdram_Org, TAG('D', 'a', 't', 'a'));
        }
		
		fnFreeMemNonPaged(puStartupParDriver, TAG('D', 'a', 't', 'a'));
    }

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service StartUserFw
//************************************************************************

BOOLEAN fnIoctlIsUserFwReady(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_IS_USER_FW_READY_IN*		pIn,
								uPNDEV_IS_USER_FW_READY_OUT*	pOut)
{
BOOLEAN			bResult			= FALSE;
uPNDEV_APP*		pApp			= NULL;
BOOLEAN			bPagedAccess	= FALSE;
volatile UINT8*	pAsicSdramSeg0	= NULL;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		{
			pApp = (uPNDEV_APP*) pIn->uAppSys.hHandle;
		}

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_IS_USER_FW_READY_OUT));
	}
	
	
	//------------------------------------------------------------------------
	// start using SdramPage 0
	fnBoard_StartSdramPage(	pDevExtIn,
							0,
							&bPagedAccess,
							&pAsicSdramSeg0);
	{
	uPNCORE_CONFIG_PAR*	pConfigPar = NULL;

		pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);
	
		if	(!pConfigPar->bCancel)
		{
			if	(pConfigPar->uFw.bReady)
				// waiting for Ready of UserFw ok
			{
				// UserFw running
				//	- this variable activates some specific code of SysThread
				pDevExtIn->bUserFwRunning = TRUE;
			
				// activate watchdog
				//	- Note: partner may check it immediately after setting Ready
				{
					// set SignOfLife of driver
					fnPnCore_SetSignOfLife(	pAsicSdramSeg0,
											LSA_FALSE);			// called by driver
			
					// signal Ready of driver to Fw
					pConfigPar->uDriver.bReady = TRUE;
				}
			
				// UserFw started by this App
				pApp->bStartedUserFw = TRUE;
				// Stop waiting for ready UserFw in service
				pOut->bIsUserFwReady = TRUE;
			}
			// success
			bResult = TRUE;
		}
	}
	//------------------------------------------------------------------------
	// stop using SdramPage
	fnBoard_StopSdramPage(pDevExtIn);

	return bResult;
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service ChangeAppLock
//************************************************************************

BOOLEAN fnIoctlChangeAppLock(	DEVICE_EXTENSION*			pDevExtIn,
								uPNDEV_CHANGE_APP_LOCK_IN*	pIn,
								uPNDEV_CHANGE_APP_LOCK_OUT*	pOut)
{
BOOLEAN		bResult			= FALSE;
uPNDEV_APP*	pApp			= NULL;
BOOLEAN		bLockMultiApp	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		pApp			= (uPNDEV_APP*) pIn->uAppSys.hHandle;
		bLockMultiApp	= pIn->bLockMultiApp;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_CHANGE_APP_LOCK_OUT));
	}

	if	(bLockMultiApp)
		// lock MultiApp
	{
		if	(pDevExtIn->lCtrApp > 1)
			// other App exist
		{
			// set ErrorString
			fnBuildString(	NULL,
							_TEXT("Internal service ChangeAppLock requires a CloseDevice of all other applications!"),
							NULL,
							_countof(pOut->sError),
							pOut->sError);

			// set event 'ReqClose'
			{
				pDevExtIn->uEvent.bReqClose = TRUE;

				// check EventList
				//	- must be done here:
				//		SysThread does not know the application which should be excluded!
				fnCheckEventList(	pDevExtIn,
									pApp,		// App excluded
									FALSE);		// not called by SysThread
			}
		}
		else
		{
			// success
			bResult = TRUE;

			// lock MultiApp
			pDevExtIn->bMultiAppLocked = TRUE;
		}
	}
	else
	{
		// success
		bResult = TRUE;

		// unlock MultiApp
		pDevExtIn->bMultiAppLocked = FALSE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service NotifyDllState
//************************************************************************

BOOLEAN fnIoctlNotifyDllState(	DEVICE_EXTENSION*				pDevExtIn,
								uPNDEV_NOTIFY_DLL_STATE_IN*		pIn,
								uPNDEV_NOTIFY_DLL_STATE_OUT*	pOut)
{
BOOLEAN bResult	= FALSE;
BOOLEAN bReady	= FALSE;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// copy input data
		bReady = pIn->bReady;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_NOTIFY_DLL_STATE_OUT));
	}

	// store DllState of first OpenDevice
	pDevExtIn->bDllReadyFirstOpenDevice = bReady;

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to corresponding Application
//************************************************************************

BOOLEAN fnIoctlMapMemory(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_MAP_MEMORY_IN*	pIn,
							uPNDEV_MAP_MEMORY_OUT*	pOut)
{
BOOLEAN bResult		= FALSE;
BOOLEAN	bResultMap	= TRUE;
UINT32	i			= 0;
uPNDEV_HW_RES_SINGLE_KM_TMP	uBarKm[6];
uPNDEV_HW_RES_SINGLE_KM_TMP	uSdramKm;
uPNDEV_HW_RES_SINGLE_KM_TMP	uHostMemKm;

	// Note:
	//	- METHOD_BUFFERED: memory of input buffer is used as output buffer!
	{
		// save input parameter
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			uBarKm[i] = pIn->uBarKm[i];
		}

		uSdramKm 	= pIn->uSdramKm;
		uHostMemKm 	= pIn->uHostMemKm;

		// preset OutputParameter
		RtlZeroMemory(	pOut,
						sizeof(uPNDEV_MAP_MEMORY_OUT));
	}

	// map Bars
	{
	
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
		UINT32 lSizeBar = uBarKm[i].lSize;

			if	(!fnMapMemVirtualToUser(pDevExtIn,
										((const void*) uBarKm[i].uVirtualAdr.pPtr),
										lSizeBar,
										_countof(pOut->sError),
										&pOut->uBarUm[i],
										pOut->sError))
				// error at mapping BARx to UserMode
			{
				bResultMap = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(bResultMap)
		// success
	{
		// map Sdram
		{
			if	(!fnMapMemVirtualToUser(pDevExtIn,
										(const void*) (uSdramKm.uVirtualAdr.pPtr),
										uSdramKm.lSize,
										_countof(pOut->sError),
										&pOut->uSdramUm,
										pOut->sError))
				// error at mapping SharedAsicSdram_Direct to UserMode
			{
				// error
				bResultMap = FALSE;
			}
		}

		if	(bResultMap)
			// success
		{
			// map SharedHostMemory
			{
				if	(uHostMemKm.uVirtualAdr.pPtr != NULL)
					// HostMem required
				{
					// map memory to userspace
					if	(!fnMapMemVirtualToUser(pDevExtIn,
												((const void*) (uHostMemKm.uVirtualAdr.pPtr)),
												uHostMemKm.lSize,
												_countof(pOut->sError),
												&pOut->uHostMemUm,
												pOut->sError))
						// mapping failed
					{
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Can not provide contiguous shared host memory for device: Failed mapping to UserSpace"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

					}
					else
					{
						// success
						bResult = TRUE;
					}
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to corresponding Application
//************************************************************************
BOOLEAN	fnIoctlSetHwResUm(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_OPEN_DEVICE_OUT*	pIn,
							uPNDEV_OPEN_DEVICE_OUT*	pOut)
{
BOOLEAN 	bResult = FALSE;
uPNDEV_APP*	pApp	= NULL;

	// copy InputBuffer to OutputBuffer
	memcpy_s(	pOut,
				sizeof(uPNDEV_OPEN_DEVICE_OUT),
				pIn,
				sizeof(uPNDEV_OPEN_DEVICE_OUT));

	pApp = (uPNDEV_APP*) pOut->uInternal.uAppSys.hHandle;

    if	(fnBoard_SetPtrHwResUm(	pDevExtIn,
    							_countof(pOut->sError),
    							&pOut->uInternal,
    							&pOut->uHwRes,
    							&pOut->uInternal.uIcu.pPtr,		// ptr to ICU
    							pApp,
    							pOut->sError))
    	// setting UserModePtr to HwRes ok
    {
    	// success
    	bResult = TRUE;
    }

	if	(!bResult)
		// error
	{
	_TCHAR sError[PNDEV_SIZE_STRING_BUF] = {0};

		if	(!fnCloseDevice(pDevExtIn,
							pApp,
							_countof(sError),
							sError))
			// error at closing device
		{
			// set trace by returned ErrorString
			fnSetTraceFctError(	pDevExtIn,
								sError);
		}
	}

	return(bResult);

}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetMsiIntVectors
//************************************************************************

BOOLEAN fnIoctlGetIntState(DEVICE_EXTENSION*			pDevExtIn,
						   uPNDEV_GET_INT_STATE_IN*		pIn,
						   uPNDEV_GET_INT_STATE_OUT*	pOut)
{
BOOLEAN bResult	= FALSE;

	// Note:
	//  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
	//  - OutputParameter must be preset!
	{
		// copy input data
		//  - no parameter
	
		// preset OutputParameter
		RtlZeroMemory(  pOut,
						sizeof(uPNDEV_ALLOC_EVENT_ID_OUT));
	}

	if	(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_INVALID)
		// invalid AppObject
	{
	_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set ErrorDetail
		_RtlStringCchPrintf(sErrorDetail,
							_countof(sErrorDetail),
							_TEXT("(): Invalid InterruptMode!"));

		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						sErrorDetail,
						NULL,
						_countof(pOut->sError),
						pOut->sError);
	}
	else
	{
		if	(	(pDevExtIn->eIntMode != ePNDEV_INTERRUPT_MODE_MSIX)
			&&	(pDevExtIn->eIntMode != ePNDEV_INTERRUPT_MODE_MSI))
			// invalid AppObject
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Invalid InterruptMode!"));

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							sErrorDetail,
							NULL,
							_countof(pOut->sError),
							pOut->sError);
		}
		else
		{
		UINT32 i = 0;
		
			// success
			bResult = TRUE;
	

			for	(i=0; i < PNDEV_MSIX_VECTOR_COUNT_MAX; i++)
				// all vectors
			{
				// acquire SpinLockIrq
				fnAcquireSpinLockIrq(pDevExtIn, i);
				//------------------------------------------------------------------------
				{
					// copy value
					pOut->bIntOccured[i] = pDevExtIn->bMsixIntOccured[i];
					
					// clear value
					pDevExtIn->bMsixIntOccured[i] = FALSE;
				}
				//------------------------------------------------------------------------			
				// release SpinLockIrq
				fnReleaseSpinLockIrq(pDevExtIn, i);
			}
		}
	}

	return(bResult);
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  check IoctlPar
//************************************************************************
ePNDEV_IOCTL fnCheckIoctlPar(	const UINT32		lIoctlIn,
								const UINT32		lSizeInputBufIn,
								const UINT32		lSizeOutputBufIn)
{
ePNDEV_IOCTL	eResult				= ePNDEV_IOCTL_INVALID;
BOOLEAN			bCompareEqual		= FALSE;
UINT32			lMinSizeInputBuf	= 0;
UINT32			lMinSizeOutputBuf	= 0;
char			sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
	// preset
	bCompareEqual = TRUE;

	switch	(lIoctlIn)
			// IoControlCode
	{
		case IOCTL_OPEN_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_OPEN_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			break;
		}
		case IOCTL_CLOSE_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CLOSE_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CLOSE_DEVICE_OUT);
			break;
		}
		case IOCTL_RESET_DEVICE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_RESET_DEVICE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_RESET_DEVICE_OUT);
			break;
		}
		case IOCTL_GET_EVENT_LIST:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_EVENT_LIST_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_EVENT_LIST_OUT);
			break;
		}
		case IOCTL_SET_EVENT_LIST:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_SET_EVENT_LIST_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_SET_EVENT_LIST_OUT);
			break;
		}
		case IOCTL_CHANGE_INT:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_INT_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_INT_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_STATE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_STATE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_STATE_OUT);
			break;
		}


		case IOCTL_PROGRAM_FLASH:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_PROGRAM_FLASH_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_PROGRAM_FLASH_OUT);
			break;
		}
		case IOCTL_CHANGE_XHIF_PAGE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_XHIF_PAGE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_XHIF_PAGE_OUT);
			break;
		}
		case IOCTL_COPY_DATA:
		{
			// size of output buffer may be larger
			bCompareEqual = FALSE;

			lMinSizeInputBuf	= sizeof(uPNDEV_COPY_DATA_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_COPY_DATA_OUT);
			break;
		}
		case IOCTL_START_BOOT_FW:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_START_BOOT_FW_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_START_BOOT_FW_OUT);
			break;
		}
		case IOCTL_START_USER_FW:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_START_USER_FW_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_START_USER_FW_OUT);
			break;
		}
		case IOCTL_START_USER_FW_LINUX:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_START_USER_FW_LINUX_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_START_USER_FW_LINUX_OUT);
			break;
		}
		case IOCTL_CHANGE_APP_LOCK:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CHANGE_APP_LOCK_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CHANGE_APP_LOCK_OUT);
			break;
		}
		case IOCTL_NOTIFY_DLL_STATE:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_NOTIFY_DLL_STATE_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_NOTIFY_DLL_STATE_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_INFO_SYS:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_INFO_SYS_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT);
			break;
		}
		case IOCTL_GET_DEVICE_HANDLE_SYS:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT);
			break;
		}
		case IOCTL_ALLOC_EVENT_ID:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_ALLOC_EVENT_ID_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_ALLOC_EVENT_ID_OUT);
			break;
		}
		case IOCTL_FREE_EVENT_ID:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_FREE_EVENT_ID_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_FREE_EVENT_ID_OUT);
			break;
		}
        case IOCTL_DO_NETPROFI:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_NP_DO_NETPROFI_REQ_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_NP_DO_NETPROFI_REQ_OUT);
			break;
		}
        case IOCTL_MAP_MEMORY:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_MAP_MEMORY_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_MAP_MEMORY_OUT);
			break;
		}
        case IOCTL_SET_HW_RES_UM:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			lMinSizeOutputBuf	= sizeof(uPNDEV_OPEN_DEVICE_OUT);
			break;
		}
        case IOCTL_GET_INT_STATE:
        {
        	lMinSizeInputBuf	= sizeof(uPNDEV_GET_INT_STATE_IN);
        	lMinSizeOutputBuf	= sizeof(uPNDEV_GET_INT_STATE_OUT);
        	break;        	
        }
        case IOCTL_IS_USER_FW_READY:
        {
			lMinSizeInputBuf	= sizeof(uPNDEV_IS_USER_FW_READY_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_IS_USER_FW_READY_OUT);
			break;
        }
		case IOCTL_OPEN_TIMER:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_OPEN_TIMER_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_OPEN_TIMER_OUT);
			break;
		}
		case IOCTL_CLOSE_TIMER:
		{
			lMinSizeInputBuf	= sizeof(uPNDEV_CLOSE_TIMER_IN);
			lMinSizeOutputBuf	= sizeof(uPNDEV_CLOSE_TIMER_OUT);
			break;
		}
		default:
		{
			break;
		}
	}

	if	(lMinSizeInputBuf == 0)
		// error
	{
		// return
		eResult = ePNDEV_IOCTL_ERROR_REQ;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ########## Invalid IOCTL request!");

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
	BOOLEAN bResultSize = FALSE;

		if	(bCompareEqual)
			// compare for equal size
		{
			if	(	(lSizeInputBufIn	== lMinSizeInputBuf)
				&&	(lSizeOutputBufIn	== lMinSizeOutputBuf))
			{
				// success
				bResultSize = TRUE;
			}
		}
		else
		{
			if	(	(lSizeInputBufIn	== lMinSizeInputBuf)
				&&	(lSizeOutputBufIn	>= lMinSizeOutputBuf))
			{
				// success
				bResultSize = TRUE;
			}
		}
	
		if	(!bResultSize)
			// error
		{
		    // return
			eResult = ePNDEV_IOCTL_ERROR_BUF_SIZE;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[x,x,x]: ########## Invalid size of IOCTL input or output buffer!");

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);
			
            

		}
		else
		{
			// success
			eResult = ePNDEV_IOCTL_OK;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  close device
//************************************************************************

BOOLEAN fnCloseDevice(				DEVICE_EXTENSION*	pDevExtIn,
					  PFD_MEM_ATTR	uPNDEV_APP*			pAppIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bResultXhif	= FALSE;

	if	(pDevExtIn->pMasterApp == pAppIn)
		// MasterApp
	{
		// Note:
		//	- interrupts are only unmasked at MasterApp

		// disable interrupts at board globally
		fnBoard_ChangeIntMaskGlobal(pDevExtIn,
									FALSE);
	}

	// preset
	bResultXhif = TRUE;

	// release XHIF page
	{
		if	(pDevExtIn->pFnBoard_ChangePageXhif != NULL)
			// dynamic UserPage exist
		{
			if	(!(*((FN_BOARD_CHANGE_PAGE_XHIF) pDevExtIn->pFnBoard_ChangePageXhif))(	pDevExtIn,
																						pAppIn,
																						ePNDEV_XHIF_PAGE_INVALID,	// release
																						0,
																						lDimErrorIn,
																						NULL,
																						NULL,
																						sErrorOut))
				// error at releasing dynamic UserPage
				//	- page can only be released by an App which allocated it
			{
				bResultXhif = FALSE;
			}
		}
	}

	if	(bResultXhif)
		// success
	{
	BOOLEAN bResultRes = TRUE;

		if	(pDevExtIn->lCtrApp == 1)
			// last CloseDevice
		{
			if (fnDisableBusMasterConfigSpace(	pDevExtIn,
												lDimErrorIn,
												sErrorOut))
				// success
			{
				if	(!fnShutDownThreadResSys(	pDevExtIn,
												lDimErrorIn,
												sErrorOut))
					// error at shutting down resources of periodic SysThread
				{
					bResultRes = FALSE;
				}
			}
			else
			{
				bResultRes = FALSE;
			}
		}

		if	(bResultRes)
			// success
		{
			if	(pAppIn == NULL)
				// AppObject not exist
			{
				// success
				bResult = TRUE;
			}
			else
			{
			BOOLEAN bResultBlk = TRUE;

				// update AppList
				{
					//------------------------------------------------------------------------
					// enter CriticalSection
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
					{
						if	(!fnBlkAtList(	&pDevExtIn->uListApp,
											((uLIST_HEADER*) pAppIn)))
							// AppObject not stored at AppList
						{
							// error
							bResultBlk = FALSE;

							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): AppObject not stored at AppList!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
						}
						else
						{
							// remove AppObject from AppList
							fnRemoveBlkFromList(&pDevExtIn->uListApp,
												((uLIST_HEADER*) pAppIn));

							pDevExtIn->lCtrApp--;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
				}

				if	(bResultBlk)
					// success
				{
					// Note:
					//	- before releasing any resources of AppObject it must be signed as invalid

					// release UserModePtr to HwRes
					{
					UINT32 i = 0;

						// Note:
						//	- caller must be in the context of the original process before calling MmUnmapLockedPages
						//	- this is because the unmapping operation occurs in the context of the calling process, and if the context is incorrect
						//	  the unmapping operation could delete the address range of a random process
						//	- solution:
						//		DLL provides handle to AppObject

						for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
						{
							// release memory mapped to UserMode
							fnReleaseMemUser(	pDevExtIn,
												&pAppIn->uHwResUm.uBar[i]);
						}

						// release memory mapped to UserMode
						fnReleaseMemUser(	pDevExtIn,
											&pAppIn->uHwResUm.uAsicSdramDirect);

                        // undo shared memory mapping
						fnReleaseSharedHostMemUM (	pDevExtIn,
													pAppIn,
													lDimErrorIn,
													sErrorOut);
					}

					if	(fnShutDownThreadResDll(pDevExtIn,
												pAppIn,
												lDimErrorIn,
												sErrorOut))
						// shutting down resources of DllThread ok
					{
						// free AppObject
						fnFreeMemNonPaged(	pAppIn,
											TAG('O','p','e','n'));

						if	(pDevExtIn->lCtrApp != 0)
							// not last CloseDevice
						{
							// success
							bResult = TRUE;
						}
						else
						{
							if	((*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																					ePNDEV_RESET_ACTION_REINIT_HW,
																					lDimErrorIn,
																					sErrorOut))
								// resetting board ok
							{
								// success
								bResult = TRUE;

								// BootFw/UserFw stopped if running
								pDevExtIn->bBootFwRunning	= FALSE;
								pDevExtIn->bUserFwRunning	= FALSE;
							}
						}

						if	(bResult)
							// success
						{
							if	(pDevExtIn->pMasterApp == pAppIn)
								// MasterApp
							{
								// release MasterApp
								pDevExtIn->pMasterApp = NULL;
							}
						}
					}
				}
			}
		}
	}

	return(bResult);
}

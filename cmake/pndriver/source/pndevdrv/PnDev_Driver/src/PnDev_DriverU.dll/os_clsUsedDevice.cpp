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
/*  F i l e               &F: os_clsUsedDevice.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	OS specific code of class "clsUsedDevice"
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
//	start up resources of ServiceThread / DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnStartUpThreadRes(	const UINT32	lPrioThreadIn,
											const UINT32	lStackSizeThreadIn,
											const _TCHAR*	sNameThreadIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultService	= FALSE;

	UNREFERENCED_PARAMETER(lStackSizeThreadIn);
	UNREFERENCED_PARAMETER(sNameThreadIn);

	// start up resources of unknown ServiceThread
	//	- PnDev_Driver services can be called at any thread!
	{
		// create EventConIntTest
		m_uThreadService.hEventConIntTest = CreateEvent(NULL,		// handle cannot be inherited
														FALSE,		// auto-reset event object
														FALSE,		// non signaled state
														NULL);		// unnamed object

		// create EventConStop
		m_uThreadService.hEventConStop = CreateEvent(	NULL,		// handle cannot be inherited
														FALSE,		// auto-reset event object
														FALSE,		// non signaled state
														NULL);		// unnamed object

		if	(	(m_uThreadService.hEventConIntTest	== NULL)
			||	(m_uThreadService.hEventConStop		== NULL))
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at creating events of ServiceThread!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResultService = TRUE;
		}
	}

	if	(bResultService)
		// success
	{
		// start up resources of DllThread
		{
			// create EventStop
			m_uThreadDll.hEventStop.hHandle			= CreateEvent(	NULL,		// handle cannot be inherited
																	FALSE,		// auto-reset event object
																	FALSE,		// non signaled state
																	NULL);		// unnamed object

			// create EventStopWithCon
			m_uThreadDll.hEventStopWithCon.hHandle	= CreateEvent(	NULL,		// handle cannot be inherited
																	FALSE,		// auto-reset event object
																	FALSE,		// non signaled state
																	NULL);		// unnamed object

			// create EventUinfo
			m_uThreadDll.hEventUinfo.hHandle		= CreateEvent(	NULL,		// handle cannot be inherited
																	FALSE,		// non manual-reset event object -> auto-reset
																	FALSE,		// non signaled state
																	NULL);		// name of the event object

			// create EventUisr
			//	- do it always independent if MasterApp
			//		(a NonMasterApp will never set it)
			m_uThreadDll.hEventUisr.hHandle			= CreateEvent(	NULL,		// handle cannot be inherited
																	FALSE,		// non manual-reset event object -> auto-reset
																	FALSE,		// non signaled state
																	NULL);		// name of the event object

			if	(	(m_uThreadDll.hEventStop.hHandle		== NULL)
				||	(m_uThreadDll.hEventStopWithCon.hHandle	== NULL)
				||	(m_uThreadDll.hEventUinfo.hHandle		== NULL)
				||	(m_uThreadDll.hEventUisr.hHandle		== NULL))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at creating events of DllThread!!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
			ULONG lThreadId = 0;

				// create DllThread
				m_uThreadDll.hThread.hHandle = CreateThread(NULL,							// handle cannot be inherited
															0,								// default stack size
															clsUsedDevice::fnThreadDll,		// function
															this,							// FunctionArg
															0,								// thread runs immediately after creation
															&lThreadId);					// ThreadId

				if	(m_uThreadDll.hThread.hHandle == NULL)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Error at creating DllThread!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					if	(fnSetThreadPrio(	m_uThreadDll.hThread.hHandle,
											lPrioThreadIn,
											PNDEV_THREAD_POLICY,
											lDimErrorIn,
											sErrorOut))
						// setting ThreadPrio ok
					{
						// success
						bResult = TRUE;
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
BOOLEAN	clsUsedDevice::fnStartUpThreadRes(	const UINT32	lPrioThreadIn,
											const UINT32	lStackSizeThreadIn,
											const _TCHAR*	sNameThreadIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultService	= FALSE;

	// start up resources of unknown ServiceThread
	//	- PnDev_Driver services can be called at any thread!
	{
		if	(	(fmq_init(&m_uFmqConIntTest)	!= ADN_OK)
			||	(fmq_init(&m_uFmqConStop)		!= ADN_OK))
			// error at initializing FMQs
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at fmq_init()!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResultService = TRUE;

			// set handle of EventConIntTest
			m_uThreadService.hEventConIntTest = (HANDLE) &m_uFmqConIntTest;

			// set handle of EventConStop
			m_uThreadService.hEventConStop = (HANDLE) &m_uFmqConStop;
		}
	}

	if	(bResultService)
		// success
	{
	int32_t			lResultTmp = ADN_ERROR;
	pthread_attr_t	uThreadPar;

		if	(fnSetThreadPar(lPrioThreadIn,
							lStackSizeThreadIn,
							sNameThreadIn,
							lDimErrorIn,
							&uThreadPar,
							sErrorOut))
			// setting ThreadPar ok
		{
			// create DLLThread
			//	- set ThreadHandle
			lResultTmp = pthread_create((pthread_t*) &m_uThreadDll.hThread,     // created ThreadId
										&uThreadPar,							// ThreadPar
										clsUsedDevice::fnThreadDll,				// function
										this);									// FunctionArg

			if	(lResultTmp != ADN_OK)
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at creating DllThread!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// start up resources of DllThread
				{
					// set handle of EventStop
					m_uThreadDll.hEventStop.lUint64 = ePNDEV_EVENT_THREAD_DLL_STOP;

					// set handle of EventStopWithCon
					m_uThreadDll.hEventStopWithCon.lUint64 = ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON;

					// set handle of EventUinfoUm
					m_uThreadDll.hEventUinfoUm.lUint64 = ePNDEV_EVENT_THREAD_DLL_UINFO;

					// create EventUinfo and EventUisr
					{
						if	(fnCreateEventSys(	ePNDEV_EVENT_THREAD_DLL_UINFO,
												lDimErrorIn,
												&m_uThreadDll.hEventUinfo,
												sErrorOut))
							// creating EventUinfo ok
						{
							if	(fnCreateEventSys(	ePNDEV_EVENT_THREAD_DLL_UISR,
													lDimErrorIn,
													&m_uThreadDll.hEventUisr,
													sErrorOut))
								// creating EventUisr ok
								//	- do it always independent if MasterApp
								//		(a NonMasterApp will never set it)
							{
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
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnStartUpThreadRes(	const UINT32	lPrioThreadIn,
											const UINT32	lStackSizeThreadIn,
											const _TCHAR*	sNameThreadIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultService	= FALSE;

	// start up resources of unknown ServiceThread
	//	- PnDev_Driver services can be called at any thread!
	{
		m_uEventConIntTest = eventfd(0,0);
		m_uEventConStop 	= eventfd(0,0);

		if	(	(m_uEventConIntTest	< 0)
			||	(m_uEventConStop		< 0))
			// error at initializing message queues
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at eventfd()!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResultService = TRUE;

			// set handle of EventConIntTest
			m_uThreadService.hEventConIntTest = (HANDLE) &m_uEventConIntTest;

			// set handle of EventConStop
			m_uThreadService.hEventConStop = (HANDLE) &m_uEventConStop;
		}
	}

	if	(bResultService)
		// success
	{
	int32_t			lResultTmp = LINUX_ERROR;
	pthread_attr_t	uThreadPar;

		if	(fnSetThreadPar(lPrioThreadIn,
							lStackSizeThreadIn,
							sNameThreadIn,
							lDimErrorIn,
							&uThreadPar,
							sErrorOut))
			// setting ThreadPar ok
		{
			// start up resources of DllThread

			// create Eventfds and remember maxEventFD for select
			m_uMaxEventFD = 	(INT32)(m_uThreadDll.hEventStop.lUint64 		= eventfd(0,0));
			if (m_uMaxEventFD < (INT32)(m_uThreadDll.hEventStopWithCon.lUint64	= eventfd(0,0)))
			{
				m_uMaxEventFD = (INT32)m_uThreadDll.hEventStopWithCon.lUint64;
			}
			if (m_uMaxEventFD < (INT32)(m_uThreadDll.hEventUinfo.lUint64		= eventfd(0,0)))
			{
				m_uMaxEventFD = (INT32)m_uThreadDll.hEventUinfo.lUint64;
			}
			if (m_uMaxEventFD < (INT32)(m_uThreadDll.hEventUisr.lUint64			= eventfd(0,0)))
			{
				m_uMaxEventFD = (INT32)m_uThreadDll.hEventUisr.lUint64;
			}
			if	(	((INT32)m_uThreadDll.hEventStop.lUint64 		!= (INT32) -1) &&
					((INT32)m_uThreadDll.hEventStopWithCon.lUint64	!= (INT32) -1) &&
					((INT32)m_uThreadDll.hEventUinfo.lUint64 		!= (INT32) -1) &&
					((INT32)m_uThreadDll.hEventUisr.lUint64			!= (INT32) -1))
				// creating eventfds ok
			{
				// create DLLThread
				//	- set ThreadHandle
				lResultTmp = pthread_create((pthread_t*) &m_uThreadDll.hThread,     // created ThreadId
											&uThreadPar,							// ThreadPar
											clsUsedDevice::fnThreadDll,				// function
											this);									// FunctionArg

				if	(lResultTmp != LINUX_OK)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Error at creating DllThread!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// Set thread name in Linux after thread creation (otherwise thread name is inherited)
					if( strlen(sNameThreadIn) > 0 && 
                        strlen(sNameThreadIn) < 16)
					    // valid user thread name
					{
						(void)pthread_setname_np(*((pthread_t*) &m_uThreadDll.hThread), sNameThreadIn);
					}
					else
					    // no/invalid user thread name, name thread "PNDEVDRV_DLL"
					{
						(void)pthread_setname_np(*((pthread_t*) &m_uThreadDll.hThread), "PNDEVDRV_DLL");
					}
					// success
					bResult = TRUE;
				}
			}
			else
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at eventfd()!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
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
//	shut down resources of ServiceThread / DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnShutDownThreadRes(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	// shut down resources of ServiceThread
	{
		CloseHandle(m_uThreadService.hEventConIntTest);
		CloseHandle(m_uThreadService.hEventConStop);

		m_uThreadService.hEventConIntTest	= NULL;
		m_uThreadService.hEventConStop		= NULL;
	}

	// shut down resources of DllThread
	{
		CloseHandle(m_uThreadDll.hEventStop.hHandle);
		CloseHandle(m_uThreadDll.hEventStopWithCon.hHandle);
		CloseHandle(m_uThreadDll.hEventUinfo.hHandle);
		CloseHandle(m_uThreadDll.hEventUisr.hHandle);

		m_uThreadDll.hEventStop.hHandle			= NULL;
		m_uThreadDll.hEventStopWithCon.hHandle	= NULL;
		m_uThreadDll.hEventUinfo.hHandle		= NULL;
		m_uThreadDll.hEventUisr.hHandle			= NULL;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnShutDownThreadRes(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult 		= FALSE;
BOOLEAN bResultService	= FALSE;

	// shut down resources of ServiceThread
	{
		if	(	(fmq_destroy(&m_uFmqConIntTest)	!= ADN_OK)
			||	(fmq_destroy(&m_uFmqConStop)	!= ADN_OK))
			// error at deleting FMQs
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at deleting FMQs!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResultService = TRUE;

			m_uThreadService.hEventConStop		= NULL;
			m_uThreadService.hEventConIntTest	= NULL;
		}
	}

	if	(bResultService)
		// success
	{
		// shut down resources of DllThread
		{
		int32_t	lResultTmp1	= ADN_ERROR;
		int32_t	lResultTmp2	= ADN_ERROR;

			// delete events
			lResultTmp1	= event_delete(	(int32_t) m_pDataSet->fnGetHandleDriverSys(),
										(eventid_t) m_uThreadDll.hEventUinfo.lUint64);
			lResultTmp2	= event_delete(	(int32_t) m_pDataSet->fnGetHandleDriverSys(),
										(eventid_t)m_uThreadDll.hEventUisr.lUint64);

			if	(	(lResultTmp1 != ADN_OK)
				||	(lResultTmp2 != ADN_OK))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at deleting event of DllThread!!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
			BOOLEAN	bResult1	= FALSE;
			BOOLEAN	bResult2	= FALSE;

				// free EventId by SYS
				bResult1 = fnFreeEventId(	(UINT32) m_uThreadDll.hEventUinfo.lUint64,
											lDimErrorIn,
											sErrorOut);
				bResult2 = fnFreeEventId(	(UINT32) m_uThreadDll.hEventUisr.lUint64,
											lDimErrorIn,
											sErrorOut);

				if	(	(bResult1)
					&&	(bResult2))
					// success
				{
					// success
					bResult = TRUE;

					m_uThreadDll.hEventStop.hHandle			= NULL;
					m_uThreadDll.hEventStopWithCon.hHandle	= NULL;
					m_uThreadDll.hEventUinfo.hHandle		= NULL;
					m_uThreadDll.hEventUisr.hHandle			= NULL;
					m_uThreadDll.hEventUinfoUm.hHandle		= NULL;
				}
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnShutDownThreadRes(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult 		= FALSE;
BOOLEAN bResultService	= FALSE;

	// shut down resources of ServiceThread
	{

		if	(	(close(m_uEventConIntTest)	!= LINUX_OK)
			||	(close(m_uEventConStop)	!= LINUX_OK))
			// error at deleting FMQs
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at closing events!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResultService = TRUE;

			m_uThreadService.hEventConStop		= NULL;
			m_uThreadService.hEventConIntTest	= NULL;
		}

	}

	if	(bResultService)
		// success
	{
		// shut down resources of DllThread
		{
			// delete eventfd
			if	(	(		((INT32)m_uThreadDll.hEventStop.lUint64 		!= (INT32) -1) &&
					(close(	(INT32)m_uThreadDll.hEventStop.lUint64) 		!= LINUX_OK)) ||
					(		((INT32)m_uThreadDll.hEventStopWithCon.lUint64	!= (INT32) -1) &&
					(close(	(INT32)m_uThreadDll.hEventStopWithCon.lUint64)	!= LINUX_OK)) ||
					(		((INT32)m_uThreadDll.hEventUinfo.lUint64 		!= (INT32) -1) &&
					(close(	(INT32)m_uThreadDll.hEventUinfo.lUint64) 		!= LINUX_OK)) ||
					(		((INT32)m_uThreadDll.hEventUisr.lUint64			!= (INT32) -1) &&
					(close(	(INT32)m_uThreadDll.hEventUisr.lUint64) 		!= LINUX_OK)))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at closing events of DllThread!!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				m_uThreadDll.hEventStop.lUint64				= (INT32) -1;
				m_uThreadDll.hEventStopWithCon.lUint64		= (INT32) -1;
				m_uThreadDll.hEventUinfo.lUint64			= (INT32) -1;
				m_uThreadDll.hEventUisr.lUint64				= (INT32) -1;

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
//	set event of DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnSetEventThreadDll(	ePNDEV_EVENT_THREAD_DLL	eEventIn,
											const UINT32			lDimErrorIn,
											_TCHAR*					sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hEvent	= NULL;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_DLL_STOP:			{hEvent = m_uThreadDll.hEventStop.hHandle;			break;}
		case ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON:	{hEvent = m_uThreadDll.hEventStopWithCon.hHandle;	break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEvent != NULL)
		// success
	{
		bResult = TRUE;

		// set event
		SetEvent(hEvent);
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnSetEventThreadDll(	ePNDEV_EVENT_THREAD_DLL	eEventIn,
											const UINT32			lDimErrorIn,
											_TCHAR*					sErrorOut)
{
BOOLEAN	bResult	= FALSE;
INT32	lEvent	= -1;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_DLL_STOP:			{lEvent = (INT32) m_uThreadDll.hEventStop.lUint64;			break;}
		case ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON:	{lEvent = (INT32) m_uThreadDll.hEventStopWithCon.lUint64;	break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(lEvent != -1)
		// success
	{
	int32_t lResultTmp = ADN_ERROR;

		// set event
		lResultTmp = pthread_kill(	(pthread_t)	m_uThreadDll.hThread.lUint64,
												lEvent);

		if	(lResultTmp != ADN_OK)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at pthread_kill()!"),
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
BOOLEAN	clsUsedDevice::fnSetEventThreadDll(	ePNDEV_EVENT_THREAD_DLL	eEventIn,
											const UINT32			lDimErrorIn,
											_TCHAR*					sErrorOut)
{
BOOLEAN	bResult	= FALSE;
INT32	lEvent	= -1;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_DLL_STOP:			{lEvent = (INT32) m_uThreadDll.hEventStop.lUint64;			break;}
		case ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON:	{lEvent = (INT32) m_uThreadDll.hEventStopWithCon.lUint64;	break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(lEvent != -1)
		// success
	{
	ssize_t lWriteSize	= 0;
	uint64_t lSignal 	= eEventIn;

		// set event
		lWriteSize = write(lEvent, &lSignal, sizeof(uint64_t));
		if	(lWriteSize != sizeof(uint64_t))
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at setting event !"),
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
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//	set event of ServiceThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE	eEventIn,
												const UINT32				lDimErrorIn,
												_TCHAR*						sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hEvent	= NULL;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hEvent = m_uThreadService.hEventConIntTest;	break;}
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEvent = m_uThreadService.hEventConStop;		break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEvent != NULL)
		// success
	{
		bResult = TRUE;

		// set event
		SetEvent(hEvent);
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE	eEventIn,
												const UINT32				lDimErrorIn,
												_TCHAR*						sErrorOut)
{
BOOLEAN			bResult		= FALSE;
uPNDEV_FMQ_MSG*	pFmqMsg		= NULL;

	// allocate memory for FmqMsg
	//	- free at fnWaitForEventThreadService()
	pFmqMsg = new uPNDEV_FMQ_MSG;

	if	(pFmqMsg == NULL)
		// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Insufficient memory for FmqMsg!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	HANDLE hFmq = NULL;

		// set FmqMsg
		pFmqMsg->eEvent = eEventIn;

		switch	(eEventIn)
				// event
		{
			case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hFmq = m_uThreadService.hEventConIntTest;	break;}
			case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hFmq = m_uThreadService.hEventConStop;		break;}
			default:
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}

		if	(hFmq != NULL)
			// success
		{
		int32_t lResultTmp = ADN_ERROR;

			// send FmqMsg
			//	- there are FMQs with index 0..(FMQ_PRIO_MAX - 1)
			//	- all priorities > (FMQ_PRIO_MAX - 1) are reduced to (FMQ_PRIO_MAX - 1)
			lResultTmp = fmq_send(	(fmq_t*) hFmq,
									pFmqMsg,
									FMQ_PRIO_MAX - 1);

			if	(lResultTmp != ADN_OK)
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at fmq_send()!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// return
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE	eEventIn,
												const UINT32				lDimErrorIn,
												_TCHAR*						sErrorOut)
{
BOOLEAN			bResult	= FALSE;
INT32* 			hEvent 	= NULL;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hEvent = (INT32*)m_uThreadService.hEventConIntTest;	break;}
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEvent = (INT32*)m_uThreadService.hEventConStop;		break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEvent != NULL)
		// success
	{
	ssize_t lWriteSize	= 0;
	uint64_t lSignal 	= eEventIn;

		// set event
		lWriteSize = write((*hEvent), &lSignal, sizeof(uint64_t));
        if	(lWriteSize != sizeof(uint64_t))
			// error
		{

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at setting event !"),
										NULL,
										lDimErrorIn,
										sErrorOut);

		}
		else
		{
			// return
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
//	wait for SingleEvent at ServiceThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
													const UINT32					lDimErrorIn,
													_TCHAR*							sErrorOut)
{
BOOLEAN	bResult		= FALSE;
HANDLE	hEventWait	= NULL;
UINT32	lTimeout	= 0;

	#ifdef _DEBUG
		// timeout = 30sec (for enabling debugging)
		lTimeout = 30*1000;
	#else
		// timeout = 5sec
		lTimeout = 5*1000;
	#endif

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hEventWait = m_uThreadService.hEventConIntTest;	break;}
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEventWait = m_uThreadService.hEventConStop;		break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid desired event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEventWait != NULL)
		// success
	{
	UINT32 lEventSignaled = 0;

		// wait for desired event
		lEventSignaled = WaitForSingleObject(	hEventWait,		// object to wait on
												lTimeout);		// timeout (INFINITE = no timeout)

		if	(lEventSignaled != WAIT_OBJECT_0)
			// error
		{
			if	(lEventSignaled == WAIT_TIMEOUT)
				// Timeout
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Timeout at waiting for event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Other error than timeout at waiting for event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
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
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
													const UINT32					lDimErrorIn,
													_TCHAR*							sErrorOut)
{
BOOLEAN			bResult		= FALSE;
HANDLE			hEventWait	= NULL;
struct timespec	uTimeSpec;

	#ifdef _DEBUG
		// timeout = 30sec (for enabling debugging)
		uTimeSpec.tv_sec	= 30;
		uTimeSpec.tv_nsec	= 0;
	#else
		// timeout = 5sec
		uTimeSpec.tv_sec	= 5;
		uTimeSpec.tv_nsec	= 0;
	#endif

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hEventWait = m_uThreadService.hEventConIntTest;	break;}
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEventWait = m_uThreadService.hEventConStop;		break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEventWait != NULL)
		// success
	{
	int32_t			lResultTmp	= ADN_ERROR;
	uPNDEV_FMQ_MSG*	pFmqMsg		= NULL;
	UINT32			lPrioRcve	= 0;

		// wait for desired event
		lResultTmp = fmq_reltimedreceive(	(fmq_t*) hEventWait,	// object to wait on
											(void**) &pFmqMsg,
											&lPrioRcve,
											&uTimeSpec);			// timeout

		if	(lResultTmp != ADN_OK)
			// error
		{
			if 	(lResultTmp == ETIMEDOUT)
				// timeout
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Timeout at waiting for event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Other error than timeout at waiting for event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
		}
		else
		{
			if 	(pFmqMsg->eEvent != eEventIn)
				// invalid received event
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid received event!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// free FmqMsg
				free(pFmqMsg);

				// return
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
													const UINT32					lDimErrorIn,
													_TCHAR*							sErrorOut)
{
BOOLEAN			bResult		= FALSE;
INT32*			hEventWait	= NULL;
struct timeval 	uTimeout;      

	#ifdef _DEBUG
		// timeout = 30sec (for enabling debugging)
	    uTimeout.tv_sec 	= 30;
	    uTimeout.tv_usec = 0;
	#else
		// timeout = 5sec
	    uTimeout.tv_sec 	= 5;
	    uTimeout.tv_usec = 0;
	#endif

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST:	{hEventWait = (INT32*) m_uThreadService.hEventConIntTest;	break;}
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEventWait = (INT32*) m_uThreadService.hEventConStop;		break;}
		default:
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid event!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	if	(hEventWait != NULL)
		// success
	{
	int32_t		lResultTmp	= LINUX_ERROR;
	uint64_t	lSignal		= ePNDEV_EVENT_THREAD_SERVICE_INVALID;
	ssize_t		lReadSize	= 0;
	fd_set		uReadFileDescriptors;
		
		// Initialize read file descriptor set
		FD_ZERO(&uReadFileDescriptors);

		// Wait for event to be ready to read
		FD_SET((*hEventWait), &uReadFileDescriptors);
		
		lResultTmp = select((*hEventWait)+1, &uReadFileDescriptors, NULL, NULL, &uTimeout);
		
		if (lResultTmp > 0)
		{
			if (FD_ISSET((*hEventWait), &uReadFileDescriptors))
			{
				lReadSize = read((*hEventWait), &lSignal, sizeof(uint64_t));
	//	    	// When event is set more than one time lSignal will be invalid!
	//	    	lSignal = (ePNDEV_EVENT_THREAD_SERVICE) eEventIn;
			}
			if (lReadSize != sizeof(uint64_t))
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): eventfd read error!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				if	(lSignal != eEventIn)
					// invalid received signal
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid received signal!"),
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
		else if (lResultTmp == 0)
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): select on eventfd says that no data was available!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): error on select on eventfd!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
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
//	wait for MultipleEvents at DllThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsUsedDevice::fnWaitForEventThreadDll(	const UINT32				lDimErrorIn,
												ePNDEV_EVENT_THREAD_DLL*	pEventOut,
												_TCHAR*						sErrorOut)
{
BOOLEAN	bResult			= FALSE;
ULONG	lEventSignaled	= 0;

	// preset OutputParameter
	*pEventOut = ePNDEV_EVENT_THREAD_DLL_INVALID;

	if	(!m_bWaitParEventOk)
		// WaitPar not initialized
	{
		m_bWaitParEventOk = TRUE;

		// preset
		memset(	m_hArrayEvent,
				0,
				(PNDEV_DIM_ARRAY_EVENT_THREAD_DLL * sizeof(HANDLE)));

		// build EventTable
		//	- index 0 must contain a valid value!
		m_hArrayEvent[ePNDEV_EVENT_THREAD_DLL_STOP - 1]				= m_uThreadDll.hEventStop.hHandle;
		m_hArrayEvent[ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON - 1]	= m_uThreadDll.hEventStopWithCon.hHandle;
		m_hArrayEvent[ePNDEV_EVENT_THREAD_DLL_UINFO - 1]			= m_uThreadDll.hEventUinfo.hHandle;
		m_hArrayEvent[ePNDEV_EVENT_THREAD_DLL_UISR - 1]				= m_uThreadDll.hEventUisr.hHandle;
	}

	// wait for events
	lEventSignaled = WaitForMultipleObjects(PNDEV_DIM_ARRAY_EVENT_THREAD_DLL,	// number of objects in array
											m_hArrayEvent,  					// array of objects
											FALSE,       						// wait for any signaled event (OR, not AND)
											INFINITE);   						// indefinite wait

	// preset
	bResult = TRUE;

	switch	(lEventSignaled)
			// signaled event
	{
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_THREAD_DLL_STOP - 1)):			{*pEventOut = ePNDEV_EVENT_THREAD_DLL_STOP;				break;}
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON - 1)):	{*pEventOut = ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON;	break;}
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_THREAD_DLL_UINFO - 1)):			{*pEventOut = ePNDEV_EVENT_THREAD_DLL_UINFO;			break;}
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_THREAD_DLL_UISR - 1)):			{*pEventOut = ePNDEV_EVENT_THREAD_DLL_UISR;				break;}
		default:
		{
			// error
			bResult = FALSE;

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Unexpected event at DllThread!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsUsedDevice::fnWaitForEventThreadDll(	const UINT32				lDimErrorIn,
												ePNDEV_EVENT_THREAD_DLL*	pEventOut,
												_TCHAR*						sErrorOut)
{
BOOLEAN bResult		= FALSE;
int32_t	lResultTmp	= ADN_ERROR;

	// preset OutputParameter
	*pEventOut = ePNDEV_EVENT_THREAD_DLL_INVALID;

	if	(!m_bWaitParEventOk)
		// WaitPar not initialized
	{
		if	(sigemptyset(&m_lSignalMask) != ADN_OK)
			// error at initializing SignalMask
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at initializing SignalMask!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(	(sigaddset(&m_lSignalMask, ePNDEV_EVENT_THREAD_DLL_STOP)			!= ADN_OK)
				||	(sigaddset(&m_lSignalMask, ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON)	!= ADN_OK)
				||	(sigaddset(&m_lSignalMask, ePNDEV_EVENT_THREAD_DLL_UINFO)			!= ADN_OK)
				||	(sigaddset(&m_lSignalMask, ePNDEV_EVENT_THREAD_DLL_UISR)			!= ADN_OK))
				// error at setting signal at SignalMask
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at setting signal at SignalMask!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				m_bWaitParEventOk = TRUE;
			}
		}
	}

	if	(m_bWaitParEventOk)
		//  WaitPar initialized
	{
	INT32 lSignal = 0;

		// wait for signal
		lResultTmp = sigwait(	&m_lSignalMask,
								&lSignal);

		if	(lResultTmp	!= ADN_OK)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at waiting for signal!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(	(lSignal < PNDEV_EVENT_THREAD_DLL_MIN)
				||	(lSignal > PNDEV_EVENT_THREAD_DLL_MAX))
				// invalid received signal
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid received signal!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// return
				*pEventOut = (ePNDEV_EVENT_THREAD_DLL) lSignal;

				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

BOOLEAN clsUsedDevice::fnWaitForEventThreadDll(	const UINT32				lDimErrorIn,
												ePNDEV_EVENT_THREAD_DLL*	pEventOut,
												_TCHAR*						sErrorOut)
{
BOOLEAN		bResult		= FALSE;
int32_t		lResultTmp	= LINUX_ERROR;
uint64_t	lSignal 	= ePNDEV_EVENT_THREAD_DLL_INVALID;
ssize_t		lReadSize	= 0;
fd_set		uReadFileDescriptors;


	// preset OutputParameter
	*pEventOut = ePNDEV_EVENT_THREAD_DLL_INVALID;


	// Initialize read file descriptor set
	FD_ZERO(&uReadFileDescriptors);

	// Wait for event to be ready to read
	FD_SET((INT32)	m_uThreadDll.hEventStop.lUint64,		&uReadFileDescriptors);
	FD_SET((INT32)	m_uThreadDll.hEventStopWithCon.lUint64,	&uReadFileDescriptors);
	FD_SET((INT32)	m_uThreadDll.hEventUinfo.lUint64,		&uReadFileDescriptors);
	FD_SET((INT32)	m_uThreadDll.hEventUisr.lUint64,		&uReadFileDescriptors);

	lResultTmp = select(m_uMaxEventFD+1, &uReadFileDescriptors, NULL, NULL, NULL);

	if (lResultTmp > 0)
	{
	    if (FD_ISSET((INT32)m_uThreadDll.hEventStop.lUint64, &uReadFileDescriptors))
	    {
	    	lReadSize = read((INT32)m_uThreadDll.hEventStop.lUint64, &lSignal, sizeof(UINT64));
	    	// When event is set more than one time lSignal will be invalid (mulitipe of ePNDEV_EVENT_THREAD_DLL_STOP)
	    	lSignal = ePNDEV_EVENT_THREAD_DLL_STOP;
	    }
	    else if (FD_ISSET((INT32)m_uThreadDll.hEventStopWithCon.lUint64, &uReadFileDescriptors))
	    {
	    	lReadSize = read((INT32)m_uThreadDll.hEventStopWithCon.lUint64, &lSignal, sizeof(UINT64));
	    	// When event is set more than one time lSignal will be invalid (mulitipe of ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON)
	    	lSignal = ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON;
	    }
	    else if (FD_ISSET((INT32)m_uThreadDll.hEventUisr.lUint64, &uReadFileDescriptors))
	    {
	    	lReadSize = read((INT32)m_uThreadDll.hEventUisr.lUint64, &lSignal, sizeof(UINT64));
	    	// When event is set more than one time lSignal will be invalid (mulitipe of ePNDEV_EVENT_THREAD_DLL_UISR)
	    	lSignal = ePNDEV_EVENT_THREAD_DLL_UISR;
	    }
	    else if (FD_ISSET((INT32)m_uThreadDll.hEventUinfo.lUint64, &uReadFileDescriptors))
	    {
	    	lReadSize = read((INT32)m_uThreadDll.hEventUinfo.lUint64, &lSignal, sizeof(UINT64));
	    	// When event is set more than one time lSignal will be invalid (mulitipe of ePNDEV_EVENT_THREAD_DLL_UINFO)
	    	lSignal = ePNDEV_EVENT_THREAD_DLL_UINFO;
	    }
		if (lReadSize != sizeof(uint64_t))
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): eventfd read error!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(	(lSignal < PNDEV_EVENT_THREAD_DLL_MIN)
				||	(lSignal > PNDEV_EVENT_THREAD_DLL_MAX))
				// invalid received signal
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid received signal!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// return
				*pEventOut = (ePNDEV_EVENT_THREAD_DLL) lSignal;

				// success
				bResult = TRUE;
			}
		}
	}
	else if (lResultTmp == 0)
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): select on eventfd says that no data was available!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): error on select on eventfd!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
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
//	create event for SYS
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN	clsUsedDevice::fnCreateEventSys(const ePNDEV_EVENT_THREAD_DLL	eEventIn,
										const UINT32					lDimErrorIn,
										uPNDEV64_HANDLE*				pHandleEventOut,
										_TCHAR*							sErrorOut)
{
BOOLEAN bResult		= FALSE;
UINT32	lEventId	= 0;

	// preset
	pHandleEventOut->lUint64 = (INT32) -1;

	// Note:
	//	- for firing an event by SYS an Adonis EventObject is required
	//	- such an EventObject is created by an implicit IOCTL_CREATE_EVENT during call of event_create()
	//	- this IOCTL_CREATE_EVENT needs a unique EventId for storing the Adonis EventObject
	//	- each EventId must be unique for the whole Adonis driver!
	//		-> this can only be managed by SYS

	if	(fnAllocEventId(lDimErrorIn,
						&lEventId,
						sErrorOut))
		// allocating EventId by SYS ok
	{
	int32_t		lResultTmp = ADN_ERROR;
	sigevent	uSigEvent;

		// set SigEvent
		lResultTmp = sigevent_set_notification(	&uSigEvent,							// SigEvent
												0, 									// cycle always 0 (event should never be send to creator if fired several times)
												(UINT32) eEventIn,					// SignalNo
												(pthread_t) m_uThreadDll.hThread.lUint64);	// ThreadId of receiver

		if	(lResultTmp != ADN_OK)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Error at sigevent_set_notification()!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// preset
			lResultTmp = ADN_ERROR;

			// create Event
			lResultTmp = event_create(	(int32_t) m_pDataSet->fnGetHandleDriverSys(),	// FileDescriptor
										&uSigEvent,										// SigEvent
										lEventId);										// ID unique for driver!

			if	(lResultTmp != ADN_OK)
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at event_create()!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;

				// return
				pHandleEventOut->lUint64 = lEventId;
			}
		}
	}

	return(bResult);
}

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
//  DllThread
//	- each device has its own thread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
ULONG WINAPI clsUsedDevice::fnThreadDll(void* pFctArgIn)
{
UINT32			lExitCode	= MAXUINT32;
clsUsedDevice*	pUsedDevice	= NULL;

	pUsedDevice = (clsUsedDevice*) pFctArgIn;

	if	(	(pUsedDevice	== NULL)
		||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
		// error
	{
	_TCHAR	sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set DebugMsg
		{
			_stprintf_s(sTraceDetail,
						_countof(sTraceDetail),
						_TEXT("PnDev[x,x,x]: ########## Invalid DeviceHandle!"));

			clsDataSet::fnSetTrace(sTraceDetail);
		}
	}
	else
	{
		// return
		lExitCode = 0;

		// do actions of DllThread
		pUsedDevice->fnDoThreadDll();
	}

	return(lExitCode);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
// Note:
//	- ExitCode is implemented by a ptr (pExitCode)
//	- by pthread_join(hThread, void** pPtrExitCode) it would be possible to wait for termination of thread but there is no timeout!
//	- after end of this function pthread_exit() is called by Adonis, ExitCode is written to TCB

void* clsUsedDevice::fnThreadDll(void* pFctArgIn)
{
void*			pExitCode	= (void*) -1;
clsUsedDevice*	pUsedDevice	= NULL;

	pUsedDevice = (clsUsedDevice*) pFctArgIn;

	if	(	(pUsedDevice	== NULL)
		||	(pUsedDevice	!= pUsedDevice->m_hDeviceVerifier))
		// error
	{
	_TCHAR	sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set DebugMsg
		{
			_stprintf_s(sTraceDetail,
						_countof(sTraceDetail),
						_TEXT("PnDev[x,x,x]: ########## Invalid DeviceHandle!"));

			clsDataSet::fnSetTrace(sTraceDetail);
		}
	}
	else
	{
		// return
		pExitCode = NULL;

		// do actions of DllThread
		pUsedDevice->fnDoThreadDll();
	}

	return(pExitCode);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif


//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to Application in Usermode
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN) || defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
ePNDEV_RESULT clsUsedDevice::fnMapMemory(	clsUsedDevice*			pUsedDeviceIn,
											uPNDEV_MAP_MEMORY_IN*	pIn,
											uPNDEV_MAP_MEMORY_OUT*	pOut)
{
ePNDEV_RESULT eResult = ePNDEV_FATAL;

	if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) pUsedDeviceIn,
										IOCTL_MAP_MEMORY,
										sizeof(uPNDEV_MAP_MEMORY_IN),
										sizeof(uPNDEV_MAP_MEMORY_OUT),
										pIn,
										pOut))
		// error at IoctlOpenDevice
	{
		// ErrorString already set
		eResult = ePNDEV_FATAL;
	}
	else
	{
		eResult = ePNDEV_OK;

	}

	return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
ePNDEV_RESULT clsUsedDevice::fnMapMemory(	clsUsedDevice*			pUsedDeviceIn,
											uPNDEV_MAP_MEMORY_IN*	pIn,
											uPNDEV_MAP_MEMORY_OUT*	pOut)
{
ePNDEV_RESULT 	eResult 	= ePNDEV_FATAL;
BOOLEAN			bResultTmp	= FALSE;
UINT32			i			= 0;

#if (PNDEV_PLATFORM == PNDEV_UM_PLATFORM_64)
  // selected platform = x64
INT32           lFlags      = MAP_SHARED | MAP_LOCKED;
#else
// selected platform = x32
INT32           lFlags      = MAP_SHARED | MAP_32BIT | MAP_LOCKED;      //MAP_32BIT only available when compiling on 64bit systems
#endif


	UNREFERENCED_PARAMETER(pUsedDeviceIn);

	// release BarMem
	{
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			if	((pIn->uBarKm[i].uVirtualAdr.pPtr)!= NULL)
				// valid ptr
			{
				pOut->uBarUm[i].uAdrUser.pPtr = (volatile UINT8*) (mmap(0,
																		pIn->uBarKm[i].lSize,							// length
																		PROT_READ | PROT_WRITE,							// protection
																		lFlags,										    // flags
																		(int32_t) m_pDataSet->fnGetHandleDriverSys(),	// fildes
																		(off_t) (pIn->uBarKm[i].lPhyAdr)));				// physical address

				if	(pOut->uBarUm[i].uAdrUser.pPtr != MAP_FAILED)
					// success
				{
					bResultTmp = TRUE;

					// set Size
					pOut->uBarUm[i].lSize = pIn->uBarKm[i].lSize;
				}
				else
				{
					// error
					bResultTmp = FALSE;
					break;
				}
			}
			else
			{
				// ignore error, because they can be zero
				// success
				bResultTmp = TRUE;

			}
		}
	}

	if	(bResultTmp)
		// success
	{
		// release Sdram
		{
			if	(pIn->uSdramKm.uVirtualAdr.pPtr != NULL)
				// valid virtual Ptr
			{
				pOut->uSdramUm.uAdrUser.pPtr =  (volatile UINT8*) (mmap(0,												// address
                                                                        pIn->uSdramKm.lSize,							// length
                                                                        PROT_READ | PROT_WRITE,										// protection
                                                                        lFlags,										    // flags
                                                                        (int32_t) m_pDataSet->fnGetHandleDriverSys(),	// fildes
                                                                        (off_t) (pIn->uSdramKm.lPhyAdr)));				// physical address

				if	(pOut->uSdramUm.uAdrUser.pPtr != MAP_FAILED)
					// success
				{
					bResultTmp = TRUE;

					// set Size
					pOut->uSdramUm.lSize = pIn->uSdramKm.lSize;
				}
				else
				{
					// error
					bResultTmp = FALSE;
				}
			}
			else
			{
				// ignore because there are some card without sdram, e.g. Intel
				bResultTmp = TRUE;

			}
		}

		if	(bResultTmp)
			// success
		{
			if	(pIn->uHostMemKm.uVirtualAdr.pPtr!= NULL)
				// valid ptr.
			{
				pOut->uHostMemUm.uAdrUser.pPtr =(volatile UINT8*) (mmap(0,												// address
																	    pIn->uHostMemKm.lSize,							// length
																	    PROT_READ | PROT_WRITE,										// protection
																	    lFlags,										    // flags
																	    (int32_t) m_pDataSet->fnGetHandleDriverSys(),	// fildes
																	    (off_t) (pIn->uHostMemKm.lPhyAdr)));			// physical address

				if	(pOut->uHostMemUm.uAdrUser.pPtr != MAP_FAILED)
					// success
				{
					bResultTmp = TRUE;

					// set Size
					pOut->uHostMemUm.lSize = pIn->uHostMemKm.lSize;
				}
				else
				{
					// error
					bResultTmp = FALSE;
				}
			}
			else
			{
				// ignore because there are some card without sdram, e.g. Intel
				bResultTmp = TRUE;

			}
		}
	}

	if	(bResultTmp)
		// success
	{
		eResult = ePNDEV_OK;
	}

	return(eResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  MapMemory to Application in Usermode
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN) || defined (PNDEV_OS_ADONIS)
//------------------------------------------------------------------------
BOOLEAN clsUsedDevice::fnUnmapMemory(	uPNDEV_HW_RES_SINGLE_UM*	pBarUm,
										uPNDEV_HW_RES_SINGLE_UM*	pSdramUm,
										uPNDEV_HW_RES_SINGLE_UM*	pHostMemUm)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(pBarUm);
	UNREFERENCED_PARAMETER(pSdramUm);
	UNREFERENCED_PARAMETER(pHostMemUm);

	// the Useremory was already released in KernelMode

	// success
	bResult = TRUE;

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsUsedDevice::fnUnmapMemory(	uPNDEV_HW_RES_SINGLE_UM*	pBarUm,
										uPNDEV_HW_RES_SINGLE_UM*	pSdramUm,
										uPNDEV_HW_RES_SINGLE_UM*	pHostMemUm)
{
BOOLEAN bResult 	= FALSE;
INT32	lResultTmp	= LINUX_ERROR;
UINT32	i			= 0;

	// release BarMem
	{
		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			if	((pBarUm[i].uAdrUser.pPtr)!= NULL)
				// valid ptr
			{
				lResultTmp = munmap(	(void*) pBarUm[i].uAdrUser.pPtr,
										pBarUm[i].lSize);

				if	(lResultTmp != LINUX_OK)
					// error
				{
					break;
				}
			}
			else
			{
				// ignore error, because they can be zero
				// success
				lResultTmp = LINUX_OK;

			}
		}
	}

	if	(lResultTmp == LINUX_OK)
		// success
	{
		// release Sdram
		{
			if	((pSdramUm->uAdrUser.pPtr)!= NULL)
				// valid ptr
			{
				lResultTmp = munmap((void*) pSdramUm->uAdrUser.pPtr,
									pSdramUm->lSize);
			}
			else
			{
				// ignore error, because it can be zero, e.g. Intel
				// success
				lResultTmp = LINUX_OK;
			}
		}

		if	(lResultTmp == LINUX_OK)
			// success
		{
			// release SharedHostRam
			{
				if	((pHostMemUm->uAdrUser.pPtr)!= NULL)
					// valid ptr
				{
					lResultTmp = munmap((void*) pHostMemUm->uAdrUser.pPtr,
										pHostMemUm->lSize);
				}
				else
				{
					// ignore error, because it can be zero
					// success
					lResultTmp = LINUX_OK;
				}
			}
		}
	}

	if	(lResultTmp == LINUX_OK)
		// success
	{
		bResult = TRUE;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif


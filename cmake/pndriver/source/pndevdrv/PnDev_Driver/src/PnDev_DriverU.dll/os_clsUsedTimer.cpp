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
/*  F i l e               &F: os_clsUsedTimer.cpp                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	OS specific code of class "clsUsedTimer"
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
//	start up resources of ServiceThread / TimerThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnStartUpTimerRes(	const UINT32	lPrioThreadIn,
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
		// create EventConStop
		m_hEventConStop = CreateEvent(	NULL,		// handle cannot be inherited
										FALSE,		// auto-reset event object
										FALSE,		// non signaled state
										NULL);		// unnamed object

		if	(m_hEventConStop == NULL)
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
		// start up resources of TimerThread
		{
			// create EventStopWithCon
			m_uTimer.hEventStopWithCon.hHandle	= CreateEvent(	NULL,		// handle cannot be inherited
																FALSE,		// auto-reset event object
																FALSE,		// non signaled state
																NULL);		// unnamed object
															
			// create EventUtmrTick
			m_uTimer.hEventUtmrTick.hHandle		= CreateEvent(	NULL,		// handle cannot be inherited
																FALSE,		// auto-reset event object
																FALSE,		// non signaled state
																NULL);		// unnamed object

			// create EventUtmrTickSynch
			m_uTimer.hEventUtmrTickSynch.hHandle	= CreateEvent(	NULL,		// handle cannot be inherited
																	FALSE,		// auto-reset event object
																	FALSE,		// non signaled state
																	NULL);		// unnamed object


			if	(	(m_uTimer.hEventStopWithCon.hHandle		== NULL)
				||	(m_uTimer.hEventUtmrTick.hHandle		== NULL)
				||	(m_uTimer.hEventUtmrTickSynch.hHandle	== NULL))
				// error
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Error at creating events of Timer!!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
			ULONG lThreadId = 0;

				// create Timerthread for receiving kernel timer tick events
				m_uTimer.hThread.hHandle = CreateThread(NULL,						// handle cannot be inherited
														0,							// default stack size
														clsUsedTimer::fnTimer,		// function
														this,						// FunctionArg
														0,							// thread runs immediately after creation
														&lThreadId);				// ThreadId

				if	(m_uTimer.hThread.hHandle == NULL)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Error at creating Timerthread!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					if	(fnSetThreadPrio(	m_uTimer.hThread.hHandle,
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
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnStartUpTimerRes(	const UINT32	lPrioThreadIn,
											const UINT32	lStackSizeThreadIn,
											const _TCHAR*	sNameThreadIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lPrioThreadIn);
	UNREFERENCED_PARAMETER(lStackSizeThreadIn);
	UNREFERENCED_PARAMETER(sNameThreadIn);
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
//	shut down resources of ServiceThread / TimerThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnShutDownTimerRes(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(sErrorOut);

	// success
	bResult = TRUE;

	// shut down resources of ServiceThread
	{
		CloseHandle(m_hEventConStop);

		m_hEventConStop		= NULL;
	}

	// shut down resources of Timer
	{
		CloseHandle(m_uTimer.hEventStopWithCon.hHandle);
		CloseHandle(m_uTimer.hEventUtmrTick.hHandle);
		CloseHandle(m_uTimer.hEventUtmrTickSynch.hHandle);

		m_uTimer.hEventStopWithCon.hHandle	= NULL;
		m_uTimer.hEventUtmrTick.hHandle		= NULL;
		m_uTimer.hEventUtmrTickSynch.hHandle	= NULL;
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnShutDownTimerRes(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
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
//	set event of TimerThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnSetEventTimer(	ePNDEV_EVENT_TIMER		eEventIn,
										const UINT32			lDimErrorIn,
										_TCHAR*					sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hEvent	= NULL;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_TIMER_STOP_WITH_CON:		{hEvent = m_uTimer.hEventStopWithCon.hHandle;	break;}
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
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnSetEventTimer(	ePNDEV_EVENT_TIMER		eEventIn,
										const UINT32			lDimErrorIn,
										_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(eEventIn);
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
//	set event of ServiceThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE	eEventIn,
												const UINT32				lDimErrorIn,
												_TCHAR*						sErrorOut)
{
BOOLEAN	bResult	= FALSE;
HANDLE	hEvent	= NULL;

	switch	(eEventIn)
			// event
	{
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEvent = m_hEventConStop;		break;}
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
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE	eEventIn,
												const UINT32				lDimErrorIn,
												_TCHAR*						sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(eEventIn);
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
//	wait for SingleEvent at ServiceThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
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
		case ePNDEV_EVENT_THREAD_SERVICE_CON_STOP:		{hEventWait = m_hEventConStop;		break;}
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
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN	clsUsedTimer::fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
													const UINT32					lDimErrorIn,
													_TCHAR*							sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(eEventIn);
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
//	wait for MultipleEvents of TimerThread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
BOOLEAN clsUsedTimer::fnWaitForEventTimer(	const UINT32			lDimErrorIn,
											ePNDEV_EVENT_TIMER*		pEventOut,
											_TCHAR*					sErrorOut)
{
BOOLEAN	bResult			= FALSE;
ULONG	lEventSignaled	= 0;

	// preset OutputParameter
	*pEventOut = ePNDEV_EVENT_TIMER_INVALID;

	if	(!m_bWaitParEventOk)
		// WaitPar not initialized
	{
		m_bWaitParEventOk = TRUE;

		// preset
		memset(	m_hArrayEvent,
				0,
				(PNDEV_DIM_ARRAY_EVENT_TIMER * sizeof(HANDLE)));

		// build EventTable
		//	- index 0 must contain a valid value!
		m_hArrayEvent[ePNDEV_EVENT_TIMER_STOP_WITH_CON - 1]	= m_uTimer.hEventStopWithCon.hHandle;
		m_hArrayEvent[ePNDEV_EVENT_TIMER_KTICK - 1]			= m_uTimer.hEventUtmrTick.hHandle;
		m_hArrayEvent[ePNDEV_EVENT_TIMER_SYNC_KTICK - 1]	= m_uTimer.hEventUtmrTickSynch.hHandle;

	}

	// wait for events
	lEventSignaled = WaitForMultipleObjects(PNDEV_DIM_ARRAY_EVENT_TIMER,	// number of objects in array
											m_hArrayEvent,  				// array of objects
											FALSE,       					// wait for any signaled event (OR, not AND)
											INFINITE);   					// indefinite wait

	// preset
	bResult = TRUE;

	switch	(lEventSignaled)
			// signaled event
	{
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_TIMER_STOP_WITH_CON - 1)):	{*pEventOut = ePNDEV_EVENT_TIMER_STOP_WITH_CON;	break;}
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_TIMER_KTICK		- 1)):		{*pEventOut = ePNDEV_EVENT_TIMER_KTICK;				break;}
		case (WAIT_OBJECT_0 + (ePNDEV_EVENT_TIMER_SYNC_KTICK - 1)):		{*pEventOut = ePNDEV_EVENT_TIMER_SYNC_KTICK;		break;}

		default:
		{
			// error
			bResult = FALSE;

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Unexpected event at TimerThread!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

			break;
		}
	}

	return(bResult);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN clsUsedTimer::fnWaitForEventTimer(	const UINT32			lDimErrorIn,
											ePNDEV_EVENT_TIMER*		pEventOut,
											_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	UNREFERENCED_PARAMETER(lDimErrorIn);
	UNREFERENCED_PARAMETER(pEventOut);
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
//  TimerThread
//	- each Timer has its own thread
//************************************************************************

//------------------------------------------------------------------------
#if defined (PNDEV_OS_WIN)
//------------------------------------------------------------------------
ULONG WINAPI clsUsedTimer::fnTimer(void* pFctArgIn)
{
UINT32			lExitCode	= MAXUINT32;
clsUsedTimer*	pUsedTimer	= NULL;

	pUsedTimer = (clsUsedTimer*) pFctArgIn;

	if	(	(pUsedTimer	== NULL)
		||	(pUsedTimer	!= pUsedTimer->m_hTimerVerifier))
		// error
	{
	_TCHAR	sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

		// set DebugMsg
		{
			_stprintf_s(sTraceDetail,
						_countof(sTraceDetail),
						_TEXT("PnDev[x,x,x]: ########## Invalid TimerHandle!"));

			clsDataSet::fnSetTrace(sTraceDetail);
		}
	}
	else
	{
		// return
		lExitCode = 0;

		// do actions of Timer
		pUsedTimer->fnDoTimer();
	}

	return(lExitCode);
}

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_ADONIS) || defined (PNDEV_OS_LINUX)
//------------------------------------------------------------------------

//------------------------------------------------------------------------
#elif defined (PNDEV_OS_UNKNOWN)
//------------------------------------------------------------------------
#endif

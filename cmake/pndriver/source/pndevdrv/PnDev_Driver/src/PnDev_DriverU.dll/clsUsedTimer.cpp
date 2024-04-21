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
/*  F i l e               &F: clsUsedTimer.cpp                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsUsedTimer"
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

clsUsedTimer::clsUsedTimer(void)
{
	// preset
	memset(	this,
			0,
			sizeof(clsUsedTimer));

	// set verifier for TimerHandle
	m_hTimerVerifier = this;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

clsUsedTimer::~clsUsedTimer(void)
{
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set variables of UsedTimerObject
//************************************************************************

void clsUsedTimer::fnSetVarOpenTimer1(	HANDLE					hDriverDllIn,
										clsDataSet*				pDataSetIn,
										clsUsedDevice*			pUsedDevice,
										uPNDEV_OPEN_TIMER_IN*	pIn)
{
	m_hDriverDll		= hDriverDllIn;
	m_pDataSet			= pDataSetIn;
	m_pUsedDevice		= pUsedDevice;

	m_pCbfTick			= pIn->uCbfTick.pPtr;
	m_pCbfParUser		= pIn->uCbfParUser.pPtr;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set variables of UsedTimerObject
//************************************************************************

void clsUsedTimer::fnSetVarOpenTimer2(uPNDEV_OPEN_TIMER_OUT* pOut)
{
	// use 64Bit value because DriverHandle may be a 64Bit handle!
	m_uReceiverSys.lUint64 = pOut->uInternal.uReceiverSys.lUint64;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do actions of TimerThread
//************************************************************************

void clsUsedTimer::fnDoTimer(void)
{
BOOLEAN	bStop				= FALSE;
_TCHAR	sErrorThread[PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR	sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

	// Note:
	//	- if there is an error at this thread it is notified by CbfEvent

	// set DebugMsg
	{
		_stprintf_s(sTraceDetail,
					_countof(sTraceDetail),
					_TEXT("PnDev[x,x,x]: TimerThread created"));

		clsDataSet::fnSetTrace(sTraceDetail);
	}

	while	(!bStop)
			// thread not stopped
	{
		ePNDEV_EVENT_TIMER eEvent = ePNDEV_EVENT_TIMER_INVALID;

		if	(fnWaitForEventTimer(	_countof(sErrorThread),
									&eEvent,
									sErrorThread))
			// waiting for MultipleEvents at Timer ok
		{
			switch	(eEvent)
					// signaled event
			{
				case ePNDEV_EVENT_TIMER_STOP_WITH_CON:
				{
					// stop timer
					bStop = TRUE;

					// set DebugMsg
					{
						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[x,x,x]: TimerThread - EventStop"));

						clsDataSet::fnSetTrace(sTraceDetail);
					}

					break;
				}
				case ePNDEV_EVENT_TIMER_KTICK:
				{
					// call CbfTick
					m_pCbfTick(	m_pUsedDevice,
								ePNDEV_EVENT_CLASS_TICK,
								(const void*) m_pCbfParUser);

					break;
				}
				case ePNDEV_EVENT_TIMER_SYNC_KTICK:
				{
					// call CbfTick
					m_pCbfTick(	m_pUsedDevice,
								ePNDEV_EVENT_CLASS_SYNC_TICK,
								(const void*) m_pCbfParUser);

					break;
				}
				default:
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Unexpected event at TimerThread!"),
												NULL,
												_countof(sErrorThread),
												sErrorThread);

					clsDataSet::fnSetTrace(sErrorThread);

					break;
				}
			}
		}
	}


	// set DebugMsg
	{
		_stprintf_s(sTraceDetail,
					_countof(sTraceDetail),
					_TEXT("PnDev[x,x,x]: TimerThread terminated"));

		clsDataSet::fnSetTrace(sTraceDetail);
	}

	// set EventConStop
	fnSetEventThreadService(ePNDEV_EVENT_THREAD_SERVICE_CON_STOP,
							_countof(sErrorThread),
							sErrorThread);

}

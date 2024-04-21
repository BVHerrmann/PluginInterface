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
/*  F i l e               &F: General.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for general functions
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "General.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnCheckEventList)
//	#pragma alloc_text (PAGE, fnDoThreadSys)
//	#pragma alloc_text (PAGE, fnTimeLoop)
//	#pragma alloc_text (PAGE, fnSetTraceFctError)
#endif

//########################################################################
//  Variables
//########################################################################

static UINT32			g_lValueTmp	= 0;
static volatile UINT32*	g_pValueTmp	= &g_lValueTmp;

//************************************************************************
//  D e s c r i p t i o n :
//
//  check EventList
//************************************************************************

void fnCheckEventList(	DEVICE_EXTENSION*	pDevExtIn,
						uPNDEV_APP*			pAppExcludedIn,
						BOOLEAN				bThreadSysIn)
{
KIRQL lIrqlCur = PASSIVE_LEVEL;

	if	(!fnCheckIrql(	DISPATCH_LEVEL,
						&lIrqlCur))
		// invalid IRQL
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Invalid Irql (=%u) [%s()]!",
							pDevExtIn->sPciLocShortAscii,
							lIrqlCur,
							__FUNCTION__);
					
		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
		if	(	(pDevExtIn->uEvent.lFatalFw		!= 0)
			||	(pDevExtIn->uEvent.eFatalAsync	!= ePNDEV_FATAL_ASYNC__INVALID)
			||	(pDevExtIn->uEvent.eWarning		!= ePNDEV_WARNING__INVALID)
			||	(pDevExtIn->uEvent.bReqClose)
			||	(pDevExtIn->uEvent.eDebug		!= ePNDEV_DEBUG__INVALID))
			// event exist
		{
			// Note:
			//	- don't mask board interrupt here because it is not unmasked again (only at fnEvtIntEnable())

			if	(pDevExtIn->lCtrApp == 0)
				// no App exist
			{
				// don't clear events
			}
			else
			{
				// inform all AppObjects
				//	- AppList not used at ISR
				{
				uLIST_HEADER*	pList		= NULL;
				uLIST_HEADER*	pBlockTmp	= NULL;
				uPNDEV_EVENT	uEventCopy;

					// copy EventList
					//	- otherwise different Apps may get different events because of possible new occurring events
					*((uPNDEV_EVENT*) &uEventCopy)	= *((uPNDEV_EVENT*) &pDevExtIn->uEvent);

					// clear events
					pDevExtIn->uEvent.lFatalFw		= 0;
					pDevExtIn->uEvent.eFatalAsync	= ePNDEV_FATAL_ASYNC__INVALID;
					pDevExtIn->uEvent.eWarning		= ePNDEV_WARNING__INVALID;
					pDevExtIn->uEvent.bReqClose		= FALSE;
					pDevExtIn->uEvent.eDebug		= ePNDEV_DEBUG__INVALID;

					pList = &pDevExtIn->uListApp;

					//------------------------------------------------------------------------
					// enter CriticalSection
					fnEnterCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
					{
						// get first block of AppList
						pBlockTmp = (uLIST_HEADER*) pList->pNext;

						while	(pBlockTmp != pList)
								// end of list not reached
						{
						uPNDEV_APP* pApp = (uPNDEV_APP*) pBlockTmp;

							if	(pApp != pAppExcludedIn)
								// not excluded App
							{
								// copy events to AppObject
								//	- necessary because each App will read it by service GetEventList
								//	- otherwise there would be no real trigger for clearing events
								pApp->uEvent.lFatalFw		= uEventCopy.lFatalFw;
								pApp->uEvent.eFatalAsync	= uEventCopy.eFatalAsync;
								pApp->uEvent.eWarning 		= uEventCopy.eWarning;
								pApp->uEvent.bReqClose		= uEventCopy.bReqClose;
								pApp->uEvent.eDebug 		= uEventCopy.eDebug;

								// set EventUinfo
								fnSetEventThreadDll(pDevExtIn,
													pApp,
													ePNDEV_EVENT_THREAD_DLL_UINFO,
													bThreadSysIn);
							}

							// get next block of AppList
							pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;
						}
					}
					//------------------------------------------------------------------------
					// leave CriticalSection
					fnLeaveCritSec(	pDevExtIn,
									ePNDEV_CRIT_SEC_SYS_APP_LIST);
				}
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do actions of SysThread
//************************************************************************

void fnDoThreadSys(	DEVICE_EXTENSION*	pDevExtIn,
					UINT32				lTimeCycle_10msecIn)
{
	// Note:
	//	- if there is an event then IOCTL_GET_EVENT_LIST must be called by DLL which needs pApp
	//	- pApp is returned by OpenDevice
	//		-> EventUinfo must not be set before DLL of first OpenDevice is ready
        
	if	(pDevExtIn->bDllReadyFirstOpenDevice)
		// DLL ready of first OpenDevice
	{
		pDevExtIn->lCtrThreadSys++;

		if	(pDevExtIn->lCtrThreadSys == 1)
			// ThreadStart
		{
			// simulate FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS;
		}

		if	(pDevExtIn->bResetRunning)
			// reset running
		{
			// don't access hardware during reset (wrong values would be read)
		}


		// check EventList periodically
		fnCheckEventList(	pDevExtIn,
							NULL,		// no App excluded
							TRUE);		// called by SysThread
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	TimeLoop
//
//	Note for Windows:
//	- KeDelayExecutionThread' only allowed if callers are running at IRQL = PASSIVE_LEVEL)
//************************************************************************

BOOLEAN fnTimeLoop(	DEVICE_EXTENSION*	pDevExtIn,
					const UINT32		lTimeUsecIn,
					const BOOLEAN		bShowDetailsIn)
{
BOOLEAN	bResult				= FALSE;
UINT32	lTimeDesiredUsec	= 0;
UINT32	lTimeCurUsec		= 0;		// placed here for DebugPurpose
UINT32	lCtrLoop			= 0;		// placed here for DebugPurpose

	// update desired WaitTime
	lTimeDesiredUsec = lTimeUsecIn * pDevExtIn->lTimeScaleFactor;

	if	(lTimeDesiredUsec != 0)
		// valid WaitTime
	{
	uPNDEV_CLOCK	uClockStart;
	UINT32			i = 0;

		// success
		bResult = TRUE;

		// get clock
		fnGetClock(&uClockStart);

		do
		{
			// loop by instructions (it's not optimal to call fnGetClock() in a loop)
			for	(i = 0; i < 500; i++)
			{
				*g_pValueTmp = (((*g_pValueTmp / 3) * 7) / 17);
			}

			// compute Runtime in usec
			lTimeCurUsec = fnComputeRunTimeUsec(&uClockStart);

			if	(	(bShowDetailsIn)
				&&	((lCtrLoop % 10000) == 0))
				// details should be shown
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: Current WaitTime=%u usec",
									lTimeCurUsec);
					
				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}

			lCtrLoop++;
		}
		while	(lTimeCurUsec < lTimeDesiredUsec);
				// current WaitTime < desired WaitTime (usec)
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set trace by returned ErrorString
//************************************************************************

void fnSetTraceFctError(DEVICE_EXTENSION*	pDevExtIn,
						_TCHAR*				sErrorIn)
{
char	sErrorAscii [PNDEV_SIZE_STRING_BUF]		= {0};
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]	= {0};

	// convert _TCHAR to ASCII string
	fnConvertStringTcharToChar(	sErrorIn,
								sErrorAscii);

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: #### %s",
						(pDevExtIn == NULL) ? "x,x,x" : pDevExtIn->sPciLocShortAscii,
						sErrorAscii);
					
	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
				ePNDEV_TRACE_CONTEXT_BASIC,
				TRUE,						// logging
				sTraceDetail);
}

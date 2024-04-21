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
/*  F i l e               &F: IsrDpc.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling of ISR and DPC
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "IsrDpc.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnIsr)
//	#pragma alloc_text (PAGE, fnDpc)
//	#pragma alloc_text (PAGE, fnEvtIsrSharedInt)
//	#pragma alloc_text (PAGE, fnUpdateStatisticIsr)
//	#pragma alloc_text (PAGE, fnUpdateStatisticDpc)
//	#pragma alloc_text (PAGE, fnStartRuntimeIsr)
//	#pragma alloc_text (PAGE, fnStopRuntimeIsr)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  ISR - Legacy
//************************************************************************

BOOLEAN fnIsrLeg(	DEVICE_EXTENSION*	pDevExtIn,
					HANDLE				hOsParIn,
					UINT32				lMsgIdMsiIn)
{
BOOLEAN				bResult = FALSE;
uPNDEV_INT_DETAIL	uIntDetail;

	// Note:
	//	- don't access hardware during reset (possible hangup if using Soc1)
	//	- if a reset is running then values of AsicRegisters may be undefined
	//	- if an existing DefaultFw is not stopped then Asic may be used by this firmware -> no access allowed

	if	(	(pDevExtIn->bResetRunning)
		||	(pDevExtIn->pMasterApp == NULL)
		||	(!fnBoard_CheckInt(	pDevExtIn,
								&uIntDetail)))
		//		reset running
		// OR	no MasterApp exist
		// OR	no interrupt exist
	{
		// return FALSE to indicate that this device did not cause the interrupt

		// SharedInt occurred
		fnEvtIsrSharedInt(pDevExtIn);
	}
	else
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		if	(pDevExtIn->bLegIsrRunning)
			// ISR interrupted itself
		{
			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## ISR lock error!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
			// interrupt caused by this device
			bResult = TRUE;

			// set state IsrRunning
			pDevExtIn->bLegIsrRunning = TRUE;
			{
			BOOLEAN bReqDpc = FALSE;

				#ifdef PNDEV_GET_RUNTIME_ISR
					// start RuntimeIsr
					fnStartRuntimeIsr(pDevExtIn);
				#endif

				// update IsrStatistic
				fnUpdateStatisticIsr(pDevExtIn);

				if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
					// IntAction=Standard
				{			
					// standard ISR
					fnBoard_IsrStandard(pDevExtIn,
										&uIntDetail,
										&bReqDpc,
										lMsgIdMsiIn);

                    // If the NetPROFI Mode is active, a few Interrupts are handled by the netPROFI kernel, a few interrupts are handled by the UserSpace implementation. Overwrite value in netPROFI Mode
                    if(pDevExtIn->bNetProfi)
                    {
                        bReqDpc = fnNetProfiIntHandler (pDevExtIn);
                    }

				}
				else	// IntAction=Test
				{
					// ISR for IntTest
					fnBoard_IsrIntTest(	pDevExtIn,
										&uIntDetail,
										&bReqDpc);
				}

				if	(bReqDpc)
					// DPC should be requested
				{
					fnRequestDpc(pDevExtIn,
								 hOsParIn);
				}

				#ifdef PNDEV_GET_RUNTIME_ISR
					// stop RuntimeIsr
					fnStopRuntimeIsr(pDevExtIn);
				#endif
			}
			// reset state IsrRunning
			pDevExtIn->bLegIsrRunning = FALSE;
		}

		// set AsicEoi
		//	- a running EOI-Timer is restarted if EOI is written again!
		fnBoard_IsrSetEoi(pDevExtIn);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  ISR - Msix
//************************************************************************

BOOLEAN fnIsrMsix(	DEVICE_EXTENSION*	pDevExtIn,
					HANDLE				hOsParIn,
					UINT32				lMsgIdMsiIn)
{
BOOLEAN				bResult = FALSE;
uPNDEV_INT_DETAIL	uIntDetail;

	// Note:
	//	- don't access hardware during reset (possible hangup if using Soc1)
	//	- if a reset is running then values of AsicRegisters may be undefined
	//	- if an existing DefaultFw is not stopped then Asic may be used by this firmware -> no access allowed

	if	(	(pDevExtIn->bResetRunning)
		||	(pDevExtIn->pMasterApp == NULL)
		||	(!fnBoard_CheckInt(	pDevExtIn,
								&uIntDetail)))
		//		reset running
		// OR	no MasterApp exist
		// OR	no interrupt exist
	{
		// return FALSE to indicate that this device did not cause the interrupt

		// SharedInt occurred
		fnEvtIsrSharedInt(pDevExtIn);
	}
	else
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		if	(pDevExtIn->bMsixIsrRunning[lMsgIdMsiIn])
			// ISR interrupted itself
		{
			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## ISR lock error!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
			// interrupt caused by this device
			bResult = TRUE;

			// set state IsrRunning
			pDevExtIn->bMsixIsrRunning[lMsgIdMsiIn] = TRUE;
			{
			BOOLEAN bReqDpc = FALSE;

				#ifdef PNDEV_GET_RUNTIME_ISR
					// start RuntimeIsr
					fnStartRuntimeIsr(pDevExtIn);
				#endif

				// update IsrStatistic
				fnUpdateStatisticIsr(pDevExtIn);

				if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
					// IntAction=Standard
				{
					// save caused MsixVector
					pDevExtIn->bMsixIntOccured[lMsgIdMsiIn] = TRUE;
					
					// standard ISR
					fnBoard_IsrStandard(pDevExtIn,
										&uIntDetail,
										&bReqDpc,
										lMsgIdMsiIn);
				}
				else	// IntAction=Test
				{
					// ISR for IntTest
					fnBoard_IsrIntTest(	pDevExtIn,
										&uIntDetail,
										&bReqDpc);
				}

				if	(bReqDpc)
					// DPC should be requested
				{
					fnRequestDpc(pDevExtIn,
								 hOsParIn);
				}

				#ifdef PNDEV_GET_RUNTIME_ISR
					// stop RuntimeIsr
					fnStopRuntimeIsr(pDevExtIn);
				#endif
			}
			// reset state IsrRunning
			pDevExtIn->bMsixIsrRunning[lMsgIdMsiIn] = FALSE;
		}

		// set AsicEoi
		//	- a running EOI-Timer is restarted if EOI is written again!
		fnBoard_IsrSetEoi(pDevExtIn);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  Queue Dpc
//************************************************************************

void fnRequestDpc(DEVICE_EXTENSION* pDevExtIn,
                  HANDLE            hOsParIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
	
	if	(pDevExtIn->bOsDpcSupport)
		// OS supports DPC
	{
		if	(	(pDevExtIn->lCtrDpcQueued == 0)
			||	(pDevExtIn->eIntMode != ePNDEV_INTERRUPT_MODE_LEG))
			// DPC not queued up to now, or device not in legacy int mode
		{
			if	(!fnQueueDpc(	pDevExtIn,
								hOsParIn))
				// error at queuing DPC
			{
				if	(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
					// legacy ints use one interrupt line for one device, 
					// after masking legacy isr should not run again before dpc is finished! -> error
					// msix interrupts can run parallel (e.g. on SMP Windows System) -> no error when dpc is already queued
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__DPC_NOT_QUEUED;
				}

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## DPC not queued!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				if	(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
				{
					pDevExtIn->lCtrDpcQueued++;
				}

				if	(pDevExtIn->uIntStatistic.lCtrIsr == 1)
					// first call of ISR
				{
					#ifdef PNDEV_TRACE_ISR_DPC
						// set TraceDetail
						RtlStringCchPrintfA(sTraceDetail,
											_countof(sTraceDetail),
											"PnDev[%s]: ++++ DPC queued",
											pDevExtIn->sPciLocShortAscii);

						// set trace
						fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
									ePNDEV_TRACE_CONTEXT_ISR,
									FALSE,						// logging
									sTraceDetail);
					#endif
				}
			}
		}
	}
	else
	{	
		if	(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
		{
			pDevExtIn->lCtrDpcQueued++;
		}

		// do DPC
		fnDpc(pDevExtIn);
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  DPC
//************************************************************************

void fnDpc(DEVICE_EXTENSION* pDevExtIn)
{
	//------------------------------------------------------------------------
	// acquire DpcSpinLock
	//	- protect against re-entrance at MultiCore system!
	fnAcquireSpinLock(	pDevExtIn,
						ePNDEV_SPIN_LOCK_DPC);
	{
		if	(pDevExtIn->bResetRunning)
			// reset running
		{
			// don't access hardware during reset (possible hangup if using Soc1)
			// ignore stored DPCs
		}
		else
		{
		BOOLEAN bReqUisr = FALSE;

			if	(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
			{
				pDevExtIn->lCtrDpcQueued--;
			}

			// update DpcStatistic
			fnUpdateStatisticDpc(pDevExtIn);

			if	(pDevExtIn->eIntAction <= PNDEV_INT_ACTION_STD_MAX)
				// IntAction=Standard
			{
				// standard DPC
				fnBoard_DpcStandard(pDevExtIn,
									&bReqUisr);
			}
			else	// IntAction=Test
			{
				// DPC for IntTest
				fnBoard_DpcIntTest(	pDevExtIn,
									&bReqUisr);
			}

			if	(bReqUisr)
				// UISR should be requested
			{
			char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				if	(pDevExtIn->pMasterApp == NULL)
					// no MasterApp exist
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UISR_NOT_QUEUED;

					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## No MasterApp, UISR not queued!",
										pDevExtIn->sPciLocShortAscii);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_DPC,
								TRUE,						// logging
								sTraceDetail);
				}
				else
				{
					if	(pDevExtIn->pMasterApp->hEventUisr != NULL)
						// valid event
					{
						// set EventUisr of AppObject (MasterApp)
						fnSetEventThreadDll(pDevExtIn,
											pDevExtIn->pMasterApp,
											ePNDEV_EVENT_THREAD_DLL_UISR,
											FALSE);
					}

					if	(pDevExtIn->uIntStatistic.lCtrDpc == 1)
						// first call of DPC
					{
						#ifdef PNDEV_TRACE_ISR_DPC
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: ++++ UISR queued",
												pDevExtIn->sPciLocShortAscii);

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_DPC,
										FALSE,						// logging
										sTraceDetail);
						#endif
					}
				}
			}

			//------------------------------------------------------------------------
			// acquire SpinLockIrq of this device
			//fnAcquireSpinLockIrq(pDevExtIn);
			//	- Stringfunctions with Unicode format codes can only be used at PASSIVE_LEVEL
			{
				//...
			}
			//------------------------------------------------------------------------
			// release SpinLockIrq
			//fnReleaseSpinLockIrq(pDevExtIn);
		}
	}
	//------------------------------------------------------------------------
	// release DpcSpinLock
	fnReleaseSpinLock(	pDevExtIn,
						ePNDEV_SPIN_LOCK_DPC);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  SharedInt occurred
//************************************************************************

void fnEvtIsrSharedInt(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->uIntStatistic.lCtrSharedInt++;

	if	((pDevExtIn->uIntStatistic.lCtrSharedInt % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
		{
		char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: SharedInt=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrSharedInt);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		}
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update IsrStatistic
//************************************************************************

void fnUpdateStatisticIsr(DEVICE_EXTENSION*	pDevExtIn)
{
#ifdef PNDEV_TRACE_ISR_DPC
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	// increment CtrIntTest
	pDevExtIn->uIntStatistic.lCtrIsr++;

	if	(pDevExtIn->uIntStatistic.lCtrIsr == 1)
		// first call of ISR
	{
		pDevExtIn->uIntStatistic.bIsrEntered = TRUE;

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ++++ ISR entered",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);
		#endif
	}
	else if	((pDevExtIn->uIntStatistic.lCtrIsr % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ISRs=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrIsr);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update DpcStatistic
//************************************************************************

void fnUpdateStatisticDpc(DEVICE_EXTENSION*	pDevExtIn)
{
#ifdef PNDEV_TRACE_ISR_DPC
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	// increment CtrIntTest
	pDevExtIn->uIntStatistic.lCtrDpc++;

	if	(pDevExtIn->uIntStatistic.lCtrDpc == 1)
		// first call of DPC
	{
		pDevExtIn->uIntStatistic.bDpcEntered = TRUE;

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ++++ DPC entered",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_DPC,
						TRUE,						// logging
						sTraceDetail);
		#endif
	}
	else if	((pDevExtIn->uIntStatistic.lCtrDpc % PNDEV_DEBUG_CTR_THRESHOLD) == 0)
	{
		#ifdef PNDEV_DEVELOPMENT
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: DPCs=%u",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->uIntStatistic.lCtrDpc);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_DPC,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start RuntimeIsr
//************************************************************************

void fnStartRuntimeIsr(DEVICE_EXTENSION* pDevExtIn)
{
	pDevExtIn->lTimeIrqEdge_Nsec	= 0;
	pDevExtIn->lTimeIsrStart_Nsec	= 0;

	// get clock
	fnGetClock(&pDevExtIn->uClockStart);

	if	(pDevExtIn->pPnIp != NULL)
		// board with PNIP
	{
	char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

		switch	(pDevExtIn->eBoardDetail)
				// BoardDetail
		{

			default:
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Invalid BoardDetail!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);

				break;
			}
		}

		#ifdef PNDEV_TRACE_ISR_DPC
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: t_Irq=0x%x, t_Isr=0x%x",
								pDevExtIn->sPciLocShortAscii,
								pDevExtIn->lTimeIrqEdge_Nsec,
								pDevExtIn->lTimeIsrStart_Nsec);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
						ePNDEV_TRACE_CONTEXT_ISR,
						FALSE,						// logging
						sTraceDetail);
		#endif
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  stop RuntimeIsr
//************************************************************************

void fnStopRuntimeIsr(DEVICE_EXTENSION* pDevExtIn)
{
UINT32	lIsrRuntimeWin_Usec	= 0;


	// compute runtime of ISR by PC timer
	{
		// compute Runtime in usec
		lIsrRuntimeWin_Usec = fnComputeRunTimeUsec(&pDevExtIn->uClockStart);

		// update statistic
		{
			if	(	(pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec == 0)
				&&	(pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec == 0))
				// first time after Startup/UserRead
			{
				// initialize Min/Max
				pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec	= lIsrRuntimeWin_Usec;
				pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec	= lIsrRuntimeWin_Usec;
			}
			else if	(lIsrRuntimeWin_Usec < pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec)
				// Cur < Min
			{
				// update Min
				pDevExtIn->uIntStatistic.lIsrRuntimeMin_Usec = lIsrRuntimeWin_Usec;
			}
			else if	(lIsrRuntimeWin_Usec > pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec)
				// Cur > Max
			{
				// update Max
				pDevExtIn->uIntStatistic.lIsrRuntimeMax_Usec = lIsrRuntimeWin_Usec;
			}
		}
	}

/*
	#ifdef PNDEV_TRACE_ISR_DPC
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: IsrDelay=%u, RuntimeNsec=%u, RuntimeUsec=%u",
							pDevExtIn->sPciLocShortAscii,
							lIsrDelayAsic_Nsec,
							IsrRuntimeAsic_Nsec,
							lIsrRuntimeWin_Usec);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_ISR,
					FALSE,						// logging
					sTraceDetail);
	#endif
*/
}

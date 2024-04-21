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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_bma.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Master functions and best master algorithm                               */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2006-09-04  P03.12.00.00_00.25.04.02 lrg gsy_usr.c aufgeteilt in gsy_usr */
/*              .c gsy_msm.c (MasterSlaveManager Procs:gsy_Slave*()) und     */
/*              gsy_bma.c (BestMasterAlgorithm Procs:gsyMaster*())           */
/*  2006-10-04  P03.12.00.00_00.30.01.02 041006lrg002: PrmIndication an User */
/*              bei PRIMARY kommt/geht (ArtsRQ 371309)                       */
/*  2006-10-12  P03.12.00.00_00.30.02.02 lrg GSY_DRIFT_STATE_TRANSIENT       */
/*              FollowUp nur noch im GSY_MASTER_STATE_PRIMARY senden         */
/*  2006-10-23  P03.12.00.00_00.30.03.02 lrg GSY_PUT_INFO_1                  */
/*              231006lrg001, 231006lrg002: PriMasterWaits                   */
/*              231006lrg003: Slave state definiert auf SYNC setzen          */
/*  2007-01-16  P03.12.00.00_00.40.06.02 160107lrg001: Stratum immer = 0 und */
/*              ClockRole nur noch PRIMARY oder SECONDARY projektiert durch  */
/*              SyncProperties.SyncClass                                     */
/*  2007-02-12  P03.12.00.00_00.40.09.02 lrg #ifdef GSY_USE_VLAN             */
/*  2007-02-19  P03.12.00.00_00.40.10.02 lrg LSA_TRACE_LEVEL_NOTE_HIGH       */
/*  2007-02-20  P03.12.00.00_00.40.10.03 200207lrg001:Masteraging untersetzen*/
/*  2007-03-22  P03.12.00.00_00.40.11.02 lrg: Trace gsy_MasterQueue          */
/*  2007-04-26  P03.12.00.00_00.40.11.03 260407lrg001: k. Vergleich der MAc- */
/*              adresse in gsy_MasterBestCheck() mit GSY_NO_MASTER_MAC_CHECK */
/*  2007-05-08  P04.00.00.00_00.10.01.02 kein GSY_NO_MASTER_MAC_CHECK        */
/*  2007-07-13  P04.00.00.00_00.30.01.00 gsy_SyncSend(), Priority1,          */
/*              GSY_FRAMEID_SYNC, GSY_MULTICAST_ADDR_SYNC, 170707lrg001      */
/*  2007-07-31  P04.00.00.00_00.30.01.02 310707js001:  keine driftcorrection */
/*              der linedelay.                                               */
/*  2007-09-06  P04.00.00.00_00.30.05.02 lrg gsy_MasterStartupEnd            */
/*  2007-09-17  P04.00.00.00_00.30.05.03 070907lrg001: GSY_SYNC_PRIO1_ACTIVE */
/*              SyncTakeover, Master.StartupTimer                            */
/*  2007-11-16  P04.00.00.00_00.50.01.02 161107lrg001: ERTEC400 HW-Delay     */
/*  2008-01-07  P04.00.00.00_00.60.02.02 070108lrg003: Diag.Subdomain        */
/*  2008-01-18  P04.00.00.00_00.70.01.03 180108lrg001: Rate parallel         */
/*              berechnen                                                    */
/*  2008-02-28  P04.00.00.00_00.70.02.02 270206lrg001: ArtsRQ AP00591435     */
/*              FrameId falsch bei MSVC 6.0 und "Optimize for speed"         */
/*              280208lrg001: Neue Funktion gsy_MasterSendReal()             */
/*              Bei Uebername der Masterfunktion sofort senden               */
/*  2008-04-17  P04.00.00.00_00.80.01.02 200308lrg001: Update running Master */
/*              170408lrg002: Priority1.Level, RcvSyncPrio, LocalPrio        */
/*              170408lrg003: Bei 2 Primary Mastern mit gleichem Level       */
/*              gewinnt der lokale Master den BMA                            */
/*  2008-05-30  P04.00.00.00_00.80.02.02 290508lrg001: Hierarchie: Der       */
/*              nach PRIMARY wird nur bis zur Slave-Synchronität verschoben, */
/*              wenn ein anderer Master nicht nur Announce- sondern auch     */
/*              SyncFrames sendet.                                           */
/*              300508lrg001: nur im STARTUP-State einsynchronisieren        */
/*  2008-06-12  P04.00.00.00_00.80.04.02 120608lrg001: Verhindern, dass beim */
/*              Uebergang von PRIMARY -> SLAVE der naechste RCF-Interrupt    */
/*              in gsy_DriftAdjust() einen gsy_DriftReset() ausfuehrt und    */
/*              damit das Weiterleiten eingestellt wird.                     */
/*              130608lrg001: ERROR_TRACE fuer E400-HwDelay entfernt.        */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-10-17  P04.01.00.00_00.02.02.02 GSY_HW_TYPE stett EDD_HW_TYPE_USED  */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataSend() versenden                                     */
/*  2009-01-15  P04.01.00.00_00.02.03.03 150109lrg001: PortId and PduLen     */
/*              exchanged at sending of announce frame                       */
/*  2009-08-20  P04.02.00.00_00.01.04.02 120809lrg001,200809lrg001: HwMaster */
/*  2009-10-08  P04.02.00.00_00.01.04.04 lrg: GSY_HW_TYPE_MASTER_MSK...      */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: GSY_LOWER_SLAVE/MASTER_UPDATE  */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-04-21  P05.00.00.00_00.01.04.02 200410lrg001: AP00958586 VLAN tag   */
/*  2010-06-25  P05.00.00.00_00.01.05.02 250610lrg001: for announce frames:  */
/*              GSY_LOWER_RQB_SEND_SYNC_PTR_GET->GSY_LOWER_RQB_SEND_PTR_GET  */
/*              German comments translated to english                        */
/*  2010-10-04  P05.00.00.00_00.01.08.04 041010lrg001: check Accuracy in BMA */
/*              Handle PDSyncPLL record                                      */
/*  2010-11-15  P05.00.00.00_00.02.01.02 lrg: PLL-Timer                      */
/*              161110lrg001: Rewiew Part 6: ClockClass 0xff->0x0d (default) */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-02-18  P05.00.00.00_00.03.19.02 lrg: GSY_MASTER_TIMER_CHECK         */
/*  2011-04-14  P05.00.00.00_00.04.24.02 140411lrg: AP01162398_BMA           */
/*  2011-05-16  P05.00.00.00_00.05.21.02 160511lrg: AP01183082 gsy_SlaveTimer*/
/*  2011-05-27  P05.00.00.00_00.05.21.02 270511lrg: Trace (%d)               */
/*  2011-09-23  P05.00.00.00_00.05.56.02 AP01191740 CycleCounter -> PTCP_Time*/
/*  2011-09-29  S05.01.00.00_00.04.10.01 AP01236058 Lint Warnings entfernen  */
/*  2014-05-16  P06.00.00.00_00.01.30.01 987888,1033202: EDD API-Änderung    */
/*              des Dienstes EDD_SRV_SET_SYNC_TIME                           */
/*  2014-08-08  P06.00.00.00_00.01.23.01 966426: [GSY] Die Synchronisation   */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	6		/* GSY_MODULE_ID_BMA */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)

/*****************************************************************************/
/* Lokale Daten                                                              */
/* ...                                                                       */
/*****************************************************************************/

//#define GSY_PLL_TIMER

/*****************************************************************************/
/* Internal access function: gsy_MasterPLLTimer()                            */
/* Timer input function of the PLL controlled Sync Master.                   */
/*****************************************************************************/
//lint -esym(818, pTimer) Pointer could be const, but the function pointer cannot be always const, HM 18.05.2016
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterPLLTimer(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT8		SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_USR_PTR	pChUsr = pTimer->pChUsr;
	GSY_DRIFT_PTR	pDrift = &pChUsr->pChSys->Drift[SyncId][0];
	LSA_UINT16 		RetVal;

	if (GSY_PLL_STATE_SYNC_STOP == pChUsr->PLLData.State)
	{
		/* PLL sync stopped: stop and free Timer
		*/
		GSY_MASTER_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterPLLTimer(%02x) PLL state change ->STOP", SyncId);

		if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr->pSyncSetRbl, LSA_NULL))
		{
			RetVal = gsy_TimerClean(&pChUsr->PLLData.PLLTimer);
			if (LSA_RET_OK == RetVal)
			{
				/* Lower RQB is back and timer clean
				*/
				GSY_MASTER_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterPLLTimer(%02x) PLL state change ->OFF", SyncId);
				pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_OFF;

				if (GSY_DRIFT_STATE_MASTER == pDrift->State)
				{
					/* Set Drift for Master-SyncID to UNKNOWN
					*/
					gsy_DriftSet(pChUsr->pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_UNKNOWN);
				}

				if (GSY_CHA_STATE_CLOSING == pChUsr->State)
				{
					GSY_MASTER_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLTimer(%02x) closing: master/slave state=%u/%u",
										SyncId, pChUsr->Master.State, pChUsr->Slave.State);

					if ((GSY_SLAVE_STATE_OFF == pChUsr->Slave.State)
					&&  (GSY_MASTER_STATE_OFF == pChUsr->Master.State))
					{
						/* Finish close channel
						*/
						gsy_CloseChUsrDone(pChUsr);
					}
				}
				else
				{
					/* Switch PrmCommit state machine
					*/
					gsy_PrmCommitNext(pChUsr, LSA_NULL);
				}
			}
		}
	}
	else
	{
#ifdef GSY_PLL_TIMER_CHECK
		LSA_UINT32 DiffPLL, TimeStampPLL, TimeStampCB, Seconds, Nanos;

		RetVal = GSY_SYNC_GET_EXT_PLL_IN(&TimeStampPLL, &Seconds,
						&Nanos, &TimeStampCB, SyncId, pChUsr->pSys);

		DiffPLL = TimeStampPLL - pChUsr->PLLData.LastPLL;

		if (DiffPLL != 0)
		{
			GSY_MASTER_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLTimer(%02x) Time=%u,%09u TimeStamp=%09u TimeStampPLL=%09u LastPLL=%09u DiffPLL=%u",
				SyncId, Seconds, Nanos, TimeStampCB, TimeStampPLL, pChUsr->PLLData.LastPLL, DiffPLL);
		}

		pChUsr->PLLData.LastPLL = TimeStampPLL;
#endif
		gsy_MasterPLLGet(pChUsr, SyncId);
	}
}
//lint +esym(818, pTimer)

/*****************************************************************************/
/* Internal function: gsy_MasterPLLStart()                                   */
/* Read the ExtPLL input first time to detect "NO PLL INPUT" and set state   */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterPLLStart(
GSY_CH_USR_PTR  		pChUsr)
{
	LSA_UINT16			RetVal, Base, Count;
	LSA_UINT8			SyncId = GSY_SYNCID_CLOCK;
	GSY_DRIFT_PTR		pDrift = &pChUsr->pChSys->Drift[SyncId][0];
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterPLLStart() pChUsr=%X", pChUsr);

	GSY_MASTER_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLStart(%02x) CycleLength=%u ReductionRatio=%u PLLHWDelay=%u SetWaitCount=%u: PLL state change ->START",
						SyncId, pChUsr->PLLData.CycleLength, pChUsr->PLLData.ReductionRatio, pChUsr->PLLData.PLLHWDelay, pChUsr->PLLData.SetWaitCount);

	RetVal = GSY_SYNC_GET_EXT_PLL_IN(&pChUsr->PLLData.New.TimeStampPLL, &pChUsr->PLLData.New.Seconds,
					&pChUsr->PLLData.New.Nanos, &pChUsr->PLLData.New.TimeStampCB, SyncId, pChUsr->pSys);

	if (LSA_RET_OK != RetVal)
	{
		GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLStart() pChUsr=%X: GSY_SYNC_GET_EXT_PLL_IN: RetVal=0x%x",
						pChUsr, RetVal);
		gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
	}
	else
	{
		/* Calculate best combination of Timerbase/-Count
		 * and allocate cyclic ExtPLL-In timer and start it
		*/
		if ((10000 <= pChUsr->PLLData.PLLInterval)			//lrgi210
		&&  (0 == (pChUsr->PLLData.PLLInterval % 10000)))
		{
			Base = LSA_TIME_BASE_10S;
			Count = pChUsr->PLLData.PLLInterval / 10000;
		}
		else if ((1000 <= pChUsr->PLLData.PLLInterval)
		     &&  (0 == (pChUsr->PLLData.PLLInterval % 1000)))
		{
			Base = LSA_TIME_BASE_1S;
			Count = pChUsr->PLLData.PLLInterval / 1000;
		}
		else if ((100 <= pChUsr->PLLData.PLLInterval)
		     &&  (0 == (pChUsr->PLLData.PLLInterval % 100)))
		{
			Base = LSA_TIME_BASE_100MS;
			Count = pChUsr->PLLData.PLLInterval / 100;
		}
		else if ((10 <= pChUsr->PLLData.PLLInterval)
		     &&  (0 == (pChUsr->PLLData.PLLInterval % 10)))
		{
			Base = LSA_TIME_BASE_10MS;
			Count = pChUsr->PLLData.PLLInterval / 10;
		}
		else
		{
			Base = LSA_TIME_BASE_1MS;
			Count = pChUsr->PLLData.PLLInterval;			//lrgi210
		}
		pChUsr->PLLData.PLLTimer.InUse = LSA_FALSE;
		pChUsr->PLLData.PLLTimer.Running = LSA_FALSE;
		pChUsr->PLLData.PLLTimer.pChUsr = pChUsr;
		pChUsr->PLLData.PLLTimer.pChSys = pChUsr->pChSys;
		pChUsr->PLLData.PLLTimer.pTimeoutProc = gsy_MasterPLLTimer;

		RetVal = gsy_TimerAlloc(&pChUsr->PLLData.PLLTimer, LSA_TIMER_TYPE_CYCLIC, Base);
		if (LSA_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLStart() pChUsr=%X cannot allocate timer: RetVal=0x%x",
							pChUsr, RetVal);
			gsy_ErrorUser(GSY_MODULE_ID, __LINE__, GSY_ERR_TIMER, LSA_NULL, 0);
		}
		else
		{
			gsy_TimerStart(&pChUsr->PLLData.PLLTimer, (LSA_UINT16)Count);
			pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_START;
			pChUsr->PLLData.ActWaitCount = 0;
			pChUsr->PLLData.PLLWaitMillis = 0;		//lrgi210

			GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLStart(%02x) Seconds=%u Nanos=%u TimeStamp=%u TimeStampPLL=%u PLLTimer: Base=%u Count=%u Id=%u",
				SyncId, pChUsr->PLLData.New.Seconds, pChUsr->PLLData.New.Nanos, pChUsr->PLLData.New.TimeStampCB, pChUsr->PLLData.New.TimeStampPLL, Base, Count, pChUsr->PLLData.PLLTimer.Id);

			/* Allocate drift strukture with NULL MAC address and subdomain for diagnosis
			*/
			pDrift->MasterAddr = NoMacAddr;
			GSY_MEMSET_LOCAL(&pDrift->Subdomain, 0, sizeof(GSY_PTCP_UUID));

			/* Set Drift for Master-SyncID to 0
			*/
			gsy_DriftSet(pChUsr->pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_MASTER);
		}
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterPLLStart() pChUsr=%X ", pChUsr);
}

/*****************************************************************************/
/* Internal function: gsy_MasterPLLSet()                                     */
/* Set local PTCP time strongly to reference time.                           */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterPLLSet(
GSY_CH_USR_PTR	pChUsr,
LSA_UINT64		Nanoseconds,
LSA_UINT32		CycleTimeStamp)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT32				Seconds = (LSA_UINT32)(Nanoseconds/GSY_NANOS_PER_SECOND);
	LSA_UINT32				Nanos = (LSA_UINT32)(Nanoseconds%GSY_NANOS_PER_SECOND);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterPLLSet() pChUsr=%X", pChUsr);

	pRbl = pChUsr->pSyncSetRbl;
	if  (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		if (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK))
		{
			/* 987888,1033202: Use PTCP offset with sign on INTEL i210
			*/
			GSY_LOWER_RQB_SYNC_MODE_SET_OFFSET(pRbl);
			GSY_LOWER_RQB_SYNC_SIGN_SET_POSITIVE(pRbl);
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pChUsr, 0); // remove when supported
		}
		else
		{
			/* Use absolute PTCP time with local timestamp
			*/
			GSY_LOWER_RQB_SYNC_MODE_SET_ABSOLUTE(pRbl);
			GSY_LOWER_RQB_SYNC_TIMESTAMP_SET(pRbl, CycleTimeStamp);
		}
		GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, Nanos);
		GSY_LOWER_RQB_SYNC_SECONDS_SET(pRbl, Seconds);
		GSY_LOWER_RQB_SYNC_RCF_INTERVAL_SET(pRbl, 0);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChUsr->pChSys->HandleLower);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_TIME_SET);
		GSY_LOWER_RQB_USER_PTR_SET(pRbl, LSA_NULL);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

		/* Send lower Request
		*/
		pChUsr->pSyncSetRbl = LSA_NULL;
		GSY_REQUEST_LOWER(pRbl, pChUsr->pChSys->pSys);
	}
	else
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLSet() pChUsr=%X: NO lower SyncSetRbl", pChUsr);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBL, pChUsr, 0);
	}
	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterPLLSet() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal function: gsy_MasterPLLGet()                                     */
/* Read the ExtPLL input                                                     */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterPLLGet(
GSY_CH_USR_PTR  		pChUsr,
LSA_UINT8				SyncId)
{
	LSA_UINT16	RetVal;
	LSA_UINT32	TimeStampMin, TimeStampMax, TimeStampPLL, TimeStampCB;
	LSA_UINT32	OffsetLast, OffsetNext, TimeStampSet;
	LSA_INT32	OffsetReal, OffsetTicks = 0;
	LSA_INT64	OffsetNanos;
	LSA_UINT64	/*DiffTS,*/ Nanoseconds, NanosecondsLast, NanosecondsNext, NanosecondsCB, NanosecondsPLL, NanosecondsSet;
	LSA_UINT64	PLLInterval = (LSA_UINT64)pChUsr->PLLData.CycleLength * (LSA_UINT64)pChUsr->PLLData.ReductionRatio;		//AP01236058
	LSA_UINT8	OldState = pChUsr->PLLData.State;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterPLLGet() pChUsr=%X State=%u", pChUsr, OldState);

	if ((pChUsr->PLLData.State != GSY_PLL_STATE_SYNC_SLEEP)
	&&  (pChUsr->PLLData.State != GSY_PLL_STATE_SYNC_WAIT_SET))
	{
		if (GSY_PLL_STATE_SYNC_WAIT_OK == pChUsr->PLLData.State)
		{
			/* Waiting after SET
			*/
			pChUsr->PLLData.ActWaitCount++;
			if (pChUsr->PLLData.ActWaitCount > pChUsr->PLLData.SetWaitCount)
			{
				/* End of waiting after SET
				*/
				pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_SET;
			}
			else if (1 == pChUsr->PLLData.ActWaitCount)
			{
				/* For diag indication of state change
				*/
				OldState = GSY_PLL_STATE_SYNC_WAIT_SET;
			}
		}

		/* Save old time values and get new
		*/
		pChUsr->PLLData.Old = pChUsr->PLLData.New;
		RetVal = GSY_SYNC_GET_EXT_PLL_IN(&pChUsr->PLLData.New.TimeStampPLL, &pChUsr->PLLData.New.Seconds,
						&pChUsr->PLLData.New.Nanos, &pChUsr->PLLData.New.TimeStampCB, SyncId, pChUsr->pSys);

		if (LSA_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLGet() pChUsr=%X: GSY_SYNC_GET_EXT_PLL_IN: RetVal=0x%x",
							pChUsr, RetVal);
			gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
		}
		else
		{
			/* Check for new ExtPLL-IN signal
			*/
			if (pChUsr->PLLData.Old.TimeStampPLL == pChUsr->PLLData.New.TimeStampPLL)
			{
				/* No new ExtPLL-IN signal
				 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
				 * lrgi210: PLLWaitMillis
				*/
				pChUsr->PLLData.PLLWaitMillis += pChUsr->PLLData.PLLInterval;
				//DiffTS = (LSA_UINT64)(pChUsr->PLLData.New.TimeStampCB - pChUsr->PLLData.Old.TimeStampCB) * (LSA_UINT64)pChUsr->pChSys->RxTxNanos;
				GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_MasterPLLGet() Seconds=%u Nanos=%u TimeStampPLL=%u PLLWaitMillis=%u: no new ExtPLL signal",
							pChUsr->PLLData.New.Seconds, pChUsr->PLLData.New.Nanos, pChUsr->PLLData.New.TimeStampPLL, pChUsr->PLLData.PLLWaitMillis);

//				if (DiffTS > PLLInterval)
				if (pChUsr->PLLData.PLLWaitMillis > PLLInterval/GSY_NANOS_PER_MILLISECOND)
				{
					/* Missing ExtPLL-IN signal
					*/
					pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_WAIT;
				}
			}
			else
			{
				/* Calculate last cycle begin offset to last ExtPLL-IN
				*/
				if  (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK))
				{
					/* lrgi210: calculation not required because PTCP time at last PLL-In is returned in Seconds and Nanos
					*/
					TimeStampMin = 0;
					TimeStampMax = 0;
					OffsetNanos = 0;
				}
				else
				{
					TimeStampMin = pChUsr->PLLData.New.TimeStampPLL - (pChUsr->PLLData.CycleLength / pChUsr->pChSys->RxTxNanos);
					TimeStampMax = pChUsr->PLLData.New.TimeStampPLL + (LSA_UINT32)(PLLInterval / pChUsr->pChSys->RxTxNanos);
					TimeStampPLL = pChUsr->PLLData.New.TimeStampPLL;
					TimeStampCB = pChUsr->PLLData.New.TimeStampCB;
					if (TimeStampMin > TimeStampMax)
					{
						/* Shift all values to positive area
						*/
						GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLGet() TimeStampMin=%u TimeStampMax=%u: >>>>>>> TimeStampCB=%u -- TimeStampPLL=%u",
										TimeStampMin, TimeStampMax, pChUsr->PLLData.New.TimeStampCB, pChUsr->PLLData.New.TimeStampPLL);

						TimeStampMin += ((pChUsr->PLLData.CycleLength + (LSA_UINT32)PLLInterval) / pChUsr->pChSys->RxTxNanos);
						TimeStampMax += ((pChUsr->PLLData.CycleLength + (LSA_UINT32)PLLInterval) / pChUsr->pChSys->RxTxNanos);
						TimeStampPLL += ((pChUsr->PLLData.CycleLength + (LSA_UINT32)PLLInterval) / pChUsr->pChSys->RxTxNanos);
						TimeStampCB += ((pChUsr->PLLData.CycleLength + (LSA_UINT32)PLLInterval) / pChUsr->pChSys->RxTxNanos);
					}

					if ((TimeStampCB >= TimeStampPLL) && (TimeStampCB < TimeStampMax))
					{
						/* Offset in ticks positive (local clock too fast) and possible multiple of cycle length
						*/
						OffsetTicks = (LSA_INT32)(TimeStampCB - TimeStampPLL);
					}
					else if ((TimeStampCB > TimeStampMin) && (TimeStampCB < TimeStampPLL))
					{
						/* Offset in ticks negative (local clock too slow)
						*/
						OffsetTicks = -(LSA_INT32)(TimeStampPLL - TimeStampCB);
					}
					else
					{
						/* Something is wrong
						*/
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLGet() TimeStampMin=%u TimeStampMax=%u: Invalid TimeStampCB=%u or TimeStampPLL=%u",
										TimeStampMin, TimeStampMax, pChUsr->PLLData.New.TimeStampCB, pChUsr->PLLData.New.TimeStampPLL);
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLGet() TimeStampMin=%u TimeStampMax=%u: Invalid TimeStampCB=%u or TimeStampPLL=%u",
										TimeStampMin, TimeStampMax, TimeStampCB, TimeStampPLL);
						RetVal = GSY_RET_ERR_PARAM;
						//tmp:gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
					}
					OffsetNanos = (LSA_INT64)(pChUsr->pChSys->RxTxNanos) * OffsetTicks;
				}
				if (GSY_RET_OK == RetVal)
				{
					/* Calculate Time at last ExtPLL-IN and normalize it to the nearest muliple of the PLLInterval
					*/
					NanosecondsCB   = (LSA_UINT64)pChUsr->PLLData.New.Seconds * GSY_NANOS_PER_SECOND;
					NanosecondsCB  += (LSA_UINT64)pChUsr->PLLData.New.Nanos;
					NanosecondsPLL  = (LSA_UINT64)((LSA_INT64)NanosecondsCB - OffsetNanos);
					NanosecondsPLL -= pChUsr->PLLData.PLLHWDelay;
					NanosecondsLast = NanosecondsPLL - (NanosecondsPLL % PLLInterval);
					NanosecondsNext = NanosecondsLast + PLLInterval;
					OffsetLast = (LSA_UINT32)(NanosecondsPLL - NanosecondsLast);
					OffsetNext = (LSA_UINT32)(NanosecondsNext - NanosecondsPLL);

					/* Calculate real offset at last ExtPLL-IN
					if (NanosecondsCB >= NanosecondsPLL)
						OffsetReal = (NanosecondsCB - NanosecondsPLL) % pChUsr->PLLData.CycleLength;
					else
						OffsetReal = -(LSA_INT32)((NanosecondsPLL - NanosecondsCB) % pChUsr->PLLData.CycleLength);
					*/
					if (OffsetLast <= OffsetNext)
					{
						OffsetReal = (LSA_INT32)OffsetLast;
						Nanoseconds = NanosecondsLast;
					}
					else
					{
						OffsetReal = -(LSA_INT32)OffsetNext;
						Nanoseconds = NanosecondsNext;
					}

					if ((GSY_PLL_STATE_SYNC_START == pChUsr->PLLData.State)
					||  (GSY_PLL_STATE_SYNC_WAIT == pChUsr->PLLData.State)
					||  (OffsetReal > (LSA_INT32)(pChUsr->PLLData.PLLWindow * GSY_SYNC_WINDOW_RESET_FACTOR))
					||  (OffsetReal < (LSA_INT32)(-((LSA_INT32)pChUsr->PLLData.PLLWindow * GSY_SYNC_WINDOW_RESET_FACTOR))))
					{
						GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"-1- gsy_MasterPLLGet() Seconds=%u Nanos=%u TimeStampCB=%u TimeStampPLL=%u TimeStampMin=%u TimeStampMax=%u OffsetTicks=%d OffsetNanos=%d",
									pChUsr->PLLData.New.Seconds, pChUsr->PLLData.New.Nanos, pChUsr->PLLData.New.TimeStampCB, pChUsr->PLLData.New.TimeStampPLL, TimeStampMin, TimeStampMax, OffsetTicks, (LSA_INT32)OffsetNanos);
						GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"-2- gsy_MasterPLLGet() NanosecondsCB=%u,%09u NanosecondsPLL=%u,%09u NanosecondsLast=%u,%09u NanosecondsNext=%u,%09u",
									(LSA_UINT32)(NanosecondsCB / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsCB % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsPLL / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsPLL % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsLast / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsLast % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsNext / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsNext % GSY_NANOS_PER_SECOND));

						if  (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK))
						{
							/* lrgi210: SET is done by offset in seconds/nanos and sign in timestamp
							*/
							if (OffsetReal < 0)
							{
								NanosecondsSet = (LSA_UINT32)(-OffsetReal);
								TimeStampSet = 0;
							}
							else
							{
								NanosecondsSet = (LSA_UINT32)(OffsetReal);
								TimeStampSet = 1;
							}
						}
						else
						{
							/* Calculate Time and timstamp at next ExtPLL-IN
							*/
							NanosecondsSet = Nanoseconds + PLLInterval;
							TimeStampSet = pChUsr->PLLData.New.TimeStampPLL + (LSA_UINT32)((PLLInterval - pChUsr->PLLData.PLLHWDelay) / pChUsr->pChSys->RxTxNanos);
						}
						GSY_MASTER_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLGet() OffsetReal=%d --> set: NanosecondsSet=0x%08x%08x (%u,%09us) TimeStampSet=%u",
									OffsetReal, (LSA_UINT32)(NanosecondsSet >> 32), (LSA_UINT32)NanosecondsSet, (LSA_UINT32)(NanosecondsSet / GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsSet % GSY_NANOS_PER_SECOND), TimeStampSet);

						/* Reset sync control algorithm
						*/
						RetVal = gsy_SyncReset(0, SyncId, pChUsr);
						if (LSA_RET_OK != RetVal)
						{
							GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_MasterPLLGet() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
											pChUsr, SyncId, RetVal);
							gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, LSA_NULL, 0);
						}
						else
						{
							/*  Set the new state and time
							*/
							pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_WAIT_SET;
							gsy_MasterPLLSet(pChUsr, NanosecondsSet, TimeStampSet);
						}
					}
					else
					{
						GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"1-- gsy_MasterPLLGet() Seconds=%u Nanos=%u TimeStampCB=%u TimeStampPLL=%u TimeStampMin=%u TimeStampMax=%u OffsetTicks=%d OffsetNanos=%d",
									pChUsr->PLLData.New.Seconds, pChUsr->PLLData.New.Nanos, pChUsr->PLLData.New.TimeStampCB, pChUsr->PLLData.New.TimeStampPLL, TimeStampMin, TimeStampMax, OffsetTicks, (LSA_INT32)OffsetNanos);
						GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"2-- gsy_MasterPLLGet() NanosecondsCB=%u,%09u NanosecondsPLL=%u,%09u NanosecondsLast=%u,%09u NanosecondsNext=%u,%09u",
									(LSA_UINT32)(NanosecondsCB / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsCB % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsPLL / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsPLL % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsLast / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsLast % GSY_NANOS_PER_SECOND),
									(LSA_UINT32)(NanosecondsNext / GSY_NANOS_PER_SECOND), (LSA_UINT32)(NanosecondsNext % GSY_NANOS_PER_SECOND));

						/* Call controller and set state
						*/
						pChUsr->PLLData.AdjustSeconds = (LSA_UINT32)(Nanoseconds / GSY_NANOS_PER_SECOND);
						pChUsr->PLLData.AdjustNanos = (LSA_UINT32)(Nanoseconds % GSY_NANOS_PER_SECOND);
						pChUsr->PLLData.AdjustOffset = OffsetReal;

						RetVal = gsy_SyncAlgorithm(pChUsr->PLLData.AdjustSeconds, pChUsr->PLLData.AdjustNanos, pChUsr->PLLData.AdjustOffset,
													&pChUsr->PLLData.AdjustInterval, &pChUsr->PLLData.AdjustTermP, &pChUsr->PLLData.AdjustTermI,
													SyncId, pChUsr);

						if (LSA_RET_OK != RetVal)
						{
							GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterPLLGet() pChUsr=%X: GSY_SYNC_ALGORITHM: RetVal=0x%x",
											pChUsr, RetVal);
						}
						else
						{
							GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterPLLGet() OffsetReal=%d --> control: AdjustTime=%u,%09us AdjustInterval=%d",
										pChUsr->PLLData.AdjustOffset, pChUsr->PLLData.AdjustSeconds, pChUsr->PLLData.AdjustNanos, pChUsr->PLLData.AdjustInterval);

							if (GSY_PLL_STATE_SYNC_WAIT_OK != pChUsr->PLLData.State)
							{
								if ((OffsetReal > (LSA_INT32)pChUsr->PLLData.PLLWindow)
								||  (OffsetReal < -(LSA_INT32)pChUsr->PLLData.PLLWindow))
								{
									pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_OUT;
								}
								else
								{
									pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_OK;
								}
							}
						}
					}
				}
			}
		}
		if (OldState != pChUsr->PLLData.State)
		{
			GSY_DRIFT_PTR pDrift = &pChUsr->pChSys->Drift[SyncId][0];

			if (GSY_PLL_STATE_SYNC_WAIT == OldState)
			{
				pChUsr->PLLData.PLLWaitMillis = 0;		//lrgi210
			}

			GSY_MASTER_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterPLLGet(%02x) PLL state change [%u]->[%u]",
					SyncId, OldState, pChUsr->PLLData.State);

			/* Send diagnosis indication on PLL state change
			*/
			pDrift->Diag.StateSync = LSA_FALSE;
			if (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
			{
				pDrift->Diag.StateSync = LSA_TRUE;
			}

			gsy_DiagUserInd(GSY_DIAG_SOURCE_MASTER, pChUsr->pChSys, pDrift,
							0      /* LSA_UINT32: MasterSeconds */,
							0      /* LSA_UINT32: MasterNanoseconds */,
							0      /* LSA_INT32: Offset */,
							0      /* LSA_INT32: AdjustInterval */,
							0      /* LSA_INT32: UserValue1 */,
							0      /* LSA_INT32: UserValue2 */,
							0      /* LSA_INT32: SetTimeHigh */,
							0      /* LSA_INT32: SetTimeLow */,
							0	   /* LSA_INT16: PortId */);
		}
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterPLLGet() pChUsr=%X State=%u",
						pChUsr, pChUsr->PLLData.State);
}

/*****************************************************************************/
/* Internal function: gsy_MasterSyncFill()                                   */
/* Fill data into sync frame buffer                                          */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSyncFill(
GSY_CH_USR_PTR  		pChUsr,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_SYNC};
	GSY_MAC_ADR_TYPE	DstAddrFu = {GSY_MULTICAST_ADDR_FUSYNC};
	LSA_UINT16			FrameId;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSyncFill() pChUsr=%X pPdu=%X SyncId=0x%02x",
					pChUsr, pPdu, pChUsr->SyncId);

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_SYNC_PDU_SIZE, 0);

#ifdef GSY_USE_VLAN
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPdu->PduHdr.VlanTag = VlanTag;
	}
#endif
#ifdef GSY_MESSAGE
	/* TODO: only clock sync w/o FollowUp?
	*/
#endif /* GSY_MESSAGE */
	if (pChUsr->SyncId == GSY_SYNCID_CLOCK)
	{
		pPdu->PduHdr.DstMacAdr = DstAddr;
		FrameId = GSY_FRAMEID_SYNC | pChUsr->SyncId;	//270206lrg001
	}
	else
	{
		pPdu->PduHdr.DstMacAdr = DstAddrFu;
		FrameId = GSY_FRAMEID_FUSYNC | pChUsr->SyncId;	//270206lrg001
	}
	pPdu->PduHdr.FrameId.Word = GSY_SWAP16(FrameId);	//270206lrg001
	pPdu->PduHdr.DstMacAdr.MacAdr[5] |= pChUsr->SyncId;

	pPdu->PduHdr.SrcMacAdr = pChUsr->pChSys->MACAddr;
	pPdu->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);

	pPdu->PtcpData.Sync.TLV.Subdomain.SubdomainUUID = pChUsr->SyncData.SubdomainID;
	pPdu->PtcpData.Sync.TLV.Subdomain.MasterSourceAddress = pChUsr->pChSys->MACAddr;
	pPdu->PtcpData.Sync.TLV.Subdomain.TypeLen.Word = GSY_SWAP16(GSY_TLV_SUBDOMAIN_TYPELEN);

	pPdu->PtcpData.Sync.TLV.Time.TypeLen.Word = GSY_SWAP16(GSY_TLV_TIME_TYPELEN);

	pPdu->PtcpData.Sync.TLV.TimeExt.Flags.Word = GSY_SWAP16(0);
	pPdu->PtcpData.Sync.TLV.TimeExt.CurrentUTCOffset.Word = GSY_SWAP16(pChUsr->Master.CurrentUTCOffset);
	pPdu->PtcpData.Sync.TLV.TimeExt.Reserved1 = 0;
	pPdu->PtcpData.Sync.TLV.TimeExt.Reserved2 = 0;
	pPdu->PtcpData.Sync.TLV.TimeExt.TypeLen.Word = GSY_SWAP16(GSY_TLV_TIMEEXT_TYPELEN);

	pPdu->PtcpData.Sync.TLV.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
	pPdu->PtcpData.Sync.TLV.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;
	pPdu->PtcpData.Sync.TLV.Master.ClockClass = pChUsr->Master.LocalMaster.Class;
	pPdu->PtcpData.Sync.TLV.Master.ClockAccuracy = pChUsr->Master.LocalMaster.Accuracy;
	pPdu->PtcpData.Sync.TLV.Master.ClockVariance.Word = GSY_SWAP16(pChUsr->Master.LocalMaster.Variance);
	pPdu->PtcpData.Sync.TLV.Master.TypeLen.Word = GSY_SWAP16(GSY_TLV_MASTER_TYPELEN);

	pPdu->PtcpData.Sync.TLV.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSyncFill() pChUsr=%X FrameId=0x%04x",
					pChUsr, FrameId);
}

/*****************************************************************************/
/* Internal function: gsy_MasterFuFill()                                     */
/* Fill data into FollowUp frame buffer                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterFuFill(
GSY_CH_USR_PTR  		pChUsr,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_SYNC_FU};
	LSA_UINT16			FrameId = GSY_FRAMEID_SYNC_FU | pChUsr->SyncId;		//270206lrg001

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterFuFill() pChUsr=%X pPdu=%X FrameId=0x%04x",
					pChUsr, pPdu, FrameId);

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_SYNC_FU_PDU_SIZE, 0);

	DstAddr.MacAdr[5] |= pChUsr->SyncId;
	pPdu->PduHdr.DstMacAdr = DstAddr;
	pPdu->PduHdr.SrcMacAdr = pChUsr->pChSys->MACAddr;
#ifdef GSY_USE_VLAN
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPdu->PduHdr.VlanTag = VlanTag;
	}
#endif
	pPdu->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
	pPdu->PduHdr.FrameId.Word = GSY_SWAP16(FrameId);		//270206lrg001

	pPdu->PtcpData.SyncFu.Subdomain.SubdomainUUID = pChUsr->SyncData.SubdomainID;
	pPdu->PtcpData.SyncFu.Subdomain.MasterSourceAddress = pChUsr->pChSys->MACAddr;
	pPdu->PtcpData.SyncFu.Subdomain.TypeLen.Word = GSY_SWAP16(GSY_TLV_SUBDOMAIN_TYPELEN);

	pPdu->PtcpData.SyncFu.Time.TypeLen.Word = GSY_SWAP16(GSY_TLV_TIME_TYPELEN);

	pPdu->PtcpData.SyncFu.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterFuFill() pChUsr=%X", pChUsr);
}


/*****************************************************************************/
/* Internal function: gsy_MasterAnnoFill()                                   */
/* Fill data into announce frame buffer                                      */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterAnnoFill(
GSY_CH_USR_PTR  		pChUsr,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_ANNOUNCE};
	LSA_UINT16			FrameId = GSY_FRAMEID_ANNOUNCE | pChUsr->SyncId;	//270206lrg001
	GSY_LOWER_TX_ANNO_PDU_PTR	pPduA = (GSY_LOWER_TX_ANNO_PDU_PTR)(void*)pPdu;	//200410lrg001

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAnnoFill() pChUsr=%X pPdu=%X FrameId=0x%04x",
					pChUsr, pPdu, FrameId);

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_ANNOUNCE_PDU_SIZE, 0);

	DstAddr.MacAdr[5] |= pChUsr->SyncId;
	pPduA->PduHdr.DstMacAdr = DstAddr;
	pPduA->PduHdr.SrcMacAdr = pChUsr->pChSys->MACAddr;
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPduA->PduHdr.VlanTag = VlanTag;
	}
	pPduA->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
	pPduA->PduHdr.FrameId.Word = GSY_SWAP16(FrameId);	//270206lrg001

	pPduA->PtcpData.Announce.Subdomain.SubdomainUUID = pChUsr->SyncData.SubdomainID;
	pPduA->PtcpData.Announce.Subdomain.MasterSourceAddress = pChUsr->pChSys->MACAddr;
	pPduA->PtcpData.Announce.Subdomain.TypeLen.Word = GSY_SWAP16(GSY_TLV_SUBDOMAIN_TYPELEN);

	pPduA->PtcpData.Announce.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
	pPduA->PtcpData.Announce.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;
	pPduA->PtcpData.Announce.Master.ClockClass = pChUsr->Master.LocalMaster.Class;
	pPduA->PtcpData.Announce.Master.ClockAccuracy = pChUsr->Master.LocalMaster.Accuracy;
	pPduA->PtcpData.Announce.Master.ClockVariance.Word = GSY_SWAP16(pChUsr->Master.LocalMaster.Variance);
	pPduA->PtcpData.Announce.Master.TypeLen.Word = GSY_SWAP16(GSY_TLV_MASTER_TYPELEN);

	pPduA->PtcpData.Announce.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAnnoFill() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterAnnoEnable()                          */
/* Enable receiving of Announce frames                                       */
/*****************************************************************************/
static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MasterAnnoEnable(
GSY_CH_USR_PTR			pChUsr)
{
	GSY_CH_SYS_PTR			pChSys = pChUsr->pChSys;
	LSA_UINT32				NumRqb;
	LSA_UINT16				RetVal = LSA_RET_OK;
	LSA_UINT16				RespVal = GSY_RSP_OK;
	GSY_MAC_ADR_TYPE    	AnnoMcAddr = {GSY_MULTICAST_ADDR_ANNOUNCE};

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAnnoEnable() pChUsr=%X SyncId=0x%02x",
						pChUsr, pChUsr->SyncId);

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_MasterAnnoEnable() pChSys=%X: Allocating %u announce recv RQBs",
					pChSys, (pChSys->PortCount*GSY_NUM_RQB_ANNOUNCE_RECV)); 

	for (NumRqb = 0; NumRqb < (LSA_UINT32)GSY_NUM_RQB_ANNOUNCE_RECV*pChSys->PortCount; NumRqb++)
	{
		/* 101208lrg001, 1126470 : Allocate new  receive requestblocks and  data buffer
		 * for announce frames and put RQBs into lower layer
		*/
		RespVal = gsy_AllocEthRecv(pChSys, LSA_NULL, LSA_NULL, pChUsr->SyncId, GSY_ANNO_TYPE_ID, LSA_TRUE);
		if (GSY_RSP_OK != RespVal)
		{
			/* NOTE: Error in PrmCommit -> MasterStart is FATAL -> no cancel of allocated RQBs here
			*/
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterAnnoEnable() RespVal=0x%04x from gsy_AllocEthRecv()",
					RespVal);
			break;
		}
	}

	if (GSY_RSP_OK == RespVal)
	{
		/* Enable Announce multicast address
		*/
		AnnoMcAddr.MacAdr[5] |= pChUsr->SyncId;
		RetVal = gsy_MucAddrSet(pChSys,	LSA_NULL, AnnoMcAddr.MacAdr, LSA_TRUE, LSA_TRUE);
		if (GSY_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterAnnoEnable() RetVal=0x%04x from gsy_MucAddrSet()",
					RetVal);
			RespVal = GSY_RSP_ERR_RESOURCE;
		}
	}
	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAnnoEnable() RespVal=0x%04x",
						RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterStart()                               */
/* Start a sync master on the user channel                                   */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MasterStart(
GSY_CH_USR_PTR			pChUsr)
{
	GSY_CH_SYS_PTR			pChSys = pChUsr->pChSys;
	GSY_LOWER_RQB_PTR_TYPE  pRbl;
	GSY_LOWER_TX_PDU_PTR	pPdu;
	LSA_UINT16 Base, Count, PortId;
	LSA_INT					Idx;
	LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT16 RespVal = GSY_RSP_ERR_SEQUENCE;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterStart() pChUsr=%X SyncId=0x%02x",
						pChUsr, pChUsr->SyncId);

	/* If there is no master running on this channel...
	*/
	if (GSY_MASTER_STATE_OFF == pChUsr->Master.State)
	{
		/* Build clock parameters and initialise BMA data
		*/
		pChUsr->Master.SyncIdBit = 1;
		pChUsr->Master.SyncIdBit <<= pChUsr->SyncId;

		/* 161110lrg001: Rewiew Part 6: ClockClass 0xff -> 0x0d (default)
		*/
		pChUsr->Master.EpochNumber = 0;
		pChUsr->Master.LocalMaster.Class = 0x0d;
		pChUsr->Master.LocalMaster.Variance = 0;
		pChUsr->Master.LocalMaster.Accuracy = 0xfe;

		pChUsr->Master.LocalMaster.MacAddr = pChSys->MACAddr;
		pChUsr->Master.LocalMaster.Receipt = GSY_ENTRY_SM_USED;
		pChUsr->Master.LocalMaster.Priority1 = pChUsr->Master.Priority1;
		pChUsr->Master.LocalMaster.Priority2 = pChUsr->Master.Priority2;
		pChUsr->Master.pPriMaster = LSA_NULL;
		pChUsr->Master.pSecMaster = LSA_NULL;
		
		for (Idx = 0; Idx < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; Idx++)
		{
			pChUsr->Master.PriList[Idx].Receipt = GSY_ENTRY_SM_UNUSED;
			pChUsr->Master.SecList[Idx].Receipt = GSY_ENTRY_SM_UNUSED;
		}
		pChUsr->Master.PriListUsed = 0;
		pChUsr->Master.SecListUsed = 0;

		/* 120809lrg001
		*/
		pChUsr->Master.QueueSync.pFirst = LSA_NULL;
		pChUsr->Master.QueueSync.pLast  = LSA_NULL;
		pChUsr->Master.QueueSync.Count  = 0;
		pChUsr->Master.QueueFu.pFirst = LSA_NULL;
		pChUsr->Master.QueueFu.pLast  = LSA_NULL;
		pChUsr->Master.QueueFu.Count  = 0;
		pChUsr->Master.QueueAnnounce.pFirst   = LSA_NULL;
		pChUsr->Master.QueueAnnounce.pLast    = LSA_NULL;
		pChUsr->Master.QueueAnnounce.Count    = 0;
		pChUsr->Master.RbAnnoCount = 0;
		pChUsr->Master.RbSyncCount = 0;
		pChUsr->Master.RbFuCount = 0;

		/* Calculate best combination of Timerbase/-Count
		 * and allocate cyclic master send timer
		*/
		if ((10000 <= pChUsr->Master.SendInterval)
		&&  (0 == (pChUsr->Master.SendInterval % 10000)))
		{
			Base = LSA_TIME_BASE_10S;
			Count = pChUsr->Master.SendInterval / 10000;
		}
		else if ((1000 <= pChUsr->Master.SendInterval)
		     &&  (0 == (pChUsr->Master.SendInterval % 1000)))
		{
			Base = LSA_TIME_BASE_1S;
			Count = pChUsr->Master.SendInterval / 1000;
		}
		else if ((100 <= pChUsr->Master.SendInterval)
		     &&  (0 == (pChUsr->Master.SendInterval % 100)))
		{
			Base = LSA_TIME_BASE_100MS;
			Count = pChUsr->Master.SendInterval / 100;
		}
		else if ((10 <= pChUsr->Master.SendInterval)
		     &&  (0 == (pChUsr->Master.SendInterval % 10)))
		{
			Base = LSA_TIME_BASE_10MS;
			Count = pChUsr->Master.SendInterval / 10;
		}
		else
		{
			Base = LSA_TIME_BASE_1MS;
			Count = pChUsr->Master.SendInterval;
		}
		RetVal = gsy_TimerAlloc(&pChUsr->Master.SendTimer, LSA_TIMER_TYPE_CYCLIC, Base);

		if (LSA_RET_OK == RetVal)
			RetVal = gsy_TimerAlloc(&pChUsr->Master.StartupTimer, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_1S);

		if (LSA_RET_OK != RetVal)
			RespVal = GSY_RSP_ERR_RESOURCE;
		else
		{
			/* Allocate send RQBs
			*/
			RespVal = GSY_RSP_OK;
			for (PortId = 0; PortId < pChSys->PortCount; PortId++)
			{
				for (Idx = 0; Idx < GSY_NUM_RQB_MASTER_SEND; Idx++)
				{
					/* Allocate new send request block with parameter and data buffer
					 * for announce frames and put it into the queue
					*/
					RespVal = gsy_AllocEthSend(pChSys, &pChUsr->Master.QueueAnnounce, pChUsr, LSA_TRUE);
					if (GSY_RSP_OK == RespVal)
						pChUsr->Master.RbAnnoCount++;
					else
						break;

					/* 120809lrg001: Allocate Sync- and FU-RQBs only if there is no hardware
					 * support for sending CLOCK and TIME sync frames
					*/
					if ((GSY_HW_TYPE_MASTER_HW != (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MASTER_MSK))
					||  ((GSY_SYNCID_CLOCK != pChUsr->SyncId) && (GSY_SYNCID_TIME != pChUsr->SyncId)))
					{
						/* Allocate new send request block with parameter and data buffer
						 * for sync frames and put it into the queue
						*/
						RespVal = gsy_AllocEthSend(pChSys, &pChUsr->Master.QueueSync, pChUsr, LSA_FALSE);
						if (GSY_RSP_OK == RespVal)
							pChUsr->Master.RbSyncCount++;
						else
							break;
#ifdef GSY_MESSAGE
						/* TODO: only clock sync w/o FollowUp?
						*/
#endif /* GSY_MESSAGE */
						if (pChUsr->SyncId != GSY_SYNCID_CLOCK)
						{
							/* Allocate new send request block with parameter and data buffer
							 * for FU frames and put it into the queue
							*/
							RespVal = gsy_AllocEthSend(pChSys, &pChUsr->Master.QueueFu, pChUsr, LSA_FALSE);
							if (GSY_RSP_OK == RespVal)
								pChUsr->Master.RbFuCount++;
							else
								break;
						}
						pChUsr->Master.HwMaster = LSA_FALSE;
					}
					else
					{
						/* Control master in hardware
						*/
						pChUsr->Master.HwMaster = LSA_TRUE;
					}
				}
				if (GSY_RSP_OK != RespVal)
					break;
			}
			if (GSY_RSP_OK == RespVal)
			{
				GSY_MASTER_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--- gsy_MasterQueue(%02x) QueueSy.Count=%02u QueueSy.pFirst=%X",
								pChUsr->SyncId, pChUsr->Master.QueueSync.Count, pChUsr->Master.QueueSync.pFirst);

				GSY_MASTER_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--- gsy_MasterQueue(%02x) QueueFu.Count=%02u QueueFu.pFirst=%X",
								pChUsr->SyncId, pChUsr->Master.QueueFu.Count, pChUsr->Master.QueueFu.pFirst);

				GSY_MASTER_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--- gsy_MasterQueue(%02x) QueueAn.Count=%02u QueueAn.pFirst=%X",
								pChUsr->SyncId, pChUsr->Master.QueueAnnounce.Count, pChUsr->Master.QueueAnnounce.pFirst);

				/* Initialise send data buffer of RQBs in queues
				*/
				pRbl = pChUsr->Master.QueueSync.pFirst;
				while (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
				{
					pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
					gsy_MasterSyncFill(pChUsr, pPdu);
					pRbl = GSY_LOWER_RQB_NEXT_GET(pRbl);
				}
				pRbl = pChUsr->Master.QueueFu.pFirst;
				while (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
				{
					pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
					gsy_MasterFuFill(pChUsr, pPdu);
					pRbl = GSY_LOWER_RQB_NEXT_GET(pRbl);
				}
				pRbl = pChUsr->Master.QueueAnnounce.pFirst;
				while (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
				{
					pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);	//250610lrg001: was GSY_LOWER_RQB_SEND_SYNC_PTR_GET
					gsy_MasterAnnoFill(pChUsr, pPdu);
					pRbl = GSY_LOWER_RQB_NEXT_GET(pRbl);
				}

				/* First master state is STARTUP
				*/
				pChUsr->Master.State = GSY_MASTER_STATE_STARTUP;
				pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE;
				pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
				pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;

				/* Start sync slave
				*/
				RespVal = gsy_SlaveStart(pChUsr);
				if (GSY_RSP_OK == RespVal)
				{
					/* Start master timer 200207lrg001: initialise SendCount
					*/
#ifdef GSY_MASTER_TIMER_CHECK
					pChUsr->Master.OldSeqId = 1;
					pChUsr->Master.OldTimestamp = 0;
#endif
					pChUsr->Master.Timestamp = 0;
					pChUsr->Master.Seconds = 0;
					pChUsr->Master.Nanos = 0;
					pChUsr->Master.SeqId = 0;
					pChUsr->Master.AnnoSeqId = 0;
					pChUsr->Master.SendCount = 0;
					pChUsr->Event.PriMasterWaits = LSA_TRUE;

					/* 070907lrg001: Set MasterTimeout to SyncTakeover
					 * if not disabled - else to SyncTimeout
					 * Set reduction factor for send interval of announce frames
					*/
					if (GSY_SYNC_TAKEOVER_DISABLED != pChUsr->Slave.SyncTakeover)
						pChUsr->Master.TimeoutCount = pChUsr->Slave.SyncTakeover;
					else
						pChUsr->Master.TimeoutCount = pChUsr->Slave.SyncTimeout;
					if (GSY_SYNCID_CLOCK == pChUsr->SyncId)
						pChUsr->Master.AnnounceCount = GSY_ANNO_SEND_FACTOR_CLOCK;
					else
						pChUsr->Master.AnnounceCount = GSY_ANNO_SEND_FACTOR_TIME;

					for (Idx = 0; Idx < GSY_CH_MAX_PORTS ; Idx++)
					{
						pChUsr->Master.SeqIdSnd[Idx] = 0;
						pChUsr->Master.SeqIdCnf[Idx] = 0;
					}
					gsy_TimerStart(&pChUsr->Master.SendTimer, Count);

					if (0 != pChUsr->Master.StartupTime)
					{
						/* 070907lrg001: Run startup timer if MasterStartup != DISABLE
						*/
						gsy_TimerStart(&pChUsr->Master.StartupTimer, pChUsr->Master.StartupTime);
						pChUsr->Master.Startup = LSA_TRUE;
						GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStart(%02x) master state change ->STARTUP",
									pChUsr->SyncId);
					}
					else
					{
						pChUsr->Master.Startup = LSA_FALSE;
						pChUsr->Master.pSecMaster = LSA_NULL;
						if (GSY_SYNC_PRIO1_PRIMARY == GSY_SYNC_PRIO1_PRIO(pChUsr->Master.LocalMaster.Priority1))
						{
							/* 200907lrg001: if MasterStartup is disabled, the primary master is ACTIVE from start
							*/
							pChUsr->Master.LocalMaster.Priority1 |= GSY_SYNC_PRIO1_ACTIVE;
							pChUsr->Master.pSecMaster = &pChUsr->Master.LocalMaster;
							pChUsr->Master.pPriMaster = &pChUsr->Master.LocalMaster;	//140411lrg: AP01162398_BMA
						}
						pChUsr->Master.State = GSY_MASTER_STATE_QUICKUP;
						GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStart(%02x) master state change ->QUICKUP",
									pChUsr->SyncId);
					}
				}
				else
					pChUsr->Master.State = GSY_MASTER_STATE_OFF;
			}

			if (GSY_RSP_OK != RespVal)
			{
				/* On error: free allokated RQBs and timer
				*/
				(LSA_VOID)gsy_TimerClean(&pChUsr->Master.SendTimer);
				(LSA_VOID)gsy_TimerClean(&pChUsr->Master.StartupTimer);
				while (pChUsr->Master.QueueSync.Count != 0)
				{
					pRbl = pChUsr->Master.QueueSync.pFirst;
					gsy_DequeLower(&pChUsr->Master.QueueSync, pRbl);
					gsy_FreeLowerRb(pChUsr->pChSys, pRbl, LSA_FALSE);
				}
				while (pChUsr->Master.QueueFu.Count != 0)
				{
					pRbl = pChUsr->Master.QueueFu.pFirst;
					gsy_DequeLower(&pChUsr->Master.QueueFu, pRbl);
					gsy_FreeLowerRb(pChUsr->pChSys, pRbl, LSA_FALSE);
				}
				while (pChUsr->Master.QueueAnnounce.Count != 0)
				{
					pRbl = pChUsr->Master.QueueAnnounce.pFirst;
					gsy_DequeLower(&pChUsr->Master.QueueAnnounce, pRbl);
					gsy_FreeLowerRb(pChUsr->pChSys, pRbl, LSA_TRUE);
				}
			}
			else
			{
				/* 1126470: enable Announce MAC address and allocate receive RQBs
				*/
				RespVal = gsy_MasterAnnoEnable(pChUsr);
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterStart() MasterState=%u SlaveState=%u RespVal=0x%x",
					pChUsr->Master.State, pChUsr->Slave.State, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterStop()                                */
/* Stop a sync master on the user channel                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterStop(
GSY_CH_USR_PTR	pChUsr)
{
	GSY_LOWER_RQB_PTR_TYPE  pRbl;
	GSY_DRIFT_PTR	pDrift = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterStop() pChUsr=%X SyncId=0x%02x SlaveState=0x%x",
					pChUsr, pChUsr->SyncId , pChUsr->Slave.State);

	if ((pChUsr->Master.QueueAnnounce.Count == pChUsr->Master.RbAnnoCount)
	&&  (pChUsr->Master.QueueSync.Count == pChUsr->Master.RbSyncCount)
	&&  (pChUsr->Master.QueueFu.Count == pChUsr->Master.RbFuCount)
	&&  (pChUsr->qRTC2Rbl.Count == pChUsr->RTC2RblCount))
	{
		/* All RQBs are back from lower layer:
		 * free them and return upper RQB to user
		*/
		while (pChUsr->Master.QueueSync.Count != 0)
		{
			pRbl = pChUsr->Master.QueueSync.pFirst;
			gsy_DequeLower(&pChUsr->Master.QueueSync, pRbl);
			gsy_FreeLowerRb(pChUsr->pChSys, pRbl, LSA_FALSE);
		}
		while (pChUsr->Master.QueueFu.Count != 0)
		{
			pRbl = pChUsr->Master.QueueFu.pFirst;
			gsy_DequeLower(&pChUsr->Master.QueueFu, pRbl);
			gsy_FreeLowerRb(pChUsr->pChSys,pRbl, LSA_FALSE);
		}
		while (pChUsr->Master.QueueAnnounce.Count != 0)
		{
			pRbl = pChUsr->Master.QueueAnnounce.pFirst;
			gsy_DequeLower(&pChUsr->Master.QueueAnnounce, pRbl);
			gsy_FreeLowerRb(pChUsr->pChSys,pRbl, LSA_TRUE);
		}
		pChUsr->Master.State = GSY_MASTER_STATE_OFF;
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStop(%02x) SlaveState=%u master state change ->OFF", pChUsr->SyncId, pChUsr->Slave.State);

		if (!pChUsr->Master.HwMaster)
		{
			/* Free master drift structure
			*/
			pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
			if ((pDrift->SyncId == pChUsr->SyncId)
			&& 	GSY_MACADDR_CMP(pDrift->MasterAddr, pChUsr->Master.LocalMaster.MacAddr))
			{
				if (GSY_SUBDOMAIN_IS_NULL(pChUsr->SyncData.SubdomainID))
				{
					/* 070108lrg003: OHA indicates error on diagnosis
					 * if Diag.Subdomain is set when no sync record has been written
					*/
					pDrift->Diag.Subdomain = LSA_FALSE;
				}
				gsy_DriftReset(pChUsr->pChSys, pDrift, pChUsr->SyncId);
			}
		}

		/* The slave terminates the upper Request oder Close
		 * if he is stopped
		*/
		if ((GSY_SLAVE_STATE_STOP != pChUsr->Slave.State)
		&&  (GSY_SLAVE_STATE_STOPPING != pChUsr->Slave.State))
		{
			if (GSY_CHA_STATE_CLOSING == pChUsr->State)
			{
				/* 151210lrg001: PLL sync will finish close channel
				*/
				if (GSY_PLL_STATE_SYNC_OFF == pChUsr->PLLData.State)
				{
					gsy_CloseChUsrDone(pChUsr);
				}
			}
			else if (GSY_PRM_STATE_STOP == pChUsr->PrmState)
			{
				/* Switch PrmCommit state machine
				*/
				gsy_PrmCommitNext(pChUsr, LSA_NULL);
			}
			else
			{
				gsy_CallbackUsr(pChUsr, LSA_NULL, GSY_RSP_OK);
			}
		}
	}
	else
	{
		pChUsr->Master.State = GSY_MASTER_STATE_STOPPING;
		GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStop(%02x) RTC2RblCount=%u SyncRblCount=%u AnRblCount=%u master state change ->STOPPING",
							pChUsr->SyncId, pChUsr->RTC2RblCount, pChUsr->Master.QueueSync.Count, pChUsr->Master.QueueAnnounce.Count);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterStop() MasterState=0x%x", pChUsr->Master.State);
}

/*****************************************************************************/
/* Internal function: gsy_MasterSyncSend()                                   */
/* Send a sync frame                                                         */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSyncSend(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16				PortId)
{
	GSY_LOWER_RQB_PTR_TYPE  pRbl;
	GSY_LOWER_TX_PDU_PTR	pPdu;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSyncSend() pChUsr=%X SyncId=0x%02x Port=%u State=0x%x",
					pChUsr, pChUsr->SyncId, PortId, pChUsr->Master.State);

	/* 071205lrg002: if the last sent sync frame is confirmed...
	*/
	if (pChUsr->Master.SeqIdSnd[PortId] == pChUsr->Master.SeqIdCnf[PortId])
	{
		/* ...get a RQB with sync frame data pointer from queue
		*/
		pRbl = pChUsr->Master.QueueSync.pFirst;
		if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterSyncSend() pChUsr=%X: No sync RQB", pChUsr);
		}
		else
		{
			/* Complete sync frame with sequence ID, actual time and priority.
			 * Write timestamp of the PTCP time into header field for the HW receive timstamp
			 * So the HW is able to calculate the initial delay and store it in the frame:
			 * inital delay (cc_del) = send timestamp (cc_snd) - receive timestamp (cc_rcv)
			*/
			GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"<-- gsy_MasterQueue(%02x) QueueSy.Count=%02u QueueSy.pFirst=%X Port=%u",
							pChUsr->SyncId, pChUsr->Master.QueueSync.Count, pChUsr->Master.QueueSync.pFirst, PortId+1);

			pChUsr->Master.SeqIdSnd[PortId] = pChUsr->Master.SeqId;
			pChUsr->Master.SeqIdCnf[PortId] = pChUsr->Master.SeqId-1;
			gsy_DequeLower(&pChUsr->Master.QueueSync, pRbl);
			pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
			pPdu->PduHdr.SrcMacAdr = pChUsr->pChSys->Port[PortId].MACAddr;
			pPdu->PtcpData.Sync.PtcpHdr.SequenceID.Word = GSY_SWAP16(pChUsr->Master.SeqId);
			pPdu->PtcpData.Sync.PtcpHdr.ReceiveTS.Dword = GSY_SWAP32(pChUsr->Master.Timestamp);
			pPdu->PtcpData.Sync.PtcpHdr.Delay10ns.Dword = 0;
			pPdu->PtcpData.Sync.PtcpHdr.Delay1ns = 0;
			pPdu->PtcpData.Sync.PtcpHdr.DelayNS.Dword = 0;
			pPdu->PtcpData.Sync.TLV.Time.EpochNumber.Word = GSY_SWAP16(pChUsr->Master.EpochNumber);
			pPdu->PtcpData.Sync.TLV.Time.Seconds.Dword = GSY_SWAP32(pChUsr->Master.Seconds);
			pPdu->PtcpData.Sync.TLV.Time.NanoSeconds.Dword = GSY_SWAP32(pChUsr->Master.Nanos);
			pPdu->PtcpData.Sync.TLV.TimeExt.CurrentUTCOffset.Word = GSY_SWAP16(pChUsr->Master.CurrentUTCOffset);
			pPdu->PtcpData.Sync.TLV.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
			pPdu->PtcpData.Sync.TLV.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;	//200308lrg001: Update running Master

			GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterSend(%02x,SY) Port=%u SeqId=%u Seconds=%u Nanos=%d Timestamp=%u",
							pChUsr->SyncId, PortId+1, pChUsr->Master.SeqId,
							pChUsr->Master.Seconds, pChUsr->Master.Nanos, pChUsr->Master.Timestamp);

			/* Send Sync frame
			 * lrgtst: Test: APDU-Status
			if (GSY_SYNCID_CLOCK == pChUsr->SyncId)
				gsy_SyncSend(pChUsr->pChSys, pRbl, GSY_SYNC_PDU_SIZE+2, (LSA_UINT16)(PortId+1), GSY_SWAP16(pPdu->PduHdr.FrameId.Word));
			else
			*/
				gsy_SyncSend(pChUsr->pChSys, pRbl, GSY_SYNC_PDU_SIZE, (LSA_UINT16)(PortId+1), GSY_SWAP16(pPdu->PduHdr.FrameId.Word));
		}
	}
	else
	{
		GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterSend(%02x) Port=%u: SeqId=%u not confirmed: Dropping Sync Frame SeqId=%u",
						pChUsr->SyncId, PortId+1, pChUsr->Master.SeqIdSnd[PortId], pChUsr->Master.SeqId);
	}
	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSyncSend() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterAnnoSend()                            */
/* Send an announce frame                                                    */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterAnnoSend(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16				PortId)
{
	GSY_LOWER_RQB_PTR_TYPE  	pRbl;
	GSY_LOWER_TX_ANNO_PDU_PTR	pPdu;	//200410lrg001

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAnnoSend() pChUsr=%X SyncId=0x%02x Port=%u State=0x%x",
					pChUsr, pChUsr->SyncId, PortId, pChUsr->Master.State);

	/* Get a RQB with announce frame data pointer from queue
	*/
	pRbl = pChUsr->Master.QueueAnnounce.pFirst;
	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterAnnoSend() pChUsr=%X: No announce RQB", pChUsr);
	}
	else
	{
		/* Complete announce frame with port MAC, sequence ID and priority.
		*/
		gsy_DequeLower(&pChUsr->Master.QueueAnnounce, pRbl);
		pPdu = (GSY_LOWER_TX_ANNO_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);	//200410lrg001, 250610lrg001: was GSY_LOWER_RQB_SEND_SYNC_PTR_GET
		pPdu->PduHdr.SrcMacAdr = pChUsr->pChSys->Port[PortId].MACAddr;
		pPdu->PtcpData.Sync.PtcpHdr.SequenceID.Word = GSY_SWAP16(pChUsr->Master.AnnoSeqId);
		pPdu->PtcpData.Announce.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
		pPdu->PtcpData.Announce.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;	//200308lrg001: Update running Master

		GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterSend(%02x,AN) Port=%u SeqId=%u",
						pChUsr->SyncId, PortId+1, pChUsr->Master.AnnoSeqId);

		/* Send announce frame without timestamp
		 * 101208lrg001: Announce with gsy_DataSend()
		 * 150109lrg001: PortId and PduLen exchanged
         * 110210lrg001: Send Announce with gsy_AnnoSend()
		*/
		gsy_AnnoSend(pChUsr->pChSys, pRbl, (LSA_UINT16)(PortId+1), GSY_ANNOUNCE_PDU_SIZE);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAnnoSend() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterStartupEnd()                          */
/* The startup phase is stopped by timer.                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterStartupEnd(
GSY_TIMER_PTR	pTimer)
{
	GSY_CH_USR_PTR	pChUsr = pTimer->pChUsr;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterStartupEnd() pTimer=%X pChUsr=%X Startup=%u State=0x%x",
					pTimer, pChUsr, pChUsr->Master.Startup, pTimer->pChUsr->Master.State);

	pChUsr->Master.Startup = LSA_FALSE;

	if ((GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
	&&  (0 == (pChUsr->Master.LocalMaster.Priority1 & GSY_SYNC_PRIO1_ACTIVE)))
	{
		/* 070907lrg001: Primary Master changes to ACTIVE
		*/
		pChUsr->Master.LocalMaster.Priority1 |= GSY_SYNC_PRIO1_ACTIVE;
		GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStartupEnd(%02x) master state change PRIMARY->ACTIVE",
							pChUsr->SyncId);

		if (pChUsr->Master.HwMaster)
		{
			GSY_DRIFT_PTR pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];

			gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_STOP, GSY_LOWER_MASTER_ACTIVE);

			/* Diagnose update ActiveBit in LocalPrio
			*/
			pDrift->Diag.StateSync = LSA_TRUE;
			gsy_DiagUserInd(GSY_DIAG_SOURCE_SYNC, pChUsr->pChSys, pDrift,
			    			0      /* LSA_UINT32: MasterSeconds */,
							0      /* LSA_UINT32: MasterNanoseconds */,
							0      /* LSA_INT32: Offset */,
							0      /* LSA_INT32: AdjustInterval */,
							0      /* LSA_INT32: UserValue1 */,
							0      /* LSA_INT32: UserValue2 */,
							0      /* LSA_INT32: SetTimeHigh */,
							0      /* LSA_INT32: SetTimeLow */,
							0	   /* LSA_INT16: PortId */);
		}
		else
		{
			/* 170408lrg002: Diagnose update ActiveBit in LocalPrio
			*/
			gsy_SyncInd(pChUsr, LSA_TRUE);
		}
	}
	else
	{
		if ((GSY_MASTER_STATE_STARTUP == pChUsr->Master.State)
		&&  (pChUsr->SyncPLLActive && pChUsr->PLLData.State != GSY_PLL_STATE_SYNC_SLEEP))
		{
			GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStartupEnd(%02x) master state change ->Startup.PLL",
							pChUsr->SyncId);

			/* Extend startup phase if PLL sync is not reached yet
			*/
			gsy_TimerStart(&pChUsr->Master.StartupTimer, pChUsr->Master.StartupTime);
			pChUsr->Master.Startup = LSA_TRUE;
		}
		else
		{
			GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStartupEnd(%02x) master state change ->Startup.End",
							pChUsr->SyncId);
		}
	}

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterStartupEnd() pTimer=%X pChUsr=%X Startup=%u State=0x%x",
					pTimer, pChUsr, pChUsr->Master.Startup, pChUsr->Master.State);
}

/*****************************************************************************/
/* Internal function: gsy_MasterSendReal()                                   */
/* Send Sync- and/or announce frame.                                         */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSendReal(
GSY_CH_USR_PTR	pChUsr)
{
	LSA_UINT16		PortId;
	LSA_UINT16		RetVal = GSY_RET_OK;
	LSA_BOOL		SendAnno = LSA_FALSE;
	LSA_BOOL		SendSync = LSA_FALSE;
	LSA_BOOL		PortAnno = LSA_FALSE;
	LSA_BOOL		PortSync = LSA_FALSE;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSendReal() pChUsr=%X State=0x%x",
					pChUsr, pChUsr->Master.State);

	if ((pChUsr->Master.Startup)
	&&  (0 == (pChUsr->Master.SendCount % pChUsr->Master.AnnounceCount))
	&&  ((0 == pChUsr->Master.SecListUsed)
//	 ||  (pChUsr->Master.LocalMaster.Priority1 & GSY_SYNC_PRIO1_ACTIVE)	300508lrg001
	 ||  (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
	 ||  (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)))
	{
		/* An announce frame should be send
		 * 151007lrg003: if there is already an other master: wait until local slave is synchronous
		 * else the other master stopps sending before the local one can takeover sending sync frames.
		 * 181007lrg001: only if the local master is not already active
		 * 300508lrg001: only in STARTUP state
		*/
		SendAnno = LSA_TRUE;
		pChUsr->Master.AnnoSeqId++;
	}

	if (!pChUsr->Master.HwMaster && (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State))
	{
		/* A sync frame should be send
		*/
		if ((GSY_SLAVE_STATE_WAIT_SET != pChUsr->Slave.State)
		&& (GSY_PLL_STATE_SYNC_WAIT_SET != pChUsr->PLLData.State)
		&& (GSY_PLL_STATE_SYNC_WAIT_OK != pChUsr->PLLData.State))
		{
			if ((pChUsr->SyncId == GSY_SYNCID_CLOCK)
			&&  (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)))
			{
				/* lrgi210: PTCP time is inserted in frame by hardware
				*/
                pChUsr->Master.EpochNumber = 0;
			    pChUsr->Master.Seconds = 0;
                pChUsr->Master.Nanos = 0;
                pChUsr->Master.Timestamp = 0;
				RetVal = LSA_RET_OK;
			}
			else
			{
				/* Read actual PTCP time with timestamp from the system
				*/
				RetVal = GSY_SYNC_GET(&pChUsr->Master.EpochNumber, &pChUsr->Master.Seconds, &pChUsr->Master.Nanos,
									  &pChUsr->Master.Timestamp, pChUsr->SyncId, pChUsr->pSys);
			}
			if (LSA_RET_OK == RetVal)
			{
#ifdef GSY_MASTER_TIMER_CHECK
				LSA_UINT32 Diff = (pChUsr->Master.Timestamp - pChUsr->Master.OldTimestamp) * pChUsr->pChSys->RxTxNanos;

				if (!((pChUsr->Master.OldSeqId == 1) && (pChUsr->Master.OldTimestamp == 0))
				&&  (pChUsr->Master.SeqId == pChUsr->Master.OldSeqId)
//				&&  (Diff > 200000000))
				&&  (Diff > 500000000))
				{
					/*Timestamp difference > 200ms --> FATAL
					*/
					GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterSend() pChUsr=%X SeqId=%u: Time since last GSY_SYNC_GET: %u ns",
								pChUsr, pChUsr->Master.SeqId, Diff);
					//gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_TIMER, pChUsr, 0);
				}
				pChUsr->Master.OldTimestamp = pChUsr->Master.Timestamp;
				pChUsr->Master.OldSeqId = pChUsr->Master.SeqId + 1;
#endif
				/* Use same SequenceID for frames on all ports
				*/
				SendSync = LSA_TRUE;
				pChUsr->Master.SeqId++;
			}
			else
			{
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterSend() pChUsr=%X: GSY_SYNC_GET: RetVal=0x%x",	pChUsr, RetVal);
			}
		}
		else
		{
			GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"*** gsy_MasterSend(%02x) SlaveState=%u PLLState=%u LastSeqId=%u: Waiting for slave/PLL state change after SET",
								pChUsr->SyncId, pChUsr->Slave.State, pChUsr->PLLData.State, pChUsr->Master.SeqId);
		}
	}

	if (SendAnno || !pChUsr->Master.HwMaster)
	{
		for (PortId = 0; PortId < pChUsr->pChSys->PortCount; PortId++)
		{
			/* If on this port is Forwarding allowed for frames of this SyncId (260210lrg001: moved below)
			 * 060706lrg001: and the LineDelay is measured
			*/
//			if ((pChUsr->pChSys->PortFwd[PortId].FwdMode & pChUsr->Master.SyncIdBit)
//			&&  (0 != pChUsr->pChSys->Port[PortId].SyncLineDelay))
			if (0 != pChUsr->pChSys->Port[PortId].SyncLineDelay)
			{
				/* 181105lrg002: and the port state is OK...
				 * 190907lrg001: send not longer if DISABLED
				*/
				if ((GSY_PORT_STATE_UP == pChUsr->pChSys->Port[PortId].State)
				 || (GSY_PORT_STATE_CLOSED == pChUsr->pChSys->Port[PortId].State))
				{
					/* 260210lrg001: Check forwarding output and input bit for announce frames here
					*/
					if ((pChUsr->pChSys->PortFwd[PortId].FwdMode & pChUsr->Master.SyncIdBit)
					||  (pChUsr->pChSys->PortInp[PortId].FwdMode & pChUsr->Master.SyncIdBit))
					{
						/* 070907lrg001: Primary Master sends Announce with reduction ratio while StartupTime
						 * and Slave Master stopps sending after StartupTime
						*/
						PortAnno = LSA_TRUE;
						if (SendAnno)
						{
							gsy_MasterAnnoSend(pChUsr, PortId);
						}
					}
					else if (SendAnno)
					{
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: FwdMode=%08x: Dropping Anno SeqId=%u",
										pChUsr->SyncId, PortId+1, pChUsr->pChSys->PortFwd[PortId].FwdMode, pChUsr->Master.AnnoSeqId);
					}

					/* 170707lrg001: Check ClockSyncOk from SyncTopoOk for SyncId 0 from ext.LinkInd
					 * 260210lrg001: Check forwarding output bit for sync frames here
					*/
					if ((pChUsr->pChSys->PortFwd[PortId].FwdMode & pChUsr->Master.SyncIdBit)
					&&  ((pChUsr->SyncId != GSY_SYNCID_CLOCK) || pChUsr->pChSys->Port[PortId].ClockSyncOk))
					{
						PortSync = LSA_TRUE;
						if (SendSync)
						{
							gsy_MasterSyncSend(pChUsr, PortId);
						}
					}
					else if (SendSync)
					{
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: ClockSyncOk=%u: Dropping Sync SeqId=%u",
										pChUsr->SyncId, PortId+1, pChUsr->pChSys->Port[PortId].ClockSyncOk, pChUsr->Master.SeqId);
					}
				}
				else
				{
					if (SendAnno)
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: PortState=0x%x: Dropping Anno SeqId=%u",
									pChUsr->SyncId, PortId+1, pChUsr->pChSys->Port[PortId].State, pChUsr->Master.AnnoSeqId);
					if (SendSync)
						GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: PortState=0x%x: Dropping Sync SeqId=%u",
									pChUsr->SyncId, PortId+1, pChUsr->pChSys->Port[PortId].State, pChUsr->Master.SeqId);
				}
			}
			else
			{
				if (SendAnno)
					GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: LineDelay=%u: Dropping Anno SeqId=%u",
								pChUsr->SyncId, PortId+1, pChUsr->pChSys->Port[PortId].SyncLineDelay, pChUsr->Master.AnnoSeqId);
				if (SendSync)
					GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_MasterSend(%02x) Port=%u: LineDelay=%u: Dropping Sync SeqId=%u",
								pChUsr->SyncId, PortId+1, pChUsr->pChSys->Port[PortId].SyncLineDelay, pChUsr->Master.SeqId);
			}
		}
	}

	/* 200207lrg001: Reduce Master aging (with 2)
	 * 070907lrg001: Reduce TimeoutAging (SyncMessages) with SyncTakeover
	 * 190907lrg001: Reduce Master StartupAging (AnnounceMessages) with 2.
	*/
	pChUsr->Master.SendCount++;

	GSY_FUNCTION_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSendReal() PortSync=%u PortAnno=%u SendAnno=%u SendSync=%u SendCount=%u",
					PortSync, PortAnno, SendAnno, SendSync, pChUsr->Master.SendCount);

	LSA_UNUSED_ARG(PortSync);
	LSA_UNUSED_ARG(PortAnno);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterSend()                                */
/* Timer input function of the Sync Master.                                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSend(
GSY_TIMER_PTR	pTimer)
{
	GSY_CH_USR_PTR	pChUsr = pTimer->pChUsr;
	LSA_UINT16		RetVal = GSY_RET_OK;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSend() pTimer=%X pChUsr=%X State=0x%x",
					pTimer, pChUsr, pChUsr->Master.State);

	if (GSY_MASTER_STATE_STOP == pChUsr->Master.State)
	{
		/* Master stopped:	stop and free Timers 250706lrg001: retry on error
		*/
		GSY_MASTER_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterSend(%02x) master state change ->STOP", pChUsr->SyncId);
		RetVal = gsy_TimerClean(&pChUsr->Master.SendTimer);
		if (LSA_RET_OK == RetVal)
		{
			RetVal = gsy_TimerClean(&pChUsr->Master.StartupTimer);
		}
		if (LSA_RET_OK == RetVal)
		{
			/* Start Master stop state machine
			*/
			(LSA_VOID)gsy_CancelAnno(pChUsr);
		}
	}
	else
	{
#ifdef GSY_PLL_TIMER_NONE
		/* Run PLL sync before gsy_MasterStateSet() if it is active
		*/
		if (pChUsr->SyncPLLActive && !pChUsr->Master.HwMaster)
		{
			gsy_MasterPLLGet(pChUsr);
		}
#endif
		if (GSY_MASTER_STATE_QUICKUP == pChUsr->Master.State)
		{
			/* Start Master immediately
			*/
			gsy_MasterStateSet(pChUsr);
		}
		/* 280208lrg001: Code moved to new function gsy_MasterSendReal()
		*/
		gsy_MasterSendReal(pChUsr);

		/* On every second "announce timer"...
		*/
		if (0 == (pChUsr->Master.SendCount % (pChUsr->Master.AnnounceCount*2)))
		{
			/* ...Do the aging of the Announce Master list and check whether this master is best master
			*/
			gsy_MasterAgingAnno(pChUsr);
		}

		/* 200809lrg001: No receiving of  SyncFrames at the HwMaster: pChUsr->Master.PriListUsed is always 0
		 * Aging will be called from Slave Takeover in gsy_DiagIndK32()
		 * 301110lrg001: always indication from slave

		if (!pChUsr->Master.HwMaster && PortSync && (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
		*/
//		&& (0 != pChUsr->Master.TimeoutCount) &&  (0 == (pChUsr->Master.SendCount % pChUsr->Master.TimeoutCount)))
//		{
			/* Instead of SlaveInd(MASTER_LOST): Aging of the Master list and check wether we will get primary Master
			*/
//			gsy_MasterAging(pChUsr);
//		}
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSend() SendCount=%u AnnounceCount=%u TimeoutCount=%u",
					pChUsr->Master.SendCount, pChUsr->Master.AnnounceCount, pChUsr->Master.TimeoutCount);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterSyncCnf()                             */
/* Confirmation for a sent Sync Frame                                        */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSyncCnf(
GSY_CH_USR_PTR			pChUsr,
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	GSY_LOWER_RQB_PTR_TYPE  pRbs;
	GSY_LOWER_TX_PDU_PTR	pPdu;
	GSY_LOWER_TX_PDU_PTR	pPduTx = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
	LSA_UINT16				SeqId = GSY_SWAP16(pPduTx->PtcpData.Sync.PtcpHdr.SequenceID.Word);
	LSA_UINT16				PortId = GSY_LOWER_RQB_SEND_SYNC_PORT_GET(pRbl);
	LSA_UINT16				Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	LSA_UINT32				Timestamp = GSY_LOWER_RQB_SEND_SYNC_TIME_GET(pRbl);
//	LSA_UINT32				SendDelay = 0;
	LSA_INT32				FuDelay = 0;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSyncCnf() pChUsr=%X pRbl=%X Port=%u SeqId=%u",
					pChUsr, pRbl, PortId, SeqId);

	/* 071205lrg002: Set confirmed SeqId
	*/
	pChUsr->Master.SeqIdCnf[PortId-1] = SeqId;

	if (GSY_LOWER_RSP_OK != Response)
	{
		GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterSyncCnf() pChUsr=%X pRbl=%X SeqId=%u: Send response=0x%x",
						pChUsr, pRbl, SeqId, Response);
	}
	else if (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
	{
#ifdef GSY_MESSAGE
		/* TODO: Only Clocksync without FollowUp?
		*/
#endif /* GSY_MESSAGE */
		if ((pChUsr->SyncId == GSY_SYNCID_TIME)
		&& (GSY_FRAMEID_FUTIME == GSY_SWAP16(pPduTx->PduHdr.FrameId.Word)))	// 1225375lrg
		{
			/* If BestPrimaryMaster and not clock synchronisation:
			 * Get RQB with data pointer to FollowUp frame from Queue
			*/
			pRbs = pChUsr->Master.QueueFu.pFirst;
			if (LSA_HOST_PTR_ARE_EQUAL(pRbs, LSA_NULL))
			{
				GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_MasterSyncCnf() pChUsr=%X: No follow up RQB", pChUsr);
			}
			else
			{
				GSY_MASTER_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"<-- gsy_MasterQueue(%02x) QueueFu.Count=%02u QueueFu.pFirst=%X Port=%u",
								pChUsr->SyncId, pChUsr->Master.QueueFu.Count, pChUsr->Master.QueueFu.pFirst, PortId);

				/* Complete FU frame with Sequence number and sent PTCP time.
				*/
				gsy_DequeLower(&pChUsr->Master.QueueFu, pRbs);
				pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbs);
				pPdu->PtcpData.SyncFu.PtcpHdr.SequenceID = pPduTx->PtcpData.Sync.PtcpHdr.SequenceID;
				pPdu->PtcpData.SyncFu.Time.EpochNumber = pPduTx->PtcpData.Sync.TLV.Time.EpochNumber;
				pPdu->PtcpData.SyncFu.Time.Seconds = pPduTx->PtcpData.Sync.TLV.Time.Seconds;
				pPdu->PtcpData.SyncFu.Time.NanoSeconds = pPduTx->PtcpData.Sync.TLV.Time.NanoSeconds;

				/* Calculate the value for the delay field dependent of the hardware used
				 * and correct the drift
				*/
				switch (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)
				{
				case GSY_HW_TYPE_ERTEC_200:
				case GSY_HW_TYPE_SOC:
					FuDelay = 0;
					break;

				case GSY_HW_TYPE_ERTEC_400:
					/* Correct the (incorrect) to sync frame written receive delay now in the FU
					*/
					FuDelay = (LSA_INT32)(GSY_LOWER_RQB_SEND_SYNC_LINE_DELAY_HW_GET(pRbl) * pChUsr->pChSys->RxTxNanos);

                    /* 310707js001 */
                    /* Without drift correction. value has been added without correction in sync frame */
                    /*
                    FuDelay += gsy_DriftCorrection(pChUsr->pChSys, FuDelay, (LSA_UINT32)pChUsr->SyncId);
                    */

					FuDelay = 0 - FuDelay;
					break;

				default:
					/* Delay between reading of PTCP time and sending of the Sync frame
					*/
					if (GSY_LOWER_RQB_SEND_SYNC_TS_VALID_GET(pRbl))
					{
						FuDelay  = (LSA_INT32)(Timestamp - GSY_SWAP32(pPduTx->PtcpData.Sync.PtcpHdr.ReceiveTS.Dword));
						FuDelay *= (LSA_INT32)(pChUsr->pChSys->RxTxNanos);
					}
					else
					{
						FuDelay = 0;
						GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterSyncCnf() pChUsr=%X pRbl=%X: Send timestamp not valid on HardwareType=%u",
								pChUsr, pRbl, (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK));
					}
					FuDelay += gsy_DriftCorrection(pChUsr->pChSys, (LSA_UINT32)(FuDelay), (LSA_UINT32)pChUsr->SyncId);
				}

				pPdu->PtcpData.SyncFu.PtcpHdr.Delay10ns.Dword = 0;
				pPdu->PtcpData.SyncFu.PtcpHdr.Delay1ns  = 0;
				pPdu->PtcpData.SyncFu.PtcpHdr.DelayNS.Dword  = GSY_SWAP32((LSA_UINT32)FuDelay);
				pPdu->PduHdr.SrcMacAdr = pChUsr->pChSys->Port[PortId-1].MACAddr;

				GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_MasterSend(%02x,FU) Port=%u SeqId=%u FuDelay=%d SendTimestamp=%u RcvTimestamp=%u",
								pChUsr->SyncId, PortId, SeqId, FuDelay, Timestamp, GSY_SWAP32(pPduTx->PtcpData.SyncFu.PtcpHdr.ReceiveTS.Dword));

				/* Send FU frame without timestamp
				*/
				gsy_SyncSend(pChUsr->pChSys, pRbs, GSY_SYNC_FU_PDU_SIZE, PortId, GSY_SWAP16(pPdu->PduHdr.FrameId.Word));
			}
		}
	}

	/* Put Sync RQB into Queue again
	*/
	gsy_EnqueLower(&pChUsr->Master.QueueSync, pRbl);

	GSY_MASTER_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--> gsy_MasterQueue(%02x) QueueSy.Count=%02u QueueSy.pFirst=%X Port=%u pRbl=%X",
					pChUsr->SyncId, pChUsr->Master.QueueSync.Count, pChUsr->Master.QueueSync.pFirst, PortId, pRbl);

	if (GSY_MASTER_STATE_STOPPING == pChUsr->Master.State)
	{
		/* Master stopped
		*/
		gsy_MasterStop(pChUsr);
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSyncCnf() pChUsr=%X FuDelay=%d", pChUsr, FuDelay);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterFuCnf()                               */
/* Confirmation for a sent FollowUp frame                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterFuCnf(
GSY_CH_USR_PTR			pChUsr,
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	LSA_UINT16		Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterFuCnf() pChUsr=%X pRbl=%X", pChUsr, pRbl);

	if (GSY_LOWER_RSP_OK != Response)
	{
		GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterFuCnf() pChUsr=%X pRbl=%X: Send response=0x%x",
						pChUsr, pRbl, Response);
	}

	/* Requeue FollowUp RQB
	*/
	gsy_EnqueLower(&pChUsr->Master.QueueFu, pRbl);

	GSY_MASTER_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"--> gsy_MasterQueue(%02x) QueueFu.Count=%02u QueueFu.pFirst=%X Port=%u pRbl=%X",
					pChUsr->SyncId, pChUsr->Master.QueueFu.Count, pChUsr->Master.QueueFu.pFirst, GSY_LOWER_RQB_SEND_SYNC_PORT_GET(pRbl), pRbl);

	if (GSY_MASTER_STATE_STOPPING == pChUsr->Master.State)
	{
		/* Master stopped
		*/
		gsy_MasterStop(pChUsr);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterFuCnf() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterAnnounceCnf()                         */
/* Confirmation for a sent Announce frame                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterAnnoCnf(
GSY_CH_USR_PTR			pChUsr,
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	LSA_UINT16		Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAnnounceCnf() pChUsr=%X pRbl=%X", pChUsr, pRbl);

	if (GSY_LOWER_RSP_OK != Response)
	{
		GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterAnnounceCnf() pChUsr=%X pRbl=%X: Send response=0x%x",
						pChUsr, pRbl, Response);
	}

	/* Requeue Announce RQB
	*/
	gsy_EnqueLower(&pChUsr->Master.QueueAnnounce, pRbl);

	if (GSY_MASTER_STATE_STOPPING == pChUsr->Master.State)
	{
		/* Master stopped
		*/
		gsy_MasterStop(pChUsr);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAnnounceCnf() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterIndAnno()                             */
/* Indication for a received Announce frame                                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterIndAnno(
GSY_CH_USR_PTR			pChUsr,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId)
{
	GSY_BMA_PTR_TYPE	pMasterNew, pMasterRemote = &pTLVInfo->Param.Announce.Master;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterIndAnno() pChUsr=%X SeqId=%u Port=%u",
					pChUsr, pTLVInfo->Param.Announce.SequenceID, PortId);

	GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterIndAnno(%02x) SeqId=%u from: %02x-%02x-%02x-%02x-%02x-%02x",
		pChUsr->SyncId, pTLVInfo->Param.Announce.SequenceID,
		pMasterRemote->MacAddr.MacAdr[0], pMasterRemote->MacAddr.MacAdr[1],
		pMasterRemote->MacAddr.MacAdr[2], pMasterRemote->MacAddr.MacAdr[3],
		pMasterRemote->MacAddr.MacAdr[4], pMasterRemote->MacAddr.MacAdr[5]);

	/* put remote Master into secondary list for best master or update it
	*/
	pMasterNew = gsy_MasterListUpdate(pChUsr, pChUsr->Master.SecList, pMasterRemote, &pChUsr->Master.SecListUsed);

	if ((GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
	&&  (0 != pChUsr->Master.StartupTime) && (!pChUsr->Master.Startup))
	{
		/* 070907lrg001: Start timer if MasterStartup != DISABLE is projected
		*/
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterIndAnno(%02x) SeqId=%d master state change ->Startup",
							pChUsr->SyncId, pTLVInfo->Param.Announce.SequenceID);

		gsy_TimerStart(&pChUsr->Master.StartupTimer, pChUsr->Master.StartupTime);
		pChUsr->Master.Startup = LSA_TRUE;
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterIndAnno() pChUsr=%X pMasterNew=%X", pChUsr, pMasterNew);
	LSA_UNUSED_ARG(PortId);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterIndSync()                             */
/* Indication for a received Sync frame                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterIndSync(
GSY_CH_USR_PTR			pChUsr,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId)
{
	GSY_BMA_PTR_TYPE	pMasterNew, pMasterRemote = &pTLVInfo->Param.Sync.Master;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterIndSync() pChUsr=%X SeqId=%u Port=0x%x",
					pChUsr, pTLVInfo->Param.Sync.SequenceID , PortId);

	GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterIndSync(%02x) SeqId=%u from: %02x-%02x-%02x-%02x-%02x-%02x",
		pChUsr->SyncId, pTLVInfo->Param.Sync.SequenceID,
		pMasterRemote->MacAddr.MacAdr[0], pMasterRemote->MacAddr.MacAdr[1],
		pMasterRemote->MacAddr.MacAdr[2], pMasterRemote->MacAddr.MacAdr[3],
		pMasterRemote->MacAddr.MacAdr[4], pMasterRemote->MacAddr.MacAdr[5]);

	/* put remote Master into primary list for timeout or update it
	*/
	pMasterNew = gsy_MasterListUpdate(pChUsr, pChUsr->Master.PriList, pMasterRemote, &pChUsr->Master.PriListUsed);

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterIndSync() pChUsr=%X pMasterNew=%X", pChUsr, pMasterNew);

	LSA_UNUSED_ARG(PortId);
}

/*****************************************************************************/
/* Internal function: gsy_MasterListPut()                                    */
/* Put new Master into the liste                                             */
/*****************************************************************************/
GSY_BMA_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_MasterListPut(
GSY_CH_USR_PTR			pChUsr,
GSY_BMA_PTR_TYPE		pMasterList,
GSY_BMA_PTR_TYPE   		pMasterNew,
LSA_UINT16	*			pListCnt)
{
	LSA_INT 			Idx;
	GSY_BMA_PTR_TYPE	pMaster = LSA_NULL;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterListPut() pChUsr=%X pMasterList=%X pMasterNew=%X ListCnt=%u",
					pChUsr, pMasterList, pMasterNew, *pListCnt);

	/* If counter says that there is a free slot in the list...
	*/
	if (*pListCnt < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL)
	{
		/* ...search for it
		*/
		pMaster = pMasterList;
		for (Idx = 0; Idx < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; Idx++)
		{
			if (Idx > 0) pMaster++;
			if (GSY_ENTRY_SM_UNUSED == pMaster->Receipt)
			{
				/* Free slot found: take it for the new Master
				*/
				*pMaster = *pMasterNew;
				pMaster->Receipt = GSY_ENTRY_SM_USED;
				(*pListCnt)++;
				GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterListPut() New Master %02x-%02x-%02x-%02x-%02x-%02x Index=%u ListCnt=%u",
						pMaster->MacAddr.MacAdr[0],	pMaster->MacAddr.MacAdr[1], pMaster->MacAddr.MacAdr[2], pMaster->MacAddr.MacAdr[3],
						pMaster->MacAddr.MacAdr[4],	pMaster->MacAddr.MacAdr[5], Idx, *pListCnt);
				break;
			}
			else
			{
				GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterListPut() Master %02x-%02x-%02x-%02x-%02x-%02x Index=%u Receipt=0x%x",
						pMaster->MacAddr.MacAdr[0],	pMaster->MacAddr.MacAdr[1], pMaster->MacAddr.MacAdr[2], pMaster->MacAddr.MacAdr[3],
						pMaster->MacAddr.MacAdr[4],	pMaster->MacAddr.MacAdr[5], Idx, pMaster->Receipt);
			}
		}
		if (Idx >= GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL)
		{
			/* List or counter corrupted?
			*/
			pMaster = LSA_NULL;
			GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterListPut() pChUsr=%X No free Masterslot: ListCnt=%u", pChUsr, *pListCnt);
		}
	}

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterListPut() pChUsr=%X pMasterList=%X pMasterNew=%X ListCnt=%u",
					pChUsr, pMasterList, pMasterNew, *pListCnt);
	LSA_UNUSED_ARG(pChUsr);
	return(pMaster);
}

/*****************************************************************************/
/* Internal function: gsy_MasterListFind()                                   */
/* Search Master in list                                                     */
/*****************************************************************************/
GSY_BMA_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_MasterListFind(
GSY_CH_USR_PTR			pChUsr,
GSY_BMA_PTR_TYPE   		pMasterList,
GSY_BMA_PTR_TYPE		pMasterNew)
{
	LSA_INT				Idx;
	GSY_BMA_PTR_TYPE	pMaster = pMasterList;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterListFind() pChUsr=%X pMasterList=%X pMasterNew=%X",
					pChUsr, pMasterList, pMasterNew);

	for (Idx = 0; Idx < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; Idx++)
	{
		if (Idx > 0) pMaster++;
		if ((GSY_ENTRY_SM_UNUSED != pMaster->Receipt)
		&&  GSY_MACADDR_CMP(pMaster->MacAddr, pMasterNew->MacAddr))
		{
			/* Master found
			*/
			GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterListFind() Master %02x-%02x-%02x-%02x-%02x-%02x pMaster=%X Index=%u",
					pMaster->MacAddr.MacAdr[0],	pMaster->MacAddr.MacAdr[1], pMaster->MacAddr.MacAdr[2], pMaster->MacAddr.MacAdr[3],
					pMaster->MacAddr.MacAdr[4],	pMaster->MacAddr.MacAdr[5], pMaster, Idx);
			break;
		}
	}
	if (Idx >= GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL)
	{
		/* Not found
		*/
		pMaster = LSA_NULL;
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterListFind() pChUsr=%X pMasterList=%X  pMasterRet=%X",
					pChUsr, pMasterList, pMaster);
	LSA_UNUSED_ARG(pChUsr);
	return(pMaster);
}

/*****************************************************************************/
/* Internal function: gsy_MasterListUpdate()                                 */
/* Put new Master into list or update master if already in list              */
/*****************************************************************************/
GSY_BMA_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_MasterListUpdate(
GSY_CH_USR_PTR			pChUsr,
GSY_BMA_PTR_TYPE		pMasterList,
GSY_BMA_PTR_TYPE   		pMasterNew,
LSA_UINT16	*			pListCnt)
{
	GSY_BMA_PTR_TYPE	pMaster = LSA_NULL;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterListUpdate() pChUsr=%X pMasterList=%X pMasterNew=%X ListCnt=%u",
					pChUsr, pMasterList, pMasterNew, *pListCnt);

	/* If any Master is in the list: search for the new one
	*/
	if (*pListCnt > 0)
		pMaster =  gsy_MasterListFind(pChUsr, pMasterList, pMasterNew);

	/* Master not in list: add new Master to list
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pMaster, LSA_NULL))
		pMaster = gsy_MasterListPut(pChUsr, pMasterList, pMasterNew, pListCnt);
	else
	{
		/* Update Master
		*/
		pMaster->Class = pMasterNew->Class;
		pMaster->Priority1 = pMasterNew->Priority1;
		pMaster->Priority2 = pMasterNew->Priority2;
		pMaster->Accuracy = pMasterNew->Accuracy;
		pMaster->Variance = pMasterNew->Variance;
		pMaster->Receipt++;
		GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterListUpdate() Master %02x-%02x-%02x-%02x-%02x-%02x pMaster=%X Receipt=%u",
				pMaster->MacAddr.MacAdr[0],	pMaster->MacAddr.MacAdr[1], pMaster->MacAddr.MacAdr[2], pMaster->MacAddr.MacAdr[3],
				pMaster->MacAddr.MacAdr[4],	pMaster->MacAddr.MacAdr[5], pMaster, pMaster->Receipt);
	}

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterListUpdate() pChUsr=%X pMasterList=%X pMasterRet=%X ListCnt=%u",
					pChUsr, pMasterList, pMaster, *pListCnt);
	LSA_UNUSED_ARG(pChUsr);
	return(pMaster);
}

/*****************************************************************************/
/* Internal function: gsy_MasterBestCheck()                                  */
/* Pointer to the best of two Masters will be returned                       */
/*****************************************************************************/
GSY_BMA_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_MasterBestCheck(
GSY_CH_USR_PTR		pChUsr,
GSY_BMA_PTR_TYPE	pMaster1,
GSY_BMA_PTR_TYPE	pMaster2)
{
//	LSA_UINT16			MacSum1 = 0, MacSum2 = 0;
	LSA_INT				Idx = 0;
	GSY_BMA_PTR_TYPE	pMaster = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterBestCheck() pChUsr=%X pMaster1=%X pMaster2=%X",
					pChUsr, pMaster1, pMaster2);

	/* If one of both is NULL, the other is best
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pMaster1, LSA_NULL))
		pMaster = pMaster2;
	else if (LSA_HOST_PTR_ARE_EQUAL(pMaster2, LSA_NULL))
		pMaster = pMaster1;
	else
	{
		/* The lower Priority1 is better
		 * 070907lrg001: GSY_SYNC_PRIO1_ACTIVE
		*/
		if ((pMaster1->Priority1 & GSY_SYNC_PRIO1_ACTIVE)
		||  (pMaster2->Priority1 & GSY_SYNC_PRIO1_ACTIVE))
		{
			/* 290508lrg001: the smaller level is even better than GSY_SYNC_PRIO1_ACTIVE
			 * becuse SyncFrames from a greater level cannot be received if the boundaries are set well
			if ((pMaster1->Priority1 & GSY_SYNC_PRIO1_ACTIVE)
			||  (GSY_SYNC_PRIO1_LEVEL(pMaster1->Priority1) < GSY_SYNC_PRIO1_LEVEL(pMaster2->Priority1)))
			*/
			if (pMaster1->Priority1 & GSY_SYNC_PRIO1_ACTIVE)
				pMaster = pMaster1;
			else
				pMaster = pMaster2;
			if ((pMaster1->Priority1 & GSY_SYNC_PRIO1_ACTIVE)
			&&  (pMaster2->Priority1 & GSY_SYNC_PRIO1_ACTIVE))
			{
				/* 070907lrg001: On 2 active Masters the local Master is better
				*/
				GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"  * gsy_MasterBestCheck() GSY_SYNC_PRIO1_ACTIVE from master %02x-%02x-%02x-%02x-%02x-%02x Level=%u Prio=%u]",
									pMaster2->MacAddr.MacAdr[0], pMaster2->MacAddr.MacAdr[1], pMaster2->MacAddr.MacAdr[2], pMaster2->MacAddr.MacAdr[3],
									pMaster2->MacAddr.MacAdr[4], pMaster2->MacAddr.MacAdr[5], GSY_SYNC_PRIO1_LEVEL(pMaster2->Priority1),
									GSY_SYNC_PRIO1_PRIO(pMaster2->Priority1));
			}
		}
		else if (pMaster1->Priority1 < pMaster2->Priority1)
				pMaster = pMaster1;
		else if (pMaster2->Priority1 < pMaster1->Priority1)
			pMaster = pMaster2;
		else if (GSY_SYNC_PRIO1_PRIO(pMaster1->Priority1))
		{
			/* 170408lrg003: On 2 primary Masters with the same level the local Master is better
			*/
			pMaster = pMaster1;
		}
		else
		{
			/* The lower Priority2 is better on same Priority1.
			*/
			if (pMaster1->Priority2 < pMaster2->Priority2)
				pMaster = pMaster1;
			else if (pMaster2->Priority2 < pMaster1->Priority2)
				pMaster = pMaster2;
			else
			{
				/* On same Priority2 the smaller Variance is better
				*/
				if (pMaster1->Variance < pMaster2->Variance)
					pMaster = pMaster1;
				else if (pMaster2->Variance < pMaster1->Variance)
					pMaster = pMaster2;
				else
				{
					/* 041010lrg001: On same Variance the smaller Accuracy is better
					*/
					if (pMaster1->Accuracy < pMaster2->Accuracy)
						pMaster = pMaster1;
					else if (pMaster2->Accuracy < pMaster1->Accuracy)
						pMaster = pMaster2;
					else
					{
#ifdef GSY_NO_MASTER_MAC_CHECK		//260407lrg001; pMaster1 (local) is better if all other values are identical
						Idx = -1;
#else
						/* All values are identical:
						 * the owner of the "smaller" MAC Address or pMaster1 (local) is better
						*/
						for (Idx = 0; Idx < GSY_MAC_ADR_LENGTH; Idx++)
						{
							/* The first different byte is used	for decision
							*/
							if (pMaster1->MacAddr.MacAdr[Idx] != pMaster2->MacAddr.MacAdr[Idx])
								break;
						}
						if ((Idx < GSY_MAC_ADR_LENGTH)
						&&  (pMaster2->MacAddr.MacAdr[Idx] < pMaster1->MacAddr.MacAdr[Idx]))
						{
							pMaster = pMaster2;
						}
						else
#endif
						{
							pMaster = pMaster1;
						}
					}
				}
			}
		}
		GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterBestCheck() Priority1_1/2=0x%x/0x%x Priority2_1/2=0x%x/0x%x Variance_1/2=%u/%u Idx=%d Best=%X",
					pMaster1->Priority1, pMaster2->Priority1, pMaster1->Priority2, pMaster2->Priority2,
					pMaster1->Variance, pMaster2->Variance, Idx, pMaster);
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterBestCheck() pChUsr=%X pMasterRet=%X", pChUsr, pMaster);
	LSA_UNUSED_ARG(pChUsr);
	return(pMaster);
}

/*****************************************************************************/
/* Internal function: gsy_MasterBestGet()                                    */
/* Return pointer to the best Master of a list                               */
/*****************************************************************************/
GSY_BMA_PTR_TYPE GSY_LOCAL_FCT_ATTR gsy_MasterBestGet(
GSY_CH_USR_PTR		pChUsr,
GSY_BMA_PTR_TYPE	pMasterList)
{
	LSA_INT 			Idx;
	GSY_BMA_PTR_TYPE	pMaster = pMasterList;
	GSY_BMA_PTR_TYPE	pMasterRet = LSA_NULL;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterBestGet() pChUsr=%X pMasterList=%X",
					pChUsr, pMasterList);

	for (Idx = 0; Idx < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; Idx++)
	{
		if (Idx > 0) pMaster++;
		if (GSY_ENTRY_SM_UNUSED != pMaster->Receipt)
		{
			/* If the current entry is valid, the better Master will be chosen
			*/
			pMasterRet = gsy_MasterBestCheck(pChUsr, pMasterRet, pMaster);
		}
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterBestGet() pChUsr=%X pMasterRet=%X",
					pChUsr, pMasterRet);
	LSA_UNUSED_ARG(pChUsr);
	return(pMasterRet);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterAging()                               */
/* Indication from slave takeover = master lost                              */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterAging(
GSY_CH_USR_PTR			pChUsr)
{
	GSY_BMA_PTR_TYPE	pMaster;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAging() pChUsr=%X PriUsed=%u SecUsed=%u]",
					pChUsr, pChUsr->Master.PriListUsed, pChUsr->Master.SecListUsed);

	/* 301110lrg001: after slave takeover the local master will become new primary Master
	pChUsr->Master.pPriMaster = &pChUsr->Master.LocalMaster;
	pChUsr->Master.pSecMaster = LSA_NULL;
	*/

	/* 060411lrg002: look into announce list for best master
	 * Choose new best master from list
	*/
	pMaster = LSA_NULL;
	if (pChUsr->Master.SecListUsed != 0)
		pMaster = gsy_MasterBestGet(pChUsr, pChUsr->Master.SecList);

	/* If the list is empty...
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pMaster, LSA_NULL))
	{
		/* ... the local master will become new best Master
		*/
		pChUsr->Master.pSecMaster = &pChUsr->Master.LocalMaster;
	}
	else
	{
		/* ... else the best remote Master or the local Master will become new best Master
		*/
		pChUsr->Master.pSecMaster = gsy_MasterBestCheck(pChUsr, &pChUsr->Master.LocalMaster, pMaster);	//260407lrg001
	}

	if (LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, &pChUsr->Master.LocalMaster))
	{
		/* 130411lrg: AP01162398_BMA: local is best master
		*/
		pChUsr->Master.pPriMaster = &pChUsr->Master.LocalMaster;
	}
	else if (LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, pChUsr->Master.pPriMaster))
	{
		/* 1430411lrg: AP01162398_BMA: announce from last primary master is present
		*/
		pChUsr->Master.pPriMaster = LSA_NULL;
	}

	GSY_MASTER_TRACE_09(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterAging(%02x) BestMaster=%X (%02x-%02x-%02x-%02x-%02x-%02x) ListCnt=%u",
			pChUsr->SyncId, pChUsr->Master.pSecMaster,
			pChUsr->Master.pSecMaster->MacAddr.MacAdr[0],pChUsr->Master.pSecMaster->MacAddr.MacAdr[1],
			pChUsr->Master.pSecMaster->MacAddr.MacAdr[2],pChUsr->Master.pSecMaster->MacAddr.MacAdr[3],
			pChUsr->Master.pSecMaster->MacAddr.MacAdr[4],pChUsr->Master.pSecMaster->MacAddr.MacAdr[5],pChUsr->Master.SecListUsed);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pPriMaster, LSA_NULL))
	{
		GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterAging(%02x)  PriMaster=%X (%02x-%02x-%02x-%02x-%02x-%02x)",
			pChUsr->SyncId, pChUsr->Master.pPriMaster,
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[0],pChUsr->Master.pPriMaster->MacAddr.MacAdr[1],
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[2],pChUsr->Master.pPriMaster->MacAddr.MacAdr[3],
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[4],pChUsr->Master.pPriMaster->MacAddr.MacAdr[5]);
	}

	#ifdef GSY_MASTER_TIMER_CHECK
			pChUsr->Master.OldSeqId = 1;
			pChUsr->Master.OldTimestamp = 0;
#endif
/* Update Master state
	*/
	gsy_MasterStateSet(pChUsr);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAging() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal function: gsy_MasterAgingAnno()                                  */
/* Aging of the Announce Master list and choosing of the local master state  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterAgingAnno(
GSY_CH_USR_PTR			pChUsr)
{
	LSA_INT 			Idx;
	GSY_BMA_PTR_TYPE	pMaster;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterAgingAnno() pChUsr=%X PriUsed=%u SecUsed=%u]",
					pChUsr, pChUsr->Master.PriListUsed, pChUsr->Master.SecListUsed);

	if (pChUsr->Master.SecListUsed != 0)
	{
		for (Idx = 0; Idx < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL; Idx++)
		{
			pMaster = &pChUsr->Master.SecList[Idx];
			if (GSY_ENTRY_SM_OLD == pMaster->Receipt)
			{
				/* Remove remote Master from list
				*/
				pMaster->Receipt = GSY_ENTRY_SM_UNUSED;
				pChUsr->Master.SecListUsed--;
				if (LSA_HOST_PTR_ARE_EQUAL(pMaster, pChUsr->Master.pSecMaster))
				{
					/* Actual best Master has gone
					*/
					pChUsr->Master.pSecMaster = LSA_NULL;
				}
				GSY_MASTER_TRACE_09(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterAgingAnno(%02x) removed Master %02x-%02x-%02x-%02x-%02x-%02x Index=%u ListCnt=%u",
						pChUsr->SyncId, pMaster->MacAddr.MacAdr[0],	pMaster->MacAddr.MacAdr[1], pMaster->MacAddr.MacAdr[2], pMaster->MacAddr.MacAdr[3],
						pMaster->MacAddr.MacAdr[4],	pMaster->MacAddr.MacAdr[5], Idx, pChUsr->Master.SecListUsed);
			}
			else if (GSY_ENTRY_SM_USED <= pMaster->Receipt)
			{
				/* Age Remote Master
				*/
				pMaster->Receipt = GSY_ENTRY_SM_OLD;
			}
		}
	}

	/* Choose new best master from list
	*/
	pMaster = LSA_NULL;
	if (pChUsr->Master.SecListUsed != 0)
		pMaster = gsy_MasterBestGet(pChUsr, pChUsr->Master.SecList);

	/* If the list is empty...
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pMaster, LSA_NULL))
	{
		/* ... the local master will become new secondary Master
		*/
		pChUsr->Master.pSecMaster = &pChUsr->Master.LocalMaster;

		if ((GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
		&&  GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))
		{
			/* 140411lrg: AP01162398_BMA: for return from slave state after takeover
			*/
			pChUsr->Master.pPriMaster = &pChUsr->Master.LocalMaster;
		}
	}
	else
	{
		/* ... else the best remote Master or the local Master will become new secondary Master
		*/
		pChUsr->Master.pSecMaster = gsy_MasterBestCheck(pChUsr, &pChUsr->Master.LocalMaster, pMaster);	//260407lrg001
	}

	GSY_MASTER_TRACE_09(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterAgingAnno(%02x) BestMaster=%X (%02x-%02x-%02x-%02x-%02x-%02x) ListCnt=%u",
		pChUsr->SyncId,pChUsr->Master.pSecMaster,
		pChUsr->Master.pSecMaster->MacAddr.MacAdr[0],pChUsr->Master.pSecMaster->MacAddr.MacAdr[1],
		pChUsr->Master.pSecMaster->MacAddr.MacAdr[2],pChUsr->Master.pSecMaster->MacAddr.MacAdr[3],
		pChUsr->Master.pSecMaster->MacAddr.MacAdr[4],pChUsr->Master.pSecMaster->MacAddr.MacAdr[5],pChUsr->Master.SecListUsed);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pPriMaster, LSA_NULL))
	{
		GSY_MASTER_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_MasterAgingAnno(%02x)  PriMaster=%X (%02x-%02x-%02x-%02x-%02x-%02x)",
			pChUsr->SyncId, pChUsr->Master.pPriMaster,
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[0],pChUsr->Master.pPriMaster->MacAddr.MacAdr[1],
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[2],pChUsr->Master.pPriMaster->MacAddr.MacAdr[3],
			pChUsr->Master.pPriMaster->MacAddr.MacAdr[4],pChUsr->Master.pPriMaster->MacAddr.MacAdr[5]);
	}
	/* Update Master state
	*/
	gsy_MasterStateSet(pChUsr);

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterAgingAnno() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal function: gsy_MasterStateSet()                                   */
/* The actual Master state will be chosen - BMA state F_CMP                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterStateSet(
GSY_CH_USR_PTR			pChUsr)
{
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	GSY_DRIFT_PTR		pDrift = LSA_NULL;
	LSA_UINT8	OldState = pChUsr->Master.State;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterStateSet() pChUsr=%X SyncId=0x%02x OldState=%u",
					pChUsr, pChUsr->SyncId, pChUsr->Master.State);

	switch (OldState)
	{
	default:
		break;

	case GSY_MASTER_STATE_QUICKUP:
	case GSY_MASTER_STATE_STARTUP:
	case GSY_MASTER_STATE_PRIMARY:

		/* Decision from Announce Message Master List
		*/
		if (LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, &pChUsr->Master.LocalMaster))
		{
			/* Become primary Master or remain primary
			*/
			if ((GSY_MASTER_STATE_PRIMARY != OldState)
			&&  ((pChUsr->SyncPLLActive && (GSY_PLL_STATE_SYNC_OK == pChUsr->PLLData.State))
			 ||  (!pChUsr->SyncPLLActive && ((0 == pChUsr->Master.PriListUsed) || (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)))))
//			 ||  !pChUsr->SyncPLLActive && ((0 == pChUsr->Master.SecListUsed) || (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)))	290508lrg001
			{
				/* 111007lrg002: Change to primary not until Slave is sync - or no other master is present
				 * 290508lrg001: Hierarchie: from the other master also SyncFrames have to be received
				*/
				GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStateSet(%02x) Startup=%u Prio1=0x%02x master state change [%u]->PRIMARY",
									pChUsr->SyncId, pChUsr->Master.Startup, pChUsr->Master.LocalMaster.Priority1, OldState);

				pChUsr->Master.pPriMaster = &pChUsr->Master.LocalMaster;	//140411lrg: AP01162398_BMA: local master is best
				pChUsr->Master.State = GSY_MASTER_STATE_PRIMARY;
				pChUsr->Event.PriMasterWaits = LSA_FALSE;


				if (pChUsr->Master.HwMaster)
				{
					gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_STOP, GSY_LOWER_MASTER_START);
				}
				else
				{
					/* Already done in gsy_MasterPLLStart()
					*/
					if (!pChUsr->SyncPLLActive)
					{
						/* Allocate drift strukture with own Master address
						*/
						pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
						pDrift->MasterAddr = pChUsr->Master.LocalMaster.MacAddr;
						pDrift->Subdomain = pChUsr->SyncData.SubdomainID;

#ifdef GSY_RATE_DECRAY
						if ((GSY_DRIFT_STATE_READY != pDrift->State)
						||  (GSY_HW_TYPE_ERTEC_400 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)))
						{
							/* Set Drift for Master-SyncID to 0
							*/
							gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_MASTER);
						}
						else
						{
							/* Decray correction of the master clock by adjusting it with increasing RCF-Interval
							*/
							gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, pDrift->AveInterval, GSY_DRIFT_STATE_TRANSIENT);
						}
#else
						/* Set Drift for Master-SyncID to 0
						*/
						gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_MASTER);
#endif
					}
					/* 041006lrg002: Indication to User
					*/
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK;
					gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
					gsy_SyncInd(pChUsr, LSA_TRUE);
				}
			}
		}
		else
		{
			/* Change to SLAVE State and finish the Startup Interval
			*/
			pChUsr->Event.PriMasterWaits = LSA_FALSE;
			pChUsr->Master.State = GSY_MASTER_STATE_SLAVE;
			pChUsr->Master.Startup = LSA_FALSE;
			gsy_TimerStop(&pChUsr->Master.StartupTimer);

//			if (pChUsr->Master.HwMaster)
			{
				/* Store Remote best Master:
				 * will be set  to LSA_NULL at Takeover by MasterAging()
				 * 301110lrg001: always indication from slave
				*/
				pChUsr->Master.pPriMaster = pChUsr->Master.pSecMaster;
			}
			if (pChUsr->SyncPLLActive)
			{
				/* Let PLL sync sleep
				*/
				GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStateSet(%02x) Startup=%u Prio1=0x%02x PLL state change [%u]->SLEEP",
									pChUsr->SyncId, pChUsr->Master.Startup, pChUsr->Master.LocalMaster.Priority1, pChUsr->PLLData.State);
				pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_SLEEP;
			}

			if (GSY_MASTER_STATE_PRIMARY == OldState)
			{
				/* Slave starts in GSY_SLAVE_STATE_FIRST
				 * 151007lrg001: Set MasterMAC for TopoOk auf 0 setzen
				 * 180108lrg001
				*/
				GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStateSet(%02x) master state change PRIMARY->SLAVE",
									pChUsr->SyncId);
				pChUsr->Slave.SumTimeout = 0;
				pChUsr->Slave.MasterAddr = NoMacAddr;
				pChUsr->Slave.State = GSY_SLAVE_STATE_FIRST;

				if (pChUsr->Master.HwMaster)
				{
					gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_START, GSY_LOWER_MASTER_SLEEP);
				}
				else
				{
					pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
					pDrift->OldMasterAddr = pDrift->MasterAddr;
					pDrift->MasterAddr = NoMacAddr;

					/* 120608lrg001: Prevent gsy_DriftReset() by gsy_DriftAdjust() at the next RCF interrupt
					*/
					pDrift->Act.SeqId = pDrift->OldSeqId + 1;

					/* 171007lrg001: Resetting the Drift lets the Slave run into Timeout
					 * --> try to change smoothly
					gsy_DriftReset(pChUsr->pChSys, pDrift, pChUsr->SyncId);
					gsy_SyncInd(pChUsr, LSA_FALSE);
					*/
					gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_READY);
					gsy_SyncInd(pChUsr, LSA_TRUE);

					/* 160511lrg: start slave timer
					*/
					gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
				}
			}
			else
			{
				GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStateSet(%02x) Startup=%u Prio1=0x%02x master state change [%u]->SLAVE",
									pChUsr->SyncId, pChUsr->Master.Startup, pChUsr->Master.LocalMaster.Priority1, OldState);

				if (pChUsr->SyncPLLActive)
				{
					/* Free master drift structure
					*/
					pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
					pDrift->MasterAddr = NoMacAddr;
					gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_UNKNOWN);
//					gsy_DriftReset(pChUsr->pChSys, pDrift, pChUsr->SyncId);
				}
			}
		}
		break;

	case GSY_MASTER_STATE_SLAVE:

		/* Decision for Master Timeout from Sync Message Master List
		 * 111007lrg: and for the BestMaster from Anno Message List
		*/
		if (LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pPriMaster, &pChUsr->Master.LocalMaster)
		&& 	(LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, &pChUsr->Master.LocalMaster)
		 ||  LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, LSA_NULL)))
		{
			GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_MasterStateSet(%02x) master state change SLAVE->PRIMARY",
								pChUsr->SyncId);

			/* It takes too long if we change now to STARTUP State:
			 * go immediately to PRIMARY state but in Startup mode
			pChUsr->Master.State = GSY_MASTER_STATE_STARTUP;
			*/
			pChUsr->Master.State = GSY_MASTER_STATE_PRIMARY;
			if (0 != pChUsr->Master.StartupTime)
			{
				/* 070907lrg001: Start timer if MasterStartup != DISABLE is projected
				*/
				gsy_TimerStart(&pChUsr->Master.StartupTimer, pChUsr->Master.StartupTime);
				pChUsr->Master.Startup = LSA_TRUE;
			}
			else
				pChUsr->Master.Startup = LSA_FALSE;

			if (pChUsr->Master.HwMaster)
			{
				gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_STOP, GSY_LOWER_MASTER_START);
			}
			else
			{
				/* Allocate drift strukture with own Master address
				*/
				pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
				pDrift->MasterAddr = pChUsr->Master.LocalMaster.MacAddr;
				pDrift->Subdomain = pChUsr->SyncData.SubdomainID;

#ifdef GSY_RATE_DECRAY
				if ((GSY_DRIFT_STATE_READY != pDrift->State)
				||  (GSY_HW_TYPE_ERTEC_400 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)))
				{
					/* Set Drift for Master-SyncID to 0
					*/
					gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_MASTER);
				}
				else
				{
					/* Decray correction of the master clock by adjusting it with increasing RCF-Interval
					*/
					gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, pDrift->AveInterval, GSY_DRIFT_STATE_TRANSIENT);
				}
#else
				/* Set Drift for Master-SyncID to 0
				*/
				gsy_DriftSet(pChUsr->pChSys, pDrift, pChUsr->SyncId, 0, GSY_DRIFT_STATE_MASTER);
#endif
			}

			pChUsr->Master.SendCount = 0;
			gsy_MasterSendReal(pChUsr);

			if (!pChUsr->Master.HwMaster)
			{
				/* 101006lrg001: Indication to User
				*/
				pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK;
				gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
				gsy_SyncInd(pChUsr, LSA_TRUE);
			}
		}
#if 0	//1404lrg001 AP01162398_BMA: is set to LSA_NULL at TAKEOVER
		else if (/*(pChUsr->Master.HwMaster) 301110lrg001
			 &&  */!LSA_HOST_PTR_ARE_EQUAL(pChUsr->Master.pSecMaster, &pChUsr->Master.LocalMaster))
		{
			/* Store Remote primary Master:
			 * will be set  to LocalMaster at Takeover by MasterAging()
			 * 301110lrg001: always indication from slave
			*/
			pChUsr->Master.pPriMaster = pChUsr->Master.pSecMaster;
		}
#endif
		break;
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterStateSet() pChUsr=%X SyncId=0x%02x NewState=%u",
					pChUsr, pChUsr->SyncId, pChUsr->Master.State);
}

/*****************************************************************************/
/* Internal access function: gsy_MasterSlaveControl()                        */
/* Controls Master and Slave in Hardware                                     */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_MasterSlaveControl(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT8				SlaveMode,
LSA_UINT8				MasterMode)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	GSY_SYNC_PDU			SyncPdu;
	GSY_CH_SYS_PTR 			pChSys = pChUsr->pChSys;

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_MasterSlaveControl() pChUsr=%X SyncId=0x%02x SlaveMode=%u MasterMode=%u",
					pChUsr, pChUsr->SyncId, SlaveMode, MasterMode);

	/* 200809lrg001: Control Master and Slave in hardware by sending a RQB to EDDP
	*/
	pRbl = pChSys->QueueLowerCtrl.pFirst;
	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_MasterSlaveControl() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_SYNC_CTRL=%u GSY_NUM_RQB_DELAY_CTRL=%u",
						GSY_NUM_RQB_SYNC_CTRL, GSY_NUM_RQB_DELAY_CTRL);
		gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
	}
	else
	{
		gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SYNC_CTRL);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		GSY_LOWER_RQB_K32_SYNC_ID_SET(pRbl, pChUsr->SyncId);
		GSY_LOWER_RQB_K32_SYNC_SLAVE_MODE_SET(pRbl, SlaveMode);
		GSY_LOWER_RQB_K32_SYNC_MASTER_MODE_SET(pRbl, MasterMode);
		GSY_LOWER_RQB_K32_SYNC_TIMEOUT_FACTOR_SET(pRbl, (LSA_UINT16)pChUsr->Slave.SyncTimeout);
		GSY_LOWER_RQB_K32_SYNC_TAKEOVER_FACTOR_SET(pRbl, (LSA_UINT16)pChUsr->Slave.SyncTakeover);
		GSY_LOWER_RQB_K32_SYNC_PLL_WINDOW_SET(pRbl, pChUsr->Slave.WindowSize);

		if (GSY_LOWER_SLAVE_UPDATE == SlaveMode)
		{
			if (GSY_LOWER_MASTER_OFF != MasterMode)
			{
				/* Update KRISC32 master
				*/
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterSlaveControl(%02x) pChUsr=%X pChSys=%X updating HW master: MasterMode=%u",
								pChUsr->SyncId, pChUsr, pChSys, MasterMode);

				GSY_LOWER_RQB_K32_SYNC_SEND_FACTOR_SET(pRbl, pChUsr->Master.SendInterval);
				if (GSY_LOWER_MASTER_UPDATE == MasterMode)
				{
					SyncPdu.TLV.Master.TypeLen.Word = GSY_SWAP16(GSY_TLV_MASTER_TYPELEN);
					SyncPdu.TLV.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
					SyncPdu.TLV.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;
					SyncPdu.TLV.Master.ClockClass = pChUsr->Master.LocalMaster.Class;
					SyncPdu.TLV.Master.ClockAccuracy = pChUsr->Master.LocalMaster.Accuracy;
					SyncPdu.TLV.Master.ClockVariance.Word = GSY_SWAP16(pChUsr->Master.LocalMaster.Variance);

					GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, GSY_TLV_MASTER_SIZE);
					GSY_LOWER_RQB_K32_SYNC_TLV_SET(pRbl, &SyncPdu.TLV.Master, GSY_TLV_MASTER_SIZE);
				}
				else
				{
					GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, 0);
				}
			}
			else
			{
				/* Update KRISC32 slave
				*/
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterSlaveControl(%02x) pChUsr=%X pChSys=%X updating HW slave: MasterMode=%u",
								pChUsr->SyncId, pChUsr, pChSys, MasterMode);

				GSY_LOWER_RQB_K32_SYNC_SEND_FACTOR_SET(pRbl, pChUsr->Slave.SyncInterval);
				GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, 0);
			}
		}
		else if (GSY_LOWER_SLAVE_START == SlaveMode)
		{
			/* Start KRISC32 slave
			*/
			GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterSlaveControl(%02x) pChUsr=%X pChSys=%X starting HW slave: MasterMode=%u",
							pChUsr->SyncId, pChUsr, pChSys, MasterMode);

			GSY_LOWER_RQB_K32_SYNC_SEND_FACTOR_SET(pRbl, pChUsr->Slave.SyncInterval);
			GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, GSY_TLV_SUBDOMAIN_SIZE);

			SyncPdu.TLV.Subdomain.SubdomainUUID = pChUsr->SyncData.SubdomainID;
			SyncPdu.TLV.Subdomain.MasterSourceAddress = pChSys->MACAddr;
			SyncPdu.TLV.Subdomain.TypeLen.Word = GSY_SWAP16(GSY_TLV_SUBDOMAIN_TYPELEN);
			GSY_LOWER_RQB_K32_SYNC_TLV_SET(pRbl, &SyncPdu.TLV.Subdomain, GSY_TLV_SUBDOMAIN_SIZE);
		}
		else if (GSY_LOWER_MASTER_START == MasterMode)
		{
			/* Stop KRISC32 slave and start master
			*/
			GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterSlaveControl(%02x) pChUsr=%X pChSys=%X starting HW master: SlaveMode=%u",
							pChUsr->SyncId, pChUsr, pChSys, SlaveMode);

			GSY_LOWER_RQB_K32_SYNC_SEND_FACTOR_SET(pRbl, pChUsr->Master.SendInterval);
			GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, GSY_LOWER_MASTER_TLV_LEN);

			SyncPdu.TLV.Subdomain.TypeLen.Word = GSY_SWAP16(GSY_TLV_SUBDOMAIN_TYPELEN);
			SyncPdu.TLV.Subdomain.SubdomainUUID = pChUsr->SyncData.SubdomainID;
			SyncPdu.TLV.Subdomain.MasterSourceAddress = pChUsr->pChSys->MACAddr;

			SyncPdu.TLV.Time.TypeLen.Word = GSY_SWAP16(GSY_TLV_TIME_TYPELEN);
			SyncPdu.TLV.Time.EpochNumber.Word = GSY_SWAP16(pChUsr->Master.EpochNumber);

			SyncPdu.TLV.TimeExt.TypeLen.Word = GSY_SWAP16(GSY_TLV_TIMEEXT_TYPELEN);
			SyncPdu.TLV.TimeExt.Flags.Word = GSY_SWAP16(0);
			SyncPdu.TLV.TimeExt.CurrentUTCOffset.Word = GSY_SWAP16(pChUsr->Master.CurrentUTCOffset);
			SyncPdu.TLV.TimeExt.Reserved1 = 0;
			SyncPdu.TLV.TimeExt.Reserved2 = 0;

			SyncPdu.TLV.Master.TypeLen.Word = GSY_SWAP16(GSY_TLV_MASTER_TYPELEN);
			SyncPdu.TLV.Master.MasterPriority1 = pChUsr->Master.LocalMaster.Priority1;
			SyncPdu.TLV.Master.MasterPriority2 = pChUsr->Master.LocalMaster.Priority2;
			SyncPdu.TLV.Master.ClockClass = pChUsr->Master.LocalMaster.Class;
			SyncPdu.TLV.Master.ClockAccuracy = pChUsr->Master.LocalMaster.Accuracy;
			SyncPdu.TLV.Master.ClockVariance.Word = GSY_SWAP16(pChUsr->Master.LocalMaster.Variance);

			GSY_LOWER_RQB_K32_SYNC_TLV_SET(pRbl, &SyncPdu.TLV, GSY_LOWER_MASTER_TLV_LEN);

        }
		else
		{
			/* Stop KRISC32 slave and master or activate master
			*/
			GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_MasterSlaveControl(%02x) pChUsr=%X pChSys=%X MasterMode=%u SlaveMode=%u",
							pChUsr->SyncId, pChUsr, pChSys, MasterMode, SlaveMode);

			GSY_LOWER_RQB_K32_SYNC_LENGTH_TLV_SET(pRbl, 0);
		}
		/* Send lower request
		*/
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_MasterSlaveControl() pChUsr=%X pChSys=%X", pChUsr, pChSys);
}

/*****************************************************************************/
/*  end of file GSY_BMA.C                                                    */
/*****************************************************************************/

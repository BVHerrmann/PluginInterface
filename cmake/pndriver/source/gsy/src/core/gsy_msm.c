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
/*  F i l e               &F: gsy_msm.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Slave functions and master slave manager                                 */
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
/*              250906lrg001: Driftkompensation auch bei GSY_REF_CYCLE_BEGIN */ 
/*              HWabhaengige Delayaddition: gsy_SlaveAction->gsy_DelayGet()  */
/*  2006-10-23  P03.12.00.00_00.30.03.02 lrg GSY_PUT_INFO_1                  */
/*              231006lrg002: PriMasterWaits, 231006lrg003                   */
/*              261006lrg002: Regler Reset bei Slave Start                   */
/*  2007-01-10  P03.12.00.00_00.40.06.02 100107lrg001: Offset f. DiagRecord  */
/*  2007-02-20  P03.12.00.00_00.40.10.02 200207lrg001: SyncTrace erweitert   */
/*  2007-08-06  P04.00.00.00_00.30.03.02 210807lrg001: SyncTimeout = Zaehler */
/*  2007-09-27  P04.00.00.00_00.30.06.02 270907lrg005: MasterMAC auf NULL    */
/*              setzen und Takeover durchfuehren                             */ 
/*  2007-11-16  P04.00.00.00_00.50.01.02 131107lrg001: doch state check      */
/*  2008-01-07  P04.00.00.00_00.60.02.02 070108lrg001: Arts-RQ AP00561490    */
/*              Ruecksetzen des PhaseCorrectionRegister bei SlaveStop        */
/*              070108lrg002: Erweiterter (Error)Trace mit SeqId             */
/*              211207lrg001: NichtSync durch Timeout auch nach Wechsel des  */
/*              Master in den SlaveMode (GSY_SLAVE_STATE_FIRST) melden       */
/*  2008-02-14  P04.00.00.00_00.70.02.02 140208lrg001: Traceerweiterung      */
/*  2008-02-14  P04.00.00.00_00.70.02.03 180208lrg001: CycleTimeStamp setzen */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-06-20  P04.01.00.00_00.01.02.02 020708lrg001: Beim Uebergang vom    */
/*              aktiven zum passiven Master keinen SlaveSet erzwingen        */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-01-09  P04.01.00.00_00.02.03.02 lrg: KRISC32 Support                */
/*  2009-01-28  P04.01.00.00_00.02.03.03 280109lrg001: LSA_UINT16 Cast       */
/*  2009-04-08  P04.01.00.00_00.03.03.02 lrg: K32_RQB_TYPE_SLAVE_REQ         */
/*  2009-09-30  P04.02.00.00_00.01.04.02 lrg: GSY_LOWER_MASTER_START         */
/*  2009-10-08  P04.02.00.00_00.01.04.04 lrg: GSY_HW_TYPE_SLAVE_MSK...       */
/*  2009-11-17  P05.00.00.00_00.01.01.02 171109lrg001: check for PDU from    */
/*              myself in gsy_TLVGetInfo() not longer in gsy_SlaveIndSync()  */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-06-02  P05.00.00.00_00.01.05.02 110510lrg001: AP00969698:           */
/*              "Verbindungsabruch nach Sync-Master-Wechsel":                */
/*              Timeoutzaehler beim ersten SyncFrame des neuen Masters       */
/*              zurücksetzen, nicht erst beim FollowUpFrame. Dazwischen kann */
/*              sonst Takeover sein.                                         */
/*              German comments translated to english                        */
/*  2010-10-08  P05.00.00.00_00.01.08.03 lrg: Subdomain check and call of    */
/*              gsy_MasterIndSync() moved from gsy_SlaveIndSync() to         */
/*              gsy_FwdFrameUserForward(). 071010lrg001: WARN -> NOTE (MRP)  */
/*  2010-11-30  P05.00.00.00_00.02.01.02 301110lrg001:                       */
/*              Indicate slave Takeover and Timeout to master                */
/*  2010-12-15  P05.00.00.00_00.02.01.03 151210lrg001:                       */
/*              Do not check offset for GSY_SYNC_SET if PLLWindow is 0.      */
/*              Do not finish closing channel if PLL sync is running.        */
/*              Do not wait for pSyncSetRbl if PLL sync is running.          */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-04-14  P05.00.00.00_00.04.24.02 060411lrg001: AP01162398_BMA        */
/*  2011-05-16  P05.00.00.00_00.05.21.02 160511lrg: AP01183082 gsy_SlaveTimer*/
/*  2011-05-27  P05.00.00.00_00.05.21.02 270511lrg: Trace (%d)               */
/*  2011-06-21  P05.00.00.00_00.05.22.02 080611lrg: AP01185610 SNYC-OK       */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: cast                     */
/*  2011-09-23  P05.00.00.00_00.05.56.02 AP01191740 GSY_EDD_SET_CYCLE removed*/
/*  2011-09-29  S05.01.00.00_00.04.10.01 AP01236058 Lint Warnings entfernen  */
/*  2012-05-31  P05.00.00.00_00.05.97.02 AP01361288 RTC2 - Fremdlast, Fatal  */
/*              No FATAL_ERROR on GSY_SYNC_GET RetVal != LSA_RET_OK          */
/*  2012-07-13  P05.00.00.00_00.06.02.02 AP01361288_2 RTC2 - Fremdlast,Fatal */
/*              No FATAL_ERROR in gsy_SlaveSet() if SyncSetRbl is not back   */
/*              from EDD: just retry setting clock at next sync frame arrival*/
/*  2013-03-20  P05.00.00.00_00.05.97.02 AP01512559: Allow sync by FU        */
/*              after Takeover                                               */
/*  2014-05-16  P06.00.00.00_00.01.30.01 987888,1033202: EDD API-Änderung    */
/*              des Dienstes EDD_SRV_SET_SYNC_TIME                           */
/*  2014-06-05  P05.03.00.00_00.03.07.01 1028592/1066330: FuTimeout          */
/*  2014-08-08  P06.00.00.00_00.01.23.01 966426: [GSY] Die Synchronisation   */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*  2015-06-09  V06.00, Inc07, Feature 1227823, Task 1261424                 */
/*              GSY und CLP Konfigurationsschalter bereinigen                */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	7		/* GSY_MODULE_ID_MSM */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)

/*****************************************************************************/
/* Local Data                                                                */
/* ...                                                                       */
/*****************************************************************************/

/*****************************************************************************/
/* Internal function: gsy_SlaveTimerStart()                                  */
/* Start the slave takeover and timeout timer                                */
/*****************************************************************************/
LSA_VOID gsy_SlaveTimerStart(
GSY_CH_USR_PTR			pChUsr,
LSA_BOOL				Restart)
{
	LSA_UINT16 TimerCount = 0;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveTimerStart() pChUsr=%X Restart=%u",pChUsr, Restart);

	if (GSY_SLAVE_STATE_STOP == pChUsr->Slave.State)
	{
		TimerCount = 1;
	}
	else if (Restart)
	{
		/* Difference time from Takeover until Timeout in 10ms ticks
		 * 040811lrg: cast
		*/
		TimerCount = (LSA_UINT16)(((pChUsr->Slave.SyncTimeout - pChUsr->Slave.SyncTakeover) * pChUsr->Slave.SyncInterval) / 10);
	}
	else
	{
		/* Ticks until Takeover
		*/
		if (GSY_SYNC_TAKEOVER_DISABLED != pChUsr->Slave.SyncTakeover)
		{
			TimerCount = (LSA_UINT16)(pChUsr->Slave.SyncTakeover * pChUsr->Slave.SyncInterval / 10);
		}
		else if (GSY_SYNC_TIMEOUT_DISABLED != pChUsr->Slave.SyncTimeout)
		{
			/* No Takeover: ticks until Timeout
			 * 040811lrg: cast
			*/
			TimerCount = (LSA_UINT16)(pChUsr->Slave.SyncTimeout * pChUsr->Slave.SyncInterval / 10);
		}
		pChUsr->Slave.SumTimeout = 0;
	}
	GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_SlaveTimerStart(%02x) Restart=%u Time=%u * 10ms",
		pChUsr->SyncId, Restart, TimerCount);

	if (0 != TimerCount)
	{
		/* 160511lrg: Start slave timer only if takeover or timeout are configured
		*/
		gsy_TimerStart(&pChUsr->Slave.SyncTimer, TimerCount);
	}
	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveTimerStart() pChUsr=%X TimerCount=%u",pChUsr, TimerCount);
}

/*****************************************************************************/
/* Internal function: gsy_SlaveStart()                                       */
/* Start a slave enity for the SyncId of the user channel                    */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SlaveStart(
GSY_CH_USR_PTR			pChUsr)
{
	LSA_UINT16 	RespVal = GSY_RSP_OK;
	LSA_UINT8	MasterMode = GSY_LOWER_MASTER_OFF;
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveStart() pChUsr=%X SyncRole=%u", pChUsr, pChUsr->SyncRole);

	if (GSY_SYNC_ROLE_MASTER == pChUsr->SyncRole)
	{
		/* Start from master
		*/
		MasterMode = GSY_LOWER_MASTER_START;
		RespVal = GSY_RSP_OK;
	}
	else if ((GSY_MASTER_STATE_OFF != pChUsr->Master.State)
		 ||  (GSY_SLAVE_STATE_OFF != pChUsr->Slave.State))
	{
		/* Start from user and master or slave is already running
		*/
		RespVal = GSY_RSP_ERR_SEQUENCE;
	}

	if (GSY_RSP_OK == RespVal)
	{
		/* Start slave
		*/
		if ((GSY_HW_TYPE_SLAVE_HW == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
		&&  ((GSY_SYNCID_CLOCK == pChUsr->SyncId) || (GSY_SYNCID_TIME == pChUsr->SyncId)))
		{
			/* Start KRISC32 slave by sending a RQB to EDDP
			*/
			GSY_SYNC_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveStart(%02x) slave state change ->START", pChUsr->SyncId);
			pChUsr->Event.PriMasterWaits = LSA_FALSE;
			pChUsr->Event.Prm.Seconds = 0;
			pChUsr->Event.Prm.Nanoseconds = 0;
			pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
			pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
			pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE; 
			pChUsr->Slave.SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED;
			pChUsr->Slave.State = GSY_SLAVE_STATE_OFF;
			pChUsr->Slave.MasterAddr = NoMacAddr;
			pChUsr->Slave.AdjustSeconds = 0;
			pChUsr->Slave.AdjustNanos = 0;
			pChUsr->Slave.SeqId = 0;
			pChUsr->Slave.SyncPortId = 0;
			pChUsr->Slave.WaitFU = LSA_FALSE;
			pChUsr->Slave.SumTimeout = 0;
			pChUsr->Slave.FuTimeout = 0;
			pChUsr->Slave.AdjustTermP = 0;
			pChUsr->Slave.AdjustTermI = 0;
			pChUsr->Slave.pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];
			pChUsr->Slave.pDrift->State = GSY_DRIFT_STATE_UNKNOWN;
			pChUsr->Slave.pDrift->SyncId = pChUsr->SyncId;

			gsy_MasterSlaveControl(pChUsr, GSY_LOWER_SLAVE_START, MasterMode);

			/* Activate reception of AnnounceFrames
			*/
			RespVal = gsy_FwdUserForwardCtrl(pChUsr->pChSys, GSY_FWD_MODE_INPUT, 
											(LSA_UINT32)pChUsr->SyncId);
		}
		else
		{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
			LSA_UINT16	RetVal;

			/* Start SW-Slave
			*/
			pChUsr->Slave.SyncOutTime = 0;

			/* Calculate best combination of timer base and count
			 * and allocate slave timer
			 * 210807lrg001: Sync/SendInterval are muliple of 10ms
			*/
			RetVal = gsy_TimerAlloc(&pChUsr->Slave.SyncTimer, LSA_TIMER_TYPE_ONE_SHOT, LSA_TIME_BASE_10MS);

			if (LSA_RET_OK != RetVal)
			{
				RespVal = GSY_RSP_ERR_RESOURCE;
			}
			else
			{
				/* Activate reception of Announce-, Sync- and FollowUp-Frames
				*/
				RespVal = gsy_FwdUserForwardCtrl(pChUsr->pChSys, GSY_FWD_MODE_ENABLE, 
												(LSA_UINT32)pChUsr->SyncId);
				if (GSY_RSP_OK == RespVal)
				{
					/* State: wait for master
					*/
					pChUsr->Event.PriMasterWaits = LSA_FALSE;
					pChUsr->Event.Prm.Seconds = 0;
					pChUsr->Event.Prm.Nanoseconds = 0;
					pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
					pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE; 
					pChUsr->Slave.SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED;
					pChUsr->Slave.State = GSY_SLAVE_STATE_FIRST;
					pChUsr->Slave.MasterAddr = NoMacAddr;
					pChUsr->Slave.AdjustSeconds = 0;
					pChUsr->Slave.AdjustNanos = 0;
					pChUsr->Slave.SeqId = 0;
					pChUsr->Slave.SyncPortId = 0;
					pChUsr->Slave.WaitFU = LSA_FALSE;
					pChUsr->Slave.SumTimeout = 0;
					pChUsr->Slave.AdjustTermP = 0;
					pChUsr->Slave.AdjustTermI = 0;

					/* 261006lrg002: Reset control unit 070108lrg002: Error Trace on RetVal
					*/
					RetVal = gsy_SyncReset(0, pChUsr->SyncId, pChUsr);
					if (LSA_RET_OK != RetVal)
					{
						GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveStart() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
										pChUsr, pChUsr->SyncId, RetVal);
					}
					/* 160511lrg: start slave timer
					*/
					gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
					GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveStart(%02x) SyncInterval=%u Takeover=%u Timeout=%u: slave state change ->START",
									pChUsr->SyncId, pChUsr->Slave.SyncInterval, pChUsr->Slave.SyncTakeover, pChUsr->Slave.SyncTimeout);
					gsy_SyncInd(pChUsr, LSA_FALSE);
	 			}
				else
				{
					/* Free allocated timer on error
					*/
					(LSA_VOID)gsy_TimerClean(&pChUsr->Slave.SyncTimer);
				}
			}
#else
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
		}
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveStart() pChUsr=%X RespVal=0x%x", pChUsr, RespVal);

	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_SlaveStop()                                 */
/* The SyncSlave is finished when all lower RQBs are back                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveStop(
GSY_CH_USR_PTR	pChUsr)
{
	LSA_UINT16 RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveStop() pChUsr=%X SyncId=0x%02x SlaveState=0x%x MasterState=0x%x",
					pChUsr, pChUsr->SyncId , pChUsr->Slave.State, pChUsr->Master.State);

	/* If clock slave and all lower RQBs are back...
	 * 190707lrg001: now it works also for SyncId != GSY_SYNCID_CLOCK
	 * 151210lrg001: do not wait for pSyncSetRbl if PLL sync is running
	*/
	if ((GSY_SYNCID_CLOCK != pChUsr->SyncId)
	||  (GSY_HW_TYPE_SLAVE_SW != (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
	||  ((!LSA_HOST_PTR_ARE_EQUAL(pChUsr->pSyncSetRbl, LSA_NULL) || (GSY_PLL_STATE_SYNC_OFF != pChUsr->PLLData.State))
	 &&  (pChUsr->qRTC2Rbl.Count == pChUsr->RTC2RblCount)))
	{
		pChUsr->Slave.State = GSY_SLAVE_STATE_OFF;
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveStop(%02x) MasterState=%u slave state change ->OFF", pChUsr->SyncId, pChUsr->Master.State);

		if ((GSY_HW_TYPE_SLAVE_HW != (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
		||  ((GSY_SYNCID_CLOCK != pChUsr->SyncId) && (GSY_SYNCID_TIME != pChUsr->SyncId)))
		{
			/* 070108lrg001: Arts-RQ AP00561490 Reset of controller and PhaseCorrectionRegister
			*/
			RetVal = gsy_SyncReset(0, pChUsr->SyncId, pChUsr);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveStop() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
								pChUsr, pChUsr->SyncId, RetVal);
			}
		}
		/* If the master state is stopped , 
		 * the master will finish the upper Request or Close
		*/
		if ((GSY_MASTER_STATE_STOP != pChUsr->Master.State)
		&&  (GSY_MASTER_STATE_STOPPING != pChUsr->Master.State))
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
				/* Trigger PrmCommit state machine
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
		pChUsr->Slave.State = GSY_SLAVE_STATE_STOPPING;
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveStop(%02x) RTC2RblCount=0x%x slave state change ->STOPPING", pChUsr->SyncId, pChUsr->RTC2RblCount);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveStop() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_SlaveTimer()                                */
/* Timer function for synchronisation of the PTCP Clocks                     */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveTimer(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT16			RetVal;
	GSY_CH_USR_PTR		pChUsr = pTimer->pChUsr;
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	LSA_BOOL			DiagSend = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveTimer() pTimer=%X pChUsr=%X SlaveState=%d",
					pTimer, pChUsr, pChUsr->Slave.State);

	if (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
	{
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveTimer() pChUsr=%X SyncId=0x%02x: master is primary",
					pChUsr, pChUsr->SyncId);
	}
	else if ((GSY_SLAVE_STATE_OFF == pChUsr->Slave.State) 
		 ||  (GSY_SLAVE_STATE_STOPPING == pChUsr->Slave.State))
	{
		GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveTimer() pChUsr=%X SyncId=0x%02x: no slave running State=%d",
					pChUsr, pChUsr->SyncId, pChUsr->Slave.State);
	}
	else if ((GSY_SLAVE_STATE_WAIT == pChUsr->Slave.State)
		 &&  GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))		//080611lrg
	{
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveTimer() pChUsr=%X SyncId=0x%02x: slave is already waiting",
					pChUsr, pChUsr->SyncId);
	}
	else if (GSY_SLAVE_STATE_STOP == pChUsr->Slave.State)
	{
		/* Slave stopped: stop and free timer
		 * 250706lrg001: retry deallocation on error
		*/
		RetVal = gsy_TimerClean(&pChUsr->Slave.SyncTimer);
		GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveTimer(%02x) TimerClean=0x%x slave state change ->STOP", 
							pChUsr->SyncId, RetVal);
		if (LSA_RET_OK == RetVal)
		{
			gsy_SlaveStop(pChUsr);
		}
		else
		{
			/* Start Timer again for retry
			*/
			GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveTimer(%02x) RetVal=0x%x from gsy_TimerClean()",
								pChUsr->SyncId, RetVal);
			gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
		}
	}
	else		// 080611lrg: handle takeover or timeout even in GSY_SLAVE_STATE_WAIT_SET
	{
		if ((GSY_SYNC_TIMEOUT_DISABLED != pChUsr->Slave.SyncTimeout)
		||  (GSY_SYNC_TAKEOVER_DISABLED != pChUsr->Slave.SyncTakeover))
		{
			/* Increment and check timeout counter for sync frames from the master
			*/
			pChUsr->Slave.SumTimeout++;

			if ((GSY_SYNC_TIMEOUT_DISABLED != pChUsr->Slave.SyncTimeout)
			&&  ((2 == pChUsr->Slave.SumTimeout)
			 ||  (GSY_SYNC_TAKEOVER_DISABLED == pChUsr->Slave.SyncTakeover)
			 ||  (pChUsr->Slave.SyncTimeout <= pChUsr->Slave.SyncTakeover)))
			{
				/* Slave timeout
				*/
 				if (!GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))
				{
 					/* 270907lrg005: set MasterMAC to NULL
					*/
					pChUsr->Slave.MasterAddr = NoMacAddr;
					DiagSend = LSA_TRUE;
				}
				if (GSY_SLAVE_STATE_WAIT != pChUsr->Slave.State)
				{
					GSY_DRIFT_PTR pDrift = &pChUsr->pChSys->Drift[pChUsr->SyncId][0];

					/* 211207lrg001: notify NotSync on timeout also after state change
					 * of the master to SlaveMode (GSY_SLAVE_STATE_FIRST)
					*/
					if (pDrift->Diag.StateSync)
						gsy_SyncInd(pChUsr, LSA_FALSE);

					pChUsr->Slave.State = GSY_SLAVE_STATE_WAIT;
					pChUsr->Slave.AveSumCount = 0;
					if (!pChUsr->Event.PriMasterWaits)
					{
						/* 231006lrg002: do not notify timeout if primary master waits
						*/
						pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
						gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
					}

					GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_SlaveTimer(%02x) SyncTimeout=%u Offset=%d SeqId=%u OrgTime=%u,%u: slave state change ->WAIT",
										pChUsr->SyncId, pChUsr->Slave.SyncTimeout, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId, pChUsr->Slave.Act.OrgSeconds, pChUsr->Slave.Act.OrgNanos);

					if (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
					{
						/* 301110lrg001: Indicate slave Timeout to master
						*/
						gsy_MasterAging(pChUsr);
					}
				}
				else if (DiagSend)
				{
					/* 080611lrg: Timeout between NewMaster Sync- and FU frame
					*/
					GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_SlaveTimer(%02x) Offset=%d SeqId=%u: slave state change [%u] MasterMAC->NIL",
									pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId, pChUsr->Slave.State);

					/* notify change of MasterMAC to 0
					*/
					gsy_SyncInd(pChUsr, LSA_FALSE);
				}
				/* 240907lrg003: reset counter because of change from primary master
				*/
				pChUsr->Slave.SumTimeout = 0;
			}
			else if ((GSY_SYNC_TAKEOVER_DISABLED != pChUsr->Slave.SyncTakeover)
			&&  (1 == pChUsr->Slave.SumTimeout))
			{
				/* 270907lrg005: Takeover - prepare to accept a new master
				*/
				if (!GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr)
				||  (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State))		//060411lrg001: trigger BMA on TAKEOVER
				{
					GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_SlaveTimer(%02x) SyncTakeover=%u Offset=%d SeqId=%u OrgTime=%u,%u: slave state change ->TAKEOVER",
										pChUsr->SyncId, pChUsr->Slave.SyncTakeover, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId, pChUsr->Slave.Act.OrgSeconds, pChUsr->Slave.Act.OrgNanos);
					pChUsr->Slave.MasterAddr = NoMacAddr;

					if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr->Slave.pDrift, LSA_NULL))
						pChUsr->Slave.pDrift->MasterAddr = NoMacAddr;

					if (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
					{
						/* 301110lrg001: Indicate slave Takeover to master
						*/
						gsy_MasterAging(pChUsr);
					}
					else
					{
						/* 041007lrg001: notify change of MasterMAC to 0
						*/
						if (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)
							gsy_SyncInd(pChUsr, LSA_TRUE);
						else
							gsy_SyncInd(pChUsr, LSA_FALSE);
					}
				}
				if ((GSY_SYNC_TIMEOUT_DISABLED != pChUsr->Slave.SyncTimeout)
				&&  (pChUsr->Slave.SyncTimeout > pChUsr->Slave.SyncTakeover))
				{
					/* Restart slave timer
					*/
					gsy_SlaveTimerStart(pChUsr, LSA_TRUE);
				}
			}
			else
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_SlaveTimer(%02x) on SyncTimeout=%u and SyncTakeover=%u",
									pChUsr->SyncId, pChUsr->Slave.SyncTimeout, pChUsr->Slave.SyncTakeover);
			}
		}
	}
	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveTimer() pTimer=%X pChUsr=%X SlaveState=%d",
					pTimer, pChUsr, pChUsr->Slave.State);
}
/*****************************************************************************/
/* Internal access function: gsy_SlaveAdjust()                               */
/* Time adjust function for synchronisation of the PTCP Clocks               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveAdjust(
GSY_CH_USR_PTR		pChUsr)
{
	LSA_INT32			DriftInterval = 0;
	LSA_UINT16			RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveAdjust() pChUsr=%X SlaveState=%d",
					pChUsr, pChUsr->Slave.State);

	if ((GSY_MASTER_STATE_PRIMARY != pChUsr->Master.State)
	&&  ((GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State) 
	 ||  (GSY_SLAVE_STATE_SYNC_IN == pChUsr->Slave.State)
	 ||  (GSY_SLAVE_STATE_SET == pChUsr->Slave.State) 
	 ||  (GSY_SLAVE_STATE_OUT == pChUsr->Slave.State) 
	 ||  (GSY_SLAVE_STATE_SYNC_OUT == pChUsr->Slave.State)
	 ||  (GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State)))
	{
		if ((GSY_SLAVE_STATE_FIRST != pChUsr->Slave.State)
		&&  (0 != pChUsr->Slave.AveSumCount))
		{
			/* Master is NOT primary BestMaster and no TIMEOUT:
			 * Let control algorithm calculate adjust value for actual time and adjust PTCP time
			 * *** If AveTimeOffset is negative, the local clock is too slow ***
			 * 290606lrg001: new parameter AdjustTermP, AdjustTermI 
			*/
			RetVal = gsy_SyncAlgorithm(pChUsr->Slave.AdjustSeconds, pChUsr->Slave.AdjustNanos, 
										pChUsr->Slave.AveTimeOffset, &pChUsr->Slave.AdjustInterval, 
										&pChUsr->Slave.AdjustTermP, &pChUsr->Slave.AdjustTermI, pChUsr->SyncId, pChUsr);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAdjust() pChUsr=%X GSY_SYNC_ALGORITHM: RetVal=0x%x",
								pChUsr, RetVal);
			}
			else
			{
				/* Get actual drift for RESET und Trace
				*/
				if (GSY_DRIFT_STATE_READY == pChUsr->Slave.pDrift->State)
					DriftInterval = pChUsr->Slave.pDrift->AveInterval;

				if  ((0 == pChUsr->Slave.WindowSize)
				||   ((pChUsr->Slave.AveTimeOffset <= (LSA_INT32)pChUsr->Slave.WindowSize)
				 &&  (pChUsr->Slave.AveTimeOffset >= -((LSA_INT32)pChUsr->Slave.WindowSize))))
/* 270706lrg002: Bug of GNU-Compiler (0-)
				 &&  (pChUsr->Slave.AveTimeOffset >= (0 - (LSA_INT32)pChUsr->Slave.WindowSize))))
*/
				{
					/* Offset in PLL-Window
					*/
					switch (pChUsr->Slave.State)
					{
					case GSY_SLAVE_STATE_OUT:
					case GSY_SLAVE_STATE_SET:

						if (pChUsr->Slave.pDrift->State == GSY_DRIFT_STATE_READY)
						{
							/* New SyncState is: sync_in (200207lrg001)
							*/
							GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change %d->SYNC_IN",
												pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId, pChUsr->Slave.State);
							pChUsr->Slave.State = GSY_SLAVE_STATE_SYNC_IN;
							pChUsr->Slave.SyncInAct = 0;
						}
						break;

					case GSY_SLAVE_STATE_SYNC_IN:
						pChUsr->Slave.SyncInAct += pChUsr->Slave.SyncInterval;
						if (pChUsr->Slave.SyncInAct >= pChUsr->Slave.SyncInTime)
						{
							/* When the SyncIn time is reached, 
							 * SyncState changes to: synchronous
							*/
							pChUsr->Slave.State = GSY_SLAVE_STATE_SYNC;
							gsy_SyncInd(pChUsr, LSA_TRUE);

							if (GSY_SYNC_EVENT_SYNC_OK != pChUsr->Event.SyncEvent)	//170706lrg002
							{
								/* Notify user by indication
								*/
								pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK; 
								gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
							}
							GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_IN->SYNC",
												pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId);
						}
						break;

					case GSY_SLAVE_STATE_SYNC:
						/* SyncState remains: synchronous
						*/
						break;

					case GSY_SLAVE_STATE_SYNC_OUT:
						/* New SyncState is again: synchronous
						*/
						pChUsr->Slave.State = GSY_SLAVE_STATE_SYNC;
						pChUsr->Slave.SyncOutAct = 0;
						GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_OUT->SYNC",
											pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId);
						break;

					default:
						GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveAdjust() pChUsr=%X invalid Slave.State=0x%x", pChUsr, pChUsr->Slave.State);
						gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SYNCSTATE, pChUsr, pChUsr->Slave.State);

					}
				}
				else
				{
					/* Offset out of PLL-Window
					*/
					switch (pChUsr->Slave.State)
					{
					case GSY_SLAVE_STATE_SYNC:
						/* New SyncState is: sync_out
						*/
						pChUsr->Slave.State = GSY_SLAVE_STATE_SYNC_OUT;
						pChUsr->Slave.SyncOutAct = 0;
						if  (pChUsr->Slave.AveTimeOffset < (LSA_INT32)pChUsr->Slave.WindowSize)
							pChUsr->Statistics.PLLWindowUnderun++;
						else
							pChUsr->Statistics.PLLWindowOverrun++;
						GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC->SYNC_OUT",
											pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId);
						break;

					case GSY_SLAVE_STATE_SYNC_OUT:
						pChUsr->Slave.SyncOutAct += pChUsr->Slave.SyncInterval;
						if (pChUsr->Slave.SyncOutAct >= pChUsr->Slave.SyncOutTime)
						{
							/* When the SyncOut time is reached, 
							 * SyncState changes to: not synchronous
							*/
							pChUsr->Slave.State = GSY_SLAVE_STATE_OUT;
							gsy_SyncInd(pChUsr, LSA_FALSE);

							/* Reset controller
							*/
							RetVal = gsy_SyncReset(DriftInterval, pChUsr->SyncId, pChUsr);
							if (LSA_RET_OK != RetVal)
							{
								GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAdjust() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
												pChUsr, pChUsr->SyncId, RetVal);
							}
							/* Notify user by indication
							*/
							pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY; 
							gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
							GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u OrgTime=%u,%u: slave state change SYNC_OUT->OUT",
												pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId, pChUsr->Slave.Act.OrgSeconds, pChUsr->Slave.Act.OrgNanos);
						}
						break;

					case GSY_SLAVE_STATE_SET:
					case GSY_SLAVE_STATE_OUT:
						/* SyncState remains: not synchronous
						*/
						break;

					case GSY_SLAVE_STATE_SYNC_IN:
						/* New SyncState is again: not synchronous
						*/
						pChUsr->Slave.State = GSY_SLAVE_STATE_OUT;
						pChUsr->Slave.SyncInAct = 0;
		
						/* Reset controller
						*/
						RetVal = gsy_SyncReset(DriftInterval, pChUsr->SyncId, pChUsr);
						if (LSA_RET_OK != RetVal)
						{
							GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAdjust() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
											pChUsr, pChUsr->SyncId, RetVal);
						}
						GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_IN->OUT",
											pChUsr->SyncId, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.SeqId);
						break;

					default:
						GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveAdjust() pChUsr=%X invalid Slave.State=0x%x", pChUsr, pChUsr->Slave.State);
						gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SYNCSTATE, pChUsr, pChUsr->Slave.State);
					}

				}

				if (0 == GSY_SYNC_MAX_COUNT)       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to False, define-based behaviour, HM 20.05.2016
				{
					/* Start new average with 0 as first value
					*/
					pChUsr->Slave.AveSumOffset = 0;
					pChUsr->Slave.AveTimeOffset = 0;
					pChUsr->Slave.AveSumCountLast = pChUsr->Slave.AveSumCount;
					pChUsr->Slave.AveSumCount = 0;
				}

			}

		}
		else if ((GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State)		//170706lrg002 
		     &&  (GSY_SYNC_EVENT_NONE == pChUsr->Event.SyncEvent))
		{
			/* Slave started and no Sync/Fu-Message received yet:
			 * first indication to User 170706lrg002: OK instead of NO_MSG
			*/
			pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK; 
			gsy_SyncUserInd(pChUsr, LSA_TRUE, LSA_TRUE);
		}

		/* Update diagnosis data and send Indication
		 * 130606lrg002: not only if offset or SyncState have changed
		*/
		gsy_DiagUserInd(GSY_DIAG_SOURCE_ADJUST, pChUsr->pChSys, pChUsr->Slave.pDrift, 
		    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
						pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
						0       						   /* LSA_INT32: Offset */, 
						pChUsr->Slave.AdjustInterval       /* LSA_INT32: AdjustInterval */, 
						pChUsr->Slave.State                /* LSA_INT32: UserValue1 */, 
						2 	    						   /* LSA_INT32: UserValue2 */, 
						0                                  /* LSA_INT32: SetTimeHigh */, 
						0                                  /* LSA_INT32: SetTimeLow */, 
						pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */);

		GSY_SYNC_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--+ gsy_SlaveAdjust(%02x) OCFInterval=%d AveOffset=%d RCFInterval=%d SumTimeout=%u SlaveState=%u SeqId=%u MasterState=%u",
			pChUsr->SyncId, pChUsr->Slave.AdjustInterval, pChUsr->Slave.AveTimeOffset, DriftInterval, pChUsr->Slave.SumTimeout, pChUsr->Slave.State, pChUsr->Slave.SeqId, pChUsr->Master.State);
	}

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveAdjust() pChUsr=%X AveTimeOffset=%d AdjustInterval=%d SlaveState=%d",
						pChUsr, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.AdjustInterval, pChUsr->Slave.State);
}

/*****************************************************************************/
/* Internal function: gsy_SlaveSet()                                         */
/* Set local PTCP time strongly to reference time.                           */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SlaveSet(
GSY_CH_USR_PTR	pChUsr,
LSA_UINT16		Epoch,
LSA_UINT32		Seconds,
LSA_UINT32		Nanos,
LSA_UINT32		TimeStamp)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT16				RetVal = GSY_RSP_OK;

    #if (GSY_CFG_TRACE_MODE == 0)
        LSA_UNUSED_ARG(Epoch);
    #endif
	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveSet() pChUsr=%X Seconds=%u Nanos=%u", pChUsr, Seconds, Nanos);

	GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_SlaveSet(%02x) Epoch=%u Seconds=%u Nanos=%u Timestamp=%u",
					pChUsr->SyncId, Epoch, Seconds, Nanos, TimeStamp);

	if (GSY_SYNCID_CLOCK == pChUsr->SyncId)
	{
		/* Use asynchronous request on clock synchronisation
		*/
		pRbl = pChUsr->pSyncSetRbl;
		if  (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			if (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK))
			{
                /* 987888,1033202: Use PTCP offset with sign on INTEL i210
                */
                GSY_LOWER_RQB_SYNC_MODE_SET_OFFSET(pRbl);
				if (pChUsr->Slave.SecondsOffset < 0)
				{
					/* Offset negative: correction value has to be positive
					*/
	                GSY_LOWER_RQB_SYNC_SIGN_SET_POSITIVE(pRbl);
					if (pChUsr->Slave.AveTimeOffset < 0)
					{
						GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, (LSA_UINT32)(-pChUsr->Slave.AveTimeOffset));
					}
					else
					{
						GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, (LSA_UINT32)(pChUsr->Slave.AveTimeOffset));
					}
					GSY_LOWER_RQB_SYNC_SECONDS_SET(pRbl, (LSA_UINT32)(-pChUsr->Slave.SecondsOffset));
				}
				else
				{
					if ((pChUsr->Slave.SecondsOffset == 0) && (pChUsr->Slave.AveTimeOffset < 0))
					{
						/* Offset negative: correction value has to be positive
						*/
	                    GSY_LOWER_RQB_SYNC_SIGN_SET_POSITIVE(pRbl);
						GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, (LSA_UINT32)(-pChUsr->Slave.AveTimeOffset));
					}
					else

					{
						/* Offset positive: correction value has to be negative
						*/
	                    GSY_LOWER_RQB_SYNC_SIGN_SET_NEGATIVE(pRbl);
						GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, (LSA_UINT32)(pChUsr->Slave.AveTimeOffset));
					}
					GSY_LOWER_RQB_SYNC_SECONDS_SET(pRbl, (LSA_UINT32)(pChUsr->Slave.SecondsOffset));
				}
			}
			else
			{
                /* Use absolute PTCP time with local timestamp
                */
                GSY_LOWER_RQB_SYNC_MODE_SET_ABSOLUTE(pRbl);
				GSY_LOWER_RQB_SYNC_NANOSECONDS_SET(pRbl, Nanos);
				GSY_LOWER_RQB_SYNC_SECONDS_SET(pRbl, Seconds);
				GSY_LOWER_RQB_SYNC_TIMESTAMP_SET(pRbl, TimeStamp);
			}
			GSY_LOWER_RQB_SYNC_RCF_INTERVAL_SET(pRbl, pChUsr->Slave.pDrift->AveInterval);
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
			RetVal = GSY_RSP_OK_ACTIVE;
		}
		else
		{
			/* AP01361288_2: do not call gsy_ErrorInternal(). Just retry setting clock at next sync frame arrival
			*/
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveSet() pChUsr=%X no SyncSetRbl", pChUsr);
			RetVal = GSY_RSP_ERR_RESOURCE;
		}

		/* 140208lrg001: also to diagnosis record
		*/
		pChUsr->Slave.AdjustTermP =	0;
		pChUsr->Slave.AdjustTermI =	0;

		gsy_DiagUserInd(GSY_DIAG_SOURCE_SET, pChUsr->pChSys, pChUsr->Slave.pDrift, 
		    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
						pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
						pChUsr->Slave.AveTimeOffset		   /* LSA_INT32: Offset */, 
						0                                  /* LSA_INT32: AdjustInterval */, 
						pChUsr->Slave.Act.SeqId            /* LSA_INT32: UserValue1 */, 
						1 	    						   /* LSA_INT32: UserValue2 */, 
		    			Seconds                            /* LSA_INT32: SetTimeHigh */, 
						Nanos                              /* LSA_INT32: SetTimeLow */, 
						pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */);

		pChUsr->Slave.AdjustTermP =	0;
		pChUsr->Slave.AdjustTermI =	0;
	}
	else
	{
		/* Use synchronous output macro on other SyncIds than GSY_SYNCID_CLOCK
		*/
		RetVal = GSY_SYNC_SET(Epoch, Seconds, Nanos, TimeStamp, pChUsr->Slave.pDrift->AveInterval, pChUsr->SyncId, pChUsr->pSys);

		gsy_DiagUserInd(GSY_DIAG_SOURCE_SET, pChUsr->pChSys, pChUsr->Slave.pDrift, 
		    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
						pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
						pChUsr->Slave.AveTimeOffset		   /* LSA_INT32: Offset */, 
						0                                  /* LSA_INT32: AdjustInterval */, 
						pChUsr->Slave.Act.SeqId            /* LSA_INT32: UserValue1 */, 
						1 	    						   /* LSA_INT32: UserValue2 */, 
						Seconds                   		   /* LSA_INT32: SetTimeHigh */, 
						Nanos                              /* LSA_INT32: SetTimeLow */, 
						pChUsr->Slave.SyncPortId  		   /* LSA_INT16: PortId */);

		if (LSA_RET_OK != RetVal)       //lint !e774 Boolean within 'if' always evaluates to False, it depends on GSY_SYNC_SET() implementation, HM 20.05.2016
		{
			GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveSet() pChUsr=%X SyncId=%02x GSY_SYNC_SET: RetVal=0x%x", 
							pChUsr, pChUsr->SyncId, RetVal);
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, pChUsr, 0);
		}
		else
			RetVal = GSY_RSP_OK;
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveSet() pChUsr=%X TimeStamp=%u RetVal=0x%x",
					pChUsr, TimeStamp, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: gsy_SlaveAction()                                      */
/* Calculate average value of offset to master time.                         */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveAction(
GSY_CH_USR_PTR			pChUsr)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

    LSA_UINT32  OrgMasterTimeInS;
	LSA_UINT64  OrgMasterTimeInNS;
	LSA_UINT32  OrgMasterTimeStampInTicks;

	//LSA_UINT64  RefActMasterTimeInNS;
    //LSA_UINT32  RefActMasterTimeInS;

#ifdef GSY_REF_CYCLE_BEGIN
	LSA_UINT64  RefCycleBeginMasterTimeInNS;
    LSA_UINT32  RefCycleBeginMasterTimeInS;
	LSA_UINT32  RefCycleBeginMasterTimeStampInTicks;
#else
    LSA_UINT32  RecvLocalSeconds = 0;
    LSA_UINT64  RecvLocalNanos = 0;
#endif
    LSA_INT32   DriftTimeInNS = 0;		/* 250906lrg001 */
    LSA_UINT32  RefMasterTimeInS;
	LSA_UINT64  RefMasterTimeInNS;
	LSA_UINT32  RefMasterTimeStampInTicks;


/*  LSA_UINT32  LineDelayInNS; */
	LSA_UINT32  RcvDelayInS;
	LSA_UINT32  RcvDelayInNS;

    LSA_UINT32  OrgCycleBeginLocalTimeInS;
    LSA_UINT32  OrgCycleBeginLocalTimeInNS;
    LSA_UINT32  OrgCycleBeginLocalTimeStampInTicks = 0;

    //LSA_UINT32  RefActLocalTimeInS;
	//LSA_UINT64  RefActLocalTimeInNS;

	LSA_INT32   DiffTimeInNS = 0;
    LSA_INT32   DiffTimeInS  = 0;

	LSA_UINT32  DiffTimeStampInTicks = 0;
	LSA_UINT64  DiffTimeStampInNS    = 0;

    LSA_UINT32  ActTimeStampInTicks = 0;
	LSA_UINT16	ActLocalEpoch, OrgMasterEpoch;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveAction() pChUsr=%X OrgEpoch=%u OrgSeconds=%u OrgNanos=%u",
					pChUsr, pChUsr->Slave.Act.OrgEpoch, pChUsr->Slave.Act.OrgSeconds, pChUsr->Slave.Act.OrgNanos);


    /*********************************************************************************************************/
    /* 1. Find PTCPMaster Time at local receiving time (including all Delays)                                */
    /*********************************************************************************************************/
	{
	    
 	    /* 1.1 Take PTCP-Master Time and receive timestamp
 	    */
		OrgMasterEpoch            = pChUsr->Slave.Act.OrgEpoch;
		OrgMasterTimeInS          = pChUsr->Slave.Act.OrgSeconds;
		OrgMasterTimeInNS         = (LSA_UINT64) pChUsr->Slave.Act.OrgNanos;
		OrgMasterTimeStampInTicks = pChUsr->Slave.Act.Timestamp;

		/* 1.2 Add all delays from SyncFrame and FollowUp
		*/
		RcvDelayInS               = pChUsr->Slave.Act.DelaySeconds;
		RcvDelayInNS              = pChUsr->Slave.Act.DelayNanos;

		OrgMasterTimeInS         += RcvDelayInS;
		OrgMasterTimeInNS        += RcvDelayInNS;

		/* 1.3 Convert nanoseconds > 999.999.999 to seconds
		*/
		OrgMasterTimeInS         += (LSA_UINT32)(OrgMasterTimeInNS / GSY_NANOS_PER_SECOND);
		OrgMasterTimeInNS         =  OrgMasterTimeInNS % GSY_NANOS_PER_SECOND;

		GSY_SYNC_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--= gsy_SlaveAction(%02x) OrgMasterTimeInS=%09u OrgMasterTimeInNS=%09u OrgMasterTimeStampInTicks=%09u RcvDelayInS=%09u RcvDelayInNS=%09u LineDelay=%09u",
			              pChUsr->SyncId, OrgMasterTimeInS,(LSA_UINT32)OrgMasterTimeInNS, OrgMasterTimeStampInTicks, RcvDelayInS, RcvDelayInNS, 
			              pChUsr->pChSys->Port[pChUsr->Slave.SyncPortId-1].SyncLineDelay);
	

	}

	if ((pChUsr->SyncId == GSY_SYNCID_CLOCK)
	&&  (GSY_HW_TYPE_I210 == (pChUsr->pChSys->HardwareType & GSY_HW_TYPE_MSK)))
	{
		/* lrgi210: local PTCP time is inserted in front of frame by hardware
		*/
		RecvLocalNanos = pChUsr->Slave.Act.LocalNanos;
		RecvLocalSeconds = pChUsr->Slave.Act.LocalSeconds;

		/* Convert nanoseconds > 999.999.999 to seconds
		*/
		RecvLocalSeconds += (LSA_UINT32)(RecvLocalNanos / GSY_NANOS_PER_SECOND);
		RecvLocalNanos =  RecvLocalNanos % GSY_NANOS_PER_SECOND;
	}
	else
	{
		/*********************************************************************************************************/
		/* 2. Find local PTCP Time of last cycle begin                                                           */
		/*********************************************************************************************************/
		{
			/* 2.1 Read local PTCP Time of last cycle begin
			 */
			RetVal = GSY_SYNC_GET(&ActLocalEpoch, &OrgCycleBeginLocalTimeInS, &OrgCycleBeginLocalTimeInNS, 
					&OrgCycleBeginLocalTimeStampInTicks, pChUsr->SyncId, pChUsr->pSys);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAction() pChUsr=%X: GSY_SYNC_GET: RetVal=0x%x", pChUsr, RetVal);
//				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, pChUsr, 0);		AP01361288
			}
			else		//AP01361288
			{
				/* 2.2 Convert nanoseconds > 999.999.999 to seconds
				 */
				OrgCycleBeginLocalTimeInS  += (LSA_UINT32)(OrgCycleBeginLocalTimeInNS / GSY_NANOS_PER_SECOND);
				OrgCycleBeginLocalTimeInNS  =  OrgCycleBeginLocalTimeInNS % GSY_NANOS_PER_SECOND;

				GSY_SYNC_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--= gsy_SlaveAction(%02x) OrgCycleBeginLocalTimeInS=%09u OrgCycleBeginLocalTimeInNS=%09u LocalTimeStampInTicks=%09u",
					pChUsr->SyncId, OrgCycleBeginLocalTimeInS, OrgCycleBeginLocalTimeInNS, OrgCycleBeginLocalTimeStampInTicks);
			}
		}
	
		/*********************************************************************************************************/
		/* 3. Find local reference time RefLocal in relation to the actual timestamp                             */
		/*********************************************************************************************************/
		if (LSA_RET_OK == RetVal)		//AP01361288
		{
			/* 3.1 Read actual timestamp as reference
			 */
			ActTimeStampInTicks   = GSY_GET_TIMESTAMP(pChUsr->pSys);

			/* 3.2 Calculate the difference from now to the last cycle begin
			 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
			 */	   
			DiffTimeStampInTicks  = ActTimeStampInTicks - OrgCycleBeginLocalTimeStampInTicks;
			DiffTimeStampInNS     = (LSA_UINT64)DiffTimeStampInTicks * (LSA_UINT64)pChUsr->pChSys->RxTxNanos;

			/* 3.3 Take the local PTCP Time of the last cycle begin
			 */	   
			//RefActLocalTimeInS    = OrgCycleBeginLocalTimeInS;
			//RefActLocalTimeInNS   = OrgCycleBeginLocalTimeInNS;

			/* 3.4 Add the difference to reference time
			 */	   
			//RefActLocalTimeInNS  += DiffTimeStampInNS;

			/* 3.5 Convert nanoseconds > 999.999.999 to seconds
			 */
			//RefActLocalTimeInS   += (LSA_UINT32)(RefActLocalTimeInNS / GSY_NANOS_PER_SECOND);
			//RefActLocalTimeInNS   =  RefActLocalTimeInNS % GSY_NANOS_PER_SECOND;
		}

		/*********************************************************************************************************/
		/* 4. Find master reference time RefMaster in relation to the actual timestamp                           */
		/*********************************************************************************************************/
		if (LSA_RET_OK == RetVal)		//AP01361288
		{
			/* 4.1 Calculate the difference from now to the receive time stamp
			 */	   
			DiffTimeStampInTicks         = ActTimeStampInTicks - OrgMasterTimeStampInTicks;
			DiffTimeStampInNS            = (LSA_UINT64)DiffTimeStampInTicks * (LSA_UINT64)pChUsr->pChSys->RxTxNanos;

			/* 4.2 Take the master PTCP Time of the local receive time 
			 */	   
			//RefActMasterTimeInS          = OrgMasterTimeInS;
			//RefActMasterTimeInNS         = OrgMasterTimeInNS;

			/* 4.3 Add the difference to master reference time
			 */	   
			//RefActMasterTimeInNS        += DiffTimeStampInNS;

			/* 4.4 Convert nanoseconds > 999.999.999 to seconds
			 */
			//RefActMasterTimeInS         += (LSA_UINT32)(RefActMasterTimeInNS / GSY_NANOS_PER_SECOND);
			//RefActMasterTimeInNS         = RefActMasterTimeInNS % GSY_NANOS_PER_SECOND;
		}


#ifdef GSY_REF_CYCLE_BEGIN
		/*********************************************************************************************************/
		/* 5. Find master time at local cycle begin                                                              */
		/*********************************************************************************************************/
		if (LSA_RET_OK == RetVal)		//AP01361288
		{
			/* 5.1 Shift MasterTime to local CycleBegin Timestamp
			 */
#else
		/*********************************************************************************************************/
		/* 5. Find local time at receive timestamp of SyncFrame                                                  */
		/*********************************************************************************************************/
		if (LSA_RET_OK == RetVal)		//AP01361288
		{
			/* 5.1 Shift LocalTime to Receive Timestamp  
			 */
#endif
			if( (ActTimeStampInTicks - OrgCycleBeginLocalTimeStampInTicks) > (ActTimeStampInTicks - OrgMasterTimeStampInTicks) )
			{

				/* The local CycleBegin Timestamp is prior to the Master Timestamp:
				* therefore the difference has to be subtract from OrginalMasterTime 
				* to build the correct MasterTime at cycle begin
				*
				* TIMESTAMP -->
				*   --- CYL ------- MS ----------- ACT
				*       CYL ---------------------- ACT is ACT - CYL
				*                   MS------------ ACT is ACT - MS
				*       CYL ------- MS                 difference is (ACT - CYL) - (ACT - MS)
				*
				*/
	
				/* 5.2.1 Build difference (like illustration)
				 */
				DiffTimeStampInTicks   =  ActTimeStampInTicks - OrgCycleBeginLocalTimeStampInTicks;
				DiffTimeStampInTicks  -= (ActTimeStampInTicks - OrgMasterTimeStampInTicks);
				DiffTimeStampInNS      = (LSA_UINT64)DiffTimeStampInTicks * (LSA_UINT64)pChUsr->pChSys->RxTxNanos;

				if (DiffTimeStampInNS <= GSY_NANOS_PER_SECOND)
				{
					/* 250906lrg001: "drift compensate" the time difference between the timestamps
					*/
					DriftTimeInNS = gsy_DriftCorrection(pChUsr->pChSys, (LSA_UINT32)DiffTimeStampInNS, (LSA_UINT32)pChUsr->SyncId);
					DiffTimeStampInNS += DriftTimeInNS;     //lint !e737 Loss of sign, this code is correct, Drift could be negative, HM 13.06.2016
    
#ifdef GSY_REF_CYCLE_BEGIN

					/* 5.2.2 Take the orginal MasterTime as reference time for MasterTime at cycle begin
					*/           
					RefCycleBeginMasterTimeInS          = OrgMasterTimeInS;
					RefCycleBeginMasterTimeInNS         = OrgMasterTimeInNS;
					RefCycleBeginMasterTimeStampInTicks = OrgMasterTimeStampInTicks;
	
					/* 5.2.3 Subtract difference Nanoseconds from RefCycleBeginMasterTimeInNS
					*/
					{
						LSA_UINT64  RestTimeInNS;
						LSA_UINT32  RestInS;

						RestTimeInNS = DiffTimeStampInNS;
						if( RestTimeInNS > RefCycleBeginMasterTimeInNS)
						{
							RestTimeInNS -= RefCycleBeginMasterTimeInNS;
							RefCycleBeginMasterTimeInNS = 0;
						}
						else 
						{
							RefCycleBeginMasterTimeInNS -= RestTimeInNS;
							RestTimeInNS = 0;
						}

						/* Subtract remaining Nanoseconds from S field if possible
						 */
						RestInS = (LSA_UINT32) (RestTimeInNS / GSY_NANOS_PER_SECOND);
						if( (RestTimeInNS % GSY_NANOS_PER_SECOND) > 0)
							RestInS++;

						if(RefCycleBeginMasterTimeInS >=  RestInS)
						{
							/* Move Seconds from S field to NS field
							*/
							RefCycleBeginMasterTimeInS   -= RestInS;
							RefCycleBeginMasterTimeInNS  += RestInS * GSY_NANOS_PER_SECOND;

							/* Subtract remaining Nanoseconds
							*/
							RefCycleBeginMasterTimeInNS  -= RestTimeInNS; 
						}
						else 
						{
							/* Master PTCPTime is smaller than the value to subtract!
							*/
							RetVal = LSA_RET_ERR_PARAM;
						}
					}

					/* 5.2.4 Find MasterTimeStampInTicks at cycle begin
					*/
					RefCycleBeginMasterTimeStampInTicks -= DiffTimeStampInTicks;
					if (RefCycleBeginMasterTimeStampInTicks != OrgCycleBeginLocalTimeStampInTicks)
					{
						GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveAction() pChUsr=%X: RefCycleBeginMasterTimeStampInTicks=0x%x OrgCycleBeginLocalTimeStampInTicks=0x%x",
										pChUsr, RefCycleBeginMasterTimeStampInTicks, OrgCycleBeginLocalTimeStampInTicks);
						gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, pChUsr, 0);
					}
#else
					/* Add difference between the timestamps to the GET time
					*/
					RecvLocalSeconds = OrgCycleBeginLocalTimeInS;
					RecvLocalNanos =  DiffTimeStampInNS + OrgCycleBeginLocalTimeInNS;

					/* Move hole seconds from the NS field to the S field
					*/
					RecvLocalSeconds += (LSA_UINT32)(RecvLocalNanos / GSY_NANOS_PER_SECOND);
					RecvLocalNanos = RecvLocalNanos % GSY_NANOS_PER_SECOND;
#endif
				}
				else
				{
					RetVal = LSA_RET_ERR_PARAM;
					GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveAction() pChUsr=%X SyncId=0x%02x: DiffTimeStampInNS=%u too large",
									pChUsr, pChUsr->SyncId, DiffTimeStampInNS);
				}
			}
			else 
			{

				/* The Master Timestamp is prior to the local CycleBegin Timestamp:
				* therefore the difference has to be added to OrginalMasterTime 
				* to build the correct MasterTime at cycle begin
				*
				* TIMESTAMP -->
				*   --- MS ------- CYL ----------- ACT
				*                  CYL ----------- ACT is  ACT - CYL
				*       MS------------------------ ACT is  ACT - MS
				*       MS ------- CYL                 difference is (ACT - MS) - (ACT - CYL)
				*
				*/
	
				/* 5.2.1 Build difference (like illustration)
				*/
				DiffTimeStampInTicks   =  ActTimeStampInTicks  - OrgMasterTimeStampInTicks;
				DiffTimeStampInTicks  -= (ActTimeStampInTicks  - OrgCycleBeginLocalTimeStampInTicks);
				DiffTimeStampInNS      = (LSA_UINT64)DiffTimeStampInTicks * (LSA_UINT64)pChUsr->pChSys->RxTxNanos;
	
				if (DiffTimeStampInNS <= GSY_NANOS_PER_SECOND)
				{
					/* 250906lrg001: "drift compensate" the time difference between the timestamps
					*/
					DriftTimeInNS = gsy_DriftCorrection(pChUsr->pChSys, (LSA_UINT32)DiffTimeStampInNS, (LSA_UINT32)pChUsr->SyncId);
					DiffTimeStampInNS += DriftTimeInNS;     //lint !e737 Loss of sign, this code is correct, Drift could be negative, HM 13.06.2016
    
#ifdef GSY_REF_CYCLE_BEGIN

					/* 5.2.2 Take the orginal MasterTime as reference time for MasterTime at cycle begin
					*/           
					RefCycleBeginMasterTimeInS          = OrgMasterTimeInS;
					RefCycleBeginMasterTimeInNS         = OrgMasterTimeInNS;
					RefCycleBeginMasterTimeStampInTicks = OrgMasterTimeStampInTicks;
	
					/* 5.2.3 Add difference Nanoseconds to RefCycleBeginMasterTimeInNS
					*/
					RefCycleBeginMasterTimeInNS        += DiffTimeStampInNS;
	
					/* Move hole seconds from the NS field to the S field
					*/
					RefCycleBeginMasterTimeInS         += (LSA_UINT32)(RefCycleBeginMasterTimeInNS / GSY_NANOS_PER_SECOND);
					RefCycleBeginMasterTimeInNS         = RefCycleBeginMasterTimeInNS % GSY_NANOS_PER_SECOND;
					
					/* 5.2.4 Find MasterTimeStampInTicks at cycle begin
					*/
					RefCycleBeginMasterTimeStampInTicks += DiffTimeStampInTicks;
					if (RefCycleBeginMasterTimeStampInTicks != OrgCycleBeginLocalTimeStampInTicks)
					{
						GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveAction() pChUsr=%X: RefCycleBeginMasterTimeStampInTicks=0x%x OrgCycleBeginLocalTimeStampInTicks=0x%x",
										pChUsr, RefCycleBeginMasterTimeStampInTicks, OrgCycleBeginLocalTimeStampInTicks);
						gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, pChUsr, 0);
					}
#else
					/* Subtract difference between the timestamps from the GET time (if possible)
					*/
					RecvLocalSeconds = OrgCycleBeginLocalTimeInS;
					RecvLocalNanos = (LSA_UINT64)OrgCycleBeginLocalTimeInNS;
					{
						LSA_UINT64  RestTimeInNS;
						LSA_UINT32  RestInS;
	
						RestTimeInNS = DiffTimeStampInNS;
						if( RestTimeInNS > RecvLocalNanos)
						{
							RestTimeInNS -= RecvLocalNanos;
							RecvLocalNanos = 0;
						}
						else 
						{
							RecvLocalNanos -= RestTimeInNS;
							RestTimeInNS = 0;
						}
	
						/* Subtract remaining Nanoseconds from S field if possible
						*/
						RestInS = (LSA_UINT32)(RestTimeInNS / GSY_NANOS_PER_SECOND);
						if( (RestTimeInNS % GSY_NANOS_PER_SECOND) > 0)
							RestInS++;
	
						if(RecvLocalSeconds >=  RestInS)
						{
							/* Move Seconds from S field to NS field
							*/
							RecvLocalSeconds   -= RestInS;
							RecvLocalNanos  += (LSA_UINT64)RestInS * GSY_NANOS_PER_SECOND;
	
							/* Subtract remaining Nanoseconds
							*/
							RecvLocalNanos  -= RestTimeInNS; 
						}
						else 
						{
							/* Local PTCPTime is smaller than the value to subtract!
							*/
							RetVal = LSA_RET_ERR_PARAM;
						}
					}
#endif
				}
				else
				{
					RetVal = LSA_RET_ERR_PARAM;
					GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveAction() pChUsr=%X SyncId=0x%02x: DiffTimeStampInNS=%u too large",
									pChUsr, pChUsr->SyncId, DiffTimeStampInNS);
				}
			}
		}
	}

    /*********************************************************************************************************/
    /* 6. Find local offset to master time                                                                   */
    /*********************************************************************************************************/

	if (LSA_RET_OK == RetVal)
	{
		/* Find offset (diff) of local time:
		*    Diff positive --> local time (eg 13:01) is greater than MasterTime (eg 13:00) --> The local clock is too fast 
		*    Diff negative --> local time (eg 13:00) is smaller than MasterTime (eg 13:01) --> The local clock is too slow 
		*/
#ifdef GSY_REF_CYCLE_BEGIN

		GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--= gsy_SlaveAction(%02x) OrgCycleBeginLocalTimeInS=%09u OrgCycleBeginLocalTimeInNS=%09u OrgMasterTimeInS=%09u OrgMasterTimeInNS=%09u DiffTimeStampInNS=%09u",
			pChUsr->SyncId, OrgCycleBeginLocalTimeInS, OrgCycleBeginLocalTimeInNS, OrgMasterTimeInS, (LSA_INT32)OrgMasterTimeInNS, (LSA_INT32)DiffTimeStampInNS);

		/* Diff = OrgCycleBeginLocalTime - RefCycleBeginMasterTime!
		*/
		DiffTimeInS = (LSA_INT32) (OrgCycleBeginLocalTimeInS - RefCycleBeginMasterTimeInS);
		DiffTimeInNS = (LSA_INT32) (OrgCycleBeginLocalTimeInNS - RefCycleBeginMasterTimeInNS);
		RefMasterTimeInS = RefCycleBeginMasterTimeInS;
		RefMasterTimeInNS = RefCycleBeginMasterTimeInNS;
		RefMasterTimeStampInTicks = RefCycleBeginMasterTimeStampInTicks;
#else
		GSY_SYNC_TRACE_06(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--= gsy_SlaveAction(%02x) RecvLocalSeconds=%09u RecvLocalNanos=%09u OrgMasterTimeInS=%09u OrgMasterTimeInNS=%09u DiffTimeStampInNS=%09u",
			pChUsr->SyncId, RecvLocalSeconds, (LSA_INT32)RecvLocalNanos, OrgMasterTimeInS, (LSA_INT32)OrgMasterTimeInNS, (LSA_INT32)DiffTimeStampInNS);

		/* Diff = OrgRecvLocalTime - OrgMasterTime!
		*/
		DiffTimeInS = (LSA_INT32) (RecvLocalSeconds - OrgMasterTimeInS);
		DiffTimeInNS = (LSA_INT32) (RecvLocalNanos - OrgMasterTimeInNS);
		RefMasterTimeInS = OrgMasterTimeInS;
		RefMasterTimeInNS = OrgMasterTimeInNS;
		RefMasterTimeStampInTicks = OrgMasterTimeStampInTicks;
#endif
		if ((DiffTimeInS < 0) && (DiffTimeInNS > 0))
		{
			DiffTimeInS++;
			DiffTimeInNS -= GSY_NANOS_PER_SECOND;
		}
		else if ((DiffTimeInS > 0) && (DiffTimeInNS < 0))
		{
			DiffTimeInS--;
			DiffTimeInNS += GSY_NANOS_PER_SECOND;
		}

		GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE," -- gsy_SlaveAction(%02x) DiffTimeInS=%d DiffTimeInNS=%d DriftTimeInNS=%d SlaveState=%d",
			pChUsr->SyncId, DiffTimeInS, DiffTimeInNS, DriftTimeInNS, pChUsr->Slave.State);

		if ((GSY_SLAVE_STATE_WAIT == pChUsr->Slave.State)	//280606lrg001: In WAIT --> SET
		||  ((GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State) && (GSY_MASTER_STATE_SLAVE != pChUsr->Master.State))	//170706lrg002 020708lrg001
		||  ((pChUsr->Slave.WindowSize != 0)																		//151210lrg001
		 &&	 ((DiffTimeInS != 0)
		 ||  (DiffTimeInNS > (LSA_INT32)(pChUsr->Slave.WindowSize * GSY_SYNC_WINDOW_RESET_FACTOR))
		  ||  (DiffTimeInNS < (LSA_INT32)(-((LSA_INT32)pChUsr->Slave.WindowSize * GSY_SYNC_WINDOW_RESET_FACTOR))))
		   ))
		{
			/* Set new local time after timeout or at first SyncFrame from master
			 * or offset is >= 1 second 
			 * or offset is > GSY_SYNC_WINDOW_RESET_FACTOR * PLLwindowSize
			*/
			if (GSY_DRIFT_STATE_READY == pChUsr->Slave.pDrift->State)
			{
     			LSA_INT32  DriftInterval;
				
				/* If the local drift interval has already been calculated...
				*/
				DriftInterval = pChUsr->Slave.pDrift->AveInterval;
				RetVal = gsy_SyncReset(DriftInterval, pChUsr->SyncId, pChUsr);
				if (LSA_RET_OK != RetVal)
				{
					GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAction() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
									pChUsr, pChUsr->SyncId, RetVal);
				}

				/* Set local time to master reference time
				 * 250906lrg001: RefMasterTime instead of OrgMasterTime
				 * 100107lrg001: Set AveTimeOffset for Diagnosis record
				*/
				pChUsr->Slave.AveTimeOffset = DiffTimeInNS;
				pChUsr->Slave.SecondsOffset = DiffTimeInS;		//lrgi210
				RetVal = gsy_SlaveSet(pChUsr, OrgMasterEpoch, RefMasterTimeInS, (LSA_UINT32)RefMasterTimeInNS, RefMasterTimeStampInTicks);

				if ((GSY_RSP_OK_ACTIVE == RetVal) || (GSY_RSP_OK == RetVal))	//AP01361288_2
				{
					if ((GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY != pChUsr->Event.SyncEvent)
					&&  (GSY_SLAVE_STATE_FIRST != pChUsr->Slave.State))		//170706lrg002
					{
						/* Send User Indication
						*/
						if ((GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)
						||  (GSY_SLAVE_STATE_SYNC_OUT == pChUsr->Slave.State))
							gsy_SyncInd(pChUsr, LSA_FALSE);
						pChUsr->Event.Prm.Seconds = OrgMasterTimeInS; 
						pChUsr->Event.Prm.Nanoseconds = (LSA_UINT32) OrgMasterTimeInNS; 
						pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY;
						gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
					}

					if (GSY_RSP_OK_ACTIVE == RetVal)
					{
						GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAction(%02x) Offset=%ds %dns SeqId=%u: slave state change %u->WAIT_SET",
								pChUsr->SyncId, DiffTimeInS, DiffTimeInNS, pChUsr->Slave.SeqId, pChUsr->Slave.State);
						pChUsr->Slave.State = GSY_SLAVE_STATE_WAIT_SET;
					}
					else
					{
						GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAction(%02x) Offset=%ds %dns SeqId=%u: slave state change %u->SET",
								pChUsr->SyncId, DiffTimeInS, DiffTimeInNS, pChUsr->Slave.SeqId, pChUsr->Slave.State);
						pChUsr->Slave.State = GSY_SLAVE_STATE_SET;
					}

					/* 160511lrg: start slave timer
					*/
					gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
					pChUsr->Statistics.FrameOk++;

					/* Reset offset avarage value
					*/
					pChUsr->Slave.AveTimeOffset = 0;
					pChUsr->Slave.AveSumOffset = 0;
					pChUsr->Slave.AveSumCountLast = 0;
					pChUsr->Slave.AveSumCount = 0;
					pChUsr->Slave.AveMaxOffset = 0;
					pChUsr->Slave.AveMinOffset = 0;
				}
			}
			else
			{
				GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_SlaveAction() pChUsr=%X SyncId=0x%02x: No RCFInterval",
						pChUsr, pChUsr->SyncId);
			}
		}
		else if ((GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State) && (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State))
		{
			/* 020708lrg001: Transition from master to slave and offset < GSY_SYNC_WINDOW_RESET_FACTOR * PLL-Window
			*/
			pChUsr->Slave.AveTimeOffset = DiffTimeInNS;
			pChUsr->Slave.AveMaxOffset = DiffTimeInNS;
			pChUsr->Slave.AveMinOffset = DiffTimeInNS;
			pChUsr->Slave.AveSumOffset = DiffTimeInNS;
			pChUsr->Slave.AveSumCountLast = pChUsr->Slave.AveSumCount;
			pChUsr->Slave.AveSumCount = 1;

			/* 160511lrg: start slave timer
			*/
			gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
			pChUsr->Statistics.FrameOk++;

			/* Store actual master time lrg: better Ref time?
			*/
			pChUsr->Slave.AdjustSeconds = OrgMasterTimeInS;
			pChUsr->Slave.AdjustNanos   = (LSA_UINT32) OrgMasterTimeInNS;

			/* Update diagnosis data and send indication (if offset has changed)
			*/
			gsy_DiagUserInd(GSY_DIAG_SOURCE_OFFSET, pChUsr->pChSys, pChUsr->Slave.pDrift, 
    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
				pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
				pChUsr->Slave.AveTimeOffset		   /* LSA_INT32: Offset */, 
				0                                  /* LSA_INT32: AdjustInterval */, 
				pChUsr->Slave.Act.SeqId            /* LSA_INT32: UserValue1 */, 
				1 	    						   /* LSA_INT32: UserValue2 */, 
				0                          		   /* LSA_INT32: SetTimeHigh */, 
				0                                  /* LSA_INT32: SetTimeLow */, 
				pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */
				);
		    if ((DiffTimeInNS > (LSA_INT32)pChUsr->Slave.WindowSize)
		    ||  (-DiffTimeInNS > (LSA_INT32)pChUsr->Slave.WindowSize))
			{
				GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAction(%02x) Offset=%dns SeqId=%u: slave state change FIRST->OUT",
						pChUsr->SyncId, DiffTimeInNS, pChUsr->Slave.SeqId);
				pChUsr->Slave.State = GSY_SLAVE_STATE_OUT;
			}
			else
			{
				GSY_SYNC_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_SlaveAction(%02x) Offset=%dns SeqId=%u: slave state change FIRST->SYNC",
						pChUsr->SyncId, DiffTimeInNS, pChUsr->Slave.SeqId);
				pChUsr->Slave.State = GSY_SLAVE_STATE_SYNC;
			}

			/* Reset controller
			*/
			RetVal = gsy_SyncReset(pChUsr->Slave.pDrift->AveInterval, pChUsr->SyncId, pChUsr);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_SlaveAction() pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
								pChUsr, pChUsr->SyncId, RetVal);
			}
			/* 160511lrg: adjust synchronously
			*/
			gsy_SlaveAdjust(pChUsr);	
		}
		else
		{
			/* Build offset average value if not yet exists or the actual offset is not too large
			*/
			if ((pChUsr->Slave.AveTimeOffset == 0)
			||  (pChUsr->Slave.AveSumCount < GSY_SYNC_IGNORE_DIST_CNT)
			||  ((DiffTimeInNS >= pChUsr->Slave.AveTimeOffset - GSY_SYNC_MAX_DIST) 
			 &&  (DiffTimeInNS <= pChUsr->Slave.AveTimeOffset + GSY_SYNC_MAX_DIST)))
			{

				/* If the count of offset values ha reached the maximum... 
				*/
				if ((GSY_SYNC_MAX_COUNT != 0)
				&&  (GSY_SYNC_MAX_COUNT <= pChUsr->Slave.AveSumCount))       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to True, define-based behaviour, HM 20.05.2016
				{
					/* ...start a new average build
					*/
					pChUsr->Slave.AveSumOffset = DiffTimeInNS;
					pChUsr->Slave.AveSumCount = 1;
					pChUsr->Slave.AveTimeOffset = DiffTimeInNS;
				}
				else
				{
					/* Add actual offset to the sum and calculate average value
					*/
					pChUsr->Slave.AveSumOffset += DiffTimeInNS;
					pChUsr->Slave.AveSumCount++;
					pChUsr->Slave.AveTimeOffset = pChUsr->Slave.AveSumOffset / pChUsr->Slave.AveSumCount;
				}

				/* Store minimum and maximum of offset
				*/
				if ((pChUsr->Slave.AveMaxOffset == 0) || (pChUsr->Slave.AveMaxOffset < DiffTimeInNS))
					pChUsr->Slave.AveMaxOffset = DiffTimeInNS;
				if ((pChUsr->Slave.AveMinOffset == 0) || (pChUsr->Slave.AveMinOffset > DiffTimeInNS))
					pChUsr->Slave.AveMinOffset = DiffTimeInNS;

				/* 160511lrg: start slave timer
				*/
				gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
				pChUsr->Statistics.FrameOk++;

				/* Store actual master time lrg: better Ref time?
				pChUsr->Slave.AdjustSeconds = RefCycleBeginMasterTimeInS;
				pChUsr->Slave.AdjustNanos   = (LSA_UINT32) RefCycleBeginMasterTimeInNS;
				*/
				pChUsr->Slave.AdjustSeconds = OrgMasterTimeInS;
				pChUsr->Slave.AdjustNanos   = (LSA_UINT32) OrgMasterTimeInNS;

				/* Update diagnosis data and send indication (if offset has changed)
				*/
				gsy_DiagUserInd(GSY_DIAG_SOURCE_OFFSET, pChUsr->pChSys, pChUsr->Slave.pDrift, 
	    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
					pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
					pChUsr->Slave.AveTimeOffset		   /* LSA_INT32: Offset */, 
					0                                  /* LSA_INT32: AdjustInterval */, 
					pChUsr->Slave.Act.SeqId            /* LSA_INT32: UserValue1 */, 
					1 	    						   /* LSA_INT32: UserValue2 */, 
					0                          		   /* LSA_INT32: SetTimeHigh */, 
					0                                  /* LSA_INT32: SetTimeLow */, 
					pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */
					);
			}
			else
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_SlaveAction() SyncId=0x%02x: Invalid time difference=%d (offset=%d)",
						pChUsr->SyncId, DiffTimeInNS, pChUsr->Slave.AveTimeOffset);
				pChUsr->Statistics.FrameDataInvalid++;
			}
			/* 160511lrg: adjust synchronously
			*/
			gsy_SlaveAdjust(pChUsr);	
		}

		GSY_SYNC_TRACE_08(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--+ gsy_SlaveAction(%02x) ActOffset=%d AveOffset=%d AveSum=%d SumCount=%u AveMin=%d AveMax=%d State=%u",
			pChUsr->SyncId, DiffTimeInNS, pChUsr->Slave.AveTimeOffset, pChUsr->Slave.AveSumOffset, 
			pChUsr->Slave.AveSumCount, pChUsr->Slave.AveMinOffset, pChUsr->Slave.AveMaxOffset, pChUsr->Slave.State);

	}
	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveAction() State=%u AveTimeOffset=%d DiffNanos=%d",
					pChUsr->Slave.State, pChUsr->Slave.AveTimeOffset, DiffTimeInNS);
}

/*****************************************************************************/
/* Internal function: gsy_SlaveSeqIdCheck()                                  */
/* Check SequenceId of a received SyncFrame                                  */
/*****************************************************************************/
LSA_BOOL GSY_LOCAL_FCT_ATTR gsy_SlaveSeqIdCheck(
GSY_CH_USR_PTR			pChUsr,
LSA_UINT16				SeqId)
{
	LSA_UINT16	SeqDist;
	LSA_BOOL	RetVal = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveSeqIdCheck() pChUsr=%X SeqId=%d LastSeqId=%d",
					pChUsr, SeqId, pChUsr->Slave.SeqId);

	if (SeqId < pChUsr->Slave.SeqId)
	{
		SeqDist = SeqId + 0xffff + 1 - pChUsr->Slave.SeqId;
	}
	else
	{
		SeqDist = SeqId - pChUsr->Slave.SeqId;
	}

	if ((SeqDist == 0) || (GSY_SEQID_MAX_DIST < SeqDist))
	{
		RetVal = LSA_FALSE;
		pChUsr->Statistics.FrameWrongSequenceNr++;
	}

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveSeqIdCheck() pChUsr=%X RetVal=%u FrameWrongSequenceNr=%u",
					pChUsr, RetVal, pChUsr->Statistics.FrameWrongSequenceNr);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_SlaveIndSync()                              */
/* Get a received SyncFrame for slave and master                             */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveIndSync(
GSY_CH_USR_PTR			pChUsr,
GSY_PTCP_DATA		*	pPdu,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId,
LSA_UINT32				TimeStamp)
{
	LSA_BOOL NewMaster = LSA_FALSE, DelayOk;

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveIndSync() pChUsr=%X pPdu=%X TimeStamp=%u Port=%d",
					pChUsr, pPdu, TimeStamp, PortId);

	/* If the drift to this master is calculated
	 * and this SyncFrame is the first from this master
	 * or the SequenceID is valid...
	*/
	pChUsr->Slave.SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
	if (!LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pDrift, LSA_NULL)
	&& ((GSY_SLAVE_STATE_WAIT == pChUsr->Slave.State)
	||  (GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State)	//170706lrg002
	||  (GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))	//270907lrg005: Takeover
	||  ((GSY_MACADDR_CMP(pTLVInfo->Param.Sync.Master.MacAddr, pChUsr->Slave.MasterAddr)) 
	 &&  gsy_SlaveSeqIdCheck(pChUsr, pTLVInfo->Param.Sync.SequenceID))))
	{
		/* Store receive port and values from SyncFrame in slave structure
		 * 070108lrg002: Errortrace
		*/
		if (pChUsr->Slave.WaitFU)
		{
			/* Missing FollowUp of last received SyncFrame
			*/
			pChUsr->Slave.FuTimeout++;
			GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_SlaveIndSync() pChUsr=%x Missing FU SeqId=%u from Port=%u FuTimeout=%u",
						pChUsr, pChUsr->Slave.SeqId, pChUsr->Slave.SyncPortId, pChUsr->Slave.FuTimeout);
			if (pChUsr->Slave.FuTimeout == pChUsr->Slave.SyncTimeout)
			{
				/* 1066330: Simulate timeout for FU frame 
				*/
				pChUsr->Slave.SumTimeout = 1; 
				gsy_SlaveTimer(&pChUsr->Slave.SyncTimer);
				pChUsr->Slave.FuTimeout = 0; 
			}
		}
		pChUsr->Slave.SeqId = pTLVInfo->Param.Sync.SequenceID;
		pChUsr->Slave.WaitFU = pTLVInfo->Param.Sync.FUFlag;
		pChUsr->Slave.pDrift = pTLVInfo->pDrift;
		pChUsr->Slave.SyncPortId = PortId;
		pChUsr->Slave.Act.SeqId = pTLVInfo->Param.Sync.SequenceID;
		pChUsr->Slave.Act.Timestamp = TimeStamp;
		pChUsr->Slave.Act.IndTimestamp = GSY_GET_TIMESTAMP(pChUsr->pChSys->pSys);
		pChUsr->Slave.Act.OrgEpoch = GSY_SWAP16(pPdu->Sync.TLV.Time.EpochNumber.Word);
		pChUsr->Slave.Act.OrgSeconds = GSY_SWAP32(pPdu->Sync.TLV.Time.Seconds.Dword);
		pChUsr->Slave.Act.OrgNanos = GSY_SWAP32(pPdu->Sync.TLV.Time.NanoSeconds.Dword);
		pChUsr->Slave.CurrentUTCOffset = GSY_SWAP16(pPdu->Sync.TLV.TimeExt.CurrentUTCOffset.Word); 
		pChUsr->Slave.TimeFlags = GSY_SWAP16(pPdu->Sync.TLV.TimeExt.Flags.Word); 

		if ((GSY_SLAVE_STATE_WAIT == pChUsr->Slave.State)
		||  (GSY_SLAVE_STATE_FIRST == pChUsr->Slave.State)	//170706lrg002
		||  GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))	//270907lrg005: Takeover 131107lrg001: state check
		{
			/* Store constants only from first SyncFrame
			*/
			NewMaster = LSA_TRUE;
			pChUsr->Slave.MasterAddr = pTLVInfo->Param.Sync.Master.MacAddr;
			pChUsr->Slave.ClockVariance = pTLVInfo->Param.Sync.Master.Variance; 
			pChUsr->Slave.ClockAccuracy = pTLVInfo->Param.Sync.Master.Accuracy;
			pChUsr->Slave.ClockClass = pTLVInfo->Param.Sync.Master.Class;

			/* 160511lrg: start slave timer
			pChUsr->Slave.SumTimeout = 0;	// 110510lrg001: Reset timeout count
			*/
			gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
		}
		/* Move frame delay to slave structure
		*/
		DelayOk = gsy_DelayGet(pChUsr->pChSys, &pChUsr->Slave.Act, pTLVInfo, PortId);

		GSY_SYNC_TRACE_07(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_SlaveIndSync(%02x) SeqId=%u Delay10NS=%u Delay1NS=%u DelayNS=%u: DelaySeconds=%u DelayNanos=%u",
						pChUsr->SyncId, pChUsr->Slave.SeqId, pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS, 
						pTLVInfo->Param.Sync.DelayNS, pChUsr->Slave.Act.DelaySeconds, pChUsr->Slave.Act.DelayNanos);

		if (!DelayOk)
		{
			GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveIndSync() pChUsr=%X Invalid delay: Delay10NS=%u Delay1NS=%u DelayNS=%u",
							pChUsr, pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS, pTLVInfo->Param.Sync.DelayNS);
		}
		else if ((!pChUsr->Slave.WaitFU)
		&&  (GSY_SLAVE_STATE_WAIT_SET != pChUsr->Slave.State)
		&&  (GSY_MASTER_STATE_PRIMARY != pChUsr->Master.State))
		{
			/* Calculate offset if no FollowUp follows
			*/
			gsy_SlaveAction(pChUsr);
		}
	}

	if (NewMaster)
	{
		/* 221106lrg001: Update diagnosis data and send Indication
		*/
		gsy_DiagUserInd(GSY_DIAG_SOURCE_MASTER, pChUsr->pChSys, pChUsr->Slave.pDrift, 
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

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveIndSync() pChUsr=%X SyncId=0x%02x SeqId=%u",
					pChUsr, pChUsr->SyncId, pTLVInfo->Param.Sync.SequenceID);
}

/*****************************************************************************/
/* Internal access function: gsy_SlaveIndFu()                                */
/* Get a received FollowUpFrame for slave                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SlaveIndFu(
GSY_CH_USR_PTR			pChUsr,
GSY_PTCP_DATA		*	pPdu,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId)
{
	LSA_BOOL			FuOk = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveIndFu() pChUsr=%X pPdu=%X Port=%d", pChUsr, pPdu, PortId);

	/* The receive port of the FollowUp must be identical to the port of the SyncFrame
	*/
	if ((PortId == pChUsr->Slave.SyncPortId)
	&&  (pChUsr->Slave.WaitFU))
	{
		/* Check Subdomain, Masteraddress and SequenceID
		*/
		if (GSY_SUBDOMAIN_CMP(pPdu->SyncFu.Subdomain.SubdomainUUID, pChUsr->SyncData.SubdomainID))
		{
			pChUsr->Slave.SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
			if ((pTLVInfo->Param.FollowUp.SequenceID == pChUsr->Slave.SeqId)
			&&	(GSY_MACADDR_CMP(pTLVInfo->Param.FollowUp.Master.MacAddr, pChUsr->Slave.MasterAddr)
			  || GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr)))	//AP01512559: Allow sync by FU after Takeover
			{
				if  (GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))
				{
					/* Restore MAC address and start slave timer after takeover
					*/
					pChUsr->Slave.MasterAddr = pTLVInfo->Param.FollowUp.Master.MacAddr;
					gsy_SlaveTimerStart(pChUsr, LSA_FALSE);
				}
				/* Update Delay in slave structure
				*/
				FuOk = gsy_DelayGet(pChUsr->pChSys, &pChUsr->Slave.Act, pTLVInfo, PortId);

				GSY_SYNC_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_SlaveIndFu(%02x) SeqId=%u DelayNS=%d: DelaySeconds=%u DelayNanos=%u",
								pChUsr->SyncId, pChUsr->Slave.SeqId, pTLVInfo->Param.FollowUp.DelayNS, 
								pChUsr->Slave.Act.DelaySeconds, pChUsr->Slave.Act.DelayNanos);

				/* Call slave offset calculation
				*/
				if (FuOk)
				{
					if ((GSY_SLAVE_STATE_WAIT_SET != pChUsr->Slave.State)
					&&  (GSY_MASTER_STATE_PRIMARY != pChUsr->Master.State))
						gsy_SlaveAction(pChUsr);
				}
				else
				{
					GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_SlaveIndFu() pChUsr=%X Invalid delay: DelayNS=%d",
									pChUsr, pTLVInfo->Param.FollowUp.DelayNS);
				}
				pChUsr->Slave.WaitFU = LSA_FALSE;
				pChUsr->Slave.FuTimeout = 0;
			}
			else	//070108lrg002: Errortrace
			{
				GSY_ERROR_TRACE_03(pChUsr->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_SlaveIndFu() pChUsr=%X Invalid FU: SeqId=%u  ActSeqId=%u",
								pChUsr, pTLVInfo->Param.FollowUp.SequenceID, pChUsr->Slave.SeqId);
			}
		}
		else
		{
			pChUsr->Slave.SyncError = GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID; 
			GSY_SYNC_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_SlaveIndFu(%02x) SubomainUUID mismatch: SeqId=%u",
							pChUsr->SyncId, pTLVInfo->Param.FollowUp.SequenceID);
		}
	}
	else
	{
		// 070108lrg002: Errortrace	071010lrg001: WARN -> NOTE (MRP)
		GSY_ERROR_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_SlaveIndFu() pChUsr=%X ignoring FU SeqId=%u WaitFU=%u from Port=%u",
						pChUsr, pTLVInfo->Param.FollowUp.SequenceID, pChUsr->Slave.WaitFU, PortId);
	}

	GSY_FUNCTION_TRACE_04(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveIndFu() pChUsr=%X SyncId=0x%02x SeqId=%u FuOk=%d",
					pChUsr, pChUsr->SyncId, pTLVInfo->Param.FollowUp.SequenceID, FuOk);
}

/*=============================================================================
 * function name:  gsy_SyncReset
 *
 * function:       Function call to Reset state variable(s) of Sync-Controller
 *                 and set OCF interval in hardware to RCF interval
 *
 * parameters:     RateInterval: RCF interval
 *                 SyncId:       0 = Cycle, 1 = Time
 *                 pChUsr:       Pointer to user channel
 *
 * return value: LSA_RET_OK         ok
 *               LSA_RET_ERR_PARAM  wrong pSys or SyncId
 *===========================================================================*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncReset(
LSA_INT32                     RateInterval,
LSA_UINT8                     SyncId,
GSY_CH_USR_PTR                pChUsr)
{
	LSA_UINT16	RetVal;

	RetVal = gsy_ClpReset(RateInterval, SyncId, &pChUsr->Slave.Ctrl);
	if (LSA_RET_OK == RetVal)
	{
		/* Set adjust interval in hardware
		*/
		(LSA_VOID)GSY_SYNC_ADJUST(RateInterval, SyncId, pChUsr->pSys);
	}
	return(RetVal);
}

/*=============================================================================
 * function name:  gsy_SyncAlgorithm
 *
 * function:       Function call to calculate control variable (sync controller)
 *
 * parameters:     DeltaTimeSec      Seconds since last call
 *                                   (currently only zero supported)
 *                 DeltaTimeNs       Nanoseconds since last call
 *                                   (currently only values < 32ms supported)
 *				   OffsetNs          Current Offset in Nanoseconds to master clock
 *                                   (negativ: local clock is too slow)
 *                 pAdjustIntervalNs Pointer to return correction interval
 *                                   (Interval in ns that has to be corrected
 *                                    by +1/0/-1 Nanosecond)
 *                 pProportionalTerm Pointer to return the P-Part of the controller
 *                                    for diagnosis
 *                 pIntegralTerm     Pointer to return the I-Part of the controller
 *                                    for diagnosis
 *                 SyncId            Identifies structure in data union
 *                 pCtrl             Pointer to working data union
 *
 * return value: LSA_RET_OK           ok
 *               LSA_RET_ERR_PARAM    wrong DeltaTime
 *               LSA_RET_ERR_SYS_PATH wrong NicId, SyncId or
 *               LSA_RET_ERR_SEQUENCE not initialized by gsy_ClpReset()
 *===========================================================================*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgorithm(
LSA_UINT32                    TimeSec,
LSA_UINT32                    TimeNs,
LSA_INT32                     OffsetNs,
LSA_INT32                     *pAdjustIntervalNs,
LSA_INT64                     *pProportionalTerm,
LSA_INT64                     *pIntegralTerm,
LSA_UINT8                     SyncId,
GSY_CH_USR_PTR                pChUsr)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	if (GSY_SYNCID_CLOCK == SyncId)
	{
		/* Calculate adjust interval
		*/
		RetVal = gsy_SyncAlgoCycle(TimeSec, TimeNs,	OffsetNs, pAdjustIntervalNs, 
								pProportionalTerm, pIntegralTerm, SyncId, &pChUsr->Slave.Ctrl);
	}
	else
	{
		RetVal = LSA_RET_ERR_PARAM;
	}
	if (LSA_RET_OK == RetVal)
	{
		/* Set adjust interval in hardware
		*/
		(LSA_VOID)GSY_SYNC_ADJUST(*pAdjustIntervalNs, SyncId, pChUsr->pSys);
	}
	return(RetVal);
}
/*****************************************************************************/
/*  end of file GSY_MSM.C                                                    */
/*****************************************************************************/

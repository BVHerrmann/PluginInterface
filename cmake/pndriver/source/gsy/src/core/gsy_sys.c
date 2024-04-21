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
/*  F i l e               &F: gsy_sys.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System-Interface                                                         */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2004-11-29  P01.00.00.00_00.01.00.00  lrg  file creation                 */
/*  2005-04-04  P01.00.00.00_00.01.00.01  lrg  Umstellung auf TLVs           */
/*  2005-05-06  P03.05.00.00_00.03.00.01  lrg  Returnwerte fuer gsy_Timer-   */
/*              Clean() und gsy_ChSysClean() wegen Timer-RQB.                */
/*  2005-06-02  P03.06.00.00_00.03.00.01  lrg  Prototype for PT2             */
/*  2005-06-15  P03.06.00.00_00.03.02.01  lrg  OpenChannel trotz RxTxNanos=0 */
/*  2005-06-24  P03.06.00.00_00.03.02.02  lrg  neues Sync-Userinterface      */
/*  2005-12-22  P03.09.00.00_00.01.01.02 lrg pChUsr->RTC2Ctrl initialisieren */
/*  2006-03-29  P03.10.00.00_00.02.01.02 290306lrg: Alle konfigurierten      */
/*              SyncIds auf allen Ports fuer die Weiterleitung freigeben.    */
/*  2006-05-29  P03.11.00.00_00.04.01.02 lrg: Initialisierung:               */
/*              pChSys->Drift[i][ii].New.Used = LSA_TRUE                     */
/*              pChUsr->Event.NewPrm = LSA_FALSE;                            */
/*              pChUsr->Event.PortId = 0;                                    */
/*              SeqIdSnd und SeqIdCnf von .Port[] nach .Master verschoben    */
/*  2006-06-02  P03.11.00.00_00.04.03.02 lrg  neu: GSY_OLD_IF; "neue" Sync-  */
/*              Schnitts. entfernt                                           */
/*  2006-06-28  P03.12.00.00_00.02.01.02 lrg GSY_FILE_SYSTEM_EXTENSION()     */
/*  2006-07-07  P03.12.00.00_00.25.01.02 AsChannelDiag entfernt aus der PRM- */
/*              Indication-Datenstruktur GSY_PRM_IND_DATA_TYPE               */
/*  2006-07-25  P03.12.00.00_00.25.01.03 250706lrg001: TimerRQB-Semaphore    */
/*  2006-08-03  P03.12.00.00_00.25.04.01                                     */
/*              040806js001 : gsy_data immer nullen im Anlauf.               */
/*              040806js001 : Zugriff auf userid macht probleme. Unklar warum*/
/*              110806js001 : Änderung der SubdomainUUID bewirkt Neustart    */
/*                            der Synchronisation.                           */ 
/*              240806lrg001: pChUsr->SyncData in gsy_ChUsrAlloc() nullen    */
/*              240806lrg002; RQB-Zaehler fuer RTC2-RQBs: RTC2RblCount       */
/*              Master.SeqId = 0: in gsy_ChUsrAlloc()                        */
/*  2006-09-27  P03.12.00.00_00.25.04.02 lrg gsy_TimeoutRequest() aufgeraeumt*/
/*  2006-11-09  P03.12.00.00_00.30.04.02 GSY_PRM_DOMAIN_STATE_INI            */
/*  2007-02-12  P03.12.00.00_00.40.09.02 120207lrg001: QueueEvent            */
/*  2007-07-17  P04.00.00.00_00.30.01.00 170707lrg001: RTSync                */
/*              Doppelkommentar in gsy_TimeoutRequest() entfernt             */
/*              GSY_PRM_DOMAIN_STATE_INI -> GSY_PRM_PORT_STATE_INI           */
/*  2007-07-31  P04.00.00.00_00.30.01.02 310707js001 : RTSync                */
/*  2007-09-07  P04.00.00.00_00.30.05.02 070907lrg001: Master.StartupTimer   */
/*  2007-09-28  P04.00.00.00_00.30.06.02 280907lrg001: PrmTimer entfernt     */
/*              #define GSY_OLD_IF entfernt                                  */ 
/*  2007-10-16  P04.00.00.00_00.40.01.02 AP00526517: RateValid,PrimaryMaster */
/*  2008-01-18  P04.00.00.00_00.70.01.03 180108lrg001: Rate parallel         */
/*              berechnen                                                    */
/*  2008-04-17  P04.00.00.00_00.80.01.02 170408lrg002: RcvSyncPrio,LocalPrio */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-06-24  P04.01.00.00_00.01.01.02 240608lrg001: ArtsRQ AP00536908     */
/*              DelaySet zwischen Senderequest und Confirmation verzögern    */
/*  2008-07-25  P04.01.00.00_00.01.02.03 250708lrg001: InfoSync              */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-15-06  P04.02.00.00_00.01.01.02 060509lrg001: Cast                  */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-03-23  P05.00.00.00_00.01.04.02 230310lrg001: init Diag.SyncId      */
/*  2010-07-20  P05.00.00.00_00.01.06.02 lrg: AP00999110 trace fcts removed  */
/*              German comments translated to english                        */
/*  2010-10-08  P05.00.00.00_00.01.08.03 230910lrg001: PDSyncPLL record      */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-05-16  P05.00.00.00_00.05.21.02 160511lrg: AP01183082 gsy_SlaveTimer*/
/*  2011-05-27  P05.00.00.00_00.05.21.02 270511lrg: Trace (GSY_PROC_xx)      */
/*  2011-05-31  P05.00.00.00_00.05.21.02 310511lrg: AP01178714: SetWaitCount */
/*  2011-08-01  S05.01.00.00_00.03.07.01 010811lrg: LSA_RET_OK_TIMER_RESTA.. */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: cast                     */
/*  2012-05-31  P05.00.00.00_00.05.97.02 AP01360545 Timer trace              */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01482839: Warnings bei KM         */
/*              Generierung: (LSA_UINT32)pTimer->Id                          */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*  2015-07-17  V06.00, Inc07, Feature 1204141, Task 1366974                 */
/*              Timesync Diagnosen richtig an CM geben                       */
/*  2016-01-13  V06.00, Inc13, RQ 1558755:fatal error during closing of EB200*/
/*              gsy_TimerClean now always calls GSY_TIMER_STOP               */
/*  2016-05-02  V06.00-V07.00, RQ 1684006: initialize PendingSet             */
/*  2016-06-28  V07.00, Task 1737674: Remove _PRM_INDICATION::ChannelNumber  */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	1		/* GSY_MODULE_ID_SYS */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)


/*****************************************************************************/
/* External access function: gsy_init()                                      */
/* Initialise data struktures                                                */
/*****************************************************************************/
LSA_UINT16  GSY_SYSTEM_IN_FCT_ATTR  gsy_init(
LSA_VOID)
{
	LSA_HANDLE_TYPE     ChId;
	GSY_CH_USR_PTR  	pChUsr;
	GSY_CH_ANNO_PTR  	pChAnno;
	LSA_INT				Idx;
	LSA_UINT16          RetVal = LSA_RET_OK;
	LSA_UINT32			Protocols = GSY_SYNC_ID_SUPPORTED;
	
	GSY_MEMSET_LOCAL(&gsy_data, 0x00, sizeof(gsy_data) ); /* 040806js001 */

	GSY_SYSTEM_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE,"--> gsy_init() &gsy_data=%X",
					&gsy_data);

#ifdef GSY_LOCAL_MEM_STATIC

    /* Use static internal data
     */
    gsy_data.pChSysList = gsy_data.ChSysList;
    gsy_data.pChAnnoList = gsy_data.ChAnnoList;
    gsy_data.pChUsrList = gsy_data.ChUsrList;
#else
#ifdef GSY_MESSAGE
    /* TBD: Use dynamic internal data
     */
#endif /* GSY_MESSAGE */
    RetVal = LSA_RET_ERR_RESOURCE;
#endif

    /* Initialise channel lists:
     */
    if (RetVal == LSA_RET_OK)       //lint !e774 Boolean within 'if' always evaluates to True, it depends on the define GSY_LOCAL_MEM_STATIC, HM 20.05.2016
    {
        /* System channel list:
         */
        for (ChId = 0; ChId < GSY_MAX_SYS_CHANNELS; ChId++)
		{
			/* Initialise system channel structures 
			 * Handles: 1 to GSY_MAX_SYS_CHANNELS
			*/
			gsy_data.ChSysList[ChId].State = GSY_CHA_STATE_FREE;
			gsy_data.ChSysList[ChId].Handle = ChId + 1;
			gsy_data.ChSysList[ChId].pCDB = LSA_NULL;
			gsy_data.ChSysList[ChId].pCbf = 0;
			gsy_data.ChSysList[ChId].pChAnno = LSA_NULL;
			gsy_data.ChSysList[ChId].UserCount = 0;
			gsy_data.ChSysList[ChId].PortCount = 0;
			gsy_data.ChSysList[ChId].QueueUpper.pFirst = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueUpper.pLast = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueUpper.Count = 0;
			gsy_data.ChSysList[ChId].QueueFwdCtrl.pFirst = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueFwdCtrl.pLast = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueFwdCtrl.Count = 0;
			gsy_data.ChSysList[ChId].QueueLowerSend.pFirst = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerSend.pLast = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerSend.Count = 0;
			gsy_data.ChSysList[ChId].QueueLowerRecv.pFirst = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerRecv.pLast = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerRecv.Count = 0;
			gsy_data.ChSysList[ChId].QueueLowerCtrl.pFirst = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerCtrl.pLast = LSA_NULL;
			gsy_data.ChSysList[ChId].QueueLowerCtrl.Count = 0;
			for (Idx=0; Idx < GSY_MAX_PROTOCOLS; Idx++)
			{
				gsy_data.ChSysList[ChId].QueueDiag[Idx].pFirst = LSA_NULL;
				gsy_data.ChSysList[ChId].QueueDiag[Idx].pLast = LSA_NULL;
				gsy_data.ChSysList[ChId].QueueDiag[Idx].Count = 0;
			}
		}

        /* Announce channel list:
         */
        for (ChId = 0; ChId < GSY_MAX_SYS_CHANNELS; ChId++)
        {
			pChAnno = (GSY_CH_ANNO_PTR)(gsy_data.pChAnnoList + ChId);

			/* Initialise announce channel structures 
			 * Handles: GSY_MAX_SYS_CHANNELS + 1 to
			 *          2*GSY_MAX_SYS_CHANNELS
			*/
			pChAnno->State = GSY_CHA_STATE_FREE;
			pChAnno->Handle = ChId + GSY_MAX_SYS_CHANNELS + 1;
			pChAnno->pCDB = LSA_NULL;
			pChAnno->pCbf = 0;
			pChAnno->pChSys = LSA_NULL;
			pChAnno->QueueUpper.pFirst = LSA_NULL;
			pChAnno->QueueUpper.pLast = LSA_NULL;
			pChAnno->QueueUpper.Count = 0;
			pChAnno->QueueLower.pFirst = LSA_NULL;
			pChAnno->QueueLower.pLast = LSA_NULL;
			pChAnno->QueueLower.Count = 0;
        }

        /* User channel list:
         */
        for (ChId = 0; ChId < GSY_MAX_USR_CHANNELS; ChId++)
        {
			pChUsr = (GSY_CH_USR_PTR)(gsy_data.pChUsrList + ChId);

			/* Initialise user channel structures
			 * Handles: 2 * GSY_MAX_SYS_CHANNELS + 1 to
			 *          2 * GSY_MAX_SYS_CHANNELS + GSY_MAX_USR_CHANNELS
			*/
			pChUsr->State = GSY_CHA_STATE_FREE;
			pChUsr->Handle = ChId + (2 * GSY_MAX_SYS_CHANNELS) + 1;
			pChUsr->pCDB = LSA_NULL;
			pChUsr->pCbf = 0;
			pChUsr->pChSys = LSA_NULL;
			pChUsr->QueueUpper.pFirst = LSA_NULL;
			pChUsr->QueueUpper.pLast  = LSA_NULL;
			pChUsr->QueueUpper.Count  = 0;
			pChUsr->QueueRcv.pFirst   = LSA_NULL;
			pChUsr->QueueRcv.pLast    = LSA_NULL;
			pChUsr->QueueRcv.Count    = 0;
			pChUsr->QueueEvent.pFirst = LSA_NULL;
			pChUsr->QueueEvent.pLast  = LSA_NULL;
			pChUsr->QueueEvent.Count  = 0;
			pChUsr->QueuePrm.pFirst   = LSA_NULL;
			pChUsr->QueuePrm.pLast    = LSA_NULL;
			pChUsr->QueuePrm.Count    = 0;
			pChUsr->QueueWait.pFirst  = LSA_NULL;
			pChUsr->QueueWait.pLast   = LSA_NULL;
			pChUsr->QueueWait.Count   = 0;
			pChUsr->Master.QueueSync.pFirst = LSA_NULL;
			pChUsr->Master.QueueSync.pLast  = LSA_NULL;
			pChUsr->Master.QueueSync.Count  = 0;
			pChUsr->Master.QueueFu.pFirst = LSA_NULL;
			pChUsr->Master.QueueFu.pLast  = LSA_NULL;
			pChUsr->Master.QueueFu.Count  = 0;
			pChUsr->Master.QueueAnnounce.pFirst   = LSA_NULL;
			pChUsr->Master.QueueAnnounce.pLast    = LSA_NULL;
			pChUsr->Master.QueueAnnounce.Count    = 0;
        }
    }
	
	RetVal = gsy_FwdInit(Protocols);

	GSY_SYSTEM_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE,"<-- gsy_init() &gsy_data=%X RetVal=0x%x",
					&gsy_data, RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* External access function: gsy_undo_init()                                 */
/* Undo init                                                                 */
/*****************************************************************************/
LSA_UINT16  GSY_SYSTEM_IN_FCT_ATTR  gsy_undo_init(
LSA_VOID)
{
	LSA_UINT16      RetVal;

	GSY_SYSTEM_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE,"--> gsy_undo_init() &gsy_data=%X",
					&gsy_data);

#ifdef GSY_LOCAL_MEM_STATIC
    /* Static internal data - nothing to do.
     */
#else
#ifdef GSY_MESSAGE
    /* TBD: free dynamic internal data
     */
#endif /* GSY_MESSAGE */
#endif
    /* Finish Forwarding
    */
	RetVal = gsy_FwdExit();

	GSY_SYSTEM_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_NOTE,"<-- gsy_undo_init() &gsy_data=%X RetVal=0x%x",
					&gsy_data, RetVal);
    return(RetVal);
}

/*****************************************************************************/
/* Function: gsy_timeout_int()                                               */
/* Internal function for all GSY timers                                      */
/*****************************************************************************/
static LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_timeout_int(
//LSA_TIMER_ID_TYPE TimerId,
LSA_USER_ID_TYPE  UserId)
{
#ifdef GSY_TIMER_CHECK	
	LSA_UINT32		DiffTicks, Timestamp;
#endif
	LSA_UINT16		RetVal;
	GSY_TIMER_PTR	pTimer = (GSY_TIMER_PTR)UserId.void_ptr;
	LSA_BOOL		DropTimer = LSA_FALSE;

	LSA_UINT32		TraceIdx = 0;

    if(!LSA_HOST_PTR_ARE_EQUAL(pTimer->pChUsr, LSA_NULL))
	{
		TraceIdx = pTimer->pChUsr->TraceIdx;
	}
    else if(!LSA_HOST_PTR_ARE_EQUAL(pTimer->pChSys, LSA_NULL))
	{
		TraceIdx = pTimer->pChSys->TraceIdx;
	}

	GSY_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_timeout_int() pTimer=%X TimerId=%d", pTimer, pTimer->Id);

#ifdef GSY_TIMER_CHECK	
	/* 261006lrg001: drop cyclic timer
	 * if time difference to last timer is less than 1/3 of the regular difference
	*/
	if (LSA_TIMER_TYPE_CYCLIC == pTimer->Type)
	{
		Timestamp = GSY_GET_TIMESTAMP(pTimer->pChSys->pSys);
		DiffTicks = Timestamp - pTimer->Timestamp;
		if (DiffTicks < (pTimer->TimeTicks)/3)
		{
			DropTimer = LSA_TRUE;
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_timeout_int() pTimer=%X Id=%d DiffTicks=%u dropping timer",
							pTimer, pTimer->Id, pTimer->Id);
		}
	}
#endif
	pTimer->Waiting = LSA_FALSE;		//160511lrg
	if (pTimer->Restart)
	{
		/* 160511lrg: Restart elapsed but working LSA timer
		*/
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_timeout_int() pTimer=%X Id=%u restart time=%u",
							pTimer, pTimer->Id, pTimer->Time);
		pTimer->Restart = LSA_FALSE;
		pTimer->Running = LSA_TRUE;
		GSY_START_TIMER(&RetVal, pTimer->Id, UserId, pTimer->Time); 
		if ((LSA_RET_OK != RetVal)
		&&  (LSA_RET_OK_TIMER_RESTARTED != RetVal))		//010811lrg: for slave timer restart
		{
			pTimer->Running = LSA_FALSE;
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_TIMER, pTimer, (LSA_UINT32)pTimer->Id);
		}
	}
	else if (!DropTimer)       //lint !e774 Boolean within 'if' always evaluates to True, it depends on define GSY_TIMER_CHECK, HM 20.05.2016
	{
		/* Call specific timeout procedure
		*/
		pTimer->pTimeoutProc(pTimer);
	}

	GSY_FUNCTION_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_timeout_int() pTimer=%X", UserId.void_ptr);
}

/*****************************************************************************/
/* External access function: gsy_timeout()                                   */
/* Timer elapsed: if this function is called in interrupt context            */
/* (#define GSY_DO_TIMEOUT_EXTERN), a change to task context is done by      */
/* GSY_DO_TIMER_REQUEST(), sending a upper RQB to GSY.                       */
/* Else the timeout is handeled immediately.                                 */
/*****************************************************************************/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_timeout(
LSA_TIMER_ID_TYPE	TimerId,
LSA_USER_ID_TYPE	UserId)
{
	GSY_TIMER_PTR	pTimer = (GSY_TIMER_PTR)UserId.void_ptr;

    if(!LSA_HOST_PTR_ARE_EQUAL(pTimer, LSA_NULL))
	{
		if (pTimer->InUse)
		{
#ifdef GSY_DO_TIMEOUT_EXTERN		//AP01360545
			GSY_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW,"--> gsy_timeout() pTimer=%X TimerId=%u RbTimerPtr=%X",
							pTimer, pTimer->Id, pTimer->RbTimerPtr);
#else
			GSY_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW,"--> gsy_timeout() pTimer=%X TimerId=%u",
							pTimer, pTimer->Id);
#endif

			if (LSA_TIMER_TYPE_ONE_SHOT == pTimer->Type)
			{
				pTimer->Running = LSA_FALSE;
			}
			pTimer->Waiting = LSA_TRUE;		//160511lrg

#ifdef GSY_DO_TIMEOUT_EXTERN
			{
				GSY_UPPER_RQB_PTR_TYPE	pRb;
			    if (!pTimer->RbInUse && !LSA_HOST_PTR_ARE_EQUAL(pTimer->RbTimerPtr, LSA_NULL))
				{
					/* 250706lrg001: lock Timer RQB
					*/
					pTimer->RbInUse = LSA_TRUE;
					pRb = pTimer->RbTimerPtr;
					pTimer->RbTimerPtr = LSA_NULL;
//					pRb->args.Timer.TimerId = pTimer->Id;
					pRb->args.Timer.TimerId = TimerId;
					pRb->args.Timer.UserId = UserId;
					GSY_DO_TIMER_REQUEST(pRb);
				}
				else
				{
					/* Timer RQB locked
					*/
					GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_timeout() pTimer=%X no local RQB (RbInUse=%u)",
									pTimer, pTimer->RbInUse);
				}
			}
#else
		    GSY_ENTER();
			gsy_timeout_int(UserId);
    		GSY_EXIT();
#endif
			GSY_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW,"<-- gsy_timeout() pTimer=%X",
						UserId.void_ptr);
		}
		else
		{
			GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_timeout() pTimer=%X TimerId=%d not in use",
							pTimer, pTimer->Id);
		}
	}
	else
	{
	    GSY_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_timeout() invalid UserId=%u", 
			UserId.uvar32);
	}
	LSA_UNUSED_ARG(TimerId);
}

#ifdef GSY_DO_TIMEOUT_EXTERN
/*****************************************************************************/
/* Internal access function: gsy_TimeoutRequest()                            */
/* Timer RQB arrived: pass timer to internal timeout function                */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_TimeoutRequest(
GSY_UPPER_RQB_PTR_TYPE  pRbu)
{
    LSA_UINT16			RetVal = 0;
    LSA_USER_ID_TYPE	UserId = pRbu->args.Timer.UserId;
	GSY_TIMER_PTR		pTimer = (GSY_TIMER_PTR)UserId.void_ptr;

    GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_TimeoutRequest() pRbu=%X pTimer=%X",
    				pRbu, pTimer);

    if(!LSA_HOST_PTR_ARE_EQUAL(pTimer, LSA_NULL))
	{
	    if (pTimer->RbInUse && LSA_HOST_PTR_ARE_EQUAL(pTimer->RbTimerPtr, LSA_NULL))
		{
			/* Free locked Timer RQB for usage and call internal timeout function
			 * 250706lrg001: additionally check semaphore
			*/
			pTimer->RbTimerPtr = pRbu;
			pTimer->RbInUse = LSA_FALSE;
		    gsy_timeout_int(UserId);
		}
	}
	else
	{
	    GSY_FREE_UPPER_RQB_LOCAL(&RetVal, pRbu);

	    GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_TimeoutRequest() pRbu=%X: invalid UserId=0x%x", pRbu, UserId.uvar32);
	}

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_TimeoutRequest() pRbu=%X", pRbu);

    LSA_UNUSED_ARG(RetVal);     // Return value is not evaluated
}

/*****************************************************************************/
/* Internal access function: gsy_InitTimerRqb()                              */
/*****************************************************************************/
static LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_InitTimerRqb(
GSY_UPPER_RQB_PTR_TYPE  pRbu,
GSY_TIMER_PTR		 	pTimer)
{

    GSY_UPPER_RQB_OPCODE_SET(pRbu, GSY_OPC_TIMER);      
    GSY_UPPER_RQB_RESPONSE_SET(pRbu, GSY_RSP_OK_ACTIVE);
	GSY_UPPER_RQB_USERID_PTR_SET(pRbu, pTimer); 
    GSY_UPPER_RQB_PREV_SET(pRbu, LSA_NULL);
    GSY_UPPER_RQB_NEXT_SET(pRbu, LSA_NULL);
    GSY_UPPER_RQB_CBF_SET(pRbu, LSA_NULL);
}

#endif /* #ifdef GSY_DO_TIMEOUT_EXTERN */

/*****************************************************************************/
/* Internal access function: gsy_TimerAlloc()                                */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_TimerAlloc(
GSY_TIMER_PTR  	pTimer,
LSA_UINT16		Type,
LSA_UINT16		Base)
{
	LSA_UINT16 RetVal;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_TimerAlloc() pTimer=%X Type=%d Base=%d",
					pTimer, Type, Base);

	/* Allocate LSA timer
	*/
	GSY_ALLOC_TIMER(&RetVal, &pTimer->Id, Type, Base);

#ifdef GSY_DO_TIMEOUT_EXTERN
	if (RetVal == LSA_RET_OK)
	{
		/* Allocate timer RQB
		*/
        GSY_ALLOC_UPPER_RQB_LOCAL(&pTimer->RbTimerPtr, sizeof(GSY_RQB_TYPE));
	    if(!LSA_HOST_PTR_ARE_EQUAL(pTimer->RbTimerPtr, LSA_NULL))
            gsy_InitTimerRqb (pTimer->RbTimerPtr, pTimer);
        else
	        RetVal = LSA_RET_ERR_RESOURCE;
	}
#endif
	if (RetVal == LSA_RET_OK)
	{
	 	pTimer->Type = Type;
	 	pTimer->InUse = LSA_TRUE;
	 	pTimer->Running = LSA_FALSE;
		pTimer->Waiting = LSA_FALSE;
		pTimer->Restart = LSA_FALSE;

#ifdef GSY_TIMER_CHECK	
		/* 261006lrg001: set timer base in system ticks
		*/
		pTimer->BaseTicks = 0;
		if (0 != pTimer->pChSys->RxTxNanos)
		{
			switch (Base)
			{
			default:
				break;
			case LSA_TIME_BASE_10S:
				pTimer->BaseTicks = 10000000000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_1S:
				pTimer->BaseTicks = 1000000000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_100MS:
				pTimer->BaseTicks = 100000000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_10MS:
				pTimer->BaseTicks = 10000000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_1MS:
				pTimer->BaseTicks = 1000000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_100US:
				pTimer->BaseTicks = 100000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_10US:
				pTimer->BaseTicks = 10000 / pTimer->pChSys->RxTxNanos;
				break;
			case LSA_TIME_BASE_1US:
				pTimer->BaseTicks = 1000 / pTimer->pChSys->RxTxNanos;
				break;
			}
		}
#endif
	}

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_TimerAlloc() pTimer=%X RetVal=0x%x", pTimer, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_TimerStart()                                */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_TimerStart(
GSY_TIMER_PTR	pTimer,
LSA_UINT16		Time)
{
	LSA_USER_ID_TYPE	UserId;
	LSA_UINT16			RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_TimerStart() pTimer=%X Time=%d",
					pTimer, Time);

	UserId.void_ptr = pTimer;

#ifdef GSY_TIMER_CHECK	
	/* 261006lrg001: set time in system ticks and start timestamp
	*/
	pTimer->TimeTicks = pTimer->BaseTicks * Time;
	if (0 != pTimer->BaseTicks)
		pTimer->Timestamp = GSY_GET_TIMESTAMP(pTimer->pChSys->pSys);
#endif

	if (Time != 0)
		pTimer->Time = Time;

	if ((LSA_TIMER_TYPE_ONE_SHOT == pTimer->Type)
	&&  (pTimer->Waiting))
	{
		/* 160511lrg: the timer restarts itself
		*/
		pTimer->Restart = LSA_TRUE;
		GSY_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,"--- gsy_TimerStart() pTimer=%X setting restart time=%u", pTimer, Time);
	}
	else
	{
		/* Start LSA timer
		*/
		pTimer->Running = LSA_TRUE;
		GSY_START_TIMER(&RetVal, pTimer->Id, UserId, pTimer->Time); 
		if ((LSA_RET_OK != RetVal)
		&&  (LSA_RET_OK_TIMER_RESTARTED != RetVal))		//160511lrg: for slave timer restart
		{
			pTimer->Running = LSA_FALSE;
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_TIMER, pTimer, (LSA_UINT32)pTimer->Id);
		}
	}

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_TimerStart() pTimer=%X RetVal=0x%x", pTimer, RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_TimerStop()                                 */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_TimerStop(
GSY_TIMER_PTR  		pTimer)
{
	LSA_UINT16			RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_TimerStop() pTimer=%X Time=%d",
					pTimer, pTimer->Time);

	if (pTimer->Running)
	{
		/* Stop LSA timer
		*/ 
		pTimer->Running = LSA_FALSE;
        GSY_STOP_TIMER(&RetVal, pTimer->Id);
	}

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_TimerStop() pTimer=%X RetVal=0x%x",
					pTimer, RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_TimerClean()                                */
/* Stop and free allocated timers                                            */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_TimerClean(
GSY_TIMER_PTR  		pTimer)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_TimerClean() pTimer=%X TimerId=%d",
					pTimer, pTimer->Id);

	if (pTimer->InUse)
	{
	    /* Stop timer and mark it unused (1558755)
		*/ 
		pTimer->Running = LSA_FALSE;
		GSY_STOP_TIMER(&RetVal, pTimer->Id);

		if ((LSA_RET_OK == RetVal)
		|| 	(LSA_RET_OK_TIMER_NOT_RUNNING == RetVal))
		{
#ifdef GSY_DO_TIMEOUT_EXTERN
			/* 250706lrg001: additionally check semaphore
			*/
		    if (!pTimer->RbInUse && !LSA_HOST_PTR_ARE_EQUAL(pTimer->RbTimerPtr, LSA_NULL))
			{
				/* Free timer RQB
				*/
			    GSY_FREE_UPPER_RQB_LOCAL(&RetVal, pTimer->RbTimerPtr);
				if (RetVal != LSA_RET_OK) 
			        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_RBU_FREE, 
		    					pTimer->RbTimerPtr, (LSA_UINT32)pTimer->Id);
				else
					pTimer->RbTimerPtr = LSA_NULL;
			}
			else
			{
				/* Free timer RQB on its return
				*/
				RetVal = LSA_RET_ERR_TIMER_IS_RUNNING;
/*250706lrg001	pTimer->InUse = LSA_FALSE;*/
			}
#endif 
			if (LSA_RET_ERR_TIMER_IS_RUNNING != RetVal)
			{
			    /* Free timer
				*/ 
				GSY_FREE_TIMER(&RetVal, pTimer->Id);
				pTimer->InUse = LSA_FALSE;
				if (RetVal != LSA_RET_OK) 
		    	    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_TIMER_FREE, 
		    						pTimer, (LSA_UINT32)pTimer->Id);
			}
		}
	}

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_TimerClean() pTimer=%X RetVal=0x%x",
					pTimer, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_ChUsrAlloc()                                */
/* Allocate user channel structure                                           */
/*****************************************************************************/
GSY_CH_USR_PTR  GSY_LOCAL_FCT_ATTR  gsy_ChUsrAlloc(
GSY_CH_SYS_PTR 			pChSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_SYS_PTR_TYPE		pSys,
GSY_DETAIL_PTR_TYPE		pCDB)
{
	LSA_INT			Idx;
	GSY_CH_USR_PTR 	pChUsr = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChUsrAlloc() pChSys=%X pRbu=%X pCDB=%X",
					pChSys, pRbu, pCDB);

	for (Idx = 0; Idx < GSY_MAX_USR_CHANNELS; Idx++)
	{
		pChUsr = (GSY_CH_USR_PTR)(gsy_data.pChUsrList + Idx);
		if (pChUsr->State == GSY_CHA_STATE_FREE)
		{
			/* Found free channel: allocate and initialise
			*/
			pChUsr->State = GSY_CHA_STATE_OPENING;
			pChUsr->pSys = pSys;
			pChUsr->pCDB = pCDB;
			pChUsr->SysPath = GSY_UPPER_RQB_SYSPATH_GET(pRbu);
			pChUsr->HandleUpper = GSY_UPPER_RQB_HANDLE_UPPER_GET(pRbu);
			pChUsr->pCbf = GSY_UPPER_RQB_CBF_GET(pRbu);
			pChUsr->pChSys = pChSys;
			pChUsr->TraceIdx = pChSys->TraceIdx;
			pChUsr->qRTC2Rbl.pFirst = LSA_NULL;
			pChUsr->qRTC2Rbl.pLast = LSA_NULL;
			pChUsr->qRTC2Rbl.Count = 0;
			pChUsr->RTC2RblCount = 0;
			pChUsr->pSyncSetRbl	= LSA_NULL;
			pChUsr->PrmState = GSY_PRM_STATE_NONE;
			pChUsr->PrmSubdomainIDChanged = LSA_FALSE; /* 110806js001 */
			pChUsr->PrmRTC2 = LSA_FALSE;
			pChUsr->RTC2Sync = LSA_FALSE;
			pChUsr->RTC2Active = LSA_FALSE;
			pChUsr->RTC2Ctrl = LSA_FALSE;
			pChUsr->SyncId = GSY_SYNCID_NONE;
			pChUsr->SyncRole = GSY_SYNC_ROLE_LOCAL;
			pChUsr->DiagCount = 0;
			pChUsr->DiagIdxWr = 0;
			pChUsr->DiagNo = 0;
			pChUsr->MasterControlState = GSY_MASTER_CONTROL_STATE_NONE;
			pChUsr->InfoSync = LSA_TRUE;
			pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NONE;
			pChUsr->Event.OldEvent = GSY_SYNC_EVENT_NONE;
			pChUsr->Event.Prm.ChannelProperties = GSY_PRM_CHANNEL_PROPERTIES;
			pChUsr->Event.Prm.ChannelErrorType = GSY_PRM_CHANNEL_ERROR_TYPE_SYNC;
			pChUsr->Event.Prm.ExtChannelAddValue = GSY_PRM_CHANNEL_ADD_NONE;
			pChUsr->Event.OldPrm.ExtChannelErrorType = GSY_PRM_EVENT_NONE;
			pChUsr->Event.PrmFlags = GSY_PRM_IND_FLAGS_NONE;
			pChUsr->Event.NewPrm = LSA_FALSE;
			pChUsr->Event.PortId = 0;
			pChUsr->Slave.SyncTimer.InUse = LSA_FALSE;
			pChUsr->Slave.SyncTimer.Running = LSA_FALSE;
			pChUsr->Slave.SyncTimer.pChUsr = pChUsr;
			pChUsr->Slave.SyncTimer.pChSys = pChSys;
			pChUsr->Slave.SyncTimer.pTimeoutProc = gsy_SlaveTimer;
			pChUsr->Slave.State = GSY_SLAVE_STATE_OFF;
			pChUsr->Slave.Act.Timestamp = 0;
			pChUsr->Slave.Act.IndTimestamp = 0;
			pChUsr->Slave.Act.DelaySeconds = 0;
			pChUsr->Slave.Act.DelayNanos = 0;
			pChUsr->PLLData.State = GSY_PLL_STATE_SYNC_OFF;
			pChUsr->PLLData.CycleLength = 0;
			pChUsr->PLLData.ReductionRatio = 0;
			pChUsr->PLLData.PLLHWDelay = 0;
			pChUsr->PLLData.SetWaitCount = 0;
			pChUsr->PLLData.ActWaitCount = 0;
			pChUsr->SyncPLLActive = LSA_FALSE;
			pChUsr->Master.SeqId = 0;
			pChUsr->Master.State = GSY_MASTER_STATE_OFF;
			pChUsr->Master.SendTimer.InUse = LSA_FALSE;
			pChUsr->Master.SendTimer.Running = LSA_FALSE;
			pChUsr->Master.SendTimer.pChUsr = pChUsr;
			pChUsr->Master.SendTimer.pChSys = pChSys;
			pChUsr->Master.SendTimer.pTimeoutProc = gsy_MasterSend;
			pChUsr->Master.StartupTimer.InUse = LSA_FALSE;
			pChUsr->Master.StartupTimer.Running = LSA_FALSE;
			pChUsr->Master.StartupTimer.pChUsr = pChUsr;
			pChUsr->Master.StartupTimer.pChSys = pChSys;
			pChUsr->Master.StartupTimer.pTimeoutProc = gsy_MasterStartupEnd;
			GSY_MEMSET_LOCAL(&pChUsr->SyncData, 0, sizeof(GSY_SYNC_DATA_RECORD));

			/* Write own channel to RQB
			 */
			GSY_UPPER_RQB_HANDLE_SET(pRbu, pChUsr->Handle);
			break;
		}
		else
			pChUsr = LSA_NULL;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChUsrAlloc() pRbu=%X pChUsr=%X Idx=%d",
					pRbu, pChUsr, Idx);
	return(pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_ChAnnoAlloc()                               */
/* Allocate announce channel structure                                       */
/*****************************************************************************/
GSY_CH_ANNO_PTR	GSY_LOCAL_FCT_ATTR	gsy_ChAnnoAlloc(
GSY_CH_SYS_PTR 			pChSys,
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_SYS_PTR_TYPE		pSys,
GSY_DETAIL_PTR_TYPE		pCDB)
{
	LSA_INT				Idx;
	GSY_CH_ANNO_PTR 	pChAnno = LSA_NULL;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChAnnoAlloc() pChSys=%X pRbu=%X pCDB=%X",
					pChSys, pRbu, pCDB);

	for (Idx = 0; Idx < GSY_MAX_SYS_CHANNELS; Idx++)
	{
		pChAnno = (GSY_CH_ANNO_PTR)(gsy_data.pChAnnoList + Idx);
		if (pChAnno->State == GSY_CHA_STATE_FREE)
		{
			/* Found free channel: allocate and initialise
			*/
			pChAnno->State = GSY_CHA_STATE_OPENING;
			pChAnno->pSys = pSys;
			pChAnno->pCDB = pCDB;
			pChAnno->Resp = GSY_RSP_OK;
			pChAnno->SysPath = GSY_UPPER_RQB_SYSPATH_GET(pRbu);
			pChAnno->HandleUpper = GSY_UPPER_RQB_HANDLE_UPPER_GET(pRbu);
			pChAnno->pCbf = GSY_UPPER_RQB_CBF_GET(pRbu);
			pChAnno->pChSys = pChSys;
			pChAnno->TraceIdx = pChSys->TraceIdx;
			pChSys->pChAnno = pChAnno;

			/* Write own channel to RQB
			 */
			GSY_UPPER_RQB_HANDLE_SET(pRbu, pChAnno->Handle);
			break;
		}
		else
			pChAnno = LSA_NULL;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChAnnoAlloc() pRbu=%X pChAnno=%X Idx=%d",
					pRbu, pChAnno, Idx);
	return(pChAnno);
}

/*****************************************************************************/
/* Internal access function: gsy_ChSysAlloc()                               */
/* Systemchannel-Struktur allokieren                                         */
/*****************************************************************************/
GSY_CH_SYS_PTR  GSY_LOCAL_FCT_ATTR  gsy_ChSysAlloc(
GSY_UPPER_RQB_PTR_TYPE  pRbu,
LSA_SYS_PTR_TYPE		pSys,
GSY_DETAIL_PTR_TYPE		pCDB)
{
	LSA_INT				iii, ii, i, Idx;
	GSY_CH_SYS_PTR 		pChSys = LSA_NULL;

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChSysAlloc() pRbu=%X pCDB=%X",
					pRbu, pCDB);

	for (Idx = 0; Idx < GSY_MAX_SYS_CHANNELS; Idx++)
	{
		pChSys = (GSY_CH_SYS_PTR)(gsy_data.pChSysList + Idx);
		if (pChSys->State == GSY_CHA_STATE_FREE)
		{
			/* Found free channel: allocate and initialise
			*/
			pChSys->State = GSY_CHA_STATE_OPENING;
			pChSys->pSys = pSys;
			pChSys->pCDB = pCDB;
			pChSys->Resp = GSY_RSP_OK;
			pChSys->PortInit = 0;
			pChSys->PortCount = 0;
			pChSys->RxTxNanos = 0;
			pChSys->TraceIdx = 0;
			pChSys->HardwareType = GSY_HW_TYPE_STANDARD;
			pChSys->SysPath = GSY_UPPER_RQB_SYSPATH_GET(pRbu);
			pChSys->HandleUpper = GSY_UPPER_RQB_HANDLE_UPPER_GET(pRbu);
			pChSys->pCbf = GSY_UPPER_RQB_CBF_GET(pRbu);
			pChSys->PortDelayReqActive = 0;
			pChSys->PortDelayRspActive = 0;
			pChSys->DriftTimer.InUse = LSA_FALSE;
			pChSys->DriftTimer.Running = LSA_FALSE;
			pChSys->DriftTimer.pChSys = pChSys;
			pChSys->DriftTimer.pTimeoutProc = gsy_DriftAdjust;

			for (i = 0; i < GSY_CH_MAX_PORTS ; i++)
			{
				/* 290306lrg001: allow forwarding for all configured SyncIds on all ports
				*/
				pChSys->PortFwd[i].FwdMode = GSY_SYNC_ID_SUPPORTED;
				pChSys->PortInp[i].FwdMode = GSY_SYNC_ID_SUPPORTED;

				pChSys->Port[i].State = GSY_PORT_STATE_INIT;
				pChSys->Port[i].SyncLineDelay = 0;
				pChSys->Port[i].CableDelay = 0;
				pChSys->Port[i].MinDelay = 0;
				pChSys->Port[i].MaxDelay = 0;
				pChSys->Port[i].SumCount = 0;
				pChSys->Port[i].TxDelay = 0;
				pChSys->Port[i].RxDelay = 0;
				pChSys->Port[i].ClockSyncOk = LSA_FALSE; 				// 170707lrg001
				pChSys->Port[i].DelayReqActive = LSA_FALSE;
				pChSys->Port[i].DelayRspActive = LSA_FALSE;				// 291008lrg001

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

				pChSys->Port[i].SumDelay = 0;
				pChSys->Port[i].SumCountLast = 0;
				pChSys->Port[i].DelayEvent = GSY_SYNC_EVENT_DELAY_OK;
				pChSys->Port[i].ActDelay.State = GSY_DELAY_STATE_INI;
				pChSys->Port[i].ActDelay.pDrift = LSA_NULL;
				pChSys->Port[i].LastError = LSA_RSP_OK; 
				pChSys->Port[i].ErrorCount = 0; 
				pChSys->Port[i].DelayReqSeqId = 0;
				pChSys->Port[i].DelayTimer.InUse = LSA_FALSE;
				pChSys->Port[i].DelayTimer.Running = LSA_FALSE;
				pChSys->Port[i].DelayTimer.Port = (LSA_UINT16)(i + 1);	// 060509lrg001
				pChSys->Port[i].DelayTimer.pChSys = pChSys;
				pChSys->Port[i].DelayTimer.pTimeoutProc = gsy_DelayReqNext;

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32

				pChSys->Port[i].PrmDomainWritten = LSA_FALSE;
				pChSys->Port[i].PrmDomainState = GSY_PRM_PORT_STATE_INI;
				pChSys->Port[i].PrmPeerWritten = LSA_FALSE;
				pChSys->Port[i].PrmPeerState = GSY_PRM_PORT_STATE_INI;
				pChSys->Port[i].DelaySetPending = LSA_FALSE;			//240608lrg001
				pChSys->Port[i].SendCnfPending = 0;						//240608lrg001
				pChSys->Port[i].pExtLinkRbl = LSA_NULL;
			}
			pChSys->pChUsr = LSA_NULL;
			for (i = 0; i < GSY_MAX_PROTOCOLS ; i++)
			{
				for (ii = 0; ii < GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL ; ii++)
				{
					GSY_MEMSET_LOCAL(&pChSys->Drift[i][ii], 0, sizeof(GSY_DRIFT_TYPE));
					pChSys->Drift[i][ii].Diag.SyncId = (LSA_UINT8)i;				//230310lrg001; 040811lrg: cast
					pChSys->Drift[i][ii].Diag.PLLState = GSY_PLL_STATE_SYNC_OFF;
					pChSys->Drift[i][ii].SyncId = GSY_SYNCID_NONE;
					pChSys->Drift[i][ii].SyncIdRate = (LSA_UINT8)i;		// 1684006
					pChSys->Drift[i][ii].State = GSY_DRIFT_STATE_UNKNOWN;
					pChSys->Drift[i][ii].PendingSet = LSA_FALSE;		// 1684006
					pChSys->Drift[i][ii].DiagCount = 0;
					pChSys->Drift[i][ii].IntervalSetValid = LSA_FALSE;	/* 310707js001 */
//					pChSys->Drift[i][ii].PrimaryMaster = LSA_FALSE;		/* 161007lrg003 170408lrg002 */
					pChSys->Drift[i][ii].RcvSyncPrio = 0;				/* 170408lrg002 */
					pChSys->Drift[i][ii].LocalPrio = 0;					/* 170408lrg002 */
					for (iii = 0; iii < GSY_DRIFT_MAX_COUNT ; iii++)
					{
						pChSys->Drift[i][ii].Old[iii].Used = LSA_TRUE;	//180108lrg001
					}
				}
			}
			pChSys->Fwd.Status = GSY_FWD_CHANNEL_CLOSED;

			/* Write own channel to RQB
			 */
			GSY_UPPER_RQB_HANDLE_SET(pRbu, pChSys->Handle);
			break;
		}
		else
			pChSys = LSA_NULL;
	}

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChSysAlloc() pRbu=%X pChSys=%X Idx=%d",
					pRbu, pChSys, Idx);
	return(pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChSysFind()                                 */
/* Find NicId in system channel structures                                   */
/*****************************************************************************/
GSY_CH_SYS_PTR  GSY_LOCAL_FCT_ATTR  gsy_ChSysFind(
LSA_UINT16 NicId)
{
	LSA_INT			Idx;
	GSY_CH_SYS_PTR 	pChSys = LSA_NULL;

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChSysFind() NicId=%u",
					NicId);


	for (Idx = 0; Idx < GSY_MAX_SYS_CHANNELS; Idx++)
	{
		pChSys = (GSY_CH_SYS_PTR)(gsy_data.pChSysList + Idx);
		if ((pChSys->State != GSY_CHA_STATE_FREE)
		&&  (pChSys->pCDB->NicId == NicId))
		{
			/* Channel found
			*/
			break;
		}
	}

	if (Idx == GSY_MAX_SYS_CHANNELS)
			pChSys = LSA_NULL;

	GSY_FUNCTION_TRACE_02(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChSysFind() pChSys=%X Idx=%d", pChSys, Idx);
	return(pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChSysClean()                                */
/* Free system channel resources                                             */
/*****************************************************************************/
LSA_UINT16  GSY_LOCAL_FCT_ATTR  gsy_ChSysClean(
GSY_CH_SYS_PTR pChSys)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_INT					Idx;
	LSA_UINT16				RetVal = LSA_RET_OK;
	GSY_TIMER_PTR  			pTimer = LSA_NULL;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChSysClean() pChSys=%X UserCount=%d",
					pChSys, pChSys->UserCount);
	
	/* Free lower RQBs
	*/ 
	while (pChSys->QueueLowerSend.Count != 0)
	{
		pRbl = pChSys->QueueLowerSend.pFirst;
		gsy_DequeLower(&pChSys->QueueLowerSend, pRbl);
		gsy_FreeLowerRb(pChSys,pRbl, LSA_FALSE);
	}
	while (pChSys->QueueLowerRecv.Count != 0)
	{
		pRbl = pChSys->QueueLowerRecv.pFirst;
		gsy_DequeLower(&pChSys->QueueLowerRecv, pRbl);
		gsy_FreeLowerRb(pChSys,pRbl, LSA_FALSE);
	}
	while (pChSys->QueueLowerCtrl.Count != 0)
	{
		pRbl = pChSys->QueueLowerCtrl.pFirst;
		gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
		gsy_FreeLowerRb(pChSys,pRbl, LSA_FALSE);
	}
	for (Idx = 0; Idx < pChSys->PortCount; Idx++)
	{
		if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[Idx].pExtLinkRbl, LSA_NULL))
		{
			gsy_FreeLowerRb(pChSys,pChSys->Port[Idx].pExtLinkRbl, LSA_FALSE);
			pChSys->Port[Idx].pExtLinkRbl = LSA_NULL;
		}
	}

	/* Free Timer
	*/ 
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

	RetVal = gsy_TimerClean(&pChSys->DriftTimer);       //lint !e838 Previously assigned value has not been used, but it depends on the define GSY_CFG_SYNC_ONLY_IN_KRISC32, HM 15.06.2016

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

	if (RetVal == LSA_RET_OK)
	{
		LSA_INT	i;
		for (i = 0; i < pChSys->PortCount; i++)
		{
			pTimer = &pChSys->Port[i].DelayTimer;
			RetVal = gsy_TimerClean(pTimer);
			if (RetVal != LSA_RET_OK)
				break; 
		}
	}
#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32

/* 220710lrg001: moved to gsy_ChSysFree

    if ((!LSA_HOST_PTR_ARE_EQUAL(pChSys->pCDB, LSA_NULL))
	&&  (LSA_RET_OK == RetVal))
	{
	    GSY_RELEASE_PATH_INFO(&RetVal, pChSys->pSys, pChSys->pCDB);
		if (LSA_RET_OK != RetVal)
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_REL_PATH, pChSys->pCDB, 0);
		pChSys->pCDB = LSA_NULL;
	}
*/
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChSysClean() pChSys=%X pTimer=%X RetVal=0x%x", 
					pChSys, pTimer, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_ChSysFree()                                 */
/* Free system channel structure  and path                                   */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_ChSysFree(
GSY_CH_SYS_PTR pChSys)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChSysFree() pChSys=%X",
					pChSys);
	
    if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pCDB, LSA_NULL))
	{
	    GSY_RELEASE_PATH_INFO(&RetVal, pChSys->pSys, pChSys->pCDB);
		if (LSA_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChSysFree() pChSys=%X RetVal=0x%x from GSY_RELEASE_PATH_INFO",
							pChSys, RetVal);
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_REL_PATH, pChSys->pCDB, 0);
		}
		pChSys->pCDB = LSA_NULL;
	}

	/* If pointer points to a system channel structure...
	*/
	if ((pChSys >= (GSY_CH_SYS_PTR)gsy_data.pChSysList) 
	&&  (pChSys <= (GSY_CH_SYS_PTR)(gsy_data.pChSysList + GSY_MAX_SYS_CHANNELS)))
	{
		/* ...and the RQB queues are empty... 
		*/
		if ((pChSys->QueueUpper.Count != 0)
		||  (pChSys->UserCount != 0))
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChSysFree() pChSys=%X  QueueUpper.Count=%u UserCount=%u",
							pChSys, pChSys->QueueUpper.Count, pChSys->UserCount);
	        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_FREE, pChSys, pChSys->UserCount);
		}

		/* ...mark structure as free
		*/
		pChSys->State = GSY_CHA_STATE_FREE;
	}
    else
	{
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChSysFree() Invalid pChSys=%X",	pChSys);
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pChSys, 0);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChSysFree() pChSys=%X", pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChAnnoClean()                               */
/* Free announce channel resources                                           */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_ChAnnoClean(
GSY_CH_ANNO_PTR pChAnno)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;

	GSY_FUNCTION_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChAnnoClean() pChAnno=%X pChSys=%X",
					pChAnno, pChAnno->pChSys);
	
	/* Free lower RQBs
	*/ 
	while (pChAnno->QueueLower.Count != 0)
	{
		pRbl = pChAnno->QueueLower.pFirst;
		gsy_DequeLower(&pChAnno->QueueLower, pRbl);
		gsy_FreeLowerRb(pChAnno->pChSys,pRbl, LSA_TRUE);
	}

/* 220710lrg001: moved to gsy_ChAnnoFree

    if ((!LSA_HOST_PTR_ARE_EQUAL(pChAnno->pCDB, LSA_NULL))
	&&  (LSA_RET_OK == RetVal))
	{
	    GSY_RELEASE_PATH_INFO(&RetVal, pChAnno->pSys, pChAnno->pCDB);
		if (LSA_RET_OK != RetVal)
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_REL_PATH, pChAnno->pCDB, 0);
		pChAnno->pCDB = LSA_NULL;
	}
*/
	GSY_FUNCTION_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChAnnoClean() pChAnno=%X pChSys=%X",
						pChAnno, pChAnno->pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChAnnoFree()                                */
/* Free announce channel structure  and path                                 */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_ChAnnoFree(
GSY_CH_ANNO_PTR pChAnno)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_01(pChAnno->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChAnnoFree() pChAnno=%X",
					pChAnno);
	

    if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno->pCDB, LSA_NULL))
	{
	    GSY_RELEASE_PATH_INFO(&RetVal, pChAnno->pSys, pChAnno->pCDB);
		if (LSA_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChAnnoFree() pChAnno=%X RetVal=0x%x from GSY_RELEASE_PATH_INFO",
							pChAnno, RetVal);
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_REL_PATH, pChAnno->pCDB, RetVal);
		}
		pChAnno->pCDB = LSA_NULL;
	}

	/* If pointer points to a announce channel structure...
	*/
	if ((pChAnno >= (GSY_CH_ANNO_PTR)gsy_data.pChAnnoList) 
	&&  (pChAnno <= (GSY_CH_ANNO_PTR)(gsy_data.pChAnnoList + GSY_MAX_SYS_CHANNELS)))
	{
		/* ...and the RQB queues are empty... 
		*/
		if (pChAnno->QueueUpper.Count != 0)
		{
			GSY_ERROR_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChAnnoFree() pChAnno=%X QueueUpper.Count=%u",
							pChAnno, pChAnno->QueueUpper.Count); 
	        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_FREE, pChAnno, pChAnno->QueueUpper.Count);
		}

		/* ...mark structure as free
		*/
		pChAnno->State = GSY_CHA_STATE_FREE;
		if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno->pChSys, LSA_NULL))
		{
			pChAnno->pChSys->pChAnno = LSA_NULL;
			pChAnno->pChSys = LSA_NULL;
		}
	}
    else
	{
		GSY_ERROR_TRACE_01(pChAnno->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChAnnoFree() Invalid pChAnno=%X",
						pChAnno->QueueUpper.Count); 
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pChAnno, 0);
	}

	GSY_FUNCTION_TRACE_01(pChAnno->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChAnnoFree() pChAnno=%X", pChAnno);
}

/*****************************************************************************/
/* Internal access function: gsy_ChUsrClean()                                */
/* Free user channel resources                                               */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_ChUsrClean(
GSY_CH_USR_PTR pChUsr)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChUsrClean() pChUsr=%X pChSys=%X",
					pChUsr, pChUsr->pChSys);
	
	if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr->pSyncSetRbl, LSA_NULL))
	{
		/*  Free Lower SyncSet-RQB
		*/
		gsy_FreeLowerRb(pChUsr->pChSys, pChUsr->pSyncSetRbl, LSA_FALSE);
		pChUsr->pSyncSetRbl = LSA_NULL;
	}
	while (pChUsr->qRTC2Rbl.Count != 0)
	{
		/*  Free Lower Rtc2Ctrl-RQBs
		*/
		pRbl = pChUsr->qRTC2Rbl.pFirst;
		gsy_DequeLower(&pChUsr->qRTC2Rbl, pRbl);
		gsy_FreeLowerRb(pChUsr->pChSys,pRbl, LSA_FALSE);
		pChUsr->RTC2RblCount--;
	}

	/* Detach user channel from system channel
	*/
	pChUsr->pChSys->UserCount--;
	pChUsr->pChSys->pChUsr = LSA_NULL;
	pChUsr->pChSys = LSA_NULL;

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChUsrClean() pChUsr=%X pChSys=%X", 
						pChUsr, pChUsr->pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChUsrFree()                                 */
/* Free user channel structure and path                                      */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR  gsy_ChUsrFree(
GSY_CH_USR_PTR pChUsr)
{
	LSA_UINT16 RetVal;

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChUsrFree() pChUsr=%X",
					pChUsr);
	
    GSY_RELEASE_PATH_INFO(&RetVal, pChUsr->pSys, pChUsr->pCDB);
	if (LSA_RET_OK != RetVal)
	{
		GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChUsrFree() pChUsr=%X RetVal=0x%x from GSY_RELEASE_PATH_INFO",
						pChUsr, RetVal);
	    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_REL_PATH, pChUsr->pCDB, 0);
	}

	/* If pointer points to a system channel structure...
	*/
	if ((pChUsr >= (GSY_CH_USR_PTR)gsy_data.pChUsrList) 
	&&  (pChUsr <= (GSY_CH_USR_PTR)(gsy_data.pChUsrList + GSY_MAX_USR_CHANNELS)))
	{
		/* ...and the RQB queues are empty... 
		*/
		if ((pChUsr->QueueUpper.Count != 0)
		||  (pChUsr->QueuePrm.Count != 0)
		||  (pChUsr->QueueRcv.Count != 0)
		||  (pChUsr->Master.QueueSync.Count != 0)
		||  (pChUsr->Master.QueueFu.Count != 0)
		||  (pChUsr->Master.QueueAnnounce.Count != 0))
		{
			GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChUsrFree() pChUsr=%X Queue not empty", pChUsr);
	        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_FREE, pChUsr, pChUsr->QueueUpper.Count);
		}
		else
		{
			/* ...mark structure as free
			*/
			pChUsr->State = GSY_CHA_STATE_FREE;
		}
	}
    else
	{
		GSY_ERROR_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_ChUsrFree() Invalid pChUsr=%X", pChUsr);
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pChUsr, 0);
	}

	GSY_FUNCTION_TRACE_01(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChUsrFree() pChUsr=%X", pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_ChSysGet()                                  */
/* Get channel pointer to allocated system channel structure                 */
/*****************************************************************************/
GSY_CH_SYS_PTR GSY_LOCAL_FCT_ATTR  gsy_ChSysGet(
LSA_HANDLE_TYPE  Handle)
{
	LSA_HANDLE_TYPE	ChId = 0;
	GSY_CH_SYS_PTR	pChSys = LSA_NULL;
    LSA_UINT32		TraceIdx = 0;

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChSysGet() Handle=%d",
					Handle);

	/* Check Handle
	*/
	if (Handle > 0)
	{
		ChId = Handle - 1;
	    if(ChId < GSY_MAX_SYS_CHANNELS)
	    {
			/* Build pointer to system channel structure
			*/
			pChSys = (GSY_CH_SYS_PTR)(gsy_data.pChSysList + ChId);

			/* Is not valid if channel is not allocated          
			*/ 
			if (pChSys->State == GSY_CHA_STATE_FREE)
			{
				pChSys = LSA_NULL;
			}
			else
			{
				TraceIdx = pChSys->TraceIdx;
			}
	    }
	}
	GSY_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChSysGet() pChSys=%X ChId=%d", pChSys, ChId);
    return(pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_ChAnnoGet()	                             */
/* Get channel pointer to allocated announce channel structure               */
/*****************************************************************************/
GSY_CH_ANNO_PTR  GSY_LOCAL_FCT_ATTR  gsy_ChAnnoGet(
LSA_HANDLE_TYPE  Handle)
{
	LSA_HANDLE_TYPE	ChId = 0;
	GSY_CH_ANNO_PTR	pChAnno = LSA_NULL;
    LSA_UINT32		TraceIdx = 0;

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChAnnoGet() Handle=%d",
					Handle);

	/* Check Handle
	*/
	if (Handle > GSY_MAX_SYS_CHANNELS)
	{
		ChId = (Handle - GSY_MAX_SYS_CHANNELS) - 1;
	    if(ChId < GSY_MAX_SYS_CHANNELS)
	    {
			/* Build pointer to channel structure
			*/
			pChAnno = (GSY_CH_ANNO_PTR)(gsy_data.pChAnnoList + ChId);

			/* Is not valid if channel is not allocated          
			*/ 
			if (pChAnno->State == GSY_CHA_STATE_FREE)
			{
				pChAnno = LSA_NULL;
			}
			else
			{
				TraceIdx = pChAnno->TraceIdx;
			}
	    }
	}
	GSY_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChAnnoGet() pChAnno=%X ChId=%d", pChAnno, ChId);
    return(pChAnno);
}

/*****************************************************************************/
/* Internal access function: gsy_ChUsrGet()	                                 */
/* Get channel pointer to allocated user channel structure                   */
/*****************************************************************************/
GSY_CH_USR_PTR  GSY_LOCAL_FCT_ATTR  gsy_ChUsrGet(
LSA_HANDLE_TYPE  Handle)
{
	LSA_HANDLE_TYPE	ChId = 0;
	GSY_CH_USR_PTR	pChUsr = LSA_NULL;
    LSA_UINT32		TraceIdx = 0;

	GSY_FUNCTION_TRACE_01(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_ChUsrGet() Handle=%d",
					Handle);

	/* Check Handle
	*/
	if (Handle > 2*GSY_MAX_SYS_CHANNELS)
	{
		ChId = (Handle - 2*GSY_MAX_SYS_CHANNELS) - 1;
	    if(ChId < GSY_MAX_USR_CHANNELS)
	    {
			/* Build pointer to channel structure
			*/
			pChUsr = (GSY_CH_USR_PTR)(gsy_data.pChUsrList + ChId);

			/* Is not valid if channel is not allocated          
			*/ 
			if (pChUsr->State == GSY_CHA_STATE_FREE)
			{
				pChUsr = LSA_NULL;
			}
			else
			{
				TraceIdx = pChUsr->TraceIdx;
			}
	    }
	}
	GSY_FUNCTION_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_ChUsrGet() pChUsr=%X ChId=%d", pChUsr, ChId);
    return(pChUsr);
}

/*****************************************************************************/
/* Internal access function: gsy_PutUpperQ()	                             */
/* Store upper Request block in Queue                                        */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PutUpperQ(
GSY_UPPER_QUEUE		  *	pQue,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	GSY_UPPER_RQB_PTR_TYPE  pLast;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_PutUpperQ() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);

	/* Append RQB to the upper Queue
	*/
	pLast = pQue->pLast;

	/* Mark new element as Queue end
	*/
	GSY_UPPER_RQB_NEXT_SET(pRbu, LSA_NULL);
	GSY_UPPER_RQB_PREV_SET(pRbu, pLast);
	pQue->pLast = pRbu;
	
	/* If the queue was empty... 
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pLast, LSA_NULL))
	{
		/* ...the last element is also th first one
		*/
		pQue->pFirst = pRbu;
	}
	else
	{
		/* Connect the new last to the old last element
		*/
		GSY_UPPER_RQB_NEXT_SET(pLast, pRbu);
	}
	/* Queue now contains one more element
	*/
	pQue->Count++;
	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_PutUpperQ() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);
}

/*****************************************************************************/
/* Internal access function: gsy_GetUpperQ()	                             */
/* Remove upper Request block from Queue                                     */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_GetUpperQ(
GSY_UPPER_QUEUE		  *	pQue,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	GSY_UPPER_RQB_PTR_TYPE  pPrev;
	GSY_UPPER_RQB_PTR_TYPE  pNext;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_GetUpperQ() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);

	pNext = GSY_UPPER_RQB_NEXT_GET(pRbu);
	pPrev = GSY_UPPER_RQB_PREV_GET(pRbu);
	
	/* If a predecessor is present... 
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(pPrev, LSA_NULL))
	{
		/* ...set its successor to next element
		*/
		GSY_UPPER_RQB_NEXT_SET(pPrev, pNext);
	}
	else
	{
		/* The successor becomes first element of the queue
		*/
		pQue->pFirst = pNext;
	}
	/* If a successor is present... 
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
	{
		/* ...set its predecessor to previous element
		*/
		GSY_UPPER_RQB_PREV_SET(pNext, pPrev);
	}
	else
	{
		/* The predecessor becomes last element of the queue
		*/
		pQue->pLast = pPrev;
	}
	/* Delete chain pointers of removed RQB
	*/
	GSY_UPPER_RQB_PREV_SET(pRbu, LSA_NULL);
	GSY_UPPER_RQB_NEXT_SET(pRbu, LSA_NULL);
	
	/* Queue now contains one less element
	*/
	pQue->Count--;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_GetUpperQ() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);
}

/*****************************************************************************/
/* Internal access function: gsy_FindUpper()		                         */
/* Find upper Request block in a Queue                                       */
/*****************************************************************************/
LSA_BOOL    GSY_LOCAL_FCT_ATTR gsy_FindUpper(
GSY_UPPER_QUEUE		  *	pQue,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	GSY_UPPER_RQB_PTR_TYPE  pNext;
	LSA_BOOL				RbFound = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_FindUpper() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);

	pNext = pQue->pFirst;
	while (!LSA_HOST_PTR_ARE_EQUAL(pNext, pRbu)
	   &&  !LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
	{
		pNext = GSY_UPPER_RQB_NEXT_GET(pNext);
	}

	if (LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
	{
		RbFound = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_FindUpper() pRbu=%X pQue=%X RbFound=%d",
					pRbu, pQue, RbFound);
	return(RbFound);
}

/*****************************************************************************/
/* Internal access function: gsy_DequeUpper()		                         */
/* Find and dequeue upper RQB                                                */
/*****************************************************************************/
LSA_VOID    GSY_LOCAL_FCT_ATTR gsy_DequeUpper(
GSY_UPPER_QUEUE		  *	pQue,
GSY_UPPER_RQB_PTR_TYPE	pRbu)
{
	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_DequeUpper() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);

	/* Find Request block in the Queue...
	*/
	if (gsy_FindUpper(pQue, pRbu))
	{
		/* ...and if found: remove it
		*/
		gsy_GetUpperQ(pQue, pRbu);
	}

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_DequeUpper() pRbu=%X pQue=%X Count=%d",
					pRbu, pQue, pQue->Count);
}

/*****************************************************************************/
/* Internal access function: gsy_EnqueLower()	                             */
/* Store lower request block in queue                                        */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_EnqueLower(
GSY_LOWER_QUEUE		  *	pQue,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	GSY_LOWER_RQB_PTR_TYPE  pLast;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_EnqueLower() pRbu=%X pQue=%X Count=%d",
					pRbl, pQue, pQue->Count);

	/* Append RQB to the lower Queue
	*/
	pLast = pQue->pLast;

	/* Mark new element as Queue end
	*/
	GSY_LOWER_RQB_NEXT_SET(pRbl, LSA_NULL);
	GSY_LOWER_RQB_PREV_SET(pRbl, pLast);
	pQue->pLast = pRbl;
	
	/* If the queue was empty... 
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pLast, LSA_NULL))
	{
		/* ...the last element is also th first one
		*/
		pQue->pFirst = pRbl;
	}
	else
	{
		/* Connect the new last to the old last element
		*/
		GSY_LOWER_RQB_NEXT_SET(pLast, pRbl);
	}
	/* Queue now contains one more element
	*/
	pQue->Count++;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_EnqueLower() pRbu=%X pQue=%X Count=%d",
					pRbl, pQue, pQue->Count);
}

/*****************************************************************************/
/* Internal access function: gsy_DequeLower()		                         */
/* Remove lower Request block from Queue                                     */
/*****************************************************************************/
LSA_VOID    GSY_LOCAL_FCT_ATTR gsy_DequeLower(
GSY_LOWER_QUEUE		  *	pQue,
GSY_LOWER_RQB_PTR_TYPE 	pRbl)
{
	GSY_LOWER_RQB_PTR_TYPE	pPrev;
	GSY_LOWER_RQB_PTR_TYPE  pNext;

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  > gsy_DequeLower() pRbu=%X pQue=%X Count=%d",
					pRbl, pQue, pQue->Count);

	/* Find Request block in the Queue...
	*/
	pNext = pQue->pFirst;
	while (!LSA_HOST_PTR_ARE_EQUAL(pNext, pRbl)
	   &&  !LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
	{
		pNext = GSY_LOWER_RQB_NEXT_GET(pNext);
	}
	/* ...and if found: remove it
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
	{
		pNext = GSY_LOWER_RQB_NEXT_GET(pRbl);
		pPrev = GSY_LOWER_RQB_PREV_GET(pRbl);
		
		/* If a predecessor is present... 
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pPrev, LSA_NULL))
		{
			/* ...set its successor to next element
			*/
			GSY_LOWER_RQB_NEXT_SET(pPrev, pNext);
		}
		else
		{
			/* The successor becomes first element of the queue
			*/
			pQue->pFirst = pNext;
		}
		/* If a successor is present... 
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pNext, LSA_NULL))
		{
			/* ...set its predecessor to previous element
			*/
			GSY_LOWER_RQB_PREV_SET(pNext, pPrev);
		}
		else
		{
			/* The predecessor becomes last element of the queue
			*/
			pQue->pLast = pPrev;
		}
		/* Delete chain pointers of removed RQB
		*/
		GSY_LOWER_RQB_PREV_SET(pRbl, LSA_NULL);
		GSY_LOWER_RQB_NEXT_SET(pRbl, LSA_NULL);
		
		/* Queue now contains one less element
		*/
		pQue->Count--;
	}

	GSY_FUNCTION_TRACE_03(GSY_UNDEF_TRACE_IDX, LSA_TRACE_LEVEL_CHAT,"  < gsy_DequeLower() pRbu=%X pQue=%X Count=%d",
					pRbl, pQue, pQue->Count);
}

/*****************************************************************************/
/* Internal access function: gsy_ErrorUser()                                 */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR gsy_ErrorUser(
LSA_INT				Module,
LSA_INT				Line,
LSA_INT  			ErrNum,
LSA_VOID_PTR_TYPE	Pointer,
LSA_UINT16			Size)
{
    gsy_data.Error.lsa_component_id = LSA_COMP_ID_GSY;
    gsy_data.Error.module_id = 			(LSA_UINT16)Module;
    gsy_data.Error.line = 	   			(LSA_UINT16)Line;
    gsy_data.Error.error_data_length = 	Size;
    gsy_data.Error.error_code[0] = (LSA_UINT32)(ErrNum);
    gsy_data.Error.error_code[1] = 0;
    gsy_data.Error.error_code[2] = 0;
    gsy_data.Error.error_code[3] = 0;
    gsy_data.Error.error_data_ptr = Pointer;

    GSY_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &gsy_data.Error);
}

/*****************************************************************************/
/* Internal access function: gsy_ErrorLower()                               */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR gsy_ErrorLower(
LSA_INT				Module,
LSA_INT				Line,
LSA_INT  			ErrNum,
LSA_VOID_PTR_TYPE	Pointer,
LSA_UINT16			Size)
{
    gsy_data.Error.lsa_component_id = LSA_COMP_ID_GSY;
    gsy_data.Error.module_id = 			(LSA_UINT16)Module;
    gsy_data.Error.line = 	   			(LSA_UINT16)Line;
    gsy_data.Error.error_data_length = 	Size;
    gsy_data.Error.error_code[0] = (LSA_UINT32)(ErrNum);
    gsy_data.Error.error_code[1] = 0;
    gsy_data.Error.error_code[2] = 0;
    gsy_data.Error.error_code[3] = 0;
    gsy_data.Error.error_data_ptr = Pointer;

    GSY_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &gsy_data.Error);
}

/*****************************************************************************/
/* Internal access function: gsy_ErrorInternal()                            */
/*****************************************************************************/
LSA_VOID  GSY_LOCAL_FCT_ATTR gsy_ErrorInternal(
LSA_INT		Module,
LSA_INT		Line,
LSA_INT  	ErrNum,
LSA_VOID *	Pointer,
LSA_UINT32	Value)
{
    gsy_data.Error.lsa_component_id = LSA_COMP_ID_GSY;
    gsy_data.Error.module_id 	 = (LSA_UINT16)Module;
    gsy_data.Error.line 		 = (LSA_UINT16)Line;
    gsy_data.Error.error_code[0] = (LSA_UINT32)(ErrNum);
    gsy_data.Error.error_code[1] = Value;
    gsy_data.Error.error_code[2] = 0;
    gsy_data.Error.error_code[3] = 0;
    gsy_data.Error.error_data_ptr = Pointer;
    gsy_data.Error.error_data_length = 	0;

    GSY_FATAL_ERROR(sizeof(LSA_FATAL_ERROR_TYPE), &gsy_data.Error);
}

/*****************************************************************************/
/*  end of file GSY_SYS.C                                                    */
/*****************************************************************************/

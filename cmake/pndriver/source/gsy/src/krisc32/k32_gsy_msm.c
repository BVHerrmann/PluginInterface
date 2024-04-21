/*****************************************************************************/
/*  Copyright (C) 2017 Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: k32_gsy_msm.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P06.01.00.00_00.02.00.02         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2017-03-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Slave and master functions for synchronization in KRISC32                */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                   Who  What                          */
/*  2008-10-30  P04.01.00.00_00.03.01.02 lrg  file creation from gsy_msm.c   */
/*  2009-03-18  P04.01.00.00_00.03.03.02 180309lrg001: k32gsy_SyncMsg        */
/*  2009-05-07  P04.02.00.00_00.01.01.02 lrg: K32GSY_CHANGE_x asynchron      */
/*  2009-06-18  P04.02.00.00_00.01.03.02 180609lrg001: Cast bei Offs.berech. */
/*  2009-06-25  P04.02.00.00_00.01.03.03 250609lrg001: Weiterleitung von     */
/*              ClockSyncFrames zum Port nur bei dort gemessenem LineDelay   */
/*              erlauben                                                     */
/*  2009-07-08  P04.02.00.00_00.01.04.01 lrg: Master implemented             */
/*  2009-07-20  P04.02.00.00_00.01.04.02 lrg: Receive frame buffers: 32 bit  */
/*              aligned                                                      */
/*  2009-07-29  P04.02.00.00_00.01.04.03 290709lrg001 pChSys->MACAddr fuer   */
/*              die lokale IF-MAC benutzen (todo: auch beim Slave)           */
/*  2009-09-03  P04.02.00.00_00.01.04.04 030909lrg001 Diagnose bei Wechsel   */
/*              der Drift von/nach 0 und bei geaenderter RcvSyncPrio         */
/*  2009-09-24  P04.02.00.00_00.01.04.05 lrg: K32_MASTER_IN_FW               */
/*  2009-10-12  P04.02.00.00_00.01.04.06 121009lrg001: changed RcvSyncPrio   */
/*              only with MAC address                                        */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: K32_SYNC_UPDATE                */
/*  2009-11-13  P05.00.00.00_00.01.01.03 251109lrg001: "!" removed           */
/*  2009-12-01  P05.00.00.00_00.01.01.04 lrg: GSY_... -> K32_PTCP_SYNC_FRAME */
/*  2009-12-02  P05.00.00.00_00.01.01.05 lrg: k32gsy_FwdSet() not K32_STATIC */
/*  2010-03-11  P05.00.00.00_00.01.03.02 110310lrg001 k32gsy_MasterStart():  */
/*              Activate SeqID check in hardware by setting the own master   */
/*              MAC address                                                  */
/*  2010-04-28  P05.00.00.00_00.01.04.02 280410lrg001: k32gsy_SyncMsg() nach */
/*              k32gsy_SlaveSet()                                            */
/*  2010-07-20  P05.00.00.00_00.01.06.02 200710lrg001: AP00999110            */
/*              LSA_UNUSED_ARG for GSY_CFG_TRACE_MODE 0                      */
/*  2010-08-25  P05.00.00.00_00.01.07.02 250810lrg001: AP00697597 AP01021800 */
/*              German comments translated to english                        */
/*  2010-10-18  P05.00.00.00_00.01.08.03 181010lrg001: MasterStop/Sleep      */
/*              confirmation waits for MasterFrame confirmation              */
/*  2011-01-26  P05.00.00.00_00.03.16.02 260111lrg001: AP01091974 Slave      */
/*              darf sich nicht auf die eigene MAC-Adresse synchronisieren   */
/*  2011-02-16  P05.00.00.00_00.04.07.02 lrg: AP01123956 Index-Trace         */
/*  2011-06-08  P05.00.00.00_00.05.26.02 080611lrg: AP01185610 SNYC-OK       */
/*  2011-06-20  P05.00.00.00_00.05.22.02 200611lrg: Check only one interface */
/*  2012-10-25  P05.02.00.00_00.04.08.01 AP01425250:ERTEC200P:               */
/*  KRISC32 stürzt ab bei Ethernet Verbindung zu Scalance X204IRT FW V3      */
/*  2014-06-05  P05.03.00.00_00.03.07.01 1028592/1066330: FuTimeout          */
/*  2014-06-11  P05.03.00.00_00.03.07.01 1034808: watchdog Fatal Error       */
/*              k32gsy_WatchdogCbf() LSA_TRACE_LEVEL_UNEXP statt _FATAL      */
/*  2015-04-15  V06.00, Inc04, Feature FT1204141, Task 1234432               */
/*              eddp_GetSyncTime(), Ratetimeout flex. Ki anpassen            */
/*  2015-11-25  V06.00, Inc12, Task 1499164: K32_FU_FWD_IN_FW                */
/*              SyncFU-/FU-Frames statt in HW in der KRISC32-FW weiterleiten */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

//#define K32_MASTER_SEND_FU_TEST

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	11		/* K32_MODULE_ID_GSY_MSM */
#define K32_MODULE_ID 		LTRC_ACT_MODUL_ID

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#define   GSY_INCLUDE_FOR_KRISC32

#include "gsy_inc.h"
#include "gsy_int.h"

GSY_FILE_SYSTEM_EXTENSION(K32_MODULE_ID)

/*****************************************************************************/
/* Local Data                                                                */
/* ...                                                                       */
/*****************************************************************************/
#ifdef K32_MASTER_SEND_FU_TEST
extern K32_PTCP_FU_FRAME	K32_MEM_ATTR	k32_MasterFuFrame;
#endif
extern K32_PTCP_SYNC_FRAME	K32_MEM_ATTR	k32_MasterFrame[K32_INTERFACE_COUNT + 1];
extern LSA_UINT16							k32_RecvFrameSize;

/*****************************************************************************/
/* Internal function: k32gsy_FwdSetCbf()                                     */
/* Callback function for k32gsy_FwdSet()                                     */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_FwdSetCbf(
GSY_CH_K32_PTR  	pChSys,
LSA_UINT8			SyncId)
{

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_FwdSetCbf() pChSys=%x SyncId=%u New=%u",
					pChSys, SyncId , pChSys->Slave[SyncId].FwdCtrl.New);

	if (pChSys->Slave[SyncId].FwdCtrl.New)
	{
		/* A follow-up order is present:
		 * Call appropriate output function
		*/
		LSA_UINT8	*pSubdomain = pChSys->Slave[SyncId].FwdCtrl.Subdomain.Data1.Octet;
		LSA_UINT8	*pMasterMAC = pChSys->Slave[SyncId].FwdCtrl.MasterAddr.MacAdr;
		LSA_UINT8	RestartSeqId = pChSys->Slave[SyncId].FwdCtrl.RestartSeqId;

		GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_FwdSetCbf(%u) New: RestartSeqId=%u",
					SyncId, RestartSeqId);

		pChSys->Slave[SyncId].FwdCtrl.New = LSA_FALSE;
		pChSys->Slave[SyncId].FwdCtrl.Pending = LSA_TRUE;
		if (GSY_SYNCID_TIME == SyncId)
		{
			K32GSY_CHANGE_TIME(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfTime);
		}
		else if (K32_INTERFACE_ID_A == pChSys->InterfaceId)
		{
			K32GSY_CHANGE_CLOCK_A(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfClockA);
		}
		else
		{
			K32GSY_CHANGE_CLOCK_B(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfClockB);
		}
	}
	else
	{
		GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_FwdSetCbf(%u) Done.", SyncId);

		pChSys->Slave[SyncId].FwdCtrl.Pending = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_FwdSetCbf() SyncId=%u Pending=%u New=%u",
					SyncId, pChSys->Slave[SyncId].FwdCtrl.Pending , pChSys->Slave[SyncId].FwdCtrl.New);
}

/*****************************************************************************/
/* External access function: k32gsy_ChangeCbfClockA()                        */
/* Asynchronous K32GSY_CHANGE_CLOCK_A is finished                            */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_ChangeCbfClockA(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_ChangeCbfClockA() pChSys=%x SyncId=0x%02x Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	k32gsy_FwdSetCbf(pChSys, SyncId);

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_ChangeCbfClockA()");
}

/*****************************************************************************/
/* External access function: k32gsy_ChangeCbfClockB()                        */
/* Asynchronous K32GSY_CHANGE_CLOCK_B is finished                            */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_ChangeCbfClockB(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_ChangeCbfClockB() pChSys=%x SyncId=0x%02x Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	k32gsy_FwdSetCbf(pChSys, SyncId);

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_ChangeCbfClockB()");
}

/*****************************************************************************/
/* External access function: k32gsy_ChangeCbfTime()                          */
/* Asynchronous K32GSY_CHANGE_TIME is finished                               */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_ChangeCbfTime(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_TIME;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	if (!pChSys->Slave[SyncId].FwdCtrl.Pending)
	{
		if (K32_INTERFACE_VALID(K32_INTERFACE_ID_B))
		{
			pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);
		}
		if (!pChSys->Slave[SyncId].FwdCtrl.Pending)
		{
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_ChangeCbfTime() No FwdCtrl.Pending");
		}
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_ChangeCbfTime() pChSys=%x Interface=%u FwdCtrl.Pending=%u",
					pChSys, pChSys->InterfaceId, pChSys->Slave[SyncId].FwdCtrl.Pending);

	if (pChSys->Slave[SyncId].FwdCtrl.Pending)		//251109lrg001: "!" removed
	{
		k32gsy_FwdSetCbf(pChSys, SyncId);
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_ChangeCbfTime()");
}

/*****************************************************************************/
/* Internal access function: k32gsy_FwdSet()                                 */
/* Control HW forwarding in GenSync                                          */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_FwdSet(
GSY_CH_K32_PTR  	pChSys,
LSA_UINT8			SyncId,
LSA_BOOL			RestartSeqId,
GSY_PTCP_UUID		Subdomain,
GSY_MAC_ADR_TYPE	MasterAddr)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_FwdSet() pChSys=%x SyncId=%u Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	if (pChSys->Slave[SyncId].FwdCtrl.Pending)
	{
		/* Asynchronous order not yet finished:
		 * store one new order without overwriting RestartSeqId from SlaveStart
		*/
		if ((pChSys->Slave[SyncId].FwdCtrl.New) && (!pChSys->Slave[SyncId].FwdCtrl.RestartSeqId))
			pChSys->Slave[SyncId].FwdCtrl.RestartSeqId = RestartSeqId;
		pChSys->Slave[SyncId].FwdCtrl.Subdomain = Subdomain;
		pChSys->Slave[SyncId].FwdCtrl.MasterAddr = MasterAddr;

		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_FwdSet(%u) Pending: New=%u RestartSeqId stored=%u act=%u",
					SyncId, pChSys->Slave[SyncId].FwdCtrl.New, pChSys->Slave[SyncId].FwdCtrl.RestartSeqId, RestartSeqId);

		pChSys->Slave[SyncId].FwdCtrl.New = LSA_TRUE;
	}
	else
	{
		/* Call appropriate output function with callback function address
		*/
		LSA_UINT8	*pSubdomain = Subdomain.Data1.Octet;
		LSA_UINT8	*pMasterMAC = MasterAddr.MacAdr;

		GSY_SYNC_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_FwdSet(%u) fwd state change: MAC=%02x-%02x-%02x-%02x-%02x-%02x SUB=%02x...%02x RestartSeqId=%u",
					SyncId, *pMasterMAC, *(pMasterMAC+1), *(pMasterMAC+2), *(pMasterMAC+3), *(pMasterMAC+4), *(pMasterMAC+5), *pSubdomain, *(pSubdomain+15), RestartSeqId);

		pChSys->Slave[SyncId].FwdCtrl.Pending = LSA_TRUE;
		if (GSY_SYNCID_TIME == SyncId)
		{
			K32GSY_CHANGE_TIME(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfTime);
		}
		else if (K32_INTERFACE_ID_A == pChSys->InterfaceId)
		{
			K32GSY_CHANGE_CLOCK_A(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfClockA);
		}
		else
		{
			K32GSY_CHANGE_CLOCK_B(pSubdomain, pMasterMAC, RestartSeqId, k32gsy_ChangeCbfClockB);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_FwdSet() SyncId=%u RestartSeqId=%u [%x]",
					SyncId, RestartSeqId , 0);
}

/*****************************************************************************/
/* External access function: k32gsy_UpdateCbfClockA()                        */
/* Asynchronous K32GSY_UPDATE_CLOCK_A is finished                            */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_UpdateCbfClockA(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_UpdateCbfClockA() pChSys=%x SyncId=0x%02x Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	if (GSY_SLAVE_STATE_WAIT_SET != pChSys->Slave[SyncId].State)
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"*** k32gsy_UpdateCbfClockA() pChSys=%x Interface=%u: SlaveState=%u not WAIT_SET",
					pChSys, pChSys->InterfaceId, pChSys->Slave[SyncId].State);
	}
	else
	{
		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_UpdateCbfClockA(%02x) [%x] SeqId=%u: slave state change %u->SET",
				SyncId, LSA_NULL, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);

		pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SET;
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_UpdateCbfClockA()");
}

/*****************************************************************************/
/* External access function: k32gsy_UpdateCbfClockB()                        */
/* Asynchronous K32GSY_UPDATE_CLOCK_B is finished                            */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_UpdateCbfClockB(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_UpdateCbfClockB() pChSys=%x SyncId=0x%02x Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	if (GSY_SLAVE_STATE_WAIT_SET != pChSys->Slave[SyncId].State)
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"*** k32gsy_UpdateCbfClockB() pChSys=%x Interface=%u: SlaveState=%u not WAIT_SET",
					pChSys, pChSys->InterfaceId, pChSys->Slave[SyncId].State);
	}
	else
	{
		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_UpdateCbfClockB(%02x) [%x] SeqId=%u: slave state change %u->SET",
				SyncId, LSA_NULL, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);

		pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SET;
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_UpdateCbfClockB()");
}

/*****************************************************************************/
/* External access function: k32gsy_UpdateCbfTime()                          */
/* Asynchronous K32GSY_UPDATE_TIME is finished                               */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_UpdateCbfTime(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_TIME;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	if (!pChSys->Slave[SyncId].TimeSetPending)
	{
		if (K32_INTERFACE_VALID(K32_INTERFACE_ID_B))
		{
			pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);
		}
		if (!pChSys->Slave[SyncId].TimeSetPending)
		{
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_UpdateCbfTime() No TimeSetPending");
		}
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_UpdateCbfTime() pChSys=%x Interface=%u TimeSetPending=%u",
					pChSys, pChSys->InterfaceId, pChSys->Slave[SyncId].TimeSetPending);

	if (GSY_SLAVE_STATE_WAIT_SET != pChSys->Slave[SyncId].State)
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"*** k32gsy_UpdateCbfTime() pChSys=%x Interface=%u: SlaveState=%u not WAIT_SET",
					pChSys, pChSys->InterfaceId, pChSys->Slave[SyncId].State);
	}
	else
	{
		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_UpdateCbfTime(%02x) [%x] SeqId=%u: slave state change %u->SET",
				SyncId, LSA_NULL, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);

		pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SET;
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_UpdateCbfTime()");
}


/*****************************************************************************/
/* External access function: k32gsy_WatchdogCbfClockA()                      */
/* Watchdog for monitoring reception of sync frames has expired.             */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_WatchdogCbfClockA(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	k32gsy_WatchdogCbf(pChSys, SyncId);
}

/*****************************************************************************/
/* External access function: k32gsy_WatchdogCbfClockB()                      */
/* Watchdog for monitoring reception of sync frames has expired.             */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_WatchdogCbfClockB(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_CLOCK;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);

	k32gsy_WatchdogCbf(pChSys, SyncId);
}

/*****************************************************************************/
/* External access function: k32gsy_WatchdogCbfTime()                        */
/* Watchdog for monitoring reception of sync frames has expired.             */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_WatchdogCbfTime(LSA_VOID)
{
	LSA_UINT8 SyncId = GSY_SYNCID_TIME;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_A);

	if (!pChSys->Slave[SyncId].TimeWdPending)
	{
		if (K32_INTERFACE_VALID(K32_INTERFACE_ID_B))
		{
			pChSys = k32gsy_SysPtrGet(K32_INTERFACE_ID_B);
		}
	}
	if (!pChSys->Slave[SyncId].TimeWdPending)
	{
		GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_UpdateCbfTime() No TimeWdPending");
	}
	else
	{
		pChSys->Slave[SyncId].TimeWdPending = LSA_FALSE;
		k32gsy_WatchdogCbf(pChSys, SyncId);
	}
}

/*****************************************************************************/
/* Internal function: k32gsy_WatchdogStart()                                 */
/* Start Watchdog for monitoring reception of sync frames                    */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_WatchdogStart(
GSY_CH_K32_PTR  pChSys,
LSA_UINT8		SyncId,
LSA_BOOL		Restart)
{
	LSA_UINT32 WatchdogMillis = 0;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_WatchdogStart() pChSys=%x SyncId=%u Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	if ((Restart)
	&& (0 != pChSys->Slave[SyncId].SyncTimeout))
	{
		/* Difference time from Takeover until Timeout
		*/
		WatchdogMillis = pChSys->Slave[SyncId].SyncTimeout - pChSys->Slave[SyncId].SyncTakeover;
	}
	else
	{
		/* Time until Takeover
		*/
		WatchdogMillis = pChSys->Slave[SyncId].SyncTakeover;
		pChSys->Slave[SyncId].SumTimeout = 0;

		if (0 == WatchdogMillis)
		{
			/* No Takeover: time until Timeout
			*/
			WatchdogMillis = pChSys->Slave[SyncId].SyncTimeout;
		}
	}

	GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - k32gsy_WatchdogStart(%u) If=%u Restart=%u Millis=%u",
					SyncId , pChSys->InterfaceId, Restart, WatchdogMillis);

	if (GSY_SYNCID_TIME == SyncId)
	{
		pChSys->Slave[SyncId].TimeWdPending = LSA_FALSE;
		K32GSY_WD_STOP_TIME();
		if (0 != WatchdogMillis)
		{
			pChSys->Slave[SyncId].TimeWdPending = LSA_TRUE;
			K32GSY_WD_START_TIME(WatchdogMillis, k32gsy_WatchdogCbfTime);
		}
	}
	else if (K32_INTERFACE_ID_A == pChSys->InterfaceId)
	{
		K32GSY_WD_STOP_CLOCK_A();
		if (0 != WatchdogMillis)
		{
			K32GSY_WD_START_CLOCK_A(WatchdogMillis, k32gsy_WatchdogCbfClockA);
		}
	}
	else
	{
		K32GSY_WD_STOP_CLOCK_B();
		if (0 != WatchdogMillis)
		{
			K32GSY_WD_START_CLOCK_B(WatchdogMillis, k32gsy_WatchdogCbfClockB);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_WatchdogStart() SyncId=%u Millis=%u Restart=%u",
					SyncId, WatchdogMillis , Restart);
}

/*****************************************************************************/
/* Internal function: k32gsy_WatchdogCbf()                                   */
/* Handle Watchdog expiration                                                */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_WatchdogCbf(
GSY_CH_K32_PTR  pChSys,
LSA_UINT8		SyncId)
{
	LSA_BOOL	DiagSend = LSA_FALSE;
	K32_PARAMS_TYPE_GSY_DIAG Diag = { GSY_NO_MASTER_ADDR,
										0,0,0,0,0, 0};
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_WatchdogCbf() pChSys=%x SyncId=%u Interface=%u",
					pChSys, SyncId , pChSys->InterfaceId);

	if (GSY_SLAVE_STATE_OFF == pChSys->Slave[SyncId].State)
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_WatchdogCbf() pChSys=%x SyncId=0x%02x Interface=%u: no slave running",
					pChSys, SyncId , pChSys->InterfaceId);
	}
	else if ((GSY_SLAVE_STATE_WAIT == pChSys->Slave[SyncId].State)
		 &&  GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr))		//080611lrg
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_WatchdogCbf() pChSys=%x SyncId=0x%02x Interface=%u: slave is already waiting",
					pChSys, SyncId , pChSys->InterfaceId);
	}
	else if (GSY_MASTER_STATE_PRIMARY == pChSys->Master[SyncId].State)
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_WatchdogCbf() pChSys=%x SyncId=0x%02x Interface=%u: master is primary",
					pChSys, SyncId , pChSys->InterfaceId);
	}
	else
	{
		pChSys->Slave[SyncId].SumTimeout++;

		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_WatchdogCbf(%u) SumTimeout=%u SyncTakeover=%u SyncTimeout=%u",
					SyncId, pChSys->Slave[SyncId].SumTimeout, pChSys->Slave[SyncId].SyncTakeover, pChSys->Slave[SyncId].SyncTimeout);

		if ((0 != pChSys->Slave[SyncId].SyncTakeover)
		&&  (1 == pChSys->Slave[SyncId].SumTimeout))
		{
			/* Takeover: prepare to accept a new master
			*/
			if (!GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr)
			||  (pChSys->Master[SyncId].State == GSY_MASTER_STATE_SLAVE)
			||  (pChSys->Master[SyncId].State == GSY_MASTER_STATE_STARTUP))
			{
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- k32gsy_WatchdogCbf(%02x) Offset=%d SeqId=%u: slave state change %u->TAKEOVER",
									SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);

				/* Set master MAC address to NULL
				*/
				pChSys->Slave[SyncId].MasterAddr = NoMacAddr;
				if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Slave[SyncId].pDrift, LSA_NULL))
				{
					pChSys->Slave[SyncId].pDrift->MasterAddr = NoMacAddr;
				}
				DiagSend = LSA_TRUE;
			}
			if (pChSys->Slave[SyncId].SyncTimeout > pChSys->Slave[SyncId].SyncTakeover)
			{
				/* Restart Watchdog with the remaining time until Timeout
				*/
				k32gsy_WatchdogStart(pChSys, SyncId, LSA_TRUE);
			}
		}
		else
		{
			/* Timeout
			*/
			if (!GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr))
			{
				/* Set master MAC address to NULL
				*/
				pChSys->Slave[SyncId].MasterAddr = NoMacAddr;
				DiagSend = LSA_TRUE;
			}
			if (GSY_SLAVE_STATE_WAIT != pChSys->Slave[SyncId].State)
			{
				/* Change slave state to WAIT
				*/
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- k32gsy_WatchdogCbf(%02x) Offset=%d SeqId=%u: slave state change %u->WAIT",
								SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);
				DiagSend = LSA_TRUE;
				pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_WAIT;
			}
			else if (DiagSend)
			{
				/* 080611lrg: Timeout between NewMaster Sync- and FU frame
				*/
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- k32gsy_WatchdogCbf(%02x) Offset=%d SeqId=%u: slave state change [%u] MasterMAC->NIL",
								SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);
			}
			pChSys->Slave[SyncId].AveSumCount = 0;
			pChSys->Slave[SyncId].SumTimeout = 0;

			/* Send NOT SYNC to EDDP
			*/
			k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);
		}

		/* Release master MAC address for HW forwarding
		*/
		k32gsy_FwdSet(pChSys, SyncId, LSA_FALSE, pChSys->Slave[SyncId].Subdomain, NoMacAddr);

		if (DiagSend)
		{
			/* Indicate diagnosis
			*/
			Diag.SlaveState = pChSys->Slave[SyncId].State;
			Diag.SyncId = SyncId;
			Diag.DiagSrc = GSY_DIAG_SOURCE_MASTER;
            K32_COPY_MAC_ADDR_ARRAY (Diag.MasterMacAddr, pChSys->Slave[SyncId].MasterAddr.MacAdr);
            Diag.RcvSyncPrio = pChSys->Drift[SyncId].RcvSyncPrio;
			Diag.RateInterval = pChSys->Drift[SyncId].AveInterval;
			Diag.SequenceId = pChSys->Slave[SyncId].SeqId;
			pChSys->Drift[SyncId].Diag.pK32Rqb->Params.DiagInd = Diag;
			pChSys->Drift[SyncId].Diag.pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
			K32GSY_CALLBACK(pChSys->Drift[SyncId].Diag.pK32Rqb);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_WatchdogCbf() [%x%x] SumTimeout=%u",
					0, 0 , pChSys->Slave[SyncId].SumTimeout);
}

/*****************************************************************************/
/* Internal function: k32gsy_SyncReset()                                     */
/* Reset controller and set OCF correction to the given RateInterval         */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SyncReset(
LSA_INT32		RateInterval,
LSA_UINT8		SyncId,
GSY_CH_K32_PTR	pChSys)
{
	LSA_UINT32	OffsetCompensation;
	LSA_UINT16	RetVal = LSA_RET_OK;
	GSY_SYNC_CONTROLLER_TYPE *pCtrl = &pChSys->Slave[SyncId].Ctrl;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncReset() pChSys=%x SyncId=%u RateInterval=%d",
					pChSys, SyncId , RateInterval);

	/* Reset controller
	*/
	RetVal = gsy_ClpReset(RateInterval, SyncId, pCtrl);
	if (LSA_RET_OK != RetVal)
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReset() SyncId=%x RateInterval=%d: gsy_SyncReset: RetVal=0x%x",
						SyncId, RateInterval, RetVal);
	}
	else
	{
		if (RateInterval >= 0)
			OffsetCompensation = (LSA_UINT32)RateInterval;
		else
			OffsetCompensation = (LSA_UINT32)-RateInterval;

		if (OffsetCompensation > K32_COMP_INTERVAL)
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncReset() SyncId=%u RateInterval=%d: invalid OffsetCompensation=%u",
							SyncId, RateInterval, OffsetCompensation);
		}
		else
		{
			/* Set RCF interval as OCF interval in PNIP
			*/
			OffsetCompensation &= K32_COMP_INTERVAL;
			OffsetCompensation |= K32_COMP_VALID;
			if (RateInterval >= 0)
			{
				OffsetCompensation |= K32_COMP_PLUS;
			}
			if (GSY_SYNCID_TIME == SyncId)
			{
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncReset(%02x) RateInterval=%u OffsetCompensation=0x%x Param_Ax1000=%u",
					SyncId, RateInterval, OffsetCompensation, pCtrl->TimeCtrl.Param_Ax1000);

				K32GSY_OCF_SET_TIME(OffsetCompensation);
			}
			else
			{
				GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncReset(%02x) RateInterval=%u OffsetCompensation=0x%x Kp=%u Ki=%u",
					SyncId, RateInterval, OffsetCompensation, pCtrl->CycleCtrl.Kp_1perSec, pCtrl->CycleCtrl.Ki_1perSecSec);

				if	(K32_INTERFACE_ID_A == pChSys->InterfaceId)
				{
					K32GSY_OCF_SET_CLOCK_A(OffsetCompensation);
				}
				else
				{
					K32GSY_OCF_SET_CLOCK_B(OffsetCompensation);
				}
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncReset() [%x%x] RetVal=0x%x",
					0, 0 , RetVal);
}

/*****************************************************************************/
/* Internal function: k32gsy_SyncAlgorithm()                                 */
/* Call controller to calculate correction value for the actual PTCP time    */
/* and set it as OCF interval.                                               */
/*****************************************************************************/
K32_STATIC LSA_UINT16 K32_LOCAL_FCT_ATTR k32gsy_SyncAlgorithm(
LSA_UINT32						Seconds,
LSA_UINT32						NanoSeconds,
LSA_INT32						AveTimeOffset,
LSA_INT32	K32_MEM_ATTR	*	pAdjustInterval,
LSA_INT64   K32_MEM_ATTR	*	pProportionalTerm,
LSA_INT64   K32_MEM_ATTR	*	pIntegralTerm,
LSA_UINT8						SyncId,
GSY_CH_K32_PTR					pChSys)
{
	LSA_UINT32	OffsetCompensation;
	LSA_UINT16	RetVal = LSA_RET_OK;
	GSY_SYNC_CONTROLLER_TYPE *pCtrl = &pChSys->Slave[SyncId].Ctrl;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncAlgorithm() pChSys=%x SyncId=%u Offset=%d",
					pChSys, SyncId , AveTimeOffset);

	/* Calculate correction interval for +- 1 nanosecond
	*/
	if (GSY_SYNCID_CLOCK == SyncId)
	{
		RetVal = gsy_SyncAlgoCycle(Seconds,NanoSeconds, AveTimeOffset, pAdjustInterval,
								pProportionalTerm, pIntegralTerm, SyncId, pCtrl);
	}
	else
	{
		LSA_BOOL SyncState = LSA_FALSE;
		if (GSY_SLAVE_STATE_SYNC == pChSys->Slave[SyncId].State) SyncState = LSA_TRUE;
		gsy_SyncAlgoTime(SyncState, AveTimeOffset, pChSys->Slave[SyncId].pDrift->AveInterval,pChSys->Slave[SyncId].SyncInterval, pAdjustInterval, pCtrl);
	}
	if (LSA_RET_OK != RetVal)
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncAlgorithm() SyncId=%x AveTimeOffset=%d: clp_SyncAlgorithm: RetVal=0x%x",
						SyncId, AveTimeOffset, RetVal);
	}
	else
	{
		GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"--- k32gsy_SyncAlgorithm(%02x) Seconds=%u Nanos=%u Offset=%d AdjustInterval=%d",
				SyncId, Seconds, NanoSeconds, AveTimeOffset, *pAdjustInterval);

		if (*pAdjustInterval >= 0)
			OffsetCompensation = (LSA_UINT32)*pAdjustInterval;
		else
			OffsetCompensation = (LSA_UINT32)-(*pAdjustInterval);
		if (OffsetCompensation > K32_COMP_INTERVAL)
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncAlgorithm() SyncId=%u AdjustInterval=%d: invalid OffsetCompensation=%u",
							SyncId, *pAdjustInterval, OffsetCompensation);
		}
		else
		{
			/* Set OCF interval in PNIP
			*/
			OffsetCompensation &= K32_COMP_INTERVAL;
			OffsetCompensation |= K32_COMP_VALID;
			if (*pAdjustInterval >= 0)
			{
				OffsetCompensation |= K32_COMP_PLUS;
			}
			if (GSY_SYNCID_TIME == SyncId)
			{
				K32GSY_OCF_SET_TIME(OffsetCompensation);
			}
			else if	(K32_INTERFACE_ID_A == pChSys->InterfaceId)
			{
				K32GSY_OCF_SET_CLOCK_A(OffsetCompensation);
			}
			else
			{
				K32GSY_OCF_SET_CLOCK_B(OffsetCompensation);
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncAlgorithm() [%x] AdjustInterval=%d RetVal=0x%x",
					0, *pAdjustInterval , RetVal);

	return(RetVal);
}

/*****************************************************************************/
/* Internal function: k32gsy_SyncMsg()                                       */
/* Send change of clock sync state to EDDP                                   */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SyncMsg(
GSY_CH_K32_PTR	pChSys,
LSA_UINT8		SyncId,
LSA_BOOL		SyncOk)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncMsg() pChSys=%x SyncId=%u SyncOk=%u",
					pChSys, SyncId, SyncOk);

	if (((GSY_SYNCID_TIME == SyncId) && (GSY_MASTER_STATE_PRIMARY != pChSys->Master[SyncId].State))
	||  ((GSY_SYNCID_CLOCK == SyncId) && (pChSys->pK32SyncRqb->Params.SyncInd.SyncOk != SyncOk)))
	{
		if (GSY_SYNCID_TIME == SyncId)
		{
			/* Update time slave state and parameters
			*/
			pChSys->pK32SyncRqb->Params.SyncInd.UTCOffset = pChSys->Slave[SyncId].CurrentUTCOffset;
			pChSys->pK32SyncRqb->Params.SyncInd.SecondsHigh = pChSys->Slave[SyncId].Act.OrgEpoch;
			switch (pChSys->Slave[SyncId].State)
			{
			default:
				pChSys->pK32SyncRqb->Params.SyncInd.SyncState = EDDP_K32_SYNC_STATE_JITTER_OUT_OF_BOUNDARY;
				break;
			case GSY_SLAVE_STATE_WAIT:
				pChSys->pK32SyncRqb->Params.SyncInd.SyncState = EDDP_K32_SYNC_STATE_NO_MESSAGE_RECEIVED;
				break;
			case GSY_SLAVE_STATE_SYNC:
			case GSY_SLAVE_STATE_SYNC_OUT:
				pChSys->pK32SyncRqb->Params.SyncInd.SyncState = EDDP_K32_SYNC_STATE_OK;
				break;
			case GSY_SLAVE_STATE_STOP:
				pChSys->pK32SyncRqb->Params.SyncInd.SyncState = EDDP_K32_SYNC_STATE_OFF;
				break;
			}
			GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncMsg(01) SyncState=%u SecondsHigh=%u UTCOffset=%u",
				pChSys->pK32SyncRqb->Params.SyncInd.SyncState,pChSys->pK32SyncRqb->Params.SyncInd.SecondsHigh, pChSys->pK32SyncRqb->Params.SyncInd.UTCOffset);
		}
		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncMsg(%02u) SlaveState=%u MasterState=%u sync state change ->%u",
				SyncId, pChSys->Slave[SyncId].State, pChSys->Master[SyncId].State, SyncOk);
		pChSys->pK32SyncRqb->Params.SyncInd.SyncId = SyncId;
		pChSys->pK32SyncRqb->Params.SyncInd.SyncOk = SyncOk;
		pChSys->pK32SyncRqb->Hdr.InterfaceID = pChSys->InterfaceId;
		K32GSY_CALLBACK(pChSys->pK32SyncRqb);
	}
	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncMsg()");
	return;
}

/*****************************************************************************/
/* Internal function: k32gsy_SyncUpdate()                                    */
/* Update sync parameters while slave/master is running                      */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SyncUpdate(
GSY_CH_K32_PTR			pChSys,
LSA_UINT8				SyncId,
LSA_UINT32				PLLWindow,
LSA_UINT16				SyncTimeoutFactor,
LSA_UINT16				SyncTakeoverFactor,
K32GSY_TLV_MASTER_PTR	pTLVMaster)
{
	LSA_UINT32	NewSyncTimeout;
	LSA_UINT32	NewSyncTakeover;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncUpdate() pChSys=%x SyncId=%u pTLVMaster=%x",
					pChSys, SyncId, pTLVMaster);

	/* Calculate new timeout values
	*/
	if (GSY_SYNC_TIMEOUT_DISABLED != SyncTimeoutFactor)
	{
		pChSys->Slave[SyncId].SyncInTime = pChSys->Slave[SyncId].SyncInterval * SyncTimeoutFactor;
		NewSyncTimeout = pChSys->Slave[SyncId].SyncInTime;
	}
	else
	{
		pChSys->Slave[SyncId].SyncInTime = 0;
		NewSyncTimeout = 0;
	}

	if ((GSY_SYNC_TAKEOVER_DISABLED == SyncTakeoverFactor)
	||  ((GSY_SYNC_TIMEOUT_DISABLED != SyncTimeoutFactor)
	  && (SyncTakeoverFactor >= SyncTimeoutFactor)))
	{
		/* Takeover disabled or Timeout prior to Takeover -> no Takeover
		*/
		NewSyncTakeover = 0;
	}
	else
	{
		NewSyncTakeover = pChSys->Slave[SyncId].SyncInterval * SyncTakeoverFactor;
	}

	GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncUpdate(%02x) Timeout=%u->%u Takeover=%u->%u Window=%u->%u slave state change ->UPDATE",
						SyncId, pChSys->Slave[SyncId].SyncTimeout, NewSyncTimeout, pChSys->Slave[SyncId].SyncTakeover, NewSyncTakeover, pChSys->Slave[SyncId].WindowSize, PLLWindow);

	pChSys->Slave[SyncId].WindowSize = PLLWindow;

	if ((pChSys->Slave[SyncId].SyncTimeout != NewSyncTimeout)
	||	(pChSys->Slave[SyncId].SyncTakeover != NewSyncTakeover))
	{
		/* Update Timeout values and restart Watchdog
		*/
		pChSys->Slave[SyncId].SyncTimeout = NewSyncTimeout;
		pChSys->Slave[SyncId].SyncTakeover = NewSyncTakeover;
		k32gsy_WatchdogStart(pChSys, SyncId, LSA_FALSE);
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(pTLVMaster, LSA_NULL))
	{
		/* Master TLV update
		*/
		pChSys->Master[SyncId].MasterTLV = *pTLVMaster;
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncUpdate()");
	return;
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveStart()                                    */
/* Start a slave enity for the SyncId                                        */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveStart(
GSY_CH_K32_PTR	pChSys,
LSA_UINT8		SyncId,
LSA_UINT32		PLLWindow,
LSA_UINT32		SyncInterval,
LSA_UINT16		SyncTimeout,
LSA_UINT16		SyncTakeover)
{
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveStart() pChSys=%x SyncId=%u SyncInterval=%u",
					pChSys, SyncId, SyncInterval);

	/* Set slave state and parameters
	*/

	pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_FIRST;
	pChSys->Slave[SyncId].SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED;
	pChSys->Slave[SyncId].MasterAddr = NoMacAddr;
	pChSys->Slave[SyncId].AdjustSeconds = 0;
	pChSys->Slave[SyncId].AdjustNanos = 0;
	pChSys->Slave[SyncId].SeqId = 0;
	pChSys->Slave[SyncId].SyncPortId = 0;
	pChSys->Slave[SyncId].WaitFU = LSA_FALSE;
	pChSys->Slave[SyncId].SumTimeout = 0;
	pChSys->Slave[SyncId].FuTimeout = 0;
	pChSys->Slave[SyncId].AdjustTermP = 0;
	pChSys->Slave[SyncId].AdjustTermI = 0;

	pChSys->Slave[SyncId].WindowSize = PLLWindow;
	pChSys->Slave[SyncId].SyncInterval = (LSA_UINT16)SyncInterval;
	pChSys->Slave[SyncId].SyncOutTime = 0;

	if (GSY_SYNC_TIMEOUT_DISABLED != SyncTimeout)
	{
		pChSys->Slave[SyncId].SyncInTime = (LSA_UINT16)SyncInterval * SyncTimeout;
		pChSys->Slave[SyncId].SyncTimeout = SyncInterval * SyncTimeout;
	}
	else
	{
		pChSys->Slave[SyncId].SyncInTime = 0;
		pChSys->Slave[SyncId].SyncTimeout = 0;
	}


	if ((GSY_SYNC_TAKEOVER_DISABLED == SyncTakeover)
	||  ((GSY_SYNC_TIMEOUT_DISABLED != SyncTimeout)
	  && (SyncTakeover >= SyncTimeout)))
	{
		/* Takeover disabled or Timeout prior to Takeover -> no Takeover
		*/
		pChSys->Slave[SyncId].SyncTakeover = 0;
	}
	else
	{
		pChSys->Slave[SyncId].SyncTakeover = SyncInterval * SyncTakeover;
	}

	//260111lrg001: MAC added
	GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveStart(%02x) Interval=%u Timeout=%u Takeover=%u Window=%u MAC=...%02x-%02x-%02x: slave state change ->START",
					SyncId, SyncInterval, SyncTimeout, SyncTakeover, PLLWindow, pChSys->MACAddr.MacAdr[3], pChSys->MACAddr.MacAdr[4], pChSys->MACAddr.MacAdr[5]);

	/* Reset rate and controller, start Watchdog and
	 * set Subdomain for HW forwarding
	*/
	k32gsy_DriftReset(pChSys, &pChSys->Drift[SyncId], SyncId);
	k32gsy_SyncReset(0, SyncId, pChSys);
	k32gsy_WatchdogStart(pChSys, SyncId, LSA_FALSE);
	k32gsy_FwdSet(pChSys, SyncId, LSA_TRUE, pChSys->Slave[SyncId].Subdomain, NoMacAddr);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveStart() Timeout=%u Takeover=%u Window=%u",
					SyncTimeout, SyncTakeover, PLLWindow);
	return;
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveStop()                                     */
/* Terminate slave enity                                                     */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveStop(
GSY_CH_K32_PTR	pChSys,
LSA_UINT8		SyncId)
{
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveStop() pChSys=%x SyncId=%u SlaveState=0x%x",
					pChSys, SyncId , pChSys->Slave[SyncId].State);

	GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveStop(%02x) slave state change ->STOP/OFF", SyncId);
	pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_STOP;

	/* Reset rate and controller, stop Watchdog and
	 * release Subdomain for HW forwarding
	*/
	k32gsy_FwdSet(pChSys, SyncId, LSA_FALSE, pChSys->Slave[SyncId].Subdomain, NoMacAddr);
	k32gsy_DriftReset(pChSys, &pChSys->Drift[SyncId], SyncId);
	k32gsy_SyncReset(0, SyncId, pChSys);

	if (GSY_SYNCID_TIME == SyncId)
	{
		K32GSY_WD_STOP_TIME();
	}
	else if (K32_INTERFACE_ID_A == pChSys->InterfaceId)
	{
		K32GSY_WD_STOP_CLOCK_A();
	}
	else
	{
		K32GSY_WD_STOP_CLOCK_B();
	}
	/* Send NOT SYNC to EDDP
	*/
	k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);
	pChSys->Slave[SyncId].SyncTimeout = 0;
	pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_OFF;

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveStop()");
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveSet()                                      */
/* Set local PTCP time strongly to reference time.                           */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveSet(
GSY_CH_K32_PTR	pChSys,
LSA_INT64		DiffNanos,
LSA_UINT8		SyncId)
{
	K32_PARAMS_TYPE_GSY_DIAG Diag = {GSY_NO_MASTER_ADDR,
										0,0,0,0,0, 0};

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveSet() pChSys=%x SyncId=%u DiffNanos=%d",
					pChSys, SyncId, (LSA_UINT32)DiffNanos);

	GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveSet(%02x) Offset=0x%08x.%08x SeqId=%u: slave state change %u->WAIT_SET",
			SyncId, (LSA_UINT32)(DiffNanos >> 32), (LSA_UINT32)(DiffNanos & 0xFFFFFFFF), pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);

	pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_WAIT_SET;

	if (GSY_SYNCID_TIME == SyncId)
	{
		pChSys->Slave[SyncId].TimeSetPending = LSA_TRUE;
		K32GSY_UPDATE_TIME(DiffNanos, k32gsy_UpdateCbfTime);
	}
	else if	(K32_INTERFACE_ID_A == pChSys->InterfaceId)
	{
		K32GSY_UPDATE_CLOCK_A(DiffNanos, k32gsy_UpdateCbfClockA);
	}
	else
	{
		K32GSY_UPDATE_CLOCK_B(DiffNanos, k32gsy_UpdateCbfClockB);
	}

	/* Indicate diagnosis
	*/
	Diag.SlaveState = pChSys->Slave[SyncId].State;
	Diag.SyncId = SyncId;
	Diag.DiagSrc = GSY_DIAG_SOURCE_SET;
	Diag.RcvSyncPrio = pChSys->Drift[SyncId].RcvSyncPrio;
	Diag.RateInterval = pChSys->Drift[SyncId].AveInterval;
	Diag.SequenceId = pChSys->Slave[SyncId].SeqId;
    K32_COPY_MAC_ADDR_ARRAY (Diag.MasterMacAddr, pChSys->Slave[SyncId].MasterAddr.MacAdr);
    pChSys->Drift[SyncId].Diag.pK32Rqb->Params.DiagInd = Diag;
	pChSys->Drift[SyncId].Diag.pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
	K32GSY_CALLBACK(pChSys->Drift[SyncId].Diag.pK32Rqb);

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveSet() State=%u",
					pChSys->Slave[SyncId].State);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveAdjust()                                   */
/* Synchronize local PTCP time and update slave state                        */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveAdjust(
GSY_CH_K32_PTR	pChSys,
LSA_UINT8		SyncId)
{
	LSA_INT32			DriftInterval = 0;
	LSA_UINT16			RetVal = LSA_RET_OK;
	LSA_UINT8			OldState = pChSys->Slave[SyncId].State;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveAdjust() pChSys=%x SyncId=%u Interface=%u",
					pChSys, SyncId, pChSys->InterfaceId);

	if ((GSY_SLAVE_STATE_SYNC == pChSys->Slave[SyncId].State)
	||  (GSY_SLAVE_STATE_SYNC_IN == pChSys->Slave[SyncId].State)
	||  (GSY_SLAVE_STATE_SET == pChSys->Slave[SyncId].State)
	||  (GSY_SLAVE_STATE_OUT == pChSys->Slave[SyncId].State)
	||  (GSY_SLAVE_STATE_SYNC_OUT == pChSys->Slave[SyncId].State))
	{

		/* Let control algorithm calculate adjust value for actual time and adjust PTCP time
		 * *** If AveTimeOffset is negative, the local clock is too slow ***
		 * 290606lrg001: new parameter AdjustTermP, AdjustTermI
		*/
		RetVal = k32gsy_SyncAlgorithm(pChSys->Slave[SyncId].AdjustSeconds, pChSys->Slave[SyncId].AdjustNanos,
									pChSys->Slave[SyncId].AveTimeOffset, &pChSys->Slave[SyncId].AdjustInterval,
									&pChSys->Slave[SyncId].AdjustTermP, &pChSys->Slave[SyncId].AdjustTermI,
									SyncId, pChSys);
		if (LSA_RET_OK == RetVal)
		{
			/* Get actual drift for RESET und Trace
			*/
			if (GSY_DRIFT_STATE_READY == pChSys->Slave[SyncId].pDrift->State)
				DriftInterval = pChSys->Slave[SyncId].pDrift->AveInterval;

			if  ((0 == pChSys->Slave[SyncId].WindowSize)
			||   ((pChSys->Slave[SyncId].AveTimeOffset <= (LSA_INT32)pChSys->Slave[SyncId].WindowSize)
			 &&  (pChSys->Slave[SyncId].AveTimeOffset >= -((LSA_INT32)pChSys->Slave[SyncId].WindowSize))))
			{
				/* Offset is inside of PLL-Window
				*/
				switch (pChSys->Slave[SyncId].State)
				{
				case GSY_SLAVE_STATE_OUT:
				case GSY_SLAVE_STATE_SET:

					if (pChSys->Slave[SyncId].pDrift->State == GSY_DRIFT_STATE_READY)
					{
						/* New SyncState is: sync_in (200207lrg001)
						*/
						GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change %u->SYNC_IN",
								SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].State);
						pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SYNC_IN;
						pChSys->Slave[SyncId].SyncInAct = 0;
					}
					break;

				case GSY_SLAVE_STATE_SYNC_IN:
					pChSys->Slave[SyncId].SyncInAct += pChSys->Slave[SyncId].SyncInterval;
					if (pChSys->Slave[SyncId].SyncInAct >= pChSys->Slave[SyncId].SyncInTime)
					{
						/* When the SyncIn time is reached,
						 * SyncState changes to: synchronous
						*/
						GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_IN->SYNC",
										SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId);
						pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SYNC;

						/* Send SYNC to EDDP
						*/
						k32gsy_SyncMsg(pChSys, SyncId, LSA_TRUE);

					}
					break;

				case GSY_SLAVE_STATE_SYNC:
					/* SyncState remains: synchronous
					*/
					break;

				case GSY_SLAVE_STATE_SYNC_OUT:
					/* New SyncState is again: synchronous
					*/
					GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_OUT->SYNC",
										SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId);
					pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SYNC;
					pChSys->Slave[SyncId].SyncOutAct = 0;
					break;

				default:
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_SlaveAdjust() pChSys=%x SyncId=0x%02x: Invalid slave state=%u",
											pChSys, SyncId, pChSys->Slave[SyncId].State);

				}
			}
			else
			{
				/* Offset is out of PLL-Window
				*/
				switch (pChSys->Slave[SyncId].State)
				{
				case GSY_SLAVE_STATE_SYNC:
					/* New SyncState is: sync_out
					*/
					GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC->SYNC_OUT",
										SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId);
					pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SYNC_OUT;
					pChSys->Slave[SyncId].SyncOutAct = 0;
					if  (pChSys->Slave[SyncId].AveTimeOffset < (LSA_INT32)pChSys->Slave[SyncId].WindowSize)
						pChSys->Slave[SyncId].Statistics.PLLWindowUnderun++;
					else
						pChSys->Slave[SyncId].Statistics.PLLWindowOverrun++;
					break;

				case GSY_SLAVE_STATE_SYNC_OUT:
					pChSys->Slave[SyncId].SyncOutAct += pChSys->Slave[SyncId].SyncInterval;
					if (pChSys->Slave[SyncId].SyncOutAct >= pChSys->Slave[SyncId].SyncOutTime)
					{
						/* When the SyncOut time is reached,
						 * SyncState changes to: not synchronous
						*/
						GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_OUT->OUT",
										SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId);
						pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_OUT;

						/* Send NOT SYNC to EDDP
						*/
						k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);

						/* Reset controller
						*/
						k32gsy_SyncReset(DriftInterval, SyncId, pChSys);
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
					GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SlaveAdjust(%02x) Offset=%d SeqId=%u: slave state change SYNC_IN->OUT",
										SyncId, pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].SeqId);
					pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_OUT;
					pChSys->Slave[SyncId].SyncInAct = 0;

					/* Reset controller
					*/
					k32gsy_SyncReset(DriftInterval, SyncId, pChSys);
					break;

				default:
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SlaveAdjust() pChSys=%x SyncId=0x%02x: Invalid slave state=%u",
											pChSys, SyncId, pChSys->Slave[SyncId].State);
				}
			}
		}
		GSY_SYNC_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--+ k32gsy_SlaveAdjust(%02x) OrgS.OrgN=%u.%09u DelayN=%09u OCFInterval=%d AveOffset=%d RCFInterval=%d SlaveState=%u->%u SeqId=%u",
			SyncId, pChSys->Slave[SyncId].Act.OrgSeconds, pChSys->Slave[SyncId].Act.OrgNanos, pChSys->Slave[SyncId].Act.DelayNanos,
			pChSys->Slave[SyncId].AdjustInterval, pChSys->Slave[SyncId].AveTimeOffset, DriftInterval, OldState, pChSys->Slave[SyncId].State, pChSys->Slave[SyncId].SeqId);

		if (OldState != pChSys->Slave[SyncId].State)
		{
			/* On state change: indicate diagnosis
			*/
			K32_PARAMS_TYPE_GSY_DIAG Diag = { GSY_NO_MASTER_ADDR,
											  0,0,0,0,0,0};

			Diag.SlaveState = pChSys->Slave[SyncId].State;
			Diag.SyncId = SyncId;
			Diag.DiagSrc = GSY_DIAG_SOURCE_ADJUST;
			Diag.RcvSyncPrio = pChSys->Drift[SyncId].RcvSyncPrio;
			Diag.RateInterval = pChSys->Drift[SyncId].AveInterval;
			Diag.SequenceId = pChSys->Slave[SyncId].SeqId;
            K32_COPY_MAC_ADDR_ARRAY (Diag.MasterMacAddr, pChSys->Slave[SyncId].MasterAddr.MacAdr);
            pChSys->Drift[SyncId].Diag.pK32Rqb->Params.DiagInd = Diag;
			pChSys->Drift[SyncId].Diag.pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
			K32GSY_CALLBACK(pChSys->Drift[SyncId].Diag.pK32Rqb);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveAdjust() AveTimeOffset=%d AdjustInterval=%d SlaveState=%d",
						pChSys->Slave[SyncId].AveTimeOffset, pChSys->Slave[SyncId].AdjustInterval, pChSys->Slave[SyncId].State);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveAction()                                   */
/* Calculate local offset to master time.                                    */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveAction(
GSY_CH_K32_PTR			pChSys,
LSA_UINT8				SyncId)
{
	LSA_UINT32	ActSeconds, ActNanos;
	LSA_UINT32	RefSeconds, RefNanos;
	LSA_INT64	DiffSeconds,DiffNanos;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveAction() pChSys=%x SyncId=%u Interface=%u",
					pChSys, SyncId, pChSys->InterfaceId);

	/* Build reference time by adding all delays til frame receive to master time
	*/
	RefSeconds = pChSys->Slave[SyncId].Act.OrgSeconds + pChSys->Slave[SyncId].Act.DelaySeconds;
	RefNanos = pChSys->Slave[SyncId].Act.OrgNanos + pChSys->Slave[SyncId].Act.DelayNanos;

	while (RefNanos >= GSY_NANOS_PER_SECOND)
	{
		RefNanos -= GSY_NANOS_PER_SECOND;
		RefSeconds++;
	}

	/* Build local frame receive time
	*/
	ActSeconds = pChSys->Slave[SyncId].Act.LocalSeconds;
	ActNanos = pChSys->Slave[SyncId].Act.LocalNanos;
	while (ActNanos >= GSY_NANOS_PER_SECOND)
	{
		ActNanos -= GSY_NANOS_PER_SECOND;
		ActSeconds++;
	}

	/* Calculate deviation of local frame receive time to master reference time
	 * 180609lrg001: calculation is wrong without cast (LSA_INT64)
	*/
	DiffSeconds = (LSA_INT64)ActSeconds - (LSA_INT64)RefSeconds;
	DiffNanos = (LSA_INT64)ActNanos - (LSA_INT64)RefNanos;
	if ((DiffSeconds < 0) && (DiffNanos > 0))
	{
		DiffSeconds++;
		DiffNanos -= GSY_NANOS_PER_SECOND;
	}
	else if ((DiffSeconds > 0) && (DiffNanos < 0))
	{
		DiffSeconds--;
		DiffNanos += GSY_NANOS_PER_SECOND;
	}
	if (DiffSeconds != 0)
	{
		DiffNanos += DiffSeconds * GSY_NANOS_PER_SECOND;
	}

	GSY_SYNC_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--= k32gsy_SlaveAction(%02x) OrgS=%u OrgN=%09u DelayN=%09u RefS=%u RefN=%09u ActS=%09u ActN=%09u DiffS=%d DiffN=%d",
			SyncId, pChSys->Slave[SyncId].Act.OrgSeconds, pChSys->Slave[SyncId].Act.OrgNanos, pChSys->Slave[SyncId].Act.DelayNanos,
			RefSeconds, RefNanos, ActSeconds, ActNanos, (LSA_UINT32)DiffSeconds, (LSA_UINT32)DiffNanos);


	if ((GSY_SLAVE_STATE_WAIT == pChSys->Slave[SyncId].State)
	||  ((GSY_SLAVE_STATE_FIRST == pChSys->Slave[SyncId].State) && (GSY_MASTER_STATE_SLAVE != pChSys->Master[SyncId].State))
	||  (DiffSeconds != 0)
	||  (DiffNanos >  (LSA_INT64)(GSY_SYNC_WINDOW_RESET_FACTOR * pChSys->Slave[SyncId].WindowSize))
	||  (DiffNanos < -(LSA_INT64)(GSY_SYNC_WINDOW_RESET_FACTOR * pChSys->Slave[SyncId].WindowSize)))
	{
		/* Set new local time after timeout or at first SyncFrame from master
		 * or offset is >= 1 second
		 * or offset is > GSY_SYNC_WINDOW_RESET_FACTOR * PLLwindowSize
		*/
		if (GSY_DRIFT_STATE_READY == pChSys->Slave[SyncId].pDrift->State)
		{
			LSA_UINT8 OldState = pChSys->Slave[SyncId].State;	//280410lrg001

			/* If the local drift interval has already been calculated...
			*/
			k32gsy_SyncReset(pChSys->Slave[SyncId].pDrift->AveInterval, SyncId, pChSys);

			/* Set local time to master reference time
			*/
			k32gsy_SlaveSet(pChSys, -DiffNanos, SyncId);

			/* 280410lrg001: Send NOT SYNC to EDDP
			*/
			if ((GSY_SLAVE_STATE_SYNC == OldState) || (GSY_SLAVE_STATE_SYNC_OUT == OldState) 
			||  (GSY_SYNCID_TIME == SyncId))
			{
				k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);
			}

			/* Reset offset avarage value
			*/
			pChSys->Slave[SyncId].AveTimeOffset = 0;
			pChSys->Slave[SyncId].AveSumOffset = 0;
			pChSys->Slave[SyncId].AveSumCountLast = 0;
			pChSys->Slave[SyncId].AveSumCount = 0;
			pChSys->Slave[SyncId].AveMaxOffset = 0;
			pChSys->Slave[SyncId].AveMinOffset = 0;
			pChSys->Slave[SyncId].Statistics.FrameOk++;
		}
		else
		{
	       GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** k32gsy_SlaveAction() SyncId=0x%02x: No RCFInterval",
		    				SyncId);
		}
	}
	else if ((GSY_SLAVE_STATE_FIRST == pChSys->Slave[SyncId].State) 
	     && (GSY_MASTER_STATE_SLAVE == pChSys->Master[SyncId].State))
	{
		/* 020708lrg001: Transition from master to slave and offset < GSY_SYNC_WINDOW_RESET_FACTOR * PLL-Window
		*/
		pChSys->Slave[SyncId].AveTimeOffset = (LSA_INT32)DiffNanos;
		pChSys->Slave[SyncId].AveMaxOffset = (LSA_INT32)DiffNanos;
		pChSys->Slave[SyncId].AveMinOffset = (LSA_INT32)DiffNanos;
		pChSys->Slave[SyncId].AveSumOffset = (LSA_INT32)DiffNanos;
		pChSys->Slave[SyncId].AveSumCount = 1;

		/* Store local frame receive time for controller call
		*/
		pChSys->Slave[SyncId].AdjustNanos = ActNanos;
		pChSys->Slave[SyncId].AdjustSeconds = ActSeconds;
		pChSys->Slave[SyncId].Statistics.FrameOk++;

	    if ((DiffNanos > (LSA_INT64)pChSys->Slave[SyncId].WindowSize)
	    ||  (-DiffNanos > (LSA_INT64)pChSys->Slave[SyncId].WindowSize))
		{
			GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveAction(%02x) Offset=%dns SeqId=%u: slave state change FIRST->OUT",
					SyncId, (LSA_INT32)DiffNanos, pChSys->Slave[SyncId].SeqId);
			pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_OUT;
		}
		else
		{
			GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveAction(%02x) Offset=%dns SeqId=%u: slave state change FIRST->SYNC",
					SyncId, (LSA_INT32)DiffNanos, pChSys->Slave[SyncId].SeqId);
			pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_SYNC;
		}

		/* Reset controller
		*/
		k32gsy_SyncReset(pChSys->Slave[SyncId].pDrift->AveInterval, SyncId, pChSys);

		/* Adjust slave PTCP time
		*/
		k32gsy_SlaveAdjust(pChSys, SyncId);
	}
	else
	{
		/* Store offset in slave data
		 * DiffNanos is max. 1s = 1.000.000.000ns = 3B9A CA00
		*/
		pChSys->Slave[SyncId].AveSumOffset = (LSA_INT32)DiffNanos;
		pChSys->Slave[SyncId].AveSumCount = 1;
		pChSys->Slave[SyncId].AveTimeOffset = (LSA_INT32)DiffNanos;

		/* Store minimum and maximum
		*/
		if ((pChSys->Slave[SyncId].AveMaxOffset == 0) || (pChSys->Slave[SyncId].AveMaxOffset < DiffNanos))
			pChSys->Slave[SyncId].AveMaxOffset = (LSA_INT32)DiffNanos;
		if ((pChSys->Slave[SyncId].AveMinOffset == 0) || (pChSys->Slave[SyncId].AveMinOffset > DiffNanos))
			pChSys->Slave[SyncId].AveMinOffset = (LSA_INT32)DiffNanos;

		/* Store local frame receive time for controller call
		*/
		pChSys->Slave[SyncId].AdjustNanos = ActNanos;
		pChSys->Slave[SyncId].AdjustSeconds = ActSeconds;
		pChSys->Slave[SyncId].Statistics.FrameOk++;

		/* Adjust slave PTCP time
		*/
		k32gsy_SlaveAdjust(pChSys, SyncId);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveAction() State=%u AveTimeOffset=%d DiffNanos=%d",
					pChSys->Slave[SyncId].State, pChSys->Slave[SyncId].AveTimeOffset, DiffNanos);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveSeqIdCheck()                               */
/* Check SequenceId of a received SyncFrame                                  */
/*****************************************************************************/
K32_STATIC LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_SlaveSeqIdCheck(
GSY_CH_K32_PTR			pChSys,
LSA_UINT16				SeqId,
LSA_UINT16				PortId,
LSA_UINT8				SyncId)
{
	LSA_UINT16	SeqDist;
	LSA_BOOL	RetVal = LSA_TRUE;

	GSY_FUNCTION_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SlaveSeqIdCheck() pChSys=%x SeqId/PortId=%d/%d LastSeqId/PortId=%d/%d",
					pChSys, SeqId, PortId, pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].SyncPortId);

	if (SeqId < pChSys->Slave[SyncId].SeqId)
		SeqDist = SeqId + 0xffff - pChSys->Slave[SyncId].SeqId + 1;
	else
		SeqDist = SeqId - pChSys->Slave[SyncId].SeqId;

	if (((SeqDist == 0) && !(pChSys->Slave[SyncId].WaitFU && (PortId != pChSys->Slave[SyncId].SyncPortId)))	// RQ 1476333: patch PN-IP behaviour
	||  (GSY_SEQID_MAX_DIST < SeqDist))
	{
		RetVal = LSA_FALSE;
		pChSys->Slave[SyncId].Statistics.FrameWrongSequenceNr++;
		GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_SlaveSeqIdCheck(%02x) Invalid SeqId=%d LastSeqId=%d PortId=%d",
					SyncId, SeqId, pChSys->Slave[SyncId].SeqId, PortId);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SlaveSeqIdCheck() pChUsr=%x RetVal=%d FrameWrongSequenceNr=%d",
					pChSys, RetVal, pChSys->Slave[SyncId].Statistics.FrameWrongSequenceNr);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveIndSync()                                  */
/* Get the data of SyncFrame for slave                                       */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveIndSync(
GSY_CH_K32_PTR			pChSys,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId,
LSA_UINT32				TimeStamp,
LSA_UINT32				LocalSeconds,
LSA_UINT32				LocalNanos)
{
	LSA_UINT8	SyncId = pTLVInfo->SyncId;
	LSA_BOOL NewMaster = LSA_FALSE, DelayOk;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveIndSync() pChSys=%x SyncId=%u Port=%d",
					pChSys, SyncId, PortId);

	/* Check Subdomain
	*/
	if (GSY_SUBDOMAIN_CMP(pTLVInfo->Subdomain, pChSys->Slave[SyncId].Subdomain))
	{
		/* If the drift to this master is calculated
		 * and this SyncFrame is the first from this master
		 * or the SequenceID is valid...
		*/
		pChSys->Slave[SyncId].SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
		if (!LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pDrift, LSA_NULL)
		&& ((GSY_SLAVE_STATE_WAIT == pChSys->Slave[SyncId].State)
		||  (GSY_SLAVE_STATE_FIRST == pChSys->Slave[SyncId].State)
		||  (GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr))
		||  ((GSY_MACADDR_CMP(pTLVInfo->Param.Sync.Master.MacAddr, pChSys->Slave[SyncId].MasterAddr)) 
		 &&  k32gsy_SlaveSeqIdCheck(pChSys, pTLVInfo->Param.Sync.SequenceID, PortId, SyncId))))
		{
			/* Store receive port and values from SyncFrame in slave structure
			 * 070108lrg002: Errortrace
			*/
			if (pChSys->Slave[SyncId].WaitFU)
			{
				/* Missing FollowUp of last received SyncFrame
				*/
				if ((pTLVInfo->Param.Sync.SequenceID == pChSys->Slave[SyncId].SeqId) && (PortId != pChSys->Slave[SyncId].SyncPortId))	
				{
					/* RQ 1476333: patch PN-IP behaviour: replace prior from other port received frame with same sequence id
					*/
					GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_SlaveIndSync() Replacing SyncFU SeqId=%u from PortId=%u with PortId=%u",
								pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].SyncPortId, PortId);
				}
				else
				{
					pChSys->Slave[SyncId].FuTimeout += pChSys->Slave[SyncId].SyncInterval;
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_SlaveIndSync() Missing FU SeqId=%u from Port=%u FuTimeout=%u",
									pChSys->Slave[SyncId].SeqId, pChSys->Slave[SyncId].SyncPortId, pChSys->Slave[SyncId].FuTimeout);
					if (pChSys->Slave[SyncId].FuTimeout == pChSys->Slave[SyncId].SyncTimeout)
					{
						/* 1028592: Simulate watchdog for FU frame timeout
						 * Set SyncTimeout = 0 to prevent restart of watchdog on takeover
						*/
						pChSys->Slave[SyncId].SumTimeout = 1; 
						k32gsy_WatchdogCbf(pChSys, SyncId);
						pChSys->Slave[SyncId].FuTimeout = 0; 
					}
				}
			}
			pChSys->Slave[SyncId].SeqId = pTLVInfo->Param.Sync.SequenceID;
			pChSys->Slave[SyncId].WaitFU = pTLVInfo->Param.Sync.FUFlag;
			pChSys->Slave[SyncId].pDrift = pTLVInfo->pDrift;
			pChSys->Slave[SyncId].SyncPortId = PortId;
			pChSys->Slave[SyncId].Act.SeqId = pTLVInfo->Param.Sync.SequenceID;
			pChSys->Slave[SyncId].Act.Timestamp = TimeStamp;
			pChSys->Slave[SyncId].Act.OrgEpoch = pTLVInfo->Param.Sync.Epoch;
			pChSys->Slave[SyncId].Act.OrgSeconds = pTLVInfo->Param.Sync.Seconds;
			pChSys->Slave[SyncId].Act.OrgNanos = pTLVInfo->Param.Sync.Nanos;
			pChSys->Slave[SyncId].Act.LocalSeconds = LocalSeconds;
			pChSys->Slave[SyncId].Act.LocalNanos = LocalNanos;
			pChSys->Slave[SyncId].CurrentUTCOffset = pTLVInfo->Param.Sync.UTCOffset;
			pChSys->Slave[SyncId].TimeFlags = pTLVInfo->Param.Sync.TimeFlags;

			if ((GSY_DRIFT_STATE_READY == pTLVInfo->pDrift->State)
			&&  ((GSY_SLAVE_STATE_WAIT == pChSys->Slave[SyncId].State)
			 ||  (GSY_SLAVE_STATE_FIRST == pChSys->Slave[SyncId].State)
			 ||  GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr)))
			{
				/* Store constants only from first SyncFrame
				*/
				NewMaster = LSA_TRUE;
				pChSys->Slave[SyncId].MasterAddr = pTLVInfo->Param.Sync.Master.MacAddr;
				pChSys->Slave[SyncId].ClockVariance = pTLVInfo->Param.Sync.Master.Variance;
				pChSys->Slave[SyncId].ClockAccuracy = pTLVInfo->Param.Sync.Master.Accuracy;
				pChSys->Slave[SyncId].ClockClass = pTLVInfo->Param.Sync.Master.Class;
			}
			/* Move frame delay to slave structure
			*/
			DelayOk = k32gsy_DelayGet(pChSys, &pChSys->Slave[SyncId].Act, pTLVInfo, PortId);

			GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SlaveIndSync(%02x) SeqId=%u Delay10NS=%u Delay1NS=%u DelayNS=%u: DelaySeconds=%u DelayNanos=%u",
							SyncId, pChSys->Slave[SyncId].SeqId, pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS,
							pTLVInfo->Param.Sync.DelayNS, pChSys->Slave[SyncId].Act.DelaySeconds, pChSys->Slave[SyncId].Act.DelayNanos);

			if (!DelayOk)
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SlaveIndSync() Invalid delay: Delay10NS=%u Delay1NS=%u DelayNS=%u",
								pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS, pTLVInfo->Param.Sync.DelayNS);
			}
			else if ((!pChSys->Slave[SyncId].WaitFU)
			&&  (GSY_SLAVE_STATE_WAIT_SET != pChSys->Slave[SyncId].State)
			&&  (GSY_MASTER_STATE_PRIMARY != pChSys->Master[SyncId].State))
			{
				/* Calculate offset if no FollowUp follows
				*/
				k32gsy_SlaveAction(pChSys, SyncId);
			}
			if ((GSY_SYNCID_TIME == SyncId)
			&&  ((pChSys->Slave[SyncId].CurrentUTCOffset != pChSys->pK32SyncRqb->Params.SyncInd.UTCOffset)
			||   (pChSys->Slave[SyncId].Act.OrgEpoch != pChSys->pK32SyncRqb->Params.SyncInd.SecondsHigh)))
			{
				/* FT1204141: Update time sync indication
				*/
				if ((GSY_SLAVE_STATE_SYNC == pChSys->Slave[SyncId].State)
				||  (GSY_SLAVE_STATE_SYNC_OUT == pChSys->Slave[SyncId].State))
				{
					k32gsy_SyncMsg(pChSys, SyncId, LSA_TRUE);
				}
				else
				{
					k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);
				}
			}
		}
	}
	else
	{
		/* Invalid Subdomain
		*/
		pChSys->Slave[SyncId].SyncError = GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID;
		pChSys->Slave[SyncId].Statistics.FrameWrongSubdomain++;
		GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** k32gsy_SlaveIndSync(%02x) SubomainUUID mismatch: SeqId=%u",
						SyncId, pTLVInfo->Param.Sync.SequenceID);
	}

	if (!LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pDrift, LSA_NULL)
	&&  (NewMaster
	 || (!GSY_MACADDR_IS_NULL(pChSys->Slave[SyncId].MasterAddr)			//121009lrg001: changed RcvSyncPrio only with MAC address
	  && (pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.RcvSyncPrio != pTLVInfo->pDrift->RcvSyncPrio))))
	{
		/* 030909lrg001: Diagnosis on changed RcvSyncPrio
		*/
		if (NewMaster)
		{
			GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SlaveIndSync(%02x) NewMaster=%02x-%02x-%02x-%02x-%02x-%02x: SeqId=%u",
							SyncId, pChSys->Slave[SyncId].MasterAddr.MacAdr[0], pChSys->Slave[SyncId].MasterAddr.MacAdr[1],
							pChSys->Slave[SyncId].MasterAddr.MacAdr[2],pChSys->Slave[SyncId].MasterAddr.MacAdr[3],
							pChSys->Slave[SyncId].MasterAddr.MacAdr[4],pChSys->Slave[SyncId].MasterAddr.MacAdr[5],
							pTLVInfo->Param.Sync.SequenceID);

			/* Set master MAC address for HW forwarding
			*/
			k32gsy_FwdSet(pChSys, SyncId, LSA_FALSE, pChSys->Slave[SyncId].Subdomain, pChSys->Slave[SyncId].MasterAddr);

			/* Restart Watchdog
			*/
			k32gsy_WatchdogStart(pChSys, SyncId, LSA_FALSE);
		}
//		else
		{
			/* Update diagnosis data and send indication
			*/
			pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.SyncId = SyncId;
			pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.DiagSrc = GSY_DIAG_SOURCE_MASTER;
			pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.SlaveState = pChSys->Slave[SyncId].State;
            K32_COPY_MAC_ADDR_ARRAY (pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.MasterMacAddr, pChSys->Slave[SyncId].MasterAddr.MacAdr);
            pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.RcvSyncPrio = pTLVInfo->pDrift->RcvSyncPrio;
			pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.RateInterval = pTLVInfo->pDrift->AveInterval;
			pTLVInfo->pDrift->Diag.pK32Rqb->Params.DiagInd.SequenceId = pTLVInfo->Param.Sync.SequenceID;
			pTLVInfo->pDrift->Diag.pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
			K32GSY_CALLBACK(pTLVInfo->pDrift->Diag.pK32Rqb);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveIndSync() SyncId=0x%02x SeqId=%u TimeStamp=%u",
					SyncId, pTLVInfo->Param.Sync.SequenceID, TimeStamp);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveIndFu()                                    */
/* Get the data of FollowUpFrame for slave                                   */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_SlaveIndFu(
GSY_CH_K32_PTR			pChSys,
GSY_TLV_INFO_PTR_TYPE   pTLVInfo,
LSA_UINT16				PortId)
{
	LSA_UINT8	SyncId = pTLVInfo->SyncId;
	LSA_BOOL	FuOk = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveIndFu() pChSys=%x SyncId=%u Port=%d",
					pChSys, SyncId, PortId);

	/* The receive port of the FollowUp must be identical to the port of the SyncFrame
	*/
	if ((PortId == pChSys->Slave[SyncId].SyncPortId)
	&&  (pChSys->Slave[SyncId].WaitFU))
	{
		/* Check Subdomain, Masteraddress and SequenceID
		*/
		if (GSY_SUBDOMAIN_CMP(pTLVInfo->Subdomain, pChSys->Slave[SyncId].Subdomain))
		{
			pChSys->Slave[SyncId].SyncError = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED;
			if (pTLVInfo->Param.FollowUp.SequenceID == pChSys->Slave[SyncId].SeqId)
			{
				if (!LSA_HOST_PTR_ARE_EQUAL(pTLVInfo->pDrift, LSA_NULL))
				{
					if (GSY_MACADDR_CMP(pTLVInfo->Param.FollowUp.Master.MacAddr, pChSys->Slave[SyncId].MasterAddr))
					{
						/* Update Delay in slave structure
						*/
						FuOk = k32gsy_DelayGet(pChSys, &pChSys->Slave[SyncId].Act, pTLVInfo, PortId);

						GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SlaveIndFu(%02x) SeqId=%u DelayNS=%d: DelaySeconds=%u DelayNanos=%u",
										SyncId, pChSys->Slave[SyncId].SeqId, pTLVInfo->Param.FollowUp.DelayNS,
										pChSys->Slave[SyncId].Act.DelaySeconds, pChSys->Slave[SyncId].Act.DelayNanos);

						if (FuOk)
						{
							if ((GSY_SLAVE_STATE_WAIT_SET != pChSys->Slave[SyncId].State)
							&&  (GSY_MASTER_STATE_PRIMARY != pChSys->Master[SyncId].State))
							{
								/* Call slave offset calculation
								*/
								k32gsy_SlaveAction(pChSys, SyncId);
							}
						}
						else
						{
							GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SlaveIndFu() Invalid delay: DelayNS=%d [%x%x]",
											pTLVInfo->Param.FollowUp.DelayNS, 0, 0);
						}
					}
					else
					{
						GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** k32gsy_SlaveIndFu(%02x) Master MAC address mismatch: SeqId=%u",
										SyncId, pTLVInfo->Param.FollowUp.SequenceID);
					}
				}
				pChSys->Slave[SyncId].WaitFU = LSA_FALSE;
				pChSys->Slave[SyncId].FuTimeout = 0;
			}
			else	//070108lrg002: Errortrace
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_SlaveIndFu() Invalid FU: SeqId=%u  ActSeqId=%u [%x]",
								pTLVInfo->Param.FollowUp.SequenceID, pChSys->Slave[SyncId].SeqId, 0);
			}
		}
		else
		{
			pChSys->Slave[SyncId].SyncError = GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID;
			GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** k32gsy_SlaveIndFu(%02x) SubomainUUID mismatch: SeqId=%u",
							SyncId, pTLVInfo->Param.FollowUp.SequenceID);
		}
	}
	else	// 070108lrg002: Errortrace
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_SlaveIndFu() ignoring FU SeqId=%u  WaitFU=%u from Port=%u",
						pTLVInfo->Param.FollowUp.SequenceID, pChSys->Slave[SyncId].WaitFU, PortId);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveIndFu() SyncId=0x%02x SeqId=%u FuOk=%d",
					SyncId, pTLVInfo->Param.FollowUp.SequenceID, FuOk);
}

/*****************************************************************************/
/* Internal function: k32gsy_SlaveIsRunning()                                */
/* Check for a running slave                                                 */
/*****************************************************************************/
K32_STATIC LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_SlaveIsRunning(
LSA_UINT8	InterfaceId,
LSA_UINT8	SyncId)
{
	LSA_UINT8		IfId = InterfaceId;
	LSA_BOOL		Running = LSA_FALSE;
	GSY_CH_K32_PTR	pChSys = k32gsy_SysPtrGet(InterfaceId);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SlaveIsRunning() pChSys=%x SyncId=%u Interface=%u",
		pChSys, SyncId, InterfaceId);

	if (GSY_SYNCID_CLOCK == SyncId)
	{
		/* 090611lrg: Check only one interface for clock slave
		*/
		if (pChSys->Slave[SyncId].State != GSY_SLAVE_STATE_OFF)
		{
			Running = LSA_TRUE;
		}
	}
	else
	{
		/* Check all Interfaces at time slave
		*/
		for (IfId = K32_INTERFACE_FIRST; IfId <= K32_INTERFACE_LAST; IfId++)
		{
			pChSys = k32gsy_SysPtrGet(IfId);
			if (pChSys->Slave[SyncId].State != GSY_SLAVE_STATE_OFF)
			{
				Running = LSA_TRUE;
				break;
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SlaveIsRunning() pChSys=%x IfId=%u Running=%u",
		pChSys, InterfaceId, Running);

	return (Running);
}

#ifdef K32_MASTER_IN_FW
/*****************************************************************************/
/* Internal function: k32gsy_MasterIsRunning()                               */
/* Check for a running master                                                */
/*****************************************************************************/
K32_STATIC LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_MasterIsRunning(
LSA_UINT8	InterfaceId,
LSA_UINT8	SyncId)
{
	LSA_UINT8		IfId = InterfaceId;
	LSA_BOOL		Running = LSA_FALSE;
	GSY_CH_K32_PTR	pChSys = k32gsy_SysPtrGet(InterfaceId);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterIsRunning() pChSys=%x SyncId=%u Interface=%u",
		pChSys, SyncId, InterfaceId);

	if (GSY_SYNCID_CLOCK == SyncId)
	{
		/* 200611lrg: Check only one interface for clock master
		*/
		if (pChSys->Master[SyncId].State == GSY_MASTER_STATE_PRIMARY)
		{
			Running = LSA_TRUE;
		}
	}
	else
	{
		/* Check all Interfaces at time master
		*/
		for (IfId = K32_INTERFACE_FIRST; IfId <= K32_INTERFACE_LAST; IfId++)
		{
			pChSys = k32gsy_SysPtrGet(IfId);
			if (pChSys->Master[SyncId].State == GSY_MASTER_STATE_PRIMARY)
			{
				Running = LSA_TRUE;
				break;
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterIsRunning() pChSys=%x IfId=%u Running=%u",
		pChSys, InterfaceId, Running);

	return (Running);
}

/*****************************************************************************/
/* Internal function: k32gsy_MasterStart()                                   */
/* Start a master enity for synchronization                                  */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_MasterStart(
GSY_CH_K32_PTR			pChSys,
LSA_UINT8				SyncId,
LSA_UINT32				SyncSendFactor,
K32GSY_TLV_PTR			pTLV)
{
//110310lrg001:	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};

	GSY_DRIFT_PTR		pDrift = &pChSys->Drift[SyncId];
	K32_MASTER_TYPE		*pMaster = &pChSys->Master[SyncId];

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterStart() pChSys=%x SyncId=0x%02x SyncSendFactor=%u",
					pChSys, SyncId, SyncSendFactor);

	GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_MasterStart(%02x) SlaveState=%u master state change ->PRIMARY SyncSendFactor=%u",
					SyncId, pChSys->Slave[SyncId].State, SyncSendFactor);

	if (GSY_MASTER_STATE_SLAVE != pMaster->State)
	{
		/* Copy TLVs into master send frame
		*/
		pMaster->pFrame->Sync.TLV.Subdomain = pTLV->Subdomain;
		pMaster->pFrame->Sync.TLV.Time = pTLV->Time;
		pMaster->pFrame->Sync.TLV.TimeExt = pTLV->TimeExt;
		pMaster->pFrame->Sync.TLV.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

		pChSys->Master[SyncId].MasterTLV = pTLV->Master;
		pChSys->Slave[SyncId].Subdomain = pTLV->Subdomain.SubdomainUUID;
		pChSys->MACAddr = pTLV->Subdomain.MasterSourceAddress;		//290709lrg001
	}
	pMaster->State = GSY_MASTER_STATE_PRIMARY;

	/* Set own MAC address in drift structure
	*/
	pDrift->MasterAddr = pChSys->MACAddr;
	pDrift->Subdomain = pChSys->Slave[SyncId].Subdomain;

#ifdef GSY_RATE_DECRAY
	if (GSY_DRIFT_STATE_READY != pDrift->State)
	{
		/* Set drift for the master SyncID to 0
		*/
		k32gsy_DriftSet(pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_MASTER);
	}
	else
	{
		/* Decray correction of the master clock by adjusting it with increasing RCF-Interval
		*/
		k32gsy_DriftSet(pChSys, pDrift, SyncId, pDrift->AveInterval, GSY_DRIFT_STATE_TRANSIENT);
	}
#else
	/* Set drift for the master SyncID to 0
	*/
	k32gsy_DriftSet(pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_MASTER);
#endif
	k32gsy_SyncReset(0, SyncId, pChSys);
#ifdef K32_MASTER_SEND_FU_TEST_FWD
	{
		GSY_MAC_ADR_TYPE DoNotFwdMacAddr = pChSys->MACAddr;
		GSY_PTCP_UUID	 Subdomain =  pChSys->Slave[SyncId].Subdomain;
		Subdomain.Data4[7]++;
		//DoNotFwdMacAddr.MacAdr[5]++; // MC or own port address does not work against circulating FU frames
		k32gsy_FwdSet(pChSys, SyncId, LSA_TRUE, Subdomain, DoNotFwdMacAddr);
	}
#else
	k32gsy_FwdSet(pChSys, SyncId, LSA_TRUE, pChSys->Slave[SyncId].Subdomain, pChSys->MACAddr);	//110310lrg001: was NoMacAddr
#endif

	/* Send first sync frame
	*/
	k32gsy_MasterSend(&pMaster->SendTimer);

	/* Start master timer and send SYNC to EDDP
	*/
	k32gsy_TimerStart(&pMaster->SendTimer, SyncSendFactor/10);

	k32gsy_SyncMsg(pChSys, SyncId, LSA_TRUE);

	if (GSY_SYNCID_TIME == SyncId)
	{
		K32GSY_UPDATE_TIME(0, k32gsy_UpdateCbfTime);	//lrgtime: activate PLL_OUT_TIME
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterStart() MasterState=%u SlaveState=%u [%x]",
					pChSys->Master[SyncId].State, pChSys->Slave[SyncId].State, 0);
}

/*****************************************************************************/
/* Internal function: k32gsy_MasterStop()                                    */
/* Terminate a master enity                                                  */
/*****************************************************************************/
K32_STATIC LSA_BOOL GSY_LOCAL_FCT_ATTR k32gsy_MasterStop(
GSY_CH_K32_PTR			pChSys,
LSA_UINT8				SyncId,
K32_RQB_PTR_TYPE  		pRQB)
{
//	GSY_DRIFT_PTR		pDrift = &pChSys->Drift[SyncId];
//	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	LSA_BOOL			ReqCnf = LSA_TRUE;			//181010lrg001

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterStop() pChSys=%x SyncId=0x%02x SlaveState=0x%x",
					pChSys, SyncId , pChSys->Slave[SyncId].State);

	pChSys->Master[SyncId].State = GSY_MASTER_STATE_OFF;
	GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_MasterStop(%02x) SlaveState=%u pFrame=%x master state change ->OFF",
					SyncId, pChSys->Slave[SyncId].State,pChSys->Master[SyncId].pFrame);

	/* Stop master timer
	*/
	k32gsy_TimerStop(&pChSys->Master[SyncId].SendTimer);

	/* 110909lrg001: all done in k32gsy_SlaveStop():

	if ((pDrift->SyncId == SyncId)
	&& 	GSY_MACADDR_CMP(pDrift->MasterAddr, pChSys->MACAddr))	//290709lrg001
	{	
		k32gsy_DriftReset(pChSys, pDrift, SyncId);
	}

	k32gsy_FwdSet(pChSys, SyncId, LSA_FALSE, pChSys->Slave[SyncId].Subdomain, NoMacAddr);
	*/

	/* Send NOT SYNC to EDDP
	k32gsy_SyncMsg(pChSys, SyncId, LSA_FALSE);
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Master[SyncId].pFrame, LSA_NULL))
	{
		pChSys->Master[SyncId].pReqRQB = pRQB;			//181010lrg001
		ReqCnf = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterStop() ReqCnf=%u",
					ReqCnf);
	return(ReqCnf);			//181010lrg001
}

/*****************************************************************************/
/* Internal function: k32gsy_MasterSleep()                                   */
/* Der SyncMaster wird deaktiviert                                           */
/*****************************************************************************/
K32_STATIC LSA_BOOL GSY_LOCAL_FCT_ATTR k32gsy_MasterSleep(
GSY_CH_K32_PTR			pChSys,
LSA_UINT8				SyncId,
K32_RQB_PTR_TYPE  		pRQB)
{
	GSY_DRIFT_PTR		pDrift = &pChSys->Drift[SyncId];
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	LSA_BOOL			ReqCnf = LSA_TRUE;			//181010lrg001

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterSleep() pChSys=%x SyncId=0x%02x SlaveState=0x%x",
					pChSys, SyncId , pChSys->Slave[SyncId].State);

	pChSys->Master[SyncId].State = GSY_MASTER_STATE_SLAVE;
	GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_MasterSleep(%02x) SlaveState=%u pFrame=%x master state change ->SLAVE",
					SyncId, pChSys->Slave[SyncId].State, pChSys->Master[SyncId].pFrame);

	/* Stop master timer
	*/
	k32gsy_TimerStop(&pChSys->Master[SyncId].SendTimer);

	/* Release drift structure if still used by master
	*/
	if ((pDrift->SyncId == SyncId)
	&& 	GSY_MACADDR_CMP(pDrift->MasterAddr, pChSys->MACAddr))	//290709lrg001
	{
		k32gsy_DriftReset(pChSys, pDrift, SyncId);
	}

	/* Release Subdomain for HW forwarding and start Watchdog
	*/
	k32gsy_FwdSet(pChSys, SyncId, LSA_FALSE, pChSys->Slave[SyncId].Subdomain, NoMacAddr);
	k32gsy_WatchdogStart(pChSys, SyncId, LSA_FALSE);

	if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Master[SyncId].pFrame, LSA_NULL))
	{
		pChSys->Master[SyncId].pReqRQB = pRQB;			//181010lrg001
		ReqCnf = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterSleep() ReqCnf=%x",
					ReqCnf);
	return(ReqCnf);			//181010lrg001
}

/*****************************************************************************/
/* Internal access function: k32gsy_MasterSend()                             */
/* Timerfunction of SyncMaster                                               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR k32gsy_MasterSend(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT8			SyncId, UseType;
	K32_MASTER_TYPE		*pMaster;
	K32GSY_FRAME_PTR	pFrame;
	GSY_CH_K32_PTR		pChSys = pTimer->pChSys;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterSend() pTimer=%x pChSys=%x Interface=%u",
					pTimer, pChSys, pChSys->InterfaceId);

	if (LSA_HOST_PTR_ARE_EQUAL(&pChSys->Master[0].SendTimer, pTimer))
	{
		SyncId = GSY_SYNCID_CLOCK;
		UseType = K32_USETYPE_CLOCK;
	}
	else
	{
		SyncId = GSY_SYNCID_TIME;
		UseType = K32_USETYPE_TIME;
	}
	pMaster = &pChSys->Master[SyncId];
	pFrame = (K32GSY_FRAME_PTR)pMaster->pFrame;

	if (!LSA_HOST_PTR_ARE_EQUAL(pFrame, LSA_NULL))
	{
		/* Send frame with next sequence id only if last sent frame is already confirmed
		*/
		pMaster->SeqId++;
		pMaster->pFrame->Sync.PtcpHdr.SequenceID.Word = GSY_SWAP16(pMaster->SeqId);
		pMaster->pFrame->Sync.TLV.Master = pMaster->MasterTLV;
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_MasterSend(%02x) sending sync frame=%x SeqId=%u", 
						SyncId, pFrame, pMaster->SeqId);
#ifdef K32_MASTER_SEND_FU_TEST
		pMaster->pFrame->PduHdr.DstMacAdr.MacAdr[5] = 0x20 | SyncId;
		pMaster->pFrame->PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_FUSYNC | SyncId);
#endif
		/* Send SyncFrame without Timestamp
		 * Pointer will be restored on send confirmation
		*/
		pChSys->Master[SyncId].pFrame = LSA_NULL;
		K32GSY_SEND_FRAME(pFrame, sizeof(K32_PTCP_SYNC_FRAME), UseType, pChSys->InterfaceId, 0, LSA_FALSE);
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_MasterSend() InterfaceId=%u SyncId=%u SeqId=%u: not confirmed",
					pChSys->InterfaceId, SyncId, pMaster->SeqId);
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterSend()");
}

/*****************************************************************************/
/* Internal access function: k32gsy_MasterSyncCnf()                          */
/* Confirmation for a sent SyncFrame                                         */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_MasterSyncCnf(
GSY_CH_K32_PTR			pChSys,
GSY_LOWER_TX_PDU_PTR	pPduTx,
LSA_UINT16				Response,
LSA_UINT8				SyncId)
{
#ifdef K32_MASTER_SEND_FU_TEST
	LSA_UINT16	FrameId = GSY_SWAP16(pPduTx->PduHdr.FrameId.Word);
#endif
	LSA_UINT16	SeqId = GSY_SWAP16(pPduTx->PtcpData.Sync.PtcpHdr.SequenceID.Word);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_MasterSyncCnf() pChSys=%x pPduTx=%x Response=%u",
					pChSys, pPduTx, Response);

#ifdef K32_MASTER_SEND_FU_TEST
	if ((FrameId != GSY_FRAMEID_SYNC_FU) && (FrameId != GSY_FRAMEID_TIME_FU))
	{
#endif
	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Master[SyncId].pFrame, LSA_NULL))
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_MasterSyncCnf() Sync frame=%x not sent at Interface=%u SyncId=%u",
						pPduTx, pChSys->InterfaceId, SyncId);
	}
	else
	{
		GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_MasterSyncCnf(%02x) confirmed sync frame=%x SeqId=%u",
						SyncId, pPduTx, pChSys->Master[SyncId].SeqId);

		/* Restore confirmed frame buffer pointer
		*/
		pChSys->Master[SyncId].pFrame = (GSY_SYNC_SEND_FRAME_PTR)pPduTx;
	}

	if (K32_TXRESULT_OK != Response)
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_MasterSyncCnf() SyncId=%u SeqId=%u: Send response=0x%x",
						SyncId, SeqId, Response);
	}
#ifdef K32_MASTER_SEND_FU_TEST
	else if ((FrameId == GSY_FRAMEID_FUSYNC) || (FrameId == GSY_FRAMEID_FUTIME))
	{
		// send FU
		LSA_UINT8 SyncId = pPduTx->PduHdr.DstMacAdr.MacAdr[5] & 1;
		LSA_UINT8 UseType = K32_USETYPE_CLOCKFU + SyncId;

		k32_MasterFuFrame.PduHdr = pPduTx->PduHdr;
		k32_MasterFuFrame.PduHdr.DstMacAdr.MacAdr[5] = 0x40 | SyncId;
		k32_MasterFuFrame.PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_SYNC_FU | SyncId);
		k32_MasterFuFrame.SyncFu.PtcpHdr.SequenceID = pPduTx->PtcpData.Sync.PtcpHdr.SequenceID;
		k32_MasterFuFrame.SyncFu.PtcpHdr.ApiRecvTS.Dword = 0;
		k32_MasterFuFrame.SyncFu.PtcpHdr.ReceiveTS.Dword = 0;
		k32_MasterFuFrame.SyncFu.PtcpHdr.Reserved3 = 0;
		k32_MasterFuFrame.SyncFu.PtcpHdr.Delay10ns.Dword = 0;
		k32_MasterFuFrame.SyncFu.PtcpHdr.Delay1ns = 0;
		k32_MasterFuFrame.SyncFu.PtcpHdr.DelayNS.Dword = 0;
		k32_MasterFuFrame.SyncFu.Subdomain = pPduTx->PtcpData.Sync.Subdomain;
		k32_MasterFuFrame.SyncFu.Time = pPduTx->PtcpData.Sync.Time;
		k32_MasterFuFrame.SyncFu.End = pPduTx->PtcpData.Sync.End;
		K32GSY_SEND_FRAME((K32GSY_FRAME_PTR)&k32_MasterFuFrame, sizeof(K32_PTCP_FU_FRAME), UseType, pChSys->InterfaceId, 0, LSA_FALSE);
	}
#endif
	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Master[SyncId].pReqRQB, LSA_NULL))
	{
		/* 181010lrg001: Confirm MasterStop/Sleep now
		*/
		GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_MasterSyncCnf(%02x) confirm now: pSyncReq=%x",
						SyncId, pChSys->Master[SyncId].pReqRQB);
		K32GSY_CALLBACK(pChSys->Master[SyncId].pReqRQB);
		pChSys->Master[SyncId].pReqRQB = LSA_NULL;
	}
#ifdef K32_MASTER_SEND_FU_TEST
	}
#endif
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_MasterSyncCnf() SyncId=%u SeqId=%u",
					SyncId, SeqId);

	LSA_UNUSED_ARG(SeqId);	//200710lrg001
}
#endif // K32_MASTER_IN_FW

/*****************************************************************************/
/* External access function: k32gsy_SyncInd()                                */
/* Handle received Clock/Time-SyncFrame                                      */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_SyncInd(
K32GSY_FRAME_PTR	pFrameBuffer,
LSA_UINT16			FrameLength,
LSA_UINT8			InterfaceId,
LSA_UINT16			PortId,
LSA_UINT8			Result,
LSA_UINT8			UseType,
LSA_UINT32			Timestamp)
{
	GSY_TLV_INFO_TYPE		TLVInfo;
	LSA_BOOL				PduOk = LSA_FALSE;
	LSA_BOOL				RecvAgain = LSA_TRUE;
	LSA_UINT8				SyncId = GSY_SYNCID_NONE;
	LSA_UINT32				LocalSeconds = GSY_SWAP32(*(LSA_UINT32*)pFrameBuffer);
	LSA_UINT32				LocalNanos = GSY_SWAP32(*(LSA_UINT32*)(pFrameBuffer + 4));
	LSA_UINT32				TraceIdx = 0;
	GSY_LOWER_RX_PDU_PTR	pPdu = LSA_NULL;
	GSY_LOWER_RX_RCV_PTR	pPduRcv = (GSY_LOWER_RX_RCV_PTR)(pFrameBuffer + 8);
	GSY_LOWER_RXBYTE_PTR	pPduRx = LSA_NULL;
	LSA_UINT16				DataLength = 0;
	GSY_CH_K32_PTR  		pChSys = k32gsy_SysPtrGet(InterfaceId);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		TraceIdx = pChSys->TraceIdx;
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncInd() pChSys=%x pFrame=%x Port=%u",
					pChSys, pFrameBuffer, PortId);

		if (Result == K32_RXRESULT_OK)
		{
			if (K32_USETYPE_CLOCK == UseType)
			{
				SyncId = GSY_SYNCID_CLOCK;
			}
			else if (K32_USETYPE_TIME == UseType)
			{
				SyncId = GSY_SYNCID_TIME;
			}
			else
			{
				GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** k32gsy_SyncInd() Invalid UseType=%u",
								UseType);
			}

			if ((GSY_SYNCID_NONE != SyncId)
			&& (0 != pChSys->Port[PortId-1].SyncLineDelay))
			{
				/* On SyncId CLOCK or TIME and LineDelay at port:
				 * Skip PDU-Header and FrameID
				*/
				LSA_UINT16 EtheTypeNoVlan = GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.EthType.Word);
				if (GSY_ETHETYPE == EtheTypeNoVlan)
				{
					/* PDU without VLAN tag
					*/
					/* FrameId = GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.FrameId.Word); */
					pPduRx = (GSY_LOWER_RXBYTE_PTR)&pPduRcv->PduNoVlan.PtcpData;
					pPdu = (GSY_LOWER_RX_PDU_PTR)pPduRx;
					pPduRx -= 2;
					DataLength = FrameLength - GSY_PDU_HDR_SIZE_NO_VLAN + 2;
				}
				else
				{
					LSA_UINT16 EtheTypeVlan = GSY_SWAP16(pPduRcv->PduVlan.PduHdr.EthType.Word);
					if (GSY_ETHETYPE == EtheTypeVlan)
					{
						/* PDU with VLAN tag
						*/
						/* FrameId = GSY_SWAP16(pPduRcv->PduVlan.PduHdr.FrameId.Word); */
						pPduRx = (GSY_LOWER_RXBYTE_PTR)&pPduRcv->PduVlan.PtcpData;
						pPdu = (GSY_LOWER_RX_PDU_PTR)pPduRx;
						pPduRx -= 2;
						DataLength = FrameLength - GSY_PDU_HDR_SIZE_VLAN + 2;
					}
					else
					{
						GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncInd() pChSys=%x EtheTypeNoVlan=0x%04x EtheTypeVlan=0x%04x: Invalid PDU",
										pChSys, EtheTypeNoVlan, EtheTypeVlan);
					}
				}
			}

			if (!LSA_HOST_PTR_ARE_EQUAL(pPduRx, LSA_NULL))
			{
				/* Get informationen from frame
				*/
				TLVInfo.SyncId = SyncId;
				PduOk = k32gsy_TLVGetInfo(pChSys, pPduRx, DataLength, PortId, &TLVInfo);

#ifdef K32_FU_FWD_IN_FW
				if (PduOk && (GSY_TLV_TYPE_SYNC == TLVInfo.Type) && TLVInfo.Param.Sync.FUFlag && TLVInfo.RateReady)
				{
					LSA_UINT16 RcvLength = FrameLength - 8;
					LSA_UINT16 FwdPortId = GSY_FWD_PORT_GET(PortId);

					if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Fwd[SyncId].pSyncFrame[FwdPortId-1], LSA_NULL)
					&&  ((pChSys->Fwd[SyncId].SeqId != TLVInfo.Param.Sync.SequenceID) || pChSys->Fwd[SyncId].FuWait))
					{
						/* Store SyncFu frame to forward it when the FU frame arrives
						*/
						GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SyncInd(%02x)   storing sync frame=%x SeqId=%u to Port=%u", 
										SyncId, pChSys->Fwd[SyncId].pSyncFrame[FwdPortId-1], TLVInfo.Param.Sync.SequenceID, FwdPortId);
						K32GSY_COPY_RX_TO_TX_FRAME(pChSys->Fwd[SyncId].pSyncFrame[FwdPortId-1], pPduRcv, RcvLength);
						pChSys->Fwd[SyncId].RxStamp[FwdPortId-1] = GSY_SWAP32(pPdu->Sync.PtcpHdr.ApiRecvTS.Dword);
						pChSys->Fwd[SyncId].SeqId = TLVInfo.Param.Sync.SequenceID;
						pChSys->Fwd[SyncId].FuWait = LSA_TRUE;
					}
					else
					{
						GSY_ERROR_TRACE_06(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncInd() pChSys=%x SyncId=%u FuWait=%u FwdSeqId=%u: Cannot forward ActSeqId=%u to Port=%u",
							pChSys, SyncId, pChSys->Fwd[SyncId].FuWait, pChSys->Fwd[SyncId].SeqId, TLVInfo.Param.Sync.SequenceID, FwdPortId);
					}
				}
#endif // K32_FU_FWD_IN_FW

				if (PduOk && (GSY_SLAVE_STATE_OFF != pChSys->Slave[SyncId].State))
				{
					/* Receive next frame and indicate this frame info to slave
					*/
					K32GSY_RECV_FRAME(pFrameBuffer, k32_RecvFrameSize, UseType, InterfaceId);
					RecvAgain = LSA_FALSE;
					k32gsy_SlaveIndSync(pChSys, &TLVInfo, PortId, Timestamp, LocalSeconds, LocalNanos);
				}
			}
		}
		/* AP01425250:ERTEC200P: KRISC32 stürzt ab bei Ethernet Verbindung zu Scalance X204IRT FW V3
		 * --> only ignore frame if it is too long
		*/
		else if (Result == K32_RXRESULT_LEN)
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncInd() pChSys=%x: Ignoring invalid sync frame length=%u from port=%u",
							pChSys, FrameLength, PortId);
		}
		else
		{
			GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SyncInd() pChSys=%x: RecvResult=0x%02x",
							pChSys, Result);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SyncInd() Port=%u UseType=%u: Invalid Interface=%u",
						PortId, UseType, InterfaceId);
	}

	if (RecvAgain)
	{
		/* Receive next frame
		*/
		K32GSY_RECV_FRAME(pFrameBuffer, k32_RecvFrameSize, UseType, InterfaceId);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncInd() Interface=%u Timestamp=%u Result=%u",
					InterfaceId, Timestamp, Result);
}

/*****************************************************************************/
/* External access function: k32gsy_SyncFuInd()                              */
/* Handle received Clock/Time-FollowUpFrame                                  */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_SyncFuInd(
K32GSY_FRAME_PTR	pFrameBuffer,
LSA_UINT16			FrameLength,
LSA_UINT8			InterfaceId,
LSA_UINT16			PortId,
LSA_UINT8			Result,
LSA_UINT8			UseType,
LSA_UINT32			Timestamp)
{
	GSY_TLV_INFO_TYPE		TLVInfo;
	LSA_BOOL				PduOk = LSA_FALSE;
	LSA_BOOL				RecvAgain = LSA_TRUE;
	LSA_UINT8				SyncId = GSY_SYNCID_NONE;
	GSY_LOWER_RXBYTE_PTR	pPduRx = LSA_NULL;
	GSY_LOWER_RX_RCV_PTR	pPduRcv = (GSY_LOWER_RX_RCV_PTR)pFrameBuffer;
	LSA_UINT16				DataLength = 0;
	GSY_CH_K32_PTR  		pChSys = k32gsy_SysPtrGet(InterfaceId);
	LSA_UINT32				TraceIdx = 0;

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		TraceIdx = pChSys->TraceIdx;
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncFuInd() pChSys=%x pFrame=%x Port=%u",
					pChSys, pFrameBuffer, PortId);

		if (Result == K32_RXRESULT_OK)
		{
			if (K32_USETYPE_CLOCKFU == UseType)
			{
				SyncId = GSY_SYNCID_CLOCK;
			}
			else if (K32_USETYPE_TIMEFU == UseType)
			{
				SyncId = GSY_SYNCID_TIME;
			}
			else
			{
				GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** k32gsy_SyncFuInd() Invalid UseType=%u",
								UseType);
			}

			if ((GSY_SYNCID_NONE != SyncId)
			&& (0 != pChSys->Port[PortId-1].SyncLineDelay))
			{
				/* On SyncId CLOCK or TIME and LineDelay at port:
				 * Skip PDU-Header and FrameID
				*/
				if (GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.EthType.Word))
				{
					/* PDU without VLAN tag
					*/
					pPduRx = (GSY_LOWER_RXBYTE_PTR)&pPduRcv->PduNoVlan.PtcpData;
					pPduRx -= 2;
					DataLength = FrameLength - GSY_PDU_HDR_SIZE_NO_VLAN + 2;
				}
				else if (GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduVlan.PduHdr.EthType.Word))
				{
					/* PDU with VLAN tag
					*/
					pPduRx = (GSY_LOWER_RXBYTE_PTR)&pPduRcv->PduVlan.PtcpData;
					pPduRx -= 2;
					DataLength = FrameLength - GSY_PDU_HDR_SIZE_VLAN + 2;
				}
				else
				{
					GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncFuInd() pChSys=%x: Invalid PDU",
									pChSys);
				}
			}

			if (!LSA_HOST_PTR_ARE_EQUAL(pPduRx, LSA_NULL))
			{
				/* Get informationen from frame
				*/
				TLVInfo.SyncId = SyncId;
				PduOk = k32gsy_TLVGetInfo(pChSys, pPduRx, DataLength, PortId, &TLVInfo);

#ifdef K32_FU_FWD_IN_FW
				if (PduOk && (GSY_TLV_TYPE_FOLLOWUP == TLVInfo.Type) && TLVInfo.RateReady)
				{
					LSA_UINT16 FwdPortId = GSY_FWD_PORT_GET(PortId);
					K32GSY_FRAME_PTR pFrame = (K32GSY_FRAME_PTR)pChSys->Fwd[SyncId].pSyncFrame[FwdPortId-1];

					if (!LSA_HOST_PTR_ARE_EQUAL(pFrame, LSA_NULL)
					&&  (pChSys->Fwd[SyncId].SeqId == TLVInfo.Param.FollowUp.SequenceID)
					&&  pChSys->Fwd[SyncId].FuWait
					&&  !pChSys->Fwd[SyncId].CnfWait)
					{
						/* Store FollowUp frame to forward it when the SyncFu frame is confirmed
						*/
						GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SyncFuInd(%02x) storing   FU frame=%x", 
										SyncId, pChSys->Fwd[SyncId].pFuFrame);
						pChSys->Fwd[SyncId].FuWait = LSA_FALSE;
						pChSys->Fwd[SyncId].CnfWait = LSA_TRUE;
						K32GSY_COPY_RX_TO_TX_FRAME(pChSys->Fwd[SyncId].pFuFrame, pPduRcv, FrameLength);

						/* Send SyncFu frame with timestamp on forwarding port
						 * Pointer will be restored on send confirmation
						*/
						GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SyncFuInd(%02x) sending sync frame=%x SeqId=%u on Port=%u", 
										SyncId, pFrame, pChSys->Fwd[SyncId].SeqId, FwdPortId);
						pChSys->Fwd[SyncId].pSyncFrame[FwdPortId-1] = LSA_NULL;
						K32GSY_SEND_FRAME(pFrame, sizeof(K32_PTCP_SYNC_FRAME), UseType, pChSys->InterfaceId, FwdPortId, LSA_TRUE);
					}
					else
					{
						GSY_ERROR_TRACE_06(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncFuInd() pChSys=%x SyncId=%u FuWait=%u CnfWait=%u: Cannot forward SeqId=%u to Port=%u",
							pChSys, SyncId, pChSys->Fwd[SyncId].FuWait, pChSys->Fwd[SyncId].CnfWait, TLVInfo.Param.Sync.SequenceID, FwdPortId);
					}
				}
#endif // K32_FU_FWD_IN_FW

				if (PduOk && (GSY_SLAVE_STATE_OFF != pChSys->Slave[SyncId].State))
				{
					/*  receive next frame and indicate this frame info to slave
					*/
					K32GSY_RECV_FRAME(pFrameBuffer, k32_RecvFrameSize, UseType, InterfaceId);
					RecvAgain = LSA_FALSE;
					k32gsy_SlaveIndFu(pChSys, &TLVInfo, PortId);
				}
			}
		}
		/* AP01425250:ERTEC200P: KRISC32 stürzt ab bei Ethernet Verbindung zu Scalance X204IRT FW V3
		 * --> only ignore frame if it is too long
		*/
		else if (Result == K32_RXRESULT_LEN)
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_SyncFuInd() pChSys=%x: Ignoring invalid syncFu frame length=%u from port=%u",
							pChSys, FrameLength, PortId);
		}
		else
		{
			GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SyncFuInd() pChSys=%x: RecvResult=0x%02x",
							pChSys, Result);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SyncFuInd() Port=%u UseType=%u: Invalid Interface=%u",
						PortId, UseType, InterfaceId);
	}

	if (RecvAgain)
	{
		/* Receive next frame
		*/
		K32GSY_RECV_FRAME(pFrameBuffer, k32_RecvFrameSize, UseType, InterfaceId);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncFuInd() Interface=%u UseType=%u Result=%u",
					InterfaceId, UseType, Result);
	LSA_UNUSED_ARG(Timestamp);
}

/*****************************************************************************/
/* External access function: k32gsy_SyncReq()                                */
/* Handle KRISK32-RRQB with Opcode K32_OPC_GSY_SYNC.                         */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_SyncReq (
#ifndef K32_RAM32
	K32_RQB_PTR_TYPE  pRQB)
{
	GSY_CH_K32_PTR				pChSys = k32gsy_SysPtrGet(pRQB->Hdr.InterfaceID);
	LSA_UINT8					SyncId = pRQB->Params.SyncReq.SyncId;
	LSA_UINT8					InterfaceId = pRQB->Hdr.InterfaceID;
	LSA_UINT16					Response = GSY_RSP_ERR_PARAM;
	K32_PARAMS_TYPE_GSY_DIAG 	Diag = {GSY_NO_MASTER_ADDR,
										0,0,0,0,0, 0};
	LSA_BOOL					DiagInd = LSA_FALSE;
	LSA_BOOL					ReqCnf = LSA_TRUE;
	K32GSY_TLV_MASTER_PTR		pTLVMaster = LSA_NULL;
	LSA_UINT32					TraceIdx = 0;
	K32GSY_TLV_PTR				pTLV;
#else
	K32_RQB_PTR_TYPE  pRQB32)
{
	LSA_UINT8		SyncId;
	LSA_UINT8		InterfaceId;
	GSY_CH_K32_PTR  pChSys;
	K32_RQB_TYPE  		RQB;
	K32_RQB_PTR_TYPE  	pRQB = &RQB;
	LSA_UINT16					Response = GSY_RSP_ERR_PARAM;
	K32_PARAMS_TYPE_GSY_DIAG 	Diag = {0};
	LSA_BOOL					DiagInd = LSA_FALSE;
	LSA_BOOL				ReqCnf = LSA_TRUE;
	K32GSY_TLV_MASTER_PTR	pTLVMaster = LSA_NULL;
	LSA_UINT32				TraceIdx = 0;
	K32GSY_TLV_PTR			pTLV;

	K32_RAM32_CPY(pRQB32, pRQB);
	SyncId = pRQB->Params.SyncReq.SyncId;
	InterfaceId = pRQB->Hdr.InterfaceID;
	pChSys = k32gsy_SysPtrGet(InterfaceId);
#endif

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		TraceIdx = pChSys->TraceIdx;
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SyncReq() pChSys=%x pRQB=%x SyncId=%u",
					pChSys, pRQB, SyncId);

		if (K32_OPC_GSY_SYNC == pRQB->Hdr.Opcode)
		{
			if ((SyncId == GSY_SYNCID_CLOCK)
			||  (SyncId == GSY_SYNCID_TIME))
			{
				/* Check master slave state combination
				*/
#ifdef K32_MASTER_IN_FW
				if (((K32_SYNC_START == pRQB->Params.SyncReq.SlaveState) && (K32_SYNC_ACTIVE == pRQB->Params.SyncReq.MasterState))
				||  ((K32_SYNC_OFF == pRQB->Params.SyncReq.SlaveState) && (K32_SYNC_SLEEP == pRQB->Params.SyncReq.MasterState)))
				{
					Response = GSY_RSP_ERR_PARAM;
					GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() SyncId=%u: Invalid SlaveState=%u and MasterState=%u",
							SyncId, pRQB->Params.SyncReq.SlaveState, pRQB->Params.SyncReq.MasterState);
#else
				if (K32_SYNC_OFF != pRQB->Params.SyncReq.MasterState)
				{
					GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x Interface=%u SyncId=%u: No Master configured",
								pChSys, InterfaceId, SyncId);
					Response = GSY_RSP_ERR_CONFIG;
#endif
				}
				else
				{
					Response = GSY_RSP_OK;

					switch (pRQB->Params.SyncReq.SlaveState)
					{
					case K32_SYNC_START:

						if (K32_SYNC_SLEEP == pRQB->Params.SyncReq.MasterState)
						{
							pChSys->Slave[SyncId].State = GSY_SLAVE_STATE_FIRST;
						}
						else if (k32gsy_SlaveIsRunning(InterfaceId, SyncId))
						{
							Response = GSY_RSP_ERR_SEQUENCE;
							GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x Interface=%u: Slave already running SyncId=%u",
										pChSys, InterfaceId, SyncId);
						}
						else
						{
							/* Start Slave
							*/
							pTLV = (K32GSY_TLV_PTR)pRQB->Params.SyncReq.TLV;
							pChSys->Slave[SyncId].Subdomain = pTLV->Subdomain.SubdomainUUID;
							pChSys->MACAddr = pTLV->Subdomain.MasterSourceAddress;			//260111lrg001

							k32gsy_SlaveStart(pChSys, SyncId, pRQB->Params.SyncReq.PLLWindow, pRQB->Params.SyncReq.SyncSendFactor,
											pRQB->Params.SyncReq.TimeoutFactor, pRQB->Params.SyncReq.TakeoverFactor);
							DiagInd = LSA_TRUE;
						}
						break;

					case K32_SYNC_UPDATE:

						if (!k32gsy_SlaveIsRunning(InterfaceId, SyncId))
						{
							Response = GSY_RSP_ERR_SEQUENCE;
							GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x Interface=%u: Slave not running SyncId=%u",
										pChSys, InterfaceId, SyncId);
						}
						else
						{
							/* Slave/Master update
							*/
							if (K32_SYNC_UPDATE == pRQB->Params.SyncReq.MasterState)
								pTLVMaster = (K32GSY_TLV_MASTER_PTR)pRQB->Params.SyncReq.TLV;
							k32gsy_SyncUpdate(pChSys, SyncId, pRQB->Params.SyncReq.PLLWindow, pRQB->Params.SyncReq.TimeoutFactor,
												pRQB->Params.SyncReq.TakeoverFactor, pTLVMaster);
						}
						break;

					case K32_SYNC_OFF:

						if ((pRQB->Params.SyncReq.MasterState != K32_SYNC_START)
						&&  (pRQB->Params.SyncReq.MasterState != K32_SYNC_ACTIVE)
						&&  (GSY_SLAVE_STATE_OFF != pChSys->Slave[SyncId].State))
						{
							/* Stop Slave
							*/
							GSY_SUBDOMAIN_SET_NULL(pChSys->Slave[SyncId].Subdomain);
							k32gsy_SlaveStop(pChSys, SyncId);
							DiagInd = LSA_TRUE;
						}
						break;

					default:
						GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() Interface=%u: Invalid SlaveState=%u (MasterState=%u)",
								InterfaceId, pRQB->Params.SyncReq.SlaveState, pRQB->Params.SyncReq.MasterState);
						Response = GSY_RSP_ERR_PARAM;
					}
#ifdef K32_MASTER_IN_FW
					if ((GSY_RSP_OK == Response)
					&&  (K32_SYNC_UPDATE != pRQB->Params.SyncReq.SlaveState))
					{
						switch (pRQB->Params.SyncReq.MasterState)
						{
						case K32_SYNC_START:

							if (K32_SYNC_START == pRQB->Params.SyncReq.SlaveState)
							{
								/* Set Master to startup state
								*/
								GSY_SYNC_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_SyncReq(%02x) Interface=%u: master state change ->STARTUP",
										SyncId, InterfaceId);
								pChSys->Master[SyncId].State = GSY_MASTER_STATE_STARTUP;
							}
							else
							{
								/* Start Master
								*/
								if (pRQB->Params.SyncReq.SyncSendFactor < 10)
								{
									Response = GSY_RSP_ERR_PARAM;
								}
								else if (k32gsy_MasterIsRunning(InterfaceId, SyncId))
								{
									Response = GSY_RSP_ERR_SEQUENCE;
									GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x Interface=%u: Master already running SyncId=%u",
												pChSys, InterfaceId, SyncId);
								}
								else
								{
									pTLV = (K32GSY_TLV_PTR)pRQB->Params.SyncReq.TLV;
									k32gsy_MasterStart(pChSys, SyncId, pRQB->Params.SyncReq.SyncSendFactor, pTLV);
								}
							}
							break;

						case K32_SYNC_ACTIVE:

							/* Finish Master startup
							*/
							pChSys->Master[SyncId].MasterTLV.MasterPriority1 |= GSY_SYNC_PRIO1_ACTIVE;
							break;

						case K32_SYNC_SLEEP:

							/* Deactivate Master
							*/
							ReqCnf= k32gsy_MasterSleep(pChSys, SyncId, pRQB);
							break;

						case K32_SYNC_OFF:

							if (GSY_MASTER_STATE_OFF != pChSys->Master[SyncId].State)
							{
								/* Stop Master
								*/
								ReqCnf= k32gsy_MasterStop(pChSys, SyncId, pRQB);
							}
							break;

						default:
							GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() Interface=%u: Invalid MasterState=%u (SlaveState=%u)",
									InterfaceId, pRQB->Params.SyncReq.MasterState, pRQB->Params.SyncReq.SlaveState);
							Response = GSY_RSP_ERR_PARAM;
						}
					}
#endif
					if (DiagInd)
					{
						/* Indicate Subdomain Diagnosis
						*/
						Diag.SlaveState = pChSys->Slave[SyncId].State;
						Diag.SyncId = SyncId;
						Diag.DiagSrc = GSY_DIAG_SOURCE_SUBDOMAIN;
						pChSys->Drift[SyncId].Diag.pK32Rqb->Params.DiagInd = Diag;
						pChSys->Drift[SyncId].Diag.pK32Rqb->Hdr.InterfaceID = pRQB->Hdr.InterfaceID;
						K32GSY_CALLBACK(pChSys->Drift[SyncId].Diag.pK32Rqb);
					}
				}
			}
			else
			{
				GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x pRQB=%x: Invalid SyncId=%u",
								pChSys, pRQB, pRQB->Params.SyncReq.SyncId);
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x pRQB=%x: Invalid Opcode=%u",
							pChSys, pRQB, pRQB->Hdr.Opcode);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SyncReq() pChSys=%x pRQB=%x: Invalid Interface=%u",
						pChSys, pRQB, pRQB->Hdr.InterfaceID);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SyncReq() SlaveState=%u MasterState=%u Response=0x%x",
					pRQB->Params.SyncReq.SlaveState, pRQB->Params.SyncReq.MasterState, Response);

	pRQB->Hdr.Response = Response;
#ifdef K32_RAM32
	K32_RAM32_CPY(pRQB, pRQB32);
	K32GSY_CALLBACK(pRQB32);
#else	/* K32_RAM32 */
	if (ReqCnf)
	{
		K32GSY_CALLBACK(pRQB);
	}
#endif	/* K32_RAM32 */
}

/*****************************************************************************/
/* External access function: k32gsy_TopoStateSet()                           */
/* Control the implicit boundary of GenSync in PN-IP                         */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_TopoStateSet (
#ifndef K32_RAM32
	K32_RQB_PTR_TYPE  pRQB)
{
	LSA_UINT16		PortId = pRQB->Params.TopoState.PortID;
	LSA_BOOL		TopoOk = pRQB->Params.TopoState.TopoStateOk;
	LSA_UINT8		InterfaceId = pRQB->Hdr.InterfaceID;
	LSA_UINT32		TraceIdx = 0;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(InterfaceId);
#else
	K32_RQB_PTR_TYPE  pRQB32)
{
	LSA_UINT16		PortId;
	LSA_BOOL		TopoOk;
	LSA_UINT8		InterfaceId;
	LSA_UINT32		TraceIdx = 0;
	GSY_CH_K32_PTR  pChSys;
	K32_RQB_TYPE  		RQB;
	K32_RQB_PTR_TYPE  	pRQB = &RQB;
	K32_RAM32_CPY(pRQB32, pRQB);

	PortId = pRQB->Params.TopoState.PortID;
	TopoOk = pRQB->Params.TopoState.TopoStateOk;
	InterfaceId = pRQB->Hdr.InterfaceID;
	pChSys = k32gsy_SysPtrGet(InterfaceId);
#endif

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		TraceIdx = pChSys->TraceIdx;
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_TopoStateSet() pChSys=%x pRQB=%x Port=%u",
					pChSys, pRQB, PortId);

		if ((PortId > 0) && (PortId <= pChSys->PortCount))
		{
			/* Implicit topology boundaries only for clock synchronisation
			*/
			if (pChSys->Port[PortId-1].ClockSyncOk != TopoOk)
			{
				/* Set or clear boundary at port only if changed
				*/
				pChSys->Port[PortId-1].ClockSyncOk = TopoOk;
				GSY_SYNC_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_TopoStateSet(%u,%u) clock topo state change ->[%u] LineDelay=%u",
							InterfaceId, PortId, TopoOk, pChSys->Port[PortId-1].SyncLineDelay);

				/* 250609lrg001: allow forwarding of ClockSyncFrames to port only if there is line delay != 0 set
				*/
				if (!TopoOk)
				{
					GSY_SYNC_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_TopoStateSet(%u,%u) clock topo state change ->[0]",
							InterfaceId, PortId);
					K32GSY_FWD_TX_SET_CLOCK(InterfaceId, PortId, LSA_FALSE);
				}
				else if (pChSys->Port[PortId-1].SyncLineDelay != 0)
				{
					GSY_SYNC_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_TopoStateSet(%u,%u) clock topo state change ->[1]",
							InterfaceId, PortId);
					K32GSY_FWD_TX_SET_CLOCK(InterfaceId, PortId, LSA_TRUE);
				}
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_TopoStateSet() pChSys=%x Interface=%x: Invalid Port=%u",
							pChSys, InterfaceId, PortId);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_TopoStateSet() pChSys=%x pRQB=%x: Invalid Interface=%u",
						pChSys, pRQB, pRQB->Hdr.InterfaceID);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_TopoStateSet() pChSys=%x pRQB=%x Interface=%u",
		pChSys, pRQB, InterfaceId);

#ifdef K32_RAM32
	K32_RAM32_CPY(pRQB, pRQB32);
	K32GSY_CALLBACK(pRQB32);
#else	/* K32_RAM32 */
	K32GSY_CALLBACK(pRQB);
#endif	/* K32_RAM32 */

}

#ifdef K32_DEBUG_FRAME_RECEIVE_ENABLE
//z00301au 19.04.2013: Added Debug function. AP01516637
/*****************************************************************************/
/* Debug-Function to trace received frames                                   */
/*                                                                           */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_DebugRecvFrame(
K32GSY_FRAME_PTR	pFrameBuffer,
LSA_UINT16			FrameLength,
LSA_UINT8			InterfaceId,
LSA_UINT16			PortId,
LSA_UINT8			Result,
LSA_UINT8			UseType,
LSA_UINT32			Timestamp)
{
	LSA_UINT32				TraceIdx = 0;
	GSY_LOWER_RX_RCV_PTR	pPduRcv;
	LSA_UINT16				FrameId = 0;
    // Calc offset to FrameID
    if(K32_USETYPE_CLOCK == UseType || K32_USETYPE_TIME == UseType)
    {
        pPduRcv = (GSY_LOWER_RX_RCV_PTR)(pFrameBuffer + 8);
    }
    else
    {
        pPduRcv = (GSY_LOWER_RX_RCV_PTR)(pFrameBuffer);
    }
    //Check if there's a VLAN tag, get FrameId
    if (GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.EthType.Word))
    {
	    FrameId = GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.FrameId.Word);
    }
    else if(GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduVlan.PduHdr.EthType.Word))
    {
	    FrameId = GSY_SWAP16(pPduRcv->PduVlan.PduHdr.FrameId.Word);
    }
    else
    {
        GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"k32gsy_DebugRecvFrame() Invalid EtherType=(with VLAN=0x%X, without VLAN=0x%X)",  GSY_SWAP16(pPduRcv->PduVlan.PduHdr.FrameId.Word), GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.EthType.Word));
    }
    // Switch by UseType (== FilterDecisionCode from PNIP HWFilter)
    switch(UseType)
    {
    case K32_USETYPE_CLOCK:
        GSY_DIAG_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"k32gsy_DebugRecvFrame UseType=%d(K32_USETYPE_CLOCK), FrameID = 0x%04X, FrameReceiveResult =%d, FrameLength=%d", UseType, FrameId, Result, FrameLength );
        break;
    case K32_USETYPE_TIME: // Note: Currently, this checks for PTCP SyncFrame (Time) FrameID. Will be replayed by gPTP-Frame?
        GSY_DIAG_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"k32gsy_DebugRecvFrame UseType=%d(K32_USETYPE_TIME), FrameID = 0x%04X, FrameReceiveResult =%d, FrameLength=%d", UseType, FrameId, Result, FrameLength );
        break;
    case K32_USETYPE_DELAY:
        GSY_DIAG_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"k32gsy_DebugRecvFrame UseType=%d(K32_USETYPE_DELAY), FrameID = 0x%04X, FrameReceiveResult =%d, FrameLength=%d", UseType, FrameId, Result, FrameLength );
        break;
    case K32_USETYPE_CLOCKFU:
        GSY_DIAG_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"k32gsy_DebugRecvFrame UseType=%d(K32_USETYPE_CLOCKFU), FrameID = 0x%04X, FrameReceiveResult =%d, FrameLength=%d", UseType, FrameId, Result, FrameLength );
        break;
    case K32_USETYPE_TIMEFU: // Note: Currently, this checks for PTCP SyncFUFrame FrameID. Will be replayed by gPTP-Frame?
        GSY_DIAG_TRACE_04(TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"k32gsy_DebugRecvFrame UseType=%d(K32_USETYPE_TIMEFU), FrameID = 0x%04X, FrameReceiveResult =%d, FrameLength=%d", UseType, FrameId, Result, FrameLength );
        break;
    default :
        GSY_ERROR_TRACE_00(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"k32gsy_DebugRecvFrame() Invalid UseType");
    }

}
#endif

/*****************************************************************************/
/*  end of file GSY_MSM.C                                                    */
/*****************************************************************************/

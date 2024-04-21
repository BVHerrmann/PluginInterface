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
/*  F i l e               &F: gsy_low.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Lower Interface (to EDD)                                                 */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  file creation                  */
/*  2005-04-04  P01.00.00.00_00.01.00.01 lrg  Umstellung auf TLVs            */
/*  2005-05-03  P03.05.00.00_00.03.00.01 lrg  gsy_UserDelayInd() bei der     */
/*              lower DelaySet-Confirmation aufrufen.                        */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2005-06-15  P03.06.00.00_00.03.02.01 lrg  OpenChannel trotz RxTxNanos=0  */
/*  2005-07-26  P03.07.00.00_00.04.01.01 lrg  gsy_OpenLowerError() setzt     */
/*              Multicastadressen im EDD wieder zurueck.                     */
/*  2005-09-14  P03.07.00.00_00.08.01.01 js   Initialisierung einiger        */
/*                                            Variablen um Compiler Warning  */
/*                                            zu verhindern.                 */
/*  2005-10-27  P03.07.00.00_00.09.01.01 lrg  gsy_PortStatus() Delaymessung  */
/*                                            wird nur im FULL_DUPLEX-Mode   */
/*                                            gestartet. Indication an fwd.  */
/*  2005-11-18  P03.08.00.00_00.01.02.01 lrg  181105lrg001                   */
/*  2006-02-15  P03.09.00.00_00.02.01.02 lrg  gsy_FwdRcvIndication() nur im  */
/*              State GSY_CHA_STATE_OPEN aufrufen.                           */
/*  2006-03-29  P03.10.00.00_00.02.01.02 lrg PortFwd[] von gsy_detail nach   */
/*              GSY_CH_SYS verschoben                                        */
/*  2006-05-12  P03.11.00.00_00.03.01.02 lrg #ifndef GSY_TEST_MODE_FULL      */
/*  2006-05-15  P03.11.00.00_00.03.01.03 lrg GSY_LOWER_RQB_PORT_MACADDR_GET()*/
/*              gsy_OpenLowerDelay(): 2 RQBs fuer die EDD-Diagnosewerte      */
/*  2006-06-28  P03.12.00.00_00.02.01.02 lrg GSY_FILE_SYSTEM_EXTENSION()     */
/*  2006-07-27  P03.12.00.00_00.25.02.02 lrg GSY_RT_CLASS3_SUPPORT_ON        */
/*  2006-08-06  P03.12.00.00_00.25.04.01 js  Avoid compiler warning with gcc */
/*  2006-09-26  P03.12.00.00_00.25.04.02 lrg Neu: GSY_LOWER_RTC2_PARAMS_GET  */
/*  2007-01-15  P03.12.00.00_00.40.06.02 lrg 150107lrg002: Arts-RQ 402778    */
/*              Neuen State nur bei Full Duplex Mode setzen, damit die       */
/*              Delaymessung nicht nach LinkUp beim naechsten Commit startet */
/*  2007-01-18  P03.12.00.00_00.40.07.02 180107lrg001: Test ohne Delay-FU    */
/*  2007-07-09  P04.00.00.00_00.30.01.00 090707lrg001: gsy_SendSyncConfirm() */
/*              _SEND_SYNC, _RECV_SYNC, _CANCEL_SYNC... 170707lrg001         */
/*  2007-08-09  P04.00.00.00_00.30.03.00 080907js001:  falscher sync nicht   */
/*              mit fatal abbrechen.                                         */
/*  2007-08-27  P04.00.00.00_00.30.03.02 270807lrg001:  GSY_LINK_UP_DISABLED */
/*              für Delaymessung ignorieren                                  */
/*  2007-09-13  P04.00.00.00_00.30.05.02 130907lrg001:  Keine Ueberprüfung   */
/*              des Frameheaders mehr bei RECV_SYNC und SEND_SYNC            */
/*  2007-10-15  P04.00.00.00_00.30.05.02 151007lrg002: Traceausgabe TopoOk   */
/*  2008-01-11  P04.00.00.00_00.60.02.02 lrg: Trace +++ LNK NOTE_HIGH->NOTE  */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-06-24  P04.01.00.00_00.01.01.02 240608lrg001: ArtsRQ AP00536908     */
/*              DelaySet zwischen Senderequest und Confirmation verzögern    */
/*  2008-06-24  P04.01.00.00_00.01.02.02 110708lrg001: Arts-RQ AP00664879    */
/*              ExtLinkInd: Speed check: Delaymeasurement only at 100MBit    */
/*  2008-07-24  P04.01.00.00_00.01.02.03 240708lrg001: ArtsRQ AP00676859     */
/*              Check ClockTopoOk at output port on forwarding.              */
/*  2008-07-29  P04.01.00.00_00.01.03.02 290708lrg001: GSY_LOWER_CANCEL_SYNC */
/*              ueberspringen wenn der EDD keine Zeitstempel beherrscht      */
/*  2008-08-25  P04.01.00.00_00.01.03.03 250808lrg001: ArtsRQ AP00691597     */
/*              Stop delay measurement at GSY_LINK_UP_DISABLED               */
/*              CheckIn 2008-10-17 after gen P04.01.00.00_00.02.02.01        */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataSend() versenden                                     */
/*  2009-04-08  P04.01.00.00_00.03.03.02 lrg: K32_RQB_TYPE_DIAG_REQ          */
/*  2009-04-23  P04.01.00.00_00.03.04.02 lrg: EDD_SRV_PTCP_xxx               */
/*              240409lrg001: PortRx/Tx-Delays nur noch aus ExtLinkInd holen */
/*  2009-05-06  P04.02.00.00_00.01.01.01 060509lrg001: LSA_UNUSED_ARG()      */
/*  2009-05-14  P04.02.00.00_00.01.01.02 140509lrg001: gsy_DiagIndK32()      */
/*  2009-07-01  P04.02.00.00_00.01.03.02 lrg: gsy_DiagIndK32()               */
/*              pChUsr = LSA_NULL bei Rate fuer SyncId 1                     */
/*  2009-08-20  P04.02.00.00_00.01.04.02 200809lrg001: gsy_DiagIndK32()      */
/*              Indicate slave Takeover to master                            */
/*  2009-09-29  P04.02.00.00_00.01.04.03 290909lrg001: Retry setting Drift   */
/*  2009-10-08  P04.02.00.00_00.01.04.04 lrg: GSY_HW_TYPE_SLAVE_MSK...       */
/*  2009-11-13  P05.00.00.00_00.01.01.02 131109lrg001: StateSync aus Slave-  */
/*              State bei GSY_DIAG_SOURCE_DRIFT in gsy_DiagIndK32()          */
/*  2009-12-11  P05.00.00.00_00.01.02.02  111209lrg001: ArtsRQ AP00909835    */
/*              Free lower KRISC32 diagnostic RQB if channel is closing      */
/*              141209lrg001: Diagnose: NO Subdomain if K32 slave is stopped */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-03-31  P05.00.00.00_00.01.04.02 310310lrg001: gsy_DiagIndK32()      */
/*              Indicate slave Timeout to master                             */
/*  2010-04-15  P05.00.00.00_00.01.04.03 150410lrg001:                       */
/*              GSY_SLAVE_STATE_SYNC_OUT ist sync                            */
/*              GSY_LOWER_RQB_SEND_PRIO_SET(), GSY_SEND_PRIO_ANNO            */
/*  2010-04-28  P05.00.00.00_00.01.04.04 280410lrg002:                       */
/*              reprovide the receive Rbl with invalid FrameId               */
/*  2010-06-23  P05.00.00.00_00.01.05.02 230610lrg001: gsy_DiagIndK32()      */
/*              TAKEOVER trace with LSA_TRACE_LEVEL_WARN                     */
/*              German comments translated to english                        */
/*  2010-07-20  P05.00.00.00_00.01.06.03 lrg: AP00999110 GSY_TRACE_SR removed*/
/*  2010-08-25  P05.00.00.00_00.01.07.02 250810lrg001: AP00697597            */
/*              German comments translated to english                        */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-07-25  P05.00.00.00_00.05.38.02 lrg: GSY_PROC_ removed from Trace   */
/*  2011-09-27  P05.00.00.00_00.05.56.02 AP01191740 Trace parameter count    */
/*  2012-06-11  P05.00.00.00_00.05.97.02 AP01366052 LSA trace %012u -> %010u */
/*  2012-11-07  P05.02.00.00_00.04.xx.xx AP01447671 GSY_MAC_ADR_TYPE moved   */
/*              from gsy_cfg.h/txt to gsy_pck.h/txt                          */
/*  2012-11-07  P05.03.00.00_00.02.22.01 955647,956018 [GSY] GSY_START_TIMER */
/*              führt zu ACP FATAL                                           */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*  2015-10-02  V06.00, Inc10, RQ 1460015: check GSY_SYNC_ID_SUPPORTED in    */
/*              gsy_DiagIndK32()                                             */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	3		/* GSY_MODULE_ID_LOW */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"
GSY_FILE_SYSTEM_EXTENSION(GSY_MODULE_ID)

/*****************************************************************************/
/* External access function: gsy_request_lower_done()                        */
/* A lower requestblock comes back from lower layer on sync channel.         */
/*****************************************************************************/
LSA_VOID GSY_LOWER_IN_FCT_ATTR gsy_request_lower_done(
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	GSY_CH_SYS_PTR			pChSys;
	LSA_OPCODE_TYPE			Opcode;
	GSY_LOWER_SERVICE_TYPE  Service;
	LSA_USER_ID_TYPE       	UserId;
	GSY_LOWER_RXBYTE_PTR	pMemRx;
	GSY_DRIFT_PTR			pDrift;
	LSA_UINT16				RecvLen;
	LSA_UINT16				FrameId;
	LSA_UINT16				PortId;
	LSA_UINT16				Response;
	LSA_UINT16				LowerRsp;
	LSA_HANDLE_TYPE			Handle;

    GSY_ENTER();

 	/* Read Opcode, Service, UserID and Response from lower RB
	*/
	Opcode = GSY_LOWER_RQB_OPCODE_GET(pRbl);
	Service = GSY_LOWER_RQB_SERVICE_GET(pRbl);
	UserId = GSY_LOWER_RQB_USERID_GET(pRbl);
	LowerRsp = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	Handle = GSY_LOWER_RQB_HANDLE_GET(pRbl);

	GSY_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, ">>> gsy_request_lower_done() pRbl=%X Response=0x%x Handle=%u", 
					pRbl, LowerRsp, Handle);

	switch (LowerRsp)
	{
	case GSY_LOWER_RSP_OK:
		Response = GSY_RSP_OK;
		break;

	case GSY_LOWER_RSP_OK_CANCEL:
		Response = GSY_RSP_OK_CANCEL;
		break;

	default:
		Response = GSY_RSP_ERR_LOWER_LAYER;
		break;
	}

	pChSys = gsy_ChSysGet(Handle);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		GSY_LOWER_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "  > gsy_request_lower_done() Opcode=%u Service=0x%x UserId=0x%x", 
					Opcode, Service, UserId.uvar32);

		switch (Opcode)
		{
		case GSY_LOWER_OPC_OPEN_CHANNEL:
			gsy_OpenLowerGet(pChSys, pRbl);
			break;

		case GSY_LOWER_OPC_CLOSE_CHANNEL:

			/* Finish Close Channel
			*/
			gsy_CloseLowerFin(pChSys, pRbl);
			break;

		case GSY_LOWER_OPC_REQUEST:

			/* Decode Service
			*/
			switch (Service)
			{

			/* ----------------------------------------------------------------
			 * Enable/disable receiving of a multicast address on this channel
			*/
			case GSY_LOWER_MULTICAST:
				if ((pChSys->State == GSY_CHA_STATE_CLOSING)
				||  (pChSys->State == GSY_CHA_STATE_ERROR))
					gsy_CloseLower(pChSys, pRbl, LSA_FALSE);
				else if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL)
				&&  ((GSY_CHA_STATE_OPEN_FWD == pChSys->pChAnno->State)
				 || (GSY_CHA_STATE_CLOSE_FWD == pChSys->pChAnno->State)))
				{
					gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
					(LSA_VOID)gsy_FwdMCConfirmation(pChSys, LowerRsp);
				}
				else
				{
					Response = gsy_OpenLowerDelay(pChSys, pRbl);
				}
				break;

			/* ----------------------------------------------------------------
			*/
			case GSY_LOWER_SYNC_SET:
			case GSY_LOWER_TIME_SET:
				gsy_Rtc2Cnf(pChSys, pRbl);
				break;

			case GSY_LOWER_PARAMS_GET:
				gsy_OpenLowerSet(pChSys, pRbl);
				break;

			case GSY_LOWER_PORT_PARAMS_GET:
				Response = gsy_OpenLowerPort(pChSys, pRbl);
				break;

			case GSY_LOWER_DELAY_SET:

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

				if (Response != GSY_RSP_OK)
				{
					GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_LowerDelaySet() Response=0x%x from GSY_LOWER_DELAY_SET Port=%d", 
									LowerRsp, GSY_LOWER_RQB_SET_LINE_DELAY_PORT_GET(pRbl));
				}
				gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRbl);
				gsy_UserDelayInd(pChSys, GSY_LOWER_RQB_SET_LINE_DELAY_PORT_GET(pRbl), LowerRsp);
#else
				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
				break;

			case GSY_LOWER_RATE_SET:
				pDrift = &pChSys->Drift[GSY_SYNCID_CLOCK][0];
				if (GSY_RSP_OK != Response)
				{
					/* 290909lrg001: Restore old drift values on lower rate set error
					*/
					LSA_UINT8	State = pDrift->State;
					LSA_INT32	Interval = pDrift->AveInterval;

					pDrift->State = pDrift->PendingState;
					pDrift->AveInterval = pDrift->PendingInterval;
					pDrift->PendingState = State;
					pDrift->PendingInterval = Interval;
					pDrift->PendingSet = LSA_TRUE;
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_DriftSet() Response=0x%x from GSY_LOWER_RATE_SET Rate=%d Valid=%u", 
									LowerRsp, GSY_LOWER_RQB_SET_RATE_VALUE_GET(pRbl), GSY_LOWER_RQB_SET_RATE_VALID_GET(pRbl));
				}
				gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRbl);
				break;

			case GSY_LOWER_LINK_STATUS_EXT:
				gsy_PortStatus(pChSys, pRbl);
				break;

			/* ----------------------------------------------------------------
			 * Receive indication of a delay PDU
			*/
			case GSY_LOWER_RECV:

				pMemRx = GSY_LOWER_RQB_RECV_PTR_GET(pRbl);
				if (GSY_RSP_OK == Response)
					RecvLen = GSY_LOWER_RQB_RECV_LEN_GET(pRbl);
				else
					RecvLen = 0;
//				GSY_TRACE_SR(pChSys, pRbl);
				GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "==> gsy recv data:", pMemRx, RecvLen);

				if (RecvLen != 0)
				{
					/* Frame-ID aus dem Puffer holen
					*/
					pMemRx += GSY_LOWER_RQB_RECV_DATA_OFFSET_GET(pRbl);
					FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemRx));
				}
				else
					FrameId = 0;

				gsy_Indication(pChSys, pRbl, UserId.uvar32, FrameId);
				break;

			/* ----------------------------------------------------------------
			 * Receive indication of a  Sync, SyncFU or FollowUp PDU
			*/
			case GSY_LOWER_RECV_SYNC:
			{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
				LSA_UINT16				SyncId;

				pMemRx = GSY_LOWER_RQB_RECV_SYNC_PTR_GET(pRbl);
				if (GSY_RSP_OK == Response)
					RecvLen = GSY_LOWER_RQB_RECV_SYNC_LEN_GET(pRbl);
				else
					RecvLen = 0;
//				GSY_TRACE_SR(pChSys, pRbl);
				GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "==> gsy recv sync:", pMemRx, RecvLen);

				if (RecvLen != 0)
				{
					/* Get 5Bit Sync-ID and Frame-ID from receive buffer
					*/
					pMemRx += GSY_LOWER_RQB_RECV_SYNC_DATA_OFFSET_GET(pRbl);
					FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemRx));
					SyncId = FrameId & GSY_SYNCID_MASK;
				}
				else
					SyncId = FrameId = 0;

				if ((pChSys->State == GSY_CHA_STATE_CLOSING)
				||  (pChSys->State == GSY_CHA_STATE_CLOSE_FWD)
				||  (pChSys->State == GSY_CHA_STATE_ERROR)
				||  (Response == GSY_LOWER_RSP_OK_CANCEL))
				{
					/* Free Recv-RQB
					*/
					GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnew*/,"  - gsy_request_lower_done() GSY_LOWER_RECV_SYNC: pChSys=%X Response=0x%x ReqestCnt=%u", 
	 						pChSys, Response, GSY_LOWER_RQB_RECV_SYNC_CNT_GET(pRbl));
					gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
					pRbl = LSA_NULL;
				}
				else if (((GSY_FRAMEID_SYNC_MIN <= FrameId) && (GSY_FRAMEID_SYNC_MAX >= FrameId))
				 ||  ((GSY_FRAMEID_FUSYNC_MIN <= FrameId) && (GSY_FRAMEID_FUSYNC_MAX >= FrameId))
				 ||  ((GSY_FRAMEID_FU_MIN <= FrameId) && (GSY_FRAMEID_FU_MAX >= FrameId)))
				{
					/* Indicate RQB to forwarding module
					*/
					(LSA_VOID)gsy_FwdRcvIndication(pChSys, pRbl, LSA_TRUE);
				}
				else
				{
					GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_request_lower_done() Invalid RECV_SYNC: FrameId=0x%04x SyncId=0x%02x", 
								FrameId, SyncId);

					/* 280410lrg002: reprovide the receive Rbl with invalid FrameId
					*/
					gsy_SyncRecv(pChSys, pRbl, 0);
				}
#else
				GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_request_lower_done() RECV_SYNC when GSY_CFG_SYNC_ONLY_IN_KRISC32 is defined");
				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_RECEIVE, LSA_NULL, 0);
	
#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
				break;
			}
	
			/* ----------------------------------------------------------------
			 * Send confirmation of a delay PDU
			*/
			case GSY_LOWER_SEND:
			case GSY_LOWER_SEND_TS:
			{
				LSA_UINT16				Word;
				GSY_LOWER_TXBYTE_PTR	pMemTx;

				pMemTx = GSY_LOWER_RQB_SEND_PTR_GET(pRbl);
				pMemTx += 12;
				Word = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemTx));

				/* VLAN tag or ethertype
				*/
				if (Word == GSY_ETHETYPE)
					pMemTx += 2;
				else
					pMemTx += 6;
				FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemTx));

				gsy_SendConfirm(pChSys, pRbl, FrameId);
				break;
			}

			/* ----------------------------------------------------------------
			 * Send confirmation of a Sync, SyncFU oder FollowUp PDU
			*/
			case GSY_LOWER_SEND_SYNC:
				/* 130907lrg001:
				 * 240608lrg001: Set LineDelay delayed
				*/
				PortId = GSY_LOWER_RQB_SEND_SYNC_PORT_GET(pRbl);
				pChSys->Port[PortId-1].SendCnfPending--;
				if ((pChSys->Port[PortId-1].DelaySetPending)
				&&  (0 == pChSys->Port[PortId-1].SendCnfPending))
				{
					/* Give LineDelay to FWD, EDD and User
					*/
					GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_request_lower_done() pChSys=%X Port=%u DelaySetPending=%u SendCnfPending=%u", 
	 						pChSys, PortId, pChSys->Port[PortId-1].DelaySetPending, pChSys->Port[PortId-1].SendCnfPending);
					gsy_DelaySet(pChSys, PortId);
				}

				FrameId = GSY_LOWER_RQB_SEND_SYNC_FRAMEID_GET(pRbl);

				/* If the frame has not been sent by the master
				 * and the FrameID is OK...
				*/
				if (LSA_HOST_PTR_ARE_EQUAL(UserId.void_ptr, LSA_NULL)
				&&  ((
#if GSY_FRAMEID_SYNC_MIN != 0  /* avoid gcc warning */
					  (GSY_FRAMEID_SYNC_MIN <= FrameId) &&
#endif
					  (GSY_FRAMEID_SYNC_MAX >= FrameId))
				 ||  ((GSY_FRAMEID_FUSYNC_MIN <= FrameId) && (GSY_FRAMEID_FUSYNC_MAX >= FrameId))
				 ||  ((GSY_FRAMEID_FU_MIN <= FrameId) && (GSY_FRAMEID_FU_MAX >= FrameId))))
				{
					/* ...call the forwarding modules confirmation
					*/
					(LSA_VOID)gsy_FwdSendConfirmation(pChSys, pRbl);
				}
				else
				{
					gsy_SendSyncConfirm(pChSys, pRbl, FrameId);
				}
				break;

			case GSY_LOWER_CANCEL:
				if ((pChSys->State == GSY_CHA_STATE_CLOSING)
				||  (pChSys->State == GSY_CHA_STATE_ERROR))
				{
					if (GSY_HW_TYPE_SLAVE_SW == (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
					{
						/* Cancel sync recv if sync services are supported
						*/
						(LSA_VOID)gsy_CancelLower(pChSys, LSA_TRUE, 0, 0, pRbl, GSY_LOWER_CANCEL_SYNC);
					}
					else if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
					{
						/* No delay multicast address is enabled 
						 * and no sync services are supported
						*/
						gsy_CloseLower(pChSys, pRbl, LSA_FALSE);
					}
					else
					{
						/* 290708lrg001: Skip GSY_LOWER_CANCEL_SYNC 
						*/
						LSA_UINT8 DelayMcAddr[] = GSY_MULTICAST_ADDR_DELAY;
						(LSA_VOID)gsy_MucAddrSet(pChSys, pRbl, DelayMcAddr, LSA_FALSE, LSA_FALSE);
					}
				}
				else
				{
					gsy_CancelConfirm(pChSys, pRbl);
				}
				break;

			case GSY_LOWER_CANCEL_SYNC:
				if ((pChSys->State == GSY_CHA_STATE_CLOSING)
				||  (pChSys->State == GSY_CHA_STATE_ERROR))
				{
					/* Disablen Delay multicast address 
					*/
					LSA_UINT8 DelayMcAddr[] = GSY_MULTICAST_ADDR_DELAY;
					(LSA_VOID)gsy_MucAddrSet(pChSys, pRbl, DelayMcAddr, LSA_FALSE, LSA_FALSE);
				}
				else
					gsy_CancelConfirm(pChSys, pRbl);
				break;

			case GSY_LOWER_SYNC_CTRL:
			case GSY_LOWER_DELAY_CTRL:

				/* Put lower RQB back into queue
				*/
				gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRbl);

				if ((LowerRsp != GSY_RSP_OK) && (LowerRsp != GSY_LOWER_RSP_OK_CANCEL))
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_request_lower_done()  pRbl=%X: Response=0x%x from Service=%u ", 
									pRbl, LowerRsp, Service);
				    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
				}
				break;

			case GSY_LOWER_SYNC_DIAG:

				/* Lower KRISC32 diagnostic RQB
				*/
				if ((LowerRsp != GSY_RSP_OK) && (LowerRsp != GSY_LOWER_RSP_OK_CANCEL))
				{
					GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_request_lower_done()  pRbl=%X: Response=0x%x from Service GSY_LOWER_SYNC_DIAG", 
									pRbl, LowerRsp);
				    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
				}
				else if (GSY_LOWER_RSP_OK_CANCEL == LowerRsp)
				{
					/* Put lower RQB back into queue
					*/
					gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRbl);
				}
				else
				{
					gsy_DiagIndK32(pChSys, pRbl);
				}
				break;

 			default:
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_done() pChSys=%X pRbl=%X: unknown service=0x%x", 
							pChSys, pRbl, Service);
			    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_BAD_SERVICE, pRbl, 
			    				sizeof(GSY_LOWER_RQB_TYPE));
			}
			break;
			
			/* End of: case GSY_LOWER_OPC_REQUEST
			*/

		default:
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_done() pChSys=%X pRbl=%X: unknown opcode=0x%x", 
						pChSys, pRbl, Opcode);
		    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_BAD_OPCODE, pRbl, 
		    				sizeof(GSY_LOWER_RQB_TYPE));
		}
	}
	else
	{
		/* No Channel
		*/
		GSY_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_done() pRbl=%X Handle=%u Opcode=%u Service=0x%x: no sync channel", 
				pRbl, Handle, Opcode, Service);
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
	}

	GSY_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< gsy_request_lower_done() pRbl=%X pChSys=%X Response=0x%x", 
					pRbl, pChSys, Response);
    GSY_EXIT();
}

/*****************************************************************************/
/* External access function: gsy_request_lower_anno_done()                   */
/* A lower requestblock comes back from lower layer on announce channel.     */
/*****************************************************************************/
LSA_VOID GSY_LOWER_IN_FCT_ATTR gsy_request_lower_anno_done(
GSY_LOWER_RQB_PTR_TYPE  pRbl)
{
	GSY_CH_SYS_PTR			pChSys = LSA_NULL;
	GSY_CH_ANNO_PTR			pChAnno;
	GSY_CH_USR_PTR			pChUsr;
	LSA_OPCODE_TYPE			Opcode;
	GSY_LOWER_SERVICE_TYPE  Service;
	LSA_USER_ID_TYPE       	UserId;
	GSY_LOWER_RXBYTE_PTR	pMemRx;
	LSA_UINT16				RecvLen;
	LSA_UINT16				FrameId;
	LSA_UINT16				Response;
	LSA_UINT16				LowerRsp;
	LSA_HANDLE_TYPE			Handle;

    GSY_ENTER();

 	/* Get Opcode, Service, UserID und Response from lower RB
	*/
	Opcode = GSY_LOWER_RQB_OPCODE_GET(pRbl);
	Service = GSY_LOWER_RQB_SERVICE_GET(pRbl);
	UserId = GSY_LOWER_RQB_USERID_GET(pRbl);
	LowerRsp = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	Handle = GSY_LOWER_RQB_HANDLE_GET(pRbl);

	GSY_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, ">>> gsy_request_lower_anno_done() pRbl=%X Response=0x%x Handle=%u", 
					pRbl, LowerRsp, Handle);

	switch (LowerRsp)
	{
	case GSY_LOWER_RSP_OK:
		Response = GSY_RSP_OK;
		break;

	case GSY_LOWER_RSP_OK_CANCEL:
		Response = GSY_RSP_OK_CANCEL;
		break;

	default:
		Response = GSY_RSP_ERR_LOWER_LAYER;

#ifdef GSY_MESSAGE
		 /* Todo: support error structure
		  * 
		if (LSA_COMP_ID_UNUSED != GSY_LOWER_RQB_ERR_COMPID_GET(_pRbl))
		    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, pRbl, 
		    				sizeof(GSY_LOWER_RQB_TYPE));
		*/ 
#endif /* GSY_MESSAGE */
	}

	pChAnno = gsy_ChAnnoGet(Handle);
	if (!LSA_HOST_PTR_ARE_EQUAL(pChAnno, LSA_NULL))
	{
		GSY_LOWER_TRACE_03(pChAnno->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "  > gsy_request_lower_anno_done() Opcode=%u Service=0x%x UserId=0x%x", 
					Opcode, Service, UserId.uvar32);

		pChSys = pChAnno->pChSys;
		if (LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
		{
			GSY_ERROR_TRACE_04(pChAnno->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_anno_done() pChAnno=%X pRbl=%X Handle=%u Service=0x%x: pChSys is NULL", 
				pChAnno, pRbl, Handle, Service);
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
		}
		else
		{
			switch (Opcode)
			{
			case GSY_LOWER_OPC_OPEN_CHANNEL:
				(LSA_VOID)gsy_OpenLowerFwd(pChSys, pRbl);
				break;

			case GSY_LOWER_OPC_CLOSE_CHANNEL:

				/* Finish close channel
				*/
				gsy_CloseAnnoFin(pChSys, pRbl);
				break;

			case GSY_LOWER_OPC_REQUEST:

				/* Decode Service
				*/
				switch (Service)
				{

				/* ----------------------------------------------------------------
				 * Switch on/off local receive of announce MC address
				*/
				case GSY_LOWER_MULTICAST:
					pChUsr = pChSys->pChUsr;
					if (LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
					{
						/* No user channel ?!?
						*/
						gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);
						GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_request_lower_anno_done() pChSys=%X GSY_LOWER_MULTICAST: No user channel",
									pChSys);
					}
					else if ((GSY_LOWER_RQB_MULTICAST_MODE_GET(pRbl) == EDD_MULTICAST_ENABLE)
					&&  (GSY_LOWER_RSP_OK == LowerRsp))
					{
						/* MasterStart: Store lower RQB for cancel at master stop
						*/
						pChUsr->pCancelRbl = pRbl;
					}
					else
					{
						/* MasterStop or lower error: Free lower RQB
						*/
						pChUsr->pCancelRbl = LSA_NULL;
						gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);

						if (GSY_LOWER_RSP_OK != LowerRsp)
						{
							/* NOTE: Error in PrmCommit -> MasterStart/Stop is FATAL
							*/
							GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_request_lower_anno_done() pChSys=%x LowerRsp=0x%04x from GSY_LOWER_MULTICAST",
									pChSys, LowerRsp);
							gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_MC_REQUEST, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
						}
						else
						{
							gsy_MasterStop(pChUsr);
						}
					}
					break;

				/* ----------------------------------------------------------------
				 * Announce PDU received
				*/
				case GSY_LOWER_RECV:

					pMemRx = GSY_LOWER_RQB_RECV_PTR_GET(pRbl);
					if (GSY_RSP_OK == Response)
						RecvLen = GSY_LOWER_RQB_RECV_LEN_GET(pRbl);
					else
						RecvLen = 0;

//					GSY_TRACE_SR(pChSys, pRbl);
					GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "==> gsy recv anno:", pMemRx, RecvLen);

					if (RecvLen != 0)
					{
						/* Get FrameID from buffer
						*/
						pMemRx += GSY_LOWER_RQB_RECV_DATA_OFFSET_GET(pRbl);
						FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemRx));
					}
					else
						FrameId = 0;

					gsy_IndicationAnno(pChSys, pRbl, FrameId);
					break;

				/* ----------------------------------------------------------------
				 * Send confirmation of announce PDU
				*/
				case GSY_LOWER_SEND:
				{
					LSA_UINT16				Word;
					GSY_LOWER_TXBYTE_PTR	pMemTx;

					pMemTx = GSY_LOWER_RQB_SEND_PTR_GET(pRbl);
					pMemTx += 12;
					Word = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemTx));

					/* VLAN tag or ethertype
					*/
					if (Word == GSY_ETHETYPE)
						pMemTx += 2;
					else
						pMemTx += 6;
					FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pMemTx));

					if ((GSY_FRAMEID_ANNOUNCE_MIN <= FrameId) && (GSY_FRAMEID_ANNOUNCE_MAX >= FrameId))
					{
						if (LSA_HOST_PTR_ARE_EQUAL(UserId.void_ptr, LSA_NULL))
						{
							/* 101208lrg001: Announce PDU sent by forwarding
							*/
							(LSA_VOID)gsy_FwdSendConfirmation(pChSys, pRbl);
						}
						else
						{
							/* Announce PDU sent by master
							*/
							gsy_SendConfirm(pChSys, pRbl, FrameId);
						}
					}
					else
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_anno_done() pChSys=%X pRbl=%X: unknown FrameId=0x%x in send cnf", 
										pChSys, pRbl, FrameId);
					    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
					}
					break;
				}

				case GSY_LOWER_CANCEL:
					if (GSY_LOWER_CANCEL_MODE_ALL == GSY_LOWER_RQB_CANCEL_MODE_GET(pRbl))
					{
						/* CancelAll done: close announce channel
						*/
						gsy_CloseLower(pChSys, pRbl, LSA_TRUE);
					}
					else
					{
						/* Master stopped: disable announce multicast address and stop master
						*/
						GSY_MAC_ADR_TYPE AnnoMcAddr = {GSY_MULTICAST_ADDR_ANNOUNCE};

						pChUsr = (GSY_CH_USR_PTR)GSY_LOWER_RQB_USER_PTR_GET(pRbl);
						AnnoMcAddr.MacAdr[5] |= pChUsr->SyncId;
						(LSA_VOID)gsy_MucAddrSet(pChSys,	pRbl, AnnoMcAddr.MacAdr, LSA_FALSE, LSA_TRUE);
						
					}
					break;

				default:
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_anno_done() pChSys=%X pRbl=%X: unknown service=0x%x", 
									pChSys, pRbl, Service);
				    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_BAD_SERVICE, pRbl, sizeof(GSY_LOWER_RQB_TYPE));

				/* End of: case(Service)
				*/
				}
				break;
				

			default:
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_anno_done() pChSys=%X pRbl=%X: unknown opcode=0x%x", 
								pChSys, pRbl, Opcode);
			    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_BAD_OPCODE, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
			}
		}
	}
	else
	{
		/* Kein Channel
		*/
		GSY_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** gsy_request_lower_anno_done() pRbl=%X Handle=%u Opcode=%u Service=0x%x: no announce channel", 
				pRbl, Handle, Opcode, Service);
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CH_PTR, pRbl, sizeof(GSY_LOWER_RQB_TYPE));
	}

	GSY_LOWER_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< gsy_request_lower_anno_done() pRbl=%X pChSys=%X pChAnno=%X", 
					pRbl, pChSys, pChAnno);
    GSY_EXIT();
}

/*****************************************************************************/
/* Internal function: gsy_DiagIndK32()                                       */
/* Diagnosis RQB returns from KRISC32 slave.                                 */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DiagIndK32(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT8			SyncId = GSY_LOWER_RQB_K32_DIAG_ID_GET(pRbl);
	LSA_UINT8			SlaveState = GSY_LOWER_RQB_K32_DIAG_STATE_GET(pRbl);
	GSY_MAC_ADR_TYPE	MasterMacAddr = {GSY_NO_MASTER_ADDR};
	LSA_UINT8			RcvSyncPrio = GSY_LOWER_RQB_K32_DIAG_PRIO_GET(pRbl);
	LSA_UINT16			SequenceId = GSY_LOWER_RQB_K32_DIAG_SEQID_GET(pRbl);
	LSA_INT32			RateInterval = GSY_LOWER_RQB_K32_DIAG_RATE_GET(pRbl);
	LSA_UINT8			DiagSrc = GSY_LOWER_RQB_K32_DIAG_SOURCE_GET(pRbl);
	GSY_CH_USR_PTR		pChUsr = pChSys->pChUsr;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DiagIndK32() pChSys=%X pRbl=%d pChUsr=%X",
						pChSys, pRbl, pChUsr);

	GSY_LOWER_RQB_K32_DIAG_ADDR_GET(MasterMacAddr, pRbl);		//041112lrg001

	GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"-+- gsy_DiagIndK32(%02x) Source=%u SeqId=%u Master=%02x-%02x-%02x-%02x-%02x-%02x", 
					SyncId, DiagSrc, SequenceId, MasterMacAddr.MacAdr[0], MasterMacAddr.MacAdr[1],
					MasterMacAddr.MacAdr[2], MasterMacAddr.MacAdr[3], MasterMacAddr.MacAdr[4], MasterMacAddr.MacAdr[5]);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChUsr, LSA_NULL))
	{
		LSA_UINT8	OldSlaveState = pChUsr->Slave.State;
		LSA_BOOL	NewSlaveState = LSA_FALSE;
		GSY_DRIFT_PTR		pDrift = pChUsr->Slave.pDrift;

		if (LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
		&& (GSY_DIAG_SOURCE_DRIFT == DiagSrc)
//		&& ((GSY_SYNCID_CLOCK == SyncId) || (GSY_SYNCID_TIME == SyncId)) )
		&& (0 != (((LSA_UINT32)(SyncId + 1)) & GSY_SYNC_ID_SUPPORTED)))
		{
			/* 160909lrg001: set slave pointer to drift structure
			*/
			pChUsr->Slave.pDrift = &pChSys->Drift[SyncId][0];
			pDrift = pChUsr->Slave.pDrift;
		}
		if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL) 
		&&  (GSY_DIAG_SOURCE_SUBDOMAIN == DiagSrc) && (GSY_SLAVE_STATE_OFF == SlaveState))
		{
			/* Slave is stopped
			*/
			pDrift->Diag.Subdomain = LSA_FALSE;	//141209lrg001: Diagnose: NO Subdomain

			gsy_DiagUserInd(DiagSrc, pChUsr->pChSys, pDrift, 
			    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
							pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
							0       						   /* LSA_INT32: Offset */, 
							pChUsr->Slave.AdjustInterval       /* LSA_INT32: AdjustInterval */, 
							pChUsr->Slave.State                /* LSA_INT32: UserValue1 */, 
							2 	    						   /* LSA_INT32: UserValue2 */, 
							0                                  /* LSA_INT32: SetTimeHigh */, 
							0                                  /* LSA_INT32: SetTimeLow */, 
							pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */);

			gsy_SlaveStop(pChUsr);
		}
		else if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL))	//140509lrg001
		{
			if (SyncId == pDrift->SyncId)
			{
				if (SlaveState != OldSlaveState)
				{
					NewSlaveState = LSA_TRUE;
					GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_DiagIndK32(%02x) SeqId=%u: slave state change %u->%u", 
								SyncId, SequenceId, OldSlaveState, SlaveState);
				}

				/* Set Values from K32 slave
				*/
				pChUsr->Slave.State = SlaveState;
				pChUsr->Slave.SeqId = SequenceId;
				pChUsr->Slave.MasterAddr = MasterMacAddr;
			}
			else if ((GSY_SYNCID_CLOCK == SyncId) || (GSY_SYNCID_TIME == SyncId))
			{
				//Select the Drift depending on the SyncId and keep the Slave unchanged
				pDrift = &pChSys->Drift[SyncId][0];
			}
			pDrift->MasterAddr = MasterMacAddr;
			pDrift->AveInterval = RateInterval;
			pDrift->RcvSyncPrio = RcvSyncPrio;
			pDrift->SyncId = SyncId;

			GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--+ gsy_DiagIndK32(%02x) OCFInterval=%d AveOffset=%d RCFInterval=%d RcvSyncPrio=%02x SlaveState=%u SeqId=%u MasterState=%u",
			SyncId, pChUsr->Slave.AdjustInterval, pChUsr->Slave.AveTimeOffset, RateInterval, RcvSyncPrio, pChUsr->Slave.State, pChUsr->Slave.SeqId, pChUsr->Master.State);

			switch (DiagSrc)
			{
			case GSY_DIAG_SOURCE_SUBDOMAIN:

				if (GSY_SUBDOMAIN_IS_NULL(pChUsr->SyncData.SubdomainID))
				{
					pDrift->Diag.Subdomain = LSA_FALSE;
				}
				else
				{
					pDrift->Diag.Subdomain = LSA_TRUE;
				}

				if ((GSY_SLAVE_STATE_FIRST == SlaveState)
			    &&  (GSY_SYNC_EVENT_NONE == pChUsr->Event.SyncEvent))
				{
					/* Slave started: send first PrmIndication to the user
					*/
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK; 
					gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
				}
				break;

			case GSY_DIAG_SOURCE_MASTER:

				if ((NewSlaveState) && (GSY_SLAVE_STATE_WAIT == SlaveState))
				{
					/* Slave Timeout
					*/
					if (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
					{
						/* 310310lrg001: Indicate slave Timeout to master
						*/
						gsy_MasterAging(pChUsr);
						pDrift->Diag.StateSync = LSA_TRUE;
					}
					else
					{
						pDrift->Diag.StateSync = LSA_FALSE;
						if ((GSY_MASTER_STATE_PRIMARY != pChUsr->Master.State)
						&&  (GSY_MASTER_STATE_STARTUP != pChUsr->Master.State)
						&&  (GSY_MASTER_STATE_QUICKUP != pChUsr->Master.State))
						{
							/* 231006lrg002: Do not indicate timeout if primary Master waits
							*/
							pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED; 
							gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
						}
					}
				}
				else
				{
					LSA_BOOL StateSync = LSA_FALSE;

					/* NewMaster, Takeover or change of RcvSyncPrio 
					*/
					if (GSY_MACADDR_IS_NULL(pChUsr->Slave.MasterAddr))
					{
						/* Slave Takeover 230610lrg001:	LEVEL_NOTE_HIGH -> LEVEL_WARN
						*/
						GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"--- gsy_DiagIndK32(%02x) SeqId=%u MasterState=%u: slave state change [%u]->TAKEOVER (%u)", 
							SyncId, SequenceId, pChUsr->Master.State, OldSlaveState, SlaveState);
						if (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
						{
							/* 200809lrg001: Indicate slave Takeover to master
							*/
							gsy_MasterAging(pChUsr);
							StateSync = LSA_TRUE;
						}
						else if (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)
							StateSync = LSA_TRUE;
					}
					else
					{
						/* NewMaster or change of RcvSyncPrio 
						 * 150410lrg001: GSY_SLAVE_STATE_SYNC_OUT ist sync
						*/
						if ((GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)
						||  (GSY_SLAVE_STATE_SYNC_OUT == pChUsr->Slave.State)
						||  (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
						||  (GSY_MASTER_STATE_SLAVE == pChUsr->Master.State))
						{
							StateSync = LSA_TRUE;
						}
					}
					pDrift->Diag.StateSync = StateSync;
				}
				break;

			case GSY_DIAG_SOURCE_ADJUST:

				if ((GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State)
				&&  (GSY_SLAVE_STATE_SYNC_IN == OldSlaveState))
				{
					DiagSrc = GSY_DIAG_SOURCE_SYNC;
					pDrift->Diag.StateSync = LSA_TRUE;
					if (GSY_SYNC_EVENT_SYNC_OK != pChUsr->Event.SyncEvent)
					{
						/* Send PrmIndication if not a change from state FIRST to SYNC
						*/
						pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_SYNC_OK; 
						gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
					}
				}
				else if (GSY_SLAVE_STATE_OUT == pChUsr->Slave.State)
				{
					/* Indicate NOT_SYNC and send PrmIndication
					*/
					DiagSrc = GSY_DIAG_SOURCE_SYNC;
					pDrift->Diag.StateSync = LSA_FALSE;
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY; 
					gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
				}
				break;

			case GSY_DIAG_SOURCE_SET:

				if ((GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY != pChUsr->Event.SyncEvent)
				&&  (GSY_SLAVE_STATE_FIRST != OldSlaveState))
				{
					/* PrmIndication NOT_SYNC for SET TIME
					*/
					pDrift->Diag.StateSync = LSA_FALSE;
					pChUsr->Event.SyncEvent = GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY;
					gsy_SyncUserInd(pChUsr, LSA_FALSE, LSA_TRUE);
				}
				break;

			case GSY_DIAG_SOURCE_DRIFT:

				if (GSY_MASTER_STATE_PRIMARY == pChUsr->Master.State)
				{
					/* Indication from lower master rate set
					*/
					pDrift->State = GSY_DRIFT_STATE_MASTER;
					pDrift->IntervalSetValid = LSA_TRUE;
					pDrift->Diag.StateSync = LSA_TRUE;
				}
				else
				{
					if (0 == RateInterval)
					{
						pDrift->State = GSY_DRIFT_STATE_UNKNOWN;
						pDrift->IntervalSetValid = LSA_FALSE;
					}
					else
					{
						pDrift->State = GSY_DRIFT_STATE_READY;
						pDrift->IntervalSetValid = LSA_TRUE;
					}
					if ((GSY_MASTER_STATE_SLAVE == pChUsr->Master.State)
					||  (GSY_SLAVE_STATE_SYNC == pChUsr->Slave.State))		//131109lrg001
					{
						pDrift->Diag.StateSync = LSA_TRUE;
					}
					else
					{
						pDrift->Diag.StateSync = LSA_FALSE;
					}
				}
				GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_DiagIndK32(%02x) SeqId=%u Interval=%d Valid=%u: drift state change ->[%u]", 
					SyncId, SequenceId, RateInterval, pDrift->IntervalSetValid, pDrift->State);
				break;
/*
			case GSY_DIAG_SOURCE_OFFSET:
			case GSY_DIAG_SOURCE_SYNC:
*/
			default:

				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** gsy_DiagIndK32() pChSys=%X SyncId=%X: Invalid diagnosis source=%u", 
								pChSys, SyncId, DiagSrc);
			}

			/* Diagnosedaten aktualisieren und Indication hochgeben
			*/
			gsy_DiagUserInd(DiagSrc, pChUsr->pChSys, pDrift, 
			    			pChUsr->Slave.Act.OrgSeconds       /* LSA_UINT32: MasterSeconds */, 
							pChUsr->Slave.Act.OrgNanos         /* LSA_UINT32: MasterNanoseconds */, 
							0       						   /* LSA_INT32: Offset */, 
							pChUsr->Slave.AdjustInterval       /* LSA_INT32: AdjustInterval */, 
							pChUsr->Slave.State                /* LSA_INT32: UserValue1 */, 
							2 	    						   /* LSA_INT32: UserValue2 */, 
							0                                  /* LSA_INT32: SetTimeHigh */, 
							0                                  /* LSA_INT32: SetTimeLow */, 
							pChUsr->Slave.SyncPortId 		   /* LSA_INT16: PortId */);
		}
	}

	if ((pChSys->State == GSY_CHA_STATE_CLOSING)
	||  (pChSys->State == GSY_CHA_STATE_CLOSE_FWD))
	{
		/* 111209lrg001: free lower KRISC32 diagnostic RQB if channel is closing
		*/
		gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		pRbl = LSA_NULL;
	}
	else
	{
		/* Reprovide lower KRISC32 diagnostic RQB
		*/
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DiagIndK32() pRbl=0x%X", pRbl);
}

/*****************************************************************************/
/* Internal function: gsy_PortStatus()                                       */
/* PortLinkStatusIndication RQB is back from lower layer.                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_PortStatus(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT32	CableDelay = GSY_LOWER_RQB_LINK_STATUS_CABLEDELAY_GET(pRbl);
	LSA_UINT32	LineDelay = GSY_LOWER_RQB_LINK_STATUS_LINEDELAY_GET(pRbl) * pChSys->RxTxNanos;
	LSA_UINT16	Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	LSA_UINT16	PortId = GSY_LOWER_RQB_LINK_STATUS_PORT_GET(pRbl);
	LSA_UINT8	Status =  GSY_LOWER_RQB_LINK_STATUS_STATUS_GET(pRbl);
	LSA_UINT8	Mode =  GSY_LOWER_RQB_LINK_STATUS_MODE_GET(pRbl);
	LSA_UINT8	Speed =  GSY_LOWER_RQB_LINK_STATUS_SPEED_GET(pRbl);		// 110708lrg001
//	LSA_UINT8	Speed =  EDD_LINK_SPEED_10;								// 110708lrg001
	LSA_BOOL	TopoOk =  GSY_LOWER_RQB_LINK_STATUS_TOPO_OK_GET(pRbl);
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT8	PortState = pChSys->Port[Idx].State;


	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_PortStatus() pChSys=%X pRbl=%X Port16/Status8/PortState8=0x%08x", 
					pChSys, pRbl, ((((LSA_UINT32)PortId) << 16)+(((LSA_UINT32)Status) << 8)+PortState));

	GSY_DEL_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "+++ LNK(%x,%x) Port=%u TopoOk=%u Mode=0x%02x Speed=0x%02x Status=0x%02x PortState=0x%02x RxDelay=%u TxDelay=%u", 
		pChSys, Response, PortId, TopoOk, Mode, Speed, Status, PortState, 
		GSY_LOWER_RQB_LINK_STATUS_RXDELAY_GET(pRbl), GSY_LOWER_RQB_LINK_STATUS_TXDELAY_GET(pRbl));

	if ((pChSys->State == GSY_CHA_STATE_CLOSING)
	||  (pChSys->State == GSY_CHA_STATE_ERROR)
	||  (Response == GSY_LOWER_RSP_OK_CANCEL))
	{
		/* Free LinkStatusIndication RQB
		*/
		gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		pRbl = LSA_NULL;
	}
	else if (Response == GSY_LOWER_RSP_OK)
	{
		if ((LineDelay != pChSys->Port[Idx].SyncLineDelay) && ((0 == LineDelay) || (0 == pChSys->Port[Idx].SyncLineDelay)))
		{
			GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_PortStatus(%u) delay state change: Cable=%u->%u Line=%u->%u", 
						PortId, pChSys->Port[Idx].CableDelay, CableDelay, pChSys->Port[Idx].SyncLineDelay, LineDelay);
		}
		if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
		{
			/* Update line delay (in ticks) and cable delay measured by gsy in krisc32
			*/
			if ((CableDelay != pChSys->Port[Idx].CableDelay)
			||  (LineDelay != pChSys->Port[Idx].SyncLineDelay))
			{
				if ((0 != LineDelay) && (0 != pChSys->Port[Idx].SyncLineDelay))
				{
					GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- gsy_PortStatus(%u) [%x%x%x] Update Delay (old/new) Cable=%u/%u Line=%u/%u", 
								PortId, 0, 0, 0, pChSys->Port[Idx].CableDelay, CableDelay, pChSys->Port[Idx].SyncLineDelay, LineDelay);
				}
				pChSys->Port[Idx].SumCount++; 
				pChSys->Port[Idx].CableDelay = CableDelay;
				pChSys->Port[Idx].SyncLineDelay = LineDelay;
				if ((pChSys->Port[Idx].MaxDelay == 0) || (pChSys->Port[Idx].MaxDelay < CableDelay))
					pChSys->Port[Idx].MaxDelay = CableDelay;
				if ((pChSys->Port[Idx].MinDelay == 0) || (pChSys->Port[Idx].MinDelay > CableDelay))
					pChSys->Port[Idx].MinDelay = CableDelay;
				gsy_DelaySet(pChSys, PortId);
			}
		}

		switch (Status)
		{
		case GSY_LINK_DOWN:

			if ((GSY_PORT_STATE_UP == PortState) 
			||  (GSY_PORT_STATE_CLOSED == PortState))
//			||  (GSY_PORT_STATE_DISABLED == PortState))		250808lrg001
				{
				/* Stop Delay Measurement
				*/
				GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PortStatus(%u) link state change ->DOWN", PortId);
				gsy_DelayReqStop(pChSys, PortId, GSY_LOWER_DELAY_ACTIVE);
			}
			pChSys->Port[Idx].State = GSY_PORT_STATE_DOWN;
			break;

		case GSY_LINK_UP:
		case GSY_LINK_UP_CLOSED:
									/* 270807lrg001: MRP switches port to DISABLED instead of CLOSED and
									 * GSY gets the response EDD_STS_ERR_TX on sending
		case GSY_LINK_UP_DISABLED:
									*/
			/* Update port delays
			*/
			pChSys->Port[Idx].RxDelay = GSY_LOWER_RQB_LINK_STATUS_RXDELAY_GET(pRbl);
			pChSys->Port[Idx].TxDelay = GSY_LOWER_RQB_LINK_STATUS_TXDELAY_GET(pRbl);
			if (pChSys->Port[Idx].ClockSyncOk != TopoOk)
			{
				/* 151007lrg002: Trace
				*/
				GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PortStatus(%u) topo state change ->[%d]", PortId, TopoOk);
				pChSys->Port[Idx].ClockSyncOk = TopoOk;
			}

			if (
#ifndef GSY_TEST_MODE_FULL
				(GSY_LINK_MODE_FULL == Mode) &&
				(EDD_LINK_SPEED_100 == Speed) &&
#endif
		        ((GSY_PORT_STATE_DOWN == PortState) 
			 ||  (GSY_PORT_STATE_DISABLED == PortState)
			 ||  (GSY_PORT_STATE_INIT == PortState)))
			{
				GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PortStatus(%u) link state change ->[%u]", PortId, Status);

				/* Start Delay-Measurement on LINK_UP_xxx after DOWN or INIT state
				 * 150107lrg002: Set new State only on Mode = Full Duplex,
				 * so that the Delay-Measurement starts not on the next Commit
				 * 110708lrg001: Check Speed: Delay-Measurement only at 100MHz
				 * 250808lrg001: Restart Delay-Measurement after DISABLED
				*/
				gsy_DelayReqStart(pChSys, PortId);

				if (GSY_LINK_UP_CLOSED == Status)
					pChSys->Port[Idx].State = GSY_PORT_STATE_CLOSED;
//250808lrg001	else if (GSY_LINK_UP_DISABLED == Status)					// 181105lrg001
//250808lrg001		pChSys->Port[Idx].State = GSY_PORT_STATE_DISABLED;
				else
					pChSys->Port[Idx].State = GSY_PORT_STATE_UP;
			}
			break;

		case GSY_LINK_UP_DISABLED:

			if ((GSY_PORT_STATE_UP == PortState) 
			||  (GSY_PORT_STATE_CLOSED == PortState))
			{
				GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_PortStatus(%u) link state change ->DISABLED", PortId);

				/* 250808lrg001: Stop Delay-Measurement on GSY_LINK_UP_DISABLED
				*/
				gsy_DelayReqStop(pChSys, PortId, GSY_LOWER_DELAY_ACTIVE);
			}
			pChSys->Port[Idx].State = GSY_PORT_STATE_DISABLED;
			break;

		default:
			break;

		}
		/* Pass LinkStatusIndication to Forwarding
		 * 240708lrg001: TopoOk -> Forwarding
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL)
		&&  (GSY_CHA_STATE_OPEN == pChSys->pChAnno->State))
		{
			(LSA_VOID)gsy_FwdLinkStatus(pChSys, PortId, Status, Mode, TopoOk);
		}
	}
	else
	{
		/* ExtLinkIndication with error
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_PortStatus() pChSys=%X pRbl=%X: Invalid Response=0x%04x", 
						pChSys, pRbl, Response);
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_RECEIVE, pRbl, Response);
	}

	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* Return RQB to lower Layer
		*/
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_LINK_STATUS_EXT);

		/* Send request
		*/
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_PortStatus() pChSys=%X pRbl=%X new PortState=%X", 
						pChSys, pRbl, pChSys->Port[Idx].State);

	LSA_UNUSED_ARG(Speed);
}

/*****************************************************************************/
/* Internal function: gsy_Indication()                                       */
/* Receive-RQB comes back from lower layer on sync channel.                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_Indication(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT32				CancelId,
LSA_UINT16				FrameId)
{
	LSA_UINT16 Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	LSA_UINT16 PortId = GSY_LOWER_RQB_RECV_PORT_GET(pRbl);
	LSA_UINT16 DataLength = GSY_LOWER_RQB_RECV_DATA_LEN_GET(pRbl);  
	LSA_UINT16 DataOffset = GSY_LOWER_RQB_RECV_DATA_OFFSET_GET(pRbl);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_Indication() pChSys=%X pRbl=%X Offset16/Length16=0x%08x", 
					pChSys, pRbl, ((((LSA_UINT32)DataOffset) << 16)+(LSA_UINT32)DataLength));

	if ((pChSys->State == GSY_CHA_STATE_CLOSING)
	||  (pChSys->State == GSY_CHA_STATE_CLOSE_FWD)
	||  (pChSys->State == GSY_CHA_STATE_ERROR)
	||  (Response == GSY_LOWER_RSP_OK_CANCEL))
	{
		/* Free Recv-RQB
		*/
		GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnew*/,"  - gsy_Indication() GSY_LOWER_RECV: pChSys=%X Response=0x%x ReqestCnt=%u", 
	 			pChSys, Response, GSY_LOWER_RQB_RECV_CNT_GET(pRbl));
		gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		pRbl = LSA_NULL;
	}
	else if (Response == GSY_LOWER_RSP_OK)
	{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
		/* Delay PDU
		*/
		if (pChSys->PortInit == pChSys->PortCount)
		{
			LSA_UINT32 TimeStamp = GSY_LOWER_RQB_RECV_TIME_GET(pRbl);
			GSY_LOWER_RXBYTE_PTR pMemRx = (GSY_LOWER_RXBYTE_PTR)GSY_LOWER_RQB_RECV_PTR_GET(pRbl);

			/* Skip PDU-Header inclusive FrameID
			*/
			GSY_LOWER_RX_PDU_PTR pPduRx = (GSY_LOWER_RX_PDU_PTR)(void*)(pMemRx + DataOffset + 2);
			DataLength -= 2;

			GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "++> RCV(%x,%x) Port=%u FrameId=0x%04x SeqId=%u Delay10=%010u Delay1=%u TimeStamp=%u", 
							pChSys, Response, PortId, FrameId, GSY_SWAP16(pPduRx->DelayReq.PtcpHdr.SequenceID.Word), 
							GSY_SWAP32(pPduRx->DelayReq.PtcpHdr.Delay10ns.Dword), pPduRx->DelayReq.PtcpHdr.Delay1ns, TimeStamp);
			switch (FrameId)
			{
			default:
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** gsy_Indication() Invalid FrameId=0x%04x", 
								FrameId);
				break;

			case GSY_FRAMEID_DELAY_REQ:

				/* Receive delay request PDU
				*/
				gsy_DelayReqInd(pChSys, PortId, pPduRx, DataLength, TimeStamp);
				break;

			case GSY_FRAMEID_DELAY_FURSP:

				/* Receive delay response PDU with FollowUp
				*/
				gsy_DelayRspInd(pChSys, PortId, pPduRx, DataLength, TimeStamp, LSA_TRUE);
				break;

			case GSY_FRAMEID_DELAY_RSP:

				/* Receive delay response PDU without FollowUp
				*/
				gsy_DelayRspInd(pChSys, PortId, pPduRx, DataLength, TimeStamp, LSA_FALSE);
				break;

			case GSY_FRAMEID_DELAY_FU:

				/* Receive delay FollowUp
				*/
				gsy_DelayFuInd(pChSys, PortId, pPduRx, DataLength);
				break;
			}
		}
#else
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_Indication() Invalid RECV: FrameId=0x%04x", 
						FrameId);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
	}
	else
	{
		/* ReceiveIndication with error
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_Indication() pChSys=%X pRbl=%X: Invalid Response=0x%04x", 
						pChSys, pRbl, Response);
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_RECEIVE, pRbl, Response);
	}

	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* Send RQB to lower layer again
		*/
		gsy_DataRecv(pChSys, pRbl, CancelId);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_Indication() pChSys=%X pRbl=%X CancelId=0x%08x", 
					pChSys, pRbl, CancelId);
	LSA_UNUSED_ARG(PortId);		//060509lrg001
}

/*****************************************************************************/
/* Internal function: gsy_IndicationAnno()                                   */
/* Receive-RQB comes back from lower layer on announce channel.              */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_IndicationAnno(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT16				FrameId)
{
	LSA_UINT16 Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_IndicationAnno() pChSys=%X pChAnno=%X pRbl=%X", 
					pChSys, pChSys->pChAnno, pRbl);

	if ((pChSys->pChAnno->State == GSY_CHA_STATE_CLOSING)
	||  (pChSys->pChAnno->State == GSY_CHA_STATE_CLOSE_FWD)
	||  (pChSys->pChAnno->State == GSY_CHA_STATE_ERROR)
	||  (Response == GSY_LOWER_RSP_OK_CANCEL))
	{
		/* Free Recv-RQB
		*/
		GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelnew*/,"  - gsy_IndicationAnno() GSY_LOWER_RECV: pChSys=%X Response=0x%x ReqestCnt=%u", 
	 			pChSys, Response, GSY_LOWER_RQB_RECV_CNT_GET(pRbl));
		gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);
	}
	else if (Response == GSY_LOWER_RSP_OK)
	{
		if ((GSY_FRAMEID_ANNOUNCE_MIN <= FrameId) && (GSY_FRAMEID_ANNOUNCE_MAX >= FrameId))
		{
			/* 101208lrg001: Pass Announce PDU to Forwarding
			*/
			Response = gsy_FwdRcvIndication(pChSys, pRbl, LSA_FALSE);
		}
		else
		{
			GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_IndicationAnno() Invalid RECV: FrameId=0x%04x", 
							FrameId);

			/* 280410lrg002: reprovide the receive Rbl with invalid FrameId
			*/
			gsy_AnnoRecv(pChSys, pRbl, 0);
		}
	}
	else
	{
		/* ReceiveIndication with error
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_IndicationAnno() pChSys=%X pRbl=%X: Invalid Response=0x%04x", 
						pChSys, pRbl, Response);
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_RECEIVE, pRbl, Response);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_IndicationAnno() pChSys=%X pRbl=%X Response=0x%x", 
					pChSys, pRbl, Response);
}

/*****************************************************************************/
/* Internal function: gsy_SendConfirm()                                      */
/* Delay- or Announce-Send-RQB comes up from lower layer.                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SendConfirm(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT16				FrameId)
{
	GSY_LOWER_TX_PDU_PTR	pPduTx = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);
	LSA_UINT16				PortId = GSY_LOWER_RQB_SEND_PORT_GET(pRbl);
	LSA_UINT16				Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SendConfirm() pChSys=%X pRbl=%X PortId16/Response16=0x%08x", 
					pChSys, pRbl, ((((LSA_UINT32)PortId) << 16)+(LSA_UINT32)Response));

	if ((GSY_FRAMEID_ANNOUNCE_MIN <= FrameId) && (GSY_FRAMEID_ANNOUNCE_MAX >= FrameId))
	{
		GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<++ SND(%x,%x) Port=%u FrameId=0x%04x SeqId=%u", 
					pChSys, Response, PortId, FrameId, GSY_SWAP16(pPduTx->PtcpData.Announce.PtcpHdr.SequenceID.Word));

		/* 101208lrg001: Announce sent by the Master
		*/
		gsy_MasterAnnoCnf((GSY_CH_USR_PTR)(GSY_LOWER_RQB_USER_PTR_GET(pRbl)), pRbl);
	}
	else
	{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<++ SND(%x,%x) Port=%u FrameId=0x%04x SeqId=%u Delay10=%010u DelayNS=%d TS=%u", 
					pChSys, Response, PortId, FrameId, GSY_SWAP16(pPduTx->PtcpData.DelayReq.PtcpHdr.SequenceID.Word), 
					GSY_SWAP32(pPduTx->PtcpData.DelayReq.PtcpHdr.Delay10ns.Dword),
					GSY_SWAP32(pPduTx->PtcpData.DelayReq.PtcpHdr.DelayNS.Dword), GSY_LOWER_RQB_SEND_TIME_GET(pRbl));

		/* Delay PDUs:
		*/
		switch (FrameId)
		{
		default:
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"  < gsy_SendConfirm() pChSys=%X pRbl=%X FrameId=0x%4x", 
							pChSys, pRbl, FrameId);
		    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pPduTx, FrameId);
			break;

		case GSY_FRAMEID_DELAY_REQ:
			gsy_DelayReqCnf(pChSys, Response, PortId, pPduTx, GSY_LOWER_RQB_SEND_TIME_GET(pRbl));
			break;

		case GSY_FRAMEID_DELAY_FURSP:
			gsy_DelayRspCnf(pChSys, Response, PortId, pPduTx, GSY_LOWER_RQB_SEND_TIME_GET(pRbl));
			break;

//		case GSY_FRAMEID_DELAY_RSP:		 180107lrg001: Test without FU
		case GSY_FRAMEID_DELAY_FU:
			break;
		}

#else
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_SendConfirm() Unexpected Send.cnf: Port=%u FrameId=0x%04x", 
							PortId, FrameId);
	    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pPduTx, FrameId);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32

		/* Put RQB again into lower Queue
		*/
		gsy_EnqueLower(&pChSys->QueueLowerSend, pRbl);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SendConfirm() pChSys=%X pRbl=%X QueueCount=0x%x", 
					pChSys, pRbl, pChSys->QueueLowerSend.Count);
}

/*****************************************************************************/
/* Internal function: gsy_SendSyncConfirm()                                  */
/* Sync-,Fu-Send-RQB comes up from lower layer.                              */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SendSyncConfirm(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT16				FrameId)
{
	GSY_LOWER_TX_PDU_PTR	pPduTx = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
	LSA_UINT16				PortId = GSY_LOWER_RQB_SEND_SYNC_PORT_GET(pRbl);
	LSA_UINT16				Response = GSY_LOWER_RQB_RESPONSE_GET(pRbl);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SendSyncConfirm() pChSys=%X pRbl=%X PortId16/Response16=0x%08x", 
					pChSys, pRbl, ((((LSA_UINT32)PortId) << 16)+(LSA_UINT32)Response));

	GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<++ SND(%x,%x) Port=%u FrameId=0x%04x SeqId=%u Delay10=%010u DelayNS=%d TS=%u", 
					pChSys, Response, PortId, FrameId, GSY_SWAP16(pPduTx->PtcpData.Sync.PtcpHdr.SequenceID.Word), 
					GSY_SWAP32(pPduTx->PtcpData.Sync.PtcpHdr.Delay10ns.Dword),
					GSY_SWAP32(pPduTx->PtcpData.Sync.PtcpHdr.DelayNS.Dword), GSY_LOWER_RQB_SEND_SYNC_TIME_GET(pRbl));

	if (((GSY_FRAMEID_FUSYNC_MIN <= FrameId) && (GSY_FRAMEID_FUSYNC_MAX >= FrameId))
	||  ((GSY_FRAMEID_SYNC_MIN <= FrameId) && (GSY_FRAMEID_SYNC_MAX >= FrameId)))
	{
		/* SyncFrame sent by the Master
		*/
		gsy_MasterSyncCnf((GSY_CH_USR_PTR)(GSY_LOWER_RQB_USER_PTR_GET(pRbl)), pRbl);
	}
	else if ((GSY_FRAMEID_FU_MIN <= FrameId) && (GSY_FRAMEID_FU_MAX >= FrameId))
	{
		/* FollowUp sent by the Master
		*/
		gsy_MasterFuCnf((GSY_CH_USR_PTR)(GSY_LOWER_RQB_USER_PTR_GET(pRbl)), pRbl);
	}
//	else if ((GSY_FRAMEID_ANNOUNCE_MIN <= FrameId) && (GSY_FRAMEID_ANNOUNCE_MAX >= FrameId))
//	{
		/* FollowUp vom Master
		*/
//		gsy_MasterAnnoCnf((GSY_CH_USR_PTR)(GSY_LOWER_RQB_USER_PTR_GET(pRbl)), pRbl);
//	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"  < gsy_SendSyncConfirm() pChSys=%X pRbl=%X FrameId=0x%4x", 
						pChSys, pRbl, FrameId);
	    gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pPduTx, FrameId);

		/* Put RQB again into lower Queue
		*/
		gsy_EnqueLower(&pChSys->QueueLowerSend, pRbl);      //lint !e527 Unreachable code, if gsy_ErrorInternal() is an exit (but this must not be), HM 10.06.2016
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SendSyncConfirm() pChSys=%X pRbl=%X QueueCount=0x%x", 
					pChSys, pRbl, pChSys->QueueLowerSend.Count);
	LSA_UNUSED_ARG(PortId);
	LSA_UNUSED_ARG(Response);
}

/*****************************************************************************/
/* Internal function: gsy_CancelConfirm()                                    */
/* Cancel-RQB comes up from lower layer.                                     */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_CancelConfirm(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_CancelConfirm() pChSys=%X pRbl=%X", 
					pChSys, pRbl);

	GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL, "*** gsy_CancelConfirm() pChSys=%X pRbl=%X: Unexpected Cancel confirmation",
					pChSys, pRbl);

    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_CANCEL, pChSys, pChSys->Handle);

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_CancelConfirm()");
	LSA_UNUSED_ARG(pRbl);
}

/*****************************************************************************/
/* Internal access function: gsy_OpenLowerStart()                            */
/* Starts lower open channel by allocating a lower request block.            */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerStart(
GSY_CH_SYS_PTR  			pChSys,
LSA_BOOL					ChAnno)
{
	LSA_USER_ID_TYPE        UserId;
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	GSY_LOWER_QUEUE			*pQueue;
	LSA_SYS_PTR_TYPE		pSys;
	LSA_UINT16 				RbuRespVal = GSY_RSP_OK_ACTIVE;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerStart() pChSys=%X ChAnno=%u", 
					pChSys, ChAnno);

	pRbl = LSA_NULL;

	UserId.uvar32 = 0;
	if (ChAnno)
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->pChAnno->Handle;
		pSys = pChSys->pChAnno->pSys;
		pQueue = &pChSys->pChAnno->QueueLower; 
	}
	else
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->Handle;
		pSys = pChSys->pSys;
		pQueue = &pChSys->QueueLowerSend; 
	}

	/* Request lower RQB
	*/
	GSY_ALLOC_LOWER_RQB(&pRbl, UserId, sizeof(GSY_LOWER_RQB_TYPE), pSys);

	/* If the lower RB has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* ...put it into the channels queue and request lower parameter block
		*/
		GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
		gsy_EnqueLower(pQueue, pRbl);
		RbuRespVal = gsy_OpenLowerMem(pChSys, ChAnno);
	}
	else
	{
		/* Next step: get RQB asynchronously by gsy_lower_rqb()
		 * is Currently not supported!
		*/
		RbuRespVal = GSY_RSP_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerStart() pChSys=%X pRbl=%X RetVal=0x%x", 
					pChSys, pRbl, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerMem()                                     */
/* Allocate lower memory for parameter block.                                */
/*****************************************************************************/

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerMem(
GSY_CH_SYS_PTR  			pChSys,
LSA_BOOL					ChAnno)
{
	LSA_USER_ID_TYPE        UserId;
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	GSY_LOWER_MEM_PTR_TYPE	pMem;
	GSY_LOWER_QUEUE			*pQueue;
	LSA_SYS_PTR_TYPE		pSys;
	LSA_UINT16 				RbuRespVal = GSY_RSP_OK_ACTIVE;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerMem() pChSys=%X ChAnno=%u", 
					pChSys, ChAnno);

	UserId.uvar32 = 0;
	if (ChAnno)
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->pChAnno->Handle;
		pSys = pChSys->pChAnno->pSys;
		pQueue = &pChSys->pChAnno->QueueLower; 
	}
	else
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->Handle;
		pSys = pChSys->pSys;
		pQueue = &pChSys->QueueLowerSend; 
	}
	pMem = LSA_NULL;

	/* Request lower memory
	*/
	GSY_ALLOC_LOWER_MEM(&pMem, UserId, sizeof(GSY_LOWER_PARAM_TYPE), pSys);

	/* ...put pointer to the memory or NULL into queued RQB 
	*/
	pRbl = pQueue->pFirst;
	GSY_LOWER_RQB_PPARAM_SET(pRbl, pMem);

	/* If the lower PB has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
	{
		/* ...open lower channel
		*/
		gsy_OpenLowerCh(pChSys, ChAnno);
	}
	else
	{
		/* Next step: get RQB asynchronously by gsy_lower_mem()
		 * is Currently not supported!
		*/
		gsy_DequeLower(pQueue, pRbl);
		GSY_FREE_LOWER_RQB(&RbuRespVal, pRbl, pSys);
		RbuRespVal = GSY_RSP_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerMem() pChSys=%X pMem=%X RetVal=0x%x", 
					pChSys, pMem, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerCh()                                      */
/* Open lower channel.                                                       */
/*****************************************************************************/

LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerCh(
GSY_CH_SYS_PTR  			pChSys,
LSA_BOOL					ChAnno)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	GSY_LOWER_QUEUE			*pQueue;
	LSA_SYS_PTR_TYPE		pSys;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerCh() pChSys=%X ChAnno=%u", 
					pChSys, ChAnno);

	if (ChAnno)
	{
		pSys = pChSys->pChAnno->pSys;
		pQueue = &pChSys->pChAnno->QueueLower; 
	}
	else
	{
		pSys = pChSys->pSys;
		pQueue = &pChSys->QueueLowerSend; 
	}
	pRbl = pQueue->pFirst;
	
	/* Fill lower requst block for open_channel()
	*/
	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_OPEN_CHANNEL);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, 0);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

	if (ChAnno)
	{
		GSY_LOWER_RQB_HANDLE_SET(pRbl, pChSys->pChAnno->Handle);
		GSY_LOWER_RQB_HANDLE_UPPER_SET(pRbl, pChSys->pChAnno->Handle);
		GSY_LOWER_RQB_SYS_PATH_SET(pRbl, pChSys->pChAnno->SysPath);
		GSY_LOWER_RQB_CBF_PTR_SET(pRbl, gsy_request_lower_anno_done);
	}
	else
	{
		GSY_LOWER_RQB_HANDLE_SET(pRbl, pChSys->Handle);
		GSY_LOWER_RQB_HANDLE_UPPER_SET(pRbl, pChSys->Handle);
		GSY_LOWER_RQB_SYS_PATH_SET(pRbl, pChSys->SysPath);
		GSY_LOWER_RQB_CBF_PTR_SET(pRbl, gsy_request_lower_done);
	}

	/* Get requst block from queue and send it to lower layer
	*/
	gsy_DequeLower(pQueue, pRbl);
	GSY_OPEN_CHANNEL_LOWER(pRbl, pSys);

	/* At callback: gsy_request_lower(_anno)_done() and gsy_OpenLowerGet()
	*/

 	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerCh() pChSys=%X pRbl=%X", 
 					pChSys, pRbl);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerGet()                                     */
/* Read parameters (MAC Addresse, number of ports) of the lower channel      */
/*****************************************************************************/

LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerGet(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerGet() pChSys=%X pRbl=%X", 
					pChSys, pRbl);

	if (GSY_LOWER_RQB_RESPONSE_GET(pRbl) == GSY_LOWER_RSP_OK)
	{
		/* Store lower channel handle in channel data structure
		*/
		pChSys->HandleLower = GSY_LOWER_RQB_HANDLE_LOWER_GET(pRbl);

		/* Fill Lower Requstblock for REQUEST, Service: GetParams
		*/
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_PARAMS_GET);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

		/* Send Lower Request
		*/
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);

		/* On Callback: gsy_request_lower_done() and gsy_LowerSet()
		*/
	}
	else
	{
		/* Error opening Lower Layer Channel
		*/
		gsy_OpenLowerError(pChSys, pRbl, GSY_RSP_ERR_LOWER_LAYER);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerGet() pChSys=%X pRbl=%X", 
					pChSys, pRbl);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerSet()                                     */
/* Activate multicast address for Delay Frames.                              */
/*****************************************************************************/

LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerSet(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT8			DelayMcAddr[] = GSY_MULTICAST_ADDR_DELAY;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerSet() pChSys=%X pRbl=%X", 
					pChSys, pRbl);

	if (GSY_LOWER_RQB_RESPONSE_GET(pRbl) == GSY_LOWER_RSP_OK)
	{
		/* Store parameters of the channel
		*/
		pChSys->PortCount = GSY_LOWER_RQB_PARAM_PORTS_GET(pRbl);
		GSY_LOWER_RQB_PARAM_MACADDR_GET(pChSys->MACAddr, pRbl);			//041112lrg001
		pChSys->RxTxNanos = GSY_LOWER_RQB_PARAM_NANOSECONDS_GET(pRbl);
		pChSys->HardwareType = GSY_LOWER_RQB_PARAM_HWTYPE_GET(pRbl);
		pChSys->TraceIdx = GSY_LOWER_RQB_PARAM_TRACEIDX_GET(pRbl);
		if ((pChSys->PortCount > GSY_CH_MAX_PORTS)
		||  ((GSY_HW_TYPE_MASTER_HW == (pChSys->HardwareType & GSY_HW_TYPE_MASTER_MSK))
		 &&  (GSY_HW_TYPE_SLAVE_HW != (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))))
		{
			/* Too many ports or wrong HW sync type
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** gsy_OpenLowerSet() pChSys=%X too many ports (%u) or unsupported HW sync type (0x%08x)", 
							pChSys, pChSys->PortCount, (pChSys->HardwareType & (GSY_HW_TYPE_MASTER_MSK|GSY_HW_TYPE_SLAVE_MSK|GSY_HW_TYPE_DELAY_MSK)));
			gsy_OpenLowerError(pChSys, pRbl, GSY_RSP_ERR_CONFIG);
		}
		else if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
		{
			/* Do not enable delay multicast address on system channels
			 * with delay measurement in hardware (KRISC32)
			*/
			(LSA_VOID)gsy_OpenLowerDelay(pChSys, pRbl);
		}
		else
		{
			/* Enable Delay multicast address and on callback:
			 * gsy_request_lower_done() and gsy_OpenLowerDelay()
			*/
			(LSA_VOID)gsy_MucAddrSet(pChSys,	pRbl, DelayMcAddr, LSA_TRUE, LSA_FALSE);
		}

	}
	else
	{
		/* Error reading parameters of lower channel 
		*/
		gsy_OpenLowerError(pChSys, pRbl, GSY_RSP_ERR_LOWER_LAYER);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerSet() pChSys=%X pRbl=%X", 
					pChSys, pRbl);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerDelay()                                   */
/* Allocate delay timer for every port and get port parameter.               */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerDelay(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	GSY_LOWER_RQB_PTR_TYPE	pRb;
	LSA_INT					NumRqb;
	LSA_UINT16 				PortId;
	LSA_UINT16 				RetVal;
	LSA_UINT16 				RbuRespVal = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerDelay() pChSys=%X pRbl=%X", 
					pChSys, pRbl);

	if (GSY_LOWER_RQB_RESPONSE_GET(pRbl) != GSY_LOWER_RSP_OK)
	{
		/* Error enableing delay multicast address
		*/
		RbuRespVal = GSY_RSP_ERR_LOWER_LAYER;
	}
	else
	{
		/* State is open lower
		*/
		pChSys->State = GSY_CHA_STATE_OPEN_LOW;
	}

	if ((RbuRespVal == GSY_RSP_OK)
	&&  (pChSys->RxTxNanos != 0))
	{
		/* To get PHY- and MAC Delays of all ports...
		*/
		for (PortId = 0; PortId < pChSys->PortCount; PortId++)
		{
			/* ...allocate new RQB with memory for parameters 
			 * and put it into lower Queue
			*/
			RetVal = gsy_AllocEthParam(pChSys, &pRb, LSA_FALSE);
			if (LSA_RET_OK == RetVal)
			{
				gsy_EnqueLower(&pChSys->QueueLowerSend, pRb);
			}
			else
			{
				RbuRespVal = GSY_RSP_ERR_RESOURCE;
				break;
			}

			if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
			{
				/* Allocate RQBs for KRISC32 delay control
				*/
				GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerDelay(0x%x, %u) Allocating %u HW delay ctrl RQBs [%x%x%x%x%x]",
							pChSys, PortId+1, GSY_NUM_RQB_DELAY_CTRL,0,0,0,0,0); 
				for (NumRqb = 0; NumRqb < GSY_NUM_RQB_DELAY_CTRL; NumRqb++)
				{
					RetVal = gsy_AllocEthKrisc32(pChSys);
					if (LSA_RET_OK != RetVal)
					{
						/* break inner loop
						*/
						break;
					}
				}
			}
			else
			{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

				/* Allocate delay timer for this port
				*/
				RetVal = gsy_TimerAlloc(&pChSys->Port[PortId].DelayTimer, 
											LSA_TIMER_TYPE_ONE_SHOT,
											GSY_DELAY_TIME_BASE);
				if (LSA_RET_OK != RetVal)
				{
					RbuRespVal = GSY_RSP_ERR_RESOURCE;
					break;
				}
				/* Allocate RQBs with memory for parameters for DelaySet requests
				 * and put it into lower Queue
				*/
				GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerDelay(0x%x, %u) Allocating %u delay set RQBs [%x%x%x%x%x]",
							pChSys, PortId+1, GSY_NUM_RQB_DELAY_SET,0,0,0,0,0); 
				for (NumRqb = 0; NumRqb < GSY_NUM_RQB_DELAY_SET; NumRqb++)
				{
					RetVal = gsy_AllocEthParam(pChSys, &pRb, LSA_FALSE);
					if (LSA_RET_OK == RetVal)
						gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRb);
					else
						/* break inner loop
						*/
						break;
				}
#else
				gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
			}
			if (LSA_RET_OK != RetVal)
			{
				/* break outer loop
				*/
				RbuRespVal = GSY_RSP_ERR_RESOURCE;
				break;
			}
		}

		if (RbuRespVal == GSY_RSP_OK)
		{
			if (GSY_HW_TYPE_SLAVE_HW == (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
			{
				/* Get lower RQBs for Clock- und TimeSync control in KRISC32
				*/
				GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerDelay(0x%x, -) Allocating %u HW diag/sync ctrl RQBs [%x%x%x%x%x%x]",
							pChSys, GSY_NUM_RQB_SYNC_CTRL,0,0,0,0,0,0); 
				for (NumRqb = 0; NumRqb < GSY_NUM_RQB_SYNC_CTRL; NumRqb++)
				{
					RetVal = gsy_AllocEthKrisc32(pChSys);
					if (LSA_RET_OK != RetVal)
					{
						RbuRespVal = GSY_RSP_ERR_RESOURCE;
						break;
					}
				}

				/* Provide Diagnostic RQBs to KRISC32 slave
				*/
				for (NumRqb=0; NumRqb < GSY_NUM_RQB_DIAG_CTRL; NumRqb++)
				{
					GSY_LOWER_RQB_PTR_TYPE pLowerRbl = pChSys->QueueLowerCtrl.pFirst;
					if (LSA_HOST_PTR_ARE_EQUAL(pLowerRbl, LSA_NULL))
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_OpenLowerDelay() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_SYNC_CTRL=%u GSY_NUM_RQB_DELAY_CTRL=%u GSY_NUM_RQB_DIAG_CTRL=%u",
										GSY_NUM_RQB_SYNC_CTRL, GSY_NUM_RQB_DELAY_CTRL, GSY_NUM_RQB_DIAG_CTRL);
						gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);
						break;      //lint !e527 Unreachable code, if gsy_ErrorInternal() is an exit (but this must not be), HM 10.06.2016
					}
					else
					{
						gsy_DequeLower(&pChSys->QueueLowerCtrl, pLowerRbl);
						GSY_LOWER_RQB_OPCODE_SET(pLowerRbl, GSY_LOWER_OPC_REQUEST);
                        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
						GSY_LOWER_RQB_LOWER_HANDLE_SET(pLowerRbl, pChSys->HandleLower);
						GSY_LOWER_RQB_SERVICE_SET(pLowerRbl, GSY_LOWER_SYNC_DIAG);
						GSY_LOWER_RQB_ERR_COMPID_SET(pLowerRbl, LSA_COMP_ID_UNUSED);

						GSY_REQUEST_LOWER(pLowerRbl, pChSys->pSys);
					}
				}

			}
			else
			{
				/* Requestbloecke mit Parameterpuffer fuer Rate (Drift) Set-Requests
				 * fuer ClockSync allokieren und in die lower Queue stecken
				*/
				/* Allocate RQBs with memory for parameters for Rate(Drift)Set requests
				 * for ClockSync and put it into lower Queue
				*/
				GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerDelay(0x%x, -) Allocating %u drift set RQBs [%x%x%x%x%x%x]",
							pChSys, GSY_NUM_RQB_RATE_SET,0,0,0,0,0,0); 
				for (NumRqb = 0; NumRqb < GSY_NUM_RQB_RATE_SET; NumRqb++)
				{
					RetVal = gsy_AllocEthParam(pChSys, &pRb, LSA_FALSE);
					if (LSA_RET_OK == RetVal)
						gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRb);
					else
						break;
				}
			}
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

			if (RbuRespVal == GSY_RSP_OK)
			{
				/* Allocate cyclic timer for rate calculation
				*/
				RetVal = gsy_TimerAlloc(&pChSys->DriftTimer,
										LSA_TIMER_TYPE_CYCLIC,
										GSY_DRIFT_TIME_BASE);
				if (RetVal != LSA_RET_OK)
					RbuRespVal = GSY_RSP_ERR_RESOURCE;
			}
#endif
			if (RbuRespVal == GSY_RSP_OK)
			{
				/* Request PHY- and MAC Delays of all ports
				*/
				for (PortId = 1; PortId <= pChSys->PortCount; PortId++)
				{
					/* Dequeue Lower RQB 
					 * and fill for REQUEST, Service: GetPortParams
					*/
					pRb = pChSys->QueueLowerSend.pFirst;
					gsy_DequeLower(&pChSys->QueueLowerSend, pRb);

					GSY_LOWER_RQB_OPCODE_SET(pRb, GSY_LOWER_OPC_REQUEST);
                    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
					GSY_LOWER_RQB_LOWER_HANDLE_SET(pRb, pChSys->HandleLower);
					GSY_LOWER_RQB_SERVICE_SET(pRb, GSY_LOWER_PORT_PARAMS_GET);
					GSY_LOWER_RQB_PORT_DELAY_ID_SET(pRb, PortId);
					GSY_LOWER_RQB_ERR_COMPID_SET(pRb, LSA_COMP_ID_UNUSED);

					/* Send Request
					*/
					GSY_REQUEST_LOWER(pRb, pChSys->pSys);

					/* On callback: gsy_request_lower_done() and gsy_OpenLowerFwd()
					*/
				}
			}
		}
	}

	if (RbuRespVal == GSY_RSP_OK)
	{
		/* Store Lower Open Channel RQB for close channel
		*/
		pChSys->pOpenRbl = pRbl;
		if (pChSys->RxTxNanos == 0)
		{
			/* If the lower layer supports no timestamps:
			 * finish OPEN_CHANNEL now with OK without allocating
			 * delay timers and Receive RQBs. PortInit remains = 0. 
			*/
			gsy_CallbackSys(pChSys, LSA_NULL, GSY_RSP_OK);
		}
	}
	else
		gsy_OpenLowerError(pChSys, pRbl, RbuRespVal);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerDelay() pChSys=%X pRbl=%X RetVal=0x%x", 
					pChSys, pRbl, RbuRespVal);

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerPort()                                    */
/* Store Port delays, allocate Receive-RQBs and send them to lower layer,    */
/* allocate Send RQBs and queue them.                                        */
/* Send one Link-Status-Indication-Request to lower layer.                   */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerPort(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT16 	PortId;
	LSA_UINT16 	RbuRespVal = pChSys->Resp;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerPort() pChSys=%X pRbl=%X PortInit=%d", 
					pChSys, pRbl, pChSys->PortInit);

	pChSys->PortInit++;

	if ((RbuRespVal == GSY_RSP_OK)
	&& (GSY_LOWER_RQB_RESPONSE_GET(pRbl) != GSY_LOWER_RSP_OK))
	{
		/* Error reading port parameter
		*/
		RbuRespVal = GSY_RSP_ERR_LOWER_LAYER;
	}

	if (RbuRespVal == GSY_RSP_OK)
	{
		/* Store Port-MAC-Address
		 * 240409lrg001: Get PortRx/Tx-Delays only from ExtLinkInd
		*/
		PortId = GSY_LOWER_RQB_PORT_DELAY_ID_GET(pRbl);
		GSY_LOWER_RQB_PORT_MACADDR_GET(pChSys->Port[PortId-1].MACAddr, pRbl);		//041112lrg001

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerPort($$) Port=%u MAC=%02x-%02x-%02x-%02x-%02x-%02x [%x]", 
				PortId, pChSys->Port[PortId-1].MACAddr.MacAdr[0], pChSys->Port[PortId-1].MACAddr.MacAdr[1],
				pChSys->Port[PortId-1].MACAddr.MacAdr[2], pChSys->Port[PortId-1].MACAddr.MacAdr[3], 
				pChSys->Port[PortId-1].MACAddr.MacAdr[4], pChSys->Port[PortId-1].MACAddr.MacAdr[5], 0);

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

		if (GSY_HW_TYPE_SLAVE_SW == (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
		{
			LSA_INT		i;

			/* If SyncServices are supported on this channel:
			 * allocate lower Request/data blocks for (Forwarding-) SYNC and FU
			*/
			GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerPort(0x%x, %u) Allocating %u sync/fu recv RQBs [%x%x%x%x%x]",
							pChSys, PortId, GSY_NUM_RQB_SYNCFU_RECV,0,0,0,0,0); 
			for (i = 0; i < GSY_NUM_RQB_SYNCFU_RECV; i++)
			{
				/* Allocate Receive Requestblock with data buffer and send RQB to lower Layer
				*/
				RbuRespVal = gsy_AllocEthRecv(pChSys, LSA_NULL, LSA_NULL, 0, GSY_SYNC_TYPE_ID, LSA_FALSE);
				if (GSY_RSP_OK != RbuRespVal)
					break;
			}
		}
#endif
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
		
		if (RbuRespVal == GSY_RSP_OK)
		{
			LSA_INT		i;

			if (GSY_HW_TYPE_DELAY_HW != (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
			{
				/* Allocate lower Request/data blocks for DELAY RECV Request/Response/FollowUp
				*/
				GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerPort(0x%x, %u) Allocating %u delay recv RQBs [%x%x%x%x%x]",
							pChSys, PortId, GSY_NUM_RQB_DELAY_RECV,0,0,0,0,0); 
				for (i = 0; i < GSY_NUM_RQB_DELAY_RECV; i++)
				{
					/* Allocate Receive Requestblock with data buffer and send RQB to lower Layer
					*/
					RbuRespVal = gsy_AllocEthRecv(pChSys, LSA_NULL, LSA_NULL, 0, GSY_DELAY_TYPE_ID, LSA_FALSE);
					if (GSY_RSP_OK != RbuRespVal)
						break;
				}

				if (RbuRespVal == GSY_RSP_OK)
				{
					/* Allocate lower Request/data blocks for DELAY SEND Request/Response/FollowUp
					*/
					GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  - gsy_OpenLowerPort(0x%x, %u) Allocating %u delay send RQBs [%x%x%x%x%x]",
								pChSys, PortId, GSY_NUM_RQB_DELAY_SEND,0,0,0,0,0); 
					for (i = 0; i < GSY_NUM_RQB_DELAY_SEND; i++)
					{
						/* Allocate Send Requestblock with data buffer and queue it
						*/
						RbuRespVal = gsy_AllocEthSend(pChSys, &pChSys->QueueLowerSend, LSA_NULL, LSA_FALSE);
						if (GSY_RSP_OK != RbuRespVal)
							break;
					}
				}
			}
		}
#endif
		if (RbuRespVal == GSY_RSP_OK)
		{
			/* Fill Linkstatus-Indication-Request for this port
			 * 170707lrg001: ClockSyncOk from SyncTopoOk for SyncId 0 from ext.LinkInd
			*/
            GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
			GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
			GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
			GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
			GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_LINK_STATUS_EXT);
			GSY_LOWER_RQB_LINK_STATUS_PORT_SET(pRbl, PortId);
			GSY_LOWER_RQB_LINK_STATUS_TOPO_OK_SET(pRbl, pChSys->Port[PortId-1].ClockSyncOk);
			GSY_LOWER_RQB_LINK_STATUS_ALL_SET_UNKNOWN(pRbl);

			/* Send Request
			 * 110210lrg002: stor RQB untisl Forwarding is initialised
			GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
			*/
			pChSys->Port[PortId-1].pExtLinkRbl = pRbl;
		}
	}

	if (RbuRespVal != GSY_RSP_OK)
	{
		if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		}
		/* Store error until all PortParamsGet-Requests are back
		*/
		pChSys->Resp = RbuRespVal;
	}
	if (pChSys->PortInit == pChSys->PortCount)
	{
		/* If all ports are initialised: finish OPEN_CHANNEL
		*/
		gsy_OpenLowerEnd(pChSys, RbuRespVal);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerPort() pChSys=%X pRbl=%X RetVal=0x%x", 
					pChSys, pRbl, RbuRespVal);

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerEnd()                                     */
/* If gsy_open_channel() has run til gsy_OpenLowerPort(), it finishes here.  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerEnd(
GSY_CH_SYS_PTR	pChSys,
LSA_UINT16		RbuRespVal)
{
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerEnd() pChSys=%X RbuRespVal=0x%x", 
					pChSys, RbuRespVal);

	if (RbuRespVal == GSY_RSP_OK)
	{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32			//955647,956018
		/* Start timer for rate calculation
		*/
		gsy_TimerStart(&pChSys->DriftTimer, GSY_DRIFT_TIME_VALUE);
#endif
		/* Finish upper OpenSysChannel with response OK when
		 * all PortParamsGet-Requests are back from lower layer
		*/
		gsy_CallbackSys(pChSys, LSA_NULL, RbuRespVal);
	}
	else
	{
		/* On error: start close channel by CANCEL of the Recv-RQBs
		*/
		pChSys->Resp = RbuRespVal;
		pChSys->State = GSY_CHA_STATE_ERROR;
		RbuRespVal = gsy_CancelLower(pChSys, LSA_TRUE, 0, 0, LSA_NULL, GSY_LOWER_CANCEL);
		if (RbuRespVal != GSY_RSP_OK_ACTIVE)
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CANCEL, LSA_NULL, 0);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerEnd() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerError()      		                     */
/* Undo lower channel open.                                                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerError(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT16 				RbuRespVal)
{
	GSY_UPPER_RQB_PTR_TYPE  pRbu;
	LSA_UINT8				DelayMcAddr[] = GSY_MULTICAST_ADDR_DELAY;
	LSA_UINT16 				RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerError() pChSys=%X pRbl=%X RbuRespVal=0x%x", 
					pChSys, pRbl, RbuRespVal);

	pRbu = LSA_NULL;

	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* Use stored Open-RQB
		*/
		pRbl = pChSys->pOpenRbl;
		pChSys->pOpenRbl = LSA_NULL;
	}

	if (pChSys->State == GSY_CHA_STATE_OPEN_LOW)
	{
		/* Disable Delay-Multicastaddresse and close lower Channel
		*/
		pChSys->Resp = RbuRespVal;
		pChSys->State = GSY_CHA_STATE_ERROR;
		if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
		{
			/* Do not disable delay multicast address on system channels
			 * with delay measurement in hardware (KRISC32)
			*/
			gsy_CloseLower(pChSys, pRbl, LSA_FALSE);
		}
		else
		{
			(LSA_VOID)gsy_MucAddrSet(pChSys,	pRbl, DelayMcAddr, LSA_FALSE, LSA_FALSE);
		}
	}
	else
	{
		/* On error: free lower RQB and take the first upper RQB from queue
		*/
		gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		pRbu = pChSys->QueueUpper.pFirst;
		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* No upper RQB in queue ?!?
			*/
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
		}

		/* Clean up channel
		*/ 
		RetVal = gsy_ChSysClean(pChSys);
		if (LSA_RET_OK == RetVal)
		{
			gsy_DequeUpper(&pChSys->QueueUpper, pRbu);

			/* Send upper OPEN_CHANNEL RQB back to user
			*/ 
			gsy_CallbackSys(pChSys, pRbu, RbuRespVal);
			gsy_ChSysFree(pChSys);						//220710lrg001: after callback
		}
		else
		{
			pChSys->State = GSY_CHA_STATE_CLOSED;
			pChSys->Resp = RbuRespVal;
		}
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerError() pChSys=%X pRbu=%X", 
					pChSys, pRbu);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerFwd()                                     */
/* Allocate Receive-RQBs for announce frames and initialize forwarding       */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerFwd(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT16 	RbuRespVal;
	GSY_UPPER_RQB_PTR_TYPE  pRbu;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerFwd() pChSys=%X pChAnno=%X pRbl=%X", 
					pChSys, pChSys->pChAnno, pRbl);

	RbuRespVal = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
	if (RbuRespVal != GSY_LOWER_RSP_OK)
	{
		/* Cannot open lower layer channel
		*/
		RbuRespVal = GSY_RSP_ERR_LOWER_LAYER;

		gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);
		pRbu = pChSys->pChAnno->QueueUpper.pFirst;
		if (LSA_HOST_PTR_ARE_EQUAL(pRbu, LSA_NULL))
		{
			/* No upper RQB for callback ?!?
			*/
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_NO_RBU, LSA_NULL, 0);
		}

		/* Cleanup channel structure
		*/ 
		gsy_ChAnnoClean(pChSys->pChAnno);
		gsy_DequeUpper(&pChSys->pChAnno->QueueUpper, pRbu);

		/* Return upper OPEN_CHANNEL-RQB to user
		*/ 
		gsy_CallbackAnno(pChSys, pRbu, RbuRespVal);
		gsy_ChAnnoFree(pChSys->pChAnno);			//2207lrg001: after callback
	}
	else
	{
		/* Save lower handle in channel data
		*/
		pChSys->pChAnno->HandleLower = GSY_LOWER_RQB_HANDLE_LOWER_GET(pRbl);

		/* Store lower OpenChannel RQB for close channel
		*/
		pChSys->pChAnno->pOpenRbl = pRbl;
		RbuRespVal = GSY_RSP_OK;

		if (pChSys->RxTxNanos != 0)
		{
			/* Initialize channel forwarding 
			*/
			pChSys->pChAnno->State = GSY_CHA_STATE_OPEN_FWD;
			RbuRespVal = gsy_FwdChannelInit(pChSys, pChSys->PortCount, pChSys->PortFwd, gsy_OpenLowerAnnoEnd);
		}

		if (RbuRespVal != GSY_RSP_OK_ACTIVE)
		{
			/* On error or synchronous fwd init: finish OPEN_CHANNEL
			*/
			gsy_OpenLowerAnnoEnd(pChSys, RbuRespVal);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerFwd() pChSys=%X pRbl=%X RetVal=0x%x", 
					pChSys, pRbl, RbuRespVal);

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_OpenLowerAnnoEnd()                                 */
/* If gsy_open_channel() has run til gsy_OpenLowerFwd(), it finishes here.   */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_OpenLowerAnnoEnd(
GSY_CH_SYS_PTR	pChSys,
LSA_UINT16		RbuRespVal)
{
	LSA_INT Idx;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_OpenLowerAnnoEnd() pChSys=%X pChAnno=%X RbuRespVal=0x%x", 
					pChSys, pChSys->pChAnno, RbuRespVal);

	if (RbuRespVal == GSY_RSP_OK)
	{
		/* Finsh upper open channel without error
		*/
		gsy_CallbackAnno(pChSys, LSA_NULL, RbuRespVal);

		/* Send ExtLinkIndication RQBs for all ports on system channel
		*/
		for (Idx = 0; Idx < pChSys->PortCount; Idx++)
		{
			if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[Idx].pExtLinkRbl, LSA_NULL))
			{
				GSY_REQUEST_LOWER(pChSys->Port[Idx].pExtLinkRbl, pChSys->pSys);
				pChSys->Port[Idx].pExtLinkRbl = LSA_NULL;
			}
		}
	}
	else
	{
		/* On error: start close channel
		*/
		pChSys->pChAnno->Resp = RbuRespVal;
		pChSys->pChAnno->State = GSY_CHA_STATE_ERROR;
		RbuRespVal = gsy_CancelAnnoAll(pChSys->pChAnno);
		if (RbuRespVal != GSY_RSP_OK_ACTIVE)
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CANCEL, LSA_NULL, 0);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_OpenLowerAnnoEnd() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_CancelAnno()                                */
/* Canncel announce receive RQBs for one SyncId                              */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelAnno(
GSY_CH_USR_PTR					pChUsr)
{
	LSA_USER_ID_TYPE		UserId;
	GSY_CH_ANNO_PTR			pChAnno = pChUsr->pChSys->pChAnno;
	GSY_LOWER_RQB_PTR_TYPE	pRbl = pChUsr->pCancelRbl;
	LSA_UINT16 				RespVal = GSY_RSP_OK_ACTIVE;
	LSA_UINT32				CancelId = GSY_ANNO_TYPE_ID;

	CancelId <<= 8;
	CancelId |= pChUsr->SyncId;

	GSY_FUNCTION_TRACE_05(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_CancelAnno() pChUsr=%X pChSys=%X pChAnno=%X pRbl=%X CancelId=0x%08x", 
			pChUsr, pChUsr->pChSys, pChUsr->pChSys->pChAnno, pRbl, CancelId);

	UserId.void_ptr = pChUsr;

	/* Use master start MC enable RB
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		RespVal = GSY_RSP_ERR_RESOURCE;
		GSY_ERROR_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_CancelAnno(%02u) no lower RQB for pChUsr=%X",
				pChUsr->SyncId, pChUsr);
	}
	
	if (GSY_RSP_OK_ACTIVE == RespVal)
	{
        GSY_IS_VALID_PTR(pRbl);

		/* Fill CANCEL recv by ID request
		*/
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChAnno->HandleLower);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_CANCEL);
		GSY_LOWER_RQB_CANCEL_MODE_SET(pRbl, GSY_LOWER_CANCEL_MODE_ID);
		GSY_LOWER_RQB_USER_PTR_SET(pRbl, UserId.void_ptr);
		GSY_LOWER_RQB_CANCEL_ID_SET(pRbl, CancelId);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

		/* Send CANCEL request 
		*/
		GSY_REQUEST_LOWER(pRbl, pChAnno->pSys);
	}

	GSY_FUNCTION_TRACE_02(pChUsr->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_CancelAnno() pChAnno=%X RetVal=0x%x",
						pChAnno, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_CancelAnnoAll()                             */
/* Prepare lower announce channel for being closed                           */
/* by sending of a CANCEL request.                                           */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelAnnoAll(
GSY_CH_ANNO_PTR 			pChAnno)
{
	LSA_USER_ID_TYPE		UserId;
	GSY_LOWER_RQB_PTR_TYPE	pRbl = pChAnno->pOpenRbl;
	LSA_UINT16 				RespVal = GSY_RSP_OK_ACTIVE;

	GSY_FUNCTION_TRACE_03(pChAnno->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CancelAnnoAll() pChSys=%X pChAnno=%X pRbl=%X", 
					pChAnno->pChSys, pChAnno, pRbl);

	UserId.uvar32 = 0;

	/* Use Open Channel RB, if stored
	*/
	if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		pChAnno->pOpenRbl = LSA_NULL;
	else
		RespVal = GSY_RSP_ERR_RESOURCE;
	
	if (GSY_RSP_OK_ACTIVE == RespVal)
	{
        GSY_IS_VALID_PTR(pRbl);

		/* Fill CANCEL request
		*/
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChAnno->HandleLower);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_CANCEL);
		GSY_LOWER_RQB_CANCEL_MODE_SET(pRbl, GSY_LOWER_CANCEL_MODE_ALL);
		GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_CANCEL_ID_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

		/* Send CANCEL request 
		*/
		GSY_REQUEST_LOWER(pRbl, pChAnno->pSys);
	}

	GSY_FUNCTION_TRACE_02(pChAnno->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CancelAnnoAll() pChAnno=%X RetVal=0x%x",
						pChAnno, RespVal);
	return(RespVal);
}
/*****************************************************************************/
/* Internal access function: gsy_CancelLower()                               */
/* If CancelAll is TRUE, the lower Channel is prepared for being closed by   */
/* sending of CANCEL requests. Else end of a delay request at timeout.       */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelLower(
GSY_CH_SYS_PTR 			pChSys,
LSA_BOOL				CancelAll,
LSA_UINT8				TypeId,
LSA_HANDLE_TYPE			UserHandle,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_UINT32				Service)
{
	LSA_USER_ID_TYPE	UserId;
	LSA_UINT16 			RespVal = GSY_RSP_OK_ACTIVE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CancelLower() pChSys=%X pRbl=%X CancelAll16/TypeId8/UserHandle8=0x%08x", 
					pChSys, pRbl, ((((LSA_UINT32)CancelAll) << 16)+(((LSA_UINT32)TypeId) << 8)+(((LSA_UINT32)UserHandle) << 8)));

	UserId.uvar32 = 0;
	UserId.uvar8_array[GSY_HANDLE_UID] = UserHandle;
	UserId.uvar8_array[GSY_TYPE_UID] = TypeId;

	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* Use Open Channel RB, if stored
		*/
		pRbl = pChSys->pOpenRbl;
		if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
			pChSys->pOpenRbl = LSA_NULL;
		else
			RespVal = GSY_RSP_ERR_RESOURCE;
	}
	
	if (GSY_RSP_OK_ACTIVE == RespVal)
	{
        GSY_IS_VALID_PTR(pRbl);

		/* Fill CANCEL request
		*/
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, Service);
		GSY_LOWER_RQB_CANCEL_ID_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

		if (CancelAll)
			GSY_LOWER_RQB_CANCEL_MODE_SET(pRbl, GSY_LOWER_CANCEL_MODE_ALL);
		else
			GSY_LOWER_RQB_CANCEL_MODE_SET(pRbl, GSY_LOWER_CANCEL_MODE_ID);

		/* Send CANCEL request 
		*/
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
	}
	else
		RespVal = GSY_RSP_ERR_RESOURCE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CancelLower() pChSys=%X Service=0x%x RetVal=0x%x", 
					pChSys, Service, RespVal);
	return(RespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_CloseLower()                                */
/* Close Lower Channel.                                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_CloseLower(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl,
LSA_BOOL				ChAnno)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseLower() pChSys=%X pRbl=%X ChAnno=%u", 
					pChSys, pRbl, ChAnno);

	/* Fill CLOSE request
	*/
    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_CLOSE_CHANNEL);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, 0);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

	/* Send CLOSE request
	*/
	if (ChAnno)
	{
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->pChAnno->HandleLower);
		GSY_CLOSE_CHANNEL_LOWER(pRbl, pChSys->pChAnno->pSys);
	}
	else
	{
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_CLOSE_CHANNEL_LOWER(pRbl, pChSys->pSys);
	}
	
	/* On callback: gsy_request_lower_done() and gsy_CloseLowerFin()
	*/
	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseLower() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal function: gsy_CloseLowerFin()                                    */
/* Lower Channel is closed: clean up and confirm user request.               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_CloseLowerFin(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	LSA_UINT16 RespVal = GSY_RSP_OK;
	LSA_BOOL   ChRemove = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseLowerFin() pChSys=%X pRbl=%X State=%X", 
					pChSys, pRbl, pChSys->State);

	if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		if (GSY_RSP_OK != pChSys->Resp)
		{
			/* Error on gsy_open_channel()
			*/
			RespVal = pChSys->Resp;
		}
		else
		{
			RespVal = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
			if (GSY_LOWER_RSP_OK != RespVal)
			{
				RespVal = GSY_RSP_ERR_LOWER_LAYER;
				ChRemove = LSA_FALSE;
			}
		}
	}

	if (ChRemove)
	{
		/* Free lower RQB and clean up channel structure
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
			gsy_FreeLowerRb(pChSys, pRbl, LSA_FALSE);
		RetVal = gsy_ChSysClean(pChSys);
		if (LSA_RET_OK != RetVal)
		{
			pChSys->State = GSY_CHA_STATE_CLOSED;
			ChRemove = LSA_FALSE;
			pChSys->Resp = RespVal;
		}
	}
	else
	{
		/* Store lower RQB for close_channel() and reset channel state
		*/ 
		GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"  * gsy_CloseLowerFin() pChSys=%X: pRbl=%X stored", 
					pChSys, pRbl);
		pChSys->pOpenRbl = pRbl;
//		pChSys->State = GSY_CHA_STATE_OPEN;
	}

	/* If all resources are free now: confirm actual upper RQB to the user
	*/
	if (LSA_RET_OK == RetVal)
		gsy_CallbackSys(pChSys, LSA_NULL, RespVal);

	if (ChRemove)
	{
		/* Free channel structure
		*/
		gsy_ChSysFree(pChSys);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseLowerFin() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal function: gsy_CloseAnnoFin()                                     */
/* Lower Announce Channel is closed: clean up and confirm user request.      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_CloseAnnoFin(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	pRbl)
{
	LSA_UINT16 RespVal = GSY_RSP_OK;
	LSA_BOOL   ChRemove = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_CloseAnnoFin() pChSys=%X pChAnno=%X pRbl=%X", 
					pChSys, pChSys->pChAnno, pRbl);

	if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		if (GSY_RSP_OK != pChSys->pChAnno->Resp)
		{
			/* Error at gsy_open_channel()
			*/
			RespVal = pChSys->pChAnno->Resp;
		}
		else
		{
			RespVal = GSY_LOWER_RQB_RESPONSE_GET(pRbl);
			if (GSY_LOWER_RSP_OK != RespVal)
			{
				RespVal = GSY_RSP_ERR_LOWER_LAYER;
				ChRemove = LSA_FALSE;
			}
		}
	}

	if (ChRemove)
	{
		/* Free lower RQB and channel ressources
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			gsy_FreeLowerRb(pChSys, pRbl, LSA_TRUE);
		}
		gsy_ChAnnoClean(pChSys->pChAnno);
	}
	else
	{
		/* Store lower RQB for close_channel()
		*/ 
		GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"  * gsy_CloseAnnoFin() pChSys=%X pChAnno=%X: pRbl=%X stored", 
					pChSys, pChSys->pChAnno, pRbl);
		pChSys->pChAnno->pOpenRbl = pRbl;
	}

	/* Confirm actual upper RQB to user
	*/
	gsy_CallbackAnno(pChSys, LSA_NULL, RespVal);

	if (ChRemove)
	{
		/* Free channel structure
		*/
		gsy_ChAnnoFree(pChSys->pChAnno);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_CloseAnnoFin() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_MucAddrSet()                                */
/* Enabled or disable a GSY Multicast address                                */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MucAddrSet(
GSY_CH_SYS_PTR   		pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT8       const * pMucAddr,
LSA_BOOL				Enable,
LSA_BOOL				ChAnno)
{
	LSA_UINT16 RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_MucAddrSet() pChSys=%X pRbl=%X Enable=%u", 
					pChSys, pRbl, Enable);

	/* Allocate lower request block if the parameter pointer is NULL
	*/
	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		RetVal = gsy_AllocEthParam(pChSys, &pRbl, ChAnno);

	if (LSA_RET_OK == RetVal)
	{
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_MULTICAST);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		if (Enable)
		{
			GSY_LOWER_RQB_MULTICAST_ENABLE(pRbl, pMucAddr);
		}
		else
		{
			GSY_LOWER_RQB_MULTICAST_DISABLE(pRbl, pMucAddr);
		}

		/* Send Lower Request
		*/
		if (ChAnno)
		{
			GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->pChAnno->HandleLower);
			GSY_REQUEST_LOWER(pRbl, pChSys->pChAnno->pSys);
		}
		else
		{
			GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
			GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
		}
	}

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_MucAddrSet() pRbl=%X Addr[3-5]=%02x-%02x-%02x", 
					pRbl, pMucAddr[3], pMucAddr[4], pMucAddr[5]);
	return(RetVal);
}

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
/*****************************************************************************/
/* Internal access function: gsy_LowerDelaySet()                             */
/* Send the LineDelay for one port to the lower layer.                       */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_LowerDelaySet(
GSY_CH_SYS_PTR	pChSys,
LSA_UINT16		PortId)
{
	GSY_LOWER_RQB_PTR_TYPE  pRbl;
	LSA_UINT32				LowerDelayTicks;
	LSA_UINT32				LowerDelayRest;
	LSA_UINT16				Response = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_LowerDelaySet() pChSys=%X Port=%u Delay=%u", 
					pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay);

	/* Calculate delay ticks for lower Layer with the with
	 * the resolution from GET_PARAMS and round up
	*/
	LowerDelayTicks = pChSys->Port[PortId-1].SyncLineDelay / pChSys->RxTxNanos;
	LowerDelayRest = pChSys->Port[PortId-1].SyncLineDelay % pChSys->RxTxNanos;
	if ((LowerDelayRest*2) >= pChSys->RxTxNanos)
		LowerDelayTicks++;

	/* Use RQB of Lower Ctrl Queue
	*/
	pRbl = pChSys->QueueLowerCtrl.pFirst;
	if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		Response = GSY_RSP_ERR_RESOURCE;

	if ((GSY_RSP_OK == Response)
	&&  (LowerDelayTicks != pChSys->Port[PortId-1].LowerDelayTicks))
	{
        GSY_IS_VALID_PTR(pRbl);

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- gsy_LowerDelaySet() [%u%u%u%u] Port=%u LineDelay=%u CableDelay=%u LowerDelayTicks=%u", 
						0, 0, 0, 0, PortId, pChSys->Port[PortId-1].SyncLineDelay, pChSys->Port[PortId-1].CableDelay, LowerDelayTicks);

		/* If there is one RQB in queue and the lower Delay has changed
		*/
		pChSys->Port[PortId-1].LowerDelayTicks = LowerDelayTicks;
		gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
		GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
		GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_DELAY_SET);
		GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
		GSY_LOWER_RQB_SET_LINE_DELAY_PORT_SET(pRbl, PortId);
		GSY_LOWER_RQB_SET_LINE_DELAY_DELAY_SET(pRbl, LowerDelayTicks);
		GSY_LOWER_RQB_SET_LINE_DELAY_CABLE_SET(pRbl, pChSys->Port[PortId-1].CableDelay);

		/* Send Lower Request
		*/
		GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
	}
	else
	{
		/* Trace error and indicate Delay to User (normally done on confirmation)
		*/
		if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN, "*** gsy_LowerDelaySet() no RQB");
		}
		gsy_UserDelayInd(pChSys, PortId, Response);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_LowerDelaySet() pChSys=%X LowerDelayTicks=%d", 
					pChSys, LowerDelayTicks);
}
#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32

/*****************************************************************************/
/* Internal access function: gsy_DataSend()                                  */
/* Send a Delay frame.                                                       */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DataSend(
GSY_CH_SYS_PTR   		pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT16				PortId,
LSA_INT					SendLen,
LSA_BOOL				TimeStamp)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DataSend() pChSys=%X pRbl=%X SendLen=%d", 
					pChSys, pRbl, SendLen);

	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
	if (TimeStamp)
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SEND_TS);
	else
		GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SEND);
	GSY_LOWER_RQB_SEND_LEN_SET(pRbl, SendLen);
	GSY_LOWER_RQB_SEND_PORT_SET(pRbl, PortId);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

//	GSY_TRACE_SR(pChSys, pRbl);
	GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<== gsy send:",
							(LSA_UINT8*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl), SendLen);

	GSY_REQUEST_LOWER(pRbl, pChSys->pSys);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DataSend() pChSys=%X Port=%u Send_TS=%u", 
					pChSys, PortId, TimeStamp);
}

/*****************************************************************************/
/* Internal access function: gsy_DataRecv()                                  */
/* Request the receiving of a Delay-Frame.                                   */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DataRecv(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT32				CancelId)
{
	LSA_USER_ID_TYPE UserId;

	UserId.uvar32 = CancelId;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DataRecv() pChSys=%X pRbl=%X CancelId=0x%x", 
					pChSys, pRbl, CancelId);

	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RECV);
	GSY_LOWER_RQB_RECV_LEN_SET(pRbl, GSY_LOWER_MEM_MAX_LEN);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
	GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
	GSY_LOWER_RQB_RECV_ID_SET(pRbl, CancelId);

	GSY_REQUEST_LOWER(pRbl, pChSys->pSys);

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DataRecv() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_AnnoSend()                                  */
/* Send Announce frame.                                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_AnnoSend(
GSY_CH_SYS_PTR   		pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT16				PortId,
LSA_INT					SendLen)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AnnoSend() pChSys=%X pRbl=%X SendLen=%d", 
					pChSys, pRbl, SendLen);

	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SEND);
	GSY_LOWER_RQB_SEND_LEN_SET(pRbl, SendLen);
	GSY_LOWER_RQB_SEND_PORT_SET(pRbl, PortId);
	GSY_LOWER_RQB_SEND_PRIO_SET(pRbl, GSY_SEND_PRIO_ANNO);		//150410lrg001
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

//	GSY_TRACE_SR(pChSys, pRbl);
	GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<== gsy send anno:",
							(LSA_UINT8*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl), SendLen);

    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->pChAnno->HandleLower);
	GSY_REQUEST_LOWER(pRbl, pChSys->pChAnno->pSys);

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AnnoSend() pChSys=%X Port=%u", 
					pChSys, PortId);
}

/*****************************************************************************/
/* Internal access function: gsy_AnnoRecv()                                  */
/* Request the receiving of a Announce-Frame.                                */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_AnnoRecv(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT32				CancelId)
{
	LSA_USER_ID_TYPE UserId;

	UserId.uvar32 = CancelId;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AnnoRecv() pChSys=%X pRbl=%X CancelId=0x%x", 
					pChSys, pRbl, CancelId);

	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RECV);
	GSY_LOWER_RQB_RECV_LEN_SET(pRbl, GSY_LOWER_MEM_MAX_LEN);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
	GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
	//GSY_LOWER_RQB_RECV_ID_SET(pRbl, CancelId);

    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->pChAnno->HandleLower);
	GSY_REQUEST_LOWER(pRbl, pChSys->pChAnno->pSys);

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AnnoRecv() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal access function: gsy_SyncSend()                                  */
/* Send a Sync-/Fu-Frame.                                                    */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SyncSend(
GSY_CH_SYS_PTR   		pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_INT					SendLen,
LSA_UINT16				PortId,
LSA_UINT16				FrameId)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SyncSend() pChSys=%X pRbl=%X SendLen=%d", 
					pChSys, pRbl, SendLen);

    if (0 == PortId)
    {
        GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"  < gsy_SyncSend() pChSys=%X pRbl=%X PortId=%u", 
                        pChSys, pRbl, PortId);
        gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_SEND, pRbl, FrameId);
    }

	/* 240608lrg001: Set LineDelay delayed
	*/
	pChSys->Port[PortId-1].SendCnfPending++;
	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SEND_SYNC);
	GSY_LOWER_RQB_SEND_SYNC_LEN_SET(pRbl, SendLen);
	GSY_LOWER_RQB_SEND_SYNC_PORT_SET(pRbl, PortId);
	GSY_LOWER_RQB_SEND_SYNC_FRAMEID_SET(pRbl, FrameId);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);

//	GSY_TRACE_SR(pChSys, pRbl);
	GSY_SNDRCV_TRACE_BYTE_ARRAY(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "<== gsy send sync:",
							(LSA_UINT8*)GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl), SendLen);

	GSY_REQUEST_LOWER(pRbl, pChSys->pSys);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SyncSend() pChSys=%X Port=%u FrameId=0x%04x", 
					pChSys, PortId, FrameId);
}

/*****************************************************************************/
/* Internal access function: gsy_SyncRecv()                                  */
/* Request the receiving of a Sync-/Fu-Frame.                                */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_SyncRecv(
GSY_CH_SYS_PTR 			pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_UINT32				CancelId)
{
	LSA_USER_ID_TYPE UserId;

	UserId.uvar32 = CancelId;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_SyncRecv() pChSys=%X pRbl=%X CancelId=0x%x", 
					pChSys, pRbl, CancelId);

	GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
    GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
	GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
	GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RECV_SYNC);
	GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
	GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
	GSY_LOWER_RQB_RECV_SYNC_LEN_SET(pRbl, GSY_LOWER_MEM_MAX_LEN);

	GSY_REQUEST_LOWER(pRbl, pChSys->pSys);

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_SyncRecv() pChSys=%X", 
					pChSys);
}

/*****************************************************************************/
/* Internal function: gsy_AllocEthParam()      		                         */
/* Allocate lower Requstblock with Parameterblock                            */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthParam(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE	*ppRbl,
LSA_BOOL				ChAnno)
{
	GSY_LOWER_RQB_PTR_TYPE		pRbl;
	GSY_LOWER_MEM_PTR_TYPE		pMem;
	LSA_USER_ID_TYPE        	UserId;
	LSA_SYS_PTR_TYPE			pSys;
	LSA_UINT16 					RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AllocEthParam() pChSys=%X ChAnno=%u", 
					pChSys, ChAnno);

	pMem = LSA_NULL;

	UserId.uvar32 = 0;
	if (ChAnno)
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->pChAnno->Handle;
		pSys = pChSys->pChAnno->pSys;
	}
	else
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->Handle;
		pSys = pChSys->pSys;
	}

	/* Request lower RQB
	*/
	GSY_ALLOC_LOWER_RQB(&pRbl, UserId, sizeof(GSY_LOWER_RQB_TYPE), pSys);

	/* If the lower RB has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* ...allocate memory for parameter block
		*/
		GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_PPARAM_SET(pRbl, LSA_NULL);
		GSY_ALLOC_LOWER_MEM(&pMem, UserId, sizeof(GSY_LOWER_PARAM_TYPE), pSys);

		/* If the lower PB has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
		{
			/* ...set param pointer of the RB to the address of PB
			 * and return pointer to RB
			*/
			GSY_LOWER_RQB_PPARAM_SET(pRbl, pMem);
			*ppRbl = pRbl;
		}
		else
		{
			/* Next step: get RQB asynchronously by gsy_lower_mem()
			 * is currently not supported!
			*/
			GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
			RetVal = LSA_RET_ERR_RESOURCE;
		}
	}
	else
	{
		/* Next step: get RQB asynchronously by gsy_lower_rqb()
		 * is currently not supported!
		*/
		RetVal = LSA_RET_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AllocEthParam() pRbl=%X pMem=%X RetVal=0x%x", 
					pRbl, pMem, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal function: gsy_AllocEthSend()      		                         */
/* Allocate Lower Send-Requstblock with Parameterblock and Data block        */
/* and put it into the given queue.                                          */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthSend(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_QUEUE		*	pQueue,
LSA_VOID *				pUser,
LSA_BOOL				ChAnno)
{
	LSA_USER_ID_TYPE        	UserId;
	LSA_SYS_PTR_TYPE			pSys;
	GSY_LOWER_RQB_PTR_TYPE		pRbl = LSA_NULL;
	GSY_LOWER_MEM_PTR_TYPE		pMem = LSA_NULL;
	GSY_LOWER_TXMEM_PTR_TYPE	pMemTx = LSA_NULL;
	LSA_UINT16 					RetVal = LSA_RET_OK;
	LSA_UINT16 					RbuRespVal = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AllocEthSend() pChSys=%X pQueue=%X ChAnno=%u", 
					pChSys, pQueue, ChAnno);

	UserId.uvar32 = 0;
	if (ChAnno)
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->pChAnno->Handle;
		pSys = pChSys->pChAnno->pSys;
	}
	else
	{
		UserId.uvar8_array[GSY_HANDLE_UID] = pChSys->Handle;
		pSys = pChSys->pSys;
	}

	/* Request lower RQB
	*/
	GSY_ALLOC_LOWER_RQB(&pRbl, UserId, sizeof(GSY_LOWER_RQB_TYPE), pSys);

	/* If the lower RB has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		/* ...put it into the channels queue and request lower parameter block
		*/
		GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
		GSY_LOWER_RQB_PPARAM_SET(pRbl, LSA_NULL);
		gsy_EnqueLower(pQueue, pRbl);
		GSY_ALLOC_LOWER_MEM(&pMem, UserId, sizeof(GSY_LOWER_PARAM_TYPE), pSys);

		/* If the lower PB has been delivered synchronously...
		*/
		if(!LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
		{
			/* ...set param pointer of the RB to the address of PB
			 * and request memory for send data	block
			*/
			GSY_LOWER_RQB_PPARAM_SET(pRbl, pMem);
			GSY_LOWER_RQB_SEND_DATA_SET(pRbl, LSA_NULL, 0);
			GSY_ALLOC_LOWER_TXMEM(&pMemTx, UserId, GSY_LOWER_MEM_MAX_LEN, pSys);

			/* If the lower DB has been delivered synchronously...
			*/
			if(!LSA_HOST_PTR_ARE_EQUAL(pMemTx, LSA_NULL))
			{
				/* ...set data pointer of the RB to the address of DB
				 * and set service code to SEND
				*/
				GSY_LOWER_RQB_SEND_DATA_SET(pRbl, pMemTx, GSY_LOWER_MEM_MAX_LEN);
				GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_SEND);
				GSY_LOWER_RQB_USER_PTR_SET(pRbl, pUser);
			}
			else
			{
				/* Next step: get RQB asynchronously by gsy_lower_tx_mem()
				 * is currently not supported!
				*/
				gsy_DequeLower(pQueue, pRbl);
				pMem = GSY_LOWER_RQB_PPARAM_GET(pRbl);
				GSY_FREE_LOWER_MEM(&RetVal, pMem, pSys);
				GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
				RbuRespVal = GSY_RSP_ERR_RESOURCE;
			}
		}
		else
		{
			/* Next step: get RQB asynchronously by gsy_lower_mem()
			 * is currently not supported!
			*/
			gsy_DequeLower(pQueue, pRbl);
			GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
			RbuRespVal = GSY_RSP_ERR_RESOURCE;
		}
	}
	else
	{
		/* Next step: get RQB asynchronously by gsy_lower_rqb()
		 * is currently not supported!
		*/
		RbuRespVal = GSY_RSP_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AllocEthSend() pRbl=%X pMemTx=%X RetVal=0x%x", 
					pRbl, pMemTx, RbuRespVal);

    LSA_UNUSED_ARG(RetVal);     // Return value is not evaluated

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_AllocEthRecv()      		                         */
/* Allocate Lower Recv-Requstblock with Parameterblock and Data block        */
/* and put it into the given queue or send it to the lower layer.            */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthRecv(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE 	pRbr,
GSY_LOWER_QUEUE		*	pQueue,
LSA_HANDLE_TYPE			UserHandle,
LSA_UINT8				TypeId,
LSA_BOOL				ChAnno)
{
	LSA_USER_ID_TYPE        	UserId;
	LSA_SYS_PTR_TYPE			pSys;
	GSY_LOWER_RXMEM_PTR_TYPE	pMemRx = LSA_NULL;
	GSY_LOWER_MEM_PTR_TYPE		pMem = LSA_NULL;
	GSY_LOWER_RQB_PTR_TYPE		pRbl = LSA_NULL;
	LSA_UINT16 					RetVal = LSA_RET_OK;
	LSA_UINT16 					RbuRespVal = GSY_RSP_OK;

    GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AllocEthRecv() pChSys=%X pRbr=%X TypeId=0x%x", 
					pChSys, pRbr, TypeId);

	UserId.uvar32 = 0;
	UserId.uvar8_array[GSY_HANDLE_UID] = UserHandle;
	UserId.uvar8_array[GSY_TYPE_UID] = TypeId;
	if (ChAnno)
	{
		pSys = pChSys->pChAnno->pSys;
	}
	else
	{
		pSys = pChSys->pSys;
	}

	if(LSA_HOST_PTR_ARE_EQUAL(pRbr, LSA_NULL))
	{
		/* Allocate Lower Requestblock
		*/
		GSY_ALLOC_LOWER_RQB(&pRbl, UserId, sizeof(GSY_LOWER_RQB_TYPE), pSys);
	}
	else
	{
		pRbl = pRbr;
		pMem = GSY_LOWER_RQB_PPARAM_GET(pRbl);
	}

	/* If the lower RB has been delivered synchronously...
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
	{
		if(LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
		{
			/* Allocate Parameterblock and set pointer of the RQB
			*/
			GSY_LOWER_RQB_USERID_UVAR32_SET(pRbl, UserId.uvar32);
			GSY_ALLOC_LOWER_MEM(&pMem, UserId, sizeof(GSY_LOWER_PARAM_TYPE), pSys);
			GSY_LOWER_RQB_PPARAM_SET(pRbl, pMem);
		}

		if(!LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
		{
			/* Allocate Receive data block
			 * 101208lrg: receive Announce with GSY_LOWER_RECV
			*/
			if (GSY_SYNC_TYPE_ID == TypeId)
			{
				GSY_LOWER_RQB_RECV_SYNC_DATA_SET(pRbl, LSA_NULL, 0);
			}
			else
			{
				GSY_LOWER_RQB_RECV_DATA_SET(pRbl, LSA_NULL, 0);
			}
			GSY_ALLOC_LOWER_RXMEM(&pMemRx, UserId, GSY_LOWER_MEM_MAX_LEN, pSys);

			/* Wenn der lower DB synchron geliefert wurde...
			*/
			if(!LSA_HOST_PTR_ARE_EQUAL(pMemRx, LSA_NULL))
			{
				/* Fill RQB and set buffer pointers
				*/
				if (ChAnno)
				{
					GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->pChAnno->HandleLower);
				}
				else
				{
					GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
				}
                GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
				GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
				GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
				if (GSY_SYNC_TYPE_ID == TypeId)
				{
					GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RECV_SYNC);
					GSY_LOWER_RQB_RECV_SYNC_PTR_SET(pRbl, pMemRx);
					GSY_LOWER_RQB_RECV_SYNC_LEN_SET(pRbl, GSY_LOWER_MEM_MAX_LEN);
					GSY_LOWER_RQB_RECV_SYNC_ID_SET(pRbl, UserId.uvar32);
				}
				else
				{
					GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RECV);
					GSY_LOWER_RQB_RECV_PTR_SET(pRbl, pMemRx);
					GSY_LOWER_RQB_RECV_LEN_SET(pRbl, GSY_LOWER_MEM_MAX_LEN);
					GSY_LOWER_RQB_RECV_ID_SET(pRbl, UserId.uvar32);
				}

				if (!LSA_HOST_PTR_ARE_EQUAL(pQueue, LSA_NULL))
				{
					/* Put it into given queue
					*/
					gsy_EnqueLower(pQueue, pRbl);
				}
				else
				{
					/* Send Lower Receive Request
					*/
					GSY_REQUEST_LOWER(pRbl, pSys);
				}
			}
			else
			{
#ifdef GSY_MESSAGE
				/* Next step: get RQB asynchronously by gsy_lower_rx_mem()
				 * todo: Do not free the already allocated memory
				 * is currently not supported!
				*/
#endif /* GSY_MESSAGE */
				if (LSA_HOST_PTR_ARE_EQUAL(pRbr, LSA_NULL))
				{
					pMem = GSY_LOWER_RQB_PPARAM_GET(pRbl);
					GSY_FREE_LOWER_MEM(&RetVal, pMem, pSys);
					GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
				}
				RbuRespVal = GSY_RSP_ERR_RESOURCE;
			}
		}
		else
		{
#ifdef GSY_MESSAGE
			/* Next step: get RQB asynchronously by gsy_lower_mem()
			 * todo: Do not free the already allocated memory
			 * is currently not supported!
			*/
#endif /* GSY_MESSAGE */
			if(LSA_HOST_PTR_ARE_EQUAL(pRbr, LSA_NULL))
				GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
			RbuRespVal = GSY_RSP_ERR_RESOURCE;
		}
	}
	else
	{
		/* Next step: get RQB asynchronously by gsy_lower_rqb()
		 * is currently not supported!
		*/
		RbuRespVal = GSY_RSP_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AllocEthRecv() pRbl=%X pMemRx=%X RetVal=0x%x", 
					pRbl, pMemRx, RbuRespVal);

    LSA_UNUSED_ARG(RetVal);     // Return value is not evaluated

	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal function: gsy_AllocEthKrisc32()                                  */
/* Allocate lower Krisc32-Requstblock with Parameterblock                    */
/* and put it into QueueLowerCtrl.                                           */
/*****************************************************************************/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthKrisc32(
GSY_CH_SYS_PTR  		pChSys)
{
	GSY_LOWER_RQB_PTR_TYPE		pRbl = LSA_NULL;
	LSA_UINT16 					RetVal;
	LSA_UINT16 					RbuRespVal = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_AllocEthKrisc32() pChSys=%X QueueLowerCtrl.Count=%u", 
					pChSys, pChSys->QueueLowerCtrl.Count);

	/* 220409lrg001: Allocate Requestblock with Parameterblock
	*/
	RetVal = gsy_AllocEthParam(pChSys, &pRbl, LSA_FALSE);
	if (LSA_RET_OK == RetVal)
	{
		/* ...put it into into  ctrl queue
		*/
		gsy_EnqueLower(&pChSys->QueueLowerCtrl, pRbl);
	}
	else
	{
		/* Currently only synchronous allocation is supported!
		*/
		RbuRespVal = GSY_RSP_ERR_RESOURCE;
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_AllocEthKrisc32() pRbl=%X RetVal=0x%x", 
					pRbl, RbuRespVal);
	return(RbuRespVal);
}

/*****************************************************************************/
/* Internal access function: gsy_FreeLowerRb()                               */
/* Free Requestblock, Parameterblock and Data buffer.                        */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_FreeLowerRb(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RQB_PTR_TYPE  pRbl,
LSA_BOOL				ChAnno)
{
	LSA_SYS_PTR_TYPE			pSys;
	GSY_LOWER_MEM_PTR_TYPE		pMem;
	GSY_LOWER_TXMEM_PTR_TYPE	pMemTx = LSA_NULL;
	GSY_LOWER_RXMEM_PTR_TYPE	pMemRx = LSA_NULL;
	LSA_UINT16 					RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_FreeLowerRb() pChSys=%X pRbl=%X ChAnno=%u", 
					pChSys, pRbl, ChAnno);
	if (ChAnno)
	{
		pSys = pChSys->pChAnno->pSys;
	}
	else
	{
		pSys = pChSys->pSys;
	}

	pMem = GSY_LOWER_RQB_PPARAM_GET(pRbl);
	if (!LSA_HOST_PTR_ARE_EQUAL(pMem, LSA_NULL))
	{
		if ((GSY_LOWER_RECV == GSY_LOWER_RQB_SERVICE_GET(pRbl))
		||  (GSY_LOWER_RECV_SYNC == GSY_LOWER_RQB_SERVICE_GET(pRbl)))
		{
			/* Free Receive Memory
			*/
			if (GSY_LOWER_RECV == GSY_LOWER_RQB_SERVICE_GET(pRbl))
			{
				pMemRx = GSY_LOWER_RQB_RECV_PTR_GET(pRbl);
			}
			else
			{
				pMemRx = GSY_LOWER_RQB_RECV_SYNC_PTR_GET(pRbl);
			}
			if (!LSA_HOST_PTR_ARE_EQUAL(pMemRx, LSA_NULL))
			{
				GSY_FREE_LOWER_RXMEM(&RetVal, pMemRx, pSys);
				if (LSA_RET_OK != RetVal)
				    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_LOWER_MEM, pMemRx, 0);
			}
		}
		else if ((GSY_LOWER_SEND == GSY_LOWER_RQB_SERVICE_GET(pRbl))
			 ||  (GSY_LOWER_SEND_TS == GSY_LOWER_RQB_SERVICE_GET(pRbl))
			 ||  (GSY_LOWER_SEND_SYNC == GSY_LOWER_RQB_SERVICE_GET(pRbl)))
		{
			/* Free Transmit Memory
			*/
			if (GSY_LOWER_SEND_SYNC == GSY_LOWER_RQB_SERVICE_GET(pRbl))
			{
				pMemTx = GSY_LOWER_RQB_SEND_SYNC_PTR_GET(pRbl);
			}
			else
			{
				pMemTx = GSY_LOWER_RQB_SEND_PTR_GET(pRbl);
			}
			if (!LSA_HOST_PTR_ARE_EQUAL(pMemTx, LSA_NULL))
			{
				GSY_FREE_LOWER_TXMEM(&RetVal, pMemTx, pSys);
				if (LSA_RET_OK != RetVal)
				    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_LOWER_MEM, pMemTx, 0);
			}
		}
		/* Free Parameter Memory
		*/
		GSY_FREE_LOWER_MEM(&RetVal, pMem, pSys);
		if (LSA_RET_OK != RetVal)
		    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_LOWER_MEM, pMem, 0);
	}

	GSY_FREE_LOWER_RQB(&RetVal, pRbl, pSys);
	if (LSA_RET_OK != RetVal)
	    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_FREE_LOWER_RQB, pRbl, 0);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_FreeLowerRb() pChSys=%X pMemTx=%X pMemRx=%X", 
					pChSys, pMemTx, pMemRx);
}

/*****************************************************************************/
/* External LSA access function: gsy_lower_rqb()                             */
/* Get Lower Requestblock asynchronously.                                    */
/*****************************************************************************/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_rqb(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
GSY_LOWER_RQB_PTR_TYPE   pRbl)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	GSY_CH_SYS_PTR pChSys = gsy_ChSysGet(UserId.uvar8_array[GSY_HANDLE_UID]);

	/* Currently only synchronous allocation is supported!
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
		GSY_FREE_LOWER_RQB(&RetVal, pRbl, pChSys->pSys);

    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_ASYNC_PTR, pChSys, Len);

    LSA_UNUSED_ARG(RetVal);    // Return value is not evaluated
}

/*****************************************************************************/
/* External LSA access function: gsy_lower_mem()                             */
/* Get lower datablock asynchronously.                                       */
/*****************************************************************************/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
GSY_LOWER_MEM_PTR_TYPE   DataPtr)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	GSY_CH_SYS_PTR pChSys = gsy_ChSysGet(UserId.uvar8_array[GSY_HANDLE_UID]);

	/* Currently only synchronous allocation is supported!
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
		GSY_FREE_LOWER_MEM(&RetVal, DataPtr, pChSys->pSys);

    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_ASYNC_PTR, pChSys, Len);

    LSA_UNUSED_ARG(RetVal);    // Return value is not evaluated
}

/*****************************************************************************/
/* External LSA access function: gsy_lower_tx_mem()                          */
/* Get lower send datablock asynchronously.                                  */
/*****************************************************************************/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_tx_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
GSY_LOWER_TXMEM_PTR_TYPE DataPtr)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	GSY_CH_SYS_PTR pChSys = gsy_ChSysGet(UserId.uvar8_array[GSY_HANDLE_UID]);

	/* Currently only synchronous allocation is supported!
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
		GSY_FREE_LOWER_TXMEM(&RetVal, DataPtr, pChSys->pSys);

    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_ASYNC_PTR, pChSys, Len);
}

/*****************************************************************************/
/* External LSA access function: gsy_lower_rx_mem()                          */
/* Get lower receive datablock asynchronously.                               */
/*****************************************************************************/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_lower_rx_mem(
LSA_USER_ID_TYPE         UserId,
LSA_UINT16               Len,
GSY_LOWER_RXMEM_PTR_TYPE DataPtr)
{
	LSA_UINT16 RetVal = LSA_RET_OK;
	GSY_CH_SYS_PTR pChSys = gsy_ChSysGet(UserId.uvar8_array[GSY_HANDLE_UID]);

	/* Currently only synchronous allocation is supported!
	*/
	if(!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
		GSY_FREE_LOWER_RXMEM(&RetVal, DataPtr, pChSys->pSys);

    gsy_ErrorLower(GSY_MODULE_ID, __LINE__, GSY_ERR_ASYNC_PTR, pChSys, Len);
}

/*-----------------------------------------------------------------------------
* EOF gsy_low.c
*/

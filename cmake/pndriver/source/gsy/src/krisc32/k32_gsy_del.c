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
/*  F i l e               &F: k32_gsy_del.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P06.01.00.00_00.02.00.02         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2017-03-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Functions for Delay measurement in KRISC32 firmware                      */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2008-10-30  P04.01.00.00_00.02.02.02 lrg  file creation from gsy_del.c   */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-01-07  P04.01.00.00_00.02.03.02 lrg: gsy_ -> k32gsy_                */
/*  2009-03-18  P04.01.00.00_00.03.03.02 180309lrg001: pK32SyncRqb           */
/*  2009-06-25  P04.02.00.00_00.01.03.03 250609lrg001: Weiterleitung von     */
/*              ClockSyncFrames zum Port bei dort gemessenem LineDelay       */
/*              freigeben und bei Wechsel des LineDelay auf 0 sperren        */
/*  2009-07-08  P04.02.00.00_00.01.04.01 lrg: Master implemented             */
/*  2009-07-20  P04.02.00.00_00.01.04.02 lrg: Receive frame buffers: 32 bit  */
/*              aligned                                                      */
/*  2009-09-03  P04.02.00.00_00.01.04.04 030909lrg001 Diagnose bei Wechsel   */
/*              der Drift von/nach 0 und bei geaenderter RcvSyncPrio         */
/*  2009-09-24  P04.02.00.00_00.01.04.05 lrg: K32_MASTER_IN_FW               */
/*  2009-10-12  P04.02.00.00_00.01.04.06 lrg: EDDP_ALIGNED_ATTRIBUTE_        */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: K32_SYNC_UPDATE                */
/*  2009-11-25  P05.00.00.00_00.01.01.03 251109lrg002: enable sending and    */
/*              forwarding of TimeFrames not until Delay is measured         */
/*  2009-12-01  P05.00.00.00_00.01.01.04 lrg: K32_RECV_FRAME_BUFFER and      */
/*              K32_DELAY_FRAME_BUFFER moved to gsy_pck.h                    */
/*  2010-03-11  P05.00.00.00_00.01.03.02 021209lrg001 k32gsy_DriftSet():     */
/*              (De)activate SeqID check in hardware by setting the master   */
/*              MAC address if no slave or master is running.                */
/*  2010-04-23  P05.00.00.00_00.01.04.02 230410lrg001 Local ApiTimeStamp     */
/*  2010-07-19  P05.00.00.00_00.01.06.02 140710lrg001: AP00991839 AP00999658 */
/*              Check RspDelayNS for GSY_PTCP_DELAY_NS_INVALID               */
/*  2010-08-25  P05.00.00.00_00.01.07.02 250810lrg001: AP00697597 AP01021800 */
/*              German comments translated to english                        */
/*  2010-10-18  P05.00.00.00_00.01.08.03 150910lrg001: check Delay10NS       */
/*              220910lrg001: report ReqSeqId                                */
/*              181010lrg002: DelayStopReq conf. waits for DelayFrame conf.  */
/*  2011-02-16  P05.00.00.00_00.04.07.02 lrg: AP01123956 Index-Trace         */
/*  2011-07-25  P05.00.00.00_00.05.38.02 lrg: AP01215492                     */
/*                                            K32_OPC_GSY_DELAY_CNF          */
/*  2011-09-29  S05.01.00.00_00.04.10.01 AP01236058 Lint Warnings entfernen  */
/*  2011-10-24  S05.01.00.00_00.05.05.02 AP01262791 Negative CableDelay      */
/*  2012-06-11  P05.00.00.00_00.05.97.02 AP01366052 LSA trace %012u -> %010u */
/*  2012-10-25  P05.02.00.00_00.04.08.01 AP01425250:ERTEC200P:               */
/*  KRISC32 stürzt ab bei Ethernet Verbindung zu Scalance X204IRT FW V3      */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01384863: DelayFilter             */
/*              AP01482167:LineDelayMessung: --> K32GSY_RCF_SET_DELAY        */
/*  2013-04-17  P05.02.00.00_00.04.22.02 AP01529351/AP01527161: RspDelayNanos*/
/*  2013-11-25  P05.03.00.00_00.02.20.01 903419/907378: Mittelwertfreie      */
/*              Berechnung der LineDelay                                     */
/*  2015-04-15  V06.00, Inc04, Feature FT1204141, Task 1234432               */
/*              eddp_GetSyncTime(), Ratetimeout flex. Ki anpassen            */
/*  2015-11-25  V06.00, Inc12, Task 1499164: K32_FU_FWD_IN_FW                */
/*              SyncFU-/FU-Frames statt in HW in der KRISC32-FW weiterleiten */
/*  2016-04-13  V05.02-V07.00, RQ 1628273 Cable Delay value check            */
/*              for values below -50ns removed                               */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	10		/* K32_MODULE_ID_GSY_DEL */
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
LSA_UINT16					k32_RecvFrameSize;
LSA_UINT16					k32_DelayFrameSize;

K32_RECV_FRAME_BUFFER K32_MEM_ATTR	k32_RecvFrame[K32_NUM_RFB_CLOCK_RECV+K32_NUM_RFB_TIME_RECV];
K32_DELAY_FRAME_BUFFER K32_MEM_ATTR	k32_DelayFrame[K32_NUM_RFB_DELAY_RECV];

#ifdef K32_MASTER_IN_FW
/* Number of master send frame buffers: 1 ClockMaster per Interface + 1 TimeMaster
*/
K32_PTCP_SYNC_FRAME			K32_MEM_ATTR  k32_MasterFrame[K32_INTERFACE_COUNT + 1];
#ifdef K32_MASTER_SEND_FU_TEST
K32_PTCP_FU_FRAME			K32_MEM_ATTR  k32_MasterFuFrame;
#endif
#endif // K32_MASTER_IN_FW

#ifdef K32_FU_FWD_IN_FW
K32_PTCP_SYNC_FRAME			K32_MEM_ATTR  k32_FwdSyncFrame[2][K32_PORT_MAX];
K32_PTCP_FU_FRAME			K32_MEM_ATTR  k32_FwdFuFrame[2];
#endif // K32_FU_FWD_IN_FW

K32_PTCP_DELAY_REQ_FRAME	K32_MEM_ATTR  k32_DelayReqFrame[K32_PORT_MAX];
K32_PTCP_DELAY_REQ_FRAME	K32_MEM_ATTR *k32_pDelayReqFrame[K32_PORT_MAX];
GSY_CH_K32 					K32_MEM_ATTR  k32_InterfaceChannel[K32_INTERFACE_COUNT];

/*****************************************************************************/
/* External access function: k32gsy_Init()                                   */
/* Initialise data structures, allocate RRQBs and timers                     */
/*****************************************************************************/
LSA_UINT16 K32_SYS_IN_FCT_ATTR k32gsy_Init()
{
	LSA_UINT8			InterfaceNo;
	LSA_INT				i, ii;
	LSA_INT				RecvFrameIdx = 0;
	LSA_INT				DelayFrameIdx = 0;
	LSA_UINT16			RetVal = GSY_RET_OK;
	GSY_CH_K32_PTR		pChSys = LSA_NULL;

#ifdef K32_MASTER_IN_FW
	LSA_TIMER_ID_TYPE	TimeTimerId = 0;
	GSY_MAC_ADR_TYPE	ClockSyncMac = {GSY_MULTICAST_ADDR_SYNC};
	GSY_MAC_ADR_TYPE	TimeSyncMac = {GSY_MULTICAST_ADDR_TIME};
#endif // K32_MASTER_IN_FW

	GSY_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_Init() K32_INTERFACE_FIRST=%u K32_INTERFACE_LAST=%u",
		K32_INTERFACE_FIRST, K32_INTERFACE_LAST);

	/* Set size of receive frame buffer
	*/
	k32_RecvFrameSize = sizeof(K32_RECV_FRAME_BUFFER);
	k32_DelayFrameSize = sizeof(K32_DELAY_FRAME_BUFFER);

#ifdef K32_MASTER_IN_FW
	/* Allocate sync timer for time master
	*/
	TimeTimerId = K32GSY_TIMER_ALLOC_SYNC();
#endif // K32_MASTER_IN_FW

	/* Initialize delay frame buffer pointers
	*/
	for (i = 0; i < K32_PORT_MAX ; i++)
	{
		k32_pDelayReqFrame[i] = &k32_DelayReqFrame[i];
	}

	for (InterfaceNo = K32_INTERFACE_FIRST; InterfaceNo <= K32_INTERFACE_LAST; InterfaceNo++)
	{
		pChSys = k32gsy_SysPtrGet(InterfaceNo);
		GSY_MEMSET_LOCAL(pChSys, 0, sizeof(GSY_CH_K32));

		pChSys->State = GSY_CHA_STATE_OPEN;
		pChSys->InterfaceId = InterfaceNo;
		pChSys->TraceIdx = K32GSY_TRACE_IDX_GET(InterfaceNo);
		pChSys->RxTxNanos = K32GSY_RXTXNANOS;
		pChSys->PortCount = K32GSY_PORT_COUNT_GET(pChSys->InterfaceId);
		pChSys->DriftTimer.Running = LSA_FALSE;
		pChSys->DriftTimer.pChSys = pChSys;
		pChSys->DriftTimer.pTimeoutProc = k32gsy_DriftTimer;
		pChSys->DriftTimer.Id = K32GSY_TIMER_ALLOC_DELAY();
		if (0 == pChSys->DriftTimer.Id)
		{
			RetVal = GSY_RET_ERR_RESOURCE;
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate drift timer");
			break;
		}

		for (i = 0; i < pChSys->PortCount ; i++)
		{
			pChSys->Port[i].State = GSY_PORT_STATE_INIT;
			pChSys->Port[i].SyncLineDelay = 0;
			pChSys->Port[i].CableDelay = 0;
			pChSys->Port[i].MinDelay = 0;
			pChSys->Port[i].MaxDelay = 0;
			pChSys->Port[i].SumCount = 0;
			pChSys->Port[i].TxDelay = 0;
			pChSys->Port[i].RxDelay = 0;
			pChSys->Port[i].DelayReqActive = LSA_FALSE;
			pChSys->Port[i].DelayRspActive = LSA_FALSE;				// 291008lrg001
			pChSys->Port[i].SumDelay = 0;
			pChSys->Port[i].SumCountLast = 0;
			pChSys->Port[i].DelayEvent = GSY_SYNC_EVENT_DELAY_OK;
			pChSys->Port[i].ActDelay.State = GSY_DELAY_STATE_INI;
			pChSys->Port[i].ActDelay.pDrift = LSA_NULL;
			pChSys->Port[i].LastError = K32_TXRESULT_OK;
			pChSys->Port[i].ErrorCount = 0;
			pChSys->Port[i].DelayReqSeqId = 0;
			pChSys->Port[i].ClockSyncOk = K32_INITIAL_TOPO_STATE;
			pChSys->Port[i].pReqRQB = LSA_NULL;					//181010lrg002
			pChSys->Port[i].pK32Rqb = K32GSY_RRQB_ALLOC();
			if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[i].pK32Rqb, LSA_NULL))
			{
				RetVal = GSY_RET_ERR_RESOURCE;
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate K32Rqb for port=%u",
								i+1);
				break;
			}
			else
			{
				pChSys->Port[i].pK32Rqb->Hdr.Opcode = K32_OPC_GSY_DELAY_IND;
				pChSys->Port[i].pK32Rqb->Hdr.Response = K32_RSP_OK;
			}
			pChSys->Port[i].pCnfRQB = K32GSY_RRQB_ALLOC();
			if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[i].pCnfRQB, LSA_NULL))
			{
				RetVal = GSY_RET_ERR_RESOURCE;
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate pCnfRQB for port=%u",
								i+1);
				break;
			}
			else
			{
				pChSys->Port[i].pCnfRQB->Hdr.Opcode = K32_OPC_GSY_DELAY_CNF;
				pChSys->Port[i].pCnfRQB->Hdr.Response = K32_RSP_OK;
			}
			pChSys->Port[i].DelayTimer.Running = LSA_FALSE;
			pChSys->Port[i].DelayTimer.Port = i + 1;
			pChSys->Port[i].DelayTimer.pChSys = pChSys;
			pChSys->Port[i].DelayTimer.pTimeoutProc = k32gsy_DelayTimer;
			pChSys->Port[i].DelayTimer.Id = K32GSY_TIMER_ALLOC_DELAY();
			if (0 == pChSys->Port[i].DelayTimer.Id)
			{
				RetVal = GSY_RET_ERR_RESOURCE;
				GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate delay timer");
				break;
			}
			/* Search for unused delay frame buffer
			*/
			for (ii = 0; ii < K32_PORT_MAX ; ii++)
			{
				if (!LSA_HOST_PTR_ARE_EQUAL(k32_pDelayReqFrame[ii], LSA_NULL))
				{
					pChSys->Port[i].pDelayReqPdu = (GSY_LOWER_TX_PDU_PTR)k32_pDelayReqFrame[ii];
					k32_pDelayReqFrame[ii] = LSA_NULL;
					break;
				}
			}
			if (ii >= K32_PORT_MAX)
			{
				RetVal = GSY_RET_ERR_RESOURCE;
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Too few delay frame buffers: %u",
								K32_PORT_MAX);
				break;
			}

			/* Put receive frame buffers to PN-IP
			*/
			for (ii = 0; ii < K32_NUM_RFB_DELAY_PORT; ii++)
			{
				K32GSY_RECV_FRAME((K32GSY_FRAME_PTR)&k32_DelayFrame[DelayFrameIdx++],
					k32_DelayFrameSize, K32_USETYPE_DELAY, pChSys->InterfaceId);
			}
			for (ii = 0; ii < K32_NUM_RFB_CLOCK_PORT; ii++)
			{
				K32GSY_RECV_FRAME((K32GSY_FRAME_PTR)&k32_RecvFrame[RecvFrameIdx++],
					k32_RecvFrameSize, K32_USETYPE_CLOCK, pChSys->InterfaceId);
				K32GSY_RECV_FRAME((K32GSY_FRAME_PTR)&k32_RecvFrame[RecvFrameIdx++],
					k32_RecvFrameSize, K32_USETYPE_CLOCKFU, pChSys->InterfaceId);
			}
			for (ii = 0; ii < K32_NUM_RFB_TIME_PORT; ii++)
			{
				K32GSY_RECV_FRAME((K32GSY_FRAME_PTR)&k32_RecvFrame[RecvFrameIdx++],
					k32_RecvFrameSize, K32_USETYPE_TIME, pChSys->InterfaceId);
				K32GSY_RECV_FRAME((K32GSY_FRAME_PTR)&k32_RecvFrame[RecvFrameIdx++],
					k32_RecvFrameSize, K32_USETYPE_TIMEFU, pChSys->InterfaceId);
			}
			/* Enable receiving SyncFrames and receiving/sending TimeFrames
			 * 251109lrg002: sending of TimeFrames when Delay is measured
			 */
			K32GSY_FWD_RX_SET_CLOCK(pChSys->InterfaceId, i+1, LSA_TRUE);
			K32GSY_FWD_RX_SET_TIME(pChSys->InterfaceId, i+1, LSA_TRUE);
//			K32GSY_FWD_TX_SET_TIME(pChSys->InterfaceId, i+1, LSA_TRUE);
		}
		for (i = 0; i < 2 ; i++)
		{
			pChSys->Drift[i].SyncId = GSY_SYNCID_NONE;
			pChSys->Drift[i].State = GSY_DRIFT_STATE_UNKNOWN;
			pChSys->Drift[i].DiagCount = 0;
			pChSys->Drift[i].IntervalSetValid = LSA_FALSE;	/* 310707js001 */
			pChSys->Drift[i].RcvSyncPrio = 0;				/* 170408lrg002 */
			pChSys->Drift[i].LocalPrio = 0;					/* 170408lrg002 */
			pChSys->Drift[i].TimeoutMillis = 0;				// FT1204141
			for (ii = 0; ii < GSY_DRIFT_MAX_COUNT ; ii++)
			{
				pChSys->Drift[i].Old[ii].Used = LSA_TRUE;	//180108lrg001
			}
			pChSys->Drift[i].Diag.pK32Rqb = K32GSY_RRQB_ALLOC();
			if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Drift[i].Diag.pK32Rqb, LSA_NULL))
			{
				RetVal = GSY_RET_ERR_RESOURCE;
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate K32Rqb for SyncId=%u",i);
				break;
			}
			else
			{
				pChSys->Drift[i].Diag.pK32Rqb->Hdr.Opcode = K32_OPC_GSY_DIAG;
				pChSys->Drift[i].Diag.pK32Rqb->Hdr.Response = K32_RSP_OK;
			}
#ifdef K32_FU_FWD_IN_FW
			for (ii = 0; ii < K32_PORT_MAX ; ii++)
			{
				pChSys->Fwd[i].pSyncFrame[ii] = &k32_FwdSyncFrame[i][ii];
			}
			pChSys->Fwd[i].pFuFrame = &k32_FwdFuFrame[i];
			pChSys->Fwd[i].FuWait = LSA_FALSE;
			pChSys->Fwd[i].CnfWait = LSA_FALSE;
#endif // K32_FU_FWD_IN_FW
		}
		pChSys->pK32SyncRqb = K32GSY_RRQB_ALLOC();
		if (LSA_HOST_PTR_ARE_EQUAL(pChSys->pK32SyncRqb, LSA_NULL))
		{
			RetVal = GSY_RET_ERR_RESOURCE;
			GSY_ERROR_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate pK32SyncRqb");
			break;
		}
		else
		{
			pChSys->pK32SyncRqb->Hdr.Opcode = K32_OPC_GSY_SYNC_IND;
			pChSys->pK32SyncRqb->Hdr.Response = K32_RSP_OK;
			pChSys->pK32SyncRqb->Params.SyncInd.SyncId = GSY_SYNCID_CLOCK;
			pChSys->pK32SyncRqb->Params.SyncInd.SyncOk = LSA_FALSE;
		}
		GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - k32gsy_Init() Interface=%u TraceIdx=%u PortCount=%u",
			pChSys->InterfaceId, pChSys->TraceIdx, pChSys->PortCount);
	}

#ifdef K32_MASTER_IN_FW

	/* Initialize master frame buffer and data
	*/
	GSY_MEMSET_LOCAL(k32_MasterFrame, 0, sizeof(k32_MasterFrame));
	for (i = 0; i < K32_INTERFACE_COUNT; i++)
	{
		k32_MasterFrame[i].PduHdr.DstMacAdr = ClockSyncMac;
		k32_MasterFrame[i].PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_SYNC);
		k32_MasterFrame[i].PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
		for (ii = 0; ii < 2 ; ii++)
		{
			k32_InterfaceChannel[i].Master[ii].State = GSY_MASTER_STATE_OFF;
			k32_InterfaceChannel[i].Master[ii].SeqId = 0;
			k32_InterfaceChannel[i].Master[ii].SendTimer.Running = LSA_FALSE;
			k32_InterfaceChannel[i].Master[ii].SendTimer.pChSys = &k32_InterfaceChannel[i];
			k32_InterfaceChannel[i].Master[ii].SendTimer.pTimeoutProc = k32gsy_MasterSend;
			k32_InterfaceChannel[i].Master[ii].pReqRQB = LSA_NULL;			//181010lrg001
		}
		k32_InterfaceChannel[i].Master[0].pFrame = &k32_MasterFrame[i];
		k32_InterfaceChannel[i].Master[1].pFrame = &k32_MasterFrame[K32_INTERFACE_COUNT];

		k32_InterfaceChannel[i].Master[0].SendTimer.Id = K32GSY_TIMER_ALLOC_SYNC();
		if (0 == k32_InterfaceChannel[i].Master[0].SendTimer.Id)
		{
			RetVal = GSY_RET_ERR_RESOURCE;
			GSY_ERROR_TRACE_00(k32_InterfaceChannel[i].TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate clock master timer");
			break;
		}
		/* Only 1 TimeMaster timer allocated above exists for all interfaces
		*/
		k32_InterfaceChannel[i].Master[1].SendTimer.Id = TimeTimerId;
		if (0 == k32_InterfaceChannel[i].Master[1].SendTimer.Id)
		{
			RetVal = GSY_RET_ERR_RESOURCE;
			GSY_ERROR_TRACE_00(k32_InterfaceChannel[i].TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_Init() Cannot allocate time master timer");
			break;
		}
 	}

	/* Same time sync frame for all interfaces
	*/
	k32_MasterFrame[K32_INTERFACE_COUNT].PduHdr.DstMacAdr = TimeSyncMac;
	k32_MasterFrame[K32_INTERFACE_COUNT].PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_TIME);
	k32_MasterFrame[K32_INTERFACE_COUNT].PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);

#else // K32_MASTER_IN_FW

	for (i = 0; i < K32_INTERFACE_COUNT; i++)
	{
		for (ii = 0; ii < 2 ; ii++)
		{
			k32_InterfaceChannel[i].Master[ii].State = GSY_MASTER_STATE_OFF;
			k32_InterfaceChannel[i].Master[ii].SeqId = 0;
			k32_InterfaceChannel[i].Master[ii].pFrame = LSA_NULL;
			k32_InterfaceChannel[i].Master[ii].SendTimer.Running = LSA_FALSE;
			k32_InterfaceChannel[i].Master[ii].SendTimer.pChSys = LSA_NULL;
			k32_InterfaceChannel[i].Master[ii].SendTimer.pTimeoutProc = LSA_NULL;
			k32_InterfaceChannel[i].Master[ii].SendTimer.Id = 0;
		}
 	}

#endif // K32_MASTER_IN_FW

 	GSY_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_Init() pChSys=%x InterfaceNo=%u",
					pChSys, InterfaceNo);

	return(RetVal);
}

/*****************************************************************************/
/* External access function: k32gsy_UndoInit()                               */
/* Free timers and RRQBs                                                     */
/*****************************************************************************/
LSA_UINT16 K32_SYS_IN_FCT_ATTR k32gsy_UndoInit()
{
	LSA_UINT8		InterfaceNo;
	LSA_INT			i;
	LSA_UINT16		RetVal = GSY_RET_OK;
	GSY_CH_K32_PTR	pChSys = LSA_NULL;
	LSA_TIMER_ID_TYPE	TimeTimerId = 0;

	GSY_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_UndoInit() K32_INTERFACE_FIRST=%u K32_INTERFACE_LAST=%u",
					K32_INTERFACE_FIRST, K32_INTERFACE_LAST);

	for (InterfaceNo = K32_INTERFACE_FIRST; InterfaceNo <= K32_INTERFACE_LAST; InterfaceNo++)
	{
		pChSys = k32gsy_SysPtrGet(InterfaceNo);

		GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - k32gsy_UndoInit() InterfaceNo=%u TraceIdx=%u",
						InterfaceNo, pChSys->TraceIdx);

		if (pChSys->DriftTimer.Id != 0)
		{
			if (pChSys->DriftTimer.Running)
				K32GSY_TIMER_STOP(pChSys->DriftTimer.Id);
			RetVal = K32GSY_TIMER_FREE(pChSys->DriftTimer.Id);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error=0x%x from K32GSY_TIMER_FREE(TimerId=%u)",
								RetVal, pChSys->DriftTimer.Id);
			}
		}
		if (pChSys->Master[0].SendTimer.Id != 0)
		{
			if (pChSys->Master[0].SendTimer.Running)
				K32GSY_TIMER_STOP(pChSys->Master[0].SendTimer.Id);
			RetVal = K32GSY_TIMER_FREE(pChSys->Master[0].SendTimer.Id);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error=0x%x from K32GSY_TIMER_FREE(TimerId=%u)",
								RetVal, pChSys->Master[0].SendTimer.Id);
			}
		}
		if ((0 == TimeTimerId) && (pChSys->Master[1].SendTimer.Id != 0))
		{
			/* Only 1 TimeMaster timer exists for all interfaces
			*/
			TimeTimerId = pChSys->Master[1].SendTimer.Id;
			if (pChSys->Master[1].SendTimer.Running)
				K32GSY_TIMER_STOP(pChSys->Master[1].SendTimer.Id);
			RetVal = K32GSY_TIMER_FREE(pChSys->Master[1].SendTimer.Id);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error=0x%x from K32GSY_TIMER_FREE(TimerId=%u)",
								RetVal, pChSys->Master[1].SendTimer.Id);
			}
		}
		for (i = 0; i < pChSys->PortCount ; i++)
		{
			if (pChSys->Port[i].DelayTimer.Id != 0)
			{
				if (pChSys->Port[i].DelayTimer.Running)
					K32GSY_TIMER_STOP(pChSys->Port[i].DelayTimer.Id);
				RetVal = K32GSY_TIMER_FREE(pChSys->Port[i].DelayTimer.Id);
				if (LSA_RET_OK != RetVal)
				{
					GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error=0x%x from K32GSY_TIMER_FREE(TimerId=%u)",
									RetVal, pChSys->Port[i].DelayTimer.Id);
				}
			}
			if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[i].pK32Rqb, LSA_NULL))
			{
				if (!K32GSY_RRQB_FREE(pChSys->Port[i].pK32Rqb))
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error from K32GSY_RRQB_FREE(pK32Rqb=%x) Port=%u Interface=%u",
										pChSys->Port[i].pK32Rqb, i+1, pChSys->InterfaceId);
				}
			}
			if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[i].pCnfRQB, LSA_NULL))
			{
				if (!K32GSY_RRQB_FREE(pChSys->Port[i].pCnfRQB))
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error from K32GSY_RRQB_FREE(pCnfRQB=%x) Port=%u Interface=%u",
										pChSys->Port[i].pCnfRQB, i+1, pChSys->InterfaceId);
				}
			}
		}
		for (i = 0; i < 2 ; i++)
		{
			if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Drift[i].Diag.pK32Rqb, LSA_NULL))
			{
				if (!K32GSY_RRQB_FREE(pChSys->Drift[i].Diag.pK32Rqb))
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error from K32GSY_RRQB_FREE(pK32Rqb=%x) SyncId=%u Interface=%u",
										pChSys->Drift[i].Diag.pK32Rqb, i, pChSys->InterfaceId);
				}
			}
		}
		if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pK32SyncRqb, LSA_NULL))
		{
			if (!K32GSY_RRQB_FREE(pChSys->pK32SyncRqb))
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_UndoInit() Error from K32GSY_RRQB_FREE(pK32SyncRqb=%x) Interface=%u",
								pChSys->pK32SyncRqb, pChSys->InterfaceId);
			}
		}
	}

	GSY_FUNCTION_TRACE_02(0, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_UndoInit() pChSys=%x InterfaceNo=%u",
					pChSys, InterfaceNo);

	return(RetVal);
}

#ifdef K32_FU_FWD_IN_FW
/*****************************************************************************/
/* Internal access function: k32gsy_FwdSyncCnf()                             */
/* Confirmation for a sent SyncFuFrame                                       */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_FwdSyncCnf(
GSY_CH_K32_PTR			pChSys,
GSY_LOWER_TX_PDU_PTR	pPduTx,
LSA_UINT16				Response,
LSA_UINT32				Timestamp,
LSA_UINT16				PortId,
LSA_UINT8				SyncId)
{
	LSA_INT64			Delay64;
	LSA_UINT32			FwdDelay, Delay32;
	LSA_UINT16			SeqId = GSY_SWAP16(pPduTx->PtcpData.Sync.PtcpHdr.SequenceID.Word);
	K32GSY_FRAME_PTR	pFrame = (K32GSY_FRAME_PTR)pChSys->Fwd[SyncId].pFuFrame;

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_FwdSyncCnf() pChSys=%x pPduTx=%x Response=%u Timestamp=%u",
					pChSys, pPduTx, Response, Timestamp);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Fwd[SyncId].pSyncFrame[PortId-1], LSA_NULL))
	{
		GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_FwdSyncCnf() Sync frame=%x not sent at Interface=%u Port=%u SyncId=%u",
						pPduTx, pChSys->InterfaceId, PortId, SyncId);
	}
	else
	{
		/* Restore confirmed frame buffer pointer
		*/
		pChSys->Fwd[SyncId].pSyncFrame[PortId-1] = (GSY_SYNC_SEND_FRAME_PTR)pPduTx;
		pChSys->Fwd[SyncId].CnfWait = LSA_FALSE;

		if (K32_TXRESULT_OK != Response)
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_FwdSyncCnf() SyncId=%u SeqId=%u: Send response=0x%x",
							SyncId, SeqId, Response);
		}
		else if (!LSA_HOST_PTR_ARE_EQUAL(pFrame, LSA_NULL))
		{
			/* Calculate and add bridge delay nanoseconds to FollowUp frame header
			*/
			FwdDelay = (LSA_UINT32)(Timestamp - pChSys->Fwd[SyncId].RxStamp[PortId-1]);
			Delay64 = GSY_SWAP32(pChSys->Fwd[SyncId].pFuFrame->SyncFu.PtcpHdr.DelayNS.Dword);
			Delay64 += FwdDelay;
			if ((Delay64 <= GSY_MAX_NANOS_PER_INT32_64) && (Delay64 >= GSY_MIN_NANOS_PER_INT32_64))
			{
				Delay32 = (LSA_UINT32)Delay64;
				GSY_SWAPD(&Delay32, &(pChSys->Fwd[SyncId].pFuFrame->SyncFu.PtcpHdr.DelayNS.Dword));

				/* Send FollowUp frame without timestamp on forwarding port
				* Pointer will be restored on send confirmation
				*/
				GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_FwdSyncCnf(%02x) sending  FU frame=%x SeqId=%u on Port=%u FwdDelay=%u", 
								SyncId, pFrame, pChSys->Fwd[SyncId].SeqId, PortId, FwdDelay);
				pChSys->Fwd[SyncId].pFuFrame = LSA_NULL;
				K32GSY_SEND_FRAME(pFrame, sizeof(K32_PTCP_FU_FRAME), GSY_FWD_USETYPE_GET(SyncId), pChSys->InterfaceId, PortId, LSA_FALSE);
			}
			else
			{
				GSY_ERROR_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_FwdSyncCnf() pChSys=%x SyncId=%u FwdDelay=%u: Delay overflow for SeqId=%u to Port=%u",
					pChSys, SyncId, FwdDelay, pChSys->Fwd[SyncId].SeqId, PortId);
			}
		}
		else
		{
			GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_FwdSyncCnf() pChSys=%x SyncId=%u: Cannot forward SeqId=%u to Port=%u",
				pChSys, SyncId, pChSys->Fwd[SyncId].SeqId, PortId);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_FwdSyncCnf() SyncId=%u SeqId=%u Port=%u",
					SyncId, SeqId, PortId);

	LSA_UNUSED_ARG(SeqId);
}
#endif // K32_FU_FWD_IN_FW

/*****************************************************************************/
/* External access function: k32gsy_SendConf()                               */
/* Handle send confimation.                                                  */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_SendConf(
K32GSY_FRAME_PTR	pFrameBuffer,
LSA_UINT8			InterfaceId,
LSA_UINT16			PortId,
LSA_UINT8			Result,
LSA_BOOL			TimestampWait,
LSA_UINT32			Timestamp)
{
	GSY_CH_K32_PTR  		pChSys = k32gsy_SysPtrGet(InterfaceId);
	GSY_LOWER_TX_PDU_PTR	pFrameTx = (GSY_LOWER_TX_PDU_PTR)pFrameBuffer;
	LSA_UINT16				FrameId = GSY_SWAP16(pFrameTx->PduHdr.FrameId.Word);
	LSA_UINT8				SyncId = FrameId & GSY_SYNCID_MASK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SendConf() pFrameBuffer=%x Port=%u Result=%u",
					pFrameBuffer, PortId, Result);

	GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - k32gsy_SendConf() FrameId=0x%04x pFrameBuffer=%x IfId=%u Port=%u Result=%u TsWait=%u Ts=%u",
			FrameId, pFrameBuffer, InterfaceId, PortId, Result, TimestampWait, Timestamp);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		switch (GSY_SWAP16(pFrameTx->PduHdr.FrameId.Word))
		{
#ifdef K32_FU_FWD_IN_FW
		case GSY_FRAMEID_FUSYNC:
		case GSY_FRAMEID_FUTIME:
			k32gsy_FwdSyncCnf(pChSys, pFrameTx, Result, Timestamp, PortId, SyncId);
			break;

		case GSY_FRAMEID_SYNC_FU:
		case GSY_FRAMEID_TIME_FU:
			/* Restore confirmed FollowUp frame buffer pointer
			*/
			pChSys->Fwd[SyncId].pFuFrame = (GSY_FU_SEND_FRAME_PTR)pFrameTx;
			GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SendConf(%02x) confirmed FU frame=%x SeqId=%u Port=%u",
							SyncId, pFrameTx, pChSys->Fwd[SyncId].SeqId, PortId);
			break;
#endif // K32_FU_FWD_IN_FW
#ifdef K32_MASTER_IN_FW

#ifdef K32_MASTER_SEND_FU_TEST
		case GSY_FRAMEID_FUSYNC:
		case GSY_FRAMEID_FUTIME:
		case GSY_FRAMEID_SYNC_FU:
		case GSY_FRAMEID_TIME_FU:
#endif
		case GSY_FRAMEID_SYNC:
		case GSY_FRAMEID_TIME:
			k32gsy_MasterSyncCnf(pChSys, pFrameTx, Result, SyncId);
			break;

#endif // K32_MASTER_IN_FW

		case GSY_FRAMEID_DELAY_REQ:
			if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[PortId-1].pDelayReqPdu, LSA_NULL))
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SendConf() Delay req frame=%x not sent at Interface=%u Port=%u",
								pFrameBuffer, InterfaceId, PortId);
			}
			else
			{
				/* Call confirmation fct and put back frame buffer pointer
				*/
				k32gsy_DelayReqCnf(pChSys, Result, PortId, pFrameTx, Timestamp);
				pChSys->Port[PortId-1].pDelayReqPdu = pFrameTx;
				if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[PortId-1].pReqRQB, LSA_NULL))
				{
					/* 181010lrg002: Confirm DelayReq now
					*/
					GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_SendConf() Port=%u confirm now: pDelayReq=%x",
									PortId, pChSys->Port[PortId-1].pReqRQB);
					pChSys->Port[PortId-1].pReqRQB->Hdr.Opcode = K32_OPC_GSY_DELAY_CNF;
					pChSys->Port[PortId-1].pReqRQB->Hdr.Response = K32_RSP_OK;
					K32GSY_CALLBACK(pChSys->Port[PortId-1].pReqRQB);
					pChSys->Port[PortId-1].pReqRQB = LSA_NULL;
				}
			}
			break;

		case GSY_FRAMEID_DELAY_RSP:
			if (Result != K32_TXRESULT_OK)
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SendConf() Delay rsp send result=%u at Port=%u SeqId=%u",
					Result, PortId, GSY_SWAP16(pFrameTx->PtcpData.DelayRsp.PtcpHdr.SequenceID.Word));
			}
			/* Receive again after delay response is sent
			*/
			K32GSY_RECV_FRAME(pFrameBuffer, k32_DelayFrameSize, K32_USETYPE_DELAY, InterfaceId);
			break;

		default:
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_SendConf() pChSys=%x: Invalid FrameId=0x%04x",
						pChSys, GSY_SWAP16(pFrameTx->PduHdr.FrameId.Word));
		}
	}
	else
	{
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SendConf() pChSys=%x: Invalid Interface=%u",
						pChSys, InterfaceId);
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SendConf() Interface=%x TimestampWait=%u Timestamp=%u",
					InterfaceId, TimestampWait, Timestamp);

	LSA_UNUSED_ARG(TimestampWait);
}

/*****************************************************************************/
/* External access function: k32gsy_Timeout()                                */
/* Handle timer                                                              */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_Timeout(
LSA_UINT16 			TimerId,
LSA_USER_ID_TYPE	UserId)
{
	GSY_TIMER_PTR	pTimer = (GSY_TIMER_PTR)UserId.void_ptr;

	if (pTimer->Id == TimerId)
	{
		if (pTimer->Running)
		{
			/* Call timer function
			*/
			pTimer->pTimeoutProc(pTimer);
		}
		else
		{
			GSY_ERROR_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** k32gsy_Timeout() Timer is not running: pTimer%x Id=%u",
							pTimer, TimerId);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_Timeout() Timer ID mismatch: pTimer=%x ->Id=%u TimerId=%u",
						pTimer, pTimer->Id, TimerId);
	}
}

/*****************************************************************************/
/* Internal access function: k32gsy_TimerStart()                             */
/*****************************************************************************/
LSA_VOID  K32_LOCAL_FCT_ATTR  k32gsy_TimerStart(
GSY_TIMER_PTR	pTimer,
LSA_UINT16		Time)
{
	LSA_USER_ID_TYPE	UserId;
	LSA_UINT16			RetVal = GSY_RET_OK;

	GSY_FUNCTION_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_TimerStart() pTimer=%x Time=%d",
					pTimer, Time);

	if (pTimer->Running)
	{
		GSY_ERROR_TRACE_03(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_TimerStart() Timer is already running: pTimer%x Id=%u Time=%u",
						pTimer, pTimer->Id, Time);
	}
	else
	{
		UserId.void_ptr = pTimer;
		pTimer->Running = LSA_TRUE;
		pTimer->Time = 0;

		RetVal = K32GSY_TIMER_START(pTimer->Id, UserId, Time);
		if (RetVal == 0)
		{
			pTimer->Running = LSA_FALSE;
			GSY_ERROR_TRACE_03(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_TimerStart() Error from GSY_TIMER_START: pTimer%x Id=%u Time=%u",
						pTimer, pTimer->Id, Time);
		}
	}

	GSY_FUNCTION_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_TimerStart() pTimer=%x RetVal=0x%x",
					pTimer, RetVal);
}

/*****************************************************************************/
/* Internal access function: k32gsy_TimerStop()                              */
/*****************************************************************************/
LSA_UINT16  K32_LOCAL_FCT_ATTR  k32gsy_TimerStop(
GSY_TIMER_PTR  		pTimer)
{
	LSA_UINT16 RetVal = GSY_RET_OK;

	GSY_FUNCTION_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_TimerStop() pTimer=%x Time=%d",
					pTimer, pTimer->Time);

	if (pTimer->Running)
	{
		/* Stop timer
		*/
		pTimer->Running = LSA_FALSE;
        RetVal = K32GSY_TIMER_STOP(pTimer->Id);
		if (RetVal == 0)
		{
			RetVal = GSY_RET_ERR_PARAM;
			pTimer->Running = LSA_TRUE;
			GSY_ERROR_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_TimerStop() Error from K32GSY_TIMER_STOP: pTimer%x Id=%u",
						pTimer, pTimer->Id);
		}
	}

	GSY_FUNCTION_TRACE_02(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_TimerStop() pTimer=%x RetVal=0x%x",
					pTimer, RetVal);
	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: k32gsy_SysPtrGet()                              */
/* Return a pointer to the interface data structure.                         */
/*****************************************************************************/
GSY_CH_K32_PTR K32_LOCAL_FCT_ATTR k32gsy_SysPtrGet(
LSA_UINT8	InterfaceId)
{
	GSY_CH_K32_PTR	pChSys = LSA_NULL;

	GSY_FUNCTION_TRACE_01(0, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_SysPtrGet() Interface=%u",
		InterfaceId);

	if (K32_INTERFACE_VALID(InterfaceId))
	{
		pChSys = &k32_InterfaceChannel[InterfaceId - K32_INTERFACE_FIRST];
		GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_SysPtrGet() pChSys=%x Interface=%u",
						pChSys, InterfaceId);
	}
	else
	{
		GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_SysPtrGet() pChSys=%x: Invalid Interface=%u",
			pChSys, InterfaceId);
	}

	return (pChSys);
}

/*****************************************************************************/
/* Internal access function: k32gsy_DelayGet()                               */
/* This function transforms the delay values from TLVInfo to seconds and     */
/* nanoseconds in the given structure (*pTime) for drift calculation and     */
/* synchronisation.                                                          */
/* Return: LSA_FALSE:  invalid delay values                                  */
/*         LSA_TRUE:   OK.                                                   */
/*****************************************************************************/
LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_DelayGet(
GSY_CH_K32_PTR  		pChSys,
GSY_TIME_TYPE 			*pTime,
GSY_TLV_INFO_PTR_TYPE	pTLVInfo,
LSA_UINT16				PortId)
{
	LSA_UINT32			DelayNanos = 0;
	LSA_UINT32			DelaySeconds = 0;
	LSA_BOOL			PduOk = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayGet() pTime=%x pTLVInfo=%x Port=%u",
					pTime, pTLVInfo, PortId);

	if (GSY_TLV_TYPE_SYNC == pTLVInfo->Type)
	{
		if (((pTLVInfo->Param.Sync.Delay1NS & 0x0f) < 10)
		&&  (pTLVInfo->Param.Sync.Delay10NS != GSY_PTCP_DELAY_NS_INVALID)	//150910lrg001
		&&  (pTLVInfo->Param.Sync.DelayNS != GSY_PTCP_DELAY_NS_INVALID))
		{
			/* Calculate seconds and nanoseconds from SyncDelay
			*/
			DelayNanos = pTLVInfo->Param.Sync.Delay10NS;
			while (DelayNanos >= GSY_10N_PER_SECOND)
			{
				DelayNanos -= GSY_10N_PER_SECOND;
				DelaySeconds++;
			}
			DelayNanos *= 10;
			DelayNanos += pTLVInfo->Param.Sync.Delay1NS & 0x0f;
			pTime->DelaySeconds = DelaySeconds;
			pTime->DelayNanos = DelayNanos;

			DelayNanos = pTLVInfo->Param.Sync.DelayNS;
			while (DelayNanos >= GSY_NANOS_PER_SECOND)
			{
				DelayNanos -= GSY_NANOS_PER_SECOND;
				pTime->DelaySeconds++;
			}
			pTime->DelayNanos += DelayNanos;
 		}
		else
			PduOk = LSA_FALSE;
	}
	else if (GSY_TLV_TYPE_FOLLOWUP == pTLVInfo->Type)
	{
		/* Calculate seconds and nanoseconds from FollowUpDelay
		*/
		if (pTLVInfo->Param.FollowUp.DelayNS < 0)
			DelayNanos = (LSA_UINT32)(-pTLVInfo->Param.FollowUp.DelayNS);
		else
			DelayNanos = (LSA_UINT32)pTLVInfo->Param.FollowUp.DelayNS;
		DelaySeconds = 0;

		while (DelayNanos >= GSY_NANOS_PER_SECOND)
		{
			DelayNanos -= GSY_NANOS_PER_SECOND;
			DelaySeconds++;
		}

		if (pTLVInfo->Param.FollowUp.DelayNS < 0)
		{
			/* Subtract Delay of FollowUp from Delay of SyncFrame
			*/
			if (pTime->DelaySeconds >= DelaySeconds)
			{
				pTime->DelaySeconds -= DelaySeconds;
				if (pTime->DelayNanos >= DelayNanos)
					pTime->DelayNanos -= DelayNanos;
				else if (pTime->DelaySeconds > 0)
				{
					pTime->DelaySeconds--;
					pTime->DelayNanos = GSY_NANOS_PER_SECOND - DelayNanos + pTime->DelayNanos;
				}
				else
					PduOk = LSA_FALSE;
			}
			else
				PduOk = LSA_FALSE;
		}
		else
		{
			/* Add Delay of FollowUp to Delay of SyncFrame
			*/
			pTime->DelaySeconds += DelaySeconds;
			pTime->DelayNanos += DelayNanos;
			while (pTime->DelayNanos >= GSY_NANOS_PER_SECOND)
			{
				pTime->DelayNanos -= GSY_NANOS_PER_SECOND;
				pTime->DelaySeconds++;
			}
		}
	}
	else
		PduOk = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayGet() DelaySeconds=%u DelayNanos=%u RetVal=%d",
					pTime->DelaySeconds, pTime->DelayNanos, PduOk);

	LSA_UNUSED_ARG(pChSys);
	LSA_UNUSED_ARG(PortId);
	return(PduOk);
}

/*****************************************************************************/
/* Internal access function: k32gsy_TLVGetInfo()                             */
/* -> function validates PDU (incl. TLV-Chain) and fills structure TLVInfo.  */
/* -> TLVInfo contains parameters dependent of the PDU type.                 */
/* pPduRx   points to start of PDU in frame (Frame-ID)                       */
/* PDULen   length of PDU                                                    */
/* pTLVInfo pointer to structure to be filled                                */
/* Return:  LSA_FALSE:  invalid PDU                                          */
/*          LSA_TRUE:   OK. Structure filled.                                */
/*****************************************************************************/
LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_TLVGetInfo(
GSY_CH_K32_PTR  		pChSys,
GSY_LOWER_RXBYTE_PTR	pPduRx,
LSA_UINT16				PduLen,
LSA_UINT16				PortId,
GSY_TLV_INFO_PTR_TYPE	pTLVInfo)
{
	GSY_LOWER_RX_PDU_PTR	pPdu;
	LSA_UINT16			Epoch, FrameId;
	LSA_UINT8			SyncId;
	LSA_INT				NextIdx;
	GSY_TLV_SUBDOMAIN	*pTlvSubdomain = LSA_NULL;
	GSY_DRIFT_PTR		pDrift = LSA_NULL;
	LSA_UINT32  		PduType = GSY_TLV_TYPE_NONE;
	LSA_UINT16  		PduLength = 0;
	LSA_BOOL			PduOk = LSA_FALSE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_TLVGetInfo() pPduRx=%x PduLen=%d pTLVInfo=%x",
					pPduRx, PduLen, pTLVInfo);

	pPdu = (GSY_LOWER_RX_PDU_PTR)(pPduRx + 2);
	FrameId = GSY_SWAP16(*((LSA_UINT16*)pPduRx));
	SyncId = GSY_LOBYTE(FrameId) & 0x1f;

	if (SyncId != pTLVInfo->SyncId)
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() SyncId=%u Port=%u: Invalid FrameId.SyncId=%u",
					pTLVInfo->SyncId, PortId, SyncId);
	}
	else if ((PduLen >= GSY_SYNC_RX_SIZE + 2)
	&&  ((GSY_FRAMEID_SYNC == FrameId) || (GSY_FRAMEID_FUSYNC == FrameId)
	  || (GSY_FRAMEID_TIME == FrameId) || (GSY_FRAMEID_FUTIME == FrameId)))
	{
		/* Sync PDU
		*/
		PduType = GSY_TLV_TYPE_SYNC;
		pTlvSubdomain = &(pPdu->Sync.TLV.Subdomain);
	}
	else if ((PduLen >= GSY_SYNC_FU_RX_SIZE + 2)
		 &&  ((GSY_FRAMEID_SYNC_FU == FrameId)
	      ||  (GSY_FRAMEID_TIME_FU == FrameId)))
	{
		/* Sync-FollowUp PDU
		*/
		PduType = GSY_TLV_TYPE_FOLLOWUP;
		pTlvSubdomain = &(pPdu->SyncFu.Subdomain);
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() pChSys=%x Port=%u: Invalid FrameId=0x%04x",
					pChSys, PortId, FrameId);
	}

	/* PDU-Check
	*/
	if (PduType != GSY_TLV_TYPE_NONE)
	{
		/* The first TLV has to be the same at Sync-, FollowUp und Announce PDUs
		*/
		if (GSY_TLV_SUBDOMAIN_TYPELEN == GSY_SWAP16(pTlvSubdomain->TypeLen.Word))
		{
			PduLength = 2 + GSY_PTCP_HDR_SIZE + GSY_TLV_SUBDOMAIN_SIZE;
			if (GSY_TLV_TYPE_SYNC == PduType)
			{
				if ((GSY_TLV_TIME_TYPELEN == GSY_SWAP16(pPdu->Sync.TLV.Time.TypeLen.Word))
				&&  (GSY_TLV_TIMEEXT_TYPELEN == GSY_SWAP16(pPdu->Sync.TLV.TimeExt.TypeLen.Word))
				&&  (GSY_TLV_MASTER_TYPELEN == GSY_SWAP16(pPdu->Sync.TLV.Master.TypeLen.Word))
				&&  (GSY_TLV_END_TYPELEN == GSY_SWAP16(pPdu->Sync.TLV.End.TypeLen.Word)))
				{
					/* 280907lrg002: check Epoch to be 0
					*/
					Epoch = GSY_SWAP16(pPdu->Sync.TLV.Time.EpochNumber.Word);
					if ((0 == Epoch) || (GSY_SYNCID_CLOCK != pTLVInfo->SyncId))		//lrgtime
					{
						PduLength += GSY_TLV_TIME_SIZE + GSY_TLV_TIMEEXT_SIZE + GSY_TLV_MASTER_SIZE;
						PduOk = LSA_TRUE;
					}
					else
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() Port=%u SeqId=%u: Invalid Time.Epoch=0x%04x in SyncFrame",
							PortId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word), Epoch);
					}
				}
			}
			else if (GSY_TLV_TYPE_FOLLOWUP == PduType)
			{
				if ((GSY_TLV_TIME_TYPELEN == GSY_SWAP16(pPdu->SyncFu.Time.TypeLen.Word))
				&&  (GSY_TLV_END_TYPELEN == GSY_SWAP16(pPdu->SyncFu.End.TypeLen.Word)))
				{
					PduLength += GSY_TLV_TIME_SIZE;
					PduOk = LSA_TRUE;
				}
			}
			if (PduOk)
			{
#ifdef GSY_MESSAGE
				/* Todo: scan option TLVs if present
				*/
#endif /* GSY_MESSAGE */
				PduLength += GSY_TLV_END_SIZE;
			}
			else
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() Port=%u FrameId=0x%04x SeqId=%u: Invalid TLV",
							PortId, FrameId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() Port=%u SeqId=%u: Invalid Subdomain TypeLen=0x%04x",
						PortId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word), GSY_SWAP16(pTlvSubdomain->TypeLen.Word));
		}
	}

	if (PduOk &&  GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pChSys->MACAddr))
	{
		/* 171109lrg001: check for PDU from myself here to avoid calculating drift etc.
		*/
		PduOk = LSA_FALSE;
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() Port=%u FrameId=0x%04x SeqId=%u: Sync/FU from myself",
							PortId, FrameId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
	}
	if (PduOk)
	{
		/* Search drift structure * 190906lrg001: only one per SyncId
		*/
		pDrift = &pChSys->Drift[SyncId];
		if (GSY_TLV_TYPE_SYNC == PduType)
		{
			/* 150107lrg001: Allow initialising only if subdomain matches or is dont care
			*/
			if ((GSY_SYNCID_NONE == pDrift->SyncId)
			&&  (GSY_SUBDOMAIN_IS_NULL(pDrift->Subdomain)
			 ||  GSY_SUBDOMAIN_CMP(pTlvSubdomain->SubdomainUUID, pDrift->Subdomain)))
			{
				GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"-X- k32gsy_TLVGetInfo(%02x) Old RCF master=%02x-%02x-%02x-%02x-%02x-%02x",
								pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
								pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5]);

				/* Structure free: initialise * 120606lrg001: set in brackets
				 * 200207lrg001: 2 x LSA_TRACE_LEVEL_NOTE_HIGH
				 * 161007lrg003: AP00526517 PrimaryMaster
				 * 210108lrg002: init OldMasterAddr for DriftCalc
				 * 170408lrg002: RcvSyncPrio instead of PrimaryMaster
				*/
				pDrift->OldMasterAddr = pDrift->MasterAddr;
				pDrift->MasterAddr = pTlvSubdomain->MasterSourceAddress;
				pDrift->Subdomain = pTlvSubdomain->SubdomainUUID;
				k32gsy_DriftSet(pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_UNKNOWN);

				GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_TLVGetInfo(%02x) New RCF master=%02x-%02x-%02x-%02x-%02x-%02x SeqId=%u Port=%u",
								pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
								pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5],
								GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word), PortId);
			}
			else if (!GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->MasterAddr))
			{
				/* Structure already used but new master 231006lrg: with same Subdomain
				 * 191006lrg001: but not used by local master
				 * 041007lrg005: NULL for Takeover
				*/
				if (GSY_SUBDOMAIN_CMP(pTlvSubdomain->SubdomainUUID, pDrift->Subdomain)
//				&&  !GSY_MACADDR_CMP(pChSys->MACAddr, pDrift->MasterAddr))
				&&  GSY_MACADDR_IS_NULL(pDrift->MasterAddr))
				{
					/* Replace MasterMAC 161007lrg003: AP00526517 PrimaryMaster
					 * 210108lrg002: init OldMasterAddr for DriftCalc
					 * 170408lrg002: RcvSyncPrio instead of PrimaryMaster
					*/
					pDrift->OldMasterAddr = pDrift->MasterAddr;
					pDrift->MasterAddr = pTlvSubdomain->MasterSourceAddress;
					GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_TLVGetInfo(%02x) Change RCF master=%02x-%02x-%02x-%02x-%02x-%02x SeqId=%u Port=%u",
								pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
								pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5],
								GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word), PortId);
				}
				else
				{
					/* 231106lrg001: ignore PDU of other Master for drift
					 * 141206lrg001: write ErrorTrace
					 * 200207lrg001: Text changed
					*/
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_TLVGetInfo() SyncId=0x%02x: Ignoring master from Port=%u SeqId=%u",
								pDrift->SyncId, PortId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
					pDrift = LSA_NULL;
				}
			}
		}
		else if (!GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->MasterAddr))
		{
			/* FollowUp with other SourceMAC
			*/
			pDrift = LSA_NULL;
		}

		/* Fill return structure
		 * 250707lrg001: ignore APDU status (not in Length = ->FrameId,Header...EndTlv<-)
		*/
		pTLVInfo->Type = PduType;
		pTLVInfo->Length = PduLength;
		pTLVInfo->Subdomain = pTlvSubdomain->SubdomainUUID;

		if (GSY_TLV_TYPE_SYNC == PduType)
		{
			pTLVInfo->Param.Sync.Epoch = GSY_SWAP16(pPdu->Sync.TLV.Time.EpochNumber.Word);
			pTLVInfo->Param.Sync.Seconds = GSY_SWAP32(pPdu->Sync.TLV.Time.Seconds.Dword);
			pTLVInfo->Param.Sync.Nanos = GSY_SWAP32(pPdu->Sync.TLV.Time.NanoSeconds.Dword);
			pTLVInfo->Param.Sync.UTCOffset = GSY_SWAP16(pPdu->Sync.TLV.TimeExt.CurrentUTCOffset.Word);
			pTLVInfo->Param.Sync.TimeFlags = GSY_SWAP16(pPdu->Sync.TLV.TimeExt.Flags.Word);
			pTLVInfo->Param.Sync.Delay10NS = GSY_SWAP32(pPdu->Sync.PtcpHdr.Delay10ns.Dword);
			pTLVInfo->Param.Sync.Delay1NS = pPdu->Sync.PtcpHdr.Delay1ns;
			pTLVInfo->Param.Sync.DelayNS = GSY_SWAP32(pPdu->Sync.PtcpHdr.DelayNS.Dword);
			pTLVInfo->Param.Sync.Delay10NSOffset = GSY_PTCP_DELAY10_OFFSET;
			pTLVInfo->Param.Sync.Delay1NSOffset = GSY_PTCP_DELAY1_OFFSET;
			pTLVInfo->Param.Sync.DelayNSOffset = GSY_PTCP_DelayNS_OFFSET;
			pTLVInfo->Param.Sync.SequenceID = GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word);
			pTLVInfo->Param.Sync.Master.MacAddr = pTlvSubdomain->MasterSourceAddress;
			pTLVInfo->Param.Sync.Master.Variance = GSY_SWAP16(pPdu->Sync.TLV.Master.ClockVariance.Word);
			pTLVInfo->Param.Sync.Master.Accuracy = pPdu->Sync.TLV.Master.ClockAccuracy;
			pTLVInfo->Param.Sync.Master.Class = pPdu->Sync.TLV.Master.ClockClass;
			pTLVInfo->Param.Sync.Master.Priority1 = pPdu->Sync.TLV.Master.MasterPriority1;
			pTLVInfo->Param.Sync.Master.Priority2 = pPdu->Sync.TLV.Master.MasterPriority2;
			pTLVInfo->Param.Sync.Master.Receipt = 1;

			if ((GSY_FRAMEID_FUSYNC == FrameId) || (GSY_FRAMEID_FUTIME == FrameId))
				pTLVInfo->Param.Sync.FUFlag = LSA_TRUE;
			else
				pTLVInfo->Param.Sync.FUFlag = LSA_FALSE;

			if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL))
			{
				/* Store sync values for drift clalculation
				*/
				NextIdx = pDrift->NextIdx;
				pDrift->Act.PortId = PortId;
				pDrift->Act.SeqId = pTLVInfo->Param.Sync.SequenceID;
				pDrift->Act.Timestamp = GSY_SWAP32(pPdu->Sync.PtcpHdr.ApiRecvTS.Dword);		// 230410lrg001: Local ApiTimeStamp for rate
				pDrift->Act.OrgSeconds = GSY_SWAP32(pPdu->Sync.TLV.Time.Seconds.Dword);
				pDrift->Act.OrgNanos = GSY_SWAP32(pPdu->Sync.TLV.Time.NanoSeconds.Dword);

				/* 170408lrg002: RcvSyncPrio instead of PrimaryMaster
				*/
				pDrift->RcvSyncPrio = pPdu->Sync.TLV.Master.MasterPriority1;

				/* Write Delay into drift structure
				*/
				PduOk = k32gsy_DelayGet(pChSys, &pDrift->Act, pTLVInfo, PortId);
				if (PduOk)
				{
					GSY_SYNC_TRACE_10(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_TLVGetInfo(%02x,SY) Port=%u Timestamp=%10u SeqIdOld/New/Act=%u/%u/%u UsedOld/New/Act=%d/%d/%d NextIdx=%u",
							pDrift->SyncId, PortId, pDrift->Act.Timestamp, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId,
							pDrift->Act.SeqId, pDrift->Old[NextIdx].Used, pDrift->New[NextIdx].Used, pDrift->Act.Used, NextIdx);

					if (pTLVInfo->Param.Sync.FUFlag)
						pDrift->Act.Used = LSA_TRUE;
					else
					{
						/* Transfer actual PDU values to new values
						 * 180108lrg001: and calculate the rate based on the new values
						*/
						pDrift->Act.Used = LSA_FALSE;
						pDrift->New[NextIdx] = pDrift->Act;
						k32gsy_DriftCalc(pChSys, pDrift);

					}
				}
				else
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() Invalid sync delay: Delay10NS=%u Delay1NS=%u DelayNS=%u",
									pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS, pTLVInfo->Param.Sync.DelayNS);
				}
			}
		}
		else if (GSY_TLV_TYPE_FOLLOWUP == PduType)
		{
			pTLVInfo->Param.FollowUp.DelayNS = GSY_SWAP32(pPdu->SyncFu.PtcpHdr.DelayNS.Dword);
			pTLVInfo->Param.FollowUp.DelayNSOffset = GSY_PTCP_DelayNS_OFFSET;
			pTLVInfo->Param.FollowUp.SequenceID = GSY_SWAP16(pPdu->SyncFu.PtcpHdr.SequenceID.Word);
			pTLVInfo->Param.FollowUp.Master.MacAddr = pTlvSubdomain->MasterSourceAddress;

			if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
			&&	(pDrift->Act.Used)
			&&	(pDrift->Act.SeqId == pTLVInfo->Param.FollowUp.SequenceID)
			&&  (pDrift->Act.PortId == PortId))
			{
				/* Refresh Delay in drift structure
				*/
				NextIdx = pDrift->NextIdx;
				PduOk = k32gsy_DelayGet(pChSys, &pDrift->Act, pTLVInfo, PortId);
				if (PduOk)
				{
					/* Transfer actual PDU values to new values
					 * 180108lrg001: and calculate the rate based on the new values
					*/
					GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_TLVGetInfo(%02x,FU) Port=%u SeqIdOld/New/Act=%u/%u/%u UsedOld/New/Act=%d/%d/%d NextIdx=%u",
						pDrift->SyncId, PortId, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId, pDrift->Act.SeqId,
						pDrift->Old[NextIdx].Used, pDrift->New[NextIdx].Used, pDrift->Act.Used, NextIdx);

					pDrift->Act.Used = LSA_FALSE;
					pDrift->New[NextIdx] = pDrift->Act;
					k32gsy_DriftCalc(pChSys, pDrift);
				}
				else
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo() SyncId=%u: Invalid FU sync delay at SeqId=%u: DelayNS=%d",
									pTLVInfo->SyncId, pTLVInfo->Param.FollowUp.SequenceID, pTLVInfo->Param.FollowUp.DelayNS);
				}
			}
			else if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL))
			{
				GSY_SYNC_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo(%02x,FU) ignoring from Port=%u/%u SeqId=%u/%u Used=%u",
								pTLVInfo->SyncId, pDrift->Act.PortId, PortId, pDrift->Act.SeqId, pTLVInfo->Param.FollowUp.SequenceID, pDrift->Act.Used);
			}
			else
			{
				GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_TLVGetInfo(%02x,FU) Port=%u SeqId=%u pDrift=NULL",
								pTLVInfo->SyncId, PortId, pTLVInfo->Param.FollowUp.SequenceID);
			}
		}
		pTLVInfo->pDrift = pDrift;

		/* 090707lrg001: Set RateReady for the forwarding module
		*/
		pTLVInfo->RateReady = LSA_FALSE;
		if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
		&& ((GSY_DRIFT_STATE_READY == pDrift->State)
		 || (GSY_DRIFT_STATE_MASTER == pDrift->State)
		 || (GSY_DRIFT_STATE_TRANSIENT == pDrift->State)))
			pTLVInfo->RateReady = LSA_TRUE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_TLVGetInfo() pPduRx=%x FrameId=0x%04x PduOk=%d",
					pPduRx, FrameId, PduOk);
	return(PduOk);
}

/* 310707js001****************************************************************/
/* Internal function: k32gsy_DriftCalcRealInterval()                         */
/*                                                                           */
/* Calculate the real rate for SyncId=0 to the master including the          */
/* drift_correction already set and working in EDD (at ERTEC200/SOC). By     */
/* setting the drift_correction the counter for the timstamps is modified    */
/* and therefore the calculated rate interval is not the real one.           */
/* The EDD needs the real interval.                                          */
/*                                                                           */
/* This calculation is only done for SyncID 0!                               */
/* For SyncID <> 0 the interval will not be modified.                        */
/*                                                                           */
/* Input:  Interval measured (INT)                                           */
/* Return: Interval (INTr) to set next with respect to the actual set        */
/*         drift_correction (INTset).                                        */
/*                                                                           */
#ifdef GSY_MESSAGE
/* Es gilt:                                                                  */
/*                                                                           */
/* RCFr  = RCF * RCFset                                                      */
/*                                                                           */
/* RCF = (1/INT) +1                                                          */
/*                                                                           */
/* INTr= (INT * INTset) / ( 1+ INT + INTset )                                */
/*                                                                           */
/* die 1+  im Nenner kann man vernachlässigen, weil INT > 4000..             */
/*                                                                           */
/*                                                                           */
/* Damit                                                                     */
/*                                                                           */
/* INTr = 1 / ((1/INT) + ( 1/INTset))                                        */
/*                                                                           */
/* normiert um mit integer rechnen zu können:                                */
/*                                                                           */
/*                                                                           */
/* INTr = 2^30 / ((2^30/INT) + (2^30/INTset))                                */
/*                                                                           */
/*                                                                           */
/* Durch die Berechnung gibt es bei großen Intervallen gewisse Rundungsfehler*/
/* Falls der MIN-Wert unterschritten wird wird er auf MIN gesetzt.           */
/*                                                                           */
#endif /* GSY_MESSAGE */
/*****************************************************************************/
K32_STATIC LSA_INT32 K32_LOCAL_FCT_ATTR k32gsy_DriftCalcRealInterval(
    GSY_CH_K32_PTR  pChSys,
    GSY_DRIFT_PTR	pDrift,
    LSA_INT32	  	Interval)

{
    LSA_INT32       IntervalReal;
    LSA_INT32       IntervalSet;
    LSA_INT32       Norm = 0x40000000; /* == 2^30 */
    LSA_INT32       Help;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftCalcRealInterval() pChSys=%x pDrift=%x, Interval=%d",
				pChSys, pDrift, Interval);

    IntervalSet = pDrift->IntervalSet; /* Driftcorrection SET (if IntervalSetValid) */

    if (( pDrift->IntervalSetValid ) &&	/* Interval in HW gesetzt ? */
        ( IntervalSet))					/* Interval <> 0 (0= no modification) */
    {

        GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"--- k32gsy_DriftCalcRealInterval() IntervalSet=%d",IntervalSet);

        /* drift_correction in hardware: calculate Real
        */
        if (Interval)
        {
            /* Note: Intervals shall be > +/-1 !!!
            */
            Help = ((Norm/Interval) + (Norm/IntervalSet));

            if (Help)
            {
                IntervalReal = Norm / Help;

                /* Set to MIN value if lower than MIN. On 0 result the intervals are too small
                 * (one oder both is/are 1). The intervals usually are >= 4000.
                */
                if (IntervalReal == 0)
                {
                    GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_DriftCalcRealInterval() Interval to small!");

                    if (Interval < 0)
                        IntervalReal = (0-GSY_DRIFT_MIN_INTERVAL);
                    else
                        IntervalReal = GSY_DRIFT_MIN_INTERVAL;
                }
                else
                if ((IntervalReal > 0) && (IntervalReal < GSY_DRIFT_MIN_INTERVAL))
                {
                    GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalcRealInterval() Underflow. Use + Minimum");
                    IntervalReal = GSY_DRIFT_MIN_INTERVAL;
                }
                else
                if ((IntervalReal < 0) && (IntervalReal > (0-GSY_DRIFT_MIN_INTERVAL)))
                {
                    GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalcRealInterval() Underflow. Use - Minimum");
                    IntervalReal = (0-GSY_DRIFT_MIN_INTERVAL);
                }

                GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"--- k32gsy_DriftCalcRealInterval() IntervalReal=%d",IntervalReal);

            }
            else
            {
                /* Help = 0 means there is NO drift. Use Maximum
                */

                GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalcRealInterval() Overflow. Use Maximum");

                IntervalReal = GSY_DRIFT_MAX_INTERVAL;
            }
        }
        else
        {
            GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_DriftCalcRealInterval() Interval is 0");
#ifdef GSY_MESSAGE
            /* Invalid state */
#endif /* GSY_MESSAGE */
            IntervalReal = Interval;
        }
    }
    else
    {
        /* No Interval set in HW
        */
        IntervalReal = Interval;
    }

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftCalcRealInterval() pChSys=%x RealInterval=%d",
				pChSys, IntervalReal);

	LSA_UNUSED_ARG(pChSys);
    return(IntervalReal);
}

/*****************************************************************************/
/* Internal access function: k32gsy_DriftSet()                               */
/* Set and publish new drift interval and state.                             */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DriftSet(
GSY_CH_K32_PTR 		pChSys,
GSY_DRIFT_PTR		pDrift,
LSA_UINT8			SyncId,
LSA_INT32			Interval,
LSA_UINT8			State)
{
	LSA_BOOL	RateValid = LSA_TRUE;
	LSA_UINT8	SyncIdRate = pDrift->SyncId;
	LSA_UINT8	OldSyncId = pDrift->SyncId;
	LSA_UINT8	OldState = pDrift->State;
	LSA_INT32	OldInterval = pDrift->AveInterval;
	LSA_UINT32	RateCompensation = 0;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftSet() pChSys=%x pDrift=%x State=%u",
					pChSys, pDrift, State);

	pDrift->State = State;
	pDrift->SyncId = SyncId;
	pDrift->AveInterval = Interval;

	if (pDrift->AveInterval < 0)
	{
		pDrift->CorrectInterval = (LSA_UINT32)(0 - pDrift->AveInterval);
		pDrift->CorrectPlus = LSA_FALSE;
	}
	else
	{
		pDrift->CorrectInterval = (LSA_UINT32)pDrift->AveInterval;
		pDrift->CorrectPlus = LSA_TRUE;
	}

	GSY_SYNC_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftSet() Old/New: SyncId=%x/%x Interval=%d/%d State=%u/%u",
					OldSyncId, pDrift->SyncId, OldInterval, pDrift->AveInterval, OldState, pDrift->State);

	if ((Interval != OldInterval)
	||  ((State != OldState)
	 &&  ((State == GSY_DRIFT_STATE_UNKNOWN) || (OldState == GSY_DRIFT_STATE_UNKNOWN))))
	{
		/* Publish change of interval
		*/
		if (GSY_SYNCID_NONE != SyncId)
			SyncIdRate = SyncId;
		else if (GSY_SYNCID_NONE != OldSyncId)
			SyncIdRate = OldSyncId;
		if (GSY_SYNCID_NONE != SyncIdRate)
		{
 			if (GSY_DRIFT_STATE_UNKNOWN == State)
				RateValid = LSA_FALSE;

			RateCompensation = pDrift->CorrectInterval;
			if (RateCompensation > K32_COMP_INTERVAL)
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_DriftSet() limit CorrectInterval=0x%x to K32_COMP_INTERVAL=%x",
								RateCompensation, K32_COMP_INTERVAL);
				RateCompensation &= K32_COMP_INTERVAL;
			}
			if (RateValid)
				RateCompensation |= K32_COMP_VALID;
			if (pDrift->CorrectPlus)
				RateCompensation |= K32_COMP_PLUS;

			GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--> k32gsy_DriftSet(%02x) K32GSY_RCF_SET_x: Interface=%u Interval=%d Value=0x%08x State=%u",
							SyncIdRate, pChSys->InterfaceId, pDrift->AveInterval, RateCompensation, State);

 			if (GSY_SYNCID_TIME == SyncIdRate)
			{
				K32GSY_RCF_SET_TIME(RateCompensation);
			}
 			else if (K32_INTERFACE_ID_A == pChSys->InterfaceId)
			{
				K32GSY_RCF_SET_CLOCK_A(RateCompensation);
				K32GSY_RCF_SET_DELAY(RateCompensation);		// AP01482167:LineDelayMessung
			}
 			else
			{
				K32GSY_RCF_SET_CLOCK_B(RateCompensation);
			}
            /* 310707js001: save currently values
            */
            pDrift->IntervalSet      = pDrift->AveInterval;
            pDrift->IntervalSetValid = RateValid;

			if (((State != OldState) && ((State == GSY_DRIFT_STATE_UNKNOWN) || (OldState == GSY_DRIFT_STATE_UNKNOWN)))
			||  ((Interval != OldInterval) &&  ((Interval == 0) || (OldInterval == 0))))
			{
				/* 021209lrg001:
				 * (De)activate SeqID check in hardware by setting the master
				 *  MAC address if no slave or master is running.
				*/
				if (GSY_SLAVE_STATE_OFF == pChSys->Slave[SyncIdRate].State)
				{
					GSY_MAC_ADR_TYPE NoMasterAddr = {GSY_NO_MASTER_ADDR};
					GSY_PTCP_UUID NoSubdomain;
					GSY_MEMSET_LOCAL(&NoSubdomain, 0, sizeof(GSY_PTCP_UUID));

					if ((Interval == 0) || (State == GSY_DRIFT_STATE_UNKNOWN))
					{
						k32gsy_FwdSet(pChSys, SyncIdRate, LSA_TRUE, NoSubdomain, NoMasterAddr);
					}
					else
					{
						k32gsy_FwdSet(pChSys, SyncIdRate, LSA_FALSE, NoSubdomain, pDrift->MasterAddr);
					}
				}

				/* Update diagnosis data and send up Indication
				 * 030909lrg001: also if rate changes from/to 0 (because of Master-MAC)
				*/
				GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- k32gsy_DriftSet(%02x) SeqId=%u Interval=%d: drift state change [%u]->[%u]",
					SyncIdRate, pDrift->Act.SeqId, pDrift->AveInterval, OldState, State);
				pDrift->Diag.pK32Rqb->Params.DiagInd.SyncId = SyncIdRate;
				pDrift->Diag.pK32Rqb->Params.DiagInd.DiagSrc = GSY_DIAG_SOURCE_DRIFT;
				pDrift->Diag.pK32Rqb->Params.DiagInd.SlaveState = pChSys->Slave[SyncIdRate].State;
                K32_COPY_MAC_ADDR_ARRAY (pDrift->Diag.pK32Rqb->Params.DiagInd.MasterMacAddr, pDrift->MasterAddr.MacAdr);
				pDrift->Diag.pK32Rqb->Params.DiagInd.RcvSyncPrio = pDrift->RcvSyncPrio;
				pDrift->Diag.pK32Rqb->Params.DiagInd.RateInterval = pDrift->AveInterval;
				pDrift->Diag.pK32Rqb->Params.DiagInd.SequenceId = pDrift->Act.SeqId;
				pDrift->Diag.pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
				K32GSY_CALLBACK(pDrift->Diag.pK32Rqb);
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftSet() SyncId=%02x OldInterval=%d NewInterval=%d",
					SyncId, OldInterval, Interval);
}

/*****************************************************************************/
/* Internal function: k32gsy_DriftMedian()                                   */
/* Build median value of a sample.                                           */
/*****************************************************************************/
K32_STATIC LSA_INT32 K32_LOCAL_FCT_ATTR k32gsy_DriftMedian(
GSY_CH_K32_PTR  	pChSys,
GSY_DRIFT_PTR		pDrift)
{
	LSA_BOOL	Moved;
	LSA_INT		Idx;
	LSA_INT		DriftMaxCount = GSY_DRIFT_MAX_COUNT - pDrift->SyncId*(GSY_DRIFT_MAX_COUNT-3); //lrgdrift: calculate rate all 3 frames at SyncId 1
	LSA_INT32	Tmp;
	LSA_INT32	Sort[GSY_DRIFT_MAX_COUNT] = {0};
	LSA_INT32	Median = 0;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftMedian() pDrift=%x DriftMaxCount=%u",
					pDrift, DriftMaxCount);

	if ((DriftMaxCount%2) == 0)
	{
		/* Even count of sample values is not supported
		*/
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DriftMedian() even DriftMaxCount=%u not supported",
						DriftMaxCount);
	}
	else
	{
		/* Coppy values sorted by time into sort array
		 * e.g. (10, -10, 2, -3, -5, 7, 1)
		*/
		for (Idx = 0; Idx < DriftMaxCount; Idx++)
		{
			Sort[Idx] = pDrift->AveActInterval[Idx];
		}
#ifdef GSY_MESSAGE
//		GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"->- k32gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} [%u]",
//						pDrift->SyncId, Sort[0], Sort[1], Sort[2], Sort[3], Sort[4], Sort[5], Sort[6], GSY_DRIFT_MAX_COUNT);
#endif /* GSY_MESSAGE */

		/* Sort values of RCF intervals for median by size, where a great interval represents a small rate:
		 * at the left negative or greater values with same algebraic sign
		 * e.g. (-3, -5, -10, 10, 7, 2, 1)
		*/
		do
		{
			/* Pass through the sort array until no more element is swapped
			*/
			Moved = LSA_FALSE;
			for (Idx = 0; Idx < (DriftMaxCount-1); Idx++)
			{
				/* If left element is positive and right one negative
				 * or both have same sign and left element is smaller than the right one:
				*/
				if (((Sort[Idx] > 0) && (Sort[Idx+1] < 0))
				||  ((Sort[Idx] < Sort[Idx+1])
				 &&  (((Sort[Idx] > 0) && (Sort[Idx+1] > 0)) || ((Sort[Idx] < 0) && (Sort[Idx+1] < 0)))))
				{
					/* Swap the elements
					*/
					Tmp = Sort[Idx];
					Sort[Idx] = Sort[Idx+1];
					Sort[Idx+1] = Tmp;
					Moved = LSA_TRUE;
				}
//				GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"-*- k32gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} :%d",
//								pDrift->SyncId, Sort[0], Sort[1], Sort[2], Sort[3], Sort[4], Sort[5], Sort[6], Idx);
			}
		} while (Moved);

		/* The Median now is the element in the middle of the sort array
		*/
		Median = Sort[(DriftMaxCount-1)/2];

//		GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} =%d",
//						pDrift->SyncId, Sort[0], Sort[1], Sort[2], Sort[3], Sort[4], Sort[5], Sort[6], Median);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftMedian() pDrift=%x Median=%d",
					pDrift, Median);
	LSA_UNUSED_ARG(pChSys);
	return(Median);
}

/*****************************************************************************/
/* Internal function: k32gsy_DriftCalc()                                     */
/* Calculate drift (rate) interval.                                          */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DriftCalc(
GSY_CH_K32_PTR		pChSys,
GSY_DRIFT_PTR		pDrift)
{
	LSA_UINT32	DiffTimeStamp, TicksPerSecond;
	LSA_UINT32	SlaveInterval, DriftNanosU;
	LSA_INT32	MasterSeconds, MasterNanos;
	LSA_INT32	SlaveSeconds, SlaveNanos;
	LSA_INT32	DriftSeconds, DriftNanos;
	LSA_INT32	DriftInterval = 0;
	LSA_INT32	NewAveInterval = 0;
	LSA_INT		Idx = 0;
	LSA_INT		NextIdx = pDrift->NextIdx;
	LSA_INT		DriftMaxCount = GSY_DRIFT_MAX_COUNT - pDrift->SyncId*(GSY_DRIFT_MAX_COUNT-3); //lrgdrift: calculate rate all 3 frames at SyncId 1

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftCalc() pChSys=%x OldSeqId=%u NewSeqId=%u",
					pChSys, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId);

	if (!GSY_MACADDR_CMP(pDrift->OldMasterAddr, pDrift->MasterAddr))
	{
		/* 180108lrg001: if Master has changed:
		 * Invalidate all frames of the previous master
		*/
		for (Idx = 0; Idx < DriftMaxCount ; Idx++)
		{
			pDrift->Old[Idx].Used = LSA_TRUE;
		}
		/* 210108lrg001: Reset Median History on DriftReset
		*/
		if (GSY_DRIFT_STATE_UNKNOWN == pDrift->State)
			pDrift->AveSumCount = 0;

		GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) New master: Old.Used reset DriftMaxCount=%d",pDrift->SyncId, DriftMaxCount);
	}
	else if (!(pDrift->Old[NextIdx].Used))
	{
		/* Old valid SyncFrame (280906lrg001: from actual Master) is available!
		 * Calculate interval for drift correction:
		 * 1. MasterTimeDiff = MasterTimeNew - MasterTimeOld
		 * 2. SlaveTimeDiff = (SlaveTimeStampNew - DelayNew) - (SlaveTimeStampOld - DelayOld)
		 * -> SlaveTimeDiff = SlaveTimeStampNew - SlaveTimeStampOld - DelayNew + DelayOld
		 * 3. SlaveDriftCount = MasterTimeDiff - SlaveTimeDiff
		 *    SlaveDriftInterval = SlaveTimeDiff/SlaveDriftCount (-> round)
		*/
		MasterSeconds = pDrift->New[NextIdx].OrgSeconds - pDrift->Old[NextIdx].OrgSeconds;
		MasterNanos = pDrift->New[NextIdx].OrgNanos - pDrift->Old[NextIdx].OrgNanos;
		if ((MasterSeconds < 0) && (MasterNanos > 0))
		{
			MasterSeconds++;
			MasterNanos -= GSY_NANOS_PER_SECOND;
		}
		else if ((MasterSeconds > 0) && (MasterNanos < 0))
		{
			MasterSeconds--;
			MasterNanos += GSY_NANOS_PER_SECOND;
		}
		/* 2. Slave
		*/
		DiffTimeStamp = pDrift->New[NextIdx].Timestamp - pDrift->Old[NextIdx].Timestamp;
		TicksPerSecond = GSY_NANOS_PER_SECOND / pChSys->RxTxNanos;
		SlaveSeconds = DiffTimeStamp / TicksPerSecond;
		SlaveNanos = (DiffTimeStamp % TicksPerSecond) * pChSys->RxTxNanos;
		while (SlaveNanos >= GSY_NANOS_PER_SECOND)
		{
			SlaveNanos -= GSY_NANOS_PER_SECOND;
			SlaveSeconds++;
		}

		GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) SeqIdOld/New=%u/%u SlaveSs:Ns=%d:%d",
						pDrift->SyncId, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId, SlaveSeconds, SlaveNanos);
		GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) OldDelaySs:Ns=%d:%d NewDelaySs:Ns=%d:%d",
						pDrift->SyncId, pDrift->Old[NextIdx].DelaySeconds, pDrift->Old[NextIdx].DelayNanos,
						pDrift->New[NextIdx].DelaySeconds, pDrift->New[NextIdx].DelayNanos);

		/* Delays
		*/
		SlaveSeconds -= pDrift->New[NextIdx].DelaySeconds;
		SlaveNanos -= pDrift->New[NextIdx].DelayNanos;
		if ((SlaveSeconds < 0) && (SlaveNanos > 0))
		{
			SlaveSeconds++;
			SlaveNanos -= GSY_NANOS_PER_SECOND;
		}
		else if ((SlaveSeconds > 0) && (SlaveNanos < 0))
		{
			SlaveSeconds--;
			SlaveNanos += GSY_NANOS_PER_SECOND;
		}
		SlaveSeconds += pDrift->Old[NextIdx].DelaySeconds;
		SlaveNanos += pDrift->Old[NextIdx].DelayNanos;
		while (SlaveNanos >= GSY_NANOS_PER_SECOND)
		{
			SlaveNanos -= GSY_NANOS_PER_SECOND;
			SlaveSeconds++;
		}

		if ((GSY_SECONDS_PER_DWORD < SlaveSeconds)
		||  ((GSY_SECONDS_PER_DWORD == SlaveSeconds) && (SlaveNanos > GSY_MAX_NANOS)))
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_DriftCalc() pChSys=%x Slave interval too large: Seconds=%d Nanos=%d",
						pChSys, SlaveSeconds, SlaveNanos);
		}
		else
		{
			SlaveInterval = (LSA_UINT32)SlaveSeconds * GSY_NANOS_PER_SECOND;
			SlaveInterval += (LSA_UINT32)SlaveNanos;

			/* 3. Drift
			*/
			DriftSeconds = MasterSeconds - SlaveSeconds;
			DriftNanos = MasterNanos - SlaveNanos;
			if ((DriftSeconds < 0) && (DriftNanos > 0))
			{
				DriftSeconds++;
				DriftNanos -= GSY_NANOS_PER_SECOND;
			}
			else if ((DriftSeconds > 0) && (MasterNanos < 0))
			{
				DriftSeconds--;
				DriftNanos += GSY_NANOS_PER_SECOND;
			}
			if (DriftSeconds != 0)
			{
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_DriftCalc() pChSys=%x Invalid Drift: Seconds=%d Nanos=%d",
							pChSys, DriftSeconds, DriftNanos);
			}
			else
			{
				GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) MasterSs=%d MasterNs=%d SlaveSs=%d SlaveNs=%d DriftSs=%d DriftNs=%d",
								pDrift->SyncId, MasterSeconds, MasterNanos, SlaveSeconds, SlaveNanos, DriftSeconds, DriftNanos);

				/* Calculate Interval in nanoseconds which have to be corrected by one nanosecond.
				*/
				if (DriftNanos < 0)
					DriftNanosU = (LSA_UINT32)(0 - DriftNanos);
				else
					DriftNanosU = (LSA_UINT32)DriftNanos;

				if (DriftNanosU != 0)
				{
					DriftInterval = SlaveInterval / DriftNanosU;

					/* Round
					*/
					if ((SlaveInterval % DriftNanosU) >= (DriftNanosU / 2))
						DriftInterval++;

					/* 100107lrg003: Use Minimum if lower
					*/
					if (DriftInterval < GSY_DRIFT_MIN_INTERVAL)
						DriftInterval = GSY_DRIFT_MIN_INTERVAL;

					/* Set sign
					*/
					if (DriftNanos < 0)
						DriftInterval = (LSA_INT32)(0 - DriftInterval);
				}
				else
                {
					/* 100107lrg003: Use Maximum if greater
					*/
					DriftInterval = GSY_DRIFT_MAX_INTERVAL;
                }

                /* 310707js001: Calculate real drift interval
                */
                DriftInterval = k32gsy_DriftCalcRealInterval(pChSys,pDrift,DriftInterval);

				/* Actual Drift interval overwrites oldest entry in average list
				*/
				Idx = pDrift->AveOldest;
				pDrift->AveActInterval[Idx] = DriftInterval;
				pDrift->AveOldest++;
				if (DriftMaxCount == pDrift->AveOldest)
					pDrift->AveOldest = 0;
				if (DriftMaxCount > pDrift->AveSumCount)
					pDrift->AveSumCount++;

				/* Store Minimum and Maximum
				*/
				if ((pDrift->AveMaxInterval == 0) || (pDrift->AveMaxInterval < DriftInterval))
					pDrift->AveMaxInterval = DriftInterval;
				if ((pDrift->AveMinInterval == 0) || (pDrift->AveMinInterval > DriftInterval))
					pDrift->AveMinInterval = DriftInterval;

				if (DriftMaxCount <= pDrift->AveSumCount)
				{
					/* Build Median of sample, if the list is full
					*/
					NewAveInterval = k32gsy_DriftMedian(pChSys, pDrift);
				}

				GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) DriftInterval=%d NewAveInterval=%d AveInterval=%d AveOldest=%d AveSumCount=%u AveMinInterval=%d AveMaxInterval=%d",
								pDrift->SyncId, DriftInterval, NewAveInterval, pDrift->AveInterval, pDrift->AveOldest, pDrift->AveSumCount,
								pDrift->AveMinInterval, pDrift->AveMaxInterval);

				/* If enough single values have build the average value...
				*/
				if ((DriftMaxCount <= pDrift->AveSumCount)
				&&  (NewAveInterval != 0))
				{
					/* ...set new average value for drift correction interval
					*/
					k32gsy_DriftSet(pChSys, pDrift, pDrift->SyncId, NewAveInterval, GSY_DRIFT_STATE_READY);
				}
			}
		}
	}
	/* New SyncFrame becomes old for next calculation
	 * 180108lrg001: and the index is increased
	*/
	pDrift->OldMasterAddr = pDrift->MasterAddr;
	pDrift->Old[NextIdx] = pDrift->New[NextIdx];
	pDrift->Old[NextIdx].Used = LSA_FALSE;
	pDrift->New[NextIdx].Used = LSA_TRUE;
	pDrift->NextIdx++;
	if (DriftMaxCount <= pDrift->NextIdx)
		pDrift->NextIdx = 0;

	GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DriftCalc(%02x) MACadr=%02x-%02x-%02x-%02x-%02x-%02x AveInterval=%d ActInterval=%d",
					pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1], pDrift->MasterAddr.MacAdr[2],
					                pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4], pDrift->MasterAddr.MacAdr[5],
					pDrift->AveInterval, DriftInterval);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftCalc() Correction=%d CorrectPlus=%d tState=%d",
					pDrift->CorrectInterval, pDrift->CorrectPlus, pDrift->State);
}

/*****************************************************************************/
/* Internal access function: k32gsy_DriftReset()                             */
/* Free drift strukture for usage of another master.                         */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DriftReset(
GSY_CH_K32_PTR  	pChSys,
GSY_DRIFT_PTR		pDrift,
LSA_UINT8			SyncId)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftReset() pChSys=%x pDrift=%x SyncId=%u",
					pChSys, pDrift, SyncId);

	pDrift->Subdomain = pChSys->Slave[SyncId].Subdomain;

	GSY_SYNC_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_DriftReset() pChSys=%x pDrift=%x SyncId=%u SubdomainUUID=0x%08x, 0x%04x...0x%02x",
					  pChSys, pDrift, SyncId,
					  GSY_SWAP32(pDrift->Subdomain.Data1.Dword), GSY_SWAP16(pDrift->Subdomain.Data2.Word), pDrift->Subdomain.Data4[7]);

	k32gsy_DriftSet(pChSys, pDrift, GSY_SYNCID_NONE, 0, GSY_DRIFT_STATE_UNKNOWN);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftReset() SubdomainUUID=0x%08x, 0x%04x...0x%02x",
					GSY_SWAP32(pDrift->Subdomain.Data1.Dword),
					GSY_SWAP16(pDrift->Subdomain.Data2.Word), pDrift->Subdomain.Data4[7]);
}

/*****************************************************************************/
/* Internal function: k32gsy_DriftTimer()                                    */
/* Timer controlled funktion to reset drift on timeout                       */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DriftTimer(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT8			SyncId;
	/* variable 'OldState' / 'OldInterval' set but not used [-Wunused-but-set-variable] */
	GSY_DRIFT_PTR		pDrift = LSA_NULL;
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	GSY_CH_K32_PTR  	pChSys = pTimer->pChSys;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DriftTimer() pChSys=%x pTimer=%x Id=%u",
					pTimer->pChSys, pTimer, pTimer->Id);

	if (pChSys->State == GSY_CHA_STATE_OPEN)
	{
		/* Step through drift table * 190906lrg001
		*/
		for (SyncId = 0; SyncId < 2; SyncId++)
		{
			pDrift = &pChSys->Drift[SyncId];
			if ((pDrift->SyncId != GSY_SYNCID_NONE)
			&&  (GSY_DRIFT_STATE_MASTER != pDrift->State))
			{
				/* FT1204141: Do not reset rate before slave timeout 
				*/
				pDrift->TimeoutMillis += GSY_DRIFT_TIME_MILLIS;
				if (((GSY_SLAVE_STATE_OFF == pChSys->Slave[SyncId].State)				// No slave is running
				  && ((GSY_SYNCID_CLOCK == SyncId) || (pDrift->TimeoutMillis >= GSY_DRIFT_TIME_OUT_1)))
				||  ((GSY_SLAVE_STATE_OFF != pChSys->Slave[SyncId].State)
				  && (pDrift->TimeoutMillis >= pChSys->Slave[SyncId].SyncTimeout)))		// Slave timeout time reached
				{
					if ((pDrift->Act.SeqId == pDrift->OldSeqId)
					&&  (GSY_MACADDR_CMP(pDrift->OldMasterAddr, pDrift->MasterAddr)
					 ||  GSY_MACADDR_IS_NULL(pDrift->MasterAddr)))
					{
						/* 150106lrg001: No new SyncFrame arrived from drift master:
						 * 180108lrg001: Check SeqId and MasterMAC
						 * 210108lrg004: On Timeout: delete Master MAC
						 * Mark drift structure free for new master
						*/
						GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** k32gsy_DriftTimer() pChSys=%x SyncId=0x%02x OldSeqId=%u TimeoutMillis=%u: No new sync frame",
										pChSys, SyncId, pDrift->OldSeqId, pDrift->TimeoutMillis);
						k32gsy_DriftReset(pChSys, pDrift, SyncId);
						pDrift->MasterAddr = NoMacAddr;
					}
					else
					{
						/* Store actual SeqId
						*/
						pDrift->OldSeqId = pDrift->Act.SeqId;
					}
					pDrift->TimeoutMillis = 0;
				}
			}
		}
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DriftTimer()");
}

/*****************************************************************************/
/* Internal function: k32gsy_DelaySet()                                      */
/* Set delay values in PN-IP and indicate to EDDP                            */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelaySet(
GSY_CH_K32_PTR  pChSys,
LSA_UINT16		PortId)
{
	K32_RQB_PTR_TYPE pK32Rqb = pChSys->Port[PortId-1].pK32Rqb;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelaySet() pChSys=%x Port=%u LineDelay=%u",
				pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay);

	/* Set delay in PN-IP
	*/
	K32GSY_DELAY_SET(pChSys->InterfaceId, PortId, pChSys->Port[PortId-1].SyncLineDelay);

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - k32gsy_DelaySet() pChSys=%x Port=%u LineDelay=%u CableDelay=%u",
				pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay, pChSys->Port[PortId-1].CableDelay);

	if (!LSA_HOST_PTR_ARE_EQUAL(pK32Rqb, LSA_NULL))
	{
		/* Send delay indication to EDDP
		*/
		pK32Rqb->Hdr.InterfaceID = pChSys->InterfaceId;
		pK32Rqb->Hdr.Response = K32_RSP_OK;
		pK32Rqb->Params.DelayInd.PortID = PortId;
		pK32Rqb->Params.DelayInd.LineDelay = pChSys->Port[PortId-1].SyncLineDelay;
		pK32Rqb->Params.DelayInd.CableDelay = pChSys->Port[PortId-1].CableDelay;
		K32GSY_CALLBACK(pK32Rqb);
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DelaySet() No pK32Rqb: pChSys=%x Interface=%u Port=%u",
							pChSys, pChSys->InterfaceId, PortId);
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelaySet() pChSys=%x Port=%u CableDelay=%u",
				pChSys, PortId, pChSys->Port[PortId-1].CableDelay);
}

/* js040806 ******************************************************************/
/* Internal function: k32gsy_DelayTempToActual()                             */
/* Copies the temporary delay values of one port to the actual and sets them */
/* if they are valid (DelayValid).                                           */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayTempToActual(
GSY_CH_K32_PTR  pChSys,
LSA_UINT16		PortId)
{
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT32	OldDelay = pChSys->Port[Idx].SyncLineDelay;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayTempToActual() pChSys=%x OldDelay=%d Port=%d",
				pChSys, OldDelay, PortId);

	if (pChSys->Port[Idx].DelayValid)
	{
#ifdef GSY_DELAY_FILTER	//AP01384863: DelayFilter
		if (0 != pChSys->Port[Idx].SyncLineDelayTemp)
		{
			/* 903419: Mittelwertfreie Berechnung der LineDelay
			*/
			LSA_UINT64 Tmp = pChSys->Port[Idx].FilterDelayTemp + (GSY_DELAY_FILTER_FACTOR / 2); 

			pChSys->Port[Idx].SyncLineDelay = (LSA_UINT32)(Tmp / GSY_DELAY_FILTER_FACTOR);
			if (0 != pChSys->Port[Idx].PortDelayTemp)
			{
				if (pChSys->Port[Idx].PortDelayTemp < pChSys->Port[Idx].SyncLineDelay)
				{
					pChSys->Port[Idx].CableDelay = pChSys->Port[Idx].SyncLineDelay - pChSys->Port[Idx].PortDelayTemp;
				}
				else
				{
					pChSys->Port[Idx].CableDelay = 1;
				}
			}
			else
			{
				pChSys->Port[Idx].CableDelay = pChSys->Port[Idx].CableDelayTemp;
			}
		}
		else
		{
			pChSys->Port[Idx].SyncLineDelay = 0;
			pChSys->Port[Idx].CableDelay = 0;
		}
		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE," -- k32gsy_DelayTempToActual() pChSys=%x Port=%u OldDelay=%u TempDelay=%u CableDelay=%u CableDelayTemp=%u PortDelay=%u FilterDelayWeight=%u",
			pChSys, PortId, OldDelay, pChSys->Port[Idx].SyncLineDelayTemp, pChSys->Port[Idx].CableDelay,pChSys->Port[Idx].CableDelayTemp,pChSys->Port[Idx].PortDelayTemp,pChSys->Port[Idx].FilterDelayWeight);
#else
		pChSys->Port[Idx].SyncLineDelay = pChSys->Port[Idx].SyncLineDelayTemp;
		pChSys->Port[Idx].CableDelay = pChSys->Port[Idx].CableDelayTemp;
#endif
		/* 240608lrg001: Now set DelayValid = LSA_FALSE
		*/
		pChSys->Port[Idx].DelayValid = LSA_FALSE;

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - k32gsy_DelayTempToActual() pChSys=%x Port=%u OldDelay=%u LineDelay=%u CableDelay=%u [%x%x%x]",
			pChSys, PortId, OldDelay, pChSys->Port[PortId-1].SyncLineDelay, pChSys->Port[PortId-1].CableDelay,0,0,0);

		/* If the new delay value differs from the old one: set new in PN-IP and EDD
		*/
		if (OldDelay != pChSys->Port[Idx].SyncLineDelay)
		{
			if ((0 == OldDelay) || (0 == pChSys->Port[Idx].SyncLineDelay))
			{
				GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_DelayTempToActual(%u,%u) SeqId=%u: delay state change ->[%u]",
					pChSys->InterfaceId, PortId, pChSys->Port[PortId-1].ActDelay.ReqSeqId, pChSys->Port[PortId-1].SyncLineDelay);
			}
			/* 250609lrg001: allow HW forwarding of ClockSyncFrames to a port with line delay measured
			 * and disable HW forwarding on line delay change to 0.
			 * 251109lrg002: also TimeSyncFrames
			*/
			if (0 == OldDelay)
			{
				if (pChSys->Port[Idx].ClockSyncOk)
				{
					GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_DelayTempToActual(%u,%u) clock topo state change ->[1]",
						pChSys->InterfaceId, PortId);
					K32GSY_FWD_TX_SET_CLOCK(pChSys->InterfaceId, PortId, LSA_TRUE);
				}
				GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_DelayTempToActual(%u,%u)  time topo state change ->[1]",
					pChSys->InterfaceId, PortId);
				K32GSY_FWD_TX_SET_TIME(pChSys->InterfaceId, PortId, LSA_TRUE);
			}
			else if (0 == pChSys->Port[Idx].SyncLineDelay)
			{
				GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- k32gsy_DelayTempToActual(%u,%u) clock/time topo state change ->[0]",
						pChSys->InterfaceId, PortId);
				K32GSY_FWD_TX_SET_CLOCK(pChSys->InterfaceId, PortId, LSA_FALSE);
				K32GSY_FWD_TX_SET_TIME(pChSys->InterfaceId, PortId, LSA_FALSE);
			}

			/* Set new delay
			*/
			k32gsy_DelaySet(pChSys, PortId);
		}
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayTempToActual() pChSys=%x",
				pChSys);
}

/*****************************************************************************/
/* Internal function: k32gsy_RcfCorrection()                                 */
/* Calculate the correction value for the given RCF interval                 */
/*****************************************************************************/
K32_STATIC LSA_INT32 K32_LOCAL_FCT_ATTR k32gsy_RcfCorrection(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT32				Interval,
LSA_UINT16				PortIdx)
{
	LSA_INT32		RetVal = 0;
	LSA_UINT32		Rest = 0, CorrectU = 0;
	GSY_RCF_LOCAL *	pRcf = &pChSys->Port[PortIdx].ActDelay.RcfLocal;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_RcfCorrection() pChSys=%x Port=%u Interval=%u",
					pChSys, (PortIdx+1), Interval);

	if ((pRcf->Interval != 0) && (Interval != 0))
	{
		if (pRcf->Interval >= 0)
			CorrectU = pRcf->Interval;
		else
			CorrectU = -pRcf->Interval;

		RetVal = Interval / CorrectU;
		Rest   = Interval % CorrectU;
		if (pRcf->Interval >= 0)
		{
			/* Positive correction with round up
			*/
			if (Rest >= (CorrectU / 2))
				RetVal++;
		}
		else
		{
			/* Negative correction
			*/
			RetVal = -RetVal;
			if (Rest >= (CorrectU / 2))
				RetVal--;
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_RcfCorrection() CorrectU=%d Rcf.Interval=%d RetVal=%d",
					CorrectU, pRcf->Interval, RetVal);

	return(RetVal);
}

/*****************************************************************************/
/* Internal function: k32gsy_RcfCalc()                                       */
/* Calculate RCF interval for a port from 2 sequent delay measurements       */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_RcfCalc(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				SeqId,
LSA_UINT16				PortIdx)
{
	LSA_UINT32		T1 = 0, T2 = 0, TdiffU = 0, Interval = 0;
	LSA_INT			Tdiff = 0;
	GSY_RCF_LOCAL *	pRcf = &pChSys->Port[PortIdx].ActDelay.RcfLocal;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_RcfCalc() pChSys=%x Port=%u SeqId=%u",
					pChSys, (PortIdx+1), SeqId);

	if (GSY_DRIFT_STATE_UNKNOWN == pRcf->State)
	{
		pRcf->State = GSY_DRIFT_STATE_WAIT;
	}
	else if ((pRcf->SeqAct == SeqId) && ((pRcf->SeqOld+1) == SeqId))
	{
		/* Find actual local RCF interval
		*/
		T1 = pRcf->T1Act - pRcf->T1Old;
		T2 = pRcf->T2Act - pRcf->T2Old;
		if (T1 == T2)
		{
			/* No correction
			*/
			pRcf->Interval = 0;
		}
		else
		{
			/* Correction by +/- 1ns per RCF-Interval depending on its sign
			*/
			Tdiff = (LSA_INT)T2 - (LSA_INT)T1;
			if (Tdiff < 0)
				TdiffU = T1 - T2;
			else
				TdiffU = Tdiff;
			Interval = T1 / TdiffU;

			/* Round
			*/
			if ((T1 % TdiffU) >= (TdiffU / 2))
				Interval++;

			if (Interval < GSY_DRIFT_MIN_INTERVAL)
			{
				/* Use Minimum as replacement value
				*/
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** k32gsy_RcfCalc() Port=%u SeqId=%u Interval=%u: using GSY_DRIFT_MIN_INTERVAL",
						(PortIdx+1), SeqId, Interval);
				Interval = GSY_DRIFT_MIN_INTERVAL;
			}

			/* Restore sign
			*/
			if (Tdiff < 0)
				pRcf->Interval = (LSA_INT32)(0 - Interval);
			else
				pRcf->Interval = (LSA_INT32)Interval;

			GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE," -- k32gsy_RcfCalc(%u) State=%u SeqId=%u T1=%u T2=%u TdiffU=%u Tdiff=%d Interval=%u Rcf.Interval=%d [%x%x]",
						(PortIdx+1), pRcf->State, SeqId, T1, T2, TdiffU, Tdiff, Interval, pRcf->Interval, 0, 0);
		}
		pRcf->State = GSY_DRIFT_STATE_READY;
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** k32gsy_RcfCalc() Cannot calc RCF interval for Port=%u: State=%u SeqId=%u",
				(PortIdx+1), pRcf->State, SeqId);
	}

	GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "--- k32gsy_RcfCalc(%u) State=%u SeqId=%u SeqOld/Act=%u/%u T1Old/Act=%u/%u  T2Old/Act=%u/%u Interval=%d Tdiff=%d",
				(PortIdx+1), pRcf->State, SeqId, pRcf->SeqOld, pRcf->SeqAct, pRcf->T1Old, pRcf->T1Act, pRcf->T2Old, pRcf->T2Act, pRcf->Interval, Tdiff);

	/* Store acual values to old values for the next calculation
	*/
	pRcf->SeqOld = pRcf->SeqAct;
	pRcf->T1Old = pRcf->T1Act;
	pRcf->T2Old = pRcf->T2Act;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_RcfCalc() Port=%u State=%u Interval=%d",
					(PortIdx+1), pRcf->State, pRcf->Interval);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayCalc()                                     */
/* Calculate line delay and cable delay for a port                           */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayCalc(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				PortId,
LSA_UINT16				SeqId)
{
	LSA_UINT32	RspDelayNanos;
	LSA_UINT32	ReqDelayNanos;
	LSA_INT32	DriftNanos = 0;
	LSA_INT32	CableDelay = 0;			//AP01262791: was UINT32
	LSA_INT32	LineDelay = 0;
	LSA_UINT32	PortDelay = 0;
	LSA_INT32	Sum = 0;				//AP01262791: was UINT32
	LSA_UINT16	Cnt = 0, New = 0;
	LSA_UINT16	Idx = PortId - 1;
	LSA_BOOL	DoCalc = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayCalc() pChSys=%x ReqTicks=%u RxTxNanos=%u",
					pChSys, pChSys->Port[Idx].ActDelay.ReqDelay, pChSys->RxTxNanos);

	/* Responder bridge delay is in nanoseconds
	*/
	RspDelayNanos = pChSys->Port[Idx].ActDelay.RspDelayNS;

	/* 030407lrg001: Calculate local RCF interval
	 * 270907lrg003: GSY_DRIFT_STATE_NONE removed
	*/
	k32gsy_RcfCalc(pChSys, SeqId, Idx);

	if (GSY_DRIFT_STATE_WAIT != pChSys->Port[Idx].ActDelay.RcfLocal.State)
	{
		/* 050907lrg002: Calculate delay only if local RCF interval is known
		 * 270907lrg001
		*/
		DriftNanos = k32gsy_RcfCorrection(pChSys, RspDelayNanos, Idx);
	}
	else
		DoCalc = LSA_FALSE;

	pChSys->Port[Idx].ActDelay.RcfLocal.Count++;

	if (DoCalc)
	{
		/* Normalize responder bridge delay locally by reverse drift correction
		*/
		if ((RspDelayNanos != 0)			//180408lrg001
		&&  (RspDelayNanos != GSY_PTCP_DELAY_NS_INVALID))	//140710lrg001
		{
			RspDelayNanos -= DriftNanos;
		}

		/* Do not correct the requestor time by drift
		*/
		ReqDelayNanos =	pChSys->Port[Idx].ActDelay.ReqDelay * pChSys->RxTxNanos;

		/* Build difference of requestor and responder time and sum of the port delays
		 * and calculate cable delay
		 * AP01262791: only set CableDelayTemp to 1ns if average value of CableDelay is < 1ns
		*/
		CableDelay = (LSA_INT32)(ReqDelayNanos - RspDelayNanos);
		PortDelay  = pChSys->Port[Idx].TxDelay
				   + pChSys->Port[Idx].ActDelay.RspRxPortDelay
				   + pChSys->Port[Idx].ActDelay.RspTxPortDelay
				   + pChSys->Port[Idx].RxDelay;

		CableDelay = (CableDelay - (LSA_INT32)PortDelay) / 2;

		GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - k32gsy_DelayCalc(%u) SeqId=%u Cable=%d Line=%d T1=%u T4=%u T2=%u T3=%u Req=%u Rsp=%u Drift=%d",
			PortId, SeqId, CableDelay, CableDelay+(LSA_INT32)pChSys->Port[Idx].ActDelay.RspTxPortDelay+(LSA_INT32)pChSys->Port[Idx].RxDelay,
			pChSys->Port[Idx].TxDelay, pChSys->Port[Idx].RxDelay, pChSys->Port[Idx].ActDelay.RspRxPortDelay, pChSys->Port[Idx].ActDelay.RspTxPortDelay,
			ReqDelayNanos, RspDelayNanos, DriftNanos);

		if ((RspDelayNanos == 0)			//AP01527161
		||  ((ReqDelayNanos > RspDelayNanos)
		 &&  (RspDelayNanos != GSY_PTCP_DELAY_NS_INVALID)	//140710lrg001
		 &&  ((GSY_RSPDELAY_MAX == 0) || (GSY_RSPDELAY_MAX >= RspDelayNanos))))
		{
			/* If a cable delay could be calculated
			 * 190506lrg001: and the responder bridge delay is not too great:
			 * Build average value and save it in port data
			 * AP01527161: no tolerance check

			if ((pChSys->Port[Idx].CableDelay == 0)
			||  (pChSys->Port[Idx].SumCount < GSY_DELAY_IGNORE_DIST_CNT)
			||  ((GSY_DELAY_MAX_DIST > pChSys->Port[Idx].CableDelay)
			 &&  (CableDelay <= (LSA_INT32)pChSys->Port[Idx].CableDelay + GSY_DELAY_MAX_DIST))
			||  ((CableDelay >= (LSA_INT32)pChSys->Port[Idx].CableDelay - GSY_DELAY_MAX_DIST) 
			 &&  (CableDelay <= (LSA_INT32)pChSys->Port[Idx].CableDelay + GSY_DELAY_MAX_DIST)))
			*/
			{
				/* If the history list is full: restart building list
				*/
				if (GSY_DELAY_MAX_COUNT == pChSys->Port[Idx].SumCount)
				{
					pChSys->Port[Idx].SumCount = 0;
				}
				if (0 == pChSys->Port[Idx].SumCount)
				{
					/* On (re)start the hole list is initialised with the actual cable delay value
					*/
					for (Cnt = 0; Cnt < GSY_DELAY_MAX_COUNT; Cnt++)
					{
						pChSys->Port[Idx].OldDelay[Cnt] = CableDelay;
					}
					pChSys->Port[Idx].SumCount = 1;
					pChSys->Port[Idx].Oldest = 0;
				}
				else
				{
					/* Overwrite the oldest value of the history list with the actual one
					*/
					New = pChSys->Port[Idx].Oldest;
					pChSys->Port[Idx].OldDelay[New] = CableDelay;
					pChSys->Port[Idx].SumCount++;
					pChSys->Port[Idx].Oldest++;
					if (pChSys->Port[Idx].Oldest >= GSY_DELAY_MAX_COUNT)
						pChSys->Port[Idx].Oldest = 0;
				}
				/* Build the maximum sum of entries without overflow
				 * and divide it by the count of the entries
				*/
				Cnt = 0;
				while (Cnt < GSY_DELAY_MAX_COUNT)
				{
					if (((pChSys->Port[Idx].OldDelay[New] > 0) && ((GSY_MAX_NANOS_PER_INT32 - Sum) < pChSys->Port[Idx].OldDelay[New]))
					||  ((pChSys->Port[Idx].OldDelay[New] < 0) && ((GSY_MIN_NANOS_PER_INT32 - Sum) > pChSys->Port[Idx].OldDelay[New])))
						break;
					else
					{
						Sum += pChSys->Port[Idx].OldDelay[New];
						Cnt++;
						if (0 == New)
							New = GSY_DELAY_MAX_COUNT - 1;
						else
							New--;
					}
				}

				/* js040806: Save delay values in temporary variables.
				 * Setting these values in FWD and EDD is done later by gsy_DelayTempToActual()
				 * AP01262791: only set Cable delay to 1ns if average value is < 1ns and not < -50ns
				*/
				CableDelay = Sum / Cnt;

				if (RspDelayNanos == 0) //AP01527161: Set cable delay to 0 
				{
					pChSys->Port[Idx].CableDelayTemp = 0;
				}
				else if (CableDelay > 0)
				{
					pChSys->Port[Idx].CableDelayTemp = (LSA_UINT32)CableDelay;
				}
				else
				{
					pChSys->Port[Idx].CableDelayTemp = 1;
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,  "*** k32gsy_DelayCalc() Port=%u SeqId=%u: setting CableDelay=%d to 1",
							PortId, SeqId, CableDelay);
				}

				LineDelay = CableDelay \
							+ pChSys->Port[Idx].ActDelay.RspTxPortDelay \
							+ pChSys->Port[Idx].RxDelay;

				if (LineDelay < 1)
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,  "*** k32gsy_DelayCalc() Port=%u SeqId=%u: setting LineDelay=%d to 1",
							PortId, SeqId, LineDelay);
					LineDelay = 1;
				}

#ifdef GSY_DELAY_FILTER	//AP01384863: DelayFilter

				if (RspDelayNanos != 0)			//181212lrg001: get zero cable delay for OHA from CableDelayTemp in gsy_DelayTempToActual()
				{
					pChSys->Port[Idx].PortDelayTemp = pChSys->Port[Idx].ActDelay.RspTxPortDelay \
													+ pChSys->Port[Idx].RxDelay;
				}
				else
				{
					pChSys->Port[Idx].PortDelayTemp = 0;
				}

				if (0 == pChSys->Port[Idx].SyncLineDelayTemp)
				{
					/* Filter init
					*/
					pChSys->Port[Idx].FilterDelayTemp = LineDelay;
					pChSys->Port[Idx].FilterDelayTemp *= GSY_DELAY_FILTER_FACTOR;
					pChSys->Port[Idx].FilterDelayWeight = GSY_DELAY_FILTER_MIN;
				}
				else
				{
					/* Filter already active: use and modify it
					*/
					LSA_UINT64 help = LineDelay;
					help *= GSY_DELAY_FILTER_FACTOR;
					pChSys->Port[Idx].FilterDelayTemp *= (pChSys->Port[Idx].FilterDelayWeight - 1);
					pChSys->Port[Idx].FilterDelayTemp += help;
					pChSys->Port[Idx].FilterDelayTemp /= pChSys->Port[Idx].FilterDelayWeight;
					if (pChSys->Port[Idx].FilterDelayWeight + GSY_DELAY_FILTER_INC <= GSY_DELAY_FILTER_MAX)
					{
						pChSys->Port[Idx].FilterDelayWeight += GSY_DELAY_FILTER_INC;
					}
				}
#endif
				pChSys->Port[Idx].SyncLineDelayTemp = (LSA_UINT32)LineDelay;

				/* Store Minimum and Maximum in temporary variable
				*/
				if ((pChSys->Port[Idx].MaxDelay == 0) || (pChSys->Port[Idx].MaxDelay < pChSys->Port[Idx].CableDelayTemp))
				{
					pChSys->Port[Idx].MaxDelayTemp = pChSys->Port[Idx].CableDelayTemp;
				}
				if ((pChSys->Port[Idx].MinDelay == 0) || (pChSys->Port[Idx].MinDelay > pChSys->Port[Idx].CableDelayTemp))
				{
					pChSys->Port[Idx].MinDelayTemp = pChSys->Port[Idx].CableDelayTemp;
				}

				pChSys->Port[Idx].DelayValid = LSA_TRUE;  /* delay within temp valid */
			}
			/* AP01527161: no tolerance check
			else
			{
				// Delay too large or too small
				//
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,  "*** k32gsy_DelayCalc() Invalid delay: Port=%u SeqId=%u CableDelay=%d",
							PortId, SeqId, CableDelay);
			}
			*/
		}
		else
		{
			/* 190506lrg001: Response delay to large or invalid
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** k32gsy_DelayCalc() Invalid response delay: Port=%u SeqId=%u RespDelay=%u",
					PortId, SeqId, RspDelayNanos);
		}
		/* AP01527161: already done above
		if (0 == RspDelayNanos)			//180408lrg001
		{
			// Signal for OHA not to compare local und remote LineDelay at this port
			//
			pChSys->Port[Idx].CableDelayTemp = 0;
		}
		*/
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayCalc() SeqId=%u AvCableDelay=%u AvLineDelay=%u",
					SeqId, pChSys->Port[Idx].CableDelayTemp, pChSys->Port[Idx].SyncLineDelayTemp);

	LSA_UNUSED_ARG(SeqId);

}

/*****************************************************************************/
/* Internal function: k32gsy_DelayReqFill()                                  */
/* Fill values into delay request frame                                      */
/*****************************************************************************/
K32_STATIC LSA_UINT16 K32_LOCAL_FCT_ATTR k32gsy_DelayReqFill(
GSY_CH_K32_PTR    	pChSys,
LSA_UINT16		  	PortId,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	LSA_UINT16			Idx = PortId - 1;
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_DELAY};

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqFill() pChSys=%x pPdu=%x Port=%d",
					pChSys, pPdu, PortId);

	K32GSY_TXMEM_ZERO(pPdu, GSY_DELAY_REQ_PDU_SIZE);

	pPdu->PduHdr.SrcMacAdr = pChSys->Port[Idx].MACAddr;
	pPdu->PduHdr.DstMacAdr = DstAddr;

#ifdef GSY_USE_VLAN
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPdu->PduHdr.VlanTag = VlanTag;
	}
#endif
	pPdu->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
	pPdu->PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_REQ);

	pPdu->PtcpData.DelayReq.DelayParam.RequestSourceAddress = pChSys->Port[Idx].MACAddr;
	pPdu->PtcpData.DelayReq.DelayParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_DELAYPARAM_TYPELEN);
	pPdu->PtcpData.DelayReq.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayReqFill() pChSys=%x pPdu=%x",
					pChSys, pPdu);
	return(0);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayRspSend()                                  */
/* Receive delay request frame and send response in the same frame buffer    */
/*****************************************************************************/
K32_STATIC LSA_BOOL K32_LOCAL_FCT_ATTR k32gsy_DelayRspSend(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_RX_RCV_PTR	pPduRcv,
LSA_UINT16				FrameLength,
LSA_UINT32				Timestamp,
LSA_BOOL				VlanTag)
{
	GSY_LOWER_RX_PDU_PTR	pPdu;
	LSA_UINT64				Nanos;
	LSA_UINT16				RecvLength = FrameLength;
	LSA_UINT16				SendLength = sizeof(GSY_DELAY_RSP_PDU);
	LSA_BOOL				RspNotSent = LSA_TRUE;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayRspSend() pChSys=%x Port=%d TimeStamp=%u",
					pChSys, PortId, Timestamp);

	/* If response for delay request is activated...
	*/
	if (pChSys->Port[PortId-1].DelayRspActive)
	{
		/* ... exchange Source MAC and FrameId to
		 * send response without FU in received frame buffer
		*/
		if (VlanTag)
		{
			pPduRcv->PduVlan.PduHdr.SrcMacAdr = pChSys->Port[PortId-1].MACAddr;
			pPduRcv->PduVlan.PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_RSP);
			pPdu = (GSY_LOWER_RX_PDU_PTR)&pPduRcv->PduNoVlan.PtcpData;
			RecvLength -= GSY_PDU_HDR_SIZE_VLAN;
			SendLength += GSY_PDU_HDR_SIZE_VLAN;
		}
		else
		{
			pPduRcv->PduNoVlan.PduHdr.SrcMacAdr = pChSys->Port[PortId-1].MACAddr;
			pPduRcv->PduNoVlan.PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_RSP);
			pPdu = (GSY_LOWER_RX_PDU_PTR)&pPduRcv->PduNoVlan.PtcpData;
			RecvLength -= GSY_PDU_HDR_SIZE_NO_VLAN;
			SendLength += GSY_PDU_HDR_SIZE_NO_VLAN;
		}
		/* If the received delay request is a new one (without Subdomain) and not too short...
		*/
		if ((GSY_TLV_DELAYPARAM_TYPELEN == GSY_SWAP16(pPdu->DelayReq.DelayParam.TypeLen.Word))
		&&  (GSY_DELAY_REQ_RX_SIZE <= RecvLength))
		{
			/* Build delay response frame.
			 * Send back delay parameters of the requests and local port delays
			*/
			pPdu->DelayRsp.PortParam.T2PortRxDelay.Dword = GSY_SWAP32(pChSys->Port[PortId-1].RxDelay);
			pPdu->DelayRsp.PortParam.T3PortTxDelay.Dword = GSY_SWAP32(pChSys->Port[PortId-1].TxDelay);
			pPdu->DelayRsp.PortParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_PORTPARAM_TYPELEN);

			/* 030407lrg001: Calculate T2TimeStamp in nanoseconds with 32Bit WrapAround
			 * from local receive timestamp in ticks and put it into GSY_TLV_PORTTIME
			 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
			*/
			Nanos = (LSA_UINT64)Timestamp * (LSA_UINT64)pChSys->RxTxNanos;
			Nanos = Nanos & 0xFFFFFFFF;	// was: % GSY_NANOS_PER_DWORD;

			pPdu->DelayRsp.PtcpHdr.DelayNS.Dword = 0;
			pPdu->DelayRsp.PtcpHdr.Delay10ns.Dword = 0;
			pPdu->DelayRsp.PortTime.T2TimeStamp.Dword = GSY_SWAP32((LSA_UINT32)Nanos);
			pPdu->DelayRsp.PortTime.TypeLen.Word = GSY_SWAP16(GSY_TLV_PORTTIME_TYPELEN);
			pPdu->DelayRsp.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

			/* Send delay response frame without timestamp
			*/
			K32GSY_SEND_FRAME((K32GSY_FRAME_PTR)pPduRcv, SendLength, K32_USETYPE_DELAY,
				pChSys->InterfaceId, PortId, LSA_FALSE);
			RspNotSent = LSA_FALSE;
		}
		else
		{
			/* ...Trace and ignore error
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_DelayRspSend() Invalid delay.req PDU SeqId=%u: DelayParam.TypeLen=0x%04x Length=%d",
				GSY_SWAP16(pPdu->DelayReq.PtcpHdr.SequenceID.Word), GSY_SWAP16(pPdu->DelayReq.DelayParam.TypeLen.Word), RecvLength);
		}
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayRspSend() pPduRcv=%x FrameLength=%u VlanTag=%u",
					pPduRcv, FrameLength, VlanTag);

	return(RspNotSent);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayRspInd()                                   */
/* Receive a Delay response frame                                            */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayRspInd(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_RX_PDU_PTR	pPduRx,
LSA_UINT16				Length,
LSA_UINT32				TimeStamp,
LSA_BOOL				RspWithFu)
{
	LSA_UINT16	SeqId;
	LSA_UINT16	Idx = PortId - 1;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayRspInd() pChSys=%x Port=%d TimeStamp=%u",
					pChSys, PortId, TimeStamp);

	/* If the received delay response is a new one (without Subdomain) and is not too short...
	*/
	if  (((GSY_DELAY_RSP_RX_SIZE - GSY_TLV_PORTTIME_SIZE) <= Length)
	&&   (GSY_TLV_DELAYPARAM_TYPELEN == GSY_SWAP16(pPduRx->DelayRsp.DelayParam.TypeLen.Word)))
	{
		/* Check elements wich has been sent with the request and have to be echoed in the response frame
		*/
		SeqId = GSY_SWAP16(pPduRx->DelayRsp.PtcpHdr.SequenceID.Word);

		if ((SeqId == pChSys->Port[Idx].ActDelay.ReqSeqId)
		&&  GSY_MACADDR_CMP(pPduRx->DelayRsp.DelayParam.RequestSourceAddress, pChSys->Port[Idx].MACAddr))
		{

			/* If there was as yet no response for the actual request...
			 * and we are not in the wait phase (timeout occured)
			 * 181010lrg002: and delay measurement is active
			*/
			if (!pChSys->Port[Idx].ResponseArrived
			&&  !(pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_TIME)
			&&  pChSys->Port[Idx].DelayReqActive)
			{

				pChSys->Port[Idx].ResponseArrived = LSA_TRUE;/* js040806 */

				/* Valid Response: change delay state
				*/
				pChSys->Port[Idx].ActDelay.State |= GSY_DELAY_STATE_RSP;

				if (!RspWithFu)
				{
					/* No delay FollowUp follows:
					 * change delay state and store responder bridge delay
					*/
					pChSys->Port[Idx].ActDelay.State |= GSY_DELAY_STATE_FU;
					pChSys->Port[Idx].ActDelay.RspDelayNS = GSY_SWAP32(pPduRx->DelayRsp.PtcpHdr.DelayNS.Dword);
				}
				/* Store parameters for calculation of local RCF
				*/
				pChSys->Port[Idx].ActDelay.RcfLocal.SeqAct = SeqId;
				pChSys->Port[Idx].ActDelay.RcfLocal.T2Act = GSY_SWAP32(pPduRx->DelayRsp.PortTime.T2TimeStamp.Dword);
				pChSys->Port[Idx].ActDelay.RspDrift = LSA_FALSE;

				/* Store responder port delays
				*/
				pChSys->Port[Idx].ActDelay.RspRxPortDelay = GSY_SWAP32(pPduRx->DelayRsp.PortParam.T2PortRxDelay.Dword);
				pChSys->Port[Idx].ActDelay.RspTxPortDelay = GSY_SWAP32(pPduRx->DelayRsp.PortParam.T3PortTxDelay.Dword);

				if (pChSys->Port[Idx].ActDelay.State & GSY_DELAY_STATE_CNF)
				{
					/* If send timestamp is already present:
					 * calculate requestor time of this delay measurement
					*/
					pChSys->Port[Idx].ActDelay.ReqDelay = TimeStamp - pChSys->Port[Idx].ActDelay.ReqDelay;
				}
				else
				{
					/* Only store receive timestamp of the response.
					 * requestor time will be calculated at request send confirmation
					*/
					pChSys->Port[Idx].ActDelay.ReqDelay = TimeStamp;
				}

				if (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_END)
				{
					/* Calculate LineDElay and CableDelay
					*/
					k32gsy_DelayCalc(pChSys, PortId, SeqId);
				}
			}
			else
	 		{
				if ( pChSys->Port[Idx].ResponseArrived )
 				{
					/* js040806:
					 * muliple Response
					*/
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** k32gsy_DelayRspInd() Multiple Delay.rsp: Port=%u SeqId=%u DelayState=0x%x",
						(Idx+1), SeqId, pChSys->Port[Idx].ActDelay.State);

					/* set state to ERROR if not in TIME state. this will prevent further folluphandling
					 * 040608lrg001: set delay to zero on multiple Response
					if ( pChSys->Port[Idx].ActDelay.State != GSY_DELAY_STATE_TIME )
					*/
					pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_ERR;
					pChSys->Port[Idx].LastError = GSY_RSP_ERR_PROTOCOL;
					pChSys->Port[Idx].ErrorCount = GSY_DELAY_NUM_ERROR;
				}
				else
				{
					/* First response received after timeout (not multiple!)
					 * -> trace and ignore
					*/
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** k32gsy_DelayRspInd() Delay.rsp too late. ignored: Port=%u SeqId=%d State=0x%x",
								PortId, SeqId, pChSys->Port[Idx].ActDelay.State);
				}
	 		}
		}
		else
		{
			/* ...trace error and ignore
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayRspInd() Port=%u Invalid response parameter: ReqSeqId=%u SeqID=%u",
						Idx+1, pChSys->Port[Idx].ActDelay.ReqSeqId, SeqId);
 		}
	}
	else
	{
		/* ...trace error and ignore
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayRspInd() Port=%u SeqId=%u Invalid DelayRsp PDU: Length=%u",
					PortId, GSY_SWAP16(pPduRx->DelayRsp.PtcpHdr.SequenceID.Word), Length);
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayRspInd() pChSys=%x pPduRx=%x Length=%d",
					pChSys, pPduRx, Length);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayFuInd()                                    */
/* Receive a Delay FollowUp frame                                            */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayFuInd(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_RX_PDU_PTR	pPduRx,
LSA_UINT16				Length)
{
	LSA_UINT16	SeqId;
	LSA_UINT16	Idx = PortId - 1;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayFuInd() pChSys=%x pPduRx=%x Port=%d",
					pChSys, pPduRx, PortId);

	/* If the FollowUp frame is not too short
	 * and the Delay response has been received
	 * and no FU has been received...
	*/
	if  ((GSY_DELAY_FU_RX_SIZE <= Length)
	&&   (pChSys->Port[Idx].ActDelay.State & GSY_DELAY_STATE_RSP)
	&&   (!(pChSys->Port[Idx].ActDelay.State & GSY_DELAY_STATE_FU)))
	{
		/* If this FollowUp matches Request and Response...
		*/
		SeqId = GSY_SWAP16(pPduRx->DelayFu.PtcpHdr.SequenceID.Word);

		if ((SeqId == pChSys->Port[Idx].ActDelay.ReqSeqId)
		&& 	GSY_MACADDR_CMP(pPduRx->DelayFu.DelayParam.RequestSourceAddress, pChSys->Port[Idx].MACAddr))
		{

			/* ...change delay state and store responder bridge delay
			*/
			pChSys->Port[Idx].ActDelay.State |= GSY_DELAY_STATE_FU;
			pChSys->Port[Idx].ActDelay.RspDelayNS = GSY_SWAP32(pPduRx->DelayFu.PtcpHdr.DelayNS.Dword);

			if (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_END)
			{
				/* If send confirmation already occured:
				 * calculate LineDelay and CableDelay
				*/
				k32gsy_DelayCalc(pChSys, PortId, SeqId);
			}
		}
		else
		{
			/* ...Trace error and ignore it
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayFuInd() Invalid SeqId: Port=%u ReqSeqId=%u SeqId=%u",
						PortId, pChSys->Port[Idx].ActDelay.ReqSeqId, SeqId);
		}
	}
	else
	{
		/* ...Trace error and ignore it 220910lrg001: report ReqSeqId
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayFuInd() Invalid PDU length or state: ReqSeqId=%u Length=%d Port/DelayState=0x%08x",
					pChSys->Port[Idx].ActDelay.ReqSeqId, Length, (((LSA_UINT32)(Idx+1) << 16) + pChSys->Port[Idx].ActDelay.State));
	}


	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayFuInd() pChSys=%x pPduRx=%x Length=%d",
					pChSys, pPduRx, Length);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayReqCnf()                                   */
/* A Delay Request frame has been sent.                                      */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayReqCnf(
GSY_CH_K32_PTR  		pChSys,
LSA_UINT16				Response,
LSA_UINT16				PortId,
GSY_LOWER_TX_PDU_PTR	pPduTx,
LSA_UINT32				TimeStamp)
{
	LSA_UINT64		Nanos;
	LSA_UINT16	 	SeqId;
	LSA_UINT16		Idx = PortId - 1;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqCnf() pChSys=%x Port=%d TimeStamp=%u",
					pChSys, PortId, TimeStamp);

	SeqId = GSY_SWAP16(pPduTx->PtcpData.DelayReq.PtcpHdr.SequenceID.Word);
	if (Response == K32_TXRESULT_OK)
	{
		if (pChSys->Port[Idx].ActDelay.State != GSY_DELAY_STATE_TIME)
		{
			/* Confirmation in time: Update delay state for port and store send timestamp
			*/
			pChSys->Port[Idx].ActDelay.State |= GSY_DELAY_STATE_CNF;

			/* 030407lrg001: Calculate time in nanoseconds with 32Bit WrapAround
			 * from local send timestamp and store time and SequenceId
			 * 270907lrg003: GSY_DRIFT_STATE_NONE removed
			 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
			*/
			Nanos = (LSA_UINT64)TimeStamp * (LSA_UINT64)pChSys->RxTxNanos;
			Nanos = Nanos & 0xFFFFFFFF;	// was: % GSY_NANOS_PER_DWORD;

			pChSys->Port[Idx].ActDelay.RcfLocal.T1Act = (LSA_UINT32)Nanos;
			pChSys->Port[Idx].ActDelay.RcfLocal.SeqAct = SeqId;

			if (pChSys->Port[Idx].ActDelay.State & GSY_DELAY_STATE_RSP)
			{
				/* If the response is already received:
				 * calculate local requestor time of this measurement
				*/
				pChSys->Port[Idx].ActDelay.ReqDelay = pChSys->Port[Idx].ActDelay.ReqDelay - TimeStamp;
			}
			else
			{
				/* Store only request send time.
				 * The Delay will be calculated at response indication.
				*/
				pChSys->Port[Idx].ActDelay.ReqDelay = TimeStamp;
			}

			if (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_END)
			{
				/* Measurement complete: calculate LineDelay and CableDelay
				*/
				k32gsy_DelayCalc(pChSys, PortId, SeqId);
			}
		}
		else
		{
			/* js040806: if we are in the wait phase: confirmation is too late - ignore it
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayReqCnf() DelayReq.cnf too late: pChSys=%x Port=%u SeqId=%u",
						pChSys, PortId, SeqId);
		}
	}
	else
	{
		/* Store request send error
		*/
		pChSys->Port[Idx].LastError = Response;
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** k32gsy_DelayReqCnf() Port=%u SeqId=%u: Send response=0x%x",
			PortId, SeqId, Response);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqCnf() pChSys=%x",pChSys);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayReqSend()                                  */
/* Send a delay request frame to the given port.                             */
/*****************************************************************************/
K32_STATIC LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayReqSend(
GSY_CH_K32_PTR  pChSys,
LSA_UINT16		PortId)
{
	GSY_LOWER_TX_PDU_PTR	pPdu;
	LSA_UINT16			Idx = PortId - 1;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqSend() pChSys=%x Port=%d",
					pChSys, PortId);

	/*  050907lrg002: no response received yet
	*/
	pChSys->Port[Idx].ResponseArrived = LSA_FALSE;

	/* 130607lrg001: if the previous request send RQB is confirmed
	*/
	if ((pChSys->Port[Idx].ActDelay.State & GSY_DELAY_STATE_CNF)
	|| (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_INI)
	|| (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_TIME)
	|| (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_ERR))
	{
		/* If there is a send frame buffer...
		*/
		if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[Idx].pDelayReqPdu, LSA_NULL))
		{
			/* Mark frame buffer as sending by setting ptr to NULL
			*/
			pPdu = pChSys->Port[Idx].pDelayReqPdu;
			pChSys->Port[Idx].pDelayReqPdu = LSA_NULL;

			/* Increment SequenceID to send
			*/
			pChSys->Port[Idx].DelayReqSeqId++;
			pChSys->Port[Idx].ActDelay.ReqSeqId = pChSys->Port[Idx].DelayReqSeqId;
			pPdu->PtcpData.DelayReq.PtcpHdr.SequenceID.Word = GSY_SWAP16(pChSys->Port[Idx].DelayReqSeqId);

			pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_REQ;

			GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- k32gsy_DelayReqSend() pChSys=%x Port=%u Seq=%u",
				pChSys, PortId, pChSys->Port[Idx].ActDelay.ReqSeqId);

			/* Send Delay Request with Timestamp
			*/
			K32GSY_SEND_FRAME((K32GSY_FRAME_PTR)pPdu, sizeof(K32_PTCP_DELAY_REQ_FRAME), K32_USETYPE_DELAY,
				pChSys->InterfaceId, PortId, LSA_TRUE);
		}
		else
		{
			/* Send frame buffer not present
			*/
			pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_ERR;
			pChSys->Port[Idx].LastError = GSY_RSP_ERR_RESOURCE;
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayReqSend() No send frame buffer: pChSys=%x Port=%d ErrorCount=%d",
						pChSys, Idx+1, pChSys->Port[Idx].ErrorCount);
		}
	}
	else
	{
		/* 130607lrg001: the previous request send RQB is not yet confirmed
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** k32gsy_DelayReqSend() DelayReq not confirmed: Port=%u State=0x%02x SeqId=%u",
					PortId, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].ActDelay.ReqSeqId);
	}

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayReqSend() pChSys=%x DelayState=0x%x",
					pChSys, pChSys->Port[Idx].ActDelay.State);
}

/*****************************************************************************/
/* Internal access function: k32gsy_DelayTimer()                             */
/* Timer function for delay measurement                                      */
/*****************************************************************************/
LSA_VOID K32_LOCAL_FCT_ATTR k32gsy_DelayTimer(
GSY_TIMER_PTR	pTimer)
{
	GSY_CH_K32_PTR  pChSys = pTimer->pChSys;
	LSA_UINT8	State;
	LSA_UINT16	Time = 0;
	LSA_UINT16	Idx = pTimer->Port - 1;
	LSA_BOOL	SendReq = LSA_TRUE;
	LSA_BOOL	SetDelay = LSA_FALSE;
	LSA_BOOL	LinkUp;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayTimer() pChSys=%x TimerId=%d Port=%d",
					pChSys, pTimer->Id, pTimer->Port);

	if (pChSys->State == GSY_CHA_STATE_OPEN)
	{
		/* Timer occurs every 200 ms: ignore it while pause
		*/
		pTimer->Time += GSY_DELAY_TIME_START;
		if (pTimer->Time >= pChSys->Port[Idx].ActDelay.Time)
		{
			/* Reset elapsed time and check LinkUp
			*/
			pTimer->Time = 0;
			LinkUp = K32GSY_LINK_GET(pChSys->InterfaceId, pTimer->Port);
			if (LinkUp)
				State = pChSys->Port[Idx].ActDelay.State;
			else
				State = GSY_DELAY_STATE_DOWN;
			Time = pChSys->Port[Idx].ActDelay.Time;

			switch (State)
			{
			case GSY_DELAY_STATE_TIME:

				/* Pause terminated: GSY_DELAY_TIME_START until next measurement
				 * 270907lrg002: restart local RCF calculation
				*/
				pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_INI;
				pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_START;
				pChSys->Port[Idx].ActDelay.RcfLocal.State = GSY_DRIFT_STATE_UNKNOWN;
				pChSys->Port[Idx].ActDelay.RcfLocal.Count = 0;
				break;

			case GSY_DELAY_STATE_END:

				/* Single measurement finished successfully
				*/
				if ((0 != pChSys->Port[Idx].SumCount)
				&&  (0 == (pChSys->Port[Idx].SumCount % GSY_DELAY_MAX_COUNT)))
				{
					/* Sequence of measurements finished: GSY_DELAY_TIME_CONT pause until next measurement
					 * 280907lrg001: restart building average value after the pause
					*/
					pChSys->Port[Idx].SumCount = 0;
					pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_TIME;
					pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_CONT;
					SendReq = LSA_FALSE;
					SetDelay = LSA_TRUE;
				}
				else if ((0 == pChSys->Port[Idx].SyncLineDelay)
					 &&  (0 != pChSys->Port[Idx].SyncLineDelayTemp))
				{
					/* First delay calculated: publish it
					*/
					SetDelay = LSA_TRUE;
				}
				/* 040608lrg001: restart error counter
				*/
				pChSys->Port[Idx].ErrorCount = 0;
				break;

			case GSY_DELAY_STATE_DOWN:

				SendReq = LSA_FALSE;
				if (pChSys->Port[Idx].ActDelay.State != GSY_DELAY_STATE_DOWN)
				{
					/* LinkDown detected: set Delay to 0
					*/
					GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,  "--- k32gsy_DelayTimer(%u,%u) SeqId=%u State=0x%02x LastError=0x%x: delay state change ->LINKDOWN",
									pChSys->InterfaceId,Idx+1,pChSys->Port[Idx].ActDelay.ReqSeqId,pChSys->Port[Idx].ActDelay.State,pChSys->Port[Idx].LastError);
					pChSys->Port[Idx].SyncLineDelayTemp = 0;
					pChSys->Port[Idx].CableDelayTemp = 0;
					pChSys->Port[Idx].MinDelayTemp = 0;
					pChSys->Port[Idx].MaxDelayTemp = 0;
					pChSys->Port[Idx].SumCount = 0;
					pChSys->Port[Idx].DelayValid = LSA_TRUE;
					pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_DOWN;
					SetDelay = LSA_TRUE;
				}
				else if (LinkUp)
				{
					/* End of LinkDown: restart delay measurement
					*/
					GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,  "--- k32gsy_DelayTimer(%u,%u) SeqId=%u State=0x%02x LastError=0x%x: delay state change ->LINKUP",
									pChSys->InterfaceId,Idx+1,pChSys->Port[Idx].ActDelay.ReqSeqId,pChSys->Port[Idx].ActDelay.State,pChSys->Port[Idx].LastError);
					pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_INI;
					pChSys->Port[Idx].ActDelay.RcfLocal.State = GSY_DRIFT_STATE_UNKNOWN;
					pChSys->Port[Idx].ActDelay.RcfLocal.Count = 0;
					SendReq = LSA_TRUE;
				}
				pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_START;
				break;

			default:

				/* Single measurement not finished:
				 * 060907lrg001: set GSY_DELAY_STATE_TIME
				 * 040608lrg001: after GSY_DELAY_NUM_ERROR sequential errors the delay is set to 0
				 * 170608lrg001: LEVEL_ERROR only if a delay was calculated prior to error
				*/
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** k32gsy_DelayTimer() Delay error at Port=%u State=0x%02x LastError=0x%x",
						Idx+1, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].LastError);

				pChSys->Port[Idx].ErrorCount++;
				if (pChSys->Port[Idx].ErrorCount >= GSY_DELAY_NUM_ERROR)
				{
					if (0 != pChSys->Port[Idx].SyncLineDelay)
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** k32gsy_DelayTimer() Setting delay to zero: Port=%u State=0x%02x LastError=0x%x",
								Idx+1, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].LastError);
					}
					pChSys->Port[Idx].SyncLineDelayTemp = 0;
					pChSys->Port[Idx].CableDelayTemp = 0;
					pChSys->Port[Idx].MinDelayTemp = 0;
					pChSys->Port[Idx].MaxDelayTemp = 0;
					pChSys->Port[Idx].SumCount = 0;
					pChSys->Port[Idx].DelayValid = LSA_TRUE;
					pChSys->Port[Idx].ErrorCount = 0;
					SetDelay = LSA_TRUE;
				}
				pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_TIME;
				pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_NEW;
				SendReq = LSA_FALSE;
				break;
			}

			GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- k32gsy_DelayTimer() pChSys=%x Port=%u SetDelay=%u SendReq=%u Old/New State=0x%02x/0x%02x Time=%u/%u",
					pChSys, pTimer->Port, SetDelay, SendReq, State, pChSys->Port[Idx].ActDelay.State, Time, pChSys->Port[Idx].ActDelay.Time);

			if (SetDelay)
			{
				/* js040806: put last calculated temporary values to actual delay values
				*/
				k32gsy_DelayTempToActual(pChSys, pTimer->Port);

				/* Temporary variables invalid / response not yet received
				 * 240608lrg001: moved to gsy_DelayTempToActual()
				pChSys->Port[Idx].DelayValid = LSA_FALSE;
				*/
				pChSys->Port[Idx].ResponseArrived = LSA_FALSE;
			}

			if (SendReq)
			{
				/* Send next Delay Request Frame
				*/
				k32gsy_DelayReqSend(pChSys, pTimer->Port);
			}
		}
	}
	else if (pChSys->State == GSY_CHA_STATE_CLOSED)
	{
		/* Channel is closing:
		 * retry to free all resources
		RetVal = k32gsy_ChSysClean(pChSys);
		if (GSY_RET_OK == RetVal)
		*/
		{
			/* Callback Upper RQB and free channel structure
			k32gsy_CallbackSys(pChSys, LSA_NULL, pTimer->pChSys->Resp);
			k32gsy_ChSysFree(pChSys);
			*/
		}
	}
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayTimer() pTimer=%x State=%u Time=%u",
					pTimer, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].ActDelay.Time);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayReqStart()                                 */
/* Start the state machine for delay measurement at one port                 */
/*****************************************************************************/
K32_STATIC LSA_UINT16 K32_LOCAL_FCT_ATTR k32gsy_DelayReqStart(
GSY_CH_K32_PTR  pChSys,
LSA_UINT16		PortId)
{
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT16	Response = GSY_RSP_OK;

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqStart() pChSys=%x Port=%u",
					pChSys, PortId);

 	/* Initialise delay variables and timer for this port
	*/
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- k32gsy_DelayReqStart(%u,%u) delay state change ->START",
					pChSys->InterfaceId, PortId);

	if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[Idx].pDelayReqPdu, LSA_NULL))
	{
		/* No send frame buffer present for port
		*/
		Response = GSY_RSP_ERR_RESOURCE;
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DelayReqStart() pChSys=%x Port=%u: No DelayReqPdu",
					pChSys, PortId);
	}
	else
	{
		pChSys->Port[Idx].MinDelay = 0;
		pChSys->Port[Idx].MaxDelay = 0;
		pChSys->Port[Idx].SyncLineDelay = 0;

		/* js040806
		*/
		pChSys->Port[Idx].DelayValid = LSA_FALSE;
		pChSys->Port[Idx].CableDelayTemp = 0;
		pChSys->Port[Idx].MinDelayTemp = 0;
		pChSys->Port[Idx].MaxDelayTemp = 0;
		pChSys->Port[Idx].SyncLineDelayTemp  = 0;
		pChSys->Port[Idx].ResponseArrived = LSA_FALSE;

		pChSys->Port[Idx].SumCount = 0;
		pChSys->Port[Idx].SumDelay = 0;
		pChSys->Port[Idx].LastError = K32_TXRESULT_OK;
		pChSys->Port[Idx].ErrorCount = 0;
		pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_START;

		/* 030407lrg001: restart local RCF calculation
		*/
		pChSys->Port[Idx].ActDelay.RcfLocal.State = GSY_DRIFT_STATE_UNKNOWN;
		pChSys->Port[Idx].ActDelay.RcfLocal.Count = 0;

		/* Delay responder is active now
		*/
		pChSys->Port[Idx].DelayRspActive = LSA_TRUE;

		/* Build Delay Request PDU
		*/
		k32gsy_DelayReqFill(pChSys, PortId, pChSys->Port[Idx].pDelayReqPdu);

		/* On LinkUp: send Delay Request frame
		*/
		if (K32GSY_LINK_GET(pChSys->InterfaceId, PortId))
		{
			k32gsy_DelayReqSend(pChSys, PortId);
		}
		/* Start Delay Timer
		*/
		k32gsy_TimerStart(&pChSys->Port[Idx].DelayTimer, GSY_DELAY_TIME_START);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayReqStart() pChSys=%x",pChSys);
	return(Response);
}

/*****************************************************************************/
/* Internal function: k32gsy_DelayReqStop()                                  */
/* Stop the state machine for delay measurement at one port                  */
/*****************************************************************************/
K32_STATIC LSA_UINT16 K32_LOCAL_FCT_ATTR k32gsy_DelayReqStop(
GSY_CH_K32_PTR  	pChSys,
LSA_UINT16			PortId,
LSA_UINT8			RspActive,
K32_RQB_PTR_TYPE  	pRQB)
{
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT32	OldDelay = pChSys->Port[Idx].SyncLineDelay;
	LSA_UINT16	Response = K32_RSP_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReqStop() pChSys=%x OldDelay=%d Port=%d",
					pChSys, OldDelay, PortId);

	if (K32_DELAY_ACTIVE == RspActive)
		pChSys->Port[Idx].DelayRspActive = LSA_TRUE;
	else
		pChSys->Port[Idx].DelayRspActive = LSA_FALSE;

	GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- k32gsy_DelayReqStop(%u,%u) LineDelay=%u CableDelay=%u pFrame=%x [%x%x%x] delay state change ->STOP",
			pChSys->InterfaceId, PortId, OldDelay, pChSys->Port[Idx].CableDelay, pChSys->Port[Idx].pDelayReqPdu, 0, 0, 0);

	/* Stop Timer
	*/
	k32gsy_TimerStop(&pChSys->Port[Idx].DelayTimer);

	/* Reinitialise temporary delay variables and put them to actual
	 * js040806:
	*/
	pChSys->Port[Idx].CableDelayTemp = 0;
	pChSys->Port[Idx].SyncLineDelayTemp = 0;
	pChSys->Port[Idx].MinDelayTemp = 0;
	pChSys->Port[Idx].MaxDelayTemp = 0;
	pChSys->Port[Idx].DelayValid = LSA_TRUE;

	pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_INI;

	k32gsy_DelayTempToActual(pChSys, PortId);

	if (LSA_HOST_PTR_ARE_EQUAL(pChSys->Port[Idx].pDelayReqPdu, LSA_NULL))
	{
		/* 181010lrg002: No send frame buffer present for port
		 * 250711lrg: copy reqest RQB to confirmation RQB
		 * and confirm request on frame send confirmation
		*/
		pChSys->Port[Idx].pReqRQB = pChSys->Port[Idx].pCnfRQB;
		*pChSys->Port[Idx].pCnfRQB = *pRQB;
		Response = K32_RSP_OK_ACTIVE;
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayReqStop() pChSys=%x RspActive=%u Response=0x%x",
					pChSys, RspActive, Response);

	LSA_UNUSED_ARG(OldDelay);
	return(Response);			//181010lrg002
}

/*****************************************************************************/
/* External access function: k32gsy_DelayInd()                               */
/* Handle received DelayFrame                                                */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_DelayInd(
K32GSY_FRAME_PTR	pFrameBuffer,
LSA_UINT16			FrameLength,
LSA_UINT8			InterfaceId,
LSA_UINT16			PortId,
LSA_UINT8			Result,
LSA_UINT8			UseType,
LSA_UINT32			Timestamp)
{
	LSA_BOOL				VlanTag = LSA_FALSE;
	LSA_BOOL				RecvAgain = LSA_TRUE;
	GSY_LOWER_RX_PDU_PTR	pPduRx = LSA_NULL;
	GSY_LOWER_RX_RCV_PTR	pPduRcv = (GSY_LOWER_RX_RCV_PTR)pFrameBuffer;
	LSA_UINT16				FrameId = 0, DataLength = 0;
	LSA_UINT32				TraceIdx = 0;
	GSY_CH_K32_PTR  		pChSys = k32gsy_SysPtrGet(InterfaceId);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL) && (K32_USETYPE_DELAY == UseType))
	{
		TraceIdx = pChSys->TraceIdx;
		GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayInd() pChSys=%x pFrame=%x Port=%u",
					pChSys, pFrameBuffer, PortId);

		if (Result == K32_RXRESULT_OK)
		{
			/* Skip PDU header inclusive FrameID
			*/
			if (GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.EthType.Word))
			{
				/* PDU without VLAN tag
				*/
				FrameId = GSY_SWAP16(pPduRcv->PduNoVlan.PduHdr.FrameId.Word);
				pPduRx = (GSY_LOWER_RX_PDU_PTR)&pPduRcv->PduNoVlan.PtcpData;
				DataLength = FrameLength - GSY_PDU_HDR_SIZE_NO_VLAN;
			}
			else if (GSY_ETHETYPE == GSY_SWAP16(pPduRcv->PduVlan.PduHdr.EthType.Word))
			{
				/* PDU with VLAN tag
				*/
				VlanTag = LSA_TRUE;
				FrameId = GSY_SWAP16(pPduRcv->PduVlan.PduHdr.FrameId.Word);
				pPduRx = (GSY_LOWER_RX_PDU_PTR)&pPduRcv->PduVlan.PtcpData;
				DataLength = FrameLength - GSY_PDU_HDR_SIZE_VLAN;
			}
			else
			{
				GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_DelayInd() pChSys=%x: Invalid PDU",pChSys);
			}

			if (!LSA_HOST_PTR_ARE_EQUAL(pPduRx, LSA_NULL))
			{
				GSY_DEL_TRACE_08(TraceIdx, LSA_TRACE_LEVEL_NOTE, "++> RCV(%x,%x) Port=%u FrameId=0x%04x SeqId=%u Delay10=%010u Delay1=%u Timestamp=%u",
								pChSys, Result, PortId, FrameId, GSY_SWAP16(pPduRx->DelayReq.PtcpHdr.SequenceID.Word),
								GSY_SWAP32(pPduRx->DelayReq.PtcpHdr.Delay10ns.Dword), pPduRx->DelayReq.PtcpHdr.Delay1ns, Timestamp);
				switch (FrameId)
				{
				default:
					GSY_ERROR_TRACE_01(TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** k32gsy_Indication() Invalid FrameId=0x%04x",
									FrameId);
					break;

				case GSY_FRAMEID_DELAY_REQ:

					/* Delay request received
					*/
					RecvAgain = k32gsy_DelayRspSend(pChSys, PortId, pPduRcv, FrameLength, Timestamp, VlanTag);
					break;

				case GSY_FRAMEID_DELAY_FURSP:

					/* Delay response with FollowUp received
					*/
					k32gsy_DelayRspInd(pChSys, PortId, pPduRx, DataLength, Timestamp, LSA_TRUE);
					break;

				case GSY_FRAMEID_DELAY_RSP:

					/* Delay response without FollowUp received
					*/
					k32gsy_DelayRspInd(pChSys, PortId, pPduRx, DataLength, Timestamp, LSA_FALSE);
					break;

				case GSY_FRAMEID_DELAY_FU:

					/* Delay FollowUp received
					*/
					k32gsy_DelayFuInd(pChSys, PortId, pPduRx, DataLength);
					break;
				}
			}
		}
		/* AP01425250:ERTEC200P: KRISC32 stürzt ab bei Ethernet Verbindung zu Scalance X204IRT FW V3
		 * --> only ignore frame if it is too long
		*/
		else if (Result == K32_RXRESULT_LEN)
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** k32gsy_DelayInd() pChSys=%x: Ignoring invalid delay frame length=%u from port=%u",
							pChSys, FrameLength, PortId);
		}
		else
		{
			GSY_ERROR_TRACE_02(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_DelayInd() pChSys=%x: RecvResult=0x%02x",
							pChSys, Result);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_FATAL,"*** k32gsy_DelayInd() pChSys=%x: Invalid Interface=%u or UseType=%u",
						pChSys, InterfaceId, UseType);
	}

	if (RecvAgain)
	{
		/* Receive next frame
		*/
		K32GSY_RECV_FRAME(pFrameBuffer, k32_DelayFrameSize, K32_USETYPE_DELAY, InterfaceId);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayInd() Interface=%u Timestamp=%u Result=%u",
					InterfaceId, Timestamp, Result);
}

/*****************************************************************************/
/* External access function: k32gsy_DelayReq()                               */
/* Handle KRISK32-RRQB with Opcode K32_OPC_GSY_DELAY_REQ.                    */
/*****************************************************************************/
LSA_VOID K32_SYS_IN_FCT_ATTR k32gsy_DelayReq (
#ifndef K32_RAM32
K32_RQB_PTR_TYPE  pRQB)
{
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(pRQB->Hdr.InterfaceID);
#else
K32_RQB_PTR_TYPE  pRQB32)
{
	K32_RQB_TYPE  		RQB;
	K32_RQB_PTR_TYPE  	pRQB = &RQB;
	GSY_CH_K32_PTR  pChSys = k32gsy_SysPtrGet(pRQB32->Hdr.InterfaceID);
#endif
	LSA_UINT32		TraceIdx = 0;
	LSA_UINT16		Response = GSY_RSP_ERR_PARAM;
	LSA_UINT16		Idx;
#ifdef K32_RAM32
	K32_RAM32_CPY(pRQB32, pRQB);
#endif
	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys, LSA_NULL))
	{
		TraceIdx = pChSys->TraceIdx;
		if (K32_OPC_GSY_DELAY_REQ == pRQB->Hdr.Opcode)
		{
			if ((pRQB->Params.DelayReq.PortID != 0)
			&&  (pRQB->Params.DelayReq.PortID <= pChSys->PortCount))
			{
				GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > k32gsy_DelayReq() pChSys=%x pRQB=%x Port=%d",
								pChSys, pRQB, pRQB->Params.DelayReq.PortID);

				Response = GSY_RSP_OK;
				Idx = pRQB->Params.DelayReq.PortID - 1;

				if ((K32_DELAY_INACTIVE == pRQB->Params.DelayReq.ReqActive)
				&&  pChSys->Port[Idx].DelayReqActive)
				{
					/* Stop Delay measurement
					*/
					Response = k32gsy_DelayReqStop(pChSys, pRQB->Params.DelayReq.PortID, pRQB->Params.DelayReq.RspActive, pRQB);
					pChSys->Port[Idx].DelayReqActive = LSA_FALSE;
				}
				else if ((K32_DELAY_ACTIVE == pRQB->Params.DelayReq.ReqActive)
				     &&  (!pChSys->Port[Idx].DelayReqActive))
				{
					/* Start Delay measurement
					*/
					pChSys->Port[Idx].RxDelay = pRQB->Params.DelayReq.RxPortDelay;
					pChSys->Port[Idx].TxDelay = pRQB->Params.DelayReq.TxPortDelay;
                    K32_COPY_MAC_ADDR_ARRAY (pChSys->Port[Idx].MACAddr.MacAdr, pRQB->Params.DelayReq.PortMacAddr);
					pChSys->Port[Idx].DelayReqActive = LSA_TRUE;
					Response = k32gsy_DelayReqStart(pChSys, pRQB->Params.DelayReq.PortID);
					if ((GSY_RSP_OK == Response) && (!pChSys->DriftTimer.Running))
					{
						/* Start rate timer only once
						*/
						k32gsy_TimerStart(&pChSys->DriftTimer, GSY_DRIFT_TIME_VALUE);
					}
					/* Only for TEST:
					 * 250711lrg: copy reqest RQB to confirmation RQB
					 * and confirm request on frame send confirmation
					GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- k32gsy_DelayReq() pChSys=%x pRQB=%x: TEST confirmation Port=%u",
								pChSys, pRQB, pRQB->Params.DelayReq.PortID);
					pChSys->Port[Idx].pReqRQB = pChSys->Port[Idx].pCnfRQB;
					*pChSys->Port[Idx].pCnfRQB = *pRQB;
					Response = K32_RSP_OK_ACTIVE;
					*/
				}
			}
			else
			{
				GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DelayReq() pChSys=%x pRQB=%x: Invalid Port=%u",
								pChSys, pRQB, pRQB->Params.DelayReq.PortID);
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DelayReq() pChSys=%x pRQB=%x: Invalid Opcode=%u",
							pChSys, pRQB, pRQB->Hdr.Opcode);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** k32gsy_DelayReq() pChSys=%x pRQB=%x: Invalid Interface=%u",
						pChSys, pRQB, pRQB->Hdr.InterfaceID);
	}

	GSY_FUNCTION_TRACE_03(TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < k32gsy_DelayReq() ReqActive=%u RspActive=%u Response=0x%x",
					pRQB->Params.DelayReq.ReqActive, pRQB->Params.DelayReq.RspActive, Response);

	pRQB->Hdr.Response = Response;
#ifdef K32_RAM32
	K32_RAM32_CPY(pRQB, pRQB32);
	K32GSY_CALLBACK(pRQB32);
#else	/* K32_RAM32 */
	K32GSY_CALLBACK(pRQB);
#endif	/* K32_RAM32 */
}

/*****************************************************************************/
/*  end of file k32_gsy_del.c                                                */
/*****************************************************************************/

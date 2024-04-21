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
/*  F i l e               &F: gsy_del.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Functions for Delay measurement.                                         */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2005-03-07  P01.00.00.00_00.01.00.00 lrg  file creation                  */
/*  2005-04-04  P01.00.00.00_00.01.00.01 lrg  Umstellung auf TLVs            */
/*  2005-05-03  P03.05.00.00_00.03.00.01 lrg  gsy_UserDelayInd() bei der     */
/*              lower DelaySet-Confirmation statt in gsy_DelayCalc() und     */
/*              gsy_DelayReqStop() aufrufen.                                 */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg Prototype for PT2               */
/*  2005-06-09  P03.06.00.00_00.03.00.02 lrg DriftCorrection() in signed NS  */
/*  2005-06-24  P03.06.00.00_00.03.02.02 lrg neues Sync-Userinterface        */
/*  2005-07-26  P03.07.00.00_00.04.01.01 lrg neu: gsy_DelayRestart()         */
/*  2005-09-14  P03.07.00.00_00.08.01.01 js  Initialisierung einiger         */
/*                                           Variablen um Compiler Warning   */
/*                                           zu verhindern.                  */
/*  2005-09-16  P03.07.00.00_00.08.01.01 js  ps (pico seconds) ergänzt.      */
/*  2005-10-28  P03.08.00.00_00.01.01.01 lrg Drift fuer alle SyncIDs.        */
/*              Delaymessung benutzt Drift der kleinsten SyncID am Port.     */
/*  2005-11-10  P03.08.00.00_00.01.02.00 lrg 101105lrg001:GSY_DRIFT_EXTERNAL */
/*  2005-12-06  P03.08.00.00_00.01.02.01 lrg GSY_DRIFT_EXTERNAL entfernt     */
/*              statt dessen neu: gsy_DriftIndication()                      */
/*              071205lrg001: kurzes (20cm) statt keinem Kabel               */
/*  2006-02-15  P03.09.00.00_00.03.01.02 lrg In gsy_DelayRestart() die       */
/*              Delaymessung nur neu starten, wenn der Portstatus es erlaubt */
/*              (ArtsRQ285959: Timer laeuft schon bei gsy_TimerStart())      */
/*  2006-03-30  P03.10.00.00_00.02.01.02 lrg gsy_CalcNanosDiff() -> clp_     */
/*              CalcNanosDiff(). Aufrufstelle in User2 an gsy_DiagUserInd()  */
/*  2006-05-12  P03.11.00.00_00.03.01.02 lrg gsy_DelayRspInd():              */
/*              mehrfache Response: neue Messung mit Delay=0 starten         */
/*  2006-05-12  P03.11.00.00_00.03.01.03 lrg  Port-MAC-Adresse benutzen in:  */
/*              gsy_DelayReqFill(), gsy_DelayRspFill(), gsy_DelayFuFill()    */
/*  2006-05-29  P03.11.00.00_00.04.01.02 lrg  erweitert: GSY_DIAG_DATA_BLOCK */
/*  2006-06-13  P03.12.00.00_00.01.01.02 120606lrg001, 120606lrg002,         */
/*              130606lrg001                                                 */
/*  2006-06-28  P03.12.00.00_00.02.01.02 lrg GSY_FILE_SYSTEM_EXTENSION(),    */
/*              gleitender Mittelwert in gsy_DriftCalc() und gsy_DelayCalc() */
/*  2006-07-27  P03.12.00.00_00.25.02.02 lrg GSY_RT_CLASS3_SUPPORT_ON        */
/*  2006-08-06  P03.12.00.00_00.25.04.01                                     */
/*                        Timeoutbehandlung bei Delaymessung geändert. Rampe */
/*                        immer fahren und nicht alle Sekunde Delayrequest   */
/*                        senden.                                            */
/*              js040806: Delaymessung umgestellt um mit multiple responses  */
/*                        besser klar zu kommen..                            */
/*                        Das Setzen des neuen Delays erfolgt zum Ende des   */
/*                        aktuellen Zyklusses (bzw. beim Start des neuen).   */
/*                        Treffen bis dahin weitere respones ein (multiple)  */
/*                        so wird die Messung rückgesetzt (delay auf 0). Es  */
/*                        wird aber normal weitergemacht. Es erfolgt kein    */
/*                        Neustart der Delaymessung mehr. D.h. im Fehlerfall */
/*                        wird genauso gemessen wie im Gutfall. Allerdings   */
/*                        bleibt der linedelay auf "0" solange multiple      */
/*                        responses kommen.                                  */
/*              js040806: Avoid compiler warning with gcc                    */
/*              090806js001 : SyncEvents müssen auch indication erzeugen.    */
/*                            Alle anderen werden nicht mehr vermerkt.       */ 
/*              090806js001 : Mittelwertbildung rücksetzen wenn MultipleResp.*/
/*  2006-09-16  P03.12.00.00_00.25.04.02                                     */
/*              190906lrg001: nur noch ein RCF-Intervall pro SyncId          */
/*              280906lrg001: Driftwerte ohne MIN/MAX immer verwenden, aber  */
/*                            nur aus PDUs vom selben Master berechnen       */
/*              HWabhaengige Delayaddition: gsy_SlaveAction->gsy_DelayGet()  */
/*  2006-10-12  P03.12.00.00_00.30.02.02 lrg GSY_DRIFT_STATE_TRANSIENT       */
/*  2006-10-19  P03.12.00.00_00.30.03.02 191006lrg001: lokale Drift-MAC-     */
/*              Adresse nicht ueberschreiben 231006lrg: gleiche Subdomain    */
/*  2006-11-09  P03.12.00.00_00.30.04.02 lrg GSY_DELAY_DEBUG_DIAG            */
/*  2006-11-23  P03.12.00.00_00.40.01.02 231106lrg001: PDU des fremden       */
/*              Masters fuer Drift ignorieren                                */
/*  2006-11-30  P03.12.00.00_00.40.03.02 301106lrg004: Das Verwenden aller   */
/*              Driftwerte mit #define GSY_CHECK_RCF abschaltbar machen      */ 
/*  2006-12-14  P03.12.00.00_00.40.05.02 141206lrg001: ErrorTrace bei Empf.  */
/*              einer unbekannten SubdomainUUID (falsche Driftkorrektur)     */
/*  2006-12-19  P03.12.00.00_00.40.06.02 191206lrg001: gsy_DriftIndication() */
/*              ComClass immer setzen, 191206lrg002: LostCount != 0 abfangen */
/*  2007-01-10  P03.12.00.00_00.40.06.03 100107lrg003: RCF-Mittelwert als    */
/*              Median oder den Betraegen der Einzelwerte bilden             */
/*              150106lrg001: Driftstruktur freigeben wenn nach 1 Messinter- */
/*              noch kein neues SyncFrame von dem aktuellen Master da ist.   */
/*              neu: gsy_DriftReset() belegt SubdomainUUID                   */
/*  2007-01-18  P03.12.00.00_00.40.07.02 180107lrg001: Test ohne Delay-FU    */
/*  2007-02-12  P03.12.00.00_00.40.09.02 lrg #ifdef GSY_USE_VLAN             */
/*  2007-02-20  P03.12.00.00_00.40.10.02 200207lrg001: SyncTrace erweitert   */
/*  2007-04-03  P04.00.00.00_00.01.01.02 030407lrg001: Delaymessung mit      */
/*              lokaler RCF-Bestimmung zum Nachbarn                          */
/*  2007-04-25  P03.12.00.00_00.40.11.02 250407lrg001: ArtsRQ AP00455352     */
/*              Delaymessung auch bei DriftIndication des Masters neu starten*/
/*  2007-06-15  P04.00.00.00_00.20.01.02 130607lrg001: ArtsRQ AP00477562     */
/*              Delay.req nur senden wenn der vorhergehende Sende-RQB        */
/*              konfirmiert ist.                                             */
/*  2007-07-16  P04.00.00.00_00.20.01.02 160707lrg001:Umstellung auf DelayNS */
/*              gsy_DriftIndication() entfernt                               */
/*  2007-07-27  P04.00.00.00_00.30.01.00 270707lrg001: HW-abh. Delayaddition */
/*              in gsy_DelayGet() beim Eintreffen des SyncFrames statt FU    */
/*  2007-07-31  P04.00.00.00_00.30.01.02 310707js001:  Beachten dass beim    */
/*              ERTEC200/SOC das Setzen der Rate den timestamp verändert und */
/*              die ermittelte Rate. Die EDD Funktion benötigt die unver-    */
/*              ändere Rate, die Mittelwertbildung muss über die unveränderte*/
/*              Rate erfolgen. Wenn die Rate in hardware korrigiert wird darf*/
/*              diese nicht auch noch in Software korrigiert werden.         */
/*  2007-09-03  P04.00.00.00_00.30.05.02 030907lrg001:  PortDelayReqActive   */
/*  2007-09-05  P04.00.00.00_00.30.05.03 050907lrg001:  GSY_DEL_TRACE_08     */
/*  2007-09-05  P04.00.00.00_00.30.05.04 050907lrg002: Delayrampe -> Burst   */
/*  2007-09-06  P04.00.00.00_00.30.05.05 060907lrg001: GSY_DELAY_STATE_TIME  */
/*              110907lrg001: LEVEL_UNEXP bei "Zero cable delay"             */
/*  2007-09-06  P04.00.00.00_00.30.06.02 240907lrg002: ->Sync -> ->Announce  */
/*              270907lrg001: #define GSY_DELAY_BURST entfernt               */
/*              270907lrg002: lokale RCF-Berechnung neu starten              */
/*              270907lrg003: GSY_DRIFT_STATE_NONE entfernt                  */
/*              280907lrg001: Mittelwertbildung nach Pause neu beginnen      */
/*              280907lrg002: Epoch auf 0 pruefen                            */
/*  2007-10-04  P04.00.00.00_00.40.01.02 041007lrg005: Test-Traceausgaben in */
/*              gsy_TLVGetInfo() entfernt.                                   */
/*              161007lrg003: AP00526517 PrimaryMaster                       */
/*  2007-10-04  P04.00.00.00_00.50.01.02 301007lrg001: ArtsRQ AP00535301     */
/*              Delay Request bei PeerBoundary am Port nicht beantworten     */
/*  2008-01-18  P04.00.00.00_00.70.01.03 180108lrg001: Rate parallel         */
/*              berechnen. 210108lrg001: Median-History loeschen.            */
/*  2008-04-18  P04.00.00.00_00.80.01.03 170408lrg002: RcvSyncPrio,LocalPrio */
/*              180408lrg001: AP00618160/AP00615803 Wenn RspDelayNanos 0 ist */
/*              CableDelay auf 0 setzen als Zeichen für OHA, dass fuer den   */  
/*              Port kein Vergleich zwischen Local und Remote LineDelay      */
/*              gemacht werden soll.                                         */
/*  2008-06-04  P04.00.00.00_00.80.03.02 040608lrg001: ArtsRQ AP00648600     */
/*              GSY_DELAY_NUM_ERROR = Anzahl Fehler bis zum Delayreset       */
/*              040608lrg002: Delay.req auch bei gesetzter PeerToPeer-       */
/*              Boundary beantworten (ArtsRQ AP00648620).                    */
/*  2008-06-17  P04.00.00.00_00.80.04.02 170608lrg001: LEVEL_ERROR bei       */
/*              Delay-Fehler nur wenn vorher Delay berechnet war             */
/*              Traceausgaben in gsy_DelayReqStart/Stop                      */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-06-24  P04.01.00.00_00.01.01.02 240608lrg001: ArtsRQ AP00536908     */
/*              DelaySet zwischen Senderequest und Confirmation verzögern    */
/*  2008-10-17  P04.01.00.00_00.02.02.02 GSY_HW_TYPE stett EDD_HW_TYPE_USED  */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2009-09-29  P04.02.00.00_00.01.04.02 290909lrg001: Retry setting Drift.  */
/*              Required because master sets drift to zero only one time.    */ 
/*  2009-10-08  P04.02.00.00_00.01.04.04 lrg: GSY_HW_TYPE_DELAY_MSK...       */
/*  2009-11-17  P05.00.00.00_00.01.01.02 171109lrg001: check for PDU from    */
/*              myself in gsy_TLVGetInfo() to avoid calculating drift etc.   */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-03-12  P05.00.00.00_00.01.03.03 lrg: Check Subdomain of Sync and FU */
/*  2010-07-14  P05.00.00.00_00.01.06.02 140710lrg001: AP00991839            */
/*              Check RspDelayNS for GSY_PTCP_DELAY_NS_INVALID.              */
/*              German comments translated to english                        */
/*  2010-08-25  P05.00.00.00_00.01.07.02 250810lrg001: AP00697597            */
/*              German comments translated to english                        */
/*  2010-09-22  P05.00.00.00_00.01.08.02 150910lrg001: check Delay10NS       */
/*              220910lrg001: report ReqSeqId                                */
/*  2010-12-15  P05.00.00.00_00.01.08.02 151210lrg001:                       */
/*              Do not reset rate if master state is pending.                */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-05-27  P05.00.00.00_00.05.21.02 270511lrg: Trace (%d)               */
/*  2012-05-30  P05.00.00.00_00.05.97.02 AP01336020: Change OldMasterAddr    */
/*              only if different to act. master (after takeover)            */
/*  2011-09-29  S05.01.00.00_00.04.10.01 AP01236058 Lint Warnings entfernen  */
/*  2011-10-24  S05.01.00.00_00.05.05.02 AP01262791 Negative CableDelay      */
/*  2012-06-11  P05.00.00.00_00.05.97.02 AP01366052: LSA trace               */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01384863: DelayFilter             */
/*  2013-03-20  P05.00.00.00_00.05.97.02 AP01512559: Allow forwarding of FU  */
/*              after Takeover                                               */
/*  2013-04-17  P05.02.00.00_00.04.22.02 AP01527161: RspDelayNanos = 0       */
/*  2013-11-25  P05.03.00.00_00.02.20.01 903419/907378: Mittelwertfreie      */
/*              Berechnung der LineDelay                                     */
/*  2014-05-20  P06.00.00.00_00. TODO gl 966426: [GSY] Die Synchronisation   */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 remove gcc warnings         */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*  2015-12-15  V06.00, Inc13, RQ 1534107 AR dropped after break inside MRPD */
/*              check port id of FU frame before rate calculation            */
/*  2016-05-02  V06.00-V07.00, RQ 1684006: PendingSyncId                     */
/*  2017-02-21  V06.01-V07.00, RQ 2133976: Tracelevel WARN->NOTE/WARN        */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	4		/* GSY_MODULE_ID_DEL */
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
/* Internal access function: gsy_DelayGet()                                  */
/* This function transforms the delay values from TLVInfo to seconds and     */
/* nanoseconds in the given structure (*pTime) for drift calculation and     */
/* synchronisation.                                                          */
/* Return: LSA_FALSE:  invalid delay values                                  */
/*         LSA_TRUE:   OK.                                                   */
/*****************************************************************************/
LSA_BOOL GSY_LOCAL_FCT_ATTR gsy_DelayGet(
GSY_CH_SYS_PTR  		pChSys,
GSY_TIME_TYPE 			*pTime,
GSY_TLV_INFO_PTR_TYPE	pTLVInfo,
LSA_UINT16				PortId)
{
	LSA_UINT32			DelayNanos = 0;
	LSA_UINT32			DelaySeconds = 0;
	LSA_BOOL			PduOk = LSA_TRUE;

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayGet() pChSys=%X pTime=%X pTLVInfo=%X Port=%u", 
					pChSys, pTime, pTLVInfo, PortId);

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

			/* Depending on hardware: add LineDelay of receiving port
			 * 270707lrg001: at SYNC frame instead at FU
			*/
			switch (pChSys->HardwareType & GSY_HW_TYPE_MSK) 
			{
			case GSY_HW_TYPE_ERTEC_200:
			case GSY_HW_TYPE_SOC:
				/* LineDelay of port is already added
				*/
				break;

			case GSY_HW_TYPE_ERTEC_400:
			default:
				/* Add LineDelay of port now
				*/
				pTime->DelayNanos += pChSys->Port[PortId-1].SyncLineDelay;
				while (pTime->DelayNanos >= GSY_NANOS_PER_SECOND)
				{
					pTime->DelayNanos -= GSY_NANOS_PER_SECOND;
					pTime->DelaySeconds++;
				}
			}
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
					pTime->DelayNanos = GSY_NANOS_PER_SECOND + pTime->DelayNanos - DelayNanos;
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

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayGet() pChSys=%X DelaySeconds=%u DelayNanos=%u RetVal=%u", 
					pChSys, pTime->DelaySeconds, pTime->DelayNanos, PduOk);
	return(PduOk);
}

/*****************************************************************************/
/* Internal access function: gsy_TLVGetInfo()                                */
/* -> function validates PDU (incl. TLV-Chain) and fills structure TLVInfo.  */
/* -> TLVInfo contains parameters dependent of the PDU type.                 */
/* pPduRx   points to start of PDU in frame (Frame-ID)                       */
/* PDULen   length of PDU                                                    */
/* pTLVInfo pointer to structure to be filled                                */
/* Return:  LSA_FALSE:  invalid PDU                                          */
/*          LSA_TRUE:   OK. Structure filled.                                */
/*****************************************************************************/
LSA_BOOL GSY_LOCAL_FCT_ATTR gsy_TLVGetInfo(
GSY_CH_SYS_PTR  		pChSys,
GSY_LOWER_RXBYTE_PTR	pPduRx,
LSA_UINT16				PduLen,
LSA_UINT32				Timestamp,
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

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_TLVGetInfo() pChSys=%X pPduRx=%X PduLen=%u pTLVInfo=%X", 
					pChSys, pPduRx, PduLen, pTLVInfo);

	pPdu = (GSY_LOWER_RX_PDU_PTR)(void*)(pPduRx + 2);
	FrameId = GSY_SWAP16(*((LSA_UINT16*)(void*)pPduRx));
	SyncId = GSY_LOBYTE(FrameId) & 0x1f;

	if ((PduLen >= GSY_SYNC_RX_SIZE + 2)
	&&  ((
#if GSY_FRAMEID_SYNC_MIN != 0  /* avoid gcc warning js040806 */
		  (GSY_FRAMEID_SYNC_MIN <= FrameId) &&
#endif                     
		  (GSY_FRAMEID_SYNC_MAX >= FrameId))
	 ||  ((GSY_FRAMEID_FUSYNC_MIN <= FrameId) && (GSY_FRAMEID_FUSYNC_MAX >= FrameId))))
	{
		/* Sync PDU
		*/
		PduType = GSY_TLV_TYPE_SYNC;
		pTlvSubdomain = &(pPdu->Sync.TLV.Subdomain);
	}
	else if ((GSY_FRAMEID_FU_MIN <= FrameId)
	     &&  (GSY_FRAMEID_FU_MAX >= FrameId)
	     &&  (PduLen >= GSY_SYNC_FU_RX_SIZE + 2))
	{
		/* Sync-FollowUp PDU
		*/
		PduType = GSY_TLV_TYPE_FOLLOWUP;
		pTlvSubdomain = &(pPdu->SyncFu.Subdomain);
	}
	else if ((GSY_FRAMEID_ANNOUNCE_MIN <= FrameId)
	     &&  (GSY_FRAMEID_ANNOUNCE_MAX >= FrameId)
	     &&  (PduLen >= GSY_ANNOUNCE_RX_SIZE + 2))
	{
		/* Announce PDU
		*/
		PduType = GSY_TLV_TYPE_ANNOUNCE;
		pTlvSubdomain = &(pPdu->Announce.Subdomain);
	}
	else
	{
		GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() pChSys=%X Port=%u PduLen=%u: Invalid FrameId=0x%04x",
					pChSys, PortId, PduLen, FrameId);
	}

	/* PDU-Check
	*/
	if (PduType != GSY_TLV_TYPE_NONE)
	{
        GSY_IS_VALID_PTR(pTlvSubdomain);

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
					if (0 == Epoch)
					{
						PduLength += GSY_TLV_TIME_SIZE + GSY_TLV_TIMEEXT_SIZE + GSY_TLV_MASTER_SIZE;
						PduOk = LSA_TRUE;
					}
					else
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() pChSys=%X Port=%u Time.Epoch=0x%04x: Invalid SyncFrame",
							pChSys, PortId, Epoch);
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
			else	/* Announce PDU */
			{
				if ((GSY_TLV_MASTER_TYPELEN == GSY_SWAP16(pPdu->Announce.Master.TypeLen.Word))
				&&  (GSY_TLV_END_TYPELEN == GSY_SWAP16(pPdu->Announce.End.TypeLen.Word)))
				{
					PduLength += GSY_TLV_MASTER_SIZE;
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
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() pChSys=%X Port=%u FrameId=0x%04x: Invalid TLV",
							pChSys, PortId, FrameId);
			}
		}
		else
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() pChSys=%X Port=%u: Invalid Subdomain TypeLen=0x%04x",
						pChSys, PortId, GSY_SWAP16(pTlvSubdomain->TypeLen.Word));
		}
	}

	if (PduOk)
	{
        GSY_IS_VALID_PTR(pTlvSubdomain);

	    if (GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pChSys->MACAddr))
	    {
		    /* 171109lrg001: check for PDU from myself here to avoid calculating drift and forwarding
		    */
		    PduOk = LSA_FALSE;
		    GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"*** gsy_TLVGetInfo() Port=%u FrameId=0x%04x: dropping SeqId=%u from myself",
						    PortId, FrameId, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
	    }
	}
	if (PduOk)
	{
        GSY_IS_VALID_PTR(pTlvSubdomain);

		/* Search drift structure * 190906lrg001: only one per SyncId
		*/
		pDrift = &pChSys->Drift[SyncId][0];
		if (GSY_TLV_TYPE_ANNOUNCE != PduType)
		{
			if (GSY_SUBDOMAIN_IS_NULL(pDrift->Subdomain)
			||  GSY_SUBDOMAIN_CMP(pTlvSubdomain->SubdomainUUID, pDrift->Subdomain))
			{
				if ((GSY_SYNCID_NONE == pDrift->SyncId)
				&&  (GSY_TLV_TYPE_SYNC == PduType))
				{
					/* 150107lrg001: Allow initialising only if subdomain matches or is dont care
					*/
					GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"-X- gsy_TLVGetInfo(%02x) Old RCF master=%02x-%02x-%02x-%02x-%02x-%02x",
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
					gsy_DriftSet(pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_UNKNOWN);  

					GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_TLVGetInfo(%02x) New RCF master=%02x-%02x-%02x-%02x-%02x-%02x SeqId=%u",
									pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
									pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5],
									GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
				}
				else 
				{
					/* Drift for SyncId already initialized or FU frame
					*/
					if (!GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->MasterAddr))
					{
						/* 250810lrg001: 
						 * Structure already used but new master 231006lrg: with same Subdomain
						 * 191006lrg001: but not used by local master
						 * 041007lrg005: NULL for Takeover
						*/
						if (GSY_SUBDOMAIN_CMP(pTlvSubdomain->SubdomainUUID, pDrift->Subdomain)
//						&&  !GSY_MACADDR_CMP(pChSys->MACAddr, pDrift->MasterAddr))
						&&  GSY_MACADDR_IS_NULL(pDrift->MasterAddr))
						{
							if  (GSY_TLV_TYPE_SYNC == PduType)
						    {
							    /* Replace MasterMAC 161007lrg003: AP00526517 PrimaryMaster
							     * 210108lrg002: init OldMasterAddr for DriftCalc
							     * 170408lrg002: RcvSyncPrio instead of PrimaryMaster
							     * AP01336020: - only if different to act. master (after takeover pDrift->MasterAddr is NULL)
							    */
							    if (!GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->OldMasterAddr))
							    {
								    pDrift->OldMasterAddr = pDrift->MasterAddr;
							    }
							    pDrift->MasterAddr = pTlvSubdomain->MasterSourceAddress;
							    GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_TLVGetInfo(%02x) Change RCF master=%02x-%02x-%02x-%02x-%02x-%02x SeqId=%u",
										    pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
										    pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5],
										    GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
						    }
							else if (GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->OldMasterAddr))
							{
								/* AP01512559: Allow forwarding of FU from old master after Takeover:
								 * restore MasterAddr for rate calculation and do not set pDrift to NULL to forward with this rate
								*/
								pDrift->MasterAddr = pTlvSubdomain->MasterSourceAddress;
								GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_TLVGetInfo(%02x) Restore RCF master=%02x-%02x-%02x-%02x-%02x-%02x SeqId=%u",
											pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1],pDrift->MasterAddr.MacAdr[2],
											pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4],pDrift->MasterAddr.MacAdr[5],
											GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word));
							}
							else
							{
								/* AP01512559: ignore FU from other master after Takeover
								*/
								pDrift = LSA_NULL;
								GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_TLVGetInfo() pChSys=%X SeqId=%u: Ignoring FU from Port=%u",
										pChSys, GSY_SWAP16(pPdu->Sync.PtcpHdr.SequenceID.Word), PortId);
							}
						}
						else
						{
							/* 231106lrg001: ignore PDU of other Master for drift
							 * 141206lrg001: write ErrorTrace
							 * 200207lrg001: Text changed
							*/
							if (GSY_TLV_TYPE_SYNC == PduType)
							{
								GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_TLVGetInfo() pChSys=%X SyncId=0x%02x: Ignoring master from Port=%u",
										pChSys, pDrift->SyncId, PortId);
							}
							pDrift = LSA_NULL;
						}
					}
				}
			}
			else
			{
				/* Ignore PDU with wrong SubdomainUUID
				*/
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_TLVGetInfo() pChSys=%X SyncId=0x%02x: Ignoring subdomain from Port=%u",
								pChSys, pDrift->SyncId, PortId);
				pDrift = LSA_NULL;
			}
		}
		else if (!GSY_MACADDR_CMP(pTlvSubdomain->MasterSourceAddress, pDrift->MasterAddr))
		{
			/* Announce with other SourceMAC
			*/
			pDrift = LSA_NULL;
		}

		/* Fill return structure
		 * 250707lrg001: ignore APDU status (not in Length = ->FrameId,Header...EndTlv<-)
		*/
		pTLVInfo->Type = PduType;
		pTLVInfo->Length = PduLength;

		if (GSY_TLV_TYPE_SYNC == PduType)
		{
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

			if ((GSY_FRAMEID_FUSYNC_MIN <= FrameId) && (GSY_FRAMEID_FUSYNC_MAX >= FrameId))
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
				pDrift->Act.Timestamp = Timestamp; //lrgi210: was GSY_SWAP32(pPdu->Sync.PtcpHdr.ReceiveTS.Dword);
				pDrift->Act.OrgSeconds = GSY_SWAP32(pPdu->Sync.TLV.Time.Seconds.Dword);
				pDrift->Act.OrgNanos = GSY_SWAP32(pPdu->Sync.TLV.Time.NanoSeconds.Dword);

				/* 170408lrg002: RcvSyncPrio instead of PrimaryMaster
				*/
				pDrift->RcvSyncPrio = pPdu->Sync.TLV.Master.MasterPriority1;

				/* Write Delay into drift structure
				*/
				PduOk = gsy_DelayGet(pChSys, &pDrift->Act, pTLVInfo, PortId);
				if (PduOk)
				{
					GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_TLVGetInfo(%02x,SY) Port=%u SeqIdOld/New/Act=%u/%u/%u UsedOld/New/Act=%d/%d/%d NextIdx=%u",
							pDrift->SyncId, PortId, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId, pDrift->Act.SeqId, 
							pDrift->Old[NextIdx].Used, pDrift->New[NextIdx].Used, pDrift->Act.Used, NextIdx);

					if (pTLVInfo->Param.Sync.FUFlag)
						pDrift->Act.Used = LSA_TRUE;
					else
					{
						/* Transfer actual PDU values to new values
						 * 180108lrg001: and calculate the rate based on the new values
						*/
						pDrift->Act.Used = LSA_FALSE;
						pDrift->New[NextIdx] = pDrift->Act;
						gsy_DriftCalc(pChSys, pDrift);

					}
				}
				else
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() Invalid sync delay: Delay10NS=%u Delay1NS=%u DelayNS=%u",
									pTLVInfo->Param.Sync.Delay10NS, pTLVInfo->Param.Sync.Delay1NS, pTLVInfo->Param.Sync.DelayNS);
				}
			}
		}
		else if (GSY_TLV_TYPE_FOLLOWUP == PduType)
		{
			pTLVInfo->Param.FollowUp.DelayNS = (LSA_INT32)(GSY_SWAP32(pPdu->SyncFu.PtcpHdr.DelayNS.Dword));
			pTLVInfo->Param.FollowUp.DelayNSOffset = GSY_PTCP_DelayNS_OFFSET;
			pTLVInfo->Param.FollowUp.SequenceID = GSY_SWAP16(pPdu->SyncFu.PtcpHdr.SequenceID.Word);
			pTLVInfo->Param.FollowUp.Master.MacAddr = pTlvSubdomain->MasterSourceAddress;

			if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
			&&	(pDrift->Act.Used)
			&&	(pDrift->Act.SeqId == pTLVInfo->Param.FollowUp.SequenceID)
			&&  (pDrift->Act.PortId == PortId))								//1534107
			{
				/* Refresh Delay in drift structure
				*/
				NextIdx = pDrift->NextIdx;
				PduOk = gsy_DelayGet(pChSys, &pDrift->Act, pTLVInfo, PortId);
				if (PduOk)
				{
					/* Transfer actual PDU values to new values
					 * 180108lrg001: and calculate the rate based on the new values
					*/
					GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_TLVGetInfo(%02x,FU) Port=%u SeqIdOld/New/Act=%u/%u/%u UsedOld/New/Act=%d/%d/%d NextIdx=%u",
						pDrift->SyncId, PortId, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId, pDrift->Act.SeqId, 
						pDrift->Old[NextIdx].Used, pDrift->New[NextIdx].Used, pDrift->Act.Used, NextIdx);

					pDrift->Act.Used = LSA_FALSE;
					pDrift->New[NextIdx] = pDrift->Act;
					gsy_DriftCalc(pChSys, pDrift);
				}
				else
				{
					GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo() Invalid FU delay: DelayNS=%d", pTLVInfo->Param.FollowUp.DelayNS);
				}
			}
			else if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL))
			{
				GSY_SYNC_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo(%02x,FU) ignoring from Port=%u Act=%u SeqId=%u Act=%u Used=%u",
								SyncId, PortId, pDrift->Act.PortId, pTLVInfo->Param.FollowUp.SequenceID, pDrift->Act.SeqId, pDrift->Act.Used);
			}
			else
			{
				GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,"*** gsy_TLVGetInfo(%02x,FU) Port=%u SeqId=%u pDrift=NULL",
								SyncId, PortId, pTLVInfo->Param.FollowUp.SequenceID);
			}
		}
		else
		{
			/* 240907lrg002: pPdu->Sync -> pPdu->Announce
			*/
			pTLVInfo->Param.Announce.SequenceID = GSY_SWAP16(pPdu->Announce.PtcpHdr.SequenceID.Word);
			pTLVInfo->Param.Announce.Master.MacAddr = pPdu->Announce.Subdomain.MasterSourceAddress;
			pTLVInfo->Param.Announce.Master.Variance = GSY_SWAP16(pPdu->Announce.Master.ClockVariance.Word);
			pTLVInfo->Param.Announce.Master.Accuracy = pPdu->Announce.Master.ClockAccuracy;
			pTLVInfo->Param.Announce.Master.Class = pPdu->Announce.Master.ClockClass;
			pTLVInfo->Param.Announce.Master.Priority1 = pPdu->Announce.Master.MasterPriority1;
			pTLVInfo->Param.Announce.Master.Priority2 = pPdu->Announce.Master.MasterPriority2;
			pTLVInfo->Param.Announce.Master.Receipt = 1;

			GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_TLVGetInfo(%02x,AN) Port=%u SeqId=%u Len=%u",
					SyncId, PortId, pTLVInfo->Param.Announce.SequenceID, PduLength);
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

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_TLVGetInfo() pChSys=%X pPduRx=%X FrameId=0x%04x PduOk=%u", 
					pChSys, pPduRx, FrameId, PduOk);
	return(PduOk);
}

/* 310707js001****************************************************************/
/* Internal function: gsy_DriftCalcRealInterval()                            */
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
static LSA_INT32 GSY_LOCAL_FCT_ATTR gsy_DriftCalcRealInterval(
    GSY_CH_SYS_PTR  pChSys,
    GSY_DRIFT_PTR	pDrift,
    LSA_INT32	  	Interval)

{
    LSA_INT32       IntervalReal;
    LSA_INT32       IntervalSet;
    LSA_INT32       Norm = 0x40000000; /* == 2^30 */
    LSA_INT32       Help;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftCalcRealInterval() pChSys=%X pDrift=%X Interval=%d", 
				pChSys, pDrift, Interval);

    /* HW Register drift_correction exists only at ERTEC200/SOC and only for SyncID 0 (Clock)
    */
	switch (pChSys->HardwareType & GSY_HW_TYPE_MSK) 
	{
    	case GSY_HW_TYPE_ERTEC_200:
		case GSY_HW_TYPE_SOC:

			/* Driftcorrection SET (if IntervalSetValid)
			*/
            IntervalSet = pDrift->IntervalSet;

            if (( pDrift->SyncId == GSY_SYNCID_CLOCK ) &&	// Only SyncID 0
                ( pDrift->IntervalSetValid ) && 			// Interval set in EDD?
                ( IntervalSet))								// Interval <> 0 (0= no modification)
            {

                GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"--- gsy_DriftCalcRealInterval() IntervalSet=%d",IntervalSet);
                
                /* ERTEC200/SOC: drift_correction in hardware: calculate Real
                */
                if (Interval)
                {
					 /* Note: Intervals shall be > +/-1 !!! 
					 */
                    Help = (( Norm/Interval ) + ( Norm/IntervalSet ));

                    if ( Help )
                    {
                        IntervalReal = Norm / Help;

						/* Set to MIN value if lower than MIN. On 0 result the intervals are too small
						 * (one oder both is/are 1). The intervals usually are >= 4000.
						 */
                        if ( IntervalReal == 0 ) 
                        {
                            GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_DriftCalcRealInterval() Interval to small!");

                            if ( Interval < 0 ) 
                                IntervalReal = (0-GSY_DRIFT_MIN_INTERVAL);
                            else
                                IntervalReal = GSY_DRIFT_MIN_INTERVAL;
                        }
                        else
                        if (( IntervalReal > 0) && (IntervalReal < GSY_DRIFT_MIN_INTERVAL ))
                        {
                            GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalcRealInterval() Underflow. Use + Minimum");
                            IntervalReal = GSY_DRIFT_MIN_INTERVAL;
                        }
                        else
                        if (( IntervalReal < 0) && (IntervalReal > (0-GSY_DRIFT_MIN_INTERVAL) ))
                        {
                            GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalcRealInterval() Underflow. Use - Minimum");
                            IntervalReal = (0-GSY_DRIFT_MIN_INTERVAL);
                        }
                        GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,"--- gsy_DriftCalcRealInterval() IntervalReal=%d",IntervalReal);
                    }
                    else
                    {
                        /* Help = 0 means there is NO drift. Use Maximum
                        */
                        GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalcRealInterval() Overflow. Use Maximum");
                        IntervalReal = GSY_DRIFT_MAX_INTERVAL; 
                    }
                }
                else
                {
                    GSY_SYNC_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_DriftCalcRealInterval() Interval is 0");
#ifdef GSY_MESSAGE
					/* Invalid state */
#endif /* GSY_MESSAGE */
                    IntervalReal = Interval;
                }
            }
            else
            {
                /* No Interval set in EDD
                */
                IntervalReal = Interval;
            }
            break;

        case GSY_HW_TYPE_ERTEC_400:
        default:

            /* ERTEC400 and default: No correction in hardware -> Real == Interval
            */
            IntervalReal = Interval;
            break;
    }

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftCalcRealInterval() pChSys=%X RealInterval=%d", pChSys, IntervalReal);

    return(IntervalReal);
}

/*****************************************************************************/
/* Internal access function: gsy_DriftSet()                                  */
/* Set and publish new drift interval and state.                             */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DriftSet(
GSY_CH_SYS_PTR 		pChSys,
GSY_DRIFT_PTR		pDrift,
LSA_UINT8			SyncId,
LSA_INT32			Interval,
LSA_UINT8			State)
{
	GSY_LOWER_RQB_PTR_TYPE  pRbl;
	LSA_UINT16	RetVal;
	LSA_BOOL	RateValid = LSA_TRUE;
	LSA_UINT8	OldState = pDrift->State;
	LSA_INT32	OldInterval = pDrift->AveInterval;
    #if (GSY_CFG_TRACE_MODE != 0)
    LSA_UINT8	OldSyncId = pDrift->SyncId;
    #endif	

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftSet() pChSys=%X pDrift=%X Interval=%d State=%u",
					pChSys, pDrift, Interval, State);

	if (Interval < 0)
	{
		pDrift->CorrectInterval = (LSA_UINT32)(0 - Interval);
		pDrift->CorrectPlus = LSA_FALSE;
	}
	else
	{
		pDrift->CorrectInterval = (LSA_UINT32)Interval;
		pDrift->CorrectPlus = LSA_TRUE;
	}
	pDrift->SyncId = SyncId;

	GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftSet() Old/New: SyncId=%x/%x Interval=%d/%d State=%u/%u PendingSet=%u (DelayReq/Rsp=0x%08x/0x%08x)",
					OldSyncId, SyncId, OldInterval, Interval, OldState, State, pDrift->PendingSet, pChSys->PortDelayReqActive, pChSys->PortDelayRspActive);

	if ((Interval != OldInterval)
	||  pDrift->PendingSet
	||  ((State != OldState) 
	 &&  ((State == GSY_DRIFT_STATE_UNKNOWN) || (OldState == GSY_DRIFT_STATE_UNKNOWN))))
	{
		/* Publish change of interval
		*/
		pDrift->PendingSet = LSA_FALSE;
		if (GSY_SYNCID_CLOCK == pDrift->SyncIdRate)
		{
			if (GSY_HW_TYPE_SLAVE_HW != (pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))
			{
				if ((0 == pChSys->PortDelayReqActive) && (0 == pChSys->PortDelayRspActive))
				{
					/* If not just now a delay measurement is running:
					 * get RQB from queue and...
					*/
					pRbl = pChSys->QueueLowerCtrl.pFirst;
					if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
					{
						/* 290909lrg001: queue empty: save new values for retry
						*/
						pDrift->PendingState = State;
						pDrift->PendingInterval = Interval;
						pDrift->PendingSyncId = SyncId;
						pDrift->PendingSet = LSA_TRUE;
						GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftSet() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_RATE_SET=%u GSY_NUM_RQB_DELAY_SET=%u",
										GSY_NUM_RQB_RATE_SET, GSY_NUM_RQB_DELAY_SET);		//AP01366052
					}
					else
					{
						/* Send drift interval to EDD for HW based correction
						*/
						GSY_SYNC_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--> gsy_DriftSet() GSY_LOWER_RATE_SET Interval=%d State=%u", 
										Interval, State);

						/* 290909lrg001: Save old state and interval to restore after confirmation error 
						*/
						pDrift->PendingState = OldState;
						pDrift->PendingInterval = OldInterval;
						pDrift->PendingSyncId = SyncId;

						gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
						GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
                        GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
						GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
						GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_RATE_SET);
						GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
						GSY_LOWER_RQB_SET_RATE_SYNCID_SET(pRbl, GSY_SYNCID_CLOCK);
						GSY_LOWER_RQB_SET_RATE_VALUE_SET(pRbl, Interval);
			 			if (State == GSY_DRIFT_STATE_UNKNOWN)
						{
							GSY_LOWER_RQB_SET_RATE_VALID_SET(pRbl, EDD_SYNC_RATE_INVALID);
	                        RateValid = LSA_FALSE; /* 310707js001 */
						}
						else
						{
							GSY_LOWER_RQB_SET_RATE_VALID_SET(pRbl, EDD_SYNC_RATE_VALID);
						}

						/* Send lower request
						*/
						GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
					}
				}
				else
				{
					/* Just now a delay measurement is running:
					 * 290909lrg001: Save new values for retry
					*/
					pDrift->PendingState = State;
					pDrift->PendingInterval = Interval;
					pDrift->PendingSyncId = SyncId;
					pDrift->PendingSet = LSA_TRUE;
					GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "  * gsy_DriftSet() cannot GSY_LOWER_RATE_SET Interval=%d State=%u DelayReqActive=0x%08x DelayRspActive=0x%08x", 
									Interval, State, pChSys->PortDelayReqActive, pChSys->PortDelayRspActive);
				}
			}
			else
			{
				GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftSet() pChSys=%X Cannot GSY_LOWER_RATE_SET on GSY_HW_TYPE_SLAVE_HW", pChSys);
			}
		}
		else
		{
			/* Call output macro for all SyncIds except 0
			*/
	 		if (State == GSY_DRIFT_STATE_UNKNOWN)
				RateValid = LSA_FALSE;
			GSY_SYNC_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--> gsy_DriftSet() GSY_RATE_SET: Interval=%d State=%u RateValid=%u PendingSet=%u", 
							Interval, State, RateValid, pDrift->PendingSet);
			RetVal = GSY_RATE_SET(Interval, (RateValid ? GSY_DIAG_RATE_VALID : GSY_DIAG_RATE_INVALID), pDrift->SyncIdRate, pChSys->pSys);
			if (LSA_RET_OK != RetVal)
			{
				pDrift->PendingSet = LSA_TRUE;
				pDrift->PendingState = State;
				pDrift->PendingInterval = Interval;
				pDrift->PendingSyncId = SyncId;
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftSet() pChSys=%X GSY_RATE_SET() RetVal=0x%x",
								pChSys, RetVal);
			}
		}
		if (!pDrift->PendingSet)
		{
			if ((0 == Interval) || (0 == OldInterval))
			{
				GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_DriftSet(%02x) SeqId=%u Interval=%d: drift state change [%u]->[%u]", 
					pDrift->SyncIdRate, pDrift->Act.SeqId, Interval, OldState, State);
			}
			/* Update diagnosis data and send user Indication
			*/
			pDrift->State = State;
			pDrift->AveInterval = Interval;

			/* 310707js001: save currently values set within edd.
			 *              Note: better after confirmation?
			 *              No - IntervalSetValid is part of diagnosis indication
			*/
			pDrift->IntervalSet = Interval;
			pDrift->IntervalSetValid = RateValid;
			gsy_DiagUserInd(GSY_DIAG_SOURCE_DRIFT, pChSys, pDrift, 
			    			pDrift->Act.OrgSeconds      /* LSA_UINT32: MasterSeconds */, 
							pDrift->Act.OrgNanos        /* LSA_UINT32: MasterNanoseconds */, 
							0           				/* LSA_INT32: Offset */, 
							0							/* LSA_INT32: AdjustInterval */, 
							0                           /* LSA_INT32: UserValue1 */, 
							3							/* LSA_INT32: UserValue2 */, 
							0       					/* LSA_INT32: SetTimeHigh */, 
							0       					/* LSA_INT32: SetTimeLow */, 
							pDrift->Act.PortId      	/* LSA_INT16: PortId */);
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftSet() SyncId=%02x OldInterval=%d NewInterval=%d",
					SyncId, OldInterval, Interval);
}

/*****************************************************************************/
/* Internal function: gsy_DriftMedian()                                      */
/* Build median value of a sample.                                           */
/*****************************************************************************/
LSA_INT32 GSY_LOCAL_FCT_ATTR gsy_DriftMedian(
GSY_CH_SYS_PTR		pChSys,
GSY_DRIFT_PTR		pDrift)
{
	LSA_BOOL	Moved;
	LSA_INT		Idx;
	LSA_INT32	Tmp;
	LSA_INT32	Sort[GSY_DRIFT_MAX_COUNT];
	LSA_INT32	Median = 0;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftMedian() pChSys=%X pDrift=%X GSY_DRIFT_MAX_COUNT=%u",
					pChSys, pDrift, GSY_DRIFT_MAX_COUNT);

	if ((GSY_DRIFT_MAX_COUNT%2) == 0)       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to False, define-based behaviour, HM 20.05.2016
	{
		/* Even count of sample values is not supported
		*/
		GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftMedian() pChSys=%X even GSY_DRIFT_MAX_COUNT=%u not supported",
						pChSys, GSY_DRIFT_MAX_COUNT);
	}
	else
	{
		/* Coppy values sorted by time into sort array
		 * e.g. (10, -10, 2, -3, -5, 7, 1)
		*/
		for (Idx = 0; Idx < GSY_DRIFT_MAX_COUNT; Idx++)
		{
			Sort[Idx] = pDrift->AveActInterval[Idx];
		}
#ifdef GSY_MESSAGE
//		GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"->- gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} [%u]",
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
			for (Idx = 0; Idx < (GSY_DRIFT_MAX_COUNT-1); Idx++)
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
//				GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"-*- gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} :%d",
//								pDrift->SyncId, Sort[0], Sort[1], Sort[2], Sort[3], Sort[4], Sort[5], Sort[6], Idx);
			}
		} while (Moved);

		/* The Median now is the element in the middle of the sort array
		*/
		Median = Sort[(GSY_DRIFT_MAX_COUNT-1)/2];

		GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftMedian(%02x) {%d,%d,%d,%d,%d,%d,%d} =%d",
						pDrift->SyncId, Sort[0], Sort[1], Sort[2], Sort[3], Sort[4], Sort[5], Sort[6], Median);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftMedian() pChSys=%X pDrift=%X Median=%d",
					pChSys, pDrift, Median);
	LSA_UNUSED_ARG(pChSys);
	return(Median);
}

/*****************************************************************************/
/* Internal function: gsy_DriftCalc()                                        */
/* Calculate drift (rate) interval.                                          */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DriftCalc(
GSY_CH_SYS_PTR		pChSys,
GSY_DRIFT_PTR		pDrift)
{
	LSA_UINT32	DiffTimeStamp, TicksPerSecond;
	LSA_UINT32	SlaveInterval, DriftNanosU;
	LSA_INT32	MasterSeconds, MasterNanos;
	LSA_INT32	SlaveSeconds, SlaveNanos;
	LSA_INT32	DriftSeconds, DriftNanos; 
	LSA_INT32	DriftInterval = 0;
	LSA_INT32	NewAveInterval = 0;
	LSA_UINT32	Idx = 0;
	LSA_INT		NextIdx = pDrift->NextIdx;

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftCalc() pChSys=%X pDrift=%X OldSeqId=%u NewSeqId=%u",
					pChSys, pDrift, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId);

	if (!GSY_MACADDR_CMP(pDrift->OldMasterAddr, pDrift->MasterAddr))
	{
		/* 180108lrg001: if Master has changed: 
		 * Invalidate all frames of the previous master
		*/
		for (Idx = 0; Idx < GSY_DRIFT_MAX_COUNT ; Idx++)
		{
			pDrift->Old[Idx].Used = LSA_TRUE;
		}
		/* 210108lrg001: Reset Median History on DriftReset
		*/
		if (GSY_DRIFT_STATE_UNKNOWN == pDrift->State)
			pDrift->AveSumCount = 0;

		GSY_SYNC_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalc(%02x) New master: Old.Used reset", pDrift->SyncId);
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
		MasterSeconds = (LSA_INT32)(pDrift->New[NextIdx].OrgSeconds - pDrift->Old[NextIdx].OrgSeconds);
		MasterNanos   = (LSA_INT32)(pDrift->New[NextIdx].OrgNanos - pDrift->Old[NextIdx].OrgNanos);
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
		DiffTimeStamp  = pDrift->New[NextIdx].Timestamp - pDrift->Old[NextIdx].Timestamp;
		TicksPerSecond = GSY_NANOS_PER_SECOND / pChSys->RxTxNanos;
		SlaveSeconds   = (LSA_INT32)(DiffTimeStamp / TicksPerSecond);
		SlaveNanos     = (LSA_INT32)((DiffTimeStamp % TicksPerSecond) * pChSys->RxTxNanos);
		while (SlaveNanos >= GSY_NANOS_PER_SECOND)
		{
			SlaveNanos -= GSY_NANOS_PER_SECOND;
			SlaveSeconds++;
		}

		GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalc(%02x) SlaveSs:Ns=%d:%d SeqIdOld/New=%u/%u OldDelaySs:Ns=%u:%u NewDelaySs:Ns=%u:%u",
						SlaveSeconds, SlaveNanos, pDrift->SyncId, pDrift->Old[NextIdx].SeqId, pDrift->New[NextIdx].SeqId,
						pDrift->Old[NextIdx].DelaySeconds, pDrift->Old[NextIdx].DelayNanos, 
						pDrift->New[NextIdx].DelaySeconds, pDrift->New[NextIdx].DelayNanos);

		/* Delays
		*/
		SlaveSeconds -= (LSA_INT32)(pDrift->New[NextIdx].DelaySeconds);
		SlaveNanos   -= (LSA_INT32)(pDrift->New[NextIdx].DelayNanos);
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
		SlaveSeconds += (LSA_INT32)(pDrift->Old[NextIdx].DelaySeconds);
		SlaveNanos   += (LSA_INT32)(pDrift->Old[NextIdx].DelayNanos);
		while (SlaveNanos >= GSY_NANOS_PER_SECOND)
		{
			SlaveNanos -= GSY_NANOS_PER_SECOND;
			SlaveSeconds++;
		}

		if ((GSY_SECONDS_PER_DWORD < SlaveSeconds)
		||  ((GSY_SECONDS_PER_DWORD == SlaveSeconds) && (SlaveNanos > GSY_MAX_NANOS)))
		{
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_DriftCalc() pChSys=%X Slave interval too large: Seconds=%d Nanos=%d",
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
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_DriftCalc() pChSys=%X Invalid Drift: Seconds=%d Nanos=%d",
							pChSys, DriftSeconds, DriftNanos);
			}
			else
			{
				GSY_SYNC_TRACE_07(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalc(%02x) MasterSs=%d MasterNs=%d SlaveSs=%d SlaveNs=%d DriftSs=%d DriftNs=%d",
								pDrift->SyncId, MasterSeconds, MasterNanos, SlaveSeconds, SlaveNanos, DriftSeconds, DriftNanos);

				/* Calculate Interval in nanoseconds which have to be corrected by one nanosecond.
				*/
				if (DriftNanos < 0)
					DriftNanosU = (LSA_UINT32)(0 - DriftNanos);
				else
					DriftNanosU = (LSA_UINT32)DriftNanos;

				if (DriftNanosU != 0)
				{
					DriftInterval = (LSA_INT32)(SlaveInterval / DriftNanosU);

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
                DriftInterval = gsy_DriftCalcRealInterval(pChSys,pDrift,DriftInterval);

				/* Actual Drift interval overwrites oldest entry in average list
				*/
				Idx = pDrift->AveOldest;
				pDrift->AveActInterval[Idx] = DriftInterval;
				pDrift->AveOldest++;
				if (GSY_DRIFT_MAX_COUNT == pDrift->AveOldest)
					pDrift->AveOldest = 0;
				if (GSY_DRIFT_MAX_COUNT > pDrift->AveSumCount)
					pDrift->AveSumCount++;

				/* Store Minimum and Maximum
				*/
				if ((pDrift->AveMaxInterval == 0) || (pDrift->AveMaxInterval < DriftInterval))
					pDrift->AveMaxInterval = DriftInterval;
				if ((pDrift->AveMinInterval == 0) || (pDrift->AveMinInterval > DriftInterval))
					pDrift->AveMinInterval = DriftInterval;

				if (GSY_DRIFT_MAX_COUNT <= pDrift->AveSumCount)
				{
					/* Build Median of sample, if the list is full
					*/
					NewAveInterval = gsy_DriftMedian(pChSys, pDrift);
				}

				GSY_SYNC_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalc(%02x) DriftInterval=%d NewAveInterval=%d AveInterval=%d AveOldest=%d AveSumCount=%u AveMinInterval=%d AveMaxInterval=%d",
								pDrift->SyncId, DriftInterval, NewAveInterval, pDrift->AveInterval, pDrift->AveOldest, pDrift->AveSumCount, 
								pDrift->AveMinInterval, pDrift->AveMaxInterval);

				/* If enough single values have build the average value... 
				*/
				if ((GSY_DRIFT_MAX_COUNT <= pDrift->AveSumCount)
				&&  (NewAveInterval != 0))
				{
					/* ...set new average value for drift correction interval
					*/
					gsy_DriftSet(pChSys, pDrift, pDrift->SyncId, NewAveInterval, GSY_DRIFT_STATE_READY);  
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
	if (GSY_DRIFT_MAX_COUNT <= pDrift->NextIdx)
		pDrift->NextIdx = 0;

	GSY_SYNC_TRACE_09(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftCalc(%02x) MACadr=%02x-%02x-%02x-%02x-%02x-%02x AveInterval=%d ActInterval=%d",
					pDrift->SyncId, pDrift->MasterAddr.MacAdr[0], pDrift->MasterAddr.MacAdr[1], pDrift->MasterAddr.MacAdr[2],
					                pDrift->MasterAddr.MacAdr[3], pDrift->MasterAddr.MacAdr[4], pDrift->MasterAddr.MacAdr[5],
					pDrift->AveInterval, DriftInterval);

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftCalc() pChSys=%X Correction=%d CorrectPlus=%u State=%u",
					pChSys, pDrift->CorrectInterval, pDrift->CorrectPlus, pDrift->State);
}

/*****************************************************************************/
/* Internal function: gsy_DriftDecray()                                      */
/* Reduce the correction of the local clock slowly until 0 by increasing     */
/* of the RCF interval and using it as OCF interval.                         */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DriftDecray(
GSY_CH_SYS_PTR  	pChSys,
GSY_DRIFT_PTR		pDrift)
{
	LSA_UINT8	SyncId = pDrift->SyncId;
	LSA_UINT16	RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftDecray() pChSys=%X pDrift=%X SyncId=0x%02x", 
					pChSys, pDrift, SyncId);

	if ((pDrift->CorrectInterval != 0)
	&&  (pDrift->CorrectInterval < GSY_DRIFT_DECRAY_MAX))
	{
		/* Slowly increase the RCF interval
		*/
		gsy_DriftSet(pChSys, pDrift, SyncId, (pDrift->AveInterval * GSY_DRIFT_DECRAY_FACTOR), pDrift->State);  
	}
	else
	{
		/* End of the transient phase: Masterdrift = 0
		*/
		gsy_DriftSet(pChSys, pDrift, SyncId, 0, GSY_DRIFT_STATE_MASTER);  
	}

	GSY_SYNC_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"--- gsy_DriftDecray(%02x) State=%u Interval=%d",
					SyncId, pDrift->State, pDrift->AveInterval);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL))
	{
		/* Set the OCF interval to the RCF interval by reset of the controller
		*/
		RetVal = gsy_SyncReset(pDrift->AveInterval, SyncId, pChSys->pChUsr);
		if (LSA_RET_OK != RetVal)
		{
			GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftDecray() pChSys=%X pChUsr=%X SyncId=0x%02x: gsy_SyncReset: RetVal=0x%x",
							pChSys, pChSys->pChUsr, SyncId, RetVal);
		}
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DriftDecray() pChSys=%X pChSys->pChUsr=%X SyncId=0x%02x",
						pChSys, pChSys->pChUsr, SyncId);
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftDecray() pChSys=%X pDrift->State=%u AveInterval=%d", 
					pChSys, pDrift->State, pDrift->AveInterval);
}

/*****************************************************************************/
/* Internal access function: gsy_DriftReset()                                */
/* Free drift strukture for usage of another master.                         */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DriftReset(
GSY_CH_SYS_PTR  	pChSys,
GSY_DRIFT_PTR		pDrift,
LSA_UINT8			SyncId)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftReset() pChSys=%X pDrift=%X SyncId=%u", 
					pChSys, pDrift, SyncId);

	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChUsr, LSA_NULL)
	&&  (pChSys->pChUsr->SyncId == SyncId))
	{
		/* If on the related user channel is synchronisation active:
		 * allow drift measurement only to masters of the same subdomain
		*/
		pDrift->Subdomain = pChSys->pChUsr->SyncData.SubdomainID;
	}
	else
	{
		GSY_MEMSET_LOCAL(&pDrift->Subdomain, 0, sizeof(GSY_PTCP_UUID));
	}
	GSY_SYNC_TRACE_06(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_DriftReset() pChSys=%X pDrift=%X SyncId=%u SubdomainUUID=0x%08x, 0x%04x...0x%02x", 
					  pChSys, pDrift, SyncId, 
					  GSY_SWAP32(pDrift->Subdomain.Data1.Dword), GSY_SWAP16(pDrift->Subdomain.Data2.Word), pDrift->Subdomain.Data4[7]);

	gsy_DriftSet(pChSys, pDrift, GSY_SYNCID_NONE, 0, GSY_DRIFT_STATE_UNKNOWN);  

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftReset() SubdomainUUID=0x%08x, 0x%04x...0x%02x, 0x%02x", 
					GSY_SWAP32(pDrift->Subdomain.Data1.Dword), GSY_SWAP16(pDrift->Subdomain.Data2.Word), 
					pDrift->Subdomain.Data4[6], pDrift->Subdomain.Data4[7]);
}

/*****************************************************************************/
/* Internal access function: gsy_DriftAdjust()                               */
/* Timer controlled funktion to reset drift on timeout and retry setting     */
/* pending drift correction interval.                                        */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DriftAdjust(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT8			SyncId;
	GSY_DRIFT_PTR		pDrift = LSA_NULL;
	GSY_MAC_ADR_TYPE	NoMacAddr = {GSY_NO_MASTER_ADDR};
	LSA_INT32			RetVal = LSA_RET_OK;

	GSY_FUNCTION_TRACE_03(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftAdjust() pChSys=%X pTimer=%X Id=%u", 
					pTimer->pChSys, pTimer, pTimer->Id);

	if (pTimer->pChSys->State == GSY_CHA_STATE_OPEN)
	{
		/* Step through drift table * 190906lrg001
		*/
		for (SyncId = 0; SyncId < GSY_MAX_PROTOCOLS; SyncId++)
		{
			pDrift = &pTimer->pChSys->Drift[SyncId][0];
			if (pDrift->PendingSet)
			{
				/* 290909lrg001: Retry setting the rate, 1684006: use PendingSyncId
				*/
				GSY_SYNC_TRACE_06(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"--- gsy_DriftAdjust(%02x) pChSys=%X pDrift=%X: Retry setting rate: pending SyncId=%u interval=%d state=%u", 
					  SyncId, pTimer->pChSys, pDrift, pDrift->PendingSyncId, pDrift->PendingInterval, pDrift->PendingState);

				gsy_DriftSet(pTimer->pChSys, pDrift, pDrift->PendingSyncId, pDrift->PendingInterval, pDrift->PendingState);  
			}
			if ((pDrift->SyncId != GSY_SYNCID_NONE)
			&&  ((pDrift->SyncId > GSY_SYNCID_TIME)
			 ||  (GSY_HW_TYPE_SLAVE_HW != (pTimer->pChSys->HardwareType & GSY_HW_TYPE_SLAVE_MSK))))
			{
				if (GSY_DRIFT_STATE_TRANSIENT == pDrift->State)
				{
					/* Slowly decray RCF interval
					*/
					gsy_DriftDecray(pTimer->pChSys, pDrift);
				}
				else if (!((GSY_DRIFT_STATE_MASTER == pDrift->State)
					 ||  (pDrift->PendingSet && (GSY_DRIFT_STATE_MASTER == pDrift->PendingState))))	//151210lrg001
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
						GSY_ERROR_TRACE_03(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,"*** gsy_DriftAdjust() pChSys=%X SyncId=0x%02x OldSeqId=%u: No new sync frame",
										pTimer->pChSys, SyncId, pDrift->OldSeqId);
						gsy_DriftReset(pTimer->pChSys, pDrift, SyncId);
						pDrift->MasterAddr = NoMacAddr;
					}
					else
					{
						/* Store actual SeqId
						*/
						pDrift->OldSeqId = pDrift->Act.SeqId;
					}
				}
			}
		}
	}
	else if (GSY_CHA_STATE_CLOSED == pTimer->pChSys->State)
	{
		/* Channel is closing:
		 * try again to free all resources
		*/
		RetVal = gsy_ChSysClean(pTimer->pChSys);
		if (LSA_RET_OK == RetVal)
		{
			/* Give back upper RQB and free system channel structure
			*/ 
			gsy_CallbackSys(pTimer->pChSys, LSA_NULL, pTimer->pChSys->Resp);
			gsy_ChSysFree(pTimer->pChSys);
		}
	}

	GSY_FUNCTION_TRACE_00(pTimer->pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftAdjust()");
}

/*****************************************************************************/
/* Internal access function: gsy_DriftCorrection()                           */
/* Returns time in ns, to correct the given Interval (ns).                   */
/*****************************************************************************/
LSA_INT32 GSY_LOCAL_FCT_ATTR gsy_DriftCorrection(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT32				Interval,
LSA_UINT32				SyncId)
{
	GSY_DRIFT_PTR	pDrift = LSA_NULL;
	LSA_UINT32		DriftCorrection = 0;
	LSA_UINT32		Rest; 
	LSA_INT32		RetVal = 0;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DriftCorrection() pChSys=%X Interval=%u SyncId=0x%08x", 
					pChSys, Interval, SyncId);

	/* Find drift structure in table * 190906lrg001: only one structure (drift master) for one SyncId
	*/
	if (SyncId < GSY_MAX_PROTOCOLS)
	{
		if (pChSys->Drift[SyncId][0].SyncId == (LSA_UINT8)SyncId)
			pDrift = &pChSys->Drift[SyncId][0];
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_DriftCorrection() pChSys=%X Interval=%u Invalid SyncId=0x%08x", 
					pChSys, Interval, SyncId);
	}

    /* 310707js001: Do nothing here if the hardware does the drift correction
    */
    if (SyncId == GSY_SYNCID_CLOCK )
    {
	    switch (pChSys->HardwareType & GSY_HW_TYPE_MSK) 
	    {
        	case GSY_HW_TYPE_ERTEC_200:
		    case GSY_HW_TYPE_SOC:
                Interval = 0; /* set interval to 0 so no correction is applied */
                break;
            default:
                break;
        }
    }


	if (!LSA_HOST_PTR_ARE_EQUAL(pDrift, LSA_NULL)
	&&  (pDrift->CorrectInterval != 0) && (Interval != 0))
	{

		DriftCorrection = pDrift->CorrectInterval;

		RetVal = (LSA_INT32)(Interval / DriftCorrection);
		Rest   = Interval % DriftCorrection;
		if (pDrift->CorrectPlus)
		{
			/* Positive correction with round up
			*/
			if (Rest >= (DriftCorrection / 2))
				RetVal++;
		}
		else
		{
			/* Negative correction
			*/
			RetVal = 0 - RetVal;
			if (Rest >= (DriftCorrection / 2))
				RetVal--;
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DriftCorrection() pDrift=0x%x CorrectInterval=%u RetVal=%d", 
					pDrift, DriftCorrection, RetVal);

	return(RetVal);
}

/*****************************************************************************/
/* Internal access function: gsy_DelaySet()                                  */
/* Send port delay values to FWD, EDD and User                               */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelaySet(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId)
{
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelaySet() pChSys=%X Port=%u LineDelay=%u", 
				pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay);

	GSY_DEL_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_DelaySet() pChSys=%X Port=%u LineDelay=%u CableDelay=%u", 
				pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay, pChSys->Port[PortId-1].CableDelay);

	pChSys->Port[PortId-1].DelaySetPending = LSA_FALSE;
	
	if (!LSA_HOST_PTR_ARE_EQUAL(pChSys->pChAnno, LSA_NULL)
	&&  (GSY_CHA_STATE_OPEN == pChSys->pChAnno->State))
	{
		/* Set LineDelay in forwarding module
		*/
		(LSA_VOID)gsy_FwdLineDelaySet(pChSys, PortId, pChSys->Port[PortId-1].SyncLineDelay);
	}

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
	if (GSY_HW_TYPE_DELAY_SW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
	{
		/* Set LineDelay in lower layer
		*/
		gsy_LowerDelaySet(pChSys, PortId);
	}
#endif
	gsy_DiagUserInd(GSY_DIAG_SOURCE_DELAY, pChSys, LSA_NULL, 
	    			0       /* LSA_UINT32: MasterSeconds */, 
					0       /* LSA_UINT32: MasterNanoseconds */, 
					0       /* LSA_INT32: Offset */, 
					0       /* LSA_INT32: AdjustInterval */, 
					0       /* LSA_INT32: UserValue1 */, 
					0 	    /* LSA_INT32: UserValue2 */, 
					0       /* LSA_INT32: SetTimeHigh */, 
					0       /* LSA_INT32: SetTimeLow */, 
					PortId  /* LSA_INT16: PortId */);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelaySet() pChSys=%X Port=%u CableDelay=%u", 
				pChSys, PortId, pChSys->Port[PortId-1].CableDelay);
}

#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
/* js040806 ******************************************************************/
/* Internal function: gsy_DelayTempToActual()                                */
/* Copies the temporary delay values of one port to the actual and sets them */
/* if they are valid (DelayValid).                                           */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayTempToActual(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId)
{
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT32	OldDelay = pChSys->Port[Idx].SyncLineDelay;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayTempToActual() pChSys=%X OldDelay=%d Port=%d", 
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
		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE," -- gsy_DelayTempToActual() pChSys=%X Port=%u OldDelay=%u TempDelay=%u CableDelay=%u CableDelayTemp=%u PortDelay=%u FilterDelayWeight=%u",
			pChSys, PortId, OldDelay, pChSys->Port[Idx].SyncLineDelayTemp, pChSys->Port[Idx].CableDelay,pChSys->Port[Idx].CableDelayTemp,pChSys->Port[Idx].PortDelayTemp,pChSys->Port[Idx].FilterDelayWeight);
#else
		pChSys->Port[Idx].SyncLineDelay = pChSys->Port[Idx].SyncLineDelayTemp;
		pChSys->Port[Idx].CableDelay = pChSys->Port[Idx].CableDelayTemp;
#endif
		/* 240608lrg001: Now set DelayValid = LSA_FALSE
		*/
		pChSys->Port[Idx].DelayValid = LSA_FALSE;
		pChSys->Port[Idx].MinDelay = pChSys->Port[Idx].MinDelayTemp;
		pChSys->Port[Idx].MaxDelay = pChSys->Port[Idx].MaxDelayTemp;

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,"  - gsy_DelayTempToActual() pChSys=%X Port=%u OldDelay=%u LineDelay=%u CableDelay=%u DelaySetPending=%u SendCnfPending=%u [%x]", 
			pChSys, PortId, OldDelay, pChSys->Port[PortId-1].SyncLineDelay, pChSys->Port[PortId-1].CableDelay,pChSys->Port[Idx].DelaySetPending,pChSys->Port[Idx].SendCnfPending,0);

		/* If the new delay value differs from the old one: set it in FWD, EDD...
		*/
		if (OldDelay != pChSys->Port[Idx].SyncLineDelay)
		{
			/* 240608lrg001: Check wether we have to wait for send confirmation
			*/
			if (0 == pChSys->Port[Idx].SendCnfPending)
			{
				if ((0 == OldDelay) || (0 == pChSys->Port[Idx].SyncLineDelay))
				{
					GSY_SYNC_TRACE_05(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_DelayTempToActual() pChSys=%X Port=%u SeqId=%u: delay state change [%u]->[%u]", 
						pChSys, PortId, pChSys->Port[Idx].ActDelay.ReqSeqId, OldDelay, pChSys->Port[Idx].SyncLineDelay);
				}
				/* Set new delay
				*/
				gsy_DelaySet(pChSys, PortId);
			}
			else
			{
				pChSys->Port[Idx].DelaySetPending = LSA_TRUE;
				GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,"  - gsy_DelayTempToActual() pChSys=%X Port=%u DelaySetPending=%u SendCnfPending=%u", 
 						pChSys, PortId, pChSys->Port[Idx].DelaySetPending, pChSys->Port[Idx].SendCnfPending);
			}
		}
	}

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayTempToActual() pChSys=%X DelaySetPending=%u SendCnfPending=%u", 
				pChSys, pChSys->Port[Idx].DelaySetPending, pChSys->Port[Idx].SendCnfPending);
}

/*****************************************************************************/
/* Internal function: gsy_RcfCorrection()                                    */
/* Calculate the correction value for the given RCF interval                 */
/*****************************************************************************/
static LSA_INT32 GSY_LOCAL_FCT_ATTR gsy_RcfCorrection(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT32				Interval,
LSA_UINT16				PortIdx)
{
	LSA_INT32		RetVal = 0;
	LSA_UINT32		Rest = 0, CorrectU = 0;
	GSY_RCF_LOCAL *	pRcf = &pChSys->Port[PortIdx].ActDelay.RcfLocal;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_RcfCorrection() pChSys=%X Port=%u Interval=%u", 
					pChSys, (PortIdx+1), Interval);

	if ((pRcf->Interval != 0) && (Interval != 0))
	{
		if (pRcf->Interval >= 0)
			CorrectU = (LSA_UINT32)(pRcf->Interval);
		else
			CorrectU = (LSA_UINT32)(-pRcf->Interval);

		RetVal = (LSA_INT32)(Interval / CorrectU);
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

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_RcfCorrection() CorrectU=%d Rcf.Interval=%d RetVal=%d", 
					CorrectU, pRcf->Interval, RetVal);

	return(RetVal);
}

/*****************************************************************************/
/* Internal function: gsy_RcfCalc()                                          */
/* Calculate RCF interval for a port from 2 sequent delay measurements       */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_RcfCalc(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				SeqId,
LSA_UINT16				PortIdx)
{
	LSA_UINT32		T1 = 0, T2 = 0, TdiffU = 0, Interval = 0;
	LSA_INT			Tdiff = 0;
	GSY_RCF_LOCAL *	pRcf = &pChSys->Port[PortIdx].ActDelay.RcfLocal;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_RcfCalc() pChSys=%X Port=%u SeqId=%u", 
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
				TdiffU = T2 - T1;
			Interval = T1 / TdiffU;

			/* Round
			*/
			if ((T1 % TdiffU) >= (TdiffU / 2))
				Interval++;

			if (Interval < GSY_DRIFT_MIN_INTERVAL)
			{
				/* Use Minimum as replacement value
				*/
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** gsy_RcfCalc() Port=%u SeqId=%u Interval=%u: using GSY_DRIFT_MIN_INTERVAL", 
						(PortIdx+1), SeqId, Interval);
				Interval = GSY_DRIFT_MIN_INTERVAL;
			}

			/* Restore sign
			*/
			if (Tdiff < 0)
				pRcf->Interval = (LSA_INT32)(0 - Interval);
			else
				pRcf->Interval = (LSA_INT32)Interval;

			GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE," -- gsy_RcfCalc(%u) State=%u SeqId=%u T1=%u T2=%u TdiffU=%u Tdiff=%d Interval=%u Rcf.Interval=%d [%x%x]", 
						(PortIdx+1), pRcf->State, SeqId, T1, T2, TdiffU, Tdiff, Interval, pRcf->Interval, 0, 0);
		}
		pRcf->State = GSY_DRIFT_STATE_READY;
	}
	else
	{
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** gsy_RcfCalc() Cannot calc RCF interval for Port=%u: State=%u SeqId=%u", 
				(PortIdx+1), pRcf->State, SeqId);
	}

	GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "--- gsy_RcfCalc(%u) State=%u SeqId=%u SeqOld/Act=%u/%u T1Old/Act=%u/%u  T2Old/Act=%u/%u Interval=%d Tdiff=%d", 
				(PortIdx+1), pRcf->State, SeqId, pRcf->SeqOld, pRcf->SeqAct, pRcf->T1Old, pRcf->T1Act, pRcf->T2Old, pRcf->T2Act, pRcf->Interval, Tdiff);

	/* Store acual values to old values for the next calculation
	*/
	pRcf->SeqOld = pRcf->SeqAct;
	pRcf->T1Old = pRcf->T1Act;
	pRcf->T2Old = pRcf->T2Act;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_RcfCalc() Port=%u State=%u Interval=%d", 
					(PortIdx+1), pRcf->State, pRcf->Interval);
}

/*****************************************************************************/
/* Internal function: gsy_DelayCalc()                                        */
/* Calculate line delay and cable delay for a port                           */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayCalc(
GSY_CH_SYS_PTR  		pChSys,
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
	LSA_BOOL	DelayValid = LSA_TRUE;	//AP01262791: 0 now is valid for CableDelay

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayCalc() pChSys=%X ReqTicks=%u RxTxNanos=%u", 
					pChSys, pChSys->Port[Idx].ActDelay.ReqDelay, pChSys->RxTxNanos);

	/* Responder bridge delay is in nanoseconds
	*/
	RspDelayNanos = (LSA_UINT32)(pChSys->Port[Idx].ActDelay.RspDelayNS);

	/* 030407lrg001: Calculate local RCF interval
	 * 270907lrg003: GSY_DRIFT_STATE_NONE removed
	*/
	gsy_RcfCalc(pChSys, SeqId, Idx);

	if (GSY_DRIFT_STATE_WAIT != pChSys->Port[Idx].ActDelay.RcfLocal.State)
	{
		/* 050907lrg002: Calculate delay only if local RCF interval is known
		 * 270907lrg001
		*/
		DriftNanos = gsy_RcfCorrection(pChSys, RspDelayNanos, Idx);
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
			RspDelayNanos -= DriftNanos;    //lint !e737 Loss of sign, this code is correct, a cast is wrong, HM 13.06.2016
		}

		/* Do not correct the requestor time by drift
		*/
		ReqDelayNanos =	pChSys->Port[Idx].ActDelay.ReqDelay * pChSys->RxTxNanos;

		/* Build difference of requestor and responder time and sum of the port delays
			* and calculate cable delay
		*/
		CableDelay = (LSA_INT32)(ReqDelayNanos - RspDelayNanos);
		PortDelay  = pChSys->Port[Idx].TxDelay
					+ pChSys->Port[Idx].ActDelay.RspRxPortDelay
					+ pChSys->Port[Idx].ActDelay.RspTxPortDelay
					+ pChSys->Port[Idx].RxDelay;

		CableDelay = (CableDelay - (LSA_INT32)PortDelay) / 2;

		GSY_DEL_TRACE_11(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "  - gsy_DelayCalc(%u) SeqId=%u Cable=%d Line=%d T1=%u T4=%u T2=%u T3=%u Req=%u Rsp=%u Drift=%d", 
			PortId, SeqId, CableDelay, CableDelay+(LSA_INT32)pChSys->Port[Idx].ActDelay.RspTxPortDelay+(LSA_INT32)pChSys->Port[Idx].RxDelay,
			pChSys->Port[Idx].TxDelay, pChSys->Port[Idx].RxDelay, pChSys->Port[Idx].ActDelay.RspRxPortDelay, pChSys->Port[Idx].ActDelay.RspTxPortDelay, 
			ReqDelayNanos, RspDelayNanos, DriftNanos);

		if ((RspDelayNanos == 0)			//AP01527161
		||  ((ReqDelayNanos > RspDelayNanos)
		 &&  (RspDelayNanos != GSY_PTCP_DELAY_NS_INVALID)	//140710lrg001
		 &&  ((GSY_RSPDELAY_MAX == 0) || (GSY_RSPDELAY_MAX >= RspDelayNanos))))       //lint !e506 !e774 Constant Boolean within 'if' always evaluates to False, define-based behaviour, HM 20.05.2016
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
                if (0 != Cnt)
                {
                    CableDelay = Sum / Cnt;
                }
                else 
                { 
                    CableDelay = 0; 
                }

				if (RspDelayNanos == 0) //AP01527161: Set cable delay to 0 
				{
					pChSys->Port[Idx].CableDelayTemp = 0;
				}
				else if (CableDelay > 0)
				{
					pChSys->Port[Idx].CableDelayTemp = (LSA_UINT32)CableDelay;
				}
				else if ((!pChSys->Port[Idx].Is_gPTP) || (CableDelay >= GSY_DELAY_CABLE_MIN))   //RQ 1628273: Skip -50nsec test for PTCP
				{
					LTRC_LEVEL_TYPE Tracelevel = LSA_TRACE_LEVEL_NOTE;		// 2133976
					if (CableDelay < -50)
					{
						Tracelevel = LSA_TRACE_LEVEL_WARN;
					}
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, Tracelevel, "*** gsy_DelayCalc() Port=%u SeqId=%u: setting CableDelay=%d to 1",
							PortId, SeqId, CableDelay);
					LSA_UNUSED_ARG(Tracelevel);
					pChSys->Port[Idx].CableDelayTemp = 1;
				}
				else
				{
					/* AP01262791: average cable delay < -50ns
					*/
					DelayValid = LSA_FALSE;
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** gsy_DelayCalc() Port=%u SeqId=%u: invalid average CableDelay=%d",
							PortId, SeqId, CableDelay);
				}
				if (DelayValid)
				{
					LineDelay = CableDelay
							  + (LSA_INT32)(pChSys->Port[Idx].ActDelay.RspTxPortDelay)
						   	  + (LSA_INT32)(pChSys->Port[Idx].RxDelay);

					if (LineDelay < 1)
					{
						GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,  "*** gsy_DelayCalc() Port=%u SeqId=%u: setting LineDelay=%d to 1", 
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
						pChSys->Port[Idx].FilterDelayTemp = (LSA_UINT32)(LineDelay);
						pChSys->Port[Idx].FilterDelayTemp *= GSY_DELAY_FILTER_FACTOR;
						pChSys->Port[Idx].FilterDelayWeight = GSY_DELAY_FILTER_MIN;
					}
					else
					{
						/* Filter already active: use and modify it
						*/
						LSA_UINT64 help = (LSA_UINT32)(LineDelay);
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
			}
			/* AP01527161: no tolerance check
			else
			{
				// Delay too large or too small
				//
				GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_WARN,  "*** gsy_DelayCalc() Invalid delay: Port=%u SeqId=%u CableDelay=%d", 
							PortId, SeqId, CableDelay);
			}
			*/
		}
		else
		{
			/* 190506lrg001: Response delay to large or invalid
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** gsy_DelayCalc() Invalid response delay: Port=%u SeqId=%u RespDelay=%u", 
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

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayCalc() SeqId=%u AvCableDelay=%u AvLineDelay=%u", 
					SeqId, pChSys->Port[Idx].CableDelayTemp, pChSys->Port[Idx].SyncLineDelayTemp);

	LSA_UNUSED_ARG(SeqId);

}

/*****************************************************************************/
/* Internal function: gsy_DelayReqFill()                                     */
/* Fill values into delay request frame                                      */
/*****************************************************************************/
static LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_DelayReqFill(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	LSA_UINT16			SeqId;
	LSA_UINT16			Idx = PortId - 1;
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_DELAY};
	
	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqFill() pChSys=%X pPdu=%X Port=%d", 
					pChSys, pPdu, PortId);

	/* Increment SequenceID to send
	*/
	pChSys->Port[Idx].DelayReqSeqId++;
	SeqId = pChSys->Port[Idx].DelayReqSeqId;

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_DELAY_REQ_PDU_SIZE, 0);
	
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

	pPdu->PtcpData.DelayReq.PtcpHdr.SequenceID.Word = GSY_SWAP16(SeqId);

	pPdu->PtcpData.DelayReq.DelayParam.RequestSourceAddress = pChSys->Port[Idx].MACAddr;
	pPdu->PtcpData.DelayReq.DelayParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_DELAYPARAM_TYPELEN);
	pPdu->PtcpData.DelayReq.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqFill() pChSys=%X pPdu=%X SeqId=%d", 
					pChSys, pPdu, SeqId);
	return(SeqId);
}

/*****************************************************************************/
/* Internal function: gsy_DelayRspFill()                                     */
/* Fill values into delay response frame                                     */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayRspFill(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				PortId,
LSA_UINT16				SeqId,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_DELAY};
	LSA_UINT16			Idx = PortId - 1;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayRspFill() pChSys=%X pPdu=%X Port=%u SeqId=%u", 
					pChSys, pPdu, PortId, GSY_SWAP16(SeqId));

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_DELAY_RSP_PDU_SIZE, 0);
	
	pPdu->PduHdr.SrcMacAdr = pChSys->Port[Idx].MACAddr;
	pPdu->PduHdr.DstMacAdr = DstAddr;
#ifdef GSY_USE_VLAN
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPdu->PduHdr.VlanTag = VlanTag;
	}
#endif
	pPdu->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
	pPdu->PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_FURSP);

	pPdu->PtcpData.DelayRsp.PtcpHdr.SequenceID.Word = SeqId;

	/* Send local port delays
	*/
	pPdu->PtcpData.DelayRsp.PortParam.T2PortRxDelay.Dword = GSY_SWAP32(pChSys->Port[Idx].RxDelay);
	pPdu->PtcpData.DelayRsp.PortParam.T3PortTxDelay.Dword = GSY_SWAP32(pChSys->Port[Idx].TxDelay);
	pPdu->PtcpData.DelayRsp.PortParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_PORTPARAM_TYPELEN);

	pPdu->PtcpData.DelayRsp.DelayParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_DELAYPARAM_TYPELEN);
	pPdu->PtcpData.DelayRsp.PortTime.TypeLen.Word = GSY_SWAP16(GSY_TLV_PORTTIME_TYPELEN);
	pPdu->PtcpData.DelayRsp.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayRspFill()");
}

/*****************************************************************************/
/* Internal function: gsy_DelayFuFill()                                      */
/* Fill values into delay followup frame                                     */
/*****************************************************************************/
static LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayFuFill(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				PortId,
LSA_UINT16				SeqId,
GSY_LOWER_TX_PDU_PTR	pPdu)
{
	GSY_MAC_ADR_TYPE	DstAddr = {GSY_MULTICAST_ADDR_DELAY};
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayFuFill() pChSys=%X pPdu=%X Port=%d SeqId=%u", 
					pChSys, pPdu, PortId, GSY_SWAP16(SeqId));

	GSY_LOWER_TXMEM8_SET(pPdu, GSY_DELAY_FU_PDU_SIZE, 0);
	
	pPdu->PduHdr.SrcMacAdr = pChSys->Port[PortId-1].MACAddr;
	pPdu->PduHdr.DstMacAdr = DstAddr;
#ifdef GSY_USE_VLAN
	{
		GSY_VLAN_TAG_TYPE	VlanTag = {GSY_VLAN_TAG};
		pPdu->PduHdr.VlanTag = VlanTag;
	}
#endif
	pPdu->PduHdr.EthType.Word = GSY_SWAP16(GSY_ETHETYPE);
	pPdu->PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_FU);

	pPdu->PtcpData.DelayFu.PtcpHdr.SequenceID.Word = SeqId;
	pPdu->PtcpData.DelayFu.DelayParam.TypeLen.Word = GSY_SWAP16(GSY_TLV_DELAYPARAM_TYPELEN);
	pPdu->PtcpData.DelayFu.End.TypeLen.Word = GSY_SWAP16(GSY_TLV_END_TYPELEN);

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayFuFill()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqInd()                               */
/* Receive a delay request frame                                             */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqInd(
GSY_CH_SYS_PTR  		    pChSys,
LSA_UINT16				    PortId,
const GSY_LOWER_RX_PDU_PTR	pPduRx,
LSA_UINT16				    Length,
LSA_UINT32				    TimeStamp)
{
	GSY_LOWER_TX_PDU_PTR	pPdu;
	GSY_LOWER_RQB_PTR_TYPE	pRbl = LSA_NULL;
	GSY_DRIFT_PTR			pDrift = LSA_NULL;
	LSA_UINT32				PortBit = 1;
	LSA_UINT64				Nanos;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqInd() pChSys=%X Port=%u Length=%u TimeStamp=%u", 
					pChSys, PortId, Length, TimeStamp);

	/* 301007lrg001: Ignore Delay Request if PeerBoundary is set at the port
	 * 040608lrg002: do not longer ignore delay request ;-)
	if (!pChSys->Port[PortId-1].DelayBoundary)
	 * 291008lrg001: Response active seperately configurable
	*/
	if (pChSys->Port[PortId-1].DelayRspActive)
	{
		/* If there is a send RQB for the response in the Lower Queue
		 * and the received delay request is a new one (without Subdomain) and is not too short...
		*/
		if ((0 < pChSys->QueueLowerSend.Count)
		&&  (GSY_TLV_DELAYPARAM_TYPELEN == GSY_SWAP16(pPduRx->DelayReq.DelayParam.TypeLen.Word))
		&&  (GSY_DELAY_REQ_RX_SIZE <= Length))
		{
			/* Get Lower Send-RQB from Queue and build response frame
			*/
			pRbl = pChSys->QueueLowerSend.pFirst;
			gsy_DequeLower(&pChSys->QueueLowerSend, pRbl);
			pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);

			gsy_DelayRspFill(pChSys, PortId, pPduRx->DelayReq.PtcpHdr.SequenceID.Word, pPdu);

			/* Send back the delay parameters of the request
			*/
			pPdu->PtcpData.DelayRsp.DelayParam = pPduRx->DelayReq.DelayParam;

			/* 030407lrg001: Calculate T2TimeStamp in nanoseconds with 32Bit WrapAround 
			 * from local receive timestamp in ticks and put it into GSY_TLV_PORTTIME
			 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
			*/
			Nanos = (LSA_UINT64)TimeStamp * (LSA_UINT64)pChSys->RxTxNanos;
			Nanos = Nanos & 0xFFFFFFFF;	// was: % GSY_NANOS_PER_DWORD;

			pPdu->PtcpData.DelayRsp.PortTime.T2TimeStamp.Dword = GSY_SWAP32((LSA_UINT32)Nanos);
			pPdu->PtcpData.DelayRsp.PtcpHdr.Delay10ns.Dword = 0;

#ifdef GSY_MESSAGE
			/* 180107lrg001: Test without FU: simulate bridge delay
			pPdu->PduHdr.FrameId.Word = GSY_SWAP16(GSY_FRAMEID_DELAY_RSP);
			pPdu->PtcpData.DelayRsp.PtcpHdr.DelayNS.Dword = GSY_SWAP32(720000);
			*/
#endif /* GSY_MESSAGE */

			/* 200707lrg002: Delay measurement at this port must not 
			 * be disturbed by setting the rate: block it
			*/
			PortBit <<= (PortId-1);
			pChSys->PortDelayRspActive |= PortBit;

			/* Send delay response with Timestamp 
			 * and DriftPtr or LSA_NULL in User structure
			*/
			GSY_LOWER_RQB_USER_PTR_SET(pRbl, pDrift);
			gsy_DataSend(pChSys, pRbl, PortId, GSY_DELAY_RSP_PDU_SIZE, LSA_TRUE);
		}
		else
			/* ...Trace error and ignore it
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayReqInd() Send queue empty or invalid PDU: pRbl=%X DelayParam.TypeLen=0x%04x Length=%u", 
						pRbl, GSY_SWAP16(pPduRx->DelayReq.DelayParam.TypeLen.Word), Length);
	}
		/* ...Trace error and ignore it, 040608lrg002: not an error
	else
		GSY_ERROR_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE,  "*** gsy_DelayReqInd() Peer Boundary set at Port=%u", 
					PortId);
		*/

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqInd()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayRspCnf()                               */
/* A Delay response frame has been sent: send FollowUp now                   */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayRspCnf(
GSY_CH_SYS_PTR  		    pChSys,
LSA_UINT16				    Response,
LSA_UINT16				    PortId,
const GSY_LOWER_TX_PDU_PTR	pPduTx,
LSA_UINT32				    TimeStamp)
{
	GSY_LOWER_TX_PDU_PTR	pPdu;
	GSY_LOWER_RQB_PTR_TYPE	pRbl = LSA_NULL;
	LSA_UINT32				NanoSecs = 0;
	LSA_UINT32				PortBit = 1;
	LSA_UINT32				T2Time;
	LSA_UINT64				Nanos;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayRspCnf() pChSys=%X Port=%u Response=0x%x TimeStamp=%u", 
					pChSys, PortId, Response, TimeStamp);

	/* 200707lrg002: Delay measurement at this port must not 
	 * be disturbed by setting the rate: deblock it
	*/
	PortBit <<= (PortId-1);
	pChSys->PortDelayRspActive &= ~PortBit;

	/* If response was sent successfully and a send RQB is in the Lower Queue...
	*/

	if ((Response == GSY_LOWER_RSP_OK) && (0 < pChSys->QueueLowerSend.Count))
	{
		pRbl = pChSys->QueueLowerSend.pFirst;
		gsy_DequeLower(&pChSys->QueueLowerSend, pRbl);
		pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);

		gsy_DelayFuFill(pChSys, PortId, pPduTx->PtcpData.DelayRsp.PtcpHdr.SequenceID.Word, pPdu);

		/* The Delay parameters of the Response are also in the FollowUp
		*/
		pPdu->PtcpData.DelayFu.DelayParam = pPduTx->PtcpData.DelayRsp.DelayParam;

		/* 030407lrg001: Calculate time in nanoseconds with 32Bit WrapAround 
		 * from local send timestamp
		 * AP01236058 Lint Warnings entfernen: (LSA_UINT64)
		*/
		Nanos = (LSA_UINT64)TimeStamp * (LSA_UINT64)pChSys->RxTxNanos;
		Nanos = Nanos & 0xFFFFFFFF;	// was: % GSY_NANOS_PER_DWORD;

		/* Calculate local bridge delay betwwen reception of delay requests (T2)
		 * and send timestamp of the Response (T3)
		*/
		T2Time = GSY_SWAP32(pPduTx->PtcpData.DelayRsp.PortTime.T2TimeStamp.Dword);
		NanoSecs = (LSA_UINT32)Nanos - T2Time;

		/* Set responder delay in th PTCP header
		*/
		pPdu->PtcpData.DelayFu.PtcpHdr.DelayNS.Dword = GSY_SWAP32(NanoSecs);

		/* Send Delay FollowUp frame
		*/
		gsy_DataSend(pChSys, pRbl, PortId, GSY_DELAY_FU_PDU_SIZE, LSA_FALSE);

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- gsy_DelayRspCnf() Port=%u Seq=%05u DelayNS=%u Timestamp3=%u T2Time=%u [%x%x%x]", 
				PortId, GSY_SWAP16(pPduTx->PtcpData.DelayRsp.PtcpHdr.SequenceID.Word), NanoSecs, TimeStamp, T2Time, 0, 0, 0);
	}
	else
		/* ...Trace error and ignore it
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayRspCnf() Send queue empty or response not OK: pChSys=%X Count=%d Response=0x%x", 
					pChSys, pChSys->QueueLowerSend.Count, Response);

	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayRspCnf() pRbl=%X NanoSecs=%u", 
					pRbl, NanoSecs);
}

/*****************************************************************************/
/* Internal access function: gsy_DelayRspInd()                               */
/* Receive a Delay response frame                                            */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayRspInd(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_RX_PDU_PTR	pPduRx,
LSA_UINT16				Length,
LSA_UINT32				TimeStamp,
LSA_BOOL				RspWithFu)
{
	LSA_UINT16	SeqId;
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT32	PortBit = 1;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayRspInd() pChSys=%X Port=%u Length=%u TimeStamp=%u", 
					pChSys, PortId, Length, TimeStamp);

	/* 200707lrg002: Delay measurement at this port must not 
	 * be disturbed by setting the rate: deblock it
	*/
	PortBit <<= (Idx);
	pChSys->PortDelayReqActive &= ~PortBit;

	/* If the received delay response is a new one (without Subdomain) and is not too short...
	*/
	if  (((GSY_DELAY_RSP_RX_SIZE - GSY_TLV_PORTTIME_SIZE) <= Length)
	&&   (GSY_TLV_DELAYPARAM_TYPELEN == GSY_SWAP16(pPduRx->DelayRsp.DelayParam.TypeLen.Word)))
	{
		/* Check elements wich has been sent with the request and have to be echoed in the response frame
		*/
		SeqId = GSY_SWAP16(pPduRx->DelayFu.PtcpHdr.SequenceID.Word);

		if ((SeqId == pChSys->Port[Idx].ActDelay.ReqSeqId)
		&&  GSY_MACADDR_CMP(pPduRx->DelayRsp.DelayParam.RequestSourceAddress, pChSys->Port[Idx].MACAddr))
		{

			/* If there was as yet no response for the actual request...
			 * and we are not in the wait phase (timeout occured)
			*/
			if ((!pChSys->Port[Idx].ResponseArrived ) &&  /* js040806 */
			     !(pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_TIME ))
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
					pChSys->Port[Idx].ActDelay.RspDelayNS = (LSA_INT32)(GSY_SWAP32(pPduRx->DelayRsp.PtcpHdr.DelayNS.Dword));
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
					gsy_DelayCalc(pChSys, PortId, SeqId);
				}
			}
			else
	 		{
				if ( pChSys->Port[Idx].ResponseArrived ) 
 				{
					/* js040806:
					 * muliple Response
					*/
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR, "*** gsy_DelayRspInd() Multiple Delay.rsp: pChSys=%X Port=%u DelayState=0x%x", 
						pChSys, (Idx+1), pChSys->Port[Idx].ActDelay.State);

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
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP, "*** gsy_DelayRspInd() Delay.rsp too late. ignored: Port=%u SeqId=%d State=0x%x", 
								PortId, SeqId, pChSys->Port[Idx].ActDelay.State);
				}
	 		}
		}	
		else
		{
			/* ...trace error and ignore
			*/
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayRspInd() Invalid response parameter: pChSys=%X SeqID=%u", 
						pChSys, SeqId);
 		}
	}
	else
	{
		/* ...trace error and ignore
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayRspInd() Invalid DelayRsp PDU: pChSys=%X pPduRx=%X Length=%d", 
					pChSys, pPduRx, Length);
	}
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayRspInd() pPduRx=%X RspWithFu=%u", 
					pPduRx, RspWithFu);
}

/*****************************************************************************/
/* Internal access function: gsy_DelayFuInd()                                */
/* Receive a Delay FollowUp frame                                            */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayFuInd(
GSY_CH_SYS_PTR  		pChSys,
LSA_UINT16				PortId,
GSY_LOWER_RX_PDU_PTR	pPduRx,
LSA_UINT16				Length)
{
	LSA_UINT16	SeqId;
	LSA_UINT16	Idx = PortId - 1;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayFuInd() pChSys=%X pPduRx=%X Port=%u Length=%u", 
					pChSys, pPduRx, PortId, Length);

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
			pChSys->Port[Idx].ActDelay.RspDelayNS = (LSA_INT32)(GSY_SWAP32(pPduRx->DelayFu.PtcpHdr.DelayNS.Dword));

			if (pChSys->Port[Idx].ActDelay.State == GSY_DELAY_STATE_END)
			{
				/* If send confirmation already occured:
				 * calculate LineDelay and CableDelay
				*/
				gsy_DelayCalc(pChSys, PortId, SeqId);
			}
		}
		else
			/* ...Trace error and ignore it
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayFuInd() Invalid SeqId: Port=%u ReqSeqId=%u SeqId=%u", 
						PortId, pChSys->Port[Idx].ActDelay.ReqSeqId, SeqId);
	}
	else
		/* ...Trace error and ignore it 220910lrg001: report ReqSeqId
		*/
		GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayFuInd() Invalid PDU length or state: ReqSeqId=%u Length=%d Port=%u DelayState=0x%x", 
					pChSys->Port[Idx].ActDelay.ReqSeqId, Length, Idx+1 ,pChSys->Port[Idx].ActDelay.State);


	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayFuInd()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqCnf()                               */
/* A Delay Request frame has been sent.                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqCnf(
GSY_CH_SYS_PTR  		    pChSys,
LSA_UINT16				    Response,
LSA_UINT16				    PortId,
const GSY_LOWER_TX_PDU_PTR	pPduTx,
LSA_UINT32				    TimeStamp)
{
	LSA_UINT64		Nanos;
	LSA_UINT16	 	SeqId;
	LSA_UINT16		Idx = PortId - 1;
	LSA_UINT32		PortBit = 1;

	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqCnf() pChSys=%X Port=%u Response=0x%x TimeStamp=%u", 
					pChSys, PortId, Response, TimeStamp);

	if (Response == GSY_LOWER_RSP_OK)
	{
		SeqId = GSY_SWAP16(pPduTx->PtcpData.DelayReq.PtcpHdr.SequenceID.Word);
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
				gsy_DelayCalc(pChSys, PortId, SeqId);
			}
		}
		else
		{
			/* js040806: if we are in the wait phase: confirmation is too late - ignore it 
			*/
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayReqCnf() DelayReq.cnf too late: pChSys=%X Port=%u SeqId=%u", 
						pChSys, PortId, SeqId);
		}
	}
	else
	{
		/* 030907lrg001: Delay measurement at this port must not 
		 * be disturbed by setting the rate: deblock it
		*/
		PortBit <<= Idx;
		pChSys->PortDelayReqActive &= ~PortBit;

		/* Store request send error
		*/
		pChSys->Port[Idx].LastError = Response; 
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** gsy_DelayReqCnf() Send not OK: pChSys=%X Port=%d Response=0x%x", 
					pChSys, PortId, Response);
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqCnf()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqSend()                              */
/* Send a delay request Frame to the given port.                             */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqSend(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId)
{
	GSY_LOWER_TX_PDU_PTR	pPdu;
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT32				PortBit = 1;
	LSA_UINT16				Idx = PortId - 1;
	
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqSend() pChSys=%X Port=%u", 
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
		/* If there is a RQB in the Lower Queue...
		*/
		if (0 < pChSys->QueueLowerSend.Count)
		{
			/* Get a Lower Send RQB from Queue and build Delay Request Frame
			*/
			pRbl = pChSys->QueueLowerSend.pFirst;
			gsy_DequeLower(&pChSys->QueueLowerSend, pRbl);
			pPdu = (GSY_LOWER_TX_PDU_PTR)(void*)GSY_LOWER_RQB_SEND_PTR_GET(pRbl);

			pChSys->Port[Idx].ActDelay.ReqSeqId = gsy_DelayReqFill(pChSys, PortId, pPdu);
			pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_REQ;

			/* Send Delay Request with Timestamp
			 * 200707lrg002: Delay measurement at this port must not 
			 * be disturbed by setting the rate: block it
			 * 050907lrg001: Trace
			GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- gsy_DelayReqSend() pChSys=%X Port=%u Seq=%u [%x%x%x%x%x]", 
				pChSys, PortId, pChSys->Port[Idx].ActDelay.ReqSeqId, 0, 0, 0, 0, 0);
			*/
			PortBit <<= Idx;
			pChSys->PortDelayReqActive |= PortBit;
			gsy_DataSend(pChSys, pRbl, PortId, GSY_DELAY_REQ_PDU_SIZE, LSA_TRUE);
		}
		else
		{
			/* No send RQB in the Lower Queue
			*/
			pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_ERR;
			pChSys->Port[Idx].LastError = GSY_RSP_ERR_RESOURCE; 
			GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayReqSend() Send RQB queue empty: pChSys=%X Port=%u ErrorCount=%d", 
						pChSys, Idx+1, pChSys->Port[Idx].ErrorCount);
		}
	}
	else
	{
		/* 130607lrg001: the previous request send RQB is not yet confirmed
		*/
		GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_UNEXP,  "*** gsy_DelayReqSend() DelayReq not confirmed: Port=%u State=0x%02x SeqId=%d", 
					PortId, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].ActDelay.ReqSeqId);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqSend() DelayState=0x%x", 
					pChSys->Port[Idx].ActDelay.State);
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqNext()                              */
/* Timer function for delay measurement                                      */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqNext(
GSY_TIMER_PTR	pTimer)
{
	LSA_UINT16	RetVal;
	LSA_UINT8	State;
	LSA_UINT16	Time = 0;
	LSA_UINT16	Idx = pTimer->Port - 1;
	LSA_UINT32	PortBit = 1;
	LSA_BOOL	SendReq = LSA_TRUE;
	LSA_BOOL	SetDelay = LSA_FALSE;
	GSY_CH_SYS_PTR  pChSys = pTimer->pChSys;

	GSY_FUNCTION_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqNext() pChSys=%X TimerId=%d Port=%u", 
					pChSys, pTimer->Id, pTimer->Port);

	/* If the Timer has not been restarted between Timeout and now...
	 * (this may happen if GSY_DO_TIMEOUT_EXTERN is defined ArtsRQ285959)
	*/
	if ((pChSys->State == GSY_CHA_STATE_OPEN)
	&&  (!pTimer->Running))
	{
		/* 200707lrg002: Delay measurement at this port must not 
		 * be disturbed by setting the rate: deblock it
		*/
		PortBit <<= Idx;
		pChSys->PortDelayReqActive &= ~PortBit;

		/* 050907lrg002: not longer delay events and delay ramp
		*/
		State = pChSys->Port[Idx].ActDelay.State;
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

		default:

			/* Single measurement not finished: 
			 * 060907lrg001: set GSY_DELAY_STATE_TIME
			 * 040608lrg001: after GSY_DELAY_NUM_ERROR sequential errors the delay is set to 0
			 * 170608lrg001: LEVEL_ERROR only if a delay was calculated prior to error
			*/
			GSY_ERROR_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH,  "*** gsy_DelayReqNext() Delay error at Port=%u ReqSeqId=%u State=0x%02x LastError=0x%x", 
					Idx+1, pChSys->Port[Idx].ActDelay.ReqSeqId, pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].LastError);

			pChSys->Port[Idx].ErrorCount++; 
			if (pChSys->Port[Idx].ErrorCount >= GSY_DELAY_NUM_ERROR)
			{
				if (0 != pChSys->Port[Idx].SyncLineDelay)
				{
					GSY_ERROR_TRACE_03(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,  "*** gsy_DelayReqNext() Setting delay to zero: Port=%u State=0x%02x LastError=0x%x", 
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

		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE, "--- gsy_DelayReqNext() pChSys=%X Port=%u SetDelay=%u SendReq=%u Old/New State=0x%02x/0x%02x Time=%u/%u", 
				pChSys, pTimer->Port, SetDelay, SendReq, State, pChSys->Port[Idx].ActDelay.State, Time, pChSys->Port[Idx].ActDelay.Time);

		if (SetDelay)
		{
			/* js040806: put last calculated temporary values to actual delay values
			*/
			gsy_DelayTempToActual(pChSys, pTimer->Port);

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
			gsy_DelayReqSend(pChSys, pTimer->Port);
		}

		/* Restart Delay Timer
		*/
		gsy_TimerStart(pTimer, pChSys->Port[Idx].ActDelay.Time);
	}
	else if (pChSys->State == GSY_CHA_STATE_CLOSED)
	{
		/* Channel is closing:
		 * retry to free all resources
		*/
		RetVal = gsy_ChSysClean(pChSys);
		if (LSA_RET_OK == RetVal)
		{
			/* Callback Upper RQB and free channel structure
			*/ 
			gsy_CallbackSys(pChSys, LSA_NULL, pTimer->pChSys->Resp);
			gsy_ChSysFree(pChSys);
		}
	}
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqNext() State=%u Time=%u", 
					pChSys->Port[Idx].ActDelay.State, pChSys->Port[Idx].ActDelay.Time);
}

#endif // GSY_CFG_SYNC_ONLY_IN_KRISC32

/*****************************************************************************/
/* Internal function: gsy_DelayRspStart()                                    */
/* Start responding to delay requests at one port.                           */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayRspStart(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT16	Idx = PortId - 1;
	
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayRspStart() pChSys=%X Port=%u", 
					pChSys, PortId);

	GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_DelayRspStart() pChSys=%X Port=%u TxDelay=%u RxDelay=%u [%x%x%x%x] delay state change ->START", 
			pChSys, PortId, pChSys->Port[Idx].TxDelay, pChSys->Port[Idx].RxDelay, 0, 0, 0, 0);

	if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
	{
		/* Start KRISC32 delay responder by sending a RQB to EDDP
		*/
		pRbl = pChSys->QueueLowerCtrl.pFirst;
		if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
		{
			GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DelayRspStart() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_SYNC_CTRL=%u GSY_NUM_RQB_DELAY_CTRL=%u",
							GSY_NUM_RQB_SYNC_CTRL, GSY_NUM_RQB_DELAY_CTRL);
		}
		else
		{
			gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
			GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
            GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
			GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
			GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_DELAY_CTRL);
			GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
			GSY_LOWER_RQB_K32_DELAY_REQ_ACTIVE_SET(pRbl, GSY_LOWER_DELAY_INACTIVE);
			GSY_LOWER_RQB_K32_DELAY_RSP_ACTIVE_SET(pRbl, GSY_LOWER_DELAY_ACTIVE);
			GSY_LOWER_RQB_K32_DELAY_PORT_ID_SET(pRbl, PortId);
			GSY_LOWER_RQB_K32_DELAY_PORT_RXDELAY_SET(pRbl, pChSys->Port[Idx].RxDelay);
			GSY_LOWER_RQB_K32_DELAY_PORT_TXDELAY_SET(pRbl, pChSys->Port[Idx].TxDelay);
			GSY_LOWER_RQB_K32_DELAY_PORT_ADDR_SET(pRbl, pChSys->Port[Idx].MACAddr);

			/* Send Lower Request
			*/
			GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
		}

	}
	/* Allow sending of delay responses
	*/
	pChSys->Port[Idx].DelayRspActive = LSA_TRUE;

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayRspStart()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqStart()                             */
/* Start the state machine for delay measurement at one port                 */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqStart(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT16	Idx = PortId - 1;
	
	GSY_FUNCTION_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  > gsy_DelayReqStart() pChSys=%X Port=%u", 
					pChSys, PortId);

	if (pChSys->Port[Idx].DelayReqActive)
	{
		/* If the delay measurement is activated at this port...
		*/
		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_DelayReqStart() pChSys=%X Port=%u TxDelay=%u RxDelay=%u [%x%x%x%x] delay state change ->START", 
				pChSys, PortId, pChSys->Port[Idx].TxDelay, pChSys->Port[Idx].RxDelay, 0, 0, 0, 0);

		if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
		{
			/* Start KRISC32 delay requestor and responder by sending a RQB to EDDP
			*/
			pRbl = pChSys->QueueLowerCtrl.pFirst;
			if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DelayReqStart() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_SYNC_CTRL=%u GSY_NUM_RQB_DELAY_CTRL=%u",
								GSY_NUM_RQB_SYNC_CTRL, GSY_NUM_RQB_DELAY_CTRL);
			}
			else
			{
				gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
				GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
                GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
				GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
				GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_DELAY_CTRL);
				GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
				GSY_LOWER_RQB_K32_DELAY_REQ_ACTIVE_SET(pRbl, GSY_LOWER_DELAY_ACTIVE);
				GSY_LOWER_RQB_K32_DELAY_RSP_ACTIVE_SET(pRbl, GSY_LOWER_DELAY_ACTIVE);
				GSY_LOWER_RQB_K32_DELAY_PORT_ID_SET(pRbl, PortId);
				GSY_LOWER_RQB_K32_DELAY_PORT_RXDELAY_SET(pRbl, pChSys->Port[Idx].RxDelay);
				GSY_LOWER_RQB_K32_DELAY_PORT_TXDELAY_SET(pRbl, pChSys->Port[Idx].TxDelay);
				GSY_LOWER_RQB_K32_DELAY_PORT_ADDR_SET(pRbl, pChSys->Port[Idx].MACAddr);

				/* Send Lower Request
				*/
				GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
			}

		}
		else
		{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32

		 	/* Initialise delay variables and timer for this port
			*/
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
			pChSys->Port[Idx].LastError = LSA_RSP_OK; 
			pChSys->Port[Idx].ErrorCount = 0; 
			pChSys->Port[Idx].ActDelay.Time = GSY_DELAY_TIME_START;

			/* 030407lrg001: restart local RCF calculation
			*/
			pChSys->Port[Idx].ActDelay.RcfLocal.State = GSY_DRIFT_STATE_UNKNOWN;
			pChSys->Port[Idx].ActDelay.RcfLocal.Count = 0;

			/* Send Delay Request frame and start timer
			*/
			gsy_TimerStop(&pChSys->Port[Idx].DelayTimer);
			gsy_DelayReqSend(pChSys, PortId);
			gsy_TimerStart(&pChSys->Port[Idx].DelayTimer, GSY_DELAY_TIME_START);
#else
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
		}
		/* Allow sending of delay responses
		*/
		pChSys->Port[Idx].DelayRspActive = LSA_TRUE;

		/* No gPTP support until now
		*/
        pChSys->Port[Idx].Is_gPTP = LSA_FALSE;
	}

	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_CHAT,"  < gsy_DelayReqStart()");
}

/*****************************************************************************/
/* Internal access function: gsy_DelayReqStop()                              */
/* Stop the state machine for delay measurement at one port                  */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayReqStop(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId,
LSA_UINT8		RspActive)
{
	GSY_LOWER_RQB_PTR_TYPE	pRbl;
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT32	OldDelay = pChSys->Port[Idx].SyncLineDelay;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_DelayReqStop() pChSys=%X OldDelay=%u Port=%u RspActive=%u", 
					pChSys, OldDelay, PortId, RspActive);

	if (pChSys->Port[Idx].DelayReqActive)
	{
		/* If the delay measurement is activated at this port...
		*/
		GSY_DEL_TRACE_08(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "--- gsy_DelayReqStop() pChSys=%X Port=%u LineDelay=%u CableDelay=%u [%x%x%x%x] delay state change ->STOP", 
				pChSys, PortId, OldDelay, pChSys->Port[Idx].CableDelay, 0, 0, 0, 0);

		if (GSY_HW_TYPE_DELAY_HW == (pChSys->HardwareType & GSY_HW_TYPE_DELAY_MSK))
		{
			/* Stop KRISC32 delay requestor (and responder) by sending a RQB to EDDP
			*/
			pRbl = pChSys->QueueLowerCtrl.pFirst;
			if (LSA_HOST_PTR_ARE_EQUAL(pRbl, LSA_NULL))
			{
				GSY_ERROR_TRACE_02(pChSys->TraceIdx, LSA_TRACE_LEVEL_ERROR,"*** gsy_DelayReqStop() No lower RQB in QueueLowerCtrl: GSY_NUM_RQB_SYNC_CTRL=%u GSY_NUM_RQB_DELAY_CTRL=%u",
								GSY_NUM_RQB_SYNC_CTRL, GSY_NUM_RQB_DELAY_CTRL);
			}
			else
			{
				gsy_DequeLower(&pChSys->QueueLowerCtrl, pRbl);
				GSY_LOWER_RQB_OPCODE_SET(pRbl, GSY_LOWER_OPC_REQUEST);
                GSY_LOWER_RQB_HANDLE_SET(pRbl, 0); /* sanity */
				GSY_LOWER_RQB_LOWER_HANDLE_SET(pRbl, pChSys->HandleLower);
				GSY_LOWER_RQB_SERVICE_SET(pRbl, GSY_LOWER_DELAY_CTRL);
				GSY_LOWER_RQB_ERR_COMPID_SET(pRbl, LSA_COMP_ID_UNUSED);
				GSY_LOWER_RQB_K32_DELAY_REQ_ACTIVE_SET(pRbl, GSY_LOWER_DELAY_INACTIVE);
				GSY_LOWER_RQB_K32_DELAY_RSP_ACTIVE_SET(pRbl, RspActive);
				GSY_LOWER_RQB_K32_DELAY_PORT_ID_SET(pRbl, PortId);
				GSY_LOWER_RQB_K32_DELAY_PORT_RXDELAY_SET(pRbl, pChSys->Port[Idx].RxDelay);
				GSY_LOWER_RQB_K32_DELAY_PORT_TXDELAY_SET(pRbl, pChSys->Port[Idx].TxDelay);
				GSY_LOWER_RQB_K32_DELAY_PORT_ADDR_SET(pRbl, pChSys->Port[Idx].MACAddr);

				/* Send Lower Request
				*/
				GSY_REQUEST_LOWER(pRbl, pChSys->pSys);
			}

		}
		else
		{
#ifndef GSY_CFG_SYNC_ONLY_IN_KRISC32
			LSA_UINT32	PortBit = 1;

			/* 030907lrg001: Delay measurement at this port must not 
			 * be disturbed by setting the rate: deblock it
			*/
			PortBit <<= Idx;
			pChSys->PortDelayReqActive &= ~PortBit;

			/* Stop Timer
			*/
			gsy_TimerStop(&pChSys->Port[Idx].DelayTimer);

			/* Reinitialise temporary delay variables and put them to actual
			 * js040806:
			*/
			pChSys->Port[Idx].CableDelayTemp = 0;
			pChSys->Port[Idx].SyncLineDelayTemp = 0;
			pChSys->Port[Idx].MinDelayTemp = 0;
			pChSys->Port[Idx].MaxDelayTemp = 0;
			pChSys->Port[Idx].DelayValid = LSA_TRUE; 

			pChSys->Port[Idx].ActDelay.State = GSY_DELAY_STATE_INI;

			gsy_DelayTempToActual(pChSys,PortId);
#else
			gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_CONFIG, LSA_NULL, 0);

#endif //GSY_CFG_SYNC_ONLY_IN_KRISC32
		}
	}
	GSY_FUNCTION_TRACE_00(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_DelayReqStop()");

	LSA_UNUSED_ARG(OldDelay);
}

/*****************************************************************************/
/* Internal access function: gsy_DelayUserCtrl()                             */
/* Start or stop the state machine for delay measurement at one port.        */
/* If port state does not allow that it happens on change in gsy_PortState() */
/*****************************************************************************/
LSA_VOID GSY_LOCAL_FCT_ATTR gsy_DelayUserCtrl(
GSY_CH_SYS_PTR  pChSys,
LSA_UINT16		PortId,
LSA_UINT16		Ctrl)
{
	LSA_UINT16	Idx = PortId - 1;
	LSA_UINT8	PortState = pChSys->Port[Idx].State;
	LSA_UINT8	RspActive = GSY_LOWER_DELAY_ACTIVE;
	
	GSY_FUNCTION_TRACE_04(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  > gsy_DelayUserCtrl() pChSys=%X Port=%u Ctrl=%u DelayReqActive=%u", 
					pChSys, PortId, Ctrl, pChSys->Port[Idx].DelayReqActive);

	if (((GSY_DELAY_STOP == Ctrl) || (GSY_DELAY_END == Ctrl))
	&&  pChSys->Port[Idx].DelayReqActive)
	{
		if (GSY_DELAY_END == Ctrl)
		{
			/* Stop delay responder
			*/
			RspActive = GSY_LOWER_DELAY_INACTIVE;
			pChSys->Port[Idx].DelayRspActive = LSA_FALSE;
		}
		if ((GSY_DELAY_END == Ctrl)
		|| ((GSY_PORT_STATE_DOWN != PortState) 
		&&  (GSY_PORT_STATE_INIT != PortState)))
		{
			/* Delay Measurement running or CloseChannel: stop
			*/
			gsy_DelayReqStop(pChSys, PortId, RspActive);
		}
		pChSys->Port[Idx].DelayReqActive = LSA_FALSE;
	}
	else if ((GSY_DELAY_START == Ctrl)
	     &&  (!pChSys->Port[Idx].DelayReqActive))
	{
		pChSys->Port[Idx].DelayReqActive = LSA_TRUE;
		if ((GSY_PORT_STATE_DOWN != PortState) 
		&& (GSY_PORT_STATE_INIT != PortState))
		{
			/* Start Delay Measurement
			*/
			gsy_DelayReqStart(pChSys, PortId);
		}
	}
	else if ((GSY_DELAY_STOP == Ctrl)
		 &&  (pChSys->Port[Idx].DelayBoundary)
	     &&  (!pChSys->Port[Idx].DelayRspActive))
	{
		/* Start delay responder at delay boundary
		*/
		pChSys->Port[Idx].DelayRspActive = LSA_TRUE;
  		gsy_DelayRspStart(pChSys, PortId);
	}

	GSY_FUNCTION_TRACE_01(pChSys->TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH/*lrglevelchat*/,"  < gsy_DelayUserCtrl() DelayReqActive=%u", 
					pChSys->Port[Idx].DelayReqActive);
}

/*****************************************************************************/
/*  end of file gsy_del.c                                                    */
/*****************************************************************************/

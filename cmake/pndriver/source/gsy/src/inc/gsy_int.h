
#ifndef GSY_INT_H                        /* ----- reinclude-protection ----- */
#define GSY_INT_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: gsy_int.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile                                                      */
/*  Defines internal constants, types, data, macros and prototyping.         */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-04-04  P01.00.00.00_00.01.00.01 lrg  Frame-IDs geändert...          */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2005-06-24  P03.06.00.00_00.03.02.02 lrg  neues Sync-Userinterface       */
/*  2005-09-16  P03.07.00.00_00.08.01.01 js   ps (pico seconds) ergänzt.     */
/*  2005-11-22  P03.08.00.00_00.01.02.00 lrg  GSY_NANOS_PER_DWORD 4294967295U*/
/*  2005-11-22  P03.08.00.00_00.01.02.02 lrg  -GSY_OPC_SYNC_RECORD_READ_REAL */
/*  2005-12-20  P03.09.00.00_00.01.00.00 lrg  QueueWait in GSY_CH_USR        */
/*  2005-12-22  P03.09.00.00_00.01.01.02 lrg  RTC2Ctrl in GSY_CH_USR         */
/*  2006-03-08  P03.10.00.00_00.01.01.02 lrg  Generalisierte CM-PD-Schnitt-  */
/*              stelle mit gsy_Prm...                                        */
/*  2006-03-29  P03.10.00.00_00.02.01.02 lrg PortFwd[] von gsy_detail nach   */
/*              GSY_CH_SYS verschoben                                        */
/*  2006-04-26  P03.11.00.00_00.01.01.04 lrg IndTimestamp in GSY_TIME_TYPE   */
/*              GSY_DELAY_MAX_COUNT von 0 (unbegrenzt) auf 8 (PNIO-Norm)     */
/*  2006-05-22  P03.11.00.00_00.03.01.02 lrg  neu: GSY_RSPDELAY_MAX          */
/*  2006-05-29  P03.11.00.00_00.04.01.02 lrg  erweitert: GSY_DIAG_DATA_BLOCK */
/*              SeqIdSnd und SeqIdCnf von .Port[] nach .Master verschoben    */
/*  2006-06-01  P03.11.00.00_00.04.02.02 lrg  neu: GSY_TS_WRAP_LOW           */
/*  2006-06-02  P03.11.00.00_00.04.03.02 lrg  neu: GSY_OLD_IF; "neue" Sync-  */
/*              Schnitts. entfernt                                           */
/*  2006-06-29  P03.12.00.00_00.02.01.02 lrg  GSY_SYNCID_XXX nach gsy_usr.h  */
/*              neu: AdjustTermP, AdjustTermI, Felder fuer Mittelwerte       */
/*  2006-07-25  P03.12.00.00_00.25.02.02 170706lrg002: GSY_SLAVE_STATE_FIRST */
/*              250706lrg001: TimerRQB-Semaphore RbInUse                     */
/*              GSY_DRIFT_TIME_VALUE 1 -> 2                                  */
/*  2006-08-04  P03.12.00.00_00.25.04.01 js  Temporäre Variablen hinzugefügt */
/*                                           GSY_DELAY_STATE_TIME wird nicht */
/*                                           verORed bzw. verAND.            */
/*              110806js001 : Änderung der SubdomainUUID bewirkt Neustart    */
/*                            der Synchronisation.                           */
/*              240806lrg002; RQB-Zaehler fuer RTC2-RQBs: RTC2RblCount       */
/*  2006-09-04  P03.12.00.00_00.25.04.02 lrg GSY_MODULE_ID_BMA und           */
/*              GSY_MODULE_ID_MSM                                            */
/*              Neu: gsy_PrmWriteClockCheck()                                */
/*  2006-10-23  P03.12.00.00_00.30.03.02 231006lrg002: PriMasterWaits        */
/*  2006-11-16  P03.12.00.00_00.30.04.03 161106js001 : added PrmDomainCfg    */
/*              neu: GSY_SUBDOMAIN_ARRAY_CMP                                 */
/*  2006-12-06  P03.12.00.00_00.40.04.02 051206lrg001: neu: PrmSendClock     */
/*  2006-12-13  P03.12.00.00_00.40.05.02 131206lrg001: GSY_DRIFT_MAX_COUNT=1 */
/*  2007-01-10  P03.12.00.00_00.40.06.02 100107lrg003: GSY_DRIFT_MAX_COUNT=7 */
/*              Ersatzintervall GSY_DRIFT_MAX_INTERVAL fuer "keine Drift"    */
/*  2007-02-12  P03.12.00.00_00.40.09.02 120207lrg001: QueueEvent            */
/*  2007-02-20  P03.12.00.00_00.40.10.02 200207lrg001:Masteraging untersetzen*/
/*  2007-07-17  P04.00.00.00_00.30.01.00 170707lrg001: RTSync                */
/*  2007-07-31  P04.00.00.00_00.30.01.02 310707js001 : RTSync                */
/*  2007-08-29  P04.00.00.00_00.30.04.02 290807lrg001: MasterStartupTime     */
/*  2007-09-07  P04.00.00.00_00.30.05.02 070907lrg001: Master.Startup        */
/*  2007-09-27  P04.00.00.00_00.30.06.02 270907lrg005: GSY_MACADDR_IS_NULL   */
/*              PrmTimer entfernt, #define GSY_OLD_IF entfernt               */
/*  2007-09-27  P04.00.00.00_00.40.01.02 101007lrg002: nie mehr Delay-Rampe, */
/*              Messsequenz: alle 8 Sek. 5 mal, bei Fehler 1 Sekunde Pause   */
/*              161007lrg003: AP00526517 PrimaryMaster, RateValid            */
/*  2007-11-22  P04.00.00.00_00.60.01.02 lrg: gsy_PrmValidName()             */
/*  2008-01-18  P04.00.00.00_00.70.01.03 180108lrg001: Rate parallel         */
/*              berechnen                                                    */
/*  2008-04-17  P04.00.00.00_00.80.01.02 170408lrg002: RcvSyncPrio,LocalPrio */
/*  2008-06-04  P04.00.00.00_00.80.03.02 040608lrg001: ArtsRQ AP AP00648600  */
/*              GSY_DELAY_NUM_ERROR = Anzahl Fehler bis zum Delayreset       */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-06-24  P04.01.00.00_00.01.01.02 240608lrg001: ArtsRQ AP00536908     */
/*              DelaySet zwischen Senderequest und Confirmation verzögern    */
/*  2008-07-25  P04.01.00.00_00.01.02.03 250708lrg001: InfoSync              */
/*  2008-11-28  P04.01.00.00_00.02.02.03 KRISC32-Unterstuetzung              */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-10  P04.01.00.00_00.02.03.02 101208lrg001: Announce mit          */
/*              gsy_DataSend() versenden                                     */
/*  2009-03-18  P04.01.00.00_00.03.03.02 180309lrg001: pK32SyncRqb           */
/*  2009-07-02  P04.02.00.00_00.01.03.02 lrg: K32_MASTER_TYPE                */
/*  2009-09-29  P04.02.00.00_00.01.04.02 290909lrg001: Retry setting Drift   */
/*  2009-11-13  P05.00.00.00_00.01.01.02 lrg: K32_SYNC_UPDATE                */
/*  2009-12-02  P05.00.00.00_00.01.01.03 lrg: k32gsy_FwdSet,GSY_NO_SUBDOMAIN */
/*  2010-03-03  P05.00.00.00_00.01.03.02 lrg: Announce channel               */
/*  2010-07-21  P05.00.00.00_00.01.06.02 lrg: gsy_ChUsrClean()               */
/*              AP00697597: German comments translated to english            */
/*  2010-10-08  P05.00.00.00_00.01.08.03 230910lrg001: PDSyncPLL record      */
/*  2010-11-15  P05.00.00.00_00.02.01.02 lrg: PLLTimer, LastPLL              */
/*  2010-12-13  P05.00.00.00_00.02.01.03 131210lrg001: PDSyncPLL-Record solo */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2011-02-14  lrg: Remove global edd_usr.h for KRISC32                     */
/*  2011-02-18  P05.00.00.00_00.03.19.02 lrg: GSY_MASTER_TIMER_CHECK         */
/*  2011-05-16  P05.00.00.00_00.05.21.02 160511lrg: AP01183082 gsy_SlaveTimer*/
/*  2011-07-25  P05.00.00.00_00.05.38.02 250711lrg: AP01215492, AP01212974   */
/*  2011-10-24  S05.01.00.00_00.05.05.02 AP01262791 Negative CableDelay      */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01384863: DelayFilter             */
/*  2014-05-20  P06.00.00.00_00. TODO gl 966426 - [GSY] Die Synchronisation  */
/*              über EDDL und i210 soll ermöglicht werden                    */
/*  2014-06-05  P05.03.00.00_00.03.07.01 1028592/1066330: FuTimeout          */
/*  2014-09-12  P06.00.00.00_00.01.44.01 1092865 PDEV-Prm, Erweiterung von   */
/*              PrmWrite und PrmEnd um Fehler-Informationen                  */
/*  2014-11-05  P06.00.00.00_00.01.49.01 1126470 [GSY] Announce-MAC-Adressen */
/*              nur bei Masterprojektierung aktivieren                       */
/*  2015-04-15  V06.00, Inc04, Feature 1204141, Task 1234427                 */
/*              GSY_OPC_MASTER_CONTROL added                                 */
/*  2015-04-15  V06.00, Inc04, Feature FT1204141, Task 1234432               */
/*              eddp_GetSyncTime(), Ratetimeout flex. Ki anpassen            */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*  2015-07-15  V06.00, Inc08, Feature 1227823, Task 1260584                 */
/*              [TSyncOverPTCP][GSY] Integrate CLP in GSY (clp_sys.h)        */
/*  2015-06-09  V06.00, Inc07, Feature 1227823, Task 1261424                 */
/*              GSY und CLP Konfigurationsschalter bereinigen                */
/*  2015-10-02  V06.00, Inc10, RQ 1460015: gsy_PrmCommitEndDiag()            */
/*  2015-11-25  V06.00, Inc12, Task 1499164: K32_FU_FWD_IN_FW                */
/*              SyncFU-/FU-Frames statt in HW in der KRISC32-FW weiterleiten */
/*  2016-05-02  V06.00-V07.00, RQ 1684006: PendingSyncId                     */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

//#define GSY_MASTER_TIMER_CHECK

/* packed struktures begin */

#include "pnio_pck2_on.h"

/*===========================================================================*/
/* constants of CLP                                                          */
/*===========================================================================*/
/* dynamic adaptation to the SyncInterval
*/
#define GSY_DELTA_NS_MAX		32000000

/* PI-Params (AP01376794: Kp/Ki=20/80 -> 11/65)
*/
#define GSY_SYNC_CONTROL_PARAM_P   11	// Kp-Value, Entity: 1/Sec
#define GSY_SYNC_CONTROL_PARAM_I   65	// Ki-Value, Entity: 1/(Sec^2)
#define GSY_SYNC_CONTROL_PARAM_P_1 11	// Kp-Value, Entity: 1/Sec
#define GSY_SYNC_CONTROL_PARAM_I_1 20	// Ki-Value, Entity: 1/(Sec^2) for time sync FT1204141

/* Constant to describe a 64Bit-Integer with Value: 10exp12
*/
#define GSY_SYNC_VALUE_10Exp12_64BIT		0xE8D4A51000LL
//#define GSY_SYNC_VALUE_10Exp12_64BIT		(LSA_INT64)0xE8D4A51000

/* Status after Intialisation
*/
#define GSY_SYNC_CTRL_STATE_INITIALIZED      0xABCD1234UL
#define GSY_SYNC_CTRL_STATE_RUNNING          0xABCD1235UL

#define GSY_LIMIT_LSA_INT32_MAX 0x7FFFFFFFL
#define GSY_LIMIT_LSA_INT32_MIN ((LSA_INT)(-1L * GSY_LIMIT_LSA_INT32_MAX))

/*===========================================================================*/
/* types of CLP                                                              */
/*===========================================================================*/

/* Controller data union for supported SyncIds (0 = Clock, 1 = Time)
 * Pointer to this union is passed from system adaption to
 * clp_SyncAlgorithm() and clp_SyncReset
*/
typedef struct _GSY_SYNC0_CONTROLLER_TYPE
{
  LSA_INT32        Kp_1perSec;           // Kp in 1/sec
  LSA_INT32        Ki_1perSecSec;        // Ki in 1/sec^2
  LSA_INT32        PhaseError_ns;        // PhaseError in ns (Input_Value of PI)
  LSA_INT32        LastPhaseError_ns;    // State variable (Zustandsgroesse)
  LSA_INT64        ManipVar_ppt;         // actuating variable (Stellgroesse: Steigung der Gegendrift)
  LSA_INT32        DeltaTime_ms;         
  LSA_UINT32       AdjustIntervalNs;     // HelperValue
  LSA_UINT32       State;                // State of Sync              
  LSA_UINT32       LastSec;              // Time of last offset
  LSA_UINT32       LastNs;               // Time of last offset

} GSY_SYNC0_CONTROLLER_TYPE;

typedef struct _GSY_SYNC1_CONTROLLER_TYPE
{
  LSA_INT32        Param_Ax1000;         // 1...1000
  LSA_INT32        Param_X;              // 999...0

} GSY_SYNC1_CONTROLLER_TYPE;

typedef union _GSY_SYNC_CONTROLLER_TYPE
{
  GSY_SYNC0_CONTROLLER_TYPE	CycleCtrl;
  GSY_SYNC1_CONTROLLER_TYPE	TimeCtrl;
} GSY_SYNC_CONTROLLER_TYPE;

/******************************************************************************
 * GSY pdu structures
*/

typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_WORD_TYPE
{               
    LSA_UINT8       Octet[2];
    LSA_UINT16      Word;
} PNIO_PACKED_ATTRIBUTE_POST GSY_WORD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_DWORD_TYPE
{               
    LSA_UINT8       Octet[4];
    LSA_UINT16      Word[2];
    LSA_UINT32      Dword;
} PNIO_PACKED_ATTRIBUTE_POST GSY_DWORD_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_QWORD_TYPE
{               
    LSA_UINT8       Octet[8];
    LSA_UINT16      Word[4];
    LSA_UINT32      Dword[2];
} PNIO_PACKED_ATTRIBUTE_POST GSY_QWORD_TYPE;

#define GSY_UUID_BYTE_LENGTH	8

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_UUID_TYPE
{               
    LSA_UINT32      Dword;
    LSA_UINT16      Word[2];
    LSA_UINT8       Byte[GSY_UUID_BYTE_LENGTH];
} PNIO_PACKED_ATTRIBUTE_POST GSY_UUID_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_UUID
{               
    GSY_DWORD_TYPE  Data1;
    GSY_WORD_TYPE   Data2;
    GSY_WORD_TYPE   Data3;
    LSA_UINT8       Data4[GSY_UUID_BYTE_LENGTH];
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_UUID;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct	_GSY_OUI_TYPE
{               
    LSA_UINT8       Octet[3];
} PNIO_PACKED_ATTRIBUTE_POST GSY_OUI_TYPE;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct	_GSY_VLAN_TAG_TYPE
{               
    LSA_UINT8       Octet[4];
} PNIO_PACKED_ATTRIBUTE_POST GSY_VLAN_TAG_TYPE;

/* PROFINET-Header
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PDU_HDR_VLAN
{
	GSY_MAC_ADR_TYPE	DstMacAdr;		/* Destination MAC address */
	GSY_MAC_ADR_TYPE	SrcMacAdr;		/* Source MAC address */
	GSY_VLAN_TAG_TYPE	VlanTag;		/* VLAN-Tag */
	GSY_WORD_TYPE		EthType;		/* Ethertype */
	GSY_WORD_TYPE		FrameId;		/* Frame-ID */
} PNIO_PACKED_ATTRIBUTE_POST GSY_PDU_HDR_VLAN;

#define GSY_PDU_HDR_SIZE_VLAN		20

/* PROFINET-Header without VLAN tag
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PDU_HDR_NO_VLAN
{
	GSY_MAC_ADR_TYPE	DstMacAdr;		/* Destination MAC address */
	GSY_MAC_ADR_TYPE	SrcMacAdr;		/* Source MAC address */
	GSY_WORD_TYPE		EthType;		/* Ethertype */
	GSY_WORD_TYPE		FrameId;		/* Frame-ID */
} PNIO_PACKED_ATTRIBUTE_POST GSY_PDU_HDR_NO_VLAN;

#define GSY_PDU_HDR_SIZE_NO_VLAN	16

/* 030707lrg002: PDU structure and size with(out) VLAN tag 
*/
#ifdef GSY_USE_VLAN
#define	GSY_PDU_HDR			GSY_PDU_HDR_VLAN
#define	GSY_PDU_HDR_SIZE	GSY_PDU_HDR_SIZE_VLAN
#else
#define	GSY_PDU_HDR			GSY_PDU_HDR_NO_VLAN
#define	GSY_PDU_HDR_SIZE	GSY_PDU_HDR_SIZE_NO_VLAN
#endif

/* PTCP-Header
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_HDR
{
	GSY_DWORD_TYPE	ApiRecvTS;			/* 230410lrg001: Local ApiTimeStamp */
	GSY_DWORD_TYPE	ReceiveTS;			/* LocalReciveTimeStamp */
	GSY_DWORD_TYPE	Delay10ns;			/* Delay */
	GSY_WORD_TYPE	SequenceID;			/* SequenceNumber */
	LSA_UINT8		Delay1ns;			/* Sign Flags*/
	LSA_UINT8		Reserved3;			/* Reserved  */
	GSY_DWORD_TYPE	DelayNS;			/* 32 Bit Nanoseconds */
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_HDR;

#define GSY_PTCP_HDR_SIZE			20
#define GSY_PTCP_DELAY10_OFFSET		10	/* mit Frame-ID */
#define GSY_PTCP_DELAY1_OFFSET		16	/* mit Frame-ID */
#define GSY_PTCP_DelayNS_OFFSET		18	/* mit Frame-ID */

/* Values for Delay1ns, DelayNS
*/
#define GSY_PTCP_DELAY_SIGN_FLAG	0x80
#define GSY_PTCP_DELAY_NS_INVALID	0xFFFFFFFF

/* PTCP-Subdomain TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_SUBDOMAIN
{
	GSY_WORD_TYPE		TypeLen;	/* 7 Bit Type, 9 Bit Length */
	GSY_MAC_ADR_TYPE	MasterSourceAddress; 
	GSY_PTCP_UUID		SubdomainUUID;
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_SUBDOMAIN;

#define GSY_TLV_SUBDOMAIN_SIZE		24
#define GSY_TLV_SUBDOMAIN_LENGTH	22
#define GSY_TLV_SUBDOMAIN_TYPELEN	0x0216

/* PTCP-Time TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_TIME
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
	GSY_WORD_TYPE	EpochNumber; 
	GSY_DWORD_TYPE	Seconds; 
	GSY_DWORD_TYPE	NanoSeconds;
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_TIME;

#define GSY_TLV_TIME_SIZE			12
#define GSY_TLV_TIME_LENGTH			10
#define GSY_TLV_TIME_TYPELEN		0x040A

/* PTCP-TimeExtension TLV
*/
typedef struct PNIO_PACKED_ATTRIBUTE_PRE _GSY_TLV_TIMEEXT
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
	GSY_WORD_TYPE	Flags; 
	GSY_WORD_TYPE	CurrentUTCOffset; 
	LSA_UINT8		Reserved1;
	LSA_UINT8		Reserved2;
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_TIMEEXT;

#define GSY_TLV_TIMEEXT_SIZE		8
#define GSY_TLV_TIMEEXT_LENGTH		6
#define GSY_TLV_TIMEEXT_TYPELEN		0x0606

/* Values for Flags
*/
#define GSY_PTCP_FLAG_FU			0x0001
#define GSY_PTCP_FLAG_LS61			0x0100
#define GSY_PTCP_FLAG_LS59			0x0200

/* PTCP-Grandmaster TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_MASTER
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
	LSA_UINT8		MasterPriority1; 
	LSA_UINT8		MasterPriority2; 
	LSA_UINT8		ClockClass; 
	LSA_UINT8		ClockAccuracy; 
	GSY_WORD_TYPE	ClockVariance; 
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_MASTER;

#define GSY_TLV_MASTER_SIZE			8
#define GSY_TLV_MASTER_LENGTH		6
#define GSY_TLV_MASTER_TYPELEN		0x0806

/* Values for ClockStratum
*/
#define GSY_PTCP_STRATUM_FORCE		0
#define GSY_PTCP_STRATUM_PRIMARY	1
#define GSY_PTCP_STRATUM_SECONDARY	2
#define GSY_PTCP_STRATUM_SIGNAL		3
#define GSY_PTCP_STRATUM_NOSIGNAL	4
#define GSY_PTCP_STRATUM_DEFAULT	255

/* Values for MasterPriority1
*/
#define GSY_PTCP_ROLE_PRIMARY	1
#define GSY_PTCP_ROLE_SECONDARY	2

/* PTCP-PortParameter TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_PORTPARAM
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
	LSA_UINT8       Reserved1; 
	LSA_UINT8       Reserved2;
	GSY_DWORD_TYPE	T2PortRxDelay; 
	GSY_DWORD_TYPE	T3PortTxDelay; 
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_PORTPARAM;

#define GSY_TLV_PORTPARAM_SIZE		12
#define GSY_TLV_PORTPARAM_LENGTH	10
#define GSY_TLV_PORTPARAM_TYPELEN	0x0A0A

/* PTCP-DelayParameter TLV for Delay PDUs
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_DELAYPARAM
{
	GSY_WORD_TYPE		TypeLen;	/* 7 Bit Type, 9 Bit Length */
	GSY_MAC_ADR_TYPE	RequestSourceAddress; 
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_DELAYPARAM;

#define GSY_TLV_DELAYPARAM_SIZE		8
#define GSY_TLV_DELAYPARAM_LENGTH	6
#define GSY_TLV_DELAYPARAM_TYPELEN	0x0C06

/* PTCP-DelayParameter TLV for Delay PDUs
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_PORTTIME
{
	GSY_WORD_TYPE		TypeLen;	/* 7 Bit Type, 9 Bit Length */
	LSA_UINT8       	Reserved1; 
	LSA_UINT8       	Reserved2;
	GSY_DWORD_TYPE		T2TimeStamp; 
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_PORTTIME;

#define GSY_TLV_PORTTIME_SIZE		8
#define GSY_TLV_PORTTIME_LENGTH		6
#define GSY_TLV_PORTTIME_TYPELEN	0x0E06

/* PTCP-Option TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_OPTION
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
	GSY_OUI_TYPE	OUI; 
	LSA_UINT8		SubType;
	LSA_UINT8		Data[1];
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_OPTION;

#define GSY_TLV_OPTION_SIZE			6
#define GSY_TLV_OPTION_LENGTH		4
#define GSY_TLV_OPTION_TYPELEN		0xfe04

/* PTCP-End TLV
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_END
{
	GSY_WORD_TYPE	TypeLen;		/* 7 Bit Type, 9 Bit Length */
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_END;

#define GSY_TLV_END_SIZE			2
#define GSY_TLV_END_LENGTH			0
#define GSY_TLV_END_TYPELEN			0x0000

/* Values for TLV-Type
*/
#define GSY_TLV_END_TYPE			0
#define GSY_TLV_SUBDOMAIN_TYPE		1
#define GSY_TLV_TIME_TYPE			2
#define GSY_TLV_TIMEEXT_TYPE		3
#define GSY_TLV_MASTER_TYPE			4
#define GSY_TLV_PORTPARAM_TYPE		5
#define GSY_TLV_DELAYPARAM_TYPE		6
#define GSY_TLV_OPTION_TYPE			0x7f

/* Structure of Sync messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_TLV_K32
{
	GSY_TLV_SUBDOMAIN	Subdomain;
	GSY_TLV_TIME		Time;
	GSY_TLV_TIMEEXT		TimeExt;
	GSY_TLV_MASTER		Master;
	/*					[Options] */
	GSY_TLV_END			End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_TLV_K32;

/* Structure of Sync messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_SYNC_PDU
{
	GSY_PTCP_HDR		PtcpHdr;
	GSY_TLV_K32         TLV;
} PNIO_PACKED_ATTRIBUTE_POST GSY_SYNC_PDU;

#define GSY_SYNC_PDU_SIZE \
		(GSY_PDU_HDR_SIZE + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_SUBDOMAIN_SIZE + \
		GSY_TLV_TIME_SIZE + \
		GSY_TLV_TIMEEXT_SIZE + \
		GSY_TLV_MASTER_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_SYNC_RX_SIZE  \
		(GSY_SYNC_PDU_SIZE -\
		GSY_PDU_HDR_SIZE)

/* Structure of Sync-FollowUp messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_SYNC_FU_PDU
{
	GSY_PTCP_HDR		PtcpHdr;
	GSY_TLV_SUBDOMAIN	Subdomain;
	GSY_TLV_TIME		Time;
	/*					[Options] */
	GSY_TLV_END			End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_SYNC_FU_PDU;

#define GSY_SYNC_FU_PDU_SIZE \
		(GSY_PDU_HDR_SIZE + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_SUBDOMAIN_SIZE + \
		GSY_TLV_TIME_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_SYNC_FU_RX_SIZE  \
		(GSY_SYNC_FU_PDU_SIZE -\
		GSY_PDU_HDR_SIZE)

/* Structure of Announce messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_ANNOUNCE_PDU
{
	GSY_PTCP_HDR		PtcpHdr;
	GSY_TLV_SUBDOMAIN	Subdomain;
	GSY_TLV_MASTER		Master;
	/*					[Options] */
	GSY_TLV_END			End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_ANNOUNCE_PDU;

//200410lrg001:
#define GSY_ANNOUNCE_PDU_SIZE \
		(GSY_PDU_HDR_SIZE_VLAN + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_SUBDOMAIN_SIZE + \
		GSY_TLV_MASTER_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_ANNOUNCE_RX_SIZE  \
		(GSY_ANNOUNCE_PDU_SIZE -\
		GSY_PDU_HDR_SIZE_VLAN)

/* Structure of Delay request messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DELAY_REQ_PDU
{
	GSY_PTCP_HDR			PtcpHdr;
//	GSY_TLV_SUBDOMAIN		Subdomain;
	GSY_TLV_DELAYPARAM		DelayParam;
	/*						[Options] */
	GSY_TLV_END				End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_DELAY_REQ_PDU;

#define GSY_DELAY_REQ_PDU_SIZE \
		(GSY_PDU_HDR_SIZE + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_DELAYPARAM_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_DELAY_REQ_RX_SIZE  \
		(GSY_DELAY_REQ_PDU_SIZE -\
		GSY_PDU_HDR_SIZE)

/* Structure of Delay response messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DELAY_RSP_PDU
{
	GSY_PTCP_HDR			PtcpHdr;
	GSY_TLV_DELAYPARAM		DelayParam;
	GSY_TLV_PORTPARAM		PortParam;
	GSY_TLV_PORTTIME		PortTime;
	/*						[Options] */
	GSY_TLV_END				End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_DELAY_RSP_PDU;

#define GSY_DELAY_RSP_PDU_SIZE \
		(GSY_PDU_HDR_SIZE + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_DELAYPARAM_SIZE + \
		GSY_TLV_PORTPARAM_SIZE + \
		GSY_TLV_PORTTIME_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_DELAY_RSP_RX_SIZE \
		(GSY_DELAY_RSP_PDU_SIZE -\
		GSY_PDU_HDR_SIZE)

/* Structure of Delay-FollowUp messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DELAY_FU_PDU
{
	GSY_PTCP_HDR			PtcpHdr;
	GSY_TLV_DELAYPARAM		DelayParam;
	/*						[Options] */
	GSY_TLV_END				End;
} PNIO_PACKED_ATTRIBUTE_POST GSY_DELAY_FU_PDU;

#define GSY_DELAY_FU_PDU_SIZE \
		(GSY_PDU_HDR_SIZE + \
		GSY_PTCP_HDR_SIZE + \
		GSY_TLV_DELAYPARAM_SIZE + \
		GSY_TLV_END_SIZE)

#define GSY_DELAY_FU_RX_SIZE \
		(GSY_DELAY_FU_PDU_SIZE -\
		GSY_PDU_HDR_SIZE)

/* Union of all PTCP messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_PTCP_DATA
{
	GSY_SYNC_PDU			Sync;
	GSY_SYNC_FU_PDU			SyncFu;
	GSY_ANNOUNCE_PDU		Announce;
	GSY_DELAY_REQ_PDU		DelayReq;
	GSY_DELAY_RSP_PDU		DelayRsp;
	GSY_DELAY_FU_PDU		DelayFu;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_DATA;

/* Union of Delay PTCP messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_PTCP_DATA_DELAY
{
	GSY_DELAY_REQ_PDU		DelayReq;
	GSY_DELAY_RSP_PDU		DelayRsp;
	GSY_DELAY_FU_PDU		DelayFu;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_DATA_DELAY;

/* Structure of all PTCP messages
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_PDU
{
	GSY_PDU_HDR				PduHdr;
	GSY_PTCP_DATA			PtcpData;

} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_PDU;

#define GSY_MAX_PDU_SIZE	GSY_SYNC_PDU_SIZE


/* Structure of all PTCP messages without VLAN tag
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_PDU_NO_VLAN
{
	GSY_PDU_HDR_NO_VLAN 	PduHdr;
	GSY_PTCP_DATA			PtcpData;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_PDU_NO_VLAN;

/* Structure of all PTCP messages with VLAN tag
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_PDU_VLAN
{
	GSY_PDU_HDR_VLAN	 	PduHdr;
	GSY_PTCP_DATA			PtcpData;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_PDU_VLAN;


/* Union of all PTCP messages with and without VLAN tag
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE union _GSY_PTCP_PDU_RECV
{
	GSY_PTCP_PDU_VLAN	 	PduVlan;
	GSY_PTCP_PDU_NO_VLAN	PduNoVlan;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_PDU_RECV;


#ifdef GSY_INCLUDE_FOR_KRISC32
#include "pnio_pck_off.h"
/*** Frame buffers: 32 bit aligned ***
*/
#ifdef K32_KRISC32OS
    #include "pnio_k32_pck4_on.h"
#else
    #include "pnio_pck4_on.h"
#endif

/* Struct of K32 receive Time/Clock Sync Framebuffer with local PTCP time
*/
PNIO_PACKED_ATTRIBUTE_PRE struct _K32_RECV_FRAME_BUFFER
{
	LSA_UINT32				Seconds;
	LSA_UINT32				Nanos;
	GSY_PTCP_PDU_VLAN	 	Frame;
	LSA_UINT16				__align_to_32bit;
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _K32_RECV_FRAME_BUFFER K32_RECV_FRAME_BUFFER;

/* Struct of K32 receive delay frame buffer
*/
PNIO_PACKED_ATTRIBUTE_PRE struct _K32_DELAY_FRAME_BUFFER
{
	GSY_PDU_HDR_VLAN	 	PduHdr;
	GSY_PTCP_DATA_DELAY	 	Frame;
	LSA_UINT16				__align_to_32bit;
} 	PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _K32_DELAY_FRAME_BUFFER K32_DELAY_FRAME_BUFFER;
/* Structure of Delay-Req frame without VLAN tag
*/
PNIO_PACKED_ATTRIBUTE_PRE struct _K32_PTCP_DELAY_REQ_FRAME
{
	GSY_PDU_HDR_NO_VLAN		PduHdr;
	GSY_DELAY_REQ_PDU		DelayReq;
//	LSA_UINT16				Padding[9];		//PN-IP: Autopadding?
	LSA_UINT16				__align_to_32bit;
} 	PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _K32_PTCP_DELAY_REQ_FRAME K32_PTCP_DELAY_REQ_FRAME;

/* Structure of Sync frame without VLAN tag
*/
PNIO_PACKED_ATTRIBUTE_PRE struct _K32_PTCP_SYNC_FRAME
{
	GSY_PDU_HDR_NO_VLAN		PduHdr;
	GSY_SYNC_PDU			Sync;
	LSA_UINT16				__align_to_32bit;
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _K32_PTCP_SYNC_FRAME K32_PTCP_SYNC_FRAME;

PNIO_PACKED_ATTRIBUTE_PRE struct _K32_PTCP_FU_FRAME
{
	GSY_PDU_HDR_NO_VLAN		PduHdr;
	GSY_SYNC_FU_PDU			SyncFu;
	LSA_UINT16				__align_to_32bit;
} PNIO_PACKED_ATTRIBUTE_POST;
typedef struct _K32_PTCP_FU_FRAME K32_PTCP_FU_FRAME;

#ifdef K32_KRISC32OS
    #include "pnio_k32_pck_off.h"
#else
    #include "pnio_pck_off.h"
#endif

#include "pnio_pck2_on.h"

typedef K32_PTCP_SYNC_FRAME	GSY_LOWER_TXMEM_ATTR *GSY_SYNC_SEND_FRAME_PTR;
typedef K32_PTCP_FU_FRAME	GSY_LOWER_TXMEM_ATTR *GSY_FU_SEND_FRAME_PTR;

/* Struct of K32 RRQB param TLVs
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PTCP_TLV_K32
{
	GSY_TLV_SUBDOMAIN	Subdomain;
	GSY_TLV_TIME		Time;
	GSY_TLV_TIMEEXT		TimeExt;
	GSY_TLV_MASTER		Master;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PTCP_TLV_K32;

typedef GSY_PTCP_TLV_K32 K32_UPPER_RQB_ATTR	*K32GSY_TLV_PTR;
typedef GSY_TLV_MASTER	 K32_UPPER_RQB_ATTR	*K32GSY_TLV_MASTER_PTR;

#endif // GSY_INCLUDE_FOR_KRISC32

/* SyncData-Record
*/
#define GSY_SYNC_DATA_NAMELEN_MIN	1		/* 290107lrg001 PDSyncData 1.1 */
#define GSY_SYNC_DATA_NAMELEN_MAX	240		/* 290107lrg001 PDSyncData 1.1 */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_SYNC_DATA_RECORD
{
	GSY_WORD_TYPE	BlockType;			/* 00 PDSyncData: GSY_SYNC_DATA_BLOCKTYPE */
	GSY_WORD_TYPE	BlockLength;		/* 02 */
	LSA_UINT8		VersionHigh;		/* 04 1 */
	LSA_UINT8		VersionLow;			/* 05 2 */
	GSY_WORD_TYPE	Padding1;			/* 06 */
	GSY_PTCP_UUID	SubdomainID;		/* 08 Sync Subdomain UUID	*/ 
	GSY_DWORD_TYPE	ResIntervalBegin;	/* 24 */
	GSY_DWORD_TYPE	ResIntervalEnd;		/* 28 */
	GSY_DWORD_TYPE	PLLWindow;			/* 32 max. offset from master clock in ns	*/
	GSY_DWORD_TYPE	SyncSendFactor;		/* 36 Master send interval in 31250 ns		*/
	GSY_WORD_TYPE	SendClockFactor;	/* 40 */
	GSY_WORD_TYPE	TimeoutFactor;		/* 42 Max. time (* SyncSendFactor) w/o master sync for slave timeout */
	GSY_WORD_TYPE	TakeoverFactor;		/* 44 Max. time (* SyncSendFactor) w/o master sync for slave/sec.master takeover	*/
	GSY_WORD_TYPE	MasterStartupTime;	/* 46 290807lrg001: Time in Seconds for Best Master Algorithm (BMA)	*/
	GSY_WORD_TYPE	SyncProperties;		/* 48 Bit0-1 ClockRole: Master/Slave and 170707lrg002: Bit8-12 SyncId			*/
	LSA_UINT8		MasterPriority1;	/* 50 170707lrg002 PDSyncData 1.2 */
	LSA_UINT8		MasterPriority2;	/* 51 170707lrg002 PDSyncData 1.2 */
	LSA_UINT8		LengthSubdomainName;/* 52 290107lrg001 PDSyncData 1.1 */
										/* 53 290107lrg001 PDSyncData 1.1 octet string with variable length */
	LSA_UINT8		SubdomainName[GSY_SYNC_DATA_NAMELEN_MAX];
	LSA_UINT8		Padding2;			/* 293 290107lrg001 PDSyncData 1.2 Length of Struct = 292 */
	GSY_WORD_TYPE	Padding3;			/* 295 290807lrg001 PDSyncData 1.2 Length of Struct = 296 */
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_SYNC_DATA_RECORD;

typedef  GSY_SYNC_DATA_RECORD GSY_UPPER_MEM_ATTR * GSY_UPPER_SYNC_DATA_PTR;

#define GSY_SYNC_DATA_LEN		   	53 /*51, 68*/	/* Arts-RQ 272190 170707lrg002, 290807lrg001 */
#define GSY_SYNC_DATA_IDX			0x802D	/* Real PDSyncData Index for clock sync */
#define GSY_SYNC_TIME_IDX			0x802E	/* Real PDSyncTime Index for time sync TSyncOverPTCP */
#define GSY_SYNC_DATA_BLOCKTYPE		0x0203
#define GSY_SYNC_TIME_BLOCKTYPE		0x0203
#define GSY_SYNC_DATA_BLOCKLEN		(GSY_SYNC_DATA_LEN - 4)

/* Values for SyncData
*/
#define GSY_SYNC_PRIO1_SLAVE		0
#define GSY_SYNC_PRIO1_PRIMARY		1
#define GSY_SYNC_PRIO1_SECONDARY	2
#define GSY_SYNC_PRIO1_LEVEL_MAX	7			/* 170408lrg001: Priority1.Level */
#define GSY_SYNC_PRIO1_ACTIVE 		0x80		/* 070907lrg001: Primary Master is active */
#define GSY_SYNC_PRIO1_MASK_LEVEL	0x78		/* 170408lrg001: Priority1.Level */
#define GSY_SYNC_PRIO1_MASK_PRIO	0x07		/* 170408lrg001 */
#define GSY_SYNC_PRIO1_PRIO(_p1)	(((LSA_UINT8)(_p1)) & GSY_SYNC_PRIO1_MASK_PRIO)
#define GSY_SYNC_PRIO1_LEVEL(_p1)	((((LSA_UINT8)(_p1)) & GSY_SYNC_PRIO1_MASK_LEVEL) >> 3)
#define GSY_SYNC_PRIO2_DEFAULT		255
#define GSY_SYNC_ROLE_LOCAL			0
#define GSY_SYNC_ROLE_SLAVE			1
#define GSY_SYNC_ROLE_MASTER		2
#define GSY_SYNC_SENDFACTOR_MIN		1			/* 31250 nanoseconds */
#define GSY_SYNC_SENDFACTOR_MAX		0xA4CB8000	/* 24 hours */
//#define GSY_SYNC_SENDFACTOR_MIN	32			/* 1 Millisecond */
//#define GSY_SYNC_SENDFACTOR_MAX	1000		/* 32 Milliseconds */
#define GSY_SYNC_TIMEOUT_DISABLED	0
#define GSY_SYNC_TIMEOUT_MIN		0			/* no timeout */
#define GSY_SYNC_TIMEOUT_MAX		511			/* 511 * SyncSendFactor */
#define GSY_SYNC_TAKEOVER_DISABLED	0
#define GSY_SYNC_TAKEOVER_MIN		0			/* disabled */
#define GSY_SYNC_TAKEOVER_MAX		511			/* 511 * SyncSendFactor */
#define GSY_SYNC_STARTUP_DISABLED	0
#define GSY_SYNC_STARTUP_MIN		0
#define GSY_SYNC_STARTUP_MAX		300			/* 0x12C: 5 Minuten */
#define GSY_SYNC_PLLWINDOW_MAX		10000000	/* 10 Milliseconds */
#define GSY_SYNC_CLOCK_BASE			31250		/* Nanoseconds in 31,25µs */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DOMAIN_BOUNDARY
{
	GSY_DWORD_TYPE	DomainBoundary;
	GSY_WORD_TYPE	Properties;
	GSY_WORD_TYPE	Padding2;
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_DOMAIN_BOUNDARY;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DOMAIN_BOUNDARY1
{
	GSY_DWORD_TYPE	DomainBoundaryIngress;
	GSY_DWORD_TYPE	DomainBoundaryEgress;
	GSY_WORD_TYPE	Properties;
	GSY_WORD_TYPE	Padding2;
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_DOMAIN_BOUNDARY1;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PEER_BOUNDARY
{
	GSY_DWORD_TYPE	PeerBoundary;
	GSY_WORD_TYPE	Properties;
	GSY_WORD_TYPE	Padding2;
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_PEER_BOUNDARY;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PORT_DATA_SUBBLOCK
{
	GSY_WORD_TYPE	BlockType;
	GSY_WORD_TYPE	BlockLength;
	LSA_UINT8		VersionHigh;
	LSA_UINT8		VersionLow;
	GSY_WORD_TYPE	Padding1;
    union {
        GSY_DOMAIN_BOUNDARY1    Domain1;	/* VersionLow = 1 */
        GSY_DOMAIN_BOUNDARY     Domain;		/* VersionLow = 0 */
        GSY_PEER_BOUNDARY       Peer;
    } Boundary;
} PNIO_PACKED_ATTRIBUTE_POST GSY_PORT_DATA_SUBBLOCK;

/* PortDataAdjust-Record
*/
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PORT_DATA_HEADER
{
	GSY_WORD_TYPE			BlockType;			/* 00 PDPortDataAdjust */
	GSY_WORD_TYPE			BlockLength;		/* 02 */
	LSA_UINT8				VersionHigh;		/* 04 */
	LSA_UINT8				VersionLow;			/* 05 */
	GSY_WORD_TYPE			Padding1;			/* 06 */
	GSY_WORD_TYPE			SlotNumber;			/* 08 */
	GSY_WORD_TYPE			SubslotNumber;		/* 10 */
												/* 12 */
} PNIO_PACKED_ATTRIBUTE_POST GSY_PORT_DATA_HEADER;

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_PORT_DATA_RECORD
{
	GSY_PORT_DATA_HEADER	Header;
	GSY_PORT_DATA_SUBBLOCK	SubBlock;
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_PORT_DATA_RECORD;

typedef  GSY_PORT_DATA_RECORD  	GSY_UPPER_MEM_ATTR * GSY_UPPER_PORT_DATA_PTR;
typedef  GSY_PORT_DATA_SUBBLOCK	GSY_UPPER_MEM_ATTR * GSY_UPPER_SUBBLOCK_PTR;
typedef  GSY_DOMAIN_BOUNDARY	GSY_UPPER_MEM_ATTR * GSY_UPPER_ADJ_DOMAIN_PTR;
typedef  GSY_PEER_BOUNDARY		GSY_UPPER_MEM_ATTR * GSY_UPPER_ADJ_PEER_PTR;

#define GSY_PORT_DATA_IDX				0x802F	/* PDPortDataAdjust Index */
#define GSY_PORT_DATA_BLOCKTYPE			0x0202	/* Block type PortDataAdjust */
#define GSY_PORT_DATA_HDR_LEN			12		/* Length of GSY_PORT_DATA_HEADER */
#define GSY_BLOCK_TYPE_LEN_SIZE  		4       /* Size of BlockType + BlockLength */
#define GSY_BLOCK_HEADER_SIZE			6       /* Size of BlockType + BlockLength + VersionHigh + VersionLow */
#define GSY_SUBBLOCK_HEADER_SIZE		8       /* Size of BlockType + BlockLength + VersionHigh + VersionLow + Padding1 */
#define GSY_DOMAIN_BOUNDARY_BLOCKTYPE	0x0209	/* SubBlock type DomainBoundary */
#define GSY_DOMAIN_BOUNDARY_BLOCKLEN	12		/* SubBlock length DomainBoundary */
#define GSY_DOMAIN_BOUNDARY1_BLOCKLEN	16		/* SubBlock length DomainBoundary Version 1*/
#define GSY_PEER_BOUNDARY_BLOCKTYPE		0x0224	/* SubBlock type PeerToPeerBoundary */
#define GSY_PEER_BOUNDARY_BLOCKLEN		12		/* SubBlock length PeerToPeerBoundary */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_SEND_CLOCK_RECORD /* 051206lrg001 */
{
	GSY_WORD_TYPE			BlockType;			/* 00 SendClockFactor */
	GSY_WORD_TYPE			BlockLength;		/* 02 */
	LSA_UINT8				VersionHigh;		/* 04 */
	LSA_UINT8				VersionLow;			/* 05 */
	GSY_WORD_TYPE			SendClockFactor;	/* 06 */
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_SEND_CLOCK_RECORD;

typedef  GSY_SEND_CLOCK_RECORD 		GSY_UPPER_MEM_ATTR * GSY_UPPER_SEND_CLOCK_PTR;

#define GSY_SEND_CLOCK_IDX				0x10000	/* SendClockFactor Record-Index */
#define GSY_SEND_CLOCK_BLOCKTYPE		0xF000	/* SendClockFactor Blocktype */
#define GSY_SEND_CLOCK_DEFAULT			32		/* SendClockFactor default value = 1ms */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_SYNC_PLL_RECORD	// 230910lrg001: PDSyncPLL record
{
	GSY_WORD_TYPE		BlockType;			// 00 GSY_SYNC_PLL_BLOCKTYPE
	GSY_WORD_TYPE		BlockLength;		// 02 GSY_SYNC_PLL_BLOCKLENGTH
	LSA_UINT8			VersionHigh;		// 04 GSY_SYNC_PLL_VERSIONHIGH
	LSA_UINT8			VersionLow;			// 05 GSY_SYNC_PLL_VERSIONLOW
	GSY_WORD_TYPE		ReductionRatio;		// 06 Range: 1,2,4,8,16
	GSY_WORD_TYPE		PLLHWDelay;			// 08 Range GSY_SYNC_PLL_DELAYMIN … GSY_SYNC_PLL_DELAYMAX ns
	GSY_WORD_TYPE		ControlInterval;	// 10 131210lrg001: control timer interval in ms
	GSY_DWORD_TYPE		PLLWindow;			// 12 131210lrg001: max. offset from master clock in ns
	GSY_WORD_TYPE		SetWaitCount;		// 16 310511lrg: multiple of ControlInterval to wait after setting time
	GSY_WORD_TYPE		Reserved;			// 18 310511lrg: padding
	 
} PNIO_PACKED_ATTRIBUTE_POST GSY_SYNC_PLL_RECORD;

typedef  GSY_SYNC_PLL_RECORD 		GSY_UPPER_MEM_ATTR * GSY_UPPER_SYNC_PLL_PTR;

#define GSY_SYNC_PLL_IDX			0x11020	// Record-Index
#define GSY_SYNC_PLL_BLOCKTYPE		0x7000	// Blocktype
#define GSY_SYNC_PLL_BLOCKLENGTH	16		// Blocktlength 310511lrg: 12 -> 16
#define GSY_SYNC_PLL_VERSIONHIGH	1		// VersionHigh
#define GSY_SYNC_PLL_VERSIONLOW		1		// VersionLow 310511lrg: 0 -> 1
#define GSY_SYNC_PLL_DELAYMIN		1		// min. PLLHWDelay
#define GSY_SYNC_PLL_DELAYMAX		10000	// max. PLLHWDelay

/* 
 * DiagData-Record (in network byte order)
*/
#define	GSY_DIAG_DATA_IDX			0xb000	/* Base Index of DiagData record (=SyncLogbook) */
#define	GSY_DIAG_BLOCK_COUNT		100		/* Count of blocks in record */
#define	GSY_DIAG_BLOCK_LEN			80		/* Packed size of one record block struct */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DIAG_DATA_BLOCK
{											/* actual if GSY_DIAG_SOURCE...	(_XXX = always) */
	GSY_DWORD_TYPE		MasterSeconds;		/* 00 _OFFSET: Seconds of master time */
	GSY_DWORD_TYPE		MasterNanoseconds;	/* 04 _OFFSET: Nanoseconds of master time */
	GSY_DWORD_TYPE		ExtPllOffset;		/* 08 _xXX:    Signed local offset to Ext_PLL signal in +-ns */
	GSY_DWORD_TYPE		LocalOffset;		/* 12 _OFFSET: Signed local offset to master time in +-ns (CLP input) */
	GSY_DWORD_TYPE		AdjustInterval;		/* 16 _ADJUST: Signed CLP output in +-ns */
	GSY_DWORD_TYPE		DriftInterval;		/* 20 _DRIFT:  Signed Local drift to master time in +-ns */
	GSY_DWORD_TYPE		LineDelay;			/* 24 _DELAY:  Line delay at port [PortId] in ns */
	GSY_DWORD_TYPE		SyncDelay;			/* 28 _OFFSET: SyncFrame delay in ns (0xffffffff = >= 4.294.967.295) */
	GSY_DWORD_TYPE		RecvDelay;			/* 32 _OFFSET: Delay from RcvTimestamp to RecvIndication in ns */
	GSY_WORD_TYPE		PortId;				/* 36 _DELAY:  Line delay at port LineDelayPort in ns */
	GSY_MAC_ADR_TYPE	MasterMacAddr;		/* 38 _DRIFT:  Current master source MAC address */
	LSA_UINT8			SyncId;				/* 44 _XXX:    Sync ID 0 = clock, 1 = time,... */
	LSA_UINT8			ComClass;	  		/* 45 _XXX:    0 = RTClass2(RTA) , 1 = RTClass3(RTC) */
	LSA_UINT8			SlaveState;	  		/* 46 _XXX:    see Values for Slave.State in gsy_int.h  */
											/*             GSY_SLAVE_STATE_OUT (=JitterOutOfBounday), */
											/*             GSY_SLAVE_STATE_WAIT (=NoMsgReceived)... */
	LSA_UINT8			DiagSource;  		/* 47 _XXX:    see #define GSY_DIAG_SOURCE_... */
	GSY_WORD_TYPE		BlockNo;			/* 48 _XXX:    sequence number of this block */
	GSY_WORD_TYPE		SeqId;				/* 50 _XXX:    sequence id of last sent(master) or received (slave) sync frame */
	GSY_DWORD_TYPE		TimeStamp;			/* 52 _XXX:    ERTEC-Timestamp register in 10 ns ticks */
	GSY_DWORD_TYPE		LocalTimeHigh;		/* 56 _SET:    HighPart  of  local time AP01191740: Seconds of PTCP time */
	GSY_DWORD_TYPE		LocalTimeLow;		/* 60 _SET:    LowPart of local time AP01191740: Nanoseconds of PTCP time */
	GSY_DWORD_TYPE		ClpTermPHigh;		/* 64 _ADJUST: Signed high Part of 64bit proportional term from PI controller */
	GSY_DWORD_TYPE		ClpTermPLow;		/* 68 _ADJUST:        low  Part of 64bit proportional term from PI controller */
	GSY_DWORD_TYPE		ClpTermIHigh;		/* 72 _ADJUST: Signed high Part of 64bit integral term from PI controller */
	GSY_DWORD_TYPE		ClpTermILow;		/* 76 _ADJUST:        low  Part of 64bit integral term from PI controller */

} PNIO_PACKED_ATTRIBUTE_POST GSY_DIAG_DATA_BLOCK;

#define	GSY_DIAG_SOURCE_OFFSET		1		/* Block change value is LocalOffset */
#define	GSY_DIAG_SOURCE_ADJUST		2		/* Block change value is AdjustInterval */
#define	GSY_DIAG_SOURCE_DRIFT		3		/* Block change value is DriftInterval */
#define	GSY_DIAG_SOURCE_DELAY		4		/* Block change value is LineDelay and PortId */
#define	GSY_DIAG_SOURCE_SET			5		/* Block change value is LocalTime */
#define	GSY_DIAG_SOURCE_SYNC		6		/* No block change value */
#define	GSY_DIAG_SOURCE_SUBDOMAIN	7		/* No block change value */
#define	GSY_DIAG_SOURCE_MASTER		8		/* No block change value */

typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_DIAG_DATA_RECORD
{
    GSY_DIAG_DATA_BLOCK	DiagBlock[GSY_DIAG_BLOCK_COUNT];

} PNIO_PACKED_ATTRIBUTE_POST GSY_DIAG_DATA_RECORD;

typedef  GSY_DIAG_DATA_BLOCK GSY_UPPER_MEM_ATTR * GSY_UPPER_DIAG_BLOCK_PTR;

/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

#include "pnio_pck_off.h"

/* packed structures end                                                     */

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

#define GSY_MODULE_ID_SYS 		1
#define GSY_MODULE_ID_USR 		2
#define GSY_MODULE_ID_LOW 		3
#define GSY_MODULE_ID_DEL 		4
#define GSY_MODULE_ID_FWD 		5
#define GSY_MODULE_ID_BMA 		6
#define GSY_MODULE_ID_MSM 		7
#define GSY_MODULE_ID_CLP 		8

/* Supported SyncIDs 0 and 1
*/
#if (GSY_SYNC_ID_SUPPORTED == 0x00000003)
#define GSY_MAX_PROTOCOLS		2
#define GSY_NUM_PROTOCOLS		2
#elif (GSY_SYNC_ID_SUPPORTED == 0x00000002)
#define GSY_MAX_PROTOCOLS		2
#define GSY_NUM_PROTOCOLS		1
#elif (GSY_SYNC_ID_SUPPORTED == 0x00000001)
#define GSY_MAX_PROTOCOLS		1
#define GSY_NUM_PROTOCOLS		1
#endif
/* --------------------------------------------------------------------------*/  
/* GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL                                      */
/*                                                                           */  
/* Forwarding module:                                                        */  
/* Maximum number of masters supported per protocol                          */  
/*                                                                           */  
/* --------------------------------------------------------------------------*/  

#define GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL   4

/* 5-Bit mask for sync ID
*/
#define GSY_SYNCID_MASK				GSY_SYNCID_NONE
#define GSY_SYNCID_MAX				GSY_SYNCID_TIME

/* Multicast addresses for PTCP telegrams
*/
#define GSY_MULTICAST_ADDR_DELAY	{0x01, 0x80, 0xC2, 0x00, 0x00, 0x0E}
#define GSY_MULTICAST_ADDR_SYNC_IRT	{0x01, 0x0E, 0xCF, 0x00, 0x01, 0x02}
#define GSY_MULTICAST_ADDR_SYNC		{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x80}
#define GSY_MULTICAST_ADDR_TIME		{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x81}
#define GSY_MULTICAST_ADDR_FUSYNC	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x20}
#define GSY_MULTICAST_ADDR_FUTIME	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x21}
#define GSY_MULTICAST_ADDR_SYNC_FU	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x40}
#define GSY_MULTICAST_ADDR_TIME_FU	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x41}
#define GSY_MULTICAST_ADDR_ANNOUNCE	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x00}
#define GSY_MULTICAST_ADDR_ANNO_MAX	{0x01, 0x0E, 0xCF, 0x00, 0x04, 0x1F}

/* Frame IDs for GSY PTCP telegrams
*/
#define GSY_FRAMEID_SYNC			0x0080
#define GSY_FRAMEID_TIME			0x0081
#define GSY_FRAMEID_FUSYNC			0x0020
#define GSY_FRAMEID_FUTIME			0x0021
#define GSY_FRAMEID_SYNC_FU			0xFF20
#define GSY_FRAMEID_TIME_FU			0xFF21
#define GSY_FRAMEID_ANNOUNCE		0xFF00
#define GSY_FRAMEID_DELAY_REQ		0xFF40
#define GSY_FRAMEID_DELAY_FURSP		0xFF41
#define GSY_FRAMEID_DELAY_FU		0xFF42
#define GSY_FRAMEID_DELAY_RSP		0xFF43

#define GSY_FRAMEID_SYNC_MIN		GSY_FRAMEID_SYNC
#define GSY_FRAMEID_SYNC_MAX		(GSY_FRAMEID_SYNC + 0x1F)
#define GSY_FRAMEID_FUSYNC_MIN		GSY_FRAMEID_FUSYNC
#define GSY_FRAMEID_FUSYNC_MAX		(GSY_FRAMEID_FUSYNC + 0x1F)
#define GSY_FRAMEID_FU_MIN			GSY_FRAMEID_SYNC_FU
#define GSY_FRAMEID_FU_MAX			(GSY_FRAMEID_SYNC_FU + 0x1F)
#define GSY_FRAMEID_ANNOUNCE_MIN	GSY_FRAMEID_ANNOUNCE
#define GSY_FRAMEID_ANNOUNCE_MAX	(GSY_FRAMEID_ANNOUNCE + 0x1F)

/* Extract SyncId from FrameId (= *pPdu) of a PDU
*/
#define GSY_SYNCID_GET_FROM_PDU(pPdu)	(*(((GSY_LOWER_RXBYTE_PTR)pPdu)+1) & GSY_SYNCID_MASK)

/* EtheType and VlanTag of GSY PTCP telegrams (Priority = 7)
 * +- 2 Byte Type -+- 3 Bit Prio -+- 1 Bit 0 -+- 12 Bit ID -+
*/
#define GSY_ETHETYPE				0x8892
#ifndef GSY_VLAN_TAG
#define GSY_VLAN_TAG				{0x81, 0x00, 0xE0, 0x00}
#endif

/* PTCP constants
*/
#define GSY_NO_MASTER_ADDR		{0,0,0,0,0,0}
#define GSY_NO_SUBDOMAIN		{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

/* Constants for time stamps
*/
#define GSY_TS_WRAP_LOW				1000000000U				// 10 seconds in 10ns ticks
#define GSY_TS_WRAP_HIGH			(4294967295U-GSY_TS_WRAP_LOW)

#define GSY_NANOS_PER_MILLISECOND	1000000
#define GSY_10N_PER_SECOND			100000000
#define GSY_NANOS_PER_SECOND		1000000000
#define GSY_NANOS_PER_DWORD			4294967295U
#define GSY_SECONDS_PER_DWORD		4
#define GSY_MAX_NANOS				 294967295
#define GSY_MAX_NANOS_PER_INT32		0x7FFFFFFF
#define GSY_MIN_NANOS_PER_INT32		(LSA_INT32)0x80000000
#define GSY_MAX_NANOS_PER_INT32_64	 2147483647LL
#define GSY_MIN_NANOS_PER_INT32_64	-2147483648LL

/* How often is 31250ns in 1 millisecond?
*/
#define GSY_CLOCK_BASE_PER_MILLISECOND	32

/* Max. allowed deviation to the average value (ns) at delay measurement
 * and allowed count of invalid deviations at the begin of the measurement
 * and max. count of req/rsp for an average delay value
 * 040608lrg001: GSY_DELAY_NUM_ERROR = count of errors until delay reset to 0
*/
#define GSY_DELAY_MAX_DIST			100
#define GSY_DELAY_IGNORE_DIST_CNT	5
#define GSY_DELAY_MAX_COUNT			4
#define GSY_DELAY_NUM_ERROR			3

#define GSY_DELAY_FILTER					//AP01384863: DelayFilter
#ifdef GSY_DELAY_FILTER
#define GSY_DELAY_FILTER_FACTOR		1000000	// for resolution
#define GSY_DELAY_FILTER_MIN		2		// for inertia
#define GSY_DELAY_FILTER_MAX		1000	// for inertia
#define GSY_DELAY_FILTER_INC		1		// for inertia
#endif

/* max. allowed bridge delay (ns) of the responders (0 = no limit)
 * default: 100ms
*/
#define GSY_RSPDELAY_MAX			100000000
#define GSY_DELAY_CABLE_MIN			-50		// Minimum average cable delay

/* Count of sample values for one average value at drift measurement
 * and min. allowed value of a RCF interval (ns)
 * 100107lrg003: replacement (max) interval for "no rate" and Median out of 7 samples
*/
#define GSY_DRIFT_MAX_COUNT			7
#define GSY_DRIFT_MIN_INTERVAL		4000		// 250 ppm
#define GSY_DRIFT_MAX_INTERVAL		0x7FFFFFFF	// 0 ppm

/* Max. allowed deviation to the average value (ns) at synchronisation
 * and allowed count of invalid deviations at the begin
 * and max. count of single offsets for an average offsets value
 * (if == 0: automatic limitation by slave SyncInterval)
*/
#define GSY_SYNC_MAX_DIST			5000
#define GSY_SYNC_IGNORE_DIST_CNT	5
#define GSY_SYNC_MAX_COUNT			1

/* If the offset is greater than this multiple of the PLL window
 * the clock will be set to new time by GSY_SYNC_SET()
*/
#define GSY_SYNC_WINDOW_RESET_FACTOR	5

/* Max. allowed difference from old to new sequence ID
 * of two sequentially received sync frames
*/
#define GSY_SEQID_MAX_DIST			0xF000		//250711lrg was 4096

/* Count of Send-/Receive-/DelaySet-RQBs per Port
 * 101208lrg/1126470: GSY_NUM_RQB_ANNOUNCE_RECV/per protocol
*/
#define GSY_NUM_RQB_MASTER_SEND		2	/* for each master Sync and FollowUp */
#define GSY_NUM_RQB_DELAY_SEND		3	/* for all Delay-Request -Response and -FollowUp */
#define GSY_NUM_RQB_DELAY_RECV		3	/* for all Delay-Responseaund -FollowUp */
#define GSY_NUM_RQB_DELAY_SET		1	/* fuer seting the LineDelay in EDD */
#define GSY_NUM_RQB_RATE_SET		1	/* fuer seting the Rate(Drift) in EDD */
#define GSY_NUM_RQB_DELAY_CTRL		2	/* for control of delay measurement in KRISC32 per port */
#define GSY_NUM_RQB_DIAG_CTRL		6	/* for diag of clock and time sync in KRISC32 */
#define GSY_NUM_RQB_SYNC_CTRL		10	/* for start, stop and diag of clock and time sync in KRISC32 */
#define GSY_NUM_RQB_ANNOUNCE_RECV	GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL

/* for all Slave Sync and FollowUp per port:
*/
#define GSY_NUM_RQB_SYNCFU_RECV		(2*GSY_NUM_PROTOCOLS*GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL)

/* Index value of multi user ID
*/
#define GSY_HANDLE_UID				0
#define GSY_TYPE_UID				1

/* Values of user_id.uvar8_array[GSY_TYPE_UID]
 * 101208lrg: receive Announce with GSY_LOWER_RECV
*/
#define GSY_ANNO_TYPE_ID			0x20
#define GSY_DELAY_TYPE_ID			0x40
#define GSY_SYNC_TYPE_ID			0x80

/* Values for DelayTimer:
 * Start with GSY_DELAY_TIME_START (* GSY_DELAY_TIME_BASE)
 * 030407lrg001: on local RCF measurement: keep this interval
*/
#ifndef GSY_DELAY_TIME_BASE
#define GSY_DELAY_TIME_BASE			LSA_TIME_BASE_100MS
#define GSY_DELAY_TIME_START		2

/* 101007lrg002: not longer ramp, sequence of measurements:
 * 5 times all 8 seconds, on error: pause 1 second
*/
#define GSY_DELAY_TIME_CONT			80
#define GSY_DELAY_TIME_NEW			10
#endif

/* Values for the cyclic drift timer
 * and the transient master phase
*/
#define GSY_DRIFT_TIME_BASE			LSA_TIME_BASE_100MS
#define GSY_DRIFT_TIME_VALUE		2
#define GSY_DRIFT_TIME_MILLIS		(100*GSY_DRIFT_TIME_VALUE)	// FT1204141
#define GSY_DRIFT_TIME_OUT_1		(200*6)						// FT1204141: timeout for SyncId 1
#define GSY_DRIFT_DECRAY_FACTOR		2			/* double RCF interval */
#define GSY_DRIFT_DECRAY_MAX		1000000		/* til 1 ns correction per 1 ms */

//#define GSY_ANNO_SEND_FACTOR_CLOCK	10		// 161007lrg001
#define GSY_ANNO_SEND_FACTOR_CLOCK	3
#define GSY_ANNO_SEND_FACTOR_TIME	3

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

#ifndef GSY_INCLUDE_FOR_KRISC32
#define GSY_CH_USR_PTR struct _GSY_CH_USR		*
#define GSY_CH_SYS_PTR struct _GSY_CH_SYS		*
#define GSY_CH_ANNO_PTR struct _GSY_CH_ANNO		*
#endif
#define GSY_CH_K32_PTR struct _GSY_CH_K32		*
#define GSY_TIMER_PTR  struct _GSY_TIMER		*
#define GSY_DRIFT_PTR  struct _GSY_DRIFT_TYPE	*

/* Best Master List structure
*/
typedef struct _GSY_BMA_TYPE
{
	GSY_MAC_ADR_TYPE			MacAddr;
	LSA_UINT8					Priority1;
	LSA_UINT8					Priority2;
	LSA_UINT8					Class;
	LSA_UINT8					Accuracy;
	LSA_UINT16					Variance;
	LSA_INT						Receipt;

} GSY_BMA_TYPE;
typedef GSY_BMA_TYPE *			GSY_BMA_PTR_TYPE;

/* Values for Receipt
*/
#define GSY_ENTRY_SM_OLD		0
#define GSY_ENTRY_SM_USED		1
#define GSY_ENTRY_SM_UNUSED		-1

/* --------------------------------------------------------------------------*/
/* TLV                                                                       */
/* --------------------------------------------------------------------------*/

typedef struct _GSY_TLV_SYNC_PARAM_TYPE
{
	GSY_BMA_TYPE		Master;
	LSA_UINT16			SequenceID;
	LSA_BOOL			FUFlag;          /* LSA_TRUE: with FollowUp */
	LSA_UINT8			Delay1NS;        /* 1 NS Part (0..9)                   */
	LSA_UINT16			TimeFlags;       /* Master time flags                  */
	LSA_UINT16			UTCOffset;       /* Master time CurrentUTCOffset       */
	LSA_UINT16			Epoch;           /* Master time epoch                  */
	LSA_UINT32			Seconds;         /* Master time seconds                */
	LSA_UINT32			Nanos;           /* Master time nanoseconds            */
	LSA_UINT32			DelayNS;         /* NS Part (unsigned)                 */
	LSA_UINT32			Delay10NS;       /* 10 NS Part                         */
	LSA_UINT32			DelayNSOffset;   /* Offset within PDU of NS field      */
	LSA_UINT32			Delay1NSOffset;  /* Offset within PDU of 1 NS  field   */
	LSA_UINT32			Delay10NSOffset; /* Offset within PDU of 10 NS field   */
} GSY_TLV_SYNC_PARAM_TYPE;

typedef struct _GSY_TLV_FU_PARAM_TYPE
{
	GSY_BMA_TYPE		Master;
	LSA_UINT16			SequenceID;
	LSA_INT32			DelayNS;         /* NS Part (signed)                   */
//	LSA_UINT8			Delay1NS;        /* 1 NS Part (0..9)                   */
//	LSA_UINT32			Delay10NS;       /* 10 NS Part                         */
	LSA_UINT32			DelayNSOffset;   /* Offset within PDU of 1 PS  field   */
//	LSA_UINT32			Delay1NSOffset;  /* Offset within PDU of 1 NS  field   */
//	LSA_UINT32			Delay10NSOffset; /* Offset within PDU of 10 NS field   */
} GSY_TLV_FU_PARAM_TYPE;

typedef struct _GSY_TLV_ANNOUNCE_PARAM_TYPE
{
	GSY_BMA_TYPE		Master;
	LSA_UINT16			SequenceID;
} GSY_TLV_ANNOUNCE_PARAM_TYPE;

typedef struct _GSY_TLV_INFO_TYPE
{
	LSA_UINT32			Type;
	LSA_UINT16			Length;
	GSY_DRIFT_PTR		pDrift;
	LSA_BOOL			RateReady;
	LSA_UINT8			SyncId;
	GSY_PTCP_UUID		Subdomain;

	union _GSY_TLV_PARAMS_TYPE
	{
		GSY_TLV_SYNC_PARAM_TYPE		Sync;
		GSY_TLV_FU_PARAM_TYPE    	FollowUp;
		GSY_TLV_ANNOUNCE_PARAM_TYPE	Announce;
	} Param;

} GSY_TLV_INFO_TYPE;

typedef struct _GSY_TLV_INFO_TYPE       * GSY_TLV_INFO_PTR_TYPE;
typedef struct _GSY_TLV_INFO_TYPE const * GSY_TLV_INFO_CONST_PTR_TYPE;

/* Type */

#define GSY_TLV_TYPE_NONE            0
#define GSY_TLV_TYPE_SYNC            1
#define GSY_TLV_TYPE_FOLLOWUP        2
#define GSY_TLV_TYPE_ANNOUNCE        3

#ifdef GSY_INCLUDE_FOR_KRISC32

/* KRISC32 GSY timer
*/
typedef struct _GSY_TIMER
{
	LSA_BOOL				Running;
	LSA_UINT32				Time;
	LSA_TIMER_ID_TYPE		Id;
	LSA_UINT16				Port;
	GSY_CH_K32_PTR			pChSys;
    LSA_VOID LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,
							pTimeoutProc)
							(GSY_TIMER_PTR pTimer);
} GSY_TIMER;

#else  //not GSY_INCLUDE_FOR_KRISC32

/* GSY timer
*/
typedef struct _GSY_TIMER
{
#ifdef GSY_TIMER_CHECK
	LSA_UINT32				Timestamp;
	LSA_UINT64				BaseTicks;
	LSA_UINT64				TimeTicks;
#endif
	LSA_BOOL				InUse;
	LSA_BOOL				Running;
	LSA_BOOL				Waiting;		//1605lrg: for slave timer restart
	LSA_BOOL				Restart;		//1605lrg: for slave timer restart
	LSA_UINT16				Type;
	LSA_UINT16				Time;
	LSA_TIMER_ID_TYPE		Id;
	LSA_UINT16				Port;
	GSY_CH_SYS_PTR			pChSys;
	GSY_CH_USR_PTR			pChUsr;
    LSA_VOID LSA_FCT_PTR(GSY_LOCAL_FCT_ATTR,
							pTimeoutProc)
							(GSY_TIMER_PTR pTimer);
#ifdef GSY_DO_TIMEOUT_EXTERN
    GSY_UPPER_RQB_PTR_TYPE	RbTimerPtr;
	LSA_BOOL				RbInUse;
#endif
} GSY_TIMER;


/* GSY reqest block queue structures
*/
typedef struct _GSY_UPPER_QUEUE
{
	LSA_UINT16				Count;
	GSY_UPPER_RQB_PTR_TYPE	pFirst;
	GSY_UPPER_RQB_PTR_TYPE	pLast;

} GSY_UPPER_QUEUE;

typedef struct _GSY_LOWER_QUEUE
{
	LSA_UINT8				Count;
	GSY_LOWER_RQB_PTR_TYPE	pFirst;
	GSY_LOWER_RQB_PTR_TYPE	pLast;

} GSY_LOWER_QUEUE;

#endif //GSY_INCLUDE_FOR_KRISC32

/* Values of actual, new and old sync telegrams for drift calculation
*/
typedef struct _GSY_TIME_TYPE
{
	LSA_UINT16					OrgEpoch;
	LSA_UINT32					OrgSeconds;
	LSA_UINT32					OrgNanos;
//#ifdef GSY_INCLUDE_FOR_KRISC32				lrgi210
	LSA_UINT32					LocalSeconds;
	LSA_UINT32					LocalNanos;
//#endif //GSY_INCLUDE_FOR_KRISC32
	LSA_UINT32					DelaySeconds;
	LSA_UINT32					DelayNanos;
	LSA_UINT32					Timestamp;
	LSA_UINT32					IndTimestamp;
	LSA_UINT16					PortId;
	LSA_UINT16					SeqId;
	LSA_BOOL					Used;
} GSY_TIME_TYPE;

/* Structure for diagnose data
*/
typedef struct _GSY_DIAG_DATA_TYPE
{
	LSA_UINT32								MasterSeconds;
	LSA_UINT32								MasterNanoseconds;
	LSA_INT32								Offset;
	LSA_INT32								DriftInterval;
	LSA_INT32								AdjustInterval;
	LSA_INT32								UserValue1;
	LSA_INT32								UserValue2;
	GSY_MAC_ADR_TYPE						MasterMacAddr;
	LSA_BOOL								Subdomain;
	LSA_UINT8								SyncId;
	LSA_BOOL								StateSync;
//	LSA_BOOL								PrimaryMaster;	170408lrg002
	LSA_BOOL								RateValid;
	LSA_UINT8								RcvSyncPrio;	//170408lrg002
	LSA_UINT8								LocalPrio;		//170408lrg002
	LSA_UINT8								PLLState;		//230910lrg001
#ifndef GSY_INCLUDE_FOR_KRISC32
	GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE	Ind;
#else
	K32_RQB_PTR_TYPE						pK32Rqb;
#endif //GSY_INCLUDE_FOR_KRISC32
} GSY_DIAG_DATA_TYPE;

/* Average Slave Drift structure
*/
typedef struct _GSY_DRIFT_TYPE
{
	LSA_UINT8					SyncId;
	LSA_UINT8					State;
	LSA_UINT16					OldSeqId;
	GSY_MAC_ADR_TYPE			OldMasterAddr;
	GSY_MAC_ADR_TYPE			MasterAddr;
	GSY_PTCP_UUID				Subdomain;
	LSA_BOOL					CorrectPlus;
	LSA_UINT32					CorrectInterval;
	LSA_INT						NextIdx;				//180108lrg001
	GSY_TIME_TYPE				Old[GSY_DRIFT_MAX_COUNT];
	GSY_TIME_TYPE				New[GSY_DRIFT_MAX_COUNT];
	GSY_TIME_TYPE				Act;
	LSA_UINT32					TimeoutMillis;		// FT1204141
	LSA_INT32					AveInterval;
	LSA_INT32					AveMinInterval;
	LSA_INT32					AveMaxInterval;
	LSA_INT32					AveActInterval[GSY_DRIFT_MAX_COUNT];
    LSA_INT32                   IntervalSet;		/* Interval SET       310707js001*/
    LSA_BOOL                    IntervalSetValid;	/* Interval SET valid 310707js001*/
	LSA_UINT8					RcvSyncPrio;		//170408lrg002
	LSA_UINT8					LocalPrio;			//170408lrg002
	LSA_UINT16					AveSumCount;
	LSA_UINT16					AveOldest;
	LSA_INT32					DiagCount;
	GSY_DIAG_DATA_TYPE			Diag;
#ifndef GSY_INCLUDE_FOR_KRISC32
	LSA_UINT8					SyncIdRate;			//1684006
	LSA_UINT8					PendingSyncId;		//1684006
    LSA_BOOL                    PendingSet;			//290909lrg001
	LSA_UINT8					PendingState;		//290909lrg001
	LSA_INT32					PendingInterval;	//290909lrg001
#endif //GSY_INCLUDE_FOR_KRISC32
} GSY_DRIFT_TYPE;

/* Values for Drift.State
*/
#define GSY_DRIFT_STATE_UNKNOWN		0
#define GSY_DRIFT_STATE_TRANSIENT	1
#define GSY_DRIFT_STATE_MASTER		2
#define GSY_DRIFT_STATE_READY		3

/* Additional values for RcfLocal.State
*/
#define GSY_DRIFT_STATE_WAIT		4
#define GSY_DRIFT_STATE_NONE		10

typedef struct _GSY_RCF_LOCAL
{
	LSA_UINT32					T1Old;
	LSA_UINT32					T1Act;
	LSA_UINT32					T2Old;
	LSA_UINT32					T2Act;
	LSA_UINT16					SeqOld;
	LSA_UINT16					SeqAct;
	LSA_UINT32					Count;
	LSA_INT32					Interval;
	LSA_UINT8					State;
} GSY_RCF_LOCAL;

typedef struct _GSY_ACT_DELAY
{
	GSY_DRIFT_PTR				pDrift;
	GSY_RCF_LOCAL				RcfLocal;
	LSA_UINT32					RspTxPortDelay;
	LSA_UINT32					RspRxPortDelay;
	LSA_UINT32					ReqDelay;
//	LSA_UINT32					RspDelay10;
//	LSA_UINT32					RspDelay1;
	LSA_INT32					RspDelayNS;
	LSA_UINT16					ReqSeqId;
	LSA_UINT16					Time;
	LSA_BOOL					RspDrift;
	LSA_UINT8					State;
} GSY_ACT_DELAY;

/* Port properties structure
*/
typedef struct _GSY_CH_PORT
{
	LSA_UINT32					RxDelay;
	LSA_UINT32					TxDelay;

	/* actual valid delay variables */
	LSA_UINT32					SyncLineDelay;
	LSA_UINT32					CableDelay;
	LSA_UINT32					MinDelay;
	LSA_UINT32					MaxDelay;
	LSA_UINT32					SumCount;

	LSA_UINT32					FwdMode;
	GSY_MAC_ADR_TYPE			MACAddr;
	LSA_BOOL					ClockSyncOk;		/* 170707lrg001: SyncTopoOk fuer SyncId 0 aus ext.LinkInd */
	LSA_BOOL					DelayReqActive;
	LSA_BOOL					DelayRspActive;		/* 291008lrg001: Delay response allowed if LSA_TRUE */
	LSA_UINT8					State;
	LSA_BOOL					Is_gPTP;            /* LSA_TRUE if gPTP, LSA_FALSE if PTCP */

	/* temporary delay variables. copied to actual ones on cycle end */
	LSA_BOOL					DelayValid;			/* LSA_TRUE if delays within temp-vars are valid */
	LSA_BOOL					ResponseArrived;	/* LSA_TRUE if a delay response arrived within this cycle */
	LSA_UINT32					SyncLineDelayTemp;
	LSA_UINT32					CableDelayTemp;
	LSA_UINT32					MinDelayTemp;
	LSA_UINT32					MaxDelayTemp;
#ifdef GSY_DELAY_FILTER	//160112lrg:
	LSA_UINT32					PortDelayTemp;		//160112lrg
	LSA_UINT32					FilterDelayWeight;	//220112lrg
	LSA_UINT64					FilterDelayTemp;	//160112lrg
#endif
	LSA_INT32					OldDelay[GSY_DELAY_MAX_COUNT];	//AP01262791: was UINT32
	LSA_UINT32					SumDelay;
	LSA_UINT32					SumCountLast;
	LSA_UINT32					LowerDelayTicks;
	LSA_UINT8					Oldest;
	LSA_UINT16					DelayEvent;
	LSA_UINT16					LastError;
	LSA_UINT16					ErrorCount;
	LSA_UINT16					DelayReqSeqId;
	GSY_ACT_DELAY				ActDelay;
	GSY_TIMER					DelayTimer;

#ifndef GSY_INCLUDE_FOR_KRISC32

	GSY_PORT_DATA_HEADER		PortDataAdjust;
	GSY_PORT_DATA_HEADER		PrmPortDataAdjust;
	GSY_PORT_DATA_SUBBLOCK		DomainBoundary;
	GSY_PORT_DATA_SUBBLOCK		PrmDomainBoundary;
	LSA_BOOL					PrmDomainWritten;
	LSA_BOOL					PrmDomainCfg;       /* LSA_TRUE: config Port. LSA_FALSE: already configured */
	LSA_UINT8					PrmDomainState;
	GSY_PORT_DATA_SUBBLOCK		PeerBoundary;
	GSY_PORT_DATA_SUBBLOCK		PrmPeerBoundary;
	LSA_UINT8					PrmPeerState;
	LSA_BOOL					PrmPeerWritten;
	LSA_BOOL					DelayBoundary;
	LSA_BOOL					DelaySetPending;
	LSA_UINT16					SendCnfPending;
	GSY_LOWER_RQB_PTR_TYPE		pExtLinkRbl;
#else
	GSY_LOWER_TX_PDU_PTR		pDelayReqPdu;
	K32_RQB_PTR_TYPE			pK32Rqb;
	K32_RQB_PTR_TYPE  			pReqRQB;			//181010lrg001
	K32_RQB_PTR_TYPE  			pCnfRQB;			//250711lrg
#endif //GSY_INCLUDE_FOR_KRISC32

} GSY_CH_PORT;

/* Values for Port.State
*/
#define GSY_PORT_STATE_INIT		0xf0
#define GSY_PORT_STATE_DOWN		0xf1
#define GSY_PORT_STATE_UP		0xf2
#define GSY_PORT_STATE_CLOSED	0xf3
#define GSY_PORT_STATE_DISABLED	0xf4

/* Values for Port.ActDelay.State (AND)

   TIME-State is not ANDed.
   If TIME-State is reached the state is set to GSY_DELAY_STATE_TIME
   and remains till a new send is started!

   TIME-State is the State between send-timeout (1s) and start of the
   next send (cycle-end).
*/
#define GSY_DELAY_STATE_INI		0
#define GSY_DELAY_STATE_ERR		0x01
#define GSY_DELAY_STATE_REQ		0x02
#define GSY_DELAY_STATE_CNF		0x04
#define GSY_DELAY_STATE_RSP		0x08
#define GSY_DELAY_STATE_FU		0x10
#define GSY_DELAY_STATE_END		0x1E
#define GSY_DELAY_STATE_TIME	0x20
#define GSY_DELAY_STATE_DOWN	0x40

/* Values for Port.PrmDomainState and PrmPeerState
*/
#define GSY_PRM_PORT_STATE_INI	0
#define GSY_PRM_PORT_STATE_CFG	1

#ifndef GSY_INCLUDE_FOR_KRISC32
/* Master structure
*/
typedef struct _GSY_MASTER_TYPE
{
	LSA_UINT8					State;
	LSA_UINT8					Priority1;
	LSA_UINT8					Priority2;
	LSA_BOOL					HwMaster;		/* 120809lrg001 */
	LSA_BOOL					Startup;		/* 070907lrg001 */
	LSA_UINT16					StartupTime;	/* 290807lrg001 */
	LSA_UINT16					EpochNumber;
	LSA_UINT16					CurrentUTCOffset;
	LSA_UINT16					AnnoSeqId;
	LSA_UINT16					SeqId;
	LSA_UINT16					SeqIdSnd[GSY_CH_MAX_PORTS];
	LSA_UINT16					SeqIdCnf[GSY_CH_MAX_PORTS];
	LSA_UINT16					SendInterval;
	LSA_UINT32					SyncIdBit;
	LSA_UINT32					Seconds;
	LSA_UINT32					Nanos;
	LSA_UINT32					Timestamp;
#ifdef GSY_MASTER_TIMER_CHECK
	LSA_UINT32					OldTimestamp;
	LSA_UINT16					OldSeqId;
#endif
	GSY_TIMER					StartupTimer;	/* 290807lrg001 */
	GSY_TIMER					SendTimer;
	GSY_LOWER_QUEUE				QueueSync;
	GSY_LOWER_QUEUE				QueueFu;
	GSY_LOWER_QUEUE				QueueAnnounce;
	GSY_BMA_PTR_TYPE			pPriMaster;
	GSY_BMA_PTR_TYPE			pSecMaster;
	GSY_BMA_TYPE				LocalMaster;
	GSY_BMA_TYPE				PriList[GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL];
	GSY_BMA_TYPE				SecList[GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL];
	LSA_UINT16					PriListUsed;
	LSA_UINT16					SecListUsed;
	LSA_UINT16					RbAnnoCount;
	LSA_UINT16					RbSyncCount;
	LSA_UINT16					RbFuCount;
	LSA_UINT32					SendCount;			// 200207lrg001: Masteraging untersetzen
	LSA_UINT32					TimeoutCount;		// 070907lrg001: Masteraging untersetzen
	LSA_UINT32					AnnounceCount;		// 070907lrg001: Senden von Announce untersetzen
} GSY_MASTER_TYPE;

#else // GSY_INCLUDE_FOR_KRISC32
typedef struct _K32_MASTER_TYPE
{
	LSA_UINT8					State;
	LSA_UINT16					SeqId;
	GSY_TIMER					SendTimer;
	GSY_TLV_MASTER				MasterTLV;
	GSY_SYNC_SEND_FRAME_PTR		pFrame;
	K32_RQB_PTR_TYPE  			pReqRQB;			//181010lrg001
} K32_MASTER_TYPE;
#endif // GSY_INCLUDE_FOR_KRISC32

/* Values for Master.State
*/
#define GSY_MASTER_STATE_OFF		0
#define GSY_MASTER_STATE_STOP		1
#define GSY_MASTER_STATE_STOPPING	2
#define GSY_MASTER_STATE_QUICKUP	3
#define GSY_MASTER_STATE_STARTUP	4
#define GSY_MASTER_STATE_SLAVE		5
#define GSY_MASTER_STATE_PRIMARY	6

/* Slave structure
*/
typedef struct _GSY_SLAVE_TYPE
{
	LSA_UINT8					State;
	LSA_BOOL					WaitFU;
	LSA_UINT16					SeqId;
	LSA_UINT16              	SyncError;
	LSA_UINT16					SyncInterval;
	LSA_UINT16					SyncPortId;
	LSA_UINT16					SyncInAct;
	LSA_UINT16					SyncOutAct;
	LSA_UINT16					SyncInTime;
	LSA_UINT16					SyncOutTime;
	LSA_UINT32					WindowSize;
	LSA_UINT32					SyncTakeover;
	LSA_UINT32					SyncTimeout;
	LSA_UINT32					SumTimeout;
	LSA_UINT32					FuTimeout;
	LSA_INT32					AveTimeOffset;
	LSA_INT32					SecondsOffset;		//lrgi210
	LSA_INT32					AveMinOffset;
	LSA_INT32					AveMaxOffset;
	LSA_INT32					AveSumOffset;
	LSA_INT32					AveSumCount;
	LSA_INT32					AveSumCountLast;
	LSA_INT32					AdjustInterval;
	LSA_UINT32					AdjustSeconds;
	LSA_UINT32					AdjustNanos;
	LSA_INT64					AdjustTermP;
	LSA_INT64					AdjustTermI;
	LSA_UINT16					CurrentUTCOffset;
	LSA_UINT16					TimeFlags;
	LSA_UINT16					ClockVariance;
	LSA_UINT8					ClockAccuracy;
	LSA_UINT8					ClockClass;
	GSY_MAC_ADR_TYPE			MasterAddr;
	GSY_SYNC_CONTROLLER_TYPE 	Ctrl;
#ifdef GSY_INCLUDE_FOR_KRISC32
	LSA_BOOL		 			TimeSetPending;
	LSA_BOOL		 			TimeWdPending;
	GSY_PTCP_UUID				Subdomain;
	GSY_SYNC_STATISTIC_TYPE		Statistics;
	struct _GSY_FWD_CTRL_TYPE
	{
	LSA_BOOL		 Pending;
	LSA_BOOL		 New;
	LSA_BOOL		 RestartSeqId;
	GSY_PTCP_UUID	 Subdomain;
	GSY_MAC_ADR_TYPE MasterAddr;
	}							FwdCtrl;
#else
	GSY_TIMER					SyncTimer;
#endif //GSY_INCLUDE_FOR_KRISC32
	GSY_DRIFT_PTR				pDrift;
	GSY_TIME_TYPE				Act;

} GSY_SLAVE_TYPE;

/* Values for Slave.State
*/
#define GSY_SLAVE_STATE_OFF			0
#define GSY_SLAVE_STATE_FIRST		1
#define GSY_SLAVE_STATE_WAIT		2
#define GSY_SLAVE_STATE_WAIT_SET	3
#define GSY_SLAVE_STATE_SET			4
#define GSY_SLAVE_STATE_SYNC_IN 	5
#define GSY_SLAVE_STATE_SYNC		6
#define GSY_SLAVE_STATE_SYNC_OUT	7
#define GSY_SLAVE_STATE_OUT			8
#define GSY_SLAVE_STATE_STOP		9
#define GSY_SLAVE_STATE_STOPPING	10

#ifndef GSY_INCLUDE_FOR_KRISC32
/* Event structure
*/
typedef struct _GSY_EVENT_TYPE
{
	GSY_PRM_IND_DATA_TYPE		Prm;
	GSY_PRM_IND_DATA_TYPE		OldPrm;
	LSA_UINT8					PrmFlags;
	LSA_UINT8					IndFlags;
	LSA_UINT16					PortId;
	LSA_UINT16              	SyncEvent;
	LSA_UINT16              	OldEvent;
	LSA_UINT16              	OldPort;
	LSA_BOOL					NewPrm;
	LSA_BOOL					NewOldPrm;
	LSA_BOOL					PriMasterWaits;

} GSY_EVENT_TYPE;

/* Values for PrmFlags
*/
#define GSY_PRM_IND_FLAGS_NONE			0
#define GSY_PRM_IND_FLAGS_NOTSYNC		1
#define GSY_PRM_IND_FLAGS_TIMEOUT		2

#endif //GSY_INCLUDE_FOR_KRISC32

/* SyncPLL  structures
*/
typedef struct _GSY_SYNC_PLL_TIME_TYPE
{
	LSA_UINT32					TimeStampPLL;	// TimeStamp of last ExtPLL-IN signal
	LSA_UINT32					TimeStampCB;	// TimeStamp of last cycle begin
	LSA_UINT32					Seconds;		// PTCP seconds of last cycle begin
	LSA_UINT32					Nanos;			// PTCP nanoseconds of last cycle begin

} GSY_SYNC_PLL_TIME_TYPE;

typedef struct _GSY_SYNC_PLL_TYPE
{
	LSA_UINT8					State;
	LSA_UINT32					CycleLength;
	LSA_UINT32					ReductionRatio;
	LSA_UINT32					PLLHWDelay;
	LSA_UINT32					PLLWindow;
	LSA_UINT32					AdjustSeconds;
	LSA_UINT32					AdjustNanos;
	LSA_INT32					AdjustOffset;
	LSA_INT32					AdjustInterval;
	LSA_INT64					AdjustTermP;
	LSA_INT64					AdjustTermI;
	GSY_SYNC_PLL_TIME_TYPE		Old;
	GSY_SYNC_PLL_TIME_TYPE		New;
	GSY_TIMER					PLLTimer;
	LSA_UINT16					PLLInterval;		//lrgi210
	LSA_UINT32					PLLWaitMillis;		//lrgi210
	LSA_UINT32					LastPLL;
	LSA_UINT16					SetWaitCount;
	LSA_UINT16					ActWaitCount;
} GSY_SYNC_PLL_TYPE;

#ifdef GSY_INCLUDE_FOR_KRISC32
#ifdef K32_FU_FWD_IN_FW
typedef struct _K32_FWD_TYPE
{
	LSA_BOOL					FuWait;
	LSA_BOOL					CnfWait;
	LSA_UINT16					SeqId;
	LSA_UINT32					TxStamp;
	LSA_UINT32					RxStamp[GSY_CH_MAX_PORTS];
	GSY_SYNC_SEND_FRAME_PTR		pSyncFrame[GSY_CH_MAX_PORTS];
	GSY_FU_SEND_FRAME_PTR		pFuFrame;
} K32_FWD_TYPE;
#endif

/* Krisc32 interface channel structure
*/
typedef struct _GSY_CH_K32
{
	LSA_UINT8										State;
	LSA_UINT8										InterfaceId;
	LSA_UINT16										PortCount;
    LSA_UINT32										TraceIdx;
	LSA_UINT32										RxTxNanos;
	GSY_CH_PORT										Port[GSY_CH_MAX_PORTS];
	GSY_MAC_ADR_TYPE								MACAddr;
	GSY_TIMER										DriftTimer;
	GSY_DRIFT_TYPE									Drift[2];
	GSY_SLAVE_TYPE									Slave[2];
	K32_MASTER_TYPE									Master[2];
#ifdef K32_FU_FWD_IN_FW
	K32_FWD_TYPE									Fwd[2];
#endif
	K32_RQB_PTR_TYPE								pK32SyncRqb;
} GSY_CH_K32;

#else //GSY_INCLUDE_FOR_KRISC32

/* inlcude fowarding modul types/defines
*/
#include "gsy_fwd.h"

/* system channel structure
*/
typedef struct _GSY_CH_SYS
{
	LSA_UINT8										State;
	LSA_HANDLE_TYPE 								Handle;
	LSA_HANDLE_TYPE 								HandleUpper;
	EDD_HANDLE_LOWER_TYPE							HandleLower;
	LSA_SYS_PATH_TYPE								SysPath;
	GSY_UPPER_QUEUE									QueueUpper;
	GSY_UPPER_QUEUE									QueueFwdCtrl;
	GSY_LOWER_QUEUE									QueueLowerSend;
	GSY_LOWER_QUEUE									QueueLowerRecv;
	GSY_LOWER_QUEUE									QueueLowerCtrl;
	LSA_UINT32										RxTxNanos;
	LSA_UINT32										HardwareType;
    LSA_UINT32										TraceIdx;
	LSA_UINT16										PortInit;
	LSA_UINT16										PortCount;
	LSA_UINT16										UserCount;
	LSA_UINT16										Resp;
	GSY_DETAIL_PTR_TYPE								pCDB;
	LSA_SYS_PTR_TYPE								pSys;
	GSY_CH_ANNO_PTR 								pChAnno;
	GSY_CH_USR_PTR 									pChUsr;
	GSY_UPPER_QUEUE									QueueDiag[GSY_MAX_PROTOCOLS];
	GSY_LOWER_RQB_PTR_TYPE							pOpenRbl;
	LSA_VOID LSA_FCT_PTR(GSY_UPPER_OUT_FCT_ATTR, 	pCbf)
						(GSY_UPPER_RQB_PTR_TYPE pRbu);
	GSY_CH_PORT										Port[GSY_CH_MAX_PORTS];
	GSY_FWD_INIT_TYPE								PortFwd[GSY_CH_MAX_PORTS];
	GSY_FWD_INIT_TYPE								PortInp[GSY_CH_MAX_PORTS];
	GSY_MAC_ADR_TYPE								MACAddr;
	GSY_FWD_CHANNEL_TYPE                            Fwd;
	LSA_BOOL										PrmFwdInd;
	LSA_UINT32										PortDelayReqActive;		/* 200707lrg001: fuer EDD-Rate */
	LSA_UINT32										PortDelayRspActive;		/* 200707lrg001: fuer EDD-Rate */
	GSY_UPPER_RQB_PTR_TYPE							pCommitRbu;
	GSY_TIMER										DriftTimer;
	GSY_DRIFT_TYPE									Drift[GSY_MAX_PROTOCOLS][GSY_CFG_FWD_MAX_MASTERS_PER_PROTOCOL];
} GSY_CH_SYS;

/* announce channel structure
*/
typedef struct _GSY_CH_ANNO
{
	LSA_UINT8										State;
	LSA_HANDLE_TYPE 								Handle;
	LSA_HANDLE_TYPE 								HandleUpper;
	EDD_HANDLE_LOWER_TYPE							HandleLower;
    LSA_UINT32										TraceIdx;
	LSA_SYS_PATH_TYPE								SysPath;
	GSY_UPPER_QUEUE									QueueUpper;
	GSY_LOWER_QUEUE									QueueLower;
	LSA_UINT16										Resp;
	GSY_DETAIL_PTR_TYPE								pCDB;
	LSA_SYS_PTR_TYPE								pSys;
	GSY_CH_SYS_PTR 									pChSys;
	GSY_LOWER_RQB_PTR_TYPE							pOpenRbl;
	LSA_VOID LSA_FCT_PTR(GSY_UPPER_OUT_FCT_ATTR, 	pCbf)
						(GSY_UPPER_RQB_PTR_TYPE pRbu);
} GSY_CH_ANNO;

/* user channel structure
*/
typedef struct _GSY_CH_USR
{
	LSA_UINT8										State;
	LSA_HANDLE_TYPE 								Handle;
	LSA_HANDLE_TYPE 								HandleUpper;
	EDD_HANDLE_LOWER_TYPE							HandleLower;
    LSA_UINT32										TraceIdx;
	LSA_SYS_PATH_TYPE								SysPath;
	LSA_BOOL										InfoSync;
	GSY_UPPER_QUEUE									QueueUpper;
	GSY_UPPER_QUEUE									QueueEvent;				/* 120207lrg001 */
	GSY_UPPER_QUEUE									QueuePrm;
	GSY_UPPER_QUEUE									QueueRcv;
	GSY_UPPER_QUEUE									QueueWait;
	GSY_DETAIL_PTR_TYPE								pCDB;
	LSA_SYS_PTR_TYPE								pSys;
	GSY_CH_SYS_PTR 									pChSys;
	LSA_UINT8										SyncId;
	LSA_UINT8										SyncRole;
	LSA_BOOL										RTC2Sync;
	LSA_BOOL										RTC2Active;
	LSA_BOOL										RTC2Ctrl;
	GSY_LOWER_QUEUE									qRTC2Rbl;
	LSA_UINT32										RTC2RblCount;
	GSY_LOWER_RQB_PTR_TYPE							pSyncSetRbl;
	GSY_LOWER_RQB_PTR_TYPE							pCancelRbl;				// 1126470
	LSA_BOOL										PrmRTC2;
	LSA_UINT8										PrmState;
	LSA_BOOL  										PrmSubdomainIDChanged;  /* 110806js001 */
	LSA_BOOL  										PrmSubdomainInd;
	LSA_BOOL  										PrmSyncIdChanged;
	GSY_WORD_TYPE									SendClockFactor;		/* 131210lrg001 */
	GSY_WORD_TYPE									PrmSendClock;			/* 051206lrg001 */
	GSY_SYNC_DATA_RECORD							PrmData;
	GSY_SYNC_DATA_RECORD							SyncData;
	GSY_SYNC_PLL_RECORD								PrmPLL;			// 230910lrg001: PDSyncPLL record B
	GSY_SYNC_PLL_RECORD								SyncPLL;		// 230910lrg001: PDSyncPLL record A
	LSA_BOOL										PrmPLLWritten;
	LSA_BOOL										SyncPLLChanged;
	LSA_BOOL										SyncPLLActive;
	LSA_UINT32										PrmRecordIndexWritten;
	LSA_BOOL										PrmClockAndTimeWritten;
	GSY_SYNC_PLL_TYPE								PLLData;
	GSY_MASTER_TYPE									Master;
	GSY_SLAVE_TYPE									Slave;
	GSY_EVENT_TYPE									Event;
	GSY_SYNC_STATISTIC_TYPE							Statistics;
	GSY_DIAG_DATA_RECORD							DiagRecord;
	LSA_UINT32										DiagCount;
	LSA_UINT32										DiagIdxWr;
	LSA_UINT16										DiagNo;
	LSA_UINT16										MasterControlState;
	LSA_VOID LSA_FCT_PTR(GSY_UPPER_OUT_FCT_ATTR, 	pCbf)
						(GSY_UPPER_RQB_PTR_TYPE pRbu);
} GSY_CH_USR;

#endif //GSY_INCLUDE_FOR_KRISC32

/* Values for channel.State	(to be seen well at memory dump)
*/
#define GSY_CHA_STATE_ERROR		0xfe
#define GSY_CHA_STATE_FREE		0xff
#define GSY_CHA_STATE_OPENING	0xf0
#define GSY_CHA_STATE_OPEN_LOW	0xf1
#define GSY_CHA_STATE_OPEN_FWD	0xf2
#define GSY_CHA_STATE_OPEN		0xf3
#define GSY_CHA_STATE_CLOSE_FWD	0xf4
#define GSY_CHA_STATE_CLOSING	0xf5
#define GSY_CHA_STATE_CLOSED	0xf6

/* Values for channel.PrmState
*/
#define GSY_PRM_STATE_NONE		0xa0
#define GSY_PRM_STATE_PREPARE	0xa1
#define GSY_PRM_STATE_WRITE		0xa2
#define GSY_PRM_STATE_END		0xa3
#define GSY_PRM_STATE_COMMIT	0xa4
#define GSY_PRM_STATE_START		0xa5
#define GSY_PRM_STATE_STOP		0xa6
#define GSY_PRM_STATE_ERROR		0xa7

/* Values for channel.MasterControlState
*/
#define GSY_MASTER_CONTROL_STATE_NONE       0
#define GSY_MASTER_CONTROL_STATE_STARTABLE  1
#define GSY_MASTER_CONTROL_STATE_STARTED    2


#ifndef GSY_INCLUDE_FOR_KRISC32

/*-----------------------------------------------------------------------------
* local data structure
*/
typedef struct gsy_data_tag
{
	GSY_CH_SYS 			*	pChSysList;
	GSY_CH_ANNO			*	pChAnnoList;
	GSY_CH_USR 			*	pChUsrList;
	GSY_CH_SYS				ChSysList[GSY_MAX_SYS_CHANNELS];
	GSY_CH_ANNO				ChAnnoList[GSY_MAX_SYS_CHANNELS];
	GSY_CH_USR				ChUsrList[GSY_MAX_USR_CHANNELS];
	GSY_FWD_GLOBAL_TYPE     Fwd;
    LSA_FATAL_ERROR_TYPE	Error;

} GSY_DATA_TYPE;


/*===========================================================================*/
/*                                  data                                     */
/*===========================================================================*/

#if (GSY_MODULE_ID == 1)
GSY_DATA_TYPE  GSY_LOCAL_MEM_ATTR  gsy_data;
#else
LSA_EXTERN GSY_DATA_TYPE  GSY_LOCAL_MEM_ATTR  gsy_data;
#endif

#endif //GSY_INCLUDE_FOR_KRISC32

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/
#ifndef GSY_BIG_ENDIAN

/* Swapping macros for high and low bytes of an unsigned 16 Bit variable
*/
#define GSY_SWAP16(u16_)	                                       \
          (LSA_UINT16)  (  (((LSA_UINT16)(u16_) >> 8) & 0x00FF)    \
                         | (((LSA_UINT16)(u16_) << 8) & 0xFF00) )  \

#define GSY_SWAPW(src16ptr_, dst16ptr_) \
                {*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+1); \
                 *((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+0);}

/* Swapping macros for high and low words and the bytes of these words 
 * of an unsigned 32 Bit variable: 0x44332211 -> 0x11223344
*/
#define GSY_SWAP32(u32_)	                                \
                     (  ((( (u32_)) & 0xFF000000L) >> 24)   \
                      + ((( (u32_)) & 0x00FF0000L) >>  8)   \
                      + ((( (u32_)) & 0x0000FF00L) <<  8)   \
                      + ((( (u32_)) & 0x000000FFL) << 24))  \

#define GSY_SWAPD(src32ptr_, dst32ptr_) \
                {*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+3); \
                 *((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+2); \
                 *((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+1); \
                 *((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+0);}

#else	/* GSY_BIG_ENDIAN defined: */

/* No swapping nessesary
*/
#define GSY_SWAP16(u16_)	(u16_)
#define GSY_SWAP32(u32_)	(u32_)

#define GSY_SWAPW(src16ptr_, dst16ptr_) \
                {*((LSA_UINT8 *)dst16ptr_+0) = *((LSA_UINT8 *)src16ptr_+0); \
                 *((LSA_UINT8 *)dst16ptr_+1) = *((LSA_UINT8 *)src16ptr_+1);}

#define GSY_SWAPD(src32ptr_, dst32ptr_) \
                {*((LSA_UINT8 *)dst32ptr_+0) = *((LSA_UINT8 *)src32ptr_+0); \
                 *((LSA_UINT8 *)dst32ptr_+1) = *((LSA_UINT8 *)src32ptr_+1); \
                 *((LSA_UINT8 *)dst32ptr_+2) = *((LSA_UINT8 *)src32ptr_+2); \
                 *((LSA_UINT8 *)dst32ptr_+3) = *((LSA_UINT8 *)src32ptr_+3);}

#endif	/* GSY_BIG_ENDIAN defined */


#define GSY_BYTE_COPY(_DstPtr, _SrcPtr, _Len)			\
  		{int _i; for (_i = 0; _i < _Len; _i++)	\
			*(_DstPtr+_i) = *(_SrcPtr+_i);}

#define GSY_MACADDR_CMP(_s1, _s2)	\
		((_s1.MacAdr[0] == _s2.MacAdr[0]) && (_s1.MacAdr[1] == _s2.MacAdr[1]) &&	\
		 (_s1.MacAdr[2] == _s2.MacAdr[2]) && (_s1.MacAdr[3] == _s2.MacAdr[3]) &&	\
		 (_s1.MacAdr[4] == _s2.MacAdr[4]) && (_s1.MacAdr[5] == _s2.MacAdr[5]))

#define GSY_MACADDR_IS_NULL(_s1)	\
		((_s1.MacAdr[0] == 0) && (_s1.MacAdr[1] == 0) &&	\
		 (_s1.MacAdr[2] == 0) && (_s1.MacAdr[3] == 0) &&	\
		 (_s1.MacAdr[4] == 0) && (_s1.MacAdr[5] == 0))

#define GSY_SUBDOMAIN_CMP(_s1, _s2)	\
		((_s1.Data1.Dword == _s2.Data1.Dword) && (_s1.Data2.Word == _s2.Data2.Word) && (_s1.Data3.Word == _s2.Data3.Word) &&	\
		 (_s1.Data4[0] == _s2.Data4[0]) && (_s1.Data4[1] == _s2.Data4[1]) &&	\
		 (_s1.Data4[2] == _s2.Data4[2]) && (_s1.Data4[3] == _s2.Data4[3]) &&	\
		 (_s1.Data4[4] == _s2.Data4[4]) && (_s1.Data4[5] == _s2.Data4[5]) &&	\
		 (_s1.Data4[6] == _s2.Data4[6]) && (_s1.Data4[7] == _s2.Data4[7]))

#define GSY_SUBDOMAIN_IS_NULL(_s1)	\
		((_s1.Data1.Dword == 0) && (_s1.Data2.Word == 0) && (_s1.Data3.Word ==0) &&	\
		 (_s1.Data4[0] == 0) && (_s1.Data4[1] == 0) &&	\
		 (_s1.Data4[2] == 0) && (_s1.Data4[3] == 0) &&	\
		 (_s1.Data4[4] == 0) && (_s1.Data4[5] == 0) &&	\
		 (_s1.Data4[6] == 0) && (_s1.Data4[7] == 0))

#define GSY_SUBDOMAIN_SET_NULL(_s1)	\
		{_s1.Data1.Dword = 0; _s1.Data2.Word = 0; _s1.Data3.Word = 0; \
		 _s1.Data4[0] = 0; _s1.Data4[1] = 0; _s1.Data4[2] = 0; _s1.Data4[3] = 0; \
		 _s1.Data4[4] = 0; _s1.Data4[5] = 0; _s1.Data4[6] = 0; _s1.Data4[7] = 0;}

#define GSY_SUBDOMAIN_ARRAY_CMP(_s1, _s2)	\
		((_s1.Octet[0]  == _s2.Octet[0])  && (_s1.Octet[1]  == _s2.Octet[1])  &&	\
		 (_s1.Octet[2]  == _s2.Octet[2])  && (_s1.Octet[3]  == _s2.Octet[3])  &&	\
		 (_s1.Octet[4]  == _s2.Octet[4])  && (_s1.Octet[5]  == _s2.Octet[5])  &&	\
		 (_s1.Octet[6]  == _s2.Octet[6])  && (_s1.Octet[7]  == _s2.Octet[7])  &&	\
		 (_s1.Octet[8]  == _s2.Octet[8])  && (_s1.Octet[9]  == _s2.Octet[9])  &&	\
		 (_s1.Octet[10] == _s2.Octet[10]) && (_s1.Octet[11] == _s2.Octet[11]) &&	\
		 (_s1.Octet[12] == _s2.Octet[12]) && (_s1.Octet[13] == _s2.Octet[13]) &&	\
		 (_s1.Octet[14] == _s2.Octet[14]) && (_s1.Octet[15] == _s2.Octet[15]))

#define GSY_MACADDR_SUM(_s1)	\
		((LSA_UINT16)_s1.MacAdr[0] + (LSA_UINT16)_s1.MacAdr[1] + (LSA_UINT16)_s1.MacAdr[2] +\
		 (LSA_UINT16)_s1.MacAdr[3] + (LSA_UINT16)_s1.MacAdr[4] + (LSA_UINT16)_s1.MacAdr[5])

#define GSY_OFFSET_GET(_p1, _p2)	\
		((LSA_UINT32)((LSA_UINT8 *)_p2  - (LSA_UINT8 *)_p1))

//Specific macro for Lint Msg 613
#if defined(_lint) || defined(lint)
#define GSY_IS_VALID_PTR(ptr)  if(LSA_HOST_PTR_ARE_EQUAL(LSA_NULL,(ptr)))\
                               {\
                                   gsy_ErrorInternal(GSY_MODULE_ID, __LINE__, GSY_ERR_FATAL, (ptr), 0);\
                               }
#else
#define GSY_IS_VALID_PTR(ptr) {}
#endif


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

#ifndef GSY_INCLUDE_FOR_KRISC32

/* gsy_usr.c
*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_ChOpen (GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CloseChSys (GSY_CH_SYS_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CloseChAnno (GSY_CH_ANNO_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CloseChUsr (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CloseChUsrDone (GSY_CH_USR_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CallbackSys (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CallbackAnno (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CallbackUsr (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CallbackRbu(LSA_SYS_PTR_TYPE,GSY_UPPER_RQB_PTR_TYPE,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelUpper (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_UserDelayInd (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_UserDelayCtrl (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_UserDelayGet (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_UserFwdCbf (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_UserFwdCtrl (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_DiagCancel (GSY_CH_USR_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DiagUserInd (LSA_INT,GSY_CH_SYS_PTR,GSY_DRIFT_PTR,LSA_UINT32,LSA_UINT32,LSA_INT32,LSA_INT32,LSA_INT32,LSA_INT32,LSA_UINT32,LSA_UINT32,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_DiagUserGet (GSY_CH_SYS_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncStop (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncParams (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SyncInd (GSY_CH_USR_PTR,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SyncUserInd (GSY_CH_USR_PTR,LSA_BOOL,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_Rtc2Cnf (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmPrepare (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWrite (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWriteClockCheck (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmChangePort (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmEnd (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmCommit (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_PrmCommitNext (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmRead (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmEventSet (GSY_CH_USR_PTR,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmUserInd (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_PrmDiagWrite (GSY_CH_USR_PTR,GSY_DIAG_DATA_TYPE*,LSA_UINT32,LSA_UINT32,LSA_UINT16,LSA_UINT8,LSA_UINT8,LSA_INT);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_PrmWritePortData (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_PrmCommitEnd (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_BOOL   GSY_LOCAL_FCT_ATTR gsy_PrmValidName (GSY_CH_USR_PTR,LSA_UINT16,GSY_UPPER_BYTE_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MasterControl (GSY_CH_USR_PTR,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_PrmCommitEndDiag(GSY_CH_SYS_PTR,GSY_DRIFT_PTR,LSA_UINT8);

/* gsy_msm.c
*/
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveTimer (GSY_TIMER_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveTimerStart (GSY_CH_USR_PTR, LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveAdjust (GSY_CH_USR_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SlaveStart (GSY_CH_USR_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SlaveSet (GSY_CH_USR_PTR,LSA_UINT16,LSA_UINT32,LSA_UINT32,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveAction (GSY_CH_USR_PTR);
LSA_BOOL   GSY_LOCAL_FCT_ATTR gsy_SlaveSeqIdCheck (GSY_CH_USR_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveIndSync (GSY_CH_USR_PTR,GSY_PTCP_DATA*,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveIndFu (GSY_CH_USR_PTR,GSY_PTCP_DATA*,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SlaveStop (GSY_CH_USR_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgorithm(
LSA_UINT32                    DeltaSeconds,
LSA_UINT32                    DeltaNanoSeconds,
LSA_INT32                     PhaseError_ns,
LSA_INT32                     *pAdjustInterval,
LSA_INT64                     *pProportionalTerm,
LSA_INT64                     *pIntegralTerm,
LSA_UINT8                     SyncId,
GSY_CH_USR_PTR                pChUsr);

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncReset(
LSA_INT32                     RateInterval,
LSA_UINT8                     SyncId,
GSY_CH_USR_PTR                pChUsr);

/* gsy_bma.c
*/
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterSyncFill (GSY_CH_USR_PTR,GSY_LOWER_TX_PDU_PTR);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterFuFill (GSY_CH_USR_PTR,GSY_LOWER_TX_PDU_PTR);
LSA_UINT16 			GSY_LOCAL_FCT_ATTR gsy_MasterStart (GSY_CH_USR_PTR);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterStop (GSY_CH_USR_PTR);
LSA_VOID 			GSY_LOCAL_FCT_ATTR gsy_MasterStartupEnd (GSY_TIMER_PTR);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterSend (GSY_TIMER_PTR);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterSyncCnf (GSY_CH_USR_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterFuCnf (GSY_CH_USR_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterAnnoCnf (GSY_CH_USR_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID			GSY_LOCAL_FCT_ATTR gsy_MasterInd (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterIndAnno (GSY_CH_USR_PTR,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterIndSync (GSY_CH_USR_PTR,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterAging (GSY_CH_USR_PTR);
LSA_VOID   			GSY_LOCAL_FCT_ATTR gsy_MasterAgingAnno (GSY_CH_USR_PTR);
LSA_VOID            GSY_LOCAL_FCT_ATTR gsy_MasterStateSet (GSY_CH_USR_PTR);
GSY_BMA_PTR_TYPE    GSY_LOCAL_FCT_ATTR gsy_MasterListUpdate (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE,GSY_BMA_PTR_TYPE,LSA_UINT16*);
GSY_BMA_PTR_TYPE	GSY_LOCAL_FCT_ATTR gsy_MasterBestCheck (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE,GSY_BMA_PTR_TYPE);
GSY_BMA_PTR_TYPE	GSY_LOCAL_FCT_ATTR gsy_MasterBestGet (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE);
GSY_BMA_PTR_TYPE    GSY_LOCAL_FCT_ATTR gsy_MasterListPut (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE,GSY_BMA_PTR_TYPE,LSA_UINT16*);
GSY_BMA_PTR_TYPE    GSY_LOCAL_FCT_ATTR gsy_MasterListFind (GSY_CH_USR_PTR,GSY_BMA_PTR_TYPE,GSY_BMA_PTR_TYPE);
LSA_VOID			GSY_LOCAL_FCT_ATTR gsy_MasterSlaveControl(GSY_CH_USR_PTR,LSA_UINT8,LSA_UINT8);
LSA_VOID			GSY_LOCAL_FCT_ATTR gsy_MasterPLLGet(GSY_CH_USR_PTR,LSA_UINT8);
LSA_VOID			GSY_LOCAL_FCT_ATTR gsy_MasterPLLStart(GSY_CH_USR_PTR);

/* gsy_sys.c
*/
LSA_UINT16      GSY_LOCAL_FCT_ATTR gsy_TimerAlloc (GSY_TIMER_PTR,LSA_UINT16,LSA_UINT16);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_TimerStart (GSY_TIMER_PTR,LSA_UINT16);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_TimerStop (GSY_TIMER_PTR);
LSA_UINT16      GSY_LOCAL_FCT_ATTR gsy_TimerClean (GSY_TIMER_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_TimeoutRequest (GSY_UPPER_RQB_PTR_TYPE);
GSY_CH_ANNO_PTR GSY_LOCAL_FCT_ATTR gsy_ChAnnoAlloc (GSY_CH_SYS_PTR, GSY_UPPER_RQB_PTR_TYPE,LSA_SYS_PTR_TYPE,GSY_DETAIL_PTR_TYPE);
GSY_CH_USR_PTR  GSY_LOCAL_FCT_ATTR gsy_ChUsrAlloc (GSY_CH_SYS_PTR, GSY_UPPER_RQB_PTR_TYPE,LSA_SYS_PTR_TYPE,GSY_DETAIL_PTR_TYPE);
GSY_CH_SYS_PTR  GSY_LOCAL_FCT_ATTR gsy_ChSysAlloc (GSY_UPPER_RQB_PTR_TYPE,LSA_SYS_PTR_TYPE,GSY_DETAIL_PTR_TYPE);
GSY_CH_SYS_PTR  GSY_LOCAL_FCT_ATTR gsy_ChSysFind (LSA_UINT16);
LSA_UINT16      GSY_LOCAL_FCT_ATTR gsy_ChSysClean (GSY_CH_SYS_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ChAnnoClean (GSY_CH_ANNO_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ChUsrClean(GSY_CH_USR_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ChSysFree (GSY_CH_SYS_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ChUsrFree (GSY_CH_USR_PTR);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ChAnnoFree (GSY_CH_ANNO_PTR);
GSY_CH_SYS_PTR  GSY_LOCAL_FCT_ATTR gsy_ChSysGet (LSA_HANDLE_TYPE);
GSY_CH_ANNO_PTR GSY_LOCAL_FCT_ATTR gsy_ChAnnoGet (LSA_HANDLE_TYPE);
GSY_CH_USR_PTR  GSY_LOCAL_FCT_ATTR gsy_ChUsrGet (LSA_HANDLE_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_PutUpperQ (GSY_UPPER_QUEUE*,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_GetUpperQ (GSY_UPPER_QUEUE*,GSY_UPPER_RQB_PTR_TYPE);
LSA_BOOL        GSY_LOCAL_FCT_ATTR gsy_FindUpper (GSY_UPPER_QUEUE*,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_DequeUpper (GSY_UPPER_QUEUE*,GSY_UPPER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_EnqueLower (GSY_LOWER_QUEUE*,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_DequeLower (GSY_LOWER_QUEUE*,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_TraceIn (LSA_INT,LSA_INT,LSA_VOID*,LSA_VOID*,LSA_UINT32);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_TraceOut (LSA_INT,LSA_INT,LSA_VOID*,LSA_VOID*,LSA_UINT32);
LSA_VOID 	    GSY_LOCAL_FCT_ATTR gsy_TraceSR (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ErrorUser (LSA_INT,LSA_INT,LSA_INT,LSA_VOID_PTR_TYPE,LSA_UINT16);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ErrorLower (LSA_INT,LSA_INT,LSA_INT,LSA_VOID_PTR_TYPE,LSA_UINT16);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ErrorInternal (LSA_INT,LSA_INT,LSA_INT,LSA_VOID_PTR_TYPE,LSA_UINT32);
LSA_VOID        GSY_LOCAL_FCT_ATTR gsy_ErrorFatal (LSA_INT,LSA_INT);

/* gsy_low.c
*/
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DiagIndK32 (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_PortStatus (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_Indication (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT32,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_IndicationAnno (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SendConfirm (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SendSyncConfirm (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CancelConfirm (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerStart (GSY_CH_SYS_PTR,LSA_BOOL);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerMem (GSY_CH_SYS_PTR,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerCh (GSY_CH_SYS_PTR,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerSet (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerGet (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerDelay (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerPort (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_OpenLowerFwd (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerAnnoEnd (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerEnd (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_OpenLowerError (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelAnno (GSY_CH_USR_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelAnnoAll(GSY_CH_ANNO_PTR);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_CancelLower (GSY_CH_SYS_PTR,LSA_BOOL,LSA_UINT8,LSA_HANDLE_TYPE,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CloseLower (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CloseLowerFin (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_CloseAnnoFin (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_MucAddrSet (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT8 const *,LSA_BOOL,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DataSend (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16,LSA_INT,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DataRecv (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_AnnoSend (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT16,LSA_INT);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_AnnoRecv (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SyncSend (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_INT,LSA_UINT16,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_SyncRecv (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_UINT32);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthParam (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE*,LSA_BOOL);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthSend (GSY_CH_SYS_PTR,GSY_LOWER_QUEUE*,LSA_VOID*,LSA_BOOL);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthRecv (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,GSY_LOWER_QUEUE*,LSA_HANDLE_TYPE,LSA_UINT8,LSA_BOOL);
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_AllocEthKrisc32 (GSY_CH_SYS_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_FreeLowerRb (GSY_CH_SYS_PTR,GSY_LOWER_RQB_PTR_TYPE,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_LowerDelaySet (GSY_CH_SYS_PTR,LSA_UINT16);

/* gsy_del.c
*/
LSA_BOOL   GSY_LOCAL_FCT_ATTR gsy_DelayGet (GSY_CH_SYS_PTR,GSY_TIME_TYPE*,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16);
LSA_BOOL   GSY_LOCAL_FCT_ATTR gsy_TLVGetInfo (GSY_CH_SYS_PTR,GSY_LOWER_RXBYTE_PTR,LSA_UINT16,LSA_UINT32,LSA_UINT16,GSY_TLV_INFO_PTR_TYPE); //lrgi210: TimeStamp
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayCalc (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqInd (GSY_CH_SYS_PTR,LSA_UINT16,const GSY_LOWER_RX_PDU_PTR,LSA_UINT16,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayRspInd (GSY_CH_SYS_PTR,LSA_UINT16,GSY_LOWER_RX_PDU_PTR,LSA_UINT16,LSA_UINT32,LSA_BOOL);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayFuInd (GSY_CH_SYS_PTR,LSA_UINT16,GSY_LOWER_RX_PDU_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayRspCnf (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT16,const GSY_LOWER_TX_PDU_PTR,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqCnf (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT16,const GSY_LOWER_TX_PDU_PTR,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqSend (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqStart (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayRspStart (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqNext (GSY_TIMER_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayReqStop (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT8);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayRestart (GSY_CH_SYS_PTR,/*GSY_DRIFT_PTR,*/LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayRestartAll (GSY_CH_SYS_PTR,GSY_DRIFT_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayUserCtrl (GSY_CH_SYS_PTR,LSA_UINT16,LSA_UINT16);
LSA_INT32  GSY_LOCAL_FCT_ATTR gsy_DriftMedian (GSY_CH_SYS_PTR,GSY_DRIFT_PTR);
LSA_INT32  GSY_LOCAL_FCT_ATTR gsy_DriftCorrection (GSY_CH_SYS_PTR,LSA_UINT32,LSA_UINT32);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DriftAdjust (GSY_TIMER_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DriftCalc (GSY_CH_SYS_PTR,GSY_DRIFT_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DriftDecray (GSY_CH_SYS_PTR,GSY_DRIFT_PTR);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DriftReset (GSY_CH_SYS_PTR,GSY_DRIFT_PTR,LSA_UINT8);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelayEvent (GSY_CH_SYS_PTR,LSA_UINT16);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DriftSet (GSY_CH_SYS_PTR,GSY_DRIFT_PTR,LSA_UINT8,LSA_INT32,LSA_UINT8);
LSA_VOID   GSY_LOCAL_FCT_ATTR gsy_DelaySet (GSY_CH_SYS_PTR,LSA_UINT16);

/* gsy_fwd.c see gsy_fwd.h
*/

#else //GSY_INCLUDE_FOR_KRISC32

/* k32_gsy_del.c: Prototypes here because of function parameters
*/
#if K32_MODULE_ID == K32_MODULE_ID_GSY_DEL
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_RcfCalc (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT16);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayCalc (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT16);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayRspInd (GSY_CH_K32_PTR,LSA_UINT16,GSY_LOWER_RX_PDU_PTR,LSA_UINT16,LSA_UINT32,LSA_BOOL);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayFuInd (GSY_CH_K32_PTR,LSA_UINT16,GSY_LOWER_RX_PDU_PTR,LSA_UINT16);
//K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayRspCnf (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT16,GSY_LOWER_TX_PDU_PTR,LSA_UINT32);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqCnf (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT16,GSY_LOWER_TX_PDU_PTR,LSA_UINT32);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqSend (GSY_CH_K32_PTR,LSA_UINT16);
K32_STATIC	LSA_UINT16	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqStart (GSY_CH_K32_PTR,LSA_UINT16);
K32_STATIC	LSA_UINT16	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqStop (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT8,K32_RQB_PTR_TYPE);
K32_STATIC	LSA_INT32	K32_LOCAL_FCT_ATTR	k32gsy_DriftMedian (GSY_CH_K32_PTR,GSY_DRIFT_PTR);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DriftTimer (GSY_TIMER_PTR);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DriftCalc (GSY_CH_K32_PTR,GSY_DRIFT_PTR);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelaySet (GSY_CH_K32_PTR,LSA_UINT16);
K32_STATIC	LSA_BOOL	K32_LOCAL_FCT_ATTR	k32gsy_DelayRspSend (GSY_CH_K32_PTR,LSA_UINT16,GSY_LOWER_RX_RCV_PTR,LSA_UINT16,LSA_UINT32,LSA_BOOL);
#endif
			LSA_BOOL	K32_LOCAL_FCT_ATTR	k32gsy_DelayGet (GSY_CH_K32_PTR,GSY_TIME_TYPE*,GSY_TLV_INFO_PTR_TYPE,LSA_UINT16);
			LSA_BOOL	K32_LOCAL_FCT_ATTR	k32gsy_TLVGetInfo (GSY_CH_K32_PTR,GSY_LOWER_RXBYTE_PTR,LSA_UINT16,LSA_UINT16,GSY_TLV_INFO_PTR_TYPE);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqInd (GSY_CH_K32_PTR,LSA_UINT16,GSY_LOWER_RX_PDU_PTR,LSA_UINT16,LSA_UINT32);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayRspStart (GSY_CH_K32_PTR,LSA_UINT16);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayReqNext (GSY_TIMER_PTR);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayRestart (GSY_CH_K32_PTR,/*GSY_DRIFT_PTR,*/LSA_UINT16);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayRestartAll (GSY_CH_K32_PTR,GSY_DRIFT_PTR);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayUserCtrl (GSY_CH_K32_PTR,LSA_UINT16,LSA_UINT16);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DriftReset (GSY_CH_K32_PTR,GSY_DRIFT_PTR,LSA_UINT8);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DriftSet (GSY_CH_K32_PTR,GSY_DRIFT_PTR,LSA_UINT8,LSA_INT32,LSA_UINT8);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_TimerStart (GSY_TIMER_PTR,LSA_UINT16);
			LSA_UINT16	K32_LOCAL_FCT_ATTR	k32gsy_TimerStop (GSY_TIMER_PTR);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_DelayTimer (GSY_TIMER_PTR);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_MasterSend (GSY_TIMER_PTR);
		GSY_CH_K32_PTR	K32_LOCAL_FCT_ATTR	k32gsy_SysPtrGet (LSA_UINT8);

/* k32_gsy_msm.c
*/
#if K32_MODULE_ID == K32_MODULE_ID_GSY_MSM
K32_STATIC	LSA_BOOL	K32_LOCAL_FCT_ATTR	k32gsy_SlaveIsRunning (LSA_UINT8,LSA_UINT8);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_WatchdogCbf(GSY_CH_K32_PTR,LSA_UINT8);
K32_STATIC	LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_SyncMsg(GSY_CH_K32_PTR,LSA_UINT8,LSA_BOOL);
#endif
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_FwdSet(GSY_CH_K32_PTR,LSA_UINT8,LSA_BOOL,GSY_PTCP_UUID,GSY_MAC_ADR_TYPE);
			LSA_VOID	K32_LOCAL_FCT_ATTR	k32gsy_MasterSyncCnf(GSY_CH_K32_PTR,GSY_LOWER_TX_PDU_PTR,LSA_UINT16,LSA_UINT8);
#endif

/* gsy_clp.c
*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgoTime(
LSA_BOOL                      StateSync,
LSA_INT32                     Offset_ns,
LSA_INT32                     DriftInterval_ns,
LSA_UINT16                    SyncInterval_ms,
LSA_INT32                     *pAdjustInterval,	//Oufput
GSY_SYNC_CONTROLLER_TYPE      *pCtrl);

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgoCycle(
LSA_UINT32                    DeltaSeconds,
LSA_UINT32                    DeltaNanoSeconds,
LSA_INT32                     PhaseError_ns,
LSA_INT32                     *pAdjustInterval,
LSA_INT64                     *pProportionalTerm,
LSA_INT64                     *pIntegralTerm,
LSA_UINT8                     SyncId,
GSY_SYNC_CONTROLLER_TYPE      *pCtrl);

LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_ClpReset(
LSA_INT32                     RelativeDrift_Ppt,
LSA_UINT8                     SyncId,
GSY_SYNC_CONTROLLER_TYPE    * pCtrl);

/*****************************************************************************/
/*  end of file GSY_INT.H                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_INT_H */

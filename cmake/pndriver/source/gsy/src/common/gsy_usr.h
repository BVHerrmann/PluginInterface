
#ifndef GSY_USR_H                     /* ----- reinclude-protection ----- */
#define GSY_USR_H

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
/*  F i l e               &F: gsy_usr.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for GSY.                */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version                  Who  What                           */
/*  2004-11-29  P01.00.00.00_00.01.00.00 lrg  from LSA_GLOB P2.1.0.0_0.4.1.1 */
/*  2005-04-07  P01.00.00.00_00.01.00.01 lrg  new: GSY_OPC_DELAY_CTRL        */
/*  2005-06-02  P03.06.00.00_00.03.00.01 lrg  Prototype for PT2              */
/*  2005-06-20  P03.06.00.00_00.03.00.02 lrg  Neue SYNC-Schnittstelle vom    */
/*              vom ERTEC-EDD 3.6 uebernommen                                */
/*  2005-06-24  P03.06.00.00_00.03.02.02 lrg  neues Sync-Userinterface       */
/*  2005-10-27  P03.07.00.00_00.09.01.01 lrg  DriftSyncId entfernt aus       */
/*                                            GSY_RQB_DELAY_CTRL_TYPE        */
/*  2005-11-21  P03.08.00.00_00.01.02.00 lrg  neue Dienste:                  */
/*              GSY_OPC_DIAG_INDICATION_PROVIDE und                          */
/*              GSY_OPC_ERROR_INDICATION_PROVIDE                             */
/*  2005-12-09  P03.08.00.00_00.01.02.02 lrg  -GSY_OPC_SYNC_RECORD_READ_REAL */
/*              Upper RQB macros moved here from gsy_cfg.h                   */
/*  2005-12-22  P03.09.00.00_00.01.01.02 lrg  Upper RQB header macros moved  */
/*              back to gsy_cfg.h                                            */
/*  2006-02-13  P03.09.00.00_00.03.01.02 lrg  neuer Dienst                   */
/*              GSY_OPC_ERROR_INDICATION_PROVIDE ersetzt durch               */
/*              GSY_OPC_PRM_INDICATION_PROVIDE                               */
/*  2006-02-16  P03.09.00.00_00.03.01.03 lrg  Clock-Parameter aus Struktur   */
/*              GSY_RQB_SYNC_CONTROL_TYPE und Zugriffsmakros entfernt        */
/*  2006-03-08  P03.10.00.00_00.01.01.02 lrg  Generalisierte CM-PD-Schnitt-  */
/*              stelle mit Opcodes GSY_OPC_PRM_...                           */
/*  2006-03-20  P03.10.00.00_00.01.01.03 lrg  GSY_OPC_CHANNEL_INFO_GET       */
/*  2006-04-25  P03.11.00.00_00.01.01.04 lrg  An den EDD weiterzuleitende    */
/*              Dienste entfernt.                                            */
/*  2006-06-02  P03.11.00.00_00.04.03.02 lrg  neu: GSY_OLD_IF; "neue" Sync-  */
/*              Schnitts. entfernt                                           */
/*  2006-06-21  P03.12.00.00_00.02.01.02 lrg  GSY_SYNCID_XXX von gsy_int.h   */
/*  2006-06-22  P03.12.00.00_00.02.01.03 LSA_GLOB_P02.01.00.00_00.07.01.01:  */
/*              PTRC_SUBLABEL_LIST_*, LSA_RSP_OK_OFFSET, LSA_RSP_ERR_OFFSET  */
/*  2006-07-07  P03.12.00.00_00.25.01.02 AsChannelDiag entfernt aus der PRM- */
/*              Indication-Datenstruktur GSY_PRM_IND_DATA_TYPE               */
/*  2006-11-14  P03.12.00.00_00.40.01.02 Trigger-defines ergänzt.            */
/*  2006-11-30  P03.12.00.00_00.40.03.02 301106lrg002: ArtsRQ AP00391256     */
/*              RecordIndex ist LSA_UINT32 statt LSA_UINT16                  */
/*  2006-12-15  P03.12.00.00_00.40.05.02 151206lrg001:                       */
/*              GSY_OPC_DELAY_CTRL u. GSY_OPC_FWD_CTRL entfernt              */
/*  2007-06-01  P04.00.00.00_00.10.02.02 lrg  LSA_GLOB P2.1.0.0_0.8.1.1      */
/*  2007-07-16  P04.00.00.00_00.30.01.00 160707lrg001: UUIDs aus Diagnose-   */
/*              indication entfernt.                                         */
/*  2007-09-27  P04.00.00.00_00.30.06.02 270907lrg004: MaintenaceDemanded    */
/*              #define GSY_OLD_IF entfernt                                  */ 
/*  2007-10-16  P04.00.00.00_00.40.01.02 AP00526517: RateValid,PrimaryMaster */
/*  2008-04-17  P04.00.00.00_00.80.01.02 170408lrg002: RcvSyncPrio,LocalPrio */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2010-07-16  P05.00.00.00_00.01.06.02 lrg: AP00999032 _REQUEST_UPPER_DONE */
/*              AP00697597: German comments translated to english            */
/*  2010-10-08  P05.00.00.00_00.01.08.03 230910lrg001: PLLState              */
/*  2010-12-13  P05.00.00.00_00.02.01.03 131210lrg001: PDSyncPLL-Record solo */
/*  2011-05-31  P05.00.00.00_00.05.21.02 310511lrg: AP01178714: SetWaitCount */
/*  2011-08-04  T05.00.00.00_00.05.44.02 040811lrg: AP01205450:              */
/*              AP01205450 Struktur err aus RQB entfernen                    */
/*  2012-11-07  P05.02.00.00_00.04.08.01 AP01069493 GSY_REQUEST_UPPER_DONE   */
/*              and all other functions with GSY_SYSTEM_OUT_FCT_ATTR moved   */
/*              from gsy_usr.h to gsy_sys.h                                  */
/*  2014-09-12  P06.00.00.00_00.01.44.01 1092865 PDEV-Prm, Erweiterung von   */
/*              PrmWrite und PrmEnd um Fehler-Informationen                  */
/*  2015-04-15  V06.00, Inc04, Feature 1204141, Task 1234427                 */
/*              GSY_OPC_MASTER_CONTROL added                                 */
/*  2015-05-11  V06.00, Inc05, Feature 1227823, Task 1311090                 */
/*              only one user channel for clock or time sync                 */
/*              GSY_OPC_CHANNEL_INFO_GET removed                             */
/*  2015-07-17  V06.00, Inc07, Feature 1204141, Task 1366974                 */
/*              Timesync Diagnosen richtig an CM geben                       */
/*  2016-06-28  V07.00, Task 1737674: Remove _PRM_INDICATION::ChannelNumber  */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* Port-ID for all Ports of Channeldevice
*/
#define GSY_PORT_ALL		0xffff

/* Delay measurement control codes
*/
#define GSY_DELAY_START		1
#define GSY_DELAY_STOP		0
#define GSY_DELAY_END		2

/*=============================================================================
 *      GSY request block (== upper request block)
 *===========================================================================*/

/*=====  opcodes within request block  =====*/

PTRC_SUBLABEL_LIST_OPEN("GSY_OPC")
#define GSY_OPC_OPEN_CHANNEL			1
#define GSY_OPC_CLOSE_CHANNEL			2
#define GSY_OPC_TIMER					3
#define GSY_OPC_GET_DELAY				4
//#define GSY_OPC_CHANNEL_INFO_GET		5	//Task 1311090
#define GSY_OPC_RECV_DATA				6
#define GSY_OPC_CANCEL_IND				7
#define GSY_OPC_MASTER_CONTROL			8	/* Requests the start or stop of the PTCP time master */
#define GSY_OPC_SYNC_GET_PARAMS			14

/* PRM Schnittstelle
*/
#define GSY_OPC_PRM_PREPARE				15	/* Load default records to set B: no args */
#define GSY_OPC_PRM_WRITE				16	/* Load user record to set B */
#define GSY_OPC_PRM_END					17	/* Check record set B: no args */
#define GSY_OPC_PRM_COMMIT				18	/* Copy record set B to A: no args */
#define GSY_OPC_PRM_READ				19	/* Read record from set A */
#define GSY_OPC_PRM_CHANGE_PORT         20  /* inform GSY about a change in MediaType */

#define GSY_OPC_DIAG_INDICATION_PROVIDE	21
#define GSY_OPC_PRM_INDICATION_PROVIDE	22

#define GSY_OPC_ALL						0
#define GSY_OPC_UNUSED					0xff
PTRC_SUBLABEL_LIST_CLOSE("GSY_OPC")

/*=====  response codes within request block  =====*/

/* If possible, a component should define its PREFIX_RSP_* constants
 * on the LSA constants in lsa_usr.h. If there is no LSA constant, the 
 * component specific response codes are defined with the help of 
 * LSA_RSP_OK_OFFSET and LSA_RSP_ERR_OFFSET.
 * Examples:
*/
PTRC_SUBLABEL_LIST_OPEN("GSY_RSP")
#define GSY_RSP_OK					(LSA_UINT16) 	LSA_RSP_OK
#define GSY_RSP_OK_ACTIVE			(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  1)
#define GSY_RSP_OK_CANCEL			(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  2)
#define GSY_RSP_OK_MORE				(LSA_UINT16)	(LSA_RSP_OK_OFFSET  +  3)

#define GSY_RSP_ERR_RESOURCE		(LSA_UINT16)	LSA_RSP_ERR_RESOURCE
#define GSY_RSP_ERR_PARAM			(LSA_UINT16)	LSA_RSP_ERR_PARAM
#define GSY_RSP_ERR_SEQUENCE		(LSA_UINT16)	LSA_RSP_ERR_SEQUENCE
#define GSY_RSP_ERR_SYS_PATH		(LSA_UINT16)	LSA_RSP_ERR_SYS_PATH
#define GSY_RSP_ERR_SYNTAX			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  1)
#define GSY_RSP_ERR_REF				(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  2)
#define GSY_RSP_ERR_PROTOCOL		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  3)
#define GSY_RSP_ERR_TIMEOUT			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  4)
#define GSY_RSP_ERR_LOWER_LAYER		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  5)
#define GSY_RSP_ERR_CONFIG			(LSA_UINT16)	(LSA_RSP_ERR_OFFSET +  6)
#define GSY_RSP_ERR_PRM_INDEX		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 10)
#define GSY_RSP_ERR_PRM_DATA		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 11)
#define GSY_RSP_ERR_PRM_BLOCK		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 12)
#define GSY_RSP_ERR_PRM_CONSISTENCY	(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 13)
#define GSY_RSP_ERR_PRM_PORTID		(LSA_UINT16)	(LSA_RSP_ERR_OFFSET + 14)
PTRC_SUBLABEL_LIST_CLOSE("GSY_RSP")

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/
#include "pnio_pck2_on.h"
/* MAC address (6 byte packed) 041112lrg001
*/
#define GSY_MAC_ADR_LENGTH          6
typedef PNIO_PACKED_ATTRIBUTE_PRE struct _GSY_MAC_ADR_TYPE
{               
    LSA_UINT8       MacAdr[GSY_MAC_ADR_LENGTH];
} PNIO_PACKED_ATTRIBUTE_POST GSY_MAC_ADR_TYPE;

#include "pnio_pck_off.h"

/*=============================================================================
 *      GSY request block (== upper request block)
 *===========================================================================*/

/*=====  forward declaration  =====*/
struct gsy_rqb_tag;

#define GSY_UPPER_RQB_PTR_TYPE                  /* pointer to upper RQB */ \
    struct gsy_rqb_tag     GSY_UPPER_RQB_ATTR *

#define GSY_UPPER_BYTE_PTR_TYPE					/* pointer to upper memory */ \
    LSA_UINT8              GSY_UPPER_MEM_ATTR *

#define GSY_UPPER_WORD_PTR_TYPE					/* pointer to upper memory */ \
    LSA_UINT16              GSY_UPPER_MEM_ATTR *

#ifndef GSY_UPPER_MEM_PTR_TYPE
#define GSY_UPPER_MEM_PTR_TYPE					/* pointer to upper memory */ \
    LSA_VOID               GSY_UPPER_MEM_ATTR *
#endif
#ifndef GSY_UPPER_DIAG_PTR_TYPE
#define GSY_UPPER_DIAG_PTR_TYPE					/* pointer to diagnose list at upper memory */ \
    GSY_DIAG_DATA_TYPE     GSY_UPPER_MEM_ATTR *
#endif

#ifndef GSY_UPPER_PRM_IND_PTR_TYPE
#define GSY_UPPER_PRM_IND_PTR_TYPE				/* pointer to PRM Indiction diagnose list */ \
    GSY_PRM_IND_DATA_TYPE  GSY_UPPER_MEM_ATTR *
#endif

typedef struct _GSY_RQB_SYNC_GET_PARAMS_TYPE  GSY_UPPER_MEM_ATTR * \
		GSY_RQB_SYNC_GET_PARAMS_PTR_TYPE;

/*=====  arguments of GSY request block  =====*/

/* Structure for opcode GSY_OPC_TIMER
*/
typedef struct _GSY_RQB_TIMER_TYPE
{
    LSA_TIMER_ID_TYPE		TimerId;
    LSA_USER_ID_TYPE		UserId;
} GSY_RQB_TIMER_TYPE;


/* Structure for opcode GSY_OPC_CHANNEL_INFO_GET //Task 1311090
typedef struct _GSY_RQB_CHANNEL_INFO_TYPE
{
    LSA_UINT8				SyncId;
    LSA_UINT8				Reserved8;
    LSA_UINT16				Reserved16;
    LSA_UINT32				Reserved32;
} GSY_CHANNEL_INFO_TYPE;
*/

/* Values for SyncId
*/
PTRC_SUBLABEL_LIST_OPEN("GSY_SYNCID")
#define GSY_SYNCID_CLOCK		0x00
#define GSY_SYNCID_TIME			0x01
#define GSY_SYNCID_NONE			0x1f
PTRC_SUBLABEL_LIST_CLOSE("GSY_SYNCID")


/* Structure for opcode GSY_OPC_GET_DELAY
*/
typedef struct _GSY_RQB_GET_DELAY_TYPE
{								 	/* Read delay values ********************/
    LSA_UINT16				Port;	/* I/O: Port ID				 			*/
    LSA_UINT32				Time;	/* O: Accumulated cable RX delay value	*/
    								/*    in nanoseconds at this eth. port	*/
    LSA_UINT32				Count;	/* O: Count of accumulated delay values	*/
    LSA_UINT32				Min;	/* O: Minimum delay value				*/
    LSA_UINT32				Max;	/* O: Maximum delay value				*/
} GSY_RQB_GET_DELAY_TYPE;

/* Structure for opcode GSY_OPC_RECV_DATA
*/
typedef struct _GSY_RQB_RECV_DATA_TYPE
{										/* Receive PTCP PDU data part *******/
    GSY_UPPER_MEM_PTR_TYPE	pBuffer;	/* I:   Pointer to PDU buffer		*/
    LSA_UINT16				Length;		/* I/O: Length of buffer/copied data*/
    LSA_UINT32				TimeStamp;	/* O:   Receive timestamp			*/
    LSA_UINT32				Resolution;	/* 0:   Timestamp resolution in ns	*/
    LSA_UINT32				LineDelay;	/* 0:   Delay in ns to add to PDU	*/
										/*      delay fields				*/
} GSY_RQB_RECV_DATA_TYPE;

/* Structure for opcode GSY_OPC_CANCEL_IND
*/
typedef struct _GSY_RQB_CANCEL_IND_TYPE
{										/* Cancel Recv/SyncInd RQBs *********/
    LSA_OPCODE_TYPE			Opcode;		/* I: Opcode or GSY_OPC_ALL			*/
    LSA_UINT16				Count;		/* I/O: Count of RQBs to cancel or	*/
										/*      0 for ALL					*/
} GSY_RQB_CANCEL_IND_TYPE;

/* Structure for opcode GSY_OPC_MASTER_CONTROL
*/
typedef struct _GSY_RQB_MASTER_CONTROL_TYPE
{
    LSA_UINT32				Mode;			// I: GSY_MASTER_STOP or GSY_MASTER_START
    LSA_UINT16				UTCOffset;		// I: Current offset in seconds of PTCP time to UTC time (leap seconds)
} GSY_RQB_MASTER_CONTROL_TYPE;

/* defines for Mode
*/
#define GSY_MASTER_STOP		0
#define GSY_MASTER_START	1

/* Structure for opcode GSY_OPC_DIAG_INDICATION_PROVIDE
*/

#define GSY_DIAG_SYNCID_NOT_SYNCHRONIZED    0
#define GSY_DIAG_SYNCID_SYNCHRONIZED        1
#define GSY_DIAG_SUBDOMAIN_NO_RECORD        0
#define GSY_DIAG_SUBDOMAIN_CONFIGURED       1
#define GSY_DIAG_RATE_INVALID               0
#define GSY_DIAG_RATE_VALID                 1

typedef struct _GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE
{
	LSA_UINT16				Trigger;		// I: GSY_TRIGGER_NOW: an Inidication will be thrown at once 
											//    GSY_TRIGGER_ALL: Indication on change of (1)...(8)
											//    GSY_TRIGGER_SYNC: Indication on change of (1)...(7)
											//    GSY_TRIGGER_CHECK: Indication if one current (1)...(7) is different from I(1...7)
	LSA_UINT16				IndCount;		// O: Number of DiagInd RQBs in the user channel queue
	LSA_UINT8				SyncId;			// O: Sync ID of the events (0 = clock, 1 = time...)
    LSA_UINT8				StateSync;	  	// [I/]0: (1) GSY_DIAG_SYNCID_SYNCHRONIZED if SyncId is synchronized, else GSY_DIAG_SYNCID_NOT_SYNCHRONIZED
	GSY_MAC_ADR_TYPE		MasterMacAddr;	// [I/]0: (2) Interface MAC of current SyncMaster
    LSA_UINT8				Subdomain;		// [I/]0: (3) GSY_DIAG_SUBDOMAIN_CONFIGURED if Sync-Subdomain configured (GSY_DIAG_SUBDOMAIN_NO_RECORD: no PDSyncData record loaded)
//	LSA_BOOL				PrimaryMaster;	// [I/]0: (4) LSA_TRUE if current SyncMaster is primary (LSA_FALSE: secondary)
    LSA_UINT8				RateValid;		// [I/]0: (4) GSY_DIAG_RATE_VALID if the Rate (RCF-Interval) to the current SyncMaster is known
	LSA_UINT8				RcvSyncPrio;	// [I/]0: (5) MasterPriority1 from SyncFrame (0 at active master)
	LSA_UINT8				LocalPrio;		// [I/]0: (6) MasterPriority1 from SyncRecord (0 at slave) with ActiveBit set at active Master after Startup
	LSA_UINT8				PLLState;		// [I/]0: (7) State of PLL sync at ClockMaster with PDSyncPLL record else always GSY_PLL_STATE_SYNC_OFF
	LSA_INT32				DriftInterval;	// 0: (8) RCF-Interval if Trigger == GSY_TRIGGER_ALL/_NOW, else 0
	LSA_INT32				UserValue1;		// for future extensions
	LSA_INT32				UserValue2;		// for future extensions
											// 170408lrg002: PrimaryMaster -> RcvSyncPrio, LocalPrio
} GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE;

typedef struct _GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_DIAG_INDICATION_PTR_TYPE;

/* Trigger 
*/
#define GSY_TRIGGER_NOW		    0
#define GSY_TRIGGER_ALL		    1
#define GSY_TRIGGER_SYNC		2
#define GSY_TRIGGER_CHECK		3

/* PLLState
*/
#define GSY_PLL_STATE_SYNC_OFF		0
#define GSY_PLL_STATE_SYNC_START	1
#define GSY_PLL_STATE_SYNC_WAIT		2
#define GSY_PLL_STATE_SYNC_WAIT_SET	3
#define GSY_PLL_STATE_SYNC_SET		4
#define GSY_PLL_STATE_SYNC_IN 		5
#define GSY_PLL_STATE_SYNC_OK		6
#define GSY_PLL_STATE_SYNC_OUT		7
#define GSY_PLL_STATE_SYNC_SLEEP	8
#define GSY_PLL_STATE_SYNC_STOP		9
#define GSY_PLL_STATE_SYNC_WAIT_OK	10		//310511lrg: new PLL state

/* Structure for opcode: GSY_OPC_PRM_INDICATION_PROVIDE
*/
typedef struct _GSY_PRM_IND_DATA_TYPE
{               
	LSA_UINT16				ChannelProperties;
	LSA_UINT16				ChannelErrorType;
	LSA_UINT16				ExtChannelErrorType;
	LSA_UINT32				ExtChannelAddValue;
	LSA_UINT32				Seconds;
	LSA_UINT32				Nanoseconds;
} GSY_PRM_IND_DATA_TYPE;

typedef struct _GSY_RQB_PRM_INDICATION_PROVIDE_TYPE
{
	LSA_UINT16					EddPortId;		// 0: Port ID where the event occured or 0 for interface
	LSA_UINT16					DiagCnt;		// O: Act. number of events in the DiagList
	GSY_UPPER_PRM_IND_PTR_TYPE	pDiag;			// I: Pointer to DiagList (length GSY_PRM_IND_MAX_ENTRIES)
} GSY_RQB_PRM_INDICATION_PROVIDE_TYPE;

typedef struct _GSY_RQB_PRM_INDICATION_PROVIDE_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_INDICATION_PTR_TYPE;

#define GSY_PRM_IND_MAX_ENTRIES					2

/* ChannelProperties:
 * 270907lrg004: Maintenace.Demanded statt Required 
 * 3Bit Dir=In/Out(0), 2Bit Spec=Appears(1), 1Bit Maint.Dem=1, 1Bit Maint.Req=0, 1Bit Accumulative=0, 8Bit Type=0
*/
#define GSY_PRM_CHANNEL_PROPERTIES				0x0400	
#define GSY_PRM_EVENT_DISAPPEARS				0x1000	
#define GSY_PRM_EVENT_APPEARS					0x0800	

/* defines for (Ext)ChannelErrorType
*/
#define GSY_PRM_CHANNEL_ERROR_TYPE_SYNC			0x8003	// Sync mismatch
#define GSY_PRM_CHANNEL_ERROR_TYPE_TIME			0x8009	// Time mismatch

PTRC_SUBLABEL_LIST_OPEN("GSY_PRM_EVENT")
#define GSY_PRM_EVENT_NONE						0
#define GSY_PRM_EVENT_NO_MESSAGE_RECEIVED		0x8000	// No sync PDU (timeout)
#define GSY_PRM_EVENT_JITTER_OUT_OF_BOUNDARY	0x8003	// Clock out of PLL window
PTRC_SUBLABEL_LIST_CLOSE("GSY_PRM_EVENT")

/* defines for ExtChannelAddValue
*/
#define GSY_PRM_CHANNEL_ADD_NONE				0x0		// No info

/* defines for SyncEvent
*/
PTRC_SUBLABEL_LIST_OPEN("GSY_SYNC_EVENT")
#define GSY_SYNC_EVENT_NONE                        0
#define GSY_SYNC_EVENT_NO_MESSAGE_RECEIVED         0x0001
#define GSY_SYNC_EVENT_WRONG_PTCP_SUBDOMAIN_ID     0x0002
#define GSY_SYNC_EVENT_WRONG_IR_DATA_ID            0x0003
#define GSY_SYNC_EVENT_JITTER_OUT_OF_BOUNDARY      0x0004
#define GSY_SYNC_EVENT_SYNC_OK                     0x0005
#define GSY_SYNC_EVENT_DELAY_ERROR                 0x0006
#define GSY_SYNC_EVENT_DELAY_OK                    0x0007
PTRC_SUBLABEL_LIST_CLOSE("GSY_SYNC_EVENT")

/* Structure for opcode: GSY_OPC_PRM_READ
*/
typedef struct _GSY_RQB_PRM_RECORD_TYPE
{
	LSA_UINT16				Slot;			// I:
	LSA_UINT16				Subslot;		// I:
	LSA_UINT16				EddPortId;		// I: Port ID or 0 for interface
	LSA_UINT32				RecordIndex;	// I: Index of record 301106lrg002: 16 -> 32
	LSA_UINT32				RecordDataLen;	// I/O: Length of record data
	GSY_UPPER_BYTE_PTR_TYPE	pRecordData;	// I: Pointer to record data buffer
} GSY_RQB_PRM_RECORD_TYPE;
#define GSY_RQB_PRM_READ_TYPE   GSY_RQB_PRM_RECORD_TYPE

typedef struct _GSY_RQB_PRM_RECORD_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_READ_PTR_TYPE;

/* Structure for opcode: GSY_OPC_PRM_WRITE
*/
typedef struct _GSY_RQB_PRM_WRITE_TYPE		// 1092865
{
	LSA_UINT16				Slot;			// I:
	LSA_UINT16				Subslot;		// I:
	LSA_UINT16				EddPortId;		// I: Port ID or 0 for interface
	LSA_UINT32				RecordIndex;	// I: Index of record 301106lrg002: 16 -> 32
	LSA_UINT32				RecordDataLen;	// I/O: Length of record data
	GSY_UPPER_BYTE_PTR_TYPE	pRecordData;	// I: Pointer to record data buffer
	LSA_UINT32				ErrOffset;		// O: Offset of error from pRecordData
} GSY_RQB_PRM_WRITE_TYPE;

typedef struct _GSY_RQB_PRM_WRITE_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_WRITE_PTR_TYPE;

/* Structure for opcode: GSY_OPC_PRM_END
*/
typedef struct _GSY_RQB_PRM_END_TYPE		// 1092865
{
	LSA_UINT16				ErrPortId;		// O: EddPortId of the port related to the error
	LSA_UINT32				ErrIndex;		// O: Index of the record that caused the error
	LSA_UINT32				ErrOffset;		// O: Offset of error in RecordData of ErrIndex
    LSA_UINT8				ErrFault;		// O: Offset of error from pRecordData
    LSA_UINT8               PortparamsNotApplicable[EDD_CFG_MAX_PORT_CNT];  // O: For Change Media Type in Run
    LSA_UINT8               IsModularPort          [EDD_CFG_MAX_PORT_CNT];
} GSY_RQB_PRM_END_TYPE;

typedef struct _GSY_RQB_PRM_END_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_END_PTR_TYPE;

/* default values for structure GSY_RQB_PRM_END_TYPE */
#define GSY_PRM_ERR_FAULT_NO_OR_TOO_LESS_PARAMS     0
#define GSY_PRM_ERR_FAULT_WRONG_OR_TOO_MANY_PARAMS  1

#define GSY_PRM_PORT_PARAMS_APPLICABLE              0
#define GSY_PRM_PORT_PARAMS_NOT_APPLICABLE          1

#define GSY_PRM_PORT_IS_COMPACT                     0
#define GSY_PRM_PORT_IS_MODULAR                     1

#define GSY_PRM_ERR_OFFSET_DEFAULT		0			                                // for ErrOffset
#define GSY_PRM_ERR_PORT_ID_DEFAULT		0			                                // for ErrPortId
#define GSY_PRM_ERR_INDEX_DEFAULT		0xFFFF		                                // for ErrIndex
#define GSY_PRM_ERR_FAULT_DEFAULT		GSY_PRM_ERR_FAULT_WRONG_OR_TOO_MANY_PARAMS	// for ErrFault


/* Structure for opcode: GSY_OPC_PRM_COMMIT
*/
typedef struct _GSY_RQB_PRM_COMMIT_TYPE
{
  LSA_UINT8   ApplyDefaultPortparams[EDD_CFG_MAX_PORT_CNT];  // I: For Change Media Type in Run
  
} GSY_RQB_PRM_COMMIT_TYPE;

typedef struct _GSY_RQB_PRM_COMMIT_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_COMMIT_PTR_TYPE;


/* Structure for opcode: GSY_OPC_PRM_CHANGE_PORT
*/
typedef struct _GSY_RQB_PRM_CHANGE_PORT_TYPE
{
    LSA_UINT16     PortID;
    LSA_UINT8      ModuleState;
    LSA_UINT8      PortparamsNotApplicable;
    LSA_UINT8      ApplyDefaultPortparams;
  
} GSY_RQB_PRM_CHANGE_PORT_TYPE;

typedef struct _GSY_RQB_PRM_CHANGE_PORT_TYPE   GSY_UPPER_MEM_ATTR *    GSY_UPPER_PRM_CHANGE_PORT_PTR_TYPE;

#define GSY_PRM_PORTMODULE_PLUG_PREPARE     1
#define GSY_PRM_PORTMODULE_PLUG_COMMIT      2
#define GSY_PRM_PORTMODULE_PULLED           3

#define GSY_PRM_APPLY_RECORD_PORT_PARAMS    0
#define GSY_PRM_APPLY_DEFAULT_PORT_PARAMS   1


/* Structure for opcode: GSY_OPC_SYNC_GET_PARAMS
*/
typedef struct _GSY_CYCLE_COUNTER_TYPE
{               
	LSA_UINT32				Seconds;
	LSA_UINT32				Nanoseconds;
} GSY_CYCLE_COUNTER_TYPE;

typedef struct _GSY_SYNC_STATISTIC_TYPE
{
  GSY_CYCLE_COUNTER_TYPE    StatisticAge;                // Runtime-Age of statistic in CycleCounter-Units
  LSA_UINT32                FrameOk;                     // Count of good frames received since start of 
                                                         // Statistic.
  LSA_UINT32                FrameDataInvalid;            // Count of ignored Frames, due to FrameDataValid-
                                                         // Bit	== 0
  LSA_UINT32                FrameWrongSequenceNr;        // Count of ignored Frames, due to wrong 
                                                         // sequence number
  LSA_UINT32                FrameWrongSubdomain;         // Count of ignored Frames, due to wrong 
                                                         // PTCP_SubdomainID
  LSA_UINT32                FrameWrongIrDataId;          // Count of ignored Frames, due to wrong 
                                                         // IR data id
  LSA_UINT32                FrameCylceNumberMismatch;    // Count of ignored Frames, due to mismatch 
                                                         // between CycleNumber and PTCP_Time
  LSA_UINT32                FrameNotReceived;            // Count of not received Frames
  LSA_UINT32                PLLWindowOverrun;            // Counts all Overruns among PLLWindow
  LSA_UINT32                PLLWindowUnderun;            // Counts all Underuns among PLLWindow

} GSY_SYNC_STATISTIC_TYPE;

#define GSY_SYNC_ROLE_IS_NOT_SYNCMASTER     0
#define GSY_SYNC_ROLE_IS_SYNCMASTER         1
#define GSY_SYNC_SYNCACTIVITY_NO_SERVICE    0
#define GSY_SYNC_SYNCACTIVITY_ACTIVE        1
    typedef struct _GSY_RQB_SYNC_GET_PARAMS_TYPE
{
  GSY_SYNC_STATISTIC_TYPE     Statistics;         // Several counters for Errors/Events etc.   

  LSA_UINT8                   RoleIsSyncMaster;   // GSY_SYNC_ROLE_IS_SYNCMASTER if Role is SyncMaster.
  LSA_UINT32                  PLLWindow;          // Width of the PLL-Window in ns
                                                  // is the maximum offset from master time allowed
  LSA_UINT32                  PTCPTimeoutFactor;  // Max. time (in 31,25us Entities) to wait for
                                                  // valid sync frames before leaving state SYNC_OK 
  LSA_UINT16                  SyncInTime;         // Time in ms not to leave the PLL-Window til state SYNC_OK 
  LSA_UINT32                  SamplePeriod;       // Not used: 0
  LSA_UINT32                  ControlPeriod;      // Length of control cycle (in 31,25us Entities)
  LSA_UINT8                   SyncActivity;       // Activity of Synchronisation
  LSA_UINT8                   IrtActivity;                       // Not used: 0.
  LSA_UINT8                   IrtPortActivity[GSY_CH_MAX_PORTS]; // Not used: 0

} GSY_RQB_SYNC_GET_PARAMS_TYPE;

/*****************************************************************************
 * for GSY_OPC_OPEN_CHANNEL, see gsy_open_channel()
*/
typedef  LSA_VOID  LSA_FCT_PTR(GSY_UPPER_OUT_FCT_ATTR, 
								GSY_UPPER_CALLBACK_FCT_PTR_TYPE)(
		GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

typedef struct gsy_rqb_args_channel_tag
{
    LSA_HANDLE_TYPE                 handle;
                                    /* req: -                                */
                                    /* cnf: channel-handle of GSY            */
                                    /* At all later RQBs user will write it  */
                                    /* into handle of GSY_RQB_HEADER.        */
    LSA_HANDLE_TYPE                 handle_upper;
                                    /* req: channel-handle of user           */
                                    /* cnf: -                                */
                                    /* At all confirmations GSY writes it    */
                                    /* into handle of GSY_RQB_HEADER.        */
    LSA_SYS_PATH_TYPE               sys_path;
                                    /* req: system-path of channel           */
                                    /* cnf: -                                */
    GSY_UPPER_CALLBACK_FCT_PTR_TYPE gsy_request_upper_done_ptr;
                                    /* req: pointer to callback-function     */
                                    /* cnf: -                                */
} GSY_RQB_ARGS_CHANNEL_TYPE;

typedef struct gsy_rqb_args_channel_tag   GSY_UPPER_MEM_ATTR *    GSY_UPPER_OPEN_CHANNEL_PTR_TYPE;

/* for GSY_OPC_CLOSE_CHANNEL, see gsy_close_channel().
 * GSY_RQB_ARGS_TYPE isn't needed.
*/

/*=====  union of arguments of GSY request block  =====
*/
typedef union gsy_rqb_args_tag
{
    GSY_RQB_TIMER_TYPE						Timer;          /* Internal timer RQB           */
//  GSY_CHANNEL_INFO_TYPE					ChannelInfo;    /* Get SyncId...                */
    GSY_RQB_GET_DELAY_TYPE					Delay;          /* Get RX cable delay at port   */
    GSY_RQB_RECV_DATA_TYPE					RecvData;       /* Receive PTCP PDU data part   */
    GSY_RQB_CANCEL_IND_TYPE					CancelInd;      /* Cancel Recv/Indication RQBs  */
    GSY_RQB_MASTER_CONTROL_TYPE				MasterControl;  /* Start/Stop PTCP time master  */
    GSY_RQB_DIAG_INDICATION_PROVIDE_TYPE	DiagInd;        /* Diagnosis indication         */
    GSY_RQB_PRM_INDICATION_PROVIDE_TYPE		PrmInd;         /* Error event indication       */
    GSY_RQB_PRM_RECORD_TYPE					PrmRecord;      /* Read record                  */
    GSY_RQB_PRM_WRITE_TYPE					PrmWrite;       /* Write record (1092865)       */
    GSY_RQB_PRM_CHANGE_PORT_TYPE            PrmChangePort;  /* change port                  */
    GSY_RQB_PRM_END_TYPE					PrmEnd;         /* PRM end      (1092865)       */
    GSY_RQB_PRM_COMMIT_TYPE                 PrmCommit;      /* PRM Commit                   */
    GSY_RQB_SYNC_GET_PARAMS_PTR_TYPE		pSyncParams;    /* Get sychronisation info      */
    GSY_RQB_ARGS_CHANNEL_TYPE				channel;        /* Open Channel: see            */
                                                            /* GSY_RQB_ARGS_CHANNEL_TYPE    */
} GSY_RQB_ARGS_TYPE;


/*=====  GSY request block definition  =====*/
typedef struct gsy_rqb_tag
{
    GSY_RQB_HEADER                           /* PREFIX RQB header, see file  */
                                             /* gsy_cfg.h                    */
/* 040811lrg
	LSA_RQB_LOCAL_ERROR_TYPE        err;     // req: -
                                             // cnf: error from lower-layer
                                             // see LSA_RQB_LOCAL_ERROR_TYPE
*/
    GSY_RQB_ARGS_TYPE               args;    /* arguments corresponding to   */
                                             /* the opcode, see              */
                                             /* GSY_RQB_ARGS_TYPE            */
    GSY_RQB_TRAILER                          /* additional RQB trailer, see  */
                                             /* file gsy_cfg.h               */
} GSY_RQB_TYPE;


/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*
 * Access to upper RQB
*/
#define GSY_UPPER_RQB_DELAY_PORT_GET(_pRbu)					((_pRbu)->args.Delay.Port)
#define GSY_UPPER_RQB_DELAY_PORT_SET(_pRbu, _Val)			((_pRbu)->args.Delay.Port = (_Val))
#define GSY_UPPER_RQB_DELAY_TIME_GET(_pRbu)					((_pRbu)->args.Delay.Time)
#define GSY_UPPER_RQB_DELAY_TIME_SET(_pRbu, _Val)			((_pRbu)->args.Delay.Time = (_Val))
#define GSY_UPPER_RQB_DELAY_MIN_GET(_pRbu)					((_pRbu)->args.Delay.Min)
#define GSY_UPPER_RQB_DELAY_MIN_SET(_pRbu, _Val)			((_pRbu)->args.Delay.Min = (_Val))
#define GSY_UPPER_RQB_DELAY_MAX_GET(_pRbu)					((_pRbu)->args.Delay.Max)
#define GSY_UPPER_RQB_DELAY_MAX_SET(_pRbu, _Val)			((_pRbu)->args.Delay.Max = (_Val))
#define GSY_UPPER_RQB_DELAY_COUNT_GET(_pRbu)	  			((_pRbu)->args.Delay.Count)
#define GSY_UPPER_RQB_DELAY_COUNT_SET(_pRbu, _Val)			((_pRbu)->args.Delay.Count = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_PORT_GET(_pRbu)				((_pRbu)->args.PrmRecord.EddPortId)
#define GSY_UPPER_RQB_PRMRECORD_PORT_SET(_pRbu,_Val)		((_pRbu)->args.PrmRecord.EddPortId = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_IDX_GET(_pRbu)				((_pRbu)->args.PrmRecord.RecordIndex)
#define GSY_UPPER_RQB_PRMRECORD_IDX_SET(_pRbu,_Val)			((_pRbu)->args.PrmRecord.RecordIndex = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_LEN_GET(_pRbu)				((_pRbu)->args.PrmRecord.RecordDataLen)
#define GSY_UPPER_RQB_PRMRECORD_LEN_SET(_pRbu,_Val)			((_pRbu)->args.PrmRecord.RecordDataLen = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_PTR_GET(_pRbu)				((_pRbu)->args.PrmRecord.pRecordData)
#define GSY_UPPER_RQB_PRMRECORD_PTR_SET(_pRbu,_Val)			((_pRbu)->args.PrmRecord.pRecordData = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_SLOT_GET(_pRbu)				((_pRbu)->args.PrmRecord.Slot)
#define GSY_UPPER_RQB_PRMRECORD_SLOT_SET(_pRbu,_Val)		((_pRbu)->args.PrmRecord.Slot = (_Val))
#define GSY_UPPER_RQB_PRMRECORD_SUBSLOT_GET(_pRbu)			((_pRbu)->args.PrmRecord.Subslot)
#define GSY_UPPER_RQB_PRMRECORD_SUBSLOT_SET(_pRbu,_Val)		((_pRbu)->args.PrmRecord.Subslot = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_PORT_GET(_pRbu)				((_pRbu)->args.PrmWrite.EddPortId)			// 1092865 start
#define GSY_UPPER_RQB_PRMWRITE_PORT_SET(_pRbu,_Val)			((_pRbu)->args.PrmWrite.EddPortId = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_IDX_GET(_pRbu)				((_pRbu)->args.PrmWrite.RecordIndex)
#define GSY_UPPER_RQB_PRMWRITE_IDX_SET(_pRbu,_Val)			((_pRbu)->args.PrmWrite.RecordIndex = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_LEN_GET(_pRbu)				((_pRbu)->args.PrmWrite.RecordDataLen)
#define GSY_UPPER_RQB_PRMWRITE_LEN_SET(_pRbu,_Val)			((_pRbu)->args.PrmWrite.RecordDataLen = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_PTR_GET(_pRbu)				((_pRbu)->args.PrmWrite.pRecordData)
#define GSY_UPPER_RQB_PRMWRITE_PTR_SET(_pRbu,_Val)			((_pRbu)->args.PrmWrite.pRecordData = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_SLOT_GET(_pRbu)				((_pRbu)->args.PrmWrite.Slot)
#define GSY_UPPER_RQB_PRMWRITE_SLOT_SET(_pRbu,_Val)			((_pRbu)->args.PrmWrite.Slot = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_SUBSLOT_GET(_pRbu)			((_pRbu)->args.PrmWrite.Subslot)
#define GSY_UPPER_RQB_PRMWRITE_SUBSLOT_SET(_pRbu,_Val)		((_pRbu)->args.PrmWrite.Subslot = (_Val))
#define GSY_UPPER_RQB_PRMWRITE_ERROFFSET_GET(_pRbu)			((_pRbu)->args.PrmWrite.ErrOffset)
#define GSY_UPPER_RQB_PRMWRITE_ERROFFSET_SET(_pRbu,_Val)	((_pRbu)->args.PrmWrite.ErrOffset = (_Val))
#define GSY_UPPER_RQB_PRMCHANGEPORT_PORTID_GET(_pRbu)						((_pRbu)->args.PrmChangePort.PortID)
#define GSY_UPPER_RQB_PRMCHANGEPORT_PORTID_SET(_pRbu,_Val)					((_pRbu)->args.PrmChangePort.PortID = (_Val))
#define GSY_UPPER_RQB_PRMCHANGEPORT_MODULESTATE_GET(_pRbu)					((_pRbu)->args.PrmChangePort.ModuleState)
#define GSY_UPPER_RQB_PRMCHANGEPORT_MODULESTATE_SET(_pRbu,_Val)				((_pRbu)->args.PrmChangePort.ModuleState = (_Val))
#define GSY_UPPER_RQB_PRMCHANGEPORT_PORTPARAMSNOTAPPLICABLE_GET(_pRbu)		((_pRbu)->args.PrmChangePort.PortparamsNotApplicable)
#define GSY_UPPER_RQB_PRMCHANGEPORT_PORTPARAMSNOTAPPLICABLE_SET(_pRbu,_Val)	((_pRbu)->args.PrmChangePort.PortparamsNotApplicable = (_Val))
#define GSY_UPPER_RQB_PRMCHANGEPORT_APPLYDEFAULTPARAMS_GET(_pRbu)			((_pRbu)->args.PrmChangePort.ApplyDefaultPortparams)
#define GSY_UPPER_RQB_PRMCHANGEPORT_APPLYDEFAULTPARAMS_SET(_pRbu,_Val)		((_pRbu)->args.PrmChangePort.ApplyDefaultPortparams = (_Val))
#define GSY_UPPER_RQB_PRMEND_ERRPORTID_GET(_pRbu)			((_pRbu)->args.PrmEnd.ErrPortId)
#define GSY_UPPER_RQB_PRMEND_ERRPORTID_SET(_pRbu,_Val)		((_pRbu)->args.PrmEnd.ErrPortId = (_Val))
#define GSY_UPPER_RQB_PRMEND_ERRINDEX_GET(_pRbu)			((_pRbu)->args.PrmEnd.ErrIndex)
#define GSY_UPPER_RQB_PRMEND_ERRINDEX_SET(_pRbu,_Val)		((_pRbu)->args.PrmEnd.ErrIndex = (_Val))
#define GSY_UPPER_RQB_PRMEND_ERROFFSET_GET(_pRbu)			((_pRbu)->args.PrmEnd.ErrOffset)
#define GSY_UPPER_RQB_PRMEND_ERROFFSET_SET(_pRbu,_Val)		((_pRbu)->args.PrmEnd.ErrOffset = (_Val))
#define GSY_UPPER_RQB_PRMEND_ERRFAULT_GET(_pRbu)			((_pRbu)->args.PrmEnd.ErrFault)
#define GSY_UPPER_RQB_PRMEND_ERRFAULT_SET(_pRbu,_Val)		((_pRbu)->args.PrmEnd.ErrFault = (_Val))	// 1092865 end

#define GSY_UPPER_RQB_PRMEND_PORTPARAMSNOTAPPLICABLE_SET(_pRbu,_pVal,_Cnt)                              \
{   LSA_UINT32  j;                                                                                      \
    for (j = 0; j < (_Cnt); ++j) { ((_pRbu)->args.PrmEnd.PortparamsNotApplicable[j] = (_pVal[j])); }    \
}
#define GSY_UPPER_RQB_PRMCOMMIT_APPLYDEFAULTPORTPARAMS_GET(_pRbu,_pVal,_Cnt)                            \
{   LSA_UINT32  j;                                                                                      \
    for (j = 0; j < (_Cnt); ++j) { ((_pVal[j]) = (_pRbu)->args.PrmCommit.ApplyDefaultPortparams[j]); }  \
}

#define GSY_UPPER_RQB_PRMIND_PTR_GET(_pRbu)					((_pRbu)->args.PrmInd.pDiag)
#define GSY_UPPER_RQB_PRMIND_PTR_SET(_pRbu,_Val)			((_pRbu)->args.PrmInd.pDiag = (_Val))
#define GSY_UPPER_RQB_PRMIND_PORTID_GET(_pRbu)				((_pRbu)->args.PrmInd.EddPortId)
#define GSY_UPPER_RQB_PRMIND_PORTID_SET(_pRbu,_Val)			((_pRbu)->args.PrmInd.EddPortId = (_Val))
#define GSY_UPPER_RQB_PRMIND_ACTCNT_GET(_pRbu)				((_pRbu)->args.PrmInd.DiagCnt)
#define GSY_UPPER_RQB_PRMIND_ACTCNT_SET(_pRbu,_Val)			((_pRbu)->args.PrmInd.DiagCnt = (_Val))
#define GSY_UPPER_RQB_DIAGIND_PARAM_GET(_pRbu)				((_pRbu)->args.DiagInd)
#define GSY_UPPER_RQB_DIAGIND_TRIGGER_GET(_pRbu)			((_pRbu)->args.DiagInd.Trigger)
#define GSY_UPPER_RQB_DIAGIND_TRIGGER_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.Trigger = (_Val))
#define GSY_UPPER_RQB_DIAGIND_INDCNT_GET(_pRbu)				((_pRbu)->args.DiagInd.IndCount)
#define GSY_UPPER_RQB_DIAGIND_INDCNT_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.IndCount = (_Val))
#define GSY_UPPER_RQB_DIAGIND_SYNCID_GET(_pRbu)				((_pRbu)->args.DiagInd.SyncId)
#define GSY_UPPER_RQB_DIAGIND_SYNCID_SET(_pRbu,_Val) 		((_pRbu)->args.DiagInd.SyncId = (_Val))
#define GSY_UPPER_RQB_DIAGIND_MASTERMAC_GET(_pRbu)		 	((_pRbu)->args.DiagInd.MasterMacAddr)
#define GSY_UPPER_RQB_DIAGIND_MASTERMAC_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.MasterMacAddr = (_Val))
#define GSY_UPPER_RQB_DIAGIND_SUBDOMAIN_GET(_pRbu)		 	((_pRbu)->args.DiagInd.Subdomain)
#define GSY_UPPER_RQB_DIAGIND_SUBDOMAIN_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.Subdomain = ((_Val) ? GSY_DIAG_SUBDOMAIN_CONFIGURED : GSY_DIAG_SUBDOMAIN_NO_RECORD))
#define GSY_UPPER_RQB_DIAGIND_STATESYNC_GET(_pRbu)			((_pRbu)->args.DiagInd.StateSync)
#define GSY_UPPER_RQB_DIAGIND_STATESYNC_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.StateSync = ((_Val) ? GSY_DIAG_SYNCID_SYNCHRONIZED : GSY_DIAG_SYNCID_NOT_SYNCHRONIZED))
#define GSY_UPPER_RQB_DIAGIND_RATEVALID_GET(_pRbu)			((_pRbu)->args.DiagInd.RateValid)
#define GSY_UPPER_RQB_DIAGIND_RATEVALID_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.RateValid = ((_Val) ? GSY_DIAG_RATE_VALID : GSY_DIAG_RATE_INVALID))
#define GSY_UPPER_RQB_DIAGIND_RCVPRIO_GET(_pRbu)			((_pRbu)->args.DiagInd.RcvSyncPrio)
#define GSY_UPPER_RQB_DIAGIND_RCVPRIO_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.RcvSyncPrio = (_Val))
#define GSY_UPPER_RQB_DIAGIND_LOCPRIO_GET(_pRbu)			((_pRbu)->args.DiagInd.LocalPrio)
#define GSY_UPPER_RQB_DIAGIND_LOCPRIO_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.LocalPrio = (_Val))
#define GSY_UPPER_RQB_DIAGIND_PLLSTATE_GET(_pRbu)			((_pRbu)->args.DiagInd.PLLState)
#define GSY_UPPER_RQB_DIAGIND_PLLSTATE_SET(_pRbu,_Val)		((_pRbu)->args.DiagInd.PLLState = (_Val))
#define GSY_UPPER_RQB_DIAGIND_DRIFT_GET(_pRbu)				((_pRbu)->args.DiagInd.DriftInterval)
#define GSY_UPPER_RQB_DIAGIND_DRIFT_SET(_pRbu,_Val)			((_pRbu)->args.DiagInd.DriftInterval = (_Val))
#define GSY_UPPER_RQB_DIAGIND_USER1_GET(_pRbu)				((_pRbu)->args.DiagInd.UserValue1)
#define GSY_UPPER_RQB_DIAGIND_USER1_SET(_pRbu,_Val)			((_pRbu)->args.DiagInd.UserValue1 = (_Val))
#define GSY_UPPER_RQB_DIAGIND_USER2_GET(_pRbu)				((_pRbu)->args.DiagInd.UserValue2)
#define GSY_UPPER_RQB_DIAGIND_USER2_SET(_pRbu,_Val)			((_pRbu)->args.DiagInd.UserValue2 = (_Val))
#define GSY_UPPER_RQB_RECV_DATA_PTR_GET(_pRbu)	   			((_pRbu)->args.RecvData.pBuffer)
#define GSY_UPPER_RQB_RECV_DATA_PTR_SET(_pRbu,_Val)			((_pRbu)->args.RecvData.pBuffer = (_Val))
#define GSY_UPPER_RQB_RECV_DATA_LEN_GET(_pRbu)				((_pRbu)->args.RecvData.Length)
#define GSY_UPPER_RQB_RECV_DATA_LEN_SET(_pRbu,_Val)			((_pRbu)->args.RecvData.Length = (_Val))
#define GSY_UPPER_RQB_RECV_DATA_TIMESTAMP_GET(_pRbu)		((_pRbu)->args.RecvData.TimeStamp)
#define GSY_UPPER_RQB_RECV_DATA_TIMESTAMP_SET(_pRbu,_Val)	((_pRbu)->args.RecvData.TimeStamp = (_Val))
#define GSY_UPPER_RQB_RECV_DATA_RESOLUTION_GET(_pRbu)		((_pRbu)->args.RecvData.Resolution)
#define GSY_UPPER_RQB_RECV_DATA_RESOLUTION_SET(_pRbu,_Val)	((_pRbu)->args.RecvData.Resolution = (_Val))
#define GSY_UPPER_RQB_RECV_DATA_LINEDELAY_GET(_pRbu)		((_pRbu)->args.RecvData.LineDelay)
#define GSY_UPPER_RQB_RECV_DATA_LINEDELAY_SET(_pRbu,_Val)	((_pRbu)->args.RecvData.LineDelay = (_Val))
#define GSY_UPPER_RQB_CANCEL_IND_OPCODE_GET(_pRbu)			((_pRbu)->args.CancelInd.Opcode)
#define GSY_UPPER_RQB_CANCEL_IND_OPCODE_SET(_pRbu,_Val)		((_pRbu)->args.CancelInd.Opcode = (_Val))
#define GSY_UPPER_RQB_CANCEL_IND_COUNT_GET(_pRbu)			((_pRbu)->args.CancelInd.Count)
#define GSY_UPPER_RQB_CANCEL_IND_COUNT_SET(_pRbu,_Val)		((_pRbu)->args.CancelInd.Count = (_Val))
#define GSY_UPPER_RQB_GET_PARAMS_PTR_GET(_pRbu)	   			((_pRbu)->args.pSyncParams)
#define GSY_UPPER_RQB_GET_PARAMS_PTR_SET(_pRbu,_Val)		((_pRbu)->args.pSyncParams = (_Val))
#define GSY_UPPER_RQB_CHANNELINFO_SYNCID_GET(_pRbu)			((_pRbu)->args.ChannelInfo.SyncId)
#define GSY_UPPER_RQB_CHANNELINFO_SYNCID_SET(_pRbu,_Val)	((_pRbu)->args.ChannelInfo.SyncId = (_Val))
#define GSY_UPPER_RQB_MASTER_CONTROL_MODE_GET(_pRbu)			((_pRbu)->args.MasterControl.Mode)
#define GSY_UPPER_RQB_MASTER_CONTROL_MODE_SET(_pRbu,_Val)		((_pRbu)->args.MasterControl.Mode = (_Val))
#define GSY_UPPER_RQB_MASTER_CONTROL_UTCOFFSET_GET(_pRbu)		((_pRbu)->args.MasterControl.UTCOffset)
#define GSY_UPPER_RQB_MASTER_CONTROL_UTCOFFSET_SET(_pRbu,_Val)	((_pRbu)->args.MasterControl.UTCOffset = (_Val))

/*---------------------------------------------------------------------------* 
* Low- und High Byte, Word
*/
#define GSY_HIBYTE(u16_)	((LSA_UINT8)(((LSA_UINT16)(u16_)) >> 8))
#define GSY_LOBYTE(u16_)	((LSA_UINT8)(u16_))
#define GSY_HIWORD(u32_)	((LSA_UINT16)(((LSA_UINT32)(u32_)) >> 16))
#define GSY_LOWORD(u32_)	((LSA_UINT16)(u32_))

/* Macros to put High- and Low bytes/words to an unsigned 16/32-Bit value
*/
#define GSY_WORD(l8_,h8_)		\
		((LSA_UINT16)((((LSA_UINT16)(l8_)) | (((LSA_UINT16)(h8_)) << 8))))

#define GSY_DWORD(l16_,h16_)	\
		((LSA_UINT32)((((LSA_UINT32)(l16_)) | (((LSA_UINT32)(h16_)) << 16))))


/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  gsy_open_channel
 *
 * function:       open a communication channel
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE    opcode:        GSY_OPC_OPEN_CHANNEL
 *     LSA_HANDLE_TYPE    handle:        channel-handle of user
 *     LSA_USER_ID_TYPE   user-id:       id of user
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle_upper:  channel-handle of user
 *     LSA_SYS_PATH_TYPE  sys_path:      system-path of channel                 
 *     LSA_VOID  LSA_FCT_PTR(GSY_UPPER_OUT_FCT_ATTR, 
                             gsy_request_upper_done_ptr)
 *                                    (GSY_UPPER_RQB_PTR_TYPE upper_rqb_ptr)
 *                                       pointer to callback-function
 *
 * RQB return values via callback-function:
 *     RQB args:
 *     LSA_HANDLE_TYPE    handle:        channel-handle of GSY
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_open_channel(
    GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_close_channel
 *
 * function:       close a communication channel
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:   GSY_OPC_CLOSE_CHANNEL
 *     LSA_HANDLE_TYPE   handle:   channel-handle of GSY
 *     LSA_USER_ID_TYPE  user-id:  id of user
 *
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     All other RQB parameters are unchanged.
 *===========================================================================*/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_close_channel(
    GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_request
 *
 * function:       send a request to a communication channel
 *
 * parameters:     GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *
 * RQB parameters: 
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    GSY_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of GSY
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     RQB args:                    Depend on kind of request.
 *     
 * RQB return values via callback-function:
 *     RQB header:
 *     LSA_OPCODE_TYPE   opcode:    GSY_OPC_*
 *     LSA_HANDLE_TYPE   handle:    channel-handle of user
 *     LSA_USER_ID_TYPE  user-id:   id of user
 *     All other RQB parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  GSY_UPPER_IN_FCT_ATTR  gsy_request(
    GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_upper_rqb
 *
 * function:       asynchronous allocation of an upper RQB
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of GSY
 *                 LSA_UINT16                 length:         length of RQB
 *                 GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper-
 *                                                            RQB
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_upper_rqb(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);

/*=============================================================================
 * function name:  gsy_upper_mem
 *
 * function:       asynchronous allocation of an upper memory
 *
 * parameters:     LSA_USER_ID_TYPE           user_id:        id of GSY
 *                 LSA_UINT16                 length:         length of memory
 *                 GSY_UPPER_MEM_PTR_TYPE  upper_mem_ptr:  pointer to upper-
 *                                                            memory
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  GSY_SYSTEM_IN_FCT_ATTR  gsy_upper_mem(
    LSA_USER_ID_TYPE        user_id,
    LSA_UINT16              length,
    GSY_UPPER_MEM_PTR_TYPE  upper_mem_ptr
);


/*====  out functions  =====*/

/*=============================================================================
 * function name:  GSY_RQB_ERROR
 *
 * function:       handle a RQB that cannot be returned to upper callback
 *
 * parameters:     LSA_UINT16         ...  *  ret_val_ptr:
 *                 GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr:  pointer to upper
 *                                                            request block
 * return value:   LSA_VOID
 *===========================================================================*/
#ifndef GSY_RQB_ERROR
LSA_VOID  GSY_UPPER_OUT_FCT_ATTR  GSY_RQB_ERROR(
	GSY_UPPER_RQB_PTR_TYPE  upper_rqb_ptr
);
#endif


/*****************************************************************************/
/*  end of file gsy_usr.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of GSY_USR_H */

#ifndef PND_IOC_EVENT_HANDLER_H
#define PND_IOC_EVENT_HANDLER_H
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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_IOCEventHandler.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Definition of IOC Event Handler classes                                  */
/*  (Listener and Handler for CM CL User or CM CL Device Events)             */
/*                                                                           */
/*****************************************************************************/

class CIOCUser;
class CIOCDevice;

/////////////////////////////////////////////////////////////////////////////
// PND Defines
/////////////////////////////////////////////////////////////////////////////

// constants for filling the tinfo struct
#define PNIO_AL_PROFILE_TYPE            0x08
#define PNIO_AL_AINFO_TYPE              0x00
#define PNIO_AL_CTRL_FLAGS              0x01

// modtypes for plug/pull alarms
#define PNIO_AL_REAL_EQUAL_EXPECT   0x8101
#define PNIO_AL_REAL_NOT_EXPECT     0x8102

typedef enum {
    PNIO_AOFF_BLKTYPE                        = 0,
    PNIO_AOFF_BLKLEN                         = 2,
    PNIO_AOFF_BLKVERSION                     = 4,
    PNIO_AOFF_ALARMTYPE                      = 6,
    PNIO_AOFF_APPLICATIONPROCESSIDENTIFIER   = 8,
    PNIO_AOFF_SLOT                           = 12,
    PNIO_AOFF_SUBSLOT                        = 14,
    PNIO_AOFF_MODULIDENTIFICATION            = 16,
    PNIO_AOFF_SUBMODULIDENTIFICATION         = 20,
    PNIO_AOFF_ALARMSPECIFIER                 = 24,
    PNIO_AOFF_USERSTRUCTUREIDENTIFIER        = 26,
    PNIO_ALARM_NORM_HEADER_OFFSETS_LEN       = 28

} PNIO_IOC_ALARM_NORM_HEADER_OFFSETS;


/////////////////////////////////////////////////////////////////////////////
// CIOCEventHandler
/////////////////////////////////////////////////////////////////////////////

class CIOCEventHandler
{
	// Attributes
private:
	LSA_BOOL         m_isUserStopping;   // CM channel close requested
	PND_HANDLE_TYPE  m_pndHandle;        // PND handle
	CIOCUser        *m_pUser;            // Link to IOC User
	CIOCDevice      *m_pDevice;          // Link to IOC Device
	LSA_VOID (CIOCEventHandler::*m_handler[CM_OPC_MAX])( CM_UPPER_RQB_PTR_TYPE p_rb );  // Pointer to response handler

	// Construction/ Destruction
public:
	CIOCEventHandler( PND_HANDLE_PTR_TYPE pHandle, CIOCUser   *pUser );
	CIOCEventHandler( PND_HANDLE_PTR_TYPE pHandle, CIOCDevice *pDevice );
	virtual ~CIOCEventHandler();

	// Accessors

	// User spefic indications
	LSA_VOID ErrorUserEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB ); // Default for not handled user indications

	// Device specific indications
	LSA_VOID ErrorDeviceEventHandling( CM_UPPER_RQB_PTR_TYPE pRQB ); // Default for not handled device indications
	LSA_VOID ArAlarmIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArAbortIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArPreDataIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArPreOfflineIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArInDataIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArOfflineIndication( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID ArPrmBeginIndication( CM_UPPER_RQB_PTR_TYPE pRqb );
	LSA_VOID ArPrmInDataIndication( CM_UPPER_RQB_PTR_TYPE pRqb );
	LSA_VOID ArDataStatusIndication( CM_UPPER_RQB_PTR_TYPE pRQB );

	// Requests
	LSA_VOID StartUserListening();
	LSA_VOID StopUserListening();
	LSA_VOID StartDeviceListening( LSA_UINT16 nrOfEvents, LSA_UINT16 nrOfAlarms );
	LSA_VOID HandleEvent( LSA_OPCODE_TYPE opcode, CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID RespondIndication( LSA_VOID*p_rb );

	// Class methodes ( Used for callbacks on user and device level )
	static LSA_VOID EventUserReceived( LSA_VOID*p_inst, LSA_VOID*p_rb );
	static LSA_VOID EventDeviceReceived( LSA_VOID*p_inst, LSA_VOID*p_rb );
	LSA_VOID ArAlarmIndicationDone(CM_UPPER_RQB_PTR_TYPE pRQB );

private:
	LSA_VOID initUserMap();
	LSA_VOID initDeviceMap();
	LSA_VOID reprovideUserInd( CM_UPPER_RQB_PTR_TYPE pRQB );
	LSA_VOID traceAbortReason( CM_UPPER_CL_AR_ABORT_PTR_TYPE pAbort ) const;

};

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

#endif // !defined(PND_IOC_EVENT_HANDLER_H)

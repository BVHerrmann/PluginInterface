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
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: clsUsedTimer.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsUsedTimer"
//
//****************************************************************************/

#ifndef __clsUsedTimer_h__
#define __clsUsedTimer_h__

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_TIMER_STATE__IOCTL_OPEN_TIMER_DONE	PNDEV_UINT32_SET_BIT_0
	#define	PNDEV_TIMER_STATE__ADD_LIST_DONE			PNDEV_UINT32_SET_BIT_1


	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uPNDEV_TIMER_RES
	{
		uPNDEV64_HANDLE	hEventStopWithCon;
		uPNDEV64_HANDLE	hEventUtmrTick;
		uPNDEV64_HANDLE	hEventUtmrTickSynch;
		uPNDEV64_HANDLE	hThread;
	}
	uPNDEV_TIMER_RES;

	//########################################################################
	//  Class
	//########################################################################

	class clsUsedTimer
	{
	public:

	//========================================================================
	//  PUBLIC-attributes
	//========================================================================

	// Note:
	//	- ChainPtr must be first element of structure!
	void*	m_pNext;															// ChainPtr for putting object to UsedTimerList
	void*	m_pPrev;															// ChainPtr for putting object to UsedTimerList

	//------------------------------------------------------------------------
	void*	m_hTimerVerifier;													// for verifying TimerHandle
	UINT32	m_lStateOpenTimer;

	//========================================================================
	//  PUBLIC-inline-methods
	//========================================================================

	inline clsUsedDevice*		fnGetUsedDevice(				void)					{return(m_pUsedDevice);}
	inline UINT64				fnGetHandleReceiverSys(			void)					{return(m_uReceiverSys.lUint64);}
	inline HANDLE				fnGetHandleEventUtmrTick(		void)					{return(m_uTimer.hEventUtmrTick.hHandle);}
	inline HANDLE				fnGetHandleEventUtmrTickSynch(	void)					{return(m_uTimer.hEventUtmrTickSynch.hHandle);}
	inline HANDLE				fnGetHandleTimerThread(			void)					{return(m_uTimer.hThread.hHandle);}

	//========================================================================
	//  PUBLIC-methods
	//========================================================================

	clsUsedTimer(	void);
	~clsUsedTimer(	void);

	//========================================================================
	// located at clsUsedTimer.cpp

	void	fnSetVarOpenTimer1(			HANDLE					hDriverDllIn,
										clsDataSet*				pDataSetIn,
										clsUsedDevice*			pUsedDevice,
										uPNDEV_OPEN_TIMER_IN*	pIn);
	void	fnSetVarOpenTimer2(			uPNDEV_OPEN_TIMER_OUT*	pOut);
	void	fnDoTimer(					void);



	//========================================================================
	// located at os_clsUsedTimer.cpp (OS specific)

	BOOLEAN	fnStartUpTimerRes(			const UINT32					lPrioThreadIn,
										const UINT32					lStackSizeThreadIn,
										const _TCHAR*					sNameThreadIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnShutDownTimerRes(			const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnSetEventTimer(			ePNDEV_EVENT_TIMER				eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnWaitForEventThreadService(ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);

	BOOLEAN	fnWaitForEventTimer(		const UINT32					lDimErrorIn,
										ePNDEV_EVENT_TIMER*			pEventOut,
										_TCHAR*							sErrorOut);

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	static ULONG WINAPI	fnTimer(void* pFctArgIn);

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	private:

	//========================================================================
	//  PRIVATE-attributes
	//========================================================================

	HANDLE						m_hDriverDll;
	clsDataSet*					m_pDataSet;
	clsUsedDevice*				m_pUsedDevice;
	uPNDEV64_HANDLE				m_uReceiverSys;									// handle to ReceiverObject used at driver (use 64Bit value because DriverHandle may be a 64Bit handle!)
	PNDEV_CBF_TICK				m_pCbfTick;
	void*						m_pCbfParUser;
	BOOLEAN						m_bWaitParEventOk;
	uPNDEV_TIMER_RES			m_uTimer;
	HANDLE						m_hEventConStop;

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	HANDLE	m_hArrayEvent[PNDEV_DIM_ARRAY_EVENT_TIMER];


	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	//========================================================================
	//  PRIVATE-inline-methods
	//========================================================================

	//========================================================================
	//  PRIVATE-methods
	//========================================================================
	};

#endif

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
/*  F i l e               &F: clsUsedDevice.h                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsUsedDevice"
//
//****************************************************************************/

#ifndef __clsUsedDevice_h__
#define __clsUsedDevice_h__

	//########################################################################
	//  Defines
	//########################################################################

	#define	PNDEV_DEVICE_STATE__IOCTL_OPEN_DEVICE_DONE	PNDEV_UINT32_SET_BIT_0
	#define	PNDEV_DEVICE_STATE__ADD_LIST_DONE			PNDEV_UINT32_SET_BIT_1

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_EVENT_THREAD_SERVICE
	{
		ePNDEV_EVENT_THREAD_SERVICE_INVALID			= 0,
		ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST	= 1,
		ePNDEV_EVENT_THREAD_SERVICE_CON_STOP		= 2,

		ePNDEV_EVENT_THREAD_SERVICE_TIMEOUT			= 0xffff
	}
	ePNDEV_EVENT_THREAD_SERVICE;

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_DMM_QUEUE
	{
		ePNDEV_DMM_QUEUE_MEM_NRT		= 0,
		ePNDEV_DMM_QUEUE_MEM_DESC_TABLE	= 1
	}
	ePNDEV_DMM_QUEUE;

	//########################################################################
	//  Structures
	//########################################################################

	typedef struct _uPNDEV_THREAD_RES_SERVICE
	{
		HANDLE	hEventConIntTest;
		HANDLE	hEventConStop;
	}
	uPNDEV_THREAD_RES_SERVICE;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_THREAD_RES_DLL
	{
		uPNDEV64_HANDLE	hEventStop;
		uPNDEV64_HANDLE	hEventStopWithCon;
		uPNDEV64_HANDLE	hEventUinfo;
		uPNDEV64_HANDLE	hEventUinfoUm;			// EventHandle if event is fired by a UserModeThread
		uPNDEV64_HANDLE	hEventUisr;
		uPNDEV64_HANDLE	hThread;
	}
	uPNDEV_THREAD_RES_DLL;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_EVENT_FATAL_ASYNC
	{
		ePNDEV_EVENT_CLASS	eEventClass;
		_TCHAR				sEvent[PNDEV_SIZE_STRING_BUF];
	}
	uPNDEV_EVENT_FATAL_ASYNC;

	//------------------------------------------------------------------------
	typedef struct _uPNDEV_NRT_PRE_BUFFER
	{
		uPNDEV_NRT_RQB*	pNext;
		uPNDEV_NRT_RQB*	pPrev;
	}
	uPNDEV_NRT_PRE_BUFFER;

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	typedef struct _uPNDEV_FMQ_MSG
	{
		UINT32						lNext;
		ePNDEV_EVENT_THREAD_SERVICE	eEvent;
	}
	uPNDEV_FMQ_MSG;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	//########################################################################
	//  Class
	//########################################################################

	class clsUsedDevice
	{
	public:

	//========================================================================
	//  PUBLIC-attributes
	//========================================================================

	// Note:
	//	- ChainPtr must be first element of structure!
	void*	m_pNext;															// ChainPtr for putting object to UsedDeviceList
	void*	m_pPrev;															// ChainPtr for putting object to UsedDeviceList

	//------------------------------------------------------------------------
	void*	m_hDeviceVerifier;													// for verifying DeviceHandle
	UINT32	m_lStateOpenDevice;
	clsDmm	m_oMemDmm;

	//========================================================================
	//  PUBLIC-inline-methods
	//========================================================================

	inline uPNDEV64_HANDLE		fnGetHandleDeviceSys(	void)					{return(m_hDeviceSys);}
	inline UINT64				fnGetHandleAppSys(		void)					{return(m_uAppSys.lUint64);}
	inline ePNDEV_ICU			fnGetTypeIcu(			void)					{return(m_eIcu);}
	inline ePNDEV_INT_ACTION	fnGetIntAction(			void)					{return(m_eIntAction);}
	inline BOOLEAN				fnGetMasterApp(			void)					{return(m_bMasterApp);}
	inline UINT32				fnGetSizeSdramSys(		void)					{return(m_lSizeSdramSys);}
	inline UINT32				fnGetSizePnipIrte(		void)					{return(m_lSizePnipIrte);}
	inline UINT32				fnGetCtrUisr(			void)					{return(m_lCtrUisr);}
	inline _TCHAR*				fnGetPciLoc(			void)					{return(m_sPciLoc);}
	inline HANDLE				fnGetHandleEventUinfo(	void)					{return(m_uThreadDll.hEventUinfo.hHandle);}
	inline HANDLE				fnGetHandleEventUinfoUm(void)					{return(m_uThreadDll.hEventUinfoUm.hHandle);}
	inline HANDLE				fnGetHandleEventUisr(	void)					{return(m_uThreadDll.hEventUisr.hHandle);}
	inline HANDLE				fnGetHandleThreadDll(	void)					{return(m_uThreadDll.hThread.hHandle);}
	inline uLIST_HEADER*		fnGetPtrListUsedTimer(	void)					{return(&m_uListUsedTimer);}
	inline void					fnIncrCtrUsedTimer(		void)					{m_lCtrUsedTimer++;}
	inline void					fnDecrCtrUsedTimer(		void)					{m_lCtrUsedTimer--;}


	//------------------------------------------------------------------------
	inline void		fnSetIntAction(					ePNDEV_INT_ACTION	eIntActionIn)	{m_eIntAction = eIntActionIn;}
	inline void		fnSetIntEnabledIrtePnipIntel(	BOOLEAN				bIntEnabledIn)	{m_bIntEnabledIrtePnipIntel = bIntEnabledIn;}
	inline void		fnSetCtrUisr(					UINT32				lCtrUisrIn)		{m_lCtrUisr = lCtrUisrIn;}

	//------------------------------------------------------------------------
	__forceinline
	UINT32 READ_REG_ICU(const UINT32 lOffsetIn)
	{
	volatile UINT32* pRegTmp = ((volatile UINT32*) (m_pIcu + lOffsetIn));

		return(*pRegTmp);
	}

	//------------------------------------------------------------------------
	__forceinline
	void WRITE_REG_ICU(	const UINT32	lOffsetIn,
						const UINT32	lValueIn)
	{
	volatile UINT32* pRegTmp = ((volatile UINT32*) (m_pIcu + lOffsetIn));

		*pRegTmp = lValueIn;
	}

	//------------------------------------------------------------------------
	__forceinline
	UINT32 READ_REG_PNIP(const UINT32 lOffsetIn)
	{
	volatile UINT32* pRegTmp = ((volatile UINT32*) (m_pPnIp + lOffsetIn));

		return(*pRegTmp);
	}

	//------------------------------------------------------------------------
	__forceinline
	void WRITE_REG_PNIP(const UINT32	lOffsetIn,
						const UINT32	lValueIn)
	{
	volatile UINT32* pRegTmp = ((volatile UINT32*) (m_pPnIp + lOffsetIn));

		*pRegTmp = lValueIn;
	}

	//------------------------------------------------------------------------
	__forceinline
	void WRITE_REG_PNIP_UINT16(	const UINT32	lOffsetIn,
								const UINT16	lValueIn)
	{
	volatile UINT16* pRegTmp = ((volatile UINT16*) (m_pPnIp + lOffsetIn));

		*pRegTmp = lValueIn;
	}

	//========================================================================
	//  PUBLIC-methods
	//========================================================================

	clsUsedDevice(	void);
	~clsUsedDevice(	void);

	//========================================================================
	// located at os_clsUsedDevice.cpp (OS specific)

	BOOLEAN	fnStartUpThreadRes(			const UINT32					lPrioThreadIn,
										const UINT32					lStackSizeThreadIn,
										const _TCHAR*					sNameThreadIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnShutDownThreadRes(		const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnSetEventThreadDll(		ePNDEV_EVENT_THREAD_DLL			eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnWaitForEventThreadService(ePNDEV_EVENT_THREAD_SERVICE		eEventIn,
										const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnWaitForEventThreadDll(	const UINT32					lDimErrorIn,
										ePNDEV_EVENT_THREAD_DLL*		pEventOut,
										_TCHAR*							sErrorOut);
	BOOLEAN	fnCreateEventSys(			const ePNDEV_EVENT_THREAD_DLL	eEventIn,
										const UINT32					lDimErrorIn,
										uPNDEV64_HANDLE*				pHandleEventOut,
										_TCHAR*							sErrorOut);
	BOOLEAN fnShutDownUsedTimer(		const UINT32					lDimErrorIn,
										_TCHAR*							sErrorOut);

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	static ULONG WINAPI	fnThreadDll(void* pFctArgIn);

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	static void* fnThreadDll(void* pFctArgIn);

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	static void* fnThreadDll(void* pFctArgIn);

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_UNKNOWN)
	//------------------------------------------------------------------------
	#endif

	ePNDEV_RESULT 	fnMapMemory(	clsUsedDevice*						pUsedDeviceIn,
									uPNDEV_MAP_MEMORY_IN*				pIn,
									uPNDEV_MAP_MEMORY_OUT*				pOut);
	BOOLEAN 		fnUnmapMemory(	uPNDEV_HW_RES_SINGLE_UM*			pBarUm,
									uPNDEV_HW_RES_SINGLE_UM*			pSdramUm,
									uPNDEV_HW_RES_SINGLE_UM*			pHostMemUm);

	//========================================================================
	// located at clsUsedDevice.cpp

	void	fnSetVarOpenDevice1(		HANDLE						hDriverDllIn,
										clsDataSet*					pDataSetIn,
										uPNDEV64_HANDLE				hDeviceSysIn,
										uPNDEV_OPEN_DEVICE_IN*		pIn);
	BOOLEAN	fnSetVarOpenDevice2(		uPNDEV_OPEN_DEVICE_OUT*		pOut);
	BOOLEAN	fnResetDevice(				const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnVerifyEnvFirstOpen(		const BOOLEAN				bVerifyEnvironmentIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnVerifyIntHandling(		const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnSetEventList(				const ePNDEV_EVENT_CLASS	eEventClassIn,
										const UINT32				lEventIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnStartFw(					uPNDEV_OPEN_DEVICE_IN*		pOpenDeviceIn,
										uPNDEV_OPEN_DEVICE_OUT*		pCurDevice,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnStartBootFw(				uPNDEV_FW_DESC*				pBootFwIn,
										uPNDEV_HW_INFO*				pHwInfoIn,
										const UINT32				lSizeDriverSdramIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnStartUserFw(				uPNDEV_USER_FW*				pUserFwIn,
										uPNDEV_HW_INFO*				pHwInfoIn,
										const UINT32				lDimErrorIn,
										uPNCORE_PCI_HOST_PAR*		pPciMasterParIn,
										uPNDEV_HW_DESC_SDRAM*		pHwResSdramInOut,
										_TCHAR*						sErrorOut);
	#if !defined (PNDEV_OS_ADONIS)
	BOOLEAN	fnStartUserFwLinux(			uPNDEV_USER_FW*				pUserFwIn,
										uPNDEV_HW_INFO*				pHwInfoIn,
										const UINT32				lDimErrorIn,
										uPNCORE_PCI_HOST_PAR*		pPciMasterParIn,
										uPNDEV_HW_DESC_SDRAM*		pHwResSdramInOut,
										_TCHAR*						sErrorOut);
	#endif
	BOOLEAN	fnCopyFwToAsicSdram(		const UINT32				lSizeDriverSdramIn,
										uPNDEV_FW_DESC*				pFwDescIn,
										uPNDEV_HW_INFO*				pHwInfoIn,
										const BOOLEAN				bUserFwIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnLockMultiApp(				const BOOLEAN				bLockMultiAppIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnNotifyDllState(			const BOOLEAN				bReadyIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnAllocEventId(				const UINT32				lDimErrorIn,
										UINT32*						pEventIdOut,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnFreeEventId(				const UINT32				lEventIdIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	void	fnStopIntTest(				void);
	void	fnMaskIcu_All(				BOOLEAN						bInitIn);
	BOOLEAN	fnUnmaskIcu_Desired(		uPNDEV_CHANGE_INT_IN*		pChangeIntIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	void	fnSetEoiInactiveTime_Mux(	const ePNDEV_INT_SRC		eIntSrcIn);
	void	fnAckTopLevelIcu(			const ePNDEV_INT_SRC		eIntSrcIn);
	void	fnDoThreadDll(				void);
	BOOLEAN	fnEvtUinfo(					const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnEvtUisr(					const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	void	fnFoundIntSrc(				const ePNDEV_INT_SRC		eIntSrcIn,
										const BOOLEAN				bDeviceTestIn);
	BOOLEAN	fnInitDmmNrt(				uPNDEV_HW_DESC				uMemNrtIn,
										uPNDEV_HW_DESC				uMemDescTableIn,
										const UINT32				lAlignmentIn,
										const UINT32				lDimErrorIn,
										UINT32*						pCtrNrtRqbOut,
										_TCHAR*						sErrorOut);

	private:

	//========================================================================
	//  PRIVATE-attributes
	//========================================================================

	HANDLE						m_hDriverDll;
	clsDataSet*					m_pDataSet;
	uPNDEV64_HANDLE				m_hDeviceSys;									// handle to DeviceObject used at driver
	uPNDEV64_HANDLE				m_uAppSys;										// handle to AppObject used at driver (use 64Bit value because DriverHandle may be a 64Bit handle!)
	BOOLEAN						m_bMasterApp;
	PNDEV_CBF_EVENT				m_pCbfEvent;
	PNDEV_CBF_ISR				m_pCbfIsr;
	void*						m_pCbfParUser;
	_TCHAR						m_sPciLoc[PNDEV_SIZE_STRING_BUF];
	ePNDEV_ICU					m_eIcu;
	volatile UINT8*				m_pIcu;
	volatile UINT8*				m_pPnIp;
	uPNDEV_HW_INFO				m_uHwInfo;
	UINT32						m_lSizeSdramSys;
	UINT32						m_lSizePnipIrte;
	ePNDEV_INT_ACTION			m_eIntAction;
	BOOLEAN						m_bIntEnabledIrtePnipIntel;
	UINT32						m_lEventEnabled0;
	UINT32						m_lEventEnabled1;
	UINT32						m_lEventEnabled2;
	BOOLEAN						m_bEventEnable[PNDEV_MSIX_VECTOR_COUNT_MAX];
	UINT32						m_lCtrUisr;
	BOOLEAN						m_bWaitParEventOk;
	uPNDEV_THREAD_RES_SERVICE	m_uThreadService;
	uPNDEV_THREAD_RES_DLL		m_uThreadDll;
	UINT32						m_lCtrUsedTimer;
	uLIST_HEADER				m_uListUsedTimer;								// UsedTimerList
	UINT32						m_lBitStreamId;									// Inc12 - can be undone when Inc9 support will be removed


	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	HANDLE	m_hArrayEvent[PNDEV_DIM_ARRAY_EVENT_THREAD_DLL];

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	fmq_t		m_uFmqConStop;
	fmq_t		m_uFmqConIntTest;
	sigset_t	m_lSignalMask;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	INT32	m_uEventConStop;
	INT32	m_uEventConIntTest;
	INT32	m_uMaxEventFD;

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

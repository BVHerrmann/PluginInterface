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
/*  F i l e               &F: clsDataSet.h                              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations of class "clsDataSet" (DataSet of DLL, dependent functions)
//
//****************************************************************************/


#ifndef __clsDataSet_h__
#define __clsDataSet_h__

	//########################################################################
	//  Defines
	//########################################################################

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	#define PNDEV_DRV_NAME	"/dev/pndevdrv"

	#endif

	//------------------------------------------------------------------------
	typedef enum _ePNDEV_CRIT_SEC_DLL
	{
		ePNDEV_CRIT_SEC_DLL_INVALID		= 0,
		ePNDEV_CRIT_SEC_DLL_SERVICE		= 1,
		ePNDEV_CRIT_SEC_DLL_IOCTL		= 2
	}
	ePNDEV_CRIT_SEC_DLL;

	//########################################################################
	//  Structures
	//########################################################################
	//------------------------------------------------------------------------

	//########################################################################
	//  Class
	//########################################################################

	class clsDataSet
	{
	public:

	//========================================================================
	//  PUBLIC-attributes
	//========================================================================

	//========================================================================
	//  PUBLIC-inline-methods
	//========================================================================

	inline HANDLE			fnGetDriverVerifier(	void)						{return(m_hDriverVerifier);}
	inline uLIST_HEADER*	fnGetPtrListUsedDevice(	void)						{return(&m_uListUsedDevice);}
	inline INT32			fnGetHandleDriverSys(	void)						{return(m_hDriverSys);}

	//------------------------------------------------------------------------
	inline void				fnSetDriverVerifier(	HANDLE hDriverVerifierIn)	{m_hDriverVerifier = hDriverVerifierIn;}
	inline void				fnIncrCtrUsedDevice(	void)						{m_lCtrUsedDevice++;}
	inline void				fnDecrCtrUsedDevice(	void)						{m_lCtrUsedDevice--;}
	inline void				fnSuppressCritSec(		BOOLEAN	bSuppressIn)		{m_bSuppressCritSec = bSuppressIn;}

	//========================================================================
	//  PUBLIC-methods
	//========================================================================

	clsDataSet(	const UINT32	lDimErrorIn,
				_TCHAR*			sErrorOut);
	~clsDataSet(void);

	//========================================================================
	// located at os_clsDataSet.cpp (OS specific)

	BOOLEAN	fnStartUpSys(				const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnShutDownSys(				const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnGetCtrDevice(				const UINT32				lDimErrorIn,
										UINT32*						pCtrDeviceOut,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnGetDeviceInfoAll(			const UINT32				lDimOutputArrayIn,
										const UINT32				lDimErrorIn,
										uPNDEV_GET_DEVICE_INFO_OUT	uArrayDeviceInfoOut[],
										_TCHAR*						sErrorOut);
	BOOLEAN	fnGetDeviceHandleSys(		const _TCHAR*				sPciLocIn,
										const UINT32				lDimErrorIn,
										uPNDEV64_HANDLE*  			pHandleDeviceOut,
										BOOLEAN*					pErrorDeviceNotFoundOut,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnReleaseDeviceHandleSys(	const uPNDEV64_HANDLE		hDeviceIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnDoDeviceIoctl(			void*						pUsedDeviceIn,
										const UINT32				lIoctlIn,
										const UINT32				lSizeInputBufIn,
										const UINT32				lSizeOutputBufIn,
										void*						pIn,
										void*						pOut);
	BOOLEAN	fnStartUpLockRes(			const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnShutDownLockRes(			const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnEnterCritSec(				ePNDEV_CRIT_SEC_DLL			eCritSecIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);
	BOOLEAN	fnLeaveCritSec(				ePNDEV_CRIT_SEC_DLL			eCritSecIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut);

	//------------------------------------------------------------------------
	static BOOLEAN	fnCheckVersionDetails(	const UINT32		lDimErrorIn,
											_TCHAR*				sErrorOut);
	static void		fnAddLastError(			const _TCHAR*		sSrc1In,
											const _TCHAR*		sSrc2In,
											const UINT32		lDimResultIn,
											_TCHAR*				sResultOut);
	static void		fnSetCurAction(			const void*			hListBoxIn,
											const _TCHAR*		sMsgIn);
	static void		fnSetTrace(				const _TCHAR*		sTraceIn);


	//========================================================================
	// located at clsDataSet.cpp

	BOOLEAN	fnStartUpDll(		const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut);
	BOOLEAN	fnShutDownDll(		HANDLE			hDriverIn,
								const UINT32	lDimErrorIn,
								_TCHAR*			sErrorOut);
	BOOLEAN	fnDoEntryActions(	const UINT32	lSizeInputBufIn,
								const UINT32	lSizeOutputBufIn,
								const UINT32	lMinSizeInputBufIn,
								const UINT32	lMinSizeOutputBufIn,
								const BOOLEAN	bCompareEqualIn,
								void*			pOutputBufIn,
								HANDLE			hDriverIn,
								const UINT32	lDimErrorIn,
								BOOLEAN*		pFatalBufOut,
								_TCHAR*			sErrorOut);
	BOOLEAN	fnDeviceInUse(		_TCHAR*			sPciLocIn);

	private:

	//========================================================================
	//  PRIVATE-attributes
	//========================================================================

	HANDLE			m_hDriverVerifier;
	UINT32			m_lCtrDevice;
	UINT32			m_lCtrUsedDevice;
	uLIST_HEADER	m_uListUsedDevice;											// UsedDeviceList
	INT32			m_hDriverSys;												// Windows:	NULL
																				// Adonis:	FileDescriptor
	BOOLEAN			m_bSuppressCritSec;

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	CRITICAL_SECTION	m_uCritSec_Service;
	CRITICAL_SECTION	m_uCritSec_Ioctl;
	const GUID*			m_pInterfaceClassGuid;
	HDEVINFO			m_hDeviceInfoList;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_ADONIS)
	//------------------------------------------------------------------------
	pthread_mutex_t	m_uCritSec_Service;
	pthread_mutex_t	m_uCritSec_Ioctl;

	//------------------------------------------------------------------------
	#elif defined (PNDEV_OS_LINUX)
	//------------------------------------------------------------------------
	pthread_mutex_t	m_uCritSec_Service;
	pthread_mutex_t	m_uCritSec_Ioctl;
	
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

	//------------------------------------------------------------------------
	#if defined (PNDEV_OS_WIN)
	//------------------------------------------------------------------------
	BOOLEAN			fnAllocDeviceInfoList(		const GUID*			pInterfaceClassGuidIn,
												const UINT32		lDimErrorIn,
												HDEVINFO*			pDeviceInfoListOut,
												UINT32*				pCtrDeviceGuidOut,
												_TCHAR*				sErrorOut);
	BOOLEAN			fnFreeDeviceInfoList(		const HDEVINFO		hDeviceInfoListIn);
	BOOLEAN			fnGetDevicePath(			const HDEVINFO		hDeviceInfoListIn,
												const GUID*			pInterfaceClassGuidIn,
												const _TCHAR*		sPciLocIn,
												const UINT32		lDimDevicePathIn,
												const UINT32		lDimErrorIn,
												_TCHAR*				sDevicePathOut,
												BOOLEAN*			pErrorDeviceNotFoundOut,
												_TCHAR*				sErrorOut);
	BOOLEAN			fnGetDeviceProperty(		const HDEVINFO		hDeviceInfoListIn,
												SP_DEVINFO_DATA*	pDeviceElementIn,
												const UINT32		lPropertyIn,
												const UINT32		lBytesInfoIn,
												_TCHAR*				sInfoOut,
												_TCHAR*				sErrorOut);
	BOOLEAN			fnGetDevicePropertiesAll(	const HDEVINFO		hDeviceInfoListIn,
												SP_DEVINFO_DATA*	pDeviceElementIn,
												const UINT32		lDimHardwareIdIn,
												const UINT32		lDimPciLocIn,
												const UINT32		lDimClassNameIn,
												const UINT32		lDimErrorIn,
												_TCHAR*				sHardwareIdOut,
												_TCHAR*				sPciLocOut,
												_TCHAR*				sClassNameOut,
												UINT32*				pBusOut,
												UINT32*				pDeviceOut,
												UINT32*				pFunctionOut,
												_TCHAR*				sErrorOut);
	ePNDEV_BOARD	fnGetBoardType(				const _TCHAR*		sHardwareIdIn,
												UINT16*				pVendorIdOut,
												UINT16*				pDeviceIdOut);

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

	};

#endif

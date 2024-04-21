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
/*  F i l e               &F: clsUsedDevice.cpp                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code of class "clsUsedDevice"
//
//****************************************************************************/

#include "stdafx.h"															// environment specific IncludeFiles
#include "Inc.h"

//########################################################################
//	PUBLIC
//########################################################################

//************************************************************************
//  D e s c r i p t i o n :
//
//	constructor
//************************************************************************

clsUsedDevice::clsUsedDevice(void)
{
	// preset all member of this class
	memset(	this,
			0,
			sizeof(clsUsedDevice));

	// set verifier for DeviceHandle
	m_hDeviceVerifier = this;

	// initialize UsedTimerList
	fnInitList(&m_uListUsedTimer);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	destructor
//************************************************************************

clsUsedDevice::~clsUsedDevice(void)
{
	m_pNext				= NULL;
	m_pPrev				= NULL;
	m_hDeviceVerifier	= NULL;
	m_hDriverDll		= NULL;
	m_pDataSet			= NULL;
	m_pCbfParUser		= NULL;
	m_pIcu				= NULL;
	m_pPnIp				= NULL;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set variables of UsedDeviceObject
//************************************************************************

void clsUsedDevice::fnSetVarOpenDevice1(HANDLE					hDriverDllIn,
										clsDataSet*				pDataSetIn,
										uPNDEV64_HANDLE			hDeviceSysIn,
										uPNDEV_OPEN_DEVICE_IN*	pIn)
{
	m_hDriverDll	= hDriverDllIn;
	m_pDataSet		= pDataSetIn;
	m_hDeviceSys	= hDeviceSysIn;

	m_bMasterApp	= pIn->bMasterApp;
	m_pCbfEvent		= pIn->uCbfEvent.pPtr;
	m_pCbfIsr		= pIn->uCbfIsr.pPtr;
	m_pCbfParUser	= pIn->uCbfParUser.pPtr;

	_tcscpy_s(	m_sPciLoc,
				_countof(m_sPciLoc),
				pIn->sPciLoc);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set variables of UsedDeviceObject
//************************************************************************

BOOLEAN clsUsedDevice::fnSetVarOpenDevice2(uPNDEV_OPEN_DEVICE_OUT* pOut)
{
BOOLEAN bResult = FALSE;

	// use 64Bit value because DriverHandle may be a 64Bit handle!
	m_uAppSys.lUint64 = pOut->uInternal.uAppSys.lUint64;

	m_eIcu	= (ePNDEV_ICU) pOut->uInternal.eIcu;
	m_pIcu	= pOut->uInternal.uIcu.pPtr;

	// store HwInfo
	*((uPNDEV_HW_INFO*) &m_uHwInfo) = *((uPNDEV_HW_INFO*) &pOut->uHwInfo);

	// preset
	bResult = TRUE;

	switch	(pOut->uHwInfo.eAsic)
			// AsicType
	{
		case ePNDEV_ASIC_ERTEC400:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uErtec400.uEmifSdram.lSizeDriver;
			m_lSizePnipIrte	= pOut->uHwRes.as.uErtec400.uIrte.lSize;

			break;
		}
		case ePNDEV_ASIC_ERTEC200:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uErtec200.uEmifSdram.lSizeDriver;
			m_lSizePnipIrte	= pOut->uHwRes.as.uErtec200.uIrte.lSize;

			break;
		}
		case ePNDEV_ASIC_SOC1:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uSoc1.uDdr2Sdram.lSizeDriver;
			m_lSizePnipIrte	= pOut->uHwRes.as.uSoc1.uIrte.lSize;

			break;
		}
		case ePNDEV_ASIC_ERTEC200P:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uErtec200P.uEmcSdram.lSizeDriver;
			m_lSizePnipIrte	= pOut->uHwRes.as.uErtec200P.uPnIp.lSize;
			
			m_pPnIp	= pOut->uHwRes.as.uErtec200P.uPnIp.uBase.pPtr;

			break;
		}
		case ePNDEV_ASIC_I210:
		{
			break;
		}
		case ePNDEV_ASIC_I210IS:
		{
			break;
		}
		case ePNDEV_ASIC_I82574:
		{
			break;
		}
		case ePNDEV_ASIC_IX1000:
		{
			break;
		}
		case ePNDEV_ASIC_HERA:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uHera.uDdr3Sdram.lSizeDriver;
			
			if	(pOut->uHwRes.as.uHera.uPnIp_A.uBase.pPtr != NULL)
				// valid ptr
			{
				m_lSizePnipIrte	= pOut->uHwRes.as.uHera.uPnIp_A.lSize;
				m_pPnIp			= pOut->uHwRes.as.uHera.uPnIp_A.uBase.pPtr;
			}
			else
			{
				m_lSizePnipIrte	= pOut->uHwRes.as.uHera.uPnIp_B.lSize;
				m_pPnIp			= pOut->uHwRes.as.uHera.uPnIp_B.uBase.pPtr;		
			}

			// Inc12 - temporarily - get the bitstream ID direct from register ip_developement from SCRB
			// can be undone when Inc9 support will be removed
			m_lBitStreamId = ((*((volatile UINT32*)(pOut->uHwRes.as.uHera.uApbPer.uScrb.uBase.pPtr)) >> 11) & 0x1F);

			break;
		}
		case ePNDEV_ASIC_KSZ8841:
		case ePNDEV_ASIC_KSZ8842:
		{
			break;
		}
		case ePNDEV_ASIC_AM5728:
		{
			m_lSizeSdramSys	= pOut->uHwRes.as.uAM5728.uEmifSdram.lSizeDriver;
			
			break;
		}
		default:
		{
			// error
			bResult = FALSE;

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid AsicType!"),
										NULL,
										_countof(pOut->sError),
										pOut->sError);

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset device
//************************************************************************

BOOLEAN clsUsedDevice::fnResetDevice(	const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN					bResult		= FALSE;
uPNDEV_RESET_DEVICE_IN	uResetDeviceIn;
uPNDEV_RESET_DEVICE_OUT	uResetDeviceOut;

	// set InputParameter
	{
		memset(	&uResetDeviceIn,
				0,
				sizeof(uPNDEV_RESET_DEVICE_IN));

		// set internal parameter for SYS
		//	- use 64Bit value because DriverHandle may be a 64Bit handle!
		uResetDeviceIn.uInternal.uAppSys.lUint64 = m_uAppSys.lUint64;
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
										IOCTL_RESET_DEVICE,
										sizeof(uPNDEV_RESET_DEVICE_IN),
										sizeof(uPNDEV_RESET_DEVICE_OUT),
										&uResetDeviceIn,
										&uResetDeviceOut))
		// error at IoctlResetDevice
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uResetDeviceOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify environment at first OpenDevice
//************************************************************************

BOOLEAN clsUsedDevice::fnVerifyEnvFirstOpen(const BOOLEAN	bVerifyEnvironmentIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(!bVerifyEnvironmentIn)
		// don't verify environment
	{
		// success
		bResult = TRUE;
	}
	else if	(!m_bMasterApp)
			// NonMasterApp
	{
		// success
		bResult = TRUE;
	}
	else if	(m_uHwInfo.eAsic == ePNDEV_ASIC_IX1000)
			// IX1000, don't test Environment
	{
		// success
		bResult = TRUE;
	}
	else if	(	(m_uHwInfo.eAsic == ePNDEV_ASIC_KSZ8841)
			||	(m_uHwInfo.eAsic == ePNDEV_ASIC_KSZ8842))
			// Micrel, don't test Environment
	{
		// success
		bResult = TRUE;
	}
	else
	{
		// Note:
		//	- FatalAsync is simulated at SysThread
		//	- currently only one FatalAsync can be handled simultaneously
/*
		if	(fnSetEventList(ePNDEV_EVENT_CLASS_FATAL_ASYNC,
							ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS,
							lDimErrorIn,
							sErrorOut))
			// simulating FatalAsync ok
*/
		{
			// IntTest desired
			//	- changes ResetState of hardware
			//	- ResetDevice required at end of this test!

			if	(fnVerifyIntHandling(	lDimErrorIn,
										sErrorOut))
				// verifying IntHandling ok
			{
				if	(fnResetDevice(	lDimErrorIn,
									sErrorOut))
					// ResetDevice ok
				{
					// success
					bResult = TRUE;
				}
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify IntHandling
//************************************************************************

BOOLEAN clsUsedDevice::fnVerifyIntHandling(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN					bResult	= FALSE;
ePNDEV_RESULT			eResult	= ePNDEV_FATAL;
uPNDEV_CHANGE_INT_IN	uChangeIntIn;
uPNDEV_CHANGE_INT_OUT	uChangeIntOut;
	// set InputParameter
	{
		memset(	&uChangeIntIn,
				0,
				sizeof(uPNDEV_CHANGE_INT_IN));

		uChangeIntIn.uDevice.hHandle	= this;
		uChangeIntIn.eIntAction			= ePNDEV_INT_ACTION_TEST__ONCE_UISR;
		uChangeIntIn.lPrioThread		= PNDEV_THREAD_PRIO_UNCHANGED;
	}

	// start IntTest
	eResult = fnPnDev_ChangeInt(m_hDriverDll,
								sizeof(uPNDEV_CHANGE_INT_IN),
								sizeof(uPNDEV_CHANGE_INT_OUT),
								&uChangeIntIn,
								&uChangeIntOut);

	
	if	(eResult != ePNDEV_OK)
		// error
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uChangeIntOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	BOOLEAN bResultEvent = FALSE;

		// wait for SingleEvent EventConIntTest
		bResultEvent = fnWaitForEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST,
													lDimErrorIn,
													sErrorOut);

		// set InputParameter
		{
			memset(	&uChangeIntIn,
					0,
					sizeof(uPNDEV_CHANGE_INT_IN));

			uChangeIntIn.uDevice.hHandle	= this;
			uChangeIntIn.eIntAction			= ePNDEV_INT_ACTION_MASK_ALL;
			uChangeIntIn.lPrioThread		= PNDEV_THREAD_PRIO_UNCHANGED;
		}

		// stop IntTest
		eResult = fnPnDev_ChangeInt(m_hDriverDll,
									sizeof(uPNDEV_CHANGE_INT_IN),
									sizeof(uPNDEV_CHANGE_INT_OUT),
									&uChangeIntIn,
									&uChangeIntOut);

		if	(eResult != ePNDEV_OK)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										uChangeIntOut.sError,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			if	(bResultEvent)
				// success
			{
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set EventList
//************************************************************************

BOOLEAN clsUsedDevice::fnSetEventList(	const ePNDEV_EVENT_CLASS	eEventClassIn,
										const UINT32				lEventIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut)
{
BOOLEAN						bResult			= FALSE;
BOOLEAN						bResultEvent	= TRUE;
uPNDEV_SET_EVENT_LIST_IN	uSetEventListIn;
uPNDEV_SET_EVENT_LIST_OUT	uSetEventListOut;

	// set InputParameter
	{
		memset(	&uSetEventListIn,
				0,
				sizeof(uPNDEV_SET_EVENT_LIST_IN));

		// set internal parameter for driver
		//	- use 64Bit value because DriverHandle may be a 64Bit handle!
		uSetEventListIn.uAppSys.lUint64 = m_uAppSys.lUint64;

		switch	(eEventClassIn)
				// EventClass
		{
			case ePNDEV_EVENT_CLASS_FATAL_FW:		{uSetEventListIn.uEvent.lFatalFw	= lEventIn;							break;}
			case ePNDEV_EVENT_CLASS_TEST:			{uSetEventListIn.uEvent.eFatalAsync	= (ePNDEV_FATAL_ASYNC)	lEventIn;	break;}
			case ePNDEV_EVENT_CLASS_FATAL_ASYNC:	{uSetEventListIn.uEvent.eFatalAsync	= (ePNDEV_FATAL_ASYNC)	lEventIn;	break;}
			case ePNDEV_EVENT_CLASS_WARNING:		{uSetEventListIn.uEvent.eWarning	= (ePNDEV_WARNING)		lEventIn;	break;}
			case ePNDEV_EVENT_CLASS_REQ_CLOSE:		{uSetEventListIn.uEvent.bReqClose	= TRUE;								break;}
			case ePNDEV_EVENT_CLASS_DEBUG:			{uSetEventListIn.uEvent.eDebug		= (ePNDEV_DEBUG)		lEventIn;	break;}
			default:
			{
				// error
				bResultEvent = FALSE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid EventClass!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}
	}

	if	(bResultEvent)
		// success
	{
		if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
											IOCTL_SET_EVENT_LIST,
											sizeof(uPNDEV_SET_EVENT_LIST_IN),
											sizeof(uPNDEV_SET_EVENT_LIST_OUT),
											&uSetEventListIn,
											&uSetEventListOut))
			// error at IoctlSetEventList
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): "),
										uSetEventListOut.sError,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
			// success
			bResult = TRUE;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start Fw
//************************************************************************

BOOLEAN clsUsedDevice::fnStartFw(	uPNDEV_OPEN_DEVICE_IN*	pOpenDeviceIn,
									uPNDEV_OPEN_DEVICE_OUT*	pCurDevice,
									const UINT32			lDimErrorIn,
									_TCHAR*					sErrorOut)
{
BOOLEAN					bResult				= FALSE;
UINT32 					lBatHostAddressTmp 	= 0;
uPNDEV_HW_DESC_SDRAM*	pHwResSdram			= NULL;

	// Note:
	//	- BootFw is required for downloading of user firmware
	//	- BootFw is required for service ProgramFlash
	//	- but: some Apps don't provide BootFw


	if	(pOpenDeviceIn->uUserFw.lCtrFw == 0)
		// no UserFw desired
	{
		switch	(pCurDevice->uHwInfo.eAsic)
				// AsicType
		{
			// set DefaultBatAddress
			case ePNDEV_ASIC_ERTEC400:	{lBatHostAddressTmp = PNCORE_AHB_PCI_MASTER3_ERTEC400_BAT;	break;}
			case ePNDEV_ASIC_SOC1:		{lBatHostAddressTmp = PNCORE_AHB_PCI_MASTER3_SOC1_BAT;		break;}
			default:
			{
				break;
			}
		}
	}
	else
	{
		// set UserBatAddress
		lBatHostAddressTmp = pOpenDeviceIn->uUserFw.lBatHostAddress;
	}

	// set uHwRes.as.xx.uPciMaster here
	if	(pCurDevice->uHwInfo.eAsic == ePNDEV_ASIC_SOC1)
		// Soc1
	{
		// set PciMasterPar
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[0].lBarHost		= PNCORE_AHB_PCI_MASTER1_SOC1_BAR;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[1].lBarHost 		= PNCORE_AHB_PCI_MASTER2_SOC1_BAR;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[2].lBarHost 		= PNCORE_AHB_PCI_MASTER3_SOC1_BAR;

		pCurDevice->uHwRes.as.uSoc1.uPciMaster[0].lBatPciHost	= PNCORE_AHB_PCI_MASTER1_SOC1_BAT;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[1].lBatPciHost 	= PNCORE_AHB_PCI_MASTER2_SOC1_BAT;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[2].lBatPciHost 	= lBatHostAddressTmp;

		pCurDevice->uHwRes.as.uSoc1.uPciMaster[0].lSizePciHost	= PNCORE_AHB_PCI_MASTER1_SOC1_SIZE;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[1].lSizePciHost 	= PNCORE_AHB_PCI_MASTER2_SOC1_SIZE;
		pCurDevice->uHwRes.as.uSoc1.uPciMaster[2].lSizePciHost 	= PNCORE_AHB_PCI_MASTER3_SOC1_SIZE;
	}
	else if (pCurDevice->uHwInfo.eAsic == ePNDEV_ASIC_ERTEC400)
			// Ertec400
	{
		// set PciMasterPar
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[0].lBarHost		= PNCORE_AHB_PCI_MASTER1_ERTEC400_BAR;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[1].lBarHost 		= PNCORE_AHB_PCI_MASTER2_ERTEC400_BAR;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[2].lBarHost 		= PNCORE_AHB_PCI_MASTER3_ERTEC400_BAR;

		pCurDevice->uHwRes.as.uErtec400.uPciMaster[0].lBatPciHost	= PNCORE_AHB_PCI_MASTER1_ERTEC400_BAT;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[1].lBatPciHost 	= PNCORE_AHB_PCI_MASTER2_ERTEC400_BAT;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[2].lBatPciHost 	= lBatHostAddressTmp;

		pCurDevice->uHwRes.as.uErtec400.uPciMaster[0].lSizePciHost	= PNCORE_AHB_PCI_MASTER1_ERTEC400_SIZE;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[1].lSizePciHost 	= PNCORE_AHB_PCI_MASTER2_ERTEC400_SIZE;
		pCurDevice->uHwRes.as.uErtec400.uPciMaster[2].lSizePciHost 	= PNCORE_AHB_PCI_MASTER3_ERTEC400_SIZE;
	}

	switch	(pCurDevice->uHwInfo.eAsic)
			// AsicType
	{
		case ePNDEV_ASIC_ERTEC400:	{pHwResSdram = &pCurDevice->uHwRes.as.uErtec400.uEmifSdram;	break;}
		case ePNDEV_ASIC_ERTEC200:	{pHwResSdram = &pCurDevice->uHwRes.as.uErtec200.uEmifSdram;	break;}
		case ePNDEV_ASIC_SOC1:		{pHwResSdram = &pCurDevice->uHwRes.as.uSoc1.uDdr2Sdram;		break;}
		case ePNDEV_ASIC_ERTEC200P:	{pHwResSdram = &pCurDevice->uHwRes.as.uErtec200P.uEmcSdram;	break;}
		case ePNDEV_ASIC_AM5728:	{pHwResSdram = &pCurDevice->uHwRes.as.uAM5728.uEmifSdram;	break; }
		case ePNDEV_ASIC_HERA:		{pHwResSdram = &pCurDevice->uHwRes.as.uHera.uDdr3Sdram;		break;}
		default:
		{
			break;
		}
	}

	if	(pHwResSdram == NULL)
		// no sdram available
	{
		if	(	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_I210)
			||	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_I210SFP)
			||	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_I82574)
			||	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_IX1000)
			||	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
			||	(pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_MICREL_KSZ8842))
			// standard board
		{
			if 	(	(pOpenDeviceIn->uUserFw.lCtrFw != 0)
				||	(pOpenDeviceIn->uBootFw.eSrc != ePNDEV_FW_SRC_INVALID))
				// Fw download
			{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Board doesn't support download of BootFw or UserFw!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
		else
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid pointer pHwResSdram!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
	}
	else
	{
		if(     pOpenDeviceIn->uUserFw.uFw[0].eFwType == ePNDEV_FW_TYPE_ADONIS
            ||  pOpenDeviceIn->uUserFw.uFw[0].eFwType == ePNDEV_FW_TYPE_BOOT
			||  pOpenDeviceIn->uUserFw.uFw[0].eFwType == ePNDEV_FW_TYPE_INVALID)
		{
		if	(pOpenDeviceIn->uBootFw.eSrc == ePNDEV_FW_SRC_INVALID
			&&	pCurDevice->uHwInfo.eBoard != ePNDEV_BOARD_TI_AM5728)  // We are going to make a workaround for TI AM5728 because we have no BootFW)
			// BootFw not available
		{
			if	(pOpenDeviceIn->uUserFw.lCtrFw != 0)
				// UserFw desired
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Loading of UserFw requires the BootFw!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
		else
		{
			// workaround for TI AM5728 because there is no BootFW available
			if (pCurDevice->uHwInfo.eBoard == ePNDEV_BOARD_TI_AM5728)
			{
				if (pOpenDeviceIn->uUserFw.lCtrFw != 0)
					// UserFw desired
				{
				uPNCORE_PCI_HOST_PAR* pPciMasterPar = NULL;

					// we just want to start the UserFW

					if (fnStartUserFw(	&pOpenDeviceIn->uUserFw,
										&pCurDevice->uHwInfo,
										lDimErrorIn,
										pPciMasterPar,
										pHwResSdram,
										sErrorOut))
						// starting UserFw ok
					{
						// success
						bResult = TRUE;
					}
				}
				else
				{
					bResult = TRUE;
				}
			}
			else
			{
				// this is the usual part were the BootFW will be started first
				// for every board except the AM5728
			
				if	(fnStartBootFw(	&pOpenDeviceIn->uBootFw,
									&pCurDevice->uHwInfo,
									pHwResSdram->lSizeDriver,
									lDimErrorIn,
									sErrorOut))
					// starting BootFw ok
				{

					if	(pOpenDeviceIn->uUserFw.lCtrFw == 0)
						// no UserFw desired
					{
						// success
						bResult = TRUE;
					}
					else
					{
					uPNCORE_PCI_HOST_PAR* pPciMasterPar = NULL;

						switch	(pCurDevice->uHwInfo.eAsic)
								// AsicType
						{
							case ePNDEV_ASIC_ERTEC400:	{pPciMasterPar = &pCurDevice->uHwRes.as.uErtec400.uPciMaster[0];	break;}
							case ePNDEV_ASIC_SOC1:		{pPciMasterPar = &pCurDevice->uHwRes.as.uSoc1.uPciMaster[0];		break;}
							default:
							{
								break;
							}
						}

						if	(fnStartUserFw(	&pOpenDeviceIn->uUserFw,
											&pCurDevice->uHwInfo,
											lDimErrorIn,
											pPciMasterPar,
											pHwResSdram,
											sErrorOut))
							// starting UserFw ok
						{
							// success
							bResult = TRUE;
						}
					}
				}
			}
		}
	}

		#if !defined (PNDEV_OS_ADONIS)
		else if (pOpenDeviceIn->uUserFw.uFw[0].eFwType == ePNDEV_FW_TYPE_LINUX)
		{
		uPNCORE_PCI_HOST_PAR* pPciMasterPar = NULL;

			switch	(pCurDevice->uHwInfo.eAsic)
					// AsicType
			{
				case ePNDEV_ASIC_ERTEC400:	{pPciMasterPar = &pCurDevice->uHwRes.as.uErtec400.uPciMaster[0];	break;}
				case ePNDEV_ASIC_SOC1:		{pPciMasterPar = &pCurDevice->uHwRes.as.uSoc1.uPciMaster[0];		break;}
				default:
				{
					break;
				}
			}

			
			if	(fnStartUserFwLinux(	&pOpenDeviceIn->uUserFw,
										&pCurDevice->uHwInfo,
										lDimErrorIn,
										pPciMasterPar,
										pHwResSdram,
										sErrorOut))
				// starting UserFw ok
			{
				// success
				bResult = TRUE;
			}			
		}
		#endif
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start BootFw
//************************************************************************

BOOLEAN clsUsedDevice::fnStartBootFw(	uPNDEV_FW_DESC*	pBootFwIn,
										uPNDEV_HW_INFO*	pHwInfoIn,
										const UINT32	lSizeDriverSdramIn,
										const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(!m_uHwInfo.bAsicCoreExist)
		// no AsicCore exist
	{
		// success
		bResult = TRUE;
	}	
	else
	{
		if	(fnCopyFwToAsicSdram(	lSizeDriverSdramIn,
									pBootFwIn,
									pHwInfoIn,
									FALSE,					// BootFw
									lDimErrorIn,
									sErrorOut))
			// copying BootFw to AsicSdram ok
		{
		uPNDEV_START_BOOT_FW_IN		uStartBootFwIn;
		uPNDEV_START_BOOT_FW_OUT	uStartBootFwOut;

			// set InputParameter
			{
				memset(	&uStartBootFwIn,
						0,
						sizeof(uPNDEV_START_BOOT_FW_IN));
			}

			if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
												IOCTL_START_BOOT_FW,
												sizeof(uPNDEV_START_BOOT_FW_IN),
												sizeof(uPNDEV_START_BOOT_FW_OUT),
												&uStartBootFwIn,
												&uStartBootFwOut))
				// error at IoctlStartBootFw
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											uStartBootFwOut.sError,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start UserFw
//************************************************************************

BOOLEAN clsUsedDevice::fnStartUserFw(	uPNDEV_USER_FW*			pUserFwIn,
										uPNDEV_HW_INFO*			pHwInfoIn,
										const UINT32			lDimErrorIn,
										uPNCORE_PCI_HOST_PAR*	pPciMasterParIn,
										uPNDEV_HW_DESC_SDRAM*	pHwResSdramInOut,
										_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(!m_uHwInfo.bAsicCoreExist)
		// no AsicCore exist
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Downloading of UserFw not possible because there is no AsicCore!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else if	(pUserFwIn->lCtrFw > 2)
			// invalid FwCtr
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid value of lCtrFw!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	BOOLEAN bResultEvent = TRUE;

		if	(pUserFwIn->bDebugMode)
			// DebugMode
		{
			// set event 'SetSemaphoreByDebugger'
			bResultEvent = fnSetEventList(	ePNDEV_EVENT_CLASS_DEBUG,
											ePNDEV_DEBUG__SET_SEMAPHORE_BY_DEBUGGER,
											lDimErrorIn,
											sErrorOut);
		}

		if	(bResultEvent)
			// success
		{
		UINT32 i = 0;

			for	(i = 0; i < pUserFwIn->lCtrFw; i++)
				// all ElfFw
			{
			UINT32 lOffsetFwBuf = 0;

				// preset
				bResult = FALSE;

				if	(fnCopyFwToAsicSdram(	pHwResSdramInOut->lSizeDriver,
											&pUserFwIn->uFw[i],
											pHwInfoIn,
											TRUE,					// UserFw
											lDimErrorIn,
											sErrorOut))
					// copying UserFw to AsicSdram ok
				{
				uPNDEV_START_USER_FW_IN		uStartUserFwIn;
				uPNDEV_START_USER_FW_OUT	uStartUserFwOut;

					// set InputParameter
					{
						memset(	&uStartUserFwIn,
								0,
								sizeof(uPNDEV_START_USER_FW_IN));

						// set internal parameter for driver
						//	- use 64Bit value because DriverHandle may be a 64Bit handle!
						uStartUserFwIn.uAppSys.lUint64 = m_uAppSys.lUint64;

						uStartUserFwIn.lOffsetFwBuf						= lOffsetFwBuf;
						uStartUserFwIn.lIndexFw							= i;
						uStartUserFwIn.lIndexFwMax						= (pUserFwIn->lCtrFw - 1);
						uStartUserFwIn.bDebugMode						= pUserFwIn->bDebugMode;
						uStartUserFwIn.bShmDebug						= pUserFwIn->bShmDebug;
						uStartUserFwIn.bShmHaltOnStartup				= pUserFwIn->bShmHaltOnStartup;
						uStartUserFwIn.lTimeoutFwLoad_10msec			= pUserFwIn->lTimeoutFwLoad_10msec;
						uStartUserFwIn.lTimeoutFwExe_10msec				= pUserFwIn->lTimeoutFwExe_10msec;
						uStartUserFwIn.lTimeoutFwStartupStd_10msec		= pUserFwIn->lTimeoutFwStartupStd_10msec;
						uStartUserFwIn.lTimeoutFwStartupDebug_10msec	= pUserFwIn->lTimeoutFwStartupDebug_10msec;
						uStartUserFwIn.lTimeoutDebugger_10msec			= pUserFwIn->lTimeoutDebugger_10msec;
						uStartUserFwIn.lTimeWaitDebugger_10msec			= pUserFwIn->lTimeWaitDebugger_10msec;
						uStartUserFwIn.lTimeStartupSignOfLife_10msec	= pUserFwIn->lTimeStartupSignOfLife_10msec;
						uStartUserFwIn.lTimeUpdateSignOfLife_10msec		= pUserFwIn->lTimeUpdateSignOfLife_10msec;

						if	(uStartUserFwIn.lIndexFw == uStartUserFwIn.lIndexFwMax)
							// last ElfFw
						{
						UINT32 j = 0;

							// copy BAT for 3.PciMaster
							uStartUserFwIn.lBatHostAddress = pUserFwIn->lBatHostAddress;

							if	(pPciMasterParIn != NULL)
								// valid pointer
							{
								// copy PciMasterPar
								uStartUserFwIn.uPciMaster[0] = *pPciMasterParIn++;
								uStartUserFwIn.uPciMaster[1] = *pPciMasterParIn++;
								uStartUserFwIn.uPciMaster[2] = *pPciMasterParIn;
							}

							for	(j = 0; j < PNCORE_DIM_ARRAY_USER_DATA; j++)
								// dim array
							{
								// copy FwUserData
								uStartUserFwIn.lArrayUserData[j] = pUserFwIn->lArrayUserData[j];
							}

							// provide SdramPar for updating by Fw settings
							*((uPNDEV_HW_DESC_SDRAM*) &uStartUserFwIn.uAsicSdram_Org) = *pHwResSdramInOut;
						}
					}

					if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
														IOCTL_START_USER_FW,
														sizeof(uPNDEV_START_USER_FW_IN),
														sizeof(uPNDEV_START_USER_FW_OUT),
														&uStartUserFwIn,
														&uStartUserFwOut))
						// error at IoctlStartUserFw
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): "),
													uStartUserFwOut.sError,
													lDimErrorIn,
													sErrorOut);
					}
					else
					{
					BOOLEAN bIsUserFwReady 					= !uStartUserFwIn.bShmDebug;
					UINT32	lTimeoutFwStartupDebug_10msec	= uStartUserFwIn.lTimeoutFwStartupDebug_10msec;
						
						if(	!bIsUserFwReady)
						{
						uPNDEV_IS_USER_FW_READY_IN	uIsUserFwReadyIn;
						uPNDEV_IS_USER_FW_READY_OUT	uIsUserFwReadyOut;

							// set internal parameter for driver
							//	- use 64Bit value because DriverHandle may be a 64Bit handle!
							uIsUserFwReadyIn.uAppSys.lUint64 = m_uAppSys.lUint64;
							
							if	(lTimeoutFwStartupDebug_10msec == 0)
								// default value desired
							{
								// 300sec (=300.000msec=30000*10msec)
								lTimeoutFwStartupDebug_10msec = 30000;
							}
							
							for	(i = 0; i < lTimeoutFwStartupDebug_10msec; i++)
								// MaxTime
							{
								if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
																	IOCTL_IS_USER_FW_READY,
																	sizeof(uPNDEV_IS_USER_FW_READY_IN),
																	sizeof(uPNDEV_IS_USER_FW_READY_OUT),
																	&uIsUserFwReadyIn,
																	&uIsUserFwReadyOut))
									// error at IoctlStartUserFw
								{
									// set ErrorString
									clsString::fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Wait for ready user fw was canceled!"),
																uIsUserFwReadyOut.sError,
																lDimErrorIn,
																sErrorOut);
									break;
								}

								if	(!uIsUserFwReadyOut.bIsUserFwReady)
									// Fw not ready
								{
									// try again in 10msec
									Sleep(10);
								}
								else
								{
									// success
									bIsUserFwReady = TRUE;

									// leave loop
									break;
								}
							}
						}
						if (bIsUserFwReady)
						{
							// success
							bResult = TRUE;

							if	(uStartUserFwIn.lIndexFw == uStartUserFwIn.lIndexFwMax)
								// last ElfFw
							{
								// return SdramPar
								*pHwResSdramInOut = *((uPNDEV_HW_DESC_SDRAM*) &uStartUserFwOut.uAsicSdram_Updated);
							}
						}
					}
				}

				if	(!bResult)
					// error
				{
					// leave loop
					break;
				}
			}
		}
	}

	return(bResult);
}

#if !defined (PNDEV_OS_ADONIS)
//************************************************************************
//  D e s c r i p t i o n :
//
//  start Linux UserFw
//************************************************************************

BOOLEAN clsUsedDevice::fnStartUserFwLinux(	uPNDEV_USER_FW*			pUserFwIn,
											uPNDEV_HW_INFO*			pHwInfoIn,
											const UINT32			lDimErrorIn,
											uPNCORE_PCI_HOST_PAR*	pPciMasterParIn,
											uPNDEV_HW_DESC_SDRAM*	pHwResSdramInOut,
											_TCHAR*					sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(!m_uHwInfo.bAsicCoreExist)
		// no AsicCore exist
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Downloading of UserFw not possible because there is no AsicCore!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else if	(pUserFwIn->lCtrFw > 1)
			// invalid FwCtr
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid value of lCtrFw! For Linux only one FW file is supported"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	uPNDEV_START_USER_FW_LINUX_IN	uStartUserFwLinuxIn;
	uPNDEV_START_USER_FW_LINUX_OUT	uStartUserFwLinuxOut;

		if (fnCopyFwToAsicSdram(pHwResSdramInOut->lSizeDriver,
								&pUserFwIn->uFw[0],
								pHwInfoIn,
								TRUE,					// UserFw
								lDimErrorIn,
								sErrorOut))
		// copying UserFw to AsicSdram ok
		{
			// set InputParameter
			{
				memset(	&uStartUserFwLinuxIn,
						0,
						sizeof(uPNDEV_START_USER_FW_LINUX_IN));

				// set internal parameter for driver
				//	- use 64Bit value because DriverHandle may be a 64Bit handle!
				uStartUserFwLinuxIn.uAppSys.lUint64						= m_uAppSys.lUint64;

				uStartUserFwLinuxIn.lOffsetFwBuf						= 0;
				uStartUserFwLinuxIn.lIndexFw							= 0;
				uStartUserFwLinuxIn.lIndexFwMax							= (pUserFwIn->lCtrFw - 1);
				uStartUserFwLinuxIn.bDebugMode							= pUserFwIn->bDebugMode;
				uStartUserFwLinuxIn.bShmDebug							= pUserFwIn->bShmDebug;
				uStartUserFwLinuxIn.bShmHaltOnStartup					= pUserFwIn->bShmHaltOnStartup;
				uStartUserFwLinuxIn.lTimeoutFwLoad_10msec				= pUserFwIn->lTimeoutFwLoad_10msec;
				uStartUserFwLinuxIn.lTimeoutFwExe_10msec				= pUserFwIn->lTimeoutFwExe_10msec;
				uStartUserFwLinuxIn.lTimeoutFwStartupStd_10msec			= pUserFwIn->lTimeoutFwStartupStd_10msec;
				uStartUserFwLinuxIn.lTimeoutFwStartupDebug_10msec		= pUserFwIn->lTimeoutFwStartupDebug_10msec;
				uStartUserFwLinuxIn.lTimeoutDebugger_10msec				= pUserFwIn->lTimeoutDebugger_10msec;
				uStartUserFwLinuxIn.lTimeWaitDebugger_10msec			= pUserFwIn->lTimeWaitDebugger_10msec;
				uStartUserFwLinuxIn.lTimeStartupSignOfLife_10msec		= pUserFwIn->lTimeStartupSignOfLife_10msec;
				uStartUserFwLinuxIn.lTimeUpdateSignOfLife_10msec		= pUserFwIn->lTimeUpdateSignOfLife_10msec;

				if	(uStartUserFwLinuxIn.lIndexFw == uStartUserFwLinuxIn.lIndexFwMax)
					// last ElfFw
				{
				UINT32 j = 0;

					// copy BAT for 3.PciMaster
					uStartUserFwLinuxIn.lBatHostAddress = pUserFwIn->lBatHostAddress;

					if	(pPciMasterParIn != NULL)
						// valid pointer
					{
						// copy PciMasterPar
						uStartUserFwLinuxIn.uPciMaster[0] = *pPciMasterParIn++;
						uStartUserFwLinuxIn.uPciMaster[1] = *pPciMasterParIn++;
						uStartUserFwLinuxIn.uPciMaster[2] = *pPciMasterParIn;
					}

					for	(j = 0; j < PNCORE_DIM_ARRAY_USER_DATA; j++)
						// dim array
					{
						// copy FwUserData
						uStartUserFwLinuxIn.lArrayUserData[j] = pUserFwIn->lArrayUserData[j];
					}

					// provide SdramPar for updating by Fw settings
					*((uPNDEV_HW_DESC_SDRAM*) &uStartUserFwLinuxIn.uAsicSdram_Org) = *pHwResSdramInOut;
				}
			}

			if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
												IOCTL_START_USER_FW_LINUX,
												sizeof(uPNDEV_START_USER_FW_LINUX_IN),
												sizeof(uPNDEV_START_USER_FW_LINUX_OUT),
												&uStartUserFwLinuxIn,
												&uStartUserFwLinuxOut))
				// error at IoctlStartBootFw
			{
				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): "),
											uStartUserFwLinuxOut.sError,
											lDimErrorIn,
											sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;

				if	(uStartUserFwLinuxIn.lIndexFw == uStartUserFwLinuxIn.lIndexFwMax)
					// last ElfFw
				{
					// return SdramPar
					*pHwResSdramInOut = *((uPNDEV_HW_DESC_SDRAM*) &uStartUserFwLinuxOut.uAsicSdram_Updated);
				}					
			}
		}		
	}

	return(bResult);
}
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  copy firmware to AsicSdram
//************************************************************************

BOOLEAN clsUsedDevice::fnCopyFwToAsicSdram(	const UINT32	lSizeDriverSdramIn,
											uPNDEV_FW_DESC*	pFwDescIn,
											uPNDEV_HW_INFO*	pHwInfoIn,
											const BOOLEAN	bUserFwIn,
											const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN	bResult				= FALSE;
HANDLE	hFileFw				= NULL;
UINT32	lSizeFw				= 0;
UINT32	lSizeBuf_CopyData	= 0;

	if	(pFwDescIn->eSrc == ePNDEV_FW_SRC_BUF)
		// Fw by HostBuf
	{
		lSizeFw = pFwDescIn->as.uBuf.lSize;
	}
	else	// Fw by file
	{
		if	(clsFile::fnOpenFileForRead(pFwDescIn->as.uFile.sPath,
										lDimErrorIn,
										&hFileFw,
										sErrorOut))
			// opening FwFile for reading ok
		{
			if	(clsFile::fnGetFileSize(hFileFw,
										lDimErrorIn,
										&lSizeFw,
										sErrorOut))
				// getting size of FwFile ok
			{
			}
		}
	}

	if	(lSizeFw != 0)
		// success
	{
	UINT8*	pBufDriver	= NULL;
	BOOLEAN	bResultBuf	= FALSE;

		// compute BufSize of service CopyData
		lSizeBuf_CopyData = sizeof(uPNDEV_COPY_DATA_OUT) + lSizeFw;

		// allocate OutputBuffer
		pBufDriver = (UINT8*) new UINT8[lSizeBuf_CopyData];

		if	(pBufDriver == NULL)
			// error
		{
			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Insufficient memory for buffer of service CopyData!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
		UINT8* pHostData_CopyData = NULL;

			memset(	pBufDriver,
					0,
					lSizeBuf_CopyData);

			pHostData_CopyData = pBufDriver + sizeof(uPNDEV_COPY_DATA_OUT);

			if	(pFwDescIn->eSrc == ePNDEV_FW_SRC_BUF)
				// Fw by HostBuf
			{
				// copy Fw to OutputBuffer
				memcpy_s(	pHostData_CopyData,
							lSizeFw,
							(const void*) pFwDescIn->as.uBuf.uBase.pPtr,
							lSizeFw);

				// success
				bResultBuf = TRUE;
			}
			else	// Fw by file
			{
				if	(clsFile::fnReadFile(	hFileFw,
											pHostData_CopyData,
											lSizeFw,
											lDimErrorIn,
											sErrorOut))
					// reading from FwFile ok
				{
					// success
					bResultBuf = TRUE;
				}
			}
		}

		if	(bResultBuf)
			// success
		{
		BOOLEAN				bResultSdram	= FALSE;
		ePNDEV_RESULT		eResult			= ePNDEV_FATAL;
		uPNDEV_COPY_DATA_IN	uCopyDataIn;

			// Note:
			//	- UserFw:		will be copied to start of SharedAsicSdram_Direct
			//	- BootFw-combi:	consists of 2 parts:
			//					- first 1kB:	place independent JmpBootFw,
			//									will be temporarily copied to start of SharedAsicSdram_Direct (afterwards to specific PBL location by driver)
			//					- rest:			BootFw,
			//									will be copied to start of SharedAsicSdram_Indirect0

			// set InputParameter
			{
				memset(	&uCopyDataIn,
						0,
						sizeof(uPNDEV_COPY_DATA_IN));

				uCopyDataIn.uDevice.hHandle	= this;
				uCopyDataIn.eHwRes			= ePNDEV_COPY_HW_RES_SDRAM;

				if	(bUserFwIn)
					// UserFw
				{
				UINT32 lSizeMax = 0;

					lSizeMax = lSizeDriverSdramIn - PNCORE_SHARED_ASIC_SDRAM__SIZE_RESERVED;

					if	(lSizeFw > lSizeMax)
						// UserFw too large for SharedAsicSdram_Direct/Indirect2
					{
					_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

						// set ErrorDetail
						_stprintf_s(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT(" (Required size=%u, SDRAM-Direct/Indirect2 size=%u)"),
									lSizeFw,
									lSizeMax);

						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): UserFw too large!"),
													sErrorDetail,
													lDimErrorIn,
													sErrorOut);
					}
					else
					{
						// success
						bResultSdram = TRUE;

						// set WriteBlocks
						{
							uCopyDataIn.uArrayBlockWrite[0].lOffsetHostDataBuf	= 0;
							uCopyDataIn.uArrayBlockWrite[0].lSize				= lSizeFw;

							if(pFwDescIn->eFwType == ePNDEV_FW_TYPE_LINUX)
								uCopyDataIn.uArrayBlockWrite[0].lOffsetAsic = PNCORE_SHARED_ASIC_SDRAM__OFFSET_LINUX_FW;
							else
							uCopyDataIn.uArrayBlockWrite[0].lOffsetAsic			= PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
						}
					}
				}
				else	// BootFw
				{
				UINT32	lSizeJmpBootFw	= 0;
				UINT32	lSizeBootFw		= 0;
	
					if	(pHwInfoIn->eAsic == ePNDEV_ASIC_HERA)
						// Hera
					{
						lSizeBootFw		= lSizeFw;
					}
					else
					{
						lSizeJmpBootFw	= 1024;
						lSizeBootFw		= lSizeFw - 1024;
					}
	
					if	(lSizeBootFw > PNCORE_SHARED_ASIC_SDRAM__SIZE_INDIRECT0)
						// BootFw too large for SharedAsicSdram_Indirect0
					{
						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): BootFw too large for SharedAsicSdram_Indirect0!"),
													NULL,
													lDimErrorIn,
													sErrorOut);
					}
					else
					{
						// success
						bResultSdram = TRUE;
	
						// set WriteBlocks
						{
							if	(lSizeJmpBootFw != 0)
								// JmpBootFw exist
							{
								uCopyDataIn.uArrayBlockWrite[0].lOffsetHostDataBuf	= 0;
								uCopyDataIn.uArrayBlockWrite[0].lOffsetAsic			= PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
								uCopyDataIn.uArrayBlockWrite[0].lSize				= lSizeJmpBootFw;
	
								uCopyDataIn.uArrayBlockWrite[1].lOffsetHostDataBuf	= lSizeJmpBootFw;
								uCopyDataIn.uArrayBlockWrite[1].lOffsetAsic			= PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0;
								uCopyDataIn.uArrayBlockWrite[1].lSize				= lSizeBootFw;
							}
							else
							{
								uCopyDataIn.uArrayBlockWrite[0].lOffsetHostDataBuf	= 0;
								uCopyDataIn.uArrayBlockWrite[0].lOffsetAsic			= PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0;
								uCopyDataIn.uArrayBlockWrite[0].lSize				= lSizeFw;
							}
						}
					}
				}
			}

			if	(bResultSdram)
				// success
			{
				// copy data between HostBuffer and external AsicSdram
				eResult = fnPnDev_CopyData(	m_hDriverDll,
											sizeof(uPNDEV_COPY_DATA_IN),
											lSizeBuf_CopyData,
											&uCopyDataIn,
											(uPNDEV_COPY_DATA_OUT*) pBufDriver);

				if	(eResult != ePNDEV_OK)
					// error
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												((uPNDEV_COPY_DATA_OUT*) pBufDriver)->sError,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;
				}
			}
		}

		if	(pBufDriver != NULL)
			// buffer allocated
		{
			// free buffer
			delete[] pBufDriver;
			pBufDriver = NULL;
		}
	}

	if	(hFileFw != NULL)
		// file opened
	{
		// close file
		clsFile::fnCloseFile(hFileFw);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  lock MultiApp
//************************************************************************

BOOLEAN clsUsedDevice::fnLockMultiApp(	const BOOLEAN	bLockMultiAppIn,
										const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN						bResult = FALSE;
uPNDEV_CHANGE_APP_LOCK_IN	uChangeAppLockIn;
uPNDEV_CHANGE_APP_LOCK_OUT	uChangeAppLockOut;

	// set InputParameter
	{
		memset(	&uChangeAppLockIn,
				0,
				sizeof(uPNDEV_CHANGE_APP_LOCK_IN));

		// set internal parameter for driver
		//	- use 64Bit value because DriverHandle may be a 64Bit handle!
		uChangeAppLockIn.uAppSys.lUint64	= m_uAppSys.lUint64;
		uChangeAppLockIn.bLockMultiApp		= bLockMultiAppIn;
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
										IOCTL_CHANGE_APP_LOCK,
										sizeof(uPNDEV_CHANGE_APP_LOCK_IN),
										sizeof(uPNDEV_CHANGE_APP_LOCK_OUT),
										&uChangeAppLockIn,
										&uChangeAppLockOut))
		// error at IoctlChangeAppLock
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uChangeAppLockOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  notify DllState
//************************************************************************

BOOLEAN	clsUsedDevice::fnNotifyDllState(const BOOLEAN	bReadyIn,
										const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN						bResult = FALSE;
uPNDEV_NOTIFY_DLL_STATE_IN	uNotifyDllStateIn;
uPNDEV_NOTIFY_DLL_STATE_OUT	uNotifyDllStateOut;

	// set InputParameter
	{
		memset(	&uNotifyDllStateIn,
				0,
				sizeof(uPNDEV_NOTIFY_DLL_STATE_IN));

		// set internal parameter for driver
		uNotifyDllStateIn.bReady = bReadyIn;
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
										IOCTL_NOTIFY_DLL_STATE,
										sizeof(uPNDEV_NOTIFY_DLL_STATE_IN),
										sizeof(uPNDEV_NOTIFY_DLL_STATE_OUT),
										&uNotifyDllStateIn,
										&uNotifyDllStateOut))
		// error at IoctlNotifyDllState
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uNotifyDllStateOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  alloc EventId
//************************************************************************

BOOLEAN	clsUsedDevice::fnAllocEventId(	const UINT32	lDimErrorIn,
										UINT32*			pEventIdOut,
										_TCHAR*			sErrorOut)
{
BOOLEAN						bResult = FALSE;
uPNDEV_ALLOC_EVENT_ID_IN	uAllocEventIdIn;
uPNDEV_ALLOC_EVENT_ID_OUT	uAllocEventIdOut;

	// preset OutputParameter
	*pEventIdOut = 0;

	// set InputParameter
	{
		memset(	&uAllocEventIdIn,
				0,
				sizeof(uPNDEV_ALLOC_EVENT_ID_IN));
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	NULL,								// DriverService (not a DeviceService) -> no DeviceHandle!
										IOCTL_ALLOC_EVENT_ID,
										sizeof(uPNDEV_ALLOC_EVENT_ID_IN),
										sizeof(uPNDEV_ALLOC_EVENT_ID_OUT),
										&uAllocEventIdIn,
										&uAllocEventIdOut))
		// error at IoctlAllocEventId
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uAllocEventIdOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;

		// return
		*pEventIdOut = uAllocEventIdOut.lEventId;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  free EventId
//************************************************************************

BOOLEAN	clsUsedDevice::fnFreeEventId(	const UINT32	lEventIdIn,
										const UINT32	lDimErrorIn,
										_TCHAR*			sErrorOut)
{
BOOLEAN						bResult = FALSE;
uPNDEV_FREE_EVENT_ID_IN		uFreeEventIdIn;
uPNDEV_FREE_EVENT_ID_OUT	uFreeEventIdOut;

	// set InputParameter
	{
		memset(	&uFreeEventIdIn,
				0,
				sizeof(uPNDEV_FREE_EVENT_ID_IN));

		uFreeEventIdIn.lEventId = lEventIdIn;
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	NULL,								// DriverService (not a DeviceService) -> no DeviceHandle!
										IOCTL_FREE_EVENT_ID,
										sizeof(uPNDEV_FREE_EVENT_ID_IN),
										sizeof(uPNDEV_FREE_EVENT_ID_OUT),
										&uFreeEventIdIn,
										&uFreeEventIdOut))
		// error at IoctlFreeEventId
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uFreeEventIdOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  stop IntTest
//************************************************************************

void clsUsedDevice::fnStopIntTest(void)
{
	switch	(m_eIcu)
			// IcuType
	{
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// ack IRTE-IrqNrt-HP
			WRITE_REG_ICU(	IRTE_REG__ICU_ACK_NRT,
							(UINT32) IRTE_IRQ_NRT__HP);

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// ack PNIP-ICU-Event63
			WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_ACK_MID,
							(UINT32) PNDEV_UINT32_SET_BIT_31);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// clear PCI-ICU-Event31
			WRITE_REG_ICU(SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A, SOC1_PCI_ICU_VECTOR_MUX_A__31);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			// clear ARM-ICU-Event95 (set by software -> cannot be cleared by IRCLVEC (look to FSpec))
			WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__SWIIR2,
							(READ_REG_ICU(ERTEC200P_ARM_ICU_REG__SWIIR2) & PNDEV_UINT32_CLR_BIT_31));

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// ack PERIF-ICU-Event63
			WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,
							(UINT32) PNDEV_UINT32_SET_BIT_31);

			break;
		}
		case ePNDEV_ICU_INTEL:	// Intel
		{
			// read ICR clears Event31 in InterruptCauseRegister
			WRITE_REG_ICU(	BOARD_INTEL__ICR,
							(UINT32) PNDEV_UINT32_SET_BIT_31);

			break;
		}
		case ePNDEV_ICU_IX1000:	// IX1000
		{
			// writing bit in status register clears interrupt
			WRITE_REG_ICU(	BOARD_IX1000__STATUS,
							(UINT32) PNDEV_UINT32_SET_BIT_31);

			break;
		}
		case ePNDEV_ICU_MICREL:	// Micrel
		{
			// read ICR clears Event31 in InterruptCauseRegister
			WRITE_REG_ICU(	BOARD_MICREL_KSZ88XX__INTST,
							(UINT32) PNDEV_UINT32_SET_BIT_31);

			break;
		}
		case ePNDEV_ICU_HERA:	// Hera
		{	
			
			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  mask all interrupts at ICU
//************************************************************************

void clsUsedDevice::fnMaskIcu_All(BOOLEAN bInitIn)
{
UINT32 lTmp = 0;
UNREFERENCED_PARAMETER(lTmp);

	// if no TopLevel-ICU exist it is implemented by software
	m_bIntEnabledIrtePnipIntel = FALSE;

	// update Masks
	m_lEventEnabled0	= 0;
	m_lEventEnabled1	= 0;
	m_lEventEnabled2	= 0;

	switch	(m_eIcu)
			// IcuType
	{
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// mask all interrupts of IRTE-ICU
			WRITE_REG_ICU(	IRTE_REG__ICU_IRQ1_MASK_IRT,
							0x00000000);
			WRITE_REG_ICU(	IRTE_REG__ICU_IRQ1_MASK_NRT,
							0x00000000);

			// ensure updating of IrqLevel
			WRITE_REG_ICU(	IRTE_REG__ICU_IRQ1_EOI,
							1);

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(IRTE_REG__ICU_IRQ1_EOI);

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// mask all interrupts of PNIP-ICU2
			WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_MASK_LOW,
							0xFFFFffff);
			WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_MASK_MID,
							0xFFFFffff);
			WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_MASK_HIGH,
							0xFFFFffff);

			// ensure updating of IrqLevel
			WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_EOI,
							1);

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(PNIP_REG__ICU2_IRQ0_EOI);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// disable all events
			WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKALL_A,
							1);

			if	(bInitIn)
				// initialization
			{
				// mask all events of PCI-ICU-MuxA
				WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKREG_A,
								0xFFFFffff);
			}

			// clear ISREG_A
			WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_END_A,
							1);

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(SOC1_PCI_ICU_REG__IRQ_END_A);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			// disable all events
			WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASKALL,
							1);

			if	(bInitIn)
				// initialization
			{
				// mask all events of ARM-ICU-Event31:0
				WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK0,
								0xFFFFffff);

				// mask all events of ARM-ICU-Event63:32
				WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK1,
								0xFFFFffff);

				// mask all events of ARM-ICU-Event95:64
				WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK2,
								0xFFFFffff);
			}

			// Note:
			//	- setting of EOI not necessary because EOI resets only priority scheduler
			//	- clearing of ISREG reactivates interrupts with same or lower priority

			// ensure updating of IrqLevel
			//WRITE_REG_UINT32(	ERTEC200P_ARM_ICU_REG__EOI,
			//					1);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// mask all interrupts of PERIF-ICU
			WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_MASK_LOW,
							0xFFFFffff);
			WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH,
							0xFFFFffff);

			// update IrqLevel
			//	- masking of interrupts does not change IrqLevel
			//	- if IrqLevel=active it will remain active -> no new falling edge!
			WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_EOI,
							1);

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(ERTEC200P_PERIF_REG__HOST_IRQ_EOI);

			break;
		}
		case ePNDEV_ICU_INTEL:	// I210 + I210SFP + I82574
		{
			// mask all interrupts
			WRITE_REG_ICU(	BOARD_INTEL__IMC,
							0xFFFFFFFF);

			if (bInitIn)
			{
				// clear all pendings interrupts (write 1 to corresponding bit)
				WRITE_REG_ICU(	BOARD_INTEL__ICR,
								0xFFFFFFFF);
			}

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(BOARD_INTEL__IMS);

			break;
		}
		case ePNDEV_ICU_IX1000:	// IX1000
		{
			// mask all interrupts
			WRITE_REG_ICU(	BOARD_IX1000__INTEN,
							0x00000000);

			if (bInitIn)
			{
				// clear all pendings interrupts (write 1 to corresponding bit)
				WRITE_REG_ICU(	BOARD_IX1000__STATUS,
								BOARD_IX1000__STATUS_INT_MASK);
			}

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(BOARD_IX1000__STATUS);

			break;
		}
		case ePNDEV_ICU_MICREL:	// Micrel
		{
			// mask all interrupts
			WRITE_REG_ICU(	BOARD_MICREL_KSZ88XX__INTEN,
							0x00000000);

			if (bInitIn)
			{
				// clear all pendings interrupts (write 1 to corresponding bit)
				WRITE_REG_ICU(	BOARD_MICREL_KSZ88XX__INTST,
								0xFFFFffff);
			}

			// ensure that last PCI-Write is done
			lTmp = READ_REG_ICU(BOARD_MICREL_KSZ88XX__INTST);

			break;
		}
		case ePNDEV_ICU_HERA:	// Hera
		{
		UINT32 i = 0;
		
			for (i = 0; i < HERA_MSIX_VECTOR_COUNT_MAX; i++)
				// each vector
			{
				// mask msix vector at msix table
				WRITE_REG_ICU(	((i *0x10) + 0xc),
								0x1);				
			}

			if	(!bInitIn)
			{
				// and delete the backup array
				for (i = 0; i < HERA_MSIX_VECTOR_COUNT_MAX; i++)
					// each vector
				{
					m_bEventEnable[i] = FALSE;
				}
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  unmask desired interrupts at ICU
//************************************************************************

BOOLEAN clsUsedDevice::fnUnmaskIcu_Desired(	uPNDEV_CHANGE_INT_IN*	pChangeIntIn,
											const UINT32			lDimErrorIn,
											_TCHAR*					sErrorOut)
{
BOOLEAN bResult				= FALSE;
BOOLEAN	bUnmaskStdInt		= FALSE;
BOOLEAN	bUnmaskDemoIntOnce	= FALSE;
BOOLEAN	bUnmaskDemoIntCont	= FALSE;

	// preset
	bResult = TRUE;

	if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
		// IntAction=Standard
	{
		// unmask StandardInt
		bUnmaskStdInt = TRUE;

		if	(pChangeIntIn != NULL)
			// service ChangeInt
		{
		UINT32 i = 0;

			for	(i = 0; i < PNDEV_DIM_ARRAY_INT_SRC; i++)
			{
				if	(pChangeIntIn->bArrayIntAck[i])
					// current IntSrc should be acked
				{
					// ack TopLevel-ICU
					fnAckTopLevelIcu((ePNDEV_INT_SRC) i);
				}
			}
		}
	}
	else	// IntAction=Test
	{
		switch	(m_eIntAction)
				// desired IntAction
		{
			case ePNDEV_INT_ACTION_TEST__ONCE_UISR:
			case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS:
			case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL:
			{
				if	(pChangeIntIn != NULL)
					// service ChangeInt
				{
					// unmask DemoIntOnce
					bUnmaskDemoIntOnce = TRUE;
				}
				else
				{
					// 1* sequence ISR/DPC/UISR must be ensured
					// -> mask DemoInt at ISR (never unmask again)

					if	(m_eIntAction == ePNDEV_INT_ACTION_TEST__ONCE_UISR)
					{
						if	(!fnSetEventThreadService(	ePNDEV_EVENT_THREAD_SERVICE_CON_INT_TEST,
														lDimErrorIn,
														sErrorOut))
							// error at confirming success of IntTest
						{
							// error
							bResult = FALSE;
						}
					}
					else if	(m_eIntAction == ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL)
					{
						// simulate FatalAsync
						//	- verify that a FatalError at DllThread is signaled to user
						clsString::fnBuildString(	PNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_DLL,
													NULL,
													NULL,
													lDimErrorIn,
													sErrorOut);
					}
				}

				break;
			}
			case ePNDEV_INT_ACTION_TEST__CONT_UISR:
			{
				if	(pChangeIntIn != NULL)
					// service ChangeInt
				{
					// unmask DemoIntContinuous
					bUnmaskDemoIntCont = TRUE;
				}
				else
				{
					// continuous sequence ISR/DPC/UISR must be ensured
					// -> mask   AsicInt at ISR
					// -> unmask AsicInt at UISR

					// unmask DemoIntContinuous
					bUnmaskDemoIntCont = TRUE;
				}

				break;
			}
			case ePNDEV_INT_ACTION_TEST__CONT_DPC:
			case ePNDEV_INT_ACTION_TEST__CONT_ISR:
			{
				if	(pChangeIntIn != NULL)
					// service ChangeInt
				{
					// unmask DemoIntContinuous
					bUnmaskDemoIntCont = TRUE;
				}
				else
				{
					// error
					bResult = FALSE;

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid IntAction at UISR!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
				}

				break;
			}
			default:
			{
				// error
				bResult = FALSE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid IntSrc at IntTest!"),
											NULL,
											lDimErrorIn,
											sErrorOut);
				break;
			}
		}
	}

	if	(bResult)
		// success
	{
		// unmask desired interrupt(s)

		switch	(m_eIcu)
				// IcuType
		{
			case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of IRTE is done by user
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// unmask IRTE-IrqNrt-HP
					WRITE_REG_ICU(	IRTE_REG__ICU_IRQ1_MASK_NRT,
									(READ_REG_ICU(IRTE_REG__ICU_IRQ1_MASK_NRT) | IRTE_IRQ_NRT__HP));
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// unmask IRTE-IrqNrt-HP
					WRITE_REG_ICU(	IRTE_REG__ICU_IRQ1_MASK_NRT,
									(READ_REG_ICU(IRTE_REG__ICU_IRQ1_MASK_NRT) | IRTE_IRQ_NRT__HP));
				}

				break;
			}
			case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of PNIP is done by user
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// BoardInt is unmasked at driver
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// unmask PNIP-ICU-Event63
					WRITE_REG_ICU(	PNIP_REG__ICU2_IRQ0_MASK_MID,
									(READ_REG_ICU(PNIP_REG__ICU2_IRQ0_MASK_MID) & PNDEV_UINT32_CLR_BIT_31));
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of IRTE is done by user

					// unmask desired IntSrc of TopLevel-ICU

					if	(pChangeIntIn != NULL)
						// service ChangeInt
					{
						// compute EventEnabled0/1/2
						//	- combination of IntSrc allowed)
						{
							// preset masks
							m_lEventEnabled0	= 0;
							m_lEventEnabled1	= 0;
							m_lEventEnabled2	= 0;

							// compute mask for PCI-ICU-MuxA events
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__IRTE_IRQ1])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER0])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER0);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER1])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER1);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER2])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER2);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER3])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER3);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER4])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER4);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__TIMER5])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER5);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__GDMA])		{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GDMA);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__GPIO180])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__GPIO188])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__PB_PCI_F1])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F1);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__PB_PCI_F2])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F2);}
							if  (pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__IRTE_IRQ0])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ0);}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_SOC__SW_IRQ_14])	{m_lEventEnabled0 |= (1<<SOC1_PCI_ICU_VECTOR_MUX_A__SW_IRQ_14);}

							// unmask desired PCI-ICU-MuxA events
							WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKREG_A,
											(~m_lEventEnabled0));
						}
					}

					if	(m_lEventEnabled0 != 0)
						// any event unmasked
					{
						// enable all unmasked events
						WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKALL_A,
										0);
					}
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// unmask PCI-ICU-MuxA-Event31
					WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKREG_A,
									(READ_REG_ICU(SOC1_PCI_ICU_REG__IRQ_MASKREG_A) & PNDEV_UINT32_CLR_BIT_31));

					// enable all unmasked events
					WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKALL_A,
									0);
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// unmask PCI-ICU-MuxA-Event31
					WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKREG_A,
									(READ_REG_ICU(SOC1_PCI_ICU_REG__IRQ_MASKREG_A) & PNDEV_UINT32_CLR_BIT_31));

					// enable all unmasked events
					WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_MASKALL_A,
									0);
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of PNIP is done by user

					// unmask desired IntSrc of TopLevel-ICU

					if	(pChangeIntIn != NULL)
						// service ChangeInt
					{
						// compute EventEnabled0/1/2
						//	- combination of IntSrc allowed)
						{
							// preset masks
							m_lEventEnabled0	= 0;
							m_lEventEnabled1	= 0;
							m_lEventEnabled2	= 0;

							// compute mask for Event31:0
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI1_COMBINED])			{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI1_COMBINED;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI1_OVERRUN_ERROR])		{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI1_OVERRUN_ERROR;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI2_COMBINED])			{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI2_COMBINED;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI2_OVERRUN_ERROR])		{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI2_OVERRUN_ERROR;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI12_PARITY_ERROR])		{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI12_PARITY_ERROR;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI1_SEND_FIFO_EMPTY])		{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI1_SEND_FIFO_EMPTY;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI1_RCVE_FIFO_NOT_EMPTY])	{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI1_RCVE_FIFO_NOT_EMPTY;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI2_SEND_FIFO_EMPTY])		{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI2_SEND_FIFO_EMPTY;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__SPI2_RCVE_FIFO_NOT_EMPTY])	{m_lEventEnabled0 |= ERTEC200P_ARM_ICU_EVENT0__SPI2_RCVE_FIFO_NOT_EMPTY;}

							// compute mask for Event63:32
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO1])				{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__EXT_GPIO1;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO2])				{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__EXT_GPIO2;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF])					{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PERIF;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_ICU_IRQ0;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_ICU_IRQ1;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ2;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ3;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ4;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ5;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ6;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7])			{m_lEventEnabled1 |= ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ7;}

							// compute mask for Event95:64
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ8;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ9;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ10;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ11;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ12;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ13;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ14;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15])			{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ15;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT9])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT9;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT10])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT10;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT11])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT11;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT12])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT12;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT13])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT13;}
							if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT14])				{m_lEventEnabled2 |= ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT14;}
						}

						// unmask desired events of ARM-ICU-Event31:0
						WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK0,
										(~m_lEventEnabled0));

						// unmask desired events of ARM-ICU-Event63:32
						WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK1,
										(~m_lEventEnabled1));

						// unmask desired events of ARM-ICU-Event95:64
						WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK2,
										(~m_lEventEnabled2));
					}

					if	(	(m_lEventEnabled0	!= 0)
						||	(m_lEventEnabled1	!= 0)
						||	(m_lEventEnabled2	!= 0))
						// any event unmasked
					{
						// enable all unmasked events
						WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASKALL,
										0);
					}
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// BoardInt is unmasked at driver
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// unmask ARM-ICU-Event95
					WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASK2,
									(READ_REG_ICU(ERTEC200P_ARM_ICU_REG__MASK2) & PNDEV_UINT32_CLR_BIT_31));

					// enable all unmasked events
					WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__MASKALL,
									0);
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of PNIP is done by user

					// unmask desired IntSrc of TopLevel-ICU

					if	(pChangeIntIn != NULL)
						// service ChangeInt
					{
						// compute EventEnabled0/1/2
						//	- combination of IntSrc allowed)
						{
							// compute mask for Event31:0
							{
								// enable always
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_AHB_HOSTIF;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_XHIF_HOSTIF;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_SPI_HOSTIF;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_APP;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_PNIP;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_APP;
								m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_PNIP;

								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA1]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA1;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA2]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA2;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA3]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA3;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA4]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA4;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA5]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA5;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA6]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA6;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA7]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA7;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA8]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA8;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA9]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA9;}

								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ0;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ1;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ2;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ3;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ4;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ5;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ6;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ7;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ8;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9]) 	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ9;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ10;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ11;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ12;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ13;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ14;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15])	{m_lEventEnabled0 |= ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ15;}
							}

							// compute mask for Event63:32
							{
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT0]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT0;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT1]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT1;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT2]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT2;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT3]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT3;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT4]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT4;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT5]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT5;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT6]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT6;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT7]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT7;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT8]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT8;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT9]) 	{m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT9;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT10]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT10;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT11]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT11;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT12]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT12;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT13]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT13;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT14]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT14;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT15]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT15;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT16]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT16;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT17]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT17;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT18]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT18;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT19]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT19;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT20]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT20;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT21]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT21;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT22]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT22;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT23]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT23;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT24]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT24;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT25]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT25;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT26]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT26;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT27]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT27;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT28]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT28;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT29]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT29;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT30]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT30;}
								if	(pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT31]) {m_lEventEnabled1 |= ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT31;}
							}
						}
					}

					// unmask desired events of PERIF-ICU-Event31:0
					WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_MASK_LOW,
									(~m_lEventEnabled0));

					// unmask desired events of PERIF-ICU-Event63:32
					WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH,
									(~m_lEventEnabled1));
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// BoardInt is used, unmasking at driver
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// unmask PERIF-ICU-Event63
					WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH,
									(READ_REG_ICU(ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH) & PNDEV_UINT32_CLR_BIT_31));
				}

				break;
			}
			case ePNDEV_ICU_INTEL:	// Intel
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of INTEL is done by user
				}
				else
				{
				UINT32 lTmpReservedMask = 0;

					if	(m_uHwInfo.eBoard == ePNDEV_BOARD_I210)
						// I210 board
					{
						lTmpReservedMask = BOARD_I210__ICU_RESERVED_MASK;
					}
					else if (m_uHwInfo.eBoard == ePNDEV_BOARD_I210SFP)
							// I210SFP board
					{
						lTmpReservedMask = BOARD_I210__ICU_RESERVED_MASK;
					}
					else if	(m_uHwInfo.eBoard == ePNDEV_BOARD_I82574)
							// Intel I82574 board
					{
						lTmpReservedMask = BOARD_I82574__ICU_RESERVED_MASK;
					}

					if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
					{
						// unmask PCI-ICU-MuxA-Event31
						WRITE_REG_ICU(	BOARD_INTEL__IMS,
										((READ_REG_ICU(BOARD_INTEL__IMS) & lTmpReservedMask) | PNDEV_UINT32_SET_BIT_24 | PNDEV_UINT32_SET_BIT_0));
					}
					else if	(bUnmaskDemoIntCont)
							// unmask DemoIntContinuous
					{
						// unmask PCI-ICU-MuxA-Event31
						WRITE_REG_ICU(	BOARD_INTEL__IMS,
										((READ_REG_ICU(BOARD_INTEL__IMS) & lTmpReservedMask) | PNDEV_UINT32_SET_BIT_24 | PNDEV_UINT32_SET_BIT_0));
					}
				}

				break;
			}
			case ePNDEV_ICU_IX1000:	// IX1000
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of INTEL is done by user
				}
				else
				{
					if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
					{
						// unmask Event31
						WRITE_REG_ICU(	BOARD_IX1000__INTEN,
										((READ_REG_ICU(BOARD_IX1000__INTEN)) | PNDEV_UINT32_SET_BIT_0));
					}
					else if	(bUnmaskDemoIntCont)
							// unmask DemoIntContinuous
					{
						// unmask Event31
						WRITE_REG_ICU(	BOARD_IX1000__INTEN,
										((READ_REG_ICU(BOARD_IX1000__INTEN)) | PNDEV_UINT32_SET_BIT_0));
					}
				}

				break;
			}
			case ePNDEV_ICU_MICREL:	// Micrel
			{
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of Micrel is done by user
				}
				else
				{
					if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
					{
						// unmask Event31
						WRITE_REG_ICU(	BOARD_MICREL_KSZ88XX__INTEN,
										((READ_REG_ICU(BOARD_MICREL_KSZ88XX__INTEN)) | PNDEV_UINT32_SET_BIT_0));
					}
					else if	(bUnmaskDemoIntCont)
							// unmask DemoIntContinuous
					{
						// unmask Event31
						WRITE_REG_ICU(	BOARD_MICREL_KSZ88XX__INTEN,
										((READ_REG_ICU(BOARD_MICREL_KSZ88XX__INTEN)) | PNDEV_UINT32_SET_BIT_0));
					}
				}

				break;
			}
			case ePNDEV_ICU_HERA:	// Hera
			{
			UINT32 lOffsetVectorEntry 	= 0xc;
			UINT32 lVectorWidth			= 0x10;
            UINT32 lLoopCount           = 0;
			
				if	(bUnmaskStdInt)
					// unmask StandardInt
				{
					// unmasking of PNIP is done by user

					// unmask desired IntSrc of MsixVectorTable

					if	(pChangeIntIn != NULL)
						// service ChangeInt
					{
						// copy array from ChangeInt to MemberVariable for backup

						for(lLoopCount = 0; lLoopCount < PNDEV_MSIX_VECTOR_COUNT_MAX; lLoopCount++)
                        {
                            m_bEventEnable[lLoopCount] |= pChangeIntIn->bArrayIntUnmask[ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_SUM + lLoopCount];
                        }
					}

					
					// unmask corresping MsixVector
					{	
						if	(m_bEventEnable[HERA_MSIX__VECTOR_0 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_0  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_1 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_1  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_2 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_2  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_3 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_3  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_4 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_4  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_5 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_5  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_6 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_6  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_7 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_7  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_8 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_8  * lVectorWidth) + lOffsetVectorEntry),	0x0);}                                                                                                                
						if	(m_bEventEnable[HERA_MSIX__VECTOR_9 ]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_9  * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_10]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_10 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_11]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_11 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_12]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_12 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_13]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_13 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_14]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_14 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_15]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_15 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_16]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_16 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_17]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_17 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_18]) 	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_18 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_19])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_19 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_20])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_20 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_21])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_21 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_22])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_22 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_23])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_23 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_24])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_24 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_25])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_25 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_26])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_26 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_27])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_27 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_28])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_28 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_29])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_29 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_30])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_30 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
						if	(m_bEventEnable[HERA_MSIX__VECTOR_31])	{WRITE_REG_ICU(	((HERA_MSIX__VECTOR_31 * lVectorWidth) + lOffsetVectorEntry),	0x0);}
					}
				}
				else if	(bUnmaskDemoIntOnce)
						// unmask DemoIntOnce
				{
					// Inc9 unmask INT_SW_16 msix vector 8 at msix table			
					WRITE_REG_ICU(((8 * 0x10) + lOffsetVectorEntry), 0x0);

					// Inc12 unmask INT_SW_16 msix vector 10 at msix table			
					WRITE_REG_ICU(	((10 *0x10) + lOffsetVectorEntry), 0x0);
				}
				else if	(bUnmaskDemoIntCont)
						// unmask DemoIntContinuous
				{
					// Inc9 unmask INT_SW_16 msix vector 8 at msix table			
					WRITE_REG_ICU(((8 * 0x10) + 0xc), 0x0);

					// Inc12 unmask INT_SW_16 msix vector 10 at msix table			
					WRITE_REG_ICU(	((10 *0x10) + 0xc),	0x0);
				}
				
				break;
			}
			default:
			{
				// error
				bResult = FALSE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid IcuType!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set EOI-InactiveTime of MUX
//************************************************************************

void clsUsedDevice::fnSetEoiInactiveTime_Mux(const ePNDEV_INT_SRC eIntSrcIn)
{
	switch	(m_eIcu)
			// IcuType
	{
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// no MUX exist

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// no MUX exist

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// no MUX exist

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
				// IntAction=Standard
			{
				// PN-ICU:	IRQ level is inactive -> no action necessary
				// PN-MUX:	reload EOI-InactiveTime for preventing a new immediate event

				// PNIP-ICU2/MUX2 must be used!

				switch	(eIntSrcIn)
						// IntSrc
				{
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ2_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ3_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ4_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ5_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ6_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ7_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ8_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ9_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ10_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ11_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ12_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ13_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ14_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX2_IRQ15_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					default:
					{
						break;
					}
				}
			}
			else	// IntAction=Test
			{
				// nothing to do
				//	- ARM-ICU-Event95 is directly set by software at ARM-ICU -> no PN-MUX involved
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
				// IntAction=Standard
			{
				// PN-ICU:	IRQ level is inactive -> no action necessary
				// PN-MUX:	reload EOI-InactiveTime for preventing a new immediate event

				// PNIP-ICU3/MUX3 must be used!

				switch	(eIntSrcIn)
						// IntSrc
				{
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ2_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ3_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ4_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ5_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ6_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ7_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ8_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ9_CONTROL +2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ10_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ11_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ12_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ13_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ14_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15:	{WRITE_REG_PNIP_UINT16(	(PNIP_REG__MUX3_IRQ15_CONTROL+2),	PNIP_MUX__EOI_MAX_VALUE);	break;}
					default:
					{
						break;
					}
				}
			}
			else	// IntAction=Test
			{
				// nothing to do
				//	- ARM-ICU-Event95 is directly set by software at ARM-ICU -> no PN-MUX involved
			}

			break;
		}
		case ePNDEV_ICU_INTEL:
		case ePNDEV_ICU_IX1000:
		case ePNDEV_ICU_MICREL:
		{
			// no MUX exist

			break;
		}
		case ePNDEV_ICU_HERA:	// Hera
		{
			if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
				// IntAction=Standard
			{
				// PN-ICU:	IRQ level is inactive -> no action necessary
				// PN-MUX:	reload EOI-InactiveTime for preventing a new immediate event

				// PNIP-ICU3/MUX3 must be used!

				if (m_lBitStreamId == 0x09)
					// Inc09
				{
					switch (eIntSrcIn)
						// IntSrc
					{
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ2_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ3: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ3_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ4: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ4_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ5: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ5_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ6: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ6_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ7: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ7_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }

						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ2_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ3: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ3_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ4: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ4_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ5: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ5_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ6: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ6_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }
						case ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ7: {WRITE_REG_PNIP_UINT16((PNIP_REG__MUX3_IRQ7_CONTROL + 2), PNIP_MUX__EOI_MAX_VALUE);	break; }

						default:
						{
							break;
						}
					}
				}
				else
					// Inc12
				{
					// nothing to do
				}
			}
			else	// IntAction=Test
			{
				// nothing to do
				//	-no PN-MUX involved
			}
			
			break;
		}
		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  ack TopLevel-ICU
//************************************************************************

void clsUsedDevice::fnAckTopLevelIcu(const ePNDEV_INT_SRC eIntSrcIn)
{
	if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
		// IntAction=Standard
	{
		switch	(m_eIcu)
				// IcuType
		{
			case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
			{
				// nothing to do

				break;
			}
			case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
			{
				// nothing to do

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
			{
				switch	(eIntSrcIn)
						// IntSrc
				{
					case ePNDEV_INT_SRC_SOC__IRTE_IRQ1:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1);	break;}
					case ePNDEV_INT_SRC_SOC__TIMER0:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER0);		break;}
					case ePNDEV_INT_SRC_SOC__TIMER1:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER1);		break;}
					case ePNDEV_INT_SRC_SOC__TIMER2:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER2);		break;}
					case ePNDEV_INT_SRC_SOC__TIMER3:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER3);		break;}
					case ePNDEV_INT_SRC_SOC__TIMER4:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER4);		break;}
					case ePNDEV_INT_SRC_SOC__TIMER5:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__TIMER5);		break;}
					case ePNDEV_INT_SRC_SOC__GDMA:		{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__GDMA);		break;}
					case ePNDEV_INT_SRC_SOC__GPIO180:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180);	break;}
					case ePNDEV_INT_SRC_SOC__GPIO188:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188);	break;}
					case ePNDEV_INT_SRC_SOC__PB_PCI_F1:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F1);	break;}
					case ePNDEV_INT_SRC_SOC__PB_PCI_F2:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F2);	break;}
					case ePNDEV_INT_SRC_SOC__IRTE_IRQ0:	{WRITE_REG_ICU(	SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ0);	break;}
					case ePNDEV_INT_SRC_SOC__SW_IRQ_14:	{WRITE_REG_ICU( SOC1_PCI_ICU_REG__IRQ_SWIRREG_A,	0x0); // clear SW IRQ otherwise, new IRQ is generated by acknowledging IRQ in IRCLVEC
														 WRITE_REG_ICU( SOC1_PCI_ICU_REG__IRQ_IRCLVEC_A,	SOC1_PCI_ICU_VECTOR_MUX_A__SW_IRQ_14);	break;}

					default:
					{
						break;
					}
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
			{
				switch	(eIntSrcIn)
						// IntSrc
				{
					case ePNDEV_INT_SRC_ERTEC200P__SPI1_COMBINED:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI1_COMBINED);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI1_OVERRUN_ERROR:			{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI1_OVERRUN_ERROR);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI2_COMBINED:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI2_COMBINED);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI2_OVERRUN_ERROR:			{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI2_OVERRUN_ERROR);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI12_PARITY_ERROR:			{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI12_PARITY_ERROR);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI1_SEND_FIFO_EMPTY:		{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI1_SEND_FIFO_EMPTY);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI1_RCVE_FIFO_NOT_EMPTY:	{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI1_RCVE_FIFO_NOT_EMPTY);	break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI2_SEND_FIFO_EMPTY:		{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI2_SEND_FIFO_EMPTY);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__SPI2_RCVE_FIFO_NOT_EMPTY:	{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_LOW__SPI2_RCVE_FIFO_NOT_EMPTY);	break;}

					case ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO1:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__EXT_GPIO1);					break;}
					case ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO2:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__EXT_GPIO2);					break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF:						{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PERIF);						break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_ICU_IRQ0);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_ICU_IRQ1);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ2);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ3);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ4);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ5);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ6);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_MID__PNIP_MUX_IRQ7);				break;}

					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ8);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ9);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ10);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ11);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ12);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ13);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ14);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15:				{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNIP_MUX_IRQ15);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT9:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT9);					break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT10:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT10);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT11:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT11);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT12:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT12);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT13:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT13);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT14:					{WRITE_REG_ICU(	ERTEC200P_ARM_ICU_REG__IRCLVEC,	ERTEC200P_ARM_ICU_VECTOR_HIGH__PNPLL_OUT14);				break;}

					default:
					{
						break;
					}
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
			{
				switch	(eIntSrcIn)
						// IntSrc
				{
					// PERIF-ICU-Event31:0
					case ePNDEV_INT_SRC_ERTEC200P__ERR_AHB_HOSTIF:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_AHB_HOSTIF);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_XHIF_HOSTIF:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_XHIF_HOSTIF);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_SPI_HOSTIF:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_SPI_HOSTIF);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_APP:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_APP);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_PNIP:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_PNIP);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_APP:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_APP);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_PNIP:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_PNIP);	break;}

					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA1:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA1);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA2:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA2);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA3:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA3);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA4:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA4);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA5:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA5);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA6:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA6);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA7:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA7);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA8:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA8);				break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA9:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA9);				break;}

					case ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ0);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ1);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ2);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ3);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ4);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ5);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ6);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ7);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ8);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ9);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ10);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ11);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ12);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ13);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ14);		break;}
					case ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ15);		break;}

					// PERIF-ICU-Event63:32
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT0:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT0);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT1:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT1);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT2:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT2);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT3:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT3);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT4:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT4);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT5:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT5);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT6:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT6);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT7:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT7);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT8:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT8);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT9:		{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT9);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT10:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT10);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT11:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT11);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT12:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT12);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT13:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT13);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT14:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT14);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT15:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT15);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT16:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT16);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT17:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT17);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT18:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT18);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT19:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT19);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT20:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT20);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT21:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT21);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT22:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT22);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT23:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT23);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT24:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT24);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT25:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT25);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT26:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT26);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT27:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT27);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT28:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT28);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT29:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT29);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT30:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT30);			break;}
					case ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT31:	{WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_HIGH,	(UINT32) ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT31);			break;}
					default:
					{
						break;
					}
				}

				break;
			}
			case ePNDEV_ICU_INTEL:
			case ePNDEV_ICU_IX1000:
			case ePNDEV_ICU_MICREL:
			{
				// nothing to do

				break;
			}
			case ePNDEV_ICU_HERA:	// Hera
			{
				// nothing to do
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
	else	// IntAction=Test
	{
		// nothing to do
		//	- if desired interrupts are unmasked at ICU then cycle ISR-DPC-UISR will start again
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do actions of DllThread
//************************************************************************

void clsUsedDevice::fnDoThreadDll(void)
{
BOOLEAN	bStop				= FALSE;
BOOLEAN	bEventStopWithCon	= FALSE;
_TCHAR	sErrorThread[PNDEV_SIZE_STRING_BUF] = {0};
_TCHAR	sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

	// Note:
	//	- if there is an error at this thread it is notified by CbfEvent

	// set DebugMsg
	{
		_stprintf_s(sTraceDetail,
					_countof(sTraceDetail),
					_TEXT("PnDev[%s]: DllThread created"),
					m_sPciLoc);

		clsDataSet::fnSetTrace(sTraceDetail);
	}

	while	(!bStop)
			// thread not stopped
	{
	ePNDEV_EVENT_THREAD_DLL eEvent = ePNDEV_EVENT_THREAD_DLL_INVALID;

		if	(fnWaitForEventThreadDll(	_countof(sErrorThread),
										&eEvent,
										sErrorThread))
			// waiting for MultipleEvents at DllThread ok
		{
			switch	(eEvent)
					// signaled event
			{
				case ePNDEV_EVENT_THREAD_DLL_STOP:
				{
					// stop thread
					bStop = TRUE;

					// set DebugMsg
					{
						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: DllThread - EventStop"),
									m_sPciLoc);

						clsDataSet::fnSetTrace(sTraceDetail);
					}

					break;
				}
				case ePNDEV_EVENT_THREAD_DLL_STOP_WITH_CON:
				{
					bEventStopWithCon = TRUE;

					// stop thread
					bStop = TRUE;

					// set DebugMsg
					{
						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: DllThread - EventStopWithCon"),
									m_sPciLoc);

						clsDataSet::fnSetTrace(sTraceDetail);
					}

					break;
				}
				case ePNDEV_EVENT_THREAD_DLL_UINFO:
				{
					// call handler for EventUinfo
					fnEvtUinfo(	_countof(sErrorThread),
								sErrorThread);

					break;
				}
				case ePNDEV_EVENT_THREAD_DLL_UISR:
				{
					m_lCtrUisr++;

					// call handler for EventUisr
					fnEvtUisr(	_countof(sErrorThread),
								sErrorThread);

					break;
				}
				default:
				{
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Unexpected event at DllThread!"),
												NULL,
												_countof(sErrorThread),
												sErrorThread);

					break;
				}
			}
		}

		if	(_tcscmp(sErrorThread, _TEXT("")) != 0)
			// ErrorThread occurred
		{
		ePNDEV_EVENT_CLASS eEventClass = ePNDEV_EVENT_CLASS_INVALID;

			// Note:
			//	- don't stop thread here!
			//	- otherwise UsedDeviceObject will be deleted
			//		-> pUsedDevice cannot be used for following PnDev_Driver services (e.g. ChangeInt)!

			if	(_tcscmp(sErrorThread, PNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_DLL) == 0)
				// simulated FatalAsync at DllThread
			{
				eEventClass = ePNDEV_EVENT_CLASS_TEST;
			}
			else
			{
				eEventClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;
			}

			// call CbfEvent
			m_pCbfEvent(this,
						eEventClass,
						sErrorThread,
						(const void*) m_pCbfParUser);

			// clear ErrorThread
			_tcscpy_s(	sErrorThread,
						_countof(sErrorThread),
						_TEXT(""));
		}
	}

	// set DebugMsg
	{
		_stprintf_s(sTraceDetail,
					_countof(sTraceDetail),
					_TEXT("PnDev[%s]: DllThread terminated"),
					m_sPciLoc);

		clsDataSet::fnSetTrace(sTraceDetail);
	}

	// see fnPnDev_CloseDevice()

	if	(bEventStopWithCon)
		// EventStopWithCon
	{
		// set EventConStop
		fnSetEventThreadService(ePNDEV_EVENT_THREAD_SERVICE_CON_STOP,
								_countof(sErrorThread),
								sErrorThread);
	}
	else	// thread stopped by EventStop or error
	{
		// shut down resources of ServiceThread / DllThread
		fnShutDownThreadRes(_countof(sErrorThread),
							sErrorThread);

		// free UsedDeviceObject
		delete this;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  handler for EventUinfo
//************************************************************************

BOOLEAN clsUsedDevice::fnEvtUinfo(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN						bResult		= FALSE;
uPNDEV_GET_EVENT_LIST_IN	uGetEventListIn;
uPNDEV_GET_EVENT_LIST_OUT	uGetEventListOut;

	// set InputParameter
	{
		memset(	&uGetEventListIn,
				0,
				sizeof(uPNDEV_GET_EVENT_LIST_IN));

		// set internal parameter for driver
		//	- use 64Bit value because DriverHandle may be a 64Bit handle!
		uGetEventListIn.uAppSys.lUint64 = m_uAppSys.lUint64;
	}

	if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
										IOCTL_GET_EVENT_LIST,
										sizeof(uPNDEV_GET_EVENT_LIST_IN),
										sizeof(uPNDEV_GET_EVENT_LIST_OUT),
										&uGetEventListIn,
										&uGetEventListOut))
		// error at IoctlGetEventList
	{
		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): "),
									uGetEventListOut.sError,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
	_TCHAR sEventDetail[PNDEV_SIZE_STRING_BUF] = {0};

		if	(uGetEventListOut.uEvent.lFatalFw != 0)
			// event of class 'FatalFw' exist
		{
		_TCHAR sEvent[PNDEV_SIZE_STRING_BUF] = {0};

			// set ErrorDetail
			_stprintf_s(sEventDetail,
						_countof(sEventDetail),
						_TEXT(" (ErrorCode=0x%x)"),
						uGetEventListOut.uEvent.lFatalFw);

			// set ErrorString
			clsString::fnBuildString(	_TEXT(""),
										_TEXT("Fatal error at user firmware!"),
										sEventDetail,
										_countof(sEvent),
										sEvent);

			// call CbfEvent
			m_pCbfEvent(this,
						ePNDEV_EVENT_CLASS_FATAL_FW,
						sEvent,
						(const void*) m_pCbfParUser);
		}

		if	(uGetEventListOut.uEvent.eFatalAsync != ePNDEV_FATAL_ASYNC__INVALID)
			// event of class 'FatalAsync' exist
		{
		ePNDEV_EVENT_CLASS	eClass = ePNDEV_EVENT_CLASS_INVALID;
		_TCHAR				sEvent[PNDEV_SIZE_STRING_BUF] = {0};

			switch	(uGetEventListOut.uEvent.eFatalAsync)
					// FatalAsync
			{
				case ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS:			{eClass = ePNDEV_EVENT_CLASS_TEST;			_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__SIMULATED_FATAL_SYS);		break;}
				case ePNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS:	{eClass = ePNDEV_EVENT_CLASS_TEST;			_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__SIMULATED_FATAL_THREAD_SYS);	break;}

				case ePNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_BOARD_DETAIL);		break;}
				case ePNDEV_FATAL_ASYNC__INVALID_ICU_MODE:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_ICU_MODE);			break;}
				case ePNDEV_FATAL_ASYNC__INVALID_INT_ACTION:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_INT_ACTION);			break;}
				case ePNDEV_FATAL_ASYNC__INVALID_ASIC_INT:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_ASIC_INT);			break;}
				case ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT);		break;}
				case ePNDEV_FATAL_ASYNC__INVALID_DPC_CALL:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_DPC_CALL);			break;}
				case ePNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET);		break;}
				case ePNDEV_FATAL_ASYNC__BOARD_DEMO_INT_NOT_SET:		{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__BOARD_DEMO_INT_NOT_SET);		break;}
				case ePNDEV_FATAL_ASYNC__DPC_NOT_QUEUED:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__DPC_NOT_QUEUED);				break;}
				case ePNDEV_FATAL_ASYNC__UISR_NOT_QUEUED:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__UISR_NOT_QUEUED);			break;}
				case ePNDEV_FATAL_ASYNC__ERROR_ISR_LOCK:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ERROR_ISR_LOCK);				break;}
				case ePNDEV_FATAL_ASYNC__ERROR_INT_MASK:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ERROR_INT_MASK);				break;}
				case ePNDEV_FATAL_ASYNC__PCIE_ACCESS_TIMEOUT:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__PCIE_ACCESS_TIMEOUT);		break;}
				case ePNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST:			{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST);		break;}
				case ePNDEV_FATAL_ASYNC__INVALID_DYN_DRIVER_PAGE:		{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__INVALID_DYN_DRIVER_PAGE);	break;}
				case ePNDEV_FATAL_ASYNC__DYN_DRIVER_PAGE_SET_ERROR:		{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__DYN_DRIVER_PAGE_SET_ERROR);	break;}
				case ePNDEV_FATAL_ASYNC__SIGN_OF_LIFE:					{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__SIGN_OF_LIFE);				break;}
				case ePNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ERROR_SPIN_LOCK);			break;}
				case ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC:				{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ERROR_CRIT_SEC);				break;}
				case ePNDEV_FATAL_ASYNC__ERROR_EVENT:					{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ERROR_EVENT);				break;}
				case ePNDEV_FATAL_ASYNC__ACCESS_ERROR:					{eClass = ePNDEV_EVENT_CLASS_FATAL_ASYNC;	_tcscpy_s(sEvent, _countof(sEvent), PNDEV_FATAL_ASYNC__ACCESS_ERROR);				break;}
				default:
				{
					// set ErrorDetail
					_stprintf_s(sEventDetail,
								_countof(sEventDetail),
								_TEXT(" (ErrorCode=%d)"),
								uGetEventListOut.uEvent.eFatalAsync);

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid event of class 'FatalAsync'!"),
												sEventDetail,
												_countof(sEvent),
												sEvent);

					break;
				}
			}

			// call CbfEvent
			m_pCbfEvent(this,
						eClass,
						sEvent,
						(const void*) m_pCbfParUser);
		}

		if	(uGetEventListOut.uEvent.eWarning != ePNDEV_WARNING__INVALID)
			// event of class 'warning' exist
		{
		_TCHAR sEvent[PNDEV_SIZE_STRING_BUF] = {0};

			switch	(uGetEventListOut.uEvent.eWarning)
					// event
			{
				case ePNDEV_WARNING__PCIE_ACCESS_TIMEOUT:
				{
					_tcscpy_s(	sEvent,
								_countof(sEvent),
								PNDEV_WARNING__PCIE_ACCESS_TIMEOUT);

					break;
				}
				default:
				{
					// set ErrorDetail
					_stprintf_s(sEventDetail,
								_countof(sEventDetail),
								_TEXT(" (ErrorCode=%d)"),
								uGetEventListOut.uEvent.eWarning);

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid event of class 'Warning'!"),
												sEventDetail,
												_countof(sEvent),
												sEvent);

					break;
				}
			}

			// call CbfEvent
			m_pCbfEvent(this,
						ePNDEV_EVENT_CLASS_WARNING,
						sEvent,
						(const void*) m_pCbfParUser);
		}

		if	(uGetEventListOut.uEvent.bReqClose)
			// event of class 'ReqClose' exist
		{
		_TCHAR sEvent[PNDEV_SIZE_STRING_BUF] = {0};

			_tcscpy_s(	sEvent,
						_countof(sEvent),
						PNDEV_REQ_CLOSE);

			// call CbfEvent
			m_pCbfEvent(this,
						ePNDEV_EVENT_CLASS_REQ_CLOSE,
						sEvent,
						(const void*) m_pCbfParUser);
		}

		if	(uGetEventListOut.uEvent.eDebug != ePNDEV_DEBUG__INVALID)
			// event of class 'debug' exist
		{
		_TCHAR sEvent[PNDEV_SIZE_STRING_BUF] = {0};

			switch	(uGetEventListOut.uEvent.eDebug)
					// event
			{
				case ePNDEV_DEBUG__SET_SEMAPHORE_BY_DEBUGGER:
				{
					_tcscpy_s(	sEvent,
								_countof(sEvent),
								PNDEV_DEBUG__SET_SEMAPHORE_BY_DEBUGGER);

					break;
				}
				default:
				{
					// set ErrorDetail
					_stprintf_s(sEventDetail,
								_countof(sEventDetail),
								_TEXT(" (ErrorCode=%d)"),
								uGetEventListOut.uEvent.eFatalAsync);

					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid event of class 'debug'!"),
												sEventDetail,
												_countof(sEvent),
												sEvent);

					break;
				}
			}

			// call CbfEvent
			m_pCbfEvent(this,
						ePNDEV_EVENT_CLASS_DEBUG,
						sEvent,
						(const void*) m_pCbfParUser);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  handler for EventUisr
//************************************************************************

BOOLEAN clsUsedDevice::fnEvtUisr(	const UINT32	lDimErrorIn,
									_TCHAR*			sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset
	bResult = TRUE;

	if	(m_eIntAction <= PNDEV_INT_ACTION_STD_MAX)
		// IntAction=Standard
	{
		switch	(m_eIcu)
				// IcuType
		{
			case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
			{
				// Note:
				//	- user is responsible for unmasking IRTE events
				//	- DLL does not know which events are unmasked by user
				//	- if there is an IRTE interrupt there must have been an unmasking of at least one IRTE event
				//	- IR already checked by driver, at least one bit must be set
				//	- IRTE interrupts are masked by driver -> checking of IRR doesn't make sense

				if	(m_bIntEnabledIrtePnipIntel)
					// interrupts of IRTE-XIRQ1_HP enabled (TopLevel-ICU implemented by software)
				{
				UINT32	lIrrIrt	= 0;
				UINT32	lIrrNrt	= 0;

					lIrrIrt	= READ_REG_ICU(IRTE_REG__ICU_IRR_IRT);
					lIrrNrt	= READ_REG_ICU(IRTE_REG__ICU_IRR_NRT);

					if	(	(lIrrIrt	!= 0)
						||	(lIrrNrt	!= 0))
						// any IRTE interrupt (IRR)
					{
						// don't check further on in while loop
						//	- otherwise reloading of Asic-EOI-Inactive-Time of IRTE-ICU would be useless

						// active IntSrc found
						fnFoundIntSrc(	ePNDEV_INT_SRC_ERTECX00__IRTE_IRQ1,
										FALSE);
					}
				}

				break;
			}
			case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
			{
				// Note:
				//	- user is responsible for unmasking PNIP events
				//	- if there is an PNIP interrupt there must have been an unmasking of at least one PNIP event by the user
				//	  -> at least one IRR-bit must be set (but DLL does not know which events are unmasked by user)

				if	(m_bIntEnabledIrtePnipIntel)
					// interrupts of IRQ0 (combined interrupt) of PN-ICU2 enabled
				{
				UINT32	lIrrLow		= 0;
				UINT32	lIrrMid		= 0;
				UINT32	lIrrHigh	= 0;

					lIrrLow		= READ_REG_ICU(PNIP_REG__ICU2_IRQ0_EVENT_LOW);
					lIrrMid		= READ_REG_ICU(PNIP_REG__ICU2_IRQ0_EVENT_MID);
					lIrrHigh	= READ_REG_ICU(PNIP_REG__ICU2_IRQ0_EVENT_HIGH);

					if	(	(lIrrLow	!= 0)
						||	(lIrrMid	!= 0)
						||	(lIrrHigh	!= 0))
						// any PNIP-ICU2 interrupt (IRR)
					{
						// don't check further on in while loop
						//	- otherwise reloading of Asic-EOI-Inactive-Time of IRTE-ICU would be useless

						// active IntSrc found
						fnFoundIntSrc(	ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0,
										FALSE);
					}
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
			{
			UINT32 lIrMuxA = 0;

				// get PCI-ICU-MuxA interrupts (IR = IRR & Mask)
				lIrMuxA = (READ_REG_ICU(SOC1_PCI_ICU_REG__IRQ_IRREG_A) & m_lEventEnabled0);

				if	(lIrMuxA != 0)
					// any PCI-ICU-MuxA event
				{
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ1))	{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__IRTE_IRQ1,	FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER0))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER0,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER1))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER1,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER2))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER2,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER3))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER3,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER4))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER4,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__TIMER5))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__TIMER5,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GDMA))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__GDMA,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO180))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__GPIO180,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__GPIO188))		{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__GPIO188,		FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F1))	{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__PB_PCI_F1,	FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__PB_PCI_F2))	{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__PB_PCI_F2,	FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__IRTE_IRQ0))	{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__IRTE_IRQ0,	FALSE);}
					if	(lIrMuxA & (1<<SOC1_PCI_ICU_VECTOR_MUX_A__SW_IRQ_14))	{fnFoundIntSrc(ePNDEV_INT_SRC_SOC__SW_IRQ_14,	FALSE);}
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
			{
			UINT32	lIr0 = 0;
			UINT32	lIr1 = 0;
			UINT32	lIr2 = 0;

				// get ARM-ICU interrupts (IR = IRR & Mask)
				lIr0	= (READ_REG_ICU(ERTEC200P_ARM_ICU_REG__IRR0) & m_lEventEnabled0);
				lIr1	= (READ_REG_ICU(ERTEC200P_ARM_ICU_REG__IRR1) & m_lEventEnabled1);
				lIr2	= (READ_REG_ICU(ERTEC200P_ARM_ICU_REG__IRR2) & m_lEventEnabled2);
/*
				#ifdef _DEBUG
				{
					// set DebugMsg
					{
					_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: IR0=0x%08x, IR1=0x%08x, IR2=0x%08x"),
									m_sPciLoc,
									lIr0,
									lIr1,
									lIr2);

						clsDataSet::fnSetTrace(sTraceDetail);
					}
				}
				#endif
*/
				if	(lIr0 != 0)
					// any event of ARM-ICU-Event31:0
				{
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI1_COMBINED)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI1_COMBINED,				FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI1_OVERRUN_ERROR)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI1_OVERRUN_ERROR,		FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI2_COMBINED)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI2_COMBINED,				FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI2_OVERRUN_ERROR)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI2_OVERRUN_ERROR,		FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI12_PARITY_ERROR)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI12_PARITY_ERROR,		FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI1_SEND_FIFO_EMPTY)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI1_SEND_FIFO_EMPTY,		FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI1_RCVE_FIFO_NOT_EMPTY)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI1_RCVE_FIFO_NOT_EMPTY,	FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI2_SEND_FIFO_EMPTY)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI2_SEND_FIFO_EMPTY,		FALSE);}
					if	(lIr0 & ERTEC200P_ARM_ICU_EVENT0__SPI2_RCVE_FIFO_NOT_EMPTY)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__SPI2_RCVE_FIFO_NOT_EMPTY,	FALSE);}
				}

				if	(lIr1 != 0)
					// any event of ARM-ICU-Event63:32
				{
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__EXT_GPIO1)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO1,		FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__EXT_GPIO2)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__EXT_GPIO2,		FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PERIF)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF,			FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_ICU_IRQ0)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_ICU_IRQ1)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ2)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ3)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ4)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ5)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ6)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6,	FALSE);}
					if	(lIr1 & ERTEC200P_ARM_ICU_EVENT1__PNIP_MUX_IRQ7)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7,	FALSE);}
				}

				if	(lIr2 != 0)
					// any event of ARM-ICU-Event95:64
				{
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ8)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ9)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ10)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ11)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ12)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ13)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ14)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNIP_MUX_IRQ15)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15,	FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT9)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT9,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT10)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT10,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT11)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT11,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT12)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT12,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT13)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT13,		FALSE);}
					if	(lIr2 & ERTEC200P_ARM_ICU_EVENT2__PNPLL_OUT14)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNPLL_OUT14,		FALSE);}
				}

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
			{
			UINT32	lIr0 = 0;
			UINT32	lIr1 = 0;

				// get PERIF-ICU interrupts (IR = IRR & Mask)
				lIr0	= (READ_REG_ICU(ERTEC200P_PERIF_REG__HOST_IRQ_EVENT_LOW)  & m_lEventEnabled0);
				lIr1	= (READ_REG_ICU(ERTEC200P_PERIF_REG__HOST_IRQ_EVENT_HIGH) & m_lEventEnabled1);
/*
				#ifdef _DEBUG
				{
					// set DebugMsg
					{
					_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: IRR_LOW=0x%04x, IRR_HIGH=0x%04x"),
									m_sPciLoc,
									lIr0,
									lIr1);

						// set DebugMsg
						clsDataSet::fnSetTrace(sTraceDetail);
					}
				}
				#endif
*/
				if	(lIr0 != 0)
					// any event of PERIF-ICU-Event31:0
				{
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_AHB_HOSTIF)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_AHB_HOSTIF,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_XHIF_HOSTIF)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_XHIF_HOSTIF,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_SPI_HOSTIF)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_SPI_HOSTIF,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_APP)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_APP,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_ADR_PNIP)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_ADR_PNIP,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_APP)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_APP,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__ERR_BUF_STATE_PNIP)	{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__ERR_BUF_STATE_PNIP,	FALSE);}

					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA1)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA1,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA2)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA2,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA3)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA3,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA4)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA4,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA5)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA5,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA6)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA6,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA7)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA7,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA8)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA8,	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__NEW_DATA9)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_NEW_DATA9,	FALSE);}

					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ0)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ0,		FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ1)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_ICU_IRQ1,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ2)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ2,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ3)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ3,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ4)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ4,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ5)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ5,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ6)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ6,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ7)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ7,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ8)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ8,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ9)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ9,  	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ10)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ10, 	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ11)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ11, 	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ12)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ12, 	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ13)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ13, 	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ14)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ14, 	FALSE);}
					if	(lIr0 & ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_MUX_IRQ15)		{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PNIP_MUX_IRQ15, 	FALSE);}

					if	(m_uHwInfo.eAsicDetail == ePNDEV_ASIC_DETAIL_PNIP_REV1)
						// revision 1
					{
						// Note:
						//	- entries of PERIF-ICU are level-triggered, not conform to specification (Ertec200P error)
						//	- levels of PNIP_ICU_IRQ0/1 are active until PNIP sources are acked and EOI is set
						//		-> ack of TopLevel-ICU cannot be effective if it is done before calling CBF
						//		-> ack TopLevel-ICU afterwards
						//			- at level triggering interrupt cannot be lost if source is not a hardware pulse
						//			- without ack CBF would be called a second time although there is no new PNIP interrupt!

						// ack TopLevel-ICU: PNIP_ICU_IRQ0/1
						WRITE_REG_ICU(	ERTEC200P_PERIF_REG__HOST_IRQ_ACK_LOW,
										(UINT32) (ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ0 | ERTEC200P_PERIF_ICU_EVENT_LOW__PNIP_ICU_IRQ1));
					}
				}

				if	(lIr1 != 0)
					// any event of PERIF-ICU-Event63:32
				{
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT0)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT0,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT1)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT1,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT2)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT2,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT3)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT3,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT4)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT4,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT5)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT5,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT6)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT6,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT7)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT7,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT8)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT8,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT9)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT9,  	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT10)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT10,	FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT11)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT11,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT12)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT12,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT13)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT13,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT14)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT14,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT15)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT15,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT16)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT16,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT17)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT17,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT18)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT18,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT19)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT19,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT20)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT20,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT21)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT21,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT22)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT22,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT23)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT23,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT24)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT24,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT25)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT25,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT26)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT26,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT27)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT27,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT28)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT28,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT29)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT29,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT30)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT30,  FALSE);}
					if	(lIr1 & ERTEC200P_PERIF_ICU_EVENT_HIGH__SW_EVENT31)			{fnFoundIntSrc(ePNDEV_INT_SRC_ERTEC200P__PERIF_SW_EVENT31,  FALSE);}
				}

				break;
			}
			case ePNDEV_ICU_INTEL:	// Intel
			{
				if	(m_bIntEnabledIrtePnipIntel)
					// interrupts of Intel enabled
				{
					switch(m_uHwInfo.eBoard)
					{
						case ePNDEV_BOARD_I210:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_I210,
											FALSE);

							break;
						}
						case ePNDEV_BOARD_I210SFP:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_I210SFP,
											FALSE);

							break;
						}
						case ePNDEV_BOARD_I82574:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_I82574,
											FALSE);

							break;
						}
						default:
						{
							break;
						}
					}
				}

				break;
			}
			case ePNDEV_ICU_IX1000:	// IX1000
			{
				if	(m_bIntEnabledIrtePnipIntel)
					// interrupts of Intel enabled
				{
					switch(m_uHwInfo.eBoard)
					{
						case ePNDEV_BOARD_IX1000:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_IX1000,
											FALSE);

							break;
						}
						default:
						{
							break;
						}
					}
				}

				break;
			}
			case ePNDEV_ICU_MICREL:	// Micrel
			{
				if	(m_bIntEnabledIrtePnipIntel)
					// interrupts of Intel enabled
				{
					switch(m_uHwInfo.eBoard)
					{
						case ePNDEV_BOARD_MICREL_KSZ8841:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_MICREL_KSZ8841,
											FALSE);

							break;
						}
						case ePNDEV_BOARD_MICREL_KSZ8842:
						{
							fnFoundIntSrc(	ePNDEV_INT_SRC_MICREL_KSZ8842,
											FALSE);

							break;
						}
						default:
						{
							break;
						}
					}
				}

				break;
			}
			case ePNDEV_ICU_HERA:	// Hera
			{
			uPNDEV_GET_INT_STATE_IN		uGetIntStateIn;
			uPNDEV_GET_INT_STATE_OUT	uGetIntStateOut;

				m_pDataSet->fnEnterCritSec(	ePNDEV_CRIT_SEC_DLL_SERVICE,
											lDimErrorIn,
											sErrorOut);

				// set InputParameter
				{
					memset(	&uGetIntStateIn,
							0,
							sizeof(uPNDEV_GET_INT_STATE_IN));
				}

				if	(!m_pDataSet->fnDoDeviceIoctl(	(void*) this,
													IOCTL_GET_INT_STATE,
													sizeof(uPNDEV_GET_INT_STATE_IN),
													sizeof(uPNDEV_GET_INT_STATE_OUT),
													&uGetIntStateIn,
													&uGetIntStateOut))
					// error at IoctlResetDevice
				{
					// error
					bResult = FALSE;
					
					m_pDataSet->fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL_SERVICE,
												lDimErrorIn,
												sErrorOut);
					
					// set ErrorString
					clsString::fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): "),
												uGetIntStateOut.sError,
												lDimErrorIn,
												sErrorOut);
				}
				else
				{
					// success
					bResult = TRUE;
					
					{
					UINT32 lLoopCount = 0;
						// copy array from ChangeInt to MemberVariable

						for(lLoopCount = 0; lLoopCount < PNDEV_MSIX_VECTOR_COUNT_MAX; lLoopCount++)
                        {
                            m_bEventEnable[lLoopCount] |= uGetIntStateOut.bIntOccured[lLoopCount];
                        }
					}
								
					m_pDataSet->fnLeaveCritSec(	ePNDEV_CRIT_SEC_DLL_SERVICE,
												lDimErrorIn,
												sErrorOut);
					
					// call cbf with corresponding IntSrc
					{
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_0] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_SUM	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_1] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT12,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_2] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT13,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_3] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT14,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_4] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT15,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_5] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT16,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_6] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__XGMDA_JOB_DONE_INT20,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_7] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__WD_OUT0				,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_8] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__SCRB_SW_INT14		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_9] )	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__SCRB_SW_INT15		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_10])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__SCRB_SW_INT16		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_11])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__SCRB_SW_INT17		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_12])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__GPIO_EXT_INT_0		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_13])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__GPIO_EXT_INT_4		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_14])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__GPIO_EXT_INT_8		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_15])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__GPIO_EXT_INT_9		,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_16])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ0	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_17])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_ICU_IRQ1	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_18])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ2	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_19])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ3	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_20])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ4	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_21])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ5	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_22])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ6	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_23])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFA_MUX_IRQ7	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_24])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ0	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_25])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_ICU_IRQ1	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_26])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ2	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_27])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ3	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_28])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ4	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_29])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ5	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_30])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ6	,  FALSE);}
						if	(uGetIntStateOut.bIntOccured[HERA_MSIX__VECTOR_31])	{fnFoundIntSrc(ePNDEV_INT_SRC_HERA__PNIP_IFB_MUX_IRQ7	,  FALSE);}
					}
					
				}

				break;
			}
			default:
			{
				// error
				bResult = FALSE;

				// set ErrorString
				clsString::fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): Invalid IcuType!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

				break;
			}
		}
	}
	else	// IntAction=Test
	{
		// IntTest
		fnFoundIntSrc(	ePNDEV_INT_SRC_TEST,
						TRUE);
	}

	if	(bResult)
		// success
	{
		// unmask desired interrupts at ICU
		bResult = fnUnmaskIcu_Desired(	NULL,
										lDimErrorIn,
										sErrorOut);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  IntSrc was found
//************************************************************************

void clsUsedDevice::fnFoundIntSrc(	const ePNDEV_INT_SRC	eIntSrcIn,
									const BOOLEAN			bDeviceTestIn)
{
	// set EOI-InactiveTime of MUX
	//	- must be done before ack of TopLevel-ICU!
	fnSetEoiInactiveTime_Mux(eIntSrcIn);

	// ack TopLevel-ICU
	fnAckTopLevelIcu(eIntSrcIn);

	// call CbfUisr
	m_pCbfIsr(	this,
				bDeviceTestIn,
				eIntSrcIn,
				(const void*) m_pCbfParUser);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	shut down used timer
//************************************************************************

BOOLEAN clsUsedDevice::fnShutDownUsedTimer(	const UINT32	lDimErrorIn,
											_TCHAR*			sErrorOut)
{
BOOLEAN bResult	= FALSE;

	{
		// preset
		bResult = TRUE;

		// close all used timer
		//	- get TimerHandles by UsedTimerList
		//	- close devices in the same way as a user would do it
		{
		BOOLEAN			bStop			= FALSE;
		uLIST_HEADER*	pUsedTimerTmp	= NULL;

			while	(!bStop)
					// not stopped
			{
				// get first element of UsedTimerList
				pUsedTimerTmp = (uLIST_HEADER*) fnGetFirstBlkFromList(fnGetPtrListUsedTimer());

				if	(pUsedTimerTmp == NULL)
					// no (further) UsedTimer exist
				{
					// stop
					bStop = TRUE;
				}
				else
				{
				uPNDEV_CLOSE_TIMER_IN	uCloseTimerIn;
				uPNDEV_CLOSE_TIMER_OUT	uCloseTimerOut;
				ePNDEV_RESULT			eResultTmp = ePNDEV_FATAL;

					// by service CloseDevice the UsedDevice will be removed from UsedDeviceList

					// set DebugMsg
					{
					_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

						_stprintf_s(sTraceDetail,
									_countof(sTraceDetail),
									_TEXT("PnDev[%s]: Shut down DLL - Close timer"), 
									fnGetPciLoc());

						clsDataSet::fnSetTrace(sTraceDetail);
					}

					// set InputParameter
					{
						memset(	&uCloseTimerIn,
								0,
								sizeof(uPNDEV_CLOSE_TIMER_IN));

						uCloseTimerIn.uTimer.hHandle	= pUsedTimerTmp;
					}

					// close timer
					eResultTmp = fnPnDev_CloseTimer(	m_hDriverDll,
														sizeof(uPNDEV_CLOSE_TIMER_IN),
														sizeof(uPNDEV_CLOSE_TIMER_OUT),
														&uCloseTimerIn,
														&uCloseTimerOut);

					if	(eResultTmp != ePNDEV_OK)
						// error
					{
						// error
						bResult = FALSE;

						// set ErrorString
						clsString::fnBuildString(	_TEXT(__FUNCTION__),
													_TEXT("(): "),
													uCloseTimerOut.sError,
													lDimErrorIn,
													sErrorOut);

						// set DebugMsg
						{
						_TCHAR sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

							_stprintf_s(sTraceDetail,
										_countof(sTraceDetail),
										_TEXT("PnDev[%s]: %s"), 
										fnGetPciLoc(),
										sErrorOut);

							clsDataSet::fnSetTrace(sTraceDetail);
						}
					}
				}
			}
		}
	}

	return(bResult);
}

#if (PNDEV_PLATFORM==PNDEV_UM_PLATFORM_32)
// ToDo: MK: sind die NRT-Services überhaupt in Verwendung
//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize DMM (NRT)
//************************************************************************
BOOLEAN clsUsedDevice::fnInitDmmNrt(uPNDEV_HW_DESC	uMemNrtIn,
									uPNDEV_HW_DESC	uMemDescTableIn,
									const UINT32	lAlignmentIn,
									const UINT32	lDimErrorIn,
									UINT32*			pCtrNrtRqbOut,
									_TCHAR*			sErrorOut)
{
BOOLEAN					bResult			= FALSE;
eDMM_ERROR				eErrorDmm		= eDMM_ERROR_INVALID;
UINT32					lSizeRealMemNrt	= 0;
uDMM_INIT_BLOCK_QUEUE	uInitBlock;
_TCHAR					sErrorDetail[PNDEV_SIZE_STRING_BUF]	= {0};

	// initialize DMM MemoryManagement
	m_oMemDmm.dmm_startup();

	// set InputParameter for DmmQueue of NrtMem
	{
	volatile UINT8* pAlignedStart = NULL;

		pAlignedStart = (volatile UINT8*) ((UINT32) fnGetAlignedValue((UINT64) uMemNrtIn.uBase.pPtr, lAlignmentIn));

		lSizeRealMemNrt = uMemNrtIn.lSize - (UINT32) ((UINT64) pAlignedStart - (UINT64) uMemNrtIn.uBase.pPtr);

		// set DMM InitBlock of NrtMem
		uInitBlock.lQueueId			= ePNDEV_DMM_QUEUE_MEM_NRT;
		uInitBlock.lAdrStart		= (LSA_UINT32) pAlignedStart;
		uInitBlock.lLen				= lSizeRealMemNrt;
		uInitBlock.lAlignment		= lAlignmentIn;
		uInitBlock.lLenPrebuffer	= sizeof(uPNDEV_NRT_PRE_BUFFER);
	}

	if	(!m_oMemDmm.dmm_init_queue(	uInitBlock,
									&eErrorDmm))
		// error
	{
		// set ErrorDetail
		_stprintf_s(sErrorDetail,
					_countof(sErrorDetail),
					_TEXT(" (NrtMem, ErrorCode=%d)"),
					eErrorDmm);

		// set ErrorString
		clsString::fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): DMM error at dmm_init_queue()!"),
									sErrorDetail,
									lDimErrorIn,
									sErrorOut);
	}
	else
	{
		// set InputParameter for DmmQueue of DescTableMem
		{
		volatile UINT8*	pAlignedStart	= NULL;
		UINT32			lSizeReal		= 0;

			pAlignedStart = (volatile UINT8*) ((UINT32) fnGetAlignedValue((UINT64) uMemDescTableIn.uBase.pPtr, lAlignmentIn));

			lSizeReal = uMemDescTableIn.lSize - (UINT32) ((UINT64) pAlignedStart - (UINT64) uMemDescTableIn.uBase.pPtr);

			// set DMM InitBlock of DescTableMem
			uInitBlock.lQueueId			= ePNDEV_DMM_QUEUE_MEM_DESC_TABLE;
			uInitBlock.lAdrStart		= (LSA_UINT32) pAlignedStart;
			uInitBlock.lLen				= lSizeReal;
			uInitBlock.lAlignment		= lAlignmentIn;
			uInitBlock.lLenPrebuffer	= 0;
		}

		if	(!m_oMemDmm.dmm_init_queue(	uInitBlock,
										&eErrorDmm))
			// error
		{
			// set ErrorDetail
			_stprintf_s(sErrorDetail,
						_countof(sErrorDetail),
						_TEXT(" (DescTableMem, ErrorCode=%d)"),
						eErrorDmm);

			// set ErrorString
			clsString::fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): DMM error at dmm_init_queue()!"),
										sErrorDetail,
										lDimErrorIn,
										sErrorOut);
		}
		else
		{
		UINT32	lSizeAlignedDmmHeader	= 0;
		UINT32	lSizeAlignedPreBuffer	= 0;
		UINT32	lSizeAlignedNrtRqb		= 0;

			// success
			bResult = TRUE;

			lSizeAlignedDmmHeader	= (UINT32)	fnGetAlignedValue(sizeof(uDMM_HEADER_BLOCK),	lAlignmentIn);
			lSizeAlignedPreBuffer	= (UINT32)	fnGetAlignedValue(sizeof(uPNDEV_NRT_PRE_BUFFER),lAlignmentIn);
			lSizeAlignedNrtRqb		= (UINT32)	fnGetAlignedValue(sizeof(uPNDEV_NRT_RQB),		lAlignmentIn);

			// return number of available NrtRqb
			*pCtrNrtRqbOut = lSizeRealMemNrt / (lSizeAlignedDmmHeader + lSizeAlignedPreBuffer + lSizeAlignedNrtRqb);
		}
	}

	return(bResult);
}
#endif

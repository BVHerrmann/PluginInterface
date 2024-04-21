/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: Device.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Entry points of device
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Device.tmh"
#endif

#ifdef ALLOC_PRAGMA
	#pragma alloc_text (PAGE, fnAddDevice)
	#pragma alloc_text (PAGE, fnPrepareHw)
	#pragma alloc_text (PAGE, fnReleaseHw)
	#pragma alloc_text (PAGE, fnShutdown)
	#pragma alloc_text (PAGE, fnGetPciLocShort)
//	#pragma alloc_text (PAGE, fnCheckSharedHostMemUM)
//	#pragma alloc_text (PAGE, fnMapSharedHostMemUM)
//	#pragma alloc_text (PAGE, fnReleaseSharedHostMemUM)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  AddDevice
//************************************************************************

BOOLEAN fnAddDevice(DEVICE_EXTENSION*		pDevExtIn,
					HANDLE					hOsParIn,
					const BOOLEAN			bOsDpcSupportIn,
					const ePNDEV_BOARD		eBoardIn,
					const uPNDEV_PCI_LOC*	pPciLocIn,
					const UINT32			lDimErrorIn,
					_TCHAR*					sErrorOut)
{
BOOLEAN	bResult = FALSE;

	PAGED_CODE();

	// initialize AppList
	fnInitList(&pDevExtIn->uListApp);

	// preset TimeScaleFactor
	//	- Windows: cannot be passed by OpenDriver because of Plug&Play
	//	- overwritten at first OpenDevice
	pDevExtIn->lTimeScaleFactor = 1;

	// store parameters
	pDevExtIn->hDeviceOs		= hOsParIn;
	pDevExtIn->eBoard			= eBoardIn;
	pDevExtIn->bOsDpcSupport	= bOsDpcSupportIn;

	// update PciLoc
	{
		*((uPNDEV_PCI_LOC*) &pDevExtIn->uPciLoc) = *pPciLocIn;

		if	(_tcscmp(pDevExtIn->uPciLoc.sString, _TEXT("")) != 0)
			// PciLoc as string
		{
			// get PciLocShort
			fnGetPciLocShort(pDevExtIn);
		}
		else
		{
			// set PciLoc
			_RtlStringCchPrintf(pDevExtIn->uPciLoc.sString,
								_countof(pDevExtIn->uPciLoc.sString),
								_TEXT("%u,%u,%u"),
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);

			// set PciLocShort
			_RtlStringCchPrintf(pDevExtIn->sPciLocShort,
								_countof(pDevExtIn->sPciLocShort),
								_TEXT("%u,%u,%u"),
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);

			// set PciLocShortAscii
			RtlStringCchPrintfA(pDevExtIn->sPciLocShortAscii,
								_countof(pDevExtIn->sPciLocShortAscii),
								"%u,%u,%u",
								pDevExtIn->uPciLoc.uNo.lBus,
								pDevExtIn->uPciLoc.uNo.lDevice,
								pDevExtIn->uPciLoc.uNo.lFunction);
		}
	}

	if	(fnStartUpLockRes(	pDevExtIn,
							lDimErrorIn,
							sErrorOut))
		// starting up resources for locking ok
	{
	uPNDEV_CONFIG_SPACE	uConfigSpace;

		// Note:
		//	- at startup a LinkTraining is done
		//	- these tests may cause PCIe errors.
		//	  -> they must be cleared after startup

		if	(fnGetInfoConfigSpace(	pDevExtIn,
									lDimErrorIn,
									&uConfigSpace,
									sErrorOut))
			// getting ConfigSpaceInfo ok
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
//  prepare hardware
//************************************************************************

BOOLEAN fnPrepareHw(DEVICE_EXTENSION*	        pDevExtIn,
					HANDLE				        hOsTransParIn,
                    const ePNDEV_RESET_ACTION	eResetActionIn,
					const UINT32		        lDimErrorIn,
					_TCHAR*				        sErrorOut)
{
BOOLEAN	bResult = FALSE;
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	PAGED_CODE();

	if	(fnBoard_GetHwResKm(pDevExtIn,
							hOsTransParIn,
							lDimErrorIn,
							sErrorOut))
		// getting HwRes of board ok
	{
	BOOLEAN	bResultMap = TRUE;
	UINT32	i = 0;


		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			if	(!fnMapMemPhyToVirtual(	pDevExtIn,
										&pDevExtIn->uHwResKm.uBar[i],
										i,
										lDimErrorIn,
										sErrorOut))
				// error at mapping a range of physical addresses to non-paged virtual memory
			{
				bResultMap = FALSE;

				// leave loop
				break;
			}
			else
			{

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: BAR%u: PhyAdr=0x%llx, VirtualAdr=0x%p, size=0x%x",
									pDevExtIn->sPciLocShortAscii,
									i,
									pDevExtIn->uHwResKm.uBar[i].uPhyAdr.QuadPart,
									pDevExtIn->uHwResKm.uBar[i].pVirtualAdr,
									pDevExtIn->uHwResKm.uBar[i].lSize);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}
		}

		if	(bResultMap)
			// success
		{
			if	(fnBoard_SetPtrHwResKm(	pDevExtIn,
										lDimErrorIn,
										sErrorOut))
				// setting KernelModePtr to HwRes ok
			{
				// KernelModePtr to HwRes ok
				pDevExtIn->bPtrHwResKmOk = TRUE;

				// Note:
				//	- HwReset is done by PowerOn or fnReleaseHw()
				//	- for connecting interrupts (fnConnectInt) HW must be initialized (IntPolarity!)

				if	((*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
                                                                        eResetActionIn,
																		lDimErrorIn,
																		sErrorOut))
					// resetting board ok
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
//  release hardware
//************************************************************************

BOOLEAN fnReleaseHw(DEVICE_EXTENSION*	pDevExtIn,
					const UINT32		lDimErrorIn,
					_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bResultClose	= FALSE;
UINT32	i = 0;

	PAGED_CODE();

	// Note:
	//	- function only called by Windows if there are no more Apps independant if service CloseDevice was called correctly
	//	- SysThread may not be terminated
	//		-> exception if virtual memory is released afterwards
	//		-> cleanup required!

	// preset
	bResultClose = TRUE;

	if	(pDevExtIn->lCtrApp != 0)
		// App exist
	{
	uLIST_HEADER*	pList	= NULL;
	uPNDEV_APP*		pApp	= NULL;

		pList = &pDevExtIn->uListApp;

		while	(!fnListEmpty(pList))
				// AppList not empty
		{
			// get first block of AppList
			pApp = (uPNDEV_APP*) pList->pNext;

			if	(!fnCloseDevice(pDevExtIn,
								pApp,
								lDimErrorIn,
								sErrorOut))
				// error at closing device
				//	- remove block
			{
				bResultClose = FALSE;
			}
		}
	}

	if	(bResultClose)
		// success
	{
	BOOLEAN	bResultReset = TRUE;

		if	(pDevExtIn->bPtrHwResKmOk)
			// KernelModePtr to HwRes ok
		{
			if	(!(*((FN_BOARD_RESET) pDevExtIn->pFnBoard_Reset))(	pDevExtIn,
																	ePNDEV_RESET_ACTION_RELEASE_HW,
																	lDimErrorIn,
																	sErrorOut))
				// error at resetting board
			{
				bResultReset = FALSE;
			}
		}

		if	(bResultReset)
			// success
		{
			if	(fnDisconnectInt(	pDevExtIn,
									lDimErrorIn,
									sErrorOut))
				// disconnecting interrupt ok
			{
				if	(fnShutDownLockRes(	pDevExtIn,
										lDimErrorIn,
										sErrorOut))
					// shutting down resources for locking ok
				{
					// success
					bResult = TRUE;
				}
			}
		}

		for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
		{
			// release virtual memory
			fnReleaseMemVirtual(pDevExtIn,
								&pDevExtIn->uHwResKm.uBar[i]);
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down
//	- called from D0Exit when the device is torn down or when the system is shutdown
//	- called at PASSIVE_LEVEL
//************************************************************************

void fnShutdown(DEVICE_EXTENSION* pDevExtIn)
{
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]	= {0};

	// Note:
	//	- Reset the device to put the device in a known initial state
	//	- WDF has already called out EvtDisable callback to disable the interrupt

	PAGED_CODE();

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[%s]: ---- %s",
						pDevExtIn->sPciLocShortAscii,
						__FUNCTION__);

	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				FALSE,						// logging
				sTraceDetail);

	// WdfInterrupt is already disabled so issue a full reset

	// function called before fnEvtDeviceReleaseHw() -> no ResetDevice necessary
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get PciLocShort
//
//	Note:
//	- don't return an error because PciLocShort is only used for trace
//	- creating PciLocShort may fail e.g. in china if there is no comma
//************************************************************************

void fnGetPciLocShort(DEVICE_EXTENSION* pDevExtIn)
{
_TCHAR*	sComma1	= NULL;
_TCHAR*	sComma2	= NULL;
char	sPciLocAscii[PNDEV_SIZE_STRING_BUF_SHORT]	= {0};
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE]		= {0};

	PAGED_CODE();

	// convert _TCHAR to ASCII string
	fnConvertStringTcharToChar(	pDevExtIn->uPciLoc.sString,
								sPciLocAscii);

	// set TraceDetail
	RtlStringCchPrintfA(sTraceDetail,
						_countof(sTraceDetail),
						"PnDev[x,x,x]: PciLoc=%s",
						sPciLocAscii);

	// set trace
	fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
				ePNDEV_TRACE_CONTEXT_BASIC,
				TRUE,						// logging
				sTraceDetail);

	// Note:
	//	- the nomenclature of PciLoc depends on used language settings
	//	- examples:
	//		- "PCI bus 5, device 10, function 0"
	//		- "PCI Slot 3 (PCI bus 5, device 10, function 0)"
	//		- "PCI-Bus 5, Ger�t 10, Funktion 0"
	//		- "PCI-Steckplatz 4 (PCI-Bus 5, Ger�t 10, Funktion 0)"
/*
	// debug purpose:
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI bus 5, device 10, function 0"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI Slot 3 (PCI bus 5, device 10, function 0)"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI-Bus 5, Ger�t 10, Funktion 0"));
	_RtlStringCchCopy(	sPciLocIn, _countof(sPciLocIn), _TEXT("PCI-Steckplatz 4 (PCI-Bus 5, Ger�t 10, Funktion 0)"));
*/
	// search for first comma
	sComma1 = _tcsstr(pDevExtIn->uPciLoc.sString, _TEXT(","));

	if	(sComma1 == NULL)
		// error
	{
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ########## First comma at PciLocation not found!");

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,						// logging
					sTraceDetail);
	}
	else
	{
		// search for second comma
		sComma2 = _tcsstr((sComma1 + 1), _TEXT(","));

		if	(sComma2 == NULL)
			// error
		{
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[x,x,x]: ########## Second comma at PciLocation not found!");

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);
		}
		else
		{
		_TCHAR*	sBlankBeforeComma1 = NULL;
		UINT32	i = 0;

			// search for blank before comma1
			{
				// temporary: terminate string at comma1
				*sComma1 = _TEXT('\0');

				// search for last blank
				//	- search backward
				sBlankBeforeComma1 = _tcsrchr(pDevExtIn->uPciLoc.sString, _TEXT(' '));

				// undo change at comma1
				*sComma1 = _TEXT(',');
			}

			if	(sBlankBeforeComma1 == NULL)
				// error
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: ########## Blank before comma1 at PciLocation not found!");

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
			UINT32	lLen	= 0;
			_TCHAR	cChar	= _TEXT('\0');
			UINT32	k		= 0;

				lLen = (UINT32)_tcslen(sBlankBeforeComma1);

				for	(i = 0; i < lLen; i++)
					// all characters of string 'blank before comma1'
				{
					cChar = sBlankBeforeComma1[i];

					if	(	(	(cChar >= _TEXT('0'))
							&&	(cChar <= _TEXT('9')))
						||	(cChar == _TEXT(',')))
						//		0..9
						// OR	comma
					{
						// add character to TargetString
						pDevExtIn->sPciLocShort[k] = cChar;
						k++;
					}
				}

				// add terminating null
				pDevExtIn->sPciLocShort[k] = _TEXT('\0');

				// convert _TCHAR to ASCII string
				fnConvertStringTcharToChar(	pDevExtIn->sPciLocShort,
											pDevExtIn->sPciLocShortAscii);

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: PciLocShort computed",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							FALSE,						// logging
							sTraceDetail);
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	get Memory for Device
//************************************************************************

BOOLEAN fnCheckSharedHostMemUM(	uPNDEV_APP*			pAppIn,
								DEVICE_EXTENSION*	pDevExtIn,
								BOOLEAN				bReqHostMem,
								BOOLEAN             bReqSharedHostMem,
								BOOLEAN				bMasterApp,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (bReqHostMem && bReqSharedHostMem)
        // both do not work
    {
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Can not provide host memory AND shared host memory!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else if	(bReqHostMem)
    	// HostMem required
    {
        if	(pDevExtIn->eValidHostMemPool != ePNDEV_VAL_HOST_MEM_INVALID)
			// valid BoardType
		{
		BOOLEAN	bHostMem	= FALSE;
		INT32 	i 			= 0;

			if	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_ALL)
				// all pools are valid
			{
				for	(i=3; i>=0; i--)
					// valid HostMemIndex
				{
					if	(	(!g_uMemSys.uHostMem[i].bInUse)
						&& 	(g_uMemSys.uHostMem[i].pAdrVirt != NULL))
						// memory is available
						// && memory pointer valid - DriverEntry was able to allocate memory
					{
						// success
						bHostMem = TRUE;

						break;
					}
				}
			}

			if	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_FIRST)
				// only first pool valid
			{
				if	(	(!g_uMemSys.uHostMem[0].bInUse)
					&& 	(g_uMemSys.uHostMem[0].pAdrVirt != NULL))
					// memory0 is available
					// && memory pointer valid - DriverEntry was able to allocate memory
				{
					// success
					bHostMem = TRUE;
				}
			}

			if 	(bHostMem)
				// got HostMemory
			{
					pAppIn->bUsesHostMem 	= TRUE;
					bResult 				= TRUE;
			}
			else
			{
				// invalid board type
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Can not provide contiguous shared host memory for device: The corresponding resources are already in use or Memory allocation failed at DriverEntry!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
		}
		else
		{
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Can not provide contiguous shared host memory for device: Board type not supported!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
    }
    else if (bReqSharedHostMem)
        // SharedHostMem required
    {
        if  (pDevExtIn->eValidHostMemPool != ePNDEV_VAL_HOST_MEM_INVALID)
            // valid BoardType
        {
        BOOLEAN bSharedHostMem    = FALSE;
        INT32   i           = 0;

            for (i=0; i>=0; i--)
                // valid SharedHostMemIndex
            {
                if  (g_uMemSys.uSharedHostMem[i].pAdrVirt != NULL)
                    // memory pointer valid - DriverEntry was able to allocate memory
                {
                    // success
                    bSharedHostMem = TRUE;

                    break;
                }
            }

            if  (bSharedHostMem)
                // got HostMemory
            {
                    pAppIn->bUsesSharedHostMem  = TRUE;
                    bResult                     = TRUE;
            }
            else
            {
                // invalid board type
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Can not provide contiguous shared host memory (SHARED) for device: The corresponding resources are already in use or Memory allocation failed at DriverEntry!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
        }
        else
        {
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Can not provide contiguous shared host memory (SHARED) for device: Board type not supported!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
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
//	Map SharedHostMem UserMode
//************************************************************************

BOOLEAN fnMapSharedHostMemUM(	uPNDEV_APP*							pAppIn,
								DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
								uPNDEV_SHARED_HOST_SDRAM*			pSharedHostSdram,
								_TCHAR*								sErrorOut)
{
BOOLEAN bResult 		= FALSE;
INT32	lFoundHostMem 	= -1;
INT32   lFoundSharedHostMem = -1;

    if  (pAppIn->bUsesHostMem && pAppIn->bUsesSharedHostMem)
        // both does not work
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Cannot provide HostMem AND SharedHostMem!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }    
    else if	(pAppIn->bUsesHostMem)
    	// HostMem available
    {
    INT32 i = 0;

    	for	(i = 3; i >= 0; i--)
    		// valid HostMemIndex
    	{
            if  (   (!g_uMemSys.uHostMem[i].bInUse)
                &&  (g_uMemSys.uHostMem[i].pAdrVirt != NULL))
                // memory is available
                // && memory pointer valid - DriverEntry was able to allocate memory
    		{
	            if	(	((pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_FIRST) && (i == 0))	// only HostMemory0 valid
	            	||	(pDevExtIn->eValidHostMemPool == ePNDEV_VAL_HOST_MEM_ALL))					// all HostMemories valid
	            	// valid HostMemory
	            {
	            	if 	(pAppIn->lUsesHostMemIndex == (UINT32)i)
	            		// right Index
	            	{
						if	(pInternalIn->uHostMemUm.uAdrUser.pPtr != NULL)
							// valid ptr
						{
							g_uMemSys.uHostMem[i].uHwResUserSpace.uAdrUser.pPtr	= pInternalIn->uHostMemUm.uAdrUser.pPtr;
							g_uMemSys.uHostMem[i].uHwResUserSpace.lSize 		= pInternalIn->uHostMemUm.lSize;
							g_uMemSys.uHostMem[i].uHwResUserSpace.uMdl.pPtr		= pInternalIn->uHostMemUm.uMdl.pPtr;

							// success
							lFoundHostMem = i;

							break;
						}
						else
						{
							// set ErrorString
							fnBuildString(	_TEXT(__FUNCTION__),
											_TEXT("(): No valid ptr from MappingVirtualToUM!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

							break;
						}
	            	}
	            	else
	            	{
	            		// set ErrorString
	            		fnBuildString(	_TEXT(__FUNCTION__),
	            						_TEXT("(): No valid lUsesHostMemIndex!"),
	            						NULL,
	            						lDimErrorIn,
	            						sErrorOut);
	            	}
	            }
    		}
    	}

    	if	(lFoundHostMem > -1)
    		// success
    	{
    		if 	(g_uMemSys.uHostMem[lFoundHostMem].pAdrVirt != NULL)
    			// ptr valid
    		{
    			// set UsedFlag
    			g_uMemSys.uHostMem[lFoundHostMem].bInUse = TRUE;

    			// set SharedHostSdram
    			pSharedHostSdram->uBase.pPtr   	=  g_uMemSys.uHostMem[lFoundHostMem].uHwResUserSpace.uAdrUser.pPtr;
    			pSharedHostSdram->lPhyAdr      	=  g_uMemSys.uHostMem[lFoundHostMem].lPhyAdr;
    			pSharedHostSdram->lSize        	=  g_uMemSys.uHostMem[lFoundHostMem].uHwResUserSpace.lSize;
    			pAppIn->lUsesHostMemIndex		=  (UINT32)lFoundHostMem;

    			
				switch	(pDevExtIn->eAsic)
						// AsicType
				{


					default:
					{
	    				// success
	    				bResult = TRUE;	

						break;
					}
				}
    		}
    	}
    }
    else if (pAppIn->bUsesSharedHostMem)
        // SharedHostMem available
    {
    INT32 i = 0;

        for (i = 0; i >= 0; i--)
            // valid SharedHostMemIndex
        {
            if  (g_uMemSys.uSharedHostMem[i].pAdrVirt != NULL)
                // memory pointer valid - DriverEntry was able to allocate memory
            {
                if  (pAppIn->lUsesSharedHostMemIndex == (UINT32)i)
                    // right Index
                {
                    // usually we would set the uHwResUserSpace structure here
                    // with the given values from pInternalIn->uHostMemUm
                    // but we already set the values during AllocateContigousSharedMemory

                    // success
                    lFoundSharedHostMem = i;
                }
                else
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): No valid lUsesSharedHostMemIndex!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
            }
        }

        if  (lFoundSharedHostMem > -1)
            // success
        {
            if  (g_uMemSys.uSharedHostMem[lFoundSharedHostMem].pAdrVirt != NULL)
                // ptr valid
            {
                // set UsedFlag
                g_uMemSys.uSharedHostMem[lFoundSharedHostMem].bInUse = TRUE;
                
                // increment the Use counter
                g_uMemSys.uSharedHostMem[lFoundSharedHostMem].lUseCounter++;

                // set SharedHostSdram
                pSharedHostSdram->uBase.pPtr        =  g_uMemSys.uSharedHostMem[lFoundSharedHostMem].uHwResUserSpace.uAdrUser.pPtr;
                pSharedHostSdram->lPhyAdr           =  g_uMemSys.uSharedHostMem[lFoundSharedHostMem].lPhyAdr;
                pSharedHostSdram->lSize             =  g_uMemSys.uSharedHostMem[lFoundSharedHostMem].uHwResUserSpace.lSize;
                pAppIn->lUsesSharedHostMemIndex     =  (UINT32)lFoundSharedHostMem;
                
                switch  (pDevExtIn->eAsic)
                        // AsicType
                {

                    default:
                    {
                        // success
                        bResult = TRUE; 

                        break;
                    }
                }
            }
        }
    }
    else
    {
    	// success
    	bResult = TRUE;
    }

    return (bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	Release SharedHostMem UserMode
//************************************************************************

void fnReleaseSharedHostMemUM(	DEVICE_EXTENSION*	pDevExtIn,
								uPNDEV_APP*			pAppIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{

			if(pAppIn->bUsesHostMem)
			{
				fnReleaseMemUser(	pDevExtIn,       // devExt not used
									&(g_uMemSys.uHostMem[pAppIn->lUsesHostMemIndex].uHwResUserSpace));

				// reset ptr
				g_uMemSys.uHostMem[pAppIn->lUsesHostMemIndex].bInUse	= FALSE;
				pAppIn->bUsesHostMem									= FALSE;
				pAppIn->lUsesHostMemIndex 								= 0;
				
				if(pDevExtIn->eAsic == ePNDEV_ASIC_HERA)
				{
					// reset pci master window
					fnBoard_ConfigPciMaster(pDevExtIn,
											0,
											0,
											0,
											lDimErrorIn,
											sErrorOut);
				}
			}
    
    if(pAppIn->bUsesSharedHostMem)
    {
        // decrease the number of devices which are using the SharedHostMem
        g_uMemSys.uSharedHostMem[pAppIn->lUsesSharedHostMemIndex].lUseCounter--;
        
        if  (g_uMemSys.uSharedHostMem[pAppIn->lUsesSharedHostMemIndex].lUseCounter == 0)
            // this was the last device
        {
            // reset ptr
            g_uMemSys.uSharedHostMem[pAppIn->lUsesSharedHostMemIndex].bInUse    = FALSE;
        }
        
        pAppIn->bUsesSharedHostMem                                    = FALSE;
        pAppIn->lUsesSharedHostMemIndex                               = 0;
        
        if(pDevExtIn->eAsic == ePNDEV_ASIC_HERA)
        {
            // reset pci master window
            fnBoard_ConfigPciMaster(pDevExtIn,
                                    0,
                                    0,
                                    0,
                                    lDimErrorIn,
                                    sErrorOut);
        }
    }
}


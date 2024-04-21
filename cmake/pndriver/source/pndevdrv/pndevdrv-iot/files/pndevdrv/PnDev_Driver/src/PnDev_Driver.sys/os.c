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
/*  F i l e               &F: os.c                                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//  Code for adaption of SYS to used OS
//
//****************************************************************************/

#include "precomp.h"                                                        // environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
    #include "os.tmh"
#endif

#ifdef ALLOC_PRAGMA
    #pragma alloc_text (INIT, DriverEntry)
    #pragma alloc_text (PAGE, fnEvtDriverUnload)
    #pragma alloc_text (PAGE, fnEvtDriverContextCleanup)
    #pragma alloc_text (PAGE, fnEvtDeviceAdd)
//  #pragma alloc_text (PAGE, fnEvtDeviceD0Entry)                           // see function
    #pragma alloc_text (PAGE, fnEvtDeviceD0Exit)
    #pragma alloc_text (PAGE, fnEvtDevicePrepareHw)
    #pragma alloc_text (PAGE, fnEvtDeviceReleaseHw)
//  #pragma alloc_text (PAGE, fnEvtIoctl)
//  All driver functions are by default nonpaged. see https://docs.microsoft.com/en-us/windows-hardware/drivers/kernel/making-drivers-pageable
//  Defining #pragma alloc_text (PAGE, ...) makes function pageable!
//  Don't permit paging for isr, dpc and timer call back functions!
//  When a function is marked pagable and the code section is paged out, it will generate a page fault!
//  -> page fault in isr, dpc or timer call back leads to blue screen!
//  #pragma alloc_text (PAGE, fnEvtIsr)
//  #pragma alloc_text (PAGE, fnEvtDpc)
//  #pragma alloc_text (PAGE, fnTickEvtTimer)
//  #pragma alloc_text (PAGE, fnEvtIntEnable)
//  #pragma alloc_text (PAGE, fnEvtIntDisable)
//  #pragma alloc_text (PAGE, fnThreadSys)
//------------------------------------------------------------------------
//  #pragma alloc_text (PAGE, fnStartUpLockRes)
//  #pragma alloc_text (PAGE, fnShutDownLockRes)
//  #pragma alloc_text (PAGE, fnAcquireSpinLockIrq)
//  #pragma alloc_text (PAGE, fnReleaseSpinLockIrq)
//  #pragma alloc_text (PAGE, fnAcquireSpinLock)
//  #pragma alloc_text (PAGE, fnReleaseSpinLock)
//  #pragma alloc_text (PAGE, fnEnterCritSec)
//  #pragma alloc_text (PAGE, fnLeaveCritSec)
//  #pragma alloc_text (PAGE, fnStartUpThreadResSys)
//  #pragma alloc_text (PAGE, fnShutDownThreadResSys)
//  #pragma alloc_text (PAGE, fnStartUpThreadResDll)
//  #pragma alloc_text (PAGE, fnShutDownThreadResDll)
//  #pragma alloc_text (PAGE, fnSetEventThreadDll)
//  #pragma alloc_text (PAGE, fnGetIoctlBuf)
//  #pragma alloc_text (PAGE, fnIoctlGetDeviceInfoSys)
//  #pragma alloc_text (PAGE, fnIoctlGetDeviceHandleSys)
//  #pragma alloc_text (PAGE, fnIoctlAllocEventId)
//  #pragma alloc_text (PAGE, fnIoctlFreeEventId)
//  #pragma alloc_text (PAGE, fnIoctlDoNetProfi)
//  #pragma alloc_text (PAGE, fnConnectInt)
//  #pragma alloc_text (PAGE, fnDisconnectInt)
//  #pragma alloc_text (PAGE, fnRegisterIsr)
//  #pragma alloc_text (PAGE, fnQueueDpc)
//  #pragma alloc_text (PAGE, fnAllocMemNonPaged)
//  #pragma alloc_text (PAGE, fnFreeMemNonPaged)
//  #pragma alloc_text (PAGE, fnMapMemPhyToVirtual)
//  #pragma alloc_text (PAGE, fnReleaseMemVirtual)
//  #pragma alloc_text (PAGE, fnMapMemVirtualToUser)
//  #pragma alloc_text (PAGE, fnReleaseMemUser)
//  #pragma alloc_text (PAGE, fnDelayThread)
//  #pragma alloc_text (PAGE, fnGetClock)
//  #pragma alloc_text (PAGE, fnComputeRunTimeUsec)
//  #pragma alloc_text (PAGE, fnSetTrace)
//  #pragma alloc_text (PAGE, fnCheckIrql)
//  #pragma alloc_text (PAGE, fnGetResListConfigSpace)
//  #pragma alloc_text (PAGE, fnGetInfoConfigSpace)
//  #pragma alloc_text (PAGE, fnGetBarConfigSpace)
//  #pragma alloc_text (PAGE, fnGetExtendedConfigSpace)
//  #pragma alloc_text (PAGE, fnPrepareConfigSpaceHwReset)
//  #pragma alloc_text (PAGE, fnRestoreConfigSpaceHwReset)
//  #pragma alloc_text (PAGE, fnScanDevice)
//  #pragma alloc_text (PAGE, fnGetBoardType)
//  #pragma alloc_text (PAGE, fnGetFreeIndexEventHandleTable)
//  #pragma alloc_text (PAGE, fnReleaseIndexEventHandleTable)
//  #pragma alloc_text (PAGE, fnMmAllocateContiguousMemory)
//  #pragma alloc_text (PAGE, fnMmFreeContiguousMemory)
#endif

//########################################################################
//  Variables
//########################################################################

uPNDEV_MEM_SYS g_uMemSys;

//------------------------------------------------------------------------
//------------------------------------------------------------------------
static uPNDEV_VAR_SYS g_uVarSys;

static struct pci_device_id g_pci_drv_tbl[] = {
		PNDEV_SCAN_DEVICES,
		{ 0, }
};

static struct pci_driver g_pci_drv = {
		.name= PNDEV_PCI_DRV_NAME,
		.id_table= g_pci_drv_tbl,
		.probe= fnEvtDeviceProbe,
		.remove= fnEvtDeviceRemove,
};

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverEntry
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

int DriverEntry(void)
{
int     lResult     = LINUX_ERROR;
int     lResultTmp  = LINUX_ERROR;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset variables
    {
        RtlZeroMemory(  &g_uVarSys,
                        sizeof(uPNDEV_VAR_SYS));

        // initialize DeviceList
        fnInitList(&g_uVarSys.uListDevice);
    }

    // set function operaterations
    g_uVarSys.uFctOps.owner             = THIS_MODULE;
    g_uVarSys.uFctOps.unlocked_ioctl    = fnEvtIoctl;
    g_uVarSys.uFctOps.compat_ioctl		= fnEvtIoctl;
    g_uVarSys.uFctOps.mmap              = fnEvtMmap;
    g_uVarSys.uFctOps.open              = fnOpenDriver;
    g_uVarSys.uFctOps.release           = fnCloseDriver;

    // get MajorNumber and MinorNumber in uDev
    lResultTmp = alloc_chrdev_region(   &g_uVarSys.uDev,
                                        0,
                                        1,
                                        PNDEV_DRV_NAME);

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
        _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at register_chrdev()!"), _TRUNCATE);

        // set trace by returned ErrorString
        fnSetTraceFctError( NULL,
                            sError);
    }
    else
    {

        // create class
        g_uVarSys.pDrvClass = class_create(	THIS_MODULE,
                                    		PNDEV_DRV_NAME);

        if (g_uVarSys.pDrvClass == NULL)
        {
            // unregister region
            unregister_chrdev_region(   g_uVarSys.uDev,
                                        1);

            // set ErrorString
            _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
            _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at class_create()!"), _TRUNCATE);

            // set trace by returned ErrorString
            fnSetTraceFctError( NULL,
                                sError);
        }
        else
        {
			// create device
			g_uVarSys.pDrvDevice = device_create(	g_uVarSys.pDrvClass,
													NULL,
													g_uVarSys.uDev,
													NULL,
													PNDEV_DRV_NAME);

            if  (g_uVarSys.pDrvDevice == NULL)
                // error
            {
                // destroy class
                class_destroy(g_uVarSys.pDrvClass);

                // unregister region
                unregister_chrdev_region(   g_uVarSys.uDev,
                                            1);

                // set ErrorString
                _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
                _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at device_create()!"), _TRUNCATE);

                // set trace by returned ErrorString
                fnSetTraceFctError( NULL,
                                    sError);
            }
            else
            {
            struct cdev* pDrvCdev = &g_uVarSys.uDrvCdev;

                // init Device with corresponding FunctionOperations
                cdev_init(pDrvCdev, &g_uVarSys.uFctOps);

                // add Device
                lResultTmp = cdev_add(  pDrvCdev,
                                        g_uVarSys.uDev,
                                        1);

                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // destroy device
                    device_destroy( g_uVarSys.pDrvClass,
                                    g_uVarSys.uDev);

                    // destroy class
                    class_destroy(g_uVarSys.pDrvClass);

                    // unregister region
                    unregister_chrdev_region(   g_uVarSys.uDev,
                                                1);

                    // set ErrorString
                    _tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
                    _tcsncat_s( sError, _countof(sError), _TEXT("(): Error at device_create()!"), _TRUNCATE);

                    // set trace by returned ErrorString
                    fnSetTraceFctError( NULL,
                                        sError);
                }
                else
                {
                    // register pci driver
					lResultTmp = pci_register_driver(&g_pci_drv);

					if	(lResultTmp < 0)
						// error
                    {
						// destroy device
						device_destroy(	g_uVarSys.pDrvClass,
										g_uVarSys.uDev);

						// destroy class
						class_destroy(g_uVarSys.pDrvClass);

						// unregister region
						unregister_chrdev_region(	g_uVarSys.uDev,
													1);

						// set ErrorString
						_tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
						_tcsncat_s( sError, _countof(sError), _TEXT("(): Error at device_create()!"), _TRUNCATE);

						// set trace by returned ErrorString
						fnSetTraceFctError(	NULL,
											sError);
                    }
                    
                    else
                    {
						if  (!fnCreateMutex(_countof(sError),
											&g_uVarSys.uCritSecIoctlDriver,
											sError))
							// creating DriverMutex ok
						{
							// unregister pci driver
							pci_unregister_driver(&g_pci_drv);
							
							// delete device
							cdev_del(&g_uVarSys.uDrvCdev);

							// destroy device
							device_destroy(	g_uVarSys.pDrvClass,
											g_uVarSys.uDev);

							// destroy class
							class_destroy(  g_uVarSys.pDrvClass);

							// unregister region
							unregister_chrdev_region(	g_uVarSys.uDev,
														1);
                            
							// set trace by returned ErrorString
							fnSetTraceFctError(	NULL,
												sError);
                        }
						else
                        {
                            // success
                            //  - return index for first opening
                                lResult = LINUX_OK;

                            // allocate ContigousMemory
                            {
                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                768*1024*1024,
                                                                &(g_uMemSys.uHostMem[0]));   // get memory below 768MB

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[1]));

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[2]));

                                fnMmAllocateContiguousMemory(   16*1024*1024,               // 16MB
                                                                0,
                                                                0xFFFFFFFF,                 // get memoryblocks with address within 32bit
                                                                &(g_uMemSys.uHostMem[3]));
                            }
                        }
                    }
                }
            }
        }
    }

    return(lResult);
}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverUnload
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

void fnEvtDeviceRemove(struct pci_dev* pPciDevIn)
{
BOOLEAN			bResult = FALSE;
char 			sTraceDetail[PNDEV_SIZE_STRING_TRACE]  = {0};

	if	(g_uVarSys.lCtrDevice != 0)
		// device exist
	{
	uLIST_HEADER*		pList	= NULL;
	DEVICE_EXTENSION*	pDevExt	= NULL;

		pList = &g_uVarSys.uListDevice;

		while	(!fnListEmpty(pList))
				// search in DeviceList
        {
			// get first block of DeviceList
			pDevExt = (DEVICE_EXTENSION*) pList->pNext;
            
			if	(pDevExt->pPciDev == pPciDevIn)
				// found pci device
			{
				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: Removing Device",
									pDevExt->sPciLocShortAscii);

				/* set trace */
				fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,                       /* logging */
							sTraceDetail);
				
				// call CBF for releasing hardware
				fnEvtDeviceReleaseHw(pDevExt);
				
				// remove DeviceExtension from DeviceList
				fnRemoveBlkFromList(&g_uVarSys.uListDevice,
									((uLIST_HEADER*) pDevExt));
    
				g_uVarSys.lCtrDevice--;
    
				// free DeviceExtension
				fnFreeMemNonPaged(	pDevExt,
									TAG('I','n','i','t'));
				
				bResult = TRUE;
				break;
			}
			// next list entry
			pList = pList->pNext;
        }
	}
	if(!bResult)
	{
		// set ErrorString
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%u,%u,%u]: Device not found at fnEvtDeviceRemove()",
							pPciDevIn->bus->number,			// Bus
							PCI_SLOT(pPciDevIn->devfn),		// Device
							PCI_FUNC(pPciDevIn->devfn));	// Fucntion

		/* set trace */
		fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,                       /* logging */
					sTraceDetail);
	}
}

void fnEvtDriverUnload(void)
{
_TCHAR	sError[PNDEV_SIZE_STRING_BUF] = {0};

	// release all devices
	{
		// unregister pci driver
		pci_unregister_driver(&g_pci_drv);
		
		// all devices should have been released by os with fnEvtDeviceRemove()!
		if	(g_uVarSys.lCtrDevice != 0)
			// device exists
		{
			// set ErrorString
			_tcscpy_s(  sError, _countof(sError), _TEXT(__FUNCTION__));
			_tcsncat_s( sError, _countof(sError), _TEXT("(): g_uVarSys.lCtrDevice != 0"), _TRUNCATE);
			
			// set error trace
			fnSetTraceFctError(	NULL,
								sError);
		}
	}

    // free HostMem
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[0]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[1]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[2]));
    fnMmFreeContiguousMemory(&(g_uMemSys.uHostMem[3]));

    if  (!fnDeleteMutex(&g_uVarSys.uCritSecIoctlDriver,
                        _countof(sError),
                        sError))
        // error at deleting DriverMutex
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( NULL,
                            sError);
    }

    // delete driver
    {
        // delete device
        cdev_del(&g_uVarSys.uDrvCdev);

        // destroy device
        device_destroy( g_uVarSys.pDrvClass,
                        g_uVarSys.uDev);

        // destroy class
        class_destroy(  g_uVarSys.pDrvClass);

        // unregister region
        unregister_chrdev_region(   g_uVarSys.uDev,
                                    1);
    }

    // preset variables
    {
        RtlZeroMemory(  &g_uVarSys,
                        sizeof(uPNDEV_VAR_SYS));

        // initialize DeviceList
        fnInitList(&g_uVarSys.uListDevice);
    }

}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DriverContextCleanup
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for AddDevice
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnEvtDeviceAdd( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_BOARD        eBoardIn,
                        UINT32              lNoBusIn,
                        UINT32              lNoDeviceIn,
                        UINT32              lNoFunctionIn)
{
BOOLEAN         bResult = FALSE;

uPNDEV_PCI_LOC  uPciLoc;
_TCHAR          sError[PNDEV_SIZE_STRING_BUF] = {0};

    // preset
    RtlZeroMemory(  &uPciLoc,
                    sizeof(uPNDEV_PCI_LOC));

    // set PciLoc
    uPciLoc.uNo.lBus        = lNoBusIn;
    uPciLoc.uNo.lDevice     = lNoDeviceIn;
    uPciLoc.uNo.lFunction   = lNoFunctionIn;

    if  (!fnAddDevice(  pDevExtIn,
                        NULL,
                        FALSE,          // LINUX does not support DPC
                        eBoardIn,
                        &uPciLoc,
                        _countof(sError),
                        sError))
        // error at AddDevice
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        // success
        bResult = TRUE;

        // Note:
        //  - now PciLocShort is valid and can be used for RtlStringCchPrintfA() / fnSetTrace()
    }

    return(bResult);

}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for start of device D0 state
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for end of device D0 state
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for preparing hardware
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnEvtDevicePrepareHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult         = FALSE;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnPrepareHw(  pDevExtIn,
                        NULL,
                        ePNDEV_RESET_ACTION_REINIT_HW,
                        _countof(sError),
                        sError))
        // error at preparing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {  
    	if  (!fnConnectInt( pDevExtIn,
							NULL,
							NULL,
                            _countof(sError),
                            sError))
            // error at connecting interrupt
            //  - Linux: do it after fnPrepareHw() (HW is initialized, otherwise IntPolarity may be wrong!)
        {
            // set trace by returned ErrorString
            fnSetTraceFctError( pDevExtIn,
                                sError);
        }
        else
        {
            // success
            bResult = TRUE;
        }
    }
            
    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for releasing hardware
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnEvtDeviceReleaseHw(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;
_TCHAR  sError[PNDEV_SIZE_STRING_BUF] = {0};

    if  (!fnReleaseHw(  pDevExtIn,
                        _countof(sError),
                        sError))
        // error at releasing hardware
    {
        // set trace by returned ErrorString
        fnSetTraceFctError( pDevExtIn,
                            sError);
    }
    else
    {
        // success
        bResult = TRUE;

        // undo pci init
        {
            // release regions
            pci_release_regions(pDevExtIn->pPciDev);

            // disable device
            pci_disable_device(pDevExtIn->pPciDev);
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for IoControl / IoDeviceControl
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
long fnEvtIoctl (	struct file* pFdIn,
                 	unsigned int lCmdIn,
                 	unsigned long lArgIn)
{
long                lResult         = LINUX_ERROR;
BOOLEAN             bResultTmp      = FALSE;
uPNDEV_IOCTL_ARG    uIoctlArg;
uPNDEV64_PTR_VOID   uInBuffer;
uPNDEV64_PTR_VOID   uOutBuffer;
char sTraceDetail[PNDEV_SIZE_STRING_TRACE]  = {0};

        // retrieve ioctl buffer
        bResultTmp = RetrieveIoctlBuffer(   lCmdIn,
                                            lArgIn,
                                            &uIoctlArg,
                                            &uInBuffer,
                                            &uOutBuffer);

        if  (bResultTmp == FALSE)
            // error
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at RetrieveIoctlBuffer!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            uPNDEV_IOCTL_OUT*   pOutCommon      = NULL;
            ePNDEV_IOCTL        eResultIoctl    = ePNDEV_IOCTL_INVALID;

            pOutCommon = (uPNDEV_IOCTL_OUT*) uOutBuffer.pPtr;

            // do IoControl
            eResultIoctl = fnDoIoctl(   uIoctlArg.uDeviceSys.hHandle,   // pDevExt
                                        NULL,                           // no OsPar
                                        lCmdIn,                         // Ioctl
                                        uIoctlArg.lSizeInputBuf,
                                        uIoctlArg.lSizeOutputBuf,
                                        uInBuffer.pPtr,
                                        uOutBuffer.pPtr);

            if  (eResultIoctl == ePNDEV_IOCTL_PENDING)
                // request pending
                //  - request should be stored for a later confirmation
            {
                eResultIoctl = ePNDEV_IOCTL_ERROR_BY_STRING;

                // set ErrorString
                fnBuildString(  NULL,
                                _TEXT("Invalid UseCase (request pending)!"),
                                NULL,
                                _countof(pOutCommon->sError),
                                pOutCommon->sError);
            }

            switch  (eResultIoctl)
                    // result
            {
                case ePNDEV_IOCTL_OK:
                case ePNDEV_IOCTL_ERROR_BY_STRING:
                {
                    // nothing to do

                    break;
                }
                case ePNDEV_IOCTL_ERROR_REQ:
                case ePNDEV_IOCTL_ERROR_BUF_SIZE:
                case ePNDEV_IOCTL_ERROR_BUF_IN:
                case ePNDEV_IOCTL_ERROR_BUF_OUT:
                default:
                {
                _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                    // set ErrorDetail
                    _RtlStringCchPrintf(sErrorDetail,
                                        _countof(sErrorDetail),
                                        _TEXT("(): Error=0x%x!"),
                                        (UINT32) eResultIoctl);

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    sErrorDetail,
                                    NULL,
                                    _countof(pOutCommon->sError),
                                    pOutCommon->sError);

                    break;
                }
            }

            // return ioctl buffer
            bResultTmp = ReturnIoctlBuffer( &uIoctlArg,
                                            &uInBuffer,
                                            &uOutBuffer);

            if  (bResultTmp == FALSE)
                // error
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at RetrieveIoctlBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // success
                lResult = LINUX_OK;
            }
        }

    return(lResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for ISR
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
irqreturn_t fnEvtIsr(   int    lMsgIdMsiIn,
                        void*  pFctArgIn)
{
DEVICE_EXTENSION*   pDevExt     = (DEVICE_EXTENSION*) pFctArgIn;
BOOLEAN             bIntHandled = FALSE;

    // increase performance for adonis
    // if TopLevel-ICU, call Evnet immediately
    if  (pDevExt->eIcu == ePNDEV_ICU_TOP_LEVEL_PCI)
        // TopLevel-ICU = PCI-ICU
    {
    }
    else
    {
    	if	(pDevExt->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
    		// legacy interrupt
    	{
        // do ISR
    	    //  - ReturnValue indicates if interrupt is caused by this device
            bIntHandled = fnIsrLeg( pDevExt,
                                    NULL,
                                    lMsgIdMsiIn);
    	}
    	else if	(	(pDevExt->eIntMode == ePNDEV_INTERRUPT_MODE_MSI)
    			||	(pDevExt->eIntMode == ePNDEV_INTERRUPT_MODE_MSIX))
    		// msi/msix interrupt
    	{    	    
    	    UINT32 i = 0;
    	    
    	    // do ISR
    	    //  - ReturnValue indicates if interrupt is caused by this device
    	    
    	    // in Linux we have to convert the given lMsgIdMsiIn to the 0-based index of our arrays
    	    // so we are looking for the lMsgIdMsiIn in our uMsixEntry struct and using this index from now on
    	    for(i = 0; i < PNDEV_MSIX_VECTOR_COUNT_MAX; i++)
    	    {
    	        if(pDevExt->uMsixEntry[i].vector == lMsgIdMsiIn)
    	            break;
    	    }    	    
    	    
    	    if  (i != PNDEV_MSIX_VECTOR_COUNT_MAX)
    	        // given vector was in range
    	    {
                bIntHandled = fnIsrMsix(  pDevExt,
                                          NULL,
                                          i);
    	    }
    	}
    }

    if (bIntHandled)
    {
        return IRQ_HANDLED;
    }
    else
    {
        return IRQ_NONE;
    }
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for DPC (deferred procedure call)
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  CBF for TimerEvent
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable interrupt
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  disable interrupt
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  SysThread
//  - each device has its own thread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#ifdef PNDEV_GET_RUNTIME_THREAD
    UINT32          g_lCtrEntry = 0;
    uPNDEV_CLOCK    g_uClockStart;
    UINT32          g_lArrayRuntime_Usec[10] = {0,0,0,0,0,0,0,0,0,0};
#endif

void fnThreadSys(unsigned long arg)
{
UINT32  lTimeCycle_Sec = 0;
UINT32  lTimeCycle_Msec = 0;
DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) arg;

    if  (!pDevExt->bSysThreadTimerStop)
        // don't TimerStop
    {
            //calculate CycleTime in sec/nsec
            lTimeCycle_Sec  = (pDevExt->lTimeCycleThreadSys_10msec / 100);
            lTimeCycle_Msec = (pDevExt->lTimeCycleThreadSys_10msec % 100) * 10; // *10 -> msec,...

            //set timer to next expirationtime
            mod_timer(&pDevExt->uSysThreadTimer,
                      jiffies + (HZ * lTimeCycle_Sec) + (HZ * lTimeCycle_Msec)/1000); // restarting timer


        #ifdef PNDEV_GET_RUNTIME_THREAD
        {
            if  (g_lCtrEntry == 0)
                // first entry
            {
                // get clock
                fnGetClock(&g_uClockStart);
            }
            else if (g_lCtrEntry < 10)
            {
            UINT32 lRuntime_Usec = 0;

                // compute Runtime in usec
                lRuntime_Usec = fnComputeRunTimeUsec(&g_uClockStart);

                g_lArrayRuntime_Usec[g_lCtrEntry] = lRuntime_Usec;
            }
            else
            {
                g_lCtrEntry--;
                g_lCtrEntry++;
            }

            g_lCtrEntry++;
        }
        #endif

        // do actions of SysThread
        fnDoThreadSys(pDevExt,
                      pDevExt->lTimeCycleThreadSys_10msec);
    }
}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources for locking
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnStartUpLockRes(   DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - DPC is directly called by ISR
    //      -> hSpinLockDpc not required

    if  (fnCreateMutex( lDimErrorIn,
                        &pDevExtIn->uCritSecIoctl,
                        sErrorOut))
        // creating DeviceMutex ok
    {
        if  (fnCreateMutex( lDimErrorIn,
                            &pDevExtIn->uCritSecPage,
                            sErrorOut))
            // creating DeviceMutex ok
        {
            if  (fnCreateMutex( lDimErrorIn,
                                &pDevExtIn->uCritSecAppList,
                                sErrorOut))
                // creating DeviceMutex ok
            {
            int32_t lLoop       = 0;
            
                for(lLoop = 0; lLoop < PNDEV_MSIX_VECTOR_COUNT_MAX; lLoop++)
                {            
                    // initialize SpinLockIrq
                    spin_lock_init( &pDevExtIn->uSpinlockIrq[lLoop]);
                }
                
                // success
                bResult = TRUE;
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down resources for locking
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnShutDownLockRes(  DEVICE_EXTENSION*   pDevExtIn,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (fnDeleteMutex( &pDevExtIn->uCritSecIoctl,
                        lDimErrorIn,
                        sErrorOut))
        // deleting DeviceMutex ok
    {
        if  (fnDeleteMutex( &pDevExtIn->uCritSecPage,
                            lDimErrorIn,
                            sErrorOut))
            // deleting DeviceMutex ok
        {
            if  (fnDeleteMutex( &pDevExtIn->uCritSecAppList,
                                lDimErrorIn,
                                sErrorOut))
                // deleting DeviceMutex ok
            {
                // there is no spin_lock_destroy in linux kernel
                bResult = TRUE;
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  acquire SpinLockIrq
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnAcquireSpinLockIrq(DEVICE_EXTENSION*	pDevExtIn,
						  UINT32			lIndexIn)
{
    // disable IntLine at IO-APIC
    spin_lock_irq(&pDevExtIn->uSpinlockIrq[lIndexIn]);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  release SpinLockIrq
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnReleaseSpinLockIrq(DEVICE_EXTENSION*	pDevExtIn,
						  UINT32			lIndexIn)
{
    // enable IntLine at IO-APIC
    spin_unlock_irq(&pDevExtIn->uSpinlockIrq[lIndexIn]);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  acquire SpinLock
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnAcquireSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Linux this function has now effect
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  release SpinLock
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnReleaseSpinLock( DEVICE_EXTENSION*   pDevExtIn,
                        ePNDEV_SPIN_LOCK    eSpinLockIn)
{
    // Note:
    //  - at Linux this function will do nothing
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  enter CriticalSection
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnEnterCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
UINT32  lSubClass = 0;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;      lSubClass = 1;  break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;       lSubClass = 2;  break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    lSubClass = 2;  break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // mutex_lock_nested is neccasary, if more than one mutex needs to be holded at one time.
        // IOCTL mutex protects ioctl on device.
        // PAGE and APP_LIST mutexes can be taken while executing an ioctl.
        // Therefore IOCTL, PAGE and APP_LIST mutexes have to be locked with mutex_lock_nested.
        mutex_lock_nested((struct mutex*) hCritSec, lSubClass);
        // mutex_lock_nested has no return value
    }
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  leave CriticalSection
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnLeaveCritSec(DEVICE_EXTENSION*   pDevExtIn,
                    ePNDEV_CRIT_SEC_SYS eCritSecIn)
{
HANDLE  hCritSec = NULL;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    switch  (eCritSecIn)
            // CritSec
    {
        case ePNDEV_CRIT_SEC_SYS_IOCTL:     {hCritSec = &pDevExtIn->uCritSecIoctl;  break;}
        case ePNDEV_CRIT_SEC_SYS_PAGE:      {hCritSec = &pDevExtIn->uCritSecPage;   break;}
        case ePNDEV_CRIT_SEC_SYS_APP_LIST:  {hCritSec = &pDevExtIn->uCritSecAppList;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid CriticalSection!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_CRIT_SEC;

            break;
        }
    }

    if  (hCritSec != NULL)
        // success
    {
        // Unlock critical section
        mutex_unlock((struct mutex*) hCritSec);
        // mutex_unlock has no return value
    }
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources of periodic SysThread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResSys(  DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lPrioThreadIn,
                                const UINT32        lStackSizeThreadIn,
                                const _TCHAR*       sNameThreadIn,
                                const UINT32        lDimErrorIn,
                                UINT64*             pThreadIdOut,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;
UINT32  lTimeCycle_Sec = 0;
UINT32  lTimeCycle_Msec = 0;

    //init timer with monotonic clock, so resetting the systemclock doesn't matter
    init_timer(&pDevExtIn->uSysThreadTimer);

    /* register the timer function an parameters*/
    pDevExtIn->uSysThreadTimer.function = &fnThreadSys;             // function called when timer expires
    pDevExtIn->uSysThreadTimer.data = (unsigned long) pDevExtIn;    // parameter to fnThreadSys

    //calculate CycleTime in sec/nsec
    lTimeCycle_Sec  = (pDevExtIn->lTimeCycleThreadSys_10msec / 100);
    lTimeCycle_Msec = (pDevExtIn->lTimeCycleThreadSys_10msec % 100) * 10; //MSec = 10msec * 10 => 10ms = 1 * 10ms

    pDevExtIn->uSysThreadTimer.expires = jiffies +                        //current ticks
                                         (HZ * lTimeCycle_Sec) +          //secounds (HZ are ticks per second)
                                         (HZ * lTimeCycle_Msec)/1000;     //msecs (HZ/1000 are ticks per millisecond)

	// set Flag: restart periodically timer
	pDevExtIn->bSysThreadTimerStop = FALSE;
																		  
	// start the timer
    add_timer(&pDevExtIn->uSysThreadTimer);

    // no ThreadId in Linux
    *pThreadIdOut = 0xFF;

    bResult = TRUE;

    return(bResult);
}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down resources of periodic SysThread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResSys( DEVICE_EXTENSION*   pDevExtIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	if  (!timer_pending(&pDevExtIn->uSysThreadTimer))
		// timer not running
	{
		// success
		bResult = TRUE;
	}
	else
	{
		// set Flag: don't restart periodically timer
		pDevExtIn->bSysThreadTimerStop = TRUE;

		if  (del_timer_sync(&pDevExtIn->uSysThreadTimer) < LINUX_OK)
			// error at deleting timer
		{
			// set ErrorString
			fnBuildString(  _TEXT(__FUNCTION__),
							_TEXT("(): Error at del_timer_sync()!"),
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

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  start up resources of DllThread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnGetEventFdFile(   DEVICE_EXTENSION*   pDevExtIn,
                            INT32               lEventfdIn,
                            void**              pEventfileOut,
                            const UINT32        lDimErrorIn,
                            _TCHAR*             sErrorOut)
{
BOOLEAN             bResult         = FALSE;
struct file*        pEventfdFile    = NULL;     // eventfd's file struct
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    // preset OutputParameter
    *pEventfileOut = NULL;

    if(lEventfdIn != -1)
    {
		// fcheck_files has to be protected by rcu or file lock!
		rcu_read_lock();
		// Resolve pointer to the userspace program's eventfd's file struct
		pEventfdFile = fcheck_files(    current->files,
										lEventfdIn);
		rcu_read_unlock();

		if(pEventfdFile != NULL)
		{
			// Return resolved pointer to the userspace program's eventfd's file struct
			*pEventfileOut = pEventfdFile;

			// success
			bResult = TRUE;
		}
		else
		{
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Can't find userspace eventfd file to send event!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,                       // logging
						sTraceDetail);

			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
		}
    }

    return(bResult);
}


//------------------------------------------------------------------------
BOOLEAN fnStartUpThreadResDll(  DEVICE_EXTENSION*       pDevExtIn,
                                uPNDEV_APP*             pAppIn,
                                uPNDEV_THREAD_IOCTL*    pThreadIoctlIn,
                                const UINT32            lDimErrorIn,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - pThreadIoctlIn->hEventXx is eventfd from usermode program
    //      -> it has to be resolved it's context send events from kernel mode
    //      -> We have to save the addresses to the eventfd files, while executing an ioctl
    //          since it has to be done in context of the application to get the files which belong to the application.
    //      -> The eventfd context has to be resolved right before sending an event,
    //          this will be done with eventfd_ctx_fileget and the saved file address.


    // Get eventfd file adress out of eventfd descriptor and current->pid
    fnGetEventFdFile(   pDevExtIn,
                        pThreadIoctlIn->uEventUinfo.lUint64,
                        &pAppIn->hEventUinfo,
                        lDimErrorIn,
                        sErrorOut);
    fnGetEventFdFile(   pDevExtIn,
                        pThreadIoctlIn->uEventUisr.lUint64,
                        &pAppIn->hEventUisr,
                        lDimErrorIn,
                        sErrorOut);

    if  (   (   pAppIn->hEventUinfo     == NULL)
        ||  (   pAppIn->hEventUisr      == NULL))
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Invalid handle!"),
                        NULL,
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
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  shut down resources of DllThread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnShutDownThreadResDll( DEVICE_EXTENSION*   pDevExtIn,
                                uPNDEV_APP*         pAppIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    pAppIn->hEventUinfo     = NULL;
    pAppIn->hEventUisr      = NULL;

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  set event of DllThread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnSetEventThreadDll(   DEVICE_EXTENSION*       pDevExtIn,
                            uPNDEV_APP*             pAppIn,
                            ePNDEV_EVENT_THREAD_DLL eEventIn,
                            BOOLEAN                 bThreadSysIn)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE]  = {0};
struct file*        pEventFile      = NULL;     // pointer to usermode eventfd file
struct eventfd_ctx* pEventfdCtx     = NULL;     // eventfd context

    switch  (eEventIn)
            // event
    {
        case ePNDEV_EVENT_THREAD_DLL_UINFO: {pEventFile = (struct file*) pAppIn->hEventUinfo;   break;}
        case ePNDEV_EVENT_THREAD_DLL_UISR:  {pEventFile = (struct file*) pAppIn->hEventUisr;    break;}
        default:
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Invalid event for DllThread!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;

            break;
        }
    }

    if(pEventFile != NULL)
    {
        pEventfdCtx = eventfd_ctx_fileget(pEventFile);

        if  (pEventfdCtx != NULL)
            // Successfully resolved eventfd context
        {
            // Increment userspace program's eventfd's counter by eEventIn
            eventfd_signal( pEventfdCtx,
                            eEventIn);
            eventfd_ctx_put(pEventfdCtx);
        }
        else
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[%s]: ########## Error at eventfd_ctx_fileget()!",
                                pDevExtIn->sPciLocShortAscii);

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

            // set FatalAsync
            pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_EVENT;
        }
    }
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get IoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
ePNDEV_IOCTL fnGetIoctlBuf( HANDLE          hOsParIn,
                            const UINT32    lSizeInputBufIn,
                            const UINT32    lSizeOutputBufIn,
                            void**          pPtrInputBufOut,
                            void**          pPtrOutputBufOut)
{
ePNDEV_IOCTL eResult = ePNDEV_IOCTL_INVALID;

    // Note:
    //  - at Linux this function isn't nessecary

    return(eResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetDeviceInfoSys
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceInfoSys(uPNDEV_GET_DEVICE_INFO_SYS_IN*  pIn,
                                uPNDEV_GET_DEVICE_INFO_SYS_OUT* pOut)
{
    BOOLEAN bResult         = FALSE;

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_GET_DEVICE_INFO_SYS_OUT));
    }

    if  (g_uVarSys.lCtrDevice == 0)
        // no device exist
    {
        // success
        bResult = TRUE;
    }
    else
    {
        if  (pIn->lIndexDevice > (g_uVarSys.lCtrDevice - 1))
            // invalid DeviceIndex
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Invalid DeviceIndex (real: %u, max: %u)!"),
                                pIn->lIndexDevice,
                                (g_uVarSys.lCtrDevice - 1));

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
                            NULL,
                            _countof(pOut->sError),
                            pOut->sError);
        }
        else
        {
        uLIST_HEADER*   pList       = NULL;
        uLIST_HEADER*   pBlockTmp   = NULL;
        UINT32          i = 0;

            pList = &g_uVarSys.uListDevice;

            // get first block of DeviceList
            pBlockTmp = (uLIST_HEADER*) pList->pNext;

            while   (pBlockTmp != pList)
                    // end of list not reached
            {
                if  (i == pIn->lIndexDevice)
                    // desired DeviceIndex
                {
                DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pBlockTmp;

					if	(fnGetVendorDeviceIdAtConfigSpace(	pDevExt,
															(UINT16*) &pOut->lVendorId,
															(UINT16*) &pOut->lDeviceId,
															_countof(pOut->sError),
															pOut->sError))
						// successful
                    {
			            // success
			            bResult = TRUE;
			            
			            // set return Param
                        pOut->lCtrDevice    = g_uVarSys.lCtrDevice;
                        pOut->eBoard        = pDevExt->eBoard;
                        pOut->lBus			= pDevExt->uPciLoc.uNo.lBus;
						pOut->lDevice		= pDevExt->uPciLoc.uNo.lDevice;
						pOut->lFunction		= pDevExt->uPciLoc.uNo.lFunction;

                        _RtlStringCchCopy(  pOut->sPciLoc,
                                            _countof(pOut->sPciLoc),
                                            pDevExt->uPciLoc.sString);
                    }

                    // leave loop
                    break;
                }

                // get next block of DeviceList
                pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;

                i++;
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service GetDeviceHandleSys
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlGetDeviceHandleSys(  uPNDEV_GET_DEVICE_HANDLE_SYS_IN*    pIn,
                                    uPNDEV_GET_DEVICE_HANDLE_SYS_OUT*   pOut)
{
BOOLEAN bResult = FALSE;
_TCHAR  sPciLoc[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

    // Note:
    //  - simulate METHOD_BUFFERED: memory of input buffer is used as output buffer!
    //  - OutputParameter must be preset!
    {
        // copy input data
        _RtlStringCchCopy(  sPciLoc,
                            _countof(sPciLoc),
                            pIn->sPciLoc);

        // preset OutputParameter
        RtlZeroMemory(  pOut,
                        sizeof(uPNDEV_GET_DEVICE_HANDLE_SYS_OUT));
    }

    if  (g_uVarSys.lCtrDevice > 0)
        // device exist
    {
    uLIST_HEADER*   pList       = NULL;
    uLIST_HEADER*   pBlockTmp   = NULL;

        // success
        bResult = TRUE;

        pList = &g_uVarSys.uListDevice;

        // get first block of DeviceList
        pBlockTmp = (uLIST_HEADER*) pList->pNext;

        while   (pBlockTmp != pList)
                // end of list not reached
        {
        DEVICE_EXTENSION* pDevExt = (DEVICE_EXTENSION*) pBlockTmp;

            if  (_tcscmp(pDevExt->uPciLoc.sString, pIn->sPciLoc) == 0)
                // device with desired PciLoc found
            {
                // success
                bResult = TRUE;

                // return
                pOut->uDeviceSys.hHandle = pDevExt;

                // leave loop
                break;
            }

            // get next block of DeviceList
            pBlockTmp = (uLIST_HEADER*) pBlockTmp->pNext;
        }
    }

    if  (!bResult)
        // device not found
    {
        // set ErrorString
        //  - changed by DLL to boolean
        fnBuildString(  PNDEV_ERROR_DEVICE_NOT_FOUND,
                        NULL,
                        NULL,
                        _countof(pOut->sError),
                        pOut->sError);
    }

    return(bResult);
}

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service AllocEventId
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlAllocEventId(uPNDEV_ALLOC_EVENT_ID_IN*   pIn,
                            uPNDEV_ALLOC_EVENT_ID_OUT*  pOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this service will never be called by DLL

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  PNDEV_ERROR_SUPPORT,
                    NULL,
                    NULL,
                    _countof(pOut->sError),
                    pOut->sError);

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service FreeEventId
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlFreeEventId( uPNDEV_FREE_EVENT_ID_IN*    pIn,
                            uPNDEV_FREE_EVENT_ID_OUT*   pOut)
{
BOOLEAN bResult = FALSE;

	// Note:
	//  - at Linux this service will never be called by DLL

	// set ErrorString
	//  - not changed by DLL
	fnBuildString(  PNDEV_ERROR_SUPPORT,
					NULL,
					NULL,
					_countof(pOut->sError),
					pOut->sError);

	return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  OS abstraction for NetProfi Kernel IOCTL handling
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlDoNetProfi(  DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_IN*  pOnlParIn,
                            uPNDEV_NP_DO_NETPROFI_REQ_OUT* pOnlParOut)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this Service is not supported

    // set ErrorString
    //  - not changed by DLL
    fnBuildString(  _TEXT(__FUNCTION__),
                    _TEXT("(): OperatingSystem not supported."),
                    NULL,
                    _countof(pOnlParOut->sError),
                    pOnlParOut->sError);

    return (bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service OpenTimer
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlOpenTimer(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_OPEN_TIMER_IN*	pIn,
							uPNDEV_OPEN_TIMER_OUT*	pOut)
{
BOOLEAN bResult = FALSE;

	// Note:
	//  - at Linux this service will never be called by DLL

	// set ErrorString
	//  - not changed by DLL
	fnBuildString(  PNDEV_ERROR_SUPPORT,
					NULL,
					NULL,
					_countof(pOut->sError),
					pOut->sError);

	return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  IoctlHandler of service CloseTimer
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnIoctlCloseTimer(	DEVICE_EXTENSION*		pDevExtIn,
							uPNDEV_CLOSE_TIMER_IN*	pIn,
							uPNDEV_CLOSE_TIMER_OUT*	pOut)
{
BOOLEAN bResult = FALSE;

	// Note:
	//  - at Linux this service will never be called by DLL

	// set ErrorString
	//  - not changed by DLL
	fnBuildString(  PNDEV_ERROR_SUPPORT,
					NULL,
					NULL,
					_countof(pOut->sError),
					pOut->sError);

	return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  connect interrupt
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnConnectInt(   DEVICE_EXTENSION*       pDevExtIn,
						HANDLE				    hOsRawParIn,
						HANDLE				    hOsTransParIn,
                        const UINT32            lDimErrorIn,
                        _TCHAR*                 sErrorOut)
{ 
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eIntMode)		
			// IntMode
	{
		case ePNDEV_INTERRUPT_MODE_LEG:			// LegacyMode
		{
			bResult = fnRegisterIsr(    pDevExtIn,
										pDevExtIn->pPciDev->irq,
										0,
										lDimErrorIn,
										sErrorOut);
			
			if  (!bResult)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at registering ISR!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			/*else
			{
			enable IntLine at Interruptcontroller
			not necessary, because no disable will be done during initialization
			enable_irq(pDevExtIn->pPciDev->irq);
			}*/
	
			break;
		}
		case ePNDEV_INTERRUPT_MODE_MSI:		// MsiMode
		{
		INT32	lIntCount	= LINUX_ERROR;
		INT32	i			= 0;
		
			// preset lIntCount
			lIntCount = PNDEV_MSIX_VECTOR_COUNT_MAX;
			
			// the MsixEntry Array has to be preset (no duplicated entries allowed!)
			for (i = 0; i<lIntCount; i++)
				// lIntCount
			{
				// init MSIEntries
				pDevExtIn->uMsixEntry[i].entry = i; 
			}
		
			// enable Msi for device
			#ifdef PNDEV_LINUX_MSI_BLOCK
      //UINT16  lStatus     = 0;
      //pci_read_config_word(dev, dev->msi_cap + PCI_MSI_FLAGS, &lStatus);
      //lIntCount = 1 << ((lStatus & PCI_MSI_FLAGS_QMASK) >> 1);
      
            // ToDo: not tested yet, only one interrupt vector is enabled.
            lIntCount = 1; // preset
            if (pci_enable_msi_block( pDevExtIn->pPciDev,  lIntCount) != 0)
            {
                lIntCount = -1; // error at pci_enable_msi_block()
            }

			#else
            
      lIntCount = pci_enable_msi_range( pDevExtIn->pPciDev,  
                                        1,
                                        lIntCount);
			#endif
			if	(lIntCount < 1)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at pci_enable_msi_block()/pci_enable_msi_range()!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				//preset 
				bResult = TRUE;
				
				for (i = 0; i < lIntCount; i++)
				   // all possible IntVectors
				{
                    pDevExtIn->uMsixEntry[i].vector = pDevExtIn->pPciDev->irq + i;
                    
					if 	(!fnRegisterIsr(pDevExtIn,
										pDevExtIn->uMsixEntry[i].vector,
										i,
										_countof(sErrorOut),
										sErrorOut))
						// registering ISR for vector i cause error
					{				
						// error
						bResult = FALSE;						
			   
						// leave loop
						break;
					}
				}
				
		    	// save IntCount
		    	pDevExtIn->lIntCount = lIntCount;
			}			
			break;		
		}
		case ePNDEV_INTERRUPT_MODE_MSIX:		// MsixMode
		{
		INT32	lIntCount	= LINUX_ERROR;
        INT32	lResultTmp 	= LINUX_ERROR;
		INT32	i			= 0;
		
			// preset lIntCount
            lIntCount = PNDEV_MSIX_VECTOR_COUNT_MAX;
            
            // the MsixEntry Array has to be preset (no duplicated entries allowed!)
            for (i = 0; i<lIntCount; i++)
                // lIntCount
            {
                // init MSIEntries
                pDevExtIn->uMsixEntry[i].entry = i; 
            }
                      
            while (lIntCount >= 1)
            {               
            	// enable msix
                // make sure that uMsixEntry is preset
				lResultTmp = pci_enable_msix(	pDevExtIn->pPciDev,
												pDevExtIn->uMsixEntry,
												lIntCount);
				
				if	(lResultTmp > LINUX_OK)
				{
					// try again with returned value
					lIntCount = lResultTmp;
				}
				else
				{
					break;
				}
            }
		
			if  (lResultTmp != LINUX_OK)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at pci_enable_msix()!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				//preset 
				bResult = TRUE;
				
				for (i = 0; i < lIntCount; i++)
				   // all possible IntVectors
				{        	
					if 	(!fnRegisterIsr(pDevExtIn,
										pDevExtIn->uMsixEntry[i].vector,
										i,
										_countof(sErrorOut),
										sErrorOut))
						// registering ISR for vector i cause error
					{				
						// error
						bResult = FALSE;						
			   
						// leave loop
						break;
					}
				}
				
		    	// save IntCount
		    	pDevExtIn->lIntCount = lIntCount;
			}
			
			break;		
		}
        case ePNDEV_INTERRUPT_MODE_INVALID:
        {
            // this is a board, which doesn't support interrupts
            // return true, so the fnConnectInt was "successful"
            bResult = TRUE;
            
            break;
        }
		default:
		{	
			break;
		}
	}

    if  (!bResult)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at registering ISR!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {   						
    	// LegIntConnected
        pDevExtIn->bIntConnected = TRUE;    	
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  disconnect interrupt
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnDisconnectInt(DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{  
BOOLEAN bResult = FALSE;

	if	(pDevExtIn->bIntConnected)
		// connected Int
	{
		switch	(pDevExtIn->eIntMode)		
				// IntMode
		{
			case ePNDEV_INTERRUPT_MODE_LEG:			// LegacyMode
			{
				// disable irq
				remove_irq(	pDevExtIn->pPciDev->irq, 
							&pDevExtIn->uIrqAaction[0]);
				// clear irqaction
				memset(	&pDevExtIn->uIrqAaction[0],
						0,
						sizeof(struct irqaction));
			    
			    // success
			    bResult = TRUE;
			    
			    break;
			}
			case ePNDEV_INTERRUPT_MODE_MSI:			// MsiMode
			{
			UINT32 i = 0;
			
                for	(i=0; i<pDevExtIn->lIntCount; i++)
					// IntCount
				{
					// disable irq
					remove_irq(	pDevExtIn->uMsixEntry[i].vector, 
								&pDevExtIn->uIrqAaction[i]);
					// clear irqaction
					memset(	&pDevExtIn->uIrqAaction[i],
 							0,
							sizeof(struct irqaction));
				}
				
				// disable msi
				pci_disable_msi(pDevExtIn->pPciDev);
				
				// success
				bResult = TRUE;
				
				break;
			}
			case ePNDEV_INTERRUPT_MODE_MSIX:		// MsixMode
			{
			UINT32 i = 0;
			
				for	(i=0; i<pDevExtIn->lIntCount; i++)
					// IntCount
				{
					// disable irq
					remove_irq(	pDevExtIn->uMsixEntry[i].vector, 
								&pDevExtIn->uIrqAaction[i]);
					// clear irqaction
					memset(	&pDevExtIn->uIrqAaction[i],
							0,
							sizeof(struct irqaction));
				}
				
				// disable msix
				pci_disable_msix(pDevExtIn->pPciDev);
				
				// success
				bResult = TRUE;
				
				break;
			}
			default:
			{	
				break;
			}
		}
		
		if	(bResult)
			//success
		{
			pDevExtIn->bIntConnected = FALSE;
		}
	}
	else
	{
		// success
		bResult = TRUE;
	}
    
    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  register ISR
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnRegisterIsr(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lIntVectorIn,
						const UINT32        lMsgIndexIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN				bResult		= FALSE;
int32_t				lResultTmp	= LINUX_ERROR;
struct  irqaction*	action		= &pDevExtIn->uIrqAaction[lMsgIndexIn];

	// init irqaction
    memset( action,
            0,
            sizeof(struct irqaction));
    
	// configure IRQ action for selected IntVector
	action->handler 	= fnEvtIsr;			    	// isr function
	action->flags 		= IRQF_SHARED |				// allow shared interrupt, other devices may share same interrupt line
			        	  IRQF_NO_THREAD;			// don't thread the interrupt
	action->name 		= pDevExtIn->sPciLocShort;	// name of interrupt owner in /proc/interrupts, make name unique
	action->dev_id 		= (void*) pDevExtIn;		// identifier to deregister shared interrupts (also usefull if not shared)

	// setup IRQ for selected IntVector
	lResultTmp = setup_irq( lIntVectorIn,   		// interrupt number
							action);        		// irq action

	if	(lResultTmp != LINUX_OK)
		// error with non threaded interrupt setup, try again without IRQF_NO_THREAD
	{
		action->flags 	= IRQF_SHARED;				// allow shared interrupt, other devices may share same interrupt line

		lResultTmp = setup_irq(	lIntVectorIn,		// interrupt number
								action);			// irq action
	}

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at setup_irq()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    	if	(action->thread != NULL)
    		// thread was created raise it's priority
    	{
    		struct sched_param param = {
    			.sched_priority = 99,
    		};
    		sched_setscheduler_nocheck(action->thread, SCHED_FIFO, &param);
    	}

        // success
        bResult = TRUE;
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  queue DPC
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnQueueDpc( DEVICE_EXTENSION*   pDevExtIn,
                    HANDLE              hOsParIn)
{
BOOLEAN bResult = FALSE;

    // Note:
    //  - at Linux this function will never be called

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate memory
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void* fnAllocMemNonPaged(   size_t  lSizeIn,
                            ULONG   lTagIn)
{
void* pResult = NULL;

    pResult = __kmalloc(lSizeIn,
                        GFP_ATOMIC);

    return(pResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  free memory
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnFreeMemNonPaged( void*   pMemIn,
                        ULONG   lTagIn)
{
    kfree(pMemIn);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  map physical addresses to non-paged virtual memory
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnMapMemPhyToVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                                uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut,
                                const UINT32                lBarIndexIn,
                                const UINT32                lDimErrorIn,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    if  (pMemKmInOut->lSize == 0)
        // memory not used
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // map PhyAdr to UserMode
        //  -> access by KernelMode automatically
        pMemKmInOut->pVirtualAdr = pci_ioremap_bar(   pDevExtIn->pPciDev,
                                                    lBarIndexIn);

        if  (pMemKmInOut->pVirtualAdr == NULL)
            // error
        {
        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

            // set ErrorDetail
            _RtlStringCchPrintf(sErrorDetail,
                                _countof(sErrorDetail),
                                _TEXT("(): Unable to map Bar%d physical memory 0x%Lx, size 0x%x to virtual memory!"),
                                lBarIndexIn,
                                pMemKmInOut->uPhyAdr.QuadPart,
                                pMemKmInOut->lSize);

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            sErrorDetail,
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

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  release virtual memory
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnReleaseMemVirtual(   DEVICE_EXTENSION*           pDevExtIn,
                            uPNDEV_HW_RES_SINGLE_KM*    pMemKmInOut)
{

    if  (pMemKmInOut->pVirtualAdr != NULL)
        // memory mapped to system space
    {
        // release virtual memory
        pci_iounmap(pDevExtIn->pPciDev,
                    (void*) pMemKmInOut->pVirtualAdr);

        pMemKmInOut->pVirtualAdr = NULL;
    }

}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  map virtual memory addresses to UserMode
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnMapMemVirtualToUser(  DEVICE_EXTENSION*           pDevExtIn,
                                const void*                 pVirtualAdrIn,
                                const UINT32                lSizeMemIn,
                                const UINT32                lDimErrorIn,
                                uPNDEV_HW_RES_SINGLE_UM*    pMemUmOut,
                                _TCHAR*                     sErrorOut)
{
BOOLEAN bResult = FALSE;

    // will never be called in Linux

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  release memory mapped to UserMode
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnReleaseMemUser(  DEVICE_EXTENSION*               pDevExtIn,
                        const uPNDEV_HW_RES_SINGLE_UM*  pMemUmIn)
{
    // munmap will be called in Usermode

    // reset
    RtlZeroMemory(  (void*) pMemUmIn,
                    sizeof(uPNDEV_HW_RES_SINGLE_UM));
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  start WaitTime of current thread
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnDelayThread(  DEVICE_EXTENSION*   pDevExtIn,
                        const UINT32        lTimeMsecIn,
                        const UINT32        lDimErrorIn,
                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult             = FALSE;
UINT32  lTimeDesiredMsec    = 0;

    // update desired WaitTime
    lTimeDesiredMsec = lTimeMsecIn * pDevExtIn->lTimeScaleFactor;

    if  (lTimeDesiredMsec == 0)
        // no WaitTime
    {
        // success
        bResult = TRUE;
    }
    else
    {
        // wait msec
        msleep(lTimeDesiredMsec);

        // success
        bResult = TRUE;
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get clock
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnGetClock(uPNDEV_CLOCK* pClockOut)
{
    getrawmonotonic(pClockOut);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  compute RunTime in usec
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
UINT32 fnComputeRunTimeUsec(uPNDEV_CLOCK* pClockStartIn)
{
UINT32          lResult         = 0;
struct timespec uClockStop;

    getrawmonotonic(&uClockStop);

    if  (   (uClockStop.tv_nsec > 0)
        ||  (uClockStop.tv_sec > 0))
        // getting clock ok
    {
    UINT64 lTimeStart_Usec  = 0;
    UINT64 lTimeStop_Usec   = 0;

        // compute StartTime in usec
        lTimeStart_Usec = (pClockStartIn->tv_sec * 1000 * 1000) + (pClockStartIn->tv_nsec / 1000);

        // compute StopTime
        lTimeStop_Usec = (uClockStop.tv_sec * 1000 * 1000) + (uClockStop.tv_nsec / 1000);

        // compute Runtime in usec
        lResult = (UINT32) (lTimeStop_Usec - lTimeStart_Usec);
    }

    return(lResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  set trace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnSetTrace(const ePNDEV_TRACE_LEVEL    eLevelIn,
                const ePNDEV_TRACE_CONTEXT  eContextIn,
                const BOOLEAN               bLogIn,
                char*                       sTraceIn)
{
    printk(KERN_WARNING "%s\n", sTraceIn);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  check IRQL
//************************************************************************

//------------------------------------------------------------------------
#if   defined (PNDEV_OS_ADONIS) || (PNDEV_OS_LINUX)
//------------------------------------------------------------------------
BOOLEAN fnCheckIrql(const KIRQL lIrqlMaxIn,
                    KIRQL*      pIrqlCurOut)
{
BOOLEAN bResult = FALSE;

    // success
    bResult = TRUE;

    // return
    *pIrqlCurOut = lIrqlMaxIn;

    return(bResult);
}

//------------------------------------------------------------------------
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get ResList of ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnGetResListConfigSpace(DEVICE_EXTENSION*   pDevExtIn,
                                HANDLE              hOsTransParIn,               // hResTranslatedIn
                                const UINT32        lFirstBarIn,
                                const UINT32        lDimErrorIn,
                                _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;

    // preset
    bResult = TRUE;

    // reserve PCI I/O and memory resources
    lResultTmp = pci_request_regions(   pDevExtIn->pPciDev,
                                        "PnDevDrv");

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Error at pci_request_regions()!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    UINT32  i = 0;

        // preset
        bResult     = TRUE;

        // parse the resource list
        for (i = lFirstBarIn; i < PNDEV_DIM_ARRAY_BAR; i++)
            // all resources
        {
        UINT32 lResourceFlags = 0;

            // get current Ressource
            lResourceFlags = pci_resource_flags(pDevExtIn->pPciDev,
                                                i);

            if  (lResourceFlags == 0)
                // error
            {
                // ignore the value zero
                // Note:    - some Bars has no resource
                //          - e.g. Eb200P has no resources at Bar 3,4 and 5
                // bResult = FALSE;

                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at pci_ressource_flags()!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                if  (lResourceFlags & IORESOURCE_IO)
                    // Io Ports
                {
                    // error, but ignore it
                    bResult = TRUE;

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Unexpected IO Ressource!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                }
                else if (lResourceFlags & IORESOURCE_IO)
                        // Interrupt
                {
                    //ToDo:
                    /*if    ((pResDescriptor->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) == CM_RESOURCE_INTERRUPT_MESSAGE)
                        // MessageSignaledInt (MSI)
                    {
                        // error
                        bResult = FALSE;

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Unexpected message signaled interrupt (MSI)!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }*/

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Unexpected Interrupt Ressource!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                }
                else if (lResourceFlags & IORESOURCE_MEM)
                        // Memory
                {
                    switch  (i)
                            // BarIndex
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        {
                            // store at DeviceExtension
                            pDevExtIn->uHwResKm.uBar[i].uPhyAdr.QuadPart    = pci_resource_start(   pDevExtIn->pPciDev, i);
                            pDevExtIn->uHwResKm.uBar[i].lSize               = pci_resource_len(     pDevExtIn->pPciDev, i);

                            break;
                        }
                        default:
                        {
                        _TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

                            // error
                            bResult = FALSE;

                            // set ErrorDetail
                            _RtlStringCchPrintf(sErrorDetail,
                                                _countof(sErrorDetail),
                                                _TEXT("(): Invalid BAR number (%u)!"),
                                                i);

                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            sErrorDetail,
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);

                            break;
                        }
                    }
                }
                else
                {
                    // ignore all other flags
                }
            }
        }
    }
    
    if  (bResult)
        // success
    {
		if  (pDevExtIn->eBoard == ePNDEV_BOARD_IX1000)
			// Intel Quark X1000 on Galileo board
		{
		UINT16  lCommand    = 0;
		
			// preset
			bResult = FALSE;
		
			// get command of ConfigSpace
			lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
												PCI_COMMAND,
												&lCommand);
		
			if  (lResultTmp != LINUX_OK)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at reading Command of ConfigSpace!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// enable Memory Space before mapping bars!
				// also enable Bus Master and SERR.
				lCommand |= (PNDEV_UINT32_SET_BIT_1 | PNDEV_UINT32_SET_BIT_2 | PNDEV_UINT32_SET_BIT_8);
	
				// update command at ConfigSpace
				lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
													PCI_COMMAND,
													lCommand);
	
				if  (lResultTmp != LINUX_OK)
					// error
				{
					// set ErrorString
					fnBuildString(  _TEXT(__FUNCTION__),
									_TEXT("Error at writing Command at ConfigSpace!"),
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
		}
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get ConfigSpaceInfo
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnGetInfoConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                const UINT32            lDimErrorIn,
                                uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                _TCHAR*                 sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultInt  = FALSE;

    if(pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_LEG)
    {
        // get IntPin / IntLine
        {
            if  (pDevExtIn->pPciDev->pin <= 0)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at reading IntPin!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // save IntPin value
                pConfigSpaceOut->lIntPin = pDevExtIn->pPciDev->pin;
    
                // get IntLine
                {
                    if  (pDevExtIn->pPciDev->irq <= 0)
                        // error
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Error at reading IntLine!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                        // success
                        bResultInt = TRUE;
    
                        // store value
                        pConfigSpaceOut->lIntLine   = pDevExtIn->pPciDev->irq;
                        pDevExtIn->lIntLine         = pDevExtIn->pPciDev->irq;
                    }
                }
            }
        }
    }
    else
    {
        bResultInt = TRUE;
    }

    if  (bResultInt)
        // success
    {
    UINT32 lOffsetCapability = 0;
        
        if  (fnCheckConfigSpaceExpressCapabilityExists( pDevExtIn,
														PNDEV_PCIE_CAPABILITY_ID,
                                                        lDimErrorIn,
                                                        &lOffsetCapability,
                                                        sErrorOut))
            // a PCI express capability exists
        {
            // preset
            bResult = FALSE;

            // get data of PCIe Capability
            bResult = fnGetExtendedConfigSpace( pDevExtIn,
                                                lOffsetCapability,
                                                lDimErrorIn,
                                                pConfigSpaceOut,
                                                sErrorOut);                       
        }
        else
        {
        	// success for pci boards
        	bResult = TRUE;
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BarValues of ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get data of ExtendedConfigSpace from PCIe
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnGetExtendedConfigSpace(   DEVICE_EXTENSION*       pDevExtIn,
                                    const UINT32            lOffsetCapabilityIn,
                                    const UINT32            lDimErrorIn,
                                    uPNDEV_CONFIG_SPACE*    pConfigSpaceOut,
                                    _TCHAR*                 sErrorOut)
{
BOOLEAN bResult = FALSE;

    if (lOffsetCapabilityIn == 0)
        // invalid CapabilityPtr
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("Invalid OffsetCapability!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
    UINT16  lLinkStatus             = 0;
    UINT16  lDeviceControl          = 0;
    UINT16  lDeviceStatus           = 0;
    UINT32  lDeviceControlStatus    = 0;
    UINT32  lDeviceCapability       = 0;
    UINT32  lResultTmp              = LINUX_ERROR;
    UINT32  lResultTmp1             = LINUX_ERROR;
    UINT32  lResultTmp2             = LINUX_ERROR;
    UINT32  lResultTmp3             = LINUX_ERROR;
    UINT32  lResultTmp4             = LINUX_ERROR;
    UINT32  lResultTmp5             = LINUX_ERROR;

        // get LinkStatus
        lResultTmp1 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_LINK_STATUS_OFFSET,     // offset
                                            &lLinkStatus);

        // get DeviceControl
        lResultTmp2 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                            &lDeviceControl);

        // get DeviceStatus
        lResultTmp3 = pci_read_config_word( pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET,   // offset
                                            &lDeviceStatus);

        // get DeviceControl + DeviceStatus
        //  - required for ack of ErrorBits
        lResultTmp4 = pci_read_config_dword(pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,  // offset
                                            &lDeviceControlStatus);
        
        // get Device Capability
        lResultTmp5 = pci_read_config_dword(pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CAPABILITY_OFFSET,  // offset
                                            &lDeviceCapability);

        if  (   (lResultTmp1 != LINUX_OK)
            ||  (lResultTmp2 != LINUX_OK)
            ||  (lResultTmp3 != LINUX_OK)
            ||  (lResultTmp4 != LINUX_OK)
            ||  (lResultTmp5 != LINUX_OK))
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading data of PCIe Capability!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        UINT32  lMaxPayLoad = 0;
        UINT32  lLinkWidth  = 0;

            // save LinkWidth
            lLinkWidth= (lLinkStatus & PNDEV_PCIE_CAPABILITY_MASK_LINK_WIDTH) >> 4;

            switch  (lLinkWidth)
                    // LinkWidth
            {
                case 0x1:   {pConfigSpaceOut->uExtended.lLinkWidth = 1;     break;}
                case 0x2:   {pConfigSpaceOut->uExtended.lLinkWidth = 2;     break;}
                case 0x4:   {pConfigSpaceOut->uExtended.lLinkWidth = 4;     break;}
                case 0x8:   {pConfigSpaceOut->uExtended.lLinkWidth = 8;     break;}
                case 0x12:  {pConfigSpaceOut->uExtended.lLinkWidth = 12;    break;}
                case 0x16:  {pConfigSpaceOut->uExtended.lLinkWidth = 16;    break;}
                case 0x32:  {pConfigSpaceOut->uExtended.lLinkWidth = 32;    break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid LinkWidth!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            // save MaxPayloadSize
            lMaxPayLoad = (lDeviceControl & PNDEV_PCIE_CAPABILITY_MASK_PAY_LOAD_SIZE) >> 5;

            switch  (lMaxPayLoad)
                    // MaxPayloadSize
            {
                case 0: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 128;  break;}
                case 1: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 256;  break;}
                case 2: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 512;  break;}
                case 3: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 1024; break;}
                case 4: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 2048; break;}
                case 5: {pConfigSpaceOut->uExtended.lMaxPayloadSize = 4096; break;}
                default:
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Invalid MaxPayLoadSize!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                    break;
                }
            }

            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_0)    {pConfigSpaceOut->uExtended.bCorrectableErrorDetected   = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_1)    {pConfigSpaceOut->uExtended.bNonFatalErrorDetected      = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_2)    {pConfigSpaceOut->uExtended.bFatalErrorDetected         = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_3)    {pConfigSpaceOut->uExtended.bUnsupportedRequestDetected = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_4)    {pConfigSpaceOut->uExtended.bAuxPowerDetected           = TRUE;}
            if  (lDeviceStatus & PNDEV_UINT32_SET_BIT_5)    {pConfigSpaceOut->uExtended.bTransactionsPending        = TRUE;}
            
            // Flr supported by this Device
            if  ((lDeviceCapability>>28) & 0x1) {pDevExtIn->bFlrSupport = TRUE;}
        }

        // ack ErrorBits
        //  - write DeviceStatus back to PCIe CapabilityList
        lResultTmp = pci_write_config_dword(pDevExtIn->pPciDev,
                                            lOffsetCapabilityIn + PNDEV_PCIE_CAPABILITY_DEVICE_CONTROL_OFFSET,
                                            lDeviceControlStatus);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at ack of ErrorBits!"),
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

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  check ConfigSpace for Express Capability
//
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnCheckConfigSpaceExpressCapabilityExists(  DEVICE_EXTENSION*   pDevExtIn,
													const UINT8			lCapabilityIn,
                                                    const UINT32        lDimErrorIn,
                                                    UINT32*             lOffsetOut,
                                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;    
UINT16  lResultTmp  = LINUX_ERROR;
UINT16  lStatus     = 0;

    // preset
    *lOffsetOut = 0;

    // get status
    lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                        PCI_STATUS,
                                        &lStatus);

    if  (lResultTmp != LINUX_OK)
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("Error at reading status!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }
    else
    {
        if  ((lStatus & PNDEV_PCI_STATUS_CAPABILITIES_LIST) == 0)
            // no CapabilityList present
        {
            // do nothing
        }
        else
        {
        UINT8 lHeaderType = 0;

            // device supports CapabilityLists -> find the capabilities

            // get HeaderType
            lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                PCI_HEADER_TYPE,
                                                &lHeaderType);

            if  (lResultTmp != LINUX_OK)
                // error
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("Error at reading HeaderType!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
            uint8_t lOffsetCapability   = 0;

                // get offset to CapabilitiesPtr
                //  - position of the CapabilitiesPtr in the header depends on whether this is a BridgeTypeDevice

                if  ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_BRIDGE_TYPE)
                    // PciConfigType = PCI to PCI Bridge
                {
                    // Type1
                    lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                }
                else if ((lHeaderType & (~PNDEV_PCI_MULTIFUNCTION)) == PNDEV_PCI_CARDBUS_BRIDGE_TYPE)
                        // PciConfigType = PCI to CARDBUS Bridge
                {
                    // Type2
                    lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_2;
                }
                else
                {
                    // Type0
                    lOffsetCapability = PNDEV_PCI_CAPABILITY_PTR_TYPE_0_1;
                }

                // read address of first capability
                lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                    lOffsetCapability,
                                                    &lOffsetCapability);

                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at reading OffsetCapability!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);

                }
                else
                {
                    // loop through the capabilities, search for the desired capability
                    //  - the list is NULL-terminated, so the last offset will always be zero

                    while   (lOffsetCapability)
                            // not end of list
                    {
                    uint8_t lCapabilityId = 0;

                        // read CapabilityId
                        lResultTmp =  pci_read_config_byte( pDevExtIn->pPciDev,
                                                            lOffsetCapability,
                                                            &lCapabilityId);

                        if  (lResultTmp != LINUX_OK)
                            // error
                        {
                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            _TEXT("Error at reading CapabilityId!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);

                            // leave loop
                            break;
                        }
                        else
                        {
                            if  (lCapabilityId == lCapabilityIn)
                                // PCIe capability found
                            {
                                // success
                                *lOffsetOut = lOffsetCapability;
                                bResult = TRUE;

                                // leave loop
                                break;
                            }
                            else
                            {
                                // get next CapabilityPtr
                                lResultTmp = pci_read_config_byte(  pDevExtIn->pPciDev,
                                                                    lOffsetCapability + sizeof(UINT8),
                                                                    &lOffsetCapability);

                                if  (lResultTmp != LINUX_OK)
                                    // error
                                {
                                    // set ErrorString
                                    fnBuildString(  _TEXT(__FUNCTION__),
                                                    _TEXT("Error at reading OffsetCapability!"),
                                                    NULL,
                                                    lDimErrorIn,
                                                    sErrorOut);

                                    // leave loop
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }   
    
    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  prepare ConfigSpace for HwReset
//
//  Note:
//  - a PCI ConfigSpace consists of maximal 256 Bytes
//  - Soc1: ConfigSpace consists of 64 Bytes + PowerManagement capability (readonly)
//          restoring more than 64 Bytes creates a SystemError!
//          -> store only 64 Bytes
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnPrepareConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    const UINT32        lDimErrorIn,
                                    UINT32              lArrayConfigSpaceOut[],     // array=ptr
                                    UINT32				lArrayExpressCapabilityOut[],  // array=ptr
                                    UINT32*             lOffsetExpressCapabilityOut,
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
BOOLEAN bResultTmp  = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT32  i = 0;

    // preset
    bResultTmp = TRUE;
    
    if  (fnCheckConfigSpaceExpressCapabilityExists( pDevExtIn,
													PNDEV_PCIE_CAPABILITY_ID,
                                                    lDimErrorIn,
                                                    lOffsetExpressCapabilityOut,
                                                    sErrorOut))
        // a PCI express capability exists
    {
        for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__EXPRESS_CAPABILITY_SIZE / sizeof(UINT32)); i++)
            // complete express capability
        {
            // get value of ConfigSpace
            lResultTmp = pci_read_config_dword( pDevExtIn->pPciDev,
                                                ((i*sizeof(UINT32)) + *lOffsetExpressCapabilityOut),
                                                &lArrayExpressCapabilityOut[i]);
    
            if  (lResultTmp != LINUX_OK)
                // error
            {   
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at reading express capability!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
    
                // leave loop
                bResultTmp = FALSE;
                break;
            }
        }
    }
	else
	{
		// success
		bResultTmp = TRUE;        	
	}
    
	if	(bResultTmp)
		// success
	{
		for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
			// complete ConfigSpace
		{
			// get value of ConfigSpace
			lResultTmp = pci_read_config_dword( pDevExtIn->pPciDev,
												(i*sizeof(UINT32)),
												&lArrayConfigSpaceOut[i]);
	
			if  (lResultTmp != LINUX_OK)
				// error
			{
				bResultTmp = FALSE;
	
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at reading ConfigSpace!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
	
				// leave loop
				break;
			}
		}
	
		if  (bResultTmp)
			// success
		{
		uint16_t lCommand = 0;
	
			// get command of ConfigSpace
			lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
												PCI_COMMAND,
												&lCommand);
	
			if  (lResultTmp != LINUX_OK)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("(): Error at reading command of ConfigSpace!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// disable BusMaster
				lCommand &= PNDEV_UINT32_CLR_BIT_2;
	
				// update command at ConfigSpace
				lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
													PCI_COMMAND,
													lCommand);
	
				if  (lResultTmp != LINUX_OK)
					// error
				{
					// set ErrorString
					fnBuildString(  _TEXT(__FUNCTION__),
									_TEXT("Error at writing Command at ConfigSpace!"),
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
		}
	}

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  restore ConfigSpace after HwReset
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnRestoreConfigSpaceHwReset(DEVICE_EXTENSION*   pDevExtIn,
                                    UINT32              lArrayConfigSpaceIn[],      // array=ptr
                                    UINT32				lArrayExpressCapabilityIn[],  // array=ptr
                                    UINT32              lOffsetExpressCapabilityIn,
                                    const UINT32        lDimErrorIn,
                                    _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
int32_t lResultTmp  = LINUX_ERROR;
UINT32  i = 0;

    // preset
    bResult = TRUE;

    for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)); i++)
        // complete ConfigSpace
    {
        // update value at ConfigSpace
        lResultTmp = pci_write_config_dword(pDevExtIn->pPciDev,
                                            (i*sizeof(UINT32)),
                                            lArrayConfigSpaceIn[i]);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            bResult = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at writing ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            // leave loop
            break;
        }
    }
    
    if  (lResultTmp == LINUX_OK)
    {
    UINT32 lOffsetExpressCapabilityTemp = 0;

        if  (fnCheckConfigSpaceExpressCapabilityExists( pDevExtIn,
														PNDEV_PCIE_CAPABILITY_ID,
                                                        lDimErrorIn,
                                                        &lOffsetExpressCapabilityTemp,
                                                        sErrorOut))
            // a PCI express capability exists
        {
            if  (lOffsetExpressCapabilityTemp != lOffsetExpressCapabilityIn)
            {
                bResult = FALSE;
                
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): Error at restore express capability!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                // restore express capability
                for (i = 0; i < (PNDEV_PCI_CONFIG_SPACE__EXPRESS_CAPABILITY_SIZE / sizeof(UINT32)); i++)
                    // complete express capability
                {
                    // update value at ConfigSpace
                    lResultTmp = pci_write_config_dword(pDevExtIn->pPciDev,
                                                        ((i*sizeof(UINT32)) + lOffsetExpressCapabilityIn),
                                                        lArrayExpressCapabilityIn[i]);

                    if  (lResultTmp != LINUX_OK)
                        // error
                    {
                        bResult = FALSE;

                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Error at writing express capability!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);

                        // leave loop
                        break;
                    }
                }             
            }
        }
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Enable BusMaster in ConfigSpace (is called on first open device)
//  In Linux BusMaster is not enabled by default after reset/power up!
//  Standard/Intel cards Springville and Hartwell need BusMaster enabled
//  as well as SOC1 which has disabled BusMaster after reset!
//  IX1000 has BusMaster and Memory Space disabled
//  -> since Memory Space setting is needed for virtual Mapping Memory
//     IX1000 ConfigSpace settings are set before mapping virtual
//     Memory regions! see fnGetResListConfigSpace()     
//
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnEnableBusMasterConfigSpace(   DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT16  lCommand    = 0;

	// get command of ConfigSpace
	lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
										PCI_COMMAND,
										&lCommand);

	if  (lResultTmp != LINUX_OK)
		// error
	{
		// set ErrorString
		fnBuildString(  _TEXT(__FUNCTION__),
						_TEXT("(): Error at reading Command of ConfigSpace!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		if  ((lCommand >> 2) & 1)
			// BusMaster already enabled
		{
			// nothing to do
			bResult = TRUE;
		}
		else
		{
			// enable BusMaster
			lCommand |= PNDEV_UINT32_SET_BIT_2;

			// update command at ConfigSpace
			lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
												PCI_COMMAND,
												lCommand);

			if  (lResultTmp != LINUX_OK)
				// error
			{
				// set ErrorString
				fnBuildString(  _TEXT(__FUNCTION__),
								_TEXT("Error at writing Command at ConfigSpace!"),
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
	}

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Disable BusMaster for Standard/Intel cards in ConfigSpace 
//  (is called on last close device)
//
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnDisableBusMasterConfigSpace(  DEVICE_EXTENSION*   pDevExtIn,
                                        const UINT32        lDimErrorIn,
                                        _TCHAR*             sErrorOut)
{
BOOLEAN bResult     = FALSE;
UINT32  lResultTmp  = LINUX_ERROR;
UINT16  lCommand    = 0;

    if  (   (pDevExtIn->eBoard == ePNDEV_BOARD_I210)
		||	(pDevExtIn->eBoard == ePNDEV_BOARD_I210SFP)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842)
        ||  (pDevExtIn->eBoard == ePNDEV_BOARD_TI_AM5728))
        // standard card
    {
        // get command of ConfigSpace
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            PCI_COMMAND,
                                            &lCommand);

        if  (lResultTmp != LINUX_OK)
            // error
        {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Error at reading command of ConfigSpace!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
            if  (!((lCommand >> 2) & 1))
                // BusMaster already disabled
            {
                // nothing to do
                bResult = TRUE;
            }
            else
            {
                // disable BusMaster
                lCommand &= PNDEV_UINT32_CLR_BIT_2;
    
                // update command at ConfigSpace
                lResultTmp = pci_write_config_word( pDevExtIn->pPciDev,
                                                    PCI_COMMAND,
                                                    lCommand);
    
                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at writing Command at ConfigSpace!"),
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
        }
    }
    else
    {
        // success
        bResult = TRUE;     
    }

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  read BitStreamVersion from ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
UINT32 fnReadConfigSpaceBitStreamVer(	DEVICE_EXTENSION*   pDevExtIn,
    									UINT32				lOffsetIn,
    									const UINT32        lDimErrorIn,
    									_TCHAR*             sErrorOut)
{
UINT32 lBitStreamVersion    = 0;
UINT32 lResultTmp           = 0;

    lResultTmp = pci_read_config_dword(pDevExtIn->pPciDev,
                                       lOffsetIn,  // offset
                                       &lBitStreamVersion);
    
    if  ( lResultTmp != LINUX_OK )
        // error
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("Error at reading BitStreamVer of PCIe Capability!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }

	return lBitStreamVersion;
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  check pending Transaction of ExtendedConfigSpace from PCIe
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnCheckTransactionPending(	DEVICE_EXTENSION*	pDevExtIn,
                                	const UINT32        lDimErrorIn,
                                	_TCHAR*             sErrorOut)
{
BOOLEAN bResult             = FALSE;
BOOLEAN lResultTmp          = LINUX_ERROR;
UINT32  lOffsetCapability   = 0;
    
    if  (fnCheckConfigSpaceExpressCapabilityExists( pDevExtIn,
													PNDEV_PCIE_CAPABILITY_ID,
                                                    lDimErrorIn,
                                                    &lOffsetCapability,
                                                    sErrorOut))
        // a PCI express capability exists
    {
    uint16_t    lTransactionsPending    = 0;
    UINT32      lWaitTmp                = 0;
    
        // preset
        lResultTmp = LINUX_ERROR;
            
        // read DeviceStatus
        lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                            lOffsetCapability + PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET, // Offest DeviceStatus,
                                            &lTransactionsPending);
        
        if  (lResultTmp != LINUX_OK)
            // error
        {
            bResult = FALSE;
    
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("Error at reading OffsetCapability!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
        }
        else
        {
        
            while   ((lTransactionsPending>>5) & 0x1)
                    // TransactionPending is true
            {
                // read DeviceControl
                lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
                                                    lOffsetCapability + PNDEV_PCIE_CAPABILITY_DEVICE_STATUS_OFFSET, // Offest DeviceStatus,
                                                    &lTransactionsPending);
                
                if  (lResultTmp != LINUX_OK)
                    // error
                {
                    bResult = FALSE;
    
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("Error at reading OffsetCapability!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                    
                    // leave loop
                    break;
                    
                }
                else
                {
                    // wait 100ms
                    if  (fnDelayThread( pDevExtIn,
                                        100,
                                        lDimErrorIn,
                                        sErrorOut))
                        // starting WaitTime of current thread ok
                    {
                        lWaitTmp++;
                        
                        if (lWaitTmp == 5)
                            // don't wait
                        {
                            // set ErrorString
                            fnBuildString(  _TEXT(__FUNCTION__),
                                            _TEXT("(): Error at Waiting for TransactionPending!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);
                            
                            // stop waiting
                            break;                          
                        }   
                    }
                }
            }
            
            if  (!((lTransactionsPending>>5) & 0x1))
                // no pending transaction exists
            {
                // success
                bResult = TRUE;
            }   
            else
            {
                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("Error at TransactionPending!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
        }
    }
    
    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  set FunctionLevelReset in ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnSetFlrAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
                               	const UINT32        lDimErrorIn,
                               	_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	bResult = TRUE;

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  check Vendor-/DeviceId
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnGetVendorDeviceIdAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
											UINT16*				pVendorIdOut,
											UINT16*				pDeviceIdOut,
											const UINT32        lDimErrorIn,
											_TCHAR*             sErrorOut)
{
BOOLEAN	bResult     = FALSE;
BOOLEAN lResultTmp 	= LINUX_OK;

	// get VendorId
	lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
										PCI_VENDOR_ID,
										pVendorIdOut);
	
	if  (lResultTmp != LINUX_OK)
		// error
	{
		// set ErrorString
		fnBuildString(  _TEXT(__FUNCTION__),
						_TEXT("Error at reading VendorId!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}
	else
	{
		// get DeviceId
		lResultTmp = pci_read_config_word(  pDevExtIn->pPciDev,
											PCI_DEVICE_ID,
											pDeviceIdOut);
		
		if  (lResultTmp != LINUX_OK)
			// error
		{
			// set ErrorString
			fnBuildString(  _TEXT(__FUNCTION__),
							_TEXT("Error at reading DeviceId!"),
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

    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable MSIX at ConfigSpace
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnEnableMsixAtConfigSpace(	DEVICE_EXTENSION*	pDevExtIn,
                                	const UINT32        lDimErrorIn,
                                	_TCHAR*             sErrorOut)
{
BOOLEAN     bResult             = LINUX_ERROR;


    return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  scan for desired device
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fnEvtDeviceProbe(			struct pci_dev*			pPciDevIn,
						const	struct pci_device_id*	pPciDevIdIn)
{
UINT32	lResult		= LINUX_ERROR;
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	lResult = pci_enable_device(pPciDevIn);

	if	(lResult != LINUX_OK)
		// error
	{
		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: ########## Error at pci_enable_device()!");

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,                       // logging
					sTraceDetail);

	}
	else
	{
	DEVICE_EXTENSION* pDevExt = NULL;

		// allocate DeviceExtension
		pDevExt = fnAllocMemNonPaged(	sizeof(DEVICE_EXTENSION),
										TAG('I','n','i','t'));

		if	(pDevExt == NULL)
		// error
		{
			lResult = LINUX_ERROR;
	
			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[x,x,x]: ########## Insufficient memory for DeviceExtension!");
	
			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,                       // logging
						sTraceDetail);
		}
		else
		{
			// preset DeviceExtension
			RtlZeroMemory(	(void*) pDevExt,
							sizeof(DEVICE_EXTENSION));
	
			// save PciDev at DeviceExtention
			pDevExt->pPciDev = pPciDevIn;
	
			if  (!fnEvtDeviceAdd(	pDevExt,
									pPciDevIdIn->driver_data,
									pPciDevIn->bus->number,			// Bus
									PCI_SLOT(pPciDevIn->devfn),		// Device
									PCI_FUNC(pPciDevIn->devfn)))	// Fucntion
				// error at CBF for AddDevice
			{
				lResult = LINUX_ERROR;
			}
			else
			{
				if	(!fnEvtDevicePrepareHw(pDevExt))
					// error at CBF for preparing hardware
				{
					lResult = LINUX_ERROR;
				}
				else
				{
					// put DeviceExtension to DeviceList
					fnPutBlkToList(	&g_uVarSys.uListDevice,
									((uLIST_HEADER*) pDevExt));
	
					g_uVarSys.lCtrDevice++;
				}
			}
		}
	}
	
	return(lResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get BoardType
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  get first free index of EventHandleTable
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  release used index of EventHandleTable
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------


//************************************************************************
//  D e s c r i p t i o n :
//
//  allocate physical contiguous memory for the DMA of the boards.
//  lAdrMin and lAdrMax is determined by the address window that the boards can access (determined by PCI/e Bridge!)
//  This function has a high chance of success if it is called at DriverEntry.
//  Due to memory fragmentaion it is more likely that the function will fail if it is called at a later time (e.g. AddDevice, OpenDevice...).
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------

#define PNDEVDRV_DMA_ALLOC_SIZE     (16 * 1024 * 1024)                                                  /* 16 MB */
#define PNDEVDRV_DMA_ALLOC_ALIGN    PNDEVDRV_DMA_ALLOC_SIZE                                             /* align allocated memory to its size */
#define DMA_ALLOC_MAX_DUMP_DEFAULT  2500                                                                /* entries for none fitting blocks */
#define DMA_MAX_ALLOC_BLOCK_SIZE    (4 * 1024 * 1024)                                                   /* 4 MB */
#define DMA_ALLOC_MAX_BUFFER        ((PNDEVDRV_DMA_ALLOC_SIZE / DMA_MAX_ALLOC_BLOCK_SIZE + 1) * 2 - 1)  /* entries for allocated contigious memory */ 

void fnMmAllocateContiguousMemory(  UINT32                          lSizeIn,
                                    const UINT32                    lAdrMinIn,
                                    const UINT32                    lAdrMaxIn,
                                    uPNDEV_HW_DESC_CONT_MEM_HOST*   pContMemOut)
{
char sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};

    if (lSizeIn == PNDEVDRV_DMA_ALLOC_SIZE /* 16MB or 8 MB*/)
        /* split in smaller alloc sections (max. alloc limit is DMA_MAX_ALLOC_BLOCK_SIZE)
         *
         * algorithm goes like this:
         * - array pBufVirt: 2 * blocks needed (contains virt address of physical aligned blocks, when finished)
         * - array pBufPhys: contains phy adress of allocated blocks (just for saving physical addresses)
         *
         * Step 1: - allocate a 4MB (DMA_MAX_ALLOC_BLOCK_SIZE) block and save in the middle of the array (count == 0)
         * Step 2: - allocate next 4MB block, and place in arrays (pBufVirt and pBufPhys) at
         *         -- index + 1, if adr is bigger then the adr of the last allocated block
         *         -- index - 1, if adr is smaller then the adr of the last allocated block
         * Step 3: - check if array pBufVirt is BOTTOM (index == 0) or TOP (index == DMA_ALLOC_MAX_BUFFER)
         *         -- BOTTOM: return virt and phys adr at index = 0 (blocks are descending ordered: last block allocated has return adr)
         *         -- TOP: return virt and phys adr at index = (DMA_ALLOC_MAX_BUFFER/2), what is the first block allocated. (blocks are ascending ordered)
         *         -- not BOTTOM nor TOP: goTo Step 2 and allocate next block
         */
    {
    struct file*    pFile;
    mm_segment_t    old_fs;
    UINT64          uDumpArrayCount = 0;
    UINT64          uCmaFreeMem 	= 0;
    BOOLEAN			bCmaConfigured	= FALSE;
    
        /* Read size of  */
        old_fs = get_fs();  /* save current FS segment before reading */
        set_fs(get_ds());
    
        pFile = filp_open("/proc/meminfo", O_RDONLY, 0);

        if(pFile == NULL)
            /* error, could not open file */
        {
            uDumpArrayCount = DMA_ALLOC_MAX_DUMP_DEFAULT;    /* default value, works on Systems with less than 16GB of RAM */
            
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: WARNING could not open /proc/meminfo. Take default value for bufferlist: %llu entries",
                                uDumpArrayCount);

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
        else
        	/* Read size of installed RAM and free CMA Memory out of file */
        {
		loff_t          pos    		= 0;
		// 4 mb should always be enough to read whole file, /proc/meminfo file size property does not exist, since it's no real file!
		size_t			lSizeBytes	= (4 * 1024 * 1024);
		char*			sContent 	= (char*) vmalloc(sizeof(char) * lSizeBytes );
		char*			sCmaFree;
		
			if(sContent && vfs_read(pFile, sContent, lSizeBytes, &pos))
			{
			UINT64  uRAM;
				if(sscanf(sContent, "MemTotal: %llu kB", &uRAM) == 1)
				{
					uDumpArrayCount =  (uRAM * 1024 /* Bytes */) / DMA_MAX_ALLOC_BLOCK_SIZE; //Number of max amount of not fittig blocks to store in dumplist
				}
				sCmaFree = strstr(sContent, "CmaFree"); 
				if(sCmaFree && sscanf(sCmaFree, "CmaFree: %llu kB", &uRAM) == 1)
				{
					bCmaConfigured = TRUE;
					uCmaFreeMem = (uRAM * 1024 /* Bytes */);
				}
				else
				{
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[x,x,x]: CmaFree section not found in /proc/meminfo");

					/* set trace */
					fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
								ePNDEV_TRACE_CONTEXT_BASIC,
								TRUE,                       /* logging */
								sTraceDetail);
				}
			}
			else
			{
				uDumpArrayCount = DMA_ALLOC_MAX_DUMP_DEFAULT;    /* default value, works on Systems with less than 16GB of RAM */

	            RtlStringCchPrintfA(sTraceDetail,
	                                _countof(sTraceDetail),
	                                "PnDev[x,x,x]: WARNING could not read /proc/meminfo. Take default value for bufferlist: %llu entries",
	                                uDumpArrayCount);

	            /* set trace */
	            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	                        ePNDEV_TRACE_CONTEXT_BASIC,
	                        TRUE,                       /* logging */
	                        sTraceDetail);
			}
			if(sContent)
			{
				vfree(sContent);
			}

            filp_close(pFile,NULL);  /* close file */
        
            set_fs(old_fs); /* Reset stored FS */

//            /* Info */
//            RtlStringCchPrintfA(sTraceDetail,
//                                _countof(sTraceDetail),
//                                "PnDev[x,x,x]: Found %llukB of RAM: %llu entries for bufferlist",
//                                (uDumpArrayCount * DMA_MAX_ALLOC_BLOCK_SIZE /* Bytes */) / 1024,
//                                uDumpArrayCount);
//
//            /* set trace */
//            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
//                        ePNDEV_TRACE_CONTEXT_BASIC,
//                        TRUE,                       /* logging */
//                        sTraceDetail);
        }
        
        if(bCmaConfigured)
        {
			if(uCmaFreeMem >= lSizeIn)
			{
				// Allocate contiguous memory with CMA using DMA API dma_alloc_coherent()
				// This needs Continuos Memory Allocator (CMA) enabled and DMA CMA enabled in Linux Kernel.
				// Also PnDevDrv.ko has to be built with CMA and DMA CMA enabled settings!
				pContMemOut->pAdrVirt = dma_alloc_coherent(NULL, lSizeIn, &pContMemOut->lDmaAdr, GFP_KERNEL);
				
				if(pContMemOut->pAdrVirt != NULL)
				{
					pContMemOut->lPhyAdr 	= virt_to_phys(pContMemOut->pAdrVirt);
					pContMemOut->lSize 		= lSizeIn;
					
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[x,x,x]: allocated contiguous memory with dma_alloc_coherent(): dma=0x%x, virt=0x%p, phys=0x%x, size=%dB",
										(UINT32)pContMemOut->lDmaAdr,
										pContMemOut->pAdrVirt,
										pContMemOut->lPhyAdr,
										pContMemOut->lSize);
		
					/* set trace */
					fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
								ePNDEV_TRACE_CONTEXT_BASIC,
								TRUE,                       /* logging */
								sTraceDetail);
				}
			}
			else
			{
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[x,x,x]: FAILED to allocate Contiguous Memory - not enough cma memory left (needed: 0x%x, CmaFree: 0x%Lx)",
									lSizeIn,
									uCmaFreeMem);

				/* set trace */
				fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							TRUE,                       /* logging */
							sTraceDetail);
			}
        }
    	else
    	{
		char**          pDumpVirt       = NULL;
		char**          pBufVirt        = NULL;
		dma_addr_t*     uDumpPhys       = NULL;
		dma_addr_t*     uBufPhys        = NULL;
		INT32           uDumpIndex      = 0;

			/* buffers for saving none fitting blocks */
			pDumpVirt = (char**) vmalloc(sizeof(char*) * uDumpArrayCount );
			uDumpPhys = (dma_addr_t*) vmalloc(sizeof(dma_addr_t) * uDumpArrayCount );
			
			/* buffer for fitting blocks */
			pBufVirt = (char**) vmalloc(sizeof(char*) * DMA_ALLOC_MAX_BUFFER);
			uBufPhys = (dma_addr_t*) vmalloc(sizeof(dma_addr_t) * DMA_ALLOC_MAX_BUFFER);
				
			if(pDumpVirt == NULL || uDumpPhys == NULL || pBufVirt == NULL || uBufPhys == NULL)
				/* error - pDumpVirt or uDumpPhys, pBufVirt or uBufPhys couldn't be allocated */
			{
	
			}
			else
				/* null the memory */
			{
			UINT32 		i                   = 0;
			UINT16  	index               = DMA_ALLOC_MAX_BUFFER / 2;
			UINT8   	bFirstBlock         = 1;
			char*   	pAddrVirtFirstBlock = NULL;
			uintptr_t 	uDmaAlign      		= 0;
	
				for(i = 0; i < uDumpArrayCount; i++)
				{
					pDumpVirt[i] = NULL;
				}
				
				for(i = 0; i < DMA_ALLOC_MAX_BUFFER; i++)
				{
					pBufVirt[i] = NULL;
				}
	
				/* start with the algorithm */
				while(index >= 0 && index < DMA_ALLOC_MAX_BUFFER)
				{
				char*       pAddrVirt;
				dma_addr_t  uAddrPhys;
		
					/* allocate one block (4MB) in whole kernel space */
					#ifdef PNDEV_KM_PLATFORM_32
					pAddrVirt = kmalloc(DMA_MAX_ALLOC_BLOCK_SIZE, GFP_KERNEL | GFP_DMA32);
					uAddrPhys = virt_to_phys(pAddrVirt);
					#else
					pAddrVirt = dma_alloc_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE, &uAddrPhys, GFP_KERNEL);
					#endif
	
	//				RtlStringCchPrintfA(sTraceDetail,
	//									_countof(sTraceDetail),
	//									"PnDev[x,x,x]: kmalloc pAddrVirt=0x%p, uDumpIndex=0x%x, index=0x%x",
	//									pAddrVirt,									
	//									uDumpIndex,
	//									index);
	//
	//				/* set trace */
	//				fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	//							ePNDEV_TRACE_CONTEXT_BASIC,
	//							TRUE,                       /* logging */
	//							sTraceDetail);
				
					if(pAddrVirt == NULL)
						/* error - not enough RAM for allocation of 4MB (DMA_MAX_ALLOC_BLOCK_SIZE) Block*/
					{
						RtlStringCchPrintfA(sTraceDetail,
											_countof(sTraceDetail),
											"PnDev[x,x,x]: FAILED to allocate Contiguous Memory - could not allocate block of size=%dB",
											DMA_MAX_ALLOC_BLOCK_SIZE);
		
						/* set trace */
						fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
									ePNDEV_TRACE_CONTEXT_BASIC,
									TRUE,                       /* logging */
									sTraceDetail);
		
						for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
						{
							if(pBufVirt[index] != NULL)
							{
								/* free all saved buffers and exit*/
								#ifdef PNDEV_KM_PLATFORM_32
								kfree(pBufVirt[index]);
								#else
								dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) pBufVirt[index], uBufPhys[index]);
								#endif
							}
						}
		
						break;
					}
					else if (lAdrMaxIn > 0 && (UINT64)uAddrPhys >= (UINT64) (lAdrMaxIn - DMA_MAX_ALLOC_BLOCK_SIZE))
						/* error - memory of allocated block is above max address (lAdrMaxIn) */
					{
						if(uDumpIndex >= uDumpArrayCount)
							/* error - not enough space in pDumpVirt and uDumpPhys array - Free and EXIT*/
						{
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[x,x,x]: FAILED to allocate Contiguous Memory - not enough space to save overaddressed memory blocks");
		
							/* set trace */
							fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,                       /* logging */
										sTraceDetail);
		
							for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
							{
								if(pBufVirt[index] != NULL)
								{
									/* free saved blocks, if there are some */
									#ifdef PNDEV_KM_PLATFORM_32
									kfree(pBufVirt[index]);
									#else
									dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) pBufVirt[index], uBufPhys[index]);
									#endif
								}
							}
		
							break;
						}
						else
							/* put block in dummybufferlist and get next block */
						{
	//						RtlStringCchPrintfA(sTraceDetail,
	//											_countof(sTraceDetail),
	//											"PnDev[x,x,x]: put block in dummybufferlist and get next block pAddrVirt=0x%p, uDumpIndex=0x%x, index=0x%x",
	//											pAddrVirt,									
	//											uDumpIndex,
	//											index);
	//						/* set trace */
	//						fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	//									ePNDEV_TRACE_CONTEXT_BASIC,
	//									TRUE,                       /* logging */
	//									sTraceDetail);
	
							pDumpVirt[uDumpIndex] = pAddrVirt;
							uDumpPhys[uDumpIndex] = uAddrPhys;
							uDumpIndex++;
		
							continue;
						}
					}
					else if (bFirstBlock == 1)
						/* first block is initialization */
					{
					uintptr_t uBlockOffset   = ((uintptr_t)pAddrVirt % (uintptr_t)DMA_MAX_ALLOC_BLOCK_SIZE);
					char*  pAddrNextBlock    = (char*)(((uintptr_t)pAddrVirt + ((uintptr_t)DMA_MAX_ALLOC_BLOCK_SIZE - uBlockOffset)));
					char*  pAddrNextDma      = NULL;
	
					uDmaAlign      = ((uintptr_t)pAddrVirt % (uintptr_t)PNDEVDRV_DMA_ALLOC_ALIGN);
					pAddrNextDma   = (char*)(((uintptr_t)pAddrVirt + ((uintptr_t)PNDEVDRV_DMA_ALLOC_ALIGN - uDmaAlign)));
					
						if	(	(uDmaAlign == (uintptr_t)0) 
							|| 	(pAddrNextBlock == pAddrNextDma))
							/* first block fits in dma alignment or contains start of requested dma alignment! */
						{
							pAddrVirtFirstBlock = pAddrVirt;
							pBufVirt[index] = pAddrVirt;
							uBufPhys[index] = uAddrPhys;
							bFirstBlock = 0;
							
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[x,x,x]: allocated first block of contiguous memory: pAddrVirtFirstBlock=0x%p, pAddrVirt=0x%p, pAddrNextBlock=0x%p, pAddrNextDma=0x%p",
												pAddrVirtFirstBlock,
												pAddrVirt,
												pAddrNextBlock,
												pAddrNextDma);
			
							/* set trace */
							fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,                       /* logging */
										sTraceDetail);
							
							continue;
						}
						else
						{
							/* put block in dummybufferlist and get next block */
	
	//						RtlStringCchPrintfA(sTraceDetail,
	//											_countof(sTraceDetail),
	//											"PnDev[x,x,x]: (bFirstBlock == 1) put block in dummybufferlist and get next block pAddrVirt=0x%p, uDumpIndex=0x%x, index=0x%x",
	//											pAddrVirt,									
	//											uDumpIndex,
	//											index);
	//						/* set trace */
	//						fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	//									ePNDEV_TRACE_CONTEXT_BASIC,
	//									TRUE,                       /* logging */
	//									sTraceDetail);
	
	
							pDumpVirt[uDumpIndex] = pAddrVirt;
							uDumpPhys[uDumpIndex] = uAddrPhys;
							uDumpIndex++;
		
							continue;
						}
					}
					else
						/* successfully allocated */
					{
						if((UINT64)uBufPhys[index] - (UINT64)uAddrPhys == (UINT64)DMA_MAX_ALLOC_BLOCK_SIZE)
							/* success - allocated buffer is below last allocated buffer */
						{
							if(index == 0)
								/* error - buffer full */
							{
								break;
							}
							else
								/* success - store next blockaddress in array */
							{
								index = index - 1;
								pBufVirt[index] = pAddrVirt;
								uBufPhys[index] = uAddrPhys;
							}
		
						}
						else if((UINT64)uAddrPhys - (UINT64)uBufPhys[index] == (UINT64)DMA_MAX_ALLOC_BLOCK_SIZE)
							/* success - allocated buffer is above last allocated buffer */
						{
							if(index > (DMA_ALLOC_MAX_BUFFER - 1) )
								/* error - buffer full */
							{
								break;
							}
							else
								/* success - store next blockaddress in array */
							{
								index = index + 1;
								pBufVirt[index] = pAddrVirt;
								uBufPhys[index] = uAddrPhys;
							}
		
						}
						else
							/* error - allocated block is not directly fitting to block allocated bevor: memory not physically contiguous */
						{
						UINT8 result = 1;
						
							for(i = 0; i < DMA_ALLOC_MAX_BUFFER; i++)
								/* move so far allocated blocks in dump list - will be freed at end of function*/
							{
								if(pBufVirt[i] != NULL)
								{
									if(uDumpIndex >= uDumpArrayCount)
										/* error - not enough space in pDumpVirt and uDumpPhys array */
									{
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[x,x,x]: FAILED to allocate Contiguous Memory - not enough space to save incontiguous memory blocks");
		
										/* set trace */
										fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
													ePNDEV_TRACE_CONTEXT_BASIC,
													TRUE,                       /* logging */
													sTraceDetail);
		
										for(index = 0; index < DMA_ALLOC_MAX_BUFFER; index++)
											/* free allocated blocks in buffer list */
										{
											if(pBufVirt[index] != NULL)
											{
												/* free saved blocks */
												#ifdef PNDEV_KM_PLATFORM_32
												kfree(pBufVirt[index]);
												#else
												dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) pBufVirt[index], uBufPhys[index]);
												#endif
											}
										}
										result = 0;
										break;
									}
									else
										/* success - move block */
									{
	
	//									RtlStringCchPrintfA(sTraceDetail,
	//														_countof(sTraceDetail),
	//														"PnDev[x,x,x]: move block pBufVirt[0x%x]=0x%p, pDumpVirt[0x%x]=0x%p, index=0x%x",
	//														i,
	//														pBufVirt[i],									
	//														uDumpIndex,
	//														pDumpVirt[uDumpIndex],
	//														index);
	//									/* set trace */
	//									fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	//												ePNDEV_TRACE_CONTEXT_BASIC,
	//												TRUE,                       /* logging */
	//												sTraceDetail);
	
	
										pDumpVirt[uDumpIndex] = pBufVirt[i];
										uDumpPhys[uDumpIndex] = uBufPhys[i];
										pBufVirt[i] = NULL; /* set buffer empty */
										uDumpIndex++;
									}
								}
							}
							if(result == 0)
								/* error in loop above */
							{
								break;
							}
							
							/* start algorithm with new startblock */
							index = DMA_ALLOC_MAX_BUFFER / 2;
							bFirstBlock         = 1;
							pAddrVirtFirstBlock = NULL;
							pDumpVirt[uDumpIndex] = pAddrVirt;
							uDumpPhys[uDumpIndex] = uAddrPhys;
							uDumpIndex++;
		
							continue;
						}
					}
		
					/* check if 4 blocks were allocated and 16MB are reached*/
					if(index == 0)
						/* got descending memory, return last allocated block */
					{
						pContMemOut->pAdrVirtStart  = pAddrVirt;
						pContMemOut->lPhyAdr        = uBufPhys[0];
						pContMemOut->pAdrVirt       = pBufVirt[0];
						pContMemOut->lSize          = PNDEVDRV_DMA_ALLOC_SIZE;
					}
					else if(index == (DMA_ALLOC_MAX_BUFFER - 1))
						/* got ascending memory, return first allocated block */
					{
						pContMemOut->pAdrVirtStart  = pAddrVirtFirstBlock;
						pContMemOut->lPhyAdr        = uBufPhys[DMA_ALLOC_MAX_BUFFER / 2];
						pContMemOut->pAdrVirt       = pBufVirt[DMA_ALLOC_MAX_BUFFER / 2];
						pContMemOut->lSize          = PNDEVDRV_DMA_ALLOC_SIZE; /* 16MB */
					}
	
					uDmaAlign = ((uintptr_t)pContMemOut->pAdrVirt % (uintptr_t)PNDEVDRV_DMA_ALLOC_ALIGN);
	
					if(uDmaAlign != (uintptr_t)0)
						/* first block does not fit in dma alignment! */
					{
						pContMemOut->pAdrVirt = (char*)(((uintptr_t)pContMemOut->pAdrVirt + ((uintptr_t)PNDEVDRV_DMA_ALLOC_ALIGN - uDmaAlign)));
						pContMemOut->lPhyAdr  = virt_to_phys(pContMemOut->pAdrVirt);
					}
					
					/* Output success */
					if(index == 0  || index == (DMA_ALLOC_MAX_BUFFER - 1))
					{
						RtlStringCchPrintfA(sTraceDetail,
											_countof(sTraceDetail),
											"PnDev[x,x,x]: allocated contiguous memory: start=0x%p, virt=0x%p, phys=0x%x, size=%dB",
											pContMemOut->pAdrVirtStart,
											pContMemOut->pAdrVirt,
											pContMemOut->lPhyAdr,
											pContMemOut->lSize);
		
						/* set trace */
						fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
									ePNDEV_TRACE_CONTEXT_BASIC,
									TRUE,                       /* logging */
									sTraceDetail);
		
						/* all done - quit while loop*/
						break;
					}
				} /* while loop */
				
				/* Free lists with memory addresses */
				if(pDumpVirt)
				{
					vfree(pBufVirt);
				}
				if(uBufPhys)
				{
					vfree(uBufPhys);
				}
				
				/* Free list of dummybuffers if they were used */
				for (uDumpIndex--; uDumpIndex >= 0 && uDumpIndex < uDumpArrayCount ; uDumpIndex--)
				{
					if(pDumpVirt[uDumpIndex] != NULL)
					{
	//					RtlStringCchPrintfA(sTraceDetail,
	//										_countof(sTraceDetail),
	//										"PnDev[x,x,x]: free pDumpVirt[0x%x]=0x%p",
	//										uDumpIndex,
	//										pDumpVirt[uDumpIndex]);
	//	
	//					/* set trace */
	//					fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
	//								ePNDEV_TRACE_CONTEXT_BASIC,
	//								TRUE,                       /* logging */
	//								sTraceDetail);
	
						#ifdef PNDEV_KM_PLATFORM_32
						kfree(pDumpVirt[uDumpIndex]);
						#else
						dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) pDumpVirt[uDumpIndex], uDumpPhys[uDumpIndex]);
						#endif
					}
				}
				
				/* free the save lists */
				if(pDumpVirt)
				{
					vfree(pDumpVirt);
				}
				if(uDumpPhys)
				{
					vfree(uDumpPhys);
				}
			}
    	}
    }
    else
        /* alloc memory until 4MB and exit */
    {
    char*       pVirt;
    dma_addr_t  uPhys;

		#ifdef PNDEV_KM_PLATFORM_32
        pVirt = kmalloc(lSizeIn, GFP_KERNEL);
        uPhys = virt_to_phys(pVirt);
		#else
        pVirt = dma_alloc_coherent(NULL, lSizeIn, &uPhys, GFP_KERNEL);
		#endif

        if(pVirt == NULL)
            /* Not enough space for allocation of lSizeIn Kbytes */
        {
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev - FAILED to allocate Contiguous Memory - not enough memory");

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
        else
        {
            pContMemOut->pAdrVirt = pVirt;
            pContMemOut->lPhyAdr = uPhys;
            pContMemOut->lSize = lSizeIn;
            
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: allocated contiguous memory: virt=0x%p, phys=0x%x, size=%dB",
                                pContMemOut->pAdrVirt,
                                pContMemOut->lPhyAdr,
                                pContMemOut->lSize);

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
    }
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void fnMmFreeContiguousMemory(uPNDEV_HW_DESC_CONT_MEM_HOST *pContMemIn)
{
    if(pContMemIn->pAdrVirt != NULL)
    {
    char    sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};
    
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[x,x,x]: free contiguous memory: dma=0x%x, start=0x%p, virt=0x%p, phys=0x%x, size=%dB",
							(UINT32)pContMemIn->lDmaAdr,
							pContMemIn->pAdrVirtStart,
							pContMemIn->pAdrVirt,
							pContMemIn->lPhyAdr,
							pContMemIn->lSize);
	
		/* set trace */
		fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
					ePNDEV_TRACE_CONTEXT_BASIC,
					TRUE,                           /* logging */
					sTraceDetail);

		if(pContMemIn->lDmaAdr != 0)
		{
			dma_free_coherent(NULL, pContMemIn->lSize,(void *) pContMemIn->pAdrVirt, pContMemIn->lDmaAdr);
		}
		else if(pContMemIn->lSize == PNDEVDRV_DMA_ALLOC_SIZE)
            /* free the 16MB aligned buffer in 5 steps 16 MB buffer + 4MB alignment block*/
        {
			#ifdef PNDEV_KM_PLATFORM_32
            kfree((void *) (pContMemIn->pAdrVirtStart + 4 * DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) (pContMemIn->pAdrVirtStart + 3 * DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) (pContMemIn->pAdrVirtStart + 2 * DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) (pContMemIn->pAdrVirtStart + DMA_MAX_ALLOC_BLOCK_SIZE));
            kfree((void *) pContMemIn->pAdrVirtStart);
			#else
		    dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) (pContMemIn->pAdrVirtStart + 4 * DMA_MAX_ALLOC_BLOCK_SIZE), (pContMemIn->lPhyAdr + 4 * DMA_MAX_ALLOC_BLOCK_SIZE));
		    dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) (pContMemIn->pAdrVirtStart + 3 * DMA_MAX_ALLOC_BLOCK_SIZE), (pContMemIn->lPhyAdr + 3 * DMA_MAX_ALLOC_BLOCK_SIZE));
		    dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) (pContMemIn->pAdrVirtStart + 2 * DMA_MAX_ALLOC_BLOCK_SIZE), (pContMemIn->lPhyAdr + 2 * DMA_MAX_ALLOC_BLOCK_SIZE));
		    dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) (pContMemIn->pAdrVirtStart + 1 * DMA_MAX_ALLOC_BLOCK_SIZE), (pContMemIn->lPhyAdr + 1 * DMA_MAX_ALLOC_BLOCK_SIZE));
		    dma_free_coherent(NULL, DMA_MAX_ALLOC_BLOCK_SIZE,(void *) (pContMemIn->pAdrVirtStart + 0 * DMA_MAX_ALLOC_BLOCK_SIZE), (pContMemIn->lPhyAdr + 0 * DMA_MAX_ALLOC_BLOCK_SIZE));
			#endif
        }
        else
            /* free the buffer*/
        {
			#ifdef PNDEV_KM_PLATFORM_32
            kfree((void *) pContMemIn->pAdrVirt);
			#else
            dma_free_coherent(NULL, pContMemIn->lSize,(void *) pContMemIn->pAdrVirt, pContMemIn->lPhyAdr);
			#endif
        }
        
        /* reset host struct */
        pContMemIn->bInUse         = FALSE;
        pContMemIn->lPhyAdr        = 0;
        pContMemIn->pAdrVirt       = NULL;
        pContMemIn->lSize          = 0;
        pContMemIn->pAdrVirtStart  = NULL;
        pContMemIn->lDmaAdr        = 0;
    }
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  OS abstraction for NetProfi Kernel Interrupt handling
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnNetProfiIntHandler(   DEVICE_EXTENSION* pDevExtIn)
{
    // not supported in Linux
    return FALSE;
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Kernel file operations - OpenForRead
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnFileOpenForRead(  DEVICE_EXTENSION* pDevExtIn,
              HANDLE*       pFileHandle,
              _TCHAR*       sPath,
              UINT32*       lFileSize)
{
char 		sTraceDetail[PNDEV_SIZE_STRING_BUF] = {0};
BOOLEAN		bResult = FALSE;
struct file*    pFile;
mm_segment_t    old_fs;

	old_fs = get_fs();  /* save current FS segment before reading */
	set_fs(get_ds());

	pFile = filp_open(sPath, O_RDONLY, 0);

       if(pFile == NULL)
            /* error, could not open file */
        {            
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: Could not open file %s",
                                sPath);

            /* set trace */
            fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       /* logging */
                        sTraceDetail);
        }
        else
        	/* Read */
        {
        	struct kstat pStats;

        	int error = vfs_stat (sPath, &pStats);

        	if(error)
        	{
                RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: Could not open file - error 0x%x",
                                error);

                /* set trace */
                fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       /* logging */
                            sTraceDetail);
        	}
        	else
        	{
        		UINT8* pFileData = NULL;

    			// allocate memory for the file
        		pFileData = fnAllocMemNonPaged(   	pStats.size * sizeof(UINT8),
													TAG('F','i','l','e'));

        		if(!pFileData)
        		{
                    RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: Could not allocate memory for file data.");

                    /* set trace */
                    fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                                ePNDEV_TRACE_CONTEXT_BASIC,
                                TRUE,                       /* logging */
                                sTraceDetail);
        		}
        		else
        		{
        			loff_t          pos    		= 0;

        			vfs_read(pFile, pFileData, pStats.size, &pos);

        			*lFileSize   = (UINT32)pStats.size;
        			*pFileHandle = (HANDLE*)pFileData;

        			bResult = TRUE;
        		}
        	}

			// close the file
			filp_close(pFile,NULL);  /* close file */
        }

	set_fs(old_fs); /* Reset stored FS */

	return bResult;

}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Kernel file operations - Close
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnFileReadBytes(  DEVICE_EXTENSION* pDevExtIn,
              HANDLE        hFile,
              UINT32        lFileOffset,
              UINT32        lBufferSize,
              UINT8*        pDataBuffer)
{
UINT32 lLoop = 0;
UINT8* pFileData = NULL;

	pFileData = (UINT8*)hFile;

	for(lLoop = 0; lLoop < lBufferSize; lLoop++)
	{
		pDataBuffer[lLoop] = pFileData[lLoop + lFileOffset];
	}

	return TRUE;
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Kernel file operations - Close
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnFileClose(  DEVICE_EXTENSION* pDevExtIn,
            HANDLE  hFile)
{
BOOLEAN	bResult	= FALSE;

	if (hFile != NULL)
	{
		fnFreeMemNonPaged(hFile, TAG('F','i','l','e'));

		bResult = TRUE;
	}

	return (bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux FunctionOption .open
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fnOpenDriver(   struct inode *inode,
                    struct file *file)
{
UINT32 lResult = LINUX_ERROR;

    if  (try_module_get(THIS_MODULE))
        // got module reference
    {
        lResult = LINUX_OK;
    }

    return lResult;
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux FunctionOption .release
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fnCloseDriver(  struct inode *inode,
                    struct file *file)
{
    // release module reference
    module_put(THIS_MODULE);

    return LINUX_OK;
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux RetrieveIoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN RetrieveIoctlBuffer(unsigned int        lCmdIn,
                            unsigned long       lArgIn,
                            uPNDEV_IOCTL_ARG*   pIoctlArgIn,
                            uPNDEV64_PTR_VOID*  pInBufferIn,
                            uPNDEV64_PTR_VOID*  pOutBufferIn)
{
BOOLEAN bResult         = FALSE;
BOOLEAN bResultIoctl    = FALSE;
INT32   lResultTmp      = LINUX_ERROR;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};


    // check input arguments
    {
        if  (_IOC_TYPE(lCmdIn) != PNDEV_IOC_MAGIC)
            // no valid MagicNumber included
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Invalid MagicNumber in IoctlCmd!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);

        }
        else
        {
            if  (_IOC_DIR(lCmdIn) & _IOC_READ)
                // Ioctl has ReadDirection
            {
                // check if Useradress is writable
                lResultTmp = access_ok( VERIFY_WRITE,
                                        (void __user *) lArgIn,
                                        sizeof(uPNDEV_IOCTL_ARG));

                if  (   (_IOC_DIR(lCmdIn) & _IOC_WRITE)
                    &&  (lResultTmp > 0))
                    // Ioctl has WriteDirection
                {
                    lResultTmp = access_ok( VERIFY_READ,
                                            (void __user *) lArgIn,
                                            sizeof(uPNDEV_IOCTL_ARG));

                    if  (lResultTmp > 0)
                        // success
                    {
                        bResultIoctl = TRUE;
                    }
                    else
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[x,x,x]: ########## Error at access_ok()!");

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                }
            }
        }
    }

    if  (!bResultIoctl)
        // error
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## RetrieveIoctlBuffer access error on buffer");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);

    }
    else
    {
        // get IoctlArg
        lResultTmp = copy_from_user((void*) pIoctlArgIn,
                                    (void __user *) lArgIn,
                                    sizeof(uPNDEV_IOCTL_ARG));

        if  (lResultTmp > 0)
            // error at copy data
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at copy_from_user!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            // alloc InBuffer
            pInBufferIn->pPtr = vmalloc(pIoctlArgIn->lSizeInputBuf);

            // alloc OutBuffer
            pOutBufferIn->pPtr = vmalloc(pIoctlArgIn->lSizeOutputBuf);

            if  (   (pInBufferIn->pPtr  == NULL)
                ||  (pOutBufferIn->pPtr == NULL))
                // no valid ptr
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at vmalloc for InBuffer oder OutBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // get uIn
                lResultTmp = copy_from_user((void*) pInBufferIn->pPtr,
                                            (void __user *) pIoctlArgIn->uIn.pPtr,
                                            pIoctlArgIn->lSizeInputBuf);

                if  (lResultTmp > 0)
                    // error at copy data
                {
                    // set TraceDetail
                    RtlStringCchPrintfA(sTraceDetail,
                                        _countof(sTraceDetail),
                                        "PnDev[x,x,x]: ########## Error at copy_from_user InBuffer!");

                    // set trace
                    fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                ePNDEV_TRACE_CONTEXT_BASIC,
                                TRUE,                       // logging
                                sTraceDetail);
                }
                else
                {
                    // get uOut
                    lResultTmp = copy_from_user((void*) pOutBufferIn->pPtr,
                                                (void __user *) pIoctlArgIn->uOut.pPtr,
                                                pIoctlArgIn->lSizeOutputBuf);

                    if  (lResultTmp > 0)
                        // error at copy data
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[x,x,x]: ########## Error at copy_from_user OutBuffer!");

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
                    else
                    {
                        // success
                        bResult = TRUE;
                    }
                }
            }
        }
    }

    return (bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux ReturnIoctlBuffer
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN ReturnIoctlBuffer(  uPNDEV_IOCTL_ARG*   pIoctlArgIn,
                            uPNDEV64_PTR_VOID*  pInBufferIn,
                            uPNDEV64_PTR_VOID*  pOutBufferIn)
{
BOOLEAN bResult         = FALSE;
INT32   lResultTmp      = LINUX_ERROR;
char    sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

    if  (   (pInBufferIn  == NULL)
        ||  (pOutBufferIn == NULL))
        // no valid ptr
    {
        // set TraceDetail
        RtlStringCchPrintfA(sTraceDetail,
                            _countof(sTraceDetail),
                            "PnDev[x,x,x]: ########## No valid Ptr for InBuffer or OutBuffer!");

        // set trace
        fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                    ePNDEV_TRACE_CONTEXT_BASIC,
                    TRUE,                       // logging
                    sTraceDetail);
    }
    else
    {
        // get uIn
        lResultTmp = copy_to_user(  (void __user *) pIoctlArgIn->uIn.pPtr,
                                    (void*) pInBufferIn->pPtr,
                                    pIoctlArgIn->lSizeInputBuf);

        if  (lResultTmp > 0)
            // error at copy data
        {
            // set TraceDetail
            RtlStringCchPrintfA(sTraceDetail,
                                _countof(sTraceDetail),
                                "PnDev[x,x,x]: ########## Error at copy_to_user InBuffer!");

            // set trace
            fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                        ePNDEV_TRACE_CONTEXT_BASIC,
                        TRUE,                       // logging
                        sTraceDetail);
        }
        else
        {
            // get uOut
            lResultTmp = copy_to_user(  (void __user *) pIoctlArgIn->uOut.pPtr,
                                        (void*) pOutBufferIn->pPtr,
                                        pIoctlArgIn->lSizeOutputBuf);

            if  (lResultTmp > 0)
                // error at copy data
            {
                // set TraceDetail
                RtlStringCchPrintfA(sTraceDetail,
                                    _countof(sTraceDetail),
                                    "PnDev[x,x,x]: ########## Error at copy_to_user OutBuffer!");

                // set trace
                fnSetTrace( ePNDEV_TRACE_LEVEL_ERROR,
                            ePNDEV_TRACE_CONTEXT_BASIC,
                            TRUE,                       // logging
                            sTraceDetail);
            }
            else
            {
                // free InBuffer
                vfree(pInBufferIn->pPtr);

                // free OutBuffer
                vfree(pOutBufferIn->pPtr);

                // success
                bResult = TRUE;
            }
        }
    }

    return (bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Linux Map Memory to Usermode
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
int fnEvtMmap(  struct file* pFdIn,
                struct vm_area_struct* pVmaIn)
{
INT32 lResult = LINUX_ERROR;

    // mark page protection value as uncached
    pVmaIn->vm_page_prot = pgprot_noncached(pVmaIn->vm_page_prot);

    if(io_remap_pfn_range(pVmaIn, pVmaIn->vm_start, pVmaIn->vm_pgoff, pVmaIn->vm_end - pVmaIn->vm_start, pVmaIn->vm_page_prot))
    {
        lResult = -EAGAIN;
    }
    else
    {
        lResult = LINUX_OK;
    }

    return (lResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//  Callback functions for Linux
//  Check PciLoc in Array of UserInpput
//************************************************************************

//------------------------------------------------------------------------
//------------------------------------------------------------------------
BOOLEAN fnCheckPciLocIn(PNDEV_CHAR*		sPciLocIn[],
						const UINT32	lDimArrayIn,
						UINT32			pBusIn,
						UINT32			pDevIn,
						UINT32			pFuncIn)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bListEmpty 	= FALSE;
UINT32	i			= 0;
PNDEV_CHAR sPciLocInTmp[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

	// set PciLocShort
	_RtlStringCchPrintf(sPciLocInTmp,
						_countof(sPciLocInTmp),
						_TEXT("%i/%i/%i"),
						pBusIn,
						pDevIn,
						pFuncIn);

	// preset
	bListEmpty = TRUE;

	// check if parameter list is empty
	for	(i = 0; i < lDimArrayIn; i++)
		// all entries of Array PciLocIn
	{
		if	(sPciLocIn[i] != NULL)
			// element not empty
		{
			bListEmpty = FALSE;

			break;
		}
	}

	if (bListEmpty)
		// List is empty
	{
		// success, because all boards should be added
		bResult = TRUE;
	}
	else
	{
		for	(i = 0; i < lDimArrayIn; i++)
			// all entries of Array PciLocIn
		{
			if (sPciLocIn[i] != NULL)

			{
				if	(_tcscmp(sPciLocInTmp, sPciLocIn[i]) == 0)
					// device found
				{
					// success
					bResult = TRUE;

					// leave loop
					break;
				}
			}
		}
	}

	return(bResult);
}
//------------------------------------------------------------------------

//************************************************************************
//  D e s c r i p t i o n :
//
//************************************************************************
//------------------------------------------------------------------------
//------------------------------------------------------------------------

MODULE_LICENSE("GPL");

module_init(DriverEntry);
module_exit(fnEvtDriverUnload);
//------------------------------------------------------------------------

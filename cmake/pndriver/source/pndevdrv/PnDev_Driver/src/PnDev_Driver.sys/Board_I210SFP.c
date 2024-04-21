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
/*  F i l e               &F: Board_I210SFP.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for I210SFP
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
#include "Board_I210SFP.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_Reset)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_ResetHw)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_InitHw1)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_InitHw2)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoard_I210SFP_GetMacAdress)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoard_I210SFP_Reset(  DEVICE_EXTENSION*			pDevExtIn,
                                const ePNDEV_RESET_ACTION	eActionIn,
                                const UINT32				lDimErrorIn,
                                _TCHAR*						sErrorOut)
{
    BOOLEAN bResult = FALSE;
    BOOLEAN bResultAction = FALSE;
    BOOLEAN	bStopDefaultFw = FALSE;
    BOOLEAN	bResetHw = FALSE;
    BOOLEAN	bInitHw = FALSE;

    // preset
    bResultAction = TRUE;

    switch (eActionIn)
        // action
    {
        case ePNDEV_RESET_ACTION_ALLOC_HW:
        {
            // Note:
            //	- EntryBoardState = PowerOnState
            //	- ExitBoardState  = ReadyForUse by driver
            //	- an existing DefaultFw must be stopped

            bInitHw = TRUE;

            break;
        }
        case ePNDEV_RESET_ACTION_RELEASE_HW:
        {
            // Note:
            //	- ExitBoardState = PowerOnState
            //	- an existing DefaultFw must be started

            bStopDefaultFw = FALSE;
            bResetHw = TRUE;

            break;
        }
        case ePNDEV_RESET_ACTION_REINIT_HW:
        {
            // Note:
            //	- ExitBoardState = ReadyForUse by driver
            //	- an existing DefaultFw must be stopped

            bStopDefaultFw = TRUE;
            bResetHw = TRUE;
            bInitHw = TRUE;

            break;
        }
        case ePNDEV_RESET_ACTION_USE_HW:
        {
            // Note:
            //	- HwSettings must not be changed
            //	- HwInit is done by another DriverInstance

            break;
        }
        default:
        {
            // error
            bResultAction = FALSE;

            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Invalid ResetAction!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);

            break;
        }
    }

    if (bResultAction)
        // success
    {
        if ((!bStopDefaultFw)
            && (!bResetHw)
            && (!bInitHw))
            // nothing to do
        {
            // success
            bResult = TRUE;
        }
        else
        {
            // disable interrupts at board globally
            fnBoard_ChangeIntMaskGlobal(pDevExtIn,
                                        FALSE);

            if (fnBoard_I210SFP_InitHw1(pDevExtIn,
                                        bStopDefaultFw,
                                        lDimErrorIn,
                                        sErrorOut))
                // initializing hardware (phase 1) ok
            {
                BOOLEAN bResultHwReset = FALSE;

                if (!bResetHw)
                    // don't reset Hw
                {
                    // success
                    bResultHwReset = TRUE;
                }
                else
                {
                    // execute HwReset (bResetRunning=TRUE)
                    bResultHwReset = fnBoard_I210SFP_ResetHw(   pDevExtIn,
                                                                lDimErrorIn,
                                                                sErrorOut);
                }

                if (bResultHwReset)
                    // success
                {
                    if (!bInitHw)
                        // don't initialize Hw
                    {
                        // success
                        bResult = TRUE;
                    }
                    else
                    {
                        if (fnBoard_I210SFP_InitHw2(pDevExtIn,
                                                    bResetHw,
                                                    lDimErrorIn,
                                                    sErrorOut))
                            // initializing hardware (phase 2) ok
                        {
                            if (fnBoard_I210SFP_VerifyHwAfterReset( pDevExtIn,
                                                                    lDimErrorIn,
                                                                    sErrorOut))
                                // verifying hardware after HwReset ok
                            {
                                char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = { 0 };

                                // set TraceDetail
                                RtlStringCchPrintfA(sTraceDetail,
                                                    _countof(sTraceDetail),
                                                    "PnDev[%s]: RESET done",
                                                    pDevExtIn->sPciLocShortAscii);

                                // set trace
                                fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                                            ePNDEV_TRACE_CONTEXT_BASIC,
                                            TRUE,						// logging
                                            sTraceDetail);

                                // success
                                bResult = TRUE;
                            }
                        }

                        // enable interrupts at board globally
                        //	- do it only if Hw is initialized, otherwise don't change ResetState
                        fnBoard_ChangeIntMaskGlobal(pDevExtIn,
                                                    TRUE);
                    }

                    // reset finished
                    pDevExtIn->bResetRunning = FALSE;
                }
            }
        }
    }

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 1)
//************************************************************************

BOOLEAN fnBoard_I210SFP_InitHw1(DEVICE_EXTENSION*	pDevExtIn,
                                const BOOLEAN		bStopDefaultFwIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN bResult = FALSE;

    // Nothing special to do here
    bResult = TRUE;

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  execute HwReset
//************************************************************************

BOOLEAN fnBoard_I210SFP_ResetHw(DEVICE_EXTENSION*	pDevExtIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN	bResult = FALSE;

    // reset running
    pDevExtIn->bResetRunning = TRUE;

    // reset I210SFP
    {
        UINT32 lValueTmp = 0;

        lValueTmp = (1 << BOARD_I210SFP__CTRL_RST) | (1 << BOARD_I210SFP__CTRL_LRST) | (1 << BOARD_I210SFP__CTRL_DEV_RST);

        // do SoftwareReset, DeviceReset and LinkReset
        WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__CTRL),
                        (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__CTRL) & BOARD_I210SFP__CTRL_RESERVED_MASK) | lValueTmp);
    }

    // wait 100msec
    if (fnDelayThread(  pDevExtIn,
                        100,
                        lDimErrorIn,
                        sErrorOut))
    // starting WaitTime of current thread ok
    {
        UINT32 	lResStatus = 0;
        BOOLEAN bResetError = TRUE;

        // read ResetStatus
        lResStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__CTRL);
        lResStatus = (lResStatus >> BOARD_I210SFP__CTRL_RST) & 1;

        if (lResStatus == 0x0)
        // success
        {
            // read ResetStatus
            lResStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__STATUS);
            lResStatus = (lResStatus >> BOARD_I210SFP__STATUS_PF_RST_DONE) & 1;

            if (lResStatus == 0x1)
            // success
            {
                // read ResetStatus
                lResStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EECD);
                lResStatus = (lResStatus >> BOARD_I210SFP__EDCD_AUTO_RD) & 1;

                if (lResStatus == 0x1)
                // success
                {
                    // no Error at SoftwareReset
                    bResetError = FALSE;
                }
            }
        }

        if (!bResetError)
        // success
        {
            // we need to read the ICR after reset to clear the register of the pending reset interrupts
            (void)READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICR);

            bResult = TRUE;
        }
    }
 
    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 2)
//************************************************************************

BOOLEAN fnBoard_I210SFP_InitHw2(DEVICE_EXTENSION*	pDevExtIn,
                                const BOOLEAN		bResetHwIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN bResult = FALSE;

    if (pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_MSIX)
        // MSIX interrupts
    {
        // set GPIE register for MSIX
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__GPIE,
            (PNDEV_UINT32_SET_BIT_0 | PNDEV_UINT32_SET_BIT_4 | PNDEV_UINT32_SET_BIT_30 | PNDEV_UINT32_SET_BIT_31));

        // set IVAR_MISC
        // Bit10:8 = 0x0; INT_ALLOC[9]=OtherInterrupt -> Vector 0 
        // Bit15 = 0x1, valid INT_ALLOC[9]
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__IVAR_MISC,
                        PNDEV_UINT32_SET_BIT_15);

        // set EIMS, bit for masking OtherInterrupt in EICR Register
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EIMS,
                        PNDEV_UINT32_SET_BIT_0);

        // set EIAC, bit for auto clearing EICR (OtherInterrupt)
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EIAC,
                        PNDEV_UINT32_SET_BIT_0);
    }

    // set status DriverLoaded
    WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__CTRL_EXT,
                    ((READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__CTRL_EXT) & BOARD_I210SFP__CTRL_EXT_RESERVED_MASK) | BOARD_I210SFP__CTRL_EXT_DRV_LOAD));


    // success
    bResult = TRUE;
 
    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoard_I210SFP_VerifyHwAfterReset( DEVICE_EXTENSION*	pDevExtIn,
                                            const UINT32		lDimErrorIn,
                                            _TCHAR*				sErrorOut)
{
    BOOLEAN bResult = FALSE;

    // Nothing special to do here
    bResult = TRUE;

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get MacAddress
//************************************************************************

BOOLEAN fnBoard_I210SFP_GetMacAdress(   DEVICE_EXTENSION*	pDevExtIn,
                                        const UINT32		lDimErrorIn,
                                        _TCHAR*				sErrorOut)
{
    BOOLEAN 		bResult = FALSE;
    volatile UINT8* pBase = NULL;
    UINT32			lMacAddrHigh = BOARD_I210SFP__RAH;
    UINT32			lMacAddrLow = BOARD_I210SFP__RAL;

    // set Number of MacAdr to one, because there will no PortMacAddresse computed
    pBase = pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr;
    pDevExtIn->uRemanentData.lCtrMacAdr = 1;

    if ((pDevExtIn->uRemanentData.lCtrMacAdr != 0)
        && (pBase != NULL))
        // valid ptr and CtrMac
    {
        UINT32 	lTmpValue0 = 0;
        UINT32 	lTmpValue1 = 0;

        // Read mac address low
        lTmpValue0 = READ_REG_UINT32(pBase + lMacAddrLow);

        // Read mac address high
        lTmpValue1 = READ_REG_UINT32(pBase + lMacAddrHigh);

        pDevExtIn->uRemanentData.lArrayMacAdr[0][0] = (lTmpValue0 >> 0) & 0xFF;
        pDevExtIn->uRemanentData.lArrayMacAdr[0][1] = (lTmpValue0 >> 8) & 0xFF;
        pDevExtIn->uRemanentData.lArrayMacAdr[0][2] = (lTmpValue0 >> 16) & 0xFF;
        pDevExtIn->uRemanentData.lArrayMacAdr[0][3] = (lTmpValue0 >> 24) & 0xFF;

        pDevExtIn->uRemanentData.lArrayMacAdr[0][4] = (lTmpValue1) & 0xFF;
        pDevExtIn->uRemanentData.lArrayMacAdr[0][5] = (lTmpValue1 >> 8) & 0xFF;

        // success
        bResult = TRUE;
    }
    else
    {
        // set ErrorString
        fnBuildString(  _TEXT(__FUNCTION__),
                        _TEXT("(): Invalid lCtrMacAdr or BasePointer!"),
                        NULL,
                        lDimErrorIn,
                        sErrorOut);
    }

    return(bResult);
}

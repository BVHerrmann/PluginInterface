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
/*  F i l e               &F: Board_IX1000.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with IX1000
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
#include "Board_IX1000.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_Reset)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_ResetHw)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_InitHw1)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_InitHw2)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoard_IX1000_GetMacAdress)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoard_IX1000_Reset(   DEVICE_EXTENSION*			pDevExtIn,
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

            if (fnBoard_IX1000_InitHw1( pDevExtIn,
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
                    bResultHwReset = fnBoard_IX1000_ResetHw(pDevExtIn,
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
                        if (fnBoard_IX1000_InitHw2( pDevExtIn,
                                                    bResetHw,
                                                    lDimErrorIn,
                                                    sErrorOut))
                            // initializing hardware (phase 2) ok
                        {
                            if (fnBoard_IX1000_VerifyHwAfterReset(  pDevExtIn,
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

BOOLEAN fnBoard_IX1000_InitHw1( DEVICE_EXTENSION*	pDevExtIn,
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

BOOLEAN fnBoard_IX1000_ResetHw( DEVICE_EXTENSION*	pDevExtIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN	bResult = FALSE;
    UINT32 lMacHigh, lMacLow;

    // reset running
    pDevExtIn->bResetRunning = TRUE;
  
    // Attention: SW-Reset deletes MAC Address, save Registers before Reset!
    lMacHigh = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__MAC0_HIGH);
    lMacLow = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__MAC0_LOW);

    // reset Intel Quark X1000
    {
        UINT8 lValueTmp = 0;

        lValueTmp = (1 << BOARD_IX1000__BUS_MODE_SWR);

        // do SoftwareReset TODO Galileo check if SWR leads to other problems, than losing MAC Address!
        WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__BUS_MODE),
                        (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__BUS_MODE) & BOARD_IX1000__BUS_MODE_RESERVED_MASK) | lValueTmp);
    }

    // wait 100msec
    if (fnDelayThread(  pDevExtIn,
                        100,
                        lDimErrorIn,
                        sErrorOut))
        // starting WaitTime of current thread ok
    {
        UINT32 	lResStatus = 0;

        // read ResetStatus
        lResStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__BUS_MODE);
        lResStatus = (lResStatus >> BOARD_IX1000__BUS_MODE_SWR) & 1;

        if (lResStatus == 0x0)
            // success
        {
            // Attention: SW-Reset deletes MAC Address, restore Registers after Reset!
            WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__MAC0_HIGH, lMacHigh);
            WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__MAC0_LOW, lMacLow);

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

BOOLEAN fnBoard_IX1000_InitHw2( DEVICE_EXTENSION*	pDevExtIn,
                                const BOOLEAN		bResetHwIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN bResult = FALSE;

    // Nothing to do, no MSIX Support!
    bResult = TRUE;

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoard_IX1000_VerifyHwAfterReset(  DEVICE_EXTENSION*	pDevExtIn,
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

BOOLEAN fnBoard_IX1000_GetMacAdress(DEVICE_EXTENSION*	pDevExtIn,
                                    const UINT32		lDimErrorIn,
                                    _TCHAR*				sErrorOut)
{
    BOOLEAN 		bResult = FALSE;
    volatile UINT8* pBase = NULL;
    UINT32			lMacAddrHigh = BOARD_IX1000__MAC0_HIGH;
    UINT32			lMacAddrLow = BOARD_IX1000__MAC0_LOW;

    // set Number of MacAdr to one, because there will no PortMacAddresse computed
    pBase = pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr;
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

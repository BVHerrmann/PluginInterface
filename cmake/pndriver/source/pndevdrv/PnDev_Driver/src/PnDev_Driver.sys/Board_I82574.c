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
/*  F i l e               &F: Board_I82574.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with I82574
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
#include "Board_I82574.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_I82574_Reset)
//	#pragma alloc_text (PAGE, fnBoard_I82574_ResetHw)
//	#pragma alloc_text (PAGE, fnBoard_I82574_InitHw1)
//	#pragma alloc_text (PAGE, fnBoard_I82574_InitHw2)
//	#pragma alloc_text (PAGE, fnBoard_I82574_VerifyHwAfterReset)
//	#pragma alloc_text (PAGE, fnBoard_I82574_GetMacAdress)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoard_I82574_Reset(   DEVICE_EXTENSION*			pDevExtIn,
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

            if (fnBoard_I82574_InitHw1( pDevExtIn,
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
                    bResultHwReset = fnBoard_I82574_ResetHw(pDevExtIn,
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
                        if (fnBoard_I82574_InitHw2( pDevExtIn,
                                                    bResetHw,
                                                    lDimErrorIn,
                                                    sErrorOut))
                            // initializing hardware (phase 2) ok
                        {
                            if (fnBoard_I82574_VerifyHwAfterReset(  pDevExtIn,
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

BOOLEAN fnBoard_I82574_InitHw1( DEVICE_EXTENSION*	pDevExtIn,
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

BOOLEAN fnBoard_I82574_ResetHw( DEVICE_EXTENSION*	pDevExtIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN	bResult = FALSE;

    // reset running
    pDevExtIn->bResetRunning = TRUE;

    // reset I82574

    // Step1: set GIO Master Disable
    {
        UINT32 lValueTmp0 = 0;

        lValueTmp0 = (1 << BOARD_I82574__CTRL_GIO_MASTER_DISABLE) | BOARD_I82574__CTRL_RESERVED_ONES_MASK;

        WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL),
                        (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL) & BOARD_I82574__CTRL_RESERVED_ZEROS_MASK) | lValueTmp0);
    }

    // Step2: wait 100ms for Status GIO Master Enable == 0
    {
        UINT32 	lValueTmp1 = 1;
        BOOLEAN bResetError = TRUE;

        if (fnDelayThread(  pDevExtIn,
                            100,
                            lDimErrorIn,
                            sErrorOut))
            // starting WaitTime of current thread ok
        {
            // read GIO Status
            lValueTmp1 = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__STATUS);
            lValueTmp1 = (lValueTmp1 >> BOARD_I82574__STATUS_GIO_MASTER_ENABLE) & 1;

            if (lValueTmp1 == 0x0)
                // success
            {
                // Step3: initiate reset
                {
                    UINT32 lValueTmp2 = 0;

                    lValueTmp2 = (1u << BOARD_I82574__CTRL_RST) | (1u << BOARD_I82574__CTRL_PHY_RST) | BOARD_I82574__CTRL_RESERVED_ONES_MASK;

                    // do SoftwareReset
                    WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL),
                                    (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL) & BOARD_I82574__CTRL_RESERVED_ZEROS_MASK) | lValueTmp2);
                }

                // Step4: wait 100msec for self clearing CTRL_RST and check for it as well as for GIO_Master_Disable to ensure board access and NVM read done?
                if (fnDelayThread(  pDevExtIn,
                                    100,
                                    lDimErrorIn,
                                    sErrorOut))
                    // starting WaitTime of current thread ok
                {
                    UINT32 	lResStatus = 0;
                    UINT32 	lResStatusAutoRD = 0;
                    UINT32 	lGIOStatus = 1;

                    // read ResetStatus
                    lResStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL);
                    lResStatus = (lResStatus >> BOARD_I82574__CTRL_RST) & 1;

                    // read GIOStatus
                    lGIOStatus = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL);
                    lGIOStatus = (lGIOStatus >> BOARD_I82574__CTRL_GIO_MASTER_DISABLE) & 1;

                    // read ResetStatus2 - NVM Auto Read done?
                    lResStatusAutoRD = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__EEC);
                    lResStatusAutoRD = (lResStatusAutoRD >> BOARD_I82574__EEC_AUTO_RD) & 1;

                    if (lResStatus == 0x0 &&
                        lGIOStatus == 0x0 &&
                        lResStatusAutoRD == 0x1)
                        // success
                    {
                        // no Error at SoftwareReset
                        bResetError = FALSE;
                    }

                    if (!bResetError)
                        // success
                    {
                        // we need to read the ICR after reset to clear the register of the pending reset interrupts
                        (void)READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICR);

                        bResult = TRUE;
                    }
                }
            }
        }
    }
  
    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 2)
//************************************************************************

BOOLEAN fnBoard_I82574_InitHw2( DEVICE_EXTENSION*	pDevExtIn,
                                const BOOLEAN		bResetHwIn,
                                const UINT32		lDimErrorIn,
                                _TCHAR*				sErrorOut)
{
    BOOLEAN bResult = FALSE;

    if (pDevExtIn->eIntMode == ePNDEV_INTERRUPT_MODE_MSIX)
        // MSIX interrupts
    {
        // set IVAR
        // Bit18:16 = 0x0; INT_ALLOC[4]=OtherInterrupt -> Vector 0 
        // Bit19 = 0x1, valid INT_ALLOC[4]
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__IVAR,
                            PNDEV_UINT32_SET_BIT_19);

        // set CTRL_EXT.PBA_support for MSIX
        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT,
                        ((READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT) & BOARD_I82574__CTRL_EXT_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_31));

    }

    // set status DriverLoaded
    WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT,
                    ((READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_I82574__CTRL_EXT) & BOARD_I82574__CTRL_EXT_RESERVED_MASK) | BOARD_I82574__CTRL_EXT_DRV_LOAD));

    bResult = TRUE;

    return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoard_I82574_VerifyHwAfterReset(  DEVICE_EXTENSION*	pDevExtIn,
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

BOOLEAN fnBoard_I82574_GetMacAdress(DEVICE_EXTENSION*	pDevExtIn,
                                    const UINT32		lDimErrorIn,
                                    _TCHAR*				sErrorOut)
{
    BOOLEAN 		bResult = FALSE;
    volatile UINT8* pBase = NULL;
    UINT32			lMacAddrHigh = BOARD_I82574__RAH;
    UINT32			lMacAddrLow = BOARD_I82574__RAL;

    // set Number of MacAdr to one, because there will no PortMacAddresse computed
    pBase = pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr;
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

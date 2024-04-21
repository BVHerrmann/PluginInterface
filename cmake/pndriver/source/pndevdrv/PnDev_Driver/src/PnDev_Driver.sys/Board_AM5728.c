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
/*  F i l e               &F: Board_AM5728.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for boards with AM5728
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Board_AM5728.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoardAM5728_Reset)
//	#pragma alloc_text (PAGE, fnBoardAM5728_ResetHw)
//	#pragma alloc_text (PAGE, fnBoardAM5728_InitHw1)
//	#pragma alloc_text (PAGE, fnBoardAM5728_InitHw2)
//	#pragma alloc_text (PAGE, fnBoardAM5728_VerifyHwAfterReset)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  reset board
//************************************************************************

BOOLEAN fnBoardAM5728_Reset(DEVICE_EXTENSION*			pDevExtIn,
							const ePNDEV_RESET_ACTION	eActionIn,
							const UINT32				lDimErrorIn,
							_TCHAR*						sErrorOut)
{
BOOLEAN bResult			= FALSE;
BOOLEAN bResultAction	= FALSE;
BOOLEAN	bStopDefaultFw	= FALSE;
BOOLEAN	bResetHw		= FALSE;
BOOLEAN	bInitHw			= FALSE;

	// preset
	bResultAction = TRUE;

	switch	(eActionIn)
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

			bStopDefaultFw	= FALSE;
			bResetHw		= TRUE;

			break;
		}
		case ePNDEV_RESET_ACTION_REINIT_HW:
		{
			// Note:
			//	- ExitBoardState = ReadyForUse by driver
			//	- an existing DefaultFw must be stopped

			bStopDefaultFw	= TRUE;
			bResetHw		= TRUE;
			bInitHw			= TRUE;

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
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid ResetAction!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResultAction)
		// success
	{
		if	(	(!bStopDefaultFw)
			&&	(!bResetHw)
			&&	(!bInitHw))
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

			if	(fnBoardAM5728_InitHw1(	pDevExtIn,
										bStopDefaultFw,
										lDimErrorIn,
										sErrorOut))
				// initializing hardware (phase 1) ok
			{
			BOOLEAN bResultHwReset = FALSE;

				if	(!bResetHw)
					// don't reset Hw
				{
					// success
					bResultHwReset = TRUE;
				}
				else
				{
					// execute HwReset (bResetRunning=TRUE)
					bResultHwReset = fnBoardAM5728_ResetHw(	pDevExtIn,
															lDimErrorIn,
															sErrorOut);
				}

				if	(bResultHwReset)
					// success
				{
					if	(!bInitHw)
						// don't initialize Hw
					{
						// success
						bResult = TRUE;
					}
					else
					{
						if	(fnBoardAM5728_InitHw2(	pDevExtIn,
													bResetHw,
													lDimErrorIn,
													sErrorOut))
							// initializing hardware (phase 2) ok
						{
							if	(fnBoardAM5728_VerifyHwAfterReset(	pDevExtIn,
																	lDimErrorIn,
																	sErrorOut))
								// verifying hardware after HwReset ok
							{
							BOOLEAN			bPagedAccess = FALSE;
							volatile UINT8*	pAsicSdramSeg0 = NULL;

								// success
								bResult = TRUE;

								//------------------------------------------------------------------------
								// start using SdramPage 0
								fnBoard_StartSdramPage(pDevExtIn,
														0,
														&bPagedAccess,
														&pAsicSdramSeg0);
								{
									// preset SharedAsicSdram_Config
									RtlZeroMemory((void*)(pAsicSdramSeg0 + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG),
										PNCORE_SHARED_ASIC_SDRAM__SIZE_CONFIG);
								}
								//------------------------------------------------------------------------
								// stop using SdramPage
								fnBoard_StopSdramPage(pDevExtIn);
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

BOOLEAN fnBoardAM5728_InitHw1(	DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bStopDefaultFwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
			// success
			bResult = TRUE;

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  execute HwReset
//************************************************************************

BOOLEAN fnBoardAM5728_ResetHw(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
UINT32 	lRegVal = 0;
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	// reset running
	pDevExtIn->bResetRunning = TRUE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
        UINT32	lArrayConfigSpace[PNDEV_PCI_CONFIG_SPACE__SIZE / sizeof(UINT32)];    
        UINT32	lArrayConfigSpaceExpressCapability[PNDEV_PCI_CONFIG_SPACE__EXPRESS_CAPABILITY_SIZE / sizeof(UINT32)];
        UINT32  lOffsetExpressCapability = 0;

            if	(fnPrepareConfigSpaceHwReset(	pDevExtIn,
												lDimErrorIn,
												lArrayConfigSpace,
                                                lArrayConfigSpaceExpressCapability,
                                                &lOffsetExpressCapability,
												sErrorOut))
				// preparing ConfigSpace for HwReset ok
			{
                if	(fnCheckTransactionPending(	pDevExtIn,
												lDimErrorIn,
												sErrorOut))
					// no TransactionPending
				{		
			        // clear ResetStatus
			        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTST), 0x1 );
			
					// warm reset
					{
				    // initiate warm reset -> works but read is never acknowledged because reset starts immediately!
						WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTCTRL),
											(READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTCTRL)) | 0x1);
	
            
					/* WD Timer would be better, but WSPR disable/enable sequence does not always work (even after 1ms wait between 2 writes) 
                    // disable WD Timer part 1 
                    WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR), 0xAAAA );
                    
                    // wait 1msec before writing WSPR again
                    if  (fnDelayThread( pDevExtIn,
                                        1,
                                        lDimErrorIn,
                                        sErrorOut))
                        // starting WaitTime of current thread ok
                    {
                        // disable WD Timer part 2
                        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR), 0x5555 );
                        
                        // wait 1msec before reading WSPR after second write
                        if  (fnDelayThread( pDevExtIn,
                                            1,
                                            lDimErrorIn,
                                            sErrorOut))
                            // starting WaitTime of current thread ok
                        {
                            // read WSPR
                            lRegVal  = READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR);
                        }
                    }
			        
			        if(lRegVal != 0x5555)
			            // Error: WD Timer is not disabled
                    {
                        // set TraceDetail
                        RtlStringCchPrintfA(sTraceDetail,
                                            _countof(sTraceDetail),
                                            "PnDev[%s]: WD Timer could not be disabled (WSPR: 0x%x)",
                                            pDevExtIn->sPciLocShortAscii,
                                            lRegVal);

                        // set trace
                        fnSetTrace( ePNDEV_TRACE_LEVEL_INFO,
                                    ePNDEV_TRACE_CONTEXT_BASIC,
                                    TRUE,                       // logging
                                    sTraceDetail);
                    }
			        else
			            // WD Timer is disabled
			        {
                        // disable Prescaler
                        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WCLR), 0x0 );
    
                        // set delay to 0
                        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WDLY), 0x0 );
                                           
                        // set counter value (countdown) to 32 ms
                        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WCRR), 0xFFFFFD00 );
                        
                        // enable WD Timer part 1
                        WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR), 0xBBBB );
                        
                        // wait 1msec before writing WSPR again
                        if  (fnDelayThread( pDevExtIn,
                                            1,
                                            lDimErrorIn,
                                            sErrorOut))
                            // starting WaitTime of current thread ok
                        {
                            // enable WD Timer part 2
                            WRITE_REG_UINT32( (pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_WSPR), 0x4444 );
                        }
						*/
                        
                        // wait 400msec (+ 32 ms countdown of WD Timer)
				    if	(fnDelayThread(	pDevExtIn,
                                            400 /* + 32 */,
									    lDimErrorIn,
									    sErrorOut))
					    // starting WaitTime of current thread ok
				    {
                        // restore Configspace
					    if	(fnRestoreConfigSpaceHwReset(	pDevExtIn,
														    lArrayConfigSpace,
                                                            lArrayConfigSpaceExpressCapability,
                                                            lOffsetExpressCapability,
														    lDimErrorIn,
														    sErrorOut))
						    // restoring ConfigSpace after HwReset ok
					    {
					        // read ResetStatus
                                lRegVal 	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTST);
    
                                // set TraceDetail
                                RtlStringCchPrintfA(sTraceDetail,
                                                    _countof(sTraceDetail),
                                                    "PnDev[%s]: PRM_RSTST (after reset): 0x%x",
                                                    pDevExtIn->sPciLocShortAscii,
                                                    lRegVal);
    
                                // set trace
                                fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
                                            ePNDEV_TRACE_CONTEXT_BASIC,
                                            TRUE,						// logging
                                            sTraceDetail);
					        
                            // ATTENTION: This driver is for TI Sitara Revision 2.0 + Hardware workaround for Warm-Reset
                            // This workaround is triggering a cold reset of the hardware
                            // --> not the Warm-Reset bit is set, but the COLD !

                            // This has to be activated if no workaorund is needed anymore
                            // lResStatus 	= (lResStatus >> 1) & 1;
	
                                if	(lRegVal	== 0x1)
						        // warm reset occured
					        {
						        // clear ResetStatus
					            WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBar[BOARD_TI_AM5728__BAR0_IDX_SECOND_PART].pVirtualAdr + BOARD_TI_AM5728__BAR0_SECOND_PART_OFFSET_PRM_RSTST, 0x1);
						
						        bResult = TRUE;
					        }
				        }
			        }
                }
				}
            }

			break;
			
		}
		default:
		{
		    // set ErrorString
		    fnBuildString(	_TEXT(__FUNCTION__),
						    _TEXT("(): Invalid board!"),
						    NULL,
						    lDimErrorIn,
						    sErrorOut);

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize hardware (phase 2)
//************************************************************************

BOOLEAN fnBoardAM5728_InitHw2(	DEVICE_EXTENSION*	pDevExtIn,
								const BOOLEAN		bResetHwIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
			// success
			bResult = TRUE;

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify hardware after HwReset
//************************************************************************

BOOLEAN fnBoardAM5728_VerifyHwAfterReset(	DEVICE_EXTENSION*	pDevExtIn,
											const UINT32		lDimErrorIn,
											_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
			// success
			bResult = TRUE;

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	return(bResult);
}



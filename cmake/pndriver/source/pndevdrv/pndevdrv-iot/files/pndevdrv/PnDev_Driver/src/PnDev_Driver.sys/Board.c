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
/*  F i l e               &F: Board.c                                   :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Common board specific code
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Board.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_InitVar)
//	#pragma alloc_text (PAGE, fnBoard_GetHwInfo)
//	#pragma alloc_text (PAGE, fnBoard_GetRemanentData)
//	#pragma alloc_text (PAGE, fnBoard_ConvertMacAdr)
//	#pragma alloc_text (PAGE, fnBoard_StartFlashPage)
//	#pragma alloc_text (PAGE, fnBoard_StopFlashPage)
//	#pragma alloc_text (PAGE, fnBoard_StartSdramPage)
//	#pragma alloc_text (PAGE, fnBoard_StopSdramPage)
//	#pragma alloc_text (PAGE, fnBoard_CopyDataDirect)
//	#pragma alloc_text (PAGE, fnBoard_CopyDataPaged_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_CopyBlockPaged_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_CheckVersionBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckSignOfLifeBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoElfCmdLoadBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoElfCmdExecuteBootFw)
//	#pragma alloc_text (PAGE, fnBoard_DoBinCmdExecuteBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForElfCmdDoneBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForBinCmdDoneBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckElfStatusBootFw)
//	#pragma alloc_text (PAGE, fnBoard_CheckBinStatusBootFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForStartupParUserFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForReadyUserFw)
//	#pragma alloc_text (PAGE, fnBoard_WaitForAckDebugger)
//	#pragma alloc_text (PAGE, fnBoard_InitLed)
//	#pragma alloc_text (PAGE, fnBoard_SetLed)
//	#pragma alloc_text (PAGE, fnBoard_ConfigPciMaster)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  initialize variables
//************************************************************************

BOOLEAN fnBoard_InitVar(DEVICE_EXTENSION* pDevExtIn)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{




		case ePNDEV_BOARD_IX1000:	// Intel Quark X1000 on Galileo board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= FALSE;
			pDevExtIn->bCopyDataSupport = FALSE;
			
			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_MSI;
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoard_IX1000_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_IX1000;

			break;
		}

		default:
		{
		char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid board!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_BASIC,
						TRUE,						// logging
						sTraceDetail);

			break;
		}
	}

	if	(bResult)
		// success
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{

            


			case ePNDEV_BOARD_IX1000: // Intel Quark X1000 on Galileo board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_IX1000;
				
				break;
			}

			default:
			{
				break;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  get HwInfo
//************************************************************************

BOOLEAN fnBoard_GetHwInfo(	DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lDimErrorIn,
							uPNDEV_HW_INFO*		pHwInfoOut,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;

	// preset
	bResult = TRUE;

	pHwInfoOut->eBoard			= pDevExtIn->eBoard;
	pHwInfoOut->eBoardDetail	= pDevExtIn->eBoardDetail;
	pHwInfoOut->eAsic			= pDevExtIn->eAsic;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{




		case ePNDEV_BOARD_IX1000: // Intel Quark X1000 on Galileo board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("IX1000 board, Version: 0x%04x"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;
			
			break;
		}

		default:
		{
			// error
			bResult = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid board!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResult)
		// success
	{
		// store info about AsicCore
		pDevExtIn->bAsicCoreExist = pHwInfoOut->bAsicCoreExist;

		if	(fnGetInfoConfigSpace(	pDevExtIn,
									lDimErrorIn,
									&pHwInfoOut->uConfigSpace,
									sErrorOut))
			// getting ConfigSpaceInfo ok
		{
			// set AsicDetail
			pHwInfoOut->eAsicDetail = pDevExtIn->eAsicDetail;

			// set AsicInfo
			{
				switch	(pDevExtIn->eAsic)
						// AsicType
				{




					case ePNDEV_ASIC_IX1000:
					{
						// no AsicInformation
						break;
					}

					default:
					{
						// error
						bResult = FALSE;

						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid Asic!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

						break;
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
//  get RemanentData
//************************************************************************

BOOLEAN fnBoard_GetRemanentData(DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN				bResult				= FALSE;


	// preset OutputParameter
	RtlZeroMemory(	&pDevExtIn->uRemanentData,
					sizeof(uPNCORE_REMANENT_DATA));

    if (0)
    {
        // Dummy for preprocessor
    }




    else if (pDevExtIn->eBoard == ePNDEV_BOARD_IX1000)
        //	IX1000
    {
        if (fnBoard_IX1000_GetMacAdress(pDevExtIn,
                                        lDimErrorIn,
                                        sErrorOut))
            // success
        {
            bResult = TRUE;
        }
    }

    
	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  convert MacAdr from UINT64 to array
//************************************************************************

void fnBoard_ConvertMacAdr(	const UINT64	lMacAdr64In,
							volatile UINT8*	pArrayMacAdr8Out)
{
UINT8 i = 0;

	// example:
	//	- MacAdr = aa-bb-cc-dd-ee-ff = 0x0000aabbccddeeff
	//                                                        offset:  0  1  2  3  4  5
	//	- LittleEndian: 0x0000aabbccddeeff has following memory dump: ff ee dd cc bb aa 00 00
	//		-> MacAdr[0] = offset 5
	//		-> MacAdr[1] = offset 4
	//		-> MacAdr[2] = offset 3
	//		-> MacAdr[3] = offset 2
	//		-> MacAdr[4] = offset 1
	//		-> MacAdr[5] = offset 0

	for	(i = 0; i < 6; i++)
	{
		*(pArrayMacAdr8Out + i) = *(((volatile UINT8*) &lMacAdr64In) + (5-i));
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start FlashPage
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  start SdramPage
//************************************************************************

void fnBoard_StartSdramPage(DEVICE_EXTENSION*	pDevExtIn,
							const UINT32		lSegmentAhbIn,
							BOOLEAN*			pPagedAccessOut,
							volatile UINT8**	pPtrCurSegmentOut)
{
	// preset OutputParameter
	*pPagedAccessOut	= FALSE;
	*pPtrCurSegmentOut	= NULL;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{

		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  stop SdramPage
//************************************************************************

void fnBoard_StopSdramPage(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{

		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  copy data direct
//************************************************************************

void fnBoard_CopyDataDirect(uPNDEV_COPY_DATA_IN*	pIn,
							volatile UINT8*			pHostDataIn,
							volatile UINT8*			pAsicDestIn)
{
UINT32 i = 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockWrite[i].lSize != 0)
			// current WriteBlock exist
		{
			RtlCopyMemory(	(void*) (pAsicDestIn + pIn->uArrayBlockWrite[i].lOffsetAsic),
							(void*) (pHostDataIn + pIn->uArrayBlockWrite[i].lOffsetHostDataBuf),
							pIn->uArrayBlockWrite[i].lSize);
		}
	}

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockRead[i].lSize != 0)
			// current ReadBlock exist
		{
			RtlCopyMemory(	(void*) (pHostDataIn + pIn->uArrayBlockRead[i].lOffsetHostDataBuf),
							(void*) (pAsicDestIn + pIn->uArrayBlockRead[i].lOffsetAsic),
							pIn->uArrayBlockRead[i].lSize);
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	copy data paged (AsicSdram)
//************************************************************************

void fnBoard_CopyDataPaged_Sdram(	DEVICE_EXTENSION*		pDevExtIn,
									uPNDEV_COPY_DATA_IN*	pIn,
									volatile UINT8*			pHostDataIn)
{
UINT32 i = 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockWrite[i].lSize != 0)
			// current WriteBlock exist
		{
			// copy current block paged (AsicSdram)
			fnBoard_CopyBlockPaged_Sdram(	pDevExtIn,
											pHostDataIn + pIn->uArrayBlockWrite[i].lOffsetHostDataBuf,
											pIn->uArrayBlockWrite[i].lOffsetAsic,
											pIn->uArrayBlockWrite[i].lSize,
											ePNDEV_COPY_ACTION_WRITE);
		}
	}

	for	(i = 0; i < PNDEV_DIM_ARRAY_COPY_DATA; i++)
	{
		if	(pIn->uArrayBlockRead[i].lSize != 0)
			// current ReadBlock exist
		{
			// copy current block paged (AsicSdram)
			fnBoard_CopyBlockPaged_Sdram(	pDevExtIn,
											pHostDataIn + pIn->uArrayBlockRead[i].lOffsetHostDataBuf,
											pIn->uArrayBlockRead[i].lOffsetAsic,
											pIn->uArrayBlockRead[i].lSize,
											ePNDEV_COPY_ACTION_READ);
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	copy current block paged (AsicSdram)
//************************************************************************

void fnBoard_CopyBlockPaged_Sdram(	DEVICE_EXTENSION*			pDevExtIn,
									volatile UINT8*				pHostDataIn,
									const UINT32				lOffsetAsicSdramIn,
									const UINT32				lSizeIn,
									const ePNDEV_COPY_ACTION	eActionIn)
{
volatile UINT8*	pHostData_Rest			= NULL;
UINT32			lOffsetAsicSdram_Rest	= 0;
UINT32			lSize_Rest				= 0;

	pHostData_Rest			= pHostDataIn;
	lOffsetAsicSdram_Rest	= lOffsetAsicSdramIn;
	lSize_Rest				= lSizeIn;

	while	(lSize_Rest != 0)
			// not all bytes copied
	{
	UINT32			lSegmentAhb				= 0;
	UINT32			lOffset_CurSegment		= 0;
	UINT32			lSize_CurSegment		= 0;
	volatile UINT8*	pAsicSdram_CurSegment	= NULL;
	BOOLEAN			bPagedAccess			= FALSE;

		// compute current segment (0,1,2,..)
		lSegmentAhb = lOffsetAsicSdram_Rest / (1024*1024*2);

		// compute offset to SegmentStartAdr
		lOffset_CurSegment = lOffsetAsicSdram_Rest % (1024*1024*2);

		if	(lOffset_CurSegment == 0)
			// block starts at SegmentStartAdr
		{
			if	(lSize_Rest <= (1024*1024*2))
				// final block
			{
				lSize_CurSegment = lSize_Rest;
			}
			else
			{
				lSize_CurSegment = (1024*1024*2);
			}
		}
		else
		{
		UINT32 lSizeMax_CurSegment = 0;

			lSizeMax_CurSegment = ((1024*1024*2) - lOffset_CurSegment);

			if	(lSize_Rest <= lSizeMax_CurSegment)
				// final block
			{
				lSize_CurSegment = lSize_Rest;
			}
			else
			{
				lSize_CurSegment = lSizeMax_CurSegment;
			}
		}

		// start SdramPage
		fnBoard_StartSdramPage(	pDevExtIn,
								lSegmentAhb,
								&bPagedAccess,
								&pAsicSdram_CurSegment);
		{
		#ifdef PNDEV_GET_RUNTIME_COPY_BLOCK_PAGED
		uPNDEV_CLOCK uClockStart;

				// get clock
				fnGetClock(&uClockStart);
		#endif

			switch	(eActionIn)
					// action
			{
				case ePNDEV_COPY_ACTION_READ:
				{
					RtlCopyMemory(	(void*) pHostData_Rest,
									(void*) (pAsicSdram_CurSegment + lOffset_CurSegment),
									lSize_CurSegment);

					break;
				}
				case ePNDEV_COPY_ACTION_WRITE:
				{
					RtlCopyMemory(	(void*) (pAsicSdram_CurSegment + lOffset_CurSegment),
									(void*) pHostData_Rest,
									lSize_CurSegment);

					break;
				}
				default:
				{
					break;
				}
			}

			pHostData_Rest			+= lSize_CurSegment;
			lOffsetAsicSdram_Rest	+= lSize_CurSegment;
			lSize_Rest				-= lSize_CurSegment;

			#ifdef PNDEV_GET_RUNTIME_COPY_BLOCK_PAGED
			{
			UINT32	lRuntime_Usec = 0;
			char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

				// compute Runtime in usec
				lRuntime_Usec = fnComputeRunTimeUsec(&uClockStart);

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ++++ Copying page (max. 2MB): Runtime=%u usec (action=%s, size=%u)",
									pDevExtIn->sPciLocShortAscii,
									lRuntime_Usec,
									(eActionIn == ePNDEV_COPY_ACTION_READ) ? ("read") : ("write"),
									lSize_CurSegment);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
							ePNDEV_TRACE_CONTEXT_BASIC,
							FALSE,						// logging
							sTraceDetail);
			}
			#endif
		}
		// stop SdramPage
		fnBoard_StopSdramPage(pDevExtIn);
	}
}


//************************************************************************
//  D e s c r i p t i o n :
//
//  check version of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckVersionBootFw(	DEVICE_EXTENSION*	pDevExtIn,
									volatile UINT8*		pAsicSdramSeg0In,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult				= FALSE;
BOOLEAN	bResultAsic			= TRUE;
UINT32	lIdentRequired		= 0;
UINT32	lVersionRequired	= 0;

	switch	(pDevExtIn->eAsic)
			// AsicType
	{
		case ePNDEV_ASIC_ERTEC400:
		case ePNDEV_ASIC_ERTEC200:
		case ePNDEV_ASIC_ERTEC200P:
		{
			lIdentRequired		= 0xff550003;
			lVersionRequired	= 0x00020003;

			break;
		}
		case ePNDEV_ASIC_SOC1:
		{
			lIdentRequired		= 0xff550004;
			lVersionRequired	= 0x00010001;

			break;
		}
		case ePNDEV_ASIC_HERA:
		{
			lIdentRequired		= 0xff550005;
			lVersionRequired	= 0x00010001;

			break;
		}
		case ePNDEV_ASIC_AM5728:
		{
			lIdentRequired = 0xff550000;
			lVersionRequired = 0x00010002;

			break;
		}
		default:
		{
			// error
			bResultAsic = FALSE;

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid AsicType!"),
							NULL,
							lDimErrorIn,
							sErrorOut);

			break;
		}
	}

	if	(bResultAsic)
		// success
	{
	UINT32	lIdent		= 0;
	UINT32	lVersion	= 0;

		// get version of BootFw
		lIdent		= READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_IDENT);
		lVersion	= READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_VERSION);

		if	(	(lIdent		!= lIdentRequired)
			||	(lVersion	< lVersionRequired))
			// invalid BootFw
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Invalid BootFw (real: 0x%08x.0x%08x, required: 0x%08x.0x%08x)!"),
								lIdent,
								lVersion,
								lIdentRequired,
								lVersionRequired);

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  check SignOfLife of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckSignOfLifeBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		bTimeoutMsecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	lPatternPreset	= 0x11223344;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	// set AlivePattern
	WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
						lPatternPreset);

	for	(i = 0; i < bTimeoutMsecIn; i++)
		// MaxTime
	{
		if	(	(pConfigParIn != NULL)
			&&	(pConfigParIn->bCancel))
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			// wait 10msec

			if	(!fnDelayThread(pDevExtIn,
								10,
								lDimErrorIn,
								sErrorOut))
				// error at starting WaitTime of current thread
			{
				bErrorDelay	= TRUE;

				// leave loop
				break;
			}
			else
			{
			UINT32 lAlive = 0;

				lAlive = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE);

				if	(lAlive == (~lPatternPreset))
					// success
				{
					// success
					bErrorTimeout = FALSE;

					// leave loop
					break;
				}
				else if	(lAlive == lPatternPreset)
						// AlivePattern unchanged
				{
					// wait further on
				}
				else
				{
					// AlivePattern changed but wrong value
					//	- if startup of BootFw not finished before checking SignOfLife then AlivePattern may be overwritten by startup code

					// rewrite AlivePattern
					WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
										lPatternPreset);
				}
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no SignOfLife of BootFw!"),
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

	// reset AlivePattern for ensuring that AsicCore sees a changed value if function is called next time
	WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ALIVE),
						0);

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  do ElfCmd 'Load' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoElfCmdLoadBootFw(	DEVICE_EXTENSION*	pDevExtIn,
									volatile UINT8*		pAsicSdramSeg0In,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lOffsetFwBufIn,
									const UINT32		lIndexFwIn,
									const UINT32		lIndexFwMaxIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for commands

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
	UINT32 	lAdrAsicSdram_AsicCore = 0;
	BOOLEAN	bResultTmp = FALSE;

		// Note:
		//	- BootFw needs addresses which need not to be converted (=view of AsicCore)!
		//	- UserFw is copied to start of SharedAsicSdram_Direct
		//	- use AsicSdram address which always exist independent of a previous loaded firmware (which may change virtual addresses!)
	
		// preset
		bResultTmp = TRUE;

		switch	(pDevExtIn->eAsic)
				// AsicType
		{

			default:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Invalid AsicType!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
				
				// error
				bResultTmp = FALSE;

				break;
			}
		}

		if	(bResultTmp)
			// success
		{
		_TCHAR	sAction[PNDEV_SIZE_STRING_BUF_SHORT] = {0};
		UINT32	lAdrFwBuf_AsicCore = 0;

			lAdrFwBuf_AsicCore = lAdrAsicSdram_AsicCore + PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT + lOffsetFwBufIn;

			// set BaseAdr of firmware, needed by ELF loader
			WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_BASE),
								lAdrFwBuf_AsicCore);

			// set command 'Load'
			WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD),
								PNDEV_ASIC_SDRAM_ELF_CMD__LOAD);

			// set action
			_RtlStringCchPrintf(sAction,
								_countof(sAction),
								_TEXT(" (command Load, firmware %u of %u)"),
								lIndexFwIn,
								lIndexFwMaxIn);

			if	(fnBoard_WaitForElfCmdDoneBootFw(	pDevExtIn,
													pAsicSdramSeg0In,
													pConfigParIn,
													PNDEV_ASIC_SDRAM_ELF_CMD__LOAD,
													sAction,
													lTimeout_10msecIn,
													lDimErrorIn,
													sErrorOut))
				// waiting for ElfCmdDone of BootFw ok
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
//  do ElfCmd 'Execute' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoElfCmdExecuteBootFw(	DEVICE_EXTENSION*			pDevExtIn,
										volatile UINT8*				pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*			pConfigParIn,
										const BOOLEAN				bDebugModeIn,
										const UINT32				lIndexFwIn,
										const UINT32				lIndexFwMaxIn,
										const UINT32				lTimeout_10msecIn,
										const UINT32				lTimeStartupSignOfLife_10msecIn,
										uPNCORE_STARTUP_PAR_DRIVER*	pStartupParDriverIn,
										const UINT32				lDimErrorIn,
										_TCHAR*						sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for next command

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
	_TCHAR sAction[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

		// update DriverConfigPar
		//	- must be done before command 'Execute' because Fw needs info at startup (e.g. DebugMode)
		{
			pConfigParIn->bDebugMode = bDebugModeIn;

			if	(pStartupParDriverIn != NULL)
				// valid ptr
			{
				*((uPNCORE_STARTUP_PAR_DRIVER*) &pConfigParIn->uDriver.uStartupPar) = *pStartupParDriverIn;
			}

			// set TimeStartupSignOfLife
			pConfigParIn->lTimeStartupSignOfLife_10msec = lTimeStartupSignOfLife_10msecIn;
		}

		// set command 'Execute'
		WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD),
							PNDEV_ASIC_SDRAM_ELF_CMD__EXECUTE);

		// set action
		_RtlStringCchPrintf(sAction,
							_countof(sAction),
							_TEXT(" (command Execute, firmware %u of %u)"),
							lIndexFwIn,
							lIndexFwMaxIn);

		if	(fnBoard_WaitForElfCmdDoneBootFw(	pDevExtIn,
												pAsicSdramSeg0In,
												pConfigParIn,
												PNDEV_ASIC_SDRAM_ELF_CMD__EXECUTE,
												sAction,
												lTimeout_10msecIn,
												lDimErrorIn,
												sErrorOut))
			// waiting for ElfCmdDone of BootFw ok
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
//  do BinCmd 'Execute' of BootFw
//************************************************************************

BOOLEAN fnBoard_DoBinCmdExecuteBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	// BootFw must be ready for next command

	if	(fnBoard_CheckSignOfLifeBootFw(	pDevExtIn,
										pAsicSdramSeg0In,
										pConfigParIn,
										10,					// max. 10msec
										lDimErrorIn,
										sErrorOut))
		// checking SignOfLife of BootFw ok
	{
		// set command 'Execute'
		WRITE_REG_UINT32(	(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_CMD),
							PNDEV_ASIC_SDRAM_BIN_CMD__EXECUTE);



		if	(fnBoard_WaitForBinCmdDoneBootFw(	pDevExtIn,
												pAsicSdramSeg0In,
												pConfigParIn,
												PNDEV_ASIC_SDRAM_BIN_CMD__EXECUTE,
												lTimeout_10msecIn,
												lDimErrorIn,
												sErrorOut))
			// waiting for BinCmdDone of BootFw ok
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
//  wait for ElfCmdDone of BootFw
//************************************************************************

BOOLEAN fnBoard_WaitForElfCmdDoneBootFw(DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lCmdIn,
										_TCHAR*				sActionIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_CMD);

			if	(lStatus == lCmdIn)
				// command running
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime of current thread
				{
					bErrorDelay	= TRUE;

					// leave loop
					break;
				}
			}
			else
			{
				// command done anyway

				// success
				bErrorTimeout = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no reaction of AsicCore"),
							sActionIn,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_ELF_STATUS);

			if	(fnBoard_CheckElfStatusBootFw(	pDevExtIn,
												lStatus,
												sActionIn,
												lDimErrorIn,
												sErrorOut))
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
//  wait for ElfCmdDone of BootFw
//************************************************************************

BOOLEAN fnBoard_WaitForCp1625CmdDoneBootFw(	DEVICE_EXTENSION*	pDevExtIn,
											volatile UINT8*		pAsicSdramSeg0In,
											const UINT32		lCmdIn,
											_TCHAR*				sActionIn,
											const UINT32		lTimeout_10msecIn,
											const UINT32		lDimErrorIn,
											_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
	UINT32 lStatus = 0;

		// read status
		lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_CP1625_CMD);

		if	(lStatus == lCmdIn)
			// command running
		{
			// try again in 10msec

			if	(!fnDelayThread(pDevExtIn,
								10,
								lDimErrorIn,
								sErrorOut))
				// error at starting WaitTime of current thread
			{
				bErrorDelay	= TRUE;

				// leave loop
				break;
			}
		}
		else
		{
			// command done anyway

			// success
			bErrorTimeout = FALSE;

			// leave loop
			break;
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no reaction of AsicCore"),
							sActionIn,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			bResult = TRUE;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  wait for BinCmdDone of BootFw
//************************************************************************

BOOLEAN fnBoard_WaitForBinCmdDoneBootFw(DEVICE_EXTENSION*	pDevExtIn,
										volatile UINT8*		pAsicSdramSeg0In,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lCmdIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_CMD);

			if	(lStatus == lCmdIn)
				// command running
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime of current thread
				{
					bErrorDelay	= TRUE;

					// leave loop
					break;
				}
			}
			else
			{
				// command done anyway

				// success
				bErrorTimeout = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no reaction of AsicCore!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
		UINT32 lStatus = 0;

			// read status
			lStatus = READ_REG_UINT32(pAsicSdramSeg0In + PNDEV_ASIC_SDRAM__OFFSET_BIN_STATUS);

			if	(fnBoard_CheckBinStatusBootFw(	pDevExtIn,
												lStatus,
												lDimErrorIn,
												sErrorOut))
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
//  check ELF status of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckElfStatusBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lStatusIn,
										_TCHAR*				sActionIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(lStatusIn == PNDEV_ASIC_SDRAM_ELF_STATUS__SUCCESS)
		// success
	{
		bResult = TRUE;
	}
	else
	{
		switch	(lStatusIn)
				// status
		{
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_ELF:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Invalid format!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BIT_WIDTH:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Invalid BitWidth!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENDIANESS:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Wrong endianess!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_NO_EXECUTABLE:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: No executable!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_LOAD:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: Header correct but error at loading!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_BASE_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: BaseAddress for loading not in kseg0 or kseg1!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			case PNDEV_ASIC_SDRAM_ELF_STATUS__ERROR_ENTRY_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): ELF firmware: EntryAddress for starting not in kseg0 or kseg1!"),
								sActionIn,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			default:
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Invalid confirmation (=0x%x)!"),
									lStatusIn);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
								sActionIn,
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
//  check BIN status of BootFw
//************************************************************************

BOOLEAN fnBoard_CheckBinStatusBootFw(	DEVICE_EXTENSION*	pDevExtIn,
										const UINT32		lStatusIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	if	(lStatusIn == PNDEV_ASIC_SDRAM_BIN_STATUS__SUCCESS)
		// success
	{
		bResult = TRUE;
	}
	else
	{
		switch	(lStatusIn)
				// status
		{
			case PNDEV_ASIC_SDRAM_BIN_STATUS__ERROR_KSEG0_1:
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): BIN firmware not in kseg0 or kseg1!"),
								NULL,
								lDimErrorIn,
								sErrorOut);

				break;
			}
			default:
			{
			_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

				// set ErrorDetail
				_RtlStringCchPrintf(sErrorDetail,
									_countof(sErrorDetail),
									_TEXT("(): Invalid confirmation (=0x%x)!"),
									lStatusIn);

				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								sErrorDetail,
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
//  wait for StartupPar of UserFw
//************************************************************************

BOOLEAN fnBoard_WaitForStartupParUserFw(DEVICE_EXTENSION*	pDevExtIn,
										uPNCORE_CONFIG_PAR*	pConfigParIn,
										const UINT32		lTimeout_10msecIn,
										const UINT32		lDimErrorIn,
										_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->uFw.bStartupParValid)
				// StartupPar of UserFw not yet valid
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime of current thread
				{
					bErrorDelay	= TRUE;

					// leave loop
					break;
				}
			}
			else
			{
				// success
				bErrorTimeout = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout at waiting for valid StartupPar of UserFw!"),
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  wait for Ready of UserFw
//************************************************************************

BOOLEAN fnBoard_WaitForReadyUserFw(	DEVICE_EXTENSION*	pDevExtIn,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->uFw.bReady)
				// Fw not ready
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime of current thread
				{
					bErrorDelay	= TRUE;

					// leave loop
					break;
				}
			}
			else
			{
				// success
				bErrorTimeout = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout at waiting for StartupDone of UserFw!"),
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

//************************************************************************
//  D e s c r i p t i o n :
//
//  wait for ack of debugger
//************************************************************************

BOOLEAN fnBoard_WaitForAckDebugger(	DEVICE_EXTENSION*	pDevExtIn,
									uPNCORE_CONFIG_PAR*	pConfigParIn,
									const UINT32		lTimeout_10msecIn,
									const UINT32		lWaitTime_10msecIn,
									const UINT32		lDimErrorIn,
									_TCHAR*				sErrorOut)
{
BOOLEAN	bResult			= FALSE;
BOOLEAN	bErrorDelay		= FALSE;
BOOLEAN	bErrorTimeout	= FALSE;
UINT32	i = 0;

	// preset
	bErrorTimeout = TRUE;

	for	(i = 0; i < lTimeout_10msecIn; i++)
		// MaxTime
	{
		if	(pConfigParIn->bCancel)
			// cancel
		{
			// leave loop
			break;
		}
		else
		{
			if	(!pConfigParIn->bAckDebugger)
				// semaphore not set
			{
				// try again in 10msec

				if	(!fnDelayThread(pDevExtIn,
									10,
									lDimErrorIn,
									sErrorOut))
					// error at starting WaitTime of current thread
				{
					bErrorDelay	= TRUE;

					// leave loop
					break;
				}
			}
			else
			{
				// success
				bErrorTimeout = FALSE;

				// leave loop
				break;
			}
		}
	}

	if	(!bErrorDelay)
		// no TimerError
	{
		if	(bErrorTimeout)
			// timeout
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Timeout, no ack of debugger!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// wait for start of debugger (go) after setting semaphore
			//	- AsicCore must be ready for commands of BootFw

			if	(fnDelayThread(	pDevExtIn,
								lWaitTime_10msecIn,
								lDimErrorIn,
								sErrorOut))
				// starting WaitTime of current thread ok
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
//  initialize LEDs
//************************************************************************

void fnBoard_InitLed(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{

		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set state of green and red LED
//************************************************************************

void fnBoard_SetLed(DEVICE_EXTENSION*	pDevExtIn,
					BOOLEAN				bGreenOnIn,
					BOOLEAN				bRedOnIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{

		default:
		{
			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  configure OutboundRegion
//************************************************************************

BOOLEAN fnBoard_ConfigPciMaster(DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lRegionIdx,
								const UINT32		lPhyAddrIn,
								const UINT32		lSizeIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{

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
//  save register state before reset
//************************************************************************

BOOLEAN fnBoard_SaveRegisterHwReset(DEVICE_EXTENSION*	pDevExtIn,
									UINT32              lArrayRegister[],     // array=ptr
									const UINT32        lDimErrorIn,
									_TCHAR*             sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{

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
//  restore register state after reset
//************************************************************************

BOOLEAN fnBoard_RestoreRegisterHwReset(	DEVICE_EXTENSION*	pDevExtIn,
										UINT32           	lArrayRegister[],     // array=ptr
										const UINT32     	lDimErrorIn,
										_TCHAR*          	sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{

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



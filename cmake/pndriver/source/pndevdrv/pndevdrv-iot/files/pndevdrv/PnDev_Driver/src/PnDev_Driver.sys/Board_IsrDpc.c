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
/*  F i l e               &F: Board_IsrDpc.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Board specific code for handling ISR/DPC
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Board_IsrDpc.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_ChangeIntMaskGlobal)
//	#pragma alloc_text (PAGE, fnBoard_CheckInt)
//	#pragma alloc_text (PAGE, fnBoard_IsrSetEoi)
//	#pragma alloc_text (PAGE, fnBoard_IsrStandard)
//	#pragma alloc_text (PAGE, fnBoard_DpcStandard)
//	#pragma alloc_text (PAGE, fnBoard_IsrIntTest)
//	#pragma alloc_text (PAGE, fnBoard_DpcIntTest)
//	#pragma alloc_text (PAGE, fnBoard_StartIntTest)
//	#pragma alloc_text (PAGE, fnBoard_SetAsicDemoInt)
//	#pragma alloc_text (PAGE, fnBoard_MaskAsicDemoInt)
//	#pragma alloc_text (PAGE, fnBoard_UnmaskAsicDemoInt)
//	#pragma alloc_text (PAGE, fnBoard_CheckAsicDemoInt)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  enable/disable interrupts at board globally
//************************************************************************

void fnBoard_ChangeIntMaskGlobal(	DEVICE_EXTENSION*	pDevExtIn,
									const BOOLEAN		bEnableIn)
{
    if (0)
    {
        // just a dummy to make it possible to use preprocessor defines
    }


    else if (pDevExtIn->eIcu == ePNDEV_ICU_IX1000)
			// ICU IX1000
	{

		if	(pDevExtIn->pIntelIcu != NULL)
			// valid ptr
		{      
			// disable all Normal/Abnormal interrupts
			// bits 0-10, 13-16 zero
			WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INTEN),
								0x00000000);

			// Mask Timestamp interrupt
			WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INT_MASK),
								0xFFFFFFFF);
			
			// Mask MMC Receive interrupts
			WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INT_MASK_MMC_RX),
								0xFFFFFFFF);

			// Mask MMC Transmit interrupts
			WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INT_MASK_MMC_TX),
								0xFFFFFFFF);
                 
			// Mask MMC IPC Receive Checksum Offload interrupts
			WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INT_MASK_MMC_IPC_RX),
								0xFFFFFFFF);
		}
	}  

	else
	{
		if	(!bEnableIn)
			// disable interrupt
		{
			switch	(pDevExtIn->eIcu)
					// IcuType
			{
				default:
				{
					break;
				}
			}
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	check if any interrupt exist
//************************************************************************

BOOLEAN	fnBoard_CheckInt(	DEVICE_EXTENSION*	pDevExtIn,
							uPNDEV_INT_DETAIL*	pIntDetailOut)
{
BOOLEAN	bResult = FALSE;


	// preset OutputParameter
	RtlZeroMemory(	pIntDetailOut,
					sizeof(uPNDEV_INT_DETAIL));

	switch	(pDevExtIn->eIcu)
			// IcuType
	{


		case ePNDEV_ICU_IX1000:	// IX1000
		{
			pIntDetailOut->as.uIntel.lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_IX1000__STATUS);

			// IX1000 Status Register not only contains pending Interrupts, check only Interrupts in register with mask! 
			if	((pIntDetailOut->as.uIntel.lIcr & BOARD_IX1000__STATUS_INT_MASK) != 0)
				// any interrupt
			{
				// success
				bResult = TRUE;

				// any AsicInt occurred
				pIntDetailOut->uCommon.bAsicInt = TRUE;
			}

			break;
		}

		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set Eoi
//************************************************************************

void fnBoard_IsrSetEoi(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eIcu)
			// IcuType
	{
		
		
        case ePNDEV_ICU_IX1000:
		{
			// no setting of EOI because it does not exist
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
//	standard ISR
//************************************************************************

void fnBoard_IsrStandard(	DEVICE_EXTENSION*	pDevExtIn,
							uPNDEV_INT_DETAIL*	pIntDetailOut,
							BOOLEAN*			pReqDpcOut,
							UINT32				lMsgIdMsiIn)
{
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	// preset OutputParameter
	(*pReqDpcOut) = FALSE;

	#ifdef PNDEV_GET_RUNTIME_ISR_BY_GPIO

		if	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
			// FPGA1-Ertec200P board
		{
			// GPIO2=1
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_SET_0),
								PNDEV_UINT32_SET_BIT_2);
		}

	#endif

	if	(pIntDetailOut->uCommon.bAccessTimeoutInt)
		// AccessTimeoutInt
	{
		// set FatalAsync
		pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Unexpected AsicInt (AccessTimeoutInt)!",
							pDevExtIn->sPciLocShortAscii);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_ISR,
					TRUE,						// logging
					sTraceDetail);
	}

	if	(pIntDetailOut->uCommon.bGpioInt)
		// GpioInt
	{
		// set FatalAsync
		pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Unexpected AsicInt (GpioInt)!",
							pDevExtIn->sPciLocShortAscii);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_ISR,
					TRUE,						// logging
					sTraceDetail);
	}

	if	(pIntDetailOut->uCommon.bDmaInt)
		// DmaInt
	{
		// set FatalAsync
		pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Unexpected AsicInt (DmaInt)!",
							pDevExtIn->sPciLocShortAscii);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_ISR,
					TRUE,						// logging
					sTraceDetail);
	}

	if	(pIntDetailOut->uCommon.bDmaErrorInt)
		// DmaErrorInt
	{
		// set FatalAsync
		pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Unexpected AsicInt (DmaErrorInt)!",
							pDevExtIn->sPciLocShortAscii);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_ISR,
					TRUE,						// logging
					sTraceDetail);
	}

	if	(pIntDetailOut->uCommon.bBoardDemoInt)
		// BoardDemoInt
	{
		// set FatalAsync
		pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__UNEXPECTED_ASIC_INT;

		// set TraceDetail
		RtlStringCchPrintfA(sTraceDetail,
							_countof(sTraceDetail),
							"PnDev[%s]: ########## Unexpected AsicInt (BoardDemoInt)!",
							pDevExtIn->sPciLocShortAscii);

		// set trace
		fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
					ePNDEV_TRACE_CONTEXT_ISR,
					TRUE,						// logging
					sTraceDetail);
	}

	if	(pIntDetailOut->uCommon.bAsicInt)
		// any AsicInt occurred
	{
		switch	(pDevExtIn->eIcu)
				// IcuType
		{

			
            case ePNDEV_ICU_IX1000:	// IX1000
			{
				// the interrupts needs to be masked, otherwise the CbfIsr would never be called
	
				// mask all interrupts
				// bits 0-10, 13-16 zero
				WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_IX1000__INTEN),
									0x00000000);
				
				// dummy read to ensure the write is done
				(void)READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_IX1000__INTEN);
	
				// request DPC
				*pReqDpcOut = TRUE;
				
				break;
			}

			default:
			{
				break;
			}
		}
	}

	#ifdef PNDEV_GET_RUNTIME_ISR_BY_GPIO
	#endif
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	standard DPC
//************************************************************************

void fnBoard_DpcStandard(	DEVICE_EXTENSION*	pDevExtIn,
							BOOLEAN*			pReqUisrOut)
{
	// request UISR
	(*pReqUisrOut) = TRUE;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	ISR for IntTest
//************************************************************************

void fnBoard_IsrIntTest(DEVICE_EXTENSION*	pDevExtIn,
						uPNDEV_INT_DETAIL*	pIntDetailOut,
						BOOLEAN*			pReqDpcOut)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	// preset OutputParameter
	(*pReqDpcOut) = FALSE;

	switch	(pDevExtIn->eIntAction)
			// IntAction
	{
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR:					// 1* IntSequence ISR-DPC-UISR
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS:
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL:
		{
			// 1* sequence ISR/DPC/UISR must be ensured
			// -> mask DemoInt at ISR (never unmask again)

			if	(pDevExtIn->pAdminReg)
				// board with AdminReg
			{
			}
			else
			{
				if	(!fnBoard_CheckAsicDemoInt(	pDevExtIn,
												pIntDetailOut,
												TRUE))
					// AsicDemoInt not set (IR)
				{
					// Note:
					//	- this may happen at masking of interrupts
					//		DLL masks all interrupts before driver is able to update variable eIntAction

					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET;

					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## Asic-DemoInt not set!",
										pDevExtIn->sPciLocShortAscii);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_ISR,
								TRUE,						// logging
								sTraceDetail);
				}
				else
				{
					// request DPC
					*pReqDpcOut = TRUE;
				}

				// mask AsicDemoInt
				fnBoard_MaskAsicDemoInt(pDevExtIn);
			}

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_UISR:		// n* IntSequence ISR-DPC-UISR

			// continuous sequence ISR/DPC/UISR must be ensured
			// -> mask   DemoInt at ISR
			// -> unmask DemoInt at UISR

			// no break -> same actions as at ePNDEV_INT_ACTION_TEST__CONT_DPC!

		case ePNDEV_INT_ACTION_TEST__CONT_DPC:		// n* IntSequence ISR-DPC
		{
			// continuous sequence ISR/DPC must be ensured
			// -> mask   DemoInt at ISR
			// -> unmask DemoInt at DPC

			if	(!fnBoard_CheckAsicDemoInt(	pDevExtIn,
											pIntDetailOut,
											FALSE))
				// AsicDemoInt not set (IR)
			{
				// Note:
				//	- this may happen at masking of interrupts
				//		DLL masks all interrupts before driver is able to update variable eIntAction

				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Asic-DemoInt not set!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				// request DPC
				*pReqDpcOut = TRUE;
			}

			// mask AsicDemoInt
			fnBoard_MaskAsicDemoInt(pDevExtIn);

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_ISR:		// n* IntSequence ISR
		{
			// continuous sequence ISR/ISR must be ensured
			//	- DPC/UISR are never called

			// PCIe endpoint must create PCIe-AssertINTA and DeAssert-INTA messages
			//	-> IntLevel must be temporary inactive!

			if	(!fnBoard_CheckAsicDemoInt(	pDevExtIn,
											pIntDetailOut,
											FALSE))
				// AsicDemoInt not set (IR)
			{
				// Note:
				//	- this may happen at masking of interrupts
				//		DLL masks all interrupts before driver is able to update variable eIntAction

				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ASIC_DEMO_INT_NOT_SET;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Asic-DemoInt not set (may happen at masking of interrupts)!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				switch	(pDevExtIn->eIcu)
						// IcuType
				{


					case ePNDEV_ICU_IX1000:	// no TopLevel-ICU exist -> IX1000-ICU
					{
						// mask AsicDemoInt
						fnBoard_MaskAsicDemoInt(pDevExtIn);

						// unmask AsicDemoInt
						fnBoard_UnmaskAsicDemoInt(pDevExtIn);

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
		default:
		{
			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_INT_ACTION;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid IntAction!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_ISR,
						TRUE,						// logging
						sTraceDetail);

			break;
		}
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	DPC for IntTest
//************************************************************************

void fnBoard_DpcIntTest(DEVICE_EXTENSION*	pDevExtIn,
						BOOLEAN*			pReqUisrOut)
{
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};

	// preset OutputParameter
	(*pReqUisrOut) = FALSE;

	switch	(pDevExtIn->eIntAction)
			// IntAction
	{
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR:					// 1* IntSequence ISR-DPC-UISR
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS:
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL:
		{
			// 1* sequence ISR/DPC/UISR must be ensured
			// -> mask DemoInt at ISR (never unmask again)

			// request UISR
			*pReqUisrOut = TRUE;

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_UISR:		// n* IntSequence ISR-DPC-UISR
		{
			// continuous sequence ISR/DPC/UISR must be ensured
			// -> mask   DemoInt at ISR
			// -> unmask DemoInt at UISR

			// request UISR
			*pReqUisrOut = TRUE;

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_DPC:		// n* IntSequence ISR-DPC
		{
			// continuous sequence ISR/DPC must be ensured
			// -> mask   DemoInt at ISR
			// -> unmask DemoInt at DPC

			// unmask AsicDemoInt
			fnBoard_UnmaskAsicDemoInt(pDevExtIn);

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_ISR:		// n* IntSequence ISR
		{
			// continuous sequence ISR/ISR must be ensured
			//	- DPC/UISR are never called

			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_DPC_CALL;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid call of DPC (IntTest_ContIsr)!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_DPC,
						TRUE,						// logging
						sTraceDetail);

			break;
		}
		default:
		{
			// set FatalAsync
			pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__INVALID_INT_ACTION;

			// set TraceDetail
			RtlStringCchPrintfA(sTraceDetail,
								_countof(sTraceDetail),
								"PnDev[%s]: ########## Invalid IntAction!",
								pDevExtIn->sPciLocShortAscii);

			// set trace
			fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
						ePNDEV_TRACE_CONTEXT_DPC,
						TRUE,						// logging
						sTraceDetail);

			break;
		}
	}

}

//************************************************************************
//  D e s c r i p t i o n :
//
//  start IntTest
//************************************************************************

BOOLEAN fnBoard_StartIntTest(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eIntAction)
			// IntAction
	{
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR:					// 1* IntSequence ISR-DPC-UISR
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_SYS:
		case ePNDEV_INT_ACTION_TEST__ONCE_UISR_FATAL_THREAD_DLL:
		{
			// success
			bResult = TRUE;

			if	(pDevExtIn->pAdminReg != NULL)
				// board with AdminReg
			{
			}
			else
			{
				if	(fnBoard_SetAsicDemoInt(pDevExtIn,
											lDimErrorIn,
											sErrorOut))
					// setting AsicDemoInt ok
				{
					// success
					bResult = TRUE;
				}
			}

			break;
		}
		case ePNDEV_INT_ACTION_TEST__CONT_UISR:		// n* IntSequence ISR-DPC-UISR
		case ePNDEV_INT_ACTION_TEST__CONT_DPC:		// n* IntSequence ISR-DPC
		case ePNDEV_INT_ACTION_TEST__CONT_ISR:		// n* IntSequence ISR
		{
			if	(fnBoard_SetAsicDemoInt(pDevExtIn,
										lDimErrorIn,
										sErrorOut))
				// setting AsicDemoInt ok
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		default:
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Invalid IntAction!"),
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
//	set AsicDemoInt
//************************************************************************

BOOLEAN fnBoard_SetAsicDemoInt(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN bResult = FALSE;

	switch	(pDevExtIn->eIcu)
			// IcuType
	{


		case ePNDEV_ICU_IX1000:	// Intel Quark X1000 on Galileo board
		{
			// success
			bResult = TRUE;

			break;
		}
		
		default:
		{
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	mask AsicDemoInt
//************************************************************************

void fnBoard_MaskAsicDemoInt(DEVICE_EXTENSION* pDevExtIn)
{

	switch	(pDevExtIn->eIcu)
			// IcuType
	{


		case ePNDEV_ICU_IX1000:	// Intel Quark X1000 on Galileo board
		{
			// do nothing
			
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
//	unmask AsicDemoInt
//************************************************************************

void fnBoard_UnmaskAsicDemoInt(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eIcu)
			// IcuType
	{


		case ePNDEV_ICU_IX1000:
		{
			// do nothing
		
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
//	check if AsicDemoInt set (IR)
//************************************************************************

BOOLEAN fnBoard_CheckAsicDemoInt(	DEVICE_EXTENSION*	pDevExtIn,
									uPNDEV_INT_DETAIL*	pIntDetailIn,
									BOOLEAN				bHwCheckIn)
{
BOOLEAN	bResult = FALSE;


	switch	(pDevExtIn->eIcu)
			// IcuType
	{


		case ePNDEV_ICU_IX1000:	// IX1000
		{
			// nothing to do
			
			// success
			bResult = TRUE;

			break;
		}

		default:
		{
			break;
		}
	}

	return(bResult);
}

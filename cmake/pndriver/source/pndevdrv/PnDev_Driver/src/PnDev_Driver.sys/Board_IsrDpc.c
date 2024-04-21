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

    #ifdef PNDEV_SUPPORT_OTHERS
	else if	(pDevExtIn->pAdminReg != NULL)
		// board with AdminReg
	{
		// enable/disable interrupts at board globally
		fnBoardIrtePnip_ChangeIntMaskGlobal(pDevExtIn,
											bEnableIn);
	}
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
			// Eb200-PCIE
	{
		// enable/disable interrupts at board globally
		fnBoardIrtePnip_ChangeIntMaskGlobal(pDevExtIn,
											bEnableIn);
		
		if	(!bEnableIn)
			// disable interrupt
		{
			if	(pDevExtIn->pIrte != NULL)
				// valid ptr
			{
				// mask all IRTE interrupts
				WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_IRT),
									0x00000000);
				WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT),
									0x00000000);
			}
		}
		
	}
	else if	(pDevExtIn->eIcu == ePNDEV_ICU_HERA)
			// Asic Hera
	{
	UINT32 i = 0;
	
		// disable msix interrupts before and after reset
		for (i = 0; i < HERA_MSIX_VECTOR_COUNT_MAX; i++)
			// each vevtor
		{
			// mask msix vector at msix table
			*((volatile UINT32*) (pDevExtIn->pMsixIcu + ((i *0x10) + 0xc))) = 0x1;						
		}
	}
    #endif

    #ifdef PNDEV_SUPPORT_IX1000
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
    #endif

	else
	{
		if	(!bEnableIn)
			// disable interrupt
		{
			switch	(pDevExtIn->eIcu)
					// IcuType
			{
                #ifdef PNDEV_SUPPORT_OTHERS
				case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
				{
					if	(pDevExtIn->pIrte != NULL)
						// valid ptr
					{
						// mask all IRTE interrupts
						WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_IRT),
											0x00000000);
						WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT),
											0x00000000);
					}

					break;
				}
				case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
				{
					if	(pDevExtIn->pPnIp != NULL)
						// valid ptr
					{
						// mask all interrupts of PNIP-ICU2
						WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_LOW),
											0xFFFFffff);
						WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID),
											0xFFFFffff);
						WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_HIGH),
											0xFFFFffff);
					}

					break;
				}
				case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
				{
					if	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr != NULL)
						// valid ptr
					{
						// initialize Mask-Registers of PCI-ICU
						{
							// disable all events
							WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
												1);

							// mask all events of PCI-ICU-MuxA
							WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKREG_A),
												0xFFFFffff);
						}
					}

					break;
				}
				case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
				{
					if	(pDevExtIn->pArmIcu != NULL)
						// valid ptr
					{
						// initialize Mask-Registers of ARM-ICU
						{
							// disable all events
							WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASKALL),
												1);

							// mask all events of ARM-ICU-Event31:0
							WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK0),
												0xFFFFffff);

							// mask all events of ARM-ICU-Event63:32
							WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK1),
												0xFFFFffff);

							// mask all events of ARM-ICU-Event95:64
							WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK2),
												0xFFFFffff);
						}
					}

					break;
				}
				case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
				{
					if	(pDevExtIn->pPerIfIcu != NULL)
						// valid ptr
					{
						// mask all interrupts of PERIF-ICU
						WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_LOW),
											0xFFFFffff);
						WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH),
											0xFFFFffff);
					}

					break;
				}
				case ePNDEV_ICU_MICREL:	// ICU Micrell
				{
					if	(pDevExtIn->pIntelIcu != NULL)
						// valid ptr
					{
						// mask all interrupts
						// bits 0-24 zero
						WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_MICREL_KSZ88XX__INTEN),
											0x00000000);
						
						// dummy read to ensure the write is done
						(void)READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_MICREL_KSZ88XX__INTEN);
					}

					break;
				}
                #endif
                #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
                case ePNDEV_ICU_INTEL:	// ICU Intel
                {
                    if (pDevExtIn->pIntelIcu != NULL)
                        // valid ptr
                    {
                        // mask all interrupts
                        WRITE_REG_UINT32((pDevExtIn->pIntelIcu + BOARD_INTEL__IMC),
                            0xFFFFffff);
                    }

                    break;
                }
                #endif
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

#ifdef PNDEV_SUPPORT_OTHERS
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	// preset OutputParameter
	RtlZeroMemory(	pIntDetailOut,
					sizeof(uPNDEV_INT_DETAIL));

	switch	(pDevExtIn->eIcu)
			// IcuType
	{
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
		
			if	(pDevExtIn->eBoard == ePNDEV_BOARD_DB_EB200_PCIE)
				// Eb200-PCIe
			{
				if	(pDevExtIn->pAdminReg == NULL)
					// AdminReg not exist
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST;
	
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## Admin register not exist!",
										pDevExtIn->sPciLocShortAscii);
	
					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_ISR,
								TRUE,						// logging
								sTraceDetail);
				}
				else
				{
					if	(fnBoardIrtePnip_CheckIntAdminReg(	pDevExtIn,
															pIntDetailOut))
						// any interrupt at AdminReg
					{
						if	(pIntDetailOut->uCommon.bAsicInt)
							// any AsicInt occurred
						{
							pIntDetailOut->as.uIrteIcu.lIrIrt	= READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_IR_IRT);
							pIntDetailOut->as.uIrteIcu.lIrNrt	= READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_IR_NRT);
				
							if	(	(pIntDetailOut->as.uIrteIcu.lIrIrt	!= 0)
								||	(pIntDetailOut->as.uIrteIcu.lIrNrt	!= 0))
								// any IRTE interrupt (IR)
							{
								// success
								bResult = TRUE;
							}
							else
							{
								
								// set AsicEoi
								//	- a running EOI-Timer is restarted if EOI is written again!
								//  - otherwise interhupthandling could be get stuck!
								fnBoard_IsrSetEoi(pDevExtIn);								
							}
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
				pIntDetailOut->as.uIrteIcu.lIrIrt	= READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_IR_IRT);
				pIntDetailOut->as.uIrteIcu.lIrNrt	= READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_IR_NRT);
	
				if	(	(pIntDetailOut->as.uIrteIcu.lIrIrt	!= 0)
					||	(pIntDetailOut->as.uIrteIcu.lIrNrt	!= 0))
					// any IRTE interrupt (IR)
				{
					// success
					bResult = TRUE;
	
					// any AsicInt occurred
					pIntDetailOut->uCommon.bAsicInt = TRUE;

				}
				else
				{					
					// set AsicEoi
					//	- a running EOI-Timer is restarted if EOI is written again!
					//  - otherwise interhupthandling could be get stuck!
					fnBoard_IsrSetEoi(pDevExtIn);								
				}
			}

			break;		
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			pIntDetailOut->as.uPnipIcu2.lIrLow	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_LOW);
			pIntDetailOut->as.uPnipIcu2.lIrMid	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MID);
			pIntDetailOut->as.uPnipIcu2.lIrHigh	= READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_HIGH);

			if	(	(pIntDetailOut->as.uPnipIcu2.lIrLow		!= 0)
				||	(pIntDetailOut->as.uPnipIcu2.lIrMid		!= 0)
				||	(pIntDetailOut->as.uPnipIcu2.lIrHigh	!= 0))
				// any PNIP-ICU2 interrupt (IR)
			{
				// success
				bResult = TRUE;

				// any AsicInt occurred
				pIntDetailOut->uCommon.bAsicInt = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// read PCI-ICU-MuxA-Vector
			pIntDetailOut->as.uPciIcu.lIrVec = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

			if	(pIntDetailOut->as.uPciIcu.lIrVec != 0)
				// PCI-ICU-MuxA event set (0=DefaultVector)
			{
				// success
				bResult = TRUE;

				// any AsicInt occurred
				pIntDetailOut->uCommon.bAsicInt = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			// boards with this TopLevel-ICU have AdminReg

			if	(pDevExtIn->pAdminReg == NULL)
				// AdminReg not exist
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Admin register not exist!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				if	(fnBoardIrtePnip_CheckIntAdminReg(	pDevExtIn,
														pIntDetailOut))
					// any interrupt exist at AdminReg
				{
					// success
					bResult = TRUE;

					if	(pIntDetailOut->uCommon.bAsicInt)
						// any AsicInt occurred
					{
						// read ARM-ICU-Vector
						pIntDetailOut->as.uArmIcu.lIrVec = READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__IRVEC);

						if	(pIntDetailOut->as.uArmIcu.lIrVec == 0)
							// no ARM-ICU event set (0=DefaultVector)
						{
							// Note:
							//	- AdminReg shows result of TopLevel-ICU
							//	- TopLevel-ICU is acked by posted write
							//		-> it may happen that this posted write is done between checking AdminReg and TopLevel-ICU
							//		-> AdminReg may show TRUE while TopLevel-ICU shows FALSE

							// correct result
							bResult = FALSE;
						}
					}
				}
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// boards with this TopLevel-ICU have AdminReg

			if	(pDevExtIn->pAdminReg == NULL)
				// AdminReg not exist
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ADMIN_REG_NOT_EXIST;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Admin register not exist!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}
			else
			{
				if	(fnBoardIrtePnip_CheckIntAdminReg(	pDevExtIn,
														pIntDetailOut))
					// any interrupt at AdminReg
				{
					// success
					bResult = TRUE;

					if	(pIntDetailOut->uCommon.bAsicInt)
						// any AsicInt occurred
					{
						pIntDetailOut->as.uPerifIcu.lIrLow	= READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_LOW);
						pIntDetailOut->as.uPerifIcu.lIrHigh	= READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_HIGH);

						if	(	(pIntDetailOut->as.uPerifIcu.lIrLow		== 0)
							&&	(pIntDetailOut->as.uPerifIcu.lIrHigh	== 0))
							// no PERIF-ICU interrupt (IR)
						{
							// Note:
							//	- AdminReg shows result of TopLevel-ICU
							//	- TopLevel-ICU is acked by posted write
							//		-> it may happen that this posted write is done between checking AdminReg and TopLevel-ICU
							//		-> AdminReg may show TRUE while TopLevel-ICU shows FALSE

							// correct result
							bResult = FALSE;
						}
					}
				}
			}

			break;
		}
        case ePNDEV_ICU_MICREL:	// Micrel
        {
            pIntDetailOut->as.uMicrel.lIntst = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_MICREL_KSZ88XX__INTST);

            if (pIntDetailOut->as.uMicrel.lIntst != 0)
                // any interrupt
            {
                // success
                bResult = TRUE;

                // any AsicInt occurred
                pIntDetailOut->uCommon.bAsicInt = TRUE;
            }

            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
            // success
            bResult = TRUE;

            // once a msix interrupt occured, it is not necessary to look it up again
            // any AsicInt occurred
            pIntDetailOut->uCommon.bAsicInt = TRUE;

            break;
        }
        #endif

        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
		case ePNDEV_ICU_INTEL:	// Intel
		{
			pIntDetailOut->as.uIntel.lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICR);

			if	(pIntDetailOut->as.uIntel.lIcr != 0)
				// any interrupt (ICR)
			{
				WRITE_REG_UINT32(	pDevExtIn->pIntelIcu + BOARD_INTEL__ICS,
									pIntDetailOut->as.uIntel.lIcr);

				// success
				bResult = TRUE;

				// any AsicInt occurred
				pIntDetailOut->uCommon.bAsicInt = TRUE;
			}

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
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
        #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// set Asic-EOI (50MHz -> (15+1) * 20ns = 320ns)
			WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_EOI),
				0x0000000F);

			// ensure that last PCI-Write is done (read on same IP)			
			(void)READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_EOI);

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// set EOI-InactiveTime
			//	- resolution=1024nsec
			//	- 100usec = 100000/1024 = 98
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_EOI),
								98);

			// ensure that last PCI-Write is done (read on same IP)			
			(void)READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_EOI);

			// PCIe board -> EOI done

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// AsicEoi need not to be set because ISREG is not used
/*
			// set PCI-ICU-EOI
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_END_A),
								0x0000);

			// ensure that last PCI-Write is done (read on same IP)			
			(void)READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_END_A);
*/
			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			// Note:
			//	- setting of EOI not necessary because EOI resets only priority scheduler
			//	- clearing of ISREG reactivates interrupts with same or lower priority

			// set EOI of ARM-ICU
			//WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__EOI),
			//					1);

			// PCIe board -> EOI done

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// set EOI-InactiveTime of PERIF-ICU
			//	- resolution=32nsec if clock=31,25MHz	(1usec = 1000/32 = 32)
			//	- resolution=8nsec  if clock=125MHz		(1usec = 1000/8 = 125)
			//	- IntLogic of BoardFpga (Bar0) has a SpikeFilter of ~25nsec
			WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_EOI),
								125);

			// ensure that last PCI-Write is done (read on same IP)			
			(void)READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_EOI);

			// PCIe board -> EOI done

			break;
		}
        case ePNDEV_ICU_MICREL:
        {
            // no setting of EOI because it does not exist
            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
            // no setting of EOI because it does not exist
            break;
        }		
        #endif
		
        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
        case ePNDEV_ICU_INTEL:
		{
			// no setting of EOI because it does not exist
			break;
		}
        #endif
		
        #ifdef PNDEV_SUPPORT_IX1000
        case ePNDEV_ICU_IX1000:
		{
			// no setting of EOI because it does not exist
			break;
		}
        #endif

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
            #ifdef PNDEV_SUPPORT_OTHERS
			case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
			{
				// mask all interrupts of IRTE-ICU
				WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_IRT),
									0x00000000);
				WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT),
									0x00000000);

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
			case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
			{
				// mask all interrupts of PNIP-ICU2
				WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_LOW),
									0xFFFFffff);
				WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID),
									0xFFFFffff);
				WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_HIGH),
									0xFFFFffff);

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
			{
				// disable all events
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
									1);

				// debug: read PCI-ICU-MuxA-Vector (must be 0=DefaultVector if interrupts are masked)
				(void)READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
			{
				// disable all events
				WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASKALL),
									1);

				// debug: after masking of all interrupts Vector=DefaultVector=0 -> no unmasked pending interrupt
				(void)READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__IRVEC);

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
			case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
			{
				// mask all interrupts of PERIF-ICU
				WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_LOW),
									0xFFFFffff);
				WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH),
									0xFFFFffff);

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
            case ePNDEV_ICU_MICREL:	// Micrel
            {
                // the interrupts needs to be masked, otherwise the CbfIsr would never be called

                // mask all interrupts
                // bits 0-24 zero
                WRITE_REG_UINT32((pDevExtIn->pIntelIcu + BOARD_MICREL_KSZ88XX__INTEN),
                    0x00000000);

				// dummy read to ensure the write is done
                (void)READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_MICREL_KSZ88XX__INTEN);

                // request DPC
                *pReqDpcOut = TRUE;

                break;
            }
            case ePNDEV_ICU_HERA:	// Hera
            {
                //UINT32 i = 0;

                //for (i = 0; i<pDevExtIn->lIntCount; i++)
                // each vevtor
                //{
                // mask msix vector at msix table
                //	*((volatile UINT32*) (pDevExtIn->pMsixIcu + ((i *0x10) + 0xc))) = 0x1;						
                //}

                // mask corresponding msix vector at msix table
                *((volatile UINT32*)(pDevExtIn->pMsixIcu + ((lMsgIdMsiIn * 0x10) + 0xc))) = 0x1;

                // request DPC
                *pReqDpcOut = TRUE;

                break;
            }
            #endif

            #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
			case ePNDEV_ICU_INTEL:	// Intel
			{
				// the interrupts needs to be masked, otherwise the CbfIsr would never be called

				switch(pDevExtIn->eBoard)
				{
					case ePNDEV_BOARD_I210:
					case ePNDEV_BOARD_I210SFP:
					{
						// mask all interrupts
						WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_INTEL__IMC),
											0xFFFFffff);

						break;
					}

					case ePNDEV_BOARD_I82574:
					{
						// mask all interrupts
						WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uI82574.uBar0.pPtr + BOARD_INTEL__IMC),
											0xFFFFffff);

						break;
					}
					default:
					{
						break;
					}
				}

				// request DPC
				*pReqDpcOut = TRUE;

				break;
			}
            #endif
			
            #ifdef PNDEV_SUPPORT_IX1000
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
            #endif

			default:
			{
				break;
			}
		}
	}

	#ifdef PNDEV_GET_RUNTIME_ISR_BY_GPIO
    #ifdef PNDEV_SUPPORT_OTHERS

		if	(pDevExtIn->eBoard == ePNDEV_BOARD_FPGA1_ERTEC200P)
			// FPGA1-Ertec200P board
		{
			// GPIO2=0
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_GPIO + ERTEC200P_GPIO_REG__OUT_CLEAR_0),
								PNDEV_UINT32_SET_BIT_2);
		}
    #endif
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
                #ifdef PNDEV_SUPPORT_OTHERS
				if	(!pIntDetailOut->uCommon.bBoardDemoInt)
					// BoardDemoInt not set
				{
					// set FatalAsync
					pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__BOARD_DEMO_INT_NOT_SET;

					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## Board-Interrupt not set!",
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

				// clear BoardDemoInt
				//	- analog IntAck which clears interrupt level
				fnBoardIrtePnip_ChangeBoardDemoInt(	pDevExtIn,
													FALSE);
                #endif
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
                    #ifdef PNDEV_SUPPORT_OTHERS
					case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
					{
						// IntLevel will be temporary inactive by EOI-InactiveTime of PN-ICU

						break;
					}
					case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
					{
						// IntLevel will be temporary inactive by EOI-InactiveTime of PN-ICU

						break;
					}
					case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
					{
						// mask AsicDemoInt
						fnBoard_MaskAsicDemoInt(pDevExtIn);

						// unmask AsicDemoInt
						fnBoard_UnmaskAsicDemoInt(pDevExtIn);

						break;
					}
					case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
					{
						// mask AsicDemoInt
						fnBoard_MaskAsicDemoInt(pDevExtIn);

						// unmask AsicDemoInt
						fnBoard_UnmaskAsicDemoInt(pDevExtIn);

						break;
					}
					case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
					{
						// IntLevel will be temporary inactive by EOI-InactiveTime of PERIF-ICU

						break;
					}
                    case ePNDEV_ICU_MICREL:	// no TopLevel-ICU exist -> MICREL-ICU
                    {
                        // mask AsicDemoInt
                        fnBoard_MaskAsicDemoInt(pDevExtIn);

                        // unmask AsicDemoInt
                        fnBoard_UnmaskAsicDemoInt(pDevExtIn);

                        break;
                    }
                    case ePNDEV_ICU_HERA:	// Hera
                    {
                        // mask AsicDemoInt
                        fnBoard_MaskAsicDemoInt(pDevExtIn);

                        // unmask AsicDemoInt
                        fnBoard_UnmaskAsicDemoInt(pDevExtIn);

                        break;
                    }
                    #endif

                    #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
					case ePNDEV_ICU_INTEL:	// no TopLevel-ICU exist -> INTEL-ICU
					{
						// mask AsicDemoInt
						fnBoard_MaskAsicDemoInt(pDevExtIn);

						// unmask AsicDemoInt
						fnBoard_UnmaskAsicDemoInt(pDevExtIn);

						break;
					}
                    #endif

                    #ifdef PNDEV_SUPPORT_IX1000
					case ePNDEV_ICU_IX1000:	// no TopLevel-ICU exist -> IX1000-ICU
					{
						// mask AsicDemoInt
						fnBoard_MaskAsicDemoInt(pDevExtIn);

						// unmask AsicDemoInt
						fnBoard_UnmaskAsicDemoInt(pDevExtIn);

						break;
					}
                    #endif

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
                #ifdef PNDEV_SUPPORT_OTHERS
				// set BoardDemoInt
				fnBoardIrtePnip_ChangeBoardDemoInt(	pDevExtIn,
													TRUE);
                #endif
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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
		UINT32 lIrrNrt = 0;

			// IRTE-ICU already initialized by fnBoardIrte_InitIrteIcu()
			//	- IrqLevel ok
			//	- all IRQs masked

			// set IRTE-IrqNrt-HP
			WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRR_NRT),
								(READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRR_NRT) | IRTE_IRQ_NRT__HP));

			lIrrNrt = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRR_NRT);

			if	((lIrrNrt & IRTE_IRQ_NRT__HP) != IRTE_IRQ_NRT__HP)
				// IRTE-IrqNrt-HP not set (IRR)
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Asic DemoInt not set!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
		UINT32 lIrrMid = 0;

			// set PNIP-ICU-Event63
			//	- access by OR is not necessary because reading of the register returns always 0
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_ACTIVATE),
								PNDEV_UINT32_SET_BIT_3);

			lIrrMid = READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_EVENT_MID);

			if	((lIrrMid & PNDEV_UINT32_SET_BIT_31) == 0)
				// PNIP-ICU-Event63 not set (IRR)
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Asic DemoInt not set!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
		UINT32	lIrrMuxB	= 0;
		BOOLEAN	bResultMuxB	= FALSE;

			// Note:
			//	- only for testing
			//	- inverted signal PCI_INTA# is available as INTA_IN_ICU and is connected to Pin91 of PCI-ICU
			//	- a falling edge of PCI_INTA# can be detected as a rising edge of INTA_IN_ICU
			{
				// connect INTA_IN_ICU to PCI-ICU-MuxB-Input1 (default: rising edge)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_B1),
									91);

				// ack PCI-ICU-MuxB-Event1 (Vector1)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRCLVEC_B),
									1);

				// read PCI-ICU-MuxB-IRR (must be 0)
				lIrrMuxB = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRREG_B);

				if	(lIrrMuxB != 0)
					// error
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at ack of PCI-ICU-MuxB!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					// success
					bResultMuxB = TRUE;
				}
			}

			if	(bResultMuxB)
				// success
			{
			UINT32	lIrrMuxA	= 0;
			UINT32	lIrVecMuxA	= 0;

				// FSpec S.363
				//	- IRR-bit is set when the appropriate bit in SWIRREG indicates a rising edge
				//	- SWIRREG must be set to 0 before IRR-bit is cleared by IRCLVEC. Otherwise IRR is only inactive for 1 clock!
				{
					// set PCI-ICU-MuxA-Event31 by software -> rising edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SWIRREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SWIRREG_A) | PNDEV_UINT32_SET_BIT_31));

					// clear PCI-ICU-MuxA-Event31 -> falling edge
					WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SWIRREG_A),
										(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SWIRREG_A) & PNDEV_UINT32_CLR_BIT_31));
				}

				// read PCI-ICU-MuxA-IRR
				lIrrMuxA = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRREG_A);

				// read PCI-ICU-MuxA-Vector (must be 0=DefaultVector if interrupts are masked)
				lIrVecMuxA = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

				if	((lIrrMuxA & PNDEV_UINT32_SET_BIT_31) == 0)
					// PCI-ICU-MuxA-Event31 not set (IRR)
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Asic DemoInt not set!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else if	(lIrVecMuxA != 0)
						// PCI-ICU-MuxA-Vector != 0 (must be 0=DefaultVector if interrupts are masked)
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): PCI-ICU-MuxA-Vector set although interrupts are masked!"),
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

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
		UINT32 lIrr2 = 0;

			// set ARM-ICU-Event95 by software
			WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__SWIIR2),
								(READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__SWIIR2) | PNDEV_UINT32_SET_BIT_31));

			lIrr2 = READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__IRR2);

			if	((lIrr2 & PNDEV_UINT32_SET_BIT_31) == 0)
				// ARM-ICU-Event95 not set (IRR)
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Asic DemoInt not set!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
		UINT32 lIrrHigh = 0;

			// set PERIF-ICU-Event63 by software
			WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_EVENT_HIGH),
								(READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_EVENT_HIGH) | PNDEV_UINT32_SET_BIT_31));

			lIrrHigh = READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_EVENT_HIGH);

			if	((lIrrHigh & PNDEV_UINT32_SET_BIT_31) == 0)
				// PERIF-ICU-Event63 not set (IRR)
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Asic DemoInt not set!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResult = TRUE;
			}

			break;
		}
        case ePNDEV_ICU_MICREL:	// Micrel
        {
            // success
            bResult = TRUE;

            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
            UINT32 	lIcr = 0;

            if (pDevExtIn->lBitStreamId == 0x09)
                // Inc 9 - TODO Remove
            {
                // set INT_SW_16 by software
                WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_SET_REG_INC9),
                    (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_SET_REG_INC9) | PNDEV_UINT32_SET_BIT_8));

                // read if interrupt was set
                lIcr = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_REG_INC9);
                
                if (((lIcr & PNDEV_UINT32_SET_BIT_16) == 0)
                    && ((lIcr & PNDEV_UINT32_SET_BIT_8) == 0))
                    // DemoInt
                {
                    // set ErrorString
                    fnBuildString(_TEXT(__FUNCTION__),
                        _TEXT("(): Asic DemoInt not set!"),
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

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// set INT_SW_16 by software
				WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_SET_REG),
					(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_SET_REG) | PNDEV_UINT32_SET_BIT_16));

				// read if interrupt was set
				lIcr = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_REG);
                
                if (((lIcr & PNDEV_UINT32_SET_BIT_16) == 0))
                    // DemoInt
                {
                    // set ErrorString
                    fnBuildString(_TEXT(__FUNCTION__),
                        _TEXT("(): Asic DemoInt not set!"),
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
            
            break;
        }
        #endif

        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
		case ePNDEV_ICU_INTEL:	// Intel
		{
		UINT32 lIcr = 0;

			switch(pDevExtIn->eBoard)
			{
                #ifdef PNDEV_SUPPORT_I210
				case ePNDEV_BOARD_I210:				
				{
					// set by software
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS) & BOARD_I210__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

					lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS);

					if	((lIcr & PNDEV_UINT32_SET_BIT_0) == 0)
						// PERIF-ICU-Event63 not set (IRR)
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Asic DemoInt not set!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}

					break;
				}
                #endif

                #ifdef PNDEV_SUPPORT_I210SFP
                case ePNDEV_BOARD_I210SFP:
                {
                    // set by software
                    WRITE_REG_UINT32(   (pDevExtIn->pIntelIcu + BOARD_INTEL__ICS),
                                        ((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS) & BOARD_I210SFP__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

                    lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS);

                    if ((lIcr & PNDEV_UINT32_SET_BIT_0) == 0)
                        // PERIF-ICU-Event63 not set (IRR)
                    {
                        // set ErrorString
                        fnBuildString(  _TEXT(__FUNCTION__),
                                        _TEXT("(): Asic DemoInt not set!"),
                                        NULL,
                                        lDimErrorIn,
                                        sErrorOut);
                    }
                    else
                    {
                        // success
                        bResult = TRUE;
                    }

                    break;
                }
                #endif

                #ifdef PNDEV_SUPPORT_I82574
				case ePNDEV_BOARD_I82574:
				{
						
					// set by software
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS) & BOARD_I82574__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0 | PNDEV_UINT32_SET_BIT_24));

					lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICS);

					if	((lIcr & PNDEV_UINT32_SET_BIT_0) == 0)
						// DemoInt
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Asic DemoInt not set!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						// success
						bResult = TRUE;
					}

					break;
				}
                #endif

				default:
				{
					break;
				}
			}

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
		case ePNDEV_ICU_IX1000:	// Intel Quark X1000 on Galileo board
		{
			// success
			bResult = TRUE;

			break;
		}
        #endif
		
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
#ifdef PNDEV_SUPPORT_OTHERS
char sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	switch	(pDevExtIn->eIcu)
			// IcuType
	{
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// mask IRTE-IrqNrt-HP
			WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT),
								(READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT) & (~IRTE_IRQ_NRT__HP)));

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// mask PNIP-ICU-Event63
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID),
								(READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID) | PNDEV_UINT32_SET_BIT_31));

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
		UINT32 lIrVec = 0;

			// mask PCI-ICU-MuxA-Event31
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKREG_A),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKREG_A) | PNDEV_UINT32_SET_BIT_31));

			// disable all events
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
								1);

			// read PCI-ICU-MuxA-Vector (must be 0=DefaultVector if interrupts are masked)
			lIrVec = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

			if	(lIrVec != 0)
				// error
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_INT_MASK;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Interrupt mask error!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
		UINT32 lIrVec = 0;

			// mask ARM-ICU-Event95
			WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK2),
								(READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK2) | PNDEV_UINT32_SET_BIT_31));

			// read ARM-ICU-Vector (must be 0=DefaultVector if interrupts are masked)
			lIrVec = READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__IRVEC);

			if	(lIrVec != 0)
				// error
			{
				// set FatalAsync
				pDevExtIn->uEvent.eFatalAsync = ePNDEV_FATAL_ASYNC__ERROR_INT_MASK;

				// set TraceDetail
				RtlStringCchPrintfA(sTraceDetail,
									_countof(sTraceDetail),
									"PnDev[%s]: ########## Interrupt mask error!",
									pDevExtIn->sPciLocShortAscii);

				// set trace
				fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
							ePNDEV_TRACE_CONTEXT_ISR,
							TRUE,						// logging
							sTraceDetail);
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// mask PERIF-ICU-Event63
			WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH),
								(READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH) | PNDEV_UINT32_SET_BIT_31));

			break;
		}
        case ePNDEV_ICU_MICREL:	// Micrell
        {
            // do nothing

            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
			if (pDevExtIn->lBitStreamId == 0x09)
				// Inc9 - TODO Remove
			{
				// mask INT_SW_16 msix vector 8 at msix table
				*((volatile UINT32*)(pDevExtIn->pMsixIcu + ((8 * 0x10) + 0xc))) = 0x1;
			}

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// mask INT_SW_16 msix vector 10 at msix table
				*((volatile UINT32*)(pDevExtIn->pMsixIcu + ((10 * 0x10) + 0xc))) = 0x1;
			}

            break;
        }
        #endif

        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
		case ePNDEV_ICU_INTEL:	// Intel
		{
			switch(pDevExtIn->eBoard)
			{
                #ifdef PNDEV_SUPPORT_I210
				case ePNDEV_BOARD_I210:				
				{
					// mask Bit 0
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__IMC),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMC) & BOARD_I210__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

					break;
				}
                #endif

                #ifdef PNDEV_SUPPORT_I210SFP
                case ePNDEV_BOARD_I210SFP:
                {
                    // mask Bit 0
                    WRITE_REG_UINT32(   (pDevExtIn->pIntelIcu + BOARD_INTEL__IMC),
                                        ((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMC) & BOARD_I210SFP__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

                    break;
                }
                #endif

                #ifdef PNDEV_SUPPORT_I82574
				case ePNDEV_BOARD_I82574:
				{
					// mask Bit 0
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__IMC),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMC) & BOARD_I82574__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_24));

					break;
				}
                #endif

				default:
				{
					break;
				}
			}

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
		case ePNDEV_ICU_IX1000:	// Intel Quark X1000 on Galileo board
		{
			// do nothing
			
			break;
		}
        #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
			// unmask IRTE-IrqNrt-HP
			WRITE_REG_UINT32(	(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT),
								(READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_MASK_NRT) | IRTE_IRQ_NRT__HP));

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
			// unmask PNIP-ICU-Event63
			WRITE_REG_UINT32(	(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID),
								READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MASK_MID) & PNDEV_UINT32_CLR_BIT_31);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
			// unmask PCI-ICU-MuxA-Event31
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKREG_A),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKREG_A) & PNDEV_UINT32_CLR_BIT_31));

			// enable all unmasked events
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_MASKALL_A),
								0);

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
			// unmask ARM-ICU-Event95
			WRITE_REG_UINT32(	(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK2),
								(READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__MASK2) & PNDEV_UINT32_CLR_BIT_31));

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			// unmask PERIF-ICU-Event63
			WRITE_REG_UINT32(	(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH),
								(READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_MASK_HIGH) & PNDEV_UINT32_CLR_BIT_31));

			break;
		}
        case ePNDEV_ICU_MICREL:
        {
            // do nothing

            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
			if (pDevExtIn->lBitStreamId == 0x09)
				// Inc9 - TODO Remove
			{
				// unmask INT_SW_16 msix vector 8 at msix table
				*((volatile UINT32*)(pDevExtIn->pMsixIcu + ((8 * 0x10) + 0xc))) = 0x0;
			}

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// unmask INT_SW_16 msix vector 10 at msix table
				*((volatile UINT32*)(pDevExtIn->pMsixIcu + ((10 * 0x10) + 0xc))) = 0x0;
			}

            break;
        }
        #endif

        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
		case ePNDEV_ICU_INTEL:
		{
			switch(pDevExtIn->eBoard)
			{
                #ifdef PNDEV_SUPPORT_I210
				case ePNDEV_BOARD_I210:				
				{
					// mask Bit 0
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__IMS),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMS) & BOARD_I210__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

					break;
				}
                #endif

                #ifdef PNDEV_SUPPORT_I210SFP
                case ePNDEV_BOARD_I210SFP:
                {
                    // mask Bit 0
                    WRITE_REG_UINT32(   (pDevExtIn->pIntelIcu + BOARD_INTEL__IMS),
                                        ((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMS) & BOARD_I210SFP__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_0));

                    break;
                }
                #endif

                #ifdef PNDEV_SUPPORT_I82574
				case ePNDEV_BOARD_I82574:
				{
					// mask Bit 24 for MSIX
					WRITE_REG_UINT32(	(pDevExtIn->pIntelIcu + BOARD_INTEL__IMS),
										((READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__IMS) & BOARD_I82574__ICU_RESERVED_MASK) | PNDEV_UINT32_SET_BIT_24));

					break;
				}
                #endif

				default:
				{
					break;
				}
			}

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
		case ePNDEV_ICU_IX1000:
		{
			// do nothing
		
			break;
		}
        #endif

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

#ifdef PNDEV_SUPPORT_OTHERS
char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = {0};
#endif

	switch	(pDevExtIn->eIcu)
			// IcuType
	{
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_ICU_IRTE:	// no TopLevel-ICU exist -> IRTE-ICU
		{
		UINT32 lIrNrt = 0;

			lIrNrt = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__ICU_IRQ1_IR_NRT);

			if	((lIrNrt & IRTE_IRQ_NRT__HP) != 0)
				// IRTE-IrqNrt-HP set (IR)
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_PNIP:	// no TopLevel-ICU exist -> PNIP-ICU2
		{
		UINT32 lIrMid = 0;

			lIrMid = READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__ICU2_IRQ0_MID);

			if	((lIrMid & PNDEV_UINT32_SET_BIT_31) != 0)
				// PNIP-ICU-Event63 set (IR)
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PCI:	// TopLevel-ICU = PCI-ICU
		{
		UINT32 lIrVec = 0;

			if	(bHwCheckIn)
				// HwCheck
			{
			UINT32 lIrrMuxB = 0;

				// check PCI-ICU-MuxB
				//	- only for testing
				//	- disconnecting at end -> continuous HwTest only possible if fnBoard_SetAsicDemoInt() is always called before!

				// read PCI-ICU-MuxB-IRR
				lIrrMuxB = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRREG_B);

				if	(lIrrMuxB != PNDEV_UINT32_SET_BIT_1)
					// error
				{
					// set TraceDetail
					RtlStringCchPrintfA(sTraceDetail,
										_countof(sTraceDetail),
										"PnDev[%s]: ########## PCI-ICU-MuxB-IRR not set!",
										pDevExtIn->sPciLocShortAscii);

					// set trace
					fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
								ePNDEV_TRACE_CONTEXT_ISR,
								TRUE,						// logging
								sTraceDetail);
				}

				// ack PCI-ICU-MuxB-Event1 (Vector1)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRCLVEC_B),
									1);

				// debug: read PCI-ICU-MuxB-IRR (must be 0)
				lIrrMuxB = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRREG_B);

				// disconnect INTA_IN_ICU from PCI-ICU-MuxB-Input1
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_SEL_B1),
									0);
			}

			// read PCI-ICU-MuxA-Vector
			lIrVec = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uSoc1.uPciIcu.pPtr + SOC1_PCI_ICU_REG__IRQ_IRVEC_A);

			if	(lIrVec == 31)
				// PCI-ICU-Event31 set
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_ARM:	// TopLevel-ICU = ARM-ICU
		{
		UINT32 lIrVec = 0;

			// read ARM-ICU-Vector
			lIrVec = READ_REG_UINT32(pDevExtIn->pArmIcu + ERTEC200P_ARM_ICU_REG__IRVEC);

			if	(lIrVec == ERTEC200P_ARM_ICU_VECTOR_HIGH__95)
				// ARM-ICU-Event95 set
			{
				// success
				bResult = TRUE;
			}

			break;
		}
		case ePNDEV_ICU_TOP_LEVEL_PERIF:	// TopLevel-ICU = PERIF-ICU
		{
			if	((READ_REG_UINT32(pDevExtIn->pPerIfIcu + ERTEC200P_PERIF_REG__HOST_IRQ_HIGH) & PNDEV_UINT32_SET_BIT_31) != 0)
				// PERIF-ICU-Event63 set (IR)
			{
				// success
				bResult = TRUE;
			}

			break;
		}
        case ePNDEV_ICU_MICREL:	// Micrel
        {
            // nothing to do

            // success
            bResult = TRUE;

            break;
        }
        case ePNDEV_ICU_HERA:	// Hera
        {
            // nothing to do
            // there is no possibility to read an irr

            // clear AsicDemoInt, because scrb is in dll not reachable

            if (pDevExtIn->lBitStreamId == 0x09)
                // Inc 9 - TODO Remove
            {
                // clear INT_SW_16 by software
                WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_CLEAR_REG_INC9),
                    (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_CLEAR_REG_INC9) | PNDEV_UINT32_SET_BIT_8));

                // read for debug purpose
                (void)READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_REG_INC9);
            }

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// clear INT_SW_16 by software
				WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_CLEAR_REG),
					(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_CLEAR_REG) | PNDEV_UINT32_SET_BIT_16));

				// read for debug purpose
				(void)READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SCRB + HERA_SCRB_REG__SW_INT_REG);
			}

            // success
            bResult = TRUE;

            break;

			//lint --e(550) lTmp not accessed - yes, but it is needed, // debug purpose
        }
        #endif

        #if defined(PNDEV_SUPPORT_I210) || defined(PNDEV_SUPPORT_I210SFP) || defined(PNDEV_SUPPORT_I82574)
		case ePNDEV_ICU_INTEL:	// Intel
		{
			// read Icr
			pIntDetailIn->as.uIntel.lIcr = READ_REG_UINT32(pDevExtIn->pIntelIcu + BOARD_INTEL__ICR);

			if	((pIntDetailIn->as.uIntel.lIcr & PNDEV_UINT32_SET_BIT_0) != 0)
				// Event0 set
			{
				// success
				bResult = TRUE;
			}

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
		case ePNDEV_ICU_IX1000:	// IX1000
		{
			// nothing to do
			
			// success
			bResult = TRUE;

			break;
		}
        #endif

		default:
		{
			break;
		}
	}

	return(bResult);
}

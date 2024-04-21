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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;
			
			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

			// set FctPtr
			//	- no dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrte_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			//	- no TopLevel-ICU exist -> IRTE-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_IRTE;
			
			if	(	(pDevExtIn->eBoard != ePNDEV_BOARD_EB200)
				&&	(pDevExtIn->eBoard != ePNDEV_BOARD_DB_EB200_PCIE))
				// !Ertec200
			{
				pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_FIRST;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData and HostMem
			pDevExtIn->bFlashSupport 		= TRUE;
			pDevExtIn->bCopyDataSupport 	= TRUE;
			pDevExtIn->eValidHostMemPool 	= ePNDEV_VAL_HOST_MEM_FIRST;
			
			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

			// set FctPtr
			//	- no dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrte_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			//	- TopLevel-ICU = PCI-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PCI;

			break;
		}
		case ePNDEV_BOARD_CP1625:	// CP1625 board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData and HostMem
			pDevExtIn->bFlashSupport		= TRUE;
			pDevExtIn->bCopyDataSupport		= TRUE;
			pDevExtIn->eValidHostMemPool	= ePNDEV_VAL_HOST_MEM_FIRST;

			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

			// set FctPtr
			//	- no dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoardIrte_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif = NULL;

			// update IcuType
			//	- TopLevel-ICU = PCI-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PCI;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			// success
			bResult = TRUE;

			// support Flash,CopyData and no HostMem
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;
			
			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

			// set FctPtr
			pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX) fnBoardIrtePnip_Reset;

			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// set FctPtr
					//	- no dynamic UserPage exist
					pDevExtIn->pFnBoard_ChangePageXhif = NULL;

					#ifdef FPGA1_ERTEC200P_ARM_NO_TOPLEVEL_ICU

						// update IcuType
						//	- no TopLevel-ICU exist -> PNIP-ICU2
						pDevExtIn->eIcu = ePNDEV_ICU_PNIP;

					#else

						// update IcuType
						//	- TopLevel-ICU = ARM-ICU
						pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_ARM;

					#endif

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// set FctPtr
					//	- dynamic UserPage exist
					pDevExtIn->pFnBoard_ChangePageXhif = (FN_BOARD_XX) fnBoardPnip_ChangePageXhif;

					// update IcuType
					//	- TopLevel-ICU = PERIF-ICU
					pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PERIF;

					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= TRUE;
			pDevExtIn->bCopyDataSupport = TRUE;
			
			// set Interrupt Mode
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoardIrtePnip_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= (FN_BOARD_XX) fnBoardPnip_ChangePageXhif;

			// update IcuType
			//	- TopLevel-ICU = PERIF-ICU
			pDevExtIn->eIcu = ePNDEV_ICU_TOP_LEVEL_PERIF;

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
        {
            // success
            bResult = TRUE;

            // support Flash and CopyData
            pDevExtIn->bFlashSupport = TRUE;
            pDevExtIn->bCopyDataSupport = TRUE;

            // set Interrupt Mode
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_MSIX;

            // support HostMem
            pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

            // set FctPtr
            //	- dynamic UserPage exist
            pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoardIrtePnip_Reset;
            pDevExtIn->pFnBoard_ChangePageXhif = NULL;

            // update IcuType
            pDevExtIn->eIcu = ePNDEV_ICU_HERA;

            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
        case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
        {
            // success
            bResult = TRUE;

            // support Flash and CopyData
            pDevExtIn->bFlashSupport = FALSE;
            pDevExtIn->bCopyDataSupport = FALSE;

            // set Interrupt Mode
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;

            // support HostMem
            pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

            // set FctPtr
            //	- dynamic UserPage exist
            pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoardMicrel_Reset;
            pDevExtIn->pFnBoard_ChangePageXhif = NULL;

            // update IcuType
            pDevExtIn->eIcu = ePNDEV_ICU_MICREL;

            break;
        }
        case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
        {
            // success
            bResult = TRUE;

            // support Flash and CopyData
            pDevExtIn->bFlashSupport = TRUE;
            pDevExtIn->bCopyDataSupport = TRUE;

            // set Interrupt Mode
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_INVALID;

//            // support HostMem -> TODO: TI reactivate when pci master window is configured again in fnBoard_ConfigPciMaster()
//            pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

            // set FctPtr
            //	- dynamic UserPage exist
            pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoardAM5728_Reset;
            pDevExtIn->pFnBoard_ChangePageXhif = NULL;

            // update IcuType
            pDevExtIn->eIcu = ePNDEV_ICU_INVALID;

            break;
        }
        #endif

        #ifdef PNDEV_SUPPORT_I210
		case ePNDEV_BOARD_I210:		// I210 board
		{
			// success
			bResult = TRUE;

			// support Flash and CopyData
			pDevExtIn->bFlashSupport 	= FALSE;
			pDevExtIn->bCopyDataSupport = FALSE;
			
            #if defined (PNDEV_OS_WIN)
            // set Interrupt Mode			
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;
            #else
			pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_MSIX;
            #endif
			
			// support HostMem
			pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

			// set FctPtr
			//	- dynamic UserPage exist
			pDevExtIn->pFnBoard_Reset			= (FN_BOARD_XX) fnBoard_I210_Reset;
			pDevExtIn->pFnBoard_ChangePageXhif	= NULL;

			// update IcuType
			pDevExtIn->eIcu = ePNDEV_ICU_INTEL;

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_I210SFP
        case ePNDEV_BOARD_I210SFP:	// I210SFP board
        {                            
            // success
            bResult = TRUE;

            // support Flash and CopyData
            pDevExtIn->bFlashSupport = FALSE;
            pDevExtIn->bCopyDataSupport = FALSE;

            #if defined (PNDEV_OS_WIN)
            // set Interrupt Mode			
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;
            #else
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_MSIX;
            #endif

            // support HostMem
            pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

            // set FctPtr
            //	- dynamic UserPage exist
            pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoard_I210SFP_Reset;
            pDevExtIn->pFnBoard_ChangePageXhif = NULL;

            // update IcuType
            pDevExtIn->eIcu = ePNDEV_ICU_INTEL;

            break;
        }
        #endif

        #ifdef PNDEV_SUPPORT_I82574
        case ePNDEV_BOARD_I82574:	// Intel I82574 board
        {
            // success
            bResult = TRUE;

            // support Flash and CopyData
            pDevExtIn->bFlashSupport = FALSE;
            pDevExtIn->bCopyDataSupport = FALSE;

            #if defined (PNDEV_OS_WIN)
            // set Interrupt Mode			
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_LEG;
            #else
            pDevExtIn->eIntMode = ePNDEV_INTERRUPT_MODE_MSIX;
            #endif

            // support HostMem
            pDevExtIn->eValidHostMemPool = ePNDEV_VAL_HOST_MEM_ALL;

            // set FctPtr
            //	- dynamic UserPage exist
            pDevExtIn->pFnBoard_Reset = (FN_BOARD_XX)fnBoard_I82574_Reset;
            pDevExtIn->pFnBoard_ChangePageXhif = NULL;

            // update IcuType
            pDevExtIn->eIcu = ePNDEV_ICU_INTEL;

            break;
        }
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
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
        #endif

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
            #ifdef PNDEV_SUPPORT_OTHERS
			case ePNDEV_BOARD_CP1616:			// CP1616 board
			case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC400;

				// set AsicDetail
				{
				UINT32 lIrteVersion = 0;

					// get IrteVersion
					lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

					if	(lIrteVersion == IRTE_IP_VERSION__REV5_METAL_FIX)
						// MetalFix
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV5_METAL_FIX;
					}
					else
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV5;
					}
				}

				break;
			}
			case ePNDEV_BOARD_EB200:			// EB200 board
			case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC200;

				// AsicDetail: see fnBoardIrte_SetStaticPagesLbu()

				break;
			}
			case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
			case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
			case ePNDEV_BOARD_CP1625:		// CP1625 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_SOC1;

				// set AsicDetail
				{
				UINT32 lIrteVersion = 0;

					// get IrteVersion
					lIrteVersion = READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION);

					if	(lIrteVersion == IRTE_IP_VERSION__REV7_METAL_FIX)
						// MetalFix
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV7_METAL_FIX;
					}
					else
					{
						pDevExtIn->eAsicDetail = ePNDEV_ASIC_DETAIL_IRTE_REV7;
					}
				}

				break;
			}
			case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
			case ePNDEV_BOARD_EB200P:			// EB200P board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_ERTEC200P;

				// AsicDetail: see fnBoardPnip_SetStaticPagesXhif()

				break;
			}
            case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
            {
                // set AsicType
                pDevExtIn->eAsic = ePNDEV_ASIC_HERA;

                break;
            }
            case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
            {
                // set AsicType
                pDevExtIn->eAsic = ePNDEV_ASIC_KSZ8841;

                break;
            }
            case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
            {
                // set AsicType
                pDevExtIn->eAsic = ePNDEV_ASIC_KSZ8842;

                break;
            }
            case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
            {
                // set AsicType
                pDevExtIn->eAsic = ePNDEV_ASIC_AM5728;

                break;
            }
            #endif

            #ifdef PNDEV_SUPPORT_I210
			case ePNDEV_BOARD_I210:	// I210 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_I210;

				break;
			}
            #endif
            
            #ifdef PNDEV_SUPPORT_I210SFP
			case ePNDEV_BOARD_I210SFP:	// I210SFP board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_I210IS;

				break;
			}
            #endif

            #ifdef PNDEV_SUPPORT_I82574
			case ePNDEV_BOARD_I82574:	// Intel I82574 board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_I82574;

				break;
			}
            #endif

            #ifdef PNDEV_SUPPORT_IX1000
			case ePNDEV_BOARD_IX1000: // Intel Quark X1000 on Galileo board
			{
				// set AsicType
				pDevExtIn->eAsic = ePNDEV_ASIC_IX1000;
				
				break;
			}
            #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
		BOOLEAN						bResultPage			= FALSE;
		UINT32						lOffsetFlashData	= 0;
		_TCHAR						sVersionSbl[PNDEV_SIZE_STRING_BUF_SHORT] = {0};

		uPNDEV_FLASH_PAR            *puFlashPar;
        uPNDEV_FLASH_DATA_CP1616    *puFlashData;
        
            puFlashPar  = (uPNDEV_FLASH_PAR*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_PAR), TAG('D', 'a', 't', 'a'));
            puFlashData = (uPNDEV_FLASH_DATA_CP1616*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_CP1616), TAG('D', 'a', 't', 'a'));
        
            if(!puFlashPar)
            {
                // error
                bResult = FALSE;

                // set ErrorString
                fnBuildString(  _TEXT(__FUNCTION__),
                                _TEXT("(): could not allocate uPNDEV_FLASH_PAR!"),
                                NULL,
                                lDimErrorIn,
                                sErrorOut);
            }
            else
            {
                if(!puFlashData)
                {
                    // error
                    bResult = FALSE;

                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): could not allocate uPNDEV_FLASH_DATA_CP1616!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
                }
                else
                {
                    RtlZeroMemory(	puFlashPar,
						            sizeof(uPNDEV_FLASH_PAR));
                                    
                    RtlZeroMemory(	puFlashData,
						            sizeof(uPNDEV_FLASH_DATA_CP1616));
                
                    //------------------------------------------------------------------------
                    // start using FlashPage
                    bResultPage = fnBoard_StartFlashPage(	pDevExtIn,
                                                            FALSE,				// don't verify chip (AsicCore may read wrong opcode!)
                                                            ePNDEV_FLASH_ACTION_INVALID,
                                                            lDimErrorIn,
                                                            puFlashPar,
                                                            &lOffsetFlashData,
                                                            sErrorOut);
                    {
                        if	(bResultPage)
                            // success
                        {
                            // read FlashData
                            *((uPNDEV_FLASH_DATA_CP1616*) puFlashData) = *((uPNDEV_FLASH_DATA_CP1616*) (puFlashPar->pFlash + lOffsetFlashData));
                        }
                    }
                    //------------------------------------------------------------------------
                    // stop using FlashPage
                    fnBoard_StopFlashPage(pDevExtIn);
        
                    if	(!bResultPage)
                        // error
                    {
                        bResult = FALSE;
                    }
                    else
                    {
                        // get version as string
                        {
                        UINT32 lLenTmp	= 0;
                        UINT32 i		= 0;
        
                            lLenTmp = sizeof(uAM_VERSION_BOOT);
        
                            for	(i = 0; i < lLenTmp; i++)
                                // complete array
                            {
                                sVersionSbl[i] = (_TCHAR) puFlashData->uBootVersion.uByte[i];
                            }
        
                            // add \0
                            sVersionSbl[lLenTmp] = 0;
                        }
        
                        // set BoardInfo
                        _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                            _countof(pHwInfoOut->sBoardInfo),
                                            _TEXT("CP1616 board, Version='%s'"),
                                            sVersionSbl);
        
                        // AsicCore = Arm9
                        pHwInfoOut->bAsicCoreExist = TRUE;
                    }
                    
					fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));
                }
                
				fnFreeMemNonPaged(puFlashPar, TAG('D', 'a', 't', 'a'));
            }

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-EB400-PCIe board"));

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;
			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
		UINT32 lVersionFpgaMajor = 0;
		UINT32 lVersionFpgaMinor = 0;

			// read version of PciBridge-FPGA
			lVersionFpgaMajor	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MAJOR);
			lVersionFpgaMinor	= READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uPciBridgeReg.pPtr + BOARD_EB200_PCI_BRIDGE_REG__FPGA_VERSION_MINOR);

			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("EB200 board (PCI-Bridge-FPGA 0x%02x.0x%02x)"),
								lVersionFpgaMajor,
								lVersionFpgaMinor);

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("EB200 board, FPGA-Version=0x%08x"),
								    pDevExtIn->lBoardRevision);

			// AsicCore = Arm9
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-Soc1-PCI board"));

			// AsicCore = MIPS
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("DB-Soc1-PCIe board"));

			// AsicCore = MIPS
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_CP1625:	// CP1625 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("CP1625 board"));

			// AsicCore = MIPS
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					// set BoardInfo
					_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
										_countof(pHwInfoOut->sBoardInfo),
										_TEXT("FPGA1-Ertec200P board (PCIe-AHB), Toplevel=0x%02x.0x%02x"),
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion);

					// no AsicCore
					pHwInfoOut->bAsicCoreExist = FALSE;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					// set BoardInfo
					_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
										_countof(pHwInfoOut->sBoardInfo),
										_TEXT("FPGA1-Ertec200P board (PCIe-XHIF), Toplevel=0x%02x.0x%02x"),
										pDevExtIn->lBitStreamId,
										pDevExtIn->lBitStreamVersion);

					// AsicCore = Arm926
					pHwInfoOut->bAsicCoreExist = TRUE;

					break;
				}
				default:
				{
					// error
					bResult = FALSE;

					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid BoardDetail!"),
									NULL,
									lDimErrorIn,
									sErrorOut);

					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
        UINT32 lScrbId = 0;

            // get SCRB-ID
			lScrbId = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uApbPer.pPtr + ERTEC200P_APB_PER__OFFSET_SCRB + ERTEC200P_SCRB_REG__ID);

            if	(lScrbId == ERTEC200P_SCRB_ID__REV2_HW2)
				// hw-release Step2
			{
                // set BoardInfo
			    _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								    _countof(pHwInfoOut->sBoardInfo),
								    _TEXT("EB200P-2 board, FPGA-Version=0x%08x"),
								    pDevExtIn->lBoardRevision);
            }
            else
            {
			    // set BoardInfo
			    _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								    _countof(pHwInfoOut->sBoardInfo),
								    _TEXT("EB200P board, FPGA-Version=0x%08x"),
								    pDevExtIn->lBoardRevision);
            }

			// AsicCore = Arm926
			pHwInfoOut->bAsicCoreExist = TRUE;

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
        {

            if (pDevExtIn->lBitStreamPatchLabel == 0x0)
                // IF-A
            {
                // set BoardInfo
                _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                    _countof(pHwInfoOut->sBoardInfo),
                                    _TEXT("FPGA1-Hera board, Inc: 0x%02x, Label: 0x%02x, PNIP IF-A "),
                                    pDevExtIn->lBitStreamId,
                                    pDevExtIn->lBitStreamVersion);
            }
            else if (pDevExtIn->lBitStreamPatchLabel == 0x1)	// IF-B (100 Mbit)
            {
                // set BoardInfo
                _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                    _countof(pHwInfoOut->sBoardInfo),
                                    _TEXT("FPGA1-Hera board, Inc: 0x%02x, Label: 0x%02x, PNIP IF-B (100 Mbit)"),
                                    pDevExtIn->lBitStreamId,
                                    pDevExtIn->lBitStreamVersion);
            }
			else if (pDevExtIn->lBitStreamPatchLabel == 0x2)	// IF-B (1 Gbit)
			{
				// set BoardInfo
				_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
					_countof(pHwInfoOut->sBoardInfo),
					_TEXT("FPGA1-Hera board, Inc: 0x%02x, Label: 0x%02x, PNIP IF-B (1 Gbit)"),
					pDevExtIn->lBitStreamId,
					pDevExtIn->lBitStreamVersion);
			}

            // AsicCore = Arm9
            pHwInfoOut->bAsicCoreExist = TRUE;

            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8841:	// Micrel KSZ8841 board
        {
            // set BoardInfo
            _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                _countof(pHwInfoOut->sBoardInfo),
                                _TEXT("Micrel KSZ8841 board, Version: 0x%04x"),
                                pDevExtIn->lBoardRevision);

            // no AsicCore
            pHwInfoOut->bAsicCoreExist = FALSE;

            break;
        }
        case ePNDEV_BOARD_MICREL_KSZ8842:	// Micrel KSZ8842 board
        {
            // set BoardInfo
            _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                _countof(pHwInfoOut->sBoardInfo),
                                _TEXT("Micrel KSZ8842 board, Version: 0x%04x"),
                                pDevExtIn->lBoardRevision);

            // no AsicCore
            pHwInfoOut->bAsicCoreExist = FALSE;

            break;
        }
        case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
        {
            // set BoardInfo
            _RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
                                _countof(pHwInfoOut->sBoardInfo),
                                _TEXT("TI AM5728 board, Version: 0x%08x "),
                                pDevExtIn->lBitStreamVersion);

            // AsicCore = Cortex
            pHwInfoOut->bAsicCoreExist = TRUE;

            break;
        }
        #endif

        #ifdef PNDEV_SUPPORT_I210
		case ePNDEV_BOARD_I210:	// I210 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("I210 board, Version: 0x%04x (0x1=A1, 0x3=A2)"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_I210SFP
		case ePNDEV_BOARD_I210SFP:	// I210SFP board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("I210SFP board, Version: 0x%04x (0x1=A1, 0x3=A2)"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_I82574
		case ePNDEV_BOARD_I82574:	// Intel I82574 board
		{
			// set BoardInfo
			_RtlStringCchPrintf(pHwInfoOut->sBoardInfo,
								_countof(pHwInfoOut->sBoardInfo),
								_TEXT("I82574 board, Version: 0x%04x"),
								pDevExtIn->lBoardRevision);

			// no AsicCore
			pHwInfoOut->bAsicCoreExist = FALSE;

			break;
		}
        #endif

        #ifdef PNDEV_SUPPORT_IX1000
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
        #endif

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
                    #ifdef PNDEV_SUPPORT_OTHERS
					case ePNDEV_ASIC_ERTEC400:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV5_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_ERTEC200:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV6_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_SOC1:
					{
						if	(pDevExtIn->eAsicDetail == ePNDEV_ASIC_DETAIL_IRTE_REV7_METAL_FIX)
							// MetalFix
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}
						else
						{
							// set AsicInfo
							//	- IP_Version
							_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
												_countof(pHwInfoOut->sAsicInfo),
												_TEXT("IRTE Version=0x%04x (no MetalFix)"),
												READ_REG_UINT32(pDevExtIn->pIrte + IRTE_REG__IP_VERSION));
						}

						break;
					}
					case ePNDEV_ASIC_ERTEC200P:
					{
						switch	(pDevExtIn->eAsicDetail)
								// AsicDetail
						{
							case ePNDEV_ASIC_DETAIL_PNIP_REV1:
							{
								// set AsicInfo
								//	- IP_Version.IP_Development
								_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
													_countof(pHwInfoOut->sAsicInfo),
													_TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.1)"),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

								break;
							}
							case ePNDEV_ASIC_DETAIL_PNIP_REV2:
							{
								// set AsicInfo
								//	- IP_Version.IP_Development
								_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
													_countof(pHwInfoOut->sAsicInfo),
													_TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.2)"),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
													READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

								break;
							}
							default:
							{
								// error
								bResult = FALSE;

								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid AsicVersion!"),
												NULL,
												lDimErrorIn,
												sErrorOut);

								break;
							}
						}

						break;
					}
                    case ePNDEV_ASIC_HERA:
                    {
                        // set AsicInfo
                        //	- IP_Version.IP_Development
                        _RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
                                            _countof(pHwInfoOut->sAsicInfo),
                                            _TEXT("PNIP Version=0x%04x Development=0x%04x (Rev.3)"),
                                            READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_VERSION),
                                            READ_REG_UINT32(pDevExtIn->pPnIp + PNIP_REG__IP_DEVELOPMENT));

                        break;
                    }
                    case ePNDEV_ASIC_KSZ8841:
                    case ePNDEV_ASIC_KSZ8842:
                    {
                        // no AsicInformation
                        break;
                    }
                    case ePNDEV_ASIC_AM5728:
                    {
                        // no AsicInformation
                        // ToDo: investigate AsicVersion
                        break;
                    }
                    #endif

                    #ifdef PNDEV_SUPPORT_I210
					case ePNDEV_ASIC_I210:					
					{
					UINT32	lBootImageVersion 	= 0;
					UINT32	lTmpValue			= 0;

						// write address (BootImage Id is located at offset 0x5 at eeprom)
						WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD, (0x5<<2));

						do
						{
							// get register value
							lTmpValue = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD);
						}
								// read operation not done
						while	(((lTmpValue >> BOARD_I210__EERD_DONE) & 1u) != BOARD_I210__EERD_DONE);

						// read data (bit 31:16)
						lBootImageVersion = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210__EERD)>>16u);

						// set AsicInfo
						_RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
											_countof(pHwInfoOut->sAsicInfo),
											_TEXT("RevisionId BootImageVersion=0x%04x"),
											lBootImageVersion);

						break;
					}
                    #endif

                    #ifdef PNDEV_SUPPORT_I210SFP
                    case ePNDEV_ASIC_I210IS:
                    {
                        UINT32	lBootImageVersion = 0;
                        UINT32	lTmpValue = 0;

                        // write address (BootImage Id is located at offset 0x5 at eeprom)
                        WRITE_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EERD, (0x5 << 2));

                        do
                        {
                            // get register value
                            lTmpValue = READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EERD);
                        }
                        // read operation not done
                        while (((lTmpValue >> BOARD_I210SFP__EERD_DONE) & 1) != BOARD_I210SFP__EERD_DONE);

                        // read data (bit 31:16)
                        lBootImageVersion = (READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uI210.uBar0.pPtr + BOARD_I210SFP__EERD) >> 16u);

                        // set AsicInfo
                        _RtlStringCchPrintf(pHwInfoOut->sAsicInfo,
                                            _countof(pHwInfoOut->sAsicInfo),
                                            _TEXT("RevisionId BootImageVersion=0x%04x"),
                                            lBootImageVersion);

                        break;
                    }
                    #endif

                    #ifdef PNDEV_SUPPORT_I82574
					case ePNDEV_ASIC_I82574:
					{
						// set AsicInfo - read of boot image version not supported on I82574 (possible position not in spec)
						break;
					}
                    #endif

                    #ifdef PNDEV_SUPPORT_IX1000
					case ePNDEV_ASIC_IX1000:
					{
						// no AsicInformation
						break;
					}
                    #endif

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

#ifdef PNDEV_SUPPORT_OTHERS
BOOLEAN				bResultPage			= FALSE;
UINT32				lOffsetFlashData	= 0;
uPNDEV_FLASH_PAR*	puFlashPar         = 0;

    puFlashPar = (uPNDEV_FLASH_PAR*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_PAR), TAG('D', 'a', 't', 'a'));

#endif    

	// preset OutputParameter
	RtlZeroMemory(	&pDevExtIn->uRemanentData,
					sizeof(uPNCORE_REMANENT_DATA));

    if (0)
    {
        // Dummy for preprocessor
    }

    #ifdef PNDEV_SUPPORT_I210
	else if	(pDevExtIn->eBoard == ePNDEV_BOARD_I210)
		//	Springville
	{
		if	(fnBoard_I210_GetMacAdress(	pDevExtIn,
										lDimErrorIn,
										sErrorOut))
			// success
		{
			bResult = TRUE;			
		}
	}
    #endif

    #ifdef PNDEV_SUPPORT_I210SFP
    else if (pDevExtIn->eBoard == ePNDEV_BOARD_I210SFP)
        //	Springville SFP
    {
        if (fnBoard_I210SFP_GetMacAdress(   pDevExtIn,
                                            lDimErrorIn,
                                            sErrorOut))
            // success
        {
            bResult = TRUE;
        }
    }
    #endif

    #ifdef PNDEV_SUPPORT_I82574
    else if (pDevExtIn->eBoard == ePNDEV_BOARD_I82574)
        //	Hartwell
    {
        if (fnBoard_I82574_GetMacAdress(pDevExtIn,
                                        lDimErrorIn,
                                        sErrorOut))
            // success
        {
            bResult = TRUE;
        }
    }
    #endif

    #ifdef PNDEV_SUPPORT_IX1000
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
    #endif

    #ifdef PNDEV_SUPPORT_OTHERS
    else if ((pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8841)
            || (pDevExtIn->eBoard == ePNDEV_BOARD_MICREL_KSZ8842))
        //	MicrelBoard
    {
        if (fnBoardMicrel_GetMacAdress(pDevExtIn,
            lDimErrorIn,
            sErrorOut))
            // success
        {
            bResult = TRUE;
        }
    }
	else
	{
	    if(!puFlashPar)
	    {
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): could not allocate uPNDEV_FLASH_PAR!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
	    }
	    else
	    {	
            RtlZeroMemory(	puFlashPar,
			                sizeof(uPNDEV_FLASH_PAR));
            
            // Note:
            //	- if there are flash problems OpenDevice cannot solve them
            //		-> returned error is only used for debug purpose
        
            //------------------------------------------------------------------------
            // start using FlashPage
            bResultPage = fnBoard_StartFlashPage(	pDevExtIn,
                                                    FALSE,				// don't verify chip (AsicCore may read wrong opcode!)
                                                    ePNDEV_FLASH_ACTION_INVALID,
                                                    lDimErrorIn,
                                                    puFlashPar,
                                                    &lOffsetFlashData,
                                                    sErrorOut);
            {
                if	(bResultPage)
                    // success
                {
                UINT32 lCtrMacAdrMin = 0;
        
                    switch	(pDevExtIn->eAsic)
                            // AsicType
                    {
                        case ePNDEV_ASIC_ERTEC400:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC400;	break;}
                        case ePNDEV_ASIC_ERTEC200:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200;	break;}
                        case ePNDEV_ASIC_SOC1:		{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__SOC1;		break;}
                        case ePNDEV_ASIC_ERTEC200P:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__ERTEC200P;	break;}
                        case ePNDEV_ASIC_HERA:	    {lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__HERA;	    break;}
                        case ePNDEV_ASIC_AM5728:	{lCtrMacAdrMin = PNDEV_MAC_ADR_MIN__AM5728;	    break;}
                        default:
                        {
                            // set ErrorString
                            fnBuildString(	_TEXT(__FUNCTION__),
                                            _TEXT("(): Invalid AsicType!"),
                                            NULL,
                                            lDimErrorIn,
                                            sErrorOut);
        
                            break;
                        }
                    }
        
                    if	(lCtrMacAdrMin != 0)
                        // success
                    {
                    UINT64 lMacAdr64_Tmp = 0;
        
                        switch	(puFlashPar->eFlashTypePos[0])
                                // FlashType
                                //	- eFlashTypeReal not available, see above
                        {
                            case ePNDEV_FLASH_TYPE_NOR_AM29LV040B:
                            case ePNDEV_FLASH_TYPE_NOR_AM29LV641:
                            case ePNDEV_FLASH_TYPE_NOR_S29GL064N:
                            case ePNDEV_FLASH_TYPE_NOR_S29GL512P:
                            case ePNDEV_FLASH_TYPE_NOR_S29WS128P:
                            case ePNDEV_FLASH_TYPE_NOR_S29AL032D:	
                            case ePNDEV_FLASH_TYPE_NOR_STM29W320EB:
                            case ePNDEV_FLASH_TYPE_NOR_MX29LV320CB:
                            {
                                if	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
                                    // CP1616 board
                                {
                                uPNDEV_FLASH_DATA_CP1616 *puFlashData;
                                                                    
									puFlashData = (uPNDEV_FLASH_DATA_CP1616*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_CP1616), TAG('D', 'a', 't', 'a'));
                                
                                    if(!puFlashData)
                                    {
                                        // set ErrorString
                                        fnBuildString(  _TEXT(__FUNCTION__),
                                                        _TEXT("(): could not allocate uPNDEV_FLASH_DATA_CP1616!"),
                                                        NULL,
                                                        lDimErrorIn,
                                                        sErrorOut);
                                    }
                                    else
                                    {    
                                        RtlZeroMemory(puFlashData, sizeof(uPNDEV_FLASH_DATA_CP1616));
                                        
                                        // success
                                        bResult = TRUE;
            
                                        // Note:
                                        //	- IA AS:	at least 5 MacAdr are necessary (4x PortMacAdr, 1x IfaMacAdr)
                                        //	- IA SC IC: at least 6 MacAdr are necessary (4x PortMacAdr, 2x IfaMacAdr)
                                        //	- lCtrMacAdr=0/0xff: 2 MacAdr are stored -> not useful -> treat it as an empty flash without FlashData
            
                                        // read FlashData
                                        *((uPNDEV_FLASH_DATA_CP1616*) puFlashData) = *((uPNDEV_FLASH_DATA_CP1616*) (puFlashPar->pFlash + lOffsetFlashData));
            
                                        if	(	(puFlashData->lCtrMacAdr < PNDEV_MAC_ADR_MIN__ERTEC400)
                                            ||	(puFlashData->lCtrMacAdr > PNDEV_MAC_ADR_MAX))
                                            // invalid number of MacAdr
                                        {
                                            // treat it as empty flash without FlashData
                                        }
                                        else
                                        {
                                            // return
                                            {
                                                pDevExtIn->uRemanentData.lCtrMacAdr = puFlashData->lCtrMacAdr;
            
                                                RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
                                                                (void*) (puFlashData->lArrayPrivate),
                                                                PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
                                            }
            
                                            // compute MacAdr64 = base of all other MacAdr
                                            {
                                            UINT8 i = 0;
            
                                                // example:
                                                //	- MacAdr = aa-bb-cc-dd-ee-ff = 0x0000aabbccddeeff
                                                //														  offset:  0  1  2  3  4  5
                                                //	- LittleEndian: 0x0000aabbccddeeff has following memory dump: ff ee dd cc bb aa 00 00
                                                //		-> offset 5 = MacAdr[0]
                                                //		-> offset 4 = MacAdr[1]
                                                //		-> offset 3 = MacAdr[2]
                                                //		-> offset 2 = MacAdr[3]
                                                //		-> offset 1 = MacAdr[4]
                                                //		-> offset 0 = MacAdr[5]
            
                                                for	(i = 0; i < 6; i++)
                                                {
                                                    *(((volatile UINT8*) &lMacAdr64_Tmp) + (5-i)) = puFlashData->uMacAdr0[i];
                                                }
                                            }
                                        }
                                        
										fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));
                                    }
                                }
                                else
                                {
                                ePNDEV_FLASH_VERSION eVersion = ePNDEV_FLASH_VERSION_INVALID;
        
                                    // get version
                                    eVersion = *((ePNDEV_FLASH_VERSION*) (puFlashPar->pFlash + lOffsetFlashData));
        
                                    switch	(eVersion)
                                            // version
                                    {
                                        case ePNDEV_FLASH_VERSION_V0001:	// version V1
                                        {
                                        uPNDEV_FLASH_DATA_V1 *puFlashData;
                                                                                    
											puFlashData = (uPNDEV_FLASH_DATA_V1*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_V1), TAG('D', 'a', 't', 'a'));
                                        
                                            if(!puFlashData)
                                            {
                                                // set ErrorString
                                                fnBuildString(  _TEXT(__FUNCTION__),
                                                                _TEXT("(): could not allocate uPNDEV_FLASH_DATA_V1!"),
                                                                NULL,
                                                                lDimErrorIn,
                                                                sErrorOut);
                                            }
                                            else
                                            {        
                                                RtlZeroMemory(puFlashData, sizeof(uPNDEV_FLASH_DATA_V1));
                                            
                                                // read FlashData
                                                *((uPNDEV_FLASH_DATA_V1*) puFlashData) = *((uPNDEV_FLASH_DATA_V1*) (puFlashPar->pFlash + lOffsetFlashData));
            
                                                if	(	(puFlashData->lCtrMacAdr < lCtrMacAdrMin)
                                                    ||	(puFlashData->lCtrMacAdr > PNDEV_MAC_ADR_MAX))
                                                    // invalid number of MacAdr
                                                {
                                                    // set ErrorString
                                                    fnBuildString(	_TEXT(__FUNCTION__),
                                                                    _TEXT("(): Invalid number of MacAdr!"),
                                                                    NULL,
                                                                    lDimErrorIn,
                                                                    sErrorOut);
                                                }
                                                else
                                                {
                                                    // success
                                                    bResult = TRUE;
            
                                                    // return
                                                    {
                                                        pDevExtIn->uRemanentData.lCtrMacAdr = puFlashData->lCtrMacAdr;
            
                                                        RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
                                                                        (void*) (puFlashData->lArrayPrivate),
                                                                        PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
                                                    }
            
                                                    // stored MacAdr = base of all other MacAdr
                                                    lMacAdr64_Tmp = puFlashData->lMacAdr64;
                                                }
                                                
												fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));                                                
                                            }
        
                                            break;
                                        }
                                        case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
                                        {
                                            // success
                                            bResult = TRUE;
        
                                            break;
                                        }
                                        default:
                                        {
                                            // Note:
                                            //	- don't return an error but treat it as an empty flash without FlashData
                                            //	- possible error conditions:
                                            //		flash not accessible
                                            //		invalid pattern at flash
        
                                            break;
                                        }
                                    }
                                }
        
                                break;
                            }
                            case ePNDEV_FLASH_TYPE_NAND_K91G08U0B:
							case ePNDEV_FLASH_TYPE_NAND_TC58NVG0S3E:
                            {
                            ePNDEV_FLASH_VERSION eVersion = ePNDEV_FLASH_VERSION_INVALID;
        
                                // get version
                                bResult = fnFlashNand_Read(	pDevExtIn,
                                                            puFlashPar,
                                                            lOffsetFlashData,
                                                            sizeof(ePNDEV_FLASH_VERSION),
                                                            TRUE,							// EncodedRsc
                                                            lDimErrorIn,
                                                            (UINT8*) &eVersion,
                                                            sErrorOut);
        
                                if	(bResult)
                                    // success
                                {
                                    switch	(eVersion)
                                            // eVersion
                                    {
                                        case ePNDEV_FLASH_VERSION_V0001:	// version V1
                                        {
                                        uPNDEV_FLASH_DATA_V1 *puFlashData;
                                                                                    
											puFlashData = (uPNDEV_FLASH_DATA_V1*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_V1), TAG('D', 'a', 't', 'a'));
                                        
                                            if(!puFlashData)
                                            {
                                                // set ErrorString
                                                fnBuildString(  _TEXT(__FUNCTION__),
                                                                _TEXT("(): could not allocate uPNDEV_FLASH_DATA_V1!"),
                                                                NULL,
                                                                lDimErrorIn,
                                                                sErrorOut);
                                            }
                                            else
                                            {
                                                RtlZeroMemory(puFlashData, sizeof(uPNDEV_FLASH_DATA_V1));
                                            
                                                // read FlashData
                                                bResult = fnFlashNand_Read(	pDevExtIn,
                                                                            puFlashPar,
                                                                            lOffsetFlashData,
                                                                            sizeof(uPNDEV_FLASH_DATA_V1),
                                                                            TRUE,					// EncodedRsc
                                                                            lDimErrorIn,
                                                                            (UINT8*) puFlashData,
                                                                            sErrorOut);
            
                                                if	(	(puFlashData->lCtrMacAdr < lCtrMacAdrMin)
                                                    ||	(puFlashData->lCtrMacAdr > PNCORE_MAC_ADR_MAX))
                                                    // invalid number of MacAdr
                                                {
                                                    // set ErrorString
                                                    fnBuildString(	_TEXT(__FUNCTION__),
                                                                    _TEXT("(): Invalid number of MacAdr!"),
                                                                    NULL,
                                                                    lDimErrorIn,
                                                                    sErrorOut);
                                                }
                                                else
                                                {
                                                    // success
                                                    bResult = TRUE;
            
                                                    // return
                                                    {
                                                        pDevExtIn->uRemanentData.lCtrMacAdr = puFlashData->lCtrMacAdr;
            
                                                        RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
                                                                        (void*) (puFlashData->lArrayPrivate),
                                                                        PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
                                                    }
            
                                                    // stored MacAdr = base of all other MacAdr
                                                    lMacAdr64_Tmp = puFlashData->lMacAdr64;
                                                }
                                                
												fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));
                                            }
                                            break;
                                        }
                                        case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
                                        {
                                            // success
                                            bResult = TRUE;
        
                                            break;
                                        }
                                        default:
                                        {
                                            // Note:
                                            //	- don't return an error but treat it as an empty flash without FlashData
                                            //	- possible error conditions:
                                            //		flash not accessible
                                            //		invalid pattern at flash
                                            break;
                                        }
                                    }
                                    break;
                                }
        
                                break;
        
                            }
                            case ePNDEV_FLASH_TYPE_SPI_N25Q128A:
                            case ePNDEV_FLASH_TYPE_SPI_A25LQ032:
                            {
                            uPNDEV_FLASH_DATA_V1 *puFlashData;
                                                            
								puFlashData = (uPNDEV_FLASH_DATA_V1*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_V1), TAG('D', 'a', 't', 'a'));
                            
                                if(!puFlashData)
                                {
                                    // set ErrorString
                                    fnBuildString(  _TEXT(__FUNCTION__),
                                                    _TEXT("(): could not allocate uPNDEV_FLASH_DATA_V1!"),
                                                    NULL,
                                                    lDimErrorIn,
                                                    sErrorOut);
                                }
                                else
                                {         
                                    RtlZeroMemory(puFlashData, sizeof(uPNDEV_FLASH_DATA_V1));
                                                   
                                    bResult = fnFlashSPI_Read(	pDevExtIn,
                                                                puFlashPar,
                                                                lOffsetFlashData,
                                                                sizeof(uPNDEV_FLASH_DATA_V1),
                                                                FALSE,
                                                                lDimErrorIn,
                                                                (UINT8*) puFlashData,
                                                                sErrorOut);
            
                                    if	(bResult)
                                        // success
                                    {
                                        switch	(puFlashData->eVersion)
                                                // eVersion
                                        {
                                            case ePNDEV_FLASH_VERSION_V0001:	// version V1
                                            {
                                                if	(	(puFlashData->lCtrMacAdr < lCtrMacAdrMin)
                                                    ||	(puFlashData->lCtrMacAdr > PNCORE_MAC_ADR_MAX))
                                                    // invalid number of MacAdr
                                                {
                                                    // set ErrorString
                                                    fnBuildString(	_TEXT(__FUNCTION__),
                                                                    _TEXT("(): Invalid number of MacAdr!"),
                                                                    NULL,
                                                                    lDimErrorIn,
                                                                    sErrorOut);
                                                }
                                                else
                                                {
                                                    // success
                                                    bResult = TRUE;
            
                                                    // return
                                                    {
                                                        pDevExtIn->uRemanentData.lCtrMacAdr = puFlashData->lCtrMacAdr;
            
                                                        RtlCopyMemory(	(void*) (pDevExtIn->uRemanentData.lArrayPrivate),
                                                                        (void*) (puFlashData->lArrayPrivate),
                                                                        PNCORE_DIM_ARRAY_REMANENT_PRIVATE);
                                                    }
            
                                                    // stored MacAdr = base of all other MacAdr
                                                    lMacAdr64_Tmp = puFlashData->lMacAdr64;
                                                }
                                                break;
                                            }
                                            case ePNDEV_FLASH_VERSION_EMPTY:	// empty flash
                                            {
                                                // success
                                                bResult = TRUE;
            
                                                break;
                                            }
                                            default:
                                            {
                                                // Note:
                                                //	- don't return an error but treat it as an empty flash without FlashData
                                                //	- possible error conditions:
                                                //		flash not accessible
                                                //		invalid pattern at flash
                                                break;
                                            }
                                        }
                                    }
                                    
									fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));
                                }
        
                                break;
                            }
							case ePNDEV_FLASH_TYPE_SPI_M25P10A:
                            {
                            uPNDEV_FLASH_DATA_CP1625 *puFlashData;							
                                                            
								puFlashData = (uPNDEV_FLASH_DATA_CP1625*)fnAllocMemNonPaged(sizeof(uPNDEV_FLASH_DATA_CP1625), TAG('D', 'a', 't', 'a'));
                            
                                if(!puFlashData)
                                {
                                    // set ErrorString
                                    fnBuildString(  _TEXT(__FUNCTION__),
                                                    _TEXT("(): could not allocate uPNDEV_FLASH_DATA_CP1625!"),
                                                    NULL,
                                                    lDimErrorIn,
                                                    sErrorOut);
                                }
                                else
                                {
									RtlZeroMemory(puFlashData, sizeof(uPNDEV_FLASH_DATA_CP1625));
                                                   
                                    bResult = fnFlashSPI_Read(	pDevExtIn,
                                                                puFlashPar,
                                                                lOffsetFlashData,
                                                                sizeof(uPNDEV_FLASH_DATA_CP1625),
                                                                FALSE,
                                                                lDimErrorIn,
                                                                (UINT8*) puFlashData,
                                                                sErrorOut);
            
                                    if	(bResult)
                                        // success
                                    {
										// ----------------------------------------------
										// Trace of all the informations given from SPI - just kernel trace, maybe this trace could help with future issues

										char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = { 0 };
										char    sSerialNumber[19] = { 0 };
										char    sMlfb[21] = { 0 };
										UINT32  lLoopCnt = 0;
										UINT32  lCRC32Given								= 0;
										UINT16  lBlockDataVersionGiven					= 0;
										UINT16  lLengthOfBlockGiven						= 0;
										UINT16  lFunctionalStateGiven					= 0;

										lCRC32Given				= *((UINT32*)puFlashData->uCrc32);
										lBlockDataVersionGiven	= *((UINT16*)puFlashData->uBlockDataVersion);
										lLengthOfBlockGiven		= *((UINT16*)puFlashData->uLengthOfBlock);
										lFunctionalStateGiven	= *((UINT16*)puFlashData->uFunctionalState);

										// set TraceDetail
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[%s]: ++++ CP1625 SPI Info - CRC32(0x%x) BlockLen(0x%x) BlockDataVersion(0x%x) FunctState(0x%x)",
															pDevExtIn->sPciLocShortAscii,
															lCRC32Given,
															lLengthOfBlockGiven,
															lBlockDataVersionGiven,
															lFunctionalStateGiven);

										// set trace
										fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
													ePNDEV_TRACE_CONTEXT_BASIC,
													TRUE,						// logging
													sTraceDetail);

										for (lLoopCnt = 0; lLoopCnt < 18; lLoopCnt++)
											sSerialNumber[lLoopCnt] = puFlashData->uSerialNumber[lLoopCnt];

										for (lLoopCnt = 0; lLoopCnt < 20; lLoopCnt++)
											sMlfb[lLoopCnt] = puFlashData->uMlfb[lLoopCnt];

										// set TraceDetail
										RtlStringCchPrintfA(sTraceDetail,
															_countof(sTraceDetail),
															"PnDev[%s]: ++++ CP1625 SPI Info - SerialNumber(%s) MLFB(%s)",
															pDevExtIn->sPciLocShortAscii,
															sSerialNumber,
															sMlfb);

										// set trace
										fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
													ePNDEV_TRACE_CONTEXT_BASIC,
													TRUE,						// logging
													sTraceDetail);

										// ----------------------------------------------
										// checking of the block data version and the CRC32 --> consistent data?

										if (lBlockDataVersionGiven != 0x1)
											// we can only support version 1 for now, if the version changes, it has to be checked if some fields changed
										{
											// set ErrorString
											fnBuildString(	_TEXT(__FUNCTION__),
															_TEXT("(): Invalid BlockDataVersion for CP1625 detected!"),
															NULL,
															lDimErrorIn,
															sErrorOut);

											bResult = FALSE;
										}
										else
											// calculate the CRC32 to ensure that the data on the flash is not corrupted
										{
										UINT32 lCRC32Calculated = 0;

											lCRC32Calculated = fnCalculateCRC32((UINT8*)puFlashData->uBlockDataVersion, lLengthOfBlockGiven);

											if (lCRC32Given != lCRC32Calculated)
												// the CRC32 is not correct
											{
												// set ErrorString
												fnBuildString(	_TEXT(__FUNCTION__),
																_TEXT("(): Corrupt data for CP1625 detected! CRC32 value not correct"),
																NULL,
																lDimErrorIn,
																sErrorOut);

												bResult = FALSE;
											}
											else
											{
												// ----------------------------------------------
										// process the MAC addresses

										// success
										bResult = TRUE;

										// there is no counter, so we set it to 4 MACs
										pDevExtIn->uRemanentData.lCtrMacAdr = 4;

										// compute MacAdr64 = base of all other MacAdr
										{
											UINT8 i = 0;

											// example:
											//	- MacAdr = aa-bb-cc-dd-ee-ff = 0x0000aabbccddeeff
											//														  offset:  0  1  2  3  4  5
											//	- LittleEndian: 0x0000aabbccddeeff has following memory dump: ff ee dd cc bb aa 00 00
											//		-> offset 5 = MacAdr[0]
											//		-> offset 4 = MacAdr[1]
											//		-> offset 3 = MacAdr[2]
											//		-> offset 2 = MacAdr[3]
											//		-> offset 1 = MacAdr[4]
											//		-> offset 0 = MacAdr[5]

											for (i = 0; i < 6; i++)
											{
												*(((volatile UINT8*)&lMacAdr64_Tmp) + (5 - i)) = puFlashData->uMacAdr1[i];
											}

											// convert MacAdr1 from UINT64 to array
											fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp),
																	&pDevExtIn->uRemanentData.lArrayMacAdr[0][0]);

											for (i = 0; i < 6; i++)
											{
												*(((volatile UINT8*)&lMacAdr64_Tmp) + (5 - i)) = puFlashData->uMacAdr2[i];
											}

											// convert MacAdr2 from UINT64 to array
											fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp),
																	&pDevExtIn->uRemanentData.lArrayMacAdr[1][0]);

											for (i = 0; i < 6; i++)
											{
												*(((volatile UINT8*)&lMacAdr64_Tmp) + (5 - i)) = puFlashData->uMacAdr3[i];
											}

											// convert MacAdr3 from UINT64 to array
											fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp),
																	&pDevExtIn->uRemanentData.lArrayMacAdr[2][0]);

											for (i = 0; i < 6; i++)
											{
												*(((volatile UINT8*)&lMacAdr64_Tmp) + (5 - i)) = puFlashData->uMacAdr4[i];
											}

											// convert MacAdr4 from UINT64 to array
											fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp),
																	&pDevExtIn->uRemanentData.lArrayMacAdr[3][0]);
										}
                                    }
										}
                                    }
                                    
									fnFreeMemNonPaged(puFlashData, TAG('D', 'a', 't', 'a'));
                                }
        
                                break;
                            }
                            default:
                            {
                                // set ErrorString
                                fnBuildString(	_TEXT(__FUNCTION__),
                                                _TEXT("(): Invalid FlashType!"),
                                                NULL,
                                                lDimErrorIn,
                                                sErrorOut);
        
                                break;
                            }
                        }
        
                        if	(pDevExtIn->uRemanentData.lCtrMacAdr != 0)
                            // MacAdr exist
                        {
                        UINT32 i = 0;

							if (pDevExtIn->eBoard == ePNDEV_BOARD_CP1625)
							{
								// do nothing, it is done above
							}
							else
								// all other boards
							{
								for	(i = 0; i < pDevExtIn->uRemanentData.lCtrMacAdr; i++)
									// all MacAdr (based on stored MacAdr)
								{
									// convert MacAdr from UINT64 to array
									fnBoard_ConvertMacAdr(	(lMacAdr64_Tmp + i),
															&pDevExtIn->uRemanentData.lArrayMacAdr[i][0]);
								}
							}
                        }
                    }
                }
            }
            //------------------------------------------------------------------------
            // stop using FlashPage
            fnBoard_StopFlashPage(pDevExtIn);
	    }
    }
    fnFreeMemNonPaged(puFlashPar, TAG('D', 'a', 't', 'a'));   
    #endif
    
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
#ifdef PNDEV_SUPPORT_OTHERS
BOOLEAN fnBoard_StartFlashPage(	DEVICE_EXTENSION*			pDevExtIn,
								const BOOLEAN				bVerifyChipIn,
								const ePNDEV_FLASH_ACTION	eFlashActionIn,
								const UINT32				lDimErrorIn,
								uPNDEV_FLASH_PAR*			pFlashParOut,
								UINT32*						pOffsetFlashDataOut,
								_TCHAR*						sErrorOut)
{
BOOLEAN bResult = FALSE;

	// preset OutputParameter
	{
		RtlZeroMemory(	pFlashParOut,
						sizeof(uPNDEV_FLASH_PAR));

		*pOffsetFlashDataOut = 0;
	}

	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_AM29LV641;
			pFlashParOut->eFlashTypePos[1]	= ePNDEV_FLASH_TYPE_NOR_S29GL064N;
			pFlashParOut->eFlashInfo		= (ePNDEV_FLASH_INFO_SBL_EXIST + ePNDEV_FLASH_INFO_BOARD_DATA_EXIST);
			pFlashParOut->lSizeUsable		= BOARD_CP1616__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_CP1616__FLASH_SIZE_SECTOR;

			*pOffsetFlashDataOut = BOARD_CP1616__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifAsyncBank0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL064N;
			pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
			pFlashParOut->lSizeUsable		= BOARD_EB400__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_EB400__FLASH_SIZE_SECTOR;

			*pOffsetFlashDataOut = BOARD_EB400__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_EB200:	// EB200 board
		{
			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- 8Bit-flash, page0
			fnBoardIrte_StartDriverPage(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0,
											0,
											FALSE,
											__FUNCTION__);
			{
				pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr;
				pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X8_AS_X8;
				pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_AM29LV040B;
				pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
				pFlashParOut->lSizeUsable		= BOARD_EB200__FLASH_SIZE_USABLE;
				pFlashParOut->lSizeSector		= BOARD_EB200__FLASH_SIZE_SECTOR;

				*pOffsetFlashDataOut = BOARD_EB200__FLASH_OFFSET_DATA;
			}

			break;
		}
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
		BOOLEAN bNoAutoalignment = FALSE;
		
			if (	(eFlashActionIn == ePNDEV_FLASH_ACTION_PROGRAM_DATA)
				|| 	(eFlashActionIn == ePNDEV_FLASH_ACTION_PROGRAM_SBL))
				// set Autoaligment for 16bit access
			{
				bNoAutoalignment = TRUE;
			}
		
			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- 16Bit-flash, page0
			fnBoardIrte_StartDriverPage(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_ASYNC_BANK0,
											0,
											bNoAutoalignment,
											__FUNCTION__);
			{
				pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifAsyncBank0.pPtr;
				pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X16;
				pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29AL032D;
				pFlashParOut->eFlashTypePos[1]	= ePNDEV_FLASH_TYPE_NOR_STM29W320EB;
				pFlashParOut->eFlashTypePos[2]	= ePNDEV_FLASH_TYPE_NOR_MX29LV320CB;
				pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
				pFlashParOut->lSizeUsable		= BOARD_EB200_PCIE__FLASH_SIZE_USABLE;
				pFlashParOut->lSizeSector		= BOARD_EB200_PCIE__FLASH_SIZE_SECTOR;

				*pOffsetFlashDataOut = BOARD_EB200_PCIE__FLASH_OFFSET_DATA;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uLocalBus0.pPtr;
			pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X8_AS_X16;
			pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NAND_K91G08U0B;
			pFlashParOut->eFlashTypePos[1]	= ePNDEV_FLASH_TYPE_NAND_TC58NVG0S3E;
			pFlashParOut->eFlashInfo		= ePNDEV_FLASH_INFO_SBL_EXIST;
			pFlashParOut->lSizeUsable		= BOARD_SOC1_DEV__FLASH_SIZE_USABLE;
			pFlashParOut->lSizeSector		= BOARD_SOC1_DEV__FLASH_SIZE_SECTOR;

			pFlashParOut->uNand.lSizePage		= BOARD_SOC1_DEV__FLASH_SIZE_PAGE;
			pFlashParOut->uNand.lSizeRscData	= BOARD_SOC1_DEV__FLASH_SIZE_RSC_DATA;
			pFlashParOut->uNand.lSizeRscCheck	= BOARD_SOC1_DEV__FLASH_SIZE_RSC_CHECK;
			pFlashParOut->uNand.pPollReg		= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_GPIO + SOC1_GPIO_REG__IN5;
			pFlashParOut->uNand.lOffsetDataReg	= BOARD_SOC1_DEV__FLASH_OFFSET_DATA_REG;
			pFlashParOut->uNand.lOffsetCmdReg	= pFlashParOut->uNand.lOffsetDataReg + BOARD_SOC1_DEV__FLASH_OFFSET_CMD_REG;
			pFlashParOut->uNand.lOffsetAdrReg	= pFlashParOut->uNand.lOffsetDataReg + BOARD_SOC1_DEV__FLASH_OFFSET_ADR_REG;

			*pOffsetFlashDataOut = BOARD_SOC1_DEV__FLASH_OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardId)
					// BoardId
			{
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_STD:
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_STD:
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC AsyncBank1
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1,
													0,
													__FUNCTION__);
					{
						pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank1.pPtr;
						pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
						pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL512P;
						pFlashParOut->lSizeUsable		= BOARD_FPGA1__FLASH_SIZE_USABLE;
						pFlashParOut->lSizeSector		= BOARD_FPGA1__FLASH_SIZE_SECTOR;

						*pOffsetFlashDataOut = BOARD_FPGA1__FLASH_OFFSET_DATA;
					}

					break;
				}
				case eBOARD_ID_FPGA1_ERTEC200P_AHB_ET200:
				case eBOARD_ID_FPGA1_ERTEC200P_XHIF_ET200:
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC AsyncBank0
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0,
													0,
													__FUNCTION__);
					{
						pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr;
						pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
						pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29GL512P;
						pFlashParOut->lSizeUsable		= BOARD_FPGA1__FLASH_SIZE_USABLE;
						pFlashParOut->lSizeSector		= BOARD_FPGA1__FLASH_SIZE_SECTOR;

						*pOffsetFlashDataOut = BOARD_FPGA1__FLASH_OFFSET_DATA;
					}

					break;
				}
				default:
				{
					// error
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: using dynamic DriverPage]:
			//	- EMC AsyncBank1
			fnBoardPnip_StartDriverPageXhif(pDevExtIn,
//											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK1,		// PageMode flash
											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_ASYNC_BANK0,		// BurstMode flash
											15,
											__FUNCTION__);
			{
					// 2x S29WS128P -> 2x 16MByte = 32MByte

				pFlashParOut->pFlash			= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcAsyncBank0.pPtr;
				pFlashParOut->eFlashMode		= ePNDEV_FLASH_MODE_X16_AS_X32;
				pFlashParOut->eFlashTypePos[0]	= ePNDEV_FLASH_TYPE_NOR_S29WS128P;
				pFlashParOut->lSizeUsable		= BOARD_EB200P__FLASH_SIZE_USABLE;
				pFlashParOut->lSizeSector		= BOARD_EB200P__FLASH_SIZE_SECTOR;

				*pOffsetFlashDataOut = BOARD_EB200P__FLASH_OFFSET_DATA;
			}

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:
        {
			// MARKED FOR DELETE
        	if	(pDevExtIn->lBitStreamId == 0x09)
			    // Inc9 - TODO Remove
        	{
        		pFlashParOut->pFlash  = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SPI3;        		
        	}

			if (pDevExtIn->lBitStreamId >= 0x0C)
				// Inc 12
			{
				pFlashParOut->pFlash = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_SPI3;
			}
            
            pFlashParOut->eFlashMode		        = ePNDEV_FLASH_MODE_SERIAL;
			pFlashParOut->eFlashTypePos[0]	        = ePNDEV_FLASH_TYPE_SPI_N25Q128A;
            pFlashParOut->eFlashInfo		        = ePNDEV_FLASH_INFO_INVALID;
            pFlashParOut->lSizeUsable		        = BOARD_FPGA1_HERA_SPI_FLASH__SIZE_USABLE;
            pFlashParOut->lSizeSector		        = SPI_FLASH_N25Q128A__SECTOR_SIZE;

            pFlashParOut->uSPI.lSizePage	        = SPI_FLASH_N25Q128A__PAGE_SIZE;
            pFlashParOut->uSPI.lAddressMask         = SPI_FLASH_N25Q128A__ADDRESS_MASK;
            pFlashParOut->uSPI.bIsSSPInitialized    = FALSE;

            *pOffsetFlashDataOut = BOARD_FPGA1_HERA_SPI_FLASH__OFFSET_DATA;

			break;
		}
		case ePNDEV_BOARD_TI_AM5728:
		{
			pFlashParOut->pFlash                    = pDevExtIn->uHwResKm.uBoard.as.uAM5728.uQspiReg.pPtr;            
			pFlashParOut->eFlashMode		        = ePNDEV_FLASH_MODE_SERIAL;
			pFlashParOut->eFlashTypePos[0]	        = ePNDEV_FLASH_TYPE_SPI_A25LQ032;
			pFlashParOut->eFlashInfo		        = ePNDEV_FLASH_INFO_INVALID;
			pFlashParOut->lSizeUsable		        = BOARD_TI_AM5728_SPI_FLASH__SIZE_USABLE;
			pFlashParOut->lSizeSector		        = SPI_FLASH_A25LQ032__SECTOR_SIZE;

			pFlashParOut->uSPI.lSizePage	        = SPI_FLASH_A25LQ032__PAGE_SIZE;
			pFlashParOut->uSPI.lAddressMask         = SPI_FLASH_A25LQ032__ADDRESS_MASK;
			pFlashParOut->uSPI.bIsSSPInitialized    = FALSE;

			*pOffsetFlashDataOut = BOARD_TI_AM5728_SPI_FLASH__OFFSET_DATA;

            break;
        }
		case ePNDEV_BOARD_CP1625:
        {
			pFlashParOut->pFlash = pDevExtIn->uHwResKm.uBoard.as.uSoc1.uApbPer.pPtr + SOC1_APB_PER__OFFSET_SPI;
            
            pFlashParOut->eFlashMode		        = ePNDEV_FLASH_MODE_SERIAL;
			pFlashParOut->eFlashTypePos[0]	        = ePNDEV_FLASH_TYPE_SPI_M25P10A;
            pFlashParOut->eFlashInfo		        = ePNDEV_FLASH_INFO_INVALID;
            pFlashParOut->lSizeUsable		        = BOARD_CP1625_SPI_FLASH__SIZE_USABLE;
            pFlashParOut->lSizeSector		        = SPI_FLASH_M25P10A__SECTOR_SIZE;

            pFlashParOut->uSPI.lSizePage	        = SPI_FLASH_M25P10A__PAGE_SIZE;
            pFlashParOut->uSPI.lAddressMask         = SPI_FLASH_M25P10A__ADDRESS_MASK;
            pFlashParOut->uSPI.bIsSSPInitialized    = FALSE;

            *pOffsetFlashDataOut = BOARD_CP1625_SPI_FLASH__OFFSET_DATA;

			break;
		}
		default:
		{
			break;
		}
	}

	if	(pFlashParOut->pFlash != NULL)
		// success
	{
		// Note:
		//	- if a command is written then each chip must get the command -> datum must be duplicated if chips are interleaved
		//	- NOR  flash:
		//		8Bit offset of register X of chip 0 (shown at DataSheet) must be multiplied by DataWidth, this is independent of interleaving
		//		16Bit:	addressing Offset=0/1/2/.. of chip 0 -> MemOffset=0/2/4/..
		//		32Bit:	addressing Offset=0/1/2/.. of chip 0 -> MemOffset=0/4/8/..
		//	- NAND flash: offsets of DataReg/AdrReg/CmdReg are always constant independent of interleaving (ALE and CLE are set by fixed AddressBits)

		switch	(pFlashParOut->eFlashMode)
				// FlashMode
		{
			case ePNDEV_FLASH_MODE_X8_AS_X8:								// no-interleaving, a single x8 device in x8 mode
			{
				pFlashParOut->lDataWidth		= 1;						// 8Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT8 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X8_AS_X16:								// two x8 devices interleaved to form x16
			{
				pFlashParOut->lDataWidth		= 2;						// 16Bit
				pFlashParOut->lDataMultiplier	= 0x00000101;				// multiplying with a UINT8 value X will show X at higher and lower byte (= chip1 and chip2)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				// For now we only use this mode on soc1_pci or soc1_pcie boards
				if	(	pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCI ||
						pDevExtIn->eBoard == ePNDEV_BOARD_DB_SOC1_PCIE)
				{
					// the whole address is calculated with adress of all nand flashes together -> multiply with datawidth
					// when accessing the nand flashes datawidth is used to calculate actual address of each flash -> divide through datawidth
					*pOffsetFlashDataOut *= pFlashParOut->lDataWidth;
				}
				break;
			}
			case ePNDEV_FLASH_MODE_X16_AS_X16:								// no-interleaving, a single x16 device in x16 mode
			{
				pFlashParOut->lDataWidth		= 2;						// 16Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT16 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X16_AS_X32:								// two x16 devices interleaved to form x32
			{
				pFlashParOut->lDataWidth		= 4;						// 32Bit
				pFlashParOut->lDataMultiplier	= 0x00010001;				// multiplying with a UINT16 value X will show X at higher and lower word (= chip1 and chip2)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			case ePNDEV_FLASH_MODE_X32_AS_X32:								// no-interleaving, a single x32 device in x32 mode
			{
				pFlashParOut->lDataWidth		= 4;						// 32Bit
				pFlashParOut->lDataMultiplier	= 0x00000001;				// multiplying with a UINT32 value X will result X (= chip1)
				pFlashParOut->lAdrMultiplierNor	= pFlashParOut->lDataWidth;

				break;
			}
			default:
			{
				break;
			}
		}

		if	(!bVerifyChipIn)
			// don't verify flash chip
		{
			// success
			bResult = TRUE;
		}
		else
		{
			if	(fnFlash_VerifyChip(pDevExtIn,
									lDimErrorIn,
									pFlashParOut,
									sErrorOut))
				// verifying flash chip ok
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
//  stop FlashPage
//************************************************************************

void fnBoard_StopFlashPage(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		{
			break;
		}
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPage(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: end of using dynamic DriverPage]
			fnBoardPnip_StopDriverPageXhif(pDevExtIn);

			break;
		}
        case ePNDEV_BOARD_FPGA1_HERA:
        {
            break;
        }
		default:
		{
			break;
		}
	}
}
#endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec400.uEmifSdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			//------------------------------------------------------------------------
			// [LBU: using dynamic DriverPage]
			//	- 8Bit-flash, page0
			fnBoardIrte_StartDriverPage(	pDevExtIn,
											eBOARD_EB200__LBU_DYN_PAGE_EMIF_SDRAM,
											lSegmentAhbIn,
											FALSE,
											__FUNCTION__);
			{
				*pPagedAccessOut	= TRUE;
				*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200.uEmifSdram_DynPage.pPtr;
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		case ePNDEV_BOARD_CP1625:		// CP1625 board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uSoc1.uDdr2Sdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			switch	(pDevExtIn->eBoardDetail)
					// BoardDetail
			{
				case ePNDEV_BOARD_DETAIL_ERTEC200P_AHB:		// board with Ertec200P/PCIe-AHB
				{
					*pPagedAccessOut	= FALSE;
					*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram.pPtr;

					break;
				}
				case ePNDEV_BOARD_DETAIL_ERTEC200P_XHIF:	// board with Ertec200P/PCIe-XHIF
				{
					//------------------------------------------------------------------------
					// [XHIF: using dynamic DriverPage]:
					//	- EMC Sdram
					fnBoardPnip_StartDriverPageXhif(pDevExtIn,
													eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM,
													lSegmentAhbIn,
													__FUNCTION__);
					{
						*pPagedAccessOut	= TRUE;
						*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr;
					}

					break;
				}
				default:
				{
					break;
				}
			}

			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: using dynamic DriverPage]:
			//	- EMC Sdram
			fnBoardPnip_StartDriverPageXhif(pDevExtIn,
											eBOARD_ERTEC200P_XHIF__DYN_PAGE_EMC_SDRAM,
											lSegmentAhbIn,
											__FUNCTION__);
			{
				*pPagedAccessOut	= TRUE;
				*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uErtec200P.uEmcSdram_DynPage.pPtr;
			}

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr;

			break;
		}
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
			*pPagedAccessOut	= FALSE;
			*pPtrCurSegmentOut	= pDevExtIn->uHwResKm.uBoard.as.uAM5728.uEmifSdram.pPtr;

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
//  stop SdramPage
//************************************************************************

void fnBoard_StopSdramPage(DEVICE_EXTENSION* pDevExtIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:	// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			break;
		}
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			//------------------------------------------------------------------------
			// [LBU: end of using dynamic DriverPage]
			fnBoardIrte_StopDriverPage(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		case ePNDEV_BOARD_CP1625:		// CP1625 board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		case ePNDEV_BOARD_EB200P:			// EB200P board
		{
			//------------------------------------------------------------------------
			// [XHIF: end of using dynamic DriverPage]
			fnBoardPnip_StopDriverPageXhif(pDevExtIn);

			break;
		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
			break;
		}
		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
		{
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
            #ifdef PNDEV_SUPPORT_OTHERS
			case ePNDEV_ASIC_ERTEC400:		{lAdrAsicSdram_AsicCore = ERTEC400_AHB_ADR__EMIF_SDRAM;					break;}
			case ePNDEV_ASIC_ERTEC200:		{lAdrAsicSdram_AsicCore = ERTEC200_AHB_ADR__EMIF_SDRAM;					break;}
			case ePNDEV_ASIC_SOC1:			{lAdrAsicSdram_AsicCore = SOC1_AHB_ADR__DDR2_SDRAM_BY_MIPS_UNCACHED;	break;}
			case ePNDEV_ASIC_ERTEC200P:		{lAdrAsicSdram_AsicCore = ERTEC200P_AHB_ADR__EMC_SDRAM;					break;}
			case ePNDEV_ASIC_HERA:			{lAdrAsicSdram_AsicCore = HERA_AHB_ADR__DDR3SDRAM;						break;}
			case ePNDEV_ASIC_AM5728:		{lAdrAsicSdram_AsicCore = AM5728_AHB_ADR__EMIF_SDRAM;					break;}
            #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// HW_Spec_CP1616_V12.Doc:
			//	- Page 17-19

			// LEDs:
			//	- GPIO2:  Activity LED Port0
			//	- GPIO3:  Link LED Port0
			//	- GPIO4:  Activity LED Port1
			//	- GPIO5:  Link LED Port1
			//	- GPIO6:  red SF-LED
			//	- GPIO7:  red BF-LED
			//	- GPIO8:  Activity LED Port2
			//	- GPIO9:  Link LED Port2
			//	- GPIO10: Activity LED Port3
			//	- GPIO11: Link LED Port3
			//	- low active

			// initialize values of GPIOs before setting as output
			{
/*
				// clear SF/BF-LED: GPIO7=GPIO6=1
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_7
									| PNDEV_UINT32_SET_BIT_6));
*/
				// connect all Link/Activity LEDs to PHYs (GPIOx=1)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_11
									| PNDEV_UINT32_SET_BIT_10
									| PNDEV_UINT32_SET_BIT_9
									| PNDEV_UINT32_SET_BIT_8
									| PNDEV_UINT32_SET_BIT_5
									| PNDEV_UINT32_SET_BIT_4
									| PNDEV_UINT32_SET_BIT_3
									| PNDEV_UINT32_SET_BIT_2));
			}

			// set as output: all used GPIOs
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__IOCTRL0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__IOCTRL0)
								& PNDEV_UINT32_CLR_BIT_11
								& PNDEV_UINT32_CLR_BIT_10
								& PNDEV_UINT32_CLR_BIT_9
								& PNDEV_UINT32_CLR_BIT_8
//								& PNDEV_UINT32_CLR_BIT_7
//								& PNDEV_UINT32_CLR_BIT_6
								& PNDEV_UINT32_CLR_BIT_5
								& PNDEV_UINT32_CLR_BIT_4
								& PNDEV_UINT32_CLR_BIT_3
								& PNDEV_UINT32_CLR_BIT_2));

			break;
		}
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			// EB200_Handbuch_V110.pdf:
			//	- Page 15
			// PH_IEASCs_ERTEC200_V14.pdf:
			//	- Page 103-104

			// LEDs:
			//	- GPIO2:  red BF-LED
			//	- GPIO3:  green SF-LED
			//	- GPIO4:  Link LED Port0,		AltFct2
			//	- GPIO5:  Link LED Port1,		AltFct2
			//	- GPIO6:  Activity LED Port0,	AltFct4
			//	- GPIO7:  Activity LED Port1,	AltFct4
			//	- low active

			// initialize values of GPIOs before setting as output
			{
/*
				// clear SF/BF-LED: GPIO3=GPIO2=1
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									| PNDEV_UINT32_SET_BIT_3
									| PNDEV_UINT32_SET_BIT_2));
*/
			}

			// set as output: all used GPIOs (independent which AltFct is used)
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__IOCTRL0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__IOCTRL0)
								& PNDEV_UINT32_CLR_BIT_7
								& PNDEV_UINT32_CLR_BIT_6
								& PNDEV_UINT32_CLR_BIT_5
								& PNDEV_UINT32_CLR_BIT_4));
//								& PNDEV_UINT32_CLR_BIT_3
//								& PNDEV_UINT32_CLR_BIT_2));

			// setting of AltFct may additionally be done by SystemEmbedding of EDDI: see EDDI_SysE200GpioSetActivityLinkLed()
			{
				// set AltFct2: GPIO5, GPIO4
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0)
									| (1 << 10)
									| (1 << 8)));

				// set AltFct4: GPIO7, GPIO6
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__PORT_MODE_L0)
									| (3 << 14)
									| (3 << 12)));
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		case ePNDEV_BOARD_CP1625:		// CP1625 board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
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
//  set state of green and red LED
//************************************************************************

void fnBoard_SetLed(DEVICE_EXTENSION*	pDevExtIn,
					BOOLEAN				bGreenOnIn,
					BOOLEAN				bRedOnIn)
{
	switch	(pDevExtIn->eBoard)
			// BoardType
	{
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_CP1616:			// CP1616 board
		case ePNDEV_BOARD_DB_EB400_PCIE:	// DB-EB400-PCIe board
		{
			// HW_Spec_CP1616_V12.Doc:
			//	- Page 17-19

			// LEDs:
			//	- GPIO2:  Activity LED Port0
			//	- GPIO3:  Link LED Port0
			//	- GPIO4:  Activity LED Port1
			//	- GPIO5:  Link LED Port1
			//	- GPIO6:  red SF-LED
			//	- GPIO7:  red BF-LED
			//	- GPIO8:  Activity LED Port2
			//	- GPIO9:  Link LED Port2
			//	- GPIO10: Activity LED Port3
			//	- GPIO11: Link LED Port3
			//	- low active

			// clear SF/BF-LED: GPIO7=GPIO6=1
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
								| PNDEV_UINT32_SET_BIT_7
								| PNDEV_UINT32_SET_BIT_6));

			if	(bGreenOnIn)
				// green LED ON
			{
				// set red SF-LED (GPIO6=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_6));
			}

			if	(bRedOnIn)
				// red LED ON
			{
				// set red BF-LED (GPIO7=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec400.uApbPer.pPtr + ERTEC400_APB_PER__OFFSET_GPIO + ERTEC400_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_7));
			}

			break;
		}
		case ePNDEV_BOARD_EB200:			// EB200 board
		case ePNDEV_BOARD_DB_EB200_PCIE:	// DB-EB200-PCIe board
		{
			// EB200_Handbuch_V110.pdf:
			//	- Page 15
			// PH_IEASCs_ERTEC200_V14.pdf:
			//	- Page 103-104

			// LEDs:
			//	- GPIO2:  red BF-LED
			//	- GPIO3:  green SF-LED
			//	- GPIO4:  Link LED Port0,		AltFct2
			//	- GPIO5:  Link LED Port1,		AltFct2
			//	- GPIO6:  Activity LED Port0,	AltFct4
			//	- GPIO7:  Activity LED Port1,	AltFct4
			//	- low active

			// clear SF/BF-LED: GPIO3=GPIO2=1
			WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
								(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
								| PNDEV_UINT32_SET_BIT_3
								| PNDEV_UINT32_SET_BIT_2));

			if	(bGreenOnIn)
				// green LED ON
			{
				// set green SF-LED (GPIO3=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_3));
			}

			if	(bRedOnIn)
				// red LED ON
			{
				// set red BF-LED (GPIO2=0)
				WRITE_REG_UINT32(	(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0),
									(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uErtec200.uApbPer.pPtr + ERTEC200_APB_PER__OFFSET_GPIO + ERTEC200_GPIO_REG__OUT0)
									& PNDEV_UINT32_CLR_BIT_2));
			}

			break;
		}
		case ePNDEV_BOARD_DB_SOC1_PCI:	// DB-Soc1-PCI board
		case ePNDEV_BOARD_DB_SOC1_PCIE:	// DB-Soc1-PCIe board
		case ePNDEV_BOARD_CP1625:		// CP1625 board
		{
			break;
		}
		case ePNDEV_BOARD_FPGA1_ERTEC200P:	// FPGA1-Ertec200P board
		{
			break;
		}
		case ePNDEV_BOARD_EB200P:	// EB200P board
		{
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
        #ifdef PNDEV_SUPPORT_OTHERS
//		case ePNDEV_BOARD_TI_AM5728:	// TI AM5728 board
//		{
//					
//			if	(lRegionIdx != 0)
//				// valid region
//			{
//				// set ErrorString
//				fnBuildString(	_TEXT(__FUNCTION__),
//								_TEXT("(): Invalid Region!"),
//								NULL,
//								lDimErrorIn,
//								sErrorOut);
//			}
//			else
//			{
//			volatile UINT8*	pPtrTmp 	= NULL;
//			UINT32			lRegionSize = 0;
//			
//				// set ptr for Configuration
//				pPtrTmp = pDevExtIn->uHwResKm.uBoard.as.uAM5728.uPcieSs1.pPtr;
//			
//				// get lower target address
//				lRegionSize = READ_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET);
//				
//				// update ATU region index to region 0
//				WRITE_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_INDEX, 0x0);
//				
//				// get max Address
//				lRegionSize = 	(	READ_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LIMIT) 
//								- 	READ_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_BASE) + 1);
//				
//				if 	(lRegionSize < lSizeIn)
//					// valid Size
//				{
//					// set ErrorString
//					fnBuildString(	_TEXT(__FUNCTION__),
//									_TEXT("(): Invalid RegionSize!"),
//									NULL,
//									lDimErrorIn,
//									sErrorOut);
//				}
//				else
//				{
//				UINT32 lTargetAddr = 0;
//					
//					// get lower target address
//					lTargetAddr = READ_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET);
//					
//					if	(lTargetAddr != 0xAABBC000)
//						// valid lTargetAddr
//					{
//						// set ErrorString
//						fnBuildString(	_TEXT(__FUNCTION__),
//										_TEXT("(): Invalid TargetAddr set by SBL!"),
//										NULL,
//										lDimErrorIn,
//										sErrorOut);
//					}
//					else
//					{
//						if 	((lPhyAddrIn % 0x1000) != 0)
//							// not 4 kByte alligned 
//						{
//							// set ErrorString
//							fnBuildString(	_TEXT(__FUNCTION__),
//											_TEXT("(): Invalid PhyAddr (is not 4 kByte alligned)!"),
//											NULL,
//											lDimErrorIn,
//											sErrorOut);
//						}
//						else
//						{				
//							// set physical host address
//							WRITE_REG_UINT32(pPtrTmp + BOARD_TI_AM5728__PCIE_SS1_PCIECTRL_PL_IATU_REG_LOWER_TARGET, lPhyAddrIn);
//										
//							bResult = TRUE;
//						}
//					}
//				}
//			}
//
//			break;
//		}
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
		volatile UINT8*	pPtrAddrLow		= NULL;
		UINT32			lResetValue		= 0;

			if (pDevExtIn->lBitStreamId == 0xc)
				// Inc 12
			{
				switch (lRegionIdx)
				{
					case 0:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR0_LOW_RESET;
						break;
					}
					case 1:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR1_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR1_LOW_RESET;
						break;
					}
					case 2:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR2_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR2_LOW_RESET;
						break;
					}
					case 3:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR3_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR3_LOW_RESET;
						break;
					}
					case 4:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR4_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR4_LOW_RESET;
						break;
					}
					case 5:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR5_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR5_LOW_RESET;
						break;
					}
					case 6:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR6_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR6_LOW_RESET;
						break;
					}
					case 7:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR7_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR7_LOW_RESET;
						break;
					}
					case 8:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR8_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR8_LOW_RESET;
						break;
					}
					case 9:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR9_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR9_LOW_RESET;
						break;
					}
					case 10:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR10_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR10_LOW_RESET;
						break;
					}
					case 11:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR11_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR11_LOW_RESET;
						break;
					}
					case 12:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR12_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR12_LOW_RESET;
						break;
					}
					case 13:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR13_LOW;
						lResetValue = HERA_PCIE_UL_REG__ADDR13_LOW_RESET;
						break;
					}
					default:
					{
						// set ErrorString
						fnBuildString(_TEXT(__FUNCTION__),
											_TEXT("(): Invalid Region!"),
											NULL,
											lDimErrorIn,
											sErrorOut);

						break;
					}
				}
			}

			if (pDevExtIn->lBitStreamId == 0x09)
				// Inc9 - TODO Remove
			{
				switch (lRegionIdx)
				{
					case 0:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR0_LOW_RESET;
						break;
					}
					case 1:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR1_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR1_LOW_RESET;
						break;
					}
					case 2:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR2_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR2_LOW_RESET;
						break;
					}
					case 3:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR3_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR3_LOW_RESET;
						break;
					}
					case 4:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR4_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR4_LOW_RESET;
						break;
					}
					case 5:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR5_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR5_LOW_RESET;
						break;
					}
					case 6:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR6_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR6_LOW_RESET;
						break;
					}
					case 7:
					{
						// set ptr for Configuration
						pPtrAddrLow = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR7_LOW_INC9;
						lResetValue = HERA_PCIE_UL_REG__ADDR7_LOW_RESET;
						break;
					}
					default:
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid Region!"),
										NULL,
										lDimErrorIn,
										sErrorOut);

						break;
					}
				}
			}

			if	(pPtrAddrLow != NULL)
				// valid region
			{
			UINT32	lTargetAddr 	= 0;
			UINT32	lWindowAlign	= 0;			
			
				if	(pDevExtIn->lBitStreamId == 0x09)
				    // Inc9 - TODO Remove
				{
					lWindowAlign = HERA_PCIE_MEM__WINDOW_ALIGNMENT;      		
				}

				if (pDevExtIn->lBitStreamId >= 0x0C)
					// Inc 12
				{
					lWindowAlign = HERA_PCIE_MEM__WINDOW_ALIGNMENT;
				}
				
                lTargetAddr = READ_REG_UINT32(pPtrAddrLow);
				
				if		(lPhyAddrIn == 0 && lSizeIn == 0)
						// reset physical host address?
				{
					if 	(lResetValue != lTargetAddr)
						// physical host address not set to reset value
					{
						// set physical host address to reset value
						WRITE_REG_UINT32(pPtrAddrLow, lResetValue);
					}
					bResult = TRUE;
				}
				/*else if (lResetValue != lTargetAddr)
						// physical host address is already in use (not set to reset value)
				{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Invalid TargetAddr, Region already in use?"),
										NULL,
										lDimErrorIn,
										sErrorOut);
				}*/
				else if (HERA_PCIE_MEM__WINDOW_SIZE_LIMIT < lSizeIn)
						// valid Size
				{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Invalid RegionSize!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
				}
				else
				{
					char	sTraceDetail[PNDEV_SIZE_STRING_TRACE] = { 0 };
					UINT32	lOffsetAddr = 0;
					UINT32	lSizeWaste = 0;

					if (lWindowAlign == 0)
						// check if align is zero
					{
						// set ErrorString
						fnBuildString(	_TEXT(__FUNCTION__),
										_TEXT("(): Division by zero!"),
										NULL,
										lDimErrorIn,
										sErrorOut);
					}
					else
					{
						lOffsetAddr = lPhyAddrIn % lWindowAlign;

						if ((lOffsetAddr + lSizeIn) > HERA_PCIE_MEM__WINDOW_SIZE_LIMIT)
							// lSizeWaste != 0
						{
							if ((lOffsetAddr + lSizeIn) > (HERA_PCIE_MEM__WINDOW_SIZE_LIMIT + lSizeIn / 2))
							{
								lSizeWaste = HERA_PCIE_MEM__WINDOW_SIZE_LIMIT - lOffsetAddr;
							}
							else
							{
								lSizeWaste = lSizeIn - (HERA_PCIE_MEM__WINDOW_SIZE_LIMIT - lOffsetAddr);
							}
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: Invalid physical host address: lPhyAddrIn=0x%x, lOffsetAddr=0x%x, lSizeIn=0x%x, lSizeWaste=0x%x",
												pDevExtIn->sPciLocShortAscii,
												lPhyAddrIn,
												lOffsetAddr,
												lSizeIn,
												lSizeWaste);

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_ERROR,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,						// logging
										sTraceDetail);
						}
						else
						{
							// set physical host address
							WRITE_REG_UINT32(pPtrAddrLow, lPhyAddrIn);
							lTargetAddr = READ_REG_UINT32(pPtrAddrLow);

							// only for test purpose propargate RegionIdx to bootloader
							// TODO save lRegionIdx in uPNDEV_APP or uPNDEV_SHARED_HOST_SDRAM to use more than one region!
							// WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr), lRegionIdx); // needed for SharedHostSdram Test with Bootloader!
							// set TraceDetail
							RtlStringCchPrintfA(sTraceDetail,
												_countof(sTraceDetail),
												"PnDev[%s]: Physical host address: lPhyAddrIn=0x%x, lOffsetAddr=0x%x, lSizeIn=0x%x, lRegionIdx=0x%x",
												pDevExtIn->sPciLocShortAscii,
												lPhyAddrIn,
												lOffsetAddr,
												lSizeIn,
												lRegionIdx);

							// set trace
							fnSetTrace(	ePNDEV_TRACE_LEVEL_INFO,
										ePNDEV_TRACE_CONTEXT_BASIC,
										TRUE,						// logging
										sTraceDetail);

							if ((lTargetAddr + lOffsetAddr) != lPhyAddrIn)
								// Invalid TargetAddr or lOffsetAddr
							{
								// set ErrorString
								fnBuildString(	_TEXT(__FUNCTION__),
												_TEXT("(): Invalid TargetAddr or lOffsetAddr!"),
												NULL,
												lDimErrorIn,
												sErrorOut);
							}
							else
							{
								bResult = TRUE;
							}
						}
					}
				}
			}

			break;
		}
        #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
		volatile UINT8*	pPtrAddrUL		= NULL;
		UINT32  		i 				= 0;

			if (pDevExtIn->lBitStreamId == 0x09)
				// Inc 9 - TODO Remove
			{
				// save state of user logic pci master window register
				for (i = 0; i < ((HERA_PCIE_UL_REG__ADDR7_HIGH_INC9 - HERA_PCIE_UL_REG__ADDR0_LOW_INC9) / sizeof(UINT32) + 1); i++)
				{
					pPtrAddrUL = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW_INC9 + (i * 4);
					lArrayRegister[i] = READ_REG_UINT32(pPtrAddrUL);
				}
			}

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// save state of user logic pci master window register
				for (i = 0; i < ((HERA_PCIE_UL_REG__ADDR13_HIGH - HERA_PCIE_UL_REG__ADDR0_LOW) / sizeof(UINT32) + 1); i++)
				{
					pPtrAddrUL = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW + (i * 4);
					lArrayRegister[i] = READ_REG_UINT32(pPtrAddrUL);
				}
			}

			bResult = TRUE;
			break;
		}
        #endif

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
        #ifdef PNDEV_SUPPORT_OTHERS
		case ePNDEV_BOARD_FPGA1_HERA:	// FPGA1-Hera board
		{
		volatile UINT8*	pPtrAddrUL		= NULL;
		UINT32  		i 				= 0;

			if (pDevExtIn->lBitStreamId == 0x09)
				// Inc 9 - TODO Remove
			{
				// restore state of user logic pci master window register
				for (i = 0; i < ((HERA_PCIE_UL_REG__ADDR7_HIGH_INC9 - HERA_PCIE_UL_REG__ADDR0_LOW_INC9) / sizeof(UINT32) + 1); i++)
				{
					pPtrAddrUL = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW_INC9 + (i * 4);
					WRITE_REG_UINT32(pPtrAddrUL, lArrayRegister[i]);
				}
			}		

			if (pDevExtIn->lBitStreamId >= 0x0c)
				// Inc 12
			{
				// restore state of user logic pci master window register
				for (i = 0; i < ((HERA_PCIE_UL_REG__ADDR13_HIGH - HERA_PCIE_UL_REG__ADDR0_LOW) / sizeof(UINT32) + 1); i++)
				{
					pPtrAddrUL = pDevExtIn->uHwResKm.uBoard.as.uHera.uApbPer.pPtr + HERA_APB_PER__OFFSET_PCIE_UL + HERA_PCIE_UL_REG__ADDR0_LOW + (i * 4);
					WRITE_REG_UINT32(pPtrAddrUL, lArrayRegister[i]);
				}
			}
			
			// only for test purpose propargate RegionIdx to bootloader
			// TODO save lRegionIdx in uPNDEV_APP or uPNDEV_SHARED_HOST_SDRAM to use more than one region!
			// i = 0;
			// WRITE_REG_UINT32((pDevExtIn->uHwResKm.uBoard.as.uHera.uDdr3Sdram.pPtr), i); // needed for SharedHostSdram Test with Bootloader!
			
			bResult = TRUE;
			break;
		}
        #endif

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



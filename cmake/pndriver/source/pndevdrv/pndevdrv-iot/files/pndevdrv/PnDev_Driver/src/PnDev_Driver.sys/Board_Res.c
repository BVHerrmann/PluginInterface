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
/*  F i l e               &F: Board_Res.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for handling board resources
//
//****************************************************************************/

#include "precomp.h"														// environment specific IncludeFiles
#include "Inc.h"

#ifdef PNDEV_TRACE
	#include "Board_Res.tmh"
#endif

#ifdef ALLOC_PRAGMA
//	- don't permit paging (see...)
//	#pragma alloc_text (PAGE, fnBoard_GetHwResKm)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResKm)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm
//	#pragma alloc_text (PAGE, fnBoard_SetPtrMemMapUM)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_Single)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_UpdatePtrHwResUm_Sdram)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer400)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer200)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPerSoc1)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPer200P)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPerHera)
//	#pragma alloc_text (PAGE, fnBoard_SetPtrHwResUm_ApbPerAM5728)
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  get HwRes of board
//	- get the HW resources assigned by the bus driver from the start-IRP
//	- map it to system address space
//************************************************************************

BOOLEAN fnBoard_GetHwResKm(	DEVICE_EXTENSION*	pDevExtIn,
							HANDLE		        hOsTransParIn,
							const UINT32		lDimErrorIn,
							_TCHAR*				sErrorOut)
{
BOOLEAN	bResult		= FALSE;
UINT32	lFirstBar	= 0;

	if	(	(pDevExtIn->eBoard == ePNDEV_BOARD_CP1616)
		|| 	(pDevExtIn->eBoard == ePNDEV_BOARD_DB_EB400_PCIE))
		// CP1616 board or DB-EB400-PCIe board
	{
		// start with BAR1
		lFirstBar = 1;
	}

	if	(fnGetResListConfigSpace(	pDevExtIn,
									hOsTransParIn,
									lFirstBar,
									lDimErrorIn,
									sErrorOut))
		// getting ResList of ConfigSpace ok
	{
		// success
		bResult = TRUE;
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  verify BarConfig
//************************************************************************

BOOLEAN fnBoard_VerifyBarConfig(DEVICE_EXTENSION*	pDevExtIn,
								volatile UINT32*	pArrayBarSizeIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult = FALSE;
UINT32	i = 0;

	// preset
	bResult = TRUE;

	for	(i = 0; (i < PNDEV_DIM_ARRAY_BAR); i++)
		// all BARs
	{
	UINT32 lBarSizeTmp = pArrayBarSizeIn[i];

		if	(pDevExtIn->uHwResKm.uBar[i].lSize != lBarSizeTmp)
			// invalid BarSize
		{
		_TCHAR sErrorDetail[PNDEV_SIZE_STRING_BUF] = {0};

			// error
			bResult = FALSE;

			// set ErrorDetail
			_RtlStringCchPrintf(sErrorDetail,
								_countof(sErrorDetail),
								_TEXT("(): Invalid BAR%u, (expected size: 0x%x, real size: 0x%x)!"),
								i,
								lBarSizeTmp,
								pDevExtIn->uHwResKm.uBar[i].lSize);

			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							sErrorDetail,
							NULL,
							lDimErrorIn,
							sErrorOut);

			// leave loop
			break;
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set KernelModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResKm(	DEVICE_EXTENSION*	pDevExtIn,
								const UINT32		lDimErrorIn,
								_TCHAR*				sErrorOut)
{
BOOLEAN	bResult								= FALSE;
BOOLEAN	bResultTmp							= FALSE;
UINT32	lArrayBarSize[PNDEV_DIM_ARRAY_BAR]	= {0,0,0,0,0,0};

	// preset
	pDevExtIn->bPcBoard = TRUE;

	// set common MemoryMapping
	pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart	= PNCORE_SHARED_ASIC_SDRAM__OFFSET_INDIRECT0;
	pDevExtIn->uAsicSdram.uIndirect[0].lSize			= PNCORE_SHARED_ASIC_SDRAM__SIZE_INDIRECT0;
	pDevExtIn->uAsicSdram.uDirect.lOffsetPhyStart		= PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;

	switch	(pDevExtIn->eBoard)
			// BoardType
	{




		case ePNDEV_BOARD_IX1000: // Intel Quark X1000 on Galileo board
		{
			// set BarSize

			lArrayBarSize[0]	= BOARD_IX1000__BAR0_SIZE;
			lArrayBarSize[1]	= BOARD_IX1000__BAR1_SIZE;
			lArrayBarSize[2]	= BOARD_IX1000__BAR2_SIZE;
			lArrayBarSize[3]	= BOARD_IX1000__BAR3_SIZE;
			lArrayBarSize[4]	= BOARD_IX1000__BAR4_SIZE;
			lArrayBarSize[5]	= BOARD_IX1000__BAR5_SIZE;

			if	(fnBoard_VerifyBarConfig(	pDevExtIn,
											lArrayBarSize,
											lDimErrorIn,
											sErrorOut))
				// verifying BarConfig ok
			{
				// success
				bResultTmp = TRUE;

				// BAR0
				{
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr			= (pDevExtIn->uHwResKm.uBar[0].pVirtualAdr + 0x0);
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.lIndexBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.lOffsetBar		= 0;
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.lSize			= pDevExtIn->uHwResKm.uBar[0].lSize;
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.lAdrAhb			= 0;
					pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.lOffsetPhyStart	= 0;
				}

				// set IcuPtr
				pDevExtIn->pIntelIcu = pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr;

				// (bit 15:8) User-defined Version (1.0) (USERVER), (bit 7:0) Synopsys-defined Version (3.7) (SNPSVER):
				pDevExtIn->lBoardRevision = (	(READ_REG_UINT32(pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0.pPtr + BOARD_IX1000__VERSION))
											&	0xFFFF);
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

	if	(bResultTmp)
		// success
	{
		if	(fnBoard_InitVar(pDevExtIn))
			// initializing variables ok
			//	- cannot be done before pAdminReg is known
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
//  set UserModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrMemMapUM(	uPNDEV_APP*							pAppIn,
								DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalInOut,
								_TCHAR*								sErrorOut)
{
BOOLEAN	bResult	= FALSE;
UINT32	j		= 0;

	// preset
	bResult = TRUE;

	// set Bar ptr
	for	(j = 0; j < PNDEV_DIM_ARRAY_BAR; j++)
	{
		pInternalInOut->uBarKm[j].uVirtualAdr.pPtr 	= pDevExtIn->uHwResKm.uBar[j].pVirtualAdr;
		pInternalInOut->uBarKm[j].lPhyAdr 			= pDevExtIn->uHwResKm.uBar[j].uPhyAdr.QuadPart;
		pInternalInOut->uBarKm[j].lSize 			= pDevExtIn->uHwResKm.uBar[j].lSize;
	}

	// set Sdram ptr
	{
	UINT32 			lOffsetDirect 		= 0;
	UINT32 			lOffsetPhyStartTmp 	= 0;
	volatile UINT8* pVirtualPtrTmp 		= 0;
	INT64 			lPhysAddrTmp		= 0;

		switch	(pDevExtIn->eBoard)
				// BoardType
		{




			case ePNDEV_BOARD_IX1000:	// Intel Quark X1000 on Galileo board
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

		if	(pVirtualPtrTmp != NULL)
			// valid ptr
		{
			if	(lOffsetPhyStartTmp == 0)
				// KernelModePtr references start of physical AsicSdram
			{
				// offset of SharedAsicSdram_Direct to HwRes = 2MB
				lOffsetDirect = PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
			}
			else	// KernelModePtr references SharedAsicSdram_Direct (2MB page)
			{
				// no offset of SharedAsicSdram_Direct to HwRes
				lOffsetDirect = 0;
			}

			pInternalInOut->uSdramKm.uVirtualAdr.pPtr	= (volatile UINT8*) (pVirtualPtrTmp + lOffsetDirect);
			pInternalInOut->uSdramKm.lSize				= pDevExtIn->uAsicSdram.uDirect.lSize;
			pInternalInOut->uSdramKm.lPhyAdr			= (lPhysAddrTmp + lOffsetDirect);
		}
	}

	// set HostMem ptr
	{
	    if  (pAppIn->bUsesHostMem && pAppIn->bUsesSharedHostMem)
	        // both does not work
	    {
	        bResult = FALSE;
	        
            // set ErrorString
            fnBuildString(  _TEXT(__FUNCTION__),
                            _TEXT("(): Cannot provide HostMem AND SharedHostMem at the same time!"),
                            NULL,
                            lDimErrorIn,
                            sErrorOut);
	    }	    
	    else if	(pAppIn->bUsesHostMem)
	    	// HostMem available
	    {
	    INT32 i = 0;

	    	// preset
	    	bResult = FALSE;

	    	for	(i=3; i>=0; i--)
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
						pInternalInOut->uHostMemKm.uVirtualAdr.pPtr = g_uMemSys.uHostMem[i].pAdrVirt;
						pInternalInOut->uHostMemKm.lPhyAdr	 		= g_uMemSys.uHostMem[i].lPhyAdr;
						pInternalInOut->uHostMemKm.lSize			= g_uMemSys.uHostMem[i].lSize;
						pAppIn->lUsesHostMemIndex					= (UINT32)i;

						// success
		            	bResult = TRUE;

						break;
		            }
	    		}
	    		else
	    		{
	    			// error
	    			bResult = FALSE;
	    		}
	    	}

	    	if  (!bResult)
	    	    // error
	    	{
					// set ErrorString
					fnBuildString(	_TEXT(__FUNCTION__),
									_TEXT("(): Error at setting ptr uHostMemKm for MappingVirtualToUM!"),
									NULL,
									lDimErrorIn,
									sErrorOut);
	    	}
	    }
	    else if(pAppIn->bUsesSharedHostMem)
	        // SharedHostMem available
	    {
        INT32 i = 0;

            // preset
            bResult = FALSE;

            for (i=0; i>=0; i--)
                // valid SharedHostMemIndex
            {
                if  (g_uMemSys.uSharedHostMem[i].pAdrVirt != NULL)
                    // memory pointer valid - DriverEntry was able to allocate memory
                {
                        pInternalInOut->uHostMemKm.uVirtualAdr.pPtr = g_uMemSys.uSharedHostMem[i].pAdrVirt;
                        pInternalInOut->uHostMemKm.lPhyAdr          = g_uMemSys.uSharedHostMem[i].lPhyAdr;
                        pInternalInOut->uHostMemKm.lSize            = g_uMemSys.uSharedHostMem[i].lSize;
                        pAppIn->lUsesHostMemIndex                   = (UINT32)i;
                   
                        if(!g_uMemSys.uSharedHostMem[i].bInUse)
                        {
                            g_uMemSys.uSharedHostMem[i].bInUse = TRUE;                            
                        }
                        
                        // success
                        bResult = TRUE;

                        break;
                }
                else
                {
                    // error
                    bResult = FALSE;
                }
            }

            if  (!bResult)
                // error
            {
                    // set ErrorString
                    fnBuildString(  _TEXT(__FUNCTION__),
                                    _TEXT("(): Error at setting ptr uHostMemKm for MappingVirtualToUM!"),
                                    NULL,
                                    lDimErrorIn,
                                    sErrorOut);
            }
	    }
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to HwRes
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResUm(	DEVICE_EXTENSION*					pDevExtIn,
								const UINT32						lDimErrorIn,
								uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
								uPNDEV_HW_RES*						pHwResUmOpenOut,
								volatile UINT8**					pIcuOut,
								uPNDEV_APP*							pAppOut,
								_TCHAR*								sErrorOut)
{
BOOLEAN	bResult		= FALSE;
BOOLEAN	bResultMap	= TRUE;
UINT32	i			= 0;

	for	(i = 0; i < PNDEV_DIM_ARRAY_BAR; i++)
	{
		pAppOut->uHwResUm.uBar[i] = pInternalIn->uBarUm[i];
	}

	if	(bResultMap)
		// success
	{
		switch	(pDevExtIn->eBoard)
				// BoardType
		{
            



            case ePNDEV_BOARD_IX1000: // Intel Quark X1000 on Galileo board
            {
                fnBoard_SetPtrHwResUm_Single(   pAppOut,
                                                &pDevExtIn->uHwResKm.uBoard.as.uIX1000.uBar0,
                                                &pHwResUmOpenOut->as.uIX1000.uBar0);

                // DLL: return ptr to Icu
                *pIcuOut = (volatile UINT8*)pHwResUmOpenOut->as.uIX1000.uBar0.uBase.pPtr;

                if (fnMapSharedHostMemUM(   pAppOut,
                                            pDevExtIn,
                                            lDimErrorIn,
                                            pInternalIn,
                                            &pHwResUmOpenOut->as.uIX1000.uSharedHostSdram,
                                            sErrorOut))
                    // map memory to userspace ok
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
								_TEXT("(): Invalid board!"),
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
//	set UserModePtr to a single HwRes
//************************************************************************

void fnBoard_SetPtrHwResUm_Single(	uPNDEV_APP*			pAppIn,
									uPNDEV_HW_DESC_KM*	pHwDescKmIn,
									uPNDEV_HW_DESC*		pHwDescUmOut)
{
	if	(pHwDescKmIn->lSize != 0)
		// resource exist
	{
	UINT32 lSizeHwRes	= 0;
	UINT32 lSizeBar		= 0;

		// Note:
		//	- if the size of a BAR is >= 64MB then its UserMode size is already reduced
		//	- if a HwRes fills the complete BAR then its size must be reduced too
		//	  (all other HwRes are already smaller)

		lSizeHwRes	= pHwDescKmIn->lSize;
		lSizeBar	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].lSize;

		if	(lSizeHwRes > lSizeBar)
			// HwResSize > BarSize
		{
			lSizeHwRes = lSizeBar;
		}

		pHwDescUmOut->uBase.pPtr	= pAppIn->uHwResUm.uBar[pHwDescKmIn->lIndexBar].uAdrUser.pPtr + pHwDescKmIn->lOffsetBar;
		pHwDescUmOut->lOffset		= 0;
		pHwDescUmOut->lSize			= lSizeHwRes;
		pHwDescUmOut->lAdrAhb		= pHwDescKmIn->lAdrAhb;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to AsicSdram
//************************************************************************

BOOLEAN fnBoard_SetPtrHwResUm_Sdram(DEVICE_EXTENSION*					pDevExtIn,
									uPNDEV_APP*							pAppIn,
									uPNDEV_OPEN_DEVICE_INTERNAL_OUT* 	pInternalIn,
									uPNDEV_HW_DESC_KM*					pHwDescKmIn,
									const UINT32						lDimErrorIn,
									uPNDEV_HW_DESC_SDRAM*				pHwDescUmOut,
									_TCHAR*								sErrorOut)
{
BOOLEAN	bResult			= FALSE;
UINT32	lOffsetDirect	= 0;

	// Note:
	//	- board with SdramPage:	KernelModePtr references start of physical AsicSdram
	//		else:				KernelModePtr references SharedAsicSdram_Direct (2MB page)
	//	- only SharedAsicSdram_Direct will be mapped to UserMode
	//	- mapping of complete BAR would cause some inconvenience:
	//		no protection of SharedAsicSdram_Reserved
	//		WinXP:	if BarSize=64MB:	reducing of SharedAsicSdram_Direct to 61,94MB (PNDEV_MAX_SIZE_MEM_UM-2MB) instead of 62MB,
	//									further exceptions if SharedAsicSdram_Direct is reduced by Fw

	if	(pHwDescKmIn->lOffsetPhyStart == 0)
		// KernelModePtr references start of physical AsicSdram
	{
		// offset of SharedAsicSdram_Direct to HwRes = 2MB
		lOffsetDirect = PNCORE_SHARED_ASIC_SDRAM__OFFSET_DIRECT;
	}
	else	// KernelModePtr references SharedAsicSdram_Direct (2MB page)
	{
		// no offset of SharedAsicSdram_Direct to HwRes
		lOffsetDirect = 0;
	}

	if	(pInternalIn->uSdramUm.uAdrUser.pPtr != NULL)
		// valid ptr
	{
		// success
		bResult = TRUE;

		// set UserModePtr
		pAppIn->uHwResUm.uAsicSdramDirect.uAdrUser.pPtr	= pInternalIn->uSdramUm.uAdrUser.pPtr;
		pAppIn->uHwResUm.uAsicSdramDirect.lSize			= pInternalIn->uSdramUm.lSize;
		pAppIn->uHwResUm.uAsicSdramDirect.uMdl.pPtr		= pInternalIn->uSdramUm.uMdl.pPtr;

		pHwDescUmOut->uDirect.uBase.pPtr	= pAppIn->uHwResUm.uAsicSdramDirect.uAdrUser.pPtr;
		pHwDescUmOut->uDirect.lOffset		= pDevExtIn->uAsicSdram.uDirect.lOffsetPhyStart;
		pHwDescUmOut->uDirect.lSize			= pAppIn->uHwResUm.uAsicSdramDirect.lSize;
		pHwDescUmOut->uDirect.lAdrAhb		= pHwDescKmIn->lAdrAhb + lOffsetDirect;

		pHwDescUmOut->lSizePhy				= pDevExtIn->uAsicSdram.lSizePhy;
		pHwDescUmOut->lSizeDriver			= pDevExtIn->uAsicSdram.lSizeDriver;
		pHwDescUmOut->uIndirect[0].lOffset	= pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart;
		pHwDescUmOut->uIndirect[0].lSize	= pDevExtIn->uAsicSdram.uIndirect[0].lSize;
		pHwDescUmOut->uIndirect[0].lAdrAhb	= (pHwDescKmIn->lAdrAhb - pHwDescKmIn->lOffsetPhyStart) + pDevExtIn->uAsicSdram.uIndirect[0].lOffsetPhyStart;

		pHwDescUmOut->uIndirect[1].lOffset	= pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart;
		pHwDescUmOut->uIndirect[1].lSize	= pDevExtIn->uAsicSdram.uIndirect[1].lSize;

		if	(pDevExtIn->uAsicSdram.uIndirect[1].lSize)
			// SharedAsicSdram_Indirect1 exist
		{
			pHwDescUmOut->uIndirect[1].lAdrAhb = (pHwDescKmIn->lAdrAhb - pHwDescKmIn->lOffsetPhyStart) + pDevExtIn->uAsicSdram.uIndirect[1].lOffsetPhyStart;
		}
	}
	else
	{
		// set ErrorString
		fnBuildString(	_TEXT(__FUNCTION__),
						_TEXT("(): No valid ptr from MappingVirtualToUM!"),
						NULL,
						lDimErrorIn,
						sErrorOut);
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  update UserModePtr to AsicSdram
//************************************************************************

BOOLEAN fnBoard_UpdatePtrHwResUm_Sdram(	uPNCORE_STARTUP_PAR_FW*	pStartupParFwIn,
										const UINT32			lDimErrorIn,
										uPNDEV_HW_DESC_SDRAM*	pHwDescUmInOut,
										_TCHAR*					sErrorOut)
{
BOOLEAN bResult				= FALSE;
BOOLEAN bResultIndirect1	= TRUE;

	if	(pStartupParFwIn->lSizeIndirect1 != 0)
		// setup SharedAsicSdram_Indirect1
	{
		// preset
		bResultIndirect1 = FALSE;

		if	(pHwDescUmInOut->uIndirect[1].lSize != 0)
			// SharedAsicSdram_Indirect1 already exist
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: SharedAsicSdram_Indirect1 already exist!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else if	((pStartupParFwIn->lSizeIndirect1 % 4) != 0)
				// size of SharedAsicSdram_Indirect1 not multiple of 4
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: Size of new SharedAsicSdram_Indirect1 is not a multiple of 4!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else if	(pStartupParFwIn->lSizeIndirect1 > (pHwDescUmInOut->uDirect.lSize - (1024*1024*2)))
				// reduced SharedAsicSdram_Direct < 2MB
				//	- SharedAsicSdram_Direct always >= 2MB -> difference >= 0
		{
			// set ErrorString
			fnBuildString(	_TEXT(__FUNCTION__),
							_TEXT("(): Firmware: Reduced SharedAsicSdram_Direct must have a minimal size of 2MB!"),
							NULL,
							lDimErrorIn,
							sErrorOut);
		}
		else
		{
			// success
			bResultIndirect1 = TRUE;

			// reduce SharedAsicSdram_Direct
			pHwDescUmInOut->uDirect.lSize -= pStartupParFwIn->lSizeIndirect1;

			// set SharedAsicSdram_Indirect1
			pHwDescUmInOut->uIndirect[1].lOffset	= pHwDescUmInOut->uDirect.lOffset + pHwDescUmInOut->uDirect.lSize;
			pHwDescUmInOut->uIndirect[1].lSize		= pStartupParFwIn->lSizeIndirect1;
			pHwDescUmInOut->uIndirect[1].lAdrAhb	= pHwDescUmInOut->uDirect.lAdrAhb + pHwDescUmInOut->uDirect.lSize;
		}
	}

	if	(bResultIndirect1)
		// success
	{
	UINT32	lAdrAhbPhyStart	= 0;
	BOOLEAN	bResultHif		= TRUE;

		// compute AhbAdr of physical start of AsicSdram
		lAdrAhbPhyStart = pHwDescUmInOut->uDirect.lAdrAhb - pHwDescUmInOut->uDirect.lOffset;

		if	(pStartupParFwIn->uHif.lSize != 0)
			// HIF exist
		{
			// preset
			bResultHif = FALSE;

			if	(	(pStartupParFwIn->uHif.lOffset									< pHwDescUmInOut->uDirect.lOffset)
				||	((pStartupParFwIn->uHif.lOffset + pStartupParFwIn->uHif.lSize)	> (pHwDescUmInOut->uDirect.lOffset + pHwDescUmInOut->uDirect.lSize)))
				// FwHif offset outside of SharedAsicSdram_Direct
			{
				// set ErrorString
				fnBuildString(	_TEXT(__FUNCTION__),
								_TEXT("(): Firmware: HIF outside of SharedAsicSdram_Direct!"),
								NULL,
								lDimErrorIn,
								sErrorOut);
			}
			else
			{
				// success
				bResultHif = TRUE;

				// set FwHif
				pHwDescUmInOut->uFwHif.uBase.pPtr	= (pHwDescUmInOut->uDirect.uBase.pPtr - pHwDescUmInOut->uDirect.lOffset) + pStartupParFwIn->uHif.lOffset;
				pHwDescUmInOut->uFwHif.lOffset		= pStartupParFwIn->uHif.lOffset;
				pHwDescUmInOut->uFwHif.lSize		= pStartupParFwIn->uHif.lSize;
				pHwDescUmInOut->uFwHif.lAdrAhb		= lAdrAhbPhyStart + pStartupParFwIn->uHif.lOffset;
			}
		}

		if	(bResultHif)
			// success
		{
			// success
			bResult = TRUE;

			if	(pStartupParFwIn->uFatal.lSize != 0)
				// FatalBuffer exist
			{
				// set FwFatal
				pHwDescUmInOut->uFwFatal.lOffset	= pStartupParFwIn->uFatal.lOffset;
				pHwDescUmInOut->uFwFatal.lSize		= pStartupParFwIn->uFatal.lSize;
				pHwDescUmInOut->uFwFatal.lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uFatal.lOffset;
			}

			if	(pStartupParFwIn->uTrace[0].lSize != 0)
				// TraceBuffer0 exist
			{
				pHwDescUmInOut->uFwTrace[0].lOffset	= pStartupParFwIn->uTrace[0].lOffset;
				pHwDescUmInOut->uFwTrace[0].lSize	= pStartupParFwIn->uTrace[0].lSize;
				pHwDescUmInOut->uFwTrace[0].lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uTrace[0].lOffset;
			}

			if	(pStartupParFwIn->uTrace[1].lSize != 0)
				// TraceBuffer1 exist
			{
				pHwDescUmInOut->uFwTrace[1].lOffset	= pStartupParFwIn->uTrace[1].lOffset;
				pHwDescUmInOut->uFwTrace[1].lSize	= pStartupParFwIn->uTrace[1].lSize;
				pHwDescUmInOut->uFwTrace[1].lAdrAhb	= lAdrAhbPhyStart + pStartupParFwIn->uTrace[1].lOffset;
			}
		}
	}

	return(bResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec400)
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec200)
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec400)
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Ertec200P)
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (Hera)
//************************************************************************

//************************************************************************
//  D e s c r i p t i o n :
//
//  set UserModePtr to APB-Peripherals (AM5728)
//************************************************************************


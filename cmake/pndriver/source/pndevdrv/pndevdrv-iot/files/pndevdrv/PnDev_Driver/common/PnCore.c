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
/*  F i l e               &F: PnCore.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Code for Core-IF (HostCore-AsicCore)
//
//****************************************************************************/

// Note if using VisualStudio:
//	A "C" module doesn't support precompiled header
//	-> turn it off at ProjectSettings >> C/C++ >> Precompiled Headers of  t h i s  file

//------------------------------------------------------------------------
//------------------------------------------------------------------------
#include "precomp.h"
#include "PnCore_Inc.h"														// all IncludeFiles of PnCore-IF
//------------------------------------------------------------------------


#ifdef PNDEV_TRACE															// required for Driver ToolChain
	#include "PnCore.tmh"
#endif

//************************************************************************
//  D e s c r i p t i o n :
//
//  check DebugMode
//************************************************************************

LSA_BOOL fnPnCore_CheckDebugMode(volatile LSA_UINT8* pAsicSdramIn)
{
LSA_BOOL			eResult		= LSA_FALSE;
uPNCORE_CONFIG_PAR*	pConfigPar	= LSA_NULL;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

	if	(pConfigPar->bDebugMode)
		// DebugMode
	{
		eResult = LSA_TRUE;
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	exchange StartupPar
//************************************************************************

LSA_BOOL fnPnCore_ExchangeStartupPar(	volatile LSA_UINT8*			pAsicSdramIn,
										uPNCORE_STARTUP_PAR_FW*		pStartupParFwIn,
										uPNCORE_STARTUP_PAR_DRIVER*	pStartupParDriverOut)
{
LSA_BOOL				eResult		= LSA_FALSE;
uPNCORE_CONFIG_PAR* 	pConfigPar 	= LSA_NULL;

	// preset
	eResult = LSA_TRUE;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);
	
	// temporary disabled if OS is linux (basic variant cant access this memory directly
	
	// provide StartupPar of UserFw for driver
	*((uPNCORE_STARTUP_PAR_FW*) &pConfigPar->uFw.uStartupPar) = *pStartupParFwIn;	
	
	// return StartupPar of driver
	*pStartupParDriverOut = *((uPNCORE_STARTUP_PAR_DRIVER*) &pConfigPar->uDriver.uStartupPar);

	// signal StartupPar of UserFw as valid
	pConfigPar->uFw.bStartupParValid = LSA_TRUE;
	
	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	exchange StartupState
//************************************************************************

LSA_BOOL fnPnCore_ExchangeStartupState(volatile LSA_UINT8* pAsicSdramIn)
{
LSA_BOOL			eResult		= LSA_FALSE;
uPNCORE_CONFIG_PAR*	pConfigPar	= LSA_NULL;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

	if	(pConfigPar->uFw.bStartupParValid)
		// StartupPar of UserFw valid
	{
		// signal Ready of UserFw to driver
		pConfigPar->uFw.bReady = LSA_TRUE;	

		if	(pConfigPar->uDriver.bReady)
			// driver ready
		{
			eResult = LSA_TRUE;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set own SignOfLife, checked by partner
//************************************************************************

void fnPnCore_SetSignOfLife(volatile LSA_UINT8*	pAsicSdramIn,
							volatile LSA_BOOL	bCallByUserFwIn)
{
uPNCORE_CONFIG_PAR* pConfigPar = LSA_NULL;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

	if	(bCallByUserFwIn)
		// call by UserFw
	{
		// update own SignOfLife
		pConfigPar->uSignOfLifeFw.bAlive = LSA_TRUE;
	}
	else	// call by driver
	{
		// update own SignOfLife
		pConfigPar->uSignOfLifeDriver.bAlive = LSA_TRUE;
	}
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check if SignOfLife of partner is set
//************************************************************************

LSA_BOOL fnPnCore_CheckSignOfLife(	volatile LSA_UINT8* pAsicSdramIn,
									volatile LSA_UINT32	lTimeCycle_10msecIn,
									volatile LSA_BOOL	bCallByUserFwIn)
{
LSA_BOOL				eResult							= LSA_FALSE;
uPNCORE_CONFIG_PAR*		pConfigPar						= LSA_NULL;
uPNCORE_SIGN_OF_LIFE*	pSignOfLife						= LSA_NULL;
LSA_UINT32				lTimeUpdateSignOfLife_10msec	= 0;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

	if	(pConfigPar->bDebugMode)
		// DebugMode
	{
		// success
		eResult = LSA_TRUE;
	}
	else if	(	(!pConfigPar->uDriver.bReady)
			||	(!pConfigPar->uFw.bReady))
			// UserFw or driver not ready
	{
		// success
		eResult = LSA_TRUE;
	}
	else
	{
		// get pointer to SignOfLife structure of partner		(SignOfLife of partner is checked!)
		// get guaranteed UpdateTime for SignOfLife of partner	(specified at StartupPar of partner!)
		{
			if	(bCallByUserFwIn)
				// call by UserFw
			{
				pSignOfLife						= &pConfigPar->uSignOfLifeDriver;
				lTimeUpdateSignOfLife_10msec	= pConfigPar->uDriver.uStartupPar.lTimeUpdateSignOfLife_10msec;
			}
			else
			{
				pSignOfLife						= &pConfigPar->uSignOfLifeFw;
				lTimeUpdateSignOfLife_10msec	= pConfigPar->uFw.uStartupPar.lTimeUpdateSignOfLife_10msec;
			}
		}

		if	(lTimeUpdateSignOfLife_10msec == 0xFFFFffff)
			// SignOfLife of partner disabled (-> don't check it)
		{
			// success
			eResult = LSA_TRUE;
		}
		else
		{
			if	(lTimeCycle_10msecIn == 0)
				// TimeInterval of cycle is 0
			{
				// increment for avoiding division by zero
				lTimeCycle_10msecIn++;
			}

			pSignOfLife->lCtrCycleCur++;

			if	(pSignOfLife->lCtrCycleActivePhase == 0)
				// StartupPhase
			{
			LSA_UINT32 lCtrCycleStartupPhase = 0;

				// Note:
				//	- user can specify a StartupPhase until SignOfLife of partner is checked first time

				// success
				eResult = LSA_TRUE;

				// compute CycleCtr for StartupPhase
				lCtrCycleStartupPhase = (pConfigPar->lTimeStartupSignOfLife_10msec / lTimeCycle_10msecIn) + 1;

				if	(pSignOfLife->lCtrCycleCur > lCtrCycleStartupPhase)
					// StartupPhase finished
				{
					pSignOfLife->lCtrCycleCur = 0;

					// compute CycleCtr for ActivePhase
					pSignOfLife->lCtrCycleActivePhase = (lTimeUpdateSignOfLife_10msec / lTimeCycle_10msecIn) + 1;
				}
			}
			else	// ActivePhase
			{
				if	(pSignOfLife->lCtrCycleCur < pSignOfLife->lCtrCycleActivePhase)
					// time for checking SignOfLife of partner not passed
				{
					// success
					eResult = LSA_TRUE;
				}
				else
				{
					pSignOfLife->lCtrCycleCur = 0;

					if	(!pSignOfLife->bAlive)
						// SignOfLife of partner not set
					{
						if	(lTimeUpdateSignOfLife_10msec < 200)
							// time for checking SignOfLife of partner < 2sec
						{
							// increase time for checking SignOfLife of partner to 2sec for avoiding a flood of error messages
							pSignOfLife->lCtrCycleActivePhase = (200 / lTimeCycle_10msecIn) + 1;
						}
					}
					else
					{
						// success
						eResult = LSA_TRUE;

						// reset SignOfLife of partner
						pSignOfLife->bAlive = LSA_FALSE;
					}
				}
			}
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	set FatalError
//************************************************************************

void fnPnCore_SetFatal(	volatile LSA_UINT8*	pAsicSdramIn,
						LSA_UINT32			lFatalIn)
{
uPNCORE_CONFIG_PAR* pConfigPar = LSA_NULL;

	// compute pointer to SharedAsicSdram_Config
	pConfigPar = (uPNCORE_CONFIG_PAR*) (pAsicSdramIn + PNCORE_SHARED_ASIC_SDRAM__OFFSET_CONFIG);

	// set FatalError
	pConfigPar->uFw.lFatal = lFatalIn;
}

//************************************************************************
//  D e s c r i p t i o n :
//
//	get IntPar of Host- or Asic-ICU
//************************************************************************

LSA_BOOL fnPnCore_GetIntPar(ePNCORE_ASIC		eAsicIn,
							ePNCORE_ICU			eIcuIn,
							uPNCORE_INT_PAR*	pIntParOut)
{
LSA_BOOL eResult = LSA_TRUE;

	switch	(eAsicIn)
			// AsicType
	{

        case ePNCORE_ASIC_I210:
        case ePNCORE_ASIC_I210IS:
        case ePNCORE_ASIC_I82574:
        case ePNCORE_ASIC_IX1000:
        {
            // ToDo:

            // interrupt masked if corresponding MaskReg-Bit=0
            pIntParOut->eMaskState = ePNCORE_MASK_STATE_INVALID;

            if (eIcuIn == ePNCORE_ICU_HOST)
                // HostCore-ICU
            {
                // use IRQ1 of PN-ICU3 (Host), use EventLow.Int22
                pIntParOut->lOffsetIrrReg = 0;
                pIntParOut->lOffsetAckReg = 0;
                pIntParOut->lOffsetMaskReg = 0;
                pIntParOut->lOffsetIrReg = 0;
                pIntParOut->lOffsetEoiReg = 0;
                pIntParOut->lBitShift = 0;
            }
            else	// AsicCore-ICU
            {
                // IRQ0 for EDDP interrupts -> use IRQ1 of PN-ICU2 (ARM926), use EventLow.Int23
                pIntParOut->lOffsetIrrReg = 0;
                pIntParOut->lOffsetAckReg = 0;
                pIntParOut->lOffsetMaskReg = 0;
                pIntParOut->lOffsetIrReg = 0;
                pIntParOut->lOffsetEoiReg = 0;
                pIntParOut->lBitShift = 0;
            }

            break;
        }

		default:
		{
			// error
			eResult = LSA_FALSE;

			break;
		}
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  mask INT_CoreIf at Host- or Asic-ICU
//************************************************************************

void fnPnCore_MaskInt(	volatile LSA_UINT8*	pIrtePnipIn,	
						uPNCORE_INT_PAR*	pIntParIn)
{
LSA_UINT32 lValue = 0;

	if	(pIntParIn->eMaskState == ePNCORE_MASK_STATE_0)
		// interrupt masked if Bit=0
	{
		// clear bit at current MaskValue
		lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetMaskReg) & (~(1u<<pIntParIn->lBitShift)));
	}
	else	// interrupt masked if Bit=1
	{
		// set bit at current MaskValue
		lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetMaskReg) | (1u<<pIntParIn->lBitShift));
	}

	// update MaskReg
	PNCORE_REG32_WRITE(	pIrtePnipIn, 
						pIntParIn->lOffsetMaskReg, 
						lValue);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  unmask INT_CoreIf at Host- or Asic-ICU
//************************************************************************

void fnPnCore_UnmaskInt(volatile LSA_UINT8*	pIrtePnipIn,	
						uPNCORE_INT_PAR*	pIntParIn)
{
LSA_UINT32 lValue = 0;

	if	(pIntParIn->eMaskState == ePNCORE_MASK_STATE_0)
		// interrupt masked if Bit=0
	{
		// set bit at current MaskValue
		lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetMaskReg) | (1u<<pIntParIn->lBitShift));
	}
	else	// interrupt masked if Bit=1
	{
		// clear bit at current MaskValue
		lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetMaskReg) & (~(1u<<pIntParIn->lBitShift)));
	}

	// update MaskReg
	PNCORE_REG32_WRITE(	pIrtePnipIn, 
						pIntParIn->lOffsetMaskReg, 
						lValue);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  ack INT_CoreIf at Host- or Asic-ICU
//************************************************************************

void fnPnCore_AckInt(	volatile LSA_UINT8*	pIrtePnipIn,	
						uPNCORE_INT_PAR*	pIntParIn)
{
	// update AckReg
	PNCORE_REG32_WRITE(	pIrtePnipIn, 
						pIntParIn->lOffsetAckReg, 
						(1u<<pIntParIn->lBitShift));	
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set INT_CoreIf at Host- or Asic-ICU
//************************************************************************

void fnPnCore_SetInt(	volatile LSA_UINT8*	pIrtePnipIn,	
						uPNCORE_INT_PAR*	pIntParIn)
{
	// set IrrReg
	PNCORE_REG32_WRITE(	pIrtePnipIn, 
						pIntParIn->lOffsetIrrReg, 
						(1u<<pIntParIn->lBitShift));	
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  set EOI and InactiveTime of Host- or Asic-ICU
//************************************************************************

void fnPnCore_SetEoi(	volatile LSA_UINT8*	pIrtePnipIn,	
						uPNCORE_INT_PAR*	pIntParIn,
						LSA_UINT32			lInactiveTime)
{
	// set EoiReg
	PNCORE_REG32_WRITE(	pIrtePnipIn, 
						pIntParIn->lOffsetEoiReg, 
						lInactiveTime);	
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check INT_CoreIf at IR-Reg of Host- or Asic-ICU
//************************************************************************

LSA_BOOL fnPnCore_CheckIntIr(	volatile LSA_UINT8*	pIrtePnipIn,	
								uPNCORE_INT_PAR*	pIntParIn)
{
LSA_BOOL	eResult	= LSA_FALSE;
LSA_UINT32	lValue	= 0;

	// get IrReg
	lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetIrReg));

	if	(lValue & (1u<<pIntParIn->lBitShift))
		// INT_CoreIf
	{
		eResult = LSA_TRUE;
	}

	return(eResult);
}

//************************************************************************
//  D e s c r i p t i o n :
//
//  check INT_CoreIf at IRR-Reg of Host- or Asic-ICU
//************************************************************************

LSA_BOOL fnPnCore_CheckIntIrr(	volatile LSA_UINT8*	pIrtePnipIn,	
								uPNCORE_INT_PAR*	pIntParIn)
{
LSA_BOOL	eResult	= LSA_FALSE;
LSA_UINT32	lValue	= 0;

	// get IrrReg
	lValue = (PNCORE_REG32_READ(pIrtePnipIn, pIntParIn->lOffsetIrrReg));

	if	(lValue & (1u<<pIntParIn->lBitShift))
		// INT_CoreIf
	{
		eResult = LSA_TRUE;
	}

	return(eResult);
}

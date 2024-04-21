/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_clp.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*  Functions of the GSY synchronization controllers                         */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  2005-12-14  P03.09.00.00_00.01.00.00 lrg  from file sys_sync.c(Ertec-EDD)*/
/*  2006-03-24  P03.10.00.00_00.02.01.02 lrg  clp_CalcNanosDiff(): Die       */
/*              Absolutzeit der aktuellen Offsetberechnung wird uebergeben.  */
/*  2006-06-02  P03.11.00.00_00.01.00.00 lrg  Zeiger auf externe Union       */
/*              CLP_SYNC_CONTROLLER_TYPE als Parameter anstatt der NicId     */
/*  2006-06-13  P03.12.00.00_00.01.01.02 lrg  DeltaTimeNs initialisieren,    */
/*              pCtrl check                                                  */
/*  2006-06-13  P03.12.00.00_00.02.01.02 lrg  Neue Rueckgabewerte fuer clp_  */
/*              SyncAlgorithm(): *pProportionalTerm,*pIntegralTerm. clp_Sync */
/*              Reset(): Use DriftIntervalNs to set Start-Value of ManipVar  */
/*  2006-07-26  P03.12.00.00_00.25.02.02 lrg  LSA_INT64                      */
/*  2007-02-07  P03.12.00.00_00.40.09.02 lrg  new: CLP_ERROR_TRACE_04        */
/*  2007-11-21  P04.00.00.00_00.60.01.02 lrg  CLP_CALC64                     */
/*  2008-01-11  P04.00.00.00_00.60.02.02 lrg  CLP_SYNC_VALUE_10Exp12_64BIT   */
/*  2008-01-23  P04.00.00.00_00.70.01.02 lrg  CLP_SYNC_VALUE_10Exp12_64BIT   */
/*              moved to clp_cfg.h/txt                                       */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-04  P04.01.00.00_00.03.01.02 lrg: CLP_SYNCID_MAX                 */
/*              todo: gleicher Regler fuer Clock und Time?                   */
/*  2010-07-16  P05.00.00.00_00.01.06.02 lrg: AP00697597                     */
/*              German comments translated to english                        */
/*  2010-12-22  P05.00.00.00_00.03.12.02 lrg: AP01017485 Index-Trace         */
/*  2013-01-08  P05.02.00.00_00.04.12.02 AP01384863, AP01376794: new Kp, Ki  */
/*              AP01482839: Warnings bei KM Generierung                      */
/*              AP01494442: Lint Warnings in gsy                             */
/*  2015-04-15  V06.00, Inc04, Feature FT1204141, Task 1234432               */
/*              eddp_GetSyncTime(), Ratetimeout flex. Ki anpassen            */
/*  2015-07-15  V06.00, Inc08, Feature 1227823, Task 1260584                 */
/*              [TSyncOverPTCP][GSY] Integrate CLP in GSY (->gsy_clp.c)      */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

/*===========================================================================*/
/*                                 module-id                                 */
/*===========================================================================*/
#define LTRC_ACT_MODUL_ID 	8		/* GSY_MODULE_ID_CLP */
#define GSY_MODULE_ID 		LTRC_ACT_MODUL_ID 

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "gsy_inc.h"
#include "gsy_int.h"

GSY_FILE_SYSTEM_EXTENSION(LTRC_ACT_MODUL_ID)

/*===========================================================================*/
/*                                 constants                                 */
/*===========================================================================*/
#define GSY_CLP_TIME_PARAM_A 200         // 1...1000

/*===========================================================================*/
/*                                 data                                      */
/*===========================================================================*/

/*****************************************************************************/
/* Internal function: gsy_CalcNanosDiff()                                    */
/* Calculation of the difference between two second/nanosec counters in ns   */
/*****************************************************************************/
static LSA_UINT32 gsy_CalcNanosDiff(
LSA_UINT32 T2Seconds,
LSA_UINT32 T2Nanosecs,
LSA_UINT32 T3Seconds,
LSA_UINT32 T3Nanosecs,
LSA_UINT16 *pRetVal)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

	/* Limit counters of nanoseconds to max. 1s
	 * by increasing the counters of seconds
	*/
	T2Seconds += T2Nanosecs / GSY_NANOS_PER_SECOND;
	T2Nanosecs = T2Nanosecs % GSY_NANOS_PER_SECOND;
	T3Seconds += T3Nanosecs / GSY_NANOS_PER_SECOND;
	T3Nanosecs = T3Nanosecs % GSY_NANOS_PER_SECOND;
	if (T3Seconds >= T2Seconds)
	{
		T2Seconds = T3Seconds - T2Seconds;
		if (T2Seconds > 0)
		{
			/* Jump of second: build difference to full second for 1. ns counter
			 * and add both ns counters, decrement distance of seconds
			*/
			T2Seconds--;
			T2Nanosecs = GSY_NANOS_PER_SECOND - T2Nanosecs;
			T2Nanosecs += T3Nanosecs;
			T2Seconds += T2Nanosecs / GSY_NANOS_PER_SECOND;
			T2Nanosecs = T2Nanosecs % GSY_NANOS_PER_SECOND;
			if ((T2Seconds < GSY_SECONDS_PER_DWORD)
			||  ((T2Seconds == GSY_SECONDS_PER_DWORD) && (T2Nanosecs <= GSY_MAX_NANOS)))
			{
				T2Seconds *= GSY_NANOS_PER_SECOND;
				T2Nanosecs += T2Seconds;
			}
			else
				RetVal = LSA_RET_ERR_PARAM;
		}
		else if (T3Nanosecs >= T2Nanosecs)
		{
			/* Both counters are in the same second: subtract ns counter
			*/
			T2Nanosecs = T3Nanosecs - T2Nanosecs;
		}
		else
			RetVal = LSA_RET_ERR_PARAM;
	}
	else
		RetVal = LSA_RET_ERR_PARAM;

	*pRetVal = RetVal;

	return(T2Nanosecs);
}
/*=============================================================================
 * function name:  gsy_SyncAlgoCycle
 *
 * function:       Function call to calculate control variable (cycle sync controller)
 *
 * parameters:     DeltaTimeSec      Seconds since last call
 *                                   (currently only zero supported)
 *                 DeltaTimeNs       Nanoseconds since last call
 *                                   (currently only values < 32ms supported)
 *				   OffsetNs          Current Offset in Nanoseconds to master clock
 *                                   (negativ: local clock is too slow)
 *                 pAdjustIntervalNs Pointer to return correction interval
 *                                   (Interval in ns that has to be corrected
 *                                    by +1/0/-1 Nanosecond)
 *                 pProportionalTerm Pointer to return the P-Part of the controller
 *                                    for diagnosis
 *                 pIntegralTerm     Pointer to return the I-Part of the controller
 *                                    for diagnosis
 *                 SyncId            Identifies structure in data union
 *                 pCtrl             Pointer to working data union
 *
 * return value: LSA_RET_OK           ok
 *               LSA_RET_ERR_PARAM    wrong DeltaTime
 *               LSA_RET_ERR_SYS_PATH wrong NicId, SyncId or
 *               LSA_RET_ERR_SEQUENCE not initialized by gsy_ClpReset()
 *===========================================================================*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgoCycle(
LSA_UINT32                    TimeSec,
LSA_UINT32                    TimeNs,
LSA_INT32                     OffsetNs,
LSA_INT32                     *pAdjustIntervalNs,
LSA_INT64                     *pProportionalTerm,
LSA_INT64                     *pIntegralTerm,
LSA_UINT8                     SyncId,
GSY_SYNC_CONTROLLER_TYPE      *pCtrl)
{
	LSA_INT64	Result;
	LSA_INT64	Summand_P;
	LSA_INT64	Summand_I;
	GSY_SYNC0_CONTROLLER_TYPE	*pCntr = LSA_NULL;
	LSA_UINT32                  DeltaTimeNs = 0;
	LSA_UINT16	RetVal = LSA_RET_OK;

	if (!LSA_HOST_PTR_ARE_EQUAL(pCtrl, LSA_NULL) && (SyncId <= GSY_SYNCID_MAX))
	{
		pCntr = &pCtrl->CycleCtrl;
		if (pCntr->State == GSY_SYNC_CTRL_STATE_INITIALIZED)
		{
			/* First call after gsy_ClpReset()
			*/
			pCntr->LastSec = TimeSec;
			pCntr->LastNs = TimeNs;
			DeltaTimeNs = 0;
			pCntr->State = GSY_SYNC_CTRL_STATE_RUNNING;
		}
		else if (pCntr->State == GSY_SYNC_CTRL_STATE_RUNNING)
		{
			/* Calculate difference in ns from actual to last offset time
			*/
			DeltaTimeNs = gsy_CalcNanosDiff(pCntr->LastSec, pCntr->LastNs, TimeSec, TimeNs, &RetVal);
			if (LSA_RET_OK != RetVal)
			{
				GSY_ERROR_TRACE_04(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_SyncAlgorithm() invalid time difference: LastSec=%u LastNs=%u ActSec=%u ActNs=%u",
									pCntr->LastSec, pCntr->LastNs, TimeSec, TimeNs);
			}

			pCntr->LastSec = TimeSec;
			pCntr->LastNs = TimeNs;
		}
		else
		{
			RetVal = LSA_RET_ERR_SEQUENCE;
			GSY_ERROR_TRACE_01(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_SyncAlgorithm() invalid state=0x%08x",pCntr->State);
		}
	}
	else
	{
		RetVal = LSA_RET_ERR_PARAM;
		GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_SyncAlgorithm() invalid parameter: SyncId=%02x pCtrl=%x",SyncId,pCtrl);
	}

	if (LSA_RET_OK == RetVal)
	{
        GSY_IS_VALID_PTR(pCntr);

        /* ++++++++++++++++++++++++++++++++++++++
		 *           Load InputValues
		*/
		pCntr->PhaseError_ns = OffsetNs;

		/* Calculate DeltaTime
		*/
#ifdef GSY_MESSAGE
		/*  TODO: in Future DeltaTime_ms should be Drift-Corrected
		*/
#endif /* GSY_MESSAGE */
		pCntr->DeltaTime_ms  =  (LSA_INT32)(DeltaTimeNs / 1000000L);

		/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		 * Now calculate the new manipulation variable by using following Function:
		 *
		 * pCntr->ManipVar_ppt    =
		 *      (  (pCntr->Kp_1perSec * 1000UL) * (PhaseError_ns - pCntr->LastPhaseError_ns ) ) +  // Summand_P
		 *      (   pCntr->Ki_1perSecSec *  pCntr->LastPhaseError_ns * pCntr->DeltaTime_ms    ) +  // Summand_I
		 *          pCntr->ManipVar_ppt ;
		*/
		//AP01494442 Lint 647: Suspicious truncation: cast
		Summand_P = (LSA_INT64)pCntr->Kp_1perSec * 1000 * (pCntr->PhaseError_ns - pCntr->LastPhaseError_ns);    //lint !e776 Possible truncation of addition is impossible, HM 13.06.2016
		Summand_I = (LSA_INT64)pCntr->Ki_1perSecSec * pCntr->DeltaTime_ms * pCntr->LastPhaseError_ns;
		pCntr->ManipVar_ppt += Summand_P + Summand_I;

		/* ++++++++++++++++++++++++++++++++++++++
		 *    Build Reciprocal of ManipVar_ppt
		*/
		if (pCntr->ManipVar_ppt == 0)
		{
			pCntr->AdjustIntervalNs = 0;
		}
		else
		{
			/* Check Minimum and Maximum Limits for AdjustDivisor to avoid overflows.
			 * The Divisor should be inside of [GSY_LIMIT_LSA_INT32_MIN  ; GSY_LIMIT_LSA_INT32_MAX]
			*/
			Result = GSY_SYNC_VALUE_10Exp12_64BIT / pCntr->ManipVar_ppt;
			if (Result > GSY_LIMIT_LSA_INT32_MAX)
				Result = GSY_LIMIT_LSA_INT32_MAX;
			else if (Result < GSY_LIMIT_LSA_INT32_MIN)
				Result = GSY_LIMIT_LSA_INT32_MIN;
			pCntr->AdjustIntervalNs = (LSA_UINT32)(Result * -1L);
		}
		/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		 *  Map internal P- and I- Terms to return-Values (for diagnosis only)
		*/
		*pProportionalTerm = Summand_P;
		*pIntegralTerm = Summand_I;

		/* ++++++++++++++++++++++++++++++++++++++
		 *         Store State_Values
		*/
		pCntr->LastPhaseError_ns = pCntr->PhaseError_ns;

		/* ++++++++++++++++++++++++++++++++++++++
		 *  Map AdjustInterval to return Value
		*/
		*pAdjustIntervalNs = (LSA_INT32)(pCntr->AdjustIntervalNs);
	}

/*
	GSY_TRACE("CLP",LSA_TRACE_LEVEL_NOTE,"LastSec=%u LastNs=%u TimeSec=%u TimeNs=%u Delta=%u Offset=%d Adjust=%d RetVal=0x%x",
	LastSec, LastNs, TimeSec, TimeNs, DeltaTimeNs, OffsetNs, pCntr->AdjustIntervalNs, RetVal);
*/
	return(RetVal);
}


LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_SyncAlgoTime(
LSA_BOOL                      StateSync,
LSA_INT32                     Offset_ns,
LSA_INT32                     DriftInterval_ns,
LSA_UINT16                    SyncInterval_ms,
LSA_INT32                     *pAdjustInterval,	//Oufput
GSY_SYNC_CONTROLLER_TYPE      *pCtrl)
{
	GSY_SYNC1_CONTROLLER_TYPE *pCntr = &pCtrl->TimeCtrl;
	LSA_INT64	SyncInterval_ns = (LSA_INT64)SyncInterval_ms * 1000000LL;
	LSA_INT64	OffsetEstimated_fs;
	LSA_INT64	TmpAdjustInterval_ns;
	LSA_INT64	Superpose;
	LSA_INT64	Param_Ax1000;
	LSA_UINT16	RetVal = LSA_RET_OK;

	if (StateSync) 
	{
//		Param_Ax1000 = (LSA_INT64)pCntr->Param_Ax1000;
		if (pCntr->Param_X != 0)
		{
			pCntr->Param_X = pCntr->Param_X * 9 / 10;
		}
	}
	else
	{
		pCntr->Param_X = 1000 - pCntr->Param_Ax1000;
//		Param_Ax1000 = 1000LL;
	}
	Param_Ax1000 = pCntr->Param_Ax1000 + pCntr->Param_X;    //lint !e776 Possible truncation of addition is impossible, HM 13.06.2016

	// Compute estimated offset in femto seconds to minimize rounding effects
	// param_a is 1...1000 instead of 0...1 and Offset_ns is local time - master time
	OffsetEstimated_fs = Param_Ax1000 * 1000LL * (LSA_INT64)(-Offset_ns);
	if (0 == OffsetEstimated_fs)
	{
		Superpose = (LSA_INT64)DriftInterval_ns;
	}
	else
	{
		TmpAdjustInterval_ns = (SyncInterval_ns * 1000000LL) / OffsetEstimated_fs;
		Superpose = (LSA_INT64)DriftInterval_ns * TmpAdjustInterval_ns / ((LSA_INT64)DriftInterval_ns + TmpAdjustInterval_ns);
	}

	if ((Superpose <= 2147483647LL) && (Superpose >= -2147483648LL))
	{
		*pAdjustInterval = (LSA_INT32)Superpose;
	}
	else
	{
		RetVal = LSA_RET_ERR_PARAM;
	}
	return(RetVal);
}

/*=============================================================================
 * function name:  gsy_ClpReset
 *
 * function:       Function call to Reset state variable(s) of Sync-Controller
 *
 *
 * parameters:     see documentation
 *
 * return value: LSA_RET_OK         ok
 *               LSA_RET_ERR_PARAM  wrong pSys or SyncId
 *===========================================================================*/
LSA_UINT16 GSY_LOCAL_FCT_ATTR gsy_ClpReset(
LSA_INT32                     DriftIntervalNs,
LSA_UINT8                     SyncId,
GSY_SYNC_CONTROLLER_TYPE    * pCtrl)
{
	LSA_UINT16	RetVal = LSA_RET_OK;

//	if (!LSA_HOST_PTR_ARE_EQUAL(pCtrl, LSA_NULL) && (SyncId <= GSY_SYNCID_MAX))
	if (!LSA_HOST_PTR_ARE_EQUAL(pCtrl, LSA_NULL) && (SyncId == 0))
	{
		GSY_SYNC0_CONTROLLER_TYPE * pCntr = &pCtrl->CycleCtrl;
		if (0 == SyncId)       //lint !e774 Boolean within 'if' always evaluates to True, keep code unchanged for the GSY_SYNCID_MAX version, HM 20.05.2016
		{
			pCntr->Kp_1perSec = GSY_SYNC_CONTROL_PARAM_P;
			pCntr->Ki_1perSecSec = GSY_SYNC_CONTROL_PARAM_I;
		}
		else
		{
			pCntr->Kp_1perSec = GSY_SYNC_CONTROL_PARAM_P_1;
			pCntr->Ki_1perSecSec = GSY_SYNC_CONTROL_PARAM_I_1;
		}

		/* Input Values
		*/
		pCntr->DeltaTime_ms = 0;
		pCntr->PhaseError_ns = 0;

		/* Output Value
		*/
		if(DriftIntervalNs != 0)
		{
			/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			 *  Use DriftIntervalNs to set Start-Value of ManipVar
			 *  ManipVar_ppt = (10^12) / ( - DriftIntervalNs )
			*/
			//AP01494442 Lint 647: Suspicious truncation: cast
			pCntr->ManipVar_ppt = GSY_SYNC_VALUE_10Exp12_64BIT / ((-1)*(LSA_INT64)DriftIntervalNs);
		}
		else
		{
			pCntr->ManipVar_ppt = 0;
		}

		/* Inner State-Value
		*/
		pCntr->LastPhaseError_ns = 0;
		pCntr->LastSec = 0;
		pCntr->LastNs = 0;
		pCntr->AdjustIntervalNs = 0;
		pCntr->State = GSY_SYNC_CTRL_STATE_INITIALIZED;
	}
	else if (!LSA_HOST_PTR_ARE_EQUAL(pCtrl, LSA_NULL) && (SyncId <= GSY_SYNCID_MAX))
	{
		GSY_SYNC1_CONTROLLER_TYPE * pCntr = &pCtrl->TimeCtrl;
		pCntr->Param_Ax1000 = GSY_CLP_TIME_PARAM_A;
		pCntr->Param_X = 1000 - GSY_CLP_TIME_PARAM_A;
	}
	else
	{
		RetVal = LSA_RET_ERR_PARAM;
		GSY_ERROR_TRACE_02(0, LSA_TRACE_LEVEL_ERROR,"*** gsy_ClpReset() invalid parameter: SyncId=%u pCtrl=%x",SyncId,pCtrl);
	}

	return(RetVal);
}

/****************************************************************************/
/*     END OF FILE:      gsy_clp.c                                          */
/****************************************************************************/

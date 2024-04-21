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
/*  C o m p o n e n t     &C: PSI (PNIO Stack Interface)                :C&  */
/*                                                                           */
/*  F i l e               &F: gsy_syncalgorithm.c                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of GSY using PSI.                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1801
#define PSI_MODULE_ID       1801 /* PSI_MODULE_ID_GSY_SYNCALGORITHM */

#include "psi_int.h"

#if ((PSI_CFG_USE_GSY == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION( PSI_MODULE_ID )

/*----------------------------------------------------------------------------*/
/*---- EDDI ------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_UINT16 psi_gsy_sync_get_EDDI(
	LSA_UINT16        * pTimeEpoch, 
	LSA_UINT32        * pSeconds, 
	LSA_UINT32        * pNanoSeconds, 
	LSA_UINT32        * pTimeStamp,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr )
{
	LSA_UINT16          rc   = LSA_RET_ERR_PARAM;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    #if ( PSI_CFG_USE_EDDI == 1 )

	switch (SyncId)
	{
	    case GSY_SYNCID_CLOCK:
	    case GSY_SYNCID_TIME:
		{
			EDDI_SYNC_GET_SET_TYPE  tmp;
			EDDI_HANDLE             hDDB = psi_hd_get_hDDB(pSys->hd_nr);

			rc = eddi_SyncGet(hDDB, &tmp, LSA_NULL);

			if (rc != EDD_STS_OK)
			{
				PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_WARN, "psi_gsy_sync_get_EDDI(): rc(%u) from eddi_SyncGet() not ok", rc );

				*pTimeEpoch   = 0;
				*pSeconds     = 0;
				*pNanoSeconds = 0;
				*pTimeStamp   = 0;

				rc = LSA_RET_ERR_PARAM; // translate error code
			}
			else 
			{

				*pTimeEpoch   = 0;
				*pSeconds     = tmp.PTCPSeconds;
				*pNanoSeconds = tmp.PTCPNanoSeconds;
				*pTimeStamp   = tmp.CycleBeginTimeStampInTicks; // nicht umrechnen

				rc = LSA_RET_OK;
			}
		}
		break;

        default:
		{
			rc = LSA_RET_ERR_PARAM;

			PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_EDDI(): invalid SYNC_ID received" );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
        }
		break;
	}

    #else

	LSA_UNUSED_ARG( pTimeEpoch );
	LSA_UNUSED_ARG( pSeconds );
	LSA_UNUSED_ARG( pNanoSeconds );
	LSA_UNUSED_ARG( pTimeStamp );
	LSA_UNUSED_ARG( SyncId );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_get_ext_pll_in_EDDI(
	LSA_UINT32        * pTimeStampPLL, 
	LSA_UINT32        * pSeconds, 
	LSA_UINT32        * pNanoSeconds, 
	LSA_UINT32        * pTimeStamp,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr )
{
	LSA_UINT16          rc   = LSA_RET_ERR_PARAM;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    #if (PSI_CFG_USE_EDDI == 1)

	if (SyncId == GSY_SYNCID_CLOCK)
	{
		LSA_RESULT              retEddi;
		EDDI_SYNC_GET_SET_TYPE  syncGet = {0};
		EDDI_HANDLE             hDDB    = psi_hd_get_hDDB(pSys->hd_nr);

		retEddi = eddi_SyncGet(hDDB, &syncGet, pTimeStampPLL);

		PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_gsy_sync_get_ext_pll_in_EDDI(): GSY_SYNC_GET_EXT_PLL_IN eddi_SyncGet(), EDDI hDDB(0x%08x) res(%u)",
			hDDB, retEddi );

		if (EDD_STS_OK != retEddi)
		{
			rc = GSY_RET_ERR_PARAM;
		}
		else
		{
			*pSeconds = syncGet.PTCPSeconds;
			*pNanoSeconds = syncGet.PTCPNanoSeconds;
			*pTimeStamp = syncGet.CycleBeginTimeStampInTicks;

			rc = LSA_RET_OK;
		}

		PSI_SYSTEM_TRACE_08( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_gsy_sync_get_ext_pll_in_EDDI(): GSY_SYNC_GET_EXT_PLL_IN done, RetVal(%#x) SyncId(0x%02x) TimestampPLL(%u) Timestamp(%u) Seconds(%u) Nanoseconds(%u) CycleHigh(%#x) CycleLow(%#x)",
			rc, 
			SyncId, 
			*pTimeStampPLL, 
			*pTimeStamp, 
			*pSeconds, 
			*pNanoSeconds, 
			syncGet.CycleCounterHigh, 
			syncGet.CycleCounterLow );
	}

    #else

	LSA_UNUSED_ARG( pTimeStampPLL );
	LSA_UNUSED_ARG( pSeconds );
	LSA_UNUSED_ARG( pNanoSeconds );
	LSA_UNUSED_ARG( pTimeStamp );
	LSA_UNUSED_ARG( SyncId );
	LSA_UNUSED_ARG( sys_ptr );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_ext_pll_in_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_adjust_EDDI(
	LSA_INT32           AdjustInterval,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16          rc   = LSA_RET_ERR_PARAM;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    #if (PSI_CFG_USE_EDDI == 1)

	if ((SyncId == GSY_SYNCID_CLOCK) || (SyncId == GSY_SYNCID_TIME))
	{
		EDDI_HANDLE     hDDB = psi_hd_get_hDDB(pSys->hd_nr);

		rc = eddi_SyncAdjust(hDDB, AdjustInterval);

		if (rc != EDD_STS_OK)
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_adjust_EDDI(): rc(%u) from eddi_SyncAdjust(), AdjustInterval(%u)",
				rc, AdjustInterval );

			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
	}
	else 
	{
		PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_adjust_EDDI(): this SyncID (%u) is not yet implemented",
			SyncId );
		PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
	}

    #else

    LSA_UNUSED_ARG( SyncId );
	LSA_UNUSED_ARG( sys_ptr );
	LSA_UNUSED_ARG( AdjustInterval );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_adjust_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16  psi_gsy_rate_set_EDDI (
	LSA_INT32           RateInterval,
    LSA_UINT8           RateValid,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16          rc   = LSA_RET_ERR_PARAM;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	LSA_UNUSED_ARG(RateInterval);
	LSA_UNUSED_ARG(RateValid);
	LSA_UNUSED_ARG(SyncId);

    #if (PSI_CFG_USE_EDDI == 1)

    if (SyncId == GSY_SYNCID_CLOCK)
	{
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_rate_set_EDDI(): a bug: GSY does it via EDD_SRV_SYNC_SET_RATE" );
		PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
	}
	else if (SyncId == GSY_SYNCID_TIME)
	{
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_gsy_rate_set_EDDI(): dummy for SyncId 1" );

		rc = LSA_RET_OK;
	}
	else
	{
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_rate_set_EDDI(): this SyncID is not yet implemented" );
		PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
	}

    #else

    PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_rate_set_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT32 psi_gsy_get_timestamp_EDDI(
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT32          timestamp = 0;
	PSI_SYS_PTR_TYPE    pSys      = (PSI_SYS_PTR_TYPE)sys_ptr;

    #if (PSI_CFG_USE_EDDI == 1)

	EDDI_HANDLE     hDDB = psi_hd_get_hDDB(pSys->hd_nr);

	if (hDDB != LSA_NULL)
	{
		LSA_UINT16  rc = eddi_GetTimeStamp(hDDB, &timestamp);
		
		if (rc != EDD_STS_OK)
		{
			PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_timestamp_EDDI(): rc(%u) from eddi_GetTimeStamp() not ok", rc );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
	}
	else 
	{
		PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_timestamp_EDDI(): hDDB not valid" );
		PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
	}

    #else

    PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_timestamp_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return timestamp;
}

/*----------------------------------------------------------------------------*/
static LSA_INT32 psi_gsy_get_ext_pll_offset_EDDI(
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_INT32           ext_pll_offset;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    #if (PSI_CFG_USE_EDDI == 1)

    if (SyncId == GSY_SYNCID_CLOCK)
	{
		LSA_INT32       realOffsetNs;
		EDDI_HANDLE     hDDB    = psi_hd_get_hDDB(pSys->hd_nr);
		LSA_UINT16      retEdd  = eddi_SyncGetRealOffsetNs(hDDB, &realOffsetNs);

		if (EDD_STS_OK != retEdd)
		{
			if (retEdd != EDD_STS_ERR_HW)
			{
				PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_ext_pll_offset_EDDI(): rc(%u) from eddi_SyncGetRealOffsetNs() not ok",
					retEdd );
				PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
			}
			else  /* "PLL not active" or "no signal" */
			{
				/*
				 * edd_SyncGetRealOffsetNs() maps EDD_CYC_NO_EXT_PLL_IN_SIGNAL to EDD_STS_ERR_HW
				 * and we map it again to 0x7FFFFFFF which is "accidentally" the same as EDD_CYC_NO_EXT_PLL_IN_SIGNAL
				 */

				realOffsetNs = 0x7FFFFFFF; /* "no signal" */
			}
		}

		ext_pll_offset = realOffsetNs; /* hopefully never 0x7FFFFFFF */
	}
	else 
	{
		ext_pll_offset = 0x7FFFFFFF; /* "no signal" */
	}

    #else

    ext_pll_offset = 0;

	LSA_UNUSED_ARG( SyncId );
	LSA_UNUSED_ARG( sys_ptr );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_ext_pll_offset_EDDI(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );

    #endif

	return ext_pll_offset;
}

/*----------------------------------------------------------------------------*/
/*---- EDDP ------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_UINT16 psi_gsy_sync_get_EDDP(
	LSA_UINT16        * pTimeEpoch, 
	LSA_UINT32        * pSeconds, 
	LSA_UINT32        * pNanoSeconds, 
	LSA_UINT32        * pTimeStamp,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16  rc;

	LSA_UNUSED_ARG(SyncId);
	LSA_UNUSED_ARG(sys_ptr);

    #if (PSI_CFG_USE_EDDP == 1)
	*pTimeEpoch   = 0;
	*pSeconds     = 0;
	*pNanoSeconds = 0;
	*pTimeStamp   = 0;

	rc = LSA_RET_OK;
    #else
	LSA_UNUSED_ARG( pTimeEpoch );
	LSA_UNUSED_ARG( pSeconds );
	LSA_UNUSED_ARG( pNanoSeconds );
	LSA_UNUSED_ARG( pTimeStamp );
    rc = LSA_RET_ERR_PARAM;

    PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_get_ext_pll_in_EDDP(
	LSA_UINT32       *pTimeStampPLL, 
	LSA_UINT32       *pSeconds, 
	LSA_UINT32       *pNanoSeconds, 
	LSA_UINT32       *pTimeStamp,
	LSA_UINT8        SyncId,
	LSA_SYS_PTR_TYPE sys_ptr )
{
	LSA_UINT16          rc   = LSA_RET_OK;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	LSA_UNUSED_ARG( pTimeStampPLL );
	LSA_UNUSED_ARG( pSeconds );
	LSA_UNUSED_ARG( pNanoSeconds );
	LSA_UNUSED_ARG( pTimeStamp );
	LSA_UNUSED_ARG( SyncId );

    #if !(LSA_COMP_ID_EDDP == 1)
	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_ext_pll_in_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_adjust_EDDP(
	LSA_INT32           AdjustInterval,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
    LSA_UINT16  rc;

	LSA_UNUSED_ARG(AdjustInterval);
	LSA_UNUSED_ARG(SyncId);
	LSA_UNUSED_ARG(sys_ptr);

    #if (PSI_CFG_USE_EDDP == 1)
	rc = LSA_RET_OK;
    #else
    rc = LSA_RET_ERR_PARAM;
    PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_adjust_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16  psi_gsy_rate_set_EDDP (
	LSA_INT32          RateInterval,
    LSA_UINT8          RateValid,
    LSA_UINT8          SyncId,
	LSA_SYS_PTR_TYPE   sys_ptr )
{
	LSA_UINT16  rc;

	LSA_UNUSED_ARG(RateInterval);
	LSA_UNUSED_ARG(RateValid);
	LSA_UNUSED_ARG(SyncId);
	LSA_UNUSED_ARG(sys_ptr);

    #if (PSI_CFG_USE_EDDP == 1)
	rc = LSA_RET_OK;
    #else
    rc = LSA_RET_ERR_PARAM;
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_gsy_rate_set_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT32 psi_gsy_get_timestamp_EDDP(
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT32  timestamp = 0;

	LSA_UNUSED_ARG(sys_ptr);

    #if !(PSI_CFG_USE_EDDP == 1)
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_timestamp_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	return timestamp;
}

/*----------------------------------------------------------------------------*/
static LSA_INT32 psi_gsy_get_ext_pll_offset_EDDP(
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_INT32   ext_pll_offset = 0;

	LSA_UNUSED_ARG(SyncId);
	LSA_UNUSED_ARG(sys_ptr);

    #if !(PSI_CFG_USE_EDDP == 1)
	PSI_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_ext_pll_offset_EDDP(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	return ext_pll_offset;
}

/*----------------------------------------------------------------------------*/
/*---- EDDS ------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

static LSA_UINT16 psi_gsy_sync_get_EDDS(
	LSA_UINT16        * pTimeEpoch, 
	LSA_UINT32        * pSeconds, 
	LSA_UINT32        * pNanoSeconds, 
	LSA_UINT32        * pTimeStamp,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16          rc   = LSA_RET_ERR_PARAM;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	LSA_UNUSED_ARG(SyncId);

    #if (LSA_COMP_ID_EDDS == 1)
	*pTimeEpoch   = 0;
	*pSeconds     = 0;
	*pNanoSeconds = 0;
	*pTimeStamp   = 0;
	rc = LSA_RET_OK;
    #else
	LSA_UNUSED_ARG( pTimeEpoch );
	LSA_UNUSED_ARG( pSeconds );
	LSA_UNUSED_ARG( pNanoSeconds );
	LSA_UNUSED_ARG( pTimeStamp );

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_get_ext_pll_in_EDDS(
	LSA_UINT32        * pTimeStampPLL, 
	LSA_UINT32        * pSeconds, 
	LSA_UINT32        * pNanoSeconds, 
	LSA_UINT32        * pTimeStamp,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16          rc   = LSA_RET_OK;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	LSA_UNUSED_ARG(pTimeStampPLL);
	LSA_UNUSED_ARG(pSeconds);
	LSA_UNUSED_ARG(pNanoSeconds);
	LSA_UNUSED_ARG(pTimeStamp);
	LSA_UNUSED_ARG(SyncId);

    #if !(LSA_COMP_ID_EDDS == 1)
	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_get_ext_pll_in_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    //lint --e(527) Unreachable code at token 'return', but compiler says all control paths must return a value
    #endif

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16 psi_gsy_sync_adjust_EDDS(
	LSA_INT32           AdjustInterval,
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT16  rc;

    #if (PSI_CFG_USE_EDDS == 1)
	rc = LSA_RET_OK;
    #else
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    rc = LSA_RET_ERR_PARAM;
	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_sync_adjust_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	LSA_UNUSED_ARG(AdjustInterval);
	LSA_UNUSED_ARG(sys_ptr);
	LSA_UNUSED_ARG(SyncId);

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT16  psi_gsy_rate_set_EDDS (
	LSA_INT32          RateInterval,
    LSA_UINT8          RateValid,
    LSA_UINT8          SyncId,
	LSA_SYS_PTR_TYPE   sys_ptr )
{
	LSA_UINT16  rc;

    #if (PSI_CFG_USE_EDDS == 1)
	rc = LSA_RET_OK;
    #else
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    rc = LSA_RET_ERR_PARAM;
	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_rate_set_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	LSA_UNUSED_ARG(sys_ptr);
	LSA_UNUSED_ARG(RateInterval);
	LSA_UNUSED_ARG(RateValid);
	LSA_UNUSED_ARG(SyncId);

	return rc;
}

/*----------------------------------------------------------------------------*/
static LSA_UINT32 psi_gsy_get_timestamp_EDDS(
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT32  timestamp = 0;

    #if !(PSI_CFG_USE_EDDS == 1)
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_timestamp_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	LSA_UNUSED_ARG(sys_ptr);

	return timestamp;
}

/*----------------------------------------------------------------------------*/
static LSA_INT32 psi_gsy_get_ext_pll_offset_EDDS(
	LSA_UINT8           SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_INT32   ext_pll_offset = 0;

    #if !(PSI_CFG_USE_EDDS == 1)
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_SYSTEM_TRACE_00( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_gsy_get_ext_pll_offset_EDDS(): wrong EDDx COMP ID in SYS_PTR received" );
	PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
    #endif

	LSA_UNUSED_ARG(SyncId);
	LSA_UNUSED_ARG(sys_ptr);

	return ext_pll_offset;
}

/*===========================================================================*/
/*===========================================================================*/

#ifndef GSY_SYNC_GET
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_GET(
	LSA_UINT16	GSY_LOCAL_MEM_ATTR *	pTimeEpoch,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pSeconds,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pNanoSeconds,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR *	pTimeStamp,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE                    sys_ptr)
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			rc = psi_gsy_sync_get_EDDI( pTimeEpoch, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			rc = psi_gsy_sync_get_EDDP( pTimeEpoch, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			rc = psi_gsy_sync_get_EDDS( pTimeEpoch, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        default:
		{
			rc = LSA_RET_ERR_PARAM;

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_SYNC_GET(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return rc;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
#ifndef GSY_SYNC_GET_EXT_PLL_IN
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_GET_EXT_PLL_IN(
	LSA_UINT32  GSY_LOCAL_MEM_ATTR  *pTimeStampPLL,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR  *pSeconds,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR  *pNanoSeconds,
	LSA_UINT32  GSY_LOCAL_MEM_ATTR  *pTimeStamp,
	LSA_UINT8                        SyncId,
	LSA_SYS_PTR_TYPE                 sys_ptr )
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			rc = psi_gsy_sync_get_ext_pll_in_EDDI( pTimeStampPLL, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			rc = psi_gsy_sync_get_ext_pll_in_EDDP( pTimeStampPLL, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			rc = psi_gsy_sync_get_ext_pll_in_EDDS( pTimeStampPLL, pSeconds, pNanoSeconds, pTimeStamp, SyncId, sys_ptr );
		}
		break;

        default:
		{
			rc = LSA_RET_ERR_PARAM;

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_SYNC_GET_EXT_PLL_IN(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return rc;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef GSY_SYNC_ADJUST
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_SYNC_ADJUST(
	LSA_INT32							AdjustInterval,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr )
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			rc = psi_gsy_sync_adjust_EDDI( AdjustInterval, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			rc = psi_gsy_sync_adjust_EDDP( AdjustInterval, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			rc = psi_gsy_sync_adjust_EDDS( AdjustInterval, SyncId, sys_ptr );
		}
		break;

        default:
		{
			rc = LSA_RET_ERR_PARAM;

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_SYNC_ADJUST(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return rc;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/
#ifndef GSY_RATE_SET
LSA_UINT16  GSY_SYSTEM_OUT_FCT_ATTR  GSY_RATE_SET(
	LSA_INT32							RateInterval,
    LSA_UINT8							RateValid,
	LSA_UINT8							SyncId,
	LSA_SYS_PTR_TYPE					sys_ptr )
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			rc = psi_gsy_rate_set_EDDI( RateInterval, RateValid, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			rc = psi_gsy_rate_set_EDDP( RateInterval, RateValid, SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			rc = psi_gsy_rate_set_EDDS( RateInterval, RateValid, SyncId, sys_ptr );
		}
		break;

        default:
		{
			rc = LSA_RET_ERR_PARAM;

			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_RATE_SET(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return rc;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef GSY_GET_TIMESTAMP
LSA_UINT32  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_TIMESTAMP(
	LSA_SYS_PTR_TYPE    sys_ptr)
{
	LSA_UINT32          timestamp = 0;
	PSI_SYS_PTR_TYPE    pSys      = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			timestamp = psi_gsy_get_timestamp_EDDI( sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			timestamp = psi_gsy_get_timestamp_EDDP( sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			timestamp = psi_gsy_get_timestamp_EDDS( sys_ptr );
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_GET_TIMESTAMP(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return timestamp;
}
#else
#error "by design a function!"
#endif

/*===========================================================================*/

#ifndef GSY_GET_EXT_PLL_OFFSET
LSA_INT32  GSY_SYSTEM_OUT_FCT_ATTR  GSY_GET_EXT_PLL_OFFSET(
	LSA_UINT8			SyncId,
	LSA_SYS_PTR_TYPE    sys_ptr )
{
	LSA_INT32           ext_pll_offset = 0;
	PSI_SYS_PTR_TYPE    pSys           = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT(pSys);

	switch (pSys->comp_id_edd)
	{
	    case LSA_COMP_ID_EDDI:
		{
			ext_pll_offset = psi_gsy_get_ext_pll_offset_EDDI( SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDP:
		{
			ext_pll_offset = psi_gsy_get_ext_pll_offset_EDDP( SyncId, sys_ptr );
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			ext_pll_offset = psi_gsy_get_ext_pll_offset_EDDS( SyncId, sys_ptr );
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "GSY_GET_EXT_PLL_OFFSET(): wrong EDDx COMP-ID(%u/%#x) in SYS_PTR received",
				pSys->comp_id_edd, pSys->comp_id_edd );
			PSI_FATAL_COMP( LSA_COMP_ID_GSY, PSI_MODULE_ID, 0 );
		}
		break;
	}

	return ext_pll_offset;
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_GSY && PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

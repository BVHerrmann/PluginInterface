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
/*  F i l e               &F: cm_cfg.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of CM using the PSI framework.             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   1200
#define PSI_MODULE_ID       1200 /* PSI_MODULE_ID_CM_CFG */

#include "psi_int.h"

#if (PSI_CFG_USE_HD_COMP == 1)

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/

static CM_INIT_TYPE cm_init_param  = {0,0};

/*----------------------------------------------------------------------------*/

void cm_psi_startstop( int start )
{
    LSA_UINT16 rc;

	if ( start )
	{
		cm_init_param.oneshot_lsa_time_base   = LSA_TIME_BASE_100MS;
		cm_init_param.oneshot_lsa_time_factor = 1;

		rc = cm_init( &cm_init_param );
		PSI_ASSERT(rc == LSA_RET_OK);
	}
	else
	{
		rc = cm_undo_init();
		PSI_ASSERT( rc == LSA_RET_OK );
	}
}

/*------------------------------------------------------------------------------
//	System Layer (prototypes in cm_sys.h)
//----------------------------------------------------------------------------*/

#ifndef CM_GET_PATH_INFO
LSA_VOID CM_GET_PATH_INFO(
	LSA_UINT16          *  ret_val_ptr,
	LSA_SYS_PTR_TYPE    *  sys_ptr_ptr,
	CM_DETAIL_PTR_TYPE  *  detail_ptr_ptr,
	LSA_SYS_PATH_TYPE                        path )
{
	*ret_val_ptr = psi_cm_get_path_info( sys_ptr_ptr, (LSA_VOID_PTR_TYPE *)detail_ptr_ptr, path );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_RELEASE_PATH_INFO
LSA_VOID CM_RELEASE_PATH_INFO(
	LSA_UINT16         *  ret_val_ptr,
	LSA_SYS_PTR_TYPE      sys_ptr,
	CM_DETAIL_PTR_TYPE    detail_ptr )
{
	*ret_val_ptr = psi_cm_release_path_info( sys_ptr, detail_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_ALLOC_LOCAL_MEM
LSA_VOID CM_ALLOC_LOCAL_MEM(
	CM_LOCAL_MEM_PTR_TYPE * local_mem_ptr_ptr,
	LSA_UINT32                                length )
{
	LSA_USER_ID_TYPE    null_usr_id;

    PSI_INIT_USER_ID_UNION(null_usr_id);

	PSI_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)local_mem_ptr_ptr, null_usr_id, length, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_LOCAL_MEM
LSA_VOID CM_FREE_LOCAL_MEM(
	LSA_UINT16            * ret_val_ptr,
	CM_LOCAL_MEM_PTR_TYPE   local_mem_ptr )
{
	PSI_FREE_LOCAL_MEM( ret_val_ptr, local_mem_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_ALLOC_TIMER
LSA_VOID CM_ALLOC_TIMER(
	LSA_UINT16        * ret_val_ptr,
	LSA_TIMER_ID_TYPE * timer_id_ptr,
	LSA_UINT16          timer_type,
	LSA_UINT16          time_base )
{
	PSI_ALLOC_TIMER_TGROUP0( ret_val_ptr, timer_id_ptr, timer_type, time_base, cm_timeout );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_START_TIMER
LSA_VOID CM_START_TIMER(
	LSA_UINT16        *  ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id,
	LSA_USER_ID_TYPE     user_id,
	LSA_UINT16           time )
{
	PSI_START_TIMER( ret_val_ptr, timer_id, user_id, time );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_STOP_TIMER
LSA_VOID CM_STOP_TIMER(
	LSA_UINT16        *  ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id)
{
	PSI_STOP_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FREE_TIMER
LSA_VOID CM_FREE_TIMER(
	LSA_UINT16        *  ret_val_ptr,
	LSA_TIMER_ID_TYPE    timer_id)
{
	PSI_FREE_TIMER( ret_val_ptr, timer_id );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_FATAL_ERROR
LSA_VOID CM_FATAL_ERROR (
	LSA_UINT16              length,
	CM_FATAL_ERROR_PTR_TYPE error_ptr )
{
	PSI_FATAL_ERROR( "cm", length, error_ptr );
}
#else
#pragma CM_MESSAGE("CM_FATAL_ERROR redefined by compiler switch to: " CM_STRINGIFY(CM_FATAL_ERROR))
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_REQUEST_UPPER_DONE
LSA_VOID CM_REQUEST_UPPER_DONE(
	CM_UPPER_CALLBACK_FCT_PTR_TYPE  cm_request_upper_done_ptr,
	CM_UPPER_RQB_PTR_TYPE           upper_rqb_ptr,
	LSA_SYS_PTR_TYPE                sys_ptr )
{
	psi_request_done( (PSI_REQUEST_FCT)cm_request_upper_done_ptr, (struct psi_header*)upper_rqb_ptr, sys_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_REQUEST_LOCAL
LSA_VOID CM_REQUEST_LOCAL(
	CM_UPPER_RQB_PTR_TYPE  upper_rqb_ptr )
{
	psi_request_local( PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, (LSA_VOID_PTR_TYPE *)upper_rqb_ptr );
}
#else
#error "by design a function!"
#endif

/*----------------------------------------------------------------------------*/

#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("@@@@ TODO complete CM_GET_CYCLE_COUNTER for EDDS if known")
#endif

#ifndef CM_GET_CYCLE_COUNTER
LSA_VOID CM_GET_CYCLE_COUNTER(
	LSA_UINT16       *  ret_val_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT64       *  cycle_counter_ptr )
{
	PSI_SYS_PTR_TYPE pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	switch ( pSys->comp_id_edd ) // EDDx variant ?
	{
#if ( PSI_CFG_USE_EDDI == 1 )
	case LSA_COMP_ID_EDDI:
		{
			LSA_VOID_PTR_TYPE       hDDB;
			EDDI_CYCLE_COUNTER_TYPE tmp;

			hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddi_SRTProviderGetCycleCounter( hDDB, &tmp) != EDD_STS_OK )
			{
			    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
			}

			*cycle_counter_ptr = ((LSA_UINT64)tmp.High << 32) | tmp.Low;
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDP == 1 )
	case LSA_COMP_ID_EDDP:
		{
            LSA_VOID_PTR_TYPE       hDDB;
			EDDP_CYCLE_COUNTER_TYPE tmp;

            hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if( eddp_SRTProviderGetCycleCounter(hDDB, &tmp) != EDD_STS_OK )
			{
			    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
			}

			*cycle_counter_ptr = ((LSA_UINT64)tmp.High << 32) | tmp.Low;
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDS == 1 )
	case LSA_COMP_ID_EDDS:
		{
		static unsigned cyc = 0;

		cyc++; /* dummy cycle only */

		*cycle_counter_ptr = cyc;
		}
		break;
#endif
#if ( PSI_CFG_USE_EDDT == 1 )
	case LSA_COMP_ID_EDDT:
		{
		static unsigned cyc = 0;

		cyc++; /* dummy cycle only */

		*cycle_counter_ptr = cyc;
		}
		break;
#endif
	default:
		{
			// invalid EDDx
		    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
		}
	}

	*ret_val_ptr = LSA_RET_OK;
}
#else
#error "by design a function!"
#endif

#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE(" @@@@@@ TODO complete CM_GET_PRIMARY_AR for EDDP/EDDS if known" )
#endif

/*----------------------------------------------------------------------------*/

#ifndef CM_GET_PRIMARY_AR
LSA_UINT16 CM_GET_PRIMARY_AR(
	LSA_UINT16       arset_nr,
	LSA_SYS_PTR_TYPE sys_ptr )
{
	LSA_UINT16  session_key = 0; // default no SRD

	PSI_SYS_PTR_TYPE    pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	PSI_ASSERT( pSys != LSA_NULL );

	LSA_UNUSED_ARG( arset_nr );

	switch (pSys->comp_id_edd) // EDDx variant ?
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
		{
			LSA_VOID_PTR_TYPE hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddi_ProviderGetPrimaryAR( hDDB, arset_nr, &session_key) != EDD_STS_OK )
			{
                PSI_SYSTEM_TRACE_01(pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "CM_GET_PRIMARY_AR(): FATAL - eddi_ProviderGetPrimaryAR failed. arset_nr(%u)", arset_nr);
			    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
			}
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
	    case LSA_COMP_ID_EDDP:
		{
            #ifdef EDDP_CFG_SYSRED_API_INCLUDE
			LSA_VOID_PTR_TYPE hDDB = psi_hd_get_hDDB( pSys->hd_nr );

			if ( eddp_ProviderGetPrimaryAR( hDDB, arset_nr, &session_key) != EDD_STS_OK )
			{
                PSI_SYSTEM_TRACE_01(pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "CM_GET_PRIMARY_AR(): FATAL - eddp_ProviderGetPrimaryAR failed. arset_nr(%u)", arset_nr);
			    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
			}
            #else
			PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 ); // configuration error. System adaption needs to set EDDP_CFG_SYSRED_API_INCLUDE to use system redundancy functionality.
            #endif //EDDP_CFG_SYSRED_API_INCLUDE
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
        {
            LSA_VOID_PTR_TYPE hDDB = psi_hd_get_hDDB(pSys->hd_nr);

            if (edds_ProviderGetPrimaryAR(hDDB, arset_nr, &session_key) != EDD_STS_OK)
            {
                PSI_SYSTEM_TRACE_01(pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "CM_GET_PRIMARY_AR(): FATAL - edds_ProviderGetPrimaryAR failed. arset_nr(%u)", arset_nr);
                PSI_FATAL_COMP(LSA_COMP_ID_CM, PSI_MODULE_ID, 0);
            }
        }
        break;
        #endif

        default:
	    {
		    // invalid EDDx or not supported EDDx
            PSI_SYSTEM_TRACE_00(pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "CM_GET_PRIMARY_AR(): FATAL - invalid / unsupported EDD");
		    PSI_FATAL_COMP( LSA_COMP_ID_CM, PSI_MODULE_ID, 0 );
	    }
        break;
	}

	return session_key;
}
#else
#error "by design a function!"
#endif

#endif // PSI_CFG_USE_HD_COMP

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

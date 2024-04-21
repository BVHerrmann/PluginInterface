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
/*  F i l e               &F: psi_clrpc.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail things for CLRPC                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   101
#define PSI_MODULE_ID       101 /* PSI_MODULE_ID_PSI_CLRPC */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== CLRPC =================================================================*/
/*===========================================================================*/

LSA_RESULT psi_clrpc_get_path_info(
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
	PSI_SYS_PTR_TYPE        pSys;
	CLRPC_DETAIL_PTR_TYPE   pDetail;
    LSA_USER_ID_TYPE        user_id;
    LSA_UINT16              mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16              comp_id_lower = LSA_COMP_ID_UNUSED;
	LSA_UINT16              result        = LSA_RET_OK;

	LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

    PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(detail_ptr_ptr != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	switch (path)
	{
	    case PSI_PATH_GLO_SYS_CLRPC_SOCK:
		{
			pDetail->path_type            = CLRPC_PATH_TYPE_SYSTEM;
			pDetail->u.epm.nothing_so_far = 0;

			mbx_id_rsp    = PSI_MBX_ID_PSI;
			comp_id_lower = LSA_COMP_ID_SOCK;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_CLRPC:
		{
			// Note: from CLRPC to SOCK only one global channel is opened
			pDetail->path_type = CLRPC_PATH_TYPE_USER;

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_CM;
            }
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	    case PSI_PATH_GLO_APP_IOS_CLRPC: /* user path IOS */
		{
			pDetail->path_type = CLRPC_PATH_TYPE_USER;

            #if (PSI_CFG_USE_HIF_LD == 1)  // Using HIF LD
            if ((psi_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (psi_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_LD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER; // IOS user over PSI LD
            }
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	    default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_clrpc_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
		break;
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_CLRPC;
		pSys->comp_id_lower         = comp_id_lower;
		pSys->comp_id_edd           = pDetailsIF->edd_comp_id;
		pSys->mbx_id_rsp            = mbx_id_rsp;            // MBX for responses
		pSys->hSysDev               = pDetailsIF->hSysDev;   // Sys handle for HW out functions
		pSys->hPoolNrtTx            = pDetailsIF->hPoolNrtTx;   // NRT TX pool handle for NRT mem
		pSys->hPoolNrtRx            = pDetailsIF->hPoolNrtRx;   // NRT RX pool handle for NRT mem
		pSys->check_arp             = LSA_FALSE;
		pSys->trace_idx             = pDetailsIF->trace_idx;
        pSys->hd_runs_on_level_ld   = pDetailsIF->hd_runs_on_level_ld;
        pSys->psi_path              = path;

		*detail_ptr_ptr = pDetail;
		*sys_ptr_ptr    = pSys;
	}

	return (result);
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_clrpc_release_path_info(
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_VOID_PTR_TYPE   detail_ptr)
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_CLRPC);

    PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_release_path_info()" );

    PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_CLRPC, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return (LSA_RET_OK);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_clrpc_open_channel(
    LSA_SYS_PATH_TYPE   sys_path)
{
	LSA_UINT32                  trace_idx;
	LSA_USER_ID_TYPE            user_id;
	PSI_SYS_TYPE                dummy_sys = {0};
	CLRPC_UPPER_RQB_PTR_TYPE    pRQB;

	LSA_UINT16  path        = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16  hd_nr       = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16  pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

    LSA_UNUSED_ARG(dummy_sys);
    
    PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    PSI_INIT_USER_ID_UNION(user_id);

	CLRPC_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
    PSI_ASSERT(pRQB != LSA_NULL);

	CLRPC_ALLOC_UPPER_MEM( &pRQB->args.void_ptr, user_id, sizeof(CLRPC_ANY_ARGS_TYPE), &dummy_sys );
	PSI_ASSERT(pRQB->args.void_ptr);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.open->handle                         = CLRPC_INVALID_HANDLE;
	pRQB->args.channel.open->handle_upper                   = (LSA_UINT8)path;
	pRQB->args.channel.open->sys_path                       = sys_path;
	pRQB->args.channel.open->clrpc_request_upper_done_ptr   = (CLRPC_UPPER_CALLBACK_FCT_PTR_TYPE)psi_clrpc_channel_done;

	PSI_RQB_SET_HANDLE(pRQB, CLRPC_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, CLRPC_OPC_OPEN_CHANNEL );
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_CLRPC);

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_open_channel(): Open CLRPC channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x)",
		pnio_if_nr, hd_nr, pRQB->args.channel.open->sys_path, path );

	/* CLRPC is on LD side --> open can be done direct */
	psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_clrpc_close_channel(
    LSA_SYS_PATH_TYPE sys_path, 
    LSA_HANDLE_TYPE handle)
{
	CLRPC_UPPER_RQB_PTR_TYPE    pRQB;
	LSA_UINT32                  trace_idx;
	LSA_USER_ID_TYPE            user_id;
	PSI_SYS_TYPE                dummy_sys = {0};

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

    LSA_UNUSED_ARG(dummy_sys);

	PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    PSI_INIT_USER_ID_UNION(user_id);

	CLRPC_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
    PSI_ASSERT(pRQB != LSA_NULL);

	CLRPC_ALLOC_UPPER_MEM( &pRQB->args.void_ptr, user_id, sizeof(CLRPC_ANY_ARGS_TYPE), &dummy_sys );
	PSI_ASSERT(pRQB->args.void_ptr);

	PSI_RQB_SET_HANDLE(pRQB, handle);
	PSI_RQB_SET_OPCODE(pRQB, CLRPC_OPC_CLOSE_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_CLRPC);

	LSA_RQB_SET_USER_ID_UVAR16(pRQB, sys_path); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_close_channel(): Close CLRPC channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x) handle(%u)",
		pnio_if_nr, hd_nr, sys_path, path, handle );

	/* CLRPC is on LD side --> close can be done direct */
	psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_clrpc_channel_done(
    LSA_VOID_PTR_TYPE   rqb_ptr)
{
	LSA_OPCODE_TYPE                opc;
	LSA_UINT16                     rc;
	LSA_UINT32                     trace_idx;
	PSI_SYS_TYPE                   dummy_sys = {0};
	CLRPC_UPPER_RQB_PTR_TYPE const pRQB = (CLRPC_UPPER_RQB_PTR_TYPE)rqb_ptr;
    LSA_UNUSED_ARG(dummy_sys);

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(CLRPC_RQB_GET_RESPONSE(pRQB) == CLRPC_OK);

	opc = CLRPC_RQB_GET_OPCODE(pRQB);

    /* Handle response for CLRPC Opcode */
	switch (opc)
	{
	    case CLRPC_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE const sys_path = pRQB->args.channel.open->sys_path;
			LSA_HANDLE_TYPE   const handle   = pRQB->args.channel.open->handle;

			trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.open->sys_path);

			PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_channel_done(): Open CLRPC channel done, rsp(%u/%#x) sys_path(%#x) handle(%u)",
				CLRPC_RQB_GET_RESPONSE(pRQB),
				CLRPC_RQB_GET_RESPONSE(pRQB),
				pRQB->args.channel.open->sys_path,
				pRQB->args.channel.open->handle);

			CLRPC_FREE_UPPER_MEM( &rc, pRQB->args.void_ptr, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			CLRPC_FREE_UPPER_RQB( &rc, pRQB, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Register handle for this PNIO IF
			psi_ld_open_channels_done(sys_path, handle);
		}
		break;

        case CLRPC_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE   sys_path = LSA_RQB_GET_USER_ID_UVAR16(pRQB);

			trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_clrpc_channel_done(): Close CLRPC channel done, rsp(%u/%#x) sys_path(%#x)",
				CLRPC_RQB_GET_RESPONSE(pRQB),
				CLRPC_RQB_GET_RESPONSE(pRQB),
				sys_path );

			CLRPC_FREE_UPPER_MEM( &rc, pRQB->args.void_ptr, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			CLRPC_FREE_UPPER_RQB( &rc, pRQB, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Unregister handle for the channel
			psi_ld_close_channels_done(sys_path);
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_clrpc_channel_done(): invalid opcode(%u/%#x)", opc, opc );
			PSI_FATAL(0);
		}
	}
}

#endif // (PSI_CFG_USE_LD_COMP == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

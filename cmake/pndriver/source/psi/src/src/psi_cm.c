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
/*  F i l e               &F: psi_cm.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for CM                                           */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   102
#define PSI_MODULE_ID       102 /* PSI_MODULE_ID_PSI_CM */

#include "psi_int.h"

#if (PSI_CFG_USE_HD_COMP == 1)

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== CM ====================================================================*/
/*===========================================================================*/

LSA_RESULT psi_cm_get_path_info(
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
	CM_DETAIL_PTR_TYPE      pDetail;
	PSI_SYS_PTR_TYPE        pSys;
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

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_cm_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	switch (path)
	{
	    case PSI_PATH_IF_APP_CMPD: /* physical device */
		{
			pDetail->path_type = CM_PATH_TYPE_PD;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.pd.epm.recv_resource_count = 4; /* For engineering & diagnosis tools */ 
			pDetail->u.pd.epm.send_resource_count = 4;

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER; // CMPD user
            }
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	    case PSI_PATH_IF_APP_IOC_CMCL_ACP: /* IO Controller */
		{
			pDetail->path_type = CM_PATH_TYPE_CLIENT;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			/* CM-CL RPC Server only used for ApplReady.Ind and CM-PD Record Read.Request */

			pDetail->u.cl.rpc.alloc_len                          = pDetailsIF->cm.cl.max_record_length;    // max. record length /* Kommentar: Max (NDRDataRequest, NDRDataResponse) Part 6 / NDRDataRequest = CM_RECORD_OFFSET + Payload */
			pDetail->u.cl.rpc.call_resource_count                = pDetailsIF->cm.cl.device_count + (PSI_CFG_MAX_READ_IMPL);   // all possible devices may call back (appl-ready) in parallel 
			
			pDetail->u.cl.rpc.recv_resource_count                = (PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND) + (PSI_CFG_MAX_READ_IMPL);
			pDetail->u.cl.rpc.send_resource_count                = 8;  /* ApplReady.Rsp ReadImplicit.Rsp */ /* 2 Req/msec should be sent  => 2 Resources may be in EDD + 2 in Clrpc + 2 on the way to EDD / Clrpc + 2 on the way Clrpc / EDD */

			pDetail->u.cl.acp.alarm_high_prio_ind_resource_count = pDetailsIF->cm.cl.device_count;           
			pDetail->u.cl.acp.alarm_low_prio_ind_resource_count  = pDetailsIF->cm.cl.device_count;           
			pDetail->u.cl.acp.max_alarm_data_length              = pDetailsIF->cm.cl.max_alarm_data_length;  

			pDetail->u.cl.max_connects_per_second = (PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND);
			pDetail->u.cl.min_reconnect_timeout   = 1;   /* [seconds] 1 second for Simatic controllers */ 
			pDetail->u.cl.schedule_timeout        = 200; /* [milli-seconds] 200msec scheduling time for Simatic Controllers */ 

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD; // CMCL user
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER;    // CMCL user
            }
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_APP_IOM_CMMC: /* IO Multicast */
		{
			pDetail->path_type = CM_PATH_TYPE_MULTICAST;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD; // CMMC user
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER;   // CMMC user
            }
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_APP_IOD_CMSV_ACP: /* IO Device */
		{
			pDetail->path_type = CM_PATH_TYPE_SERVER;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.sv.rpc.alloc_len                          = pDetailsIF->cm.sv.max_record_length;     /*use max*/ // CM_RECORD_OFFSET + 32*1024; // 4K mandatory record length  
			pDetail->u.sv.rpc.call_resource_count                = (LSA_UINT16)(pDetailsIF->cm.sv.device_count * (pDetailsIF->cm.sv.max_ar + PSI_CFG_MAX_READ_IMPL)); // all devices may get connected to in parallel  
			pDetail->u.sv.rpc.recv_resource_count                = (LSA_UINT16)(pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar);
			pDetail->u.sv.rpc.send_resource_count                = 8;  /* 2 Req/msec should be sent  => 2 Resources may be in EDD + 2 in Clrpc + 2 on the way to EDD / Clrpc + 2 on the way Clrpc / EDD */ 
			pDetail->u.sv.acp.alarm_high_prio_ind_resource_count = (LSA_UINT16)(pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar);
			pDetail->u.sv.acp.alarm_low_prio_ind_resource_count  = (LSA_UINT16)(pDetailsIF->cm.sv.device_count * pDetailsIF->cm.sv.max_ar);
			pDetail->u.sv.acp.max_alarm_data_length              = CM_ALARM_DATA_LENGTH_MIN; /* Simatic devices can't receive alarms, see CM_OPC_SV_CREATE::enable_AlarmResponder */ 

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD; // CMMC user
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER;   // CMSV user
            }
			comp_id_lower = LSA_COMP_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_CLRPC:
		{
			// Note: from CM to CLRPC an IF specific channel is opened
			pDetail->path_type = CM_PATH_TYPE_RPC;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.rpc.recv_resource_count = 20; /* taken from clrpc path info, may a wild guess ... */ 

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_CLRPC;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_NARE:
		{
			pDetail->path_type = CM_PATH_TYPE_NARE;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.nare.resolve_notify_resource_count = (PSI_CFG_MAX_CL_OSU_DEVICES); /* Only for ASU devices needed */

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{
			pDetail->path_type = CM_PATH_TYPE_EDD;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
			if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
	            mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

        #if (PSI_CFG_USE_MRP == 1)
	    case PSI_PATH_IF_SYS_CM_MRP:
		{
			pDetail->path_type = CM_PATH_TYPE_MRP;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.mrp.nothing_so_far = 0; /* well... */

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
			if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_MRP;
		}
		break;
        #endif

        #if (PSI_CFG_USE_GSY == 1)
	    case PSI_PATH_IF_SYS_CM_GSY:
		{
			pDetail->path_type = CM_PATH_TYPE_GSY;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.gsy.nothing_so_far = 0; /* well... */

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
			if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_GSY;
		}
		break;
        #endif

	    case PSI_PATH_IF_SYS_CM_OHA:
		{
			pDetail->path_type = CM_PATH_TYPE_OHA;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			// Note: must match with OHA details
            //lint --e(506) Constant value Boolean - the "PSI_CFG_HELLO_SERVER_RESOURCE_COUNT_DCP" can be changed by system adaption
            pDetail->u.oha.hello_resource_count = PSI_CFG_HELLO_SERVER_RESOURCE_COUNT_DCP;

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
			if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_OHA;
		}
		break;

        #if (PSI_CFG_USE_POF == 1)
	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail->path_type = CM_PATH_TYPE_POF;
			pDetail->nic_id    = pDetailsIF->pnio_if_nr;

			pDetail->u.pof.nothing_so_far = 0; /* well... */

            #if (PSI_CFG_USE_HIF_HD == 1)  // Using HIF HD
			if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_PSI;
            }
			comp_id_lower = LSA_COMP_ID_POF;
		}
		break;
        #endif

	    default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_cm_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
		break;
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_CM;
		pSys->comp_id_lower         = comp_id_lower;
		pSys->comp_id_edd           = pDetailsIF->edd_comp_id;
		pSys->mbx_id_rsp            = mbx_id_rsp;               // MBX for responses
		pSys->hSysDev               = pDetailsIF->hSysDev;      // Sys handle for HW out functions
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
LSA_RESULT psi_cm_release_path_info(
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_VOID_PTR_TYPE   detail_ptr)
{
	LSA_UINT16          rc;
	PSI_SYS_PTR_TYPE    pSys;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_CM);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_cm_release_path_info()" );

    PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_CM, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return (LSA_RET_OK);
}

#endif // (PSI_CFG_USE_HD_COMP == 1)

#if (PSI_CFG_USE_LD_COMP == 1)
/*----------------------------------------------------------------------------*/
LSA_VOID psi_cm_open_channel(
    LSA_SYS_PATH_TYPE   sys_path)
{
	LSA_UINT32              trace_idx;
	LSA_USER_ID_TYPE        user_id;
	PSI_SYS_TYPE            dummy_sys = {0};
	CM_UPPER_RQB_PTR_TYPE   pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

    LSA_UNUSED_ARG(dummy_sys);
    
    PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    PSI_INIT_USER_ID_UNION(user_id);

    CM_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
    PSI_ASSERT(pRQB != LSA_NULL);

	CM_ALLOC_UPPER_MEM( &pRQB->args.void_ptr, user_id, sizeof(CM_ANY_ARGS_TYPE), &dummy_sys );
	PSI_ASSERT(pRQB->args.void_ptr);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.open->handle                     = CM_INVALID_HANDLE;
	pRQB->args.channel.open->handle_upper               = (LSA_UINT8)path;
	pRQB->args.channel.open->sys_path                   = sys_path;
	pRQB->args.channel.open->cm_request_upper_done_ptr  = (CM_UPPER_CALLBACK_FCT_PTR_TYPE)psi_cm_channel_done;

	PSI_RQB_SET_HANDLE(pRQB, CM_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, CM_OPC_OPEN_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_CM);

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_cm_open_channel(): Open CM channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x)",
		pnio_if_nr, hd_nr, pRQB->args.channel.open->sys_path, path );

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* CM is on HD side, so delegate open to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, pRQB);
    }
    else
    #endif
    {
	    /* HIF HD not configured, close can be done direct */
	    psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, pRQB);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_cm_close_channel(
    LSA_SYS_PATH_TYPE   sys_path, 
    LSA_HANDLE_TYPE     handle)
{
	LSA_UINT32              trace_idx;
	LSA_USER_ID_TYPE        user_id;
	PSI_SYS_TYPE            dummy_sys = {0};
	CM_UPPER_RQB_PTR_TYPE   pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

    LSA_UNUSED_ARG(dummy_sys);

    PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    PSI_INIT_USER_ID_UNION(user_id);

    CM_ALLOC_UPPER_RQB( &pRQB, user_id, sizeof(*pRQB), &dummy_sys );
    PSI_ASSERT(pRQB != LSA_NULL);

	CM_ALLOC_UPPER_MEM( &pRQB->args.void_ptr, user_id, sizeof(CM_ANY_ARGS_TYPE), &dummy_sys );
	PSI_ASSERT(pRQB->args.void_ptr);

	PSI_RQB_SET_HANDLE(pRQB, handle);
	PSI_RQB_SET_OPCODE(pRQB, CM_OPC_CLOSE_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_CM);
	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_cm_close_channel(): Close CM channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x) handle(%u)",
		pnio_if_nr, hd_nr, sys_path, path, handle );

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* CM is on HD side, so delegate close to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, pRQB);
    }
    else
    #endif
    {
	    /* HIF HD not configured, close can be done direct */
	    psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, pRQB);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_cm_channel_done(
    LSA_VOID_PTR_TYPE rqb_ptr)
{
	LSA_UINT16                  rc;
	LSA_UINT32                  trace_idx;
	LSA_OPCODE_TYPE             opc;
	PSI_SYS_TYPE                dummy_sys = {0};
	CM_UPPER_RQB_PTR_TYPE const pRQB = (CM_UPPER_RQB_PTR_TYPE)rqb_ptr;

    LSA_UNUSED_ARG(dummy_sys);

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(CM_RQB_GET_RESPONSE(pRQB) == CM_OK);

	opc = CM_RQB_GET_OPCODE(pRQB);

    /* Handle response for CM Opcode */
	switch (opc)
	{
	    case CM_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE const sys_path = pRQB->args.channel.open->sys_path;
			LSA_HANDLE_TYPE   const handle   = pRQB->args.channel.open->handle;

			trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.open->sys_path);

			PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_cm_channel_done(): Open CM channel done, rsp(%u/%#x) sys_path(%#x) handle(%u)",
				CM_RQB_GET_RESPONSE(pRQB),
				CM_RQB_GET_RESPONSE(pRQB),
				pRQB->args.channel.open->sys_path,
				pRQB->args.channel.open->handle );

			CM_FREE_UPPER_MEM( &rc, pRQB->args.void_ptr, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			CM_FREE_UPPER_RQB( &rc, pRQB, &dummy_sys );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Register handle for the channel
			psi_ld_open_channels_done(sys_path, handle);
		}
		break;

        case CM_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE const sys_path = LSA_RQB_GET_USER_ID_UVAR16(pRQB);

			trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_cm_channel_done(): Close CM channel done, rsp(%u/%#x) sys_path(%#x)",
				CM_RQB_GET_RESPONSE(pRQB),
				CM_RQB_GET_RESPONSE(pRQB),
				sys_path );

			CM_FREE_UPPER_MEM(&rc, pRQB->args.void_ptr, &dummy_sys);
			PSI_ASSERT(rc == LSA_RET_OK);

			CM_FREE_UPPER_RQB(&rc, pRQB, &dummy_sys);
			PSI_ASSERT(rc == LSA_RET_OK);

			// Unregister handle for the channel
			psi_ld_close_channels_done(sys_path);
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_cm_channel_done(): invalid opcode(%u/%#x)", opc, opc );
			PSI_FATAL(0);
		}
	}
}

#endif //  (PSI_CFG_USE_LD_COMP == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

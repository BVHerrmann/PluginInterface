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
/*  F i l e               &F: psi_mrp.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for MRP                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   112
#define PSI_MODULE_ID       112 /* PSI_MODULE_ID_PSI_MRP */

#include "psi_int.h"

#if ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== MRP ===================================================================*/
/*===========================================================================*/

LSA_RESULT psi_mrp_get_path_info(
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
    LSA_USER_ID_TYPE        user_id;
    MRP_DETAIL_PTR_TYPE     pDetail;
	PSI_SYS_PTR_TYPE        pSys;
	LSA_UINT16              mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16              comp_id_lower = LSA_COMP_ID_UNUSED;
	LSA_UINT16              result        = LSA_RET_OK;

	LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

	PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF  != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_mrp_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	switch (path)
	{
	    case PSI_PATH_IF_SYS_MRP_EDD:
		{
			pDetail->NicId    = pDetailsIF->pnio_if_nr;
			pDetail->PathType = MRP_PATH_TYPE_EDD;

			pDetail->type.edd.dummy = 1;

            #if (PSI_CFG_USE_HIF_HD  == 1)  // Using HIF HD
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

	    case PSI_PATH_IF_SYS_OHA_MRP:
		{
			pDetail->NicId    = pDetailsIF->pnio_if_nr;
			pDetail->PathType = MRP_PATH_TYPE_USER;

			pDetail->type.user.isPrmInterface = MRP_IS_PRM_INTERFACE_OFF;
			pDetail->type.user.PrioDefault    = 0xA000;

            #if (PSI_CFG_USE_HIF_HD  == 1)  // Using HIF HD
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_OHA;
            }
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_MRP:
		{
			pDetail->NicId    = pDetailsIF->pnio_if_nr;
			pDetail->PathType = MRP_PATH_TYPE_USER;

			pDetail->type.user.isPrmInterface = MRP_IS_PRM_INTERFACE_ON;
			pDetail->type.user.PrioDefault    = 0xA000;

			mbx_id_rsp    = PSI_MBX_ID_CM;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

	    default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_mrp_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
		break;
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_MRP;
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

		*sys_ptr_ptr    = pSys;
		*detail_ptr_ptr = pDetail;
	}

	return (result);
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_mrp_release_path_info(
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_VOID_PTR_TYPE   detail_ptr)
{
	LSA_UINT16             rc;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_MRP);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_mrp_release_path_info()" );

    PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_MRP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return (LSA_RET_OK);
}
#endif  // ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))

#if ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_LD_COMP == 1))
/*----------------------------------------------------------------------------*/
LSA_VOID psi_mrp_open_channel(
    LSA_SYS_PATH_TYPE   sys_path)
{
	LSA_UINT32              trace_idx;
	MRP_UPPER_RQB_PTR_TYPE  pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

	MRP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
    PSI_ASSERT(pRQB != LSA_NULL);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.open.handle          = MRP_INVALID_HANDLE;
	pRQB->args.channel.open.handle_upper    = (LSA_UINT8)path;
	pRQB->args.channel.open.sys_path        = sys_path;
	pRQB->args.channel.open.pCbf            = (MRP_UPPER_CALLBACK_FCT_PTR_TYPE)psi_mrp_channel_done;

	PSI_RQB_SET_HANDLE(pRQB, MRP_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, MRP_OPC_OPEN_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_MRP);

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_mrp_open_channel(): Open MRP channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x)", 
		pnio_if_nr, hd_nr, pRQB->args.channel.open.sys_path, path );

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* MRP is on HD side, so delegate open to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, pRQB);
    }
    else
    #endif
    {
        /* HIF HD not configured, open can be done direct */
        psi_request_start(PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_open_channel, pRQB);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_mrp_close_channel(
    LSA_SYS_PATH_TYPE   sys_path, 
    LSA_HANDLE_TYPE     handle)
{
	LSA_UINT32              trace_idx;
	MRP_UPPER_RQB_PTR_TYPE  pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

	MRP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(*pRQB) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_RQB_SET_HANDLE( pRQB, handle);
	PSI_RQB_SET_OPCODE( pRQB, MRP_OPC_CLOSE_CHANNEL );
	PSI_RQB_SET_COMP_ID( pRQB, LSA_COMP_ID_MRP );

	LSA_RQB_SET_USER_ID_UVAR16( pRQB, sys_path ); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_mrp_close_channel(): Close MRP channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x) handle(%u)", 
		pnio_if_nr, hd_nr, sys_path, path, handle );

    #if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
	    /* MRP is on HD side, so delegate close to HIF HD */
	    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, pRQB);
    }
    else
    #endif
    {
        /* HIF HD not configured, close can be done direct */
        psi_request_start(PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_close_channel, pRQB);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_mrp_channel_done(
    LSA_VOID_PTR_TYPE   rqb_ptr)
{
	LSA_UINT16                   rc;
	LSA_UINT32                   trace_idx;
	LSA_OPCODE_TYPE              opc;
	MRP_UPPER_RQB_PTR_TYPE const pRQB = (MRP_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(MRP_RQB_GET_RESPONSE(pRQB) == MRP_RSP_OK);

	opc = MRP_RQB_GET_OPCODE(pRQB);

    /* Handle response for MRP Opcode */
	switch (opc)
	{
	    case MRP_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE const sys_path = pRQB->args.channel.open.sys_path;
			LSA_HANDLE_TYPE   const handle   = pRQB->args.channel.open.handle;

			trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.open.sys_path);

			PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_mrp_channel_done(): Open MRP channel done, rsp(%u/%#x) sys_path(%#x) handle(%u)",
				MRP_RQB_GET_RESPONSE(pRQB), 
				MRP_RQB_GET_RESPONSE(pRQB), 
				pRQB->args.channel.open.sys_path, 
				pRQB->args.channel.open.handle );

			MRP_FREE_UPPER_RQB_LOCAL( &rc, pRQB );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Register handle for the channel
			psi_ld_open_channels_done(sys_path, handle);
		}
		break;

        case MRP_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE const sys_path = LSA_RQB_GET_USER_ID_UVAR16(pRQB);

			trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_mrp_channel_done(): Close MRP channel done, rsp(%u/%#x) sys_path(%#x)",
				MRP_RQB_GET_RESPONSE(pRQB), 
				MRP_RQB_GET_RESPONSE(pRQB), 
				sys_path );

			MRP_FREE_UPPER_RQB_LOCAL( &rc, pRQB );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Unregister handle for the channel
			psi_ld_close_channels_done(sys_path);
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_mrp_channel_done(): invalid opcode(%u/%#x)", opc, opc );
			PSI_FATAL(0);
		}
	}
}

#endif  // ((PSI_CFG_USE_MRP == 1) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

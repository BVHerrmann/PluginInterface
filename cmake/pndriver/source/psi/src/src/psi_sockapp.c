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
/*  F i l e               &F: psi_sockapp.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for SOCKAPP                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   121
#define PSI_MODULE_ID       121 /* PSI_MODULE_ID_PSI_SOCKAPP */

#include "psi_int.h"

#if ((PSI_CFG_USE_SOCKAPP == 1) && (PSI_CFG_USE_LD_COMP == 1))

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== SOCKAPP====================================================================*/
/*===========================================================================*/

LSA_RESULT psi_sockapp_get_path_info(
    LSA_SYS_PTR_TYPE  * sys_ptr_ptr,
    LSA_VOID_PTR_TYPE * detail_ptr_ptr,
    LSA_SYS_PATH_TYPE   sys_path)
{
    SOCKAPP_DETAIL_PTR_TYPE         pDetail;
    PSI_SYS_PTR_TYPE                pSys;
    LSA_USER_ID_TYPE                user_id;
    LSA_UINT16                      mbx_id_rsp    = PSI_MBX_ID_MAX;
    LSA_UINT16                      comp_id_lower = LSA_COMP_ID_UNUSED;
    LSA_UINT16                      result        = LSA_RET_OK;
    LSA_UINT16                const hd_nr         = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16                const path          = PSI_SYSPATH_GET_PATH(sys_path);
    LSA_UINT16                const hd_count      = psi_ld_get_nr_of_hd();
    PSI_DETAIL_STORE_PTR_TYPE       pDetailsIF    = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
    PSI_ASSERT(pDetailsIF  != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);
    PSI_LD_TRACE_06(0, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)",
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path);

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM );
    PSI_ASSERT(pDetail != LSA_NULL);

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM );
    PSI_ASSERT(pSys != LSA_NULL);

    switch ( path )
    {
        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01:
        {
            pDetail->PathType = SOCKAPP_PATH_TYPE_SOCK_01;
            mbx_id_rsp        = PSI_MBX_ID_PSI;     // PSI opens the channel and wants back the confirmation
            comp_id_lower     = LSA_COMP_ID_SOCK;   // no hif between sockapp and sock
        }
        break;

        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02:
        {
            pDetail->PathType = SOCKAPP_PATH_TYPE_SOCK_02;
            mbx_id_rsp        = PSI_MBX_ID_PSI;     // PSI opens the channel and wants back the confirmation
            comp_id_lower     = LSA_COMP_ID_SOCK;   // no hif between sockapp and sock
        }
        break;

        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03:
        {
            pDetail->PathType = SOCKAPP_PATH_TYPE_SOCK_03;
            mbx_id_rsp        = PSI_MBX_ID_PSI;     // PSI opens the channel and wants back the confirmation
            comp_id_lower     = LSA_COMP_ID_SOCK;   // no hif between sockapp and sock
        }
        break;

        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04:
        {
            pDetail->PathType = SOCKAPP_PATH_TYPE_SOCK_04;
            mbx_id_rsp        = PSI_MBX_ID_PSI;     // PSI opens the channel and wants back the confirmation
            comp_id_lower     = LSA_COMP_ID_SOCK;   // no hif between sockapp and sock
        }
        break;

        case PSI_PATH_GLO_APP_SOCKAPP:
        {
			LSA_UINT16 hd_detail_nr;

            pDetail->PathType = SOCKAPP_PATH_TYPE_USER;
			
			SOCKAPP_MEMSET(pDetail->bClusterIP, 0, sizeof(pDetail->bClusterIP));
			for (hd_detail_nr = 1; hd_detail_nr <= hd_count; hd_detail_nr++)
			{
				pDetailsIF = psi_get_detail_store(hd_detail_nr);
				PSI_ASSERT(pDetailsIF != LSA_NULL);
				pDetail->bClusterIP[pDetailsIF->pnio_if_nr - 1] = pDetailsIF->sockapp.bClusterIP;
			}

            #if ( PSI_CFG_USE_HIF_LD == 1)  // Using HIF LD
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

            PSI_FREE_LOCAL_MEM(&rc, pDetail, 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM);
            PSI_ASSERT(rc == LSA_RET_OK);

            PSI_FREE_LOCAL_MEM(&rc, pSys, 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM);
            PSI_ASSERT(rc == LSA_RET_OK);

            *detail_ptr_ptr = LSA_NULL;
            *sys_ptr_ptr = LSA_NULL;
            result = LSA_RET_ERR_SYS_PATH;

            PSI_LD_TRACE_05(0, LSA_TRACE_LEVEL_ERROR, "psi_sockapp_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result);
        }
        break;
    }

    if (result == LSA_RET_OK)
    {
        // now we setup the Sysptr based on information stored for this PNIO IF
        pSys->hd_nr                 = hd_nr;
        pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
        pSys->comp_id               = LSA_COMP_ID_SOCKAPP;
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

        *sys_ptr_ptr                = pSys;
        *detail_ptr_ptr             = pDetail;
    }

    return (result);
}

/*----------------------------------------------------------------------------*/

LSA_RESULT psi_sockapp_release_path_info(
    LSA_SYS_PTR_TYPE  sys_ptr,
    LSA_VOID_PTR_TYPE detail_ptr)
{
    LSA_UINT16             rc;
    PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

    PSI_ASSERT(sys_ptr);
    PSI_ASSERT(detail_ptr);

    PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_SOCKAPP);

    PSI_LD_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_release_path_info()");

    PSI_FREE_LOCAL_MEM(&rc, detail_ptr, 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM);
    PSI_ASSERT(rc == LSA_RET_OK);

    PSI_FREE_LOCAL_MEM(&rc, sys_ptr, 0, LSA_COMP_ID_SOCKAPP, PSI_MTYPE_LOCAL_MEM);
    PSI_ASSERT(rc == LSA_RET_OK);

    return (LSA_RET_OK);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sockapp_open_channel(
    LSA_SYS_PATH_TYPE   sys_path)
{
    LSA_UINT32                  trace_idx;
    SOCKAPP_UPPER_RQB_PTR_TYPE  pRQB = LSA_NULL;

    LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
    LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

    PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    SOCKAPP_ALLOC_LOCAL_MEM((LSA_VOID **)&pRQB, sizeof(*pRQB));
    PSI_ASSERT(pRQB);

    /* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
    pRQB->args.channel.handle = PSI_INVALID_HANDLE; // No SOCKAPP_INVALID_HANDLE
    pRQB->args.channel.handle_upper = (LSA_UINT8)path;
    pRQB->args.channel.sys_path = sys_path;
    pRQB->args.channel.sockapp_request_upper_done_ptr = (SOCKAPP_UPPER_CALLBACK_FCT_PTR_TYPE)psi_sockapp_channel_done;

    PSI_RQB_SET_HANDLE(pRQB, PSI_INVALID_HANDLE);
    PSI_RQB_SET_OPCODE(pRQB, SOCKAPP_OPC_OPEN_CHANNEL);
    PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_SOCKAPP);

    trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

    PSI_SYSTEM_TRACE_04(trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_open_channel(): Open SOCKAPP channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x)",
        pnio_if_nr, hd_nr, pRQB->args.channel.sys_path, path);

    /* SOCKAPP is on LD side --> open can be done direct */
    psi_request_start(PSI_MBX_ID_SOCKAPP, (PSI_REQUEST_FCT)sockapp_open_channel, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sockapp_close_channel(
    LSA_SYS_PATH_TYPE sys_path,
    LSA_HANDLE_TYPE   handle)
{
    LSA_UINT32                  trace_idx;
    SOCKAPP_UPPER_RQB_PTR_TYPE  pRQB;

    LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
    LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

    PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

    SOCKAPP_ALLOC_LOCAL_MEM((LSA_VOID **)&pRQB, sizeof(*pRQB));
    PSI_ASSERT(pRQB != LSA_NULL);

    PSI_RQB_SET_HANDLE(pRQB, handle);
    PSI_RQB_SET_OPCODE(pRQB, SOCKAPP_OPC_CLOSE_CHANNEL);
    PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_SOCKAPP);

    LSA_RQB_SET_USER_ID_UVAR16(pRQB, sys_path); // ID is used for unregister handle

    trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

    PSI_SYSTEM_TRACE_05(trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_close_channel(): Close sockapp channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x) handle(%u)",
        pnio_if_nr, hd_nr, sys_path, path, handle);

    /* SOCKAPP is on LD side --> close can be done direct */
    psi_request_start(PSI_MBX_ID_SOCKAPP, (PSI_REQUEST_FCT)sockapp_close_channel, pRQB);

}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sockapp_channel_done(
    LSA_VOID_PTR_TYPE rqb_ptr)
{
    LSA_UINT16                       rc;
    LSA_UINT32                       trace_idx;
    LSA_OPCODE_TYPE                  opc;
    SOCKAPP_UPPER_RQB_PTR_TYPE const pRQB     = (SOCKAPP_UPPER_RQB_PTR_TYPE)rqb_ptr;
    LSA_SYS_PATH_TYPE                sys_path = 0;
    LSA_HANDLE_TYPE                  handle   = 0;

    PSI_ASSERT(pRQB != LSA_NULL);
    PSI_ASSERT(SOCKAPP_RQB_GET_RESPONSE(pRQB) == SOCKAPP_OK);

    opc = SOCKAPP_RQB_GET_OPCODE(pRQB);

    /* Handle response for SOCKAPP Opcode */

    switch (opc)
    {
        case SOCKAPP_OPC_OPEN_CHANNEL:
        {
            sys_path    = pRQB->args.channel.sys_path;
            handle      = pRQB->args.channel.handle;

            trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.sys_path);

            PSI_SYSTEM_TRACE_04(trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_channel_done(): Open SOCKAPP channel done, rsp(%u/%#x) sys_path(%#x) handle(%u)",
                SOCKAPP_RQB_GET_RESPONSE(pRQB),
                SOCKAPP_RQB_GET_RESPONSE(pRQB),
                pRQB->args.channel.sys_path,
                pRQB->args.channel.handle);

            SOCKAPP_FREE_LOCAL_MEM(&rc, pRQB);
            PSI_ASSERT(rc == SOCKAPP_OK);

            // Register handle for the channel
            psi_ld_open_channels_done(sys_path, handle);
        }
        break;

        case SOCKAPP_OPC_CLOSE_CHANNEL:
        {
            // Unregister handle for this PNIO IF (stored in USER-ID)
            sys_path = LSA_RQB_GET_USER_ID_UVAR16(pRQB);

            trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

            PSI_SYSTEM_TRACE_03(trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sockapp_channel_done(): Close sockapp channel done, rsp(%u/%#x) sys_path(%#x)",
                SOCKAPP_RQB_GET_RESPONSE(pRQB),
                SOCKAPP_RQB_GET_RESPONSE(pRQB),
                sys_path);

            SOCKAPP_FREE_LOCAL_MEM(&rc, pRQB);
            PSI_ASSERT(rc == SOCKAPP_OK);

            // Unregister handle for the channel
            psi_ld_close_channels_done(sys_path);
        }
        break;

        default:
        {
            PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_sockapp_channel_done(): invalid opcode(%u/%#x)", opc, opc);
            PSI_FATAL(0);
        }
    }
}

/*----------------------------------------------------------------------------*/
#endif  // ((PSI_CFG_USE_SOCKAPP == 1) && (PSI_CFG_USE_LD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

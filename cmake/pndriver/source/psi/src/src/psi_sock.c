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
/*  F i l e               &F: psi_sock.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for SOCK                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   117
#define PSI_MODULE_ID       117 /* PSI_MODULE_ID_PSI_SOCK */

#include "psi_int.h"

#if (PSI_CFG_USE_LD_COMP == 1)

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== SOCK ==================================================================*/
/*===========================================================================*/

LSA_RESULT psi_sock_get_path_info(
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
    LSA_USER_ID_TYPE                            user_id;
    SOCK_DETAIL_PTR_TYPE                        pDetail;
	PSI_SYS_PTR_TYPE                            pSys;
	PSI_SOCK_INPUT_PTR_TYPE                     pPsiSockInputDetail;
	PSI_SOCK_APP_CH_DETAIL_DETAILS_PTR_TYPE     pPsiSockInputDetails;
	LSA_UINT16                                  mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16                                  comp_id_lower = LSA_COMP_ID_UNUSED;
	LSA_UINT16                                  result        = LSA_RET_OK;

	LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD( sys_path );
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH( sys_path );

	PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF  != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_LD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_sock_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	switch (path)
	{
	    case PSI_PATH_GLO_APP_SOCK1:
	    case PSI_PATH_GLO_APP_SOCK2:
	    case PSI_PATH_GLO_APP_SOCK3:
	    case PSI_PATH_GLO_APP_SOCK4:
	    case PSI_PATH_GLO_APP_SOCK5:
	    case PSI_PATH_GLO_APP_SOCK6:
	    case PSI_PATH_GLO_APP_SOCK7:
	    case PSI_PATH_GLO_APP_SOCK8:
		{
			pPsiSockInputDetail = psi_ld_get_sock_details();
			PSI_ASSERT(pPsiSockInputDetail != LSA_NULL);

			if ((path - PSI_PATH_GLO_APP_SOCK1) < PSI_CFG_MAX_SOCK_APP_CHANNELS)
			{
				/* User specified details */
				pPsiSockInputDetails = &pPsiSockInputDetail->sock_app_ch_details[path - PSI_PATH_GLO_APP_SOCK1].sock_detail;

				pDetail->linger_time                     = pPsiSockInputDetails->linger_time;
				pDetail->rec_buffer_max_len              = pPsiSockInputDetails->rec_buffer_max_len;
				pDetail->send_buffer_max_len             = pPsiSockInputDetails->send_buffer_max_len;
				pDetail->sock_close_at_once              = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->sock_close_at_once               ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_DONTROUTE_     = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_DONTROUTE_      ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_LINGER_        = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_LINGER_         ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_RCVBUF_        = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_RCVBUF_         ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_REUSEADDR_     = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_REUSEADDR_      ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_SNDBUF_        = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_SNDBUF_         ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_TCPNOACKDELAY_ = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_TCPNOACKDELAY_  ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_TCPNODELAY_    = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_TCPNODELAY_     ) ? LSA_TRUE : LSA_FALSE;
				pDetail->socket_option.SO_BROADCAST_     = ( PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_BROADCAST_      ) ? LSA_TRUE : LSA_FALSE;
			}
			else
			{
				PSI_FATAL(0); /* Check PSI_CFG_MAX_SOCK_APP_CHANNELS */
			}

            #if (PSI_CFG_USE_HIF_LD)  // Using HIF LD
            if ((psi_get_ld_runs_on() == PSI_LD_RUNS_ON_ADVANCED) || (psi_get_ld_runs_on() == PSI_LD_RUNS_ON_BASIC))
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_LD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_USER; // Sock User
            }
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

        #if (PSI_CFG_USE_SOCKAPP == 1)
        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01: /* Details for Socket Application sockets */
        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_02:
        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_03:
        case PSI_PATH_GLO_SYS_SOCKAPP_SOCK_04:
        {
            pPsiSockInputDetail = psi_ld_get_sock_details();
            PSI_ASSERT(pPsiSockInputDetail != LSA_NULL);

            if ((path - PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01) < PSI_CFG_MAX_SOCK_SOCKAPP_CHANNELS)
            {
                /* User specified details */
                pPsiSockInputDetails = &pPsiSockInputDetail->sock_sockapp_ch_details[path - PSI_PATH_GLO_SYS_SOCKAPP_SOCK_01].sock_detail;

                pDetail->linger_time                        = pPsiSockInputDetails->linger_time;
                pDetail->rec_buffer_max_len                 = pPsiSockInputDetails->rec_buffer_max_len;
                pDetail->send_buffer_max_len                = pPsiSockInputDetails->send_buffer_max_len;
                pDetail->sock_close_at_once                 = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->sock_close_at_once               ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_DONTROUTE_        = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_DONTROUTE_      ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_LINGER_           = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_LINGER_         ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_RCVBUF_           = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_RCVBUF_         ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_REUSEADDR_        = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_REUSEADDR_      ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_SNDBUF_           = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_SNDBUF_         ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_TCPNOACKDELAY_    = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_TCPNOACKDELAY_  ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_TCPNODELAY_       = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_TCPNODELAY_     ) ? LSA_TRUE : LSA_FALSE;
                pDetail->socket_option.SO_BROADCAST_        = (PSI_FEATURE_ENABLE == pPsiSockInputDetails->socket_option.SO_BROADCAST_      ) ? LSA_TRUE : LSA_FALSE;
            }
            else
            {
                PSI_FATAL(0); /* Check PSI_CFG_MAX_SOCK_APP_CHANNELS */
            }

            mbx_id_rsp    = PSI_MBX_ID_SOCKAPP;
            comp_id_lower = LSA_COMP_ID_UNUSED;
        }
        break;
        #endif

		case PSI_PATH_GLO_SYS_SNMPX_SOCK: /* Details for the SNMP Manager sockets (each "session" uses its own socket) */
		{
			pDetail->socket_option.SO_REUSEADDR_     = LSA_FALSE; 
			pDetail->socket_option.SO_DONTROUTE_     = LSA_FALSE; 
			pDetail->socket_option.SO_TCPNOACKDELAY_ = LSA_FALSE; /* Don't care */ 
			pDetail->socket_option.SO_TCPNODELAY_    = LSA_FALSE; /* Don't care */ 
			pDetail->sock_close_at_once              = LSA_TRUE;  
			pDetail->socket_option.SO_LINGER_        = LSA_FALSE; 
			pDetail->linger_time                     = 0;         
			pDetail->socket_option.SO_SNDBUF_        = LSA_TRUE;  
			pDetail->send_buffer_max_len             = 3 * 1500;  /* ~3 UDP Frames */ 
			pDetail->socket_option.SO_RCVBUF_        = LSA_TRUE;  
			pDetail->rec_buffer_max_len              = 3 * 1500;  /* ~3 UDP Frames */ 
			pDetail->socket_option.SO_BROADCAST_     = LSA_FALSE;

			mbx_id_rsp    = PSI_MBX_ID_SNMPX;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

		case PSI_PATH_GLO_SYS_OHA_SOCK: /* Details for SNMP Agent socket (UDP port 161) */
		{			
			pDetail->socket_option.SO_REUSEADDR_     = LSA_FALSE; 
			pDetail->socket_option.SO_DONTROUTE_     = LSA_FALSE; 
			pDetail->socket_option.SO_TCPNOACKDELAY_ = LSA_FALSE; /* Don't care */ 
			pDetail->socket_option.SO_TCPNODELAY_    = LSA_FALSE; /* Don't care */ 
			pDetail->sock_close_at_once              = LSA_TRUE;  
			pDetail->socket_option.SO_LINGER_        = LSA_FALSE; 
			pDetail->linger_time                     = 0;         
			pDetail->socket_option.SO_SNDBUF_        = LSA_TRUE;  
			pDetail->send_buffer_max_len             = PSI_CFG_MAX_DIAG_TOOLS * 1500;  /* UDP Frames / depending on count of diagnosis tools doing simultanuos requests (1/tool) */ 
			pDetail->socket_option.SO_RCVBUF_        = LSA_TRUE;  
			pDetail->rec_buffer_max_len              = PSI_CFG_MAX_DIAG_TOOLS * 1500;  /* UDP Frames / depending on count of diagnosis tools doing simultanuos requests (1/tool) */ 
			pDetail->socket_option.SO_BROADCAST_     = LSA_FALSE;

			mbx_id_rsp    = PSI_MBX_ID_OHA;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;

		case PSI_PATH_GLO_SYS_CLRPC_SOCK: /* Details for CLRPC sockets (EPM, IOC, IOD(s), Client functionality) */
		{
			/*
				Identified PNIO CLRPC Users are:

					CLRPC (connectionless RPC) uses UDP sockets
					the settings have to cover all use cases
			
					EPM (Endpoint Mapper)
					- is a CLRPC server (incoming calls)
					- uses 1 socket (EPM is global)
					- handles EPM Query (diagnosis tool)
					- handles first fragment of a call (forwarding)
			
					IOC (IO Controller RPC interface)
					- is a CLRPC server (incoming calls)
					- uses 1 socket per interface
					- handles ApplReady callback from IOD to IOC
					- handles Read-Implicit (diagnosis tool)
			
					IOD (IO Device RPC interface)
					- is a CLRPC server (incoming calls)
					- uses 1 sockets per device and interface
					- handles Connect/Read/Write/PrmEnd/Release (one after the other, not parallel)
					- handles Read-Implicit (diagnosis tool)
			
					Client functionality
					- for outgoing calls (all clients of a channel use the same socket)
					- uses 1 socket per interface (CM channels of CM_PATH_TYPE_RPC)
					- but see too other CLRPC channels of CLRPC_PATH_TYPE_USER (callers of CLRPC_OPC_SET_INTERFACE_PARAM; e.g., easy supervisor)

				The formula for calculation of the sock send/rcv buffer is based on:

					CLRPC streams after the first fragment -> CLRPC_MAX_NOFACK_SIZE = 3 fragments
					After that it waits for FACK. After that send-window is filled again and so on.

				1) IOC, RPC client (Connect, ...)

					a) IOC Startup, "max_connects_per_second" doing Connect, Write, PrmEnd
						Response (e.g. multiple Write) -> CLRPC_MAX_WINDOW_SIZE for each device

					b) IOC Operation, MAX_DEVICES are doing Read
						everyone responds -> for CLRPC_MAX_WINDOW_SIZE


				2) IOC, RPC server (ApplReady, Implicit-Read)

					a) IOC Startup, MAX_DEVICES (not: "max_connects_per_second") are doing ApplReady
						a1) first fragment is processed over EPM
						a2) further fragments directly address IOC -> CLRPC_MAX_WINDOW_SIZE for each device

					b) Plug on all devices, MAX_DEVICES are doing ApplReady
						as it isn't first response it's not going over EPM
						all fragments are directly addressed to IOC -> CLRPC_MAX_WINDOW_SIZE for each device

					c) In parallel STEP7 or DiagTool are doing Implicit-Read (request fits into 1 fragment)

				3) IOD, RPC server (for each Device-Instance = 1 Socket)

					a) max_ars_IOC ... je AR one after another: Connect, Write, PrmEnd, Read/Write -> for each CLRPC_MAX_WINDOW_SIZE
						firs fragment of Connect is targeted at EPM

					b) max_ars_DAC -> Write(s) per CLRPC_MAX_WINDOW_SIZE

					c) In parallel STEP7 or DiagTool are doing Implicit-Read (request fits into 1 fragment)


				4) EPM (is an RPC server)

					a) IOC Startup, refer to (a1) above
						first fragment of "max_connects_per_second" devices

					b) iDevice Connect
						first fragment of max_ars_IOC controllern

					c) In parallel STEP7 or DiagTool are doing EPM-Query (request fits into 1 fragment)

				=> SO_RCVBUF must fit for all:

				1.5 KB = UDP Payload of an unfragmented IP telegram + additional bytes for IP stack 
				         specific management information (=16 byte on Interniche)

				for (1) IOC = 3*1.5KB * MAX_DEVICES
				for (2) IOC = 3*1.5KB * MAX_DEVICES + 1.5KB * NrOfTools
				for (3) IOD = 3*1.5KB * (max_ars_IOC + max_ars_DAC) + 1.5KB * NrOfTools
				for (4) EPM = IfCount * (3*1.5KB * max_connects_per_second + 1.5KB * (max_ars_IOC + max_ars_DAC) + 1.5KB * NrOfTools)

				For send/receive buffer len the max of (1),(2),(3),(4) is required.
			*/

			LSA_UINT32 frame_len = 1500;
			LSA_UINT32 max_ars_all_ifs = psi_get_max_ar_count_all_ifs();
			LSA_UINT32 ioc_sr_len_1 = ( 3 * frame_len * (PSI_CFG_MAX_CL_DEVICES) );
			LSA_UINT32 ioc_sr_len_2 = ( 3 * frame_len * (PSI_CFG_MAX_CL_DEVICES) ) + ( frame_len * (PSI_CFG_MAX_DIAG_TOOLS) );
			LSA_UINT32 iod_sr_len   = ( 3 * frame_len * max_ars_all_ifs) + ( frame_len * (PSI_CFG_MAX_DIAG_TOOLS) );
			LSA_UINT32 epm_sr_len   = (PSI_CFG_MAX_IF_CNT) * (( 3 * frame_len * (PSI_CFG_MAX_CM_CL_CONNECTS_PER_SECOND)) + ( frame_len * max_ars_all_ifs ) + ( frame_len * (PSI_CFG_MAX_DIAG_TOOLS) ));
			
			LSA_UINT32 max_sr_len   = LSA_MAX(LSA_MAX(ioc_sr_len_1, ioc_sr_len_2), LSA_MAX(iod_sr_len, epm_sr_len));
			
			pDetail->socket_option.SO_REUSEADDR_     = LSA_FALSE;
			pDetail->socket_option.SO_DONTROUTE_     = LSA_FALSE;
			pDetail->sock_close_at_once              = LSA_TRUE;
			pDetail->socket_option.SO_LINGER_        = LSA_FALSE;
			pDetail->linger_time                     = 0;
			pDetail->socket_option.SO_SNDBUF_        = LSA_TRUE;
			pDetail->send_buffer_max_len             = (LSA_INT32)max_sr_len; // cast to prevent the lint warning: --e(713): Loss of precision (assignment) (unsigned int to int)
			pDetail->socket_option.SO_RCVBUF_        = LSA_TRUE;
			pDetail->rec_buffer_max_len              = (LSA_INT32)max_sr_len; // cast to prevent the lint warning: --e(713): Loss of precision (assignment) (unsigned int to int)
			pDetail->socket_option.SO_TCPNOACKDELAY_ = LSA_FALSE; /* Don't care */
			pDetail->socket_option.SO_TCPNODELAY_    = LSA_FALSE; /* Don't care */
			pDetail->socket_option.SO_BROADCAST_     = LSA_FALSE;

			mbx_id_rsp    = PSI_MBX_ID_CLRPC;
			comp_id_lower = LSA_COMP_ID_UNUSED;
		}
		break;
        
		default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT( rc == LSA_RET_OK );

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_sock_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
		break;
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_SOCK;
		pSys->comp_id_lower         = comp_id_lower;
		pSys->comp_id_edd           = pDetailsIF->edd_comp_id;
		pSys->mbx_id_rsp            = mbx_id_rsp;               // MBX for responses
		pSys->hSysDev               = pDetailsIF->hSysDev;      // Sys handle for HW out functions
		pSys->hPoolNrtTx            = pDetailsIF->hPoolNrtTx;	// NRT TX pool handle for NRT mem
		pSys->hPoolNrtRx            = pDetailsIF->hPoolNrtRx;	// NRT RX pool handle for NRT mem
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
LSA_RESULT psi_sock_release_path_info(
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_VOID_PTR_TYPE   detail_ptr)
{
	LSA_UINT16             rc;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	PSI_ASSERT( pSys->comp_id == LSA_COMP_ID_SOCK );

    PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_sock_release_path_info()" );

    PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_SOCK, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return (LSA_RET_OK);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sock_open_channel(
    LSA_SYS_PATH_TYPE   sys_path)
{
	LSA_UINT32                  trace_idx;
	SOCK_UPPER_RQB_PTR_TYPE     pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

	SOCK_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
    PSI_ASSERT(pRQB != LSA_NULL);

	/* note: using low byte of path as upper-handle (path without PNIO IF nr ! */
	pRQB->args.channel.handle                      = SOCK_INVALID_HANDLE;
	pRQB->args.channel.handle_upper                = (LSA_UINT8)path;
	pRQB->args.channel.sys_path                    = sys_path;
	pRQB->args.channel.sock_request_upper_done_ptr = (SOCK_UPPER_CALLBACK_FCT_PTR_TYPE)psi_sock_channel_done;

	PSI_RQB_SET_HANDLE(pRQB, SOCK_INVALID_HANDLE);
	PSI_RQB_SET_OPCODE(pRQB, SOCK_OPC_OPEN_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_SOCK);

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sock_open_channel(): Open SOCK channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x)", 
		pnio_if_nr, hd_nr, pRQB->args.channel.sys_path, path );

	/* SOCK is on LD side --> open can be done direct */
	psi_request_start(PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sock_close_channel(
    LSA_SYS_PATH_TYPE   sys_path, 
    LSA_HANDLE_TYPE     handle)
{
	LSA_UINT32                  trace_idx;
	SOCK_UPPER_RQB_PTR_TYPE     pRQB;

	LSA_UINT16 const path       = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_PATH(sys_path);
	LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	PSI_ASSERT(path != PSI_SYS_PATH_INVALID);

	SOCK_ALLOC_LOCAL_MEM( (LSA_VOID_PTR_TYPE *)&pRQB, sizeof(*pRQB) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_RQB_SET_HANDLE(pRQB, handle);
	PSI_RQB_SET_OPCODE(pRQB, SOCK_OPC_CLOSE_CHANNEL);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_SOCK);

	LSA_RQB_SET_USER_ID_UVAR16(pRQB, sys_path); // ID is used for unregister handle

	trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

	PSI_SYSTEM_TRACE_05( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sock_close_channel(): Close SOCK channel, pnio_if_nr(%u) hd_nr(%u) sys_path(%#x) path(%#x) handle(%u)", 
		pnio_if_nr, hd_nr, sys_path, path, handle );

	/* SOCK is on LD side --> close can be done direct */
	psi_request_start(PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_sock_channel_done(
    LSA_VOID_PTR_TYPE   rqb_ptr)
{
	LSA_UINT16                    rc;
	LSA_UINT32                    trace_idx;
	LSA_OPCODE_TYPE               opc;
	SOCK_UPPER_RQB_PTR_TYPE const pRQB = (SOCK_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(SOCK_RQB_GET_RESPONSE(pRQB) == SOCK_RSP_OK);

	opc = SOCK_RQB_GET_OPCODE(pRQB);

    /* Handle response for SOCK Opcode */
    switch (opc)
	{
		case SOCK_OPC_OPEN_CHANNEL:
		{
			LSA_SYS_PATH_TYPE const sys_path = pRQB->args.channel.sys_path;
			LSA_HANDLE_TYPE   const handle   = pRQB->args.channel.handle;

			trace_idx = psi_get_trace_idx_for_sys_path(pRQB->args.channel.sys_path);

			PSI_SYSTEM_TRACE_04( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sock_channel_done(): Open SOCK channel done, rsp(%u/%#x) sys_path(%#x) handle(%u)",
				SOCK_RQB_GET_RESPONSE(pRQB), 
				SOCK_RQB_GET_RESPONSE(pRQB), 
				pRQB->args.channel.sys_path, 
				pRQB->args.channel.handle );

			SOCK_FREE_LOCAL_MEM( &rc, pRQB );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Register handle for the channel
			psi_ld_open_channels_done(sys_path, handle);
		}
		break;

        case SOCK_OPC_CLOSE_CHANNEL:
		{
			// Unregister handle for this PNIO IF (stored in USER-ID)
			LSA_SYS_PATH_TYPE const sys_path = LSA_RQB_GET_USER_ID_UVAR16(pRQB);

			trace_idx = psi_get_trace_idx_for_sys_path(sys_path);

			PSI_SYSTEM_TRACE_03( trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_sock_channel_done(): Close SOCK channel done, rsp(%u/%#x) sys_path(%#x)",
				SOCK_RQB_GET_RESPONSE(pRQB), 
				SOCK_RQB_GET_RESPONSE(pRQB), 
				sys_path );

			SOCK_FREE_LOCAL_MEM( &rc, pRQB );
			PSI_ASSERT(rc == LSA_RET_OK);

			// Unregister handle for the channel
			psi_ld_close_channels_done(sys_path);
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_sock_channel_done(): invalid opcode(%u/%#x)", opc, opc );
			PSI_FATAL( 0 );
		}
	}
}

#endif  //  (PSI_CFG_USE_LD_COMP == 1)

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

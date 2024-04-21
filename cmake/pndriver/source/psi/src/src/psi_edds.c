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
/*  F i l e               &F: psi_edds.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for EDDS                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   107
#define PSI_MODULE_ID       107 /* PSI_MODULE_ID_PSI_EDDS */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))

#include "edds_iobuf_usr.h" // internal header

#if defined (PSI_EDDS_CFG_HW_INTEL) // using INTEL LL
#include "edds_inc.h"       // EDDS headerfiles
#include "edds_int.h"
#include "intel_inc.h"      // EDDS Lower Layer Intel Driver
#endif

#if defined(PSI_EDDS_CFG_HW_KSZ88XX)
#include "ksz88xx_inc.h"    // EDDS Lower Layer Micrel Driver
#endif

#if defined(PSI_EDDS_CFG_HW_TI)
#include "ti_inc.h"         // EDDS Lower Layer TI Driver
#endif

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== EDDS ==================================================================*/
/*===========================================================================*/

static EDDS_CDB_TYPE * psi_init_CDB_EDDS(
	EDDS_CDB_TYPE     * const cdb,
	EDDS_CDB_NRT_TYPE * const pNRT,
	LSA_VOID_PTR_TYPE   const hDDB,
	LSA_UINT32          const filter )
{
	// *--------------------------------------
	// *  CDB Header
	// *--------------------------------------
	cdb->UsedComp.UseNRT    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseCSRT   = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UsePRM    = EDD_CDB_CHANNEL_USE_OFF;

	cdb->hDDB               = hDDB;
	cdb->InsertSrcMAC       = EDD_CDB_INSERT_SRC_MAC_DISABLED; 
	cdb->pNRT               = LSA_NULL;
	cdb->pPRM               = LSA_NULL;

	// *--------------------------------------
	// *  NRT configuration
	// *--------------------------------------
	if (pNRT != LSA_NULL)
	{
		pNRT->FrameFilter       = filter;
		cdb->pNRT               = pNRT;
		cdb->UsedComp.UseNRT    = EDD_CDB_CHANNEL_USE_ON;
	}

	return cdb;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_edds_get_path_info(
	LSA_SYS_PTR_TYPE  * const sys_ptr_ptr,
	LSA_VOID_PTR_TYPE * const detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   const sys_path )
{
	EDDS_DETAIL_PTR_TYPE              pDetail;
	PSI_SYS_PTR_TYPE                  pSys;
	EDDS_UPPER_CDB_NRT_PTR_TYPE       pNRT;
	EDDS_UPPER_CDB_PRM_PTR_TYPE       pPRM;
	EDDS_HANDLE                       hDDB;
    LSA_USER_ID_TYPE                  user_id;
    LSA_UINT16                        mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_BOOL                          arp_state     = LSA_FALSE;
	LSA_UINT16                        result        = LSA_RET_OK;
	LSA_UINT16                  const hd_nr         = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16                  const path          = PSI_SYSPATH_GET_PATH(sys_path);
	PSI_DETAIL_STORE_PTR_TYPE   const pDetailsIF    = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr    != LSA_NULL);
    PSI_ASSERT(detail_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDS);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_edds_get_path_info(): sys_path(%#x) pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)",
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

	hDDB = psi_hd_get_hDDB(hd_nr);
	PSI_ASSERT(hDDB);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	PSI_MEMSET(pDetail, 0, sizeof(*pDetail));

	switch (path)
	{
	    case PSI_PATH_IF_SYS_MRP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_MRP);

			mbx_id_rsp = PSI_MBX_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ARP);

            #if (PSI_CFG_USE_HIF_HD == 1)
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_TCIP;
            }

            arp_state = LSA_TRUE; // mark this channel for ARP check at response
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ICMP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_ICMP);

            #if (PSI_CFG_USE_HIF_HD == 1)
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_TCIP;
            }
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_UDP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			if (pDetailsIF->tcip.multicast_support_on) // MC support on ?
			{
				pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_IGMP);
			}
			else
			{
				pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP);
			}

            #if (PSI_CFG_USE_HIF_HD == 1)
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_TCIP;
            }
		}
		break;

        #if defined(PSI_CFG_TCIP_CFG_SUPPORT_PATH_EDD_TCP)
	    case PSI_PATH_IF_SYS_TCIP_EDD_TCP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_TCP);

            #if (PSI_CFG_USE_HIF_HD == 1)
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_TCIP;
            }
		}
		break;
        #endif

	    case PSI_PATH_IF_SYS_DCP_EDD:
		{
			LSA_UINT32 const filter = EDD_NRT_FRAME_DCP_HELLO /* | EDD_NRT_FRAME_UDP_DCP_HELLO */;

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, filter);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{
            // ** user channel to NARE (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, 0);

			mbx_id_rsp = PSI_MBX_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{
            // ** user channel to OHA (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, 0);

            #if (PSI_CFG_USE_HIF_HD == 1)
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_OHA;
            }
		}
		break;

		// ** user channel to DCP, NRT data **
	    case PSI_PATH_IF_SYS_OHA_DCP_EDD:  /* whichever comes first! */
	    case PSI_PATH_IF_SYS_NARE_DCP_EDD: /* whichever comes first! */
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			LSA_UINT32 const filter = EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_PTCP_DELAY;

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, filter);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{ 
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_ANNO);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{ // ** user channel to LLDP with LLDP receive filter (not supported yet)**

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LLDP);

			mbx_id_rsp = PSI_MBX_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD: // ** user channel to ACP with ASRT and CSRT data)**
		{
			LSA_UINT32 const filter = EDD_NRT_FRAME_ASRT | EDD_NRT_FRAME_UDP_ASRT;

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, filter);

			mbx_id_rsp = PSI_MBX_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{ // ** user channel to CM (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pPRM), user_id, sizeof(*pPRM), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pPRM != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, 0);

            pDetail->UsedComp.UseCSRT = EDD_CDB_CHANNEL_USE_ON;
			pDetail->UsedComp.UsePRM  = EDD_CDB_CHANNEL_USE_ON;
			pDetail->pPRM             = pPRM;

			/* Note: Must match with OHA allow_non_pnio_mautypes setting */
			pPRM->PortDataAdjustLesserCheckQuality = (pDetailsIF->oha.allow_non_pnio_mautypes)?EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY:EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD;

			mbx_id_rsp = PSI_MBX_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail = psi_init_CDB_EDDS(pDetail, LSA_NULL, hDDB, 0);

			mbx_id_rsp = PSI_MBX_ID_POF;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LEN_TSYNC);
			
			pDetail->InsertSrcMAC = EDD_CDB_INSERT_SRC_MAC_ENABLED;

			mbx_id_rsp = PSI_MBX_ID_PSI; // PSI EDD user hub
		}
		break;	

		case PSI_PATH_IF_SYS_HSA_EDD:
		{
			// ** user channel to NARE (NRT send only)**
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			/* If the input parameters >additional_forwarding_rules_supported p_hd_in->application_exist are set, the value bHsyncAppl is true.
			   In this case, we register for the frame types EDD_NRT_FRAME_HSYNC. HSA will receive and send frames using this channel.
			   In the other case, HSA does not receive or send HSync frames, but HSA will use this channel to call EDD_SRV_GET_PARAMS.
			*/
			if (pDetailsIF->hsa.bHsyncAppl)
			{
				pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, EDD_NRT_FRAME_HSYNC); // NRT for sending HSA Frames
			}
			else
			{
				pDetail = psi_init_CDB_EDDS(pDetail, pNRT, hDDB, 0 /* use invalid EDD_NRT_FRAME type - we don't want to receive any frame using this channel. 0 == no reception. */);
			}

            #if (PSI_CFG_USE_HIF_HD == 1) // Using HIF HD?
            if (pDetailsIF->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    mbx_id_rsp = PSI_MBX_ID_HIF_HD;
            }
            else
            #endif
            {
			    mbx_id_rsp = PSI_MBX_ID_HSA;
            }
		}
		break;

	    default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_edds_get_path_info(): invalid path(%#x/%u), sys_path(%#x) hd_nr(%u) result(0x%x)",
                path, path, sys_path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_EDDS;
		pSys->comp_id_lower         = LSA_COMP_ID_UNUSED;
		pSys->comp_id_edd           = pDetailsIF->edd_comp_id;
		pSys->mbx_id_rsp            = mbx_id_rsp;
		pSys->hSysDev               = pDetailsIF->hSysDev;      // Sys handle for HW out functions
		pSys->hPoolDev              = pDetailsIF->hPoolDev;     // DEV pool handle
		pSys->hPoolNrtTx            = pDetailsIF->hPoolNrtTx;   // NRT TX pool handle for NRT mem
		pSys->hPoolNrtRx            = pDetailsIF->hPoolNrtRx;   // NRT RX pool handle for NRT mem
		pSys->check_arp             = arp_state;
		pSys->trace_idx             = pDetailsIF->trace_idx;
        pSys->hd_runs_on_level_ld   = pDetailsIF->hd_runs_on_level_ld;
        pSys->psi_path              = path;

		*detail_ptr_ptr = pDetail;
		*sys_ptr_ptr    = pSys;
	}

    return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_edds_release_path_info(
	LSA_SYS_PTR_TYPE  const sys_ptr,
	LSA_VOID_PTR_TYPE const detail_ptr )
{
	LSA_UINT16                 rc;
	EDDS_DETAIL_PTR_TYPE const pDetail = (EDDS_DETAIL_PTR_TYPE)detail_ptr;
	PSI_SYS_PTR_TYPE     const pSys    = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_EDDS);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_edds_release_path_info()" );

	if (pDetail->pNRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pNRT, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	if (pDetail->pPRM != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pPRM, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_EDDS, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/* EDDS system calls for device open, setup, close                            */
/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_setup_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_EDDS_SETUP_PTR_TYPE       p_setup;
	PSI_EDDS_SESSION_PTR_TYPE     p_session;

	PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
    p_session = &(p_setup->session);
	PSI_ASSERT(p_session->req_done_func != LSA_NULL);

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_setup_request_done(): hd_nr(%u)", p_session->hd_nr);

    psi_system_request_done(PSI_MBX_ID_PSI, p_session->req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_close_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_EDDS_SESSION_PTR_TYPE       p_session;

	PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_session = (PSI_EDDS_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->req_done_func != LSA_NULL);

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_close_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_session->hd_nr, pRQB);

	psi_system_request_done(PSI_MBX_ID_PSI, p_session->req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_open_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	EDD_UPPER_RQB_PTR_TYPE      const pRQB                       = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
    PSI_RQB_PTR_TYPE            const psi_hd_open_device_rqb_ptr = psi_hd_get_hd_open_device_rqb_ptr();
    PSI_HD_OUTPUT_PTR_TYPE      const p_hd_out                   = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_out;
    PSI_HD_INPUT_PTR_TYPE       const p_hd_in                    = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_args;
	PSI_EDDS_SESSION_PTR_TYPE         p_session;
	LSA_UINT16                        retVal;
    LSA_UINT32                        port_idx;
    LSA_UINT8                         MediaType, IsPOF, PhyType, FXTransceiverType;
	LSA_UINT16                        cnsCnt, provCnt;
	PSI_EDDS_SETUP_PTR_TYPE           p_setup;
	PSI_HD_STORE_PTR_TYPE             p_hd;
	EDDS_UPPER_DDB_INI_PTR_TYPE       pOpen;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDS_SRV_DEV_OPEN == EDD_RQB_GET_SERVICE(pRQB));

	pOpen = (EDDS_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	PSI_ASSERT(pOpen != LSA_NULL);

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
    p_session = &(p_setup->session);

	p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
    PSI_ASSERT(p_hd->is_used);

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_open_done(): hd_nr(%u)", p_session->hd_nr);

    // GigabitSupport enabled?
    if (PSI_FEATURE_ENABLE == p_session->b_gigabit_support)
    {
        p_hd->hd_hw.edd.edds.io_buffer_handle = LSA_NULL;
    }
    else // GigabitSupport not enabled
    {
        if (p_setup->iniDPB.NRT.IO_Configured == EDD_SYS_IO_CONFIGURED_ON) // are IO data configured?
        {
            PSI_ASSERT(pOpen->pDPB->CSRT.hIOBufferManagement != LSA_NULL); // it was allocated by EDDS on DEV_OPEN
            p_hd->hd_hw.edd.edds.io_buffer_handle = EDDS_IOBuffer_Init(pOpen->pDPB->CSRT.hIOBufferManagement, 1, &cnsCnt, &provCnt);
	        PSI_ASSERT(p_hd->hd_hw.edd.edds.io_buffer_handle != LSA_NULL);
	        PSI_ASSERT(cnsCnt  > 1);
	        PSI_ASSERT(provCnt > 1);
        }
        else
        {
            p_hd->hd_hw.edd.edds.io_buffer_handle = LSA_NULL;
        }
    }

	// Register hDDB for this board
	*(p_session->hDDB_ptr) = pOpen->hDDB;

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_OPEN_DONE;

    for (port_idx = 0; port_idx < p_hd->nr_of_ports; port_idx++)
    {
        LSA_UINT16 const HwPortId = p_hd->hd_hw.port_map[port_idx+1].hw_port_id;

        /*------------------------------------------*/
        /* get port parameters from system adaption */
        /*------------------------------------------*/
        PSI_EDD_GET_MEDIA_TYPE(
            p_hd->hd_sys_handle,
            HwPortId,
            PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO,
            &MediaType,
            &IsPOF,
            &PhyType,
            &FXTransceiverType); /* not used here */

        /*------------------------------------------*/
        /* update output parameters                 */
        /*------------------------------------------*/
        /* Media-Type */
        p_hd_out->hw_port[port_idx].media_type = (PSI_PORT_MEDIA_TYPE)MediaType;

        /* Media-Type for Port Submodule Ident Numbers */
        switch (MediaType)
        {
            case PSI_MEDIA_TYPE_COPPER:
                p_hd_out->hw_port[port_idx].media_type_psubmodidtnr = PSI_MEDIA_TYPE_PSUBMODIDTNR_COPPER;
            break;

            case PSI_MEDIA_TYPE_FIBER:
                p_hd_out->hw_port[port_idx].media_type_psubmodidtnr = PSI_MEDIA_TYPE_PSUBMODIDTNR_FO;
            break;

            case PSI_MEDIA_TYPE_WIRELESS:
                p_hd_out->hw_port[port_idx].media_type_psubmodidtnr = PSI_MEDIA_TYPE_PSUBMODIDTNR_RADIO;
            break;

            default:
                // the init value is from psi_res_calc_init_hd_outputs()
		    break;
        }

        /* Fiber-Optic-Type */
        /* a port is then a POF port, if            */
        /*  - MediaType = FIBER_OPTIC_CABLE and     */
        /*  - this is a POF-Mode                    */
        if ((MediaType == PSI_MEDIA_TYPE_FIBER) && (IsPOF == EDD_PORT_OPTICALTYPE_ISPOF))
        {
            p_hd_out->hw_port[port_idx].fiberoptic_type = PSI_FIBEROPTIC_POF;
        }

        /* PHY-Type */
        p_hd_out->hw_port[port_idx].phy_type = (PSI_PORT_PHY_TYPE)PhyType;

        /* MII-Type */
        p_hd_out->hw_port[port_idx].xmii_type = PSI_PORT_MII_UNKNOWN;

        /* EDD-Asic-Type */
        p_hd_out->hw_port[port_idx].edd_asic_type = (PSI_ASIC_TYPE)(p_hd_in->asic_type);

        /* is ring port? */
        if (p_hd_in->nr_of_ports == 2)
        {
            p_hd_out->hw_port[port_idx].isRingPort = PSI_PORT_DEFAULT_RING_PORT; // Micrel2P
        }
        else
        {
            p_hd_out->hw_port[port_idx].isRingPort = PSI_PORT_NO_DEFAULT_RING_PORT;
        }

        PSI_HD_TRACE_08(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_open_done(): HD-Outputs: HwPortId(%u) media_type(0x%x) media_type_psubmodidtnr(0x%x) fiberoptic_type(0x%x) phy_type(0x%x) xmii_type(0x%x) edd_asic_type(0x%x) isRingPort(%u)",
            HwPortId,
            p_hd_out->hw_port[port_idx].media_type,
            p_hd_out->hw_port[port_idx].media_type_psubmodidtnr,
            p_hd_out->hw_port[port_idx].fiberoptic_type,
            p_hd_out->hw_port[port_idx].phy_type,
            p_hd_out->hw_port[port_idx].xmii_type,
            p_hd_out->hw_port[port_idx].edd_asic_type,
            p_hd_out->hw_port[port_idx].isRingPort);
    }

    EDDS_FREE_UPPER_MEM_LOCAL(&retVal, pRQB->pParam);
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDS_FREE_UPPER_RQB_LOCAL(&retVal, pRQB);
	PSI_ASSERT(retVal == EDD_STS_OK);

	// continue with device setup
	psi_edds_device_setup(p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_open(
    PSI_EDDS_SETUP_PTR_TYPE const p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	PSI_EDDS_SESSION_PTR_TYPE   p_session;
	EDDS_UPPER_DDB_INI_PTR_TYPE pOpen;
	EDDS_UPPER_DPB_PTR_TYPE     pDPB;
	PSI_HD_STORE_PTR_TYPE       p_hd;
    PSI_EDD_SYS_PTR_TYPE        pSys;
    LSA_UINT16                  mbx_id;

	PSI_ASSERT(p_setup != LSA_NULL);
    p_session = &(p_setup->session);
	PSI_ASSERT(p_session->hDDB_ptr != LSA_NULL);

	pDPB = &(p_setup->iniDPB);
    //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'p_setup->iniDPB'
    PSI_ASSERT(pDPB != LSA_NULL);

	EDDS_ALLOC_UPPER_RQB_LOCAL(&pRQB, sizeof(*pRQB));
	PSI_ASSERT(pRQB != LSA_NULL);

	EDDS_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDS_RQB_DDB_INI_TYPE));
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	pOpen = (EDDS_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);

	//set opcode and service
	EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDS);
    EDD_RQB_SET_SERVICE(pRQB, EDDS_SRV_DEV_OPEN);

	p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_OPEN_START;

    //Register setup data (session data) for next request in done
	p_session->req_done_func = psi_edds_device_open_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

    mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
    pSys   = (PSI_EDD_SYS_PTR_TYPE)(p_session->hd_sys_handle);
	PSI_ASSERT(pSys);
    PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));
    //modify EDDS SysPtr entry
    pSys->mbx_id_edds_rqb = mbx_id;

	pOpen->pDPB = pDPB;
	pOpen->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_edds_setup_request_done;
	pOpen->hDDB = LSA_NULL;

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_open(): hd_nr(%u)", p_session->hd_nr);

    psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_setup_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_EDDS_SESSION_PTR_TYPE       p_session;
	LSA_UINT16                      retVal;
    LSA_UINT8                       bRetVal;
    PSI_EDDS_SETUP_PTR_TYPE         p_setup;
	PSI_HD_STORE_PTR_TYPE           p_hd;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDS_SRV_DEV_SETUP == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDS_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
    p_session = &(p_setup->session);

	p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_SETUP_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_setup_done(): hd_nr(%u)", p_session->hd_nr);

    EDDS_FREE_UPPER_MEM_LOCAL(&retVal, pRQB->pParam);
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDS_FREE_UPPER_RQB_LOCAL(&retVal, pRQB);
	PSI_ASSERT(retVal == EDD_STS_OK);

    // Enable the PNIO IRs
	PSI_HD_ENABLE_EVENT(p_session->hd_sys_handle);

	// call PSI_EDDS_NOTIFY_FILL_SETTINGS here by PSI itself
    bRetVal = PSI_EDDS_NOTIFY_FILL_SETTINGS(p_session->hd_sys_handle,
	                                        (EDD_SYS_FILL_INACTIVE != p_setup->iniDPB.NRT.FeedInLoadLimitationActive)?LSA_TRUE:LSA_FALSE,
	                                        (EDD_SYS_IO_CONFIGURED_OFF != p_setup->iniDPB.NRT.IO_Configured)?LSA_TRUE:LSA_FALSE,
                                            p_hd->hd_hw.edd.edds.bHsyncModeActive);
    LSA_UNUSED_ARG(bRetVal);

	// Device open sequence finished --> notify done
	psi_hd_edd_open_done(p_session->hd_nr, p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_setup(
    PSI_EDDS_SETUP_PTR_TYPE const p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	PSI_EDDS_SESSION_PTR_TYPE     p_session;
	EDDS_UPPER_DSB_PTR_TYPE       pDSB;
 	EDDS_UPPER_DDB_SETUP_PTR_TYPE pDDB;
	PSI_HD_STORE_PTR_TYPE         p_hd;
    LSA_UINT16                    mbx_id;

	PSI_ASSERT(p_setup != LSA_NULL);
    p_session = &(p_setup->session);
	PSI_ASSERT(p_session->hDDB_ptr != LSA_NULL);

	pDSB = &(p_setup->iniDSB); // Use setupblock as pParam

	EDDS_ALLOC_UPPER_RQB_LOCAL(&pRQB, sizeof(*pRQB));
	PSI_ASSERT(pRQB != LSA_NULL);

	EDDS_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDS_RQB_DDB_SETUP_TYPE));
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	// set opcode and service
	EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDS);
    EDD_RQB_SET_SERVICE(pRQB, EDDS_SRV_DEV_SETUP);

	p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_SETUP_START;

    // Register setup data (session data) for next request in done
	p_session->req_done_func = psi_edds_device_setup_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

    mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
    PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));

	pDDB        = (EDDS_UPPER_DDB_SETUP_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pDDB->pDSB  = pDSB;
	pDDB->Cbf   = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_edds_setup_request_done;
	pDDB->hDDB  = *(p_session->hDDB_ptr);

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_setup(): hd_nr(%u)", p_session->hd_nr);

    psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_shutdown_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      retVal;
	PSI_EDDS_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDS_SRV_DEV_SHUTDOWN == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

	// Unregister the EDDS IO Buffer handle (used for PI alloc/free)
	p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    if (p_hd->hd_hw.edd.edds.io_buffer_handle != LSA_NULL)
    {
	    EDDS_IOBuffer_Deinit(p_hd->hd_hw.edd.edds.io_buffer_handle);
	    p_hd->hd_hw.edd.edds.io_buffer_handle = LSA_NULL;
    }

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_SHUTDOWN_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_shutdown_done(): hd_nr(%u)", p_session->hd_nr);

    EDDS_FREE_UPPER_MEM_LOCAL(&retVal, pRQB->pParam);
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDS_FREE_UPPER_RQB_LOCAL(&retVal, pRQB);
	PSI_ASSERT(retVal == EDD_STS_OK);

	// Continue with device close
	psi_edds_device_close(p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_shutdown(
    PSI_EDDS_SESSION_PTR_TYPE const p_session )
{
	EDD_UPPER_RQB_PTR_TYPE           pRQB;
	EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE pShutdown;
	PSI_HD_STORE_PTR_TYPE            p_hd;
    LSA_UINT16                       mbx_id;

	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->hDDB_ptr != LSA_NULL);

	EDDS_ALLOC_UPPER_RQB_LOCAL(&pRQB, sizeof(*pRQB));
	PSI_ASSERT(pRQB != LSA_NULL);

	EDDS_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDS_RQB_DDB_SHUTDOWN_TYPE));
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	// Set opcode and service
	EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDS);
    EDD_RQB_SET_SERVICE(pRQB, EDDS_SRV_DEV_SHUTDOWN);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_SHUTDOWN_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_edds_device_shutdown_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_session);

    mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
    PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));

	pShutdown       = (EDDS_UPPER_DDB_SHUTDOWN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pShutdown->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_edds_close_request_done;
	pShutdown->hDDB = *(p_session->hDDB_ptr);

	// Disable the IR
	PSI_HD_DISABLE_EVENT(p_session->hd_sys_handle);

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_shutdown(): hd_nr(%u)", p_session->hd_nr);

    psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_edds_device_close_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      retVal;
	PSI_EDDS_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDS_SRV_DEV_CLOSE == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDS_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

	// Reset the hDDB 
	*(p_session->hDDB_ptr) = LSA_NULL;

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_CLOSE_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_close_done(): hd_nr(%u)", p_session->hd_nr);

    EDDS_FREE_UPPER_MEM_LOCAL(&retVal, pRQB->pParam);
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDS_FREE_UPPER_RQB_LOCAL(&retVal, pRQB);
	PSI_ASSERT(retVal == EDD_STS_OK);

	// Device close sequence finished --> notify done
	psi_hd_edd_close_done(p_session->hd_nr, p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_device_close(
    PSI_EDDS_SESSION_PTR_TYPE const p_session )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	EDDS_UPPER_DDB_REL_PTR_TYPE pClose;
	PSI_HD_STORE_PTR_TYPE       p_hd;
    LSA_UINT16                  mbx_id;

	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->hDDB_ptr != LSA_NULL);

	EDDS_ALLOC_UPPER_RQB_LOCAL(&pRQB, sizeof(*pRQB));
	PSI_ASSERT(pRQB != LSA_NULL);

	EDDS_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDS_RQB_DDB_REL_TYPE));
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	// Set opcode and service
	EDD_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDS);
    EDD_RQB_SET_SERVICE(pRQB, EDDS_SRV_DEV_CLOSE);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->edds_state = PSI_HD_EDDS_STATE_DEV_CLOSE_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_edds_device_close_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_session);

    mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
    PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));

	pClose          = (EDDS_UPPER_DDB_REL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pClose->Cbf     = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_edds_close_request_done;
	pClose->hDDB    = *(p_session->hDDB_ptr);

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_device_close(): hd_nr(%u)", p_session->hd_nr);

    psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_system, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edds_prepare_setup_data(
	PSI_EDDS_SETUP_PTR_TYPE const p_setup,
	PSI_HD_INPUT_PTR_TYPE   const p_inp,
	PSI_HD_STORE_PTR_TYPE   const p_hd )
{
	LSA_UINT16                  port_idx;
	PSI_HD_IF_INPUT_PTR_TYPE    if_ptr;
	PSI_HD_HW_PTR_TYPE          hd_hw_ptr;
	PSI_DETAIL_STORE_PTR_TYPE   pDetailsIF;
	PSI_HD_PORT_INPUT_PTR_TYPE  p_entry;
    EDD_RSP                     EddRspVal;

	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_inp   != LSA_NULL);
	PSI_ASSERT(p_hd    != LSA_NULL);

	if_ptr    = &(p_inp->pnio_if);
	hd_hw_ptr = &(p_hd->hd_hw);

	// ----------------- Init DPB data  -----------------
    // Use the default settings as base
    EddRspVal = edds_InitDPBWithDefaults(&p_setup->iniDPB);
    PSI_ASSERT(EddRspVal == EDD_STS_OK);

    // nLLIF parameter (hw-dependent)
	p_setup->iniDPB.pLLFunctionTable    = hd_hw_ptr->edd.edds.ll_function_table;
	p_setup->iniDPB.pLLManagementData   = hd_hw_ptr->edd.edds.ll_handle;
	p_setup->iniDPB.pLLParameter        = hd_hw_ptr->edd.edds.ll_parameter;

    #if defined(PSI_EDDS_CFG_HW_KSZ88XX)
    if (PSI_ASIC_TYPE_KSZ88XX == p_inp->asic_type)
    {
        ((KSZ88XX_LL_PARAMETER_PTR_TYPE)p_setup->iniDPB.pLLParameter)->ConfiguredPorts = p_inp->nr_of_ports; // user number of ports to Micrel-LL
    }
    #endif //PSI_EDDS_CFG_HW_KSZ88XX

    #if defined(PSI_EDDS_CFG_HW_TI)
    if (PSI_ASIC_TYPE_TI_AM5728 == p_inp->asic_type)
    {
        ((TI_LL_PARAMETER_PTR_TYPE)p_setup->iniDPB.pLLParameter)->ConfiguredPorts = p_inp->nr_of_ports; // user number of ports to TI-LL
    }
    #endif // PSI_EDDS_CFG_HW_TI

    // preset Gigabit-Support
    p_setup->session.b_gigabit_support = LSA_FALSE; // = PSI default configuration

    if (PSI_USE_SETTING_YES == p_inp->stdmac.use_setting) // Usage of StdMAC inputs?
    {
		PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_edds_prepare_setup_data(): gigabit_support(%u) nrt_copy_interface(%u)", 
            p_inp->stdmac.gigabit_support, p_inp->stdmac.nrt_copy_interface );

        if (PSI_FEATURE_ENABLE == p_inp->stdmac.gigabit_support) // Gigabit-Support is requested?
        {
            #if defined(PSI_EDDS_CFG_HW_INTEL) // using INTEL LL
            p_setup->session.b_gigabit_support = LSA_TRUE;
            #else
		    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_FATAL, "psi_edds_prepare_setup_data(): invalid params, Gigabit-Support only allowed with compiler-switch PSI_EDDS_CFG_HW_INTEL" );
		    PSI_FATAL( 0 );
            #endif //PSI_EDDS_CFG_HW_INTEL
        }

        if (PSI_FEATURE_ENABLE == p_inp->stdmac.nrt_copy_interface ) // NRT Copy Interface is requested?
        {
            hd_hw_ptr->b_edd_nrt_copy_if_on   = LSA_TRUE;                           // use EDDS-Copy-Interface
            p_setup->iniDPB.ZeroCopyInterface = EDDS_DPB_DO_NOT_USE_ZERO_COPY_IF;   // use EDDS-Copy-Interface
        }
    }

    // set parameter
	p_setup->iniDPB.NRT.FeedInLoadLimitationActive  = ( PSI_FEATURE_ENABLE == p_inp->fill_active ) ?            EDD_SYS_FILL_ACTIVE         : EDD_SYS_FILL_INACTIVE;
	p_setup->iniDPB.NRT.IO_Configured               = ( PSI_FEATURE_ENABLE == p_inp->io_configured ) ?          EDD_SYS_IO_CONFIGURED_ON    : EDD_SYS_IO_CONFIGURED_OFF;
    p_setup->iniDPB.SWI.MRAEnableLegacyMode         = ( PSI_FEATURE_ENABLE == p_inp->mra_enable_legacy_mode) ?  EDD_MRA_ENABLE_LEGACY_MODE  : EDD_MRA_DISABLE_LEGACY_MODE;

    #if defined(PSI_EDDS_CFG_HW_INTEL) // using INTEL LL
    if ( (PSI_ASIC_TYPE_INTEL_HARTWELL == p_inp->asic_type) || (PSI_ASIC_TYPE_INTEL_SPRINGVILLE == p_inp->asic_type) )
    {
        if (p_setup->session.b_gigabit_support) // Gigabit support is requested?
        {
            // set disable 1000 MBit support to FALSE
            ((INTEL_LL_PARAMETER_TYPE*)(p_setup->iniDPB.pLLParameter))->Disable1000MBitSupport = LSA_FALSE;
        }
        else // Gigabit-Support is not requested
        {
            // set disable 1000 MBit support to TRUE
            ((INTEL_LL_PARAMETER_TYPE*)(p_setup->iniDPB.pLLParameter))->Disable1000MBitSupport = LSA_TRUE;
        }
    }
    #endif //PSI_EDDS_CFG_HW_INTEL

	// ---- setup DPB ---------------
    p_setup->iniDPB.hSysDev             = (EDDS_SYS_HANDLE)(p_hd->hd_sys_handle);   // system device handle
	p_setup->iniDPB.InterfaceID         = if_ptr->edd_if_id;                        // The InterfaceID is a number (1...16) that clearly identifies the device.
	p_setup->iniDPB.TraceIdx            = if_ptr->trace_idx;                        // Trace index used for this interface

    #if (defined(PSI_EDDS_CFG_HW_INTEL) || defined(PSI_EDDS_CFG_HW_TI) || defined(PSI_EDDS_CFG_HW_IOT2000)) // using INTEL or TI or IOT2000 LL
    if (p_setup->session.b_gigabit_support) // Gigabit support is requested?
    {
        p_setup->iniDPB.RxBufferCnt = 2*1489;   // 1GBit, 1ms scheduler cycle (wirespeed with minimal packets)
        p_setup->iniDPB.TxBufferCnt = 2*1489;   // 1GBit, 1ms scheduler cycle (wirespeed with minimal packets)
    }
    else // 100 MBit
    {
        p_setup->iniDPB.RxBufferCnt = 2*149;    // 100MBit, 1ms scheduler cycle
        p_setup->iniDPB.TxBufferCnt = 2*149;    // 100MBit, 1ms scheduler cycle
    }
    #else
    p_setup->iniDPB.RxBufferCnt         = 32*2*149;                                 // 100MBit, 32ms scheduler cycle
    p_setup->iniDPB.TxBufferCnt         = 32*2*149;                                 // 100MBit, 32ms scheduler cycle
    #endif // PSI_EDDS_CFG_HW_INTEL or PSI_EDDS_CFG_HW_TI or PSI_EDDS_CFG_HW_IOT2000

    #if (defined(PSI_EDDS_CFG_HW_INTEL) || defined(PSI_EDDS_CFG_HW_TI) || defined(PSI_EDDS_CFG_HW_IOT2000)) // using INTEL or TI or IOT2000 LL
    p_setup->iniDPB.schedulerCycleIO	= (LSA_UINT16)1;                            // scheduler cycle = 1ms
    p_setup->iniDPB.schedulerCycleNRT	= (LSA_UINT16)100;                          // scheduler cycle = 100ms
    #else
    p_setup->iniDPB.schedulerCycleIO    = (LSA_UINT16)32;                           // scheduler cycle = 32ms
    p_setup->iniDPB.schedulerCycleNRT	= (LSA_UINT16)100;                          // scheduler cycle = 100ms
    #endif // PSI_EDDS_CFG_HW_INTEL or PSI_EDDS_CFG_HW_TI or PSI_EDDS_CFG_HW_IOT2000

    // Features supported
    if (PSI_USE_SETTING_YES == p_inp->stdmac.FeatureSupport.use_settings)
    {
        /* MRP interconnect */
        p_setup->iniDPB.FeatureSupport.MRPInterconnFwdRulesSupported   = (PSI_FEATURE_ENABLE == p_inp->stdmac.FeatureSupport.mrp_interconn_fwd_rules_supported) ?  EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.MRPInterconnOriginatorSupported = (PSI_FEATURE_ENABLE == p_inp->stdmac.FeatureSupport.mrp_interconn_originator_supported) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    }

    // interfaces and ports
    p_setup->iniDPB.MaxInterfaceCntOfAllEDD = p_hd->nr_of_all_if;    // Number of all interfaces of all EDDs.
    p_setup->iniDPB.MaxPortCntOfAllEDD      = p_hd->nr_of_all_ports; // Number of all ports of all EDDs.

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_prepare_setup_data(): MaxInterfaceCntOfAllEDD(%u), MaxPortCntOfAllEDD(%u)",
        p_setup->iniDPB.MaxInterfaceCntOfAllEDD, p_setup->iniDPB.MaxPortCntOfAllEDD);

	// Get mrp_ringport_type from user input and patch DPB default values
	for (port_idx = 0; port_idx < EDD_CFG_MAX_PORT_CNT; port_idx++)
	{
		// Note: User ports from [1..N], 0 is reserved for auto port ID
		p_entry = psi_get_hw_port_entry_for_user_port((port_idx+1), p_inp );
		if (p_entry != LSA_NULL)
		{
			p_setup->iniDPB.SWI.MRPRingPort[port_idx] = p_entry->mrp_ringport_type;
		}
		else
		{
			// use default settings from edds_InitDPBWithDefaults
		}
        // MRP interconnect
        p_setup->iniDPB.SWI.SupportsMRPInterconnPortConfig[port_idx] = p_inp->pnio_if.mrp.supports_mrp_interconn_port_config[port_idx];
	}

    // MRP settings
	if (PSI_USE_SETTING_YES == if_ptr->mrp.use_setting) // Usage of MRP inputs?
	{
		p_setup->iniDPB.SWI.MaxMRP_Instances            = if_ptr->mrp.max_instances;
		p_setup->iniDPB.SWI.MRPSupportedRole            = if_ptr->mrp.supported_role;
		p_setup->iniDPB.SWI.MRPSupportedMultipleRole    = if_ptr->mrp.supported_multiple_role;
		p_setup->iniDPB.SWI.MRPDefaultRoleInstance0     = if_ptr->mrp.default_role_instance0;
        // MRP interconnect
        p_setup->iniDPB.SWI.MaxMRPInterconn_Instances   = if_ptr->mrp.max_mrp_interconn_instances;
        p_setup->iniDPB.SWI.SupportedMRPInterconnRole   = if_ptr->mrp.supported_mrp_interconn_role;
    }
	// else: uses MRP values from convenience function

	///////////////////////////////////////////////////////////////////////////
	// setup DSB
    EddRspVal = edds_InitDSBWithDefaults(&p_setup->iniDSB);
    PSI_ASSERT(EddRspVal == EDD_STS_OK);

	// setup IF MAC
	PSI_MEMCPY( p_setup->iniDSB.MACAddress[0].MacAdr, hd_hw_ptr->if_mac, EDD_MAC_ADDR_SIZE );

	// setup PORT MAC
	for (port_idx = 0 ; port_idx < p_inp->nr_of_ports ; port_idx++)
	{
	    PSI_MEMCPY( p_setup->iniDSB.MACAddress[port_idx+1].MacAdr, hd_hw_ptr->port_mac[port_idx], EDD_MAC_ADDR_SIZE );
	}

	///////////////////////////////////////////////////////////////////////////
	// setup NRT subcomponent  

	// number of receive buffers for NRT Only - 1GBit/s -> we need more rx-buffers
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntARPIP       = 96; // for NRT frames: ICMP, IGMP TCP UDP IP_OTHER ARP RARP
   	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntOther       = 96; // for NRT frames: DCP, TSYNC, STDBY, HSR, SINEC_FWL, OTHER, LLDP, MRP, OTHER
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntASRTAlarm   = 96; // for NRT frames: ASRT, UDP_ASRT
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_NRT_MODE].RxBufferCntASRTOther   = 96; // for NRT frames: DCP, UDP_DCP, PTCP

	// number of receive buffers for CRT and NRT - will be used directly after startup
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntARPIP       = 48; // for NRT frames: ICMP, IGMP TCP UDP IP_OTHER ARP RARP
   	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntOther       = 48; // for NRT frames: DCP, TSYNC, STDBY, HSR, SINEC_FWL, OTHER, LLDP, MRP, OTHER
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntASRTAlarm   = 48; // for NRT frames: ASRT, UDP_ASRT
	p_setup->iniDPB.NRT.parameterSet[EDDS_DPB_PARAMETERSET_CRT_MODE].RxBufferCntASRTOther   = 48; // for NRT frames: DCP, UDP_DCP, PTCP

	p_setup->iniDPB.NRT.RxFilterUDP_Broadcast   = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Broadcast ) ?   EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;
	p_setup->iniDPB.NRT.RxFilterUDP_Unicast     = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Unicast  ) ?    EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;

	// ---- setup SRT subcomponent
	pDetailsIF = psi_get_detail_store((LSA_UINT16)p_inp->hd_id);

    if //Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32,64,128,256,512,1024 (=1ms,2ms,4ms,8ms,16ms,32ms Cylce-Time)
       (   (p_inp->send_clock_factor != (LSA_UINT16)32)
	    && (p_inp->send_clock_factor != (LSA_UINT16)64)
	    && (p_inp->send_clock_factor != (LSA_UINT16)128)
	    && (p_inp->send_clock_factor != (LSA_UINT16)256)
	    && (p_inp->send_clock_factor != (LSA_UINT16)512)
	    && (p_inp->send_clock_factor != (LSA_UINT16)1024))
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_edds_prepare_setup_data(): invalid send_clock_factor(%u)", p_inp->send_clock_factor );
		PSI_FATAL( 0 );
    }

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_edds_prepare_setup_data(): schedulerCycleIO(%u) schedulerCycleNRT(%u)",
		p_setup->iniDPB.schedulerCycleIO, p_setup->iniDPB.schedulerCycleNRT );

    p_setup->iniDPB.CSRT.CycleBaseFactor = p_inp->send_clock_factor;

    p_setup->iniDPB.CSRT.ConsumerCntClass1 = // All unicast/multicast class1
		pDetailsIF->acp.cm_cl.nr_of_consumers_class1_unicast + 
		pDetailsIF->acp.cm_sv.nr_of_consumers_class1_unicast +
		pDetailsIF->acp.cm_mc.nr_of_consumers_class1_unicast;

	p_setup->iniDPB.CSRT.ConsumerCntClass2 = // All unicast/multicast class2
		pDetailsIF->acp.cm_cl.nr_of_consumers_class2_unicast + 
		pDetailsIF->acp.cm_sv.nr_of_consumers_class2_unicast + 
		pDetailsIF->acp.cm_mc.nr_of_consumers_class2_unicast;

    // GBIT support enabled?
    if (p_setup->session.b_gigabit_support)
    {
        // application has to fill all cycle RT parameters with 0!
        // here an assertion check because PSI has to check it as ERR_PARAM in the central check-param-function
        // see corresponding checks of "gigabit_support" and "io_configured" in psi_ld_check_hd_open_params()
        PSI_ASSERT(p_inp->io_configured == PSI_FEATURE_DISABLE);
        PSI_ASSERT(p_setup->iniDPB.CSRT.ConsumerCntClass1 == 0);
        PSI_ASSERT(p_setup->iniDPB.CSRT.ConsumerCntClass2 == 0);
    }

    p_setup->iniDPB.CSRT.ProviderCnt = p_setup->iniDPB.CSRT.ConsumerCntClass1 + p_setup->iniDPB.CSRT.ConsumerCntClass2;

    p_setup->iniDPB.CSRT.ConsumerFrameIDBaseClass1 = 0xF800 - p_setup->iniDPB.CSRT.ConsumerCntClass1;
    p_setup->iniDPB.CSRT.ConsumerFrameIDBaseClass2 = 0xBC00 - p_setup->iniDPB.CSRT.ConsumerCntClass2;

    PSI_HD_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "psi_edds_prepare_setup_data(): CycleBaseFactor(%u) ConsumerCntClass1(%u) ConsumerCntClass2(%u) ProviderCnt(%u)",
		p_setup->iniDPB.CSRT.CycleBaseFactor, 
        p_setup->iniDPB.CSRT.ConsumerCntClass1, 
        p_setup->iniDPB.CSRT.ConsumerCntClass2, 
        p_setup->iniDPB.CSRT.ProviderCnt);

    p_setup->iniDPB.FeatureSupport.AdditionalForwardingRulesSupported = EDD_FEATURE_DISABLE;
	p_setup->iniDPB.FeatureSupport.ApplicationExist                   = (PSI_FEATURE_ENABLE == p_inp->application_exist)  ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    p_setup->iniDPB.FeatureSupport.ClusterIPSupport                   = (PSI_FEATURE_ENABLE == p_inp->cluster_IP_support) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
}

#endif //((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

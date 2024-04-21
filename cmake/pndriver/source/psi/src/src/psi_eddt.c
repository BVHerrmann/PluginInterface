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
/*  F i l e               &F: psi_eddt.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for EDDT                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   120
#define PSI_MODULE_ID       120 /* PSI_MODULE_ID_PSI_EDDT */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== EDDT ==================================================================*/
/*===========================================================================*/

static EDDT_CDB_TYPE * psi_init_CDB_EDDT(
	EDDT_CDB_TYPE     * const cdb,
	EDDT_CDB_NRT_TYPE * const pNRT,
    LSA_VOID_PTR_TYPE   const hDDB,
	LSA_UINT32          const filter )
{
	cdb->UsedComp.UseNRT    = LSA_FALSE;
	cdb->UsedComp.UseCRT    = LSA_FALSE;
	cdb->UsedComp.UsePRM    = LSA_FALSE;

    cdb->hDDB = hDDB;
    cdb->pNRT = LSA_NULL;
    cdb->pCRT = LSA_NULL;
	cdb->pPRM = LSA_NULL;

	// *--------------------------------------
	// *  NRT configuration
	// *--------------------------------------
	if (pNRT != LSA_NULL) 
	{
		pNRT->FrameFilter = filter;

		cdb->pNRT            = pNRT;
		cdb->UsedComp.UseNRT = LSA_TRUE;
	}

	return cdb;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_eddt_get_path_info(
	LSA_SYS_PTR_TYPE  * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   sys_path )
{
	EDDT_DETAIL_PTR_TYPE              pDetail;
	PSI_SYS_PTR_TYPE                  pSys;
	EDDT_UPPER_CDB_NRT_PTR_TYPE       pNRT;
	EDDT_UPPER_CDB_CRT_PTR_TYPE       pCRT;
	EDDT_UPPER_CDB_PRM_PTR_TYPE       pPRM;
    EDDT_HANDLE                       hDDB;
    LSA_USER_ID_TYPE                  user_id;
    LSA_UINT16                        mbx_id_rsp = PSI_MBX_ID_MAX;
	LSA_BOOL                          arp_state  = LSA_FALSE;
	LSA_UINT16                        result     = LSA_RET_OK;
	LSA_UINT16 				  	const hd_nr  	 = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 				  	const path   	 = PSI_SYSPATH_GET_PATH(sys_path);
	PSI_DETAIL_STORE_PTR_TYPE 	const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr    != LSA_NULL);
	PSI_ASSERT(detail_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDT);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)",
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    hDDB = psi_hd_get_hDDB(hd_nr);
    PSI_ASSERT(hDDB);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	switch (path)
	{
	    case PSI_PATH_IF_SYS_MRP_EDD:
		{
			LSA_UINT32 mrpFilter = EDD_NRT_FRAME_MRP;

            #if defined(MRP_CFG_PLUGIN_2_HSR)
			mrpFilter |= EDD_NRT_FRAME_LEN_HSR;   // for HSR
			mrpFilter |= EDD_NRT_FRAME_LEN_STDBY; // for STDBY
            #endif

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, mrpFilter);

			mbx_id_rsp = PSI_MBX_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ARP);

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
			arp_state  = LSA_TRUE;      // mark this channel for ARP check at response
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ICMP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_ICMP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			if (pDetailsIF->tcip.multicast_support_on) // MC support on ?
			{
				pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_IGMP);
			}
			else
			{
				pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP);
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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_TCP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP_HELLO /* | EDD_NRT_FRAME_UDP_DCP_HELLO */);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, 0); // NRT for sending ARPs

			mbx_id_rsp = PSI_MBX_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, 0);

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

	    case PSI_PATH_IF_SYS_OHA_DCP_EDD:  /* whichever comes first! */
	    case PSI_PATH_IF_SYS_NARE_DCP_EDD: /* whichever comes first! */
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP /* | EDD_NRT_FRAME_UDP_DCP */);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_PTCP_DELAY);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_ANNO);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LLDP);

			mbx_id_rsp = PSI_MBX_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ASRT /* | EDD_NRT_FRAME_UDP_ASRT */);

			mbx_id_rsp = PSI_MBX_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pPRM), user_id, sizeof(*pPRM), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pPRM != LSA_NULL);

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pCRT), user_id, sizeof(*pCRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pCRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, 0); // NRT for EDD_SRV_NRT_SET_DCP_HELLO_FILTER

			/* Note: Must match with OHA allow_non_pnio_mautypes setting */
			pPRM->PortDataAdjustLesserCheckQuality = pDetailsIF->oha.allow_non_pnio_mautypes;

			pDetail->pPRM            = pPRM;
			pDetail->UsedComp.UsePRM = LSA_TRUE;
            pDetail->UsedComp.UseCRT = LSA_TRUE;

			pCRT->Dummy   = 0;
			pDetail->pCRT = pCRT;

			mbx_id_rsp = PSI_MBX_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail = psi_init_CDB_EDDT(pDetail, LSA_NULL, hDDB, 0);

			mbx_id_rsp = PSI_MBX_ID_POF;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LEN_TSYNC);

            mbx_id_rsp = PSI_MBX_ID_PSI; // PSI EDD user hub
		}
		break;	

		case PSI_PATH_IF_SYS_HSA_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
            PSI_ASSERT(pNRT != LSA_NULL);

			/* If the input parameters p_hd_in->additional_forwarding_rules_supported and p_hd_in->application_exist are set, the value bHsyncAppl is true.
			   In this case, we register for the frame types EDD_NRT_FRAME_HSYNC. HSA will receive and send frames using this channel.
			   In the other case, HSA does not receive or send HSync frames, but HSA will use this channel to call EDD_SRV_GET_PARAMS.
			*/
			if (pDetailsIF->hsa.bHsyncAppl)
			{
				pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, EDD_NRT_FRAME_HSYNC); // NRT for sending Hsa Frames
			}
			else
			{
				pDetail = psi_init_CDB_EDDT(pDetail, pNRT, hDDB, 0 /* use invalid EDD_NRT_FRAME type - we don't want to receive any frame using this channel. 0 == no reception. */);
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

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_eddt_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
		}
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_EDDT;
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
LSA_RESULT psi_eddt_release_path_info(
	LSA_SYS_PTR_TYPE  sys_ptr,
	LSA_VOID_PTR_TYPE detail_ptr )
{
	LSA_UINT16                 rc;
	PSI_SYS_PTR_TYPE     const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	EDDT_DETAIL_PTR_TYPE       pDetail;

	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_EDDT);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_release_path_info()" );

    pDetail = (EDDT_DETAIL_PTR_TYPE)detail_ptr;

	if (pDetail->pNRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pNRT, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	if (pDetail->pCRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pCRT, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	if (pDetail->pPRM != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pPRM, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_EDDT, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/* EDDT system calls for device setup, shutdown                               */
/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_setup_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDT_SETUP_PTR_TYPE       p_setup;
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_setup = (PSI_EDDT_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_setup->session.req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_setup_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_setup->session.hd_nr, pRQB );

    psi_system_request_done(PSI_MBX_ID_PSI, p_setup->session.req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_close_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDT_SESSION_PTR_TYPE       p_session;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_session = (PSI_EDDT_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_close_request_done(): hd_nr(%u) pRQB(0x%08x)", p_session->hd_nr, pRQB );

	psi_system_request_done(PSI_MBX_ID_PSI, p_session->req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_device_open_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
    LSA_UINT16                         ret16_val;
    LSA_UINT32                         port_idx;
    LSA_UINT8                          MediaType, IsPOF, PhyType, FXTransceiverType;
    PSI_EDDT_SETUP_PTR_TYPE            p_setup;
    PSI_HD_STORE_PTR_TYPE              p_hd;
    EDDT_UPPER_DEV_OPEN_PTR_TYPE       pOpen;
    EDD_UPPER_RQB_PTR_TYPE       const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
    PSI_ASSERT(EDD_STS_OK        == EDD_RQB_GET_RESPONSE(pRQB));
    PSI_ASSERT(EDD_OPC_SYSTEM    == EDD_RQB_GET_OPCODE(pRQB));
    PSI_ASSERT(EDDT_SRV_DEV_OPEN == EDD_RQB_GET_SERVICE(pRQB));

    pOpen = (EDDT_UPPER_DEV_OPEN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    PSI_ASSERT(pOpen);

    // Read the session data (setup) for next request
    p_setup = (PSI_EDDT_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
    PSI_ASSERT(p_setup != LSA_NULL);

    // Register hDDB for this board
    *p_setup->session.hDDB_ptr = pOpen->hDDB;

    p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
    PSI_ASSERT(p_hd);
    PSI_ASSERT(p_hd->is_used);
    PSI_ASSERT(p_hd->nr_of_ports <= PSI_CFG_MAX_PORT_CNT);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_OPEN_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_open_done(): hd_nr(%u)", p_setup->session.hd_nr);

 	for (port_idx = 0; port_idx < p_hd->nr_of_ports; port_idx++)
	{
        LSA_UINT16             const HwPortId                   = p_hd->hd_hw.port_map[port_idx+1].hw_port_id;
        PSI_RQB_PTR_TYPE       const psi_hd_open_device_rqb_ptr = psi_hd_get_hd_open_device_rqb_ptr();
        PSI_HD_OUTPUT_PTR_TYPE const p_hd_out                   = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_out;
        PSI_HD_INPUT_PTR_TYPE  const p_hd_in                    = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_args;

        /*------------------------------------------*/
        /* get port parameters from system adaption */
        /*------------------------------------------*/
        PSI_EDD_GET_MEDIA_TYPE(
            p_hd->hd_sys_handle,
            HwPortId,
            PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_YES,
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
        if ((MediaType == PSI_MEDIA_TYPE_FIBER) && (IsPOF == 1))
        {
            p_hd_out->hw_port[port_idx].fiberoptic_type = PSI_FIBEROPTIC_POF;
        }

        /* PHY-Type */
        p_hd_out->hw_port[port_idx].phy_type = (PSI_PORT_PHY_TYPE)PhyType;

        /* MII-Type */
        p_hd_out->hw_port[port_idx].xmii_type = PSI_PORT_MII;

        /* EDD-Asic-Type */
        p_hd_out->hw_port[port_idx].edd_asic_type = (PSI_ASIC_TYPE)(p_hd_in->asic_type);

        /* is ring port? */
        p_hd_out->hw_port[port_idx].isRingPort = PSI_PORT_DEFAULT_RING_PORT;

        PSI_HD_TRACE_08(0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_open_done(): HD-Outputs: HwPortId(%u) media_type(0x%x) media_type_psubmodidtnr(0x%x) fiberoptic_type(0x%x) phy_type(0x%x) xmii_type(0x%x) edd_asic_type(0x%x) isRingPort(%u)",
            HwPortId,
            p_hd_out->hw_port[port_idx].media_type,
            p_hd_out->hw_port[port_idx].media_type_psubmodidtnr,
            p_hd_out->hw_port[port_idx].fiberoptic_type,
            p_hd_out->hw_port[port_idx].phy_type,
            p_hd_out->hw_port[port_idx].xmii_type,
            p_hd_out->hw_port[port_idx].edd_asic_type,
            p_hd_out->hw_port[port_idx].isRingPort);
	}

    EDDT_FREE_UPPER_MEM_LOCAL(&ret16_val, pRQB->pParam);
    PSI_ASSERT(ret16_val == EDD_STS_OK);

    EDDT_FREE_UPPER_RQB_LOCAL(&ret16_val, pRQB);
    PSI_ASSERT(ret16_val == EDD_STS_OK);

    // Enable the PNIO IRs
    PSI_HD_ENABLE_EVENT(p_setup->session.hd_sys_handle);

    // Continue with setup device
    psi_eddt_device_setup(p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_device_open(
	PSI_HD_EDDT_PTR_TYPE    p_hw_eddt,
	PSI_EDDT_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE       pRQB;
	EDDT_UPPER_DEV_OPEN_PTR_TYPE pOpen;
	PSI_HD_STORE_PTR_TYPE        p_hd;

	PSI_ASSERT(p_setup);
	PSI_ASSERT(p_hw_eddt);

	EDDT_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDT_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDT_RQB_DEV_OPEN_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDT);
	EDD_RQB_SET_SERVICE(pRQB, EDDT_SRV_DEV_OPEN);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_OPEN_START;

    // Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddt_device_open_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

	pOpen       = (EDDT_UPPER_DEV_OPEN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pOpen->pDPB = &(p_setup->iniDPB);
	pOpen->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddt_setup_request_done;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_open(): hd_nr(%u)", p_setup->session.hd_nr );

    psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_device_setup_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16            	      ret16_val;
	PSI_EDDT_SETUP_PTR_TYPE       p_setup;
	PSI_HD_STORE_PTR_TYPE  	      p_hd;
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK         == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDT_SRV_DEV_SETUP == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDT_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_SETUP_DONE;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_setup_done(): hd_nr(%u)", p_setup->session.hd_nr );

    EDDT_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDT_FREE_UPPER_RQB_LOCAL( &ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

    // Device open sequence finished --> notify done
    psi_hd_edd_open_done(p_setup->session.hd_nr, p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_device_setup(
    PSI_EDDT_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDT_UPPER_DEV_SETUP_PTR_TYPE p_dev_ptr;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(p_setup);

	EDDT_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDT_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDT_RQB_DEV_SETUP_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDT);
	EDD_RQB_SET_SERVICE(pRQB, EDDT_SRV_DEV_SETUP);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_SETUP_START;

    // Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddt_device_setup_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

	p_dev_ptr       = (EDDT_UPPER_DEV_SETUP_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	p_dev_ptr->hDDB = *p_setup->session.hDDB_ptr;
	p_dev_ptr->pDSB = &p_setup->iniDSB;
	p_dev_ptr->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddt_setup_request_done;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_setup(): hd_nr(%u)", p_setup->session.hd_nr );

    psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_device_shutdown_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      ret16_val;
	PSI_EDDT_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK            == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM        == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDT_SRV_DEV_SHUTDOWN == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data for next request
	p_session = (PSI_EDDT_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_SHUTDOWN_DONE;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_shutdown_done(): hd_nr(%u)", p_session->hd_nr );

    EDDT_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDT_FREE_UPPER_RQB_LOCAL( &ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	/* Disable the IR */
	PSI_ASSERT(p_session->hd_sys_handle != LSA_NULL);
	PSI_HD_DISABLE_EVENT(p_session->hd_sys_handle);

	// Continue with close device
	psi_eddt_device_close(p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_device_shutdown(
    PSI_EDDT_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE           pRQB;
	EDDT_UPPER_DEV_SHUTDOWN_PTR_TYPE pShutdown;
	PSI_HD_STORE_PTR_TYPE            p_hd;

	PSI_ASSERT(p_session != LSA_NULL);

	EDDT_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDT_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDT_RQB_DEV_SHUTDOWN_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDT);
	EDD_RQB_SET_SERVICE(pRQB, EDDT_SRV_DEV_SHUTDOWN);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_SHUTDOWN_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_eddt_device_shutdown_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pShutdown       = (EDDT_UPPER_DEV_SHUTDOWN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pShutdown->hDDB = *p_session->hDDB_ptr;
	pShutdown->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddt_close_request_done;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_shutdown(): hd_nr(%u)", p_session->hd_nr );

    psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddt_device_close_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      ret16_val;
	PSI_EDDT_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK         == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDT_SRV_DEV_CLOSE == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data for next request
	p_session = (PSI_EDDT_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

	*p_session->hDDB_ptr = 0;

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_CLOSE_DONE;

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_close_done(): hd_nr(%u)", p_session->hd_nr);

    EDDT_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDT_FREE_UPPER_RQB_LOCAL( &ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	// EDDT device shutdown sequence complete --> notify PSI HD
	psi_hd_edd_close_done(p_session->hd_nr, p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_device_close(
    PSI_EDDT_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDT_UPPER_DEV_CLOSE_PTR_TYPE pClose;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(p_session != LSA_NULL);

	EDDT_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET(pRQB, 0, sizeof(EDD_RQB_TYPE));

	EDDT_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDT_RQB_DEV_CLOSE_TYPE));
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDT);
	EDD_RQB_SET_SERVICE(pRQB, EDDT_SRV_DEV_CLOSE);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddt_state = PSI_HD_EDDT_STATE_DEV_CLOSE_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_eddt_device_close_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_session);

	pClose       = (EDDT_UPPER_DEV_CLOSE_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pClose->hDDB = *p_session->hDDB_ptr;
	pClose->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddt_close_request_done;

	PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_eddt_device_close(): hd_nr(%u)", p_session->hd_nr);

    psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_system, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_prepare_setup_data(
	PSI_EDDT_SETUP_PTR_TYPE p_setup,
	PSI_HD_INPUT_PTR_TYPE   p_inp,
	PSI_HD_STORE_PTR_TYPE   p_hd )
{
	PSI_HD_EDDT_PTR_TYPE    edd_ptr;
	PSI_HD_HW_PTR_TYPE      hw_ptr;
	LSA_UINT16              port_idx;
	EDD_RSP                 EddRspVal;

	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_inp   != LSA_NULL);
	PSI_ASSERT(p_hd    != LSA_NULL);

    // Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32 (=1ms Cylce-Time)
    if (p_inp->send_clock_factor != (LSA_UINT16)32)
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddt_prepare_setup_data(): invalid params, startup send_clock_factor(%u)", p_inp->send_clock_factor );
		PSI_FATAL(0);
    }

	hw_ptr  = &p_hd->hd_hw;
	edd_ptr = &p_hd->hd_hw.edd.eddt;

	// ----------------- Init Global IniBlock  -----------------
    p_setup->iniGlobal.GenIni.LifeTimeConsRequestedNs   = 31250UL;

    p_setup->iniGlobal.IOCIni.NrOfRTDevices             = 256;      // Nr of Class1/2-Devices                                                   256
    p_setup->iniGlobal.IOCIni.NrOfIRTDevices            = 64;       // Nr of Class3-Devices (including redundant devices!)                      64
    p_setup->iniGlobal.IOCIni.NrOfRedIRTDevices         = 32;       // Nr of redundant Class3-Devices                                           NrOfIRTDevices / 2
    p_setup->iniGlobal.IOCIni.NrOfQVProviders           = 1;        // Nr of MC-ControllerControllerCommunication (MC-CCC) providers (Class3)   1
    p_setup->iniGlobal.IOCIni.NrOfQVConsumers           = 16;       // Nr of MC-CCC consumers (Class3, including redundant MC-CCC consumers!)   16
    p_setup->iniGlobal.IOCIni.NrOfRedQVConsumers        = 8;        // Nr of redundant MC-CCC consumers (Class3)                                NrOfQVConsumers / 2
    p_setup->iniGlobal.IOCIni.LogicalAddressSpace       = 8192UL;   // logical addressspace                                                     8192
    p_setup->iniGlobal.IOCIni.NrOfSubmod                = 8192UL;   // Nr of submodules (1 IOPS + 1 IOCS for each submodul!)                    8192
    p_setup->iniGlobal.IOCIni.MaxIOCRDataSize           = 1440;     // Max. framelength UC-frame excluding APDUStatus                           1440
    p_setup->iniGlobal.IOCIni.MaxMCRDataSize            = 256;      // Max. framelength MC-frame excluding APDUStatus                           256

    p_setup->iniGlobal.IODIni.NrOfInstances             = 1;        // Nr. of instances                                                         1
    p_setup->iniGlobal.IODIni.NrOfARs                   = 2;        // Nr. of Ars                                                               2
    p_setup->iniGlobal.IODIni.MaxOCRDataSize            = 720;      // Max. framelength UC-frame output (Consumer)                              720
    p_setup->iniGlobal.IODIni.MaxICRDataSize            = 720;      // Max. framelength UC-frame input (Provider) excluding APDUStatus          720
    p_setup->iniGlobal.IODIni.MaxMCRDataSize            = 256;      // Max. framelength MC-frame excluding APDUStatus                           256
    p_setup->iniGlobal.IODIni.AlternativeCalc           = LSA_FALSE;// If this is LSA_TRUE then the calculation is change.                      LSA_FALSE
                                                                    // MaxOCRDataSize, MaxICRDataSize and
                                                                    // MaxMCRDataSize are not longer as input values, but the 4 next below input values.
    p_setup->iniGlobal.IODIni.NrOfSubmodules            = 0;        // Nr of submodules                                                         0
    p_setup->iniGlobal.IODIni.OutputBytes               = 0;        // Output in bytes                                                          0
    p_setup->iniGlobal.IODIni.InputBytes                = 0;        // Input in bytes                                                           0
    p_setup->iniGlobal.IODIni.SharedARSupported         = LSA_FALSE;// Shared AR Supported or not                                               LSA_FALSE

    p_setup->iniGlobal.NRTIni.Dummy                     = 0;

    p_setup->iniGlobal.PNUNIT_DATARamUsed               = 0;        // OUT
    p_setup->iniGlobal.IO_DataRamUsed_EDDT_RX           = 0;        // OUT
    p_setup->iniGlobal.IO_DataRamUsed_EDDT_TX           = 0;        // OUT
    p_setup->iniGlobal.IO_DataRamUsed_IO_RX             = 0;        // OUT
    p_setup->iniGlobal.IO_DataRamUsed_IO_TX             = 0;        // OUT
    p_setup->iniGlobal.pInternalUse                     = 0;
    p_setup->iniGlobal.InterfaceID                      = 1UL;

	// ----------------- Init DPB data  -----------------
    // Use the default settings as base
    EddRspVal = eddt_InitDPBWithDefaults(&p_setup->iniDPB, &p_setup->iniGlobal);
	PSI_ASSERT(EddRspVal == EDD_STS_OK);

	// Prepare the data for device specific handle
	p_setup->iniDPB.hSysDev     = p_hd->hd_sys_handle;
    p_setup->iniDPB.InterfaceID = p_inp->pnio_if.edd_if_id;
    p_setup->iniDPB.TraceIdx    = p_inp->pnio_if.trace_idx;

	// ports and interfaces
	p_setup->iniDPB.MaxPortCntOfAllEDD       = p_hd->nr_of_all_ports;           // Number of all ports of all EDDs
	p_setup->iniDPB.MaxInterfaceCntOfAllEDD  = p_hd->nr_of_all_if;              // Number of all interfaces of all EDDs

    // memory ranges
    p_setup->iniDPB.MEM.pPNUNITDataBase      = edd_ptr->pnunit_mem.base_ptr;
    p_setup->iniDPB.MEM.PNUNITDataSegmentLen = edd_ptr->pnunit_mem.size;
    p_setup->iniDPB.MEM.pIODataBase          = edd_ptr->sdram_NRT.base_ptr;
    p_setup->iniDPB.MEM.IODataSegmentLen     = edd_ptr->sdram_NRT.size;

    // PNUNIT SHM
    {
	    LSA_UINT32 align    = 0;
	    LSA_UINT32 ptoeSize = 0;
	    LSA_UINT32 etopSize = 0;

	    // calculate shared memory structures (command interface)
	    EddRspVal = eddt_GetSizeOfInternalStructs(EDDT_ENUM_TYPE_PCB_PTOE, &ptoeSize, &align);
        PSI_ASSERT(EddRspVal == EDD_STS_OK);
        PSI_ASSERT(align == 8UL);
        EddRspVal = eddt_GetSizeOfInternalStructs(EDDT_ENUM_TYPE_PCB_ETOP, &etopSize, &align);
        PSI_ASSERT(EddRspVal == EDD_STS_OK);
        PSI_ASSERT(align == 8UL);

        // force 8 byte alignment
        if (ptoeSize & 7UL)
        {
            ptoeSize = (LSA_UINT32)(ptoeSize & (~7UL)) + 8UL;
        }
        if (etopSize & 7UL)
        {
            etopSize = (LSA_UINT32)(etopSize & (~7UL)) + 8UL;
        }
        PSI_ASSERT((ptoeSize + etopSize) <= edd_ptr->pnunit_mem.size);

        p_setup->iniDPB.MEM.PCB_ETOPOffset   = 0;                               // EtoP: start of shared mem
        p_setup->iniDPB.MEM.PCB_PTOEOffset   = etopSize;                        // PtoE: starts after EtoP
    }

    p_setup->iniDPB.MEM.PTCPInfoOffset       = EDD_DATAOFFSET_INVALID;
    p_setup->iniDPB.MEM.PNTraceBufferOffset  = EDD_DATAOFFSET_INVALID;          // Offset of PNTrace structure for PNU in shared IO_DATA segment
    p_setup->iniDPB.MEM.PNTraceBufferSize    = 0;                               // Size of PNTrace structure for PNU in shared IO_DATA segment

	// ----------------- Init DSB data  -----------------
	// Prepare DSB data for PN board
	// Note: using EDDT default settings (see eddt_InitDSBWithDefaults() in eddt_core_sys.c)
	EddRspVal = eddt_InitDSBWithDefaults(&p_setup->iniDPB, &p_setup->iniDSB);
	PSI_ASSERT(EddRspVal == EDD_STS_OK);

    // setup IF MAC
	PSI_MEMCPY(p_setup->iniDSB.IF_MAC_Address.MacAdr, p_hd->hd_hw.if_mac, EDD_MAC_ADDR_SIZE);

	for (port_idx = 0; port_idx < p_inp->nr_of_ports; port_idx++)
	{
		// setup port MAC addresses
		PSI_MEMCPY(p_setup->iniDSB.PortParams[port_idx].Port_MAC_Address.MacAdr, p_hd->hd_hw.port_mac[port_idx], EDD_MAC_ADDR_SIZE);

		// note: MDIX setting is set to on per default
		// overwrite this setting to off is possible after POF port detection, not here
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddt_phy_get_media_type(
    EDDT_HANDLE       hDDB,
    EDDT_SYS_HANDLE   hSysDev,
    LSA_UINT32        HwPortID,
    LSA_UINT8       * pMediaType,
    LSA_UINT8       * pIsPOF )
{
    LSA_UINT8                  PhyType, FXTransceiverType;
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;

    PSI_ASSERT(pSys);
    PSI_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDT);
    PSI_ASSERT((HwPortID >= 1) && (HwPortID <= EDD_CFG_MAX_PORT_CNT));

    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(PhyType);

    // Get the status from the system adaptation
    if (PSI_EDD_IS_PORT_PULLED(pSys, HwPortID))
    {
        // default value for pulled port has to be set to EDD_MEDIATYPE_UNKNOWN
        *pMediaType = EDD_MEDIATYPE_UNKNOWN; 
        *pIsPOF     = 0;
    }
    else
    {
        PSI_EDD_GET_MEDIA_TYPE(
            hSysDev,
            HwPortID,
            PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO,
            pMediaType,
            pIsPOF,
            &PhyType,
            &FXTransceiverType); /* not used here */
    }
}

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI EDDT support (PSI_CFG_USE_EDDT=" PSI_STRINGIFY(PSI_CFG_USE_EDDT) ")")
#endif
#endif // PSI_CFG_USE_EDDT

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

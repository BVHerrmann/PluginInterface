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
/*  F i l e               &F: psi_eddp.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for EDDP                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   106
#define PSI_MODULE_ID       106 /* PSI_MODULE_ID_PSI_EDDP */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== EDDP ==================================================================*/
/*===========================================================================*/

static EDDP_CDB_TYPE * psi_init_CDB_EDDP(
	EDDP_CDB_TYPE     * const cdb,
	EDDP_CDB_NRT_TYPE * const pNRT,
    LSA_VOID_PTR_TYPE   const hDDB,
	LSA_UINT32          const filter )
{
	cdb->UsedComp.UseCRT    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseK32    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseNRT    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UsePRM    = EDD_CDB_CHANNEL_USE_OFF;

    cdb->hDDB = hDDB;
	cdb->pCRT = LSA_NULL;
	cdb->pNRT = LSA_NULL;
	cdb->pPRM = LSA_NULL;

	// *--------------------------------------
	// *  NRT configuration
	// *--------------------------------------
	if (pNRT != LSA_NULL)
	{
		pNRT->FrameFilter    = filter;
		cdb->pNRT            = pNRT;
		cdb->UsedComp.UseNRT = EDD_CDB_CHANNEL_USE_ON;
	}

	return cdb;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_eddp_get_path_info(
	LSA_SYS_PTR_TYPE  * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   sys_path )
{
	EDDP_DETAIL_PTR_TYPE              pDetail;
	PSI_SYS_PTR_TYPE                  pSys;
	EDDP_UPPER_CDB_NRT_PTR_TYPE       pNRT;
	EDDP_UPPER_CDB_CRT_PTR_TYPE       pCRT;
	EDDP_UPPER_CDB_PRM_PTR_TYPE       pPRM;
    EDDP_HANDLE                       hDDB;
    LSA_USER_ID_TYPE                  user_id;
    LSA_UINT16                        mbx_id_rsp = PSI_MBX_ID_MAX;
	LSA_BOOL                          arp_state  = LSA_FALSE;
	LSA_UINT16                        result     = LSA_RET_OK;
	LSA_UINT16 				    const hd_nr  	 = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 				    const path       = PSI_SYSPATH_GET_PATH(sys_path);
	PSI_DETAIL_STORE_PTR_TYPE 	const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr    != LSA_NULL);
    PSI_ASSERT(detail_ptr_ptr != LSA_NULL);

	PSI_ASSERT(pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDP);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)",
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

    hDDB = psi_hd_get_hDDB(hd_nr);
    PSI_ASSERT(hDDB);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
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

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, mrpFilter);

			mbx_id_rsp = PSI_MBX_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ARP);

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
			arp_state  = LSA_TRUE; // mark this channel for ARP check at response
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ICMP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_ICMP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			if (pDetailsIF->tcip.multicast_support_on) // MC support on ?
			{
				pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_IGMP);
			}
			else
			{
				pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP);
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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_TCP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP_HELLO /* | EDD_NRT_FRAME_UDP_DCP_HELLO */);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, 0); // NRT for sending ARPs

			mbx_id_rsp = PSI_MBX_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, 0);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP /* | EDD_NRT_FRAME_UDP_DCP */);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, (EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_PTCP_DELAY));
            // PTCP Sync and Delay frames are handled by KRISC
			pDetail->UsedComp.UseK32 = EDD_CDB_CHANNEL_USE_ON; // Use K32Risc IF

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_ANNO);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LLDP);

			mbx_id_rsp = PSI_MBX_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ASRT /* | EDD_NRT_FRAME_UDP_ASRT */);

			mbx_id_rsp = PSI_MBX_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pPRM), user_id, sizeof(*pPRM), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pPRM != LSA_NULL);

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pCRT), user_id, sizeof(*pCRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pCRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, 0); // NRT for EDD_SRV_NRT_SET_DCP_HELLO_FILTER

			/* Note: Must match with OHA allow_non_pnio_mautypes setting */
			pPRM->PortDataAdjustLesserCheckQuality = (pDetailsIF->oha.allow_non_pnio_mautypes) ? EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY : EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD;

			pDetail->pPRM            = pPRM;
			pDetail->UsedComp.UsePRM = EDD_CDB_CHANNEL_USE_ON;
            pDetail->UsedComp.UseCRT = EDD_CDB_CHANNEL_USE_ON;

			pCRT->Dummy   = 0;
			pDetail->pCRT = pCRT;

			mbx_id_rsp = PSI_MBX_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail = psi_init_CDB_EDDP(pDetail, LSA_NULL, hDDB, 0);

			mbx_id_rsp = PSI_MBX_ID_POF;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LEN_TSYNC);

            mbx_id_rsp = PSI_MBX_ID_PSI; // PSI EDD user hub
		}
		break;

		case PSI_PATH_IF_SYS_HSA_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
            PSI_ASSERT(pNRT != LSA_NULL);

			/* If the input parameters p_hd_in->additional_forwarding_rules_supported and p_hd_in->application_exist are set, the value bHsyncAppl is true.
			   In this case, we register for the frame types EDD_NRT_FRAME_HSYNC. HSA will receive and send frames using this channel.
			   In the other case, HSA does not receive or send HSync frames, but HSA will use this channel to call EDD_SRV_GET_PARAMS.
			*/
			if (pDetailsIF->hsa.bHsyncAppl)
			{
				pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, EDD_NRT_FRAME_HSYNC); // NRT for sending HSA Frames
			}
			else
			{
				pDetail = psi_init_CDB_EDDP(pDetail, pNRT, hDDB, 0 /* use invalid EDD_NRT_FRAME type - we don't want to receive any frame using this channel. 0 == no reception. */);
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

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_eddp_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_EDDP;
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
LSA_RESULT psi_eddp_release_path_info(
	LSA_SYS_PTR_TYPE  sys_ptr,
	LSA_VOID_PTR_TYPE detail_ptr )
{
	LSA_UINT16                 rc;
	PSI_SYS_PTR_TYPE     const pSys    = (PSI_SYS_PTR_TYPE)sys_ptr;
	EDDP_DETAIL_PTR_TYPE const pDetail = (EDDP_DETAIL_PTR_TYPE)detail_ptr;

	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_EDDP);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_release_path_info()" );

	if (pDetail->pNRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pNRT, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	if (pDetail->pCRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pCRT, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	if (pDetail->pPRM != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pPRM, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT( rc == LSA_RET_OK );

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_EDDP, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/* EDDP system calls for device setup, shutdown                               */
/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_setup_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDP_SETUP_PTR_TYPE       p_setup;
    EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_setup = (PSI_EDDP_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_setup->session.req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_setup_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_setup->session.hd_nr, pRQB );

    psi_system_request_done(PSI_MBX_ID_PSI, p_setup->session.req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_close_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDP_SESSION_PTR_TYPE p_session;

	EDD_UPPER_RQB_PTR_TYPE const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
    PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_session = (PSI_EDDP_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_close_request_done(): hd_nr(%u) pRQB(0x%08x)", p_session->hd_nr, pRQB );

	psi_system_request_done(PSI_MBX_ID_PSI, p_session->req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_device_open_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
    LSA_UINT16                         ret16_val;
    LSA_UINT32                         port_idx;
    LSA_UINT8                          MediaType, IsPOF, PhyType, FXTransceiverType;
    PSI_EDDP_SETUP_PTR_TYPE            p_setup;
    PSI_HD_STORE_PTR_TYPE              p_hd;
    EDDP_UPPER_DEV_OPEN_PTR_TYPE       pOpen;
    EDD_UPPER_RQB_PTR_TYPE       const pRQB                       = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
    PSI_RQB_PTR_TYPE             const psi_hd_open_device_rqb_ptr = psi_hd_get_hd_open_device_rqb_ptr();
    PSI_HD_OUTPUT_PTR_TYPE       const p_hd_out                   = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_out;
    PSI_HD_INPUT_PTR_TYPE        const p_hd_in                    = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_args;

    PSI_ASSERT(pRQB != LSA_NULL);
    PSI_ASSERT(EDD_STS_OK        == EDD_RQB_GET_RESPONSE(pRQB));
    PSI_ASSERT(EDD_OPC_SYSTEM    == EDD_RQB_GET_OPCODE(pRQB));
    PSI_ASSERT(EDDP_SRV_DEV_OPEN == EDD_RQB_GET_SERVICE(pRQB));

    pOpen = (EDDP_UPPER_DEV_OPEN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    PSI_ASSERT(pOpen);

    // Read the session data (setup) for next request
    p_setup = (PSI_EDDP_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
    PSI_ASSERT(p_setup != LSA_NULL);

    // Register hDDB for this board
    *p_setup->session.hDDB_ptr = pOpen->hDDB;

    p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
    PSI_ASSERT(p_hd);
    PSI_ASSERT(p_hd->is_used);
    PSI_ASSERT(p_hd->nr_of_ports <= PSI_CFG_MAX_PORT_CNT);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_OPEN_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_open_done(): hd_nr(%u)", p_setup->session.hd_nr);

 	for (port_idx = 0; port_idx < p_hd->nr_of_ports; port_idx++)
	{
        LSA_UINT16 const HwPortId = p_hd->hd_hw.port_map[port_idx+1].hw_port_id;

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
        if ((MediaType == PSI_MEDIA_TYPE_FIBER) && (IsPOF == EDD_PORT_OPTICALTYPE_ISPOF))
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

        PSI_HD_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_open_done(): HD-Outputs: HwPortId(%u) media_type(0x%x) media_type_psubmodidtnr(0x%x) fiberoptic_type(0x%x) phy_type(0x%x) xmii_type(0x%x) edd_asic_type(0x%x) isRingPort(%u)",
            HwPortId,
            p_hd_out->hw_port[port_idx].media_type,
            p_hd_out->hw_port[port_idx].media_type_psubmodidtnr,
            p_hd_out->hw_port[port_idx].fiberoptic_type,
            p_hd_out->hw_port[port_idx].phy_type,
            p_hd_out->hw_port[port_idx].xmii_type,
            p_hd_out->hw_port[port_idx].edd_asic_type,
            p_hd_out->hw_port[port_idx].isRingPort );
    }

    EDDP_FREE_UPPER_MEM_LOCAL(&ret16_val, pRQB->pParam);
    PSI_ASSERT(ret16_val == EDD_STS_OK);

    EDDP_FREE_UPPER_RQB_LOCAL(&ret16_val, pRQB);
    PSI_ASSERT(ret16_val == EDD_STS_OK);

    // Enable the PNIO IRs
    PSI_HD_ENABLE_EVENT(p_setup->session.hd_sys_handle);

    // Continue with setup device
    psi_eddp_device_setup(p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_device_open(
	PSI_HD_EDDP_CONST_PTR_TYPE const p_hw_eddp,
	PSI_EDDP_SETUP_PTR_TYPE    const p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE       pRQB;
	EDDP_UPPER_DEV_OPEN_PTR_TYPE pOpen;
	PSI_HD_STORE_PTR_TYPE        p_hd;

	PSI_ASSERT(p_hw_eddp);
	PSI_ASSERT(p_setup);

	EDDP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDP_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDP_RQB_DEV_OPEN_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDP);

	EDD_RQB_SET_SERVICE(pRQB, EDDP_SRV_DEV_OPEN);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_OPEN_START;

    // Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddp_device_open_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

	pOpen       = (EDDP_UPPER_DEV_OPEN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pOpen->pDPB = &p_setup->iniDPB;
	pOpen->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddp_setup_request_done;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_open(): hd_nr(%u)", p_setup->session.hd_nr );

    psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_device_setup_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16            	      ret16_val;
	PSI_EDDP_SETUP_PTR_TYPE	      p_setup;
	PSI_HD_STORE_PTR_TYPE  	      p_hd;
	EDD_UPPER_RQB_PTR_TYPE 	const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK         == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDP_SRV_DEV_SETUP == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDP_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_SETUP_DONE;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_setup_done(): hd_nr(%u)", p_setup->session.hd_nr );

    EDDP_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDP_FREE_UPPER_RQB_LOCAL(&ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

    // Device open sequence finished --> notify done
    psi_hd_edd_open_done(p_setup->session.hd_nr, p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_device_setup(
    PSI_EDDP_SETUP_PTR_TYPE const p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDP_UPPER_DEV_SETUP_PTR_TYPE p_dev_ptr;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(p_setup);

	EDDP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDP_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDP_RQB_DEV_SETUP_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDP);

	EDD_RQB_SET_SERVICE(pRQB, EDDP_SRV_DEV_SETUP);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_SETUP_START;

    // Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddp_device_setup_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_setup);

	p_dev_ptr       = (EDDP_UPPER_DEV_SETUP_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	p_dev_ptr->hDDB = *p_setup->session.hDDB_ptr;
	p_dev_ptr->pDSB = &p_setup->iniDSB;
	p_dev_ptr->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddp_setup_request_done;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_setup(): hd_nr(%u)", p_setup->session.hd_nr );

    psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_device_shutdown_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      ret16_val;
	PSI_EDDP_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK            == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM        == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDP_SRV_DEV_SHUTDOWN == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data for next request
	p_session = (PSI_EDDP_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_SHUTDOWN_DONE;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_shutdown_done(): hd_nr(%u)", p_session->hd_nr );

    EDDP_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDP_FREE_UPPER_RQB_LOCAL( &ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	/* Disable the IR */
	PSI_ASSERT(p_session->hd_sys_handle != LSA_NULL);
	PSI_HD_DISABLE_EVENT(p_session->hd_sys_handle);

	// Continue with close device
	psi_eddp_device_close(p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_device_shutdown(
    PSI_EDDP_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE              pRQB;
	EDDP_UPPER_DEV_SHUTDOWN_PTR_TYPE    pShutdown;
	PSI_HD_STORE_PTR_TYPE               p_hd;

	PSI_ASSERT(p_session != LSA_NULL);

	EDDP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDP_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDP_RQB_DEV_SHUTDOWN_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDP);

	EDD_RQB_SET_SERVICE(pRQB, EDDP_SRV_DEV_SHUTDOWN);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_SHUTDOWN_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_eddp_device_shutdown_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_session );

	pShutdown       = (EDDP_UPPER_DEV_SHUTDOWN_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pShutdown->hDDB = *p_session->hDDB_ptr;
	pShutdown->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddp_close_request_done;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_shutdown(): hd_nr(%u)", p_session->hd_nr );

    psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddp_device_close_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      ret16_val;
	PSI_EDDP_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

    PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK         == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDP_SRV_DEV_CLOSE == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data for next request
	p_session = (PSI_EDDP_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

	*p_session->hDDB_ptr = 0;

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_CLOSE_DONE;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_close_done(): hd_nr(%u)", p_session->hd_nr );

    EDDP_FREE_UPPER_MEM_LOCAL( &ret16_val, pRQB->pParam );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	EDDP_FREE_UPPER_RQB_LOCAL( &ret16_val, pRQB );
	PSI_ASSERT(ret16_val == EDD_STS_OK);

	// EDDP device shutdown sequence complete --> notify PSI HD
	psi_hd_edd_close_done(p_session->hd_nr, p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_device_close(
    PSI_EDDP_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDP_UPPER_DEV_CLOSE_PTR_TYPE pClose;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(p_session != LSA_NULL);

	EDDP_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
    PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDP_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDP_RQB_DEV_CLOSE_TYPE) );
	PSI_ASSERT(pRQB->pParam);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDP);

	EDD_RQB_SET_SERVICE(pRQB, EDDP_SRV_DEV_CLOSE);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddp_state = PSI_HD_EDDP_STATE_DEV_CLOSE_START;

    // Register session data for next request in done
	p_session->req_done_func = psi_eddp_device_close_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_session);

	pClose       = (EDDP_UPPER_DEV_CLOSE_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pClose->hDDB = *p_session->hDDB_ptr;
	pClose->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddp_close_request_done;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_device_close(): hd_nr(%u)", p_session->hd_nr );

    psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_system, pRQB);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_prepare_setup_data(
	PSI_EDDP_SETUP_PTR_TYPE p_setup,
	PSI_HD_INPUT_PTR_TYPE   p_inp,
	PSI_HD_STORE_PTR_TYPE   p_hd )
{
	PSI_HD_EDDP_PTR_TYPE       edd_ptr;
	PSI_HD_HW_PTR_TYPE         hw_ptr;
	PSI_HD_PORT_INPUT_PTR_TYPE p_entry;
	LSA_UINT16                 port_idx;
	EDD_RSP                    EddRspVal;

	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_inp   != LSA_NULL);
	PSI_ASSERT(p_hd    != LSA_NULL);
    PSI_ASSERT(p_hd->is_used);

    // Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32 (=1ms Cylce-Time)
    if (p_inp->send_clock_factor != (LSA_UINT16)32)
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddp_prepare_setup_data(): invalid params, startup send_clock_factor(%u)", p_inp->send_clock_factor );
		PSI_FATAL(0);
    }

	// ----------------- Init DPB data  ----------------- 
	hw_ptr  = &(p_hd->hd_hw);
	edd_ptr = &(p_hd->hd_hw.edd.eddp);

	// ----------------- Init DPB data  -----------------
    // Use the default settings as base
	EddRspVal = eddp_InitDPBWithDefaults(
		&(p_setup->iniDPB),
		edd_ptr->hw_type,           /* EDDP_HW_ERTEC200P, EDDP_HW_ERTEC200P_FPGA, ... */
		edd_ptr->icu_location,      /* EDDP_LOCATION_LOCAL, EDDP_LOCATION_EXT_HOST, ... */
        edd_ptr->hw_interface);     /* EDDP_HW_IF_A, EDDP_HW_IF_B */
	PSI_ASSERT(EddRspVal == EDD_STS_OK);

	// Prepare the data for device specific handle
	p_setup->iniDPB.hSysDev             = p_hd->hd_sys_handle;

	p_setup->iniDPB.pPnipBase           = edd_ptr->pnip.base_ptr;
	p_setup->iniDPB.pK32TCMBase         = edd_ptr->k32_tcm.base_ptr;
	p_setup->iniDPB.pK32ATCMBase        = edd_ptr->k32_Atcm.base_ptr;
	p_setup->iniDPB.pK32BTCMBase        = edd_ptr->k32_Btcm.base_ptr;
	p_setup->iniDPB.pK32DDR3Base        = edd_ptr->k32_ddr3.base_ptr;
	p_setup->iniDPB.GSharedRAM_BaseAddr = edd_ptr->g_shared_ram_base_ptr;
    p_setup->iniDPB.K32TraceLevel       = LSA_TRACE_LEVEL_OFF; // preset - setting from application see below

	// interfaces and ports
	p_setup->iniDPB.MaxInterfaceCntOfAllEDD = p_hd->nr_of_all_if;       // Number of all interfaces of all EDDs
	p_setup->iniDPB.MaxPortCntOfAllEDD      = p_hd->nr_of_all_ports;    // Number of all ports of all EDDs

    p_setup->iniDPB.InterfaceID             = p_inp->pnio_if.edd_if_id;
    p_setup->iniDPB.TraceIdx                = p_inp->pnio_if.trace_idx;
    p_setup->iniDPB.HWCfg.IRQAcycRcv        = 2;    // IRQ used with AcyclRcv, single IR starts at 2
    p_setup->iniDPB.NRT.TxItemsInTcpRing    = 32;   // enable TCP
    p_setup->iniDPB.NRT.RxItemsInTcpRing    = 32;
    
    p_setup->iniDPB.NRT.FeedInLoadLimitationActive  = ( PSI_FEATURE_ENABLE == p_inp->fill_active ) ?             EDD_SYS_FILL_ACTIVE          : EDD_SYS_FILL_INACTIVE;
    p_setup->iniDPB.NRT.IO_Configured               = ( PSI_FEATURE_ENABLE == p_inp->io_configured ) ?           EDD_SYS_IO_CONFIGURED_ON     : EDD_SYS_IO_CONFIGURED_OFF;
    p_setup->iniDPB.SWI.MRAEnableLegacyMode         = ( PSI_FEATURE_ENABLE == p_inp->mra_enable_legacy_mode ) ?  EDD_MRA_ENABLE_LEGACY_MODE   : EDD_MRA_DISABLE_LEGACY_MODE;
    if (PSI_FEATURE_ENABLE == p_inp->mra_enable_legacy_mode)
    {
        p_setup->iniDPB.SWI.MRAEnableLegacyMode = EDD_MRA_ENABLE_LEGACY_MODE;
    }
    else
    {
        p_setup->iniDPB.SWI.MRAEnableLegacyMode = EDD_MRA_DISABLE_LEGACY_MODE;
    }

    p_setup->iniDPB.CRT.pIOBaseAdrExtRam            = edd_ptr->sdram_CRT.base_ptr;
    p_setup->iniDPB.CRT.pIOBaseAdrPerif             = edd_ptr->perif_ram.base_ptr;

    // CRT ConsumerPM values
    // Fast mem - not yet supported
    p_setup->iniDPB.CRT.pCPMBaseAddrFastMem         = LSA_NULL;
    p_setup->iniDPB.CRT.CPMBaseAddrFastMemPNIP      = 0;
    // Slow mem
    p_setup->iniDPB.CRT.pCPMBaseAddrSlowMem         = edd_ptr->sdram_CRT.base_ptr;
    p_setup->iniDPB.CRT.CPMBaseAddrSlowMemPNIP      = edd_ptr->sdram_CRT.phy_addr;

    // CRT ProviderPM values
    // Fast mem  - not yet supported
    p_setup->iniDPB.CRT.pPPMBaseAddrFastMem         = LSA_NULL;
    p_setup->iniDPB.CRT.PPMBaseAddrFastMemPNIP      = 0;
    p_setup->iniDPB.CRT.PPM_Lifetime64FastMem       = 0;
    // Slow mem
    p_setup->iniDPB.CRT.pPPMBaseAddrSlowMem         = edd_ptr->sdram_CRT.base_ptr;
    p_setup->iniDPB.CRT.PPMBaseAddrSlowMemPNIP      = edd_ptr->sdram_CRT.phy_addr;
    p_setup->iniDPB.CRT.PPM_Lifetime64SlowMem       = 0;        /* 0 until calculation is supported */

    p_setup->iniDPB.CRT.ApplTimerMode               = edd_ptr->appl_timer_mode;
    p_setup->iniDPB.CRT.ApplTimerMode               = edd_ptr->appl_timer_mode;
    p_setup->iniDPB.CRT.ApplTimerReductionRatio     = edd_ptr->appl_timer_reduction_ratio;
    p_setup->iniDPB.CRT.isTransferEndCorrectionPos  = (edd_ptr->is_transfer_end_correction_pos) ? EDDP_DPB_TRANSFERENDCORRECTIONVALUE_IS_POS : EDDP_DPB_TRANSFERENDCORRECTIONVALUE_IS_NEG;
    p_setup->iniDPB.CRT.TransferEndCorrectionValue  = edd_ptr->transfer_end_correction_value;

    p_setup->iniDPB.SWI.MRPDefaultRoleInstance0     = p_inp->pnio_if.mrp.default_role_instance0;
    p_setup->iniDPB.SWI.MRPSupportedRole            = p_inp->pnio_if.mrp.supported_role;
    p_setup->iniDPB.SWI.MaxMRP_Instances            = p_inp->pnio_if.mrp.max_instances;
    p_setup->iniDPB.SWI.MRPSupportedMultipleRole    = p_inp->pnio_if.mrp.supported_multiple_role;
    // MRP interconnect
    p_setup->iniDPB.SWI.MaxMRPInterconn_Instances   = p_inp->pnio_if.mrp.max_mrp_interconn_instances;
    p_setup->iniDPB.SWI.SupportedMRPInterconnRole   = p_inp->pnio_if.mrp.supported_mrp_interconn_role;

    p_setup->iniDPB.NRT.RxFilterUDP_Unicast         = ( PSI_FEATURE_ENABLE == p_inp->pnio_if.nrt.RxFilterUDP_Unicast ) ?      EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;
    p_setup->iniDPB.NRT.RxFilterUDP_Broadcast       = ( PSI_FEATURE_ENABLE == p_inp->pnio_if.nrt.RxFilterUDP_Broadcast ) ?    EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;

    //p_setup->iniDPB.SWI.PhyAddressOffset is set by convenience function eddp_InitDPBWithDefaults().
    //p_setup->iniDPB.SWI.PhyAddressOffset          = 0;    /* for computing PHY-address at SMI-bus                                                 */
                                                            /* (SMI-bus is the management-bus between PNIP-ASIC and PHYs)                           */
                                                            /*                                                                                      */
                                                            /*             PHY-address(at SMI) = (HWPortID - 1) + PhyAddressOffset                  */
                                                            /* (this "PHY-address(at SMI)" should be equal to real PHY-address = DIP-switch-value)  */
                                                            /*                                                                                      */
                                                            /* 1.) example 4-porter (HERA):  "PhyAddressOffset" = 0 (= default)                     */
                                                            /* ------------------------------------------------------------------------------------ */
                                                            /*      slot1 (HWPortID=1) : NSC-Dual-PHY (PHY-address = 0 : no DIP-switch)             */
                                                            /*      slot2 (HWPortID=2) : NSC-Dual-PHY (PHY-address = 1 : no DIP-switch)             */
                                                            /*      slot3 (HWPortID=3) : BCM5461-PHY  (PHY-address = 2 : DIP-switch = 2)            */
                                                            /*      slot4 (HWPortID=4) : TI-PHY       (PHY-address = 3 : DIP-switch = 3)            */
                                                            /*                                                                                      */
                                                            /* 2.) example 2-porter:  "PhyAddressOffset" = 0 (= default)                            */
                                                            /* ------------------------------------------------------------------------------------ */
                                                            /*      slot1 (HWPortID=1) : BCM5461-PHY  (PHY-address = 0 : DIP-switch = 0)            */
                                                            /*      slot2 (HWPortID=2) : TI-PHY       (PHY-address = 1 : DIP-switch = 1)            */
                                                            /*                                                                                      */
                                                            /* 3.) example 2-porter:  "PhyAddressOffset" = 2                                        */
                                                            /* ------------------------------------------------------------------------------------ */
                                                            /*      slot1 (HWPortID=1) : BCM5461-PHY  (PHY-address = 2 : DIP-switch = 2)            */
                                                            /*      slot2 (HWPortID=2) : TI-PHY       (PHY-address = 3 : DIP-switch = 3)            */

    for (port_idx = 0; port_idx < EDD_CFG_MAX_PORT_CNT; port_idx++)
	{
		// Note: User ports from [1..N], 0 is reserved for auto port ID
		p_entry = psi_get_hw_port_entry_for_user_port((port_idx+1), p_inp);

		if (p_entry != LSA_NULL) // Entry found in inputs ?
		{
			// Set Port params (Portmapping and MRP setting)
			p_setup->iniDPB.SWI.PortMap.PortID_x_to_HardwarePort_y[port_idx] = hw_ptr->port_map[port_idx+1].hw_port_id;
			p_setup->iniDPB.SWI.MRPRingPort[port_idx]                        = (LSA_UINT8)p_entry->mrp_ringport_type;
		}
		else
		{
			// Set defaults for no port 
			p_setup->iniDPB.SWI.PortMap.PortID_x_to_HardwarePort_y[port_idx] = EDDP_PORT_NOT_CONNECTED;
			p_setup->iniDPB.SWI.MRPRingPort[port_idx]                        = EDD_MRP_NO_RING_PORT;
		}

        // MRP interconnect
        p_setup->iniDPB.SWI.SupportsMRPInterconnPortConfig[port_idx] = p_inp->pnio_if.mrp.supports_mrp_interconn_port_config[port_idx];
    }

	// Prepare DSB data for PN board
	// Note: using EDDP default settings (see eddp_InitDSBWithDefaults() in eddp_core_sys.c)
	EddRspVal = eddp_InitDSBWithDefaults(&p_setup->iniDPB, &p_setup->iniDSB);
	PSI_ASSERT(EddRspVal == EDD_STS_OK);

	p_setup->iniDSB.InterruptHandling.IRQ_WaitTime_AcycRcv = 31;	// 1ms throttle rate for acyclic receive irq

    // setup IF MAC
	PSI_MEMCPY(p_setup->iniDSB.IF_MAC_Address.MacAdr, p_hd->hd_hw.if_mac, EDD_MAC_ADDR_SIZE);

	for (port_idx = 0; port_idx < p_inp->nr_of_ports; port_idx++)
	{
		// setup port MAC addresses
		PSI_MEMCPY(p_setup->iniDSB.PortParams[port_idx].Port_MAC_Address.MacAdr, p_hd->hd_hw.port_mac[port_idx], EDD_MAC_ADDR_SIZE);

		// note: MDIX setting is set to on per default
		// overwrite this setting to off is possible after POF port detection, not here
    }

    // Features supported
    if (PSI_USE_SETTING_YES == p_inp->pnip.use_setting)
    {
        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddp_prepare_setup_data(): gigabit_support(%u)", p_inp->pnip.gigabit_support );

        // GBIT support only for PNIP-HERA
        // preset was done in eddp_InitDSBWithDefaults(): GBitSupport = EDD_FEATURE_DISABLE;
        if ((p_inp->asic_type == PSI_ASIC_TYPE_HERA) && (p_inp->pnip.gigabit_support == PSI_FEATURE_ENABLE))
        {
            for (port_idx = 0; port_idx < p_inp->nr_of_ports; port_idx++)
            {
                p_setup->iniDSB.PortParams[port_idx].GBitSupport = EDD_FEATURE_ENABLE;
            }

            p_setup->iniDPB.CRT.ProviderMaxNumber = 0;
            p_setup->iniDPB.CRT.ConsumerMaxNumber = 0;
        }

        p_setup->iniDPB.K32TraceLevel = p_inp->pnip.k32fw_trace_level;
    }

    // Features supported
    if (PSI_USE_SETTING_YES == p_inp->pnip.FeatureSupport.use_settings)
    {
        // Default values are overwritten
        p_setup->iniDPB.FeatureSupport.bShortPreambleSupported				= ( PSI_FEATURE_ENABLE == p_inp->pnip.FeatureSupport.short_preamble_supported )           ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.bMRPDSupported						= ( PSI_FEATURE_ENABLE == p_inp->pnip.FeatureSupport.mrpd_supported )                     ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.FragmentationtypeSupported           = p_inp->pnip.FeatureSupport.fragmentationtype_supported;
        // Values copied from input. Note: Values in irt_forwarding_mode_supported can be ored EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE, EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_RELATIVE or both
        p_setup->iniDPB.FeatureSupport.IRTForwardingModeSupported			= p_inp->pnip.FeatureSupport.irt_forwarding_mode_supported;
        p_setup->iniDPB.FeatureSupport.MaxDFPFrames							= p_inp->pnip.FeatureSupport.max_dfp_frames;
        p_setup->iniDPB.FeatureSupport.AdditionalForwardingRulesSupported   = ( PSI_FEATURE_ENABLE == p_inp->additional_forwarding_rules_supported )                  ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.ApplicationExist                     = ( PSI_FEATURE_ENABLE == p_inp->application_exist )                                      ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.ClusterIPSupport                     = ( PSI_FEATURE_ENABLE == p_inp->cluster_IP_support )                                     ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        // MRP interconnect
        p_setup->iniDPB.FeatureSupport.MRPInterconnFwdRulesSupported        = ( PSI_FEATURE_ENABLE == p_inp->pnip.FeatureSupport.mrp_interconn_fwd_rules_supported )  ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        p_setup->iniDPB.FeatureSupport.MRPInterconnOriginatorSupported      = ( PSI_FEATURE_ENABLE == p_inp->pnip.FeatureSupport.mrp_interconn_originator_supported ) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    }
}

/*----------------------------------------------------------------------------*/
#define PSI_PHY_CONFIG_VALUE_COPPER  0x417D   // init value for PHY_CONFIG Register for COPPER
#define PSI_PHY_CONFIG_VALUE_FIBER   0x437F   // init value for PHY_CONFIG Register for FIBER
#define PSI_PHY_CONFIG_OFFS          0x3C     // offset in SystemControlRegisterBlock

LSA_VOID psi_eddp_ertec200p_init_phy_config_reg(
    EDDP_SYS_HANDLE hSysDev )
{
    PSI_HD_STORE_PTR_TYPE       p_hd;
    volatile LSA_UINT32       * pPhyCfg;
    LSA_UINT32                  HwPortID;
    LSA_UINT32                  PhyCfgValue = PSI_PHY_CONFIG_VALUE_COPPER;
    PSI_EDD_SYS_PTR_TYPE  const sys_ptr     = (PSI_EDD_SYS_PTR_TYPE)hSysDev;
    LSA_UINT8                   MediaType, IsPOF, PhyType, FXTransceiverType;

    PSI_ASSERT(sys_ptr);
    PSI_ASSERT(sys_ptr->edd_comp_id == LSA_COMP_ID_EDDP);

    p_hd = psi_hd_get_hd_store(sys_ptr->hd_nr);
    PSI_ASSERT(p_hd);
    PSI_ASSERT(p_hd->is_used);

    LSA_UNUSED_ARG(PhyType);

    // set phy config register value depending on hardware type and pof
    switch (p_hd->hd_hw.edd.eddp.board_type)
    {
        case PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV1:
        case PSI_EDDP_BOARD_TYPE_FPGA1__ERTEC200P_REV2:
        {
            // With FPGA Board both Ports must be configured with the same Type. If
            // one Port is FX, both have to be configured with FX. This default setup
            // will be overwritten within EDDP depending on mediatype. Note: Config
            // both with CU and overwriting to FX later by EDDP seem not to work.
            for (HwPortID = 1; HwPortID <= PSI_CFG_MAX_PORT_CNT; HwPortID++)
            {
                PSI_EDD_GET_MEDIA_TYPE(
                    hSysDev, 
                    HwPortID, 
                    PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
                    &MediaType, 
                    &IsPOF, 
                    &PhyType,
                    &FXTransceiverType); /* not used here */

                if (MediaType == PSI_MEDIA_TYPE_FIBER) //POF port?
                {
                    PhyCfgValue = PSI_PHY_CONFIG_VALUE_FIBER;
                }
            }
        }
        break;

        case PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV1:
        case PSI_EDDP_BOARD_TYPE_EB200P__ERTEC200P_REV2:
        {
            // check the possible 2 ports for POF
            PSI_EDD_GET_MEDIA_TYPE(
                hSysDev, 
                1, 
                PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
                &MediaType, 
                &IsPOF, 
                &PhyType,
                &FXTransceiverType); /* not used here */

            if (MediaType == PSI_MEDIA_TYPE_FIBER) //HW Port 1 POF port?
            {
                PhyCfgValue &= 0xFF00;
                PhyCfgValue |= 0x007F;
            }

            PSI_EDD_GET_MEDIA_TYPE(
                hSysDev, 
                2, 
                PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
                &MediaType, 
                &IsPOF, 
                &PhyType,
                &FXTransceiverType); /* not used here */

            if (MediaType == PSI_MEDIA_TYPE_FIBER) //HW Port 2 POF port?
            {
                PhyCfgValue &= 0x00FF;
                PhyCfgValue |= 0x4300;
            }
        }
        break;

        default:
		{
		    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_eddp_ertec200p_init_phy_config_reg(): Unknown hardware type found, eddp.board_type(%u)", p_hd->hd_hw.edd.eddp.board_type);
		    PSI_FATAL(0);
		}
        //lint --e(788) enum constant not used within defaulted switch - all not mentioned types are handled in default case
    }

	// ptr to PHY_CONFIG register in mapped user ram
	pPhyCfg = (volatile LSA_UINT32 *)(((LSA_UINT32)p_hd->hd_hw.edd.eddp.apb_periph_scrb.base_ptr) + PSI_PHY_CONFIG_OFFS);

	*((volatile LSA_UINT32 *)pPhyCfg) = PhyCfgValue;
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddp_phy_get_media_type(
    EDDP_HANDLE       hDDB,
    EDDP_SYS_HANDLE   hSysDev,
    LSA_UINT32        HwPortID,
    LSA_UINT8       * pMediaType,
    LSA_UINT8       * pIsPOF,
    LSA_UINT8       * pFxTransceiverType )
{
    LSA_UINT8                  PhyType;
    PSI_EDD_SYS_PTR_TYPE const pSys = (PSI_EDD_SYS_PTR_TYPE)hSysDev;

    PSI_ASSERT(pSys);
    PSI_ASSERT(pSys->edd_comp_id == LSA_COMP_ID_EDDP);
    PSI_ASSERT((HwPortID >= 1) && (HwPortID <= EDD_CFG_MAX_PORT_CNT));

    LSA_UNUSED_ARG(hDDB);
    LSA_UNUSED_ARG(PhyType);
    
    // Get the status from the system adaptation
    if (PSI_EDD_IS_PORT_PULLED(pSys, HwPortID))
    {
        // default value for pulled port has to be set to EDD_MEDIATYPE_UNKNOWN
        *pMediaType         = EDD_MEDIATYPE_UNKNOWN; 
        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
        *pFxTransceiverType = EDD_FX_TRANSCEIVER_UNKNOWN;
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
            pFxTransceiverType);
    }
}

#endif //((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

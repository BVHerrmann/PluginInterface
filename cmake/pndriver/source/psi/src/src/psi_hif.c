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
/*  F i l e               &F: psi_hif.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for HIF                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   109
#define PSI_MODULE_ID       109 /* PSI_MODULE_ID_PSI_HIF */

#include "psi_int.h"

#if (PSI_CFG_USE_HIF == 1)

#include "psi_ld.h"

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== HIF ===================================================================*/
/*===========================================================================*/

LSA_RESULT psi_hif_ld_upper_get_path_info( /* PATH info for all channels open on LD upper side */
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
	HIF_DETAIL_PTR_TYPE     pDetails;
	PSI_SYS_PTR_TYPE        pSys;
    LSA_USER_ID_TYPE        user_id;
    LSA_UINT16              mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16              comp_id_lower = LSA_COMP_ID_UNUSED;
    HIF_HANDLE              hif_handle    = PSI_INVALID_HANDLE;
	LSA_UINT16              result        = LSA_RET_OK;

    LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_LD_TRACE_04( 0, LSA_TRACE_LEVEL_NOTE, "psi_hif_ld_upper_get_path_info(sys_path=%#x): pnio_if_nr(0) hd_nr(%u) path(%#x/%u)", 
        sys_path, hd_nr, path, path ); //psi_get_hd_runs_on_ld(hd_nr) removed, cause no psi instance available in light ld upper
    
	// Get the handle for LD Upper from Application
	PSI_HIF_GET_LD_UPPER_HANDLE(&hif_handle);

	PSI_ASSERT(sys_ptr_ptr != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetails), user_id, sizeof(*pDetails), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetails != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	pDetails->hH    = hif_handle;
	pDetails->hd_id = hd_nr;

    switch (path)
	{
		/* all global User channels */
	    #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 1)
	    case PSI_PATH_GLO_APP_SOCK1:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_1;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 2)
	    case PSI_PATH_GLO_APP_SOCK2:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_2;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 3)
	    case PSI_PATH_GLO_APP_SOCK3:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_3;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 4)
	    case PSI_PATH_GLO_APP_SOCK4:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_4;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 5)
	    case PSI_PATH_GLO_APP_SOCK5:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_5;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 6)
	    case PSI_PATH_GLO_APP_SOCK6:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_6;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 7)
	    case PSI_PATH_GLO_APP_SOCK7:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_7;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        #if (PSI_CFG_MAX_SOCK_APP_CHANNELS >= 8)
	    case PSI_PATH_GLO_APP_SOCK8:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCK_USER_CH_8;
			comp_id_lower     = LSA_COMP_ID_SOCK;
		}
		break;
	    #endif

        case PSI_PATH_GLO_APP_SOCKAPP:
        {
            mbx_id_rsp = PSI_MBX_ID_USER;
            pDetails->pipe_id = HIF_CPLD_GLO_APP_SOCKAPP_USER_CH;
            comp_id_lower = LSA_COMP_ID_SOCKAPP;
        }
        break;

        case PSI_PATH_GLO_APP_OHA:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_OHA_USER_CH;
			comp_id_lower     = LSA_COMP_ID_OHA;
		}
		break;

        case PSI_PATH_GLO_APP_DNS:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_DNS_USER_CH;
			comp_id_lower     = LSA_COMP_ID_DNS;
		}
		break;

        case PSI_PATH_GLO_APP_SNMPX:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_SNMPX_USER_CH;
			comp_id_lower     = LSA_COMP_ID_SNMPX;
		}
		break;

        case PSI_PATH_GLO_APP_IOS_CLRPC:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_CLRPC_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CLRPC;
		}
		break;

        case PSI_PATH_GLO_APP_EPS_LD:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_LD_EPS;
			comp_id_lower     = LSA_COMP_ID_PNBOARDS;
		}
		break;

		case PSI_PATH_GLO_APP_HSA:
		{
			mbx_id_rsp = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_GLO_APP_HSA_USER_CH;
			comp_id_lower = LSA_COMP_ID_HSA;
		}
		break;

        case PSI_PATH_GLO_APP_IOH:
        {
            mbx_id_rsp = PSI_MBX_ID_USER;
            pDetails->pipe_id = HIF_CPLD_GLO_APP_IOH_USER_CH;
            comp_id_lower = LSA_COMP_ID_IOH;
        }
        break;

        /* all IF specifc User channels */
	    case PSI_PATH_IF_APP_CMPD:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_CMPD_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_APP_IOC_CMCL_ACP:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_IOC_CMCL_ACP_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

        case PSI_PATH_IF_APP_IOM_CMMC:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_IOM_CMMC_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

        case PSI_PATH_IF_APP_IOD_CMSV_ACP:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_IOD_CMSV_ACP_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

        case PSI_PATH_IF_APP_OHA:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_OHA_USER_CH;
			comp_id_lower     = LSA_COMP_ID_OHA;
		}
		break;

        case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_EDD_SIMATIC_TIME_SYNC;
			comp_id_lower     = LSA_COMP_ID_EDD;
		}
		break;

        case PSI_PATH_IF_APP_NARE_IP_TEST:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_APP_NARE_IP_TEST;
			comp_id_lower     = LSA_COMP_ID_NARE;
		}
		break;

        case PSI_PATH_IF_APP_EPS_HD:
		{
			mbx_id_rsp        = PSI_MBX_ID_USER;
			pDetails->pipe_id = HIF_CPLD_IF_HD_EPS;
			comp_id_lower     = LSA_COMP_ID_PNBOARDS;
		}
		break;

        default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetails, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_LD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_hif_ld_upper_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = 0;
		pSys->comp_id               = LSA_COMP_ID_HIF;
		pSys->comp_id_lower         = comp_id_lower;
		pSys->comp_id_edd           = LSA_COMP_ID_UNUSED;
		pSys->mbx_id_rsp            = mbx_id_rsp;           // MBX for responses
		pSys->hSysDev               = 0;                    // Sys handle for HW out functions
		pSys->hPoolNrtTx            = 0;                    // NRT TX pool handle for NRT mem
		pSys->hPoolNrtRx            = 0;                    // NRT RX pool handle for NRT mem
		pSys->check_arp             = LSA_FALSE;
		pSys->trace_idx             = 0;
        pSys->hd_runs_on_level_ld   = PSI_HD_RUNS_ON_LEVEL_LD_NO;   //don't care but valid value is needed
        pSys->psi_path              = path;

		*detail_ptr_ptr = pDetails;
		*sys_ptr_ptr    = pSys;
	}

	return result;
}

#if (PSI_CFG_USE_LD_COMP == 1)

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_hif_hd_upper_get_path_info( /* PATH info for all channels open on HD upper side */
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
	HIF_DETAIL_PTR_TYPE     pDetails;
	PSI_SYS_PTR_TYPE        pSys;
    LSA_USER_ID_TYPE        user_id;
    LSA_UINT16              mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16              comp_id_lower = LSA_COMP_ID_UNUSED;
	LSA_UINT16              result        = LSA_RET_OK;

	LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

	// Get the HIF handle from PSI LD storage
	HIF_HANDLE                const hif_handle = psi_ld_get_hif_hd_upper_handle(hd_nr);
	PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(detail_ptr_ptr != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_hif_hd_upper_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetails), user_id, sizeof(*pDetails), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetails != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	pDetails->hH    = hif_handle;
	pDetails->hd_id = hd_nr;

	switch (path)
	{
		/* all IF secific SYS channels */
		/* Note: all sys channels has the endpoint in PSI LD */
	    case PSI_PATH_IF_SYS_MRP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_MRP_EDD;
			comp_id_lower     = LSA_COMP_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			mbx_id_rsp        = PSI_MBX_ID_TCIP;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_TCIP_EDD_ARP;
			comp_id_lower     = pDetailsIF->edd_comp_id;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ICMP:
		{
			mbx_id_rsp        = PSI_MBX_ID_TCIP;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_TCIP_EDD_ICMP;
			comp_id_lower     = pDetailsIF->edd_comp_id;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_UDP:
		{
			mbx_id_rsp        = PSI_MBX_ID_TCIP;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_TCIP_EDD_UDP;
			comp_id_lower     = pDetailsIF->edd_comp_id;
		}
		break;

    	#ifdef PSI_CFG_TCIP_CFG_SUPPORT_PATH_EDD_TCP
	    case PSI_PATH_IF_SYS_TCIP_EDD_TCP:
		{
			mbx_id_rsp        = PSI_MBX_ID_TCIP;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_TCIP_EDD_TCP;
			comp_id_lower     = pDetailsIF->edd_comp_id;
		}
		break;
    	#endif

	    case PSI_PATH_IF_SYS_DCP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_DCP_EDD;
			comp_id_lower     = LSA_COMP_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_NARE_EDD;
			comp_id_lower     = LSA_COMP_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_DCP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_NARE_DCP_EDD;
			comp_id_lower     = LSA_COMP_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_OHA;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_OHA_EDD;
			comp_id_lower     = pDetailsIF->edd_comp_id;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_OHA;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_OHA_LLDP_EDD;
			comp_id_lower     = LSA_COMP_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_DCP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_OHA;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_OHA_DCP_EDD;
			comp_id_lower     = LSA_COMP_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_NARE:
		{
			mbx_id_rsp        = PSI_MBX_ID_OHA;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_OHA_NARE;
			comp_id_lower     = LSA_COMP_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_MRP:
		{
			mbx_id_rsp        = PSI_MBX_ID_OHA;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_OHA_MRP;
			comp_id_lower     = LSA_COMP_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_GSY_EDD_SYNC;
			comp_id_lower     = LSA_COMP_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_GSY_EDD_ANNO;
			comp_id_lower     = LSA_COMP_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_ACP_EDD;
			comp_id_lower     = LSA_COMP_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_EDD;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_CLRPC:  /* Upper part from PSI LD to CM */
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_CLRPC;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_MRP:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_MRP;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_POF_EDD;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_GSY:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_GSY_CLOCK;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_NARE:
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_NARE;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_OHA: /* Upper part from PSI LD to CM */
		{
			mbx_id_rsp        = PSI_MBX_ID_PSI;
			pDetails->pipe_id = HIF_CPHD_IF_SYS_CM_OHA;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

		/* all IF specifc User channels */
	    case PSI_PATH_IF_APP_CMPD:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_CMPD_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_APP_IOC_CMCL_ACP:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_IOC_CMCL_ACP_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_APP_IOM_CMMC:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_IOM_CMMC_ACP_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_APP_IOD_CMSV_ACP:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_IOD_CMSV_ACP_USER_CH;
			comp_id_lower     = LSA_COMP_ID_CM;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_EDD_SIMATIC_TIME_SYNC;
			comp_id_lower     = LSA_COMP_ID_EDD;
		}
		break;

	    case PSI_PATH_IF_APP_NARE_IP_TEST:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_APP_NARE_IP_TEST;
			comp_id_lower     = LSA_COMP_ID_NARE;
		}
		break;

		case PSI_PATH_IF_APP_EPS_HD:
		{
			mbx_id_rsp        = PSI_MBX_ID_HIF_LD;
			pDetails->pipe_id = HIF_CPHD_IF_HD_EPS;
			comp_id_lower     = LSA_COMP_ID_PNBOARDS;
		}
		break;

		case PSI_PATH_IF_SYS_HSA_EDD:
		{
			mbx_id_rsp			= PSI_MBX_ID_HSA;
			pDetails->pipe_id	= HIF_CPHD_IF_SYS_HSA_EDD;
			comp_id_lower		= pDetailsIF->edd_comp_id;
		}
		break;

	    default:
		{
			LSA_UINT16      rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetails, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_hif_hd_upper_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_HIF;
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

		*detail_ptr_ptr = pDetails;
		*sys_ptr_ptr    = pSys;
	}

	return result;
}

#endif // #if (PSI_CFG_USE_LD_COMP == 1)

#if (PSI_CFG_USE_HD_COMP == 1)
LSA_RESULT psi_hif_hd_lower_get_path_info( /* PATH info for all channels open from lower side */
	LSA_SYS_PTR_TYPE      * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE     * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE       sys_path)
{
	HIF_DETAIL_PTR_TYPE     pDetails;
	PSI_SYS_PTR_TYPE        pSys;
    LSA_USER_ID_TYPE        user_id;
    LSA_UINT16              mbx_id_rsp    = PSI_MBX_ID_MAX;
	LSA_UINT16              comp_id_lower = LSA_COMP_ID_UNUSED;
	LSA_UINT16              result        = LSA_RET_OK;
	HIF_HANDLE              hif_handle;

	LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

	PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

	// Get the HIF handle from LD storage
	// Note: if HD Upper/lower not used in shortcut mode the handle is stored in HD FW system
	//       in this case the HD GET is delageted to FS sys adaption
    #if (PSI_CFG_USE_LD_COMP == 0)
    PSI_HIF_GET_HD_LOWER_HANDLE(&hif_handle, hd_nr);
    #else
    hif_handle = psi_ld_get_hif_hd_lower_handle(hd_nr);
    #endif

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(detail_ptr_ptr != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_hif_hd_lower_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetails), user_id, sizeof(*pDetails), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetails != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

	pDetails->hH    = hif_handle;
	pDetails->hd_id = hd_nr;

	switch (path)
	{
	    case PSI_PATH_IF_SYS_CM_CLRPC:  /* lower part from CM to CLRPC */
		{
			mbx_id_rsp        = PSI_MBX_ID_CM;
			pDetails->pipe_id = HIF_CPHD_L_IF_SYS_CM_CLRPC;
			comp_id_lower     = LSA_COMP_ID_CLRPC;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_OHA: /* lower part from CM to OHA */
		{
			mbx_id_rsp        = PSI_MBX_ID_CM;
			pDetails->pipe_id = HIF_CPHD_L_IF_SYS_CM_OHA;
			comp_id_lower     = LSA_COMP_ID_OHA;
		}
		break;

	    default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetails, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_hif_hd_lower_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_HIF;
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

		*detail_ptr_ptr     = pDetails;
		*sys_ptr_ptr        = pSys;
	}

	return result;
}

#endif // (PSI_CFG_USE_HD_COMP == 1)

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_hif_release_path_info(
	LSA_SYS_PTR_TYPE  sys_ptr,
	LSA_VOID_PTR_TYPE detail_ptr)
{
	LSA_UINT16       rc;
	PSI_SYS_PTR_TYPE pSys;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_HIF);

    PSI_LD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_hif_release_path_info()" );

    PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_HIF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
#else
#ifdef PSI_MESSAGE
#pragma PSI_MESSAGE("***** not compiling PSI HIF support (PSI_CFG_USE_HIF=" PSI_STRINGIFY(PSI_CFG_USE_HIF) ")")
#endif
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

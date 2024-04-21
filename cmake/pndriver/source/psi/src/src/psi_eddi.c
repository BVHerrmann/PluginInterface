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
/*  F i l e               &F: psi_eddi.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for EDDI                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   105
#define PSI_MODULE_ID       105 /* PSI_MODULE_ID_PSI_EDDI */

#include "psi_int.h"

#if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/

#if defined(PSI_CFG_EDDI_CFG_ERTEC_200)
static LSA_UINT32 const eddi_hw_type     = EDD_HW_TYPE_USED_ERTEC_200;
static LSA_UINT8  const eddi_hw_sub_type = EDDI_HW_SUBTYPE_USED_NONE;
static LSA_UINT32 const eddi_i2c_type    = EDDI_I2C_TYPE_GPIO;
static LSA_BOOL   const eddi_MII_used    = LSA_TRUE;
#elif defined(PSI_CFG_EDDI_CFG_ERTEC_400)
static LSA_UINT32 const eddi_hw_type     = EDD_HW_TYPE_USED_ERTEC_400;
static LSA_UINT8  const eddi_hw_sub_type = EDDI_HW_SUBTYPE_USED_NONE;
static LSA_UINT32 const eddi_i2c_type    = EDDI_I2C_TYPE_GPIO;
static LSA_BOOL   const eddi_MII_used    = LSA_FALSE;
#elif defined(PSI_CFG_EDDI_CFG_SOC)
static LSA_UINT32 const eddi_hw_type	 = EDD_HW_TYPE_USED_SOC;
static LSA_UINT8  const eddi_hw_sub_type = EDDI_HW_SUBTYPE_USED_SOC1;
static LSA_UINT32 const eddi_i2c_type	 = EDDI_I2C_TYPE_SOC1_HW;
static LSA_BOOL   const eddi_MII_used    = LSA_TRUE;
#else
#error "invalid EDDI configuration"
#endif

/*===========================================================================*/
/*=== EDDI ==================================================================*/
/*===========================================================================*/

static EDDI_CDB_TYPE * psi_init_CDB_EDDI(
	EDDI_CDB_TYPE     * const cdb,
	EDDI_CDB_NRT_TYPE * const pNRT,
	LSA_VOID_PTR_TYPE   const hDDB,
	LSA_UINT32          const filter )
{
	cdb->UsedComp.UseNRT    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseXRT    = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseSWITCH = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UseSYNC   = EDD_CDB_CHANNEL_USE_OFF;
	cdb->UsedComp.UsePRM    = EDD_CDB_CHANNEL_USE_OFF;

	cdb->hDDB         = hDDB;
	cdb->InsertSrcMAC = EDD_CDB_INSERT_SRC_MAC_DISABLED;
	cdb->pNRT         = LSA_NULL;

	cdb->Prm_PortDataAdjustLesserCheckQuality = EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD;

	// *--------------------------------------
	// *  NRT configuration
	// *--------------------------------------
	if (pNRT != LSA_NULL)
	{
		pNRT->Channel             = EDDI_NRT_CHANEL_A_IF_0; // default setting
		pNRT->FrameFilter         = filter;
		pNRT->UseFrameTriggerUnit = EDD_FEATURE_DISABLE;

		cdb->pNRT            = pNRT;
		cdb->UsedComp.UseNRT = EDD_CDB_CHANNEL_USE_ON;
	}

	return cdb;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_eddi_get_path_info(
	LSA_SYS_PTR_TYPE  * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   sys_path )
{
	EDDI_DETAIL_PTR_TYPE              pDetail;
	PSI_SYS_PTR_TYPE                  pSys;
	EDDI_UPPER_CDB_NRT_PTR_TYPE       pNRT;
    LSA_USER_ID_TYPE                  user_id;
    LSA_UINT16                        mbx_id_rsp = PSI_MBX_ID_MAX;
	LSA_BOOL                          arp_state  = LSA_FALSE;
	LSA_UINT16                        result     = LSA_RET_OK;
	LSA_UINT16  			  	const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
	LSA_UINT16  			  	const path       = PSI_SYSPATH_GET_PATH(sys_path);
	EDDI_HANDLE 			  	const hDDB       = psi_hd_get_hDDB(hd_nr);
	PSI_DETAIL_STORE_PTR_TYPE 	const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr    != LSA_NULL);
    PSI_ASSERT(detail_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDI);
	PSI_ASSERT(hDDB);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
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

			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, mrpFilter);

			pNRT->Channel               = EDDI_NRT_CHANEL_B_IF_0;
			pDetail->UsedComp.UseSWITCH = EDD_CDB_CHANNEL_USE_ON;

			mbx_id_rsp = PSI_MBX_ID_MRP;
		}
		break;

	    case PSI_PATH_IF_SYS_TCIP_EDD_ARP:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ARP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_ICMP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			if (pDetailsIF->tcip.multicast_support_on)  // MC support on ?
			{
				pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, (EDD_NRT_FRAME_IP_UDP | EDD_NRT_FRAME_IP_IGMP));
			}
			else
			{
				pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_UDP);
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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_IP_TCP);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP_HELLO /* | EDD_NRT_FRAME_UDP_DCP_HELLO */);

            mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_NARE_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, 0); // NRT for sending ARPs

			mbx_id_rsp = PSI_MBX_ID_NARE;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_EDD:
		{
			pDetail = psi_init_CDB_EDDI(pDetail, LSA_NULL, hDDB, 0);

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
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_DCP /* | EDD_NRT_FRAME_UDP_DCP */);

			mbx_id_rsp = PSI_MBX_ID_DCP;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, (EDD_NRT_FRAME_PTCP_SYNC | EDD_NRT_FRAME_PTCP_DELAY));

			pNRT->Channel             = EDDI_NRT_CHANEL_B_IF_0;
			pDetail->UsedComp.UseSYNC = EDD_CDB_CHANNEL_USE_ON;

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_GSY_EDD_ANNO:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_PTCP_ANNO);

			mbx_id_rsp = PSI_MBX_ID_GSY;
		}
		break;

	    case PSI_PATH_IF_SYS_OHA_LLDP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LLDP);

            pNRT->Channel = EDDI_NRT_CHANEL_B_IF_0;

			mbx_id_rsp = PSI_MBX_ID_LLDP;
		}
		break;

	    case PSI_PATH_IF_SYS_ACP_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_ASRT /* | EDD_NRT_FRAME_UDP_ASRT */);

			mbx_id_rsp = PSI_MBX_ID_ACP;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_EDD:
		{
			PSI_ALLOC_LOCAL_MEM(((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM);
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, 0); // NRT for EDD_SRV_NRT_SET_DCP_HELLO_FILTER

			pDetail->UsedComp.UsePRM = EDD_CDB_CHANNEL_USE_ON;
			pDetail->UsedComp.UseXRT = EDD_CDB_CHANNEL_USE_ON;

			/* Note: Must match with OHA allow_non_pnio_mautypes setting */
			pDetail->Prm_PortDataAdjustLesserCheckQuality = (pDetailsIF->oha.allow_non_pnio_mautypes) ? EDD_SYS_PDPORTDATAADJUST_LESSER_CHECK_QUALITY : EDD_SYS_PDPORTDATAADJUST_CHECK_FOR_PNIO_STANDARD;

			mbx_id_rsp = PSI_MBX_ID_CM;
		}
		break;

	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			pDetail = psi_init_CDB_EDDI(pDetail, LSA_NULL, hDDB, 0);

			mbx_id_rsp = PSI_MBX_ID_POF;
		}
		break;

	    case PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);

			pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_LEN_TSYNC);
			
			pDetail->InsertSrcMAC = EDD_CDB_INSERT_SRC_MAC_ENABLED;

			mbx_id_rsp = PSI_MBX_ID_PSI; // PSI EDD user hub
		}
		break;	

		case PSI_PATH_IF_SYS_HSA_EDD:
		{
			PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pNRT), user_id, sizeof(*pNRT), 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(pNRT != LSA_NULL);
			/* If the input parameters >additional_forwarding_rules_supported p_hd_in->application_exist are set, the value bHsyncAppl is true.
			   In this case, we register for the frame types EDD_NRT_FRAME_HSYNC. HSA will receive and send frames using this channel.
			   In the other case, HSA does not receive or send HSync frames, but HSA will use this channel to call EDD_SRV_GET_PARAMS.
		    */
			if (pDetailsIF->hsa.bHsyncAppl)
			{
				pDetail       = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, EDD_NRT_FRAME_HSYNC); // NRT for sending HSA Frames
				pNRT->Channel = EDDI_NRT_CHANEL_B_IF_0;
			}
			else
			{
				pDetail = psi_init_CDB_EDDI(pDetail, pNRT, hDDB, 0 /* use invalid EDD_NRT_FRAME type - we don't want to receive any frame using this channel. 0 == no reception. */);
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

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*sys_ptr_ptr    = LSA_NULL;
			*detail_ptr_ptr = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_eddi_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_EDDI;
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

		*sys_ptr_ptr    = pSys;
		*detail_ptr_ptr = pDetail;
	}

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_eddi_release_path_info(
	LSA_SYS_PTR_TYPE  sys_ptr,
	LSA_VOID_PTR_TYPE detail_ptr )
{
	LSA_UINT16                 rc;
	PSI_SYS_PTR_TYPE     const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;
	EDDI_DETAIL_PTR_TYPE       pDetail;

	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_EDDI);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_release_path_info()" );

	pDetail = (EDDI_DETAIL_PTR_TYPE)detail_ptr;

	if (pDetail->pNRT != LSA_NULL)
	{
		PSI_FREE_LOCAL_MEM( &rc, pDetail->pNRT, 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
		PSI_ASSERT(rc == LSA_RET_OK);
	}

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_EDDI, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

/*----------------------------------------------------------------------------*/
/* EDDI system calls for device open, setup, close used by PSI HD             */
/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_setup_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDI_SETUP_PTR_TYPE       p_setup;
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_setup = (PSI_EDDI_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_setup->session.req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_setup_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_setup->session.hd_nr, pRQB );

	psi_system_request_done(PSI_MBX_ID_PSI, p_setup->session.req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_close_request_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	PSI_EDDI_SESSION_PTR_TYPE       p_session;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);

	// Read the session data for changing the CBF and context switch to PSI task
	p_session = (PSI_EDDI_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->req_done_func != LSA_NULL);

	PSI_HD_TRACE_02( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_close_request_done(): hd_nr(%u) pRQB(0x%08x)",
		p_session->hd_nr, pRQB );

	psi_system_request_done(PSI_MBX_ID_PSI, p_session->req_done_func, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_device_open_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
    LSA_UINT16                	      retVal;
    LSA_UINT32                 	      port_idx;
    LSA_UINT8                  	      MediaType, IsPOF, PhyType, FXTransceiverType;
    PSI_EDDI_SETUP_PTR_TYPE    	      p_setup;
    PSI_HD_STORE_PTR_TYPE      	      p_hd;
    EDDI_UPPER_DDB_INI_PTR_TYPE       pOpen;
    EDD_UPPER_RQB_PTR_TYPE      const pRQB                       = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
    PSI_RQB_PTR_TYPE            const psi_hd_open_device_rqb_ptr = psi_hd_get_hd_open_device_rqb_ptr();
    PSI_HD_OUTPUT_PTR_TYPE      const p_hd_out                   = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_out;
    PSI_HD_INPUT_PTR_TYPE       const p_hd_in                    = &psi_hd_open_device_rqb_ptr->args.hd_open.hd_args;

    PSI_ASSERT(pRQB != LSA_NULL);
    PSI_ASSERT(EDD_STS_OK        == EDD_RQB_GET_RESPONSE(pRQB));
    PSI_ASSERT(EDD_OPC_SYSTEM    == EDD_RQB_GET_OPCODE(pRQB));
    PSI_ASSERT(EDDI_SRV_DEV_OPEN == EDD_RQB_GET_SERVICE(pRQB));

    pOpen = (EDDI_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
    PSI_ASSERT(pOpen != LSA_NULL);

    // Read the session data (setup) for next request
    p_setup = (PSI_EDDI_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
    PSI_ASSERT(p_setup != LSA_NULL);

    // Register hDDB for this board
    *p_setup->session.hDDB_ptr = pOpen->hDDB;

    p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
    PSI_ASSERT(p_hd);
    PSI_ASSERT(p_hd->is_used);
    PSI_ASSERT(p_hd->nr_of_ports <= PSI_CFG_MAX_PORT_CNT);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_OPEN_DONE;

    PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_open_done(): hd_nr(%u)", p_setup->session.hd_nr);

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
            &FXTransceiverType);

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
        //lint --e(506) --e(774) Constant value Boolean ; Boolean within 'if' always evaluates to True - the 'eddi_MII_used' can be defined TRUE/FALSE dependent of ASIC
        if (eddi_MII_used)
            p_hd_out->hw_port[port_idx].xmii_type = PSI_PORT_MII;
        else
            p_hd_out->hw_port[port_idx].xmii_type = PSI_PORT_RMII;

        /* EDD-Asic-Type */
        p_hd_out->hw_port[port_idx].edd_asic_type = (PSI_ASIC_TYPE)(p_hd_in->asic_type);

        /* is ring port? */
        p_hd_out->hw_port[port_idx].isRingPort = PSI_PORT_DEFAULT_RING_PORT;
        // for device with 3 ports: port(3) = no ring
        if ((p_hd_in->nr_of_ports == 3) && (port_idx == 2))
        {
            p_hd_out->hw_port[port_idx].isRingPort = PSI_PORT_NO_DEFAULT_RING_PORT; // port(3) of SOC1
        }

        PSI_HD_TRACE_08( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_open_done(): HD-Outputs: HwPortId(%u) media_type(0x%x) media_type_psubmodidtnr(0x%x) fiberoptic_type(0x%x) phy_type(0x%x) xmii_type(0x%x) edd_asic_type(0x%x) isRingPort(%u)",
            HwPortId,
            p_hd_out->hw_port[port_idx].media_type,
            p_hd_out->hw_port[port_idx].media_type_psubmodidtnr,
            p_hd_out->hw_port[port_idx].fiberoptic_type,
            p_hd_out->hw_port[port_idx].phy_type,
            p_hd_out->hw_port[port_idx].xmii_type,
            p_hd_out->hw_port[port_idx].edd_asic_type,
            p_hd_out->hw_port[port_idx].isRingPort );
    }

    #if defined(PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON)
    //start external timer (replacement for NEWCYCLE-Int)
    PSI_EDDI_EXT_TIMER_CMD(p_setup->session.hd_sys_handle, 
                           PSI_EDDI_EXTTIMER_CMD_INIT_START,
                           pOpen->hDDB);
    #endif

    EDDI_FREE_UPPER_MEM_LOCAL(&retVal, pRQB->pParam);
    PSI_ASSERT(retVal == LSA_RET_OK);

    EDDI_FREE_UPPER_RQB_LOCAL(&retVal, pRQB);
    PSI_ASSERT(retVal == EDD_STS_OK);

    // Enable the PNIO IRs
    PSI_HD_ENABLE_EVENT(p_setup->session.hd_sys_handle);

    // continue with basic setup
    psi_eddi_basic_setup(p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddi_device_open(
    PSI_EDDI_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE          pRQB;
	EDDI_UPPER_DDB_INI_PTR_TYPE     pOpen;
	EDDI_UPPER_DPB_PTR_TYPE         pDPB;
	PSI_HD_STORE_PTR_TYPE           p_hd;

	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_setup->session.hDDB_ptr != LSA_NULL);

	pDPB = &p_setup->iniDPB;
    //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'p_setup->iniDPB'
    PSI_ASSERT(pDPB != LSA_NULL);

	EDDI_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDI_ALLOC_UPPER_MEM_LOCAL(&pRQB->pParam, sizeof(EDDI_RQB_DDB_INI_TYPE) );
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDI);
	EDD_RQB_SET_SERVICE(pRQB, EDDI_SRV_DEV_OPEN);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_OPEN_START;

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddi_device_open_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	pOpen          = (EDDI_UPPER_DDB_INI_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pOpen->pDPB    = pDPB;
	pOpen->Cbf     = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddi_setup_request_done;
	pOpen->hSysDev = p_setup->session.hd_sys_handle;
	pOpen->hDDB    = LSA_NULL;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_open(): EDDI_SRV_DEV_OPEN, hd_nr(%u)", p_setup->session.hd_nr );

	psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_basic_setup_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                    retVal;
	PSI_EDDI_SETUP_PTR_TYPE       p_setup;
	PSI_HD_STORE_PTR_TYPE         p_hd;
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK            == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM        == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDI_SRV_DEV_COMP_INI == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDI_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_COMP_INI_DONE;

    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_basic_setup_done(): hd_nr(%u)", p_setup->session.hd_nr );

	EDDI_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT(retVal == EDD_STS_OK);

	// Continue with device setup
	psi_eddi_device_setup(p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddi_basic_setup(
    PSI_EDDI_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	EDDI_UPPER_CMP_INI_PTR_TYPE pCMP;
	PSI_HD_STORE_PTR_TYPE       p_hd;

	PSI_ASSERT(p_setup != LSA_NULL);

	pCMP = &p_setup->iniCOMP;
    //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'p_setup->iniCOMP'
    PSI_ASSERT(pCMP != LSA_NULL);

	EDDI_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDI);

	EDD_RQB_SET_SERVICE(pRQB, EDDI_SRV_DEV_COMP_INI);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_COMP_INI_START;

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddi_basic_setup_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	pRQB->pParam = pCMP;
	pCMP->Cbf    = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddi_setup_request_done;
	pCMP->hDDB   = *p_setup->session.hDDB_ptr;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_basic_setup(): EDDI_SRV_DEV_COMP_INI, hd_nr(%u)", p_setup->session.hd_nr );

	psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_device_setup_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                    retVal;
	PSI_EDDI_SETUP_PTR_TYPE       p_setup;
	EDD_UPPER_RQB_PTR_TYPE  const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_STS_OK         == EDD_RQB_GET_RESPONSE(pRQB));
	PSI_ASSERT(EDD_OPC_SYSTEM     == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDI_SRV_DEV_SETUP == EDD_RQB_GET_SERVICE(pRQB));

	// Read the session data (setup) for next request
	p_setup = (PSI_EDDI_SETUP_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_setup != LSA_NULL);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_SETUP_DONE;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_setup_done(): hd_nr(%u)", p_setup->session.hd_nr );

	EDDI_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDI_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT(retVal == EDD_STS_OK);

	// Device open sequence finished --> notify done
	psi_hd_edd_open_done(p_setup->session.hd_nr, p_setup);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddi_device_setup(
    PSI_EDDI_SETUP_PTR_TYPE p_setup )
{
	EDD_UPPER_RQB_PTR_TYPE        pRQB;
	EDDI_UPPER_DDB_SETUP_PTR_TYPE pData;
	EDDI_UPPER_DSB_PTR_TYPE       pDSB;
	PSI_HD_STORE_PTR_TYPE         p_hd;

	PSI_ASSERT(p_setup != LSA_NULL);

	pDSB = &(p_setup->iniDSB);
    //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'p_setup->iniDSB'
    PSI_ASSERT(pDSB != LSA_NULL);

	EDDI_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof( EDD_RQB_TYPE ));

	EDDI_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDI_RQB_DDB_SETUP_TYPE) );
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDI);

	EDD_RQB_SET_SERVICE(pRQB,EDDI_SRV_DEV_SETUP);

	p_hd = psi_hd_get_hd_store(p_setup->session.hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_SETUP_START;

	// Register setup data (session data) for next request in done
	p_setup->session.req_done_func = psi_eddi_device_setup_done;
	PSI_RQB_SET_USER_ID_PTR( pRQB, p_setup );

	pData       = (EDDI_UPPER_DDB_SETUP_PTR_TYPE)EDD_RQB_GET_PPARAM( pRQB );
	pData->pDSB = pDSB;
	pData->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddi_setup_request_done;
	pData->hDDB = *p_setup->session.hDDB_ptr;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_setup(): hd_nr(%u)", p_setup->session.hd_nr );

	psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_system, pRQB);
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_device_close_done(
    LSA_VOID_PTR_TYPE const rqb_ptr )
{
	LSA_UINT16                      retVal;
	PSI_EDDI_SESSION_PTR_TYPE       p_session;
	PSI_HD_STORE_PTR_TYPE           p_hd;
	EDD_UPPER_RQB_PTR_TYPE    const pRQB = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;

	PSI_ASSERT(pRQB != LSA_NULL);
	PSI_ASSERT(EDD_OPC_SYSTEM == EDD_RQB_GET_OPCODE(pRQB));
	PSI_ASSERT(EDDI_SRV_DEV_CLOSE == EDD_RQB_GET_SERVICE(pRQB));
	PSI_ASSERT(EDD_STS_OK == EDD_RQB_GET_RESPONSE(pRQB));

	// Read the session data (setup) for next request
	p_session = (PSI_EDDI_SESSION_PTR_TYPE)PSI_RQB_GET_USER_ID_PTR(pRQB);
	PSI_ASSERT(p_session != LSA_NULL);

	// set HD to closed state
	*p_session->hDDB_ptr = LSA_NULL;

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_CLOSE_DONE;

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_close_done(): hd_nr(%u)", p_session->hd_nr );

	EDDI_FREE_UPPER_MEM_LOCAL( &retVal, pRQB->pParam );
	PSI_ASSERT(retVal == LSA_RET_OK);

	EDDI_FREE_UPPER_RQB_LOCAL( &retVal, pRQB );
	PSI_ASSERT(retVal == EDD_STS_OK);

    // Device close sequence finished --> notify done
	psi_hd_edd_close_done(p_session->hd_nr, p_session);
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddi_device_close(
    PSI_EDDI_SESSION_PTR_TYPE p_session )
{
	EDD_UPPER_RQB_PTR_TYPE      pRQB;
	EDDI_UPPER_DDB_REL_PTR_TYPE pClose;
	PSI_HD_STORE_PTR_TYPE       p_hd;

	PSI_ASSERT(p_session != LSA_NULL);
	PSI_ASSERT(p_session->hDDB_ptr != LSA_NULL);

	/* Disable the IR */
	PSI_HD_DISABLE_EVENT(p_session->hd_sys_handle);

	EDDI_ALLOC_UPPER_RQB_LOCAL( &pRQB, sizeof(EDD_RQB_TYPE) );
	PSI_ASSERT(pRQB != LSA_NULL);

	PSI_MEMSET( pRQB, 0, sizeof(EDD_RQB_TYPE) );

	EDDI_ALLOC_UPPER_MEM_LOCAL( &pRQB->pParam, sizeof(EDDI_RQB_DDB_REL_TYPE) );
	PSI_ASSERT(pRQB->pParam != LSA_NULL);

	PSI_RQB_SET_OPCODE(pRQB, EDD_OPC_SYSTEM);
	PSI_RQB_SET_COMP_ID(pRQB, LSA_COMP_ID_EDDI);
	EDD_RQB_SET_SERVICE(pRQB, EDDI_SRV_DEV_CLOSE);

    p_hd = psi_hd_get_hd_store(p_session->hd_nr);
	PSI_ASSERT(p_hd);
	PSI_ASSERT(p_hd->is_used);

    p_hd->eddi_state = PSI_HD_EDDI_STATE_DEV_CLOSE_START;

	// Register setup data (session data) for next request in done
	p_session->req_done_func = psi_eddi_device_close_done;
	PSI_RQB_SET_USER_ID_PTR(pRQB, p_session);

	pClose       = (EDDI_UPPER_DDB_REL_PTR_TYPE)EDD_RQB_GET_PPARAM(pRQB);
	pClose->Cbf  = (EDD_UPPER_CALLBACK_FCT_PTR_TYPE)psi_eddi_close_request_done;
	pClose->hDDB = *(p_session->hDDB_ptr);

	PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_eddi_device_close(): EDDI_SRV_DEV_CLOSE, hd_nr(%u)", p_session->hd_nr );

    #if defined(PSI_CFG_EDDI_CFG_SII_EXTTIMER_MODE_ON)
    //stop external timer (replacement for NEWCYCLE-Int)
    PSI_EDDI_EXT_TIMER_CMD(p_session->hd_sys_handle, 
                           PSI_EDDI_EXTTIMER_CMD_STOP,
                           0 /*hDDB not needed*/); 
    #endif

	psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_system, pRQB);
}

/*----------------------------------------------------------------------------*/
/* EDDI function for setting device setup params using EDDI conv funtions     */
/*----------------------------------------------------------------------------*/
static LSA_VOID psi_eddi_init_setup_data(
	PSI_EDDI_SETUP_PTR_TYPE const setup_ptr,
	PSI_HD_INPUT_PTR_TYPE   const inp_ptr )
{
	EDDI_GLOBAL_INI_TYPE     globalIni;
	PSI_HD_IF_INPUT_PTR_TYPE if_ptr;
	LSA_UINT8                rport1 = 0;
	LSA_UINT8                rport2 = 0;
    LSA_RESULT               EddRspVal;

	PSI_ASSERT(setup_ptr != LSA_NULL);
	PSI_ASSERT(inp_ptr   != LSA_NULL);

	// Use the EDDI function to initialize the EDDI setup data
	// Note: this values are used in HW setup, and detailed in EDDx setup
	//       all settings with fixed values are taken from expample(see eddi_bsp_edd_ini.c)
	PSI_MEMSET( &globalIni, 0, sizeof(globalIni) );

	if_ptr = &inp_ptr->pnio_if;

	// get the MRP port settings for this IF
	if (psi_get_mrp_default_rports(inp_ptr, &rport1, &rport2))
	{
		rport1 = 0;
		rport2 = 0;
	}

    /* ------------------------------------------------------------ */
	/* Prepare the global input for the EDDI default settings       */
    /* ------------------------------------------------------------ */
	globalIni.InterfaceID                       = if_ptr->edd_if_id;

	// IRTE
	globalIni.GenIni.HWTypeUsed                 = eddi_hw_type;        // ASIC specific
	globalIni.GenIni.HWSubTypeUsed              = eddi_hw_sub_type;    // ASIC specific
	globalIni.GenIni.NrOfPorts                  = (LSA_UINT8)inp_ptr->nr_of_ports;
	globalIni.GenIni.bIRTSupported              = (PSI_FEATURE_ENABLE == inp_ptr->irte.irt_supported) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
	globalIni.GenIni.NrOfIRTForwarders          = inp_ptr->irte.nr_irt_forwarder;
	globalIni.GenIni.I2C_Type                   = eddi_i2c_type;       // ASIC specific

	// MRP
	globalIni.GenIni.MRPDefaultRoleInstance0    = if_ptr->mrp.default_role_instance0;
	globalIni.GenIni.MRPDefaultRingPort1        = rport1;
	globalIni.GenIni.MRPDefaultRingPort2        = rport2;

	// IRTE
	switch (inp_ptr->irte.buffer_capacity_use_case)
	{
	    case PSI_IRTE_USE_CASE_DEFAULT:
		{
            globalIni.GenIni.BufferCapacityUseCase = EDDI_GIS_USECASE_DEFAULT;
		}
		break;

        /* PLC with SOC1, 2 ports. Max 512 IODs */
	    case PSI_IRTE_USE_CASE_IOC_SOC1_2P:
		{
            /* asic_type must be SOC1 */
            if (inp_ptr->asic_type != PSI_ASIC_TYPE_SOC1)
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): asic_type(0x%X) is not SOC1 for UseCase(IOC_SOC1_2P)",
			        inp_ptr->asic_type );
		        PSI_FATAL( 0 );
            }

            /* check nr_of_ports */
            if ((inp_ptr->nr_of_ports == 0) || (inp_ptr->nr_of_ports > 2))
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): nr_of_ports(%u) is invalid for UseCase(IOC_SOC1_2P)",
			        inp_ptr->nr_of_ports );
		        PSI_FATAL( 0 );
            }

            globalIni.GenIni.BufferCapacityUseCase = EDDI_GIS_USECASE_IOC_SOC1_2P;
		}
		break;

        /* PLC with SOC1, 3 ports. Max 256 IODs */
	    case PSI_IRTE_USE_CASE_IOC_SOC1_3P:
		{
            /* asic_type must be SOC1 */
            if (inp_ptr->asic_type != PSI_ASIC_TYPE_SOC1)
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): asic_type(0x%X) is not SOC1 for UseCase(IOC_SOC1_3P)",
			        inp_ptr->asic_type );
		        PSI_FATAL( 0 );
            }

            /* check nr_of_ports */
            if ((inp_ptr->nr_of_ports == 0) || (inp_ptr->nr_of_ports > 3))
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): nr_of_ports(%u) is invalid for UseCase(IOC_SOC1_3P)",
			        inp_ptr->nr_of_ports );
		        PSI_FATAL( 0 );
            }

            globalIni.GenIni.BufferCapacityUseCase = EDDI_GIS_USECASE_IOC_SOC1_3P;
		}
		break;

        /* IOD with ERTEC400 (Scalance). No IOC resources. */
	    case PSI_IRTE_USE_CASE_IOD_ERTEC400:
		{
            /* asic_type must be ERTEC400 */
            if (inp_ptr->asic_type != PSI_ASIC_TYPE_ERTEC400)
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): asic_type(0x%X) is not ERTEC400 for UseCase(IOD_ERTEC400)",
			        inp_ptr->asic_type );
		        PSI_FATAL( 0 );
            }

            /* check nr_of_ports */
            if ((inp_ptr->nr_of_ports == 0) || (inp_ptr->nr_of_ports > 4))
            {
		        PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): nr_of_ports(%u) is invalid for UseCase(IOD_ERTEC400)",
			        inp_ptr->nr_of_ports );
		        PSI_FATAL( 0 );
            }

            globalIni.GenIni.BufferCapacityUseCase = EDDI_GIS_USECASE_IOD_ERTEC400;
		}
		break;

        default:
        {
		    PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): unknown buffer_capacity_use_case(0x%X)",
			    inp_ptr->irte.buffer_capacity_use_case );
		    PSI_FATAL( 0 );
        }
    }

	switch (inp_ptr->hsync_role)
	{
	    case PSI_HSYNC_ROLE_NONE:
		    globalIni.GenIni.HSYNCRole = EDDI_HSYNC_ROLE_NONE;
		break;

        case PSI_HSYNC_ROLE_FORWARDER:
		    globalIni.GenIni.HSYNCRole = EDDI_HSYNC_ROLE_FORWARDER;
		break;

        case PSI_HSYNC_ROLE_APPL_SUPPORT:
		    globalIni.GenIni.HSYNCRole = EDDI_HSYNC_ROLE_APPL_SUPPORT;
		break;

        default:
        {
            PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_init_setup_data(): unknown hsync_role(0x%X)", inp_ptr->hsync_role);
            PSI_FATAL(0);
        }
	}

	// IOC
    globalIni.IOCIni.NrOfRTDevices          = if_ptr->ioc.nr_of_rt_devices;
	globalIni.IOCIni.NrOfIRTDevices         = if_ptr->ioc.nr_of_irt_devices;
	globalIni.IOCIni.NrOfRedIRTDevices      = if_ptr->ioc.nr_of_irt_devices; // same as non Red
	globalIni.IOCIni.NrOfQVProviders        = if_ptr->ioc.nr_of_qv_provider;
	globalIni.IOCIni.NrOfQVConsumers        = if_ptr->ioc.nr_of_qv_consumer;
	globalIni.IOCIni.NrOfRedQVConsumers     = if_ptr->ioc.nr_of_qv_consumer; // same as non Red
	globalIni.IOCIni.LogicalAddressSpace    = if_ptr->ioc.logical_addr_space;
	globalIni.IOCIni.NrOfSubmod             = if_ptr->ioc.nr_of_submod;
	globalIni.IOCIni.MaxIOCRDataSize        = if_ptr->ioc.max_iocr_data_size;
	globalIni.IOCIni.MaxMCRDataSize         = if_ptr->ioc.max_mcr_data_size;

	// IOD
	globalIni.IODIni.NrOfInstances  		= if_ptr->iod.nr_of_instances;
	globalIni.IODIni.NrOfARs        		= if_ptr->iod.iod_max_ar_IOC; 
	globalIni.IODIni.MaxOCRDataSize 		= if_ptr->iod.max_ocr_data_size;
	globalIni.IODIni.MaxICRDataSize 		= if_ptr->iod.max_icr_data_size;
	globalIni.IODIni.MaxMCRDataSize 		= if_ptr->iod.max_mcr_data_size;

	// NRT
    //lint --e(506) Constant value Boolean - the 'eddi_MII_used' can be defined TRUE/FALSE dependent of ASIC
    globalIni.NRTIni.bMIIUsed               = (eddi_MII_used) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;       // ASIC specific

	// Use the default settings as base
    EddRspVal = eddi_InitDPBWithDefaults(&setup_ptr->iniDPB, &globalIni);
    PSI_ASSERT(EddRspVal == EDD_STS_OK);
    PSI_ASSERT(globalIni.KRAMFree > 0);

    EddRspVal = eddi_InitCOMPWithDefaults(&setup_ptr->iniCOMP, &globalIni);
    PSI_ASSERT(EddRspVal == EDD_STS_OK);

    EddRspVal = eddi_InitDSBWithDefaults(&setup_ptr->iniDSB, &globalIni);
    PSI_ASSERT(EddRspVal == EDD_STS_OK);

	// overwrite values from DSB
	setup_ptr->iniDSB.GlobPara.MRAEnableLegacyMode = ( PSI_FEATURE_ENABLE == inp_ptr->mra_enable_legacy_mode )? EDD_MRA_ENABLE_LEGACY_MODE : EDD_MRA_DISABLE_LEGACY_MODE;

	// Features supported
    if (PSI_USE_SETTING_YES == inp_ptr->irte.FeatureSupport.use_settings)
    {
        // Default values are overwritten
        setup_ptr->iniDPB.FeatureSupport.bMRPDSupported						= ( PSI_FEATURE_ENABLE == inp_ptr->irte.FeatureSupport.mrpd_supported )                     ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        setup_ptr->iniDPB.FeatureSupport.FragmentationtypeSupported         = inp_ptr->irte.FeatureSupport.fragmentationtype_supported;
        setup_ptr->iniDPB.FeatureSupport.MaxDFPFrames						= inp_ptr->irte.FeatureSupport.max_dfp_frames;
		setup_ptr->iniDPB.FeatureSupport.AdditionalForwardingRulesSupported = ( PSI_FEATURE_ENABLE == inp_ptr->additional_forwarding_rules_supported )                  ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
		setup_ptr->iniDPB.FeatureSupport.ApplicationExist					= ( PSI_FEATURE_ENABLE == inp_ptr->application_exist )                                      ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        setup_ptr->iniDPB.FeatureSupport.ClusterIPSupport                   = ( PSI_FEATURE_ENABLE == inp_ptr->cluster_IP_support )                                     ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
		// Values copied from input. Note: Values in irt_forwarding_mode_supported can be ored EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_ABSOLUTE, EDD_DPB_FEATURE_IRTFWDMODE_SUPPORTED_RELATIVE or both
		setup_ptr->iniDPB.FeatureSupport.IRTForwardingModeSupported         = inp_ptr->irte.FeatureSupport.irt_forwarding_mode_supported;

        // MRP interconnect
        setup_ptr->iniDPB.FeatureSupport.MRPInterconnFwdRulesSupported      = ( PSI_FEATURE_ENABLE == inp_ptr->irte.FeatureSupport.mrp_interconn_fwd_rules_supported )  ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
        setup_ptr->iniDPB.FeatureSupport.MRPInterconnOriginatorSupported    = ( PSI_FEATURE_ENABLE == inp_ptr->irte.FeatureSupport.mrp_interconn_originator_supported ) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_eddi_prepare_setup_data(
	PSI_EDDI_SETUP_PTR_TYPE p_setup,
	PSI_HD_INPUT_PTR_TYPE   p_inp,
	PSI_HD_STORE_PTR_TYPE   p_hd )
{
	PSI_HD_IF_INPUT_PTR_TYPE   if_ptr;
	PSI_HD_EDDI_PTR_TYPE       edd_ptr;
	PSI_HD_HW_PTR_TYPE         hw_ptr;
	PSI_HD_PORT_INPUT_PTR_TYPE p_entry;
	PSI_MAC_TYPE               set_mac;
	LSA_UINT16                 port_idx;

	PSI_ASSERT(p_setup != LSA_NULL);
	PSI_ASSERT(p_inp   != LSA_NULL);
	PSI_ASSERT(p_hd    != LSA_NULL);

    // Startup SendClockFactor (=CycleBaseFactor) invalid? valid = 32 (=1ms Cylce-Time)
    if (p_inp->send_clock_factor != (LSA_UINT16)32)
    {
		PSI_HD_TRACE_01( 0, LSA_TRACE_LEVEL_FATAL, "psi_eddi_prepare_setup_data(): invalid send_clock_factor(%u)",
			p_inp->send_clock_factor );
		PSI_FATAL( 0 );
    }

	// First initialize the EDDI device params with the EDDI defaults
	psi_eddi_init_setup_data(p_setup, p_inp);

	// Overwrite the EDDI defaults with special settings for this HD
	if_ptr  = &p_inp->pnio_if;
	hw_ptr  = &p_hd->hd_hw;
	edd_ptr = &p_hd->hd_hw.edd.eddi;

	// ----------------- Init DPB data  -----------------
	p_setup->iniDPB.InterfaceID = if_ptr->edd_if_id;
	p_setup->iniDPB.TraceIdx    = if_ptr->trace_idx;  // Trace index used for this interface

    // interfaces and ports
    p_setup->iniDPB.MaxInterfaceCntOfAllEDD     = p_hd->nr_of_all_if;       // Number of all interfaces of all EDDs
    p_setup->iniDPB.MaxPortCntOfAllEDD          = p_hd->nr_of_all_ports;    // Number of all ports of all EDDs

	p_setup->iniDPB.IRTE_SWI_BaseAdr           = (LSA_UINT32)edd_ptr->irte.base_ptr;
	p_setup->iniDPB.IRTE_SWI_BaseAdr_LBU_16Bit = 0;
    p_setup->iniDPB.GSharedRAM_BaseAdr         = (LSA_UINT32)edd_ptr->g_shared_ram_base_ptr;

    #if ( PSI_CFG_USE_NRT_CACHE_SYNC == 1 )
    p_setup->iniDPB.NRTMEM_LowerLimit          = (LSA_UINT32)edd_ptr->sdram_cached.base_ptr;
    p_setup->iniDPB.NRTMEM_UpperLimit          = (LSA_UINT32)edd_ptr->sdram_cached.base_ptr + edd_ptr->sdram_cached.size;
    #else
	p_setup->iniDPB.NRTMEM_LowerLimit          = (LSA_UINT32)edd_ptr->sdram.base_ptr;
	p_setup->iniDPB.NRTMEM_UpperLimit          = (LSA_UINT32)edd_ptr->sdram.base_ptr + edd_ptr->sdram.size;
    #endif // PSI_CFG_USE_NRT_CACHE_SYNC
	
	p_setup->iniDPB.BOARD_SDRAM_BaseAddr       = (LSA_UINT32)edd_ptr->sdram.base_ptr;

    #if defined(PSI_CFG_EDDI_CFG_SOC)
	p_setup->iniDPB.ERTECx00_SCRB_BaseAddr     = 0; // Not used in SOC
	p_setup->iniDPB.PAEA_BaseAdr               = (LSA_UINT32)edd_ptr->paea.base_ptr;
    #else
	p_setup->iniDPB.ERTECx00_SCRB_BaseAddr     = (LSA_UINT32)edd_ptr->apb_periph_scrb.base_ptr;
    #endif

    // SII configuration parameters
	p_setup->iniDPB.SII_IrqSelector             = edd_ptr->SII_IrqSelector;		    // EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
	p_setup->iniDPB.SII_IrqNumber               = edd_ptr->SII_IrqNumber;		    // EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
	p_setup->iniDPB.SII_ExtTimerInterval        = edd_ptr->SII_ExtTimerInterval;    // 0, 250, 500, 1000 in us

	// Override the Port mapping for all user ports [1..N]
	// Note: User ports from [1..N], 0 is reserved for auto port ID
	for (port_idx = 0; port_idx < EDD_CFG_MAX_PORT_CNT; port_idx++)
	{
		p_entry = psi_get_hw_port_entry_for_user_port((port_idx+1), p_inp);

		if (p_entry != LSA_NULL) // entry found for IF and UserPortID ?
		{
			p_setup->iniDPB.PortMap.UsrPortID_x_to_HWPort_y[port_idx] = hw_ptr->port_map[port_idx+1].hw_port_id;
		}
		else
		{
			p_setup->iniDPB.PortMap.UsrPortID_x_to_HWPort_y[port_idx] = EDDI_PORT_NOT_CONNECTED;
		}
	}

    // IRTFlex settings for DPB
    p_setup->iniDPB.bSupportIRTflex             = ( PSI_FEATURE_ENABLE == p_inp->irte.support_irt_flex ) ? EDD_FEATURE_ENABLE : EDD_FEATURE_DISABLE;

    // PortDelay Checks for DPB
    p_setup->iniDPB.bDisableMaxPortDelayCheck   = ( PSI_FEATURE_ENABLE == p_inp->irte.disable_max_port_delay_check ) ? EDDI_DPB_DISABLE_MAX_PORTDELAY_CHECK : EDDI_DPB_ENABLE_MAX_PORTDELAY_CHECK;

	// COMP settings
	p_setup->iniCOMP.CRT.SRT.ConsumerFrameIDBaseClass1 = EDD_SRT_FRAMEID_CSRT_START2;
	p_setup->iniCOMP.CRT.SRT.ConsumerFrameIDBaseClass2 = EDD_SRT_FRAMEID_CSRT_START1;

	// NRT IF
	// Note: only A0 and B0 are used
    p_setup->iniCOMP.NRT.u.IF.A__0.MaxRcvFrame_SendToUser   = 4;
    p_setup->iniCOMP.NRT.u.IF.A__0.MaxRcvFrame_ToLookUp     = 10;
    p_setup->iniCOMP.NRT.u.IF.A__0.MaxRcvFragments_ToCopy   = 10;

    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDDMACWDefault    = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_dmacw_default);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDRXDefault       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_rx_default);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXDefault       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_default);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXHighFrag      = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_high_frag);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXLowFrag       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_low_frag);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXMgmtHighFrag  = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mgmthigh_frag);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXMgmtLowFrag   = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mgmtlow_frag);
    p_setup->iniCOMP.NRT.u.IF.A__0.UserMemIDTXMidFrag       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mid_frag); 

	p_setup->iniCOMP.NRT.u.IF.A__0.RxFilterUDP_Broadcast    = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Broadcast ) ?   EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;
	p_setup->iniCOMP.NRT.u.IF.A__0.RxFilterUDP_Unicast      = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Unicast ) ?     EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;

    p_setup->iniCOMP.NRT.u.IF.B__0.MaxRcvFrame_SendToUser   = 4;
	p_setup->iniCOMP.NRT.u.IF.B__0.MaxRcvFrame_ToLookUp     = 10;
    p_setup->iniCOMP.NRT.u.IF.B__0.MaxRcvFragments_ToCopy   = 10;
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDDMACWDefault    = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_dmacw_default);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDRXDefault       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_rx_default);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXDefault       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_default);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXHighFrag      = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_high_frag);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXLowFrag       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_low_frag);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXMgmtHighFrag  = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mgmthigh_frag);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXMgmtLowFrag   = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mgmtlow_frag);
    p_setup->iniCOMP.NRT.u.IF.B__0.UserMemIDTXMidFrag       = &(p_hd->hd_hw.edd.eddi.user_mem_fcts_tx_mid_frag); 

	p_setup->iniCOMP.NRT.u.IF.B__0.RxFilterUDP_Broadcast    = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Broadcast ) ?   EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;
	p_setup->iniCOMP.NRT.u.IF.B__0.RxFilterUDP_Unicast      = ( PSI_FEATURE_ENABLE == if_ptr->nrt.RxFilterUDP_Unicast ) ?    EDD_SYS_UDP_WHITELIST_FILTER_ON : EDD_SYS_UDP_WHITELIST_FILTER_OFF;
	
    p_setup->iniCOMP.NRT.bFeedInLoadLimitationActive        = ( PSI_FEATURE_ENABLE == p_inp->fill_active ) ?                   EDD_SYS_FILL_ACTIVE : EDD_SYS_FILL_INACTIVE;
    p_setup->iniCOMP.NRT.bIO_Configured                     = ( PSI_FEATURE_ENABLE == p_inp->io_configured ) ?                EDD_SYS_IO_CONFIGURED_ON : EDD_SYS_IO_CONFIGURED_OFF;

	// ERTEC 200 Workaround for internal PHY : PHY doesn´t recognize cable defect, if only one line is broken
	p_setup->iniCOMP.PhyBugfix.MysticCom.bActivate               = EDD_FEATURE_ENABLE;   // general Activation
	p_setup->iniCOMP.PhyBugfix.MysticCom.bReplaceDefaultSettings = EDD_FEATURE_DISABLE;  // internal default settings

	// ini CYCLE-Parameter (details see eddo_bsp_edd_ini.c)
	// -------------------------------------------------------------------------------------
	p_setup->iniCOMP.CycleComp.ClkCy_Period      = 3125;
    p_setup->iniCOMP.CycleComp.AplClk_Divider    = 32; //TODO: get via service GET_PARAMS
	p_setup->iniCOMP.CycleComp.ClkCy_Int_Divider = p_setup->iniCOMP.CycleComp.AplClk_Divider;

	p_setup->iniCOMP.CycleComp.ClkCy_Delay     = 0x00000000;
	p_setup->iniCOMP.CycleComp.ClkCy_Int_Delay = 0x00000000;

	p_setup->iniCOMP.CycleComp.CompVal1Int = EDD_FEATURE_DISABLE;
	p_setup->iniCOMP.CycleComp.CompVal2Int = EDD_FEATURE_DISABLE;
	p_setup->iniCOMP.CycleComp.CompVal3Int = EDD_FEATURE_DISABLE;
	p_setup->iniCOMP.CycleComp.CompVal4Int = EDD_FEATURE_DISABLE;
	p_setup->iniCOMP.CycleComp.CompVal5Int = EDD_FEATURE_DISABLE;

	p_setup->iniCOMP.CycleComp.CompVal1 = 0x00000000;
	p_setup->iniCOMP.CycleComp.CompVal2 = 0x00000000;
	p_setup->iniCOMP.CycleComp.CompVal3 = 0x00000000;
	p_setup->iniCOMP.CycleComp.CompVal4 = 0x00000000;
	p_setup->iniCOMP.CycleComp.CompVal5 = 0x00000000;

	if (edd_ptr->has_ext_pll) // EXT PLL ?
	{
		// enable EXT PLL OUT
		p_setup->iniCOMP.CycleComp.PDControlPllMode = EDDI_CYCLE_INI_PLL_MODE_XPLL_EXT_OUT;
	}
	else
	{
		// turn of PLL support
		p_setup->iniCOMP.CycleComp.PDControlPllMode = EDDI_CYCLE_INI_PLL_MODE_NOTUSED;
	}

	// DSB settings
	p_setup->iniDSB.GlobPara.MaxMRP_Instances           = if_ptr->mrp.max_instances;
	p_setup->iniDSB.GlobPara.MRPSupportedRole           = if_ptr->mrp.supported_role;
	p_setup->iniDSB.GlobPara.MRPSupportedMultipleRole   = if_ptr->mrp.supported_multiple_role;
	p_setup->iniDSB.GlobPara.MRPDefaultRoleInstance0    = if_ptr->mrp.default_role_instance0;
    // MRP interconnect
    p_setup->iniDSB.GlobPara.MaxMRPInterconn_Instances  = if_ptr->mrp.max_mrp_interconn_instances;
    p_setup->iniDSB.GlobPara.SupportedMRPInterconnRole  = if_ptr->mrp.supported_mrp_interconn_role;

    // IF MAC (for EDDx from 0..N-1)
	PSI_MEMCPY(set_mac, hw_ptr->if_mac, sizeof(PSI_MAC_TYPE));

	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[0] = set_mac[0];
	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[1] = set_mac[1];
	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[2] = set_mac[2];
	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[3] = set_mac[3];
	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[4] = set_mac[4];
	p_setup->iniDSB.GlobPara.xRT.MACAddressSrc.MacAdr[5] = set_mac[5];

	// Override the port param setting
	for (port_idx = 0; port_idx < EDD_CFG_MAX_PORT_CNT; port_idx++)
	{
        //CMT!! p_setup->iniDSB.GlobPara.PortParams[port_idx].PhyPowerOff = LSA_TRUE;
		// Note: User ports from [1..N], 0 is reserved for auto port ID
		p_entry = psi_get_hw_port_entry_for_user_port((port_idx+1), p_inp);

		if ( p_entry != LSA_NULL ) // entry found for IF and UserPortID
		{
			PSI_MEMCPY(set_mac, hw_ptr->port_mac[port_idx], sizeof(PSI_MAC_TYPE));

			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[0] = set_mac[0];
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[1] = set_mac[1];
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[2] = set_mac[2];
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[3] = set_mac[3];
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[4] = set_mac[4];
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MACAddress.MacAdr[5] = set_mac[5];

			p_setup->iniDSB.GlobPara.PortParams[port_idx].PhyAdr         = hw_ptr->port_map[port_idx+1].hw_phy_nr;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].PhyTransceiver = EDDI_PHY_TRANSCEIVER_NOT_DEFINED;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MRPRingPort    = (LSA_UINT8)p_entry->mrp_ringport_type;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].IsWireless     = EDD_PORT_IS_NOT_WIRELESS;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].IsMDIX         = EDD_PORT_MDIX_DISABLED;
		}
		else
		{
			p_setup->iniDSB.GlobPara.PortParams[port_idx].PhyTransceiver = EDDI_PHY_TRANSCEIVER_NOT_DEFINED;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].PhyAdr         = EDDI_PhyAdr_NOT_CONNECTED;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].MRPRingPort    = EDD_MRP_NO_RING_PORT;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].IsWireless     = EDD_PORT_IS_NOT_WIRELESS;
			p_setup->iniDSB.GlobPara.PortParams[port_idx].IsMDIX         = EDD_PORT_MDIX_DISABLED;
		}
        p_setup->iniDSB.GlobPara.PortParams[port_idx].bBC5221_MCModeSet  = (edd_ptr->BC5221_MCModeSet)?EDDI_PORT_BC5221_MEDIACONV_STRAPPED_LOW:EDDI_PORT_BC5221_MEDIACONV_STRAPPED_HIGH;
        // MRP interconnect
        p_setup->iniDSB.GlobPara.PortParams[port_idx].SupportsMRPInterconnPortConfig = p_inp->pnio_if.mrp.supports_mrp_interconn_port_config[port_idx];
	}

	// SWIPara Device setup block not used (BC MAC internal set by EDDI)
	p_setup->iniDSB.SWIPara.Sys_StatFDB_CntEntry  = 0;
	p_setup->iniDSB.SWIPara.pSys_StatFDB_CntEntry = 0;

    #if !defined(PSI_CFG_EDDI_CFG_SOC)
	// Set calculated size of PI (result of EDDI ConvenienceFunctions)
	p_hd->hd_hw.pi_mem.size = p_setup->iniDPB.KRam.offset_ProcessImageEnd;
    #endif
}

#endif //((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

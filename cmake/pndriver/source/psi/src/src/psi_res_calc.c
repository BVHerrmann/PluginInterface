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
/*  F i l e               &F: psi_res_calc.c                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI resource calculation and storage based on input data  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   6
#define PSI_MODULE_ID       6 /* PSI_MODULE_ID_PSI_RES_CALC */

#include "psi_int.h"

#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

//lint --e(537) Repeated include file - The header is necessary here, but other C-Files may have already included
#include "cm_inc.h"   // CM header info (needed for CM defines)

#if (PSI_CFG_USE_EDDI == 1)
// reserve static memory for gshared SRD RAM used in EDDI open device and IOH 
// Note: need allocation of shared mem from upper side for LD/HD splitted
extern EDDI_GSHAREDMEM_TYPE psi_eddi_srd_shmem[PSI_CFG_MAX_IF_CNT];
#endif

#if (PSI_CFG_USE_EDDP == 1)
// reserve static memory for gshared SRD RAM used in EDDP open device and IOH 
// Note: need allocation of shared mem from upper side for LD/HD splitted
extern EDDP_GSHAREDMEM_TYPE psi_eddp_srd_shmem[PSI_CFG_MAX_IF_CNT];
#endif

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*----------------------------------------------------------------------------*/
// Storage type for a channel PNIO IF entry (global or IF)
typedef struct psi_pnio_if_entry_tag {
	LSA_BOOL                 is_used;   // PNIO IF already in use yes/no
	LSA_UINT16               hd_nr;     // HD number [1..N]
	PSI_DETAIL_STORE_TYPE    details;   // Detail storage
} PSI_PNIO_IF_ENTRY_TYPE, *PSI_PNIO_IF_ENTRY_PTR_TYPE;

/*----------------------------------------------------------------------------*/

typedef struct psi_pnio_if_store_tag { // PSI PNIO storage instance
	LSA_BOOL                    init_done;                          // init done yes/no
	PSI_PNIO_IF_ENTRY_PTR_TYPE  entry_ptr[PSI_MAX_PNIO_IF_NR+1];    // PNIO IF entry
} PSI_PNIO_IF_INST_TYPE;

/*----------------------------------------------------------------------------*/

static PSI_PNIO_IF_INST_TYPE psi_pnio_store = { LSA_FALSE, {LSA_NULL} };

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_init_details_acp(
	PSI_RT_DETAIL_PTR_TYPE const rt_detail_ptr)
{
	PSI_ASSERT(rt_detail_ptr != 0);

	rt_detail_ptr->nr_of_consumers_class1_unicast   = 0;
	rt_detail_ptr->nr_of_consumers_class2_unicast   = 0; 

	rt_detail_ptr->alarm_ref_base   = 0;
	rt_detail_ptr->alarm_nr_of_ref  = 0;
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmcl(
	PSI_RT_DETAIL_PTR_TYPE const rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE  const p_hd_in)
{
	PSI_HD_IF_INPUT_PTR_TYPE p_if;

	PSI_ASSERT(rt_detail_ptr != LSA_NULL);
	PSI_ASSERT(p_hd_in != LSA_NULL);

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&(p_hd_in->pnio_if);

	// Prepare the ACP settings based on used EDDx
	switch (p_hd_in->edd_type)
	{
	    case LSA_COMP_ID_EDDP:
	    case LSA_COMP_ID_EDDI:
		{
			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = p_if->ioc.nr_of_rt_devices + p_if->ioc.nr_of_irt_devices;
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			LSA_UINT16 rt_cons     = p_if->ioc.nr_of_rt_devices - p_if->ioc.nr_of_qv_consumer;
			LSA_UINT16 rt_prov     = p_if->ioc.nr_of_rt_devices - p_if->ioc.nr_of_qv_provider;

			rt_detail_ptr->nr_of_consumers_class1_unicast   = rt_cons;

			rt_detail_ptr->nr_of_consumers_class2_unicast   = rt_cons;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = (rt_prov + rt_prov) + p_if->ioc.nr_of_irt_devices;
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_set_details_acp_cmcl(): invalid edd_type(%u/%#x)",
				p_hd_in->edd_type, p_hd_in->edd_type );
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmsv(
	PSI_RT_DETAIL_PTR_TYPE const rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE  const p_hd_in)
{
	LSA_UINT16               nr_of_rt_uc;
	LSA_UINT16               nr_of_rt_mc;
	PSI_HD_IF_INPUT_PTR_TYPE p_if;

	PSI_ASSERT(rt_detail_ptr != LSA_NULL);
	PSI_ASSERT(p_hd_in != LSA_NULL);

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&(p_hd_in->pnio_if);

	// Prepare the ACP settings based on used EDDx
	// configure for each RT/IRT AR one IO-CR and M-CR
	// Note: M-CR only set if IOD-MC feature is configured
	nr_of_rt_uc = (LSA_UINT16)(p_if->iod.nr_of_instances * p_if->iod.iod_max_ar_IOC);
	nr_of_rt_mc = (LSA_UINT16)(p_if->iod.nr_of_instances * p_if->iod.iod_max_ar_IOC);

	switch (p_hd_in->edd_type)
	{
	    case LSA_COMP_ID_EDDP:
	    case LSA_COMP_ID_EDDI:
		{
			// Combined usage of RTC 1/2
			rt_detail_ptr->alarm_ref_base  = 0x1000;
			rt_detail_ptr->alarm_nr_of_ref = nr_of_rt_uc + nr_of_rt_mc;
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			rt_detail_ptr->nr_of_consumers_class1_unicast   = nr_of_rt_uc;

			rt_detail_ptr->nr_of_consumers_class2_unicast   = nr_of_rt_uc;

			rt_detail_ptr->alarm_ref_base  = 0x1000;
			rt_detail_ptr->alarm_nr_of_ref = (nr_of_rt_uc + nr_of_rt_mc + nr_of_rt_uc + nr_of_rt_mc);
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_set_details_acp_cmsv(): invalid edd_type(%u/%#x)",
                p_hd_in->edd_type, p_hd_in->edd_type );
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_set_details_acp_cmmc(
	PSI_RT_DETAIL_PTR_TYPE const rt_detail_ptr,
	PSI_HD_INPUT_PTR_TYPE  const p_hd_in)
{
	PSI_ASSERT(rt_detail_ptr != LSA_NULL);
	PSI_ASSERT(p_hd_in != LSA_NULL);

	// Prepare the ACP settings based on used EDDx
	switch (p_hd_in->edd_type)
	{
	    case LSA_COMP_ID_EDDP:
	    case LSA_COMP_ID_EDDI:
		{
			// Combined usage of RTC 1/2
			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = 0;
		}
		break;

        case LSA_COMP_ID_EDDS:
		{
			// Not combined usage of RTC 1/2
			rt_detail_ptr->nr_of_consumers_class1_unicast   = 0;

			// SRT: RTclass2
			rt_detail_ptr->nr_of_consumers_class2_unicast   = 0;

			rt_detail_ptr->alarm_ref_base  = 0;
			rt_detail_ptr->alarm_nr_of_ref = 0;
		}
		break;

        default:
		{
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_set_details_acp_cmmc(): invalid edd_type(%u/%#x)",
				p_hd_in->edd_type, p_hd_in->edd_type );
			PSI_FATAL( 0 );
		}
	}

    //lint --e(818) Pointer parameter 'p_hd_in' could be declared as pointing to const - we are unable to change the API
}

/*----------------------------------------------------------------------------*/
static LSA_VOID psi_init_if_details(
    PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF)
{
	PSI_ASSERT(pDetailsIF != LSA_NULL);

	pDetailsIF->is_created      = LSA_FALSE;
	pDetailsIF->hSysDev         = 0;
	pDetailsIF->hPoolDev        = 0;
	pDetailsIF->hPoolNrtTx	    = 0;
	pDetailsIF->hPoolNrtRx	    = 0;
	pDetailsIF->pnio_if_nr      = 0;
	pDetailsIF->edd_comp_id     = 0;
	pDetailsIF->trace_idx       = 0;
	pDetailsIF->nr_of_ports     = 0;

	psi_init_details_acp(&pDetailsIF->acp.cm_cl);
	psi_init_details_acp(&pDetailsIF->acp.cm_sv);
	psi_init_details_acp(&pDetailsIF->acp.cm_mc);

	pDetailsIF->cm.cl.device_count              = 0;
	pDetailsIF->cm.cl.max_alarm_data_length     = 0;
	pDetailsIF->cm.cl.max_record_length         = 0;

	pDetailsIF->cm.sv.device_count              = 0;
	pDetailsIF->cm.sv.max_ar                    = 0;
	pDetailsIF->cm.sv.max_record_length         = 0;

	pDetailsIF->oha.allow_non_pnio_mautypes     = LSA_FALSE;
    pDetailsIF->oha.allow_overlapping_subnet    = LSA_FALSE;
	pDetailsIF->oha.check_ip_enabled            = LSA_FALSE;
	pDetailsIF->oha.nos_allow_upper_cases       = LSA_FALSE;
	pDetailsIF->oha.recv_resource_count_dcp     = 0;

	pDetailsIF->tcip.send_resources             = 0;
	pDetailsIF->tcip.arp_rcv_resources          = 0;
	pDetailsIF->tcip.icmp_rcv_resources         = 0;
	pDetailsIF->tcip.tcp_rcv_resources          = 0;
	pDetailsIF->tcip.udp_rcv_resources          = 0;
	pDetailsIF->tcip.multicast_support_on       = LSA_FALSE;

	pDetailsIF->hsa.bHsyncAppl	                = LSA_FALSE;
}

/*---------------------------------------------------------------------------*/
/*  Initialization / cleanup                                                 */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_init(LSA_VOID)
{
	LSA_UINT16 idx;

	/* Init the instance vars */
	PSI_ASSERT(!psi_pnio_store.init_done);

	/* Allocate storage for PNIO IF details store (global and IF specific) */
	for (idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++)
	{
		psi_pnio_store.entry_ptr[idx] = (PSI_PNIO_IF_ENTRY_PTR_TYPE)psi_alloc_local_mem(sizeof(PSI_PNIO_IF_ENTRY_TYPE));
		PSI_ASSERT(psi_pnio_store.entry_ptr[idx] != LSA_NULL);

		psi_pnio_store.entry_ptr[idx]->is_used = LSA_FALSE;
		psi_pnio_store.entry_ptr[idx]->hd_nr   = 0;

		psi_init_if_details(&psi_pnio_store.entry_ptr[idx]->details);
	}

	psi_pnio_store.init_done = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_undo_init(LSA_VOID)
{
	LSA_UINT16 idx;

	/* Undo INIT the instance vars */
	PSI_ASSERT(psi_pnio_store.init_done);

	/* Free storage for PNIO IF details store (global and IF specific) */
	for (idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++)
	{
        PSI_ASSERT(psi_pnio_store.entry_ptr[idx] != LSA_NULL);
		PSI_ASSERT(!psi_pnio_store.entry_ptr[idx]->is_used);

		psi_free_local_mem(psi_pnio_store.entry_ptr[idx]);
	}

	psi_pnio_store.init_done = LSA_FALSE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_delete(LSA_VOID)
{
    LSA_UINT16 idx;

    PSI_ASSERT(psi_pnio_store.init_done);
  
    for (idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++)
    {
        PSI_ASSERT(psi_pnio_store.entry_ptr[idx] != LSA_NULL);

        psi_pnio_store.entry_ptr[idx]->is_used = LSA_FALSE;
        psi_pnio_store.entry_ptr[idx]->hd_nr   = 0;

        psi_init_if_details(&psi_pnio_store.entry_ptr[idx]->details);
    }
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_set_global_details(
    PSI_SYS_HANDLE const hSysDev)
{
	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF;

	PSI_ASSERT(psi_pnio_store.entry_ptr[0] != LSA_NULL);

	pDetailsIF = &(psi_pnio_store.entry_ptr[0]->details);

	// ------------ Global details ------------
	pDetailsIF->hSysDev      = hSysDev;
	pDetailsIF->pnio_if_nr   = (LSA_UINT8)PSI_GLOBAL_PNIO_IF_NR;
	pDetailsIF->nr_of_ports  = 0;  // don't care
	pDetailsIF->hPoolDev     = 0;  // don't care
	pDetailsIF->hPoolNrtTx   = 0;  // don't care
	pDetailsIF->hPoolNrtRx   = 0;  // don't care
	pDetailsIF->edd_comp_id  = 0;  // don't care
	pDetailsIF->trace_idx    = 0;  // don't care
	pDetailsIF->is_created   = LSA_TRUE;
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_set_if_details(
	PSI_SYS_HANDLE        const hSysDev,
	LSA_INT               const dev_pool_handle,
	LSA_INT               const nrt_tx_pool_handle,
	LSA_INT               const nrt_rx_pool_handle,
	PSI_HD_INPUT_PTR_TYPE const p_hd_in)
{
	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF;
	PSI_HD_IF_INPUT_PTR_TYPE  p_if;

	PSI_ASSERT(p_hd_in != LSA_NULL);

	p_if = (PSI_HD_IF_INPUT_PTR_TYPE)&(p_hd_in->pnio_if);
    //lint --e(774) Boolean within 'if' always evaluates to False - check the input parameter 'p_hd_in->pnio_if'
    PSI_ASSERT(p_if != LSA_NULL);

	pDetailsIF = psi_get_detail_store(p_hd_in->hd_id);

	// Prepares the settings for details based on input values for this HD-IF
	// Note: the settings are used during open channels for GET_PATH_INFO_xx

	// ------------ Global details ------------
	pDetailsIF->hSysDev      = hSysDev;
	pDetailsIF->hPoolDev     = dev_pool_handle;
	pDetailsIF->hPoolNrtTx   = nrt_tx_pool_handle;
	pDetailsIF->hPoolNrtRx   = nrt_rx_pool_handle;
	pDetailsIF->edd_comp_id  = p_hd_in->edd_type;
	pDetailsIF->pnio_if_nr   = (LSA_UINT8)(p_if->edd_if_id);
	pDetailsIF->trace_idx    = p_if->trace_idx;
	pDetailsIF->nr_of_ports  = p_hd_in->nr_of_ports;
	//May be overwritten: pDetailsIF->hd_runs_on_level_ld = p_hd_in->hd_runs_on_level_ld;

	// ------------ OHA details ------------
	pDetailsIF->oha.recv_resource_count_dcp  = (DCP_NUM_SERVER_IDENTIFY_RESOURCES +  // thats the server MIN value
		                                       DCP_NUM_SERVER_SET_RESOURCES +
		                                       DCP_NUM_SERVER_GET_RESOURCES);
	pDetailsIF->oha.dcp_indication_filter    = p_if->oha.dcp_indication_filter;
	pDetailsIF->oha.rema_station_format      = p_if->oha.rema_station_format;
	pDetailsIF->oha.check_ip_enabled         = (PSI_FEATURE_ENABLE == p_if->oha.check_ip_enabled)        ? LSA_TRUE : LSA_FALSE;
	pDetailsIF->oha.nos_allow_upper_cases    = (PSI_FEATURE_ENABLE == p_if->oha.nos_allow_upper_cases)   ? LSA_TRUE : LSA_FALSE;
	pDetailsIF->oha.allow_non_pnio_mautypes  = (PSI_FEATURE_ENABLE == p_hd_in->allow_none_pnio_mautypes) ? LSA_TRUE : LSA_FALSE;
    pDetailsIF->oha.allow_overlapping_subnet = (PSI_FEATURE_ENABLE == p_hd_in->allow_overlapping_subnet) ? LSA_TRUE : LSA_FALSE;

	// ------------ CM-CL/CM-MC details ------------
	if (PSI_USE_SETTING_YES == p_if->ioc.use_setting) // IOC/IOM configured?
	{
		pDetailsIF->cm.cl.device_count          = p_if->ioc.nr_of_rt_devices;
		pDetailsIF->cm.cl.max_alarm_data_length = p_if->ioc.max_alarm_data_length;

		// max_record_length is a summary of configured max_record length(=data) + protocol header length
		// note: means length of PROFINETIOServiceReqPDU but limited to 64K (see IEC)
		if (p_if->ioc.max_record_length > 0xFFFF)  // > 64K
		{
			pDetailsIF->cm.cl.max_record_length = CM_RECORD_OFFSET + 0xFFFF;
		}
		else
		{
			pDetailsIF->cm.cl.max_record_length = CM_RECORD_OFFSET + p_if->ioc.max_record_length;
		}

		// ------------ ACP details for CM-CL/CM-MC ------------
		psi_set_details_acp_cmcl(&pDetailsIF->acp.cm_cl, p_hd_in);
		psi_set_details_acp_cmmc(&pDetailsIF->acp.cm_mc, p_hd_in);
	}
	else
	{
		// Set to unused
		pDetailsIF->cm.cl.device_count           = 0;
		pDetailsIF->cm.cl.max_alarm_data_length  = 0;
		pDetailsIF->cm.cl.max_record_length      = 0;

		psi_init_details_acp(&pDetailsIF->acp.cm_cl);
		psi_init_details_acp(&pDetailsIF->acp.cm_mc);
	}

	// ------------ CM-SV details ------------
	if (PSI_USE_SETTING_YES == p_if->iod.use_setting) // IOD configured?
	{
		pDetailsIF->cm.sv.device_count   = p_if->iod.nr_of_instances;
		pDetailsIF->cm.sv.max_ar         = p_if->iod.iod_max_ar_IOC; 

		// max_record_length is a summary of configured max_record length(=data) + protocol header length
		// note: means length of PROFINETIOServiceReqPDU but limited to 64K (see IEC)
		if (p_if->iod.max_record_length > 0xFFFF) // > 64K
		{
			pDetailsIF->cm.sv.max_record_length = CM_RECORD_OFFSET + 0xFFFF;
		}
		else
		{
			pDetailsIF->cm.sv.max_record_length = CM_RECORD_OFFSET + p_if->iod.max_record_length;
		}

		// ------------ ACP details for CM-SV ------------
		psi_set_details_acp_cmsv(&pDetailsIF->acp.cm_sv, p_hd_in);
	}
	else
	{
		// Set to unused
		pDetailsIF->cm.sv.device_count          = 0;
		pDetailsIF->cm.sv.max_ar                = 0;
		pDetailsIF->cm.sv.max_record_length     = 0;

		psi_init_details_acp(&pDetailsIF->acp.cm_sv);
	}

	// ------------ TCIP details ------------
	if (PSI_USE_SETTING_YES == p_if->ip.use_setting ) // IP for TCIP configured?
	{
		pDetailsIF->tcip.send_resources         = p_if->ip.nr_of_send;
		pDetailsIF->tcip.arp_rcv_resources      = p_if->ip.nr_of_arp;
		pDetailsIF->tcip.icmp_rcv_resources     = p_if->ip.nr_of_icmp;
		pDetailsIF->tcip.udp_rcv_resources      = p_if->ip.nr_of_udp;
		pDetailsIF->tcip.tcp_rcv_resources      = p_if->ip.nr_of_tcp;
		pDetailsIF->tcip.multicast_support_on   = ( PSI_FEATURE_ENABLE == p_if->ip.multicast_support_on ) ? LSA_TRUE : LSA_FALSE;
	}
	else
	{
		pDetailsIF->tcip.send_resources         = 0;
		pDetailsIF->tcip.arp_rcv_resources      = 0;
		pDetailsIF->tcip.icmp_rcv_resources     = 0;
		pDetailsIF->tcip.udp_rcv_resources      = 0;
		pDetailsIF->tcip.tcp_rcv_resources      = 0;
		pDetailsIF->tcip.multicast_support_on   = LSA_FALSE;
	}

	// ------------ HSA details ------------
	if (   (PSI_FEATURE_ENABLE == p_hd_in->additional_forwarding_rules_supported)
	    && (PSI_FEATURE_ENABLE == p_hd_in->application_exist))
	{
		pDetailsIF->hsa.bHsyncAppl			    = LSA_TRUE;
	}
	else
	{
		pDetailsIF->hsa.bHsyncAppl			    = LSA_FALSE;
	}

	// ------------ SOCKAPP details ------------
	if (PSI_FEATURE_ENABLE == p_hd_in->cluster_IP_support)
	{
		pDetailsIF->sockapp.bClusterIP			= LSA_TRUE;
	}
	else
	{
		pDetailsIF->sockapp.bClusterIP			= LSA_FALSE;
	}

    // ------------- IOH details -----------
    #if (PSI_CFG_USE_IOH == 1)
    {
        PSI_HD_PARAM_TYPE hw_param;
        LSA_UINT16        ret;

        PSI_MEMSET(&hw_param, 0, sizeof(hw_param));
        PSI_LD_GET_HD_PARAM(&ret, p_hd_in->hd_id, &hw_param);

        if (ret != PSI_OK)
        {
            PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_res_calc_set_if_details(): getting HD params failed, hd_nr(%u) ret(%u/%#x)",
                p_hd_in->hd_id, ret, ret);
            PSI_FATAL(0);
        }

        #if (PSI_CFG_USE_EDDI == 1)
        if (p_hd_in->edd_type == LSA_COMP_ID_EDDI)
        {
            if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_YES)
            {
                // store shared mem in LD_HD store, only works when HD runs on LD level
                pDetailsIF->shared_ram_base_ptr = &(psi_eddi_srd_shmem[p_hd_in->hd_id - 1]);
            }
            else
            {
                pDetailsIF->shared_ram_base_ptr = hw_param.edd.eddi.g_shared_ram_base_ptr;
            }
            PSI_ASSERT(LSA_NULL != pDetailsIF->shared_ram_base_ptr);
        }
        #endif //PSI_CFG_USE_EDDI
        #if (PSI_CFG_USE_EDDP == 1)
        if (p_hd_in->edd_type == LSA_COMP_ID_EDDP)
        {
            if (p_hd_in->hd_runs_on_level_ld == PSI_HD_RUNS_ON_LEVEL_LD_YES)
            {
                // store shared mem in LD_HD store, only works when HD runs on LD level
                pDetailsIF->shared_ram_base_ptr = &psi_eddp_srd_shmem[p_hd_in->hd_id - 1];
            }
            else
            {
                pDetailsIF->shared_ram_base_ptr = hw_param.edd.eddp.g_shared_ram_base_ptr;
            }
            PSI_ASSERT(LSA_NULL != pDetailsIF->shared_ram_base_ptr);
        }
        #endif //PSI_CFG_USE_EDDP
    }
    #endif //PSI_CFG_USE_IOH

	pDetailsIF->is_created = LSA_TRUE;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_init_hd_outputs(
    PSI_HD_INPUT_PTR_TYPE  const p_hd_in,
	PSI_HD_STORE_PTR_TYPE  const p_hd,
    PSI_HD_OUTPUT_PTR_TYPE const p_hd_out)
{
	LSA_UINT32 port_idx;

	PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT(p_hd_out != LSA_NULL);
	PSI_ASSERT(p_hd != LSA_NULL);

    // init HD_OUTPUT_TYPE
    p_hd_out->nr_of_hw_ports    = (LSA_UINT8)p_hd_in->nr_of_ports;
    p_hd_out->hw_rev_nr         = (LSA_UINT8)p_hd_in->rev_nr;
    p_hd_out->giga_bit_support  = PSI_HD_GIGA_BIT_SUPPORT_DISABLE;

    #if (PSI_CFG_USE_EDDP == 1)
    if (p_hd_in->pnip.gigabit_support)
    {
        p_hd_out->giga_bit_support = PSI_HD_GIGA_BIT_SUPPORT_ENABLE;
    }
    #endif
    #if (PSI_CFG_USE_EDDS == 1)
    if (PSI_FEATURE_ENABLE == p_hd_in->stdmac.gigabit_support )
    {
        p_hd_out->giga_bit_support = PSI_HD_GIGA_BIT_SUPPORT_ENABLE;
    }
    #endif

    // delete hw-port array
    PSI_MEMSET( &p_hd_out->hw_port[0], 0, (PSI_CFG_MAX_PORT_CNT * (sizeof(PSI_HD_PORT_OUTPUT_TYPE))) );
    // and init it with unknown values, valid values will be set in psi_eddx_device_open_done()
    for ( port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++ )
    {
        p_hd_out->hw_port[port_idx].media_type      = PSI_MEDIA_TYPE_UNKNOWN;
        p_hd_out->hw_port[port_idx].fiberoptic_type = PSI_FIBEROPTIC_NO_FO;
        p_hd_out->hw_port[port_idx].phy_type        = PSI_PHY_TYPE_UNKNOWN;
        p_hd_out->hw_port[port_idx].xmii_type       = PSI_PORT_MII_UNKNOWN;
        p_hd_out->hw_port[port_idx].edd_asic_type   = PSI_ASIC_TYPE_UNKNOWN;
        p_hd_out->hw_port[port_idx].isRingPort      = PSI_PORT_NO_DEFAULT_RING_PORT;
    }

    #if (PSI_DEBUG == 0)
    // suppress compiler warnings
    LSA_UNUSED_ARG(p_hd);
    #endif

    //lint --e(818) Pointer parameter 'p_hd_in' could be declared as pointing to const - we are unable to change the API
    //lint --e(818) Pointer parameter 'p_hd' could be declared as pointing to const - we are unable to change the API
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_res_calc_set_if_outputs(
	PSI_HD_INPUT_PTR_TYPE  const p_hd_in,
    PSI_HD_OUTPUT_PTR_TYPE const p_hd_out)
{
	PSI_HD_IF_INPUT_PTR_TYPE  p_if_inp;
	PSI_HD_IF_OUTPUT_PTR_TYPE p_if_out;

	PSI_ASSERT(p_hd_in != LSA_NULL);
	PSI_ASSERT(p_hd_out != LSA_NULL);

	/* prepare the outputs for this IF */
	/* Note: used by an application for the CM Create services */
	p_if_inp = (PSI_HD_IF_INPUT_PTR_TYPE)&(p_hd_in->pnio_if);
	p_if_out = (PSI_HD_IF_OUTPUT_PTR_TYPE)&(p_hd_out->pnio_if);

	p_if_out->nr_of_usr_ports = p_hd_in->nr_of_ports;

	if (PSI_USE_SETTING_YES == p_if_inp->ioc.use_setting) // IOC/IOM configured?
	{
		p_if_out->ioc_max_devices = p_if_inp->ioc.nr_of_rt_devices;
		p_if_out->iom_max_devices = p_if_inp->ioc.nr_of_qv_consumer + p_if_inp->ioc.nr_of_qv_provider;
	}
	else
	{
		p_if_out->ioc_max_devices = 0;
		p_if_out->iom_max_devices = 0;
	}

	if (PSI_USE_SETTING_YES == p_if_inp->iod.use_setting) // IOD configured?
	{
		p_if_out->iod_max_devices   = p_if_inp->iod.nr_of_instances;
		p_if_out->iod_max_ar_DAC    = p_if_inp->iod.iod_max_ar_DAC;
		p_if_out->iod_max_ar_IOC    = p_if_inp->iod.iod_max_ar_IOC;
	}
	else
	{
		p_if_out->iod_max_devices   = 0;
		p_if_out->iod_max_ar_DAC    = 0;
		p_if_out->iod_max_ar_IOC    = 0;
	}
}

/*---------------------------------------------------------------------------*/
PSI_DETAIL_STORE_PTR_TYPE psi_get_detail_store(
    LSA_UINT16 const hd_nr)
{
	LSA_UINT16                idx;
	PSI_DETAIL_STORE_PTR_TYPE pDetailsIF = LSA_NULL;

    PSI_ASSERT(hd_nr <= PSI_CFG_MAX_IF_CNT);

	// lookup in PNIO storage for matching PNIO IF entry [0..16]
	for (idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++)
	{
		PSI_ASSERT(psi_pnio_store.entry_ptr[idx] != LSA_NULL);

		if (   (psi_pnio_store.entry_ptr[idx]->is_used)
            && (psi_pnio_store.entry_ptr[idx]->hd_nr == hd_nr))
		{
			pDetailsIF = &psi_pnio_store.entry_ptr[idx]->details;
			break;
		}
	}

	PSI_ASSERT(pDetailsIF);
	PSI_ASSERT(idx == pDetailsIF->pnio_if_nr);

	return pDetailsIF;
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_add_detail_store(
    LSA_UINT16            const pnio_if_id,
    PSI_HD_INPUT_PTR_TYPE const p_hd_in)
{
    LSA_UINT16 hd_id               = 0;
    LSA_UINT8  hd_runs_on_level_ld = PSI_HD_RUNS_ON_LEVEL_LD_YES;

	PSI_ASSERT(pnio_if_id <= PSI_MAX_PNIO_IF_NR);

    // get specific parts, if hd_in parameters are provided
    if (p_hd_in != LSA_NULL)
    {
        hd_id               = p_hd_in->hd_id;
        hd_runs_on_level_ld = p_hd_in->hd_runs_on_level_ld;
    }

	if (psi_pnio_store.entry_ptr[pnio_if_id]->is_used)
	{
        // already in use
		PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_add_detail_store(): pnio_if_id(%u) is already in use, check config, hd_nr(%u)",
			pnio_if_id, hd_id);
		PSI_FATAL(0);
	}

	// reserve entry with PNIO IF for HD, and HD IF 
	psi_pnio_store.entry_ptr[pnio_if_id]->is_used                       = LSA_TRUE;
	psi_pnio_store.entry_ptr[pnio_if_id]->hd_nr                         = hd_id;
	psi_pnio_store.entry_ptr[pnio_if_id]->details.pnio_if_nr            = (LSA_UINT8)pnio_if_id;
    #if defined HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT
    //Always put RQBs to HIF
    psi_pnio_store.entry_ptr[pnio_if_id]->details.hd_runs_on_level_ld   = PSI_HD_RUNS_ON_LEVEL_LD_NO;
    #else
    psi_pnio_store.entry_ptr[pnio_if_id]->details.hd_runs_on_level_ld   = hd_runs_on_level_ld;
    #endif

    //lint --e(818) Pointer parameter 'p_hd_in' could be declared as pointing to const - we are unable to change the API
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_remove_detail_store(
    LSA_UINT16 const pnio_if_id)
{
	PSI_ASSERT(pnio_if_id <= PSI_MAX_PNIO_IF_NR);
	PSI_ASSERT(psi_pnio_store.entry_ptr[pnio_if_id]->is_used);

	// reserve entry with PNIO IF for HD, and HD IF 
	psi_pnio_store.entry_ptr[pnio_if_id]->is_used                       = LSA_FALSE;
	psi_pnio_store.entry_ptr[pnio_if_id]->hd_nr                         = 0;
	psi_pnio_store.entry_ptr[pnio_if_id]->details.pnio_if_nr            = 0;
    psi_pnio_store.entry_ptr[pnio_if_id]->details.hd_runs_on_level_ld   = (LSA_UINT8) PSI_HD_RUNS_ON_LEVEL_LD_MAX;
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_get_max_ar_count_all_ifs(LSA_VOID)
{
	LSA_UINT16 idx;
	LSA_UINT32 max_ar = 0;

	// lookup in PNIO storage for matching PNIO IF entry [1..16]
	for ( idx = 0; idx <= PSI_MAX_PNIO_IF_NR; idx++ )
	{
	    PSI_PNIO_IF_ENTRY_PTR_TYPE const pnio_store_entry = psi_pnio_store.entry_ptr[idx];

		if (   (pnio_store_entry->is_used)
            && (pnio_store_entry->hd_nr != 0))
		{
			if (pnio_store_entry->details.cm.sv.max_ar > max_ar)
            {
                max_ar = pnio_store_entry->details.cm.sv.max_ar;
            }
		}
	}

	return max_ar;
}

/*---------------------------------------------------------------------------*/
LSA_UINT8 psi_get_pnio_if_nr(
    LSA_UINT16 const hd_nr)
{
	LSA_UINT8 pnio_if_nr;

	if (hd_nr == 0)
	{
        // global SYS channels
		pnio_if_nr = PSI_GLOBAL_PNIO_IF_NR;
	}
	else
	{
        // if SYS channels
		PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);
		PSI_ASSERT(pDetailsIF->is_created);

		pnio_if_nr = pDetailsIF->pnio_if_nr;
	}

	return pnio_if_nr;
}

/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_get_trace_idx_for_sys_path(
    LSA_SYS_PATH_TYPE const sys_path)
{
	LSA_UINT32       trace_idx;
	LSA_UINT16 const hd_nr      = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16 const pnio_if_nr = psi_get_pnio_if_nr(hd_nr);

	if (pnio_if_nr == PSI_GLOBAL_PNIO_IF_NR)
	{
        // global SYS channel
		trace_idx = 0; /* 0 for global traces */
	}
	else
	{
        // if SYS channels
		PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);
		PSI_ASSERT(pDetailsIF->is_created);

		trace_idx = pDetailsIF->trace_idx;
	}

	return trace_idx;
}

#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

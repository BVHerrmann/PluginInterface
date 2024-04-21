#ifndef PSI_RES_CALC_H              /* ----- reinclude-protection ----- */
#define PSI_RES_CALC_H

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: psi_res_calc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Internal headerfile                                                      */
/*                                                                           */
/*****************************************************************************/

#include "psi_hd.h"

#include "psi_int.h"

/*===========================================================================*/
/*                                 macros                                    */
/*===========================================================================*/

/*===========================================================================*/
/*                                 types                                     */
/*===========================================================================*/
// Storage type for ACP specific RTC consumer/providers settings
typedef struct psi_rt_detail_tag
{
	LSA_UINT16      nr_of_consumers_class1_unicast; 
	LSA_UINT16      nr_of_consumers_class2_unicast; 
	LSA_UINT16      alarm_ref_base; // Alarms
	LSA_UINT16      alarm_nr_of_ref;
} PSI_RT_DETAIL_TYPE, *PSI_RT_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for ACP detail settings
typedef struct psi_detail_acp_tag
{
    PSI_RT_DETAIL_TYPE  cm_sv;  // settings CM SV (IOD)
	PSI_RT_DETAIL_TYPE  cm_cl;  // settings CM CL (IOC)
	PSI_RT_DETAIL_TYPE  cm_mc;  // settings CM_MC (IOM)
} PSI_ACP_DETAIL_TYPE, *PSI_ACP_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for CM detail settings
typedef struct psi_cm_detail_tag
{
	struct psi_cm_cl_store_tag 
	{
        LSA_UINT16  device_count; 
		LSA_UINT16  max_alarm_data_length; /* Sizeof AlarmNotification-PDU */
		LSA_UINT32  max_record_length;
	} cl; 
	struct psi_cm_sv_store_tag 
	{
		LSA_UINT16  device_count;
		LSA_UINT16  max_ar;
		LSA_UINT32  max_record_length;
	} sv; 
} PSI_CM_DETAIL_TYPE, *PSI_CM_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for OHA detail settings
typedef struct psi_oha_detail_tag
{
	LSA_UINT32      dcp_indication_filter;
	LSA_UINT16      rema_station_format;
	LSA_UINT16      recv_resource_count_dcp;
	LSA_BOOL        check_ip_enabled;
	LSA_BOOL        allow_non_pnio_mautypes;
	LSA_BOOL        allow_overlapping_subnet;
	LSA_BOOL        nos_allow_upper_cases;
} PSI_OHA_DETAIL_TYPE, *PSI_OHA_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for TCIP detail settings
typedef struct psi_tcip_detail_tag
{
	LSA_UINT32      send_resources;
	LSA_UINT32      arp_rcv_resources;
	LSA_UINT32      icmp_rcv_resources;
	LSA_UINT32      udp_rcv_resources;
	LSA_UINT32      tcp_rcv_resources;
	LSA_BOOL        multicast_support_on;
} PSI_TCIP_DETAIL_TYPE, *PSI_TCIP_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for HSA detail settings
typedef struct psi_hsa_detail_tag
{
	LSA_BOOL        bHsyncAppl;
} PSI_HSA_DETAIL_TYPE, *PSI_HSA_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Storage type for SOCKAPP detail settings
typedef struct psi_sockapp_detail_tag
{
	LSA_UINT8       bClusterIP;
} PSI_SOCKAPP_DETAIL_TYPE, *PSI_SOCKAPP_DETAIL_PTR_TYPE;

/*---------------------------------------------------------------------------*/
// Setup info for each PNIO IF used for detail ptr
typedef struct psi_detail_store_tag
{
	LSA_BOOL                is_created;             // Settings for PNIO-IF created yes/no
	PSI_SYS_HANDLE          hSysDev;                // hSysDev handle for this HD
	LSA_INT                 hPoolDev;               // DEV pool handle for this HD
	LSA_INT                 hPoolNrtTx;             // NRT TX pool handle for this HD
	LSA_INT                 hPoolNrtRx;             // NRT RX pool handle for this HD
    LSA_VOID_PTR_TYPE       shared_ram_base_ptr;    // IOH: Shared mem base ptr for SRD
	LSA_UINT8               pnio_if_nr;             // PNIO IF number for this HD-IF: [0..16] (0=global, 1-16=PNIO IF) (set by TIA)
	LSA_UINT32              trace_idx;              // Trace Index assigned on open device
	LSA_UINT16              edd_comp_id;            // LSA_COMP_ID of used EDDx
	LSA_UINT16              nr_of_ports;            // nr of used ports for one HD
    LSA_UINT8               hd_runs_on_level_ld;    // 1: HD is on LD level / 0: HD is on separate level from LD
	PSI_ACP_DETAIL_TYPE     acp;
	PSI_CM_DETAIL_TYPE      cm;
	PSI_OHA_DETAIL_TYPE     oha;
	PSI_TCIP_DETAIL_TYPE    tcip;
	PSI_HSA_DETAIL_TYPE		hsa;
	PSI_SOCKAPP_DETAIL_TYPE sockapp;
} PSI_DETAIL_STORE_TYPE, *PSI_DETAIL_STORE_PTR_TYPE;

/*===========================================================================*/
/*                      prototypes                                           */
/*===========================================================================*/

LSA_VOID psi_res_calc_init(LSA_VOID);
LSA_VOID psi_res_calc_undo_init(LSA_VOID);

LSA_VOID psi_res_calc_delete(LSA_VOID);

LSA_VOID psi_res_calc_set_global_details(
    PSI_SYS_HANDLE const hSysDev);

LSA_VOID psi_res_calc_set_if_details(
	PSI_SYS_HANDLE        const hSysDev,
	LSA_INT               const dev_pool_handle,
	LSA_INT               const nrt_tx_pool_handle,
	LSA_INT               const nrt_rx_pool_handle,
	PSI_HD_INPUT_PTR_TYPE const p_hd_in);

LSA_VOID psi_res_calc_init_hd_outputs(
    PSI_HD_INPUT_PTR_TYPE  const p_hd_in,
	PSI_HD_STORE_PTR_TYPE  const p_hd,
    PSI_HD_OUTPUT_PTR_TYPE const p_hd_out);

LSA_VOID psi_res_calc_set_if_outputs(
	PSI_HD_INPUT_PTR_TYPE  const p_hd_in,
    PSI_HD_OUTPUT_PTR_TYPE const p_hd_out);

PSI_DETAIL_STORE_PTR_TYPE psi_get_detail_store(
    LSA_UINT16 const hd_nr);

LSA_VOID psi_add_detail_store(
    LSA_UINT16            const pnio_if_id,
    PSI_HD_INPUT_PTR_TYPE const p_hd_in);

LSA_VOID psi_remove_detail_store(
    LSA_UINT16 const pnio_if_id);

LSA_UINT32 psi_get_max_ar_count_all_ifs(LSA_VOID);

LSA_UINT8 psi_get_pnio_if_nr(
    LSA_UINT16 const hd_nr);

LSA_UINT32 psi_get_trace_idx_for_sys_path(
    LSA_SYS_PATH_TYPE const sys_path);

/*----------------------------------------------------------------------------*/
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif /* PSI_RES_CALC_H */

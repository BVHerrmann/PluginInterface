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
/*  F i l e               &F: psi_pof.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  channel-detail settings for POF                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   115
#define PSI_MODULE_ID       115 /* PSI_MODULE_ID_PSI_POF */

#include "psi_int.h"

#if ((PSI_CFG_USE_POF == 1) && (PSI_CFG_USE_HD_COMP == 1))

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*===========================================================================*/
/*=== POF ===================================================================*/
/*===========================================================================*/

LSA_RESULT psi_pof_get_path_info(
	LSA_SYS_PTR_TYPE  * sys_ptr_ptr,
	LSA_VOID_PTR_TYPE * detail_ptr_ptr,
	LSA_SYS_PATH_TYPE   sys_path)
{
    LSA_USER_ID_TYPE    user_id;
    POF_DETAIL_PTR_TYPE pDetail;
    PSI_SYS_PTR_TYPE    pSys;
    LSA_UINT16          mbx_id_rsp    = PSI_MBX_ID_MAX;
    LSA_UINT16          comp_id_lower = LSA_COMP_ID_UNUSED;
    LSA_UINT16          result        = LSA_RET_OK;
    LSA_UINT8           MediaType, IsPOF, PhyType, FXTransceiverType;

    LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(sys_path);
    LSA_UINT16 const path  = PSI_SYSPATH_GET_PATH(sys_path);

	PSI_DETAIL_STORE_PTR_TYPE const pDetailsIF = psi_get_detail_store(hd_nr);

    PSI_ASSERT(sys_ptr_ptr != LSA_NULL);
	PSI_ASSERT(pDetailsIF  != LSA_NULL);

    PSI_INIT_USER_ID_UNION(user_id);

    PSI_HD_TRACE_06( 0, LSA_TRACE_LEVEL_NOTE, "psi_pof_get_path_info(sys_path=%#x): pnio_if_nr(%u) hd_nr(%u) hd_runs_on_level_ld(%u) path(%#x/%u)", 
        sys_path, pDetailsIF->pnio_if_nr, hd_nr, pDetailsIF->hd_runs_on_level_ld, path, path );

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pDetail), user_id, sizeof(*pDetail), 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pDetail != LSA_NULL);

	PSI_ALLOC_LOCAL_MEM( ((LSA_VOID_PTR_TYPE*)&pSys), user_id, sizeof(*pSys), 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(pSys != LSA_NULL);

    switch (path)
	{
	    case PSI_PATH_IF_SYS_CM_POF_EDD:
		{
			LSA_UINT16                    port_idx;
			LSA_UINT16              const nrOfPorts = pDetailsIF->nr_of_ports;
	        PSI_HD_STORE_PTR_TYPE   const p_hd = psi_hd_get_hd_store(hd_nr);

            PSI_ASSERT(p_hd != LSA_NULL);
	        PSI_ASSERT(p_hd->is_used);

			for (port_idx = 0; port_idx < PSI_CFG_MAX_PORT_CNT; port_idx++)
			{
				// Initialize with Default NONE
				pDetail->TransProcess[port_idx] = POF_TRANS_PROCESS_NONE;
				pDetail->I2CMuxSelect[port_idx] = PSI_I2C_MUX_SELECT_NONE;

				if (port_idx < nrOfPorts) //number of configured ports not reached?
				{
                    LSA_UINT32  const  HwPortId = p_hd->hd_hw.port_map[port_idx+1].hw_port_id;

                    PSI_EDD_GET_MEDIA_TYPE(
                        p_hd->hd_sys_handle, 
                        HwPortId, 
                        PSI_PORT_MEDIA_TYPE_POF_AUTO_DETECTION_NO, 
                        &MediaType, 
                        &IsPOF, 
                        &PhyType,
                        &FXTransceiverType); /* not used here */

                    //Fiber port?
                    if (MediaType == PSI_MEDIA_TYPE_FIBER)
                    {
                        //an external I2C HW multiplexer exists?
                        if (pDetailsIF->edd_comp_id == LSA_COMP_ID_EDDI)
                        {
						    pDetail->I2CMuxSelect[port_idx] = (LSA_UINT16)HwPortId; //I2CMuxSelect = HwPortId => for external I2C HW multiplexer
                        }
                        //POF port?
                        if (IsPOF)
                        {
                            // Set Details for OMA POF port
                            pDetail->TransProcess[port_idx] = POF_TRANS_PROCESS_OMA;
                        }
					}
				}
			}

			mbx_id_rsp    = PSI_MBX_ID_CM;
			comp_id_lower = pDetailsIF->edd_comp_id;
		}
		break;

	    default:
		{
			LSA_UINT16 rc;

			PSI_FREE_LOCAL_MEM( &rc, pDetail, 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			PSI_FREE_LOCAL_MEM( &rc, pSys, 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
			PSI_ASSERT(rc == LSA_RET_OK);

			*detail_ptr_ptr = LSA_NULL;
			*sys_ptr_ptr    = LSA_NULL;
			result          = LSA_RET_ERR_SYS_PATH;

            PSI_HD_TRACE_05( 0, LSA_TRACE_LEVEL_ERROR, "psi_pof_get_path_info(sys_path=%#x): invalid path(%#x/%u), hd_nr(%u) result(0x%x)",
                sys_path, path, path, hd_nr, result );
        }
		break;
	}

	if (result == LSA_RET_OK)
	{
		// now we setup the Sysptr based on information stored for this PNIO IF
		pSys->hd_nr                 = hd_nr;
		pSys->pnio_if_nr            = pDetailsIF->pnio_if_nr;
		pSys->comp_id               = LSA_COMP_ID_POF;
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

	return result;
}

/*----------------------------------------------------------------------------*/
LSA_RESULT psi_pof_release_path_info(
	LSA_SYS_PTR_TYPE  sys_ptr,
	LSA_VOID_PTR_TYPE detail_ptr)
{
	LSA_UINT16             rc;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(sys_ptr);
	PSI_ASSERT(detail_ptr);

	PSI_ASSERT(pSys->comp_id == LSA_COMP_ID_POF);

    PSI_HD_TRACE_00( 0, LSA_TRACE_LEVEL_NOTE, "psi_pof_release_path_info()" );

	PSI_FREE_LOCAL_MEM( &rc, detail_ptr, 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	PSI_FREE_LOCAL_MEM( &rc, sys_ptr, 0, LSA_COMP_ID_POF, PSI_MTYPE_LOCAL_MEM );
	PSI_ASSERT(rc == LSA_RET_OK);

	return LSA_RET_OK;
}

#endif //   ((PSI_CFG_USE_POF == 1) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

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
/*  F i l e               &F: edd_cfg.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of EDDx using PSI.                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2500
#define PSI_MODULE_ID       2500 /* PSI_MODULE_ID_EDD_CFG */

#include "psi_int.h"

#if (((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1)) && (PSI_CFG_USE_HD_COMP == 1))
/*----------------------------------------------------------------------------*/

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

/*=============================================================================*/
/*        MEMORY-FUNCTIONS                                                     */
/*=============================================================================*/
LSA_VOID psi_edd_alloc_nrt_tx_mem(
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16             alloc_comp_id;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user allocates the memory?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		alloc_comp_id = pSys->comp_id_lower;
	}
	else
	{
		alloc_comp_id = lsa_comp_id;
	}

	switch (alloc_comp_id) // EDDx variant ?
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
        {
            #if defined(PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
            if (length < 64UL)
            {
                // padding to 4-byte-aligned length (EDDI specific bug)
                if (length & 3UL)
                {
                    length = (LSA_UINT32)(length & (~3UL)) + 4UL;
                }
            }
            #endif
        }
		// !!! no break, the (patched) "length" of EDDI goes on in EDDP case !!!
        //lint --e(825) -fallthrough
        #endif // PSI_CFG_USE_EDDI

	    case LSA_COMP_ID_EDDP:
		{
            // allocate NRT buffer from NRT memory pool
			PSI_NRT_ALLOC_TX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNrtTx, lsa_comp_id );

			PSI_SYSTEM_TRACE_07( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_alloc_nrt_tx_mem(): lower_mem_ptr(0x%08x) length(%u) hd_nr(%u) lsa_comp_id(%u/%#x) alloc_comp_id(%u/%#x)",
				*lower_mem_ptr_ptr, length, pSys->hd_nr, lsa_comp_id, lsa_comp_id, alloc_comp_id, alloc_comp_id );
        }
		break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
		{
            if (psi_hd_is_edd_nrt_copy_if_on(pSys->hd_nr))
            {
	            LSA_USER_ID_TYPE null_usr_id;
                PSI_INIT_USER_ID_UNION( null_usr_id );
                // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, null_usr_id, length, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
                if (length < 60UL)
                {
                    PSI_SYSTEM_TRACE_01( pSys->trace_idx, LSA_TRACE_LEVEL_NOTE, "psi_edd_alloc_nrt_tx_mem(): EDDS NRT TX buffer is increased from length(%u) to 60 bytes", length );
                    length = 60UL; //set minimum length required by TI AM5728 GMAC
                }

                // allocate NRT buffer from NRT memory pool
			    PSI_NRT_ALLOC_TX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNrtTx, lsa_comp_id );
            }

			PSI_SYSTEM_TRACE_07( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_alloc_nrt_tx_mem(): lower_mem_ptr(0x%08x) length(%u) hd_nr(%u) lsa_comp_id(%u/%#x) alloc_comp_id(%u/%#x)",
				*lower_mem_ptr_ptr, length, pSys->hd_nr, lsa_comp_id, lsa_comp_id, alloc_comp_id, alloc_comp_id );
		}
		break;
        #endif // PSI_CFG_USE_EDDS

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_edd_alloc_nrt_tx_mem(): invalid alloc_comp_id(%u/%#x)",
				alloc_comp_id, alloc_comp_id );
            *lower_mem_ptr_ptr = LSA_NULL;
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edd_free_nrt_tx_mem(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16             free_comp_id;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user free the memory?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		free_comp_id = pSys->comp_id_lower;
	}
	else
	{
		free_comp_id = lsa_comp_id;
	}

	switch (free_comp_id) // EDDx variant ?
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
        #endif
	    case LSA_COMP_ID_EDDP:
		{
			PSI_SYSTEM_TRACE_06( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_free_nrt_tx_mem(): lower_mem_ptr(0x%08x) hd_nr(%u) lsa_comp_id(%u/%#x) free_comp_id(%u/%#x)",
				lower_mem_ptr, pSys->hd_nr, lsa_comp_id, lsa_comp_id, free_comp_id, free_comp_id );

			// free NRT buffer to NRT memory pool
			PSI_NRT_FREE_TX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNrtTx, lsa_comp_id );
		}
		break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
		{
			PSI_SYSTEM_TRACE_06( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_free_nrt_tx_mem(): lower_mem_ptr(0x%08x) hd_nr(%u) lsa_comp_id(%u/%#x) free_comp_id(%u/%#x)",
				lower_mem_ptr, pSys->hd_nr, lsa_comp_id, lsa_comp_id, free_comp_id, free_comp_id );

            if (psi_hd_is_edd_nrt_copy_if_on(pSys->hd_nr))
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // free NRT buffer to NRT memory pool
			    PSI_NRT_FREE_TX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNrtTx, lsa_comp_id );
            }
		}
		break;
        #endif // PSI_CFG_USE_EDDS

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_edd_free_nrt_tx_mem(): invalid free_comp_id(%u/%#x)",
				free_comp_id, free_comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edd_alloc_nrt_rx_mem(
	LSA_VOID_PTR_TYPE * lower_mem_ptr_ptr,
	LSA_UINT32          length,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16             alloc_comp_id;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user allocates the memory?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		alloc_comp_id = pSys->comp_id_lower;
	}
	else
	{
		alloc_comp_id = lsa_comp_id;
	}

	switch (alloc_comp_id) // EDDx variant ?
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
        #endif
	    case LSA_COMP_ID_EDDP:
		{
			// allocate NRT buffer from NRT memory pool
			PSI_NRT_ALLOC_RX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNrtRx, lsa_comp_id );

			PSI_SYSTEM_TRACE_07( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_alloc_nrt_rx_mem(): lower_mem_ptr(0x%08x) length(%u) hd_nr(%u) lsa_comp_id(%u/%#x) alloc_comp_id(%u/%#x)",
				*lower_mem_ptr_ptr, length, pSys->hd_nr, lsa_comp_id, lsa_comp_id, alloc_comp_id, alloc_comp_id );
		}
		break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
		{
            if (psi_hd_is_edd_nrt_copy_if_on(pSys->hd_nr))
            {
	            LSA_USER_ID_TYPE null_usr_id;
                PSI_INIT_USER_ID_UNION( null_usr_id );
                // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( lower_mem_ptr_ptr, null_usr_id, length, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // allocate NRT buffer from NRT memory pool
			    PSI_NRT_ALLOC_RX_MEM( lower_mem_ptr_ptr, length, pSys->hPoolNrtRx, lsa_comp_id );
            }

			PSI_SYSTEM_TRACE_07( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_alloc_nrt_rx_mem(): lower_mem_ptr(0x%08x) length(%u) hd_nr(%u) lsa_comp_id(%u/%#x) alloc_comp_id(%u/%#x)",
				*lower_mem_ptr_ptr, length, pSys->hd_nr, lsa_comp_id, lsa_comp_id, alloc_comp_id, alloc_comp_id );
		}
		break;
        #endif // PSI_CFG_USE_EDDS

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_edd_alloc_nrt_rx_mem(): invalid alloc_comp_id(%u/%#x)",
				alloc_comp_id, alloc_comp_id );
            *lower_mem_ptr_ptr = LSA_NULL;
			PSI_FATAL( 0 );
		}
	}
}

/*----------------------------------------------------------------------------*/
LSA_VOID psi_edd_free_nrt_rx_mem(
	LSA_UINT16        * ret_val_ptr,
	LSA_VOID_PTR_TYPE   lower_mem_ptr,
	LSA_SYS_PTR_TYPE    sys_ptr,
	LSA_UINT16          lsa_comp_id )
{
	LSA_UINT16             free_comp_id;
	PSI_SYS_PTR_TYPE const pSys = (PSI_SYS_PTR_TYPE)sys_ptr;

	PSI_ASSERT(pSys);

	if ( (lsa_comp_id != LSA_COMP_ID_EDDI) && // Upper EDDx user free the memory?
		 (lsa_comp_id != LSA_COMP_ID_EDDP) && 
		 (lsa_comp_id != LSA_COMP_ID_EDDS) )
	{
		free_comp_id = pSys->comp_id_lower;
	}
	else
	{
		free_comp_id = lsa_comp_id;
	}

	switch (free_comp_id) // EDDx variant ?
	{
        #if (PSI_CFG_USE_EDDI == 1)
	    case LSA_COMP_ID_EDDI:
        #endif
	    case LSA_COMP_ID_EDDP:
		{
			PSI_SYSTEM_TRACE_06( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_free_nrt_rx_mem(): lower_mem_ptr(0x%08x) hd_nr(%u) lsa_comp_id(%u/%#x) free_comp_id(%u/%#x)",
				lower_mem_ptr, pSys->hd_nr, lsa_comp_id, lsa_comp_id, free_comp_id, free_comp_id );

			// free NRT buffer to NRT memory pool
			PSI_NRT_FREE_RX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNrtRx, lsa_comp_id );
		}
		break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
		{
			PSI_SYSTEM_TRACE_06( pSys->trace_idx, LSA_TRACE_LEVEL_CHAT, "psi_edd_free_nrt_rx_mem(): lower_mem_ptr(0x%08x) hd_nr(%u) lsa_comp_id(%u/%#x) free_comp_id(%u/%#x)",
				lower_mem_ptr, pSys->hd_nr, lsa_comp_id, lsa_comp_id, free_comp_id, free_comp_id );

            if (psi_hd_is_edd_nrt_copy_if_on(pSys->hd_nr))
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, lower_mem_ptr, pSys, lsa_comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
			    // free NRT buffer to NRT memory pool
			    PSI_NRT_FREE_RX_MEM( ret_val_ptr, lower_mem_ptr, pSys->hPoolNrtRx, lsa_comp_id );
            }
		}
		break;
        #endif // PSI_CFG_USE_EDDS

        default:
		{
			PSI_SYSTEM_TRACE_02( pSys->trace_idx, LSA_TRACE_LEVEL_FATAL, "psi_edd_free_nrt_rx_mem(): invalid free_comp_id(%u/%#x)",
				free_comp_id, free_comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
			PSI_FATAL( 0 );
		}
	}
}

/*=============================================================================*/
/*   Requests                                                                  */
/*=============================================================================*/
LSA_UINT16 psi_edd_get_mailbox_id(
    const LSA_VOID_PTR_TYPE const rqb_ptr,
          LSA_UINT16        const comp_id_upper,
          LSA_SYS_PTR_TYPE  const sys_ptr )
{
    LSA_UINT16             mbx_id = PSI_MBX_ID_MAX;
    PSI_SYS_PTR_TYPE const pSys   = (PSI_SYS_PTR_TYPE)sys_ptr;

    PSI_ASSERT(pSys);

    #if (PSI_CFG_USE_EDDI != 1)
    LSA_UNUSED_ARG(rqb_ptr);
    LSA_UNUSED_ARG(comp_id_upper);
    #endif

    #if (PSI_CFG_USE_HD_COMP == 1)
    // lookup for MBX-ID for EDDx lower request
    switch (pSys->comp_id_lower)
    {
        #if (PSI_CFG_USE_EDDI == 1)
        case LSA_COMP_ID_EDDI:
        {
	        LSA_UINT32 const service = EDD_RQB_GET_SERVICE((EDD_UPPER_RQB_PTR_TYPE)rqb_ptr);

			mbx_id = PSI_MBX_ID_EDDI_REST; // default for all lower requests

            // The mailbox (tasks) depends on service
            switch (service)
            {
                case EDD_SRV_NRT_RECV:
                case EDD_SRV_NRT_SEND:
                case EDD_SRV_NRT_CANCEL:
                {
                    switch (comp_id_upper)
                    {
                        case LSA_COMP_ID_MRP:
                        case LSA_COMP_ID_LLDP:
                        case LSA_COMP_ID_HSA:
                        {
					        mbx_id = PSI_MBX_ID_EDDI_NRT_ORG;
                        }
                        break;

                        case LSA_COMP_ID_GSY:
                        {
                            if (pSys->psi_path == PSI_PATH_IF_SYS_GSY_EDD_ANNO)
                            {
                                // Announce-Frames from GSY to EDDI
				                mbx_id = PSI_MBX_ID_EDDI_NRT_LOW;
                            }
                            else
                            {
					            mbx_id = PSI_MBX_ID_EDDI_NRT_ORG;
                            }
                        }
                        break;

                        default:
                        {
				            mbx_id = PSI_MBX_ID_EDDI_NRT_LOW;
                        }
                    }
                }
                break;

                case EDD_SRV_NRT_SET_DCP_FILTER:
                {
                    mbx_id = PSI_MBX_ID_EDDI_NRT_LOW;
                }
                break;

                case EDD_SRV_NRT_SEND_TIMESTAMP:
                case EDD_SRV_SYNC_RECV:
                case EDD_SRV_SYNC_SEND:
                case EDD_SRV_SYNC_CANCEL:
                case EDD_SRV_SYNC_SET_RATE:
                {
                    mbx_id = PSI_MBX_ID_EDDI_NRT_ORG;
                }
                break;

                default: break;
            }
        }
        break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
        case LSA_COMP_ID_EDDP:
		{
            mbx_id = PSI_MBX_ID_EDDP_LOW;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
            PSI_HD_STORE_PTR_TYPE const p_hd = psi_hd_get_hd_store(pSys->hd_nr);
            PSI_ASSERT(p_hd != LSA_NULL);
            PSI_ASSERT(p_hd->is_used);
            mbx_id = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
            PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
            mbx_id = PSI_MBX_ID_EDDT_LOW;
		}
		break;
        #endif

        default: break;
    }
    #endif //PSI_CFG_USE_HD_COMP

    PSI_ASSERT(mbx_id < PSI_MBX_ID_MAX);

    return mbx_id;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT psi_edd_get_open_fct(
    LSA_UINT16 const comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch (comp_id_edd)
	{
        #if (PSI_CFG_USE_EDDI == 1)
        case LSA_COMP_ID_EDDI:
		{
			p_func = (EDD_REQUEST_FCT)eddi_open_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
        case LSA_COMP_ID_EDDP:
		{
			p_func = (EDD_REQUEST_FCT)eddp_open_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			p_func = (EDD_REQUEST_FCT)edds_open_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			p_func = (EDD_REQUEST_FCT)eddt_open_channel;
		}
		break;
        #endif

        default: break;
	}

	PSI_ASSERT(p_func != LSA_NULL);

	return p_func;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT psi_edd_get_close_fct(
    LSA_UINT16 const comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch (comp_id_edd)
	{
        #if (PSI_CFG_USE_EDDI == 1)
        case LSA_COMP_ID_EDDI:
		{
			p_func = (EDD_REQUEST_FCT)eddi_close_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
        case LSA_COMP_ID_EDDP:
		{
			p_func = (EDD_REQUEST_FCT)eddp_close_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			p_func = (EDD_REQUEST_FCT)edds_close_channel;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			p_func = (EDD_REQUEST_FCT)eddt_close_channel;
		}
		break;
        #endif

        default: break;
	}

	PSI_ASSERT(p_func != LSA_NULL);

	return p_func;
}

/*----------------------------------------------------------------------------*/
EDD_REQUEST_FCT psi_edd_get_request_fct(
    LSA_UINT16 const comp_id_edd )
{
	EDD_REQUEST_FCT p_func = LSA_NULL;

	// lookup for MBX-ID for EDDx lower request
	switch (comp_id_edd)
	{
        #if (PSI_CFG_USE_EDDI == 1)
        case LSA_COMP_ID_EDDI:
		{
			p_func = (EDD_REQUEST_FCT)eddi_request;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
        case LSA_COMP_ID_EDDP:
		{
			p_func = (EDD_REQUEST_FCT)eddp_request;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			p_func = (EDD_REQUEST_FCT)edds_request;
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			p_func = (EDD_REQUEST_FCT)eddt_request;
		}
		break;
        #endif

        default: break;
	}

	PSI_ASSERT(p_func != LSA_NULL);

	return p_func;
}

#endif //(((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1)) && (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

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
/*  F i l e               &F: psi_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the PSI LD/HD user interface (see psi_usr.h)                  */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   2
#define PSI_MODULE_ID       2 /* PSI_MODULE_ID_PSI_USR */

#include "psi_int.h"
#include "psi_ld.h"
#include <string.h>

PSI_FILE_SYSTEM_EXTENSION(PSI_MODULE_ID)

#if ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

/*---------------------------------------------------------------------------*/
/* PSI EDD User Request handling                                             */
/*                                                                           */
/* As the user doesn't know which EDD is used on the HD, PSI needs to find   */
/* the correct one and to delegate the request there.                        */
/*                                                                           */
/* The comp_id of the RQB needs to be set to LSA_COMP_ID_EDD                 */
/* Currently only 1 Path is supported (PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC)*/
/*---------------------------------------------------------------------------*/

#if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

typedef struct
{
	LSA_BOOL                            bUsed;
	EDD_HANDLE_LOWER_TYPE               edd_handle;     /* EDD Channel handle lower (delivered by EDD)  */
	LSA_HANDLE_TYPE                     psi_handle;     /* PSI Channel handle                           */
	LSA_HANDLE_TYPE                     user_handle;    /* EDD Channel handle upper (delivered by User) */
	EDD_UPPER_CALLBACK_FCT_PTR_TYPE     user_cbf;       /* User callback function                       */
	LSA_UINT16                          edd_type;       /* EDD comp_id                                  */
    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
	LSA_UINT16                          mbx_id_edds_rqb;
    #endif
} PSI_EDD_USER_CHANNEL_TYPE, *PSI_EDD_USER_CHANNEL_PTR_TYPE;

typedef struct
{
	PSI_EDD_USER_CHANNEL_TYPE           channel[PSI_CFG_MAX_IF_CNT];
} PSI_EDD_USER_CHANNEL_STORE_TYPE;

static PSI_EDD_USER_CHANNEL_STORE_TYPE g_PsiEddUserStore;

static LSA_VOID psi_hd_edd_user_channel_request_done(
    EDD_UPPER_RQB_PTR_TYPE const edd_rqb_ptr);

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_edd_user_open_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	/* Handle an EDD service Open-Channel. The Request needs to be routed to the correct EDDx component. */

	EDD_RQB_TYPE                  * const edd_rqb_ptr = (EDD_RQB_TYPE*)rqb_ptr;
	EDD_RQB_OPEN_CHANNEL_TYPE           * edd_open_ptr;
	PSI_DETAIL_STORE_PTR_TYPE             pDetailsIF;
	PSI_EDD_USER_CHANNEL_PTR_TYPE         channel_ptr = LSA_NULL;
	LSA_UINT16                            hd_nr, path, i;
    LSA_UINT16                            pnio_if_nr;
    LSA_UINT16                            edd_type;
    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
    LSA_UINT16                            mbx_id;
    #endif

    PSI_ASSERT(rqb_ptr != LSA_NULL);

	PSI_ASSERT(PSI_RQB_GET_COMP_ID(rqb_ptr) == LSA_COMP_ID_EDD);

	edd_open_ptr = (EDD_RQB_OPEN_CHANNEL_TYPE*)(edd_rqb_ptr->pParam);
	PSI_ASSERT(psi_is_not_null(edd_open_ptr));

	hd_nr       = PSI_SYSPATH_GET_HD(edd_open_ptr->SysPath);
	path        = PSI_SYSPATH_GET_PATH(edd_open_ptr->SysPath);
    pnio_if_nr  = psi_get_pnio_if_nr(hd_nr);

    PSI_ASSERT(path == PSI_PATH_IF_APP_EDD_SIMATIC_TIME_SYNC);

	PSI_ENTER();

	pDetailsIF = psi_get_detail_store(hd_nr);
    edd_type   = pDetailsIF->edd_comp_id;

	for (i = 0; i < PSI_CFG_MAX_IF_CNT; i++)
	{
		if (!g_PsiEddUserStore.channel[i].bUsed)
		{
			channel_ptr             = &(g_PsiEddUserStore.channel[i]);
			channel_ptr->psi_handle = (LSA_HANDLE_TYPE)i;
			break;
		}
	}
	PSI_ASSERT(channel_ptr != LSA_NULL);

	channel_ptr->edd_type               = edd_type;
	channel_ptr->user_handle            = edd_open_ptr->HandleUpper;
	channel_ptr->user_cbf               = edd_open_ptr->Cbf;
    #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
    {
	    PSI_HD_STORE_PTR_TYPE const p_hd = psi_hd_get_hd_store(hd_nr);
	    PSI_ASSERT(p_hd != LSA_NULL);
	    PSI_ASSERT(p_hd->is_used);
        mbx_id                          = p_hd->hd_hw.edd.edds.mbx_id_edds_rqb;
	    channel_ptr->mbx_id_edds_rqb    = mbx_id;
    }
    #endif
	channel_ptr->bUsed                  = LSA_TRUE;

	edd_open_ptr->Cbf                   = psi_hd_edd_user_channel_request_done;
	edd_open_ptr->HandleUpper           = channel_ptr->psi_handle;

	PSI_EXIT();

	PSI_HD_TRACE_07(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_user_open_channel(): user to edd_type(%u/%#x) pnio_if_nr(%u) hd_nr(%u) path(%#x/%u) sys_path(%#x)", 
        edd_type, edd_type, pnio_if_nr, hd_nr, path, path, edd_open_ptr->SysPath);

    switch (edd_type)
	{
        #if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDI:
        {
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_open_channel, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDP:
        {
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_open_channel, rqb_ptr );
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDS:
        {
            PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));
			psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDT:
        {
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_open_channel, rqb_ptr);
		    break;
        }
        #endif

        default:
        {
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_user_open_channel(): invalid edd_type(%u/%#x) rqb_ptr(0x%08x)",
				edd_type, edd_type, rqb_ptr);
		    PSI_FATAL(0);
        }
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_edd_user_close_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	/* Handle an EDD service Close-Channel. The Request needs to be routed to the correct EDDx component. */

    EDD_UPPER_RQB_PTR_TYPE        const edd_rqb_ptr = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_EDD_USER_CHANNEL_PTR_TYPE       channel_ptr;
	EDD_HANDLE_LOWER_TYPE         const EDD_Handle_Lower = PSI_EDD_RQB_GET_HANDLE_LOWER(edd_rqb_ptr);
	LSA_HANDLE_TYPE               const psi_handle       = *((LSA_HANDLE_TYPE*)&EDD_Handle_Lower);
    LSA_UINT16                          edd_type;

	PSI_ASSERT((LSA_UINT32)EDD_Handle_Lower < PSI_CFG_MAX_IF_CNT);

	PSI_ENTER();

	channel_ptr = &(g_PsiEddUserStore.channel[psi_handle]);
    edd_type    = channel_ptr->edd_type;

	PSI_ASSERT(channel_ptr->bUsed);

	PSI_EDD_RQB_SET_HANDLE_LOWER(edd_rqb_ptr, channel_ptr->edd_handle);

	PSI_EXIT();

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_edd_user_close_channel(): user to edd_type(%u/0x%X)",
        edd_type, edd_type);

    switch (edd_type)
	{
        #if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDI:
        {
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_close_channel, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDP:
        {
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_close_channel, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDS:
        {
            LSA_UINT16 const mbx_id = channel_ptr->mbx_id_edds_rqb;
            PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));
			psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDT:
        {
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_close_channel, rqb_ptr);
		    break;
        }
        #endif

        default:
        {
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_user_close_channel(): invalid edd_type(%u/0x%X) rqb_ptr(0x%08X)",
				edd_type, edd_type, rqb_ptr);
		    PSI_FATAL(0);
        }
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_edd_user_request(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	/* Handle an EDD service EDD-Request. The Request needs to be routed to the correct EDDx component. */

    EDD_UPPER_RQB_PTR_TYPE        const edd_rqb_ptr      = (EDD_UPPER_RQB_PTR_TYPE)rqb_ptr;
	PSI_EDD_USER_CHANNEL_PTR_TYPE       channel_ptr;
	EDD_HANDLE_LOWER_TYPE         const EDD_Handle_Lower = PSI_EDD_RQB_GET_HANDLE_LOWER(edd_rqb_ptr);
	LSA_HANDLE_TYPE               const psi_handle       = *((LSA_HANDLE_TYPE*)&EDD_Handle_Lower);
    LSA_UINT16                          edd_type;

	PSI_ASSERT((LSA_UINT32)EDD_Handle_Lower < PSI_CFG_MAX_IF_CNT);

	PSI_ENTER();

	channel_ptr = &(g_PsiEddUserStore.channel[psi_handle]);
    edd_type    = channel_ptr->edd_type;

	PSI_ASSERT(channel_ptr->bUsed);

	PSI_EDD_RQB_SET_HANDLE_LOWER(edd_rqb_ptr, channel_ptr->edd_handle);

	PSI_EXIT();

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_edd_user_request(): rqb_ptr(0x%08X) edd_type(%u/0x%X)",
        rqb_ptr, edd_type, edd_type);

    switch (edd_type)
	{
        #if ((PSI_CFG_USE_EDDI == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDI:
        {
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDP == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDP:
        {
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_request, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDS == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDS:
        {
            LSA_UINT16 const mbx_id = channel_ptr->mbx_id_edds_rqb;
            PSI_ASSERT((mbx_id == PSI_MBX_ID_EDDS_RQB_H1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_H2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L1) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L2) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L3) || (mbx_id == PSI_MBX_ID_EDDS_RQB_L4));
			psi_request_start(mbx_id, (PSI_REQUEST_FCT)edds_request, rqb_ptr);
		    break;
        }
        #endif

        #if ((PSI_CFG_USE_EDDT == 1) && (PSI_CFG_USE_HD_COMP == 1))
		case LSA_COMP_ID_EDDT:
        {
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_request, rqb_ptr);
		    break;
        }
        #endif

        default:
        {
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_edd_user_request(): invalid edd_type(%u/%#x) rqb_ptr(0x%08x)",
				edd_type, edd_type, rqb_ptr);
		    PSI_FATAL(0);
        }
	}
}

/*---------------------------------------------------------------------------*/
static LSA_VOID psi_hd_edd_user_channel_request_done(
    EDD_UPPER_RQB_PTR_TYPE const edd_rqb_ptr)
{
	EDD_RQB_OPEN_CHANNEL_TYPE         * edd_open_ptr;
	PSI_EDD_USER_CHANNEL_PTR_TYPE       channel_ptr;
	PSI_SYS_TYPE                        Sys;
	LSA_HANDLE_TYPE               const psi_handle = PSI_RQB_GET_HANDLE(edd_rqb_ptr);
	LSA_HANDLE_TYPE                     user_handle;

	PSI_ASSERT(psi_handle < PSI_CFG_MAX_IF_CNT);

	channel_ptr = &(g_PsiEddUserStore.channel[psi_handle]);
    user_handle = channel_ptr->user_handle;

	PSI_ASSERT(channel_ptr->bUsed);

	#if (PSI_CFG_USE_HIF_HD == 1)
    if (psi_get_hd_runs_on_ld(PSI_HD_CURRENT_HD) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
    {
        Sys.hd_runs_on_level_ld = PSI_HD_RUNS_ON_LEVEL_LD_NO;
	    Sys.mbx_id_rsp          = PSI_MBX_ID_HIF_HD;
    }
    else
	#endif
    {
	    Sys.mbx_id_rsp = PSI_MBX_ID_USER; // EDD user direct
    }

	switch (LSA_RQB_GET_OPCODE(edd_rqb_ptr))
	{
		case EDD_OPC_OPEN_CHANNEL:
        {
            PSI_HD_STORE_PTR_TYPE p_hd;

			edd_open_ptr              = (EDD_RQB_OPEN_CHANNEL_TYPE*)(edd_rqb_ptr->pParam);
			edd_open_ptr->Cbf         = channel_ptr->user_cbf;
			edd_open_ptr->HandleUpper = user_handle;

			if (LSA_RQB_GET_RESPONSE(edd_rqb_ptr) == LSA_OK)
			{
                channel_ptr->edd_handle   = edd_open_ptr->HandleLower;
				edd_open_ptr->HandleLower = (EDD_HANDLE_LOWER_TYPE)(LSA_UINT32)(channel_ptr->psi_handle);
			}
			else //response negative
			{
				channel_ptr->bUsed = LSA_FALSE;
			}

            Sys.hd_nr = PSI_SYSPATH_GET_HD(edd_open_ptr->SysPath);

            p_hd = psi_hd_get_hd_store(Sys.hd_nr);
            Sys.hd_runs_on_level_ld = p_hd->hd_runs_on_level_ld;
            PSI_ASSERT(Sys.hd_runs_on_level_ld < PSI_HD_RUNS_ON_LEVEL_LD_MAX);
        }
		break;

		case EDD_OPC_CLOSE_CHANNEL:
        {
			if (LSA_RQB_GET_RESPONSE(edd_rqb_ptr) == LSA_OK)
			{
				channel_ptr->bUsed = LSA_FALSE;
			}
        }
		break;

		default: break;
	}

	PSI_RQB_SET_HANDLE(edd_rqb_ptr, user_handle);

	psi_request_done((PSI_REQUEST_FCT)channel_ptr->user_cbf, edd_rqb_ptr, &Sys);
}

#endif // ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))

#if (PSI_CFG_USE_LD_COMP == 1) 
/*---------------------------------------------------------------------------*/
/* PSI LD user interface                                                     */
/*                                                                           */
/* hif_*d_open_channel/close_channel/request can be called directly          */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_system(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
    PSI_ASSERT(rqb_ptr != LSA_NULL);

	PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_system(): rqb_ptr(0x%08x) opcode(0x%x)",
        rqb_ptr, PSI_RQB_GET_OPCODE(rqb_ptr));

	switch (PSI_RQB_GET_OPCODE(rqb_ptr))
	{
	    case PSI_OPC_LD_OPEN_DEVICE:
		{
			PSI_UPPER_LD_OPEN_PTR_TYPE const p_open = &rqb_ptr->args.ld_open;

			if (psi_function_is_null(p_open->psi_request_upper_done_ptr))
			{
                // no callback
				PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_system(): no CBF defined for LD Open, rqb_ptr(0x%08x)", rqb_ptr);
				PSI_FATAL(0);
			}
			else
			{
				// Start Open Device handling
				psi_ld_open_device(rqb_ptr);
			}
		}
		break;

        case PSI_OPC_LD_CLOSE_DEVICE:
		{
			PSI_UPPER_LD_CLOSE_PTR_TYPE const p_close = &rqb_ptr->args.ld_close;

			if (psi_function_is_null(p_close->psi_request_upper_done_ptr))
			{
                // no callback
				PSI_LD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_system(): no CBF defined for LD Close, rqb_ptr(0x%08x)", rqb_ptr);
				PSI_FATAL(0);
			}
			else
			{
				// Start Close Device handling
				psi_ld_close_device(rqb_ptr);
			}
		}
		break;

        default:
		{
			PSI_LD_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_system(): invalid opcode(0x%x) rqb_ptr(0x%08x)",
				PSI_RQB_GET_OPCODE(rqb_ptr), rqb_ptr);
            PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_open_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
    LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_open_channel(): user to comp_id(%u/%#x) rqb_ptr(0x%08x)",
        comp_id, comp_id, rqb_ptr);
	
	// Handle an open request from upper (HIF LD or user)
	// Note: the open request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected
	switch (comp_id)
	{
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if ( PSI_CFG_USE_HIF_HD == 1 )
            LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(((CM_RQB_TYPE*)rqb_ptr)->args.channel.open->sys_path);
            if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr);
            }
            else
            #endif
            {
			    psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_IOH == 1)
		case LSA_COMP_ID_IOH:
		{
            // IOH runs on LD level, delegate request direct to IOH
            psi_request_start(PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_open_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_DNS == 1)
		case LSA_COMP_ID_DNS:
		{
			// DNS runs on LD level, delegate request direct to DNS
			psi_request_start(PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_open_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start(PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_open_channel, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start(PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_open_channel, rqb_ptr);
		}
		break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD:
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
            LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(((EDD_RQB_OPEN_CHANNEL_TYPE*)(((EDD_RQB_TYPE*)rqb_ptr)->pParam))->SysPath);
            if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_open_channel, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
            LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(((NARE_RQB_TYPE*)rqb_ptr)->args.channel.sys_path);
            if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, rqb_ptr);
            }
		}
		break;
        #endif

		#if (PSI_CFG_USE_HSA == 1)
		case LSA_COMP_ID_HSA:
		{
			// HSA runs on LD level, delegate request direct to HSA
			psi_request_start(PSI_MBX_ID_HSA, (PSI_REQUEST_FCT)hsa_open_channel, rqb_ptr);
		}
		break;
		#endif

        #if (PSI_CFG_USE_SOCKAPP == 1)
        case LSA_COMP_ID_SOCKAPP:
        {
            // SOCKAPP runs on LD level, delegate request direct to SOCKAPP
            psi_request_start(PSI_MBX_ID_SOCKAPP, (PSI_REQUEST_FCT)sockapp_open_channel, rqb_ptr);
        }
        break;
        #endif

		#if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			// Pass the RQB directly to the LD-EPS if one (or both) of the following is correct:
			//	-there is no HIF_HD (HD-EPS is LD-EPS)
			//  -the RQB is determined for the LD-EPS
			LSA_UINT16 const path = PSI_SYSPATH_GET_PATH(((EPS_RQB_PTR_TYPE2)rqb_ptr)->args.channel->sys_path);

            if (path == PSI_PATH_GLO_APP_EPS_LD)
			{
				//RQB determined for LD-EPS
				psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_open_channel, rqb_ptr);
			}
			else
			{
				// RQB determined for HD-EPS
				#if (PSI_CFG_USE_HIF_HD == 1)
                LSA_UINT16 const hd_nr = PSI_SYSPATH_GET_HD(((EPS_RQB_PTR_TYPE2)rqb_ptr)->args.channel->sys_path);
                if (psi_get_hd_runs_on_ld(hd_nr) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
                {
				    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_open_channel, rqb_ptr);
                }
                else
				#endif
                {
				    //there is no HIF_HD (HD-EPS is LD-EPS)
				    psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_open_channel, rqb_ptr);
                }
			}
		}
		break;
        #endif

		default:
		{
			PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_open_channel(): invalid comp_id(%u/%#x), rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
            PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_close_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
    LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_ld_close_channel(): user to comp_id(%u/%#x) rqb_ptr(0x%08x)",
        comp_id, comp_id, rqb_ptr);

	// Handle a close request from upper (HIF LD or user)
	// Note: the close request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected
	switch (comp_id)
	{
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if (PSI_CFG_USE_HIF_HD == 1)
            if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
    			psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr);
            }
            else
            #endif
            {
			    psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_IOH == 1)
		case LSA_COMP_ID_IOH:
		{
            // IOH runs on LD level, delegate request direct to IOH
            psi_request_start(PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_close_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_DNS == 1)
		case LSA_COMP_ID_DNS:
		{
			// DNS runs on LD level, delegate request direct to DNS
			psi_request_start(PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_close_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start(PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_close_channel, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start(PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_close_channel, rqb_ptr);
		}
		break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD:
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
	        EDD_RQB_TYPE          * const edd_rqb_ptr      = (EDD_RQB_TYPE*)(LSA_VOID_PTR_TYPE)rqb_ptr;
	        EDD_HANDLE_LOWER_TYPE   const EDD_Handle_Lower = PSI_EDD_RQB_GET_HANDLE_LOWER(edd_rqb_ptr);
	        LSA_HANDLE_TYPE         const psi_handle       = *((LSA_HANDLE_TYPE*)&EDD_Handle_Lower);

            PSI_ASSERT((LSA_UINT32)EDD_Handle_Lower < PSI_MAX_CHANNELS_PER_COMPONENT);

            if (psi_ld_get_hd_runs_on_ld(comp_id, psi_handle) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_close_channel, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
            if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, rqb_ptr);
            }
		}
		break;
        #endif

		#if (PSI_CFG_USE_HSA == 1)
		case LSA_COMP_ID_HSA:
		{
			// HSA runs on LD level, delegate request direct to HSA
			psi_request_start(PSI_MBX_ID_HSA, (PSI_REQUEST_FCT)hsa_close_channel, rqb_ptr);
		}
		break;
		#endif

        #if (PSI_CFG_USE_SOCKAPP == 1)
        case LSA_COMP_ID_SOCKAPP:
        {
            // SOCKAPP runs on LD level, delegate request direct to SOCKAPP
            psi_request_start(PSI_MBX_ID_SOCKAPP, (PSI_REQUEST_FCT)sockapp_close_channel, rqb_ptr);
        }
        break;
        #endif

		#if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			//Pass the RQB directly to the LD-EPS if one (or both) of the following is correct:
			//	-there is no HIF_HD (HD-EPS is LD-EPS)
			//  -the RQB is determined for the LD-EPS
			LSA_BOOL meant_for_ld_eps = ((EPS_RQB_PTR_TYPE2)rqb_ptr)->meant_for_ld;

            if (meant_for_ld_eps)
			{
				//RQB determined for LD-EPS
				psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_close_channel, rqb_ptr);
			}
			else
			{
				//RQB determined for HD-EPS
				#if (PSI_CFG_USE_HIF_HD == 1)
                if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
                {
				    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_close_channel, rqb_ptr);
                }
                else
				#endif
                {
				    //there is no HIF_HD (HD-EPS is LD-EPS)
				    psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_close_channel, rqb_ptr);
                }
			}
		}
		break;
        #endif

		default:
		{
			PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_close_channel(): invalid comp_id(%u/%#x) rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
			PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_ld_request(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
    LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_ld_request(): rqb_ptr(0x%08x) comp_id(%u/%#x)",
        rqb_ptr, comp_id, comp_id);

	// Handle a service request from upper (HIF LD or user)
	// Note: the service request is delegated based on component and CFG-Key to HIF HD or
	//       direct to the LSA component. The addressing key is the LSA_COMP_ID
	//       information in RQB. Only components used by application are expected
	switch (comp_id)
	{
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request to HIF HD or
			// direct to CM if HIF HD not used
            #if (PSI_CFG_USE_HIF_HD == 1)
            if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
			    psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr);
            }
            else
            #endif
            {
			    psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_IOH == 1)
		case LSA_COMP_ID_IOH:
		{
            // IOH runs on LD level, delegate request direct to IOH
            psi_request_start(PSI_MBX_ID_IOH, (PSI_REQUEST_FCT)ioh_request, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_DNS == 1)
		case LSA_COMP_ID_DNS:
		{
			// DNS runs on LD level, delegate request direct to DNS
			psi_request_start(PSI_MBX_ID_DNS, (PSI_REQUEST_FCT)dns_request, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_SNMPX:
		{
			// SNMPX runs on LD level, delegate request direct to SNMPX
			psi_request_start(PSI_MBX_ID_SNMPX, (PSI_REQUEST_FCT)snmpx_request, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_SOCK:
		{
			// SOCK runs on LD level, delegate request direct to SOCK
			psi_request_start(PSI_MBX_ID_SOCK, (PSI_REQUEST_FCT)sock_request, rqb_ptr);
		}
		break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD:
		{
			// EDD runs on HD level, delegate request to HIF HD or
			// to PSI Edd user request dispatcher if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
	        EDD_RQB_TYPE          * const edd_rqb_ptr      = (EDD_RQB_TYPE*)(LSA_VOID_PTR_TYPE)rqb_ptr;
	        EDD_HANDLE_LOWER_TYPE   const EDD_Handle_Lower = PSI_EDD_RQB_GET_HANDLE_LOWER(edd_rqb_ptr);
	        LSA_HANDLE_TYPE         const psi_handle       = *((LSA_HANDLE_TYPE*)&EDD_Handle_Lower);

            PSI_ASSERT((LSA_UINT32)EDD_Handle_Lower < PSI_MAX_CHANNELS_PER_COMPONENT);

            if (psi_ld_get_hd_runs_on_ld(comp_id, psi_handle) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
    			psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_request, rqb_ptr);
            }
		}
		break;
        #endif

        #if (PSI_CFG_USE_HD_COMP == 1)
		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request to HIF HD or
			// direct to NARE if HIF HD not used
			#if (PSI_CFG_USE_HIF_HD == 1)
            if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
            {
    			psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr);
            }
            else
			#endif
            {
			    psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, rqb_ptr);
            }
		}
		break;
        #endif

		#if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			//Pass the RQB directly to the LD-EPS if one (or both) of the following is correct:
			//	-there is no HIF_HD (HD-EPS is LD-EPS)
			//  -the RQB is determined for the LD-EPS
			LSA_BOOL meant_for_ld_eps = ((EPS_RQB_PTR_TYPE2)rqb_ptr)->meant_for_ld;

            if (meant_for_ld_eps)
			{
				//RQB determined for LD-EPS
				psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_request, rqb_ptr);
			}
			else
			{
				//RQB determined for HD-EPS
				#if (PSI_CFG_USE_HIF_HD == 1)
                if (psi_ld_get_hd_runs_on_ld(comp_id, PSI_RQB_GET_HANDLE(rqb_ptr)) == PSI_HD_RUNS_ON_LEVEL_LD_NO)
                {
    				psi_request_start(PSI_MBX_ID_HIF_HD, (PSI_REQUEST_FCT)hif_hd_u_request, rqb_ptr);
                }
                else
				#endif
                {
				    //there is no HIF_HD (HD-EPS is LD-EPS)
				    psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_request, rqb_ptr);
                }
			}
		}
		break;
        #endif

		#if (PSI_CFG_USE_HSA == 1)
		case LSA_COMP_ID_HSA:
		{
			// HSA runs on LD level, delegate request direct to HSA
			psi_request_start(PSI_MBX_ID_HSA, (PSI_REQUEST_FCT)hsa_request, rqb_ptr);
		}
		break;
		#endif

        #if (PSI_CFG_USE_SOCKAPP == 1)
        case LSA_COMP_ID_SOCKAPP:
        {
            // SOCKAPP runs on LD level, delegate request direct to SOCKAPP
            psi_request_start(PSI_MBX_ID_SOCKAPP, (PSI_REQUEST_FCT)sockapp_request, rqb_ptr);
        }
        break;
        #endif

		default:
		{
			PSI_LD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_ld_request(): invalid comp_id(%u/%#x), rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
			PSI_FATAL(0);
		}
	}
}

#endif // (PSI_CFG_USE_LD_COMP == 1)

#if (PSI_CFG_USE_HD_COMP == 1)
/*---------------------------------------------------------------------------*/
/* PSI HD user interface                                                     */
/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_system(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
    PSI_ASSERT(rqb_ptr != LSA_NULL);

	PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_system(): rqb_ptr(0x%08x) opcode(0x%x)",
        rqb_ptr, PSI_RQB_GET_OPCODE(rqb_ptr));

	switch (PSI_RQB_GET_OPCODE(rqb_ptr))
	{
	    case PSI_OPC_HD_OPEN_DEVICE:
		{
			PSI_UPPER_HD_OPEN_PTR_TYPE const p_open = &rqb_ptr->args.hd_open;

			if (psi_function_is_null(p_open->psi_request_upper_done_ptr))
			{
                // no callback
				PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_system(): no CBF defined for HD Open, rqb_ptr(0x%08x)", rqb_ptr);
				PSI_FATAL(0);
			}
			else
			{
				// Start Open Device handling
				psi_hd_open_device(rqb_ptr);
			}
		}
		break;

        case PSI_OPC_HD_CLOSE_DEVICE:
		{
			PSI_UPPER_HD_CLOSE_PTR_TYPE const p_close = &rqb_ptr->args.hd_close;

			if (psi_function_is_null(p_close->psi_request_upper_done_ptr))
			{
                // no callback
				PSI_HD_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_system(): no CBF defined for HD Close, rqb_ptr(0x%08x)", rqb_ptr);
				PSI_FATAL(0);
			}
			else
			{
				// Start Close Device handling
				psi_hd_close_device(rqb_ptr);
			}
		}
		break;

        default:
		{
			PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_system(): invalid opcode(0x%x) rqb_ptr(0x%08x)",
				PSI_RQB_GET_OPCODE(rqb_ptr), rqb_ptr);
            PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_open_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_open_channel(): user to comp_id(%u/%#x) rqb_ptr(0x%08x)",
        comp_id, comp_id, rqb_ptr);

	// Handle an open request from upper (HIF HD)
	// Note: the open request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch (comp_id)
	{
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start(PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_open_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_open_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_open_channel, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_DCP:
		{
			// DCP runs on HD level, delegate request direct to DCP
			psi_request_start(PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_open_channel, rqb_ptr);
		}
		break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDI == 1)
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS (here always EDDS RQB context L1 is used)
			psi_request_start(PSI_MBX_ID_EDDS_RQB_L1, (PSI_REQUEST_FCT)edds_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			// EDDT runs on HD level, delegate request direct to EDDT
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_GSY == 1)
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start(PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_open_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start(PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_open_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_MRP == 1)
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start(PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_open_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_open_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_POF == 1)
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start(PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_open_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			// If a RQB for EPS arrives on HD-level it is always meant for HD-EPS
			psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_open_channel, rqb_ptr);
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_open_channel(): invalid comp_id(%u/%#x), rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
			PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_close_channel(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "psi_hd_close_channel(): user to comp_id(%u/%#x) rqb_ptr(0x%08x)",
        comp_id, comp_id, rqb_ptr);

	// Handle a close request from upper (HIF HD)
	// Note: the close request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch (comp_id)
	{
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start(PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_close_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_close_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_close_channel, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_DCP:
		{
			// DCP runs on HD level, delegate request direct to DCP
			psi_request_start(PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_close_channel, rqb_ptr);
		}
		break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDI == 1)
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS (here always EDDS RQB context L1 is used)
			psi_request_start(PSI_MBX_ID_EDDS_RQB_L1, (PSI_REQUEST_FCT)edds_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			// EDDT runs on HD level, delegate request direct to EDDT
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_GSY == 1)
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start(PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_close_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start(PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_close_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_MRP == 1)
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start(PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_close_channel, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_close_channel, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_close_channel, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_POF == 1)
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start(PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_close_channel, rqb_ptr);
		}
		break;
        #endif

		#if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			// If a RQB for EPS arrives on HD-level it is always meant for HD-EPS
			psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_close_channel, rqb_ptr);
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_close_channel(): invalid comp_id(%u/%#x), rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
			PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_request(
    PSI_RQB_PTR_TYPE const rqb_ptr)
{
	LSA_COMP_ID_TYPE comp_id;

    PSI_ASSERT(rqb_ptr != LSA_NULL);

    comp_id = PSI_RQB_GET_COMP_ID(rqb_ptr);

	PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "psi_hd_request(): rqb_ptr(0x%08x) comp_id(%u/%#x)",
        rqb_ptr, comp_id, comp_id);

	// Handle an service request from upper (HIF HD)
	// Note: the service request is delegated to LSA component based on CFG-Key.
	//       The addressing key is the LSA_COMP_ID information in RQB.
	//       (SNMPX, SOCK, TCIP not include runs only on LD)
	switch (comp_id)
	{
		case LSA_COMP_ID_ACP:
		{
			// ACP runs on HD level, delegate request direct to ACP
			psi_request_start(PSI_MBX_ID_ACP, (PSI_REQUEST_FCT)acp_request, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_CLRPC:
		{
			// CLRPC runs on LD level, delegate request direct to CLRPC
			// Note: in case of CM user channel to CLRPC, the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_CLRPC, (PSI_REQUEST_FCT)clrpc_request, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_CM:
		{
			// CM runs on HD level, delegate request direct to CM
			psi_request_start(PSI_MBX_ID_CM, (PSI_REQUEST_FCT)cm_request, rqb_ptr);
		}
		break;

		case LSA_COMP_ID_DCP:
	    {
		    // DCP runs on HD level, delegate request direct to DCP
		    psi_request_start(PSI_MBX_ID_DCP, (PSI_REQUEST_FCT)dcp_request, rqb_ptr);
	    }
	    break;

        #if ((PSI_CFG_USE_EDDI == 1) || (PSI_CFG_USE_EDDP == 1) || (PSI_CFG_USE_EDDS == 1) || (PSI_CFG_USE_EDDT == 1))
		case LSA_COMP_ID_EDD: 
		{
			// PSI EDD Dispatcher runs on HD level, delegate request directly to PSI
			psi_request_start(PSI_MBX_ID_PSI, (PSI_REQUEST_FCT)psi_hd_edd_user_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDI == 1)
		case LSA_COMP_ID_EDDI:
		{
			// EDDI runs on HD level, delegate request direct to EDDI
			psi_request_start(PSI_MBX_ID_EDDI_REST, (PSI_REQUEST_FCT)eddi_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDP == 1)
		case LSA_COMP_ID_EDDP:
		{
			// EDDP runs on HD level, delegate request direct to EDDP
			psi_request_start(PSI_MBX_ID_EDDP_LOW, (PSI_REQUEST_FCT)eddp_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDS == 1)
		case LSA_COMP_ID_EDDS:
		{
			// EDDS runs on HD level, delegate request direct to EDDS (here always EDDS RQB context L1 is used)
			psi_request_start(PSI_MBX_ID_EDDS_RQB_L1, (PSI_REQUEST_FCT)edds_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_EDDT == 1)
		case LSA_COMP_ID_EDDT:
		{
			// EDDT runs on HD level, delegate request direct to EDDT
			psi_request_start(PSI_MBX_ID_EDDT_LOW, (PSI_REQUEST_FCT)eddt_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_GSY == 1)
		case LSA_COMP_ID_GSY:
		{
			// GSY runs on HD level, delegate request direct to GSY
			psi_request_start(PSI_MBX_ID_GSY, (PSI_REQUEST_FCT)gsy_request, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_LLDP:
		{
			// LLDP runs on HD level, delegate request direct to LLDP
			psi_request_start(PSI_MBX_ID_LLDP, (PSI_REQUEST_FCT)lldp_request, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_MRP == 1)
		case LSA_COMP_ID_MRP:
		{
			// MRP runs on HD level, delegate request direct to MRP
			psi_request_start(PSI_MBX_ID_MRP, (PSI_REQUEST_FCT)mrp_request, rqb_ptr);
		}
		break;
        #endif

		case LSA_COMP_ID_NARE:
		{
			// NARE runs on HD level, delegate request direct to NARE
			psi_request_start(PSI_MBX_ID_NARE, (PSI_REQUEST_FCT)nare_request, rqb_ptr);
		}
		break;

        #if (PSI_CFG_USE_LD_COMP == 1)
		case LSA_COMP_ID_OHA:
		{
			// OHA runs on LD level, delegate request direct to OHA
			// Note: in case of CM user channel to OHA the request is done by
			//       PSI HD on LD side called by HIF HD from lower
			psi_request_start(PSI_MBX_ID_OHA, (PSI_REQUEST_FCT)oha_request, rqb_ptr);
		}
		break;
        #endif

        #if (PSI_CFG_USE_POF == 1)
		case LSA_COMP_ID_POF:
		{
			// POF runs on HD level, delegate request direct to POF
			psi_request_start(PSI_MBX_ID_POF, (PSI_REQUEST_FCT)pof_request, rqb_ptr);
		}
		break;
        #endif

		#if (PSI_CFG_USE_EPS_RQBS == 1)
		case LSA_COMP_ID_PNBOARDS:
		{
			// If a RQB for EPS arrives on HD-level it is always meant for HD-EPS
			psi_request_start(PSI_MBX_ID_EPS, (PSI_REQUEST_FCT)eps_request, rqb_ptr);
		}
		break;
        #endif

		default:
		{
			PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_hd_request(): invalid comp_id(%u/%#x), rqb_ptr(0x%08x)",
				comp_id, comp_id, rqb_ptr);
			PSI_FATAL(0);
		}
	}
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_hd_get_edd_handle(
    LSA_UINT16     const hd_nr, 
    PSI_EDD_HDDB * const edd_handle_ptr)
{
	PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

	*edd_handle_ptr = psi_hd_get_hDDB(hd_nr);
}

#if (PSI_CFG_USE_LD_COMP == 1)
/*---------------------------------------------------------------------------*/
LSA_UINT32 psi_get_real_pi_size(
    LSA_UINT16 const hd_nr)
{
    LSA_UINT32 pi_size;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    pi_size = psi_ld_get_real_pi_size(hd_nr);

    return pi_size;
}
#endif // (PSI_CFG_USE_LD_COMP == 1)

/*---------------------------------------------------------------------------*/
/* function for POF port auto detection */
LSA_VOID psi_pof_port_auto_detection(
    LSA_UINT16 * const ret_val_ptr,
    LSA_UINT16   const hd_nr,
    LSA_UINT32   const HwPortID,
    LSA_UINT8  * const pMediaType,
    LSA_UINT8  * const pIsPOF,
    LSA_UINT8  * const pFXTransceiverType)
{
    /* I/O - parameter - check */
    PSI_ASSERT(ret_val_ptr != LSA_NULL);
    PSI_ASSERT(pMediaType != LSA_NULL);
    PSI_ASSERT(pIsPOF != EDD_PORT_OPTICALTYPE_ISNONPOF);
    PSI_ASSERT(pFXTransceiverType != LSA_NULL);

    /* INPUT - parameter - check */
    if ((HwPortID < 1) || (HwPortID > PSI_CFG_MAX_PORT_CNT)) // HwPortID in valid range?
    {
        *ret_val_ptr = LSA_RET_ERR_PARAM; // parameter-error
        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_pof_port_auto_detection(): invalid HwPortID(%u), hd_nr(%u)", HwPortID, hd_nr);
    }
    else if ((hd_nr < 1) || (hd_nr > PSI_CFG_MAX_IF_CNT)) // hd_nr in valid range?
    {
        *ret_val_ptr = LSA_RET_ERR_PARAM; // parameter-error
        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "psi_pof_port_auto_detection(): invalid hd_nr(%u), HwPortID(%u)", hd_nr, HwPortID);
    }
    else
    {
        *ret_val_ptr = LSA_RET_OK; // all ok -> Auto POF detection
    }

    /* Auto POF detection */
    if (*ret_val_ptr == LSA_RET_OK)
    {
        LSA_UINT32                  UserPortID;
        PSI_HD_STORE_PTR_TYPE const p_hd = psi_hd_get_hd_store(hd_nr);

        PSI_ASSERT(p_hd != LSA_NULL);
        PSI_ASSERT(p_hd->is_used);

        // calculate UserPortID
        for (UserPortID = 1; UserPortID <= p_hd->nr_of_ports; UserPortID++)
        {
            if (HwPortID == p_hd->hd_hw.port_map[UserPortID].hw_port_id)
            {
                break;
            }
        }

       switch (p_hd->hd_hw.edd_type)
       {
            #if (PSI_CFG_USE_EDDI == 1)
            case LSA_COMP_ID_EDDI:
            {
                EDD_RSP                        RetVal;
                LSA_BOOL                       bTransceiverFound          = LSA_FALSE;
                char                           I2cBuf[PSI_TRANSCEIVER_LENGTH_PART_NUMBER];
                char                    const  PartNumber_QFBR_5978AZ[]   = "QFBR-5978AZ";
                char                    const  PartNumber_QFBR_5978AZ_2[] = "QFBR-5978AZ-2";
				char                    const  PartNumber_AFBR_59E4APZ[]  = "AFBR-59E4APZ";

                //execute POF port auto detection via dummy-I2C-access

                //execute dummy I2C read to Avago transceiver

                //call I2C function of EDDI
                I2cBuf[0] = '0';
                RetVal = eddi_I2C_Read_Offset( p_hd->edd_hDDB,                      //hDDB = Device handle (refer to service EDDI_SRV_DEV_OPEN)
                                              UserPortID,                           //PortId
                                              (LSA_UINT16) HwPortID,                //I2CMuxSelect = HwPortId => for external I2C HW multiplexer
                                              PSI_TRANSCEIVER_ADR_PAGE_A0,          //I2CDevAddr
                                              1,                                    //I2COffsetCnt
                                              PSI_TRANSCEIVER_OFFSET_PART_NUMBER,   //I2COffset1 = POF_ADR_PART_NUMBER_0
                                              0,                                    //I2COffset2
                                              1,                                    //RetryCnt
                                              0,                                    //RetryTime_us
                                              PSI_TRANSCEIVER_LENGTH_PART_NUMBER,   //Size
                                              (LSA_UINT8 *)I2cBuf);                 //pBuf

                PSI_ASSERT(EDD_STS_ERR_PARAM != RetVal);

                if //I2C transfer correct?
                   (RetVal == EDD_STS_OK)
                {
                    //check transceiver type
					if (0 == strncmp(I2cBuf, PartNumber_QFBR_5978AZ_2, strlen(PartNumber_QFBR_5978AZ_2)))
					{
						//Avago POF transceiver QFBR-5978AZ found
						bTransceiverFound = LSA_TRUE;

						//POF port (Fiber port)
						*pMediaType = PSI_MEDIA_TYPE_FIBER;
						*pIsPOF = EDD_PORT_OPTICALTYPE_ISPOF;
						*pFXTransceiverType = EDD_FX_TRANSCEIVER_QFBR5978_2;

						PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => POF-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_QFBR5978_2",
							UserPortID, HwPortID);
					}
					else if (0 == strncmp(I2cBuf, PartNumber_QFBR_5978AZ, strlen(PartNumber_QFBR_5978AZ)))
                    {
                        //Avago POF transceiver QFBR-5978AZ found
                        bTransceiverFound = LSA_TRUE;

                        //POF port (Fiber port)
                        *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISPOF;
                        *pFXTransceiverType = EDD_FX_TRANSCEIVER_QFBR5978;

                        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => POF-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_QFBR5978",
                            UserPortID, HwPortID);
                    }
                    else if (0 == strncmp(I2cBuf, PartNumber_AFBR_59E4APZ, strlen(PartNumber_AFBR_59E4APZ)))
                    {
                        //Avago FX transceiver AFBR-59E4APZ found
                        bTransceiverFound = LSA_TRUE;

                        //FX port (Fiber port)
                        *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                        *pFXTransceiverType = EDD_FX_TRANSCEIVER_AFBR59E4APZ;

                        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => FX-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_AFBR59E4APZ",
                            UserPortID, HwPortID);
                    }
                }

                if (!bTransceiverFound)
                {
                    //Avago FX transceiver: Maybe EEPROM with 16bit address? read again!

                    //call I2C function of EDDI
                    I2cBuf[0] = '0';
                    RetVal = eddi_I2C_Read_Offset(p_hd->edd_hDDB,                       //hDDB = Device handle (refer to service EDDI_SRV_DEV_OPEN)
                                                  UserPortID,                           //PortId
                                                  (LSA_UINT16) HwPortID,                //I2CMuxSelect = HwPortId => for external I2C HW multiplexer
                                                  PSI_TRANSCEIVER_ADR_PAGE_A0,          //I2CDevAddr
                                                  2,                                    //I2COffsetCnt
                                                  0,                                    //I2COffset1 = 0
                                                  PSI_TRANSCEIVER_OFFSET_PART_NUMBER,   //I2COffset2 = POF_ADR_PART_NUMBER_0
                                                  1,                                    //RetryCnt
                                                  0,                                    //RetryTime_us
                                                  PSI_TRANSCEIVER_LENGTH_PART_NUMBER,   //Size
                                                  (LSA_UINT8 *)I2cBuf);                 //pBuf

                    PSI_ASSERT(EDD_STS_ERR_PARAM != RetVal);

                    if //I2C transfer correct?
                       (RetVal == EDD_STS_OK)
                    {
                        //check transceiver type
                        if (0 == strncmp(I2cBuf, PartNumber_AFBR_59E4APZ, strlen(PartNumber_AFBR_59E4APZ)))
                        {
                            //Avago FX transceiver AFBR-59E4APZ found
                            bTransceiverFound = LSA_TRUE;

                            //FX port (Fiber port)
                            *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                            *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                            *pFXTransceiverType = EDD_FX_TRANSCEIVER_AFBR59E4APZ;

                            PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok (FX-EEPROM with 16bit address) => FX-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_AFBR59E4APZ",
                                UserPortID, HwPortID);
                        }
                    }
                }

                if (!bTransceiverFound)
                {
                    //Copper port
                    *pMediaType         = PSI_MEDIA_TYPE_COPPER;
                    *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                    *pFXTransceiverType = EDD_FX_TRANSCEIVER_UNKNOWN; //sanity

                    PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: no I2C transceiver found => CU-Port, UserPortID(%u) HwPortId(%u)",
                        UserPortID, HwPortID);
                }
            }
            break;
            #endif

            #if (PSI_CFG_USE_EDDP == 1)
            case LSA_COMP_ID_EDDP:
            {
                EDD_RSP                        RetVal;
                LSA_BOOL                       bTransceiverFound          = LSA_FALSE;
                char                           I2cBuf[PSI_TRANSCEIVER_LENGTH_PART_NUMBER];
                char                    const  PartNumber_QFBR_5978AZ[]   = "QFBR-5978AZ";
                char                    const  PartNumber_QFBR_5978AZ_2[] = "QFBR-5978AZ-2";
                char                    const  PartNumber_AFBR_59E4APZ[]  = "AFBR-59E4APZ";

                //execute POF port auto detection via dummy-I2C-access

                //call I2C function of EDDP
                I2cBuf[0] = '0';
                RetVal = eddp_I2C_Read_Offset(p_hd->edd_hDDB,                       //hDDB = DDB handle (refer to service EDDP_SRV_DEV_OPEN)
                                              UserPortID,                           //PortId
                                              0,                                    //I2CMuxSelect = 0 => no external I2C HW multiplexer present
                                              PSI_TRANSCEIVER_ADR_PAGE_A0,          //I2CDevAddr
                                              1,                                    //I2COffsetCnt
                                              PSI_TRANSCEIVER_OFFSET_PART_NUMBER,   //I2COffset1 = POF_ADR_PART_NUMBER_0
                                              0,                                    //I2COffset2
                                              1,                                    //RetryCnt
                                              0,                                    //RetryTime_us
                                              PSI_TRANSCEIVER_LENGTH_PART_NUMBER,   //Size
                                              (LSA_UINT8 *)I2cBuf);                 //pBuf

                PSI_ASSERT(EDD_STS_ERR_PARAM != RetVal);

                if //I2C transfer correct?
                   (RetVal == EDD_STS_OK)
                {
                    //check transceiver type
                    if (0 == strncmp(I2cBuf, PartNumber_QFBR_5978AZ_2, strlen(PartNumber_QFBR_5978AZ_2)))
                    {
                        //Avago POF transceiver QFBR-5978AZ found
                        bTransceiverFound = LSA_TRUE;

                        //POF port (Fiber port)
                        *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISPOF;
                        *pFXTransceiverType = EDD_FX_TRANSCEIVER_QFBR5978_2;

                        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => POF-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_QFBR5978_2",
                            UserPortID, HwPortID);
                    }
                    else if (0 == strncmp(I2cBuf, PartNumber_QFBR_5978AZ, strlen(PartNumber_QFBR_5978AZ)))
                    {
                        //Avago POF transceiver QFBR-5978AZ found
                        bTransceiverFound = LSA_TRUE;

                        //POF port (Fiber port)
                        *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISPOF;
                        *pFXTransceiverType = EDD_FX_TRANSCEIVER_QFBR5978; //sanity

                        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => POF-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_QFBR5978",
                            UserPortID, HwPortID);
                    }
                    else if (0 == strncmp(I2cBuf, PartNumber_AFBR_59E4APZ, strlen(PartNumber_AFBR_59E4APZ)))
                    {
                        //Avago FX transceiver AFBR-59E4APZ found
                        bTransceiverFound = LSA_TRUE;

                        //FX port (Fiber port)
                        *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                        *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                        *pFXTransceiverType = EDD_FX_TRANSCEIVER_AFBR59E4APZ; //sanity

                        PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok => FX-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_AFBR59E4APZ",
                            UserPortID, HwPortID);
                    }
                }

                if (!bTransceiverFound)
                {
                    //Avago FX transceiver: Maybe EEPROM with 16bit address? read again!

                    //call I2C function of EDDP
                    I2cBuf[0] = '0';
                    RetVal = eddp_I2C_Read_Offset(p_hd->edd_hDDB,                       //hDDB = DDB handle (refer to service EDDP_SRV_DEV_OPEN)
                                                  UserPortID,                           //PortId
                                                  0,                                    //I2CMuxSelect = 0 => no external I2C HW multiplexer present
                                                  PSI_TRANSCEIVER_ADR_PAGE_A0,          //I2CDevAddr
                                                  2,                                    //I2COffsetCnt
                                                  0,                                    //I2COffset1 = 0
                                                  PSI_TRANSCEIVER_OFFSET_PART_NUMBER,   //I2COffset2 = POF_ADR_PART_NUMBER_0
                                                  1,                                    //RetryCnt
                                                  0,                                    //RetryTime_us
                                                  PSI_TRANSCEIVER_LENGTH_PART_NUMBER,   //Size
                                                  (LSA_UINT8 *)I2cBuf);                 //pBuf

                    PSI_ASSERT(EDD_STS_ERR_PARAM != RetVal);

                    if //I2C transfer correct?
                       (RetVal == EDD_STS_OK)
                    {
                        //check transceiver type
                        if (0 == strncmp(I2cBuf, PartNumber_AFBR_59E4APZ, strlen(PartNumber_AFBR_59E4APZ)))
                        {
                            //Avago FX transceiver AFBR-59E4APZ found
                            bTransceiverFound = LSA_TRUE;

                            //FX port (Fiber port)
                            *pMediaType         = PSI_MEDIA_TYPE_FIBER;
                            *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                            *pFXTransceiverType = EDD_FX_TRANSCEIVER_AFBR59E4APZ; //sanity

                            PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: I2C ok (FX-EEPROM with 16bit address) => FX-Fiber-Port, UserPortID(%u) HwPortId(%u) FX_TRANSCEIVER_AFBR59E4APZ",
                                UserPortID, HwPortID);
                        }
                    }
                }

                if (!bTransceiverFound)
                {
                    //Copper port
                    *pMediaType         = PSI_MEDIA_TYPE_COPPER;
                    *pIsPOF             = EDD_PORT_OPTICALTYPE_ISNONPOF;
                    *pFXTransceiverType = EDD_FX_TRANSCEIVER_UNKNOWN; //sanity

                    PSI_HD_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "psi_pof_port_auto_detection(): AutoPOF: no I2C transceiver found => CU-Port, UserPortID(%u) HwPortId(%u)",
                        UserPortID, HwPortID);
                }
            }
            break;
            #endif
           
            default:
            {
                PSI_HD_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "psi_pof_port_auto_detection(): invalid edd_type(0x%08x), hd_nr(%u), HwPortID(%u)",
                    p_hd->hd_hw.edd_type, hd_nr, HwPortID);
                PSI_FATAL(0);
            }
       } /* end of switch by edd_type */
    } /* end of LSA_RET_OK */
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_usr_alloc_nrt_tx_mem(
    LSA_VOID_PTR_TYPE * const mem_ptr_ptr,
    LSA_UINT32          const length,
    LSA_UINT16          const hd_nr,
    LSA_UINT16		    const comp_id)
{
    LSA_UINT32 nrt_tx_length = length;

    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    switch (comp_id)
    {
        #if (PSI_CFG_USE_EDDI == 1)
        case LSA_COMP_ID_EDDI:
        {
            #if defined(PSI_CFG_EDDI_CFG_ENABLE_NRT_AUTOALIGNMENT)
            if (nrt_tx_length < 64UL)
            {
                // padding to 4-byte-aligned length (EDDI specific bug)
                if (nrt_tx_length & 3UL)
                {
                    nrt_tx_length = (LSA_UINT32)(nrt_tx_length & (~3UL)) + 4UL;
                }
            }
            #endif
        }
		// !!! no break, the (patched) "nrt_tx_length" of EDDI goes on in EDDP case !!!
        //lint --e(825) -fallthrough
        #endif //PSI_CFG_USE_EDDI

        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_nr);
            // allocate NRT buffer from NRT memory pool
            PSI_NRT_ALLOC_TX_MEM( mem_ptr_ptr, nrt_tx_length, nrt_pool_handle, comp_id );

			PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_alloc_nrt_tx_mem(): mem_ptr(0x%08x) length(%u) hd_nr(%u) comp_id(%u/%#x)",
				*mem_ptr_ptr, nrt_tx_length, hd_nr, comp_id, comp_id );
        }
        break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
        {
            if (psi_hd_is_edd_nrt_copy_if_on(hd_nr))
            {
	            LSA_USER_ID_TYPE null_usr_id;
                PSI_INIT_USER_ID_UNION(null_usr_id);
                // allocate NRT buffer from local memory pool
                PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, nrt_tx_length, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_nr);
                if (nrt_tx_length < 60UL)
                {
                    PSI_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_NOTE, "psi_usr_alloc_nrt_tx_mem(): EDDS NRT TX buffer is increased from length(%u) to 60 bytes", nrt_tx_length );
                    nrt_tx_length = 60UL; //set minimum length required by TI AM5728 GMAC
                }
                // allocate NRT buffer from NRT memory pool
                PSI_NRT_ALLOC_TX_MEM( mem_ptr_ptr, nrt_tx_length, nrt_pool_handle, comp_id );
            }

			PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_alloc_nrt_tx_mem(): mem_ptr(0x%08x) length(%u) hd_nr(%u) comp_id(%u/%#x)",
				*mem_ptr_ptr, nrt_tx_length, hd_nr, comp_id, comp_id );
        }
        break;
        #endif //PSI_CFG_USE_EDDS

        default:
        {
		    PSI_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "psi_usr_alloc_nrt_tx_mem(): invalid comp_id(%u/%#x)", comp_id, comp_id);
            *mem_ptr_ptr = LSA_NULL;
		    PSI_FATAL(0);
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_usr_alloc_nrt_rx_mem(
    LSA_VOID_PTR_TYPE * const mem_ptr_ptr, 
    LSA_UINT32		    const length,
    LSA_UINT16          const hd_nr,
    LSA_UINT16		    const comp_id)
{
    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    if (length != EDD_FRAME_BUFFER_LENGTH)
    {
		PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_usr_alloc_nrt_rx_mem(): invalid length(%u) != EDD_FRAME_BUFFER_LENGTH(%u)",
			length, EDD_FRAME_BUFFER_LENGTH );
        *mem_ptr_ptr = LSA_NULL;
		PSI_FATAL(0);
    }

    switch (comp_id)
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_nr);
            // allocate NRT buffer from NRT memory pool
            PSI_NRT_ALLOC_RX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );

			PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_alloc_nrt_rx_mem(): mem_ptr(0x%08x) length(%u) hd_nr(%u) comp_id(%u/%#x)",
				*mem_ptr_ptr, length, hd_nr, comp_id, comp_id );
        }
        break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
        {
            if (psi_hd_is_edd_nrt_copy_if_on(hd_nr))
            {
	            LSA_USER_ID_TYPE null_usr_id;
                PSI_INIT_USER_ID_UNION(null_usr_id);
                // allocate NRT buffer from local memory pool
	            PSI_ALLOC_LOCAL_MEM( mem_ptr_ptr, null_usr_id, length, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_nr);
                // allocate NRT buffer from NRT memory pool
                PSI_NRT_ALLOC_RX_MEM( mem_ptr_ptr, length, nrt_pool_handle, comp_id );
            }

			PSI_SYSTEM_TRACE_05( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_alloc_nrt_rx_mem(): mem_ptr(0x%08x) length(%u) hd_nr(%u) comp_id(%u/%#x)",
				*mem_ptr_ptr, length, hd_nr, comp_id, comp_id );
        }
        break;
        #endif //PSI_CFG_USE_EDDS

        default:
        {
		    PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_usr_alloc_nrt_rx_mem(): invalid comp_id(%u/%#x)", comp_id, comp_id );
            *mem_ptr_ptr = LSA_NULL;
		    PSI_FATAL(0);
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_usr_free_nrt_tx_mem(
    LSA_UINT16        * const ret_val_ptr,
    LSA_VOID_PTR_TYPE	const mem_ptr,
    LSA_UINT16          const hd_nr,
    LSA_UINT16		    const comp_id)
{
    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    switch (comp_id)
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_nr);

			PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_free_nrt_tx_mem(): mem_ptr(0x%08x) hd_nr(%u) comp_id(%u/%#x)",
				mem_ptr, hd_nr, comp_id, comp_id );

            // free NRT buffer to NRT memory pool
            PSI_NRT_FREE_TX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
        }
        break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
        {
			PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_free_nrt_tx_mem(): mem_ptr(0x%08x) hd_nr(%u) comp_id(%u/%#x)",
				mem_ptr, hd_nr, comp_id, comp_id );

            if (psi_hd_is_edd_nrt_copy_if_on(hd_nr))
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_tx_pool_handle(hd_nr);
			    // free NRT buffer to NRT memory pool
                PSI_NRT_FREE_TX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
            }
        }
        break;
        #endif //PSI_CFG_USE_EDDS

        default:
        {
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_usr_free_nrt_tx_mem(): invalid comp_id(%u/%#x)",
				comp_id, comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
		    PSI_FATAL(0);
        }
    }
}

/*---------------------------------------------------------------------------*/
LSA_VOID psi_usr_free_nrt_rx_mem(
    LSA_UINT16        * const ret_val_ptr,
    LSA_VOID_PTR_TYPE   const mem_ptr,
    LSA_UINT16          const hd_nr,
    LSA_UINT16		    const comp_id)
{
    PSI_ASSERT(hd_nr && (hd_nr <= PSI_CFG_MAX_IF_CNT));

    switch (comp_id)
    {
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        {
	        LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_nr);

			PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_free_nrt_rx_mem(): mem_ptr(0x%08x) hd_nr(%u) comp_id(%u/%#x)",
				mem_ptr, hd_nr, comp_id, comp_id );

            // free NRT buffer to NRT memory pool
            PSI_NRT_FREE_RX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
        }
        break;

        #if (PSI_CFG_USE_EDDS == 1)
        case LSA_COMP_ID_EDDS:
        {
			PSI_SYSTEM_TRACE_04( 0, LSA_TRACE_LEVEL_CHAT, "psi_usr_free_nrt_rx_mem(): mem_ptr(0x%08x) hd_nr(%u) comp_id(%u/%#x)",
				mem_ptr, hd_nr, comp_id, comp_id );

            if (psi_hd_is_edd_nrt_copy_if_on(hd_nr))
            {
			    // free NRT buffer to local memory pool
		        PSI_FREE_LOCAL_MEM( ret_val_ptr, mem_ptr, 0, comp_id, PSI_MTYPE_NRT_MEM );
            }
            else // edd nrt copy interface off
            {
	            LSA_INT const nrt_pool_handle = psi_hd_get_nrt_rx_pool_handle(hd_nr);
			    // free NRT buffer to NRT memory pool
                PSI_NRT_FREE_RX_MEM( ret_val_ptr, mem_ptr, nrt_pool_handle, comp_id );
            }
        }
        break;
        #endif //PSI_CFG_USE_EDDS

        default:
        {
			PSI_SYSTEM_TRACE_02( 0, LSA_TRACE_LEVEL_FATAL, "psi_usr_free_nrt_rx_mem(): invalid comp_id(%u/%#x)",
				comp_id, comp_id );
            *ret_val_ptr = LSA_RET_ERR_PARAM;
		    PSI_FATAL(0);
        }
    }
}

/*---------------------------------------------------------------------------*/
// Attention: This function should only be used after startup (Detail storage is already set)!
LSA_UINT8 psi_get_hd_runs_on_ld(
    LSA_UINT16 const hd_nr)
{
    #if defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
    LSA_UNUSED_ARG(hd_nr);
    return PSI_HD_RUNS_ON_LEVEL_LD_NO;
    #else
    PSI_DETAIL_STORE_PTR_TYPE pDetailsIF;

    if (hd_nr == PSI_HD_CURRENT_HD) // get the value stored in current hd instance
    {
        return psi_hd_get_runs_on_level_ld();
    }
    else
    {
        PSI_ASSERT(hd_nr <= PSI_CFG_MAX_IF_CNT);

        pDetailsIF = psi_get_detail_store(hd_nr);

        return (pDetailsIF->hd_runs_on_level_ld);
    }
    #endif
}

#endif // (PSI_CFG_USE_HD_COMP == 1)

#endif // ((PSI_CFG_USE_LD_COMP == 1) || (PSI_CFG_USE_HD_COMP == 1))

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

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
/*  C o m p o n e n t     &C: HIF (Host Interface)                      :C&  */
/*                                                                           */
/*  F i l e               &F: hif_pipe.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements HIF Pipe/Channel handling                                     */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	5
#define HIF_MODULE_ID		LTRC_ACT_MODUL_ID

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

#if (HIF_CFG_USE_HIF == 1)

/** 
 * \brief Handles open_channel for HIF upper
 * 
 * (1) Sets the pipe state
 * (2) Replaces the handle in the RQB in the channel 
 * (3) Returns the open channel RQB to the originator.
 * 
 * @param [in] pPipe    - current pipe
 * @param [in] pRQB     - open channel RQB
 */
LSA_VOID hif_upper_open_channel_done(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
	/* Open channel done? */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_open_channel_done(+) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

		/* (1) Update pipe state */
		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENED_UPPER);

		/* (2) Replace handle */
		pPipe->func.pCompFuncs->ReplaceOpenChannelLowerHandle(pRQB, pPipe->d.upper.ch_handle->hHifCompCh);
	}
	else
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_open_channel_done(-) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}

	/* Callback originator */
	hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));	
}

/**
 * @brief Handles close_channel for HIF Lower.
 *
 * (1) Save UserID from the RQB
 * (2) Sets it's own handle
 * (3) Triggers the CLOSE_CHANNEL_LOWER function
 *
 * @param [in] pPipe    - current pipe
 * @param [in] pRQB     - close channel RQB
 */
LSA_VOID hif_lower_close_channel(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
    LSA_COMP_ID_TYPE const comp_id = pPipe->comp_id;

	HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

	/* (1) remember the source of the RQB */
	hif_lower_save_return_path_to_rqb(pPipe, pRQB);

	/* (2)  close channel request received from upper */
    switch(comp_id)
    {
	    #if (HIF_CFG_USE_EDD == 1)
        case LSA_COMP_ID_EDD:
        //case LSA_COMP_ID_EDDI: //cannot occur in pipe
        //case LSA_COMP_ID_EDDP: //cannot occur in pipe
        //case LSA_COMP_ID_EDDS: //cannot occur in pipe
        //case LSA_COMP_ID_EDDT: //cannot occur in pipe
        {
	        EDD_RQB_TYPE * const pEddRQB = (EDD_RQB_TYPE*)HIF_CAST_TO_VOID_PTR(pRQB);
	        HIF_EDD_RQB_SET_HANDLE_LOWER(pEddRQB, pPipe->d.lower.lower_orig_comp_edd_ch_handle);
            break;
        }
        #endif
        default:
        {
            HIF_ASSERT(comp_id < HIF_MAX_COMP_ID);
	        HIF_RQB_SET_HANDLE(pRQB, pPipe->d.lower.lower_orig_comp_ch_handle);
        }
    }

	/* (3) request lower component */
	pPipe->pInstance->func.CLOSE_CHANNEL_LOWER(pRQB, pPipe->pInstance->hSysDev);
}

/**
 * @brief Handles close_channel_done
 * 
 * On success...
 * (1) The RQB is returned to the originator with LSA_RET_OK 
 * (2) The Pipe is set to the state HIF_PIPE_STATE_CLOSED 
 * (3) The PATH info struture is freed
 *  
 * On failure, the RQB is returned to the originator with retVal != LSA_RET_OK
 * 
 * @param [in] pPipe    - current pipe
 * @param [in] pRQB     - close channel RQB
 */
LSA_VOID hif_upper_close_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	/* Close channel done? */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_close_channel_done(+) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);
		/* (1) Callback */
		hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));
		
		/* (2) Update pipe state */
		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);

		/* (3) Free path info */
		pPipe->func.RELEASE_PATH_INFO(&retVal, pPipe->d.upper.ch_handle->sys_ptr, pPipe->d.upper.ch_handle->pDetails);

		hif_free_ch_handle(pPipe->d.upper.ch_handle);
		pPipe->d.upper.ch_handle = LSA_NULL;

		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_upper_close_channel_done() - RELEASE_PATH_INFO returned failure (0x%x)", retVal);
			HIF_FATAL(0);
		}
	}
	else
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_close_channel_done(-) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);
		
		hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));
	}
}

/**
 * @brief Handles open channel for HIF Lower
 * 
 * (1) Save UserID from the RQB
 * (2) Set Pipe state to HIF_PIPE_STATE_OPENING_LOWER
 * (3) Sets its own handle
 * (4) Call OPEN_CHANNEL_LOWER for this pipe
 * 
 * @param [in] pPipe - current pipe
 * @param [in] pRQB  - open channel RQB
 */
LSA_VOID hif_lower_open_channel(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

	/* (1) remember the source of the RQB */
	hif_lower_save_return_path_to_rqb(pPipe, pRQB);

	/* (2) Adjust Pipe state to opening */
	hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENING_LOWER);

	/* (3) Only open_channel requests valid */	
	retVal = pPipe->func.pCompFuncs->ReplaceOpenChannelCbf(pRQB, hif_lower_request_done);
	if(retVal != LSA_RET_OK)
	{
		/* somethings wrong with the rqb -> should not happen here */
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_lower_open_channel() - ReplaceOpenChannelCbf returned failure (0x%x)", retVal);
		HIF_FATAL(0);
	}

	pPipe->pInstance->func.OPEN_CHANNEL_LOWER(pRQB, pPipe->pInstance->hSysDev);
}

/**
 * @brief Handles close channel for HIF Lower
 * 
 * (1) Sets pipe state to HIF_PIPE_STATE_CLOSED
 * (2) Send RQB back to HIF Upper
 * 
 * @param [in] pPipe    - current pipe
 * @param [in] pRQB     - current RQB
 */
LSA_VOID hif_lower_close_channel_done(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
	/* Close channel RQB! */

	/* (1) Close pipe if close channel was successfull */
	if(HIF_RQB_GET_RESPONSE((HIF_RQB_PTR_TYPE)pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel_done(+) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}
	else
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_close_channel_done(-) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);
	}

	/* (2) return RQB to hif upper */
	hif_send(pPipe, pRQB);
}

/**
 * @brief Handles open channel done for HIF Lower
 * 
 * (1) Set pipe state to HIF_PIPE_STATE_OPENED_LOWER
 * (2) Send RQB to HIF Upper
 * 
 * @param [in] pPipe    - current pipe
 * @param [in] pRQB     - open channel RQB
 */
LSA_VOID hif_lower_open_channel_done(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	/* Open Channel RQB! */

	/* Check if Pipe Open was successfull */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel_done(+) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

        #if (HIF_CFG_USE_EDD == 1)
        switch(HIF_RQB_GET_COMP_ID(pRQB))
        {
            case LSA_COMP_ID_EDD:
            case LSA_COMP_ID_EDDI:
            case LSA_COMP_ID_EDDP:
            case LSA_COMP_ID_EDDS:
            case LSA_COMP_ID_EDDT:
            {
		        pPipe->d.lower.lower_orig_comp_edd_ch_handle = hif_get_open_channel_lower_handle_edd(pRQB);
                break;
            }
            default:
            {
		        pPipe->d.lower.lower_orig_comp_ch_handle = pPipe->func.pCompFuncs->GetOpenChannelLowerHandle(pRQB);
            }
        }
        #else
		pPipe->d.lower.lower_orig_comp_ch_handle = pPipe->func.pCompFuncs->GetOpenChannelLowerHandle(pRQB);
        #endif

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_OPENED_LOWER);
	}
	else
	{
		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_open_channel_done(-) - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%X)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

		hif_set_pipe_state(pPipe, HIF_PIPE_STATE_CLOSED);
	}

	/* (2) return RQB to hif upper */
	hif_send(pPipe, pRQB);
}

/**
 * @brief Sets up the pipe mapping for pHifInst
 * 
 * Detailed description: See function block.
 * 
 * @param [inout] pHifInst  - current HIF instance
 * @param [in] pRQB         - Admin RQB (HIF_OPC_LD_UPPER_OPEN, HIF_OPC_LD_LOWER_OPEN, HIF_OPC_HD_UPPER_OPEN, HIF_OPC_HD_LOWER_OPEN)
 *
 * @return LSA_RET_ERR_RESOURCE     - internal error
 * @return LSA_RET_ERR_PARAM        - error in configuration
 * @return LSA_RET_OK               - success
 */
LSA_UINT16 hif_setup_pipe_mapping(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 e=1;
	LSA_UINT16 i,f;

	#if defined(HIF_CFG_LD_GLOB_USR_PIPES) || defined(HIF_CFG_LD_IF_USR_PIPES) || defined(HIF_CFG_HD_GLOB_USR_PIPES) || defined(HIF_CFG_HD_IF_USR_PIPES)
	LSA_UINT16 g;
	#endif

	#ifdef HIF_CFG_LD_GLOB_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aLdGlobUsrPipes[] = HIF_CFG_LD_GLOB_USR_PIPES;
	#endif
	#ifdef HIF_CFG_LD_IF_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aLdIfUsrPipes[] = HIF_CFG_LD_IF_USR_PIPES;
	#endif
	#ifdef HIF_CFG_HD_GLOB_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aHdGlobUsrPipes[] = HIF_CFG_HD_GLOB_USR_PIPES;
	#endif
	#ifdef HIF_CFG_HD_IF_USR_PIPES
	HIF_USR_PIPE_DEFINITION_TYPE aHdIfUsrPipes[] = HIF_CFG_HD_IF_USR_PIPES;
	#endif

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_pipe_mapping(+) -  pHifInst->hH(0x%x) pRQB(0x%X)", pHifInst->hH, pRQB);

	switch(pHifInst->Type)
	{
		case HIF_DEV_TYPE_LD_LOWER:
		case HIF_DEV_TYPE_LD_UPPER:
        {
			/* Global LD Usr Pipes */
			#ifdef HIF_CFG_LD_GLOB_USR_PIPES
			for(g=0; g<(sizeof(aLdGlobUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
			{
				hif_setup_single_pipe_params_by_index(pHifInst, e++, aLdGlobUsrPipes[g].comp_id, 0, aLdGlobUsrPipes[g].pipe_id);
				
                if(e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
			}
			#endif
		
			/* IF specific pipes */
			for(f=0; f<pRQB->args.dev_ld_open.hd_count; f++)
			{
				/* IF specific LD Usr Pipes */
				#ifdef HIF_CFG_LD_IF_USR_PIPES
				for(g=0; g<(sizeof(aLdIfUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
				{
					hif_setup_single_pipe_params_by_index(pHifInst, e++, aLdIfUsrPipes[g].comp_id, pRQB->args.dev_ld_open.hd_args[f].hd_id, aLdIfUsrPipes[g].pipe_id);
						
                    if(e>=HIF_MAX_PIPES_INST)
                    {
                        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                        return LSA_RET_ERR_RESOURCE;
                    }
				}
				#endif

				#if (HIF_CFG_USE_EPS_RQBS == 1)
				hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_PNBOARDS, pRQB->args.dev_ld_open.hd_args[f].hd_id, HIF_CPLD_IF_HD_EPS);
				if(e>=HIF_MAX_PIPES_INST)
				{
					HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
					return LSA_RET_ERR_RESOURCE;
				}
				#endif
			}

			/* This is currently the only non-user ld pipe */
			#if (HIF_CFG_USE_EPS_RQBS == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_PNBOARDS, 0/*hd_id*/, HIF_CPLD_GLO_LD_EPS);
			if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
			}
			#endif

			break;
        }
		case HIF_DEV_TYPE_HD_LOWER:
		case HIF_DEV_TYPE_HD_UPPER:
        {
			/* Global HD Usr Pipes */
			#ifdef HIF_CFG_HD_GLOB_USR_PIPES
			for(g=0; g<(sizeof(aHdGlobUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
			{
				hif_setup_single_pipe_params_by_index(pHifInst, e++, aHdGlobUsrPipes[g].comp_id, 0, aHdGlobUsrPipes[g].pipe_id);
				
                if(e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
			}
			#endif

			/* Unique Pipes */

			#if (HIF_CFG_USE_CPHD_APP_SOCK_CHANNELS == 1)
			#if (HIF_CFG_USE_SOCK == 1)

			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_SOCK,  0, HIF_CPHD_GLO_APP_SOCK_USER_CH_1);
			
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }

			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_SOCK,  0, HIF_CPHD_GLO_APP_SOCK_USER_CH_2);
			
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif
			#endif

			/* IF specific pipes */
			/* IF specific HD Usr Pipes */
			#ifdef HIF_CFG_HD_IF_USR_PIPES
			for(g=0; g<(sizeof(aHdIfUsrPipes) / sizeof(HIF_USR_PIPE_DEFINITION_TYPE)); g++)
			{
				hif_setup_single_pipe_params_by_index(pHifInst, e++, aHdIfUsrPipes[g].comp_id, pRQB->args.dev_hd_open.hd_args.hd_id, aHdIfUsrPipes[g].pipe_id);
					
                if(e>=HIF_MAX_PIPES_INST)
                {
                    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                    return LSA_RET_ERR_RESOURCE;
                }
			}
			#endif

			#if (HIF_CFG_USE_EDD == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_TCIP_EDD_ARP);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
				
            hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_TCIP_EDD_ICMP);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
				
            hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_TCIP_EDD_UDP);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
				
            hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_TCIP_EDD_TCP);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif

			#if (HIF_CFG_USE_LLDP == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_LLDP, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_OHA_LLDP_EDD);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif

			#if (HIF_CFG_USE_MRP == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_MRP,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_OHA_MRP);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif

			#if (HIF_CFG_USE_EDD == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_OHA_EDD);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
				
            hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_DCP,  pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_OHA_DCP_EDD);
				
            if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
            
            hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_EDD, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_HSA_EDD);

            if(e >= HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)", e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }          
			#endif

			#if (HIF_CFG_USE_NARE == 1)				
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_NARE, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_SYS_OHA_NARE);
			if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif

			#if (HIF_CFG_USE_CLRPC == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_CLRPC, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_L_IF_SYS_CM_CLRPC);
			if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif
			#if (HIF_CFG_USE_OHA == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_OHA, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_L_IF_SYS_CM_OHA);
            if(e >= HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)", e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif

			#if (HIF_CFG_USE_EPS_RQBS == 1)
			hif_setup_single_pipe_params_by_index(pHifInst, e++, LSA_COMP_ID_PNBOARDS, pRQB->args.dev_hd_open.hd_args.hd_id, HIF_CPHD_IF_HD_EPS);
			if(e>=HIF_MAX_PIPES_INST)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) current pipe count e(0x%x) is bigger then the maximum (0x%x)",e, HIF_MAX_PIPES_INST);
                return LSA_RET_ERR_RESOURCE;
            }
			#endif
			break;
        }
		default:
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_pipe_mapping(-) invalid pHifInst->Type (0x%x)", pHifInst->Type);
            return LSA_RET_ERR_PARAM;
        }
	}

	for(i = 1; i < e; i++)
	{
		hif_update_std_comp_access_funcs(&pHifInst->Pipes[i]); /* Assign Matching RQB Access Funcs (Open Channel / Serialization / ...) */
		hif_set_pipe_state(&pHifInst->Pipes[i], HIF_PIPE_STATE_CLOSED);
	}

	return LSA_RET_OK;
}

/**
 * @brief Sets up a single pipe by setting the values comp_id, hd_id and pipe_id
 * 
 * 
 * @param [inout] pHifInst  - HIF instance with the current pipe
 * @param [in] index        - used to access the pipe within the instance
 * @param [in] comp_id      - This value is copied into the pipe struture 
 * @param [in] hd_id        - This value is copied into the pipe struture
 * @param [in] pipe_id      - This value is copied into the pipe struture
 */
LSA_VOID hif_setup_single_pipe_params_by_index(HIF_INST_PTR_TYPE pHifInst, LSA_UINT16 index, LSA_COMP_ID_TYPE comp_id, HIF_HD_ID_TYPE hd_id, HIF_PIPE_ID_TYPE pipe_id)
{
    HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_single_pipe_params_by_index(+) -  pHifInst(0x%x) index(0x%x) comp_id(0x%x) hd_id(0x%x) pipe_id(0x%x)", pHifInst, index, comp_id, hd_id, pipe_id);

    HIF_ASSERT(index < HIF_MAX_PIPES_INST);

	pHifInst->Pipes[index].comp_id = comp_id;
	pHifInst->Pipes[index].hd_id   = hd_id;
	pHifInst->Pipes[index].pipe_id = pipe_id;
}

/**
 * @brief Getter function for a pipe that uses hd_id and pipe_id. 
 * 
 * @param [out] ppPipe  - handle to the pipe. If the pipe was not found, this parameter is set to LSA_NULL.
 * @param [in] pHifInst - current HIF instance
 * @param [in] hd_id    - hd_id used to search for the pipe
 * @param [in] pipe_id  - pipe_id used to search for the pipe
 */
LSA_VOID hif_get_pipe(HIF_PIPE_PTR_TYPE* ppPipe, HIF_INST_PTR_TYPE pHifInst, HIF_HD_ID_TYPE hd_id, HIF_PIPE_ID_TYPE pipe_id)
{
	LSA_UINT16 i;

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_single_pipe_params_by_index(+) -  ppPipe(0x%x), hd_id(0x%x), pipe_id(0x%x)", ppPipe, hd_id, pipe_id);

	*ppPipe = LSA_NULL;

	for(i=0; i<HIF_MAX_PIPES_INST; i++)
	{
		if( (pHifInst->Pipes[i].hd_id == hd_id) &&
			(pHifInst->Pipes[i].pipe_id == pipe_id) )
		{
			*ppPipe = &(pHifInst->Pipes[i]);
			return;
		}
	}
}

/**
 * @brief Sets the pipe state to the value provided in eState
 * 
 * Calls hif_update_serialization_funcs to update the functions of the pipe.
 * 
 * @param [inout] pPipe - pointer to the pipe that is updated
 * @param [in] eState   - value of the state
 */
LSA_VOID hif_set_pipe_state(HIF_PIPE_PTR_TYPE pPipe, HIF_PIPE_STATE_TYPE eState)
{
    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_set_pipe_state(+) -  pPipe(0x%x), eState(0x%x), pPipeId(0x%x)", pPipe, eState, pPipe->pipe_id);

	pPipe->State = eState;

    #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	hif_update_serialization_funcs(pPipe);
    #endif
}

/**
 * @brief Returns if this pipe is a upper pipe or a lower pipe
 * 
 * 
 * @param [in] pPipe    - pipe to check
 * @return LSA_FALSE    - this is a lower pipe
 * @return LSA_TRUE     - this is a upper pipe
 */
LSA_BOOL hif_is_upper_pipe(HIF_PIPE_PTR_TYPE pPipe)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_is_upper_pipe(+) -  pPipe(0x%x)", pPipe);

	switch(pPipe->State) /* Get Pipe Type by State */
	{
		case HIF_PIPE_STATE_OPENING_LOWER:
		case HIF_PIPE_STATE_OPENED_LOWER:
        {
			return LSA_FALSE;
        }
        case HIF_PIPE_STATE_OPENING_UPPER:
		case HIF_PIPE_STATE_OPENED_UPPER:
        {
            return LSA_TRUE;
        }
        case HIF_PIPE_STATE_CLOSED:
        default:
        {
			break;
        }
	}

	if(pPipe->PipeIndex == HIF_GLOB_ADMIN) /* Admin Pipe? => Pipe direction is derived by instance type */
	{
		if(HIF_IS_NOT_NULL(pPipe->pInstance))
		{
			if(hif_is_upper_device(pPipe->pInstance) == LSA_TRUE)
			{
				return LSA_TRUE;
			}
			else
			{
				return LSA_FALSE;
			}
		}
	}

	/* HIF_PIPE_STATE_CLOSED - No Admin Pipe */	
	return LSA_FALSE; /* We assume that it is a lower pipe */
}

#endif // (HIF_CFG_USE_HIF == 1)

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/

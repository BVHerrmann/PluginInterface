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
/*  F i l e               &F: hif_dev.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements HIF device handling                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	3
#define HIF_MODULE_ID		LTRC_ACT_MODUL_ID

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

#if (HIF_CFG_USE_HIF == 1)
/**
 * \brief Starts the internal poll mechanism. 
 * 
 * Starts the internal poll mechanism. HIF_DO_LOW_REQUEST is called within hif_request::HIF_OPC_POLL, HIF_OPC_POLL itself calls HIF_DO_LOW_REQUEST. 
 * During startup, HIF calls HIF_DO_LOW_REQUEST once to startup the ping pong mechanism.
 * 
 * @see HIF_DO_LOW_REQUEST                      - This output function sends the internal poll RQB to hif_request
 * @see g_pHifData->internal.bUseInternalPoll   - Is variable enables or disables the additional internal trigger. If this variable is not set (by default), HIF will not use HIF_DO_LOW_REQUEST.
*/
LSA_VOID hif_start_internal_poll(LSA_VOID)
{
    /* only activate if it's configured*/
    if(g_pHifData->internal.bUseInternalPoll)
    {
        /* We may have several HIF instances. We want to activate the trigger mechanism exactly once.*/
        if(g_pHifData->internal.internalPollCnt != 0)
        {
            g_pHifData->internal.internalPollCnt++;
            HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData->internal.pInternalPoll));
            HIF_DO_LOW_REQUEST(g_pHifData->internal.pInternalPoll);
        }
    }
}

/**
 * \brief Implements HIF_OPC_LD_UPPER_CLOSE and HIF_OPC_HD_UPPER_CLOSE
 * 
 * Checks the current state of the instance, then sets the state to HIF_INST_STATE_CLOSING.
 * The RQB is forwarded to the lower device.
 *   
 * @param [in] pRQB - close device RQB
 * @return
 * - LSA_RET_ERR_PARAM
 *   -# no HIF instance could be found to close 
 *   -# the OPCODE is not HIF_OPC_LD_UPPER_CLOSE nor HIF_OPC_HD_UPPER_CLOSE
 *   -# if HIF instance is for upper device
 * - LSA_RET_ERR_SEQUENCE
 *   -# state of device is HIF_INST_STATE_FREE or HIF_INST_STATE_OPENING
 *   -# pipes are not closed
 *   -# still open RQBs exist
 * - LSA_RET_OK_ASYNC
 *   -# on success
 */
LSA_UINT16 hif_upper_device_close(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
	LSA_UINT16 i;

	/* Find HIF instance to be closed */
	hif_get_instance(&pHifInst, pRQB->args.dev_close.hH);
	if(HIF_IS_NULL(pHifInst))
	{
		return LSA_RET_ERR_PARAM;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_device_close() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* Check params */
	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		if(HIF_RQB_GET_OPCODE(pRQB) != HIF_OPC_LD_UPPER_CLOSE)
		{
            HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - wrong HIF opcode need (HIF_OPC_LD_UPPER_CLOSE) get opcode (0x%x), hH(%d) pRQB(0x%x)",HIF_RQB_GET_OPCODE(pRQB), pHifInst->hH, pRQB);
			return LSA_RET_ERR_PARAM;
		}
	}
	else if(HIF_RQB_GET_OPCODE(pRQB) != HIF_OPC_HD_UPPER_CLOSE) /* HD Device */
	{
        HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - wrong HIF opcode need (HIF_OPC_HD_UPPER_CLOSE) get opcode(0x%x), hH(%d) pRQB(0x%x)",HIF_RQB_GET_OPCODE(pRQB), pHifInst->hH, pRQB);
		return LSA_RET_ERR_PARAM;
	}

	if(hif_is_upper_device(pHifInst) != LSA_TRUE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - pHifInst is not an upper device, hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);
		return LSA_RET_ERR_PARAM;
	}

	/* Check state of device */
	if( (pHifInst->State == HIF_INST_STATE_FREE) ||
		(pHifInst->State == HIF_INST_STATE_OPENING) )
	{
        HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - pHifInst->State(0x%x) has a wrong state, hH(%d) pRQB(0x%x)", pHifInst->State, pHifInst->hH, pRQB);
		return LSA_RET_ERR_SEQUENCE;
	}

	/* Check if pipes are closed and there are no pending requests */
	for(i=0; i<HIF_MAX_PIPES_INST; i++)
	{
		if(pHifInst->Pipes[i].State != HIF_PIPE_STATE_CLOSED)
		{
            HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - pHifInst->State(0x%x) has a wrong state, hH(%d) pRQB(0x%x)", pHifInst->State, pHifInst->hH, pRQB);
			return LSA_RET_ERR_SEQUENCE;
		}
	}

    if(pHifInst->if_pars.SharedMem.uActOpenRQBs != 0)
	{
        HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close() - There are open RQBs: uActOpenRQBs(0x%x), State(0x%x), hH(%d), pRQB(0x%x)", pHifInst->if_pars.SharedMem.uActOpenRQBs, pHifInst->State, pHifInst->hH, pRQB);
		return LSA_RET_ERR_SEQUENCE;
	}

	/* Set state to closing */
	pHifInst->State = HIF_INST_STATE_CLOSING;

	/* Forward request to lower device */
	hif_send_admin_request(pHifInst, pRQB);

	return LSA_RET_OK_ASYNC;
}

/**
 * \brief Finish upper device closing 
 * 
 * Depending on transfer mode we can immediately close or we need to wait until transfer processing finished
 * In Short mode we can immediately clean up, there are no critical structures used and the local instance is secured by enter/exit
 * In Shared mem mode we have to wait with cleanup because when the sys cbf is called we might loose access to the shared memory
 * 
 * @param [in] pHifInst     - current HIF instance
 * @param [in] pRQB         - Close device RQB
 * @return
 */
LSA_VOID hif_upper_device_close_finish_1(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_device_close_finish_1(+) -  pHifInst(0x%x), pRQB(0x%x)", pHifInst, pRQB);

	pHifInst->pRQBUpperDeviceCloseFinishRsp = pRQB;

	if(pHifInst->if_pars.TmOptions.bShmHandling != LSA_TRUE)
	{
		hif_upper_device_close_finish_2(pHifInst);
	}
}

/**
 * \brief Finish upper device closing
 * 
 * (1) Check response from lower device
 * (2) Disable receive
 * (3) Disconnect the lower device
 * (4) Free all allocated ressources
 * (5) Callback system adaptation
 * 
 * @param [in] pHifInst     - current HIF instance
 * @return
 */
LSA_VOID hif_upper_device_close_finish_2(HIF_INST_PTR_TYPE pHifInst)
{
	HIF_RQB_PTR_TYPE pRQB = pHifInst->pRQBUpperDeviceCloseFinishRsp;
    LSA_UINT16       retVal;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_device_close_finish_2() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	pHifInst->pRQBUpperDeviceCloseFinishRsp = LSA_NULL;
    
	/* (1) device close returned from lower side => successful? */
	if(HIF_RQB_GET_RESPONSE(pRQB) != LSA_OK)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_device_close_finish_2() - device close from lower side was not successful, State(%d), hH(%d)", pHifInst->State, pHifInst->hH);
		hif_system_callback(pRQB, HIF_RQB_GET_RESPONSE(pRQB));        
		return;
	}

	/* (2) disable receiving new messages */
    retVal = hif_disable_receive(pHifInst);
    HIF_ASSERT(retVal == LSA_RET_OK);

	/* (3) disconnect from connected lower hif */
	retVal = hif_finish_transfer_mode(pHifInst);
    HIF_ASSERT(retVal == LSA_RET_OK);

	/* (4) free resources */
	hif_free_instance(pHifInst);

	/* (5) Callback */
	hif_system_callback(pRQB, LSA_RET_OK);
}

/**
 * \brief Implements HIF_OPC_LD_LOWER_CLOSE and HIF_OPC_HD_LOWER_CLOSE
 * 
 * (1) Check params
 * (2) Disable receive
 * (3) Disconnect the upper instance
 * (4) Free all allocated ressources
 * 
 * @param [in] pRQB - Close device RQB
 * @return LSA_RET_ERR_PARAM - error in RQB or state
 */
LSA_UINT16 hif_lower_device_close(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_INST_PTR_TYPE pHifInst;
    LSA_UINT16        retVal;

	/* Find HIF instance to be closed */
	hif_get_instance(&pHifInst, pRQB->args.dev_close.hH);
	if(HIF_IS_NULL(pHifInst))
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_device_close() - pHifInst is not valid");
		return LSA_RET_ERR_PARAM;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_device_close() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* (1) Check params */
	if(hif_is_ld_device(pHifInst) == LSA_TRUE)
	{
		if(HIF_RQB_GET_OPCODE(pRQB) != HIF_OPC_LD_LOWER_CLOSE)
		{
            HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_device_close() - wrong HIF opcode need (HIF_OPC_LD_LOWER_CLOSE) get opcode (0x%x), hH(%d) pRQB(0x%x)",HIF_RQB_GET_OPCODE(pRQB), pHifInst->hH, pRQB);
			return LSA_RET_ERR_PARAM;
		}
	}
	else if(HIF_RQB_GET_OPCODE(pRQB) != HIF_OPC_HD_LOWER_CLOSE)
	{
        HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_device_close() - wrong HIF opcode need (HIF_OPC_HD_LOWER_CLOSE) get opcode (0x%x), hH(%d) pRQB(0x%x)",HIF_RQB_GET_OPCODE(pRQB), pHifInst->hH, pRQB);
		return LSA_RET_ERR_PARAM;
	}

	if(hif_is_upper_device(pHifInst) == LSA_TRUE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_device_close() - pHifInst is not an lower device, hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);
		return LSA_RET_ERR_PARAM;
	}

	/* Check state of device */
	if(pHifInst->State != HIF_INST_STATE_OPENING)
	{
        HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_device_close() - pHifInst->State (0x%x) is not in a valid state, need (HIF_INST_STATE_OPENING), hH(%d) pRQB(0x%x)", pHifInst->State, pHifInst->hH, pRQB);
		return LSA_RET_ERR_SEQUENCE;
	}

	/* (2) disable receiving new messages */
	retVal = hif_disable_receive(pHifInst);
    HIF_ASSERT(retVal == LSA_RET_OK);

	/* (3) disconnect interface for hif upper device */
	retVal = hif_finish_transfer_mode(pHifInst);
    HIF_ASSERT(retVal == LSA_RET_OK);

	/* (4) Free Instance */
	hif_free_instance(pHifInst);

	return LSA_RET_OK;
}

/**
 * \brief Forwards a received RQB to the destination component
 * 
 * (1) Save path into in the RQB
 * (2) Set original RQB handle
 * (3) Call REQUEST_LOWER
 * 
 * Path: REQ
 * Receive RQB from transfer memory --> hif_lower_pipe_request --> destination component
 *
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_lower_pipe_request(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
    LSA_COMP_ID_TYPE const comp_id = pPipe->comp_id;

	/* (1) remember the source of the RQB */
	hif_lower_save_return_path_to_rqb(pPipe, pRQB);

	/* (2) New standard requests received from upper */
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

	HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_pipe_request() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

	/* (3) Give RQB to destination component */
	pPipe->pInstance->func.REQUEST_LOWER(pRQB, pPipe->pInstance->hSysDev);
}

/**
 * \brief Forwards a received RQB to the originator component
 * 
 * The callback is done using hif_callback
 * 
 * Path: CNF
 * Receive RQB from transfer memory --> hif_upper_pipe_request_done --> originator component
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_upper_pipe_request_done(HIF_PIPE_CONST_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
	/* Requests returned from lower */
	HIF_ASSERT(pPipe->State != HIF_PIPE_STATE_CLOSED);

	HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_pipe_request_done() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB);

	hif_callback(pPipe->d.upper.ch_handle, pRQB, HIF_RQB_GET_RESPONSE(pRQB));
}

/**
 * \brief Forwards a RQB that was received from the admin pipe to the system component
 * 
 * Path: REQ
 * Receive RQB from transfer memory --> hif_lower_device_close_proceed --> System component (HIF / PSI)
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_lower_device_close_proceed(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* Change RQB parameters with local values */
	pHifInst->backup.SystemDoneCbf = pRQB->args.dev_close.Cbf;
	pHifInst->backup.hH            = pRQB->args.dev_close.hH;
	pRQB->args.dev_close.Cbf       = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)hif_close_device_lower_done;
	pRQB->args.dev_close.hH        = pHifInst->hH;

	if(hif_is_ld_device(pHifInst) != LSA_TRUE)
	{
		pRQB->args.dev_close.hd_id = pHifInst->DevSpecific.hd.hd_id;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_device_close_proceed() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	pHifInst->func.CLOSE_DEVICE_LOWER(pRQB);
}

/**
 * \brief Sends the RQB back to HIF upper
 * 
 * Path: REQ
 * Device close for HIF lower finished --> hif_lower_device_close_finish --> Send RQB over tranfer memory
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_lower_device_close_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* device close returned from lower user => successful? */
	if(HIF_RQB_GET_RESPONSE(pRQB) == LSA_OK)
	{
		pHifInst->State = HIF_INST_STATE_OPENING;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_device_close_finish() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* Send RQB to HIF Upper */
	hif_send_admin_request(pHifInst, pRQB);
}

/**
 * \brief Sends a RQB over the admin pipe HIF_GLOB_ADMIN
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_send_admin_request(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_send_admin_request(+) -  pHifInst(0x%x), pRQB(0x%x)", pHifInst, pRQB);

	hif_send(&(pHifInst->Pipes[HIF_GLOB_ADMIN]),pRQB);
}

/**
 * \brief Forwards a RQB that was received over the admin pipe
 * 
 * This functionis called within a lower device
 * Path: REQ
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> xxx_proceed
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_receive_admin_request(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_receive_admin_request(+) -  pHifInst(0x%x), pRQB(0x%x)", pHifInst, pRQB);

	switch(HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
			hif_lower_ld_device_open_proceed(pHifInst, pRQB);
			break;
		case HIF_OPC_HD_UPPER_OPEN:
			hif_lower_hd_device_open_proceed(pHifInst, pRQB);
			break;
		case HIF_OPC_HD_UPPER_CLOSE:
		case HIF_OPC_LD_UPPER_CLOSE:
			hif_lower_device_close_proceed(pHifInst, pRQB);
			break;
		default:
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_receive_admin_request() -  pRQB(0x%x) Invalid OPC (%d)", pRQB, HIF_RQB_GET_OPCODE(pRQB));
			HIF_FATAL(0);
	}
}

/**
 * \brief Forwards a RQB that was received over the admin pipe
 * 
 * This functionis called within a upper device.
 * 
 * Path: CNF
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> xxx_finish
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_send_admin_request_done(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_send_admin_request_done(+) -  pHifInst(0x%x), pRQB(0x%x)", pHifInst, pRQB);

	switch(HIF_RQB_GET_OPCODE(pRQB))
	{
		case HIF_OPC_LD_UPPER_OPEN:
			hif_upper_ld_device_open_finish(pHifInst, pRQB);
			break;
		case HIF_OPC_HD_UPPER_OPEN:
			hif_upper_hd_device_open_finish(pHifInst, pRQB);
			break;
		case HIF_OPC_HD_UPPER_CLOSE:
		case HIF_OPC_LD_UPPER_CLOSE:
			hif_upper_device_close_finish_1(pHifInst, pRQB);
			break;
		default:
            HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_send_admin_request_done() -  pRQB(0x%x) Invalid OPC (%d)", pRQB, HIF_RQB_GET_OPCODE(pRQB));
			HIF_FATAL(0);
	}
}

/**
 * \brief Forwards a RQB that was received over the admin pipe
 * 
 * This functionis called within a lower device
 * Path: REQ
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> HIF_XD_SYSTEM_REQUEST_LOWER
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_lower_sys_request(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_COMP_ID_TYPE const comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	/* New sys requests received from upper */

	HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_sys_request() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x) Comp_id(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB, comp_id);

	/* remember the source of the RQB and save the original Cbf */
	hif_save_return_path_and_cbf_to_sys_rqb(pPipe, pRQB);

	/* replace sys cbf with own */
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
    HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[comp_id].Comp.ReplaceSysRequestCbf));
	g_pHifData->CompStore.CompSys[comp_id].Comp.ReplaceSysRequestCbf(pPipe->pInstance->hSysDev, pRQB, hif_lower_sys_request_done);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_SYSTEM_REQUEST_LOWER(pRQB);
	}
	else
	{
		HIF_HD_SYSTEM_REQUEST_LOWER(pRQB);
	}
}

/**
 * \brief Forwards a RQB that was received over the admin pipe
 * 
 * This functionis called within a upper device.
 * 
 * Path: CNF
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> HIF_XD_U_SYSTEM_REQUEST_DONE
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_upper_sys_request_done(HIF_PIPE_CONST_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_SYSTEM_CALLBACK_FCT_PTR_TYPE       pCbf;
	LSA_COMP_ID_TYPE                 const comp_id = HIF_RQB_GET_COMP_ID(pRQB);

	HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_sys_request_done() - hH(%d) hdId(%d) pipeId(%d) pRQB(0x%x) Comp_id(0x%x)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, pRQB, comp_id);

	/* Sys Request returned from lower */
	HIF_ASSERT(g_pHifData->CompStore.CompSys[comp_id].bUsed == LSA_TRUE);
    HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequestCbf));
	g_pHifData->CompStore.CompSys[comp_id].Comp.GetSysRequestCbf(pPipe->pInstance->hSysDev, pRQB, &pCbf);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_U_SYSTEM_REQUEST_DONE(pCbf, pRQB);
	}
	else
	{
		HIF_HD_U_SYSTEM_REQUEST_DONE(pCbf,pRQB);
	}
}

/**
 * \brief Implements HIF_OPC_HD_UPPER_OPEN
 * 
 * (1) Allocates a HIF instance
 * (2) Copy values from the RQB
 * (3) Call system adaptation (HIF_HD_U_GET_PARAMS)
 * (4) Create pipes
 * (5) Setup transfer mode
 * (6) Enable receive from the other HIF instance
 * (7) Forward RQB to HIF lower instance
 * 
 * @param [in] pRQB                 - RQB with HIF_OPC_HD_UPPER_OPEN
 * @return LSA_RET_ERR_RESOURCE     - internal error or error in RQB
 * @return LSA_RET_OK               - success
 */
LSA_UINT16 hif_upper_hd_device_open(HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;
	HIF_INST_PTR_TYPE pHifInst;
	HIF_GET_PAR_TYPE Pars;

	/* (1) Alloc a new HIF instance */
	hif_alloc_instance(&pHifInst,HIF_RQB_GET_OPCODE(pRQB));
	if(HIF_IS_NULL(pHifInst))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_upper_hd_device_open(-) cannot alloc hif instance");
		return LSA_RET_ERR_RESOURCE;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_hd_device_open() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* (2) Copy values */
	pHifInst->hSysDev                 = pRQB->args.dev_hd_open.hSysDev;
	pRQB->args.dev_hd_open.hH         = pHifInst->hH;
	Pars.hH                           = pHifInst->hH;

	/* (3) Get parameters from system adaption */
	HIF_HD_U_GET_PARAMS(&retVal, pRQB->args.dev_hd_open.hSysDev, &Pars);
	if(retVal != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_hd_device_open(-) cannot get params");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (4) Create PD/IF - Pipe Mapping */
	retVal = hif_setup_pipe_mapping(pHifInst, pRQB);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_hd_device_open(-) hif_setup_pipe_mapping(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (5) Set transfer mode for instance */
	retVal = hif_setup_transfer_mode(pHifInst, &Pars);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_hd_device_open(-) hif_setup_transfer_mode(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (6) Enable RQB receive */
	retVal = hif_enable_receive(pHifInst);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_hd_device_open(-) hif_enable_receive(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	hif_send_admin_request(pHifInst, pRQB); /* (7) Send RQB to HIF Lower */

	return LSA_RET_OK_ASYNC;
}

/**
 * \brief Returns the RQB back to originator of HIF_OPC_HD_UPPER_OPEN.
 *  
 * This functionis called within a upper device.
 * 
 * Path: CNF
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> hif_upper_hd_device_open_finish --> hif_system_callback
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_upper_hd_device_open_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* device open returned from lower side => successful? */
	if(HIF_RQB_GET_RESPONSE(pRQB) != LSA_OK)
	{
		hif_system_callback(pRQB, HIF_RQB_GET_RESPONSE(pRQB));

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_hd_device_open_finish(-) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

		hif_free_instance(pHifInst);
		return;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_hd_device_open_finish() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	pHifInst->State = HIF_INST_STATE_OPEN;

	hif_system_callback(pRQB, LSA_RET_OK);
}

/**
 * \brief Implements HIF_OPC_LD_UPPER_OPEN
 * 
 * (1) Allocates a HIF instance
 * (2) Copy values from the RQB
 * (3) Call system adaptation (HIF_LD_U_GET_PARAMS)
 * (4) Create pipes
 * (5) Setup transfer mode
 * (6) Enable receive from the other HIF instance
 * (7) Forward RQB to HIF lower instance
 * 
 * @param [in] pRQB                 - RQB with HIF_OPC_LD_UPPER_OPEN
 * @return LSA_RET_ERR_RESOURCE     - internal error or error in RQB
 * @return LSA_RET_OK               - success
 */
LSA_UINT16 hif_upper_ld_device_open(HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16        retVal;
	HIF_INST_PTR_TYPE pHifInst;
	HIF_GET_PAR_TYPE  Pars;

	/* (1) Alloc a new HIF instance */
	hif_alloc_instance(&pHifInst,HIF_RQB_GET_OPCODE(pRQB));
	if(HIF_IS_NULL(pHifInst))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_ld_device_open(-) cannot alloc hif instance");
		return LSA_RET_ERR_RESOURCE;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_ld_device_open() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* (2) Copy values */
	pHifInst->hSysDev                 = pRQB->args.dev_ld_open.hSysDev;
	pRQB->args.dev_ld_open.hH         = pHifInst->hH;
	Pars.hH                           = pHifInst->hH;

	/* (3) Get parameters from system adaption */
	HIF_LD_U_GET_PARAMS(&retVal, pRQB->args.dev_ld_open.hSysDev, &Pars);
	if(retVal != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_ld_device_open(-) cannot get params");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (4) Create PD/IF - Pipe Mapping */
	retVal = hif_setup_pipe_mapping(pHifInst, pRQB);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_ld_device_open(-) hif_setup_pipe_mapping(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (5) Set transfer mode for instance */
	retVal = hif_setup_transfer_mode(pHifInst, &Pars);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_ld_device_open(-) hif_setup_transfer_mode(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (6) Enable RQB receive */
	retVal = hif_enable_receive(pHifInst);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_upper_ld_device_open(-) hif_enable_receive(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	hif_send_admin_request(pHifInst, pRQB); /* (7) Send RQB to HIF Lower */

	return LSA_RET_OK_ASYNC;
}

/**
 * \brief Returns the RQB back to originator of HIF_OPC_LD_UPPER_OPEN.
 *  
 * This functionis called within a upper device.
 * 
 * Path: CNF
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> hif_upper_ld_device_open_finish --> hif_system_callback
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_upper_ld_device_open_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* device open returned from lower side => successful? */
	if(HIF_RQB_GET_RESPONSE(pRQB) != LSA_OK)
	{
		hif_system_callback(pRQB, HIF_RQB_GET_RESPONSE(pRQB));

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_ld_device_open_finish(-) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

		hif_free_instance(pHifInst);
		return;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_upper_ld_device_open_finish(+) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	pHifInst->State = HIF_INST_STATE_OPEN;

	hif_system_callback(pRQB, LSA_RET_OK);
}

/**
 * \brief Implements HIF_OPC_HD_LOWER_OPEN
 * 
 * (1) Allocates a HIF instance
 * (2) Copy values from the RQB
 * (3) Call system adaptation (HIF_HD_L_GET_PARAMS)
 * (4) Setup transfer mode
 * (5) Enable receive from the other HIF instance
 * 
 * @param [in] pRQB                 - RQB with HIF_OPC_HD_LOWER_OPEN
 * @return LSA_RET_ERR_RESOURCE     - internal error or error in RQB
 * @return LSA_RET_OK               - success
 */
LSA_UINT16 hif_lower_hd_device_open(HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16        retVal;
	HIF_INST_PTR_TYPE pHifInst;
	HIF_GET_PAR_TYPE  Pars;

	/* (1) Alloc a new HIF instance */
	hif_alloc_instance(&pHifInst, HIF_RQB_GET_OPCODE(pRQB));
	if(HIF_IS_NULL(pHifInst))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_hd_device_open(-) cannot alloc hif instance");
		return LSA_RET_ERR_RESOURCE;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_hd_device_open() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* (2) Copy values */
	pHifInst->hSysDev                 = pRQB->args.dev_lower_open.hSysDev;
	pRQB->args.dev_lower_open.hH      = pHifInst->hH;
	Pars.hH                           = pHifInst->hH;

	/* (3) Get parameters from system adaption */
	HIF_HD_L_GET_PARAMS(&retVal, pRQB->args.dev_lower_open.hSysDev, &Pars);
	if(retVal != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_hd_device_open(-) cannot get params");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (4) Set transfer mode for instance */
	retVal = hif_setup_transfer_mode(pHifInst, &Pars);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_hd_device_open(-) hif_setup_transfer_mode(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (5) Enable RQB receive */
	retVal = hif_enable_receive(pHifInst);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_hd_device_open(-) hif_enable_receive(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	return LSA_RET_OK;
}

/**
 * \brief Forwards a device open RQB 
 * 
 * This functionis called within a lower device
 * Path: REQ
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> hif_lower_hd_device_open_proceed --> HIF_HD_OPEN_DEVICE_LOWER
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_lower_hd_device_open_proceed(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* Change RQB parameters with local values */
	pHifInst->backup.SystemDoneCbf = pRQB->args.dev_hd_open.Cbf;
	pHifInst->backup.hH            = pRQB->args.dev_hd_open.hH;
	pRQB->args.dev_hd_open.Cbf     = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)hif_open_device_lower_done;
	pRQB->args.dev_hd_open.hH      = pHifInst->hH;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_hd_device_open_proceed() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	HIF_HD_OPEN_DEVICE_LOWER(pRQB);
}

/**
 * \brief Finishes open device
 * 
 * (1) Create pipe mapping
 * (2) Set HIF instance state to HIF_INST_STATE_OPEN
 * (3) pass RQB to HIF Upper
 * 
 * Path: CNF
 * Lower device is opened --> hif_lower_hd_device_open_finish --> Send RQB to HIF Upper 
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_lower_hd_device_open_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	if(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK)
	{
		/* (1)  Create PD/IF - Pipe Mapping */
		retVal = hif_setup_pipe_mapping(pHifInst, pRQB);
		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_hd_device_open_finish(-) hif_setup_pipe_mapping(...) was not successful");
			HIF_RQB_SET_RESPONSE(pRQB, HIF_ERR_PARAM);
			return;
		}

		//Backup hd_id (used in close device)
		pHifInst->DevSpecific.hd.hd_id = pRQB->args.dev_hd_open.hd_args.hd_id;

		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_hd_device_open_finish(+) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);
		/* (2) Set state */
		pHifInst->State = HIF_INST_STATE_OPEN;
	}

	hif_send_admin_request(pHifInst, pRQB); /* (3) Send RQB to HIF Upper */
}

/**
 * \brief Implements HIF_OPC_LD_LOWER_OPEN
 * 
 * (1) Allocates a HIF instance
 * (2) Copy values from the RQB
 * (3) Call system adaptation (HIF_LD_L_GET_PARAMS)
 * (4) Setup transfer mode
 * (5) Enable receive from the other HIF instance
 * 
 * @param [in] pRQB                 - RQB with HIF_OPC_LD_LOWER_OPEN
 * @return LSA_RET_ERR_RESOURCE     - internal error or error in RQB
 * @return LSA_RET_OK               - success
 */
LSA_UINT16 hif_lower_ld_device_open(HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16        retVal;
	HIF_INST_PTR_TYPE pHifInst;
	HIF_GET_PAR_TYPE  Pars;

	/* (1) Alloc a new HIF instance */
	hif_alloc_instance(&pHifInst,HIF_RQB_GET_OPCODE(pRQB));
	if(HIF_IS_NULL(pHifInst))
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_lower_ld_device_open(-) cannot alloc hif instance");
		return LSA_RET_ERR_RESOURCE;
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_ld_device_open() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	/* (2) Copy values */
	pHifInst->hSysDev                 = pRQB->args.dev_lower_open.hSysDev;
	pRQB->args.dev_lower_open.hH      = pHifInst->hH;
	Pars.hH                           = pHifInst->hH;

	/* (3) Get parameters from system adaption */
	HIF_LD_L_GET_PARAMS(&retVal, pRQB->args.dev_lower_open.hSysDev, &Pars);
	if(retVal != LSA_RET_OK)
	{
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_lower_ld_device_open(-) cannot get params");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (4) Set transfer mode for instance */
	retVal = hif_setup_transfer_mode(pHifInst, &Pars);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_ld_device_open(-) hif_setup_transfer_mode(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	/* (5) Enable RQB receive */
	retVal = hif_enable_receive(pHifInst);
	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_lower_ld_device_open(-) hif_enable_receive(...) was not successful");
		hif_free_instance(pHifInst);
		return retVal;
	}

	return LSA_RET_OK;
}

/**
 * \brief Forwards a device open RQB 
 * 
 * This functionis called within a lower device
 * Path: REQ
 * Admin RQB received from the transfer memory --> hif_receive_admin_request --> hif_lower_ld_device_open_proceed --> HIF_LD_OPEN_DEVICE_LOWER
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_lower_ld_device_open_proceed(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	/* Change RQB parameters with local values */
	pHifInst->backup.SystemDoneCbf = pRQB->args.dev_ld_open.Cbf;
	pHifInst->backup.hH            = pRQB->args.dev_ld_open.hH;
	pRQB->args.dev_ld_open.Cbf     = (HIF_UPPER_CALLBACK_FCT_PTR_TYPE)hif_open_device_lower_done;
	pRQB->args.dev_ld_open.hH      = pHifInst->hH;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_ld_device_open_proceed() - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	HIF_LD_OPEN_DEVICE_LOWER(pRQB);
}

/**
 * \brief Finishes open device
 * 
 * (1) Create pipe mapping
 * (2) Set HIF instance state to HIF_INST_STATE_OPEN
 * (3) pass RQB to HIF Upper
 * 
 * Path: CNF
 * Lower device is opened --> hif_lower_ld_device_open_finish --> Send RQB to HIF Upper 
 * 
 * @param [in] pHifInst  - current HIF instance
 * @param [in] pRQB      - RQB to forward
 * @return
 */
LSA_VOID hif_lower_ld_device_open_finish(HIF_INST_PTR_TYPE pHifInst, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_UINT16 retVal;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_ld_device_open_finish(+) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

	if(HIF_RQB_GET_RESPONSE(pRQB) == HIF_OK)
	{
		/* (1) Create PD/IF - Pipe Mapping */
		retVal = hif_setup_pipe_mapping(pHifInst, pRQB);
		if(retVal != LSA_RET_OK)
		{
			HIF_RQB_SET_RESPONSE(pRQB, HIF_ERR_PARAM);

			HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_ld_device_open_finish(-) - hH(%d) pRQB(0x%x)", pHifInst->hH, pRQB);

			return;
		}
		/* (2) Set state */
		pHifInst->State = HIF_INST_STATE_OPEN;
	}
	
	hif_send_admin_request(pHifInst, pRQB); /* (3) Send RQB to HIF Upper */
}

/**
 * \brief Replaces the UserId of the RQB by a new RQB tag. Saves the old UserId and stores it inside the tag.
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to process
 * @return
 */
LSA_VOID hif_lower_save_return_path_to_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_save_return_path_to_rqb(+) - pPipe(%d) pRQB(0x%x)", pPipe, pRQB);

	hif_alloc_rb_tag(&pRbTag, pPipe, pRQB);

	if(HIF_IS_NULL(pRbTag))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_lower_save_return_path_to_rqb(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0); /* can't handle => fatal */
	}

	/* backup original user id  */
	pRbTag->UserId = HIF_RQB_GET_USER_ID(pRQB);

	/* remember the source of the RQB in the user id */
	HIF_RQB_SET_USER_ID_PTR(pRQB, pRbTag);
}

/**
 * \brief Restores the pipe info from the RQB tag structure.
 * 
 * @param [out] ppPipe  - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_get_return_path_of_rqb(HIF_PIPE_PTR_TYPE * ppPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_get_return_path_of_rqb(+) - ppPipe(%d) pRQB(0x%x)", ppPipe, pRQB);

	pRbTag = (HIF_RB_TAG_PTR_TYPE)HIF_RQB_GET_USER_ID_PTR(pRQB);
	if(pRbTag == LSA_NULL)
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_get_return_path_of_rqb(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0);
	}

	*ppPipe = pRbTag->pPipe;
}

/**
 * \brief Extracts the RQB Tag from from the UserID, then restores the original UserID.
 * 
 * Undoes what has been done in hif_lower_save_return_path_to_rqb.
 * 
 * @param [in] pRQB     - RQB to process
 * @return
 */
LSA_VOID hif_restore_rqb_user_fields_and_free_return_path(HIF_RQB_PTR_TYPE pRQB)
{
	HIF_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_restore_rqb_user_fields_and_free_return_path(+) - pRQB(0x%x)", pRQB);

	pRbTag = (HIF_RB_TAG_PTR_TYPE)HIF_RQB_GET_USER_ID_PTR(pRQB);
	if(pRbTag == LSA_NULL)
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_restore_rqb_user_fields_and_free_return_path(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0);
	}

	HIF_RQB_SET_USER_ID(pRQB, pRbTag->UserId);
	hif_free_rb_tag(pRbTag);
}

/**
 * \brief Replaces the UserId of the RQB by a new RQB tag. Saves the old UserId and stores it inside the tag.
 * 
 * Calls GetSysRequestCbf.
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to process
 * @return
 */
LSA_VOID hif_save_return_path_and_cbf_to_sys_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_SYS_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_save_return_path_and_cbf_to_sys_rqb(+) - pPipe(%d) pRQB(0x%x)", pPipe, pRQB);

	hif_alloc_sys_rb_tag(&pRbTag, pPipe, pRQB);

	if(HIF_IS_NULL(pRbTag))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_save_return_path_and_cbf_to_sys_rqb(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0); /* can't handle => fatal */
	}

	/* backup original user id  */
	pRbTag->UserId = HIF_RQB_GET_USER_ID(pRQB);

	HIF_ASSERT(g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].bUsed == LSA_TRUE);
    HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].Comp.GetSysRequestCbf));
	g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].Comp.GetSysRequestCbf(pPipe->pInstance->hSysDev, pRQB, &pRbTag->pCbf);

	/* remember the source of the RQB in the user id*/
	HIF_RQB_SET_USER_ID_PTR(pRQB, pRbTag);
}

/**
 * \brief Restores the pipe info from the RQB tag structure.
 * 
 * @param [out] ppPipe  - pipe of this channel
 * @param [in] pRQB     - RQB to forward
 * @return
 */
LSA_VOID hif_get_return_path_of_sys_rqb(HIF_PIPE_PTR_TYPE * ppPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_SYS_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_get_return_path_of_sys_rqb(+) - ppPipe(%d) pRQB(0x%x)", ppPipe, pRQB);

	pRbTag = (HIF_SYS_RB_TAG_PTR_TYPE)HIF_RQB_GET_USER_ID_PTR(pRQB);

	if(HIF_IS_NULL(pRbTag))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_get_return_path_of_sys_rqb(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0);
	}

	*ppPipe = pRbTag->pPipe;
}

/**
 * \brief Extracts the RQB Tag from from the UserID, then restores the original UserID.
 * 
 * Undoes what has been done in hif_save_return_path_and_cbf_to_sys_rqb. 
 * Calls ReplaceSysRequestCbf
 * 
 * @param [in] pPipe    - pipe of this channel
 * @param [in] pRQB     - RQB to process
 * @return
 */
LSA_VOID hif_restore_user_fields_of_sys_rqb_and_free_return_path(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_SYS_RB_TAG_PTR_TYPE pRbTag;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_restore_user_fields_of_sys_rqb_and_free_return_path(+) - pPipe(%d) pRQB(0x%x)", pPipe, pRQB);

	pRbTag = (HIF_SYS_RB_TAG_PTR_TYPE)HIF_RQB_GET_USER_ID_PTR(pRQB);

	if(HIF_IS_NULL(pRbTag))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_restore_user_fields_of_sys_rqb_and_free_return_path(-) pRbTag(0x%x) is not valid", pRbTag);
		HIF_FATAL(0);
	}

	HIF_RQB_SET_USER_ID(pRQB, pRbTag->UserId);

	HIF_ASSERT(g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].bUsed == LSA_TRUE);
    HIF_ASSERT(HIF_FCT_IS_NOT_NULL(g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].Comp.ReplaceSysRequestCbf));
	g_pHifData->CompStore.CompSys[HIF_RQB_GET_COMP_ID(pRQB)].Comp.ReplaceSysRequestCbf(pPipe->pInstance->hSysDev, pRQB, pRbTag->pCbf);

	hif_free_sys_rb_tag(pRbTag);
}

#endif // (HIF_CFG_USE_HIF == 1)

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/

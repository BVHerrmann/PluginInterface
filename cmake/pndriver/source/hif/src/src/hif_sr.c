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
/*  F i l e               &F: hif_sr.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements Hif Upper-Lower Send/Receive handling                         */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	6
#define HIF_MODULE_ID		LTRC_ACT_MODUL_ID

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

#if (HIF_CFG_USE_HIF == 1)

static LSA_VOID hif_process_rcv_rqb(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB);

/**
 * \brief Enables the receive mechanism of a HIF instance
 * 
 * (1) If polling is configured, a LSA timer is started. Additional, the internal poll mechanism is triggered once.
 * (2) If interrupts are configured, HIF_HD_ENABLE_ISR / HIF_LD_ENABLE_ISR is called.
 * (3) pHifInst->bRcvEnabled is set to LSA_TRUE.
 * 
 * @param [in] pHifInst - handle to the HIF instance
 *
 * @return LSA_RET_OK if enabled successfully
 *         Error coce otherwise
 */
LSA_UINT16 hif_enable_receive(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 retVal = LSA_RET_OK;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_enable_receive(+) -  pHifInst(0x%x)", pHifInst);

	/* (1) activate Timer Poll */
	if( pHifInst->if_pars.TmOptions.bTimerPoll == LSA_TRUE )
	{
        #if !defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT) && !defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
        /* At least one Instance runs in polling mode. To reduce idle time between two timer calls, add additional trigger RQBs. See HIF_DO_LOW_REQUEST*/
        g_pHifData->internal.bUseInternalPoll = LSA_TRUE;
        #else
        g_pHifData->internal.bUseInternalPoll = LSA_FALSE;
        #endif

		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
            #if !defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
			/* LD Device */
			if(g_pHifData->LD.PollTimer.PollUserCnt == 0)
			{
                LSA_USER_ID_TYPE uTimerHandle;
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - activate hif ld timer poll - hH(%d)",pHifInst->hH);

				uTimerHandle.uvar32 = HIF_LD_TIMER_ID;
				HIF_START_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer, uTimerHandle, HIF_CFG_POLL_INTERVAL);
			}
			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_enable_receive() - HIF Timer for LD couldn't start - hH(%d)",pHifInst->hH);
				return retVal;
			}
			g_pHifData->LD.PollTimer.PollUserCnt++;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - timer ld poll cnt++ - hH(%d)",pHifInst->hH);
            #endif
		}
		else /* HD Device */
		{
            #if !defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
			if(g_pHifData->HD.PollTimer.PollUserCnt == 0)
			{
                LSA_USER_ID_TYPE uTimerHandle;
				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - activate hif hd timer poll - hH(%d)",pHifInst->hH);

				uTimerHandle.uvar32 = HIF_HD_TIMER_ID;
				HIF_START_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer, uTimerHandle, HIF_CFG_POLL_INTERVAL);
			}
			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_enable_receive() - HIF Timer for HD couldn't start - hH(%d)",pHifInst->hH);
				return retVal;
			}
			g_pHifData->HD.PollTimer.PollUserCnt++;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - timer hd poll cnt++ - hH(%d)",pHifInst->hH);
            #endif
		}
	}

	/* (2) enable Interrupts */
	if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
			/* LD Device */
			HIF_LD_ENABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
		else /* HD Device */
		{
			HIF_HD_ENABLE_ISR(pHifInst->hSysDev, pHifInst->hH);	
		}
	}

	/* (3) Set pHifInst->bRcvEnabled */
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_enable_receive() - receive enabled - hH(%d)",pHifInst->hH);

	pHifInst->bRcvEnabled = LSA_TRUE;

    /* Start internal trigger mechanism - this is a HD upper instance */
    hif_start_internal_poll();

	return retVal;
}

/**
 * \brief Disables the receive mechanism of a HIF instance
 * 
 * (1) Deactivate the LSA Timer
 * (2) Disable Interrupts
 * (3) Set pHifInst->bRcvEnabled to LSA_FALSE
 * 
 * @param [in] pHifInst handle to the HIF instance
 * @return LSA_RET_ERR_SEQUENCE - wrong sequence
 * @return LSA_RET_OK           - on success
 *
 * @return LSA_RET_OK if disabled successfully
 *         Error coce otherwise
 */
LSA_UINT16 hif_disable_receive(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 retVal = LSA_RET_OK;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_disable_receive(+) -  pHifInst(0x%x)", pHifInst);

	if(pHifInst->bRcvEnabled == LSA_FALSE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - pHifInst->bRcvEnabled (0x%x) is not enabled, hH(0x%x) ",pHifInst->bRcvEnabled, pHifInst->hH);
		return LSA_RET_ERR_SEQUENCE;
	}

	/* (1) deactivate Timer Poll */
	if(pHifInst->if_pars.TmOptions.bTimerPoll == LSA_TRUE)
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
            #if !defined(HIF_SHM_HSM_ONLY_LD_DEBUG_VARIANT)
			/* LD Device */
			g_pHifData->LD.PollTimer.PollUserCnt--;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer ld poll cnt-- - hH(%d)",pHifInst->hH);

			if(g_pHifData->LD.PollTimer.PollUserCnt == 0)
			{
				HIF_STOP_TIMER(&retVal, g_pHifData->LD.PollTimer.hPollTimer);

				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer ld stop - hH(%d)",pHifInst->hH);
			}

			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - HIF Timer for LD couldn't stopped, hH(0x%x)",pHifInst->hH);
				return retVal;
			}
            #endif
		}
		else /* HD Device */
		{
            #if !defined(HIF_SHM_HSM_ONLY_HD_DEBUG_VARIANT)
			/* deactivate Timer Poll */
			g_pHifData->HD.PollTimer.PollUserCnt--;

			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer hd poll cnt-- - hH(%d)",pHifInst->hH);

			if(g_pHifData->HD.PollTimer.PollUserCnt == 0)
			{
				HIF_STOP_TIMER(&retVal, g_pHifData->HD.PollTimer.hPollTimer);

				HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - timer hd stop - hH(%d)",pHifInst->hH);
			}

			if(retVal != LSA_RET_OK)
			{
                HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_disable_receive() - HIF Timer for HD couldn't stopped, hH(0x%x)",pHifInst->hH);
				return retVal;
			}
            #endif
		}
	}

	/* (2) disable Interrupts */
	if( pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE )
	{
		if(hif_is_ld_device(pHifInst) == LSA_TRUE)
		{
			/* LD Device */

			/* disable Interrupts */
			HIF_LD_DISABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
		else /* HD Device */
		{
			/* disable Interrupts */
			HIF_HD_DISABLE_ISR(pHifInst->hSysDev, pHifInst->hH);
		}
	}

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "hif_disable_receive() - receive disabled - hH(%d)",pHifInst->hH);

	pHifInst->bRcvEnabled = LSA_FALSE;

	return retVal;
}

/**
 * \brief Triggers the sening of a RQB to the other HIF instance
 * This function is used for 
 * - open/close channel RQBs during startup (hif_open_channel, hif_close_channel, hif_send_admin_request)
 * - normal operation (hif_request)
 * - returning the RQB back to the originator (hif_lower_request_done2, hif_lower_open_channel_done, hif_lower_close_channel_done, hif_lower_sys_request_done2)
 *  
 * In mode HIF_TM_SHARED_MEM, hif_shm_send is called. @see hif_shm_send
 * In mode HIF_TM_SHORT, the RQB is enqueued to the other instance
 * 
 * Update statistics:
 * Upper instance: uActOpenRQBs is increased (there are more RQBs in transfer)
 * Lower instance: uActOpenRQBs is decreased (RQBs are returned back)
 * 
 * @param [in] pPipe - pointer to the pipe that is used
 * @param [in] pRQB  - pointer to the RQB that is sent
 */
LSA_VOID hif_send(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	HIF_RQB_PTR_TYPE pRQBTrigger;

	HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE, ">>> hif_send() hH(%d) hdId(%d) pipeId(%d) opc(%d) comp_id(0x%X) pRQB(0x%X)",pPipe->pInstance->hH,pPipe->hd_id,pPipe->pipe_id,HIF_RQB_GET_OPCODE(pRQB),pPipe->comp_id,pRQB);

    if(hif_is_upper_pipe(pPipe) == LSA_TRUE) /* Upper Pipe sending RQBs increases act open RQB count */
	{
        pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs++;
	}
	else /* Lower pipe returning RQB reduces act RQB count */
	{
		HIF_ASSERT(pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs > 0);
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs--;
	}

    if(pPipe->pInstance->Mode == HIF_TM_SHARED_MEM)
	{
		/* Always add send RQBs to Send Queue and then try to transmit them */
		(LSA_VOID)hif_add_to_rqb_queue(&pPipe->pRQBRingList->pSndQueue, (HIF_RQB_PTR_TYPE)pRQB, pPipe->PipeIndex);

		hif_shm_send(pPipe->pInstance);
	}
	else /* pPipe->Mode == HIF_TM_SHORT */
	{
		/* backup/overwrite fields which should not be transported (handle, user_id) */
		pPipe->short_circuit_data_backup.hLower = HIF_RQB_GET_HANDLE(pRQB);
		HIF_RQB_SET_HANDLE(pRQB, 0xEE);

		/* => absolutely hif_enter critically! */

		/* add to target rcv list of target hif instance */
        (LSA_VOID)hif_add_to_rqb_queue(&pPipe->pInstance->if_pars.pHifPartner->if_pars.SharedMem.RQBRingList.pRcvList, pRQB, pPipe->PipeIndex);

		if(HIF_IS_NOT_NULL(pPipe->pInstance->if_pars.pHifPartner->pRQBShort))
		{
			pRQBTrigger = pPipe->pInstance->if_pars.pHifPartner->pRQBShort;
			pPipe->pInstance->if_pars.pHifPartner->pRQBShort = LSA_NULL;

			/* Send trigger to other HIF device */
			pPipe->pInstance->func.DO_REQUEST_LOCAL(pRQBTrigger);
		}

		/* <= hif_exit */
	}
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE,"<<< hif_send() ");
}

/**
 * \brief This function is called during the internal OPC HIF_OPC_RECEIVE.
 * 
 * Takes RQB from the pRcvList and processes them.
 * @see hif_process_rcv_rqb - This function calls hif_process_rcv_rqb for each RQB from the pRcvList.
 * 
 * @param [in] pHifInst - Handle to the HIF instance
 */
LSA_VOID hif_process_rcv_list(HIF_INST_PTR_TYPE const pHifInst)
{
	HIF_RQB_PTR_TYPE  pRQB;
    LSA_UINT16        PipeIndex;
    HIF_PIPE_PTR_TYPE pPipe;

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_process_rcv_list() - hH(%d)", pHifInst->hH);

	hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pRcvList, &pRQB, &PipeIndex);

	while(HIF_IS_NOT_NULL(pRQB))
	{
        //Get Pipe
        pPipe = &pHifInst->Pipes[PipeIndex];

		hif_process_rcv_rqb(pPipe, pRQB);

		hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pRcvList, &pRQB, &PipeIndex);
	}
}

/**
 * \brief Processes a single RQB from the transfer memory
 * 
 * Detailed description see function block.
 * 
 * Update statistics:
 * Upper instance: uActOpenRQBs is increased (there are more RQBs in transfer)
 * Lower instance: uActOpenRQBs is decreased (RQBs are returned back)
 * 
 * @param [inout] pPipe - current pipe
 * @param [in] pRQB     - pointer to the RQB that is processed
 */
static LSA_VOID hif_process_rcv_rqb(HIF_PIPE_PTR_TYPE const pPipe, HIF_RQB_PTR_TYPE const pRQB)
{
	HIF_PROGRAM_TRACE_06(0, LSA_TRACE_LEVEL_NOTE, "hif_process_rcv_rqb() - receive hH(%d) hdId(%d) pipeId(%d) opc(%d) comp_id(0x%X) pRQB(0x%X)", pPipe->pInstance->hH, pPipe->hd_id, pPipe->pipe_id, HIF_RQB_GET_OPCODE(pRQB), pPipe->comp_id, pRQB);

	if(hif_is_upper_pipe(pPipe) == LSA_TRUE) /* Upper Pipe receive reduces act open RQB count */
	{
		HIF_ASSERT(pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs > 0);
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs--;
	}
	else /* Lower Pipe receive increases act open RQB count */
	{
		pPipe->pInstance->if_pars.SharedMem.uActOpenRQBs++;
	}

	/* (1) Handle system requests from the admin pipe */
	if(pPipe->pipe_id == HIF_GLOB_ADMIN)
	{
		/* (1a) HIF Admin / Sys requests */
		if(HIF_RQB_GET_COMP_ID(pRQB) == LSA_COMP_ID_HIF) /* HIF Internal Admin Request */
		{
			if(hif_is_upper_device(pPipe->pInstance))
			{
				hif_send_admin_request_done(pPipe->pInstance, pRQB);
			}
			else
			{
				hif_receive_admin_request(pPipe->pInstance, pRQB);
			}
		}
		else /* (1b) Sys Request using HIF Admin Pipe */
		{
			if(hif_is_upper_device(pPipe->pInstance))
			{
				hif_upper_sys_request_done(pPipe, pRQB);
			}
			else
			{
				hif_lower_sys_request(pPipe, pRQB);
			}
		}
	}
	else /* (2) handle other RQBs from the components */
	{
		/* All other requests */

	    /* (3) Channel is closed -> Open the pipe, then call the OPEN_CHANNEL_LOWER function */
		if(pPipe->State == HIF_PIPE_STATE_CLOSED)
		{
			hif_lower_open_channel(pPipe, pRQB);
		}
		/* (4) HIF Upper: */
		else if(hif_is_upper_pipe(pPipe) == LSA_TRUE)
		{
			/* Recover RQB in case of short curcuit mode */
			if(pPipe->pInstance->Mode == HIF_TM_SHORT)
			{
				HIF_RQB_SET_HANDLE(pRQB, pPipe->short_circuit_data_backup.hLower);
			}
			/* (4a) Open channel was successful, update pipe state, then return RQB to the originator */
			if(pPipe->State == HIF_PIPE_STATE_OPENING_UPPER)
			{
				hif_upper_open_channel_done(pPipe, pRQB);
			}
			else
			{
                HIF_ASSERT(HIF_IS_NOT_NULL(pPipe->func.pCompFuncs));
                HIF_ASSERT(HIF_FCT_IS_NOT_NULL(pPipe->func.pCompFuncs->IsCloseChannelRqb));
				/* (4b) Close channel RQB. Special handling to close the pipe before returning the RQB to the originator */
				if(pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE)
				{
					/* close channel request returns from lower  */
					hif_upper_close_channel_done(pPipe, pRQB);
				}
				/* (4c) Normal operation -> give RQB back to the originator */
				else
				{
					/* request returns from lower => Call hif_callback to forward RQB to the originator */
					hif_upper_pipe_request_done(pPipe, pRQB);
				}
			}
		}
		/* (5) HIF Lower */
		else
		{
            HIF_ASSERT(HIF_IS_NOT_NULL(pPipe->func.pCompFuncs));
            HIF_ASSERT(HIF_FCT_IS_NOT_NULL(pPipe->func.pCompFuncs->IsCloseChannelRqb));
			/* (5a) Close channel RQB. Call CLOSE_CHANNEL_LOWER function */
			if(pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE)
			{
				hif_lower_close_channel(pPipe, pRQB);
			}
			/* (5b) Normal operation. Call REQUEST_LOWER */
			else
			{
				/* receiving request from upper => forward to component */
				hif_lower_pipe_request(pPipe, pRQB);
			}
		}
	}	// endif pipe is HIF_GLOB_ADMIN
}

/**
 * \brief Sets up the parameters of all pipes of a HIF instance by copying the values from HIF_GET_PAR_PTR_TYPE.
 * 
 * Detailed description see function block.
 * 
 * @param [in] pHifInst - handle to the HIF instance
 * @param [in] pPars    - structure from HIF_GET_PARAMS
 *
 * @return LSA_RET_ERR_RESOURCE - wrong internal configuration, see trace 
 * @return LSA_RET_ERR_PARAM    - wrong configuration from HIF_GET_PAR_PTR_TYPE, see trace
 * @return LSA_RET_OK           - success
 */
LSA_UINT16 hif_setup_transfer_mode(HIF_INST_PTR_TYPE pHifInst, HIF_GET_PAR_PTR_TYPE pPars)
{
	LSA_UINT16 i;
	LSA_VERSION_TYPE version;
	HIF_SHM_HEADER_TAG shmHeaderRemote;
	HIF_SHM_HEADER_TAG *pShmHeaderLocal = LSA_NULL;
	LSA_UINT8 lVersion[26];
	LSA_UINT32 uIfIdent;
	HIF_RING_BUFFER_PTR_TYPE pRcvRing;
	HIF_RING_BUFFER_PTR_TYPE pSndRing;

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_setup_transfer_mode() - hH(%d), pPars(0x%x)",pHifInst->hH, pPars);

	/* Basic Params Check */

    //!!!AH!!! HIF_TM_MIXED und HIF_TM_SHARED_MEM_NO_SER entfernen!

	if(pPars->Mode == HIF_TM_MIXED)
	{
		for(i=0; i<HIF_MAX_PIPES_INST; i++) /* In mixed mode each Pipe has to have a specific transfer mode */
		{
			if( (pPars->if_pars.Pipes[i].Mode != HIF_TM_SHORT) &&
				(pPars->if_pars.Pipes[i].Mode != HIF_TM_SHARED_MEM) )
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_setup_transfer_mode(-) unknown pipe transfer mode"); 
				return LSA_RET_ERR_RESOURCE;
			}
		}
	}

	/* Init TransferMode Options */
	/*
		bSerialization
			LSA_TRUE  = Use RQB De- Serialization on shared memory receive/send
		    LSA_FALSE = Only shared memory signalling is used. The RQB Address and it's Buffers need to be accessible on the receivers side
		bTimerPoll
			LSA_TRUE  = Use addtional polling for shared memory send (for queued sends) / receive. This is the fallback in case of missed interrupts.
		    LSA_FALSE = No polling => no fallback!
		bShmHandling
			LSA_TRUE  = Use shared nemory handling
		    LSA_FALSE = No shared memory handling
		bShmSndWaitAck
			LSA_TRUE  = Sends an interrupt (Acknowledge) from the receiver to the sender side when an request is received => Acknowledge. 
			            The sender doesn't send any further requests until it gets the acknowledge. On receiving the Ack the sender tries to process it's send queues.
		    LSA_FALSE = No Ack signalling => in case a send Req is queued you will need the bTimerPoll to get it processed. 
	*/

	pHifInst->Mode = pPars->Mode;
	if(pPars->Mode != HIF_TM_SHORT) // for short driver get_params are not called -> if_pars can't be used in short mode! 
	{
		pHifInst->if_pars.TmOptions.bTimerPoll = pPars->if_pars.SharedMem.bUseTimerPoll;
		pHifInst->if_pars.TmOptions.SendIrqMode = pPars->if_pars.SharedMem.SendIrqMode;
		pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq = pPars->if_pars.SharedMem.uNsWaitBeforeSendIrq;
	
		if( (pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_NO_WAIT) &&
			(pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_READ_BACK) &&    
			(pHifInst->if_pars.TmOptions.SendIrqMode != HIF_SEND_IRQ_AFTER_TIMEOUT) )
		{
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unexpected SendIrqMode=%d", pHifInst->if_pars.TmOptions.SendIrqMode);
			return LSA_RET_ERR_PARAM;
		}
	}

	switch(pHifInst->Mode)
	{
		case HIF_TM_MIXED:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_TRUE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined(HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		case HIF_TM_SHARED_MEM:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_TRUE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined(HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		case HIF_TM_SHORT:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bTimerPoll                 = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_FALSE;
            #if defined(HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_FALSE;
            #endif
			break;
		case HIF_TM_SHARED_MEM_NO_SER:
			pHifInst->if_pars.TmOptions.bSerialization             = LSA_FALSE;
			pHifInst->if_pars.TmOptions.bShmHandling               = LSA_TRUE;
            #if defined(HIF_CFG_USE_SND_WAIT_ACK)
			pHifInst->if_pars.TmOptions.bShmSndWaitAck             = LSA_TRUE;
            #endif
			break;
		default:
            {
			    HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unknown transfer mode"); 
			    return LSA_RET_ERR_RESOURCE;
            }
			// break; --> warning #111-D: statement is unreachable
	}

	#if (HIF_CFG_COMPILE_SERIALIZATION == 0)
	if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_setup_transfer_mode() - misconfiguration => serialization is not compiled but should be used");
		HIF_FATAL(0);
	}
	#endif

	/* Copy / Init basic input pars */
	pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase  = LSA_NULL;
	pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = LSA_NULL;

	if( (pHifInst->Mode == HIF_TM_MIXED) ||
		(pHifInst->Mode == HIF_TM_SHARED_MEM) ||
		(pHifInst->Mode == HIF_TM_SHARED_MEM_NO_SER) )
	{
		/* Initialize parameters for local and remote shared memory */
		pHifInst->if_pars.SharedMem.ShmLocal.pBase       = pPars->if_pars.SharedMem.Local.Base;
		pHifInst->if_pars.SharedMem.ShmLocal.uSize       = pPars->if_pars.SharedMem.Local.Size;
		pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader  = (HIF_SHM_HEADER_TAG*)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmLocal.pBase);
		pHifInst->if_pars.SharedMem.ShmRemote.pBase      = pPars->if_pars.SharedMem.Remote.Base;
		pHifInst->if_pars.SharedMem.ShmRemote.uSize      = pPars->if_pars.SharedMem.Remote.Size;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = (HIF_SHM_HEADER_TAG*)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmRemote.pBase);
	}
	else
	{
		/* Set parameters for shared memory to 0 */
		pHifInst->if_pars.SharedMem.ShmLocal.pBase       = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmLocal.uSize       = 0;
		pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader  = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmRemote.pBase      = LSA_NULL;
		pHifInst->if_pars.SharedMem.ShmRemote.uSize      = 0;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = LSA_NULL;
	}

	/* Set Pipe Modes */
	if(pPars->Mode == HIF_TM_SHORT)
	{
		pHifInst->Mode = HIF_TM_SHORT;
	}
	else /* HIF_TM_SHARED_MEM, HIF_TM_SHARED_MEM_NO_SER Modes are not divided on pipe level */
	{
		pHifInst->Mode = HIF_TM_SHARED_MEM;
	}

	/* Init Short Mode */
	if( (pHifInst->Mode == HIF_TM_MIXED) ||
	    (pHifInst->Mode == HIF_TM_SHORT) )
	{
		if(hif_is_upper_device(pHifInst) == LSA_TRUE)
		{
			hif_get_instance(&pHifInst->if_pars.pHifPartner, pPars->if_pars.hif_lower_device);
			if(HIF_IS_NULL(pHifInst->if_pars.pHifPartner))
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) could not find hif short partner");
				return LSA_RET_ERR_RESOURCE;
			}
			else /* HIF partner OK */
			{
				/* absolutely HIF_ENTER critically! */
                //!!!AH evtl. mutex???
				if(hif_is_ld_device(pHifInst) == LSA_TRUE)
				{
					if(pHifInst->if_pars.pHifPartner->Type != HIF_DEV_TYPE_LD_LOWER)
					{
						HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unmatching types");
						return LSA_RET_ERR_PARAM;
					}
				}
				else
				{
					if(pHifInst->if_pars.pHifPartner->Type != HIF_DEV_TYPE_HD_LOWER)
					{
                        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) unmatching types");
						return LSA_RET_ERR_PARAM;
					}
				}

				pHifInst->if_pars.pHifPartner->if_pars.pHifPartner = pHifInst;
			}
		}
	}

	/* Init Shared Mem Mode */
	if(pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE)
	{
		/*
			In shared memory mode HIF uses 2 shared memory buffers to eliminate slow pci(e) reads:
			1) Local  => this memory should be located in the local system and is almost read only because it's 
			             written by the other HIF instance, exceptions to read only are the trigger vars
			2) Remote => this memory is located at the target system which is for example connected by PCI(e).
			             The remote memory is write only!

			Structure of shared mem:
			Upper => ( Header | Ring List | RQB Buffer )
			Lower => ( Header | [otpional Pipe Header] | Ring List | RQB Buffer )
		*/

		
		/* Check if shared mem base ptr are set */
		if(HIF_IS_NULL(pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader) || HIF_IS_NULL(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader))
		{
		    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) pShmHeader (local) (0x%x) or pShmHeader (remote) (0x%x) is invalid", pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader, pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader);
			return LSA_RET_ERR_PARAM;
		}

		/* Check Min Shared Mem size */
		if(pHifInst->if_pars.SharedMem.ShmRemote.uSize < (sizeof(HIF_SHM_HEADER_TAG) + (sizeof(HIF_RING_BUFFER_TYPE))))
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) uSize (remote) (0x%x) is invalid", pHifInst->if_pars.SharedMem.ShmRemote.uSize);
			return LSA_RET_ERR_RESOURCE;
        }

		if(pHifInst->if_pars.SharedMem.ShmLocal.uSize < (sizeof(HIF_SHM_HEADER_TAG) + (sizeof(HIF_RING_BUFFER_TYPE))))
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) uSize (local) (0x%x) is invalid", pHifInst->if_pars.SharedMem.ShmLocal.uSize);
            return LSA_RET_ERR_RESOURCE;
        }

		/* Get PNIO Version */
		(LSA_VOID)pnio_version(sizeof(LSA_VERSION_TYPE), &version);
		lVersion[0]  = (LSA_UINT8)(version.lsa_component_id >> 8);
		lVersion[1]  = (LSA_UINT8)(version.lsa_component_id & 0xff);
		lVersion[2]  = (LSA_UINT8)version.lsa_prefix[0];
		lVersion[3]  = (LSA_UINT8)version.lsa_prefix[1];
		lVersion[4]  = (LSA_UINT8)version.lsa_prefix[2];
		lVersion[5]  = (LSA_UINT8)version.lsa_prefix[3];
		lVersion[6]  = (LSA_UINT8)version.lsa_prefix[4];
		lVersion[7]  = (LSA_UINT8)version.lsa_prefix[5];
		lVersion[8]  = (LSA_UINT8)version.lsa_prefix[6];
		lVersion[9]  = (LSA_UINT8)version.lsa_prefix[7];
		lVersion[10] = (LSA_UINT8)version.lsa_prefix[8];
		lVersion[11] = (LSA_UINT8)version.lsa_prefix[9];
		lVersion[12] = (LSA_UINT8)version.lsa_prefix[10];
		lVersion[13] = (LSA_UINT8)version.lsa_prefix[11];
		lVersion[14] = (LSA_UINT8)version.lsa_prefix[12];
		lVersion[15] = (LSA_UINT8)version.lsa_prefix[13];
		lVersion[16] = (LSA_UINT8)version.lsa_prefix[14];
		lVersion[17] = version.kind;
		lVersion[18] = version.version;
		lVersion[19] = version.distribution;
		lVersion[20] = version.fix;
		lVersion[21] = version.hotfix;
		lVersion[22] = version.project_number;
		lVersion[23] = version.increment;
		lVersion[24] = version.integration_counter;
		lVersion[25] = version.gen_counter;

		/* Clear Own Shared Mem Header */
		HIF_MEMSET((LSA_VOID*)&shmHeaderRemote, 0, sizeof(HIF_SHM_HEADER_TAG));
		
		/* Init quick reference to local shared mem header */
		/* Be careful => on an upper device the local shared mem header already has valid information */
		/*               on an lower device the local shared mem header is not initialised            */
		pShmHeaderLocal  = pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader;

		/* Setup trigger vars which will be used in timer poll mechanism */
		pHifInst->if_pars.SharedMem.pSndRQ                  = &(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.uRqbRQ);
		pHifInst->if_pars.SharedMem.pRcvRQ                  = &pShmHeaderLocal->tm_control.uRqbRQ;
        
		/* Setup trigger vars which will be used in interrupt mechanism */ 
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFull = 0;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFullAck = 0;

        #if defined(HIF_CFG_USE_SND_WAIT_ACK)
        pHifInst->if_pars.SharedMem.pSndRQAck               = &pShmHeaderLocal->tm_control.uRqbRQ_Ack;
		pHifInst->if_pars.SharedMem.pRcvRQAck               = &(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.uRqbRQ_Ack);
        #endif

		if(hif_is_upper_device(pHifInst) == LSA_TRUE)
		{
			/* Check Size of local shm header */
			if(pShmHeaderLocal->common.uHeaderLen != sizeof(HIF_SHM_HEADER_TAG))
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.uHeaderLen (0x%x) is invalid! Nedded uHeaderLen (0x%x)",pShmHeaderLocal->common.uHeaderLen, sizeof(HIF_SHM_HEADER_TAG));
				return LSA_RET_ERR_PARAM;
            }
			/* Check State */
			if(pShmHeaderLocal->common.State != HIF_SHM_STATE_DEVICE_OPENED)
            {
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.State (0x%x) is invalid! Nedded State (0x%x)",pShmHeaderLocal->common.State, HIF_SHM_STATE_DEVICE_OPENED);
				return LSA_RET_ERR_SEQUENCE;
            }

			/* Check Role */
			if(pShmHeaderLocal->common.IsUpper != 0)
            {
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->common.IsUpper is != 0");
				return LSA_RET_ERR_PARAM;
            }

			/* Check Shared Mem Header */
			uIfIdent = hif_get_if_ident(pHifInst); /* the Interface Ident contains configuration data which must match between upper and lower HIF instance */
			if(pShmHeaderLocal->dev_specific.lower.uIfIdent != uIfIdent)
			{
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_setup_transfer_mode(-) - Interface Ident doesn't match (0x%x/0x%x), check for ld/hd and HIF_USE compiler switches",uIfIdent,pShmHeaderLocal->dev_specific.lower.uIfIdent);
				HIF_FATAL(0);
			}

			if(pShmHeaderLocal->dev_specific.lower.uPnioVersionLen != 26)
            {
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - pShmHeaderLocal->dev_specific.lower.uPnioVersionLen != 26");
				return LSA_RET_ERR_RESOURCE;		
            }
			
			/* Compare PNIO Version */
			if(HIF_MEMCMP(lVersion, pShmHeaderLocal->dev_specific.lower.uaPnioVersion, 26) > 0)
			{
                HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - Wrong PNIO Version");
				return LSA_RET_ERR_PARAM;
			}

			/* Create Own Shared Mem Header */
			shmHeaderRemote.common.uHeaderLen     = sizeof(HIF_SHM_HEADER_TAG);
			shmHeaderRemote.common.IsUpper        = 1;
			shmHeaderRemote.common.uRingBufferLen = sizeof(HIF_RING_BUFFER_TYPE);
			shmHeaderRemote.common.uRQBBufferLen  = (pHifInst->if_pars.SharedMem.ShmRemote.uSize - shmHeaderRemote.common.uHeaderLen) - shmHeaderRemote.common.uRingBufferLen;

			/* Check: Ring buffer lengths should be identical */
			if(shmHeaderRemote.common.uRingBufferLen != pShmHeaderLocal->common.uRingBufferLen)
			{
                HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_ERROR, "hif_setup_transfer_mode(-) - shmHeaderRemote.common.uRingBufferLen (0x%x) and pShmHeaderLocal->common.uRingBufferLen (0x%x) mismatch!",shmHeaderRemote.common.uRingBufferLen, pShmHeaderLocal->common.uRingBufferLen);
				return LSA_RET_ERR_PARAM;
			}

			/* Find RQB data base address for mem pool creation  */
			/* Shared mem pools are only needed in case of serialization */
			if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
			{
				if(pHifInst->Mode == HIF_TM_MIXED)
				{
					pHifInst->if_pars.SharedMem.ShmRemote.hPool = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.hPool; /* Use lower pool as there is only 1 available in mixed mode */
					pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.pHifPartner->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
				}
				else
				{
					pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.common.uRingBufferLen;                   
                    
                    // create the transfer memory pool for this instance
                    pHifInst->if_pars.SharedMem.ShmRemote.hPool = hif_mem_ring_pool_create((LSA_VOID*)pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, shmHeaderRemote.common.uRQBBufferLen);
					HIF_ASSERT(pHifInst->if_pars.SharedMem.ShmRemote.hPool != -1);
					
                    pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmLocal.pBase + pShmHeaderLocal->common.uHeaderLen + pShmHeaderLocal->common.uRingBufferLen + pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen;
				}
			}

			shmHeaderRemote.tm_control.uRqbRQ_Ack = 1; /* Set initial acknowledge */

			/* Lock SHM interface for this HIF Inst */
			shmHeaderRemote.common.State = HIF_SHM_STATE_DEVICE_OPENED;
		}
		else
		{
			/* Create Shared Mem Header */
			shmHeaderRemote.common.uHeaderLen                   = sizeof(HIF_SHM_HEADER_TAG);
			shmHeaderRemote.common.uRingBufferLen               = sizeof(HIF_RING_BUFFER_TYPE);
			shmHeaderRemote.common.IsUpper                      = 0;
			shmHeaderRemote.dev_specific.lower.uIfIdent         = hif_get_if_ident(pHifInst);
			shmHeaderRemote.dev_specific.lower.uPnioVersionLen  = 26;
			shmHeaderRemote.dev_specific.lower.uPipeHeaderLen   = 0;

			/* Store local pnio version to shared mem */
			HIF_MEMCPY(shmHeaderRemote.dev_specific.lower.uaPnioVersion, lVersion, 26);

			/* In case of serialization we need to create mem pools in the shared memory */
			if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
			{
				shmHeaderRemote.common.uRQBBufferLen = (pHifInst->if_pars.SharedMem.ShmRemote.uSize - (shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen + shmHeaderRemote.common.uRingBufferLen));

				/* Find RQB data base address for mem pool creation  */
				pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen + shmHeaderRemote.common.uRingBufferLen;

                // create the transfer memory pool for this instance
				pHifInst->if_pars.SharedMem.ShmRemote.hPool = hif_mem_ring_pool_create((LSA_VOID*)pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, shmHeaderRemote.common.uRQBBufferLen);
				HIF_ASSERT(pHifInst->if_pars.SharedMem.ShmRemote.hPool != -1);
				
				if(pHifInst->Mode == HIF_TM_MIXED)
				{
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase;
				}
				else
				{
					pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase = pHifInst->if_pars.SharedMem.ShmLocal.pBase + sizeof(HIF_SHM_HEADER_TAG)/* (LaM) pShmHeaderLocal->common.uHeaderLen is not available here*/ + sizeof(HIF_RING_BUFFER_TYPE) /* pShmHeaderLocal->common.uRingBufferLen not available here */;
				}
			}

			/* Enable SHM interface */
			shmHeaderRemote.common.State = HIF_SHM_STATE_DEVICE_OPENED;
		}

        /* Calculate ring */
        HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList, 0, sizeof(HIF_SHM_RQB_RINGLIST_TAG_TYPE));

		if(pHifInst->Mode == HIF_TM_SHARED_MEM)
		{
			if(hif_is_upper_device(pHifInst) == LSA_TRUE)
			{
				pRcvRing = (HIF_RING_BUFFER_PTR_TYPE)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmLocal.pBase + pShmHeaderLocal->common.uHeaderLen + pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen );
				pSndRing = (HIF_RING_BUFFER_PTR_TYPE)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen );
			}
			else
			{
				pRcvRing = (HIF_RING_BUFFER_PTR_TYPE)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmLocal.pBase + sizeof(HIF_SHM_HEADER_TAG)/* (LaM) pShmHeaderLocal->common.uHeaderLen is not available here*/);
				pSndRing = (HIF_RING_BUFFER_PTR_TYPE)HIF_CAST_TO_VOID_PTR(pHifInst->if_pars.SharedMem.ShmRemote.pBase + shmHeaderRemote.common.uHeaderLen + shmHeaderRemote.dev_specific.lower.uPipeHeaderLen );

				/* Ring structures are initialized by lower device because the lower device is first available */
				HIF_MEMSET(pRcvRing, 0, sizeof(HIF_RING_BUFFER_TYPE)); /* Clear own ring structure in shared memory */
				HIF_MEMSET(pSndRing, 0, sizeof(HIF_RING_BUFFER_TYPE)); /* Clear remote ring structure in shared memory */	
			}

			/* Create local structures simplifying the access to the rings */
			HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList.SendRing, 0, sizeof(HIF_PIPE_SHM_SEND_RING_TYPE));
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pRingWriteOnly     = pSndRing->rqb;
            pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pPipeIndexWriteOnly = pSndRing->PipeIndex;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly    = &pSndRing->wi;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pRiShmReadOnly     = &pRcvRing->ri;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWriteOnlyRing     = pSndRing;
			pHifInst->if_pars.SharedMem.RQBRingList.SendRing.wi = *pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly; /* (LaM) not nice but needed */
				
			HIF_MEMSET(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, 0, sizeof(HIF_PIPE_SHM_RECEIVE_RING_TYPE));
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRingReadOnly   = pRcvRing->rqb;
            pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pPipeIndexReadOnly = pRcvRing->PipeIndex;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly = &pSndRing->ri;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pWiShmReadOnly  = &pRcvRing->wi;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pReadOnlyRing   = pRcvRing;
			pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.ri = *pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly; /* (LaM) not nice but needed */

		    /* Map one ringlist to all channels of it's own interface */
		    for(i=0; i<HIF_MAX_PIPES_INST; i++)
		    {
                pHifInst->Pipes[i].pRQBRingList = &pHifInst->if_pars.SharedMem.RQBRingList;
            }
		}

		HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_setup_transfer_mode() Local  Shm: hH(%d) ShmBase(0x%8x) ShmLen(0x%8x) RqbBufferBase(0x%8x(calculated))",pHifInst->hH, pShmHeaderLocal, pHifInst->if_pars.SharedMem.ShmLocal.uSize, pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase);
		HIF_PROGRAM_TRACE_10(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_setup_transfer_mode() Remote Shm: hH(%d) ShmBase(0x%8x) ShmLen(0x%8x) RqbBufferBase(0x%8x) isUpper(%d) HeaderLen(0x%8x) RingLen(0x%8x) PipeHeaderLen(0x%8x) RqbBufferLen(0x%8x) PoolHandle(0x%8x) ",pHifInst->hH, pHifInst->if_pars.SharedMem.ShmRemote.pBase, pHifInst->if_pars.SharedMem.ShmRemote.uSize, pHifInst->if_pars.SharedMem.ShmRemote.pRqbBufferBase, pShmHeaderLocal->common.IsUpper, pShmHeaderLocal->common.uHeaderLen, pShmHeaderLocal->common.uRingBufferLen, pShmHeaderLocal->dev_specific.lower.uPipeHeaderLen, pShmHeaderLocal->common.uRQBBufferLen, pHifInst->if_pars.SharedMem.ShmRemote.hPool);

		*pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = shmHeaderRemote; /* Write Shm Header to shared memory */
		#if defined(HIF_CFG_USE_SND_WAIT_ACK)
        *pHifInst->if_pars.SharedMem.pSndRQAck = 1; /* Set initial send acknowledge (myself) so that the processing can start */
        #endif
	}

	return LSA_RET_OK;
}

/**
 * \brief Sends all RQBs that are still in the send queue, sets device state to HIF_SHM_STATE_DEVICE_NOT_READY, then deletes the transfer memory.
 * 
 * @param [in] pHifInst - current HIF instance
 *
 * @return LSA_RET_OK - success 
 */
LSA_UINT16 hif_finish_transfer_mode(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_UINT16 uRetVal=LSA_RET_OK;

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_finish_transfer_mode() - hH(%d)",pHifInst->hH);

	if(pHifInst->Mode == HIF_TM_SHARED_MEM)
	{
        hif_shm_ring_buffer_send_clean_all_elements(pHifInst);
    }

	if(HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader))
	{
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->common.State = HIF_SHM_STATE_DEVICE_NOT_READY;
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader = LSA_NULL;
	}

	if( ( (pHifInst->Mode == HIF_TM_MIXED) &&               /* In Mixed mode the lower device kills the pool */
	      (hif_is_upper_device(pHifInst) == LSA_FALSE) ) ||
		  (pHifInst->Mode == HIF_TM_SHARED_MEM) )           /* In SHM mode both have to kill their pool */
	{
		/* Free the RQB Pool - Only one is currently supported */
	    hif_mem_ring_pool_delete(pHifInst->if_pars.SharedMem.ShmRemote.hPool);
	    uRetVal = LSA_RET_OK;
	}

	return uRetVal;
}

/**
 * \brief Sends a single RQB to the other HIF instance by serializing the RQB into the transfer memory. 
 * 
 * (1) Clean up the transfer memory
 * (2) Serialize the RQB into the transfer memory
 * (2b) Trigger a DMA transfer from the shadow buffer into the remote memory
 * 
 * This function is on the critical path, if HIF_DEBUG_MEASURE_PERFORMANCE is set, the serialization time is measured.
 * 
 * @param [in] pPipe            - current pipe
 * @param [in] pRQB             - RQB that must be serialized into the transfer memory
 *
 * @return LSA_RET_ERR_RESOURCE - not enough transfer memory
 * @return LSA_RET_OK           - success
 */
LSA_UINT16 hif_shm_send_rqb(HIF_PIPE_PTR_TYPE pPipe, HIF_RQB_PTR_TYPE pRQB)
{
	LSA_VOID_PTR_TYPE pShmSerRqb = LSA_NULL;
	LSA_UINT32 shmSerRqbOffset = 0;
	LSA_UINT32 shmSerRqbLen = 0;
	LSA_UINT32 is_full = 0;
	LSA_UINT16 ret_val;

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	LSA_UINT64 start_count_ser, end_count_ser;
	#endif
	LSA_UINT64 end_count_total;
	LSA_UINT64 start_count_total = HIF_GET_NS_TICKS();
	#endif
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,">>> hif_shm_send_rqb() ");

	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	/* (2) Serialize the RQB into the transfer memory */
	if(pPipe->pInstance->if_pars.TmOptions.bSerialization == LSA_TRUE)
	{
    	LSA_VOID_PTR_TYPE pShmSerRqbReal;

		/* copy the RQB to the shared memory and return a pointer (rqb_shm) */
        HIF_ASSERT(HIF_FCT_IS_NOT_NULL(pPipe->func.Serialize));
		
		//Measure the serialzation performance
		#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
		start_count_ser = HIF_GET_NS_TICKS();
		ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		end_count_ser = HIF_GET_NS_TICKS();
		HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_send_rqb(): Serialization of the RQB took %d ns",(end_count_ser - start_count_ser));
		#else
		ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		#endif

        if(ret_val == LSA_RET_ERR_RESOURCE)
        {
            // no more memory, try to clean all elements
            hif_shm_ring_buffer_send_clean_all_elements(pPipe->pInstance);
            // and try to serialize again
		    //Measure the serialzation performance
		    #if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
		    start_count_ser = HIF_GET_NS_TICKS();
		    ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		    end_count_ser = HIF_GET_NS_TICKS();
		    HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_send_rqb(): Serialization of the RQB took %d ns",(end_count_ser - start_count_ser));
		    #else
		    ret_val = pPipe->func.Serialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pPipe->pInstance->if_pars.SharedMem.ShmRemote.hPool, pRQB, &pShmSerRqb, &shmSerRqbLen);
		    #endif
        }

        pShmSerRqbReal = pShmSerRqb;

		/* add rqb to the ring buffer (only rqb offset to the beginning of shared memory is stored in the ring buffer */
		shmSerRqbOffset = (LSA_UINT32)((LSA_UINT8*)pShmSerRqbReal - (LSA_UINT8*)pPipe->pInstance->if_pars.SharedMem.ShmRemote.pRqbBufferBase);
	}
	else
	#endif
	{
		shmSerRqbOffset = (LSA_UINT32)pRQB;
		pShmSerRqb = pRQB;
		ret_val = LSA_RET_OK;
	}

	if(ret_val == LSA_RET_OK) /* Serialization OK */
	{
		HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE, "hif_shm_send_rqb() rqb(%d) hH(%d) pipe(%d) comp_id(0x%X) ring(0x%X) ring_index(%d) shm_buff(0x%08x) shm_buff_o(0x%08x) len(%d)",pRQB, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id, pPipe->pRQBRingList->SendRing.pRingWriteOnly, pPipe->pRQBRingList->SendRing.wi, pShmSerRqb, shmSerRqbOffset, shmSerRqbLen);

		is_full = hif_shm_ring_buffer_send_enqueue(&pPipe->pRQBRingList->SendRing, shmSerRqbOffset, (LSA_UINT32)pShmSerRqb, pPipe->PipeIndex);
		if(is_full) /* can't be full since we checked this above */
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_send_rqb() - hif_shm_ring_buffer_send_enqueue returned is full");
			HIF_FATAL(0);
		}
	} 
    else if(ret_val == LSA_RET_ERR_RESOURCE) /* Not enough available shared mem */
	{
        // trace it as WARN (not ERROR) beacause the sending will be added to shared mem send queue
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "hif_shm_send_rqb(-) Not enough available shared mem");
        return LSA_RET_ERR_RESOURCE;
	}
	else /* Serialization failed for unknown reason */
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_send_rqb(-) Serialization failed for unknown reason");
		HIF_FATAL(0);
	}

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_total = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_send_rqb(): The total function duration is %d ns",(end_count_total - start_count_total));
	#else
	HIF_PROGRAM_TRACE_00(0,LSA_TRACE_LEVEL_NOTE_LOW,"<<< hif_shm_send_rqb() ");
	#endif

	return LSA_RET_OK;
}

/**
 * \brief Send all pending RQBs from the send queue to the other HIF instance
 * 
 * (1) For all RQBs in the send queue:
 *  (1a) Get RQB from queue, @see hif_get_from_rqb_queue
 *  (1b) Send RQB, @see hif_shm_send_rqb
 *  (1c) If the transfer memory is full, save the RQB in the send queue, @see hif_add_to_rqb_queue_as_first
 * (2) Clean up all transfer memory by freeing all RQBs that have been read by the other HIF instance, @see hif_shm_ring_buffer_send_clean_all_elements,
 *     if the ring buffer became full while sending rqbs and not all rqbs could be transfered
 * (3) Update write index in transfer memory
 * (4) Trigger other HIF instance by calling SEND_IRQ
 * 
 * @param [in] pHifInst - handle to the HIF instance
 * @return
 */
LSA_VOID hif_shm_send(HIF_INST_PTR_TYPE pHifInst)
{
	LSA_BOOL           bNewRqbsSent = LSA_FALSE;
	HIF_RQB_PTR_TYPE   pSndRqb       = LSA_NULL;
	HIF_PIPE_PTR_TYPE  pPipe         = LSA_NULL;
	LSA_UINT16         retVal;
    LSA_UINT16         PipeIndex;

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send(+) -  pHifInst(0x%x)", pHifInst);

    #if defined(HIF_CFG_USE_SND_WAIT_ACK)
	if(pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE) /* Use ack wait */
	{
		if(*pHifInst->if_pars.SharedMem.pSndRQAck != 1)
		{
			return; /* need to wait for Acknowledge */
		}
	}
    #endif

    /* if ring is full, free the ring before sending */
    if(hif_shm_ring_buffer_send_is_full(&pHifInst->if_pars.SharedMem.RQBRingList.SendRing))
    {
        hif_shm_ring_buffer_send_clean_all_elements(pHifInst);
    }

    /* (1) For all RQBs */
    while((!hif_shm_ring_buffer_send_is_full(&pHifInst->if_pars.SharedMem.RQBRingList.SendRing)) && (HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue))) // is there an RQB in the local send queue?
	{
		//HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_send_rqbs_from_send_queue() send queued rqb hH(%d) pipe(%d) comp_id(0x%X)", pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id);

		/* (1a) get the first rqb in the local send queue */
		hif_get_from_rqb_queue(&pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue, &pSndRqb, &PipeIndex);
        
        //Get Pipe        
        pPipe = &pHifInst->Pipes[PipeIndex];

        /* (1b) send single RQB */
		retVal = hif_shm_send_rqb (pPipe, pSndRqb);

		/* (1c) transfer memory full? */
		if(retVal == LSA_RET_OK)
		{
			bNewRqbsSent = LSA_TRUE;
		}
		else /* Send failed, readd the RQB to send queue and don't try to send any further RQBs in this cycle */
		{
            // trace it as WARN (not ERROR) beacause the sending will be added to shared mem send queue
            HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_WARN, "hif_shm_send() failed - out of mem - add to shared mem send queue - rqb(0x%x) hH(%d) pipe(%d) comp_id(0x%X)",pSndRqb, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id);
			
            hif_add_to_rqb_queue_as_first(&pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue, pSndRqb, pPipe->PipeIndex);

			break;
		}
	}
	
	/* send queue not empty? */
	if(HIF_IS_NOT_NULL(pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue))
	{
		/* Mark bRingWasFull at HIF partner and send Interrupt*/
		pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFull = 1;
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_shm_send() send queue (0x%08x) not empty => Set WasFull = 1 and generated Interrupt", pHifInst->if_pars.SharedMem.RQBRingList.pSndQueue);
		if(bNewRqbsSent == LSA_FALSE)
			/* send Interrupt now, cause no RQB was transmitted, cause buffer is full */
			/* else send IRQ later at end of function (probably with readback) */
		{		
			pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
		}
	}

	if(bNewRqbsSent == LSA_TRUE)
	{
        /* (3) Update wi in shared memory */
		*pHifInst->if_pars.SharedMem.RQBRingList.SendRing.pWiShmWriteOnly = pHifInst->if_pars.SharedMem.RQBRingList.SendRing.wi;
        
		#if defined(HIF_CFG_USE_SND_WAIT_ACK)
		if(pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE) /* Use ack wait */
		{
			*pHifInst->if_pars.SharedMem.pSndRQAck = 0; /* reset Ack */
		}
		#endif
		
		HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() PCIWrite pHifInst->if_pars.SharedMem.pSndRQ = 1");
		*pHifInst->if_pars.SharedMem.pSndRQ = 1;
		
		if(pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_NO_WAIT)
		{
			// no wait before irq -> nothing to do
		}
		else if(pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_READ_BACK)
		{
			// read back last pci transfer before sending irq, to prevent that irq comes before data
			volatile LSA_UINT8 tmp;
			tmp = *pHifInst->if_pars.SharedMem.pSndRQ;
			
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() PCIRead pHifInst->if_pars.SharedMem.pSndRQ = %d", tmp);
		}
		else if(pHifInst->if_pars.TmOptions.SendIrqMode == HIF_SEND_IRQ_AFTER_TIMEOUT)
		{
			HIF_WAIT_NS(pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq);
			HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_send() SEND_IRQ after %d ns timeout", pHifInst->if_pars.TmOptions.uNsWaitBeforeSendIrq);
		}
		else
		{
			HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_send() pHifInst->if_pars.TmOptions.SendIrqMode not set correctly!");
			HIF_FATAL(0);
		}
		
		pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
	}
}

/**
 * \brief Reads all RQBs from the transfer memory
 * 
 * For all RQBs in the transfer memory:
 * (1) Get a RQB by getting an offset, @see hif_shm_ring_buffer_receive_dequeue_start. This also updates the read index ri
 * (2) Call Deserialization function, see hif_ser_xxx.c, xxx_deserialize_upper / xxx_deserialize_lower
 * (3) Transfer RQB to the destination, @see hif_process_rcv_rqb
 * (4) Trigger other HIF instance by calling SEND_IRQ
 *
 * @param [in] pHifInst - current HIF instance
 */
LSA_VOID hif_shm_receive(HIF_INST_PTR_TYPE pHifInst)
{
	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	#if (HIF_CFG_COMPILE_SERIALIZATION == 1)
	LSA_UINT64 start_count_deser, end_count_deser;
	#endif
	LSA_UINT64 end_count_total;
	LSA_UINT64 start_count_total = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, ">>> hif_shm_receive() pHifInst (0x%x)", pHifInst);
    
    if(*pHifInst->if_pars.SharedMem.pRcvRQ != 0)
    {
        LSA_VOID_PTR_TYPE        pRcvRqb         = LSA_NULL;
        HIF_PIPE_PTR_TYPE        pPipe           = LSA_NULL;
        #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
        LSA_VOID_PTR_TYPE        pShmSerRqb      = LSA_NULL;
        #endif
        LSA_UINT32               shmSerRqbOffset = 0; 
        LSA_UINT32               is_empty;
        LSA_UINT16               PipeIndex       = 0xFFFF;
        
        *pHifInst->if_pars.SharedMem.pRcvRQ = 0; /* Acknowledge Request on Trigger Var */

        /* (1) Get next RQB in shared mem receive ring */
        
        /* This function provides 3 infos:
         * - is_empty: An information if there are RQBs in the transfer memory
         * - shmSerRqbOffset: A pointer to the next RQB that is deserialized
         * - PipeIndex: A pipe that holds the deserialization functions  
         */
        is_empty = hif_shm_ring_buffer_receive_dequeue_start(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, &shmSerRqbOffset, &PipeIndex);
        while(!is_empty) /* received new RQB */
        {
            pPipe = &pHifInst->Pipes[PipeIndex];

            /* Get pointer to serialized RQB */
            #if (HIF_CFG_COMPILE_SERIALIZATION == 1)
            if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
            {
                LSA_UINT16  ret_val;

                pShmSerRqb = (LSA_VOID*)((LSA_UINT32)shmSerRqbOffset + pHifInst->if_pars.SharedMem.ShmLocal.pRqbBufferBase);

                /* (3) deserialize */
                if(pPipe->func.Deserialize == HIF_FCT_PTR_NULL)
				{
                    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, ">>> hif_shm_receive() Pipe(0x%08x) PipeId(0x%x/0x%x): pPipe->func.Deserialize == HIF_FCT_PTR_NULL", pPipe, PipeIndex, pPipe->pipe_id);
				}
                HIF_ASSERT(HIF_FCT_IS_NOT_NULL(pPipe->func.Deserialize));

                //Measure the deserialization performance
                #if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
                start_count_deser = HIF_GET_NS_TICKS();
                ret_val = pPipe->func.Deserialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pShmSerRqb, &pRcvRqb);
                end_count_deser = HIF_GET_NS_TICKS();
                HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_receive_queue(): Deserialization of the RQB took %d ns",(end_count_deser - start_count_deser));
                #else
                ret_val = pPipe->func.Deserialize(pPipe->pInstance->hSysDev, (LSA_VOID*)pPipe, pShmSerRqb, &pRcvRqb);
                #endif

                HIF_PROGRAM_TRACE_09(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_shm_receive() pRQB(0x%x) hH(%d) pipe(%d) comp_id(0x%X) opc(%d) ring(0x%x) new_ring_index(%d) shm_buff(0x%x) shm_buff_o(0x%x)",
                    pRcvRqb, pPipe->pInstance->hH, pPipe->pipe_id, pPipe->comp_id, HIF_RQB_GET_OPCODE((HIF_RQB_PTR_TYPE)pRcvRqb), pPipe->pRQBRingList->ReceiveRing.pRingReadOnly, pPipe->pRQBRingList->ReceiveRing.ri, pShmSerRqb, shmSerRqbOffset);
                HIF_ASSERT(ret_val == HIF_OK);
            }
            else
            #else
            LSA_UNUSED_ARG(pHifInst);
            #endif
            {
                pRcvRqb = (LSA_VOID*)shmSerRqbOffset;
            }
            /* (3) transport RQB to destination */
            hif_process_rcv_rqb(pPipe, (HIF_RQB_PTR_TYPE)pRcvRqb);
            
            /* (4) get next RQB from the SHM */
            /* This function provides 3 infos:
             * - is_empty: An information if there are RQBs in the transfer memory
             * - shmSerRqbOffset: A pointer to the next RQB that is deserialized
             * - PipeIndex: A pipe that holds the deserialization functions  
             */
            is_empty = hif_shm_ring_buffer_receive_dequeue_start(&pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing, &shmSerRqbOffset, &PipeIndex);

            if(is_empty)
            {
                /* update the receive index on the shared memory */
                /* This might cause the other processor to reuse / free the ring elements which are already read */
                *pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.pRiShmWriteOnly = pHifInst->if_pars.SharedMem.RQBRingList.ReceiveRing.ri;
            }
        }

        #if defined(HIF_CFG_USE_SND_WAIT_ACK)
        if(pHifInst->if_pars.TmOptions.bShmSndWaitAck == LSA_TRUE)
        {
            *pHifInst->if_pars.SharedMem.pRcvRQAck = 1; /* Send optional receive acknowledge */

            pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
        }
        #endif
    }

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_total = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_shm_receive(): The total function duration is %d ns",(end_count_total - start_count_total));
	#else
	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "<<< hif_shm_receive() pHifInst (0x%x)", pHifInst);
	#endif
}

/*------------------------------------------------------------------------------
//	HIF shared memory ring buffer
//----------------------------------------------------------------------------*/

/**
* \brief Cleans the transfer memory. Free all buffers at once to speed up the free process
*
* Elements that have been read by the other HIF instance can be freed here. HIF uses the read index ri and the write index wi to determine these elements.
*
* @param [in] pHifInst - current HIF instance
*/
LSA_VOID hif_shm_ring_buffer_send_clean_all_elements(HIF_INST_PTR_TYPE pHifInst)
{
    HIF_PIPE_SHM_SEND_RING_PTR_TYPE     pSndRing = &pHifInst->if_pars.SharedMem.RQBRingList.SendRing;
    LSA_VOID_PTR_TYPE                   pShmSerRqbWi = LSA_NULL;
    LSA_VOID_PTR_TYPE                   pShmSerRqbRi = LSA_NULL;
    LSA_UINT32                          wi = pSndRing->wi;
    LSA_UINT32                          ri = *pSndRing->pRiShmReadOnly;
    LSA_UINT32                          block_count = 0;
    LSA_UINT8                           WiFlag = 0;

    if(pHifInst->if_pars.TmOptions.bSerialization == LSA_TRUE)
    {
        if(ri == 0)
        {
            pShmSerRqbRi = (LSA_VOID*)(pSndRing->Ring[HIF_SHM_RB_BUFFER_LENGTH - 1]);
        }
        else
        {
            pShmSerRqbRi = (LSA_VOID*)(pSndRing->Ring[ri - 1]);
        }

        do
        {
            /*
            Free memory that the other HIF instance has already read.
            Memory structure of the transfer memory (sample):
            Upper |  Shared memory | Lower
            RQB_1
            RQB_2		<- Ring[ri]
            RQB_3
            RQB_4
            Ring[wi] ->	oldRQB_A
            oldRQB_B
            oldRQB_C
            oldRQB_A, oldRQB_B, oldRQB_C and RQB_1 can be freed. RQB_2, RQB_3 and RQB_4 are still pending.
            The local variable wi iterates down, wraps around to the value 0 after oldRQB_C and interates down again until ri is reached.
            The new RQB_5 will be written after RQB_4 (on the start position of oldRQB_A).
            */
            if(pSndRing->bRingElementUsed[wi] == LSA_TRUE)
            {
                if(WiFlag == 0)
                {
                    // Wi is the first used entry
                    pShmSerRqbWi = (LSA_VOID*)(pSndRing->Ring[wi]);
                    WiFlag = 1;
                }

                /* mark the element in the ring-buffer as free */
                pSndRing->Ring[wi] = 0;
                pSndRing->bRingElementUsed[wi] = LSA_FALSE;
                block_count++;
            }
            /* Wrap around-> Reset wi to 0*/
            wi = (wi + 1) % HIF_SHM_RB_BUFFER_LENGTH;
        } while(wi != ri); /*loop until the ri is reached*/

        /* free all unused RQBs in the shared memory at once */
        hif_mem_ring_pool_free(pShmSerRqbWi, pShmSerRqbRi, pHifInst->if_pars.SharedMem.ShmRemote.hPool, block_count);
    }
    else
    {
        /* Mark all elements in the ring as unused. No memory must be freed since not memory was allocated. */
        do
        {
            if(pSndRing->bRingElementUsed[wi] == LSA_TRUE)
            {
                /* mark the element in the ring-buffer as free */
                pSndRing->Ring[wi] = 0;
                pSndRing->bRingElementUsed[wi] = LSA_FALSE;
            }
            wi = (wi + 1) % HIF_SHM_RB_BUFFER_LENGTH;
        } while(wi != ri);
    }
}

/**
 * \brief Checks if pSndRingis full
 * 
 * @param [in] pSndRing - current send ring
 * @return LSA_TRUE     - the send buffer is full
 * @return LSA_FALSE    - the send buffer is not full
 */
LSA_BOOL hif_shm_ring_buffer_send_is_full(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing)
{
	/* This function checks if a send ring is full */
	/* Send ring is full when the write index would reach the read index */

	LSA_UINT32 ret;

	ret = (((pSndRing->wi + 1) % HIF_SHM_RB_BUFFER_LENGTH) == *pSndRing->pRiShmReadOnly);

	if(ret)
	{
	    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_is_full(+) -  pSndRing(0x%x) is full, wi index still in use by remote", pSndRing);
		return LSA_TRUE;
	}
	else
	{
        if(pSndRing->bRingElementUsed[pSndRing->wi])
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_is_full(+) -  pSndRing(0x%x) is full, all local entries used", pSndRing);
            return LSA_TRUE;
        }
        else
        {
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_is_full(-) -  pSndRing(0x%x) is not full", pSndRing);
            return LSA_FALSE;
        }
	}
}

/**
 * \brief Enqueues an serialized RQB into the ring buffer of the shared memory. 
 * 
 * @param [in] pSndRing     - current send ring
 * @param [in] uRqbOffset   - offset of the RQB in the transfer memory
 * @param [in] uRqb         - original RQB pointer
 * @param [in] PipeIndex    - PipeIndex for the RQB
 *
 * @return LSA_TRUE    - the send pipe is full
 * @return LSA_FALSE   - there is still place for more RQBs in the queue
 */
LSA_BOOL hif_shm_ring_buffer_send_enqueue(HIF_PIPE_SHM_SEND_RING_PTR_TYPE pSndRing, LSA_UINT32 uRqbOffset, LSA_UINT32 uRqb, LSA_UINT16 PipeIndex)
{
	/* This function enqueues a new Rqb to the send Ring */
	/* Additionally a local copy is stored which is used for cleanup */
	/* because send ring might be read only */

	LSA_BOOL is_full;

    HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_enqueue(+) -  pSndRing(0x%x), uRqbOffset(0x%x), uRqb(0x%x), PipeIndex(0x%x)", pSndRing, uRqbOffset, uRqb, PipeIndex);

	is_full = hif_shm_ring_buffer_send_is_full(pSndRing);

	if(!is_full)
	{
		pSndRing->pRingWriteOnly[pSndRing->wi] = uRqbOffset;
        pSndRing->pPipeIndexWriteOnly[pSndRing->wi] = (LSA_UINT32)PipeIndex; 
        pSndRing->Ring[pSndRing->wi] = uRqb;
		pSndRing->bRingElementUsed[pSndRing->wi] = LSA_TRUE;

		/* trace the info for the current RQB. This is useful for error analysis */
		HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_NOTE, "hif_shm_ring_buffer_send_enqueue() - ### Enque HIF Mem PipeIndex:%d ring:0x%x, ring_index:%d, uRqb:0x%x, shm_buff_o:0x%x",
		                     PipeIndex, pSndRing->pRingWriteOnly, pSndRing->wi, pSndRing->Ring[pSndRing->wi], uRqbOffset);
		
		/* increment the value AFTER the trace entry to get useful infos in the trace */
		pSndRing->wi = ((pSndRing->wi + 1) % HIF_SHM_RB_BUFFER_LENGTH);
		/* *pSndRing->pWiShmWriteOnly = pSndRing->wi; => moved to for optimisation */     
	}
	else
	{
	    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_send_enqueue(-) pSndRing(0x%x) Ring is full", pSndRing); 
	}
	return is_full;
}

/**
 * @brief Dumps the complete receive ring by dumping 10 elements at once. Then dumps the last elements
 * 
 * This function should only be called before HIF_FATAL is called. All traces are on level FATAL. Do not use this function during normal operation since tracing costs time and spams the trace buffer.
 * 
 * @param [in] pRcvRing - pointer to the receive ring
 */
static LSA_VOID hif_shm_trace_receive_ring(HIF_PIPE_SHM_RECEIVE_RING_CONST_PTR_TYPE pRcvRing)
{
    LSA_UINT16 idx;
	LSA_UINT16 max_value = (HIF_SHM_RB_BUFFER_LENGTH / 10) * 10; // nearest number that is diviable by 10, e.g. HIF_SHM_RB_BUFFER_LENGTH = 93 -> max_value = 90

    /* Dump pRcvRing->pRingReadOnly (all elements but the last */
    for(idx = 0; idx < max_value; idx = idx + 10)
    {
        HIF_PROGRAM_TRACE_12(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_trace_receive_ring() DUMP: pRcvRing->pRingReadOnly[%d...%d] = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x ", 
                             idx, idx+10,
                             pRcvRing->pRingReadOnly[idx], pRcvRing->pRingReadOnly[idx+1], pRcvRing->pRingReadOnly[idx+2],pRcvRing->pRingReadOnly[idx+3], pRcvRing->pRingReadOnly[idx+4],
                             pRcvRing->pRingReadOnly[idx+5], pRcvRing->pRingReadOnly[idx+6], pRcvRing->pRingReadOnly[idx+7],pRcvRing->pRingReadOnly[idx+8], pRcvRing->pRingReadOnly[idx+9]); 
    }
    
    /* dump the last values of the array (this is the case if HIF_SHM_RB_BUFFER_LENGTH % 10 != 0). Trace each entry with one trace call */
    for(/* idx already initialized by last loop */; idx < HIF_SHM_RB_BUFFER_LENGTH; idx++)
    {
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_trace_receive_ring() DUMP: pRcvRing->pRingReadOnly[%d] = 0x%x", 
                             idx, pRcvRing->pRingReadOnly[idx]);
    }
}

/**
* @brief Dumps the complete receive ring of pipeides by dumping 10 elements at once. Then dumps the last elements
*
* This function should only be called before HIF_FATAL is called. All traces are on level FATAL. Do not use this function during normal operation since tracing costs time and spams the trace buffer.
*
* @param [in] pRcvRing - pointer to the receive ring
*/
static LSA_VOID hif_shm_trace_receive_pipe_ring(HIF_PIPE_SHM_RECEIVE_RING_CONST_PTR_TYPE pRcvRing)
{
    LSA_UINT16 idx;
    LSA_UINT16 max_value = (HIF_SHM_RB_BUFFER_LENGTH / 10) * 10; // nearest number that is diviable by 10, e.g. HIF_SHM_RB_BUFFER_LENGTH = 93 -> max_value = 90

    /* Dump pRcvRing->pPipeIndexReadOnly (all elements but the last */
    for (idx = 0; idx < max_value; idx = idx + 10)
    {
        HIF_PROGRAM_TRACE_12(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_trace_receive_pipe_ring() DUMP: pRcvRing->pPipeIndexReadOnly[%d...%d] = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x ",
            idx, idx + 10,
            pRcvRing->pPipeIndexReadOnly[idx], pRcvRing->pPipeIndexReadOnly[idx + 1], pRcvRing->pPipeIndexReadOnly[idx + 2], pRcvRing->pPipeIndexReadOnly[idx + 3], pRcvRing->pPipeIndexReadOnly[idx + 4],
            pRcvRing->pPipeIndexReadOnly[idx + 5], pRcvRing->pPipeIndexReadOnly[idx + 6], pRcvRing->pPipeIndexReadOnly[idx + 7], pRcvRing->pPipeIndexReadOnly[idx + 8], pRcvRing->pPipeIndexReadOnly[idx + 9]);
    }

    /* dump the last values of the array (this is the case if HIF_SHM_RB_BUFFER_LENGTH % 10 != 0). Trace each entry with one trace call */
    for (/* idx already initialized by last loop */; idx < HIF_SHM_RB_BUFFER_LENGTH; idx++)
    {
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_trace_receive_pipe_ring() DUMP: pRcvRing->pPipeIndexReadOnly[%d] = 0x%x",
            idx, pRcvRing->pPipeIndexReadOnly[idx]);
    }
}


/**
 * \brief Dequeues a RQB from the receive ring and returns the offset of this RQB back to the caller.
 * 
 * @param [in]  pRcvRing     - current receive ring
 * @param [out] puRqb        - offset of the RQB in the shared memory. This parameter is only valid if the ring is not empty.
 * @param [out] pPipeIndex   - PipeIndex (used for deserialization). This parameter is only valid if the ring is not empty.
 *
 * @return LSA_TRUE          - ring is empty
 * @return LSA_FALSE         - ring is not empty, RQB offset returned in puRqb       
 */
LSA_BOOL hif_shm_ring_buffer_receive_dequeue_start(HIF_PIPE_SHM_RECEIVE_RING_PTR_TYPE pRcvRing, LSA_UINT32 *puRqb, LSA_UINT16 *pPipeIndex)
{
	/* This function dequeues a new RQB from the receive ring */
	/* The local receive index is increased, the shm receive isn't because the rqbs need to be processed first */  

	LSA_BOOL is_empty = LSA_FALSE;

	/* (1) Check if RcvRing is empty */
	if(pRcvRing->ri == *pRcvRing->pWiShmReadOnly)
	{
		is_empty = LSA_TRUE;
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_receive_dequeue_start(-) pRcvRing(0x%x) queue is empty", pRcvRing);
	}

	if(!is_empty)
	{
	    /* (2) Check if ri is valid */
		if(pRcvRing->ri >= HIF_SHM_RB_BUFFER_LENGTH)
		{
		    /* error handling: Pipeindex cannot be obtained since the read index is invalid */  
		    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_ring_buffer_receive_dequeue_start() FATAL: ri invalid. pRcvRing(0x%x), pRcvRing->ri(%d), *pRcvRing->pWiShmReadOnly(%d) ", 
		                         pRcvRing, pRcvRing->ri, *pRcvRing->pWiShmReadOnly);
		    
		    /* dump complete receive ring */
		    hif_shm_trace_receive_ring(pRcvRing);
		    
		    /*  internal error in HIF: The transfer memory is corrupt, HIF is unable to dequeue the RQB from the queue since the read index is corrupt. 
		        Please check the traces from the other HIF instance, 
		        Hint: Check for last trace entry in hif_shm_ring_buffer_send_enqueue 
		    */
		    HIF_FATAL(0);
		}
		else
		{
		    *pPipeIndex = (LSA_UINT16) pRcvRing->pPipeIndexReadOnly[pRcvRing->ri];
		    /* (3) Check if PipeIndex is valid */
		    if(*pPipeIndex >= HIF_MAX_PIPES_INST)
		    {	        
		        HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "hif_shm_ring_buffer_receive_dequeue_start() FATAL: PipeIndex is invalid. pRcvRing(0x%x), *pPipeIndex (%d), pRcvRing->ri(%d), *pRcvRing->pWiShmReadOnly(%d) ", 
		                             pRcvRing, *pPipeIndex, pRcvRing->ri, *pRcvRing->pWiShmReadOnly);
		        
		        /* dump complete receive ring */
                hif_shm_trace_receive_pipe_ring(pRcvRing);
		        
		        /*  internal error in HIF: The PipeIndex is invalid */
		        HIF_FATAL(0);
		    }
		    else
		    {
		        /* (4) Get the offset of the current RQB and return it */
		        *puRqb = pRcvRing->pRingReadOnly[pRcvRing->ri];
		    }
		}

        HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_shm_ring_buffer_receive_dequeue_start(+) -  pRcvRing(0x%x), *puRqb(0x%x), *pPipeIndex(0x%x)", pRcvRing, *puRqb, *pPipeIndex);

        /* (5) Increment ri value */
		pRcvRing->ri = (pRcvRing->ri + 1) % HIF_SHM_RB_BUFFER_LENGTH;
	}
	return is_empty;
}

#endif // (HIF_CFG_USE_HIF == 1)

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/

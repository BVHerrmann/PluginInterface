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
/*  F i l e               &F: hif_usr.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements the user LSA interface                                        */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID	1
#define HIF_MODULE_ID		LTRC_ACT_MODUL_ID

#include "hif_int.h"

/// @cond PREVENT_DOXYGEN
HIF_FILE_SYSTEM_EXTENSION(HIF_MODULE_ID)
/// @endcond

#if (HIF_CFG_USE_HIF == 1)

/**
 * @brief Returns the RQB to the originator
 * 
 * (1) sets the response
 * (2) Restore orignal handle
 * (3) Call REQUEST_DONE
 * 
 * Note: Only call hif_callback once for each RQB
 * 
 * @param [in] pChHandle        - HIF channel handle
 * @param [in] pRQB             - The RQB that is returned to the originator
 * @param [in] response         - This response is set in the RQB
 */
LSA_VOID hif_callback(HIF_CH_HANDLE_PTR_TYPE pChHandle, HIF_RQB_PTR_TYPE pRQB, LSA_UINT16 response)
{
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_callback(+) - pChHandle(0x%x), response(0x%x)", pChHandle, response);

	HIF_ASSERT(HIF_IS_NOT_NULL(pRQB));
	HIF_ASSERT(HIF_IS_NOT_NULL(pChHandle));
	HIF_ASSERT(HIF_IS_NOT_NULL(pChHandle->pPipe));

	/* (1) set response code */
	HIF_RQB_SET_RESPONSE(pRQB, response);

	/* (2) set handle */
	HIF_RQB_SET_HANDLE(pRQB, pChHandle->pPipe->d.upper.hUpper);

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_callback pRQB(0x%x) response(0x%x)",pRQB,response);

	/* (3) call output macro */
	pChHandle->pPipe->pInstance->func.REQUEST_DONE((HIF_UPPER_CALLBACK_FCT_PTR_TYPE)pChHandle->pCbf, pRQB, pChHandle->sys_ptr);
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_callback");
}

/**
 * @brief Open channel to remote modul
 * 
 * @param [in] rqb RQB with channel information
 * @param [in] bLDRequest LSA_TRUE: Open channel to LD instance; LSA_FALSE: Open channel to HS instance
 * @param [in] bURequest LSA_TRUE: Open channel to upper instance; LSA_FALSE: Open channel to lower instance
 * @return
 */
LSA_VOID hif_open_channel(HIF_RQB_PTR_TYPE rqb, LSA_BOOL bLDRequest, LSA_BOOL bURequest)
{
	LSA_UINT16             retVal;
	LSA_SYS_PATH_TYPE      path;
	LSA_HANDLE_TYPE        hUpper;
	HIF_INST_PTR_TYPE      pHifInst;
	HIF_CH_HANDLE_PTR_TYPE pChHandle;
	HIF_CH_HANDLE_TYPE     chHandle;
	LSA_COMP_ID_TYPE       comp_id;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_open_channel(+) - rqb(0x%x) bLDRequest(0x%x)", rqb, bLDRequest);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - rqb (0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) - check args", rqb);

	comp_id = HIF_RQB_GET_COMP_ID(rqb);

	/* get Cbf + Path of RQB (component specific) */

	if(g_pHifData->CompStore.CompStd[comp_id].bUsed != LSA_TRUE)
	{
        HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - CompStd[%d].bUsed (0x%x) is not used", comp_id, g_pHifData->CompStore.CompStd[comp_id].bUsed);
		HIF_FATAL(0);
	}

	retVal = g_pHifData->CompStore.CompStd[comp_id].Comp.GetOpenChannelCompInfo(rqb, (HIF_UPPER_CALLBACK_FCT_PTR_TYPE*)&(chHandle.pCbf), &path, &hUpper);

	if(retVal != LSA_RET_OK)
	{
        HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - GetOpenChannelCompInfo(...) was not successful");
		HIF_FATAL(0); /* HIF Could not get Cbf and Path out of open channel RQB */
	}

	HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_HIGH, "hif_open_channel - rqb(0x%x) bLDRequest(0x%x) Path(0x%x)", rqb, bLDRequest, path);

	if(bLDRequest)
	{
		/* get Detail Ptr which identifies hif instance + pipe */
		if(bURequest)
		{
			HIF_LD_U_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &(chHandle.pDetails), path);
		}
		else
		{
			HIF_LD_L_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &(chHandle.pDetails), path);
		}
		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - HIF_LD_L_GET_PATH_INFO(...) was not successful");
			HIF_FATAL(0); /* HIF Could not get the details for the target path */
		}

		HIF_LD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel_ld rqb(0x%x)", rqb);
	}
	else
	{
		/* get Detail Ptr which identifies hif instance + pipe */
		if(bURequest)
		{
			HIF_HD_U_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &(chHandle.pDetails), path);
		}
		else
		{
			HIF_HD_L_GET_PATH_INFO(&retVal, &(chHandle.sys_ptr), &(chHandle.pDetails), path);
		}
		if(retVal != LSA_RET_OK)
		{
            HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - HIF_HD_L_GET_PATH_INFO(...) was not successful");
			HIF_FATAL(0); /* HIF Could not get the details for the target path */
		}

		HIF_HD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel_hd rqb(0x%x)", rqb);
	}

	/* get Hif Inst */
	hif_get_instance(&pHifInst, chHandle.pDetails->hH);

	if(HIF_IS_NULL(pHifInst))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - pHifInst(0x%x) is not valid", pHifInst);
		HIF_FATAL(0); /* hH from Details wrong */
	}

	/* check if HIF Inst OK for Open Channel */
	if(pHifInst->State != HIF_INST_STATE_OPEN)
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - pHifInst->State(0x%x) is not valid", pHifInst->State);
		HIF_FATAL(0); /* HIF Instance in wrong state */
	}

	/* get Pipe */
	hif_get_pipe(&chHandle.pPipe, pHifInst, chHandle.pDetails->hd_id, chHandle.pDetails->pipe_id);
	if(HIF_IS_NULL(chHandle.pPipe))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_open_channel(-) - chHandle.pPipe(0x%x) is not valid", chHandle.pPipe);
		HIF_FATAL(0); /* Detail specified pipe not found */
	}

	HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) inst(%d) pipe(%d) hd_id(%d) pipe_id(%d) - forward", rqb, pHifInst->hH, chHandle.pPipe->PipeIndex, chHandle.pPipe->hd_id, chHandle.pPipe->pipe_id);

	if(chHandle.pPipe->State != HIF_PIPE_STATE_CLOSED)
	{
		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_open_channel.req (upper) (-) qrb(0x%x) - invalid pipe state (%d)", rqb, chHandle.pPipe->State);
		hif_callback(&chHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));

		if(bLDRequest)
		{
			HIF_LD_EXIT();
		}
		else
		{
			HIF_HD_EXIT();
		}
		return;		
	}

	/* alloc new ch handle */
	hif_alloc_ch_handle(chHandle.pPipe, &pChHandle);
	if(HIF_IS_NULL(pChHandle))
	{
		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_UNEXP, "hif_open_channel.req (upper) (-) rqb(0x%x) - cannot get channel handle", rqb);
		hif_callback(&chHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_RESOURCE));

		if(bLDRequest)
		{
			HIF_LD_EXIT();
		}
		else
		{
			HIF_HD_EXIT();
		}
		return;
	}

	//Copy data to real channel handle structure
	pChHandle->sys_ptr  = chHandle.sys_ptr;
	pChHandle->pDetails = chHandle.pDetails;
	pChHandle->pPipe    = chHandle.pPipe;
	pChHandle->pCbf     = chHandle.pCbf;

	pChHandle->pPipe->d.upper.hUpper    = hUpper;
	pChHandle->pPipe->d.upper.ch_handle = pChHandle;
	pChHandle->bValid                   = LSA_TRUE;

	/* adjust pipe state */
	hif_set_pipe_state(pChHandle->pPipe, HIF_PIPE_STATE_OPENING_UPPER);

	/* save device type specifics */
	if(bLDRequest)
	{
		if(bURequest)
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_LD_U_RELEASE_PATH_INFO;
		}
		else
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_LD_L_RELEASE_PATH_INFO;
		}
	}
	else
	{
		if(bURequest)
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_HD_U_RELEASE_PATH_INFO;
		}
		else
		{
			pChHandle->pPipe->func.RELEASE_PATH_INFO = HIF_HD_L_RELEASE_PATH_INFO;
		}
	}

	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_open_channel.req (upper) rqb(0x%x) chH(%d) - pipe_state=opening_upper - forward open to hif lower", rqb, pChHandle->hHifCompCh);

	/* open_channel ok for this hif instance */

	/* forward request to lower (upper) hif layer */
	hif_send(pChHandle->pPipe, rqb);

	if(bLDRequest)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}
}

/* description: see header */
LSA_VOID hif_ld_l_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_l_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_TRUE, LSA_FALSE);
}

/* description: see header */
LSA_VOID hif_ld_u_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_u_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_TRUE, LSA_TRUE);
}

/* description: see header */
LSA_VOID hif_hd_l_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_l_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_FALSE, LSA_FALSE);
}

/* description: see header */
LSA_VOID hif_hd_u_open_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_u_open_channel(+) - rqb(0x%x)", rqb);

	hif_open_channel(rqb, LSA_FALSE, LSA_TRUE);
}

/**
 * @brief Close channel to remote instance 
 * 
 * @param [in] rqb RQB with channel information
  * @param [in] bLDRequest LSA_TRUE: Close channel to LD instance; LSA_FALSE: Close channel to HD instance
 */
LSA_VOID hif_close_channel(HIF_RQB_PTR_TYPE const rqb, LSA_BOOL const bLDRequest)
{
	LSA_COMP_ID_TYPE       comp_id;
	HIF_CH_HANDLE_PTR_TYPE pChHandle;

    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_close_channel(+) - rqb(0x%x) bLDRequest(0x%x)", rqb, bLDRequest);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_close_channel(-) - rqb(0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	if(bLDRequest)
	{
		HIF_LD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_channel_ld rqb(0x%x)", rqb);
	}
	else
	{
		HIF_HD_ENTER();

		HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_close_channel_hd rqb(0x%x)", rqb);
	}

	comp_id = HIF_RQB_GET_COMP_ID(rqb);

    switch(comp_id)
    {
	    #if (HIF_CFG_USE_EDD == 1)
        case LSA_COMP_ID_EDD:
        case LSA_COMP_ID_EDDI:
        case LSA_COMP_ID_EDDP:
        case LSA_COMP_ID_EDDS:
        case LSA_COMP_ID_EDDT:
        {
	        EDD_RQB_TYPE        * const pEddRQB           = (EDD_RQB_TYPE*)HIF_CAST_TO_VOID_PTR(rqb);
            EDD_HANDLE_LOWER_TYPE const EddHandleLower    = HIF_EDD_RQB_GET_HANDLE_LOWER(pEddRQB);
            LSA_UINT32            const ChHandlesIndexU32 = (LSA_UINT32)EddHandleLower;

            HIF_ASSERT(ChHandlesIndexU32 < HIF_MAX_CH_HANDLES);
	        pChHandle = &(g_pHifData->ChHandles[ChHandlesIndexU32]);
            break;
        }
        #endif
        default:
        {
            HIF_ASSERT(comp_id < HIF_MAX_COMP_ID);
	        pChHandle = &(g_pHifData->ChHandles[HIF_RQB_GET_HANDLE(rqb)]);
        }
    }

	if(pChHandle->bUsed == LSA_TRUE)
	{
		if(pChHandle->bValid == LSA_TRUE)
		{
			if(pChHandle->pPipe->State == HIF_PIPE_STATE_OPENED_UPPER)
			{
				HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "hif_close_channel.req (upper) rqb(0x%x) hH(%d) pipe(%d) chH(%d) - forward close to hif lower", rqb, pChHandle->pPipe->pInstance->hH, pChHandle->pPipe->PipeIndex, pChHandle->hHifCompCh);

				/* request from upper => send to lower */
				hif_send(pChHandle->pPipe, rqb);
			}
			else
			{
				HIF_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_UNEXP, "hif_close_channel.req (upper) (-) rqb(0x%x) hH(%d) pipe(%d) chH(%d) - invalid pipe state (%d)", rqb, pChHandle->pPipe->pInstance->hH, pChHandle->pPipe->PipeIndex, pChHandle->hHifCompCh, pChHandle->pPipe->State);

				hif_callback(pChHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));
			}
		}
		else /* invalid channel handle!!! */
		{
			HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_close_channel (upper) (-) rqb(0x%x) chH(%d) channel handle not valid!", rqb, pChHandle->hHifCompCh);
			HIF_FATAL(0);
		}
	}
	else /* invalid channel handle!!! */
	{
		HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "hif_close_channel (upper) (-) rqb(0x%x) chH(%d) invalid channel handle", rqb, pChHandle->hHifCompCh);
		HIF_FATAL(0);
	}

	if(bLDRequest)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}
}

/* description: see header */
LSA_VOID hif_ld_l_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_get_open_channel_lower_handle_cm(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_TRUE);
}

/* description: see header */
LSA_VOID hif_ld_u_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_ld_u_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_TRUE);
}

/* description: see header */
LSA_VOID hif_hd_l_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_l_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_FALSE);
}

/* description: see header */
LSA_VOID hif_hd_u_close_channel(HIF_RQB_PTR_TYPE rqb)
{
    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_hd_u_close_channel(+) - rqb(0x%x)", rqb);

	hif_close_channel(rqb, LSA_FALSE);
}

/**
 * @brief Central trigger function to process RQBs within HIF.
 * 
 * This function is called by hif_ld_l_request, hif_ld_u_request, hif_hd_l_request, hif_hd_u_request.
 * There are two types of RQBs:
 * 
 * (A) Internal RQBs with the OPC HIF_OPC_POLL, HIF_OPC_TIMEOUT_POLL, HIF_OPC_ISR, HIF_OPC_RECEIVE
 * (B) External RQBs from a LSA component
 *
 * UseCase A: 
 *   HIF was triggerd by an internal event (timer, interrupt, hif_poll-call). 
 *   (1) The internal trigger RQBs are set to valid pointers, that way the timer or interrupt handler may trigger HIF again.
 *   (2) HIF uses this trigger to receive and send RQBs, @see hif_shm_receive, @see hif_shm_send. For interrupt mode, interrupts must be acknowledged.
 *   (3) Memory from the Admin Free list can be freed here, @see hif_buffer_admin_free.
 *   (4) For HIF_OPC_POLL, HIF_DO_LOW_REQUEST is called. That way the lower HIF task will get an additional trigger RQB that will trigger HIF_OPC_POLL again.
 *
 * UseCase B:
 *   The external RQB was sent to HIF and HIF must send the RQB to the other HIF instance. @see hif_send.
 *
 * @param [in] rqb          - RQB that is processed by HIF
 * @param [in] pPollTimer   - corresponding Polltimer of this instance
 * @param [in] bLDRequest   - LSA_TRUE: LD instance, LSA_FALSE: HD instance
 */
LSA_VOID hif_request(HIF_RQB_PTR_TYPE rqb, HIF_POLL_TIMER_TYPE * pPollTimer, LSA_BOOL bLDRequest)
{
	/** \brief Remember that my Shared Memory was marked as "full" by my HIF partner */
	static LSA_UINT8       bSHMwasFull = 0;

	HIF_INST_PTR_TYPE      pHifInst;
	HIF_CH_HANDLE_PTR_TYPE pChHandle;
	LSA_UINT16             i;
	LSA_UINT16             uInstStartIndex;
	LSA_UINT16             uInstEndIndex;
    HIF_RQB_PTR_TYPE       pRQBPoll;
	LSA_COMP_ID_TYPE       comp_id;

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64             end_count_lower_req;
	LSA_UINT64             start_count_lower_req = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_CHAT, "hif_request(+) - rqb(0x%x) pPollTimer(0x%x) bLDRequest(0x%x)", rqb, pPollTimer, bLDRequest);

	HIF_ASSERT(HIF_IS_NOT_NULL(g_pHifData));

	if(HIF_IS_NULL(rqb))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_request(-) - rqb(0x%x) is not valid", rqb);
		HIF_FATAL(0);
	}

	if(bLDRequest)
	{
		HIF_LD_ENTER();
		uInstStartIndex = 0;
		uInstEndIndex   = HIF_CFG_MAX_LD_INSTANCES;
	}
	else
	{
		HIF_HD_ENTER();
		uInstStartIndex = HIF_CFG_MAX_LD_INSTANCES;
		uInstEndIndex   = HIF_CFG_MAX_LD_INSTANCES + HIF_CFG_MAX_HD_INSTANCES;
	}

	comp_id = HIF_RQB_GET_COMP_ID(rqb);

	/* internal request ? - UseCase A */
    if(comp_id == LSA_COMP_ID_HIF)
	{
		switch(HIF_RQB_GET_OPCODE(rqb))
		{
			case HIF_OPC_POLL:
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.poll) rqb(0x%x) HIF_OPC_POLL ld(%d)", rqb, bLDRequest);

				/* (1) return resource */
				if(bLDRequest)
				{
					g_pHifData->LD.pRQBPoll = rqb;
				}
				else
				{
					g_pHifData->HD.pRQBPoll = rqb;
				}

				/* (2) check all relevant instances for received messages */
				for(i=uInstStartIndex; i<uInstEndIndex; i++)
				{
					pHifInst = &(g_pHifData->Instances[i]);

					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						if(pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE)
						{
							hif_shm_receive(pHifInst);
							hif_shm_send(pHifInst);
							if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
							{
								hif_upper_device_close_finish_2(pHifInst);
							}
						}
					}

#if (HIF_CFG_USE_DMA == 1)
					/* (3) free all transfered buffers (only in lower instance) */
					/* a call in upper instance will cause race condition in pciox advanced 3 division mode */
					if(pHifInst->bLowerDevice == LSA_TRUE)
					{
	                    hif_buffer_admin_free();					    
					}
#endif
				}
                
                /* (4) Send a trigger RQB to the low context: 
                    - If the system is in low load, HIF_OPC_POLL HIF_DO_LOW_REQUEST. The system adaptation calls hif_internal_poll, this function will send another trigger RQB with HIF_OPC_POLL. 
                    That way, both the normal prio thread and the low prio thread trigger each other by internal RQBs. This will speed up the system since HIF_OPC_POLL is triggered more often.

                    - If the system is in high load, the low prio thread is not scheduled. HIF_OPC_POLL is not called and HIF_DO_LOW_REQUEST is not called. 
                    The LSA-Timer will still call HIF_OPC_TIMEOUT_POLL, that way the system does not starve  
                */

                /* Fire only once at a time */
                if((g_pHifData->internal.bUseInternalPoll) && (g_pHifData->internal.pInternalPoll != LSA_NULL))
                {
                    /* Get RQB from internal structure, delete structure pointer. The RQB now belongs to the system adaptation*/
                    pRQBPoll = g_pHifData->internal.pInternalPoll;
                    g_pHifData->internal.pInternalPoll = LSA_NULL;

                    /* Sent poll RQB to the low prio context*/
                    HIF_DO_LOW_REQUEST(pRQBPoll);
                }
				break;
			case HIF_OPC_TIMEOUT_POLL:
				HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, "hif_request (internal.poll) rqb(0x%x) HIF_OPC_TIMEOUT_POLL ld(%d)", rqb, bLDRequest);

				/* (1) return resource */
				pPollTimer->pRQBPoll = rqb;

				/* (2) check all relevant instances for received messages */
				for(i=uInstStartIndex; i<uInstEndIndex; i++)
				{
					pHifInst = &(g_pHifData->Instances[i]);

					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						if(pHifInst->if_pars.TmOptions.bShmHandling == LSA_TRUE)
						{
							hif_shm_receive(pHifInst);
							hif_shm_send(pHifInst);
							if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
							{
								hif_upper_device_close_finish_2(pHifInst);
							}
						}
					}

#if (HIF_CFG_USE_DMA == 1)
                    /* (3) free all transfered buffers (only in lower instance) */
					/* a call in upper instance will cause race condition in pciox advanced 3 division mode */
                    if(pHifInst->bLowerDevice == LSA_TRUE)
                    {
                        hif_buffer_admin_free();                        
                    }
#endif
				}
				break;
			case HIF_OPC_ISR:
				HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.isr) rqb(0x%x) hH(%d) HIF_OPC_ISR ld(%d)", rqb, rqb->args.internal_req.receive.hHDest, bLDRequest);

				hif_get_instance(&pHifInst, rqb->args.internal_req.receive.hHDest);

				if(HIF_IS_NOT_NULL(pHifInst))
				{
					/* (1) return trigger resource */
					pHifInst->pRQBIsr = rqb;

					/* (2a) To reset the HIF RingWasFullAck at HIF partner, my bRingWasFull must be set 0 by HIF partner, but my bSHMwasFull must bigger than 0 */
					if(bSHMwasFull >= 1 &&  pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader->tm_control.bRingWasFull == 0)
						/* My SHM was full (last time, this function was called bSHMwasFull>0) and was reseted by HIF Partner */
					{
						/* Reset the bRingWasFullAck at HIF partner and reset my local marker bSHMwasFull*/
						pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFullAck = 0;
						HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.isr) - bShmFull Ack reset at partner");
						bSHMwasFull = 0;
					}
					
					/* (2b) To reset the HIF RingWasFull at HIF partner, my bRingWasFullAck must be set 1 by HIF partner */
					if(pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader->tm_control.bRingWasFullAck == 1)
						/*The bRingWasFull at my HIF partner was noticed, SHM was cleared */
					{
						/* Reset the bRingWasFull at HIF partner */
						pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFull = 0;
						HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.isr) - bShmFull reset at partner");
						/* Send next RQBs, cause queue is not completly full now */
						hif_shm_send(pHifInst);
					}

                    /* (2c) Ack pending interrupts */
					if(pHifInst->bRcvEnabled == LSA_TRUE)
					{
						hif_shm_receive(pHifInst);
						
						/* Acknowledge Interrupt after hif transfers -> hif_interrupt won't be executed until all hif recieve, send actions finished! */
						pHifInst->func.ACK_IRQ(pHifInst->hSysDev, pHifInst->hH); 

						if(pHifInst->pRQBUpperDeviceCloseFinishRsp != LSA_NULL)
						{
							hif_upper_device_close_finish_2(pHifInst);
						}
					}
					else
					{
						/* Acknowledge Interrupt */
						pHifInst->func.ACK_IRQ(pHifInst->hSysDev, pHifInst->hH); 
					}
					
					if(pHifInst->if_pars.SharedMem.ShmLocal.pShmHeader->tm_control.bRingWasFull == 1)
						/* ack my SHM full flag and send interrupt to partner */
					{
						/* It is theoretical possible, that my SHM was completely filled between the last hif_shm_receive and here!
						 * So this receiv is necessary to check the memory again, although the chance to find a new RQB is very small. 
						 */
						hif_shm_receive(pHifInst);
						
						/* Acknowledge at the HIF partner, that I have cleared the my receivelist and new RQBs may be send */
						pHifInst->if_pars.SharedMem.ShmRemote.pShmHeader->tm_control.bRingWasFullAck = 1;
						HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.isr) - set bShmFullAck at partner");						

						/* wake up the HIF partner, to send new RQBs */
						pHifInst->func.SEND_IRQ(pHifInst->hSysDev, pHifInst->hH);
					}
					
#if (HIF_CFG_USE_DMA == 1)
					/* (3) free all transfered buffers (only in lower instance) */
					/* a call in upper instance will cause race condition in pciox advanced 3 division mode */
                    if(pHifInst->bLowerDevice == LSA_TRUE)
                    {
                        hif_buffer_admin_free();                        
                    }
#endif
				}
				else
				{
                    LSA_UINT16 retVal;

					HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (internal.isr) rqb(0x%x) HIF_OPC_ISR - cannot get instance (%d)", rqb, rqb->args.internal_req.receive.hHDest);
					// hif_get_instance() returns NULL hif_free_instance() was already called.
					// Hera ipc interrupts are not synchronized (fire and forget).
					// Currently hif_free_instance() is called right after disabling ipc interrupts.
					// Hif thread receiving messages is terminated later on.
					// -> HIF_OPC_ISR can be pending in message box after hif_free_instance().
					// -> ignore HIF_OPC_ISR if hif_free_instance() was already called. -> no fatal for late interrupt.
					// HIF_FATAL(0);

                    // A HIF internal RQB is still active, that tries to trigger an interrupt.
                    // But before the RQB arrives, HIF was already closed, so the RQB is "lost" and someone must free it.
                    // This is a normal behavior, the RQB is stored in pHifInst->pRQBIsr = rqb; On Error we have to free this RQB.
                    HIF_FREE_LOCAL_MEM(&retVal, rqb, LSA_COMP_ID_HIF, HIF_MEM_TYPE_RQB);
                    HIF_ASSERT(retVal == LSA_RET_OK);
                }
				break;
			case HIF_OPC_RECEIVE:
                /* Special OPC for short mode */
				hif_get_instance(&pHifInst, rqb->args.internal_req.receive.hHDest);
				if(HIF_IS_NOT_NULL(pHifInst))
				{
					HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (internal.receive) rqb(0x%x) hH(%d) HIF_OPC_RECEIVE ld(%d)", rqb, rqb->args.internal_req.receive.hHDest, bLDRequest);

					/* return trigger resource */
					pHifInst->pRQBShort = rqb;
					/* process received rqbs */
					
                    hif_process_rcv_list(pHifInst);
				}
				else
				{
					HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "hif_request(-) (internal.receive) rqb(0x%x) HIF_OPC_RECEIVE - cannot get instance (%d) ld(%d)", rqb, rqb->args.internal_req.receive.hHDest, bLDRequest);
					HIF_FATAL(0);
				}
				break;
			default:
				HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "hif_request(-) (internal) rqb(0x%x) invalid internal opcode (%d) ld(%d)", rqb, HIF_RQB_GET_OPCODE(rqb), bLDRequest);
				HIF_FATAL(0);
		}
	}
	else /* external request! - UseCase B */
	{
        switch (comp_id)
        {
	        #if (HIF_CFG_USE_EDD == 1)
            case LSA_COMP_ID_EDD:
            case LSA_COMP_ID_EDDI:
            case LSA_COMP_ID_EDDP:
            case LSA_COMP_ID_EDDS:
            case LSA_COMP_ID_EDDT:
            {
	            EDD_RQB_TYPE        * const pEddRQB           = (EDD_RQB_TYPE*)HIF_CAST_TO_VOID_PTR(rqb);
                EDD_HANDLE_LOWER_TYPE const EddHandleLower    = HIF_EDD_RQB_GET_HANDLE_LOWER(pEddRQB);
                LSA_UINT32            const ChHandlesIndexU32 = (LSA_UINT32)EddHandleLower;

                HIF_ASSERT(ChHandlesIndexU32 < HIF_MAX_CH_HANDLES);
		        pChHandle = &(g_pHifData->ChHandles[ChHandlesIndexU32]);
                break;
            }
            #endif
            default:
            {
                HIF_ASSERT(comp_id < HIF_MAX_COMP_ID);
		        pChHandle = &(g_pHifData->ChHandles[HIF_RQB_GET_HANDLE(rqb)]);
            }
        }

		if(pChHandle->bUsed == LSA_TRUE)
		{
			if(pChHandle->bValid == LSA_TRUE)
			{
				if(pChHandle->pPipe->State == HIF_PIPE_STATE_OPENED_UPPER)
				{
					HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_request (external) rqb(0x%x) hH(%d) pipe(%d) - forward request to hif lower ld(%d)", rqb, pChHandle->pPipe->pInstance->hH, pChHandle->pPipe->PipeIndex, bLDRequest);

					/* request from upper => send to lower */
					hif_send(pChHandle->pPipe, rqb);
				}
				else
				{
					HIF_PROGRAM_TRACE_04(0, LSA_TRACE_LEVEL_UNEXP, "hif_request (external) rqb(0x%x) hH(%d) pipe(%d) - bad pipe state (%d)", rqb, pChHandle->pPipe->pInstance->hH, pChHandle->pPipe->PipeIndex, pChHandle->pPipe->State);

					hif_callback(pChHandle, rqb, hif_map_lsa_to_rqb_response(LSA_RET_ERR_SEQUENCE));
				}
			}
			else /* invalid channel handle!!! */
			{
				HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_UNEXP, "hif_request(-) (external) channel handle not valid!");
				HIF_FATAL(0);
			}
		}
		else /* invalid channel handle!!! */
		{
			HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "hif_request(-) (external) invalid channel handle");
			HIF_FATAL(0);
		}
	}

	if(bLDRequest)
	{
		HIF_LD_EXIT();
	}
	else
	{
		HIF_HD_EXIT();
	}

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_lower_req = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_request(): The total function duration is %d ns",(end_count_lower_req - start_count_lower_req)); //TOD0 z002p64u
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_request");
	#endif
}

/* description: see header */
LSA_VOID hif_ld_l_request(HIF_RQB_PTR_TYPE rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_ld_l_request(+) - rqb(0x%x), g_pHifData(0x%x)", rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->LD.PollTimer, LSA_TRUE);
}

/* description: see header */
LSA_VOID hif_ld_u_request(HIF_RQB_PTR_TYPE rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_ld_u_request(+) - rqb(0x%x), g_pHifData(0x%x)", rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->LD.PollTimer, LSA_TRUE);
}

/* description: see header */
LSA_VOID hif_hd_l_request(HIF_RQB_PTR_TYPE rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_hd_l_request(+) - rqb(0x%x), g_pHifData(0x%x)", rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->HD.PollTimer, LSA_FALSE);
}

/* description: see header */
LSA_VOID hif_hd_u_request(HIF_RQB_PTR_TYPE rqb)
{
	HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_hd_u_request(+) - rqb(0x%x), g_pHifData(0x%x)", rqb, g_pHifData);
	hif_request(rqb, &g_pHifData->HD.PollTimer, LSA_FALSE);
}

/**
 * @brief hif_lower_request_done
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_lower_request_done(LSA_VOID * rqb)
{
	/* (LaM)Real lower_request_done handling is done in hif_lower_request_done2()    */
	/* hif_lower_request_done2() has to be called in the correct hif context (HD/LD) */
	/* This function gives the system adaption the possibility to change contexts    */

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_request_done(+) - rqb(0x%x)", rqb);

#if (HIF_CFG_USE_REQUEST_LOWER_DONE_OUTPUT_MACRO == 1)
	{
		HIF_RQB_PTR_TYPE  pRQB = (HIF_RQB_PTR_TYPE)rqb;
		HIF_PIPE_PTR_TYPE pPipe;

		if(HIF_IS_NULL(pRQB))
		{
            HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_lower_request_done() - pRQB(0x%x) is not valid", pRQB);
			HIF_FATAL(0);
		}

		/* find source of request */
		hif_get_return_path_of_rqb(&pPipe, pRQB);
		HIF_ASSERT(HIF_IS_NOT_NULL(pPipe));

		HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_lower_request_done rqb(0x%x) hH(%d) pipe(%d) - return request to hif upper", rqb, pPipe->pInstance->hH, pPipe->PipeIndex);

        /* (LaM) in principle accessing pPipe structure is enter/exit critical!                 */
        /* But for performance reasons we don't use enter/exit here expecting that there is no	*/
        /* HIF user closing the channel/device while there are open requests in the system.		*/
		HIF_REQUEST_LOWER_DONE(hif_lower_request_done2, pRQB, pPipe->pInstance->hSysDev);
	}
#else
	hif_lower_request_done2(rqb);
#endif
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_lower_request_done");
}

/**
 * @brief hif_lower_request_done2
 * 
 * 
 * 
 * @param rqb
 * @return
 */
LSA_VOID hif_lower_request_done2(LSA_VOID * rqb)
{
	HIF_RQB_PTR_TYPE  pRQB = (HIF_RQB_PTR_TYPE)rqb;
	HIF_PIPE_PTR_TYPE pPipe;

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	LSA_UINT64 end_count_lower_req;
	LSA_UINT64 start_count_lower_req = HIF_GET_NS_TICKS();
	#endif

    HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_request_done2(+) - rqb(0x%x)", rqb);

	if(HIF_IS_NULL(pRQB))
	{
        HIF_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "hif_lower_request_done2(-) - pRQB(0x%x) is not valid", pRQB);
		HIF_FATAL(0);
	}

	/* find source of request */
	hif_get_return_path_of_rqb(&pPipe, pRQB);
    HIF_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "hif_lower_request_done2(-) - pRQB(0x%x) pPipe(0x%x)", pRQB, pPipe);
	HIF_ASSERT(HIF_IS_NOT_NULL(pPipe));

	hif_restore_rqb_user_fields_and_free_return_path(pRQB);

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_ENTER();
	}
	else /* HD Device */
	{
		HIF_HD_ENTER();
	}

	if(pPipe->State == HIF_PIPE_STATE_OPENING_LOWER)
	{
		hif_lower_open_channel_done(pPipe, pRQB);
	}	
	else
	{
        HIF_ASSERT(HIF_IS_NOT_NULL(pPipe->func.pCompFuncs));
        HIF_ASSERT(HIF_FCT_IS_NOT_NULL(pPipe->func.pCompFuncs->IsCloseChannelRqb));
		if(pPipe->func.pCompFuncs->IsCloseChannelRqb(pRQB) == LSA_TRUE)
		{
			hif_lower_close_channel_done(pPipe, pRQB);
		}
		else
		{
			HIF_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_NOTE_LOW, "hif_lower_request_done2 rqb(0x%x) hH(%d) pipe(%d) - return request to hif upper", rqb, pPipe->pInstance->hH, pPipe->PipeIndex);

			/* Other RQBs => return response to HIF Upper */
			hif_send(pPipe, pRQB);
		}
	}

	if(hif_is_ld_device(pPipe->pInstance) == LSA_TRUE)
	{
		HIF_LD_EXIT();
	}
	else /* HD Device */
	{
		HIF_HD_EXIT();
	}

	#if defined(HIF_DEBUG_MEASURE_PERFORMANCE)
	end_count_lower_req = HIF_GET_NS_TICKS();
	HIF_PROGRAM_TRACE_01(0,LSA_TRACE_LEVEL_NOTE_HIGH,"hif_lower_request_done2(): The total function duration is %d ns", (end_count_lower_req - start_count_lower_req));
	#else
	HIF_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "<<< hif_lower_request_done2");
	#endif
}

#endif // (HIF_CFG_USE_HIF == 1)

/*****************************************************************************/
/* End of File                                                               */
/*****************************************************************************/

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
/*  C o m p o n e n t     &C: CM (Context Management)                   :C&  */
/*                                                                           */
/*  F i l e               &F: cm_acp4.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  - cycle drop                                                             */
/*  - frame indications                                                      */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID  23
#define CM_MODULE_ID       23

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if CM_CFG_USE_ACP


/*===========================================================================*/
/*=== FRAME INDICATION ======================================================*/
/*===========================================================================*/

LSA_VOID
cm_acp_frame_indication(
	EDD_UPPER_CSRT_IND_DATA_PTR_TYPE  data,
	LSA_UINT8 * debug_info
) {
	CM_AR_GRAPH_IO_CR_PTR_TYPE  io_cr = (CM_AR_GRAPH_IO_CR_PTR_TYPE)data->UserID; /* see add_cons/add_prov */

	if (cm_is_null (io_cr))
	{
		CM_FATAL(); /* a bug in EDD */
	}
	else
	{
		CM_AR_GRAPH_PTR_TYPE  argr = io_cr->cm.backlink;

		if (! CM_ACPID_ISVALID(io_cr))
		{
			/* late, see cm_acp_frame_remove() */

			CM_ACP_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "FrameIndication: dev(%u) ar(%u) acp_id already invalid, ignored"
				, argr->device_nr, argr->ar_nr
				);
		}
		else
		{
			LSA_UINT32  state_event   = data->Event;
#if 0/*todo*/
			LSA_UINT32  total_state   = data->EventStatus; /* todo: check functionality */
#endif
			LSA_UINT16  cycle_counter = data->CycleCnt;

			/***/

			if (io_cr->cm.is_multicast )
			{
#if CM_CFG_MAX_MULTICAST_CHANNELS
				if( argr->ar_com.path_type != CM_PATH_TYPE_MULTICAST ) {

					CM_FATAL();
				}
				else if ((state_event & (EDD_CSRT_CONS_EVENT_AGAIN)) != 0)
				{
					/* note: again means again-in-red (see cm_acp_frame_add(), advanced mode) */

					if (io_cr->cm.cr_state == CM_AR_CR_WF_UP)
					{
						io_cr->cm.cr_state = CM_AR_CR_UP;

						cm_mc_ar_mcons_up(argr);
					}
					else
					{
						CM_ACP_TRACE_02(argr->trace_idx, LSA_TRACE_LEVEL_CHAT
							, "iocr(%08X) cr_state(%u) not WF_UP, ignored"
							, io_cr, io_cr->cm.cr_state
							);
					}
				}
				else if ((state_event & (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED)) != 0)
				{
					if (io_cr->cm.cr_state == CM_AR_CR_WF_UP || io_cr->cm.cr_state == CM_AR_CR_UP)
					{
						io_cr->cm.cr_state = CM_AR_CR_DOWN;
					}

					/* note: cm_ar_com_down_debug_data_set() has no effect (no alarms) */

					cm_ar_com_down2(argr, CM_AR_REASON_MISS, cycle_counter);
				}
#else
				CM_FATAL(); /* should not happen */
#endif
			}
			else /* unicast */
			{
				LSA_BOOL  do_com_check = LSA_FALSE;

				if ((state_event & (EDD_CSRT_CONS_EVENT_AGAIN)) != 0)
				{
					if( io_cr->cm.cr_state == CM_AR_CR_WF_UP )
					{
						CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), AGAIN, cycle_counter(0x%x)"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
							);

						io_cr->cm.cr_state = CM_AR_CR_UP;

						do_com_check = LSA_TRUE;
					}
					else
					{
						/* again after miss or after reset.req or passive */
						CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), AGAIN, state(%u) not WF_UP, ignoring"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, io_cr->cm.cr_state
							);
					}
				}
				else if ((state_event & (EDD_CSRT_CONS_EVENT_MISS | EDD_CSRT_CONS_EVENT_MISS_NOT_STOPPED)) != 0)
				{
					ACP_ERROR_INFO_DATA_TYPE debug_data;

					CM_ASSERT(sizeof(debug_data) == 12+4);
					CM_MEMCPY(&debug_data[0], debug_info, 12);
					CM_MEMCPY(&debug_data[12], data->DebugInfo, 4);

					if (io_cr->cm.cr_state == CM_AR_CR_WF_UP ||	io_cr->cm.cr_state == CM_AR_CR_UP)
					{
						if ((state_event & (EDD_CSRT_CONS_EVENT_MISS)) != 0)
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), MISS, last good cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}
						else
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), MISS_NOT_STOPPED, last good cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}

						io_cr->cm.cr_state = CM_AR_CR_DOWN;
						cm_ar_com_down_debug_data_set (argr, &debug_data);

						cm_ar_com_down2 (argr, CM_AR_REASON_MISS, cycle_counter);
					}
					else
					{
						CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
							, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), ignore miss in cr-state(%u)"
							, argr->device_nr, argr->ar_nr, io_cr->frame_id, io_cr->cm.cr_state
							);
					}
				}

				/***/

				if (! cm_ar_com_is_down (argr))
				{
					if ((state_event & (EDD_CSRT_CONS_EVENT_ENTERING_RED)) != 0) /* note: "leaving red" was removed with V5.3i1.2 (AP01448175) */
					{
						if (! io_cr->cm.is_rir)
						{
							io_cr->cm.is_rir = LSA_TRUE;

							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_WARN
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), Entering RecvInRed, cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);

							do_com_check = LSA_TRUE;
						}
						else
						{
							CM_ACP_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "ConsumerIndication: dev(%u) ar(%u) frame_id(0x%04x), Entering RecvInRed ignored!, cycle_counter(0x%x)"
								, argr->device_nr, argr->ar_nr, io_cr->frame_id, cycle_counter
								);
						}
					}
				}

				if (do_com_check)
				{
					cm_ar_com_check (argr);
				}
			}

			/***/

#if CM_CFG_MAX_SERVERS
# if (CM_DATA_STATUS_INIT | EDD_CSRT_DSTAT_BIT_DATA_VALID) != EDD_CSRT_DSTAT_LOCAL_INIT_VALUE
#  error "who changed that?"
# endif
#endif
			CM_ASSERT(CM_EVENT_APDU_STATUS_INITIAL_VALUE == ( CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_PRIMARY | CM_EVENT_APDU_STATUS_SYSRED_OK));
			CM_ASSERT(CM_EVENT_APDU_STATUS_INITIAL_SYSRED_VALUE == ( CM_EVENT_APDU_STATUS_RUN | CM_EVENT_APDU_STATUS_STATION_OK | CM_EVENT_APDU_STATUS_BACKUP  | CM_EVENT_APDU_STATUS_SYSRED_OK));

			if (! cm_ar_com_is_down (argr))
			{
				state_event &= (
					EDD_CSRT_CONS_EVENT_STOP                 | EDD_CSRT_CONS_EVENT_RUN |
					EDD_CSRT_CONS_EVENT_STATION_FAILURE      | EDD_CSRT_CONS_EVENT_STATION_OK |
					EDD_CSRT_CONS_EVENT_BACKUP               | EDD_CSRT_CONS_EVENT_PRIMARY |
					EDD_CSRT_CONS_EVENT_NO_PRIMARY_AR_EXISTS | EDD_CSRT_CONS_EVENT_PRIMARY_AR_EXISTS
					);

				if( state_event != 0)
				{
					cm_ar_com_cons_apdu_status_changed (argr, io_cr, state_event);
				}
			}
		}
	}
}

/*===========================================================================*/

#else
#  ifdef CM_MESSAGE
#    pragma CM_MESSAGE ("compiled _WITHOUT_ ACP; CM_CFG_USE_ACP=" CM_STRINGIFY(CM_CFG_USE_ACP))
#  endif /* CM_MESSAGE */
#endif /* CM_CFG_USE_ACP */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

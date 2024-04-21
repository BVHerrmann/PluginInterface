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
/*  F i l e               &F: cm_res.c                                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  resource things                                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID	8
#define CM_MODULE_ID		8

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


/*===========================================================================*/

#define CM_DO_RES_RESERVE 1
#define CM_DO_RES_ALLOC   2
#define CM_DO_RES_COUNT   3


/*===========================================================================*/
/*=== RESOURCES: INIT =======================================================*/
/*===========================================================================*/

LSA_VOID
cm_res_init(
	CM_CHANNEL_PTR_TYPE channel
) {
	channel->res.provider_cnt[0] = 0;
	channel->res.provider_cnt[1] = 0;

	{
	LSA_UINT16  i;

	for (i = 0; i < CM_RANGE_ID_MAX; ++i) {
		cm_range_init(&channel->res.id_range[i]);
	}
	}

	channel->res.consumer_cnt[0] = 0;
	channel->res.consumer_cnt[1] = 0;
}

/*===========================================================================*/
/*=== RESOURCES: UNDO_INIT ==================================================*/
/*===========================================================================*/

LSA_VOID
cm_res_undo_init(
	CM_CHANNEL_CONST_PTR_TYPE channel
) {
	LSA_UINT16  i;

	for (i = 0; i < CM_RANGE_ID_MAX; ++i) {
		cm_range_undo_init (& channel->res.id_range[i]);
	}

	CM_ASSERT (channel->res.provider_cnt[0] == 0);
	CM_ASSERT (channel->res.provider_cnt[1] == 0);

	CM_ASSERT (channel->res.consumer_cnt[0] == 0);
	CM_ASSERT (channel->res.consumer_cnt[1] == 0);
}


/*===========================================================================*/
/*=== ALLOCATE A FRAME RESOURCE =============================================*/
/*===========================================================================*/

LSA_BOOL
cm_res_frame_alloc (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 * frame_id_ptr /* consumer only */
) {
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(usr_channel);
	CM_EDD_CHANNEL_PARAMS_PTR_TYPE edd = cm_edd_channel_params(usr_channel);
	LSA_UINT16 do_res;
	LSA_UINT16 cons_idx;
	LSA_UINT16 cnt_idx;
	LSA_UINT16 nr_of_providers;
	LSA_UINT16 nr_of_consumers;
	LSA_BOOL  rc;

	/***/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_1_MULTICAST;
			do_res = CM_DO_RES_RESERVE;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_1_UNICAST;
			do_res = CM_DO_RES_ALLOC;
		}

		cnt_idx = 0;

		nr_of_providers = edd->ProviderCnt;

		nr_of_consumers = edd->ConsumerCntClass1;
		if( ! edd->ConsumerCntClass12Combined ) {
			nr_of_consumers += edd->ConsumerCntClass2;
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_2:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_2_MULTICAST;
			do_res = CM_DO_RES_RESERVE;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_2_UNICAST;
			do_res = CM_DO_RES_ALLOC;
		}

		cnt_idx = 0;

		nr_of_providers = edd->ProviderCnt;

		nr_of_consumers = edd->ConsumerCntClass2;
		if( ! edd->ConsumerCntClass12Combined ) {
			nr_of_consumers += edd->ConsumerCntClass1;
		}
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		cons_idx = 0; /* not used */
		do_res = CM_DO_RES_COUNT;

		cnt_idx = 1;

		nr_of_providers = edd->ProviderCntClass3;

		nr_of_consumers = edd->ConsumerCntClass3;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return LSA_FALSE; /* unreachable */
	}

	/***/

	if( is_consumer ) {

		CM_ASSERT(cm_is_not_null(frame_id_ptr));

		if( edd_channel->res.consumer_cnt[cnt_idx] >= nr_of_consumers ) {

			CM_AR_TRACE_03(edd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rt_class(%u) no cons-resource, nr_of_consumers(%u), class1_2_is_combined(%u)"
				, rt_class, nr_of_consumers, edd->ConsumerCntClass12Combined
				);

			rc = LSA_FALSE;
		}
		else {

			switch( do_res ) {
			case CM_DO_RES_COUNT:

				rc = LSA_TRUE;
				break;

			case CM_DO_RES_RESERVE:

				rc = cm_range_reserve_id  (&edd_channel->res.id_range[cons_idx], *frame_id_ptr);
				break;

			case CM_DO_RES_ALLOC:
				rc = cm_range_alloc_id (&edd_channel->res.id_range[cons_idx], frame_id_ptr);
				break;

			default:
				CM_FATAL1(do_res);
				return LSA_FALSE; /* unreachable */
			}

			if( rc ) {

				edd_channel->res.consumer_cnt[cnt_idx] += 1;
			}
		}
	}
	else {

		/* not: CM_ASSERT(cm_is_null(frame_id_ptr)); */
		/* not: CM_ASSERT(do_res); */

		if( edd_channel->res.provider_cnt[cnt_idx] >= nr_of_providers ) {

			CM_AR_TRACE_02(edd_channel->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH
				, "rt_class(%u) no prov-resource, nr_of_providers(%u)"
				, rt_class, nr_of_providers
				);

			rc = LSA_FALSE;
		}
		else {

			edd_channel->res.provider_cnt[cnt_idx] += 1;

			CM_AR_TRACE_02(edd_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u) prov-total(%u)"
				, rt_class, edd_channel->res.provider_cnt[cnt_idx]
				);

			rc = LSA_TRUE;
		}
	}

	/***/

	CM_AR_TRACE_01(edd_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
		, "rc(%u)", rc
		);

	return rc;
}

/*===========================================================================*/
/*=== FREE A FRAME RESOURCE =================================================*/
/*===========================================================================*/

LSA_VOID
cm_res_frame_free (
	CM_CHANNEL_PTR_TYPE usr_channel,
	LSA_BOOL is_consumer,
	LSA_BOOL is_multicast,
	LSA_UINT32 rt_class,
	LSA_UINT16 frame_id /* consumer only */
) {
	CM_CHANNEL_PTR_TYPE edd_channel = cm_edd_channel_get(usr_channel);
	LSA_UINT16 cons_idx;
	LSA_UINT16 cnt_idx;

	/***/

	switch (rt_class) {

	case CM_IOCR_PROP_RT_CLASS_1:
	case CM_IOCR_PROP_RT_CLASS_1_UDP:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_1_MULTICAST;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_1_UNICAST;
		}
		cnt_idx = 0;
		break;

	case CM_IOCR_PROP_RT_CLASS_2:
		if( is_multicast ) {
			cons_idx = CM_RANGE_ID_CLASS_2_MULTICAST;
		}
		else {
			cons_idx = CM_RANGE_ID_CLASS_2_UNICAST;
		}
		cnt_idx = 0;
		break;

	case CM_IOCR_PROP_RT_CLASS_3:
		cons_idx = 0; /* not used, only counting */
		cnt_idx = 1;
		break;

	default:
		CM_FATAL(); /* a bug, see caller */
		return; /* unreachable */
	}

	/***/

	if( is_consumer ) {

		if( rt_class != CM_IOCR_PROP_RT_CLASS_3 ) {

			cm_range_free_id (&edd_channel->res.id_range[cons_idx], frame_id);
		}

		if( edd_channel->res.consumer_cnt[cnt_idx] < 1 ) {

			CM_AR_TRACE_01(edd_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u)", rt_class
				);

			CM_FATAL(); /* a bug */
		}
		else {

			edd_channel->res.consumer_cnt[cnt_idx] -= 1;
		}
	}
	else {

		if( edd_channel->res.provider_cnt[cnt_idx] < 1 ) {

			CM_AR_TRACE_01(edd_channel->trace_idx, LSA_TRACE_LEVEL_CHAT
				, "rt_class(%u)", rt_class
				);

			CM_FATAL(); /* a bug */
		}
		else {

			edd_channel->res.provider_cnt[cnt_idx] -= 1;
		}
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

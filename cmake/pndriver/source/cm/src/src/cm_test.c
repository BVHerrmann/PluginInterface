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
/*  F i l e               &F: cm_test.c                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  some internal test functions                                             */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  9
#define CM_MODULE_ID       9

#include "cm_int.h"

CM_FILE_SYSTEM_EXTENSION(CM_MODULE_ID) /* no semicolon */


#if 0 /* tests disabled, must not depend on a foreign #define (AP01476188) */

/* note: entry point is cm_run_test_cases(), to be called after cm_init() */

/*----------------------------------------------------------------------------*/

#ifdef CM_MESSAGE
#pragma CM_MESSAGE("warning: TEST-CASES configured!")
#endif

/*----------------------------------------------------------------------------*/

#define CM_XLOG_01   CM_UPPER_TRA##CE_01 /* to hide 'malformed traces' from LTRC-scanner (AP01366017) */
#define CM_SLOG(s_)  CM_XLOG_01(0, LSA_TRACE_LEVEL_CHAT, "%u  struct:" #s_ , sizeof(s_));
#define CM_DLOG(s_)  CM_XLOG_01(0, LSA_TRACE_LEVEL_CHAT, "%u  define:" #s_ , s_);

static LSA_VOID
cm_test_log_size (LSA_VOID)
{
	CM_DLOG (CM_CFG_MAX_CLIENTS);

	CM_DLOG (CM_CFG_MAX_SERVERS);

	CM_DLOG (CM_CFG_MAX_MULTICAST_CHANNELS);

	CM_DLOG (CM_MAX_CHANNELS);

	/* conditional see beneath */

	CM_SLOG (cm_channels);

	CM_SLOG (CM_RQB_TYPE);
	CM_SLOG (CM_ANY_ARGS_TYPE);

	CM_SLOG (CM_CHANNEL_TYPE);
	CM_SLOG (cm_channels[0].usr);
	CM_SLOG (cm_channels[0].usr.pd);
#if CM_CFG_MAX_CLIENTS
	CM_SLOG (cm_channels[0].usr.cl);
#endif
#if CM_CFG_MAX_MULTICAST_CHANNELS
	CM_SLOG (cm_channels[0].usr.mc);
#endif
#if CM_CFG_MAX_SERVERS
	CM_SLOG (cm_channels[0].usr.sv);
#endif
	CM_SLOG (cm_channels[0].res);
	CM_SLOG (cm_channels[0].lower);
	CM_SLOG (cm_channels[0].lower.info);
	CM_SLOG (cm_channels[0].lower.info.acp);
	CM_SLOG (cm_channels[0].lower.info.edd);

	CM_SLOG (CM_DETAIL_TYPE);
	CM_SLOG (CM_INIT_TYPE);

	CM_SLOG (CM_ARCB_ADD_TYPE);

	CM_SLOG (CM_ARCB_IOCR_TYPE);
	CM_SLOG (CM_ARCB_IOAPI_TYPE);
	CM_SLOG (CM_ARCB_IODATA_OBJECT_TYPE);
	CM_SLOG (CM_ARCB_IOCS_TYPE);

	CM_SLOG (CM_ARCB_AP_TYPE);
	CM_SLOG (CM_ARCB_MODULE_TYPE);
	CM_SLOG (CM_ARCB_SUBMODULE_TYPE);
#if CM_CFG_MAX_CLIENTS
	CM_SLOG (CM_ARCB_RECORD_TYPE);
#endif

	CM_SLOG (struct cm_ar_graph_tag);

	CM_SLOG (CM_AR_GRAPH_AR_TYPE);

	CM_SLOG (struct cm_ar_graph_io_cr_tag);
	CM_SLOG (CM_ARCB_IOAPI_TYPE);
	CM_SLOG (CM_ARCB_IODATA_OBJECT_TYPE);
	CM_SLOG (CM_ARCB_IOCS_TYPE);

	CM_SLOG (struct cm_ar_graph_alarm_cr_tag);

	CM_SLOG (struct cm_ar_graph_ap_tag);
	CM_SLOG (struct cm_ar_graph_module_tag);
	CM_SLOG (struct cm_ar_graph_submodule_tag);

	CM_SLOG (struct cm_ar_graph_mcr_tag);

	CM_SLOG (CM_ARCB_IR_INFO_TYPE);

	CM_SLOG (CM_ARCB_SR_INFO_TYPE);

#if CM_CFG_MAX_CLIENTS
	CM_DLOG (CM_CFG_MAX_CL_LOG_ENTRIES);

	CM_SLOG (CM_CL_CHANNEL_TYPE);
	CM_SLOG (CM_CL_DEVICE_TYPE);
	CM_SLOG (CM_CL_ARGR_TYPE);
	CM_SLOG (struct cm_pd_record_tag);
	CM_SLOG (struct cm_ar_graph_slot_nr_tag);
#endif

#if CM_CFG_MAX_SERVERS
	CM_DLOG (CM_CFG_MAX_SV_DEVICES);
	CM_DLOG (CM_CFG_MAX_SV_ARS);
	CM_DLOG (CM_CFG_MAX_SV_LOG_ENTRIES);

	CM_SLOG (CM_SV_CHANNEL_TYPE);
	CM_SLOG (CM_SV_DEVICE_TYPE);
	CM_SLOG (CM_SV_LOG_ENTRY_TYPE);
	CM_SLOG (CM_SV_AP_TYPE);
	CM_SLOG (CM_SV_SLOT_TYPE);
	CM_SLOG (CM_SV_SUBSLOT_TYPE);
	CM_SLOG (CM_SV_DIAG_TYPE);
	CM_SLOG (CM_SV_ARGR_TYPE);
#endif

#if CM_CFG_MAX_MULTICAST_CHANNELS

	CM_SLOG (CM_MC_CHANNEL_TYPE);
	CM_SLOG (CM_MC_DEVICE_TYPE);
#endif
}


/*----------------------------------------------------------------------------*/

static LSA_UINT16
cm_test_strlen(LSA_UINT8 *str) /* AP01315376 */
{
	LSA_UINT16 len = 0;

	while( *str != 0 ) {
		str += 1;
		len += 1;
	}

	return len;
}


/*----------------------------------------------------------------------------*/

static LSA_VOID
cm_test_station_name (LSA_VOID)
{
	/* test-cases for station-name validation */
	LSA_UINT8 * s;
	LSA_BOOL ok;

	s = (LSA_UINT8*)"";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"";
	ok = cm_is_valid_station_name(10, LSA_NULL);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"A"; /* case-insensitive!!! (V3.2i2 or later) */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ABCDEFGHIJKLMNOPQRSTUVWXYZ.abcdefghijklmnopqrstuvwxyz"; /* allow upper case for compatibility */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)".";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)".a";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"-a";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a-.b";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a.";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a..";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"a.-";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz"; /*64*/
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"b.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"b.aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaz.c";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a.9";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a1.1b.c";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"a-a.b-b-b.c-c-c.d----d.d";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa."
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa."
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.z";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678.a12345678.a12345678.a12345678.a12345678."
		"a12345678.a12345678.a12345678z";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123456";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123.b";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	/* the first (leftmost) label does not match with "port-xyz" with x, y, z = 0...9 */
	/* the first (leftmost) label does not match "port-xyz-abcde" with x, y, z, a, b, c, d, e = 0...9 */

	s = (LSA_UINT8*)"port-1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-1234";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123-1";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"port-123-12345";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"Port-123-12345";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123-123456";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"ok.port-123";
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1.2.3.4"; /* ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"999.999.999.999"; /* ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"1.2.3"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1.2.3.4.5"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"1a.2.3.4"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"9991.2.3.4"; /* not an ip-addr */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	s = (LSA_UINT8*)"12345"; /* ok, "abcde" with a, b, c, d, e = 0...9 */
	ok = cm_is_valid_station_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	/*
	 * AliasNameValue = LLDP_PortID + "." + LLDP_ChassisID
	 * LLDP_PortID    = "port-xyz" or "port-xyz-rstuv"
	 * LLDP_ChassisID = NameOfStationValue
	 */

	s = (LSA_UINT8*)"port-123";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.port-123";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(!ok);

	s = (LSA_UINT8*)"port-123.a";
	ok = cm_is_valid_alias_name(cm_test_strlen(s), s);
	CM_ASSERT(ok);

	ok = ok;
}


/*----------------------------------------------------------------------------*/

#define CM_TEST_IOCR_CHECKS_SUB_TYPE_IN    1
#define CM_TEST_IOCR_CHECKS_SUB_TYPE_OUT   2
#define CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT 3

#define CM_TEST_IOCR_CHECKS_DS1_MASK 0x1111
#define CM_TEST_IOCR_CHECKS_CS1_MASK 0x2222
#define CM_TEST_IOCR_CHECKS_DS2_MASK 0x4444 /* used only by combined INOUT submodules */
#define CM_TEST_IOCR_CHECKS_CS2_MASK 0x8888 /* used only by combined INOUT submodules */

#define CM_TEST_IOCR_CHECKS_ICR1_MASK 0x000F
#define CM_TEST_IOCR_CHECKS_ICR2_MASK 0x00F0
#define CM_TEST_IOCR_CHECKS_ICR_MASK  (CM_TEST_IOCR_CHECKS_ICR1_MASK | CM_TEST_IOCR_CHECKS_ICR2_MASK)

#define CM_TEST_IOCR_CHECKS_OCR1_MASK 0x0F00
#define CM_TEST_IOCR_CHECKS_OCR2_MASK 0xF000
#define CM_TEST_IOCR_CHECKS_OCR_MASK  (CM_TEST_IOCR_CHECKS_OCR1_MASK | CM_TEST_IOCR_CHECKS_OCR2_MASK)

static LSA_VOID
cm_test_iocr_checks(CM_CHANNEL_PTR_TYPE channel)
{
	LSA_UINT16 test_submodule;
	LSA_UINT16 sub_properties;

	CM_MAC_TYPE null_mac = { {0, 0, 0, 0, 0, 0} };
	CM_AR_GRAPH_PTR_TYPE  argr;
	LSA_UINT16 ar_nr = 1;
	CLRPC_UUID_TYPE  cmi_object_uuid;
	LSA_UINT16  block_field;
	LSA_UINT16  test_id;
	LSA_UINT16  needed_crs;
	LSA_UINT16  error_field;

	LSA_UINT16 ar_ok_cnt = 0;
	LSA_UINT16 ar_nok_cnt = 0;
	LSA_UINT16 dev_nr = 1;
	CLRPC_UUID_TYPE  ar_uuid = {0x7872d94b, 0xa869, 0xcb46, 0xb5, 0xbd, {0x69, 0xae, 0xb2, 0x5b, 0x4b, 0x5e}};

	LSA_UINT32 api = 0;
	LSA_UINT16 slot_nr = 0;
	LSA_UINT16 subslot_nr = 1;

	/*------------------------------------------------------------------------*/
	// Input or output submodule
	/*------------------------------------------------------------------------*/


	for(test_submodule = CM_TEST_IOCR_CHECKS_SUB_TYPE_IN; test_submodule <= CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT; test_submodule++) {

		switch(test_submodule) {

		case CM_TEST_IOCR_CHECKS_SUB_TYPE_IN:
			test_id = 0x0003;
			sub_properties = 0x0001;
			break;

		case CM_TEST_IOCR_CHECKS_SUB_TYPE_OUT:
			test_id = 0x0003;
			sub_properties = 0x0002;
			break;

		case CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT:
			test_id = 0x000F;
			sub_properties = 0x0003;
		break;

		default:
			test_id = 0;
			sub_properties = 0x000F;
			CM_FATAL();
		}

		for( ; ; /* ds in test_id gets shifted inside the block */) { /* DS1 block begin */
			for( ; ; /* cs in test_id gets shifted inside the block */) { /* CS1 block begin */
				for( ; ; /* ds in test_id gets shifted inside the block */) { /* DS2 block begin */
					for( ; ; /* cs in test_id gets shifted inside the block */) { /* CS2 block begin */

						cm_rpc_make_object_uuid (& cmi_object_uuid
							, channel->usr.cl.vendor_id, channel->usr.cl.device_id, channel->usr.cl.instance
							);

						argr = cm_ar_graph_alloc (channel, dev_nr, ar_nr);
						if (cm_is_null (argr))
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "test_id(0x%04x): out of resource", test_id);
							CM_FATAL();
						}

						block_field = cm_ar_graph_insert_io_ar (argr
							, 0x0100a8c0 /*ip_addr 192.168.0.1*/, 1 /*ar_type*/, & ar_uuid
							, null_mac.mac, & cmi_object_uuid
							, 0x40000011 /*ar_properties*/, 0x00c8/*cmi_timeout100ms*/, 0 /* see below (search for cmi_udp_port) */
							, 0, LSA_NULL
							);
						if (block_field != 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "io_ar.block_field(%u)", block_field);
							CM_FATAL();
						}

						/***/

						/* IO-CR blocks */
						{
						CM_AR_GRAPH_IO_CR_PTR_TYPE  iocr;

						LSA_UINT16 iocr_mask = CM_TEST_IOCR_CHECKS_ICR1_MASK;
						LSA_UINT16 iocr_ref = 1;

						needed_crs = test_id;
						error_field = 0;

						for( ; needed_crs > 0; ) {

							CM_AR_GRAPH_IODATA_OBJECT_PTR_TYPE  iodata;
							CM_AR_GRAPH_IOCS_PTR_TYPE  iocs;

							LSA_UINT16 iocr_type = 0;

							if( (needed_crs & iocr_mask) != 0) {
								/* create iocr */

								if( (needed_crs & CM_TEST_IOCR_CHECKS_ICR_MASK) != 0 ) {
									iocr_type = 1;
								}
								else if( (needed_crs & CM_TEST_IOCR_CHECKS_OCR_MASK) != 0 ) {
									iocr_type = 2;
								}

								if(iocr_type == 0) {
									CM_FATAL();
								}

								block_field = cm_ar_graph_insert_io_cr (argr
									, iocr_type, iocr_ref, 0x8892 /*ethertype*/, 2 /*iocr_properties*/, 58 /*data_length*/, 0x8000 /*frame_id*/
									, 32 /*send_clock*/, 512 /*reduction_ratio*/, 1 /*phase*/, 0xffffffff /*frame_send_offset*/
									, 3 /*data_hold_factor*//*AP01304706*/, 3 /*data_hold_factor*/, 0xC000 /*iocr_tag_header*/, null_mac.mac
									, & iocr
									);
								if (block_field == 0)
								{
									iocr->cm.edd_data_offset = 0;
									iocr->cm.partial_length = iocr->data_length;
								}
								if (block_field != 0)
								{
									CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "io_cr.block_field(%u) iocr_ref(%u)", block_field, iocr_ref);
									CM_FATAL();
								}
								CM_ASSERT(cm_is_not_null (iocr));

								//CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), iocr_mask(0x%04x), iocr_ref(%u), iocr_type(%u), iocr inserted", test_id, iocr_mask, iocr_ref, iocr_type);

								{
								CM_AR_GRAPH_IOAPI_PTR_TYPE  ar_ioap;

								block_field = cm_argr_insert_ioap (iocr, api, & ar_ioap);
								if (block_field != 0)
								{
									CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ioap.block_field(%u)", block_field);
									CM_FATAL();
								}
								CM_ASSERT(cm_is_not_null (ar_ioap));

								/***/

								if( (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS1_MASK) != 0 ) {
									block_field = cm_argr_insert_iodata_objects (ar_ioap
										, slot_nr, subslot_nr, 0 /*frame_offset*/, & iodata
										);
									if (block_field != 0)
									{
										CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iodata.block_field(%u)", block_field);
										error_field |= (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS2_MASK);
									}
									CM_ASSERT(cm_is_not_null (iodata));
									CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), iocr_mask(0x%04x), iocr_ref(%u), iocr_type(%u), iocr(0x%04x), ioap(0x%04x), iodata(0x%04x) inserted", test_id, iocr_mask, iocr_ref, iocr_type, iocr, ar_ioap, iodata);
								}

								if( (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_CS1_MASK) != 0 ) {
									block_field = cm_argr_insert_iocs (ar_ioap
										, slot_nr, subslot_nr, 0 /*frame_offset*/, &iocs
										);
									if (block_field != 0)
									{
										CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iocs.block_field(%u)", block_field);
										error_field |= (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS2_MASK);
									}
									CM_ASSERT(cm_is_not_null (iocs));
									CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), iocr_mask(0x%04x), iocr_ref(%u), iocr_type(%u), iocr(0x%04x), ioap(0x%04x), iocs(0x%04x) inserted", test_id, iocr_mask, iocr_ref, iocr_type, iocr, ar_ioap, iocs);
								}
								if( (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS2_MASK) != 0 ) {
									block_field = cm_argr_insert_iodata_objects (ar_ioap
										, slot_nr, subslot_nr, 0 /*frame_offset*/, & iodata
										);
									if (block_field != 0)
									{
										CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iodata.block_field(%u)", block_field);
										error_field |= (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS2_MASK);
									}
									CM_ASSERT(cm_is_not_null (iodata));
									CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), iocr_mask(0x%04x), iocr_ref(%u), iocr_type(%u), iocr(0x%04x), ioap(0x%04x), iodata(0x%04x) inserted", test_id, iocr_mask, iocr_ref, iocr_type, iocr, ar_ioap, iodata);
								}

								if( (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_CS2_MASK) != 0 ) {
									block_field = cm_argr_insert_iocs (ar_ioap
										, slot_nr, subslot_nr, 0 /*frame_offset*/, &iocs
										);
									if (block_field != 0)
									{
										CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "iocs.block_field(%u)", block_field);
										error_field |= (test_id & iocr_mask & CM_TEST_IOCR_CHECKS_DS2_MASK);
									}
									CM_ASSERT(cm_is_not_null (iocs));
									CM_CL_TRACE_07 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), iocr_mask(0x%04x), iocr_ref(%u), iocr_type(%u), iocr(0x%04x), ioap(0x%04x), iocs(0x%04x) inserted", test_id, iocr_mask, iocr_ref, iocr_type, iocr, ar_ioap, iocs);
								}
								}

								/* prepare next creation of next iocr*/
								iocr_ref += 1;
							}

							needed_crs &= ~iocr_mask;
							iocr_mask <<= 4;
						}
						}

						/* fake alarm_cr */
						{
						block_field = cm_ar_graph_insert_alarm_cr(argr
							, 1, 0x8892, 0, 1, 3, 0, 200, 0xC000, 0xA000
							, &argr->alarm_cr
							);

						if( block_field != 0 ) {

							CM_SV_TRACE_01(argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "invalid alcr, field(%u)"
								, block_field
								);
							CM_FATAL();
						}
						}

						/* api / slot / subslot */
						{
						CM_AR_GRAPH_AP_PTR_TYPE  ap;

						CM_AR_GRAPH_MODULE_PTR_TYPE  mod;

						CM_AR_GRAPH_SUBMODULE_PTR_TYPE  submod;

						block_field = cm_ar_graph_insert_ap_module (argr
							, api, slot_nr, 0x0000002d /*mod_ident*/, 0x0000 /*mod_properties*/
							, LSA_NULL, & ap, & mod
							);
						if (block_field != 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "ap.block_field(%u)", block_field);
							CM_FATAL();
						}
						CM_ASSERT (cm_is_not_null (mod));

						{
						LSA_UINT16  cnt = 0;
						LSA_UINT16  tmp;

						block_field = cm_ar_graph_insert_submodule (argr, mod
							, subslot_nr, 0x00010001 /*sub_ident*/, sub_properties
							, channel->usr.cl.IOXS_Required
							, & submod
							);
						if (block_field != 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "mod.block_field(%u)", block_field);
							CM_FATAL();
						}


						tmp = CM_CAST_U16 (sub_properties & CM_SUB_PROP_TYPE_MASK);
						switch (tmp)
						{
						case CM_SUB_PROP_TYPE_NO_DATA:
						case CM_SUB_PROP_TYPE_INPUT:
						case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
							++cnt;
							block_field = cm_ar_graph_set_data_descr (submod
								, 0  /* 0..input   1..output */, 1 /*data_description*/
								, 0 /*data_length*/, 1 /*iops_length*/, 1 /*iocs_length*/
								);
							if (block_field != 0)
							{
								CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submod.block_field(%u)", block_field);
								CM_FATAL();
							}
							break;

						default:
							/* nothing */
							break;
						}

						switch (tmp)
						{
						case CM_SUB_PROP_TYPE_OUTPUT:
						case CM_SUB_PROP_TYPE_INPUT_OUTPUT:
							++cnt;
							block_field = cm_ar_graph_set_data_descr (submod
								, 1  /* 0..input   1..output */, 2 /*data_description*/
								, 0 /*data_length*/, 1 /*iops_length*/, 1 /*iocs_length*/
								);
							if (block_field != 0)
							{
								CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "submod.block_field(%u)", block_field);
								CM_FATAL();
							}
							break;

						default:
							/* nothing */
							break;
						}

						if (cnt == 0)
						{
							CM_CL_TRACE_01 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP
								, "wrong SubmoduleProperties.Type(0x%x), submod.block_field(12)", tmp
								);
							CM_FATAL();
						}
						}
						}

						/***/

						cm_ar_graph_dump(argr);

						/***/

						{
						LSA_UINT32 pnio_err;

						pnio_err = cm_ar_graph_check_consistency (argr);

						CM_CL_TRACE_02 (argr->trace_idx, LSA_TRACE_LEVEL_CHAT, "test_id(0x%04x), pnio_err(0x%x)", test_id, pnio_err);

						if (error_field != 0) {

							ar_nok_cnt +=1;
							CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "test_id(0x%04x), ar ok/nok(%2u/%3u), error at iocr creation (0x%04x)", test_id, ar_ok_cnt, ar_nok_cnt, error_field);
						}
						else if (pnio_err != CM_PNIO_ERR_NONE) {

							if (CM_PNIO_ERR_FETCH_CODE_1 (pnio_err) == 64/*RMPM*/)
							{
								pnio_err = CM_PNIO_FAULTY_ConnectBlockReq (62/*CMCTL*/, 3/*Out of resource*/);
							}

							ar_nok_cnt +=1;
							CM_CL_TRACE_04 (argr->trace_idx, LSA_TRACE_LEVEL_UNEXP, "test_id(0x%04x), ar ok/nok(%2u/%3u), return from cm_ar_graph_check_consistency:pnio_err(0x%x)", test_id, ar_ok_cnt, ar_nok_cnt, pnio_err);
						}
						else {

							ar_ok_cnt +=1;
							CM_CL_TRACE_03 (argr->trace_idx, LSA_TRACE_LEVEL_NOTE_HIGH, "test_id(0x%04x), ar ok/nok(%2u/%3u), cm_ar_graph_check_consistency succeeded", test_id, ar_ok_cnt, ar_nok_cnt);

							/* prepare next creation of next ar*/
							dev_nr += ar_ok_cnt;
							ar_uuid.time_low += 1;
						}

						cm_ar_graph_free(argr);
						}

						// CS2 block adjustment...
						if( (test_id & CM_TEST_IOCR_CHECKS_CS2_MASK) != 0 ) {

							CM_ASSERT(test_submodule == CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT);

							if( (test_id & CM_TEST_IOCR_CHECKS_OCR2_MASK & CM_TEST_IOCR_CHECKS_CS2_MASK) < 0x8000 ) {
								test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS2_MASK) | ( (test_id & CM_TEST_IOCR_CHECKS_CS2_MASK) << 4 );
								CM_ASSERT( (test_id & CM_TEST_IOCR_CHECKS_CS2_MASK) != 0 );
							}
							else {
								break;
							}
						}
						else {
							break;
						}
					}

					// DS2 block adjustment
					if( (test_id & CM_TEST_IOCR_CHECKS_DS2_MASK) != 0 ) {

						CM_ASSERT(test_submodule == CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT);

						if( (test_id & CM_TEST_IOCR_CHECKS_OCR2_MASK & CM_TEST_IOCR_CHECKS_DS2_MASK) < 0x4000 ) {
							test_id = (test_id & ~CM_TEST_IOCR_CHECKS_DS2_MASK) | ( (test_id & CM_TEST_IOCR_CHECKS_DS2_MASK) << 4 );
							test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS2_MASK) | 0x0008; /* set CS2 back to first ICR */
							CM_ASSERT( (test_id & CM_TEST_IOCR_CHECKS_DS2_MASK) != 0 );
						}
						else {
							break;
						}
					}
					else {
						break;
					}
				}

				if( (test_id & CM_TEST_IOCR_CHECKS_OCR2_MASK & CM_TEST_IOCR_CHECKS_CS1_MASK) < 0x2000 ) {
					test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS1_MASK) | ( (test_id & CM_TEST_IOCR_CHECKS_CS1_MASK) << 4 );
					CM_ASSERT( (test_id & CM_TEST_IOCR_CHECKS_CS1_MASK) != 0 );

					if(test_submodule == CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT) {
						test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS2_MASK) | 0x0008; /* set CS2 back to first ICR */
						test_id = (test_id & ~CM_TEST_IOCR_CHECKS_DS2_MASK) | 0x0004; /* set DS2 back to first ICR */
					}
				}
				else {
					break;
				}
			}

			if( (test_id & CM_TEST_IOCR_CHECKS_OCR2_MASK & CM_TEST_IOCR_CHECKS_DS1_MASK) < 0x1000 ) {
				test_id = (test_id & ~CM_TEST_IOCR_CHECKS_DS1_MASK) | ( (test_id & CM_TEST_IOCR_CHECKS_DS1_MASK) << 4 );
				CM_ASSERT( (test_id & CM_TEST_IOCR_CHECKS_DS1_MASK) != 0 );

				test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS1_MASK) | 0x0002; /* set CS1 back to first ICR */

				if(test_submodule == CM_TEST_IOCR_CHECKS_SUB_TYPE_INOUT) {
					test_id = (test_id & ~CM_TEST_IOCR_CHECKS_CS2_MASK) | 0x0008; /* set CS2 back to first ICR */
					test_id = (test_id & ~CM_TEST_IOCR_CHECKS_DS2_MASK) | 0x0004; /* set DS2 back to first ICR */
				}
			}
			else {
				break;
			}
		}
	}
}


/*----------------------------------------------------------------------------*/

void cm_run_test_cases_COC(void) {

	/*
	 * CombinedObjectContainer ::= ( COContainerContent, [Padding]* <a> )*
	 *   <a> The number of padding octets (value=0) shall be 0,1,2,3 to have 32 bit alignment to the next COContainerContent
	 *
	 * COContainerContent ::=
	 *   [ 0] BlockHeader, Padding, Padding,
	 *   [ 8] API, Slot, Subslot,
	 *   [16] Padding, Padding, Index,
	 *   [20] COContainerBlock
	 *
	 * COContainerBlock ::= RecordDataWrite <a>
	 *   <a> Without CombinedObjectContainer
	 */

	const LSA_UINT32 ap = 0;
	const LSA_UINT16 sl = 0;
	const LSA_UINT16 su = 0x8000;
	LSA_BOOL is_access_denied;

	{ /* empty or too short for BlockType+BlockLength; note that version is part of the content (because included in length) */
	LSA_UINT8  data[] = {0,0,0};
	LSA_UINT32 size;
	for( size = 0; size <= sizeof(data); ++size) {
		LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
		CM_ASSERT(offs == CM_RECORD_OFFSET);
		CM_ASSERT(is_access_denied == LSA_FALSE);
	}
	}

	{ /* wrong BlockType (and wrong length; blame BlockType) */
	LSA_UINT8  data[] = {0x04, 99, 0, 1};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* correct BlockType but wrong BlockLength */
	LSA_UINT8  data[] = {0x04, 0x01, 0, 1};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 2);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* correct BlockType and BlockLength but too short for a COC */
	LSA_UINT8  data[] = {0x04, 0x01, 0, 0xFF/*patched*/, 11,11, 22,22, 33,33,33,33, 44,44, 55,55, 66,66, 77,77 /*no-data*/ };
	LSA_UINT32 size;
	for( size = 4; size <= sizeof(data); ++size) {
		LSA_UINT32 offs;
		data[3] = (LSA_UINT8)(size - 4);
		offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
		CM_ASSERT(offs == CM_RECORD_OFFSET + 2);
		CM_ASSERT(is_access_denied == LSA_FALSE);
	}
	}

	{ /* minimal block */
	LSA_UINT8  data[] = {0x04, 0x01, 0, 16+1, 11,11, 22,22, 33,33,33,33, 44,44, 55,55, 66,66, 77,77, 88/*data*/ };
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs;

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 4);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[4] = 1; // correct version-high

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 5);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[5] = 0; // correct version-low

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 6);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[6] = 0; // correct padding

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 6); // hmmm... check-code sees paddings as U16
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[7] = 0; // correct padding

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 8); // API wrong
	CM_ASSERT(is_access_denied == LSA_TRUE);
	CM_PUT32_HTON(data, 8, ap); // correct API

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs ==  CM_RECORD_OFFSET + 14); // subslot wrong
	CM_ASSERT(is_access_denied == LSA_TRUE);
	CM_PUT16_HTON(data, 14, su); // correct subslot

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 16); // Ha! Fehler gefunden!
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[16] = 0; // correct padding

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 16); // hmmm... check-code sees paddings as U16
	CM_ASSERT(is_access_denied == LSA_FALSE);
	data[17] = 0; // correct padding

	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 18);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	/***/

	{ /* wrong BlockType */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x00, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* too short BlockLength (<-VH - Index->) */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x02);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* too long BlockLength */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x16, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x02);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* wrong BlockVersion */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x00, 0xaa, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x04);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* wrong API */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x08);
	CM_ASSERT(is_access_denied == LSA_TRUE);
	}

	{ /* wrong subslot*/
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs;
	LSA_UINT16 su_if;
	LSA_UINT8  su_high;

	su_if = 0x8000;
	for(su_high = 0x81; su_high <= 0x8f; ) {
		data[14] = su_high;
		offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su_if, 0, &is_access_denied);
		CM_ASSERT(offs == CM_RECORD_OFFSET + 0x14);
		CM_ASSERT(is_access_denied == LSA_TRUE);
		su_high +=1;
	}

	su_if = 0x9000;
	for(su_high = 0x91; su_high <= 0x9f; ) {
		data[12] = su_high;
		offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su_if, 0, &is_access_denied);
		CM_ASSERT(offs == CM_RECORD_OFFSET + 0x14);
		CM_ASSERT(is_access_denied == LSA_TRUE);
		su_high +=1;
	}
	}

	{ /* wrong index */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs;

	CM_PUT16_HTON(data, 18, 0x80b0); /* nested COC not allowed */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x12);
	CM_ASSERT(is_access_denied == LSA_FALSE);

	CM_PUT16_HTON(data, 18, 0xE050); /* CM_REC_IDX_8000 violated but writable */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x12);
	CM_ASSERT(is_access_denied == LSA_FALSE);

	CM_PUT16_HTON(data, 18, 0x8000); /* CM_REC_IDX_WRITE violated */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x12);
	CM_ASSERT(is_access_denied == LSA_FALSE);

	CM_PUT16_HTON(data, 18, 0xAFF1); /* CM_REC_IDX_PDEV violated */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x12);
	CM_ASSERT(is_access_denied == LSA_FALSE);

	CM_PUT16_HTON(data, 14, 0x8001);
	CM_PUT16_HTON(data, 18, 0x8020); /* CM_REC_IDX_PORT violated */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == 0/*ok!*/); // this is not part of the syntax-check!
	CM_ASSERT(is_access_denied == LSA_FALSE);

	CM_PUT16_HTON(data, 14, 0x8000);
	CM_PUT16_HTON(data, 18, 0x802b); /* CM_REC_IDX_INTERFACE violated */
	offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == 0/*ok!*/); // this is not part of the syntax-check!
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	/***/

	{ /* 2 paddings missing behind 1st COCContent */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + 1 padding byte
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x00};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x1a);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* 1 paddings missing behind 1st COCContent */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + 2 padding byte
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x00, 0x00};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x1b);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* no following COCC after padding behind 1st COCContent */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + 3 padding byte
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x00, 0x00, 0x00};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x1c);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* padding between 1st and 2nd COCContent not null */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite missing padding but correct COCC
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x2c, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x19);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* wrong 2nd BlockType */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment|<-BlockT  >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite ----------->|
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x2c, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == CM_RECORD_OFFSET + 0x1c);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	/***/

	{ /* good COC (one correct COCC) (shortest valid) */
					//  |<----     1      ---->|<----      2      ---->|<----      3      ---->|<----      4      ---->|<----      5      ---->|<----      6      ---->|<----             ---->|
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x11, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x2c, 0xab};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == 0);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* good COC (one correct COCC) (some bytes more data) */
					//  |<----     1      ---->|<----      2      ---->|<----      3      ---->|<----      4      ---->|<----      5      ---->|<----      6      ---->|<----             ---->|
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x2c, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == 0);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

	{ /* good COC (two correct COCCs) */
					//  |<-BlockT >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite + padding to 32 bit alignment|<-BlockT  >|<-BlockL ->|<-VH>|<-VL>|<-Pad ---->|<-API ---------------->|<-SL ----->|<-SU ----->|<-Pad ---->|<-Index -->|<-RecordDataWrite ----------->|
	LSA_UINT8  data[] = {0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x20, 0xab, 0xcd, 0xef, 0x01, 0x23, 0x00, 0x00, 0x00, 0x04, 0x01, 0x00, 0x15, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x2c, 0xab, 0xcd, 0xef, 0x01, 0x23};
	LSA_UINT32 size = sizeof(data);
	LSA_UINT32 offs = cm_sv_pdev_CombinedObjectContainer_check(data - CM_RECORD_OFFSET, size + CM_RECORD_OFFSET, ap, sl, su, 0, &is_access_denied);
	CM_ASSERT(offs == 0);
	CM_ASSERT(is_access_denied == LSA_FALSE);
	}

}

/*----------------------------------------------------------------------------*/

LSA_VOID
cm_run_test_cases (
	CM_CHANNEL_PTR_TYPE channel
)
{
	//cm_test_log_size ();
	//cm_test_station_name ();
	//cm_run_test_cases_COC();
	//cm_test_iocr_checks(channel);
	#error "please, select the testcases"
}

/*----------------------------------------------------------------------------*/

#endif /* test-cases */

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

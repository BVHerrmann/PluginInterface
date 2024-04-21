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
/*  C o m p o n e n t     &C: TCPIP (TCIP for Interniche Stack)         :C&  */
/*                                                                           */
/*  F i l e               &F: tcip_mib.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  MIB integration                                                          */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  7
#define TCIP_MODULE_ID     7

#include "tcip_int.h"

TCIP_FILE_SYSTEM_EXTENSION(TCIP_MODULE_ID) /* no semicolon */

/*===========================================================================*/
/*===========================================================================*/

/*
 * function:       returns interface index
 * documentation:  LSA_TCIP_Detailspec.doc
 */
LSA_UINT8
tcip_get_mib2_ifindex(
    LSA_UINT32  interface_id,
	LSA_UINT16	port_id,
	LSA_UINT32* IfIndex
)
{
	int i;
	LSA_UINT8 ret = TCIP_ERR_PARAM;

	/*
	 * find channel with matching interface_id
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch = &tcip_data.tcip_channels[i];
		TCIP_IF_DATA_PTR_TYPE p_if_data;

		if( TCIP_CH_OPEN != ch->state )
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));
		p_if_data = ch->p_if_data;

		/*
		 *  look for matching interface id
		 */
		if( interface_id == p_if_data->edd_interface_id )
		{
			if( port_id > p_if_data->port_count )
			{
				ret = TCIP_ERR_PARAM;
				break;
			}

#if TCIP_CFG_INTERNICHE
			*IfIndex = p_if_data->port[port_id].ifp->mib.ifIndex + 1; /* note: mib.ifIndex is zero-based, see declaration */
#elif TCIP_CFG_OPEN_BSD
			*IfIndex = (LSA_UINT32)obsd_pnio_get_if_index(p_if_data->port[port_id].ifp);
#else
#error "unknown config"
#endif

			ret = TCIP_OK;

			break;
		}
	}

	return ret;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   sets interface description string
 */
LSA_UINT8
tcip_set_mib2_ifgroup_data(
	LSA_UINT32 interface_id,
	LSA_UINT16 port_id,
	LSA_UINT8* p_ifDescr,
	LSA_UINT16 ifDescr_length
)
{
	TCIP_LOG_FCT("tcip_set_mib2_ifgroup_data")
	int i;
	LSA_UINT8 ret = TCIP_ERR_PARAM;

	if( is_null(p_ifDescr) || ifDescr_length > TCIP_MAX_MIBII_IFSTRING_LEN )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed interface description string longer than 255 characters (%d)", ifDescr_length);
		return TCIP_ERR_PARAM;
	}

	/*
	 *  find interface with matching interface id
	 */
	for( i=0; i<TCIP_MAX_CHANNELS; i++ )
	{
		TCIP_CHANNEL_PTR_TYPE ch;
		TCIP_IF_DATA_PTR_TYPE p_if_data;

		ch = &tcip_data.tcip_channels[i];

		if( TCIP_CH_OPEN != ch->state )
		{
			continue;
		}

		TCIP_ASSERT(is_not_null(ch->p_if_data));
		p_if_data = ch->p_if_data;

		/*
		 *  look for matching interface id
		 */
		if( interface_id == p_if_data->edd_interface_id )
		{
			if( port_id > p_if_data->port_count )
			{
				ret = TCIP_ERR_PARAM;
				break;
			}

#if TCIP_CFG_INTERNICHE
			TCIP_MEMCPY(p_if_data->port[port_id].ifp->mib.ifDescr, p_ifDescr, ifDescr_length);
			p_if_data->port[port_id].ifp->mib.ifDescr[ifDescr_length] = 0;
#endif

#if TCIP_CFG_OPEN_BSD && TCIP_CFG_SNMP_ON
			{
			OBSD_IFNET ifp = p_if_data->port[port_id].ifp;
			obsd_pnio_set_if_description(ifp, (char *)p_ifDescr, ifDescr_length); /* also sets MIB II ifDescr */
			}
#endif

			ret = TCIP_OK;
			break;
		}
	}

	return ret;
}

/*===========================================================================*/
/*===========================================================================*/

/*
 *   sets parameters of mib2 system group
 */
LSA_UINT8
tcip_set_mib2_sysgroup_data(
	LSA_UINT8 const* p_sysName,
	LSA_UINT16 sysName_length,
	LSA_UINT8 const* p_sysDescr,
	LSA_UINT16 sysDescr_length,
	LSA_UINT8 const* p_sysContact,
	LSA_UINT16 sysContact_length,
	LSA_UINT8 const* p_sysLocation,
	LSA_UINT16 sysLocation_length
)
{
	TCIP_LOG_FCT("tcip_set_mib2_sysgroup_data")

	if ( is_not_null(p_sysName) && sysName_length > TCIP_MAX_MIBII_IFSTRING_LEN )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysName string longer than 255 characters (%d)", sysName_length);
		return TCIP_ERR_PARAM;
	}

	if ( is_not_null(p_sysDescr) && sysDescr_length > TCIP_MAX_MIBII_IFSTRING_LEN )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysDescr string longer than 255 characters (%d)", sysDescr_length);
		return TCIP_ERR_PARAM;
	}

	if ( is_not_null(p_sysContact) && sysContact_length > TCIP_MAX_MIBII_IFSTRING_LEN )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysContact string longer than 255 characters (%d)", sysContact_length);
		return TCIP_ERR_PARAM;
	}

	if ( is_not_null(p_sysLocation) && sysLocation_length > TCIP_MAX_MIBII_IFSTRING_LEN )
	{
		TCIP_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "passed sysLocation string longer than 255 characters (%d)", sysLocation_length);
		return TCIP_ERR_PARAM;
	}

	/***/

#if TCIP_CFG_INTERNICHE
	if ( is_not_null(p_sysName) )
	{
		TCIP_MEMCPY(sysName, (void*)p_sysName, sysName_length );
		sysName[sysName_length] = 0;
	}

	if ( is_not_null(p_sysDescr) )
	{
		TCIP_MEMCPY(sys_descr, (void*)p_sysDescr, sysDescr_length );
		sys_descr[sysDescr_length] = 0;
	}

	if ( is_not_null(p_sysContact) )
	{
		TCIP_MEMCPY(sysContact, (void*)p_sysContact, sysContact_length );
		sysContact[sysContact_length] = 0;
	}

	if ( is_not_null(p_sysLocation) )
	{
		TCIP_MEMCPY(sysLocation, (void*)p_sysLocation, sysLocation_length );
		sysLocation[sysLocation_length] = 0;
	}
#endif

#if TCIP_CFG_OPEN_BSD && TCIP_CFG_SNMP_ON
	if ( is_not_null(p_sysName)) /* sado -- omit comparison of length >= 0; Greenhills compiler warning - useless comparision of unsigned integer */
	{
		MIBAL_set_mib2_sysName((const char *)p_sysName, sysName_length);
	}

	if ( is_not_null(p_sysDescr))
	{
		MIBAL_set_mib2_sysDescr((const char *)p_sysDescr, sysDescr_length);
	}

	if ( is_not_null(p_sysContact))
	{
		MIBAL_set_mib2_sysContact((const char *)p_sysContact, sysContact_length);
	}

	if ( is_not_null(p_sysLocation))
	{
		MIBAL_set_mib2_sysLocation((const char *)p_sysLocation, sysLocation_length);
	}
#endif

	return TCIP_OK;
}


/*===========================================================================*/
/*===========================================================================*/
#if TCIP_CFG_INTERNICHE

void tcip_iniche_mib2_written(const void * snmp_data)
{
	if( snmp_data != 0 )
	{
		if( (char*)snmp_data == sys_descr )
		{
			/* unreachable, not writable via SNMP */
		}
		else if( (char*)snmp_data == sysName )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(sysName, LSA_NULL, LSA_NULL);
		}
		else if( (char*)snmp_data == sysContact )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(LSA_NULL, sysContact, LSA_NULL);
		}
		else if( (char*)snmp_data == sysLocation )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(LSA_NULL, LSA_NULL, sysLocation);
		}
		else
		{
			/* do nothing */
		}
	}
}
#endif

#if TCIP_CFG_OPEN_BSD && TCIP_CFG_SNMP_ON
void tcip_obsd_mib2_written(const void * snmp_data)
{
	if( snmp_data != 0 )
	{
		if( (char*)snmp_data == MIBAL_get_mib2_sysDescr() )
		{
			/* unreachable, not writable via SNMP */
		}
		else if( (char*)snmp_data == MIBAL_get_mib2_sysName() )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(MIBAL_get_mib2_sysName(), LSA_NULL, LSA_NULL);
		}
		else if( (char*)snmp_data == MIBAL_get_mib2_sysContact() )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(LSA_NULL, MIBAL_get_mib2_sysContact(), LSA_NULL);
		}
		else if( (char*)snmp_data == MIBAL_get_mib2_sysLocation() )
		{
			TCIP_MIB2_SYSGROUP_DATA_WRITTEN(LSA_NULL, LSA_NULL, MIBAL_get_mib2_sysLocation());
		}
		else
		{
			/* do nothing */
		}
	}
}
#endif

/*===========================================================================*/
/*===========================================================================*/
#if TCIP_CFG_SNMP_ON
LSA_UINT8
tcip_mib2_statistics_update(
	LSA_VOID
) {
	TCIP_LOG_FCT("tcip_mib2_statistics_update")

	if( tcip_data.get_statistics_pending != 0 )
	{
		TCIP_FATAL();
	}
	else
	{
		int i;

		for( i=0; i<TCIP_MAX_CHANNELS; i++ )
		{
			TCIP_CHANNEL_PTR_TYPE channel = &tcip_data.tcip_channels[i];

			if( channel->state == TCIP_CH_OPEN )
			{
				if( channel->detailptr->path_type == TCIP_PATH_TYPE_ARP )
				{
					if( channel->edd.get_statistics_pending )
					{
						TCIP_FATAL();
					}
					else
					{
						TCIP_IF_DATA_PTR_TYPE p_if_data = channel->p_if_data;
						LSA_UINT16 port_id;
						TCIP_EDD_LOWER_RQB_PTR_TYPE pEdd;
						EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE pStat;

						pEdd = channel->edd.pEdd;

						pStat = TCIP_EDD_GET_PPARAM(pEdd, EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE);

						pStat->Reset = EDD_STATISTICS_DO_NOT_RESET_VALUES; /* AP00913629 do not reset counters */
						for (port_id = 0; port_id <= p_if_data->port_count; port_id++) /* RQ 1916135 -- new EDD Interface */
						{
							pStat->sMIB[port_id].RequestedCounters = 0;
						}

						TCIP_EDD_SET_OPCODE(pEdd, EDD_OPC_REQUEST, EDD_SRV_GET_STATISTICS_ALL );
						TCIP_EDD_SET_HANDLE(pEdd, 0);
						EDD_RQB_SET_HANDLE_LOWER(pEdd, channel->edd.handle_lower);

						TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
							">>> EDD_SRV_GET_STATISTICS (EDDHandle: 0x%X,pEdd: 0x%X)"
							, TCIP_EDD_GET_HANDLE(pEdd), pEdd
							);

						TCIP_EDD_REQUEST_LOWER(pEdd, channel->sysptr);

						/***/

						channel->edd.get_statistics_pending = LSA_TRUE;

						tcip_data.get_statistics_pending ++;
					}
				}
			}
		}
	}

	/***/

	if( tcip_data.get_statistics_pending != 0 )
	{
		return TCIP_PENDING_YES;
	}

	return TCIP_PENDING_NO;
}
#endif
/*===========================================================================*/
#if TCIP_CFG_SNMP_ON
LSA_VOID
tcip_edd_get_mib2_statistics_done(
	TCIP_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_CONST_PTR_TYPE pRQB
) {
	if(
		TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK
		|| ! channel->edd.get_statistics_pending )
	{
		TCIP_FATAL();
	}
	else
	{
		TCIP_IF_DATA_PTR_TYPE p_if_data = channel->p_if_data;
		EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE pStat = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_GET_STATISTICS_ALL_PTR_TYPE);
		LSA_UINT16 port_id;

		for( port_id = 0/*!*/; port_id <= p_if_data->port_count; port_id ++ )
		{
#if TCIP_CFG_INTERNICHE
			NET ifp = p_if_data->port[port_id].ifp;
			EDD_UPPER_GET_STATISTICS_MIB_PTR_TYPE pVal = &pStat->sMIB[port_id];

			ifp->mib.ifInOctets        = pVal->InOctets;
			ifp->mib.ifInUcastPkts     = pVal->InUcastPkts;
			ifp->mib.ifInNUcastPkts    = pVal->InNUcastPkts;
			ifp->mib.ifInDiscards      = pVal->InDiscards;
			ifp->mib.ifInErrors        = pVal->InErrors;
			ifp->mib.ifInUnknownProtos = pVal->InUnknownProtos;

			ifp->mib.ifOutOctets     = pVal->OutOctets;
			ifp->mib.ifOutUcastPkts  = pVal->OutUcastPkts;
			ifp->mib.ifOutNUcastPkts = pVal->OutNUcastPkts;
			ifp->mib.ifOutDiscards   = pVal->OutDiscards;
			ifp->mib.ifOutErrors     = pVal->OutErrors;
			ifp->mib.ifOutQLen       = pVal->OutQLen;

			ifp->mib.SupportedCounters = pVal->SupportedCounters; /* see Request ID 216015 */

#endif

#if TCIP_CFG_OPEN_BSD
			OBSD_IFNET ifp = p_if_data->port[port_id].ifp;
			EDD_UPPER_GET_STATISTICS_MIB_PTR_TYPE pVal = &pStat->sMIB[port_id];

			obsd_pnio_set_statistic_counters(ifp,
									(long)pVal->InOctets,         /* 10: EDD_MIB_SUPPORT_INOCTETS */
									(long)pVal->InUcastPkts,      /* 11: EDD_MIB_SUPPORT_INUCASTPKTS */
									(long)pVal->InNUcastPkts,     /* 12: EDD_MIB_SUPPORT_INNUCASTPKTS */
									(long)pVal->InDiscards,       /* 13: EDD_MIB_SUPPORT_INDISCARDS */
									(long)pVal->InErrors,         /* 14: EDD_MIB_SUPPORT_INERRORS */
									(long)pVal->InUnknownProtos,  /* 15: EDD_MIB_SUPPORT_INUNKNOWNPROTOS */
									(long)pVal->OutOctets,        /* 16: EDD_MIB_SUPPORT_OUTOCTETS */
									(long)pVal->OutUcastPkts,     /* 17: EDD_MIB_SUPPORT_OUTUCASTPKTS */
									(long)pVal->OutNUcastPkts,    /* 18: EDD_MIB_SUPPORT_OUTNUCASTPKTS */
									(long)pVal->OutDiscards,      /* 19: EDD_MIB_SUPPORT_OUTDISCARDS */
									(long)pVal->OutErrors,        /* 20: EDD_MIB_SUPPORT_OUTERRORS */
									(long)pVal->OutQLen,          /* 21: EDD_MIB_SUPPORT_OUTQLEN */
									(unsigned long)pVal->SupportedCounters);
#endif
		}

		/***/
		channel->edd.get_statistics_pending = LSA_FALSE;

		/*
		 *  close channel now
		 */
		if( channel->state == TCIP_CH_CLOSING )
		{
			tcip_edd_close_channel(channel);
		}

		if( tcip_data.get_statistics_pending < 1 )
		{
			TCIP_FATAL(); /* a bug */
		}
		else
		{
			if (tcip_data.get_statistics_pending > 0)
			{
				tcip_data.get_statistics_pending --;
			}
			else
			{
				TCIP_FATAL();
			}

			if( tcip_data.get_statistics_pending == 0 )
			{
				TCIP_MIB2_STATISTICS_UPDATE_DONE();
			}
		}
	}
}
#endif
/*===========================================================================*/
/*===========================================================================*/

LSA_VOID
tcip_edd_ext_link_status_provide(
	TCIP_CONST_CHANNEL_PTR_TYPE channel
) {
	TCIP_LOG_FCT("tcip_edd_ext_link_status_provide")
	LSA_UINT16 port_id;

	for ( port_id = 1; port_id <= channel->p_if_data->port_count; port_id++ )
	{
		TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB;
		EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pLink;

		pRQB = tcip_AllocRQB(channel->sysptr, sizeof(EDD_RQB_LINK_STATUS_IND_PROVIDE_TYPE));

		if ( is_null(pRQB) )
		{
			TCIP_FATAL(); /* a bug */
		}

		pLink = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE);

		TCIP_MEMSET(pLink, 0, sizeof(*pLink)); /* note: actually not necessery */

		pLink->PortID = port_id;
		pLink->Status = EDD_LINK_UNKNOWN; /* force an indication (the status in EDD is either UP or DOWN, never unknown) */
		pLink->PhyStatus = EDD_PHY_STATUS_UNKNOWN;

		TCIP_EDD_SET_OPCODE(pRQB, EDD_OPC_REQUEST, EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT);
		TCIP_EDD_SET_HANDLE(pRQB, 0);
		EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

		TCIP_UPPER_TRACE_02(0, LSA_TRACE_LEVEL_NOTE,
			">>> EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT (EDDHandle: 0x%X,pRQB: 0x%X)"
			, TCIP_EDD_GET_HANDLE(pRQB), pRQB
			);

		TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
	}
}

/*===========================================================================*/

LSA_VOID
tcip_edd_ext_link_status_indication(
	TCIP_CONST_CHANNEL_PTR_TYPE channel,
	TCIP_EDD_LOWER_RQB_PTR_TYPE pRQB
) {
	TCIP_LOG_FCT("tcip_edd_ext_link_status_indication")
	EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE pLink = TCIP_EDD_GET_PPARAM(pRQB, EDD_UPPER_LINK_STATUS_IND_PROVIDE_PTR_TYPE);

	TCIP_UPPER_TRACE_05(channel->trace_idx, LSA_TRACE_LEVEL_NOTE,
		">>> pRQB: 0x%X, PortID: %u, St/Sp/M: %u/%u/%u"
		, pRQB, pLink->PortID
		, pLink->Status, pLink->Speed, pLink->Mode
		);

	if( channel->state != TCIP_CH_OPEN )
	{
		tcip_FreeRQB(channel->sysptr, pRQB );
	}
	else if( TCIP_EDD_GET_RESPONSE(pRQB) != EDD_STS_OK )
	{
		TCIP_FATAL();
	}
	else if( pLink->PortID < 1 || pLink->PortID > channel->p_if_data->port_count )
	{
		TCIP_FATAL();
	}
	else /* ok */
	{
#if TCIP_CFG_INTERNICHE
		NET ifp = channel->p_if_data->port[pLink->PortID].ifp;
		u_long ifOperStatusOld = ifp->mib.ifOperStatus;

		if (pLink->PhyStatus == EDD_PHY_STATUS_DISABLED) /* evaluate PhyStatus */
		{
			ifp->mib.ifAdminStatus = NI_DOWN;
			ifp->mib.ifOperStatus = NI_DOWN;
		}
		else
		{
			ifp->mib.ifAdminStatus = NI_UP; /* see 1199575 */

			switch (pLink->Status)
			{
			case EDD_LINK_UP:
			case EDD_LINK_UP_CLOSED:
			case EDD_LINK_UP_DISABLED:

				ifp->mib.ifOperStatus = NI_UP;

				switch (pLink->Speed)
				{
				case EDD_LINK_SPEED_10:
					ifp->mib.ifSpeed = 10000000;
					break;
				case EDD_LINK_SPEED_100:
					ifp->mib.ifSpeed = 100000000;
					break;
				case EDD_LINK_SPEED_1000:
					ifp->mib.ifSpeed = 1000000000;
					break;
				case EDD_LINK_SPEED_10000: //TODO: was soll man hier melden, ifHighSpeed gibt es nicht
					ifp->mib.ifSpeed = 0xFFFFFFFF; /* MIB: use ifHighSpeed */
					break;
				default:
					ifp->mib.ifSpeed = 0;
					break;
				}
				break;

			default: /* EDD_LINK_DOWN */

				ifp->mib.ifOperStatus = NI_DOWN;

				ifp->mib.ifSpeed = channel->p_if_data->port[pLink->PortID].nominal_bandwidth;
				break;
			}
		}
		if( ifp->mib.ifOperStatus != ifOperStatusOld  ) /* AP00958944: update ifLastChange only if ifOperStatus changes */
		{
			ifp->n_mib->ifLastChange = cticks * (100/TPS);
		}
#endif

#if TCIP_CFG_OPEN_BSD
		{
			OBSD_IFNET ifp = channel->p_if_data->port[pLink->PortID].ifp;

			if (pLink->PhyStatus == EDD_PHY_STATUS_DISABLED) /* evaluate PhyStatus */
			{
				obsd_netif_set_admin_state(obsd_pnio_get_if_name(ifp), 0); /* DOWN, see 1199575 */
				obsd_pnio_set_link_down(ifp);
			}
			else
			{
				switch (pLink->Status)
				{
				case EDD_LINK_UP:
				case EDD_LINK_UP_CLOSED:
				case EDD_LINK_UP_DISABLED:

					obsd_pnio_set_link_up(ifp);

					switch (pLink->Speed)
					{
					case EDD_LINK_SPEED_10:
						obsd_pnio_set_baudrate(ifp,10000000);
						break;
					case EDD_LINK_SPEED_100:
						obsd_pnio_set_baudrate(ifp, 100000000);
						break;
					case EDD_LINK_SPEED_1000:
						obsd_pnio_set_baudrate(ifp, 1000000000);
						break;
					case EDD_LINK_SPEED_10000: //TODO: was soll man hier melden, ifHighSpeed gibt es nicht
						obsd_pnio_set_baudrate(ifp, 0xFFFFFFFF); /* MIB: use ifHighSpeed */
						break;
					default:
						obsd_pnio_set_baudrate(ifp, 0);
						break;
					}
					break;

				default: /* EDD_LINK_DOWN */
					obsd_pnio_set_link_down(ifp);

					obsd_pnio_set_baudrate(ifp, (long)channel->p_if_data->port[pLink->PortID].nominal_bandwidth);
					break;
				}

				obsd_netif_set_admin_state(obsd_pnio_get_if_name(ifp), 1); /* UP, see 1199575 */
			}
		}
#endif

		/* reprovide */

		TCIP_EDD_SET_HANDLE(pRQB, 0);
		EDD_RQB_SET_HANDLE_LOWER(pRQB, channel->edd.handle_lower);

		TCIP_EDD_REQUEST_LOWER(pRQB, channel->sysptr);
	}
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/


#define LTRC_ACT_MODUL_ID 4036 /* OBSD_ITGR added for LSA traces */

#define _KERNEL

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_timeout.h>
#include <sys/obsd_kernel_proc.h>
#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_if_types.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_if_ether.h>
#include <netinet/obsd_kernel_ip_var.h>
#include <netinet/obsd_kernel_ip_carp.h>
#include <obsd_kernel_if_simple.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_string.h>
#include <obsd_userland_ctype.h>
#include <obsd_kernel_usr.h>

#include <tcip_obsd_trc.h>

static struct
{
	OBSD_IFNET    ifp;
	unsigned int  interface_id;
	unsigned long cluster_ip_addr;
	unsigned long cluster_ip_subnet_mask;
	unsigned char vhid;
} carp_parameter;

static struct
{
	unsigned char request_in_progress;
	unsigned char undo_in_progress;
	unsigned int  error;
	unsigned char mode;
} carp_status;

char *obsd_pnio_get_carp_name(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	struct carp_if *my_carp;
	struct carp_softc* my_cif;
	char *if_name = NULL;

	if (my_ifp)
	{
		my_carp = (struct carp_if*) my_ifp->if_carp;
		if (my_carp)
		{
			my_cif = TAILQ_FIRST(&my_carp->vhif_vrs); /* --> first structure */
			if (my_cif)
			{
				if_name = my_cif->sc_if.if_xname;
			}
		}
	}

	return if_name;
}

OBSD_IFNET obsd_pnio_get_carp_if(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	struct carp_if *my_carp;
	struct carp_softc* my_cif;

	if (my_ifp)
	{
		my_carp = (struct carp_if*) my_ifp->if_carp;
		if (my_carp)
		{
			my_cif = TAILQ_FIRST(&my_carp->vhif_vrs); /* --> first structure */
			if (my_cif)
			{
				return (OBSD_IFNET)my_cif;
			}
		}
	}

	return NULL;
}

void obsd_pnio_reset_carp_state(void)
{
	memset(&carp_status, 0, sizeof(carp_status));
	carp_status.error = OBSD_PNIO_TCIP_OK;
	memset(&carp_parameter, 0, sizeof(carp_parameter));
}

int obsd_pnio_correct_cluster_params(unsigned int cluster_ip, unsigned int cluster_subnet_mask, unsigned char cluster_id)
{
	if (cluster_id == 0 && cluster_ip == 0 && cluster_subnet_mask == 0) return 1; /* cluster_id 0 only with Zero Record */
	if (cluster_id != 0 && cluster_ip != 0 && cluster_subnet_mask != 0) return 1;
	return 0;
}

int obsd_pnio_set_cluster_ip(OBSD_IFNET ifp, unsigned int interface_id, unsigned int cluster_ip, unsigned int cluster_subnet_mask, unsigned char cluster_id)
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);
	int sync_result = OBSD_PNIO_TCIP_OK;

	if (carp_status.request_in_progress == 1)
	{
		return OBSD_PNIO_ERR_RESOURCE;
	}
	
	if (ifp == NULL || !obsd_pnio_correct_cluster_params(cluster_ip, cluster_subnet_mask, cluster_id))
	{
		return OBSD_PNIO_ERR_PARAM;
	}

	obsd_pnio_reset_carp_state();

	if (carp_name != NULL) /* a carp interface exists */
	{
		if (cluster_id == 0) /* delete existing interface */
		{
			carp_status.mode = 0;
		}
		else /* re-configure existing interface */
		{
			carp_status.mode = 2;
		}
	}
	else /* new interface */
	{
		if (cluster_ip == 0) /* zero parameter */
		{
			return OBSD_PNIO_ERR_PARAM;
		}
		else
		{
			carp_status.mode = 1;
		}
	}

	carp_status.request_in_progress = 1;
	
	carp_parameter.ifp = ifp;
	carp_parameter.interface_id = interface_id;
	carp_parameter.cluster_ip_addr = cluster_ip;
	carp_parameter.cluster_ip_subnet_mask = cluster_subnet_mask;
	carp_parameter.vhid = cluster_id;

	if (carp_status.mode == 1)
	{
		sync_result = obsd_pnio_set_carp_step1(ifp, cluster_id); /* start set process */
	}
	else
	{
		sync_result = obsd_pnio_reset_carp_step1(ifp); /* start reset process*/
		if (obsd_pnio_get_errno() == OBSD_PNIO_EADDRNOTAVAIL) /* if no address available, try to destroy interface anyway */
		{
			sync_result = obsd_pnio_reset_carp_step2(ifp);
		}
	}

	if (sync_result != OBSD_PNIO_TCIP_OK)
	{
		return sync_result;
	}
	/* else wait for asynchronous operations to complete --> statemachine CARP_MULTICAST_DONE,  CARP_FILTER_DONE --> carp_reply() */

	return OBSD_PNIO_TCIP_OK;
}

void carp_reply(int result)
{
	int response = OBSD_PNIO_TCIP_OK;
	int interface_id = carp_parameter.interface_id;
	if (carp_status.undo_in_progress == 1) response = carp_status.error; /* if stored error (after undo) */
	else response = result;
	obsd_pnio_reset_carp_state();
	TCIP_SRV_CLUSTER_IP_CONFIG_SET_DONE(interface_id, response); /* first error occured */
}

void carp_undo_set(int result)
{
	int sync_result = OBSD_PNIO_TCIP_OK;
	carp_status.mode = 0;
	carp_status.undo_in_progress = 1;
	carp_status.error = result; /* store error for callback */
	sync_result = obsd_pnio_reset_carp_step1(carp_parameter.ifp); /* start reset process */
	if (obsd_pnio_get_errno() == OBSD_PNIO_EADDRNOTAVAIL) /* if no address available, try to destroy interface anyway */
	{
		sync_result = obsd_pnio_reset_carp_step2(carp_parameter.ifp);
	}

	if (sync_result != OBSD_PNIO_TCIP_OK) carp_reply(sync_result);
	/* else wait for asynchronous operations to complete --> statemachine CARP_MULTICAST_DONE,  CARP_FILTER_DONE --> carp_reply() */
}

void obsd_pnio_change_carp_state(int event, int result)
{
	int sync_result = OBSD_PNIO_TCIP_OK;

	if (carp_status.mode == 1) /* set */
	{
		switch (event)
		{
		case CARP_MULTICAST_DONE: /* comes in asynchronously */
			if (result == OBSD_PNIO_TCIP_OK) /* trigger ip address setting */
			{
				sync_result = obsd_pnio_set_carp_step2(carp_parameter.ifp, carp_parameter.cluster_ip_addr, carp_parameter.cluster_ip_subnet_mask);
				if (sync_result != OBSD_PNIO_TCIP_OK)
				{
					carp_undo_set(sync_result); /* store error, trigger reset statemachine */
				}
			}
			else
			{
				carp_undo_set(result); /* store error, trigger reset statemachine */
			}
			break;
		case CARP_FILTER_DONE: /* comes in asynchronously */
			if (result == OBSD_PNIO_TCIP_OK)
			{
				carp_reply(result);
			}
			else
			{
				carp_undo_set(result); /* store error, trigger reset statemachine */
			}
			break;
		default: break;
		}
	}
	else /* reset or re-configure */
	{
		switch (event)
		{
		case CARP_FILTER_DONE: /* comes in asynchronously */
			if (result == OBSD_PNIO_TCIP_OK) /* trigger destroying of interface */
			{
				sync_result = obsd_pnio_reset_carp_step2(carp_parameter.ifp);
				if (sync_result != OBSD_PNIO_TCIP_OK)
				{
					carp_reply(sync_result);
				}
			}
			else
			{
				carp_reply(result);
			}
			break;
		case CARP_MULTICAST_DONE: /* comes in asynchronously */
			if (carp_status.mode == 0)
			{
				carp_reply(result);
			}
			else /* mode == 2 --> re-configure */
			{
				carp_status.mode = 1; /* after reset, trigger set statemachine with new parameters; */
				result = obsd_pnio_set_carp_step1(carp_parameter.ifp, carp_parameter.vhid);
				
				if (result != OBSD_PNIO_TCIP_OK) carp_reply(result);
				/* else wait for asynchronous operations to complete --> statemachine CARP_MULTICAST_DONE,  CARP_FILTER_DONE --> carp_reply() */
			}
			break;
		default: break;
		}
	}
}

int obsd_pnio_create_interface(void)
{
	void *tmp = malloc(sizeof(struct obsd_softc));
	struct obsd_softc* sc=  (struct obsd_softc*)tmp;
	char *ifname;
		
	if(sc == NULL)
	{
		return -1;
	}
	else
	{
		memset(sc, 0, sizeof(*sc));
		sc->tcip_channel = 0; /* free */

		if (obsd_if_attach(sc) != 0)
		{
			return -1;
		}

		if (obsd_if_init(&sc->sc_arpcom.ac_if) != 0)
		{
			return -1;
		}

		ifname = obsd_pnio_get_if_name((OBSD_IFNET)&sc->sc_arpcom.ac_if);

		if (obsd_netif_set_admin_state(ifname, 0) != 0)
		{
			return -1;
		}

		obsd_pnio_set_link_down((OBSD_IFNET)&sc->sc_arpcom.ac_if);
	}

	return 0;
}

OBSD_IFNET obsd_pnio_find_free_if_and_activate(void *arp_channel, int *MacAddress, int ifAdminStatus /*0="DOWN", 1="UP"*/)
{
	struct ifnet* ifp;
	TAILQ_FOREACH(ifp, &ifnet, if_list) /* see loop in ifunit() */
	{
		if( ifp->if_start == obsd_if_start ) /* is a TCIP interface */
		{
			struct obsd_softc* sc = (struct obsd_softc*)ifp->if_softc;

			if( sc->tcip_channel == 0 ) /* free */
			{
				sc->tcip_channel = arp_channel; /* used */
				memcpy(&sc->sc_arpcom.ac_enaddr, MacAddress, sizeof(sc->sc_arpcom.ac_enaddr));
				bcopy((caddr_t)((struct arpcom *)ifp)->ac_enaddr, LLADDR(ifp->if_sadl), ifp->if_addrlen);
				obsd_netif_set_admin_state(ifp->if_xname, ifAdminStatus);
				break; /* found */
			}
		}
	}

	return (ifp);
}

int obsd_pnio_deactivate_interface(OBSD_IFNET ifp)
{
	struct ifnet* my_ifp = (struct ifnet *)ifp;
	struct obsd_softc* sc = (struct obsd_softc*)my_ifp->if_softc;

	sc->tcip_channel = 0; /* free */

	if (obsd_netif_set_admin_state(my_ifp->if_xname, 0) != 0) /* down */
	{
		return -1;
	}

	obsd_pnio_set_link_down(ifp);

	return 0;
}

int obsd_pnio_delete_all_interfaces(void)
{
	/*
	 *  delete all TCIP interfaces
	 */

	struct ifnet *ifp;
	struct ifnet *ifp_saved;

	tcp_close_timewait_connections();

	TAILQ_FOREACH_SAFE(ifp, &ifnet, if_list, ifp_saved)
    {
		if( ifp->if_start == obsd_if_start)  /* is a TCIP interface */
		{
			struct obsd_softc* sc = (struct obsd_softc*)ifp->if_softc;
			if( sc->tcip_channel != 0 ) /* must be free */
			{
				return -1;
			}

			if (obsd_netif_set_admin_state(ifp->if_xname, 0) != 0) /* mark iface as down */
			{
				return -1;
			}

			obsd_pnio_set_link_down(ifp);

			if (obsd_netif_del_ipv4addr(ifp->if_xname) != 0)
			{
				return -1;
			}

			if (obsd_if_detach(sc) != 0)
			{
				return -1;
			}

			TAILQ_REMOVE(&ifnet, ifp, if_list);
			free(sc);
		}
		else if (ifp->if_type == IFT_CARP)
		{
			obsd_netif_del_ipv4addr(ifp->if_xname);
			obsd_netif_destroy_if(ifp->if_xname);
		}
	}

	return 0;
}

int obsd_pnio_set_carp_role(OBSD_IFNET ifp, unsigned int enable)
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);

	if (carp_name != NULL)
	{
		return obsd_netif_set_carp_role((const char *)carp_name, enable);
	}
	else
	{
		return -1;
	}
}

int obsd_pnio_get_carp_role(OBSD_IFNET ifp)
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);

	if (carp_name != NULL)
	{
		int primary = 0;
		int retval = obsd_netif_get_carp_role((const char *)carp_name, &primary);

		if (retval != -1)
		{
			return primary;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;
	}
}

int obsd_pnio_reset_carp_step1(OBSD_IFNET ifp) /* delete ip address */
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);

	if (carp_name != NULL)
	{
		struct proc *cur_proc = sys_proc_register(NULL, NULL);  /* delete errno, we want to know if an EADDRNOTAVAIL occurs or not */
		if (cur_proc != NULL)
		{
			cur_proc->p_upls.pls_errno = 0;
		}

		if (obsd_netif_del_ipv4addr(carp_name) != 0)
		{
			return OBSD_PNIO_ERR_LOWER_LAYER;
		}
		else
		{
			return OBSD_PNIO_TCIP_OK;
		}
	}
	else
	{
		return OBSD_PNIO_ERR_PARAM;
	}
}

int obsd_pnio_reset_carp_step2(OBSD_IFNET ifp) /* destroy carp interface */
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);

	if (carp_name != NULL)
	{
		if (obsd_netif_destroy_if(carp_name) != 0)
		{
			return OBSD_PNIO_ERR_LOWER_LAYER;
		}
		else
		{
			return OBSD_PNIO_TCIP_OK;
		}
	}
	else
	{
		return OBSD_PNIO_ERR_PARAM;
	}
}

int obsd_pnio_set_carp_step1(OBSD_IFNET ifp, unsigned char vhid) /* new carp interface */
{
	if (ifp != NULL && vhid != 0)
	{
		struct ifnet *my_ifp = (struct ifnet *)ifp;

		char *if_name = my_ifp->if_xname;
		char carp_name[7];
		char ascii_high = (char)(my_ifp->if_index / 10 + 0x30);
		char ascii_low = (char)(my_ifp->if_index % 10 + 0x30);

		strlcpy(carp_name, "carp", sizeof(carp_name));

		if (ascii_high != '0')
		{
			carp_name[4] = ascii_high;
			carp_name[5] = ascii_low;
			carp_name[6] = '\0';
		}
		else
		{
			carp_name[4] = ascii_low;
			carp_name[5] = '\0';
		}

		if (obsd_netif_create_carpif(carp_name, vhid, if_name) != 0)
		{
			return OBSD_PNIO_ERR_LOWER_LAYER;
		}
		else
		{
			OBSD_IFNET carp_if = obsd_pnio_get_carp_if(my_ifp);
			if (carp_if)
			{
				char *carpDescr = obsd_pnio_generate_carp_ifDescr(my_ifp->if_description, strlen(my_ifp->if_description));
				if (carpDescr)
				{
					obsd_pnio_set_if_description(carp_if, carpDescr, strlen(carpDescr));
					free(carpDescr);
				}
			}
			return OBSD_PNIO_TCIP_OK;
		}
	}
	else
	{
		return OBSD_PNIO_ERR_PARAM;
	}
}

int obsd_pnio_set_carp_step2(OBSD_IFNET ifp, unsigned long cluster_ip_address, unsigned long cluster_ip_subnet_mask) /* set cluster ip address */
{
	char *carp_name = obsd_pnio_get_carp_name(ifp);

	if (obsd_netif_set_ipv4addr(carp_name, cluster_ip_address, cluster_ip_subnet_mask) != 0)
	{
		return OBSD_PNIO_ERR_LOWER_LAYER;
	}
	else
	{
		return OBSD_PNIO_TCIP_OK;
	}
}

void *obsd_pnio_get_tcip_channel(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	struct obsd_softc* sc = (struct obsd_softc*)my_ifp->if_softc;
	return sc->tcip_channel;
}

int obsd_pnio_get_if_index(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	return my_ifp->if_index;
}

char *obsd_pnio_get_if_name(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	return my_ifp->if_xname;
}

char *obsd_pnio_get_if_description(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	return my_ifp->if_description;
}

void obsd_pnio_set_if_description(OBSD_IFNET ifp, char *ifDescr, int ifDescr_len)
{
	int ifDescr_length = ifDescr_len;
	struct ifnet *my_ifp = (struct ifnet *)ifp;

	if (my_ifp->if_type == IFT_ETHER)
	{
		OBSD_IFNET carp_if = obsd_pnio_get_carp_if(ifp);

		if (carp_if)
		{
			char *carpDescr = obsd_pnio_generate_carp_ifDescr(ifDescr, ifDescr_len);
			if (carpDescr)
			{
				obsd_pnio_set_if_description(carp_if, carpDescr, strlen(carpDescr));
				free(carpDescr);
			}
		}
	}

	memcpy(my_ifp->if_description, ifDescr, ifDescr_length);

	if (ifDescr_length == IFDESCRSIZE)
	{
		ifDescr_length--; /* note: for OpenBSD, length of ifDescr is 64 including terminating 0 */
	}
			
	my_ifp->if_description[ifDescr_length] = 0;

	MIBAL_set_mib2_ifdescr(my_ifp->if_index, my_ifp->if_description);
}

void obsd_pnio_set_statistic_counters(OBSD_IFNET ifp, 
								long inOctets, 
								long inUcastPkts, 
								long inNUcastPkts, 
								long inDiscards, 
								long inErrors, 
								long inUnknownProtos, 
								long outOctets, 
								long outUcastPkts, 
								long outNUcastPkts, 
								long outDiscards, 
								long outErrors, 
								long outQLen, 
								unsigned long supportedCounters)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;

	my_ifp->if_data.ifi_ibytes = inOctets;			/* 10: EDD_MIB_SUPPORT_INOCTETS */							
	my_ifp->if_data.ifi_ipackets = inUcastPkts;		/* 11: EDD_MIB_SUPPORT_INUCASTPKTS */
	my_ifp->if_data.ifi_imcasts  = inNUcastPkts;	/* 12: EDD_MIB_SUPPORT_INNUCASTPKTS */
	my_ifp->if_data.ifi_iqdrops  = inDiscards;		/* 13: EDD_MIB_SUPPORT_INDISCARDS */
	my_ifp->if_data.ifi_ierrors  = inErrors;		/* 14: EDD_MIB_SUPPORT_INERRORS */
	my_ifp->if_data.ifi_noproto  = inUnknownProtos;	/* 15: EDD_MIB_SUPPORT_INUNKNOWNPROTOS */
	my_ifp->if_data.ifi_obytes   = outOctets;		/* 16: EDD_MIB_SUPPORT_OUTOCTETS */
	my_ifp->if_data.ifi_opackets = outUcastPkts;     /* 17: EDD_MIB_SUPPORT_OUTUCASTPKTS */
	my_ifp->if_data.ifi_omcasts  = outNUcastPkts;    /* 18: EDD_MIB_SUPPORT_OUTNUCASTPKTS */
	my_ifp->if_data.ifOutDiscards= outDiscards;      /* 19: EDD_MIB_SUPPORT_OUTDISCARDS */
	my_ifp->if_data.ifi_oerrors  = outErrors;        /* 20: EDD_MIB_SUPPORT_OUTERRORS */
	my_ifp->if_data.ifOutQLen    = outQLen;          /* 21: EDD_MIB_SUPPORT_OUTQLEN */
	my_ifp->if_data.SupportedCounters = supportedCounters;
}

void obsd_pnio_set_link_up(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	my_ifp->if_data.ifi_link_state = LINK_STATE_UP;
	microtime(&my_ifp->if_lastchange);
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_set_link_up: interface %u", obsd_pnio_get_if_index((char *)ifp));
}

void obsd_pnio_set_link_down(OBSD_IFNET ifp)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	my_ifp->if_data.ifi_link_state = LINK_STATE_DOWN;	
	microtime(&my_ifp->if_lastchange);
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_set_link_down: interface %u", obsd_pnio_get_if_index((char *)ifp));
}

void obsd_pnio_set_baudrate(OBSD_IFNET ifp, long baudrate)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	my_ifp->if_data.ifi_baudrate = baudrate;

	/* CARP SNMP */
	{
		char *carp_name = obsd_pnio_get_carp_name(my_ifp);
		if (carp_name)
		{
			struct ifnet *my_carp = ifunit(carp_name);
			if (my_carp) my_carp->if_data.ifi_baudrate = baudrate;
		}
	}
}

char *obsd_pnio_generate_carp_ifDescr(char *ifDescr, int length)
{
	int found = 0;
	int index =  0;
	char *result = NULL;

	if ((result = malloc(length+1)) == NULL)
		return NULL;

	for (index = 0; index < length; index++)
	{
		char letter = ifDescr[index];

		if (letter != 'X')
		{
			result[index] = letter;
		}
		else
		{
			if (index == length - 1)
			{
				result[index] = letter;
			}
			else if (isdigit(ifDescr[index + 1]))
			{
				result[index] = 'V';
				found = 1;
			}
			else
			{
				result[index] = letter;
			}
		}
	}

	if (found)
	{
		result[length] = 0;
		return result;
	}
	else
	{
		free(result);
		return NULL;
	}
}

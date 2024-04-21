/*	generic packet capture lib handling via netal layer */
/* for examples see if_ie.c, if_re_pci.c/re.c, if_ral_pci.c/rt2560.c/rt2661.c/rt2860.c and others    */ 

/* for some details of BSD's device driver interface see:
   - netbsd.gw.com/cgi-bin/man-cgi?driver+9+NetBSD-5.1+i386
   - a simple driver like puc.c
   - www.netbsd.org/docs/kernel/programming.html
 */

/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */

#define LTRC_ACT_MODUL_ID 4035 /* OBSD_ITGR added for LSA traces */

#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>    /* gh2289n: bcopy */
#include <sys/obsd_kernel_sockio.h>
#include <sys/obsd_kernel_mbuf.h>  /* m_clsetwms */
#include <sys/obsd_kernel_timeout.h> /* timeout_set */
#include <sys/obsd_kernel_socket.h>

#include <net/obsd_kernel_if.h>

#ifdef INET
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_in_var.h>
#include <netinet/obsd_kernel_if_ether.h>
#endif

#include "obsd_kernel_bpfilter.h"
#include "obsd_kernel_usr.h"
#include "obsd_kernel_if_simple.h"

#include <tcip_obsd_trc.h>

#define TX_QLEN		4 /* 64 */

#define NEXT_RX_DESC(sc, x)	(((x) + 1) % RX_DESC_CNT)
#define NEXT_TXQ(sc, x)		(((x) + 1) % TX_QLEN)

#define RX_DESC_CNT          4


/* bus independent (device level) operations */
void    obsd_if_tick(void *xsc);   /* non static to be known within DDB */

/* ------------------------ device level ---------------------------------- */

static int  obsd_if_ioctl(struct ifnet *, u_long, caddr_t);
static int	obsd_if_ll_filter(struct ifnet *, caddr_t, u_int8_t);
static int  obsd_if_ll_mac(struct ifnet *, caddr_t, int);
static void obsd_if_stop(struct ifnet *);

/* ------------------------ device level ---------------------------------- */
/*
 * Attach the interface. Allocate softc structures, do ifmedia
 * setup and ethernet/BPF attach.
 */
int obsd_if_attach( struct obsd_softc *sc)
{
	struct ifnet *ifp;
	int             error = 0;
		
	ifp = &sc->sc_arpcom.ac_if;
	ifp->if_softc   = sc;
	ifp->if_start	= obsd_if_start;
	ifp->if_ioctl 	= obsd_if_ioctl;
	ifp->if_watchdog= NULL;                  /* if_simple_watchdog; currently not used */
	ifp->if_timer   = 0;                     /* currently don't use the watchdog */
	ifp->if_flags   = IFF_BROADCAST | IFF_SIMPLEX | IFF_MULTICAST;
	/*IFQ_SET_MAXLEN(&ifp->if_snd, RL_TX_QLEN, 0);*/	/* hm Queue with watermark check */
	IFQ_SET_READY(&ifp->if_snd);
	m_clsetwms(ifp, MCLBYTES, 2, RX_DESC_CNT);

	/*ifp->if_capabilities = IFCAP_VLAN_MTU | IFCAP_CSUM_IPv4 |IFCAP_CSUM_TCPv4 | IFCAP_CSUM_UDPv4; */
	ifp->if_capabilities = 0;

	timeout_set(&sc->timer_handle, obsd_if_tick, sc);

	/* Attach the interface. */
	if_attach(ifp);

	/* OBSD_ITGR -- set name, "eth + index" ... this has to be done after if_attach where the index is set */
	if (ifp->if_index >= 100 )
	{
		return -1;
	}
	else 
	{
		char xname[6];
		char ascii_high = (char)(ifp->if_index/10 + 0x30);
		char ascii_low = (char)(ifp->if_index%10 + 0x30);

		strlcpy(xname, "eth", sizeof(xname));
		if (ascii_high != '0')
		{
			xname[3] = ascii_high;
			xname[4] = ascii_low;
			xname[5] = '\0';
			TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_CHAT, "obsd_if_attach: new name of interface is %c%c%c%c%c", xname[0], xname[1], xname[2], xname[3], xname[4]);
		}
		else
		{
			xname[3] = ascii_low;
			xname[4] = '\0';
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "obsd_if_attach: new name of interface is %c%c%c%c", xname[0], xname[1], xname[2], xname[3]);
		}

		strlcpy(ifp->if_xname, xname, sizeof(ifp->if_xname));
	}

	ether_ifattach(ifp);
	if (ifp->if_type == 0)
	{
		if_detach(ifp);
		sc->if_is_attached = 0;
		error = ENOMEM;
	}
	else
		sc->if_is_attached = 1;

	return error; /* 0 == no error */
}


void obsd_if_tick(void *xsc)
{
	struct obsd_softc	*sc = (struct obsd_softc *)xsc;
	struct ifnet	*ifp;

	ifp = &sc->sc_arpcom.ac_if;

	if (!IFQ_IS_EMPTY(&ifp->if_snd))
		obsd_if_start(ifp);

	timeout_add_sec(&sc->timer_handle, 1);
}
#if OBSD_PNIO_CFG_TRACE_ON
	void controller_add_obsd_send_measure_6();
#endif /* OBSD_PNIO_CFG_TRACE_ON */


/* Task 1560385 (use if_sendq) -- OpenBSD routines */
void obsd_if_start(struct ifnet *ifp)
{
	void *send_rqb = NULL;

#if OBSD_PNIO_CFG_TRACE_ON
	controller_add_obsd_send_measure_6();
#endif

	if ((ifp->if_flags & (IFF_RUNNING | IFF_OACTIVE)) != IFF_RUNNING)  /* running but not active */
	{
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "obsd_if_start: interface is not active, no send operation");
		return;
	}

	send_rqb = tcip_obsd_get_rqb((OBSD_IFNET)ifp);

	if (send_rqb)
	{
		obsd_send_next_packet((OBSD_IFNET)ifp, send_rqb);
	}
	/* else: do nothing; if sendQ is not empty, sending the packet is triggered either by a returning RQB or obsd_if_tick */
}

void obsd_send_next_packet(OBSD_IFNET ifp, void *pRQB)
{
	struct ifnet *my_ifp = (struct ifnet *)ifp;
	void *channel = ((struct obsd_softc *)(my_ifp->if_softc))->tcip_channel;
	struct mbuf *m;

	IFQ_DEQUEUE(&my_ifp->if_snd, m);
	if (m == 0)
	{
		tcip_obsd_put_rqb(channel, pRQB);
	}
	else
	{
		my_ifp->if_flags |= IFF_OACTIVE;
		tcip_obsd_pkt_send(channel, (void *)m, pRQB);
		my_ifp->if_flags &= ~IFF_OACTIVE;  /* send operation complete */
	}
}

int     obsd_if_init(struct ifnet *ifp)
{
	struct obsd_softc *sc = (struct obsd_softc *) ifp->if_softc;

	obsd_if_stop(ifp);
	ifp->if_flags |= IFF_RUNNING;
	ifp->if_flags &= ~IFF_OACTIVE;

	timeout_add_sec(&sc->timer_handle, 1);

	return (0);
}

static int
obsd_if_ll_filter(struct ifnet *ifp, caddr_t addr, u_int8_t user_idx)
{
	in_addr_t ip_address = 0;

	if (0 != addr )
	{
		ip_address = *(in_addr_t*)addr;
	}

	tcip_edd_srv_arp_filter(obsd_pnio_get_tcip_channel(ifp), user_idx, ip_address);
	return 0;
}

static int
obsd_if_ll_mac(struct ifnet *ifp, caddr_t addr, int mode)
{
	u_int64_t mac_address = 0;

	if (0 != addr )
	{
		int idx;

		/* rg2155 06.12.2016: copy mac address byte by byte for edd */
		for (idx = 0; idx < ETHER_ADDR_LEN; idx++)
		{
			mac_address |= (u_int64_t)((u_int8_t)addr[ETHER_ADDR_LEN - 1 - idx]) << (idx * 8);
		}
	}

	if (mode == 0)
	{
		tcip_edd_reset_carp_multicast(obsd_pnio_get_tcip_channel(ifp), mac_address);
	}
	else
	{
		tcip_edd_set_carp_multicast(obsd_pnio_get_tcip_channel(ifp), mac_address);
	}
	return 0;
}

static int obsd_if_ioctl(struct ifnet *ifp, u_long command, caddr_t data)
{
	struct obsd_softc *sc = (struct obsd_softc *) ifp->if_softc;
	/* struct ifreq	*ifr = (struct ifreq *) data; */
	struct ifaddr *ifa = (struct ifaddr *)data;
	//	struct in_aliasreq *ifra = (struct in_aliasreq *)data;
	int error = 0;

	switch(command) {

	case SIOCAIFADDR:
	case SIOCDIFADDR:
#if 0  /* atm. not used due to legacy interniche approach -- currently set by OHA */
		/* rg2155 06.12.2016: reset arp filter for regular if */
		error = obsd_if_ll_filter(ifp, data, TCIP_ARP_USER_INDEX /* regular if */);
#endif
		break;
	case SIOCSIFADDR:
		ifp->if_flags |= IFF_UP;
		if (!(ifp->if_flags & IFF_RUNNING))
			obsd_if_init(ifp);
#ifdef INET
		if (ifa->ifa_addr->sa_family == AF_INET)
			arp_ifinit(&sc->sc_arpcom, ifa);
#endif /* INET */
		break;
	case SIOCSIFFLAGS:
		if (ifp->if_flags & IFF_UP) {
			if (ifp->if_flags & IFF_RUNNING)
				error = ENETRESET;
			else
				obsd_if_init(ifp);
		} else {
			if (ifp->if_flags & IFF_RUNNING)
				obsd_if_stop(ifp);
		}
		break;

	case SIOCDEDDIP: /* remove CARP IP, IP=0, as arp filter at EDD */
	case SIOCAEDDIP: /* add CARP IP, specified IP, as arp filter at EDD */
		error = obsd_if_ll_filter(ifp, data, TCIP_CARP_USER_INDEX /* carp if */);
		break;

	case SIOCDEDDMAC: /* remove CARP virtual MAC at EDD */
		error = obsd_if_ll_mac(ifp, data, 0 /* del */);
		break;
	case SIOCAEDDMAC: /* add CARP virtual MAC at EDD */
		error = obsd_if_ll_mac(ifp, data, 1 /* add */);
		break;

	/* not supported yet:
	 * case SIOCGIFMEDIA:
	 * case SIOCSIFMEDIA:
	 * 	error = ifmedia_ioctl(ifp, ifr, &sc->sc_mii.mii_media, command);
	 *	break;
	 */
	default:
		error = ether_ioctl(ifp, &sc->sc_arpcom, command, data);
	}

	if (error == ENETRESET) {
		if (ifp->if_flags & IFF_RUNNING)
		{
		}
		error = 0;
	}

	return (error);
}

/*
 * Stop the adapter and free any mbufs allocated to the
 * RX and TX lists.
 */
static void obsd_if_stop(struct ifnet *ifp)
{
	struct obsd_softc *sc;

	sc = (struct obsd_softc *)ifp->if_softc;
	ifp->if_timer = 0;
	timeout_del(&sc->timer_handle);
	ifp->if_flags &= ~(IFF_RUNNING | IFF_OACTIVE);
}

int obsd_if_detach( struct obsd_softc *sc)
{
	struct ifnet	*ifp = &sc->sc_arpcom.ac_if;

	/* if the interface is still running: stop it */
	if (ifp->if_flags & IFF_RUNNING)
		obsd_if_stop(ifp);

	ether_ifdetach(ifp);
	if (sc->if_is_attached != 0)
		if_detach(ifp);

	return 0;  /* nothing else to do */
}



/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_mgmt_funcs.c
 *
 * @brief       PNIO mini functions for interface and default route management
 * @details     Implementation
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.07
 * @date        created: 28.05.2013
 */
/* -------------------------------------------------------------------------------------------------- */

/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_act_module) */
#define LTRC_ACT_MODUL_ID 4032 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_act_module) */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_resource.h>
#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_route.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_ip_carp.h>
#include <netinet/obsd_kernel_in_var.h>

#include <obsd_userland_errno.h>
#include <obsd_userland_string.h>
#include <obsd_userland_unistd.h>  /* write */

#include <obsd_kernel_usr.h>
#include <obsd_platform_osal.h>
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

static void print_if_list(void);

/**
@brief	Enumeration for the different CARP states used within the
functions obsd_netif_set_carp_role/obsd_netif_get_carp_role.
*/
enum OBSD_CARP_STATE {
	OBSD_CARP_STATE_INIT = 0,	/* initial state - is treated like backup */
	OBSD_CARP_STATE_BACKUP,		/* backup */
	OBSD_CARP_STATE_MASTER		/* master */
};

/* ------------------------ interface mgmt ------------------------------------------------------- */
/**
    @brief          Sets the IP(v4)-Address/Network mask for a given (and known) Network interface. 
    @details        Sets a IPv4 network address and the according network mask for an network interface of the IP-Stack.
                    The network interface must be known within the IP-Stack. 
    @param[in]      ifname: The name of the network interface. Typically a device instance name that consists of a device
                    class name and a unit name. E.g.: eth0, eth1, lo0, lo1, ..
    @param[in]      ipv4_address:     The IPv4 address to set. The address is in network byte order.
                                      (Network order is required by PN SOCK component for backward compatibility reasons)
    @param[in]      ipv4_subnet_mask: The (IPv4) network mask to set. The address is in network byte order.
                                      (Network order is required by PN SOCK component for backward compatibility reasons)
    @note           @li   The Function is intended to be called from the userland level.
                    @li   The Function can only set one IP-Address/mask to the interface. No alias-IP's possible. 
                    @li   If the interface already has an IP-Address the function will fail and returns an error. 
    @sideeffects    As usual for IP-Interfaces setting the (first) IP-Address sets the Interface "UP" too.
                    (see man 8 ifconfig)
    @attention      interface change for function obsd_netif_set_ipv4addr starting with version 0.05 of file obsd_userland_pnio_mgmt_funcs.c:
                    ipv4_address and ipv4_subnet_mask are in network byte order now (both were in host byte order in older version)
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @sa             man 8 ifconfig
*/
int obsd_netif_set_ipv4addr(const char *ifname, unsigned long ipv4_address, unsigned long ipv4_subnet_mask)
{
    struct ifreq        del_addr_req;
    struct in_aliasreq  add_addr_req;
    int rc=0;
    int sock;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_000) */
	TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_CHAT, "obsd_netif_set_ipv4addr: set new address on interface %c%c%c%c%c%c", ifname[0], ifname[1], ifname[2], ifname[3], ifname[4], ifname[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_000) */

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

    if (sock >= 0)
    {
        /* clear the old address first */
        bzero(&del_addr_req, sizeof(del_addr_req));
        strlcpy(del_addr_req.ifr_name, ifname, sizeof(del_addr_req.ifr_name));
        if (ioctl(sock, SIOCDIFADDR, (char *)&del_addr_req) < 0) /* OBSD_ITGR: cast to char* to avoid some compiler warning */
        {
            if (errno != EADDRNOTAVAIL )
            {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_002) */
				TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_set_ipv4addr: could not delete old address");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_002) */
                rc=-1;   /* let errno unchanged */
            }
            /* else: EADDRNOTAVAIL may happen, means the interface has no address so far */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_003) */
			else
			{
				TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "obsd_netif_set_ipv4addr: interface has no old address");
			}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_003) */
        }
    }
    else
    {
        rc = -1;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_004) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_FATAL, "obsd_netif_set_ipv4addr: could not open socket!");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_004) */
    }

    if (rc == 0)
    {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_005) */
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_netif_set_ipv4addr: set addr: %u.%u.%u.%u",
			               (unsigned)(ipv4_address&0xff),(unsigned)((ipv4_address>>8)&0xff),(unsigned)((ipv4_address>>16)&0xff),(unsigned)(ipv4_address>>24));
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_netif_set_ipv4addr: set mask: %u.%u.%u.%u;",
			               (unsigned)(ipv4_subnet_mask&0xff),(unsigned)((ipv4_subnet_mask>>8)&0xff),(unsigned)((ipv4_subnet_mask>>16)&0xff),(unsigned)(ipv4_subnet_mask>>24));

/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_005) */
        /* set the new address + network mask */
        bzero(&add_addr_req, sizeof(add_addr_req));
        strlcpy(add_addr_req.ifra_name, ifname, sizeof(add_addr_req.ifra_name));

        /* gh2289n: would be better/more "natural" to have the ipv4_address and ipv4_subnet_mask  */
        /*          in host order for function obsd_netif_set_ipv4addr. But due to some           */
        /*          compatibility requirements PN integration requested the function args to be   */
        /*          in network order. So no htonl needed here for ipv4_address + ipv4_subnet_mask */
        add_addr_req.ifra_addr.sin_addr.s_addr = ipv4_address; /* normally e.g. inet_aton("192.168.10.11", &add_addr_request.ifra_addr.sin_addr); */
        add_addr_req.ifra_addr.sin_len         = sizeof(struct sockaddr_in);
        add_addr_req.ifra_addr.sin_family      = AF_INET;

        add_addr_req.ifra_mask.sin_addr.s_addr = ipv4_subnet_mask; /* normally e.g. inet_aton("255.255.128.0", &add_addr_request.ifra_mask.sin_addr); */
        add_addr_req.ifra_mask.sin_len         = sizeof(struct sockaddr_in);
        add_addr_req.ifra_mask.sin_family      = AF_INET;

        /* assigning the (first) address implicitly brings the interface "UP" */
        if (ioctl(sock, SIOCAIFADDR, (char *)&add_addr_req) < 0) /* OBSD_ITGR: cast to char* to avoid some compiler warning */
        {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_006) */
  			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_set_ipv4addr: could not set new address");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_006) */
            rc=-1;   /* let errno unchanged */
        }
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_007) */
		else
		{
			 TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "obsd_netif_set_ipv4addr: interface has new address");
		}

		print_if_list();
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_007) */
    }

    if (sock >= 0)
    {
        close(sock);
    }

    return rc;
}

/**
    @brief          Deletes the IP(v4)-Address/Network mask for a given (and known) Network interface and frees the resources
    @details        Deletes the IPv4 network address / network mask and free's the (Kernel-) resources allocated during 
                    assignment of the IPv4 IP-Address and network mask via calling obsd_netif_set_ipv4addr
                    The network interface must be known within the IP-Stack. 
    @param[in]      ifname: The name of the network interface. Typically a device instance name that consists of a device
                    class name and a unit name. E.g.: eth0, eth1, lo0, lo1, ..
    @note           @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @sa             man 8 ifconfig
*/
int obsd_netif_del_ipv4addr(const char *ifname)
{
    struct ifreq        del_addr_req;
    int rc=0;
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

    if (sock >= 0)
    {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_008) */
		TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_CHAT, "obsd_netif_del_ipv4addr: delete address from interface %c%c%c%c%c%c", ifname[0], ifname[1], ifname[2], ifname[3], ifname[4], ifname[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_008) */
        /* clear the old address first */
        bzero(&del_addr_req, sizeof(del_addr_req));
        strlcpy(del_addr_req.ifr_name, ifname, sizeof(del_addr_req.ifr_name));
        if (ioctl(sock, SIOCDIFADDR, (char *)&del_addr_req) < 0) /* OBSD_ITGR: cast to char* to avoid some compiler warning */
        {
            if (errno != EADDRNOTAVAIL )
            {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_009) */
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_del_ipv4addr: could not delete address(errno: %u)", errno);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_009) */
                rc=-1;   /* let errno unchanged */
            }
            /* else: EADDRNOTAVAIL may happen, means the interface has no address so far */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_010) */
			else
			{
				TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "obsd_netif_del_ipv4addr: interface has no such address");
			}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_010) */
        }
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_011) */
		else
		{
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_netif_del_ipv4addr: address successful deleted");
		}

		print_if_list();
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_011) */

        close(sock);
    }
    else
    {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_012) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_del_ipv4addr: could not open socket!");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_012) */
        rc = -1;
    }


    return rc;
}

/**
    @brief          Sets the admin state for a given (and known) Network interface. 
    @details        Sets the admmin state of a given network interface to "UP" or "DOWN"
    @param[in]      ifname: The name of the network interface. Typically a device instance name that consists of a device
                    class name and a unit name. E.g.: eth0, eth1, lo0, lo1, ..
    @param[in]      up_or_down:  admin state to set to the interface
                    @li   == 0: "DOWN"
                    @li   != 0: "UP"
    @note           @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @sa             man 8 ifconfig
*/
int obsd_netif_set_admin_state(const char *ifname, int up_or_down)
{
    int rc=0;
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

    if (sock >=0)
    {
        struct ifreq   flags_req;
        bzero(&flags_req, sizeof(flags_req));
        strlcpy(flags_req.ifr_name, ifname, sizeof(flags_req.ifr_name));

        /* get the current flags first */
        if (ioctl(sock, SIOCGIFFLAGS, (caddr_t)&flags_req) >= 0)
        {
            if (up_or_down != 0)
            {   /* bring interface "UP" */
                flags_req.ifr_flags |= IFF_UP;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_014) */
				TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_netif_set_admin_state: bring interface up: %c%c%c%c%c%c", ifname[0], ifname[1], ifname[2], ifname[3], ifname[4], ifname[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_014) */
            }
            else
            {   /* bring interface "DOWN" */
                flags_req.ifr_flags &= ~IFF_UP;
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_015) */
 		 		TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_netif_set_admin_state: bring interface down: %c%c%c%c%c%c", ifname[0], ifname[1], ifname[2], ifname[3], ifname[4], ifname[5]);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_015) */
            }
            rc = ioctl(sock, SIOCSIFFLAGS, (caddr_t)&flags_req);

/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_016) */
			if (rc < 0)
			{
				TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_set_admin_state: could not set admin state!");
			}

/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_016) */
        }
        else
        {
            rc = -1;    /* ioctl failed */
        }

        close(sock);
    }
    else
    {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_017) */
		 TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_netif_set_admin_state: could not open socket!");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_017) */
        rc = -1;    /* no socket */
    }

    return rc;
}

/**
	@brief			Setting the carp operation role.
	@details		Setting the specified carp interface to primary or backup.
	@return			0 in case of success
	@return			-1 in case of errors
*/
int
obsd_netif_set_carp_role
(
	const char *ifcarp,
	unsigned int is_primary
)
{
	int rc = -1;
	int sock = -1;
	sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

	if (sock >= 0)
	{
		struct ifreq ifr;
		struct carpreq carpr;
		int requested_state = 0;

		memset(&ifr, 0, sizeof(ifr));
		memset(&carpr, 0, sizeof(carpr));

		ifr.ifr_data = (caddr_t)&carpr;

		if (!ifcarp || strlcpy(ifr.ifr_name, ifcarp, sizeof(ifr.ifr_name)) > IFNAMSIZ)
		{
			close(sock);
			errno = EINVAL;
			return rc;
		}

		if (ioctl(sock, SIOCGVH, &ifr) == -1)
		{
			close(sock);
			return rc;
		}

		if (is_primary)
			requested_state = OBSD_CARP_STATE_MASTER;	/* MASTER */
		else
			requested_state = OBSD_CARP_STATE_BACKUP;	/* BACKUP */

		if (carpr.carpr_state != requested_state)
		{
			carpr.carpr_state = requested_state;

			if (ioctl(sock, SIOCSVH, &ifr) == -1)
			{
				close(sock);
				return rc;
			}
		}

		rc = 0;
		close(sock);
	}

	return rc;
}

/**
	@brief			Getting the carp operation role.
	@details		Getting the current operation role of the specified carp interface.
	@param[in]		ifcarp: The name of the carp interface, e.g. "carp0".
	@param[out]		out_cr: The current carp role. e.g. -1 carp not available, 1 master, 0 backup.
	@return			0 in case of success
	@return			-1 in case of errors
*/
int
obsd_netif_get_carp_role
(
	const char *ifcarp,
	int* out_cr
)
{
	int rc = -1;
	int sock = -1;

	/* check out pointer */
	if (!out_cr)
	{
		errno = EINVAL;
		return rc;
	}
	else
	{
		/* default value */
		*out_cr = -1;
	}

	sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

	if (sock >= 0)
	{
		struct ifreq ifr;
		struct carpreq carpr;
		int carp_state = 0;

		memset(&ifr, 0, sizeof(ifr));
		memset(&carpr, 0, sizeof(carpr));

		ifr.ifr_data = (caddr_t)&carpr;

		if (!ifcarp || strlcpy(ifr.ifr_name, ifcarp, sizeof(ifr.ifr_name)) > IFNAMSIZ)
		{
			close(sock);
			errno = EINVAL;
			return rc;
		}

		if (ioctl(sock, SIOCGVH, &ifr) == -1)
		{
			close(sock);
			return rc;
		}

		carp_state = carpr.carpr_states[0];

		if (carp_state == OBSD_CARP_STATE_MASTER)
			*out_cr = 1;
		else if (carp_state == OBSD_CARP_STATE_BACKUP)
			*out_cr = 0;
		else if (carp_state == OBSD_CARP_STATE_INIT)	/* INIT is treated as BACKUP */
			*out_cr = 0;
		else
			*out_cr = -1;

		rc = 0;
		close(sock);
	}

	return rc;
}

/**
    @brief			Internal subroutine to parametrize the CARP interface.
	@details		Setting the virtual host identifier and the carp device.
    @return         0 in case of success
    @return         -1 in case of errors
*/
static int
obsd_netif_carp_vhid_dev
(
	int sock,
	struct ifreq* ifr_p,
	unsigned char vhid,
	const char* dev
)
{
	/* check parameter */
	if( 0 <= sock &&
		0 != ifr_p &&
		0 != vhid &&
		0 != dev )
	{
		struct carpreq carpr;
		memset(&carpr, 0, sizeof(carpr));
		ifr_p->ifr_data = (caddr_t)&carpr;
		/* gets carp parameters */
		if(0 == ioctl(sock, SIOCGVH, ifr_p)) /* Seems not to be mandatory. */ /* Somehow, initializes carp as backup, but why?  */
		{
			if( IFNAMSIZ > strlcpy(carpr.carpr_carpdev, dev, sizeof(carpr.carpr_carpdev)) )
			{
				carpr.carpr_vhids[0] = vhid;
				carpr.carpr_vhids[1] = 0;
				/* set carp parameters */
				if(0 == ioctl(sock, SIOCSVH, ifr_p))
					return 0;
			}
			else
				errno = EINVAL;
		}
	}
	else
		errno = EINVAL;

	return -1;
}

/**
    @brief          Internal subroutine to configure network interface
    @details        Steps:
						1. Creation of network pseudodevices.
						2. vhid != 0 -> Parametrize a CARP interfaces 
    @return         0 in case of success
    @return         -1 in case of errors
*/
static int
obsd_netif_config_if
(
	int sock,
	const char *ifname1,
	unsigned char vhid,
	const char *ifname2
)
{
    if( 0 <= sock &&
		0 != ifname1 )
    {
        struct ifreq   create_req;

        memset(&create_req, 0, sizeof(create_req));
        if( IFNAMSIZ > strlcpy(create_req.ifr_name, ifname1, sizeof(create_req.ifr_name)) )
		{
			if( 0 == ioctl(sock, SIOCIFCREATE , (caddr_t)&create_req) )
			{
				if( 0 != vhid )
				{
					return obsd_netif_carp_vhid_dev( sock, &create_req, vhid, ifname2);
				}
				return 0;
			}
		}
    }
	return -1;
}

/**
    @brief          Creates a CARP interface.
    @details        TBD 
*/
int
obsd_netif_create_carpif
(
	const char *ifcarp,
	unsigned char vhid,
	const char *ifname
)
{
    int rc=0;
    int sock;

	if( 0 == vhid ||
		0 == ifname )
	{
		/* invalid vhid */
		errno = EINVAL;
		return -1;
	}

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */
    if( 0 <= sock )
    {
        rc = obsd_netif_config_if(sock, ifcarp, vhid, ifname);
        close(sock);
    }
    else
    {
        rc = -1;    /* no socket */
    }

    return rc;
}


/**
    @brief          Creates a new (network) pseudodevice
    @details        Creates a new instance of a (network) pseudodevice. 
    @param[in]      ifname: The name of the pseudodevice to create. Typically a device instance name that consists of a device
                    class name and a unit name. E.g.: lo1, lo2, 
    @note           @li   The Function is intended to be called from the userland level.
                    @li   Only new instances of a pseudodevice are createable ("cloneable"). E.g. the function is not able to 
                          create a new device representation for a "real hardware" network interface. 
                    @li   Currently only the loopback pseudodevice device is known (lo[unit])
                    @li   The lo0 loopback device is automatically created during kernel initialization. Creating another lo0
                          is not possible and the function will return an error.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly), typical errno-values are:
                    @li   EEXIST: device already exists. 
                    @li   EINVAL: device is not createable (no cloneable pseudo device) 
*/
int obsd_netif_create_if(const char * ifname)
{
    int rc=0;
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */
    if( 0 <= sock )
    {
        rc = obsd_netif_config_if(sock, ifname, 0, 0);
        close(sock);
    }
    else
    {
        rc = -1;    /* no socket */
    }

    return rc;
}

/**
    @brief          Destroys a (network) pseudodevice formerly created using obsd_netif_create_if
    @details        destroys an existing instance of a (network) pseudodevice. 
    @param[in]      ifname: The name of the pseudodevice to destroy. Typically a device instance name that consists of a device
                    class name and a unit name. E.g.: lo1, lo2, 
    @note           @li   The Function is intended to be called from the userland level.
                    @li   Only instances of a (cloned) pseudodevice are destroyable. E.g. the function is not able to 
                          destroy a device representation for a "real hardware" network interface. 
                    @li   Currently only the loopback pseudodevice device is known (lo[unit])
                    @li   The lo0 loopback device is automatically destroyed during kernel de-initialization. destroying lo0
                          is not possible (and not necessary) and the function will return an error.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly), typical errno-values are:
                    @li   ENXIO:  device unit does not exists. 
                    @li   EINVAL: device is not destroyable (no cloned pseudo device)
                    @li   EPERM:  lo0 is not destroyable
*/
int obsd_netif_destroy_if(const char * ifname)
{
    int rc=0;
    int sock;

    sock = socket(AF_INET, SOCK_DGRAM, 0); /* IPv4 only */

    if (sock >=0)
    {
        struct ifreq   create_req;

        bzero(&create_req, sizeof(struct ifreq));
        strlcpy(create_req.ifr_name, ifname, sizeof(create_req.ifr_name));

        rc= ioctl(sock, SIOCIFDESTROY , (caddr_t)&create_req);

        close(sock);
    }
    else
    {
        rc = -1;    /* no socket */
    }

    return rc;
}

/* ------------------------ routing -------------------------------------------------------------- */
#define ROUNDUP(a) \
    ((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))

typedef union sockunion 
{
    struct sockaddr         sa;
    struct sockaddr_in      sin;
} su_t, *psu_t;

static int route_rtmsg(int sock, int cmd, int flags, u_int tableid, int rtm_addrs, psu_t pso_dst, psu_t pso_mask, psu_t pso_gate);

/**
    @brief          Sets the default gateway.
    @details        Sets the IP(v4)-Address for the default gateway. This leads to a new entry in the kernel routing table.
    @param[in]      ipv4_defgw_address: The IPv4 address to set for the default gateway. The address is in network byte order.
                                        (Network order is required by PN SOCK component for backward compatibility reasons)
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   The Function is intended to be called from the userland level.
                    @li   The Function can only set one default gateway. If already a default gateway is set, the function
                          will return an error.
    @attention      interface change for function obsd_route_add_default_gw starting with version 0.05 of file obsd_userland_pnio_mgmt_funcs.c:
                    ipv4_defgw_address is in network byte order now (was in host byte order in older version)
    @sideeffects    Deletes an existing default gateway first (if one). This behavior is requested from the PN integration and 
                    is different to the route commands (that avoids overwriting a existing default gateway). 
*/
int obsd_pnio_add_route(unsigned long ipv4_address, unsigned long ipv4_mask, unsigned long ipv4_gateway)
{
    int     sock;
    int     ret         = 0;
    int     rtm_addrs   = 0;
    u_int   tableid     = getrtable();

    sock = socket(PF_ROUTE, SOCK_RAW, 0);

    if (sock != -1)
    {
        union sockunion so_dst, so_mask, so_gate;
		
		rtm_addrs           |= RTA_DST;
        memset(&so_dst,     0, sizeof(union sockunion));
        so_dst.sa.sa_len    = sizeof(struct sockaddr_in);
        so_dst.sa.sa_family = AF_INET;
		so_dst.sin.sin_addr.s_addr = ipv4_address;
    
        rtm_addrs           |= RTA_NETMASK;
        memset(&so_mask,    0, sizeof(union sockunion));
        so_mask.sa.sa_len   = sizeof(struct sockaddr_in);
        so_mask.sa.sa_family= AF_INET;
		so_mask.sin.sin_addr.s_addr = ipv4_mask;

        rtm_addrs           |= RTA_GATEWAY;
        memset(&so_gate,    0, sizeof(union sockunion));
        so_gate.sa.sa_len = sizeof(struct sockaddr_in);
        so_gate.sa.sa_family = AF_INET;
        so_gate.sin.sin_addr.s_addr = ipv4_gateway; /* normally e.g. inet_pton(AF_INET, "192.168.10.1", &so_gate.sin.sin_addr) */

        ret = route_rtmsg(sock, RTM_ADD, RTF_STATIC | RTF_UP | RTF_GATEWAY, tableid, rtm_addrs, &so_dst, &so_mask, &so_gate);

/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_018) */
		if (ret < 0)
		{
 			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_pnio_add_route: could not set route!");
		}
		else
		{
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_add_route successful: address: %u.%u.%u.%u",
				(unsigned)(ipv4_address & 0xff), (unsigned)((ipv4_address >> 8) & 0xff), (unsigned)((ipv4_address >> 16) & 0xff), (unsigned)(ipv4_address >> 24));
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_add_route successful: mask: %u.%u.%u.%u",
				(unsigned)(ipv4_mask & 0xff), (unsigned)((ipv4_mask >> 8) & 0xff), (unsigned)((ipv4_mask >> 16) & 0xff), (unsigned)(ipv4_mask >> 24));
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_add_route successful: gateway: %u.%u.%u.%u",
				(unsigned)(ipv4_gateway & 0xff), (unsigned)((ipv4_gateway >> 8) & 0xff), (unsigned)((ipv4_gateway >> 16) & 0xff), (unsigned)(ipv4_gateway >> 24));
		}

		print_routing_table();

/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_018) */
        close(sock);
    }
    else
    {
/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_019) */
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, "obsd_pnio_add_route: could not open socket!");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_019) */
        ret = -1;
    }
    return ret;
}

/**
    @brief          Deletes the default gateway.
    @details        Deletes the IP(v4) default gateway in the routing table. An existing default gateway entry in the kernel routing table
                    will be deleted.
    @param[in]      none
    @note           @li   The Function is intended to be called from the userland level.
                    @li   The Function can only delete an existing default gateway. If no default gateway is set, the function
                          will return an error and nothing will be deleted.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_del_route(unsigned long ipv4_address, unsigned long ipv4_mask)
{
    int     sock;
    int     ret         = 0;
    int     rtm_addrs   = 0;
    u_int   tableid     = getrtable();

    sock = socket(PF_ROUTE, SOCK_RAW, 0);

    if (sock != -1)
    {
        union sockunion so_dst, so_mask, so_gate;

        rtm_addrs           |= RTA_DST;
        memset(&so_dst,     0, sizeof(union sockunion));
        so_dst.sa.sa_len    = sizeof(struct sockaddr_in);
        so_dst.sa.sa_family = AF_INET;
		so_dst.sin.sin_addr.s_addr = ipv4_address;

        rtm_addrs           |= RTA_NETMASK;
        memset(&so_mask,    0, sizeof(union sockunion));
        so_mask.sa.sa_len   = sizeof(struct sockaddr_in);
        so_mask.sa.sa_family= AF_INET;
		so_mask.sin.sin_addr.s_addr = ipv4_mask;

        memset(&so_gate,    0, sizeof(union sockunion));    /* gateway information not used / RTA_GATEWAY not set */

		ret = route_rtmsg(sock, RTM_DELETE, RTF_STATIC | RTF_UP | RTF_GATEWAY, tableid, rtm_addrs, &so_dst, &so_mask, &so_gate);

		if (ret == -1 && errno == ESRCH) /* no gateway available to delete --> no error */
		{
			ret = 0;
		}

/* OBSD_TRACE_PATCH_START - patch no. (obsd_userland_pnio_mgmt_020) */
		if (ret == -1)
		{
			TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_NOTE_LOW, "obsd_pnio_del_route failed!");
		}
		else
		{
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "obsd_pnio_del_route successful: address: %u.%u.%u.%u",
				(unsigned)(ipv4_address & 0xff), (unsigned)((ipv4_address >> 8) & 0xff), (unsigned)((ipv4_address >> 16) & 0xff), (unsigned)(ipv4_address >> 24));
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "obsd_pnio_del_route successful: mask: %u.%u.%u.%u",
				(unsigned)(ipv4_mask & 0xff), (unsigned)((ipv4_mask >> 8) & 0xff), (unsigned)((ipv4_mask >> 16) & 0xff), (unsigned)(ipv4_mask >> 24));
		}

		print_routing_table();
/* OBSD_TRACE_PATCH_END - patch no. (obsd_userland_pnio_mgmt_020) */
        close(sock);
    }
    else
    {
        ret = -1;
    }
    return ret;
}

/* ------------------------ internal functions --------------------------------------------------- */
static int route_rtmsg(int sock, int cmd, int flags, u_int tableid, int rtm_addrs, psu_t pso_dst, psu_t pso_mask, psu_t pso_gate)
{
    struct 
    {
        struct rt_msghdr    m_rtm;
        char                m_space[512];
    } m_rtmsg;

    if ((cmd == RTM_ADD) || (cmd == RTM_DELETE))
    {
        static int          seq         = 0;
        int                 written     = 0;

        char                *cp         = m_rtmsg.m_space;
        int                 len         = 0;
        struct rt_metrics   rt_metrics;
        su_t                so_genmask, so_ifa, so_ifp, so_label, so_src;

        memset(&rt_metrics, 0, sizeof(struct rt_metrics));

        memset(&so_genmask, 0, sizeof(su_t));
        memset(&so_ifa,     0, sizeof(su_t));
        memset(&so_ifp,     0, sizeof(su_t));
        memset(&so_label,   0, sizeof(su_t));
        memset(&so_src,     0, sizeof(su_t));

#define NEXTADDR(w, u)              \
    if (rtm_addrs & (w)) {          \
        len = ROUNDUP(u.sa.sa_len); \
        memcpy(cp, &(u), len);      \
        cp += len;                  \
    }

        errno = 0;
        memset(&m_rtmsg, 0, sizeof(m_rtmsg));

        m_rtmsg.m_rtm.rtm_type      = (u_char)cmd;       /* RTM_ADD or RTM_DELETE */
        m_rtmsg.m_rtm.rtm_flags     = flags;     /* 2051 */
        m_rtmsg.m_rtm.rtm_fmask     = 0; 
        m_rtmsg.m_rtm.rtm_version   = RTM_VERSION;
        m_rtmsg.m_rtm.rtm_seq       = ++seq;
        m_rtmsg.m_rtm.rtm_addrs     = rtm_addrs; /* 7 */
        m_rtmsg.m_rtm.rtm_rmx       = rt_metrics;
        m_rtmsg.m_rtm.rtm_inits     = 0; 
        m_rtmsg.m_rtm.rtm_tableid   = (u_short) tableid;
        m_rtmsg.m_rtm.rtm_priority  = 0; 
        m_rtmsg.m_rtm.rtm_mpls      = 0x0;  /* OBSD_ITGR -- MPLS_OP_LOCAL (0x00) replaced by 0; */; 
        m_rtmsg.m_rtm.rtm_hdrlen    = sizeof(m_rtmsg.m_rtm);

		NEXTADDR(RTA_DST, (*pso_dst));
        NEXTADDR(RTA_GATEWAY, (*pso_gate));
        NEXTADDR(RTA_NETMASK, (*pso_mask));
        NEXTADDR(RTA_GENMASK, so_genmask);
        NEXTADDR(RTA_IFP, so_ifp);
        NEXTADDR(RTA_IFA, so_ifa);
        NEXTADDR(RTA_LABEL, so_label);
        NEXTADDR(RTA_SRC, so_src);
        len = (int)(cp - (char *)&m_rtmsg); /* gerlach/gh2289n: added explicit type cast to avoid compiler warning C4244 */
        m_rtmsg.m_rtm.rtm_msglen = (u_short)len;

        written = write(sock, &m_rtmsg, len);
        if (written != len) 
        {
            return (-1);
        }
        return (0);
    }
    else
    {
        return (-1);
    }
}

/*=================== Set maximum number of open sockets ================== */
/**
    @brief          Sets the maximum socket count
    @details        Sets the maximum socket count (file descriptors in OpenBSD)
    @param[in]      max. count of sockets
    @return         0 in case of success
    @return         -1 in case of errors
                       (errno is set to EINVAL)
    @note           if compile option OBSD_MULTITHREADING is != 0 the Function 
                    returns 0 even in the case the requested limit is too high
                    (same as in original OpenBSD for superuser)
*/
int obsd_pnio_set_max_socket_number (int max_sockets)
{
    struct rlimit socket_number;
    socket_number.rlim_cur = max_sockets;
    socket_number.rlim_max = max_sockets;

    return setrlimit(RLIMIT_NOFILE, &socket_number);
}

/*===================   Set IP-Ports to high range   ================== */

int obsd_pnio_set_ip_portrange_high(int sock_fd)
{
	int option = IP_PORTRANGE_HIGH; /* RQ 1007104 - make portnumbers conform to IANA */
	int error = setsockopt(sock_fd, IPPROTO_IP, IP_PORTRANGE, &option, sizeof(int));
	return error;
}

/**
@brief			Enables or disables IPV4 forwarding.
@details		Forwarding of IPV4 packets to available network interfaces/subnets
				is activated/deactivated by means of sysctl.
				This way OBSD acts as a router for IPV4.
@param[in]		enable
				1 activates IPV4 forwarding
				0 deactivates IPV4 forwarding
@return			0 in case of success
@return			-1 in case of errors
*/
int obsd_set_ipv4forwarding(unsigned int enable)
{
    int	mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_FORWARDING };

    int retval = sysctl(mib, nitems(mib), NULL, 0, &enable, sizeof(enable));

    if (retval == -1)
    {
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_ERROR, 
			"obsd_set_ipv4forwarding: Failed to set IP forwarding to %u!", enable);
    }
    else
    {
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT,
			"obsd_set_ipv4forwarding: Setting IP forwarding to %u succeeded.", enable);
	}

    return retval;
}

/**
@brief			Returns whether IPV4 Forwarding is currently enabled.
@param[out]		out_is_enabled
				1 IPV4 Forwarding is enabled
				0 IPV4 Forwarding is disabled
@return			0 in case of success
@return			-1 in case of errors
*/
int obsd_get_ipv4forwarding(unsigned int *out_is_enabled)
{
    int	mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_FORWARDING };
	int retval = -1;
	size_t opt_length = 0;

	if (!out_is_enabled)
	{
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR,
			"obsd_get_ipv4forwarding: Provided out pointer not valid!");
		return -1;
	}

	opt_length = sizeof(*out_is_enabled);
    retval = sysctl(mib, nitems(mib), out_is_enabled, &opt_length, NULL, 0);

    if (retval == -1)
    {
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_ERROR, 
			"obsd_get_ipv4forwarding: Failed to get current setting of IP forwarding!");
    }
    else
    {
		TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT,
			"obsd_get_ipv4forwarding: IP forwarding is currently set to %u.", *out_is_enabled);
	}

    return retval;
}

/*===================   return last error   ================== */

int obsd_pnio_get_errno(void)
{
	return errno;
}

/*===========================================================================*/
/*                        Tracing - Routing Table                            */
/*===========================================================================*/

extern int sysctl_rtable(int *, u_int, void *, size_t *, void *, size_t);

void print_rti_info(struct sockaddr *rti_info[], int llinfo)
{
#if OBSD_PNIO_CFG_TRACE_MODE > 0
	if (rti_info[RTAX_DST] != 0)
	{
		struct sockaddr *address = rti_info[RTAX_DST];
		if (address->sa_family == AF_INET)
		{
			in_addr_t addr = ((struct sockaddr_in *)address)->sin_addr.s_addr;
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Destination: %u.%u.%u.%u",
				(unsigned)(addr & 0xff), (unsigned)((addr >> 8) & 0xff), (unsigned)((addr >> 16) & 0xff), (unsigned)(addr >> 24));
		}
		else if (address->sa_family == AF_LINK)
		{
			caddr_t addr = LLADDR((struct sockaddr_dl *)address);
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Destination: %u.%u.%u.%u",
				(unsigned)(*addr), (unsigned)(*(addr + 1)), (unsigned)(*(addr + 2)), (unsigned)(*(addr+3)));
		}
	}

	if (rti_info[RTAX_NETMASK] != 0)
	{
		struct sockaddr *address = rti_info[RTAX_NETMASK];
		if (address->sa_family == 0)
		{
			caddr_t addr = LLADDR((struct sockaddr_dl *)address);
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Netmask: %u.%u.%u.%u",
				(unsigned)(*addr), (unsigned)(*(addr + 1)), (unsigned)(*(addr + 2)), (unsigned)(*(addr + 3)));
		}
		else
		{
			in_addr_t addr = ((struct sockaddr_in *)address)->sin_addr.s_addr;
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Netmask: %u.%u.%u.%u",
				(unsigned)(addr & 0xff), (unsigned)((addr >> 8) & 0xff), (unsigned)((addr >> 16) & 0xff), (unsigned)(addr >> 24));
		}
	}

	if (rti_info[RTAX_GATEWAY] != 0)
	{
		struct sockaddr *address = rti_info[RTAX_GATEWAY];

		if (llinfo)
		{
			caddr_t addr = LLADDR((struct sockaddr_dl *)address);
			TCIP_OBSD_TRACE_06(0, OBSD_TRACE_LEVEL_CHAT, "MAC: %02x:%02x:%02x:%02x:%02x:%02x",
				(unsigned)((*addr) & 0xff), (unsigned)((*(addr + 1)) & 0xff), (unsigned)((*(addr + 2)) & 0xff), (unsigned)((*(addr + 3)) & 0xff),
				(unsigned)((*(addr + 4)) & 0xff), (unsigned)((*(addr + 5)) & 0xff));
		}
		else if (address->sa_family == AF_INET)
		{
			in_addr_t addr = ((struct sockaddr_in *)address)->sin_addr.s_addr;
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Gateway: %u.%u.%u.%u",
				(unsigned)(addr & 0xff), (unsigned)((addr >> 8) & 0xff), (unsigned)((addr >> 16) & 0xff), (unsigned)(addr >> 24));
		}
		else if (address->sa_family == AF_LINK)
		{
			caddr_t addr = LLADDR((struct sockaddr_dl *)address);
			TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Gateway: %u.%u.%u.%u",
				(unsigned)(*addr), (unsigned)(*(addr + 1)), (unsigned)(*(addr + 2)), (unsigned)(*(addr + 3)));
		}
	}

	if (rti_info[RTAX_IFP] != 0)
	{
		struct sockaddr *address = rti_info[RTAX_IFP];
		if (address->sa_family == AF_LINK)
		{
			caddr_t addr = ((struct sockaddr_dl *)address)->sdl_data;
			TCIP_OBSD_TRACE_05(0, OBSD_TRACE_LEVEL_CHAT, "Interface name: %c%c%c%c%c",
				(unsigned)(*addr), (unsigned)(*(addr + 1)), (unsigned)(*(addr + 2)), (unsigned)(*(addr + 3)), (unsigned)(*(addr + 4)));
		}
	}

	if (rti_info[RTAX_IFA] != 0)
{
		in_addr_t addr = ((struct sockaddr_in *)(rti_info[RTAX_IFA]))->sin_addr.s_addr;
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Interface address: %u.%u.%u.%u", 
		(unsigned)(addr&0xff),(unsigned)((addr>>8)&0xff),(unsigned)((addr>>16)&0xff),(unsigned)(addr>>24));
	}

	if (rti_info[RTAX_BRD] != 0)
	{
		in_addr_t addr = ((struct sockaddr_in *)(rti_info[RTAX_BRD]))->sin_addr.s_addr;
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Broadcast or p-p Destination address: %u.%u.%u.%u", 
		(unsigned)(addr&0xff),(unsigned)((addr>>8)&0xff),(unsigned)((addr>>16)&0xff),(unsigned)(addr>>24));
	}

	if (rti_info[RTAX_SRC] != 0)
	{
		in_addr_t addr = ((struct sockaddr_in *)(rti_info[RTAX_SRC]))->sin_addr.s_addr;
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Source sock address: %u.%u.%u.%u", 
		(unsigned)(addr&0xff),(unsigned)((addr>>8)&0xff),(unsigned)((addr>>16)&0xff),(unsigned)(addr>>24));
	}

	if (rti_info[RTAX_SRCMASK] != 0)
	{
		in_addr_t addr = ((struct sockaddr_in *)(rti_info[RTAX_SRCMASK]))->sin_addr.s_addr;
		TCIP_OBSD_TRACE_04(0, OBSD_TRACE_LEVEL_CHAT, "Source netmask address: %u.%u.%u.%u", 
		(unsigned)(addr&0xff),(unsigned)((addr>>8)&0xff),(unsigned)((addr>>16)&0xff),(unsigned)(addr>>24));
	}
	
	TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "----------------------------------");
#else
	OBSD_UNUSED_ARG(rti_info);
#endif
}

void print_arp_table(void)
{
	char *buf = 0;
	char *next = 0;
	char *lim = 0;
	size_t needed;
	int mib[3];
	struct rt_msghdr *rtm;
	struct sockaddr* sa;

	mib[0] = AF_INET;
	mib[1] = NET_RT_FLAGS;
	mib[2] = RTF_LLINFO;


	sysctl_rtable(mib, 3, NULL, &needed, NULL, 0);
	if (needed > 0)
	{
		buf = (char *)OSAL_kern_malloc(needed, 0, 0);
		sysctl_rtable(mib, 3, buf, &needed, NULL, 0);
		lim = buf + needed;
	}

	if (buf)
	{
		struct sockaddr	*rti_info[RTAX_MAX];
		int	i;

		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "------------ ARP Table -----------");

		for (next = buf; next < lim; next += rtm->rtm_msglen) {
			rtm = (struct rt_msghdr *)next;
			if (rtm->rtm_version != RTM_VERSION)
				continue;
			sa = (struct sockaddr *)(next + rtm->rtm_hdrlen);

			for (i = 0; i < RTAX_MAX; i++)
			{
				if (rtm->rtm_addrs & (1 << i))
				{
					rti_info[i] = sa;
					sa = (struct sockaddr *)((char *)(sa)+ROUNDUP(sa->sa_len));
				}
				else
				{
					rti_info[i] = NULL;
				}
			}

			TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "Index: %u", rtm->rtm_index);
			print_rti_info(rti_info, 1);
		}

		OSAL_kern_free(buf, 0);
		buf = 0;
	}
}


void print_routing_table(void)
{
	char *buf = 0;
	char *next = 0;
	char *lim = 0;
	size_t needed;
	int mib[3];
	struct rt_msghdr *rtm;
	struct sockaddr* sa;

	mib[0] = 0;
	mib[1] = NET_RT_DUMP;
	mib[2] = 0;


	sysctl_rtable(mib, 3, NULL, &needed, NULL, 0);
	if (needed > 0)
	{
		buf = (char *)OSAL_kern_malloc(needed, 0, 0);	
	sysctl_rtable(mib, 3, buf, &needed, NULL, 0);
	lim = buf + needed;
	}
	
	if (buf) 
	{
		struct sockaddr	*rti_info[RTAX_MAX];
		int	i;

		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "------ Routing Table w/o ARP ------");

		for (next = buf; next < lim; next += rtm->rtm_msglen) {
			rtm = (struct rt_msghdr *)next;
			if (rtm->rtm_version != RTM_VERSION)
				continue;

			if ((rtm->rtm_flags & RTF_LLINFO) == 0)
			{
				sa = (struct sockaddr *)(next + rtm->rtm_hdrlen);

				for (i = 0; i < RTAX_MAX; i++)
				{
					if (rtm->rtm_addrs & (1 << i))
					{
						rti_info[i] = sa;
						sa = (struct sockaddr *)((char *)(sa)+ROUNDUP(sa->sa_len));
					}
					else
					{
						rti_info[i] = NULL;
					}
				}

				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "Index: %u", rtm->rtm_index);
				print_rti_info(rti_info, 0);
			}
		}

		OSAL_kern_free(buf, 0);
		buf = 0;
	}

	print_arp_table();
}

static void print_if_list(void)
{
	char *buf = 0;
	char *next = 0;
	char *lim = 0;
	size_t needed;
	int mib[3];
	struct rt_msghdr *rtm;
	struct sockaddr* sa;

	mib[0] = AF_INET;
	mib[1] = NET_RT_IFLIST;
	mib[2] = 0;

	sysctl_rtable(mib, 3, NULL, &needed, NULL, 0);
	buf = (char *)OSAL_kern_malloc(needed, 0, 0);
	sysctl_rtable(mib, 3, buf, &needed, NULL, 0);
	lim = buf + needed;
	
	if (buf) 
	{
		struct sockaddr	*rti_info[RTAX_MAX];
		int	i;
		
		TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "------- Interfaces with Address -------");

		for (next = buf; next < lim; next += rtm->rtm_msglen) {
			rtm = (struct rt_msghdr *)next;
			if (rtm->rtm_version != RTM_VERSION)
				continue;
			sa = (struct sockaddr *)(next + rtm->rtm_hdrlen);
			
			for (i = 0; i < RTAX_MAX; i++) 
			{
				if (rtm->rtm_addrs & (1 << i)) 
				{
					rti_info[i] = sa;
					sa = (struct sockaddr *)((char *)(sa) + ROUNDUP(sa->sa_len));
				}	 
				else
				{
					rti_info[i] = NULL;
				}
			}


			if (rti_info[RTAX_IFA])
			{
				TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "Index: %u", rtm->rtm_index);
				print_rti_info(rti_info, 0);
			}
			
		}
		OSAL_kern_free(buf, 0);
		buf = 0;
	}
}

#include <machine/obsd_kernel_cpu.h>
#include <sys/obsd_kernel_proc.h>
#define _KERNEL
#include <sys/obsd_kernel_file.h>
#include <sys/obsd_kernel_filedesc.h>
#include <sys/obsd_kernel_socketvar.h>

void obsd_execute_callback(void *soc, int flag)
{
	struct socket *p_socket = (struct socket *)soc;		/* mh2290: LINT 578   Declaration of symbol 'obsd_socket' hides symbol 'obsd_socket()' */
	if (p_socket->callback_fct != NULL)
	{
		p_socket->callback_fct(flag, p_socket->callback_ctx_ptr);
	}
}

int obsd_get_event_list_index(void *soc)
{
	struct socket *p_socket = (struct socket *)soc;		/* mh2290: LINT 578   Declaration of symbol 'obsd_socket' hides symbol 'obsd_socket()' */
	return p_socket->event_list_index;	
}

void obsd_set_event_list_index(void *soc, int index)
{
	struct socket *p_socket = (struct socket *)soc;		/* mh2290: LINT 578   Declaration of symbol 'obsd_socket' hides symbol 'obsd_socket()' */
	p_socket->event_list_index = index;
}

int obsd_has_callback_fct(void *soc)
{
	struct socket *p_socket = (struct socket *)soc;		/* mh2290: LINT 578   Declaration of symbol 'obsd_socket' hides symbol 'obsd_socket()' */

	return p_socket->callback_fct != NULL ? 1 : 0;
}

#ifndef __OBSD_USERLAND_PNIO_TYPES_DEF_H__
#define __OBSD_USERLAND_PNIO_TYPES_DEF_H__

#define OBSD_PNIO_SO_REUSEADDR	0x0004	/* allow local address reuse */
#define OBSD_PNIO_SO_KEEPALIVE	0x0008	/* keep connections alive */
#define OBSD_PNIO_SO_DONTROUTE	0x0010	/* just use interface addresses */
#define OBSD_PNIO_SO_SNDBUF		0x1001	/* send buffer size */
#define OBSD_PNIO_SO_RCVBUF		0x1002	/* receive buffer size */
#define OBSD_PNIO_SO_LINGER		0x0080	/* linger on close if data present */
#define OBSD_PNIO_SO_BROADCAST	0x0020	/* permit sending of broadcast msgs */
#define OBSD_PNIO_SO_OOBINLINE	0x0100	/* leave received OOB data in line */

#define OBSD_PNIO_IP_MULTICAST_IF   9    /* in_addr; set/get IP multicast i/f  */
#define OBSD_PNIO_IP_MULTICAST_TTL  10   /* u_char; set/get IP multicast ttl */
#define OBSD_PNIO_IP_MULTICAST_LOOP 11   /* u_char; set/get IP multicast loopback */
#define OBSD_PNIO_IP_ADD_MEMBERSHIP 12   /* ip_mreq; add an IP group membership */
#define OBSD_PNIO_IP_DROP_MEMBERSHIP  13   /* ip_mreq; drop an IP group membership */
#define OBSD_PNIO_IP_RECVDSTADDR 7  /* bool; receive IP dst addr w/dgram */

#define OBSD_PNIO_EIO			 5	/* Input/output error */
#define	OBSD_PNIO_ENOMEM		12	/* Cannot allocate memory */
#define	OBSD_PNIO_EINVAL		22	/* Invalid argument */
#define	OBSD_PNIO_EPIPE			32	/* Broken pipe */
#define	OBSD_PNIO_EWOULDBLOCK	35	/* Operation would block */
#define	OBSD_PNIO_EINPROGRESS	36	/* Operation now in progress */
#define	OBSD_PNIO_EMSGSIZE		40	/* Message too long */
#define OBSD_PNIO_EOPNOTSUPP  45  /* Operation not supported */
#define	OBSD_PNIO_EADDRINUSE	48	/* Address already in use */
#define	OBSD_PNIO_EADDRNOTAVAIL	49	/* Can't assign requested address */
#define	OBSD_PNIO_ENETUNREACH	51	/* Network is unreachable */
#define	OBSD_PNIO_ECONNABORTED	53	/* Software caused connection abort */
#define	OBSD_PNIO_ECONNRESET	54	/* Connection reset by peer */
#define OBSD_PNIO_ENOBUFS		55	/* No buffer space available */
#define	OBSD_PNIO_ESHUTDOWN		58	/* Can't send after socket shutdown */
#define OBSD_PNIO_ETOOMANYREFS  59 /* Too many references: can't splice */
#define	OBSD_PNIO_ETIMEDOUT		60	/* Operation timed out */
#define	OBSD_PNIO_ECONNREFUSED	61	/* Connection refused */
#define	OBSD_PNIO_EHOSTUNREACH	65	/* No route to host */

#define OBSD_PNIO_TCIP_OK			0x01
#define OBSD_PNIO_ERR_RESOURCE		0x84
#define OBSD_PNIO_ERR_PARAM			0x86
#define OBSD_PNIO_ERR_LOWER_LAYER   0xc0

#define OBSD_PNIO_SENDBUFFER_SIZE 1514
#define OBSD_PNIO_ETHERMTU 1500

#define	OBSD_PNIO_IFDESCRSIZE	255 /* (see RQ2710554) proven value for C1 */

#define OBSD_PNIO_AF_INET		2
#define	OBSD_PNIO_SOCK_STREAM	1	/* stream socket */
#define	OBSD_PNIO_SOCK_DGRAM	2	/* datagram socket */

#define OBSD_PNIO_MSG_PEEK		0x2
#define OBSD_PNIO_MSG_DONTWAIT	0x80

#define	IOCPARM_MASK			0x1fff		/* parameter length, at most 13 bits */
#define	IOC_IN					(unsigned long)0x80000000 /* copy parameters in and out */
#define	_IOC(inout,group,num,len) (inout | ((len & IOCPARM_MASK) << 16) | ((group) << 8) | (num))
#define	_IOW(g,n,t)				_IOC(IOC_IN, (g), (n), sizeof(t))
#define OBSD_PNIO_FIONBIO		_IOW('f', 126, int)		/* set/clear non-blocking i/o */

#define OBSD_PNIO_SOL_SOCKET	0xffff	/* options for socket level */
#define OBSD_PNIO_SO_ERROR		0x1007	/* get error status and clear */
#define OBSD_PNIO_IPPROTO_IP 0
#define OBSD_PNIO_IPPROTO_TCP	6
#define OBSD_PNIO_TCP_NODELAY	0x01   /* don't delay send to coalesce pkts */

#define OBSD_PNIO_SO_EVENT_CALLBACK 0x2000

#define	OBSD_PNIO_NBBY	8				/* number of bits in a byte */

typedef int	obsd_pnio_fd_mask;
#define OBSD_PNIO_NFDBITS ((unsigned)(sizeof(obsd_pnio_fd_mask) * OBSD_PNIO_NBBY)) /* bits per mask */
#define	obsd_pnio_howmany(x, y)	(((x) + ((y) - 1)) / (y))

typedef	struct obsd_pnio_fd_set {
	obsd_pnio_fd_mask obsd_pnio_fds_bits[obsd_pnio_howmany(OBSD_PNIO_CFG_MAX_SOCKETS, OBSD_PNIO_NFDBITS)];
} obsd_pnio_fd_set; /* OBSD_PNIO_CFG_MAX_SOCKETS = FD_SETSIZE */

#define	OBSD_PNIO_FD_SET(n, p) \
/*lint -e701*/	((p)->obsd_pnio_fds_bits[(n) / OBSD_PNIO_NFDBITS] |= (1 << ((n) % OBSD_PNIO_NFDBITS)))

#define	OBSD_PNIO_FD_ISSET(n, p) \
	((p)->obsd_pnio_fds_bits[(n) / OBSD_PNIO_NFDBITS] & (1 << ((n) % OBSD_PNIO_NFDBITS)))

struct obsd_pnio_timeval {
	long	tv_sec;		/* seconds */
	long	tv_usec;	/* and microseconds */
};

struct	obsd_pnio_linger {
	int	l_onoff;		/* option on/off */
	int	l_linger;		/* linger time */
};

struct obsd_pnio_event_callback {
	void (*l_usr_callback)(int, void *); /* user callback function */
	void *l_ctx_ptr;		/* context pointer */
};

struct obsd_pnio_sockaddr {
	unsigned char sa_len;		/* total length */
	unsigned char sa_family;		/* address family */
	char	    sa_data[14];	/* actually longer; address value */
};

struct obsd_pnio_in_addr {
  unsigned int s_addr; /* see obsd_kernel_types.h */
};

struct obsd_pnio_ip_mreq {
  struct  obsd_pnio_in_addr imr_multiaddr;  /* IP multicast address of group */
  struct  obsd_pnio_in_addr imr_interface;  /* local IP address of interface */
};

struct obsd_pnio_iovec {
	void			*iov_base;	/* Base address. */
	unsigned long	 iov_len;	/* Length. */
};

struct obsd_pnio_cmsghdr {
	unsigned int	cmsg_len;	/* data byte count, including hdr */
	int				cmsg_level;	/* originating protocol */
	int				cmsg_type;	/* protocol-specific type */
/* followed by	unsigned char cmsg_data[]; */
};

struct obsd_pnio_msghdr {
	void					*msg_name;	/* optional address */
	unsigned int			msg_namelen;	/* size of address */
	struct obsd_pnio_iovec	*msg_iov;	/* scatter/gather array */
	unsigned int			msg_iovlen;	/* # elements in msg_iov */
	void					*msg_control;	/* ancillary data, see below */
	unsigned int 			msg_controllen;	/* ancillary data buffer len */
	int						msg_flags;	/* flags on received message */
};

extern void *obsd_kernel_memset(void *, int, unsigned int);
extern void  obsd_pnio_check_types(void);
extern void  obsd_pnio_add_socket_read_event(void *socket);
extern void  obsd_pnio_add_socket_write_event(void *socket);
extern void  obsd_pnio_add_socket_connect_event(void *socket);
extern void obsd_pnio_remove_from_event_list(int list_index, const void* socket);

#define	OBSD_PNIO_FD_ZERO(p) obsd_kernel_memset(p, 0, sizeof(*(p)))

#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
struct obsd_pnio_sb_fill_histogram {
            unsigned int ui0ByteCnt;         /*    0 Bytes in socket buf */
            unsigned int ui128ByteCnt;       /*    1...  128 Bytes in socket buf */
            unsigned int ui256ByteCnt;       /*  129 ... 256 Bytes */
            unsigned int ui512ByteCnt;       /*  257 ... 512 Bytes */
            unsigned int ui1024ByteCnt;      /*  513 ...1024 Bytes */
            unsigned int ui2048ByteCnt;      /* 1025 ...2048 Bytes */
            unsigned int ui4096ByteCnt;      /* 2049 ...4096 Bytes */
            unsigned int ui8192ByteCnt;      /* 4097 ...8192 Bytes */
            unsigned int ui16384ByteCnt;     /* 8193 ..16384 Bytes */
            unsigned int uiOverflowByteCnt;  /* >.16384 Bytes      */
		};

#define OBSD_PNIO_SO_RB_FILL_HIST 0x5000
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTROGRAM */

#endif

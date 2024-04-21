#include <obsd_kernel_param.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_filio.h>
#include <sys/obsd_kernel_errno.h>
#include <sys/obsd_kernel_uio.h>
#include <net/obsd_kernel_if.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_tcp.h>
#include <netinet/obsd_kernel_if_ether.h>
#include <sys/obsd_kernel_errno.h>
#include <obsd_userland_netdb.h>
#include <obsd_kernel_usr.h>

/* Defines */
#if OBSD_PNIO_AF_INET != AF_INET
#error (incompatible AF_INET)
#endif
#if	OBSD_PNIO_SOCK_STREAM != SOCK_STREAM
#error (incompatible SOCK_STREAM)
#endif
#if	OBSD_PNIO_SOCK_DGRAM != SOCK_DGRAM
#error (incompatible SOCK_DGRAM)
#endif
#if OBSD_PNIO_MSG_PEEK != MSG_PEEK
#error (incompatible MSG_PEEK)
#endif
#if OBSD_PNIO_MSG_DONTWAIT != MSG_DONTWAIT
#error (incompatible MSG_DONTWAIT)
#endif
#if OBSD_PNIO_SOL_SOCKET != SOL_SOCKET
#error (incompatible SOL_SOCKET)
#endif
#if OBSD_PNIO_SO_ERROR != SO_ERROR
#error (incompatible SO_ERROR)
#endif
#if OBSD_PNIO_SO_EVENT_CALLBACK != SO_EVENT_CALLBACK
#error (incompatible SO_EVENT_CALLBACK)
#endif
#if OBSD_PNIO_IPPROTO_TCP != IPPROTO_TCP
#error (incompatible IPPROTO_TCP)
#endif
#if OBSD_PNIO_TCP_NODELAY != TCP_NODELAY
#error (incompatible TCP_NODELAY)
#endif
#if OBSD_PNIO_SO_REUSEADDR != SO_REUSEADDR
#error (incompatible SO_REUSEADDR)
#endif
#if OBSD_PNIO_SO_KEEPALIVE != SO_KEEPALIVE
#error (incompatible SO_KEEPALIVE)
#endif
#if OBSD_PNIO_SO_DONTROUTE != SO_DONTROUTE
#error (incompatible SO_DONTROUTE)
#endif
#if OBSD_PNIO_SO_SNDBUF != SO_SNDBUF
#error (incompatible SO_SNDBUF)
#endif
#if OBSD_PNIO_SO_RCVBUF != SO_RCVBUF
#error (incompatible SO_RCVBUF)
#endif
#if OBSD_PNIO_SO_LINGER != SO_LINGER
#error (incompatible SO_LINGER)
#endif
#if OBSD_PNIO_SO_BROADCAST != SO_BROADCAST
#error (incompatible SO_BROADCAST)
#endif
#if OBSD_PNIO_SO_OOBINLINE != SO_OOBINLINE
#error (incompatible SO_OOBINLINE)
#endif
#if OBSD_PNIO_IFDESCRSIZE != IFDESCRSIZE
#error (incompatible IFDESCRSIZE)
#endif
#if OBSD_PNIO_ETHERMTU != ETHERMTU
#error (incompatible ETHERMTU)
#endif
#if OBSD_PNIO_SENDBUFFER_SIZE != ETHER_HDR_LEN + OBSD_PNIO_ETHERMTU
#error (incompatible SENDBUFFER_SIZE)
#endif
#if OBSD_PNIO_IP_RECVDSTADDR != IP_RECVDSTADDR
#error (incompatible RECVDSTADDR)
#endif

/* Error Codes */

#if OBSD_PNIO_EIO != EIO
#error (incompatible EIO)
#endif
#if OBSD_PNIO_ENOMEM != ENOMEM
#error (incompatible ENOMEM)
#endif
#if OBSD_PNIO_EINVAL != EINVAL
#error (incompatible EINVAL)
#endif
#if OBSD_PNIO_EPIPE != EPIPE
#error (incompatible EPIPE)
#endif
#if OBSD_PNIO_EWOULDBLOCK != EWOULDBLOCK
#error (incompatible EWOULDBLOCK)
#endif
#if OBSD_PNIO_EINPROGRESS != EINPROGRESS
#error (incompatible EINPROGRESS)
#endif
#if OBSD_PNIO_EADDRINUSE != EADDRINUSE
#error (incompatible EADDRINUSE)
#endif
#if OBSD_PNIO_EADDRNOTAVAIL != EADDRNOTAVAIL
#error (incompatible EADDRNOTAVAIL)
#endif
#if OBSD_PNIO_ENETUNREACH != ENETUNREACH
#error (incompatible ENETUNREACH)
#endif
#if OBSD_PNIO_ECONNABORTED != ECONNABORTED
#error (incompatible ECONNABORTED)
#endif
#if OBSD_PNIO_ECONNRESET != ECONNRESET
#error (incompatible ECONNRESET)
#endif
#if OBSD_PNIO_ENOBUFS != ENOBUFS
#error (incompatible ENOBUFS)
#endif
#if OBSD_PNIO_ESHUTDOWN != ESHUTDOWN
#error (incompatible )
#endif
#if OBSD_PNIO_ETIMEDOUT != ETIMEDOUT
#error (incompatible )
#endif
#if OBSD_PNIO_ECONNREFUSED != ECONNREFUSED
#error (incompatible ECONNREFUSED)
#endif

/* Datatypes */
static int u_int_size = sizeof(unsigned int);
static int socklen_t_size = sizeof(socklen_t);
static int long_size = sizeof(long);
static int ssize_t_size = sizeof(ssize_t);
static int u_long_size = sizeof(unsigned long);
static int size_t_size = sizeof(size_t);

/* Structs*/
static int obsd_pnio_sockaddr_size = sizeof(struct obsd_pnio_sockaddr);
static int sockaddr_size = sizeof(struct sockaddr);
static int obsd_pnio_fd_set_size = sizeof(struct obsd_pnio_fd_set);
static int fd_set_size = sizeof(fd_set);
static int obsd_pnio_timeval_size = sizeof(struct obsd_pnio_timeval);
static int timeval_size = sizeof(struct timeval);
static int obsd_pnio_linger_size = sizeof(struct obsd_pnio_linger);
static int linger_size = sizeof(struct linger);
static int obsd_pnio_event_callback_size = sizeof(struct obsd_pnio_event_callback);
static int event_callback_size = sizeof(struct event_callback);
static int ip_mreq_size = sizeof(struct ip_mreq);
static int obsd_pnio_ip_mreq_size = sizeof(struct obsd_pnio_ip_mreq);
static int iovec_size = sizeof(struct iovec);
static int obsd_pnio_iovec_size = sizeof(struct obsd_pnio_iovec);
static int msghdr_size = sizeof(struct msghdr);
static int obsd_pnio_msghdr_size = sizeof(struct obsd_pnio_msghdr);
static int cmsghdr_size = sizeof(struct cmsghdr);
static int obsd_pnio_cmsghdr_size = sizeof(struct obsd_pnio_cmsghdr);

static unsigned long obsd_fionbio = FIONBIO;
static unsigned long pnio_fionbio = OBSD_PNIO_FIONBIO;

void obsd_pnio_check_types(void)
{
	/* Due to the EPS-Projects this cannot be done with preprocessor directives */
	if (u_int_size != socklen_t_size) exit(0);
	if (long_size != ssize_t_size) exit(0);
	if (u_long_size != size_t_size) exit(0);
	if (obsd_pnio_sockaddr_size != sockaddr_size) exit(0);
	if (obsd_pnio_fd_set_size != fd_set_size) exit(0);
	if (obsd_pnio_timeval_size != timeval_size) exit(0);
	if (obsd_pnio_linger_size != linger_size) exit(0);
	if (obsd_pnio_event_callback_size != event_callback_size) exit(0);
	if (obsd_fionbio != pnio_fionbio) exit(0);
	if (obsd_pnio_ip_mreq_size != ip_mreq_size) exit(0);
	if (obsd_pnio_iovec_size != iovec_size) exit(0);
	if (obsd_pnio_msghdr_size != msghdr_size) exit(0);
	if (obsd_pnio_cmsghdr_size != cmsghdr_size) exit(0);
}

#if OBSD_PNIO_CFG_TRACE_ON
#if OBSD_PNIO_SO_RB_FILL_HIST != SO_RB_FILL_HIST
#error (incompatible SO_RB_FILL_HIST)
#endif
#endif /* OBSD_PNIO_CFG_TRACE_ON */

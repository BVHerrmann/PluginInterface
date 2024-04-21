#ifndef __OBSD_USERLAND_PNIO_SYSCALL_FUNCS_H__
#define __OBSD_USERLAND_PNIO_SYSCALL_FUNCS_H__

int *obsd__errno(void);
#define errno (*obsd__errno())

extern int obsd_bind(int s, struct obsd_pnio_sockaddr *address, unsigned int addr_len);
#define obsd_pnio_bind(s, address, addr_len)  obsd_bind(s, address, addr_len)

extern int obsd_connect(int s, struct obsd_pnio_sockaddr *address, unsigned int addr_len);
#define obsd_pnio_connect(s, address, addr_len)  obsd_connect(s, address, addr_len)

extern int obsd_accept(int s, struct obsd_pnio_sockaddr *address, unsigned int *addr_len);
#define obsd_pnio_accept(s, address, addr_len)  obsd_accept(s, address, addr_len)

extern long obsd_recvfrom(int s, void *buf, unsigned long len, int peek, struct obsd_pnio_sockaddr *address, unsigned int *addr_length);
#define obsd_pnio_recvfrom(s, buf, len, peek, address, addr_length) obsd_recvfrom(s, buf, len, peek, address, addr_length)

extern long obsd_sendto(int s, const void *buf, unsigned long len, int dont_wait, struct obsd_pnio_sockaddr *address, unsigned int addr_length);
#define obsd_pnio_sendto(s, buf, len, dont_wait, address, addr_length) obsd_sendto(s, buf, len, dont_wait, address, addr_length)

extern int obsd_getsockopt(int s, int level, int optname, void *optval, unsigned int *optlen);
#define obsd_pnio_getsockopt(s, level, optname, optval, optlen) obsd_getsockopt(s, level, optname, optval, optlen)

extern int obsd_ioctl(int dev, unsigned long cmd, char *addr);
#define obsd_pnio_ioctl(dev, cmd, addr) obsd_ioctl(dev, cmd, addr)

extern int obsd_setsockopt(int s, int level, int optname, const void *optval, unsigned int optlen);
#define obsd_pnio_setsockopt(s, level, optname, optval, optlen) obsd_setsockopt(s, level, optname, optval, optlen)

extern int obsd_getsockname(int s, struct obsd_pnio_sockaddr* address, unsigned int *length);
#define obsd_pnio_getsockname(s, address, length) obsd_getsockname(s, address, length)

extern int  obsd_select(int nfds, struct obsd_pnio_fd_set *my_readfds, struct obsd_pnio_fd_set *my_writefds, struct obsd_pnio_fd_set *my_exceptfds, struct obsd_pnio_timeval *my_timeout);
#define obsd_pnio_select(nfds, my_readfds, my_writefds, my_exceptfds, my_timeout) obsd_select(nfds, my_readfds, my_writefds, my_exceptfds, my_timeout)

extern int obsd_socket(int af, int type, int protocol);
#define socket obsd_socket

extern int obsd_listen(int s, int backlog);
#define listen obsd_listen

extern int obsd_close(int fildes);
#define close obsd_close

#endif

#ifndef SOCK_LOW_H
#define SOCK_LOW_H
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
/*  C o m p o n e n t     &C: SOCK (SOCKet interface)                   :C&  */
/*                                                                           */
/*  F i l e               &F: sock_low.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/* interface implemented by sock_xxx module of configured IP Stack           */
/*****************************************************************************/

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*=============================================================================
// IP stack dependent definition of FD_SETSIZE and SOCK_CFG_MAX_SOCKETS
// ==========================================================================*/

#if SOCK_CFG_OPEN_BSD

#include "tcip_obsd_cfg.h"
#include "obsd_kernel_usr.h"

#ifdef FD_SETSIZE
#error "FD_SETSIZE must not be set"
#endif
#ifdef SOCK_CFG_MAX_SOCKETS
#error SOCK_CFG_MAX_SOCKETS must not be defined!
#endif
#if TCIP_CFG_MAX_SOCKETS < 2
#error TCIP_CFG_MAX_SOCKETS must be greater than or equal to 2!
#endif
#define SOCK_CFG_MAX_SOCKETS TCIP_CFG_MAX_SOCKETS
#define FD_SETSIZE TCIP_CFG_MAX_SOCKETS

#ifdef OBSD_SOCKET_BUFFER_FILL_HISTROGRAM
LSA_BOOL
SockIF_GetSockBufHistogram(
	unsigned int sock_fd, /* in */
	struct obsd_pnio_sb_fill_histogram* histogram, /* out */
	LSA_INT* err /* sanity */
);
#endif /* OBSD_SOCKET_BUFFER_FILL_HISTROGRAM */

#endif

/*===========================================================================*/

#if SOCK_CFG_INTERNICHE

#ifdef SOCK_CFG_MAX_SOCKETS
#error SOCK_CFG_MAX_SOCKETS must not be defined!
#endif
#if TCIP_CFG_MAX_SOCKETS < 2
#error TCIP_CFG_MAX_SOCKETS must be greater than or equal to 2!
#endif
#define SOCK_CFG_MAX_SOCKETS TCIP_CFG_MAX_SOCKETS /* see ipport.h: #define FD_SETSIZE TCIP_CFG_MAX_SOCKETS */

#endif

/*===========================================================================*/

#if SOCK_CFG_WINSOCK

#ifdef FD_SETSIZE
#error FD_SETSIZE must not be defined!
#endif
#ifndef SOCK_CFG_MAX_SOCKETS
#error SOCK_CFG_MAX_SOCKETS must be defined!
#endif
#define FD_SETSIZE SOCK_CFG_MAX_SOCKETS /* see MSDN: winsock select() function */

#endif

/*===========================================================================*/

#if SOCK_CFG_CUSTOM

#ifndef SOCK_CFG_MAX_SOCKETS
#error SOCK_CFG_MAX_SOCKETS must be defined!
#endif

#endif

/*=============================================================================
// IP stack independent definitions
// ==========================================================================*/

#define SOCK_LOW_STREAM 1
#define SOCK_LOW_DGRAM  2

#define SOCK_LOW_SOCKET_ERROR -1

#define SOCK_LOW_PEEK 0x1

typedef LSA_UINT64 SOCK_LOW_HANDLE_TYPE; /* for all possible handle sizes up to 64 bit */

#define SOCK_LOW_INVALID_HANDLE ((SOCK_LOW_HANDLE_TYPE)-1)

#define SOCK_LOW_MAX(_fd1, _fd2) if ((_fd2) > (_fd1)) { (_fd1) = (_fd2);}


/*-----------------------------------------------------------------------------
// Option types for sock_low_getsockopt(), sock_low_setsockopt()
// --------------------------------------------------------------------------*/
typedef enum
{
	sock_low_sockopt_Type,
	sock_low_sockopt_Error,
	sock_low_sockopt_SendBuffer,
	sock_low_sockopt_RecvBuffer,
	sock_low_sockopt_NonBlocking,
	sock_low_sockopt_ReuseAddr,
	sock_low_sockopt_KeepAlive,
	sock_low_sockopt_DontRoute,
	sock_low_sockopt_Linger,
	sock_low_sockopt_Broadcast,
	sock_low_sockopt_TCPNoDelay,
	sock_low_sockopt_TCPNoAckDelay,
	sock_low_sockopt_OOBInline,
	sock_low_sockopt_IPRecvdStAddr
#if SOCK_CFG_WINSOCK
	,sock_low_sockopt_Connreset
#endif
} SOCK_LOW_SOCKOPT_TYPE;

/*=============================================================================
// sock_low_ interface declarations
// ==========================================================================*/

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_init
// --------------------------------------------------------------------------*/
/*!
\descr  The function makes all IP stack specific initializations and has to be
        called before all other sock_low_ ... functions.
-----------------------------------------------------------------------------*/
void
sock_low_init(void);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_socket
// --------------------------------------------------------------------------*/
/*!
\param  [in] type
        #SOCK_LOW_STREAM for TCP or #SOCK_LOW_DGRAM for UDP sockets
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval lower handle if successful or SOCK_LOW_INVALID_HANDLE;
        *err contains native error code if system call failed

\descr  The function creates an unbound socket and returns a lower handle
        containing the socket descriptor of the cofigured stack.
-----------------------------------------------------------------------------*/
SOCK_LOW_HANDLE_TYPE
sock_low_socket(
	int type,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_bind
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] address
        Points to a #SOCK_SOCKADDR_TYPE structure containing the address to be
        bound to the socket. The length and format of the address depend on
        the address family of the socket.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function assigns an address to an unnamed socket. Sockets created
        with sock_low_socket() are initially unnamed
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_listen
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] backlog
        Maximum length of pending connections queue.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes the listen() system call on the socket 'lower_handle' and passes
        the backlog parameter
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int backlog,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_connect
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] address
        Points to a #SOCK_SOCKADDR_TYPE structure containing the peer address.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes the connect() system call on the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_connect(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_accept
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] address
        pointer to a #SOCK_SOCKADDR_TYPE structure receiving the address of the
        new socket.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\return The socket descriptor or #SOCK_LOW_INVALID_FD if an error occurred.

\descr  Invokes the accept() system call on the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
SOCK_LOW_HANDLE_TYPE
sock_low_accept(
	SOCK_LOW_HANDLE_TYPE listening_sock_fd,
	SOCK_SOCKADDR_TYPE* address,
	int* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_close
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\retval SOCK_RSP_OK if the function was successful,
        otherwise SOCK_RSP_ERR_LOC_NET.

\descr  The function closes the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_send
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] buffer
        Points to the buffer containing the message to send.
\param  [in] length
        Specifies the length of the message in bytes.
\param  [out] *sent
        Points to int receiving the number of bytes sent
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes the send() system call on the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_send(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int* sent,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_recv
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] buffer
        Pointer to buffer for received data.
\param  [in] length
        Length of the buffer in bytes.
\param  [in] flags
        If flags contains SOCK_LOW_PEEK, data is copied into the buffer but not removed from the input
        queue.
\param  [out] *received
        pointer to the length of the message in bytes. If no data is available
        and the peer has performed a graceful shutdown, it returns 0.
        Otherwise, #SOCK_LOW_SOCKET_ERROR is returned.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes the recv() system call on the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_recv(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	int* received,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_sendto
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] buffer
        Points to the buffer containing the message to send.
\param  [in] length
        Length of the message in bytes.
\param  [in] address
        Points to a #SOCK_SOCKADDR_TYPE structure containing the destination
        address. Can be zero for connected udp sockets.
\param  [in] local_address
        Points to a #SOCK_SOCKADDR_TYPE structure containing the IP address
        which shall be used as source address of the outgoing IP
        packet. The IP address received with the local_address parameter of the
        sock_low_recv_from() function must be used by UDP server applications
        which bind to ANY.
        Linux IP Stacks can set the IP_PKTINFO socket option and implement
        this Function using sendmsg(). The Winsock implementation can use the
        same option and implement this function using WSASendMsg().
        IP Stacks which do not support setting the src address of the outgoing
        IP packet can ignore the parameter.
\param  [out] *sent
        Points to int receiving the number of bytes sent
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes teh sendto() system call on the socket 'lower_handle'.
        If the socket was connected with the connect() system call before,
        the address parameter may be 0. Data is sent to the connected address
        in this case.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_sendto(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	SOCK_SOCKADDR_TYPE* address,
	SOCK_SOCKADDR_TYPE* local_address,
	int* sent,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_recvfrom
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] buffer
        Points to the buffer containing the message to receive.
\param  [in] length
        Length of the buffer in bytes.
\param  [in] flags
        If flags contains SOCK_LOW_PEEK, data is copied into the buffer but not
        removed from the input queue.
\param  [out] address
        Points to a #SOCK_SOCKADDR_TYPE structure receiving the source address.
\param  [out] local_address
        Points to a #SOCK_SOCKADDR_TYPE structure receiving the destination
        address of the IP packet that conveyed this UDP message.
        For Linux IP stacks, set the IP_PKTINFO socket option and implement
        fhis function using recvmsg(). The Winsock implementation can use the
        same option and the Winsock function WSARecvMsg().
        Implementations for IP Stacks not supporting the retrieval of the
        destination address shall fill the structure with 0.
\param  [out] *received
        pointer to the length of the received message in bytes. If no data is available
        and the peer has performed a graceful shutdown, it returns 0.
        Otherwise, #SOCK_LOW_SOCKET_ERROR is returned
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  Invokes the recvfrom() system call on the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_recvfrom(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	SOCK_SOCKADDR_TYPE* address,
	SOCK_SOCKADDR_TYPE* local_address,
	int* received,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_get_socket_error
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] socket_error
        The address of variable receiving the socket error.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval LSA_TRUE if the function was successful, LSA_FALSE in case of an error.

\descr  The function retrieves the value for the SO_ERROR option
        for the socket 'lower_handle'.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_setsockopt
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] option_name
        A value of the SOCK_LOW_SOCKOPT_TYPE enumeration type.
\param  [in] option_value
        The address of an option specific variable containing the new option
        value.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval LSA_TRUE if the function was successful, LSA_FALSE in case of an error.

\descr  Invokes setsockopt() with 'option_name' and option_value
        for the socket 'lower_handle'.'option_name' is mapped to the
        configured stack's specific option name.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_setsockopt(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_LOW_SOCKOPT_TYPE option_name,
	void* option_value,
	int* err
);

#if SOCK_CFG_INTERNICHE
/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_set_multicast_sockopts
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] IfIndex
        Interface index of interface for outgoing multicast traffic.
\param  [in] group_addr
        Multicast group address in network byte order.
\param  [in] ip_multicast_ttl
        TTL number of sent multicast packets (see documentation for details)
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function sets the necessary multicast options for a multicast enabled
        socket. See documentation of opcode SOCK_OPC_UDP_MULTICAST_OPEN.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_set_multicast_sockopts(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_UINT32 IfIndex,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
);
#endif

#if SOCK_CFG_OPEN_BSD
/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_set_multicast_sockopts
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] interface_addr
        ipv4 address of interface for outgoing multicast traffic.
\param  [in] group_addr
        Multicast group address in network byte order.
\param  [in] ip_multicast_ttl
        TTL number of sent multicast packets (see documentation for details)
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function sets the necessary multicast options for a multicast enabled
        socket. See documentation of opcode SOCK_OPC_UDP_MULTICAST_OPEN.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_set_multicast_sockopts(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_UINT32 interface_addr,
	LSA_UINT32 group_addr,
	LSA_UINT8 ip_multicast_ttl,
	int *err
);
#endif

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_get_sockname
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] address
        Pointer to a #SOCK_SOCKADDR_TYPE structure where the local address of the
        socket will be returned.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\retval SOCK_RSP_OK if successful or SOCK_RSP_ERR_ code;
        *err contains native error code in this case

\descr  The function retrieves the locally-bound name of the specified socket
        and stores this address in the LD_SOCKADDR structure pointed to by the
        address argument.
        If the socket has not been bound to a local name, the value stored in
        the object pointed to by address is unspecified.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_get_sockname(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_select
// --------------------------------------------------------------------------*/
/*!
\param  [in] timeout
        The timeout interval in milliseconds.

\return Upon successful completion, the function returns the number of socket
        descriptors in the result sets. On err, the function returns
        SOCK_LOW_SOCKET_ERROR.

\descr  Invokes the select() system call and passes the timeout
        parameter. The socket descriptor sets needed must have been prepared
        previously using sock_low_readset_insert(), sock_low_writeset_insert()
        and sock_low_exceptset_insert().

\note   On Win32 systems a select() with no specifed file descriptor returns
        immediately with an error code instead of sleeping the specified time.
-----------------------------------------------------------------------------*/
LSA_INT32
sock_low_select(
	int timeout
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_readset_insert
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Inserts lower_handle into the readSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_VOID
sock_low_readset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_writeset_insert
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Inserts lower_handle into the writeSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_VOID
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_exceptset_insert
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Inserts lower_handle into the exceptSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_VOID
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_in_readset
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Returns LSA_TRUE if lower_handle is in the readSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_in_writeset
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Returns LSA_TRUE if lower_handle is in the writeSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_in_exceptset
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.

\descr  Returns LSA_TRUE if lower_handle is in the exceptSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_clear_all_sets
// --------------------------------------------------------------------------*/
/*!
\param  [in] LSA_VOID

\descr  Clears readSocketDescriptorSet, writeSocketDescriptorSet and
        exceptSocketDescriptorSet.
-----------------------------------------------------------------------------*/
LSA_VOID
sock_low_clear_all_sets(
	LSA_VOID
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_map_connect_error
// --------------------------------------------------------------------------*/
/*!
\param  [in] error
        Native error code returned by connect().

\return SOCK response code.

\descr  Maps the stack specific error code to a SOCK response code.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_map_connect_error(
	int error
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_map_tcp_error
// --------------------------------------------------------------------------*/
/*!
\param  [in] error
        Native error code returned by send() or receive().

\return SOCK response code.

\descr  Maps the stack specific error code to a SOCK response code.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_map_tcp_error(
	int error
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_set_custom_option
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [in] sock_socket_ptr
        pointer to SOCK_SOCKET structure containing SOCK's context information
        for socket sock_fd
\return LSA_FALSE, if custom option cannot be set,
		otherwise LSA_TRUE

\descr  store context pointer to SOCK's socket data.
        Remark: The function is used in event-driven mode only.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_set_custom_option(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	void* sock_socket_ptr,
	void (*event_callback_fct)(int, void*)
);

/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_has_socket_error
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] socket_error
        The address of variable receiving the socket error.
\param  [out] *err
        Points to int receiving the native result code of getsockopt()
         if not successful.
\return LSA_TRUE, if socket has error not equal to EWOULDBLOCK and EINPROGRESS,
        otherwise LSA_FALSE

\descr  retrieves error stored in socket and examines the error code. If an
		error code different than EWOULDBLOCK or EINPROGRESS is found, an error
		condition is assumed and the function returns LSA_TRUE. The error
		EWOULDBLOCK indicates the blocking condition for sockets working with
		non-blocking IO. EINPROGRESS is returned by the connect system call on
		sockets with non-blocking IO when a connect is already pending.
		Therefore, these error codes do not indicate an error condition.
		Remark: The function is used in event-driven mode only.
-----------------------------------------------------------------------------*/
LSA_BOOL
sock_low_has_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
    int* err
);

#if SOCK_CFG_OPEN_BSD
/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_abort_tcp_session
// --------------------------------------------------------------------------*/
/*!
\param  [in] lower_handle
        Configured stack's native socket handle.
\param  [out] *err
        Points to int receiving the native result code if not successful.

\descr  close tcp socket lower_handle refers to.
        Remark: The function is used by OpenBSD configuration only.
-----------------------------------------------------------------------------*/
LSA_RESPONSE_TYPE
sock_low_abort_tcp_session(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int *err
);
#endif

#if SOCK_CFG_OPEN_BSD || SOCK_CFG_INTERNICHE
/*-----------------------------------------------------------------------------
// ::FUNCTION:sock_low_get_reason_code
// --------------------------------------------------------------------------*/
/*!
\param  [in] err
        Configured stack's native result code
\return SOCK_IPSTACK_EMSGSIZE, if result code is EMSGSIZE,
        SOCK_IPSTACK_EHOSTUNREACH if result code is EHOSTUNREACH (BSD)
        or ENP_NO_ROUTE(Interniche), otherwise 0
// --------------------------------------------------------------------------*/
LSA_UINT8
sock_low_get_reason_code(
	int err
);
#endif

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
#endif  /* of SOCK_LOW_H */

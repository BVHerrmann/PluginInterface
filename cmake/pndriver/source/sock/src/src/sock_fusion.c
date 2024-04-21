/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: sock_fusion.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*  Portability layer for socket access                                      */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID 9
#define SOCK_MODULE_ID    9

#include "sock_int.h"

SOCK_FILE_SYSTEM_EXTENSION(SOCK_MODULE_ID)

/*****************************************************************************/
#if SOCK_CFG_CUSTOM
/*****************************************************************************/

#include <memory.h>

#include "fns_fnsproto.h"
#include "fns_bsd_in.h"
#include "fns_ftype.h"

/*------------------------------------------------------------------------------
// checks
//----------------------------------------------------------------------------*/
#if SOCK_AF_INET != AF_INET /* AP01145901 */
#error SOCK_AF_INET differs from AF_INET
#endif

#if SOCK_CFG_MAX_SOCKETS > FD_SETSIZE
#error FD_SETSIZE too small and override not possible
#endif

/*------------------------------------------------------------------------------
// Fusion Stack's socket descriptor type
//----------------------------------------------------------------------------*/
typedef int SOCK_LOW_FD_TYPE;


/* adaption of the special error code handling of the FUSION stack
 * to have a nearly socket-API style interface
 */

/* maximum number of sockets the adaption can handle
 */

#define MAX_SET_SIZE     512

 /* some intermediate structure for linger option */
typedef struct linger
{
	LSA_INT l_onoff ;
	LSA_INT l_linger ;
} linger_t ;

/* as FUSION has a special way of handling fd_set here an intermediate
	* handling is set up.
	* NOTE: use always multiples of 8 for max set size !!!!
	*/
#define MAX_FD_SET_SIZE 512
typedef struct
{
	unsigned char fds_bits [ MAX_FD_SET_SIZE / 8 ] ;
} f_fd_set ;

/*------------------------------------------------------------------------------
// native socket descriptor sets, see sock_low_select
//----------------------------------------------------------------------------*/
static f_fd_set readSocketDescriptorSet;
static f_fd_set writeSocketDescriptorSet;
static f_fd_set exceptSocketDescriptorSet;

static SOCK_LOW_FD_TYPE maxfds;

#define SOCKIF_FD_SET_TYPE           f_fd_set
#define SOCKIF_FD_ZERO(s1)           FD_ZERO(s1)
#define SOCKIF_FD_SET(s1,s2)         FD_SET(s1,s2)
#define SOCKIF_FD_ISSET(s1,s2)       FD_ISSET(s1,s2)

/* for some strange reasons the following is defined at several
	* places, so undefine the stuff, before defining it as needed.
	*/
#undef FD_SET
#undef FD_CLR
#undef FD_ISSET
#undef FD_ZERO
#define FD_SET(n, p)                 ((p)->fds_bits[(n)/8] |= (1 << ((n) % 8)))
#define FD_CLR(n, p)                 ((p)->fds_bits[(n)/8] &= ~(1 << ((n) % 8)))
#define FD_ISSET(n, p)               ((p)->fds_bits[(n)/8] & (1 << ((n) % 8)))
#define FD_ZERO(p)                   memset((char *)(p), 0, sizeof(*(p)))

/* as FUSION has some special features for error codes and select
	* handling, some intermediate functions are introduced.
	*/
struct timeval ;
void f_init ( void ) ;
int f_accept( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
int f_bind ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
int f_close ( int fd ) ;
int f_connect ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
int f_errno ( int fd ) ;
int f_getpeername ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
int f_getsockname ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
int f_getsockopt ( int fd, int level, int optname, char *optval, int optlen ) ;
int f_listen ( int fd, int backlog ) ;
int f_recv ( int fd, char *buffer, int length, int flags ) ;
int f_recvfrom( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int *addr_len ) ;
int f_select ( int nfd, f_fd_set *rset, f_fd_set *wset, f_fd_set *eset, struct timeval *wait ) ;
int f_send ( int fd, char *buffer, int length, int flags ) ;
int f_sendto ( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int addr_len ) ;
int f_setsockopt ( int fd, int level, int optname, char *optval, int optlen ) ;
int f_shutdown ( int fd, int how ) ;
int f_socket ( int family, int type, int protocol ) ;

/* there are some socket options unknown to FUSION. These are simulated
	* in the get/setsockopt intermediate routines. But the socket options
	* have to be defined here.
	*/
#ifndef SO_TYPE /* type of socket */
	#define SO_TYPE        0x1001
#endif
#ifndef SO_ERROR /* last error of socket */
	#define SO_ERROR       0x1002
#endif
#ifndef SO_SNDBUF /* send buffer size of socket */
	#define SO_SNDBUF      0x1003
#endif
#ifndef SO_RCVBUF /* receive buffer size of socket */
	#define SO_RCVBUF      0x1004
#endif
#ifndef SO_NONBLOCK /* blocking/non-blocking mode of socket */
	#define SO_NONBLOCK    0x1005
#endif
#ifndef SO_BROADCAST /* broadcast ability of socket */
	#define SO_BROADCAST   0x1006
#endif

struct
{
	/* storage to save error results and the connection type
	 */
	struct
	{
		int type ;
		int error ;
	} sockets [ MAX_SET_SIZE ] ;
	/* data structure to set up the interest in notification info
	 * as nselect needs it.
	 */

	struct sel sel_array [ MAX_SET_SIZE ] ;

	/* data structure to keep pending notifications until they are
	 * really delivered.
	 */
	unsigned int n_pending ;
	f_fd_set pending_rset ;
	f_fd_set pending_wset ;
	f_fd_set pending_eset ;
	/* read-, write- and except-sets used for a currently pending
	 * select. Contents is valid only for f_select_pending != 0
	 */
	f_fd_set f_act_rset ;
	f_fd_set f_act_wset ;
	f_fd_set f_act_eset ;
	/* flag to indicate that we are in the loop breaker handling
	 */
	int f_break_flag ;
	/* flag to indicate that we are in the select callback routine
	 */
	int f_callback_flag ;
	/* flag to indicate that there is at least one select pending
	 * at FUSION.
	 */
	int f_select_pending ;
} f_data ;



/* ---------------------------- */

/* function name:  f_init:
 *
 * Funktion:
 * This routine is to be called from sock-if initialization. The
 * routine initializes the helper data required for the adaption
 * from "standard" select to FUSION nselect without select polling.
 *
 * parameters:
 *   none
 *
 * result:
 *   void
 */
void f_init ( void )

{
	/* the only thing to do is to initialize the helper data structures
	 * of the adaption.
	 */
	memset ( &f_data, 0, sizeof(f_data) );
}

/* ---------------------------- */

int f_errno ( int fd )
{
	return ( f_data.sockets[fd].error );
}

/* ---------------------------- */

int f_socket ( int family, int type, int protocol )
{
	int result, error ;

	result = fns_socket ( family, type, protocol, &error);
	if ( result < 0 )
	{
		f_data.sockets[0].type = type;
		f_data.sockets[0].error = error;
	}
	else
	{
		f_data.sockets[result].type = type;
		f_data.sockets[result].error = error;
	}

	return ( result );
}

/* ---------------------------- */

int f_listen ( int fd, int backlog )
{
	int result ;

	result = fns_listen ( fd, backlog );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_shutdown ( int fd, int how )
{
	int result;

	result = fns_shutdown ( fd, how );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_close ( int fd )
{
	int result;

	result = fns_close ( fd );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_send ( int fd, char *buffer, int length, int flags )
{
	int result, error;

	result = fns_send ( fd, buffer, length, flags, &error );
	if ( result < 0 )
	{
		f_data.sockets[fd].error = error;
	}
	else
	{
		f_data.sockets[fd].error = 0;
	}

	return ( result );
}

/* ---------------------------- */

int f_recv ( int fd, char *buffer, int length, int flags )
{
	int result, error ;

	result = fns_recv ( fd, buffer, length, flags, &error );
	if ( result < 0 )
	{
		f_data.sockets[fd].error = error;
	}
	else
	{
		f_data.sockets[fd].error = 0;
	}

	return ( result );
}

/* ---------------------------- */

int f_bind ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len )
{
	int result;

	result = fns_bind ( fd, (saddr *) address, addr_len );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_connect ( int fd, SOCK_SOCKADDR_TYPE *address, int addr_len )
{
	int result ;

	result = fns_connect ( fd, (saddr *) address, addr_len );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_accept( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len )
{
	int result, error ;

	result = fns_accept ( fd, (saddr *) address, addr_len, &error );
	if ( result < 0 )
	{
		f_data.sockets[fd].error = error;
	}
	else
	{
		f_data.sockets[fd].error = 0;
		/* clone the type of the listen socket to the newly created socket
		 */
		f_data.sockets[result].type = f_data.sockets[fd].type;
	}

	return ( result );
}

/* ---------------------------- */

int f_sendto ( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int addr_len )
{
	int result, error;

#ifdef IP_ACL
	aclInsertAddrToPnioArea ( address->sin_addr.S_un.S_addr ) ;
#endif

	result = fns_sendto ( fd, buffer, length, flags, (saddr *) address, addr_len, &error );
	if ( result < 0 )
	{
		f_data.sockets[fd].error = error;
	}
	else
	{
		f_data.sockets[fd].error = 0;
	}

	return ( result );
}

/* ---------------------------- */

int f_recvfrom( int fd, char *buffer, int length, int flags, SOCK_SOCKADDR_TYPE *address, int *addr_len )
{
	int result, error;

	result = fns_recvfrom ( fd, buffer, length, flags, (saddr *) address, addr_len, &error );

	if ( result < 0 )
	{
		f_data.sockets[fd].error = error;
	}
	else
	{
		f_data.sockets[fd].error = 0;
	}

	return ( result );
}

/* ---------------------------- */

int f_getsockopt ( int fd, int level, int optname, char *optval, int optlen )
{
	int result;

	/* filter options unknown to FUSION
	 */
	if ( level == SOL_SOCKET )
	{
		switch ( optname )
		{
		case SO_TYPE:
			*((int *) optval) = f_data.sockets[fd].type;
			return ( 0 );
		case SO_ERROR:
			*((int *) optval) = f_data.sockets[fd].error;
			return ( 0 );
		case SO_SNDBUF:
			*((int *) optval) = 536;
			return ( 0 ) ;
		case SO_RCVBUF:
			*((int *) optval) = 536;
			return ( 0 ) ;
		}
	}
	/* pass all remaning options to FUSION
	 */
	result = getsopt ( fd, level, optname, optval, &optlen );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_setsockopt ( int fd, int level, int optname, char *optval, int optlen )
{
	int result;

	/* filter all options unknown to FUSION
	 */
	if ( level == SOL_SOCKET )
	{
		switch ( optname )
		{
		case SO_NONBLOCK:
			nonblocking ( fd );
			return ( 0 ) ;
		case SO_SNDBUF:
		case SO_RCVBUF:
		case SO_BROADCAST:
			return ( 0 );
		}
	}
	/* pass all remaning options to FUSION
	 */
	result = setsopt ( fd, level, optname, optval, optlen );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_getsockname ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len )
{
	int result ;

	result = fns_getsockname ( fd, (saddr *) address, addr_len );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}

/* ---------------------------- */

int f_getpeername ( int fd, SOCK_SOCKADDR_TYPE *address, int *addr_len )
{
	int result ;

	result = fns_getpeername ( fd, (saddr *) address, addr_len );
	if ( result != 0 )
	{
		f_data.sockets[fd].error = result;
		result = -1;
	}

	return ( result );
}


/* --------------------------------------------------------------- */
/* modified select handling:
 * as FUSION offers a possibility to have a callback on pending
 * notifications, there is the need to adapt the "standard" select
 * to this special feature. This will make the polling for
 * notifications of the original LSA-SOCK obsolete. To avoid
 * modifications in the original delivery files of LSA-SOCK
 * this adaption shall make the nselect look like a select.
 */

/* ---------------------------- */

/* function name:  socks_nselect:
 *
 * Funktion:
 * This routine is to be called from the main loop of the GSI task.
 *
 * parameters:
 *   none
 *
 * result:
 *   void
 */
LSA_VOID socks_nselect ( void )
{
	SOCK_ENTER();

	/* set the loop breaker flag
	 */
	f_data.f_break_flag++;

	/* start processing of the pending notifications
	 */

	sock_socket_execute_select(0);

	/* reset the loop breaker flag
	 */
	f_data.f_break_flag = 0 ;

	SOCK_EXIT();
}

/* ---------------------------- */

/* function name:  f_nselect_callback:
 *
 * Funktion:
 * This routine is called by the FUSION stack whenever an event is to be
 * notified. The parameters passed to this routine reflect the parameters
 * of the nselect call as if the nselect would have returned.
 *
 * parameters:
 *   selp         - pointer to the select array
 *   cnt          - number of elements in the select array
 *   residual     - remaining time until timeout
 *   args         - argument pointer given to the nselect call
 *
 * result:
 *   LSA_TRUE         - the selection array will be reregistered for further use
 *   LSA_FALSE        - the selection array will not be registered again
 */
int f_nselect_callback (
	struct sel *selp,
	int cnt,
	u32 residual,
	void *args
) {
	int i, reregister;

	{(void)(args);}
	{(void)(residual);}

	/* by default, reregister the notification selection
	 */
	reregister = LSA_TRUE;

	/* set the callback flag
	 */
	f_data.f_callback_flag++;

	/* extract the notification given and setup the pending notifications
	 * information.
	 */
	for ( i = 0 ; i < cnt ; i++ )
	{
		/* scan the select structure and convert into the pending sets
		 */
		if ( selp->se_outflags & (READ_NOTIFY | ACCEPT_NOTIFY | RSHUTDOWN_NOTIFY | CLOSE_NOTIFY) )
		{
			FD_SET(selp->se_fd,&(f_data.pending_rset));
			f_data.n_pending++ ;
		}
		if ( selp->se_outflags & (WRITE_NOTIFY | SENDQEMPTY_NOTIFY | CONNECT_NOTIFY | WSHUTDOWN_NOTIFY) )
		{
			FD_SET(selp->se_fd,&(f_data.pending_wset));
			f_data.n_pending++ ;
		}
		if ( selp->se_outflags & (EXCEPT_NOTIFY) )
		{
			FD_SET(selp->se_fd,&(f_data.pending_eset));
			f_data.n_pending++ ;
		}
		selp++ ;
	} /* for i */

	/* start processing of the pending notifications
	 */
	sock_socket_execute_select ( 0 );

	/* reset callback flag
	 */
	f_data.f_callback_flag = 0;

	/* to avoid loops like nselect is called from callback and directly calling
	 * callback again, there should be no reregistering
	 */
	reregister = LSA_FALSE;

	/* reset the select pending flag, as without reregistering the select causing the
	 * callback will no longer be pending after returning from this call to FUSION.
	 */
	f_data.f_select_pending = 0;

	/* all done, return
	 */
	return( reregister );
}

/* ---------------------------- */

/* function name:  f_select:
 *
 * Function:
 * This routine does the adaption from the standard-like select call
 * to the FUSION nselect. This is required to get the callback feature
 * of the FUSION nselect, which avoids the active polling of the
 * original LSA SOCK design. The routine shall be able to handle
 * pending notifications which are delivered by the callback routine.
 * Furthermore the loop "nselect - callback - nselect - callback - ..."
 * shall be avoided.
 *
 * parameters:
 *   nfd          - the number of active sockets in the select sets
 *   rset         - select set for read notifications
 *   wset         - select set for write notifications
 *   eset         - select set for exception/error notifications
 *   wait         - timeout for the select call if blocking
 *
 * result:
 *   <0           - error occured
 *   >=0          - number of notifications
 */
int f_select (
	int nfd,
	f_fd_set *rset,
	f_fd_set *wset,
	f_fd_set *eset,
	struct timeval *wait
) {
	int result, error, i, j, cnt;
	u32 tval;
	struct sel *sep;

	{(void)(wait);}

	/* first some sanity checking
	 */
	if ( (nfd < 0) || (nfd > MAX_SET_SIZE) )
	{
		f_data.sockets[0].error = fns_EINVAL;
		return ( -1 );
	} /* invalid parameter */

	/* check if the routine is called from a process pending notifications
	 * routine. In this case there shall be no new nselect call to FUSION.
	 */
	if ( f_data.f_callback_flag )
	{
		result = 0 ;
		if ( f_data.n_pending )
		{
			/* check for notifications set in the callback, move them to the
			 * appropriate sets of the routine and clear the notifications
			 * from the pending notifications
			 */
			for ( i = 0 ; i < nfd ; i++ )
			{
				/* check if the indexed socket has pending notifications, update
				 * the sets accordingly.
				 */
				if ( rset && FD_ISSET(i,rset))
				{
					/* check if there is a pending notification for the indexed
					 * socket
					 */
					if ( FD_ISSET(i,&(f_data.pending_rset)) )
					{
						/* a notification is pending, update sets and counters
						 */
						FD_CLR(i,&(f_data.pending_rset));
						FD_SET(i,rset);
						f_data.n_pending--;
						result++;
					} /* bit is set */
					else
					{
						/* there is no notification pending, so the bit is to be
						 * resetted.
						 */
						FD_CLR(i,rset);
					} /* bit is not set */
				} /* check rset */
				if ( wset && FD_ISSET(i,wset))
				{
					/* check if there is a pending notification for the indexed
					 * socket
					 */
					if ( FD_ISSET(i,&(f_data.pending_wset)) )
					{
						/* a notification is pending, update sets and counters
						 */
						FD_CLR(i,&(f_data.pending_wset)) ;
						FD_SET(i,wset);
						f_data.n_pending--;
						result++;
					} /* bit is set */
					else
					{
						/* there is no notification pending, so the bit is to be
						 * resetted.
						 */
						FD_CLR(i,wset);
					} /* bit is not set */
				} /* check wset */
				if ( eset && FD_ISSET(i,eset))
				{
					/* check if there is a pending notification for the indexed
					 * socket
					 */
					if ( FD_ISSET(i,&(f_data.pending_eset)))
					{
						/* a notification is pending, update sets and counters
						 */
						FD_CLR(i,&(f_data.pending_eset));
						FD_SET(i,eset);
						f_data.n_pending--;
						result++;
					} /* bit is set */
					else
					{
						/* there is no notification pending, so the bit is to be
						 * resetted.
						 */

						FD_CLR(i,eset);
					} /* bit is not set */
				} /* check eset */
			} /* for i */
		} /* process pending notifications */
		/* indicate pending notifications, do no new nselect
		 */
		return ( result );
	} /* called for getting pending notifications */


	/* check for identical select calls which will result in multiple identical select
	 * calls to FUSION. Here the currently active read-, write- and except-sets
	 * are compared with the parameters of the actual call.
	 */
	if ( f_data.f_select_pending )
	{
		/* check if the sets are available and identical to the stored ones
		 */
		result = 0 ;
		if ( rset && (memcmp ( &(f_data.f_act_rset), rset, sizeof(f_fd_set) ) == 0) ) result++;
		if ( wset && (memcmp ( &(f_data.f_act_wset), wset, sizeof(f_fd_set) ) == 0) ) result++;
		if ( eset && (memcmp ( &(f_data.f_act_eset), eset, sizeof(f_fd_set) ) == 0) ) result++;
		/* if all sets are identical, no new select is required, as it would be
		 * identical to the select before.
		 */
		if ( result >= 3 )return ( 0 );
	} /* there are valid actual sets at all */

	/* copy parameter sets to stored actual sets, if no set is given as parameter
	 * the stored actual set is reset.
	 */
	if ( rset )
	{
		memcpy ( &(f_data.f_act_rset), rset, sizeof(f_fd_set) );
	}
	else
	{
		memset ( &(f_data.f_act_rset),0 , sizeof(f_fd_set) );
	}
	if ( wset )
	{
		memcpy ( &(f_data.f_act_wset), wset, sizeof(f_fd_set) );
	}
	else
	{
		memset ( &(f_data.f_act_wset),0 , sizeof(f_fd_set) ) ;
	}
	if ( eset )
	{
		memcpy ( &(f_data.f_act_eset), eset, sizeof(f_fd_set) );
	}
	else
	{
		memset ( &(f_data.f_act_eset),0 , sizeof(f_fd_set) );
	}


	/* there is really the need to do a real nselect call, first set up
	 * the required information on notifications the sockets are
	 * interrested in.
	 */
	sep = &(f_data.sel_array[0]);
	cnt = 0 ;
	for ( i = 0 ; i < nfd ; i++ )
	{
		/* build up the notifications the caller is interrested in
		 */
		sep->se_inflags = 0;
		if ( rset && FD_ISSET(i,rset))
		{
			/* there is a read set and the socket is interested in notifications
			 */
			sep->se_inflags |= (READ_NOTIFY | ACCEPT_NOTIFY | RSHUTDOWN_NOTIFY | CLOSE_NOTIFY);
		}
		if ( wset && FD_ISSET(i,wset))
		{
			/* there is a write set and the socket is interested in notifications
			 */
			sep->se_inflags |= (WRITE_NOTIFY | SENDQEMPTY_NOTIFY | CONNECT_NOTIFY | WSHUTDOWN_NOTIFY);
		}
		if ( eset && FD_ISSET(i,eset))
		{
			/* there is an except set and the socket is interested in notifications
			 */
			sep->se_inflags |= (EXCEPT_NOTIFY);
		}
		/* if there are interesting notifications, set up the rest of the
		 * select slot
		 */
		if ( sep->se_inflags )
		{
			sep->se_fd = i;
			sep->se_outflags = 0;
			sep++;
			cnt++;
		}
	}

	/* FB11.02.10: Given timeout is ignored as it is most time 0.
	 * Instead a fixed value of 10 secs is used.
	 * If no timeout is used and there are connections with no network events an than
	 * a new connection is added and this toggles between connecting and disconnected,
	 * we accumulate each time resources for a new select inside the tcp/ip stack until
	 * there is no network event on the already existing sockets.
	 * See ARTS AP00915842. */
	/*
	if (wait)
		tval = (wait->tv_sec * 1000L) + (wait->tv_usec / 1000L);
	else
	*/
		tval = (u32)10000L; /* 10 secs */

	/* call the FUSION nselect to tell the stack in what we are
	 * interested and where the callback is.
	 * Under certain circumstances here several selects might be
	 * issued. This will speed up changes in the selected notifications,
	 * but will use more resources in FUSION for the selects.
	 */
	result = nselect ( &(f_data.sel_array[0]), /* events to be notified */
					   cnt,                    /* number of events to register */
					   &tval,                  /* timeout of the call */
					   &f_nselect_callback,    /* call back routine */
					   0,                      /* call back arguments */
					   &error );               /* error code */

	/* set the pending flag and store the error information
	 */
	f_data.f_select_pending++;
	f_data.sockets[0].error = error;

	/* if there is an immediate result (notification), convert this information
	 * back to the select sets.
	 */
	for ( i = 0 ; i < nfd ; i++ )
	{
		sep = &(f_data.sel_array[i]) ;
		j = sep->se_fd ;
		if ( rset && !(sep->se_outflags & (READ_NOTIFY | ACCEPT_NOTIFY | RSHUTDOWN_NOTIFY | CLOSE_NOTIFY)))
		{
			/* the socket is interrested in this notification, but there
			 * is nothing to notify, so clear the bit
			 */
			FD_CLR(j,rset);
		}
		if ( wset && !(sep->se_outflags & (WRITE_NOTIFY | SENDQEMPTY_NOTIFY | CONNECT_NOTIFY | WSHUTDOWN_NOTIFY)))
		{
			/* the socket is interrested in this notification, but there
			 * is nothing to notify, so clear the bit
			 */
			FD_CLR(j,wset);
		}
		if ( eset && !(sep->se_outflags & (EXCEPT_NOTIFY)))
		{
			/* the socket is interrested in this notification, but there
			 * is nothing to notify, so clear the bit
			 */
			FD_CLR(j,eset);
		}
	}

	/* return the number of notifications
	 */
	return ( result );
}


/*------------------------------------------------------------------------------
// inline documentation of parameters --> sock_low.h
//----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_init(
	LSA_VOID
) {
	if( sizeof(SOCK_LOW_HANDLE_TYPE) < sizeof(SOCK_LOW_FD_TYPE) ) {
		SOCK_FATAL();
	}

	if( sizeof(SOCK_SOCKADDR_TYPE) != sizeof(struct sockaddr) ) {
		SOCK_FATAL();
	}

	f_init();
}

/* --------------------------------------------------------------- */

SOCK_LOW_HANDLE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_socket(
	int type,
	int* err
) {
	SOCK_LOW_FD_TYPE s = f_socket(AF_INET, (type == SOCK_LOW_STREAM) ? SOCK_STREAM : SOCK_DGRAM, 0);

	if (s < 0)
	{
		*err = errno;

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_bind(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int *err
) {
	if( f_bind((SOCK_LOW_FD_TYPE)lower_handle, (SOCK_SOCKADDR_TYPE*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case fns_EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		case fns_ENOMEM:
		case fns_ENOBUFS:
			return SOCK_RSP_ERR_RESOURCE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_listen(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	LSA_INT32 backlog,
	int* err
) {
	if( f_listen((SOCK_LOW_FD_TYPE)lower_handle, backlog) < 0 )
	{
		*err = errno;

		switch(*err)
		{
		case fns_EADDRINUSE:
			return SOCK_RSP_ERR_ADDR_INUSE;

		default:
			return SOCK_RSP_ERR_LOC_NET;
		}

	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_connect_error(
	int error
) {
	switch(error)
	{
	case fns_EINPROGRESS:
	case fns_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case fns_ENOMEM:
	case fns_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	case fns_ECONNREFUSED:
		return SOCK_RSP_ERR_REM_ABORT;

	case fns_ENETUNREACH:
	case fns_ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_connect(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
) {
	if( f_connect((SOCK_LOW_FD_TYPE)lower_handle, (SOCK_SOCKADDR_TYPE*)address, sizeof(*address)) < 0 )
	{
		*err = errno;

		return sock_low_map_connect_error(*err);
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

SOCK_LOW_HANDLE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_accept(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
	/* user will not see accept errors (ACCEPT_NOTIFY rqb is not returned if accept fails) */
) {
	LSA_INT32 address_len = sizeof(*address);

	SOCK_LOW_FD_TYPE s = f_accept((SOCK_LOW_FD_TYPE)lower_handle, (SOCK_SOCKADDR_TYPE*)address, &address_len);

	if (s < 0)
	{
		*err = errno;

		return SOCK_LOW_INVALID_HANDLE;
	}
	else
	{
		return (SOCK_LOW_HANDLE_TYPE)s;
	}
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_close(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	int ret = f_close((SOCK_LOW_FD_TYPE)lower_handle);

	if (ret < 0)
	{
		return SOCK_RSP_ERR_LOC_NET; /* should never happen */
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_tcp_error(
	int error
) {
	switch(error)
	{
	case fns_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case fns_ECONNABORTED:
	case fns_ECONNRESET:
	case fns_ESHUTDOWN:
		return SOCK_RSP_ERR_REM_ABORT;

	case fns_EPIPE: /* socket is not connected */
	case fns_ETIMEDOUT:
		return SOCK_RSP_ERR_REM_NET;

	case fns_ENOMEM:
	case fns_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_send(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int* sent,
	int* err
) {
	*sent = f_send( (SOCK_LOW_FD_TYPE)lower_handle
				  , buffer
				  , length
				  , MSG_NONBLOCKING
				  );

	if (*sent < 0)
	{
		*err = errno;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_recv(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	int* received,
	int* err
) {
	*received = f_recv( (SOCK_LOW_FD_TYPE)lower_handle
						, buffer
						, length
						, (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
						);

	if (*received < 0)
	{
		*err = errno;

		return sock_low_map_tcp_error(*err);
	}

	return SOCK_RSP_OK;
}

/*----------------------------------------------------------------------------*/

static LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_map_udp_error(
	int err
) {
	switch(err) /* note: no ERR_REM_NET possible */
	{
	case fns_EWOULDBLOCK:
		return SOCK_RSP_OK_ACTIVE;

	case fns_ENOMEM:
	case fns_ENOBUFS:
		return SOCK_RSP_ERR_RESOURCE;

	default:
		return SOCK_RSP_ERR_LOC_NET;
	}
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_sendto(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	SOCK_SOCKADDR_TYPE* address,
	int* sent,
	int* err
) {
	*sent = f_sendto( (SOCK_LOW_FD_TYPE)lower_handle
					  , buffer
					  , length
					  , MSG_NONBLOCKING
					  , (SOCK_SOCKADDR_TYPE*)address
					  , (address) ? sizeof(SOCK_SOCKADDR_TYPE) : 0
					  );

	if (*sent < 0)
	{
		*err = errno;

		return sock_low_map_udp_error(*err);
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_recvfrom(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	char* buffer,
	int length,
	int flags,
	SOCK_SOCKADDR_TYPE* address,
	int* received,
	int* err
) {
	int address_len = sizeof(SOCK_SOCKADDR_TYPE);

	*received = f_recvfrom( (SOCK_LOW_FD_TYPE)lower_handle
							, buffer
							, length
							, (flags & SOCK_LOW_PEEK) ? MSG_PEEK : 0
							, (SOCK_SOCKADDR_TYPE*)address
							, &address_len
							);

	if (*received < 0)
	{
		*err = errno;

		return sock_low_map_udp_error(*err);
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_get_socket_error(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	int* socket_error,
	int* err
) {
	int ret = f_getsockopt((SOCK_LOW_FD_TYPE)lower_handle, SOL_SOCKET, SO_ERROR, (char*)socket_error, sizeof(*socket_error));

	if (ret < 0)
	{
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/* --------------------------------------------------------------- */

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_setsockopt(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_LOW_SOCKOPT_TYPE option_name,
	void* option_value,
	int* err
) {
	SOCK_LOG_FCT("sock_low_setsockopt")

	int ret;
	SOCK_LOW_FD_TYPE s = (SOCK_LOW_FD_TYPE)lower_handle;

	switch (option_name)
	{
	case sock_low_sockopt_NonBlocking:
		ret = f_setsockopt(s, SOL_SOCKET, SO_NONBLOCK, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoDelay:
		ret = f_setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_TCPNoAckDelay:
		ret = 0;
		break;

	case sock_low_sockopt_ReuseAddr:
		ret = f_setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_KeepAlive:
		ret = f_setsockopt(s, SOL_SOCKET, SO_KEEPALIVE, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_DontRoute:
		ret = f_setsockopt(s, SOL_SOCKET, SO_DONTROUTE, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_SendBuffer:
		ret = f_setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_RecvBuffer:
		ret = f_setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_Linger:
		{
		struct linger linger_value;
		linger_value.l_linger = *(int*)option_value;
		linger_value.l_onoff  = 1;
		ret = f_setsockopt(s, SOL_SOCKET, SO_LINGER, (char*)&linger_value, sizeof(linger_value));
		}
		break;

	case sock_low_sockopt_Broadcast:
		ret = f_setsockopt(s, SOL_SOCKET, SO_BROADCAST, (char*)option_value, sizeof(LSA_INT32));
		break;

	case sock_low_sockopt_OOBInline:
		return LSA_TRUE; /* Fusion 5.0 does not support SO_OOBINLINE */
		break;

	default:
		ret = -1; /* prevent compiler warning C4701: potentially uninitialized local variable 'ret' used */

		SOCK_UPPER_TRACE_01(0, LSA_TRACE_LEVEL_FATAL
			, "option_name(%d) not supported by this function"
			, option_name
			);

		SOCK_FATAL();

		break;

	}

	if (ret < 0)
	{
		*err = errno;

		return LSA_FALSE;
	}

	return LSA_TRUE;
}

/* --------------------------------------------------------------- */

LSA_RESPONSE_TYPE SOCK_LOCAL_FCT_ATTR
sock_low_get_sockname(
	SOCK_LOW_HANDLE_TYPE lower_handle,
	SOCK_SOCKADDR_TYPE* address,
	int* err
) {
	int address_len = sizeof(*address);

	if( f_getsockname((SOCK_LOW_FD_TYPE)lower_handle, (SOCK_SOCKADDR_TYPE*)address, &address_len) < 0 )
	{
		*err = errno;

		return SOCK_RSP_ERR_LOC_NET;
	}

	return SOCK_RSP_OK;
}

/* --------------------------------------------------------------- */

LSA_INT32 SOCK_LOCAL_FCT_ATTR
sock_low_select(
	int timeout
) {
	struct timeval tv;

	tv.tv_sec = timeout / 1000;
	tv.tv_usec = (timeout % 1000) * 1000;

	return f_select(maxfds+1
		, &readSocketDescriptorSet
		, &writeSocketDescriptorSet
		, &exceptSocketDescriptorSet
		, &tv);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_readset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_writeset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_exceptset_insert(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	FD_SET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet);
	SOCK_LOW_MAX(maxfds, (SOCK_LOW_FD_TYPE)lower_handle);
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_readset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &readSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_writeset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &writeSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_BOOL SOCK_LOCAL_FCT_ATTR
sock_low_in_exceptset(
	SOCK_LOW_HANDLE_TYPE lower_handle
) {
	return FD_ISSET((SOCK_LOW_FD_TYPE)lower_handle, &exceptSocketDescriptorSet) ? LSA_TRUE : LSA_FALSE;
}

/*----------------------------------------------------------------------------*/

LSA_VOID SOCK_LOCAL_FCT_ATTR
sock_low_clear_all_sets(
	LSA_VOID
) {
	FD_ZERO(&readSocketDescriptorSet);
	FD_ZERO(&writeSocketDescriptorSet);
	FD_ZERO(&exceptSocketDescriptorSet);

	maxfds = 0;
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_UPPER_IN_FCT_ATTR
SOCK_NTOHS(
	LSA_UINT16 parameter
) {
	return ntohs(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_UPPER_IN_FCT_ATTR
SOCK_NTOHL(
	LSA_UINT32 parameter
) {
	return ntohl(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT16 SOCK_UPPER_IN_FCT_ATTR
SOCK_HTONS(
	LSA_UINT16 parameter
) {
	return htons(parameter);
}

/*----------------------------------------------------------------------------*/

LSA_UINT32 SOCK_UPPER_IN_FCT_ATTR
SOCK_HTONL(
	LSA_UINT32 parameter
) {
	return htonl(parameter);
}
/*----------------------------------------------------------------------------*/

#endif

/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

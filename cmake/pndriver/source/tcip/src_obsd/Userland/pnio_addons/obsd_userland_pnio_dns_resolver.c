/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_dns_resolver.c
 *
 * @brief       asynchronous DNS resolver for PN
 * @details     Implementation
 *              note: all functions are nonblocking
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.03
 * @date        created: 23.06.2015
 */
/* -------------------------------------------------------------------------------------------------- */

#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_socket.h>
#include <netinet/obsd_kernel_in.h>
#include <obsd_userland_errno.h>

#include <obsd_userland_netdb.h>
#include <arpa/obsd_userland_inet.h>            /* prototype inet_ntoa()            */
#include <arpa/obsd_userland_nameser.h>         /* MAXDNAME, ..                     */
#include <sys/obsd_kernel_poll.h>

#include <lib/libkern/obsd_kernel_libkern.h>
#include <obsd_userland_string.h>               /* prototype bzero, strlcpy, memmove*/
#include <obsd_userland_userlandport.h>
#include <obsd_kernel_usr.h>
#include <obsd_userland_resolv.h>

#include "obsd_userland_asr.h"                  /* ASR interface                    */
#include "obsd_userland_asr_private.h"          /* prototype asr_resolver()         */
#include "obsd_userland_pnio_dns_resolver.h"    /* own interface                    */


/* ======================== types               ================================================= */
typedef struct 
{
    int  poll_cond;         /* ASR_WANT_READ or ASR_WANT_WRITE */
    int  cur_time;          /* in ms, for timeouts             */
    int  timeout;           /* in ms                           */
    int  fd;
    struct asr_query * obsd_asr_query;
} query_t;


/* ======================== declarations        ================================================= */

static int obsd_dns_fillhostent(const struct hostent *h, struct obsd_pn_dns_hostent *r, char *buf, size_t len);

/* ======================== static variables    ================================================= */

static int obsd_dns_resolver_instance_count = 0;

/* ======================== interface functions ================================================= */

/**
    @brief          Creates a new DNS resolver instance.
    @details        This function is intended to be used before any resolver queries (initialization phase)
    @param[in]      psz_config_string:  config string for the Resolver initialization
                                        See OBSD Manpage for further details:
                    http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man5/resolv.conf.5?query=resolv%2econf
    @param[out]     p_resolver_instance_handle: handle for the newly created resolver instance for use with 
                                                subsequent DNS resolver calls
    @note           The config string must be start with a "!" and each entry has to terminated with a \n
                    Currently supported config entries:
                    @li     nameserver (up to 2 DNS Servers are suppoted)
                    @li     family inet4   (currently only AF INET)
    @note           example:
                    const char * config_string = "!"
                        "nameserver 192.168.10.1 \n"
                        "nameserver [192.168.10.2]:5353\n"  --> address with port other than default
                        ("family inet4\n";                  --> not needed (is the default)
                        ("lookup bind\n" ;                  --> not needed, but possible (is the default)
                        ("search hendrik.lan\n";            --> search Domains are not supported so far)
                        ("options tcp\n";                   --> later maybe)
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_dns_resolver_instance_create(      const char *                        psz_config_string,          /* in  */
                                            obsd_pn_dns_resolver_instance_t*    p_resolver_instance_handle  /* out */
                                     )
{
    void * asr;
    *p_resolver_instance_handle = NULL;

    asr = asr_resolver(psz_config_string);
    if(asr!=NULL)
    {
        *p_resolver_instance_handle = (obsd_pn_dns_resolver_instance_t) asr;
        obsd_dns_resolver_instance_count++;
        return 0;
    }

    return -1;  /* -1 in case of error */
}


/**
    @brief          Prepares a new gethostbyname resolver request for a given DNS resolver instance.
    @details        Creates a neu DNS query and return a  handle for that
    @param[in]      psz_hostname_fqdn:          string with the FQDN to be resolved, e.g. "xxxx.hendrik.lan"
    @param[in]      resolver_instance_handle:   handle for the resolver instance to be used
    @param[out]     p_query_handle:             handle for the newly created query for use with 
                                                subsequent DNS resolver calls
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_dns_resolver_gethostbyname_prepare(const char *                        psz_hostname_fqdn,          /* in  */
                                            obsd_pn_dns_resolver_instance_t     resolver_instance_handle,   /* in  */
                                            obsd_pn_dns_resolver_query_t*       p_query_handle              /* out */
                                           )
{
    if((p_query_handle != NULL) && (resolver_instance_handle != NULL))
    {
        query_t * query = (query_t *)malloc(sizeof(query_t));

        if(query != NULL)
        {
            query->obsd_asr_query = gethostbyname_async(psz_hostname_fqdn, resolver_instance_handle /* asr */);
            if(query->obsd_asr_query != NULL)
            {
                *p_query_handle  = (obsd_pn_dns_resolver_query_t) query;
                query->poll_cond = ASR_WANT_WRITE;                  /* assume that the first asr_run will write */
                query->timeout   = 1000;                            /* don't know better until first call of obsd_dns_resolver_gethostbyname_run */
                query->cur_time  = 0; 
                query->fd        = -1;
                return 0;
            }
        }
    }

    return -1;  /* -1 in case of error */
}


/**
    @brief          Processes the handling of the asynchronous DNS resolver process / state machine
    @details        This function has to be called cyclically every 100ms (OBSD_DNS_RESOLVER_RUN_CALL_INTERVAL_MS).
                    It processes the state machine necessary to handle DNS-Requests and it's responses from the DNS Server.
                    The result of the function after processing all necessary steps (which can take multiple cyclic calls) 
                    is comparable to the synchronous counterpart gethostbyname().
    @param[in]      query_handle:               query handle like returned by obsd_dns_resolver_gethostbyname_prepare
    @param[out]     hp          :               Pointer to a standard struct obsd_pn_dns_hostent/hostent like typically used by DNS Resolvers
                                                see OpenBSD manpage for getnamebyhost() for further Details
                                                http://www.openbsd.org/cgi-bin/man.cgi/OpenBSD-current/man3/endhostent.3?query=gethostbyname
    @note                                       currently the alias name list (hp.h_aliases) is not supported to save memory (size of buf)
    @note                                       The result in hp is only available after processing is successfully completed
                                                (return value = 1) and h_errnop is NETDB_SUCCESS.
    @param[in/out]  buf                         buffer used to store the data for hp: hostname, IP-Address(es) - not alias names (because not supported)
    @note                                       The size of the buffer must be large enough to hold the data for hp delivered by the the DNS server.
                                                Buf can be destroyed (externally) after hp and it's elements are not used any longer.
    @param[in]      buflen                      available buffer size
    @param[out]     h_errnop:                   pointer to a variable the receives the DNS request error
    @note                                       h_errnop is only valid after processing is completed
                                                (return value = 1 oder -1)
                    @li                         -1 [NETDB_INTERNAL]     see errno for detailed error
                    @li                          0 [NETDB_SUCCESS]      no problem
                    @li                          1 [HOST_NOT_FOUND]     Authoritative Answer Host not found (no such name or timeout)
                    @li                          2 [TRY_AGAIN]          Non-Authoritative Host not found, or SERVERFAIL
                    @li                          3 [NO_RECOVERY]        Non recoverable errors, FORMERR, REFUSED, NOTIMP
                    @li                          4 [NO_DATA,NO_ADDRESS] Valid name, no data record of requested type
    @return         -1: processing of the request is completed, but with an error, see errno for details
    @return          0: processsing is in progress, need further calls to obsd_dns_resolver_gethostbyname_run
    @return          1: processing is completed, the result is available in via param hp (see above)
*/
int  obsd_dns_resolver_gethostbyname_run(   obsd_pn_dns_resolver_query_t        query_handle,               /* in     */
                                            struct obsd_pn_dns_hostent **       hp,                         /* out    */
                                            char *                              buf,                        /* in/out */
                                            int                                 buflen,                     /* in     */
                                            int *                               h_errnop                    /* out    */
                                        )
{
    int                 rc              = -1;
    query_t *           asr_q           = (query_t *)query_handle;

    if(asr_q != NULL)   /* check query handle */
    {
        int                 call_asr_run    = 0;
        int                 nfds            = 0;
        struct pollfd       fds[1];             /* have only 1 socket */

        /* 1. check if we have to call asr_run because something happened meanwhile */
        if(asr_q->poll_cond == ASR_WANT_READ)
        {   /* check if socket has some data */

            fds[0].fd       = asr_q->fd;
            fds[0].events   = POLLIN;
            nfds = poll(fds, 1, 0);
            if (nfds > 0)
            {   /* some read data waits for us */
                call_asr_run = 1;       /* need to run because of socket-read event: socket read data available  */
            }
			else if (nfds < 0) /* sado -- poll can return error */
			{
				*h_errnop = NETDB_INTERNAL;
				return -1;
			}
            else
            {   /* cyclic call of obsd_dns_resolver_gethostbyname_run() but got no answer from DNS server so far */
                asr_q->cur_time += OBSD_DNS_RESOLVER_RUN_CALL_INTERVAL_MS;      /* incrementy timeout counter    */
                if (asr_q->cur_time >= asr_q->timeout)
                {   /* timeout reached */
                    call_asr_run = 1;       /* need to run because of an timeout event                           */
                    asr_q->cur_time = 0;    /* reset timeout counter                                             */
                }
                /* else: no need to call asr_run because we have to wait until timeout is reached, let           */
                /*       call_asr_run set to 0                                                                   */
            }
        }
        else
        {   /* query->poll_cond == ASR_WANT_WRITE)  in case we want to write / send                              */
            /* we could check if the socket is writeable, but assume the socket is always writeable, so we don't */
            /* spent a poll(POLLOUT) here before calling asr_run                                                 */
            call_asr_run = 1; /* socket-write event: can write */
        }

        /* 2. call asr_run if necessary */
        if (call_asr_run) /* do we need to call the asr_run ? */
        {   /* read or write or timeout event */
            int                 saved_errno = 0;
            int                 async_result;
            struct asr_result   asr_result;

            bzero((void *) &asr_result, sizeof(struct asr_result));

            /* Typically first call of asr_run() will send a request and the folloing calls try to read the      */
            /* answer if it is received already.                                                                 */
            /* So possibly we save the time until the next cyclic call of obsd_dns_resolver_gethostbyname_run    */
            while((async_result = asr_run(asr_q->obsd_asr_query, &asr_result)) == ASYNC_COND)  /* read until asr_run has got all data */
            {
                asr_q->poll_cond = asr_result.ar_cond;      /* save the condition for the next cyclic call       */

                /* optimization: check if something happend already instead always wait for the nex cyclic call  */
                fds[0].fd = asr_result.ar_fd;
                fds[0].events = (asr_result.ar_cond == ASR_WANT_READ) ? POLLIN:POLLOUT;

                saved_errno = errno;        /* because of poll */
                nfds = poll(fds, 1, 0);     /* socket ready ?  */
                if (nfds == 0)
                {   /* no nothing happened, socket not ready to do anything with it */
                    rc              = 0; /* not completed */
                    /* h_errnop remains unchanged*/

                    asr_q->fd       = asr_result.ar_fd;         /* save fd we are waiting for                    */
                    asr_q->timeout  = asr_result.ar_timeout;    /* save timout for the next cyclic call          */
                    break;                                      /* no data so far, wait for next cyclic call     */
                }
				else if (nfds < 0) /* sado -- poll can return error */
				{
					*h_errnop = NETDB_INTERNAL;
					return -1;
				}
                /* else: socket is ready, next loop calling sr_run */
            } /* while */

            errno = saved_errno;    /* restore it */

            if (async_result == ASYNC_DONE) /* completed ? */
            {   /* yes, check success */
                if (asr_result.ar_h_errno == NETDB_SUCCESS)
                {
                    int err = obsd_dns_fillhostent(asr_result.ar_hostent, *hp, buf, buflen);

                    /* because of PN's need for type / headerfile separation we must map the error codes */
                    /* We uses an 1:1 mapping (no real "mapping"), but we have to check the consistency  */
#if                 !defined(OBSD_PN_DNS_NETDB_INTERNAL)    || !defined(NETDB_INTERNAL) ||  \
                    !defined(OBSD_PN_DNS_NETDB_SUCCESS)     || !defined(NETDB_SUCCESS)  ||  \
                    !defined(OBSD_PN_DNS_HOST_NOT_FOUND)    || !defined(HOST_NOT_FOUND) ||  \
                    !defined(OBSD_PN_DNS_TRY_AGAIN)         || !defined(TRY_AGAIN)      ||  \
                    !defined(OBSD_PN_DNS_NO_RECOVERY)       || !defined(NO_RECOVERY)    ||  \
                    !defined(OBSD_PN_DNS_NO_DATA)           || !defined(NO_DATA)        ||  \
                    !defined(OBSD_PN_DNS_NO_ADDRESS)        || !defined(NO_ADDRESS)
#error "DNS error code constants missing !"
#endif

#if                 ((OBSD_PN_DNS_NETDB_INTERNAL)   != (NETDB_INTERNAL))    ||  /* (-1) */\
                    ((OBSD_PN_DNS_NETDB_SUCCESS)    != (NETDB_SUCCESS))     ||  /* ( 0) */\
                    ((OBSD_PN_DNS_HOST_NOT_FOUND)   != (HOST_NOT_FOUND))    ||  /* ( 1) */\
                    ((OBSD_PN_DNS_TRY_AGAIN)        != (TRY_AGAIN))         ||  /* ( 2) */\
                    ((OBSD_PN_DNS_NO_RECOVERY)      != (NO_RECOVERY))       ||  /* ( 3) */\
                    ((OBSD_PN_DNS_NO_DATA)          != (NO_DATA))           ||  /* ( 4) */\
                    ((OBSD_PN_DNS_NO_ADDRESS)       != (NO_ADDRESS))            /* ( 4) */
#error "DNS error code constants are different !"
#endif
                    if ( err == 0)
                    {   /* successfully completed, result is stored in *hp and data is stored in buf */
                        *h_errnop = asr_result.ar_h_errno; /* NETDB_SUCCESS */
                        /* no errno */
                        rc = 1; /* successfully completed */
                    }
                    else
                    {   /* typically ERANGE */
                        *h_errnop = NETDB_INTERNAL;
                        errno = err;
                        rc = -1;  /* completed but with an error */
                    }
                }
                else
                {
                    *h_errnop = asr_result.ar_h_errno;
                    errno = asr_result.ar_errno;
                    rc = -1;  /* completed but with an error */
                }
                /* query completed */
                if (asr_result.ar_hostent != NULL)
                    free(asr_result.ar_hostent);
                if(asr_q != NULL)
                {
                    free(asr_q);
                    asr_q = NULL;
                }
            }
            else /* (call_asr_run) */
            {   /* ASYNC_COND, not completed, we want to continue in the next cyclic call */
                rc = 0; /* not completed */
                /* h_errnop remains unchanged*/
            }
       }
       else /* call_asr_run */
       {    /* no call of asr_run necessary, so continue with the next cyclic call */
            /* *h_errnop is not valid in this case */
            rc = 0; /* need to continue because we are called but got no data and timeout is not reached */
            /* h_errnop remains unchanged*/
       }
    }
    else
    {   /* invalid query handle */
        rc          = -1; /* completed but with an error */
        *h_errnop   = NETDB_INTERNAL;
        errno       = EINVAL;
    }

    return rc;
}

/**
    @brief          Abort a DNS query that is currently under processing
    @details        The function clears a running DNS query. It can be called when the query is waiting on a file descriptor. 
    @note           A completed query is already cleared when obsd_dns_resolver_gethostbyname_run() is completed, so 
                    the function MUST NOT be called in this case !!!
    @param[in]      query_handle:               query handle like returned by obsd_dns_resolver_gethostbyname_prepare
    @return          0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_dns_resolver_query_abort(          obsd_pn_dns_resolver_query_t        query_handle                /* in  */
                                 )
{
	if( query_handle == NULL)
    {
        return -1;
    }
    else
    {
        asr_abort(((query_t *)query_handle)->obsd_asr_query);
        free((void *) query_handle);
        return 0;
    }
}

/**
    @brief          Destroys a resolver instance formerly created by obsd_dns_resolver_instance_create
    @details        Function is intended to be uses for deinitialization purposes. All pending queries are abortet
                    and all the memory internally allocated by obsd_dns_resolver_instance_create() and subsequent
                    DNS queries using 
                        @li  obsd_dns_resolver_gethostbyname_prepare() and
                        @li  obsd_dns_resolver_gethostbyname_run()
                    is freed
    @param[in]      query_handle:               query handle like returned by obsd_dns_resolver_gethostbyname_prepare
    @return          0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_dns_resolver_instance_destroy(     obsd_pn_dns_resolver_instance_t     p_resolver_instance_handle  /* in  */
                                      )
{
    if(p_resolver_instance_handle == NULL)
    {
        return -1;
    }
    else
    {
        asr_resolver_done(p_resolver_instance_handle);
        p_resolver_instance_handle = NULL;

        if (--obsd_dns_resolver_instance_count == 0)
        {
            res_deinitid(); /* not longer needed for DNS transaction IDs */
        }
        return 0;
    }
}


/* gh2289n: following code is taken from internal static function _fillhostent() (gethostnamadr.c)*/
/*          In our spacial case all code around the alias list is not used and removed here,      */
/*          because the PN DNS resolver doesn't support alias lists which would require           */
/*          additional memory in the scratch buffer (buf) given by the caller/LSA component.      */
/*          Additionally long alias lists of the DNS Server response (which could come from an    */
/*          attacker too) could fill scratch buffers that are too small in the applications so    */
/*          that some other DNS response information has no room there anymore. This could lead   */
/*          to an ERANGE error.                                                                   */
static char *obsd_dns_empty_list[] = { NULL, };

static int obsd_dns_fillhostent(const struct hostent *h, struct obsd_pn_dns_hostent *r, char *buf, size_t len)
{
    char    **ptr, *end, *pos;
    size_t  n, i;
    int     naddrs;

    bzero(buf, len);
    bzero(r, sizeof(*r));
    /* r->h_aliases = obsd_dns_empty_list; */
    r->h_addr_list = obsd_dns_empty_list;

    end = buf + len;
    ptr = (char **)ALIGN(buf);

    if ((char *)ptr >= end)
        return (ERANGE);

    for (naddrs = 0; h->h_addr_list[naddrs]; naddrs++)
        ;

    pos = (char *)(ptr + (/* naliases + */ 1) + (naddrs + 1));
    if (pos >= end)
        return (ERANGE);

    r->h_name = NULL;
    r->h_addrtype = h->h_addrtype;
    r->h_length = h->h_length;
    /* r->h_aliases = ptr; */
    r->h_addr_list = ptr + /* naliases */ + 1;

    n = strlcpy(pos, h->h_name, end - pos);
    if (n >= (unsigned long)(end - pos))
        return (ERANGE);
    r->h_name = pos;
    pos += n + 1;

    pos = (char *)ALIGN(pos);
    if (pos >= end)
        return (ERANGE);

    for (i = 0; i < (unsigned long)naddrs; i++) {
        if (r->h_length > end - pos)
            return (ERANGE);
        memmove(pos, h->h_addr_list[i], r->h_length);
        r->h_addr_list[i] = pos;
        pos += r->h_length;
    }

    return (0);
}


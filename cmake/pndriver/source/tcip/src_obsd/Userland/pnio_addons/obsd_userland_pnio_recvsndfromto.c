#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_uio.h>        /* struct iovec */
#include <sys/obsd_kernel_socket.h>
#include <net/obsd_kernel_if_dl.h>      /* sockaddr_dl  */
#include <netinet/obsd_kernel_in.h>

#include <obsd_userland_string.h>       /* bzero */
#include <obsd_userland_errno.h> 

/* #define OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */ /* some debug helper */

#ifdef     OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO
#include <arpa/obsd_userland_inet.h>
#include <obsd_userland_stdio.h>
#endif  /* OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */



/**
    @brief          Enhanced sendto function that can set the source IP address of the IP packet sent
    @details        The function behaves comparable to sendto but additionally can process the following informations:
                        * the local IP address the sent UDP packet was sent from
    @note           the following socket options (level IPPROTO_IP) has to be set to process the additional informations
                    None special socket options for control messages are required
    @note           works only with IPv4 so far
    @param[in]      s:          socket
    @param[in]      msg:        message to be sent
    @param[in]      len:        size of the message
    @param[in]      flags:      see sendto(2)
    @param[in]      from:       source IP address of the message (UDP only)
                                type: struct sockaddr *
    @param[in]      fromlen:    size of the buffer associated with the "from" address 
                                (typically sizeof(struct sockaddr) or sizeof(struct sockaddr_in ) for IPv4 only) 
                                or 0 if not needed
                                type: socklen_t
    @param[in]      to          destination IP address of the message (UDP only)
                                type: struct sockaddr *
    @param[in]      tolen:      size of the buffer associated with the "to" address
                                type: socklen_t
    @return         number of bytes received
    @return         -1 in case of errors (errno is set accordingly, see recvfrom(2) for details)
*/

/* function declaration with original intended types:                                                                                                           */
/* int sendfromto(int s, const void *msg, size_t len, int flags, const struct sockaddr *from, socklen_t fromlen, const struct sockaddr *to, socklen_t tolen)    */
/* function definition with basic types only (because of the need for type separation between PN and OBSD):                                                     */
int obsd_pnio_sendfromto(int s, void *msg, unsigned int len, int flags, const void *from, unsigned int fromlen, const void *to, unsigned int tolen)
{
#ifdef     OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO
    const struct sockaddr   *from_dbg = (const struct sockaddr *)from;
    const struct sockaddr   *to_dbg   = (const struct sockaddr *)to;
#endif  /* OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */

    struct iovec            iov;
    struct msghdr           message_header;

    union {
        struct  cmsghdr hdr;
        char    buf[CMSG_SPACE(sizeof(struct in_addr))];
    } cmsgbuf;

    if (tolen == 0)         /* no to-address is given if tolen is 0, used by SOCK */
    {
        to = NULL;          /* invalidate it */
    }

    bzero(&message_header, sizeof(message_header));

    /* setup msghdr */
    iov.iov_base               = msg;
    iov.iov_len                = len;

    /* setup data to send */
    message_header.msg_iov     = &iov;
    message_header.msg_iovlen  = 1;  /* only 1 io vector */

    /* setup the destination address */
    message_header.msg_name    = (void *)to;
    message_header.msg_namelen = tolen; /* to->sa_len; */

    /* setup the source address address if needed */
    if (fromlen != 0)
    {
        const struct sockaddr   *from_address = (const struct sockaddr *)from;

        /* seems that we need to set the locla address, prepare the control message for the source address */
        if ((from_address->sa_family == AF_INET) && (fromlen == sizeof(struct sockaddr_in))) /* currently no AF_INET6 support */
        {
            struct cmsghdr *pcmsgh = (struct cmsghdr *)&cmsgbuf; /* points to the first control message in cmsgbuf */

            struct in_addr *srcadr;

            message_header.msg_controllen += CMSG_SPACE(sizeof(struct in_addr));   /* IPv4 address */
            message_header.msg_control     = pcmsgh;

            pcmsgh->cmsg_len               = CMSG_LEN(sizeof(struct in_addr));
            pcmsgh->cmsg_level             = IPPROTO_IP;
            pcmsgh->cmsg_type              = IP_SENDSRCADDR;

            srcadr  = (struct in_addr*)CMSG_DATA(pcmsgh);
            *srcadr = ((const struct sockaddr_in *)from_address)->sin_addr;
        }
        else
        {
            return EAFNOSUPPORT;
        }
    }
    /* else: no need to setup the source address */

    return (int)sendmsg(s, &message_header, flags);  /* typecast because sendmsg returns a ssize_t (long int) */
}


/**
    @brief          Enhanced recvfrom function that can deliver additional informations
    @details        The function behaves comparable to recvfrom but additionally delivers the following informations:
                        * the local IP address the received UDP packet was sent to
                        * the index of interface where we received the packet
                          (can be used with if_indextoname(3) to get the interface name)
    @note           the following socket options (level IPPROTO_IP) has to be set to get the additional informations
                        * IP_RECVDSTADDR:   for the local IP address (to, tolen)
                        * IP_RECVIF:        for the receive interface (if_index)
                    No other socket options for control messages are supported and allowed !
    @note           works only with IPv4 so far
    @param[in]      s:          socket
    @param[out]     buf:        message buffer
    @param[in]      len:        size of the message buffer
    @param[in]      flags:      see recvfrom(2)
    @param[out]     from:       source IP address of the message (UDP only)
                                type: struct sockaddr *
    @param[in/out]  fromlen:    has to be initialized to the size of the buffer associated with from,
                                and modified on return to indicate the actual size of the address stored there
                                type: socklen_t
    @param[out]     to          destination IP address of the message (UDP only)
                                type: struct sockaddr *
    @param[in/out]  tolen:      has to be initialized to the size of the buffer associated with to,
                                and modified on return to indicate the actual size of the address stored there
                                (or 0 is not needed)
                                type: socklen_t
    @param[out]     if_index:   the index of the interface where the message was received or -1 if no such information
                                (use a NULL-Pointer if not needed)
    @return         number of bytes received
    @return         -1 in case of errors (errno is set accordingly, see recvfrom(2) for details)
*/

/* function declaration with original intended types:                                                                                                           */
/* int recvfromto(int s, void *buf, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen, struct sockaddr *to, socklen_t *tolen, int *if_index)     */
/* function definition with basic types only (because of the need for type separation between PN and OBSD):                                                     */
int obsd_pnio_recvfromto(int s, void *buf, unsigned int len, int flags, void *from, unsigned int *fromlen, void *to, unsigned int *tolen, int *if_index)
{
    int               rc = 0;
    struct iovec      iov;
    struct msghdr     message_header;

    union
    {
        struct cmsghdr hdr[2];      /* only 2 control messages are supported and allowed (IP_RECVDSTADDR + IP_RECVIF) */
        char	buf[CMSG_SPACE(sizeof(struct in_addr)) +    /* IP_RECVDSTADDR */
                    CMSG_SPACE(sizeof(struct in_addr))      /* IP_RECVIF */
                   ];
    } cmsgbuf;

    if (fromlen == NULL)    /* we need an out pointer */
    {
        errno = EINVAL;
        return -1;
    }

    bzero(&message_header, sizeof(message_header));

    /* setup msghdr */
    iov.iov_base                 = buf;
    iov.iov_len                  = len;

    /* setup data to receive */
    message_header.msg_iov       = &iov;
    message_header.msg_iovlen    = 1;  /* only 1 io vector */

    /* setup the source address */
    message_header.msg_name      = from;
    message_header.msg_namelen   = *fromlen; /* from->sa_len; */

    /* setup the source address address if needed */
    if ((tolen != NULL) || (to != NULL) || (if_index != NULL))
    {
        /* seems that we need to set the local address or the interface index, prepare the            */
        /* control message header buffer for the destination address and / or the if_index            */

        if ((tolen != NULL) && (*tolen != sizeof(struct sockaddr_in)))  /* no AF_INET6 support so far */
        {
            return EAFNOSUPPORT;
        }
        else
        {
            bzero(to, *tolen);
            *tolen                     = 0;
        }

        if ((if_index != NULL))
            *if_index                  = -1;
        message_header.msg_controllen  = sizeof(cmsgbuf);
        message_header.msg_control     = (struct cmsghdr *)&cmsgbuf;
    }
    /* else: no need to setup the control information for the destination address or the if_index     */

    if ((rc = recvmsg(s, &message_header, flags)) < 0)
    {
        return rc;
    }
    else
    {
#ifdef     OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO
        struct sockaddr   *from_dbg = (struct sockaddr *)from;
#endif  /* OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */
        struct cmsghdr *pcmsgh;

        *fromlen = ((struct sockaddr *)from)->sa_len;   /* fromlen was checked earlier not to be a NULL pointer */
                                                        /* maybe fromlen = mesg.msg_namelen;                    */

        for (   pcmsgh = (struct cmsghdr *)CMSG_FIRSTHDR(&message_header);
                pcmsgh != NULL;
                pcmsgh = (struct cmsghdr *)CMSG_NXTHDR(&message_header, pcmsgh))
        {
#ifdef    OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO
            fprintf( stderr, "CMSG_NXTHDR=%p, level=%d, type=%d\n",
                CMSG_NXTHDR (&message_header, pcmsgh), pcmsgh->cmsg_level, pcmsgh->cmsg_type );
#endif /* OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */
            if ( (pcmsgh->cmsg_level    == IPPROTO_IP)      &&
                 (pcmsgh->cmsg_type     == IP_RECVDSTADDR)  &&
                 (pcmsgh->cmsg_len      == CMSG_LEN(sizeof(struct in_addr)))
                )
            {
                struct sockaddr_in *plocal_saddrin = (struct sockaddr_in *) (to);

                /* original tolen must be = sizeof(struct sockaddr_in), checked earlier */
                plocal_saddrin->sin_len     = sizeof(struct sockaddr_in);
                plocal_saddrin->sin_family  = AF_INET;
                plocal_saddrin->sin_addr    = *((struct in_addr *) CMSG_DATA(pcmsgh));
                plocal_saddrin->sin_port    = 0; /* not used */
                *tolen                      = sizeof(struct sockaddr_in);

#ifdef    OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO
                {
                    struct in_addr local_addr = *((struct in_addr *) CMSG_DATA(pcmsgh));
                    printf("Dest-IP Address is: %s\n", inet_ntoa(local_addr));
                    fflush(stdout); 
                }
#endif /* OBSD_PNIO_DEBUG_SEND_RECV_FROM_TO */
                continue;   /* next control message */
            }

            if ((pcmsgh->cmsg_level == IPPROTO_IP) && pcmsgh->cmsg_type == IP_RECVIF)
            {
                if ((if_index != NULL))
                {
                    *if_index = ((struct sockaddr_dl *) CMSG_DATA(pcmsgh))->sdl_index;
                }
                continue;   /* next control message */
            }
        }
    }

    return rc;
}


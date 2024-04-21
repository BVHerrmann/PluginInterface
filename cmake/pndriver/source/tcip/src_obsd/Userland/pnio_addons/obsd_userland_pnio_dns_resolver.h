/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_dns_resolver.h
 *
 * @brief       asynchronous DNS resolver for PN
 * @details     Interface of the resolver functions
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.04
 * @date        created: 24.06.2014
 */
/* -------------------------------------------------------------------------------------------------- */


#ifndef __OBSD_USERLAND_PNIO_DNS_RESOLVER_H__
#define __OBSD_USERLAND_PNIO_DNS_RESOLVER_H__


/******************************************************************************************************/
/*******  includes                                                                              *******/
/******************************************************************************************************/


/******************************************************************************************************/
/*******  macros and macro constants                                                            *******/
/******************************************************************************************************/
#define OBSD_PN_DNS_NETDB_INTERNAL  -1	/* see errno */
#define OBSD_PN_DNS_NETDB_SUCCESS   0	/* no problem */
#define OBSD_PN_DNS_HOST_NOT_FOUND  1 /* Authoritative Answer Host not found */
#define OBSD_PN_DNS_TRY_AGAIN       2 /* Non-Authoritative Host not found, or SERVERFAIL */
#define OBSD_PN_DNS_NO_RECOVERY     3 /* Non recoverable errors, FORMERR, REFUSED, NOTIMP */
#define OBSD_PN_DNS_NO_DATA         4 /* Valid name, no data record of requested type */
#define OBSD_PN_DNS_NO_ADDRESS      OBSD_PN_DNS_NO_DATA /* no address */

#define OBSD_DNS_RESOLVER_RUN_CALL_INTERVAL_MS 100  /* cyclic call interval needed for obsd_dns_resolver_gethostbyname_run in ms */

#define MAX_BUF_SIZE   0xBAD // Hendrik definieren, prüfen, auf welchen Wert man das am besten festlegt <TBD>

/******************************************************************************************************/
/*******  declarations                                                                          *******/
/******************************************************************************************************/
typedef void * obsd_pn_dns_resolver_instance_t;
typedef void * obsd_pn_dns_resolver_query_t;

/* struct hostent for PN: for type separation we need a separate hostent structure declaration that   */
/*                        only uses basic data types so that's not needed to include the complete     */
/*                        OBSD header tree (which would lead to type collisions with LSA and possibly */
/*                        C-Compiler header files.                                                    */
/*                        Because we don't support alias names in the resolver we don't inlude the    */
/*                        related sections of the hostent structure here.                             */
/*                        OBSD's struct hostent is declared in obsd_userland_netdb.h                  */
struct obsd_pn_dns_hostent
{
    char    *h_name;        /* official name of host                                                  */
    /* char    **h_aliases;*/    /* normally alias list, but currently not supported in PN resolver   */
    int     h_addrtype;     /* host address type                                                      */
    int     h_length;       /* length of address                                                      */
    char    **h_addr_list;  /* list of addresses from name server                                     */
#define obsd_pn_dns_h_addr h_addr_list[0]  /* address, for backward compatibility                     */
};

/******************************************************************************************************/
/*******  data definitions                                                                      *******/
/******************************************************************************************************/


/******************************************************************************************************/
/*******  interface functions                                                                   *******/
/******************************************************************************************************/
extern int obsd_dns_resolver_instance_create(
                                                    const char *                        psz_config_string,          /* in  */
                                                    obsd_pn_dns_resolver_instance_t*    p_resolver_instance_handle  /* out */
                                            );

extern int obsd_dns_resolver_gethostbyname_prepare(
                                                    const char *                        psz_hostname_fqdn,          /* in  */
                                                    obsd_pn_dns_resolver_instance_t     resolver_instance_handle,   /* in  */
                                                    obsd_pn_dns_resolver_query_t*       p_query_handle              /* out */
                                                  );

extern int  obsd_dns_resolver_gethostbyname_run(
                                                    obsd_pn_dns_resolver_query_t        query_handle,               /* in  */
                                                    struct obsd_pn_dns_hostent **       hp,                         /* out */
                                                    char *                              buf,                        /* in /out */
                                                    int                                 buflen,                     /* in  */
                                                    int *                               h_errnop                    /* out */
                                               );

extern int obsd_dns_resolver_query_abort(
                                                    obsd_pn_dns_resolver_query_t        query_handle                /* in */
                                        );

extern int obsd_dns_resolver_instance_destroy(
                                                    obsd_pn_dns_resolver_instance_t     p_resolver_instance_handle  /* in */
                                             );

#endif /* __OBSD_USERLAND_PNIO_DNS_RESOLVER_H__ */

/******************************************************************************************************/
/*******  end of file                                                                           *******/
/******************************************************************************************************/


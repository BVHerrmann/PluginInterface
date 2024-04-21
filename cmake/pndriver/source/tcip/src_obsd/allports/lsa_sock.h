/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        lsa_sock.h
 *
 * @brief       Interface to the PN LSA SOCK component
 * @details     -
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.01
 * @date        created: 07.07.2014
 */
/* -------------------------------------------------------------------------------------------------- */


#ifndef   __LSA_SOCK_H__
#define   __LSA_SOCK_H__

extern void sock_open_and_bind_snmpd_port(int snmpd_port); /* request SOCK to open & bind snmp port */
extern void sock_close_snmpd_port(void);                   /* request SOCK to close snmp port       */

#endif /* __LSA_SOCK_H__ */


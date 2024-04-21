/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_tcpparams.h
 *
 * @brief       interface of the PNIO specific functions for changing TCP parameters
 * @details     PNIO mini functions to set parameters for the tcp protocol handling
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.03
 * @date        created: 14.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef __OBSD_USERLAND_PNIO_TCPPARAMS_H__
#define __OBSD_USERLAND_PNIO_TCPPARAMS_H__

extern int obsd_pnio_set_tcp_keepinittime(int tcp_connection_timeout /* seconds */);
extern int obsd_pnio_set_tcp_keepintvl(   int tcp_keepintvl          /* seconds */);
extern int obsd_pnio_set_tcp_keepcount(   int tcp_keepcount);
extern int obsd_pnio_set_tcp_rexmtmax(    int tcp_retransmit_timeout /* seconds */);
extern int obsd_pnio_set_tcp_keepidle(    int tcp_keepidle_timeout   /* seconds */);

extern int obsd_pnio_get_tcp_keepinittime(void);
extern int obsd_pnio_get_tcp_keepintvl(void);
extern int obsd_pnio_get_tcp_keepcount(void);
extern int obsd_pnio_get_tcp_rexmtmax(void);
extern int obsd_pnio_get_tcp_keepidle(void);

#endif /* __OBSD_USERLAND_PNIO_TCPPARAMS_H__ */


/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_userland_pnio_tcpparams.c
 *
 * @brief       PNIO mini functions to set parameters for the tcp protocol handling
 * @details     Implementation
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.02
 * @date        created: 14.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_socket.h>

#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_tcp.h>
#include <netinet/obsd_kernel_tcp_timer.h>
#include <netinet/obsd_kernel_tcp_var.h>

#include <obsd_kernel_usr.h> /* own interface */

static int obsd_pn_set_tcp_param(int inTcpParamId, void * pTcpParamValue, size_t   ParamSize);
static int obsd_pn_get_tcp_param(int inTcpParamId, void * pTcpParamValue, size_t *pParamSize);
static int obsd_pn_get_tcp_slowhz(void);

/* ======================== interface functions ================================================= */

/**
    @brief          Sets an internal parameter of the TCP stack.
    @details        Sets the TCP initial connect keep alive time value.
                    If an connection is idle (no segments received) for tcp_connection_timeout amount of time,
                    but not yet established, then we drop the connection.
    @param[in]      tcp_connection_timeout: time value in seconds (must be >= 0)
    @note           @li   default value in OBSD Stack (v5.1): tcptv_keep_init = 75*PR_SLOWHZ -> 75s
                    @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_set_tcp_keepinittime(int tcp_connection_timeout /* seconds */)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();

    if ((tcp_connection_timeout >= 0) && (slowhz > 0))
    {
        int     new_tcp_keepinittime_ticks = tcp_connection_timeout * slowhz;

        return obsd_pn_set_tcp_param(TCPCTL_KEEPINITTIME, &new_tcp_keepinittime_ticks, sizeof(new_tcp_keepinittime_ticks));
    }
    else
    {   /* error from obsd_pn_get_tcp_slowhz or negative argument */
        return -1; 
    }
}

/**
    @brief          Returns an internal parameter of the TCP stack.
    @details        Returns the TCP initial connect keep alive time value.
                    If an connection is idle (no segments received) for tcp_connection_timeout amount of time,
                    but not yet established, then we drop the connection.
    @param[in]      none
    @return         tcp_connection_timeout in seconds in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   default value in OBSD Stack (v5.1): tcptv_keep_init = 75*PR_SLOWHZ -> 75s
                    @li   The Function is intended to be called from the userland level.
*/
int obsd_pnio_get_tcp_keepinittime(void)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();
    int rc     = -1;

    if (slowhz > 0)
    {
        int     keepinittime_ticks = -1;
        size_t  ParamSize          = sizeof(keepinittime_ticks);

        rc = obsd_pn_get_tcp_param(TCPCTL_KEEPINITTIME, &keepinittime_ticks, &ParamSize);

        if (rc == 0)
        {
            rc = keepinittime_ticks / slowhz;
        }
    }
    return rc;
}

/* ---------------------------------------------------------------------------------------------- */

/**
    @brief          Sets an internal parameter of the TCP stack.
    @details        Sets the TCP keep alive probe interval.
    @param[in]      tcp_keepintvl: time value in seconds (must be >= 0)
    @note           @li   default in OBSD-Stack: tcp_keepintvl = 75*PR_SLOWHZ -> 75s
                    @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_set_tcp_keepintvl(int tcp_keepintvl /* seconds */)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();

    if ((tcp_keepintvl >= 0) && (slowhz > 0))
    {
        int     new_tcp_keepintvl_ticks = tcp_keepintvl * slowhz;

        return obsd_pn_set_tcp_param(TCPCTL_KEEPINTVL, &new_tcp_keepintvl_ticks, sizeof(new_tcp_keepintvl_ticks));
    }
    else
    {   /* error from obsd_pn_get_tcp_slowhz or negative argument */
        return -1; 
    }
}

/**
    @brief          Returns an internal parameter of the TCP stack.
    @details        Returns the TCP keep alive probe interval.
    @param[in]      none
    @return         tcp_keepintvl in seconds in case of success
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   default in OBSD-Stack: tcp_keepintvl = 75*PR_SLOWHZ -> 75s
                    @li   The Function is intended to be called from the userland level.
*/
int obsd_pnio_get_tcp_keepintvl(void)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();
    int rc     = -1;

    if (slowhz > 0)
    {
        int     tcp_keepintvl_ticks = -1;
        size_t  ParamSize           = sizeof(tcp_keepintvl_ticks);

        rc = obsd_pn_get_tcp_param(TCPCTL_KEEPINTVL, &tcp_keepintvl_ticks, &ParamSize);

        if (rc == 0)
        {
            rc = tcp_keepintvl_ticks / slowhz;
        }
    }
    return rc;
}

/* ---------------------------------------------------------------------------------------------- */

/**
    @brief          Sets an internal parameter of the TCP stack.
    @details        Sets the TCP maximum keepalive probes before dropping the connection.
    @param[in]      tcp_keepcount: the count value
    @note           @li   default in OBSD-Stack: TCPTV_KEEPCNT = 8 
                    @li   default for tcp_maxidle = TCPTV_KEEPCNT * (75*PR_SLOWHZ) -> 8 * 75s = 600
                    @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_set_tcp_keepcount(int tcp_keepcount)
{
    int     new_tcp_keepcount = tcp_keepcount; 

    return obsd_pn_set_tcp_param(TCPCTL_KEEPCOUNT, &new_tcp_keepcount, sizeof(new_tcp_keepcount));
}

/**
    @brief          Returns an internal parameter of the TCP stack.
    @details        Returns the TCP maximum keepalive probes before dropping the connection
    @param[in]      none
    @return         tcp_keepcount: the count value
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   default in OBSD-Stack: TCPTV_KEEPCNT = 8 
                    @li   default for tcp_maxidle = TCPTV_KEEPCNT * (75*PR_SLOWHZ) -> 8 * 75s = 600
                    @li   The Function is intended to be called from the userland level.
*/
int obsd_pnio_get_tcp_keepcount(void)
{
    int     rc     = -1;
    int     tcp_keepcount = -1;
    size_t  ParamSize     = sizeof(tcp_keepcount);

    rc = obsd_pn_get_tcp_param(TCPCTL_KEEPCOUNT, &tcp_keepcount, &ParamSize);

    if (rc == 0)
    {
        rc = tcp_keepcount;
    }
    return rc;
}

/* ---------------------------------------------------------------------------------------------- */

/**
    @brief          Sets an internal parameter of the TCP stack.
    @details        Sets the TCP maximum allowable REXMT timer value.
    @param[in]      tcp_retransmit_timeout: time value in seconds (must be >= 0)
    @note           @li   default in OBSD-Stack: tcp_rexmtmax = (64*PR_SLOWHZ) -> 64s
                    @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_set_tcp_rexmtmax(int tcp_retransmit_timeout /* in seconds */)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();

    if ((tcp_retransmit_timeout >= 0) && (slowhz > 0))
    {
        int     new_tcp_rexmtmax_ticks = tcp_retransmit_timeout * slowhz;

        return obsd_pn_set_tcp_param(TCPCTL_REXMTMAX, &new_tcp_rexmtmax_ticks, sizeof(new_tcp_rexmtmax_ticks));
    }
    else
    {   /* error from obsd_pn_get_tcp_slowhz or negative argument */
        return -1; 
    }
}

/**
    @brief          Returns an internal parameter of the TCP stack.
    @details        Returns the TCP maximum allowable REXMT timer value.
    @param[in]      none
    @return         tcp_retransmit_timeout: time value in seconds
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   default in OBSD-Stack: tcp_rexmtmax = (64*PR_SLOWHZ) -> 64s
                    @li   The Function is intended to be called from the userland level.
*/
int obsd_pnio_get_tcp_rexmtmax(void)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();
    int rc     = -1;

    if (slowhz > 0)
    {
        int     tcp_retransmit_timeout = -1;
        size_t  ParamSize           = sizeof(tcp_retransmit_timeout);

        rc = obsd_pn_get_tcp_param(TCPCTL_REXMTMAX, &tcp_retransmit_timeout, &ParamSize);

        if (rc == 0)
        {
            rc = tcp_retransmit_timeout / slowhz;
        }
    }
    return rc;
}

/* ---------------------------------------------------------------------------------------------- */

/**
    @brief          Sets an internal parameter of the TCP stack.
    @details        Sets the TCP idle timeout before the stack starts keepalive probing.
                    Once the connection is established, if the connection is idle for tcp_keepidle_timeout
                    time (and keepalives have been enabled on the socket), we begin to probe the connection.
    @param[in]      tcp_keepidle_timeout: time value in seconds (must be >= 0)
    @note           @li   default in OBSD-Stack: tcp_keepidle = (120*60*PR_SLOWHZ) -> 120 min
                    @li   The Function is intended to be called from the userland level.
    @return         0 in case of success
    @return         -1 in case of errors (errno is set accordingly)
*/
int obsd_pnio_set_tcp_keepidle(int tcp_keepidle_timeout /* in seconds */)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();

    if ((tcp_keepidle_timeout >= 0) && (slowhz > 0))
    {
        int     new_tcp_keepidle_ticks = tcp_keepidle_timeout * slowhz;

        return obsd_pn_set_tcp_param(TCPCTL_KEEPIDLE, &new_tcp_keepidle_ticks, sizeof(new_tcp_keepidle_ticks));
    }
    else
    {   /* error from obsd_pn_get_tcp_slowhz or negative argument */
        return -1; 
    }
}

/**
    @brief          Returns an internal parameter of the TCP stack.
    @details        Returns the TCP idle timeout before the stack starts keepalive probing.
    @param[in]      none
    @return         tcp_keepidle_timeout: time value in seconds
    @return         -1 in case of errors (errno is set accordingly)
    @note           @li   default in OBSD-Stack: tcp_keepidle = (120*60*PR_SLOWHZ) -> 120 min
                    @li   The Function is intended to be called from the userland level.
*/
int obsd_pnio_get_tcp_keepidle(void)
{
    int slowhz =  obsd_pn_get_tcp_slowhz();
    int rc     = -1;

    if (slowhz > 0)
    {
        int     tcp_keepidle_timeout = -1;
        size_t  ParamSize           = sizeof(tcp_keepidle_timeout);

        rc = obsd_pn_get_tcp_param(TCPCTL_KEEPIDLE, &tcp_keepidle_timeout, &ParamSize);

        if (rc == 0)
        {
            rc = tcp_keepidle_timeout / slowhz;
        }
    }
    return rc;
}

/* ---------------------------------------------------------------------------------------------- */


/* ======================== internal functions ================================================== */
/* get the value of the given tcp parameter,                                                      */
/* if OK:            0 is returned                                                                */
/* if unsuccessful: -1 is returned and errno is set appropriately (from sysctl).                  */
static int obsd_pn_set_tcp_param(int inTcpParamId, void * pTcpParamValue, size_t   ParamSize)
{
    int mib[4];

    mib[0] = CTL_NET;
    mib[1] = PF_INET;
    mib[2] = IPPROTO_TCP;
    mib[3] = inTcpParamId;

    return sysctl(mib, 4, NULL, 0, pTcpParamValue, ParamSize);
}

/* get the value of the given tcp parameter,                                                      */
/* if OK:            0 is returned                                                                */
/* if unsuccessful: -1 is returned and errno is set appropriately (from sysctl).                  */
static int obsd_pn_get_tcp_param(int inTcpParamId, void * pTcpParamValue, size_t *pParamSize)
{
    int     mib[4];

    mib[0] = CTL_NET;
    mib[1] = PF_INET;
    mib[2] = IPPROTO_TCP;
    mib[3] = inTcpParamId;

    return sysctl(mib, 4, pTcpParamValue, pParamSize, NULL, 0);
}

/* returns the TCP slowhz timer count (ticks/sec) or -1 in case of an (internal) error            */
/* Delivers the units for tcp_keepidle, tcp_keepintvl and so on. Those variables are in ticks of  */
/* a clock that ticks tcp.slowhz times per second. (That is, their values must be multiplied      */
/* (if written) or divided (if read) by the tcp.slowhz value to get times in seconds.)            */
static int obsd_pn_get_tcp_slowhz(void)
{
    int     slowhz_ticks = -1;
    size_t  ParamSize    = sizeof(slowhz_ticks);
    int     rc = -1;
    
    rc = obsd_pn_get_tcp_param(TCPCTL_SLOWHZ, &slowhz_ticks, &ParamSize);

    if (rc == 0)
        rc = slowhz_ticks;
    return rc;
}

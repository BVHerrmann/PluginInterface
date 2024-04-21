/*	$OpenBSD: snmpd.c,v 1.32 2016/02/02 17:51:11 sthen Exp $	*/

/*
 * Copyright (c) 2007, 2008, 2012 Reyk Floeter <reyk@openbsd.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_wait.h>
#include <sys/obsd_kernel_tree.h>

#include <net/obsd_kernel_if.h>

#include <obsd_userland_string.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_stdlib.h>
#include <obsd_userland_getopt.h>
#include <obsd_userland_err.h>
#include <obsd_userland_errno.h>
#include <obsd_userland_event.h>

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_mib.h"

#define main        snmpd_main       /* avoid conflicts with other userland procs/functions */
#define usage       snmpd_usage      /* avoid conflicts with other userland procs/functions */

void	 snmpd_shutdown(struct snmpd *);
void	 snmpd_sig_handler(int, short, void *);
int	 snmpd_dispatch_snmpe(int, struct privsep_proc *, struct imsg *);
void	 snmpd_generate_engineid(struct snmpd *);
int	 check_child(pid_t, const char *);

extern int obsd_snmpe_socket_open( void);   /* close snmp socket */
extern int obsd_snmpe_socket_close(void);  /* open snmp socket */

static struct snmpd static_snmpd_env;   /* normally malloc'ed in parse_config                */

static struct privsep_proc procs[] = {
	{ "snmpe", PROC_SNMPE, snmpd_dispatch_snmpe, snmpe, snmpe_shutdown }
};


int
main(int argc, char *argv[])
{
	struct snmpd	*env;
	u_int		 flags = 0;
	struct privsep	*ps;

	smi_init();

	/* log to stderr until daemonized */
	log_init(1);

	OBSD_UNUSED_ARG(argc);  /* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(argv);  /* gh2289n: avoid compiler warning */

	/* gh2289n: if we use snmpd_main as a init function only that returns to the caller and work */
	/*          asynchronous afterwards snmpd_env needed to statically allocated               */
	env = &static_snmpd_env;     /* normally malloc'ed in parse_config, we have a static variable*/
	/* use minimal environment, see parse_config() for the "big" solution                      */
	memset((void*)env, 0, sizeof(struct snmpd));
	static_snmpd_env.sc_sock = -1;
	ps = &env->sc_ps;
	ps->ps_env = env;
	env->sc_flags                = (u_int8_t) flags; /* OBSD_ITGR -- avoid warning */
	env->sc_confpath             = NULL;
	env->sc_address.ss.ss_family = AF_INET;
	env->sc_address.port         = SNMPD_PORT;
	strlcpy(env->sc_rdcommunity, "public", SNMPD_MAXCOMMUNITYLEN);
	strlcpy(env->sc_rwcommunity, "private", SNMPD_MAXCOMMUNITYLEN);
	strlcpy(env->sc_trcommunity, "public", SNMPD_MAXCOMMUNITYLEN);
	TAILQ_INIT(&env->sc_trapreceivers);

	gettimeofday(&env->sc_starttime, NULL);
	env->sc_engine_boots = 0;

	snmpd_generate_engineid(env);

	proc_init(ps, procs, nitems(procs));           /* gerlach/gh2289n: here the snmpe will be called */
	/* gerlach/gh2289n: we expect never to return to here because snmpe's shutdown will call _exit() */
	/*                  which calls the whole process                                                */

	obsd_snmpd_snmp_on();

	return (0);
}


void obsd_snmpd_handle_events(int flag, void *ctx_ptr) /* intented for nonblocking mode like used in Std. PN variant */
{
	OBSD_UNUSED_ARG(flag);
	OBSD_UNUSED_ARG(ctx_ptr);
    event_loop(EVLOOP_NONBLOCK|EVLOOP_ONCE);    /* don't wait here and assume PN has it's own cycle that calls*/
}


/* interface function for shutting down the whole SNMPD if we are using the PN standard varian    */
void obsd_snmpd_shutdown(void)
{
	obsd_snmpd_snmp_off();
	proc_kill(&static_snmpd_env.sc_ps);
}

int
snmpd_dispatch_snmpe(int fd, struct privsep_proc *p, struct imsg *imsg)
{
	OBSD_UNUSED_ARG(fd); /* OBSD_ITGR -- avoid warning */
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(imsg);
	return (-1);
}

                                                  /* own's the socket, so we can't open an        */
                                                  /* additional socket -> snmpd_socket_af not used*/
int
snmpd_socket_af(struct sockaddr_storage *ss, in_port_t port)
{
	int	 s;

	switch (ss->ss_family) {
	case AF_INET:
		((struct sockaddr_in *)ss)->sin_port = port;
		((struct sockaddr_in *)ss)->sin_len =
		    sizeof(struct sockaddr_in);
		break;
	case AF_INET6:
		((struct sockaddr_in6 *)ss)->sin6_port = port;
		((struct sockaddr_in6 *)ss)->sin6_len =
		    sizeof(struct sockaddr_in6);
		break;
	default:
		return (-1);
	}

	s = socket(ss->ss_family, SOCK_DGRAM, IPPROTO_UDP);
	return (s);
}

void
snmpd_generate_engineid(struct snmpd *env)
{
	u_int32_t		 oid_enterprise, rnd, tim;

	/* RFC 3411 */
	memset(env->sc_engineid, 0, sizeof(env->sc_engineid));
	oid_enterprise = htonl(OIDVAL_openBSD_eid);
	memcpy(env->sc_engineid, &oid_enterprise, sizeof(oid_enterprise));
	env->sc_engineid[0] |= SNMP_ENGINEID_NEW;
	env->sc_engineid_len = sizeof(oid_enterprise);

	/* XXX alternatively configure engine id via snmpd.conf */
	env->sc_engineid[(env->sc_engineid_len)++] = SNMP_ENGINEID_FMT_EID;
	rnd = arc4random();
	memcpy(&env->sc_engineid[env->sc_engineid_len], &rnd, sizeof(rnd));
	env->sc_engineid_len += sizeof(rnd);

	tim = htonl(env->sc_starttime.tv_sec);
	memcpy(&env->sc_engineid[env->sc_engineid_len], &tim, sizeof(tim));
	env->sc_engineid_len += sizeof(tim);
}

u_long
snmpd_engine_time(void)
{
	struct timeval	 now;

	/*
	 * snmpEngineBoots should be stored in a non-volatile storage.
	 * snmpEngineTime is the number of seconds since snmpEngineBoots
	 * was last incremented. We don't rely on non-volatile storage.
	 * snmpEngineBoots is set to zero and snmpEngineTime to the system
	 * clock. Hence, the tuple (snmpEngineBoots, snmpEngineTime) is
	 * still unique and protects us against replay attacks. It only
	 * 'expires' a little bit sooner than the RFC3414 method.
	 */
	gettimeofday(&now, NULL);
	return now.tv_sec;
}

/* see TFS Task 1194574 */
/**
    @brief          Opens and binds the socket for the SNMP Agent SNMPD
    @param          -
    @note           Use this function under the following restrictions
                    @li     The SNMPD agent has to be started by calling snmpd_main() before using this function
                    @li     This function is automatically called by snmpd_main() too.
                    @li     Code that calls this function should be run in the same thread / process like
                            the caller main/snmpd_main (other scenarios currently not supported)
    @return         0 in case of success
    @return         -1 in case of error (double call to obsd_snmpd_snmp_on or SNMPD not initialized (snmpd_main() not called)
*/
int obsd_snmpd_snmp_on(void)
{
	return obsd_snmpe_socket_open();
}

/**
    @brief          Closes the socket for the SNMP Agent SNMPD
    @param          -
    @note           Use this function under the following restrictions
                    @li     This function doesn't completely shutdown the SNMP Agent SNMPD. 
                            So -if needed the SNMPD has to be shutdown separately by calling 
                            obsd_snmpd_shutdown() (or killing the process in a multithreaded environment)
                    @li     This function is automatically called by obsd_snmpd_shutdown(). 
                    @li     Code that calls this function should be run in the same thread / process like
                            the caller main/snmpd_main (other scenarios currently not supported)
    @return         0 in case of success
    @return         -1 in case of error (double call to obsd_snmpd_snmp_off() or obsd_snmpd_snmp_on() not called before)
*/
int obsd_snmpd_snmp_off(void)
{
	return obsd_snmpe_socket_close();
}


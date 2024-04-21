/*	$OpenBSD: snmpe.c,v 1.41 2015/10/08 08:17:30 sthen Exp $	*/

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

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_act_modul) */
#define LTRC_ACT_MODUL_ID 4056 /* OBSD_ITGR added for LSA traces */
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_act_modul) */
#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_stat.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_un.h>
#include <sys/obsd_kernel_tree.h>

#include <net/obsd_kernel_if.h>
#include <netinet/obsd_kernel_in.h>
#include <arpa/obsd_userland_inet.h>

#include <obsd_userland_stdlib.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_errno.h>
#include <obsd_userland_event.h>
#include <obsd_kernel_fcntl.h>
#include <obsd_userland_string.h>
#include <obsd_userland_unistd.h>

#include <net/obsd_kernel_if_types.h>

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_mib.h"

#include "obsd_platform_mibal.h"    /*                  MIBAL interface MIBAL_Init(),MIBAL_DeInit */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_include_trace_header) */
#include <tcip_obsd_trc.h>
/* OBSD_TRACE_PATCH_END - patch no. (obsd_include_trace_header) */

extern struct ifnet **ifindex2ifnet;

static int is_unknown_community(char *cname);

typedef struct
{
	int                     ref_cnt;
	int                     fd;
	struct snmp_message     msg;
	struct snmp_stats       *stats;
	struct sockaddr			local_addr;	   /* source address for sending response */
	int						local_addrlen;  /* address length */
} SnmpdResponseContext_t;

typedef enum
{
	vo_undefined = -1,/* used for initialization purposes, should never used in operation*/
	vo_new_var_request = 0, /* got a new var request from a snmp packet                        */
	vo_new_bulkop = 1, /* we are about to start a new bulkget operation                   */
	vo_varop_in_progress = 2, /* wait for var operation (get, set, ...), typically for async ops */
	vo_varop_completed = 3, /* var operation completed, e.g. by a async callback               */
	vo_varop_failed = 4,
	vo_bulkop_completed = 5  /* bulkget completed */
} varop_state_t;

typedef struct
{
	int                     ctx_ref_cnt;
	struct ber_oid          o;
	struct ber_element      *d;
	SnmpdResponseContext_t  *pRespCtx;  /* context needed to send an response message */
										/* var's for SNMP_C_GETBULKREQ */
	u_int                   j;
	struct ber_element	    *e;
	size_t                  len;
	varop_state_t           varop_state;
} MibVarbindHandlingContext_t;

static recv_msg_evt_chain_arg_t EvtHdlChain;

void	 snmpe_init(struct privsep *, struct privsep_proc *, void *);
int	 snmpe_parse(SnmpdResponseContext_t  *pRespCtx);
int	 snmpe_parsevarbinds(SnmpdResponseContext_t  *pRespCtx);
int obsd_snmpd_check_varbinds(struct ber_element* varbinds);

static void snmpe_respond_msg(SnmpdResponseContext_t * pRespCtx);
static int mib_handle_vars_and_respond(MibVarbindHandlingContext_t *ctx, varop_state_t new_state);

unsigned long
	 snmpe_application(struct ber_element *);
void	 snmpe_sig_handler(int sig, short, void *);
int	 snmpe_dispatch_parent(int, struct privsep_proc *, struct imsg *);
int	 snmpe_bind(struct address *);
void	 snmpe_recvmsg(int fd, short, void *);
int	 snmpe_encode(struct snmp_message *);
void	 snmp_msgfree(struct snmp_message *);

struct snmpd	*env = NULL;

struct imsgev	*iev_parent;

static struct privsep_proc procs[] = {
	{ "parent",	PROC_PARENT,	snmpe_dispatch_parent }
};

pid_t
snmpe(struct privsep *ps, struct privsep_proc *p)
{
	env = ps->ps_env;
	return (proc_run(ps, p, procs, nitems(procs), snmpe_init, NULL));
}

/* ARGSUSED */
void
snmpe_init(struct privsep *ps, struct privsep_proc *p, void *arg)
{
	OBSD_UNUSED_ARG(ps);
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(arg);
	kr_init();
	MIBAL_Init();                                 /* initialize the underlying platform interface */
}

void
snmpe_shutdown(void)
{
	struct privsep *ps = &env->sc_ps;
	kr_shutdown();
	MIBAL_DeInit();                  /* deinitialize the underlying platform interface, now it is */
	           
									/* safe because the SNMP port / socket is closed             */
	event_base_free(ps->sc_evbase);
	env = NULL;                      /* invaidate local env */
}

int
snmpe_dispatch_parent(int fd, struct privsep_proc *p, struct imsg *imsg)
{
	OBSD_UNUSED_ARG(fd);
	OBSD_UNUSED_ARG(p);
	OBSD_UNUSED_ARG(imsg);
	return (-1);
}

                                                  /* own's the socket, so we can't bind an own    */
                                                  /* socket -> snmpe_bind not used                */
int
snmpe_bind(struct address *addr)
{
	int	 s;

	if ((s = snmpd_socket_af(&addr->ss, htons(addr->port))) == -1)
		return (-1);

	/*
	 * Socket options
	 */
	if (fcntl(s, F_SETFL, O_NONBLOCK) == -1)
		goto bad;

	if (bind(s, (struct sockaddr *)&addr->ss, addr->ss.ss_len) == -1)
		goto bad;


	return (s);

 bad:
	close(s);
	return (-1);
}

static void mib_async_req_done(     void                *cb_ctx,
                                    struct ber_element  *result_ber_elem,
                                    struct ber_oid      *result_elem_oid,
                                    int                 snmp_op,
                                    int                 snmp_response,
        /* snmp_varbind_choice_t */ int                 varbind_choice,
                                    struct oid*         getnext_last_oid
                                  )
{
    MibVarbindHandlingContext_t *ctx        = (MibVarbindHandlingContext_t * )cb_ctx;
    varop_state_t               new_state   = vo_undefined;

    OBSD_UNUSED_ARG(varbind_choice);        /* currently we don't use the varbind choices because */
                                            /* of the  missing infrastructure in OBSD's SNMPD     */
    OBSD_UNUSED_ARG(result_ber_elem);       /* currently not needed                               */

    /* tail of mps_getnextreq processing which has to be done in the callback */
    if ( (snmp_op == SNMP_C_GETNEXTREQ) 
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
		 || (snmp_op == SNMP_C_GETBULKREQ)     /* GETBULKREQ uses mps_getnextreq too */
#endif
       )
    {   
        if ( snmp_response == SNMP_ERROR_NOSUCHNAME)  /* end of current MIB, didn't get the next OID */
        {   /* assume the next OID was not found, repeat the request trying the next MIB */
            struct oid      *oha_next       = NULL;

            new_state = vo_varop_failed;    /* a long as we don't find an other OID in an other MIB */

            for (oha_next = getnext_last_oid; oha_next != NULL;) 
            {
                oha_next = smi_next(oha_next);
                if (oha_next->o_get == NULL)
                {
                    continue; /* only interested in MIB entries with a get function */
                }
                if ( (oha_next == NULL) || (OID_NOTSET(oha_next)))
                {
                    break;  /* break loop, no other MIB found */
                }
                bcopy(&oha_next->o_id, &(ctx->o), sizeof((ctx->o)));
                new_state = vo_varop_in_progress;   /* as long we have a suited OID we can try this   */
                break;                  /* break loop and continue with the first OID of the next MIB */
            } /* end for */
        }
        else /* assume we always get the next OID from LLDP/MRP MIB Request in result_elem_oid        */
        {   /* SNMP_ERROR_NONE or some other error than SNMP_ERROR_NOSUCHNAME */
            if ( snmp_response != SNMP_ERROR_NONE )
            {
                new_state = vo_varop_failed;
            }
            else /* snmp_response == SNMP_ERROR_NONE */
            {
                new_state = vo_varop_completed;
            }

            if (result_elem_oid != NULL)
            {   /* refresh the OID in the context to the current one. This has to be done for success */
                /* case (to deliver the value for the right OID but for the error case too. Otherwise */
                /* the snmp response reports the wrong (the old) OID to be responsible for the error  */
                bcopy(result_elem_oid, &(ctx->o), sizeof(struct ber_oid));
            }
        } /* end else if */
    }
    else
    {   /* all other SNMP request types */
        if ( snmp_response != SNMP_ERROR_NONE )
        {
            new_state = vo_varop_failed;
        }
        else /* snmp_response == SNMP_ERROR_NONE */
        {
            new_state = vo_varop_completed;
        }
    }

    if ( new_state == vo_varop_failed)
    {   /* add the error code to the response */
        ctx->pRespCtx->msg.sm_error = snmp_response; /* e.g. SNMP_ERROR_NOSUCHNAME; */
    }

    mib_handle_vars_and_respond(ctx, new_state);   /* next iteration */
    /* this callback is completed, so we don't need this context reference any longer here */
    if (--ctx->ctx_ref_cnt == 0)      /* in a real multithreading szenario we would need to protect this access */
    {   /* nobody else holds a context reference, so we could free the context */
        free(ctx);                      /* ctx is also used by the receiving thread */
        /* ctx=NULL */
    }
}

/* returns: -1: in case of error (currently no such case exists) - no async callback is triggered,*/
/*              no context is referenced in an other thread / callback, ctx is not used any longer*/
/*           0: request completed, response is sent,                                              */
/*              no context is referenced in an other thread / callback, ctx is not used any longer*/
/*           1: async callback pending, ctx externally referenced (needed for a callback),        */
/*              ctx ref-Counter is incremented                                                    */
static int mib_handle_vars_and_respond(MibVarbindHandlingContext_t *ctx, varop_state_t new_state)
{
	struct snmp_message *msg = &ctx->pRespCtx->msg;
	int			 ret = 0;

/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_003) */
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "IN: mib_handle_vars_and_respond, context 0x%x", (uint32_t)ctx);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_003) */

	ctx->varop_state = new_state;

    for (/*msg->sm_i = 1, msg->sm_a = msg->sm_varbind, msg->sm_last = NULL*/ ;
          msg->sm_a != NULL && msg->sm_i < SNMPD_MAXVARBIND; msg->sm_a = msg->sm_next, msg->sm_i++) 
    {
        if (ctx->varop_state         == vo_new_var_request)
        {   /* operations that are needed after we got a new var request */
            msg->sm_next = msg->sm_a->be_next;

            if (msg->sm_a->be_class != BER_CLASS_UNIVERSAL ||
                msg->sm_a->be_type != BER_TYPE_SEQUENCE)
                continue;
            if ((msg->sm_b = msg->sm_a->be_sub) == NULL) /* gh2289n:  ctx->b holds the varbind now */
                continue;
            msg->sm_state = 0;
        }
        /* gh2289n: here we would line to do this:                                                                */
        /* for (state = 0; state < 2 && ctx->b != NULL; (ctx->b != NULL) && ((ctx->b = ctx->b->be_next) != NULL)) */
        /* but because of the weakness of some PN compiler (for MIPS) we get a warning here                       */
        /* (a la "value computed is not used") - the devel knows why.... So we had to change into a while loop to */
        /* make this damn compiler happy. This works until we need a continue into this loop ....                 */

        while (msg->sm_state < 2 /* && msg->sm_b != NULL*/ ) /* gh2289n: can't check ctx->b here because we enter the   */
                                                       /* loop code a second time in async case (via async        */
                                                       /* callback). If we enter the code the first time (new     */
                                                       /* request) ctx-> can't be NULL because we continue the    */
                                                       /* outer for loop in this case before arriving here. So no */
                                                       /* problem for state == 0. For state == 1 we use ctx->b    */
                                                       /* only in SNMP_C_SETREQ so we have to do the NULL check   */
                                                       /* there before using ctx->b. We can combine this with     */
                                                       /* advancing b to b->be_next there.                        */
        {
            /* switch (ctx->state++)   not possible because we need to enter the getbulk loop in the same state */
            switch (msg->sm_state)
            {
                case 0:
                    msg->sm_state++; 
                    if (ber_get_oid(msg->sm_b, &ctx->o) != 0) /* gh2289n: get the OID from the varbind */
                        goto varfail;
                    if (ctx->o.bo_n < BER_MIN_OID_LEN ||
                        ctx->o.bo_n > BER_MAX_OID_LEN)
                        goto varfail;
                    if (ctx->pRespCtx->msg.sm_context == SNMP_C_SETREQ)
                        ctx->pRespCtx->stats->snmp_intotalsetvars++; /* gh2289n: maybe would need a Critical Section for some pstats elements here if we want real multithreading for the callback mib_async_req_done */
                    else
                        ctx->pRespCtx->stats->snmp_intotalreqvars++; /* gh2289n: maybe would need a Critical Section for some pstats elements here if we want real multithreading for the callback mib_async_req_done */
                    log_debug("snmpe_parse: %s: oid %s", ctx->host,
                        smi_oid2string(&ctx->o, buf, sizeof(buf), 0));
                    break;
                case 1:
                    if (ctx->varop_state == vo_new_var_request)
                    { 
                        msg->sm_c = NULL;
						msg->sm_end = NULL;
                    }
                    switch (msg->sm_context) 
                    {
                        case SNMP_C_GETNEXTREQ:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_004) */
							TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "mib_handle_vars_and_respond: SNMP_C_GETNEXTREQ");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_004) */
                            if (ctx->varop_state == vo_new_var_request)
                            {
								msg->sm_c = ber_add_sequence(NULL);
								if (msg->sm_c) /* RQ 1944871 -- NULL pointer exception */
								{
									msg->sm_c->oha_par.be_async_cb = mib_async_req_done;       /* gh2289n: new struct element    */
									msg->sm_c->oha_par.be_async_cbarg = ctx;                      /* gh2289n: new struct element    */
									msg->sm_c->oha_par.snmp_operation = SNMP_C_GETNEXTREQ;        /* gh2289n: new struct element    */
									ctx->varop_state = vo_varop_in_progress;     /* mark that we are in progress   */
								}
								else
								{
									ctx->varop_state = vo_varop_failed;
									msg->sm_error = SNMP_ERROR_GENERR; /* out of memory */
								}
                            }
                            if (ctx->varop_state == vo_varop_in_progress)
                            {
                                /* maybe ctx will be given to the async handling and is needed until the callback is called   */
                                ctx->ctx_ref_cnt++;          /* in a real multithreading szenaraio we would need to protect   */
                                                             /* this access*/
								msg->sm_c->oha_par.wait_for_async_completion = 0; /* sado -- OpenBSD 5.9, reset async_completion, set during mps_getnetreq */

                                ret = mps_getnextreq(msg, msg->sm_c, &ctx->o);     /* may trigger an async callback  */
                                if (ret == 0)
                                {   /* request OK */
									if (msg->sm_c->be_type == SNMP_NOT_SUPPORTED) /* new getnext-request with the retured object */
									{
										ber_get_oid(msg->sm_c, &ctx->o);
										ber_free_elements(msg->sm_c);
										msg->sm_c = NULL;
										ctx->varop_state = vo_new_var_request;
										ctx->ctx_ref_cnt--;
										break;
									}
									
									if (msg->sm_c->oha_par.wait_for_async_completion == 0)
                                    {
                                        ctx->varop_state            = vo_varop_completed;  /* got result synchr. */
                                        ctx->ctx_ref_cnt--;  /* context not needed any longer because of sync operation */
                                    }
                                    /* else: let ctx->varop_state set to vo_varop_in_progress */
                                }
                                else /* request failed */
                                {
                                    ctx->varop_state = vo_varop_failed;
                                    msg->sm_error = SNMP_ERROR_NOSUCHNAME; /* set SNMP error, needed for synchronous case       */
                                    ctx->ctx_ref_cnt--;  /* context not needed any longer because no async callback triggered because of error*/
                                }
                            }
                            if (ctx->varop_state == vo_varop_failed )
                            {   /* used for sync. and async. operation */
								if (msg->sm_c)
								{
									ber_free_elements(msg->sm_c);
									msg->sm_c = NULL;
								}
                                /* ctx->d = NULL; assume ctx->d is not needed afterwards */
                                /* ctx->pRespCtx->msg.sm_error = ... - assume SNMP error was already set earlier         */
                                /*                                     (e.g. by async callback in async. case)           */
                                msg->sm_errorindex = msg->sm_i;
                                /* ignore error and so don't go to varfail */
                            }
                            if (ctx->varop_state != vo_varop_in_progress )
                            {   /* (ctx->varop_state == vo_varop_completed) or ctx->varop_state == vo_varop_failed       */
                                ctx->varop_state = vo_varop_completed;  /* in case of vo_varop_failed we ignore it here  */
                                                                        /* to get msg->sm_varbindresp assigned right     */
                                                                        /* if we later need c to be in the response in   */
                                                                        /* case of errs other than SNMP_ERROR_NOSUCHNAME */
                                msg->sm_state++;     /* don't increment state if ctx->varop_state == vo_varop_in_progress,  */
                                                  /* because we need to get here if async operation is completed         */
                            }
                            break;  /* switch */
                        case SNMP_C_GETREQ:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_005) */
							TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "mib_handle_vars_and_respond: SNMP_C_GETREQ");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_005) */
                            if (ctx->varop_state == vo_new_var_request)
                            {
								msg->sm_c = ber_add_sequence(NULL);
								if (msg->sm_c) /* RQ 1944871 -- NULL pointer exception */
								{
									msg->sm_c->oha_par.be_async_cb = mib_async_req_done;       /* gh2289n: new struct element    */
									/* ctx was given to the async handling and is needed until the callback is called             */
									ctx->ctx_ref_cnt++;          /* in a real multithreading szenaraio we would need to protect   */
																 /* this access                                                   */
									msg->sm_c->oha_par.be_async_cbarg = ctx;                      /* gh2289n: new struct element    */
									msg->sm_c->oha_par.snmp_operation = SNMP_C_GETREQ;            /* gh2289n: new struct element    */
									ctx->varop_state = vo_varop_in_progress;     /* mark that we are in progress   */
									ret = mps_getreq(msg, msg->sm_c, &ctx->o, msg->sm_version);  /* may trigger an async callback  */
									if (ret == 0)
									{   /* request OK */
										if (msg->sm_c->be_type == SNMP_NOT_SUPPORTED)
										{
											ctx->varop_state = vo_varop_failed;
											msg->sm_error = SNMP_ERROR_NOSUCHNAME; /* set SNMP error, needed for synchronous case       */
											ctx->ctx_ref_cnt--;  /* context no needed any longer because no async callback triggered because of error */
										}
										else if (msg->sm_c->oha_par.wait_for_async_completion == 0)
										{
											ctx->varop_state = vo_varop_completed;  /* got result synchr. */
											ctx->ctx_ref_cnt--;  /* context no needed any longer because of sync operation */
										}
										/* else: let ctx->varop_state set to vo_varop_in_progress */
									}
									else /* request failed */
									{
										ctx->varop_state = vo_varop_failed;
										if (ret == -1) msg->sm_error = SNMP_ERROR_NOSUCHNAME; /* set SNMP error, needed for synchronous case       */
										else msg->sm_error = ret;
										ctx->ctx_ref_cnt--;  /* context no needed any longer because no async callback triggered because of error */
									}
								}
								else /*msg->sm_c == NULL*/
								{
									ctx->varop_state = vo_varop_failed;
									msg->sm_error = SNMP_ERROR_GENERR; /* out of memory */
								}
                            }
                            if (ctx->varop_state == vo_varop_failed)
                            {   /* used for sync. and async. operation */
								if (msg->sm_c)
								{
									ber_free_elements(msg->sm_c);
									msg->sm_c = NULL;
								}
                                /* ctx->d = NULL; - not necessary doning this, assume ctx->d is not used afterwards */
                                /* ctx->pRespCtx->msg.sm_error = ... - assume SNMP error was already set earlier         */
                                /*                                     (e.g. by async callback in async. case)           */
                                goto varfail;
                            }
                            if (ctx->varop_state == vo_varop_completed)
                            {   /* if we continue here we are in a synchronous szenario (e.g. MIB-2 Variables) or got back the async result   */
                                msg->sm_state++;   /* next state if we completed handling for the current/pending request */
                            }
                            break;
                        case SNMP_C_SETREQ:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_006) */
							TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "mib_handle_vars_and_respond: SNMP_C_SETREQ");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_006) */
                            if (ctx->varop_state == vo_new_var_request)
                            {
                                if (msg->sm_b != NULL)
                                    msg->sm_b = msg->sm_b->be_next;   /* get the value to set from the varbind                      */
                                else
                                    goto varfail;               /* can't continue set op without a varbind value for the OID  */
                                msg->sm_b->oha_par.be_async_cb     = mib_async_req_done;       /* gh2289n: new struct element    */
                                /* ctx was given to the async handling and is needed until the callback is called             */
                                ctx->ctx_ref_cnt++;          /* in a real multithreading szenaraio we would need to protect   */
                                                             /* this access                                                   */
                                msg->sm_b->oha_par.be_async_cbarg  = ctx;                      /* gh2289n: new struct element    */
                                msg->sm_b->oha_par.snmp_operation  = SNMP_C_SETREQ;            /* gh2289n: new struct element    */
                                msg->sm_b->oha_par.wait_for_async_completion = 0;
                                ctx->varop_state                = vo_varop_in_progress;     /* mark that we are in progress   */
                                if (env->sc_readonly == 0)
								{
                                    ret = mps_setreq(msg, msg->sm_b, &ctx->o);  /* may trigger an async callback  */
 									if (ret == 0)                                                                                    
                                	{   /* request OK */
                                    	if (msg->sm_b->oha_par.wait_for_async_completion == 0)
                                    	{
                                        	ctx->varop_state            = vo_varop_completed;  /* got result synchr. */
                                        	ctx->ctx_ref_cnt--;  /* context no needed any longer because of sync operation */
                                    	}
                                    	/* else: let ctx->varop_state set to vo_varop_in_progress */
                                	}
                                	else /* request failed */
                                	{
                                    	ctx->varop_state            = vo_varop_failed;
                                    	/* set SNMP error, needed for synchronous case. Original OBSD SNMPD always uses SNMP_ERROR_READONLY here  */
                                    	/* but we know a little bit better now ...                                                                */
                                    	/* OpenBSD's SNMPD too (don't have more details info)*/
                                    	switch (ret)
                                    	{
                                        	case OBSD_SNMPD_MPS_ERRCODE_WRONG_LENGTH:
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
											if (msg->sm_version == SNMP_V2)
												msg->sm_error = SNMP_ERROR_WRONGLENGTH;
											else
#endif
												msg->sm_error = SNMP_ERROR_BADVALUE;
											break;
											case OBSD_SNMPD_MPS_ERRCODE_BADVALUE:		msg->sm_error = SNMP_ERROR_BADVALUE; break;
											case OBSD_SNMPD_MPS_ERRCODE_NOSUCHNAME:		msg->sm_error = SNMP_ERROR_NOSUCHNAME; break;
											case OBSD_SNMPD_MPS_ERRCODE_OK:             /* no break */ /* should never happen here */
                                        	case OBSD_SNMPD_MPS_ERRCODE_READ_ONLY:      /* no break */
                                        	case OBSD_SNMPD_MPS_ERRCODE_GENERIC_ERROR:  /* no break */
                                        	default:                                    /* in all other cases we don't know better, behave like OBSDs SNMPd does */
                                                                                    	msg->sm_error = SNMP_ERROR_READONLY; break;
                                    	}

                                    	ctx->ctx_ref_cnt--;  /* context no needed any longer because no async callback triggered because of error */
                                	}
								}
                            }
                            if (ctx->varop_state == vo_varop_failed)
                            {   /* used for sync. and async. operation */
                                /* ctx->pRespCtx->msg.sm_error = ... - assume SNMP error was already set earlier         */
                                /*                                     (e.g. by async callback in async. case)           */
                                goto varfail;
                            }
                            if (ctx->varop_state == vo_varop_completed)
                            {   /* if we continue here we are in a synchronous szenario (e.g. MIB-2 Variables) or got back the async result   */
                                msg->sm_state++;   /* next state if we completed handling for the current/pending request */
                            }
                            break;
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
                        case SNMP_C_GETBULKREQ:
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_007) */
							TCIP_OBSD_TRACE_00(0, OBSD_TRACE_LEVEL_CHAT, "mib_handle_vars_and_respond: SNMP_C_GETBULKREQ");
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_007) */
                            if (ctx->varop_state == vo_new_var_request)
                            {
                                /* preparation of getbulk loop iterations, setup the state variables for a single getbulkreq handling */
                                ctx->j = (u_int) msg->sm_maxrepetitions; /* OBSD_ITGR -- avoid warning */
                                msg->sm_errorindex = 0;
                                msg->sm_error = SNMP_ERROR_NOSUCHNAME;
                                ctx->d = NULL;  /* initial part of the original for loop */
                                ctx->len = 0;   /* initial part of the original for loop */
                                ctx->varop_state = vo_new_bulkop;
                            }
                            for (/*ctx->d = NULL, ctx->len = 0*/; ctx->j > 0; ctx->j--)  /* for loop needed for synchronous operation (MIB-2 OIDs */
                            {
                                if (ctx->varop_state == vo_new_bulkop)
                                {
                                    ctx->e = ber_add_sequence(NULL); /* RQ 1944871 -- NULL pointer exception */
									if (ctx->e)
									{
										if (msg->sm_c == NULL)
											msg->sm_c = ctx->e;
										/* prepare parameters needed for a async var operation (OID values that we can get async only */
										ctx->e->oha_par.be_async_cb = mib_async_req_done;       /* gh2289n: new struct element    */
										ctx->e->oha_par.be_async_cbarg = ctx;                      /* gh2289n: new struct element    */
										ctx->e->oha_par.snmp_operation = SNMP_C_GETBULKREQ;        /* gh2289n: new struct element    */
										ctx->varop_state = vo_varop_in_progress;     /* mark that we are in progress   */
									}
									else /* ctx->e == NULL */
									{
										ctx->varop_state = vo_varop_failed;
										msg->sm_error = SNMP_ERROR_GENERR; /* out of memory */
									}
                                }
                                if (ctx->varop_state == vo_varop_in_progress)
                                {
                                    /* maybe ctx will be given to the async handling and is needed until the callback is called   */
                                    ctx->ctx_ref_cnt++;          /* in a real multithreading szenaraio we would need to protect   */
                                                                 /* this access                                                   */
									ctx->e->oha_par.wait_for_async_completion = 0; /* sado -- OpenBSD 5.9, reset async_completion, set during mps_getnetreq */
                                    ret = mps_getnextreq(msg, ctx->e, &ctx->o);                        /* may trigger an async callback  */
                                    /* in syncr. case f is a newly created ber_element (new allocation). Assume that in sync.     */
                                    /* case f->oha_par.wait_for_async_completion is initialized with 0 (SNMPD's internal MIB      */
                                    /* doesn't know about the oha_par substructure, but, thanks god clears the memory for the new */
                                    /* ber_element by using calloc in ber_get_element for it's allocation)                        */
									if (ret == -1) {
                                        ctx->varop_state            = vo_varop_failed;
                                        ctx->ctx_ref_cnt--;  /* context not needed any longer because no async callback triggered because of error */
                                        /* done below, see case ctx->varop_state == varop_failed
                                        ber_free_elements(ctx->e);
                                        if (ctx->d == NULL)
                                            goto varfail;
                                        break;
                                        */
                                    }
                                    else
                                    {
                                        if (ctx->e->oha_par.wait_for_async_completion == 0)
                                        {   /* if we continue here we are in a synchronous szenario (e.g. MIB-2 Variables) */
                                            ctx->varop_state = vo_varop_completed;
                                            ctx->ctx_ref_cnt--;  /* context not needed any longer because of sync operation*/
                                        }
                                        /* else: let ctx->varop_state set to vo_varop_in_progress */
                                    }
                                } /* endif (ctx->varop_state == varop_new_var_request) */
                                if (ctx->varop_state == vo_varop_failed)
                                {
									if (ctx->e)
									{
										ber_free_elements(ctx->e);
										ctx->e = NULL;
									}
                                    if (ctx->d == NULL)
                                    {   /* got nothing at all so far, return with msg.sm_error = SNMP_ERROR_NOSUCHNAME */
                                        goto varfail;
                                    }
                                    break; /* break the bulkget loop */
                                }
                                if (ctx->varop_state == vo_varop_completed)
                                {   /* if we continue here we are in a synchronous szenario (e.g. MIB-2 Variables) or got back the async result */
									if (ctx->e->be_type != SNMP_NOT_SUPPORTED) /* supported counters (SPH) */
									{
										ctx->len += ber_calc_len(ctx->e);
										if (ctx->len > SNMPD_MAXVARBINDLEN) {
											ber_free_elements(ctx->e);
											ctx->e = NULL;
											break; /* break getbulk loop */
										}
										if (ctx->d != NULL)
											ber_link_elements(ctx->d, ctx->e);
										ctx->d = ctx->e;
									}
									else
									{
										if (msg->sm_c == ctx->e) msg->sm_c = NULL;
										ber_free_elements(ctx->e);
										ctx->e = NULL;
										ctx->j++;
									}
                                    /* continue with the next bulkreq loop iteration */
                                    ctx->varop_state = vo_new_bulkop;
                                }
                                if (ctx->varop_state == vo_varop_in_progress)
                                {
                                    break;  /* break the for loop because we have to wait for async completion before we can continue the loop */
                                }
                            } /* for */
                            if (ctx->varop_state != vo_varop_in_progress)
                            {
                                /* if we arrive here (with or without an error) we got at least one OID linked in the ctx->d chain */
                                ctx->varop_state = vo_bulkop_completed; /* for loop completed */
								msg->sm_state++;   /* don't change state until we have completed the getbulk loop */
                                msg->sm_error = 0; /* because we have something to deliver back in the ctx->d chain */
                            }
                            break; /* switch */
#endif /* OBSD_PNIO_SUPPORT_SNMPV2C */
                        default:
							msg->sm_state++;
                            goto varfail;
                    } /* switch (msg->sm_context) */
                    if (ctx->varop_state == vo_varop_in_progress)
                        break;
                    if (msg->sm_c == NULL)
                        break;
                    if ((ctx->varop_state == vo_varop_completed) || 
                        (ctx->varop_state == vo_bulkop_completed)
                        )
                    {
 					   if (msg->sm_end == NULL)
							msg->sm_end = msg->sm_c;
                       if (msg->sm_last == NULL)
                            msg->sm_varbindresp = msg->sm_c;
                       else
                            ber_link_elements(msg->sm_last, msg->sm_c);
                       msg->sm_last = msg->sm_end;
                    }
                    break;

				default:		/* mh2290: LINT 744 switch statement has no default */
					break;
            } /* switch (state++) */
        /* } */ /* for (state =  */ /* damn complier requests us to change "for" into "while" here */
            /* if (ctx->b != NULL)  gh2289n: done in SNMP_C_SETREQ where we need the varbind value */
            /*    ctx->b = ctx->b->be_next;                                                        */

            if (ctx->varop_state == vo_varop_in_progress)
            {
                break;  /* need to leave the while loop because waiting for the async result */
            }
        } /* while */
        if ((ctx->varop_state == vo_varop_completed) || 
            (ctx->varop_state == vo_bulkop_completed)
            )
            if (msg->sm_state < 2)  {
                log_debug("snmpe_parse: state %d", ctx->state);
                goto varfail;
            }
        if (ctx->varop_state == vo_varop_in_progress)
        {   /* don't continue the loop because we have to wait for the async completion of the    */
            /* current element value                                                              */
            return 1;               /* return 1 to the caller, continuation is triggered by the   */
                                    /* async callback                                             */
                                    /* note: handling of further requests is blocked via          */
                                    /*       snmp_request_in_progress until the async callback    */
                                    /*       triggers the call of snmpe_respond_msg               */
        }
        if (msg->sm_next != NULL)
        {   /* seems that we have some more OIDs to handle */
            ctx->varop_state = vo_new_var_request;     /* prepare state for the next iteration   */
        }
    } /* for */

send:
    snmpe_respond_msg(ctx->pRespCtx);/* send the response */
    ctx->pRespCtx = NULL; /* not longer needed, assume that is free'd in snmpe_respond_msg */
/* OBSD_TRACE_PATCH_START - patch no. (obsd_snmpd_snmpe_008) */
	TCIP_OBSD_TRACE_01(0, OBSD_TRACE_LEVEL_CHAT, "OUT: mib_handle_vars_and_respond with send, context 0x%x", (uint32_t)ctx);
/* OBSD_TRACE_PATCH_END - patch no. (obsd_snmpd_snmpe_008) */
    return (0); /* request response completed, ctx itself is not used any longer an could be freed by the caller if necessary */

varfail:
	if (msg->sm_error == 0)
		msg->sm_error = SNMP_ERROR_GENERR;
	msg->sm_errorindex = msg->sm_i;
    goto send;
}


static int is_unknown_community(char *cname)
{
	if (strcmp(env->sc_rdcommunity, cname) != 0 &&
		strcmp(env->sc_rwcommunity, cname) != 0 &&
		strcmp(env->sc_trcommunity, cname) != 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/* gh2289n: if we don't return -1 we have to ensure, that pRespCtx is free'd */
int snmpe_parse(SnmpdResponseContext_t  *pRespCtx)
{
	struct snmp_message *msg = &pRespCtx->msg;
	struct snmp_stats	*stats = &env->sc_stats;
	struct ber_element	*a;
	long long		 ver, req;
	long long		 errval, erridx;
	unsigned long		 type;
	u_int			 class;

	char			*comn;

	struct ber_element	*root = msg->sm_req;

	/* msg->sm_errstr = "invalid message"; -- Greenhills */ 

	if (ber_scanf_elements(root, "{ie", &ver, &a) != 0)
		goto parsefail;

	/* SNMP version and community */
	msg->sm_version = (u_int)ver; /* OBSD_ITGR -- avoid warning */
	switch (msg->sm_version) {
	case SNMP_V1:
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
	case SNMP_V2:
#endif
		if (env->sc_min_seclevel != 0)
			goto badversion;
		if (ber_scanf_elements(a, "se", &comn, &msg->sm_pdu) != 0)
			goto parsefail;
		if (strlcpy(msg->sm_community, comn,
		    sizeof(msg->sm_community)) >= sizeof(msg->sm_community)) {
			stats->snmp_inbadcommunitynames++;
			/* msg->errstr = "community name too long"; sado -- Greenhills compiler warning 550 - set but never used */
			goto fail;
		}
		break;
	default:
	badversion:
		stats->snmp_inbadversions++;
		/* msg->errstr = "bad snmp version"; sado -- Greenhills compiler warning 550 - set but never used */
		goto fail;
	}

	if (ber_scanf_elements(msg->sm_pdu, "t{e", &class, &type, &a) != 0)
		goto parsefail;

	/* SNMP PDU context */
	if (class != BER_CLASS_CONTEXT)
		goto parsefail;
	switch (type) {
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
	case SNMP_C_GETBULKREQ:
		if (msg->sm_version == SNMP_V1) {
			stats->snmp_inbadversions++;
			/* msg->errstr = "invalid request for protocol version 1"; sado -- Greenhills compiler warning 550 - set but never used */
			goto fail;
		}
		/* FALLTHROUGH */
#endif
	case SNMP_C_GETREQ:
		stats->snmp_ingetrequests++;
		/* FALLTHROUGH */
	case SNMP_C_GETNEXTREQ:
		if (type == SNMP_C_GETNEXTREQ)
			stats->snmp_ingetnexts++;
		if (msg->sm_version != SNMP_V3 &&
		    strcmp(env->sc_rdcommunity, msg->sm_community) != 0 &&
		    strcmp(env->sc_rwcommunity, msg->sm_community) != 0) 
		{
			if (is_unknown_community(msg->sm_community))
			{
				/* msg->errstr = "unknown community"; sado -- Greenhills compiler warning 550 - set but never used */
				stats->snmp_inbadcommunitynames++;
				goto fail;
			}
			else
			{
				stats->snmp_inbadcommunitynames++;
				/* msg->errstr = "wrong read community"; sado -- Greenhills compiler warning 550 - set but never used */
				goto fail_with_nosuchname;
			}
		}
		msg->sm_context = type;
		break;
	case SNMP_C_SETREQ:
		stats->snmp_insetrequests++;
		if (msg->sm_version != SNMP_V3 &&
		    strcmp(env->sc_rwcommunity, msg->sm_community) != 0)
		{
			if (is_unknown_community(msg->sm_community))
			{
				/* msg->errstr = "unknown community"; sado -- Greenhills compiler warning 550 - set but never used */
				stats->snmp_inbadcommunitynames++;
				goto fail;
			}
			else
			{
				stats->snmp_inbadcommunityuses++;
				/* msg->errstr = "wrong write community"; sado -- Greenhills compiler warning 550 - set but never used */
				goto fail_with_nosuchname;
			}
		}
		msg->sm_context = type;
		break;
	case SNMP_C_GETRESP:
		stats->snmp_ingetresponses++;
		/* msg->errstr = "response without request"; sado -- Greenhills compiler warning 550 - set but never used */
		goto parsefail;
	case SNMP_C_TRAP:
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
	case SNMP_C_TRAPV2:
#endif
		if (msg->sm_version != SNMP_V3 &&
		    strcmp(env->sc_trcommunity, msg->sm_community) != 0) {
			stats->snmp_inbadcommunitynames++;
			/* msg->errstr = "wrong trap community"; sado -- Greenhills compiler warning 550 - set but never used */
			goto fail;
		}
		stats->snmp_intraps++;
		/* msg->errstr = "received trap"; sado -- Greenhills compiler warning 550 - set but never used */
		goto fail;
	default:
		/* msg->errstr = "invalid context"; sado -- Greenhills compiler warning 550 - set but never used */
		goto parsefail;
	}

	/* SNMP PDU */
	if (ber_scanf_elements(a, "iiie{et",
	    &req, &errval, &erridx, &msg->sm_pduend,
	    &msg->sm_varbind, &class, &type) != 0) {
		stats->snmp_silentdrops++;
		/* msg->errstr = "invalid PDU"; sado -- Greenhills compiler warning 550 - set but never used */
		goto fail;
	}
	if (class != BER_CLASS_UNIVERSAL || type != BER_TYPE_SEQUENCE) {
		stats->snmp_silentdrops++;
		/* msg->errstr = "invalid varbind"; sado -- Greenhills compiler warning 550 - set but never used */
		goto fail;
	}

	/* RQ 2116385 -- malformed pkt, parse varbinds for OID and value */
	if (obsd_snmpd_check_varbinds(msg->sm_varbind) != 0)
	{
		stats->snmp_silentdrops++;
		goto fail;
	}

	msg->sm_request = req;
	msg->sm_error = errval;
	msg->sm_errorindex = erridx;

	return snmpe_parsevarbinds(pRespCtx);

 parsefail:
	stats->snmp_inasnparseerrs++;
 fail:
	return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR);

/* sado -- RQ 1710546 - response with no such name should be sent */
fail_with_nosuchname:
	msg->sm_context = type;
	if (ber_scanf_elements(a, "iiie{et",
		&req, &errval, &erridx, &msg->sm_pduend,
		&msg->sm_varbind, &class, &type) != 0) {
		stats->snmp_silentdrops++;
		/* msg->sm_errstr = "invalid PDU"; -- Greenhills */
		goto fail;
	}
	msg->sm_request = req;
	msg->sm_error = SNMP_ERROR_NOSUCHNAME;
	msg->sm_errorindex = 1;
	return (OBSD_SNMPD_MPS_ERRCODE_NOSUCHNAME);
}

int
snmpe_parsevarbinds(SnmpdResponseContext_t  *pRespCtx)
{
	struct snmp_message *msg;
	int rc;
	struct snmp_stats	*stats = &env->sc_stats;
	MibVarbindHandlingContext_t * pMibHdlContext;

	if (stats == NULL || pRespCtx == NULL) /* sado -- Task 2402776 */
	{
		return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR);
	}

	if (&pRespCtx->msg != NULL) /* sado -- Task 2402776 */
	{
		msg = &pRespCtx->msg;
	}
	else
	{
		return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR);
	}

	pMibHdlContext = calloc(1, sizeof(MibVarbindHandlingContext_t));
	/* RQ 1918011 _KLE_ avoid NULL Ptr */
	if(pMibHdlContext == NULL)
	{
		stats->snmp_outgenerrs++;
		/* errstr = "calloc error"; sado -- Greenhills compiler warning 550 - set but never used */
		return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR);
	}

	if (pMibHdlContext != NULL)
	{
		pMibHdlContext->ctx_ref_cnt = 0;
		pMibHdlContext->d = NULL;
		pMibHdlContext->e = NULL;
		pMibHdlContext->j = 0;
		pMibHdlContext->len = 0;
		pMibHdlContext->varop_state = vo_undefined;
	}
	else
	{
		return OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR;
	}

	pRespCtx->ref_cnt++;     /* in a real multithreading scenario we would need to protect this access            */
	pMibHdlContext->pRespCtx = pRespCtx; /* pRespCtx is referenced here */

	if (msg->sm_i == 0) {
		msg->sm_i = 1;
		msg->sm_a = msg->sm_varbind;
		msg->sm_last = NULL;
	}

	/* unless mib_handle_vars_and_respond doesn't return -1 it has to ensure that pRespCtx will be free'd and     */
	/* the critical section will be destroyed                                                                     */
	rc = mib_handle_vars_and_respond(pMibHdlContext, vo_new_var_request); /* handle the request an send the       */
																		  /* response, may trigger another        */
																		  /* Thread / async callback              */
	if (rc == -1)
	{   /* error occured, we expect that no one hold a reference to pMibHdlContext */
		if (pMibHdlContext->ctx_ref_cnt == 0)   /* in a real multithreading szenario we would need to protect this access */
		{   /* nobody has referenced our context, so free it */
			if (pMibHdlContext->pRespCtx != NULL)
			{
				pMibHdlContext->pRespCtx->ref_cnt--; /* not needed any longer */ /* in a real multithreading szenaraio we would need to protect this access */
			}
			if (pMibHdlContext)
			{
				free(pMibHdlContext);                 /* ctx is also used by the async handling thread */
				pMibHdlContext = NULL;
			}
		}
		return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR);
	}
	else if (rc == 0)
	{   /* request completed, response is sent, we don't expect that context is referenced in an other thread / callback */
		if (pMibHdlContext)
		{
			free(pMibHdlContext);                 /* free ctx because it's not used any longer */
			pMibHdlContext = NULL;
		}
		return (OBSD_SNMPD_MPS_ERRCODE_OK);
	}
	else
		return (OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR); /* rc == 1; asynchronous */
}

/* gh2289n: new implementation for the case that we get (some of) the MIB variables asynchronous (e.g. LLDP and MRP from PN OHA */
static int snmp_request_in_progress = 0;

void snmpe_recvmsg(int fd, short sig, void *arg)
{
    ssize_t                 len;
    SnmpdResponseContext_t  *pRespCtx   = NULL;
	int retval = 0;
	struct snmp_message	*msg;

	struct sockaddr to_addr;
	unsigned int tolen = sizeof(to_addr);
	int if_index = -1;

    pRespCtx    = malloc(sizeof(SnmpdResponseContext_t));
    if (pRespCtx == NULL)
        goto done;   /* memory allocation error */
    bzero(pRespCtx, sizeof(SnmpdResponseContext_t)); /* this also sets pRespCtx->ref_cnt = 0 */
	msg = &pRespCtx->msg;

    /* build response context */
    pRespCtx->fd        = fd;
    pRespCtx->stats     = &env->sc_stats;
	msg->sm_slen = sizeof(msg->sm_ss);

    /* own's the socket (and calls us with the data)*/
    /* so we can't call recvfrom without a socket   */
    OBSD_UNUSED_ARG(arg);
    OBSD_UNUSED_ARG(sig);

	bzero(&pRespCtx->local_addr, sizeof(pRespCtx->local_addr));
	pRespCtx->local_addrlen = 0;

	if ((len = obsd_pnio_recvfromto(fd, (char *)msg->sm_data, sizeof(msg->sm_data), 0, (struct sockaddr *)&msg->sm_ss, &msg->sm_slen, &to_addr, &tolen, &if_index)) < 1)
	{
		free(pRespCtx);
		return;
	}
	else
	{
		if (tolen > 0 && if_index >= 0)
		{
			struct ifnet *ifp = ifindex2ifnet[if_index];
			if (ifp->if_type != IFT_ETHER) /* only standard interfaces respond to SNMP requests */
			{
				free(pRespCtx);
				return;
			}
			memcpy(&pRespCtx->local_addr, &to_addr, tolen);
			pRespCtx->local_addrlen = tolen;
		}
		else /* no address information */
		{
			free(pRespCtx);
			return;
		}
	}

    pRespCtx->stats->snmp_inpkts++;
	msg->sm_datalen = (size_t)len;

    if (snmp_request_in_progress) /* avoid multiple requests at a time */
    {
        free(pRespCtx);
        return;
    }
    snmp_request_in_progress = 1;

	bzero(&msg->sm_ber, sizeof(msg->sm_ber));
	msg->sm_ber.fd = -1;
	ber_set_application(&msg->sm_ber, smi_application);
	ber_set_readbuf(&msg->sm_ber, msg->sm_data, msg->sm_datalen);

	msg->sm_req = ber_read_elements(&msg->sm_ber, NULL);
	if (msg->sm_req == NULL) {
		pRespCtx->stats->snmp_inasnparseerrs++;
		goto done;
	}

	retval = snmpe_parse(pRespCtx);
    if ( retval != OBSD_SNMPD_MIBAL_ERRCODE_OK) 
	{
		if (retval != OBSD_SNMPD_MPS_ERRCODE_GENERIC_ERROR) /* sado -- RQ 1710546 - Parse error, e.g. wrong write community */
		{
			/* RQ 1918046 _KLE_ set Ref Counter */
            pRespCtx->ref_cnt++;         /* in a real multithreading szenaraio we would need to protect   */
                                         /* this access                                                   */
			snmpe_respond_msg(pRespCtx);
			/* RQ 1918046 _KLE_ pRespCtx is freed allready in snmpe_respond_msg() */
			pRespCtx = NULL;
		}
		goto done;
    } /* else
        pmsg->sm_context = SNMP_C_GETRESP; */

    /*  response is sent by calling snmpe_respond_msg via snmpe_parse, pRespCtx has to be free'd there */
    /* OhaAsyncResponseSimThreadHandleRequest(); could call this here directly for easier debugging without OhaAsyncResponseSimThread */
    return;

done:
    if (pRespCtx != NULL)
    {
        if (pRespCtx->ref_cnt == 0)     /* in a real multithreading szenaraio we would need to protect this access */
        {
            snmp_msgfree(&pRespCtx->msg);
            free(pRespCtx);
            pRespCtx = NULL;
        }
    }
	snmp_request_in_progress = 0;
}

void
snmp_msgfree(struct snmp_message *msg)
{
	ber_free(&msg->sm_ber);
	if (msg->sm_req != NULL)
		ber_free_elements(msg->sm_req);
	if (msg->sm_resp != NULL)
		ber_free_elements(msg->sm_resp);
}

static void snmpe_respond_msg( SnmpdResponseContext_t * pRespCtx)
{
	struct snmp_message *msg = &pRespCtx->msg;
    ssize_t			 len;
    u_int8_t		*ptr = NULL;

    msg->sm_context = SNMP_C_GETRESP; /* as earlier done after successul calling snmpe_parse */

	if (msg->sm_varbindresp == NULL && msg->sm_pduend != NULL)
		msg->sm_varbindresp = ber_unlink_elements(msg->sm_pduend);

    /* gh2289n: maybe would need a Critical Section for some pstats elements here if we want real multithreading for the callback mib_async_req_done */
    switch (msg->sm_error) {
        case SNMP_ERROR_NONE:
            break;
        case SNMP_ERROR_TOOBIG:
            pRespCtx->stats->snmp_intoobigs++;
            break;
        case SNMP_ERROR_NOSUCHNAME:
            pRespCtx->stats->snmp_innosuchnames++;
            break;
        case SNMP_ERROR_BADVALUE:
            pRespCtx->stats->snmp_inbadvalues++;
            break;
        case SNMP_ERROR_READONLY:
            pRespCtx->stats->snmp_inreadonlys++;
            break;
        case SNMP_ERROR_GENERR:
        default:
            pRespCtx->stats->snmp_ingenerrs++;
            break;
    }

	/* Create new SNMP packet */
	if (snmpe_encode(msg) < 0)
		goto done;

	len = ber_write_elements(&msg->sm_ber, msg->sm_resp);
	if (ber_get_writebuf(&msg->sm_ber, (void *)&ptr) == -1)
		goto done;

	len = obsd_pnio_sendfromto(pRespCtx->fd, (void *)ptr, len, 0, &pRespCtx->local_addr, pRespCtx->local_addrlen, (struct sockaddr *)&msg->sm_ss, msg->sm_slen);

    if (len != -1)
        pRespCtx->stats->snmp_outpkts++;

done:
    if (--pRespCtx->ref_cnt == 0)     /* in a real multithreading szenaraio we would need to protect this access */
    {

        snmp_msgfree(&pRespCtx->msg);
        free(pRespCtx);
        pRespCtx = NULL;
    }
    snmp_request_in_progress = 0;
}

int
snmpe_encode(struct snmp_message *msg)
{
	struct ber_element	*ehdr;
	struct ber_element	*pdu, *epdu;

	msg->sm_resp = ber_add_sequence(NULL);

	if (msg->sm_resp == NULL) /* RQ 1944871 -- NULL pointer exception */
		goto fail;

	if ((ehdr = ber_add_integer(msg->sm_resp, msg->sm_version)) == NULL)
		goto fail;
	if ((ehdr = ber_add_string(ehdr, msg->sm_community)) == NULL)
		goto fail;

	pdu = epdu = ber_add_sequence(NULL);

	if (pdu == NULL) /* RQ 1944871 -- NULL pointer exception */
		goto fail;

	if (!ber_printf_elements(epdu, "tiii{e}.", BER_CLASS_CONTEXT,
	    msg->sm_context, msg->sm_request,
	    msg->sm_error, msg->sm_errorindex,
	    msg->sm_varbindresp)) {
		ber_free_elements(pdu);
		return -1;
	}

	ber_link_elements(ehdr, pdu);

	return 0;

fail:
	if (msg->sm_varbindresp != NULL) ber_free_elements(msg->sm_varbindresp);
	msg->sm_varbindresp = NULL;
	return -1;
}

                            /*                   see TFS Task 1194574                                               */

int obsd_snmpe_socket_open(void)
{
	/* gh2289n: we use the local env here (which could be a process local one, in case of STACK_TEST_VARIANT)   */
	/*          This is the suited environment for snmpe functions. Using the local env here requires that      */
	/*          snmpe(), which initialized this environment is called before obsd_snmpe_socket_open()           */

	if (env != NULL) /* local env need to be initialized */
	{
		if (env->sc_sock < 0)
		{
			struct event_callback event_callback_val;
			int opt_val = 1;

			if ((env->sc_sock = snmpe_bind(&env->sc_address)) == -1)
			{
				return -1;              /* return instead calling exit() */
			}

			event_callback_val.l_usr_callback = obsd_snmpd_handle_events;
			event_callback_val.l_ctx_ptr = NULL;
			if (setsockopt(env->sc_sock, OBSD_PNIO_SOL_SOCKET, OBSD_PNIO_SO_EVENT_CALLBACK, &event_callback_val, sizeof(event_callback_val)) != 0)
			{
				return -1;
			}

			if (setsockopt(env->sc_sock, IPPROTO_IP, IP_RECVDSTADDR, &opt_val, sizeof(opt_val)) != 0)
			{
				return -1;
			}

			if (setsockopt(env->sc_sock, IPPROTO_IP, IP_RECVIF, &opt_val, sizeof(opt_val)) != 0)
			{
				return -1;
			}

			EvtHdlChain.pChainedFunc    = snmpe_recvmsg;
			EvtHdlChain.pChainedFuncArg = env;
			event_set(&env->sc_ev, env->sc_sock, EV_READ|EV_PERSIST, MIBAL_snmpd_receive_msg_event, &EvtHdlChain);
		}
		
		event_add(&env->sc_ev, NULL);
		return 0;
	}
	return -1; /* no valid env, snmpe() was not call and so snmpe is not ready to go */
}

int obsd_snmpe_socket_close(void)
{
	/* gh2289n: we use the local env here (which could be a process local one, in case of STACK_TEST_VARIANT)   */
	/*          This is the suited environment for snmpe functions. Using the local env here requires that      */
	/*          snmpe(), which initialized this environment, is called before obsd_snmpe_socket_close()         */
	/*          and, in turn, obsd_snmpe_socket_close() is called before snmpe_shutdown()                       */

	if (env != NULL)  /* local env need to be initialized */
	{
		if (env->sc_sock >= 0)
		{
			event_del(&env->sc_ev);
			close(env->sc_sock); /* seem's that nobody does this so far */
			env->sc_sock = -1;
		}

		return 0;
	}
	return -1; /* no valid env, snmpe() was not call and so snmpe is not ready to go */
}

int obsd_snmpd_check_varbinds(struct ber_element* varbinds)
{
	struct ber_element *var_element = varbinds;

	if (var_element == NULL) return -1;

	while (var_element != NULL)
	{
		if (var_element->be_encoding != BER_TYPE_SEQUENCE || var_element->be_sub == NULL) return -1;
		if (var_element->be_sub->be_encoding != BER_TYPE_OBJECT || var_element->be_sub->be_next == NULL) return -1;
		var_element = var_element->be_next;
	}

	return 0;

}



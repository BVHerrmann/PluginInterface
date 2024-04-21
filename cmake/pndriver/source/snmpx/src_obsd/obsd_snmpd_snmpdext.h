/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_snmpd_snmpdext.h
 *
 * @brief       external interface of the snmpd (SNMP daemon)
 * @details     Interface of the MIBAL module
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.05
 * @date        created: 30.06.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef   __OBSD_SNMPD_SNMPDEXT_H__
#define   __OBSD_SNMPD_SNMPDEXT_H__

/* extern void obsd_snmpd_startup(  void);   there's no such function, you have to use            */
/* obsd_execv_ex("snmpd", "snmpd") or obsd_call_cmd_func(snmpd_main, 0, NULL) instead, otherwise  */
/* we miss the correct exit()/abort() handling                                                    */
/* fatal, fatalx and so on                                                                        */

extern int snmpd_main(int argc, char *argv[]);    /* must be called with an correct exit(handling)*/
                                                  /* implemented, otherwise snmpd's exit() calls  */
                                                  /* (in fatal(), fatalx() and so) will fail      */
                                                  /* (for exit handling use obsd_execv_ex or      */
                                                  /* obsd_execv or obsd_call_cmd_func             */
extern void obsd_snmpd_shutdown(void);            /* shut's down the whole SNMPD                  */
extern void obsd_snmpd_handle_events(int flag, void *ctx_ptr);  /* intended for nonblocking mode like uses in   */
                                                  /* standard PN variant                          */

extern int obsd_snmpd_snmp_off(void); /* close snmp socket */
extern int obsd_snmpd_snmp_on(void);  /* open snmp socket */

typedef struct
{
    void (*pChainedFunc)(int fd, short sig, void *arg);
    void *pChainedFuncArg;
} recv_msg_evt_chain_arg_t;

#endif /* __OBSD_SNMPD_SNMPDEXT_H__ */


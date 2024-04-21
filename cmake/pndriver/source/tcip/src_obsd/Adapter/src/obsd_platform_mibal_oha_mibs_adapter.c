/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal_oha_mibs_adapter.c
 *
 * @brief       adapter to LLDP and MRP MIBs in PN's OHA component
 * @details     adapter for access to the LLDP and MRP MIB implementation in OHA Implements the "Level 2 Interface
 *              (use of the existing "SC IC OHA interface"
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.03
 * @date        created: 28.11.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_socket.h>             /* needed for obsd_kernel_if.h */
#include <net/obsd_kernel_if.h>                 /* needed for snmpd.h          */

#include <obsd_userland_event.h>                /* needed for snmpd.h */
#include "obsd_snmpd_snmpd.h"                   /* struct oid */

#include <sys/obsd_kernel_types.h>              /* needed for obsd_snmpd_ber.h */
#include "obsd_snmpd_ber.h"                     /* obsd_snmpd_ber, ...               */

#include "obsd_snmpd_pn_mibs.h"                 /* pn_mibs_async_var_request_done_cb */

#include "obsd_platform_mibal.h"                /* MIBAL_get_lldp_var, ... */
#include "obsd_platform_mibal_internal.h"		/* MIBAL internal interface                       */

#include "obsd_platform_mibal_oha_mibs_helper.h"/* helper functions                               */

#include "snmpx_oha_interface.h"                /* OHA's interface for LLDP and MRP MIB requests  */


/* =================== MRP and LLDP MIB functions  =================== */
/* We get the LLDP and MRP Mib variables from PN's OHA.                */
/* Due to the nature of PN we have to call OHA functions asynchronous  */

/* ------------------- LLDP  ---------------------------------------*/
int MIBAL_get_lldp_var(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
    fn_snmpx_oha_mib_get_var_t fn = NULL;

    switch((*elm)->oha_par.snmp_operation)
    {
        case SNMP_C_GETREQ:     fn = snmpx_oha_get_lldp_var;
                                break;
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
        case SNMP_C_GETBULKREQ: /* no break */
#endif
		case SNMP_C_GETNEXTREQ: fn = snmpx_oha_get_next_lldp_var;
                                break;
        default:                /* let fn set to NULL */
                                break;
    }
    return MIBAL_oha_getvar_request(fn, oid, o, elm);
}

int MIBAL_set_lldp_var(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
    return MIBAL_oha_setvar_request(snmpx_oha_set_lldp_var, oid, o, elm);
}


/* ------------------- MRP  ----------------------------------------*/
int MIBAL_get_mrp_var(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{    
    fn_snmpx_oha_mib_get_var_t fn = NULL;
    
    switch((*elm)->oha_par.snmp_operation)
    {
        case SNMP_C_GETREQ:     fn = snmpx_oha_get_mrp_var;
                                break;
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
		case SNMP_C_GETBULKREQ: /* no break */
#endif
        case SNMP_C_GETNEXTREQ: fn = snmpx_oha_get_next_mrp_var;
                                break;
        default:                /* let fn set to NULL */
                                break;
    }
    return MIBAL_oha_getvar_request(fn, oid, o, elm);
}

int MIBAL_set_mrp_var(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
    return MIBAL_oha_setvar_request(snmpx_oha_set_mrp_var, oid, o, elm);
}


/* --------------------------------------------------------------------*/

/* ------------------- callbacks --------------------------------------*/

void snmpx_oha_get_lldp_var_done(       unsigned int    *oid,           /* in       */
                                        int             oid_length,     /* in       */
                                        int             snmp_response,  /* in       */
                                        void            *pvar_buffer,   /* in       */
                                        unsigned char   var_type,       /* in       */
                                        int             var_length,     /* in       */
                                        void            *pcontext_ptr   /* in       */
                                )
{
    MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}

void snmpx_oha_get_next_lldp_var_done(  unsigned int    *oid,
                                        int             oid_length,
                                        int             snmp_response,
                                        void            *pvar_buffer,
                                        unsigned char   var_type,
                                        int             var_length,
                                        void            *pcontext_ptr
                                      )
{
    MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}


void snmpx_oha_set_lldp_var_done(   unsigned int    *oid,           /* in     */
                                    int             oid_length,     /* in     */
                                    int             snmp_response,  /* in     */
                                    void            *pvar_buffer,   /* in     */
                                    unsigned char   var_type,       /* in     */
                                    int             var_length,     /* in     */
                                    void            *pcontext_ptr   /* in     */
                                )
{
    MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}

void snmpx_oha_get_mrp_var_done(   unsigned int    *oid,           /* in     */
                                    int             oid_length,     /* in     */
                                    int             snmp_response,  /* in     */
                                    void            *pvar_buffer,   /* in     */
                                    unsigned char   var_type,       /* in     */
                                    int             var_length,     /* in     */
                                    void            *pcontext_ptr   /* in     */
                                 )
{
     MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}

void snmpx_oha_get_next_mrp_var_done(   unsigned int    *oid,
                                        int             oid_length,
                                        int             snmp_response,
                                        void            *pvar_buffer,
                                        unsigned char   var_type,
                                        int             var_length,
                                        void            *pcontext_ptr
                                    )
{
     MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}

void snmpx_oha_set_mrp_var_done(    unsigned int    *oid,           /* in     */
                                    int             oid_length,     /* in     */
                                    int             snmp_response,  /* in     */
                                    void            *pvar_buffer,   /* in     */
                                    unsigned char   var_type,       /* in     */
                                    int             var_length,     /* in     */
                                    void            *pcontext_ptr   /* in     */
                               )
{
    MIBAL_oha_async_var_request_done_cb(oid, oid_length, snmp_response, pvar_buffer, var_type, var_length, pcontext_ptr);
}

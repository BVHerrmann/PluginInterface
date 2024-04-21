/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_snmpd_pn_mibs.c
 *
 * @brief       PN specific MIB enhancements for OpenBSD's SNMPD implementation
 * @details     Implementation of functions and data for the additional PN MIB's
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.06
 * @date        created: 24.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */


#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_socket.h>             /* needed for obsd_kernel_if.h                    */
#include <net/obsd_kernel_if.h>                 /* needed for snmpd.h                             */

#include <obsd_userland_event.h>                /* needed for snmpd.h                             */
#include "obsd_snmpd_snmpd.h"

#include "obsd_snmpd_mib.h"                     /* assume obsd_snmpd_pn_mibs.h is included there  */

#include "obsd_platform_mibal.h"                /* MIBAL interface                                */
#include "obsd_platform_mibal_internal.h"		/* MIBAL internal interface                       */

/* function prototype of SNMPD's normal o_table function pointer within MIBs */
typedef struct ber_oid *(*fn_pn_mibs_gettable_t)(struct oid *, struct ber_oid *, struct ber_oid *);


/* LLDP MIB tree */
/* OpenBSD's SNMPD uses an internal implementation for snmp getnext. This implementation calls    */
/* the o_get function pointer to get the single values for the OID's. The PN MIBs (LLDP and MRP)  */
/* are managed by PN's OHA which has the complete getnext functionality inside. So we need an     */
/* additional function pointer for the getnext function inside the MIB table.                     */
/* To avoid defining a new additional function pointer within all MIB's we reuse the existing     */
/* o_table function pointer and manage it's argument list by ourself using suitable casts. We     */
/* don't need an o_table function pointer for the OHA MIB's because we don't know about the       */
/* internal structure of the OID's within the LLDP and MRP MIB. So we even don't know which OID   */
/* is for a table and which one is for a scalar.                                                  */
static struct oid mib_pn_oha_managed[] = 
{
    { MIB(lldp),			OID_MIB | OID_EXTERNAL_OHA_MIB | OID_RW, MIBAL_get_lldp_var, MIBAL_set_lldp_var},
    { MIB(mrp),				OID_MIB | OID_EXTERNAL_OHA_MIB | OID_RW, MIBAL_get_mrp_var,  MIBAL_set_mrp_var },
    { MIBEND }
};


void obsd_snmpd_pn_mibs_init(void)
{
    /* LLDP and MRP MIB */
    smi_mibtree(mib_pn_oha_managed);    /* currently not a mib tree, use a special find mechanism */
                                        /* because the sub OIDs are managed by PN's OHA AND these */
                                        /* are dynamically generated depending from some PN config*/
}







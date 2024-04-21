/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_snmpd_pn_mibs.h
 *
 * @brief       PN specific MIB enhancements for OpenBSD's SNMPD implementation
 * @details     definitions and declarations for PN MIB's like LLDP- and MRP-MIBs
 *
 * @author      Hendrik Gerlach PD TI ATS TM4 2
 *
 * @version     V0.03
 * @date        created: 24.10.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef   __OBSD_SNMPD_PN_MIBS_H__
#define   __OBSD_SNMPD_PN_MIBS_H__

#define MIB_standard            MIB_iso, 0              /* 1.0, see http://oid-info.com/get/1.0   */
#define MIB_standard_elements   2                       /* coount of it's OID elements            */

/* LLDP */
#define MIB_iso8802             MIB_standard, 8802      /* 1.0.8802,                              */
                                                        /* ISO/IEC 8802 "Information technology - */
                                                        /* Telecommunications and information     */
                                                        /* exchange between systems -- Local and  */
                                                        /* metropolitan area networks --          */
                                                        /* Specific requirements"                 */
#define MIB_ieee802dot1         MIB_iso8802, 1          /* 1.0.8802.1                             */
                                                        /* IEEE 802.1 Std                         */
#define MIB_ieee802dot1mibs     MIB_ieee802dot1, 1      /* 1.0.8802.1.1                           */
                                                        /* Management Information Bases (MIBs)    */
                                                        /* defined in IEEE 802.1 Std              */
#define MIB_lldp                MIB_ieee802dot1mibs, 2  /* 1.0.8802.1.1.2                         */
                                                        /* lldpMIB                                */
#define OIDIDX_lldp             6
#define MIB_lldpObjects         MIB_lldp, 1             /* 1.0.8802.1.1.2.1                       */


/* MRP */
#define MIB_iec62439            MIB_standard, 62439      /* 1.0.62439,                            */
                                                         /* MIB module named IEC-62439-3-MIB which*/
                                                         /* defines the Network Management        */
                                                         /* interfaces for the redundancy         */
                                                         /* protocols defined by IEC 62439 series */
#define MIB_mrp                 MIB_iec62439, 1          /* 1.0.62439.1 (IEC CDV 62439-2 IEC 2014)*/
#define OIDIDX_mrp              4
#define MIB_mrpObjects          MIB_mrp, 1               /* 1.0.62439.1.1                         */

extern void obsd_snmpd_pn_mibs_init(void);

extern int pn_mibs_async_var_request_done_cb(   unsigned int        *poid,
                                                int                 oid_length,
                                                int                 snmp_response,
                                                void                *pvar_buffer,
                                                unsigned char       var_type,
                                                int                 var_length,
                                                void                *pcontext_ptr
                                      );

#endif /* __OBSD_SNMPD_PN_MIBS_H__ */

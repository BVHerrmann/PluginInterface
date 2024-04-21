/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        snmpx_oha_interface.h
 *
 * @brief       SNMP Interface of the OHA component
 * @details     this file is a temporary solution until we have the appropriate OHA header file
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.04
 * @date        created: 07.11.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#ifndef   __SNMPX_OHA_H__
#define   __SNMPX_OHA_H__


/* ================================== OHA's LLDP and MRP object types  ================================= */
/* see eMail Ernst from 19.11.2014 */

/* ASN.1 primitive types used by OHA */
#define OHA_SNMP_INTEGER        (0x02)          /* for LLDP and MRP objects */
#define OHA_SNMP_BITS           (0x03)          /* for LLDP and MRP objects */
#define OHA_SNMP_STRING         (0x04)          /* for LLDP and MRP objects */
#define OHA_SNMP_OBJID          (0x06)          /* for LLDP objects         */
#define OHA_SNMP_NULLOBJ        (0x05)          /* for LLDP and MRP objects */

/* LLDP */
/* defined types (from the SMI, RFC 1065) */
#define OHA_SNMP_COUNTER        (0x40 | 1)
#define OHA_SNMP_GAUGE          (0x40 | 2)
#define OHA_SNMP_UNSIGNED32     (0x40 | 2)
#define OHA_SNMP_TIMETICKS      (0x40 | 3)


/* MRP */
/* defined types (from the SMI, RFC 1065) */
#define OHA_SNMP_COUNTER        (0x40 | 1)
#define OHA_SNMP_UNSIGNED32     (0x40 | 2)
#define OHA_SNMP_TIMETICKS      (0x40 | 3)


/* snmp error status */ /* see OHA_USR.H */
#define  OHA_SNMP_ERR_NOERROR     (0x0)
#define  OHA_SNMP_ERR_TOOBIG      (0x1)
#define  OHA_SNMP_ERR_NOSUCHNAME  (0x2)
#define  OHA_SNMP_ERR_NOSUCH      (0x2)
#define  OHA_SNMP_ERR_BADVALUE    (0x3)
#define  OHA_SNMP_ERR_READONLY    (0x4)
#define  OHA_SNMP_ERR_GENERR      (0x5)

/* for SnmpVersion OHA_SNMP_VERSION_2 and OHA_SNMP_VERSION_3 */
#define  OHA_SNMP_ASN_NSO     (0x80)  /* RFC1905:noSuchObject   */
#define  OHA_SNMP_ASN_NSI     (0x81)  /* RFC1905:noSuchInstance */
#define  OHA_SNMP_ASN_EOM     (0x82)  /* RFC1905:endOfMib       */

#define  OHA_MAX_OID_LEN       64    /* max length in OHA_OID's of an encoded oid */


/* ================================== OHA's LLDP and MRP C types  =============================== */
typedef unsigned int OHA_oid_element_type_t;    /* C-type of an element in an OID value           */
                                                /* (e.g. 1.3.6.1.4.1.24686) for OHA. C-Type is    */
                                                /* important for the memory sizes and so on. OHA  */
                                                /* uses unsigned int type here.                   */
                                                /* (like discussed with Ulli 27.11.2014)          */

/* ================================== OHA's LLDP MIB functions  ================================= */

/* ---------------------------------- get functions --------------------------------------------- */

extern int snmpx_oha_get_lldp_var(          unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            void            *pvar_buffer,       /* out    */
                                            int             var_buffer_size,    /* in     */
                                            void            *context_ptr        /* in     */
                                 );

extern int snmpx_oha_get_mrp_var(           unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            void            *pvar_buffer,       /* out    */
                                            int             var_buffer_size,    /* in     */
                                            void            *context_ptr        /* in     */
                                 );


extern void snmpx_oha_get_lldp_var_done(    unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            int             snmp_response,      /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                        );

extern void snmpx_oha_get_mrp_var_done(     unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            int             snmp_response,      /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                      );


/* ---------------------------------- set functions --------------------------------------------- */

extern int snmpx_oha_set_lldp_var(          unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                 );

extern int snmpx_oha_set_mrp_var(           unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                  );

extern void snmpx_oha_set_lldp_var_done(    unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            int             snmp_response,      /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                      );

extern void snmpx_oha_set_mrp_var_done(     unsigned int    *oid,               /* in     */
                                            int             oid_length,         /* in     */
                                            int             snmp_response,      /* in     */
                                            void            *pvar_buffer,       /* in     */
                                            unsigned char   var_type,           /* in     */
                                            int             var_length,         /* in     */
                                            void            *context_ptr        /* in     */
                                       );


/* ---------------------------------- getnext functions ----------------------------------------- */

extern int snmpx_oha_get_next_lldp_var(         unsigned int    *oid,               /* in     */
                                                int             oid_length,         /* in     */
                                                void            *pvar_buffer,       /* out    */
                                                int             var_buffer_size,    /* in     */
                                                void            *context_ptr        /* in     */
                                      );

extern void snmpx_oha_get_next_lldp_var_done(   unsigned int    *oid,
                                                int             oid_length,
                                                int             snmp_response,
                                                void            *var_buffer,
                                                unsigned char   var_type,
                                                int             var_length,
                                                void            *context_ptr
                                            );

extern int snmpx_oha_get_next_mrp_var(          unsigned int    *oid,               /* in     */
                                                int             oid_length,         /* in     */
                                                void            *pvar_buffer,       /* out    */
                                                int             var_buffer_size,    /* in     */
                                                void            *context_ptr        /* in     */
                                       );
extern void snmpx_oha_get_next_mrp_var_done(    unsigned int    *oid,
                                                int             oid_length,
                                                int             snmp_response,
                                                void            *var_buffer,
                                                unsigned char   var_type,
                                                int             var_length,
                                                void            *context_ptr
                                           );

#endif /* __SNMPX_OHA_H__ */

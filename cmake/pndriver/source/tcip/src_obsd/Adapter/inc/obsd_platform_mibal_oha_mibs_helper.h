/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal_oha_mibs_helper.h
 *
 * @brief       internal Interface of the MIBAL module 
 * @details     Interface of obsd_platform_mibal_oha_mibs_helper.c
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.01
 * @date        created: 28.11.2014
 */
/* -------------------------------------------------------------------------------------------------- */
#ifndef    __OBSD_PLATFORM_MIBAL_OHA_MIBS_HELPER_H__
#define    __OBSD_PLATFORM_MIBAL_OHA_MIBS_HELPER_H__

typedef int (*fn_snmpx_oha_mib_get_var_t)(          unsigned int                *oid,                   /* in     */
                                                    int                         oid_length,             /* in     */
                                                    void                        *pvar_buffer,           /* out    */
                                                    int                         var_buffer_size,        /* in     */
                                                    void                        *context_ptr            /* in     */
                                          );

typedef int (*fn_snmpx_oha_mib_set_var_t)(          unsigned int                *oid,                   /* in     */
                                                    int                         oid_length,             /* in     */
                                                    void                        *pvar_buffer,           /* in     */
                                                    unsigned char               var_type,               /* in     */
                                                    int                         var_length,             /* in     */
                                                    void                        *context_ptr            /* in     */
                                          );

/* --------------------- interface functions -------------------------------------------------------- */

extern int MIBAL_oha_getvar_request(                fn_snmpx_oha_mib_get_var_t  fn_oha_get_request,     /* in       */
                                                    struct oid                  *oid,                   /* in       */
                                                    struct ber_oid              *o,                     /* in       */
                                                    struct ber_element          **pelm                  /* in       */ /* out in case of synchronus operation */
                                   );

extern int MIBAL_oha_setvar_request(                fn_snmpx_oha_mib_set_var_t  fn_oha_set_request,     /* in       */
                                                    struct oid                  *oid,                   /* in       */
                                                    struct ber_oid              *o,                     /* in       */
                                                    struct ber_element          **pelm                  /* in       */ 
                                   );

extern int MIBAL_oha_async_var_request_done_cb(     unsigned int                *oid,                   /* in       */
                                                    int                         oid_length,             /* in       */
                                                    int                         snmp_response,          /* in       */
                                                    void                        *pvar_buffer,           /* in       */
                                                    unsigned char               var_type,               /* in       */
                                                    int                         var_length,             /* in       */
                                                    void                        *pcontext_ptr           /* in       */
                                                );

#endif  /* __OBSD_PLATFORM_MIBAL_OHA_MIBS_HELPER_H__ */


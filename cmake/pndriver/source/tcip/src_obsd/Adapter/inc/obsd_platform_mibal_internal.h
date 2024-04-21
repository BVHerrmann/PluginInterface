/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal_internal.h
 *
 * @brief       MIB A(daption) L(ayer) internal include file
 * @details     internal Interface of the MIBAL module
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.01
 * @date        created: 12.07.2015
 */
/* -------------------------------------------------------------------------------------------------- */
#ifndef    __OBSD_PLATFORM_MIBAL_INTERNAL_H__
#define    __OBSD_PLATFORM_MIBAL_INTERNAL_H__


typedef int (*fn_pn_mibs_getnext_t)(struct oid *oid, struct ber_oid *o, struct ber_element **elm);

/* -------------------  LLDP ---------------------------------------*/
extern int  MIBAL_get_lldp_var(     struct oid *oid, struct ber_oid *o, struct ber_element **elm);
extern int  MIBAL_set_lldp_var(     struct oid *oid, struct ber_oid *o, struct ber_element **elm);
extern int  MIBAL_getnext_lldp_var( struct oid *oid, struct ber_oid *o, struct ber_element **elm);

/* -------------------  MRP ---------------------------------------*/
extern int  MIBAL_get_mrp_var(      struct oid *oid, struct ber_oid *o, struct ber_element **elm);
extern int  MIBAL_set_mrp_var(      struct oid *oid, struct ber_oid *o, struct ber_element **elm);
extern int  MIBAL_getnext_mrp_var(  struct oid *oid, struct ber_oid *o, struct ber_element **elm);

#endif  /* __OBSD_PLATFORM_MIBAL_INTERNAL_H__ */


/*	$OpenBSD: ber.h,v 1.9 2013/10/01 12:41:47 reyk Exp $ */

/*
 * Copyright (c) 2007, 2012 Reyk Floeter <reyk@openbsd.org>
 * Copyright (c) 2006, 2007 Claudio Jeker <claudio@openbsd.org>
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

#ifndef _BER_H
#define _BER_H

struct ber_element;                         /* forward declaration to avoid compiler warnings     */
struct ber_oid;                             /* forward declaration to avoid compiler warnings     */
struct oid;                                 /* forward declaration to avoid compiler warnings     */

#define SUPPORT_OHA_EXTERNAL_MIBS        1  /* If set we support OHA MIBs (like LLDP & MRP MIBs)  */
                                            /* that are completely external. Other than for the   */
                                            /* SNMPD managed MIBs we don't know the whole MIB     */
                                            /* tree here, but only the highest level OID for such */
                                            /* MIBs. All of such OIB's are managed completely     */
                                            /* externally (in OHA in this case)                   */

#define SUPPORT_OHA_ASYNC_MIBVAR_HANDING 1  /* gh2289n: getting LLDP and MRP MIB values runs      */
                                            /* asynchronous in PN's OHA                           */

                                            /* handling, needed for PN LLDP & MRP (managed by OHA)*/
typedef void (*be_async_cb_func_t)( void                    *be_async_cbarg,
                                    struct ber_element      *elm,
                                    struct ber_oid          *elm_oid,
                                    int                     snmp_op,
                                    int                     snmp_response,
                                    int                     varbind_choice, /* should better of   */
                                                            /* snmp_varbind_choice_t type, but due*/
                                                            /* to include depndencies hard to do  */
                                    struct oid              *getnext_last_oid
                                  );

struct ber_element {
	struct ber_element	*be_next;
	unsigned long		 be_type;
	unsigned long		 be_encoding;
	size_t			 be_len;
	off_t			 be_offs;
	int			 be_free;
	u_int8_t		 be_class;
	void			(*be_cb)(void *, size_t);
	void			*be_cbarg;
	union {
		struct ber_element	*bv_sub;
		void			*bv_val;
		long long		 bv_numeric;
	} be_union;
#define be_sub		be_union.bv_sub
#define be_val		be_union.bv_val
#define be_numeric	be_union.bv_numeric
#if       SUPPORT_OHA_EXTERNAL_MIBS
	struct ber_element_oha_adapter_params_s
	{
		int 			snmp_operation;     /* gh2289n: SNMP_C_GETNEXTREQ, SNMP_C_GETREQ, .     ..*/
						                    /*          handling, needed for PN LLDP and MRP      */
						                    /* (managed by OHA)                                   */
		be_async_cb_func_t be_async_cb;
		void			*be_async_cbarg;
		struct oid		*be_getnext_last_oid;
		int 			wait_for_async_completion;
	} oha_par;
#endif /* SUPPORT_OHA_EXTERNAL_MIBS */
};

struct ber {
	int	 fd;
	off_t	 br_offs;
	u_char	*br_wbuf;
	u_char	*br_wptr;
	u_char	*br_wend;
	u_char	*br_rbuf;
	u_char	*br_rptr;
	u_char	*br_rend;

	unsigned long	(*br_application)(struct ber_element *);
	int		seq_recursion_level;   /* recursion level for sequences and type sets */
};

/* well-known ber_element types */
#define BER_TYPE_DEFAULT	((unsigned long)-1)
#define BER_TYPE_EOC		0
#define BER_TYPE_BOOLEAN	1
#define BER_TYPE_INTEGER	2
#define BER_TYPE_BITSTRING	3
#define BER_TYPE_OCTETSTRING	4
#define BER_TYPE_NULL		5
#define BER_TYPE_OBJECT		6
#define BER_TYPE_ENUMERATED	10
#define BER_TYPE_SEQUENCE	16
#define BER_TYPE_SET		17

/* ber classes */
#define BER_CLASS_UNIVERSAL	0x0
#define BER_CLASS_UNIV		BER_CLASS_UNIVERSAL
#define BER_CLASS_APPLICATION	0x1
#define BER_CLASS_APP		BER_CLASS_APPLICATION
#define BER_CLASS_CONTEXT	0x2
#define BER_CLASS_PRIVATE	0x3
#define BER_CLASS_MASK		0x3

/* common definitions */
#define BER_MIN_OID_LEN		2	/* OBJECT */
#define BER_MAX_OID_LEN		32	/* OBJECT */

struct ber_oid {
	u_int32_t	bo_id[BER_MAX_OID_LEN + 1];
	size_t		bo_n;
};

__BEGIN_DECLS
struct ber_element	*ber_get_element(unsigned long);
void			 ber_set_header(struct ber_element *, int,
			    unsigned long);
void			 ber_link_elements(struct ber_element *,
			    struct ber_element *);
struct ber_element	*ber_unlink_elements(struct ber_element *);
void			 ber_replace_elements(struct ber_element *,
			    struct ber_element *);
struct ber_element	*ber_add_sequence(struct ber_element *);
struct ber_element	*ber_add_set(struct ber_element *);
struct ber_element	*ber_add_integer(struct ber_element *, long long);
int			 ber_get_integer(struct ber_element *, long long *);
struct ber_element	*ber_add_enumerated(struct ber_element *, long long);
int			 ber_get_enumerated(struct ber_element *, long long *);
struct ber_element	*ber_add_boolean(struct ber_element *, int);
int			 ber_get_boolean(struct ber_element *, int *);
struct ber_element	*ber_add_string(struct ber_element *, const char *);
struct ber_element	*ber_add_nstring(struct ber_element *, const char *,
			    size_t);
int			 ber_get_string(struct ber_element *, char **);
int			 ber_get_nstring(struct ber_element *, void **,
			    size_t *);
struct ber_element	*ber_add_bitstring(struct ber_element *, const void *,
			    size_t);
int			 ber_get_bitstring(struct ber_element *, void **,
			    size_t *);
struct ber_element	*ber_add_null(struct ber_element *);
int			 ber_get_null(struct ber_element *);
struct ber_element	*ber_add_eoc(struct ber_element *);
int			 ber_get_eoc(struct ber_element *);
struct ber_element	*ber_add_oid(struct ber_element *, struct ber_oid *);
struct ber_element	*ber_add_noid(struct ber_element *, struct ber_oid *, int);
struct ber_element	*ber_add_oidstring(struct ber_element *, const char *);
int			 ber_get_oid(struct ber_element *, struct ber_oid *);
size_t			 ber_oid2ber(struct ber_oid *, u_int8_t *, size_t);
int			 ber_string2oid(const char *, struct ber_oid *);
struct ber_element	*ber_printf_elements(struct ber_element *, char *, ...);
int			 ber_scanf_elements(struct ber_element *, char *, ...);
ssize_t			 ber_get_writebuf(struct ber *, void **);
int			 ber_write_elements(struct ber *, struct ber_element *);
void			 ber_set_readbuf(struct ber *, void *, size_t);
struct ber_element	*ber_read_elements(struct ber *, struct ber_element *);
off_t			 ber_getpos(struct ber_element *);
void			 ber_free_elements(struct ber_element *);
size_t			 ber_calc_len(struct ber_element *);
void			 ber_set_application(struct ber *,
			    unsigned long (*)(struct ber_element *));
void			 ber_set_writecallback(struct ber_element *,
			    void (*)(void *, size_t), void *);
void			 ber_free(struct ber *);
int			 ber_oid_cmp(struct ber_oid *, struct ber_oid *);

__END_DECLS

#endif /* _BER_H */



/*
 * Copyright (c) 2007, 2008 Reyk Floeter <reyk@openbsd.org>
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

#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_stat.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_un.h>
#include <sys/obsd_kernel_tree.h>
#include <sys/obsd_kernel_sysctl.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_dl.h>
#include <net/obsd_kernel_if_arp.h>
#include <net/obsd_kernel_if_media.h>
#include <net/obsd_kernel_route.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_if_ether.h>
#include <arpa/obsd_userland_inet.h>

#include <obsd_userland_stdlib.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_errno.h>
#include <obsd_userland_event.h>
#include <obsd_kernel_fcntl.h>
#include <obsd_userland_string.h>
#include <obsd_userland_unistd.h>
#define DEBUG

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_mib.h"
#define MINIMUM(a, b)	(((a) < (b)) ? (a) : (b))

extern struct snmpd *env;

RB_HEAD(oidtree, oid);
RB_PROTOTYPE(oidtree, oid, o_element, smi_oid_cmp);
struct oidtree smi_oidtree;

RB_HEAD(keytree, oid);
RB_PROTOTYPE(keytree, oid, o_keyword, smi_key_cmp);
struct keytree smi_keytree;

u_long
smi_getticks(void)
{
	struct timeval	 now, run;
	u_long		 ticks;

	gettimeofday(&now, NULL);
	if (timercmp(&now, &env->sc_starttime, <=))
		return (0);
	timersub(&now, &env->sc_starttime, &run);
	ticks = run.tv_sec * 100;
	if (run.tv_usec)
		ticks += run.tv_usec / 10000;

	return (ticks);
}

void
smi_oidlen(struct ber_oid *o)
{
	int	 i; /* sado -- Greenhills compiler warning 186 - pointless comparison of unsigned integer with zero */

    for (i = BER_MAX_OID_LEN-1; i >= 0 && o->bo_id[i] == 0; i--) /* sado -- v5.9 implementation does not work */
        ;
    o->bo_n = i+1;
}


char *
smi_oid2string(struct ber_oid *o, char *buf, size_t len, size_t skip)
{
	char		 str[256];
	struct oid	*value, key;
	size_t		 i, lookup = 1;

	bzero(buf, len);
	bzero(&key, sizeof(key));
	bcopy(o, &key.o_id, sizeof(struct ber_oid));
	key.o_flags |= OID_KEY;		/* do not match wildcards */

	if (env->sc_flags & SNMPD_F_NONAMES)
		lookup = 0;

	for (i = 0; i < o->bo_n; i++) {
		key.o_oidlen = i + 1;
		if (lookup && skip > i)
			continue;
		if (lookup &&
		    (value = RB_FIND(oidtree, &smi_oidtree, &key)) != NULL)
			snprintf(str, sizeof(str), "%s", value->o_name);
		else
			snprintf(str, sizeof(str), "%d", key.o_oid[i]);
		strlcat(buf, str, len);
		if (i < (o->bo_n - 1))
			strlcat(buf, ".", len);
	}

	return (buf);
}


void
smi_mibtree(struct oid *oids)
{
	struct oid	*oid, *decl;
	size_t		 i;

	for (i = 0; oids[i].o_oid[0] != 0; i++) {
		oid = &oids[i];
		smi_oidlen(&oid->o_id);
		if (oid->o_name != NULL) {
			if ((oid->o_flags & OID_TABLE) && oid->o_get == NULL)
				fatalx("smi_mibtree: invalid MIB table");
			RB_INSERT(oidtree, &smi_oidtree, oid);
			RB_INSERT(keytree, &smi_keytree, oid);
			continue;
		}
		decl = RB_FIND(oidtree, &smi_oidtree, oid);
		if (decl == NULL)
			fatalx("smi_mibtree: undeclared MIB");
		decl->o_flags = oid->o_flags;
		decl->o_get = oid->o_get;
		decl->o_set = oid->o_set;
		decl->o_table = oid->o_table;
		decl->o_val = oid->o_val;
		decl->o_data = oid->o_data;
	}
}

int
smi_init(void)
{
	/* Initialize the Structure of Managed Information (SMI) */
	RB_INIT(&smi_oidtree);
	mib_init();
	return (0);
}

struct oid *
smi_find(struct oid *oid)
{
	return (RB_FIND(oidtree, &smi_oidtree, oid));
}

struct oid *
smi_findkey(char *name)
{
	struct oid	oid;
	if (name == NULL)
		return (NULL);
	oid.o_name = name;
	return (RB_FIND(keytree, &smi_keytree, &oid));
}

struct oid *
smi_next(struct oid *oid)
{
	return (RB_NEXT(oidtree, &smi_oidtree, oid));
}

struct oid *
smi_foreach(struct oid *oid, u_int flags)
{
	/*
	 * Traverse the tree of MIBs with the option to check
	 * for specific OID flags.
	 */
	if (oid == NULL) {
		oid = RB_MIN(oidtree, &smi_oidtree);
		if (oid == NULL)
			return (NULL);
		if (flags == 0 || (oid->o_flags & flags))
			return (oid);
	}
	for (;;) {
		oid = RB_NEXT(oidtree, &smi_oidtree, oid);
		if (oid == NULL)
			break;
		if (flags == 0 || (oid->o_flags & flags))
			return (oid);
	}

	return (oid);
}



unsigned long
smi_application(struct ber_element *elm)
{
	if (elm->be_class != BER_CLASS_APPLICATION)
		return (BER_TYPE_OCTETSTRING);

	switch (elm->be_type) {
	case SNMP_T_IPADDR:
		return (BER_TYPE_OCTETSTRING);
	case SNMP_T_COUNTER32:
	case SNMP_T_GAUGE32:
	case SNMP_T_TIMETICKS:
	case SNMP_T_OPAQUE:
	case SNMP_T_COUNTER64:
		return (BER_TYPE_INTEGER);
	default:
		break;
	}
	return (BER_TYPE_OCTETSTRING);
}

int
smi_oid_cmp(struct oid *a, struct oid *b)
{
	size_t	 i;

	for (i = 0; i < MINIMUM(a->o_oidlen, b->o_oidlen); i++)
		if (a->o_oid[i] != b->o_oid[i])
			return (a->o_oid[i] - b->o_oid[i]);

	/*
	 * Return success if the matched object is a table
	 * or a MIB registered by a subagent
	 * (it will match any sub-elements)
	 */
	if ((b->o_flags & OID_TABLE ||
	    b->o_flags & OID_REGISTERED) &&
	    (a->o_flags & OID_KEY) == 0 &&
	    (a->o_oidlen > b->o_oidlen))
		return (0);

#if       SUPPORT_OHA_EXTERNAL_MIBS     /* gh2289n: for externally managed MIB's */
	/* gh2289n: Return success if the matched object is a externally managed MIB (don't know any subelements) */
	if ((b->o_flags & OID_EXTERNAL_OHA_MIB) &&
	    (a->o_flags & OID_KEY) == 0 && 
	    (a->o_oidlen > b->o_oidlen))
		return (0);
#endif /* SUPPORT_OHA_EXTERNAL_MIBS */

	return (a->o_oidlen - b->o_oidlen);
}

RB_GENERATE(oidtree, oid, o_element, smi_oid_cmp);

int
smi_key_cmp(struct oid *a, struct oid *b)
{
	if (a->o_name == NULL || b->o_name == NULL)
		return (-1);
	return (strcasecmp(a->o_name, b->o_name));
}

RB_GENERATE(keytree, oid, o_keyword, smi_key_cmp);

/*	$OpenBSD: mib.c,v 1.80 2015/11/17 12:30:23 gerhard Exp $	*/

/*
 * Copyright (c) 2012 Joel Knight <joel@openbsd.org>
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

#ifdef _MSC_VER
/* OBSD_ITGR -- TODO: Omit Warnings at Initialisation of MIB-Tables */
#pragma warning (disable:4204) /* warning C4204: nonstandard extension used : non-constant aggregate initializer */
#pragma warning (disable:4221) /* warning C4221: nonstandard extension used : 'm_ptr' : cannot be initialized using address of automatic variable 'ipstat' */
#endif 

#include <sys/obsd_kernel_queue.h>
#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_proc.h>
#include <sys/obsd_kernel_types.h>
#include <sys/obsd_kernel_stat.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_un.h>
#include <sys/obsd_kernel_time.h>
#include <sys/obsd_kernel_tree.h>
#include <sys/obsd_kernel_sysctl.h>
#include <sys/obsd_kernel_sched.h>
#include <sys/obsd_kernel_socket.h>
#include <sys/obsd_kernel_mount.h>
#include <sys/obsd_kernel_ioctl.h>
#include <sys/obsd_kernel_disk.h>

#include <net/obsd_kernel_if.h>
#include <net/obsd_kernel_if_types.h>
#include <netinet/obsd_kernel_in.h>
#include <netinet/obsd_kernel_in_systm.h>
#include <netinet/obsd_kernel_ip.h>
#include <netinet/obsd_kernel_ip_var.h>
#ifdef IPv6_Genua
#include <netinet/obsd_kernel_icmp6.h>
#include <netinet6/obsd_kernel_ip6_var.h>
#include <netinet6/obsd_kernel_nd6.h>
#endif /*IPv6_Genua*/
#include <arpa/obsd_userland_inet.h>

#include <obsd_userland_stdlib.h>
#include <obsd_userland_stdio.h>
#include <obsd_userland_errno.h>
#include <obsd_userland_event.h>
#include <obsd_kernel_fcntl.h>
#include <obsd_userland_string.h>
#include <obsd_userland_time.h>
#include <obsd_userland_unistd.h>

#include "obsd_snmpd_snmpd.h"
#include "obsd_snmpd_mib.h"

#include "obsd_snmpd_mib_addon_rfc1213.h"


#include "obsd_platform_mibal.h"

                           /*                This is necessary to be compatible to the SPH               */
                           /*                "SNMP in der SIMATIC". It requires to have the RFC1213      */
                           /*                ipRouteTable (OID=1.3.6.1.2.1.4.21) instead of the ipForward*/
                           /*                MIB for basic class devices                                 */
#define OBSD_SNMPD_DISABLE_IP_FORWARD_MIB "xxx"

extern struct snmpd	*env;
extern MIBAL_mib2_system_var_written_cb_t   SysvarWrittenCb;
extern int kr_updatearp(u_int if_index);

/*
 * Defined in SNMPv2-MIB.txt (RFC 3418)
 */

int	 mib_getsys(struct oid *, struct ber_oid *, struct ber_element **);
int	 mib_getsnmp(struct oid *, struct ber_oid *, struct ber_element **);
int	 mib_sysor(struct oid *, struct ber_oid *, struct ber_element **);
static int	 mib_setsys(struct oid *, struct ber_oid *, struct ber_element **);

static struct oid mib_tree[] = MIB_TREE;
static struct ber_oid zerodotzero = { { 0, 0 }, 2 };

#define sizeofa(_a) (sizeof(_a) / sizeof((_a)[0]))

/* base MIB tree */
static struct oid base_mib[] = {
	{ MIB(mib_2),			OID_MIB },
	{ MIB(sysDescr),		OID_RD, mib_getsys },
	{ MIB(sysOID),			OID_RD, mib_getsys },
	{ MIB(sysUpTime),		OID_RD, mib_getsys },
	{ MIB(sysContact),		OID_RW, mib_getsys, mib_setsys },
	{ MIB(sysName),			OID_RW, mib_getsys, mib_setsys },
	{ MIB(sysLocation),		OID_RW, mib_getsys, mib_setsys },
	{ MIB(sysServices),		OID_RD, mib_getsys },
	{ MIB(sysORLastChange),		OID_RD, mps_getts },
	/*  MIB(sysORIndex),		OID_TABLE, mib_sysor } */ /* TFS RQ1702299 ("[ODSB] MIB II - errors"): sysORIndex is a not accessible object */
	{ MIB(sysORID),			OID_TRD, mib_sysor },
	{ MIB(sysORDescr),		OID_TRD, mib_sysor },
	{ MIB(sysORUpTime),		OID_TRD, mib_sysor },
	{ MIB(snmp),			OID_MIB },
	{ MIB(snmpInPkts),		OID_RD, mib_getsnmp },
	{ MIB(snmpOutPkts),		OID_RD, mib_getsnmp },
	{ MIB(snmpInBadVersions),	OID_RD, mib_getsnmp },
	{ MIB(snmpInBadCommunityNames),	OID_RD, mib_getsnmp },
	{ MIB(snmpInBadCommunityUses),	OID_RD, mib_getsnmp },
	{ MIB(snmpInASNParseErrs),	OID_RD, mib_getsnmp },
	{ MIB(snmpInTooBigs),		OID_RD,	mib_getsnmp },
	{ MIB(snmpInNoSuchNames),	OID_RD, mib_getsnmp },
	{ MIB(snmpInBadValues),		OID_RD, mib_getsnmp },
	{ MIB(snmpInReadOnlys),		OID_RD, mib_getsnmp },
	{ MIB(snmpInGenErrs),		OID_RD, mib_getsnmp },
	{ MIB(snmpInTotalReqVars),	OID_RD, mib_getsnmp },
	{ MIB(snmpInTotalSetVars),	OID_RD, mib_getsnmp },
	{ MIB(snmpInGetRequests),	OID_RD, mib_getsnmp },
	{ MIB(snmpInGetNexts),		OID_RD, mib_getsnmp },
	{ MIB(snmpInSetRequests),	OID_RD, mib_getsnmp },
	{ MIB(snmpInGetResponses),	OID_RD, mib_getsnmp },
	{ MIB(snmpInTraps),		OID_RD, mib_getsnmp },
	{ MIB(snmpOutTooBigs),		OID_RD, mib_getsnmp },
	{ MIB(snmpOutNoSuchNames),	OID_RD, mib_getsnmp },
	{ MIB(snmpOutBadValues),	OID_RD, mib_getsnmp },
	{ MIB(snmpOutGenErrs),		OID_RD, mib_getsnmp },
	{ MIB(snmpOutGetRequests),	OID_RD, mib_getsnmp },
	{ MIB(snmpOutGetNexts),		OID_RD, mib_getsnmp },
	{ MIB(snmpOutSetRequests),	OID_RD, mib_getsnmp },
	{ MIB(snmpOutGetResponses),	OID_RD, mib_getsnmp },
	{ MIB(snmpOutTraps),		OID_RD, mib_getsnmp },
	{ MIB(snmpEnableAuthenTraps),	OID_RD, mib_getsnmp },
	{ MIB(snmpSilentDrops),		OID_RD, mib_getsnmp },
	{ MIB(snmpProxyDrops),		OID_RD, mib_getsnmp },
	{ MIBEND }
};

/* Task 1448783: no "german" characters like ä,ö,ü */
int mps_international_ascii(char *str, int len)
{
   int valid_character;
   int i = 0;

   for (i = 0; i < len; i++)
   {
      valid_character = 0;
      switch (str[i])
      {
         case 0:    /* NUL - null            */
         case 7:    /* BEL - bell            */
         case 8:    /* BS  - back space      */
         case 9:    /* HT  - horizontal tab  */
         case 10:   /* LF  - line feed       */
         case 11:   /* VT  - vertical tab    */
         case 12:   /* FF  - form feed       */
            valid_character = 1;
            break;
         case 13:   /* CR  - carriage return */
            /* next char must be NUL OR LF */
            if ((str[i+1] == 0) || (str[i+1] == 10))
               valid_character = 1;
            break;
         default:  
            if ((str[i] >= 32) && (str[i] <= 126))
               valid_character = 1;
            break;
      }
      if (!valid_character)
         return 0;  /* input char is no international ASCII */
   }

   return 1;
}

int
mib_getsys(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
#ifdef MIB_SYSOID_DEFAULT /* RQ 1435418 - sysObjectID = {0,0} */
	struct ber_oid		 sysoid = OID(MIB_SYSOID_DEFAULT); /* gh2289n: remark: compiler may generate code here that internally calls memset from compiler's clib */
	struct ber_oid		*so = oid->o_data;
#endif
	char			*s = oid->o_data;
	long long		 ticks;

	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	switch (oid->o_oid[OIDIDX_system]) {
	case 1:  /* gh2289n: sysDescr, DisplayString (SIZE (0..255)) */
		if (s == NULL) {
			if((s = MIBAL_get_mib2_sysDescr()) == NULL) /* strlen <= 255 */
				return (-1);
		}
		*elm = ber_add_string(*elm, s);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 2:    /* gh2289n: sysObject, ID OBJECT IDENTIFIER */
#ifdef MIB_SYSOID_DEFAULT /* RQ 1435418 - sysObjectID = {0,0} */
		if (so == NULL)
			so = &sysoid;
		smi_oidlen(so);
		*elm = ber_add_oid(*elm, so);
#else
		*elm = ber_add_oid(*elm, &zerodotzero);
#endif
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 3:     /* gh2289n: sysUpTime, TimeTicks, The time (in hundredths of a second) since the */
	            /* network management portion of the system was last re-initialized              */
		ticks = smi_getticks();
		*elm = ber_add_integer(*elm, ticks);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
		break;
	case 4:      /* gh2289n: sysContact, DisplayString (SIZE (0..255)) */
		if (s == NULL) {
			if ((s = MIBAL_get_mib2_sysContact()) == NULL) /* strlen <= 255 */
				return (-1);
		}
		*elm = ber_add_string(*elm, s);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 5:      /* gh2289n: sysName, DisplayString (SIZE (0..255)) */
		if (s == NULL) {
			if((s = MIBAL_get_mib2_sysName()) == NULL) /* strlen <= 255 */
				return (-1);
		}
		*elm = ber_add_string(*elm, s);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 6:      /* gh2289n: sysLocation, DisplayString (SIZE (0..255)) */
		if (s == NULL)
		{
			if((s= MIBAL_get_mib2_sysLocation()) == NULL) /* strlen <= 255 */
				return (-1);
		}
		*elm = ber_add_string(*elm, s);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	case 7:      /* gh2289n: sysServices, INTEGER (0..127), "A value which indicates the set of  */
	             /* services that this entity primarily offers.                                  */
	             /* The value is a sum.  This sum initially takes the value zero, Then, for each */
	             /* layer, L, in the range 1 through 7, that this node performs transactions     */
	             /* for, 2 raised to (L - 1) is added to the sum. For example, a node which      */
	             /* performs primarily routing functions would have a value of 4 (2^(3-1)).  In  */
	             /* contrast, a node which is a host offering application services would have a  */
	             /* value of 72 (2^(4-1) + 2^(7-1)).  Note that in the context of the Internet   */
	             /* suite of protocols, values should be calculated accordingly:                 */
	             /* layer  functionality                                                         */
	             /*    1  physical (e.g., repeaters)                                             */
	             /*    2  datalink/subnetwork (e.g., bridges)                                    */
	             /*    3  internet (e.g., IP gateways)                                           */
	             /*    4  end-to-end  (e.g., IP hosts)                                           */
	             /*    7  applications (e.g., mail relays)                                       */
	             /* For systems including OSI protocols, layers 5 and 6 may also be counted."    */
		oid->o_val = MIBAL_get_mib2_sysServices();
		*elm = ber_add_integer(*elm, oid->o_val);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	default:
		return (-1);
	}
	return (0);
}

static int mib_setsys(struct oid *oid, struct ber_oid *o, struct ber_element **elm)		/* mh2290: LINT 839 Storage class of symbol 'Symbol' assumed static */
{
	struct ber_element  *ber    = *elm;
	int                 SubOid  = oid->o_oid[OIDIDX_system];
	int                 rc      = OBSD_SNMPD_MPS_ERRCODE_GENERIC_ERROR;

	OBSD_UNUSED_ARG(o); /* Warning */

	if  (((oid->o_flags & OID_WR) != 0)          &&       /* to be sure */
		 (ber->be_class == BER_CLASS_UNIVERSAL)  &&
		 (ber->be_type  == BER_TYPE_OCTETSTRING)
		)
	{
		char *s;

		/* Task 1448783: no "german" characters like ä,ö,ü */
		if (!mps_international_ascii((char *)ber->be_val, ber->be_len))
		{
			return OBSD_SNMPD_MPS_ERRCODE_BADVALUE;
		}

		if (ber_get_string(ber, &s) != -1)
		{
			int string_length = (int) (ber->be_len);
			int mibal_rc      = OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR;

			switch (SubOid)
			{
				case 1:      /* gh2289n: sysDescr,    DisplayString (SIZE (0..255)) */
					/* normally we should never get here, because sysDescr is defined as readonly */
					/* in the "SPH SNMP in der SIMATIC" (and in the RFC too). But if someone      */
					/* accidentally changes the OID_RD into a OID_RW in our base_mib there might  */
					/* be a write request. Originally the sysxx-Strings in the OpenBSD SNMPD are  */
					/* allocated dynamically. For PN we introduced the UserlandAdapter and we     */
					/* tried to avoid the need to dynamically allocate the memory (by using our   */
					/* malloc) for the sysxxx strings. And so we have to avoid calling mps_setstr */
					/* for the sysxxx-strings, that are handled via the UserlandAdapter (and are  */
					/* possibly not dynamically allocated via our malloc). This is because        */
					/* mps_setstr calls free to the given MIB-string which leads to free-ing a not*/
					/* malloc'ed memory. So we introduce this case and return an error, avoiding  */
					/* any call to mps_setstr.                                                    */
					mibal_rc = OBSD_SNMPD_MIBAL_ERRCODE_READ_ONLY;
					break;
				case 4:      /* gh2289n: sysContact,  DisplayString (SIZE (0..255)) */
					mibal_rc = MIBAL_set_mib2_sysContact(s, string_length);
					if (SysvarWrittenCb != NULL)  /* call information changed callback if necessary */
					{
						SysvarWrittenCb(MIBAL_get_mib2_sysContact());
					}

					break;
				case 5:      /* gh2289n: sysName,     DisplayString (SIZE (0..255)) */
					mibal_rc = MIBAL_set_mib2_sysName(s, string_length);
					if (SysvarWrittenCb != NULL)  /* call information changed callback if necessary */
					{
						SysvarWrittenCb(MIBAL_get_mib2_sysName());
					}

					break;
				case 6:      /* gh2289n: sysLocation, DisplayString (SIZE (0..255)) */
					mibal_rc = MIBAL_set_mib2_sysLocation(s, string_length);
					if (SysvarWrittenCb != NULL)  /* call information changed callback if necessary */
					{
						SysvarWrittenCb(MIBAL_get_mib2_sysLocation());
					}

					break;
				default:		/* mh2290: LINT 744 switch statement has no default */
					break;
			}
			/* map MIBAL error codes to MPS error codes */
			switch(mibal_rc) 
			{
				case OBSD_SNMPD_MIBAL_ERRCODE_OK:               rc = OBSD_SNMPD_MPS_ERRCODE_OK;             break;
				case OBSD_SNMPD_MIBAL_ERRCODE_READ_ONLY:        rc = OBSD_SNMPD_MPS_ERRCODE_READ_ONLY;      break;
				case OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG:  rc = OBSD_SNMPD_MPS_ERRCODE_WRONG_LENGTH;   break;
				case OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR:    /* no break */
				default:                                        rc = OBSD_SNMPD_MPS_ERRCODE_GENERIC_ERROR;  break;
			}
		}
		else
		{
			return OBSD_SNMPD_MPS_ERRCODE_BADVALUE;	
		}
	}
	/* rc = mps_setstr(oid, o, elm); no other sub-OIDs supported for set */
	return rc;
}

int
mib_sysor(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	u_int32_t		 idx = 1, nmib = 0;
	struct oid		*next, *miboid;
	char			 buf[SNMPD_MAXSTRLEN];

	OBSD_UNUSED_ARG(oid); /* gh2289n: avoid compiler warning */
	/* Count MIB root OIDs in the tree */
	for (next = NULL;
	    (next = smi_foreach(next, OID_MIB)) != NULL; nmib++);

	/* Get and verify the current row index */
	idx = o->bo_id[OIDIDX_sysOREntry];
	if (idx > nmib)
		return (1);

	/* Find the MIB root element for this Id */
	for (next = miboid = NULL, nmib = 1;
	    (next = smi_foreach(next, OID_MIB)) != NULL; nmib++) {
		if (nmib == idx)
			miboid = next;
	}
	if (miboid == NULL)
		return (-1);

	/* Tables need to prepend the OID on their own */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_sysOR]) {
		case 1:
			return -1; /* PN SPH -- not available */
		case 2:
			ber = ber_add_oid(ber, &miboid->o_id);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:
			/*
			 * This should be a description of the MIB.
			 * But we use the symbolic OID string for now, it may
			 * help to display names of internal OIDs.
			 */
			smi_oid2string(&miboid->o_id, buf, sizeof(buf), 0);
			ber = ber_add_string(ber, buf);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 4:
			/*
			 * We do not support dynamic loading of MIB at runtime,
			 * the sysORUpTime value of 0 will indicate "loaded at
			 * startup".
			 */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

int
mib_getsnmp(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct snmp_stats	*stats = &env->sc_stats;
	long long		 i;
	struct statsmap {
		u_int8_t	 m_id;
		u_int32_t	*m_ptr;
	};
	
	struct statsmap mapping[29];
	bzero(mapping, sizeof(mapping));
	
	mapping[0].m_id  =  1;   mapping[0].m_ptr  = &stats->snmp_inpkts; 
	mapping[1].m_id  =  2;   mapping[1].m_ptr  = &stats->snmp_outpkts;
    mapping[2].m_id  =  3;   mapping[2].m_ptr  = &stats->snmp_inbadversions;
    mapping[3].m_id  =  4;   mapping[3].m_ptr  = &stats->snmp_inbadcommunitynames;
    mapping[4].m_id  =  5;   mapping[4].m_ptr  = &stats->snmp_inbadcommunityuses;
    mapping[5].m_id  =  6;   mapping[5].m_ptr  = &stats->snmp_inasnparseerrs;
    mapping[6].m_id  =  8;   mapping[6].m_ptr  = &stats->snmp_intoobigs;
    mapping[7].m_id  =  9;   mapping[7].m_ptr  = &stats->snmp_innosuchnames;
    mapping[8].m_id  = 10;   mapping[8].m_ptr  = &stats->snmp_inbadvalues;
    mapping[9].m_id  = 11;   mapping[9].m_ptr  = &stats->snmp_inreadonlys;
    mapping[10].m_id = 12;   mapping[10].m_ptr = &stats->snmp_ingenerrs;
    mapping[11].m_id = 13;   mapping[11].m_ptr = &stats->snmp_intotalreqvars;
    mapping[12].m_id = 14;   mapping[12].m_ptr = &stats->snmp_intotalsetvars;
    mapping[13].m_id = 15;   mapping[13].m_ptr = &stats->snmp_ingetrequests;
    mapping[14].m_id = 16;   mapping[14].m_ptr = &stats->snmp_ingetnexts;
    mapping[15].m_id = 17;   mapping[15].m_ptr = &stats->snmp_insetrequests;
    mapping[16].m_id = 18;   mapping[16].m_ptr = &stats->snmp_ingetresponses;
    mapping[17].m_id = 19;   mapping[17].m_ptr = &stats->snmp_intraps;
    mapping[18].m_id = 20;   mapping[18].m_ptr = &stats->snmp_outtoobigs;
    mapping[19].m_id = 21;   mapping[19].m_ptr = &stats->snmp_outnosuchnames;
    mapping[20].m_id = 22;   mapping[20].m_ptr = &stats->snmp_outbadvalues;
    mapping[21].m_id = 24;   mapping[21].m_ptr = &stats->snmp_outgenerrs;
    mapping[22].m_id = 25;   mapping[22].m_ptr = &stats->snmp_outgetrequests;
    mapping[23].m_id = 26;   mapping[23].m_ptr = &stats->snmp_outgetnexts;
    mapping[24].m_id = 27;   mapping[24].m_ptr = &stats->snmp_outsetrequests;
    mapping[25].m_id = 28;   mapping[25].m_ptr = &stats->snmp_outgetresponses;
    mapping[26].m_id = 29;   mapping[26].m_ptr = &stats->snmp_outtraps;
    mapping[27].m_id = 31;   mapping[27].m_ptr = &stats->snmp_silentdrops;
    mapping[28].m_id = 32;   mapping[28].m_ptr = &stats->snmp_proxydrops;

	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	switch (oid->o_oid[OIDIDX_snmp]) {
	case 30:
		i = stats->snmp_enableauthentraps == 1 ? 1 : 2;
		*elm = ber_add_integer(*elm, i);
		if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		break;
	default:
		for (i = 0;
		    (u_int)i < (sizeof(mapping) / sizeof(mapping[0])); i++) {
			if (oid->o_oid[OIDIDX_snmp] == mapping[i].m_id) {
				*elm = ber_add_integer(*elm, *mapping[i].m_ptr);
				if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
				return (0);
			}
		}
		return (-1);
	}

	return (0);
}




/*
 * Defined in IF-MIB.txt (RFCs 1229, 1573, 2233, 2863)
 */

int	 mib_ifnumber(struct oid *, struct ber_oid *, struct ber_element **);
struct kif
	*mib_ifget(u_int);
int	 mib_iftable(struct oid *, struct ber_oid *, struct ber_element **);
int	 mib_ifxtable(struct oid *, struct ber_oid *, struct ber_element **);
int	 mib_ifstacklast(struct oid *, struct ber_oid *, struct ber_element **);
int	 mib_ifrcvtable(struct oid *, struct ber_oid *, struct ber_element **);

static u_int8_t ether_zeroaddr[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

static struct oid if_mib[] = {
	{ MIB(ifMIB),			OID_MIB },
#if       OBSD_SNMPD_ENABLE_RFC2863_IFMIB     /* gh2289n: ifMIB's ifXTable (1.3.6.1.2.1.31.1.1) */
	{ MIB(ifName),			OID_TRD, mib_ifxtable },
	{ MIB(ifInMulticastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifInBroadcastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifOutMulticastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifOutBroadcastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifHCInOctets),		OID_TRD, mib_ifxtable },
	{ MIB(ifHCInUcastPkts),		OID_TRD, mib_ifxtable },
	{ MIB(ifHCInMulticastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifHCInBroadcastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifHCOutOctets),		OID_TRD, mib_ifxtable },
	{ MIB(ifHCOutUcastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifHCOutMulticastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifHCOutBroadcastPkts),	OID_TRD, mib_ifxtable },
	{ MIB(ifLinkUpDownTrapEnable),	OID_TRD, mib_ifxtable },
	{ MIB(ifHighSpeed),		OID_TRD, mib_ifxtable },
	{ MIB(ifPromiscuousMode),	OID_TRD, mib_ifxtable },
	{ MIB(ifConnectorPresent),	OID_TRD, mib_ifxtable },
	{ MIB(ifAlias),			OID_TRD, mib_ifxtable },
	{ MIB(ifCounterDiscontinuityTime), OID_TRD, mib_ifxtable },
#endif /* OBSD_SNMPD_ENABLE_RFC2863_IFMIB */
#if       OBSD_SNMPD_ENABLE_RFC2863_IFMIB     /* gh2289n: ifMIB's ifRcvAddressTable (1.3.6.1.2.1.31.1.4) */
	{ MIB(ifRcvAddressStatus),	OID_TRD, mib_ifrcvtable },
	{ MIB(ifRcvAddressType),	OID_TRD, mib_ifrcvtable },
#endif /* OBSD_SNMPD_ENABLE_RFC2863_IFMIB */
#if       OBSD_SNMPD_ENABLE_RFC2863_IFMIB     /* gh2289n: ifMIB's ifStackLastChange (1.3.6.1.2.1.31.1.6) */
	{ MIB(ifStackLastChange),	OID_RD, mib_ifstacklast },
#endif /* OBSD_SNMPD_ENABLE_RFC2863_IFMIB */
	{ MIB(ifNumber),		OID_RD, mib_ifnumber },
	{ MIB(ifIndex),			OID_TRD, mib_iftable },
	{ MIB(ifDescr),			OID_TRD, mib_iftable },
	{ MIB(ifType),			OID_TRD, mib_iftable },
	{ MIB(ifMtu),			OID_TRD, mib_iftable },
	{ MIB(ifSpeed),			OID_TRD, mib_iftable },
	{ MIB(ifPhysAddress),		OID_TRD, mib_iftable },
	{ MIB(ifAdminStatus),		OID_TRD, mib_iftable },
	{ MIB(ifOperStatus),		OID_TRD, mib_iftable },
	{ MIB(ifLastChange),		OID_TRD, mib_iftable },
	{ MIB(ifInOctets),		OID_TRD, mib_iftable },
	{ MIB(ifInUcastPkts),		OID_TRD, mib_iftable },
	{ MIB(ifInNUcastPkts),		OID_TRD, mib_iftable },
	{ MIB(ifInDiscards),		OID_TRD, mib_iftable },
	{ MIB(ifInErrors),		OID_TRD, mib_iftable },
	{ MIB(ifInUnknownProtos),	OID_TRD, mib_iftable },
	{ MIB(ifOutOctets),		OID_TRD, mib_iftable },
	{ MIB(ifOutUcastPkts),		OID_TRD, mib_iftable },
	{ MIB(ifOutNUcastPkts),		OID_TRD, mib_iftable },
	{ MIB(ifOutDiscards),		OID_TRD, mib_iftable },
	{ MIB(ifOutErrors),		OID_TRD, mib_iftable },
	{ MIB(ifOutQLen),		OID_TRD, mib_iftable },
	{ MIB(ifSpecific),		OID_TRD, mib_iftable },
	{ MIBEND }
};

int
mib_ifnumber(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	*elm = ber_add_integer(*elm, kr_ifnumber());
	if (*elm == NULL) return -1;
	return (0);
}

struct kif *
mib_ifget(u_int idx)
{
	struct kif	*kif;

	if ((kif = kr_getif((u_short)idx)) == NULL) { /* OBSD_ITGR -- omit warning */
		/*
		 * It may happen that an interface with a specific index
		 * does not exist or has been removed. Jump to the next
		 * available interface index.
		 */
		for (kif = kr_getif(0); kif != NULL;
		    kif = kr_getnextif(kif->if_index))
			if (kif->if_index > idx)
				break;
		if (kif == NULL)
			return (NULL);
	}
	idx = kif->if_index;

	/* Update interface information */
	kr_updateif(idx);
	if ((kif = kr_getif((u_short)idx)) == NULL) { /* OBSD_ITGR -- omit warning */
		log_debug("mib_ifxtable: interface %d disappeared?", idx);
		return (NULL);
	}

	return (kif);
}

                                 /*          description in the informations used for the ifmib.  */
                                 /*          Requested by OBSD_ITGR.                              */
void mib_set_ifdescr(       unsigned short if_index, /* Key: net statistics belong to             */
                            char* new_if_description /* interface description used for ifmib      */
                     )
{
    struct kif * kr_if = kr_getif(if_index);

    if (kr_if)
    {
        strlcpy(kr_if->if_descr, new_if_description, sizeof(kr_if->if_descr)/sizeof(kr_if->if_descr[0]));
    }
}


int
mib_iftable(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	u_int32_t		 idx = 0;
	struct kif		*kif;
	long long		 i;

	OBSD_UNUSED_ARG(oid);  /* gh2289n: avoid compiler warning */

	/* Get and verify the current row index */
	idx = o->bo_id[OIDIDX_ifEntry];
	if ((kif = mib_ifget(idx)) == NULL)
		return (1);

	/* Tables need to prepend the OID on their own */
	o->bo_id[OIDIDX_ifEntry] = kif->if_index;
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_if]) {
		case 1:
			ber = ber_add_integer(ber, kif->if_index);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 2:
			/*
			 * The ifDescr should contain a vendor, product, etc.
			 * but we just use the interface name (like ifName).
			 * The interface name includes the driver name on OpenBSD.
			 */
			ber = ber_add_string(ber, kif->if_descr);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:
			/* SNMP CARP */
			if (kif->if_type == IFT_CARP)
			{
				ber = ber_add_integer(ber, IFT_ETHER);
			}
			else if (kif->if_type >= 0xf0) {
				/*
				 * It does not make sense to announce the private
				 * interface types for ENC, PFSYNC, etc.
				 */
				ber = ber_add_integer(ber, IFT_OTHER);
			}
			else
				ber = ber_add_integer(ber, kif->if_type);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 4:
			ber = ber_add_integer(ber, kif->if_mtu);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 5:
			ber = ber_add_integer(ber, kif->if_baudrate);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 6:
			if (bcmp(kif->if_lladdr, ether_zeroaddr,
				sizeof(kif->if_lladdr)) == 0) {
				ber = ber_add_string(ber, "");
			}
			else {
				ber = ber_add_nstring(ber, (const char*)kif->if_lladdr, /* OBSD_ITGR - omit warnings */
					sizeof(kif->if_lladdr));
			}
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 7:
			/* ifAdminStatus up(1), down(2), testing(3) */
			i = (kif->if_flags & IFF_UP) ? 1 : 2;
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 8:
			/* ifOperStatus */
			if (kif->if_link_state == LINK_STATE_UP)
				i = 1;	/* up(1) */
			else if (kif->if_link_state == LINK_STATE_DOWN)
				i = 2; /* down(2) */
			else
				i = 4;	/* unknown(4) */
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 9:
		{
			/* gh2289n: RFC1213 ifLastChange (OID 1.3.6.1.2.1.2.2.1.9 ):                                         */
			/*          "The value of sysUpTime at the time the interface entered its current operational state. */
			/*           If the current state was entered prior to the last re-initialization of the local       */
			/*           network management subsystem, then this object contains a zero value."                  */
			/*           sysUpTime (OID 1.3.6.1.2.1.1.3): "The time (in hundredths of a second) since the        */
			/*           network management portion of the system was last re-initialized."                      */
			/*           --> so we need a time value for ifLastChange that is in hundredths of a second (10ms)   */

			long timeticks_10ms = kif->if_data.ifi_lastchange.tv_sec * 100;
			timeticks_10ms += kif->if_data.ifi_lastchange.tv_usec / 10000;
			ber = ber_add_integer(ber, timeticks_10ms);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
		}
		break;
		case 10:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INOCTETS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_ibytes);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 11:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INUCASTPKTS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_ipackets);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 12:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INNUCASTPKTS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_imcasts);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 13:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INDISCARDS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_iqdrops);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 14:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INERRORS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_ierrors);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 15:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_INUNKNOWNPROTOS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_noproto);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 16:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTOCTETS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_obytes);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 17:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTUCASTPKTS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_opackets);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 18:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTNUCASTPKTS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_omcasts);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 19:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTDISCARDS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_data.ifOutDiscards);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 20:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTERRORS) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_oerrors);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 21:
			if (kif->if_data.SupportedCounters & OBSD_MIB_EDD_SUPPORT_OUTQLEN) {
				ber = ber_add_integer(ber, (u_int32_t)kif->if_data.ifOutQLen);
				if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
				ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			}
			else {
				ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_NOT_SUPPORTED);
			}
			break;
		case 22:
			ber = ber_add_oid(ber, &zerodotzero);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

int
mib_ifxtable(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	u_int32_t		 idx = 0;
	struct kif		*kif;
	int			 i = 0;

	OBSD_UNUSED_ARG(oid);  /* gh2289n: avoid compiler warning */

	/* Get and verify the current row index */
	idx = o->bo_id[OIDIDX_ifXEntry];
	if ((kif = mib_ifget(idx)) == NULL)
		return (1);

	/* Tables need to prepend the OID on their own */
	o->bo_id[OIDIDX_ifXEntry] = kif->if_index;
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ifX]) {
		case 1:
			ber = ber_add_string(ber, kif->if_name);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 2:
			ber = ber_add_integer(ber, (u_int32_t)kif->if_imcasts);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 3:
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 4:
			ber = ber_add_integer(ber, (u_int32_t)kif->if_omcasts);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 5:
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 6:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_ibytes);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 7:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_ipackets);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 8:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_imcasts);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 9:
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 10:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_obytes);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 11:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_opackets);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 12:
			ber = ber_add_integer(ber, (u_int64_t)kif->if_omcasts);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 13:
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER64);
			break;
		case 14:
			ber = ber_add_integer(ber, 0);	/* enabled(1), disabled(2) */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 15:
			i = kif->if_baudrate >= 1000000 ?
				(int)(kif->if_baudrate / 1000000) : 0; /* OBSD_ITGR -- omit warning */
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 16:
			/* ifPromiscuousMode: true(1), false(2) */
			i = kif->if_flags & IFF_PROMISC ? 1 : 2;
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 17:
			/* ifConnectorPresent: false(2), true(1) */
			i = kif->if_type == IFT_ETHER ? 1 : 2;
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 18:
			ber = ber_add_string(ber, kif->if_descr);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 19:
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

int
mib_ifstacklast(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	ber = ber_add_integer(ber, kr_iflastchange());
	if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

	ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
	return (0);
}

int
mib_ifrcvtable(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	u_int32_t		 idx = 0;
	struct kif		*kif;
	u_int			 i = 0;

	OBSD_UNUSED_ARG(oid);  /* gh2289n: avoid compiler warning */

	/* Get and verify the current row index */
	idx = o->bo_id[OIDIDX_ifRcvAddressEntry];
	if ((kif = mib_ifget(idx)) == NULL)
		return (1);

	/*
	 * The lladdr of the interface will be encoded in the returned OID
	 * ifRcvAddressX.ifindex.6.x.x.x.x.x.x = val
	 * Thanks to the virtual cloner interfaces, it is an easy 1:1
	 * mapping in OpenBSD; only one lladdr (MAC) address per interface.
	 */

	/* first set the base OID and caluculate the length */
	idx = 0;
	o->bo_id[OIDIDX_ifRcvAddressEntry + idx++] = kif->if_index;
	o->bo_id[OIDIDX_ifRcvAddressEntry + idx] = 0;
	smi_oidlen(o);

	/* extend the OID with the lladdr length and octets */
	o->bo_id[OIDIDX_ifRcvAddressEntry + idx++] = sizeof(kif->if_lladdr);
	o->bo_n++;
	for (i = 0; i < sizeof(kif->if_lladdr); i++, o->bo_n++)
		o->bo_id[OIDIDX_ifRcvAddressEntry + idx++] = kif->if_lladdr[i];

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ifRcvAddress]) {
		case 2:
			/* ifRcvAddressStatus: RowStatus active(1), notInService(2) */
			i = kif->if_flags & IFF_UP ? 1 : 2;
			ber = ber_add_integer(ber, i);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:
			/* ifRcvAddressType: other(1), volatile(2), nonVolatile(3) */
			ber = ber_add_integer(ber, 1);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}


/*
 * Defined in IP-MIB.txt
 */

int mib_getipstat(struct ipstat *);
int mib_ipstat(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipforwarding(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipdefaultttl(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipinhdrerrs(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipinaddrerrs(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipforwdgrams(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipindiscards(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipreasmfails(struct oid *, struct ber_oid *, struct ber_element **);
int mib_ipfragfails(struct oid *, struct ber_oid *, struct ber_element **);
int mib_iproutingdiscards(struct oid *, struct ber_oid *,
    struct ber_element **);
int mib_ipaddr(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    mib_ipaddrtable(struct oid *, struct ber_oid *, struct ber_oid *);
int mib_physaddr(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    mib_physaddrtable(struct oid *, struct ber_oid *, struct ber_oid *);
#ifdef IPv6_Genua
static int mib_getip6stat(struct ip6stat *);
int obsd_snmpd_ipv6forwarding(struct oid *, struct ber_oid *, struct ber_element **);
int obsd_snmpd_ipv6defaulthoplimit(struct oid *, struct ber_oid *, struct ber_element **);
int obsd_snmpd_ipsystemstats(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    obsd_snmpd_ipsystemstatstable(struct oid *, struct ber_oid *, struct ber_oid *);

int obsd_snmpd_ipifstats(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    obsd_snmpd_ipifstatstable(struct oid *, struct ber_oid *, struct ber_oid *);

int obsd_snmpd_ipaddressprefix(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    obsd_snmpd_ipaddressprefixtable(struct oid *, struct ber_oid *, struct ber_oid *);
int obsd_snmpd_ipdefaultrouter(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    obsd_snmpd_ipdefaultroutertable(struct oid *, struct ber_oid *, struct ber_oid *);
int obsd_snmpd_spinlock(struct oid *, struct ber_oid *,
    struct ber_element **);
int obsd_snmpd_ipaddress(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
    obsd_snmpd_ipaddresstable(struct oid *, struct ber_oid *, struct ber_oid *);
#endif /*IPv6_Genua*/


static struct oid ip_mib[] = {
	{ MIB(ipMIB),			OID_MIB },
	{ MIB(ipForwarding),		OID_RD, mib_ipforwarding },
	{ MIB(ipDefaultTTL),		OID_RD, mib_ipdefaultttl },
	{ MIB(ipInReceives),		OID_RD, mib_ipstat },
	{ MIB(ipInHdrErrors),		OID_RD, mib_ipinhdrerrs },
	{ MIB(ipInAddrErrors),		OID_RD, mib_ipinaddrerrs },
	{ MIB(ipForwDatagrams),		OID_RD, mib_ipforwdgrams },
	{ MIB(ipInUnknownProtos),	OID_RD, mib_ipstat },
	{ MIB(ipInDiscards),		OID_RD, mib_ipindiscards  },
	{ MIB(ipInDelivers),		OID_RD, mib_ipstat },
	{ MIB(ipOutRequests),		OID_RD, mib_ipstat },
	{ MIB(ipOutDiscards),		OID_RD, mib_ipstat },
	{ MIB(ipOutNoRoutes),		OID_RD, mib_ipstat },
	{ MIB(ipReasmTimeout),		OID_RD, mps_getint, NULL,
	    NULL, IPFRAGTTL },
	{ MIB(ipReasmReqds),		OID_RD, mib_ipstat },
	{ MIB(ipReasmOKs),		OID_RD, mib_ipstat },
	{ MIB(ipReasmFails),		OID_RD, mib_ipreasmfails },
	{ MIB(ipFragOKs),		OID_RD, mib_ipstat },
	{ MIB(ipFragFails),		OID_RD, mib_ipfragfails },
	{ MIB(ipFragCreates),		OID_RD, mib_ipstat },
	{ MIB(ipAdEntAddr),		OID_TRD, mib_ipaddr, NULL,
	    mib_ipaddrtable },
	{ MIB(ipAdEntIfIndex),		OID_TRD, mib_ipaddr, NULL,
	    mib_ipaddrtable },
	{ MIB(ipAdEntNetMask),		OID_TRD, mib_ipaddr, NULL,
	    mib_ipaddrtable },
	{ MIB(ipAdEntBcastAddr),	OID_TRD, mib_ipaddr, NULL,
	    mib_ipaddrtable },
	{ MIB(ipAdEntReasmMaxSize),	OID_TRD, mib_ipaddr, NULL,
	    mib_ipaddrtable },
	{ MIB(ipNetToMediaIfIndex),	OID_TRD, mib_physaddr, NULL,
	    mib_physaddrtable },
	{ MIB(ipNetToMediaPhysAddress),	OID_TRD, mib_physaddr, NULL,
	    mib_physaddrtable },
	{ MIB(ipNetToMediaNetAddress),	OID_TRD, mib_physaddr, NULL,
	    mib_physaddrtable },
	{ MIB(ipNetToMediaType),	OID_TRD, mib_physaddr, NULL,
	    mib_physaddrtable },
#ifdef IPv6_Genua
#ifdef notyet
	{ MIB(ipv6IpForwarding),	OID_RD, obsd_snmpd_ipv6forwarding },
	{ MIB(ipv6DefaultHopLimit),	OID_RD, obsd_snmpd_ipv6defaulthoplimit },
#endif
	{ MIB(ipTrafficStats),			OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsTable),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsEntry),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInReceives),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInReceives),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInOctets),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInOctets),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInHdrErrors),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInNoRoutes),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInAddrErrors),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInUnknownProtos),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInTruncatedPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInForwDatagrams),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInForwDatagrams),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsReasmReqds),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsReasmOKs),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsReasmFails),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInDiscards),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInDelivers),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInDelivers),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutRequests),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutRequests),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutNoRoutes),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutForwDatagrams),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutForwDatagrams),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutDiscards),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutFragReqds),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutFragOKs),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutFragFails),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutFragCreates),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutTransmits),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutTransmits),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutOctets),		OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutOctets),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInMcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInMcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInMcastOctets),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInMcastOctets),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutMcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutMcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutMcastOctets),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutMcastOctets),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsInBcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCInBcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsOutBcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsHCOutBcastPkts),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsDiscontinuityTime),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },
	{ MIB(ipSystemStatsRefreshRate),	OID_TRD, obsd_snmpd_ipsystemstats, NULL, obsd_snmpd_ipsystemstatstable },

	{ MIB(ipIfStatsTable),			OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsEntry),			OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInReceives),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInReceives),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInHdrErrors),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInNoRoutes),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInAddrErrors),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInUnknownProtos),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInTruncatedPkts),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInForwDatagrams),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInForwDatagrams),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsReasmReqds),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsReasmOKs),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsReasmFails),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInDiscards),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInDelivers),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInDelivers),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutRequests),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutRequests),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutNoRoutes),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutForwDatagrams),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutForwDatagrams),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutDiscards),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutFragReqds),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutFragOKs),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutFragFails),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutFragCreates),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutTransmits),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutTransmits),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInMcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInMcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInMcastOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInMcastOctets),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutMcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutMcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutMcastOctets),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutMcastOctets),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsInBcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCInBcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsOutBcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsHCOutBcastPkts),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsDiscontinuityTime),	OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },
	{ MIB(ipIfStatsRefreshRate),		OID_TRD, obsd_snmpd_ipifstats, NULL, obsd_snmpd_ipifstatstable },

	{ MIB(ipAddressPrefixOrigin),	OID_TRD, obsd_snmpd_ipaddressprefix, NULL, obsd_snmpd_ipaddressprefixtable },
	{ MIB(ipAddressPrefixOnLinkFlag),		OID_TRD, obsd_snmpd_ipaddressprefix, NULL, obsd_snmpd_ipaddressprefixtable },
	{ MIB(ipAddressPrefixAutonomousFlag),		OID_TRD, obsd_snmpd_ipaddressprefix, NULL, obsd_snmpd_ipaddressprefixtable },
	{ MIB(ipAddressPrefixAdvPreferredLifetime),	OID_TRD, obsd_snmpd_ipaddressprefix, NULL, obsd_snmpd_ipaddressprefixtable },
	{ MIB(ipAddressPrefixAdvValidLifetime),		OID_TRD, obsd_snmpd_ipaddressprefix, NULL, obsd_snmpd_ipaddressprefixtable },
	{ MIB(ipAddressSpinLock),	OID_RD,  obsd_snmpd_spinlock },
	{ MIB(ipAddressType),		OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressIfIndex),	OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressType),		OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressPrefix),		OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressOrigin),		OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressStatus),		OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressCreated),	OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressLastChanged),	OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressRowStatus),	OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipAddressStorageType),	OID_TRD, obsd_snmpd_ipaddress, NULL, obsd_snmpd_ipaddresstable },
	{ MIB(ipDefaultRouterLifetime),		OID_TRD, obsd_snmpd_ipdefaultrouter, NULL, obsd_snmpd_ipdefaultroutertable },
	{ MIB(ipDefaultRouterPreference),	OID_TRD, obsd_snmpd_ipdefaultrouter, NULL, obsd_snmpd_ipdefaultroutertable },
	{ MIB(ipv6RouterAdvertSpinLock),	OID_RD, obsd_snmpd_spinlock },
#endif /*IPv6_Genua*/
	{ MIB(ipRoutingDiscards), OID_RD, mib_iproutingdiscards },
	{ MIBEND }
};

int
mib_ipforwarding(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	int	mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_FORWARDING };
	int	v;
	size_t	len = sizeof(v);

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (sysctl(mib, sizeofa(mib), &v, &len, NULL, 0) == -1)
		return (-1);

	/* ipForwarding: forwarding(1), notForwarding(2) */
	*elm = ber_add_integer(*elm, (v == 0) ? 2 : 1);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

	return (0);
}

#ifdef IPv6_Genua
int
obsd_snmpd_ipv6forwarding(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	int     mib[] = { CTL_NET, PF_INET6, IPPROTO_IPV6, IPV6CTL_FORWARDING };
	int     v;
	size_t  len = sizeof(v);

	if (sysctl(mib, sizeofa(mib), &v, &len, NULL, 0) == -1)
		return (-1);

	/* ipForwarding: forwarding(1), notForwarding(2) */
	*elm = ber_add_integer(*elm, (v == 0) ? 2 : 1);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

	return (0);
}
#endif /*IPv6_Genua*/

int
mib_ipdefaultttl(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	int	mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_DEFTTL };
	int	v;
	size_t	len = sizeof(v);

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (sysctl(mib, sizeofa(mib), &v, &len, NULL, 0) == -1)
		return (-1);

	*elm = ber_add_integer(*elm, v);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

	return (0);
}

#ifdef IPv6_Genua
int
obsd_snmpd_ipv6defaulthoplimit(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	int     mib[] = { CTL_NET, PF_INET6, IPPROTO_IPV6, IPV6CTL_DEFHLIM };
	int	v;
	size_t	len = sizeof(v);

	if (sysctl(mib, sizeofa(mib), &v, &len, NULL, 0) == -1)
		return (-1);

	*elm = ber_add_integer(*elm, v);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

	return (0);
}

static int
mib_getip6stat(struct ip6stat *ip6stat)
{
	int	 mib[] = { CTL_NET, PF_INET6, IPPROTO_IPV6, IPV6CTL_STATS };
	size_t	 len = sizeof(*ip6stat);

	return (sysctl(mib, sizeofa(mib), ip6stat, &len, NULL, 0));
}
#endif /*IPv6_Genua*/

int
mib_getipstat(struct ipstat *ipstat)
{
	int	 mib[] = { CTL_NET, PF_INET, IPPROTO_IP, IPCTL_STATS };
	size_t	 len = sizeof(*ipstat);

	return (sysctl(mib, sizeofa(mib), ipstat, &len, NULL, 0));
}

int
mib_ipstat(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ipstat		 ipstat;
	long long		 i;
	struct statsmap {
		u_int8_t	 m_id;
		u_long		*m_ptr;
	};
	
	struct statsmap mapping[10];
	bzero(mapping, sizeof(mapping));

	mapping[0].m_id =  3;   mapping[0].m_ptr = &ipstat.ips_total;
	mapping[1].m_id =  7;   mapping[1].m_ptr = &ipstat.ips_noproto;
	mapping[2].m_id =  9;   mapping[2].m_ptr = &ipstat.ips_delivered;
	mapping[3].m_id = 10;   mapping[3].m_ptr = &ipstat.ips_localout;
	mapping[4].m_id = 11;   mapping[4].m_ptr = &ipstat.ips_odropped;
	mapping[5].m_id = 12;   mapping[5].m_ptr = &ipstat.ips_noroute;
	mapping[6].m_id = 14;   mapping[6].m_ptr = &ipstat.ips_fragments;
	mapping[7].m_id = 15;   mapping[7].m_ptr = &ipstat.ips_reassembled;
	mapping[8].m_id = 17;   mapping[8].m_ptr = &ipstat.ips_fragmented;
	mapping[9].m_id = 19;   mapping[9].m_ptr = &ipstat.ips_ofragments;

	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	for (i = 0;
	    (u_int)i < (sizeof(mapping) / sizeof(mapping[0])); i++) {
		if (oid->o_oid[OIDIDX_ip] == mapping[i].m_id) {
			*elm = ber_add_integer(*elm, *mapping[i].m_ptr);
			if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			return (0);
		}
	}

	return (-1);
}

int
mib_ipinhdrerrs(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	errors;
	struct ipstat	ipstat;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	errors = ipstat.ips_badsum + ipstat.ips_badvers +
	    ipstat.ips_tooshort + ipstat.ips_toosmall +
	    ipstat.ips_badhlen +  ipstat.ips_badlen +
	    ipstat.ips_badoptions + ipstat.ips_toolong +
	    ipstat.ips_badaddr;

	*elm = ber_add_integer(*elm, errors);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_ipinaddrerrs(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	errors;
	struct ipstat	ipstat;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	errors = ipstat.ips_cantforward + ipstat.ips_badaddr;

	*elm = ber_add_integer(*elm, errors);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_ipforwdgrams(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	counter;
	struct ipstat	ipstat;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	counter = ipstat.ips_forward + ipstat.ips_redirectsent;

	*elm = ber_add_integer(*elm, counter);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_ipindiscards(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	counter;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

#if       0 /* code if we want to get an real counter value from the kernel */
	struct ipstat	ipstat;

	if (mib_getipstat(&ipstat) == -1)
		return (-1);
	counter = ipstat.ips_ipInDiscards;
#endif /* 0 */

#if       0 /* code if we want to use another existing counter, here ips_fragdropped like done in NetBSD */
	counter = ipstat.ips_fragdropped;
#endif /* 0 */

#if       1 /* the interniche way: have a counter but don't count anything */
	counter = 0;
#endif /* 1 */

	*elm = ber_add_integer(*elm, counter);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_ipreasmfails(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	counter;
	struct ipstat	ipstat;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	counter = ipstat.ips_fragdropped + ipstat.ips_fragtimeout;

	*elm = ber_add_integer(*elm, counter);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_ipfragfails(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	u_int32_t	counter;
	struct ipstat	ipstat;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

	counter = ipstat.ips_badfrags + ipstat.ips_cantfrag;
	*elm = ber_add_integer(*elm, counter);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

int
mib_iproutingdiscards(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	u_int32_t	counter;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	counter = 0; /* no value devlivered, 0 is required by SPH "SNMP in Simatic" */

	*elm = ber_add_integer(*elm, counter);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);

	return (0);
}

struct ber_oid *
mib_ipaddrtable(struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	struct sockaddr_in	 addr;
	u_int32_t		 col, id;
	struct oid		 a, b;
	struct kif_addr		*ka;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	mps_decodeinaddr(no, &addr.sin_addr, OIDIDX_ipAddr + 1);
	if (o->bo_n <= (OIDIDX_ipAddr + 1))
		ka = kr_getaddr(NULL);
	else
		ka = kr_getnextaddr((struct sockaddr *)&addr);
	if (ka == NULL || ka->addr.sa.sa_family != AF_INET) {
		/*
		 * Encode invalid "last address" marker which will tell
		 * mib_ipaddr() to fail and the SNMP engine to find the
		 * next OID.
		 */
		mps_encodeinaddr(no, NULL, OIDIDX_ipAddr + 1);
	} else {
		/* Encode real IPv4 address */
		addr.sin_addr.s_addr = ka->addr.sin.sin_addr.s_addr;
		mps_encodeinaddr(no, &addr.sin_addr, OIDIDX_ipAddr + 1);
	}
	smi_oidlen(o);

	return (no);
}

int
mib_ipaddr(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct sockaddr_in	 addr;
	struct ber_element	*ber = *elm;
	struct kif_addr		*ka;
	u_int32_t		 val;

	OBSD_UNUSED_ARG(oid);  /* gh2289n: avoid compiler warning */

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	if (mps_decodeinaddr(o, &addr.sin_addr, OIDIDX_ipAddr + 1) == -1) {
		/* Strip invalid address and fail */
		o->bo_n = OIDIDX_ipAddr + 1;
		return (1);
	}
	ka = kr_getaddr((struct sockaddr *)&addr);
	if (ka == NULL || ka->addr.sa.sa_family != AF_INET)
		return (1);

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipAddr]) {
		case 1:
			val = addr.sin_addr.s_addr;
			ber = ber_add_nstring(ber, (char *)&val, sizeof(u_int32_t));
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_IPADDR);
			break;
		case 2:
			ber = ber_add_integer(ber, ka->if_index);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:
			val = ka->mask.sin.sin_addr.s_addr;
			ber = ber_add_nstring(ber, (char *)&val, sizeof(u_int32_t));
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_IPADDR);
			break;
		case 4:
			ber = ber_add_integer(ber, ka->dstbrd.sa.sa_len ? 1 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 5:
			ber = ber_add_integer(ber, IP_MAXPACKET);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

#ifdef IPv6_Genua
struct ber_oid *
obsd_snmpd_ipaddresstable(struct oid *oid, struct ber_oid *o,
    struct ber_oid *no)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	u_int32_t		 col, id;
	struct oid		 a, b;
	struct kif_addr		*ka;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	if (o->bo_id[OIDIDX_ipAddressAF] == 1)
		obsd_snmpd_mps_decodeinaddr(no, &addr.sin_addr, OIDIDX_ipAddressEntry);
	else if (o->bo_id[OIDIDX_ipAddressAF] == 2)
		obsd_snmpd_mps_decodeinaddr6(no, &addr6.sin6_addr,
		    OIDIDX_ipAddressEntry);

	if (o->bo_n <= (OIDIDX_ipAddressEntry))
		ka = kr_getaddr(NULL);
	else {
		if (o->bo_id[OIDIDX_ipAddressAF] == 1)
			ka = kr_getnextaddr((struct sockaddr *)&addr);
		else if (o->bo_id[OIDIDX_ipAddressAF] == 2)
			ka = kr_getnextaddr((struct sockaddr *)&addr6);
		else
			ka = NULL;
	}

	if (ka == NULL) {
		/*
		 * Encode invalid "last address" marker which will tell
		 * mib_ipaddr() to fail and the SNMP engine to find the
		 * next OID.
		 */

		no->bo_id[OIDIDX_ipAddressAF] = 99;
		mps_encodeinaddr(no, NULL, OIDIDX_ipAddressEntry);
	} else {
		if (ka->addr.sa.sa_family == AF_INET) {
			no->bo_id[OIDIDX_ipAddressAF] = 1;

			/* Encode real IPv4 address */
			addr.sin_addr.s_addr = ka->addr.sin.sin_addr.s_addr;
			mps_encodeinaddr(no, &addr.sin_addr, OIDIDX_ipAddressEntry);
		}
		if (ka->addr.sa.sa_family == AF_INET6) {
			no->bo_id[OIDIDX_ipAddressAF] = 2;

			/* Encode real IPv6 address */
			memcpy(&addr6.sin6_addr, &ka->addr.sin6.sin6_addr,
			    sizeof(addr6.sin6_addr));
			obsd_snmpd_mps_encodeinaddr6(no, &addr6.sin6_addr,
			    OIDIDX_ipAddressEntry);
		}
	}
	smi_oidlen(o);

	return (no);
}

int
obsd_snmpd_ipaddress(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	struct ber_element	*ber = *elm;
	struct kif_addr		*ka = NULL;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);

	if (o->bo_id[OIDIDX_ipAddressAF] == 99) {
		o->bo_n = OIDIDX_ipAddressAF;
		return 1;
	}

	if (o->bo_id[OIDIDX_ipAddressAF] == 1) {
		if (obsd_snmpd_mps_decodeinaddr(o, &addr.sin_addr, OIDIDX_ipAddressEntry) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressEntry;
			return (1);
		}
		ka = kr_getaddr((struct sockaddr *)&addr);
	}

	if (o->bo_id[OIDIDX_ipAddressAF] == 2) {
		if (obsd_snmpd_mps_decodeinaddr6(o, &addr6.sin6_addr,
		    OIDIDX_ipAddressEntry) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressEntry;
			return (1);
		}
		ka = kr_getaddr((struct sockaddr *)&addr6);
	}

	if (ka == NULL) {
		return (1);
	}

	/* write OID */
	ber = ber_add_oid(ber, o);
	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipAddress]) {
		case 1:	/* ipAddressAddrType : InetAddressType : ipv4(1), ipv6(2) */
		case 2: /* ipAddressAddr : InetAddress */
			return (-1);
		case 3: /* ipAddressIfIndex : InterfaceIndex */
			ber = ber_add_integer(ber, ka->if_index);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 4: /* ipAddressType : INTEGER: uni(1), any(2), broad(3) */
			ber = ber_add_integer(ber, 1);	/* XXX: hard coded */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 5: /* ipAddressPrefix : RowPointer to ipAddressPrefixTable */
			/*
			 * INDEX: { ipAddressPrefixIfIndex, ipAddressPrefixType,
			 *          ipAddressPrefixPrefix,  ipAddressPrefixLength }
			 */
#if 0
		{
			struct ber_oid PrefixRow = { { 0 }, 1 };
			PrefixRow.bo_id[PrefixRow.bo_n++] = ka->if_index;
			PrefixRow.bo_id[PrefixRow.bo_n++] = o->bo_id[OIDIDX_ipAddressAF];

			if (o->bo_id[OIDIDX_ipAddressAF] == 1) {
				size_t len = obsd_snmpd_mask2prefixlen(
					ka->mask.sin.sin_addr.s_addr);
				obsd_snmpd_mps_encodeinprefix(&PrefixRow,
					&ka->addr.sin.sin_addr, PrefixRow.bo_n, len);
			}
			else {
				size_t len = obsd_snmpd_mask2prefixlen6(&ka->mask.sin6);
				obsd_snmpd_mps_encodeinprefix6(&PrefixRow,
					&ka->addr.sin6.sin6_addr, PrefixRow.bo_n, len);
			}
			ber = ber_add_oid(ber, &PrefixRow);
		}
#else
			ber = ber_add_oid(ber, &zerodotzero);
#endif
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 6: /* ipAddressOrigin      : IpAddressOriginTC */
			/* other(1), manual(2), dhcp(4), linklayer(5), random(6) */
			ber = ber_add_integer(ber, 1);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 7: /* ipAddressStatus      : IpAddressStatusTC ??? */
			/* preferred(1), deprecated(2), invalid(3), inaccessible(4),
			 * unknown(5), tentative(6), duplicate(7), optimistic(8) */
			ber = ber_add_integer(ber, 1);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 8: /* ipAddressCreated     : TimeStamp */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		case 9: /* ipAddressLastChanged : TimeStamp */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		case 10: /* ipAddressRowStatus : RowStatus active(1), notInService(2) */
			ber = ber_add_integer(ber, 1);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 11: /* ipAddressStorageType  : StorageType */
			/* other(1), volatile(2), nonVolatile(3), permanent(4),
			 * readOnly(5) */
			ber = ber_add_integer(ber, 4);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

struct ber_oid *
obsd_snmpd_ipsystemstatstable(struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	u_int32_t		 col, id;
	struct oid		 a, b;

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	if (o->bo_n < 12) {
		if (o->bo_n <  9) no->bo_id[no->bo_n++] = 1;
		if (o->bo_n < 10) no->bo_id[no->bo_n++] = 1;
		if (o->bo_n < 11) no->bo_id[no->bo_n++] = 3;
		if (o->bo_n < 12) no->bo_id[no->bo_n++] = 1;

		if (no->bo_id[ 8] < 1) no->bo_id[ 8] = 1;
		if (no->bo_id[ 9] < 1) no->bo_id[ 9] = 1;
		if (no->bo_id[10] < 1) no->bo_id[10] = 3;
		if (no->bo_id[11] < 1) no->bo_id[11] = 1;

	} else if (o->bo_id[OIDIDX_ipSystemStatsType] == 1) {
		no->bo_id[OIDIDX_ipSystemStatsType] = 2;
		if (no->bo_id[OIDIDX_ipSystemStatsType - 1] < 3)
			no->bo_id[OIDIDX_ipSystemStatsType - 1] = 3;
	} else {
		no->bo_id[OIDIDX_ipSystemStatsType] = 1;
		if (no->bo_id[OIDIDX_ipSystemStatsType - 1] < 3)
			no->bo_id[OIDIDX_ipSystemStatsType - 1] = 3;
		else
			no->bo_id[OIDIDX_ipSystemStatsType - 1]++;
	}

	smi_oidlen(o);

	return (no);
}

int
obsd_snmpd_ipsystemstats(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	struct ipstat		 ipstat;
	struct ip6stat		 ip6stat;
	struct ber_element	*ber = *elm;

	int typ = o->bo_id[OIDIDX_ipSystemStatsType];
	int col = o->bo_id[OIDIDX_ipSystemStatsEntry];

	if (o->bo_id[OIDIDX_ipTrafficStats] == 0 || o->bo_id[9] == 0 ||
	    col < 3 || col > 47) {
		return 1;
	}

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

#ifdef IPv6
	if (mib_getip6stat(&ip6stat) == -1)
		return (-1);
#else
	memset(&ip6stat, 0, sizeof(ip6stat));
#endif

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (col) {
		case 1: /* InetVersion: ipv4(1), ipv6(2) */
		case 2: /*
			 * This object ID is reserved to allow the IDs for this table's
			 * objects to align with the objects in the ipIfStatsTable.
			 */
			return (-1);
		case 3: /* ipSystemStatsInReceives */
		case 4: /* ipSystemStatsHCInReceives */
			ber = ber_add_integer(ber, typ == 1 ? ipstat.ips_total : ip6stat.ip6s_total);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 3 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 5: /* ipSystemStatsInOctets */
		case 6: /* ipSystemStatsHCInOctets */
			/* XXX */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 5 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 7: /* ipSystemStatsInHdrErrors */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_badsum + ipstat.ips_badvers +
				ipstat.ips_tooshort + ipstat.ips_toosmall +
				ipstat.ips_badhlen + ipstat.ips_badlen +
				ipstat.ips_badoptions + ipstat.ips_toolong +
				ipstat.ips_badaddr :
				ip6stat.ip6s_badvers + ip6stat.ip6s_tooshort +
				ip6stat.ip6s_toosmall + ip6stat.ip6s_badoptions +
				ip6stat.ip6s_toomanyhdr);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 8: /* ipSystemStatsInNoRoutes */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_noroute : ip6stat.ip6s_noroute);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 9: /* ipSystemStatsInAddrErrors */
			/* IPv6 value is addopted from net-snmp */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_badaddr : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 10: /* ipSystemStatsInUnknownProtos */
			ber = ber_add_integer(ber, typ == 1 ? ipstat.ips_noproto : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 11: /* ipSystemStatsInTruncatedPkts */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_toosmall : ip6stat.ip6s_toosmall);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 12: /* ipSystemStatsInForwDatagrams */
		case 13: /* ipSystemStatsHCInForwDatagrams */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_forward : ip6stat.ip6s_forward);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 12 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 14: /* ipSystemStatsReasmReqds */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_fragments : ip6stat.ip6s_fragments);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 15: /* ipSystemStatsReasmOKs  */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_reassembled : ip6stat.ip6s_reassembled);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 16: /* ipSystemStatsReasmFails */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_fragdropped + ipstat.ips_fragtimeout :
				ip6stat.ip6s_fragdropped + ip6stat.ip6s_fragtimeout);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 17: /* ipSystemStatsInDiscards */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_fragdropped : ip6stat.ip6s_fragdropped);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 18: /* ipSystemStats  InDelivers */
		case 19: /* ipSystemStatsHCInDelivers */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_delivered : ip6stat.ip6s_delivered);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 18 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 20: /* ipSystemStats  OutRequests */
		case 21: /* ipSystemStatsHCOutRequests */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_localout : ip6stat.ip6s_localout);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 20 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 22: /* ipSystemStatsOutNoRoutes */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_noroute : ip6stat.ip6s_noroute);

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 23: /* ipSystemStats  OutForwDatagrams*/
		case 24: /* ipSystemStatsHCOutForwDatagrams*/
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_forward : ip6stat.ip6s_forward);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 23 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 25: /* ipSystemStatsOutDiscards */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_odropped : ip6stat.ip6s_odropped);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 26: /* ipSystemStatsOutFragReqds */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_fragmented + ipstat.ips_cantfrag :
				ip6stat.ip6s_fragmented + ip6stat.ip6s_cantfrag);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 27: /* ipSystemStatsOutFragOKs */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_fragmented : ip6stat.ip6s_fragmented);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 28: /* ipSystemStatsOutFragFails */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_badfrags + ipstat.ips_cantfrag :
				ip6stat.ip6s_cantfrag);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 29: /* ipSystemStatsOutFragCreates */
			ber = ber_add_integer(ber,
				typ == 1 ? ipstat.ips_ofragments : ip6stat.ip6s_ofragments);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 30: /* ipSystemStats  OutTransmits */
		case 31: /* ipSystemStatsHCOutTransmits */
			/*
			 * OutTransmits = OutRequests + OutForwDatagrams +
			 *                OutFragCreates - OutFragReqds - OutNoRoutes -
			 *		  OutDiscards
			 */
			ber = ber_add_integer(ber, typ == 1 ?
				ipstat.ips_localout + ipstat.ips_forward +
				ipstat.ips_ofragments - (ipstat.ips_fragmented +
					ipstat.ips_cantfrag) - ipstat.ips_noroute -
				ipstat.ips_odropped :
				ip6stat.ip6s_localout + ip6stat.ip6s_forward +
				ip6stat.ip6s_ofragments - (ip6stat.ip6s_fragmented +
					ip6stat.ip6s_cantfrag) - ip6stat.ip6s_noroute -
				ip6stat.ip6s_odropped);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */

			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 30 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 32: /* ipSystemStats  OutOctets */
		case 33: /* ipSystemStatsHCOutOctets */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 32 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 34: /* ipSystemStats  InMcastPkts */
		case 35: /* ipSystemStatsHCInMcastPkts */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 34 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 36: /* ipSystemStats  InMcastOctets */
		case 37: /* ipSystemStatsHCInMcastOctets */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 36 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 38: /* ipSystemStats  OutMcastPkts */
		case 39: /* ipSystemStatsHCOutMcastPkts */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 38 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 40: /* ipSystemStats  OutMcastOctets */
		case 41: /* ipSystemStatsHCOutMcastOctets */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 40 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 42: /* ipSystemStats  InBcastPkts */
		case 43: /* ipSystemStatsHCInBcastPkts */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 42 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 44: /* ipSystemStats  OutBcastPkts */
		case 45: /* ipSystemStatsHCOutBcastPkts */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 44 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 46: /* ipSystemStatsDiscontinuityTime */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		case 47: /* ipSystemStatsRefreshRate */
			/* README: event based */
			ber = ber_add_integer(ber, typ == 1 ? 0 : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

struct ber_oid *
obsd_snmpd_ipifstatstable(struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	u_int32_t		 col, id;
	struct oid		 a, b;

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	if (o->bo_n < 12) {
		if (o->bo_n <  9) no->bo_id[no->bo_n++] = 3;
		if (o->bo_n < 10) no->bo_id[no->bo_n++] = 1;
		if (o->bo_n < 11) no->bo_id[no->bo_n++] = 3;
		if (o->bo_n < 12) no->bo_id[no->bo_n++] = 1;

		if (no->bo_id[ 8] < 1) no->bo_id[ 8] = 3;
		if (no->bo_id[ 9] < 1) no->bo_id[ 9] = 1;
		if (no->bo_id[10] < 1) no->bo_id[10] = 3;
		if (no->bo_id[11] < 1) no->bo_id[11] = 1;
	} else if (o->bo_id[OIDIDX_ipIfStatsType] == 1) {
		no->bo_id[OIDIDX_ipIfStatsType] = 2;
		if (no->bo_id[OIDIDX_ipIfStatsType - 1] < 3)
			no->bo_id[OIDIDX_ipIfStatsType - 1] = 3;
	} else {
		no->bo_id[OIDIDX_ipIfStatsType] = 1;
		if (no->bo_id[OIDIDX_ipIfStatsType - 1] < 3)
			no->bo_id[OIDIDX_ipIfStatsType - 1] = 3;
		else
			no->bo_id[OIDIDX_ipIfStatsType - 1]++;
	}

	smi_oidlen(o);

	return (no);
}

int
obsd_snmpd_ipifstats(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	struct ipstat		 ipstat;
	struct ip6stat		 ip6stat;
	struct ber_element	*ber = *elm;

	int col = o->bo_id[OIDIDX_ipIfStatsEntry];

	if (o->bo_id[OIDIDX_ipTrafficStats] == 0 || col < 3 || col > 47) {
		o->bo_n = OIDIDX_ipIfStatsEntry + 1;
		return 1;
	}

	if (mib_getipstat(&ipstat) == -1)
		return (-1);

#ifdef IPv6
	if (mib_getip6stat(&ip6stat) == -1)
		return (-1);
#else
	memset(&ip6stat, 0, sizeof(ip6stat));
#endif

	/* write OID */
	ber = ber_add_oid(ber, o);
	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (col) {
		case 1: /* InetVersion: ipv4(1), ipv6(2) */
		case 2: /*
			 * This object ID is reserved to allow the IDs for this table's
			 * objects to align with the objects in the ipIfStatsTable.
			 */
			return (-1);
		case 3: /* ipSystemStatsInReceives */
		case 4: /* ipSystemStatsHCInReceives */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 3 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 5: /* ipSystemStatsInOctets */
		case 6: /* ipSystemStatsHCInOctets */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 5 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 7: /* ipSystemStatsInHdrErrors */
		case 8: /* ipSystemStatsInNoRoutes */
		case 9: /* ipSystemStatsInAddrErrors */
		case 10: /* ipSystemStatsInUnknownProtos */
		case 11: /* ipSystemStatsInTruncatedPkts */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 12: /* ipSystemStatsInForwDatagrams */
		case 13: /* ipSystemStatsHCInForwDatagrams */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 12 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 14: /* ipSystemStatsReasmReqds */
		case 15: /* ipSystemStatsReasmOKs  */
		case 16: /* ipSystemStatsReasmFails */
		case 17: /* ipSystemStatsInDiscards */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 18: /* ipSystemStats  InDelivers */
		case 19: /* ipSystemStatsHCInDelivers */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 18 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 20: /* ipSystemStats  OutRequests */
		case 21: /* ipSystemStatsHCOutRequests */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 20 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 22: /* ipSystemStatsOutNoRoutes */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 23: /* ipSystemStats  OutForwDatagrams*/
		case 24: /* ipSystemStatsHCOutForwDatagrams*/
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 23 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 25: /* ipSystemStatsOutDiscards */
		case 26: /* ipSystemStatsOutFragReqds */
		case 27: /* ipSystemStatsOutFragOKs */
		case 28: /* ipSystemStatsOutFragFails */
		case 29: /* ipSystemStatsOutFragCreates */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_COUNTER32);
			break;
		case 30: /* ipSystemStats  OutTransmits */
		case 31: /* ipSystemStatsHCOutTransmits */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 30 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 32: /* ipSystemStats  OutOctets */
		case 33: /* ipSystemStatsHCOutOctets */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 32 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 34: /* ipSystemStats  InMcastPkts */
		case 35: /* ipSystemStatsHCInMcastPkts */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 34 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 36: /* ipSystemStats  InMcastOctets */
		case 37: /* ipSystemStatsHCInMcastOctets */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 36 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 38: /* ipSystemStats  OutMcastPkts */
		case 39: /* ipSystemStatsHCOutMcastPkts */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 38 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 40: /* ipSystemStats  OutMcastOctets */
		case 41: /* ipSystemStatsHCOutMcastOctets */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 40 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 42: /* ipSystemStats  InBcastPkts */
		case 43: /* ipSystemStatsHCInBcastPkts */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 42 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 44: /* ipSystemStats  OutBcastPkts */
		case 45: /* ipSystemStatsHCOutBcastPkts */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION,
				col == 44 ? SNMP_T_COUNTER32 : SNMP_T_COUNTER64);
			break;
		case 46: /* ipSystemStatsDiscontinuityTime */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_TIMETICKS);
			break;
		case 47: /* ipSystemStatsRefreshRate */
			/* README: event based */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}
static int
get_v6prefix(struct kif_addr *ka, struct in6_prefix *prefix)
{
	int mib[] = { CTL_NET, PF_INET6, IPPROTO_ICMPV6, ICMPV6CTL_ND6_PRLIST };
	char *buf, *p, *ep;
	struct in6_prefix pfx;
	size_t l;

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), NULL, &l, NULL, 0) < 0)
		return (-1);

	if (l == 0)
		return (-1);

	if ((buf = malloc(l)) == NULL)
		return (-1);

	if (sysctl(mib, sizeof(mib) / sizeof(mib[0]), buf, &l, NULL, 0) < 0)
		goto err;

	ep = buf + l;
	for (p = buf; p < ep; ) {
		memcpy(&pfx, p, sizeof(pfx));
		p += sizeof(pfx);

		if (pfx.if_index != ka->if_index)
			continue;

		if (obsd_snmpd_cmp_prefix6(&pfx.prefix, &ka->addr.sin6,
		    &ka->mask.sin6)) {
			memcpy(prefix, &pfx, sizeof *prefix);
			free(buf);
			return (0);
		}
	}

 err:
	free(buf);
	return (-1);
}

struct ber_oid *
obsd_snmpd_ipaddressprefixtable(struct oid *oid, struct ber_oid *o,
    struct ber_oid *no)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	u_int32_t		 col, id;
	struct oid		 a, b;
	struct kif_addr		*ka;
	uint8_t			 if_index;
	uint8_t			 len;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	if_index = o->bo_id[OIDIDX_ipAddressPrefixIfIndex];

	if (o->bo_id[OIDIDX_ipAddressPrefixType] == 1)
		obsd_snmpd_mps_decodeinprefix(no, &addr.sin_addr,
		    OIDIDX_ipAddressPrefixPrefix);
	else if (o->bo_id[OIDIDX_ipAddressPrefixType] == 2)
		obsd_snmpd_mps_decodeinprefix6(no, &addr6.sin6_addr,
		    OIDIDX_ipAddressPrefixPrefix);

	if (o->bo_n <= (OIDIDX_ipAddressPrefixPrefix)) {
		if_index = 1;
		ka = obsd_snmpd_kr_getnextprefix(if_index, NULL, 0);
	} else {
		if (o->bo_id[OIDIDX_ipAddressPrefixType] == 1)
			ka = obsd_snmpd_kr_getnextprefix(if_index,
			    (struct sockaddr *)&addr, 0);
		else if (o->bo_id[OIDIDX_ipAddressPrefixType] == 2)
			ka = obsd_snmpd_kr_getnextprefix(if_index,
			    (struct sockaddr *)&addr6, 0);
		else
			ka = NULL;
	}

	if (ka == NULL) {
		/*
		 * Encode invalid "last address" marker which will tell
		 * mib_ipaddr() to fail and the SNMP engine to find the
		 * next OID.
		 */

		no->bo_id[OIDIDX_ipAddressPrefixType] = 99;
		no->bo_id[OIDIDX_ipAddressPrefixIfIndex] = 1;
		obsd_snmpd_mps_encodeinprefix(no, NULL,
		    OIDIDX_ipAddressPrefixPrefix, 0);
	} else {
		no->bo_id[OIDIDX_ipAddressPrefixIfIndex] = ka->if_index;
		no->bo_n = OIDIDX_ipAddressPrefixIfIndex + 1;

		if (ka->addr.sa.sa_family == AF_INET) {
			len = obsd_snmpd_mask2prefixlen(
			    ka->mask.sin.sin_addr.s_addr);
			no->bo_id[OIDIDX_ipAddressPrefixType] = 1;

			/* Encode real IPv4 mask */
			addr.sin_addr.s_addr = ka->addr.sin.sin_addr.s_addr;
			obsd_snmpd_mps_encodeinprefix(no, &addr.sin_addr,
			    OIDIDX_ipAddressPrefixPrefix, len);
		}

		if (ka->addr.sa.sa_family == AF_INET6) {
			len = obsd_snmpd_mask2prefixlen6(&ka->mask.sin6);
			no->bo_id[OIDIDX_ipAddressPrefixType] = 2;

			/* Encode real IPv6 mask */
			memcpy(&addr6.sin6_addr, &ka->addr.sin6.sin6_addr,
			    sizeof(addr6.sin6_addr));
			obsd_snmpd_mps_encodeinprefix6(no, &addr6.sin6_addr,
			    OIDIDX_ipAddressPrefixPrefix, len);
		}
	}
	smi_oidlen(o);

	return (no);
}

int
obsd_snmpd_ipaddressprefix(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	struct ber_element	*ber = *elm;
	struct kif_addr		*ka = NULL;
	u_int32_t		 val;
	struct kif		*kif;
	struct in6_prefix	prefix;
	int got_prefix = 0;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);

	if (o->bo_id[OIDIDX_ipAddressPrefixType] == 99) {
		o->bo_n = OIDIDX_ipAddressPrefixType;
		return 1;
	}

	if (o->bo_id[OIDIDX_ipAddressPrefixType] == 1) {
		if (obsd_snmpd_mps_decodeinprefix(o, &addr.sin_addr,
		    OIDIDX_ipAddressPrefixPrefix) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressEntry;
			return (1);
		}

		uint8_t if_index = o->bo_id[OIDIDX_ipAddressPrefixIfIndex];
		uint8_t len      = o->bo_id[o->bo_n - 1];
		ka = obsd_snmpd_kr_getprefix(if_index, (struct sockaddr *)&addr,
		    len);
	}

	if (o->bo_id[OIDIDX_ipAddressPrefixType] == 2) {
		if (obsd_snmpd_mps_decodeinprefix6(o, &addr6.sin6_addr,
		    OIDIDX_ipAddressPrefixPrefix) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressPrefixEntry;
			return (1);
		}

		uint8_t if_index = o->bo_id[OIDIDX_ipAddressPrefixIfIndex];
		uint8_t len      = o->bo_id[o->bo_n - 1];
		ka = obsd_snmpd_kr_getprefix(if_index,
		    (struct sockaddr *)&addr6, len);
		if (get_v6prefix(ka, &prefix) == 0)
			got_prefix = 1;
	}

	if (ka == NULL)
		return (1);

	if ((kif = mib_ifget(ka->if_index)) == NULL)
		return (1);

	/* write OID */
	ber = ber_add_oid(ber, o);
	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipAddressPrefixEntry]) {
		case 1:	/* ipAddressPrefixIfIndex */
		case 2: /* ipAddressPrefixType : InetAddressType */
		case 3: /* ipAddressPrefixPrefix : InetAddress */
		case 4: /* ipAddressPrefixLength : InetAddressPrefixLength */
			return (-1);
		case 5: /* ipAddressPrefixOrigin : IpAddressPrefixOriginTC */
			/* other(1), manual(2), wellknown(3), dhcp(4), routeradv(5) */
			if (got_prefix && prefix.origin == PR_ORIG_RA) {
				ber = ber_add_integer(ber, 5);
			}
			else {
				ber = ber_add_integer(ber, 1);
			}
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 6: /* ipAddressPrefixOnLinkFlag : TruthValue */
			/* true(1), false(2) */
			if (ka->addr.sa.sa_family == AF_INET) {
				ber = ber_add_integer(ber, 1);
			}
			else {
				if (got_prefix && prefix.raflags.onlink)
					ber = ber_add_integer(ber, 1);
				else
					ber = ber_add_integer(ber, 2);
			}
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 7: /* ipAddressPrefixAutonomousFlag : TruthValue */
			/* true(1), false(2) */
			if (got_prefix && prefix.raflags.autonomous)
				ber = ber_add_integer(ber, 1);
			else
				ber = ber_add_integer(ber, 2);
			break;
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
		case 8: /* ipAddressPrefixAdvPreferredLifetime : Unsigned32 */
			/* infinity(4,294,967,295) */

			val = 4294967295U;

			if (got_prefix)
				val = prefix.expire - prefix.vltime + prefix.pltime -
				time(NULL);

			ber = ber_add_integer(ber, val);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 9: /* ipAddressPrefixAdvValidLifetime : Unsigned32 */
			/* infinity(4,294,967,295) */

			val = 4294967295U;

			if (got_prefix)
				val = prefix.expire - time(NULL);

			ber = ber_add_integer(ber, val);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

struct ber_oid *
obsd_snmpd_ipdefaultroutertable(struct oid *oid, struct ber_oid *o,
    struct ber_oid *no)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	u_int32_t		 col, id;
	struct oid		 a, b;
	struct kroute		*kn = NULL;
	struct in6_defrouter	 dr6;
	uint8_t			 if_index;
	int			 got_dr6 = 0;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 1) {
		obsd_snmpd_mps_decodeinaddr(no, &addr.sin_addr,
		    OIDIDX_ipDefaultRouterAddress);
		if_index = o->bo_id[OIDIDX_ipDefaultRouterAddress + 4];
	} else if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 2) {
		obsd_snmpd_mps_decodeinaddr6(no, &addr6.sin6_addr,
		    OIDIDX_ipDefaultRouterAddress);
		if_index = o->bo_id[OIDIDX_ipDefaultRouterAddress + 16];
	}

	if (o->bo_n <= (OIDIDX_ipDefaultRouterAddressType)) {
		kn = obsd_snmpd_kr_getdefaultrouter(NULL);
	} else {
		if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 1) {
			kn = obsd_snmpd_kr_getnextdefaultrouter(&addr);
			if (kn == NULL) {
				if (obsd_snmpd_kr_getdefaultrouter6(NULL, &dr6)
				    == 0)
					got_dr6 = 1;
			}
		} else if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 2 &&
		    obsd_snmpd_kr_getnextdefaultrouter6(&addr6.sin6_addr, &dr6)
		    == 0) {
			got_dr6 = 1;
		}
	}

	if (kn == NULL && got_dr6 == 0) {
		/*
		 * Encode invalid "last address" marker which will tells the
		 * value funtion below to fail and the SNMP engine to find the
		 * next OID.
		 */

		no->bo_id[OIDIDX_ipDefaultRouterAddressType] = 99;
		mps_encodeinaddr(no, NULL, OIDIDX_ipDefaultRouterAddress);
	} else {
		if (kn != NULL) {
			no->bo_id[OIDIDX_ipDefaultRouterAddressType] = 1;

			/* Encode IPv4 router address */
			addr.sin_addr.s_addr = kn->nexthop.s_addr;
			mps_encodeinaddr(no, &addr.sin_addr,
			    OIDIDX_ipDefaultRouterAddress);

			no->bo_id[no->bo_n++] = kn->if_index;
		}

		if (got_dr6) {
			no->bo_id[OIDIDX_ipDefaultRouterAddressType] = 2;

			/* Encode IPv6 router address */
			memcpy(&addr6.sin6_addr, &dr6.rtaddr.sin6_addr,
			    sizeof(addr6.sin6_addr));
			obsd_snmpd_mps_encodeinaddr6(no, &addr6.sin6_addr,
			    OIDIDX_ipDefaultRouterAddress);

			no->bo_id[no->bo_n++] = dr6.if_index;
		}
	}
	smi_oidlen(o);

	return (no);
}

int
obsd_snmpd_ipdefaultrouter(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	struct sockaddr_in	 addr;
	struct sockaddr_in6	 addr6;
	struct ber_element	*ber = *elm;
	struct kroute		*kn = NULL;
	struct in6_defrouter	 dr6;
	int			 got_dr6 = 0;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bzero(&addr6, sizeof(addr6));
	addr6.sin6_family = AF_INET6;
	addr6.sin6_len = sizeof(addr6);


	if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 99) {
		o->bo_n = OIDIDX_ipDefaultRouterEntry;
		return 1;
	}

	if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 1) {
		if (obsd_snmpd_mps_decodeinaddr(o, &addr.sin_addr,
		    OIDIDX_ipDefaultRouterAddress) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressEntry;
			return (1);
		}

		kn = obsd_snmpd_kr_getdefaultrouter(&addr);
	}

	if (o->bo_id[OIDIDX_ipDefaultRouterAddressType] == 2) {
		if (obsd_snmpd_mps_decodeinaddr6(o, &addr6.sin6_addr,
		    OIDIDX_ipDefaultRouterAddress) == -1) {
			/* Strip invalid address and fail */
			o->bo_n = OIDIDX_ipAddressEntry;
			return (1);
		}

		if (obsd_snmpd_kr_getdefaultrouter6(&addr6.sin6_addr, &dr6) ==0)
			got_dr6 = 1;
	}

	if (kn == NULL && got_dr6 == 0)
		return (1);

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipDefaultRouterEntry]) {
		case 1:	/* ipDefaultRouterAddressType : InetAddressType */
		case 2: /* ipDefaultRouterAddress : InetAddress */
		case 3: /* ipDefaultRouterIfIndex : InterfaceIndex */
			return (-1);
		case 4: /* ipDefaultRouterLifetime : Unsigned32 */

			if (got_dr6)
				ber = ber_add_integer(ber, dr6.expire - time(NULL));
			else
				ber = ber_add_integer(ber, 65535);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 5: /* ipDefaultRouterPreference : INTEGER */
			/* reserved(-2), low(-1), medium(0), high(1) */
			if (got_dr6) {
				switch (dr6.flags & ND_RA_FLAG_RTPREF_MASK) {
				case ND_RA_FLAG_RTPREF_HIGH:
					ber = ber_add_integer(ber, 1);
					break;
				case ND_RA_FLAG_RTPREF_MEDIUM:
				default:
					ber = ber_add_integer(ber, 0);
					break;
				case ND_RA_FLAG_RTPREF_LOW:
					ber = ber_add_integer(ber, -1);
					break;
				case ND_RA_FLAG_RTPREF_RSV:
					ber = ber_add_integer(ber, -2);
					break;
				}
			}
			else {
				ber = ber_add_integer(ber, 0);
			}
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}

int
obsd_snmpd_spinlock(struct oid *oid, struct ber_oid *o,
    struct ber_element **elm)
{
	*elm = ber_add_integer(*elm, 0);
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	return (0);
}
#endif /*IPv6_Genua*/

struct ber_oid *
mib_physaddrtable(struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	struct sockaddr_in	 addr;
	struct oid		 a, b;
	struct kif		*kif;
	struct kif_arp		*ka = NULL;
	u_int32_t		 id, idx = 0;

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (memcmp(&oid->o_id.bo_id, o->bo_id, sizeof(u_int32_t) * (OIDIDX_ipNetToMedia - 1)) != 0) /* if (smi_oid_cmp(o, &oid->o_id) < 0) */
	{
		bzero(o, sizeof(o));
		/* new entry into our ipNetToMediaTable, our predecessor is an other OID outside our ipNetToMediaTable                                             */
		/* assume that the last OID is smaller than ours, otherwise we could not distingoage between new entry in our table and repeaded entry in our table*/
		/* (because our own next table rows have OIDs that are larger that the last OID too)                                                               */ 
		bcopy(&oid->o_id.bo_id, o->bo_id, sizeof(u_int32_t) * OIDIDX_ipNetToMedia);  /* gh2289n: copy the OID from the MIB Tree to the request OID         */
		o->bo_n = oid->o_id.bo_n;
	}

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			o->bo_id[id] = oid->o_oid[id];
			bcopy(o, no, sizeof(*no));
		}
	}

	if (o->bo_n > OIDIDX_ipNetToMedia + 1)
		idx = o->bo_id[OIDIDX_ipNetToMedia + 1];

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);
	if (o->bo_n > OIDIDX_ipNetToMedia + 2)
		mps_decodeinaddr(no, &addr.sin_addr, OIDIDX_ipNetToMedia + 2);

	if ((kif = kr_getif((u_short)idx)) == NULL) {
		/* No configured interfaces */
		if (idx == 0)
			return (NULL);
		/*
		 * It may happen that an interface with a specific index
		 * does not exist or has been removed.  Jump to the next
		 * available interface.
		 */
		kif = kr_getif(0);
nextif:
		kr_updatearp(kif->if_index);
		for (; kif != NULL; kif = kr_getnextif(kif->if_index))
			if (kif->if_index > idx &&
			    (ka = karp_first(kif->if_index)) != NULL)
				break;
		if (kif == NULL) {
			/* No more interfaces with addresses on them */
			o->bo_id[OIDIDX_ipNetToMedia + 1] = 0;
			mps_encodeinaddr(no, NULL, OIDIDX_ipNetToMedia + 2);
			smi_oidlen(o);
			return (NULL);
		}
	} else {

		kr_updatearp(kif->if_index);
		if (idx == 0 || addr.sin_addr.s_addr == 0)
			ka = karp_first(kif->if_index);
		else
			ka = karp_getaddr((struct sockaddr *)&addr, (u_short)idx, 1);
		if (ka == NULL) {
			/* Try next interface */
			goto nextif;
		}
	}
	idx = kif->if_index;

	no->bo_id[OIDIDX_ipNetToMedia + 1] = idx;
	/* Encode real IPv4 address */
	if (ka != NULL)
	{
	memcpy(&addr, &ka->addr.sin, ka->addr.sin.sin_len);
	mps_encodeinaddr(no, &addr.sin_addr, OIDIDX_ipNetToMedia + 2);
	}

	smi_oidlen(o);
	return (no);
}

int
mib_physaddr(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	struct sockaddr_in	 addr;
	struct kif_arp		*ka;
	u_int32_t		 val, idx = 0;

	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */

	idx = o->bo_id[OIDIDX_ipNetToMedia + 1];
	if (idx == 0) {
		/* Strip invalid interface index and fail */
		o->bo_n = OIDIDX_ipNetToMedia + 1;
		return (1);
	}

	kr_updatearp(idx);
	/* Get the IP address */
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	if (mps_decodeinaddr(o, &addr.sin_addr,
	    OIDIDX_ipNetToMedia + 2) == -1) {
		/* Strip invalid address and fail */
		o->bo_n = OIDIDX_ipNetToMedia + 2;
		return (1);
	}
	if ((ka = karp_getaddr((struct sockaddr *)&addr, (u_short)idx, 0)) == NULL)
		return (1);

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (o->bo_id[OIDIDX_ipNetToMedia]) {
		case 1: /* ipNetToMediaIfIndex */
			ber = ber_add_integer(ber, ka->if_index);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 2: /* ipNetToMediaPhysAddress */
			if (bcmp(LLADDR(&ka->target.sdl), ether_zeroaddr,
				sizeof(ether_zeroaddr)) == 0)
				ber = ber_add_nstring(ber, (const char*)ether_zeroaddr,
					sizeof(ether_zeroaddr));
			else
				ber = ber_add_nstring(ber, LLADDR(&ka->target.sdl),
					ka->target.sdl.sdl_alen);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 3:	/* ipNetToMediaNetAddress */
			val = addr.sin_addr.s_addr;
			ber = ber_add_nstring(ber, (char *)&val, sizeof(u_int32_t));
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_IPADDR);
			break;
		case 4: /* ipNetToMediaType */
			if (ka->flags & F_STATIC)
				ber = ber_add_integer(ber, 4); /* static */
			else
				ber = ber_add_integer(ber, 3); /* dynamic */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}
	return (0);
}

#ifndef    OBSD_SNMPD_DISABLE_IP_FORWARD_MIB /* gh2289n/_KLE_: disable the complete IP-FORWARD-MIB (OID = 1.3.6.1.2.1.4.24)*/
/*
 * Defined in IP-FORWARD-MIB.txt (rfc4292)
 */

int mib_ipfnroutes(struct oid *, struct ber_oid *, struct ber_element **);
struct ber_oid *
mib_ipfroutetable(struct oid *oid, struct ber_oid *o, struct ber_oid *no);
int mib_ipfroute(struct oid *, struct ber_oid *, struct ber_element **);

static struct oid ipf_mib[] = {
	{ MIB(ipfMIB),			OID_MIB },
	{ MIB(ipfInetCidrRouteNumber),	OID_RD, mib_ipfnroutes },

	{ MIB(ipfRouteEntIfIndex),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntType),		OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntProto),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntAge),		OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntNextHopAS),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntRouteMetric1),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntRouteMetric2),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntRouteMetric3),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntRouteMetric4),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntRouteMetric5),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIB(ipfRouteEntStatus),	OID_TRD, mib_ipfroute, NULL,
	    mib_ipfroutetable },
	{ MIBEND }
};

int
mib_ipfnroutes(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	OBSD_UNUSED_ARG(oid);/* gh2289n: avoid compiler warning */
	OBSD_UNUSED_ARG(o);  /* gh2289n: avoid compiler warning */

	*elm = ber_add_integer(*elm, kr_routenumber());
	if (*elm == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
	ber_set_header(*elm, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);

	return (0);
}

struct ber_oid *
mib_ipfroutetable(struct oid *oid, struct ber_oid *o, struct ber_oid *no)
{
	u_int32_t		 col, id;
	struct oid		 a, b;
	struct sockaddr_in	 addr;
	struct kroute		*kr;
	int			 af, atype, idx;
	u_int8_t		 prefixlen;
	u_int8_t		 prio;

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	bcopy(&oid->o_id, no, sizeof(*no));
	id = oid->o_oidlen - 1;

	if (o->bo_n >= oid->o_oidlen) {
		/*
		 * Compare the requested and the matched OID to see
		 * if we have to iterate to the next element.
		 */
		bzero(&a, sizeof(a));
		bcopy(o, &a.o_id, sizeof(struct ber_oid));
		bzero(&b, sizeof(b));
		bcopy(&oid->o_id, &b.o_id, sizeof(struct ber_oid));
		b.o_oidlen--;
		b.o_flags |= OID_TABLE;
		if (smi_oid_cmp(&a, &b) == 0) {
			col = oid->o_oid[id];
			o->bo_id[id] = col;
			bcopy(o, no, sizeof(*no));
		}
	}

	af = no->bo_id[OIDIDX_ipfInetCidrRoute + 1];
	obsd_snmpd_mps_decodeinaddr(no, &addr.sin_addr, OIDIDX_ipfInetCidrRoute + 3);
	prefixlen = (u_int8_t) o->bo_id[OIDIDX_ipfInetCidrRoute + 7]; /* OBSD_ITGR -- omit warning */
	prio = (u_int8_t) o->bo_id[OIDIDX_ipfInetCidrRoute + 10]; /* OBSD_ITGR -- omit warning */

	if (af == 0)
		kr = kroute_first();
	else
		kr = kroute_getaddr(addr.sin_addr.s_addr, prefixlen, prio, 1);

	if (kr == NULL) {
		addr.sin_addr.s_addr = 0;
		prefixlen = 0;
		prio = 0;
		addr.sin_family = 0;
	} else {
		addr.sin_addr.s_addr = kr->prefix.s_addr;
		prefixlen = kr->prefixlen;
		prio = kr->priority;
	}

	switch (addr.sin_family) {
	case AF_INET:
		atype = 1;
		break;
	case AF_INET6:
		atype = 2;
		break;
	default:
		atype = 0;
		break;
	}
	idx = OIDIDX_ipfInetCidrRoute + 1;
	no->bo_id[idx++] = atype;
	no->bo_id[idx++] = 0x04;
	no->bo_n++;

	mps_encodeinaddr(no, &addr.sin_addr, idx);
	no->bo_id[no->bo_n++] = prefixlen;
	no->bo_id[no->bo_n++] = 0x02;
	no->bo_n += 2; /* policy */
	no->bo_id[OIDIDX_ipfInetCidrRoute + 10]  = prio;

	if (kr != NULL) {
		no->bo_id[no->bo_n++] = atype;
		no->bo_id[no->bo_n++] = 0x04;
		mps_encodeinaddr(no, &kr->nexthop, no->bo_n);
	} else
		no->bo_n += 2;

	smi_oidlen(o);

	return (no);
}

int
mib_ipfroute(struct oid *oid, struct ber_oid *o, struct ber_element **elm)
{
	struct ber_element	*ber = *elm;
	struct kroute		*kr;
	struct sockaddr_in	 addr, nhaddr;
	int			 idx = o->bo_id[OIDIDX_ipfInetCidrRoute];
	int			 af;
	u_int8_t		 prefixlen, prio, type, proto;

	OBSD_UNUSED_ARG(oid);  /* gh2289n: avoid compiler warning */

	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_len = sizeof(addr);

	af = o->bo_id[OIDIDX_ipfInetCidrRoute + 1];
	obsd_snmpd_mps_decodeinaddr(o, &addr.sin_addr, OIDIDX_ipfInetCidrRoute + 3);
	obsd_snmpd_mps_decodeinaddr(o, &nhaddr.sin_addr, OIDIDX_ipfInetCidrRoute + 23);
	prefixlen = (u_int8_t) o->bo_id[OIDIDX_ipfInetCidrRoute + 7]; /* OBSD_ITGR -- omit warning */
	prio = (u_int8_t) o->bo_id[OIDIDX_ipfInetCidrRoute + 10]; /* OBSD_ITGR -- omit warning */
	kr = kroute_getaddr(addr.sin_addr.s_addr, prefixlen, prio, 0);
	if (kr == NULL || af == 0) {
		return (1);
	}

	/* write OID */
	ber = ber_add_oid(ber, o);

	if (ber) /* RQ 1944871 -- NULL pointer exception */
	{
		switch (idx) {
		case 7: /* IfIndex */
			ber = ber_add_integer(ber,
				(kr->flags & F_CONNECTED) ? kr->if_index : 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 8: /* Type */
			if (kr->flags & F_REJECT)
				type = 2;
			else if (kr->flags & F_BLACKHOLE)
				type = 5;
			else if (kr->flags & F_CONNECTED)
				type = 3;
			else
				type = 4;
			ber = ber_add_integer(ber, type);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 9: /* Proto */
			switch (kr->priority) {
			case RTP_CONNECTED:
				proto = 2;
				break;
			case RTP_STATIC:
				proto = 3;
				break;
			case RTP_OSPF:
				proto = 13;
				break;
			case RTP_ISIS:
				proto = 9;
				break;
			case RTP_RIP:
				proto = 8;
				break;
			case RTP_BGP:
				proto = 14;
				break;
			default:
				if (kr->flags & F_DYNAMIC)
					proto = 4;
				else
					proto = 1; /* not specified */
				break;
			}
			ber = ber_add_integer(ber, proto);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 10: /* Age */
			ber = ber_add_integer(ber, 0);
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 11: /* NextHopAS */
			ber = ber_add_integer(ber, 0);	/* unknown */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			ber_set_header(ber, BER_CLASS_APPLICATION, SNMP_T_GAUGE32);
			break;
		case 12: /* Metric1 */
			ber = ber_add_integer(ber, -1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 13: /* Metric2 */
			ber = ber_add_integer(ber, -1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 14: /* Metric3 */
			ber = ber_add_integer(ber, -1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 15: /* Metric4 */
			ber = ber_add_integer(ber, -1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 16: /* Metric5 */
			ber = ber_add_integer(ber, -1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		case 17: /* Status */
			ber = ber_add_integer(ber, 1);	/* XXX */
			if (ber == NULL) return -1; /* RQ 1944871 -- NULL pointer exception */
			break;
		default:
			return (-1);
		}
	}
	else
	{
		return -1;
	}

	return (0);
}
#endif	/* OBSD_SNMPD_DISABLE_IP_FORWARD_MIB */


/*
 * Import all MIBs
 */

void
mib_init(void)
{
	/*
	 * MIB declarations (to register the OID names)
	 */
	smi_mibtree(mib_tree);

	/*
	 * MIB definitions (the implementation)
	 */

	/* SNMPv2-MIB */
	smi_mibtree(base_mib);


	/* IF-MIB */
	smi_mibtree(if_mib);                    /* gh2289n: contains the RFC1213 interface group objects AND RFC2863 ifMIB objects */

	/* IP-MIB */
	smi_mibtree(ip_mib);

#ifndef   OBSD_SNMPD_DISABLE_IP_FORWARD_MIB /* gh2289n/_KLE_: disable the complete IP-FORWARD-MIB (OID = 1.3.6.1.2.1.4.24)*/
	/* IP-FORWARD-MIB */
	smi_mibtree(ipf_mib);
#endif /* OBSD_SNMPD_DISABLE_IP_FORWARD_MIB */


#if       OBSD_SNMPD_ADDITIONAL_PN_MIBS
	obsd_snmpd_pn_mibs_init();
#endif /* OBSD_SNMPD_ADDITIONAL_PN_MIBS */

	/* initialize addon rfc1213 compatibility mib implementation */
	obsd_snmpd_mib_addon_rfc1213_init();
}

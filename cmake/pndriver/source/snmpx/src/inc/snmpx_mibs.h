/*
    '.h' file Generated by NetPort Software MIB Compiler
    Copyright 1993 by NetPort Software
*/
/*****************************************************************************/
/** Copyright 1990-96 by NetPort Software. All rights reserved.              */
/** Copyright 1996-2002 By InterNiche Technologies Inc. All rights reserved. */
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: Layer Stack Architecture (LSA)    :P&          */
/*                                                                           */
/*  P a c k a g e         &W: SNMPX                             :W&          */
/*                                                                           */
/*  C o m p o n e n t     &C: SNMPX                             :C&          */
/*                                                                           */
/*  F i l e               &F: snmpx_mibs.h                      :F&          */
/*                                                                           */
/*  V e r s i o n         &V: LSA_SNMPX_P05.00.00.00_00.02.01.01:V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2010-10-28                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SNMPX SNMP-Agent functions                       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*****************************************************************************/

#ifndef SNMPX_MIBS_H
#define SNMPX_MIBS_H


/* The following structure is the iface MIB from rfc1213. Most fields are
 * simple counters and are used as such by the InterNiche networking code.
 * Two exceptions are ifAdminStatus and ifOperStatus. We have tried to
 * keep their use consistent with the RFC.
 *    When an interface (NET) is created, it is marked down (i.e. the
 * ifAdminStatus is set to the "down" position) so that it can safely
 * be configured (a separate step), and then marked "up" once
 * configuration is complete. A NET with ifAdminStatus==DOWN will
 * not be used to send by the inet code.
 *    ifOperStatus should indicate the actual state of the link. For
 * ethernet this probably means UP almost all the time. For PPP it means
 * UP when connected, DOWN other wise; thus a modem in AUTOANSWER would
 * have the same ifAdminStatus and ifOperStatus (UP, DOWN) as a
 * disconnected modem.
 */

/* Tokens for passing to "var_" routines. These can
serve as offsets into the generated MIB group tables.
*/

/* tokens for 'snmp' group */
#define SNMPX_SNMPINPKTS                0
#define SNMPX_SNMPOUTPKTS               SNMPX_SNMPINPKTS+4
#define SNMPX_SNMPINBADVERSIONS         SNMPX_SNMPOUTPKTS+4
#define SNMPX_SNMPINBADCOMMUNITYNAMES   SNMPX_SNMPINBADVERSIONS+4
#define SNMPX_SNMPINBADCOMMUNITYUSES    SNMPX_SNMPINBADCOMMUNITYNAMES+4
#define SNMPX_SNMPINASNPARSEERRS        SNMPX_SNMPINBADCOMMUNITYUSES+4
#define SNMPX_SNMPINTOOBIGS             SNMPX_SNMPINASNPARSEERRS+4
#define SNMPX_SNMPINNOSUCHNAMES         SNMPX_SNMPINTOOBIGS+4
#define SNMPX_SNMPINBADVALUES           SNMPX_SNMPINNOSUCHNAMES+4
#define SNMPX_SNMPINREADONLYS           SNMPX_SNMPINBADVALUES+4
#define SNMPX_SNMPINGENERRS             SNMPX_SNMPINREADONLYS+4
#define SNMPX_SNMPINTOTALREQVARS        SNMPX_SNMPINGENERRS+4
#define SNMPX_SNMPINTOTALSETVARS        SNMPX_SNMPINTOTALREQVARS+4
#define SNMPX_SNMPINGETREQUESTS         SNMPX_SNMPINTOTALSETVARS+4
#define SNMPX_SNMPINGETNEXTS            SNMPX_SNMPINGETREQUESTS+4
#define SNMPX_SNMPINSETREQUESTS         SNMPX_SNMPINGETNEXTS+4
#define SNMPX_SNMPINGETRESPONSES        SNMPX_SNMPINSETREQUESTS+4
#define SNMPX_SNMPINTRAPS               SNMPX_SNMPINGETRESPONSES+4
#define SNMPX_SNMPOUTTOOBIGS            SNMPX_SNMPINTRAPS+4
#define SNMPX_SNMPOUTNOSUCHNAMES        SNMPX_SNMPOUTTOOBIGS+4
#define SNMPX_SNMPOUTBADVALUES          SNMPX_SNMPOUTNOSUCHNAMES+4
#define SNMPX_SNMPOUTGENERRS            SNMPX_SNMPOUTBADVALUES+4
#define SNMPX_SNMPOUTGETREQUESTS        SNMPX_SNMPOUTGENERRS+4
#define SNMPX_SNMPOUTGETNEXTS           SNMPX_SNMPOUTGETREQUESTS+4
#define SNMPX_SNMPOUTSETREQUESTS        SNMPX_SNMPOUTGETNEXTS+4
#define SNMPX_SNMPOUTGETRESPONSES       SNMPX_SNMPOUTSETREQUESTS+4
#define SNMPX_SNMPOUTTRAPS              SNMPX_SNMPOUTGETRESPONSES+4
#define SNMPX_SNMPENABLEAUTHENTRAPS     SNMPX_SNMPOUTTRAPS+4


/* prototypes for "var_" routines in variables table.
Simply delete any that aren't used */

U_CHAR *
snmpx_var_snmp(struct snmpx_variable * vp,
               SNMPX_OID_TYPE* name, int * len, int exact,
               int * var_len);


/* Recommended mib tables. If you edit these, be sure
to correct the values of the corrosponding tokens above.*/


/* MIB table for 'snmp' group */

struct snmpx_snmp_mib
{
    U_LONG  snmpInPkts;
    U_LONG  snmpOutPkts;
    U_LONG  snmpInBadVersions;
    U_LONG  snmpInBadCommunityNames;
    U_LONG  snmpInBadCommunityUses;
    U_LONG  snmpInASNParseErrs;
    U_LONG  snmpInTooBigs;
    U_LONG  snmpInNoSuchNames;
    U_LONG  snmpInBadValues;
    U_LONG  snmpInReadOnlys;
    U_LONG  snmpInGenErrs;
    U_LONG  snmpInTotalReqVars;
    U_LONG  snmpInTotalSetVars;
    U_LONG  snmpInGetRequests;
    U_LONG  snmpInGetNexts;
    U_LONG  snmpInSetRequests;
    U_LONG  snmpInGetResponses;
    U_LONG  snmpInTraps;
    U_LONG  snmpOutTooBigs;
    U_LONG  snmpOutNoSuchNames;
    U_LONG  snmpOutBadValues;
    U_LONG  snmpOutGenErrs;
    U_LONG  snmpOutGetRequests;
    U_LONG  snmpOutGetNexts;
    U_LONG  snmpOutSetRequests;
    U_LONG  snmpOutGetResponses;
    U_LONG  snmpOutTraps;
    long    snmpEnableAuthenTraps;
};


/* List of traps supported */
/*
extern struct trapVar nt_lldpRemTablesChange[];
extern struct trapVar nt_risingAlarm[];
extern struct trapVar nt_fallingAlarm[];
*/

/* Following is a List of new TEXTUAL CONVENTIONS defined
   in the MIBs
*/
extern struct enumList snmpx_en_TruthValue[] ;
extern struct enumList snmpx_en_RowStatus[] ;
extern struct enumList snmpx_en_StorageType[] ;
extern struct enumList snmpx_en_SnmpSecurityLevel[] ;
extern struct enumList snmpx_en_EntryStatus[] ;
extern struct enumList snmpx_en_AddressFamilyNumbers[] ;


#endif /* SNMPX_MIBS_H */
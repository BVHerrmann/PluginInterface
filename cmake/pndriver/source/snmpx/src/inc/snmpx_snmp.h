#ifndef SNMPX_SNMP_H                     /* ----- reinclude-protection ----- */
#define SNMPX_SNMP_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/** Portions Copyright 1988, 1989 by Carnegie Mellon University All Rights Reserved. */
/** Copyright 1990-96 by NetPort Software. All rights reserved.              */
/** Copyright 1996-2004 By InterNiche Technologies Inc. All rights reserved. */
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  F i l e               &F: snmpx_snmp.h                      :F&          */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15 :V&          */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                        :D&          */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SNMP-Library of SNMPX                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  20.07.04    VE    initial version.                                       */
/*                                                                           */
/*****************************************************************************/


/*
 * Definitions for SNMP (RFC 1067) agent variable finder.
 */
/*
 * Portions Copyright 1988, 1989 by Carnegie Mellon University All
 * Rights Reserved Permission to use, copy, modify, and distribute
 * this software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright notice
 * appear in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation, and that the
 * name of CMU not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission. CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL CMU BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
/* Enable MIB_VALIDATION when the validation information in the MIBs
 * is to be used to validation the values of MIB objects (at
 * runtime). This is a 2 step operation. 1. Enable MIB_VALIDATION in
 * mibcomp\parse.h. Recompile the MIB compiler and run the MIBs
 * through it. Copy the generated SNMPVARS.C file to this directory.
 * This file has all the validation information for each object. 2.
 * Enable MIB_VALIDATION in this file. Recompile the sources in this
 * file. This will enable the code in SNMP Engine which validates all
 * incoming values (objects in SET operation).

#define MIB_VALIDATION  1

 */

#ifdef MIB_VALIDATION
#define  MCT_OTHER   1
#define  MCT_INT     2
#define  MCT_STR     3

#define  MCT_RANGES  1
#define  MCT_VALUES  2

#define  MCT_MAX_VALUES 10

struct size_info
{
    int      itype ;  /* informationType - MCT_INT, MCT_STR, MCT_OTHER */
    int      rtype ;  /* representationType - MCT_RANGES, MCT_VALUES */
    unsigned count ;  /* Number of values/ranges */
    long     values[MCT_MAX_VALUES];
};

typedef struct size_info * SIZE_INFO;
#endif   /* MIB_VALIDATION */

#define  SNMPX_DEF_VARLEN  20 /* default length of oid buffers */

struct snmpx_variable
{
    SNMPX_OID_TYPE      name[SNMPX_DEF_VARLEN]; /* object identifier of variable */
    U_CHAR   namelen;       /* length of above */
    char     type;       /* type of variable, INTEGER or (octet) STRING */
    U_CHAR   magic;      /* passed to function as a hint */
    U_SHORT  acl;        /* access control list for variable */
    U_CHAR  * (*findVar)(struct snmpx_variable *, SNMPX_OID_TYPE *, int *, int,int *);
    /* function that finds variable */
#ifdef MIB_VALIDATION
    struct size_info  ranges;  /* Set of ranges specified in MIB file */
#endif
};

/* variables set by var_xxxx() routines if specialized SET is required */

struct SetParms
{
    int (*access_method)(U_CHAR *, U_CHAR,int,U_CHAR *, int);
    struct snmpx_variable * vp;   /* pointer to variables table entry */
    SNMPX_OID_TYPE * name;    /* full Object ID of variable */
    int   name_len;   /* object count in "name". */
    int   do_range;   /* TRUE for range/length checking */
    long  hi_range;   /* hi/lo range for numeric sets */
    long  lo_range;
    int   vbflags;    /* flags for special conditions like SF_NOASCIICHK
                      * cleared before processing every varbind */
    int   pduflags;   /* flags like SF_PDU_NEWROW, which are valid for PDU
                      * cleared before processing every pdu */
#ifdef MIB_VALIDATION
    SIZE_INFO   p_ranges;   /* Set of ranges specified in MIB */
#endif
};

/* Set of flags for the SetParms's vbflags field */

#define SF_NOASCIICHK      0x0001  /* don't do is_us_ascii() check for this obj */
#define SF_UNSIGNEDVALUE   0x0002  /* verify that object is non-negative */
#define SF_STORAGETYPE     0x0004  /* verify values of StorageType object */
#define SF_SECURITYLEVEL   0x0008  /* verify values of SecurityLevel object */
#define SF_NOCREATION      0x0010  /* can't create new row, return noCreation */
#define SF_WRONGVALUE      0x0020  /* can't create new row, return wrongValue */

/* Set of flags for the SetParms's pduflags field */
#define SF_PDU_NEWROW      0x0001  /* new row created */


extern   struct SetParms   snmpx_set_parms;  /* single static copy of set parms */
extern   struct snmpx_variable   snmpx_snmp_variables[];   /* produced by MIB compiler */

/* For enumerated data-types in MIB, the following structure is used */

struct enumList
{
    char * name;
    long  value;
};

/*****************************************************************************/
/*  end of file snmpx_snmp.h                                                 */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of SNMPX_SNMP_H */



/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal_oha_mibs_helper.c
 *
 * @brief       helper functions for obsd_platform_mibal_oha_mibs_adapter.c
 * @details     helper functions for the adapter for the access to the LLDP and MRP MIB implementation 
 *              in OHA Implements the "Level 2 Interface
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.02
 * @date        created: 28.11.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_socket.h>             /* needed for obsd_kernel_if.h */
#include <net/obsd_kernel_if.h>                 /* needed for snmpd.h          */

#include <obsd_userland_stdlib.h>               /* malloc  */
#include <obsd_userland_string.h>               /* memcpy  */

#include <obsd_userland_event.h>                /* needed for snmpd.h */
#include "obsd_snmpd_snmpd.h"                   /* SNMPD_MAXVARBINDLEN, SNMP_ERROR_GENERR */
#include "obsd_snmpd_ber.h"                     /* BER_TYPE_xxxx   */

#include "obsd_snmpd_pn_mibs.h"                 /* pn_mibs_async_var_request_done_cb */

#include "snmpx_oha_interface.h"                /* OHA's interface for LLDP and MRP MIB requests  */
#include "obsd_platform_mibal_oha_mibs_helper.h"/* own interface                                  */




static unsigned char * MIBAL_decode_var_for_oha(struct ber_element  *pelm,              /* in       */
                                                unsigned char       *pvar_type,         /* out      */
                                                int                 *pvar_length,       /* out      */
                                                int                 *psnmp_response     /* out      */
                                               );

static int MIBAL_encode_var_from_oha(           struct ber_oid      *poid,              /* in       */
                                                unsigned char       *pvar_buf,          /* in       */
                                                unsigned char       oha_var_type,       /* in       */
                                                int                 var_length,         /* in       */
                                                int                 *psnmp_response,    /* in       */
                                                struct ber_element  **pelm              /* in / out */
                                     );

static int MIBAL_copy_oha_oid_to_ber_oid(       unsigned int        fulloid[],          /* in       */
                                                int                 fulloid_len,        /* in       */
                                                struct ber_oid      *result_oid,        /* out      */
                                                int                 result_oid_max_len  /* in       */
                                        );


/* =================== MRP and LLDP MIB helper functions  =================== */
/* We get the LLDP and MRP Mib variables from PN's OHA.                       */
/* Due to the nature of PN we have to call OHA functions asynchronous         */

/* handles get- and getnext requests                                                              */
/* returns:                                                                                       */
/*  -1: error                                                                                     */
/*   0: OK                                                                                        */
int MIBAL_oha_getvar_request(           fn_snmpx_oha_mib_get_var_t  fn_oha_get_request,     /* in       */
                                        struct oid                  *oid,                   /* in       */
                                        struct ber_oid              *o,                     /* in       */
                                        struct ber_element          **pelm                  /* in       */ /* out in case of synchronus operation */
                            )
{
    int             rc              = -1;

    OBSD_UNUSED_ARG(oid);  /* avoid compiler warning, oid is part of the common MIB functions interface, maybe needed later */

    if (fn_oha_get_request != NULL)
    {
        unsigned char   *pvar_buffer            = (unsigned char *)malloc(SNMPD_MAXVARBINDLEN+1); /* let space for the 0-termination for strings */

        if (pvar_buffer != NULL)
        {   /* pvar_buffer points to allocated memory */
            void            *pAsyncResponseContext  = (void *)*pelm;

            /* OHA may overwrite our OID (what happens indeed in case of a getnext request).      */
            /* But assuming we always get back the right OID from OHA in the callback (if we work */
            /* asynchronous) we never need the original unchanged OID to make the new ber element */
            /* for the response. So we don't need to allocate memory here to make a OID copy of   */
            /* the OID before it's possible modification by OHA.                                  */
            int oha_oid_len = (int) o->bo_n;

            rc = fn_oha_get_request( o->bo_id, oha_oid_len, pvar_buffer, SNMPD_MAXVARBINDLEN, pAsyncResponseContext);

            /* returns:                                                                       */
            /*  -1: error                                                                     */
            /*   0: OK, but asynchronous completion required                                  */
            /*   1: for test purposes with synchronous answers                                */
            /*OSAL_SimpleSleep(3000);*/ /* for test purposes to check if we can handle race conditions */
            if (rc == 0)
            {   /* OK, but we have to wait for the async completion, we have no result so far */
                (*pelm)->oha_par.wait_for_async_completion = 1; /* asynchronous completed     */
                /* let rc = 0;     */                           /* no error detected so far   */
                /* poid_value_buffer is free'd by the async don callback */
            }
            else if (rc > 0)
            {   /* OK and ready, synchronous completion, means we have already the result     */
                /* we got the result back from the request without waiting for async          */
                /* completion breakdown the return code into the information pieces           */
                int             var_len         = rc/64;
                unsigned char   var_type        = (unsigned char) (rc%64 -5);
                int             snmp_response   = SNMP_ERROR_GENERR;

                rc = MIBAL_encode_var_from_oha( o, pvar_buffer, var_type, var_len, &snmp_response, pelm); /* pelm overwritten afterwards (points to e new elm) */
                (*pelm)->oha_par.wait_for_async_completion = 0; /* synchronous completed      */
                free(pvar_buffer);  /* not needed any longer because we don't have a callback */
                rc = 0;                                     /* OK. no error detected so far   */
            }
            else
            {   /* unexpected return code from the request function, assume we don't installed*/
                /* a async callback and have to free the memory.                              */
                free(pvar_buffer);  /* not needed any longer because we don't have a callback */
                rc = -1; /* error */
            }
        }
        else
        {   /* got no memory for the var_buffer */
            rc = -1;
        }
    }
    else
    {   /* no request function to call */
        rc = -1;
    }
    return rc;
}



/* returns:                                                                                       */
/*  -1: error                                                                                     */
/*   0: OK                                                                                        */
int MIBAL_oha_setvar_request(           fn_snmpx_oha_mib_set_var_t  fn_oha_set_request,     /* in       */
                                        struct oid                  *oid,                   /* in       */
                                        struct ber_oid              *o,                     /* in       */
                                        struct ber_element          **pelm                  /* in       */ 
                                   )
{
    int             rc              = -1;

    OBSD_UNUSED_ARG(oid);  /* avoid compiler warning, oid is part of the common MIB functions interface, maybe needed later */

    if (fn_oha_set_request != NULL)
    {
        unsigned char   *pvar_buffer    = NULL;             /* memory is allocated in mib_decode_var  */
        unsigned char   var_type        = BER_TYPE_EOC;     /* don't use BER_TYPE_DEFAULT, is negative*/
        int             var_len         = SNMPD_MAXVARBINDLEN;
        int             snmp_response   = SNMP_ERROR_GENERR;

        /* allocates memory for the variable (var_buffer) if a value !?= NULL is returned,        */
        /* pvar_buffer points to it                                                               */
        pvar_buffer = MIBAL_decode_var_for_oha( *pelm, &var_type, &var_len, &snmp_response);

        if (pvar_buffer != NULL)
        {   /* pvar_buffer points to allocated memory */
            void        * pAsyncResponseContext = (void *)*pelm;

            /* OHA may overwrite our OID (what happens indeed in case of a getnext request).      */
            /* But assuming we always get back the right OID from OHA in the callback (if we work */
            /* asynchronous) we never need the original unchanged OID to make the new ber element */
            /* for the response. So we don't need to allocate memory here to make a OID copy of   */
            /* the OID before it's possible modification by OHA.                                  */

            rc = fn_oha_set_request( o->bo_id, (int)o->bo_n, pvar_buffer, var_type, var_len, pAsyncResponseContext); /* cast from size_t to int to avoid compiler warning */
            /* returns:                                                                       */
            /*  -1: error                                                                     */
            /*   0: OK, but asynchronous completion required                                  */
            /*   1: for test purposes with synchronous answers                                */
            /* types in pvar_buffer, described by pvar_type and pvar_buffer_len:              */
            /*   OHA_SNMP_INTEGER: 32 Bit integer                                             */

            if (rc == 0)
            {   /* OK, but we have to wait for the async completion, we have no result so far */
                (*pelm)->oha_par.wait_for_async_completion = 1; /* asynchronous completed     */
                /* let rc = 0;     */                           /* no error detected so far   */
                /* poid_value_buffer is free'd by the async don callback */
            }
            else if (rc == 1)
            {   /* OK and ready, synchronous completion, means we have already the result     */
                (*pelm)->oha_par.wait_for_async_completion = 0; /* synchronous completed      */
                rc = 0;                                     /* OK. no error detected so far   */
                free(pvar_buffer);  /* not needed any longer because we don't have a callback */
            }
            else
            {   /* unexpected return code from the request function, assume we don't installed*/
                /* a async callback and have to free the memory.                              */
                free(pvar_buffer);  /* not needed any longer because we don't have a callback */
                rc = -1; /* error */
            }
        }
        else
        {    /* no memory allocated for the var_buffer */
            rc = -1;
        }
    }
    else
    {   /* no request function to call */
        rc = -1;
    }

    return rc;
}

/* types in pvar_buffer, described by pvar_type and pvar_buffer_len:                              */
/*   OHA_SNMP_INTEGER: 32 Bit integer                                                             */
int MIBAL_oha_async_var_request_done_cb(            unsigned int    *poid,          /* in       */
                                                    int             oid_length,     /* in       */
                                                    int             snmp_response,  /* in       */
                                                    void            *pvar_buffer,   /* in       */
                                                    unsigned char   var_type,       /* in       */
                                                    int             var_length,     /* in       */
                                                    void            *pcontext_ptr   /* in       */
                                       )
{
    int                     rc          = -1;

    /* the upper layer (SNMPD) uses it's internal definitions. So, in general, we had to do some  */
    /* conversions here                                                                           */

    /* conversion of the snmp_response */
    /* it's assumed that OHA delivers the right snmp_response directly suited for further         */
    /* processing and is compatible to the SNMPD's definitions. So no conversion done here.       */
    /* A simple preprocessor check is not possible, because SNMPD defines the error codes not     */
    /* using defines like OHA (are enums instead).                                                */

    if (pcontext_ptr != NULL) /* is equal to pelm != NULL */
    {
        struct ber_element      *pelm       = (struct ber_element *)pcontext_ptr;
        be_async_cb_func_t      pfn;
        void                    *pfn_arg;
        struct ber_oid          full_oid;
        int                     snmp_op             = pelm ->oha_par.snmp_operation;
        struct oid              *getnext_last_oid   = pelm->oha_par.be_getnext_last_oid;
        snmp_varbind_choice_t   varbind_choice      = SNMP_VARBIND_CHOICE_UNSPECIFIED;

        pfn     = pelm->oha_par.be_async_cb;/* save callback and callback-arg already here,       */
                                            /* because mib_encode_var will overwrite our elm      */
                                            /* pointer (points to a new ber element) afterwards   */
        pfn_arg = pelm->oha_par.be_async_cbarg;
        /* pelm->oha_par.wait_for_async_completion = 0; not needed to mark this elem completed,   */
        /*                                              because wait_for_async_completion is used */
        /*                                              like a return code only on the "trigger"  */
        /*                                              side" thread. Changing this here in the   */
        /*                                              call back could lead to race conditions.  */

        rc = MIBAL_copy_oha_oid_to_ber_oid(poid, oid_length, &full_oid, BER_MAX_OID_LEN);

        if (rc == 0)
        {
            if (snmp_response == SNMP_ERROR_NONE)  /* SNMP_ERROR_NONE: 0 */
            {
                if (    (snmp_op == SNMP_C_GETREQ)      || 
                        (snmp_op == SNMP_C_GETNEXTREQ)  
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
						|| (snmp_op == SNMP_C_GETBULKREQ)
#endif
                   )
                {   /* in case of some get request we have to encode a ber element */

                    /* pelm overwritten afterwards (points to a new chained elm) */
                    rc = MIBAL_encode_var_from_oha( &full_oid, pvar_buffer, var_type, var_length, &snmp_response, &pelm);
                }
                else if (snmp_op == SNMP_C_SETREQ)
                {   
                    rc = 0;     /* currently no need to to anything else for a set request */
                }
                else
                {   /* unsupported request callback */
                    rc = -1;
                }
            }
            else
            {   /* snmp_response != SNMP_ERROR_NONE */
                /* let rc set to -1 */
                switch(snmp_response)
                {
                    /* gh2289n: seems that OHA's snmp_response is some kind of VarBind CHOICE     */
                    /*          information (see RFC1905 Chapter 3, "-- variable binding" for     */
                    /*          details) and / or some kind of SNMP return error status.          */
                    /*          According to Ernst we could get snmp_response == OHA_SNMP_ASN_EOM */
                    /*          as indication for an EndOfMib in case of getnext requests (using  */
                    /*          our snmpx_oha_get_next_lldp/mrp_var interface). OHA_SNMP_ASN_EOM  */
                    /*          stands for the SNMPv2 endOfMibView, which is a VarBind CHOICE     */
                    /*         (but not a SNMP return error status)                               */

                    /* allowed values for snmp_response are: */
                    /* SNMPv1 */
                    case SNMP_ERROR_TOOBIG:         /*  SNMP_ERROR_TOOBIG          1              */  /* should not happen for get */
                    case SNMP_ERROR_NOSUCHNAME:     /*  SNMP_ERROR_NOSUCHNAME      2              */
                    case SNMP_ERROR_BADVALUE:       /*  SNMP_ERROR_BADVALUE        3              */  /* should not happen for get */
                    case SNMP_ERROR_READONLY:       /*  SNMP_ERROR_READONLY        4              */  /* should not happen for get */
                    case SNMP_ERROR_GENERR:         /*  SNMP_ERROR_GENERR          5              */
#if (OBSD_PNIO_SUPPORT_SNMPV2C == 1)
                    /* SNMPv2 (see RFC1905) */
                    case SNMP_ERROR_NOACCESS:       /*  SNMP_ERROR_NOACCESS        6              */
                    case SNMP_ERROR_WRONGTYPE:      /*  SNMP_ERROR_WRONGTYPE       7              */
                    case SNMP_ERROR_WRONGLENGTH:    /*  SNMP_ERROR_WRONGLENGTH     8              */
                    case SNMP_ERROR_WRONGENC:       /*  SNMP_ERROR_WRONGENC        9              */
                    case SNMP_ERROR_WRONGVALUE:     /*  SNMP_ERROR_WRONGVALUE     10              */
                    case SNMP_ERROR_NOCREATION:     /*  SNMP_ERROR_NOCREATION     11              */
                    case SNMP_ERROR_INCONVALUE:     /*  SNMP_ERROR_INCONVALUE     12              */
                    case SNMP_ERROR_RESUNAVAIL:     /*  SNMP_ERROR_RESUNAVAIL     13              */  /* EGAIN */
                    case SNMP_ERROR_COMMITFAILED:   /*  SNMP_ERROR_COMMITFAILED   14              */
                    case SNMP_ERROR_UNDOFAILED:     /*  SNMP_ERROR_UNDOFAILED     15              */
                    case SNMP_ERROR_AUTHERROR:      /*  SNMP_ERROR_AUTHERROR      16              */
                    case SNMP_ERROR_NOTWRITABLE:    /*  SNMP_ERROR_NOTWRITABLE    17              */
                    case SNMP_ERROR_INCONNAME:      /*  SNMP_ERROR_INCONNAME      18              */
#endif
                                                    /* nothing else to do, let error status as is */
                                                    break;
                    case OHA_SNMP_ASN_NSO    :      /*  RFC1905 VarBind CHOICE noSuchObject       */
                                                    varbind_choice = SNMP_VARBIND_CHOICE_NO_SUCH_OBJECT;
                                                    snmp_response  = SNMP_ERROR_NOSUCHNAME;
                                                    break;
                    case OHA_SNMP_ASN_NSI:          /* RFC1905 VarBind CHOICE noSuchInstance      */
                                                    varbind_choice = SNMP_VARBIND_CHOICE_NO_SUCH_INSTANCE;
                                                    snmp_response  = SNMP_ERROR_NOSUCHNAME;
                                                    break;
                    case OHA_SNMP_ASN_EOM:          /* RFC1905 VarBind CHOICE endOfMibView        */
                                                    varbind_choice = SNMP_VARBIND_CHOICE_END_OF_MIB_VIEW;
                                                    snmp_response  = SNMP_ERROR_NOSUCHNAME;
                                                    break;
                    default:                        /* don't know what OHA tries to tell us */
                                                    snmp_response = SNMP_ERROR_GENERR; /* map it  */
                                                    break;
                } /* end switch */
            } /* end else (snmp_response == SNMP_ERROR_NONE) */
        }
        else
        {   /* error building the full OID */
            memset(&full_oid, 0, sizeof(struct ber_oid));
        }

        if ( (rc != 0) && (snmp_response == SNMP_ERROR_NONE))
            snmp_response = SNMP_ERROR_GENERR;  /* some kind of internal error */

        if ( pfn != NULL)
        {
            pfn(pfn_arg, pelm, &full_oid, snmp_op, snmp_response, varbind_choice, getnext_last_oid);
            /* note: after this call the context to which pelm belongs to may be free'd. So       */
            /*       pelm and references to pelm elements are invalid and can not be used any     */
            /*       longer !                                                                     */
        }
    } /* if (pcontext_ptr != NULL) */

    if (pvar_buffer != NULL)
        free(pvar_buffer);

    return rc;
}


/* ------------------------------------ internal functions -------------------------------------- */

/* if returnd with != NULL memory was allocated (which the returned pointer points to)            */
static unsigned char * MIBAL_decode_var_for_oha(struct ber_element  *pelm,              /* in       */
                                                unsigned char       *pvar_type,         /* out      */
                                                int                 *pvar_length,       /* out      */
                                                int                 *psnmp_response     /* out      */
                                               )
{
    unsigned char   *pmalloced_mem = NULL;

    switch (pelm->be_encoding)
    {
    case BER_TYPE_INTEGER:      /* type OHA_SNMP_INTEGER */
        {
            long long   snmpd_int_value;        /* snmpd's ber  int values are 64 bit */
            short int   oha_var_type = OHA_SNMP_INTEGER;

            if (pelm->be_class != BER_CLASS_UNIVERSAL)
            {
                switch (pelm->be_type)
                {
                    case SNMP_T_COUNTER32:  oha_var_type = OHA_SNMP_COUNTER;    break;
                    case SNMP_T_GAUGE32:    oha_var_type = OHA_SNMP_GAUGE;      break;
                    case SNMP_T_TIMETICKS:  oha_var_type = OHA_SNMP_TIMETICKS;  break;
                    default:                oha_var_type = -1; break; /* unknown */
                }
            }

            if (oha_var_type != -1)
            {
                if (ber_get_integer(pelm, &snmpd_int_value) == 0)
                {
                    int         int_oha_value;             /* OHA's int's are 32 bit (discussed with Ulli 27.11.2014)         */

                    /* convert to OHA's int type */
                    int_oha_value = (int) snmpd_int_value; /* in case we get an 64 bit int value we loose the upper bits here */

                    pmalloced_mem = malloc(sizeof(int_oha_value));
                    if (pmalloced_mem != NULL)
                    {
                        *((int*)pmalloced_mem)      = int_oha_value;
                        *pvar_type                  = (unsigned char)oha_var_type;
                        *pvar_length                = sizeof(int_oha_value);
                        *psnmp_response             = SNMP_ERROR_NONE;
                    }
                    else
                    {
                        *psnmp_response             = SNMP_ERROR_GENERR;
                    }
                }
                else
                {
                    *psnmp_response                 = SNMP_ERROR_BADVALUE;
                }
            }
            else
            {
                *psnmp_response                 = SNMP_ERROR_BADVALUE; /* for SNMPv2 better: SNMP_ERROR_WRONGTYPE */
            }
        }
        break;
    case BER_TYPE_BITSTRING:    /* type OHA_SNMP_BITS */
        {
            unsigned char * bitstring   = NULL;
            size_t          len         = 0;

            if (ber_get_bitstring(pelm, (void**)&bitstring, &len) == 0)
            {
                /* currently ber_dump_element which is needed to built the response doesn't supports bitstrings */
                *pvar_type              = OHA_SNMP_BITS;
                *pvar_length            = (int) len;    /* type cast from size_t to int to avoid compiler warning*/
                *psnmp_response         = SNMP_ERROR_NONE;
            }
            else
            {
                *psnmp_response         = SNMP_ERROR_BADVALUE;
            }
        }
        break;
    case BER_TYPE_OCTETSTRING:  /* type OHA_SNMP_STRING */
        {
            char    *str    = NULL;
            size_t  len     = 0;

            if (ber_get_nstring(pelm, (void **) &str, &len) == 0)               /* cast to avoid PN compiler warning */
            {

                if (len <= SNMPD_MAXSTRLEN)         /* limit string length to be sure                                */
                {
                    pmalloced_mem = malloc(len+1);  /* 1 more byte to have possibility to add trailing '0'           */

                    if (pmalloced_mem != NULL)
                    {
                        memcpy((char *)pmalloced_mem, str, len); /* no strncpy because it's an octet string */
                        pmalloced_mem[len]      = '\0'; /* for those who assume that the string variable is an C string  */
                        *pvar_type              = OHA_SNMP_STRING;
                        *pvar_length            = (int) len;    /* type cast from size_t to int to avoid compiler warning*/
                        /* len is without trailing '\0'                          */
                        *psnmp_response         = SNMP_ERROR_NONE;
                    }
                    else
                    {
                        *psnmp_response         = SNMP_ERROR_GENERR;
                    }
                }
                else
                {
                    *psnmp_response         = SNMP_ERROR_TOOBIG;
                }
            }
            else
            {
                *psnmp_response             = SNMP_ERROR_BADVALUE;
            }
        }
        break;
    case BER_TYPE_OBJECT:       /* type OHA_SNMP_OBJID */
        {
            struct ber_oid oidvalue;

            if (ber_get_oid(pelm, &oidvalue) == 0)
            {
                unsigned int i;
                int space_to_alloc = oidvalue.bo_n * sizeof(OHA_oid_element_type_t);

                pmalloced_mem = malloc(space_to_alloc);
                if (pmalloced_mem != NULL)
                {
                    for (i=0; i< oidvalue.bo_n; i++)
                    {   /* no memcpy because of the type conversion */
                        ((OHA_oid_element_type_t *) pmalloced_mem)[i] = (OHA_oid_element_type_t) oidvalue.bo_id[i]; /* OID components from */
                        /* OHA are unsigned int type (discussed with Ulli 27.11.2014) */
                    } /* for */

                    *pvar_type                  = OHA_SNMP_OBJID;
                    /* according to Ernst OHA needs the var_length in Bytes for an OID Value (see eMail from 25.03.2015 14:05) */
                    *pvar_length                = (int) oidvalue.bo_n  * sizeof(OHA_oid_element_type_t);  /* cast from size_t to int to avoid compiler warning */
                    *psnmp_response             = SNMP_ERROR_NONE;
                }
                else
                {
                    *psnmp_response             = SNMP_ERROR_GENERR;
                }
            }
            else
            {
                *psnmp_response             = SNMP_ERROR_BADVALUE;
            }
        }
        break;
    case BER_TYPE_NULL:             /* type OHA_SNMP_NULLOBJ */
        /* no break */
    default:
        *psnmp_response= SNMP_ERROR_BADVALUE;  /* maybe SNMP_ERROR_WRONGTYPE for snmpv2 */
        break;
    }

    return pmalloced_mem;
}


static int MIBAL_encode_var_from_oha(   struct ber_oid      *poid,              /* in       */
                                        unsigned char       *pvar_buf,          /* in       */
                                        unsigned char       oha_var_type,       /* in       */
                                        int                 var_length,         /* in       */
                                        int                 *psnmp_response,    /* in       */
                                        struct ber_element  **pelm              /* in / out */
                                      )
{
    int                 rc      = -1;
    struct ber_element  *newelm = NULL;

    unsigned short int  snmpd_var_type    = (unsigned short int) BER_TYPE_DEFAULT;
    short int           snmpd_application = -1;
    short int           is_unsigned_type  = 0;

    switch (oha_var_type)
    {
        case OHA_SNMP_INTEGER:      snmpd_var_type = BER_TYPE_INTEGER;      break;
        case OHA_SNMP_BITS:         snmpd_var_type = BER_TYPE_BITSTRING;    break;
        case OHA_SNMP_STRING:       snmpd_var_type = BER_TYPE_OCTETSTRING;  break;
        case OHA_SNMP_OBJID:        snmpd_var_type = BER_TYPE_OBJECT;       break;
        case OHA_SNMP_NULLOBJ:      snmpd_var_type = BER_TYPE_NULL;         break;

        case OHA_SNMP_COUNTER:      snmpd_var_type = BER_TYPE_INTEGER; snmpd_application = SNMP_T_COUNTER32; is_unsigned_type = 1; break;
        /* case OHA_SNMP_UNSIGNED32:   same value as OHA_SNMP_GAUGE */
        case OHA_SNMP_GAUGE:        snmpd_var_type = BER_TYPE_INTEGER; snmpd_application = SNMP_T_GAUGE32;   is_unsigned_type = 1; break;
        case OHA_SNMP_TIMETICKS:    snmpd_var_type = BER_TYPE_INTEGER; snmpd_application = SNMP_T_TIMETICKS; break;

        default:                    break; /* let snmpd_var_type and snmpd_application set as initialized */ 
    }
    /* note: ber_add_oid creates a new elem */
    switch (snmpd_var_type)
    {
        case BER_TYPE_INTEGER:
            {
                int         int_oha_value;                      /* OHA's int's are 32 bit (discussed with Ulli 27.11.2014)      */

                if (var_length == sizeof(int_oha_value))
                {
                    long long   snmpd_int_value;                /* snmpd's ber int values are 64 bit                            */

                    int_oha_value   = *(int*) pvar_buf;
                    snmpd_int_value = (long long) int_oha_value;/* expand OHA's int to snmpd's ber int type                     */

                    newelm = ber_add_oid(*pelm, poid);          /* must add the current OID because it's declared to be dynamic */
					if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
					{
						*psnmp_response = SNMP_ERROR_GENERR;
						return -1;

					}

                    if (snmpd_application != -1)
                    {   /* have a RFC1065 application wide type */
                        if (is_unsigned_type != 0)
                        {   /* is a unsigned one */
                            newelm = ber_add_integer(newelm, (u_int32_t)snmpd_int_value);
                        }
                        else
                        {
                            newelm = ber_add_integer(newelm, snmpd_int_value);
                        }
						if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
						{
							*psnmp_response = SNMP_ERROR_GENERR;
							return -1;

						}
						else
						{
							ber_set_header(newelm, BER_CLASS_APPLICATION, snmpd_application);
						}
                    }
                    else
                    {   /* have a ASN.1 primitive type (signed) integer (non-aggregate type) */
                        newelm = ber_add_integer(newelm, snmpd_int_value);
						if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
						{
							*psnmp_response = SNMP_ERROR_GENERR;
							return -1;

						}
					}

                    *psnmp_response = SNMP_ERROR_NONE;
                    rc = 0;
                }
                else
                {
                    *psnmp_response = SNMP_ERROR_BADVALUE;
                    rc = -1;
                }
            }
            break;
        case BER_TYPE_BITSTRING:
            /* note: currently ber_dump_element which is needed to built the response doesn't supports bitstrings ! */
            newelm = ber_add_oid(*pelm, poid);
			if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
			{
				*psnmp_response = SNMP_ERROR_GENERR;
				return -1;
			}

			newelm = ber_add_bitstring(newelm, pvar_buf, var_length);
			if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
			{
				*psnmp_response = SNMP_ERROR_GENERR;
				return -1;
			}

			rc = 0;
            *psnmp_response = SNMP_ERROR_NONE; 
            break;
        case BER_TYPE_OCTETSTRING:
            newelm = ber_add_oid(*pelm, poid);          /* must add the current OID because it's declared to be dynamic */
            /*pvar_buf[var_length] = '\0';*/            /* 0 terminate string to be sure --> no, would be OHA's task    */
			if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
			{
				*psnmp_response = SNMP_ERROR_GENERR;
				return -1;
			}
			newelm = ber_add_nstring(newelm, (const char*)pvar_buf, var_length);/* no ber_add_string because it's an    */
                                                                                /* octet string                         */
			if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
			{
				*psnmp_response = SNMP_ERROR_GENERR;
				return -1;
			}
			*psnmp_response = SNMP_ERROR_NONE;
            rc = 0;
            break;
        case BER_TYPE_OBJECT:
            {
                struct ber_oid oidvalue;

                oidvalue.bo_n = var_length / sizeof(OHA_oid_element_type_t); /* counts the elements in the OID value */

                if (oidvalue.bo_n < BER_MAX_OID_LEN)
                {
                    unsigned int i;

                    for (i=0; i< oidvalue.bo_n; i++)
                    {   /* no memcpy because of the type conversion */
                        oidvalue.bo_id[i] = (u_int32_t) (((OHA_oid_element_type_t*) pvar_buf)[i]);  /* OHA's OID components from OHA are  */
                        /* unsigned int type (discussed with Ulli 27.11.2014)  */
                    }
                    newelm = ber_add_oid(*pelm, poid);          /* must add the current OID because it's declared to be dynamic */
					if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
					{
						*psnmp_response = SNMP_ERROR_GENERR;
						return -1;
					}
					newelm = ber_add_oid(newelm, &oidvalue);
					if (newelm == NULL) /* RQ 1944871 -- NULL pointer exception */
					{
						*psnmp_response = SNMP_ERROR_GENERR;
						return -1;
					}
					*psnmp_response = SNMP_ERROR_NONE;
                    rc = 0;
                }
                else
                {
                    *psnmp_response = SNMP_ERROR_TOOBIG;
                    rc = -1;
                }
            }
            break;
        case BER_TYPE_NULL:
            /* no break */
        default:
            /* if we would need the current OID which causes the error: */
            /* newelm = ber_add_oid(*pelm, poid);
               newelm = ber_add_null(newelm);
            */
            *psnmp_response= SNMP_ERROR_BADVALUE;  /* maybe SNMP_ERROR_WRONGTYPE for snmpv2 */
            rc = -1;
            break;
    } /* switch */

    if (rc == 0)
        *pelm = newelm; /* return ber element, overwrite callers elem pointer */

    return rc;
}


static int MIBAL_copy_oha_oid_to_ber_oid(   unsigned int        fulloid[],          /* in       */
                                            int                 fulloid_len,        /* in       */
                                            struct ber_oid      *result_oid,        /* out      */
                                            int                 result_oid_max_len  /* in       */
                                        )
{
    if ( (fulloid_len ) <= result_oid_max_len)
    {
        int i;

        for (i=0; i < fulloid_len; i++)
        {
            result_oid->bo_id[i] = (u_int32_t) (fulloid[i]); /* no memcpy because of the type conversion */
        }
        result_oid->bo_n  = i;
        return 0;
    }
    else
        return -1;
}



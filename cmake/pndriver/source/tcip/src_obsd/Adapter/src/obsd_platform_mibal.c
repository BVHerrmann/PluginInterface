/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal.c
 *
 * @brief       MIB A(daption) L(ayer) main include file
 * @details     Implementation of the MIBAL module
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.08
 * @date        created: 12.05.2014
 */
/* -------------------------------------------------------------------------------------------------- */

#include <sys/obsd_kernel_types.h>

#include "obsd_userland_string.h"               /* strlcpy */

#include "obsd_platform_mibal.h"                /* own interface */

#include "obsd_snmpd_mib.h"                     /* mib_set_ifdescr */
#include "obsd_snmpd_snmpdext.h"                /* recv_msg_evt_chain_arg_t */

#include "tcip_obsd_cfg.h"

#define MIB2_SYS_SERVICES_DEFAULT_VALUE   72  /* assume most devices are always IP hosts with     */
                                              /* application services, default value              */

/* default names for MIB-2 system vars, length must be <=SYS_DESCR_DISPLAY_STRING_SIZEMAX */
/* OBSD_ITGR -- according to SPH (SNMP in C3) (see 2710554) the default string is the empty string */
#define SYS_DESCRIPTION_DEFAULT ""
#define SYS_CONTACT_DEFAULT     ""
#define SYS_NAME_DEFAULT        ""
#define SYS_LOCATION_DEFAULT    ""

static int	Mib2_sysServices = OBSD_PNIO_CFG_SYS_SERVICES; /* may be substituted by a PN #define */
static int          MIBAL_inInitialized = 0;
static char         Mib2_sysDescr[   SYS_DESCR_DISPLAY_STRING_SIZEMAX   +1];  /* size including trailing zero */
static char         Mib2_sysContact[ SYS_CONTACT_DISPLAY_STRING_SIZEMAX +1];
static char         Mib2_sysName[    SYS_NAME_DISPLAY_STRING_SIZEMAX    +1];
static char         Mib2_sysLocation[SYS_LOCATION_DISPLAY_STRING_SIZEMAX+1];
MIBAL_mib2_system_var_written_cb_t   SysvarWrittenCb;

/**
    @brief          Initializes the MIBAL module
    @param          nothing
    @return         0 in case of success
*/
int MIBAL_Init(void)
{
    if (!MIBAL_inInitialized)
    {
        strlcpy(Mib2_sysDescr,    SYS_DESCRIPTION_DEFAULT, SYS_DESCR_DISPLAY_STRING_SIZEMAX   +1 );
        strlcpy(Mib2_sysContact,  SYS_CONTACT_DEFAULT,     SYS_CONTACT_DISPLAY_STRING_SIZEMAX +1 );
        strlcpy(Mib2_sysName,     SYS_NAME_DEFAULT,        SYS_NAME_DISPLAY_STRING_SIZEMAX    +1 );
        strlcpy(Mib2_sysLocation, SYS_LOCATION_DEFAULT,    SYS_LOCATION_DISPLAY_STRING_SIZEMAX+1 );
        SysvarWrittenCb  = NULL;
        MIBAL_inInitialized = 1;
    }
    return 0;
}


/**
    @brief          DeInitializes the MIBAL module
    @param          none
    @return         0 in case of success
*/
int MIBAL_DeInit(void)
{
    SysvarWrittenCb  = NULL;
    MIBAL_inInitialized = 0;
    return 0;  
}

/* ====================== get funtions =================================================== */

/**
    @brief          returns the data for the MIB-II variable sysDescr (OID=1.3.6.1.2.1.1.1)
    @param          none
    @return         sysDescr string
    @note           @li   string length must be <= 255, according to the sysDescr object type:
                          DisplayString (SIZE (0..255))
*/
char * MIBAL_get_mib2_sysDescr(void)
{
    return Mib2_sysDescr;
}

/**
    @brief          returns the data for the MIB-II variable sysContact (OID=1.3.6.1.2.1.1.4)
    @param          none
    @return         sysContact string
    @note           @li   string length must be <= 255, according to the sysContact object type:
                          DisplayString (SIZE (0..255))
*/
char * MIBAL_get_mib2_sysContact(void)
{
    return Mib2_sysContact;
}

/**
    @brief          returns the data for the MIB-II variable sysName (OID=1.3.6.1.2.1.1.5)
    @param          none
    @return         sysName string
    @note           @li   string length must be <= 255, according to the sysName object type:
                          DisplayString (SIZE (0..255))
*/
char * MIBAL_get_mib2_sysName(void)
{
    return Mib2_sysName;
}

/**
    @brief          returns the data for the MIB-II variable sysLocation (OID=1.3.6.1.2.1.1.6)
    @param          none
    @return         sysLocation string
    @note           @li   string length must be <= 255, according to the sysLocation object type:
                          DisplayString (SIZE (0..255))
*/
char * MIBAL_get_mib2_sysLocation(void)
{
    return Mib2_sysLocation;
}

/**
    @brief          returns the data for the MIB-II variable sysServices (OID=1.3.6.1.2.1.1.7)
    @param          none
    @return         sysSerices integer value 
                    @li   OID type: sysServices, INTEGER (0..127)
                    @li   OID Description: A value which indicates the set of services that this entity 
                                           primarily offers.
    @note           @li     The value is a sum.  This sum initially takes the
                            value zero, Then, for each layer, L, in the range
                            1 through 7, that this node performs transactions
                            for, 2 raised to (L - 1) is added to the sum.  For
                            example, a node which performs primarily routing
                            functions would have a value of 4 (2^(3-1)).  In
                            contrast, a node which is a host offering
                            application services would have a value of 72
                            (2^(4-1) + 2^(7-1)).  Note that in the context of
                            the Internet suite of protocols, values should be
                            calculated accordingly:
                            layer  functionality
                            1  physical (e.g., repeaters)
                            2  datalink/subnetwork (e.g., bridges)
                            3  internet (e.g., IP gateways)
                            4  end-to-end  (e.g., IP hosts)
                            7  applications (e.g., mail relays)
                            For systems including OSI protocols, layers 5 and
                            6 may also be counted."
*/
int    MIBAL_get_mib2_sysServices(void)
{
    return Mib2_sysServices;
}


/* ====================== set funtions =================================================== */

/**
    @brief          sets the data for the MIB-II variable sysDescr (OID=1.3.6.1.2.1.1.1)
    @details        The function copies string_length bytes from parameter pchNew_sysDescr to an internal 
                    storage for the system description and terminates the internal string with \0.
                    If the Source String pchNew_sysDescr is too long an error code is provided and 
                    nothing is copied into the internal storage.
    @param          pchNew_sysDescr: string for system description
    @param          string_length:   length of pchNew_sysDescr without trailing \0
    @note           @li   string length must be <= 255 (SYS_DESCR_DISPLAY_STRING_SIZEMAX),
                          according to the sysDescr object type:
                          DisplayString (SIZE (0..255))
    @return         OBSD_SNMPD_MIBAL_ERRCODE_OK:               success
    @return         OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG:  Source String pchNew_sysDescr is too long
    @note           @li     This function is not needed for the upper layer (means for handling 
                            snmp requests) - sysDescr is not writable via snmp requests, 
                            but maybe it's needed for handling of persistency
*/
int MIBAL_set_mib2_sysDescr(const char * pchNew_sysDescr, int string_length)
{
    if (string_length <= SYS_DESCR_DISPLAY_STRING_SIZEMAX)
    {
        strlcpy(Mib2_sysDescr, pchNew_sysDescr, string_length + 1); /* let room for trailing \0, assume Mib2_sysDescr        */
                                                                    /* is SYS_DESCR_DISPLAY_STRING_SIZEMAX + 1 bytes large   */
        return OBSD_SNMPD_MIBAL_ERRCODE_OK;
    }
    return OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG;
}

/**
    @brief          sets the data for the MIB-II variable sysContact (OID=1.3.6.1.2.1.1.4)
    @details        The function copies string_length bytes from parameter pchNew_sysContact to an internal 
                    storage for the system contact and terminates the internal string with \0.
                    If the Source String pchNew_sysContact is too long an error code is provided and 
                    nothing is copied into the internal storage.
    @param          pchNew_sysContact: string for system contact
    @param          string_length:     length of pchNew_sysContact without trailing \0
    @note           @li   string length must be <= 255 (SYS_CONTACT_DISPLAY_STRING_SIZEMAX),
                          according to the sysContact object type:
                          DisplayString (SIZE (0..255))
    @return         OBSD_SNMPD_MIBAL_ERRCODE_OK:               success
    @return         OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG:  Source String pchNew_sysContact is too long
*/
int MIBAL_set_mib2_sysContact(const char * pchNew_sysContact, int string_length)
{
    if (string_length <= SYS_CONTACT_DISPLAY_STRING_SIZEMAX)
    {
        strlcpy(Mib2_sysContact, pchNew_sysContact, string_length + 1);/* let room for trailing \0, assume pchNew_sysContact    */
                                                                       /* is SYS_CONTACT_DISPLAY_STRING_SIZEMAX + 1 bytes large */

        return OBSD_SNMPD_MIBAL_ERRCODE_OK;
    }
    return OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG;
}

/**
    @brief          sets the data for the MIB-II variable sysName (OID=1.3.6.1.2.1.1.5)
    @details        The function copies string_length bytes from parameter pchNew_sysName to an internal 
                    storage for the system name and terminates the internal string with \0.
                    If the Source String pchNew_sysName is too long an error code is provided and 
                    nothing is copied into the internal storage.
    @param          pchNew_sysName:    string for system name
    @param          string_length:     length of pchNew_sysName without trailing \0
    @note           @li   string length must be <= 255 (SYS_NAME_DISPLAY_STRING_SIZEMAX),
                          according to the sysName object type:
                          DisplayString (SIZE (0..255))
    @return         MIBAL_ERRCODE_OK:               success
    @return         MIBAL_ERRCODE_STRING_TOO_LONG:  Source String pchNew_sysName is too long
*/
int MIBAL_set_mib2_sysName(const char * pchNew_sysName, int string_length)
{
    if (string_length <= SYS_NAME_DISPLAY_STRING_SIZEMAX)
    {
        strlcpy(Mib2_sysName, pchNew_sysName, string_length + 1);/* let room for trailing \0, assume pchNew_sysName       */
                                                                 /* is SYS_NAME_DISPLAY_STRING_SIZEMAX + 1 bytes large    */

        return OBSD_SNMPD_MIBAL_ERRCODE_OK;
    }
    return OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG;
}

/**
    @brief          sets the data for the MIB-II variable sysLocation (OID=1.3.6.1.2.1.1.6)
    @details        The function copies string_length bytes from parameter pchNew_sysLocation to an internal 
                    storage for the system location and terminates the internal string with \0.
                    If the Source String pchNew_sysLocation is too long an error code is provided and 
                    nothing is copied into the internal storage.
    @param          pchNew_sysLocation: string for system location
    @param          string_length:      length of pchNew_sysLocation without trailing \0
    @note           @li   string length must be <= 255 (SYS_LOCATION_DISPLAY_STRING_SIZEMAX),
                          according to the sysLocation object type:
                          DisplayString (SIZE (0..255))
    @return         OBSD_SNMPD_MIBAL_ERRCODE_OK:               success
    @return         OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG:  Source String pchNew_sysLocation is too long
*/
int MIBAL_set_mib2_sysLocation(const char * pchNew_sysLocation, int string_length)
{
    if (string_length <= SYS_LOCATION_DISPLAY_STRING_SIZEMAX)
    {
        strlcpy(Mib2_sysLocation, pchNew_sysLocation, string_length + 1);/* let room for trailing \0, assume pchNew_sysLocation   */
                                                                         /* is SYS_LOCATION_DISPLAY_STRING_SIZEMAX + 1 bytes large*/

        return OBSD_SNMPD_MIBAL_ERRCODE_OK;
    }
    return OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG;
}


void MIBAL_set_mib2_ifdescr(
                            unsigned short if_index, /* Key: net statistics belong to */
                            char* ifDescr /* MIB2 interface description */
                            ) 
{
    mib_set_ifdescr(if_index, ifDescr );
}


/**
    @brief          sets a callback function that is called, if snmp writeable system variables are writte. 
                    These variables are:
                    @li   sysContact  (OID=1.3.6.1.2.1.1.4)
                    @li   sysName     (OID=1.3.6.1.2.1.1.5)
                    @li   sysLocation (OID=1.3.6.1.2.1.1.6)
    @param          fn: callback function to call or NULL if nothing has to be called
    @return         nothing
*/
void MIBAL_set_mib2_system_var_written_hook(MIBAL_mib2_system_var_written_cb_t fn)
{
    SysvarWrittenCb = fn;
}


/* ----------------------- EDD statistic counter update stuff ------------------------*/

typedef struct
{
    void (*pChainedFunc)(int fd, short sig, void *arg);
    int fd;
    short int sig;
    void *pChainedFuncArg;
} MIBAL_recv_msg_evt_chain_arg_t; /* argument type for MIBAL_snmpd_receive_msg_event */

static MIBAL_recv_msg_evt_chain_arg_t MIBAL_recv_msg_evt_chain_args;  /* better dynamically malloced in MIBAL_snmpd_receive_msg_event */

/* see TCIP_MIB2_STATISTICS_UPDATE_DONE */
void snmpe_mib2_statistics_update_done()
{
    if (MIBAL_recv_msg_evt_chain_args.fd != -1)
    {   /* call next handler */
        MIBAL_recv_msg_evt_chain_args.pChainedFunc(MIBAL_recv_msg_evt_chain_args.fd , MIBAL_recv_msg_evt_chain_args.sig , MIBAL_recv_msg_evt_chain_args.pChainedFuncArg);
    }
    /* if malloc'd: free(arg) */
}

static int pending = 0;
extern int tcip_mib2_statistics_update(void);

/*------------------------------------------------------------------------------
// snmpd requests EDD's stats counters update
//----------------------------------------------------------------------------*/
static void snmpx_usr_mib2_statistics_update(int fd, short sig, void *arg)
{
	OBSD_UNUSED_ARG(fd);
	OBSD_UNUSED_ARG(sig);
	OBSD_UNUSED_ARG(arg);

	if(pending) /* possibly more than one SNMP message in socket buffer? */
	{
		return;
	}
	
	pending = tcip_mib2_statistics_update();
}
/*------------------------------------------------------------------------------
// TCIP signals EDD's stats counters are up-to-date
//----------------------------------------------------------------------------*/
void TCIP_MIB2_STATISTICS_UPDATE_DONE(void)
{
	pending = 0;
	snmpe_mib2_statistics_update_done();
}


void MIBAL_snmpd_receive_msg_event(int fd, short sig, void *arg)
{
    recv_msg_evt_chain_arg_t *pchain_arg = (recv_msg_evt_chain_arg_t *)arg;

    MIBAL_recv_msg_evt_chain_arg_t *parg = &MIBAL_recv_msg_evt_chain_args; /* better malloc'd */
    parg->pChainedFunc      = pchain_arg->pChainedFunc;
    parg->fd                = fd;
    parg->sig               = sig;
    parg->pChainedFuncArg   = pchain_arg->pChainedFuncArg;
    snmpx_usr_mib2_statistics_update(fd, sig, arg);
}


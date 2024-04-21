
/* ---------------------------------------------------------------------------------------------- */
/*                                                                                                */
/* project    : TCP-Stack for Profinet                                                            */
/* file       : obsd_platform_stack_config.h                                                      */
/* description: configuration variables for the TCP-Stack                                         */
/*              (used for conditional compilation sections)                                       */
/* compiler   : ANSI C                                                                            */
/* maschine   : miscellaneous                                                                     */
/*                                                                                                */
/* history:                                                                                       */
/* version  date      author            change                                                    */
/* ======= ========== ================  ========================================================= */
/*  0.01   24.06.2014 Gerlach, ATS TM4  initial coding                                            */
/*  0.02   08.07.2014 Gerlach, ATS TM4  added SNMPD config constants                              */
/*  0.03   26.08.2014 Gerlach, ATS TM4  added OBSD_USE_PN_IPSTACK_PARAMETERS                      */
/*  0.04   09.09.2014 Gerlach, ATS TM4  added OBSD_EVENTLIB_ENABLE_LOGGING and                    */
/*                                            OBSD_ENABLE_ASSERTION_MESSAGES                      */
/*  0.05   11.09.2014 Gerlach, ATS TM4  added OBSD_SNMPD_USE_PN_IFMIB                             */
/*  0.10   01.09.2015 Gerlach, ATS TM4  added OBSD_PNIO_TCP_DO_RFC1323                            */
/*  0.11   21.01.2016 Gerlach, DF FA AS added OBSD_PNIO_ICMP_DO_FULL_ICMP                         */
/*  0.12   12.03.2016 Gerlach, DF FA AS added OBSD_POOL_MALLOC_NO_SLEEP                           */
/*  0.13   11.05.2016 Gerlach, DF FA AS added OBSD_SNMPD_ENABLE_RFC2863_IFMIB                     */
/*  0.14   27.06.2016 Gerlach, DF FA AS added OBSD_IF_MAX_COUNT                                   */
/* ---------------------------------------------------------------------------------------------- */

#ifndef   __OBSD_PLATFORM_STACK_CONFIG_H__
#define   __OBSD_PLATFORM_STACK_CONFIG_H__

/* configuration constants for the IP stack and basic userland */
#define OBSD_USERLAND_COMMANDS   0  /* 1: enable additional Kernel code & syscalls that are needed*/
                                    /*    for OBSD userland commands / progams / servers that     */
                                    /*    currently are not used by PN (e.g. route, ifconfig,...) */
                                    /* 0: don't enable this code                                  */
#define OBSD_MULTITHREADING      0  /* 1: enable additional kernel code that is needed to make    */
                                    /*    the IP-stack's kernel really multithead capeable        */
                                    /* 0: IP-Stack can't be used by multiple threads              */
#define OBSD_POOL_MALLOC_NO_SLEEP 1 /* 1: don't allow waits (M_WAIT, M_WAITOK, PR_WAITOK, ... )   */
                                    /*    while getting pool elements or kernel memory            */
                                    /* 0: unchanged OpenBSD behavior (pool sleeps are allowed)    */
#define STACK_TEST_VARIANT       0  /* 1: enable code that is used in extended test variants      */
                                    /* 0: don't enable additional code, this is the default for PN*/
#define OBSD_ENHANCED_KERNEL_API 0  /* 1: enable additional system calls and system functionality */
                                    /*    (e.g. AF_UNIX sockets), that is typically needed for    */
                                    /*    enhanced test variants                                  */
                                    /* 0: only a minimum set of system calls, enough for PN       */
#define OBSD_USE_PN_IPSTACK_PARAMETERS 1    /* 1: use stack parameters like required by PN        */
                                            /*    (e.g. ARP cache timeout, socket buffer size, .. */
                                            /* 0: use stack parameters like defined by OpenBSD    */

#undef DIAGNOSTIC                	/* defined: enable code parts intended for diagnostic purpose*/
                                     /*          (mostly in Kernel Code)                          */
                                     /* not defined: disable those code parts                     */

/* TCP parameters */
#define OBSD_PNIO_TCP_DO_RFC1323 0  /* 1: enable RFC1323 timestamp and window scaling             */
                                    /* 0: disable RFC1323 timestamp and window scaling            */

/* ICMP parameters */
#define OBSD_PNIO_ICMP_DO_FULL_ICMP    0    /* 1: enable full OBSD's icmp handling                */
                                            /* 0: disable full icmp handling like requested in    */
                                            /*    TFS RQ1009308. Only answer ICMP ping requests   */
                                            /*    and don't send any ICMP diagnostic error msgs   */


/* configuration constants for the SNMPD */
#define OBSD_SNMPD_ENABLE_LOGGING               0  /* 1: enable call of logging functions, needs  */
                                                   /*    logging func's in file obsd_snmpd_log.c  */
                                                   /* 0: don't enable logging in SNMPD, file      */
                                                   /*    obsd_snmpd_log.c mot needed              */
#define OBSD_SNMPD_OLD_LSA_SOCK_COMPATIBLILITY  0  /* 1: snmpd socket is owned by SOCK            */
                                                   /* 0: snmpd socket opened & bound by SNMPD     */
#define OBSD_SNMPD_USE_PN_IFMIB                 1  /* 1: enables PN specific extensions/changes   */
                                                   /*    to the ifMIB (EDD Counter)               */
                                                   /* 0: ifMIB like defined by OBSD SNMPD         */
#define OBSD_SNMPD_ENABLE_RFC2863_IFMIB         0  /* 1: enable RFC2863 ifMIB objects like they   */
                                                   /*    are part of original OBSD SNMPD (don't   */
                                                   /*    shrink original OBSD's SNMPD MIB size)   */
                                                   /* 0: disables RFC2863 ifMIB extensions,       */
                                                   /*    deliver RFC1213 interface Group objects  */
                                                   /*    only (like required for SPH SNMP Basic   */
                                                   /*    Class compliance and PN Stack 5.4        */
                                                   /*    Interniche compatibility)                */

/* configuration constants for the eventlib */
#define OBSD_EVENTLIB_ENABLE_LOGGING            0  /* 1: eventlib uses logging code               */
                                                   /* 0: eventlib doesn't use logging code        */

/* configuration constants for the libc and libkern */
#define OBSD_ENABLE_ASSERTION_MESSAGES          0  /* 1: allow assertion msgs in libc & libkern   */
                                                   /* 0: disable assertion msgs in libc & libkern */

/* configuration for CARP (Common Address Redundancy Protocol): */
#define OBSD_CARP                               1  /* 1: enable CARP to allow multiple hosts to   */
                                                   /* share the same IP address. This protocol is */
                                                   /* used in the context of redundant systems.   */
                                                   /* 0: disable CARP                             */
                                                   /* CARP advertisement:                         */
                                                   /* In general regular CARP advertisements are  */
                                                   /* sent by the master. There are different     */
                                                   /* scenarios for a switch over:                */
                                                   /* - missing advertisements                    */
                                                   /* - less frequent advertisements              */
                                                   /* - higher demotion counter                   */
                                                   /* Currently this functionality is not         */
                                                   /* required, since a dedicated redundancy      */
                                                   /* application is responsible for controlling  */
                                                   /* master/backup state machine.                */
                                                   /*                                             */
#if OBSD_CARP                                      /*                                             */
    #define OBSD_CARP_ENABLE_LOG                0  /* 1: enable CARP logging                      */
                                                   /* 0: disable CARP logging (usually disabled   */
#else                                              /* in the TIA environment)                     */
    #define OBSD_CARP_ENABLE_LOG                0  /* CARP logging is automatically disabled,     */
#endif /* OBSD_CARP  */                            /* if CARP is not enabled.                     */
                                                   /*                                             */
                                                   /*                                             */
#if OBSD_CARP                                      /*                                             */
    #define OBSD_CARP_ENABLE_INTR               0  /* 1: enable interrupt handling in CARP context*/
                                                   /* 0: disable CARP interrupt handling (usually */
#else                                              /* disabled in the TIA environment)            */
    #define OBSD_CARP_ENABLE_INTR               0  /* CARP interrupt handling is automatically    */
#endif /* OBSD_CARP  */                            /* disabled, if CARP is not enabled.           */
                                                   /*                                             */
#if OBSD_CARP                                      /*                                             */
    #define OBSD_CARP_ENABLE_IF_ACCESS          1  /* 1: enable direct access on CARP interface   */
                                                   /* 0: disable direct access on CARP interface  */
#else                                              /* (usually enabled in the TIA environment)    */
    #define OBSD_CARP_ENABLE_IF_ACCESS          0  /* direct access on CARP interface is disabled */
#endif /* OBSD_CARP  */                            /* by default.                                 */
                                                   /*                                             */
#if OBSD_CARP                                      /*                                             */
    #define OBSD_CARP_ENABLE_ADV                0  /* 1: enable CARP advertisements               */
#else                                              /* 0: disable CARP advertisements              */
    #define OBSD_CARP_ENABLE_ADV                0  /* CARP advertisements are automatically       */
#endif /* OBSD_CARP */                             /* disabled, if CARP is not enabled.           */
                                                   /* As described above, the base CARP           */
                                                   /* implementation includes automatic switch    */
                                                   /* over based on advertisements. Though this   */
                                                   /* functionality is taken over by a separate   */
                                                   /* redundancy application, such switch over    */
                                                   /* indication shall be detected and forwarded  */
                                                   /* to the PN-stack. This option requires the   */
                                                   /* activation of CARP advertisements.          */
                                                   /*                                             */
 
                                                   /* ARP behavior incompatibility with Internice RQ1783482 */
#define DISABLE_ARP_CACHE_POISONING_PROTECTION  0  /* 1: ARP poisoning protection disabled. */ 
                                                   /* ARP Replies to ARPs from a subnet that belongs to the second network device*/
                                                   /* 0: No answer to ARPs from a subnet that belongs to the second network device  */

#define OBSD_PNIO_USE_LARGE_PACKETS 0
#define OBSD_PNIO_SUPPORT_SNMPV2C 0

#endif /* __OBSD_PLATFORM_STACK_CONFIG_H__ */


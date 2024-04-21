/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_mibal.h
 *
 * @brief       MIB A(daption) L(ayer) main include file
 * @details     Interface of the MIBAL module
 *
 * @author      Hendrik Gerlach ATS TM4
 *
 * @version     V0.07
 * @date        created: 12.05.2014
 */
/* -------------------------------------------------------------------------------------------------- */
#ifndef    __OBSD_PLATFORM_MIBAL_H__
#define    __OBSD_PLATFORM_MIBAL_H__


#define DISPLAY_STRING_SIZEMAX    (255)  /* OBJECT-TYPE DisplayString (SIZE (0..255)) */

#define SYS_DESCR_DISPLAY_STRING_SIZEMAX    (DISPLAY_STRING_SIZEMAX)  /* sysDescr    OBJECT-TYPE DisplayString (SIZE (0..255)) */
#define SYS_CONTACT_DISPLAY_STRING_SIZEMAX  (DISPLAY_STRING_SIZEMAX)  /* sysContact  OBJECT-TYPE DisplayString (SIZE (0..255)) */
#define SYS_NAME_DISPLAY_STRING_SIZEMAX     (DISPLAY_STRING_SIZEMAX)  /* sysName     OBJECT-TYPE DisplayString (SIZE (0..255)) */
#define SYS_LOCATION_DISPLAY_STRING_SIZEMAX (DISPLAY_STRING_SIZEMAX)  /* sysLocation OBJECT-TYPE DisplayString (SIZE (0..255)) */


/* --------------------- interface functions -------------------------------------------------------- */
/* init and deinit */
extern int MIBAL_Init(  void);
extern int MIBAL_DeInit(void);

/* mib-II system vars */
extern  char * MIBAL_get_mib2_sysDescr(   void);
extern  char * MIBAL_get_mib2_sysContact( void);
extern  char * MIBAL_get_mib2_sysName(    void);
extern  char * MIBAL_get_mib2_sysLocation(void);
extern  int    MIBAL_get_mib2_sysServices(void);

extern  int   MIBAL_set_mib2_sysDescr(   const char *pString, int string_length);
extern  int   MIBAL_set_mib2_sysContact( const char *pString, int string_length);
extern  int   MIBAL_set_mib2_sysName(    const char *pString, int string_length);
extern  int   MIBAL_set_mib2_sysLocation(const char *pString, int string_length);

void MIBAL_snmpd_receive_msg_event(int fd, short sig, void *arg);
void MIBAL_set_mib2_ifdescr(
		unsigned short if_index, /* Key: net statistics belong to */
		char* ifDescr /* MIB2 interface description */
		);

typedef void (*MIBAL_mib2_system_var_written_cb_t)(const void * snmp_data);
extern void MIBAL_set_mib2_system_var_written_hook(MIBAL_mib2_system_var_written_cb_t fn);

/* return error codes */
#define OBSD_SNMPD_MIBAL_ERRCODE_OK                ( 0)
#define OBSD_SNMPD_MIBAL_ERRCODE_GENERIC_ERROR     (-1)
#define OBSD_SNMPD_MIBAL_ERRCODE_READ_ONLY         (-2)
#define OBSD_SNMPD_MIBAL_ERRCODE_STRING_TOO_LONG   (-3)

#endif  /* __OBSD_PLATFORM_MIBAL_H__ */


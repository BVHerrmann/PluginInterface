/* -------------------------------------------------------------------------------------------------- */
/**
 * @file        obsd_platform_hal.h
 *
 * @brief       H(ardware) A(daption) L(ayer) main include file
 * @details     Interface of the HAL module
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.06
 * @date        created: 06.05.2013
 */
/* -------------------------------------------------------------------------------------------------- */


#ifndef __OBSD_PLATFORM_HAL_H__
#define __OBSD_PLATFORM_HAL_H__

#include "obsd_platform_stack_config.h"

#define HAL_CLOCK_RATE_HZ       (10)    /* system clock frequence under windows is 20Hz (OBSD normally 100Hz) */

/* clock_gettime of the enhanced kernel API requires a timecouner                  */
/* (date and time is used in stack variant with inetd for test purposes´)          */
/* But for the standard PN variant we need the timercounter too. Otherwise some    */
/* features of the stack will no work correctly, e.g.:                             */
/* * RST Rate limiting, see OBSDPN-147                                             */
/* * snmp sysUptime                                                                */
#define HAL_TIMECOUNTER_NEEDED  (1)      /* 1 if we need date time functions       */

#define         HAL_cpu_initclocks()
#define         HAL_cpu_deinitclocks()

#if HAL_TIMECOUNTER_NEEDED
extern unsigned int HAL_get_tc_timecount(void); 
extern unsigned int HAL_get_tc_counter_mask(void);
extern unsigned int HAL_get_tc_frequency(void);
#endif /* HAL_TIMECOUNTER_NEEDED */

#endif /* __OBSD_PLATFORM_HAL_H__ */


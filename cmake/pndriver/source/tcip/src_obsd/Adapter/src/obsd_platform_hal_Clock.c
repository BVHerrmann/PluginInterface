
#include "obsd_platform_osal.h"
#include "obsd_platform_hal.h"

extern int                  cticks; /* OBSD_ITGR -- PN has an external 100ms-tick (see LSA Timer - tcip_obsd_ctick()) */
                                    /*              Xintrltimer is also triggered with this tick                      */

#define TIMER_EVENT_TIME_MS    (1000/HAL_CLOCK_RATE_HZ)  /* time between 2 timer events in ms, must fit to HAL_CLOCK_RATE_HZ */
                                   /* <TBD> normally we had to generate 10ms intervals (100hz), but windows sleep is not able to do that */

#if HAL_TIMECOUNTER_NEEDED
/* v---- following functions only are needed if date time functions expected to work ----v*/
unsigned int HAL_get_tc_timecount(void)
{
    return cticks;                  /* PN's tick counter */
}

unsigned int HAL_get_tc_counter_mask(void)
{
    return 0xffffffff;  /* s_uiClockTicks has 32 valid clock tick bits */
}

unsigned int HAL_get_tc_frequency(void)  /* returns the timer frequency in hz */
{
    return (1000/TIMER_EVENT_TIME_MS);                     /* 1000ms/TIMER_EVENT_TIME_MS */
}

/* ^-------------------------------------------------------------------------------------^ */
#endif /* HAL_TIMECOUNTER_NEEDED */


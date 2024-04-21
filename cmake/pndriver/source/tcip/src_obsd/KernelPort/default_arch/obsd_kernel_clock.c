/*	clock.c */

/* gh2289n: ##automatically insert project defines here## (done) */
/* gerlach/g2289n: need to add #define _KERNEL for all Kernel files because PN Integration has no */
/*                 possibilities to define _KERNEL in Kernel only project files (PN has only      */
/*                 one project for Kernel and userland sources - a restriction of the PN          */
/*                 development environment). So we have to mark all Kernel C-files manually by    */
/*                 adding _KERNEL in all the files itself.                                        */
#define _KERNEL
#define INET
/* gh2289n: ##automatically insert project defines here## (end) */

#include <sys/obsd_kernel_param.h>
#include <sys/obsd_kernel_systm.h>   /* hardclock prototype */

#include <sys/obsd_kernel_timetc.h>
#include "obsd_platform_hal.h"


#if HAL_TIMECOUNTER_NEEDED
static void hw_inittimecounter(void);
static struct timecounter haltc_timecounter; 
#endif /* HAL_TIMECOUNTER_NEEDED */



/* --------------- normally in arch\i386\isa\clock.c) --------------------- */
int clockintr(void * arg)  
{
	struct clockframe *frame = arg;		/* not strictly necessary */

/*	if (timecounter->tc_get_timecount == i8254_get_timecount) 
	{
		if (i8254_ticked) 
		{
			i8254_ticked = 0;
		} 
		else 
		{
			i8254_offset += rtclock_tval;
			i8254_lastcount = 0;
		}
	}
*/
	hardclock(frame);
	return (1);
}


void hw_initclocks(void)
{
    HAL_cpu_initclocks();

    /* <TBD> later we could connect the clockintr to the clock event/interrupt here (like done in the original i8254_initclocks) */
    /*(void)isa_intr_establish(NULL, 0, IST_PULSE, IPL_CLOCK, clockintr, 0, "clock"); */

#if HAL_TIMECOUNTER_NEEDED
    hw_inittimecounter();
#endif /* HAL_TIMECOUNTER_NEEDED */
}

void hw_deinitclocks(void)
{
#if HAL_TIMECOUNTER_NEEDED
    /* hw_deinittimecounter(); currently nothing to do */
#endif /* HAL_TIMECOUNTER_NEEDED */

    HAL_cpu_deinitclocks();
}


#if HAL_TIMECOUNTER_NEEDED
static u_int haltc_get_timecount(struct timecounter *tc)
{
    OBSD_UNUSED_ARG(tc);
    return HAL_get_tc_timecount();
}


static void hw_inittimecounter(void)
{
	/*(hatltc_get_timecount, NULL, ~0u, TIMER_FREQ, "haltc", 0, NULL)*/

    memset((void *) &haltc_timecounter, 0, sizeof(struct timecounter));
    haltc_timecounter.tc_get_timecount = haltc_get_timecount;       /* This function reads the counter              */
    haltc_timecounter.tc_poll_pps      = NULL; 
    haltc_timecounter.tc_counter_mask  = HAL_get_tc_counter_mask();   /* 32 bit, This mask should mask off any unimplemented bits. */
    haltc_timecounter.tc_frequency     = HAL_get_tc_frequency();    /* Frequency of the counter in Hz.              */
    haltc_timecounter.tc_name          = "haltc";                   /* Name of the timecounter.                     */
    haltc_timecounter.tc_quality       = 0;                         /* determines if this tc is better than another tc, higher means better. */
    haltc_timecounter.tc_priv          = NULL;                      /* Pointer to the timecounter's private parts.  */
    haltc_timecounter.tc_next          = NULL;                      /* Pointer to the next timecounter.             */
    haltc_timecounter.tc_freq_adj      = 0;                         /* Current frequency adjustment.                */

    tc_init(&haltc_timecounter);
}
#endif /* HAL_TIMECOUNTER_NEEDED */

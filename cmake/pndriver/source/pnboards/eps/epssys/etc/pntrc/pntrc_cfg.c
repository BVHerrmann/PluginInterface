/*****************************************************************************/
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
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_cfg.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implements system integration of LSA-component PNTRC                     */
/*                                                                           */
/*****************************************************************************/

#define PNTRC_MODULE_ID    20101
#define LTRC_ACT_MODUL_ID  20101

#include "eps_plf.h" 
#include "eps_sys.h"
#include "eps_locks.h"
#include "eps_tasks.h"
#include "eps_shm_if.h"
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"
#include "pntrc_cfg.h"
#include "pntrc_usr.h"
#include "pntrc_sys.h"
#include "pntrc_int.h"
#include "eps_register.h"

#ifdef EPS_CFG_STATISTIC_TRACE
#include "eps_statistics.h"
#endif

#ifndef EPS_PLF
#error "EPS_PLF is not defined"
#endif

// Add other platforms here if a trace timer driver exists. Remove #if as soon as all platforms have a trace timer driver.
#if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15) 
#include "eps_trace_timer_if.h"
#endif

#if ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9) || (EPS_PLF == EPS_PLF_PNIP_A53) || (EPS_PLF == EPS_PLF_ARM_CORTEX_A15))
#include "trc_infra.h"   //for handling of ERTEC200 and ERTEC400, A53, A15 trace timers
#endif
#if (EPS_PLF == EPS_PLF_ADONIS_X86)
#include "trc_infra.h"
#include "ttu.h"
#endif

#if(EPS_PLF == EPS_PLF_LINUX_SOC1)
#include <stdio.h>
#endif

#if defined(EPS_USE_RTOS_ADONIS)
#include "eps_plf.h"
#endif

//#define EPS_CFG_STATISTIC_TRACE             // Trace statistic values

#ifdef EPS_CFG_STATISTIC_TRACE 
#if (EPS_CFG_USE_STATISTICS == 0)
#error "Use EPS_CFG_STATISTIC_TRACE only in combination with EPS_CFG_USE_STATISTICS == 1!!!"
#endif
#endif

/*----------------------------------------------------------------------------*/
static LSA_UINT32 hThread;
//lint -esym(844, g_pPntrcShmHwPtr) Pointer variable 'g_pPntrcShmHwPtr' could be declared as pointing to const - no, it's changed during the runtime of the program
static EPS_SHM_HW_PTR_TYPE g_pPntrcShmHwPtr = LSA_NULL;
#if defined(EPS_USE_RTOS_ADONIS)
//TODO SMP spinlock for each core (see comments in PNTRC_GET_CATEGORY)
//static pthread_spinlock_t pntrc_trace_spinlock[ADN_SYS_SMP_CORE_NR];
static pthread_spinlock_t pntrc_trace_spinlock;
#else
static LSA_UINT16 pntrc_trace_enter_exit_id     = EPS_LOCK_ID_INVALID;
#endif

typedef struct
{
    LSA_UINT16 pntrc_service_enter_exit_id;
    LSA_UINT64 pntrc_timer_start_value;
    #if (EPS_PLF == EPS_PLF_WINDOWS_X86)
    LARGE_INTEGER freq;
    LARGE_INTEGER base;
    #endif
}EPS_PNTRC_TYPE;

static EPS_PNTRC_TYPE g_Eps_pntrc = {0};

// TTU Timer on SOC1, offset to time value. See TTU-spec
#if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
#define SOC1_OFFSET_TTU_LOW    0x20
#define SOC1_OFFSET_TTU_HIGH   0x28
#endif

// TTU Timer on EB200P, offset to time value. See TTU-spec
#if (EPS_PLF == EPS_PLF_PNIP_ARM9)
#define EB200P_OFFSET_TTU_LOW  0x8
#define EB200P_OFFSET_TTU_HIGH 0xC
#endif

// Local timer on ERTEC400, offset to time value. See ERTEC spec
// currently not used: #define ERTEC400_OFFSET_TTU_LOW 0x1C

// Local timer on ERTEC400, offset to time value. See ERTEC spec
// currently not used: #define ERTEC200_OFFSET_TTU_LOW 0x1C

#ifdef EPS_CFG_STATISTIC_TRACE
static LSA_UINT32 statistics_id;
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_READ_LOWER_CPU
/**
 * Reads a memory region from the shared memory of lower CPU and stores the values into the given pointer
 * 
 * @param [in] hLowerDev    - Handle to the lower CPU
 * @param [in] Offset       - Offset within the shared memory
 * @param [in] size         - size of the memory region
 * @param [inout]ptr        - buffer used as DST
 * @return
 */
LSA_UINT16 PNTRC_READ_LOWER_CPU(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT32 Offset, LSA_UINT32 size, LSA_UINT8* ptr)
{
	LSA_UINT16 result;
	EPS_PNDEV_HW_PTR_TYPE eps_pndev_hw;
	EPS_ASSERT(hLowerDev != LSA_NULL);

	eps_pndev_hw = (EPS_PNDEV_HW_PTR_TYPE)EPS_CAST_TO_VOID_PTR(hLowerDev);
	EPS_ASSERT(eps_pndev_hw->ReadTraceData != LSA_NULL);
	result=(*eps_pndev_hw->ReadTraceData)(eps_pndev_hw,Offset,ptr,size);
	return result;
}
#endif

#ifndef PNTRC_WRITE_LOWER_CPU
/**
 * Writes the content of a buffer into the shared memory of the lower CPU.
 * 
 * @param [in] hLowerDev    - handle to the lower CPU
 * @param [in] ptr          - buffer used as SRC
 * @param [in] Offset       - offset within the shared memory
 * @param [in] size         - size of the buffer
 * @return
 */
LSA_UINT16 PNTRC_WRITE_LOWER_CPU(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 const* ptr, LSA_UINT32 Offset, LSA_UINT32 size)
{
	LSA_UINT16 result;
	EPS_PNDEV_HW_PTR_TYPE eps_pndev_hw;
	EPS_ASSERT(hLowerDev != LSA_NULL);

	eps_pndev_hw = (EPS_PNDEV_HW_PTR_TYPE)EPS_CAST_TO_VOID_PTR(hLowerDev);
	EPS_ASSERT(eps_pndev_hw->WriteTraceData != LSA_NULL);
	result=(*eps_pndev_hw->WriteTraceData)(eps_pndev_hw,ptr,Offset,size);
	return result;
}
#endif

#ifndef PNTRC_BUFFER_FULL
/**
 * PNTRC uses this function to signal, that a given buffer is full
 * 
 * @param [in] SysHandle    - System handle
 * @param [in] BufferID     - Unique buffer ID
 * @param [in] pBuffer      - Pointer to the pntrc buffer
 * @param [in] Size         - Size of the buffer
 * @return 0    - Storage of buffer was successfull
 * @return != 0 - Storage was not successfull
 */
LSA_UINT16 PNTRC_BUFFER_FULL(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID, LSA_UINT8* pBuffer, LSA_UINT32 Size)
{
    if(g_pPntrcShmHwPtr == LSA_NULL) 
    {
        return 0; // startup, no SHM instance allocated -> drop traces
    }
    else
    {
        EPS_ASSERT(g_pPntrcShmHwPtr->TraceBufferFull != LSA_NULL);
        g_pPntrcShmHwPtr->TraceBufferFull(SysHandle, (LSA_UINT32) BufferID, pBuffer, Size);
        return 0;
    }
}
#endif

/**
 * Cyclic thread function that calls pntrc_service
 * @param [in] uParam   - not used, but required by API (we do not have multiple PNTRC instances within one firmware)
 * @param [in] pArg     - not used, but required by API
 * @return
 */
static LSA_VOID pntrc_service_proc( LSA_UINT32 uParam, LSA_VOID * pArg )
{
	LSA_UNUSED_ARG(uParam);
	LSA_UNUSED_ARG(pArg);

	EPS_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "pntrc_service_proc");

    #ifdef EPS_CFG_STATISTIC_TRACE
    eps_statistics_capture_start_value(statistics_id);
    #endif

    #if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
	pntrc_force_buffer_switch();
    #endif
    pntrc_service();
    
#ifdef EPS_CFG_STATISTIC_TRACE
    eps_statistics_capture_end_value(statistics_id);    
#endif
}

#if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1) 

	///< map this address to get the counter value in user mode (see trc_infra.c/h)
	#define TTU_DRV_MMAP_ADDRESS				    0x16000000

	///< base address of module TTU before configuration -> 0x16000000 + 0xA0000000
	#define TTU_BASE_ADDRESS		                0x16000000
    #define SCRB_BASE_ADDRESS                       0x1FB00000

	///< start address of registers for endian swapper before configuration -> 0x1FB00330 + 0xA0000000
	#define SRCB_START_ENDIAN_SWAPPER_ADDRESS		0x1FB00330

//-- Registers for endian swapper
typedef struct srcb_endian_swap_s{
	uint32_t ctrl_bits_m;
	uint32_t ctrl_bits_sl;
}srcb_endian_swap_t;

//Set bits in endian swapper register
#define SRCB_ACT_BSWAP_TRACETIMER_SL		(1<<12)		///< activate byte lane swapper at module TTU, if mode is big endian

volatile srcb_endian_swap_t *ttu_drv_endian_swap;

//-- Registers of module TTU (Trace Timer Unit)
typedef struct ttu_timer_s{
    uint64_t load_reg;									///< load register for 64-bit-access
    uint32_t load_low_reg_little_endian;				///< load register for low 32-bit, mode: little endian
    uint32_t load_low_reg_big_endian;					///< load register for low 32-bit, mode: big endian
    uint32_t load_high_reg_little_endian;				///< load register for high 32-bit, mode: little endian
    uint32_t load_high_reg_big_endian;					///< load register for high 32-bit, mode: big endian
    uint64_t cnt_read_reg;								///< count register for 64-bit-access
    uint32_t cnt_read_low_reg_little_endian;		    ///< count register for read the low 32-bit, mode: little endian
    uint32_t cnt_read_low_reg_big_endian;				///< count register for read the low 32-bit, mode: big endian
    uint32_t cnt_read_high_reg_little_endian;		    ///< count register for read the high 32-bit, mode: little endian
    uint32_t cnt_read_high_reg_big_endian;			    ///< count register for read the high 32-bit, mode: big endian
    uint32_t adjust_reg_little_endian;					///< adjust register for read the resolution, mode: little endian
    uint32_t adjust_reg_big_endian;						///< adjust register for read the resolution, mode: big endian
    uint32_t control_reg_little_endian;					///< control register for select clock pulse and en/disable timer, mode: little endian
    uint32_t control_reg_big_endian;					///< control register for select clock pulse and en/disable timer, mode: big endian
}pntrc_ttu_timer_t;

#elif (EPS_PLF == EPS_PLF_PNIP_ARM9)

	///< map this address to get the counter value in user mode (see trc_infra.c/h)
	#define TTU_DRV_MMAP_ADDRESS								0x10B00000

	///< base address of module TTU before configuration -> 0x16000000 + 0xA0000000
	#define TTU_BASE_ADDRESS										0x10B00000


typedef struct ttu_timer_s{
	uint32_t load_low_reg_little_endian;				///< load register for low 32-bit, mode: little endian
	uint32_t load_high_reg_little_endian;				///< load register for high 32-bit, mode: little endian
	uint32_t cnt_read_low_reg_little_endian;		    ///< count register for read the low 32-bit, mode: little endian
	uint32_t cnt_read_high_reg_little_endian;		    ///< count register for read the high 32-bit, mode: little endian
	uint32_t adjust_reg_little_endian;					///< adjust register for read the resolution, mode: little endian
	uint32_t control_reg_little_endian;					///< control register for select clock pulse and en/disable timer, mode: little endian
}pntrc_ttu_timer_t;

#elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	extern uint64_t trc_timer_res;
	static uint64_t x86_trc_timer_res;
#endif

#if ((EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_PNIP_ARM9) || (EPS_PLF == EPS_PLF_LINUX_SOC1))

//Set bits in control register
#define TTU_CLK_SELECT_INT_CLOCK_PULSE	(0<<0)		///< select internal clock pulse
#define TTU_COUNT_ENABLE				(1<<1)		///< enable counter
#define TTU_SET_TIMER					(TTU_CLK_SELECT_INT_CLOCK_PULSE | TTU_COUNT_ENABLE)

static volatile uint8_t* pntrc_ttu_counter_address=(void*) TTU_DRV_MMAP_ADDRESS;

#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
static uint8_t* g_pMapTTU = 0;
static uint8_t* g_pMapSCRB = 0;
#endif

/**
 * Helper function to initialize the Trace Timer Unit (TTU)
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_init_ttu(LSA_VOID)
{
    volatile pntrc_ttu_timer_t *ttu_drv_timer;
  
#if (EPS_PLF == EPS_PLF_LINUX_SOC1)

    EPS_PLF_MAP_MEMORY(&g_pMapTTU, TTU_BASE_ADDRESS, 0x100000);
    EPS_PLF_MAP_MEMORY(&g_pMapSCRB, SCRB_BASE_ADDRESS, 0x100000);

    ttu_drv_timer = (pntrc_ttu_timer_t*)g_pMapTTU;
    pntrc_ttu_counter_address= (void*)g_pMapTTU;
    
    /// Set start address for endian swapper register
    ttu_drv_endian_swap = (srcb_endian_swap_t*)(g_pMapSCRB + 0x330);

    /// Deactivate endian swapper
    ttu_drv_endian_swap->ctrl_bits_sl &= ~SRCB_ACT_BSWAP_TRACETIMER_SL;
#else
	/// Set base/start address for module TTU
	ttu_drv_timer = (pntrc_ttu_timer_t*)TTU_BASE_ADDRESS;
	pntrc_ttu_counter_address= (void*) TTU_DRV_MMAP_ADDRESS;
#endif

    #if (EPS_PLF == EPS_PLF_SOC_MIPS)
	/// Set start address for endian swapper register
	ttu_drv_endian_swap = (srcb_endian_swap_t*)SRCB_START_ENDIAN_SWAPPER_ADDRESS;

	/// Deactivate endian swapper
	ttu_drv_endian_swap->ctrl_bits_sl &= ~SRCB_ACT_BSWAP_TRACETIMER_SL;
    #endif

	/// Initialize control-, load low- and load high register with '0'
	ttu_drv_timer->control_reg_little_endian   	= 0x00000000;
	ttu_drv_timer->load_low_reg_little_endian  	= 0x00000000;
	ttu_drv_timer->load_high_reg_little_endian 	= 0x000000;

	/// Set adjust register with 0x01B4E81B for microsecond resolution
#if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
	ttu_drv_timer->adjust_reg_little_endian    	= 0x01B4E81B;
    #elif (EPS_PLF == EPS_PLF_PNIP_ARM9)
	ttu_drv_timer->adjust_reg_little_endian    	= 0x020C49BA;
    #endif
	/// Enable timer and select internal clock pulse as source
	ttu_drv_timer->control_reg_little_endian   |= TTU_SET_TIMER;

}

#endif

/**
 * Platform specific read of the time stamp counter.
 * Since we want the traces to start from the startup of the program, we safe a start time. This time is created when this function is called for the first time.
 * The time is resettet when pntrc_startstop is called. 
 * ERTEC200 and ERTEC400    : Use macro provided by adonis. The lower timestamp is read form a hardware timer, the upper timestamp is done by software. 
 * SOC1 and ERTEC200P       : usage of the TTU on the board. This hw timer provides a lower and higher timestamp.
 * Advancedx86              : Read of the TSC from the core. This value must be divided by a scaler factor x86_trc_timer_res to get a resolution in us.
 * Windows                  : Usage of the Windows API.
 * Linux                    : Usage of the POSIX API.
 * @return  - Current time value in 64 bit format
 */
LSA_UINT64 pntrc_get_trace_timer()
{
    LSA_UINT64 val;
    /// Implementation for ERTEC200P
    #if (EPS_PLF == EPS_PLF_PNIP_ARM9)
    LSA_UINT32 val_l,val_h;

    // read TTU-timer	-> 56 bit timer
    val_l = *((uint32_t*)(pntrc_ttu_counter_address + EB200P_OFFSET_TTU_LOW));
    val_h = *((uint32_t*)(pntrc_ttu_counter_address + EB200P_OFFSET_TTU_HIGH));
    val=((LSA_UINT64)val_h << 32) + val_l;
    /// Implementation for SOC1
    #elif (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
    LSA_UINT32 val_l,val_h;

    // read TTU-timer	-> 56 bit timer
    val_l = *((uint32_t*)(pntrc_ttu_counter_address + SOC1_OFFSET_TTU_LOW));		///< read low  32 bit (offset 0x20)
    val_h = *((uint32_t*)(pntrc_ttu_counter_address + SOC1_OFFSET_TTU_HIGH));		///< read high 24 bit (offset 0x28)
    val=((LSA_UINT64)val_h << 32) + val_l;
    /// Implementation for ERTEC200, ERTEC400, HERA and TI Board
    #elif ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
	LSA_UINT32 val_l,val_h;

	TRC_GET_COUNTER((uint32_t*)&val_h,(uint32_t*)&val_l);
	val=((LSA_UINT64)val_h << 32) + val_l;
    /// Implementation for TI Board
    #elif (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
    // now with Hardware Trace Timer
	val = eps_trace_timer_get_time();
	/// Implementation for HERA Board
    #elif (EPS_PLF == EPS_PLF_PNIP_A53)
    // future use when HERA Trace Timer is implemented:    val = eps_trace_timer_get_time();
	
	// old solution: Keep following code still upper line is activated then delete following lines
    LSA_UINT32 val_l,val_h;

    TRC_GET_COUNTER((uint32_t*)&val_h,(uint32_t*)&val_l);
    val=((LSA_UINT64)val_h << 32) + val_l;
	/// Implementation for x86 / Adonis
    #elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	uint64_t rv;
	__asm__ __volatile__("rdtsc" : "=a" (*((uint32_t*)&rv)), "=d" (*(((uint32_t*)&rv)+1)));
	val = rv; // convert to nanoseconds
	/// Good old windows
    #elif (EPS_PLF == EPS_PLF_WINDOWS_X86)
	LARGE_INTEGER now;
	LSA_UINT32 val_l,val_h;
    BOOL retValFreq, retVatCounter;
	if(g_Eps_pntrc.freq.QuadPart == 0 )
	{
        retValFreq = QueryPerformanceFrequency(&g_Eps_pntrc.freq);
        EPS_ASSERT(0 != retValFreq);
        retVatCounter = QueryPerformanceCounter(&g_Eps_pntrc.base);
        EPS_ASSERT(0 != retVatCounter);
	}
    retVatCounter = QueryPerformanceCounter(&now);
    EPS_ASSERT(0 != retVatCounter);

	now.QuadPart=(now.QuadPart-g_Eps_pntrc.base.QuadPart)*1000000/g_Eps_pntrc.freq.QuadPart;  //return in usec
	val_l =(LSA_UINT32)now.LowPart;
	val_h =(LSA_UINT32)now.HighPart;
	val=((LSA_UINT64)val_h << 32) + val_l;
	/// Implementation for Linux
	#elif ((EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))
    struct timeval tv;
    EPS_POSIX_GETTIMEOFDAY(&tv,NULL);
    val = tv.tv_sec*(uint64_t)1000000+tv.tv_usec;
	#else
	EPS_FATAL("Platform not supported!");
    #endif
	return val;
}
//lint -esym(765, pntrc_get_trace_timer) non static for LINUX SOC1 variants

/**
* Resets the pntrc_timer_start_value back to zero.
* Note: pntrc_timer_start_value shall only be manipulated using this function.
*/
static LSA_VOID pntrc_reset_trace_timer()
{
    g_Eps_pntrc.pntrc_timer_start_value = pntrc_get_trace_timer();
}


/**
 * Initializes the Trace timer
 * @param pInit
 * @return
 */
LSA_VOID PNTRC_INIT_TRACE_TIMER(PNTRC_INIT_TRACE_TIMER_PTR_TYPE pInit)
{
	EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"PNTRC_INIT_TRACE_TIMER");

	#if ((EPS_PLF == EPS_PLF_WINDOWS_X86) || (EPS_PLF == EPS_PLF_LINUX_X86) || (EPS_PLF == EPS_PLF_LINUX_IOT2000))
	LSA_UNUSED_ARG( pInit );  
    #elif ((EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_PNIP_ARM9) || (EPS_PLF == EPS_PLF_LINUX_SOC1))
	pntrc_init_ttu();
    #elif ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
	adn_ertec_prf_timer_init();
    #elif (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
	eps_trace_timer_start();
    #elif (EPS_PLF == EPS_PLF_PNIP_A53)
    // future use when HERA Trace Timer is implemented:    eps_trace_timer_start();
    #elif (EPS_PLF == EPS_PLF_ADONIS_X86)
	int32_t ttu_fd;
	int ret_val;

	ttu_drv_install();
	ttu_fd = open(TTU_NAME, O_RDWR);
	EPS_ASSERT(ttu_fd != -1);
	x86_trc_timer_res=(trc_timer_res*10 + 10); // FIXED, old value was x86_trc_timer_res=trc_timer_res*10.  3200 showed no drift from the clock value that windows used...! 
	pInit->Resolution=x86_trc_timer_res;
	ret_val = close(ttu_fd);
	EPS_ASSERT(ret_val != -1);
	ret_val = ttu_drv_uninstall();	///< Uninstall driver for Trace Timer Unit
	EPS_ASSERT(ret_val == 0);
    #endif
}
/**
 * Undo Initialization of the Trace timer
 * @param
 * @return
 */
LSA_VOID PNTRC_UNDO_INIT_TRACE_TIMER()
{
    EPS_SYSTEM_TRACE_00( 0, LSA_TRACE_LEVEL_CHAT,"PNTRC_UNDO_INIT_TRACE_TIMER");

    #if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
        eps_trace_timer_stop();
    #elif (EPS_PLF == EPS_PLF_PNIP_A53)
        // future use when HERA Trace Timer is implemented:    eps_trace_timer_stop();
    #endif
    #if (EPS_PLF == EPS_PLF_LINUX_SOC1)
        EPS_PLF_UNMAP_MEMORY(g_pMapTTU, 0x100000);
        EPS_PLF_UNMAP_MEMORY(g_pMapSCRB, 0x100000);
    #endif
}

/**
 * Reads the current timestamp.
 * The 64 bit timestamp is returned by splitting the value into two 32 bit values.
 * 
 * This implementation saves the very first time value as a start value (@see pntrc_timer_start_value). 
 * All other timestamps subtract this start value -> The times in the log file start with 0.
 * @param [out] pntrc_tracestamp_lo - lower part of the 64 bit value
 * @param [out] pntrc_tracestamp_hi - upper part of the 64 bit value
 * @return
 */
LSA_VOID PNTRC_GET_TRACESTAMP(LSA_UINT32* pntrc_tracestamp_lo, LSA_UINT32* pntrc_tracestamp_hi)
{
	LSA_UINT64 val;

	val=pntrc_get_trace_timer() - g_Eps_pntrc.pntrc_timer_start_value;
	(*pntrc_tracestamp_hi) = ((val >> 32) & 0xFFFFFFFF); /* main counter high 32 bit */
	(*pntrc_tracestamp_lo) = (val & 0xFFFFFFFF); /* main counter low 32 bit */
}

/*----------------------------------------------------------------------------*/
static LSA_UINT8 PNTRC_Entered[PNTRC_MAX_CATEGORY_COUNT];
/**
 * Starts the PNTRC module. 
 * Start:
 *  (1) Creates a 100 ms thread that calls pntrc_service
 *  (2) Signals PNTRC that the system adaptation is ready
 * Stop:
 *  (A) Stop the thread
 * @param [in] start    - 1: Start, 0: Stop
 * @param [in] pShmHw   - Handle to EPS SHM implementation
 * @return
 */
LSA_VOID pntrc_startstop (LSA_INT start, LSA_VOID* pShmHw)
{
    LSA_UINT32 i;
    if (start)
    {
        EPS_ASSERT(pShmHw != LSA_NULL);

        // reset the start value to the current time stamp value.
        pntrc_reset_trace_timer();

        g_pPntrcShmHwPtr = (EPS_SHM_HW_PTR_TYPE)pShmHw;
        for (i=0; i<PNTRC_MAX_CATEGORY_COUNT; i++) 
        {
            PNTRC_Entered[i]=0;
        }
        
        /// (1) Create a thread, configure statistics module
        #ifdef EPS_CFG_STATISTIC_TRACE
        eps_statistics_alloc_stats(&statistics_id, "Trace");
        #endif
        hThread = eps_tasks_start_poll_thread("EPS_PNTRC_POLL", EPS_POSIX_THREAD_PRIORITY_BELOW_NORMAL, eSchedRR, 100, pntrc_service_proc, 0, LSA_NULL, eRUN_ON_1ST_CORE );
        EPS_ASSERT(hThread != 0);

        #ifndef EPS_CFG_DO_NOT_USE_TGROUPS   
        eps_tasks_group_thread_add ( hThread, EPS_TGROUP_PSI );
        #endif
        
        /// (2) Signal that the system adaptation is ready
        pntrc_set_ready();
    }
    else
    {
        /// (A) Stop the thread
        #ifndef EPS_CFG_DO_NOT_USE_TGROUPS 
        eps_tasks_group_thread_remove(hThread);
        #endif
        eps_tasks_stop_poll_thread(hThread);
        #ifdef EPS_CFG_STATISTIC_TRACE
        eps_statistics_free_stats(statistics_id);
        #endif
    }
}

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_FREE_LOCAL_MEM
/***
 * Implementation for alloc / free
 * @param [out] ret_val_ptr      - return value is stored here. @see eps_mem_free
 * @param [in] local_mem_ptr     - pointer to be freed
 * @return
 */
LSA_VOID  PNTRC_FREE_LOCAL_MEM(
    LSA_UINT16                                      *  ret_val_ptr,
    PNTRC_MEM_PTR_TYPE                              local_mem_ptr
) 
{
	EPS_SYSTEM_TRACE_01( 0, LSA_TRACE_LEVEL_CHAT, "PNTRC_FREE_LOCAL_MEM, local_mem_ptr(0x%08x)", local_mem_ptr);
	*ret_val_ptr=eps_mem_free(local_mem_ptr, LSA_COMP_ID_LTRC, 0);
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_MEMCPY_LOCAL_MEM
/**
 * Implementation for memcopy. @see eps_memcpy
 * @param [out] dest    - Destination
 * @param [in] src      - Source
 * @param [in] count    - number of bytes to copy
 * @return
 */
LSA_VOID PNTRC_MEMCPY_LOCAL_MEM (
    PNTRC_MEM_PTR_TYPE   dest,
    const PNTRC_MEM_PTR_TYPE   src,
	LSA_UINT count
) 
{
	eps_memcpy( dest, src, count );
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/

#ifndef PNTRC_MEMSET_LOCAL_MEM
/**
 * Memset implementation for local memory. @see eps_memset
 * @param [out] dest    - Base pointer of the memory
 * @param [in] val      - value, e.g. 0
 * @param [in] count    - number of bytes
 * @return
 */
LSA_VOID PNTRC_MEMSET_LOCAL_MEM (
	PNTRC_MEM_PTR_TYPE   dest,
	LSA_UINT val,
	LSA_UINT count
) 
{
	eps_memset( dest, (LSA_INT) val, count );
}
#else
#error "by design a function"
#endif


#ifndef PNTRC_STRLEN
/**
 * Implementation for strlen
 * @param [in] str  - pointer to the string
 * @return length of the string
 */
LSA_UINT PNTRC_STRLEN(
	const PNTRC_MEM_PTR_TYPE str
) 
{
	return eps_strlen((const char *)(str));
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/
//lint --esym(750, PNTRC_HELPER_GET_HIGH_VALUE)
#define PNTRC_HELPER_GET_HIGH_VALUE(_val) ((_val >> 32) & 0xFFFFFFFF)
//lint --esym(750, PNTRC_HELPER_GET_LOW_VALUE)
#define PNTRC_HELPER_GET_LOW_VALUE(_val) (_val & 0xFFFFFFFF)

#ifdef PNTRC_CFG_NEW_TIMESYNC_MODE

#ifndef PNTRC_WRITE_SYNC_TIME_LOWER_CPU
/**
 * Writes the timestamp value of the lower CPU into the local trace.
 * @param [in] hLowerDev    - handle of the lower CPU
 * @param [in] lowerCpuNr   - index of the lower CPU
 * @return
 */
LSA_VOID PNTRC_WRITE_SYNC_TIME_LOWER_CPU(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT16 lowerCpuNr)
{
	LSA_UINT16 result;
	EPS_PNDEV_HW_PTR_TYPE eps_pndev_hw;
	EPS_ASSERT(hLowerDev != LSA_NULL);

	eps_pndev_hw = (EPS_PNDEV_HW_PTR_TYPE)EPS_CAST_TO_VOID_PTR(hLowerDev);
	EPS_ASSERT(eps_pndev_hw->WriteSyncTimeLower != LSA_NULL);
	result=(*eps_pndev_hw->WriteSyncTimeLower)(eps_pndev_hw, lowerCpuNr);
    EPS_ASSERT(result == EPS_PNDEV_RET_OK);
}
#endif

#if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_PNIP_ARM9) || (EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9) || (EPS_PLF == EPS_PLF_ADONIS_X86) || (EPS_PLF == EPS_PLF_ARM_CORTEX_A15) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
/**
* This function reads the local time stamp counter without the compensation of the start value and traces it using special trace macros.
*
* This timestamp or parts of it must be readable by the upper CPU.
* Current concept - Do the following traces:
*
* Step 1: SYNC TIME START local_start: %ld %ld - writes the start value. We substract the start-value from our timestamp. Upper instance needs this value -> Parameter local_start: %ld %ld
* Step 2: SYNC TIME LOCAL local time %ld %ld   - writes the local time without the compensation of the start value.
ERTEC200 and ERTEC400: local_time.high can not be read (it's a software time counter).  We trace the info here -> Parameter local_time: %ld %ld
* Step 3: SYNC TIME SCALE local_scaler:%ld %ld - writes the scaler value.
*												This is only required by EPS Advanced variant since the local time is calculated by local time := TSC / scaler.
*												The upper instance can only read the TSC.
*
* The conversion of the timestamp (that means the calculation of the offset) is done by the pntrc converter.
* @return
*/
LSA_VOID PNTRC_WRITE_LOCAL_SYNC_TIME()
{
    LSA_UINT32 local_start_low, local_start_high;
    // declare variables only when necessary
    LSA_UINT32 local_low, local_high;
    LSA_UINT64 dummyAccess;

    // Step 0: Preparation
    // Dummy access to the trace-Timer. If this function is exceuted before the first trace entry, the following traces WILL be the first entries. We'd get a START VALUE = 0.
    dummyAccess = pntrc_get_trace_timer();
    LSA_UNUSED_ARG(dummyAccess);

    // Step 1: local_start. Note: This value should only change if we reset the system or do a reboot of advanced
    local_start_low = PNTRC_HELPER_GET_LOW_VALUE(g_Eps_pntrc.pntrc_timer_start_value);
    local_start_high = PNTRC_HELPER_GET_HIGH_VALUE(g_Eps_pntrc.pntrc_timer_start_value);

    EPS_SYSTEM_TRACE_SYNC_TIME_START(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME START local_start: %ld %ld", local_start_low, local_start_high);

    // Step 2: local_time. Note that this is the time without the correction of the start value
    #if (EPS_PLF == EPS_PLF_SOC_MIPS) || (EPS_PLF == EPS_PLF_LINUX_SOC1)
    {
        /* Upper instance can read local_time.low and local_time.high.*/
        local_low = *((uint32_t*)(pntrc_ttu_counter_address + SOC1_OFFSET_TTU_LOW));       ///< read low  32 bit (offset 0x20)
        local_high = *((uint32_t*)(pntrc_ttu_counter_address + SOC1_OFFSET_TTU_HIGH));      ///< read high 24 bit (offset 0x28)
        EPS_SYSTEM_TRACE_SYNC_TIME_LOCAL(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOCAL local time %ld %ld", local_low, local_high);
    }
    #endif
    #if (EPS_PLF == EPS_PLF_PNIP_ARM9)
    {
        /* Upper instance can read local_time.low and local_time.high. */
        local_low = *((uint32_t*)(pntrc_ttu_counter_address + EB200P_OFFSET_TTU_LOW));
        local_high = *((uint32_t*)(pntrc_ttu_counter_address + EB200P_OFFSET_TTU_HIGH));
        EPS_SYSTEM_TRACE_SYNC_TIME_LOCAL(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOCAL local_time: %ld %ld", local_low, local_high);
    }
    #endif

    #if ((EPS_PLF == EPS_PLF_ERTEC200_ARM9) || (EPS_PLF == EPS_PLF_ERTEC400_ARM9))
    {
        /* local_time.high can not be read by the upper instance (it's a software time counter).
        We trace the info here -> Parameter local_time: %ld %ld (second is local_time.low and can be discarded or used for debug) */
        TRC_GET_COUNTER((uint32_t*)&local_high, (uint32_t*)&local_low);
        EPS_SYSTEM_TRACE_SYNC_TIME_LOCAL(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOCAL local_time: %ld %ld", local_low, local_high);
    }
    #endif

    #if (EPS_PLF == EPS_PLF_ADONIS_X86)
    /* neither the local_time.low nor the local_time.high can be read by the upper instance since we divide the value by the x86_trc_timer_res value.
    We trace the local value, but we need to also divide the factor x86_trc_timer_res in windows...*/
    {
        uint64_t rv;
        uint64_t val;

        __asm__ __volatile__("rdtsc" : "=a" (*((uint32_t*)&rv)), "=d" (*(((uint32_t*)&rv) + 1)));
        val = rv; // convert to nanoseconds

        local_low = PNTRC_HELPER_GET_LOW_VALUE(val); // main counter high 32 bit
        local_high = PNTRC_HELPER_GET_HIGH_VALUE(val); //  main counter low 32 bit
        EPS_SYSTEM_TRACE_SYNC_TIME_LOCAL(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOCAL local_time: %ld %ld", local_low, local_high);
    }
    #endif

    #if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
    {
        LSA_UINT64  l_pntrc_timer_start_value = 0;

        l_pntrc_timer_start_value = eps_trace_timer_get_time();

        local_low = PNTRC_HELPER_GET_LOW_VALUE(l_pntrc_timer_start_value);
        local_high = PNTRC_HELPER_GET_HIGH_VALUE(l_pntrc_timer_start_value);

        EPS_SYSTEM_TRACE_SYNC_TIME_LOCAL(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME LOCAL local_time: %ld %ld", local_low, local_high);
    }
    #endif

    // Step 3: Local Scaler. Note: This value should NEVER change.
    #if (EPS_PLF == EPS_PLF_ADONIS_X86)
    {
        LSA_UINT32 scaler_low, scaler_high;

        // local_scaler. Was 3200 for EPS advanced during the implementation of this functionality.
        scaler_low = PNTRC_HELPER_GET_LOW_VALUE(x86_trc_timer_res); // main counter high 32 bit 
        scaler_high = PNTRC_HELPER_GET_HIGH_VALUE(x86_trc_timer_res); // main counter low 32 bit

        EPS_SYSTEM_TRACE_SYNC_TIME_SCALER(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME SCALE local_scaler:%ld %ld", scaler_low, scaler_high);
    }
    #endif

#if (EPS_PLF == EPS_PLF_ARM_CORTEX_A15)
    {
        EPS_SYSTEM_TRACE_SYNC_TIME_SCALER(0, LSA_TRACE_LEVEL_FATAL, "SYNC TIME SCALE local_scaler:%ld %ld", 20, 0);
    }
#endif

}
#else
/**
* This function is required by PNTRC API, but not used in these platforms.
*/
LSA_VOID PNTRC_WRITE_LOCAL_SYNC_TIME()
{
    EPS_FATAL("This function should not be called since there is no upper instance!"); // This may change for an embedded linux variant.
}
#endif
#endif // PNTRC_CFG_NEW_TIMESYNC_MODE


#ifndef PNTRC_CFG_NEW_TIMESYNC_MODE

/**
 * Legacy time sync mode. Upper Instance writes timestamp, lower instance polls for change, writes upper timestamp here.
 * The jitter is very huge, for new implementations please use the new timesync mode since it's more accurate.
 * @param [in] pntrc_tracestamp_lo  - lower part of the timestamp
 * @param [in] pntrc_tracestamp_hi  - upper part of the timestamp
 * @return
 */
LSA_VOID PNTRC_WRITE_SYNC_TIME(LSA_UINT32 pntrc_tracestamp_lo, LSA_UINT32 pntrc_tracestamp_hi)
{
	EPS_SYSTEM_TRACE_SYNC_TIME(0, LSA_TRACE_LEVEL_FATAL,"SYNC TIME %ld %ld",pntrc_tracestamp_lo,pntrc_tracestamp_hi);
}
#endif

#ifndef PNTRC_ALLOC_CRITICAL_SECTION
/**
 * Lock implementation
 * Adonis:              We need a spinlock since parts of software run in kernel mode.
 * All other platform:  We need a normal critical section (PROTECED mode!)
 * 
 * Note: It is important to either use INHERIT or PROTECTED for the trace locks. Almost all threads use traces. 
 * Since the threads have different pririties, a lower prio thread could block a higher prio thread by holding the trace lock.
 * We decided to use PROTECTED mode since most platforms support these kind of locks.
 * 
 * Note2: Adonis internally only support INHERIT although PROTECED is requested by EPS. 
 * @param LSA_VOID
 * @return
 */
LSA_VOID PNTRC_ALLOC_CRITICAL_SECTION(LSA_VOID)
{
    LSA_UINT16 rsp1;
    #if defined(EPS_USE_RTOS_ADONIS)
    pthread_spin_init(&pntrc_trace_spinlock, 0);
    #else
    LSA_UINT16 rsp;

    rsp = eps_alloc_critical_section_prio_protected( &pntrc_trace_enter_exit_id, LSA_TRUE); // recursive lock, PNTRC service may call ENTER twice
    PNTRC_ASSERT(rsp == LSA_RET_OK);

    #endif
    rsp1 = eps_alloc_critical_section_prio_protected( &g_Eps_pntrc.pntrc_service_enter_exit_id, LSA_TRUE); // recursive lock, PNTRC service may call ENTER twice
    PNTRC_ASSERT(rsp1 == LSA_RET_OK);
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_FREE_CRITICAL_SECTION
/**
 * Lock implementation
 * @param LSA_VOID
 * @return
 */
LSA_VOID PNTRC_FREE_CRITICAL_SECTION(LSA_VOID)
{
    LSA_UINT16 rsp1;
    #if defined(EPS_USE_RTOS_ADONIS)
    pthread_spin_destroy(&pntrc_trace_spinlock);
    #else
    LSA_UINT16 rsp;

    rsp=eps_free_critical_section(pntrc_trace_enter_exit_id);
    PNTRC_ASSERT(rsp == LSA_RET_OK);
    pntrc_trace_enter_exit_id = EPS_LOCK_ID_INVALID;

    #endif
    rsp1=eps_free_critical_section(g_Eps_pntrc.pntrc_service_enter_exit_id);
    PNTRC_ASSERT(rsp1 == LSA_RET_OK);
    g_Eps_pntrc.pntrc_service_enter_exit_id = EPS_LOCK_ID_INVALID;
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/
#ifndef GET_CATEGORY
/**
 * Returns the current category to pntrc. 
 * For EPS, there category is the same as the current execution level. There are two categories:
 * Kernel context: 0 (all drivers as well as the operating system)
 * User context  : 1 (all threads)
 * 
 * For SMP / multicore systems, we need to add more categories here. Currently, there is a restriction within PNTRC 
 * (all firmware instances must use the same number of categories).
 * @param hSysDev
 * @return
 */
LSA_UINT8 PNTRC_GET_CATEGORY(PNTRC_SYS_HANDLE_TYPE hSysDev)
{
	LSA_UNUSED_ARG(hSysDev);
	#if defined(EPS_USE_RTOS_ADONIS)
  	if (adn_get_system_state() == ADN_SYS_STATE_IRQ) {
        //TODO SMP: 
        //Each core needs its own category for interrupt traces,
        //otherwise more than own core could trace at the same time in the same buffer!
        //Currently MAX_CATEGORY_COUNT is a fixed value for all targets, 
        //we need dynamic category counts for targets with different core numbers!
        //current workaround: only logical core 0 traces in interrupts, 
        //interrupt traces of other cores are ignored, 
        //since (adn_get_logical_core_id()*2 + 1)>MAX_CATEGORY_COUNT for other cores!
        return adn_get_logical_core_id()*2 + 1;
  	} else {
        //TODO SMP:
        //Currently all cores use same buffer when in thread context,
        //buffer is protected with spinlock->if own core traces in thread contex all other cores have to wait!
        //When dynamic category count is implemented use different categories for each core!
        return 0; //adn_get_logical_core_id()*2 + 0;
  	}
	#else
	return 0;
	#endif
/*  if (adn_sys_state[0] == ADN_SYS_STATE_BOOT) {
  	return 0;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_RUN) {
  	return 1;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_INIT) {
  	return 2;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_SYS) {
  	return 3;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_IRQ) {
  	return 4;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_DRIVER) {
  	return 5;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_EXC) {
  	return 6;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_SHUTDOWN) {
  	return 7;
  } else if (adn_sys_state[0] == ADN_SYS_STATE_UNKNOWN) {
  	return 8;
  } else {
  	return 9;
  }*/
}
#endif

#ifndef PNTRC_ENTER_TRACE
LSA_UINT32
/**
* Lock implementation for the given category.
* Category = 0: Thread context, so use a spinlock
* Category = 1: Interrupt context, no lock required => no nested irqs
* @param [in] Category - current category
* @return
*/
PNTRC_ENTER_TRACE(LSA_UINT8 Category)
{
#if defined(EPS_USE_RTOS_ADONIS)
    if (adn_get_system_state() != ADN_SYS_STATE_IRQ)
    {
        if (Category == 0)
        {
            pthread_spin_lock(&pntrc_trace_spinlock);
}
        else // disable interrupts when accessing interrupt category in thread context!!! (see pntrc_ForceBufferSwitch)
        {
            EPS_PLF_DISABLE_INTERRUPTS();
        }
    }
#else
    eps_enter_critical_section(pntrc_trace_enter_exit_id);
#endif
    if (PNTRC_Entered[Category] == 1)
    {
        // return 1;
        EPS_FATAL(7);
    }
    PNTRC_Entered[Category] = 1;
    return 0;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_EXIT_TRACE
/**
* Lock implementation.
* @param [in] Category - current category
* @return
*/
LSA_VOID PNTRC_EXIT_TRACE(LSA_UINT8 Category)
{
    PNTRC_Entered[Category] = 0;
#if defined(EPS_USE_RTOS_ADONIS)
    if (adn_get_system_state() != ADN_SYS_STATE_IRQ)
    {
        if (Category == 0)
        {
            pthread_spin_unlock(&pntrc_trace_spinlock);
        }
        else // see PNTRC_ENTER_TRACE
        {
            EPS_PLF_ENABLE_INTERRUPTS();
        }
    }
#else
    eps_exit_critical_section(pntrc_trace_enter_exit_id);
#endif
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_ENTER_SERVICE
LSA_UINT32
/**
 * Lock implementation. Uses a simple critical section.
 * @param LSA_VOID
 * @return
 */
PNTRC_ENTER_SERVICE(LSA_VOID)
{
	eps_enter_critical_section( g_Eps_pntrc.pntrc_service_enter_exit_id );
	return 0;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_EXIT_SERVICE
/**
 * Lock implementation.
 * @param LSA_VOID
 * @return
 */
LSA_VOID PNTRC_EXIT_SERVICE(LSA_VOID	)
{
	eps_exit_critical_section( g_Eps_pntrc.pntrc_service_enter_exit_id );
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_FATAL_ERROR
/**
 * PNTRC calls this function to signal a FATAL error.
 * This function is mapped to a PSI implementation.
 * @param [in] length           - length of the error structure
 * @param [in] error_ptr        - pointer to the error strcture
 * @return
 */
LSA_VOID  PNTRC_FATAL_ERROR (
    LSA_UINT16                   length,
    PNTRC_FATAL_ERROR_PTR_TYPE     error_ptr
    )
{
	PSI_FATAL_ERROR ("pntrace", length, error_ptr);
}
#else
#error "by design a function"
#endif

/*----------------------------------------------------------------------------*/
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @return LSA_TRUE
 */
#ifndef PNTRC_TRACE_OUT_00
    LSA_BOOL PNTRC_TRACE_OUT_00 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef
    )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_01
LSA_BOOL PNTRC_TRACE_OUT_01 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(a1);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_02
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_02 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
   )
{
	LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(a1);
    LSA_UNUSED_ARG(a2);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_03
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_03 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(a1);
    LSA_UNUSED_ARG(a2);
    LSA_UNUSED_ARG(a3);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_04
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_04 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(a1);
    LSA_UNUSED_ARG(a2);
    LSA_UNUSED_ARG(a3);
    LSA_UNUSED_ARG(a4);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_05
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_05 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5
   )
{
	LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_06
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_06 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_07
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_07 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_08
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_08 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_09
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_09 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_10
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_10 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif


#ifndef PNTRC_TRACE_OUT_11
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_11 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_12
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @param [in] a12
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_12 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif


#ifndef PNTRC_TRACE_OUT_13
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @param [in] a12
 * @param [in] a13
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_13 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13
   )
{
	LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @param [in] a12
 * @param [in] a13
 * @param [in] a14
 * @return LSA_TRUE
 */
#ifndef PNTRC_TRACE_OUT_14
LSA_BOOL PNTRC_TRACE_OUT_14 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @param [in] a12
 * @param [in] a13
 * @param [in] a14
 * @param [in] a15
 * @return LSA_TRUE
 */
#ifndef PNTRC_TRACE_OUT_15
LSA_BOOL PNTRC_TRACE_OUT_15 (
   LTRC_SUBSYS_TYPE       SubSys,
   PNTRC_LEVEL_TYPE       Level,
   LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
   LSA_UINT32             ModulId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
   LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
   LSA_UINT32             Line,
#endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14,
   LSA_UINT32             a15
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	LSA_UNUSED_ARG(a15);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif


#ifndef PNTRC_TRACE_OUT_16
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @param [in] a3
 * @param [in] a4
 * @param [in] a5
 * @param [in] a6
 * @param [in] a7
 * @param [in] a8
 * @param [in] a9
 * @param [in] a10
 * @param [in] a11
 * @param [in] a12
 * @param [in] a13
 * @param [in] a14
 * @param [in] a15
 * @param [in] a16
 * @return LSA_TRUE
 */
LSA_BOOL PNTRC_TRACE_OUT_16 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   LSA_UINT32             a1,
   LSA_UINT32             a2,
   LSA_UINT32             a3,
   LSA_UINT32             a4,
   LSA_UINT32             a5,
   LSA_UINT32             a6,
   LSA_UINT32             a7,
   LSA_UINT32             a8,
   LSA_UINT32             a9,
   LSA_UINT32             a10,
   LSA_UINT32             a11,
   LSA_UINT32             a12,
   LSA_UINT32             a13,
   LSA_UINT32             a14,
   LSA_UINT32             a15,
   LSA_UINT32             a16
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
	LSA_UNUSED_ARG(TracepointRef);
	LSA_UNUSED_ARG(a1);
	LSA_UNUSED_ARG(a2);
	LSA_UNUSED_ARG(a3);
	LSA_UNUSED_ARG(a4);
	LSA_UNUSED_ARG(a5);
	LSA_UNUSED_ARG(a6);
	LSA_UNUSED_ARG(a7);
	LSA_UNUSED_ARG(a8);
	LSA_UNUSED_ARG(a9);
	LSA_UNUSED_ARG(a10);
	LSA_UNUSED_ARG(a11);
	LSA_UNUSED_ARG(a12);
	LSA_UNUSED_ARG(a13);
	LSA_UNUSED_ARG(a14);
	LSA_UNUSED_ARG(a15);
	LSA_UNUSED_ARG(a16);
	return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_BYTE_ARRAY
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] DataPtr
 * @param [in] DataLen
 * @return LSA_TRUE
*/
LSA_BOOL PNTRC_TRACE_OUT_BYTE_ARRAY (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
   LSA_UINT32             TracepointRef,
   const LSA_UINT8 *      DataPtr,
   LSA_UINT16             DataLen
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(DataPtr);
    LSA_UNUSED_ARG(DataLen);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_STRING
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] St
 * @return LSA_TRUE
*/
LSA_BOOL PNTRC_TRACE_OUT_STRING (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    const LSA_CHAR *       St
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(St);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

#ifndef PNTRC_TRACE_OUT_SYNC_TIME
/**
 * Required by API. Not used.
 * @param [in] SubSys
 * @param [in] Level
 * @param [in] Idx
 * @param [in] TracepointRef
 * @param [in] a1
 * @param [in] a2
 * @return LSA_TRUE
*/
LSA_BOOL PNTRC_TRACE_OUT_SYNC_TIME (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModulId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
   )
{
    LSA_UNUSED_ARG(SubSys);
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModulId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    LSA_UNUSED_ARG(TracepointRef);
    LSA_UNUSED_ARG(a1);
    LSA_UNUSED_ARG(a2);
    return LSA_TRUE;
}
#else
#error "by design a function"
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

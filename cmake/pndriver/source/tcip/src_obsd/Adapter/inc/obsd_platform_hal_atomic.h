/**
 * @file        obsd_platform_hal_atomic.h
 *
 * @brief       HAL file that implements atomic bit operations on memory
 *
 * @author      Hendrik Gerlach ATS1
 *
 * @version     V0.03
 * @date        created: 20.08.2013
 */

#ifndef __OBSD_PLATFORM_HAL_ATOMIC_H__
#define __OBSD_PLATFORM_HAL_ATOMIC_H__

/**
    @brief          Perform atomic operation on memory: Set bits.
    @details        Should be atomic with respect to task switching (and later with respect to
                    multiple processors too)
    @note           Currently the whole stack is not multiprocessor enabled and so there is no 
                    urgent need to be multiprocessor safe here
    @param[in/out]  mem_ptr: address of the memory/variable where to set bits
    @param[in]      bits:    bits to set
    @return         no return value, but changed bits on memory
*/
static __inline void HAL_atomic_setbits_int(volatile u_int32_t *mem_ptr, unsigned long bits)
{
    /* The original OpenBSD code requires: "...be atomic with respect to interrupts and           */
    /* multiple processors." OpenBSD interrupts are comparable to thread switches in our system.  */
    /* Currently we have no deeper analysis, if the thread switch protection is really needed in  */
    /* our case with the reduced codebase.                                                        */
    /* Currently we use a very simple implementation here that might be (depending from the       */
    /* compiler and optimization level) not atomic / thread safe but that should be suited for    */
    /* our case.                                                                                  */
    /* (note: so far no problems are known using this simple implementation with win32 + MSVC6    */
    /*        but a deeper analysis regarding the need of the thread switch protection would      */
    /*        require much more time)                                                             */
    *mem_ptr |= bits;
}

/**
    @brief          Perform atomic operation on memory: Reset/clear bits.
    @details        Should be atomic with respect to task switching (and later with respect to
                    multiple processors too)
    @note           Currently the whole stack is not multiprocessor enabled and so there is no 
                    urgent need to be multiprocessor safe here
    @param[in/out]  mem_ptr: address of the memory/variable where to reset bits
    @param[in]      bits:    bits to reset
    @return         no return value, but changed bits on memory
*/
static __inline void HAL_atomic_clearbits_int(volatile u_int32_t *mem_ptr, unsigned long bits)
{
    /* very simple implemention for Win32. See the implementation comment within                  */
    /* HAL_atomic_setbits_int                                                                     */
    *mem_ptr &= ~bits;
}


#endif /* __OBSD_PLATFORM_HAL_ATOMIC_H__ */


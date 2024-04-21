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
/*  C o m p o n e n t     &C: EPS                                       :C&  */
/*                                                                           */
/*  F i l e               &F: eps_tt_a15_drv.c                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Trace Timer A15					                                     */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID  20078
#define EPS_MODULE_ID      LTRC_ACT_MODUL_ID /* EPS_MODULE_ID_EPS_XX */

/** Includes for the Trace Timer Interface
 */
#include <eps_rtos.h>           /* OS for Thread Api (open(), ...)  */
#include <eps_sys.h>            /* Types / Prototypes / Funcs       */
#include <eps_trc.h>            /* Tracing                          */
#include <eps_register.h>       /* Register Access Macros           */
#include <eps_tt_a15_drv.h>     /* Trace Timer                      */
#include <eps_trace_timer_if.h> /* Trace Timer Interface            */
#include <eps_pndrvif.h>        /* Defines ex. EPS_PN_DRV_RET_OK    */
#include <am5728_tt_drv.h>      /* Kernel Mode Driver AM5728        */


/** Global definitions for the Trace Timer
 */
EPS_TT_A15_DRV_STORE_TYPE        g_EpsTtA15Drv;
EPS_TT_A15_DRV_STORE_PTR_TYPE    g_pEpsTtA15Drv = LSA_NULL;


/** Implementation (functions) of the a15 Trace Timer for AM5728
 */
//----------------------------------------------------------------------------------------------------
/**
 * Registers the AM5728 Trace Timer and the usage functions in Trace Timer Interface administration 
 * 
 * @param  LSA_VOID
 * @return LSA_UINT16   - 0 is ok
 */
LSA_UINT16 eps_tt_a15_drv_install ()
{
    /// locals
    LSA_UINT16  retVal = 0;     
    EPS_TRACE_TIMER_IF_TYPE     l_EpsTraceTimerIf;
    EPS_TRACE_TIMER_IF_PTR_TYPE l_pTraceTimerIf = LSA_NULL;

        
    /// ? is Trace Timer a15 already in use ?
    if (g_pEpsTtA15Drv != LSA_NULL)
    {
        EPS_FATAL("eps_tt_a15_drv_install(): is called but tt a15 is already in use");
    }
    else
    {
        /// a15 Trace Timer Interface is unused
        
        /// init local Instance for Trace Timer Interface
        l_pTraceTimerIf = &l_EpsTraceTimerIf;
 
        l_pTraceTimerIf->start     = eps_tt_a15_drv_start;
        l_pTraceTimerIf->stop      = eps_tt_a15_drv_stop;
        l_pTraceTimerIf->uninstall = eps_tt_a15_drv_uninstall;
        l_pTraceTimerIf->get_time  = eps_tt_a15_drv_get_time;
        
        /// register at EPS Trace Timer Interface
        eps_trace_timer_if_register (l_pTraceTimerIf);

        /// Init pointer to Global Instance of a15 tt -> now a15 tt is in use
        g_pEpsTtA15Drv = &g_EpsTtA15Drv;
 
        /// install AM5728 trace timer driver      
        if (am5728_tt_drv_install() != EPS_PN_DRV_RET_OK)
        {
                EPS_FATAL("eps_tt_a15_drv_install(): Installing AM5728 Trace Timer Driver failed");
        }
        /// open the AM5728 Driver
        g_pEpsTtA15Drv->fd = open(AM5728_TT_DRV_NAME, O_RDWR);
        EPS_ASSERT(g_pEpsTtA15Drv->fd != -1);
    }
    
    return (retVal);
}

//----------------------------------------------------------------------------------------------------
/**
 * Uninstalls the AM5728 Trace Timer 
 * 
 * @param  LSA_VOID
 * @return LSA_UINT16   - 0 is ok
 */
LSA_UINT16 eps_tt_a15_drv_uninstall ()
{
    /// locals
    LSA_UINT16  retVal = 0;     
 
    
    /// ? is Trace Timer A15 in use ?
    if (g_pEpsTtA15Drv != LSA_NULL)
    {
        /// ? is file handle == 0 ?
        if(g_pEpsTtA15Drv->fd == -1)
        {
            EPS_FATAL("eps_tt_a15_drv_uninstall(): fd == -1");
        }
        else
        {
            /// close AM5728 Trace Timer Driver
            close(g_pEpsTtA15Drv->fd);
        }

        /// uninstall AM5728 Trace Timer Driver
        if (am5728_tt_drv_uninstall() != EPS_PN_DRV_RET_OK)
        {
                EPS_FATAL("eps_tt_a15_drv_uninstall(): Uninstalling AM5728 Trace Timer Driver failed");
        }
    
        /// release Trace Timer A15
        g_pEpsTtA15Drv = LSA_NULL;

    }
    else
    {
        EPS_FATAL("eps_tt_a15_drv_uninstall() is called but is not in use");
    }
    
    return (retVal);
}

//----------------------------------------------------------------------------------------------------
/**
 * Starts the AM5728 Trace Timer 
 * 
 * @param  [out] pbIs64Bit   - returns if timer is 64 Bit (=LSA_TRUE) or 32 Bit (=LSA_FALSE)
 * @return LSA_UINT16   - 0 is ok
 */
LSA_UINT16 eps_tt_a15_drv_start ()
{
    /// locals
    LSA_UINT16  retVal = 0;     

    /// ? is Trace Timer A15 in use ?
    if (g_pEpsTtA15Drv != LSA_NULL)
    {
        /// bInit = LSA_TRUE
        g_pEpsTtA15Drv->bInit = LSA_TRUE;
        
        /// prepare Mapping of Trace Timer (must be done only once and not again for .._stop, ...)
        retVal = ioctl (g_pEpsTtA15Drv->fd, 
                        EPS_AM5728_TT_DRV_IOCTL_SRV_MMAP_REG_BASE_ADR, 
                        (LSA_VOID*) EPS_TT_A15_TIMER2_BASE_PTR);    
        EPS_ASSERT(retVal == 0);

        /// Mapping of Trace Timer in User-Mode (must be done only once and not again for .._stop, ...)
        g_pEpsTtA15Drv->pTimer = (LSA_UINT32*) mmap ( NULL, 
                                                     EPS_TT_A15_TIMER_SIZE,
                                                     PROT_READ | PROT_WRITE,
                                                     MAP_SHARED,
                                                     g_pEpsTtA15Drv->fd, EPS_AM5728_TT_DRV_GET_PTIMER);
        
        /// ? Mapping successful ?
        if (g_pEpsTtA15Drv->pTimer == MAP_FAILED)
        {
            EPS_FATAL("eps_tt_a15_drv_start(): mapping of Timer RegisterBaseAdr failed");
        }
        else
        {
            /// read address of TimeHigh-part
            g_pEpsTtA15Drv->pTimeHigh = (LSA_UINT32*) mmap ( NULL, 
                                                             EPS_TT_A15_TIMER_SIZE,
                                                             PROT_READ | PROT_WRITE,
                                                             MAP_SHARED,
                                                             g_pEpsTtA15Drv->fd, EPS_AM5728_TT_DRV_GET_PTIMEHIGH);
            
            if (g_pEpsTtA15Drv->pTimeHigh == 0)
            {
                EPS_FATAL("eps_tt_a15_drv_start(): no address for TimeHigh-part available");
            }
            else
            {
                /// Enable the timer overflow interrupt
                EPS_REG32_WRITE_OFFSET(g_pEpsTtA15Drv->pTimer,
                                       EPS_TT_A15_IRQENABLE_SET,
                                       EPS_TT_A15_SET_OVF_EN_FLAG);

                /// Activate the timer by setting the control register to "Start and Autoreload"
                EPS_REG32_WRITE_OFFSET(g_pEpsTtA15Drv->pTimer,
                                       EPS_TT_A15_TCLR_OFFSET,
                                       EPS_TT_A15_TCLR_START_AUTORELOAD);
            }
        }
    }
    else
    {
        EPS_FATAL("eps_tt_a15_drv_start() is called but is not in use");
    }
    
    return (retVal);
}

//----------------------------------------------------------------------------------------------------
/**
 * Stops the AM5728 Trace Timer 
 * 
 * @param  LSA_VOID
 * @return LSA_UINT16   - 0 is ok
 */
LSA_UINT16 eps_tt_a15_drv_stop ()
{
    /// locals
    LSA_UINT16  retVal = 0;     

    /// ? is Trace Timer A15 in use ?
    if (g_pEpsTtA15Drv != LSA_NULL)
    {
        /// bInit = LSA_FALSE
        g_pEpsTtA15Drv->bInit = LSA_FALSE;
        
        // Stop the timer by setting the control register to "Stop"
        EPS_REG32_WRITE_OFFSET(g_pEpsTtA15Drv->pTimer,
                               EPS_TT_A15_TCLR_OFFSET,
                               EPS_TT_A15_TCLR_STOP);
        
        /// Disable the timer overflow interrupt
        EPS_REG32_WRITE_OFFSET(g_pEpsTtA15Drv->pTimer,
                               EPS_TT_A15_IRQENABLE_CLEAR,
                               EPS_TT_A15_CLEAR_OVF_EN_FLAG);

    }
    else
    {
        EPS_FATAL("eps_tt_a15_drv_stop() is called but is not in use");
    }

    return (retVal);
}

#define EPS_TT_A15_LOW_TIME_MAX             0xffffffff
#define EPS_TT_A15_LOW_TIME_OVF_FINISHED    0x00030D40      // 20d = 1usec ==> 0,01 second (10 msec)

//----------------------------------------------------------------------------------------------------
/**
 * Returns the actual AM5728 Time 
 * 
 * @param  LSA_VOID
 * @return LSA_UINT64       - returns time or 0
 */
LSA_UINT64 eps_tt_a15_drv_get_time ()
{
    LSA_UINT64  currentTime = 0; // sets all bits to 0
    
    LSA_UINT32  lHighTime1=0, lLowTime1=0, lHighTime2=0;
    LSA_UINT32  lIRQStatus=0;
    

    /// ? is Trace Timer A15 in use ?
    if (g_pEpsTtA15Drv != LSA_NULL)
    {
        /// ? Trace Timer in User Mode available ?
        if (g_pEpsTtA15Drv->pTimer == LSA_NULL)
        {
            /// no timer available
            currentTime = 0;        
        }
        else
        {
            
            /// Algorithm for Timer-Overflow!!!
            ///
            ///
            /// Problem:
            /// ========
            ///
            ///                 +---------+--------+
            /// 64 Bit-Timer =  |  High   |  Low   |
            ///                 +---------+--------+
            ///                   32 Bit    32 Bit
            ///
            /// 1.) Low ist durch einen 32 Bit Hardwaretimer (Registerwert) realisiert
            /// 2.) High wird in einer Variablen geführt und durch den TimerOverflow-Interrupt inkrementiert
            /// 3.) Das Auslesen des Low-Teils und des High-Teils kann nicht konsistent erfolgen
            /// 4.) Beide Teile (High und Low) werden sowohl im Taskkontext als auch im Interruptkontext ausgelesen
            /// 5.) Der Interruptkontext kann den TimerOverflow-Interrupt aussperren (Low bereits 0 (Ueberlauf) aber High noch nicht inkrementiert)
            /// 6.) Der Taskkontext kann vom Interruptkontext unterbrochen werden.
            ///
            /// Bem.: Mit der Auflösung des benutzten Timer2 des AM5728 dauert es ca. 3,5 Minuten bis zu einem Ueberlauf. 
            ///
            ///
            /// Ziel:
            /// =====
            ///
            /// Es sollte auch bei einem Ueberlauf der High- und der Low-Wert zusammenpassen!!!
            ///
            ///
            /// Algorithmus:
            /// ============
            ///
            ///     Lese High1
            ///     Lese Low1
            ///
            ///     if ( x < Low1 <= Low1_Max ) // ist Low1 noch vor einem Ueberlauf? (sollte Normalfall sein)
            ///                                 // x: sollte ein kleiner Wert sein, wobei man sich sicher ist, 
            ///                                 //    dass Ueberlauf lange genug vorbei
            ///     {
            ///         return ( High1 + Low1 )
            ///     }
            ///     else
            ///     {
            ///         Lese IRQ_pending // steht ein TimerOverflow-Interrupt an?
            ///         Lese High2
            ///
            ///         if ( IRQ_pending )
            ///         {
            ///             return ( (High1++) + Low1 )
            ///         }
            ///         else
            ///         {
            ///             return ( High2 + Low1 )
            ///         }
            ///     }
            ///
            ///            
            
            
            /// read HighTime 1
            /// read LowTime 1
            lHighTime1 = *g_pEpsTtA15Drv->pTimeHigh;
            lLowTime1  = EPS_REG32_READ_OFFSET(g_pEpsTtA15Drv->pTimer, EPS_TT_A15_TCRR_OFFSET);

            // preset currentTime with LowTime 1
            currentTime = lLowTime1;
            
            /// check if LowTime1 is lower than maximum but greater than a threshold (means long enough after overflow)
            if (      (lLowTime1 >  EPS_TT_A15_LOW_TIME_OVF_FINISHED) 
                  &&  (lLowTime1 <= EPS_TT_A15_LOW_TIME_MAX)  )
            {
                currentTime += (((LSA_UINT64) (lHighTime1)) << 32);
            }
            else /// can be close to overflow
            {
                /// read OVF-IRQ-Status
                lIRQStatus = EPS_REG32_READ_OFFSET(g_pEpsTtA15Drv->pTimer, EPS_TT_A15_IRQSTATUS);

                /// read HighTime 2
                lHighTime2 = *g_pEpsTtA15Drv->pTimeHigh;
                
                /// check if OVF-IRQ pending
                 if ( lIRQStatus & EPS_TT_A15_CLEAR_IRQSTATUS )
                {
                    currentTime += (((LSA_UINT64) (lHighTime1++)) << 32);  
                }
                else
                {
                    currentTime += (((LSA_UINT64) (lHighTime2)) << 32);
                }
            }
        }
    }
    else
    {
        EPS_FATAL("eps_tt_a15_drv_get_time() is called but is not in use");
    }
    
    return (currentTime);
}

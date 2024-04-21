#ifndef EPS_TT_A15_DRV_H_
#define EPS_TT_A15_DRV_H_

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

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
/*  F i l e               &F: eps_tt_a15_drv.h                          :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Trace Timer A15                                                      */
/*                                                                           */
/*****************************************************************************/

    
/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
        
#define EPS_TT_A15_TIMER2_BASE_PTR        0x48032000  /* physical base address of Timer2 register memory */
#define EPS_TT_A15_TIMER_SIZE             0x6C        /* size of Timer2 register memory */

/* Offset to IRQENABLE_SET: Overflow-Interrupt - enable - register */
#define EPS_TT_A15_IRQENABLE_SET          0x2C        
#define EPS_TT_A15_SET_OVF_EN_FLAG        0x2 
    
/* Offset to IRQENABLE_CLEAR: Overflow-Interrupt - disable - register */
#define EPS_TT_A15_IRQENABLE_CLEAR        0x30        
#define EPS_TT_A15_CLEAR_OVF_EN_FLAG      0x2 

/* Offset to IRQSTATUS: Overflow-Interrupt - status - register */
#define EPS_TT_A15_IRQSTATUS              0x28        
#define EPS_TT_A15_CLEAR_IRQSTATUS        0x2 
    
        
/* Offset to TCLR. This register controls optional features specific to the timer functionality. */
#define EPS_TT_A15_TCLR_OFFSET            0x38        

/* Value for TCLR register. 
 Bit 0 (ST) - 1: Start Timer, 
              0: Stop timer: Only the counter is frozen. If one-shotmode selected (AR =0), this bit is automatically reset by
                 internal logic when the counter is overflowed.
 Bit 1 (AR) - 0: One shot timer
              1: Autoreload timer
--> This define sets the bits 0 and 1 to "1" -> Start and Auto Reload
*/
#define EPS_TT_A15_TCLR_START_AUTORELOAD  0x3 
#define EPS_TT_A15_TCLR_STOP              0x0
    
/* Offset to TCRR. Value of TIMER counter */ 
#define EPS_TT_A15_TCRR_OFFSET            0x3C
    
    
/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/


typedef struct eps_pn_tt_a15_drv_store_tag
{
    LSA_BOOL                        bInit;
    LSA_UINT32                     *pTimer;
    volatile LSA_UINT32            *pTimeHigh;  // higher 32 Bit part of 64 Bit Timer
    int                             fd; //File descriptor to kernel mode driver
} EPS_TT_A15_DRV_STORE_TYPE, *EPS_TT_A15_DRV_STORE_PTR_TYPE;    
    

/*****************************************************************************/
/*  Prototypes                                                               */
/*****************************************************************************/

LSA_UINT16 eps_tt_a15_drv_install (LSA_VOID);
LSA_UINT16 eps_tt_a15_drv_uninstall (LSA_VOID);
LSA_UINT16 eps_tt_a15_drv_start (LSA_VOID);
LSA_UINT16 eps_tt_a15_drv_stop (LSA_VOID);
LSA_UINT64 eps_tt_a15_drv_get_time (LSA_VOID);



#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_TT_A15_DRV_H_ */

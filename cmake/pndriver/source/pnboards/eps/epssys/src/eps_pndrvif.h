#ifndef EPS_PNDRVIF_H_              /* ----- reinclude-protection ----- */
#define EPS_PNDRVIF_H_

#ifdef __cplusplus                  /* If C++ - compiler: Use C linkage */
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
/*  F i l e               &F: eps_pndrvif.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  EPS Adonis Driver for PN Devices                                         */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************/
/*  Defines                                                                  */
/*****************************************************************************/
#define EPS_PN_DRV_RET_OK                       0
#define EPS_PN_DRV_RET_ERR_PARAM                1
#define EPS_PN_DRV_RET_ERR_NOT_EXISTS           2
#define EPS_PN_DRV_RET_ERR_UNKNOWN              3

#define EPS_PN_DRV_GATH_IOC_IRQ_ICU_ENABLE		1
#define EPS_PN_DRV_NRT_IOC_IRQ_ICU_ENABLE		2
#define EPS_PN_DRV_GATH_IOC_IRQ_ICU_DISABLE		3
#define EPS_PN_DRV_NRT_IOC_IRQ_ICU_DISABLE		4
#define EPS_PN_DRV_ISOCHRONOUS_IRQ_ICU_ENABLE   5
#define EPS_PN_DRV_ISOCHRONOUS_IRQ_ICU_DISABLE  6
#define EPS_PN_DRV_HIF_SEND_ISR					7
#define EPS_PN_DRV_HIF_ISR_ACK_AND_EOI			8
#define EPS_PN_DRV_CRT_APP_ENABLE				9
#define EPS_PN_DRV_CRT_APP_DISABLE				10
#define EPS_PN_DRV_IRQ_ICU_ENABLE_KERNEL        11  // just for SOC1/EB200P - Enables that the interrupt cbf is done in kernel driver for basic variants
#define EPS_PN_DRV_IRQ_ICU_DISABLE_KERNEL       12  // just for SOC1/EB200P - Disables that the interrupt cbf is done in kernel driver for basic variants
#define EPS_PN_DRV_IRQ_START_EXTTIMER           13  // just for SOC1 - Enables the ExtTimer mode in kernel driver for basic variants
#define EPS_PN_DRV_IRQ_STOP_EXTTIMER            14  // just for SOC1 - Disables the ExtTimer mode in kernel driver for basic variants

/*****************************************************************************/
/*  Includes                                                                 */
/*****************************************************************************/
#include "eps_cp_hw.h"
#include "eps_pn_drv_if.h"

/*****************************************************************************/
/*  Types                                                                    */
/*****************************************************************************/
typedef LSA_VOID (*EPS_PN_DRV_CBF_TYPE) (int32_t param, void *args);
typedef LSA_VOID (*EPS_PN_DRV_TIMER_CBF) (union sigval);

typedef struct
{
	sigset_t                                event;              /* Event to be delivered          */
	pthread_t                               th_id;              /* Thread waiting for Event       */
	uint32_t                                int_src;            /* Int Source delivered to thread */
	EPS_PNDEV_INTERFACE_SELECTOR_TYPE       eInterfaceSelector; /* Interface Selector             */
} EPS_PN_DRV_EVENT_TYPE, *EPS_PN_DRV_EVENT_PTR_TYPE;

typedef struct
{
	EPS_PN_DRV_EVENT_TYPE evnt;
} EPS_PN_DRV_ARGS_TYPE, *EPS_PN_DRV_ARGS_PTR_TYPE;

typedef union
{
	struct
	{
		EPS_PN_DRV_CBF_TYPE                 func_ptr;
		int                                 param;
		void*                               args;
	    EPS_PNDEV_INTERFACE_SELECTOR_TYPE   eInterfaceSelector; /* Interface Selector */
	} crt_app_enable;
	struct
	{
        uint32_t                            eoi_time;
	} hif_isr_ack_eoi;
} EPS_PN_DRV_IOCTL_ARG_TYPE, *EPS_PN_DRV_IOCTL_ARG_PTR_TYPE;

typedef struct eps_pn_drv_callback_tag //this structure must be identical to EPS_PNDEV_CALLBACK_TYPE!
{
    EPS_PN_DRV_CBF_TYPE                     pCbf;
    LSA_UINT32                              uParam;
    LSA_VOID*                               pArgs;
    LSA_UINT32                              lSII_ExtTimerInterval_us;
	EPS_PNDEV_INTERFACE_SELECTOR_TYPE       eInterfaceSelector; /* Interface Selector */
} EPS_PN_DRV_CALLBACK_TYPE, *EPS_PN_DRV_CALLBACK_PTR_TYPE;

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* of EPS_PNDRVIF_H_ */


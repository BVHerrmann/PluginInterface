/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
/* This program is free software; you can redistribute it and/or             */
/* modify it under the terms of the GNU General Public License version 2     */
/* as published by the Free Software Foundation; or, when distributed        */
/* separately from the Linux kernel or incorporated into other               */
/* software packages, subject to the following license:                      */
/*                                                                           */
/*  This program is protected by German copyright law and international      */
/*  treaties. The use of this software including but not limited to its      */
/*  Source Code is subject to restrictions as agreed in the license          */
/*  agreement between you and Siemens.                                       */
/*  Copying or distribution is not allowed unless expressly permitted        */
/*  according to your license agreement with Siemens.                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  P r o j e c t         &P: PROFINET IO Runtime Software              :P&  */
/*                                                                           */
/*  P a c k a g e         &W: PROFINET IO Runtime Software              :W&  */
/*                                                                           */
/*  C o m p o n e n t     &C: PnDev_Driver                              :C&  */
/*                                                                           */
/*  F i l e               &F: pndev_config.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
//
//  D e s c r i p t i o n:
//
//	Declarations for configuration of PnDev_Driver
//
//****************************************************************************/

#ifndef __pndev_config_h__
#define __pndev_config_h__

	//########################################################################
	//  Defines
	//########################################################################

	#ifndef NO
		#define NO		1
	#endif

	#ifndef YES
		#define YES		2
	#endif

	//########################################################################
	//  Configuration
	//########################################################################

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	#define PNDEV_CONNECT_LEG_INT	YES

	//------------------------------------------------------------------------

    //########################################################################
    //  TEMP
    //########################################################################


    #ifndef LSA_UINT64
    #define LSA_UINT64                  uint64_t
    #endif

    #ifndef LSA_UINT32
    #define LSA_UINT32                  uint32_t
    #endif

    #ifndef LSA_UINT16
    #define LSA_UINT16                  uint16_t
    #endif

    #ifndef LSA_UINT8
    #define LSA_UINT8                   uint8_t
    #endif

    #ifndef LSA_BOOL
    #define LSA_BOOL                    LSA_UINT8
    #endif

    #ifndef LSA_FALSE
    #define LSA_FALSE                   ((LSA_BOOL) 0)
    #endif

    #ifndef LSA_TRUE
    #define LSA_TRUE                    (!LSA_FALSE)
    #endif

    #ifndef LSA_CHAR
    #define LSA_CHAR char
    #endif

    #ifndef LSA_UNUSED_ARG
    #define LSA_UNUSED_ARG(arg_)
    #endif

    #ifndef LSA_VOID
    #define LSA_VOID                    void
    #endif

    #ifndef LSA_NULL
    #define LSA_NULL                    0
    #endif

    #ifndef LSA_PREFIX_SIZE
    #define LSA_PREFIX_SIZE                 15    /* including '\0', see *_ver.h */
    #endif

    #ifndef LSA_COMP_ID_PNDEVDRV
    #define LSA_COMP_ID_PNDEVDRV  0x0031 /* PROFINET IO Development Driver for   */
    #endif
            
    #ifndef LSA_CFG_H
    typedef struct lsa_version_tag
    {
        LSA_UINT16  lsa_component_id;               /* LSA_COMP_ID_* */
        LSA_CHAR    lsa_prefix[LSA_PREFIX_SIZE];
        LSA_UINT8   kind;
        LSA_UINT8   version;
        LSA_UINT8   distribution;
        LSA_UINT8   fix;
        LSA_UINT8   hotfix;
        LSA_UINT8   project_number;
        LSA_UINT8   increment;
        LSA_UINT8   integration_counter;
        LSA_UINT8   gen_counter;
    } LSA_VERSION_TYPE;
    #endif

    //########################################################################
    //  TEMP2
    //########################################################################

    #define IRTE_REG_INTERRUPT_REQUEST_REGISTER_IRT_HOST__MSK_INT_NEWCYCLE                                       0x00000001
    #define IRTE_R5_REG_SWITCH_CONTROL__MSK_INTRCTRL_ENABLE                                                      0x00000400
    #define IRTE_R5_REG_SWITCH_CONTROL__MSK_CYCLESYNC_ENABLE                                                     0x00000800

    #define IRTE_R5_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST                                                     0x0001741C
    #define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_HOST                                                        0x00017414
    #define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_HOST                                                    0x00017010
    #define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_HOST                                                        0x0001740C
    #define IRTE_R5_REG_EOI_IRQ1_REGISTER_HOST                                                                  0x00017424
    #define IRTE_R5_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_HP_INT                                             12

    #define IRTE_R5_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH                                                   0x00017444
    #define IRTE_REG_INTERRUPT_ACKN_REGISTER_NRT_SWITCH                                                      0x0001743C
    #define IRTE_REG_INTERRUPT_MASKREGISTER_NRT_IRQ1_SWITCH                                                  0x00017024
    #define IRTE_REG_INTERRUPT_REGISTER_NRT_IRQ1_SWITCH                                                      0x00017434
    #define IRTE_R5_REG_EOI_IRQ1_REGISTER_SWITCH                                                                0x0001744C
    #define IRTE_R5_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_SP_INT                                           13

    #define IRTE_R6_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST                                                     0x0001741C
    #define IRTE_R6_REG_EOI_IRQ1_REGISTER_HOST                                                                  0x00017424
    #define IRTE_R6_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_HP_INT                                             12
    #define IRTE_R6_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH                                                   0x00017444
    #define IRTE_R6_REG_EOI_IRQ1_REGISTER_SWITCH                                                                0x0001744C
    #define IRTE_R6_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_SP_INT                                           13

    #define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST                                                     0x0001741C
    #define IRTE_R7_REG_EOI_IRQ1_REGISTER_HOST                                                                  0x00017424
    #define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_HOST_SHFT_HP_INT                                             12
    #define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH                                                   0x00017444
    #define IRTE_R7_REG_EOI_IRQ1_REGISTER_SWITCH                                                                0x0001744C
    #define IRTE_R7_REG_INTERRUPT_REQUEST_REGISTER_NRT_SWITCH_SHFT_SP_INT                                           13

    #define PNIP_REG_IRQ3EVENT_LOW_1                                                                    0x00043200
    #define PNIP_REG_IRQ3ACK_LOW_1                                                                      0x00043224
    #define PNIP_REG_IRQ3MASK_LOW_1                                                                     0x0004320C
    #define PNIP_REG_IRQ3_LOW_1                                                                         0x00043218
    #define PNIP_REG_IRQ3_EOI_1                                                                         0x00043230

    #define PNIP_REG_IRQ2EVENT_LOW_1                                                                    0x00042200
    #define PNIP_REG_IRQ2ACK_LOW_1                                                                      0x00042224
    #define PNIP_REG_IRQ2MASK_LOW_1                                                                     0x0004220C
    #define PNIP_REG_IRQ2_LOW_1                                                                         0x00042218
    #define PNIP_REG_IRQ2_EOI_1                                                                         0x00042230

    #define PNIP_REG_PHY_CONTROL                                                                        0x00008000
    #define PNIP_R2_REG_PHY_CONTROL__MSK_PHY_ACTIVATE                                                   0x00000001
    #define PNIP_REG_FLOWCONTROL_P1                                                                     0x00001004
    #define PNIP_REG_FLOWCONTROL_P2                                                                     0x00002004
    #define PNIP_REG_MACCONTROL_P1                                                                      0x00001018
    #define PNIP_REG_MACCONTROL_P2                                                                      0x00002018
    #define PNIP_REG_UC_DEFAULT_A                                                                       0x00011004
    #define PNIP_REG_MC_DEFAULT_API                                                                     0x0001200C
    #define PNIP_REG_MC_DEFAULT_P1                                                                      0x00012010
    #define PNIP_REG_MC_DEFAULT_P2                                                                      0x00012014


#endif

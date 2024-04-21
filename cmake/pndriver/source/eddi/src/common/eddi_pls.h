#ifndef EDDI_PLS_H              //reinclude-protection
#define EDDI_PLS_H

#ifdef __cplusplus              //If C++ - compiler: Use C linkage
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
/*  C o m p o n e n t     &C: EDDI (EthernetDeviceDriver for IRTE)      :C&  */
/*                                                                           */
/*  F i l e               &F: eddi_pls.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                              compiler errors                              */
/*===========================================================================*/

/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/* EDD_CFG_MAX_INTERFACE_CNT */
#if (!defined (EDD_CFG_MAX_INTERFACE_CNT)) || (EDD_CFG_MAX_INTERFACE_CNT==0)
#error "EDD_CFG_MAX_INTERFACE_CNT wrong"
#endif

/* EDDI_CFG_MAX_DEVICE */
#if (EDDI_CFG_MAX_DEVICES < 1) || (EDDI_CFG_MAX_DEVICES > 16)
#error "EDDI_CFG_MAX_DEVICES wrong"
#endif


#if defined (EDDI_CFG_ERTEC_400)
#elif defined (EDDI_CFG_ERTEC_200)
#elif defined (EDDI_CFG_SOC)
#else
#error "neither EDDI_CFG_ERTEC_400 nor EDDI_CFG_ERTEC_200 nor EDDI_CFG_SOC defined in eddi_cfg.h!"
#endif

#if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
#if !(defined (EDDI_CFG_SOC) || defined (EDDI_CFG_ERTEC_400)) || !defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
    #error "EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC is only allowed for ET200SP or SCALANCE XF200IRT"
#endif
#endif

#if (EDD_CFG_CSRT_MAX_PROVIDER_GROUP == 0)
//EDD_CFG_CSRT_MAX_PROVIDER_GROUP < 8 if IRTE-HW-support is used in future!
#error "EDD_CFG_CSRT_MAX_PROVIDER_GROUP defined wrong in edd_cfg.h!"
#endif

//#if (EDD_MAX_ARP_FILTER == 0) || (EDD_MAX_ARP_FILTER > 8)
//#error "EDD_MAX_ARP_FILTER defined wrong in edd_usr.h!"
//#endif

#if defined (EDDI_CFG_NRT_TS_TRACE_DEPTH)
#if !defined (EDDI_CFG_ERTEC_400)
#error "EDDI_CFG_NRT_TS_TRACE_DEPTH only allowed together with EDDI_CFG_ERTEC_400!"
#elif (EDDI_CFG_NRT_TS_TRACE_DEPTH == 0)
#error "EDDI_CFG_NRT_TS_TRACE_DEPTH defined wrong!"
#endif
#endif

#if (EDD_FRAME_BUFFER_LENGTH < 1536) 
#error "EDD_FRAME_BUFFER_LENGTH defined wrong in edd_usr.h!"
#endif

#if (EDDI_FRAME_BUFFER_LENGTH < 1536)
#error "EDDI_FRAME_BUFFER_LENGTH defined wrong in eddi_usr.h!"
#endif

/*----------------------------------------------------------------------------*/
/* LSA */
/*===========================================================================*/
/*                              compiler errors                              */
/*===========================================================================*/
#if !defined (EDD_UPPER_IN_FCT_ATTR)
#error "EDD_UPPER_IN_FCT_ATTR not defined in eddi_cfg.h"
#endif

#if !defined (EDD_UPPER_OUT_FCT_ATTR)
#error "EDD_UPPER_OUT_FCT_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDDI_SYSTEM_IN_FCT_ATTR
#error "EDDI_SYSTEM_IN_FCT_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDDI_SYSTEM_OUT_FCT_ATTR
#error "EDDI_SYSTEM_OUT_FCT_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDDI_LOCAL_FCT_ATTR
#error "EDDI_LOCAL_FCT_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDD_UPPER_RQB_ATTR
#error "EDD_UPPER_RQB_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDD_UPPER_MEM_ATTR
#error "EDD_UPPER_MEM_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDDI_SYSTEM_MEM_ATTR
#error "EDDI_SYSTEM_MEM_ATTR not defined in eddi_cfg.h"
#endif

#if !defined EDDI_LOCAL_MEM_ATTR
#error "EDDI_LOCAL_MEM_ATTR not defined in eddi_cfg.h"
#endif

#if !defined LSA_SYS_PATH_TYPE
#error "LSA_SYS_PATH_TYPE not defined in file lsa_cfg.h"
#endif

#if !defined LSA_SYS_PTR_TYPE
#error "LSA_SYS_PTR_TYPE not defined in file lsa_cfg.h"
#endif

#if (!defined (EDDI_CFG_LITTLE_ENDIAN)) && (!defined (EDDI_CFG_BIG_ENDIAN))
#error "neither EDDI_CFG_LITTLE_ENDIAN nor EDDI_CFG_BIG_ENDIAN is defined in eddi_cfg.h"
#endif

//#if (!defined (EDDI_CFG_XRT_OVER_UDP_INCLUDE)) && defined (EDDI_CFG_REV7)
//#error "Combination not possible"
//#endif

#if defined (EDDI_CFG_PORT_CNT)
#error "define EDDI_CFG_PORT_CNT replaced by EDDI_SRV_DEV_OPEN -> see eddi\cfg\eddi_bsp_edd_ini.c -> bsp_eddi_system_dev_open()"
#endif

#if defined (EDDI_ERTEC_FILE_SYSTEM_EXTENSION)
#error "macro EDDI_ERTEC_FILE_SYSTEM_EXTENSION() renamed to EDDI_FILE_SYSTEM_EXTENSION() in eddi_cfg.h"
#endif

#if defined (EDDI_CFG_DO_TIMEOUT_EXTERN)
#error "define EDDI_CFG_DO_TIMEOUT_EXTERN replaced" 
#endif

#if defined (EDDI_CFG_PORT_0_PHY_ADR) || defined (EDDI_CFG_PORT_1_PHY_ADR) || defined (EDDI_CFG_PORT_2_PHY_ADR) || defined (EDDI_CFG_PORT_3_PHY_ADR)
#error "Defines replaced by : eddi_system(EDDI_SRV_DEV_SETUP) -> dsb.GlobPara.PortParams[i].PhyAdr"
#endif

#if defined (EDDI_SYNC_USE_INTERNAL_ALGO)
#error "define EDDI_SYNC_USE_INTERNAL_ALGO -> no longer supported"
#endif

#if defined (EDDI_SYNC_EXT_EVENT)
#error "define EDDI_SYNC_EXT_EVENT -> no longer supported"
#endif

#if defined (EDDI_SYNC_ALGORITHM)
#error "define EDDI_SYNC_ALGORITHM -> no longer supported"
#endif

#if defined (EDDI_SYNC_RESET)
#error "define EDDI_SYNC_RESET -> no longer supported"
#endif

#if !defined (EDDI_SYS_HANDLE)
#error "define EDDI_SYS_HANDLE"
#endif

#if (  (defined (EDDI_CFG_APPLSYNC_SHARED)          && defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)) \
    || (defined (EDDI_CFG_APPLSYNC_SHARED)          && defined (EDDI_CFG_APPLSYNC_SEPARATE))        \
    || (defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) && defined (EDDI_CFG_APPLSYNC_SEPARATE)))
#error "Only one EDDI_CFG_APPLSYNC_xxx-define allowed at one time!"
#endif

#if (!defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)) && (!defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)) && (!defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)) && (!defined (EDDI_CFG_PHY_TRANSCEIVER_TI)) && (!defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC))
#error "neither EDDI_CFG_PHY_TRANSCEIVER_BROADCOM nor EDDI_CFG_PHY_TRANSCEIVER_NEC nor EDDI_CFG_PHY_TRANSCEIVER_NSC nor EDDI_CFG_PHY_TRANSCEIVER_TI nor EDDI_CFG_PHY_TRANSCEIVER_USERSPEC is defined in eddi_cfg.h!"
#endif

#if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
#if !defined (EDDI_CFG_PHY_REGVALUE_OUI_USERSPEC)
#error "EDDI_CFG_PHY_REGVALUE_OUI_USERSPEC is not defined in eddi_cfg.h!"
#endif
#endif

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)
#if !defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
#error "EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM only allowed together with EDDI_CFG_PHY_TRANSCEIVER_BROADCOM!"
#endif
#endif
#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)
#if !defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
#error "EDDI_CFG_PHY_BLINK_EXTERNAL_NEC only allowed together with EDDI_CFG_PHY_TRANSCEIVER_NEC!"
#endif
#endif
#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)
#if !defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
#error "EDDI_CFG_PHY_BLINK_EXTERNAL_NSC only allowed together with EDDI_CFG_PHY_TRANSCEIVER_NSC!"
#endif
#endif
#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)
#if !defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
#error "EDDI_CFG_PHY_BLINK_EXTERNAL_TI only allowed together with EDDI_CFG_PHY_TRANSCEIVER_TI!"
#endif
#endif
#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)
#if !defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
#error "EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC only allowed together with EDDI_CFG_PHY_TRANSCEIVER_USERSPEC!"
#endif
#endif

/*===========================================================================*/
/*                       SII (Standard Interrupt Integration)                */
/*===========================================================================*/

#if (defined (EDDI_CFG_SII_POLLING_MODE) && defined (EDDI_CFG_SII_FLEX_MODE))
#error "both EDDI_CFG_SII_POLLING_MODE and EDDI_CFG_SII_FLEX_MODE defined in eddi_cfg.h!"
#elif (defined (EDDI_CFG_SII_POLLING_MODE) && defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE))
#error "both EDDI_CFG_SII_POLLING_MODE and EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE defined in eddi_cfg.h!"
#elif (defined (EDDI_CFG_SII_FLEX_MODE) && defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE))
#error "both EDDI_CFG_SII_FLEX_MODE and EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE defined in eddi_cfg.h!"
#endif

#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) || defined (EDDI_CFG_APPLSYNC_SEPARATE)
#error "EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE not allowed together with EDDI_CFG_APPLSYNC_xxx in eddi_cfg.h!"
#endif
#endif

#if defined (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION)
#if defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_FLEX_MODE) || defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#error "EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION only allowed together with SII-Standard-Mode!"
#endif
#if (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION < 0) || (EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION > 20)
#error "EDDI_CFG_SII_NRT_RX_CHA0_LIMITATION defined wrong in eddi_cfg.h!"
#endif
#endif

#if defined (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION)
#if defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_FLEX_MODE) || defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#error "EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION only allowed together with SII-Standard-Mode!"
#endif
#if (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION < 0) || (EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION > 20)
#error "EDDI_CFG_SII_NRT_RX_CHB0_LIMITATION defined wrong in eddi_cfg.h!"
#endif
#endif

#if defined (EDDI_CFG_SII_INT_RESTART_MODE_LOOP)
#if defined (EDDI_CFG_SII_POLLING_MODE) || defined (EDDI_CFG_SII_FLEX_MODE) || defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#error "EDDI_CFG_SII_INT_RESTART_MODE_LOOP only allowed together with SII-Standard-Mode!"
#endif
#endif

#if defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
#if defined (EDDI_CFG_SII_POLLING_MODE)
#error "EDDI_CFG_SII_USE_SPECIAL_EOI not allowed together with SII-Polling-Mode!"
#endif
#endif

#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
#if (defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) && defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX))
#error "both EDDI_CFG_SII_EXTTIMER_MODE_ON and EDDI_CFG_SII_EXTTIMER_MODE_FLEX defined in eddi_cfg.h!"
#endif

#if defined (EDDI_CFG_SII_FLEX_MODE)
#error "EDDI_CFG_SII_EXTTIMER_MODE_ON and EDDI_CFG_SII_EXTTIMER_MODE_FLEX not allowed together with SII-Flex-Mode!"
#endif

#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
#error "EDDI_CFG_SII_EXTTIMER_MODE_ON and EDDI_CFG_SII_EXTTIMER_MODE_FLEX not allowed together with SII-New-Cycle-Sync-Mode!"
#endif

#if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON)
#if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED)
#error "EDDI_CFG_SII_EXTTIMER_MODE_ON not allowed together with ApplSyncSharedMode!"
#endif
#endif //EDDI_CFG_SII_EXTTIMER_MODE_ON
#endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX

#if defined (EDDI_CFG_SII_USE_IRQ_HP)
#error "define EDDI_CFG_SII_USE_IRQ_HP -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_USE_IRQ_SP)
#error "define EDDI_CFG_SII_USE_IRQ_SP -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_USE_IRQ_0)
#error "define EDDI_CFG_SII_USE_IRQ_0 -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_USE_IRQ_1)
#error "define EDDI_CFG_SII_USE_IRQ_1 -> no longer supported"
#endif

#if defined (EDDI_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_IN_US)
#error "define EDDI_SII_NEW_CYCLE_SYNC_MODE_NRT_CHECK_IN_US -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_EXTTIMER_INTERVAL_US)
#error "define EDDI_CFG_SII_EXTTIMER_INTERVAL_US -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NEW_OPT_PRIO1_ON)
#error "define EDDI_CFG_SII_NEW_OPT_PRIO1_ON -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NEW_OPT_PRIO2_ON)
#error "define EDDI_CFG_SII_NEW_OPT_PRIO2_ON -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NRT_CHANNEL_A0_ACTIVE)
#error "define EDDI_CFG_SII_NRT_CHANNEL_A0_ACTIVE -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NRT_CHANNEL_B0_ACTIVE)
#error "define EDDI_CFG_SII_NRT_CHANNEL_B0_ACTIVE -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NRT_CHANNEL_B1_ACTIVE)
#error "define EDDI_CFG_SII_NRT_CHANNEL_B1_ACTIVE -> no longer supported"
#endif

#if defined (EDDI_CFG_SII_NRT_RX_CHB1_LIMITATION)
#error "define EDDI_CFG_SII_NRT_RX_CHB1_LIMITATION -> no longer supported"
#endif

#if defined (EDDI_ENTER_SND_CHB1)
#error "define EDDI_ENTER_SND_CHB1 -> no longer supported"
#endif

#if defined (EDDI_ENTER_RCV_CHB1)
#error "define EDDI_ENTER_RCV_CHB1 -> no longer supported"
#endif

#if defined (EDDI_EXIT_SND_CHB1)
#error "define EDDI_EXIT_SND_CHB1 -> no longer supported"
#endif

#if defined (EDDI_EXIT_RCV_CHB1)
#error "define EDDI_EXIT_RCV_CHB1 -> no longer supported"
#endif

#if defined (EDDI_CFG_TIMECOMPARE_HANDLING)
#error "define EDDI_CFG_TIMECOMPARE_HANDLING -> no longer supported"
#endif

/*===========================================================================*/
/*                              derived defines                              */
/*===========================================================================*/
//Provider-buffer-interface
#if defined (EDDI_CFG_REV6) && defined (EDDI_CFG_USE_SW_IIIB_IF_PROV)
#define EDDI_INTCFG_PROV_BUFFER_IF_3BSW    //3-buffer-if in software
#else
#define EDDI_INTCFG_PROV_BUFFER_IF_SINGLE  //single-buffer-if
#endif

//isochronous support
#if defined (EDDI_CFG_APPLSYNC_SHARED) || defined (EDDI_CFG_APPLSYNC_NEWCYCLE_SHARED) || defined (EDDI_CFG_APPLSYNC_SEPARATE)
#define EDDI_INTCFG_ISO_SUPPORT_ON
#endif

//DFP support, small SCF, NRT FRAG support, MC-FDB
#if defined (EDDI_CFG_REV7)
#define EDDI_CFG_DFP_ON
#define EDDI_CFG_SMALL_SCF_ON
#define EDDI_CFG_FRAG_ON
#define EDDI_CFG_DEFRAG_ON
#define EDDI_CFG_ENABLE_MC_FDB
#endif

#if (defined (EDDI_INTCFG_ISO_SUPPORT_ON) && defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE))
#error "EDDI_INTCFG_ISO_SUPPORT_ON and EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE can not simultaneously be defined!"
#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_PLS_H


/*****************************************************************************/
/*  end of file eddi_pls.h                                                   */
/*****************************************************************************/


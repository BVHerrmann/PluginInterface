#ifndef EDDI_TRA_H              //reinclude-protection
#define EDDI_TRA_H

#if defined __cplusplus     //If C++ - compiler: Use C linkage
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
/*  F i l e               &F: eddi_Tra.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/
//PHY register addresses
#define EDDI_PHY_BASIC_CONTROL_REG_ADR                  0   //according IEEE 802.3
#define EDDI_PHY_ID_R1_REG_ADR                          2   //according IEEE 802.3
#define EDDI_PHY_ID_R2_REG_ADR                          3   //according IEEE 802.3
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_REG_ADR   5   //according IEEE 802.3

#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
#define EDDI_PHY_BCM_AUX_MODE2_REG_ADR                  27  //1Bh
#define EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_REG_ADR   28  //1Ch
#define EDDI_PHY_BCM_AUX_MULTI_PHY_REG_ADR              30  //1Eh
#if !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)
#define EDDI_PHY_BCM_AUX_MODE4_REG_ADR                  26  //SHADOW-Register: 1Ah
#define EDDI_PHY_BCM_TEST_REG_ADR                       31  //1Fh
#endif
#endif //EDDI_CFG_PHY_TRANSCEIVER_BROADCOM

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
#define EDDI_PHY_NEC_MODE_CONTROL_STATUS_REG_ADR        17  //11h
#define EDDI_PHY_NEC_SPECIAL_MODES_REG_ADR              18  //12h
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_REG_ADR 31  //1Fh
#endif //EDDI_CFG_PHY_TRANSCEIVER_NEC

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
#define EDDI_PHY_NSC_PHY_STATUS_REG_ADR                 16  //10h
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_REG_ADR          22  //16h
#define EDDI_PHY_NSC_PHY_CONTROL_REG_ADR                25  //19h
#define EDDI_PHY_NSC_10BASET_STATUS_CONTROL_REG_ADR     26  //1Ah
#if !defined (EDDI_CFG_NO_ENHANCED_CRS_DV)
#define EDDI_PHY_NSC_PAGE_SELECT_REG_ADR                19  //13h
#define EDDI_PHY_NSC_PMD_CNFG_REG_ADR                   20  //PAGE1-Register: 14h
#endif
#if !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_REG_ADR         24  //18h
#endif
#endif //EDDI_CFG_PHY_TRANSCEIVER_NSC

#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
#define EDDI_PHY_TI_SW_STRAP_CONTROL1_REG_ADR           9   //9h
//#define EDDI_PHY_TI_SW_STRAP_CONTROL2_REG_ADR         10  //Ah
#define EDDI_PHY_TI_PHY_CONTROL_REG_ADR                 25  //19h
#if !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)
#define EDDI_PHY_TI_LED_CONTROL_REG_ADR                 24  //18h
#endif
#endif //EDDI_CFG_PHY_TRANSCEIVER_TI

/*==========================================================================*/
/*====== PHY 22bit OUI (Organizationally Unique Identifier) ================*/
/*==========================================================================*/
#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
#define EDDI_PHY_BCM_REGVALUE_OUI                       0x001018
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
#define EDDI_PHY_NEC_REGVALUE_OUI                       0x001111
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
#define EDDI_PHY_NSC_REGVALUE_OUI                       0x080017
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
#define EDDI_PHY_TI_REGVALUE_OUI	                    0x080028
#endif

/*==========================================================================*/
/*====== Basic Control Register, 0d, 0h, IEEE 802.3 ========================*/
/*==========================================================================*/
#define EDDI_PHY_BASIC_CONTROL_BIT__Res0                                EDDI_BIT_MASK_PARA( 5, 0)
#define EDDI_PHY_BASIC_CONTROL_BIT__SpeedMSB                            EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_BASIC_CONTROL_BIT__ColTest                             EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_BASIC_CONTROL_BIT__DuplexMode                          EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_BASIC_CONTROL_BIT__RestAutoNeg                         EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_PHY_BASIC_CONTROL_BIT__Isolate                             EDDI_BIT_MASK_PARA(10,10)
#define EDDI_PHY_BASIC_CONTROL_BIT__PowerDown                           EDDI_BIT_MASK_PARA(11,11)
#define EDDI_PHY_BASIC_CONTROL_BIT__AutoNegEnable                       EDDI_BIT_MASK_PARA(12,12)
#define EDDI_PHY_BASIC_CONTROL_BIT__SpeedLSB                            EDDI_BIT_MASK_PARA(13,13)
#define EDDI_PHY_BASIC_CONTROL_BIT__LoopBack                            EDDI_BIT_MASK_PARA(14,14)
#define EDDI_PHY_BASIC_CONTROL_BIT__Reset                               EDDI_BIT_MASK_PARA(15,15)

/*==========================================================================*/
/*====== Basic Status Register, 1d, 1h, IEEE 802.3 =========================*/
/*==========================================================================*/
#define EDDI_PHY_BASIC_STATUS_BIT__ExtCap                               EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_BASIC_STATUS_BIT__JabberDetect                         EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_BASIC_STATUS_BIT__LinkStat                             EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_BASIC_STATUS_BIT__AutoNegAbale                         EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_BASIC_STATUS_BIT__RemoteFault                          EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_PHY_BASIC_STATUS_BIT__AutoNegComp                          EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_BASIC_STATUS_BIT__MfPreampSup                          EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_BASIC_STATUS_BIT__Res0                                 EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_BASIC_STATUS_BIT__ExtStatus                            EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_BASIC_STATUS_BIT__Base100HD                            EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_PHY_BASIC_STATUS_BIT__Base100FD                            EDDI_BIT_MASK_PARA(10,10)
#define EDDI_PHY_BASIC_STATUS_BIT__M10HD                                EDDI_BIT_MASK_PARA(11,11)
#define EDDI_PHY_BASIC_STATUS_BIT__M10FD                                EDDI_BIT_MASK_PARA(12,12)
#define EDDI_PHY_BASIC_STATUS_BIT__Base100XHD                           EDDI_BIT_MASK_PARA(13,13)
#define EDDI_PHY_BASIC_STATUS_BIT__Base100XFD                           EDDI_BIT_MASK_PARA(14,14)
#define EDDI_PHY_BASIC_STATUS_BIT__Base100T4                            EDDI_BIT_MASK_PARA(15,15)

/*==========================================================================*/
/*====== Autoneg Link Partner Ability Register, 5d, 5h, IEEE 802.3 =========*/
/*==========================================================================*/
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__SelField             EDDI_BIT_MASK_PARA( 4, 0)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10THDup          EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base10TFDup          EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXHDup        EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100TXFDup        EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Base100T4            EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Pause                EDDI_BIT_MASK_PARA(10,10)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__AsymPause            EDDI_BIT_MASK_PARA(11,11)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Res0                 EDDI_BIT_MASK_PARA(12,12)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__RemFault             EDDI_BIT_MASK_PARA(13,13)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__Res1                 EDDI_BIT_MASK_PARA(14,14)
#define EDDI_PHY_AUTONEG_LINK_PARTNER_ABILITY_BIT__NextPage             EDDI_BIT_MASK_PARA(15,15)

#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)

/*==========================================================================*/
/*====== Auxiliary Mode 2 Register, 27d, 1Bh, BROADCOM =====================*/
/*==========================================================================*/
#define EDDI_PHY_BCM_AUX_MODE2_BIT__TrafficMeterLEDMode                 EDDI_BIT_MASK_PARA( 6, 6)

/*==========================================================================*/
/*====== Auxiliary Error and General Status Register, 28d, 1Ch, BROADCOM ===*/
/*==========================================================================*/
#define EDDI_PHY_BCM_AUX_ERROR_AND_GEN_STATUS_BIT__MDIXManualSwap       EDDI_BIT_MASK_PARA(12,12)

/*==========================================================================*/
/*====== Auxiliary Mode Register, 29d, 1Dh, BROADCOM =======================*/
/*==========================================================================*/
#define EDDI_PHY_BCM_AUX_MODE_BIT__Res0                                 EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_BCM_AUX_MODE_BIT__BlockTXEN                            EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_BCM_AUX_MODE_BIT__Res1                                 EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_BCM_AUX_MODE_BIT__LinkLEDDisable                       EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_BCM_AUX_MODE_BIT__ActivityLEDDisable                   EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_PHY_BCM_AUX_MODE_BIT__Res2                                 EDDI_BIT_MASK_PARA(15, 5)

/*==========================================================================*/
/*====== Auxiliary Multiple PHY Register, 30d, 1Eh, BROADCOM ===============*/
/*==========================================================================*/
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__Res0                            EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__10Base_T_Serial                 EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__Res1                            EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__SuperIsolate                    EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__AbilityDetect                   EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__AcknowledgeDetected             EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__AcknowledgeComplete             EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__AutonegotiationComplete         EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__RestartAutonegotiation          EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__Res2                            EDDI_BIT_MASK_PARA(10, 9)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__HCD_10Base_T                    EDDI_BIT_MASK_PARA(11,11)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__HCD_10Base_T_FDX                EDDI_BIT_MASK_PARA(12,12)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__HCD_TX                          EDDI_BIT_MASK_PARA(13,13)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__HCD_T4                          EDDI_BIT_MASK_PARA(14,14)
#define EDDI_PHY_BCM_AUX_MULTI_PHY_BIT__HCD_FDX                         EDDI_BIT_MASK_PARA(15,15)

/*==========================================================================*/
/*====== Broadcom Test Register, 31d, 1Fh, BROADCOM ========================*/
/*==========================================================================*/
#define EDDI_PHY_BCM_TEST_BIT__Res0                                     EDDI_BIT_MASK_PARA( 6, 0)
#define EDDI_PHY_BCM_TEST_BIT__ShadowRegEnable                          EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_BCM_TEST_BIT__Res1                                     EDDI_BIT_MASK_PARA(15, 8)

/*==========================================================================*/
/*====== Auxiliary Mode 4 Register (SHADOW-Register), 26d, 1Ah, BROADCOM ===*/
/*==========================================================================*/
#define EDDI_PHY_BCM_AUX_MODE4_BIT__ForceIDDQMode                       EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_BCM_AUX_MODE4_BIT__ForceLPMode                         EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_BCM_AUX_MODE4_BIT__EnClockDuringLP                     EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_BCM_AUX_MODE4_BIT__Res0                                EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_BCM_AUX_MODE4_BIT__ForceLED                            EDDI_BIT_MASK_PARA( 5, 4)
#define EDDI_PHY_BCM_AUX_MODE4_BIT__Res1                                EDDI_BIT_MASK_PARA(15, 6)

#endif //EDDI_CFG_PHY_TRANSCEIVER_BROADCOM

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)

/*==========================================================================*/
/*====== Mode Control/Status Register, 17d, 11h, NEC =======================*/
/*==========================================================================*/
#define EDDI_PHY_NEC_MODE_CONTROL_STATUS_BIT__MDIMode                   EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_NEC_MODE_CONTROL_STATUS_BIT__AutoMDIXen                EDDI_BIT_MASK_PARA( 7, 7)

/*==========================================================================*/
/*====== Special Modes Register, 18d, 12h, NEC =============================*/
/*==========================================================================*/
#define EDDI_PHY_NEC_SPECIAL_MODES_BIT__FX_MODE                         EDDI_BIT_MASK_PARA(10,10)

/*==========================================================================*/
/*====== PHY Special Control/Status Register, 31d, 1Fh, NEC ================*/
/*==========================================================================*/
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Scramble_Disable   EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Res0               EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Speed_Indication   EDDI_BIT_MASK_PARA( 4, 2)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Res1               EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Enable_4B5B        EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Res2               EDDI_BIT_MASK_PARA(11, 7)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Autodone           EDDI_BIT_MASK_PARA(12,12)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Special            EDDI_BIT_MASK_PARA(13,13)
#define EDDI_PHY_NEC_PHY_SPECIAL_CONTROL_STATUS_BIT__Res3               EDDI_BIT_MASK_PARA(15,14)

#endif //EDDI_CFG_PHY_TRANSCEIVER_NEC

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)

/*==========================================================================*/
/*====== PHY Status Register, 16d, 10h, NSC ================================*/
/*==========================================================================*/
#define EDDI_PHY_NSC_PHY_STATUS_BIT__LinkStatus                         EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_NSC_PHY_STATUS_BIT__Speed10                            EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_NSC_PHY_STATUS_BIT__Duplex                             EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_NSC_PHY_STATUS_BIT__AutonegComplete                    EDDI_BIT_MASK_PARA( 4, 4)

/*==========================================================================*/
/*====== PCS Sub-Layer Configuration and Status Register, 22d, 16h, NSC ====*/
/*==========================================================================*/
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__DESCRAM_BYPASS              EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__SCRAM_BYPASS                EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FEFI_EN                     EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__FX_EN                       EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_NSC_PCS_CONFIG_STATUS_BIT__Signal_Detect_Option        EDDI_BIT_MASK_PARA( 8, 8)

/*==========================================================================*/
/*====== PHY Control Register, 25d, 19h, NSC ===============================*/
/*==========================================================================*/
#define EDDI_PHY_NSC_PHY_CONTROL_BIT__MDIX_EN                           EDDI_BIT_MASK_PARA(15,15)
#define EDDI_PHY_NSC_PHY_CONTROL_BIT__FORCE_MDIX                        EDDI_BIT_MASK_PARA(14,14)

/*==========================================================================*/
/*====== 10Base-T Status/Control Register, 26d, 1Ah, NSC ===================*/
/*==========================================================================*/
#define EDDI_PHY_NSC_10BASET_STATUS_CONTROL_BIT__REJECT100_BASET        EDDI_BIT_MASK_PARA(14,14)

/*==========================================================================*/
/*====== Page Select Register, 19d, 13h, NSC ===============================*/
/*==========================================================================*/
#define EDDI_PHY_NSC_PAGE_SELECT_BIT__PAGE_SEL                          EDDI_BIT_MASK_PARA( 1, 0)

/*==========================================================================*/
/*====== PMD Configuration Register (PAGE1-Register), 20d, 14h, NSC ========*/
/*==========================================================================*/
#define EDDI_PHY_NSC_PMD_CNFG_BIT__CRS_DV                               EDDI_BIT_MASK_PARA( 4, 4)

/*==========================================================================*/
/*====== LED Direct Control Register, 24d, 18h, NSC ========================*/
/*==========================================================================*/
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__ACTLED                     EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__LNKLED                     EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__SPDLED                     EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_ACTLED                 EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_LNKLED                 EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__DRV_SPDLED                 EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__BLINK_FREQ                 EDDI_BIT_MASK_PARA( 7, 6)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__LEDACT_RX                  EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_NSC_LED_DIRECT_CONTROL_BIT__Res0                       EDDI_BIT_MASK_PARA(15, 9)

#endif //EDDI_CFG_PHY_TRANSCEIVER_NSC

#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)

/*==========================================================================*/
/*====== SW Strap Control Register 1, 9d, 9h, TI ===========================*/
/*==========================================================================*/
#define EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__Fast_RXDV_Detection          EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__RMII_Enhanced                EDDI_BIT_MASK_PARA( 9, 9)
#define EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__LED_Cfg                      EDDI_BIT_MASK_PARA(10,10)
#define EDDI_PHY_TI_SW_STRAP_CONTROL1_BIT__SW_Strap_Config_Done         EDDI_BIT_MASK_PARA(15,15)

/*==========================================================================*/
/*====== SW Strap Control Register 2, 10d, Ah, TI ==========================*/
/*==========================================================================*/
//#define EDDI_PHY_TI_SW_STRAP_CONTROL2_BIT__Fiber_Mode_Control         EDDI_BIT_MASK_PARA(14,14)

/*==========================================================================*/
/*====== LED Control Register, 24d, 18h, TI ================================*/
/*==========================================================================*/
#define EDDI_PHY_TI_LED_CONTROL_BIT__ACTLEDSET                          EDDI_BIT_MASK_PARA( 0, 0)
#define EDDI_PHY_TI_LED_CONTROL_BIT__LINKLEDSET                         EDDI_BIT_MASK_PARA( 1, 1)
#define EDDI_PHY_TI_LED_CONTROL_BIT__SPEEDLEDSET                        EDDI_BIT_MASK_PARA( 2, 2)
#define EDDI_PHY_TI_LED_CONTROL_BIT__DRV_ACTLED                         EDDI_BIT_MASK_PARA( 3, 3)
#define EDDI_PHY_TI_LED_CONTROL_BIT__DRV_LINKLED                        EDDI_BIT_MASK_PARA( 4, 4)
#define EDDI_PHY_TI_LED_CONTROL_BIT__DRV_SPEEDLED                       EDDI_BIT_MASK_PARA( 5, 5)
#define EDDI_PHY_TI_LED_CONTROL_BIT__LEDACTPOL                          EDDI_BIT_MASK_PARA( 6, 6)
#define EDDI_PHY_TI_LED_CONTROL_BIT__LEDLINKPOL                         EDDI_BIT_MASK_PARA( 7, 7)
#define EDDI_PHY_TI_LED_CONTROL_BIT__LEDSPEEDPOL                        EDDI_BIT_MASK_PARA( 8, 8)
#define EDDI_PHY_TI_LED_CONTROL_BIT__BLINK_RATE                         EDDI_BIT_MASK_PARA(10, 9)
#define EDDI_PHY_TI_LED_CONTROL_BIT__Res0                               EDDI_BIT_MASK_PARA(15,11)

/*==========================================================================*/
/*====== PHY Control Register, 25d, 19h, TI ================================*/
/*==========================================================================*/
#define EDDI_PHY_TI_PHY_CONTROL_BIT__Force_MDIX                         EDDI_BIT_MASK_PARA(14,14)
#define EDDI_PHY_TI_PHY_CONTROL_BIT__Auto_MDIX_Enable                   EDDI_BIT_MASK_PARA(15,15)

#endif //EDDI_CFG_PHY_TRANSCEIVER_TI

/*===========================================================================*/
/*                      function prototypes in eddi_Tra.c                    */
/*===========================================================================*/

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown  ( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  HwPortIndex,
                                                         LSA_UINT8                const  PHYPower,
                                                         LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  HwPortIndex,
                                                         LSA_BOOL                      * pIsPowerDown);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetFXMode(     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON );

EDDI_PHY_TRANSCEIVER_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAEvaluatePhyType( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex );

/*===========================================================================*/
/*                      function prototypes in eddi_Tra_Broadcom.c           */
/*===========================================================================*/

#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex,
                                                                LSA_UINT8                const  PHYPower,
                                                                LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex,
                                                                  LSA_BOOL                      * pIsPowerDown);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bPlugCommit);

SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                        LSA_UINT32               const  HwPortIndex,
                                                                                        SER_SWI_LINK_PTR_TYPE    const  pBasePx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const *  const  pAutonegCapability );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const *  const  pConfiguredLinkSpeedAndMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_BOOL                 const  bMDIX );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bFXModeON );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex );

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex,
                                                                         LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                     LSA_UINT32               const  HwPortIndex );

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_Broadcom( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex );

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_BROADCOM

#endif //EDDI_CFG_PHY_TRANSCEIVER_BROADCOM

/*===========================================================================*/
/*                      function prototypes in eddi_Tra_NEC.c                */
/*===========================================================================*/

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT8                const  PHYPower,
                                                           LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                      * pIsPowerDown);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bPlugCommit);

SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                   LSA_UINT32               const  HwPortIndex,
                                                                                   SER_SWI_LINK_PTR_TYPE    const  pBasePx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_NEC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const * const  pAutonegCapability );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_NEC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const * const  pConfiguredLinkSpeedAndMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  HwPortIndex,
                                                      LSA_BOOL                 const  bMDIX );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON );

LSA_RESULT EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                          LSA_UINT32               const HwPortIndex );

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex,
                                                                    LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex );

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NEC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_NEC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex );

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_NEC

#endif //EDDI_CFG_PHY_TRANSCEIVER_NEC

/*===========================================================================*/
/*                      function prototypes in eddi_Tra_NSC.c                */
/*===========================================================================*/

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiNSCAutonegBugTimeout( LSA_VOID  *  const  context );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegBugInitNSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT8                const  PHYPower,
                                                           LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                      * pIsPowerDown);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bPlugCommit);

SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                   LSA_UINT32               const  HwPortIndex,
                                                                                   SER_SWI_LINK_PTR_TYPE    const  pBasePx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_NSC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const * const  pAutonegCapability );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_NSC( EDDI_LOCAL_DDB_PTR_TYPE                  const  pDDB,
                                                                       LSA_UINT32                               const  HwPortIndex,
                                                                       LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const * const  pConfiguredLinkSpeedAndMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                      LSA_UINT32               const  HwPortIndex,
                                                      LSA_BOOL                 const  bMDIX );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                              LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex,
                                                        LSA_BOOL                 const  bFXModeON );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex );

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                    LSA_UINT32               const  HwPortIndex,
                                                                    LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex );

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_NSC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                        LSA_UINT32               const  HwPortIndex );

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_NSC

#if defined (EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRARegDump_NSC( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB);
#endif //(EDDI_CFG_ENABLE_PHY_REGDUMP_NSC)


#endif //EDDI_CFG_PHY_TRANSCEIVER_NSC

/*===========================================================================*/
/*                      function prototypes in eddi_Tra_TI.c                 */
/*===========================================================================*/

#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_UINT8                const  PHYPower,
                                                          LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                            LSA_UINT32               const  HwPortIndex,
                                                            LSA_BOOL                      * pIsPowerDown);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                    LSA_UINT32               const  HwPortIndex,
                                                    LSA_BOOL                 const  bPlugCommit);

SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                  LSA_UINT32               const  HwPortIndex,
                                                                                  SER_SWI_LINK_PTR_TYPE    const  pBasePx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_TI( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                      LSA_UINT32                                const  HwPortIndex,
                                                                      LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const *  const  pAutonegCapability );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_TI( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                      LSA_UINT32                                const  HwPortIndex,
                                                                      LSA_UINT32  EDDI_LOCAL_MEM_ATTR  const *  const  pConfiguredLinkSpeedAndMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                     LSA_UINT32               const  HwPortIndex,
                                                     LSA_BOOL                 const  bMDIX );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex,
                                                       LSA_BOOL                 const  bFXModeON );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex );

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex,
                                                                   LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex );

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_TI)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_TI( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                       LSA_UINT32               const  HwPortIndex );

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_TI

#endif //EDDI_CFG_PHY_TRANSCEIVER_TI

/*===========================================================================*/
/*                      function prototypes in eddi_Tra_Userspec.c           */
/*===========================================================================*/

#if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetPowerDown_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex,
                                                                LSA_UINT8                const  PHYPower,
                                                                LSA_BOOL                 const  bRaw);

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckPowerDown_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  LSA_UINT32               const  HwPortIndex,
                                                                  LSA_BOOL                      * pIsPowerDown);

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRAStartPhy_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                          LSA_UINT32               const  HwPortIndex,
                                                          LSA_BOOL                 const  bPlugCommit);

SWI_AUTONEG_GET_SPEEDMODE_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_TRAGetSpMoAutoNegOn_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                                        LSA_UINT32               const  HwPortIndex,
                                                                                        SER_SWI_LINK_PTR_TYPE    const  pBasePx );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRACheckSpeedModeCapability_Userspec( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pAutonegCapability );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRAAutonegMappingCapability_Userspec( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
                                                                            LSA_UINT32                                const  HwPortIndex,
                                                                            LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pConfiguredLinkSpeedAndMode );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetMDIX_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                           LSA_UINT32               const  HwPortIndex,
                                                           LSA_BOOL                 const  bMDIX );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRARestoreAutoMDIX_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                   LSA_UINT32               const  HwPortIndex );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TraSetFXMode_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex,
                                                             LSA_BOOL                 const  bFXModeON );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_TRASetSoftReset_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                LSA_UINT32               const  HwPortIndex );

#if defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBeginExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                       LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetModeExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                         LSA_UINT32               const  HwPortIndex,
                                                                         LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEndExternal_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                     LSA_UINT32               const  HwPortIndex );

#elif !defined (EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC)

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkBegin_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               LSA_UINT32               const  HwPortIndex );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkSetMode_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                 LSA_UINT32               const  HwPortIndex,
                                                                 LSA_UINT16               const  LEDMode );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_TRALedBlinkEnd_Userspec( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                             LSA_UINT32               const  HwPortIndex );

#endif //EDDI_CFG_PHY_BLINK_EXTERNAL_USERSPEC

#endif //EDDI_CFG_PHY_TRANSCEIVER_USERSPEC

#if defined __cplusplus
}
#endif

#endif //EDDI_TRA_H


/*****************************************************************************/
/*  end of file eddi_Tra.h                                                   */
/*****************************************************************************/

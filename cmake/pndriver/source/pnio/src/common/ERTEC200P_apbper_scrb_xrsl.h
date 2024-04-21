#ifndef ERTEC200P_APBPER_SCRB_XRSL_H
#define ERTEC200P_APBPER_SCRB_XRSL_H

#ifdef __cplusplus
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
/*  C o m p o n e n t     &C: PNExplorer                                :C&  */
/*                                                                           */
/*  F i l e               &F: ERTEC200P_apbper_scrb_xrsl.h              :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  P a r a m e t e r s     : Version = 2, Ports = , Interface =  */
/*****************************************************************************/


/**
@defgroup       ID_REG ID_REG
@ingroup        Register
@brief          Identification ERTEC200+
@param Address  0x00000000
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ID_REG              0x00000000
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_RST__VAL     0x40280100

/**
@defgroup       ID_REG__MET_FIX MET_FIX
@ingroup        ID_REG
@brief          Metall-Fix: 00h
@param Address  0x00000000
@param Mask     0x000000FF
@param Shift    0
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ID_REG__MSK_MET_FIX       0x000000FF
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_SHFT_MET_FIX       0
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_RSTV_MET_FIX       0x00000000

/**
@defgroup       ID_REG__HW_R HW_R
@ingroup        ID_REG
@brief          HW-Release: 01h
@param Address  0x00000000
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ID_REG__MSK_HW_R       0x0000FF00
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_SHFT_HW_R       8
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_RSTV_HW_R       0x00000001

/**
@defgroup       ID_REG__COMP COMP
@ingroup        ID_REG
@brief          ERTEC200+-Kennung: 4028h
@param Address  0x00000000
@param Mask     0xFFFF0000
@param Shift    16
@param Access   r,
@param Reset    0x00004028
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ID_REG__MSK_COMP       0xFFFF0000
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_SHFT_COMP       16
#define ERTEC200P_APBPER_SCRB_REG_ID_REG_RSTV_COMP       0x00004028


/**
@defgroup       BOOT_REG BOOT_REG
@ingroup        Register
@brief          Boot-Mode-Pins BOOT(3:0) lesbar
@param Address  0x00000004
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG              0x00000004
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_RST__VAL     0x00000000

/**
@defgroup       BOOT_REG__BOOT_0 BOOT_0
@ingroup        BOOT_REG
@brief          der am Boot(0) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000004
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG__MSK_BOOT_0       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_SHFT_BOOT_0       0
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_RSTV_BOOT_0       0x00000000

/**
@defgroup       BOOT_REG__BOOT_1 BOOT_1
@ingroup        BOOT_REG
@brief          der am Boot(1) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000004
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG__MSK_BOOT_1       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_SHFT_BOOT_1       1
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_RSTV_BOOT_1       0x00000000

/**
@defgroup       BOOT_REG__BOOT_2 BOOT_2
@ingroup        BOOT_REG
@brief          der am Boot(2) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000004
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG__MSK_BOOT_2       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_SHFT_BOOT_2       2
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_RSTV_BOOT_2       0x00000000

/**
@defgroup       BOOT_REG__BOOT_3 BOOT_3
@ingroup        BOOT_REG
@brief          der am Boot(3) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000004
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG__MSK_BOOT_3       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_SHFT_BOOT_3       3
#define ERTEC200P_APBPER_SCRB_REG_BOOT_REG_RSTV_BOOT_3       0x00000000


/**
@defgroup       CONFIG_REG CONFIG_REG
@ingroup        Register
@brief          ERTEC200+-Config-Pins CONFIG(6:0) lesbar, Config-REG(6-3) schreibbar
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG              0x00000008
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_RST__VAL     0x00000000

/**
@defgroup       CONFIG_REG__CONF_0 CONF_0
@ingroup        CONFIG_REG
@brief          der am Config(0) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000008
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG__MSK_CONF_0       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_SHFT_CONF_0       0
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_RSTV_CONF_0       0x00000000

/**
@defgroup       CONFIG_REG__CONF_1 CONF_1
@ingroup        CONFIG_REG
@brief          der am Config(1) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000008
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG__MSK_CONF_1       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_SHFT_CONF_1       1
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_RSTV_CONF_1       0x00000000

/**
@defgroup       CONFIG_REG__CONF_2 CONF_2
@ingroup        CONFIG_REG
@brief          der am Config(2) Pin anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht
@param Address  0x00000008
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG__MSK_CONF_2       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_SHFT_CONF_2       2
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_RSTV_CONF_2       0x00000000

/**
@defgroup       CONFIG_REG__CONF_6_3 CONF_6_3
@ingroup        CONFIG_REG
@brief          der an den Config(6-3) Pins anstehende Wert wird bei Deaktivierung des PowerOn-Reset eingelatcht und nach  Deaktivierung des intern verlängerten Systemreset in das Register übernommen, anschliessend kann der Wert per Schreibzugriff geändert werden 00-0: XHIF = on, 16 Bit Mode, GPIO94-79, 61-60 on (all inputs), XHIF_XWR hat Read/Write-Control 00-1: XHIF = on, 16 Bit Mode, GPIO94-79, 61-60 on (all inputs), XHIF_XRD / XHIF_XWR getrennt 000-: XHIF = on, 16 Bit Mode, GPIO94-79, 61-60 on (all inputs), XHIF_XRDY ist high-aktiv 001-: XHIF = on, 16 Bit Mode, GPIO94-79, 61-60 on (all inputs), XHIF_XRDY ist low-aktiv 01-0: XHIF = on, 32 Bit Mode, GPIO95-32 off, XHIF_XWR hat Read/Write-Control 01-1: XHIF = on, 32 Bit Mode, GPIO95-32 off, XHIF_XRD / XHIF_XWR getrennt 010-: XHIF = on, 32 Bit Mode, GPIO95-32 off, XHIF_XRDY ist high-aktiv 011-: XHIF = on, 32 Bit Mode, GPIO95-32 off, XHIF_XRDY ist low-aktiv 1000: XHIF = SPI, GPIO89-32 on (all inputs) 1001: XHIF = off, GPIO95-32 on (all inputs) 1100: XHIF = off, int. PHYs = on, ext. MII = PHY-Debugging, 1101: XHIF = off, int. PHYs = off, ext. MII = on                                                                                                                                                                                                                                                                                                                                                     
@param Address  0x00000008
@param Mask     0x00000078
@param Shift    3
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG__MSK_CONF_6_3       0x00000078
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_SHFT_CONF_6_3       3
#define ERTEC200P_APBPER_SCRB_REG_CONFIG_REG_RSTV_CONF_6_3       0x00000000


/**
@defgroup       ASYN_RES_CTRL_REG ASYN_RES_CTRL_REG
@ingroup        Register
@brief          Steuerregisster für Reset des ERTEC200+ Rücksetzen erfolgt für Slice EN_WD_RES_PN durch PNIP-Reset, für Slice RES_SOFT_KRISC32_CORE durch XRESET. Die restlichen Slices werden mittels Systemreset zurückgesetzt.
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG              0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RST__VAL     0x00000028

/**
@defgroup       ASYN_RES_CTRL_REG__WD_RES_FREI_ARM926 WD_RES_FREI_ARM926
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Freigabe Watchdog Reset für den ARM926
@param Address  0x0000000C
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_WD_RES_FREI_ARM926       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_WD_RES_FREI_ARM926       0
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_WD_RES_FREI_ARM926       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__RES_SOFT RES_SOFT
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Asynchrones Software Reset (nicht speichernd, PULSE_DUR bildet Reset-Länge), setzt alles ausser PN-IP und KRISC32 zurück
@param Address  0x0000000C
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_RES_SOFT       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_RES_SOFT       1
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_RES_SOFT       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__RES_SOFT_PN RES_SOFT_PN
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Asynchrones Software Reset (nicht speichernd, PULSE_DUR bildet Reset-Länge), setzt nur den PN-IP und den KRISC32 zurück
@param Address  0x0000000C
@param Mask     0x00000004
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_RES_SOFT_PN       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_RES_SOFT_PN       2
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_RES_SOFT_PN       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__EN_WD_RES_PN EN_WD_RES_PN
@ingroup        ASYN_RES_CTRL_REG
@brief          0: PN-IP / KRISC32 werden beim ARM926 Watchdog-Reset nicht zurückgesetzt 1: PN-IP / KRISC32 werden beim ARM926 Watchdog-Reset zurückgesetzt
@param Address  0x0000000C
@param Mask     0x00000008
@param Shift    3
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_EN_WD_RES_PN       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_EN_WD_RES_PN       3
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_EN_WD_RES_PN       0x00000001

/**
@defgroup       ASYN_RES_CTRL_REG__WD_RES_FREI_KRISC WD_RES_FREI_KRISC
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Freigabe Watchdog Reset für den KRISC32
@param Address  0x0000000C
@param Mask     0x00000010
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_WD_RES_FREI_KRISC       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_WD_RES_FREI_KRISC       4
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_WD_RES_FREI_KRISC       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__RES_SOFT_KRISC32_CORE RES_SOFT_KRISC32_CORE
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Asynchrones Software Reset (speichernd), setzt nur das KRISC32 Core System zurück
@param Address  0x0000000C
@param Mask     0x00000020
@param Shift    5
@param Access   rh,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_RES_SOFT_KRISC32_CORE       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_RES_SOFT_KRISC32_CORE       5
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_RES_SOFT_KRISC32_CORE       0x00000001

/**
@defgroup       ASYN_RES_CTRL_REG__RES_SOFT_ARM926_CORE RES_SOFT_ARM926_CORE
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Asynchrones Software Reset (nicht speichernd, PULSE_DUR bildet Reset-Länge), setzt nur das ARM926EJ-S Core System zurück
@param Address  0x0000000C
@param Mask     0x00000040
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_RES_SOFT_ARM926_CORE       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_RES_SOFT_ARM926_CORE       6
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_RES_SOFT_ARM926_CORE       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__RES_SOFT_SDMMC RES_SOFT_SDMMC
@ingroup        ASYN_RES_CTRL_REG
@brief          1: Asynchrones Software Reset (speichernd), setzt nur das SDMMC-Modul zurück
@param Address  0x0000000C
@param Mask     0x00000080
@param Shift    7
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_RES_SOFT_SDMMC       0x00000080
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_RES_SOFT_SDMMC       7
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_RES_SOFT_SDMMC       0x00000000

/**
@defgroup       ASYN_RES_CTRL_REG__PULSE_DUR PULSE_DUR
@ingroup        ASYN_RES_CTRL_REG
@brief          Puls-Dauer der SW-Resets RES_SOFT, RES_SOFT_PN, ARM926 Watchdog- und KRISC32 Watchdog-Reset TRES_PULSE = (8 x n + 8) x TCLK TCLK: APB-Takt-Periode (1/125MHz = 8ns) n: Wert von PULSE_DUR (0 .. 65535) Die integrierten PHYs benötigen eine Resetdauer von - 100 us. Damit muss n - 1562 eingestellt werden                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
@param Address  0x0000000C
@param Mask     0xFFFF0000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG__MSK_PULSE_DUR       0xFFFF0000
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_SHFT_PULSE_DUR       16
#define ERTEC200P_APBPER_SCRB_REG_ASYN_RES_CTRL_REG_RSTV_PULSE_DUR       0x00000000


/**
@defgroup       SYN_RES_CTRL_REG SYN_RES_CTRL_REG
@ingroup        Register
@brief          Steuerregister für synchronen Reset des ERTEC200+.
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG              0x00000010
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_RST__VAL     0x00000000

/**
@defgroup       SYN_RES_CTRL_REG__SYN_RES_PER_IF SYN_RES_PER_IF
@ingroup        SYN_RES_CTRL_REG
@brief          0: Kein synchroner Reset zum PER-IF 1: Synchroner Reset zum PER-IF (speichernd)
@param Address  0x00000010
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG__MSK_SYN_RES_PER_IF       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_SHFT_SYN_RES_PER_IF       0
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_RSTV_SYN_RES_PER_IF       0x00000000

/**
@defgroup       SYN_RES_CTRL_REG__SYN_RES_HOST SYN_RES_HOST
@ingroup        SYN_RES_CTRL_REG
@brief          0: Kein synchroner Reset zum Host Interface 1: Synchroner Reset zum Host Interface (speichernd)
@param Address  0x00000010
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG__MSK_SYN_RES_HOST       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_SHFT_SYN_RES_HOST       1
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_RSTV_SYN_RES_HOST       0x00000000

/**
@defgroup       SYN_RES_CTRL_REG__SYN_RES_PN_IP SYN_RES_PN_IP
@ingroup        SYN_RES_CTRL_REG
@brief          0: Kein synchroner Reset zum PN-IP 1: Synchroner Reset zum PN-IP (speichernd)
@param Address  0x00000010
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG__MSK_SYN_RES_PN_IP       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_SHFT_SYN_RES_PN_IP       2
#define ERTEC200P_APBPER_SCRB_REG_SYN_RES_CTRL_REG_RSTV_SYN_RES_PN_IP       0x00000000


/**
@defgroup       RES_STAT_REG RES_STAT_REG
@ingroup        Register
@brief          Statusregister für Reset des ERTEC200+. Es ist immer nur das Bit des zuletzt aufgetretenen Reset-Ereignisses gesetzt. Die beiden anderen Bits sind zurückgesetzt.
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG              0x00000014
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_RST__VAL     0x00000004

/**
@defgroup       RES_STAT_REG__ARM926_WDOG_RES ARM926_WDOG_RES
@ingroup        RES_STAT_REG
@brief          1: Letzter Reset war Reset über den ARM926 Watchdog
@param Address  0x00000014
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG__MSK_ARM926_WDOG_RES       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_SHFT_ARM926_WDOG_RES       0
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_RSTV_ARM926_WDOG_RES       0x00000000

/**
@defgroup       RES_STAT_REG__SW_RES SW_RES
@ingroup        RES_STAT_REG
@brief          1: Letzter Reset war Reset über Software-Reset
@param Address  0x00000014
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG__MSK_SW_RES       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_SHFT_SW_RES       1
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_RSTV_SW_RES       0x00000000

/**
@defgroup       RES_STAT_REG__PWRON_HW_RES PWRON_HW_RES
@ingroup        RES_STAT_REG
@brief          1: Letzter Reset war ein PowerOn- oder Hardware-Reset
@param Address  0x00000014
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG__MSK_PWRON_HW_RES       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_SHFT_PWRON_HW_RES       2
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_RSTV_PWRON_HW_RES       0x00000001

/**
@defgroup       RES_STAT_REG__SW_RES_ARM926 SW_RES_ARM926
@ingroup        RES_STAT_REG
@brief          1: Letzter Reset war Reset über Software-Reset ARM926 Core
@param Address  0x00000014
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG__MSK_SW_RES_ARM926       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_SHFT_SW_RES_ARM926       3
#define ERTEC200P_APBPER_SCRB_REG_RES_STAT_REG_RSTV_SW_RES_ARM926       0x00000000


/**
@defgroup       PLL_STAT_REG PLL_STAT_REG
@ingroup        Register
@brief          Statusregister für PLL des ERTEC200+
@param Address  0x00000018
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG              0x00000018
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG_RST__VAL     0x00000001

/**
@defgroup       PLL_STAT_REG__LOCK LOCK
@ingroup        PLL_STAT_REG
@brief          Lock: Einrasten bei der Arbeitsfrequenz- Status der PLL: 0: PLL ist unlocked 1: PLL ist locked Dieses Bit stellt den aktuellen Lock-Zustand der PLL dar. Nur lesbar
@param Address  0x00000018
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG__MSK_LOCK       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG_SHFT_LOCK       0
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG_RSTV_LOCK       0x00000001

/**
@defgroup       PLL_STAT_REG__LOSS LOSS
@ingroup        PLL_STAT_REG
@brief          Loss: Überwachungsstatus PLL-Input-Clock 1: PLL-Input-Clock nicht erkannt 0: PLL-Input-Clock vorhanden Dieses Bit zeigt den aktuellen Überwachungssatus des PLL-Input-Clocks an. Nur lesbar
@param Address  0x00000018
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG__MSK_LOSS       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG_SHFT_LOSS       1
#define ERTEC200P_APBPER_SCRB_REG_PLL_STAT_REG_RSTV_LOSS       0x00000000


/**
@defgroup       QVZ_AHB_ADR QVZ_AHB_ADR
@ingroup        Register
@brief          Adresse einer Fehladressierung am Multi-Layer-AHB
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_ADR              0x0000001C
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_ADR_RST__VAL     0x00000000

/**
@defgroup       QVZ_AHB_ADR__QVZ_AHB_ADR QVZ_AHB_ADR
@ingroup        QVZ_AHB_ADR
@brief          Adresse einer Fehladressierung am Multi-Layer-AHB
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_ADR__MSK_QVZ_AHB_ADR       0xFFFFFFFF
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_ADR_SHFT_QVZ_AHB_ADR       0
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_ADR_RSTV_QVZ_AHB_ADR       0x00000000


/**
@defgroup       QVZ_AHB_CTRL QVZ_AHB_CTRL
@ingroup        Register
@brief          Control-Signale einer Fehladressierung am Multi-Layer-AHB
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL              0x00000020
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_RST__VAL     0x00000000

/**
@defgroup       QVZ_AHB_CTRL__R_W R_W
@ingroup        QVZ_AHB_CTRL
@brief          Read/Write 0: read 1: write
@param Address  0x00000020
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL__MSK_R_W       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_SHFT_R_W       0
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_RSTV_R_W       0x00000000

/**
@defgroup       QVZ_AHB_CTRL__HSIZE HSIZE
@ingroup        QVZ_AHB_CTRL
@brief          HSIZE
@param Address  0x00000020
@param Mask     0x0000000E
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL__MSK_HSIZE       0x0000000E
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_SHFT_HSIZE       1
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_RSTV_HSIZE       0x00000000

/**
@defgroup       QVZ_AHB_CTRL__HBURST HBURST
@ingroup        QVZ_AHB_CTRL
@brief          HBURST
@param Address  0x00000020
@param Mask     0x00000070
@param Shift    4
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL__MSK_HBURST       0x00000070
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_SHFT_HBURST       4
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_CTRL_RSTV_HBURST       0x00000000


/**
@defgroup       QVZ_AHB_M QVZ_AHB_M
@ingroup        Register
@brief          Masterkennung einer Fehladressierung am Multi-Layer-AHB
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M              0x00000024
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RST__VAL     0x00000000

/**
@defgroup       QVZ_AHB_M__ARM_I ARM_I
@ingroup        QVZ_AHB_M
@brief          ARM926-I
@param Address  0x00000024
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_ARM_I       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_ARM_I       0
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_ARM_I       0x00000000

/**
@defgroup       QVZ_AHB_M__ARM_D ARM_D
@ingroup        QVZ_AHB_M
@brief          ARM926-D
@param Address  0x00000024
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_ARM_D       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_ARM_D       1
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_ARM_D       0x00000000

/**
@defgroup       QVZ_AHB_M__KRISC32 KRISC32
@ingroup        QVZ_AHB_M
@brief          KRISC32 (ARM966E-S)
@param Address  0x00000024
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_KRISC32       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_KRISC32       2
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_KRISC32       0x00000000

/**
@defgroup       QVZ_AHB_M__PN PN
@ingroup        QVZ_AHB_M
@brief          PROFINET_IP
@param Address  0x00000024
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_PN       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_PN       3
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_PN       0x00000000

/**
@defgroup       QVZ_AHB_M__GDMA GDMA
@ingroup        QVZ_AHB_M
@brief          GDMA
@param Address  0x00000024
@param Mask     0x00000010
@param Shift    4
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_GDMA       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_GDMA       4
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_GDMA       0x00000000

/**
@defgroup       QVZ_AHB_M__HOSTIF HOSTIF
@ingroup        QVZ_AHB_M
@brief          Host Interface
@param Address  0x00000024
@param Mask     0x00000020
@param Shift    5
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M__MSK_HOSTIF       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_SHFT_HOSTIF       5
#define ERTEC200P_APBPER_SCRB_REG_QVZ_AHB_M_RSTV_HOSTIF       0x00000000


/**
@defgroup       QVZ_APB_ADR QVZ_APB_ADR
@ingroup        Register
@brief          Adresse einer Fehladressierung am APB
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_APB_ADR              0x00000028
#define ERTEC200P_APBPER_SCRB_REG_QVZ_APB_ADR_RST__VAL     0x00000000

/**
@defgroup       QVZ_APB_ADR__QVZ_APB_ADR QVZ_APB_ADR
@ingroup        QVZ_APB_ADR
@brief          Adresse einer Fehladressierung am APB
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_APB_ADR__MSK_QVZ_APB_ADR       0xFFFFFFFF
#define ERTEC200P_APBPER_SCRB_REG_QVZ_APB_ADR_SHFT_QVZ_APB_ADR       0
#define ERTEC200P_APBPER_SCRB_REG_QVZ_APB_ADR_RSTV_QVZ_APB_ADR       0x00000000


/**
@defgroup       QVZ_EMC_ADR QVZ_EMC_ADR
@ingroup        Register
@brief          Adresse die zum QVZ am EMC führte
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_EMC_ADR              0x0000002C
#define ERTEC200P_APBPER_SCRB_REG_QVZ_EMC_ADR_RST__VAL     0x00000000

/**
@defgroup       QVZ_EMC_ADR__QVZ_EMC_ADR QVZ_EMC_ADR
@ingroup        QVZ_EMC_ADR
@brief          Adresse die zum QVZ am EMC führte
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_QVZ_EMC_ADR__MSK_QVZ_EMC_ADR       0xFFFFFFFF
#define ERTEC200P_APBPER_SCRB_REG_QVZ_EMC_ADR_SHFT_QVZ_EMC_ADR       0
#define ERTEC200P_APBPER_SCRB_REG_QVZ_EMC_ADR_RSTV_QVZ_EMC_ADR       0x00000000


/**
@defgroup       MEM_SWAP MEM_SWAP
@ingroup        Register
@brief          Memory Swapping in Segment 0 auf dem AHB (ROM, EMC-SDRAM, EMC-Standard-Memory, I/D-TCM)
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MEM_SWAP              0x00000030
#define ERTEC200P_APBPER_SCRB_REG_MEM_SWAP_RST__VAL     0x00000000

/**
@defgroup       MEM_SWAP__SWAP SWAP
@ingroup        MEM_SWAP
@brief          Auswahl des Speichers in Segment 0 auf dem AHB: 00: Boot-ROM ab Adr 0h 01: EMC-SDRAM ab Adr 0h (nur die ersten 64 MB) 10: EMC-Standard-Memory ab Adr 0h (nur die ersten 64 MB) 11: reserved (es wird kein Speicherbereich auf Adr 0h gelegt, bei Zugriff wird QVZ generiert)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
@param Address  0x00000030
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MEM_SWAP__MSK_SWAP       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_MEM_SWAP_SHFT_SWAP       0
#define ERTEC200P_APBPER_SCRB_REG_MEM_SWAP_RSTV_SWAP       0x00000000


/**
@defgroup       M_LOCK_CTRL M_LOCK_CTRL
@ingroup        Register
@brief          AHB-Master-Lock-Enable. Master-selektive Freigabe der AHB-Lock-Funktionalität.
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL              0x00000034
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_RST__VAL     0x00000000

/**
@defgroup       M_LOCK_CTRL__LE_ARM926_I LE_ARM926_I
@ingroup        M_LOCK_CTRL
@brief          Lock-Enable AHB-Master ARM926-I: (I-Master unterstützt keine locked transfers)  0: Lock disabled 1: Lock enabled
@param Address  0x00000034
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL__MSK_LE_ARM926_I       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_SHFT_LE_ARM926_I       0
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_RSTV_LE_ARM926_I       0x00000000

/**
@defgroup       M_LOCK_CTRL__LE_ARM926_D LE_ARM926_D
@ingroup        M_LOCK_CTRL
@brief          Lock-Enable AHB-Master ARM926-D:  0: Lock disabled 1: Lock enabled
@param Address  0x00000034
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL__MSK_LE_ARM926_D       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_SHFT_LE_ARM926_D       1
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_RSTV_LE_ARM926_D       0x00000000

/**
@defgroup       M_LOCK_CTRL__LE_ARM966 LE_ARM966
@ingroup        M_LOCK_CTRL
@brief          Lock-Enable AHB-Master ARM966E-S:  0: Lock disabled 1: Lock enabled
@param Address  0x00000034
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL__MSK_LE_ARM966       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_SHFT_LE_ARM966       2
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_RSTV_LE_ARM966       0x00000000

/**
@defgroup       M_LOCK_CTRL__ARB ARB
@ingroup        M_LOCK_CTRL
@brief          Select Arbitrierungs-Algorithmus für AHB-Arbiter (ARB_MODE):  0: Round-robin 1: feste Prioritätszuordnung Dieses Bit sollte nicht verändert werden (Default: roundrobin)! (s. Kap. 5.3.1.1)
@param Address  0x00000034
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL__MSK_ARB       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_SHFT_ARB       6
#define ERTEC200P_APBPER_SCRB_REG_M_LOCK_CTRL_RSTV_ARB       0x00000000


/**
@defgroup       ERTEC200PLUS_TAG ERTEC200PLUS_TAG
@ingroup        Register
@brief          Tag-Number des aktuellen Schaltungsstand
@param Address  0x00000038
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG              0x00000038
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RST__VAL     0x00001000

/**
@defgroup       ERTEC200PLUS_TAG__R_LABEL R_LABEL
@ingroup        ERTEC200PLUS_TAG
@brief          R-Label
@param Address  0x00000038
@param Mask     0x000007FF
@param Shift    0
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG__MSK_R_LABEL       0x000007FF
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_SHFT_R_LABEL       0
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RSTV_R_LABEL       0x00000000

/**
@defgroup       ERTEC200PLUS_TAG__INKREMENT_NR INKREMENT_NR
@ingroup        ERTEC200PLUS_TAG
@brief          Inkrement No. (wird nicht verwendet, siehe ID_REG Slice HW_R)
@param Address  0x00000038
@param Mask     0x0000F800
@param Shift    11
@param Access   r,
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG__MSK_INKREMENT_NR       0x0000F800
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_SHFT_INKREMENT_NR       11
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RSTV_INKREMENT_NR       0x00000002

/**
@defgroup       ERTEC200PLUS_TAG__PATCH_NR PATCH_NR
@ingroup        ERTEC200PLUS_TAG
@brief          Patch No. (wird nicht verwendet, siehe ID_REG Slice MET_FIX)
@param Address  0x00000038
@param Mask     0x00070000
@param Shift    16
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG__MSK_PATCH_NR       0x00070000
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_SHFT_PATCH_NR       16
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RSTV_PATCH_NR       0x00000000

/**
@defgroup       ERTEC200PLUS_TAG__PLATFORM PLATFORM
@ingroup        ERTEC200PLUS_TAG
@brief          Platform:00 = ASIC,  01 = FPGA, 10 = reserved, 11 = userdefined 
@param Address  0x00000038
@param Mask     0x00180000
@param Shift    19
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG__MSK_PLATFORM       0x00180000
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_SHFT_PLATFORM       19
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RSTV_PLATFORM       0x00000000

/**
@defgroup       ERTEC200PLUS_TAG__IDENTIFICATION IDENTIFICATION
@ingroup        ERTEC200PLUS_TAG
@brief          Identification (wird nicht verwendet, siehe ID_REG Slice COMP)
@param Address  0x00000038
@param Mask     0xFFE00000
@param Shift    21
@param Access   r,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG__MSK_IDENTIFICATION       0xFFE00000
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_SHFT_IDENTIFICATION       21
#define ERTEC200P_APBPER_SCRB_REG_ERTEC200PLUS_TAG_RSTV_IDENTIFICATION       0x00000000


/**
@defgroup       PHY_CONFIG PHY_CONFIG
@ingroup        Register
@brief          Konfiguration von PHY1 und PHY2 Achtung: PHY_CONFIG Register wird nur mit dem asynchronen Reset vom PN-IP zurückgesetzt
@param Address  0x0000003C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG              0x0000003C
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RST__VAL     0x00000000

/**
@defgroup       PHY_CONFIG__P1_PHY_ENB P1_PHY_ENB
@ingroup        PHY_CONFIG
@brief          0: PHY1 disabled (Powerdown Mode) -  Kommentar: Wird der PHY 'Disabled' und anschließend wieder 'Enabled', dann muss eine Disable-Time von - 100?s von der SW eingehalten werden (siehe Abbildung 37 - FSpec ERTEC200+). 1: PHY1 enabled -  Kommentar: Mit P1/2_PHYENABLE = 1 wird intern im PHY noch ein Reset-Verlängerung über 5,2msec angestoßen. In dieser Zeit wird die PLL und alle analogen- und digitalen Komponenten hochgefahren. Die Betriebsbereitschaft wird im PHY_Status-Register mit P1/2_PWRUPRST = 1 signalisiert (siehe Abbildung 37 - FSpec ERTEC200+).  Kommentar: Bei CONFIG(6..3)=“1101“ Bit nicht schreibbar, liegt dann fest auf Defaultwert.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
@param Address  0x0000003C
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P1_PHY_ENB       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P1_PHY_ENB       0
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P1_PHY_ENB       0x00000000

/**
@defgroup       PHY_CONFIG__P1_FX_MODE P1_FX_MODE
@ingroup        PHY_CONFIG
@brief          1: Das 100BASE-FX Interface freigeben (nur sinnvoll bei P1_PHY_Mode = 0010 oder 0011 0: Das 100BASE-FX Interface ist gesperrt
@param Address  0x0000003C
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P1_FX_MODE       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P1_FX_MODE       1
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P1_FX_MODE       0x00000000

/**
@defgroup       PHY_CONFIG__P1_2_PHY_MODE P1_2_PHY_MODE
@ingroup        PHY_CONFIG
@brief          Einstellung wirkt für Phy P1 und Phy P2 gleichzeitig. 0000: 10BASE-T HD, Auto-Neg disabled 0001: 10BASE-T FD, Auto-Neg disabled 0010: 100BASE-TX/FX HD, Auto-Neg disabled 0011: 100BASE-TX/FX FD, Auto-Neg disabled 0100: 100BASE-TX HD angekündigt, Auto-Neg enabled 0101: 100BASE-TX HD angekündigt, Auto-Neg enabled, Repeater Mode 0110: PHY startet im Power Down Mode 0111: Auto-Neg enabled, AutoMDIX enabled, alles möglich  1000: All capable. Quick Auto-Negotiation enabled. Forced Full Duplex in Parallel Detect mode. AutoMDIX enabled. Bits 1 and 0 determine timing 1001:            1010:            1011:            1100:            1101:            1110:            1111: Loopback mode. The Phy starts in loopback mode, with the 0.14 bit set. In this mode the Phy must be configured through the SMI interface and manually enabled by clearing bit 0.14. Until then it runs in loopback mode and is isolated from the line. Apart from Bit 0.14 the Phy is configured as with Phymode = 0111.
@param Address  0x0000003C
@param Mask     0x0000003C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P1_2_PHY_MODE       0x0000003C
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P1_2_PHY_MODE       2
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P1_2_PHY_MODE       0x00000000

/**
@defgroup       PHY_CONFIG__P1_AUTOMDIXEN P1_AUTOMDIXEN
@ingroup        PHY_CONFIG
@brief          1: Enable AutoMDIX state machine 0: Disable AutoMDIX state machine
@param Address  0x0000003C
@param Mask     0x00000040
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P1_AUTOMDIXEN       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P1_AUTOMDIXEN       6
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P1_AUTOMDIXEN       0x00000000

/**
@defgroup       PHY_CONFIG__P2_PHY_ENB P2_PHY_ENB
@ingroup        PHY_CONFIG
@brief          0: PHY2 disabled (Powerdown Mode) siehe Kommentar Bit 0 1: PHY2 enabled siehe Kommentar Bit 0
@param Address  0x0000003C
@param Mask     0x00000100
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P2_PHY_ENB       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P2_PHY_ENB       8
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P2_PHY_ENB       0x00000000

/**
@defgroup       PHY_CONFIG__P2_FX_MODE P2_FX_MODE
@ingroup        PHY_CONFIG
@brief          1: Das 100BASE-FX Interface freigeben (nur sinnvoll bei P2_PHY_Mode=0010 oder 0011 0: Das 100BASE-FX Interface ist gesperrt
@param Address  0x0000003C
@param Mask     0x00000200
@param Shift    9
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P2_FX_MODE       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P2_FX_MODE       9
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P2_FX_MODE       0x00000000

/**
@defgroup       PHY_CONFIG__P2_AUTOMDIXEN P2_AUTOMDIXEN
@ingroup        PHY_CONFIG
@brief          1: Enable AutoMDIX state machine 0: Disable AutoMDIX state machine
@param Address  0x0000003C
@param Mask     0x00004000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG__MSK_P2_AUTOMDIXEN       0x00004000
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_SHFT_P2_AUTOMDIXEN       14
#define ERTEC200P_APBPER_SCRB_REG_PHY_CONFIG_RSTV_P2_AUTOMDIXEN       0x00000000


/**
@defgroup       PHY_STATUS PHY_STATUS
@ingroup        Register
@brief          Status von PHY1 und PHY2 Achtung: PHY_STATUS Register wird nur mit dem asynchronen Reset vom PN-IP zurückgesetzt
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS              0x00000040
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS_RST__VAL     0x00000000

/**
@defgroup       PHY_STATUS__P1_PWRUPRST P1_PWRUPRST
@ingroup        PHY_STATUS
@brief          0: PHY1 im Powerdown Mode oder internes Reset noch aktiv 1: PHY1 ist betriebsbereit
@param Address  0x00000040
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS__MSK_P1_PWRUPRST       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS_SHFT_P1_PWRUPRST       0
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS_RSTV_P1_PWRUPRST       0x00000000

/**
@defgroup       PHY_STATUS__P2_PWRUPRST P2_PWRUPRST
@ingroup        PHY_STATUS
@brief          0: PHY2 im Powerdown Mode oder internes Reset noch aktiv 1: PHY2 ist betriebsbereit
@param Address  0x00000040
@param Mask     0x00000100
@param Shift    8
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS__MSK_P2_PWRUPRST       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS_SHFT_P2_PWRUPRST       8
#define ERTEC200P_APBPER_SCRB_REG_PHY_STATUS_RSTV_P2_PWRUPRST       0x00000000


/**
@defgroup       AHB_BURSTBREAKER AHB_BURSTBREAKER
@ingroup        Register
@brief          Maximal number of beats in an ARM9 burst. Longer bursts are split by inserting an IDLE transfer.
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_AHB_BURSTBREAKER              0x00000044

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_ARM926_D NR_ADDR_ARM926_D
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at ARM926-D AHB port
@param Address  0x00000044
@param Mask     0x000000FF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_ARM926_D       0x000000FF
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_ARM926_D       0
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_ARM926_D       0x00000000

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_ARM926_D NR_ADDR_ARM926_D
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at ARM926-D AHB port
@param Address  0x00000044
@param Mask     0x000000FF
@param Shift    0
@param Access   r,w
@param Reset    0x00000008
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_ARM926_D       0x000000FF
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_ARM926_D       0
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_ARM926_D       0x00000008

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_ARM926_I NR_ADDR_ARM926_I
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at ARM926-I AHB port
@param Address  0x00000044
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_ARM926_I       0x0000FF00
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_ARM926_I       8
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_ARM926_I       0x00000000

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_ARM926_I NR_ADDR_ARM926_I
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at ARM926-I AHB port
@param Address  0x00000044
@param Mask     0x0000FF00
@param Shift    8
@param Access   r,w
@param Reset    0x00000008
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_ARM926_I       0x0000FF00
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_ARM926_I       8
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_ARM926_I       0x00000008

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_KRISC32 NR_ADDR_KRISC32
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at KRISC32 AHB port
@param Address  0x00000044
@param Mask     0x00FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_KRISC32       0x00FF0000
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_KRISC32       16
#define ERTEC200P_APBPER_SCRB_R1_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_KRISC32       0x00000000

/**
@defgroup       AHB_BURSTBREAKER__NR_ADDR_KRISC32 NR_ADDR_KRISC32
@ingroup        AHB_BURSTBREAKER
@brief          0: bypass 1-255: max. number of addresses at KRISC32 AHB port
@param Address  0x00000044
@param Mask     0x00FF0000
@param Shift    16
@param Access   r,w
@param Reset    0x00000008
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER__MSK_NR_ADDR_KRISC32       0x00FF0000
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_SHFT_NR_ADDR_KRISC32       16
#define ERTEC200P_APBPER_SCRB_R2_REG_AHB_BURSTBREAKER_RSTV_NR_ADDR_KRISC32       0x00000008


/**
@defgroup       LOCAL_OUT_READ_L LOCAL_OUT_READ_L
@ingroup        Register
@brief          Auswahl GPIO_In Pfad
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L              0x00000048
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RST__VAL     0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO32_IN_MUX GPIO32_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO32 Pad wird gelesen 1: LOC_IO0 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO32_IN_MUX       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO32_IN_MUX       0
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO32_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO33_IN_MUX GPIO33_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO33 Pad wird gelesen 1: LOC_IO1 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO33_IN_MUX       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO33_IN_MUX       1
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO33_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO34_IN_MUX GPIO34_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO34 Pad wird gelesen 1: LOC_IO2 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO34_IN_MUX       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO34_IN_MUX       2
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO34_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO35_IN_MUX GPIO35_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO35 Pad wird gelesen 1: LOC_IO3 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO35_IN_MUX       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO35_IN_MUX       3
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO35_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO36_IN_MUX GPIO36_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO36 Pad wird gelesen 1: LOC_IO4 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000010
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO36_IN_MUX       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO36_IN_MUX       4
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO36_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO37_IN_MUX GPIO37_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO37 Pad wird gelesen 1: LOC_IO5 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000020
@param Shift    5
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO37_IN_MUX       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO37_IN_MUX       5
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO37_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO38_IN_MUX GPIO38_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO38 Pad wird gelesen 1: LOC_IO6 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO38_IN_MUX       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO38_IN_MUX       6
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO38_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO39_IN_MUX GPIO39_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO39 Pad wird gelesen 1: LOC_IO7 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO39_IN_MUX       0x00000080
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO39_IN_MUX       7
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO39_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO40_IN_MUX GPIO40_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO40 Pad wird gelesen 1: LOC_IO8 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000100
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO40_IN_MUX       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO40_IN_MUX       8
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO40_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO41_IN_MUX GPIO41_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO41 Pad wird gelesen 1: LOC_IO9 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000200
@param Shift    9
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO41_IN_MUX       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO41_IN_MUX       9
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO41_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO42_IN_MUX GPIO42_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO42 Pad wird gelesen 1: LOC_IO10 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000400
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO42_IN_MUX       0x00000400
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO42_IN_MUX       10
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO42_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO43_IN_MUX GPIO43_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO43 Pad wird gelesen 1: LOC_IO11 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00000800
@param Shift    11
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO43_IN_MUX       0x00000800
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO43_IN_MUX       11
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO43_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO44_IN_MUX GPIO44_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO44 Pad wird gelesen 1: LOC_IO12 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00001000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO44_IN_MUX       0x00001000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO44_IN_MUX       12
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO44_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO45_IN_MUX GPIO45_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO45 Pad wird gelesen 1: LOC_IO13 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00002000
@param Shift    13
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO45_IN_MUX       0x00002000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO45_IN_MUX       13
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO45_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO46_IN_MUX GPIO46_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO46 Pad wird gelesen 1: LOC_IO14 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO46_IN_MUX       0x00004000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO46_IN_MUX       14
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO46_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO47_IN_MUX GPIO47_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO47 Pad wird gelesen 1: LOC_IO15 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO47_IN_MUX       0x00008000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO47_IN_MUX       15
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO47_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO48_IN_MUX GPIO48_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO48 Pad wird gelesen 1: LOC_IO16 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00010000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO48_IN_MUX       0x00010000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO48_IN_MUX       16
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO48_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO49_IN_MUX GPIO49_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO49 Pad wird gelesen 1: LOC_IO17 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00020000
@param Shift    17
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO49_IN_MUX       0x00020000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO49_IN_MUX       17
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO49_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO50_IN_MUX GPIO50_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO50 Pad wird gelesen 1: LOC_IO18 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00040000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO50_IN_MUX       0x00040000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO50_IN_MUX       18
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO50_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO51_IN_MUX GPIO51_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO51 Pad wird gelesen 1: LOC_IO19 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00080000
@param Shift    19
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO51_IN_MUX       0x00080000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO51_IN_MUX       19
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO51_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO52_IN_MUX GPIO52_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO52 Pad wird gelesen 1: LOC_IO20 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00100000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO52_IN_MUX       0x00100000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO52_IN_MUX       20
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO52_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO53_IN_MUX GPIO53_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO53 Pad wird gelesen 1: LOC_IO21 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00200000
@param Shift    21
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO53_IN_MUX       0x00200000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO53_IN_MUX       21
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO53_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO54_IN_MUX GPIO54_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO54 Pad wird gelesen 1: LOC_IO22 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00400000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO54_IN_MUX       0x00400000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO54_IN_MUX       22
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO54_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO55_IN_MUX GPIO55_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO55 Pad wird gelesen 1: LOC_IO23 Out wird gelesen
@param Address  0x00000048
@param Mask     0x00800000
@param Shift    23
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO55_IN_MUX       0x00800000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO55_IN_MUX       23
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO55_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO56_IN_MUX GPIO56_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO56 Pad wird gelesen 1: LOC_IO24 Out wird gelesen
@param Address  0x00000048
@param Mask     0x01000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO56_IN_MUX       0x01000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO56_IN_MUX       24
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO56_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO57_IN_MUX GPIO57_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO57 Pad wird gelesen 1: LOC_IO25 Out wird gelesen
@param Address  0x00000048
@param Mask     0x02000000
@param Shift    25
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO57_IN_MUX       0x02000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO57_IN_MUX       25
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO57_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO58_IN_MUX GPIO58_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO58 Pad wird gelesen 1: LOC_IO26 Out wird gelesen
@param Address  0x00000048
@param Mask     0x04000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO58_IN_MUX       0x04000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO58_IN_MUX       26
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO58_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO59_IN_MUX GPIO59_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO59 Pad wird gelesen 1: LOC_IO27 Out wird gelesen
@param Address  0x00000048
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO59_IN_MUX       0x08000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO59_IN_MUX       27
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO59_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO60_IN_MUX GPIO60_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO60 Pad wird gelesen 1: LOC_IO28 Out wird gelesen
@param Address  0x00000048
@param Mask     0x10000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO60_IN_MUX       0x10000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO60_IN_MUX       28
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO60_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO61_IN_MUX GPIO61_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO61 Pad wird gelesen 1: LOC_IO29 Out wird gelesen
@param Address  0x00000048
@param Mask     0x20000000
@param Shift    29
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO61_IN_MUX       0x20000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO61_IN_MUX       29
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO61_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO62_IN_MUX GPIO62_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO62 Pad wird gelesen 1: LOC_IO30 Out wird gelesen
@param Address  0x00000048
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO62_IN_MUX       0x40000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO62_IN_MUX       30
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO62_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_L__GPIO63_IN_MUX GPIO63_IN_MUX
@ingroup        LOCAL_OUT_READ_L
@brief          0: GPIO63 Pad wird gelesen 1: LOC_IO31 Out wird gelesen
@param Address  0x00000048
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L__MSK_GPIO63_IN_MUX       0x80000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_SHFT_GPIO63_IN_MUX       31
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_L_RSTV_GPIO63_IN_MUX       0x00000000


/**
@defgroup       LOCAL_OUT_READ_H LOCAL_OUT_READ_H
@ingroup        Register
@brief          Auswahl GPIO_In Pfad
@param Address  0x0000004C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H              0x0000004C
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RST__VAL     0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO64_IN_MUX GPIO64_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO64 Pad wird gelesen 1: LOC_IO32 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO64_IN_MUX       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO64_IN_MUX       0
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO64_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO65_IN_MUX GPIO65_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO65 Pad wird gelesen 1: LOC_IO33 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO65_IN_MUX       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO65_IN_MUX       1
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO65_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO66_IN_MUX GPIO66_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO66 Pad wird gelesen 1: LOC_IO34 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO66_IN_MUX       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO66_IN_MUX       2
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO66_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO67_IN_MUX GPIO67_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO67 Pad wird gelesen 1: LOC_IO35 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO67_IN_MUX       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO67_IN_MUX       3
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO67_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO68_IN_MUX GPIO68_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO68 Pad wird gelesen 1: LOC_IO36 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000010
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO68_IN_MUX       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO68_IN_MUX       4
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO68_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO69_IN_MUX GPIO69_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO69 Pad wird gelesen 1: LOC_IO71 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000020
@param Shift    5
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO69_IN_MUX       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO69_IN_MUX       5
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO69_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO70_IN_MUX GPIO70_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO70 Pad wird gelesen 1: LOC_IO38 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO70_IN_MUX       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO70_IN_MUX       6
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO70_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO71_IN_MUX GPIO71_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO71 Pad wird gelesen 1: LOC_IO39 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO71_IN_MUX       0x00000080
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO71_IN_MUX       7
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO71_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO72_IN_MUX GPIO72_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO72 Pad wird gelesen 1: LOC_IO40 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000100
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO72_IN_MUX       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO72_IN_MUX       8
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO72_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO73_IN_MUX GPIO73_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO73 Pad wird gelesen 1: LOC_IO41 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000200
@param Shift    9
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO73_IN_MUX       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO73_IN_MUX       9
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO73_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO74_IN_MUX GPIO74_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO74 Pad wird gelesen 1: LOC_IO42 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000400
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO74_IN_MUX       0x00000400
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO74_IN_MUX       10
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO74_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO75_IN_MUX GPIO75_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO75 Pad wird gelesen 1: LOC_IO43 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00000800
@param Shift    11
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO75_IN_MUX       0x00000800
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO75_IN_MUX       11
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO75_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO76_IN_MUX GPIO76_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO76 Pad wird gelesen 1: LOC_IO44 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00001000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO76_IN_MUX       0x00001000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO76_IN_MUX       12
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO76_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO77_IN_MUX GPIO77_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO77 Pad wird gelesen 1: LOC_IO45 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00002000
@param Shift    13
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO77_IN_MUX       0x00002000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO77_IN_MUX       13
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO77_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO78_IN_MUX GPIO78_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO78 Pad wird gelesen 1: LOC_IO46 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO78_IN_MUX       0x00004000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO78_IN_MUX       14
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO78_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO79_IN_MUX GPIO79_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO79 Pad wird gelesen 1: LOC_IO47 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO79_IN_MUX       0x00008000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO79_IN_MUX       15
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO79_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO80_IN_MUX GPIO80_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO80 Pad wird gelesen 1: LOC_IO48 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00010000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO80_IN_MUX       0x00010000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO80_IN_MUX       16
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO80_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO81_IN_MUX GPIO81_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO81 Pad wird gelesen 1: LOC_IO49 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00020000
@param Shift    17
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO81_IN_MUX       0x00020000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO81_IN_MUX       17
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO81_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO82_IN_MUX GPIO82_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO82 Pad wird gelesen 1: LOC_IO50 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00040000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO82_IN_MUX       0x00040000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO82_IN_MUX       18
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO82_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO83_IN_MUX GPIO83_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO83 Pad wird gelesen 1: LOC_IO51 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00080000
@param Shift    19
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO83_IN_MUX       0x00080000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO83_IN_MUX       19
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO83_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO84_IN_MUX GPIO84_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO84 Pad wird gelesen 1: LOC_IO52 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00100000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO84_IN_MUX       0x00100000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO84_IN_MUX       20
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO84_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO85_IN_MUX GPIO85_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO85 Pad wird gelesen 1: LOC_IO53 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00200000
@param Shift    21
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO85_IN_MUX       0x00200000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO85_IN_MUX       21
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO85_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO86_IN_MUX GPIO86_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO86 Pad wird gelesen 1: LOC_IO54 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00400000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO86_IN_MUX       0x00400000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO86_IN_MUX       22
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO86_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO87_IN_MUX GPIO87_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO87 Pad wird gelesen 1: LOC_IO55 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x00800000
@param Shift    23
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO87_IN_MUX       0x00800000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO87_IN_MUX       23
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO87_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO88_IN_MUX GPIO88_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO88 Pad wird gelesen 1: LOC_IO56 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x01000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO88_IN_MUX       0x01000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO88_IN_MUX       24
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO88_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO89_IN_MUX GPIO89_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO89 Pad wird gelesen 1: LOC_IO57 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x02000000
@param Shift    25
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO89_IN_MUX       0x02000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO89_IN_MUX       25
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO89_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO90_IN_MUX GPIO90_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO90 Pad wird gelesen 1: LOC_IO58 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x04000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO90_IN_MUX       0x04000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO90_IN_MUX       26
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO90_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO91_IN_MUX GPIO91_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO91 Pad wird gelesen 1: LOC_IO59 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x08000000
@param Shift    27
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO91_IN_MUX       0x08000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO91_IN_MUX       27
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO91_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO92_IN_MUX GPIO92_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO92 Pad wird gelesen 1: LOC_IO60 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x10000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO92_IN_MUX       0x10000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO92_IN_MUX       28
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO92_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO93_IN_MUX GPIO93_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO93 Pad wird gelesen 1: LOC_IO61 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x20000000
@param Shift    29
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO93_IN_MUX       0x20000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO93_IN_MUX       29
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO93_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO94_IN_MUX GPIO94_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO94 Pad wird gelesen 1: LOC_IO62 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x40000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO94_IN_MUX       0x40000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO94_IN_MUX       30
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO94_IN_MUX       0x00000000

/**
@defgroup       LOCAL_OUT_READ_H__GPIO95_IN_MUX GPIO95_IN_MUX
@ingroup        LOCAL_OUT_READ_H
@brief          0: GPIO95 Pad wird gelesen 1: LOC_IO63 Out wird gelesen
@param Address  0x0000004C
@param Mask     0x80000000
@param Shift    31
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H__MSK_GPIO95_IN_MUX       0x80000000
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_SHFT_GPIO95_IN_MUX       31
#define ERTEC200P_APBPER_SCRB_REG_LOCAL_OUT_READ_H_RSTV_GPIO95_IN_MUX       0x00000000


/**
@defgroup       CCR_I2C CCR_I2C
@ingroup        Register
@brief          Clock Control register for the I2C_3 interface divider value for determining the bit rate.
@param Address  0x00000050
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CCR_I2C              0x00000050
#define ERTEC200P_APBPER_SCRB_REG_CCR_I2C_RST__VAL     0x0000007C

/**
@defgroup       CCR_I2C__CDIV_VAL CDIV_VAL
@ingroup        CCR_I2C
@brief          Divider value for determining the bit rate fBR = fCLK/(CCR_I2C+1)- fCLK = fAPB=125 MHz in the I2C fBR: Bit rate clock (I2C) fCLK: I2C interface system cycle clock for fBR = 1MHz and fCLK=125MHz: CCR_I2C=124(dec.)
@param Address  0x00000050
@param Mask     0x000000FF
@param Shift    0
@param Access   r,w
@param Reset    0x0000007C
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CCR_I2C__MSK_CDIV_VAL       0x000000FF
#define ERTEC200P_APBPER_SCRB_REG_CCR_I2C_SHFT_CDIV_VAL       0
#define ERTEC200P_APBPER_SCRB_REG_CCR_I2C_RSTV_CDIV_VAL       0x0000007C


/**
@defgroup       EDC_EVENT EDC_EVENT
@ingroup        Register
@brief          EDC Event Register - zum Löschen muss das Register mit '0h' beschrieben werden
@param Address  0x00000054
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT              0x00000054
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RST__VAL     0x00000000

/**
@defgroup       EDC_EVENT__I_TCM926_1B I_TCM926_1B
@ingroup        EDC_EVENT
@brief          Im I-TCM des ARM926 ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_TCM926_1B       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_TCM926_1B       0
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_TCM926_1B       0x00000000

/**
@defgroup       EDC_EVENT__I_TCM926_2B I_TCM926_2B
@ingroup        EDC_EVENT
@brief          Im I-TCM des ARM926 ist ein 2Bit Error aufgetreten 
@param Address  0x00000054
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_TCM926_2B       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_TCM926_2B       1
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_TCM926_2B       0x00000000

/**
@defgroup       EDC_EVENT__D_TCM926_1B D_TCM926_1B
@ingroup        EDC_EVENT
@brief          Im D-TCM des ARM926 ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00000004
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_TCM926_1B       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_TCM926_1B       2
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_TCM926_1B       0x00000000

/**
@defgroup       EDC_EVENT__D_TCM926_2B D_TCM926_2B
@ingroup        EDC_EVENT
@brief          Im D-TCM des ARM926 ist ein 2Bit Error aufgetreten 
@param Address  0x00000054
@param Mask     0x00000008
@param Shift    3
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_TCM926_2B       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_TCM926_2B       3
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_TCM926_2B       0x00000000

/**
@defgroup       EDC_EVENT__GDMA_1B GDMA_1B
@ingroup        EDC_EVENT
@brief          Im GDMA Memory ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00000010
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_GDMA_1B       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_GDMA_1B       4
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_GDMA_1B       0x00000000

/**
@defgroup       EDC_EVENT__GDMA_2B GDMA_2B
@ingroup        EDC_EVENT
@brief          Im GDMA Memory ist ein 2Bit Error aufgetreten
@param Address  0x00000054
@param Mask     0x00000020
@param Shift    5
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_GDMA_2B       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_GDMA_2B       5
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_GDMA_2B       0x00000000

/**
@defgroup       EDC_EVENT__PN_1B PN_1B
@ingroup        EDC_EVENT
@brief          In einem der PN-IP Memories ist ein 1 Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00000040
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_PN_1B       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_PN_1B       6
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_PN_1B       0x00000000

/**
@defgroup       EDC_EVENT__PN_2B PN_2B
@ingroup        EDC_EVENT
@brief          In einem der PN-IP Memories ist ein 2 Bit Error aufgetreten
@param Address  0x00000054
@param Mask     0x00000080
@param Shift    7
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_PN_2B       0x00000080
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_PN_2B       7
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_PN_2B       0x00000000

/**
@defgroup       EDC_EVENT__PERIF_1B PERIF_1B
@ingroup        EDC_EVENT
@brief          Im PerlF Memory ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00000100
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_PERIF_1B       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_PERIF_1B       8
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_PERIF_1B       0x00000000

/**
@defgroup       EDC_EVENT__PERIF_2B PERIF_2B
@ingroup        EDC_EVENT
@brief          Im PerlF Memory ist ein 2Bit Error aufgetreten
@param Address  0x00000054
@param Mask     0x00000200
@param Shift    9
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_PERIF_2B       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_PERIF_2B       9
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_PERIF_2B       0x00000000

/**
@defgroup       EDC_EVENT__I_CACHE_PAR I_CACHE_PAR
@ingroup        EDC_EVENT
@brief          Im I-Chache ist beim Lesen ein Parity Error aufgetreten
@param Address  0x00000054
@param Mask     0x00000400
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_CACHE_PAR       0x00000400
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_CACHE_PAR       10
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_CACHE_PAR       0x00000000

/**
@defgroup       EDC_EVENT__D_CACHE_PAR D_CACHE_PAR
@ingroup        EDC_EVENT
@brief          Im D-Cache ist beim Lesen ein Parity Error aufgetreten
@param Address  0x00000054
@param Mask     0x00000800
@param Shift    11
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_CACHE_PAR       0x00000800
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_CACHE_PAR       11
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_CACHE_PAR       0x00000000

/**
@defgroup       EDC_EVENT__I_TAG_PAR I_TAG_PAR
@ingroup        EDC_EVENT
@brief          Im I-Tag RAM ist beim Lesen ein Parity Error aufgetreten
@param Address  0x00000054
@param Mask     0x00001000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_TAG_PAR       0x00001000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_TAG_PAR       12
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_TAG_PAR       0x00000000

/**
@defgroup       EDC_EVENT__D_TAG_PAR D_TAG_PAR
@ingroup        EDC_EVENT
@brief          Im D-Tag RAM ist beim Lesen ein Parity Error aufgetreten
@param Address  0x00000054
@param Mask     0x00002000
@param Shift    13
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_TAG_PAR       0x00002000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_TAG_PAR       13
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_TAG_PAR       0x00000000

/**
@defgroup       EDC_EVENT__I_TCM966_1B I_TCM966_1B
@ingroup        EDC_EVENT
@brief          Im I-TCM des ARM966 ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00004000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_TCM966_1B       0x00004000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_TCM966_1B       14
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_TCM966_1B       0x00000000

/**
@defgroup       EDC_EVENT__I_TCM966_2B I_TCM966_2B
@ingroup        EDC_EVENT
@brief          Im I-TCM des ARM966 ist ein 2Bit Error aufgetreten 
@param Address  0x00000054
@param Mask     0x00008000
@param Shift    15
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_I_TCM966_2B       0x00008000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_I_TCM966_2B       15
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_I_TCM966_2B       0x00000000

/**
@defgroup       EDC_EVENT__D_TCM966_1B D_TCM966_1B
@ingroup        EDC_EVENT
@brief          Im D-TCM des ARM966 ist ein 1Bit Error aufgetreten und wurde korrigiert
@param Address  0x00000054
@param Mask     0x00010000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_TCM966_1B       0x00010000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_TCM966_1B       16
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_TCM966_1B       0x00000000

/**
@defgroup       EDC_EVENT__D_TCM966_2B D_TCM966_2B
@ingroup        EDC_EVENT
@brief          Im D-TCM des ARM966 ist ein 2Bit Error aufgetreten 
@param Address  0x00000054
@param Mask     0x00020000
@param Shift    17
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT__MSK_D_TCM966_2B       0x00020000
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_SHFT_D_TCM966_2B       17
#define ERTEC200P_APBPER_SCRB_REG_EDC_EVENT_RSTV_D_TCM966_2B       0x00000000


/**
@defgroup       EDC_INIT_DONE EDC_INIT_DONE
@ingroup        Register
@brief          EDC Init Done Register - es kann der Status gelesen werden
@param Address  0x00000058
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE              0x00000058
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RST__VAL     0x00000000

/**
@defgroup       EDC_INIT_DONE__I_TCM926_INIT_DONE I_TCM926_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im I-TCM des ARM926 ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000001
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_I_TCM926_INIT_DONE       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_I_TCM926_INIT_DONE       0
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_I_TCM926_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__D_TCM926_INIT_DONE D_TCM926_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im D-TCM des ARM926 ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000002
@param Shift    1
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_D_TCM926_INIT_DONE       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_D_TCM926_INIT_DONE       1
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_D_TCM926_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__GDMA_INIT_DONE GDMA_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im GDMA ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000004
@param Shift    2
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_GDMA_INIT_DONE       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_GDMA_INIT_DONE       2
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_GDMA_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__PN_IP_INIT_DONE PN_IP_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im PN_IP ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000008
@param Shift    3
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_PN_IP_INIT_DONE       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_PN_IP_INIT_DONE       3
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_PN_IP_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__PERIF_INIT_DONE PERIF_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im PER-IF ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000010
@param Shift    4
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_PERIF_INIT_DONE       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_PERIF_INIT_DONE       4
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_PERIF_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__I_TCM966_INIT_DONE I_TCM966_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im I-TCM des ARM966 ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000020
@param Shift    5
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_I_TCM966_INIT_DONE       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_I_TCM966_INIT_DONE       5
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_I_TCM966_INIT_DONE       0x00000000

/**
@defgroup       EDC_INIT_DONE__D_TCM966_INIT_DONE D_TCM966_INIT_DONE
@ingroup        EDC_INIT_DONE
@brief          1: Die Initialisierung der EDC-Bits im D-TCM des ARM966 ist abgeschlossen
@param Address  0x00000058
@param Mask     0x00000040
@param Shift    6
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE__MSK_D_TCM966_INIT_DONE       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_SHFT_D_TCM966_INIT_DONE       6
#define ERTEC200P_APBPER_SCRB_REG_EDC_INIT_DONE_RSTV_D_TCM966_INIT_DONE       0x00000000


/**
@defgroup       TCM926_MAP TCM926_MAP
@ingroup        Register
@brief          TCM926 Map Register Aufteilung I / D-TCM und INITRAM
@param Address  0x0000005C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP              0x0000005C
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP_RST__VAL     0x00000000

/**
@defgroup       TCM926_MAP__PARTITION_TCM926 PARTITION_TCM926
@ingroup        TCM926_MAP
@brief          I-TCM / D-TCM ARM926 000b:  0 KB / 256 KB 001b: 64 KB / 192 KB 010b: 128 KB / 128 KB 011b: 192 KB / 64 KB 100b: 256 KB / 0 KB
@param Address  0x0000005C
@param Mask     0x00000007
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP__MSK_PARTITION_TCM926       0x00000007
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP_SHFT_PARTITION_TCM926       0
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP_RSTV_PARTITION_TCM926       0x00000000

/**
@defgroup       TCM926_MAP__INITRAM INITRAM
@ingroup        TCM926_MAP
@brief          Booten von I-TCM 0: kein Booten von I-TCM. Booten entsprechend der Parametrierung von MEM_SWAP. 1: I-TCM wird nach ARM926Core-Reset aktiviert
@param Address  0x0000005C
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP__MSK_INITRAM       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP_SHFT_INITRAM       3
#define ERTEC200P_APBPER_SCRB_REG_TCM926_MAP_RSTV_INITRAM       0x00000000


/**
@defgroup       GPIO_INT_POLSEL GPIO_INT_POLSEL
@ingroup        Register
@brief          Interrupt-Polarität für GPIO-Interrupts (15:0)
@param Address  0x00000060
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL              0x00000060
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RST__VAL     0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO0 INT_POLSEL_GPIO0
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO0 wird nicht invertiert zur A-ICU (IRQ32) und zum GDMA 1: GPIO0 wird invertiert zur A-ICU (IRQ32) und zum GDMA
@param Address  0x00000060
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO0       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO0       0
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO0       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO1 INT_POLSEL_GPIO1
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO1 wird nicht invertiert zur A-ICU (IRQ33) und zum GDMA 1: GPIO1 wird invertiert zur A-ICU (IRQ33) und zum GDMA
@param Address  0x00000060
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO1       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO1       1
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO1       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO2 INT_POLSEL_GPIO2
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO2 wird nicht invertiert zur A-ICU (IRQ34) und zum GDMA 1: GPIO2 wird invertiert zur A-ICU (IRQ34) und zum GDMA
@param Address  0x00000060
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO2       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO2       2
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO2       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO3 INT_POLSEL_GPIO3
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO3 wird nicht invertiert zur A-ICU (IRQ35) und zum GDMA 1: GPIO3 wird invertiert zur A-ICU (IRQ35) und zum GDMA
@param Address  0x00000060
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO3       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO3       3
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO3       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO4 INT_POLSEL_GPIO4
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO4 wird nicht invertiert zur A-ICU (IRQ36) und PN-ICU 1 1: GPIO4 wird invertiert zur A-ICU (IRQ36) und PN-ICU 1
@param Address  0x00000060
@param Mask     0x00000010
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO4       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO4       4
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO4       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO5 INT_POLSEL_GPIO5
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO5 wird nicht invertiert zur A-ICU (IRQ37) und PN-ICU 1 1: GPIO5 wird invertiert zur A-ICU (IRQ37) und PN-ICU 1
@param Address  0x00000060
@param Mask     0x00000020
@param Shift    5
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO5       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO5       5
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO5       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO6 INT_POLSEL_GPIO6
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO6 wird nicht invertiert zur A-ICU (IRQ38) und PN-ICU 1 1: GPIO6 wird invertiert zur A-ICU (IRQ38) und PN-ICU 1
@param Address  0x00000060
@param Mask     0x00000040
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO6       0x00000040
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO6       6
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO6       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO7 INT_POLSEL_GPIO7
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO7 wird nicht invertiert zur A-ICU (IRQ39) und PN-ICU 1 1: GPIO7 wird invertiert zur A-ICU (IRQ39) und PN-ICU 1
@param Address  0x00000060
@param Mask     0x00000080
@param Shift    7
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO7       0x00000080
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO7       7
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO7       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO8 INT_POLSEL_GPIO8
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO8 wird nicht invertiert zur A-ICU (IRQ40) 1: GPIO8 wird invertiert zur A-ICU (IRQ40)
@param Address  0x00000060
@param Mask     0x00000100
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO8       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO8       8
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO8       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO9 INT_POLSEL_GPIO9
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO9 wird nicht invertiert zur A-ICU (IRQ41) 1: GPIO9 wird invertiert zur A-ICU (IRQ41)
@param Address  0x00000060
@param Mask     0x00000200
@param Shift    9
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO9       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO9       9
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO9       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO10 INT_POLSEL_GPIO10
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO10 wird nicht invertiert zur A-ICU (IRQ42) 1: GPIO10 wird invertiert zur A-ICU (IRQ42)
@param Address  0x00000060
@param Mask     0x00000400
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO10       0x00000400
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO10       10
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO10       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO11 INT_POLSEL_GPIO11
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO11 wird nicht invertiert zur A-ICU (IRQ43) 1: GPIO11 wird invertiert zur A-ICU (IRQ43)
@param Address  0x00000060
@param Mask     0x00000800
@param Shift    11
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO11       0x00000800
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO11       11
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO11       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO12 INT_POLSEL_GPIO12
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO12 wird nicht invertiert zur A-ICU (IRQ44) 1: GPIO12 wird invertiert zur A-ICU (IRQ44)
@param Address  0x00000060
@param Mask     0x00001000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO12       0x00001000
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO12       12
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO12       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO13 INT_POLSEL_GPIO13
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO13 wird nicht invertiert zur A-ICU (IRQ45) 1: GPIO13 wird invertiert zur A-ICU (IRQ45)
@param Address  0x00000060
@param Mask     0x00002000
@param Shift    13
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO13       0x00002000
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO13       13
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO13       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO14 INT_POLSEL_GPIO14
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO14 wird nicht invertiert zur A-ICU (IRQ46) 1: GPIO14 wird invertiert zur A-ICU (IRQ46)
@param Address  0x00000060
@param Mask     0x00004000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO14       0x00004000
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO14       14
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO14       0x00000000

/**
@defgroup       GPIO_INT_POLSEL__INT_POLSEL_GPIO15 INT_POLSEL_GPIO15
@ingroup        GPIO_INT_POLSEL
@brief          0: GPIO15 wird nicht invertiert zur A-ICU (IRQ47) 1: GPIO15 wird invertiert zur A-ICU (IRQ47)
@param Address  0x00000060
@param Mask     0x00008000
@param Shift    15
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL__MSK_INT_POLSEL_GPIO15       0x00008000
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_SHFT_INT_POLSEL_GPIO15       15
#define ERTEC200P_APBPER_SCRB_REG_GPIO_INT_POLSEL_RSTV_INT_POLSEL_GPIO15       0x00000000


/**
@defgroup       EDC_PARITY_EN EDC_PARITY_EN
@ingroup        Register
@brief          EDC- and Parity Enable (2:0)
@param Address  0x00000064
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN              0x00000064
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_RST__VAL     0x00000000

/**
@defgroup       EDC_PARITY_EN__I_CACHE_PAR_EN I_CACHE_PAR_EN
@ingroup        EDC_PARITY_EN
@brief          0: Parity Logik für ARM926 I-Cache / I-Tag ist disabled (default) 1: Parity Logik für ARM926 I-Cache / I-Tag ist enabled
@param Address  0x00000064
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN__MSK_I_CACHE_PAR_EN       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_SHFT_I_CACHE_PAR_EN       0
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_RSTV_I_CACHE_PAR_EN       0x00000000

/**
@defgroup       EDC_PARITY_EN__D_CACHE_PAR_EN D_CACHE_PAR_EN
@ingroup        EDC_PARITY_EN
@brief          0: Parity Logik für ARM926 D-Cache / D-Tag ist disabled (default) 1: Parity Logik für ARM926 D-Cache / D-Tag ist enabled
@param Address  0x00000064
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN__MSK_D_CACHE_PAR_EN       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_SHFT_D_CACHE_PAR_EN       1
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_RSTV_D_CACHE_PAR_EN       0x00000000

/**
@defgroup       EDC_PARITY_EN__EDC_DISABLE_ARM926 EDC_DISABLE_ARM926
@ingroup        EDC_PARITY_EN
@brief          0: Die EDC Logik im ARM926 I- / D-TCM ist enabled (default) 1: Die EDC Logik im ARM926 I- / D-TCM ist disabled
@param Address  0x00000064
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN__MSK_EDC_DISABLE_ARM926       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_SHFT_EDC_DISABLE_ARM926       2
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_RSTV_EDC_DISABLE_ARM926       0x00000000

/**
@defgroup       EDC_PARITY_EN__EDC_DISABLE_ARM966 EDC_DISABLE_ARM966
@ingroup        EDC_PARITY_EN
@brief          0: Die EDC Logik im ARM966 I- / D-TCM ist enabled (default) 1: Die EDC Logik im ARM966 I- / D-TCM ist disabled
@param Address  0x00000064
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN__MSK_EDC_DISABLE_ARM966       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_SHFT_EDC_DISABLE_ARM966       3
#define ERTEC200P_APBPER_SCRB_REG_EDC_PARITY_EN_RSTV_EDC_DISABLE_ARM966       0x00000000


/**
@defgroup       MODUL_ACCESS_ERR MODUL_ACCESS_ERR
@ingroup        Register
@brief          Modul Access Error Register (5:0) - zum Löschen muss das Register mit '0'h beschrieben werden
@param Address  0x00000068
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR              0x00000068
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RST__VAL     0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__PN_IP_ACCESS_ERR PN_IP_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im PN-IP ist kein Access Error aufgetreten 1: Im PN-IP ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_PN_IP_ACCESS_ERR       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_PN_IP_ACCESS_ERR       0
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_PN_IP_ACCESS_ERR       0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__PER_IF_ACCESS_ERR PER_IF_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im PER-IF ist kein Access Error aufgetreten 1: Im PER-IF ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_PER_IF_ACCESS_ERR       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_PER_IF_ACCESS_ERR       1
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_PER_IF_ACCESS_ERR       0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__I_FILTER_ACCESS_ERR I_FILTER_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im I-Filter ist kein Access Error aufgetreten 1: Im I-Filter ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000004
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_I_FILTER_ACCESS_ERR       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_I_FILTER_ACCESS_ERR       2
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_I_FILTER_ACCESS_ERR       0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__HOST_IF_ACCESS_ERR HOST_IF_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im HOST-IF ist kein Access Error aufgetreten 1: Im HOST-IF ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000008
@param Shift    3
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_HOST_IF_ACCESS_ERR       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_HOST_IF_ACCESS_ERR       3
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_HOST_IF_ACCESS_ERR       0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__SCRB_ACCESS_ERR SCRB_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im SCRB ist kein Access Error aufgetreten 1: Im SCRB ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000010
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_SCRB_ACCESS_ERR       0x00000010
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_SCRB_ACCESS_ERR       4
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_SCRB_ACCESS_ERR       0x00000000

/**
@defgroup       MODUL_ACCESS_ERR__SERDIDO_ACCESS_ERR SERDIDO_ACCESS_ERR
@ingroup        MODUL_ACCESS_ERR
@brief          0: Im SERDIDO ist kein Access Error aufgetreten 1: Im SERDIDO ist ein Access Error aufgetreten
@param Address  0x00000068
@param Mask     0x00000020
@param Shift    5
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR__MSK_SERDIDO_ACCESS_ERR       0x00000020
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_SHFT_SERDIDO_ACCESS_ERR       5
#define ERTEC200P_APBPER_SCRB_REG_MODUL_ACCESS_ERR_RSTV_SERDIDO_ACCESS_ERR       0x00000000


/**
@defgroup       RES_SOFT_RETURN_ADDR RES_SOFT_RETURN_ADDR
@ingroup        Register
@brief          Einsprungadresse für den Secondary Bootloader nach einem SW-Reset ARM926EJ-S
@param Address  0x0000006C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_SOFT_RETURN_ADDR              0x0000006C
#define ERTEC200P_APBPER_SCRB_REG_RES_SOFT_RETURN_ADDR_RST__VAL     0x00000000

/**
@defgroup       RES_SOFT_RETURN_ADDR__RETURN_ADDRESS RETURN_ADDRESS
@ingroup        RES_SOFT_RETURN_ADDR
@brief          Einsprungadresse für den Secondary Bootloader nach einem RES_SOFT_ARM926_CORE. Dieser SW-Reset erfolgt nach der Parametrierung der TCM 926 Konfiguration im TCM926_MAP Register.
@param Address  0x0000006C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_RES_SOFT_RETURN_ADDR__MSK_RETURN_ADDRESS       0xFFFFFFFF
#define ERTEC200P_APBPER_SCRB_REG_RES_SOFT_RETURN_ADDR_SHFT_RETURN_ADDRESS       0
#define ERTEC200P_APBPER_SCRB_REG_RES_SOFT_RETURN_ADDR_RSTV_RETURN_ADDRESS       0x00000000


/**
@defgroup       PHY_LED_CONTROL PHY_LED_CONTROL
@ingroup        Register
@brief          PHY-LEDs Steuerung
@param Address  0x00000070
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL              0x00000070
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RST__VAL     0x00000000

/**
@defgroup       PHY_LED_CONTROL__EN_P1_XLINK_STATUS EN_P1_XLINK_STATUS
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P1_XLINK_STATUS wird von PHY gesteuert 1: LED: P1_XLINK_STATUS wird von der SW gesteuert
@param Address  0x00000070
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_EN_P1_XLINK_STATUS       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_EN_P1_XLINK_STATUS       0
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_EN_P1_XLINK_STATUS       0x00000000

/**
@defgroup       PHY_LED_CONTROL__EN_P1_XACTIVITY EN_P1_XACTIVITY
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P1_XLINK_XACTIVITY wird von PHY gesteuert 1: LED: P1_XLINK_XACTIVITY wird von der SW gesteuert
@param Address  0x00000070
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_EN_P1_XACTIVITY       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_EN_P1_XACTIVITY       1
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_EN_P1_XACTIVITY       0x00000000

/**
@defgroup       PHY_LED_CONTROL__EN_P2_XLINK_STATUS EN_P2_XLINK_STATUS
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P2_XLINK_STATUS wird von PHY gesteuert 1: LED: P2_XLINK_STATUS wird von der SW gesteuert
@param Address  0x00000070
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_EN_P2_XLINK_STATUS       0x00000004
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_EN_P2_XLINK_STATUS       2
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_EN_P2_XLINK_STATUS       0x00000000

/**
@defgroup       PHY_LED_CONTROL__EN_P2_XACTIVITY EN_P2_XACTIVITY
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P2_XLINK_XACTIVITY wird von PHY gesteuert 1: LED: P2_XLINK_XACTIVITY wird von der SW gesteuert
@param Address  0x00000070
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_EN_P2_XACTIVITY       0x00000008
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_EN_P2_XACTIVITY       3
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_EN_P2_XACTIVITY       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P1_XLINK_STATUS_SW P1_XLINK_STATUS_SW
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P1_XLINK_STATUS ist '0', gesetzt von der SW 1: LED: P1_XLINK_STATUS ist '1', gesetzt von der SW
@param Address  0x00000070
@param Mask     0x00000100
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P1_XLINK_STATUS_SW       0x00000100
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P1_XLINK_STATUS_SW       8
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P1_XLINK_STATUS_SW       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P1_XACTIVITY_SW P1_XACTIVITY_SW
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P1_XLINK_XACTIVITY ist '0', gesetzt von der SW 1: LED: P1_XLINK_XACTIVITY ist '1', gesetzt von der SW
@param Address  0x00000070
@param Mask     0x00000200
@param Shift    9
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P1_XACTIVITY_SW       0x00000200
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P1_XACTIVITY_SW       9
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P1_XACTIVITY_SW       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P2_XLINK_STATUS_SW P2_XLINK_STATUS_SW
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P2_XLINK_STATUS ist '0', gesetzt von der SW 1: LED: P2_XLINK_STATUS ist '1', gesetzt von der SW
@param Address  0x00000070
@param Mask     0x00000400
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P2_XLINK_STATUS_SW       0x00000400
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P2_XLINK_STATUS_SW       10
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P2_XLINK_STATUS_SW       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P2_XACTIVITY_SW P2_XACTIVITY_SW
@ingroup        PHY_LED_CONTROL
@brief          0: LED: P2_XLINK_XACTIVITY ist '0', gesetzt von der SW 1: LED: P2_XLINK_XACTIVITY ist '1', gesetzt von der SW
@param Address  0x00000070
@param Mask     0x00000800
@param Shift    11
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P2_XACTIVITY_SW       0x00000800
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P2_XACTIVITY_SW       11
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P2_XACTIVITY_SW       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P1_XLINK_STATUS_PHY P1_XLINK_STATUS_PHY
@ingroup        PHY_LED_CONTROL
@brief          P1_XLINK_STATUS gesteuert vom PHY
@param Address  0x00000070
@param Mask     0x00010000
@param Shift    16
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P1_XLINK_STATUS_PHY       0x00010000
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P1_XLINK_STATUS_PHY       16
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P1_XLINK_STATUS_PHY       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P1_XACTIVITY_PHY P1_XACTIVITY_PHY
@ingroup        PHY_LED_CONTROL
@brief          P1_XACTIVITY gesteuert vom PHY
@param Address  0x00000070
@param Mask     0x00020000
@param Shift    17
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P1_XACTIVITY_PHY       0x00020000
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P1_XACTIVITY_PHY       17
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P1_XACTIVITY_PHY       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P2_XLINK_STATUS_PHY P2_XLINK_STATUS_PHY
@ingroup        PHY_LED_CONTROL
@brief          P2_XLINK_STATUS gesteuert vom PHY
@param Address  0x00000070
@param Mask     0x00040000
@param Shift    18
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P2_XLINK_STATUS_PHY       0x00040000
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P2_XLINK_STATUS_PHY       18
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P2_XLINK_STATUS_PHY       0x00000000

/**
@defgroup       PHY_LED_CONTROL__P2_XACTIVITY_PHY P2_XACTIVITY_PHY
@ingroup        PHY_LED_CONTROL
@brief          P2_XACTIVITY gesteuert vom PHY
@param Address  0x00000070
@param Mask     0x00080000
@param Shift    19
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL__MSK_P2_XACTIVITY_PHY       0x00080000
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_SHFT_P2_XACTIVITY_PHY       19
#define ERTEC200P_APBPER_SCRB_REG_PHY_LED_CONTROL_RSTV_P2_XACTIVITY_PHY       0x00000000


/**
@defgroup       ACCESS_ERROR_SCRB ACCESS_ERROR_SCRB
@ingroup        Register
@brief          SCRB Access Error
@param Address  0x00000074
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB              0x00000074
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_RST__VAL     0x20000000

/**
@defgroup       ACCESS_ERROR_SCRB__APB_ADDRESS APB_ADDRESS
@ingroup        ACCESS_ERROR_SCRB
@brief          fehlerhafte APB-Adresse im SCRB
@param Address  0x00000074
@param Mask     0x000000FF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB__MSK_APB_ADDRESS       0x000000FF
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_SHFT_APB_ADDRESS       0
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_RSTV_APB_ADDRESS       0x00000000

/**
@defgroup       ACCESS_ERROR_SCRB__APB_SIZE APB_SIZE
@ingroup        ACCESS_ERROR_SCRB
@brief          fix auf 10 (word access) geklemmt, da nur Wortzugriffe erlaubt
@param Address  0x00000074
@param Mask     0x30000000
@param Shift    28
@param Access   r,
@param Reset    0x00000002
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB__MSK_APB_SIZE       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_SHFT_APB_SIZE       28
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_RSTV_APB_SIZE       0x00000002

/**
@defgroup       ACCESS_ERROR_SCRB__APB_WRITE APB_WRITE
@ingroup        ACCESS_ERROR_SCRB
@brief          0: Read-Zugriff 1: Write-Zugriff
@param Address  0x00000074
@param Mask     0x40000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB__MSK_APB_WRITE       0x40000000
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_SHFT_APB_WRITE       30
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_RSTV_APB_WRITE       0x00000000

/**
@defgroup       ACCESS_ERROR_SCRB__ERR_LOCK ERR_LOCK
@ingroup        ACCESS_ERROR_SCRB
@brief          Wird durch die HW auf 1 gesetzt, wenn ein fehlerhafter SCRB-Zugriff erkannt wird. Weitere HW-Einträge werden dadurch gesperrt. Die SW muss zur Freigabe neuer Einträge das Bit wieder auf 0 zurücksetzen.
@param Address  0x00000074
@param Mask     0x80000000
@param Shift    31
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB__MSK_ERR_LOCK       0x80000000
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_SHFT_ERR_LOCK       31
#define ERTEC200P_APBPER_SCRB_REG_ACCESS_ERROR_SCRB_RSTV_ERR_LOCK       0x00000000


/**
@defgroup       DRIVE_EMC DRIVE_EMC
@ingroup        Register
@brief          SCRB Drive Current of Dedicated Signals (1.8V )  jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt  -----1.8V-------- 00 - 4 mA  01 - 6 mA 10 - 8 mA 11 - 12 mA und Enable/Disable der Takte für BF und SDRAM
@param Address  0x00000078
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC              0x00000078

/**
@defgroup       DRIVE_EMC__DR_EMC_C DR_EMC_C
@ingroup        DRIVE_EMC
@brief          Signalliste: DTXR, XOE_DDRIVER
@param Address  0x00000078
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_C       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_C       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_C       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_AL DR_EMC_AL
@ingroup        DRIVE_EMC
@brief          Signalliste: A14 - A0
@param Address  0x00000078
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_AL       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_AL       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_AL       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_AH DR_EMC_AH
@ingroup        DRIVE_EMC
@brief          Signalliste: A23 - A15
@param Address  0x00000078
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_AH       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_AH       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_AH       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_DL DR_EMC_DL
@ingroup        DRIVE_EMC
@brief          Signalliste: D15 - D0, XBE0_DQM0, XBE1_XBE1
@param Address  0x00000078
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_DL       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_DL       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_DL       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_DH DR_EMC_DH
@ingroup        DRIVE_EMC
@brief          Signalliste: D31 - D16, XBE2_DQM2, XBE3_XBE3
@param Address  0x00000078
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_DH       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_DH       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_DH       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_RW DR_EMC_RW
@ingroup        DRIVE_EMC
@brief          Signalliste: XWR, XRD
@param Address  0x00000078
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_RW       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_RW       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_RW       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_PER DR_EMC_PER
@ingroup        DRIVE_EMC
@brief          Signalliste: XCS_PER0 - 3
@param Address  0x00000078
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_PER       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_PER       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_PER       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_CLK_SDRAM DR_EMC_CLK_SDRAM
@ingroup        DRIVE_EMC
@brief          Signalliste:CLK_O_SDRAM0, CLK_O_SDRAM1, CLK_O_SDRAM2
@param Address  0x00000078
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_CLK_SDRAM       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_CLK_SDRAM       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_CLK_SDRAM       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_SDRAM DR_EMC_SDRAM
@ingroup        DRIVE_EMC
@brief          Signalliste: XCS_SDRAM, XRAS_SDRAM, XCAS_SDRAM, XWE_SDRAM
@param Address  0x00000078
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_SDRAM       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_SDRAM       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_SDRAM       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_CLK_BF DR_EMC_CLK_BF
@ingroup        DRIVE_EMC
@brief          Signalliste: CLK_O_BF0 - 2
@param Address  0x00000078
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_CLK_BF       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_CLK_BF       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_CLK_BF       0x00000003

/**
@defgroup       DRIVE_EMC__DR_EMC_XAV_BF DR_EMC_XAV_BF
@ingroup        DRIVE_EMC
@brief          Signalliste: XAV_BF
@param Address  0x00000078
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000003
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_DR_EMC_XAV_BF       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_DR_EMC_XAV_BF       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_DR_EMC_XAV_BF       0x00000003

/**
@defgroup       DRIVE_EMC__EMC_SDRAM_CLK_SD0SD1_EBL EMC_SDRAM_CLK_SD0SD1_EBL
@ingroup        DRIVE_EMC
@brief          enable(=1) / disable(=0)  CLK_O_SDRAM0 und CLK_O_SDRAM1 (ein SDRAM-Baustein)
@param Address  0x00000078
@param Mask     0x00400000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_EMC_SDRAM_CLK_SD0SD1_EBL       0x00400000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_EMC_SDRAM_CLK_SD0SD1_EBL       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_EMC_SDRAM_CLK_SD0SD1_EBL       0x00000001

/**
@defgroup       DRIVE_EMC__EMC_SDRAM_CLK_SD2_EBL EMC_SDRAM_CLK_SD2_EBL
@ingroup        DRIVE_EMC
@brief          enable(=1) / disable(=0)  CLK_O_SDRAM2 (zusätzlich 2. SDRAM-Baustein)
@param Address  0x00000078
@param Mask     0x00800000
@param Shift    23
@param Access   r,w
@param Reset    0x00000001
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_DRIVE_EMC__MSK_EMC_SDRAM_CLK_SD2_EBL       0x00800000
#define ERTEC200P_APBPER_SCRB_R2_REG_DRIVE_EMC_SHFT_EMC_SDRAM_CLK_SD2_EBL       23
#define ERTEC200P_APBPER_SCRB_R2_REG_DRIVE_EMC_RSTV_EMC_SDRAM_CLK_SD2_EBL       0x00000001

/**
@defgroup       DRIVE_EMC__EMC_SDRAM_CLK_SD2_EBL EMC_SDRAM_CLK_SD2_EBL
@ingroup        DRIVE_EMC
@brief          enable(=1) / disable(=0)  CLK_O_SDRAM2 (zusätzlich 2. SDRAM-Baustein)
@param Address  0x00000078
@param Mask     0x00800000
@param Shift    23
@param Access   r,w
@param Reset    0x00000000
@param Version  1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R1_REG_DRIVE_EMC__MSK_EMC_SDRAM_CLK_SD2_EBL       0x00800000
#define ERTEC200P_APBPER_SCRB_R1_REG_DRIVE_EMC_SHFT_EMC_SDRAM_CLK_SD2_EBL       23
#define ERTEC200P_APBPER_SCRB_R1_REG_DRIVE_EMC_RSTV_EMC_SDRAM_CLK_SD2_EBL       0x00000000

/**
@defgroup       DRIVE_EMC__EMC_BF_CLK_BF0BF1_EBL EMC_BF_CLK_BF0BF1_EBL
@ingroup        DRIVE_EMC
@brief          enable(=1) / disable(=0)  CLK_O_BF0 und CLK_O_BF1 (ein Flash-Baustein)
@param Address  0x00000078
@param Mask     0x01000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_EMC_BF_CLK_BF0BF1_EBL       0x01000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_EMC_BF_CLK_BF0BF1_EBL       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_EMC_BF_CLK_BF0BF1_EBL       0x00000000

/**
@defgroup       DRIVE_EMC__EMC_BF_CLK_BF2_EBL EMC_BF_CLK_BF2_EBL
@ingroup        DRIVE_EMC
@brief          enable(=1) / disable(=0)  CLK_O_BF2 (zusätzlich 2. Flash-Baustein)
@param Address  0x00000078
@param Mask     0x02000000
@param Shift    25
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC__MSK_EMC_BF_CLK_BF2_EBL       0x02000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_SHFT_EMC_BF_CLK_BF2_EBL       25
#define ERTEC200P_APBPER_SCRB_REG_DRIVE_EMC_RSTV_EMC_BF_CLK_BF2_EBL       0x00000000


/**
@defgroup       DRIVE15_0GPIO DRIVE15_0GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt 00 - 4 mA 01 - 6 mA 10 - 8 mA 11 - 12mA
@param Address  0x0000007C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO              0x0000007C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO0 DR_GPIO0
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO0
@param Address  0x0000007C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO0       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO0       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO0       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO1 DR_GPIO1
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO1
@param Address  0x0000007C
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO1       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO1       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO1       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO2 DR_GPIO2
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO2
@param Address  0x0000007C
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO2       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO2       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO2       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO3 DR_GPIO3
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO3
@param Address  0x0000007C
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO3       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO3       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO3       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO4 DR_GPIO4
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO4
@param Address  0x0000007C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO4       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO4       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO4       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO5 DR_GPIO5
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO5
@param Address  0x0000007C
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO5       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO5       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO5       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO6 DR_GPIO6
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO6
@param Address  0x0000007C
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO6       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO6       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO6       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO7 DR_GPIO7
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO7
@param Address  0x0000007C
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO7       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO7       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO7       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO8 DR_GPIO8
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO8
@param Address  0x0000007C
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO8       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO8       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO8       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO9 DR_GPIO9
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO9
@param Address  0x0000007C
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO9       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO9       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO9       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO10 DR_GPIO10
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO10
@param Address  0x0000007C
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO10       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO10       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO10       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO11 DR_GPIO11
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO11
@param Address  0x0000007C
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO11       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO11       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO11       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO12 DR_GPIO12
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO12
@param Address  0x0000007C
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO12       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO12       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO12       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO13 DR_GPIO13
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO13
@param Address  0x0000007C
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO13       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO13       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO13       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO14 DR_GPIO14
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO14
@param Address  0x0000007C
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO14       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO14       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO14       0x00000001

/**
@defgroup       DRIVE15_0GPIO__DR_GPIO15 DR_GPIO15
@ingroup        DRIVE15_0GPIO
@brief          drive current GPIO15
@param Address  0x0000007C
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO__MSK_DR_GPIO15       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_SHFT_DR_GPIO15       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE15_0GPIO_RSTV_DR_GPIO15       0x00000001


/**
@defgroup       DRIVE31_16GPIO DRIVE31_16GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt 00 - 4 mA 01 - 6 mA 10 - 8 mA 11 - 12mA
@param Address  0x00000080
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO              0x00000080
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO16 DR_GPIO16
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO16
@param Address  0x00000080
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO16       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO16       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO16       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO17 DR_GPIO17
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO17
@param Address  0x00000080
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO17       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO17       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO17       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO18 DR_GPIO18
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO18
@param Address  0x00000080
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO18       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO18       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO18       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO19 DR_GPIO19
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO19
@param Address  0x00000080
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO19       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO19       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO19       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO20 DR_GPIO20
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO20
@param Address  0x00000080
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO20       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO20       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO20       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO21 DR_GPIO21
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO21
@param Address  0x00000080
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO21       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO21       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO21       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO22 DR_GPIO22
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO22
@param Address  0x00000080
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO22       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO22       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO22       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO23 DR_GPIO23
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO23
@param Address  0x00000080
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO23       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO23       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO23       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO24 DR_GPIO24
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO24
@param Address  0x00000080
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO24       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO24       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO24       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO25 DR_GPIO25
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO25
@param Address  0x00000080
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO25       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO25       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO25       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO26 DR_GPIO26
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO26
@param Address  0x00000080
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO26       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO26       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO26       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO27 DR_GPIO27
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO27
@param Address  0x00000080
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO27       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO27       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO27       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO28 DR_GPIO28
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO28
@param Address  0x00000080
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO28       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO28       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO28       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO29 DR_GPIO29
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO29
@param Address  0x00000080
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO29       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO29       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO29       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO30 DR_GPIO30
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO30
@param Address  0x00000080
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO30       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO30       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO30       0x00000001

/**
@defgroup       DRIVE31_16GPIO__DR_GPIO31 DR_GPIO31
@ingroup        DRIVE31_16GPIO
@brief          drive current GPIO31
@param Address  0x00000080
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO__MSK_DR_GPIO31       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_SHFT_DR_GPIO31       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE31_16GPIO_RSTV_DR_GPIO31       0x00000001


/**
@defgroup       DRIVE47_32GPIO DRIVE47_32GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals (3.3V / 1.8V ) jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt -----3.3V---------     -----1.8V-------- 00 -  6 mA             00 - 3 mA (n.a.) 01 - 12 mA            01 - 6 mA 10 - 18 mA (n.a.)  10 - 9 mA 11 - 24mA  (n.a.)  11 - 12 mA
@param Address  0x00000084
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO              0x00000084
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO32 DR_GPIO32
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO32
@param Address  0x00000084
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO32       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO32       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO32       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO33 DR_GPIO33
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO33
@param Address  0x00000084
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO33       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO33       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO33       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO34 DR_GPIO34
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO34
@param Address  0x00000084
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO34       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO34       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO34       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO35 DR_GPIO35
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO35
@param Address  0x00000084
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO35       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO35       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO35       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO36 DR_GPIO36
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO36
@param Address  0x00000084
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO36       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO36       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO36       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO37 DR_GPIO37
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO37
@param Address  0x00000084
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO37       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO37       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO37       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO38 DR_GPIO38
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO38
@param Address  0x00000084
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO38       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO38       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO38       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO39 DR_GPIO39
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO39
@param Address  0x00000084
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO39       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO39       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO39       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO40 DR_GPIO40
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO40
@param Address  0x00000084
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO40       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO40       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO40       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO41 DR_GPIO41
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO41
@param Address  0x00000084
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO41       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO41       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO41       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO42 DR_GPIO42
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO42
@param Address  0x00000084
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO42       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO42       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO42       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO43 DR_GPIO43
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO43
@param Address  0x00000084
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO43       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO43       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO43       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO44 DR_GPIO44
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO44
@param Address  0x00000084
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO44       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO44       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO44       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO45 DR_GPIO45
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO45
@param Address  0x00000084
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO45       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO45       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO45       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO46 DR_GPIO46
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO46
@param Address  0x00000084
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO46       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO46       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO46       0x00000001

/**
@defgroup       DRIVE47_32GPIO__DR_GPIO47 DR_GPIO47
@ingroup        DRIVE47_32GPIO
@brief          drive current GPIO47
@param Address  0x00000084
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO__MSK_DR_GPIO47       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_SHFT_DR_GPIO47       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE47_32GPIO_RSTV_DR_GPIO47       0x00000001


/**
@defgroup       DRIVE63_48GPIO DRIVE63_48GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals (3.3V / 1.8V ) jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt -----3.3V---------     -----1.8V-------- 00 -  6 mA             00 - 3 mA (n.a.) 01 - 12 mA            01 - 6 mA 10 - 18 mA (n.a.)  10 - 9 mA 11 - 24mA  (n.a.)  11 - 12 mA
@param Address  0x00000088
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO              0x00000088
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO48 DR_GPIO48
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO48
@param Address  0x00000088
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO48       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO48       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO48       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO49 DR_GPIO49
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO49
@param Address  0x00000088
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO49       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO49       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO49       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO50 DR_GPIO50
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO50
@param Address  0x00000088
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO50       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO50       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO50       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO51 DR_GPIO51
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO51
@param Address  0x00000088
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO51       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO51       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO51       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO52 DR_GPIO52
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO52
@param Address  0x00000088
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO52       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO52       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO52       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO53 DR_GPIO53
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO53
@param Address  0x00000088
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO53       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO53       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO53       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO54 DR_GPIO54
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO54
@param Address  0x00000088
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO54       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO54       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO54       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO55 DR_GPIO55
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO55
@param Address  0x00000088
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO55       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO55       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO55       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO56 DR_GPIO56
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO56
@param Address  0x00000088
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO56       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO56       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO56       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO57 DR_GPIO57
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO57
@param Address  0x00000088
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO57       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO57       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO57       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO58 DR_GPIO58
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO58
@param Address  0x00000088
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO58       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO58       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO58       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO59 DR_GPIO59
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO59
@param Address  0x00000088
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO59       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO59       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO59       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO60 DR_GPIO60
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO60
@param Address  0x00000088
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO60       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO60       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO60       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO61 DR_GPIO61
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO61
@param Address  0x00000088
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO61       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO61       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO61       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO62 DR_GPIO62
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO62
@param Address  0x00000088
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO62       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO62       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO62       0x00000001

/**
@defgroup       DRIVE63_48GPIO__DR_GPIO63 DR_GPIO63
@ingroup        DRIVE63_48GPIO
@brief          drive current GPIO63
@param Address  0x00000088
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO__MSK_DR_GPIO63       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_SHFT_DR_GPIO63       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE63_48GPIO_RSTV_DR_GPIO63       0x00000001


/**
@defgroup       DRIVE79_64GPIO DRIVE79_64GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals (3.3V / 1.8V ) jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt -----3.3V---------     -----1.8V-------- 00 -  6 mA             00 - 3 mA (n.a.) 01 - 12 mA            01 - 6 mA 10 - 18 mA (n.a.)  10 - 9 mA 11 - 24mA  (n.a.)  11 - 12 mA
@param Address  0x0000008C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO              0x0000008C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO64 DR_GPIO64
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO64
@param Address  0x0000008C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO64       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO64       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO64       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO65 DR_GPIO65
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO65
@param Address  0x0000008C
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO65       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO65       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO65       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO66 DR_GPIO66
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO66
@param Address  0x0000008C
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO66       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO66       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO66       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO67 DR_GPIO67
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO67
@param Address  0x0000008C
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO67       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO67       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO67       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO68 DR_GPIO68
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO68
@param Address  0x0000008C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO68       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO68       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO68       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO69 DR_GPIO69
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO69
@param Address  0x0000008C
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO69       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO69       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO69       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO70 DR_GPIO70
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO70
@param Address  0x0000008C
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO70       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO70       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO70       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO71 DR_GPIO71
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO71
@param Address  0x0000008C
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO71       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO71       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO71       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO72 DR_GPIO72
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO72
@param Address  0x0000008C
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO72       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO72       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO72       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO73 DR_GPIO73
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO73
@param Address  0x0000008C
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO73       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO73       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO73       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO74 DR_GPIO74
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO74
@param Address  0x0000008C
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO74       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO74       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO74       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO75 DR_GPIO75
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO75
@param Address  0x0000008C
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO75       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO75       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO75       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO76 DR_GPIO76
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO76
@param Address  0x0000008C
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO76       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO76       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO76       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO77 DR_GPIO77
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO77
@param Address  0x0000008C
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO77       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO77       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO77       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO78 DR_GPIO78
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO78
@param Address  0x0000008C
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO78       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO78       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO78       0x00000001

/**
@defgroup       DRIVE79_64GPIO__DR_GPIO79 DR_GPIO79
@ingroup        DRIVE79_64GPIO
@brief          drive current GPIO79
@param Address  0x0000008C
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO__MSK_DR_GPIO79       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_SHFT_DR_GPIO79       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE79_64GPIO_RSTV_DR_GPIO79       0x00000001


/**
@defgroup       DRIVE95_80GPIO DRIVE95_80GPIO
@ingroup        Register
@brief          SCRB Drive Current of GPIO Signals (3.3V / 1.8V ) jedes GPIO-Bit wird gemäß folgender Codierung auf den Treiberstrom eingestellt -----3.3V---------     -----1.8V-------- 00 -  6 mA             00 - 3 mA (n.a.) 01 - 12 mA            01 - 6 mA 10 - 18 mA (n.a.)  10 - 9 mA 11 - 24mA  (n.a.)  11 - 12 mA
@param Address  0x00000090
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO              0x00000090
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RST__VAL     0x55555555

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO80 DR_GPIO80
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO80
@param Address  0x00000090
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO80       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO80       0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO80       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO81 DR_GPIO81
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO81
@param Address  0x00000090
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO81       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO81       2
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO81       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO82 DR_GPIO82
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO82
@param Address  0x00000090
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO82       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO82       4
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO82       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO83 DR_GPIO83
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO83
@param Address  0x00000090
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO83       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO83       6
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO83       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO84 DR_GPIO84
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO84
@param Address  0x00000090
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO84       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO84       8
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO84       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO85 DR_GPIO85
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO85
@param Address  0x00000090
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO85       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO85       10
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO85       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO86 DR_GPIO86
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO86
@param Address  0x00000090
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO86       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO86       12
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO86       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO87 DR_GPIO87
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO87
@param Address  0x00000090
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO87       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO87       14
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO87       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO88 DR_GPIO88
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO88
@param Address  0x00000090
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO88       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO88       16
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO88       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO89 DR_GPIO89
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO89
@param Address  0x00000090
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO89       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO89       18
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO89       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO90 DR_GPIO90
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO90
@param Address  0x00000090
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO90       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO90       20
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO90       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO91 DR_GPIO91
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO91
@param Address  0x00000090
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO91       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO91       22
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO91       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO92 DR_GPIO92
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO92
@param Address  0x00000090
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO92       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO92       24
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO92       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO93 DR_GPIO93
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO93
@param Address  0x00000090
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO93       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO93       26
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO93       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO94 DR_GPIO94
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO94
@param Address  0x00000090
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO94       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO94       28
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO94       0x00000001

/**
@defgroup       DRIVE95_80GPIO__DR_GPIO95 DR_GPIO95
@ingroup        DRIVE95_80GPIO
@brief          drive current GPIO95
@param Address  0x00000090
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000001
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO__MSK_DR_GPIO95       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_SHFT_DR_GPIO95       30
#define ERTEC200P_APBPER_SCRB_REG_DRIVE95_80GPIO_RSTV_DR_GPIO95       0x00000001


/**
@defgroup       PULL15_0GPIO PULL15_0GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
@param Address  0x00000094
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO              0x00000094
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO0 PR_GPIO0
@ingroup        PULL15_0GPIO
@brief          pull control GPIO0 
@param Address  0x00000094
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO0       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO0       0
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO0       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO1 PR_GPIO1
@ingroup        PULL15_0GPIO
@brief          pull control GPIO1 
@param Address  0x00000094
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO1       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO1       2
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO1       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO2 PR_GPIO2
@ingroup        PULL15_0GPIO
@brief          pull control GPIO2 
@param Address  0x00000094
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO2       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO2       4
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO2       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO3 PR_GPIO3
@ingroup        PULL15_0GPIO
@brief          pull control GPIO3 
@param Address  0x00000094
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO3       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO3       6
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO3       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO4 PR_GPIO4
@ingroup        PULL15_0GPIO
@brief          pull control GPIO4 
@param Address  0x00000094
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO4       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO4       8
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO4       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO5 PR_GPIO5
@ingroup        PULL15_0GPIO
@brief          pull control GPIO5 
@param Address  0x00000094
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO5       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO5       10
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO5       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO6 PR_GPIO6
@ingroup        PULL15_0GPIO
@brief          pull control GPIO6 
@param Address  0x00000094
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO6       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO6       12
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO6       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO7 PR_GPIO7
@ingroup        PULL15_0GPIO
@brief          pull control GPIO7 
@param Address  0x00000094
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO7       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO7       14
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO7       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO8 PR_GPIO8
@ingroup        PULL15_0GPIO
@brief          pull control GPIO8 
@param Address  0x00000094
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO8       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO8       16
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO8       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO9 PR_GPIO9
@ingroup        PULL15_0GPIO
@brief          pull control GPIO9 
@param Address  0x00000094
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO9       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO9       18
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO9       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO10 PR_GPIO10
@ingroup        PULL15_0GPIO
@brief          pull control GPIO10 
@param Address  0x00000094
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO10       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO10       20
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO10       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO11 PR_GPIO11
@ingroup        PULL15_0GPIO
@brief          pull control GPIO11 
@param Address  0x00000094
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO11       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO11       22
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO11       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO12 PR_GPIO12
@ingroup        PULL15_0GPIO
@brief          pull control GPIO12 
@param Address  0x00000094
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO12       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO12       24
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO12       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO13 PR_GPIO13
@ingroup        PULL15_0GPIO
@brief          pull control GPIO13 
@param Address  0x00000094
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO13       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO13       26
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO13       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO14 PR_GPIO14
@ingroup        PULL15_0GPIO
@brief          pull control GPIO14 
@param Address  0x00000094
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO14       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO14       28
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO14       0x00000000

/**
@defgroup       PULL15_0GPIO__PR_GPIO15 PR_GPIO15
@ingroup        PULL15_0GPIO
@brief          pull control GPIO15 
@param Address  0x00000094
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO__MSK_PR_GPIO15       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_SHFT_PR_GPIO15       30
#define ERTEC200P_APBPER_SCRB_REG_PULL15_0GPIO_RSTV_PR_GPIO15       0x00000000


/**
@defgroup       PULL31_16GPIO PULL31_16GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 
@param Address  0x00000098
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO              0x00000098
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO16 PR_GPIO16
@ingroup        PULL31_16GPIO
@brief          pull control GPIO16 
@param Address  0x00000098
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO16       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO16       0
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO16       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO17 PR_GPIO17
@ingroup        PULL31_16GPIO
@brief          pull control GPIO17 
@param Address  0x00000098
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO17       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO17       2
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO17       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO18 PR_GPIO18
@ingroup        PULL31_16GPIO
@brief          pull control GPIO18  
@param Address  0x00000098
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO18       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO18       4
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO18       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO19 PR_GPIO19
@ingroup        PULL31_16GPIO
@brief          pull control GPIO19  
@param Address  0x00000098
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO19       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO19       6
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO19       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO20 PR_GPIO20
@ingroup        PULL31_16GPIO
@brief          pull control GPIO20  
@param Address  0x00000098
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO20       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO20       8
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO20       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO21 PR_GPIO21
@ingroup        PULL31_16GPIO
@brief          pull control GPIO21  
@param Address  0x00000098
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO21       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO21       10
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO21       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO22 PR_GPIO22
@ingroup        PULL31_16GPIO
@brief          pull control GPIO22  
@param Address  0x00000098
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO22       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO22       12
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO22       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO23 PR_GPIO23
@ingroup        PULL31_16GPIO
@brief          pull control GPIO23  
@param Address  0x00000098
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO23       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO23       14
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO23       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO24 PR_GPIO24
@ingroup        PULL31_16GPIO
@brief          pull control GPIO24 
@param Address  0x00000098
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO24       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO24       16
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO24       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO25 PR_GPIO25
@ingroup        PULL31_16GPIO
@brief          pull control GPIO25 
@param Address  0x00000098
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO25       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO25       18
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO25       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO26 PR_GPIO26
@ingroup        PULL31_16GPIO
@brief          pull control GPIO26 
@param Address  0x00000098
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO26       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO26       20
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO26       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO27 PR_GPIO27
@ingroup        PULL31_16GPIO
@brief          pull control GPIO27 
@param Address  0x00000098
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO27       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO27       22
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO27       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO28 PR_GPIO28
@ingroup        PULL31_16GPIO
@brief          pull control GPIO28 
@param Address  0x00000098
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO28       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO28       24
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO28       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO29 PR_GPIO29
@ingroup        PULL31_16GPIO
@brief          pull control GPIO29 
@param Address  0x00000098
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO29       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO29       26
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO29       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO30 PR_GPIO30
@ingroup        PULL31_16GPIO
@brief          pull control GPIO30 
@param Address  0x00000098
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO30       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO30       28
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO30       0x00000000

/**
@defgroup       PULL31_16GPIO__PR_GPIO31 PR_GPIO31
@ingroup        PULL31_16GPIO
@brief          pull control GPIO31 
@param Address  0x00000098
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO__MSK_PR_GPIO31       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_SHFT_PR_GPIO31       30
#define ERTEC200P_APBPER_SCRB_REG_PULL31_16GPIO_RSTV_PR_GPIO31       0x00000000


/**
@defgroup       PULL47_32GPIO PULL47_32GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!
@param Address  0x0000009C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO              0x0000009C
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO32 PR_GPIO32
@ingroup        PULL47_32GPIO
@brief          pull control GPIO32 
@param Address  0x0000009C
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO32       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO32       0
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO32       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO33 PR_GPIO33
@ingroup        PULL47_32GPIO
@brief          pull control GPIO33 
@param Address  0x0000009C
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO33       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO33       2
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO33       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO34 PR_GPIO34
@ingroup        PULL47_32GPIO
@brief          pull control GPIO34 
@param Address  0x0000009C
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO34       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO34       4
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO34       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO35 PR_GPIO35
@ingroup        PULL47_32GPIO
@brief          pull control GPIO35 
@param Address  0x0000009C
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO35       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO35       6
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO35       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO36 PR_GPIO36
@ingroup        PULL47_32GPIO
@brief          pull control GPIO36 
@param Address  0x0000009C
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO36       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO36       8
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO36       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO37 PR_GPIO37
@ingroup        PULL47_32GPIO
@brief          pull control GPIO37 
@param Address  0x0000009C
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO37       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO37       10
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO37       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO38 PR_GPIO38
@ingroup        PULL47_32GPIO
@brief          pull control GPIO38 
@param Address  0x0000009C
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO38       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO38       12
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO38       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO39 PR_GPIO39
@ingroup        PULL47_32GPIO
@brief          pull control GPIO39 
@param Address  0x0000009C
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO39       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO39       14
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO39       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO40 PR_GPIO40
@ingroup        PULL47_32GPIO
@brief          pull control GPIO40 
@param Address  0x0000009C
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO40       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO40       16
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO40       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO41 PR_GPIO41
@ingroup        PULL47_32GPIO
@brief          pull control GPIO41 
@param Address  0x0000009C
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO41       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO41       18
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO41       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO42 PR_GPIO42
@ingroup        PULL47_32GPIO
@brief          pull control GPIO42 
@param Address  0x0000009C
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO42       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO42       20
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO42       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO43 PR_GPIO43
@ingroup        PULL47_32GPIO
@brief          pull control GPIO43 
@param Address  0x0000009C
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO43       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO43       22
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO43       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO44 PR_GPIO44
@ingroup        PULL47_32GPIO
@brief          pull control GPIO44 
@param Address  0x0000009C
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO44       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO44       24
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO44       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO45 PR_GPIO45
@ingroup        PULL47_32GPIO
@brief          pull control GPIO45 
@param Address  0x0000009C
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO45       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO45       26
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO45       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO46 PR_GPIO46
@ingroup        PULL47_32GPIO
@brief          pull control GPIO46 
@param Address  0x0000009C
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO46       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO46       28
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO46       0x00000000

/**
@defgroup       PULL47_32GPIO__PR_GPIO47 PR_GPIO47
@ingroup        PULL47_32GPIO
@brief          pull control GPIO47 
@param Address  0x0000009C
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO__MSK_PR_GPIO47       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_SHFT_PR_GPIO47       30
#define ERTEC200P_APBPER_SCRB_REG_PULL47_32GPIO_RSTV_PR_GPIO47       0x00000000


/**
@defgroup       PULL63_48GPIO PULL63_48GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!
@param Address  0x000000A0
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO              0x000000A0
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO48 PR_GPIO48
@ingroup        PULL63_48GPIO
@brief          pull control GPIO48 
@param Address  0x000000A0
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO48       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO48       0
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO48       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO49 PR_GPIO49
@ingroup        PULL63_48GPIO
@brief          pull control GPIO49 
@param Address  0x000000A0
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO49       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO49       2
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO49       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO50 PR_GPIO50
@ingroup        PULL63_48GPIO
@brief          pull control GPIO50 
@param Address  0x000000A0
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO50       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO50       4
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO50       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO51 PR_GPIO51
@ingroup        PULL63_48GPIO
@brief          pull control GPIO51 
@param Address  0x000000A0
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO51       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO51       6
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO51       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO52 PR_GPIO52
@ingroup        PULL63_48GPIO
@brief          pull control GPIO52 
@param Address  0x000000A0
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO52       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO52       8
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO52       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO53 PR_GPIO53
@ingroup        PULL63_48GPIO
@brief          pull control GPIO53 
@param Address  0x000000A0
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO53       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO53       10
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO53       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO54 PR_GPIO54
@ingroup        PULL63_48GPIO
@brief          pull control GPIO54 
@param Address  0x000000A0
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO54       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO54       12
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO54       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO55 PR_GPIO55
@ingroup        PULL63_48GPIO
@brief          pull control GPIO55 
@param Address  0x000000A0
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO55       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO55       14
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO55       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO56 PR_GPIO56
@ingroup        PULL63_48GPIO
@brief          pull control GPIO56 
@param Address  0x000000A0
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO56       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO56       16
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO56       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO57 PR_GPIO57
@ingroup        PULL63_48GPIO
@brief          pull control GPIO57 
@param Address  0x000000A0
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO57       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO57       18
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO57       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO58 PR_GPIO58
@ingroup        PULL63_48GPIO
@brief          pull control GPIO58 
@param Address  0x000000A0
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO58       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO58       20
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO58       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO59 PR_GPIO59
@ingroup        PULL63_48GPIO
@brief          pull control GPIO59 
@param Address  0x000000A0
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO59       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO59       22
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO59       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO60 PR_GPIO60
@ingroup        PULL63_48GPIO
@brief          pull control GPIO60 
@param Address  0x000000A0
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO60       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO60       24
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO60       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO61 PR_GPIO61
@ingroup        PULL63_48GPIO
@brief          pull control GPIO61 
@param Address  0x000000A0
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO61       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO61       26
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO61       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO62 PR_GPIO62
@ingroup        PULL63_48GPIO
@brief          pull control GPIO62 
@param Address  0x000000A0
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO62       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO62       28
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO62       0x00000000

/**
@defgroup       PULL63_48GPIO__PR_GPIO63 PR_GPIO63
@ingroup        PULL63_48GPIO
@brief          pull control GPIO63 
@param Address  0x000000A0
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO__MSK_PR_GPIO63       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_SHFT_PR_GPIO63       30
#define ERTEC200P_APBPER_SCRB_REG_PULL63_48GPIO_RSTV_PR_GPIO63       0x00000000


/**
@defgroup       PULL79_64GPIO PULL79_64GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!
@param Address  0x000000A4
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO              0x000000A4
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO64 PR_GPIO64
@ingroup        PULL79_64GPIO
@brief          pull control GPIO64 
@param Address  0x000000A4
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO64       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO64       0
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO64       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO65 PR_GPIO65
@ingroup        PULL79_64GPIO
@brief          pull control GPIO65 
@param Address  0x000000A4
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO65       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO65       2
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO65       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO66 PR_GPIO66
@ingroup        PULL79_64GPIO
@brief          pull control GPIO66 
@param Address  0x000000A4
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO66       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO66       4
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO66       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO67 PR_GPIO67
@ingroup        PULL79_64GPIO
@brief          pull control GPIO67 
@param Address  0x000000A4
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO67       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO67       6
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO67       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO68 PR_GPIO68
@ingroup        PULL79_64GPIO
@brief          pull control GPIO68 
@param Address  0x000000A4
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO68       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO68       8
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO68       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO69 PR_GPIO69
@ingroup        PULL79_64GPIO
@brief          pull control GPIO69 
@param Address  0x000000A4
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO69       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO69       10
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO69       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO70 PR_GPIO70
@ingroup        PULL79_64GPIO
@brief          pull control GPIO70 
@param Address  0x000000A4
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO70       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO70       12
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO70       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO71 PR_GPIO71
@ingroup        PULL79_64GPIO
@brief          pull control GPIO71 
@param Address  0x000000A4
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO71       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO71       14
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO71       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO72 PR_GPIO72
@ingroup        PULL79_64GPIO
@brief          pull control GPIO72 
@param Address  0x000000A4
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO72       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO72       16
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO72       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO73 PR_GPIO73
@ingroup        PULL79_64GPIO
@brief          pull control GPIO73 
@param Address  0x000000A4
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO73       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO73       18
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO73       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO74 PR_GPIO74
@ingroup        PULL79_64GPIO
@brief          pull control GPIO74 
@param Address  0x000000A4
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO74       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO74       20
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO74       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO75 PR_GPIO75
@ingroup        PULL79_64GPIO
@brief          pull control GPIO75 
@param Address  0x000000A4
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO75       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO75       22
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO75       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO76 PR_GPIO76
@ingroup        PULL79_64GPIO
@brief          pull control GPIO76 
@param Address  0x000000A4
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO76       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO76       24
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO76       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO77 PR_GPIO77
@ingroup        PULL79_64GPIO
@brief          pull control GPIO77 
@param Address  0x000000A4
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO77       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO77       26
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO77       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO78 PR_GPIO78
@ingroup        PULL79_64GPIO
@brief          pull control GPIO78 
@param Address  0x000000A4
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO78       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO78       28
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO78       0x00000000

/**
@defgroup       PULL79_64GPIO__PR_GPIO79 PR_GPIO79
@ingroup        PULL79_64GPIO
@brief          pull control GPIO79 
@param Address  0x000000A4
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO__MSK_PR_GPIO79       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_SHFT_PR_GPIO79       30
#define ERTEC200P_APBPER_SCRB_REG_PULL79_64GPIO_RSTV_PR_GPIO79       0x00000000


/**
@defgroup       PULL95_80GPIO PULL95_80GPIO
@ingroup        Register
@brief          SCRB Pull GPIO Signals jedes GPIO-Bit wird gemäß folgender Codierung auf die Pull-Widerstände eingestellt 00 - highZ   01 - Pull-up  10 - highZ  11 - Pull-down   Ist der PAD auf Ausgang geschalten, wird der Pull-Widerstand automatisch deaktiviert  Resetwert kommt nicht zum tragen, da Register in Abhängigkeit der über CONFIG eingestellten Funktion geladen wird!
@param Address  0x000000A8
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO              0x000000A8
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RST__VAL     0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO80 PR_GPIO80
@ingroup        PULL95_80GPIO
@brief          pull control GPIO80 
@param Address  0x000000A8
@param Mask     0x00000003
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO80       0x00000003
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO80       0
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO80       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO81 PR_GPIO81
@ingroup        PULL95_80GPIO
@brief          pull control GPIO81 
@param Address  0x000000A8
@param Mask     0x0000000C
@param Shift    2
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO81       0x0000000C
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO81       2
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO81       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO82 PR_GPIO82
@ingroup        PULL95_80GPIO
@brief          pull control GPIO82 
@param Address  0x000000A8
@param Mask     0x00000030
@param Shift    4
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO82       0x00000030
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO82       4
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO82       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO83 PR_GPIO83
@ingroup        PULL95_80GPIO
@brief          pull control GPIO83 
@param Address  0x000000A8
@param Mask     0x000000C0
@param Shift    6
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO83       0x000000C0
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO83       6
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO83       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO84 PR_GPIO84
@ingroup        PULL95_80GPIO
@brief          pull control GPIO84 
@param Address  0x000000A8
@param Mask     0x00000300
@param Shift    8
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO84       0x00000300
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO84       8
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO84       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO85 PR_GPIO85
@ingroup        PULL95_80GPIO
@brief          pull control GPIO85 
@param Address  0x000000A8
@param Mask     0x00000C00
@param Shift    10
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO85       0x00000C00
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO85       10
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO85       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO86 PR_GPIO86
@ingroup        PULL95_80GPIO
@brief          pull control GPIO86 
@param Address  0x000000A8
@param Mask     0x00003000
@param Shift    12
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO86       0x00003000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO86       12
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO86       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO87 PR_GPIO87
@ingroup        PULL95_80GPIO
@brief          pull control GPIO87 
@param Address  0x000000A8
@param Mask     0x0000C000
@param Shift    14
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO87       0x0000C000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO87       14
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO87       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO88 PR_GPIO88
@ingroup        PULL95_80GPIO
@brief          pull control GPIO88 
@param Address  0x000000A8
@param Mask     0x00030000
@param Shift    16
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO88       0x00030000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO88       16
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO88       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO89 PR_GPIO89
@ingroup        PULL95_80GPIO
@brief          pull control GPIO89 
@param Address  0x000000A8
@param Mask     0x000C0000
@param Shift    18
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO89       0x000C0000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO89       18
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO89       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO90 PR_GPIO90
@ingroup        PULL95_80GPIO
@brief          pull control GPIO90 
@param Address  0x000000A8
@param Mask     0x00300000
@param Shift    20
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO90       0x00300000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO90       20
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO90       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO91 PR_GPIO91
@ingroup        PULL95_80GPIO
@brief          pull control GPIO91 
@param Address  0x000000A8
@param Mask     0x00C00000
@param Shift    22
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO91       0x00C00000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO91       22
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO91       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO92 PR_GPIO92
@ingroup        PULL95_80GPIO
@brief          pull control GPIO92 
@param Address  0x000000A8
@param Mask     0x03000000
@param Shift    24
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO92       0x03000000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO92       24
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO92       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO93 PR_GPIO93
@ingroup        PULL95_80GPIO
@brief          pull control GPIO93 
@param Address  0x000000A8
@param Mask     0x0C000000
@param Shift    26
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO93       0x0C000000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO93       26
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO93       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO94 PR_GPIO94
@ingroup        PULL95_80GPIO
@brief          pull control GPIO94 
@param Address  0x000000A8
@param Mask     0x30000000
@param Shift    28
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO94       0x30000000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO94       28
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO94       0x00000000

/**
@defgroup       PULL95_80GPIO__PR_GPIO95 PR_GPIO95
@ingroup        PULL95_80GPIO
@brief          pull control GPIO95 
@param Address  0x000000A8
@param Mask     0xC0000000
@param Shift    30
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO__MSK_PR_GPIO95       0xC0000000
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_SHFT_PR_GPIO95       30
#define ERTEC200P_APBPER_SCRB_REG_PULL95_80GPIO_RSTV_PR_GPIO95       0x00000000


/**
@defgroup       CLEAR_DMA_SPI_REQ_REG CLEAR_DMA_SPI_REQ_REG
@ingroup        Register
@brief          Reset SPI1/2_SSPTX_Delayed_ Request (SPI1/2 Pause-Betrieb)
@param Address  0x000000AC
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG              0x000000AC
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG_RST__VAL     0x00000000

/**
@defgroup       CLEAR_DMA_SPI_REQ_REG__CLEAR_SPI1_SSPTX_DELAYED_REQUEST CLEAR_SPI1_SSPTX_DELAYED_REQUEST
@ingroup        CLEAR_DMA_SPI_REQ_REG
@brief          1: zurücksetzen von SPI1_SSPTX_Delayed_ Request in der GDMA-Shell, nicht speichernd
@param Address  0x000000AC
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG__MSK_CLEAR_SPI1_SSPTX_DELAYED_REQUEST       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG_SHFT_CLEAR_SPI1_SSPTX_DELAYED_REQUEST       0
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG_RSTV_CLEAR_SPI1_SSPTX_DELAYED_REQUEST       0x00000000

/**
@defgroup       CLEAR_DMA_SPI_REQ_REG__CLEAR_SPI2_SSPTX_DELAYED_REQUEST CLEAR_SPI2_SSPTX_DELAYED_REQUEST
@ingroup        CLEAR_DMA_SPI_REQ_REG
@brief          1: zurücksetzen von SPI2_SSPTX_Delayed_ Request in der GDMA-Shell, nicht speichernd
@param Address  0x000000AC
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG__MSK_CLEAR_SPI2_SSPTX_DELAYED_REQUEST       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG_SHFT_CLEAR_SPI2_SSPTX_DELAYED_REQUEST       1
#define ERTEC200P_APBPER_SCRB_REG_CLEAR_DMA_SPI_REQ_REG_RSTV_CLEAR_SPI2_SSPTX_DELAYED_REQUEST       0x00000000


/**
@defgroup       SPI_PARITY_ERROR SPI_PARITY_ERROR
@ingroup        Register
@brief          SPI1/2 Parity Error (ein Parity-Check erfolgt nur beim Lesen von SPI-Daten im Adressbereich mit APB-Adressbits(10dt9) = 01) 
@param Address  0x000000B0
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR              0x000000B0
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR_RST__VAL     0x00000000

/**
@defgroup       SPI_PARITY_ERROR__SPI1_PARITY_ERR SPI1_PARITY_ERR
@ingroup        SPI_PARITY_ERROR
@brief          1: Parity-Error SPI1 (Even Parity)
@param Address  0x000000B0
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR__MSK_SPI1_PARITY_ERR       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR_SHFT_SPI1_PARITY_ERR       0
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR_RSTV_SPI1_PARITY_ERR       0x00000000

/**
@defgroup       SPI_PARITY_ERROR__SPI2_PARITY_ERR SPI2_PARITY_ERR
@ingroup        SPI_PARITY_ERROR
@brief          1: Parity-Error SPI2 (Even Parity)
@param Address  0x000000B0
@param Mask     0x00000002
@param Shift    1
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR__MSK_SPI2_PARITY_ERR       0x00000002
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR_SHFT_SPI2_PARITY_ERR       1
#define ERTEC200P_APBPER_SCRB_REG_SPI_PARITY_ERROR_RSTV_SPI2_PARITY_ERR       0x00000000


/**
@defgroup       SSI_DATA_INVERSION SSI_DATA_INVERSION
@ingroup        Register
@brief          workaorund for SSI deadlock: in case of SSI-FSM deadlock set this register to 1hex and back to 0hex. This will force SSI module to normal operation mode.
@param Address  0x000000B4
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SSI_DATA_INVERSION              0x000000B4
#define ERTEC200P_APBPER_SCRB_REG_SSI_DATA_INVERSION_RST__VAL     0x00000000

/**
@defgroup       SSI_DATA_INVERSION__INV_SSI_DATA INV_SSI_DATA
@ingroup        SSI_DATA_INVERSION
@brief          1: invert SSI_DATA, (and XSSI_DATA) 0: no inversion
@param Address  0x000000B4
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_SSI_DATA_INVERSION__MSK_INV_SSI_DATA       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_SSI_DATA_INVERSION_SHFT_INV_SSI_DATA       0
#define ERTEC200P_APBPER_SCRB_REG_SSI_DATA_INVERSION_RSTV_INV_SSI_DATA       0x00000000


/**
@defgroup       XHIF_MODE XHIF_MODE
@ingroup        Register
@brief          Umschalten XHIF-Pin zwischen XHIF_A20 und XHIF_XCS_R
@param Address  0x000000B8
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_XHIF_MODE              0x000000B8
#define ERTEC200P_APBPER_SCRB_REG_XHIF_MODE_RST__VAL     0x00000000

/**
@defgroup       XHIF_MODE__XHIF_MODE XHIF_MODE
@ingroup        XHIF_MODE
@brief          Der Eingangspin XHIF_XCS_R_A20 wird als  0: Page-Register Chipselect (XHIF_XCS_R) 1: Adressleitung (XHIF_A20) verwendet. (Der nicht ausgewaehlte XHIF-Eingang ist dabei inaktiv  beschaltet, dh. XHIF_A20='0', XHIF_XCS_R='1')
@param Address  0x000000B8
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_REG_XHIF_MODE__MSK_XHIF_MODE       0x00000001
#define ERTEC200P_APBPER_SCRB_REG_XHIF_MODE_SHFT_XHIF_MODE       0
#define ERTEC200P_APBPER_SCRB_REG_XHIF_MODE_RSTV_XHIF_MODE       0x00000000


/**
@defgroup       EXT_DRIVER_EN EXT_DRIVER_EN
@ingroup        Register
@brief          
@param Address  0x000000BC
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN              0x000000BC
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_RST__VAL     0x00000000

/**
@defgroup       EXT_DRIVER_EN__CS0_ENABLE CS0_ENABLE
@ingroup        EXT_DRIVER_EN
@brief          1: CS0 aktiviert XOE_DRIVER 0: CS0 hat keinen Einfluss auf XOE_DRIVER. Nach wegnehmen des Resets wird der Wert von XRDY_BF eingelatcht.
@param Address  0x000000BC
@param Mask     0x00000001
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN__MSK_CS0_ENABLE       0x00000001
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_SHFT_CS0_ENABLE       0
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_RSTV_CS0_ENABLE       0x00000000

/**
@defgroup       EXT_DRIVER_EN__CS1_ENABLE CS1_ENABLE
@ingroup        EXT_DRIVER_EN
@brief          1: CS1 aktiviert XOE_DRIVER 0: CS1 hat keinen Einfluss auf XOE_DRIVER.
@param Address  0x000000BC
@param Mask     0x00000002
@param Shift    1
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN__MSK_CS1_ENABLE       0x00000002
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_SHFT_CS1_ENABLE       1
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_RSTV_CS1_ENABLE       0x00000000

/**
@defgroup       EXT_DRIVER_EN__CS2_ENABLE CS2_ENABLE
@ingroup        EXT_DRIVER_EN
@brief          1: CS2 aktiviert XOE_DRIVER 0: CS2 hat keinen Einfluss auf XOE_DRIVER.
@param Address  0x000000BC
@param Mask     0x00000004
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN__MSK_CS2_ENABLE       0x00000004
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_SHFT_CS2_ENABLE       2
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_RSTV_CS2_ENABLE       0x00000000

/**
@defgroup       EXT_DRIVER_EN__CS3_ENABLE CS3_ENABLE
@ingroup        EXT_DRIVER_EN
@brief          1: CS3 aktiviert XOE_DRIVER 0: CS3 hat keinen Einfluss auf XOE_DRIVER.
@param Address  0x000000BC
@param Mask     0x00000008
@param Shift    3
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN__MSK_CS3_ENABLE       0x00000008
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_SHFT_CS3_ENABLE       3
#define ERTEC200P_APBPER_SCRB_R2_REG_EXT_DRIVER_EN_RSTV_CS3_ENABLE       0x00000000


/**
@defgroup       SPI_MODE SPI_MODE
@ingroup        Register
@brief          Umschalten der SPI-Ausgaenge zwischen direct-Mode und MISO_MOSI-Mode
@param Address  0x000000C0
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_SPI_MODE              0x000000C0
#define ERTEC200P_APBPER_SCRB_R2_REG_SPI_MODE_RST__VAL     0x00000000

/**
@defgroup       SPI_MODE__SPI_MODE SPI_MODE
@ingroup        SPI_MODE
@brief          0: direct-Mode 1: MISO_MOSI-Mode
@param Address  0x000000C0
@param Mask     0x00000001
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  2
@param Type     -1
*/
#define ERTEC200P_APBPER_SCRB_R2_REG_SPI_MODE__MSK_SPI_MODE       0x00000001
#define ERTEC200P_APBPER_SCRB_R2_REG_SPI_MODE_SHFT_SPI_MODE       0
#define ERTEC200P_APBPER_SCRB_R2_REG_SPI_MODE_RSTV_SPI_MODE       0x00000000

#ifdef __cplusplus
}
#endif

#endif

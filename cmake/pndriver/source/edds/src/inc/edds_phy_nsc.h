#ifndef EDDS_PHY_NSC_H                   /* ----- reinclude-protection ----- */
#define EDDS_PHY_NSC_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for PN-IP)     :C&  */
/*                                                                           */
/*  F i l e               &F: EDDS_phy_nsc.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.02.15    TH    initial version.                                       */
#endif
/*****************************************************************************/

/*=============================== Defines ===================================*/	

/*---------------------------------------------------------------------------*/
/* PHY specific parameters      											 */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_OUI1REGADDRESS		      0x0002    // PHY Identifier Register 1
#define EDDS_PHY_NSC_OUI1COMPAREVALUE         0x0020//0x2000    // OUI
#define EDDS_PHY_NSC_OUI2REGADDRESS		      0x0003    // PHY Identifier Register 2
#define EDDS_PHY_NSC_OUI2COMPAREVALUE         0x60C1//0x5CA2    // OUI
#define EDDS_PHY_NSC_LINKSTATUSREGADDRESS	  0x0001    // BasicModeStatus Register
#define EDDS_PHY_NSC_LINKSTATUSBITMASK        0x0004    // Link Status (0:no Link, 1:valid Link)
#define EDDS_PHY_NSC_LINKVALIDBITMASK         0x0020    // AutoNeg Complete (0:not, 1:complete)
#define EDDS_PHY_NSC_SPEEDREGADDRESS	      0x0010    // PHY Status Register
#define EDDS_PHY_NSC_SPEEDBITMASK             0x0002    // Speed Status (0:100MBit, 1:10MBit)
#define EDDS_PHY_NSC_DUPLEXITYREGADDRESS	  0x0010    // PHY Status Register
#define EDDS_PHY_NSC_DUPLEXITYBITMASK         0x0004    // Duplex Status (0:half duplex, 1:full duplex)

/*---------------------------------------------------------------------------*/
/* PHY register number                                                       */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_REG_BCMR                      0    // BasicModeControlRegister --> controls PowerDown and AutoNegotiation
#define EDDS_PHY_NSC_REG_NEG_ADVERTISEMENT         4    // Auto-Negotiation Advertisement Register --> AutoNeg Cap.
#define EDDS_PHY_NSC_REG_PCS_CFG                  22    // PCS Config and Status Register --> special signal detection
#define EDDS_PHY_NSC_REG_LED_DIRECT_CONTROL       24    // LED DirectControl Register --> Led control
#define EDDS_PHY_NSC_REG_CONTROL                  25    // PHY Control Register --> MDI-X

/*---------------------------------------------------------------------------*/
/* SHIM-Versatz                                                              */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_TX_SHIM_ERTEC200P      11  // = (10 + 12) / 2
#define EDDS_PHY_NSC_RX_SHIM_ERTEC200P      11  // = (10 + 12) / 2
#define EDDS_PHY_NSC_TX_SHIM_FPGA           22  // = (20 + 24) / 2
#define EDDS_PHY_NSC_RX_SHIM_FPGA           22  // = (20 + 24) / 2

/*---------------------------------------------------------------------------*/
/* PHY delay values         											     */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_PHY_TX_DELAY           50
#define EDDS_PHY_NSC_PHY_RX_DELAY           264

/*---------------------------------------------------------------------------*/
/* real Port delay values         											 */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_REAL_PORT_TX_DELAY_ERTEC200P   (EDDS_PHY_NSC_PHY_TX_DELAY + EDDS_SWI_PTCP_TX_DELAY_ERTEC200P - EDDS_PHY_NSC_TX_SHIM_ERTEC200P) // 39
#define EDDS_PHY_NSC_REAL_PORT_RX_DELAY_ERTEC200P   (EDDS_PHY_NSC_PHY_RX_DELAY + EDDS_SWI_PTCP_RX_DELAY_ERTEC200P + EDDS_PHY_NSC_RX_SHIM_ERTEC200P) // 275
#define EDDS_PHY_NSC_REAL_PORT_TX_DELAY_FPGA        (EDDS_PHY_NSC_PHY_TX_DELAY + EDDS_SWI_PTCP_TX_DELAY_FPGA      - EDDS_PHY_NSC_TX_SHIM_FPGA)      // 28
#define EDDS_PHY_NSC_REAL_PORT_RX_DELAY_FPGA        (EDDS_PHY_NSC_PHY_RX_DELAY + EDDS_SWI_PTCP_RX_DELAY_FPGA      + EDDS_PHY_NSC_RX_SHIM_FPGA)      // 286
#define EDDS_PHY_NSC_REAL_PORT_TX_DELAY_FPGA_FX     28
#define EDDS_PHY_NSC_REAL_PORT_RX_DELAY_FPGA_FX     182

/*---------------------------------------------------------------------------*/
/* Max Port delay values         											 */
/*---------------------------------------------------------------------------*/
#define EDDS_PHY_NSC_MAX_PORT_TX_DELAY_ERTEC200P    40
#define EDDS_PHY_NSC_MAX_PORT_RX_DELAY_ERTEC200P    276
#define EDDS_PHY_NSC_MAX_PORT_TX_DELAY_FPGA         30
#define EDDS_PHY_NSC_MAX_PORT_RX_DELAY_FPGA         288
#define EDDS_PHY_NSC_MAX_PORT_TX_DELAY_FPGA_FX      30
#define EDDS_PHY_NSC_MAX_PORT_RX_DELAY_FPGA_FX      184


/*============================== Prototypes =================================*/

/*=============================================================================
 * function name:  EDDS_PHY_NSC_Init
 *
 * function:       This function shall return all parameters of a specific PHY
 *                 needed for link change information. This includes PHY 
 *                 register addresses and bit masks for link status, speed mode
 *                 and duplexity.  
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_Init(LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 HwPortID);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_LED_BlinkBegin
 *
 * function:       EDDS calls this function within the service 
 *				   EDD_SRV_LED_BLINK() before the LED(s) start to blink.
 *				   Can be used e.g. to diable the link/activity LED function 
 *			       of a PHY if necessary.  
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_LED_BlinkBegin(
    EDDS_HANDLE         hDDB,
	EDDS_SYS_HANDLE     hSysDev,
	LSA_UINT32          HwPortID);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_LED_BlinkSetMode
 *
 * function:       EDDS calls this function repeatedly within the service 
 *				   EDD_SRV_LED_BLINK() in order to turn on and turn off 
 *				   the LED(s) alternately.
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_LED_BlinkSetMode(
	EDDS_HANDLE 	    hDDB,
	EDDS_SYS_HANDLE     hSysDev,	
	LSA_UINT32          HwPortID,
	LSA_UINT32		    LEDMode);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_LED_BlinkEnd
 *
 * function:       EDDS calls this function within the service
 *				   EDD_SRV_LED_BLINK() after blinking of LED(s) has finished. 
 *				   Can be used e.g. to re-enable the link/activity 
 *				   LED function of a PHY if necessary.
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_LED_BlinkEnd(
	EDDS_HANDLE 	    hDDB,
	EDDS_SYS_HANDLE     hSysDev,
	LSA_UINT32          HwPortID);


/*=============================================================================
 * function name:  EDDS_PHY_NSC_GetLinkStatus
 *
 * function:       Examine passed parameters in order to determine 
 *				   link, speed and duplexity and returns MAUType, MediaType, 
 *                 AutonegCapAdvertised, TxDelay, RxDelay, IsWireless and 
 *                 IsMDIX.
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_GetLinkStatus(
	EDDS_HANDLE				        hDDB,
	EDDS_SYS_HANDLE			        hSysDev,
	LSA_UINT32						Hardware,
    LSA_UINT32				        HwPortID,
    EDDS_PHY_LINK_STATUS_PTR_TYPE   pLinkStatus);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_SetPowerDown
 *
 * function:       This function turns the PHY into software power down mode 
 *                 ON or OFF. When software power mode is OFF there will be 
 *                 no link and the PHY does not receive or send any data but 
 *                 can still be controlled via SMI.  				   
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_SetPowerDown(LSA_VOID_PTR_TYPE pLLManagement, LSA_UINT32 HwPortID);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_SetSpeedDuplexityOrAutoneg
 *
 * function:       This function sets speed mode and duplexity when the 
 *                 'Autoneg' parameter is set to LSA_FALSE. If 'Autoneg' is 
 *                 set to LSA_TRUE “Speed” and “Duplexity” parameters are 
 *                 irrelevant. In case the transceiver does not support 
 *                 auto-negotiation it has to set up a connection at least at 
 *                 100MBit full duplex.        
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_SetSpeedDuplexityOrAutoneg(
    EDDS_HANDLE         hDDB,
    EDDS_SYS_HANDLE     hSysDev,
    LSA_UINT32          HwPortID,
    LSA_UINT8           *pLinkSpeedModeConfigured,
    LSA_UINT32          Speed,
    LSA_UINT32          Duplexity,
    LSA_BOOL            Autoneg);

/*=============================================================================
 * function name:  EDDS_PHY_NSC_CheckMAUType
 *
 * function:       This function checks if the given MAUType is supported by 
 *                 the hardware and returns the related Speed and Duplexity.
 *===========================================================================*/
EDD_RSP EDDS_SYSTEM_OUT_FCT_ATTR EDDS_PHY_NSC_CheckMAUType(
    EDDS_HANDLE         hDDB,
    EDDS_SYS_HANDLE     hSysDev,
    LSA_UINT32          HwPortID,
    LSA_UINT16          MAUType,
    LSA_UINT32          *pSpeed,
    LSA_UINT32          *pDuplexity);

/*****************************************************************************/
/*  end of file EDDS_PHY_NSC.H                                               */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of EDDS_PHY_NSC_H */

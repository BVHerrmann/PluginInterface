#ifndef EDDI_SYS_SOC_HW_H       //reinclude-protection
#define EDDI_SYS_SOC_HW_H

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
/*  F i l e               &F: eddi_sys_soc_hw.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/* prototypes for SOC-specific functions                                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  04.04.08    UL    created                                                */
/*                                                                           */
/*****************************************************************************/ 

/*=============================================================================*/
/*        GPIOs                                                                */
/*=============================================================================*/
//SOC1-PCI-Board: possible XPLL_OUT-GPIOs, located in X2304: (view from top)
//GPIO156 ISO_OUT4                         GND 20 19 GND
//GPIO157 ISO_OUT5                             18 17
//GPIO094 ISO_IN0                     ISO_IN0  16 15
//                                    ISO_OUT5 14 13
//                                    ISO_OUT4 12 11
//                                             10 09
//                                             08 07
//                                             06 05
//                                             04 03
//                                             02 01 GND
//ATTENTION: NO RS485 !!!!
#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
#define SYS_SOC_SOC1_PLLGPIO_OUT  156 //ISO_OUT4a   X2304_12  XIntClkCy
#define SYS_SOC_SOC1_ZAINT_OUT    157 //ISO_OUT5a   X2304_14  XInt_ClkCyInt
#define SYS_SOC_SOC1_ZMINT_OUT    158 //ISO_OUT6a   X2304_3   XIntComp1
#define SYS_SOC_SOC1_ZEINT_OUT    169 //ISO_OUT1b   X1500_B7  XIntComp2
#define SYS_SOC_SOC1_ZSINT_OUT    170 //ISO_OUT2b   X1500_B5  XIntComp3
#define SYS_SOC_SOC1_ZITINT_OUT   171 //ISO_OUT3b   X1500_B9  XIntComp4
#else
#define SYS_SOC_SOC1_PLLGPIO_OUT  156 //ISO_OUT4a
#endif

#define SYS_SOC_SOC1_PLLGPIO_IN   94  //ISO_IN0

//SOC2-PCI-Board: possible XPLL_OUT-GPIOs, located in X1804: (view from top)
//GPIO46 ISO_OUT0                         GND 20 19 GND
//GPIO47 ISO_OUT1                             18 17
//GPIO48 ISO_OUT2                         GND 16 15 GND
//GPIO49 ISO_OUT3                             14 13 IRTE_PLL_EXT_IN
//GPIO50 ISO_OUT4                             12 11 GND
//GPIO51 ISO_OUT5                         GND 10 09 ISO_OUT7
//GPIO52 ISO_OUT6                    ISO_OUT6 08 07 ISO_OUT5
//GPIO53 ISO_OUT7                    ISO_OUT4 06 05 ISO_OUT3
//GPIO55 IRTE_PLL_EXT_IN             ISO_OUT2 04 03 ISO_OUT1
//                                   ISO_OUT0 02 01 GND
//ATTENTION: NO RS485 !!!!
#define SYS_SOC_SOC2_PLLGPIO_OUT  46
#define SYS_SOC_SOC2_PLLGPIO_IN   55

extern LSA_RESULT EDDI_CpSocSetPllPort( EDDI_SYS_HANDLE hSysDev,
                                        LSA_UINT32      pIRTE,
                                        LSA_UINT32      location,
                                        LSA_UINT16      pllmode );

extern LSA_VOID EDDI_SysSetSendClockChange( LSA_UINT32 const scf );

extern LSA_VOID EDDI_SysSetSocBasePtr( LSA_UINT32 pSWIReg, LSA_UINT32 pSOC_IOCC, LSA_UINT32 pSOC_GPIO, LSA_UINT32 pSOC_TIM );

/*=============================================================================*/
/*        TIMER                                                                */
/*=============================================================================*/
#define SYS_TIMER_CMD_START 1
#define SYS_TIMER_CMD_STOP  2

extern LSA_RESULT EDDI_CpSocTimerInit( LSA_VOID  * pSOC_TIM,
                                       LSA_UINT8   TimNr,
                                       LSA_BOOL    PollMode );

extern LSA_RESULT EDDI_CpSocTimerCmd( LSA_UINT8   Command,
                                      EDDI_HANDLE hDDB,
                                      LSA_UINT32  TimeOut_ns );

extern LSA_RESULT EDDI_CpSocTimerCheck( LSA_BOOL * pbTimeoutOccured );

/*=============================================================================*/
/*        I2C                                                                  */
/*=============================================================================*/
#define SYS_SOC_SOC1_I2C_GPIO_SCL   184
#define SYS_SOC_SOC1_I2C_GPIO_SDA   185

extern LSA_VOID EDDI_SysSocSetBasePtr( LSA_UINT32  const pSOC_IRTE,
                                       LSA_UINT32  const pSOC_I2C,
                                       LSA_UINT32  const pSOC_SCRB,
                                       LSA_UINT32  const pSOC_GPIO);

extern LSA_RESULT EDDI_SysSocI2cReadOffset( EDDI_SYS_HANDLE			           const hSysDev,
	                                        LSA_UINT8			               const I2CDevAddr,
	                                        LSA_UINT8			               const I2COffsetCnt,
	                                        LSA_UINT8			               const I2COffset1,
	                                        LSA_UINT8			               const I2COffset2,
	                                        LSA_UINT32                         const Size,
	                                        LSA_UINT8       EDD_UPPER_MEM_ATTR     * pBuf );

extern LSA_RESULT EDDI_SysSocI2cWriteOffset( EDDI_SYS_HANDLE			        const hSysDev,
	                                         LSA_UINT8			                const I2CDevAddr,
	                                         LSA_UINT8			                const I2COffsetCnt,
	                                         LSA_UINT8			                const I2COffset1,
	                                         LSA_UINT8			                const I2COffset2,
	                                         LSA_UINT32                         const Size,
	                                         LSA_UINT8       EDD_UPPER_MEM_ATTR     * pBuf );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_SOC_HW_H


/*****************************************************************************/
/*  end of file eddi_sys_soc_hw.h                                            */
/*****************************************************************************/

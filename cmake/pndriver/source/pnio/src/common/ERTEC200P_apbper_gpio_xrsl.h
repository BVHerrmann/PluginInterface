#ifndef ERTEC200P_APBPER_GPIO_XRSL_H
#define ERTEC200P_APBPER_GPIO_XRSL_H

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
/*  F i l e               &F: ERTEC200P_apbper_gpio_xrsl.h				:F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*  P a r a m e t e r s     : Version = -1, Ports = , Interface =            */
/*****************************************************************************/


/**
@defgroup       GPIO_IOCTRL_0 GPIO_IOCTRL_0
@ingroup        Register
@brief          Configuration register for General Purpose IOs (31:0) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000000
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_0              0x00000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_0_RST__VAL     0xFFFFFFFF

/**
@defgroup       GPIO_IOCTRL_0__GPIO_IOCTRL_0 GPIO_IOCTRL_0
@ingroup        GPIO_IOCTRL_0
@brief          Configuration register for General Purpose IOs (31:0) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000000
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_0__MSK_GPIO_IOCTRL_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_0_SHFT_GPIO_IOCTRL_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_0_RSTV_GPIO_IOCTRL_0       0xFFFFFFFF


/**
@defgroup       GPIO_OUT_0 GPIO_OUT_0
@ingroup        Register
@brief          Output register for General Purpose IOs (31:0) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000004
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_0              0x00000004
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_0__GPIO_OUT_0 GPIO_OUT_0
@ingroup        GPIO_OUT_0
@brief          Output register for General Purpose IOs (31:0) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000004
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_0__MSK_GPIO_OUT_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_0_SHFT_GPIO_OUT_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_0_RSTV_GPIO_OUT_0       0x00000000


/**
@defgroup       GPIO_OUT_SET_0 GPIO_OUT_SET_0
@ingroup        Register
@brief          Bit-selective setting of the output register for General Purpose IOs (31:0) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_0              0x00000008
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_SET_0__GPIO_OUT_SET_0 GPIO_OUT_SET_0
@ingroup        GPIO_OUT_SET_0
@brief          Bit-selective setting of the output register for General Purpose IOs (31:0) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000008
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_0__MSK_GPIO_OUT_SET_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_0_SHFT_GPIO_OUT_SET_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_0_RSTV_GPIO_OUT_SET_0       0x00000000


/**
@defgroup       GPIO_OUT_CLEAR_0 GPIO_OUT_CLEAR_0
@ingroup        Register
@brief          Bit-selective reset of the output register for General Purpose IOs (31:0) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_0              0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_CLEAR_0__GPIO_OUT_CLEAR_0 GPIO_OUT_CLEAR_0
@ingroup        GPIO_OUT_CLEAR_0
@brief          Bit-selective reset of the output register for General Purpose IOs (31:0) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000000C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_0__MSK_GPIO_OUT_CLEAR_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_0_SHFT_GPIO_OUT_CLEAR_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_0_RSTV_GPIO_OUT_CLEAR_0       0x00000000


/**
@defgroup       GPIO_RES_DIS_0 GPIO_RES_DIS_0
@ingroup        Register
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_0              0x00000010
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_RES_DIS_0__GPIO_RES_DIS_0 GPIO_RES_DIS_0
@ingroup        GPIO_RES_DIS_0
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000010
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_0__MSK_GPIO_RES_DIS_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_0_SHFT_GPIO_RES_DIS_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_0_RSTV_GPIO_RES_DIS_0       0x00000000


/**
@defgroup       GPIO_IN_0 GPIO_IN_0
@ingroup        Register
@brief          Input register for General Purpose IOs (31:0)
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_0              0x00000014
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_0_RST__VAL     0x00000000

/**
@defgroup       GPIO_IN_0__GPIO_IN_0 GPIO_IN_0
@ingroup        GPIO_IN_0
@brief          Input register for General Purpose IOs (31:0)
@param Address  0x00000014
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_0__MSK_GPIO_IN_0       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_0_SHFT_GPIO_IN_0       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_0_RSTV_GPIO_IN_0       0x00000000


/**
@defgroup       GPIO_PORT_MODE_0_L GPIO_PORT_MODE_0_L
@ingroup        Register
@brief          Configuration register for GPIO port (15:0) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x00000018
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L              0x00000018
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_0_MODE_0_L GPIO_0_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(0)
@param Address  0x00000018
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_0_MODE_0_L       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_0_MODE_0_L       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_0_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_1_MODE_0_L GPIO_1_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(1)
@param Address  0x00000018
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_1_MODE_0_L       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_1_MODE_0_L       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_1_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_2_MODE_0_L GPIO_2_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(2)
@param Address  0x00000018
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_2_MODE_0_L       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_2_MODE_0_L       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_2_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_3_MODE_0_L GPIO_3_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(3)
@param Address  0x00000018
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_3_MODE_0_L       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_3_MODE_0_L       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_3_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_4_MODE_0_L GPIO_4_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(6)
@param Address  0x00000018
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_4_MODE_0_L       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_4_MODE_0_L       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_4_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_5_MODE_0_L GPIO_5_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(5)
@param Address  0x00000018
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_5_MODE_0_L       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_5_MODE_0_L       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_5_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_6_MODE_0_L GPIO_6_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(6)
@param Address  0x00000018
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_6_MODE_0_L       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_6_MODE_0_L       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_6_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_7_MODE_0_L GPIO_7_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(7)
@param Address  0x00000018
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_7_MODE_0_L       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_7_MODE_0_L       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_7_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_8_MODE_0_L GPIO_8_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(8)
@param Address  0x00000018
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_8_MODE_0_L       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_8_MODE_0_L       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_8_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_9_MODE_0_L GPIO_9_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(9)
@param Address  0x00000018
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_9_MODE_0_L       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_9_MODE_0_L       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_9_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_10_MODE_0_L GPIO_10_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(10)
@param Address  0x00000018
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_10_MODE_0_L       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_10_MODE_0_L       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_10_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_11_MODE_0_L GPIO_11_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(11)
@param Address  0x00000018
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_11_MODE_0_L       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_11_MODE_0_L       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_11_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_12_MODE_0_L GPIO_12_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(12)
@param Address  0x00000018
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_12_MODE_0_L       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_12_MODE_0_L       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_12_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_13_MODE_0_L GPIO_13_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(13)
@param Address  0x00000018
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_13_MODE_0_L       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_13_MODE_0_L       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_13_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_14_MODE_0_L GPIO_14_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(14)
@param Address  0x00000018
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_14_MODE_0_L       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_14_MODE_0_L       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_14_MODE_0_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_L__GPIO_15_MODE_0_L GPIO_15_MODE_0_L
@ingroup        GPIO_PORT_MODE_0_L
@brief          Port GPIO(15)
@param Address  0x00000018
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L__MSK_GPIO_15_MODE_0_L       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_SHFT_GPIO_15_MODE_0_L       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_L_RSTV_GPIO_15_MODE_0_L       0x00000000


/**
@defgroup       GPIO_PORT_MODE_0_H GPIO_PORT_MODE_0_H
@ingroup        Register
@brief          Configuration register for GPIO port (31:16) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x0000001C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H              0x0000001C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_16_MODE_0_H GPIO_16_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(16)
@param Address  0x0000001C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_16_MODE_0_H       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_16_MODE_0_H       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_16_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_17_MODE_0_H GPIO_17_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(17)
@param Address  0x0000001C
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_17_MODE_0_H       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_17_MODE_0_H       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_17_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_18_MODE_0_H GPIO_18_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(18)
@param Address  0x0000001C
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_18_MODE_0_H       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_18_MODE_0_H       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_18_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_19_MODE_0_H GPIO_19_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(19)
@param Address  0x0000001C
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_19_MODE_0_H       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_19_MODE_0_H       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_19_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_20_MODE_0_H GPIO_20_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(20)
@param Address  0x0000001C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_20_MODE_0_H       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_20_MODE_0_H       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_20_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_21_MODE_0_H GPIO_21_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(21)
@param Address  0x0000001C
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_21_MODE_0_H       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_21_MODE_0_H       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_21_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_22_MODE_0_H GPIO_22_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(22)
@param Address  0x0000001C
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_22_MODE_0_H       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_22_MODE_0_H       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_22_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_23_MODE_0_H GPIO_23_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(23)
@param Address  0x0000001C
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_23_MODE_0_H       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_23_MODE_0_H       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_23_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_24_MODE_0_H GPIO_24_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(24)
@param Address  0x0000001C
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_24_MODE_0_H       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_24_MODE_0_H       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_24_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_25_MODE_0_H GPIO_25_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(25)
@param Address  0x0000001C
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_25_MODE_0_H       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_25_MODE_0_H       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_25_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_26_MODE_0_H GPIO_26_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(26)
@param Address  0x0000001C
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_26_MODE_0_H       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_26_MODE_0_H       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_26_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_27_MODE_0_H GPIO_27_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(27)
@param Address  0x0000001C
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_27_MODE_0_H       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_27_MODE_0_H       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_27_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_28_MODE_0_H GPIO_28_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(28)
@param Address  0x0000001C
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_28_MODE_0_H       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_28_MODE_0_H       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_28_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_29_MODE_0_H GPIO_29_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(29)
@param Address  0x0000001C
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_29_MODE_0_H       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_29_MODE_0_H       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_29_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_30_MODE_0_H GPIO_30_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(30)
@param Address  0x0000001C
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_30_MODE_0_H       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_30_MODE_0_H       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_30_MODE_0_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_0_H__GPIO_31_MODE_0_H GPIO_31_MODE_0_H
@ingroup        GPIO_PORT_MODE_0_H
@brief          Port GPIO(31)
@param Address  0x0000001C
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H__MSK_GPIO_31_MODE_0_H       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_SHFT_GPIO_31_MODE_0_H       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_0_H_RSTV_GPIO_31_MODE_0_H       0x00000000


/**
@defgroup       GPIO_IOCTRL_1 GPIO_IOCTRL_1
@ingroup        Register
@brief          Configuration register for General Purpose IOs (63:32) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_1              0x00000020
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_1_RST__VAL     0xFFFFFFFF

/**
@defgroup       GPIO_IOCTRL_1__GPIO_IOCTRL_1 GPIO_IOCTRL_1
@ingroup        GPIO_IOCTRL_1
@brief          Configuration register for General Purpose IOs (63:32) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000020
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_1__MSK_GPIO_IOCTRL_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_1_SHFT_GPIO_IOCTRL_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_1_RSTV_GPIO_IOCTRL_1       0xFFFFFFFF


/**
@defgroup       GPIO_OUT_1 GPIO_OUT_1
@ingroup        Register
@brief          Output register for General Purpose IOs (63:32) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_1              0x00000024
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_1__GPIO_OUT_1 GPIO_OUT_1
@ingroup        GPIO_OUT_1
@brief          Output register for General Purpose IOs (63:32) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000024
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_1__MSK_GPIO_OUT_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_1_SHFT_GPIO_OUT_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_1_RSTV_GPIO_OUT_1       0x00000000


/**
@defgroup       GPIO_OUT_SET_1 GPIO_OUT_SET_1
@ingroup        Register
@brief          Bit-selective setting of the output register for General Purpose IOs (63:32) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_1              0x00000028
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_SET_1__GPIO_OUT_SET_1 GPIO_OUT_SET_1
@ingroup        GPIO_OUT_SET_1
@brief          Bit-selective setting of the output register for General Purpose IOs (63:32) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000028
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_1__MSK_GPIO_OUT_SET_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_1_SHFT_GPIO_OUT_SET_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_1_RSTV_GPIO_OUT_SET_1       0x00000000


/**
@defgroup       GPIO_OUT_CLEAR_1 GPIO_OUT_CLEAR_1
@ingroup        Register
@brief          Bit-selective reset of the output register for General Purpose IOs (63:32) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_1              0x0000002C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_CLEAR_1__GPIO_OUT_CLEAR_1 GPIO_OUT_CLEAR_1
@ingroup        GPIO_OUT_CLEAR_1
@brief          Bit-selective reset of the output register for General Purpose IOs (63:32) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000002C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_1__MSK_GPIO_OUT_CLEAR_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_1_SHFT_GPIO_OUT_CLEAR_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_1_RSTV_GPIO_OUT_CLEAR_1       0x00000000


/**
@defgroup       GPIO_RES_DIS_1 GPIO_RES_DIS_1
@ingroup        Register
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_1              0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_RES_DIS_1__GPIO_RES_DIS_1 GPIO_RES_DIS_1
@ingroup        GPIO_RES_DIS_1
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000030
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_1__MSK_GPIO_RES_DIS_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_1_SHFT_GPIO_RES_DIS_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_1_RSTV_GPIO_RES_DIS_1       0x00000000


/**
@defgroup       GPIO_IN_1 GPIO_IN_1
@ingroup        Register
@brief          Input register for General Purpose IOs (63:32) 0: GPIO inputx = 0, 1: GPIO inputx = 1
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_1              0x00000034
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_1_RST__VAL     0x00000000

/**
@defgroup       GPIO_IN_1__GPIO_IN_1 GPIO_IN_1
@ingroup        GPIO_IN_1
@brief          Input register for General Purpose IOs (63:32) 0: GPIO inputx = 0, 1: GPIO inputx = 1
@param Address  0x00000034
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_1__MSK_GPIO_IN_1       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_1_SHFT_GPIO_IN_1       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_1_RSTV_GPIO_IN_1       0x00000000


/**
@defgroup       GPIO_PORT_MODE_1_L GPIO_PORT_MODE_1_L
@ingroup        Register
@brief          Configuration register for GPIO port (47:32) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x00000038
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L              0x00000038
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_32_MODE_1_L GPIO_32_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(32)
@param Address  0x00000038
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_32_MODE_1_L       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_32_MODE_1_L       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_32_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_33_MODE_1_L GPIO_33_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(33)
@param Address  0x00000038
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_33_MODE_1_L       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_33_MODE_1_L       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_33_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_34_MODE_1_L GPIO_34_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(34)
@param Address  0x00000038
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_34_MODE_1_L       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_34_MODE_1_L       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_34_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_35_MODE_1_L GPIO_35_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(35)
@param Address  0x00000038
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_35_MODE_1_L       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_35_MODE_1_L       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_35_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_36_MODE_1_L GPIO_36_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(36)
@param Address  0x00000038
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_36_MODE_1_L       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_36_MODE_1_L       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_36_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_37_MODE_1_L GPIO_37_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(37)
@param Address  0x00000038
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_37_MODE_1_L       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_37_MODE_1_L       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_37_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_38_MODE_1_L GPIO_38_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(38)
@param Address  0x00000038
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_38_MODE_1_L       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_38_MODE_1_L       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_38_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_39_MODE_1_L GPIO_39_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(39)
@param Address  0x00000038
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_39_MODE_1_L       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_39_MODE_1_L       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_39_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_40_MODE_1_L GPIO_40_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(40)
@param Address  0x00000038
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_40_MODE_1_L       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_40_MODE_1_L       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_40_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_41_MODE_1_L GPIO_41_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(41)
@param Address  0x00000038
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_41_MODE_1_L       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_41_MODE_1_L       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_41_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_42_MODE_1_L GPIO_42_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(42)
@param Address  0x00000038
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_42_MODE_1_L       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_42_MODE_1_L       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_42_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_43_MODE_1_L GPIO_43_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(43)
@param Address  0x00000038
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_43_MODE_1_L       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_43_MODE_1_L       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_43_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_44_MODE_1_L GPIO_44_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(44)
@param Address  0x00000038
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_44_MODE_1_L       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_44_MODE_1_L       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_44_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_45_MODE_1_L GPIO_45_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(45)
@param Address  0x00000038
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_45_MODE_1_L       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_45_MODE_1_L       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_45_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_46_MODE_1_L GPIO_46_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(46)
@param Address  0x00000038
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_46_MODE_1_L       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_46_MODE_1_L       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_46_MODE_1_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_L__GPIO_47_MODE_1_L GPIO_47_MODE_1_L
@ingroup        GPIO_PORT_MODE_1_L
@brief          Port GPIO(47)
@param Address  0x00000038
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L__MSK_GPIO_47_MODE_1_L       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_SHFT_GPIO_47_MODE_1_L       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_L_RSTV_GPIO_47_MODE_1_L       0x00000000


/**
@defgroup       GPIO_PORT_MODE_1_H GPIO_PORT_MODE_1_H
@ingroup        Register
@brief          Configuration register for GPIO port (63:48) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x0000003C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H              0x0000003C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_48_MODE_1_H GPIO_48_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(48)
@param Address  0x0000003C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_48_MODE_1_H       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_48_MODE_1_H       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_48_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_49_MODE_1_H GPIO_49_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(49)
@param Address  0x0000003C
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_49_MODE_1_H       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_49_MODE_1_H       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_49_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_50_MODE_1_H GPIO_50_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(50)
@param Address  0x0000003C
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_50_MODE_1_H       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_50_MODE_1_H       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_50_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_51_MODE_1_H GPIO_51_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(51)
@param Address  0x0000003C
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_51_MODE_1_H       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_51_MODE_1_H       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_51_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_52_MODE_1_H GPIO_52_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(52)
@param Address  0x0000003C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_52_MODE_1_H       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_52_MODE_1_H       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_52_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_53_MODE_1_H GPIO_53_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(53)
@param Address  0x0000003C
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_53_MODE_1_H       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_53_MODE_1_H       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_53_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_54_MODE_1_H GPIO_54_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(54)
@param Address  0x0000003C
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_54_MODE_1_H       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_54_MODE_1_H       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_54_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_55_MODE_1_H GPIO_55_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(55)
@param Address  0x0000003C
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_55_MODE_1_H       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_55_MODE_1_H       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_55_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_56_MODE_1_H GPIO_56_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(56)
@param Address  0x0000003C
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_56_MODE_1_H       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_56_MODE_1_H       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_56_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_57_MODE_1_H GPIO_57_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(57)
@param Address  0x0000003C
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_57_MODE_1_H       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_57_MODE_1_H       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_57_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_58_MODE_1_H GPIO_58_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(58)
@param Address  0x0000003C
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_58_MODE_1_H       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_58_MODE_1_H       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_58_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_59_MODE_1_H GPIO_59_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(59)
@param Address  0x0000003C
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_59_MODE_1_H       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_59_MODE_1_H       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_59_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_60_MODE_1_H GPIO_60_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(60)
@param Address  0x0000003C
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_60_MODE_1_H       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_60_MODE_1_H       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_60_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_61_MODE_1_H GPIO_61_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(61)
@param Address  0x0000003C
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_61_MODE_1_H       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_61_MODE_1_H       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_61_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_62_MODE_1_H GPIO_62_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(62)
@param Address  0x0000003C
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_62_MODE_1_H       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_62_MODE_1_H       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_62_MODE_1_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_1_H__GPIO_63_MODE_1_H GPIO_63_MODE_1_H
@ingroup        GPIO_PORT_MODE_1_H
@brief          Port GPIO(63)
@param Address  0x0000003C
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H__MSK_GPIO_63_MODE_1_H       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_SHFT_GPIO_63_MODE_1_H       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_1_H_RSTV_GPIO_63_MODE_1_H       0x00000000


/**
@defgroup       GPIO_IOCTRL_2 GPIO_IOCTRL_2
@ingroup        Register
@brief          Configuration register for General Purpose IOs (95:64) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_2              0x00000040
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_2_RST__VAL     0xFFFFFFFF

/**
@defgroup       GPIO_IOCTRL_2__GPIO_IOCTRL_2 GPIO_IOCTRL_2
@ingroup        GPIO_IOCTRL_2
@brief          Configuration register for General Purpose IOs (95:64) 0: GPIOx = output, 1: GPIOx = input
@param Address  0x00000040
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0xFFFFFFFF
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_2__MSK_GPIO_IOCTRL_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_2_SHFT_GPIO_IOCTRL_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IOCTRL_2_RSTV_GPIO_IOCTRL_2       0xFFFFFFFF


/**
@defgroup       GPIO_OUT_2 GPIO_OUT_2
@ingroup        Register
@brief          Output register for General Purpose IOs (95:64) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_2              0x00000044
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_2_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_2__GPIO_OUT_2 GPIO_OUT_2
@ingroup        GPIO_OUT_2
@brief          Output register for General Purpose IOs (95:64) 0: GPIO outputx = 0, 1: GPIO outputx = 1
@param Address  0x00000044
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_2__MSK_GPIO_OUT_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_2_SHFT_GPIO_OUT_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_2_RSTV_GPIO_OUT_2       0x00000000


/**
@defgroup       GPIO_OUT_SET_2 GPIO_OUT_SET_2
@ingroup        Register
@brief          Bit-selective setting of the output register for General Purpose IOs (95:64) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_2              0x00000048
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_2_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_SET_2__GPIO_OUT_SET_2 GPIO_OUT_SET_2
@ingroup        GPIO_OUT_SET_2
@brief          Bit-selective setting of the output register for General Purpose IOs (95:64) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 1 Read always returns 0
@param Address  0x00000048
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_2__MSK_GPIO_OUT_SET_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_2_SHFT_GPIO_OUT_SET_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_SET_2_RSTV_GPIO_OUT_SET_2       0x00000000


/**
@defgroup       GPIO_OUT_CLEAR_2 GPIO_OUT_CLEAR_2
@ingroup        Register
@brief          Bit-selective reset of the output register for General Purpose IOs (95:64) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000004C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_2              0x0000004C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_2_RST__VAL     0x00000000

/**
@defgroup       GPIO_OUT_CLEAR_2__GPIO_OUT_CLEAR_2 GPIO_OUT_CLEAR_2
@ingroup        GPIO_OUT_CLEAR_2
@brief          Bit-selective reset of the output register for General Purpose IOs (95:64) For writing: 0: GPIO outputx remains unchanged  1: GPIO outputx = 0 Read always returns 0
@param Address  0x0000004C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   ,wt
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_2__MSK_GPIO_OUT_CLEAR_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_2_SHFT_GPIO_OUT_CLEAR_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_OUT_CLEAR_2_RSTV_GPIO_OUT_CLEAR_2       0x00000000


/**
@defgroup       GPIO_RES_DIS_2 GPIO_RES_DIS_2
@ingroup        Register
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000050
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_2              0x00000050
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_2_RST__VAL     0x00000000

/**
@defgroup       GPIO_RES_DIS_2__GPIO_RES_DIS_2 GPIO_RES_DIS_2
@ingroup        GPIO_RES_DIS_2
@brief          Bit-selective reset disabling of XRESET_GPIO_SM signal for registers of General Purpose IOs (31:0) For writing: 0: XRESET_GPIO_SM resets the corresponding register bit of all registers except of GPIO_RES_DIS  1: XRESET_GPIO_SM has no effect on the corresponding register bit
@param Address  0x00000050
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_2__MSK_GPIO_RES_DIS_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_2_SHFT_GPIO_RES_DIS_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_RES_DIS_2_RSTV_GPIO_RES_DIS_2       0x00000000


/**
@defgroup       GPIO_IN_2 GPIO_IN_2
@ingroup        Register
@brief          Input register for General Purpose IOs (95:64)
@param Address  0x00000054
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_2              0x00000054
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_2_RST__VAL     0x00000000

/**
@defgroup       GPIO_IN_2__GPIO_IN_2 GPIO_IN_2
@ingroup        GPIO_IN_2
@brief          Input register for General Purpose IOs (95:64)
@param Address  0x00000054
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   rh,
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_2__MSK_GPIO_IN_2       0xFFFFFFFF
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_2_SHFT_GPIO_IN_2       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_IN_2_RSTV_GPIO_IN_2       0x00000000


/**
@defgroup       GPIO_PORT_MODE_2_L GPIO_PORT_MODE_2_L
@ingroup        Register
@brief          Configuration register for GPIO port (79:64) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x00000058
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L              0x00000058
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_64_MODE_2_L GPIO_64_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(64)
@param Address  0x00000058
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_64_MODE_2_L       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_64_MODE_2_L       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_64_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_65_MODE_2_L GPIO_65_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(65)
@param Address  0x00000058
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_65_MODE_2_L       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_65_MODE_2_L       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_65_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_66_MODE_2_L GPIO_66_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(66)
@param Address  0x00000058
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_66_MODE_2_L       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_66_MODE_2_L       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_66_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_67_MODE_2_L GPIO_67_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(67)
@param Address  0x00000058
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_67_MODE_2_L       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_67_MODE_2_L       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_67_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_68_MODE_2_L GPIO_68_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(68)
@param Address  0x00000058
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_68_MODE_2_L       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_68_MODE_2_L       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_68_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_69_MODE_2_L GPIO_69_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(69)
@param Address  0x00000058
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_69_MODE_2_L       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_69_MODE_2_L       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_69_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_70_MODE_2_L GPIO_70_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(70)
@param Address  0x00000058
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_70_MODE_2_L       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_70_MODE_2_L       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_70_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_71_MODE_2_L GPIO_71_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(71)
@param Address  0x00000058
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_71_MODE_2_L       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_71_MODE_2_L       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_71_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_72_MODE_2_L GPIO_72_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(72)
@param Address  0x00000058
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_72_MODE_2_L       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_72_MODE_2_L       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_72_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_73_MODE_2_L GPIO_73_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(73)
@param Address  0x00000058
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_73_MODE_2_L       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_73_MODE_2_L       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_73_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_74_MODE_2_L GPIO_74_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(74)
@param Address  0x00000058
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_74_MODE_2_L       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_74_MODE_2_L       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_74_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_75_MODE_2_L GPIO_75_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(75)
@param Address  0x00000058
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_75_MODE_2_L       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_75_MODE_2_L       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_75_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_76_MODE_2_L GPIO_76_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(76)
@param Address  0x00000058
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_76_MODE_2_L       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_76_MODE_2_L       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_76_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_77_MODE_2_L GPIO_77_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(77)
@param Address  0x00000058
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_77_MODE_2_L       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_77_MODE_2_L       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_77_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_78_MODE_2_L GPIO_78_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(78)
@param Address  0x00000058
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_78_MODE_2_L       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_78_MODE_2_L       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_78_MODE_2_L       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_L__GPIO_79_MODE_2_L GPIO_79_MODE_2_L
@ingroup        GPIO_PORT_MODE_2_L
@brief          Port GPIO(79)
@param Address  0x00000058
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L__MSK_GPIO_79_MODE_2_L       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_SHFT_GPIO_79_MODE_2_L       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_L_RSTV_GPIO_79_MODE_2_L       0x00000000


/**
@defgroup       GPIO_PORT_MODE_2_H GPIO_PORT_MODE_2_H
@ingroup        Register
@brief          Configuration register for GPIO port (95:80) Function assignment: 00=function0 01=function1 10=function2 11=function3 
@param Address  0x0000005C
@param Mask     0xFFFFFFFF
@param Shift    0
@param Access   r,w
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H              0x0000005C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RST__VAL     0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_80_MODE_2_H GPIO_80_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(80)
@param Address  0x0000005C
@param Mask     0x00000003
@param Shift    0
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_80_MODE_2_H       0x00000003
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_80_MODE_2_H       0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_80_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_81_MODE_2_H GPIO_81_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(81)
@param Address  0x0000005C
@param Mask     0x0000000C
@param Shift    2
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_81_MODE_2_H       0x0000000C
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_81_MODE_2_H       2
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_81_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_82_MODE_2_H GPIO_82_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(82)
@param Address  0x0000005C
@param Mask     0x00000030
@param Shift    4
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_82_MODE_2_H       0x00000030
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_82_MODE_2_H       4
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_82_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_83_MODE_2_H GPIO_83_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(83)
@param Address  0x0000005C
@param Mask     0x000000C0
@param Shift    6
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_83_MODE_2_H       0x000000C0
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_83_MODE_2_H       6
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_83_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_84_MODE_2_H GPIO_84_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(84)
@param Address  0x0000005C
@param Mask     0x00000300
@param Shift    8
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_84_MODE_2_H       0x00000300
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_84_MODE_2_H       8
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_84_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_85_MODE_2_H GPIO_85_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(85)
@param Address  0x0000005C
@param Mask     0x00000C00
@param Shift    10
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_85_MODE_2_H       0x00000C00
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_85_MODE_2_H       10
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_85_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_86_MODE_2_H GPIO_86_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(86)
@param Address  0x0000005C
@param Mask     0x00003000
@param Shift    12
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_86_MODE_2_H       0x00003000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_86_MODE_2_H       12
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_86_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_87_MODE_2_H GPIO_87_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(87)
@param Address  0x0000005C
@param Mask     0x0000C000
@param Shift    14
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_87_MODE_2_H       0x0000C000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_87_MODE_2_H       14
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_87_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_88_MODE_2_H GPIO_88_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(88)
@param Address  0x0000005C
@param Mask     0x00030000
@param Shift    16
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_88_MODE_2_H       0x00030000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_88_MODE_2_H       16
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_88_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_89_MODE_2_H GPIO_89_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(89)
@param Address  0x0000005C
@param Mask     0x000C0000
@param Shift    18
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_89_MODE_2_H       0x000C0000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_89_MODE_2_H       18
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_89_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_90_MODE_2_H GPIO_90_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(90)
@param Address  0x0000005C
@param Mask     0x00300000
@param Shift    20
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_90_MODE_2_H       0x00300000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_90_MODE_2_H       20
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_90_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_91_MODE_2_H GPIO_91_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(91)
@param Address  0x0000005C
@param Mask     0x00C00000
@param Shift    22
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_91_MODE_2_H       0x00C00000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_91_MODE_2_H       22
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_91_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_92_MODE_2_H GPIO_92_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(92)
@param Address  0x0000005C
@param Mask     0x03000000
@param Shift    24
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_92_MODE_2_H       0x03000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_92_MODE_2_H       24
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_92_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_93_MODE_2_H GPIO_93_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(93)
@param Address  0x0000005C
@param Mask     0x0C000000
@param Shift    26
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_93_MODE_2_H       0x0C000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_93_MODE_2_H       26
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_93_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_94_MODE_2_H GPIO_94_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(94)
@param Address  0x0000005C
@param Mask     0x30000000
@param Shift    28
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_94_MODE_2_H       0x30000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_94_MODE_2_H       28
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_94_MODE_2_H       0x00000000

/**
@defgroup       GPIO_PORT_MODE_2_H__GPIO_95_MODE_2_H GPIO_95_MODE_2_H
@ingroup        GPIO_PORT_MODE_2_H
@brief          Port GPIO(95)
@param Address  0x0000005C
@param Mask     0xC0000000
@param Shift    30
@param Access   r,w
@param Reset    0x00000000
@param Version  -1
@param Type     -1
*/
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H__MSK_GPIO_95_MODE_2_H       0xC0000000
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_SHFT_GPIO_95_MODE_2_H       30
#define ERTEC200P_APBPER_GPIO_REG_GPIO_PORT_MODE_2_H_RSTV_GPIO_95_MODE_2_H       0x00000000

#ifdef __cplusplus
}
#endif

#endif

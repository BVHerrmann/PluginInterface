#ifndef EDDI_SYS_REG_ERTEC_H    //reinclude-protection
#define EDDI_SYS_REG_ERTEC_H


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
/*  F i l e               &F: eddi_sys_reg_ertec.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Register definitions for ERTEC200/400.                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 


/*****************************************************************************/
/*   MACROS                                                                  */
/*****************************************************************************/
//AddressOffsets relative to start of GPIOs
#define EDDI_GPIO_IOCTRL         0x00000000UL
#define EDDI_GPIO_OUT            0x00000004UL
#define EDDI_GPIO_PORT_MODE_H    0x00000010UL

//AddressOffsets relative to start of TIMERs
#define ERTEC_TIMER_CTRL_STAT0    0x00000000UL
#define ERTEC_TIMER_CTRL_STAT1    0x00000004UL
#define ERTEC_TIMER_RELD0         0x00000008UL
#define ERTEC_TIMER_RELD1         0x0000000CUL
#define ERTEC_TIMER_CTRL_PREDIV   0x00000010UL
#define ERTEC_TIMER_CTRL_RELD_PREDIV 0x00000014UL
#define ERTEC_TIMER_CTRL_TIM0     0x00000018UL
#define ERTEC_TIMER_CTRL_TIM1     0x0000001CUL

//AddressOffsets relative to start of IRTE
#define MUX_SELECTOR        0x00011030

#define ALTERNATE__GPIO         0x0UL
#define ALTERNATE__FUNCTION_1   0x1UL

#if defined (EDDI_CFG_LITTLE_ENDIAN)

//#define EDDI_SWAP_32(var)       (var)

#define EDDI_SWAP_VALUE32(var)

//#define EDDI_PASTE_32(a, b, c, d)                        (0x##a##b##c##d##L)

#else //EDDI_CFG_BIG_ENDIAN

#define EDDI_SWAP_VALUE32(var)  var = EDDI_SWAP_32(var);

LSA_UINT32 EDDI_SWAP_32_FCT( LSA_UINT32 const var );

/* defined in 'eddi_sys.h'
//
//#define EDDI_SWAP_32(var)                                 \
//(  ((((var)) & 0xFF000000UL) >> 24)    \
//+  ((((var)) & 0x00FF0000UL) >>  8)    \
//+  ((((var)) & 0x0000FF00UL) <<  8)    \
//+  ((((var)) & 0x000000FFUL) << 24))
//
//
//#define EDDI_PASTE_32(a, b, c, d)                        (0x##d##c##b##a##L)

end of define in  'eddi_sys.h'   */

#endif

//Register access
#define REG32b(base, offset)    (*(unsigned long volatile *)((unsigned char *)(base)+(offset)))

//Bitmasks
#define EDDI_BIT_MAX  31

//#define EDDI_BIT_MASK_PARA(BitHigh, BitLow)     (LSA_UINT32)BitHigh,(LSA_UINT32)BitLow

static LSA_UINT32   EDDI_SINGLE_MASKS_32[] = {
    EDDI_PASTE_32(00,00,00,01),
    EDDI_PASTE_32(00,00,00,02),
    EDDI_PASTE_32(00,00,00,04),
    EDDI_PASTE_32(00,00,00,08),
    EDDI_PASTE_32(00,00,00,10),
    EDDI_PASTE_32(00,00,00,20),
    EDDI_PASTE_32(00,00,00,40),
    EDDI_PASTE_32(00,00,00,80),

    EDDI_PASTE_32(00,00,01,00),
    EDDI_PASTE_32(00,00,02,00),
    EDDI_PASTE_32(00,00,04,00),
    EDDI_PASTE_32(00,00,08,00),
    EDDI_PASTE_32(00,00,10,00),
    EDDI_PASTE_32(00,00,20,00),
    EDDI_PASTE_32(00,00,40,00),
    EDDI_PASTE_32(00,00,80,00),

    EDDI_PASTE_32(00,01,00,00),
    EDDI_PASTE_32(00,02,00,00),
    EDDI_PASTE_32(00,04,00,00),
    EDDI_PASTE_32(00,08,00,00),
    EDDI_PASTE_32(00,10,00,00),
    EDDI_PASTE_32(00,20,00,00),
    EDDI_PASTE_32(00,40,00,00),
    EDDI_PASTE_32(00,80,00,00),

    EDDI_PASTE_32(01,00,00,00),
    EDDI_PASTE_32(02,00,00,00),
    EDDI_PASTE_32(04,00,00,00),
    EDDI_PASTE_32(08,00,00,00),
    EDDI_PASTE_32(10,00,00,00),
    EDDI_PASTE_32(20,00,00,00),
    EDDI_PASTE_32(40,00,00,00),
    EDDI_PASTE_32(80,00,00,00)   };

#define EDDI_MASK_32(a, b, c, d)                        (0x##a##b##c##d##L)

static LSA_UINT32   EDDI_MULTI_MASKS_32[] = {
    EDDI_MASK_32(00,00,00,01),
    EDDI_MASK_32(00,00,00,03),
    EDDI_MASK_32(00,00,00,07),
    EDDI_MASK_32(00,00,00,0F),
    EDDI_MASK_32(00,00,00,1F),
    EDDI_MASK_32(00,00,00,3F),
    EDDI_MASK_32(00,00,00,7F),
    EDDI_MASK_32(00,00,00,FF),

    EDDI_MASK_32(00,00,01,FF),
    EDDI_MASK_32(00,00,03,FF),
    EDDI_MASK_32(00,00,07,FF),
    EDDI_MASK_32(00,00,0F,FF),
    EDDI_MASK_32(00,00,1F,FF),
    EDDI_MASK_32(00,00,3F,FF),
    EDDI_MASK_32(00,00,7F,FF),
    EDDI_MASK_32(00,00,FF,FF),

    EDDI_MASK_32(00,01,FF,FF),
    EDDI_MASK_32(00,03,FF,FF),
    EDDI_MASK_32(00,07,FF,FF),
    EDDI_MASK_32(00,0F,FF,FF),
    EDDI_MASK_32(00,1F,FF,FF),
    EDDI_MASK_32(00,3F,FF,FF),
    EDDI_MASK_32(00,7F,FF,FF),
    EDDI_MASK_32(00,FF,FF,FF),

    EDDI_MASK_32(01,FF,FF,FF),
    EDDI_MASK_32(03,FF,FF,FF),
    EDDI_MASK_32(07,FF,FF,FF),
    EDDI_MASK_32(0F,FF,FF,FF),
    EDDI_MASK_32(1F,FF,FF,FF),
    EDDI_MASK_32(3F,FF,FF,FF),
    EDDI_MASK_32(7F,FF,FF,FF),
    EDDI_MASK_32(FF,FF,FF,FF)   };

/********************************/
/********** ERTEC 200 ***********/
/********************************/

/**** System-Control-Register-Block ****/
#ifdef EDDI_CFG_ERTEC_200

//Config_Reg
#define ERTEC200_AUX_CONFIG_REG_BIT__Reserved1                EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_CONFIG_REG_BIT__Config_6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC200_AUX_CONFIG_REG_BIT__Reserved2                EDDI_BIT_MASK_PARA(31, 7)

typedef struct ERTEC200_AUX_CONFIG_REG_S
{
    LSA_UINT32                      reg;
} ERTEC200_AUX_CONFIG_REG_T;

//PHY_CONFIG
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_ENB                  EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_FX_MODE                  EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_PHY_MODE                 EDDI_BIT_MASK_PARA( 4, 2)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P1_AUTO_MDIXEN              EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res0                        EDDI_BIT_MASK_PARA( 7, 6)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_ENB                  EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_FX_MODE                  EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_PHY_MODE                 EDDI_BIT_MASK_PARA(12,10)
#define ERTEC200_AUX_PHY_CONFIG_BIT__P2_AUTO_MDIXEN              EDDI_BIT_MASK_PARA(13,13)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res1                        EDDI_BIT_MASK_PARA(15,14)
#define ERTEC200_AUX_PHY_CONFIG_BIT__PHY_RES_SEL                 EDDI_BIT_MASK_PARA(16,16)
#define ERTEC200_AUX_PHY_CONFIG_BIT__res2                        EDDI_BIT_MASK_PARA(31,17)

typedef struct ERTEC200_AUX_PHY_CONFIG_S
{
    LSA_UINT32                      reg;
} ERTEC200_AUX_PHY_CONFIG_T;

//PHY_STATUS
#define ERTEC200_AUX_PHY_STATUS_BIT__P1_PWRUPRST              EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_PHY_STATUS_BIT__res0                     EDDI_BIT_MASK_PARA( 7, 1)
#define ERTEC200_AUX_PHY_STATUS_BIT__P2_PWRUPRST              EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_PHY_STATUS_BIT__res1                     EDDI_BIT_MASK_PARA(15, 9)
#define ERTEC200_AUX_PHY_STATUS_BIT__res3                     EDDI_BIT_MASK_PARA(31,16)

typedef struct ERTEC200_AUX_PHY_STATUS_S
{
    LSA_UINT32                      reg;
} ERTEC200_AUX_PHY_STATUS_T;

#endif

/**** GPIO ****/

//GPIO_PORT_MODE_L
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI0                EDDI_BIT_MASK_PARA( 1, 0)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI1                EDDI_BIT_MASK_PARA( 3, 2)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI2                EDDI_BIT_MASK_PARA( 5, 4)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI3                EDDI_BIT_MASK_PARA( 7, 6)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI4                EDDI_BIT_MASK_PARA( 9, 8)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI5                EDDI_BIT_MASK_PARA(11,10)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI6                EDDI_BIT_MASK_PARA(13,12)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI7                EDDI_BIT_MASK_PARA(15,14)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI8                EDDI_BIT_MASK_PARA(17,16)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI9                EDDI_BIT_MASK_PARA(19,18)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI10               EDDI_BIT_MASK_PARA(21,20)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI11               EDDI_BIT_MASK_PARA(23,22)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI12               EDDI_BIT_MASK_PARA(25,24)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI13               EDDI_BIT_MASK_PARA(27,26)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI14               EDDI_BIT_MASK_PARA(29,28)
#define ERTEC200_AUX_GPIO_PORT_MODE_L_BIT__GPI15               EDDI_BIT_MASK_PARA(31,30)

typedef struct ERTEC200_AUX_GPIO_PORT_MODE_L_S
{
    LSA_UINT32                            reg;
} ERTEC200_AUX_GPIO_PORT_MODE_L_T;

//GPIO_IOCTRL
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI0                 EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI7                 EDDI_BIT_MASK_PARA( 7, 7)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI8                 EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI9                 EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI10                EDDI_BIT_MASK_PARA(10,10)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI11                EDDI_BIT_MASK_PARA(11,11)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI12                EDDI_BIT_MASK_PARA(12,12)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI13                EDDI_BIT_MASK_PARA(13,13)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI14                EDDI_BIT_MASK_PARA(14,14)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI15                EDDI_BIT_MASK_PARA(15,15)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI16                EDDI_BIT_MASK_PARA(16,16)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI17                EDDI_BIT_MASK_PARA(17,17)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI18                EDDI_BIT_MASK_PARA(18,18)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI19                EDDI_BIT_MASK_PARA(19,19)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI20                EDDI_BIT_MASK_PARA(20,20)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI21                EDDI_BIT_MASK_PARA(21,21)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI22                EDDI_BIT_MASK_PARA(22,22)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI23                EDDI_BIT_MASK_PARA(23,23)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI24                EDDI_BIT_MASK_PARA(24,24)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI25                EDDI_BIT_MASK_PARA(25,25)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI26                EDDI_BIT_MASK_PARA(26,26)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI27                EDDI_BIT_MASK_PARA(27,27)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI28                EDDI_BIT_MASK_PARA(28,28)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI29                EDDI_BIT_MASK_PARA(29,29)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI30                EDDI_BIT_MASK_PARA(30,30)
#define ERTEC200_AUX_GPIO_IOCTRL_BIT__GPI31                EDDI_BIT_MASK_PARA(31,31)

typedef struct ERTEC200_AUX_GPIO_IOCTRL_S
{
    LSA_UINT32                        reg;
} ERTEC200_AUX_GPIO_IOCTRL_T;

//GPIO_OUT
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI0                 EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI7                 EDDI_BIT_MASK_PARA( 7, 7)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI8                 EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI9                 EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI10                EDDI_BIT_MASK_PARA(10,10)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI11                EDDI_BIT_MASK_PARA(11,11)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI12                EDDI_BIT_MASK_PARA(12,12)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI13                EDDI_BIT_MASK_PARA(13,13)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI14                EDDI_BIT_MASK_PARA(14,14)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI15                EDDI_BIT_MASK_PARA(15,15)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI16                EDDI_BIT_MASK_PARA(16,16)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI17                EDDI_BIT_MASK_PARA(17,17)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI18                EDDI_BIT_MASK_PARA(18,18)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI19                EDDI_BIT_MASK_PARA(19,19)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI20                EDDI_BIT_MASK_PARA(20,20)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI21                EDDI_BIT_MASK_PARA(21,21)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI22                EDDI_BIT_MASK_PARA(22,22)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI23                EDDI_BIT_MASK_PARA(23,23)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI24                EDDI_BIT_MASK_PARA(24,24)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI25                EDDI_BIT_MASK_PARA(25,25)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI26                EDDI_BIT_MASK_PARA(26,26)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI27                EDDI_BIT_MASK_PARA(27,27)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI28                EDDI_BIT_MASK_PARA(28,28)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI29                EDDI_BIT_MASK_PARA(29,29)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI30                EDDI_BIT_MASK_PARA(30,30)
#define ERTEC200_AUX_GPIO_OUT_BIT__GPI31                EDDI_BIT_MASK_PARA(31,31)

typedef struct ERTEC200_AUX_GPIO_OUT_S
{
    LSA_UINT32                   reg;
} ERTEC200_AUX_GPIO_OUT_T;

/********************************/
/********** ERTEC 400 ***********/
/********************************/

/**** GPIO ****/

//GPIO_PORT_MODE_L
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI0                EDDI_BIT_MASK_PARA( 1, 0)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI1                EDDI_BIT_MASK_PARA( 3, 2)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI2                EDDI_BIT_MASK_PARA( 5, 4)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI3                EDDI_BIT_MASK_PARA( 7, 6)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI4                EDDI_BIT_MASK_PARA( 9, 8)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI5                EDDI_BIT_MASK_PARA(11,10)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI6                EDDI_BIT_MASK_PARA(13,12)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI7                EDDI_BIT_MASK_PARA(15,14)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI8                EDDI_BIT_MASK_PARA(17,16)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI9                EDDI_BIT_MASK_PARA(19,18)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI10               EDDI_BIT_MASK_PARA(21,20)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI11               EDDI_BIT_MASK_PARA(23,22)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI12               EDDI_BIT_MASK_PARA(25,24)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI13               EDDI_BIT_MASK_PARA(27,26)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI14               EDDI_BIT_MASK_PARA(29,28)
#define ERTEC400_AUX_GPIO_PORT_MODE_L_BIT__GPI15               EDDI_BIT_MASK_PARA(31,30)

typedef struct ERTEC400_AUX_GPIO_PORT_MODE_L_S
{
    LSA_UINT32                           reg;
} ERTEC400_AUX_GPIO_PORT_MODE_L_T;

//GPIO_PORT_MODE_H
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI16               EDDI_BIT_MASK_PARA( 1, 0)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI17               EDDI_BIT_MASK_PARA( 3, 2)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI18               EDDI_BIT_MASK_PARA( 5, 4)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI19               EDDI_BIT_MASK_PARA( 7, 6)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI20               EDDI_BIT_MASK_PARA( 9, 8)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI21               EDDI_BIT_MASK_PARA(11,10)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI22               EDDI_BIT_MASK_PARA(13,12)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI23               EDDI_BIT_MASK_PARA(15,14)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI24               EDDI_BIT_MASK_PARA(17,16)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI25               EDDI_BIT_MASK_PARA(19,18)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI26               EDDI_BIT_MASK_PARA(21,20)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI27               EDDI_BIT_MASK_PARA(23,22)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI28               EDDI_BIT_MASK_PARA(25,24)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI29               EDDI_BIT_MASK_PARA(27,26)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI30               EDDI_BIT_MASK_PARA(29,28)
#define ERTEC400_AUX_GPIO_PORT_MODE_H_BIT__GPI31               EDDI_BIT_MASK_PARA(31,30)

typedef struct ERTEC400_AUX_GPIO_PORT_MODE_H_S
{
    LSA_UINT32                           reg;
} ERTEC400_AUX_GPIO_PORT_MODE_H_T;

//GPIO_IOCTRL
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI0                 EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI7                 EDDI_BIT_MASK_PARA( 7, 7)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI8                 EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI9                 EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI10                EDDI_BIT_MASK_PARA(10,10)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI11                EDDI_BIT_MASK_PARA(11,11)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI12                EDDI_BIT_MASK_PARA(12,12)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI13                EDDI_BIT_MASK_PARA(13,13)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI14                EDDI_BIT_MASK_PARA(14,14)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI15                EDDI_BIT_MASK_PARA(15,15)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI16                EDDI_BIT_MASK_PARA(16,16)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI17                EDDI_BIT_MASK_PARA(17,17)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI18                EDDI_BIT_MASK_PARA(18,18)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI19                EDDI_BIT_MASK_PARA(19,19)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI20                EDDI_BIT_MASK_PARA(20,20)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI21                EDDI_BIT_MASK_PARA(21,21)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI22                EDDI_BIT_MASK_PARA(22,22)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI23                EDDI_BIT_MASK_PARA(23,23)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI24                EDDI_BIT_MASK_PARA(24,24)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI25                EDDI_BIT_MASK_PARA(25,25)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI26                EDDI_BIT_MASK_PARA(26,26)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI27                EDDI_BIT_MASK_PARA(27,27)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI28                EDDI_BIT_MASK_PARA(28,28)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI29                EDDI_BIT_MASK_PARA(29,29)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI30                EDDI_BIT_MASK_PARA(30,30)
#define ERTEC400_AUX_GPIO_IOCTRL_BIT__GPI31                EDDI_BIT_MASK_PARA(31,31)

typedef struct ERTEC400_AUX_GPIO_IOCTRL_S
{
    LSA_UINT32                        reg;
} ERTEC400_AUX_GPIO_IOCTRL_T;

//GPIO_OUT
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI0                 EDDI_BIT_MASK_PARA( 0, 0)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI1                 EDDI_BIT_MASK_PARA( 1, 1)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI2                 EDDI_BIT_MASK_PARA( 2, 2)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI3                 EDDI_BIT_MASK_PARA( 3, 3)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI4                 EDDI_BIT_MASK_PARA( 4, 4)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI5                 EDDI_BIT_MASK_PARA( 5, 5)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI6                 EDDI_BIT_MASK_PARA( 6, 6)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI7                 EDDI_BIT_MASK_PARA( 7, 7)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI8                 EDDI_BIT_MASK_PARA( 8, 8)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI9                 EDDI_BIT_MASK_PARA( 9, 9)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI10                EDDI_BIT_MASK_PARA(10,10)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI11                EDDI_BIT_MASK_PARA(11,11)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI12                EDDI_BIT_MASK_PARA(12,12)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI13                EDDI_BIT_MASK_PARA(13,13)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI14                EDDI_BIT_MASK_PARA(14,14)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI15                EDDI_BIT_MASK_PARA(15,15)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI16                EDDI_BIT_MASK_PARA(16,16)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI17                EDDI_BIT_MASK_PARA(17,17)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI18                EDDI_BIT_MASK_PARA(18,18)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI19                EDDI_BIT_MASK_PARA(19,19)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI20                EDDI_BIT_MASK_PARA(20,20)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI21                EDDI_BIT_MASK_PARA(21,21)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI22                EDDI_BIT_MASK_PARA(22,22)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI23                EDDI_BIT_MASK_PARA(23,23)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI24                EDDI_BIT_MASK_PARA(24,24)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI25                EDDI_BIT_MASK_PARA(25,25)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI26                EDDI_BIT_MASK_PARA(26,26)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI27                EDDI_BIT_MASK_PARA(27,27)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI28                EDDI_BIT_MASK_PARA(28,28)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI29                EDDI_BIT_MASK_PARA(29,29)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI30                EDDI_BIT_MASK_PARA(30,30)
#define ERTEC400_AUX_GPIO_OUT_BIT__GPI31                EDDI_BIT_MASK_PARA(31,31)

typedef struct ERTEC400_AUX_GPIO_OUT_S
{
    LSA_UINT32                   reg;
} ERTEC400_AUX_GPIO_OUT_T;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_REG_ERTEC_H


/*****************************************************************************/
/*  end of file eddi_sys_reg_ertec.h                                         */
/*****************************************************************************/

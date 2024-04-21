#ifndef EDDI_SYS_REG_SOC12_H    //reinclude-protection
#define EDDI_SYS_REG_SOC12_H

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
/*  F i l e               &F: eddi_sys_reg_soc12.h                      :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Register definitions for SOC1/2. (based on template)                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/ 


/****************************/
/*   DDR2                   */
/****************************/
#define DDR2C_BASE                  0                 //needs to be accessed indirectly!

#define D_RAM_CONFIG                (DDR2C_BASE+0x0)
#define D_INIT_CONFIG               (DDR2C_BASE+0x4)
#define D_BUFFER_CONFIG             (DDR2C_BASE+0x8)
#define D_BA_RAS_CAS_SIZE           (DDR2C_BASE+0x10)
#define D_REFRESH_CTRL              (DDR2C_BASE+0x14)
#define D_COMMAND1_CTRL             (DDR2C_BASE+0x18)
#define D_COMMAND2_CTRL             (DDR2C_BASE+0x1C)
#define D_MODE_REG_SET              (DDR2C_BASE+0x20)
#define D_MODE1_REG_SET             (DDR2C_BASE+0x24)
#define D_MODE2_REG                 (DDR2C_BASE+0x28)
#define D_MODE3_REG                 (DDR2C_BASE+0x2C)
#define D_SINGLE_ADDR               (DDR2C_BASE+0x30)
#define D_SINGLE_PORT               (DDR2C_BASE+0x34)
#define D_DOUBLE_ADDR               (DDR2C_BASE+0x38)
#define D_DOUBLE_PORT               (DDR2C_BASE+0x3C)
#define D_EDC_INVERT_ADDR           (DDR2C_BASE+0x40)
#define D_INT_STATUS                (DDR2C_BASE+0x50)
#define D_INT_STATUS_SET            (DDR2C_BASE+0x54)
#define D_INT_MASK                  (DDR2C_BASE+0x58)
#define D_AGEING                    (DDR2C_BASE+0x60)
#define D_PRIO_PORT1                (DDR2C_BASE+0x70)
#define D_PRIO_PORT2                (DDR2C_BASE+0x74)
#define D_PRIO_PORT3                (DDR2C_BASE+0x78)
#define D_PRIO_PORT4                (DDR2C_BASE+0x7C)
#define D_PRIO_PORT5                (DDR2C_BASE+0x80)
#define D_PRIO_PORT6                (DDR2C_BASE+0x84)
#define D_PRIO_PORT7                (DDR2C_BASE+0x88)
#define D_PRIO_PORT8                (DDR2C_BASE+0x8C)
#define D_PRIO_PORT9                (DDR2C_BASE+0x90)
#define D_PRIO_PORT10               (DDR2C_BASE+0x94)
#define D_PRIO_PORT11               (DDR2C_BASE+0x98)
#define D_PRIO_PORT12               (DDR2C_BASE+0x9C)
#define D_INCRCONF_PORT1            (DDR2C_BASE+0xB0)
#define D_INCRCONF_PORT2            (DDR2C_BASE+0xB4)
#define D_INCRCONF_PORT3            (DDR2C_BASE+0xB8)
#define D_INCRCONF_PORT4            (DDR2C_BASE+0xBC)
#define D_INCRCONF_PORT5            (DDR2C_BASE+0xC0)
#define D_INCRCONF_PORT6            (DDR2C_BASE+0xC4)
#define D_INCRCONF_PORT7            (DDR2C_BASE+0xC8)
#define D_INCRCONF_PORT8            (DDR2C_BASE+0xCC)
#define D_INCRCONF_PORT9            (DDR2C_BASE+0xD0)
#define D_INCRCONF_PORT10           (DDR2C_BASE+0xD4)
#define D_INCRCONF_PORT11           (DDR2C_BASE+0xD8)
#define D_INCRCONF_PORT12           (DDR2C_BASE+0xDC)
#define D_WIN_B1                    (DDR2C_BASE+0xF0)
#define D_WIN_E1                    (DDR2C_BASE+0xF4)
#define D_WIN_B2                    (DDR2C_BASE+0xF8)
#define D_WIN_E2                    (DDR2C_BASE+0xFC)
#define D_WIN_B3                    (DDR2C_BASE+0x100)
#define D_WIN_E3                    (DDR2C_BASE+0x104)
#define D_WIN_B4                    (DDR2C_BASE+0x108)
#define D_WIN_E4                    (DDR2C_BASE+0x10C)
#define D_WIN_B5                    (DDR2C_BASE+0x110)
#define D_WIN_E5                    (DDR2C_BASE+0x114)
#define D_WIN_B6                    (DDR2C_BASE+0x118)
#define D_WIN_E6                    (DDR2C_BASE+0x11C)
#define D_WIN_PORT_CONFIG1          (DDR2C_BASE+0x120)
#define D_WIN_PORT_CONFIG2          (DDR2C_BASE+0x124)
#define D_WIN_PORT_CONFIG3          (DDR2C_BASE+0x128)
#define D_WIN_PORT_CONFIG4          (DDR2C_BASE+0x12C)
#define D_WIN_PORT_CONFIG5          (DDR2C_BASE+0x130)
#define D_WIN_PORT_CONFIG6          (DDR2C_BASE+0x134)
#define D_ADDR_F_WIN                (DDR2C_BASE+0x138)
#define D_WIN_INDICATION            (DDR2C_BASE+0x13C)
#define D_TRIG_COND                 (DDR2C_BASE+0x150)
#define D_TRIG_SETUP                (DDR2C_BASE+0x154)
#define D_HIST_PRE_COUNT            (DDR2C_BASE+0x158)
#define D_GHIST_SETUP               (DDR2C_BASE+0x15c)
#define D_HIST_CNTRL                (DDR2C_BASE+0x160)
#define D_HIST_BUF_PTR              (DDR2C_BASE+0x164)
#define D_HIST_BUFFER1              (DDR2C_BASE+0x168)
#define D_HIST_BUFFER2              (DDR2C_BASE+0x16c)
#define D_HIST_BUFFER3              (DDR2C_BASE+0x170)
#define D_VTP_IO_CONTRL1            (DDR2C_BASE+0x180)
#define D_VTP_IO_CNTRL2             (DDR2C_BASE+0x184)
#define D_PHY_CNTRL                 (DDR2C_BASE+0x188)
#define D_VERSION                   (DDR2C_BASE+0x1Fc)

/****************************/
/*   GPIO                   */
/****************************/
#define GPIO_BASE                   0       //needs to be accessed indirectly!

#define GPIO_IOCTRL_0       (GPIO_BASE+0x0)
#define GPIO_OUT_0          (GPIO_BASE+0x4)
#define GPIO_OUT_SET_0      (GPIO_BASE+0x8)
#define GPIO_OUT_CLEAR_0    (GPIO_BASE+0xC)
#define GPIO_RES_DIS_0      (GPIO_BASE+0x10)
#define GPIO_IN_0           (GPIO_BASE+0x14)
#define GPIO_PORT_MODE_0_L  (GPIO_BASE+0x18)
#define GPIO_PORT_MODE_0_H  (GPIO_BASE+0x1C)
#define GPIO_IOCTRL_1       (GPIO_BASE+0x20)
#define GPIO_OUT_1          (GPIO_BASE+0x24)
#define GPIO_OUT_SET_1      (GPIO_BASE+0x28)
#define GPIO_OUT_CLEAR_1    (GPIO_BASE+0x2C)
#define GPIO_RES_DIS_1      (GPIO_BASE+0x30)
#define GPIO_IN_1           (GPIO_BASE+0x34)
#define GPIO_PORT_MODE_1_L  (GPIO_BASE+0x38)
#define GPIO_PORT_MODE_1_H  (GPIO_BASE+0x3C)
#define GPIO_IOCTRL_2       (GPIO_BASE+0x40)
#define GPIO_OUT_2          (GPIO_BASE+0x44)
#define GPIO_OUT_SET_2      (GPIO_BASE+0x48)
#define GPIO_OUT_CLEAR_2    (GPIO_BASE+0x4C)
#define GPIO_RES_DIS_2      (GPIO_BASE+0x50)
#define GPIO_IN_2           (GPIO_BASE+0x54)
#define GPIO_PORT_MODE_2_L  (GPIO_BASE+0x58)
#define GPIO_PORT_MODE_2_H  (GPIO_BASE+0x5C)
#define GPIO_IOCTRL_3       (GPIO_BASE+0x60)
#define GPIO_OUT_3          (GPIO_BASE+0x64)
#define GPIO_OUT_SET_3      (GPIO_BASE+0x68)
#define GPIO_OUT_CLEAR_3    (GPIO_BASE+0x6C)
#define GPIO_RES_DIS_3      (GPIO_BASE+0x70)
#define GPIO_IN_3           (GPIO_BASE+0x74)
#define GPIO_PORT_MODE_3_L  (GPIO_BASE+0x78)
#define GPIO_PORT_MODE_3_H  (GPIO_BASE+0x7C)
#define GPIO_IOCTRL_4       (GPIO_BASE+0x80)
#define GPIO_OUT_4          (GPIO_BASE+0x84)
#define GPIO_OUT_SET_4      (GPIO_BASE+0x88)
#define GPIO_OUT_CLEAR_4    (GPIO_BASE+0x8C)
#define GPIO_RES_DIS_4      (GPIO_BASE+0x90)
#define GPIO_IN_4           (GPIO_BASE+0x94)
#define GPIO_PORT_MODE_4_L  (GPIO_BASE+0x98)
#define GPIO_PORT_MODE_4_H  (GPIO_BASE+0x9C)
#define GPIO_IOCTRL_5       (GPIO_BASE+0xA0)
#define GPIO_OUT_5          (GPIO_BASE+0xA4)
#define GPIO_OUT_SET_5      (GPIO_BASE+0xA8)
#define GPIO_OUT_CLEAR_5    (GPIO_BASE+0xAC)
#define GPIO_RES_DIS_5      (GPIO_BASE+0xB0)
#define GPIO_IN_5           (GPIO_BASE+0xB4)
#define GPIO_PORT_MODE_5_L  (GPIO_BASE+0xB8)
#define GPIO_PORT_MODE_5_H  (GPIO_BASE+0xBC)

#define SOC_GPIO_FUNC                        0
#define SOC_ALTERNATE_FUNC_0                 1
#define SOC_ALTERNATE_FUNC_1                 2
#define SOC_ALTERNATE_FUNC_2                 3

#define SOC_GPIO_MASK(x)                     (1 << (x % 32))
#define SOC_GPIO_PORT(x)                     (x / 32)
#define SOC_GPIO_IO_IN(x)                    SOC_GPIO_MASK(x)
#define SOC_GPIO_IO_OUT(x)                   (0)
#define SOC_GPIO_MODE(x, mode)               (((unsigned long long)mode) << ((x % 32)<<1))
#define SOC_GPIO_MODE_MASK(x)                (((unsigned long long)0x3) << ((x % 32)<<1))
#define GPIO_REG32(x,port)                   REG32(x + (0x20 * port))

// set GPIO ioctrl-register
#define SET_SOC_GPIO_IOCTRL(port, io, mask)  (GPIO_REG32(GPIO_IOCTRL_0,port) = ((GPIO_REG32(GPIO_IOCTRL_0,port) & (~(mask))) | io))

// set GPIO mode-register
#define SET_SOC_GPIO_MODE(port, mode, mask)  GPIO_REG32(GPIO_PORT_MODE_0_L,port) = ((GPIO_REG32(GPIO_PORT_MODE_0_L,port) & (~(GET64_L(mask)))) | GET64_L(mode)); \
GPIO_REG32(GPIO_PORT_MODE_0_H,port) = ((GPIO_REG32(GPIO_PORT_MODE_0_H,port) & (~(GET64_H(mask)))) | GET64_H(mode))

// set GPIO out-register
#define SET_SOC_GPIO_OUT(port, data)         (GPIO_REG32(GPIO_OUT_SET_0,port) = data)

// clear GPIO out-register
#define CLR_SOC_GPIO_OUT(port, data)         (GPIO_REG32(GPIO_OUT_CLEAR_0,port) = data)

// get GPIO input-register
#define GET_SOC_GPIO_IN(port, mask)          (GPIO_REG32(GPIO_IN_0,port) & mask)

/****************************/
/*   SCRB                   */
/****************************/
#define SCRB_BASE                   0                 //needs to be accessed indirectly!

#define FILT4IO_DSAVE_N             (SCRB_BASE+0x0)
#define FILT4IO_RXD_UART1           (SCRB_BASE+0x4)
#define FILT4IO_GPIO0               (SCRB_BASE+0x8)
#define FILT4IO_GPIO1               (SCRB_BASE+0xC)
#define FILT4IO_GPIO2               (SCRB_BASE+0x10)
#define FILT4IO_GPIO3               (SCRB_BASE+0x14)
#define FILT4IO_GPIO4               (SCRB_BASE+0x18)
#define FILT4IO_GPIO5               (SCRB_BASE+0x1C)
#define FILT4IO_GPIO6               (SCRB_BASE+0x20)
#define FILT4IO_GPIO7               (SCRB_BASE+0x24)
#define FILT4IO_GPIO8               (SCRB_BASE+0x28)
#define FILT4IO_GPIO9               (SCRB_BASE+0x2C)
#define FILT4IO_GPIO10              (SCRB_BASE+0x30)
#define FILT4IO_GPIO11              (SCRB_BASE+0x34)
#define FILT4IO_GPIO12              (SCRB_BASE+0x38)
#define FILT4IO_GPIO13              (SCRB_BASE+0x3C)
#define FILT4IO_GPIO14              (SCRB_BASE+0x40)
#define FILT4IO_GPIO15              (SCRB_BASE+0x44)
#define FILT4IO_GPIO16              (SCRB_BASE+0x48)
#define FILT4IO_GPIO17              (SCRB_BASE+0x4C)
#define FILT4IO_GPIO18              (SCRB_BASE+0x50)
#define FILT4IO_GPIO19              (SCRB_BASE+0x54)
#define FILT4IO_GPIO20              (SCRB_BASE+0x58)
#define FILT4IO_GPIO21              (SCRB_BASE+0x5C)
#define FILT4IO_GPIO22              (SCRB_BASE+0x60)
#define FILT4IO_GPIO23              (SCRB_BASE+0x64)
#define FILT4IO_GPIO24              (SCRB_BASE+0x68)
#define FILT4IO_GPIO25              (SCRB_BASE+0x6C)
#define FILT4IO_GPIO26              (SCRB_BASE+0x70)
#define FILT4IO_GPIO27              (SCRB_BASE+0x74)
#define FILT4IO_GPIO28              (SCRB_BASE+0x78)
#define FILT4IO_GPIO29              (SCRB_BASE+0x7C)

#define DSA_SCRB_CTRL               (SCRB_BASE+0x100)
#define IRTE_SCRB_CTRL              (SCRB_BASE+0x150)
#define USB_SCRB_CTRL               (SCRB_BASE+0x170)

#define DSA_SOFT_TRIG_WIDTH         (SCRB_BASE+0x200)
#define DSA_SOFT_TRIG               (SCRB_BASE+0x204)

#define CONFIG_HW                   (SCRB_BASE+0x300)
#define BOOT_HW                     (SCRB_BASE+0x304)
#define LOSS_LOCK                   (SCRB_BASE+0x308)
#define CLOCK_ENABLE                (SCRB_BASE+0x30C)
#define RES_STATE_REG               (SCRB_BASE+0x310)
#define RES_CTRL_REG                (SCRB_BASE+0x314)
#define RES_PULS_DURATION           (SCRB_BASE+0x318)
#define RESET_ENABLE                (SCRB_BASE+0x31C)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA0                                    (0x00000001)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA1                                    (0x00000002)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA2                                    (0x00000004)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA3                                    (0x00000008)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA4                                    (0x00000010)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_DSA5                                    (0x00000020)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES_USB                                       (0x00000040)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_PCI                                     (0x00000080)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES50_MMC                                     (0x00000100)
#define U_SCRB_CLOCK_RESET_UNIT__RESET_ENABLE__EN_RES48_FMIO                                    (0x00000200)

#define IOC_RES_DIS                 (SCRB_BASE+0x320)
#define LB_CONFIG                   (SCRB_BASE+0x324)

#define ENDIAN_SWAPPER_M            (SCRB_BASE+0x330)
#define ENDIAN_SWAPPER_SL           (SCRB_BASE+0x334)

#define PCI_SOFT_RES                (SCRB_BASE+0x400)
#define PCI_RES_STATE               (SCRB_BASE+0x404)
#define PCI_PM_STATE_REQ            (SCRB_BASE+0x408)
#define PCI_PM_STATE_ACK            (SCRB_BASE+0x40C)
#define PCI_PME                     (SCRB_BASE+0x410)
#define PCI_SOFT_RES_REQ            (SCRB_BASE+0x414)
#define PCI_SOFT_RES_ACK            (SCRB_BASE+0x418)
#define PCI_IRTE_CTRL               (SCRB_BASE+0x41C)
#define PCI_XSERR_INT_CTRL          (SCRB_BASE+0x420)
#define PCI_DIS_BUS                 (SCRB_BASE+0x424)
#define PCI_CLK_CONFIG              (SCRB_BASE+0x428)
#define PCI_XINTA_CTRL_REG          (SCRB_BASE+0x42C)
#define PCI_VTP_SOFT                (SCRB_BASE+0x430)
#define PCI_VTP_STATUS              (SCRB_BASE+0x434)
#define PCI_VTP_MD                  (SCRB_BASE+0x438)
#define PCI_VTP_REG                 (SCRB_BASE+0x43C)

#define MSML_ARBITRATION            (SCRB_BASE+0x500)
#define HSML_ARBITRATION            (SCRB_BASE+0x504)
#define QVZ_PCI_ADDR                (SCRB_BASE+0x508)
#define QVZ_PFB_ADDR                (SCRB_BASE+0x50C)
#define QVZ_GDMA_ADDR               (SCRB_BASE+0x510)
#define QVZ_IOCNTRL_M1_ADDR         (SCRB_BASE+0x514)
#define QVZ_IOCNTRL_M2_ADDR         (SCRB_BASE+0x518)
#define QVZ_IRTE_ADDR               (SCRB_BASE+0x51C)
#define QVZ_USB_ADDR                (SCRB_BASE+0x520)
#define QVZ_PCI_CTRL                (SCRB_BASE+0x524)
#define QVZ_PFB_CTRL                (SCRB_BASE+0x528)
#define QVZ_GDMA_CTRL               (SCRB_BASE+0x52C)
#define QVZ_IOCNTRL_M1_CTRL         (SCRB_BASE+0x530)
#define QVZ_IOCNTRL_M2_CTRL         (SCRB_BASE+0x534)
#define QVZ_IRTE_CTRL               (SCRB_BASE+0x538)
#define QVZ_USB_CTRL                (SCRB_BASE+0x53C)
#define QVZ_PCI                     (SCRB_BASE+0x540)
#define QVZ_AE_INT                  (SCRB_BASE+0x544)
#define QVZ_AE_ACK                  (SCRB_BASE+0x548)
#define QVZ_AE_MASK                 (SCRB_BASE+0x54C)
#define QVZ_HS_ADDR_PFB             (SCRB_BASE+0x550)
#define QVZ_HS_CTRL_PFB             (SCRB_BASE+0x554)
#define QVZ_HS_ADDR_MS2HSBRIDGE     (SCRB_BASE+0x558)
#define QVZ_HS_CTRL_MS2HSBRIDGE     (SCRB_BASE+0x55C)
#define AE_MS_ADDR_PFB_AHB          (SCRB_BASE+0x560)
#define AE_MS_CTRL_PFB_AHB          (SCRB_BASE+0x564)
#define OCP_MONITOR_STATUS          (SCRB_BASE+0x568)
#define OCP_MONITOR_CLEAR           (SCRB_BASE+0x56C)

#define QVZ_APB_ADDR                (SCRB_BASE+0x600)

#define CCR_I2C_0                   (SCRB_BASE+0x700)
#define CCR_I2C_1                   (SCRB_BASE+0x704)

#define NOR_RAM_AN_ADR0             (SCRB_BASE+0x708)

#define IOC_RAM_INIT_DONE           (SCRB_BASE+0x70c)

#define SOC1_ID                     (SCRB_BASE+0x800)
#define SOC2_ID                     (SCRB_BASE+0x804)
#define CHIP_ID                     (SCRB_BASE+0x810)
#define SC_ID_IOC                   (SCRB_BASE+0x814)
#define SC_ID_DDR2                  (SCRB_BASE+0x818)
#define SC_ID_CPU                   (SCRB_BASE+0x81C)

#define zsv_outputmux_cam           (SCRB_BASE+0x900)
#define zsv_outputmux_enc_if        (SCRB_BASE+0x904)
#define zsv_outputmux_mpulse1       (SCRB_BASE+0x908)
#define zsv_outputmux_mpulse2       (SCRB_BASE+0x90C)
#define zsv_outputmux_mpulse3       (SCRB_BASE+0x910)
#define zsv_outputmux_pdm           (SCRB_BASE+0x914)
#define zsv_outputmux_ssi           (SCRB_BASE+0x918)
#define zsv_outputmux_ASIC_IO0      (SCRB_BASE+0x91C)
#define zsv_outputmux_ASIC_IO1      (SCRB_BASE+0x920)
#define zsv_outputmux_dsa           (SCRB_BASE+0x924)
#define zsv_outputmux_ddr1          (SCRB_BASE+0x928)
#define zsv_outputmux_ddr2          (SCRB_BASE+0x92C)
#define zsv_outputmux_timer         (SCRB_BASE+0x930)

#define ENABLE_PN_PB_PORTS_REG      (SCRB_BASE+0xA00)
#define GPIO_PULL_CONTROL_1         (SCRB_BASE+0xA20)
#define GPIO_PULL_CONTROL_2         (SCRB_BASE+0xA24)
#define GPIO_PULL_CONTROL_3         (SCRB_BASE+0xA28)
#define GPIO_PULL_CONTROL_4         (SCRB_BASE+0xA2C)
#define GPIO_PULL_CONTROL_5         (SCRB_BASE+0xA30)
#define GPIO_PULL_CONTROL_6         (SCRB_BASE+0xA34)
#define GPIO_PULL_CONFIGURATION     (SCRB_BASE+0xA38)

#define OCDS                        (SCRB_BASE+0xB00)
#define MASK_DINT                   (SCRB_BASE+0xB50)

#define SLEW_RATE_CTRL              (SCRB_BASE+0xC00)

/****************************/
/*   IOCC                   */
/****************************/
#define IOCC_BASE                   0           //needs to be accessed indirectly!

#define U_IOCC_HOST_LL_inst_hwp0_ram_start                  (0x000020 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_ram_end                    (0x00061F + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_ram_start                  (0x000820 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_ram_end                    (0x000E1F + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_ll_ram_start                    (0x002000 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_ll_ram_end                      (0x003FFF + IOCC_BASE)
#define U_IOCC_PA_EA_DIRECT_start                           (0x100000 + IOCC_BASE)
#define U_IOCC_PA_EA_DIRECT_end                             (0x18FFFC + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_mb_adr                     (0x000000 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_mb_data                    (0x000004 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_mb_access                  (0x000008 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_mb_access_mb_type          (0x00000003)
#define U_IOCC_HOST_LL_inst_hwp0_cntrl                      (0x00000C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_cntrl_mb_en                (0x00000001)
#define U_IOCC_HOST_LL_inst_hwp0_cntrl_wrtrig               (0x00000002)
#define U_IOCC_HOST_LL_inst_hwp0_ll_sl                      (0x000010 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_ll_nl                      (0x000014 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_nmb_data                   (0x000018 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp0_dummy                      (0x00001C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_mb_adr                     (0x000620 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_mb_data                    (0x000624 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_mb_access                  (0x000628 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_mb_access_mb_type          (0x00000003)
#define U_IOCC_HOST_LL_inst_hrp0_cntrl                      (0x00062C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_cntrl_mb_en                (0x00000001)
#define U_IOCC_HOST_LL_inst_hrp0_cntrl_wrtrig               (0x00000002)
#define U_IOCC_HOST_LL_inst_hrp0_ll_sl                      (0x000630 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_ll_nl                      (0x000634 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_dest_adr                   (0x000638 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_dummy                      (0x00063C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_mb_adr                     (0x000800 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_mb_data                    (0x000804 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_mb_access                  (0x000808 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_mb_access_mb_type          (0x00000003)
#define U_IOCC_HOST_LL_inst_hwp1_cntrl                      (0x00080C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_cntrl_mb_en                (0x00000001)
#define U_IOCC_HOST_LL_inst_hwp1_cntrl_wrtrig               (0x00000002)
#define U_IOCC_HOST_LL_inst_hwp1_ll_sl                      (0x000810 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_ll_nl                      (0x000814 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_nmb_data                   (0x000818 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_dummy                      (0x00081C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_mb_adr                     (0x000E20 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_mb_data                    (0x000E24 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_mb_access                  (0x000E28 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_mb_access_mb_type          (0x00000003)
#define U_IOCC_HOST_LL_inst_hrp1_cntrl                      (0x000E2C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_cntrl_mb_en                (0x00000001)
#define U_IOCC_HOST_LL_inst_hrp1_cntrl_wrtrig               (0x00000002)
#define U_IOCC_HOST_LL_inst_hrp1_ll_sl                      (0x000E30 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_ll_nl                      (0x000E34 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_dest_adr                   (0x000E38 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_dummy                      (0x000E3C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_status                          (0x000F00 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_status_hwp0_used                (0x00000001)
#define U_IOCC_HOST_LL_inst_status_hwp0_acc_run             (0x00000002)
#define U_IOCC_HOST_LL_inst_status_hrp0_used                (0x00000004)
#define U_IOCC_HOST_LL_inst_status_hrp0_acc_run             (0x00000008)
#define U_IOCC_HOST_LL_inst_status_hwp1_used                (0x00000010)
#define U_IOCC_HOST_LL_inst_status_hwp1_acc_run             (0x00000020)
#define U_IOCC_HOST_LL_inst_status_hrp1_used                (0x00000040)
#define U_IOCC_HOST_LL_inst_status_hrp1_acc_run             (0x00000080)
#define U_IOCC_HOST_LL_inst_sync_reset                      (0x000F04 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_sync_reset_hwp0                 (0x00000001)
#define U_IOCC_HOST_LL_inst_sync_reset_hrp0                 (0x00000002)
#define U_IOCC_HOST_LL_inst_sync_reset_hwp1                 (0x00000004)
#define U_IOCC_HOST_LL_inst_sync_reset_hrp1                 (0x00000008)
#define U_IOCC_HOST_LL_inst_hwp0_dlength                    (0x000F08 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp0_dlength                    (0x000F0C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hwp1_dlength                    (0x000F10 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_hrp1_dlength                    (0x000F14 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_debug                           (0x000F18 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_debug_param_err                 (0x00000001)
#define U_IOCC_HOST_LL_inst_debug_dlength_err               (0x00000002)
#define U_IOCC_HOST_LL_inst_debug_paea_err                  (0x00000004)
#define U_IOCC_HOST_LL_inst_debug_size_err                  (0x00000008)
#define U_IOCC_HOST_LL_inst_debug_addr_err                  (0x00000010)
#define U_IOCC_HOST_LL_inst_iso_out_0_cntrl                 (0x000F1C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_1_cntrl                 (0x000F20 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_2_cntrl                 (0x000F24 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_3_cntrl                 (0x000F28 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_4_cntrl                 (0x000F2C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_5_cntrl                 (0x000F30 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_6_cntrl                 (0x000F34 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_7_cntrl                 (0x000F38 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_pdc_cntrl               (0x000F3C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_iso_out_pbusp_cntrl             (0x000F40 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_dsa_pll_in_cntrl                (0x000F44 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_pb_pll_in_cntrl                 (0x000F48 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_irte_pll_in_cntrl               (0x000F4C + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_clk_en                          (0x000F50 + IOCC_BASE)
#define U_IOCC_HOST_LL_inst_clk_en_io_cntrl                 (0x00000001)
#define U_IOCC_HOST_LL_inst_ll_ram_adr_offset               (0x000F54 + IOCC_BASE)

/****************************/
/*   GDMA                   */
/****************************/
#define GDMA_BASE                   0           //needs to be accessed indirectly!

#define U_gdma_GDMA_REG_ADDR                                 (0x00000 + GDMA_BASE)
#define U_gdma_GDMA_REG_ADDR_REG_ADDR                        (0xFFFFFFF0)
#define U_gdma_GDMA_LIST_ADDR                                (0x00004 + GDMA_BASE)
#define U_gdma_GDMA_LIST_ADDR_LIST_ADDR                      (0xFFFFFFF0)
#define U_gdma_GDMA_MAIN_CTRL                                (0x00008 + GDMA_BASE)
#define U_gdma_GDMA_MAIN_CTRL_DMA_EN                         (0x00000001)
#define U_gdma_GDMA_MAIN_CTRL_SW_RESET                       (0x00000002)
#define U_gdma_GDMA_MAIN_CTRL_JC_RESET                       (0x00008000)
#define U_gdma_GDMA_MAIN_CTRL_ERR_INT_EN                     (0x00010000)
#define U_gdma_GDMA_MAIN_CTRL_LIST_SIZE                      (0xFF000000)
#define U_gdma_GDMA_JC_EN                                    (0x0000C + GDMA_BASE)
#define U_gdma_GDMA_JOB0_CTRL                                (0x00010 + GDMA_BASE)
#define U_gdma_GDMA_JOBx_CTRL_SW_JOB_START                   (0x00000001)
#define U_gdma_GDMA_JOBx_CTRL_JOB_EN                         (0x00000002)
#define U_gdma_GDMA_JOBx_CTRL_HW_JOB_START_EN                (0x00000004)
#define U_gdma_GDMA_JOBx_CTRL_HW_FLOW_EN                     (0x00000008)
#define U_gdma_GDMA_JOBx_CTRL_INTR_EN                        (0x00000010)
#define U_gdma_GDMA_JOBx_CTRL_JOB_RESET                      (0x00000020)
#define U_gdma_GDMA_JOBx_CTRL_HW_SELECT                      (0x00003F00)
#define U_gdma_GDMA_JOBx_CTRL_JOB_PRIO                       (0x001F0000)
#define U_gdma_GDMA_JOBx_CTRL_TRANSFER_PTR                   (0xFF000000)
#define U_gdma_GDMA_JOB1_CTRL                                (0x00014 + GDMA_BASE)
#define U_gdma_GDMA_JOB2_CTRL                                (0x00018 + GDMA_BASE)
#define U_gdma_GDMA_JOB3_CTRL                                (0x0001C + GDMA_BASE)
#define U_gdma_GDMA_JOB4_CTRL                                (0x00020 + GDMA_BASE)
#define U_gdma_GDMA_JOB5_CTRL                                (0x00024 + GDMA_BASE)
#define U_gdma_GDMA_JOB6_CTRL                                (0x00028 + GDMA_BASE)
#define U_gdma_GDMA_JOB7_CTRL                                (0x0002C + GDMA_BASE)
#define U_gdma_GDMA_JOB8_CTRL                                (0x00030 + GDMA_BASE)
#define U_gdma_GDMA_JOB9_CTRL                                (0x00034 + GDMA_BASE)
#define U_gdma_GDMA_JOB10_CTRL                               (0x00038 + GDMA_BASE)
#define U_gdma_GDMA_JOB11_CTRL                               (0x0003C + GDMA_BASE)
#define U_gdma_GDMA_JOB12_CTRL                               (0x00040 + GDMA_BASE)
#define U_gdma_GDMA_JOB13_CTRL                               (0x00044 + GDMA_BASE)
#define U_gdma_GDMA_JOB14_CTRL                               (0x00048 + GDMA_BASE)
#define U_gdma_GDMA_JOB15_CTRL                               (0x0004C + GDMA_BASE)
#define U_gdma_GDMA_JOB16_CTRL                               (0x00050 + GDMA_BASE)
#define U_gdma_GDMA_JOB17_CTRL                               (0x00054 + GDMA_BASE)
#define U_gdma_GDMA_JOB18_CTRL                               (0x00058 + GDMA_BASE)
#define U_gdma_GDMA_JOB19_CTRL                               (0x0005C + GDMA_BASE)
#define U_gdma_GDMA_JOB20_CTRL                               (0x00060 + GDMA_BASE)
#define U_gdma_GDMA_JOB21_CTRL                               (0x00064 + GDMA_BASE)
#define U_gdma_GDMA_JOB22_CTRL                               (0x00068 + GDMA_BASE)
#define U_gdma_GDMA_JOB23_CTRL                               (0x0006C + GDMA_BASE)
#define U_gdma_GDMA_JOB24_CTRL                               (0x00070 + GDMA_BASE)
#define U_gdma_GDMA_JOB25_CTRL                               (0x00074 + GDMA_BASE)
#define U_gdma_GDMA_JOB26_CTRL                               (0x00078 + GDMA_BASE)
#define U_gdma_GDMA_JOB27_CTRL                               (0x0007C + GDMA_BASE)
#define U_gdma_GDMA_JOB28_CTRL                               (0x00080 + GDMA_BASE)
#define U_gdma_GDMA_JOB29_CTRL                               (0x00084 + GDMA_BASE)
#define U_gdma_GDMA_JOB30_CTRL                               (0x00088 + GDMA_BASE)
#define U_gdma_GDMA_JOB31_CTRL                               (0x0008C + GDMA_BASE)
#define U_gdma_GDMA_JOB_STATUS                               (0x00090 + GDMA_BASE)
#define U_gdma_GDMA_FINISHED_JOBS                            (0x00094 + GDMA_BASE)
#define U_gdma_GDMA_ACTUAL_STATUS                            (0x00098 + GDMA_BASE)
#define U_gdma_GDMA_ACTUAL_STATUS_ACT_JOB                    (0x0000001F)
#define U_gdma_GDMA_ACTUAL_STATUS_ACT_JOB_VAL                (0x00000020)
#define U_gdma_GDMA_IRQ_STATUS                               (0x0009C + GDMA_BASE)
#define U_gdma_GDMA_ERR_IRQ_STATUS                           (0x000A0 + GDMA_BASE)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_DST_ADDR              (0x00000001)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_DST_ADDR_JOB_NR       (0x0000003E)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_AHB                   (0x00000040)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_AHB_JOB_NR            (0x00000F80)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_JOB_START             (0x00001000)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_JOB_START_JOB_NR      (0x0003E000)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_AHB_SLV_WRITE         (0x00040000)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_AM_HOLD               (0x00080000)
#define U_gdma_GDMA_ERR_IRQ_STATUS_ERR_AM_HOLD_JOB_NR        (0x01F00000)
#define U_gdma_GDMA_JOB_COUNT                                (0x000A4 + GDMA_BASE)
#define U_gdma_REVISION_CODE                                 (0x000A8 + GDMA_BASE)
#define U_gdma_REVISION_CODE_MINOR_REVISION                  (0x000000FF)
#define U_gdma_REVISION_CODE_MAJOR_REVISION                  (0x0000FF00)
#define U_gdma_LIST_RAM_START                                (0x000B0 + GDMA_BASE)
#define U_gdma_LIST_RAM_END                                  (0x010AF + GDMA_BASE)
#define U_gdma_JOB_STACK_RAM_START                           (0x010B0 + GDMA_BASE)
#define U_gdma_JOB_STACK_RAM_END                             (0x012AF + GDMA_BASE)

#define U_gdma_BURST_MODE_SINGLE                    0
#define U_gdma_BURST_MODE_INCR4                     1
#define U_gdma_BURST_MODE_INCR8                     2
#define U_gdma_BURST_MODE_INCR16                    3
#define U_gdma_AMODE_INCR_ADDRESS                   0
#define U_gdma_AMODE_HOLD_ADDRESS                   2
#define U_gdma_ESIZE_8BIT                           0
#define U_gdma_ESIZE_16BIT                          1
#define U_gdma_ESIZE_32BIT                          2

/* flow-control-signals = fixed job-nr ! */
#define U_gdma_FLOWCONTROL_DW_DMA_REQ               0       //SD_MMC
#define U_gdma_FLOWCONTROL_UART0RXFNE               1
#define U_gdma_FLOWCONTROL_UART0TXINTR              2
#define U_gdma_FLOWCONTROL_UART1RXFNE               3
#define U_gdma_FLOWCONTROL_UART1TXINTR              4
#define U_gdma_FLOWCONTROL_UART2RXFNE               5       //SOC2 only!!
#define U_gdma_FLOWCONTROL_UART2TXINTR              6       //SOC2 only!!
#define U_gdma_FLOWCONTROL_SSP_SSPRXDMA             7
#define U_gdma_FLOWCONTROL_SSPTXINTR                8

#define U_gdma_FLOWCONTROL_HIGHEST_NR               8

/* job-start-signals */
#define U_gdma_JOBSTART_SPS                         16      //SOC1 only!!

/****************************/
/*   Timer                  */
/****************************/
#define TIMER_BASE                  0           //needs to be accessed indirectly!

#define TIM_0_MODE_REG              (TIMER_BASE+0x0)
#define TIM_0_PRESCALER_REG         (TIMER_BASE+0x4)
#define TIM_0_LOAD_REG              (TIMER_BASE+0x8)
#define TIM_0_COUNT_REG             (TIMER_BASE+0xC)
#define TIM_0_INT_EV_REG            (TIMER_BASE+0x10)
#define TIM_0_EXT_EV_1_REG          (TIMER_BASE+0x14)
#define TIM_0_EXT_EV_2_REG          (TIMER_BASE+0x18)
#define TIM_1_MODE_REG              (TIMER_BASE+0x20)
#define TIM_1_PRESCALER_REG         (TIMER_BASE+0x24)
#define TIM_1_LOAD_REG              (TIMER_BASE+0x28)
#define TIM_1_COUNT_REG             (TIMER_BASE+0x2C)
#define TIM_1_INT_EV_REG            (TIMER_BASE+0x30)
#define TIM_1_EXT_EV_1_REG          (TIMER_BASE+0x34)
#define TIM_1_EXT_EV_2_REG          (TIMER_BASE+0x38)
#define TIM_2_MODE_REG              (TIMER_BASE+0x40)
#define TIM_2_PRESCALER_REG         (TIMER_BASE+0x44)
#define TIM_2_LOAD_REG              (TIMER_BASE+0x48)
#define TIM_2_COUNT_REG             (TIMER_BASE+0x4C)
#define TIM_2_INT_EV_REG            (TIMER_BASE+0x50)
#define TIM_2_EXT_EV_1_REG          (TIMER_BASE+0x54)
#define TIM_2_EXT_EV_2_REG          (TIMER_BASE+0x58)
#define TIM_3_MODE_REG              (TIMER_BASE+0x60)
#define TIM_3_PRESCALER_REG         (TIMER_BASE+0x64)
#define TIM_3_LOAD_REG              (TIMER_BASE+0x68)
#define TIM_3_COUNT_REG             (TIMER_BASE+0x6C)
#define TIM_3_INT_EV_REG            (TIMER_BASE+0x70)
#define TIM_3_EXT_EV_1_REG          (TIMER_BASE+0x74)
#define TIM_3_EXT_EV_2_REG          (TIMER_BASE+0x78)
#define TIM_4_MODE_REG              (TIMER_BASE+0x80)
#define TIM_4_PRESCALER_REG         (TIMER_BASE+0x84)
#define TIM_4_LOAD_REG              (TIMER_BASE+0x88)
#define TIM_4_COUNT_REG             (TIMER_BASE+0x8C)
#define TIM_4_INT_EV_REG            (TIMER_BASE+0x90)
#define TIM_4_EXT_EV_1_REG          (TIMER_BASE+0x94)
#define TIM_4_EXT_EV_2_REG          (TIMER_BASE+0x98)
#define TIM_5_MODE_REG              (TIMER_BASE+0xA0)
#define TIM_5_PRESCALER_REG         (TIMER_BASE+0xA4)
#define TIM_5_LOAD_REG              (TIMER_BASE+0xA8)
#define TIM_5_COUNT_REG             (TIMER_BASE+0xAC)
#define TIM_5_INT_EV_REG            (TIMER_BASE+0xB0)
#define TIM_5_EXT_EV_1_REG          (TIMER_BASE+0xB4)
#define TIM_5_EXT_EV_2_REG          (TIMER_BASE+0xB8)
#define GATE_TRIG_CONTROL_REG       (TIMER_BASE+0xC0)
#define CLOCK_DIVIDER_REG           (TIMER_BASE+0xC4)
#define EXT_GATE_TRIG_MUX_REG       (TIMER_BASE+0xC8)
#define EXT_EV_1_MUX_REG            (TIMER_BASE+0xCC)
#define EXT_EV_2_MUX_REG            (TIMER_BASE+0xD0)
#define SW_EVENT_TRIGGER_REG        (TIMER_BASE+0xD4)

/****************************/
/*   I2C                    */
/****************************/
#define I2C_BASE                        0       //needs to be accessed indirectly!

#define SOC_REG_I2C_MI2C_ADDR           (I2C_BASE+0x0)
#define SOC_REG_I2C_MI2C_DATA           (I2C_BASE+0x4)
#define SOC_REG_I2C_MI2_CNTR            (I2C_BASE+0x8)
#define SOC_REG_I2C_MI2_STAT            (I2C_BASE+0xC)
#define SOC_REG_I2C_MI2C_XADDR          (I2C_BASE+0x10)
#define SOC_REG_I2C_MI2C_SOFTWARE_RESET (I2C_BASE+0x1C)
#define SOC_REG_I2C_EX_CTRL_1           (I2C_BASE+0x20)
#define SOC_REG_I2C_EX_ADDR_1           (I2C_BASE+0x24)
#define SOC_REG_I2C_EX_DATA_OUT_1       (I2C_BASE+0x28)
#define SOC_REG_I2C_EX_DATA_IN_1        (I2C_BASE+0x2C)
#define SOC_REG_I2C_EX_CTRL_2           (I2C_BASE+0x30)
#define SOC_REG_I2C_EX_ADDR_2           (I2C_BASE+0x34)
#define SOC_REG_I2C_EX_DATA_OUT_2       (I2C_BASE+0x38)
#define SOC_REG_I2C_EX_DATA_IN_2        (I2C_BASE+0x3C)
#define SOC_REG_I2C_EX_CTRL_3           (I2C_BASE+0x40)
#define SOC_REG_I2C_EX_ADDR_3           (I2C_BASE+0x44)
#define SOC_REG_I2C_EX_DATA_OUT_3       (I2C_BASE+0x48)
#define SOC_REG_I2C_EX_DATA_IN_3        (I2C_BASE+0x4C)
#define SOC_REG_I2C_EX_CTRL_4           (I2C_BASE+0x50)
#define SOC_REG_I2C_EX_ADDR_4           (I2C_BASE+0x54)
#define SOC_REG_I2C_EX_DATA_OUT_4       (I2C_BASE+0x58)
#define SOC_REG_I2C_EX_DATA_IN_4        (I2C_BASE+0x5C)
#define SOC_REG_I2C_ERROR_SLAVE_ADDRESS (I2C_BASE+0x60)
#define SOC_REG_I2C_SW_I2C_EN           (I2C_BASE+0x64)
#define SOC_REG_I2C_SW_I2C_CTRL         (I2C_BASE+0x68)

//SOC_REG_I2C_MI2_CNTR:
//*********************
// ACCESS__: r(h),(w)
// DESCR__:
#define SOC_REG_I2C_MI2_CNTR__MSK_IEN                       0x00000080
#define SOC_REG_I2C_MI2_CNTR_SHFT_IEN                       7
// ACCESS__IEN: r,w
// DESCR__IEN: Interrupt enable
#define SOC_REG_I2C_MI2_CNTR__MSK_ENAB                      0x00000040
#define SOC_REG_I2C_MI2_CNTR_SHFT_ENAB                      6
// ACCESS__ENAB: r,w
// DESCR__ENAB: Bus enable
#define SOC_REG_I2C_MI2_CNTR__MSK_STA                       0x00000020
#define SOC_REG_I2C_MI2_CNTR_SHFT_STA                       5
// ACCESS__STA: rh,w
// DESCR__STA: Master mode start
#define SOC_REG_I2C_MI2_CNTR__MSK_STP                       0x00000010
#define SOC_REG_I2C_MI2_CNTR_SHFT_STP                       4
// ACCESS__STP: rh,w
// DESCR__STP: Master mode stop
#define SOC_REG_I2C_MI2_CNTR__MSK_IFLG                      0x00000008
#define SOC_REG_I2C_MI2_CNTR_SHFT_IFLG                      3
// ACCESS__IFLG: rh,
// DESCR__IFLG: Interrupt flag
#define SOC_REG_I2C_MI2_CNTR__MSK_AAK                       0x00000004
#define SOC_REG_I2C_MI2_CNTR_SHFT_AAK                       2
// ACCESS__AAK: r,w
// DESCR__AAK: Assert acknowledge

//SOC_REG_I2C_MI2_STAT:
//*********************
// ACCESS__: rh
// DESCR__:
#define SOC_REG_I2C_MI2_STAT__MSK_STATUS                    0x000000FF
#define SOC_REG_I2C_MI2_STAT_SHFT_STATUS                    0
// ACCESS__STATUS: rh,
// DESCR__STATUS: This read only register contains a 5-bit status code. There are 31 possible status codes. When STAT contains the status code F8h, no relevant status information is available, no interrupt is generated and the IFLG bit in the CNTR register is not set. All other status codes correspond to a defined state of the MI2C.

//SOC_REG_I2C_MI2C_SOFTWARE_RESET:
//********************************
// ACCESS__: r,w
// DESCR__:
#define SOC_REG_I2C_MI2C_SOFTWARE_RESET__MSK_SOFTRESET      0x000000FF
#define SOC_REG_I2C_MI2C_SOFTWARE_RESET_SHFT_SOFTRESET      0
// ACCESS__SOFTRESET: r,w
// DESCR__SOFTRESET: Softreset

//SOC_REG_I2C_EX_CTRL_1:
//**********************
// ACCESS__: r(h),(w)
// DESCR__:
#define SOC_REG_I2C_EX_CTRL_1__MSK_SCL_TOGGLE               0x00000080
#define SOC_REG_I2C_EX_CTRL_1_SHFT_SCL_TOGGLE               7
// ACCESS__SCL_TOGGLE: r,w
// DESCR__SCL_TOGGLE: SCL Toggle Bit   (nur im Register EX_Control_1 vorhanden)\n ‚0’: SCL Ausgang vom Hardwaresteuerung (MI2C: Inventra)\n ‚1’: SCL auf low geklemmt.\nHinweis:\nWenn ein Deadlock am I2C-Bus entsteht kann man durch Schreiben von Bit SCL_Toggle (nur im Register Ex_Control_1 vorhanden) den SCL Clock Pin beeinflussen und so den Deadlock auflösen.
#define SOC_REG_I2C_EX_CTRL_1__MSK_ERROR                    0x00000010
#define SOC_REG_I2C_EX_CTRL_1_SHFT_ERROR                    4
// ACCESS__ERROR: rh
// DESCR__ERROR: Read-only.\n0   = No Error, default\n1  = Error occured
#define SOC_REG_I2C_EX_CTRL_1__MSK_BUSY                     0x00000008
#define SOC_REG_I2C_EX_CTRL_1_SHFT_BUSY                     3
// ACCESS__BUSY: rh
// DESCR__BUSY: Read-only.\n0   = Currently inactive, default\n1  = Currently active
#define SOC_REG_I2C_EX_CTRL_1__MSK_IN                       0x00000004
#define SOC_REG_I2C_EX_CTRL_1_SHFT_IN                       2
// ACCESS__IN: rh,w
// DESCR__IN: 0   = Read service off, default\n1   = Read service on \n Read input data as specified in MODE
#define SOC_REG_I2C_EX_CTRL_1__MSK_OUT                      0x00000002
#define SOC_REG_I2C_EX_CTRL_1_SHFT_OUT                      1
// ACCESS__OUT: rh,w
// DESCR__OUT: 0   = Write service off, default\n1   = Write service on\n Send output data as specified in MODE
#define SOC_REG_I2C_EX_CTRL_1__MSK_MODE                     0x00000001
#define SOC_REG_I2C_EX_CTRL_1_SHFT_MODE                     0
// ACCESS__MODE: r,w
// DESCR__MODE: 0   = Service on demand, default\n1   = Periodic service

//SOC_REG_I2C_SW_I2C_EN:
//**********************
// ACCESS__: r,w
// DESCR__:
#define SOC_REG_I2C_SW_I2C_EN__MSK_I2C_PARAMENABLE          0x00000001
#define SOC_REG_I2C_SW_I2C_EN_SHFT_I2C_PARAMENABLE          0
// ACCESS__I2C_PARAMENABLE: r,w
// DESCR__I2C_PARAMENABLE: Bei der I2C-Schnittstelle ist die:\n ’0’: Hardware-Steuerung (siehe /10/) aktiviert,\n                    Software-Steuerung gesperrt\n ’1’: Software-Steuerung (per Register I2C_Control) aktiviert,\n                    Hardware-Steuerung gesperrt

//SOC_REG_I2C_SW_I2C_CTRL:
//************************
// ACCESS__: r(h),(w)
// DESCR__: 
#define SOC_REG_I2C_SW_I2C_CTRL__MSK_SDA_I                  0x00000008
#define SOC_REG_I2C_SW_I2C_CTRL_SHFT_SDA_I                  3
// ACCESS__SDA_I: rh,
// DESCR__SDA_I: read only (Schreibzugriffe werden ignoriert)\nPolarität der I2C Eingangsdaten-Leitung (es muss dazu I2C_Enable.I2C_ParamEnable  =’0’ sein, anderen Falls zeigt das Bit konstant ’1’ an)\n '0' : I2C Daten-Leitung  = '0'\n '1' : I2C Daten-Leitung  = '1'
#define SOC_REG_I2C_SW_I2C_CTRL__MSK_SCL_I                  0x00000004
#define SOC_REG_I2C_SW_I2C_CTRL_SHFT_SCL_I                  2
// ACCESS__SCL_I: rh
// DESCR__SCL_I: read only (Schreibzugriffe werden ignoriert)\nPolarität der I2C Eingangstakt-Leitung lesbar (es muss dazu I2C_Enable.I2C_ParamEnable  =’0’ sein, anderen Falls zeigt das Bit konstant ’1’ an)\n ’0' : I2C Takt-Leitung  = '0'\n '1' : I2C Takt-Leitung  = '1'
#define SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SDA_O               0x00000002
#define SOC_REG_I2C_SW_I2C_CTRL_SHFT_SW_SDA_O               1
// ACCESS__SW_SDA_O: r,w
// DESCR__SW_SDA_O: Ist die Software-Steuerung der I2C-Schnittstelle aktiviert (I2C_Enable.I2C_ParamEnable  = ’1’) so kann mit Bit das I2C Ausgangsdatensignal wie folgt erzeugt werden: \n ‘0’: I2C Ausgangsdaten  = ‘0’\n ‘1’: I2C Ausgangsdaten   = Tristate (Default)
#define SOC_REG_I2C_SW_I2C_CTRL__MSK_SW_SCL_O               0x00000001
#define SOC_REG_I2C_SW_I2C_CTRL_SHFT_SW_SCL_O               0
// ACCESS__SW_SCL_O: r,w
// DESCR__SW_SCL_O: Ist die Software-Steuerung der I2C-Schnittstelle aktiviert (I2C_Enable.I2C_ParamEnable  =’1’) so kann mit Bit SW_SCL_O der I2C Ausgangstakt wie folgt erzeugt werden: \n ‘0’: I2C Ausgangstakt  = ‘0’\n ‘1’: I2C Ausgangstakt   = Tristate (Default)

/****************************/
/*   IRTE                   */
/****************************/
#define SOC1_IRTE_BASE  0               //needs to be accessed indirectly!
#define U_IRTE_ZYKLUSSYNCHRONISATION_SC_BUS_INST__MUX_SELEKTOR   (0x11030 + SOC1_IRTE_BASE)

/****************************/
/*   MISC                   */
/****************************/
#define REG32b(base, offset)    (*(unsigned long volatile *)((unsigned char *)(base)+(offset)))
#define GET64_L(x)              ((unsigned long)x)
#define GET64_H(x)              ((unsigned long)(((unsigned long long)x)>>32))

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_REG_SOC12_H


/*****************************************************************************/
/*  end of file eddi_sys_reg_soc12.h                                         */
/*****************************************************************************/

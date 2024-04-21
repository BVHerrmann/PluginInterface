#ifndef EDDI_SER_ADR_R6_H       //reinclude-protection
#define EDDI_SER_ADR_R6_H

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
/*  F i l e               &F: eddi_ser_adr_r6.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                       internal Register of SER10 - Rev6                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Version        Who  What                                     */
/*                                                                           */
/*****************************************************************************/

#if defined (EDDI_CFG_REV6)

/*===========================================================================*/
/*                        A D D R E S S - D E F I N E S                      */
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_DEV_ETH_PORT_X_TYPE ========================*/
/*===========================================================================*/

//PORT 0
//#define P0_PORT_CTRL        0x00000000
#define P0_MIN_PREAMBLE       0x00000004
#define P0_IRT_DIAG           0x00000008
#define P0_LINE_DELAY         0x0000000C
#define P0_NRT_CTRL           0x00000010
#define P0_DEFAULT_VLAN_TAG   0x00000014

//PORT 1
//#define P1_PORT_CTRL        0x00001000
#define P1_MIN_PREAMBLE       0x00001004
#define P1_IRT_DIAG           0x00001008
#define P1_LINE_DELAY         0x0000100C
#define P1_NRT_CTRL           0x00001010
#define P1_DEFAULT_VLAN_TAG   0x00001014

#if defined (EDDI_CFG_ERTEC_400)
//PORT 2
//#define P2_PORT_CTRL        0x00002000
#define P2_MIN_PREAMBLE       0x00002004
#define P2_IRT_DIAG           0x00002008
#define P2_LINE_DELAY         0x0000200C
#define P2_NRT_CTRL           0x00002010
#define P2_DEFAULT_VLAN_TAG   0x00002014

//PORT 3
#define P3_PORT_CTRL          0x00003000
#define P3_MIN_PREAMBLE       0x00003004
#define P3_IRT_DIAG           0x00003008
#define P3_LINE_DELAY         0x0000300C
#define P3_NRT_CTRL           0x00003010
#define P3_DEFAULT_VLAN_TAG   0x00003014
#endif

/*===========================================================================*/

/*===========================================================================*/
/*====================== END OF EDDI_DEV_ETH_PORT_X_TYPE ====================*/
/*===========================================================================*/
/*===========================================================================*/
/*========================== EDDI_DEV_CONS_BLOCK_TYPE =======================*/
/*===========================================================================*/

//#define CONS_BLOCK_REQ_RD       0x0000A000
//#define CONS_BLOCK_ACK_RD       0x0000A004
//#define CONS_BLOCK_REQ_WR       0x0000A008
//#define CONS_BLOCK_ACK_WR       0x0000A00C
/*===========================================================================*/
/*==================== END OF EDDI_DEV_CONS_BLOCK_TYPE ======================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================== TAKTMANAGEMENT              ======================*/
/*===========================================================================*/

#define Clk_Control           0x00010000

/*===========================================================================*/
/*======================== TAKTMANAGEMENT              ======================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================= EDDI_DEV_CYCL_SYNC_CTRL_TYPE ======================*/
/*===========================================================================*/

#define DIV_APP_CLK               0x00011000
#define CYC_CLK_DELAY             0x00011004
#define CYC_CLK_INT_DELAY         0x00011008
#define DIV_CYC_CLK_INT           0x0001100C
#define CLK_CYC_PERIOD            0x00011010
#define CLK_COMMAND               0x00011014
#define CLK_COMP_VALUE1           0x00011018
#define CLK_COMP_VALUE2           0x0001101C
#define CLK_COMP_VALUE3           0x00011020
#define CLK_COMP_VALUE4           0x00011024
#define CLK_COMP_VALUE5           0x00011028
#define CYCL_LENGTH               0x0001102C
//#define MUX_SELECTOR            0x00011030
#define PHASE_CORRECTION          0x00011034
#define DRIFT_CORRECTION          0x00011038

#define CYCL_CNT_ENTITY           0x0001103c
//#define CYCL_FRAME_ID           0x00011040

#define CYCL_COUNT_VALUE          0x00011400
#define CYCL_COUNT_VALUE_47_32    0x00011404

#define CYCL_TIME_VALUE           0x00011408

#define CLK_COUNT_BEGIN_EXT       0x0001140c
//#define APP_CLK_TIMER           0x00011410
#define CLK_STATUS                0x00011414
#define CLK_COUNT_VALUE           0x00011418
#define CLK_COUNT_BEGIN_VALUE     0x0001141c
#define ISRT_TIME_VALUE           0x00011420
//#define MAX_ISRT_LENGTH_VALUE   0x00011424
//#define CLK_COUNT_VALUE_SND     0x00011428
//#define ISRT_OUT_OF_BOUND_VALUE 0x0001142c

#if defined (EDDI_RED_PHASE_SHIFT_ON)
#define IRT_START_TIME_SND_P0     0x0000B000
#define IRT_START_TIME_RCV_P0     0x0000B004
#define IRT_START_TIME_SND_P1     0x0000B008
#define IRT_START_TIME_RCV_P1     0x0000B00C
#define IRT_START_TIME_SND_P2     0x0000B010
#define IRT_START_TIME_RCV_P2     0x0000B014
#define IRT_START_TIME_SND_P3     0x0000B018
#define IRT_START_TIME_RCV_P3     0x0000B01C
#endif
/*===========================================================================*/

/*===========================================================================*/
/*================== END OF EDDI_DEV_CYCL_SYNC_CTRL_TYPE ====================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================== EDDI_DEV_NRT_CTRL_TYPE ===========================*/
/*===========================================================================*/

#define NRT_RCV_MAP_CHA             0x00012000
#define NRT_RCV_MAP_CHB             0x00012004
//#define NRT_AUTO_ENABLE_CHB       0x00012008

#define ARP_TABLE_BASE              0x0001200C
#define ARP_TABLE_LENGTH            0x00012010
#define DCP_TABLE_BASE              0x00012014

#define NRT_ENABLE_CHA0             0x00012400
#define NRT_SND_DESCRIPTOR_CHA0     0x00012404
#define NRT_RCV_DESCRIPTOR_CHA0     0x00012408
#define NRT_ENABLE_CHA1             0x0001240C
#define NRT_SND_DESCRIPTOR_CHA1     0x00012410
#define NRT_RCV_DESCRIPTOR_CHA1     0x00012414
#define NRT_ENABLE_CHB0             0x00012418
#define NRT_SND_DESCRIPTOR_CHB0     0x0001241C
#define NRT_RCV_DESCRIPTOR_CHB0     0x00012420
#define NRT_ENABLE_CHB1             0x00012424
//#define NRT_SND_DESCRIPTOR_CHB1   0x00012428
//#define NRT_RCV_DESCRIPTOR_CHB1   0x0001242C

/*===========================================================================*/

/*===========================================================================*/
/*===================== END OF EDDI_DEV_NRT_CTRL_TYPE =======================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================== EDDI_DEV_IRT_CTRL_TYPE ===========================*/
/*===========================================================================*/

#define IRT_CTRL              0x00013000
#define IMAGE_MODE            0x00013004

//#define APP_DATA_STATUS_MASK  0x00013008
//#define APP_DATA_STATUS_1   0x0001300C
//#define APP_DATA_STATUS_2   0x00013010
//#define APP_DATA_STATUS_3   0x00013014
//#define APP_DATA_STATUS_4   0x00013018
//#define APP_DATA_STATUS_5   0x0001301C
//#define APP_DATA_STATUS_6   0x00013020
//#define APP_DATA_STATUS_7   0x00013024
//#define ASYNC_MODE          0x00013028

#define DMA_IN_DEST_ADR0    0x0001302C
#define DMA_IN_DEST_ADR1    0x00013030
#define DMA_IN_SRC_ADR      0x00013034
#define DMA_IN_LENGTH       0x00013038
#define DMA_OUT_SRC_ADR0    0x0001303C
#define DMA_OUT_SRC_ADR1    0x00013040
#define DMA_OUT_DEST_ADR    0x00013044
#define DMA_OUT_LENGTH      0x00013048

#define IRT_COMP_TIME         0x0001304C

#define DATA_UPDATE           0x00013404
//#define IRT_TRANSF_ERR      0x00013408
//#define DMA_SWITCH_ADR      0x0001340C
#define APP_WATCHDOG        0x00013410

/*===========================================================================*/

/*===========================================================================*/
/*==================== END OF EDDI_DEV_IRT_CTRL_TYPE ========================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*==================== EDDI_PHY_SMI_CONTROL =================================*/
/*===========================================================================*/
// SMI
#define SMI_CONFIGURATION   0x00015008

#define P0_PHY_CMD        0x00015010
#define P0_PHY_STAT       0x00015014
#define P1_PHY_CMD        0x00015018
#define P1_PHY_STAT       0x0001501C
//#define P2_PHY_CMD        0x00015020
//#define P2_PHY_STAT       0x00015024
//#define P3_PHY_CMD        0x00015028
//#define P3_PHY_STAT       0x0001502C

// PHY
#define MD_DATA           0x00015000
#define MD_CA             0x00015004
//#define SMI_CFG         0x00015008
#define LINK_CHANGE       0x0001500C

// Port 0
#define P0_MAC_CTRL       0x00015440
#define P0_ARC_CTRL       0x00015444
#define P0_TRANS_CTRL     0x00015448
//#define P0_TRANS_STATE  0x0001544C
#define P0_RCV_CTRL       0x00015450
//#define P0_RCV_STATE    0x00015454

// Port 1
#define P1_MAC_CTRL       0x000154C0
#define P1_ARC_CTRL       0x000154C4
#define P1_TRANS_CTRL     0x000154C8
//#define P1_TRANS_STATE  0x000154CC
#define P1_RCV_CTRL       0x000154D0
//#define P1_RCV_STATE    0x000154D4

#if defined (EDDI_CFG_ERTEC_400)
// Port 2
#define P2_MAC_CTRL       0x00015540
#define P2_ARC_CTRL       0x00015544
#define P2_TRANS_CTRL     0x00015548
//#define P2_TRANS_STATE  0x0001554C
#define P2_RCV_CTRL       0x00015550
//#define P2_RCV_STATE    0x00015554

// Port 3
#define P3_MAC_CTRL       0x000155C0
#define P3_ARC_CTRL       0x000155C4
#define P3_TRANS_CTRL     0x000155C8
//#define P3_TRANS_STATE  0x000155CC
#define P3_RCV_CTRL       0x000155D0
//#define P3_RCV_STATE    0x000155D4
#endif

/*===========================================================================*/

/*===========================================================================*/
/*=================== END OF EDDI_PHY_SMI_CONTROL ===========================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================= EDDI_DEV_KRAM_CTRL_TYPE ===========================*/
/*===========================================================================*/

#define NRT_FCW_LIMIT         0x00016000
#define NRT_FCW_LIMIT_DOWN    0x00016004
#define NRT_DB_LIMIT          0x00016008
#define NRT_DB_LIMIT_DOWN     0x0001600C
#define HOL_LIMIT_CH          0x00016010
#define HOL_LIMIT_CH_DOWN     0x00016014
#define HOL_LIMIT_PORT        0x00016018
#define HOL_LIMIT_PORT_DOWN   0x0001601C
#define UC_TABLE_BASE         0x00016020
#define UC_TABLE_LENGTH       0x00016024
#define UC_DEFAULT_CTRL       0x00016028
#define MC_TABLE_BASE         0x0001602C
#define MC_TABLE_LENGTH       0x00016030
#define MC_DEFAULT_CTRL       0x00016034
#define UCMC_LFSR_CTRL        0x00016038
#define UC_TABLE_RANGE        0x0001603C

#define VLAN_TABLE_BASE       0x00016040
#define VLAN_TABLE_LENGTH     0x00016044
#define VLAN_LFSR_MASK        0x00016048
#define TIMER_SCOREB_BASE     0x0001604C
#define TIMER_SCOREB_ENTRIES  0x00016050
#define PS_SCOREB_BASE_0      0x00016054
#define PS_SCOREB_BASE_1      0x00016058
//#define TM_LIST_BASE        0x0001605C
//#define TM_LIST_LENGTH      0x00016060
#define FC_MASK               0x00016064
#define HOL_MASK_P0           0x00016068
#define HOL_MASK_P1           0x0001606C

#if defined (EDDI_CFG_ERTEC_400)
#define HOL_MASK_P2           0x00016070
#define HOL_MASK_P3           0x00016074
#endif

#define PRIM_COMMAND          0x00016400
#define SEC_COMMAND           0x00016404
#define CONF_COMMAND          0x00016408
#define PORT_CTRL             0x0001640C
#define FLOW_CTRL             0x00016410
#define PROD_CTRL             0x00016414
#define SS_QUEUE_DISABLE      0x00016418
//#define ACW_LIST_STATE      0x0001641C

#define NRT_FCW_COUNT         0x00016420
#define NRT_DB_COUNT          0x00016424
#define SRT_FCW_COUNT         0x00016428
#define HOL_COUNT_CHA_SS0     0x0001642c
#define HOL_COUNT_CHA_SS1     0x00016430
//#define LIST_STATUS_CHA     0x00016434
#define HOL_COUNT_CHB_SS0     0x00016438
#define HOL_COUNT_CHB_SS1     0x0001643c
//#define LIST_STATUS_CHB     0x00016440
//#define HOL_COUNT_P0        0x00016444
#define LIST_STATUS_P0        0x00016448
//#define HOL_COUNT_P1        0x0001644c
#define LIST_STATUS_P1        0x00016450
//#define HOL_COUNT_P2        0x00016454
//#define LIST_STATUS_P2      0x00016458
//#define HOL_COUNT_P3        0x0001645c
//#define LIST_STATUS_P3      0x00016460

/*===========================================================================*/

/*===========================================================================*/
/*====================== END OF EDDI_KRAM_CONTROL ===========================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*======================== EDDI_DEV_IRQ_CTRL_TYPE ===========================*/
/*===========================================================================*/

#define HP_IRQ_MODE         0x00017000
#define HP_IRQ0_MASK_IRT    0x00017004
#define HP_IRQ0_MASK_NRT    0x00017008
#define HP_IRQ1_MASK_IRT    0x0001700C
#define HP_IRQ1_MASK_NRT    0x00017010
#define HP_IRQ0_IRT         0x00017400
#define HP_IRQ0_NRT         0x00017404
#define HP_IRQ1_IRT         0x00017408
#define HP_IRQ1_NRT         0x0001740C
#define HP_IRQ_ACK_IRT      0x00017410
#define HP_IRQ_ACK_NRT      0x00017414
#define HP_IRR_IRT          0x00017418
#define HP_IRR_NRT          0x0001741C
#define HP_EOI_IRQ0         0x00017420
#define HP_EOI_IRQ1         0x00017424

#define SP_IRQ_MODE         0x00017014
#define SP_IRQ0_MASK_IRT    0x00017018
#define SP_IRQ0_MASK_NRT    0x0001701C
#define SP_IRQ1_MASK_IRT    0x00017020
#define SP_IRQ1_MASK_NRT    0x00017024
#define SP_IRQ0_IRT         0x00017428
#define SP_IRQ0_NRT         0x0001742C
#define SP_IRQ1_IRT         0x00017430
#define SP_IRQ1_NRT         0x00017434
#define SP_IRQ_ACK_IRT      0x00017438
#define SP_IRQ_ACK_NRT      0x0001743C
#define SP_IRR_IRT          0x00017440
#define SP_IRR_NRT          0x00017444
#define SP_EOI_IRQ0         0x00017448
#define SP_EOI_IRQ1         0x0001744C

#define FATAL_IRT_ERROR     0x00017458
#define FATAL_NRT_ERROR     0x0001745C

/*===========================================================================*/

/*===========================================================================*/
/*===================== END OF EDDI_DEV_IRQ_CTRL_TYPE =======================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*===================== EDDI_DEV_IRT_TRACE_CTRL =============================*/
/*===========================================================================*/

#define TRC_BASE_ADR          0x00018000
#define TRC_LENGTH            0x00018004
#define TRC_TRANSFER_LIMIT    0x00018008
#define TRC_COMMAND           0x00018400
#define TRC_MODE              0x00018404
//#define TRC_COUNT_ENTRY     0x00018408
#define TRC_COUNT_FAULT       0x0001840C

/*===========================================================================*/

/*===========================================================================*/
/*================= END OF EDDI_DEV_IRT_TRACE_CTRL ==========================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*========================= EDDI_DEV_GlOB_PARA_TYPE =========================*/
/*===========================================================================*/
#define ETH_TYPE_0          0x00019000
#define ETH_TYPE_1          0x00019004
#define ETH_TYPE_2          0x00019008
#define ETH_TYPE_3          0x0001900C
#define IRT_CYCL_ID         0x00019010
#define IRT_DA_31__0        0x00019014
#define IRT_DA_47_32        0x00019018
#define SA_31__0            0x0001901C
#define SA_47_32            0x00019020
#define IRT_CTRL_BASE_ADR   0x00019024
#define NRT_CTRL_BASE_ADR   0x00019028
#define FREE_CTRL_BASE_ADR  0x0001902C
#define ACW_BASE_ADR        0x00019030
#define ACW_MASK            0x00019034
#define SWI_CTRL            0x00019038
#define MIN_IDLE_TIME       0x0001903C
#define MAC_EMPTY_COUNT     0x00019040
#define NRT_TRANS_CTRL      0x00019044
#define NRT_SAFETY_MARGIN   0x00019048
#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define MONITOR_CTRL        0x0001904C
#endif //end of EDDI_CFG_MONITOR_MODE_INCLUDE
#define STAT_CTRL_BASE_ADR  0x00019050
#define SWITCH_SETUP_MODE   0x00019054
#define FILTER_CTRL_BASE    0x00019058
//#define SWITCH_CT_DELAY   0x0001905C

#define SWI_VERSION         0x00019400
#define SWI_STATUS          0x00019404
//#define ACTIVITY_STATUS   0x00019408

//GenSync
#define TM_LIST_BASE_ADR    0x0001605C
//#define TM_LIST_SIZE      0x00016060

/*===========================================================================*/

/*===========================================================================*/
/*======================= END OF EDDI_DEV_GlOB_PARA_TYPE =====================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*===================== EDDI_DEV_BUS_MONITOR =================================*/
/*===========================================================================*/
/* ToDo --> Trigger-Unit */
//#define SC_BUS_MONITOR_RESET  0x0001A000
//#define SC_BUS_FAULT_ADR    0x0001A004
//#define SC_BUS_FAULT_INFO   0x0001A008
//#define K_BUS_MONITOR_RESET   0x0001A00C
//#define K_BUS_FAULT_ADR     0x0001A010
//#define K_BUS_FAULT_INFO    0x0001A014

/*===========================================================================*/

/*===========================================================================*/
/*==================== END OF EDDI_DEV_BUS_MONITOR ===========================*/
/*===========================================================================*/

/*===========================================================================*/
/*===========================================================================*/

/*===========================================================================*/
/*====================== EDDI_DEV_STAT_CTRL ==================================*/
/*===========================================================================*/

// Port 0
#define STAT_CTRL_PORT0_GBRX        0x0001B000  /* good bytes received              */
#define STAT_CTRL_PORT0_TBRX        0x0001B004  /* total bytes received             */
#define STAT_CTRL_PORT0_BTX         0x0001B008  /* total bytes send               */
#define STAT_CTRL_PORT0_FTX         0x0001B00C  /* total frames send              */
#define STAT_CTRL_PORT0_UFRX        0x0001B010  /* Unicast received               */
#define STAT_CTRL_PORT0_UFRXU       0x0001B014  /* Unknown Unicast received           */
#define STAT_CTRL_PORT0_MFRX        0x0001B018  /* Multicast received             */
#define STAT_CTRL_PORT0_MFRXU       0x0001B01C  /* Unknown Multicast received         */
#define STAT_CTRL_PORT0_BFRX        0x0001B020  /* Broadcast received             */
#define STAT_CTRL_PORT0_UFTX        0x0001B024  /* Unicast send                 */
#define STAT_CTRL_PORT0_MFTX        0x0001B028  /* Multicast send               */
#define STAT_CTRL_PORT0_BFTX        0x0001B02C  /* Broadcast send               */
#define STAT_CTRL_PORT0_F0_64       0x0001B030  /* Frames of size less than 64 bytes      */
#define STAT_CTRL_PORT0_F64         0x0001B034  /* Frames of size 64 byte           */
#define STAT_CTRL_PORT0_F65_127     0x0001B038  /* Frames of size 56 to 127 bytes       */
#define STAT_CTRL_PORT0_F128_255    0x0001B03C  /* Frames of size 128 to 255 bytes        */
#define STAT_CTRL_PORT0_F256_511    0x0001B040  /* Frames of size 256 to 511 bytes        */
#define STAT_CTRL_PORT0_F512_1023   0x0001B044  /* Frames of size 512 to 1023 bytes       */
#define STAT_CTRL_PORT0_F1024_1536  0x0001B048  /* Frames of size 1024 to max size bytes    */
#define STAT_CTRL_PORT0_GFRX        0x0001B04C  /* Good frames received             */
#define STAT_CTRL_PORT0_TFRX        0x0001B050  /* Total frames received            */
#define STAT_CTRL_PORT0_FRX0_64_CRC 0x0001B054  /* Received frames under 64 byte with crc error */
#define STAT_CTRL_PORT0_RXOF        0x0001B058  /* Received frames overflow           */
#define STAT_CTRL_PORT0_RXOSF       0x0001B05C  /* Received total frames > 1536         */
#define STAT_CTRL_PORT0_CRCER       0x0001B060  /* CRC and alignment errors           */
#define STAT_CTRL_PORT0_FDROP       0x0001B064  /* Dropped FCW and DB frames          */
#define STAT_CTRL_PORT0_TXCOL       0x0001B068  /* Transmit collisions              */
#define STAT_CTRL_PORT0_HOLBC       0x0001B06C  /* HOL boundary crossed             */

#define STAT_CTRL_PORT0_CC_DIFF     0x0001B070  /* Summary clock-counter difference     */
#define STAT_CTRL_PORT0_CC_DELAY    0x0001B074  /* Summary clock-counter delay        */
#define STAT_CTRL_PORT0_CC_COUNT    0x0001B078  /* Number of accumulated values       */

// Port 1
#define STAT_CTRL_PORT1_GBRX        0x0001B100  /* good bytes received              */
#define STAT_CTRL_PORT1_TBRX        0x0001B104  /* total bytes received             */
#define STAT_CTRL_PORT1_BTX         0x0001B108  /* total bytes send               */
#define STAT_CTRL_PORT1_FTX         0x0001B10C  /* total frames send              */
#define STAT_CTRL_PORT1_UFRX        0x0001B110  /* Unicast received               */
#define STAT_CTRL_PORT1_UFRXU       0x0001B114  /* Unknown Unicast received           */
#define STAT_CTRL_PORT1_MFRX        0x0001B118  /* Multicast received             */
#define STAT_CTRL_PORT1_MFRXU       0x0001B11C  /* Unknown Multicast received         */
#define STAT_CTRL_PORT1_BFRX        0x0001B120  /* Broadcast received             */
#define STAT_CTRL_PORT1_UFTX        0x0001B124  /* Unicast send                 */
#define STAT_CTRL_PORT1_MFTX        0x0001B128  /* Multicast send               */
#define STAT_CTRL_PORT1_BFTX        0x0001B12C  /* Broadcast send               */
#define STAT_CTRL_PORT1_F0_64       0x0001B130  /* Frames of size less than 64 bytes      */
#define STAT_CTRL_PORT1_F64         0x0001B134  /* Frames of size 64 byte           */
#define STAT_CTRL_PORT1_F65_127     0x0001B138  /* Frames of size 56 to 127 bytes       */
#define STAT_CTRL_PORT1_F128_255    0x0001B13C  /* Frames of size 128 to 255 bytes        */
#define STAT_CTRL_PORT1_F256_511    0x0001B140  /* Frames of size 256 to 511 bytes        */
#define STAT_CTRL_PORT1_F512_1023   0x0001B144  /* Frames of size 512 to 1023 bytes       */
#define STAT_CTRL_PORT1_F1024_1536  0x0001B148  /* Frames of size 1024 to max size bytes    */
#define STAT_CTRL_PORT1_GFRX        0x0001B14C  /* Good frames received             */
#define STAT_CTRL_PORT1_TFRX        0x0001B150  /* Total frames received            */
#define STAT_CTRL_PORT1_FRX0_64_CRC 0x0001B154  /* Received frames under 64 byte with crc error */
#define STAT_CTRL_PORT1_RXOF        0x0001B158  /* Received frames overflow           */
#define STAT_CTRL_PORT1_RXOSF       0x0001B15C  /* Received total frames > 1536         */
#define STAT_CTRL_PORT1_CRCER       0x0001B160  /* CRC and alignment errors           */
#define STAT_CTRL_PORT1_FDROP       0x0001B164  /* Dropped FCW and DB frames          */
#define STAT_CTRL_PORT1_TXCOL       0x0001B168  /* Transmit collisions              */
#define STAT_CTRL_PORT1_HOLBC       0x0001B16C  /* HOL boundary crossed             */

#define STAT_CTRL_PORT1_CC_DIFF     0x0001B170  /* Summary clock-counter difference     */
#define STAT_CTRL_PORT1_CC_DELAY    0x0001B174  /* Summary clock-counter delay        */
#define STAT_CTRL_PORT1_CC_COUNT    0x0001B178  /* Number of accumulated values       */


#if defined (EDDI_CFG_ERTEC_400)
// Port 2
#define STAT_CTRL_PORT2_GBRX        0x0001B200  /* good bytes received              */
#define STAT_CTRL_PORT2_TBRX        0x0001B204  /* total bytes received             */
#define STAT_CTRL_PORT2_BTX         0x0001B208  /* total bytes send               */
#define STAT_CTRL_PORT2_FTX         0x0001B20C  /* total frames send              */
#define STAT_CTRL_PORT2_UFRX        0x0001B210  /* Unicast received               */
#define STAT_CTRL_PORT2_UFRXU       0x0001B214  /* Unknown Unicast received           */
#define STAT_CTRL_PORT2_MFRX        0x0001B218  /* Multicast received             */
#define STAT_CTRL_PORT2_MFRXU       0x0001B21C  /* Unknown Multicast received         */
#define STAT_CTRL_PORT2_BFRX        0x0001B220  /* Broadcast received             */
#define STAT_CTRL_PORT2_UFTX        0x0001B224  /* Unicast send                 */
#define STAT_CTRL_PORT2_MFTX        0x0001B228  /* Multicast send               */
#define STAT_CTRL_PORT2_BFTX        0x0001B22C  /* Broadcast send               */
#define STAT_CTRL_PORT2_F0_64       0x0001B230  /* Frames of size less than 64 bytes      */
#define STAT_CTRL_PORT2_F64         0x0001B234  /* Frames of size 64 byte           */
#define STAT_CTRL_PORT2_F65_127     0x0001B238  /* Frames of size 56 to 127 bytes       */
#define STAT_CTRL_PORT2_F128_255    0x0001B23C  /* Frames of size 128 to 255 bytes        */
#define STAT_CTRL_PORT2_F256_511    0x0001B240  /* Frames of size 256 to 511 bytes        */
#define STAT_CTRL_PORT2_F512_1023   0x0001B244  /* Frames of size 512 to 1023 bytes       */
#define STAT_CTRL_PORT2_F1024_1536  0x0001B248  /* Frames of size 1024 to max size bytes    */
#define STAT_CTRL_PORT2_GFRX        0x0001B24C  /* Good frames received             */
#define STAT_CTRL_PORT2_TFRX        0x0001B250  /* Total frames received            */
#define STAT_CTRL_PORT2_FRX0_64_CRC 0x0001B254  /* Received frames under 64 byte with crc error */
#define STAT_CTRL_PORT2_RXOF        0x0001B258  /* Received frames overflow           */
#define STAT_CTRL_PORT2_RXOSF       0x0001B25C  /* Received total frames > 1536         */
#define STAT_CTRL_PORT2_CRCER       0x0001B260  /* CRC and alignment errors           */
#define STAT_CTRL_PORT2_FDROP       0x0001B264  /* Dropped FCW and DB frames          */
#define STAT_CTRL_PORT2_TXCOL       0x0001B268  /* Transmit collisions              */
#define STAT_CTRL_PORT2_HOLBC       0x0001B26C  /* HOL boundary crossed             */

#define STAT_CTRL_PORT2_CC_DIFF     0x0001B270  /* Summary clock-counter difference     */
#define STAT_CTRL_PORT2_CC_DELAY    0x0001B274  /* Summary clock-counter delay        */
#define STAT_CTRL_PORT2_CC_COUNT    0x0001B278  /* Number of accumulated values       */

// Port 3
#define STAT_CTRL_PORT3_GBRX        0x0001B300  /* good bytes received              */
#define STAT_CTRL_PORT3_TBRX        0x0001B304  /* total bytes received             */
#define STAT_CTRL_PORT3_BTX         0x0001B308  /* total bytes send               */
#define STAT_CTRL_PORT3_FTX         0x0001B30C  /* total frames send              */
#define STAT_CTRL_PORT3_UFRX        0x0001B310  /* Unicast received               */
#define STAT_CTRL_PORT3_UFRXU       0x0001B314  /* Unknown Unicast received           */
#define STAT_CTRL_PORT3_MFRX        0x0001B318  /* Multicast received             */
#define STAT_CTRL_PORT3_MFRXU       0x0001B31C  /* Unknown Multicast received         */
#define STAT_CTRL_PORT3_BFRX        0x0001B320  /* Broadcast received             */
#define STAT_CTRL_PORT3_UFTX        0x0001B324  /* Unicast send                 */
#define STAT_CTRL_PORT3_MFTX        0x0001B328  /* Multicast send               */
#define STAT_CTRL_PORT3_BFTX        0x0001B32C  /* Broadcast send               */
#define STAT_CTRL_PORT3_F0_64       0x0001B330  /* Frames of size less than 64 bytes      */
#define STAT_CTRL_PORT3_F64         0x0001B334  /* Frames of size 64 byte           */
#define STAT_CTRL_PORT3_F65_127     0x0001B338  /* Frames of size 56 to 127 bytes       */
#define STAT_CTRL_PORT3_F128_255    0x0001B33C  /* Frames of size 128 to 255 bytes        */
#define STAT_CTRL_PORT3_F256_511    0x0001B340  /* Frames of size 256 to 511 bytes        */
#define STAT_CTRL_PORT3_F512_1023   0x0001B344  /* Frames of size 512 to 1023 bytes       */
#define STAT_CTRL_PORT3_F1024_1536  0x0001B348  /* Frames of size 1024 to max size bytes    */
#define STAT_CTRL_PORT3_GFRX        0x0001B34C  /* Good frames received             */
#define STAT_CTRL_PORT3_TFRX        0x0001B350  /* Total frames received            */
#define STAT_CTRL_PORT3_FRX0_64_CRC 0x0001B354  /* Received frames under 64 byte with crc error */
#define STAT_CTRL_PORT3_RXOF        0x0001B358  /* Received frames overflow           */
#define STAT_CTRL_PORT3_RXOSF       0x0001B35C  /* Received total frames > 1536         */
#define STAT_CTRL_PORT3_CRCER       0x0001B360  /* CRC and alignment errors           */
#define STAT_CTRL_PORT3_FDROP       0x0001B364  /* Dropped FCW and DB frames          */
#define STAT_CTRL_PORT3_TXCOL       0x0001B368  /* Transmit collisions              */
#define STAT_CTRL_PORT3_HOLBC       0x0001B36C  /* HOL boundary crossed             */

#define STAT_CTRL_PORT3_CC_DIFF     0x0001B370  /* Summary clock-counter difference      */
#define STAT_CTRL_PORT3_CC_DELAY    0x0001B374  /* Summary clock-counter delay       */
#define STAT_CTRL_PORT3_CC_COUNT    0x0001B378  /* Number of accumulated values        */

#endif // EDDI_CFG_ERTEC_400

/*===========================================================================*/
/*================== END OF EDDI_DEV_STAT_CTRL ==============================*/
/*===========================================================================*/

#endif //EDDI_CFG_REV6

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_ADR_R6_H


/*****************************************************************************/
/*  end of file eddi_ser_adr_r6.h                                            */
/*****************************************************************************/

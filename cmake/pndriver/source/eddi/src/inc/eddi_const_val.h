#ifndef EDDI_CONST_VAL_H        //reinclude-protection
#define EDDI_CONST_VAL_H

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
/*  F i l e               &F: eddi_const_val.h                          :F&  */
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
/*  10.09.2010  AH    Initial Version                                        */
/*****************************************************************************/

/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/
/*****************************************************/
/******* CRT *****************************************/
/*****************************************************/
// constant "ProviderCyclePhaseMaxByteCnt" for CycleBaseFactor = 32/64/128/256/512/1024
#define EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_32                6250
// division factor for calculation of "ProviderCyclePhaseMaxByteCnt" for CycleBaseFactor = 1/2/4/8/16
#define EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_DIVISION_FACTOR   160
// Multiplier for ProviderCyclePhaseMaxByteCnt. Set to 1 for normal usage, set to 0 to allow overplanning!
#define EDDI_CRT_PROVIDER_CYCLEPHASE_MAX_BYTE_CNT_MULTIPLIER        1
#define EDDI_CRT_PROVIDER_HEADER_OVERHEAD                           28  //Header bytes on top of IO-bytes

#define EDDI_CRT_PROVIDER_DEFAULT_PER_1MS                           64UL  //default nr of providers per ms

/*****************************************************/
/******* Values for resource limitation resulting  ***/
/******* from SPH Resource Connections             ***/
/*****************************************************/
#define EDDI_RESLIM_MAX_NR_PROV_SOC_2P                              512

/*****************************************************/
/******* CYC *****************************************/
/*****************************************************/
/* CycleBaseFactor                                                          */
/* Base factor for calculation of base cycle time.                          */
/* For calculation of Cycle-Time: 31.25*2^n                                 */
/* ERTEC400 only supports 0x0004..0x8000 due to SWSB                        */
/* ERTEC200 only supports 0x0004..0x0080 due to non existing fragmentation  */
#define EDDI_CRT_CYCLE_BASE_FACTOR_CRIT 0x0008  // critical cbf below which no RTC12 prov/cons may be added
#if defined (EDDI_CFG_REV5)
#define EDDI_CRT_CYCLE_BASE_FACTOR_MIN  0x0004  // 4 * 31,25us => 125us
#define EDDI_CRT_CYCLE_BASE_FACTOR_MAX  0x8000  // (1 <<15)  2^15 = 0x8000 => 1024ms
#elif defined (EDDI_CFG_REV6)
#define EDDI_CRT_CYCLE_BASE_FACTOR_MIN  0x0004  // 4  * 31,25us
#define EDDI_CRT_CYCLE_BASE_FACTOR_MAX  0x0080  // 128 * 31,25us => 4ms (due to RCW-Tree)
#else
#if defined (EDDI_CFG_SMALL_SCF_ON)
#define EDDI_CRT_CYCLE_BASE_FACTOR_MIN  0x0001  // 1  * 31,25us
#else
#define EDDI_CRT_CYCLE_BASE_FACTOR_MIN  0x0004  // 4  * 31,25us
#endif //defined (EDDI_CFG_SMALL_SCF_ON)
#define EDDI_CRT_CYCLE_BASE_FACTOR_MAX  0x0080  // 128 * 31,25us => 4ms (due to RCW-Tree)
#endif //defined (EDDI_CFG_REVx)


#define EDDI_CRT_CYCLE_BASE_FACTOR_125  4UL     // 4   * 31,25 us   = 125 us
#define EDDI_CRT_CYCLE_BASE_FACTOR_188  6UL     // 6   * 31,25 us   = 187.5 us
#define EDDI_CRT_CYCLE_BASE_FACTOR_250  8UL     // 8   * 31,25 us   = 250 us



#define EDDI_CRT_CYCLE_BASE_FACTOR_1MS  32UL    // SCF for 1ms

// Total Propagationdelay for EXT_PLL-Signal:
// ERTEC --> RS485 --> Cable --> RS485 --> ERTEC
// Delay = CableLength * 5ns/m + 90.25 ns +- 13.75ns
#define EXT_PLL_IN_INTERNAL_DELAY_10NS   9 // internal Time Delay for EXT_PLL-Signal in Ticks (== 90ns)

//Limitation of addable consumers for small SCF
#define EDDI_CRT_MAX_CONS_SMALL_SCF_INIT    {0/*0*/,64/*1*/,112/*2*/,112/*3*/,192/*4*/,192/*5*/,192/*6*/,192/*7*/}

/*****************************************************/
/******* CONV ***************************************/
/*****************************************************/
#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7) 
#define EDDI_CONV_MIN_IDLE_TIME_10MBIT                                  0x0196U
#define EDDI_CONV_MIN_IDLE_TIME_100MBIT                                 0x001DU
#define EDDI_CONV_MAC_EMPTY_CNT_10MBIT                                  0x10U
#define EDDI_CONV_MAC_EMPTY_CNT_100MBIT                                 0x10U
#else
#define EDDI_CONV_MIN_IDLE_TIME_10MBIT_FPGA                             0x00C5U
#define EDDI_CONV_MIN_IDLE_TIME_100MBIT_FPGA                            0x0008U
#define EDDI_CONV_MAC_EMPTY_CNT_10MBIT_FPGA                             0x08U
#define EDDI_CONV_MAC_EMPTY_CNT_100MBIT_FPGA                            0x08U
#endif

#define EDDI_CONV_DEF_VLAN_ADRESS                                       0x0000
#define EDDI_CONV_DEF_VLAN_PRIO                                         0
#define EDDI_CONV_NRT_SAFETY_MARGIN_10MBIT                              0x0640U
#define EDDI_CONV_NRT_SAFETY_MARGIN_100MBIT                             0x00A5U

#define EDDI_CONV_UC_MC_LFSR_MASK_R5R6                                  0x4A4
#define EDDI_CONV_UC_MC_LFSR_MASK_R7                                    0x4A9
#define EDDI_CONV_UCMC_TABLE_MAX_ENTRY                                  1024   
#define EDDI_CONV_UCMC_TABLE_RANGE                                      7
#define EDDI_CONV_MC_MAX_ENTRIES_PER_SUBTABLE                           15
#define EDDI_CONV_MC_MAX_SUBTABLES                                      32
#define EDDI_CONV_TWO_PORT_CT_MODE                                      0
#define EDDI_CONV_SRT_REALTIME_COUNTER                                  0
#define EDDI_CONV_HOL_LIMIT_CH_DOWN                                     0xFFFF
#define EDDI_CONV_HOL_LIMIT_PORT_DOWN                                   0xFFFF
#define EDDI_CONV_AGE_POLL_TIME                                         30
#define EDDI_CONV_CYCLE_BASE_FACTOR                                     32
#define EDDI_CONV_TOS_DSCP                                              0

//CONV Part II
#define EDDI_CONV_FORWARDER_CNT                                         128 
#define EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT                          64

#define EDDI_CONV_CONSUMER_FRAME_ID_BASE_CLASS1                         0xC000
#define EDDI_CONV_CONSUMER_FRAME_ID_BASE_CLASS2                         0x8000

#define EDDI_CONV_IF_A_0_TX_CNT_DSCR                                    40        
#define EDDI_CONV_IF_A_0_RX_CNT_DSCR                                    40                                                              

#define EDDI_CONV_IF_B_0_TX_CNT_DSCR                                    40        
#define EDDI_CONV_IF_B_0_RX_CNT_DSCR                                    40                                                                

#define EDDI_CONV_SYS_STAT_FDB_CNT_ENTRY                                0


#define EDDI_NRT_FRAME_HEADER_SIZE                       (12+8+4) /* IFG + Preamble + FCS */  
#define EDDI_NRT_FILLSIZE_OVERALL                        3084
#define EDDI_NRT_FILLSIZE_PRIO_LOW                       3084
#define EDDI_NRT_FILLSIZE_PRIO_MEDIUM                    1542
#define EDDI_NRT_FILLSIZE_PRIO_HIGH                      1542
#define EDDI_NRT_FILLSIZE_Prio_HSYNC_1                   3084
#define EDDI_NRT_FILLSIZE_Prio_HSYNC_2                   3084
#define EDDI_NRT_FILLSIZE_PRIO_MGMT_LOW                  1542
#define EDDI_NRT_FILLSIZE_PRIO_MGMT_HIGH                  280

#define EDDI_NRT_FILLSIZE_UNRESTRICTED                   0xFFFFFFFFUL

#define EDDI_NRT_FILLSIZE_UNRESTRICTED_OVERALL           EDDI_NRT_FILLSIZE_UNRESTRICTED
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_LOW          EDDI_NRT_FILLSIZE_UNRESTRICTED
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MEDIUM       EDDI_NRT_FILLSIZE_UNRESTRICTED
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_HIGH         EDDI_NRT_FILLSIZE_UNRESTRICTED
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_1      EDDI_NRT_FILLSIZE_Prio_HSYNC_1
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_Prio_HSYNC_2      EDDI_NRT_FILLSIZE_Prio_HSYNC_2
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MGMT_LOW     EDDI_NRT_FILLSIZE_UNRESTRICTED
#define EDDI_NRT_FILLSIZE_UNRESTRICTED_PRIO_MGMT_HIGH    EDDI_NRT_FILLSIZE_UNRESTRICTED
        
//#define  EDDI_HSYNC_MEAN_NRT_FRAMES     21UL                            // HSYNC  BestCase: 2;  WorstCase: 21
//#define  EDDI_HSYNC_MEAN_NRT_FCWS       21UL                            // HSYNC  BestCase: 11;  WorstCase: 21
//#define  EDDI_HSYNC_MEAN_NRT_DBS        21UL                            // HSYNC  BestCase: SWI_MAX_DB_NR_FULL_NRT_FRAME ~ 13;  WorstCase: 21

/* HSYNC - Application values */
#define  EDDI_HSYNC_APPLICATION_DB_NRT_FRAMES       21UL    // HSYNC  BestCase:  2;  WorstCase: 21;  Forwarder:  2 ( 1);
#define  EDDI_HSYNC_APPLICATION_DBS_PER_NRT_FRAME    2UL    // HSYNC  BestCase: 13;  WorstCase:  2;  Forwarder: 13 (13);
#define  EDDI_HSYNC_APPLICATION_FCW_NRT_FRAMES      21UL    // HSYNC  BestCase: 11;  WorstCase: 21;  Forwarder:  2 ( 2);
#define  EDDI_HSYNC_APPLICATION_FCWS_PER_NRT_FRAME   1UL    // HSYNC  BestCase:  1;  WorstCase:  1;  Forwarder:  1 ( 1);
#define  EDDI_HSYNC_APPLICATION_NR_OF_PORTS          2UL    // HSYNC  Receiver:  2;  Receiver:   2;  Forwarder:  1 ( 1);
#define  EDDI_HSYNC_APPLICATION_SAMPLE_FACTOR        2UL    // HSYNC             2;              2;              1 ( 1);


/* HSYNC - Forwarder values */
#define  EDDI_HSYNC_FORWARDER_DB_NRT_FRAMES          1UL    // HSYNC  BestCase:  2;  WorstCase: 21;  Forwarder:  2 ( 1);
#define  EDDI_HSYNC_FORWARDER_DBS_PER_NRT_FRAME     13UL    // HSYNC  BestCase: 13;  WorstCase:  2;  Forwarder: 13 (13);
#define  EDDI_HSYNC_FORWARDER_FCW_NRT_FRAMES         2UL    // HSYNC  BestCase: 11;  WorstCase: 21;  Forwarder:  2 ( 2);
#define  EDDI_HSYNC_FORWARDER_FCWS_PER_NRT_FRAME     1UL    // HSYNC  BestCase:  1;  WorstCase:  1;  Forwarder:  1 ( 1);
#define  EDDI_HSYNC_FORWARDER_NR_OF_PORTS            1UL    // HSYNC  Receiver:  2;  Receiver:   2;  Forwarder:  1 ( 1);
#define  EDDI_HSYNC_FORWARDER_SAMPLE_FACTOR          1UL    // HSYNC             2;              2;              1 ( 1);


#if defined (EDDI_CFG_REV5)
//ERTEC 400
#define EDDI_CONV_REV5_PROVFRAMEFACTOR                                  1
#define EDDI_CONV_REV5_MAXPORT                                          4
#define EDDI_CONV_REV5_MAXBRIDGEDELAY                                   1920
#define EDDI_CONV_ERTEC400                                              0
#define EDDI_CONV_MIN_FRAMESIZE_CONS12                                  EDD_CSRT_DATALEN_MIN    //no partialdata access rtc12
#define EDDI_CONV_MIN_FRAMESIZE_CONS3                                   EDD_CSRT_DATALEN_MIN    //no partialdata access rtc3
#define EDDI_CONV_MIN_FRAMESIZE_PROV                                    EDD_CSRT_DATALEN_MIN    //no autopadding
#define EDDI_CONV_CONS_PART_DATALEN_ERR_SIZE                            0
#elif defined (EDDI_CFG_REV6)
//ERTEC 200
#define EDDI_CONV_REV6_PROVFRAMEFACTOR                                  3
#define EDDI_CONV_REV6_MAXPORT                                          2
#define EDDI_CONV_REV6_MAXBRIDGEDELAY                                   2920
#define EDDI_CONV_ERTEC200                                              1
#define EDDI_CONV_MIN_FRAMESIZE_CONS12                                  1                       //partialdata access rtc12
#define EDDI_CONV_MIN_FRAMESIZE_CONS3                                   EDD_CSRT_DATALEN_MIN    //no partialdata access rtc3
#define EDDI_CONV_MIN_FRAMESIZE_PROV                                    EDD_CSRT_DATALEN_MIN    //no autopadding
// Bugfix for IRTE issue:
// In case of receiving valid frames, IRTE checks the Partial_DataLen first and generates an IRTFatalListError
#define EDDI_CONV_CONS_PART_DATALEN_ERR_SIZE                            4
#elif defined (EDDI_CFG_REV7)
//SOC1/2
#define EDDI_CONV_REV7_MAXPORT                                          3
#define EDDI_CONV_REV7_SOC2_MAXPORT                                     2
#define EDDI_CONV_REV7_MAXBRIDGEDELAY                                   2920
#define EDDI_CONV_REV7_PROVFRAMEFACTOR                                  1
#define EDDI_CONV_ERTECSOC1                                             2
#define EDDI_CONV_ERTECSOC2                                             3
#define EDDI_CONV_MIN_FRAMESIZE_CONS12                                  1                       //partialdata access rtc12
#define EDDI_CONV_MIN_FRAMESIZE_CONS3                                   1                       //partialdata access rtc3
#define EDDI_CONV_MIN_FRAMESIZE_PROV                                    1                       //autopadding
// Bugfix for IRTE issue:
// In case of receiving valid frames, IRTE checks the Partial_DataLen first and generates an IRTFatalListError
#define EDDI_CONV_CONS_PART_DATALEN_ERR_SIZE                            4
#endif                                                           

#define EDDI_CONV_PHY_BCM_5221_MII                                      0
#define EDDI_CONV_PHY_BCM_5221_RMII                                     1
#define EDDI_CONV_PHY_BCM_5221_MC_MII                                   2
#define EDDI_CONV_PHY_BCM_5221_MC_RMII                                  3
#define EDDI_CONV_PHY_NEC_ERTEC200_INT                                  4
#define EDDI_CONV_PHY_NSC_DP83849I_MII                                  5
#define EDDI_CONV_PHY_NSC_DP83849I_RMII                                 6
#define EDDI_CONV_PHY_TI_TLK111_MII                                     7
#define EDDI_CONV_PHY_TI_TLK111_RMII                                    8
#define EDDI_CONV_PHY_NULL                                              9
#define EDDI_CONV_PHY_MAX                                               10

#define EDDI_CONV_NR_OF_RT_MC_PROVIDER                                  0
#define EDDI_CONV_NR_OF_RT_MC_CONSUMER                                  0

#define EDDI_CONV_ERTEC_X00_PERIPHERY                                   0

#define EDDI_CONV_MAX_RCV_FRAME_SEND_TO_USER                            0
#define EDDI_CONV_MAX_RCV_FRAME_TO_LOOK_UP                              0
#define EDDI_CONV_MAX_RCV_FRAGMENT_FRAME_TO_COPY                        0

#define EDDI_CONV_TRIGGER_DEADLINE_IN_10NS                              0
#define EDDI_CONV_TRIGGER_MODE                                          0
#define EDDI_CONV_INVALID_MAC_ADDR                                      0xFF

#define EDDI_CONV_ERTEC_X00_SCRB                                        0

#define EDDI_CONV_MRP_CAPABILITY_ROLE_CLIENT                            0x0001
#define EDDI_CONV_MRP_CAPABILITY_ROLE_MGR                               0x0002

#define EDDI_CONV_KRAM_RESERVE                                          0x100 
 
#define  EDDI_CNST_VALUES_FCMASK_PRIO  { LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE }
#define  EDDI_CNST_VALUES_HOLMASK_PRIO { LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_FALSE, LSA_TRUE,  LSA_TRUE,  LSA_FALSE }

#if defined (EDDI_CFG_REV5)
#define  EDDI_CONST_VALUES_INIT   /*ERTEC400*/  {EDDI_SWI_FDB_PRIO_DEFAULT/*XRTPrio*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Copper*/, EDD_LINK_100MBIT_FULL/*LinkSpeedMode_FO*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Other*/, LSA_TRUE/*EnCutThrough*/, LSA_TRUE/*EnSRTPriority*/, \
                                                 LSA_TRUE/*MCLearningEn*/, LSA_FALSE/*IngressFilter*/, LSA_FALSE/*EnCheckSA*/, LSA_FALSE/*EnISRT*/, LSA_FALSE/*EnMonitorCyclic*/, \
                                                 LSA_FALSE/*EnMonitorAcyclic*/, EDDI_CONV_DEF_VLAN_ADRESS/*DefVLANAdress*/, EDDI_CONV_DEF_VLAN_PRIO/*DefVLANPrio*/, EDD_PORT_STATE_FORWARDING/*SpanningTreeState*/, \
                                                 LSA_FALSE/*DisableLinkChangePHYSMI*/, EDDI_CONV_MIN_IDLE_TIME_10MBIT/*MinIdleTime10Mbit*/, EDDI_CONV_MIN_IDLE_TIME_100MBIT/*MinIdleTime100Mbit*/, \
                                                 EDDI_CONV_MAC_EMPTY_CNT_10MBIT/*MacEmptyCnt10Mbit*/, EDDI_CONV_MAC_EMPTY_CNT_100MBIT/*MacEmptyCnt100Mbit*/, EDDI_CONV_NRT_SAFETY_MARGIN_10MBIT/*NRTSafetyMargin10Mbit*/, \
                                                 EDDI_CONV_NRT_SAFETY_MARGIN_100MBIT/*NRTSafetyMargin100Mbit*/, EDDI_CNST_VALUES_FCMASK_PRIO/*FCMaskPrio[EDDI_NRT_MAX_PRIO]*/, \
                                                 EDDI_CNST_VALUES_HOLMASK_PRIO/*HOLMaskPrio[EDDI_NRT_MAX_PRIO]*/, LSA_FALSE/*UCDefaultControlCHA*/, LSA_FALSE/*UCDefaultControlCHB*/, \
                                                 LSA_TRUE/*UCDefaultControlDestPort*/, LSA_FALSE/*MCDefaultControlCHA*/, LSA_FALSE/*MCDefaultControlCHB*/, LSA_TRUE/*MCDefaultControlDestPort*/, \
                                                 EDDI_CONV_UCMC_TABLE_MAX_ENTRY/*UCMCTableMaxEntry*/, EDDI_CONV_UC_MC_LFSR_MASK_R5R6/*UCMCLFSRMask*/, EDDI_CONV_UCMC_TABLE_RANGE/*UCMCTableRange*/, \
                                                 0/*MC_MaxEntriesPerSubTable*/, 0/*MC_MaxSubTable*/, \
                                                 EDDI_CONV_TWO_PORT_CT_MODE/*TwoPortCTMode*/, EDDI_CONV_SRT_REALTIME_COUNTER/*SRTRelTimeCounter*/, EDDI_CONV_HOL_LIMIT_CH_DOWN/*HOLLimitCHDown*/, \
                                                 EDDI_CONV_HOL_LIMIT_PORT_DOWN/*HOLLimitPortDown*/, EDDI_CONV_AGE_POLL_TIME/*AgePollTime*/, \
                                                 /*EDD_CFG_CSRT_MAX_PROVIDER_GROUP  *ProviderGroupCnt*,*/ EDDI_CONV_CYCLE_BASE_FACTOR/*CycleBaseFactor*/, EDDI_CONV_TOS_DSCP/*TOSDSCP*/, EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT/*ProviderCyclePhaseMaxCnt*/, \
                                                 /*LSA_BOOL  PhyPowerOff*/ LSA_TRUE, /* LSA_UINT8  IsPulled */ EDD_PORTMODULE_IS_PLUGGED }
#elif defined EDDI_CFG_REV6
#define  EDDI_CONST_VALUES_INIT   /*ERTEC200*/  {EDDI_SWI_FDB_PRIO_DEFAULT/*XRTPrio*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Copper*/, EDD_LINK_100MBIT_FULL/*LinkSpeedMode_FO*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Other*/, LSA_TRUE/*EnCutThrough*/, LSA_TRUE/*EnSRTPriority*/, \
                                                 LSA_TRUE/*MCLearningEn*/, LSA_FALSE/*IngressFilter*/, LSA_FALSE/*EnCheckSA*/, LSA_FALSE/*EnISRT*/, LSA_FALSE/*EnMonitorCyclic*/, \
                                                 LSA_FALSE/*EnMonitorAcyclic*/, EDDI_CONV_DEF_VLAN_ADRESS/*DefVLANAdress*/, EDDI_CONV_DEF_VLAN_PRIO/*DefVLANPrio*/, EDD_PORT_STATE_FORWARDING/*SpanningTreeState*/, \
                                                 LSA_FALSE/*DisableLinkChangePHYSMI*/, EDDI_CONV_MIN_IDLE_TIME_10MBIT/*MinIdleTime10Mbit*/, EDDI_CONV_MIN_IDLE_TIME_100MBIT/*MinIdleTime100Mbit*/, \
                                                 EDDI_CONV_MAC_EMPTY_CNT_10MBIT/*MacEmptyCnt10Mbit*/, EDDI_CONV_MAC_EMPTY_CNT_100MBIT/*MacEmptyCnt100Mbit*/, EDDI_CONV_NRT_SAFETY_MARGIN_10MBIT/*NRTSafetyMargin10Mbit*/, \
                                                 EDDI_CONV_NRT_SAFETY_MARGIN_100MBIT/*NRTSafetyMargin100Mbit*/, EDDI_CNST_VALUES_FCMASK_PRIO/*FCMaskPrio[EDDI_NRT_MAX_PRIO]*/, \
                                                 EDDI_CNST_VALUES_HOLMASK_PRIO/*HOLMaskPrio[EDDI_NRT_MAX_PRIO]*/, LSA_FALSE/*UCDefaultControlCHA*/, LSA_FALSE/*UCDefaultControlCHB*/, \
                                                 LSA_TRUE/*UCDefaultControlDestPort*/, LSA_FALSE/*MCDefaultControlCHA*/, LSA_FALSE/*MCDefaultControlCHB*/, LSA_TRUE/*MCDefaultControlDestPort*/, \
                                                 EDDI_CONV_UCMC_TABLE_MAX_ENTRY/*UCMCTableMaxEntry*/, EDDI_CONV_UC_MC_LFSR_MASK_R5R6/*UCMCLFSRMask*/, EDDI_CONV_UCMC_TABLE_RANGE/*UCMCTableRange*/, \
                                                 0/*MC_MaxEntriesPerSubTable*/, 0/*MC_MaxSubTable*/, \
                                                 EDDI_CONV_TWO_PORT_CT_MODE/*TwoPortCTMode*/, EDDI_CONV_SRT_REALTIME_COUNTER/*SRTRelTimeCounter*/, EDDI_CONV_HOL_LIMIT_CH_DOWN/*HOLLimitCHDown*/, \
                                                 EDDI_CONV_HOL_LIMIT_PORT_DOWN/*HOLLimitPortDown*/, EDDI_CONV_AGE_POLL_TIME/*AgePollTime*/, \
                                                 /*EDD_CFG_CSRT_MAX_PROVIDER_GROUP  *ProviderGroupCnt*,*/ EDDI_CONV_CYCLE_BASE_FACTOR/*CycleBaseFactor*/, EDDI_CONV_TOS_DSCP/*TOSDSCP*/, EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT/*ProviderCyclePhaseMaxCnt*/, \
                                                 /*LSA_BOOL  PhyPowerOff*/ LSA_TRUE, /* LSA_UINT8  IsPulled */ EDD_PORTMODULE_IS_PLUGGED }
#elif defined EDDI_CFG_REV7
#define  EDDI_CONST_VALUES_INIT   /*ERTECSOC*/  {EDDI_SWI_FDB_PRIO_DEFAULT/*XRTPrio*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Copper*/, EDD_LINK_100MBIT_FULL/*LinkSpeedMode_FO*/, EDD_LINK_AUTONEG/*LinkSpeedMode_Other*/, LSA_TRUE/*EnCutThrough*/, LSA_TRUE/*EnSRTPriority*/, \
                                                 LSA_TRUE/*MCLearningEn*/, LSA_FALSE/*IngressFilter*/, LSA_FALSE/*EnCheckSA*/, LSA_FALSE/*EnISRT*/, LSA_FALSE/*EnMonitorCyclic*/, \
                                                 LSA_FALSE/*EnMonitorAcyclic*/, EDDI_CONV_DEF_VLAN_ADRESS/*DefVLANAdress*/, EDDI_CONV_DEF_VLAN_PRIO/*DefVLANPrio*/, EDD_PORT_STATE_FORWARDING/*SpanningTreeState*/, \
                                                 LSA_FALSE/*DisableLinkChangePHYSMI*/, EDDI_CONV_MIN_IDLE_TIME_10MBIT/*MinIdleTime10Mbit*/, EDDI_CONV_MIN_IDLE_TIME_100MBIT/*MinIdleTime100Mbit*/, \
                                                 EDDI_CONV_MAC_EMPTY_CNT_10MBIT/*MacEmptyCnt10Mbit*/, EDDI_CONV_MAC_EMPTY_CNT_100MBIT/*MacEmptyCnt100Mbit*/, EDDI_CONV_NRT_SAFETY_MARGIN_10MBIT/*NRTSafetyMargin10Mbit*/, \
                                                 EDDI_CONV_NRT_SAFETY_MARGIN_100MBIT/*NRTSafetyMargin100Mbit*/, EDDI_CNST_VALUES_FCMASK_PRIO/*FCMaskPrio[EDDI_NRT_MAX_PRIO]*/, \
                                                 EDDI_CNST_VALUES_HOLMASK_PRIO/*HOLMaskPrio[EDDI_NRT_MAX_PRIO]*/, LSA_FALSE/*UCDefaultControlCHA*/, LSA_FALSE/*UCDefaultControlCHB*/, \
                                                 LSA_TRUE/*UCDefaultControlDestPort*/, LSA_FALSE/*MCDefaultControlCHA*/, LSA_FALSE/*MCDefaultControlCHB*/, LSA_TRUE/*MCDefaultControlDestPort*/, \
                                                 EDDI_CONV_UCMC_TABLE_MAX_ENTRY/*UCMCTableMaxEntry*/, EDDI_CONV_UC_MC_LFSR_MASK_R7/*UCMCLFSRMask*/, EDDI_CONV_UCMC_TABLE_RANGE/*UCMCTableRange*/, \
                                                 EDDI_CONV_MC_MAX_ENTRIES_PER_SUBTABLE/*MC_MaxEntriesPerSubTable*/, EDDI_CONV_MC_MAX_SUBTABLES/*MC_MaxSubTable*/, \
                                                 EDDI_CONV_TWO_PORT_CT_MODE/*TwoPortCTMode*/, EDDI_CONV_SRT_REALTIME_COUNTER/*SRTRelTimeCounter*/, EDDI_CONV_HOL_LIMIT_CH_DOWN/*HOLLimitCHDown*/, \
                                                 EDDI_CONV_HOL_LIMIT_PORT_DOWN/*HOLLimitPortDown*/, EDDI_CONV_AGE_POLL_TIME/*AgePollTime*/, \
                                                 /* EDD_CFG_CSRT_MAX_PROVIDER_GROUP  *ProviderGroupCnt*,*/ EDDI_CONV_CYCLE_BASE_FACTOR/*CycleBaseFactor*/, EDDI_CONV_TOS_DSCP/*TOSDSCP*/, EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT/*ProviderCyclePhaseMaxCnt*/, \
                                                 /*LSA_BOOL  PhyPowerOff*/ LSA_TRUE, /* LSA_UINT8  IsPulled */ EDD_PORTMODULE_IS_PLUGGED }
#else 
#define  EDDI_CONST_VALUES_INIT   /*ERTECFPGA*/ {EDDI_SWI_FDB_PRIO_DEFAULT/*XRTPrio*/, EDD_LINK_AUTONEG/*LinkSpeedMode*/, LSA_TRUE/*EnCutThrough*/, LSA_TRUE/*EnSRTPriority*/, \
                                                 LSA_TRUE/*MCLearningEn*/, LSA_FALSE/*IngressFilter*/, LSA_FALSE/*EnCheckSA*/, LSA_FALSE/*EnISRT*/, LSA_FALSE/*EnMonitorCyclic*/, \
                                                 LSA_FALSE/*EnMonitorAcyclic*/, EDDI_CONV_DEF_VLAN_ADRESS/*DefVLANAdress*/, EDDI_CONV_DEF_VLAN_PRIO/*DefVLANPrio*/, EDD_PORT_STATE_FORWARDING/*SpanningTreeState*/, \
                                                 LSA_FALSE/*DisableLinkChangePHYSMI*/, EDDI_CONV_MIN_IDLE_TIME_10MBIT_FPGA/*MinIdleTime10Mbit*/, EDDI_CONV_MIN_IDLE_TIME_100MBIT_FPGA/*MinIdleTime100Mbit*/, \
                                                 EDDI_CONV_MAC_EMPTY_CNT_10MBIT_FPGA/*MacEmptyCnt10Mbit*/, EDDI_CONV_MAC_EMPTY_CNT_100MBIT_FPGA/*MacEmptyCnt100Mbit*/, \
                                                 EDDI_CONV_NRT_SAFETY_MARGIN_10MBIT/*NRTSafetyMargin10Mbit*/, EDDI_CONV_NRT_SAFETY_MARGIN_100MBIT/*NRTSafetyMargin100Mbit*/, \
                                                 EDDI_CNST_VALUES_FCMASK_PRIO/*FCMaskPrio[EDDI_NRT_MAX_PRIO]*/, EDDI_CNST_VALUES_HOLMASK_PRIO/*HOLMaskPrio[EDDI_NRT_MAX_PRIO]*/, \
                                                 LSA_FALSE/*UCDefaultControlCHA*/, LSA_FALSE/*UCDefaultControlCHB*/, LSA_TRUE/*UCDefaultControlDestPort*/, \
                                                 LSA_FALSE/*MCDefaultControlCHA*/, LSA_FALSE/*MCDefaultControlCHB*/, LSA_TRUE/*MCDefaultControlDestPort*/, 1024/*UCMCTableMaxEntry*/, \
                                                 EDDI_CONV_UC_MC_LFSR_MASK_R5R6/*UCMCLFSRMask*/, EDDI_CONV_UCMC_TABLE_RANGE/*UCMCTableRange*/, 
                                                 0/*MC_MaxEntriesPerSubTable*/, 0/*MC_MaxSubTable*/, EDDI_CONV_TWO_PORT_CT_MODE/*TwoPortCTMode*/, \
                                                 EDDI_CONV_SRT_REALTIME_COUNTER/*SRTRelTimeCounter*/, EDDI_CONV_HOL_LIMIT_CH_DOWN/*HOLLimitCHDown*/, EDDI_CONV_HOL_LIMIT_PORT_DOWN/*HOLLimitPortDown*/, \
                                                 EDDI_CONV_AGE_POLL_TIME/*AgePollTime*/, /* EDD_CFG_CSRT_MAX_PROVIDER_GROUP  *ProviderGroupCnt*,*/ \
                                                 EDDI_CONV_CYCLE_BASE_FACTOR/*CycleBaseFactor*/, EDDI_CONV_TOS_DSCP/*TOSDSCP*/, EDDI_CONV_PROVIDER_CYCLE_PHASE_MAX_CNT/*ProviderCyclePhaseMaxCnt*/, \
                                                 /*LSA_BOOL  PhyPowerOff*/ LSA_TRUE, /* LSA_UINT8  IsPulled */ EDD_PORTMODULE_IS_PLUGGED }
#endif

/*===========================================================================*/
/*                               Structs                                     */
/*===========================================================================*/
#ifndef EDDI_NRT_MAX_PRIO
    #define EDDI_NRT_MAX_PRIO  10
#endif

typedef struct _EDDI_INI_PARAMS_FIXED_TYPE
{
    LSA_UINT16      XRTPrio;                                
    LSA_UINT16      LinkSpeedMode_Copper;
    LSA_UINT16      LinkSpeedMode_FO;
    LSA_UINT16      LinkSpeedMode_Other;
    LSA_BOOL        EnCutThrough;  
    LSA_BOOL        EnSRTPriority;  
    LSA_BOOL        MCLearningEn;  
    LSA_BOOL        IngressFilter;          
    LSA_BOOL        EnCheckSA;     
    LSA_BOOL        EnISRT;      
    LSA_BOOL        EnMonitorCyclic;
    LSA_BOOL        EnMonitorAcyclic;
    LSA_UINT16      DefVLANAdress;                          
    LSA_UINT8       DefVLANPrio;                         
    LSA_UINT16      SpanningTreeState; 
    LSA_BOOL        DisableLinkChangePHYSMI;
    LSA_UINT16      MinIdleTime10Mbit;
    LSA_UINT16      MinIdleTime100Mbit;
    LSA_UINT8       MacEmptyCnt10Mbit;
    LSA_UINT8       MacEmptyCnt100Mbit;
    LSA_UINT16      NRTSafetyMargin10Mbit;                  
    LSA_UINT16      NRTSafetyMargin100Mbit;                 
    LSA_UINT32      FCMaskPrio[EDDI_NRT_MAX_PRIO];
    LSA_BOOL        HOLMaskPrio[EDDI_NRT_MAX_PRIO];
    LSA_BOOL        UCDefaultControlCHA;                    
    LSA_BOOL        UCDefaultControlCHB;                    
    LSA_BOOL        UCDefaultControlDestPort;               
    LSA_BOOL        MCDefaultControlCHA;                    
    LSA_BOOL        MCDefaultControlCHB;                    
    LSA_BOOL        MCDefaultControlDestPort;               
    LSA_UINT16      UCMCTableMaxEntry;                      
    LSA_UINT16      UCMCLFSRMask;                           
    LSA_UINT16      UCMCTableRange;                         
    LSA_UINT16      MC_MaxEntriesPerSubTable;
    LSA_UINT32      MC_MaxSubTable;
    LSA_BOOL        TwoPortCTMode;                          
    LSA_UINT32      iSRTRealTime;                         
    LSA_UINT32      HOLLimitCHDown;                         
    LSA_UINT16      HOLLimitPortDown;                       
    LSA_UINT16      AgePollTime;                            
    /*LSA_UINT16      ProviderGroupCnt;*/
    LSA_UINT16      CycleBaseFactor;                        
    LSA_UINT8       TOSDSCP;
    LSA_UINT32      ProviderCyclePhaseMaxCnt;
    LSA_BOOL        PhyPowerOff;        //PHY startup behaviour
    LSA_UINT8       IsPulled;
   
} EDDI_INI_PARAMS_FIXED_TYPE;

typedef struct _EDDI_INI_PARAMS_FIXED_TYPE  EDD_UPPER_MEM_ATTR  *  EDDI_INI_PARAMS_FIXED_PTR_TYPE;

//MIB2 support constants

#define EDDI_MIB2_SUPPORTED_COUNTERS_PORT   EDD_MIB_SUPPORT_INOCTETS            \
                                          | EDD_MIB_SUPPORT_INUCASTPKTS         \
                                          | EDD_MIB_SUPPORT_INNUCASTPKTS        \
                                          | EDD_MIB_SUPPORT_INDISCARDS          \
                                          | EDD_MIB_SUPPORT_INERRORS            \
                                          | EDD_MIB_SUPPORT_INUNKNOWNPROTOS     \
                                          | EDD_MIB_SUPPORT_OUTOCTETS           \
                                          | EDD_MIB_SUPPORT_OUTUCASTPKTS        \
                                          | EDD_MIB_SUPPORT_OUTNUCASTPKTS       \
                                          | EDD_MIB_SUPPORT_OUTERRORS           \
                                          | EDD_MIB_SUPPORT_INMULTICASTPKTS     \
                                          | EDD_MIB_SUPPORT_INBROADCASTPKTS     \
                                          | EDD_MIB_SUPPORT_OUTMULTICASTPKTS    \
                                          | EDD_MIB_SUPPORT_OUTBROADCASTPKTS

#define EDDI_MIB2_SUPPORTED_COUNTERS_IF     EDD_MIB_SUPPORT_INUCASTPKTS         \
                                          | EDD_MIB_SUPPORT_OUTUCASTPKTS              

#define EDDI_RECORD_SUPPORTED_COUNTERS_PORT         EDD_RECORD_SUPPORT_INOCTETS         \
                                                |   EDD_RECORD_SUPPORT_OUTOCTETS        \
                                                |   EDD_RECORD_SUPPORT_INDISCARDS       \
                                                |   EDD_RECORD_SUPPORT_INERRORS         \
                                                |   EDD_RECORD_SUPPORT_OUTERRORS

#define EDDI_RECORD_SUPPORTED_COUNTERS_IF           0


#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CONST_VAL_H


/*****************************************************************************/
/*  end of file eddi_const_val.h                                             */
/*****************************************************************************/

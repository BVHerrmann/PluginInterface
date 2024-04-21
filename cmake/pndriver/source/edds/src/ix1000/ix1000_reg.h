#ifndef _IX1000_REG_H                  /* ----- reinclude-protection ----- */
#define _IX1000_REG_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
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
/*  C o m p o n e n t     &C: EDDS (EthernetDeviceDriver for Std. MAC)  :C&  */
/*                                                                           */
/*  F i l e               &F: template_reg.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  TEMPLATE register description.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  26.09.11    MA    initial version.                                       */
#endif
/*****************************************************************************/

/* --------------------------------------------------------------------------*/
/* IX1000 register access                                                    */
/* --------------------------------------------------------------------------*/
#define IX1000_GETBIT(bit,value)                       EDDS_GET_BIT(value, bit)
#define IX1000_GETBITS(begin,end,value)                EDDS_GET_BITS(value, begin, end)
#define IX1000_BIT(bit,value)                          EDDS_WRITE_BIT_FOR_MASK(bit, value)
#define IX1000_BITS(begin,end,value)                   EDDS_WRITE_BITS_FOR_MASK(begin, end, value)
#define IX1000_SETBIT(lvalue,bit)                      EDDS_SET_BIT(lvalue, bit)
#define IX1000_SETBITS(lvalue,begin,end,value)         EDDS_WRITE_BITS(lvalue, begin, end, value)
#define IX1000_CLRBIT(lvalue,bit)                      EDDS_CLR_BIT(lvalue, bit)

#define IX1000_base                                    (pHandle->pParams->pRegBaseAddr_BAR0)
#define IX1000_reg32(addr)                             ( ((volatile LSA_UINT32*)IX1000_base)[addr/4] )

/* --------------------------------------------------------------------------*/
/* Host Communication                                                        */
/* --------------------------------------------------------------------------*/

/* - Receive Descriptors RDES0-RDES3 bits -----------------------------------*/
#define IX1000_RDES0_OWN           31
//#define IX1000_RDES0_AFM           30
#define IX1000_RDES0_FL_e          29
#define IX1000_RDES0_FL_s          16  
//#define IX1000_RDES0_ES            15
//#define IX1000_RDES0_DE            14
//#define IX1000_RDES0_SAF           13
//#define IX1000_RDES0_LE            12
//#define IX1000_RDES0_OE            11
//#define IX1000_RDES0_VLAN          10
#define IX1000_RDES0_FS            9
#define IX1000_RDES0_LS            8
//#define IX1000_RDES0_IPHCE_TSV     7
//#define IX1000_RDES0_LCO           6
//#define IX1000_RDES0_FT            5
//#define IX1000_RDES0_RWT           4
//#define IX1000_RDES0_RE            3    
//#define IX1000_RDES0_DRE           2
//#define IX1000_RDES0_CE            1
//#define IX1000_RDES0_PCE_ESA       0  
 
//#define IX1000_RDES1_DIC           31
//#define IX1000_RDES1_RBS2_e        28
//#define IX1000_RDES1_RBS2_s        16
//#define IX1000_RDES1_RER           15
#define IX1000_RDES1_RCH           14
#define IX1000_RDES1_RBS1_e        12
#define IX1000_RDES1_RBS1_s        0
  
//#define IX1000_RDES2_RBP1_RTLS_e   31
//#define IX1000_RDES2_RBP1_RTLS_s   0 

//#define IX1000_RDES3_RBP2_RTSH_e   31
//#define IX1000_RDES3_RBP2_RTSH_s   0

/* Enhanced Receive Descriptors RDES4-RDES7 bits */  
//#define IX1000_RDES4_PV            13 
//#define IX1000_RDES4_PFT           12
//#define IX1000_RDES4_PMT_e         11
//#define IX1000_RDES4_PMT_s         8
//#define IX1000_RDES4_IPV6PR        7
//#define IX1000_RDES4_IPV4PR        6
//#define IX1000_RDES4_PBCB          5
//#define IX1000_RDES4_IPPE          4
//#define IX1000_RDES4_IPHE          3
//#define IX1000_RDES4_IPPT_e        2
//#define IX1000_RDES4_IPPT_s        0
  
//RDES5: reserved 

//#define IX1000_RDES6_TRSL_e        31
//#define IX1000_RDES6_TRSL_s        0  
  
//#define IX1000_RDES7_TRSH_e        31
//#define IX1000_RDES7_TRSH_s        0    
  
/* - Transmit Descriptors TDES0-TDES3 bits -----------------------------------*/  
#define IX1000_TDES0_OWN           31
//#define IX1000_TDES0_IC            30
#define IX1000_TDES0_LS            29
#define IX1000_TDES0_FS            28
//#define IX1000_TDES0_DC            27
//#define IX1000_TDES0_DP            26
//#define IX1000_TDES0_TTSE          25
//#define IX1000_TDES0_CRCR          24
//#define IX1000_TDES0_CIC_e         23
//#define IX1000_TDES0_CIC_s         22
//#define IX1000_TDES0_TER           21
#define IX1000_TDES0_TCH           20
//#define IX1000_TDES0_TTSS          17
//#define IX1000_TDES0_ES            15
//#define IX1000_TDES0_JT            14
//#define IX1000_TDES0_FF            13
//#define IX1000_TDES0_IPE           12
//#define IX1000_TDES0_LCA           11
//#define IX1000_TDES0_NC            10
//#define IX1000_TDES0_LCO           9
//#define IX1000_TDES0_EC            8
//#define IX1000_TDES0_VF            7
//#define IX1000_TDES0_CC_e          6
//#define IX1000_TDES0_CC_s          3
//#define IX1000_TDES0_ED            2
//#define IX1000_TDES0_UF            1
//#define IX1000_TDES0_DB            0
  
//#define IX1000_TDES1_TBS2_e        28
//#define IX1000_TDES1_TBS2_s        16  
#define IX1000_TDES1_TBS1_e        12
#define IX1000_TDES1_TBS1_s        0
    
//#define IX1000_TDES2_TBP1_TBP_TTSL_e 31
//#define IX1000_TDES2_TBP1_TBP_TTSL_s 0
  
//#define IX1000_TDES3_TBP1_TBP_TTSL_e 31
//#define IX1000_TDES3_TBP1_TBP_TTSL_s 0  
  
/* Enhanced Receive Descriptors TDES4-TDES7 bits */
//TDES4: reserved 
//TDES5: reserved 
//#define IX1000_TDES6_TTSL_e        31
//#define IX1000_TDES6_TTSL_s        0

//#define IX1000_TDES7_TTSL_e        31
//#define IX1000_TDES7_TTSL_s        0  

/* --------------------------------------------------------------------------*/
/* PCI Configuration Registers                                               */
/* --------------------------------------------------------------------------*/

//not needed  

/* --------------------------------------------------------------------------*/
/* IX1000 register addresses                                                */
/* --------------------------------------------------------------------------*/
#define IX1000_GMAC_REG_0                0x0 
//#define IX1000_GMAC_REG_0_SARC_e         30
//#define IX1000_GMAC_REG_0_SARC_s         28
//#define IX1000_GMAC_REG_0_PE2K           27
//#define IX1000_GMAC_REG_0_WD             23
//#define IX1000_GMAC_REG_0_JD             22
//#define IX1000_GMAC_REG_0_JE             20
//#define IX1000_GMAC_REG_0_IFG_e          19
//#define IX1000_GMAC_REG_0_IFG_s          17
//#define IX1000_GMAC_REG_0_DCRS           16
//#define IX1000_GMAC_REG_0_GMII           15  
#define IX1000_GMAC_REG_0_FES            14
//#define IX1000_GMAC_REG_0_DO             13
//#define IX1000_GMAC_REG_0_LM             12
#define IX1000_GMAC_REG_0_DM             11
//#define IX1000_GMAC_REG_0_IPC            10
//#define IX1000_GMAC_REG_0_DR             9
//#define IX1000_GMAC_REG_0_ACS            7
//#define IX1000_GMAC_REG_0_BL_e           6
//#define IX1000_GMAC_REG_0_BL_s           5
//#define IX1000_GMAC_REG_0_DC             4
#define IX1000_GMAC_REG_0_TE             3
#define IX1000_GMAC_REG_0_RE             2
//#define IX1000_GMAC_REG_0_PRELEN_e       1
//#define IX1000_GMAC_REG_0_PRELEN_s       0
  
#define IX1000_GMAC_REG_1                0x4 
//#define IX1000_GMAC_REG_1_RA             31
//#define IX1000_GMAC_REG_1_VTFE           16
//#define IX1000_GMAC_REG_1_HPF            10
//#define IX1000_GMAC_REG_1_SAF            9
//#define IX1000_GMAC_REG_1_SAIF           8
//#define IX1000_GMAC_REG_1_PCF_e          7
//#define IX1000_GMAC_REG_1_PCF_s          6
//#define IX1000_GMAC_REG_1_DBF            5
//#define IX1000_GMAC_REG_1_PM             4
//#define IX1000_GMAC_REG_1_DAIF           3  
#define IX1000_GMAC_REG_1_HMC            2
//#define IX1000_GMAC_REG_1_HUC            1
//#define IX1000_GMAC_REG_1_PR             0
  
#define IX1000_GMAC_REG_2                0x8  
//#define IX1000_GMAC_REG_2_HTH_e          31
//#define IX1000_GMAC_REG_2_HTH_s          0
  
#define IX1000_GMAC_REG_3                0xC  
//#define IX1000_GMAC_REG_3_HTL_e          31
//#define IX1000_GMAC_REG_2_HTL_s          0
  
#define IX1000_GMAC_REG_4                0x10
#define IX1000_GMAC_REG_4_PA_e           15
#define IX1000_GMAC_REG_4_PA_s           11
#define IX1000_GMAC_REG_4_GR_e           10
#define IX1000_GMAC_REG_4_GR_s           6
#define IX1000_GMAC_REG_4_CR_e           5
#define IX1000_GMAC_REG_4_CR_s           2
#define IX1000_GMAC_REG_4_GW             1
#define IX1000_GMAC_REG_4_GB             0  
  
#define IX1000_GMAC_REG_5                0x14
//#define IX1000_GMAC_REG_5_GD_e           15
//#define IX1000_GMAC_REG_5_GD_s           0
  
//#define IX1000_GMAC_REG_6                0x18
//#define IX1000_GMAC_REG_6_PT_e           31
//#define IX1000_GMAC_REG_6_PT_s           16
//#define IX1000_GMAC_REG_6_DZPQ           7
//#define IX1000_GMAC_REG_6_PLT_e          5
//#define IX1000_GMAC_REG_6_PLT_s          4
//#define IX1000_GMAC_REG_6_UP             3
//#define IX1000_GMAC_REG_6_RFE            2
//#define IX1000_GMAC_REG_6_TFE            1
//#define IX1000_GMAC_REG_6_FCV_BPA        0  
  
//#define IX1000_GMAC_REG_7                0x1C
//#define IX1000_GMAC_REG_7_VTHM           19
//#define IX1000_GMAC_REG_7_ESVL           18
//#define IX1000_GMAC_REG_7_VTIM           17
//#define IX1000_GMAC_REG_7_ETV            16
//#define IX1000_GMAC_REG_7_VL_e           15
//#define IX1000_GMAC_REG_7_VL_s           0  
  
//#define IX1000_GMAC_REG_8                0x20  //Version Register
//#define IX1000_GMAC_REG_8_USERVER_e      15
//#define IX1000_GMAC_REG_8_USERVER_s      8
//#define IX1000_GMAC_REG_8_SNPSVER_e      7
//#define IX1000_GMAC_REG_8_SNPSVER_s      0

//#define IX1000_GMAC_REG_9                0x24  //Debug Register
//#define IX1000_GMAC_REG_9_TXFSTS         24
//#define IX1000_GMAC_REG_9_TWCSTS         22
//#define IX1000_GMAC_REG_9_TRCSTS_e       21
//#define IX1000_GMAC_REG_9_TRCSTS_s       20
//#define IX1000_GMAC_REG_9_TXPAUSED       19
//#define IX1000_GMAC_REG_9_TFCSTS_e       18
//#define IX1000_GMAC_REG_9_TFCSTS_s       17
//#define IX1000_GMAC_REG_9_TPESTS         16
//#define IX1000_GMAC_REG_9_RXFSTS_e       9           
//#define IX1000_GMAC_REG_9_RXFSTS_s       8
//#define IX1000_GMAC_REG_9_RRCSTS_e       6           
//#define IX1000_GMAC_REG_9_RRCSTS_s       5
//#define IX1000_GMAC_REG_9_RWCSTS         4
//#define IX1000_GMAC_REG_9_RFCFCSTS_e     2
//#define IX1000_GMAC_REG_9_RFCFCSTS_s     1
//#define IX1000_GMAC_REG_9_RPESTS         0   
  
//#define IX1000_GMAC_REG_14               0x38  //Interrupt Register
//#define IX1000_GMAC_REG_14_TSIS          9
//#define IX1000_GMAC_REG_14_MMCRXIPIS     7
//#define IX1000_GMAC_REG_14_MMCTXIS       6
//#define IX1000_GMAC_REG_14_MMCRXIS       5
//#define IX1000_GMAC_REG_14_MMCIS         4
  
#define IX1000_GMAC_REG_15               0x3C  //Interrupt Mask Register
//#define IX1000_GMAC_REG_15_TSIM          9
  
#define IX1000_GMAC_REG_16               0x40  //MAC Address0 High Register
#define IX1000_GMAC_REG_16_AE            31
//#define IX1000_GMAC_REG_16_ADDRHI_e      15
//#define IX1000_GMAC_REG_16_ADDRHI_s      0
  
#define IX1000_GMAC_REG_17               0x44  //MAC Address0 Low Register  
//#define IX1000_GMAC_REG_17_ADDRLO_e      31
//#define IX1000_GMAC_REG_17_ADDRLO_s      0  

#define IX1000_GMAC_REG_64               0x100
    #define IX1000_GMAC_REG_64_RSTONRD      2

//#define IX1000_MMC_INTR_RX                0x104
//#define IX1000_MMC_INTR_TX                0x108
#define IX1000_MMC_INTR_MASK_RX           0x10C
#define IX1000_MMC_INTR_MASK_TX           0x110
//#define IX1000_TXOCTETCOUNT_GB            0x114
//#define IX1000_TXFRAMECOUNT_GB            0x118
//#define IX1000_TXBROADCASTFRAMES_G        0x11C
//#define IX1000_TXMULTICASTFRAMES_G        0x120
//#define IX1000_TX64OCTETS_GB              0x124
//#define IX1000_TX65TO127OCTETS_GB         0x128
//#define IX1000_TX128TO255OCTETS_GB        0x12C
//#define IX1000_TX256TO511OCTETS_GB        0x130
//#define IX1000_TX512TO1023OCTETS_GB       0x134
//#define IX1000_TX1024TOMAXOCTETS_GB       0x138
//#define IX1000_TXUNICASTFRAMES_GB         0x13C
//#define IX1000_TXMULTICASTFRAMES_GB       0x140
//#define IX1000_TXBROADCASTFRAMES_GB       0x144
#define IX1000_TXUNDERFLOWERROR           0x148
//#define IX1000_TXSINGLECOL_G              0x14C
//#define IX1000_TXMULTICOL_G               0x150
//#define IX1000_TXDEFERRED                 0x154
#define IX1000_TXLATECOL                  0x158
#define IX1000_TXEXESSCOL                 0x15C
#define IX1000_TXCARRIERERROR             0x160
//#define IX1000_TXOCTETCOUNT_G             0x164
//#define IX1000_TXFRAMECOUNT_G             0x168
#define IX1000_TXEXCESSDEF                0x16C
//#define IX1000_TXPAUSEFRAMES              0x170
//#define IX1000_TXVLANFRAMES_G             0x174
//#define IX1000_TXOVERSIZE_G               0x178
//#define IX1000_RXFRAMECOUNT_GB            0x180
//#define IX1000_RXOCTETCOUNT_GB            0x184
//#define IX1000_RXOCTETCOUNT_G             0x188
//#define IX1000_RXBROADCASTFRAMES_G        0x18C
//#define IX1000_RXMULTICASTFRAMES_G        0x190
#define IX1000_RXCRCERROR                 0x194
#define IX1000_RXALIGNMENTERROR           0x198
#define IX1000_RXRUNTERROR                0x19C
#define IX1000_RXJABBERERROR              0x1A0
//#define IX1000_RXUNDERSIZE_G              0x1A4
//#define IX1000_RXOVERSIZE_G               0x1A8
//#define IX1000_RX64OCTETS_GB              0x1AC
//#define IX1000_RX65TO127OCTETS_GB         0x1B0
//#define IX1000_RX128TO255OCTETS_GB        0x1B4
//#define IX1000_RX256TO511OCTETS_GB        0x1B8
//#define IX1000_RX512TO1023OCTETS_GB       0x1BC
//#define IX1000_RX1024TOMAXOCTETS_GB       0x1C0
//#define IX1000_RXUNICASTFRAMES_G          0x1C4
#define IX1000_RXLENGTHERROR              0x1C8
#define IX1000_RXOUTOFRANGETYPE           0x1CC
//#define IX1000_RXPAUSEFRAMES              0x1D0
#define IX1000_RXFIFOOVERFLOW             0x1D4
//#define IX1000_RRXFIFOOVERFLOW            0x1D8
#define IX1000_RXWATCHDOGERROR            0x1DC
//#define IX1000_RXRCVERROR                 0x1E0
//#define IX1000_RXCTRLFRAMES_G             0x1E4
#define IX1000_MMC_IPC_INTR_MASK_RX       0x200
//#define IX1000_MMC_IPC_INTR_RX            0x208
//#define IX1000_RXIPV4_GD_FRMS             0x210
//#define IX1000_RXIPV4_HDRERR_FRMS         0x214
//#define IX1000_RXIPV4_NOPAY_FRMS          0x218
//#define IX1000_RXIPV4_FRAG_FRMS           0x21C
//#define IX1000_RXIPV4_UDSBL_FRMS          0x220
//#define IX1000_RXIPV6_GD_FRMS             0x224
//#define IX1000_RXIPV6_HDRERR_FRMS         0x228
//#define IX1000_RXIPV6_NOPAY_FRMS          0x22C
//#define IX1000_RXUDP_GD_FRMS              0x230
//#define IX1000_RXUDP_ERR_FRMS             0x234
//#define IX1000_RXTCP_GD_FRMS              0x238
//#define IX1000_RXTCP_ERR_FRMS             0x23C
//#define IX1000_RXICMP_GD_FRMS             0x240
//#define IX1000_RXICMP_ERR_FRMS            0x244
//#define IX1000_RXIPV4_GD_OCTETS           0x250
//#define IX1000_RXIPV4_HDRERR_OCTETS       0x254
//#define IX1000_RXIPV4_NOPAY_OCTETS        0x258
//#define IX1000_RXIPV4_FRAG_OCTETS         0x25C
//#define IX1000_RXIPV4_UDSBL_OCTETS        0x260
//#define IX1000_RXIPV6_GD_OCTETS           0x264
//#define IX1000_RXIPV6_HDRERR_OCTETS       0x268
//#define IX1000_RXIPV6_NOPAY_OCTETS        0x26C
//#define IX1000_RXUDP_GD_OCTETS            0x270
//#define IX1000_RXUDP_ERR_OCTETS           0x274
//#define IX1000_RXTCP_GD_OCTETS            0x278
//#define IX1000_RXTCP_ERR_OCTETS           0x27C
//#define IX1000_RXICMP_GD_OCTETS           0x280
//#define IX1000_RXICMP_ERR_OCTETS          0x284
      
#define IX1000_DMA_REG_0                0x1000  //Bus Mode Register
//#define IX1000_DMA_REG_0_RIX            31
//#define IX1000_DMA_REG_0_PRWG           30
//#define IX1000_DMA_REG_0_PRWG_e         29
//#define IX1000_DMA_REG_0_PRWG_s         28
//#define IX1000_DMA_REG_0_TXPR           27
//#define IX1000_DMA_REG_0_MB             26
//#define IX1000_DMA_REG_0_AAL            25
//#define IX1000_DMA_REG_0_PBL8X          24
//#define IX1000_DMA_REG_0_USP            23
//#define IX1000_DMA_REG_0_RPBL_e         22
//#define IX1000_DMA_REG_0_RPBL_s         17
#define IX1000_DMA_REG_0_FB             16
#define IX1000_DMA_REG_0_PR_e           15
#define IX1000_DMA_REG_0_PR_s           14
#define IX1000_DMA_REG_0_PBL_e          13
#define IX1000_DMA_REG_0_PBL_s          8
//#define IX1000_DMA_REG_0_ATDS           7
//#define IX1000_DMA_REG_0_DSL_e          6
//#define IX1000_DMA_REG_0_DSL_s          2
//#define IX1000_DMA_REG_0_DA             1
#define IX1000_DMA_REG_0_SWR            0
  
#define IX1000_DMA_REG_1                0x1004  //Transmit Poll Demand Register
//#define IX1000_DMA_REG_TPD_e            31
//#define IX1000_DMA_REG_TPD_s            0
  
#define IX1000_DMA_REG_2                0x1008  //Receive Poll Demand Register
//#define IX1000_DMA_REG_2_RPD_e          31
//#define IX1000_DMA_REG_2_RPD_s          0
  
#define IX1000_DMA_REG_3                0x100C  //Receive Descriptor List Address Register
//#define IX1000_DMA_REG_3_RDESLA_32BIT_e 31
//#define IX1000_DMA_REG_3_RDESLA_32BIT_s 0 // BIT0 and 1 are reserved

#define IX1000_DMA_REG_4                0x1010  //Transmit Descriptor List Address Register
//#define IX1000_DMA_REG_4_TDESLA_32BIT_e 31
//#define IX1000_DMA_REG_4_TDESLA_32BIT_s 0  // BIT0 and 1 are reserved
  
//#define IX1000_DMA_REG_5                0x1014  //Status Register
//#define IX1000_DMA_REG_5_TTI            29
//#define IX1000_DMA_REG_5_GMI            27
//#define IX1000_DMA_REG_5_EB_e           25
//#define IX1000_DMA_REG_5_EB_s           23
//#define IX1000_DMA_REG_5_TS_e           22
//#define IX1000_DMA_REG_5_TS_s           20
//#define IX1000_DMA_REG_5_RS_e           19
//#define IX1000_DMA_REG_5_RS_s           17
//#define IX1000_DMA_REG_5_NIS            16
//#define IX1000_DMA_REG_5_AIS            15
//#define IX1000_DMA_REG_5_ERI            14
//#define IX1000_DMA_REG_5_FBI            13
//#define IX1000_DMA_REG_5_ETI            10
//#define IX1000_DMA_REG_5_RWT            9
//#define IX1000_DMA_REG_5_RPS            8
//#define IX1000_DMA_REG_5_RU             7
//#define IX1000_DMA_REG_5_RI             6
//#define IX1000_DMA_REG_5_UNF            5
//#define IX1000_DMA_REG_5_OVF            4
//#define IX1000_DMA_REG_5_TJT            3
//#define IX1000_DMA_REG_5_TU             2
//#define IX1000_DMA_REG_5_TPS            1
//#define IX1000_DMA_REG_5_TI             0
  
#define IX1000_DMA_REG_6                0x1018  //Operation Mode Register  
//#define IX1000_DMA_REG_6_DT             26
#define IX1000_DMA_REG_6_RSF            25
#define IX1000_DMA_REG_6_DFF            24
//#define IX1000_DMA_REG_6_RFA_2          23
//#define IX1000_DMA_REG_6_RFD_2          22
#define IX1000_DMA_REG_6_TSF            21
#define IX1000_DMA_REG_6_FTF            20
//#define IX1000_DMA_REG_6_TTC_e          16
//#define IX1000_DMA_REG_6_TTC_s          14
#define IX1000_DMA_REG_6_ST             13
//#define IX1000_DMA_REG_6_RFD_e          12
//#define IX1000_DMA_REG_6_RFD_s          11
//#define IX1000_DMA_REG_6_RFA_e          10
//#define IX1000_DMA_REG_6_RFA_s          9
//#define IX1000_DMA_REG_6_EFC            8
//#define IX1000_DMA_REG_6_FEF            7
//#define IX1000_DMA_REG_6_FUF            6
//#define IX1000_DMA_REG_6_RTC_e          4
//#define IX1000_DMA_REG_6_RTC_s          3
//#define IX1000_DMA_REG_6_OSF            2
#define IX1000_DMA_REG_6_SR             1

#define IX1000_DMA_REG_7                0x101C //Interrupt Enable Register
//#define IX1000_DMA_REG_7_NIE            16
//#define IX1000_DMA_REG_7_AIE            15
//#define IX1000_DMA_REG_7_ERE            14
//#define IX1000_DMA_REG_7_FBE            13
//#define IX1000_DMA_REG_7_ETE            10
//#define IX1000_DMA_REG_7_RWE            9
//#define IX1000_DMA_REG_7_RSE            8
//#define IX1000_DMA_REG_7_RUE            7
//#define IX1000_DMA_REG_7_RIE            6
//#define IX1000_DMA_REG_7_UNE            5
//#define IX1000_DMA_REG_7_OVE            4
//#define IX1000_DMA_REG_7_TJE            3
//#define IX1000_DMA_REG_7_TUE            2
//#define IX1000_DMA_REG_7_TSE            1
//#define IX1000_DMA_REG_7_TIE            0
  
//#define IX1000_DMA_REG_8                0x1020 //Missed Frame and Buffer Overflow Counter Register
//#define IX1000_DMA_REG_8_OVFCNTOVF      28
//#define IX1000_DMA_REG_8_OVFFRMCNT_e    27
//#define IX1000_DMA_REG_8_OVFFRMCNT_s    17
//#define IX1000_DMA_REG_8_MISCNTOVF      16
//#define IX1000_DMA_REG_8_MISFRMCNT_e    15
//#define IX1000_DMA_REG_8_MISFRMCNT_s    0
  
//#define IX1000_DMA_REG_9                0x1024 //Receive Watchdog Timer Register
//#define IX1000_DMA_REG_9_RIWT_e         7
//#define IX1000_DMA_REG_9_RIWT_s         0
  
//#define IX1000_DMA_REG_11               0x102C //AHB Status Register
//#define IX1000_DMA_REG_11_AXWHSTS       0
  
//#define IX1000_DMA_REG_18               0x1048  //Current Host Transmit Descriptor Register
//#define IX1000_DMA_REG_18_CURTDESAPTR_e 31
//#define IX1000_DMA_REG_18_CURTDESAPTR_s 0 
  
//#define IX1000_DMA_REG_19               0x104C  //Current Host Receive Descriptor Register
//#define IX1000_DMA_REG_19_CURRDESAPTR_e 31
//#define IX1000_DMA_REG_19_CURRDESAPTR_s 0  
  
//#define IX1000_DMA_REG_20               0x1050  //Current Host Transmit Buffer Address Register
//#define IX1000_DMA_REG_20_CURTBUFAPTR_e 31
//#define IX1000_DMA_REG_20_CURTBUFAPTR_s 0
  
//#define IX1000_DMA_REG_21               0x1054  //Current Host Receive Buffer Address Register
//#define IX1000_DMA_REG_21_CURRBUFAPTR_e 31
//#define IX1000_DMA_REG_21_CURRBUFAPTR_s 0
  
//#define IX1000_DMA_REG_22               0x1058  //HW Feature Register
//#define IX1000_DMA_REG_22_ACTPHYIF_e    30
//#define IX1000_DMA_REG_22_ACTPHYIF_s    28
//#define IX1000_DMA_REG_22_SAVLANINS     27
//#define IX1000_DMA_REG_22_FLEXIPPSEN    26
//#define IX1000_DMA_REG_22_INTTSEN       25
//#define IX1000_DMA_REG_22_ENHDESSEL     24
//#define IX1000_DMA_REG_22_TXCHCNT_e     23
//#define IX1000_DMA_REG_22_TXCHCNT_s     22
//#define IX1000_DMA_REG_22_RXCHCNT_e     21
//#define IX1000_DMA_REG_22_RXCHCNT_s     20
//#define IX1000_DMA_REG_22_RXFIFOSIZE    19
//#define IX1000_DMA_REG_22_RXTYP2COE     18
//#define IX1000_DMA_REG_22_RXTYP1COE     17
//#define IX1000_DMA_REG_22_TXCOESEL      16
//#define IX1000_DMA_REG_22_AVSEL         15
//#define IX1000_DMA_REG_22_EEESEL        14
//#define IX1000_DMA_REG_22_TSVER2SEL     13
//#define IX1000_DMA_REG_22_TSVER1SEL     12
//#define IX1000_DMA_REG_22_MMCSEL        11
//#define IX1000_DMA_REG_22_MGKSEL        10
//#define IX1000_DMA_REG_22_RWKSEL        9
//#define IX1000_DMA_REG_22_SMASEL        8
//#define IX1000_DMA_REG_22_PCSSEL        6
//#define IX1000_DMA_REG_22_ADDMAADRSEL   5
//#define IX1000_DMA_REG_22_HASHSEL       4
//#define IX1000_DMA_REG_22_EXTHASHEN     3
//#define IX1000_DMA_REG_22_HDSEL         2
//#define IX1000_DMA_REG_22_GMIISEL       1
//#define IX1000_DMA_REG_22_MIISEL        0

/* --------------------------------------------------------------------------*/
/* MII register / addresses                                                */
/* --------------------------------------------------------------------------*/

/* Generic MII registers. */
#define MII_BMCR              0x00      /* MII Control Register */
    #define MII_BMCR_RESET_CMD    0x8000

//#define MII_BMCR_RESET        15        /* Reset */
//#define MII_BMCR_LOOPBACK     14        /* Loopback */
//#define MII_BMCR_SPEED100     13        /* Speed Selection LSB 100Mbps/10Mbps */
#define MII_BMCR_ANENABLE     12        /* Auto-Negotiation enable */
#define MII_BMCR_PDOWN        11        /* Powerdown */
//#define MII_BMCR_ISOLATE      10        /* Isolate */
//#define MII_BMCR_ANRESTART    9         /* Restart Auto-Negotiation */
//#define MII_BMCR_FULLDPLX     8         /* Full duplex */
//#define MII_BMCR_CTST         7         /* Collision test */
//#define MII_BMCR_SPEED1000    6         /* Speed Selection MSB 1000Mbps */

#define MII_BMSR              0x01      /* Status Register */
#define MII_BMSR_100BASE4     15        /* 100mbps, 4k packets  */
#define MII_BMSR_100FULL      14        /* 100mbps, full-duplex */
#define MII_BMSR_100HALF      13        /* 100mbps, half-duplex */
#define MII_BMSR_10FULL       12        /* 10mbps, full-duplex  */
#define MII_BMSR_10HALF       11        /* 10mbps, half-duplex  */
//#define MII_BMSR_100FULL2     10        /* 100BASE-T2 FDX */
//#define MII_BMSR_100HALF2     9         /* 100BASE-T2 HDX */
//#define MII_BMSR_ESTATEN      8         /* Extended Status in R15 */
//#define MII_BMSR_UNIAV        7         /* Unidrectional ability */
//#define MII_BMSR_MFPRESUP     6         /* MF Preamble Suppression */
//#define MII_BMSR_ANEGCOMPLETE 5         /* Auto-negotiation complete   */
//#define MII_BMSR_RFAULT       4         /* Remote Fault Eetect       */
//#define MII_BMSR_ANEGABIL     3         /* Auto-Negotiation Ability   */
//#define MII_BMSR_LSTATUS      2         /* Link Status           */
//#define MII_BMSR_JCD          1         /* Jabber Detect */
//#define MII_BMSR_ERCAP        0         /* Ext-reg capability        */

//#define MII_PHYSID1           0x02      /* PHYS ID 1 */
//#define MII_PHYSID2           0x03      /* PHYS ID 2 */
    
#define MII_LEDCR             0x18
#define MII_LEDCR_ON          0x38
#define MII_LEDCR_OFF         0x3F

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif /* _IX1000_REG_H */

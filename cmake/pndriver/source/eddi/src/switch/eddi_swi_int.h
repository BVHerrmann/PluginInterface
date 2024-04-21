#ifndef EDDI_SWI_INT_H          //reinclude-protection
#define EDDI_SWI_INT_H

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
/*  F i l e               &F: eddi_swi_int.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  SWITCH                                                                   */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  26.11.2007  JS    added SyncMACPrio Variable                             */
/*  08.01.2008  JS    added MRPRedundantPortsUsed,MRPRedundantPort Variables */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/

/* ========================================================================= */
/*                                                                           */
/* ModuelIDs                                                                 */
/*                                                                           */
/* Note : This is only for reference. The value is hardcoded in every C-File!*/
/*                                                                           */
/*        The main EDD functions uses the following IDs                      */
/*                                                                           */
/*        EDD:   0x00 .. 0x0F                                                */
/*                                                                           */
/*        Each Ethernethardwarefunctions uses the folling IDs                */
/*                                                                           */
/*        EIB:   0x10 .. 0x1F                                                */
/*                                                                           */
/*        Each component uses the followin module IDs (defined in component) */
/*                                                                           */
/*        NRT:   0x80 .. 0x8F                                                */
/*        SRT:   0x90 .. 0x9F                                                */
/*        IRT:   0xA0 .. 0xAF                                                */
/*        SWI:   0xB0 .. 0xBF                                                */
/*                                                                           */
/* ========================================================================= */

/*===========================================================================*/
/*                                defines                                    */
/*===========================================================================*/
#define REG_NRT_CONTROL            0UL
#define REG_LINE_DELAY             1UL
#define REG_MAC_CONTROL            2UL
#define REG_PHY_CMD                3UL
#define REG_PHY_STAT               4UL
#define REG_DEFAULT_VLAN_TAG       5UL
#define REG_MIN_PREAMBLE           6UL
#define REG_ARC_CTRL               7UL
#define REG_TRANS_CTRL             8UL
#define REG_RCV_CTRL               9UL
#define REG_IRT_DIAG               10UL
#define REG_STAT_CTRL_GBRX         11UL
#define REG_STAT_CTRL_TBRX         12UL
#define REG_STAT_CTRL_BTX          13UL
#define REG_STAT_CTRL_FTX          14UL
#define REG_STAT_CTRL_UFRX         15UL
#define REG_STAT_CTRL_UFRXU        16UL
#define REG_STAT_CTRL_MFRX         17UL
#define REG_STAT_CTRL_MFRXU        18UL
#define REG_STAT_CTRL_BFRX         19UL
#define REG_STAT_CTRL_UFTX         20UL
#define REG_STAT_CTRL_MFTX         21UL
#define REG_STAT_CTRL_BFTX         22UL
#define REG_STAT_CTRL_F0_64        23UL
#define REG_STAT_CTRL_F64          24UL
#define REG_STAT_CTRL_F65_127      25UL
#define REG_STAT_CTRL_F128_255     26UL
#define REG_STAT_CTRL_F256_511     27UL
#define REG_STAT_CTRL_F512_1023    28UL
#define REG_STAT_CTRL_F1024_1536   29UL
#define REG_STAT_CTRL_GFRX         30UL
#define REG_STAT_CTRL_TFRX         31UL
#define REG_STAT_CTRL_FRX0_64_CRC  32UL
#define REG_STAT_CTRL_RXOF         33UL
#define REG_STAT_CTRL_RXOSF        34UL
#define REG_STAT_CTRL_CRCER        35UL
#define REG_STAT_CTRL_FDROP        36UL
#define REG_STAT_CTRL_TXCOL        37UL
#define REG_STAT_CTRL_HOLBC        38UL
#define REG_STAT_HOL_MASK          39UL

#if defined (EDDI_CFG_REV5)
#define REG_STAT_LIST_STATUS_Px    40UL
#endif

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
/*#define REG_STAT_CTRL_CC_DIFF      40UL
#define REG_STAT_CTRL_CC_DELAY     41UL 
#define REG_STAT_CTRL_CC_COUNT     42UL
//#define REG_STAT_LIST_STATUS_Px  43UL*/
#endif

/*===========================================================================*/
/*                MAC-Table                                                  */
/*===========================================================================*/
#define  MAC_PROFINET_LEN           0x03
#define  MAC_RT_3_DA_LEN            0x01   /* IEC - PNIO */
#define  MAC_RT_3_INVALID_DA_LEN    0x01
#define  MAC_RT_3_QUER_LEN          0x20
#define  MAC_PTP_ANNOUNCE_LEN       0x01    
#define  MAC_TIME_ANNOUNCE_LEN      0x01    
#define  MAC_PTP_TIME_ANNOUNCE_LEN  0x02    
#define  MAC_PTP_SYNC_WITH_FU_LEN   0x01    
#define  MAC_TIME_SYNC_WITH_FU_LEN  0x01    
#define  MAC_PTP_TIME_SYNC_WITH_FU_LEN  0x02    
#define  MAC_PTP_FOLLOW_UP_LEN      0x01    
#define  MAC_TIME_FOLLOW_UP_LEN     0x01    
#define  MAC_PTP_TIME_FOLLOW_UP_LEN 0x02    
#define  MAC_PTP_SYNC_LEN           0x01    
#define  MAC_TIME_SYNC_LEN          0x01    
#define  MAC_PTP_TIME_SYNC_LEN      0x02    
#define  MAC_MRP_1_LEN              0x01    /* IEC - MRP 1 */
#define  MAC_MRP_2_LEN              0x01    /* IEC - MRP 2 */
#define  MAC_MRP_IC_1_LEN           0x01    /* IEC - MRP IC 1 */
#define  MAC_MRP_IC_2_LEN           0x01    /* IEC - MRP IC 2 */
#define  MAC_HSYNC_LEN              0x01    /* IEC - HSYNC */
#define  MAC_IEEE_RESERVED_LEN_1    0x0E    /* IEEE Reserved addresses - 1 00-00-00 - 00-00-0D */
#define  MAC_IEEE_RESERVED_LEN_1_1  0x0D    /* IEEE Reserved addresses - 1 00-00-01 - 00-00-0D */
#define  MAC_LLDP_PTP_DELAY_LEN     0x01    /* PTP (Delay) and LLDP        00-00-0E            */
#define  MAC_IEEE_RESERVED_LEN_2    0x02    /* IEEE Reserved addresses - 2 00-00-0F - 00-00-10 */
#define  MAC_HSR_1_LEN              0x01    /* HSR 1 */
#define  MAC_HSR_2_LEN              0x01    /* HSR 2 */
#define  MAC_STBY_1_LEN             0x01    /* HSR STBY 1 Unicast! */ 
#define  MAC_STBY_2_LEN             0x01    /* HSR STBY 2 */
#define  MAC_DCP_LEN                0x02
#define  MAC_BROADCAST_LEN          0x01
#define  MAC_CARP_LEN               0xFF
#define  MAC_CARP_MC_LEN            0x01

#if defined (EDDI_CFG_ENABLE_MC_FDB)
#define  EDDI_MAC_MAX_MCMAC_ADR     (MAC_RT_3_QUER_LEN        + MAC_RT_3_DA_LEN           + MAC_RT_3_INVALID_DA_LEN + MAC_PTP_ANNOUNCE_LEN   +  \
                                     MAC_PTP_SYNC_WITH_FU_LEN + MAC_PTP_FOLLOW_UP_LEN     + MAC_PTP_SYNC_LEN        + MAC_LLDP_PTP_DELAY_LEN +  \
                                     MAC_IEEE_RESERVED_LEN_1  + MAC_IEEE_RESERVED_LEN_1_1 + MAC_IEEE_RESERVED_LEN_2 + MAC_MRP_1_LEN          +  \
                                     MAC_MRP_2_LEN            + MAC_HSYNC_LEN             + MAC_HSR_1_LEN           + MAC_HSR_2_LEN          +  \
                                     MAC_STBY_2_LEN           + MAC_DCP_LEN               + MAC_BROADCAST_LEN       + MAC_PROFINET_LEN )
#endif

/************************* EDDI_CFG_REV6 ***********************************/

typedef struct _SWI_PORTINDEX_REG_TYPE
{
    LSA_UINT32     HWPort[EDDI_MAX_IRTE_PORT_CNT];

} SWI_PORTINDEX_REG_TYPE;

extern const SWI_PORTINDEX_REG_TYPE     ERTEC_RegAdr[];

/*---------------------------------------------------------------------------*/
/* ERTEC400-Ports                                                            */
/*---------------------------------------------------------------------------*/
#define SWI_PORTINDEX_0    0x00
#define SWI_PORTINDEX_1    0x01
#define SWI_PORTINDEX_2    0x02
#define SWI_PORTINDEX_3    0x03

/*---------------------------------------------------------------------------*/
/* DestinationPort in der MAC-Tabelle                                        */
/*---------------------------------------------------------------------------*/
#define SWI_MAC_CH_A       0x01
#define SWI_MAC_CH_B       0x02
//#define SWI_MAC_PORT_0     0x04
//#define SWI_MAC_PORT_1     0x08
//#define SWI_MAC_PORT_2     0x10
//#define SWI_MAC_PORT_3     0x20

/*---------------------------------------------------------------------------*/
/* Mulitcast-Adresse                                                         */
/*---------------------------------------------------------------------------*/
#define SWI_MULTICAST_ADR  0x01

/*---------------------------------------------------------------------------*/
/* Mulitcast-Adresse                                                         */
/*---------------------------------------------------------------------------*/
#if defined (EDDI_CFG_ENABLE_MC_FDB)
#define SWI_MC_TABLE_MAX_SIZE     (0x1FEUL * 8)   //Max. MCTable-size is limited by 9bit-ptrs. 0x1FF is not usable.
#endif

#if defined (EDDI_CFG_REV7)
#define EDDI_MAX_MACADR_BAND_SIZE    32    /* max. size of a MC-MACAddr-Band */
#endif

#define SWI_MAX_DB_NR_FULL_NRT_FRAME    13
#define SWI_MIN_NRT_FRAME_SIZE          84 
#define SWI_TRANS_TIME_BYTE_100MBIT_NS  80 //ns

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/
typedef enum _SWI_LOW_WATER_EVENT_TYPE
{
    LOW_WATER_EVENT_IRQ,
    LOW_WATER_EVENT_TIME_OUT

} SWI_LOW_WATER_EVENT_TYPE;

typedef struct _SWI_LINK_PARAM_TYPE
{
    LSA_UINT8                 LastLinkStatus;
    LSA_UINT8                 LastLinkSpeed;
    LSA_UINT8                 LastLinkMode;
    LSA_UINT8                 LinkStatus;
    LSA_UINT8                 LinkSpeed;
    LSA_UINT8                 LinkMode;
    LSA_UINT16                LinkSpeedMode_Config;
    LSA_UINT16                SpanningTreeState;
    LSA_BOOL                  bFireLinkDownAction;   /* Execute activity after LinkDown      */
    LSA_BOOL                  PhyPowerOff;           /* StartUp behaviour Phy                */
    LSA_BOOL                  bMRPRedundantPort;     /* Configuration state (DSB, will never change afterwards): TRUE if this is a MRP Redundant Port */

} SWI_LINK_PARAM_TYPE;

//typedef SWI_LINK_PARAM_TYPE EDDI_LOCAL_MEM_ATTR * SWI_LINK_PARAM_PTR_TYPE;

#define EDDI_STATE_PHASE_SHIFT_CHECK        0

#if defined (EDDI_CFG_ERTEC_200)
#define EDDI_STATE_PHASE_SHIFT_FIRE_IND     1
#define EDDI_STATE_PHASE_SHIFT_DO_NOTHING   2
#endif

typedef struct _SWI_LINK_IND_EXT_PARAM_TYPE
{
    //        Last.... Last value
    //Without Last.... Current value

    //Both should always be equal, because changing of the current value will lead
    //to setting of "Last". Only a trigger (inequality) is needed.

    // LineDelay set in the HW
    LSA_UINT32                  LineDelayInTicksHardware;
    LSA_UINT32                  LastLineDelayInTicksHardware;

    // LineDelay measuers by GSY
    LSA_UINT32                  LineDelayInTicksMeasured;
    LSA_UINT32                  LastLineDelayInTicksMeasured;

    // CableDelay measured by GSY
    LSA_UINT32                  CableDelayInNsMeasured;
    LSA_UINT32                  LastCableDelayInNsMeasured;

    // PortDelay (MAC+ PHY) in TX direction
    LSA_UINT32                  PortTxDelay;
    LSA_UINT32                  LastPortTxDelay;

    // PortDelay (MAC+ PHY) in RX direction
    LSA_UINT32                  PortRxDelay;
    LSA_UINT32                  LastPortRxDelay;

    //PortStatus plugged or pulled
    LSA_UINT8                   PortStatus;
    LSA_UINT8                   LastPortStatus;

    //PhyStatus Disabled/Enabled
    LSA_UINT8                   PhyStatus;
    LSA_UINT8                   LastPhyStatus;

    //Current negotiation configuration
    LSA_UINT8                   Autoneg;
    LSA_UINT8                   LastAutoneg;

    //IRT Status of Port
    LSA_UINT8                   IRTPortStatus;
    LSA_UINT8                   LastIRTPortStatus;

    //iSRT Status of Port
    LSA_UINT8                   RTClass2_PortStatus;
    LSA_UINT8                   LastRTClass2_PortStatus;

    //IRT Status of Port
    LSA_UINT16                  PortState;
    LSA_UINT16                  LastPortState;

    //MAU-Type of Port
    LSA_UINT16                  MAUType;
    LSA_UINT16                  LastMAUType;

    //Media-Type of Port
    LSA_UINT8                   LastMediaType;

    //Is "real" POF port if Media-Type==optical
    LSA_UINT8                   LastIsPOF;

    //After LinkUp the Phase-Shift-Delay has been determined with the first received packet
    LSA_UINT32                  StateDeterminePhaseShift;

    //Remote Port State for SyncId0 (as reported by OHA)
    LSA_UINT8                   SyncId0_TopoOk;
    LSA_UINT8                   LastSyncId0_TopoOk;

    //TX Fragmentation State of Port
    #if defined (EDDI_CFG_FRAG_ON)
    LSA_UINT8                   TxFragmentation;
    LSA_UINT8                   LastTxFragmentation;
    #endif

    //Additional link check protocol is enabled/disabled
    LSA_UINT8	                AddCheckEnabled;
    LSA_UINT8	                LastAddCheckEnabled;

    //Reason for the link change performed by an additional link check protocol
    LSA_UINT8	                ChangeReason;
    LSA_UINT8	                LastChangeReason;

} SWI_LINK_IND_EXT_PARAM_TYPE;

/*---------------------------------------------------------------------------*/
/* actual LinkStatus/Speed and Mode of PHY                                   */
/*---------------------------------------------------------------------------*/
typedef struct _SWI_LINK_STAT_STRUCT
{
    LSA_UINT8  LinkStatus;
    LSA_UINT8  LinkSpeed;
    LSA_UINT8  LinkMode;

} SWI_LINK_STAT_TYPE;

typedef LSA_VOID  ( EDDI_LOCAL_FCT_ATTR *  SWI_PHYBUG_RESET_VAL_FCT) ( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                                       LSA_UINT32                 const HwPortIndex,
                                                                       SER_SWI_LINK_PTR_TYPE      const pEDDILinkStatusPx );

typedef LSA_VOID  ( EDDI_LOCAL_FCT_ATTR *  SWI_PHYBUG_TIMER_FCT) ( LSA_VOID  *  const  context );

typedef struct _SWI_PHY_BUGS_PORT
{
    LSA_TIMER_ID_TYPE                    PowerUpDelayTimerID;
    LSA_BOOL                             bPhyBugs_WA_Running; //TRUE: Timer for workarounds running, ignore "real" LINK-Interrupts
    LSA_BOOL                             bPhyReset; 
    EDD_UPPER_RQB_PTR_TYPE               pPhyResetRQB;
    LSA_UINT32                           HwPortIndex;
    EDDI_LOCAL_DDB_PTR_TYPE              pDDB;

    LSA_UINT32                           Rcv_New;
    LSA_UINT32                           Rcv_Old;

    LSA_UINT32                           RxErr_Old;
    LSA_UINT32                           RxErr_New;
    LSA_UINT32                           RxErr_Start;
    LSA_UINT32                           t_NoRcvInMs;

    LSA_UINT32                           LinkStatus;

    SER_SWI_LINK_TYPE                    EDDILinkStatusPx;

    LSA_UINT32                           LastTicks_in_10ns;
    LSA_UINT32                           ActTimerTicks_in_10ns;

} SWI_PHY_BUGS_PORT;

typedef struct _SWI_PHY_BUGS
{
    LSA_UINT32                           Tp_RcvFrame_in_10ns;

    SWI_PHYBUG_RESET_VAL_FCT             SwiPhyBugsResetValFct;
    SWI_PHY_BUGS_PORT                    port[EDDI_MAX_IRTE_PORT_CNT];

    SWI_PHYBUG_TIMER_FCT                 EDDI_SwiPhyBugTimerFct;

    LSA_TIMER_ID_TYPE                    NewCycleTimerID;

    EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE     Fix;

} SWI_PHY_BUGS;

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC) 
/* struct for the NSC Bug, Speed and Mode calulating during autoneg */
typedef enum _SWI_NSC_AUTONEG_BUGS_EVENT_TYPE
{
    SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_ON, /* change to autoneg      */
    SWI_NSC_AUTONEG_BUGS_EVENT_AUTONEG_OFF,/* change to fix settings */
    SWI_NSC_AUTONEG_BUGS_EVENT_TIMER,
    SWI_NSC_AUTONEG_BUGS_EVENT_LINKUP,  /* only   appearance during autoneg */
    SWI_NSC_AUTONEG_BUGS_EVENT_LINKDOWN /* always appearance */

} SWI_NSC_AUTONEG_BUGS_EVENT_TYPE;
#endif

/* struct for the NSC Bug, Speed and Mode calulating during autoneg */
typedef enum _SWI_AUTONEG_GET_SPEEDMODE_TYPE
{
    SWI_AUTONEG_GET_SPEEDMODE_OK,
    SWI_AUTONEG_GET_SPEEDMODE_LINKDOWN,
    SWI_AUTONEG_GET_SPEEDMODE_AUTONEGCOMPLETE_NO,
    SWI_AUTONEG_GET_SPEEDMODE_CALL_ASYNC,
    SWI_AUTONEG_GET_SPEEDMODE_RESTART,
    SWI_AUTONEG_GET_SPEEDMODE_ERROR,
    SWI_AUTONEG_GET_SPEEDMODE_NOT_RELEVANT

} SWI_AUTONEG_GET_SPEEDMODE_TYPE;

//list of function pointers for transceivers
typedef struct _EDDI_TRA_FCTPTR_TYPE
{
    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRASetPowerDown)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_UINT8                           const  PHYPower,
        LSA_BOOL                            const  bRaw);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRACheckPowerDown)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_BOOL                                 * pIsPowerDown);

    LSA_VOID                        LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRAStartPhy)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_BOOL                            const  bPlugCommit);

    SWI_AUTONEG_GET_SPEEDMODE_TYPE  LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRAGetSpMoAutoNegOn)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        SER_SWI_LINK_PTR_TYPE               const  pBasePx);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRACheckSpeedModeCapability)(
        EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
        LSA_UINT32                                const  HwPortIndex,
        LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pCapability);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRAAutonegMappingCapability)(
        EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,
        LSA_UINT32                                const  HwPortIndex,
        LSA_UINT32  EDDI_LOCAL_MEM_ATTR const  *  const  pCapability);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRASetMDIX)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_BOOL                            const  bMDIX);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRARestoreAutoMDIX)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRASetFXMode)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_BOOL                            const  bFXModeON);

    LSA_RESULT                      LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRASetSoftReset)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex);

    LSA_VOID                        LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRALedBlinkBegin)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex);

    LSA_VOID                        LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRALedBlinkSetMode)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex,
        LSA_UINT16                          const  LEDMode);

    LSA_VOID                        LSA_FCT_PTR(EDDI_LOCAL_FCT_ATTR, pTRALedBlinkEnd)(
        EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
        LSA_UINT32                          const  HwPortIndex);

} EDDI_TRA_FCTPTR_TYPE;

#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
//Timer running or stopped
typedef enum _SWI_NSC_TIMER_STATE_RUNNUING_TYPE
{
    SWI_NSC_TIMER_STATE_STOPPED,
    SWI_NSC_TIMER_STATE_RUNNUING

} SWI_NSC_TIMER_STATE_RUNNUING_TYPE;

typedef SWI_AUTONEG_GET_SPEEDMODE_TYPE  ( EDDI_LOCAL_FCT_ATTR *  SWI_NSC_AUTONEG_VAL_FCT) ( EDDI_LOCAL_DDB_PTR_TYPE          const  pDDB,
                                                                                            LSA_UINT32                       const  HwPortIndex,
                                                                                            SWI_NSC_AUTONEG_BUGS_EVENT_TYPE  const  AppearEvent );

typedef struct _SWI_NSC_AUTONEG_PORT_BUGS
{
    SER_SWI_LINK_TYPE                   LinkState;
    SWI_NSC_AUTONEG_VAL_FCT             SwiNSCAutoNegBugStateFct;
    LSA_UINT32                          SwiNSCAutoNegTimer100ms_Count;
    SWI_NSC_TIMER_STATE_RUNNUING_TYPE   SwiNSCAutoNegTimerState;

} SWI_NSC_AUTONEG_PORT_BUGS;

typedef struct _SWI_NSC_AUTONEG_BUGS
{
    SWI_NSC_AUTONEG_PORT_BUGS           Port[EDDI_MAX_IRTE_PORT_CNT];
    LSA_TIMER_ID_TYPE                   AutonegBugTimer100ms_id;

} SWI_NSC_AUTONEG_BUGS;
#endif //EDDI_CFG_PHY_TRANSCEIVER_NSC

/* defines for Status see EDD_SRV_GET_LINK_STATUS */
/* defines for Speed  see EDD_SRV_GET_LINK_STATUS */
/* defines for Mode   see EDD_SRV_GET_LINK_STATUS */

/* Fcode fuer EDD_SRV_SWITCH_FLUSH_FILTERING_DB */
#define SWI_FCODE_NOT_DEFINIED                          0
#define SWI_CLEAR_TX_QUEUES_FCODE_RESET_TX_QUEUE        1
#define SWI_FLUSH_FILTERING_DB_TX_FCODE_RESET_TX_QUEUE  2
#define SWI_FLUSH_FILTERING_DB_TX_FCODE_CLEAR_FDB       3

/*REV7*/
//Borderline bit for Rev7
#define  EDDI_BL_USR_GSY_OHA        0
#define  EDDI_BL_USR_PRM            1
#define  EDDI_BL_USR_NONE           0xFF


#define  EDDI_BL_MAC_ADR_BAND_PTCP_SYNC_INDEX           0
#define  EDDI_BL_MAC_ADR_BAND_PTP_ANNOUNCE_INDEX        1
#define  EDDI_BL_MAC_ADR_BAND_PTP_SYNC_WITH_FU_INDEX    2
#define  EDDI_BL_MAC_ADR_BAND_PTP_FOLLOW_UP_INDEX       3
#define  EDDI_BL_MAC_ADR_BAND_DEFAULT_INDEX             4

#if defined (EDDI_CFG_REV7)
#define  EDDI_BL_MAC_USR_SIZE                  5
#define  EDDI_BL_USR_SIZE                      2
#define  EDDI_BL_FW                            0
#define  EDDI_BL_EGRESS                        1
#define  EDDI_BL_IGRESS_EGRESS                 2

typedef struct _EDDI_BORDERLINE_TYPE
{
    LSA_UINT8  PortState[EDDI_MAX_IRTE_PORT_CNT];

} EDDI_BORDERLINE_TYPE;
#endif

/*---------------------------------------------------------------------------*/
/* DDB-SWITCH                                                                */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DDB_COMP_SWITCH_TYPE
{
    LSA_UINT32                              UsrHandleCnt; //Number of handles using

    SWI_LINK_PARAM_TYPE                     LinkPx[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    SWI_LINK_IND_EXT_PARAM_TYPE             LinkIndExtPara[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    SWI_LINK_STAT_TYPE                      AutoLastLinkStatus; //current Link-Status

    //set value only for 100MBit, 10Mbit => 0
    LSA_UINT8                               MinPreamble100Mbit;
    LSA_UINT8                               MinPreamble10Mbit;

    //Service EDDI_SRV_SWITCH_SET_AGING_TIME
    LSA_TIMER_ID_TYPE                       AgePollTimerId;
    LSA_UINT16                              AgePollTime;
    LSA_UINT16                              AgePollTimeBase;

    //Service EDDI_SRV_SWITCH_CLEAR_TX_QUEUES
    //Service is protected against recursive call
    LSA_TIMER_ID_TYPE                       ResetSQ_TimerId;
    LSA_UINT32                              ResetSQ_Cmd;
    EDD_UPPER_RQB_PTR_TYPE                  ResetSQ_pRQB;
    EDD_SERVICE                             ServiceCanceled;
    LSA_UINT16                              ResetSQ_Cmd_Fcode;
    LSA_BOOL                                ResetSQ_Reestablish_SpanningTreeState;
    LSA_UINT16                              ResetSQ_SpanningTreeState[EDDI_MAX_IRTE_PORT_CNT];

    //Service EDDI_SRV_SWITCH_ENABLE_LINK_CHANGE
    //Service is protected against recursive call
    LSA_BOOL                                DisableLinkChangePHYSMI;
    EDD_UPPER_RQB_PTR_TYPE                  EnableLinkChange_pRQB;

    SWI_PHY_BUGS                            PhyBugs;
    LSA_UINT32                              StartErtecTicks;
    EDDI_TRA_FCTPTR_TYPE                    TraFctPtrs[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    #if defined (EDDI_CFG_ERTEC_200)
    LSA_TIMER_ID_TYPE                       PhyPhaseShift_TimerId;
    #endif

    #if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
    SWI_NSC_AUTONEG_BUGS                    NSCAutoNegBug;
    #endif
    
    LSA_UINT16                              SyncMACPrio;    /* actual MAC-Priority                             */
                                                            /* EDDI_SWI_FDB_PRIO_DEFAULT/EDDI_SWI_FDB_PRIO_ORG */
                                                            /* of PTCP Sync Frame without FU                   */

    LSA_BOOL                                bMRPRedundantPortsUsed; /* set to TRUE if MRPRedundantPorts are */
                                                                    /* configured within DSB. We use MRP.   */

    LSA_UINT8                               MaxMRP_Instances;
    LSA_UINT8                               MRPDefaultRoleInstance0; //default-role forstartup (EDD_MRP_ROLE_NONE, _CLIENT, _MANAGER), can be overwritten by record 0x8052
    LSA_UINT8                               MRPSupportedRole;
    LSA_UINT8                               MRPSupportedMultipleRole;
    LSA_UINT8                               MRAEnableLegacyMode;
    LSA_UINT8                               MaxMRPInterconn_Instances;
    LSA_UINT8                               SupportedMRPInterconnRole;
    LSA_UINT16                              HSyncUserPortID[EDDI_MAX_HSYNC_PORT_CNT];

    
    #if defined (EDDI_CFG_REV7)
    EDDI_BORDERLINE_TYPE                    BLTable[EDDI_BL_USR_SIZE][EDDI_BL_MAC_USR_SIZE*EDDI_MAX_MACADR_BAND_SIZE];
    #endif

    #if defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    LSA_BOOL                                bResetIRTCtrl;         /* LSA_TRUE: reset IRTCtrl when resetting the switch-module */
    #endif

} EDDI_DDB_COMP_SWITCH_TYPE;

typedef EDDI_DDB_COMP_SWITCH_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Header for dynamic list                                                   */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DYN_LIST_ENTRY_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DYN_LIST_ENTRY_PTR_TYPE;

typedef struct _EDDI_DYN_LIST_ENTRY_TYPE
{
    EDDI_DYN_LIST_ENTRY_PTR_TYPE   next_dyn_ptr;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE   prev_dyn_ptr;

    LSA_VOID_PTR_TYPE              pData;

} EDDI_DYN_LIST_ENTRY_TYPE;

typedef struct _EDDI_DYN_LIST_HEAD_TYPE
{
    EDDI_DYN_LIST_ENTRY_PTR_TYPE   pBottom;
    EDDI_DYN_LIST_ENTRY_PTR_TYPE   pTop;
    LSA_UINT32                     Cnt;

} EDDI_DYN_LIST_HEAD_TYPE;

typedef struct _EDDI_DYN_LIST_HEAD_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_DYN_LIST_HEAD_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* SWI SRV                                                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: ...                                     */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE
{
    LSA_UINT16  PortIDLinkSpeedMode[EDDI_MAX_IRTE_PORT_CNT]; //Index = UsrPortIndex

} EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE;

typedef struct _EDDI_RQB_SWI_SET_PORT_SPEEDMODE_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_SWI_SET_PORT_SPEEDMODE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_LOW_WATER_IND_PROVIDE   */
/*---------------------------------------------------------------------------*/
#define EDDI_SWI_LOW_WATER_STATE_OPERATE  0x01
#define EDDI_SWI_LOW_WATER_STATE_PAUSE    0x02

typedef struct _EDDI_RQB_SWI_LOW_WATER_TYPE
{
    LSA_BOOL     DoCheck;
    LSA_UINT16   State;
    LSA_UINT16   NRT_FCW_Limit;
    LSA_UINT16   NRT_DB_Limit;
    LSA_UINT16   FreeFCWCnt;
    LSA_UINT16   FreeDBCnt;
    LSA_UINT16   LostLowWaterMark;

} EDDI_RQB_SWI_LOW_WATER_TYPE;

/*---------------------------------------------------------------------------*/
/* Macros                                                                    */
/*---------------------------------------------------------------------------*/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SWI_INT_H


/*****************************************************************************/
/*  end of file eddi_swi_int.h                                               */
/*****************************************************************************/

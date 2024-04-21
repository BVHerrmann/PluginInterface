#ifndef _INTEL_USR_H                      /* ----- reinclude-protection ----- */
#define _INTEL_USR_H

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
/*  F i l e               &F: INTEL_usr.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Level user header (PCAP)                     */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
#endif
/*****************************************************************************/

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_OPEN(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDDS_HANDLE hDDB,
        EDDS_UPPER_DPB_PTR_TO_CONST_TYPE pDPB,
        LSA_UINT32 TraceIdx,
		EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE pCaps);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SETUP(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDDS_UPPER_DSB_PTR_TYPE pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE pHwParam);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SHUTDOWN(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_CLOSE(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV( LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE EDDS_LOCAL_MEM_ATTR *pBufferAddr,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pLength,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pPortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_PROVIDE(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE pBufferAddr);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_RECV_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND(
        LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE pBufferAddr,
        LSA_UINT32 Length,
        LSA_UINT32 PortID);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_TRIGGER(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SEND_STS(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_STATS(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        EDDS_LOCAL_STATISTICS_PTR_TYPE pStats);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_GET_LINK_STATE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        EDD_UPPER_GET_LINK_STATUS_PTR_TYPE pLinkStat,
        EDDS_LOCAL_MEM_U16_PTR_TYPE pMAUType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pMediaType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pIsPOF,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pPortStatus,
        EDDS_LOCAL_MEM_U32_PTR_TYPE pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE pLinkSpeedModeConfigured);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SET_LINK_STATE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_UINT32 PortID,
        LSA_UINT8 * const pLinkStat,
        LSA_UINT8 * const pPHYPower);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_ENABLE(LSA_VOID_PTR_TYPE pLLManagement,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL DisableAll, EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_BACKUP_MODE(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_RESTORE_MODE(
        LSA_VOID_PTR_TYPE pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR INTEL_LL_LED_SET_MODE(
        LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL LEDOn);

LSA_RESULT EDDS_LOCAL_FCT_ATTR INTEL_LL_SWITCH_CHANGE_PORT(
        LSA_VOID_PTR_TYPE pLLManagement,
		LSA_UINT16        PortID,
		LSA_UINT8         isPulled);

LSA_UINT32 EDDS_LOCAL_FCT_ATTR INTEL_LL_RECURRING_TASK(
        LSA_VOID_PTR_TYPE pLLManagement, LSA_BOOL hwTimeSlicing);

/*---------------------------------------------------------------------------*/
/* Define the INTEL_LL_HANDLE_TYPE for use by EDDS                            */
/*---------------------------------------------------------------------------*/

/* minimum size for framebuffer to receive 1518 byte is 2048 byte */
typedef LSA_UINT8 INTEL_FRAMEBUFFER[2048];
typedef INTEL_FRAMEBUFFER INTEL_RX_FRAMEBUFFER;
//typedef INTEL_FRAMEBUFFER INTEL_TX_FRAMEBUFFER;

typedef struct _INTEL_LL_PARAMETER_TYPE
{
    LSA_VOID_PTR_TYPE pRegBaseAddr;
    LSA_UINT16 DeviceID;
    LSA_BOOL Disable1000MBitSupport; // Defines if 1000 MBit shall not be supported with fixed
                                     // setting and with Autoneg. With autoneg, the PHY does not
                                     // advertise 1000 MBit support if configured for autoneg.
                                     // With fixed setting the PRM – Record (PortDataAdjust) will
                                     // be rejected if a 1000 MBit MAUType shall be configured.
} INTEL_LL_PARAMETER_TYPE;

/* states of statemachine for statistics */
/* see INTEL_LL_RECURRING_TASK */
typedef enum INTEL_READ_STATS_SM_STATES_TYPE_
{
    INTEL_READ_IN_STATISTICS1 = 0,
    INTEL_READ_IN_STATISTICS2,
    INTEL_READ_IN_STATISTICS3,
    INTEL_READ_IN_STATISTICS4,
    INTEL_READ_OUT_STATISTICS1,
    INTEL_READ_OUT_STATISTICS2,
    INTEL_READ_OUT_STATISTICS3,
    INTEL_READ_OUT_STATISTICS4,
    INTEL_READ_IN_ERROR_STATISTICS1,
    INTEL_READ_IN_ERROR_STATISTICS2,
    INTEL_READ_IN_ERROR_STATISTICS3,
    INTEL_READ_IN_ERROR_STATISTICS4,
    INTEL_READ_OUT_ERROR_STATISTICS1,
    INTEL_READ_OUT_ERROR_STATISTICS2
} INTEL_READ_STATS_SM_STATES_TYPE;

/* statistics index in register shadow table */
/* see INTEL_LL_RECURRING_TASK */
enum INTEL_STATISTICS_REGISTER_INDEX_TYPE_
{
    /* IN statistics */
    INTEL_STATS_INDEX_GPRC = 0, /* Good Packets Received Count  - GPRC (0x04074) */
    INTEL_STATS_INDEX_BPRC, /* Broadcast Packets Received Count - BPRC (0x04078) */
    INTEL_STATS_INDEX_MPRC, /* Multicast Packets Received Count - MPRC (0x0407C) */
    INTEL_STATS_INDEX_GORC, /* Good Octets Received Count       - GORCL(0x04088) */
    /* OUT statistics */
    INTEL_STATS_INDEX_GPTC, /* Good Packets Transmitted Count       - GPTC  (0x04080) */
    INTEL_STATS_INDEX_GOTC, /* Good Octets Transmitted Count        - GOTCL (0x04090) */
    INTEL_STATS_INDEX_MPTC, /* Management Packets Transmitted Count - MPTC  (0x040BC) */
    INTEL_STATS_INDEX_BPTC, /* Broadcast Packets Transmitted Count  - BPTC  (0x040F4) */
    /* IN ERROR statistics */
    INTEL_STATS_INDEX_CRCERRS, /* CRC Error Count         - CRCERRS (0x04000) */
    INTEL_STATS_INDEX_MPC, /* Missed Packets Count        - MPC     (0x04010) */
    INTEL_STATS_INDEX_RXERRC, /* RX Error Count           - RXERRC  (0x0400C) */
    INTEL_STATS_INDEX_RLEC, /* Receive Length Error Count - RLEC    (0x04040) */
    /* OUT ERRIR statistics */
    INTEL_STATS_INDEX_ECOL, /* Excessive Collision Count  - ECOL    (0x04018) */
    INTEL_STATS_INDEX_LATECOL, /* Late Collisions Count   - LATECOL (0x04020) */
    INTEL_STATS_INDEX_MAX
};


/* all waiting times are in microseconds */

/** time to wait after disabling tx/rx queues */
#define INTEL_TIME_RXTX_QUEUE_SHUTDOWN                  1000

/** minimum time after chip reset with no register access. */
#define INTEL_TIME_RESET_REGISTER_ACCESS                3000

/** minimum time for polling status of flushing TX queues.
 *
 * \note    Software must have a chance to to a EDDS_LL_SEND_STS on
 *          remaining frames in tx queues.
 */
#define INTEL_TIME_TX_QUEUE_SW_FLUSH                    1000

/** minimum time that phy reset should be active */
#define INTEL_TIME_RESET_PHY_ACTIVE                      300

/** minimum time after phy reset, until phy could be accessed */
#define INTEL_TIME_RESET_PHY_ACCESS                      300

/** minimum time between accessing the hardware semaphore mechanism */
#define INTEL_TIME_SEMAPHORE                              10

/** maximum time for PHY register setup programming */
#define INTEL_TIME_PHY_REGISTER_ACCESS                    64

/** minimal time to wait between MAC register programming (polling of queue enable bits etc.) */
#define INTEL_TIME_REGISTER_ACCESS                        10

/* only one port supported */
#define EDDS_INTEL_SUPPORTED_PORTS 1


/* states of statemachine for "link reset" */
/* see INTEL_LL_RECURRING_TASK */
typedef enum INTEL_LINK_RESET_STATE_SM_STATES_TYPE_
{
    INTEL_RESET_STATE_IDLE = 0,                 /**< idle state */
    INTEL_RESET_STATE_RXTX_QUEUE_SHUTDOWN,      /**< shutdown rx/tx queues */
    INTEL_RESET_STATE_RESET_CHIP_ON,            /**< set reset of chip ON*/
    INTEL_RESET_STATE_WF_SW_QUEUE_FLUSH,        /**< Wait for EDDS_LL_SEND_STS has flushed tx queues */
    INTEL_RESET_STATE_RESET_CHIP_OFF,           /**< set reset of chip OFF*/
    INTEL_RESET_STATE_RESET_PHY_ON,             /**< set reset of phy ON */
    INTEL_RESET_STATE_RESET_PHY_OFF,            /**< set reset of phy OFF */
    INTEL_RESET_STATE_RESET_PHY_FINISH,         /**< finish reset of phy */
    INTEL_RESET_STATE_WF_EEPROM,                /**< wait for eeprom initialization is finished */
    INTEL_RESET_STATE_WF_SETUP_PHY_SM,          /**< wait for handling phy setup state machine */
    INTEL_RESET_STATE_WF_REGISTER_SETUP_SM,     /**< wait for handling register setup state machine */
    INTEL_RESET_STATE_WF_SET_LINK_SM            /**< wait for handling link setup state machine */
} INTEL_LINK_RESET_STATE_SM_STATES_TYPE;

/** states for "PHY setup" state machine */
typedef enum INTEL_PHY_SETUP_SM_ENUM
{
    INTEL_PHY_SETUP_SM_IDLE = 0,
    INTEL_PHY_SETUP_SM_STEP1,
    INTEL_PHY_SETUP_SM_STEP2,
    INTEL_PHY_SETUP_SM_STEP3,
    INTEL_PHY_SETUP_SM_STEP4,
    INTEL_PHY_SETUP_SM_STEP5,
    INTEL_PHY_SETUP_SM_STEP6,
    INTEL_PHY_SETUP_SM_STEP7,
    INTEL_PHY_SETUP_SM_STEP8,
    INTEL_PHY_SETUP_SM_STEP9,
    INTEL_PHY_SETUP_SM_STEP10,
    INTEL_PHY_SETUP_SM_STEP11,
    INTEL_PHY_SETUP_SM_STEP12,
    INTEL_PHY_SETUP_SM_STEP13,
    INTEL_PHY_SETUP_SM_STEP14,
    INTEL_PHY_SETUP_SM_STEP15,
    INTEL_PHY_SETUP_SM_STEP16,
    INTEL_PHY_SETUP_SM_STEP17,
    INTEL_PHY_SETUP_SM_FINISH
} INTEL_PHY_SETUP_SM_TYPE;

/* states for "register setup" state machine */
typedef enum INTEL_REG_SETUP_SM_ENUM
{
    INTEL_REG_SETUP_SM_IDLE = 0,
    INTEL_REG_SETUP_SM_STEP1,
    INTEL_REG_SETUP_SM_STEP2,
    INTEL_REG_SETUP_SM_STEP3,
    INTEL_REG_SETUP_SM_STEP4,
    INTEL_REG_SETUP_SM_STEP5,
    INTEL_REG_SETUP_SM_FINISH
} INTEL_REG_SETUP_SM_TYPE;

/* states for "set link" state machine */
typedef enum INTEL_SET_LINK_SM_ENUM
{
    INTEL_SET_LINK_SM_IDLE = 0,
    INTEL_SET_LINK_SM_SETUP,
    INTEL_SET_LINK_SM_STEP1,
    INTEL_SET_LINK_SM_STEP2,
    INTEL_SET_LINK_SM_FINISH
}INTEL_SET_LINK_SM_TYPE;



/* used phy registers (shadows for reset and set link) */
typedef struct INTEL_PHY_REGS_STRUCT
{
    LSA_UINT16 pctrl;
    LSA_UINT16 ana;
    LSA_UINT16 gcon;
    LSA_UINT16 pscon;
} INTEL_PHY_REGS;

typedef struct _INTEL_LL_HANDLE_TYPE
{
    INTEL_LL_PARAMETER_TYPE params;
    LSA_UINT32 TraceIdx;
    LSA_VOID_PTR_TYPE hDDB;
    EDDS_SYS_HANDLE hSysDev;

    struct INTEL_RDESC_* pRxDescriptors; // start of Tx descr --> aligned address
    EDD_UPPER_MEM_PTR_TYPE pRxDescrAlloc; // start of allocated memory for Rx descr
    INTEL_RX_FRAMEBUFFER** RxFrameBufferPtr;
    LSA_UINT32 RxFrameBufferCount;
    LSA_UINT32 RXDescriptorIndex;
    LSA_UINT32 RXDescriptorIndexEnd;

    union INTEL_TDESC_* pTxDescriptors; // start of Tx descr --> aligned address
    EDD_UPPER_MEM_PTR_TYPE pTxDescrAlloc; // start of allocated memory for Tx descr
    LSA_UINT32 TxFrameBufferCount;
    LSA_UINT32 TXDescriptorIndex;
    LSA_UINT32 TXDescriptorIndexEnd;
    LSA_UINT32 TXDescriptorsUsed;
    EDD_MAC_ADR_TYPE InterfaceMAC;
    EDD_MAC_ADR_TYPE PortMAC[1];

    LSA_UINT32 MCRegL[16];
    LSA_UINT32 MCRegH[16];
    LSA_UINT32 backupLEDCTL;
    LSA_UINT32 savedLEDCTL;
    LSA_UINT32 fakeSendCounter;
    LSA_UINT8 savedLinkStat;
    LSA_UINT8 savedPHYPower;
    LSA_UINT32 lastIMS;
    LSA_UINT32 savedIVAR; /* for MSIX */

    LSA_UINT32 AutoNegCapAdvertised;
    LSA_UINT8 LinkSpeedModeConfigured;

    /* reset state machine parameter */
    /* see INTEL_LL_RECURRING_TASK */
    INTEL_LINK_RESET_STATE_SM_STATES_TYPE ResetState;
    LSA_BOOL isResetPending;
    LSA_UINT64 ResetTargetSysTim;

    /* statistic handling parameter */
    /* see INTEL_LL_RECURRING_TASK */
    INTEL_READ_STATS_SM_STATES_TYPE ReadStatisticsState;
    EDDS_STATISTICS_TYPE Statistics;
    LSA_UINT64 StatsRegShadow[INTEL_STATS_INDEX_MAX];
    LSA_UINT64 InUcastPktsShadow;
    LSA_UINT64 OutUcastPktsShadow;

    /* link status handling parameter */
    /* see INTEL_LL_RECURRING_TASK */
    LSA_UINT8 StatusShadow;
    LSA_UINT8 SpeedShadow;
    LSA_UINT8 ModeShadow;
    LSA_BOOL isLinkUp;

    INTEL_REG_SETUP_SM_TYPE  regSetupSM;

    /*  */
    INTEL_SET_LINK_SM_TYPE  setLinkSM;
    LSA_BOOL isLinkStateChanged;
    LSA_UINT8 newLinkStat;
    LSA_UINT8 newPHYPower;

    INTEL_PHY_SETUP_SM_TYPE  phySetupSM;
    /* phy register shadow storage for SetLink/Phy Reset */
    INTEL_PHY_REGS  phyRegs;

    EDDS_INTERNAL_STATISTIC_TYPE     perf_as;
    EDDS_INTERNAL_STATISTIC_TYPE     perf_rs;
    EDDS_INTERNAL_STATISTIC_TYPE     perf_tas;
    EDDS_INTERNAL_STATISTIC_TYPE     perf_trs;

    LSA_UINT8 PortStatus[EDDS_INTEL_SUPPORTED_PORTS];
} INTEL_LL_HANDLE_TYPE;

#define EDDS_INTEL_MAC_TABLE_START_OFFSET_MC     (1 + 1) /* the first addresses: if mac and port mac */

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* _INTEL_USR_H_ */

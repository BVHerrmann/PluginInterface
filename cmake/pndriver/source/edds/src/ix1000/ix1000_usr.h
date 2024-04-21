#ifndef _IX1000_USR_H                  /* ----- reinclude-protection ----- */
#define _IX1000_USR_H

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
/*  F i l e               &F: TEMPLATE_usr.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Low Layer user header (TEMPLATE).                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
#ifdef EDDS_MESSAGE
/*  23.06.10    AM    initial version.                                       */
/*  27.06.11    AM    replace "TEMPLATE" in file with your LLIF namespace,   */
/*                    e.g. "MY_ETHERNET_CONTROLLER".                         */
#endif
/*****************************************************************************/
#include "ix1000_cfg.h"
//#define IX1000_NUMBER_OF_PORTS                 1

//#define IX1000_IX1000_DID                      0x937   //DeviceID (IX1000)
//#define IX1000_IX1000_VID                      0x8086  //VendorID (Intel)

#define IX1000_CRC_32_IEE802_3                  0x04C11DB7ul /* CRC-32 Polynom (IEEE802.3) */
#define IX1000_err                              ~(0u)        /* error value                */
#define IX1000_CR                               0x3          /* clock range; 3: 35-60 MHz  */
#define IX1000_PHY_ID                           1

#define IX1000_TIMEOUT_US                       10
#define IX1000_CRC_LENGTH                       4

/* @note chapter 15.7.1 (ix1000 datasheet)
 * You can select one of the following options for descriptor structure:
 * — If timestamping is enabled in Register 448 (Timestamp Control Register) or
 * Checksum Offload is enabled in Register 0 (MAC Configuration Register), the
 * software needs to allocate 32-bytes (8 DWORDS) of memory for every
 * descriptor. For this, the software should set Bit 7 (Alternate Descriptor Size) of
 * Register 0 (Bus Mode Register).
 * — If timestamping or Checksum Offload is not enabled, the extended descriptors
 * (DES4 to DES7) are not required. Therefore, the software can use alternate
 * descriptors with the default size of 16 bytes.
 */
typedef volatile struct _IX1000_RX_DSCR {
    LSA_UINT32 rdes0;
    LSA_UINT32 rdes1;
    LSA_UINT32 rdes2;
    LSA_UINT32 rdes3;
} IX1000_RX_DSCR;

typedef volatile struct _IX1000_TX_DSCR {
    LSA_UINT32 tdes0;
    LSA_UINT32 tdes1;
    LSA_UINT32 tdes2;
    LSA_UINT32 tdes3;
} IX1000_TX_DSCR;
    
typedef struct _IX1000_LL_PARAMETER_TYPE
{
    volatile LSA_VOID_PTR_TYPE pRegBaseAddr_BAR0;
    //LSA_UINT32 DeviceID;
    //LSA_UINT32 VendorID;
    LSA_UINT32 Aligment;
    //LSA_UINT32 Granularity;
} IX1000_LL_PARAMETER_TYPE;
typedef IX1000_LL_PARAMETER_TYPE* IX1000_LL_PARAMETER_PTR_TYPE;

/* states of statemachine for statistics */
/* see INTEL_LL_RECURRING_TASK */
typedef enum IX1000_READ_STATS_SM_STATES_TYPE_
{
    IX1000_READ_IN_STATS_INDISC = 0,
    IX1000_READ_IN_STATS_INERR_CRC,
    IX1000_READ_IN_STATS_INERR_ALIGN,
    IX1000_READ_IN_STATS_INERR_RUNT,
    IX1000_READ_IN_STATS_INERR_JABB,
    IX1000_READ_IN_STATS_INERR_LEN,
    IX1000_READ_IN_STATS_INERR_OOR,
    IX1000_READ_IN_STATS_INERR_WD,
    IX1000_READ_OUT_STATS_ERR_UFL,
    IX1000_READ_OUT_STATS_ERR_LCOL,
    IX1000_READ_OUT_STATS_ERR_XCOL,
    IX1000_READ_OUT_STATS_ERR_CE,
    IX1000_READ_OUT_STATS_ERR_EXDEF
    
} IX1000_READ_STATS_SM_STATES_TYPE;

typedef struct _IX1000_LL_LINK_DATA_TYPE
{
    LSA_UINT8           Status;
    LSA_UINT8           Speed;
    LSA_UINT8           Mode;
    LSA_BOOL            Autoneg;
}IX1000_LL_LINK_DATA_TYPE;

typedef volatile struct _IX1000_TX_DSCR* IX1000_TX_DSCR_PTR;
typedef volatile struct _IX1000_RX_DSCR* IX1000_RX_DSCR_PTR;
typedef LSA_UINT8 IX1000_FRAMEBUFFER[EDD_FRAME_BUFFER_LENGTH];
typedef IX1000_FRAMEBUFFER IX1000_RX_FRAMEBUFFER;

typedef struct _IX1000_LL_MAC_TYPE {
    EDD_MAC_ADR_TYPE        mcMacAddr;
    LSA_UINT32              hashValue;
}IX1000_LL_MAC_TYPE;

typedef enum _IX1000_LL_LINK_SM_STATE
{
    IX1000_LINK_IDLE = 0,
    IX1000_LINK_GET_DATA,
    IX1000_LINK_SET_DATA,
    IX1000_LINK_UPDATE    
}IX1000_LL_LINK_SM_STATE;
    
typedef struct _IX1000_LL_HANDLE_TYPE
{
        IX1000_LL_PARAMETER_PTR_TYPE    pParams;
        LSA_UINT32                      TraceIdx;
        LSA_VOID_PTR_TYPE               hDDB;
        EDDS_SYS_HANDLE                 hSysDev;
        
        EDD_UPPER_MEM_PTR_TYPE          pRxDescrAlloc;                      // start of allocated memory for Rx descr
        IX1000_RX_DSCR_PTR              pRxDescriptors;                     // start of Rx descr --> aligned address
        LSA_UINT32                      RXDescriptorIndex;                  // current Rx Index
        IX1000_RX_FRAMEBUFFER**         RxFrameBufferPtr;                   // Array of Rx Frame Buffers
        LSA_UINT32                      RxFrameBufferCount;                 // Count of Rx Frame Buffers
        
        EDD_UPPER_MEM_PTR_TYPE          pTxDescrAlloc;                      // start of allocated memory for Tx descr
        IX1000_TX_DSCR_PTR              pTxDescriptors;                     // start of Tx descr --> aligned address
        LSA_UINT32                      TXDescriptorIndex;                  // current Rx Index
        LSA_UINT32                      TXDescriptorIndexEnd;               // End of Rx Descriptor Array
        LSA_UINT32                      TxFrameBufferCount;                 // Count of Tx Frame Buffers
        LSA_UINT32                      TxDescriptorsUsed;                  // Nmb of used Tx Descriptors
        
        EDD_MAC_ADR_TYPE                InterfaceMAC;                       // IF Mac == PortMac[1]
        
        LSA_UINT8                       phyID;
        
        LSA_UINT32                      AutonegCapsAdv;
        LSA_UINT8                       LinkSpeedModeConfigured;

        /* data for link handling */
        IX1000_LL_LINK_DATA_TYPE        LinkData;
        IX1000_LL_LINK_DATA_TYPE        LinkData_old;
        IX1000_LL_LINK_SM_STATE         LinkSM_State;
        LSA_UINT16                      phyData_Shadow;
        LSA_UINT8                       IsSetLink;
        LSA_UINT8                       newPhyPower;
        LSA_UINT8                       newLinkStat;
        LSA_UINT8                       savedPhyPower;
        LSA_UINT8                       savedLinkStat;

        EDDS_STATISTICS_TYPE            LL_Stats;
        
        LSA_UINT16                      LedBackUp;
        
        LSA_UINT8                       PortStatus;
        
        IX1000_LL_MAC_TYPE              macHashTable[EDDS_MAX_MC_MAC_CNT];
        
        /* shadow for stats state-machine */
        LSA_UINT32                      InErr_Shadow;
        LSA_UINT32                      OutErr_Shadow;
        IX1000_READ_STATS_SM_STATES_TYPE    readStatsState;
        
} IX1000_LL_HANDLE_TYPE;

typedef IX1000_LL_HANDLE_TYPE EDD_UPPER_MEM_ATTR *          IX1000_LL_HANDLE_PTR_TYPE;
typedef IX1000_LL_HANDLE_TYPE EDD_UPPER_MEM_ATTR const *    IX1000_LL_HANDLE_PTR_TO_CONST_TYPE;

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_OPEN(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDDS_HANDLE                         hDDB,
        EDDS_UPPER_DPB_PTR_TO_CONST_TYPE    pDPB,
        LSA_UINT32                          TraceIdx,
        EDDS_LOCAL_EDDS_LL_CAPS_PTR_TYPE    pCaps);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SETUP(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDDS_UPPER_DSB_PTR_TYPE             pDSB,
        EDDS_LOCAL_LL_HW_PARAM_PTR_TYPE     pHwParam);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SHUTDOWN(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_CLOSE(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV( LSA_VOID_PTR_TYPE pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE              EDDS_LOCAL_MEM_ATTR *pBufferAddr,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pLength,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortID);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV_PROVIDE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE              pBufferAddr);

LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_RECV_TRIGGER(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDD_UPPER_MEM_PTR_TYPE              pBufferAddr,
        LSA_UINT32                          Length,
        LSA_UINT32                          PortID);

LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND_TRIGGER(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SEND_STS(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_GET_STATS(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_UINT32                          PortID,
        EDDS_LOCAL_STATISTICS_PTR_TYPE      pStats);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_GET_LINK_STATE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_UINT32                          PortID,
        EDD_UPPER_GET_LINK_STATUS_PTR_TYPE  pLinkStat,
        EDDS_LOCAL_MEM_U16_PTR_TYPE         pMAUType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pMediaType,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pIsPOF,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pPortStatus,
        EDDS_LOCAL_MEM_U32_PTR_TYPE         pAutonegCapAdvertised,
        EDDS_LOCAL_MEM_U8_PTR_TYPE          pLinkSpeedModeConfigured);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SET_LINK_STATE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_UINT32                          PortID,
        LSA_UINT8 * const                   pLinkStat,
        LSA_UINT8 * const                   pPHYPower);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_MC_ENABLE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_MC_DISABLE(LSA_VOID_PTR_TYPE pLLManagement,
        LSA_BOOL                            DisableAll, 
        EDD_MAC_ADR_TYPE EDD_UPPER_MEM_ATTR *pMCAddr);

LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_BACKUP_MODE(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_RESTORE_MODE(
        LSA_VOID_PTR_TYPE                   pLLManagement);

LSA_VOID EDDS_LOCAL_FCT_ATTR IX1000_LL_LED_SET_MODE(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_BOOL                            LEDOn);

LSA_RESULT EDDS_LOCAL_FCT_ATTR IX1000_LL_SWITCH_CHANGE_PORT(
        LSA_VOID_PTR_TYPE                   pLLManagement,
        LSA_UINT16                          PortID,
        LSA_UINT8                           isPulled);

LSA_UINT32 EDDS_LOCAL_FCT_ATTR IX1000_LL_RECURRING_TASK(
        LSA_VOID_PTR_TYPE                   pLLManagement, 
        LSA_BOOL                            hwTimeSlicing);
    
#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif  /* _IX1000_USR_H */

#ifndef EDDI_SYS_H              //reinclude-protection
#define EDDI_SYS_H

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
/*  F i l e               &F: eddi_sys.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*  Note: All output macros are in eddi_out.h                                */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.02.04    ZR    add icc modul                                          */
/*  10.10.07    ZR    added EDDI_FATAL_ERR_CMD_IF_TIMEOUT                    */
/*  08.07.08    UL    Structure EDDI_SYNC_GET_SET_TYPE: element              */
/*                    CycleBeginTimeStamp changed to CycleBeginTimeStampInTic*/
/*  28.07.16    BV    #define EDDI_PHY_TRANSCEIVER_TYPE                      */
/*                    structure _EDDI_GLOB_PARA_TYPE: delete DelayParamsPOF  */
/*                                                    extend DelayParamsFX   */
/*                                                                           */
/*****************************************************************************/


/*===========================================================================*/
/*                              constants                                    */
/*===========================================================================*/

#define EDDI_BIT_MASK_8BIT(BitHigh, BitLow)     (LSA_UINT8)BitHigh,(LSA_UINT8)BitLow

#define EDDI_SET_SINGLE_BIT(t32, mask)          (t32 = t32 | mask)
#define EDDI_CLEAR_SINGLE_BIT(t32, mask)        (t32 = t32 & ~mask)

#define EDDI_BIT_MASK_PARA(BitHigh, BitLow)     (LSA_UINT32)BitHigh,(LSA_UINT32)BitLow

#define EDDI_SWAP_ALWAYS_32(var)            \
/*lint --e(941) --e(572) --e(778)*/         \
(  ((((var)) & 0xFF000000UL) >> 24)         \
 + ((((var)) & 0x00FF0000UL) >>  8)         \
 + ((((var)) & 0x0000FF00UL) <<  8)         \
 + ((((var)) & 0x000000FFUL) << 24))

#define EDDI_SWAP_ALWAYS_16(var)            \
/*lint --e(941) --e(572) --e(778)*/         \
(LSA_UINT16) ((((((var)) & 0xFF00) >> 8)    \
             + ((((var)) & 0x00FF) << 8)))

#if defined (EDDI_CFG_LITTLE_ENDIAN)

#define EDDI_PASTE_16(a, b)                              (0x##a##b)
#define EDDI_PASTE_32(a, b, c, d)                        (0x##a##b##c##d##L)

#define EDDI_SWAP_32(var)       (var)
#define EDDI_SWAP_32_FCT(var)   (var)

#define EDDI_SWAP_16(var)       (var)

#define EDDI_VALTOLE16(var)     (var)

#else //EDDI_CFG_BIG_ENDIAN

#define EDDI_PASTE_16(a, b)                              (0x##b##a)
#define EDDI_PASTE_32(a, b, c, d)                        (0x##d##c##b##a##L)

LSA_UINT32 EDDI_SWAP_32_FCT( LSA_UINT32 const var );

#define EDDI_SWAP_32(var)                   \
/*lint --e(941) --e(572) --e(778)*/         \
(  ((((var)) & 0xFF000000UL) >> 24)         \
 + ((((var)) & 0x00FF0000UL) >>  8)         \
 + ((((var)) & 0x0000FF00UL) <<  8)         \
 + ((((var)) & 0x000000FFUL) << 24))

#define EDDI_SWAP_16(var)                   \
(LSA_UINT16) ((  ((((var)) & 0xFF00) >> 8)  \
               + ((((var)) & 0x00FF) << 8)))

#define EDDI_VALTOLE16(var)    EDDI_SWAP_16(var)

#endif //EDDI_CFG_BIG_ENDIAN

#define EDDI_HOST2IRTE32(val_) EDDI_SWAP_32(val_)
#define EDDI_IRTE2HOST32(val_) EDDI_SWAP_32(val_)
#define EDDI_HOST2IRTE16(val_) EDDI_SWAP_16(val_)
#define EDDI_IRTE2HOST16(val_) EDDI_SWAP_16(val_)
#define EDDI_HOST2IRTE32x(dest_, val_) {                \
    LSA_UINT32 const val_local_ = (val_);               \
    (dest_) = EDDI_SWAP_32(val_local_); }

#define EDDI_IRTE2HOST32x(dest_, val_) {                \
    LSA_UINT32 const val_local_ = (val_);               \
    (dest_) = EDDI_SWAP_32(val_local_); }

#define EDDI_HOST2IRTE16x(dest_, val_) {                \
    LSA_UINT16 const val_local_ = (val_);               \
    (dest_) = EDDI_SWAP_16(val_local_); }

#define EDDI_IRTE2HOST16x(dest_, val_) {                \
    LSA_UINT16 const val_local_ = (val_);               \
    (dest_) = EDDI_SWAP_16(val_local_); }

/*---------------------------------------------------------------------------*/
/* fatal-errorcodes (used for EDDI_Excp())                                   */
/*---------------------------------------------------------------------------*/

#define EDDI_FATAL_ERROR_TYPE  LSA_UINT32

#define EDDI_FATAL_NO_ERROR              (EDDI_FATAL_ERROR_TYPE)  0x00
#define EDDI_FATAL_ERR_HANDLE_CLOSE      (EDDI_FATAL_ERROR_TYPE)  0x01
#define EDDI_FATAL_ERR_HANDLE_REL        (EDDI_FATAL_ERROR_TYPE)  0x02
#define EDDI_FATAL_ERR_DEV_INFO_CLOSE    (EDDI_FATAL_ERROR_TYPE)  0x03
#define EDDI_FATAL_ERR_CLOSE_DDB         (EDDI_FATAL_ERROR_TYPE)  0x04
#define EDDI_FATAL_ERR_NULL_PTR          (EDDI_FATAL_ERROR_TYPE)  0x05
#define EDDI_FATAL_ERR_MEM_FREE          (EDDI_FATAL_ERROR_TYPE)  0x06
#define EDDI_FATAL_ERR_LL                (EDDI_FATAL_ERROR_TYPE)  0x07
#define EDDI_FATAL_ERR_RELEASE_PATH_INFO (EDDI_FATAL_ERROR_TYPE)  0x08
#define EDDI_FATAL_ERR_ISR               (EDDI_FATAL_ERROR_TYPE)  0x09
#define EDDI_FATAL_ERR_SEND              (EDDI_FATAL_ERROR_TYPE)  0x0A
#define EDDI_FATAL_ERR_TIMEOUT_SEND      (EDDI_FATAL_ERROR_TYPE)  0x0B
#define EDDI_FATAL_ERR_CCW_ADD           (EDDI_FATAL_ERROR_TYPE)  0x0C
#define EDDI_FATAL_ERR_EXCP              (EDDI_FATAL_ERROR_TYPE)  0x0D
#define EDDI_FATAL_ERR_TIMER             (EDDI_FATAL_ERROR_TYPE)  0x0E
#define EDDI_FATAL_ERR_PPARAM            (EDDI_FATAL_ERROR_TYPE)  0x0F
#define EDDI_FATAL_ERR_STATE             (EDDI_FATAL_ERROR_TYPE)  0x10
#define EDDI_FATAL_ERR_EVENT             (EDDI_FATAL_ERROR_TYPE)  0x11
#define EDDI_FATAL_ERR_INI               (EDDI_FATAL_ERROR_TYPE)  0x12
#define EDDI_FATAL_ERR_INVALID_HANDLE    (EDDI_FATAL_ERROR_TYPE)  0x13
#define EDDI_FATAL_ERR_MEMCONV           (EDDI_FATAL_ERROR_TYPE)  0x14
#define EDDI_FATAL_ERR_CMD_IF_TIMEOUT    (EDDI_FATAL_ERROR_TYPE)  0x15
#define EDDI_FATAL_ERR_APPLSYNC          (EDDI_FATAL_ERROR_TYPE)  0x20

/*===========================================================================*/
/*                                 defines                                   */
/*===========================================================================*/

#define EDDI_DETAIL_PTR_TYPE                    /* pointer to EDDI_DETAIL */        \
struct eddi_detail_tag   EDDI_SYSTEM_MEM_ATTR *

#define EDDI_FATAL_ERROR_PTR_TYPE               /* pointer to LSA_FATAL_ERROR */    \
struct lsa_fatal_error_tag  EDDI_LOCAL_MEM_ATTR *

/* API defines */
/* EDDI_I2C_SDA_LOW_HIGHZ, EDDI_I2C_SCL_LOW_HIGHZ */
#define EDDI_I2C_SWITCH_PORT_OFF     0   //switch port to input or set level to 0
#define EDDI_I2C_SWITCH_PORT_ON      1   //switch port to output or set level to 1

/* EDDI_LL_NSC_PRESTART_ACTION */
#define EDDI_NSC_PRESTART_ACTION_CALLED_FROM_STARTUP        0       //EDDI_LL_NSC_PRESTART_ACTION called in startup sequence
#define EDDI_NSC_PRESTART_ACTION_CALLED_FROM_PLUG_COMMIT    1       //EDDI_LL_NSC_PRESTART_ACTION called during PLUG_COMMIT

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/


/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_OPEN                       */
/*---------------------------------------------------------------------------*/
#define EDDI_KRAM_SIZE_ERTEC400   0x30000
#define EDDI_KRAM_SIZE_ERTEC200   0x10000
#define EDDI_KRAM_SIZE_FPGA200    0x10000
#define EDDI_KRAM_SIZE_FPGA400    0x20000
#define EDDI_KRAM_SIZE_FPGASOC    0x10000
#define EDDI_KRAM_SIZE_SOC        0x2C000 /* 0x2A000 -> 170 kByte */

/*****************************************************/
/******* Values for resource limitation resulting  ***/
/******* from SPH Resource Connections             ***/
/*****************************************************/
#define EDDI_RESLIM_MAX_NR_PROV_SOC_2P                              512


#if !defined (EDDI_CFG_3BIF_2PROC)
    #include "pnio_pck4_on.h"

    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_GSHAREDMEM_TYPE
    {
      #if defined (EDDI_CFG_SYSRED_2PROC)
        volatile LSA_UINT32  ChangeCount;
        volatile LSA_UINT32  DataStatusShadow     [EDDI_RESLIM_MAX_NR_PROV_SOC_2P];
        volatile LSA_UINT32  DataStatusAcknowledge[EDDI_RESLIM_MAX_NR_PROV_SOC_2P];
      #endif

      LSA_UINT16    ARSet[EDD_CFG_MAX_NR_ARSETS+1];
    } PNIO_PACKED_ATTRIBUTE_POST;
    typedef struct _EDDI_GSHAREDMEM_TYPE EDDI_GSHAREDMEM_TYPE;

    #include "pnio_pck_off.h"

    #include "pnio_pck1_on.h"
    PNIO_PACKED_ATTRIBUTE_PRE struct _EDDI_SET_DS_ORDERTYPE
    {
        LSA_UINT16   OrderID;
        LSA_UINT8    Status;
        LSA_UINT8    Mask;
    }  PNIO_PACKED_ATTRIBUTE_POST;

    typedef struct _EDDI_SET_DS_ORDERTYPE EDDI_SET_DS_ORDERTYPE;

    PNIO_PACKED_ATTRIBUTE_PRE union _EDDI_SET_DS_ORDER
    {
        EDDI_SET_DS_ORDERTYPE  SetDS_Order;
        LSA_UINT32             SetDS_OrderCombined;
    }  PNIO_PACKED_ATTRIBUTE_POST;

    typedef union _EDDI_SET_DS_ORDER EDDI_SET_DS_ORDER;

#include "pnio_pck_off.h"

#endif

typedef struct _EDDI_KRAM_TYPE
{
    LSA_UINT32            offset_ProcessImageEnd;
    LSA_UINT32            size_reserved_for_eddi; // zum Quercheck

} EDDI_KRAM_TYPE;

/* The InterfaceID is a number uniquely identifying the interface (device) and should correspond to the 
   case labelling. This number together with the port number is used by CM for the interface submodule numbering. 
   EDDI only propagates the InterfaceID without using it. It can be read by the service EDD_SRV_GET_PARAMS. 
   Each EDD(I)-device needs to get it´s own unique InterfaceID. 
   Possible values: 0..15. Derives from the interface-subslot nr (0x8ipp), where i is the InterfaceID and pp the port number. */

/* PhyTransceiver */
#define EDDI_PHY_TRANSCEIVER_TYPE   LSA_UINT32

#if defined (EDDI_CFG_PHY_TRANSCEIVER_BROADCOM)
#define EDDI_PHY_TRANSCEIVER_BROADCOM       0   //BROADCOM transceiver (PHY) (BCM5221) e.g. used as external PHY for ERTEC400
#define EDDI_PHY_TRANSCEIVER_BROADCOM_MC    1   //INTERNAL use only!
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_NEC)
#define EDDI_PHY_TRANSCEIVER_NEC            2   //NEC transceiver (PHY) (AATHPHYC2) e.g. used as internal PHY for ERTEC200
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_NSC)
#define EDDI_PHY_TRANSCEIVER_NSC            3   //NSC transceiver (PHY) e.g. used as external PHY for ERTEC400 and SOC1
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_TI)
#define EDDI_PHY_TRANSCEIVER_TI             4   //TI transceiver (PHY) (TLK111) e.g. used as external PHY for SOC1
#endif
#if defined (EDDI_CFG_PHY_TRANSCEIVER_USERSPEC)
#define EDDI_PHY_TRANSCEIVER_USERSPEC       5   //user specific transceiver (PHY)
#endif
#define EDDI_PHY_TRANSCEIVER_MAX            6
#define EDDI_PHY_TRANSCEIVER_NOT_DEFINED    255

typedef struct _EDDI_BOOTLOADER_TYPE
{
    LSA_UINT8                  Activate_for_use_as_pci_slave; /* deactivates the ERTEC400 bootloader, if no firmware is to run on the ARM9. EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE  */

} EDDI_BOOTLOADER_TYPE;

#define EDDI_PORT_NOT_CONNECTED   0xFFFFFFFFUL
//see eddi_bsp_ini.c -> bsp_eddi_system_dev_open

typedef struct _EDDI_PORTMAP_TYPE
{
    LSA_UINT32            PortCnt; //Nr of ports contained in PMTable
    LSA_UINT32            UsrPortID_x_to_HWPort_y[EDD_CFG_MAX_PORT_CNT]; //index = UserPortIndex

} EDDI_PORTMAP_TYPE;

//SII configuration parameters
#define EDDI_SII_IRQ_SP         1UL
#define EDDI_SII_IRQ_HP         2UL
#define EDDI_SII_IRQ_0          1UL
#define EDDI_SII_IRQ_1          2UL
#define EDDI_SII_STANDARD_MODE  1UL
#define EDDI_SII_POLLING_MODE   2UL

//I2C configuration parameters
#define EDDI_I2C_TYPE_NONE      0UL
#define EDDI_I2C_TYPE_GPIO      1UL
#define EDDI_I2C_TYPE_SOC1_HW   2UL

/* defines for HSYNC role */

#define EDDI_HSYNC_ROLE_NONE                     1           /*    HSYNC neither   */
#define EDDI_HSYNC_ROLE_FORWARDER                2           /*    HSYNC Forwarder   */
#define EDDI_HSYNC_ROLE_APPL_SUPPORT             3           /*    HSYNC Application */


typedef struct _EDDI_DPB_FEATURE_TYPE
{
    LSA_UINT8     bMRPDSupported;               // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8     FragmentationtypeSupported;   // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8     IRTForwardingModeSupported;   // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT16    MaxDFPFrames;
    LSA_UINT8     AdditionalForwardingRulesSupported;
    LSA_UINT8     ApplicationExist;
    LSA_UINT8     ClusterIPSupport;
    LSA_UINT8     MRPInterconnFwdRulesSupported;    // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8     MRPInterconnOriginatorSupported;  // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
} EDDI_DPB_FEATURE_TYPE;

typedef struct _EDDI_DPB_TYPE
{
    LSA_UINT32              InterfaceID;                //Interface ID
    LSA_UINT32              TraceIdx;                   //TraceIdx ID
    LSA_UINT32              IRTE_SWI_BaseAdr;           //Base of Reg and KRAM
    LSA_UINT32              IRTE_SWI_BaseAdr_LBU_16Bit; //ERTEC 200 Base of Reg and KRAM for 16 Bit access via the LBU interface
    #if !defined (EDDI_CFG_3BIF_2PROC)
    LSA_UINT32              GSharedRAM_BaseAdr;         //Base Addr of 2proc shared mem (virt.)
    #endif
    LSA_UINT32              NRTMEM_LowerLimit;          //Lower Limit of NRT-Memory (virt.)
    LSA_UINT32              NRTMEM_UpperLimit;          //Upper Limit of NRT-Memory (virt.)
    LSA_UINT32              BOARD_SDRAM_BaseAddr;       //Base of SDRAM, needed for bootloader-operations
    LSA_UINT32              ERTECx00_SCRB_BaseAddr;
    LSA_UINT32              PAEA_BaseAdr;               //Base-address of the PAEA-RAM, when using IOC on SOC
    LSA_UINT8               EnableReset;                //Use Case ERTEC is already initialized
    LSA_UINT8               bSupportIRTflex;            //Accept(EDD_FEATURE_ENABLE)/Reject(EDD_FEATURE_DISABLE) a parametrization containing IRTFlex
    LSA_UINT8               bDisableMaxPortDelayCheck;  //disable(EDDI_DPB_DISABLE_MAX_PORTDELAY_CHECK) the check of MaxPort(T/R)xDelay against PDIRGlobalData.MaxPort(T/R)xDelay. Needed for some old devices
    EDDI_KRAM_TYPE          KRam;
    EDDI_BOOTLOADER_TYPE    Bootloader;
    EDDI_PORTMAP_TYPE       PortMap;
    LSA_UINT16              MaxInterfaceCntOfAllEDD;
    LSA_UINT16              MaxPortCntOfAllEDD;

    //SII configuration parameters
    LSA_UINT32              SII_IrqSelector;            //EDDI_SII_IRQ_SP or EDDI_SII_IRQ_HP
    LSA_UINT32              SII_IrqNumber;              //EDDI_SII_IRQ_0  or EDDI_SII_IRQ_1
    LSA_UINT32              SII_ExtTimerInterval;       //0, 250, 500, 1000 in us
    LSA_UINT32              SII_Mode;                   //EDDI_SII_STANDARD_MODE or EDDI_SII_POLLING_MODE

    //I2C configuration parameters
    LSA_UINT32              I2C_Type;                   //EDDI_I2C_TYPE_NONE or EDDI_I2C_TYPE_GPIO or EDDI_I2C_TYPE_SOC1_HW

    EDDI_DPB_FEATURE_TYPE   FeatureSupport;

} EDDI_DPB_TYPE;

typedef struct _EDDI_DPB_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_DPB_PTR_TYPE;

#define EDDI_DPB_ENABLE_MAX_PORTDELAY_CHECK     0
#define EDDI_DPB_DISABLE_MAX_PORTDELAY_CHECK    1

/*--------------------------------------------------------------------------*/
/* Optimizations for zero-copy-interface:                                   */
/*--------------------------------------------------------------------------*/
/* RxBufferSize and RxBufferOffset are used for allocating receive-buffers  */
/* inside EDDI. RxBufferSize is the size of the whole buffer allocated.     */
/* RxBufferOffset is the offset from start where the frame will be put.     */
/* This is also the pointer which will be returned to the user when a frame */
/* is returned! (so there is RxBufferOffset free room before this address!) */
/* Note: RxBufferSize-RxBufferOffset >= EDD_FRAME_BUFFER_LENGTH             */
/* Note: When deallocating rx-buffers you must subtract RxBufferOffset from */
/*       the frame-buffer pointer!                                          */
/* Note: The edd does not allocate  or frees any transmitbuffers exchanged  */
/*       whith the user.                                                    */
/* Note: RxBufferOffset must be a multiple of the rx-buffer-alignment       */
/*       (i.e. a multiple of (EDDI_RX_FRAME_BUFFER_ALIGN+1)                 */
/*--------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_DDB_INI_TYPE
{
    EDDI_SYS_HANDLE          hSysDev;   //UpperDeviceHandle //DeviceOpen input parameter
    EDDI_HANDLE              hDDB;      //LowerDeviceHandle //DeviceOpen output parameter
    EDDI_UPPER_DPB_PTR_TYPE  pDPB;
    LSA_VOID                 LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);

} EDDI_RQB_DDB_INI_TYPE;

typedef struct _EDDI_RQB_DDB_INI_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_DDB_INI_PTR_TYPE;

/*===========================================================================*/
/*=================  SETUP/CLOSE DEVICE           ===========================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_SETUP                      */
/*---------------------------------------------------------------------------*/

/*====== EDDI_SER10_INTERNAL PRIOx ======*/
#define EDDI_NRT_PRIO_0_TAG_1_2      0      /* Prio0        */
#define EDDI_NRT_PRIO_1_TAG_0_3      1      /* Prio1        */
#define EDDI_NRT_PRIO_2_TAG_4_5      2      /* Prio2        */
#define EDDI_NRT_PRIO_3_TAG_6_7      3      /* Prio3        */
#define EDDI_NRT_PRIO_4_aSRTLow      4      /* Prio4        */
#define EDDI_NRT_PRIO_5_cSRT         5      /* Prio5        */
#define EDDI_NRT_PRIO_6_aSRTHigh     6      /* Prio6        */
#define EDDI_NRT_PRIO_7_iSRT         7      /* Prio7        */
#define EDDI_NRT_PRIO_8_ORG          8      /* Prio8        */
#define EDDI_NRT_PRIO_9_Fluss        9      /* Prio9        */

#define EDDI_NRT_MAX_PRIO            10     /* max. Prio    */

/*====== EDDI_ETH_DEF_VLAN_TAG_TYPE ======*/
typedef struct _EDDI_ETH_DEF_VLAN_TAG_TYPE
{
    LSA_UINT16          Address;            /* 12-Bit, Default-VLAN-Address */
    LSA_UINT8           Prio;               /*  3-Bit, Default-Prio         */

} EDDI_ETH_DEF_VLAN_TAG_TYPE;

/*---------------------------------------------------------------------------*/
/* Delay-Time MAC and PHY                                                    */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_PHY_DELAY_PARAMS_TYPE
{
    LSA_UINT32                  PortTxDelay;
    LSA_UINT32                  PortRxDelay;
    LSA_UINT32                  MaxPortTxDelay;     //IEC 61158 PTCP_PortTxDelay
    LSA_UINT32                  MaxPortRxDelay;     //IEC 61158 PTCP_PortRxDelay                                 

} EDDI_PHY_DELAY_PARAMS_TYPE;


typedef struct _EDDI_PORT_PARAMS_TYPE
{
    LSA_UINT16                  PhyAdr;             //Address of the PHY
    LSA_UINT8                   MediaType;          //Relevant in DDB, irrelevant in DSB!
    LSA_UINT8                   IsPOF;              //Relevant in DDB, irrelevant in DSB!
    LSA_UINT8                   FXTransceiverType;  //Relevant in DDB, irrelevant in DSB!

    EDDI_PHY_TRANSCEIVER_TYPE   PhyTransceiver;     //PHY transceiver (BROADCOM, NEC, etc.). Relevant in DDB, irrelevant in DSB!
    LSA_UINT8                   PhySupportsHotPlugging;  //Phy supports pulling and plugging. Relevant in DDB, irrelevant in DSB!
    EDDI_PHY_DELAY_PARAMS_TYPE  DelayParams;        //Relevant in DDB, irrelevant in DSB!

    LSA_UINT32                  AutonegCapAdvertised;
    LSA_UINT32                  AutonegMappingCapability;   //value will be adjusted for autonegotiation (for different mediums)

    EDD_MAC_ADR_TYPE            MACAddress;         //Portspecific MAC-Addresses                              
    LSA_UINT8                   MRPRingPort;        //EDD_MRP_NO_RING_PORT / EDD_MRP_RING_PORT / EDD_MRP_RING_PORT_DEFAULT
                                                    //If MRP is used two Ports must be EDD_MRP_NO_RING_PORT!
    LSA_UINT8                   SupportsMRPInterconnPortConfig;  //Only EDD_SUPPORTS_MRP_INTERCONN_PORT_CONFIG_NO allowed
    LSA_UINT8                   IsWireless;         //EDD_PORT_IS_WIRELESS:     This is a  wireless Port.                    
                                                    //EDD_PORT_IS_NOT_WIRELESS: This is no wireless Port.                          
    LSA_UINT8                   IsMDIXDSB;          //Relevant in DDB, irrelevant in DSB
    LSA_UINT8                   IsMDIX;             //EDD_PORT_MDIX_ENABLED:  If Auto-Neg is off; Rx and Tx lines will be crossed 
                                                    //EDD_PORT_MDIX_DISABLED: If Auto-Neg is off; Rx and Tx lines will not be crossed 

    LSA_UINT8                   IsPulled;           //Defines whether this port is pulled or plugged
                                                    //EDD_PORTMODULE_IS_PULLED:  Port is  pulled.
                                                    //          Allowed for ports with NSC transceiver only!
                                                    //EDD_PORTMODULE_IS_PLUGGED:  Port is plugged.
    LSA_UINT8                   bBC5221_MCModeSet;  //EDDI_PORT_BC5221_MEDIACONV_STRAPPED_HIGH: Pin “MEDIA_CONV#” of a BC5221 is strapped to 1.
                                                    //EDDI_PORT_BC5221_MEDIACONV_STRAPPED_LOW : Pin “MEDIA_CONV#” of a BC5221 is strapped to 0.
} EDDI_PORT_PARAMS_TYPE;

#define EDDI_PhyAdr_NOT_CONNECTED   0xFFFFUL
#define EDDI_PORT_BC5221_MEDIACONV_STRAPPED_HIGH    0
#define EDDI_PORT_BC5221_MEDIACONV_STRAPPED_LOW     1

/*====== EDDI_XRT_PRIO_TYPE ======*/
typedef struct _EDDI_XRT_ADDR_TYPE
{
    EDD_MAC_ADR_TYPE        MACAddressSrc;  //Local MACAddr for CH A and SrcMACAddr for xRT telegrams

} EDDI_XRT_ADDR_TYPE;


typedef struct _EDDI_GLOB_PARA_TYPE
{
    EDDI_XRT_ADDR_TYPE          xRT;
    EDDI_PORT_PARAMS_TYPE       PortParams[EDD_CFG_MAX_PORT_CNT]; //index = UserPortIndex
    EDDI_PHY_DELAY_PARAMS_TYPE  DelayParamsCopper[EDDI_PHY_TRANSCEIVER_MAX];
    EDDI_PHY_DELAY_PARAMS_TYPE  DelayParamsFX    [EDD_FX_TRANSCEIVER_MAX ][EDDI_PHY_TRANSCEIVER_MAX];
    LSA_UINT32                  MaxBridgeDelay;
    LSA_UINT8                   MaxMRP_Instances;
    LSA_UINT8                   MRPDefaultRoleInstance0;  //default-role for startup (EDD_MRP_ROLE_NONE, _CLIENT, _MANAGER), can be overwritten by record 0x8052
    LSA_UINT8                   MRPSupportedRole;         //role capability: EDD_MRP_ROLE_CAP_CLIENT / EDD_MRP_ROLE_CAP_MANAGER / EDD_MRP_ROLE_CAP_AUTOMANAGER or both or 0
    LSA_UINT8                   MRPSupportedMultipleRole;
    LSA_UINT8                   MRAEnableLegacyMode;      //MRA2.0 support: EDD_MRA_ENABLE_LEGACY_MODE/EDD_MRA_DISABLE_LEGACY_MODE
    LSA_UINT8                   MaxMRPInterconn_Instances;
    LSA_UINT8                   SupportedMRPInterconnRole;
} EDDI_GLOB_PARA_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_SET_FDB_ENTRY           */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE;

/*======= EDDI_SWI_PARA_TYPE =======*/
typedef struct _EDDI_SWI_PARA_TYPE
{
    LSA_UINT32                              Sys_StatFDB_CntEntry;                   /* Nr of static FDB entries parametrized by system adaption  */
    EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE   pSys_StatFDB_CntEntry;                  /* Ptr to array of static FDB entries                        */
    #if defined (EDDI_CFG_LEAVE_IRTCTRL_UNCHANGED)
    LSA_UINT8                               bResetIRTCtrl;                          /* EDD_FEATURE_ENABLE: reset IRTCtrl when resetting the switch-module */
    #endif
    LSA_UINT8                               Multicast_Bridge_IEEE802RSTP_Forward;   /* MAC:01-80-C2-00-00-00, setting for bridging (filter/forward) */
                                                                                    /* Default (EDD_FEATURE_DISABLE): filter,EDD_FEATURE_ENABLE:forward                */
} EDDI_SWI_PARA_TYPE;

typedef struct _EDDI_NRT_PARA_TYPE
{
    LSA_UINT16          NRT_FCW_Count;      /* Nr of free NRT-FCWs */
    LSA_UINT16          NRT_DB_Count;       /* Nr of free NRT-DBs */

    LSA_UINT16          NRT_DB_Limit_Down;  /* Lower threshold: minimum nr of DBs, that have to be provided */
    LSA_UINT16          NRT_FCW_Limit_Down; /* Lower threshold: minimum nr of FCWs, that have to be provided */

    LSA_UINT16          HOL_Limit_CH_Up;    /* Upper threshold: Maximum nr of DBs enqueued in channels A and B */
    LSA_UINT16          HOL_Limit_Port_Up;  /* Upper threshold: Maximum nr of DBs enqueued in Ports 0..3 */

} EDDI_NRT_PARA_TYPE;

typedef struct _EDDI_SRT_PARA_TYPE
{
    LSA_UINT16  SRT_FCW_Count;      // Count of lower SRT-FCWs (needed for Sending SRT-Telegrams)

} EDDI_SRT_PARA_TYPE;

/*===========================================================================*/

/*====== EDDI_DSB_TYPE ======*/
typedef struct _EDDI_DSB_TYPE
{
    EDDI_GLOB_PARA_TYPE             GlobPara;
    EDDI_SWI_PARA_TYPE              SWIPara;
    EDDI_SRT_PARA_TYPE              SRTPara;                     // IN OUT
    EDDI_NRT_PARA_TYPE              NRTPara;                     // IN OUT
    LSA_UINT32                      free_internal_KRAM_buffer;   // OUT
    LSA_UINT32                      Ticks_till_PhyReset;         // OUT

} EDDI_DSB_TYPE;

typedef struct _EDDI_DSB_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_DSB_PTR_TYPE;

/* defines for LinkSpeedMode                */
/* should be used for all hardware variants */

typedef struct _EDDI_RQB_DDB_SETUP_TYPE
{
    EDDI_HANDLE              hDDB;
    EDDI_UPPER_DSB_PTR_TYPE  pDSB;
    LSA_VOID                 LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);

} EDDI_RQB_DDB_SETUP_TYPE;

typedef struct _EDDI_RQB_DDB_SETUP_TYPE  EDD_UPPER_MEM_ATTR *    EDDI_UPPER_DDB_SETUP_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_CLOSE                      */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_RQB_DDB_REL_TYPE
{
    EDDI_HANDLE             hDDB;
    LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);

} EDDI_RQB_DDB_REL_TYPE;

typedef struct _EDDI_RQB_DDB_REL_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_DDB_REL_PTR_TYPE;

/*===========================================================================*/
/*=========================  INI COMPONENTS  ================================*/
/*===========================================================================*/

/*====== EDDI_MEMORY_MODE_TYPE ======*/
typedef enum _EDDI_MEMORY_MODE_TYPE
{
    //buffer must be necessarily allocated with EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM
    MEMORY_SDRAM_ERTEC = 8,     //local memory on the board

    //buffer must be necessarily allocated with EDDI_ALLOC_DEV_SHARED_MEM
    MEMORY_SHARED_MEM           //special usecase PCI board CP1616

} EDDI_MEMORY_MODE_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_COMP_NRT_INI               */
/*---------------------------------------------------------------------------*/

/* defines for NRT-Interface */
#define EDDI_NRT_CHA_IF_0       0UL
#define EDDI_NRT_CHB_IF_0       1UL
#define EDDI_NRT_IF_CNT         2UL
#define EDDI_NRT_NO_IF          0xFFFFFFFFUL

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_COMP_SW_INI                */
/*---------------------------------------------------------------------------*/

#define EDDI_USERMEMID_TYPE     void *

typedef struct _EDDI_NRT_IF_INI_TYPE
{
    EDDI_MEMORY_MODE_TYPE   MemModeBuffer;    /* Memory-Mode: SDRAM, PC-Shared-Memory */
    EDDI_MEMORY_MODE_TYPE   MemModeDMACW;     /* Memory-Mode: SDRAM, PC-Shared-Memory */

    EDDI_USERMEMID_TYPE     UserMemIDDMACWDefault;
    EDDI_USERMEMID_TYPE     UserMemIDTXDefault;
    EDDI_USERMEMID_TYPE     UserMemIDRXDefault;
    EDDI_USERMEMID_TYPE     UserMemIDTXLowFrag;
    EDDI_USERMEMID_TYPE     UserMemIDTXMidFrag;
    EDDI_USERMEMID_TYPE     UserMemIDTXHighFrag;
    EDDI_USERMEMID_TYPE     UserMemIDTXMgmtLowFrag;
    EDDI_USERMEMID_TYPE     UserMemIDTXMgmtHighFrag;

    LSA_UINT32              TxCntDscr;        /* Count of DSCRs for the Tx-NRT-IF */
    LSA_UINT32              RxCntDscr;        /* Count of DSCRs for the Rx-NRT-IF */

    LSA_UINT32              MaxRcvFrame_SendToUser;   /* Receive load limit */
    LSA_UINT32              MaxRcvFrame_ToLookUp;     /* Receive load limit */
    LSA_UINT32              MaxRcvFragments_ToCopy;   /* Receive load limit to copy */

    LSA_UINT8               RxFilterUDP_Unicast;     /* Unicast Whitelist */
    LSA_UINT8               RxFilterUDP_Broadcast;   /* Broadcast Whitelist */
    LSA_UINT8               RxFilterIFMACAddr;       /* Own IFMAC address */
} EDDI_NRT_IF_INI_TYPE;


typedef struct _EDDI_RQB_CMP_NRT_INI_TYPE
{
    union
    {
        struct
        {
            EDDI_NRT_IF_INI_TYPE    A__0;
            EDDI_NRT_IF_INI_TYPE    B__0;
        } IF;

        EDDI_NRT_IF_INI_TYPE        Interface[EDDI_NRT_IF_CNT];
    } u;
    LSA_UINT8                       bFeedInLoadLimitationActive;
    LSA_UINT8                       bIO_Configured;     

} EDDI_RQB_CMP_NRT_INI_TYPE;

typedef struct _EDDI_RQB_CMP_NRT_INI_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_CMP_NRT_INI_PTR_TYPE;

#define  EDDI_LOW_WATER_INT_MASK_REG_HOST_PROC_IRQ1        3
#define  EDDI_LOW_WATER_INT_MASK_REG_HOST_PROC_IRQ0        4
#define  EDDI_LOW_WATER_INT_MASK_REG_SWITCH_PROC_IRQ1      5
#define  EDDI_LOW_WATER_INT_MASK_REG_SWITCH_PROC_IRQ0      6

typedef struct _EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE
{
    struct
    {
        LSA_UINT8           bActivate;              //        0 -> Workaround is disabled
        LSA_UINT8           bReplaceDefaultSettings;
        LSA_UINT32          Tp_RcvFrame_InMs;       // Polltimeout for received franes
        LSA_UINT16          Diff_ESD;               // Max nr of ESD disturbances per Tp_ESD_InMs
        LSA_UINT32          T_LinkDown_NoRcv_InMs;  // Timeout in ms (if no more frames are received), after that a linkdown is forced
                                                    // and no ESD disturbances occur. 0: Feature disabled
    } MysticCom;

} EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE;

typedef struct _EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDI_RQB_CMP_PHY_BUGFIX_INI_PTR_TYPE;

/* ProviderGroupCnt                                                          */
/* Max Numer of ProviderGroups                                               */
/* Range: >1                                                                 */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDD_SRV_DEV_COMP_SRT_INI                */
/*---------------------------------------------------------------------------*/

/* cyclic SRT init-Structure */

typedef struct _EDDI_SRT_INI_TYPE
{
    LSA_UINT16      ConsumerFrameIDBaseClass1;
    LSA_UINT16      ConsumerFrameIDBaseClass2;

    LSA_UINT16      ConsumerCntClass12; // Sum parameter

    LSA_UINT16      ProviderCnt;

} EDDI_SRT_INI_TYPE;

typedef struct _EDDI_SRT_INI_TYPE   EDD_UPPER_MEM_ATTR *    EDDI_SRT_INI_PTR_TYPE;

/* ConsumerFrameIDBase has to be >= EDD_SRT_FRAMEID_CSRT_START and           */
/* ConsumerFrameIDBase+ConsumerCnt has to be <= EDD_SRT_FRAMEID_CSRT_STOP    */

/* ConsumerFrameIDBase                                                       */
/* Start of FrameID-Range valid for Consumers.                               */

/* ConsumerCnt                                                               */
/* Max Consumer  supported. (will also set max FrameID valid for consumer)   */
/* Range: 1..(EDDI_SRT_CONSUMER_FRAMEID_CSRT_STOP-ConsumerFrameIDBase-1)     */

/* ProviderCnt                                                               */
/* Max Provider supported.                                                   */
/* Range: 1..EDDI_SRT_PROVIDER_CNT_MAX                                       */

#define EDDI_SRT_PROVIDER_CNT_MAX                0x7FFF   /* < 0xFFFF ! */

/* ConsumerCheckFactor                                                       */
/* Check factor for consumer frames based on consumers cycle time            */

#define EDDI_SRT_CONSUMER_CHECK_FACTOR_MIN       1
#define EDDI_SRT_CONSUMER_CHECK_FACTOR_MAX       255

/* ProviderCycleDropCnt for SRT-EDD Only                                     */
/* Number of provider cycles to drop on machine overload (if we can not send */
/* all providers within cycle.                                               */
/* 0: no drops                                                               */

#define EDDI_SRT_PROVIDER_CYCLE_DROP_MAX         255

/* ProviderCylcePhaseMaxByteCnt                                              */
/* Max number of i/o-bytes for all providers within one knot.                */

/* ProviderGroupCnt                                                          */
/* Max Numer of ProviderGroups                                               */
/* Range: >1                                                                 */

/* IRT init-Structure */

typedef struct _EDDI_IRT_INI_TYPE
{
    LSA_UINT16              ConsumerCntClass3;
    LSA_UINT16              ProviderCnt;
    LSA_UINT16              ForwarderCnt;

} EDDI_IRT_INI_TYPE;

/* ConsumerCntClass3      Maximum of allowed RT_CLASS_3-Consumer              */
/* ProviderCnt            Maximum of allowed RT_CLASS_3-Provider              */
/* ForwarderCnt           Maximum of pure    RT_CLASS_3-Forwarder             */
/*                        If not all RT_CLASS_3-Consumer are used, the rest   */
/*                        of them is used for additional RT_CLASS_3-Forwarders*/

typedef struct _EDDI_IRT_INI_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_IRT_INI_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_SYSTEM   Service: EDDI_SRV_DEV_COMP_CRT_INI               */
/*---------------------------------------------------------------------------*/

#define EDDI_SYNC_TRIGMODE_IRT_END         0x0 /* for EDD-internal use only !*/
#define EDDI_SYNC_TRIGMODE_ISRT_END        0x1 /* for EDD-internal use only !*/
#define EDDI_SYNC_TRIGMODE_DEADLINE        0x2 /* for EDD-internal use only !*/
#define EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC  0x3 /* for EDD-internal use only !*/

/************************************************************************
*
* EDDI_CRT_SYNC_IMAGE_TYPE
*
* TriggerMode:      Defines which conditions trigger the
*                   StartOp-Interrupt:
*
*                   EDDI_SYNC_TRIGMODE_IRT_END:        StartOp will be triggered when the
*                                                      IRT-Transfer-Phase is finished
*                                                      but not later than TriggerDeadline is reached.
*
*                   EDDI_SYNC_TRIGMODE_ISRT_END:       StartOp will be triggered when the
*                                                      iSRT-Transfer-Phase is finished
*                                                      but not later than TriggerDeadline is reached.
*                                                      --> typical mode for synchronized devices
*
*                   EDDI_SYNC_TRIGMODE_DEADLINE:       Only TriggerDeadline causes the StartOp
*
*                   EDDI_SYNC_TRIGMODE_DEADLINE_NO_TC: Only TriggerDeadline causes the StartOp
*                                                      Unlike the other modes no transfer control
*                                                      will be made here. That is the StartOp-Interrupt
*                                                      will be fired immediately - irrespective of
*                                                      a running IRT-Send/Receive-Task.
*
*                   REMARK: There is no interface in eddi_usr.h which will provide the
*                   StartOp-Interrupt to the user ! Instead that must be handled via
*                   system-adaption.
*
* TriggerDeadlineIn10ns:  Deadline in Ticks (10ns) from the begin of the Cycle
*                   used to define the trigger-condition of the
*                   StartOP-Interrupt.
*                   (see TriggerMode)
*
***********************************************************************/

// Parameters for the Synchronized-Image (typically needed by IRT-Users).
// Pure SRT-Users wich do not need IRT or SyncImages should set all params
// to zero and BufferMode to EDDI_SYNC_IMAGE_OFF.
typedef struct _EDDI_CRT_SYNC_IMAGE_EXPERT_TYPE
{
    LSA_UINT32              TriggerDeadlineIn10ns;                        //USER: don´t care, used internally only!
    LSA_UINT16              TriggerMode;                                  //USER: don´t care, used internally only! see EDDI_SYNC_TRIGMODE_xx above

} EDDI_CRT_SYNC_IMAGE_EXPERT_TYPE;

typedef struct _EDDI_CRT_SYNC_IMAGE_TYPE
{
    EDDI_CRT_SYNC_IMAGE_EXPERT_TYPE   ImageExpert;                         //USER: don´t care, used internally only!

} EDDI_CRT_SYNC_IMAGE_TYPE;

/************************************************************************
*
* EDDI_CRT_INI_TYPE
*
* CycleBaseFactor:     Defines the length of the cycle in entities of 31.25 us.
*
*                      Allowed Values:     4      8    16   32   64  128 ... 0x8000
*                      Period          125us  250us 500us  1ms  2ms  4ms ... 1024ms
*
***********************************************************************/

typedef struct _EDDI_CRT_INI_TYPE
{
    LSA_UINT8                 bUseTransferEnd;    // EDD_FEATURE_ENABLE: IRT_COMP_TIME gets initialized for TransferEnd-Int

    // see above
    EDDI_IRT_INI_TYPE         IRT;
    EDDI_SRT_INI_TYPE         SRT;

} EDDI_CRT_INI_TYPE;

typedef struct _EDDI_CRT_INI_TYPE   EDD_UPPER_MEM_ATTR *    EDDI_CRT_INI_PTR_TYPE;

typedef struct _EDDI_CYCLE_COMP_INI_TYPE
{
    LSA_UINT16    ClkCy_Period;
    LSA_UINT16    ClkCy_Int_Delay;
    LSA_UINT16    ClkCy_Int_Divider;
    LSA_UINT16    ClkCy_Delay;
    LSA_UINT16    AplClk_Divider;
    LSA_UINT8     CompVal1Int;  //
    LSA_UINT8     CompVal2Int;  //
    LSA_UINT8     CompVal3Int;  //
    LSA_UINT8     CompVal4Int;  //
    LSA_UINT8     CompVal5Int;  //
    LSA_UINT32    CompVal1;     //
    LSA_UINT32    CompVal2;     //
    LSA_UINT32    CompVal3;     //
    LSA_UINT32    CompVal4;     //
    LSA_UINT32    CompVal5;     //
    LSA_UINT16    PDControlPllMode;        /* determine synchronize signal mode */

} EDDI_CYCLE_COMP_INI_TYPE;

typedef struct _EDDI_CYCLE_COMP_INI_TYPE  EDD_UPPER_MEM_ATTR *    EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE;

/*===========================================================================*/

typedef struct _EDDI_RQB_COMP_INI_TYPE
{
    EDDI_HANDLE                          hDDB;
    LSA_VOID                             LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);
    EDDI_RQB_CMP_NRT_INI_TYPE            NRT;
    EDDI_RQB_CMP_PHY_BUGFIX_INI_TYPE     PhyBugfix;
    EDDI_CRT_INI_TYPE                    CRT;
    EDDI_CYCLE_COMP_INI_TYPE             CycleComp; 

} EDDI_RQB_COMP_INI_TYPE;

typedef struct _EDDI_RQB_COMP_INI_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_CMP_INI_PTR_TYPE;


/* For controlling the signals XPLL_Ext_Out and XPLL_EXT_IN to synchronize, following steps are necessary                     */
/* EDDI_CYCLE_INI_PLL_MODE_OFF       or EDDI_CYCLE_INI_PLL_MODE_XPLL_EXT_OUT     must be selected                             */

/* During the ERTEC is running the register AplClk_Divider will adjust to the CycleLength, the register ClkCy_Period is fix   */
/* The phase of the synchronize signal XPLL_Ext_Out -> T(XPLL_Ext_Out) = T(Register[ClkCy_Period]) * Register[AplClk_Divider] */
/* The register ClkCy_Period will be set on 3125 and register AplClk_Divider on CycleBaseFactor                               */

/* PDControlPllMode */
#define  EDDI_CYCLE_INI_PLL_MODE_NOTUSED       0x00  /* synchronize signal XPLL_Ext_Out and XPLL_EXT_IN !are not used! */
#define  EDDI_CYCLE_INI_PLL_MODE_OFF           0x01  /* synchronize signal XPLL_Ext_Out and XPLL_EXT_IN are disabled   */
#define  EDDI_CYCLE_INI_PLL_MODE_XPLL_EXT_OUT  0x02  /* synchronize signal XPLL_Ext_Out is enabled                     */

#define  EDDI_PRM_DEF_PLL_MODE_OFF                               0x00         /* off */
#define  EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_OUT                      0x01         /* on, out */
#define  EDDI_PRM_DEF_PLL_MODE_XPLL_EXT_IN                       0x02         /* on, in */

/*====== EDDI_CYCLE_COMP_INI_TYPE ======*/

/*===========================================================================*/
/*================ GIS ( Global initialization structure ) ==================*/
/*===========================================================================*/
#define EDDI_GIS_USECASE_DEFAULT        0       //Default: no restrictions, buffercapacity is set to 500µs
#define EDDI_GIS_USECASE_IOC_SOC1_2P    1       //PLC with SOC1, 2 ports. Max 512 IODs, buffercapacity is set to 1000µs
#define EDDI_GIS_USECASE_IOC_SOC1_3P    2       //PLC with SOC1, 3 ports. Max 256 IODs, buffercapacity is set to 1000µs
#define EDDI_GIS_USECASE_IOD_ERTEC400   3       //IOD with ERTEC400 (Scalance). No IOC resources. Buffercapacity is set to 1400µs

#define EDDI_GIS_IOSPACE_IS_IN_PAEARAM  0
#define EDDI_GIS_IOSPACE_IS_IN_KRAM     1
typedef struct _EDDI_GLOBAL_IOSPACE_OUT_TYPE
{
    LSA_UINT8     bIsInKRAM;
    LSA_UINT16    PNIOC_MCInput;
    LSA_UINT16    PNIOC_MCOutput;
    LSA_UINT16    PNIODInput;
    LSA_UINT16    PNIODOutput;
    
} EDDI_GLOBAL_IOSPACE_OUT_TYPE;

typedef struct _EDDI_GEN_INI_TYPE
{
    LSA_UINT32   HWTypeUsed;
    LSA_UINT8    HWSubTypeUsed;
    LSA_UINT8    NrOfPorts;
    LSA_UINT8    bIRTSupported;
    LSA_UINT16   NrOfIRTForwarders;
    LSA_UINT16   MRPDefaultRoleInstance0;
    LSA_UINT8    MRPDefaultRingPort1;
    LSA_UINT8    MRPDefaultRingPort2;
    LSA_UINT8    HSYNCRole;
    LSA_UINT16   BufferCapacityUseCase;
    LSA_UINT32   I2C_Type; //EDDI_I2C_TYPE_NONE or EDDI_I2C_TYPE_GPIO or EDDI_I2C_TYPE_SOC1_HW

} EDDI_GEN_INI_TYPE;

typedef struct _EDDI_IOC_INI_TYPE
{
    LSA_UINT16    NrOfRTDevices;
    LSA_UINT16    NrOfIRTDevices;
    LSA_UINT16    NrOfRedIRTDevices;
    LSA_UINT16    NrOfQVProviders;
    LSA_UINT16    NrOfQVConsumers;
    LSA_UINT16    NrOfRedQVConsumers;
    LSA_UINT32    LogicalAddressSpace;
    LSA_UINT32    NrOfSubmod;
    LSA_UINT16    MaxIOCRDataSize;
    LSA_UINT16    MaxMCRDataSize;
    
} EDDI_IOC_INI_TYPE;

typedef struct _EDDI_IOD_INI_TYPE
{
    LSA_UINT16    NrOfInstances;
    LSA_UINT16    NrOfARs;
    LSA_UINT16    MaxOCRDataSize;
    LSA_UINT16    MaxICRDataSize;
    LSA_UINT16    MaxMCRDataSize;

    LSA_UINT8     AlternativeCalc;

    LSA_UINT16    NrOfSubmodules;
    LSA_UINT16    OutputBytes;
    LSA_UINT16    InputBytes;
    LSA_UINT16    SharedARSupported;
   
} EDDI_IOD_INI_TYPE;

typedef struct _EDDI_NRT_INI_TYPE
{
    LSA_UINT8   bMIIUsed;

} EDDI_NRT_INI_TYPE;

typedef struct _EDDI_GLOBAL_INI_TYPE
{
    EDDI_GEN_INI_TYPE             GenIni;
    EDDI_IOC_INI_TYPE             IOCIni;
    EDDI_IOD_INI_TYPE             IODIni;
    EDDI_NRT_INI_TYPE             NRTIni;
    LSA_UINT32                    KRAMUsed;    //OUT
    LSA_INT32                     KRAMFree;    //OUT
    LSA_UINT32                    BufferCapacity_us; //OUT
    EDDI_GLOBAL_IOSPACE_OUT_TYPE  IOSpace;     //OUT
    LSA_UINT32                    CRTSpace;    //OUT
    LSA_UINT32                    SRTSpace;    //OUT
    LSA_UINT32                    IRTSpace;    //OUT
    LSA_UINT32                    SWISpace;    //OUT
    LSA_UINT32                    NRTSpace;    //OUT
    LSA_VOID                   *  pInternalUse;
    LSA_UINT32                    InterfaceID;

} EDDI_GLOBAL_INI_TYPE;

typedef struct _EDDI_GLOBAL_INI_TYPE  EDD_UPPER_MEM_ATTR  *  EDDI_UPPER_GLOBAL_INI_PTR_TYPE;

//Use this size to allocate the GIS structure! 
#define EDDI_UPPER_GLOBAL_INI_SIZE  sizeof(EDDI_GLOBAL_INI_TYPE)

/*===========================================================================*/
/*                   CDB (Channel Description Block)                         */
/*                     (used by EDDI_GET_PATH_INFO)                          */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* CDB- Parameter for NRT                                                    */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_CDB_NRT_TYPE
{
    LSA_UINT32  Channel;              /* defines NRT-Frames to be received on this channel */
    LSA_UINT32  FrameFilter;          /* Filter */
    LSA_UINT8   UseFrameTriggerUnit;  /* from REV 6 on */

} EDDI_CDB_NRT_TYPE;

/* defines for Channel */
#define EDDI_NRT_CHANEL_A_IF_0   0x01L
#define EDDI_NRT_CHANEL_B_IF_0   0x02L

typedef EDDI_CDB_NRT_TYPE  EDD_UPPER_MEM_ATTR *  EDDI_UPPER_CDB_NRT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* CDB- Channel Description Block                                            */
/*      (used as detail-ptr in EDDI_GET_PATH_INFO)                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_COMP_USE_TYPE
{
    LSA_UINT8  UseNRT;          /*      EDD_CDB_CHANNEL_USE_ON: NRT-Component used by channel         */
    LSA_UINT8  UseXRT;          /*      EDD_CDB_CHANNEL_USE_ON: SRT IRT ICC Component used by channel */
    LSA_UINT8  UseSWITCH;       /*      EDD_CDB_CHANNEL_USE_ON: SWI-Component used by channel         */
    LSA_UINT8  UseSYNC;         /*      EDD_CDB_CHANNEL_USE_ON: SYNC Component used by channel        */
    LSA_UINT8  UsePRM;          /*      EDD_CDB_CHANNEL_USE_ON: PRM  Component used by channel        */

} EDDI_COMP_USE_TYPE;

typedef struct eddi_detail_tag
{
    EDDI_COMP_USE_TYPE             UsedComp;
    EDDI_HANDLE                    hDDB;
    LSA_UINT8                      InsertSrcMAC; /* EDD_CDB_INSERT_SRC_MAC_ENABLED: eddi inserts src-mac in send frame */
    EDDI_UPPER_CDB_NRT_PTR_TYPE    pNRT;
    LSA_UINT8                      Prm_PortDataAdjustLesserCheckQuality;

} EDDI_CDB_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SYNC_INDICATION_PROVIDE        */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_CYCLE_COUNTER_TYPE
{
    LSA_UINT32      High;
    LSA_UINT32      Low;

} EDDI_CYCLE_COUNTER_TYPE;

typedef struct _EDDI_CYCLE_COUNTER_TYPE  EDD_UPPER_MEM_ATTR * EDDI_CYCLE_COUNTER_PTR_TYPE;

/* predefined substitution of FrameFilter: */

#define EDDI_NRT_FRAME_IP (EDD_NRT_FRAME_IP_ICMP  |    \
                           EDD_NRT_FRAME_IP_TCP   |    \
                           EDD_NRT_FRAME_IP_UDP   |    \
                           EDD_NRT_FRAME_IP_VRRP  |    \
                           EDD_NRT_FRAME_ARP)

/*---------------------------------------------------------------------------*/
/* Prm_PortDataAdjustLesserCheckQuality:                                     */
/*                                                                           */
/*      !=0:   Do a lesser strict check (see table below)                    */
/*        0:  No special handling.                                           */
/*                                                                           */
/* PDPortDataAdjust checking rules:                                          */
/*                                                                           */
/* --------+--------------------+----------------------+-------------------- */
/*  PRM    |     IEC61158       |  LesserCheckQuality  |     Local-Flag      */
/* --------+--------------------+----------------------+-------------------- */
/*  WRITE  | PortState = OFF    |             PortState = OFF                */
/*         |                    |                 10 FD/HD                   */
/*         |      100 FD        |                100 FD/HD                   */
/*         |     1000 FD        |               1000 FD/HD                   */
/*         |    10000 FD        |              10000 FD/HD                   */
/*         |     Autoneg 1)     |                 Autoneg 1)                 */
/* --------+--------------------+----------------------+-------------------- */
/*  END    | At least one port has to be set to        |  no restrictions    */
/*         | Autoneg, 100 FD, 1000 FD or 10000 FD      |                     */
/* --------+-------------------------------------------+-------------------- */
/*                                                                           */
/* IEC61158           : Checking rule according to IEC61158.                 */
/* LesserCheckQuality : Rule if Prm_PortDataAdjustLesserCheckQuality is set  */
/* Local-Flag         : Rule if "Local"  flag set with PRM-request by CMPD.  */
/*                                                                           */
/*  1) Autoneg is used if no MAUType or PortState is present for this port.  */
/*     This is the default setting.                                          */
/*---------------------------------------------------------------------------*/

/*===========================================================================*/

/*====  in functions  =====*/

/*=============================================================================
 * function name:  eddi_init()
 *
 * function:       initialize EDD
 *
 * parameters:
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_PARAM
 *                              LSA_RET_ERR_RESOURCE
 *                              or others
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR eddi_init( LSA_VOID );

/*=============================================================================
 * function name:  eddi_undo_init()
 *
 * function:       undo the initialization of EDD
 *
 * parameters:     LSA_VOID
 *
 * return value:   LSA_RESULT:  LSA_RET_OK
 *                              possibly:
 *                              LSA_RET_ERR_SEQUENCE
 *                              or others
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_undo_init( LSA_VOID );

/*=============================================================================
 * function name:  eddi_timeout()
 *
 * function:       expiration of the running time of a timer
 *
 * parameters:     LSA_TIMER_ID_TYPE timer_id:  id of timer
 *                 LSA_USER_ID_TYPE   user_id:  id of prefix
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  eddi_timeout( LSA_TIMER_ID_TYPE  const timer_id,
                                                 LSA_USER_ID_TYPE   const user_id );

/*=============================================================================
 * function name:  eddi_interrupt()
 *
 * function:       EDDI-ISR-Handler called in isr
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt( const  EDDI_HANDLE  const  hDDB );

#if defined (EDDI_CFG_SII_POLLING_MODE)
/*=============================================================================
 * function name:  eddi_interrupt_low()
 *
 * function:       specific SII-interrupt-function for USER for low-context-polling.
 *                 e.g. this function can be used in an IDLE-task.
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_low( const  EDDI_HANDLE  const  hDDB );
#endif //EDDI_CFG_SII_POLLING_MODE

/*=============================================================================
 * function name:  eddi_ExtTimerInterrupt()
 *
 * function:       EDDI-ISR-Handler for external timer-interrupt called in isr
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_ExtTimerInterrupt( const  EDDI_HANDLE  const  hDDB );

/*=============================================================================
 * function name:  eddi_interrupt_set_eoi()
 *
 * function:       Set EOI in IRTE interrupt controller
 *
 * parameters:     ...
 *
 * return value:   LSA_VOID
 *===========================================================================*/
#if !defined (EDDI_CFG_SII_USE_SPECIAL_EOI)
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_set_eoi( const  EDDI_HANDLE  const  hDDB,
                                                         LSA_UINT32          const  EOIInactiveTime );
#endif

/*=============================================================================
 * function name:  eddi_interrupt_mask_user_int()
 *
 * function:       input-function for User-Int-Share-Mode only:
 *                 - only modifies selected USER-interrupts
 *                 - function is only allowed after Device-Open
 *
 * parameters:     ...
 *
 * return value:   LSA_RESULT:  EDD_STS_OK
 *                              EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_mask_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                 LSA_UINT32          const  IRTIrqIRTE,
                                                                 LSA_UINT32          const  NRTIrqIRTE );

/*=============================================================================
 * function name:  eddi_interrupt_unmask_user_int()
 *
 * function:       input-function for User-Int-Share-Mode only:
 *                 - only modifies selected USER-interrupts
 *                 - function is only allowed after Device-Open
 *
 * parameters:     ...
 *
 * return value:   LSA_RESULT:  EDD_STS_OK
 *                              EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_unmask_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                   LSA_UINT32          const  IRTIrqIRTE,
                                                                   LSA_UINT32          const  NRTIrqIRTE );

/*=============================================================================
 * function name:  eddi_interrupt_ack_user_int()
 *
 * function:       input-function for User-Int-Share-Mode only:
 *                 - only acknowledge selected USER-interrupts
 *                 - function is only allowed after Device-Open
 *
 * parameters:     ...
 *
 * return value:   LSA_RESULT:  EDD_STS_OK
 *                              EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_ack_user_int( const  EDDI_HANDLE  const  hDDB,
                                                                LSA_UINT32          const  IRTIrqIRTE,
                                                                LSA_UINT32          const  NRTIrqIRTE );

/*=============================================================================
 * function name:  eddi_interrupt_get_user_int()
 *
 * function:       input-function for User-Int-Share-Mode only:
 *                 - only reads unmasked USER-interrupts
 *                 - function is only allowed after Device-Open
 *
 * parameters:     ...
 *
 * return value:   LSA_RESULT:  EDD_STS_OK
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_get_user_int( const  EDDI_HANDLE     const  hDDB,
                                                                LSA_UINT32          *  const  pIRTIrqIRTE,
                                                                LSA_UINT32          *  const  pNRTIrqIRTE );


/***************************************************************************/
/* F u n c t i o n:       eddi_interrupt_set_nrt_rx_int()                  */
/*                                                                         */
/* D e s c r i p t i o n: Enable or disable selected NRT RX Interrupt:     */
/*                        - function is only allowed after Device-Open     */
/*                                                                         */
/* A r g u m e n t s:     hDDB:            Device handle                   */
/*                        NRTChannel:      EDDI_NRT_CHANEL_A_IF_0,         */
/*                                         EDDI_NRT_CHANEL_B_IF_0,         */
/*                        bEnableNrtRxInt:      !=0  = enable  interrupt   */
/*                                                0 = disable interrupt    */
/*                                                                         */
/* Return Value:          LSA_RESULT:      EDD_STS_OK                      */
/*                                         EDD_STS_ERR_PARAM               */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_interrupt_set_nrt_rx_int( const  EDDI_HANDLE  const  hDDB,
                                                                  LSA_UINT32          const  NRTChannel,
                                                                  LSA_UINT8           const  bEnableNrtRxInt );

#define EDDI_SII_DISABLE_NRT_RX_INT     0
#define EDDI_SII_ENABLE_NRT_RX_INT      1

#endif

/*=============================================================================
 * function name:  eddi_SRTProviderGetCycleCounter()
 *
 * function:       Get SRT-provider CycleCounter (31.25us ticks)
 *
 * parameters:     EDDI_HANDLE                  hDDB
 *                 EDDI_CYCLE_COUNTER_PTR_TYPE  pCycleCounter
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SRTProviderGetCycleCounter( EDDI_HANDLE                  hDDB,
                                                                      EDDI_CYCLE_COUNTER_PTR_TYPE  pCycleCounter );

typedef struct _EDDI_SYNC_GET_SET_TYPE
{
    LSA_UINT32      CycleBeginTimeStampInTicks; // Ticks (1ns or 10ns)
    LSA_UINT32      CycleCounterLow;     // 31,25 us
    LSA_UINT32      CycleCounterHigh;
    LSA_UINT32      PTCPSeconds;
    LSA_UINT32      PTCPNanoSeconds;    

} EDDI_SYNC_GET_SET_TYPE;

/*=============================================================================
 * function name:  eddi_SyncGet()
 *
 * function:       Get the consistent values of:
 *                 - timestamp
 *                 - CycleCounter
 *                 - ExtPllIn-Timestamp (only if no NIL-ptr!)
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_SyncGet( EDDI_HANDLE              const hDDB,
                                               EDDI_SYNC_GET_SET_TYPE * const pSyncGet,
                                               LSA_UINT32             * const pExtPLLTimeStampInTicks );

/*=============================================================================
 * function name:  eddi_SyncAdjust()
 *
 * function:
 *
 * parameters:     EDDI_HANDLE                  hDDB
 *                 EDDI_CYCLE_COUNTER_PTR_TYPE  pCycleCounter
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_SyncAdjust( EDDI_HANDLE const hDDB,
                                                  LSA_INT32   const AdjustIntervalNs );
/*
- stoppt die zyklischen Listen
- verriegelt den EDD: keine Interruptausführung, keine Anwenderfunktionen mehr moeglich !
 */
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_SetExternalException( EDDI_HANDLE const hDDB );

/*=============================================================================
 * function name:  eddi_GetCycleCounterLow()
 *
 * function:
 *
 * parameters:     EDDI_HANDLE                  hDDB
 *
 * return value:   CycleCounterLow
 *
 *
 *===========================================================================*/
LSA_UINT32 EDDI_SYSTEM_IN_FCT_ATTR  eddi_GetCycleCounterLow( EDDI_HANDLE const hDDB );

/*=============================================================================
 * function name:  eddi_GetTimeStamp()
 *
 * function:
 *
 * parameters:     EDDI_HANDLE                  hDDB
 *
 * return value:   current Value of ClockCountValue-Register
 *                 wich Counter is used to generate all TimeStamps on EDD
 *
 *
 *===========================================================================*/
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_GetTimeStamp( EDDI_HANDLE        const   hDDB,
                                                    LSA_UINT32      *  const   pTimeStamp );

/*=============================================================================
 * function name:  eddi_KRAMWriteLock()
 *
 * function:
 *
 * parameters:     LSA_UINT32                  InstanceHandle
 *                 LSA_UINT32                  offset
 *                 LSA_UINT32                  Length
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *                 EDD_STS_ERR_HW
 *                 EDD_STS_ERR_TIMEOUT
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteLock( LSA_UINT32 const InstanceHandle,
                                                        LSA_UINT32 const Offset,
                                                        LSA_UINT32 const Length );

/*=============================================================================
 * function name:  eddi_KRAMWriteUnLock()
 *
 * function:
 *
 * parameters:     LSA_UINT32                  InstanceHandle
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *                 EDD_STS_ERR_HW
 *                 EDD_STS_ERR_TIMEOUT
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMWriteUnLock( LSA_UINT32 const InstanceHandle );

/*=============================================================================
 * function name:  eddi_KRAMReadLock()
 *
 * function:
 *
 * parameters:     LSA_UINT32                  InstanceHandle
 *                 LSA_UINT32                  offset
 *                 LSA_UINT32                  Length
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *                 EDD_STS_ERR_HW
 *                 EDD_STS_ERR_TIMEOUT
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadLock (  LSA_UINT32 const InstanceHandle,
                                                         LSA_UINT32 const Offset,
                                                         LSA_UINT32 const Length );

/*=============================================================================
 * function name:  eddi_KRAMReadUnLock()
 *
 * function:
 *
 * parameters:     LSA_UINT32                  InstanceHandle
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *                 EDD_STS_ERR_HW
 *                 EDD_STS_ERR_TIMEOUT
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMReadUnLock ( LSA_UINT32 const InstanceHandle );

/***************************************************************************/
/* F u n c t i o n:       eddi_KRAMInOutDone()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_RESULT                                       */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_KRAMInOutDone ( LSA_UINT32 const InstanceHandle );


#if !defined (EDDI_CFG_3BIF_2PROC)
/*=============================================================================
 * function name: eddi_ProviderGetPrimaryAR()
 *
 * function:      Get the primary AR
 *
 * parameters:    hDDB            Device handle (see EDDI_SRV_DEV_OPEN service).
 *                ARSetID         ID of the ARSet from which the current AR in 
 *                                state PRIMARY shall be determined.
 *                pPrimaryARID    Pointer to where the returnvalue shall be written to.
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ProviderGetPrimaryAR( EDDI_HANDLE            hDDB,
                                                                LSA_UINT16      const  ARSetID,
                                                                LSA_UINT16   *  const  pPrimaryARID );
#endif //EDDI_CFG_3BIF_2PROC

/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Init()
 *
 * function:      Initialisation of the system for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange
 *
 * parameters:    -
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Init( LSA_VOID);


/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Deinit()
 *
 * function:      Deinitialisation of the system for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange
 *
 * parameters:    -
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Deinit( LSA_VOID);


/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_Setup()
 *
 * function:      Initialisation of the interface for controling the provider 
 *                DataStatus and for ERTEC200 Provider-buffer-exchange for one 
 *                instance
 *
 * parameters:    InstanceHandle  An instance handle unambiguously identifying 
 *                                the used device/interface throughout the whole 
 *                                system. It is advised to use the InterfaceID.
 *                TraceIdx        Trace index
 *                pKRAM           Pointer to the start of KRAM. This is a virtual 
 *                        address (as the user sees the KRAM).
 *                pIRTE           Pointer to the start of the IRTE-registers. 
 *                                This is a virtual address (as the user sees 
 *                                the IRTE-registers).
 *                pGSharedRAM     Base address of an area for storing commonly 
 *                                used variables. This is a virtual address 
 *                                (as the user sees this area).
 *                pPAEARAM        Don´t care. Reserved for later versions.
 *                MaxNrProviders  EDDI_IO_CFG_MAX_NR_PROVIDERS is set in eddi_cfg.h:
 *                                Don´t care. Internal control structures get allocated statically.
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_Setup ( LSA_UINT32    const   InstanceHandle,
                                                                     LSA_UINT32    const   TraceIdx,
                                                                     LSA_VOID    * const   pKRAM, 
                                                                     LSA_VOID    * const   pIRTE, 
                                                                     LSA_VOID    * const   pGSharedRAM, 
                                                                     LSA_VOID    * const   pPAEARAM,
                                                                     LSA_UINT32    const   offset_ProcessImageEnd,
                                                                     LSA_UINT32    const   PollTimeOut_ns,
                                                                     LSA_UINT16    const   MaxNrProviders);

/*=============================================================================
 * function name: eddi_SysRed_BufferRequest_UndoSetup()
 *
 * function:      Free instance resources
 *
 * parameters:    InstanceHandle  The instance handle.
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SysRed_BufferRequest_UndoSetup ( LSA_UINT32    const   InstanceHandle);

/*=============================================================================
 * function name: eddi_SysRed_Provider_Add()
 *
 * function:      Add a provider
 *
 * parameters:    InstanceHandle    The instance handle.
 *                pApplProviderID   Ptr to ApplProviderID (return value). 
 *                CWOffset          Offset of a controlword from the beginning of 
 *                                  KRAM. (ERTEC200 with 3BIF in SW only.)
 *                DataOffset        Offset to the beginning of KRAM of 3 adjacent
 *                                  buffers. (ERTEC200 with 3BIF in SW only.)
 *                DataStatusOffset  Offset of the DataStatus from the beginning 
 *                                  of KRAM. (!=ERTEC200 with 3BIF in SW only)
 *                GroupID           Don´t care. Reserved for later versions.
 *                ProvType          EDD_PROVADD_TYPE_... (see edd_usr.h)
 *                DataLen           Data length of 1 ProviderBuffer.
 *                                  (ERTEC200 with 3BIF in SW only) 
 *
 * return value:   EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_Provider_Add( LSA_UINT32     const  InstanceHandle,
                                                               LSA_UINT16  *  const  pApplProviderID,
                                                               LSA_UINT32     const  CWOffset,
                                                               LSA_UINT32     const  DataOffset,
                                                               LSA_UINT32     const  DataStatusOffset,
                                                               LSA_UINT16     const  GroupID,
                                                               LSA_UINT8      const  ProvType,
                                                               LSA_UINT16     const  DataLen );

/*=============================================================================
 * function name: eddi_SysRed_Provider_Remove()
 *
 * function:      Remove a provider
 *
 * parameters:    InstanceHandle  The instance handle.
 *                ApplProviderID  The provider-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_Provider_Remove( LSA_UINT32  const  InstanceHandle,
                                                                  LSA_UINT16  const  ApplProviderID );

/*=============================================================================
 * function name: eddi_SysRed_ProviderDataStatus_Set()
 *
 * function:      Set the provider specific DataStatus 
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID    The provider-ID
 *                Status            Data status value
 *                Mask              Mask for status bits that must be changed
 *                pOrderID          Pointer to an address to return the Order-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_ProviderDataStatus_Set(LSA_UINT32      const  InstanceHandle,
                                                                        LSA_UINT16      const  ApplProviderID,
                                                                        LSA_UINT8       const  Status,
                                                                        LSA_UINT8       const  Mask,
                                                                        LSA_UINT16    * const  pOrderID);

#if defined (EDDI_CFG_SYSRED_2PROC)
/*=============================================================================
 * function name: eddi_SysRed_ProviderDataStatus_Get()
 *
 * function:      Get the provider specific DataStatus and OrderID
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID    The provider-ID
 *                pStatus           Constant pointer to an address that will receive the value of the 
 *                                  Status parameter that has been written to the APDU_Status.DataStatus
 *                pOrderID          Pointer to an address to return the Order-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM, EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_ProviderDataStatus_Get(LSA_UINT32      const  InstanceHandle,
                                                                        LSA_UINT16      const  ApplProviderID,
                                                                        LSA_UINT8     * const  pStatus,
                                                                        LSA_UINT16    * const  pOrderID);

/*=============================================================================
 * function name: eddi_SysRed_Provider_MergeDS()
 *
 * function:      merges the application provided APDUStatus.DataStatus-Bits 
 *                from the Global Shared Memory into the real APDUStatus.DataStatus
 *                Has to be called from within the EDDI-context
 *
 * parameters:    EDDI_HANDLE       hDDB
 *
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_Provider_MergeDS( EDDI_HANDLE  const  hDDB);
#endif

/*=============================================================================
 * function name: EDDI_SysRedPoll()
 *
 * function:      
 *
 * parameters:    EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:  LSA_VOID
 *===========================================================================*/
#if defined (EDDI_CFG_SYSRED_2PROC)
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_SysRedPoll(  EDD_UPPER_RQB_PTR_TYPE  const  pRQB );
#endif


/*=============================================================================
* function name: eddi_SysRed_PrimaryAR_Set()
*
* function:      Set the provider specific Primary AR-ID 
*
* parameters:    InstanceHandle     The instance handle.
*                ARSetID            ID of the ARSet from which the current AR in 
*                                   state PRIMARY shall be determined.
*                PrimaryARID        ARID of the AR being the current primary AR. 
*
* return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
*===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysRed_PrimaryAR_Set(LSA_UINT32    const   InstanceHandle,
                                                               LSA_UINT16    const   ARSetID, 
                                                               LSA_UINT16    const   PrimaryARID);

/*=============================================================================
 * function name: eddi_ProviderBufferRequest_Xchange()
 *
 * function:      Exchange a provider-buffer
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID  The provider-ID
 *                pOffsetDB   ptr to the offset of the new buffer
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *                *pOffsetDB   offset of the old buffer
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ProviderBufferRequest_Xchange( LSA_UINT32  const  InstanceHandle,
                                                                         LSA_UINT16  const  ApplProviderID,
                                                                         LSA_UINT32      *  pOffsetDB );

/*=============================================================================
 * function name: eddi_ProviderBufferRequest_GetUserBuffer()
 *
 * function:      Get the offset of the current USER-buffer
 *
 * parameters:    InstanceHandle    The instance handle.
 *                ApplProviderID  The provider-ID
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM, EDD_STS_ERR_SEQUENCE
 *                *pOffsetDB   offset of the existent user-buffer
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_ProviderBufferRequest_GetUserBuffer( LSA_UINT32  const  InstanceHandle,
                                                                               LSA_UINT16  const  ApplProviderID,
                                                                               LSA_UINT32      *  pOffsetDB );


//Service-Codes in eddi_usr.h:
//  - EDD_SRV_SRT_PROVIDER_LOCK_CURRENT
//  - ...

typedef struct _EDDI_RT_PROVIDER_BUFFER_REQ_TYPE
{
    EDDI_HANDLE                     hDDB; //IN
    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer; //IN or OUT  depends on Service
    EDD_SERVICE                     Service; //IN
    LSA_UINT16                      ProviderID; //IN

} EDDI_RT_PROVIDER_BUFFER_REQ_TYPE;

/*=============================================================================
 * function name:  eddi_ProviderBufferRequest()
 *
 * function:
 *
 * parameters:    EDDI_RT_Provider_BUFFER_REQ_TYPE  * const pBufReq
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_PARAM
 *                 EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ProviderBufferRequest( const EDDI_RT_PROVIDER_BUFFER_REQ_TYPE  * const pBufReq );

//needed for EDDI_RtoCompConsumerUnLock3Buf
typedef struct _EDDI_RT_CONSUMER_BUFFER_REQ_TYPE
{
    EDDI_HANDLE                     hDDB; //IN
    EDD_UPPER_MEM_U8_PTR_TYPE       pBuffer; //IN or OUT
    EDD_SERVICE                     Service; //IN
    LSA_UINT16                      ConsumerID; //IN
    LSA_UINT16                      CycleCntStamp; // OUT
    LSA_UINT16                      CycleCntAct;   // OUT

} EDDI_RT_CONSUMER_BUFFER_REQ_TYPE;

typedef struct _EDDI_RT_BUFFER_REQ_GET_ID_TYPE
{
    EDDI_HANDLE                     hDDB; //IN
    LSA_UINT32                      DataOffset;//IN
    LSA_UINT16                      ConsumerProviderID; //Out

} EDDI_RT_BUFFER_REQ_GET_ID_TYPE;

/*=============================================================================
 * function name: eddi_BufferRequest_GetConsumerID()
 *
 * function:
 *
 * parameters:    EDDI_RT_BUFFER_REQ_GET_ID_TYPE  * const pBufReqGetID
 *
 *
 *
 * return value:  EDD_STS_OK
 *                EDD_STS_ERR_MODE
 *                EDD_STS_ERR_PARAM
 *                EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_BufferRequest_GetConsumerID( EDDI_RT_BUFFER_REQ_GET_ID_TYPE  * const pBufReqGetID);

/*=============================================================================
 * function name: eddi_BufferRequest_GetProviderID()
 *
 * function:
 *
 * parameters:    EDDI_RT_BUFFER_REQ_GET_ID_TYPE  * const pBufReqGetID
 *
 * return value:  EDD_STS_OK
 *                EDD_STS_ERR_PARAM
 *                EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_BufferRequest_GetProviderID( EDDI_RT_BUFFER_REQ_GET_ID_TYPE  * const pBufReqGetID);

/*=============================================================================
 * function name: eddi_SetDMACtrl()
 *
 * function:
 *
 * parameters:    EDDI_SET_DMA_CTRL_TYPE  * const pSetDMACtrl
 *
 * return value:  EDD_STS_OK
 *                EDD_STS_ERR_PARAM
 *                EDD_STS_ERR_SEQUENCE
 *===========================================================================*/
typedef struct _EDDI_SET_DMA_CTRL_TYPE
{
    EDDI_HANDLE                 hDDB;

    EDD_UPPER_MEM_U8_PTR_TYPE   pUserRam;
    LSA_UINT32                  UserBufferSize;         // >= Size of process image -> EDDI_SRV_DEV_OPEN: eddi_dpb.KRam.offset_ProcessImageEnd

    LSA_UINT32                  InData_CopyStartOffset; // copying of indata: 
    LSA_UINT32                  InDataCopy_Size;        // InDataCopy_Size (InData_Sync + InData_CopyStartOffset) --> (pInDataDst_UserRam + InData_CopyStartOffset)
    LSA_UINT32                  OutData_CopyStartOffset;// copying of outdata: 
    LSA_UINT32                  OutDataCopy_Size;       // OutDataCopy_Size (pOutDataDst_UserRam + OutData_CopyStartOffset) --> (OutData_Sync + OutData_CopyStartOffset)

    EDDI_MEMORY_MODE_TYPE       MemModeDMA;             //determines the kind of memory for the DMA-Transfer:
    // MEMORY_SHARED_MEM or MEMORY_SDRAM_ERTEC

} EDDI_SET_DMA_CTRL_TYPE;

LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_SetDMACtrl( EDDI_SET_DMA_CTRL_TYPE  * const pSetDMACtrl);

/*=============================================================================
 * function name:  eddi_SyncGetRealOffsetNs()
 *
 * function:       Return the Sync-Deviation messured via EXT_PLL-IN-Signal
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_INT32                 * pDeviationNs
 *
 * return value:   EDD_STS_OK
 *                 EDD_STS_ERR_SEQUENCE
 *                 EDD_STS_ERR_MODE
 *                 EDD_STS_ERR_HW
 *===========================================================================*/
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_SyncGetRealOffsetNs( EDDI_HANDLE    const   hDDB,
                                                           LSA_INT32   *  const   pDeviationNs );

/* LED Modes */
#define EDDI_LED_MODE_ON          0x8 /* turn led on                        */
#define EDDI_LED_MODE_OFF         0x9 /* turn led off                       */
#define EDDI_LED_MODE_BLINK       0xA /* led blinks                         */

LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR eddi_ResetIRTStarttime( EDDI_HANDLE  const  hDDB );

/*---------------------------------------------------------------------------*/
/* maximum number of timer needed -> do not change !!!                       */
/*---------------------------------------------------------------------------*/
#if defined (EDDI_CFG_FRAG_ON)
#define EDDI_CFG_MAX_TIMER_FRAG_VALUE   1
#else
#define EDDI_CFG_MAX_TIMER_FRAG_VALUE   0
#endif

#if defined (EDDI_CFG_DFP_ON)
#define EDDI_CFG_MAX_TIMER_DFP_VALUE    1
#else
#define EDDI_CFG_MAX_TIMER_DFP_VALUE    0
#endif

#define EDDI_CFG_MAX_TIMER   (EDDI_CFG_MAX_DEVICES * (17 + EDDI_CFG_MAX_TIMER_FRAG_VALUE + EDDI_CFG_MAX_TIMER_DFP_VALUE))


/*=============================================================================
 * function name:  eddi_ll_PhyWrite()
 *
 * function:       This function writes a value to a PHY register.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT32                  HwPortIndex
 *                 LSA_UINT32                  Register
 *                 LSA_UINT32                 *pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_PhyWrite(       EDDI_SYS_HANDLE  const hSysDev,
                                                 const EDDI_HANDLE            hDDB,
                                                       LSA_UINT32       const HwPortIndex,
                                                       LSA_UINT16             Register,
                                                 const LSA_UINT32     * const pValue );


#if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
/*=============================================================================
 * function name:  eddi_ll_PhyWriteRaw()
 *
 * function:       Writes any PHY register at any PHY address.
 *
 * parameters:     EDDI_HANDLE                   hDDB
 *                 LSA_UINT16                    PhyAddr
 *                 LSA_UINT16                    Register
 *                 LSA_UINT32 *           const  pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR   eddi_ll_PhyWriteRaw(EDDI_HANDLE         hDDB,
                                                    LSA_UINT16          PhyAddr,
                                                    LSA_UINT16          Register,
                                                    LSA_UINT32 *  const pValue );
#endif


/*=============================================================================
 * function name:  eddi_ll_PhyRead()
 *
 * function:       This function reads a value from a PHY register.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT32                  HwPortIndex
 *                 LSA_UINT32                  Register
 *                 LSA_UINT32                  *pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_PhyRead( EDDI_SYS_HANDLE  const  hSysDev,
                                          const EDDI_HANDLE      const  hDDB,
                                                LSA_UINT32       const  HwPortIndex,
                                                LSA_UINT16              Register,
                                                LSA_UINT32           *  pValue );


#if defined (EDDI_CFG_PHY_VARIABLE_PORTSWAPPING_NSC)
/*=============================================================================
 * function name:  eddi_ll_PhyReadRaw()
 *
 * function:       Reads any PHY register at any PHY address.
 *
 * parameters:     EDDI_HANDLE                 hDDB
 *                 LSA_UINT16                  PhyAddr
 *                 LSA_UINT16                  Register
 *                 LSA_UINT32 *                pValue
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR   eddi_ll_PhyReadRaw( EDDI_HANDLE         hDDB,
                                                    LSA_UINT16          PhyAddr,
                                                    LSA_UINT16          Register,
                                                    LSA_UINT32 *        pValue );
#endif



/*=============================================================================
 * function name:  eddi_ll_GetPhyType()
 *
 * function:       This function gets the PHY type.
 *
 * parameters:     EDDI_SYS_HANDLE               const  hSysDev
 *                 EDDI_HANDLE                   const  hDDB
 *                 LSA_UINT32                    const  HwPortIndex
 *                 EDDI_PHY_TRANSCEIVER_TYPE  *  const  pPhyType
 *
 * return value:   LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  eddi_ll_GetPhyType( EDDI_SYS_HANDLE               const  hSysDev,
                                             const EDDI_HANDLE                   const  hDDB,
                                                   LSA_UINT32                    const  HwPortIndex,
                                                   EDDI_PHY_TRANSCEIVER_TYPE  *  const  pPhyType );


/*=============================================================================
 * function name:  eddi_system()
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_SYSTEM
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edd
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDD_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edd
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_system( EDD_UPPER_RQB_PTR_TYPE  upper_rqb_ptr );

/*=============================================================================
 * function name:  eddi_open_channel()
 *
 * function:       open a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDD_OPC_OPEN_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 * RQB-Parameter:
 *      LSA_HANDLE_TYPE         HandleUpper:    channel-handle of user
 *      LSA_SYS_PATH_TYPE       SysPath:        system-path of channel
 *      LSA_VOID                LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf)
 *                                         (EDD_UPPER_RQB_PTR_TYPE pRQB)
 *                                              callback-function
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of edd
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_open_channel( EDD_UPPER_RQB_PTR_TYPE  pRQB );

/*=============================================================================
 * function name:  eddi_close_channel()
 *
 * function:       close a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-Header:
 *      LSA_OPCODE_TYPE         Opcode:         EDDI_OPC_CLOS_CHANNEL
 *      LSA_HANDLE_TYPE         Handle:         channel-handle of user
 *      LSA_USER_ID_TYPE        UserId:         Id of user
 *      LSA_COMP_ID_TYPE        CompId:         Cmponent-ID
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:
 *
 * RQB-return values via callback-function:
 *      LSA_HANDLE_TYPE         Handle:         HandleUpper from Open-Request
 *      LSA_RESULT              Response:       EDD_STS_OK
 *                                              EDD_STS_ERR_PARAM
 *                                              EDD_STS_ERR_SEQUENCE
 *                                              or others
 *
 *     All other RQB-parameters are unchanged.
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_close_channel( EDD_UPPER_RQB_PTR_TYPE  pRQB );

/*=============================================================================
 * function name:  eddi_request()
 *
 * function:       send a request to a communication channel
 *
 * parameters:     EDD_UPPER_RQB_PTR_TYPE  pRQB:  pointer to Upper-RQB
 *
 * return value:   LSA_VOID
 *
 * RQB-parameters:
 *      RQB-header:
 *      LSA_OPCODE_TYPE         Opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     channel-handle of edd
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:     Cmponent-ID
 *      EDD_SERVICE             Service:    EDD_SRV_*
 *      LSA_RESULT              Response:
 *      EDD_UPPER_MEM_PTR_TYPE  pParam:     Depend on kind of request.
 *
 * RQB-return values via callback-function:
 *      LSA_OPCODE_TYPE         opcode:     EDD_OPC_REQUEST
 *      LSA_HANDLE_TYPE         Handle:     HandleUpper from Open-Request
 *      LSA_USER_ID_TYPE        UserId:     ID of user
 *      LSA_COMP_ID_TYPE        CompId:
 *      EDD_SERVICE             Service:
 *      LSA_RESULT              Response:   EDD_STS_OK
 *                                          EDD_STS_ERR_PARAM
 *                                          EDD_STS_ERR_SEQUENCE
 *                                          or others
 *
 *     All other RQB-parameters depend on kind of request.
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_request( EDD_UPPER_RQB_PTR_TYPE  upper_rqb_ptr );

/*=============================================================================
 * function name: eddi_ApplSync_Init()
 *
 * function:      Initialisation of the interface for application-synchronisation
 *
 * parameters:    -
 *
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_Init( LSA_VOID );

/*=============================================================================
 * function name: eddi_ApplSync_Param()
 *
 * function:      Start/Stop/Parametrize 1 instance for application-synchronisation
 *
 * parameters:    pParam  ptr to parameterblock
 *
 * return value:  EDD_STS_OK, EDD_STS_PENDING, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_Param( EDDI_HANDLE                  const  hDDB, 
                                                        EDD_APPL_SYNC_PARAM_TYPE  *  const  pParam );

/*=============================================================================
 * function name: eddi_ApplSync_CheckTracks()
 *
 * function:      Check the tracks "early", "late", and "in_time"
 *
 * parameters:    Handle       Handle of this instance
 *                bStoreCycle  EDDI_APPLSYNC_CHECKTRACKS_STORE_CYCLE_ON:  Store CycleCtr of 1st ApplPhase
 *                             EDDI_APPLSYNC_CHECKTRACKS_STORE_CYCLE_OFF: Check against stored CycleCtr
 *
 * return value:  EDD_STS_OK, EDD_STS_PENDING, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *
 *                pTracks Track-bits
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_CheckTracks( LSA_UINT16     const  Handle,
                                                              LSA_UINT8   *  const  pTracks,
                                                              LSA_UINT8      const  bStoreCycle );
#define EDDI_APPLSYNC_CHECKTRACKS_STORE_CYCLE_OFF    0  
#define EDDI_APPLSYNC_CHECKTRACKS_STORE_CYCLE_ON     1

/*=============================================================================
 * function name: eddi_ApplSync_Interrupt()
 *
 * function:      Inthandler-function
 *
 * parameters:    Handle  Handle of this instance
 *
 *                Int     Specificies which interrupt has occurred
 *
 * return value:  EDD_STS_OK, EDD_STS_PENDING, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_Interrupt( LSA_UINT16  const  Handle, 
                                                            LSA_UINT32  const  Int );

/*=============================================================================
 * function name: eddi_ApplSync_GetApplTimeOffset()
 *
 * function:      Delivers the actual offset to the start of the 
 *                application cycle in ns
 *
 * parameters:    Handle  Handle of this instance
 *
 *                pApplTimeOffset_ns     Ptr to store the ApplTimeOffset
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_GetApplTimeOffset( LSA_UINT16     const  Handle,
                                                                    LSA_UINT32  *  const  pApplTimeOffset_ns );

/*=============================================================================
 * function name: eddi_ApplSync_GetTimeValues()
 *
 * function:      Delivers time values to the application
 *
 * parameters:    Handle  Handle of this instance
 *
 *                pTimeValues     Ptr to struct for returning the values
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_GetTimeValues (LSA_UINT16                       const  Handle,
	                                                            EDD_APPL_SYNC_TIME_VALUES_TYPE * const  pTimeValues);

/*=============================================================================
 * function name: eddi_InitDPBWithDefaults()
 *
 * function:      convenience function
 *
 * parameters:    pDPB                  Pointer to DPB Block
 *                pGlobalInitStruct     Pointer to Global Init Block
 *
 * usage:         1. Allocate the device parameter block (DPB)
 *                2. Preset the fields of the DPB by calling eddi_initDPBWithDefaults, passing the DPB and 
 *                   the GIS as arguments.
 *                3. check KRAM-Overflow: GIS-element KRAMFree has to be >0 !
 *                4. complete DPB by filling out user-specific fields marked with "param" in the DPB 
 *                   description.
 *                5. execute service EDDI_SRV_DEV_OPEN.
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitDPBWithDefaults( EDDI_UPPER_DPB_PTR_TYPE         pDPB,
                                                           EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct );

/*=============================================================================
 * function name: eddi_InitCOMPWithDefaults()
 *
 * function:      convenience function
 *
 * parameters:    pParam                Pointer to CompIni Block
 *                pGlobalInitStruct     Pointer to Global Init Block
 *
 * usage:         1. Allocate the component initialisation structure (CIS)
 *                2. Preset the fields of the (CIS) by calling eddi_initCOMPWithDefaults, passing the CIS and 
 *                   the GIS as arguments. 
 *                3. check KRAM-Overflow: GIS-element KRAMFree has to be >0 !
 *                4. complete DPB by filling out user-specific fields marked with "param" in the DPB 
 *                   description.
 *                5. execute service EDDI_SRV_DEV_COMP_INI.
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitCOMPWithDefaults( EDDI_UPPER_CMP_INI_PTR_TYPE     pParam,
                                                            EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct );

/*=============================================================================
 * function name: eddi_InitDSBWithDefaults()
 *
 * function:      convenience function
 *
 * parameters:    pDPB                  Pointer to DPB Block
 *                pGlobalInitStruct     Pointer to Global Init Block
 *
 * usage:         1. Allocate the device setup block (DSB)
 *                2. Preset the fields of the DSB by calling eddi_initDSBWithDefaults, passing the DSB and 
 *                   the GIS as arguments.
 *                3. check KRAM-Overflow: GIS-element KRAMFree has to be >0 !
 *                4. complete DPB by filling out user-specific fields marked with "param" in the DPB 
 *                   description.
 *                5. execute service EDDI_SRV_DEV_SETUP.
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_InitDSBWithDefaults( EDDI_UPPER_DSB_PTR_TYPE         pDSB,
                                                           EDDI_UPPER_GLOBAL_INI_PTR_TYPE  pGlobalInitStruct );

/*====  I2C-API-functions  =====*/

/*=============================================================================
 * function name: eddi_I2C_Write_Offset()
 *
 * function:      API-function: writes n bytes to the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *===========================================================================*/
EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Write_Offset( EDDI_HANDLE                    const  hDDB,          //Device handle (refer to service EDDI_SRV_DEV_OPEN)                           
                                                       LSA_UINT32                     const  PortId,        //User-PortId 1...4
                                                       LSA_UINT16                     const  I2CMuxSelect,  //can be used for HW-multiplexer-control
                                                       LSA_UINT8                      const  I2CDevAddr,    //0...127
                                                       LSA_UINT8                      const  I2COffsetCnt,  //0...2
                                                       LSA_UINT8                      const  I2COffset1,    //optional: 0...255
                                                       LSA_UINT8                      const  I2COffset2,    //optional: 0...255
                                                       LSA_UINT16                     const  RetryCnt,      //0: disabled,    1..65535
                                                       LSA_UINT16                     const  RetryTime_us,  //0: no waiting,  1..65535
                                                       LSA_UINT32                     const  Size,          //in bytes, 1...300
                                                       LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf );        //pointer to buffer

/*=============================================================================
 * function name: eddi_I2C_Read_Offset()
 *
 * function:      API-function: reads n bytes from the I2C device of the selected port
 *
 * parameters:    ...
 *
 * return value:  EDD_RSP
 *===========================================================================*/
EDD_RSP  EDD_UPPER_IN_FCT_ATTR  eddi_I2C_Read_Offset( EDDI_HANDLE                    const  hDDB,           //Device handle (refer to service EDDI_SRV_DEV_OPEN)                           
                                                      LSA_UINT32                     const  PortId,         //User-PortId 1...4
                                                      LSA_UINT16                     const  I2CMuxSelect,   //can be used for HW-multiplexer-control
                                                      LSA_UINT8                      const  I2CDevAddr,     //0...127
                                                      LSA_UINT8                      const  I2COffsetCnt,   //0...2
                                                      LSA_UINT8                      const  I2COffset1,     //optional: 0...255
                                                      LSA_UINT8                      const  I2COffset2,     //optional: 0...255
                                                      LSA_UINT16                     const  RetryCnt,       //0: disabled,    1..65535
                                                      LSA_UINT16                     const  RetryTime_us,   //0: no waiting,  1..65535
                                                      LSA_UINT32                     const  Size,           //in bytes, 1...300
                                                      LSA_UINT8  EDD_UPPER_MEM_ATTR      *  pBuf );         //pointer to buffer

/*=============================================================================
 * function name: eddi_SysChangeOwnMACAddr()
 *
 * function:      Change own MAC address
 *
 * parameters:    hDDB    Device handle
 *                pMAC    Pointer to new MAC Adr
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  eddi_SysChangeOwnMACAddr( EDDI_HANDLE                 hDDB,
                                                               EDD_MAC_ADR_TYPE  *  const  pMAC );

/*=============================================================================
 * function name: eddi_CalcUCMCLFSRMask()
 *
 * function:      
 *
 * parameters:    ...
 *
 * return value:  LSA_RESULT
 *===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  eddi_CalcUCMCLFSRMask( const EDDI_SWI_PARA_TYPE  *  const  pStatMCEntries,
                                                              LSA_UINT16          *  const  pUCMCLFSRMask );

/*****************************************************************************/
/*                                                                           */
/* Output-functions are in eddi_out.h                                        */
/*                                                                           */
/*****************************************************************************/

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SYS_H


/*****************************************************************************/
/*  end of file eddi_sys.h                                                   */
/*****************************************************************************/

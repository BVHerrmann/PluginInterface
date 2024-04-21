#ifndef EDDI_DEV_H              //reinclude-protection
#define EDDI_DEV_H

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
/*  F i l e               &F: eddi_dev.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  User Interface                                                           */
/*  Defines constants, types, macros and prototyping for prefix.             */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  17.02.04    ZR    - add icc in Device-Description-structure              */
/*                    - EDDI_COMP_ICC added                                  */
/*                    for new icc modul                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_const_val.h"
#include "eddi_conv_calc.h"

#include "eddi_ser_tree_bcw_mod.h"

#if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
#include "eddi_ser_tree_rcw.h"
#endif

#include "eddi_ser_tree.h"

#include "eddi_sync_rec.h"

#include "eddi_tree.h"
#include "eddi_irt_tree_typ.h"

#include "eddi_prm.h"

#include "eddi_sync_frm.h"

#include "eddi_irt_class2_stm.h"
#include "eddi_irt_class3_stm.h"
#include "eddi_sync_fwd_stm.h"
#include "eddi_sync_port_stms.h"
#include "eddi_sync_snd.h"
#include "eddi_sync_rcv.h"
#include "eddi_sync_typ.h"
#include "eddi_crt_phase_rx.h"
#include "eddi_crt_phase_tx.h"

#include "eddi_crt_prov.h"
#include "eddi_crt_cons.h"

#include "eddi_crt_int.h"

#include "eddi_csrt_int.h"
#include "eddi_irt_int.h"

#include "eddi_trac_int.h"
#include "eddi_swi_int.h"
#include "eddi_swi_phy_bugs.h"

#include "eddi_prof_kram.h"
#include "eddi_rto_int.h"

#include "eddi_prm_rec.h"
#include "eddi_prm_type.h"

#include "eddi_irte.h"
#include "eddi_sii.h"
#include "eddi_i2c.h"


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of LittleEndian byteorder to Hostbyteorder                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if defined (EDDI_CFG_LITTLE_ENDIAN)
/*****************************/
/* Little Endian             */
/*****************************/

#ifndef EDDI_LE2H_L
#define EDDI_LE2H_L(Value)                              /* Little Endian TO Host Long */
#endif

#ifndef EDDI_H2LE_L
#define EDDI_H2LE_L(Value)                              /* Host TO Little Endian Long */
#endif

#else
/*****************************/
/* Big Endian                */
/*****************************/
#ifndef EDDI_LE2H_L
#define EDDI_LE2H_L(Value) (EDDI_SWAP_ALWAYS_32(Value)) /* Little Endian TO Host Long */
#endif

#ifndef EDDI_H2LE_L
#define EDDI_H2LE_L(Value)  (EDDI_SWAP_ALWAYS_32(Value)) /* Host TO Little Endian Long */
#endif

#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ Conversion of Network byteorder to Hostbyteorder                        +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#if defined (EDDI_CFG_LITTLE_ENDIAN)
/*****************************/
/* Little Endian             */
/*****************************/
#ifndef EDDI_NTOHS
#define EDDI_NTOHS(Value) (EDDI_SWAP_ALWAYS_16(Value)) /* Network TO Host Short */
#endif

#ifndef EDDI_NTOHL
#define EDDI_NTOHL(Value) (EDDI_SWAP_ALWAYS_32(Value)) /* Network TO Host Long */
#endif

#ifndef EDDI_HTONS
#define EDDI_HTONS(Value) (EDDI_SWAP_ALWAYS_16(Value)) /* Host To Network Short */
#endif

#ifndef EDDI_HTONL
#define EDDI_HTONL(Value) (EDDI_SWAP_ALWAYS_32(Value)) /* Host TO Network Long */
#endif

#define EDDI_CONST_NTOHS(a, b) (0x##b##a)

/* access to Network byteordered variables, keeping the byteorder */
/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U16_KEEP_BYTEORDER(pValue)  ((LSA_UINT16)((((LSA_UINT16)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+1)))<<8) +  *(((LSA_UINT8 *)((LSA_VOID *)(pValue))))))

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U32_KEEP_BYTEORDER(pValue)  ((LSA_UINT32)( (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+3)))<<24) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+2)))<<16) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+1)))<<8 ) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))  )))    ) ))

#else
/*****************************/
/* Big Endian                */
/*****************************/
#ifndef EDDI_NTOHS
#define EDDI_NTOHS(Value) (Value) /* Network TO Host Short */
#endif

#ifndef EDDI_NTOHL
#define EDDI_NTOHL(Value) (Value) /* Network TO Host Long */
#endif

#ifndef EDDI_HTONS
#define EDDI_HTONS(Value) (Value) /* Host To Network Short */
#endif

#ifndef EDDI_HTONL
#define EDDI_HTONL(Value) (Value) /* Host TO Network Long */
#endif

#define EDDI_CONST_NTOHS(a, b) (0x##a##b)

/* access to Network byteordered variables, keeping the byteorder */
/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U16_KEEP_BYTEORDER(pValue)  ((LSA_UINT16)((((LSA_UINT16)(*((LSA_UINT8 *)((LSA_VOID *)(pValue)))))<<8) +  *(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+1)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U32_KEEP_BYTEORDER(pValue)  ((LSA_UINT32)( (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))  )))<<24) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+1)))<<16) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+2)))<<8 ) + \
                                                            (((LSA_UINT32)(*(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+3)))    ) ))

#endif
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ access to Network byteordered variables                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U16(pValue)  ((LSA_UINT16)((((LSA_UINT16)(*((LSA_UINT8 *)((LSA_VOID *)(pValue)))))<<8) +  *(((LSA_UINT8 *)((LSA_VOID *)(pValue)))+1)))

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U32(pValue)  ((LSA_UINT32) (((LSA_UINT32)(*(LSA_UINT8 *)((LSA_VOID *)(pValue)))    << 24) + \
                                            ((LSA_UINT32)(*((LSA_UINT8 *)((LSA_VOID *)(pValue))+1)) << 16) + \
                                            ((LSA_UINT32)(*((LSA_UINT8 *)((LSA_VOID *)(pValue))+2)) << 8)  + \
                                                          *((LSA_UINT8 *)((LSA_VOID *)(pValue))+3)) )

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U8_INCR(pValue, Value)  \
{                                        \
    Value = *pValue;                     \
    pValue += sizeof(LSA_UINT8);         \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U16_INCR(pValue,Value) \
{                                       \
    Value = EDDI_GET_U16(pValue);       \
    pValue += sizeof(LSA_UINT16);       \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U32_INCR(pValue,Value) \
{                                       \
    Value = EDDI_GET_U32(pValue);       \
    pValue += sizeof(LSA_UINT32);       \
}


/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U16_INCR_NO_SWAP(pValue, Value)  \
{                                                 \
    Value = EDDI_GET_U16_KEEP_BYTEORDER(pValue);  \
    pValue += sizeof(LSA_UINT16);                 \
}

/* pValue has to be a UINT8 - Ptr! */
#define EDDI_GET_U32_INCR_NO_SWAP(pValue, Value)  \
{                                                 \
    Value = EDDI_GET_U32_KEEP_BYTEORDER(pValue);  \
    pValue += sizeof(LSA_UINT32);                 \
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+ access to misaligned structures                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#define EDDI_BUILD_BYTEPTR(pBase_, Offset_) (((LSA_UINT8 *)(LSA_VOID *)(pBase_))+(Offset_))

#define EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBase_, Offset_, pU8_) LSA_UINT8 * pU8_ = ((LSA_UINT8 *)(LSA_VOID *)(pBase_))+(Offset_)

#define EDDI_GET_U16Offset(pBase_, Offset_, Dest_) { EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBase_, Offset_, const pU8); Dest_ = EDDI_GET_U16(pU8); }

#define EDDI_GET_U32Offset(pBase_, Offset_, Dest_) { EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBase_, Offset_, const pU8); Dest_ = EDDI_GET_U32(pU8); }

#define EDDI_COPY_MISALIGNED_U16(pBaseDst_, pBaseSrc_, Offset_) { EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBaseDst_, Offset_, pU8Dst);  \
                                                                  EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBaseSrc_, Offset_, pU8Src);  \
                                                                  *pU8Dst++     = *pU8Src++;                                  \
                                                                  *pU8Dst       = *pU8Src;     }

#define EDDI_COPY_MISALIGNED_U32(pBaseDst_, pBaseSrc_, Offset_) { EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBaseDst_, Offset_, pU8Dst);  \
                                                                  EDDI_GET_XX_CONSTRUCT_BYTEPTR(pBaseSrc_, Offset_, pU8Src);  \
                                                                  *pU8Dst++     = *pU8Src++;                                  \
                                                                  *pU8Dst++     = *pU8Src++;                                  \
                                                                  *pU8Dst++     = *pU8Src++;                                  \
                                                                  *pU8Dst       = *pU8Src;     }

/*===========================================================================*/
/*  Bit range operations (get/set) within a variable                         */
/*                                                                           */
/*  Msk  : Masks bit area within Val   (e.g. 0x0070 -> bits 4,5,6)           */
/*  Shift: Position (starting with 0) of first bit of Msk (e.g. 4 )          */
/*                                                                           */
/*===========================================================================*/

#define EDDI_GET_BIT_VALUE(Val,Msk,Shift)  (((Val) & (Msk)) >> Shift)

/*=====================================================================================================================*/
/*=====================================================================================================================*/

#define EDDI_SET_BIT_VALUE8(Target,Val,Msk,Shift)                                                       \
{                                                                                                       \
    Target = ((Target) &~ ((LSA_UINT8)Msk)) | ((((LSA_UINT8)Val) << (Shift)) & ((LSA_UINT8)Msk));       \
}

#define EDDI_SET_BIT_VALUE16(Target,Val,Msk,Shift)                                                      \
{                                                                                                       \
    Target = ((Target) &~ ((LSA_UINT16)Msk)) | ((((LSA_UINT16)Val) << (Shift)) & ((LSA_UINT16)Msk));    \
}

#define EDDI_SET_BIT_VALUE32(Target,Val,Msk,Shift)                                                      \
{                                                                                                       \
    Target = ((Target) &~ ((LSA_UINT32)Msk)) | ((((LSA_UINT32)Val) << (Shift)) & ((LSA_UINT32)Msk));    \
}

#define EDDI_SET_BIT_VALUE64(Target,Val,Msk,Shift)                                                      \
{                                                                                                       \
    Target = ((Target) &~ ((LSA_UINT64)Msk)) | ((((LSA_UINT64)Val) << (Shift)) & ((LSA_UINT64)Msk));    \
}

/*=====================================================================================================================*/
/*=====================================================================================================================*/

#define EDDI_CLR_BIT_VALUE8(Target,Msk)             \
{                                                   \
    Target &= (~((LSA_UINT8)Msk));                  \
}

#define EDDI_CLR_BIT_VALUE16(Target,Msk)            \
{                                                   \
    Target &= (~((LSA_UINT16)Msk));                 \
}

#define EDDI_CLR_BIT_VALUE32(Target,Msk)            \
{                                                   \
    Target &= (~((LSA_UINT32)Msk));                 \
}

#define EDDI_CLR_BIT_VALUE64(Target,Msk)            \
{                                                   \
    Target &= (~((LSA_UINT64)Msk));                 \
}

/*=====================================================================================================================*/
/*=====================================================================================================================*/

#define EDDI_SHIFT_VALUE_RIGHT(Target,Shift)        \
{                                                   \
    Target = ((Target) >> (Shift));                 \
}

#define EDDI_SHIFT_VALUE_LEFT(Target,Shift)         \
{                                                   \
    Target = (((Target)&((2^(Shift))-1)) << (Shift));                 \
}

/*=====================================================================================================================*/
/*=====================================================================================================================*/

//defines for UsedComp (can be ored)
#define EDDI_COMP_NRT       0x0001
#define EDDI_COMP_XRT       0x0002
#define EDDI_COMP_SYNC      0x0004

//#define EDDI_COMP_SRT       0x0008
//#define EDDI_COMP_IRT       0x0010

#define EDDI_COMP_SWI       0x0080

#define EDDI_COMP_PRM       0x0100

#define EDDI_COMP_GENERAL   0xFFFF

//#define EDDI_LOC_FPGA_XC2_V6000 0x00
//#define EDDI_LOC_FPGA_XC2_V8000 0x01
//#define EDDI_LOC_ERTEC400       0x02
//#define EDDI_LOC_ERTEC200       0x03
//#define EDDI_LOC_SOC1           0x04
//#define EDDI_LOC_SOC2           0x05

//defines for LinkIndProvideType
#define LinkIndProvide_NoExist 0x00 //No Indication exist
#define LinkIndProvide_Normal  0x01 //EDD_SRV_LINK_STATUS_IND_PROVIDE
#define LinkIndProvide_EXT     0x02 //EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT

//typedef struct _EDDI_LINK_BASE_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LINK_BASE_TYPE_PTR;

/*===========================================================================*/

typedef struct _EDDI_RX_OVERLOAD_TYPE
{
    LSA_UINT32              cIndication;
    EDD_UPPER_RQB_PTR_TYPE  pRQB;

} EDDI_RX_OVERLOAD_TYPE;

typedef struct _EDDI_HDB_SYNC_DIAG_TYPE
{
    EDDI_RQB_QUEUE_TYPE     IndQueue;       //SyncDiagIndReq
    EDD_UPPER_RQB_PTR_TYPE  pCurrentRqb;    //RQB currently used to akkumulate DiagEntries
    LSA_UINT32              LostEntryCnt;   //LostEntryCnt > 0, if user couldn't be informed
                                            //because of missing INDICATION-RESSOURCE

} EDDI_HDB_SYNC_DIAG_TYPE;

typedef struct _EDDI_DDB_LOCAL_IF_STATS_TYPE
{
    LSA_UINT32     RxGoodCtr;
    LSA_UINT32     TxGoodCtr;
    LSA_UINT32     OutDiscards;
    LSA_BOOL       bOpen[EDDI_MAX_IRTE_PORT_CNT];
    LSA_UINT32     CurrClosedDropCtr[EDDI_MAX_IRTE_PORT_CNT];   //drop-ctr (irte) at the time the port is closed. Index is HwPortIndex!
    LSA_UINT32     SubstClosedDropCtr[EDDI_MAX_IRTE_PORT_CNT];  //last corrected drop-ctr before closing the port. Index is HwPortIndex!
    LSA_UINT32     AccCorrDropCtr[EDDI_MAX_IRTE_PORT_CNT];      //accumulated drop-events during the time a port is in state CLOSED. Index is HwPortIndex!

} EDDI_DDB_LOCAL_IF_STATS_TYPE;

typedef struct _EDDI_STATISTICS_SHADOW_CTRS
{

    EDD_RQB_GET_STATISTICS_TYPE       ShadowStatistics;  //shadow-values used for subtraction, updated through "reset_statistcs"
    EDD_GET_STATISTICS_IRTE_RAW_TYPE  LocalStatistics;   //current read ctrs from hw
    EDDI_DDB_LOCAL_IF_STATS_TYPE      ShadowLocalIFCtr;  //current read ctrs from localIF

} EDDI_STATISTICS_SHADOW_CTRS;

typedef struct _EDDI_STATISTICS_SHADOW_CTRS  EDD_UPPER_MEM_ATTR * EDDI_UPPER_STATISTICS_SHADOW_CTRS_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Handle-management-structures                                              */
/*---------------------------------------------------------------------------*/
/* This structure is assigned to every open channel. Each component (NRT,SRT,*/
/* IRT) can have individual parameters within management structure only used */
/* in component.                                                             */
typedef struct _EDDI_HDB_TYPE
{
    EDDI_QUEUE_ELEMENT_TYPE         QueueLink;       // Contains Queuing-Links

    LSA_UINT32                      InUse;
    LSA_UINT32                      UsedComp;
    LSA_SYS_PATH_TYPE               SysPath;
    LSA_SYS_PTR_TYPE                pSys;
    EDDI_DETAIL_PTR_TYPE            pDetail;
    LSA_HANDLE_TYPE                 UpperHandle;
    LSA_HANDLE_TYPE                 Handle;                                     //Ctr in HDB array

    LSA_VOID                        LSA_FCT_PTR(EDD_UPPER_OUT_FCT_ATTR, Cbf) (EDD_UPPER_RQB_PTR_TYPE pRQB);

    LSA_UINT8                       InsertSrcMAC;                               //EDDI inserts station MAC address in send frame before sending
    EDDI_RQB_QUEUE_TYPE             AutoLinkIndReq;                             //Link-Indication Requests
    EDDI_RQB_QUEUE_TYPE             PortIDLinkIndReq[EDDI_MAX_IRTE_PORT_CNT];   //Link-Indication Requests, user view no port mapping
    LSA_UINT16                      LinkIndProvideType[EDDI_MAX_IRTE_PORT_CNT]; //LinkIndication of type EDD_SRV_LINK_STATUS_IND_PROVIDE_EXT available

    //each switch-channel refers to a separate low-water-mark-indication
    EDDI_RQB_QUEUE_TYPE             LowWaterIndReq;
    LSA_UINT16                      LowWaterLostMark;   //number of low-water-mark not indicated

    EDDI_LOCAL_DDB_PTR_TYPE         pDDB;               //link to devicemanagement

    EDDI_NRT_RX_USER_TYPE       *   pRxUser;
    LSA_UINT32                      FrameFilter;        //filter

    EDDI_NRT_CHX_SS_IF_TYPE     *   pIF;

    EDDI_RX_OVERLOAD_TYPE           RxOverLoad;

    //SyncDiag-Queue
    EDDI_HDB_SYNC_DIAG_TYPE         SyncDiag;

    LSA_UINT32                      RefCnt;

    EDDI_STATISTICS_SHADOW_CTRS     sStatisticShadow[EDDI_MAX_IRTE_PORT_CNT+1];
    
    LSA_INT32                       TxCount;
    LSA_INT32                       RxCount;
    EDD_UPPER_RQB_PTR_TYPE          pRQBTxCancelPending;

    EDDI_RQB_QUEUE_TYPE             intTxRqbQueue; //internal Tx RQB queue
    LSA_UINT32                      intTxRqbCount; //counting internal Tx RQBs
    EDDI_RQB_QUEUE_TYPE             intRxRqbQueue; //internal Rx RQB queue
    LSA_UINT32                      intRxRqbCount; //counting internal Rx RQBs

} EDDI_HDB_TYPE;

/*===========================================================================*/

typedef struct _EDDI_DEVICE_ERR_INTS
{
    LSA_UINT32      hol_interrupts[EDDI_NRT_IF_CNT];

} EDDI_DEVICE_ERR_INTS;

typedef enum _EDDI_SETUP_MODE
{
    EDDI_HW_SETUP_ALL = 2,
    EDDI_HW_SETUP_PARTIAL

} EDDI_SETUP_MODE;

typedef struct _EDDI_LINKDOWN_SIMULATE
{
    LSA_BOOL      bActive;
    LSA_UINT8     LinkStatus;
    LSA_UINT8     LinkSpeed;
    LSA_UINT8     LinkMode;

} EDDI_LINKDOWN_SIMULATE;

/*---------------------------------------------------------------------------*/
/* GLOBAL-DEVICE-structures                                                  */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DEVICE_GLOB_DSB_TYPE
{
    EDDI_GLOB_PARA_TYPE Glob;
} EDDI_DEVICE_GLOB_DSB_TYPE;

typedef struct _EDDI_DEVICE_GLOB_TYPE
{
    EDDI_PORT_PARAMS_TYPE       PortParams[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    LSA_UINT32                  MaxBridgeDelay;

    SER_HANDLE_TYPE             LLHandle;               //Low-Level-fct. handle

    LSA_BOOL                    HWIsSetup;              //LSA_TRUE if device_setup was successful

    LSA_TIMER_ID_TYPE           CycleCountTimerID;

    LSA_UINT32                  InterfaceID;            //Interface ID

    LSA_UINT32                  TraceIdx;

    EDDI_DEVICE_ERR_INTS        ErrInt;

    EDDI_IP_TYPE                LocalIP;

    LSA_UINT32                  OpenCount;              //ctr for open channels

    EDD_UPPER_RQB_PTR_TYPE      pCloseDevicePendingRQB; //if != 0: close device is waiting for all static rqbs to return

    EDDI_DYN_LIST_HEAD_TYPE     MCMACTabelleLocal;

    LSA_UINT32                  HardwareType;

    EDDI_SETUP_MODE             SetMode;

    LSA_BOOL                    bSupportIRTflex;            //Accept(LSA_TRUE)/Reject(LSA_FALSE) a parametrization containing IRTFlex

    LSA_BOOL                    bDisableMaxPortDelayCheck;  //disable(LSA_TRUE) the check of MaxPort(T/R)xDelay against PDIRGlobalData.MaxPort(T/R)xDelay. Needed for some old devices

    //external or internal PHY
    LSA_BOOL                    bPhyExtern;

    //LED-Timer for flash-frequency
    LSA_TIMER_ID_TYPE           LED_OnOffDuration_TimerID;

    //LED running
    LSA_BOOL                    LED_IsRunning;

    //LED on/off
    LSA_UINT16                  LED_OnOffDuration_Status;

    //Nr of 100ms ticks
    LSA_UINT16                  LED_TotalBlink_Cnt_100ms;

    //Nr of 100ms ticks
    LSA_UINT16                  LED_OnOff_Cnt_100ms;

    //Total duration of flashing
    LSA_UINT16                  LED_TotalBlink_Duration_100ms;

    //On/Off-Time
    LSA_UINT16                  LED_OnOff_Duration_100ms;

    //LINK Down simulation
    EDDI_LINKDOWN_SIMULATE      LinkDownSimulate[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    //PHY Powerstatus down/up
    LSA_BOOL                    PhyPowerOff[EDDI_MAX_IRTE_PORT_CNT]; //index = HwPortIndex

    //Max. Interface count
    LSA_UINT16                  MaxInterfaceCntOfAllEDD;

    //Max. Port count
    LSA_UINT16                  MaxPortCntOfAllEDD;

    //statemachine for PRM PortModule states
    EDDI_PRM_PORT_MODULE_SM_TYPE    PortModuleSm[EDDI_MAX_IRTE_PORT_CNT]; 

    //Stored DSB parameters
    EDDI_DEVICE_GLOB_DSB_TYPE   DSBStored;

} EDDI_DEVICE_GLOB_TYPE;

//typedef struct _EDDI_DEVICE_GLOB_TYPE  EDDI_LOCAL_MEM_ATTR *  EDDI_LOCAL_DEVICE_GLOB_PTR_TYPE;
/*===========================================================================*/
typedef LSA_UINT32 ( EDDI_SYSTEM_OUT_FCT_ATTR  *DEV_ADR_CONVERT_FCT) ( EDDI_SYS_HANDLE                    hSysDev,
                                                                       LSA_VOID                  * const  p,
                                                                       LSA_UINT32                  const  location );

typedef LSA_UINT32 ( EDDI_SYSTEM_OUT_FCT_ATTR  *DEV_OFFSET_CONVERT_FCT) ( EDDI_SYS_HANDLE                    hSysDev,
                                                                          LSA_UINT32                  const  p,
                                                                          LSA_UINT32                  const  location );

/* Definite ErrorCode of EDDI -> see eddi_m_id.h
   Is only important for response != EDD_STS_OK
   see also EDDI_SET_DETAIL_ERR in eddi_m_id.h */
typedef struct _EDDI_DETAIL_ERR_TYPE
{
    LSA_UINT32                    bSet;
    #if !defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //reduce codesize
    LSA_UINT32                    Line;
    #endif
    LSA_UINT32                    ModuleID;
    LSA_UINT32                    Error;
    LSA_UINT32                    Value;

} EDDI_DETAIL_ERR_TYPE;

typedef struct _EDDI_PROCESS_IMAGE_TYPE
{
    EDD_UPPER_MEM_U8_PTR_TYPE     pUserDMA;

    LSA_UINT32                    offset_ProcessImageEnd;

    #if !defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)
    EDD_UPPER_MEM_U8_PTR_TYPE     pKRAMMirror;     //local memory
    #endif //!defined (EDDI_CFG_DISABLE_KRAM_OVERLAP_CHECK)

} EDDI_PROCESS_IMAGE_TYPE;

//typedef struct _EDDI_PROCESS_IMAGE_TYPE  EDD_UPPER_MEM_ATTR *  EDDI_PROCESS_IMAGE_PTR_TYPE;

typedef struct _EDDI_SNDCLKCH_TRANSITION_TYPE
{
    LSA_BOOL                      bTransitionRunning;   //Set to LSA_TRUE if a SNDCLKCH-Transition is running

    LSA_UINT32                    TriggerDeadline_10ns; //Temp storage only
    // Request is stored only if a State-Transition is running
    EDD_UPPER_RQB_PTR_TYPE        pRunningRQB;         // Running Request of current Transition;

} EDDI_SNDCLKCH_TRANSITION_TYPE;

typedef enum _EDDI_CYC_COUNTER_STATE_TYPE
{
    EDDI_CYC_COUNTER_STATE_RUNNING        = 0,
    EDDI_CYC_COUNTER_STATE_JUMP           = 1,
    EDDI_CYC_COUNTER_STATE_ADJUST_PHASE   = 2 //,
    // EDDI_CYC_COUNTER_STATE_VERIFY         = 3,

} EDDI_CYC_COUNTER_STATE_TYPE;

typedef enum _EDDI_CYC_COUNTER_EVENT_TYPE
{
    EDDI_CYC_COUNTER_EVENT_SET            = 0,
    EDDI_CYC_COUNTER_EVENT_NEWCYCLE       = 1

} EDDI_CYC_COUNTER_EVENT_TYPE;

// Struct to handle NewCycle-Reduction during "hard setting"
// of CycleCounter
typedef struct _EDDI_CYCLE_EXPANSION_TYPE
{
    LSA_UINT32 WorstCaseInterval_10ns;  // Maximum expected length    for NewCycle-Interval
    LSA_UINT32 WorstCaseCycleReduction; // Maximum expected Reduction for NewCycle-Calls

    LSA_UINT32 LastNewCycleBeginInTicks;     // NewCycleBegin-Timestamp of last Update

    LSA_UINT32 Time_10ns;               // RunTime of running expansion (10ns-Units)

    LSA_INT32  TotalExpansion;          // For Debug Purposes only
    LSA_INT32  RestExpansion;

} EDDI_CYCLE_EXPANSION_TYPE;

typedef struct _EDDI_DDB_CYC_COUNTER_TYPE
{
    EDDI_CYC_COUNTER_STATE_TYPE        State;              // For asynchronous SET_CYCLE_COUNT-Service
    EDD_RQB_TYPE                     * pRQB;               // For asynchronous SET_CYCLE_COUNT-Service
    EDDI_HDB_TYPE                    * pHDB;               // For asynchronous SET_CYCLE_COUNT-Service
    EDD_RQB_SET_SYNC_TIME_TYPE         Param;              // For asynchronous SET_CYCLE_COUNT-Service
    EDDI_CYCLE_COUNTER_TYPE            CycleCountEndOfJump;// For asynchronous SET_CYCLE_COUNT-Service
    LSA_UINT32                         CycleCounterLow;    // For asynchronous SET_CYCLE_COUNT-Service
    LSA_UINT32                         CycleCounterHigh;   // For asynchronous SET_CYCLE_COUNT-Service

    LSA_UINT32                         Entity;              // CycleLength in 31,25us entities
    LSA_UINT32                         CycleLength_us;      // CycleLength in us
    LSA_UINT32                         CycleLength_10ns;    // CycleLength in 10ns entities
    LSA_BOOL                           bCBFIsBinary;

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_UINT32                         cOldHardwareLow;
    LSA_UINT32                         cOldHardwareHigh;
    #endif

    LSA_INT32                          AdjustInterval;      // AdjustInterval --> used to set PhaseCorrection-Interval.
    // only changed in EDDI_SetAdjustInterval !
    // This Value defines the slope of the "Adjustment-Ramp"
    // Examples: Value: 1000000
    //           means: each 1000000 Ticks the time is incremented by 1 Tick
    //           or each 1000000 Ns the time is incremented by 1 Ns
    //           Value: -1000000
    //           means: each 1000000 Ticks the local time is *de*cremented by 1 Tick
    //               or each 1000000 Ns    the local time is *de*cremented by 1 Ns

    // The relation between RateRatio (f0/f1) and AdjustInterval is:
    // RateRatio = 1 + (1 / AdjustInterval)
    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    LSA_INT32                          DriftInterval;       // Used to set DriftCorrection-Interval.
    // Meaning of sign is analogous to AdjustInterval.
    #endif

    LSA_UINT32                         EffCycLen_10ns;      // Effective CycleLength expressed in 10ns-Units.
    //                                                    
    // ERTEC400: EffCycLen_10ns depends on PhaseCorrection-Register
    //
    // ERTEC200: EffCycLen_10ns depends on PhaseCorrection-Register
    //                                     and DriftCorrection-Register

    EDDI_SYNC_GET_SET_TYPE              SyncGet;

    EDDI_CYCLE_EXPANSION_TYPE           CycleExpansion;     // Needed for Hard Correction of CycleLength;

    EDDI_SNDCLKCH_TRANSITION_TYPE       SndClkChTransition; // Stores data for asynchronous SendClockChange Transition

} EDDI_DDB_CYC_COUNTER_TYPE;

#define EDDI_SRV_AGE_DUMMY                 0xF0FF

#define EDDI_OPC_AGE_DUMMY_UNUSED          0x0000  //AGE_FDB-dummy-rqb is free
#define EDDI_OPC_AGE_DUMMY_IN_USE          0x0001  //AGE_FDB-dummy-rqb is in use, command is running
#define EDDI_OPC_AGE_DUMMY_QUEUED          0x0002  //AGE_FDB-dummy-rqb is in use, command has been queued
#define EDDI_OPC_AGE_DUMMY_DEQUEUED        0x0003  //AGE_FDB-dummy-rqb is in use, ready for execution

#define EDDI_CMD_IF_FREE                1
#define EDDI_CMD_IF_LOCKED              2
#define EDDI_CMD_IF_LOCKED_CONF_DONE    3

typedef struct _EDDI_DDB_ASYNC_SER_CMD_TYPE
{
    LSA_UINT32               State;
    EDD_UPPER_RQB_PTR_TYPE   pBottom;
    EDD_UPPER_RQB_PTR_TYPE   pTop;

    #if defined (EDDI_SER_CMD_STAT)
    LSA_UINT32               StartClkCountValue;
    LSA_UINT32               StartClkCountValueAsync;
    #endif

    EDD_RQB_TYPE             AgeDummyRQB;

    LSA_UINT32               cRequest_CLEAR_DYN_FDB;

    EDD_RQB_TYPE             *pActAsyncRQB;
    EDD_SERVICE              ActAsyncService; 

} EDDI_DDB_ASYNC_SER_CMD_TYPE;

#define EDDI_DDB_USED      1UL
#define EDDI_DDB_NOT_USED  2UL

#if defined (EDDI_CFG_USE_SW_RPS)
typedef LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *FP_CRTRpsCheckDataStatesEvent)(EDDI_LOCAL_DDB_PTR_TYPE     const pDDB,
                                                                         EDDI_CRT_RPS_TYPE         * const pRps );
#endif

#define  EDDI_BORDERLINE_PAT        EDDI_PASTE_32(FE,FD,FC,FB)
#define  EDDI_BORDERLINE_ELEMENTS   0x4

typedef struct _EDDI_DDB_PA_BORDERLINE_KRAM_TYPE
{
    LSA_UINT32       dw[EDDI_BORDERLINE_ELEMENTS];

} EDDI_DDB_PA_BORDERLINE_KRAM_TYPE;

typedef struct _EDDI_DDB_PA_BORDERLINE_TYPE
{
    EDDI_DDB_PA_BORDERLINE_KRAM_TYPE    *pBoLiKram;
    LSA_UINT32                          act_index;

} EDDI_DDB_PA_BORDERLINE_TYPE;

#define EDDI_CFG_INTERRUPT_STATISTIC

typedef struct _EDDI_DDB_INT_STAT_TYPE
{
    LSA_UINT32                cycle_in_10ns_ticks;
    LSA_UINT32                last_in_10ns_ticks;

    LSA_UINT32                cycleLostRingBuffer[EDDI_MAX_CNT_CYCLE_LOST_ARRAY];
    LSA_UINT8                 actBufferElem;
    //DELTETE LSA_BOOL                  outOfSyncFlag;

    PROF_EDDI_INT_STATS_INFO   info;

} EDDI_DDB_INT_STAT_TYPE;

typedef struct EDDI_ERTEC_TYPE_S
{
    LSA_UINT32     Debug;
    LSA_UINT32     Version;
    LSA_UINT32     MacroRevision;
    LSA_UINT32     Variante;
    LSA_UINT32     Location;

} EDDI_ERTEC_TYPE_T;

typedef struct EDDI_PM_INT_TYPE_S
{
    EDDI_PORTMAP_TYPE                   PortMap;

    LSA_UINT32                          HwTypeMaxPortCnt;   //Max Number of Ports supported by HwType
    //  ERTEC200 : 2
    //  ERTEC400 : 4
    //  SOC_1    : 3
    //  SOC_2    : 2

    LSA_UINT32                          PortMask_01_08[EDDI_MAX_IRTE_PORT_CNT];
    LSA_UINT32                          PortMask_02_10[EDDI_MAX_IRTE_PORT_CNT];
    LSA_UINT32                          PortMask_04_20[EDDI_MAX_IRTE_PORT_CNT];

    LSA_UINT32                          UsrPortID_to_HWPort_0_3[EDD_CFG_MAX_PORT_CNT + 1];
    LSA_UINT32                          HWPortIndex_to_UsrPortID[EDDI_MAX_IRTE_PORT_CNT];
    LSA_BOOL                            DomainBoundaryBLNotAllowed; //if any port has an egress filter without an igress-filter set -> Borderline not allowed

} EDDI_PM_INT_TYPE_T;

typedef struct EDDI_KRAM_MEM_TYPE_S  //structure for allocating KramFixMem (directly after IO-space)
{
    EDDI_DDB_PA_BORDERLINE_KRAM_TYPE   BorderLine;
    EDDI_SER_NRT_FREE_CCW_TYPE         NRT_FREE_CCW;
    EDDI_SER10_ACW_SND_TYPE            RootAcw;
    EDDI_SER10_EOL_TYPE                RootEol;
    EDDI_SER_IRT_BASE_LIST_TYPE        IrtBaseList;
    EDDI_SER10_TCW_TYPE                TCW;
    EDDI_TRACE_DIAG_ENTRY_TYPE         TraceDiagEntry[MAX_TRACE_DIAG_ENTRIES];

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    EDDI_R6_DCP_FILTER_TABLE_TYPE      DCP_FilterTable;
    EDDI_R6_DCP_FILTER_STRING_BUF_TYPE DCP_FilterStringBuf; //only 1 dcp filter string buffer is allocated!
    EDDI_SER_NRT_CCW_ENTRY_TYPE        DCP_ARP_CCW_Entry;
    //EDDI_SER_TIME_MASTER_LIST        TimeMasterList;
    NRT_ARP_FILTER_TABLE_TYPE          ARP_FilterTable;
    #endif

} EDDI_KRAM_MEM_TYPE_T;

/*---------------------------------------------------------------------------*/
/* DDB-Device-Description-Block-structures                                   */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_DDB_TYPE
{
    LSA_UINT32                          DeviceNr;       /* 0.. 3 */
    LSA_UINT32                          bUsed;

    EDDI_DEVICE_GLOB_TYPE               Glob;           /* Global Device management */

    EDDI_LOCAL_DDB_COMP_NRT_PTR_TYPE    pLocal_NRT;     /* NRT-specific management  */
    EDDI_DDB_COMP_NRT_TYPE              NRT;            /* NRT-specific management  */

    EDDI_LOCAL_DDB_COMP_CRT_PTR_TYPE    pLocal_CRT;     /* CRT-specific management  */
    EDDI_DDB_COMP_CRT_TYPE              CRT;            /* CRT-specific management  */

    #if defined (EDDI_CFG_USE_SW_RPS)
    FP_CRTRpsCheckDataStatesEvent       SW_RPS_CheckDataStatesEvent;
    LSA_UINT32                          SW_RPS_counter;
    LSA_UINT32                          SW_RPS_internal_reduction;
    LSA_UINT32                          SW_RPS_NextUK_Cycle_10ns;   /* reduced cycle-time for UKCountdown-calls in 10ns */
    LSA_UINT32                          SW_RPS_NextUK_ClockCntVal;  /* Discard UKCountdown-calls when ClockCntVal is below this value */
    LSA_BOOL                            SW_RPS_NextUK_Always;       /* This UKCountdown-call is always valid (oneshot) */
    #if defined (EDDI_CFG_CONSTRACE_DEPTH)
    LSA_UINT32                          SW_RPS_TotalCallsCtr;
    LSA_UINT32                          SW_RPS_RejectedCallsCtr;
    #endif
    #endif

    EDDI_LOCAL_DDB_COMP_RTOUDP_PTR_TYPE pLocal_RTOUDP;  /* RTOUDP-specific management */
    EDDI_DDB_COMP_RTOUDP_TYPE           RTOUDP;         /* RTOUDP-specific management */

    EDDI_LOCAL_DDB_COMP_SRT_PTR_TYPE    pLocal_SRT;     /* SRT-specific management    */
    EDDI_DDB_COMP_SRT_TYPE              SRT;            /* SRT-specific management    */

    EDDI_LOCAL_DDB_COMP_IRT_PTR_TYPE    pLocal_IRT;     /* IRT-specific management    */
    EDDI_DDB_COMP_IRT_TYPE              IRT;            /* IRT-specific management    */

    EDDI_LOCAL_DDB_COMP_SYNC_PTR_TYPE   pLocal_SYNC;    /* SYNC-specific management   */
    EDDI_DDB_COMP_SYNC_TYPE             SYNC;           /* SYNC-specific management   */

    EDDI_LOCAL_DDB_COMP_SWITCH_PTR_TYPE pLocal_SWITCH;  /* SWI-specific management    */
    EDDI_DDB_COMP_SWITCH_TYPE           SWITCH;         /* SWI-specific management    */

    EDDI_DDB_COMP_SII_TYPE              SII;            /* SII-specific management    */

    EDDI_DDB_COMP_I2C_TYPE              I2C;            /* I2C-specific management    */

    LSA_UINT32                          IRTE_SWI_BaseAdr;       /* Base of Reg */
    LSA_UINT32                          IRTE_SWI_BaseAdr_32Bit; /* Base of Reg */

    LSA_UINT32                          NRTMEM_LowerLimit;      /* Lower Limit of NRT-Memory (virt.) */
    LSA_UINT32                          NRTMEM_UpperLimit;      /* Upper Limit of NRT-Memory (virt.) */

    LSA_UINT32                          ERTECx00_SCRB_BaseAddr;

    EDDI_HANDLE                         hDDB;                   /* Device-handle (currently pointer) to EDDI_DEVICE_TYP */
    EDDI_SYS_HANDLE                     hSysDev;

    DEV_OFFSET_CONVERT_FCT              kram_adr_asic_to_local;
    DEV_ADR_CONVERT_FCT                 shared_mem_adr_to_asic;
    DEV_ADR_CONVERT_FCT                 sdram_mem_adr_to_asic;
    DEV_ADR_CONVERT_FCT                 kram_adr_loc_to_asic_dma;

    EDDI_ERTEC_TYPE_T                   ERTEC_Version;
    
    EDDI_DETAIL_ERR_TYPE                ErrDetail;

    EDDI_QUEUE_TYPE                     HDBQueue;       // HDBs used by this device
    EDDI_HDB_TYPE                       InternalHDB;    // This HDB has no external user !
    // Only used to store pDDB
    // needed for internal Scheduled-Requests
    // (e.g. RSM-Statemachine-Callbacks)

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //EDDI_SER_TIME_MASTER_LIST         *pDev_TimeMasterList;
    #endif

    LSA_UINT8                          *pKRam;       /* Base of KRam */
    LSA_UINT8                          *pKRamEDDI;   /* Base of EDD-KRAM-Space */
    LSA_UINT32                          SizeKRamEDDI;

    #if !defined (EDDI_CFG_3BIF_2PROC)
    EDDI_GSHAREDMEM_TYPE               *pGSharedRAM; /* Base of 2proc shared mem in global mem */ 
    #endif
    #if defined (EDDI_CFG_REV7)
    LSA_UINT8                          *pKRamStateBuffer; /* Base of the StateBuffers in KRAM */
    LSA_UINT32                          SizeKRamStateBuffer;
    #endif

    EDDI_MEM_VERW_H_T                   MemVerw;

    EDDI_DDB_PA_BORDERLINE_TYPE         PABorderLine;
    LSA_UINT32                          KRamMemHandle;

    PROF_KRAM_INFO                     *pProfKRamInfo;

    LSA_UINT32                          free_KRAM_buffer;

    EDDI_PROCESS_IMAGE_TYPE             ProcessImage;

    EDDI_DDB_CYC_COUNTER_TYPE           CycCount;

    EDDI_DDB_ASYNC_SER_CMD_TYPE         CmdIF;

    EDDI_PROF_KRAM_RES_TYPE             KramRes;
    EDDI_PROF_NRAM_RES_TYPE             NramRes;

    EDDI_RQB_QUEUE_TYPE                 CloseReq;
    LSA_TIMER_ID_TYPE                   CloseReqTimerID;

    EDDI_PRM_TYPE                       PRM;

    EDDI_DDB_INT_STAT_TYPE              IntStat;

    EDDI_DDB_LOCAL_IF_STATS_TYPE        LocalIFStats;

    EDDI_PM_INT_TYPE_T                  PM;

    LSA_UINT32                          ClkCounter_in_10ns_ticks_NewCycle;

    EDDI_KRAM_MEM_TYPE_T               *pKramFixMem;

    EDDI_ISR_T                          ISR_Table[EDDI_MAX_ISR];

    LSA_UINT32                          NewCycleReductionCtr;                       //reduction counter of NewCycle for reduced new-cycle
    LSA_UINT32                          NewCycleReductionFactor;                    //reduction factor  of NewCycle for reduced new-cycle (needed for SII and SWSB)
    #if defined (EDDI_CFG_SII_EXTTIMER_MODE_ON) || defined (EDDI_CFG_SII_EXTTIMER_MODE_FLEX)
    LSA_UINT32                          ExtTimerReductionCtr;                       //reduction counter of ExtTimer for reduced new-cycle
    LSA_UINT32                          ExtTimerReductionFactor;                    //reduction factor  of ExtTimer for reduced new-cycle
    #endif //EDDI_CFG_SII_EXTTIMER_MODE_ON || EDDI_CFG_SII_EXTTIMER_MODE_FLEX
    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    LSA_UINT32                          NewCycleReductionCtrForNRTCheck;            //reduction counter for NRT-check in New-Cycle-Sync-Mode
    LSA_UINT32                          NewCycleReductionFactorForNRTCheck;         //reduction factor  for NRT-check in New-Cycle-Sync-Mode
    #endif

    #if (EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE)
    LSA_BOOL                            bFilterLinkIndTrace;    
    #endif

    const EDDI_INI_PARAMS_FIXED_TYPE *  pConstValues;
    EDDI_DPB_FEATURE_TYPE               FeatureSupport;                             //copied from DPB
    LSA_UINT8                           HSYNCRole;

    EDDI_RQB_QUEUE_TYPE                 ClearFDBQueue;

} EDDI_DDB_TYPE;
/*===========================================================================*/

typedef  void  (EDDI_LOCAL_FCT_ATTR  *  EDDI_TIMEOUT_CBF)( void  *  const  context );

typedef struct _EDDI_TIMER_TYPE
{
    LSA_UINT32                  used;
    LSA_TIMER_ID_TYPE           SysTimerID;
    EDDI_LOCAL_DDB_PTR_TYPE     pDDB;
    void                      * context;
    EDDI_TIMEOUT_CBF            cbf;

    EDDI_SHED_OBJ_TYPE          ShedObj;

} EDDI_TIMER_TYPE;

#define  EDDI_MAX_EXCP_INFO     0x140

typedef struct _EDDI_EXCP_TYPE
{
    EDDI_FATAL_ERROR_TYPE       Error;
    LSA_UINT32                  ModuleID;
    LSA_UINT32                  Line;
    LSA_UINT8                   sInfo[EDDI_MAX_EXCP_INFO];

} EDDI_EXCP_TYPE;

typedef  LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *  EDDI_REQ_FCT )( EDD_UPPER_RQB_PTR_TYPE  const  pRQB );

typedef struct _EDDI_REQ
{
    EDDI_REQ_FCT                fct;

} EDDI_REQ;

typedef struct _EDDI_INFO_TYPE
{
    LSA_INT32                   CntDDB;
    EDDI_DDB_TYPE               DDB[EDDI_CFG_MAX_DEVICES];
    EDDI_HDB_TYPE               HDB[EDDI_CFG_MAX_CHANNELS];
    PROF_KRAM_INFO              ProfInfo[EDDI_CFG_MAX_DEVICES];

    EDDI_TIMER_TYPE             Timer[EDDI_CFG_MAX_TIMER];
    EDDI_EXCP_TYPE              Excp;

    LSA_INT32                   EDDI_Lock_Sema_Intern;
    LSA_INT32                   EDDI_Lock_Sema_REST;
    LSA_INT32                   EDDI_Lock_Sema_SYNC;
    LSA_INT32                   EDDI_Lock_Sema_SYNC_Special;
    LSA_INT32                   EDDI_Lock_Sema_COM;

    EDDI_REQ                    eddi_req[0x10];
    EDDI_QUEUE_TYPE             FreeHDBQueue;    // Free HDBs
} EDDI_INFO_TYPE;

/*===========================================================================*/

LSA_EXTERN  EDDI_INFO_TYPE                  *  g_pEDDI_Info ;
LSA_EXTERN  LSA_UINT32                         g_LocalMem;

LSA_EXTERN  EDDI_INI_PARAMS_FIXED_TYPE  const  EDDIConstValues[1];
LSA_EXTERN  LSA_UINT16                  const  EDDICrtMaxConsSmallSCF[EDDI_CRT_CYCLE_BASE_FACTOR_CRIT];

#define DEV32_MEMCOPY(pDest32, pSrct32, anzahl_byte)  EDDI_Dev32Memcpy((LSA_UINT32 *)(pDest32), (LSA_UINT32 *)(pSrct32), (LSA_UINT32)(anzahl_byte))

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_Dev32Memcpy( LSA_UINT32        *  const  pDest32,
                                               LSA_UINT32  const *  const  pSrc32,
                                               LSA_UINT32           const  anzahl_byte );

//macro contains no endianess-swapping
#define DEV_kram_adr_to_asic_register(p, pDDB)   ((LSA_UINT32)(p) - (pDDB)->IRTE_SWI_BaseAdr)

/*************/

#define EDDI_RQB_DDB(_p)    (((EDDI_LOCAL_HDB_PTR_TYPE)_p->internal_context)->pDDB)

#if defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //reduce codesize
#define EDDI_SET_DETAIL_ERR(pDDB, Error)  EDDI_SetDetailError(pDDB, Error, EDDI_MODULE_ID)
#else
#define EDDI_SET_DETAIL_ERR(pDDB, Error)  EDDI_SetDetailError(pDDB, Error, EDDI_MODULE_ID, __LINE__)
#endif

#if defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //reduce codesize
LSA_VOID  EDDI_SetDetailError( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                               LSA_UINT32               const  Error,
                               LSA_UINT32               const  ModuleID );
#else
LSA_VOID  EDDI_SetDetailError( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                               LSA_UINT32               const  Error,
                               LSA_UINT32               const  ModuleID,
                               LSA_UINT32               const  Line );
#endif


#define EDDI_PRM_ADD_ERR_OFFSET(pDDB, Offset)           \
{                                                       \
    pDDB->PRM.PrmDetailErr.ErrOffset += (Offset);       \
}

#if 1   // for debugging purposes set it to 0    see 'eddi_dev.c'
#define EDDI_PRM_SET_ERR_OFFSET(pDDB, Offset)           \
{                                                       \
    pDDB->PRM.PrmDetailErr.ErrOffset += (Offset);       \
    pDDB->PRM.PrmDetailErr.ErrValid   = LSA_TRUE;       \
}
#else
LSA_VOID  EDDI_PRM_SET_ERR_OFFSET( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                   LSA_UINT32               const  Offset);
#endif

#define EDDI_PRM_RESET_ERR_OFFSET(pDDB, Offset)         \
{                                                       \
    pDDB->PRM.PrmDetailErr.ErrOffset  = (Offset);       \
    pDDB->PRM.PrmDetailErr.ErrValid   = LSA_FALSE;      \
}
//#define EDDI_GET_ERR_OFFSET(pDDB)               (pDDB->PRM.PrmDetailErr.ErrOffset)

/** macro to set all values for PrmDetailErr
*
* @param[in]   pDetailErr      pointer to struct EDDI_PRM_DETAIL_ERR_TYPE
* @param[in]   _ErrPortID      LSA_UINT16 value for ErrPortID
* @param[in]   _ErrIndex       LSA_UINT32 value for ErrIndex
* @param[in]   _ErrOffset      LSA_UINT32 value for ErrOffset
* @param[in]   _ErrFault       LSA_UINT8 value for ErrFault
*/
#define EDDI_PRM_SET_DETAIL_ERR(pDetailErr, _ErrPortID, _ErrIndex, _ErrOffset, _ErrFault)       \
{                                                                                               \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrPortID   = (LSA_UINT16)(_ErrPortID);         \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrIndex    = (LSA_UINT32)(_ErrIndex);          \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrOffset   = (LSA_UINT32)(_ErrOffset);         \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrFault    =  (LSA_UINT8)(_ErrFault);          \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrValid    =  LSA_TRUE;                        \
}

/** macro to first init PrmDetailErr with default values
*
* @param[in]   pDetailErr      pointer to struct EDDI_PRM_DETAIL_ERR_TYPE
*/
#define EDDI_PRM_INIT_DETAIL_ERR(pDetailErr)                                                    \
{                                                                                               \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrPortID   = EDD_PRM_ERR_PORT_ID_DEFAULT;      \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrIndex    = EDD_PRM_ERR_INDEX_DEFAULT;        \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrOffset   = EDD_PRM_ERR_OFFSET_DEFAULT;       \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrFault    = EDD_PRM_ERR_FAULT_DEFAULT;        \
    ((EDDI_PRM_DETAIL_ERR_PTR_TYPE)pDetailErr)->ErrValid    = LSA_FALSE;                        \
}




LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FREELOCALMEM( LSA_UINT16             EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                  EDDI_DEV_MEM_PTR_TYPE                          lower_mem_ptr );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_AllocLocalMem( EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                   LSA_UINT32                                     length );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVAllocMode( EDDI_LOCAL_DDB_PTR_TYPE                          const  pDDB,
                                                  EDDI_MEMORY_MODE_TYPE                            const  MemMode,
                                                  EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  *  const  lower_mem_ptr_ptr,
                                                  LSA_UINT32                                       const  length,
                                                  EDDI_USERMEMID_TYPE                              const  UserMemID );

#if !defined EDDI_CFG_MAX_NR_PROVIDERS
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IOAllocLocalMem( EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                     LSA_UINT32                                     length );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IOFreeLocalMem( EDDI_SYS_HANDLE                                  hSysDev,
                                                    LSA_UINT16                EDDI_LOCAL_MEM_ATTR  * ret_val_ptr,
                                                    EDDI_LOCAL_MEM_PTR_TYPE                          local_mem_ptr);
#endif

#if defined (EDDI_CFG_FRAG_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVAllocModeTxFrag( EDDI_LOCAL_DDB_PTR_TYPE                          const  pDDB,
                                                        EDDI_MEMORY_MODE_TYPE                            const  MemMode,
                                                        EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  *  const  lower_mem_ptr_ptr,
                                                        LSA_UINT32                                       const  length,
                                                        EDDI_USERMEMID_TYPE                              const  UserMemID );
#endif

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVFreeMode( EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB,
                                                 EDDI_MEMORY_MODE_TYPE                         const  MemMode,
                                                 EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  const  lower_mem_ptr,
                                                 EDDI_USERMEMID_TYPE                           const  UserMemID );

LSA_VOID  EDDI_LOCAL_FCT_ATTR EDDI_MemSet( EDDI_LOCAL_MEM_PTR_TYPE  pMem,
                                           LSA_UINT8                Value,
                                           LSA_UINT32               Length );

LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_MemCopy( EDDI_DEV_MEM_PTR_TYPE    pDest,
                                            EDDI_LOCAL_MEM_PTR_TYPE  pSrc,
                                            LSA_UINT32               Size );

#if defined (EDDI_CFG_REV7)
LSA_UINT32 eddi_getClockCount10ns( LSA_UINT32  const  clk );
#endif

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_DEV_H


/*****************************************************************************/
/*  end of file eddi_dev.h                                                   */
/*****************************************************************************/

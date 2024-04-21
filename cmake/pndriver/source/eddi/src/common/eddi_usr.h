#ifndef EDDI_USR_H              //reinclude-protection
#define EDDI_USR_H

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
/*  F i l e               &F: eddi_usr.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* User Interface                                                            */
/* Defines constants, types, macros and prototyping for prefix.              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
#ifdef EDD_MESSAGE
/*  25.09.12     TP   Added buffer services (moved from edd_usr.h)           */
/*                    EDDI_SRV_SRT_PROVIDER_LOCK_CURRENT                     */
/*                    EDDI_SRV_SRT_PROVIDER_LOCK_NEW                         */
/*                    EDDI_SRV_SRT_PROVIDER_UNLOCK                           */
#endif
/*                                                                           */
/*****************************************************************************/

//#include "edd_usr.h"

/*===========================================================================*/
/*                                constants                                  */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Opcodes within RQB                                                        */
/*---------------------------------------------------------------------------*/
#define EDDI_SRV_DEV_OPEN                                (EDD_SERVICE)   0x100
#define EDDI_SRV_DEV_SETUP                               (EDD_SERVICE)   0x101
#define EDDI_SRV_DEV_CLOSE                               (EDD_SERVICE)   0x102
#define EDDI_SRV_DEV_COMP_INI                            (EDD_SERVICE)   0x103

/*=====   SWI-Services   ====*/
#define EDDI_SRV_SWITCH_GET_FDB_ENTRY_MAC                (EDD_SERVICE)   0x2206
#define EDDI_SRV_SWITCH_GET_FDB_ENTRY_INDEX              (EDD_SERVICE)   0x2207
#define EDDI_SRV_SWITCH_REMOVE_FDB_ENTRY                 (EDD_SERVICE)   0x2208
#define EDDI_SRV_SWITCH_CLEAR_DYN_FDB                    (EDD_SERVICE)   0x2209
#define EDDI_SRV_SWITCH_SET_AGING_TIME                   (EDD_SERVICE)   0x220a
#define EDDI_SRV_SWITCH_GET_AGING_TIME                   (EDD_SERVICE)   0x220b
#if defined (EDDI_CFG_MIRROR_MODE_INCLUDE)
#define EDDI_SRV_SWITCH_SET_PORT_MONITOR                 (EDD_SERVICE)   0x221b
#define EDDI_SRV_SWITCH_GET_PORT_MONITOR                 (EDD_SERVICE)   0x221c
#define EDDI_SRV_SWITCH_SET_MIRROR_PORT                  (EDD_SERVICE)   0x221d
#define EDDI_SRV_SWITCH_GET_MIRROR_PORT                  (EDD_SERVICE)   0x221e
#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE
#define EDDI_SRV_SWITCH_SET_FDB_ENTRY                    (EDD_SERVICE)   0x2205

/*=====  EDDI_OPC_REQUEST:  GENERAL-Services (0x2300..0x23FF) ======*/
#define EDDI_SRV_SETUP_PHY                               (EDD_SERVICE)   0x230c

/*=====   Buffer Services (0x2500..0x25FF) ======*/
#define EDDI_SRV_SRT_BUFFER_TYPE                         (EDD_SERVICE)   0x2500

#define EDDI_SRV_SRT_PROVIDER_LOCK_CURRENT               (EDD_SERVICE)   0x2500
#define EDDI_SRV_SRT_PROVIDER_LOCK_NEW                   (EDD_SERVICE)   0x2501
#define EDDI_SRV_SRT_PROVIDER_UNLOCK                     (EDD_SERVICE)   0x2502

/*=====   GENERAL-Services ======*/
#define EDDI_SRV_EV_TIMER_REST                           (EDD_SERVICE)   0x3000
#if defined (EDDI_CFG_SII_ADD_PRIO1_AUX)
#define EDDI_SRV_EV_INTERRUPT_PRIO1_AUX                  (EDD_SERVICE)   0x8000     //do not change without changing function-pointer-table eddi_req[]!
#endif
#define EDDI_SRV_EV_INTERRUPT_PRIO2_ORG                  (EDD_SERVICE)   0x9000     //do not change without changing function-pointer-table eddi_req[]!
#define EDDI_SRV_EV_INTERRUPT_PRIO3_REST                 (EDD_SERVICE)   0xA000     //do not change without changing function-pointer-table eddi_req[]!
#define EDDI_SRV_EV_INTERRUPT_PRIO4_NRT_LOW              (EDD_SERVICE)   0xB000     //do not change without changing function-pointer-table eddi_req[]!
#if defined (EDDI_CFG_SYSRED_2PROC)
#define EDDI_SRV_EV_SYSRED_POLL                          (EDD_SERVICE)   0xC000     //do not change without changing function-pointer-table eddi_req[]!
#endif

/*---------------------------------------------------------------------------*/
/*  Status within RQB (Response)                                             */
/*---------------------------------------------------------------------------*/
/* LSA OK-Codes */
#define EDDI_STS_OK_TIMER_RESTARTED       (LSA_RESULT)    LSA_RET_OK_TIMER_RESTARTED
#define EDDI_STS_OK_TIMER_NOT_RUNNING     (LSA_RESULT)    LSA_RET_OK_TIMER_NOT_RUNNING

/*---------------------------------------------------------------------------*/
/*  ASIC-Types ("location")                                                  */
/*---------------------------------------------------------------------------*/

//#define EDDI_LOC_FPGA_XC2_V6000    0x00
#define EDDI_LOC_FPGA_XC2_V8000_0    0x00
#define EDDI_LOC_FPGA_XC2_V8000      0x01
#define EDDI_LOC_ERTEC400            0x02
#define EDDI_LOC_ERTEC200            0x03
#define EDDI_LOC_SOC1                0x04
#define EDDI_LOC_SOC2                0x05

//Event
#define EDDI_CSRT_CONS_EVENT_NO      0x00

//Convenience functions
#define EDDI_HW_SUBTYPE_USED_NONE    0x00
#define EDDI_HW_SUBTYPE_USED_SOC1    0x01
#define EDDI_HW_SUBTYPE_USED_SOC2    0x02

/*===========================================================================*/
/*                              basic types for EDD                          */
/*===========================================================================*/
#define EDDI_LOWER_MEM_PTR_TYPE                  /* pointer to lower-memory */  \
LSA_VOID                    EDDI_LOWER_MEM_ATTR *

#define EDDI_LOCAL_MEM_PTR_TYPE                  /* pointer to local-memory */  \
LSA_VOID                    EDDI_LOCAL_MEM_ATTR *

#define EDDI_LOCAL_MEM_U8_PTR_TYPE               /* pointer to local-memory */  \
LSA_UINT8                   EDDI_LOCAL_MEM_ATTR *

#define EDDI_LOCAL_MEM_U16_PTR_TYPE              /* pointer to local-memory */  \
LSA_UINT16                  EDDI_LOCAL_MEM_ATTR *

#define EDDI_LOCAL_MEM_U32_PTR_TYPE              /* pointer to local-memory */  \
LSA_UINT32                  EDDI_LOCAL_MEM_ATTR *

#define EDDI_DEV_MEM_PTR_TYPE                    /* pointer to device-memory */ \
LSA_VOID                    EDDI_LOWER_MEM_ATTR *

#define EDDI_DEV_MEM_U8_PTR_TYPE                 /* pointer to device-memory */ \
LSA_UINT8                   EDDI_LOWER_MEM_ATTR *

#define EDDI_DEV_MEM_U32_PTR_TYPE                /* pointer to device-memory */ \
LSA_UINT32                  EDDI_LOWER_MEM_ATTR *

#define EDDI_DEV_MEM_U64_PTR_TYPE                /* pointer to device-memory */ \
LSA_UINT64                  EDDI_LOWER_MEM_ATTR *

#define EDDI_HANDLE         LSA_VOID EDDI_LOCAL_MEM_ATTR *
/* for device-handle        */

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* MAC/IP-Address                                                            */
/*---------------------------------------------------------------------------*/
typedef struct _EDD_MAC_ADR_TYPE    EDDI_LOCAL_MEM_ATTR        *          EDDI_LOCAL_MAC_ADR_PTR_TYPE;
typedef struct _EDD_MAC_ADR_TYPE    EDDI_LOCAL_MEM_ATTR const  *    EDDI_CONST_LOCAL_MAC_ADR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* The EDD needs the bufferrequirements mentioned below.                     */
/* For Rx-Frames and SRT - Tx-Frames it needs DWORD-Aligned buffers because  */
/* it needs to access the framecontent with WORD and DWORD - accesses. To    */
/* prevent misaligned accesses the buffers have to be proper aligned.        */
/* For NRT - Tx- Buffers the EDD does not access the buffer, so the edd has  */
/* no special requirements.                                                  */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* IRTE-IP receive max. 1536 bytes                                           */
/*---------------------------------------------------------------------------*/
#define EDDI_FRAME_BUFFER_LENGTH         1536

#define EDDI_RX_FRAME_BUFFER_ALIGN       0x03 //4-Byte-aligned frame buffers
#define EDDI_TX_NRT_FRAME_BUFFER_ALIGN   0x03 //IRTE needs no alignment, value set for compatibility with PNIP
#define EDDI_TX_SRT_FRAME_BUFFER_ALIGN   0x03 //4-Byte-aligned frame buffers

/*---------------------------------------------------------------------------*/
/* Because of the zero-copy - interface we must take care of alignment and   */
/* buffersize requirements from the ethernetcontroller within application.   */
/*---------------------------------------------------------------------------*/
/* All (receive+transmit)- buffers allocated within EDD must be at least     */
/* the following size (EDDI_FRAME_BUFFER_LENGTH). Receivebuffersize will     */
/* be configured within DPB (see below).                                     */
/* NOTE: This values must be >= EDD_IEEE_FRAME_LENGTH!! not checked !!       */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/* xRT - ERTEC400-Ports                                                      */
/*---------------------------------------------------------------------------*/
#define EDDI_CRT_PORT_NO                                   0x00
#define EDDI_CRT_PORT_0                                    0x01
#define EDDI_CRT_PORT_1                                    0x02
#define EDDI_CRT_PORT_2                                    0x04
#define EDDI_CRT_PORT_3                                    0x08
#define EDDI_CRT_PORT_CHA                                  0x10
#define EDDI_CRT_PORT_CHB                                  0x20
#define EDDI_CRT_PORT_L                                    0x80


#define EDDI_AUTONEG_CAP_100MBIT_HALF      (EDD_AUTONEG_CAP_100BASET4 + EDD_AUTONEG_CAP_100BASETX + EDD_AUTONEG_CAP_100BASET2)
#define EDDI_AUTONEG_CAP_100MBIT_FULL      (EDD_AUTONEG_CAP_100BASETXFD + EDD_AUTONEG_CAP_100BASET2FD)
#define EDDI_AUTONEG_CAP_10MBIT_HALF       (EDD_AUTONEG_CAP_10BASET)
#define EDDI_AUTONEG_CAP_10MBIT_FULL       (EDD_AUTONEG_CAP_10BASETFD)


/*===========================================================================*/
/*                        DebugInfo                                          */
/*===========================================================================*/
/*---------------------------------------------------------------------------*/
/* Service EDD_SRV_SRT_IND_PROVIDE:                                          */
/*---------------------------------------------------------------------------*/

/* Byte 0..11 of DebugInfo, contained in EDD_RQB_CSRT_IND_PROVIDE_TYPE (common to all events of  */
/* this indication). Only valid if at least 1 MISS is present                                    */ 
#define EDDI_DEBUGINFO_GLOBAL_IS_VALID                              0x01   
#define EDDI_DEBUGINFO_GLOBAL_IS_SYNCOK                             0x02   

#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_NOT_SUPPORTED     0       // same as EDD_IRT_NOT_SUPPORTED
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_INACTIVE          (1<<0)  // same as EDD_IRT_PORT_INACTIVE
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_TX                (2<<0)  // same as EDD_IRT_PORT_ACTIVE_TX_UP
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3PORTSTATUS_RX_TX             (3<<0)  // same as EDD_IRT_PORT_ACTIVE_TX_RX

#define EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_NOT_SUPPORTED     0       // same as EDD_RTCLASS2_NOT_SUPPORTED
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_INACTIVE          (1<<2)  // same as EDD_RTCLASS2_INACTIVE
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC2PORTSTATUS_ACTIVE            (2<<2)  // same as EDD_RTCLASS2_ACTIVE

#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_OFF              0       // same as EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_OFF
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_TX               (1<<4)  // same as EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_TX
#define EDDI_DEBUGINFO_PORTSTATUS1_RTC3REMOTESTATE_RX_TX            (2<<4)  // same as EDD_SET_REMOTE_PORT_STATE_RT_CLASS3_RXTX

typedef struct _EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_PORTSTATUS_TYPE
{
    LSA_UINT8       Status1;        // Bit 0..1:  RTC3PortStatus 
                                    // Bit 2..3:  RTC2PortStatus
                                    // Bit 4..5:  RtClass3_RemoteState
                                    // Bit 6..7:  reserved
    LSA_UINT8       Status2;        // Bit 0:  RtClass2_TopoState (1 = ok)
                                    // Bit 1:  RtClass3_TopoState (1 = ok) 
                                    // Bit 2:  RtClass3_PDEVTopoState (1 =ok)
                                    // Bit 3:  Neighbour_TopoState (1=ok)
                                    // Bit 4..7:  PortState (see EDD_PORT_STATE_...)
} EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_PORTSTATUS_TYPE;

typedef struct _EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE
{
    LSA_UINT8       Ident;          // EDD_DEBUGINFO_IND_PROVIDE_IDENT_EDDI
    LSA_UINT8       Global;         // Bit 0:   Valid (1= DebugInfo Byte 2..12  is valid/present) 
                                    // Bit 1:   SyncOK (1=SyncOK)
                                    // Bit 2..7: reserved 
    EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_PORTSTATUS_TYPE PortStatus[4];       
} EDDI_RQB_CSRT_IND_COMMON_DEBUGINFO_TYPE;


/* Byte 12..15 of DebugInfo, contained in EDD_RQB_CSRT_IND_DATA_TYPE */
/* (Event specific). Only valid if at least 1 MISS is present        */ 
#define EDDI_DEBUGINFO_FRAMETYPE_UNKNOWN        0x00
#define EDDI_DEBUGINFO_FRAMETYPE_DFP_SUBFRAME   0x05
#define EDDI_DEBUGINFO_FRAMETYPE_RTC123         0x04
#define EDDI_DEBUGINFO_FRAMETYPE_AUX            0x08 

typedef struct _EDDI_RQB_CSRT_IND_DATA_DEBUGINFO_TYPE
{
    LSA_UINT8   FrameType;  // Bit0: 0: no DFP Subframe, 1: DFP Subframe
                            // Bit1: DFP Subframe CRC (always 0)
                            // Bit2..3:	Frametype that triggered the event:
                            //          00: unknown
                            //          01: RTC1/2/3
                            //          10:	AUX
                            //Bit 4..7:	Nr of frames received since “SetToUnknown” or synchronous reset of the framecounter
                            //          0x0:	unknown
                            //          0x1…0xD:	1 to 13 frames
                            //          0xE:	>13 frames
                            //          0xF:	0 Frames
    LSA_UINT8   Reserved;
    LSA_UINT8   CycleCtrHigh;  
    LSA_UINT8   CycleCtrLow;  
} EDDI_RQB_CSRT_IND_DATA_DEBUGINFO_TYPE;


/*---------------------------------------------------------------------------*/
/* Services EDD_SRV_SRT_CONSUMER_CONROL, EDD_SRV_SRT_PROVIDER_CONTROL:       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_RQB_CSRT_CONTROL_DEBUGINFO_TYPE
{
    LSA_UINT8       Ident;          // EDD_DEBUGINFO_CONTROL_IDENT_EDDI
    LSA_UINT8       Reserved1;
    LSA_UINT8       ResponseHigh;
    LSA_UINT8       ResponseLow;
    LSA_UINT8       ModuleIDHigh;
    LSA_UINT8       ModuleIDLow;
    LSA_UINT8       LineHigh;
    LSA_UINT8       LineLow;
    LSA_UINT8       Reserved2[EDD_DEBUG_INFO_SIZE - 8];
} EDDI_RQB_CSRT_CONTROL_DEBUGINFO_TYPE;

/*===========================================================================*/
/*                        RQB (Request Block)                                */
/*===========================================================================*/

typedef struct EDDI_LOCAL_ERROR_CODE_
{
    LSA_UINT16    ModuleID;
    LSA_UINT16    Line;
    LSA_UINT32    Error;
    LSA_UINT32    Value;

} EDDI_LOCAL_ERROR_CODE_TYPE;

typedef struct EDDI_LOCAL_ERROR_CODE_   EDD_UPPER_RQB_ATTR *    EDDI_LOCAL_ERROR_CODE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDD_SRV_SETUP_PHY                       */
/*---------------------------------------------------------------------------*/

typedef struct _EDDI_RQB_SETUP_PHY_TYPE
{
    LSA_UINT16          PortID;
    LSA_UINT8           LinkSpeedMode;
    LSA_UINT8           PHYPower;
    LSA_UINT8           PHYReset;   //EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE

} EDDI_RQB_SETUP_PHY_TYPE;

/* LinkSpeedMode: */

/* PHYPower */
#define EDDI_PHY_POWER_ON         0  /* Switch the Phy on, -> Link-Ability    */
#define EDDI_PHY_POWER_OFF        1  /* Switch the Phy off -> No Link-Ability */
#define EDDI_PHY_POWER_UNCHANGED  2  /* No Changes on Phy                     */

typedef struct _EDDI_RQB_SETUP_PHY_TYPE    EDD_UPPER_MEM_ATTR *    EDDI_UPPER_SETUP_PHY_PTR_TYPE;

/*===========================================================================*/

/*===========================================================================*/
/*========================= SWITCH-INTERFACE ================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_SET_FDB_ENTRY           */
/*---------------------------------------------------------------------------*/
#define EDDI_SWI_FDB_PRIO_DEFAULT 0x01
#define EDDI_SWI_FDB_PRIO_3       0x02
#define EDDI_SWI_FDB_PRIO_ORG     0x03
#define EDDI_SWI_FDB_PRIO_FLUSS   0x04

typedef struct _EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE
{
    LSA_UINT8           PortID[EDD_CFG_MAX_PORT_CNT];   // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8           CHA;                            // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8           CHB;                            // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    EDD_MAC_ADR_TYPE    MACAddress;
    LSA_UINT16          Prio;
    LSA_UINT8           Pause;                          // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8           Filter;                         // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8           Borderline;                     // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE

} EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE;

/* typedef struct _EDDI_RQB_SWI_SET_FDB_ENTRY_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE; */

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_GET_FDB_ENTRY_MAC       */
/*---------------------------------------------------------------------------*/
#define EDDI_SWI_FDB_ENTRY_STATIC      0x01
#define EDDI_SWI_FDB_ENTRY_DYNAMIC     0x02

#define EDDI_SWI_FDB_ENTRY_INVALID      0
#define EDDI_SWI_FDB_ENTRY_VALID        1

typedef struct _EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE
{
    EDD_MAC_ADR_TYPE  MACAddress;
    LSA_UINT32        Index;
    LSA_UINT8         PortID[EDD_CFG_MAX_PORT_CNT];     // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         CHA ;                             // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         CHB ;                             // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT16        Type;
    LSA_UINT8         Valid;                            // EDDI_SWI_FDB_ENTRY_INVALID/ EDDI_SWI_FDB_ENTRY_VALID
    LSA_UINT16        Prio;
    LSA_UINT8         Pause;
    LSA_UINT8         Filter;                           // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         Borderline;                       // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE

} EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE;

typedef struct _EDDI_RQB_SWI_GET_FDB_ENTRY_MAC_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_GET_FDB_ENTRY_INDEX     */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_GET_FDB_ENTRY_INDEX_TYPE
{
    EDD_MAC_ADR_TYPE  MACAddress;
    LSA_UINT32        Index;
    LSA_UINT8         PortID[EDD_CFG_MAX_PORT_CNT];     // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         CHA ;                             // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         CHB ;                             // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT16        Type;                 
    LSA_UINT8         Valid;                            // EDDI_SWI_FDB_ENTRY_INVALID/ EDDI_SWI_FDB_ENTRY_VALID
    LSA_UINT16        Prio;                             // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         Pause;                            // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         Filter;                           // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE
    LSA_UINT8         Borderline;                       // EDD_FEATURE_DISABLE/EDD_FEATURE_ENABLE

} EDDI_RQB_SWI_GET_FDB_ENTRY_INDEX_TYPE;

typedef struct _EDDI_RQB_SWI_GET_FDB_ENTRY_INDEX_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_REMOVE_FDB_ENTRY        */
/*---------------------------------------------------------------------------*/
#define EDDI_SWI_FDB_ENTRY_NOT_EXIST    0
#define EDDI_SWI_FDB_ENTRY_EXISTS       1

typedef struct _EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE
{
    LSA_UINT8           FDBEntryExist;      //EDDI_SWI_FDB_ENTRY_NOT_EXIST / EDDI_SWI_FDB_ENTRY_EXISTS
    EDD_MAC_ADR_TYPE    MACAddress;

} EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE;

typedef struct _EDDI_RQB_SWI_REMOVE_FDB_ENTRY_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_CLEAR_DYN_FDB           */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_CLEAR_DYN_FDB_TYPE
{
    LSA_UINT32    dummy;

} EDDI_RQB_SWI_CLEAR_DYN_FDB_TYPE;

typedef struct _EDDI_RQB_SWI_CLEAR_DYN_FDB_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_CLEAR_DYN_FDB_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_SET_AGING_TIME          */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_SET_AGING_TIME_TYPE
{
    LSA_UINT16    AgePollTime;

} EDDI_RQB_SWI_SET_AGING_TIME_TYPE;

typedef struct _EDDI_RQB_SWI_SET_AGING_TIME_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_SET_AGING_TIME_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_GET_AGING_TIME          */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_GET_AGING_TIME_TYPE
{
    LSA_UINT16    AgePollTime;

} EDDI_RQB_SWI_GET_AGING_TIME_TYPE;

typedef struct _EDDI_RQB_SWI_GET_AGING_TIME_TYPE EDD_UPPER_MEM_ATTR * EDDI_UPPER_SWI_GET_AGING_TIME_PTR_TYPE;

#if defined EDDI_CFG_MIRROR_MODE_INCLUDE

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_SET_PORT_MONITOR        */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_SET_PORT_MONITOR_TYPE
{
    LSA_UINT8 PortIDMonitorCyclic [EDD_CFG_MAX_PORT_CNT];
    LSA_UINT8 PortIDMonitorAcyclic[EDD_CFG_MAX_PORT_CNT];

} EDDI_RQB_SWI_SET_PORT_MONITOR_TYPE;

typedef struct _EDDI_RQB_SWI_SET_PORT_MONITOR_TYPE  EDD_UPPER_MEM_ATTR  * EDDI_UPPER_SWI_SET_PORT_MONITOR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_GET_PORT_MONITOR        */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_GET_PORT_MONITOR_TYPE
{
    LSA_UINT8 PortIDMonitorCyclic [EDD_CFG_MAX_PORT_CNT];
    LSA_UINT8 PortIDMonitorAcyclic[EDD_CFG_MAX_PORT_CNT];

} EDDI_RQB_SWI_GET_PORT_MONITOR_TYPE;

typedef struct _EDDI_RQB_SWI_GET_PORT_MONITOR_TYPE  EDD_UPPER_MEM_ATTR  * EDDI_UPPER_SWI_GET_PORT_MONITOR_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_SET_MIRROR_PORT         */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_SET_MIRROR_PORT_TYPE
{
    LSA_UINT8 PortID[EDD_CFG_MAX_PORT_CNT];

} EDDI_RQB_SWI_SET_MIRROR_PORT_TYPE;

typedef struct _EDDI_RQB_SWI_SET_MIRROR_PORT_TYPE  EDD_UPPER_MEM_ATTR  * EDDI_UPPER_SWI_SET_MIRROR_PORT_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* Opcode: EDD_OPC_REQUEST  Service: EDDI_SRV_SWITCH_GET_MIRROR_PORT         */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_SWI_GET_MIRROR_PORT_TYPE
{
    LSA_UINT8 PortID[EDD_CFG_MAX_PORT_CNT];

} EDDI_RQB_SWI_GET_MIRROR_PORT_TYPE;

typedef struct _EDDI_RQB_SWI_GET_MIRROR_PORT_TYPE  EDD_UPPER_MEM_ATTR  * EDDI_UPPER_SWI_GET_MIRROR_PORT_PTR_TYPE;

#endif //end of EDDI_CFG_MIRROR_MODE_INCLUDE

/*===========================================================================*/
/*========================= SYNC-INTERFACE ==================================*/
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
/* Opcode: -  Service: - Dummy for removed opcodes                           */
/*---------------------------------------------------------------------------*/
typedef struct _EDDI_RQB_DUMMY_PARAM_TYPE
{
    LSA_UINT32  dummy;

} EDDI_RQB_DUMMY_PARAM_TYPE;

typedef struct _EDDI_RQB_DUMMY_PARAM_TYPE  EDD_UPPER_MEM_ATTR  * EDDI_UPPER_DUMMY_PARAM_PTR_TYPE;

/*===========================================================================*/
/*=========== Synchronous interface for isochronous-mode support =========== */
/*===========================================================================*/
#define EDDI_APPLSYNC_INTSRC_HP0    1               //interrupt-sources: IRTE hostproc controller 0
#define EDDI_APPLSYNC_INTSRC_HP1    2               //interrupt-sources: IRTE hostproc controller 1
#define EDDI_APPLSYNC_INTSRC_SP0    3               //interrupt-sources: IRTE switchproc controller 0
#define EDDI_APPLSYNC_INTSRC_SP1    4               //interrupt-sources: IRTE switchproc controller 1

/*===========================================================================*/
/*========================= PROVIDER-AUTOSTOP ===============================*/
/*===========================================================================*/
#define EDDI_CONSUMER_ID_INVALID     0xFFFE

/*===========================================================================*/
/*========================= Signaling SendClock-change ======================*/
/*===========================================================================*/
#define EDDI_SENDCLOCK_CHANGE_XPLLOUT_DISABLED  1 //the signal that can be routed to xpllout will be disabled 
                                                  //(e.g. before changing the sendclock)
#define EDDI_SENDCLOCK_CHANGE_XPLLOUT_ENABLED   2 //the signal that can be routed to xpllout has been enabled

/*===========================================================================*/
/*                                prototyping                                */
/*===========================================================================*/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

#endif //EDDI_USR_H


/*****************************************************************************/
/*  end of file eddi_usr.h                                                   */
/*****************************************************************************/

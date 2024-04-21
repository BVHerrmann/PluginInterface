#ifndef EDDI_INT_H              //reinclude-protection
#define EDDI_INT_H

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
/*  F i l e               &F: eddi_int.h                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Internal headerfile:                                                     */
/*  Defines, internal constants, types, data, macros and prototyping for     */
/*  EDDI.                                                                    */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  17.02.04    ZR    add includes for new icc modul                         */
/*                                                                           */
/*****************************************************************************/

/*---------------------------------------------------------------------------*/
/* maximum number of ports supported                                         */
/*---------------------------------------------------------------------------*/
// allowed: 1..4
#define EDDI_MAX_IRTE_PORT_CNT  4UL
#define EDDI_MAX_HSYNC_PORT_CNT 2UL

#include "eddi_inc.h"

#if (EDD_CFG_MAX_PORT_CNT < EDDI_MAX_IRTE_PORT_CNT)
    #error "EDD_CFG_MAX_PORT_CNT wrong"
#endif

/*===========================================================================*/
/* Internal Settings                                                         */
/*===========================================================================*/

/**** All revisions ****/
#define EDDI_RED_PHASE_SHIFT_ON             //RED-PHASE-SHIFT

#define EDDI_FIXED_ACW_PROVIDER_SEQUENCE       //Workaround for AP00543330

#define EDDI_CFG_APPLY_IRTPU_RESTRICTIONS      // Restrictions to IRT-PU until further notice: 1) SCF is limited to 125us, 187.5us and every value above or equal to 250us in TIAv14
                                               //                                              2) For SCF<8 RR is limited to 1

//#define EDDI_CFG_CONSTRACE_ADVANCED          //pcIOX only! uses huge amounts of memory
//#define EDDI_CFG_UP_ON_REMOTE_TOPO_MISMATCH  //REMOTE_TOPO_MISMATCH leads to RTC3PSM-State UP, not to OFF
//#define EDDI_CFG_DISABLE_PROVIDER_AUTOSTOP   //provider-autostop is never switched on, EDD_CONS_CONTROL_MODE_PROV_AUTOSTOP_ENABLE is ignored

//#define EDDI_CFG_DISABLE_ARP_FILTER          //disable EDDI-ARP-Filter
//#define EDDI_CFG_DISABLE_DCP_FILTER          //disable EDDI-DCP-Filter
//#define EDDI_CFG_DISABLE_DCP_HELLO_FILTER    //disable EDDI-DCP-Hello-Filter
#define EDDI_CFG_EXTLINKIND_FILTER_LINEDELAY_TOLERANCE      25   //hysteresis for tracing linedelay-changes in ns!
//#define EDDI_CFG_ENABLE_FAST_SB_TIMING       //record min/max time of SB-check ATTENTION:Uses 16B uncalculated KRAM-Space!
//#define EDDI_CFG_RTC3REMOVE_WITHOUT_PASSIVATING //remove includes passivating, even for RTC3 frames (nonlegacy only!)
#define EDDI_CFG_ACCEPT_INVALID_ADDS			//eddi_SysRed_Provider_Add accepts invalid buffer ptrs
#define EDDI_CFG_LIMIT_NRTPCI_WRITE_CNT         4 //if set, rx/tx-enable is not written for each DMACW
//#define EDDI_CFG_SYNCFRAME_FWD_BASED_ON_MRP3P   //if defined, SYNCFRAME forwarding is based on "if MRP is active, and we are SyncMaster or have more than 2 ports (MRP with stub), we have to deactivate HW-forwarding"

/* No of ARP filters. Setting is internal until all EDDs support the same amount again */

/**** Revision-specific ****/
#if defined (EDDI_CFG_REV5)
#define EDDI_MAX_ARP_FILTER                     1

#define EDDI_CFG_ENABLE_PADDING_CLASS12        //Enable Padding for NRT. Class 1/Class 2-Frames cannot be padded, because the vlan-tag has
                                               //not been considered! Therefore EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT is set!
#define EDDI_CFG_DONT_ACCEPT_TS_ZOMBIE_INT     //don't accept TS-Zombie-interrupts in TS-SM for Rev5 (TS-INT but no TS-RQB existent or 2 TS-INT with same timestamp)!
//#define EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1    //Enable system redundancy support for RTC1 Prov/Cons

#elif defined (EDDI_CFG_REV6)
#define EDDI_MAX_ARP_FILTER                     1

#define EDDI_CFG_ENABLE_PADDING_CLASS12        //Enable Padding for NRT. Class 1/Class 2-Frames cannot be padded, because the vlan-tag has
                                               //not been considered! Therefore EDD_HW_TYPE_FEATURE_NO_AUTOPADDING_CLASS_12_SUPPORT is set!
//#define EDDI_CFG_ENABLE_MRPD_ASYNC_PROVIDER  //Enable Async Provider as 1 Buffer Mode, even if MRPD on Rev6 allows SYNC providers only
//#define EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1    //Enable system redundancy support for RTC1 Prov/Cons

#elif defined (EDDI_CFG_REV7)
#define EDDI_MAX_ARP_FILTER                     2

#define EDDI_CFG_ENABLE_PADDING_CLASS12        //Enable Padding for NRT, Class 1/Class 2
#define EDDI_CFG_ENABLE_PADDING_CLASS3         //only evaluated if EDDI_CFG_ENABLE_PADDING_CLASS12 is set!
                                               //pay attention to EDD_HW_TYPE_FEATURE_AUTOPADDING_CLASS_3_SUPPORT!
#define EDDI_CFG_ENABLE_NONBIN_SCF_CLASS12   //enable nonbinary SCFs with RR>1 for RTC1/2 (ACW-Tree)
//#define EDDI_CFG_ENABLE_NONBIN_SCF_CLASS3    //enable nonbinary SCFs with RR>1 for RTC3 (FCW-Tree)
#define EDDI_CFG_ENABLE_SYSRED_SUPPORT_RTC1    //Enable system redundancy support for RTC1 Prov/Cons

#define EDDI_CFG_DFP_MAX_PACKFRAMES_INTERNAL         8   /* max nr of DFP-packframes supported */
//#define EDDI_CFG_ENABLE_PHY_REGDUMP_NSC       //enable regdump of NSC phy to LSA trace if record 0xB050 is read
//#define EDDI_CFG_TRACE_HSA                    //enable trace of HSYNC frames (HSA) with WARN level
#endif

/*===========================================================================*/

#define EDDI_NULL_PTR    0

#include "eddi_m_id.h"

#include "eddi_ser_adr_r6.h"
#include "eddi_ser_adr_r7.h"
#include "eddi_ser_adr.h"

#include "eddi_ser_reg.h"
#include "eddi_ser_typ.h"
#include "eddi_bf.h"

#define EDDI_SER_CMD_STAT

#define EDDI_CNS_TRY_CNT                   0x8UL

#define EDDI_ETHERTYPE                     0x8892

/* Not used yet -> currently will lead to an exception! */
//#define EDDI_SYNC_IMAGE_CHBUF_MODE       0x3 //sync double buffered  -> not yet supported!!
#define EDDI_SYNC_IMAGE_OFF                0x0

#if defined (TOOL_CHAIN_MS_WINDOWS)
#pragma warning (disable:4805) /* warning C4805: '!=' : unsafe combination from type 'unsigned long' with type 'bool' in the same operation*/
#endif

#define EDDI_TIMER_TYPE_ONE_SHOT           (LSA_UINT16)LSA_TIMER_TYPE_ONE_SHOT
#define EDDI_TIMER_TYPE_CYCLIC             (LSA_UINT16)LSA_TIMER_TYPE_CYCLIC

#define EDDI_TIME_BASE_1US                 (LSA_UINT16)LSA_TIME_BASE_1US
#define EDDI_TIME_BASE_10US                (LSA_UINT16)LSA_TIME_BASE_10US
#define EDDI_TIME_BASE_100US               (LSA_UINT16)LSA_TIME_BASE_100US
#define EDDI_TIME_BASE_1MS                 (LSA_UINT16)LSA_TIME_BASE_1MS
#define EDDI_TIME_BASE_10MS                (LSA_UINT16)LSA_TIME_BASE_10MS
#define EDDI_TIME_BASE_100MS               (LSA_UINT16)LSA_TIME_BASE_100MS
#define EDDI_TIME_BASE_1S                  (LSA_UINT16)LSA_TIME_BASE_1S
#define EDDI_TIME_BASE_10S                 (LSA_UINT16)LSA_TIME_BASE_10S
#define EDDI_TIME_BASE_100S                (LSA_UINT16)LSA_TIME_BASE_100S

#if defined (EDDI_CFG_XRT_OVER_UDP_INCLUDE)
#if defined (EDDI_CFG_REV7)
#elif defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV6)
#define EDDI_XRT_OVER_UDP_SOFTWARE
#else
#error "unsupported revision!"
#endif
#endif

#define EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS           (EDD_CSRT_DSTAT_LOCAL_INIT_VALUE & (~EDD_CSRT_DSTAT_BIT_DATA_VALID))
#define EDDI_CSRT_DSTAT_LOCAL_INIT_VALUE_CONS_BACKUP    (EDD_CSRT_DSTAT_LOCAL_INIT_VALUE & (~(EDD_CSRT_DSTAT_BIT_DATA_VALID+EDD_CSRT_DSTAT_BIT_STATE)))

/*---------------------------------------------------------------------------*/
/* type/len-field types                                                      */
/*---------------------------------------------------------------------------*/
#ifndef EDDI_CFG_BIG_ENDIAN
#define EDDI_VLAN_TAG                            0x0081
#define EDDI_xRT_TAG                             0x9288
#define EDDI_LLDP_TAG                            0xCC88
#define EDDI_IP_TAG                              0x0008
#define EDDI_ARP_TAG                             0x0608
#define EDDI_RARP_TAG                            0x3580    /* reversed ARP */
#define EDDI_MRP_TAG                             0xE388
#define EDDI_NRT_FRAG                            0x9288
#define EDDI_HSYNC_TAG                           0x0A80
#else
#define EDDI_xRT_TAG                             0x8892
#define EDDI_LLDP_TAG                            0x88CC
#define EDDI_IP_TAG                              0x0800
#define EDDI_ARP_TAG                             0x0806
#define EDDI_RARP_TAG                            0x8035    /* reversed ARP */
#define EDDI_VLAN_TAG                            0x8100
#define EDDI_MRP_TAG                             0x88E3
#define EDDI_NRT_FRAG                            0x8892
#define EDDI_HSYNC_TAG                           0x800A
#endif

/*---------------------------------------------------------------------------*/
/* xRToverUDP UDP-Port                                                       */
/*---------------------------------------------------------------------------*/
#ifndef EDDI_CFG_BIG_ENDIAN
#define EDDI_xRT_UDP_PORT                        0x9288
#else
#define EDDI_xRT_UDP_PORT                        0x8892
#endif

/*---------------------------------------------------------------------------*/

// new frame-id-classes
#define EDDI_SRT_FRAMEID_RT_CLASS1_START                   EDD_SRT_FRAMEID_CSRT_START2
#define EDDI_SRT_FRAMEID_RT_CLASS1_STOP                    EDD_SRT_FRAMEID_CSRT_STOP2
#define EDDI_SRT_FRAMEID_RT_CLASS2_START                   EDD_SRT_FRAMEID_CSRT_START1
#define EDDI_SRT_FRAMEID_RT_CLASS2_STOP                    EDD_SRT_FRAMEID_CSRT_STOP1
#define EDDI_SRT_FRAMEID_RT_CLASS2DFP_START                EDD_SRT_FRAMEID_DFP_START
#define EDDI_SRT_FRAMEID_RT_CLASS2DFP_STOP                 EDD_SRT_FRAMEID_DFP_STOP
#define EDDI_SRT_FRAMEID_RT_CLASS3_START                   EDD_SRT_FRAMEID_IRT_START
#define EDDI_SRT_FRAMEID_RT_CLASS3_STOP                    EDD_SRT_FRAMEID_IRT_STOP

/*===========================================================================*/
/*                                  types                                    */
/*===========================================================================*/

typedef struct _EDDI_HDB_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_HDB_PTR_TYPE;

typedef struct _EDDI_DDB_TYPE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_PTR_TYPE;

typedef struct _EDDI_RQB_QUEUE_TYPE
{
    EDD_UPPER_RQB_PTR_TYPE    pBottom;
    EDD_UPPER_RQB_PTR_TYPE    pTop;

    LSA_UINT32                Cnt;

} EDDI_RQB_QUEUE_TYPE;

typedef struct _EDDI_SHED_OBJ_TYPE
{
    LSA_BOOL                  bUsed;
    EDD_RQB_TYPE              IntRQB;
    LSA_UINT32                Para;

} EDDI_SHED_OBJ_TYPE;

//struct _EDDI_DDB_TYPE;
typedef  LSA_VOID  ( EDDI_LOCAL_FCT_ATTR  *  EDDI_ISR_FCT )( struct _EDDI_DDB_TYPE  *  const  pDDB,
                                                             LSA_UINT32                const  para_1 );

#define EDDI_MAX_ISR    0x45

typedef struct  EDDI_ISR_S
{
    EDDI_ISR_FCT    fct;
    LSA_UINT32      para_1;

} EDDI_ISR_T;

#include "eddi_isr_co.h"
#include "eddi_mem.h"

#include "eddi_nrt_frame.h"
#include "eddi_nrt_int.h"

#include "eddi_ser_usr.h"

#define EDDI_MAX_DB_PER_FRAME    13

#define EDDI_RES_DB_LIMIT_DOWN      ( ( (pDDB->PM.PortMap.PortCnt + 1) * EDDI_MAX_DB_PER_FRAME ) + 1 )
#define EDDI_RES_FCW_LIMIT_DOWN     ( ( (pDDB->PM.PortMap.PortCnt + 1) * pDDB->PM.PortMap.PortCnt     ) + 1 )

/*===========================================================================*/
/*                                chain-macros                               */
/*===========================================================================*/

/*=============================================================================
 * function name:  EDDI_PUT_BLOCK_BOTTOM
 *
 * function:       insert block in queue on bottom position
 *
 * parameters:     _Bottom, _Top, _Block
 *
 * return value:
 *
 *===========================================================================*/
#define EDDI_PUT_BLOCK_BOTTOM(_Bottom, _Top, _Block)    \
{                \
_Block->next_dyn_ptr = _Bottom;         \
_Block->prev_dyn_ptr = EDDI_NULL_PTR;         \
\
if( ((_Bottom) == EDDI_NULL_PTR) )   \
{               \
_Bottom = _Block;          \
_Top = _Block;          \
}               \
else              \
{               \
_Bottom->prev_dyn_ptr = _Block;        \
_Bottom   = _Block;        \
}               \
}                \

/*=============================================================================
 * function name: EDDI_REM_TOP
 *
 * function:  remove block from queue on any position
 *
 * parameters:  _Bottom, _Top, _Block
 *
 * return value: _Block
 *
 *===========================================================================*/
#define EDDI_REM_BLOCK_NO_CAST(_Bottom, _Top, _Block)               \
{                                                          \
if( !((_Block->prev_dyn_ptr) == EDDI_NULL_PTR) )                            \
{                                                                                          \
(_Block->prev_dyn_ptr)->next_dyn_ptr = _Block->next_dyn_ptr;      \
}                                                                                          \
if( !((_Block->next_dyn_ptr) == EDDI_NULL_PTR) )                            \
{                                                                                          \
(_Block->next_dyn_ptr)->prev_dyn_ptr = _Block->prev_dyn_ptr;      \
}                                                                                          \
if( ((_Top) == (_Block)) )            \
{                                                         \
_Top = _Block->prev_dyn_ptr;     \
}                                                         \
if( ((_Bottom) == (_Block)) )         \
{                                                         \
_Bottom = _Block->next_dyn_ptr;  \
}                                                         \
}                                                          \

/*=============================================================================
 * function name: EDDI_GET_NEXT_TOP_NO_CAST
 *
 * function:  remove block from queue on bottom position
 *
 * parameters:  _Bottom, _Top, _Block
 *
 * return value: _Block
 *
 *===========================================================================*/
#define EDDI_GET_NEXT_TOP_NO_CAST(_Top, _Block)    \
{                \
_Block = _Top;            \
\
if( !((_Top) == EDDI_NULL_PTR) )    \
{               \
_Top = (_Top)->prev_dyn_ptr;  /*lint !e961 */        \
}               \
}                \

/*=============================================================================
 * function name: EDDI_REM_BLOCK_TOP
 *
 * function:  remove block from queue on bottom position
 *
 * parameters:  _Bottom, _Top, _Block
 *
 * return value: _Block
 *
 *===========================================================================*/
#define EDDI_REM_BLOCK_TOP_NO_CAST(_Bottom, _Top, _Block)    \
{                \
_Block = _Top;            \
\
if( !((_Top) == EDDI_NULL_PTR) )    \
{               \
_Top = _Top->prev_dyn_ptr;          \
\
        if (((_Top) == EDDI_NULL_PTR))                      \
        {                                                   \
            _Bottom = EDDI_NULL_PTR;                        \
        }                                                   \
        else                                                \
        {                                                   \
            _Top->next_dyn_ptr = EDDI_NULL_PTR;             \
        }                                                   \
    }                                                       \
}

// predefine
typedef struct _EDDI_QUEUE_ELEMENT_TYPE * EDDI_QUEUE_ELEMENT_PTR_TYPE;

typedef struct _EDDI_QUEUE_ELEMENT_TYPE
{
    EDDI_QUEUE_ELEMENT_PTR_TYPE  next_ptr;
    EDDI_QUEUE_ELEMENT_PTR_TYPE  prev_ptr;

} EDDI_QUEUE_ELEMENT_TYPE;

typedef struct _EDDI_QUEUE_TYPE
{
    EDDI_QUEUE_ELEMENT_PTR_TYPE  pFirst;
    EDDI_QUEUE_ELEMENT_PTR_TYPE  pLast;
    LSA_UINT32                  Count;

} EDDI_QUEUE_TYPE;

typedef struct _EDDI_QUEUE_TYPE * EDDI_QUEUE_PTR_TYPE;

#if defined (EDDI_CFG_REV7)
#if defined (EDDI_CFG_BIG_ENDIAN)
#define EDDI_ACW_xRT_UDP_PORT                        0x9288
#define EDDI_IP_TTL_UDP_DEFAULT                      0x4000 /* 64 sec */
#else
#define EDDI_ACW_xRT_UDP_PORT                        0x8892
#define EDDI_IP_TTL_UDP_DEFAULT                      0x0040 /* 64 sec */
#endif
#endif

#include "eddi_glb.h"
#include "eddi_pm.h"

#define EDDI_IMAGE_MODE_SYNC         0x01
#define EDDI_IMAGE_MODE_ASYNC        0x02
#define EDDI_IMAGE_MODE_STAND_ALONE  (EDDI_IMAGE_MODE_SYNC | EDDI_IMAGE_MODE_ASYNC)

#define EDDI_IMAGE_MODE_UNDEFINED    0xFF

// EDDI_SYNC_IMAGE_MODE-Defines for BufferMode in EDDI_CRT_INI_TYPE
//#define EDDI_SYNC_IMAGE_UNBUF_MODE 0x1
#define EDDI_SYNC_IMAGE_BUF_MODE     0x2

#define EDDI_AUX_PROVIDER_RR                         128   //fixed redrat for AUX-providers
#define EDDI_AUX_MIN_WDT_TIMEOUT_MS                  256   //minimum WDT timeout in ms for AUX-consumers
#define EDDI_MAX_WDT_TIMEOUT_MS                      1920  //maximum WDT timeout in ms for all consumers
#define EDDI_CSRT_PROV_PROP_RTCLASS_AUX              0x8005
#define EDDI_CSRT_CONS_PROP_RTCLASS_AUX              0x8005
#define EDDI_HWPORTINDEX_INVALID                     0xFF
#define EDDI_TIMEOUT_POWERUP_DELAY_FOR_SSD_WA_100MS  30  //3 sec
#define EDDI_TIMEOUT_POLL_FOR_PHY_RESET_10NS         500000 //5000 µs, 10ns ticks 

#define EDDI_FRAME_HEADER_SIZE                       (2 * EDD_MAC_ADDR_SIZE /* DA and SA MAC Addr*/ + sizeof(LSA_UINT16) /*ethertype*/ + sizeof(LSA_UINT16) /*frameid*/ + sizeof(EDDI_CRT_DATA_APDU_STATUS)) 
#define EDDI_CRC32_SIZE                              4

#define EDDI_MAX_IRTE_TRANSFER_END_DELAY             30000 //30µs
#define EDDI_TIME_BYTE_RESOLUTION_RT_FRAME_NS        80 //ns
#define EDDI_SDU_MAX_LENGTH		                     1440

#define EDDI_R5_RX_APPL_DELAY_MAX                    30700 //ns
#define EDDI_R6_RX_APPL_DELAY_MAX                    30000  //ns
#define EDDI_R7_RX_APPL_DELAY_MAX                    (pDDB->PM.PortMap.PortCnt-1) * 1000 + 28000 //ns

#define EDDI_R5_RX_APPL_DELAY_1                      185 //18,5 ns (18,5*10)
#define EDDI_R6_RX_APPL_DELAY_1                      184 //18,4 ns (18,4*10)
#define EDDI_R7_RX_APPL_DELAY_1                      16 //16,0 ns 

#define EDDI_R5_RX_APPL_DELAY_2                      4000 //ns
#define EDDI_R6_RX_APPL_DELAY_2                      3500 //ns
#define EDDI_R7_RX_APPL_DELAY_2                      200 //ns
#define EDDI_R7_RX_APPL_DELAY_3                      5000 //ns

#define EDDI_CALC_ROUNDn(val_, n_)                   (((val_)+(n_)-1UL) & ((0xFFFFFFFFUL-(n_))+1))


// EDDI Calc64
typedef struct _EDDI_UINT64
{
    LSA_UINT32   High;
    LSA_UINT32   Low;

} EDDI_UINT64;

typedef struct _EDDI_INT64
{
    LSA_INT32    High;
    LSA_UINT32   Low;

} EDDI_INT64;

#define EDDI_MIN(_a,_b) (/*lint --e(506)*/((_a) < (_b)) ? (_a) : (_b))
#define EDDI_MAX(_a,_b) (/*lint --e(506)*/((_a) > (_b)) ? (_a) : (_b))

#define ADD_UI64_AND_UI32_WITH_CARRY(_high, _low, _add_ui32, _carry) \
{ \
  LSA_UINT32 const _min = EDDI_MIN(_add_ui32, _low);\
  (_carry) = 0UL;                          \
  (_low)  = (_low) + (_add_ui32);          \
  if ((_low) < _min )                      \
  {                                        \
    (_high)  = (_high) + (1UL);            \
    if ((_high) == 0)                      \
    {                                      \
      _carry = 1UL;                        \
    }                                      \
  }                                        \
}                                          \

// Bitmask to get MSB-Bit (Sign-Bit) of an int32-Value;
#define SIGN_BIT31   0x80000000UL

// Check if High_Value is a negative value
#define EDDI_INT64_IS_NEGATIVE(_highPart) ((_highPart) & SIGN_BIT31))


#define LIMIT_LSA_INT32_MAX 0x7FFFFFFFL
#define LIMIT_LSA_INT32_MIN ((LSA_INT)(-1L * LIMIT_LSA_INT32_MAX))


#define GET_HIGH16_OF_UI32(_ui32)\
((_ui32) >> 16)

#define GET_LOW16_OF_UI32(_ui32)\
((_ui32) & 0x0000FFFFUL)

#define SHIFT_LEFT_64BIT(_high, _low, _count, _carry) \
{                                                            \
    (_carry) =  (_high)  >> (32UL - (_count));               \
    (_high)  =  (_high)  << (_count);                        \
    (_high)  =  (_high)  | ( (_low) >> (32UL - (_count)) );  \
    (_low)   =  (_low)   << (_count);                        \
}


#define EDDI_POINTER_BYTE_DISTANCE(_HighPointer, _LowPointer)  \
    (LSA_UINT32)((LSA_UINT32)((void*)(_HighPointer)) - (LSA_UINT32)(((void*)(_LowPointer))))

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_INT_H


/*****************************************************************************/
/*  end of file eddi_int.h                                                   */
/*****************************************************************************/

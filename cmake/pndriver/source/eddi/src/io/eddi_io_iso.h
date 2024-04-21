#ifndef EDDI_IO_ISO_H           //reinclude-protection
#define EDDI_IO_ISO_H

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
/*  F i l e               &F: eddi_io_iso.h                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* Defines for isochronous support                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

#include "eddi_io_iso_cfg.h"

/*===========================================================================*/
/*                              Types                                        */
/*===========================================================================*/
typedef enum EDDI_CONFIG_APPLCLK_CMD_
{
  EDDI_ISO_DEFAULT_APPLCLK_ENABLE,   /* enable applclk for default use (xpllout) */
  EDDI_ISO_DEFAULT_APPLCLK_DISABLE,  /* disable applclk for default use */
  EDDI_ISO_DEFAULT_APPLCLK_SET,      /* set values for DIV and PERIOD only */
  EDDI_ISO_DEFAULT_APPLCLK_START,    /* start timer */
  EDDI_ISO_DEFAULT_APPLCLK_STOP      /* stop timer */

} EDDI_CONFIG_APPLCLK_CMD;

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_IsoInitDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE            const pDDB,
                                                           EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE const pCyclComp );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_IsoCtrlDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                             EDDI_CONFIG_APPLCLK_CMD const Cmd,
                                                             LSA_UINT32              const AplClk_Divider,
                                                             LSA_UINT32              const ClkCy_Period );

#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)

/*===========================================================================*/
/*                              Settings                                     */
/*===========================================================================*/

#define EDDI_ISO_CFG_MAX_CACF                   16          //Highest CACF supported
#define EDDI_ISO_CFG_MAX_RR                     512         //Highest ReductionRatio supported
                                                            //Limit: (8*MAX_CACF*MAX_RR)<65536, because AplClk_Divider/ClkCy-Divider regs have only 16bit!
#define EDDI_ISO_CFG_MAX_APPL_CYCLE_TIME_NS     512000000   //longest ApplCycle (unit: ns) supported
                                                            //Limit: 31250ns*65535
#define EDDI_ISO_CFG_MAX_READ_CTR               8           //try counts to read consistent counter values. If not defined no consistency is provided

/*===========================================================================*/
/*                              Defines                                      */
/*===========================================================================*/
#define EDDI_ISO_NS_PER_TICK            10    //Timer-resolution Rev5/6/7: 100MHz

#define EDDI_ISO_INSTANCE_NOT_USED      0xFFFF

//VersionNr registers without debug-version
#if defined (EDDI_CFG_REV5)
#define EDDI_ISO_IRTE_VERSION_NR1     0x20051600  //ERTEC400
#define EDDI_ISO_IRTE_VERSION_NR2     0x20051600
#elif defined (EDDI_CFG_REV6)
#define EDDI_ISO_IRTE_VERSION_NR1     0x3B060800  //ERTEC200
#define EDDI_ISO_IRTE_VERSION_NR2     0x3B060800
#elif defined (EDDI_CFG_REV7)
#define EDDI_ISO_IRTE_VERSION_NR1     0x47070900 //SOC1
#define EDDI_ISO_IRTE_VERSION_NR2     0x5F070900 //SOC2
#endif

//Interrupt-bits
#define EDDI_ISO_INT_Clkcy_Int          0x00002000  //Bit 13 
#define EDDI_ISO_INT_Comp1              0x00020000  //Bit 17 
#if defined (EDDI_ISO_CFG_NO_ZE_INT)
#define EDDI_ISO_INT_Comp2            0           //not used 
#else
#define EDDI_ISO_INT_Comp2            0x00040000  //Bit 18 
#endif
#if defined (EDDI_ISO_CFG_NO_ZS_INT)
  #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
    #define EDDI_ISO_INT_Comp3      0           //not used 
    #define EDDI_ISO_INT_Comp4      0           //not used
  #else
    #define EDDI_ISO_INT_Comp3      0           //not used
    #define EDDI_ISO_INT_Comp4      0           //not used
  #endif
#else
  #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
    #define EDDI_ISO_INT_Comp3      0x00080000  //Bit 19
    #define EDDI_ISO_INT_Comp4      0x00100000  //Bit 20 
  #else
    #define EDDI_ISO_INT_Comp3      0x00080000  //Bit 19
    #define EDDI_ISO_INT_Comp4      0           //not used
  #endif
#endif
#define EDDI_ISO_INT_Comp5              0x00200000  //Bit 21
#if defined (EDDI_ISO_CFG_PASS_INTREG)
#define EDDI_ISO_INT_NewCycle           0x00000001  //Bit 00
#endif

//assignment of the comparators to the ISO-events
#define EDDI_ISO_INT_ZA                 EDDI_ISO_INT_Clkcy_Int
#define EDDI_ISO_INT_ZM                 EDDI_ISO_INT_Comp1
#define EDDI_ISO_INT_ZE                 EDDI_ISO_INT_Comp2
#if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
  #define EDDI_ISO_INT_ZSI            EDDI_ISO_INT_Comp3
  #define EDDI_ISO_INT_ZSO            EDDI_ISO_INT_Comp4
  #define EDDI_ISO_INT_ZS             (EDDI_ISO_INT_ZSI | EDDI_ISO_INT_ZSO)
#else
    #define EDDI_ISO_INT_ZS             EDDI_ISO_INT_Comp3
#endif
#define EDDI_ISO_INT_ZIT                EDDI_ISO_INT_Comp5

#define EDDI_ISO_COMP_ZM                CLK_COMP_VALUE1
#define EDDI_ISO_COMP_ZE                CLK_COMP_VALUE2
#if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
  #define EDDI_ISO_COMP_ZSI           CLK_COMP_VALUE3
  #define EDDI_ISO_COMP_ZSO           CLK_COMP_VALUE4
  //#define EDDI_ISO_COMP_ZS            (EDDI_ISO_COMP_ZSI | EDDI_ISO_COMP_ZSO)
#else
  #define EDDI_ISO_COMP_ZS                CLK_COMP_VALUE3
#endif
#define EDDI_ISO_COMP_ZIT               CLK_COMP_VALUE5

#define EDDI_ISO_ENABLE_COMP_ZM         PARA_ENABLE_COMP1_IRQ
#define EDDI_ISO_ENABLE_COMP_ZE         PARA_ENABLE_COMP2_IRQ
#if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
  #define EDDI_ISO_ENABLE_COMP_ZSI        PARA_ENABLE_COMP3_IRQ
  #define EDDI_ISO_ENABLE_COMP_ZSO        PARA_ENABLE_COMP4_IRQ
  #define EDDI_ISO_ENABLE_COMP_ZS         (EDDI_ISO_ENABLE_COMP_ZSI | EDDI_ISO_ENABLE_COMP_ZSO)
#else
  #define EDDI_ISO_ENABLE_COMP_ZS         PARA_ENABLE_COMP3_IRQ
#endif
#define EDDI_ISO_ENABLE_COMP_ZIT        PARA_ENABLE_COMP5_IRQ

/*===========================================================================*/
/*                              Structs                                      */
/*===========================================================================*/
#define EDDI_ISO_FATAL_ERR_STATE        1
#define EDDI_ISO_FATAL_ERR_INTERRUPT    2
#if defined (EDDI_CFG_REV5)
#define EDD_ISO_FATAL_ERR_PARAM         3
#endif

//SM trigger-events
typedef enum _EDDI_ISO_TRIGGER_EVENTS_TYPE
{
    EDDI_ISO_TRIGGER_START = 1,
    EDDI_ISO_TRIGGER_NEWCYCLE,
    EDDI_ISO_TRIGGER_TAINT,
    EDDI_ISO_TRIGGER_TMINT,
    EDDI_ISO_TRIGGER_FREERUN,
    EDDI_ISO_TRIGGER_RESTART,
    EDDI_ISO_TRIGGER_STOP
} EDDI_ISO_TRIGGER_EVENTS_TYPE;

//States
typedef enum _EDDI_ISO_STATES_TYPE
{
    EDDI_ISO_STATE_INIT = 1,
    EDDI_ISO_STATE_PREPARE,
    EDDI_ISO_STATE_CHECKPHASE_START,
    EDDI_ISO_STATE_CHECKPHASE_WAIT,
    EDDI_ISO_STATE_CHECKPHASE_CHECK,
    EDDI_ISO_STATE_COUPLED,
    EDDI_ISO_STATE_FREERUN,
    EDDI_ISO_STATE_STOPPED
} EDDI_ISO_STATES_TYPE;

typedef struct _EDDI_ISO_CB_ERROR_TYPE
{
    LSA_UINT32  StartErrorCtr;
    LSA_UINT32  Param1;
    LSA_UINT32  Param2;
    LSA_UINT32  Param3;
    LSA_UINT32  Param4;
} EDDI_ISO_CB_ERROR_TYPE;

//internal trace
#if defined (EDDI_ISO_CFG_TRACE_DEPTH)
typedef enum _EDDI_ISO_TRACE_EVENT_TYPE
{
    EDDI_ISO_TRACE_STATE_CHANGE = 1,
    EDDI_ISO_TRACE_EXEC_PREPARE,
    EDDI_ISO_TRACE_SM,
    EDDI_ISO_TRACE_RESTART_CMD,
    EDDI_ISO_TRACE_CHECKPHASE_WAIT,
    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    EDDI_ISO_TRACE_CALLCBF,
    #endif
    //EDDI_ISO_TRACE_CHECKTRACKS,
    EDDI_ISO_TRACE_INT
} EDDI_ISO_TRACE_EVENT_TYPE;

typedef struct _EDDI_ISO_TRACE_ELEMENT_TYPE
{
    LSA_UINT32                    time;
    EDDI_ISO_TRACE_EVENT_TYPE     trace_event;
    EDDI_ISO_STATES_TYPE          state;
    LSA_UINT32                    detail1;
    LSA_UINT32                    detail2;
} EDDI_ISO_TRACE_ELEMENT_TYPE;

typedef struct _EDDI_ISO_TRACE_TYPE
{
    LSA_UINT16                    next;
    EDDI_ISO_TRACE_ELEMENT_TYPE   element[EDDI_ISO_CFG_TRACE_DEPTH];
} EDDI_ISO_TRACE_TYPE;
#endif //EDDI_ISO_CFG_TRACE_DEPTH

#if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
typedef struct _EDDI_ISO_STATISTICS_ELEMENT_TYPE
{
    LSA_UINT32      MaxTime_ns;
    LSA_UINT32      MinTime_ns;
    LSA_UINT32      CurTime_ns;
} EDDI_ISO_STATISTICS_ELEMENT_TYPE;

typedef struct _EDDI_ISO_STATISTICS_TYPE
{
    EDDI_ISO_STATISTICS_ELEMENT_TYPE  Zm;
    #if !(defined (EDDI_ISO_CFG_NO_ZE_INT) && defined (EDDI_ISO_CFG_NO_ZS_INT))
    EDDI_ISO_STATISTICS_ELEMENT_TYPE  Za;
    #endif
} EDDI_ISO_STATISTICS_TYPE;
#endif //EDDI_ISO_CFG_ENABLE_INT_STATISTICS

typedef struct _EDDI_ISO_CB_INSTANCE_TYPE
{
    EDDI_ISO_STATES_TYPE            State;          //State
    EDDI_ISO_STATES_TYPE            NextCoupledState; //Freerun or Coupled
    LSA_BOOL                        bIFSync;
    LSA_UINT16                      Instance;       //Instance-Nr. (Handle)
    LSA_BOOL                        bCBFLocked;     //FALSE: CBF is allowed to be called
    LSA_UINT32                    * pIntReqReg;     //Ptr to Interrupt Request Register (contains all ints)
    LSA_UINT32                    * pIntMaskReg;    //Ptr to Interrupt Mask Register
    LSA_UINT32                    * pIntReg;        //Ptr to Interrupt Register (contains only unmasked ints)
    LSA_UINT32                    * pIntAckReg;     //Ptr to Interrupt Acknowledge Register
    EDDI_HANDLE                     hDDB;           //Lower Handle of EDDI (only valid in SHARED_MODE!)
    LSA_UINT32                      StartupErrorCtr; //allowable startup errors in state CHECKPHASE_WAIT until resynchronization is started
    #if defined (EDDI_CFG_REV5)
    LSA_UINT32                    * pCycleCtrHigh;  //Ptr to location in KRAM where EDDI copies the CycleCtr47..32 built in SW
    LSA_UINT32                      OldCycleCtrHigh;
    LSA_UINT32                      OldCycleCtrLow;
    LSA_BOOL                        bInconsistent;
    #endif
    
    //Times based on the application cycle:
    struct
    {
        LSA_UINT32                  ClkCyPeriod_10ns;     //calculated Clockcycle-period in ticks (10ns), 16bit only!
        LSA_UINT32                  ApplRedRat;     //Application reduction ratio (CACF*ReductionRatio) for applicationtimer
        LSA_UINT32                  RedRatInt;      //Internal calculated ReductionRatio:
        //(StartParam.ReductionRatio*CycleLen_ns)=(RedRatInt*ClkCyPeriod_Ticks*10)
        LSA_UINT32                  ApplCyclInBaseCycles; //Application cycle in entities of 31,25us
    } ApplCycle;
    //Times based on the network cycle:
    struct
    {
        LSA_UINT32                  CycleLen_ns;          //Len of cycle in ns
        LSA_UINT32                  CycleBaseFactor;//CycleBaseFactor * 31,25us = CycleLen_ns;
        LSA_UINT16                  CycleBaseFactorCACF;  //CACF*RedRat*CycleBaseFactor
        LSA_UINT64                  LastCBFCycleCtr;//CycleCtr of 1st applphase at CBF-start
        LSA_UINT64                  CheckSFCCycleCtr;     //CycleCtr of 1st applphase when bStoreCycle is selected at CHECKTRACKS
        #if defined (EDDI_ISO_CFG_NO_ZE_INT)
        LSA_UINT16                  CycleCtrMin_Ze;  //CycleCtr-Value at Ze-Phase
        LSA_UINT32                  PhaseOffset_Ze_10ns; //Offset in Ze-Phase to Ze-value in 10ns
        #endif
        #if defined (EDDI_ISO_CFG_NO_ZS_INT)
        #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
        LSA_UINT16                  CycleCtrMin_Zsi;    //CycleCtr-Value at Zsi-Phase
        LSA_UINT32                  PhaseOffset_Zsi_10ns;//Offset in Zsi-Phase to Zsi-value in 10ns
        LSA_UINT16                  CycleCtrMin_Zso;    //CycleCtr-Value at Zso-Phase
        LSA_UINT32                  PhaseOffset_Zso_10ns;//Offset in Zso-Phase to Zso-value in 10ns        
        #else
        LSA_UINT16                  CycleCtrMin_Zs;       //CycleCtr-Value at Zs-Phase
        LSA_UINT32                  PhaseOffset_Zs_10ns;  //Offset in Zs-Phase to Zs-value in 10ns
        #endif
    #endif
    } NetworkCycle;

    EDD_APPL_SYNC_START_PARAM_TYPE  StartParam;
    EDDI_ISO_CB_ERROR_TYPE          Error;
    LSA_RESULT LSA_FCT_PTR(EDDI_SYSTEM_IN_FCT_ATTR, pIntHandlerFct) (LSA_VOID * const pInstance, LSA_UINT32 const Int); //Pointer to the currently valid inthandler-fct
    #if defined (EDDI_ISO_CFG_TRACE_DEPTH)
    LSA_UINT32                    * pClkCntVal;     //Ptr to ClockCountVal register
    EDDI_ISO_TRACE_TYPE             Trace;
    #endif
    #if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
    EDDI_ISO_STATISTICS_TYPE        Stat;
    #endif
} EDDI_ISO_CB_INSTANCE_TYPE;

typedef struct _EDDI_ISO_CB_INSTANCE_TYPE  * EDDI_ISO_CB_INSTANCE_PTR_TYPE;

typedef struct _EDDI_ISO_CB_TYPE
{
    LSA_UINT8                     EDD_Lock_Sema_ISO;
    EDDI_ISO_CB_INSTANCE_TYPE   Instance[EDDI_ISO_CFG_MAX_NR_INSTANCES+1];  //0 is not used
} EDDI_ISO_CB_TYPE;

/*===========================================================================*/
/*                              Macros                                       */
/*===========================================================================*/
#if defined (EDDI_ISO_CFG_LITTLE_ENDIAN)
#define EDDI_ISO_SWAP_32(var)     (var)
#else
#define EDDI_ISO_SWAP_32(var)                          \
  /*lint --e(941) --e(572) --e(778)  */                     \
(  ((((var)) & 0xFF000000UL) >> 24)    \
+ ((((var)) & 0x00FF0000UL) >>  8)    \
+ ((((var)) & 0x0000FF00UL) <<  8)    \
+ ((((var)) & 0x000000FFUL) << 24))
#endif

#define EDDI_ISO_R32(pBase_, offset_, val_)  {(val_) = EDDI_ISO_SWAP_32(*(volatile LSA_UINT32 *)((LSA_UINT32)(pBase_) + (offset_)));}

#define EDDI_ISO_W32(pBase_, offset_, val_)  {*(LSA_UINT32 *)((LSA_UINT32)(pBase_) + (offset_)) = EDDI_ISO_SWAP_32(val_);}

#define EDDI_ISO_FREE_INSTANCE(pInstance_)  {                       \
    (pInstance_)->State          = EDDI_ISO_STATE_INIT;             \
    (pInstance_)->Instance       = EDDI_ISO_INSTANCE_NOT_USED;      \
    (pInstance_)->pIntHandlerFct = EDDI_IsoInthandlerDummy;         \
}


#define EDDI_ENTER_APPLSYNC_S()                                                             \
EDDI_ENTER_APPLSYNC();                                                                      \
if ( !eddi_iso_cb.EDD_Lock_Sema_ISO )                                                       \
{                                                                                           \
eddi_iso_cb.EDD_Lock_Sema_ISO++;                                                            \
}                                                                                           \
else                                                                                        \
{                                                                                           \
/* no trace entries in macros possible (tracescanner) */                                    \
EDDI_Excp("EDD_ENTER_APPLSYNC -> ", EDDI_FATAL_ERR_EXCP, eddi_iso_cb.EDD_Lock_Sema_ISO, 0); \
}

#define EDDI_EXIT_APPLSYNC_S()                                                              \
eddi_iso_cb.EDD_Lock_Sema_ISO--;                                                            \
if ( 0 == eddi_iso_cb.EDD_Lock_Sema_ISO )                                                   \
{                                                                                           \
}                                                                                           \
else                                                                                        \
{                                                                                           \
/* no trace entries in macros possible (tracescanner) */                                    \
EDDI_Excp("EDD_EXIT_APPLSYNC -> ", EDDI_FATAL_ERR_EXCP, eddi_iso_cb.EDD_Lock_Sema_ISO, 0);  \
}                                                                                           \
EDDI_EXIT_APPLSYNC();

#if defined (EDDI_ISO_CFG_TRACE_DEPTH)
#define EDDI_ISO_TRACE(pInstance_, trace_event_, detail1_, detail2_) {                              \
EDDI_ISO_TRACE_ELEMENT_TYPE * const pTrace = &(pInstance->Trace.element[pInstance->Trace.next]);    \
pTrace->time          = *pInstance->pClkCntVal;                                                     \
pTrace->trace_event   = (trace_event_);                                                             \
pTrace->state         = pInstance->State;                                                           \
pTrace->detail1       = (detail1_);                                                                 \
pTrace->detail2       = (detail2_);                                                                 \
pInstance->Trace.next++;                                                                            \
if (pInstance->Trace.next >= EDDI_ISO_CFG_TRACE_DEPTH) {pInstance->Trace.next = 0;}                 \
}

#else
#define EDDI_ISO_TRACE(pInstance_, trace_event_, detail1_, detail2_)

#endif

#define EDDI_ISO_GET_APPLTIME_OFFSET(pInstance_, CycleCtr64_, ApplTimeOffset_ns_) {                                                             \
    LSA_UINT32  CycleTimeVal_10ns_;                                                                                                             \
    (CycleCtr64_) = EDDI_IsoGetCycleCtr64(pInstance);                                                                                           \
    EDDI_ISO_R32((pInstance_)->StartParam.pIRTE_PNIP, CYCL_TIME_VALUE, CycleTimeVal_10ns_);                                                     \
    (ApplTimeOffset_ns_) = (((LSA_UINT16)((CycleCtr64_) % pInstance_->NetworkCycle.CycleBaseFactorCACF)*3125UL) + CycleTimeVal_10ns_) * 10UL;   \
    }

#endif //EDDI_INTCFG_ISO_SUPPORT_ON

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IO_ISO_H


/*****************************************************************************/
/*  end of file eddi_io_iso.h                                                */
/*****************************************************************************/

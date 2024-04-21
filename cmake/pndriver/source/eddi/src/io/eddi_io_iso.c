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
/*  F i l e               &F: eddi_io_iso.c                             :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Functions for isochronous support                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*                                                  27.01.09    UL    created*/
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_io_iso.h"

#define EDDI_MODULE_ID     M_ID_IO_ISO
#define LTRC_ACT_MODUL_ID  502

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_IO_ISO) //satisfy lint!
#endif

#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*===========================================================================*/
/*                              Local Protos                                 */
/*===========================================================================*/
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInthandlerDummy (LSA_VOID * const pInstance, LSA_UINT32 const Int);
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInthandlerWork  (LSA_VOID * const pInstance, LSA_UINT32 const Int);
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInthandlerStart (LSA_VOID * const pInstance, LSA_UINT32 const Int);
static LSA_UINT32 EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoCorrectCompVal  (LSA_UINT32 const compVal);
/*===========================================================================*/
/*                              Local Macros                                 */
/*===========================================================================*/

/*===========================================================================*/
/*                              Local Vars                                   */
/*===========================================================================*/
EDDI_ISO_CB_TYPE  eddi_iso_cb;
#endif //defined (EDDI_INTCFG_ISO_SUPPORT_ON)


#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IsoInitDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE  const  pCyclComp )
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(pCyclComp);
    return;
}
#else //defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*=============================================================================
 * function name: EDDI_IsoInitDefaultApplclock()
 *
 * function:      Initialization of the applclk-unit for MC-usage or/and XPLLOUT-signal generation
 *
 * parameters:    pDDB      DDB
 *                pCyclComp Ptr to the RQB params
 *
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IsoInitDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE             const  pDDB,
                                                             EDDI_UPPER_CYCLE_COMP_INI_PTR_TYPE  const  pCyclComp )
{
    LSA_UINT32  Value;

    //using the applclk-unit for other purposes than isochronous support is only allowed if iso support is not used or off!
    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
    //check PDControlPllMode
    if (pCyclComp->PDControlPllMode != EDDI_CYCLE_INI_PLL_MODE_NOTUSED)
    {
        IO_W32(CLK_CYC_PERIOD,    3125UL);
        IO_W32(DIV_CYC_CLK_INT,   0);
        IO_W32(CYC_CLK_INT_DELAY, 0);
    }
    else //PDControlPllMode == EDDI_CYCLE_INI_PLL_MODE_NOTUSED
    {
        EDDI_Excp("EDDI_IsoInitDefaultApplclock, SII-NEW-CYCLE-SYNC_MODE, PDControlPllMode == EDDI_CYCLE_INI_PLL_MODE_NOTUSED!", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }
    #else
    IO_W32(CLK_CYC_PERIOD,    pCyclComp->ClkCy_Period);
    IO_W32(DIV_CYC_CLK_INT,   pCyclComp->ClkCy_Int_Divider);
    IO_W32(CYC_CLK_INT_DELAY, pCyclComp->ClkCy_Int_Delay);
    #endif
    IO_W32(DIV_APP_CLK,       pCyclComp->AplClk_Divider);
    IO_W32(CYC_CLK_DELAY,     pCyclComp->ClkCy_Delay);

    //set COMPx-Value -> Default nach Reset 0
    //IO_W32(MUX_SELECTOR, 0);

    //=== start AppClk and Clk_Cyc
    IO_W32(CLK_COMMAND, (START_CYCL_CLK | START_APP_CLK));

    //==== Set Masks for activation of Comparator-Interrupts
    Value = IO_R32(IRT_CYCL_ID);

    if (pCyclComp->CompVal1)
    {
        IO_W32(CLK_COMP_VALUE1,  pCyclComp->CompVal1);
        Value |=  PARA_ENABLE_COMP1_IRQ;
    }
    else
    {
        Value &= ~PARA_ENABLE_COMP1_IRQ;
    }

    if (pCyclComp->CompVal2)
    {
        IO_W32(CLK_COMP_VALUE2,  pCyclComp->CompVal2);
        Value |=  PARA_ENABLE_COMP2_IRQ;
    }
    else
    {
        Value &= ~PARA_ENABLE_COMP2_IRQ;
    }

    if (pCyclComp->CompVal3)
    {
        Value |= PARA_ENABLE_COMP3_IRQ;
        IO_W32(CLK_COMP_VALUE3,  pCyclComp->CompVal3);
    }
    else
    {
        Value &= ~PARA_ENABLE_COMP3_IRQ;
    }

    if (pCyclComp->CompVal4)
    {
        Value |= PARA_ENABLE_COMP4_IRQ;
        IO_W32(CLK_COMP_VALUE4,  pCyclComp->CompVal4);
    }
    else
    {
        Value &= ~PARA_ENABLE_COMP4_IRQ;
    }

    if (pCyclComp->CompVal5)
    {
        Value |= PARA_ENABLE_COMP5_IRQ;
        IO_W32(CLK_COMP_VALUE5,  pCyclComp->CompVal5);
    }
    else
    {
        Value &= ~PARA_ENABLE_COMP5_IRQ;
    }
        
    IO_W32(IRT_CYCL_ID, Value);
}
#endif //defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
 LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_IsoCtrlDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               EDDI_CONFIG_APPLCLK_CMD  const  Cmd,
                                                               LSA_UINT32               const  AplClk_Divider,
                                                               LSA_UINT32               const  ClkCy_Period )
{
    LSA_UNUSED_ARG(pDDB);
    LSA_UNUSED_ARG(Cmd);
    LSA_UNUSED_ARG(AplClk_Divider);
    LSA_UNUSED_ARG(ClkCy_Period);
    return EDD_STS_OK; 
}
#else //defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*=============================================================================
 * function name: EDDI_IsoCtrlDefaultApplclock()
 *
 * function:      Controlling the applclk-unit for MC-usage or/and XPLLOUT-signal generation
 *
 * parameters:    pDDB              DDB
 *                Cmd               Command
 *                AplClk_Divider    APPLCLK_DIVIDER
 *                ClkCy_Period      APPLCLK_PERIOD
 *
 * return value:  EDD_STS_OK, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *===========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_IsoCtrlDefaultApplclock( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               EDDI_CONFIG_APPLCLK_CMD  const  Cmd,
                                                               LSA_UINT32               const  AplClk_Divider,
                                                               LSA_UINT32               const  ClkCy_Period )
{
    LSA_RESULT  result = EDD_STS_OK;
    LSA_UINT32  ClkCommandValue;

    /* using the applclk-unit for other purposes than isochronous support is only allowed if iso support is not used or off! */
    switch (Cmd)
    {
        case EDDI_ISO_DEFAULT_APPLCLK_ENABLE:
        {
            if (pDDB->PRM.PDControlPLL.PDControlPllActive)
            {
                //Reactivate ApplicationClock
                ClkCommandValue  = IO_R32(CLK_COMMAND);
                ClkCommandValue |= START_APP_CLK;
                IO_W32(CLK_COMMAND, ClkCommandValue);
            }
            break;
        }
        case EDDI_ISO_DEFAULT_APPLCLK_DISABLE:
        {
            if (pDDB->PRM.PDControlPLL.PDControlPllActive)
            {
                // === Switch off Application-Clock ===
                // Technical Background:
                // The internal Application-Clock-Counter acts as a free running counter
                // if the ENABLE_MULTICYCLE_APL_CLOCKS-Bit is set.
                // To avoid a phase-difference between CycleTimeCounter and ApplicationClockCounter
                // during CycleLengthExpansion, we need to reset the ApplicationClockCounter during CycleLength-Expansion.
                ClkCommandValue = IO_R32(CLK_COMMAND);
                if (ClkCommandValue & ENABLE_MULTICYCLE_APL_CLOCKS)
                {
                    result = EDD_STS_ERR_SEQUENCE;
                }
                else
                {
                    ClkCommandValue &= ~START_APP_CLK;
                    IO_W32(CLK_COMMAND, ClkCommandValue);
                    IO_W32(DIV_APP_CLK, AplClk_Divider);
                    #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
                    if //HW-divider necessary?
                        (pDDB->SII.HWClkCycleIntDividerForNRTCheck)
                    {
                        IO_W32(DIV_CYC_CLK_INT, pDDB->SII.HWClkCycleIntDividerForNRTCheck);
                    }
                    #else
                    if (   (AplClk_Divider <= 2)  
                        && (3125 == ClkCy_Period) )
                    {
                        //special condition: 31,25µs or 62,50µs cycle
                        IO_W32(DIV_CYC_CLK_INT, AplClk_Divider);                    
                    }
                    #endif
                }
            }
            break;
        }
        case EDDI_ISO_DEFAULT_APPLCLK_SET:
        {
            IO_W32(DIV_APP_CLK,    AplClk_Divider);
            IO_W32(CLK_CYC_PERIOD, ClkCy_Period);
            #if defined (EDDI_CFG_SII_NEW_CYCLE_SYNC_MODE)
            if //HW-divider necessary?
                (pDDB->SII.HWClkCycleIntDividerForNRTCheck)
            {
                IO_W32(DIV_CYC_CLK_INT, pDDB->SII.HWClkCycleIntDividerForNRTCheck);
            }
            #endif
            break;
        }
        case EDDI_ISO_DEFAULT_APPLCLK_START:
        {
            ClkCommandValue = IO_R32(CLK_COMMAND);
            ClkCommandValue |= START_APP_CLK;
            IO_W32(CLK_COMMAND, ClkCommandValue);
            break;
        }
        case EDDI_ISO_DEFAULT_APPLCLK_STOP:
        {
            ClkCommandValue = IO_R32(CLK_COMMAND);
            ClkCommandValue &= ~START_APP_CLK;
            IO_W32(CLK_COMMAND, ClkCommandValue);
            break;
        }
        default:
        {
            result = EDD_STS_ERR_PARAM;
        }
    }

    return result;
}
#endif //defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*---------------------- end [subroutine] ---------------------------------*/



#if defined (EDDI_INTCFG_ISO_SUPPORT_ON)
/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/
static LSA_UINT64 EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoGetCycleCtr64(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UINT32  CycleCtrLow;
    LSA_UINT32  CycleCtrHigh;
    LSA_UINT64  CycleCtr64;

    #if defined (EDDI_CFG_REV5)
    //CycleCtrHigh contains b16...b47 built by SW and passed to IO_ISO via KRAM
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtrLow); //16bit only!
    CycleCtrHigh = *pInstance->pCycleCtrHigh;
    if /* inconsistency had been detected before */
       (pInstance->bInconsistent)
    {
        if /* high part still did not change? */
           (CycleCtrHigh == pInstance->OldCycleCtrHigh)
        {
            CycleCtrHigh++;
        }
        else
        {
            //EDDI finally incremented the high part
            pInstance->bInconsistent    = LSA_FALSE;
            pInstance->OldCycleCtrHigh  = CycleCtrHigh;
        }
    }
    else
    {
        if /* low part has experienced a wraparound and high part did not change? */
           (   (CycleCtrLow < pInstance->OldCycleCtrLow)
            && (CycleCtrHigh == pInstance->OldCycleCtrHigh) )
        {
            //EDDI did not increment CycleCtrHigh yet
            CycleCtrHigh++;
            pInstance->bInconsistent = LSA_TRUE;
        }
        else
        {
            pInstance->OldCycleCtrHigh = CycleCtrHigh;
        }
    }
    pInstance->OldCycleCtrLow = CycleCtrLow;
    CycleCtr64 = ((LSA_UINT64)CycleCtrLow) + (((LSA_UINT64)CycleCtrHigh)<<16); 
    #else
    LSA_UINT32 Tmp;
    LSA_UINT8  Ctr=3;

    do
    {
        //CycleCtrHigh can be read from a register
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE_47_32, CycleCtrHigh);
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtrLow);
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE_47_32, Tmp);
        if /* CycleCtrHigh got incremented in the meantime */
           (Tmp != CycleCtrHigh)
        {
            Ctr--;
        }
        else
        {
            break;
        }
    }
    while (Ctr);

    CycleCtr64 = ((LSA_UINT64)CycleCtrLow) + (((LSA_UINT64)CycleCtrHigh)<<32); 
    #endif

    return CycleCtr64;
}
/*---------------------- end [subroutine] ---------------------------------*/

static void EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInitCycleCtr64(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    //initialize OldCycleCtrHigh/OldCycleCtrLow
    #if defined (EDDI_CFG_REV5)
    //CycleCtrHigh contains b16...b47 built by SW and passed to IO_ISO via KRAM
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, pInstance->OldCycleCtrLow); //16bit only!
    pInstance->OldCycleCtrHigh = *pInstance->pCycleCtrHigh;
    pInstance->bInconsistent = LSA_FALSE;
    #else
    LSA_UNUSED_ARG(pInstance);
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/

#if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInitStatistics(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    #if !(defined (EDDI_ISO_CFG_NO_ZE_INT) && defined (EDDI_ISO_CFG_NO_ZS_INT))
    pInstance->Stat.Za.MaxTime_ns = 0;
    pInstance->Stat.Za.MinTime_ns = 0xFFFFFFFFUL;
    #endif
    
    pInstance->Stat.Zm.MaxTime_ns = 0;
    pInstance->Stat.Zm.MinTime_ns = 0xFFFFFFFFUL;
}
/*---------------------- end [subroutine] ---------------------------------*/
#else //defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInitStatistics(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UNUSED_ARG(pInstance); //satisfy lint!
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)

//*****************************************************************************
//mask/unmask interrupts
// pIntMaskReg:  Ptr to the mask-register
// Value:        Bits that shall be masked/unmasked
// bEnable:      TRUE:  Mask (=disable) all 1-bits in Value,
//               FALSE: Unmask (=enable) all 1-bits in Value
//*****************************************************************************
#if !defined (EDDI_CFG_APPLSYNC_SHARED) 
static  LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoMaskInts( EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance, 
                                                             LSA_UINT32                     const  Value, 
                                                             LSA_BOOL                       const  bEnable )
{
    LSA_UINT32  reg;

    EDDI_ISO_R32((LSA_UINT32)pInstance->pIntMaskReg, 0, reg);
    if (bEnable)
    {
        reg |= Value;
    }
    else
    {
        reg &= ~Value;
    }
    EDDI_ISO_W32((LSA_UINT32)pInstance->pIntMaskReg, 0, reg);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Acknowledge interrupts
// pIntAckReg:   Ptr to the acknowledge-register
// Value:        Bits that shall be acknowledged
//*****************************************************************************
#if !defined (EDDI_CFG_APPLSYNC_SHARED)
static  LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoAckInts( EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance,
                                                            LSA_UINT32                     const  Value )
{
    EDDI_ISO_W32((LSA_UINT32)pInstance->pIntAckReg, 0, Value);
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//mask/unmask interrupts
// pIntMaskReg:  Ptr to the mask-register
// Value:        Bits that shall be masked/unmasked
// Enable:       TRUE:  Mask (=disable) all 1-bits in Value,
//               FALSE: Unmask (=enable) all 1-bits in Value
//*****************************************************************************
//temporarily_disabled_lint -esym(528, EDDI_IsoMaskIntsSII)
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoMaskIntsSII(         EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance,
                                                                LSA_UINT32                     const  Value,
                                                                LSA_BOOL                       const  bEnable )
{
    LSA_RESULT  Result;

    if (bEnable)
    {
        Result = eddi_interrupt_unmask_user_int(pInstance->hDDB, Value /*IRTIrqIRTE*/, 0 /*NRTIrqIRTE*/);
    }
    else
    {
        Result = eddi_interrupt_mask_user_int(pInstance->hDDB, Value /*IRTIrqIRTE*/, 0 /*NRTIrqIRTE*/);
    }
    
    if (EDD_STS_OK != Result)
    {
        EDDI_ISO_Excp("EDDI_IsoMaskIntsSII", EDDI_FATAL_ERR_APPLSYNC, EDDI_ISO_FATAL_ERR_INTERRUPT, Value);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Acknowledge interrupts
// pIntAckReg:   Ptr to the acknowledge-register
// Value:        Bits that shall be acknowledged
//*****************************************************************************
//temporarily_disabled_lint -esym(528, EDDI_IsoAckIntsSII)
LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoAckIntsSII(         EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance,
                                                               LSA_UINT32                     const  Value )
{
    if (EDD_STS_OK != eddi_interrupt_ack_user_int(pInstance->hDDB, Value /*IRTIrqIRTE*/, 0 /*NRTIrqIRTE*/))
    {
        EDDI_ISO_Excp("EDDI_IsoAckIntsSII", EDDI_FATAL_ERR_APPLSYNC, EDDI_ISO_FATAL_ERR_INTERRUPT, Value);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//check the handle
//*****************************************************************************
static  LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoCheckHandle( LSA_UINT16                        const  Handle,
                                                                  EDDI_ISO_CB_INSTANCE_PTR_TYPE  *  const  ppInstance )
{
    if (   (Handle > EDDI_ISO_CFG_MAX_NR_INSTANCES)
        || (0 == Handle))
    {
        return EDD_STS_ERR_PARAM;
    }

    *ppInstance = &eddi_iso_cb.Instance[Handle];

    if (eddi_iso_cb.Instance[Handle].Instance != Handle)
    {
        return EDD_STS_ERR_PARAM;
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//initialize interrupts
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInitInts(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    //Mask all iso-ints
    EDDI_APPLSYNC_MASK_INTS(pInstance, (EDDI_ISO_INT_ZA + EDDI_ISO_INT_ZM + EDDI_ISO_INT_ZE + EDDI_ISO_INT_ZS + EDDI_ISO_INT_ZIT), LSA_FALSE /*!Enable*/);

    //Acknowledge all iso-ints
    EDDI_APPLSYNC_CLEAR_INTS(pInstance, (EDDI_ISO_INT_ZA + EDDI_ISO_INT_ZM + EDDI_ISO_INT_ZE + EDDI_ISO_INT_ZS + EDDI_ISO_INT_ZIT));
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//enter state PREPARE
//initialize interrupts and appl.timer-registers
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoEnterPREPARE(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UINT32  reg,TmpCompValue;

    pInstance->State      = EDDI_ISO_STATE_PREPARE;
    pInstance->bCBFLocked = LSA_TRUE;
    
    if (pInstance->bIFSync)
    {
        pInstance->NextCoupledState = EDDI_ISO_STATE_COUPLED; //assume that we are synchronized
    }
    else
    {
        pInstance->NextCoupledState = EDDI_ISO_STATE_FREERUN;   //we are not synchronized ==> running free
    }

    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0 /*trigger*/, 0 /*detail*/);

    //initialize interrupts
    EDDI_IsoInitInts(pInstance);
    pInstance->pIntHandlerFct = EDDI_IsoInthandlerStart;

    //initialize the ApplicationTimer
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CLK_COMMAND, 0);  //stop generation of aplclk/clkcyint
    //wait until timers are stopped (normally 1 cycle)
    do
    {
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CLK_STATUS, reg);
    }
    while (reg);

    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CLK_CYC_PERIOD,  pInstance->ApplCycle.ClkCyPeriod_10ns);                 //basic period
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, DIV_APP_CLK,  pInstance->ApplCycle.ApplRedRat);                          //applclk-divider
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, DIV_CYC_CLK_INT, pInstance->ApplCycle.ApplRedRat);                       //clkcyint-divider
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CYC_CLK_INT_DELAY, 0);                                                   //Delay 1
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CYC_CLK_DELAY, 0);                                                       //Delay 2
    TmpCompValue=EDDI_IsoCorrectCompVal(pInstance->StartParam.Tm_ns/EDDI_ISO_NS_PER_TICK);     //Tm
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZM, TmpCompValue);
    TmpCompValue=EDDI_IsoCorrectCompVal(pInstance->StartParam.Te_ns/EDDI_ISO_NS_PER_TICK);     //Te
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZE, TmpCompValue);
    #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
    TmpCompValue=EDDI_IsoCorrectCompVal( ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.SeparateTracks.Tki_ns)/EDDI_ISO_NS_PER_TICK);  //Tsi
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZSI,TmpCompValue);
    TmpCompValue=EDDI_IsoCorrectCompVal(((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.SeparateTracks.Tko_ns)/EDDI_ISO_NS_PER_TICK);   //Tso
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZSO,TmpCompValue);
    #else
    TmpCompValue=EDDI_IsoCorrectCompVal(((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.Tk_ns)/EDDI_ISO_NS_PER_TICK);//Ts
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZS,TmpCompValue);
    #endif
    TmpCompValue=EDDI_IsoCorrectCompVal(((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio)/EDDI_ISO_NS_PER_TICK)- 10); //Tit-100ns
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, EDDI_ISO_COMP_ZIT, TmpCompValue);

    //enable COMP-signals
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, IRT_CYCL_ID, reg);
    reg |= (EDDI_ISO_ENABLE_COMP_ZM | EDDI_ISO_ENABLE_COMP_ZE | EDDI_ISO_ENABLE_COMP_ZS |EDDI_ISO_ENABLE_COMP_ZIT);
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, IRT_CYCL_ID, reg);

    //clear and unmask Tm- and Ta-Int (needed for SM)
    EDDI_APPLSYNC_CLEAR_INTS(pInstance, (EDDI_ISO_INT_ZM + EDDI_ISO_INT_ZA));
    EDDI_APPLSYNC_MASK_INTS(pInstance, (EDDI_ISO_INT_ZM + EDDI_ISO_INT_ZA), LSA_TRUE /*Enable*/);
    
    #if defined (EDDI_CFG_REV5)
    //get the KRAM-location for the CycleCtrHigh
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, NRT_CTRL_BASE_ADR, reg);
    
    if (!reg)  
    {
        EDDI_ISO_Excp("EDDI_IsoEnterPREPARE", EDDI_FATAL_ERR_APPLSYNC, EDD_ISO_FATAL_ERR_PARAM, pInstance->Instance);
    }
    else
    {
        pInstance->pCycleCtrHigh = &((EDDI_SER_NRT_HEAD_PTR_TYPE)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + reg))->Arg.CycleCtrHigh;
    }
    #endif
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_IsoCorrectCompVal()
 *
 * function:      Returns a valid value for the comparator value.
                  0 and 2...(2^32 -1) are valid
                  1 is not valid and instead 2 should be returned, for details see TFS-RQ:1092739 
 *
 * parameters:
 *                compVal: comparatpr value
                           possible values:0..(2^32 -1)

 *
 * return value:  LSA_UINT32 possible value 0 and 2...65535
 *===========================================================================*/
static LSA_UINT32 EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoCorrectCompVal  (LSA_UINT32 const CompVal)
{
    LSA_UINT32 ReturnValue = CompVal;

    if(1 == CompVal)
    {
        ReturnValue=2;
    }

    return ReturnValue;
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//execute state PREPARE
//wait for the correct phase
//*****************************************************************************
static  LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoExecPREPARE( EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance )
{
    LSA_UINT64  CycleCtr64;

    //wait for the last network-cycle of the application-cycle to appear:
    //( (CycleCtr + NetworkCycle.CycleBaseFactor) mod (NetworkCycle.CycleBaseFactor * ReductionRatio * CACF) ) == 0
    //EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtr);
    EDDI_IsoInitCycleCtr64(pInstance);
    CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);
    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_EXEC_PREPARE, 0 /*trigger*/, (LSA_UINT32)CycleCtr64 /*detail*/);
    if (0 == ((CycleCtr64 + pInstance->NetworkCycle.CycleBaseFactor) % pInstance->ApplCycle.ApplCyclInBaseCycles))
    {
        //Start ApplTimer without Multicycles
        EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CLK_COMMAND, START_CYCL_CLK + START_APP_CLK);

        pInstance->State = EDDI_ISO_STATE_CHECKPHASE_START;
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0 /*trigger*/, 0 /*detail*/);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//enter state CHECKPHASE_WAIT
//set multicycle-bit
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoEnterCHECKPHASE_WAIT(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UINT32  reg;

    pInstance->State = EDDI_ISO_STATE_CHECKPHASE_WAIT;
    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0 /*trigger*/, 0 /*detail*/);

    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CLK_COMMAND, reg);
    reg |= ENABLE_MULTICYCLE_APL_CLOCKS;
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CLK_COMMAND, reg);
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//enter state CHECKPHASE_CHECK
//final check if the phase is correct (this function is called in context
//of TA-INT)
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoEnterCHECKPHASE_CHECK(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UINT64 CycleCtr64;

    //check the correctness of the phase:
    //nominal phase = 0 (TA-Int!)
    //( CycleCtr - (NetworkCycle.CycleBaseFactor*nominal phase) mod (NetworkCycle.CycleBaseFactor * ReductionRatio * CACF) ) == 0
    //EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtr);
    CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);

    if (0 == (CycleCtr64 % pInstance->ApplCycle.ApplCyclInBaseCycles))
    {
        //ok! just wait for the next Tm-Int
        pInstance->State = EDDI_ISO_STATE_CHECKPHASE_CHECK;
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0/*trigger*/, (LSA_UINT32)CycleCtr64 /*detail*/);
        pInstance->StartupErrorCtr = 0;
    }
    else
    {
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CHECKPHASE_WAIT, pInstance->Error.StartErrorCtr /*trigger*/, pInstance->ApplCycle.ApplRedRat /*detail*/);
        if(pInstance->StartupErrorCtr > pInstance->ApplCycle.ApplRedRat)
        {
            //ERROR! Start again!
            EDDI_IsoEnterPREPARE(pInstance);
        }
        else
        {
            //inc counters
            pInstance->Error.StartErrorCtr++;
            pInstance->StartupErrorCtr++;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//execute state CHECKPHASE_CHECK
//clear ints
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoExecCHECKPHASE_CHECK(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    LSA_UINT64  CycleCtr64;

    //Acknowledge (=clear) all iso-ints
    EDDI_APPLSYNC_CLEAR_INTS(pInstance, (EDDI_ISO_INT_ZA + EDDI_ISO_INT_ZM + EDDI_ISO_INT_ZE + EDDI_ISO_INT_ZS + EDDI_ISO_INT_ZIT));

    //Proceed to state COUPLED
    pInstance->State          = pInstance->NextCoupledState;
    pInstance->pIntHandlerFct = EDDI_IsoInthandlerWork;
    pInstance->bCBFLocked     = LSA_FALSE;

    //EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtr);
    CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);
    pInstance->NetworkCycle.LastCBFCycleCtr = (CycleCtr64 / pInstance->NetworkCycle.CycleBaseFactorCACF) * pInstance->NetworkCycle.CycleBaseFactorCACF; //temporarily_disabled_lint !e734

    //init statistics
    EDDI_IsoInitStatistics(pInstance);

    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0 /*trigger*/, 0 /*detail*/);
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//enter state STOPPED
//clear and mask all ints
//*****************************************************************************
static LSA_VOID EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoEnterSTOPPED(EDDI_ISO_CB_INSTANCE_PTR_TYPE const pInstance)
{
    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    LSA_UINT32  reg;
    #endif

    pInstance->State = EDDI_ISO_STATE_STOPPED;

    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, 0 /*trigger*/, 0 /*detail*/);

    pInstance->bCBFLocked = LSA_TRUE;

    //stop generation of aplclk/clkcyint
    EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, CLK_COMMAND, 0);

    //disable COMP1 ... COMP4
    //  EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, IRT_CYCL_ID, reg);
    //  reg &= ~(EDDI_ISO_ENABLE_COMP_ZM | EDDI_ISO_ENABLE_COMP_ZE | EDDI_ISO_ENABLE_COMP_ZS |EDDI_ISO_ENABLE_COMP_ZIT);
    //  EDDI_ISO_W32(pInstance->StartParam.pIRTE_PNIP, IRT_CYCL_ID, reg);

    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    //clear interrupts
    EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_INT, 6 /*trigger*/, reg /*detail*/);
    #endif

    EDDI_IsoInitInts(pInstance);

    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    //initialize interrupts
    EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_INT, 7 /*trigger*/, reg /*detail*/);
    #endif

    //free instance
    EDDI_ISO_FREE_INSTANCE(pInstance);
    //pInstance->Trace.next = 0;  //keep trace!
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Statemachine
//*****************************************************************************
static  LSA_VOID  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoSm( EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance,
                                                       EDDI_ISO_TRIGGER_EVENTS_TYPE   const  Trigger )
{
    EDDI_ENTER_APPLSYNC_S();

    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_SM, (LSA_UINT32)Trigger /*trigger*/, 0 /*detail*/);

    switch (pInstance->State)
    {
        /*** INIT ***/
        case EDDI_ISO_STATE_INIT:
        /*** STOPPED ***/
        case EDDI_ISO_STATE_STOPPED:
        {
            if (EDDI_ISO_TRIGGER_START == Trigger)
            {
                EDDI_IsoEnterPREPARE(pInstance); //initialize timers and interrupts
            }
            break;
        }

        /*** PREPARE ***/
        case EDDI_ISO_STATE_PREPARE:
        {
            if (EDDI_ISO_TRIGGER_NEWCYCLE == Trigger)
            {
                EDDI_IsoExecPREPARE(pInstance);
            }
            else if (EDDI_ISO_TRIGGER_FREERUN == Trigger)
            {
                // we assume that the user call this command if interface is !sync
                pInstance->bIFSync = LSA_FALSE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_FREERUN;
            }
            else if (EDDI_ISO_TRIGGER_RESTART == Trigger)
            {
                // we assume that the user call this command if interface is sync
                pInstance->bIFSync = LSA_TRUE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_COUPLED;
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        /*** CHECKPHASE_START ***/
        case EDDI_ISO_STATE_CHECKPHASE_START:
        {
            if (EDDI_ISO_TRIGGER_NEWCYCLE == Trigger)
            {
                if (pInstance->ApplCycle.ApplRedRat > 1)
                {
                    //multicycles needed, if a newcycle-event arrives, set the multicycle-bit
                    EDDI_IsoEnterCHECKPHASE_WAIT(pInstance);
                }
            }
            else if (EDDI_ISO_TRIGGER_TAINT == Trigger)
            {
                if (1 == pInstance->ApplCycle.ApplRedRat)
                {
                    //no multicycles needed, if a ta-event arrives, proceed directly to checking the phase
                    EDDI_IsoEnterCHECKPHASE_CHECK(pInstance);
                }
            }
            else if (EDDI_ISO_TRIGGER_FREERUN == Trigger)
            {
                // we assume that the user call this command if interface is !sync
                pInstance->bIFSync = LSA_FALSE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_FREERUN;
            }
            else if (EDDI_ISO_TRIGGER_RESTART == Trigger)
            {
                // we assume that the user call this command if interface is sync
                pInstance->bIFSync = LSA_TRUE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_COUPLED;
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        /*** CHECKPHASE_WAIT ***/
        case EDDI_ISO_STATE_CHECKPHASE_WAIT:
        {
            if (EDDI_ISO_TRIGGER_TAINT == Trigger)
            {
                EDDI_IsoEnterCHECKPHASE_CHECK(pInstance);
            }
            else if (EDDI_ISO_TRIGGER_FREERUN == Trigger)
            {
                // we assume that the user call this command if interface is !sync
                pInstance->bIFSync = LSA_FALSE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_FREERUN;
            }
            else if (EDDI_ISO_TRIGGER_RESTART == Trigger)
            {
                // we assume that the user call this command if interface is sync
                pInstance->bIFSync = LSA_TRUE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_COUPLED;
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        /*** CHECKPHASE_CHECK ***/
        case EDDI_ISO_STATE_CHECKPHASE_CHECK:
        {
            if  (EDDI_ISO_TRIGGER_TMINT == Trigger)
            {
                //inside this function interrupts will be cleared
                EDDI_IsoExecCHECKPHASE_CHECK(pInstance);
            }
            else if (EDDI_ISO_TRIGGER_FREERUN == Trigger)
            {
                // we assume that the user call this command if interface is !sync
                pInstance->bIFSync = LSA_FALSE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_FREERUN;
            }
            else if (EDDI_ISO_TRIGGER_RESTART == Trigger)
            {
                // we assume that the user call this command if interface is sync
                pInstance->bIFSync = LSA_TRUE;
                pInstance->NextCoupledState = EDDI_ISO_STATE_COUPLED;
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        /*** COUPLED ***/
        case EDDI_ISO_STATE_COUPLED:
        {
            if (EDDI_ISO_TRIGGER_FREERUN == Trigger)
            {
                // we assume that the user call this command if interface is !sync
                pInstance->bIFSync = LSA_FALSE;
                pInstance->State = EDDI_ISO_STATE_FREERUN;
                EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, (LSA_UINT32)Trigger /*trigger*/, 0 /*detail*/);
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        /*** FREERUN ***/
        case EDDI_ISO_STATE_FREERUN:
        {
            if (EDDI_ISO_TRIGGER_RESTART == Trigger)
            {
                // we assume that the user call this command if interface is sync
                pInstance->bIFSync = LSA_TRUE;
                //if the ClkCy_Period of the ApplTimer has a lower period than TDC, re-synchronisation has to be started
                if (pInstance->NetworkCycle.CycleBaseFactorCACF != pInstance->ApplCycle.ApplCyclInBaseCycles)
                {
                    EDDI_IsoEnterPREPARE(pInstance);
                }
                else
                {
                    pInstance->State = EDDI_ISO_STATE_COUPLED;
                    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_STATE_CHANGE, (LSA_UINT32)Trigger /*trigger*/, 0 /*detail*/);
                }
            }
            else if (EDDI_ISO_TRIGGER_STOP == Trigger)
            {
                EDDI_IsoEnterSTOPPED(pInstance);
            }
            break;
        }

        default:
        {
            EDDI_EXIT_APPLSYNC_S();
            EDDI_ISO_Excp("EDDI_IsoSm", EDDI_FATAL_ERR_APPLSYNC, EDDI_ISO_FATAL_ERR_STATE, pInstance->Instance);
        }
    }
    EDDI_EXIT_APPLSYNC_S();
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//executes the command "EDD_APPLSYNC_CMD_START"
//*****************************************************************************
static  LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_ApplSyncSTART_CMD( EDDI_HANDLE                  const  hDDB,
                                                                     EDD_APPL_SYNC_PARAM_TYPE  *  const  pParam,
                                                                     LSA_BOOL                     const  bIFSync,
                                                                     LSA_BOOL                     const  bCheck)
{
    LSA_UINT16                      Instance;
    EDDI_ISO_CB_INSTANCE_PTR_TYPE   pInstance = &eddi_iso_cb.Instance[0]; //temporarily_disabled_lint
    LSA_UINT32                      VersionNr;
    LSA_UINT32                      TempCycleLen_10ns;
    #if defined (EDDI_CFG_APPLSYNC_SHARED)
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)hDDB;
    #endif
    LSA_UINT32                      Tk_max_ns;

    //find free instance
    for (Instance=1; Instance<=EDDI_ISO_CFG_MAX_NR_INSTANCES; Instance++)
    {
        if (   (EDDI_ISO_STATE_INIT        == eddi_iso_cb.Instance[Instance].State)
            && (EDDI_ISO_INSTANCE_NOT_USED == eddi_iso_cb.Instance[Instance].Instance))
        {
            pInstance = &eddi_iso_cb.Instance[Instance];
            pInstance->Instance = Instance;
            break; //found!
        }
    }

    if (Instance > EDDI_ISO_CFG_MAX_NR_INSTANCES)
    {
        //no more free instances, or wrong command-sequence
        LSA_UNUSED_ARG(hDDB); //satisfy lint!
        return EDD_STS_ERR_SEQUENCE;
    }

    //copy parameters
    pInstance->StartParam          = pParam->Param.Start;
    pInstance->bIFSync             = bIFSync; /* store sync state => false (not sync), true (sync) */
    pInstance->pIntHandlerFct      = EDDI_IsoInthandlerDummy;
    #if defined (EDDI_CFG_APPLSYNC_SHARED)
    pParam->Param.Start.pIRTE_PNIP = (void *)pDDB->IRTE_SWI_BaseAdr;
    pInstance->hDDB                = hDDB;
    #endif

    //check pIRTE
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, SWI_VERSION,  VersionNr);
    VersionNr &= 0xFFFFFF00UL; //mask DEBUG_VERSION
    if ((VersionNr != EDDI_ISO_IRTE_VERSION_NR1) && (VersionNr != EDDI_ISO_IRTE_VERSION_NR2))
    {
        //Wrong pIRTE_PNIP?
        EDDI_ISO_FREE_INSTANCE(pInstance);
        return EDD_STS_ERR_PARAM;
    }
    else
    {
        if (!bCheck)
        {
            EDDI_ENTER_APPLSYNC_S();
            //check all other instances against double use of 1 IRTE
            for (Instance=1; Instance<=EDDI_ISO_CFG_MAX_NR_INSTANCES; Instance++)
            {
                if (   (EDDI_ISO_STATE_INIT    != eddi_iso_cb.Instance[Instance].State)
                    && (EDDI_ISO_STATE_STOPPED != eddi_iso_cb.Instance[Instance].State)
                    && (Instance != pInstance->Instance))
                {
                    if (eddi_iso_cb.Instance[Instance].StartParam.pIRTE_PNIP == pInstance->StartParam.pIRTE_PNIP)
                    {
                        EDDI_EXIT_APPLSYNC_S();
                        EDDI_ISO_FREE_INSTANCE(pInstance);
                        return EDD_STS_ERR_PARAM;
                    }
                }
            }
            EDDI_EXIT_APPLSYNC_S();
        }
    }

    //check and evaluate IntSrc
    //temporarily_disabled_lint -save -e539
    switch (pInstance->StartParam.IntSrc)
    {
        case EDDI_APPLSYNC_INTSRC_HP0:
        {
            pInstance->pIntMaskReg  = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ0_MASK_IRT);
            pInstance->pIntReg      = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ0_IRT);
            pInstance->pIntAckReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ_ACK_IRT);
            pInstance->pIntReqReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRR_IRT);
            break;
        }
        case EDDI_APPLSYNC_INTSRC_HP1:
        {
            pInstance->pIntMaskReg  = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ1_MASK_IRT);
            pInstance->pIntReg      = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ1_IRT);
            pInstance->pIntAckReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRQ_ACK_IRT);
            pInstance->pIntReqReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + HP_IRR_IRT);
            break;
        }
        case EDDI_APPLSYNC_INTSRC_SP0:
        {
            pInstance->pIntMaskReg  = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ0_MASK_IRT);
            pInstance->pIntReg      = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ0_IRT);
            pInstance->pIntAckReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ_ACK_IRT);
            pInstance->pIntReqReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRR_IRT);
            break;
        }
        case EDDI_APPLSYNC_INTSRC_SP1:
        {
            pInstance->pIntMaskReg  = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ1_MASK_IRT);
            pInstance->pIntReg      = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ1_IRT);
            pInstance->pIntAckReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRQ_ACK_IRT);
            pInstance->pIntReqReg   = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + SP_IRR_IRT);
            break;
        }
        default:
        {
            EDDI_ISO_FREE_INSTANCE(pInstance);
            return EDD_STS_ERR_PARAM;
        }
    } //switch
    //temporarily_disabled_lint -restore

    //read/calc cyclelenghts
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_CNT_ENTITY,  pInstance->NetworkCycle.CycleBaseFactor);
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_LENGTH,  TempCycleLen_10ns); //CycleLen in 10ns-Units
    pInstance->NetworkCycle.CycleLen_ns   = TempCycleLen_10ns * 10UL; //Cyclelength in ns
    pInstance->ApplCycle.ClkCyPeriod_10ns = TempCycleLen_10ns;
    pInstance->ApplCycle.RedRatInt        = pInstance->StartParam.ReductionRatio;

    //check CACF / RR
    if (   (pInstance->StartParam.CACF < 1)           || (pInstance->StartParam.CACF > EDDI_ISO_CFG_MAX_CACF)
        || (pInstance->StartParam.ReductionRatio < 1) || (pInstance->StartParam.ReductionRatio > EDDI_ISO_CFG_MAX_RR)
        || ((TempCycleLen_10ns * 10UL * pInstance->StartParam.CACF * pInstance->StartParam.ReductionRatio) > EDDI_ISO_CFG_MAX_APPL_CYCLE_TIME_NS) ) 
    {
        EDDI_ISO_FREE_INSTANCE(pInstance);
        return EDD_STS_ERR_PARAM;
    }

    //The ClkCyPeriod has to fit into a 16bit-register. Calculate it so that:
    //(InternalClkCyPeriod * InternalReductionRatio) = (GivenCycleLen * GivenReductionRatio)
    {
        LSA_UINT32  const  Period_10ns      = pInstance->ApplCycle.ClkCyPeriod_10ns;
        LSA_UINT32         PeriodRedRat     = 1UL;
        LSA_UINT32         Period_10ns_temp = Period_10ns;
    
        while (   ((Period_10ns_temp & 0xFFFF0000UL) != 0)              /* >16bit not allowed */
               || ((Period_10ns % PeriodRedRat) != 0)                   /* no reminder allowed */
               #if defined (EDDI_ISO_CFG_ALWAYS_MULTI_APPL_CYCL)
               || ((PeriodRedRat * pInstance->ApplCycle.RedRatInt) < 2) /* no divider 1 allowed */
               #endif
              )
        {
            PeriodRedRat++;
            Period_10ns_temp = Period_10ns/PeriodRedRat;
        }
        
        pInstance->ApplCycle.ClkCyPeriod_10ns = Period_10ns_temp;
        pInstance->ApplCycle.RedRatInt *= PeriodRedRat; 
    }
    
    pInstance->ApplCycle.ApplRedRat = pInstance->StartParam.CACF * pInstance->ApplCycle.RedRatInt;
    pInstance->ApplCycle.ApplCyclInBaseCycles = pInstance->NetworkCycle.CycleBaseFactor * pInstance->ApplCycle.ApplRedRat;

    pInstance->NetworkCycle.CycleBaseFactorCACF = (LSA_UINT16)(pInstance->StartParam.CACF * pInstance->StartParam.ReductionRatio * pInstance->NetworkCycle.CycleBaseFactor);

    //calc max transfer time
    #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
    Tk_max_ns = (pInstance->StartParam.Tk.SeparateTracks.Tki_ns > pInstance->StartParam.Tk.SeparateTracks.Tko_ns)?pInstance->StartParam.Tk.SeparateTracks.Tki_ns:pInstance->StartParam.Tk.SeparateTracks.Tko_ns;
    #else
    Tk_max_ns = pInstance->StartParam.Tk.Tk_ns;  
    #endif
        
    //check Tk_ns/Tm_ns/Te_ns
    if (   (Tk_max_ns > ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tm_ns))
        || (pInstance->StartParam.Tm_ns > ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - Tk_max_ns))
        || (pInstance->StartParam.Te_ns > ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - Tk_max_ns)))
    {
        EDDI_ISO_FREE_INSTANCE(pInstance);
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_ISO_CFG_NO_ZE_INT)
    //evaluate cycle and offset in this cycle where Ze occurs
    pInstance->NetworkCycle.CycleCtrMin_Ze      = (LSA_UINT16)(((pInstance->StartParam.Te_ns/10UL) / TempCycleLen_10ns) * pInstance->NetworkCycle.CycleBaseFactor);
    pInstance->NetworkCycle.PhaseOffset_Ze_10ns = (pInstance->StartParam.Te_ns/10UL) % TempCycleLen_10ns;
    #endif

    #if defined (EDDI_ISO_CFG_NO_ZS_INT)
    {
        #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
        //evaluate cycle and offset in this cycle where Zsi and Zso occurs
        LSA_UINT32 const TsiLocal_10ns = ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.SeparateTracks.Tki_ns)/10UL;
        LSA_UINT32 const TsoLocal_10ns = ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.SeparateTracks.Tko_ns)/10UL;

        pInstance->NetworkCycle.CycleCtrMin_Zsi      = (LSA_UINT16)((TsiLocal_10ns / TempCycleLen_10ns) * pInstance->NetworkCycle.CycleBaseFactor);
        pInstance->NetworkCycle.PhaseOffset_Zsi_10ns = TsiLocal_10ns % TempCycleLen_10ns;
        pInstance->NetworkCycle.CycleCtrMin_Zso      = (LSA_UINT16)((TsoLocal_10ns / TempCycleLen_10ns) * pInstance->NetworkCycle.CycleBaseFactor);
        pInstance->NetworkCycle.PhaseOffset_Zso_10ns = TsoLocal_10ns % TempCycleLen_10ns;
        #else
        //evaluate cycle and offset in this cycle where Zs occurs
        LSA_UINT32 const TsLocal_10ns = ((pInstance->NetworkCycle.CycleLen_ns * pInstance->StartParam.ReductionRatio) - pInstance->StartParam.Tk.Tk_ns)/10UL;

        pInstance->NetworkCycle.CycleCtrMin_Zs      = (LSA_UINT16)((TsLocal_10ns / TempCycleLen_10ns) * pInstance->NetworkCycle.CycleBaseFactor);
        pInstance->NetworkCycle.PhaseOffset_Zs_10ns = TsLocal_10ns % TempCycleLen_10ns;
        #endif
    }
    #endif

    #if defined (EDDI_ISO_CFG_TRACE_DEPTH)
    pInstance->pClkCntVal = (LSA_UINT32 *)((LSA_UINT32)pInstance->StartParam.pIRTE_PNIP + CLK_COUNT_VALUE);
    #endif

    //return XPLLOUT cyclelenghth
    *(pParam->Param.Start.pXPLLOUT_CycleLen_ns) = pInstance->ApplCycle.ClkCyPeriod_10ns * 10UL;

    if (bCheck)
    {
        //check only, free instance
        EDDI_ISO_FREE_INSTANCE(pInstance);
        return EDD_STS_OK;
    }
    else
    {
        //***************************************
        //* checks passed, start coupling!
        //***************************************
        //set return-handle, reserve instance
        *(pParam->Param.Start.pHandle) = pInstance->Instance;

        EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_START);

        return EDD_STS_OK_PENDING;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//executes the command "EDD_APPLSYNC_CMD_RESTART"
//*****************************************************************************
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_ApplSyncRESTART_CMD(EDD_APPL_SYNC_PARAM_TYPE * const pParam)
{
    EDDI_ISO_CB_INSTANCE_TYPE  *  pInstance;

    //on IRTE, this command does nothing!
    if (EDDI_IsoCheckHandle(pParam->Param.Restart.Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }

    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_RESTART_CMD, 0 /*trigger*/, 0 /*detail*/);

    switch (pInstance->State)
    {
        case EDDI_ISO_STATE_PREPARE:
        case EDDI_ISO_STATE_CHECKPHASE_START:
        case EDDI_ISO_STATE_CHECKPHASE_WAIT:
        case EDDI_ISO_STATE_CHECKPHASE_CHECK:
        case EDDI_ISO_STATE_FREERUN:
        {
            EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_RESTART);
            return EDD_STS_OK_PENDING;
        }
        case EDDI_ISO_STATE_INIT:
        case EDDI_ISO_STATE_COUPLED:
        case EDDI_ISO_STATE_STOPPED:
        default:
        {
            return EDD_STS_ERR_SEQUENCE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//executes the command "EDD_APPLSYNC_CMD_CHECK"
//*****************************************************************************
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_ApplSyncCHECK_CMD(EDD_APPL_SYNC_PARAM_TYPE * const pParam)
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE pInstance;

    if (EDDI_IsoCheckHandle(pParam->Param.Check.Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }

    switch (pInstance->State)
    {
        case EDDI_ISO_STATE_PREPARE:
        case EDDI_ISO_STATE_CHECKPHASE_START:
        case EDDI_ISO_STATE_CHECKPHASE_WAIT:
        case EDDI_ISO_STATE_CHECKPHASE_CHECK:
        {
            return EDD_STS_OK_PENDING;
            //break;
        }
        case EDDI_ISO_STATE_FREERUN:
        case EDDI_ISO_STATE_COUPLED:
        {
            return EDD_STS_OK;
            //break;
        }
        case EDDI_ISO_STATE_INIT:
        case EDDI_ISO_STATE_STOPPED:
        default:
        {
            return EDD_STS_ERR_SEQUENCE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//executes the command "EDD_APPLSYNC_CMD_FREERUN"
//*****************************************************************************
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_ApplSyncFREERUN_CMD(EDD_APPL_SYNC_PARAM_TYPE * const pParam)
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE pInstance;
    //on IRTE, this command does nothing!
    if (EDDI_IsoCheckHandle(pParam->Param.FreeRun.Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }
    
    switch (pInstance->State)
    {
        case EDDI_ISO_STATE_PREPARE:
        case EDDI_ISO_STATE_CHECKPHASE_START:
        case EDDI_ISO_STATE_CHECKPHASE_WAIT:
        case EDDI_ISO_STATE_CHECKPHASE_CHECK:
        {
            EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_FREERUN);
            return EDD_STS_OK_PENDING;
        }
        case EDDI_ISO_STATE_COUPLED:
        {
            EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_FREERUN);
            return EDD_STS_OK;
        }
        case EDDI_ISO_STATE_INIT:
        case EDDI_ISO_STATE_FREERUN:
        case EDDI_ISO_STATE_STOPPED:
        default:
        {
            return EDD_STS_ERR_SEQUENCE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//executes the command "EDD_APPLSYNC_CMD_STOP"
//*****************************************************************************
static LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_ApplSyncSTOP_CMD(EDD_APPL_SYNC_PARAM_TYPE * const pParam)
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE pInstance;

    if (EDDI_IsoCheckHandle(pParam->Param.Stop.Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }

    EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_STOP);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Dummy-Interrupthandler
//*****************************************************************************
LSA_RESULT EDDI_SYSTEM_IN_FCT_ATTR EDDI_IsoInthandlerDummy(LSA_VOID * const pVInstance, LSA_UINT32 const Int)
{
    LSA_UNUSED_ARG(pVInstance);
    LSA_UNUSED_ARG(Int);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Inthandler for working states
//*****************************************************************************
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoInthandlerWork( LSA_VOID  *  const  pVInstance, 
                                                             LSA_UINT32   const  Int )
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance = (EDDI_ISO_CB_INSTANCE_PTR_TYPE)pVInstance;
    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    LSA_UINT32                            reg;
    #endif
    LSA_UINT64                            CycleCtr64;
    #if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
    LSA_UINT32                            ApplTimeOffset_ns;
    LSA_UINT32                            Delta_ns;
    #endif

    EDDI_ENTER_APPLSYNC_S();

    #if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
    //get current position in ns to start of application-cycle
    EDDI_ISO_GET_APPLTIME_OFFSET(pInstance, CycleCtr64, ApplTimeOffset_ns);
    #endif

    #if defined (EDDI_ISO_CFG_PASS_INTREG)
    if (Int & EDDI_ISO_INT_ZA)
    #else
    if (Int & EDD_APPLSYNC_INT_TA)
    #endif
    {
        #if defined (EDDI_ISO_CFG_TRACE_INTS)
        EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_INT, 4 /*trigger*/, reg /*detail*/);
        #endif

        //Acknowledge Za-Int (this one)
        EDDI_APPLSYNC_ACK_INTS(pInstance, EDDI_ISO_INT_ZA);

        #if !(defined (EDDI_ISO_CFG_NO_ZE_INT) && defined (EDDI_ISO_CFG_NO_ZS_INT))
        //clear Ze-Int (!early), Zs-Int (late) or Zsi (late input) / Zso (late output)
        EDDI_APPLSYNC_CLEAR_INTS(pInstance, (EDDI_ISO_INT_ZE+EDDI_ISO_INT_ZS));

        #if defined (EDDI_ISO_CFG_TRACE_INTS)
        EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_INT, 5 /*trigger*/, reg /*detail*/);
        #endif

        #if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
        Delta_ns = ApplTimeOffset_ns;
        pInstance->Stat.Za.CurTime_ns = Delta_ns;
        if  (Delta_ns > pInstance->Stat.Za.MaxTime_ns)
        {
            pInstance->Stat.Za.MaxTime_ns = Delta_ns;
        }
        if  (Delta_ns < pInstance->Stat.Za.MinTime_ns)
        {
            pInstance->Stat.Za.MinTime_ns = Delta_ns;
        }
        #endif
        #else
        
        //if both ints are not used, TA is not needed!
        //Mask Ta-ints
        EDDI_APPLSYNC_MASK_INTS(pInstance, EDDI_ISO_INT_ZA, LSA_FALSE /*!Enable*/);
        #endif //defined (EDDI_ISO_CFG_NO_ZE_INT) && defined (EDDI_ISO_CFG_NO_ZS_INT)
    }

    #if defined (EDDI_ISO_CFG_PASS_INTREG)
    if (Int & EDDI_ISO_INT_ZM)
    #else
    if (Int & EDD_APPLSYNC_INT_TM)
    #endif
    {
        #if defined (EDDI_ISO_CFG_TRACE_INTS)
        EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CALLCBF, 1 /*trigger*/, reg /*detail*/);
        #endif

        //clear Zit-Int (in_time)
        EDDI_APPLSYNC_CLEAR_INTS(pInstance, EDDI_ISO_INT_ZIT);

        #if defined (EDDI_ISO_CFG_TRACE_INTS)
        EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
        EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CALLCBF, 2 /*trigger*/, reg /*detail*/);
        #endif

        //call cbf
        if (!pInstance->bCBFLocked)
        {
            LSA_UINT8  bOBLoss = EDD_APPLSYNC_OB_CALL_IN_SEQ;

            //get current cyclectr
            #if !defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
            //EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtr);
            CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);
            #endif
            //The last call of the cbf has to have occured in the last application-cycle
            if ((CycleCtr64 - pInstance->NetworkCycle.LastCBFCycleCtr) >= (2 * pInstance->NetworkCycle.CycleBaseFactorCACF))
            {
                bOBLoss = EDD_APPLSYNC_OB_CALL_LOST;
            }

            pInstance->NetworkCycle.LastCBFCycleCtr = CycleCtr64 / pInstance->NetworkCycle.CycleBaseFactorCACF; //temporarily_disabled_lint !e734
            pInstance->NetworkCycle.LastCBFCycleCtr = pInstance->NetworkCycle.LastCBFCycleCtr * pInstance->NetworkCycle.CycleBaseFactorCACF; //temporarily_disabled_lint !e734

            pInstance->StartParam.pCBF(pInstance->StartParam.UserID, bOBLoss);

            #if defined (EDDI_ISO_CFG_ENABLE_INT_STATISTICS)
            Delta_ns = ApplTimeOffset_ns - pInstance->StartParam.Tm_ns;
            pInstance->Stat.Zm.CurTime_ns = Delta_ns;
            if (Delta_ns > pInstance->Stat.Zm.MaxTime_ns)
            {
                pInstance->Stat.Zm.MaxTime_ns = Delta_ns;
            }
            if  (Delta_ns < pInstance->Stat.Zm.MinTime_ns)
            {
                pInstance->Stat.Zm.MinTime_ns = Delta_ns;
            }
            #endif
        }

        //EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
        //EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CALLCBF, 3 /*trigger*/, reg /*detail*/);

        //Acknowledge Zm-Int
        EDDI_APPLSYNC_ACK_INTS(pInstance, EDDI_ISO_INT_ZM);
    }

    EDDI_EXIT_APPLSYNC_S();

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

//*****************************************************************************
//Inthandler for starting states
//*****************************************************************************
LSA_RESULT  EDDI_SYSTEM_IN_FCT_ATTR  EDDI_IsoInthandlerStart( LSA_VOID  *  const  pVInstance, 
                                                              LSA_UINT32   const  Int )
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  const  pInstance = (EDDI_ISO_CB_INSTANCE_PTR_TYPE)pVInstance;

    #if defined (EDDI_ISO_CFG_TRACE_INTS)
    EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_INT, Int , 0);
    #endif
    
    #if defined (EDDI_ISO_CFG_PASS_INTREG)
    if (Int & EDDI_ISO_INT_NewCycle)
    #else
    if (Int & EDD_APPLSYNC_INT_NEWCYCLE)
    #endif
    {
        EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_NEWCYCLE);
    }
    #if defined (EDDI_ISO_CFG_PASS_INTREG)
    if (Int & EDDI_ISO_INT_ZA)
    #else
    if (Int & EDD_APPLSYNC_INT_TA)
    #endif
    {
        //Acknowledge Za-Int (this one)
        EDDI_APPLSYNC_ACK_INTS(pInstance, EDDI_ISO_INT_ZA);
        EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_TAINT);
    }
    #if defined (EDDI_ISO_CFG_PASS_INTREG)
    if (Int & EDDI_ISO_INT_ZM)
    #else
    if (Int & EDD_APPLSYNC_INT_TM)
    #endif
    {
        //Acknowledge Zm-Int (this one)
        EDDI_APPLSYNC_ACK_INTS(pInstance, EDDI_ISO_INT_ZM);
        EDDI_IsoSm(pInstance, EDDI_ISO_TRIGGER_TMINT);
    }

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*===========================================================================*/
/*                              global functions                             */
/*===========================================================================*/

/*=============================================================================
 * function name: eddi_ApplSync_Init()
 *
 * function:      Initialisation of the interface for application-synchronisation
 *
 * parameters:    
 *
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID  EDD_UPPER_IN_FCT_ATTR  eddi_ApplSync_Init( LSA_VOID )
{
    LSA_UINT16     i;
    LSA_UINT8   *  ptr8 = (LSA_UINT8 *)(void *)&eddi_iso_cb;

    //clear controlblock
    for (i=0; i<sizeof(eddi_iso_cb); i++)
    {
        *ptr8++=0;
    }

    //set initial state
    for (i=0; i<=EDDI_ISO_CFG_MAX_NR_INSTANCES; i++)
    {
        eddi_iso_cb.Instance[i].State          = EDDI_ISO_STATE_INIT;
        eddi_iso_cb.Instance[i].Instance       = EDDI_ISO_INSTANCE_NOT_USED;
        eddi_iso_cb.Instance[i].pIntHandlerFct = EDDI_IsoInthandlerDummy;
        eddi_iso_cb.Instance[i].bCBFLocked     = LSA_TRUE;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/

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
                                                        EDD_APPL_SYNC_PARAM_TYPE  *  const  pParam )
{
    LSA_RESULT  result;

    switch (pParam->Cmd)
    {
        case EDD_APPLSYNC_CMD_CHECK_PARAMS:
        {
            result = EDDI_ApplSyncSTART_CMD(hDDB, pParam, LSA_TRUE /*bIFSync:don´t care*/, LSA_TRUE /*bCheck*/);
            break;
        }
        
        case EDD_APPLSYNC_CMD_START_SYNC:
        {
            result = EDDI_ApplSyncSTART_CMD(hDDB, pParam, LSA_TRUE /*bIFSync:interface is sync*/, LSA_FALSE /*bCheck*/);
            break;
        }
        
        case EDD_APPLSYNC_CMD_START_ASYNC:
        {
            result = EDDI_ApplSyncSTART_CMD(hDDB, pParam, LSA_FALSE /*bIFSync:interface is not sync*/, LSA_FALSE /*bCheck*/);
            break;
        }
        
        case EDD_APPLSYNC_CMD_RESTART:
        {
            result = EDDI_ApplSyncRESTART_CMD(pParam);
            break;
        }
        case EDD_APPLSYNC_CMD_CHECK:
        {
            result = EDDI_ApplSyncCHECK_CMD(pParam);
            break;
        }
        case EDD_APPLSYNC_CMD_FREERUN:
        {
            result = EDDI_ApplSyncFREERUN_CMD(pParam);
            break;
        }
        case EDD_APPLSYNC_CMD_STOP:
        {
            result = EDDI_ApplSyncSTOP_CMD(pParam);
            break;
        }
        default:
        {
            result = EDD_STS_ERR_PARAM;
        }
    }

    return result;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=============================================================================
 * function name: eddi_ApplSync_CheckTracks()
 *
 * function:      Check the tracks "early", "late", and "in_time"
 *
 * parameters:    Handle  Handle of this instance
 *                bStoreCycle  LSA_TRUE:  Store CycleCtr of 1st ApplPhase
 *                             LSA_FALSE: Check against stored CycleCtr
 *
 * return value:  EDD_STS_OK, EDD_STS_PENDING, EDD_STS_ERR_SEQUENCE, EDD_STS_ERR_PARAM
 *
 *                pTracks Track-bits
 *===========================================================================*/
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_CheckTracks (LSA_UINT16 const Handle, LSA_UINT8 * const pTracks, LSA_UINT8 const bStoreCycle)
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  pInstance;
    LSA_UINT8                      Tracks = 0;
    LSA_UINT32                     reg;
    LSA_UINT64                     CycleCtr64;
    #if defined (EDDI_ISO_CFG_NO_ZE_INT) || defined (EDDI_ISO_CFG_NO_ZS_INT)
    LSA_UINT32                     CycleTimeVal_10ns;
    LSA_UINT16                     ActualCycleCtrMod;
    #endif

    if (EDDI_IsoCheckHandle(Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }

    #if defined (EDDI_ISO_CFG_NO_ZE_INT) || defined (EDDI_ISO_CFG_NO_ZS_INT)
    //get cycle time
    EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_TIME_VALUE, CycleTimeVal_10ns);
    #endif
    
    //get cycle ctr
    CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);
       
    //check for oversize OB
    if (EDDI_APPLSYNC_CHECKTRACKS_STORE_CYCLE_ON == bStoreCycle)
    {
        //Store CycleCtr
        pInstance->NetworkCycle.CheckSFCCycleCtr = (CycleCtr64 / pInstance->NetworkCycle.CycleBaseFactorCACF) * pInstance->NetworkCycle.CycleBaseFactorCACF;  //temporarily_disabled_lint !e734

        //Only storage of CycleCtr is requested. eddi_ApplSync_CheckTracks is then called 3 times:
        //1. Store cycle only, directly in Tm-Inthandler: pTracks = 0, bStoreCycle = LSA_TRUE
        //2. Check at beginning of synchronous task/OB:   pTracks = 0xXXXXXXX, bStoreCycle = LSA_FALSE
        //3. Check at end of synchronous task/OB:         pTracks = 0xXXXXXXX, bStoreCycle = LSA_FALSE
        if (!pTracks)
        {
            return (EDD_STS_OK);
        }
    }
    else
    {
        //Check CycleCtr
        if (pInstance->NetworkCycle.CheckSFCCycleCtr != ((CycleCtr64 / pInstance->NetworkCycle.CycleBaseFactorCACF) * pInstance->NetworkCycle.CycleBaseFactorCACF))
        {
            Tracks = EDD_APPLSYNC_TRACK_MASK_LATE;
            //EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CHECKTRACKS, Tracks|0x80 , (LSA_UINT32)CycleCtr64 /*detail*/);
        }

        if (!pTracks)
        {
            return (EDD_STS_ERR_PARAM);
        }
    }

    if (0 == Tracks)
    {
        //get IR-register (all ints, not only the unmasked ones)
        EDDI_ISO_R32(pInstance->pIntReqReg, 0, reg);
      
        #if defined (EDDI_ISO_CFG_NO_ZE_INT) || defined (EDDI_ISO_CFG_NO_ZS_INT)
        //calc actual CycleCtrMod
        ActualCycleCtrMod = (LSA_UINT16)(CycleCtr64 % pInstance->NetworkCycle.CycleBaseFactorCACF);
        #endif
        
        /****************
         * check in-time
         ****************/
        if (!(reg & EDDI_ISO_INT_ZIT))
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_INTIME;
        }

        /****************
         * check late
         ****************/
        #if defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
        #if !defined (EDDI_ISO_CFG_NO_ZS_INT)
        if (reg & EDDI_ISO_INT_ZS) //late int for input and output
        {
            //compare single flag and set mask
            if (reg & EDDI_ISO_INT_ZSI) {Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_I;}
            if (reg & EDDI_ISO_INT_ZSO) {Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_O;}
        }
        #else
        //check Zsi:
        //Zsi is reached when:
        //- the current (application)-phase is greater than the nominal phase at Tsi
        //- the current offset in this phase is greater than the nominal offset
        if (ActualCycleCtrMod > pInstance->NetworkCycle.CycleCtrMin_Zsi)
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_I;
        }
        else if (   (ActualCycleCtrMod == pInstance->NetworkCycle.CycleCtrMin_Zsi)
                 && (CycleTimeVal_10ns >  pInstance->NetworkCycle.PhaseOffset_Zsi_10ns))
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_I;
        }
        //check Zso:
        //Zso is reached when:
        //- the current (application)-phase is greater than the nominal phase at Tso
        //- the current offset in this phase is greater than the nominal offset
        if (ActualCycleCtrMod > pInstance->NetworkCycle.CycleCtrMin_Zso)
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_O;
        }
        else if (   (ActualCycleCtrMod == pInstance->NetworkCycle.CycleCtrMin_Zso)
                 && (CycleTimeVal_10ns >  pInstance->NetworkCycle.PhaseOffset_Zso_10ns))
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE_O;
        }
        #endif
        #else
        #if !defined (EDDI_ISO_CFG_NO_ZS_INT)
        if (reg & EDDI_ISO_INT_ZS)
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE;
        }
        #else
        //check Zs:
        //Zs is reached when:
        //- the current (application)-phase is greater than the nominal phase at Ts
        //- the current offset in this phase is greater than the nominal offset
        if (ActualCycleCtrMod > pInstance->NetworkCycle.CycleCtrMin_Zs)
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE;
        }
        else if (   (ActualCycleCtrMod == pInstance->NetworkCycle.CycleCtrMin_Zs)
                 && (CycleTimeVal_10ns > pInstance->NetworkCycle.PhaseOffset_Zs_10ns))
        {
            Tracks |= EDD_APPLSYNC_TRACK_MASK_LATE;
        }
        #endif
        #endif // defined (EDDI_ISO_CFG_SEPARATE_SFC_TRACKS)
        
        /*******************************************
         * check early, only if late is not set yet
         *******************************************/
        if (!(Tracks & EDD_APPLSYNC_TRACK_MASK_LATE))
        {
            #if !defined (EDDI_ISO_CFG_NO_ZE_INT)
            if (!(reg & EDDI_ISO_INT_ZE))
            {
                Tracks |= EDD_APPLSYNC_TRACK_MASK_EARLY;
            }
            #else
            //check Ze:
            //Ze is reached when:
            //- the current (application)-phase is greater than the nominal phase at Te
            //- the current offset in this phase is greater than the nominal offset
            if (ActualCycleCtrMod < pInstance->NetworkCycle.CycleCtrMin_Ze)
            {
                Tracks |= EDD_APPLSYNC_TRACK_MASK_EARLY;
            }
            else if (   (ActualCycleCtrMod == pInstance->NetworkCycle.CycleCtrMin_Ze)
                     && (CycleTimeVal_10ns < pInstance->NetworkCycle.PhaseOffset_Ze_10ns))
            {
                Tracks |= EDD_APPLSYNC_TRACK_MASK_EARLY;
            }
            #endif
        }
    }

    *pTracks = Tracks;

    //EDDI_ISO_TRACE(pInstance, EDDI_ISO_TRACE_CHECKTRACKS, Tracks , reg /*detail*/);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

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
                                                            LSA_UINT32  const  Int )
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  pInstance;
    LSA_RESULT                     retval;

    if (EDDI_IsoCheckHandle(Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }

    retval = pInstance->pIntHandlerFct(pInstance, Int);

    return retval;
}
/*---------------------- end [subroutine] ---------------------------------*/

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
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_GetApplTimeOffset( LSA_UINT16   const Handle, 
                                                                  LSA_UINT32 * const pApplTimeOffset_ns )
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  pInstance;
    LSA_UINT64                     CycleCtr64;
    LSA_UINT32                     ApplTimeOffset_ns;
    
    //get instance
    if (EDDI_IsoCheckHandle(Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }
    
    //check actual state
    if (   (pInstance->State != EDDI_ISO_STATE_COUPLED)
        && (pInstance->State != EDDI_ISO_STATE_FREERUN))
    {
        //only possible if coupled or freerun
        return EDD_STS_ERR_SEQUENCE;
    }
    
    EDDI_ISO_GET_APPLTIME_OFFSET(pInstance, CycleCtr64, ApplTimeOffset_ns);
    *pApplTimeOffset_ns = ApplTimeOffset_ns;
    
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

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
LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_GetTimeValues (LSA_UINT16                       const  Handle,
	                                                          EDD_APPL_SYNC_TIME_VALUES_TYPE * const  pTimeValues)
{
    EDDI_ISO_CB_INSTANCE_PTR_TYPE  pInstance;
    LSA_UINT64                     CycleCtr64;
    LSA_UINT32                     CycleTimeVal_10ns;
    LSA_UINT32                     TimeStamp;
    LSA_UINT32                     CycleCtrLow;
    #if defined (EDDI_ISO_CFG_MAX_READ_CTR)
    LSA_UINT8                      Ctr;
    #endif
    
    //get instance
    if (EDDI_IsoCheckHandle(Handle, &pInstance) != EDD_STS_OK)
    {
        return EDD_STS_ERR_PARAM;
    }
    
    //check actual state
    if (   (pInstance->State != EDDI_ISO_STATE_COUPLED)
        && (pInstance->State != EDDI_ISO_STATE_FREERUN))
    {
        //only possible if coupled or freerun
        return EDD_STS_ERR_SEQUENCE;
    }
    
    #if defined (EDDI_ISO_CFG_MAX_READ_CTR)
    for (Ctr=0; Ctr<EDDI_ISO_CFG_MAX_READ_CTR; Ctr++)
    #endif //(EDDI_ISO_CFG_MAX_READ_CTR)
    {
        CycleCtr64 = EDDI_IsoGetCycleCtr64(pInstance);
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_TIME_VALUE, CycleTimeVal_10ns);
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CLK_COUNT_VALUE, TimeStamp);
        #if !defined (EDDI_ISO_CFG_MAX_READ_CTR)
        CycleCtrLow = (LSA_UINT32)(CycleCtr64 && 0xFFFFFFFF);
        #else
        EDDI_ISO_R32(pInstance->StartParam.pIRTE_PNIP, CYCL_COUNT_VALUE, CycleCtrLow);
        if (CycleCtrLow == (LSA_UINT32)(CycleCtr64 & 0xFFFFFFFF) )
        #endif //(EDDI_ISO_CFG_MAX_READ_CTR)
        {
            pTimeValues->ApplTimeOffset_ns  = (((LSA_UINT16)(CycleCtr64 % pInstance->NetworkCycle.CycleBaseFactorCACF)*3125UL) + CycleTimeVal_10ns) * 10UL;
            pTimeValues->CycleCounterLow    = CycleCtrLow;
            pTimeValues->TimeStamp_ns       = TimeStamp;
            return (EDD_STS_OK);
        }
    }

    #if defined (EDDI_ISO_CFG_MAX_READ_CTR)
    return (EDD_STS_ERR_TIMEOUT);
    #endif //(EDDI_ISO_CFG_MAX_READ_CTR)
}
/*---------------------- end [subroutine] ---------------------------------*/

#else //defined (EDDI_INTCFG_ISO_SUPPORT_ON)

LSA_VOID EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_Init( LSA_VOID )
{
}

LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_Param( EDDI_HANDLE const hDDB, EDD_APPL_SYNC_PARAM_TYPE * const pParam )
{
    LSA_UNUSED_ARG(pParam);
    LSA_UNUSED_ARG(hDDB);
    return EDD_STS_ERR_NOT_SUPPORTED;
}

LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_CheckTracks( LSA_UINT16 const Handle, LSA_UINT8 * const pTracks, LSA_UINT8 const bStoreCycle )
{
    LSA_UNUSED_ARG(Handle);
    LSA_UNUSED_ARG(pTracks);
    LSA_UNUSED_ARG(bStoreCycle);
    return EDD_STS_ERR_NOT_SUPPORTED;
}

LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_Interrupt( LSA_UINT16 const Handle, LSA_UINT32 const Int )
{
    LSA_UNUSED_ARG(Handle);
    LSA_UNUSED_ARG(Int);
    return EDD_STS_ERR_NOT_SUPPORTED;
}

LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_GetApplTimeOffset( LSA_UINT16   const Handle, 
                                                                  LSA_UINT32 * const pApplTimeOffset_ns )
{
    LSA_UNUSED_ARG(Handle);
    LSA_UNUSED_ARG(pApplTimeOffset_ns);
    return EDD_STS_ERR_NOT_SUPPORTED;
}

LSA_RESULT EDD_UPPER_IN_FCT_ATTR eddi_ApplSync_GetTimeValues (LSA_UINT16                       const  Handle,
	                                                          EDD_APPL_SYNC_TIME_VALUES_TYPE * const  pTimeValues)
{
    LSA_UNUSED_ARG(Handle);
    LSA_UNUSED_ARG(pTimeValues);
    return EDD_STS_ERR_NOT_SUPPORTED;
}

#endif //defined (EDDI_INTCFG_ISO_SUPPORT_ON)


/*****************************************************************************/
/*  end of file eddi_io_iso.c                                                */
/*****************************************************************************/

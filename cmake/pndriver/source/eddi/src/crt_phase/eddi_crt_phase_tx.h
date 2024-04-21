#ifndef EDDI_CRT_PHASE_TX_H     //reinclude-protection
#define EDDI_CRT_PHASE_TX_H

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
/*  F i l e               &F: eddi_crt_phase_tx.h                       :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* D e s c r i p t i o n:                                                    */
/*                                                                           */
/* CRT-                                                                      */
/* Defines constants, types, macros and prototyping for prefix.              */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/* H i s t o r y :                                                           */
/* ________________________________________________________________________  */
/*                                                                           */
/* Date      Who   What                                                      */
/*                                                                           */
/*****************************************************************************/

// ATTENTION: This enum is related to FunctionTable CrtPhaseTxFctTable !!
typedef enum _EDDI_CRT_PHASE_TX_STATE
{
    EDDI_CRT_PHASE_TX_STS_NO               = 0,             // No IRT, No ISRT
    EDDI_CRT_PHASE_TX_STS_IRT              = 1,             // IRT only
    EDDI_CRT_PHASE_TX_STS_IRT_SHIFTED      = 2              // IRT only with Starttime

} EDDI_CRT_PHASE_TX_STATE;

typedef enum _EDDI_CRT_PHASE_TX_EVENT
{
    EDDI_CRT_PHASE_TX_EVT_IRT_ACTIVATE,                     // No IRT, No ISRT  //StartTime=0
    EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT,           // ISRT only

    EDDI_CRT_PHASE_TX_EVT_SHIFT,                            // No IRT, No ISRT with StartTime
    EDDI_CRT_PHASE_TX_EVT_UNSHIFT,                          // No IRT, No ISRT without StartTime
    EDDI_CRT_PHASE_TX_EVT_IRT_DEACTIVATE_UNSHIFT_FINISH,
    EDDI_CRT_PHASE_TX_EVT_SHIFT_FINISH,
    EDDI_CRT_PHASE_TX_EVT_UNSHIFT_FINISH

} EDDI_CRT_PHASE_TX_EVENT;

typedef struct _EDDI_CRT_PHASE_TX_MACHINE
{
    EDDI_CRT_PHASE_TX_STATE                           State;
    LSA_UINT32                                        HwPortIndex;

    #if defined (EDDI_RED_PHASE_SHIFT_ON)
    LSA_UINT32                                        StartTime10Ns;          // parameter is only set at the start of shifting/unshifting-sequences
    EDDI_SER_IRT_PORT_START_TIME_TX_BLOCK_PTR_TYPE    pIrtPortStartTimeTxBlock;
    volatile  LSA_UINT32                           *  pFcwListHead;           // Pointer to FCW-List-Header
    EDDI_SER_CCW_PTR_TYPE                             pRootCw;                // Pointer to the RootCw (NOOP)
    volatile  LSA_UINT32                              pRootCwAsic;            // Pointer to the RootCw (NOOP) in asic-format with the right endianess
    volatile  LSA_UINT32                              pShiftListAsic;         // Pointer to the start of Shift-List in asic-format with the right endianess, used in FCW-List
    volatile  LSA_UINT32                              pShiftSolAsic;          // Pointer to the Shift-SOL in asic-format with the right endianess, used in FCW-List
    EDDI_LOCAL_EOL_PTR_TYPE                           pEOLMax;                // Pointer to the EOL with the maximum start-time
    LSA_UINT32                                        IrtStartTimeSndRegAdr;  // IRTE-Register-Adress

    //Transition-Flags:
    LSA_BOOL                                          bSOLRunning;            // TRUE if a SOL was activated to set or reset IRTStartTimeRegister.
    LSA_BOOL                                          bDisablingFCWs;         // TRUE if the FCW-List is disabled, either by disconnecting the listheader or
                                                                              // by disabling the IRTCONTROL-Bit. This transition lasts one cycle.

    //Finish-Event:
    EDDI_CRT_PHASE_TX_EVENT                           FinishEvent;            // Event to be used to finish a transition.
    #endif

    LSA_UINT32                                        TxFcwSolTimeMac10Ns;    // time is already corrected by Preframe-Send-Delay!
                                                                              // this parameter is only used for FCWs!

} EDDI_CRT_PHASE_TX_MACHINE;

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_CRT_PHASE_TX_H


/*****************************************************************************/
/*  end of file eddi_crt_phase_tx.h                                          */
/*****************************************************************************/

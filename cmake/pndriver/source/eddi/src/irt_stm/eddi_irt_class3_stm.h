#ifndef EDDI_IRT_CLASS3_STM_H   //reinclude-protection
#define EDDI_IRT_CLASS3_STM_H

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
/*  F i l e               &F: eddi_irt_class3_stm.h                     :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

//RtClass3_OutputState
#define EDDI_RT_CLASS3_STATE_OFF     0UL  // RX-Port off and not shifted       TX-Port off    and not shifted
#define EDDI_RT_CLASS3_STATE_TX      1UL  // RX-Port off and not shifted       TX-Port on     and not shifted
#if defined (EDDI_RED_PHASE_SHIFT_ON)
#define EDDI_RT_CLASS3_STATE_RXsTX   2UL  // RX-Port on  and shifted           TX-Port on     and not shifted
#endif
#define EDDI_RT_CLASS3_STATE_RXsTXs  3UL  // RX-Port on  and shifted           TX-Port on     and shifted

//State machine for RtClass3 Activity for a single port
typedef struct _EDDI_RT_CLASS3_MACHINE
{
    LSA_UINT32  RtClass3_OutputState;
    LSA_BOOL    bFinishNecessary;
    LSA_BOOL    bHandleAsyncLinkDown;

} EDDI_RT_CLASS3_MACHINE;

typedef struct _EDDI_RT_CLASS3_MACHINE EDDI_LOCAL_MEM_ATTR * EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE;

/*---------------------------------------------------------------------------*/
/* External functions                                                        */
/*---------------------------------------------------------------------------*/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_IrtClass3StmsInit   (EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                       LSA_UINT32                                 const  HwPortIndex);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_IrtClass3StmsTrigger(EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                       LSA_UINT32                                 const  HwPortIndex,
                                                       LSA_BOOL                                   const  bOldSolTransitionRunning);

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_IrtClass3StmsFinish (EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                       EDDI_LOCAL_DDB_RT_CLASS3_MACHINE_PTR_TYPE  const  pRtClass3_Machine,
                                                       LSA_UINT32                                 const  HwPortIndex);

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_IRT_CLASS3_STM_H


/*****************************************************************************/
/*  end of file eddi_irt_class3_stm.h                                        */
/*****************************************************************************/

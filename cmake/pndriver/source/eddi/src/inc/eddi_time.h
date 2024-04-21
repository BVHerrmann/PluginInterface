#ifndef EDDI_TIME_H             //reinclude-protection
#define EDDI_TIME_H

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
/*  F i l e               &F: eddi_time.h                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  Version of prefix                                                        */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#define EDDI_TIMER_REST     0xF

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_TimerInfoFree( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_AllocTimer( EDDI_LOCAL_DDB_PTR_TYPE    const pDDB,
                                                LSA_UINT16               * eddi_timer_id_ptr,
                                                void                     * context,
                                                const EDDI_TIMEOUT_CBF     cbf,
                                                LSA_UINT16                 TimerType,
                                                LSA_UINT16                 TimeBase,
                                                const LSA_UINT32           QueueIdx );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_StartTimer( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                LSA_UINT16              const eddi_timer_id,
                                                LSA_UINT16              const TimeTics );

LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_StopTimer( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                               LSA_UINT16              const eddi_timer_id );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_TIME_H


/*****************************************************************************/
/*  end of file eddi_time.h                                                  */
/*****************************************************************************/

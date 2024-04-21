
#ifndef CLP_SYS_H                     /* ----- reinclude-protection ----- */
#define CLP_SYS_H

#ifdef __cplusplus                       /* If C++ - compiler: Use C linkage */
extern "C"
{
#endif

/*****************************************************************************/
/*  Copyright (C) 2xxx Siemens Aktiengesellschaft. All rights reserved.      */
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
/*  C o m p o n e n t     &C: GSY (Generic Sync Module)                 :C&  */
/*                                                                           */
/*  F i l e               &F: clp_sys.h                                 :F&  */
/*                                                                           */
/*  V e r s i o n         &V: LSA_PNIO_Pxx.xx.xx.xx_xx.xx.xx.xx         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: xxxx-xx-xx                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*  System interface                                                         */
/*  Defines constants, types, macros and prototyping for CLP.                */
/*                                                                           */
/*****************************************************************************/
#ifdef GSY_MESSAGE /**********************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  2006-06-02  P03.11.00.00_00.01.00.00 lrg  new                            */
/*  2006-06-13  P03.12.00.00_00.02.01.02 lrg  Neue Rueckgabewerte fuer clp_  */
/*              SyncAlgorithm(): *pProportionalTerm,*pIntegralTerm           */
/*  2007-11-21  P04.00.00.00_00.60.01.02 lrg  CLP_CALC64                     */
/*  2008-01-14  P04.00.00.00_00.70.01.02 lrg  CLP_LIMIT_LSA_INT32_MAX/MIN    */
/*  2008-06-20  P04.00.00.00_00.80.05.02 lrg: GSY_MESSAGE                    */
/*  2008-12-04  P04.01.00.00_00.02.03.01 lrg: Package Version (PNIO)         */
/*  2008-12-04  P04.01.00.00_00.03.01.02 lrg: CLP_SYNCID_MAX                 */
/*              todo: gleicher Regler fuer Clock und Time?                   */
/*  2010-07-16  P05.00.00.00_00.01.06.02 lrg: AP00697597                     */
/*              German comments translated to english                        */
/*                                                                           */
/*****************************************************************************/
#endif /* GSY_MESSAGE */

#define CLP_LIMIT_LSA_INT32_MAX 0x7FFFFFFFL
#define CLP_LIMIT_LSA_INT32_MIN ((LSA_INT)(-1L * CLP_LIMIT_LSA_INT32_MAX))

#define CLP_SYNCID_MAX	1	// GSY_SYNC_ID_TIME

/* Controller data union for supported SyncIds (0 = Clock, 1 = Time)
 * Pointer to this union is passed from system adaption to
 * clp_SyncAlgorithm() and clp_SyncReset
*/
typedef struct _CLP_SYNC0_CONTROLLER_TYPE
{
  LSA_INT32        Kp_1perSec;           // Kp in 1/sec
  LSA_INT32        Ki_1perSecSec;        // Ki in 1/sec^2
  LSA_INT32        PhaseError_ns;        // PhaseError in ns (Input_Value of PI)
  LSA_INT32        LastPhaseError_ns;    // State variable (Zustandsgroesse)

#ifdef CLP_CALC64
  CLP_INT64        ManipVar_ppt;         // actuating variable (Stellgroesse: Steigung der Gegendrift)
                                         // Entity: scalar, ppt (parts per trillion = 10^-12) 
  CLP_UINT64       AdjustDividend;       // Divident needed to calculate AdjustInterval
#else
  LSA_INT64        ManipVar_ppt;         // actuating variable (Stellgroesse: Steigung der Gegendrift)
                                         // Entity: scalar, ppt (parts per trillion = 10^-12) 
#endif //CLP_CALC64

  LSA_INT32        DeltaTime_ms;         
  LSA_UINT32       AdjustIntervalNs;     // HelperValue
  LSA_UINT32       State;                // State of Sync              
  LSA_UINT32       LastSec;              // Time of last offset
  LSA_UINT32       LastNs;               // Time of last offset

} CLP_SYNC0_CONTROLLER_TYPE;

typedef union _CLP_SYNC_CONTROLLER_TYPE
{
  CLP_SYNC0_CONTROLLER_TYPE	ClockCtrl;
} CLP_SYNC_CONTROLLER_TYPE;

/*****************************************************************************/
/*  prototypes                                                               */
/*****************************************************************************/

LSA_UINT16 CLP_SYSTEM_IN_FCT_ATTR clp_SyncAlgorithm(
LSA_UINT32                    DeltaSeconds,
LSA_UINT32                    DeltaNanoSeconds,
LSA_INT32                     PhaseError_ns,
LSA_INT32                     CLP_USR_MEM_ATTR *pAdjustInterval,
LSA_INT64                     CLP_USR_MEM_ATTR *pProportionalTerm,
LSA_INT64                     CLP_USR_MEM_ATTR *pIntegralTerm,
LSA_UINT8                     SyncId,
CLP_SYNC_CONTROLLER_TYPE      CLP_SYS_MEM_ATTR *pCtrl);

LSA_UINT16 CLP_SYSTEM_IN_FCT_ATTR clp_SyncReset(
LSA_INT32                     RelativeDrift_Ppt,
LSA_UINT8                     SyncId,
CLP_SYNC_CONTROLLER_TYPE      CLP_SYS_MEM_ATTR *pCtrl);

/*****************************************************************************/
/*  end of file clp_sys.h                                                    */
/*****************************************************************************/

#ifdef __cplusplus  /* If C++ - compiler: End of C linkage */
}
#endif

/*---------------------------------------------------------------------------*/
#endif  /* of CLP_SYS_H */

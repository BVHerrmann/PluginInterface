#ifndef EDDI_SER_CMD_H          //reinclude-protection
#define EDDI_SER_CMD_H

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
/*  F i l e               &F: eddi_ser_cmd.h                            :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*****************************************************************************/

/*                                                                           */
/*****************************************************************************/

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERCommand( LSA_UINT16                 const FCode,
                                              LSA_UINT32                 const PrimPara,
                                              LSA_UINT32                 const SecPara,
                                              EDDI_LOCAL_MEM_U32_PTR_TYPE const pResult,
                                              EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSingleCommand( LSA_UINT16                 const FCode,
                                                    LSA_UINT32                 const PrimPara,
                                                    EDDI_LOCAL_MEM_U32_PTR_TYPE const pResult,
                                                    EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERAsyncCmd( LSA_UINT16              const FCode,
                                               LSA_UINT32              const PrimPara,
                                               LSA_UINT32              const SecPara,
                                               EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                               EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                               LSA_BOOL                const bLock );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERAsyncConf( EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSheduledRequest( EDDI_LOCAL_DDB_PTR_TYPE const pDDB,
                                                      EDD_UPPER_RQB_PTR_TYPE  const pRQB,
                                                      LSA_UINT32              const eddi_req_fct );

LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SERSingleDirectCmd( LSA_UINT16                  const FCode,
                                                      LSA_UINT32                  const PrimPara,
                                                      EDDI_LOCAL_MEM_U32_PTR_TYPE const pResult,
                                                      EDDI_LOCAL_DDB_PTR_TYPE     const pDDB );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SER_CMD_H


/*****************************************************************************/
/*  end of file eddi_ser_cmd.h                                               */
/*****************************************************************************/

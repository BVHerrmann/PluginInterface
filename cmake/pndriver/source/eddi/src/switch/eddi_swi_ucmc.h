#ifndef EDDI_SWI_UCMC_H         //reinclude-protection
#define EDDI_SWI_UCMC_H

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
/*  F i l e               &F: eddi_swi_ucmc.h                           :F&  */
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
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBPrio( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                     LSA_UINT8                const  FDBPrio,
                                                     LSA_UINT16                   *  pRQBPrio );

LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcSetFDBPrio( EDDI_LOCAL_DDB_PTR_TYPE   const  pDDB, 
                                                       LSA_UINT16                const  RQBPrio,
                                                       LSA_UINT8                     *  pFDBPrio );

LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBAdress( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pFDBEntry,
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE         pRQBFDBAdress );

LSA_UINT32 EDDI_LOCAL_FCT_ATTR EDDI_SwiUcCalcAdr( EDD_MAC_ADR_TYPE EDDI_LOCAL_MEM_ATTR const * const  pMAC,
                                                  LSA_UINT32                                   const  Mask,
                                                  LSA_UINT32                                   const  TableLength );

#ifdef __cplusplus //If C++ - compiler: End of C linkage
}
#endif

#endif //EDDI_SWI_UCMC_H


/*****************************************************************************/
/*  end of file eddi_swi_ucmc.h                                              */
/*****************************************************************************/

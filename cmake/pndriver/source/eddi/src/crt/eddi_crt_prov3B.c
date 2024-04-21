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
/*  F i l e               &F: eddi_crt_prov3B.c                         :F&  */
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
/*                                                            H i s t o r y :*/
/*   ________________________________________________________________________*/
/*                                                                           */
/*                                                     Date        Who   What*/
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_CRT_PROV_3B
#define LTRC_ACT_MODUL_ID  106

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if defined (M_ID_CRT_PROV_3B) //satisfy lint!
#endif

#if !defined (EDDI_INTCFG_PROV_BUFFER_IF_SINGLE) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)

#include "eddi_dev.h"
#include "eddi_crt_ext.h"
#include "eddi_crt_prov3B.h"
#include "eddi_ser_cmd.h"

//temporarily_disabled_lint -save -esym(754, _SER_GET_OUTPUT_BUF_RESULT_*)
#if !defined (EDDI_INTCFG_PROV_BUFFER_IF_3BSW)
#define SER_GET_OUTPUT_BUF_RESULT_BIT__OutputDataBuffer                      EDDI_BIT_MASK_PARA(20, 0)
#define SER_GET_OUTPUT_BUF_RESULT_BIT__Reserved                              EDDI_BIT_MASK_PARA(31,21)

typedef struct _SER_GET_OUTPUT_BUF_RESULT_TYPE
{
    LSA_UINT32                     Value;

} SER_GET_OUTPUT_BUF_RESULT_TYPE;
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_CRTProvInit3B()                             */
/*                                                                         */
/* D e s c r i p t i o n: not used for FCW-Provider and AUX-Provider!      */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CRTProvInit3B( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB, 
                                                   EDDI_CRT_PROVIDER_PTR_TYPE  const  pProvider)
{
    LSA_UINT32  length = pProvider->LowerParams.DataLen + sizeof(EDDI_CRT_DATA_APDU_STATUS);
    LSA_BOOL    const bSysRed = (pProvider->Properties & EDD_CSRT_PROV_PROP_SYSRED)?LSA_TRUE:LSA_FALSE;
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_CRTProvInit3B");

    // Preserve 4 Byte-Alignment
    length = (length + 0x03) & ~0x00000003;

    EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer,            pProvider->LocalDataStatus, (LSA_UINT8)0, bSysRed);
    EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer + length,   pProvider->LocalDataStatus, (LSA_UINT8)0, bSysRed);
    EDDI_CRTCompResetAPDUStatus(pDDB, pProvider->LowerParams.pKRAMDataBuffer + 2*length, pProvider->LocalDataStatus, (LSA_UINT8)0, bSysRed);
}
/*---------------------- end [subroutine] ---------------------------------*/

//temporarily_disabled_lint -restore

#endif //!defined (EDDI_INTCFG_PROV_BUFFER_IF_SINGLE) || defined (EDDI_XRT_OVER_UDP_SOFTWARE)

/*****************************************************************************/
/*  end of file eddi_crt_prov3B.c                                            */
/*****************************************************************************/

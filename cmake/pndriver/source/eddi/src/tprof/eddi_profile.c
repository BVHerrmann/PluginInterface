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
/*  F i l e               &F: eddi_profile.c                            :F&  */
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
/*  Date        Who   What                                                   */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"

#define EDDI_MODULE_ID     M_ID_PROFILE
#define LTRC_ACT_MODUL_ID  27

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#include "eddi_dev.h"

#include "eddi_profile.h"

#if defined (PROF_KRAM_OFFSET_SER_CMD_SIZE)
//to avoid a lint-warning 755. PROF_KRAM_OFFSET_SER_CMD_SIZE is only used when generating RAMVIEW
#endif


/***************************************************************************/
/* F u n c t i o n:       EDDI_PROFStop()                                  */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PROFStop( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PROFStop->");

    pDDB->pProfKRamInfo->mode = PROF_MODE_NOT_ACTIVATED;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_PROFIni()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_PROFIni( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_UINT32  size = 0;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_PROFIni->");

    //new KRam structure
    size = sizeof(PROF_KRAM_INFO);

    EDDI_MemCopy(&pDDB->pProfKRamInfo->offset, &size, (LSA_UINT32)sizeof(LSA_UINT32));

    pDDB->pProfKRamInfo->SerCmdSize = PROF_KRAM_SIZE_SERCMD;

    pDDB->pProfKRamInfo->mode     = PROF_MODE_NOT_ACTIVATED;
    pDDB->pProfKRamInfo->ram_type = (LSA_UINT32)MEMORY_SDRAM_ERTEC;

    size = 0;
    EDDI_MemCopy(&pDDB->pProfKRamInfo->offset, &size, (LSA_UINT32)sizeof(LSA_UINT32));

    return;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_profile.c                                               */
/*****************************************************************************/

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
/*  F i l e               &F: eddi_csrt_usr.c                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  SRT (soft real time) for EDDI.                   */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  04.02.04    AZ    Umbau neue CRT-SS                                      */
/*                    - Unbenutzte Funktion SRTRequest wurde entfernt        */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"

#include "eddi_csrt_ext.h"
#include "eddi_ser_ext.h"

#define EDDI_MODULE_ID     M_ID_CSRT_USR
#define LTRC_ACT_MODUL_ID  123

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/*===========================================================================*/
/*                                 functions                                 */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_SRTInitComponent()                     +*/
/*+  Input/Output          :                                                +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+                                                                         +*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pRQB       : Pointer to Upper-RQB with Requestparameters               +*/
/*+                                                                         +*/
/*+  RQB-parameters:                                                        +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   Opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user (not used)                      +*/
/*+   LSA_COMP_ID_TYPE  CompId:  Cmponent-ID                                +*/
/*+   EDD_SERVICE    Service: EDDI_SRV_COMP_SRT_INI                         +*/
/*+   LSA_RESULT    Status:     Return status                               +*/
/*+   EDD_UPPER_MEM_PTR_TYPE pParam:  Depend on kind of request.            +*/
/*+                                                                         +*/
/*+  pParam points to EDDI_RQB_CMP_SRT_INI_TYPE                             +*/
/*+                                                                         +*/
/*+  hDDB               : Valid DDB-Handle                                  +*/
/*+  Cbf                : optional Callbackfunction                         +*/
/*+                                                                         +*/
/*+  RQB-return values via callback-function or on return.                  +*/
/*+   RQB-header:                                                           +*/
/*+   LSA_OPCODE_TYPE   opcode:  EDD_OPC_SYSTEM                             +*/
/*+   LSA_HANDLE_TYPE   Handle:                                             +*/
/*+   LSA_USER_ID_TYPE  UserId:  ID of user                                 +*/
/*+   LSA_COMP_ID_TYPE  CompId:                                             +*/
/*+   EDD_SERVICE    Service:    EDDI_SRV_COMP_SRT_INI                      +*/
/*+   LSA_RESULT    Status:      EDD_STS_OK                                 +*/
/*+                              EDD_STS_ERR_SEQUENCE                       +*/
/*+                              EDDI_STS_ERR_RESOURCES                     +*/
/*+                              EDD_STS_ERR_PARAM                          +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: This function will initialize the SRT-Component management+*/
/*+               structure for the device spezified in RQB. The structure  +*/
/*+               will be allocated and put into the DDB structure.         +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_SEQUENCE is returned, if the component is     +*/
/*+               already initialized for this device.                      +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM is returned if DDB handle is invalid    +*/
/*+               or pParam is NULL                                         +*/
/*+                                                                         +*/
/*+               The confirmation is done by calling the callback fct if   +*/
/*+               present. If not the status can also be read after function+*/
/*+               return.                                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SRTInitComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_LOCAL_DDB_COMP_SRT_PTR_TYPE  pSRTComp;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SRTInitComponent->");

    if (!(pDDB->pLocal_SRT == EDDI_NULL_PTR))
    {
        EDDI_Excp("EDDI_SRTInitComponent", EDDI_FATAL_ERR_EXCP, pDDB->pLocal_SRT, 0);
        return;
    }

    pSRTComp = &pDDB->SRT;

    //init with 0
    //EDDI_MemSet(pSRTComp, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_COMP_SRT_TYPE));

    pDDB->pLocal_SRT = pSRTComp;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:                                                        */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SRTRelComponent( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SRTRelComponent->");

    if (pDDB->pLocal_SRT == EDDI_NULL_PTR)
    {
        //EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SRTRelComponent ??? Pointer to SRT-Struct is already Null");
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*---------------------------------------------------------------------------*/
/* SRT-Provider-Consumer-Functions                                           */
/*---------------------------------------------------------------------------*/


/*=============================================================================
 * function name: EDDI_SRTConsumerAdd()
 *
 * function:      create structs für IRT-Communication
 *
 * parameters:    
 *
 * return value:  LSA_VOID
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SRTConsumerAdd( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                    EDDI_CRT_CONSUMER_PTR_TYPE  const  pConsumer )
{
    EDDI_SER10_CCW_TYPE          CCWLocal;
    EDDI_CCW_CTRL_PTR_TYPE       pACWCtrl;
    LSA_UINT32                   Index;
    EDDI_CCW_CTRL_HEAD_PTR_TYPE  pACWHead;

    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SRTConsumerAdd->");

    pACWCtrl = EDDI_NULL_PTR;

    //ini ACW
    EDDI_SERIniConsumerACW(&CCWLocal, &pConsumer->LowerParams, pDDB);

    Index    = pConsumer->LowerParams.FrameId & pDDB->SRT.ACWRxMask; // calculate ACWRxList-Header.
    pACWHead = &pDDB->SRT.pACWRxHeadCtrl[Index];

    //add ACW
    EDDI_SERCcwAdd(&CCWLocal, pACWHead, &pACWCtrl, pDDB);

    //set the return Value of Lower CCW-Stucture
    pConsumer->pLowerCtrlACW = pACWCtrl;

    pDDB->pLocal_CRT->ConsumerList.UsedACWs++;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_SRTConsumerRemove()
 *
 * function:      
 *
 * parameters:    pKRAMSmallApduBuffer: pointer to a APDU-Buffer or EDDI_NULL_PTR
 *
 * return value:  LSA_VOID
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SRTConsumerRemove( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                       EDDI_CCW_CTRL_PTR_TYPE      const  pCtrlACW,
                                                       LSA_VOID                 *  const  pKRAMSmallApduBuffer )
{
    EDDI_CRT_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SRTConsumerRemove->");

    if (pCtrlACW == EDDI_NULL_PTR)
    {
        EDDI_Excp("EDDI_SRTConsumerRemove", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    EDDI_SERCcwRemove(pDDB, pCtrlACW, pKRAMSmallApduBuffer);

    if (0 == pDDB->pLocal_CRT->ConsumerList.UsedACWs)
    {
        EDDI_CRT_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SRTConsumerRemove, UsedACWs=0, UsedEntries123:%i", 
                          pDDB->pLocal_CRT->ConsumerList.UsedEntriesRTC123);
        EDDI_Excp("EDDI_SRTConsumerRemove, UsedACWs=0", EDDI_FATAL_ERR_EXCP, pDDB->pLocal_CRT->ConsumerList.UsedEntriesRTC123, 0);
    }
    
    pDDB->pLocal_CRT->ConsumerList.UsedACWs--;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_csrt_usr.c                                              */
/*****************************************************************************/




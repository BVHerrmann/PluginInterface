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
/*  F i l e               &F: eddi_glb.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-global functions                             */
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

#include "eddi_dev.h"

//#include "eddi_profile.h"
#include "eddi_req.h"

#define EDDI_MODULE_ID     M_ID_EDDI_GLB
#define LTRC_ACT_MODUL_ID  5

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_FatalError()                           +*/
/*+  Input/Output               EDDI_FATAL_ERROR_TYPE Error                 +*/
/*+  Input/Output          :    LSA_UINT16            ModuleID              +*/
/*+                             LSA_UINT16            Line                  +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Error                : EDD-fatal-errorcode                             +*/
/*+  ModuleID             : module - id of error                            +*/
/*+  Line                 : line of code (optional)                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Signals Fatal-error via EDDI_FATAL_ERROR macro.           +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FatalError( LSA_UINT32     const  Line,
                                                LSA_UINT8   *  const  sFile,
                                                LSA_UINT32     const  ModuleID,
                                                LSA_UINT8   *  const  sErr,
                                                LSA_UINT32     const  Error,
                                                LSA_UINT32     const  DW_0,
                                                LSA_UINT32     const  DW_1 )
{
    LSA_FATAL_ERROR_TYPE     LSAError;
    LSA_UINT8             *  p;
    LSA_UINT32               Ctr, start;

    if (g_pEDDI_Info ->Excp.Error != EDDI_FATAL_NO_ERROR)
    {
        return;
    }

    g_pEDDI_Info ->Excp.Error    = Error;
    g_pEDDI_Info ->Excp.Line     = Line;
    g_pEDDI_Info ->Excp.ModuleID = ModuleID;

    EDDI_PROGRAM_TRACE_STRING(0, LSA_TRACE_LEVEL_FATAL, "EDDI_FatalError->%s", sErr);
    EDDI_PROGRAM_TRACE_05(0, LSA_TRACE_LEVEL_FATAL, "EDDI_FatalError->ModuleID:0x%X Line:%d Param0:0x%X Param1:0x%X Error:0x%X", ModuleID, Line, DW_0, DW_1, Error);
    
    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        EDDI_DDB_TYPE  *  const  pDDB = &g_pEDDI_Info ->DDB[Ctr];

        if (pDDB->bUsed  == EDDI_DDB_NOT_USED)
        {
            continue;
        }

        if (pDDB->pProfKRamInfo)
        {
            pDDB->pProfKRamInfo->Excp.Error    = Error;
            pDDB->pProfKRamInfo->Excp.Line     = Line;
            pDDB->pProfKRamInfo->Excp.ModuleID = ModuleID;
            pDDB->pProfKRamInfo->Excp.DW_0     = DW_0;
            pDDB->pProfKRamInfo->Excp.DW_1     = DW_1;
        }

        //EDDI_PROGRAM_TRACE_05(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_FatalError->ModuleID:0x%X Line:%d Param0:0x%X Param1:0x%X Error:0x%X", ModuleID, Line, DW_0, DW_1, Error);

        EDDI_SIISetDummyIsr(pDDB);
    }

    LSAError.lsa_component_id  = LSA_COMP_ID_EDD_ERTEC;
    LSAError.module_id         = (LSA_UINT16)ModuleID;
    LSAError.line              = (LSA_UINT16)Line;
    LSAError.error_code[0]     = Error;
    LSAError.error_code[1]     = EDDI_MODULE_ID;
    LSAError.error_code[2]     = DW_0;
    LSAError.error_code[3]     = DW_1;
    LSAError.error_data_length = EDDI_MAX_EXCP_INFO;

    EDDI_MemSet(g_pEDDI_Info ->Excp.sInfo, (LSA_UINT8)0, (LSA_UINT32)EDDI_MAX_EXCP_INFO);

    p = sFile;

    if (p)
    {
        start = 0;
        for (Ctr = 0; Ctr < 256; Ctr++)
        {
            if (*p == '\\')
            {
                start = 0;
                p++;
                continue;
            }

            if ((*p == 0) || (start == (EDDI_MAX_EXCP_INFO - 5)))
            {
                g_pEDDI_Info ->Excp.sInfo[start] = ' ';
                start++;
                g_pEDDI_Info ->Excp.sInfo[start] = '=';
                start++;
                g_pEDDI_Info ->Excp.sInfo[start] = '>';
                start++;
                g_pEDDI_Info ->Excp.sInfo[start] = ' ';
                start++;
                break;
            }

            g_pEDDI_Info ->Excp.sInfo[start] = *p;
            p++;
            start++;
        }

        if (sErr)
        {
            p = sErr;
        }
        else
        {
            p = (LSA_UINT8 *)(void *)"No Message specified!";
        }

        for (Ctr = start; Ctr < (EDDI_MAX_EXCP_INFO - 1); Ctr++)
        {
            if (*p == 0)
            {
                break;
            }

            g_pEDDI_Info ->Excp.sInfo[Ctr] = *p;
            p++;
        }
    }
    else //EDDI_CFG_NO_FATAL_FILE_INFO
    {
        start = 0;
        g_pEDDI_Info ->Excp.sInfo[start] = 'N';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'o';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'F';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'i';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'l';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'e';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'I';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'n';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'f';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = 'o';
        start++;

        g_pEDDI_Info ->Excp.sInfo[start] = ' ';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = '=';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = '>';
        start++;
        g_pEDDI_Info ->Excp.sInfo[start] = ' ';
        start++;
    }

    LSAError.error_data_ptr = g_pEDDI_Info ->Excp.sInfo;

    EDDI_StopUser();
    
    EDDI_FATAL_ERROR((LSA_UINT16)sizeof(LSA_FATAL_ERROR_TYPE), &LSAError);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_RequestFinish()                        +*/
/*+  Input/Output               EDDI_LOCAL_HDB_PTR_TYPE     pHDB            +*/
/*+                             EDD_UPPER_RQB_PTR_TYPE      pRQB            +*/
/*+                             LSA_RESULT                  Status          +*/
/*+                                                                         +*/
/*+  Result                :    LSA_VOID                                    +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHDB                 : Pointer to HDB                                  +*/
/*+  pRQB                 : Pointer to RQB                                  +*/
/*+  Status               : Status to set in RQB                            +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Finishs Request by calling call-back-function located in  +*/
/*+               HDB. if Cbf is LSA_NULL nothing is done.                  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_RequestFinish( EDDI_LOCAL_HDB_PTR_TYPE  const  pHDB,
                                                   EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                   LSA_RESULT               const  Status )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = pHDB->pDDB;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RequestFinish->");

    EDD_RQB_SET_RESPONSE(pRQB, Status);
    EDD_RQB_SET_HANDLE(pRQB, pHDB->UpperHandle);

    EDDI_REQUEST_UPPER_DONE(pHDB->Cbf, pRQB, pHDB->pSys);
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_QueueAddToEnd()
 *
 * function:      Adds an Element to the end of the Queue
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_QueueAddToEnd( EDDI_QUEUE_PTR_TYPE          const  pQueue,
                                                   EDDI_QUEUE_ELEMENT_PTR_TYPE  const  pNewElement )
{
    if (pQueue->Count == 0)
    {
        //First Element in List --> init pBegin
        pQueue->pFirst        = pNewElement;
        pNewElement->prev_ptr = EDDI_NULL_PTR;
    }
    else
    {
        pQueue->pLast->next_ptr = pNewElement;
        pNewElement->prev_ptr   = pQueue->pLast;
    }
    pNewElement->next_ptr = EDDI_NULL_PTR;
    //Update pEnd and Counter
    pQueue->pLast = pNewElement;
    pQueue->Count++;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_QueueRemoveFromEnd()
 *
 * function:      Removes an Element from the end of the Queue
 *
 *===========================================================================*/
EDDI_QUEUE_ELEMENT_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_QueueRemoveFromEnd( EDDI_QUEUE_PTR_TYPE  const  pQueue )
{
    EDDI_QUEUE_ELEMENT_PTR_TYPE  pRemElement;

    if (pQueue->Count == 0)
    {
        return EDDI_NULL_PTR;
    }

    pRemElement = pQueue->pLast;

    if (pQueue->Count == 1)
    {
        //Last Element in List --> also update pBegin
        pQueue->pFirst  = EDDI_NULL_PTR;
        pQueue->pLast   = EDDI_NULL_PTR;
    }
    else
    {
        //Update pEnd
        pQueue->pLast           = pQueue->pLast->prev_ptr;
        pQueue->pLast->next_ptr = EDDI_NULL_PTR;
    }

    pQueue->Count --;

    pRemElement->next_ptr = EDDI_NULL_PTR;
    pRemElement->prev_ptr = EDDI_NULL_PTR;

    return pRemElement;
}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV5) || defined (EDDI_CFG_REV7)
/*=============================================================================
 * function name: EDDIQueueAddToBegin()
 *
 * function:      Adds an Element to the Begin of the Queue
 *
 *===========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDIQueueAddToBegin( EDDI_QUEUE_PTR_TYPE          const  pQueue,
                                                    EDDI_QUEUE_ELEMENT_PTR_TYPE  const  pNewElement )
{
    if (pQueue->Count == 0)
    {
        //First Element in List --> init pBegin
        pQueue->pLast         = pNewElement;
        pNewElement->next_ptr = EDDI_NULL_PTR;
    }
    else
    {
        pQueue->pFirst->prev_ptr = pNewElement;
        pNewElement->next_ptr    = pQueue->pFirst;
    }
    pNewElement->prev_ptr = EDDI_NULL_PTR;
    //Update pEnd and Counter
    pQueue->pFirst = pNewElement;
    pQueue->Count++;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif


/*=============================================================================
 * function name: EDDI_QueueRemoveFromBegin()
 *
 * function:      Removes an Element from the End of the Queue
 *
 *===========================================================================*/
EDDI_QUEUE_ELEMENT_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_QueueRemoveFromBegin( EDDI_QUEUE_PTR_TYPE  const  pQueue )
{
    EDDI_QUEUE_ELEMENT_PTR_TYPE  pRemElement;

    if (pQueue->Count == 0)
    {
        return EDDI_NULL_PTR;
    }

    pRemElement = pQueue->pFirst;

    if (pQueue->Count == 1)
    {
        //Last Element in List --> also update pBegin
        pQueue->pFirst = EDDI_NULL_PTR;
        pQueue->pLast  = EDDI_NULL_PTR;
    }
    else
    {
        //Update pBegin
        pQueue->pFirst           = pQueue->pFirst->next_ptr;
        pQueue->pFirst->prev_ptr = EDDI_NULL_PTR;
    }

    pQueue->Count --;

    pRemElement->next_ptr = EDDI_NULL_PTR;
    pRemElement->prev_ptr = EDDI_NULL_PTR;

    return pRemElement;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
 * function name: EDDI_QueueRemove()
 *
 * function:      Removes an Element from the End of the Queue
 *
 *===========================================================================*/
LSA_VOID EDDI_LOCAL_FCT_ATTR  EDDI_QueueRemove( EDDI_QUEUE_PTR_TYPE         const  pQueue,
                                                EDDI_QUEUE_ELEMENT_PTR_TYPE const  pElement)
{
    if (0 == pQueue->Count)
    {
        EDDI_PROGRAM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "EDDI_QueueRemove: Queue empty, pQueue:0x%X, pFirst:0x%X, pLast:0x%X", 
            pQueue, pQueue->pFirst, pQueue->pLast);
        EDDI_Excp("EDDI_QueueRemove: Queue empty", EDDI_FATAL_ERR_EXCP, pElement, pQueue);
        return;
    }

    if  //first element
        (pQueue->pFirst == pElement)
    {
        pQueue->pFirst = pElement->next_ptr;
        if (1 == pQueue->Count)
        {
            pQueue->pLast = EDDI_NULL_PTR;
        }
        else
        {
            pQueue->pFirst->prev_ptr = EDDI_NULL_PTR;
        }
    }
    else if //last element
        (pQueue->pLast == pElement)
    {
        pQueue->pLast = pElement->prev_ptr;
        pElement->prev_ptr->next_ptr = EDDI_NULL_PTR;
    }
    else
    {
        //Element.prev->next = Element.next
        pElement->prev_ptr->next_ptr = pElement->next_ptr;

        //Element->next.prev = Element.prev
        pElement->next_ptr->prev_ptr = pElement->prev_ptr;
    }

    pQueue->Count--;

    pElement->next_ptr = EDDI_NULL_PTR;
    pElement->prev_ptr = EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_QueueGetNext()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:                                                           */
/*                                                                         */
/***************************************************************************/
EDDI_QUEUE_ELEMENT_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_QueueGetNext( EDDI_QUEUE_PTR_TYPE          const  pQueue,
                                                                     EDDI_QUEUE_ELEMENT_PTR_TYPE  const  pElement )
{
    if (pElement)
    {
        return (pElement->next_ptr);
    }
    else
    {
        return (pQueue->pFirst);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_AddToQueueEnd()                             */
/*                                                                         */
/* D e s c r i p t i o n: puts a RQB to the end of a simple queue          */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_AddToQueueEnd( EDDI_LOCAL_DDB_PTR_TYPE    const  pDDB,
                                                   EDDI_RQB_QUEUE_TYPE     *  const  pQueue,
                                                   EDD_UPPER_RQB_PTR_TYPE     const  pRQB )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_AddToQueueEnd->");

    EDD_RQB_SET_NEXT_RQB_PTR(pRQB, EDDI_NULL_PTR);

    if //queue empty?
       (pQueue->pTop == EDDI_NULL_PTR)
    {
        pQueue->pTop = pRQB;
        pQueue->Cnt  = 1UL;
    }
    else //queue not empty
    {
        EDD_RQB_SET_NEXT_RQB_PTR(pQueue->pBottom, pRQB);
        pQueue->Cnt++;
    }

    pQueue->pBottom = pRQB;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_RemoveFromQueue()                           */
/*                                                                         */
/* D e s c r i p t i o n: removes a RQB from the start of a simple queue   */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          EDD_UPPER_RQB_PTR_TYPE                           */
/*                                                                         */
/***************************************************************************/
EDD_UPPER_RQB_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_RemoveFromQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                                                   EDDI_RQB_QUEUE_TYPE      *  const  pQueue )
{
    EDD_UPPER_RQB_PTR_TYPE  const  pRQB = pQueue->pTop;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_RemoveFromQueue->");

    if //RQB available?
       (pRQB)
    {
        //remove RQB
        pQueue->pTop = (EDD_UPPER_RQB_PTR_TYPE)EDD_RQB_GET_NEXT_RQB_PTR(pRQB);
        pQueue->Cnt--;
    }

    LSA_UNUSED_ARG(pDDB);
    return pRQB;
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
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_InitQueue( EDDI_LOCAL_DDB_PTR_TYPE     const  pDDB,
                                               EDDI_RQB_QUEUE_TYPE      *  const  pQueue )
{
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_InitQueue->");

    pQueue->pTop    = EDDI_NULL_PTR;
    pQueue->pBottom = EDDI_NULL_PTR;
    pQueue->Cnt     = 0;
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_glb.c                                                   */
/*****************************************************************************/


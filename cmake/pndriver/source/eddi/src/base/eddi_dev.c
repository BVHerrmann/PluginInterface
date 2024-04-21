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
/*  F i l e               &F: eddi_dev.c                                :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  EDD-Device and handle-management functions       */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  H i s t o r y :                                                          */
/*  ________________________________________________________________________ */
/*                                                                           */
/*  Date        Who   What                                                   */
/*  18.02.04    ZR    add icc modul                                          */
/*                                                                           */
/*****************************************************************************/

#include "eddi_int.h"
#include "eddi_dev.h"
#include "eddi_ext.h"

#if defined (EDDI_CFG_USE_SW_RPS)
#include "eddi_crt_ext.h"
#endif

#define EDDI_MODULE_ID     M_ID_EDDI_DEV
#define LTRC_ACT_MODUL_ID  2

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

LSA_UINT32  g_LocalMem = 0;


/*===========================================================================*/
/*                                global Variables                           */
/*===========================================================================*/
//static EDDI_LOCAL_MEM_ATTR  EDDI_HDB_TYPE  g_EDDIHDB[EDDI_CFG_MAX_CHANNELS];


/*===========================================================================*/
/*                                 Macros                                    */
/*===========================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :   EDDI_HandleNew()                             +*/
/*+  Input/Output          :   LSA_HANDLE_TYPE EDDI_LOCAL_MEM_ATTR * pHandle+*/
/*+                            EDDI_LOCAL_HDB_PTR_TYPE             * ppHDB  +*/
/*+  Result                :   LSA_RESULT                                   +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pHandle      Pointer to address for LSA_HANDLE which will be returned. +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Creates new EDD channel Handle                            +*/
/*+               On success an LSA_HANDLE and a pointer to the HDB will be +*/
/*+               returned.                                                 +*/
/*+               InUse and Handle is setup, the rest is initialized with 0 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_HandleNew( LSA_HANDLE_TYPE  EDDI_LOCAL_MEM_ATTR      *  pHandle,
                                                 EDDI_LOCAL_HDB_PTR_TYPE                   *  ppHDB,
                                                 EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB )
{
    EDDI_LOCAL_HDB_PTR_TYPE     pHDB;
    LSA_UINT32                  RefCnt;
    LSA_HANDLE_TYPE             Handle;

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_HandleNew->");

    //Get free HDB (if any)        
    pHDB = (EDDI_LOCAL_HDB_PTR_TYPE)(void *)EDDI_QueueRemoveFromEnd(&g_pEDDI_Info ->FreeHDBQueue);
    if (EDDI_NULL_PTR == pHDB)
    {
        return EDD_STS_ERR_RESOURCE;
    }

    //check HDB
    if (pHDB->InUse)
    {
        EDDI_Excp("EDDI_HandleNew, HDB queue destroyed", EDDI_FATAL_ERR_INVALID_HANDLE, pHDB, pHDB->pDDB);
        return EDD_STS_ERR_RESOURCE;
    }

    //preset HDB
    RefCnt = pHDB->RefCnt;  //save vars before clearing HDB
    Handle = pHDB->Handle;

    EDDI_MemSet(pHDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_HDB_TYPE));
    EDDI_QueueAddToEnd(&pDDB->HDBQueue, &pHDB->QueueLink);  //add to "used" queue in DDB

    pHDB->InUse                  = 1;
    pDDB->Glob.OpenCount++;
    pHDB->pDDB                   = pDDB;
    pHDB->RefCnt                 = RefCnt + 1;
    pHDB->RxOverLoad.cIndication = 0;
    pHDB->Handle                 = Handle;
    
    //set return values
    *pHandle = Handle;
    *ppHDB   = pHDB;

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_HandleGetHDB()                         +*/
/*+  Input/Output          :    ...                                         +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+  ppHDB        Pointer to address for handle-management which will be    +*/
/*+               returned.                                                 +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_EXCP                                          +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets pointer to handle-managment entry (HDB) for handle.  +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_HandleGetHDB( EDD_HANDLE_LOWER_TYPE                            const  Handle,
                                                    EDDI_LOCAL_HDB_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  ppHDB )
{
    EDDI_HDB_TYPE          *  pHDB = (EDDI_LOCAL_HDB_PTR_TYPE) Handle;
    EDDI_DDB_TYPE          *  pDDB;

    EDDI_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "EDDI_HandleGetHDB->");

    if (LSA_HOST_PTR_ARE_EQUAL(pHDB, LSA_NULL))
    {
        EDDI_Excp("EDDI_HandleGetHDB, requested Handle invalid", EDDI_FATAL_ERR_INVALID_HANDLE, Handle, 0);
        //invalid handle
        return EDD_STS_ERR_RESOURCE;
    }

    if ( (pHDB->InUse == 1UL) && (pHDB->Handle < EDDI_CFG_MAX_CHANNELS) )
    {            
        if (!LSA_HOST_PTR_ARE_EQUAL(pHDB, &g_pEDDI_Info->HDB[pHDB->Handle]))
        {
            EDDI_Excp("EDDI_HandleGetHDB, requested Handle is unknown", EDDI_FATAL_ERR_INVALID_HANDLE, Handle, 0);
            return EDD_STS_ERR_RESOURCE;
        }

        *ppHDB = pHDB;

        pDDB = pHDB->pDDB;
        if (pDDB->bUsed != EDDI_DDB_USED)
        {
            EDDI_Excp("EDDI_HandleGetHDB, requested Handle invalid, pDDB->bUsed == 0", EDDI_FATAL_ERR_INVALID_HANDLE, Handle, 0);
            //invalid handle
            return EDD_STS_ERR_RESOURCE;
        }
    }
    else
    {
        EDDI_Excp("EDDI_HandleGetHDB, requested Handle not found", EDDI_FATAL_ERR_INVALID_HANDLE, Handle, 0);

        //invalid handle
        return EDD_STS_ERR_RESOURCE;
    }

    return (EDD_STS_OK);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_HandleRel()                            +*/
/*+  Input/Output          :    LSA_HANDLE_TYPE  Handle                     +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  Handle       Channel Handle                                            +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_SEQUENCE                                      +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Frees channel handle.                                     +*/
/*+                                                                         +*/
/*+               If the handle is not in use, EDD_STS_ERR_SEQUENCE is      +*/
/*+               returned.                                                 +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_HandleRel( EDD_HANDLE_LOWER_TYPE  const  Handle )
{
    EDDI_LOCAL_HDB_PTR_TYPE  pHDB;
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;

    if (EDD_STS_OK != EDDI_HandleGetHDB(Handle, &pHDB))
    {
        EDDI_Excp("EDDI_HandleRel, requested Handle invalid", EDDI_FATAL_ERR_INVALID_HANDLE, Handle, 0);
        //invalid handle
        return EDD_STS_ERR_RESOURCE;
    }

    /* DDB is valid here, checked within EDDI_HandleGetHDB */
    pDDB = pHDB->pDDB;
   
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_HandleRel->");

    pDDB->Glob.OpenCount--;

    pHDB->InUse = 0;
    pHDB->RefCnt++;

    //remove from "used" queue in DDB
    EDDI_QueueRemove(&pDDB->HDBQueue, &pHDB->QueueLink);

    //add to "free" queue
    EDDI_QueueAddToEnd(&g_pEDDI_Info ->FreeHDBQueue, &pHDB->QueueLink);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CreateDDB()                            +*/
/*+  Input/Output          :    EDDI_LOCAL_DDB_PTR_TYPE  * ppDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_RESOURCE                                      +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Allocates an DDB structure and initializes it. The new DDB+*/
/*+               is chained into the global DDB-info structure and a       +*/
/*+               pointer to the new DDB is returned.                       +*/
/*+                                                                         +*/
/*+               EDDIDevInfoIni() has  to be called before using this funct.+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_RESOURCE is returned, if alloc fails or no    +*/
/*+               more devices supported.                                   +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_CreateDDB( EDDI_LOCAL_DDB_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  ppDDB,
                                                 LSA_UINT32                                       const  IRTE_SWI_BaseAdr )
{
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;
    LSA_UINT32               Ctr;

    //Check that not another DDB already references this device 
    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        pDDB = &g_pEDDI_Info ->DDB[Ctr];

        if (pDDB->bUsed == EDDI_DDB_USED)
        {
            if (pDDB->IRTE_SWI_BaseAdr == IRTE_SWI_BaseAdr)
            {
                return EDD_STS_ERR_SEQUENCE;
            }
        }
    }

    /*-----------------------------------------------------------------------*/
    //Find free DDB
    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        pDDB = &g_pEDDI_Info ->DDB[Ctr];

        if (pDDB->bUsed == EDDI_DDB_NOT_USED)
        {
            //When the DDB is freed, it is NULLed with EDDI_MemSet(pDDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_TYPE));

            pDDB->bUsed            = EDDI_DDB_USED;
            pDDB->DeviceNr         = Ctr;
            pDDB->InternalHDB.pDDB = pDDB;  //This InternalHDB is needed for internal Scheduled Requests.
                                            //(see SyncRsm_Callback --> RSM-Statemachine)

            g_pEDDI_Info ->CntDDB++;

            pDDB->hDDB = (EDDI_HANDLE)pDDB;

            pDDB->CmdIF.State = EDDI_CMD_IF_FREE;

            pDDB->CmdIF.AgeDummyRQB.Service = EDDI_SRV_AGE_DUMMY;
            EDD_RQB_SET_OPCODE(&pDDB->CmdIF.AgeDummyRQB, EDDI_OPC_AGE_DUMMY_UNUSED);

            #if defined (EDDI_CFG_USE_SW_RPS)
            pDDB->SW_RPS_CheckDataStatesEvent = EDDI_CRTRpsCheckDataStatesEventDummy;
            #endif

            //init HDB queue
            pDDB->HDBQueue.Count  = 0;
            pDDB->HDBQueue.pFirst = EDDI_NULL_PTR;
            pDDB->HDBQueue.pLast  = EDDI_NULL_PTR;

            *ppDDB = pDDB;
            
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CreateDDB->free DDB available");
            
            return EDD_STS_OK;
        }
    }

    return EDD_STS_ERR_RESOURCE;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_GetDDB()                               +*/
/*+  Input/Output          :    EDDI_HANDLE                hDDB             +*/
/*+                             EDDI_LOCAL_DDB_PTR_TYPE  * ppDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  hDDB         : Valid DDB-Handle                                        +*/
/*+  ppDDB        : Address for pointer to DDB which will be returned       +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+                                                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Gets the DDB associated with the handle.                  +*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM    is returned, if no DDB was found.    +*/
/*+                                                                         +*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_GetDDB( const EDDI_HANDLE                                const  hDDB,
                                              EDDI_LOCAL_DDB_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  const  ppDDB )
{
    LSA_UINT32  Ctr;

    if (!g_pEDDI_Info )
    {
        return EDD_STS_ERR_SEQUENCE;
    }

    for (Ctr = 0; Ctr < EDDI_CFG_MAX_DEVICES; Ctr++)
    {
        EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = &g_pEDDI_Info ->DDB[Ctr];

        if ((pDDB->bUsed == EDDI_DDB_USED) && (pDDB->hDDB == hDDB))
        {
            *ppDDB = pDDB;
            EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_GetDDB->");
            return EDD_STS_OK;
        }
    }

    return EDD_STS_ERR_PARAM;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*+                                                                         +*/
/*+  Functionname          :    EDDI_CloseDDB()                             +*/
/*+  Input/Output               EDDI_LOCAL_DDB_PTR_TYPE     pDDB            +*/
/*+  Result                :    LSA_RESULT                                  +*/
/*+-------------------------------------------------------------------------+*/
/*+  Input/Output:                                                          +*/
/*+                                                                         +*/
/*+  pDDB         : Pointer to DDB to be closed                             +*/
/*+                                                                         +*/
/*+  Result:      EDD_STS_OK                                                +*/
/*+               EDD_STS_ERR_PARAM                                         +*/
/*+-------------------------------------------------------------------------+*/
/*+  Description: Closes an DDB. The DDB is unqueued and the memory is freed+*/
/*+                                                                         +*/
/*+               EDD_STS_ERR_PARAM    is returned, if hDDB is invalid.     +*/
/*+                                                                         +*/
/*+               A fatal error is signaled, if no DDB is present.          +*/
/*+                                                                         +*/
/*+               Note: This function does not check for open handles for   +*/
/*+                     this device!!                                       +*/
/*+                     Does not check for open components!                 +*/
/*+                                                                         +*/
/*+               Important: No internal RQB must be in use !!! not checked.+*/
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_CloseDDB( const EDDI_HANDLE  const  hDDB )
{
    EDDI_LOCAL_DDB_PTR_TYPE  pDDB;
    LSA_RESULT               Status;

    Status = EDDI_GetDDB(hDDB, &pDDB);
    if (Status != EDD_STS_OK)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_CloseDDB, EDDI_GetDDB() failed with Status: 0x%X", Status);
        EDDI_Excp("EDDI_CloseDDB, EDDI_GetDDB() failed with Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
        return;
    }

    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_CHAT, "EDDI_CloseDDB->");
    EDDI_PROGRAM_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SIISetDummyIsr->set device granular interrupt-function-pointer to dummy-interrupt-function");
    EDDI_ENTER_SII_CRITICAL_S(EDDI_SII_CALLER_ID_CLOSE_DDB);
    EDDI_MemSet(pDDB, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_DDB_TYPE));
    EDDI_SIISetDummyIsr(pDDB);
    EDDI_EXIT_SII_CRITICAL_S(EDDI_SII_CALLER_ID_CLOSE_DDB);

    pDDB->bUsed = EDDI_DDB_NOT_USED;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_Dev32Memcpy()                               */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_Dev32Memcpy(       LSA_UINT32  *  const  pDest32,
                                                 const LSA_UINT32  *  const  pSrc32,
                                                       LSA_UINT32     const  AnzahlBytes )
{
    LSA_UINT32  AnzahlDWs, Ctr;

    if (AnzahlBytes & 0x3)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_Dev32Memcpy: AnzahlBytes (0x%X) not aligned", AnzahlBytes);
        EDDI_Excp("EDDI_Dev32Memcpy", EDDI_FATAL_ERR_EXCP, AnzahlBytes, 0);
        return;
    }

    if (AnzahlBytes > 0x40)
    {
        EDDI_PROGRAM_TRACE_01(0, LSA_TRACE_LEVEL_FATAL, "EDDI_Dev32Memcpy: AnzahlBytes (0x%X) >0x40", AnzahlBytes);
        EDDI_Excp("EDDI_Dev32Memcpy", EDDI_FATAL_ERR_EXCP, AnzahlBytes, 0);
        return;
    }

    AnzahlDWs = AnzahlBytes >> 2;   //dividiert durch 4

    for (Ctr = 0; Ctr < AnzahlDWs; Ctr++)
    {
        *(pDest32 + Ctr) = (*(pSrc32 + Ctr));
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_MemSet()                                    */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MemSet( EDDI_LOCAL_MEM_PTR_TYPE  pMem,
                                            LSA_UINT8                Value,
                                            LSA_UINT32               Length )
{
    if (pMem)
    {
        EDDI_MEMSET((pMem), (LSA_UINT8)(Value), (LSA_UINT32)(Length));
    }
    else
    {
        EDDI_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "EDDI_MemSet, p == 0, Value:0x%X, Length:0x%X", Value, Length);
        EDDI_Excp("EDDI_MemSet, p == 0", EDDI_FATAL_ERR_EXCP, Value, Length);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_MemCopy()                                   */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MemCopy( EDDI_DEV_MEM_PTR_TYPE           pDest,
                                             EDDI_LOCAL_MEM_PTR_TYPE         pSrc,
                                             LSA_UINT32                      Size  )
{
    if (pDest && pSrc)
    {
        EDDI_MEMCOPY(pDest, pSrc, (LSA_UINT)Size);
    }
    else
    {
       EDDI_PROGRAM_TRACE_02(0, LSA_TRACE_LEVEL_FATAL, "EDDI_MemCopy, p == 0, pDest:0x%X, pSrc:0x%X", pDest, pSrc);
        EDDI_Excp("EDDI_MemCopy, p == 0", EDDI_FATAL_ERR_EXCP, pDest, pSrc);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/



/***************************************************************************/
/* F u n c t i o n:       EDDI_AllocLocalMem()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_AllocLocalMem( EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                   LSA_UINT32                                     length )
{
    EDDI_ALLOC_LOCAL_MEM(lower_mem_ptr_ptr, length);

    if (LSA_HOST_PTR_ARE_EQUAL(*lower_mem_ptr_ptr, LSA_NULL))
    {
        *lower_mem_ptr_ptr = EDDI_NULL_PTR;

        EDDI_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "EDDI_AllocLocalMem, LSA_NULL");
    }

    g_LocalMem += length;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_FREELOCALMEM()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FREELOCALMEM( LSA_UINT16             EDDI_LOCAL_MEM_ATTR  *  ret_val_ptr,
                                                  EDDI_DEV_MEM_PTR_TYPE                          lower_mem_ptr )
{
    EDDI_FREE_LOCAL_MEM(ret_val_ptr, lower_mem_ptr);
}
/*---------------------- end [subroutine] ---------------------------------*/




/***************************************************************************/
/* F u n c t i o n:       EDDI_AllocLocalMem()                             */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if !defined EDDI_CFG_MAX_NR_PROVIDERS
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IOAllocLocalMem( EDDI_DEV_MEM_PTR_TYPE  EDDI_LOCAL_MEM_ATTR  *  lower_mem_ptr_ptr,
                                                     LSA_UINT32                                     length )
{
    EDDI_IO_ALLOC_LOCAL_MEM(lower_mem_ptr_ptr, length);

    if (LSA_HOST_PTR_ARE_EQUAL(*lower_mem_ptr_ptr, LSA_NULL))
    {
        *lower_mem_ptr_ptr = EDDI_NULL_PTR;

        EDDI_PROGRAM_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "EDDI_AllocLocalMem, LSA_NULL");
    }

    g_LocalMem += length;
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_FREELOCALMEM()                              */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_IOFreeLocalMem( EDDI_SYS_HANDLE                                  hSysDev,
                                                    LSA_UINT16                EDDI_LOCAL_MEM_ATTR  * ret_val_ptr,
                                                    EDDI_LOCAL_MEM_PTR_TYPE                          local_mem_ptr)
{
    LSA_UNUSED_ARG(hSysDev);

    EDDI_IO_FREE_LOCAL_MEM(hSysDev, ret_val_ptr, local_mem_ptr);
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif




/***************************************************************************/
/* F u n c t i o n:       EDDI_DEVAllocMode()                              */
/*                                                                         */
/* D e s c r i p t i o n: with no-MEM-check                                */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVAllocMode( EDDI_LOCAL_DDB_PTR_TYPE                          const  pDDB,
                                                  EDDI_MEMORY_MODE_TYPE                            const  MemMode,
                                                  EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  *  const  lower_mem_ptr_ptr,
                                                  LSA_UINT32                                       const  length,
                                                  EDDI_USERMEMID_TYPE                              const  UserMemID )
{
    LSA_UINT32  adr;

    switch (MemMode)
    {
        case MEMORY_SDRAM_ERTEC:
        {
            EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM(pDDB->hSysDev, lower_mem_ptr_ptr, length, UserMemID);
            pDDB->NramRes.NRT_SDRAM += length;
            adr = (LSA_UINT32)(*lower_mem_ptr_ptr);
            break;
        }

        case MEMORY_SHARED_MEM:
        {
            EDDI_ALLOC_DEV_SHARED_MEM(pDDB->hSysDev, lower_mem_ptr_ptr, length, UserMemID);
            pDDB->NramRes.NRT_SHARED_MEM += length;
            adr = (LSA_UINT32)(*lower_mem_ptr_ptr);

            if (   (adr <  pDDB->NRTMEM_LowerLimit)
                || (adr >= pDDB->NRTMEM_UpperLimit))
            {
                //user has possibly switched addresses by mistake 
               EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocMode, Range error: NRTMEM_LowerLimit(0x%X) < adr: 0x%X < NRTMEM_UpperLimit(0x%X)", 
                  pDDB->NRTMEM_LowerLimit, adr, pDDB->NRTMEM_UpperLimit);
                EDDI_Excp("EDDI_DEVAllocMode, Range error", EDDI_FATAL_ERR_EXCP, adr, 0);
                return;
            }

            break;
        }

        default:
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocMode, Invalid MemMode:0x%X", MemMode);
            EDDI_Excp("EDDI_DEVAllocMode, Invalid MemMode:", EDDI_FATAL_ERR_EXCP, MemMode, 0);
            return;
        }
    }

    if (LSA_HOST_PTR_ARE_EQUAL(*lower_mem_ptr_ptr, LSA_NULL))
    {
        EDDI_PROGRAM_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocMode, No MEM: MemMode:0x%X, lenght:0x%X", MemMode, length);
        EDDI_Excp("EDDI_DEVAllocMode, No MEM", EDDI_FATAL_ERR_EXCP, 0, 0);
        return;
    }

    if (adr & 0x07)
    {
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocMode, Alignment error, Memory has to be 64bit aligned. adr:0x%X, MemMode:0x%X, lenght:0x%X", adr, MemMode, length);
        EDDI_Excp("EDDI_DEVAllocMode, Alignment error, Memory has to be 64bit aligned, adr: length:", EDDI_FATAL_ERR_EXCP, adr, length);
        return;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DEVAllocModeTxFrag()                        */
/*                                                                         */
/* D e s c r i p t i o n: without no-MEM-check                             */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_FRAG_ON)
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVAllocModeTxFrag( EDDI_LOCAL_DDB_PTR_TYPE                          const  pDDB,
                                                        EDDI_MEMORY_MODE_TYPE                            const  MemMode,
                                                        EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  *  const  lower_mem_ptr_ptr,
                                                        LSA_UINT32                                       const  length,
                                                        EDDI_USERMEMID_TYPE                              const  UserMemID )
{
    LSA_UINT32  adr;

    switch (MemMode)
    {
        case MEMORY_SDRAM_ERTEC:
        {
            EDDI_ALLOC_DEV_SDRAM_ERTEC_MEM(pDDB->hSysDev, lower_mem_ptr_ptr, length, UserMemID);
            pDDB->NramRes.NRT_SDRAM += length;
            adr = (LSA_UINT32)(*lower_mem_ptr_ptr);
            break;
        }

        case MEMORY_SHARED_MEM:
        {
            EDDI_ALLOC_DEV_SHARED_MEM(pDDB->hSysDev, lower_mem_ptr_ptr, length, UserMemID);
            pDDB->NramRes.NRT_SHARED_MEM += length;
            adr = (LSA_UINT32)(*lower_mem_ptr_ptr);

            if (   (adr <  pDDB->NRTMEM_LowerLimit)
                || (adr >= pDDB->NRTMEM_UpperLimit))
            {
                //user has possibly switched addresses by mistake 
               EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocModeTxFrag, Range error: NRTMEM_LowerLimit(0x%X) < adr: 0x%X < NRTMEM_UpperLimit(0x%X)", 
                  pDDB->NRTMEM_LowerLimit, adr, pDDB->NRTMEM_UpperLimit);
                EDDI_Excp("EDDI_DEVAllocModeTxFrag, Range error", EDDI_FATAL_ERR_EXCP, adr, 0);
                return;
            }

            break;
        }

        default:
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocModeTxFrag, Invalid MemMode:0x%X", MemMode);
            EDDI_Excp("EDDI_DEVAllocModeTxFrag, Invalid MemMode:", EDDI_FATAL_ERR_EXCP, MemMode, 0);
            return;
        }
    }

    if (adr & 0x07)
    {
        EDDI_PROGRAM_TRACE_03(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVAllocModeTxFrag, Alignment error, Memory has to be 64bit aligned. adr:0x%X, MemMode:0x%X, lenght:0x%X", adr, MemMode, length);
        EDDI_Excp("EDDI_DEVAllocModeTxFrag, Alignment error, Memory has to be 64bit aligned, adr: length:", EDDI_FATAL_ERR_EXCP, adr, length);
        return;
    }
}
#endif
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_DEVFreeMode()                               */
/*                                                                         */
/* D e s c r i p t i o n: MEM is freed that was allocated by               */
/*                        EDDI_DEVAllocMode and EDDI_DEVAllocModeTxFrag    */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_DEVFreeMode( EDDI_LOCAL_DDB_PTR_TYPE                       const  pDDB,
                                                 EDDI_MEMORY_MODE_TYPE                         const  MemMode,
                                                 EDDI_DEV_MEM_PTR_TYPE    EDDI_LOCAL_MEM_ATTR  const  lower_mem_ptr,
                                                 EDDI_USERMEMID_TYPE                           const  UserMemID )
{
    LSA_UINT16  ret_val;

    switch (MemMode)
    {
        case MEMORY_SDRAM_ERTEC:
        {
            EDDI_FREE_DEV_SDRAM_ERTEC_MEM(pDDB->hSysDev, &ret_val, lower_mem_ptr, UserMemID);
            if (ret_val != LSA_RET_OK)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVFreeMode, SDRAM ret_val:0x%X", ret_val);
                EDDI_Excp("EDDI_DEVFreeMode, SDRAM", EDDI_FATAL_ERR_EXCP, ret_val, 0);
            }
            return;
        }

        case MEMORY_SHARED_MEM:
        {
            EDDI_FREE_DEV_SHARED_MEM(pDDB->hSysDev, &ret_val, lower_mem_ptr, UserMemID);
            if (ret_val != LSA_RET_OK)
            {
                EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVFreeMode, SHARED_MEM ret_val:0x%X", ret_val);
                EDDI_Excp("EDDI_DEVFreeMode, SHARED_MEM", EDDI_FATAL_ERR_EXCP, ret_val, 0);
            }
            return;
        }

        default:
        {
            EDDI_PROGRAM_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_DEVFreeMode, Invalid MemMode:0x%X", MemMode);
            EDDI_Excp("EDDI_DEVFreeMode, Invalid MemMode:", EDDI_FATAL_ERR_EXCP, MemMode, 0);
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/***************************************************************************/
/* F u n c t i o n:       EDDI_SetDetailError()                            */
/*                                                                         */
/* D e s c r i p t i o n:                                                  */
/*                                                                         */
/* A r g u m e n t s:                                                      */
/*                                                                         */
/* Return Value:          LSA_VOID                                         */
/*                                                                         */
/***************************************************************************/
#if defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //leads to smaller code
LSA_VOID  EDDI_SetDetailError( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                               LSA_UINT32               const  Error,
                               LSA_UINT32               const  ModuleID )
#else
LSA_VOID  EDDI_SetDetailError( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                               LSA_UINT32               const  Error,
                               LSA_UINT32               const  ModuleID,
                               LSA_UINT32               const  Line )
#endif
{
    if (pDDB->ErrDetail.bSet == 0)
    {
        pDDB->ErrDetail.Error    = Error;
        pDDB->ErrDetail.ModuleID = ModuleID;
        #if !defined (EDDI_CFG_NO_DETAIL_ERROR_LINE_INFO) //leads to smaller code
        pDDB->ErrDetail.Line     = Line;
        #endif
        pDDB->ErrDetail.bSet     = 1;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/
#if 0   // for debugging purposes set it to 1    see 'eddi_dev.h'
LSA_VOID  EDDI_PRM_SET_ERR_OFFSET( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                   LSA_UINT32               const  Offset)
{
    pDDB->PRM.PrmDetailErr.ErrOffset += (Offset);
    pDDB->PRM.PrmDetailErr.ErrValid   = LSA_TRUE;
}
#endif


#if defined (EDDI_CFG_REV7)
//0xFFFFFFFF / 10
#define CLK_WRAP_AROUND     0x19999999UL
#define MAX_NUM_LSA_UINT32  0xFFFFFFFFUL

//////////////////////////////////////////////////////////////////////////
///
/// Reduction of the 1ns granular SOC Timer to a 10ns ERTEC200/400 Timer
/// Corrects the value shift around the wraparound point at 0x1999...
/// Because both timer have the same value-range about 32Bit.
///
/// @note The downsampling only works if this function is called each
///       4.29 seconds to keep track of a possible overflow.
///
/// @param  clk The 1ns timervalue of the SOC-hardware
/// @return downsampled 10ns value
///
//////////////////////////////////////////////////////////////////////////
LSA_UINT32  eddi_getClockCount10ns( LSA_UINT32  const  clk )
{
    static  LSA_UINT32  last_clk_cycle  = 0;
    static  LSA_UINT32  last_clk        = 0;
    static  LSA_UINT32  correction_term = 0;
    LSA_UINT32          red_clk;

    if (clk == MAX_NUM_LSA_UINT32)
    {
        //We hit the wraparound-point
        red_clk = ((clk + correction_term) / 10) + last_clk_cycle;

        last_clk_cycle += CLK_WRAP_AROUND;
        correction_term += 6;

        if (last_clk_cycle == MAX_NUM_LSA_UINT32)
        {
            correction_term = 0;
        }
    }
    else if ((last_clk > clk) && (last_clk != MAX_NUM_LSA_UINT32))
    {
        //We surrpassed the wraparound-point
        correction_term += 6;
        last_clk_cycle  += CLK_WRAP_AROUND;

        red_clk = ((clk + correction_term) / 10) + last_clk_cycle;

        if (last_clk_cycle == MAX_NUM_LSA_UINT32)
        {
            correction_term = 0;
        }
    }
    else
    {
        //Normal reduction about factor 10
        red_clk = ((clk + correction_term) / 10) + last_clk_cycle;
    }

    last_clk = clk;

    return red_clk;
}
#endif //EDDI_CFG_REV7
/*---------------------- end [subroutine] ---------------------------------*/


/*****************************************************************************/
/*  end of file eddi_dev.c                                                   */
/*****************************************************************************/


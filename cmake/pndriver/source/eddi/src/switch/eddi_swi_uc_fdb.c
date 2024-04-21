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
/*  F i l e               &F: eddi_swi_uc_fdb.c                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:  Ser10 File Data Base for EDDI.                   */
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
#include "eddi_time.h"
#include "eddi_ser_cmd.h"

#include "eddi_swi_ext.h"
#include "eddi_swi_ucmc.h"
#include "eddi_lock.h"

#define EDDI_MODULE_ID     M_ID_SWI_UC_FDB
#define LTRC_ACT_MODUL_ID  316

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiUcIniFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE               const pDDB,
                                                             EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE const pRQBEntry,
                                                             EDDI_SER_UCMC_PTR_TYPE                      pFDBEntry );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUcIniFDBGetEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE                     const pDDB,
                                                                   EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE       pRQBEntry,
                                                                   EDDI_SER_UCMC_PTR_TYPE                      const pFDBEntry );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_SwiUcFDBIniGetEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                   const pDDB,
                                                                 EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE const pRQBEntry,
                                                                 EDDI_SER_UCMC_PTR_TYPE                    const pFDBEntry,
                                                                 LSA_UINT32                                const Index );

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SwiUcSetFDBEntry( CONST_SER_HANDLE_PTR          const pSWIPara,
                                                             EDDI_SER_UCMC_PTR_TYPE  const pFDBEntry,
                                                             LSA_UINT32              const Index,
                                                             EDDI_LOCAL_DDB_PTR_TYPE const pDDB );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE const pDDB, 
                                                                CONST_SER_HANDLE_PTR          const pSWIPara,
                                                                EDDI_SER_UCMC_PTR_TYPE  const pFDBEntry,
                                                                LSA_UINT32              const Index );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcFDBSearchEntry( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB, 
                                                                   CONST_SER_HANDLE_PTR                    pSWIPara,
                                                                   EDDI_LOCAL_MAC_ADR_PTR_TYPE       macAdr,
                                                                   LSA_UINT32                      * pIndex,
                                                                   EDDI_SER_FDB_SEARCH_TYPE    const searchMode );

static void EDDI_LOCAL_FCT_ATTR EDDI_FDBControlPFBit( EDDI_SER_UCMC_PTR_TYPE pEntry );

static  EDDI_SER_UCMC_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcFDBSearchFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE     const pDDB, 
                                                                                   CONST_SER_HANDLE_PTR              const pSWIPara,
                                                                                   LSA_UINT32                        Index,
                                                                                   EDDI_LOCAL_MAC_ADR_PTR_TYPE       pMACAdr );


/*===========================================================================*/
/*                            local function definition                      */
/*===========================================================================*/


/*=============================================================================
* function name: EDDI_SwiUcSetFDBDestPort()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcSetFDBDestPort( EDDI_LOCAL_DDB_PTR_TYPE            const  pDDB,
                                                         LSA_UINT8                          const  bRQBCHA,
                                                         LSA_UINT8                          const  bRQBCHB,
                                                         LSA_UINT8                const  *  const  pRQBPortID,
                                                         LSA_UINT8                              *  pFDBDestPort )
{
    LSA_UINT16  UsrPortIndex;
    LSA_UINT8   MACPort;

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcSetFDBDestPort->bRQBCHA:0x%X bRQBCHB:0x%X pRQBPortID:0x%X *pFDBDestPort:0x%X", 
                      bRQBCHA, bRQBCHB, *pRQBPortID, *pFDBDestPort);

    *pFDBDestPort = 0;

    //Channel A
    if (bRQBCHA)
    {
        *pFDBDestPort |= SWI_MAC_CH_A;
    }

    //Channel B
    if (bRQBCHB)
    {
        *pFDBDestPort |= SWI_MAC_CH_B;
    }

    //user structure

    //Port0 = 0x04, Port1= 0x08, Port2 = 0x10; Port3 = 0x20
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        MACPort = (LSA_UINT8)EDDI_PmGetMask0420(pDDB, UsrPortIndex); //SWI_MAC_PORT_0

        if (pRQBPortID[UsrPortIndex])
        {
            *pFDBDestPort |= MACPort;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcGetFDBDestPort()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBDestPort( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                         LSA_UINT8                const  FDBDestPort,
                                                         LSA_UINT8                    *  pRQBCHA,
                                                         LSA_UINT8                    *  pRQBCHB,
                                                         LSA_UINT8                    *  pRQBPortID )
{
    LSA_UINT8   MACPort;
    LSA_UINT16  UsrPortIndex;

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcGetFDBDestPort->RQBCHA:0x%X RQBCHB:0x%X pRQBPortID:0x%X FDBDestPort:0x%X",
                      *pRQBCHA, *pRQBCHB, *pRQBPortID, FDBDestPort);

    //Channel A
    if (0 != (SWI_MAC_CH_A & FDBDestPort))
    {
        *pRQBCHA = EDD_FEATURE_ENABLE;
    }
    else
    {
        *pRQBCHA = EDD_FEATURE_DISABLE;
    }

    //Channel B
    if (0 != (SWI_MAC_CH_B & FDBDestPort))
    {
        *pRQBCHB = EDD_FEATURE_ENABLE;
    }
    else
    {
        *pRQBCHB = EDD_FEATURE_DISABLE;
    }

    //user structure

    //Port0 = 0x04, Port1= 0x08, Port2 = 0x10; Port3 = 0x20
    for (UsrPortIndex = 0; UsrPortIndex < pDDB->PM.PortMap.PortCnt; UsrPortIndex++)
    {
        MACPort = (LSA_UINT8)EDDI_PmGetMask0420(pDDB, UsrPortIndex);

        if (0 != (MACPort & FDBDestPort))
        {
            pRQBPortID[UsrPortIndex] = EDD_FEATURE_ENABLE;
        }
        else
        {
            pRQBPortID[UsrPortIndex] = EDD_FEATURE_DISABLE;
        }
    }

    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcGetFDBDestPort<-RQBCHA:0x%X RQBCHB:0x%X pRQBPortID:0x%X FDBDestPort:0x%X",
                      *pRQBCHA, *pRQBCHB, *pRQBPortID, FDBDestPort);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_FDBControlPFBit()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_FDBControlPFBit( EDDI_SER_UCMC_PTR_TYPE  pEntry )
{
    /* set filter bit only if flow prio requested not if PAUSE entry!!! */
    /* When PAUSE entry requested, F-/P-Bit is still set */
    /* (PAUSE entry: typ=1; prio=3; F-/P-Bit=1)*/
    if (!((EDDI_GetBitField32(pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type)) &&
          (EDDI_GetBitField32(pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio) == SER_FDB_FLUSS_PRIO) &&
          (!EDDI_GetBitField32(pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter))))
    {
        EDDI_SetBitField32(&pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter, 0x1);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcIniFDBEntry()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcIniFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE  const  pRQBEntry,
                                                                EDDI_SER_UCMC_PTR_TYPE                        pFDBEntry )
{
    LSA_UINT8   FDBDestPort = 0;
    LSA_UINT8   FDBPrio = 0;
    LSA_RESULT  Status;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcIniFDBEntry->");

    pFDBEntry->Value.U32_0 = 0;
    //is initialized with MacAdr pFDBEntry->Value.U32_1 = 0;

    //Check Filter and Pause Bit
    //The MAC table contains only 1 bit
    if ((EDD_FEATURE_DISABLE != pRQBEntry->Pause) && (EDD_FEATURE_DISABLE != pRQBEntry->Filter))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcIniFDBEntry, 1, Pause:0x%X Filter:0x%X", pRQBEntry->Pause, pRQBEntry->Filter);
        return EDD_STS_ERR_PARAM;
    }

    //PauseBit is only allowed if T==1 and Prio==11
    if ((EDD_FEATURE_DISABLE != pRQBEntry->Pause) && (EDDI_SWI_FDB_PRIO_FLUSS != pRQBEntry->Prio))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcIniFDBEntry, 2, Pause:0x%X Prio:0x%X", pRQBEntry->Pause, pRQBEntry->Prio);
        return EDD_STS_ERR_PARAM;
    }

    //FilterBit is only allowed if T==1 and Prio!=11
    if ((EDD_FEATURE_DISABLE != pRQBEntry->Filter) && (EDDI_SWI_FDB_PRIO_FLUSS == pRQBEntry->Prio))
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcIniFDBEntry, 3, Pause:0x%X Filter:0x%X", pRQBEntry->Pause, pRQBEntry->Filter);
        return EDD_STS_ERR_PARAM;
    }

    //Filter and Pause
    if (EDD_FEATURE_DISABLE != pRQBEntry->Pause)
    {
        EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter, 1);
    }
    else if (EDD_FEATURE_DISABLE != pRQBEntry->Filter)
    {
        EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter, 1);
    }
    else
    {
        //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter, 0);
    }

    //Port
    EDDI_SwiUcSetFDBDestPort(pDDB, pRQBEntry->CHA, pRQBEntry->CHB, &pRQBEntry->PortID[0], &FDBDestPort);

    //Borderline
    #if defined (EDDI_CFG_REV7)
    if (EDD_FEATURE_DISABLE != pRQBEntry->Borderline)
    {
        EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__BLBit, 1);
    }
    else if (EDD_FEATURE_DISABLE != pRQBEntry->Filter)
    {
        EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__BLBit, 0);
    }
    #endif

    //Prio
    Status = EDDI_SwiUcSetFDBPrio(pDDB, pRQBEntry->Prio, &FDBPrio);
    if (EDD_STS_OK != Status)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcIniFDBEntry, 3, Prio:0x%X", pRQBEntry->Prio);
        return Status;
    }

    //MAC-Address
    EDDI_MEMCOPY(&pFDBEntry->Byte[2], &pRQBEntry->MACAddress, EDD_MAC_ADDR_SIZE);

    EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__DestPort, FDBDestPort);

    EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio, FDBPrio);

    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Reserved_0, 0);

    #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__SyncBit   , 0); //TimeMaster list is deactivated
    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__LS_FU     , 0); //TimeMaster list is deactivated
    EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Age_DCP     , 1);
    #else
    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Reserved_1, 0);
    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Ls        , 0);
    //EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Age       , 0);
    #endif

    EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type , 1);
    EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Valid, 1);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcIniFDBEntry<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcIniFDBGetEntryIndex()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcIniFDBGetEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB,
                                                                      EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE         pRQBEntry,
                                                                      EDDI_SER_UCMC_PTR_TYPE                       const  pFDBEntry )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcIniFDBGetEntryIndex->");

    //Port
    EDDI_SwiUcGetFDBDestPort(pDDB,
                             (LSA_UINT8)EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__DestPort),
                             &pRQBEntry->CHA,
                             &pRQBEntry->CHB,
                             &pRQBEntry->PortID[0]);

    //Prio
    EDDI_SwiUcGetFDBPrio(pDDB, (LSA_UINT8)EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio), &pRQBEntry->Prio);

    //MAC-Adress
    EDDI_SwiUcGetFDBAdress(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&pFDBEntry->Byte[2], &pRQBEntry->MACAddress);

    //Type
    pRQBEntry->Type   =
        (LSA_UINT16)(EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type) ? EDDI_SWI_FDB_ENTRY_STATIC : EDDI_SWI_FDB_ENTRY_DYNAMIC);

    //Valid
    pRQBEntry->Valid  =
        (LSA_BOOL)(EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Valid)  ? EDDI_SWI_FDB_ENTRY_VALID:EDDI_SWI_FDB_ENTRY_INVALID);

    //Pause or Filter
    if (0 == EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter))
    {
        pRQBEntry->Pause  = EDD_FEATURE_DISABLE;
        pRQBEntry->Filter = EDD_FEATURE_DISABLE;
    }
    else
    {
        if (   (1 == EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type))
            && (EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio) == SER_FDB_FLUSS_PRIO))
        {
            pRQBEntry->Pause  = EDD_FEATURE_ENABLE;
            pRQBEntry->Filter = EDD_FEATURE_DISABLE;
        }
        else
        {
            pRQBEntry->Pause  = EDD_FEATURE_DISABLE;
            pRQBEntry->Filter = EDD_FEATURE_ENABLE;
        }
    }

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcIniFDBGetEntryIndex<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcFDBIniGetEntryMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcFDBIniGetEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                                    EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE  const  pRQBEntry,
                                                                    EDDI_SER_UCMC_PTR_TYPE                     const  pFDBEntry,
                                                                    LSA_UINT32                                 const  Index )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcFDBIniGetEntryMAC->");

    //Borderline
    #if defined (EDDI_CFG_REV7)
    pRQBEntry->Borderline = (LSA_UINT8)EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__BLBit);
    #else
    pRQBEntry->Borderline = EDD_FEATURE_DISABLE;
    #endif

    //Port
    EDDI_SwiUcGetFDBDestPort(pDDB,
                             (LSA_UINT8)EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__DestPort),
                             &pRQBEntry->CHA,
                             &pRQBEntry->CHB,
                             &pRQBEntry->PortID[0]);

    //Prio
    EDDI_SwiUcGetFDBPrio(pDDB, (LSA_UINT8)EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio), &pRQBEntry->Prio);

    //MAC-Adress
    EDDI_SwiUcGetFDBAdress(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&pFDBEntry->Byte[2], &pRQBEntry->MACAddress);

    //Type
    pRQBEntry->Type  = (LSA_UINT16)(EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type) ? EDDI_SWI_FDB_ENTRY_STATIC : EDDI_SWI_FDB_ENTRY_DYNAMIC);

    //Valid
    pRQBEntry->Valid = (LSA_BOOL)(EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Valid) ? EDDI_SWI_FDB_ENTRY_VALID : EDDI_SWI_FDB_ENTRY_INVALID);

    //Index
    pRQBEntry->Index = Index;

    //Pause or Filter
    if (0 == EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter))
    {
        pRQBEntry->Pause  = EDD_FEATURE_DISABLE;
        pRQBEntry->Filter = EDD_FEATURE_DISABLE;
    }
    else
    {
        if (   (1 == EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Type))
            && (EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_UCMC_BIT__Prio) == SER_FDB_FLUSS_PRIO))
        {
            pRQBEntry->Pause  = EDD_FEATURE_ENABLE;
            pRQBEntry->Filter = EDD_FEATURE_DISABLE;
        }
        else
        {
            pRQBEntry->Pause  = EDD_FEATURE_DISABLE;
            pRQBEntry->Filter = EDD_FEATURE_ENABLE;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcSetFDBEntry()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcSetFDBEntry( CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                                EDDI_SER_UCMC_PTR_TYPE   const  pFDBEntry,
                                                                LSA_UINT32               const  Index,
                                                                EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_SER_UCMC_PTR_TYPE  pEntry;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcSetFDBEntry->Index:0x%X", Index);

    //value range 0..UCMC_Table_Length
    if (Index > pSWIPara->UCMC_Table_Length)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcIniFDBEntry, pSWIPara->UCMC_Table_Length:0x%X Index:0x%X", pSWIPara->UCMC_Table_Length, Index);
        return EDD_STS_ERR_PARAM;
    }

    pEntry = &pSWIPara->pDev_UCMC_Table_Base[Index];

    //setting MAC Address
    pEntry->Value.U32_1 = pFDBEntry->Value.U32_1;

    if (pDDB->IRTE_SWI_BaseAdr == pDDB->IRTE_SWI_BaseAdr_32Bit)
    {
        //setting valid bit at last write high word first
        pEntry->Value.U32_0 = pFDBEntry->Value.U32_0;
    }
    else
    {
        //LBU access model
        LSA_UINT16 *pDestTemp16, *pSrcTemp16;

        //setting rest of the MAC Address
        pDestTemp16  = (LSA_UINT16*)(void*)&pEntry->Byte[2];
        pSrcTemp16   = (LSA_UINT16*)(void*)&pFDBEntry->Byte[2];
        *pDestTemp16 = *pSrcTemp16;

        //setting the valid at last
        pDestTemp16  = (LSA_UINT16*)(void*)&pEntry->Byte[0];
        pSrcTemp16   = (LSA_UINT16*)(void*)&pFDBEntry->Byte[0];
        *pDestTemp16 = *pSrcTemp16;
    }

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcSetFDBEntry<-pU32_0:0x%X U32_1:0x%X", pFDBEntry->Value.U32_0, pFDBEntry->Value.U32_1);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcGetFDBEntry()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcGetFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                                CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                EDDI_SER_UCMC_PTR_TYPE       const  pFDBEntry,
                                                                LSA_UINT32                   const  Index )
{
    EDDI_SER_UCMC_PTR_TYPE  pEntry;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcGetFDBEntry->Index:0x%X", Index);

    //value range 0..UCMC_Table_Length
    if (Index > pSWIPara->UCMC_Table_Length)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcGetFDBEntry, pSWIPara->UCMC_Table_Length:0x%X Index:0x%X", pSWIPara->UCMC_Table_Length, Index);
        return EDD_STS_ERR_PARAM;
    }

    pEntry = &pSWIPara->pDev_UCMC_Table_Base[Index];

    pFDBEntry->Value.U32_0 = pEntry->Value.U32_0;
    pFDBEntry->Value.U32_1 = pEntry->Value.U32_1;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcGetFDBEntry<-pU32_0:0x%X U32_1:0x%X", pFDBEntry->Value.U32_0, pFDBEntry->Value.U32_1);

    LSA_UNUSED_ARG(pDDB);
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcFDBSearchEntry()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcFDBSearchEntry( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                                   CONST_SER_HANDLE_PTR                      pSWIPara,
                                                                   EDDI_LOCAL_MAC_ADR_PTR_TYPE         macAdr,
                                                                   LSA_UINT32                       *  pIndex,
                                                                   EDDI_SER_FDB_SEARCH_TYPE     const  searchMode )
{
    LSA_UINT32          i;
    LSA_INT             timeout;
    EDDI_SER_UCMC_TYPE  FDBSearchEntry;
    LSA_RESULT          Result;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcFDBSearchEntry->Index:0x%X searchMode:0x%X", *pIndex, searchMode);

    for (i = 0; i <= pSWIPara->UCMC_Table_Range; i++)
    {
        Result = EDDI_SwiUcGetFDBEntry(pDDB, pSWIPara, &FDBSearchEntry, *pIndex);
        if (EDD_STS_OK != Result)
        {
            return Result;
        }

        //distinguish between different search modes
        switch (searchMode)
        {
            case FDB_SEARCH_VALID_STATIC_ENTRY:
            {  //valid==1 && typ==1 && identical mac address
                if (EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Valid) &&
                    EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Type) &&
                    !EDDI_SwiUcCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&FDBSearchEntry.Byte[2], macAdr))
                {
                    return EDD_STS_OK;
                }
                break;
            }

            case FDB_SEARCH_VALID_DYN_ENTRY: //searches only dynamical entries
            {
                //wait until ls-bit is reset or timeout
                timeout = (LSA_INT)10;
                #if defined (EDDI_CFG_REV6) || defined (EDDI_CFG_REV7)
                while ((EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__LS_FU)) && (--timeout))  //Waiting point until IRTE has written the dynamic entry
                #else
                while ((EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Ls)) && (--timeout))  //Waiting point until IRTE has written the dynamic entry
                #endif
                {
                    Result = EDDI_SwiUcGetFDBEntry(pDDB, pSWIPara, &FDBSearchEntry, *pIndex);
                    if (EDD_STS_OK != Result)
                    {
                        return Result;
                    }
                }

                if (!timeout)
                {
                    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SwiUcFDBSearchEntry, ser_fdb Learning-Semaphor-Bit wasn't reset");
                    return EDD_STS_ERR_TIMEOUT;
                }

                //valid && !typ && identical mac address //search valid dynamic entry
                if ( EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Valid) &&
                     !EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Type) &&
                     !EDDI_SwiUcCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&FDBSearchEntry.Byte[2], macAdr))
                {
                    return EDD_STS_OK;
                }
                break;
            }

            case FDB_SEARCH_FREE_ENTRY:
            {
                //typ==0 && filter==0
                if (!EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Type) &&
                    !EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__PauseFilter)) //Pausefilter at dyn. entry also
                {
                    return EDD_STS_OK;
                }
                break;
            }

            case FDB_SEARCH_VALID_ENTRY:
            {   
                //valid==1 && identical mac address
                if (EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Valid) &&
                    !EDDI_SwiUcCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&FDBSearchEntry.Byte[2], macAdr))
                {
                    return EDD_STS_OK;
                }
                break;
            }

            default:
            {
                EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SwiUcFDBSearchEntry, ser_fdb_wrong");
                return EDD_STS_ERR_PARAM;
            }
        }

        //wrap around
        (*pIndex)++;

        //Value range 0..UCMC_Table_Length
        if (*pIndex > pSWIPara->UCMC_Table_Length)
        {
            *pIndex = 0;
        }
    }


    return EDD_STS_OK_NO_DATA;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SwiUcFDBSearchFreeEntry()
*
* function:
*
* parameters:
*
* return value:
*
* comment:
*==========================================================================*/
static  EDDI_SER_UCMC_PTR_TYPE  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcFDBSearchFreeEntry( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                                                   CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                                   LSA_UINT32                          Index,
                                                                                   EDDI_LOCAL_MAC_ADR_PTR_TYPE         pMACAdr )
{
    LSA_UINT32              i;
    EDDI_SER_UCMC_PTR_TYPE  pEntry;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcFDBSearchFreeEntry->Index:0x%X", Index);

    pEntry = &pSWIPara->pDev_UCMC_Table_Base[Index];

    for (i = 0; i <= pSWIPara->UCMC_Table_Range; i++)
    {
        //typ==0 && filter==0
        if (pEntry->Value.U32_0 == 0)
        {
            //free entry
            return pEntry;
        }

        //MACAdr already exists, not allowed during config mode
        if (!EDDI_SwiUcCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&pEntry->Byte[2], pMACAdr))
        {
            EDDI_Excp("EDDI_SwiUcFDBSearchFreeEntry, MACAdr already exist, not allowed during config mode", EDDI_FATAL_ERR_EXCP, Index, 0);
            return EDDI_NULL_PTR;
        }

        Index++;

        //value range 0..UCMC_Table_Length
        if (Index <= pSWIPara->UCMC_Table_Length)
        {
            pEntry++;
        }
        else
        {
            //wrap around
            pEntry = &pSWIPara->pDev_UCMC_Table_Base[0];
            Index  = 0;
        }
    }

    EDDI_Excp("EDDI_SwiUcFDBSearchFreeEntry", EDDI_FATAL_ERR_EXCP, 0, 0);
    return EDDI_NULL_PTR;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================*/
/*                            extern function definition                     */
/*===========================================================================*/

/*=============================================================================
* function name: EDDI_SwiUcCompareMac()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT32
*
* comment:
*==========================================================================*/
LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_SwiUcCompareMac( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB, 
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p1,
                                                       EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p2 )
{
    LSA_INT32   Index;
    LSA_UINT32  retVal = 0;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SwiUcCompareMac->");

    for (Index = EDD_MAC_ADDR_SIZE - 1; Index >= 0; Index--)
    {
        if (p1->MacAdr[Index] == p2->MacAdr[Index])
        {
            continue;
        }

        if (p1->MacAdr[Index] < p2->MacAdr[Index])
        {
            retVal = 0xFFFFFFFFUL;
        }
        else
        {
            retVal = 1UL;
        }

        break;
    }

    LSA_UNUSED_ARG(pDDB);
    return retVal;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetFDBEntryConfig()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetFDBEntryConfig( CONST_SER_HANDLE_PTR                         pSWIPara,
                                                          EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE  pUsrFDBEntry,
                                                          EDDI_LOCAL_DDB_PTR_TYPE                pDDB )
{
    LSA_RESULT              Result;
    LSA_UINT32              Index;
    EDDI_SER_UCMC_TYPE      FDBEntry;
    EDDI_SER_UCMC_PTR_TYPE  pEntry;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERSetFDBEntryConfig->");

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        if (pUsrFDBEntry->MACAddress.MacAdr[0] & 0x01)
        {
            EDDI_ENTER_SYNC_S_SPECIAL();
            Result = EDDI_SERSetMCFDBEntry(pDDB, pSWIPara, pUsrFDBEntry);
            EDDI_EXIT_SYNC_S_SPECIAL();
           
            if (EDD_STS_OK != Result)
            {
                EDDI_Excp("EDDI_SERSetFDBEntryConfig, EDDI_SERSetMCFDBEntry", EDDI_FATAL_ERR_EXCP, Result, 0);
            }
            return;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)

    Result = EDDI_SwiUcIniFDBEntry(pDDB, pUsrFDBEntry, &FDBEntry);
    if (EDD_STS_OK != Result)
    {
        EDDI_Excp("EDDI_SERSetFDBEntryConfig, EDDI_SwiUcIniFDBEntry", EDDI_FATAL_ERR_EXCP, Result, 0);
        return;
    }
    
    EDDI_ENTER_SYNC_S_SPECIAL();
    //calc index, value range 0..UCMC_Table_Length
    Index = EDDI_SwiUcCalcAdr(&pUsrFDBEntry->MACAddress, (LSA_UINT32)pSWIPara->UCMC_LFSR_Mask,
                              (LSA_UINT32)pSWIPara->UCMC_Table_Length);

    pEntry = EDDI_SwiUcFDBSearchFreeEntry(pDDB, pSWIPara, Index, &pUsrFDBEntry->MACAddress);
    EDDI_EXIT_SYNC_S_SPECIAL();    
    
    if (pEntry == EDDI_NULL_PTR)
    {
        return;
    }  

    //check if pf-bit should be set
    EDDI_FDBControlPFBit(&FDBEntry);

    //(3) set entry to fdb
    //setting MAC Address
    pEntry->Value.U32_1 = FDBEntry.Value.U32_1;

    if (pDDB->IRTE_SWI_BaseAdr == pDDB->IRTE_SWI_BaseAdr_32Bit)
    {
        //setting valid bit at last write high word first
        pEntry->Value.U32_0 = FDBEntry.Value.U32_0;
    }
    else
    {
        //LBU access model
        LSA_UINT16 *pDestTemp16, *pSrcTemp16;

        //setting rest of the MAC Address
        pDestTemp16  = (LSA_UINT16*)(void*)&pEntry->Byte[2];
        pSrcTemp16   = (LSA_UINT16*)(void*)&FDBEntry.Byte[2];
        *pDestTemp16 = *pSrcTemp16;

        //setting the valid at last
        pDestTemp16  = (LSA_UINT16*)(void*)&pEntry->Byte[0];
        pSrcTemp16   = (LSA_UINT16*)(void*)&FDBEntry.Byte[0];
        *pDestTemp16 = *pSrcTemp16;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERSetFDBEntryRun()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetFDBEntryRun( CONST_SER_HANDLE_PTR                                pSWIPara,
                                                         EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE         pUsrFDBEntry,
                                                         LSA_BOOL                               const  bCheckPN,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                       pDDB )
{
    LSA_RESULT          Result;
    LSA_UINT32          Index;
    LSA_UINT32          JumpInIndex;
    EDDI_SER_UCMC_TYPE  FDBEntry;
    EDDI_SER_UCMC_TYPE  FDBSearchEntry;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERSetFDBEntryRun->");

    //PNIO-MAC-Addresses are note to be changed!
    if (bCheckPN)
    {
        if ((pUsrFDBEntry->MACAddress.MacAdr[0] & 0x01) || EDDI_SwiPNIsSTBY1MAC(pDDB, &pUsrFDBEntry->MACAddress))
        {
            if (EDDI_SwiPNIsMACInFDB(pDDB, &pUsrFDBEntry->MACAddress, LSA_TRUE /*bSetInRun*/))
            {
                EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "SERSetFDBEntry, EDD_STS_ERR_NOT_ALLOWED");

                return EDD_STS_ERR_NOT_ALLOWED;
            }
        }
    }

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        if (pUsrFDBEntry->MACAddress.MacAdr[0] & 0x01)
        {
            EDDI_ENTER_SYNC_S_SPECIAL();
            Result = EDDI_SERSetMCFDBEntry(pDDB, pSWIPara, pUsrFDBEntry);
            EDDI_EXIT_SYNC_S_SPECIAL();

            return Result;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)

    Result = EDDI_SwiUcIniFDBEntry(pDDB, pUsrFDBEntry, &FDBEntry);
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    //calc index, valuerange 0..UCMC_Table_Length
    JumpInIndex = EDDI_SwiUcCalcAdr(&pUsrFDBEntry->MACAddress, (LSA_UINT32)pSWIPara->UCMC_LFSR_Mask,
                                    (LSA_UINT32)pSWIPara->UCMC_Table_Length);

    /* (1) Search for existing static entry */
    Index = JumpInIndex;

    if (EDD_STS_OK == EDDI_SwiUcFDBSearchEntry(pDDB, pSWIPara, &pUsrFDBEntry->MACAddress, &Index, FDB_SEARCH_VALID_STATIC_ENTRY))
    {
        //MAC address in FDB table exists as static entry
        EDDI_FDBControlPFBit(&FDBEntry);
        return EDDI_SwiUcSetFDBEntry(pSWIPara, &FDBEntry, Index, pDDB);
    }

    EDDI_MemSet(&FDBSearchEntry, (LSA_UINT8)0, (LSA_UINT32)sizeof(EDDI_SER_UCMC_TYPE));

    //reserve entry via command interface
    //Result = SERReserveAdrEntryFDB(JumpInIndex, &Index, pDDB);
    EDDI_SERSingleCommand(FCODE_RESERVE_ADR_ENTRY, JumpInIndex * sizeof(EDDI_SER_UCMC_TYPE), &Index, pDDB);
    //ok && valid Index ? (when Index=0x1fffff than no entry can be reserved)
    if (Index < SER10_NULL_PTR_SWAP)
    {
        Index -= DEV_kram_adr_to_asic_register(pSWIPara->pDev_UCMC_Table_Base, pDDB);
        Index /= sizeof(EDDI_SER_UCMC_TYPE);
    }
    else
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "SERSetFDBEntry, Cannot reserve AddressEntry");
        return EDD_STS_ERR_HW;
    }

    /* check if reserve command is successful */
    Result = EDDI_SwiUcGetFDBEntry(pDDB, pSWIPara, &FDBSearchEntry, Index);
    if (Result != EDD_STS_OK)
    {
        return Result;
    }

    if ( EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Valid) ||
        !EDDI_GetBitField32(FDBSearchEntry.Value.U32_0, EDDI_SER_UCMC_BIT__Type))
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "SERSetFDBEntry, Invalid reserved address entry at");
        return EDD_STS_ERR_RESOURCE;
    }

    /* check if pf-bit should be set */
    EDDI_FDBControlPFBit(&FDBEntry);
    // (3) set entry to fdb
    Result = EDDI_SwiUcSetFDBEntry(pSWIPara, &FDBEntry, Index, pDDB);
    if (Result != EDD_STS_OK)
    {
        return Result;
    }

    // (4) remove possible dynamic entries
    Index = JumpInIndex;

    if (EDD_STS_OK == EDDI_SwiUcFDBSearchEntry(pDDB, pSWIPara, &pUsrFDBEntry->MACAddress, &Index, FDB_SEARCH_VALID_DYN_ENTRY))
    {
        //set entry non valid
        EDDI_SERSingleCommand(FCODE_CLEAR_FDB_ENTRY, Index * sizeof(EDDI_SER_UCMC_TYPE), 0, pDDB);
        return EDD_STS_OK;
    }

    #if defined (EDDI_CFG_REV7)
    if (EDD_FEATURE_DISABLE != pUsrFDBEntry->Borderline)
    {
        return EDD_STS_ERR_NOT_IMPL;
    }
    #endif

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "SERSetFDBEntry<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERRemoveFDBEntry()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERRemoveFDBEntry( CONST_SER_HANDLE_PTR                                   pSWIPara,
                                                         EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE         pFDBEntryRQB,
                                                         EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB )
{
    LSA_UINT32          Index;
    EDDI_SER_UCMC_TYPE  FDBEntry;
    LSA_RESULT          Result;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERRemoveFDBEntry->");

    //PNIO-MAC-Adressen duerfen nicht geloescht werden
    if ( (pFDBEntryRQB->MACAddress.MacAdr[0] & 0x01) || EDDI_SwiPNIsSTBY1MAC(pDDB, &pFDBEntryRQB->MACAddress) ) //MC-address or MRP-special-address
    {
        if (EDDI_SwiPNIsMACInFDB(pDDB, &pFDBEntryRQB->MACAddress, LSA_FALSE /*bSetInRun*/))
        {
            EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERRemoveFDBEntry, EDDI_STS_ERR_NO_ALLOWED");
            return EDD_STS_ERR_NOT_ALLOWED;
        }
    }

    pFDBEntryRQB->FDBEntryExist = EDDI_SWI_FDB_ENTRY_NOT_EXIST;

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        if (pFDBEntryRQB->MACAddress.MacAdr[0] & SWI_MULTICAST_ADR)
        {           
            EDDI_ENTER_SYNC_S_SPECIAL();
            Result = EDDI_SERRemoveMCFDBEntry(pDDB, pSWIPara, pFDBEntryRQB);
            EDDI_EXIT_SYNC_S_SPECIAL();

            return Result;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)

    // calc index, value range 0..UCMC_Table_Length
    Index = EDDI_SwiUcCalcAdr(&pFDBEntryRQB->MACAddress,
                              (LSA_UINT32)pSWIPara->UCMC_LFSR_Mask,
                              (LSA_UINT32)pSWIPara->UCMC_Table_Length);

    //search for static entry
    Result = EDDI_SwiUcFDBSearchEntry(pDDB, pSWIPara, &pFDBEntryRQB->MACAddress, &Index, FDB_SEARCH_VALID_STATIC_ENTRY);

    //Kein statischer Eintrag vorhanden
    if (EDD_STS_OK_NO_DATA == Result)
    {
        return EDD_STS_OK;
    }

    //error after EDDI_SwiUcFDBSearchEntry
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    //static entry found
    FDBEntry.Value.U32_0 = 0;
    FDBEntry.Value.U32_1 = 0;

    Result = EDDI_SwiUcSetFDBEntry(pSWIPara, &FDBEntry, Index, pDDB);
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    //static FDBEntry exist
    pFDBEntryRQB->FDBEntryExist = EDDI_SWI_FDB_ENTRY_EXISTS;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERRemoveFDBEntry<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERGetFDBSize()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetFDBSize( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB, 
                                                   CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                   LSA_UINT32                   *  pMACSizeTable )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetFDBSize->");

    //value range 0..UCMC_Table_Length
    *pMACSizeTable = pSWIPara->UCMC_Table_Length + 1;

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        //value range 0..MC_Table_Max_Index_All
        LSA_UINT32  const SizeTableMC = pDDB->pConstValues->MC_MaxEntriesPerSubTable * (pDDB->pConstValues->MC_MaxSubTable);
        *pMACSizeTable += SizeTableMC;
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)
    LSA_UNUSED_ARG(pDDB);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERGetFDBEntryMAC()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetFDBEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                         CONST_SER_HANDLE_PTR                                    pSWIPara,
                                                         EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE         pFDBEntry )
{
    LSA_UINT32          Index;
    EDDI_SER_UCMC_TYPE  FDBSearchEntry;
    LSA_RESULT          Result;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetFDBEntryMAC->");

    pFDBEntry->Valid = EDDI_SWI_FDB_ENTRY_INVALID;

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        if (pFDBEntry->MACAddress.MacAdr[0] & SWI_MULTICAST_ADR)
        {
            EDDI_ENTER_SYNC_S_SPECIAL();
            Result = EDDI_SERGetMCFDBEntryMAC(pDDB, pSWIPara, pFDBEntry);
            EDDI_EXIT_SYNC_S_SPECIAL();

            return Result;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)
    
    //calc index, value range 0..UCMC_Table_Length
    Index = EDDI_SwiUcCalcAdr(&pFDBEntry->MACAddress,
                              (LSA_UINT32)pSWIPara->UCMC_LFSR_Mask,
                              (LSA_UINT32)pSWIPara->UCMC_Table_Length);

    //search for static entry
    Result = EDDI_SwiUcFDBSearchEntry(pDDB, pSWIPara, &pFDBEntry->MACAddress, &Index, FDB_SEARCH_VALID_STATIC_ENTRY);

    //no static entry exist?
    if (EDD_STS_OK_NO_DATA == Result)
    {
        return EDD_STS_OK;
    }

    //error after EDDI_SwiUcFDBSearchEntry
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    Result = EDDI_SwiUcGetFDBEntry(pDDB, pSWIPara, &FDBSearchEntry, Index);
    if (EDD_STS_OK != Result)
    {
        return Result;
    }

    EDDI_SwiUcFDBIniGetEntryMAC(pDDB, pFDBEntry, &FDBSearchEntry, Index);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetFDBEntryMAC<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SERGetFDBEntryIndex()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetFDBEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB,
                                                           CONST_SER_HANDLE_PTR                               const  pSWIPara,
                                                           EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE         pFDBEntryRQB )
{
    EDDI_SER_UCMC_TYPE      FDBSearchEntry;
    EDDI_SER_UCMC_PTR_TYPE  pFDBSearchEntry;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetFDBEntryIndex->");

    #if defined (EDDI_CFG_ENABLE_MC_FDB)
    if (pDDB->Glob.LLHandle.bMCTableEnabled)
    {
        LSA_UINT32  LengthTableMC;
        LSA_UINT32  LengthTableUC;

        LengthTableMC = pDDB->pConstValues->MC_MaxEntriesPerSubTable * pDDB->pConstValues->MC_MaxSubTable;

        LengthTableUC = pSWIPara->UCMC_Table_Length + 1;

        if (pFDBEntryRQB->Index >= (LengthTableUC + LengthTableMC))
        {
            return EDD_STS_ERR_PARAM;
        }

        if (pFDBEntryRQB->Index < LengthTableUC)
        {
            //search in UC-table
            pFDBSearchEntry = &pSWIPara->pDev_UCMC_Table_Base[pFDBEntryRQB->Index];

            //read entry from table
            FDBSearchEntry.Value.U32_0 = pFDBSearchEntry->Value.U32_0;
            FDBSearchEntry.Value.U32_1 = pFDBSearchEntry->Value.U32_1;

            EDDI_SwiUcIniFDBGetEntryIndex(pDDB, pFDBEntryRQB, &FDBSearchEntry);

            return EDD_STS_OK;
        }
        else
        {
            //Search in MC table
            LSA_RESULT Result;

            EDDI_ENTER_SYNC_S_SPECIAL();
            Result = EDDI_SERGetMCFDBEntryID(pDDB, pSWIPara, pFDBEntryRQB, pFDBEntryRQB->Index - LengthTableUC);
            EDDI_EXIT_SYNC_S_SPECIAL();

            return Result;
        }
    }
    #endif //(EDDI_CFG_ENABLE_MC_FDB)
    
    //value range 0..UCMC_Table_Length
    if (pFDBEntryRQB->Index > pSWIPara->UCMC_Table_Length)
    {
        EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERGetFDBEntryIndex, Index:0x%X UCMC_Table_Length:0x%X", pFDBEntryRQB->Index, pSWIPara->UCMC_Table_Length);
        return EDD_STS_ERR_PARAM;
    }

    pFDBSearchEntry = &pSWIPara->pDev_UCMC_Table_Base[pFDBEntryRQB->Index];

    //read entry from table
    FDBSearchEntry.Value.U32_0 = pFDBSearchEntry->Value.U32_0;
    FDBSearchEntry.Value.U32_1 = pFDBSearchEntry->Value.U32_1;

    EDDI_SwiUcIniFDBGetEntryIndex(pDDB, pFDBEntryRQB, &FDBSearchEntry);

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERGetFDBEntryIndex<-FDBSearchEntry.Value.U32_0:0x%X :0x%X", FDBSearchEntry.Value.U32_0, FDBSearchEntry.Value.U32_1);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SWISetAgeTime()
*
* function:
*
* parameters:
*
* return value:  LSA_RESULT
*
* comment:
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SWISetAgeTime( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                     EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    LSA_RESULT                              Result;
    EDDI_UPPER_SWI_SET_AGING_TIME_PTR_TYPE  pAgeTime;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWISetAgeTime->");

    pAgeTime = (EDDI_UPPER_SWI_SET_AGING_TIME_PTR_TYPE)pRQB->pParam;

    if (pAgeTime->AgePollTime == pDDB->pLocal_SWITCH->AgePollTime)
    {
        return EDD_STS_OK;
    }

    pDDB->pLocal_SWITCH->AgePollTime = pAgeTime->AgePollTime;

    Result = EDDI_StopTimer(pDDB, pDDB->pLocal_SWITCH->AgePollTimerId);
    switch (Result)
    {
        case EDD_STS_OK:
        case EDDI_STS_OK_TIMER_NOT_RUNNING:
            break;

        default:
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_TIMER);
            EDDI_Excp("EDDI_SWISetAgeTime, 0", EDDI_FATAL_ERR_EXCP, Result, pDDB->pLocal_SWITCH->AgePollTimerId);
            return EDD_STS_ERR_RESOURCE;
    }

    if (pAgeTime->AgePollTime > 0)
    {
        Result = EDDI_StartTimer(pDDB, pDDB->pLocal_SWITCH->AgePollTimerId, pAgeTime->AgePollTime);
        if (Result != EDD_STS_OK)
        {
            EDDI_SET_DETAIL_ERR(pDDB, EDDI_ERR_TIMER);
            EDDI_Excp("EDDI_SWISetAgeTime, 1", EDDI_FATAL_ERR_EXCP, Result, pDDB->pLocal_SWITCH->AgePollTimerId);
            return EDD_STS_ERR_RESOURCE;
        }
    }

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SWISetAgeTime<-AgePollTime:0x%X", pAgeTime->AgePollTime);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SWIGetAgeTime()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SWIGetAgeTime( EDD_UPPER_RQB_PTR_TYPE   const  pRQB,
                                                   EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB )
{
    EDDI_UPPER_SWI_GET_AGING_TIME_PTR_TYPE  pAgeTime;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIGetAgeTime->");

    pAgeTime = (EDDI_UPPER_SWI_GET_AGING_TIME_PTR_TYPE) pRQB->pParam;

    pAgeTime->AgePollTime = pDDB->pLocal_SWITCH->AgePollTime;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIGetAgeTime<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_SWIAgeTimeout()
*
* function:
*
* parameters:
*
* return value:  LSA_VOID
*
* comment:
*==========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SWIAgeTimeout( LSA_VOID  *  const  context )
{
    EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB = (EDDI_LOCAL_DDB_PTR_TYPE)context;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SWIAgeTimeout->");

    if (EDDI_NULL_PTR == context)
    {
        return;
    }

    //e.g. after SHUTDOWN
    if (!pDDB->Glob.HWIsSetup)
    {
        return;
    }

    if (   (EDDI_OPC_AGE_DUMMY_IN_USE == EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB))
        || (EDDI_OPC_AGE_DUMMY_QUEUED == EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB)) )
    {
        //still running or in queue
        return;
    }
    else if (   (EDDI_OPC_AGE_DUMMY_DEQUEUED == EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB))
             || (EDDI_OPC_AGE_DUMMY_UNUSED   == EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB)) )
    {
        //called from SerAsyncConf
    }
    else
    {
        EDDI_Excp("EDDI_SWIAgeTimeout, StateError AGE_DUMMY_RQB",
                  EDDI_FATAL_ERR_EXCP, EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB), 0);
    }

    EDDI_ENTER_COM_S();
    if (pDDB->CmdIF.State != EDDI_CMD_IF_FREE) //Queue not free, enqueue request
    {
        EDDI_EXIT_COM_S();
        if (EDDI_OPC_AGE_DUMMY_DEQUEUED == EDD_RQB_GET_OPCODE(&pDDB->CmdIF.AgeDummyRQB))
        {
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_HIGH, "EDDI_SWIAgeTimeout, Called from SerAsyncConf, but CmdIF not Free. CmdIF.State:0x%X", (LSA_UINT32)pDDB->CmdIF.State);
        }
        EDD_RQB_SET_OPCODE(&pDDB->CmdIF.AgeDummyRQB, EDDI_OPC_AGE_DUMMY_QUEUED);

        EDDI_SERSheduledRequest(pDDB, &pDDB->CmdIF.AgeDummyRQB, (LSA_UINT32)EDDI_SWIAgeTimeout);
        return;
    }

    EDD_RQB_SET_OPCODE(&pDDB->CmdIF.AgeDummyRQB, EDDI_OPC_AGE_DUMMY_IN_USE);
    EDDI_SERAsyncCmd(FCODE_AGE_FDB, pDDB->Glob.LLHandle.AlterAddressTableWait, 0UL, pDDB, &pDDB->CmdIF.AgeDummyRQB, LSA_FALSE /*bLock*/);
    EDDI_EXIT_COM_S();

}
/*---------------------- end [subroutine] ---------------------------------*/


#if defined (EDDI_CFG_REV7)
/***************************************************************************/
/* F u n c t i o n:       EDDI_SwiPNSetBL()                                */
/*                                                                         */
/* D e s c r i p t i o n: Sets/Resets the borderline in mac-address-       */
/*                        ranges. The caller has to make sure that a       */
/*                        borderline is only set for those ports having    */
/*                        both igress and egress filtering!                */
/*                                                                         */
/* A r g u m e n t s:     BLMask   : 1-Bits set the BL, 0-bits reset it    */
/*                        pMACBegin: first mac-adress to be changed        */
/*                                   (corresponds to bit0 of BLMask)       */
/*                        MacLen   : Length of mac-area                    */
/*                                                                         */
/***************************************************************************/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SwiPNSetBL( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                  LSA_UINT32                          BLMask,
                                                  EDD_MAC_ADR_TYPE    const *  const  pMACBegin,
                                                  LSA_UINT32                   const  MacLen )
{
    LSA_RESULT              Status = EDD_STS_ERR_PARAM;
    EDD_MAC_ADR_TYPE        MACAddr;
    LSA_UINT32              MACIndex;
    LSA_UINT32              JumpInIndex;

    if ((MacLen <= 32) && (((LSA_UINT16)pMACBegin->MacAdr[5] + MacLen) < 0x100))  //only byte6 can be incremented
    {
        #if defined (EDDI_CFG_ENABLE_MC_FDB)
        if (   pDDB->Glob.LLHandle.bMCTableEnabled 
            && (pMACBegin->MacAdr[0] & SWI_MULTICAST_ADR) )
        {
            EDDI_ENTER_SYNC_S_SPECIAL();
            EDDI_SERSetBLMC(pDDB, BLMask, pMACBegin, MacLen );
            EDDI_EXIT_SYNC_S_SPECIAL();
            
            Status = EDD_STS_OK;
        }
        else
        #endif //defined (EDDI_CFG_ENABLE_MC_FDB)
        {
            EDDI_SER_UCMC_PTR_TYPE  pEntry;

            //set first MAC-Address
            MACAddr = *pMACBegin;

            //Go through all MAC-Addresses
            for (MACIndex = 0; MACIndex < MacLen; MACIndex++)
            {
                //calc index, valid range: 0..UCMC_Table_Length
                JumpInIndex = EDDI_SwiUcCalcAdr(&MACAddr, pDDB->Glob.LLHandle.UCMC_LFSR_Mask, pDDB->Glob.LLHandle.UCMC_Table_Length);

                /* search static entry */
                Status = EDDI_SwiUcFDBSearchEntry(pDDB, &pDDB->Glob.LLHandle, &MACAddr, &JumpInIndex, FDB_SEARCH_VALID_STATIC_ENTRY);

                if (EDD_STS_OK != Status)
                {
                    //MAC-Address is not contained in FDB-table as static entry!
                    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_FATAL, "EDDI_SwiPNSetBL, MACAdr not in FDB. Status:0x%X", Status);
                    EDDI_Excp("EDDI_SwiPNSetBL, EDDI_SwiUcFDBSearchEntry failed, Status:", EDDI_FATAL_ERR_EXCP, Status, 0);
                    return (Status);
                }

                //MAC-Address is contained in FDB-table as static entry!
                pEntry = &pDDB->Glob.LLHandle.pDev_UCMC_Table_Base[JumpInIndex];

                if //set borderline for this mac-address?
                   (BLMask & 0x1UL)
                {
                    EDDI_SetBitField32(&pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__BLBit, 1); //set BL
                }
                else
                {
                    EDDI_SetBitField32(&pEntry->Value.U32_0, EDDI_SER_UCMC_BIT__BLBit, 0); //clear BL
                }

                BLMask = BLMask >> 1;                       //select next address
                MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 1] += 1; //Next address. increment last byte

            } /* for */
        }
    }

    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/
#endif //defined (EDDI_CFG_REV7)


/*****************************************************************************/
/*  end of file eddi_swi_uc_fdb.c                                            */
/*****************************************************************************/

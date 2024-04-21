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
/*  F i l e               &F: eddi_swi_mc_fdb.c                         :F&  */
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

#include "eddi_int.h"  /* internal header */

#define EDDI_MODULE_ID     M_ID_SWI_MC_FDB
#define LTRC_ACT_MODUL_ID  310

EDDI_FILE_SYSTEM_EXTENSION(EDDI_MODULE_ID)

#if !defined (EDDI_CFG_ENABLE_MC_FDB)

#if defined (M_ID_SWI_MC_FDB) //satisfy lint!
#endif
#else
#include "eddi_dev.h"
#include "eddi_swi_ext.h"  /* internal header */
#include "eddi_swi_ucmc.h" /* internal header */

#define MC_FIND_MAC_ALREADY_EXIST       1UL
#define MC_FIND_MAC_INSERT_AFTER_INDEX  2UL
#define MC_FIND_MAC_INSERT_FIRST        3UL
#define MC_FIND_MAC_INSERT_LAST         4UL

typedef enum EDDI_MCFDB_BL_MODIFIER_TYPE_
{
    EDDI_MCFDB_BL_LEAVE,
    EDDI_MCFDB_BL_SET,
    EDDI_MCFDB_BL_CLEAR
} EDDI_MCFDB_BL_MODIFIER_TYPE;

#define P2_greater_P1 0xFFFFFFFFUL
#define P1_greater_P2 1UL
#define P1_equal_P2   0UL

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCWriteTableHeader( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                                LSA_UINT32               const  MCTabAll_Index,
                                                                LSA_UINT16               const  TabSize_Index,
                                                                LSA_UINT16               const  MC_Tab_Ptr );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetTabKRAMIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                               LSA_UINT32               const  MCTabAll_Index,
                                                               LSA_UINT32                   *  MCTabAll_KRAM_Index );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetTabNewKRAMIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                                  LSA_UINT32               const  MCTabAll_Index,
                                                                  LSA_UINT32                   *  pMCTabAll_KRAM_Index );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MCSetNewMACToSubTable( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                     CONST_SER_HANDLE_PTR                        const  pSWIPara,
                                                                     EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  const  pMC_FDBEntry,
                                                                     LSA_UINT32                            const  MCTabAll_Index );

static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MCRemoveMACFromTable( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                    CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                    EDDI_LOCAL_MAC_ADR_PTR_TYPE         pMAC_Adr,
                                                                    LSA_UINT32                   const  MCTabAll_Index,
                                                                    LSA_UINT8                        *  pFDBEntryExist );

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetMACFromTable( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                               CONST_SER_HANDLE_PTR                       const  pSWIPara,
                                                               EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE const  pMC_FDBEntry,
                                                               EDDI_LOCAL_MAC_ADR_PTR_TYPE          const  pMAC_Adr,
                                                               LSA_UINT32                           const  MCTabAll_Index,
                                                               LSA_BOOL                                 *  pExist,
                                                               LSA_UINT32                               *  pIndex,
                                                               EDDI_MCFDB_BL_MODIFIER_TYPE          const  BLMod);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCWriteMACEntryToTable( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                    CONST_SER_HANDLE_PTR                        const  pSWIPara,
                                                                    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  const  pMC_FDBEntryToInsert,
                                                                    LSA_UINT32                            const  MCTabAll_KRAM_IndexOld,
                                                                    LSA_UINT32                            const  MCTabAll_KRAM_IndexNew,
                                                                    LSA_UINT16                            const  FindResult,
                                                                    LSA_UINT16                            const  FindTabSize_Index,
                                                                    LSA_UINT16                                *  pTabSize_Index );

static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_MCRemoveMACEntryToTable( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                    CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                    EDDI_LOCAL_MAC_ADR_PTR_TYPE         pRemoveMAC_Adr,
                                                                    LSA_UINT32                   const  MCTabAll_KRAM_IndexOld,
                                                                    LSA_UINT32                   const  MCTabAll_KRAM_IndexNew,
                                                                    LSA_UINT16                   const  TabSize_Index );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCClearMACTable( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                          CONST_SER_HANDLE_PTR           const pSWIPara,
                                                          LSA_UINT32               const MCTabAll_KRAM_IndexOld);

static LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_MCFDBSetIniEntry( EDDI_LOCAL_DDB_PTR_TYPE                const pDDB,
                                                             EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE  const pRQBEntry,
                                                             EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE   const p_MC_FDBEntry);

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCFDBIniGetEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB,
                                                                EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE        pRQBEntry,
                                                                EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE         const p_MC_FDBEntry);

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCFDBIniGetEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                   const pDDB,
                                                              EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE       p_MC_RQB_FDBEntry,
                                                              EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE      const p_MC_FDBEntry,
                                                              LSA_UINT32                                const Index);

static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCEDDIFDBCcontrolLpfBit( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB, 
                                                                     EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE          pFDBEntry );

static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCFindMACEntry( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                         CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                         EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMC_FDBEntryToFind,
                                                         LSA_UINT32                   const  MCTabAll_KRAM_Index,
                                                         LSA_UINT16                       *  pFindAction,
                                                         LSA_UINT16                       *  pTabSize_Index );

static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MCCompareMac( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                            EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p1,
                                                            EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p2 );




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
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCEDDIFDBCcontrolLpfBit( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB, 
                                                                     EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE          pFDBEntry )
{
    LSA_UNUSED_ARG(pDDB); //Satisfy lint

    /* set filter bit only if flow prio requested not if PAUSE entry!!! */
    /* When PAUSE entry requested, F-/P-Bit is still set */
    /* (PAUSE entry: typ=1; prio=3; F-/P-Bit=1)*/
    if (!((EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio) == SER_FDB_FLUSS_PRIO) &&
          (!EDDI_GetBitField32(pFDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter))))
    {
        EDDI_SetBitField32(&pFDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter, 0x1);
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_SERSetMCFDBEntry()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetMCFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                        CONST_SER_HANDLE_PTR                                pSWIPara,
                                                        EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE         p_MC_RQB_FDBEntry )
{
    LSA_UINT32                        MCTabAll_Index;
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE  FDBEntry;
    LSA_RESULT                        Status;

    //calc index, value range 0..MC_MaxSubTable
    MCTabAll_Index = EDDI_SwiUcCalcAdr(&p_MC_RQB_FDBEntry->MACAddress,
                                       (LSA_UINT32) pSWIPara->UCMC_LFSR_Mask,
                                       (pDDB->pConstValues->MC_MaxSubTable - 1));
    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERSetMCFDBEntry->MCTabAll_Index:0x%X", MCTabAll_Index);

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1)) 
    {
        EDDI_Excp("EDDI_SERSetMCFDBEntry, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP,
                  MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return EDD_STS_ERR_PARAM;
    }

    Status = EDDI_MCFDBSetIniEntry(pDDB, p_MC_RQB_FDBEntry, &FDBEntry);
    if (EDD_STS_OK != Status)
    {
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_SERSetMCFDBEntry, Error EDDI_MCFDBSetIniEntry, Status:0x%X", Status);
        return Status;
    }

    EDDI_MCEDDIFDBCcontrolLpfBit(pDDB, &FDBEntry);

    //Insert Entry
    Status = EDDI_MCSetNewMACToSubTable(pDDB, pSWIPara, &FDBEntry, MCTabAll_Index);
    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERSetMCFDBEntry<-MCTabAll_Index:0x%X", MCTabAll_Index);
    return Status;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_SERRemoveMCFDBEntry()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERRemoveMCFDBEntry( EDDI_LOCAL_DDB_PTR_TYPE                   const  pDDB,    
                                                           CONST_SER_HANDLE_PTR                                   pSWIPara,
                                                           EDDI_UPPER_SWI_REMOVE_FDB_ENTRY_PTR_TYPE         p_MC_RQB_FDBEntry )
{
    LSA_UINT32  MCTabAll_Index;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERRemoveMCFDBEntry->");

    //calc index, Wertebereich 0..MC_Table_Max_Index_All
    MCTabAll_Index = EDDI_SwiUcCalcAdr(&p_MC_RQB_FDBEntry->MACAddress,
                                       (LSA_UINT32) pSWIPara->UCMC_LFSR_Mask,
                                       (pDDB->pConstValues->MC_MaxSubTable - 1));

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1)) 
    {
        EDDI_Excp("EDDI_SERRemoveMCFDBEntry, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return EDD_STS_ERR_PARAM;
    }  

    p_MC_RQB_FDBEntry->FDBEntryExist = EDDI_SWI_FDB_ENTRY_NOT_EXIST;

    return EDDI_MCRemoveMACFromTable(pDDB, pSWIPara,
                                     &p_MC_RQB_FDBEntry->MACAddress,
                                     MCTabAll_Index,
                                     &p_MC_RQB_FDBEntry->FDBEntryExist);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_SERGetMCFDBEntryMAC()
*----------------------------------------------------------------------------
* PURPOSE  :
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   :
*
*
*
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
LSA_RESULT EDDI_LOCAL_FCT_ATTR EDDI_SERGetMCFDBEntryMAC(EDDI_LOCAL_DDB_PTR_TYPE              const pDDB,
                                                        CONST_SER_HANDLE_PTR                            pSWIPara,
                                                        EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE  p_MC_RQB_FDBEntry )
{
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE  Entry_MAC_Entry;
    LSA_UINT32                        MCTabAll_Index;
    LSA_UINT32                        Index;
    LSA_BOOL                          bExist;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetMCFDBEntryMAC->");

    //calc index, value range 0..MC_Table_Max_Index_All
    MCTabAll_Index = EDDI_SwiUcCalcAdr(&p_MC_RQB_FDBEntry->MACAddress,
                                       (LSA_UINT32) pSWIPara->UCMC_LFSR_Mask,
                                       (pDDB->pConstValues->MC_MaxSubTable - 1));

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1)) 
    {
        EDDI_Excp("EDDI_SERGetMCFDBEntryMAC, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return EDD_STS_ERR_PARAM;
    }  

    p_MC_RQB_FDBEntry->Valid = EDDI_SWI_FDB_ENTRY_INVALID;

    EDDI_MCGetMACFromTable(pDDB, pSWIPara,
                           &Entry_MAC_Entry,
                           &p_MC_RQB_FDBEntry->MACAddress,
                           MCTabAll_Index,
                           &bExist,
                           &Index,
                           EDDI_MCFDB_BL_LEAVE);

    if (!bExist)
    {
        return EDD_STS_OK;
    }

    //Index range of the subtables in front
    Index += MCTabAll_Index * pDDB->pConstValues->MC_MaxEntriesPerSubTable;  

    //Calculate index from collective index
    Index += pSWIPara->UCMC_Table_Length + 1;

    EDDI_MCFDBIniGetEntryMAC (pDDB, p_MC_RQB_FDBEntry, &Entry_MAC_Entry, Index);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_SERGetMCFDBEntryID()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_SERGetMCFDBEntryID( EDDI_LOCAL_DDB_PTR_TYPE                      const  pDDB,
                                                          CONST_SER_HANDLE_PTR                               const  pSWIPara,
                                                          EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE  const  p_MC_FDBEntry,
                                                          LSA_UINT32                                   const  Index )
{
    EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE   TableHeaderOld;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAM;
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE      Entry_MAC_Entry;
    LSA_UINT32                            MCTabAll_KRAM_Index;
    LSA_UINT32                            MCTabAll_Index;
    LSA_UINT32                            LineIndex;
    LSA_UINT16                            TabSize_Index;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_SERGetMCFDBEntryID->Index:0x%X", Index);

    MCTabAll_Index = Index / pDDB->pConstValues->MC_MaxEntriesPerSubTable;  

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1))  
    {
        EDDI_Excp("EDDI_SERGetMCFDBEntryID, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return EDD_STS_ERR_PARAM;
    }  

    p_MC_FDBEntry->Valid = EDDI_SWI_FDB_ENTRY_INVALID;

    LineIndex = Index % pDDB->pConstValues->MC_MaxEntriesPerSubTable; //0..(MC_MaxEntriesPerSubTable-1)
    
    TableHeaderOld.Value = pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value;

    //Sub-Tabelle noch nicht vorhanden
    if (g_MC_Table_Ptr_FREE == EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr))
    {
        return EDD_STS_OK;
    }

    TabSize_Index = EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index);

    //Laenge Sub-Tabelle ueberschritten
    if (LineIndex > TabSize_Index)
    {
        return EDD_STS_OK;
    }

    //Zugeordnete KRAM-Tabelle
    EDDI_MCGetTabKRAMIndex(pDDB, pSWIPara, MCTabAll_Index, &MCTabAll_KRAM_Index);

    pEntry_MAC_KRAM = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_Index].pSubTable_Base_Adress;

    pEntry_MAC_KRAM += LineIndex;

    Entry_MAC_Entry.Value.U32_0 = pEntry_MAC_KRAM->Value.U32_0;
    Entry_MAC_Entry.Value.U32_1 = pEntry_MAC_KRAM->Value.U32_1;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_SERGetMCFDBEntryID, pEntry_MAC_KRAM:0x%X", (LSA_UINT32)pEntry_MAC_KRAM);

    EDDI_MCFDBIniGetEntryIndex(pDDB, p_MC_FDBEntry, &Entry_MAC_Entry);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/

/*=========================================================================*/
/* FUNCTION : EDDI_SERSetBLMC()                                            */
/* D e s c r i p t i o n: Sets/Resets the borderline in mac-address-       */
/*                        ranges. The caller has to make sure that a       */
/*                        borderline is only set for those ports having    */
/*                        both igress and egress filtering!                */
/*                                                                         */
/* A r g u m e n t s:     BLMask   : 1-Bits set the BL, 0-bits reset it    */
/*                        pMACBegin: first mac-adress to be changed        */
/*                                   (corresponds to bit0 of BLMask)       */
/*                        MacLen   : Length of mac-area                    */
/*=========================================================================*/
LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_SERSetBLMC( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                LSA_UINT32                          BLMask,
                                                EDD_MAC_ADR_TYPE    const *  const  pMACBegin,
                                                LSA_UINT32                   const  MacLen )
{
    EDD_MAC_ADR_TYPE                  MACAddr;
    LSA_UINT32                        MACIndex;
    LSA_UINT32                        JumpInIndex;
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE  MC_FDBEntry;
    LSA_BOOL                          bExist;
    LSA_UINT32                        Index;

    //set first MAC-Address
    MACAddr = *pMACBegin;

    //Go through all MAC-Addresses
    for (MACIndex = 0; MACIndex < MacLen; MACIndex++)
    {
        JumpInIndex = EDDI_SwiUcCalcAdr(&MACAddr, pDDB->Glob.LLHandle.UCMC_LFSR_Mask, (pDDB->pConstValues->MC_MaxSubTable - 1));

        if (JumpInIndex > (pDDB->pConstValues->MC_MaxSubTable - 1)) 
        {
            EDDI_Excp("EDDI_SERSetBLMC, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP, JumpInIndex, pDDB->pConstValues->MC_MaxSubTable);
            return;
        }  

        //Get MCFDBEntry
        EDDI_MCGetMACFromTable(pDDB, &pDDB->Glob.LLHandle, &MC_FDBEntry, &MACAddr, JumpInIndex, &bExist, &Index, (BLMask & 0x1UL)?EDDI_MCFDB_BL_SET:EDDI_MCFDB_BL_CLEAR);
        if (!bExist)
        {
            //MAC-Address is not contained in FDB-table as static entry!
            EDDI_Excp("EDDI_SERSetBLMC, MACAdr not in FDB.", EDDI_FATAL_ERR_EXCP, 0, 0);
            return;
        }

        BLMask = BLMask >> 1;                       //select next address
        MACAddr.MacAdr[EDD_MAC_ADDR_SIZE - 1] += 1; //Next address. increment last byte
    } /* for */
}

/*===========================================================================*/
/*                            local function                                 */
/*===========================================================================*/

/*===========================================================================
* FUNCTION : EDDI_MCGetTabKRAMIndex()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetTabKRAMIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                               CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                               LSA_UINT32               const  MCTabAll_Index,
                                                               LSA_UINT32                   *  MCTabAll_KRAM_Index )
{
    LSA_UINT16  Index;

    *MCTabAll_KRAM_Index = g_MC_Table_Index_FREE;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCGetTabKRAMIndex->MCTabAll_Index:0x%X", MCTabAll_Index);

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1))
    {
        EDDI_Excp("EDDI_MCGetTabKRAMIndex, MCTabAll_Index > (MC_MaxSubTable - 1)", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return;
    } 

    //Run through all (+1 for shadow table)
    for (Index = 0; Index < (pDDB->pConstValues->MC_MaxSubTable + 1); Index++)   
    {
        if (MCTabAll_Index == pSWIPara->pMC_SubTableDescr[Index].SubTable_LFSR)
        {
            *MCTabAll_KRAM_Index = Index;

            if (g_MC_Table_Index_FREE == Index)
            {
                EDDI_Excp("EDDI_MCGetTabKRAMIndex, g_MC_Table_Index_FREE", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
            }
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCGetTabKRAMIndex<-Index:0x%X", Index);
            return;
        }
    }
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCGetTabKRAMIndex<-IndexFREE");

    //EDDI_Excp("EDDI_MCGetTabKRAMIndex, table not found", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_MCGetTabNewKRAMIndex()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetTabNewKRAMIndex( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                  CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                                  LSA_UINT32               const  MCTabAll_Index,
                                                                  LSA_UINT32                   *  pMCTabAll_KRAM_Index )
{
    LSA_UINT16  Index;

    *pMCTabAll_KRAM_Index = g_MC_Table_Index_FREE;

    EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCGetTabNewKRAMIndex->MCTabAll_Index:0x%X", MCTabAll_Index);

    if (MCTabAll_Index > (pDDB->pConstValues->MC_MaxSubTable - 1))   
    {
        EDDI_Excp("EDDI_MCGetTabNewKRAMIndex, MCTabAll_Index > MC_MaxSubTable", EDDI_FATAL_ERR_EXCP,
                  MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return;
    }  

    //Alle durchlaufen + 1 Schatten Tabelle
    for (Index = 0; Index < (pDDB->pConstValues->MC_MaxSubTable + 1); Index++)  
    {
        //Gefunden
        if (g_MC_Table_Index_FREE == pSWIPara->pMC_SubTableDescr[Index].SubTable_LFSR)
        {
            pSWIPara->pMC_SubTableDescr[Index].SubTable_LFSR = (LSA_UINT16)MCTabAll_Index;
            *pMCTabAll_KRAM_Index                                                      = Index;

            if (g_MC_Table_Index_FREE == Index)
            {
                EDDI_Excp("EDDI_MCGetTabNewKRAMIndex, g_MC_Table_Index_FREE", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
            }
            EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCGetTabNewKRAMIndex<-Index:0x%X", Index);
            return;
        }
    } 
    
    EDDI_Excp("MCTabAll_Index, table not found", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_MCSetNewMACToSubTable()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MCSetNewMACToSubTable( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                     CONST_SER_HANDLE_PTR                        const  pSWIPara,
                                                                     EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  const  pMC_FDBEntry,
                                                                     LSA_UINT32                            const  MCTabAll_Index )
{
    EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE   TableHeaderOld;
    LSA_UINT16                            TabSize_Index;
    LSA_UINT32                            MCTabAll_KRAM_IndexNew;
    LSA_UINT32                            MCTabAll_KRAM_IndexOld;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_New;
    LSA_UINT16                            FindResult;
    LSA_UINT16                            FindTabSize_Index;

    TableHeaderOld.Value = pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value;
    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCSetNewMACToSubTable->TableHeaderOld.Value:0x%X, MCTabAll_Index:0x%X", TableHeaderOld.Value, MCTabAll_Index);

    //*** Sub-Tabelle noch nicht vorhanden ***
    if (g_MC_Table_Ptr_FREE == EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr))
    {
        TableHeaderOld.Value = 0;

        //Neue Tabelle anfordern
        EDDI_MCGetTabNewKRAMIndex(pDDB, pSWIPara, MCTabAll_Index,  &MCTabAll_KRAM_IndexNew);

        pEntry_MAC_New = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].pSubTable_Base_Adress;

        //MAC-Value schreiben
        // setting valid bit at last write high word first
        pEntry_MAC_New->Value.U32_1 = pMC_FDBEntry->Value.U32_1;
        pEntry_MAC_New->Value.U32_0 = pMC_FDBEntry->Value.U32_0;

        //Tabellenheader akutualisieren
        EDDI_MCWriteTableHeader(pDDB, pSWIPara, MCTabAll_Index, (LSA_UINT16)0,
                                pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].SubTable_Offset);
        EDDI_SWI_TRACE_01(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCSetNewMACToSubTable<-New table:MCTabAll_KRAM_IndexNew:0x%X", MCTabAll_KRAM_IndexNew);
        return EDD_STS_OK;
    }

    //*** Sub-Tabelle bereits vorhanden ***
    EDDI_MCGetTabKRAMIndex(pDDB, pSWIPara, MCTabAll_Index,  &MCTabAll_KRAM_IndexOld);

    TabSize_Index = EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index);

    //Wo der Eintrag eingetragen werden muss
    FindTabSize_Index = EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index);

    EDDI_MCFindMACEntry(pDDB, pSWIPara, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&pMC_FDBEntry->Byte[2],
                        MCTabAll_KRAM_IndexOld, &FindResult, &FindTabSize_Index);

    //Eintrag noch nicht vorhanden
    if (MC_FIND_MAC_ALREADY_EXIST != FindResult)  
    {
        //Maximale Tabellen-Laenge erreicht
        if (TabSize_Index >= (pDDB->pConstValues->MC_MaxEntriesPerSubTable - 1))
        {
            EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_ERROR, "EDDI_MCSetNewMACToSubTable->MCTable 0x%X full (max. 0x%X entries)", 
                MCTabAll_Index, pDDB->pConstValues->MC_MaxEntriesPerSubTable);
            return EDD_STS_ERR_RESOURCE;
        }
    } 

    //Waehrend dem Betrieb muss einen neue Tabelle angelegt werden. Alte + Neuen Eintrag in die Tabelle kopieren
    EDDI_MCGetTabNewKRAMIndex(pDDB, pSWIPara, MCTabAll_Index,  &MCTabAll_KRAM_IndexNew);

    //MAC_Value in die Tabelle schreiben
    EDDI_MCWriteMACEntryToTable(pDDB, pSWIPara, pMC_FDBEntry, MCTabAll_KRAM_IndexOld,
                                MCTabAll_KRAM_IndexNew, FindResult, FindTabSize_Index, &TabSize_Index);

    //Tabellenheader akutualisieren
    EDDI_MCWriteTableHeader(pDDB, pSWIPara, MCTabAll_Index, TabSize_Index,
                            pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].SubTable_Offset);

    //Vorherige KRAM-Tabelle wieder freigeben
    pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].SubTable_LFSR = g_MC_Table_Index_FREE;

    //Tabelleninhalt von alter Tabelle loeschen
    EDDI_MCClearMACTable(pDDB, pSWIPara, MCTabAll_KRAM_IndexOld);

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCSetNewMACToSubTable<-");
    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_MCGetMACFromTable()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCGetMACFromTable( EDDI_LOCAL_DDB_PTR_TYPE              const  pDDB,
                                                               CONST_SER_HANDLE_PTR                       const  pSWIPara,
                                                               EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE const  pMC_FDBEntry,
                                                               EDDI_LOCAL_MAC_ADR_PTR_TYPE          const  pMAC_Adr,
                                                               LSA_UINT32                           const  MCTabAll_Index,
                                                               LSA_BOOL                                 *  pExist,
                                                               LSA_UINT32                               *  pIndex,
                                                               EDDI_MCFDB_BL_MODIFIER_TYPE          const  BLMod)
{
    EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE   TableHeaderOld;
    LSA_UINT32                           RetCompare;
    LSA_UINT32                           MCTabAll_KRAM_IndexOld;
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE      Entry_MAC;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAM;

    *pExist = LSA_FALSE;

    TableHeaderOld.Value = pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value;
    EDDI_SWI_TRACE_04(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCGetMACFromTable->MacAdr[5]:0x%X, MCTabAll_Index:0x%X, BLMod:0x%X, TableHeaderOld.Value:0x%X",
        pMAC_Adr->MacAdr[5], MCTabAll_Index, BLMod, TableHeaderOld.Value);

    if (g_MC_Table_Ptr_FREE == EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr)) //Tabelle nicht vorhanden
    {
        EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCGetMACFromTable->g_MC_Table_Ptr_FREE");
        return;
    }

    //In der Sub-Tabelle suchen
    EDDI_MCGetTabKRAMIndex(pDDB, pSWIPara, MCTabAll_Index, &MCTabAll_KRAM_IndexOld);

    pEntry_MAC_KRAM = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].pSubTable_Base_Adress;

    for (*pIndex = 0; *pIndex <= EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index); (*pIndex)++)
    {
        Entry_MAC = *pEntry_MAC_KRAM;

        RetCompare = EDDI_MCCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&Entry_MAC.Byte[2], pMAC_Adr);

        //Old == New, MAC found
        if (P1_equal_P2 == RetCompare)
        {
            switch (BLMod)
            {
                case EDDI_MCFDB_BL_LEAVE: 
                {
                    pMC_FDBEntry->Value.U32_0 = Entry_MAC.Value.U32_0;
                    pMC_FDBEntry->Value.U32_1 = Entry_MAC.Value.U32_1;
                    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCGetMACFromTable: Entry_MAC.Value.U32_0:0x%X, U32_1:0x%X", Entry_MAC.Value.U32_0, Entry_MAC.Value.U32_1);
                    break;
                }
                case EDDI_MCFDB_BL_SET: 
                {
                    EDDI_SetBitField32(&pEntry_MAC_KRAM->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__BL, 1); //set BL
                    break;
                }
                case EDDI_MCFDB_BL_CLEAR: 
                {
                    EDDI_SetBitField32(&pEntry_MAC_KRAM->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__BL, 0); //clear BL
                    break;
                }
                default:
                {
                    EDDI_Excp("EDDI_MCGetMACFromTable, illegal parameter BLMod", EDDI_FATAL_ERR_EXCP, BLMod, 0);
                }
            }
            
            *pExist                   = LSA_TRUE;
            break;
        }
        pEntry_MAC_KRAM++;
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : SERSetFDBEntry()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MCRemoveMACFromTable( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                    CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                    EDDI_LOCAL_MAC_ADR_PTR_TYPE         pMAC_Adr,
                                                                    LSA_UINT32                   const  MCTabAll_Index,
                                                                    LSA_UINT8                        *  pFDBEntryExist )
{
    EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE  TableHeaderOld;
    LSA_UINT16                          TabSize_Index;
    LSA_UINT16                          FindResult;
    LSA_UINT16                          FindTabSize_Index;
    LSA_UINT32                          MCTabAll_KRAM_IndexNew;
    LSA_UINT32                          MCTabAll_KRAM_IndexOld;

    *pFDBEntryExist      = EDDI_SWI_FDB_ENTRY_NOT_EXIST;

    TableHeaderOld.Value = pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value;

    EDDI_SWI_TRACE_02(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE, "EDDI_MCRemoveMACFromTable->MCTabAll_Index:0x%X TableHeaderOld.Value:0x%X", MCTabAll_Index, TableHeaderOld.Value);

    //Table does not exist (therefor no entry exists)
    if (g_MC_Table_Ptr_FREE == EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr))
    {
        return EDD_STS_OK;
    }

    //Search entry
    EDDI_MCGetTabKRAMIndex(pDDB, pSWIPara, MCTabAll_Index, &MCTabAll_KRAM_IndexOld);

    //Where does the entry has to be placed?
    FindTabSize_Index = EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index);

    EDDI_MCFindMACEntry(pDDB, pSWIPara, pMAC_Adr, MCTabAll_KRAM_IndexOld, &FindResult, &FindTabSize_Index);

    //Entry does not exist
    if (MC_FIND_MAC_ALREADY_EXIST != FindResult)
    {
        return EDD_STS_OK;
    }

    //Entry deleted 
    *pFDBEntryExist = EDDI_SWI_FDB_ENTRY_EXISTS;

    //*** This SubTable contains 1 entry only ***
    if (0 == EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index))
    {
        //Update table header
        EDDI_MCWriteTableHeader(pDDB, pSWIPara, MCTabAll_Index, (LSA_UINT16)0, (LSA_UINT16)g_MC_Table_Ptr_FREE);

        //Release previously used KRAM-table
        pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].SubTable_LFSR = g_MC_Table_Index_FREE;

        //Delete table contens
        EDDI_MCClearMACTable(pDDB, pSWIPara, MCTabAll_KRAM_IndexOld);

        return EDD_STS_OK;
    }

    //*** This SubTable contains more than 1 entry ***

    //A new table has to be setup at runtime. Copy entries-1 to new table
    EDDI_MCGetTabNewKRAMIndex(pDDB, pSWIPara, MCTabAll_Index,  &MCTabAll_KRAM_IndexNew);

    TabSize_Index = EDDI_GetBitField16(TableHeaderOld.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index);

    //Write MAC_Value to table
    EDDI_MCRemoveMACEntryToTable(pDDB, pSWIPara, pMAC_Adr, MCTabAll_KRAM_IndexOld, MCTabAll_KRAM_IndexNew, TabSize_Index);

    //Update table header
    EDDI_MCWriteTableHeader(pDDB, pSWIPara, MCTabAll_Index, (LSA_UINT16)(TabSize_Index - 1),
                            pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].SubTable_Offset);

    //Release previously used KRAM-tablen
    pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].SubTable_LFSR = g_MC_Table_Index_FREE;

    //Delete table contens of old table
    EDDI_MCClearMACTable(pDDB, pSWIPara, MCTabAll_KRAM_IndexOld);

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_MCWriteMACEntryToTable()
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCWriteMACEntryToTable( EDDI_LOCAL_DDB_PTR_TYPE               const  pDDB,
                                                                    CONST_SER_HANDLE_PTR                        const  pSWIPara,
                                                                    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  const  pMC_FDBEntryToInsert,
                                                                    LSA_UINT32                            const  MCTabAll_KRAM_IndexOld,
                                                                    LSA_UINT32                            const  MCTabAll_KRAM_IndexNew,
                                                                    LSA_UINT16                            const  FindResult,
                                                                    LSA_UINT16                            const  FindTabSize_Index,
                                                                    LSA_UINT16                                *  pTabSize_Index )
{
    LSA_UINT16                            Index;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAMOld;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAMNew;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCWriteMACEntryToTable->");

    //MAC in neue Tabelle verschieben
    pEntry_MAC_KRAMOld = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].pSubTable_Base_Adress;
    pEntry_MAC_KRAMNew = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].pSubTable_Base_Adress;

    //New MAC entry is entered as 1st entry
    if (MC_FIND_MAC_INSERT_FIRST == FindResult)
    {
        //Put entry on 1st place
        pEntry_MAC_KRAMNew->Value.U32_0 = pMC_FDBEntryToInsert->Value.U32_0;
        pEntry_MAC_KRAMNew->Value.U32_1 = pMC_FDBEntryToInsert->Value.U32_1;
        pEntry_MAC_KRAMNew++;

        //Then copy the entire table
        for (Index = 0; Index <= *pTabSize_Index; Index++)
        {
            //Write old value
            pEntry_MAC_KRAMNew->Value.U32_0 = pEntry_MAC_KRAMOld->Value.U32_0;
            pEntry_MAC_KRAMNew->Value.U32_1 = pEntry_MAC_KRAMOld->Value.U32_1;
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }

        //Increase table entries by 1
        (*pTabSize_Index)++;

        return;
    }

    if (MC_FIND_MAC_ALREADY_EXIST == FindResult)
    {
        //Copy complete table
        for (Index = 0; Index <= *pTabSize_Index; Index++)
        {
            if (FindTabSize_Index == Index)
            {
                //Overwrite old entry with new one
                pEntry_MAC_KRAMNew->Value.U32_0 = pMC_FDBEntryToInsert->Value.U32_0;
                pEntry_MAC_KRAMNew->Value.U32_1 = pMC_FDBEntryToInsert->Value.U32_1;
            }
            else
            {
                //Write old entry
                pEntry_MAC_KRAMNew->Value.U32_0 = pEntry_MAC_KRAMOld->Value.U32_0;
                pEntry_MAC_KRAMNew->Value.U32_1 = pEntry_MAC_KRAMOld->Value.U32_1;
            }
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }

        return;
    }

    if (MC_FIND_MAC_INSERT_AFTER_INDEX == FindResult)
    {
        //Copy MAC entries in front of new entry
        for (Index = 0; Index <= FindTabSize_Index; Index++)
        {
            //Write old value
            pEntry_MAC_KRAMNew->Value.U32_0 = pEntry_MAC_KRAMOld->Value.U32_0;
            pEntry_MAC_KRAMNew->Value.U32_1 = pEntry_MAC_KRAMOld->Value.U32_1;
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }

        //Insert new entry with new entry
        pEntry_MAC_KRAMNew->Value.U32_0 = pMC_FDBEntryToInsert->Value.U32_0;
        pEntry_MAC_KRAMNew->Value.U32_1 = pMC_FDBEntryToInsert->Value.U32_1;
        pEntry_MAC_KRAMNew++;

        //Copy remaining entries to new entry
        for (Index = (LSA_UINT16) (FindTabSize_Index + (LSA_UINT16)1); Index <= *pTabSize_Index; Index++)
        {
            //Write old values
            pEntry_MAC_KRAMNew->Value.U32_0 = pEntry_MAC_KRAMOld->Value.U32_0;
            pEntry_MAC_KRAMNew->Value.U32_1 = pEntry_MAC_KRAMOld->Value.U32_1;
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }

        //Increase table entries by 1
        (*pTabSize_Index)++;

        return;
    }

    if (MC_FIND_MAC_INSERT_LAST == FindResult)
    {
        //Copy complete table
        for (Index = 0; Index <= *pTabSize_Index; Index++)
        {
            //Write old value
            pEntry_MAC_KRAMNew->Value.U32_0 = pEntry_MAC_KRAMOld->Value.U32_0;
            pEntry_MAC_KRAMNew->Value.U32_1 = pEntry_MAC_KRAMOld->Value.U32_1;
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }

        //Put new entry to end of table
        pEntry_MAC_KRAMNew->Value.U32_0 = pMC_FDBEntryToInsert->Value.U32_0;
        pEntry_MAC_KRAMNew->Value.U32_1 = pMC_FDBEntryToInsert->Value.U32_1;

        //Increase table entries by 1
        (*pTabSize_Index)++;

        return;
    }

    LSA_UNUSED_ARG(pDDB);
    EDDI_Excp("EDDI_MCWriteMACEntryToTable, FindResult not well-known", EDDI_FATAL_ERR_EXCP, FindResult, 0);
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : SERSetFDBEntry
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCFindMACEntry( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                         CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                         EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  pMC_FDBEntryToFind,
                                                         LSA_UINT32                   const  MCTabAll_KRAM_Index,
                                                         LSA_UINT16                       *  pFindAction,
                                                         LSA_UINT16                       *  pTabSize_Index )
{
    LSA_UINT32                            RetCompare;
    LSA_INT16                             Index;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAM;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFindMACEntry->");

    pEntry_MAC_KRAM = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_Index].pSubTable_Base_Adress;

    //Neuer MAC-Eintrag muss steigend eingetragen werden
    for (Index = (LSA_INT16)*pTabSize_Index; Index >= 0; Index--)
    {
        RetCompare = EDDI_MCCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&pEntry_MAC_KRAM[Index].Byte[2], pMC_FDBEntryToFind);

        //MAC-Entry already exists
        if (P1_equal_P2 == RetCompare)
        {
            *pFindAction    = MC_FIND_MAC_ALREADY_EXIST;
            *pTabSize_Index = (LSA_UINT16)Index;
            return;
        }

        if (P2_greater_P1 == RetCompare) //New MAC entry > current MAC entry
        {
            if (Index == *pTabSize_Index) //New MAC entry has to be written at end of table
            {
                *pFindAction    = MC_FIND_MAC_INSERT_LAST;
                *pTabSize_Index = (LSA_UINT16)Index;
                return;
            }
            if (0 == Index)   //New MAC entry has to be written at 1st (2nd) place
            {
                *pFindAction    = MC_FIND_MAC_INSERT_AFTER_INDEX;
                *pTabSize_Index = (LSA_UINT16)Index;
                return;
            }
            //New MAC entry has to be written in between existing entries
            *pFindAction    = MC_FIND_MAC_INSERT_AFTER_INDEX;
            *pTabSize_Index = (LSA_UINT16)Index;
            return;
        }
    }

    //Has to be entered at 1st place, because all other entries are bigger
    *pFindAction    = MC_FIND_MAC_INSERT_FIRST;
    *pTabSize_Index = (LSA_UINT16)Index;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : SERSetFDBEntry
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static LSA_VOID   EDDI_LOCAL_FCT_ATTR EDDI_MCRemoveMACEntryToTable( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                                    CONST_SER_HANDLE_PTR               const  pSWIPara,
                                                                    EDDI_LOCAL_MAC_ADR_PTR_TYPE         pRemoveMAC_Adr,
                                                                    LSA_UINT32                   const  MCTabAll_KRAM_IndexOld,
                                                                    LSA_UINT32                   const  MCTabAll_KRAM_IndexNew,
                                                                    LSA_UINT16                   const  TabSize_Index )
{
    LSA_UINT32                            RetCompare;
    LSA_UINT16                            Index;
    EDDI_SER_MC_TABLE_MAC_ENTRY_TYPE      Entry_MAC;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAMOld;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE  pEntry_MAC_KRAMNew;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW,  "EDDI_MCRemoveMACEntryToTable->");

    //Dislocate MAC to new table
    pEntry_MAC_KRAMOld = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].pSubTable_Base_Adress;
    pEntry_MAC_KRAMNew = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexNew].pSubTable_Base_Adress;

    for (Index = 0; Index <= TabSize_Index; Index++)
    {
        Entry_MAC  = *pEntry_MAC_KRAMOld;
        RetCompare = EDDI_MCCompareMac(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&Entry_MAC.Byte[2], pRemoveMAC_Adr);

        //Old == New, found entry to be dleted, do not copy
        if (P1_equal_P2 == RetCompare)
        {
            pEntry_MAC_KRAMOld++;
        }
        else
        {
            //Write old value
            pEntry_MAC_KRAMNew->Value.U32_0 = Entry_MAC.Value.U32_0;
            pEntry_MAC_KRAMNew->Value.U32_1 = Entry_MAC.Value.U32_1;
            pEntry_MAC_KRAMNew++;
            pEntry_MAC_KRAMOld++;
        }
    }
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : SERSetFDBEntry
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCClearMACTable( EDDI_LOCAL_DDB_PTR_TYPE  const pDDB,
                                                          CONST_SER_HANDLE_PTR           const pSWIPara,
                                                          LSA_UINT32               const MCTabAll_KRAM_IndexOld)
{
    LSA_UINT16                            Index;
    EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE   pEntry_MAC_KRAMOld;

    pEntry_MAC_KRAMOld = pSWIPara->pMC_SubTableDescr[MCTabAll_KRAM_IndexOld].pSubTable_Base_Adress;

    for (Index = 0; Index < pDDB->pConstValues->MC_MaxEntriesPerSubTable; Index++)    
    {
        pEntry_MAC_KRAMOld[Index].Value.U32_0 = 0;
        pEntry_MAC_KRAMOld[Index].Value.U32_1 = 0;
    }  
}
/*---------------------- end [subroutine] ---------------------------------*/


/*===========================================================================
* FUNCTION : EDDI_MCWriteTableHeader
*----------------------------------------------------------------------------
* PURPOSE  : calculate index and set address to fdb
*----------------------------------------------------------------------------
* RETURNS  :
*----------------------------------------------------------------------------
* INPUTS   : *fdbEntry = fdb entry
*            configMode = 1: init without cmd if (only necessary during init)
*                         0: init with cmd if
*            valid      = 0: delete entry, when entry already exist
* OUTPUTS  :
*----------------------------------------------------------------------------
* COMMENTS :
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCWriteTableHeader( EDDI_LOCAL_DDB_PTR_TYPE  const  pDDB,
                                                                CONST_SER_HANDLE_PTR           const  pSWIPara,
                                                                LSA_UINT32               const  MCTabAll_Index,
                                                                LSA_UINT16               const  TabSize_Index,
                                                                LSA_UINT16               const  MC_Tab_Ptr )
{
    EDDI_SER_MC_TABLE_HEADER_ENTRY_TYPE  TableHeader;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCWriteTableHeader->");

    if (MCTabAll_Index >  (pDDB->pConstValues->MC_MaxSubTable - 1) )   
    {
        EDDI_Excp("EDDI_MCWriteTableHeader, MCTabAll_Index > MC_MaxSubTable - 1", EDDI_FATAL_ERR_EXCP, MCTabAll_Index, pDDB->pConstValues->MC_MaxSubTable);
        return;
    }    

    if (TabSize_Index > 0x000F)
    {
        EDDI_Excp("EDDI_MCWriteTableHeader, TabSize_Index > 0x000F", EDDI_FATAL_ERR_EXCP, TabSize_Index, 0);
        return;
    }

    if (MC_Tab_Ptr > 0x1FF)
    {
        EDDI_Excp("EDDI_MCWriteTableHeader, MC_Tab_Ptr > 0x1FF", EDDI_FATAL_ERR_EXCP, MC_Tab_Ptr, 0);
        return;
    }

    TableHeader.Value = pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value;

    //Update table header
    EDDI_SetBitField16(&TableHeader.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__Res,  0);
    EDDI_SetBitField16(&TableHeader.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__MC_Tab_Ptr    , MC_Tab_Ptr);
    EDDI_SetBitField16(&TableHeader.Value, EDDI_SER_MC_TABLE_HEADER_ENTRY_BIT__TabSize_Index , TabSize_Index);

    pSWIPara->pDev_MC_Table_Base[MCTabAll_Index].Value = TableHeader.Value;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_MCFDBSetIniEntry()
*
* function:      set fdb entry to hardware
*
* parameters:    pDDB     : Pointer to DeviceDescriptionBlock
*                fdbEntry : fdb entry
*                index    : table offset
*
* return value:
*
* comment:       write fdb 64 Bit value to harware
*
*==========================================================================*/
static  LSA_RESULT  EDDI_LOCAL_FCT_ATTR  EDDI_MCFDBSetIniEntry( EDDI_LOCAL_DDB_PTR_TYPE                const  pDDB,
                                                                EDDI_UPPER_SWI_SET_FDB_ENTRY_PTR_TYPE  const  pRQBEntry,
                                                                EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE   const  pMC_FDBEntry )
{
    LSA_UINT8   FDBDestPort = 0;
    LSA_UINT8   FDBPrio = 0;
    LSA_RESULT  Status;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBSetIniEntry->");

    pMC_FDBEntry->Value.U32_0 = 0;
    pMC_FDBEntry->Value.U32_1 = 0;

    EDDI_SwiUcSetFDBDestPort(pDDB, pRQBEntry->CHA, pRQBEntry->CHB, &pRQBEntry->PortID[0], &FDBDestPort);

    Status = EDDI_SwiUcSetFDBPrio(pDDB, pRQBEntry->Prio, &FDBPrio);
    if (EDD_STS_OK != Status)
    {
        return Status;
    }

    EDDI_MEMCOPY(&pMC_FDBEntry->Byte[2], &pRQBEntry->MACAddress, EDD_MAC_ADDR_SIZE);

    EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DestPort, FDBDestPort);

    EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio, FDBPrio);

    
    //DCP
    if (EDDI_SwiPNIsDCPMAC(pDDB, &pRQBEntry->MACAddress))
    {
        EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DCP, 1);
    }
    else
    {
        EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DCP, 0);
    }

    //Pause
    EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter,
                        (EDD_FEATURE_DISABLE != pRQBEntry->Pause) ? 1 : 0);

    //Borderline
    #if defined (EDDI_CFG_REV7)
    if (EDD_FEATURE_DISABLE != pRQBEntry->Borderline)
    {
        EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__BL, 1);
    }
    else if (EDD_FEATURE_DISABLE != pRQBEntry->Filter)
    {
        EDDI_SetBitField32(&pMC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__BL, 0);
    }
    #endif

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBSetIniEntry<-");

    return EDD_STS_OK;
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_MCFDBIniGetEntryMAC()
*
* function:  set fdb entry to hardware
*
* parameters:  pDDB     : Pointer to DeviceDescriptionBlock
*     fdbEntry : fdb entry
*     index    : table offset
*
* return value:
*
* comment:   write fdb 64 Bit value to harware
*
*==========================================================================*/
static  LSA_VOID  EDDI_LOCAL_FCT_ATTR  EDDI_MCFDBIniGetEntryMAC( EDDI_LOCAL_DDB_PTR_TYPE                    const  pDDB,
                                                                 EDDI_UPPER_SWI_GET_FDB_ENTRY_MAC_PTR_TYPE         p_MC_RQB_FDBEntry,
                                                                 EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE       const  p_MC_FDBEntry,
                                                                 LSA_UINT32                                 const  Index )
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBIniGetEntryMAC->");

    //Port
    EDDI_SwiUcGetFDBDestPort(pDDB,
                             (LSA_UINT8)EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DestPort),
                             &p_MC_RQB_FDBEntry->CHA,
                             &p_MC_RQB_FDBEntry->CHB,
                             &p_MC_RQB_FDBEntry->PortID[0]);

    //Prio
    EDDI_SwiUcGetFDBPrio(pDDB, (LSA_UINT8)EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio),
                         &p_MC_RQB_FDBEntry->Prio);

    //MAC-Address
    EDDI_SwiUcGetFDBAdress(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&p_MC_FDBEntry->Byte[2], &p_MC_RQB_FDBEntry->MACAddress);

    //Type
    p_MC_RQB_FDBEntry->Type   = EDDI_SWI_FDB_ENTRY_STATIC;

    p_MC_RQB_FDBEntry->Valid  = EDDI_SWI_FDB_ENTRY_VALID;

    //Index
    p_MC_RQB_FDBEntry->Index  = Index;

    //Pause / Filter
    if (0 == EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter))
    {
        p_MC_RQB_FDBEntry->Pause  = EDD_FEATURE_DISABLE;
        p_MC_RQB_FDBEntry->Filter = EDD_FEATURE_DISABLE;
    }
    else
    {
        if (EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio) == SER_FDB_FLUSS_PRIO)
        {
            p_MC_RQB_FDBEntry->Pause  = EDD_FEATURE_ENABLE;
            p_MC_RQB_FDBEntry->Filter = EDD_FEATURE_DISABLE;
        }
        else
        {
            p_MC_RQB_FDBEntry->Pause  = EDD_FEATURE_DISABLE;
            p_MC_RQB_FDBEntry->Filter = EDD_FEATURE_ENABLE;
        }
    }

    #if defined (EDDI_CFG_REV7)
    p_MC_RQB_FDBEntry->Borderline = ((LSA_UINT8)EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__BL))?EDD_FEATURE_ENABLE:EDD_FEATURE_DISABLE;
    #else
    p_MC_RQB_FDBEntry->Borderline = EDD_FEATURE_DISABLE;
    #endif

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBIniGetEntryMAC<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: FDBIniGetEntry
*
* function:  set fdb entry to hardware
*
* parameters:  pDDB     : Pointer to DeviceDescriptionBlock
*     fdbEntry : fdb entry
*     index    : table offset
*
* return value:
*
* comment:   write fdb 64 Bit value to harware
*
*==========================================================================*/
static LSA_VOID EDDI_LOCAL_FCT_ATTR EDDI_MCFDBIniGetEntryIndex( EDDI_LOCAL_DDB_PTR_TYPE                      const pDDB,
                                                                EDDI_UPPER_SWI_GET_FDB_ENTRY_INDEX_PTR_TYPE        pRQBEntry,
                                                                EDDI_SER_MC_TABLE_MAC_ENTRY_PTR_TYPE         const p_MC_FDBEntry)
{
    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBIniGetEntryIndex->");

    //Port
    EDDI_SwiUcGetFDBDestPort(pDDB,
                             (LSA_UINT8)EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__DestPort),
                             &pRQBEntry->CHA,
                             &pRQBEntry->CHB,
                             &pRQBEntry->PortID[0]);

    //Prio
    EDDI_SwiUcGetFDBPrio(pDDB, (LSA_UINT8)EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio),
                         &pRQBEntry->Prio);

    //MAC-Address
    EDDI_SwiUcGetFDBAdress(pDDB, (EDDI_LOCAL_MAC_ADR_PTR_TYPE)(void *)&p_MC_FDBEntry->Byte[2], &pRQBEntry->MACAddress);

    //Type
    pRQBEntry->Type    = EDDI_SWI_FDB_ENTRY_STATIC;

    pRQBEntry->Valid   = EDDI_SWI_FDB_ENTRY_VALID;

    //Pause / Filter
    if (0 == EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__PauseFilter))
    {
        pRQBEntry->Pause  = EDD_FEATURE_DISABLE;
        pRQBEntry->Filter = EDD_FEATURE_DISABLE;
    }
    else
    {
        if (EDDI_GetBitField32(p_MC_FDBEntry->Value.U32_0, EDDI_SER_MC_TABLE_MAC_ENTRY_BIT__Prio) == SER_FDB_FLUSS_PRIO)
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

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCFDBIniGetEntryIndex<-");
}
/*---------------------- end [subroutine] ---------------------------------*/


/*=============================================================================
* function name: EDDI_MCCompareMac()
*
* function:
*
* parameters:
*
* return value:  LSA_UINT32
*
* comment:
*==========================================================================*/
static  LSA_UINT32  EDDI_LOCAL_FCT_ATTR  EDDI_MCCompareMac( EDDI_LOCAL_DDB_PTR_TYPE      const  pDDB,
                                                            EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p1,
                                                            EDDI_LOCAL_MAC_ADR_PTR_TYPE  const  p2 )
{
    LSA_INT32   Index;
    LSA_UINT32  retVal = P1_equal_P2;

    EDDI_SWI_TRACE_00(pDDB->Glob.TraceIdx, LSA_TRACE_LEVEL_NOTE_LOW, "EDDI_MCCompareMac->");

    for (Index = EDD_MAC_ADDR_SIZE - 1;  Index >= 0; Index--)
    {
        if (p1->MacAdr[Index] == p2->MacAdr[Index])
        {
            continue;
        }

        if (p1->MacAdr[Index] < p2->MacAdr[Index])
        {
            retVal = P2_greater_P1;
        }
        else
        {
            retVal = P1_greater_P2;
        }

        break;
    }

    LSA_UNUSED_ARG(pDDB);
    return retVal;
}
/*---------------------- end [subroutine] ---------------------------------*/

#endif //!defined (EDDI_CFG_ENABLE_MC_FDB)


/*****************************************************************************/
/*  end of file eddi_swi_mc_fdb.c                                            */
/*****************************************************************************/


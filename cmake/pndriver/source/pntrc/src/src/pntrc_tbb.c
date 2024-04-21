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
/*  C o m p o n e n t     &C: PNTRC (PN Trace)                          :C&  */
/*                                                                           */
/*  F i l e               &F: pntrc_tbb.c                               :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNRUN_P07.01.00.00_00.02.00.15         :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-08-05                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n:                                                   */
/*    TBB = Two Buff Binary                                                  */
/*  LSA-Trace                                                                */
/*                                                                           */
/*****************************************************************************/

/*===========================================================================*/
/*                                 module-buffer_id                          */
/*===========================================================================*/

#define PNTRC_MODULE_ID   2
#define LTRC_ACT_MODUL_ID 2

/*===========================================================================*/
/*                                 includes                                  */
/*===========================================================================*/
#include "pntrc_inc.h"
#include "pntrc_trc.h"
#include "pntrc_int.h"
#include "pntrc_tbb.h"

#if(EPS_PLF == EPS_PLF_WINDOWS_X86)
    #include <stddef.h> // bcs of offsetof()
#endif


/*===========================================================================*/
/*                           sys structs/ unions                             */
/*===========================================================================*/

/*===========================================================================*/
/*                                global data                                */
/*===========================================================================*/

/*===========================================================================*/
/*                              local functions                              */
/*===========================================================================*/


/*===========================================================================*/
/*                              user functions                               */
/*===========================================================================*/

#define PNTRC_LITTLE_ENDIAN 0

#define PNTRC_TIMESTAMP_UNIT_NS 0

#define PNTRC_MAX_BYTE_ARRAY_LENGTH 1016 //because ParCount is a byte (255 values) in the trace header

/**
 * Initializes a buffer header with the parameters passed to this function.
 * 
 * @param [in] Offset       - Offset within the trace buffer
 * @param [in] Id           - Identifier
 * @param [in] Size         - Size of this header
 * @param [in] FirstBlock     ??
 * @param [in] MaxTraceBlocks ??
 * @return
 */
static LSA_VOID pntrc_ttb_init_buffer_header(LSA_UINT32 Offset, LSA_UINT32 Id, LSA_UINT32 Size, LSA_UINT32 FirstBlock, LSA_UINT32 MaxTraceBlocks)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE pHeader;
    LSA_UINT32 md5Array[4];
    pHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)(LSA_VOID_PTR_TYPE)((LSA_UINT8*)g_pPntrcData->TraceMem+Offset);

    pHeader->MagicNumber=TRACE_BUF_MAGICNUMBER;
    pHeader->ByteWidth=4;
    pHeader->FirstBlock=FirstBlock;
    pHeader->Id=Id;
    pHeader->MaxBlockCount=MaxTraceBlocks;
    pHeader->Options=PNTRC_LITTLE_ENDIAN;
    pHeader->Size=Size;
    pHeader->TimeStampUnit=PNTRC_TIMESTAMP_UNIT_NS;
    pHeader->TimeStampWidth=8;
    pHeader->CurBlockCount=0;
    pHeader->Flags=0;
    pHeader->Reserved2=0;

#ifdef PNTRC_CFG_CHECK_SCANNERINFO
    // Get the MD5 Checksum of the tracemap. This information is generated by the ltrcscanner in the build process (pntrc_checksum.c)
    pntrc_get_md5sum_scannerinfo(md5Array);
    pHeader->TraceMap_MD5isUsed = 1;
    pHeader->TraceMap_MD5_1=md5Array[0];
    pHeader->TraceMap_MD5_2=md5Array[1];
    pHeader->TraceMap_MD5_3=md5Array[2];
    pHeader->TraceMap_MD5_4=md5Array[3];
#else
    pHeader->TraceMap_MD5isUsed = 0;
    pHeader->TraceMap_MD5_1=0x0; // Dummy value
    pHeader->TraceMap_MD5_2=0x0; // Dummy value
    pHeader->TraceMap_MD5_3=0x0; // Dummy value
    pHeader->TraceMap_MD5_4=0x0; // Dummy value
    LSA_UNUSED_ARG(md5Array);
#endif
}

/**
 * Sets a key value within the Master MIF and in all slave MIFs
 * @param [in] ofs      - offset of the key within the MIF
 * @param [in] value    - value of the key
 * @return
 */
static LSA_VOID pntrc_tbb_set_mifs_value(LSA_UINT32 ofs, LSA_UINT32 value)
{
    LSA_UINT8 i;

    *(LSA_UINT32*)(LSA_VOID_PTR_TYPE) ((LSA_UINT8*)(LSA_VOID_PTR_TYPE)(&g_pPntrcData->TraceMem->MasterMIF)+ofs)=value;
    for (i=0; i<PNTRC_MAX_SLAVE_MIF; i++)
    {
        *(LSA_UINT32*)(LSA_VOID_PTR_TYPE) ((LSA_UINT8*)(LSA_VOID_PTR_TYPE)(&g_pPntrcData->TraceMem->SlaveMif[i])+ofs)=value;
    }
}

/**
 * @brief Initializes all MIFs (Master MIF and all Slave MIFs) for the given parameters
 * 
 * This function takes a Start pointer and a given Size and divides this memory into two trace blocks (Twin Buffer 1 and 2).
 * Memory layout: 
 * (1) Twin buffer 1 (BuffersDesc[CPUNr][Category].OffsetBuffer[0]) -> Offset = Start + Alignment
 *   PNTRC_BUFFER_HEADER_TYPE                   - Header of buffer 1
 *   MaxTraceBlocks * PNTRC_BUFFER_ENTRY_TYPE   - Buffers
 * (2) Twin buffer 2 (BuffersDesc[CPUNr][Category].OffsetBuffer[1]) -> Offset = Start + (Size/2) + Alignment
 *   PNTRC_BUFFER_HEADER_TYPE                   - Header of buffer 2
 *   MaxTraceBlocks * PNTRC_BUFFER_ENTRY_TYPE   - Buffers
 * 
 * Both Twin Buffers have a header (Size = FirstBlock) and the same size
 * 
 * @param [in] CPUNr        - index of the CPU
 * @param [in] Category     - index of the category
 * @param [in] Start        - Start offset for the trace buffer
 * @param [in] Size         - total size of the trace buffer for this category
 * @return
 */
static LSA_VOID pntrc_tbb_init_category(LSA_UINT32 CPUNr, LSA_UINT32 Category, LSA_UINT32 Start, LSA_UINT32 Size)
{
    LSA_UINT32 MaxTraceBlocks;  /* maximum number of trace blocks in the buffer */
    LSA_UINT32 UsedSize;
    LSA_UINT32 HalfSize;
    LSA_UINT32 Offset0,Offset1;
    LSA_UINT32 AdjustedSize;
    LSA_UINT32 FirstBlock;
    LSA_UINT32 End;
    
    Offset0=((Start+3)/4)*4; //round up to 32bit boundary
    End=Start+Size-1;
    AdjustedSize=(End-Offset0)+1;
    AdjustedSize=(AdjustedSize/8)*8; //round down, adjustedSize is now a multiple of 64 bit
    HalfSize = AdjustedSize / 2; // HalfSize is now a multiple of 32 bit
    Offset1 = Offset0 + HalfSize;
    
    /* calculate valid trace buffer size */
    FirstBlock=sizeof(PNTRC_BUFFER_HEADER_TYPE);
    MaxTraceBlocks = (HalfSize - sizeof(PNTRC_BUFFER_HEADER_TYPE) ) / sizeof (PNTRC_BUFFER_ENTRY_TYPE);
    UsedSize = sizeof(PNTRC_BUFFER_HEADER_TYPE) + sizeof(PNTRC_BUFFER_ENTRY_TYPE) * MaxTraceBlocks;
    
    //set values in MIF
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].OffsetBuffer[0]),Offset0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].SizeBuffer[0]),UsedSize);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].StateBuffer[0]),PNTRC_BUFFER_STATE_EMPTY); // was PNTRC_BUFFER_STATE_FILLING
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].OffsetBuffer[1]),Offset1);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].SizeBuffer[1]),UsedSize);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].StateBuffer[1]),PNTRC_BUFFER_STATE_EMPTY);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][Category].CurBufferNr),0);

    pntrc_ttb_init_buffer_header(Offset0, ((g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU & 1) << 9) | Category, UsedSize, FirstBlock, MaxTraceBlocks);
    pntrc_ttb_init_buffer_header(Offset1, ((g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU & 1) << 9) | Category | 0x100, UsedSize, FirstBlock, MaxTraceBlocks);
    
    g_pPntrcData->FirstTraceEntry[Category][0].data.header.Subsys=0; //indicate that buffers have been never used
    g_pPntrcData->FirstTraceEntry[Category][1].data.header.Subsys=0; //indicate that buffers have been never used

}

/**
 * Gets the buffer header from the MIF
 * 
 * @param [in] Mif          - MIF of CPU instance
 * @param [in] CPUNr        - index of CPUNr
 * @param [in] Category     - index of Category
 * @param [in] BufferNr     - index of Buffer 
 * @return PNTRC_BUFFER_HEADER_PTR_TYPE - Pointer to the header
 */
static PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category, LSA_UINT8 BufferNr)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
    LSA_UINT32 Offset;

    Offset=Mif->BuffersDesc[CPUNr][Category].OffsetBuffer[BufferNr];
    Ptr=(PNTRC_BUFFER_HEADER_PTR_TYPE)(LSA_VOID_PTR_TYPE)((LSA_UINT8*)(Mif) + Offset);
    return Ptr;
}

/**
 * Returns the current header where the next trace entry should be stored into
 * 
 * @param [in] Mif      - MIF of the CPU instance
 * @param [in] CPUNr    - Index of the CPUNr
 * @param [in] Category - Index of the category
 * @return PNTRC_BUFFER_HEADER_PTR_TYPE - Pointer to the header
 */
static PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_cur_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
    Ptr=pntrc_get_buffer_header(Mif,CPUNr,Category,(LSA_UINT8)Mif->BuffersDesc[CPUNr][Category].CurBufferNr);
    return Ptr;
}

/**
 * Returns the other twin buffer that is currently not in use
 * Calculation is done by 1 - CurBufferNr 
 * -> CurBufferNr = 1 -> Index = 1-1 = 0
 * -> CurBufferNr = 0 -> Index = 1-0 = 1
 * @param [in] Mif      - MIF of the CPU instance
 * @param [in] CPUNr    - Index of the CPUNr
 * @param [in] Category - Index of the category
 * 
 * @return PNTRC_BUFFER_HEADER_PTR_TYPE - Pointer to the header
 */
static PNTRC_BUFFER_HEADER_PTR_TYPE pntrc_get_other_buffer_header(PNTRC_MIF_PTR_TYPE Mif, LSA_UINT8 CPUNr, LSA_UINT8 Category)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE Ptr;
    Ptr=pntrc_get_buffer_header(Mif,CPUNr,Category,(LSA_UINT8)(1-Mif->BuffersDesc[CPUNr][Category].CurBufferNr));
    return Ptr;
}

/**
 * Returns the offset of a trace block within the category (calculated by CurBlockCount) for the local CPU.
 * 
 * The trace block is calculated by ptr = Header + CurBlockCount * PNTRC_BUFFER_ENTRY_TYPE
 * 
 * @param [in] Category                 - Category
 * @param [in] FirstBlock               - Offset of the header value, @see pntrc_tbb_init_category     
 * @param [in] CurBlockCount            - Current block
 * @return PNTRC_BUFFER_ENTRY_PTR_TYPE  - Pointer to the trace buffer
 */
static PNTRC_BUFFER_ENTRY_PTR_TYPE pntrc_get_cur_trace_block(LSA_UINT8 Category, LSA_UINT32 FirstBlock, LSA_UINT32 CurBlockCount)
{
    PNTRC_BUFFER_ENTRY_PTR_TYPE Ptr;
    Ptr=(PNTRC_BUFFER_ENTRY_PTR_TYPE)(LSA_VOID_PTR_TYPE)((LSA_UINT8*)(LSA_VOID_PTR_TYPE)pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category)+FirstBlock+CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE));
    return Ptr;
}

/**
 * Returns the current trace block (calculated by CurBufferNr) for the local CPU.
 * 
 * @param [in] Category - Category
 * @return
 */
static PNTRC_BUFFER_ENTRY_PTR_TYPE pntrc_get_cur_first_trace_entry_save(LSA_UINT8 Category)
{
    PNTRC_BUFFER_ENTRY_PTR_TYPE Ptr;
    Ptr=(PNTRC_BUFFER_ENTRY_PTR_TYPE)&g_pPntrcData->FirstTraceEntry[Category][g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].CurBufferNr];
    return Ptr;
}

#ifdef PNTRC_CFG_NEW_TIMESYNC_MODE
/**
 * Triggers that the lower CPU does a time sync mechanism by writing trace values into the buffer
 * Usecase (1) - Legacy mode:                   This CPU gets the current trace time and stores this value into the lower MIF
 * Usecase (2) - PNTRC_CFG_NEW_TIMESYNC_MODE:   Here, only the boolean within the MIF is written. The timestamp is discarded in the lower CPU. 
 * 
 * Since both instances may be configured differently, the timestamp is written in both cases.
 * @param [in] hLowerDev - Handle to the lower CPU
 * @return
 */
static LSA_VOID pntrc_tbb_send_sync_time(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev)
{
    PNTRC_MIF_TYPE Mif;
    LSA_UINT32 stamp_low,stamp_high;
    LSA_UINT16 resp;

    // Get current time from system adaptation
    PNTRC_GET_TRACESTAMP(&stamp_low,&stamp_high);
    Mif.TimeSync.SyncTimeLow=stamp_low;
    Mif.TimeSync.SyncTimeHigh=stamp_high;
    Mif.TimeSync.NewSyncTime=1;
    // Update the MIF of the lower CPU
    resp = PNTRC_WRITE_LOWER_CPU(hLowerDev,(LSA_UINT8*)(&Mif.TimeSync),PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,TimeSync),sizeof(Mif.TimeSync));
    PNTRC_ASSERT(resp == LSA_RET_OK);
}
#endif // PNTRC_CFG_NEW_TIMESYNC_MODE

/**
 * Initializes all MIFs with startup parameters passed in the structure pInit 
 * @param [in] pInit  - Init structure passed by the system adaptation
 * @return
 */
LSA_VOID pntrc_tbb_init(const PNTRC_INIT_TYPE *pInit)
{
    LSA_UINT32 idxLowerCpu, idxLowerLowerCpu, idxLowerCategory, idxLowerTracebuffer;
    LSA_UINT32 SizePerCategory;
    
    #ifdef ZERO_TRACE_BUFFERS
    PNTRC_MEMSET_LOCAL_MEM(pInit->Ptr,0,pInit->Size);
    #endif
    
    g_pPntrcData->TraceMem=(PNTRC_TRACE_MEM_PTR_TYPE)(LSA_VOID_PTR_TYPE)(pInit->Ptr);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,MagicNumber),TRACE_MEM_MAGICNUMBER);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Ready),0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,AnyBufferFull),0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,FatalOccurred),0);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.MajorVersion),1);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.MinorVersion),0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.Endiness),0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,General.Padding),0);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.MaxCategoryCount),PNTRC_MAX_CATEGORY_COUNT);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.MaxLowerCPUCount),PNTRC_CFG_MAX_LOWER_CPU_CNT);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.IsTopMostCPU),pInit->bIsTopMostCPU);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config.TraceSubSysNum),TRACE_SUBSYS_NUM);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,ForceBufferSwitch),0);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,TimeSync.NewSyncTime),0);
    
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),0);
    pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewReadTraceLevels),0);
    
    g_pPntrcData->bufferSwitchInProgress=LSA_FALSE;
    
    SizePerCategory = ((pInit->Size - (1+PNTRC_MAX_SLAVE_MIF)*sizeof(PNTRC_MIF_TYPE) ) / PNTRC_MAX_CATEGORY_COUNT) / (1+PNTRC_CFG_MAX_LOWER_CPU_CNT);
    for (idxLowerLowerCpu =0; idxLowerLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerLowerCpu++)
    {
        for (idxLowerCategory =0; idxLowerCategory<PNTRC_MAX_CATEGORY_COUNT; idxLowerCategory++)
        {
            pntrc_tbb_init_category(idxLowerLowerCpu, idxLowerCategory,(1+PNTRC_MAX_SLAVE_MIF)*sizeof(PNTRC_MIF_TYPE) + idxLowerLowerCpu*(SizePerCategory*PNTRC_MAX_CATEGORY_COUNT)+ idxLowerCategory*SizePerCategory,SizePerCategory);
        }
    }

    /// Set all local tracebuffer pointers to NULL
    // for all lower cpus of this instance
    for (idxLowerCpu = 0; idxLowerCpu < PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        // for all lower cpus of each lower cpu (used if there is a middle cpu / there are more that 2 levels in the tree)
        for (idxLowerLowerCpu = 0; idxLowerLowerCpu < 1 + PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerLowerCpu++)
        {
            // for all categories
            for (idxLowerCategory = 0; idxLowerCategory < PNTRC_MAX_CATEGORY_COUNT; idxLowerCategory++)
            {
                // for all buffers
                for (idxLowerTracebuffer = 0; idxLowerTracebuffer < PNTRC_TWINBUF_NUMBER; idxLowerTracebuffer++)
                {
                    g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].pTraceBuffer[idxLowerLowerCpu][idxLowerCategory][idxLowerTracebuffer] = 0;
                }
            }
        }
    }
    g_pPntrcData->UseInitLogLevels=pInit->bUseInitLogLevels;
}

/**
 * Saves all buffers, frees all allocated buffers.
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_tbb_uninit(LSA_VOID)
{
    LSA_UINT32 idxLowerCpu, idxLowerLowerCpu, idxLowerCategory, idxLowerTracebuffer;
    LSA_UINT16 rc;
    
    pntrc_tbb_save_all_buffers(LSA_FALSE);
    
    /// Free all local trace buffers that are still allocated

    // for all lower cpu of this instance
    for (idxLowerCpu = 0; idxLowerCpu < PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        // for all lower cpus of this specific lower cpu (lowerLowerCpu)
        for (idxLowerLowerCpu = 0; idxLowerLowerCpu < 1 + PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerLowerCpu++)
        {
            // for all categories
            for (idxLowerCategory = 0; idxLowerCategory < PNTRC_MAX_CATEGORY_COUNT; idxLowerCategory++)
            {
                // for both tracebuffers
                for (idxLowerTracebuffer = 0; idxLowerTracebuffer < PNTRC_TWINBUF_NUMBER; idxLowerTracebuffer++)
                {
                    // only free the memory if the pointer is != NULL
                    if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].pTraceBuffer[idxLowerLowerCpu][idxLowerCategory][idxLowerTracebuffer] != 0)
                    {
                        PNTRC_FREE_LOCAL_MEM(&rc, g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].pTraceBuffer[idxLowerLowerCpu][idxLowerCategory][idxLowerTracebuffer]);
                        PNTRC_ASSERT(rc == LSA_RET_OK);
                        g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].pTraceBuffer[idxLowerLowerCpu][idxLowerCategory][idxLowerTracebuffer] = 0;
                    }
                }
            }
        }
    }
}

/**
 * Updates the MIF -> PNTRC is ready to trace. The upper PNTRC polls this value.
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_tbb_set_ready(LSA_VOID)
{
  PNTRC_ASSERT(g_pPntrcData->TraceMem != LSA_NULL);
  pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Ready),PNTRC_MIF_READY);
}

/**
 * Sets the log levels for the lower CPU
 * @param [in] hLowerDev        - handle to the lower CPU
 * @param [in] pLogLevels       - Array with the Size 8 * TRACE_SUBSYS_NUM. This array contains the log level for each subsystem
 * @return
 */
static LSA_VOID pntrc_tbb_set_log_levels_lower_cpu(PNTRC_CPU_HANDLE_PTR_TYPE hLowerDev, const LSA_UINT8* pLogLevels)
{
    PNTRC_MIF_TYPE Mif;
    LSA_UINT32 i;
    LSA_UINT16 resp;

    // Copy the values into the MIF
    for(i=1; i<TRACE_SUBSYS_NUM; i++)
    {
        Mif.WriteTraceLevels[i] = pLogLevels[i];
    }
    // Update the MIF that there are new log levels
    Mif.NewWriteTraceLevels=1;
    // Copy everything to the lower MIF by calling the system adaptation
    resp = PNTRC_WRITE_LOWER_CPU(hLowerDev->SysHandle,(LSA_UINT8*)(&Mif.NewWriteTraceLevels),PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),sizeof(Mif.NewWriteTraceLevels)+sizeof(Mif.WriteTraceLevels));
    PNTRC_ASSERT(resp == LSA_RET_OK);

    hLowerDev->SetLogLevelPending=LSA_TRUE;
}

/**
 * Opens a handle to a lower CPU
 * @param LSA_VOID
 * @return LSA_TRUE     - Lower CPU was successfully opened
 * @return LSA_FALSE    - All free instances are taken (PNTRC_CFG_MAX_LOWER_CPU_CNT too small?)
 */
static LSA_BOOL pntrc_lower_cpus_open(LSA_VOID)
{
    LSA_UINT8 idxLowerCpu;
    for (idxLowerCpu = 0; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened == LSA_TRUE)
        {
            return LSA_TRUE;
        }
    }
    return LSA_FALSE;
}

/**
 * Sets the log level for the current instance
 * 
 * @param [in] pLogLevels       - Pointer to a log level array, size assumed to be 8 * TRACE_SUBSYS_NUM
 * @param [in] UserID           - user id passed to the CBF
 * @param [in] SetLogLevelCB    - callback function
 * @return
 */
LSA_VOID pntrc_tbb_set_log_levels(const LSA_UINT8* pLogLevels, LSA_USER_ID_TYPE UserID, PNTRC_SYSTEM_CALLBACK_FCT_PTR_TYPE SetLogLevelCB)
{
    LSA_UINT32 idxTraceSubsys, idxLowerCpu;
    PNTRC_ASSERT(pLogLevels != LSA_NULL);
    
    g_pPntrcData->SetLogLevelCB=SetLogLevelCB;
    g_pPntrcData->SetLogLevelUserID=UserID;
    
    for(idxTraceSubsys = 1; idxTraceSubsys<TRACE_SUBSYS_NUM; idxTraceSubsys++)
    {
        pntrc_current_level[idxTraceSubsys] = pLogLevels[idxTraceSubsys];
    }
    
    g_pPntrcData->SetLogLevel_pLogLevels=pntrc_current_level;
    
    PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting %d log levels",TRACE_SUBSYS_NUM);
    if (pntrc_lower_cpus_open())
    {
        PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting %d log levels for lower cpus",TRACE_SUBSYS_NUM);
        g_pPntrcData->SetLogLevelPending=LSA_TRUE;
        for (idxLowerCpu =1; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
        {
            if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened)
            {
                pntrc_tbb_set_log_levels_lower_cpu(&g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu],pLogLevels);
            }
        }
    }
    else
    {
        PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: No lower cpus open, calling CB immediately");
        if (SetLogLevelCB != 0)
        {
            PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: CB != 0, call it");
            (*SetLogLevelCB)(g_pPntrcData->RQBSetTraceLevels);
            g_pPntrcData->RQBSetTraceLevels=LSA_NULL;
        }
    }
}

/**
 * Copies the current log levels to the array pLogLevels
 * @param [inout] pLogLevels   - Array with log levels. Size assumed to be 8 * TRACE_SUBSYS_NUM
 * @return
 */
LSA_VOID pntrc_tbb_get_log_levels(LSA_UINT8* pLogLevels)
{
    LSA_UINT32 idxTraceSubsys;
    
    for(idxTraceSubsys =1; idxTraceSubsys<TRACE_SUBSYS_NUM; idxTraceSubsys++)
    {
        pLogLevels[idxTraceSubsys] = pntrc_current_level[idxTraceSubsys];
    }
}

/**
 * Calls the system adaptation to save the buffer 
 * @param [in] SysHandle        - System handle
 * @param [in] BufferID         - Unique id of the buffer
 * @param [in] pBufferHeader    - Pointer to the buffer header
 * @return
 */
static LSA_UINT32 pntrc_tbb_SendBufferSystemAdaption(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID, PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader)
{
    LSA_UINT32 UsedSize;
    if (g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU)
    {
        UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);
        if (UsedSize>0)
        {
            // call system adaptation
            if (PNTRC_BUFFER_FULL(SysHandle, BufferID, (LSA_UINT8*)pBufferHeader, UsedSize) != 0)
            {
                return LSA_RET_ERR_RESOURCE;
            }
        }
    }
    return LSA_RET_OK;
}

/**
 * Marks the buffer as empty
 * (1) If this is the TopMost CPU, update the MasterMIF
 * (2) If there 
 * @param [in] SysHandle    - System handle
 * @param [in] IsTopMostCPU - 1 - This is the topmost CPU. 0 - There is an upper CPU above this CPU
 * @param [in] CPUNr        - index of the CPU
 * @param [in] CategoryNr   - index of the category
 * @param [in] BufferNr     - index of the buffer
 * @return
 */
static LSA_VOID pntrc_tbb_set_buffer_empty(LSA_VOID* SysHandle, LSA_UINT32 IsTopMostCPU, LSA_UINT32 CPUNr, LSA_UINT32 CategoryNr, LSA_UINT32 BufferNr)
{
    PNTRC_MIF_TYPE Mif;
    LSA_UINT16 resp;

    if (IsTopMostCPU)
    {
        pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[0][CategoryNr].StateBuffer[BufferNr]),PNTRC_BUFFER_STATE_EMPTY);
    }
    else
    {
        Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr] = PNTRC_BUFFER_STATE_EMPTY;
        //lint --e(866) Unusual use of 'SYM' in argument to sizeof.
        resp = PNTRC_WRITE_LOWER_CPU((PNTRC_SYS_HANDLE_LOWER_PTR_TYPE)SysHandle,
                              (LSA_UINT8*)(&Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]),
                              PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]),
                              sizeof(Mif.BuffersDesc[CPUNr][CategoryNr].StateBuffer[BufferNr]));
        PNTRC_ASSERT(resp == LSA_RET_OK);
    }
}

/**
 * @brief Saves all tracebuffers of a lower instance
 * Usecase (1) - If this is the toplevel cpu, the trace buffers are given to the system adaptation
 * Usecase (2) - If this is a middle level CPU, the trace buffers are given to the upper cpu
 *
 * The information about the tracebuffers of the lower cpu are given in the parameter Mif. The Mif has up to
 * (1 + PNTRC_CFG_MAX_LOWER_CPU_CNT) * PNTRC_MAX_CATEGORY_COUNT * PNTRC_TWINBUF_NUMBER buffers
 * Where...
 * - CPUNr = 0                                  - these are the trace buffers for the local traces produced by the lower cpu itself
 * - CPUNr = 1...PNTRC_CFG_MAX_LOWER_CPU_CNT    - (only valid if there are more cpus beneath this lower cpu) - traces of the lowerLowerCpus
 * - CategoryNr = 0...PNTRC_MAX_CATEGORY_COUNT  - Categories within one CPU
 * - BufferNr = 0...PNTRC_TWINBUF_NUMBER        - Twin buffers within one category
 *
 * This function allocates tracebuffers, the pointers are stored within the PNTRC_CPU_HANDLE_TYPE in g_pPntrcData->pntrc_lower_cpu_list. The PNTRC_CPU_HANDLE_TYPE is accessed by the handle LowerCPUNr.
 *
 * @param [in] hLowerDev    - handle to the lower cpu
 * @param [in] LowerCPUNr   - index of the lower cpu
 * @param [in] Mif          - Mif of the lower instance
 * @param [in] CPUNr        - current CPUNr to be saved (0...PNTRC_CFG_MAX_LOWER_CPU_CNT)
 * @param [in] CategoryNr   - current Category to be saved (0...PNTRC_MAX_CATEGORY_COUNT)
 * @param [in] BufferNr     - current Buffer to be saved (0...PNTRC_TWINBUF_NUMBER)
 * @param [in] FetchFatal   - (only used for trace?)
 * @return
 */
static LSA_VOID pntrc_tbb_copy_lower_buffer(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, const PNTRC_MIF_TYPE *Mif, LSA_UINT32 CPUNr, LSA_UINT32 CategoryNr, LSA_UINT32 BufferNr, LSA_BOOL FetchFatal)
{
    LSA_UINT32 Offset,Size;
    LSA_UINT8* pTraceBuffer;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT32 UsedSize;
    LSA_UINT32 DestOffset;
    LSA_UINT8* pDest;
    LSA_UINT32 CPUId;
    LSA_UINT16 resp;
    LSA_UINT32 resp32;
    
    // get the offset and size out of the Mif
    Offset=Mif->BuffersDesc[CPUNr][CategoryNr].OffsetBuffer[BufferNr];
    Size=Mif->BuffersDesc[CPUNr][CategoryNr].SizeBuffer[BufferNr];
    
    /*
    * Usecase 1: This is the topmost cpu. The buffers are read from the lower cpu and provided to the system adaptation
    */
    if (g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU)
    {
        // If the buffer does not exist yet, allocate it
        if (g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[CPUNr][CategoryNr][BufferNr] == 0)
        {
            PNTRC_ALLOC_LOCAL_MEM((void**)&g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[CPUNr][CategoryNr][BufferNr], Size);
            PNTRC_ASSERT(g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[CPUNr][CategoryNr][BufferNr] != 0);
        }
        // get current tracebuffer pointer
        pTraceBuffer=g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[CPUNr][CategoryNr][BufferNr];
        // ask system adaptation to copy the trace data into pTraceBuffer
        resp = PNTRC_READ_LOWER_CPU(hLowerDev,Offset,Size,pTraceBuffer);
        PNTRC_ASSERT(resp == LSA_RET_OK);

        pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)(LSA_VOID_PTR_TYPE)pTraceBuffer;
    
        // patch the CPUId inside the buffer header
        CPUId=(LowerCPUNr*10) + (pBufferHeader->Id >> 10);
        pBufferHeader->Id=pBufferHeader->Id & (~0xfc00); /*Mask out the board nr*/
        pBufferHeader->Id=pBufferHeader->Id | (CPUId << 10);
        if (FetchFatal)
        {
            PNTRC_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, CPU %ld, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,CPUNr,CategoryNr,BufferNr);
        }
        // pass the buffer to the system adaptation
        resp32 = pntrc_tbb_SendBufferSystemAdaption(hLowerDev,pBufferHeader->Id,pBufferHeader);
        PNTRC_ASSERT(resp32 == LSA_RET_OK);
    }
    else // Usecase 2: We are in the middle of two CPUs.
    {
        /*
        * Our lower CPU has number LowerCPUNr. As currently there are only three levels, our lower level has no underlying CPUs. Therefore there can only be trace data
        * from our lower CPU itself, which are the buffers with CPUNr=0. So CPUNr can only be 0 here. We have now to transfer these buffers to the middle CPU, where they shall
        * be copied to the buffer with CPUNr=LowerCPUNr.
        */
        /*First check, if our destination buffer is empty and already fetched by the upper CPU*/
        if (g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[LowerCPUNr][CategoryNr].StateBuffer[BufferNr] != PNTRC_BUFFER_STATE_FULL)
        {
            // If the buffer does not exist yet, allocate it
            if (g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr] == 0)
            {
                PNTRC_ALLOC_LOCAL_MEM((void**)&g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr], Size);
                PNTRC_ASSERT(g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr] != 0);
            }
            // get current tracebuffer pointer
            pTraceBuffer=g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr];
            
            // ask system adaptation to copy the trace data into pTraceBuffer
            resp = PNTRC_READ_LOWER_CPU(hLowerDev,Offset,Size,pTraceBuffer);
            PNTRC_ASSERT(resp == LSA_RET_OK);

            pBufferHeader= (PNTRC_BUFFER_HEADER_PTR_TYPE)(LSA_VOID_PTR_TYPE)pTraceBuffer;
            UsedSize=pBufferHeader->FirstBlock+pBufferHeader->CurBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE);
            if (UsedSize>0)
            {
                if (FetchFatal)
                {
                    PNTRC_SYSTEM_TRACE_04(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, CPU %ld, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,CPUNr,CategoryNr,BufferNr);
                }
                DestOffset=g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[LowerCPUNr][CategoryNr].OffsetBuffer[BufferNr];
                pDest=(LSA_UINT8*)(&g_pPntrcData->TraceMem->MasterMIF) + DestOffset;
                /*copy to upper CPU shared memory*/
                PNTRC_MEMCPY_LOCAL_MEM(pDest, g_pPntrcData->pntrc_lower_cpu_list[LowerCPUNr].pTraceBuffer[LowerCPUNr][CategoryNr][BufferNr], UsedSize);
                pBufferHeader=(PNTRC_BUFFER_HEADER_PTR_TYPE)(LSA_VOID_PTR_TYPE)pDest;
                pBufferHeader->Id=pBufferHeader->Id | (LowerCPUNr << 10);
                pntrc_tbb_set_mifs_value(PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,BuffersDesc[LowerCPUNr][CategoryNr].StateBuffer[BufferNr]),PNTRC_BUFFER_STATE_FULL);
                /*free the buffer of the lower CPU*/
                /*The lower (3d level) CPU has its buffers in CPUNr=0*/
                pntrc_tbb_set_buffer_empty(hLowerDev, g_pPntrcData->TraceMem->MasterMIF.Config.IsTopMostCPU, CPUNr, CategoryNr, BufferNr);
            }
        }
    } // Usecase 2: Middle CPU
}

/**
 * @brief Saves the tracebuffers of all lower cpus.
 *
 * @param [in] hLowerDev        handle to the lower CPU
 * @param [in] LowerCPUNr       index of the lower CPU
 * @param [in] pMif             Mif of the lower CPU. If this parameter is NULL, a local MIF is generated.
 * @param [in] FetchFatal       (only used for trace?)
 * @return
 */
LSA_VOID pntrc_tbb_save_all_lower_buffers(PNTRC_SYS_HANDLE_LOWER_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, PNTRC_MIF_PTR_TYPE pMif, LSA_BOOL FetchFatal)
{
    LSA_UINT32 idxLowerLowerCpu, idxLowerCategory, idxLowerTracebuffer;
    PNTRC_MIF_TYPE Mif;
    LSA_UINT16 resp;
    
    if (pMif == LSA_NULL)
    {
        pMif=&Mif;
    }

    // read (1) the config data and (2) the BufferDesc from the lower cpu
    resp = PNTRC_READ_LOWER_CPU(hLowerDev,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config),sizeof(pMif->Config)+sizeof(pMif->BuffersDesc),(LSA_UINT8*)(&pMif->Config));
    PNTRC_ASSERT(resp == LSA_RET_OK);
    
    for (idxLowerLowerCpu = 0; idxLowerLowerCpu< 1+pMif->Config.MaxLowerCPUCount; idxLowerLowerCpu++)
    {
        for (idxLowerCategory = 0; idxLowerCategory<pMif->Config.MaxCategoryCount; idxLowerCategory++)
        {
            for (idxLowerTracebuffer = 0; idxLowerTracebuffer < PNTRC_TWINBUF_NUMBER; idxLowerTracebuffer++)
            {
                if (pMif->BuffersDesc[idxLowerLowerCpu][idxLowerCategory].StateBuffer[idxLowerTracebuffer] != PNTRC_BUFFER_STATE_EMPTY)
                {
                    pntrc_tbb_copy_lower_buffer(hLowerDev,LowerCPUNr,pMif, idxLowerLowerCpu, idxLowerCategory, idxLowerTracebuffer,FetchFatal);
                }
            }
        }
    }
}

/**
 * @brief Fetches the tracebuffers of all lower cpus
 *
 * @param [in] hLowerDev
 * @param [in] LowerCPUNr
 * @param [in] sendSyncTime
 * @return
 */
static LSA_VOID pntrc_tbb_service_lower_cpu(PNTRC_CPU_HANDLE_PTR_TYPE hLowerDev, LSA_UINT8 LowerCPUNr, LSA_BOOL sendSyncTime)
{
    PNTRC_MIF_TYPE Mif;
    LSA_UINT32 idxLowerLowerCpu, idxLowerCategory, idxLowerTracebuffer;
    LSA_UINT16 resp;
    
    /// read the Mif with (1) Magic Number, (2) ReadFlag, (3) AnyBufferFull, (4) FatalOccured from the lower cpu
    resp = PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,MagicNumber),sizeof(Mif.MagicNumber)+sizeof(Mif.Ready)+sizeof(Mif.AnyBufferFull)+sizeof(Mif.FatalOccurred),(LSA_UINT8*)(&Mif.MagicNumber));
    PNTRC_ASSERT(resp == LSA_RET_OK);

    if (Mif.Ready != PNTRC_MIF_READY)
    {
        return;
    }
    /// If a fatal occurred, save all tracebuffers of the lower instance
    if (Mif.FatalOccurred && !hLowerDev->FatalHandled)
    {
        hLowerDev->FatalHandled=LSA_TRUE;
        PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching fatal trace begin ***");
        pntrc_tbb_save_all_lower_buffers(hLowerDev->SysHandle,LowerCPUNr,&Mif,LSA_TRUE);
        PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching fatal trace end ***");
        return;
    }
    /// If any of the buffers is full, read more infos from the MIF and copy all buffers that are full.
    if (Mif.AnyBufferFull)
    {
        // read the config block and buffer descriptions from the lower cpu
        resp = PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,Config),sizeof(Mif.Config)+sizeof(Mif.BuffersDesc),(LSA_UINT8*)(&Mif.Config));
        PNTRC_ASSERT(resp == LSA_RET_OK);

        // copy the tracebuffers from all CPUs with all categories and all switch buffers to the upper instance
        for (idxLowerLowerCpu = 0; idxLowerLowerCpu < 1+Mif.Config.MaxLowerCPUCount; idxLowerLowerCpu++)
        {
            for (idxLowerCategory = 0; idxLowerCategory < Mif.Config.MaxCategoryCount; idxLowerCategory++)
            {
                for (idxLowerTracebuffer = 0; idxLowerTracebuffer < PNTRC_TWINBUF_NUMBER; idxLowerTracebuffer++)
                {
                    if (Mif.BuffersDesc[idxLowerLowerCpu][idxLowerCategory].StateBuffer[idxLowerTracebuffer] == PNTRC_BUFFER_STATE_FULL)
                    {
                        pntrc_tbb_copy_lower_buffer(hLowerDev->SysHandle,LowerCPUNr,&Mif,idxLowerLowerCpu,idxLowerCategory,idxLowerTracebuffer,LSA_FALSE);
                    }
                }
            }
        }
    }
    // check if the initial log levels have to be set
    if (hLowerDev->InitLogLevelsSent == LSA_FALSE)
    {
        if (g_pPntrcData->SetLogLevel_pLogLevels != LSA_NULL)
        {  //log levels have been set at least once
            PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Upper: setting initial log levels for lower CPU");
            pntrc_tbb_set_log_levels_lower_cpu(hLowerDev,pntrc_current_level);
        }
        hLowerDev->InitLogLevelsSent = LSA_TRUE;
    }
    // synchronize the trace time of the lower cpus to the time of the local cpu.
    if (sendSyncTime)
    {
        // New sync time mode:
        #ifdef PNTRC_CFG_NEW_TIMESYNC_MODE
        PNTRC_WRITE_SYNC_TIME_LOWER_CPU(hLowerDev->SysHandle, LowerCPUNr);
        // Trigger the lower instance to write SYNC TIME START, SYNC TIME LOCAL and SYNC TIME SCALER
        pntrc_tbb_send_sync_time(hLowerDev->SysHandle);
        #endif
    }
    // check if the loglevels of the lower cpu have already been set
    if (hLowerDev->SetLogLevelPending)
    {
        resp = PNTRC_READ_LOWER_CPU(hLowerDev->SysHandle,
        PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,NewWriteTraceLevels),
        sizeof(Mif.NewWriteTraceLevels),
        (LSA_UINT8*)(&Mif.NewWriteTraceLevels));

        PNTRC_ASSERT(resp == LSA_RET_OK);

        if (Mif.NewWriteTraceLevels == 0)
        {
            hLowerDev->SetLogLevelPending=LSA_FALSE;
        }
    }
}

/**
 * @brief Cyclic service function that calls pntrc_tbb_service_lower_cpu for all underlying CPUs.
 *
 * @see pntrc_tbb_service_lower_cpu - Does a service for one CPU
 *
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_tbb_service_lower_cpus(LSA_VOID)
{
    LSA_UINT8 idxLowerCpu;
    LSA_BOOL sendSyncTime;
    LSA_BOOL anyLowerLogLevelPending;
    
    sendSyncTime=LSA_FALSE;
    // The time is synchronized every PNTRC_SEND_SYNC_SCALER calls of this function.
    g_pPntrcData->pntrc_SendSyncScaler++;
    if (g_pPntrcData->pntrc_SendSyncScaler >= PNTRC_SEND_SYNC_SCALER)
    {
        sendSyncTime=LSA_TRUE; // parameter is passed to pntrc_tbb_service_lower_cpu
        g_pPntrcData->pntrc_SendSyncScaler=0;
    }

    // iterate over all lower cpus
    for (idxLowerCpu = 1; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened)
        {
            // sent a SYNC TIME after adding the lower cpu. otherwise we would have to wait PNTRC_SEND_SYNC_SCALER cycles before sending the first SYNC TIME
            if(!g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].SyncTimeSent)
            {
                sendSyncTime = LSA_TRUE;
                g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].SyncTimeSent = LSA_TRUE;
            }
            pntrc_tbb_service_lower_cpu(&g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu],idxLowerCpu,sendSyncTime);
        }
    }
    // check if all lower cpus have set the log levels. If yes, call the callback function of the user.
    if (g_pPntrcData->SetLogLevelPending)
    {
        anyLowerLogLevelPending=LSA_FALSE;
        for (idxLowerCpu =1; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
        {
            if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened && g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].SetLogLevelPending)
            {
                anyLowerLogLevelPending=LSA_TRUE;
            }
        }
        if (!anyLowerLogLevelPending)
        {
            if (g_pPntrcData->SetLogLevelCB != 0)
            {
                (*g_pPntrcData->SetLogLevelCB)(g_pPntrcData->RQBSetTraceLevels);
                g_pPntrcData->RQBSetTraceLevels=LSA_NULL;
            }
            g_pPntrcData->SetLogLevelPending=LSA_FALSE;
        }
    }
}
/**
 * @todo Not used?
 * @param SysHandle
 * @param BufferID
 * @return
 */
LSA_VOID pntrc_tbb_buffer_full_done(LSA_VOID* SysHandle, PNTRC_BUFFER_ID_TYPE BufferID)
{
    LSA_UINT32 BufferNr;
    LSA_UINT32 Category;
    LSA_UINT32 IsTopMostCPU;
    LSA_UINT8 CPUId;
    LSA_UINT8 LowerCPUNr=0;
    
    BufferNr=(BufferID & 0x100)>>8;
    Category=BufferID & 0xFF;
    IsTopMostCPU=(BufferID & 0x200)>>9;
    CPUId=(BufferID & 0xFC00)>>10;
    if (CPUId > 10)
    {
        /* three CPU levels involved */
        LowerCPUNr=((BufferID & 0xFC00)>>10) % 10;
    }
    
    pntrc_tbb_set_buffer_empty(SysHandle,IsTopMostCPU,LowerCPUNr,Category,BufferNr);
}

/**
 * @brief Writes a trace header with the parameters given to this function
 *
 * @param [inout] pTraceBlock   - traceblock with the header
 * @param [in] SubSys           - Subsystem number
 * @param [in] Level            - trace level
 * @param [in] TracepointRef    - tracepoint (line of code and module id)
 * @param [in] ParCount         - number of parameters
 * @param [in] TimeStampHigh    - upper timestamp value
 * @param [in] TimeStampLow     - lower timestamp value
 * @return
 */
static LSA_VOID pntrc_tbb_WriteTraceHeader(PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock,
            LTRC_SUBSYS_TYPE SubSys,
            PNTRC_LEVEL_TYPE Level,
            LSA_UINT32 TracepointRef,
            LSA_UINT8 ParCount,
            LSA_UINT32   TimeStampHigh, LSA_UINT32   TimeStampLow)
{
    pTraceBlock->data.header.Subsys = SubSys;
    pTraceBlock->data.header.Flags = Level;
    pTraceBlock->data.header.ParCnt = ParCount;
    pTraceBlock->data.classic.a1 = TracepointRef;
    
    pTraceBlock->data.header.TimestampLow = TimeStampLow;
    pTraceBlock->data.header.TimestampHigh = TimeStampHigh;
}

/**
 * @brief Writes the trace header and a trace entry.
 *
 * @param [in] Category         - current category
 * @param [in] SubSys           - Subsystem number
 * @param [in] Level            - trace level
 * @param [in] TracepointRef    - tracepoint (line of code and module id)
 * @param [in] ParCount         - number of parameters
 * @param [in] TimeStampHigh    - upper timestamp value
 * @param [in] TimeStampLow     - lower timestamp value
 * @return
 */
static LSA_VOID pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount, LSA_UINT32 TimeStampHigh, LSA_UINT32 TimeStampLow)
{
    LSA_UINT32 CurBlockCount;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    PNTRC_BUFFER_ENTRY_PTR_TYPE pFirstTraceEntrySave;
    PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock;
    
    pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
    CurBlockCount=pBufferHeader->CurBlockCount;

    if (CurBlockCount > pBufferHeader->MaxBlockCount - 1)
    {
        CurBlockCount=CurBlockCount; //PNTRC_FATAL (0);
    }
    pTraceBlock=pntrc_get_cur_trace_block(Category,pBufferHeader->FirstBlock,CurBlockCount);
    
    pntrc_tbb_WriteTraceHeader(pTraceBlock,SubSys,Level,TracepointRef,ParCount,TimeStampHigh,TimeStampLow);
    if (CurBlockCount == 0)
    {
        pFirstTraceEntrySave=pntrc_get_cur_first_trace_entry_save(Category);
        if (pFirstTraceEntrySave->data.header.Subsys == 0)
        {  
            LSA_UINT32 CurentBufferNr;

            /* SubSys=0 indicates this is the first entry of a not overwritten buffer */
            /*Store 1st trace block. It is needed, when the buffer gets overwritten */
            pntrc_tbb_WriteTraceHeader(pFirstTraceEntrySave,SubSys,Level,TracepointRef,ParCount,TimeStampHigh,TimeStampLow);
            CurentBufferNr = g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].CurBufferNr;
            g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].StateBuffer[CurentBufferNr] = PNTRC_BUFFER_STATE_FILLING;
        }
    }
    
    pBufferHeader->CurBlockCount = pBufferHeader->CurBlockCount + 1;
}

/**
 * @brief Gets a timestamp and calls pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp.
 *
 * @see pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp - used to write the trace entry
 * @param [in] Category         - current category
 * @param [in] SubSys           - Subsystem number
 * @param [in] Level            - trace level
 * @param [in] TracepointRef    - tracepoint (line of code and module id)
 * @param [in] ParCount         - number of parameters
 * @return
 */
static LSA_VOID pntrc_tbb_WriteHeaderTraceBlock(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount)
{
    LSA_UINT32   TimeStampHigh;
    LSA_UINT32   TimeStampLow;
    
    PNTRC_GET_TRACESTAMP(&TimeStampLow, &TimeStampHigh);
    
    pntrc_tbb_WriteHeaderTraceBlockWithTimeStamp(Category, SubSys, Level, TracepointRef, ParCount, TimeStampHigh, TimeStampLow);
}

/**
 * @brief Writes 4 parameters into the current trace block
 *
 * PNTRC calls this function multiple times, depending on how much parameters are given to the trace macro.
 * For XXX_TRACE__00, this function is not called (only the header is written).
 * For XXX_TRACE__01...04, this function is called once.
 * For XXX_TRACE__11...16, this function is called four times.
 *
 * Example: XXX_TRACE__02 calls this with Par1 = <value>, Par2 = <value2>, Par3 = 0, Par4 = 0
 * -> Unused parameters shall be set to 0.
 *
 * @param [in] Category         - current category
 * @param [in] ParCount         - actual number of parameters. Valid values: 1...4
 * @param [in] Par1             - parameter value 1
 * @param [in] Par2             - parameter value 2
 * @param [in] Par3             - parameter value 3
 * @return
 */
static LSA_VOID pntrc_tbb_WriteParTraceBlock(LSA_UINT8 Category, LSA_UINT8 ParCount, LSA_UINT32 Par1, LSA_UINT32 Par2, LSA_UINT32 Par3)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    PNTRC_BUFFER_ENTRY_PTR_TYPE pTraceBlock;
    
    pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
    pTraceBlock=pntrc_get_cur_trace_block(Category,pBufferHeader->FirstBlock,pBufferHeader->CurBlockCount);
    
    pTraceBlock->data.header.Subsys = 0;
    pTraceBlock->data.header.Flags = 0;
    
    pTraceBlock->data.traceEntry.ParCnt = ParCount;
    pTraceBlock->data.traceEntry.par.pars.par1 = Par1;
    pTraceBlock->data.traceEntry.par.pars.par2 = Par2;
    pTraceBlock->data.traceEntry.par.pars.par3 = Par3;
    
    pBufferHeader->CurBlockCount = pBufferHeader->CurBlockCount + 1;
}

/**
 * @brief Does a buffer switch for the given category.
 *
 * @param [in] Category The buffers for this category are switched
 * @return LSA_ERR_SWITCH_ALREADY_IN_PROGRESS   - The buffer switch is already in progress
 * @return LSA_OK                               - Buffer switch was successfull
 */
static LSA_UINT32 pntrc_tbb_SwitchBuffer(LSA_UINT8 Category)
{
    PNTRC_BUFFER_HEADER_PTR_TYPE pNewBufferHeader;
    PNTRC_BUFFER_HEADER_PTR_TYPE pCurBufferHeader;
    PNTRC_BUFFER_ENTRY_PTR_TYPE pFirstTraceEntrySave;
    LSA_UINT32* pCurBufferNr;
    LSA_UINT32 NewBufferNr;
    LSA_UINT32* pCurBufferState;
    LSA_UINT32* pNewBufferState;
    #ifdef ZERO_TRACE_BUFFERS
    LSA_VOID* Ptr;
    #endif
    LSA_UINT32 resp;

    if (g_pPntrcData->bufferSwitchInProgress)
    {
        pCurBufferHeader = pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
        pCurBufferHeader->Flags=pCurBufferHeader->Flags | TRACE_BUF_FLAG_INTR_SKIP;
        return LSA_ERR_SWITCH_ALREADY_IN_PROGRESS;
    }
    g_pPntrcData->bufferSwitchInProgress=LSA_TRUE;

    pCurBufferHeader = pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
    /* BUFFER SWITCH algorithm */
    pNewBufferHeader = pntrc_get_other_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
    
    /* WRAP AROUND algorithm */
    pCurBufferNr=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].CurBufferNr;
    pCurBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].StateBuffer[*pCurBufferNr];
    NewBufferNr=1-*pCurBufferNr;
    pNewBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][Category].StateBuffer[NewBufferNr];
   
    if ((*pNewBufferState == PNTRC_BUFFER_STATE_FULL)   /*Nothing fetched yet, so stay in 2nd buffer with wrap around*/
    || (*pNewBufferState != PNTRC_BUFFER_STATE_EMPTY))
    {   
        /*User blocks other buffer, so stay in buffer with wrap around*/
        /*if other buffer is in use, so wrap around the current one*/
        //Now prepare buffer ptr, because we want to write traces immediately
        *pCurBufferState=PNTRC_BUFFER_STATE_EMPTY;
        pCurBufferHeader->CurBlockCount = 0;
        /*Indicate the wrap around. The original first entry of the buffer is retrieved and the associated time stamp is traced as the new first entry.
        So it can be reconstructed between which time stamps trace information is lost.*/
        pFirstTraceEntrySave=pntrc_get_cur_first_trace_entry_save(Category);
        pntrc_tbb_WriteHeaderTraceBlock(Category, (LTRC_SUBSYS_TYPE)pFirstTraceEntrySave->data.header.Subsys,(PNTRC_LEVEL_TYPE)PNTRC_OVERRUN_MASK,0,2);
        pntrc_tbb_WriteParTraceBlock(Category,2,
                                     pFirstTraceEntrySave->data.header.TimestampHigh,
                                     pFirstTraceEntrySave->data.header.TimestampLow,
                                     0);
    }
    else
    {
        pNewBufferHeader->CurBlockCount=0;
        g_pPntrcData->FirstTraceEntry[Category][NewBufferNr].data.header.Subsys=0; //indicate that buffers is new
        *pCurBufferNr=1-*pCurBufferNr;  //switch the buffers
        
        #ifdef ZERO_TRACE_BUFFERS
        Ptr=(LSA_VOID*)((LSA_UINT8*)pNewBufferHeader+pNewBufferHeader->FirstBlock);
        PNTRC_MEMSET_LOCAL_MEM(Ptr,0,pNewBufferHeader->MaxBlockCount*sizeof(PNTRC_BUFFER_ENTRY_TYPE));
        #endif
        
        *pCurBufferState=PNTRC_BUFFER_STATE_FULL;
        resp = pntrc_tbb_SendBufferSystemAdaption(&g_pPntrcData->hSysDev,pCurBufferHeader->Id,pCurBufferHeader);
        PNTRC_ASSERT(resp == LSA_RET_OK);
    }

    g_pPntrcData->bufferSwitchInProgress=LSA_FALSE;
    return LSA_OK;
}

/**
 * @brief Saves all buffers from the local CPUs and from all lower CPUs
 *
 * First, all local buffers of the current instance are passed to the system adaptation. Second, all buffers from all lower CPUs are saved.
 * @param [in] FetchFatal Only used for trace
 * @return
 */
LSA_VOID pntrc_tbb_save_all_buffers(LSA_BOOL FetchFatal)
{
    LSA_UINT8 idxCategory,idxBuffer, idxLowerCpu;
    PNTRC_TWIN_BUFFERS_DESC_PTR_TYPE BuffersDesc;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT32 resp;
    
    BuffersDesc=g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0]	;
    
    if (FetchFatal)
    {
        PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching buffers");
    }
    // First, save all traces from the local CPU
    for (idxCategory = 0; idxCategory<PNTRC_MAX_CATEGORY_COUNT; idxCategory++)
    {
        for (idxBuffer=0; idxBuffer<PNTRC_TWINBUF_NUMBER; idxBuffer++)
        {
            if (BuffersDesc[idxCategory].StateBuffer[idxBuffer] != PNTRC_BUFFER_STATE_EMPTY)
            {
                pBufferHeader = pntrc_get_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,idxCategory,idxBuffer);
                if (pBufferHeader->CurBlockCount > 0)
                {
                    if (FetchFatal)
                    {
                        PNTRC_SYSTEM_TRACE_03(0, LSA_TRACE_LEVEL_FATAL, "*** Fetching %ld trace blocks, Category %ld, BufferNr %ld",pBufferHeader->CurBlockCount,idxCategory,idxBuffer);
                    }
                    resp = pntrc_tbb_SendBufferSystemAdaption(&g_pPntrcData->hSysDev,pBufferHeader->Id,pBufferHeader);
                    PNTRC_ASSERT(resp == LSA_RET_OK);
                }
            }
        }
    }
    // Second, save all buffers from all openend lower CPUs.
    for (idxLowerCpu = 1; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened)
        {
            pntrc_tbb_save_all_lower_buffers(g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].SysHandle, idxLowerCpu, LSA_NULL, FetchFatal);
        }
    }
}

/**
 * Checks if a trace buffer is full, if yes, do a buffer switch.
 *
 * @param [in] Category - current category
 * @param [in[ ParCount - Number of parameters
 * @return LSA_ERR_SWITCH_ALREADY_IN_PROGRESS - Buffer is already in switch progress
 * @return LSA_RET_OK                         - Buffer was switched
 */
static LSA_UINT32 pntrc_tbb_CheckAndSwitchBuffer(LSA_UINT8 Category, LSA_UINT8 ParCount)
{
    LSA_UINT16 TraceBlockCount;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT32 Ret=LSA_OK;
    
    TraceBlockCount = (ParCount+2)/3 + 1;  //+2 = round up to 3
    
    pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,Category);
    if (pBufferHeader->CurBlockCount + TraceBlockCount -1 > pBufferHeader->MaxBlockCount - 1)
    {
        Ret=pntrc_tbb_SwitchBuffer(Category);
    }
    return Ret;
}

/**
 * Lock implementation
 *
 * Depending on the current execution level stored in Category, the system adaptation has to implement different locks.
 * For a Category where the Trace Macros are called in thread level, a mutex is enough.
 * For a Category within kernel threads, consider a spinlock or spinirqlock.
 * For a Category within the interrupt context, consider an interrupt lock.
 *
 * @param [in] Category - current category
 * @return 0            - Lock was successfully obtained
 * @return != 0         - Lock was not obtained
 */
static LSA_UINT32 pntrc_enter_trace(LSA_UINT8 Category)
{
    LSA_UINT32 ret;
    
    ret = PNTRC_ENTER_TRACE(Category);
    return ret;
}

/**
 * Lock implementation
 * @param Category
 * @return
 */
static LSA_VOID pntrc_exit_trace(LSA_UINT8 Category)
{
    PNTRC_EXIT_TRACE(Category);
}

/**
 * @brief Forces PNTRC to do a buffer switch
 *
 * First, all tracebuffers for the local CPU are saved. Then, all trace buffers for all opened lower CPUs are saved.
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_ForceBufferSwitch(LSA_VOID)
{
    LSA_UINT32* pCurBufferNr;
    LSA_UINT32 NewBufferNr;
    LSA_UINT32* pNewBufferState;
    PNTRC_BUFFER_HEADER_PTR_TYPE pBufferHeader;
    LSA_UINT16 idxLowerCpu;
    LSA_UINT8 idxCategory;
    PNTRC_MIF_TYPE Mif;
    LSA_UINT16 resp;
    LSA_UINT32 resp32;

    for (idxCategory=0; idxCategory<PNTRC_MAX_CATEGORY_COUNT; idxCategory++)
    {
        pBufferHeader=pntrc_get_cur_buffer_header(&g_pPntrcData->TraceMem->MasterMIF,0,idxCategory);
        if (pBufferHeader->CurBlockCount > 0)
        {
            pCurBufferNr=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][idxCategory].CurBufferNr;
            NewBufferNr=1-*pCurBufferNr;
            pNewBufferState=&g_pPntrcData->TraceMem->MasterMIF.BuffersDesc[0][idxCategory].StateBuffer[NewBufferNr];
            
            if (*pNewBufferState == PNTRC_BUFFER_STATE_EMPTY)
            {   
                /*only switch buffer, if the other one is free*/
                if (pntrc_enter_trace(idxCategory) == 0)
                {  
                    //avoid reentrancy, if a trace entry wants to switch the buffers
                    resp32 = pntrc_tbb_SwitchBuffer(idxCategory);
                    PNTRC_ASSERT(resp32 == LSA_RET_OK);
                }
                pntrc_exit_trace(idxCategory);
            }
        }
    }
    for (idxLowerCpu = 1; idxLowerCpu<1+PNTRC_CFG_MAX_LOWER_CPU_CNT; idxLowerCpu++)
    {
        if (g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].Opened)
        {
            Mif.ForceBufferSwitch=1;
            resp = PNTRC_WRITE_LOWER_CPU(g_pPntrcData->pntrc_lower_cpu_list[idxLowerCpu].SysHandle,
                                  (LSA_UINT8*)(&Mif.ForceBufferSwitch),
                                  PNTRC_OFFSET_OF(PNTRC_MIF_TYPE,ForceBufferSwitch),
                                  sizeof(Mif.ForceBufferSwitch));
            PNTRC_ASSERT(resp == LSA_RET_OK);
        }
    }
}

/**
 * Check if the given local MIF has any Tracebuffers that are full.
 *
 * @param Mif
 * @return 1 - There is at least one buffer that require a buffer switch
 * @return 0 - None of the buffers of the local instance need a buffer switch
 */
static LSA_UINT8 CheckIfAnyBufferFull(const PNTRC_MIF_TYPE *Mif)
{
    LSA_UINT16 idxLowerCpu,idxCategory,idxBuffer;
    LSA_UINT8 Full=0;
    
    for (idxLowerCpu = 0; idxLowerCpu<1+Mif->Config.MaxLowerCPUCount; idxLowerCpu++)
    {
        for (idxCategory = 0; idxCategory<Mif->Config.MaxCategoryCount; idxCategory++)
        {
            for (idxBuffer = 0; idxBuffer< PNTRC_TWINBUF_NUMBER; idxBuffer++)
            {
                if (Mif->BuffersDesc[idxLowerCpu][idxCategory].StateBuffer[idxBuffer] == PNTRC_BUFFER_STATE_FULL)
                {
                    Full=1;
                }
            }
        }
    }
    return Full;
}

/**
 * Dummy callback function that does a simple trace
 * @param rqb0
 * @return
 */
static LSA_VOID pntrc_service_MIF_set_log_levels_CB(LSA_VOID* rqb0)
{
    LSA_UNUSED_ARG(rqb0);
    
    PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "PNTRC: pntrc_service_MIF_set_log_levels_CB - NewWriteTraceLevels=0");
    g_pPntrcData->TraceMem->MasterMIF.NewWriteTraceLevels=0;
}

/**
 * @brief Cyclic service function for the local MIF
 *
 * This function is called by pntrc_service.
 *
 * Either the local CPU instance or the upper CPU may update the MIF. Several booleans from the MIF are checked here,
 * the following functions are executed if the booleans within the MIF are set:
 *
 * (1)  Write the local sync time once after the startup (required to synchronize the timestamps to other CPU instances) - Calls PNTRC_WRITE_LOCAL_SYNC_TIME
 * (2a) Legacy mode                 - Write the provided sync time value of the upper instance (less accuracy, see docu to PNTRC_CFG_NEW_TIMESYNC_MODE)
 * (2b) PNTRC_CFG_NEW_TIMESYNC_MODE - Write the local time if the upper instance requested it (boolean MasterMIF.TimeSync.NewSyncTime)
 * (3)  Switch all buffers if a buffer switch was requested (boolean MasterMIF.ForceBufferSwitch)
 * (4)  Set new log levels (boolean MasterMIF.NewWriteTraceLevels)
 * (5)  Check if Any buffer is full, update the MIF with this information (used by upper CPU instance)
 *
 * @return
 */
LSA_VOID pntrc_service_MIF(LSA_VOID)
{
    LSA_USER_ID_TYPE UserID;
    #ifdef PNTRC_CFG_NEW_TIMESYNC_MODE
    static LSA_UINT16 counter = PNTRC_SEND_SYNC_SCALER; // Write it directly after startup
    #endif

    /*
    * (1) Write the local sync time (once). Trace time should only be written once during startup.
    * After the upper instance has connected to the MIF of this CPU, the boolean MasterMIF.TimeSync.NewSyncTime is used.
    */
    if(!g_pPntrcData->bStartTraceTimeWritten)
    {
        g_pPntrcData->bStartTraceTimeWritten = LSA_TRUE;
        #ifdef PNTRC_CFG_NEW_TIMESYNC_MODE
        PNTRC_WRITE_LOCAL_SYNC_TIME(); // trace it once.
        #endif
    }
    
    #ifdef PNTRC_CFG_NEW_TIMESYNC_MODE
    if(counter >= PNTRC_SEND_SYNC_SCALER)
    {
        /*
         * we could let the system adaptation trace the local time every PNTRC_SEND_SYNC_SCALER ticks. But this would result in a spam of LOCAL_SYNC_TIME traces.
         * Instead, we use the TimeSync.NewSyncTime as a trigger, see below.
         * commented out:
         * PNTRC_WRITE_LOCAL_SYNC_TIME(); // let system adaptation trace the local time
        */
        counter = 0;
    }
    else
    {
        counter ++;
    }
    #endif
    UserID.void_ptr=LSA_NULL;  /*avoid compiler warning*/
    /*
    * (2) Synchronize the local timestamps to the timestamps of the upper cpu....
    */
    if (g_pPntrcData->TraceMem->MasterMIF.TimeSync.NewSyncTime)
    {
        /*
        * (2a) ... by writing the time stamp value of the upper cpu into a tracebuffer (Legacy mode)
        */
        #ifndef PNTRC_CFG_NEW_TIMESYNC_MODE
        // Let the system adaptation trace the time from the upper instance
        PNTRC_WRITE_SYNC_TIME(g_pPntrcData->TraceMem->MasterMIF.TimeSync.SyncTimeLow,g_pPntrcData->TraceMem->MasterMIF.TimeSync.SyncTimeHigh);
        #else
        /*
        * (2b) ... by simply writing the local time (PNTRC_CFG_NEW_TIMESYNC_MODE)
        * Use the trigger from the upper instance to trace the local sync time, This prevents that the buffer is spammed with LOCAL_SYNC_TIME traces
        */
        PNTRC_WRITE_LOCAL_SYNC_TIME();
        #endif
        g_pPntrcData->TraceMem->MasterMIF.TimeSync.NewSyncTime=0;
    }
    /*
    * (3) Perform a buffer switch for all buffers
    */
    if (g_pPntrcData->TraceMem->MasterMIF.ForceBufferSwitch)
    {
        PNTRC_SYSTEM_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "MIF service: pntrc force buffer switch");
        pntrc_ForceBufferSwitch();
        g_pPntrcData->TraceMem->MasterMIF.ForceBufferSwitch=0;
    }
    /*
    * (4) Set Log levels
    */
    if (!g_pPntrcData->SetLogLevelPending && g_pPntrcData->TraceMem->MasterMIF.NewWriteTraceLevels)
    {
        PNTRC_SYSTEM_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Lower: setting %d log levels from MIF",TRACE_SUBSYS_NUM);
        pntrc_tbb_set_log_levels(g_pPntrcData->TraceMem->MasterMIF.WriteTraceLevels, UserID, pntrc_service_MIF_set_log_levels_CB);
    }
    /*
    * (5) Update AnyBufferFull (used by upper CPU)
    */
    g_pPntrcData->TraceMem->MasterMIF.AnyBufferFull=CheckIfAnyBufferFull(&g_pPntrcData->TraceMem->MasterMIF);
}

/**
 * Callback function that sets the value of the lower MIF
 * @param rqb0
 * @return
 */
static LSA_VOID pntrc_service_slave_MIF_set_log_levels_CB(LSA_VOID* rqb0)
{
    LSA_UNUSED_ARG(rqb0);
    
    g_pPntrcData->TraceMem->SlaveMif[0].NewWriteTraceLevels=0;
}


/**
 * @brief Cyclic function for all underlying slave MIFs.
 * Checks if new log levels have to be set for the lower CPU instances.
 * @param LSA_VOID
 * @return
 */
LSA_VOID pntrc_service_slave_MIF(LSA_VOID)
{
    LSA_USER_ID_TYPE UserID;
    LSA_UINT8 i;
    
    UserID.void_ptr=LSA_NULL;  /*avoid compiler warning*/
    for (i=0; i<PNTRC_MAX_SLAVE_MIF; i++)
    {
        if (!g_pPntrcData->SetLogLevelPending && g_pPntrcData->TraceMem->SlaveMif[i].NewWriteTraceLevels)
        {
            PNTRC_SYSTEM_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "PNTRC Lower: setting %d log levels from slave MIF %d",TRACE_SUBSYS_NUM,i);
            pntrc_tbb_set_log_levels(g_pPntrcData->TraceMem->SlaveMif[i].WriteTraceLevels, UserID, pntrc_service_slave_MIF_set_log_levels_CB);
        }
    }
}

/**
 * @brief Writes the current trace entry into the next trace header
 * (1) Trigger a buffer switch if the current header is full
 * (2) Write trace info into the trace header
 *
 * @param [in] Category         - current category
 * @param [in] SubSys           - Subsystem number
 * @param [in] Level            - trace level
 * @param [in] TracepointRef    - tracepoint (line of code and module id)
 * @param [in] ParCount         - number of parameters
 * @return LSA_OK                               - Normal operation
 * @return LSA_ERR_SWITCH_ALREADY_IN_PROGRESS   - Buffer switch in progress, trace entry cannot be written
 *
 * @todo: Currently, there is no error handling if Ret == LSA_ERR_SWITCH_ALREADY_IN_PROGRESS
 */
static LSA_UINT32 pntrc_tbb_CheckAndWriteHeader(LSA_UINT8 Category, LTRC_SUBSYS_TYPE SubSys, PNTRC_LEVEL_TYPE Level, LSA_UINT32 TracepointRef, LSA_UINT8 ParCount)
{
    LSA_UINT32 Ret;
    
    Ret=pntrc_tbb_CheckAndSwitchBuffer(Category,ParCount);
    if (Ret == LSA_RET_OK)
    {
        pntrc_tbb_WriteHeaderTraceBlock(Category,SubSys, Level, TracepointRef, ParCount);
    }
    return Ret;
}


/**
 * Trace implementation for 0 parameters.
 *
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_00 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    LSA_UINT32 resp;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    
    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            resp = pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 0);
            PNTRC_ASSERT(resp == LSA_RET_OK);
        }
        pntrc_exit_trace(Category);
    }
    return Result;
}


/**
 * Trace implementation for 1 parameters.
 *
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Argument 1
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_01 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 1) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 1, a1, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}


/**
 * Trace implementation for 2 parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_02 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 2) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}

/**
 * Trace implementation for 3 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_03 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
  LSA_UINT32             Idx,
#if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
  LSA_UINT32             ModuleId,
#endif
#if (PNTRC_CFG_COMPILE_FILE == 1)
  LSA_CHAR*              File,
#endif
#if (PNTRC_CFG_COMPILE_LINE == 1)
  LSA_UINT32             Line,
#endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 3) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 3, a1, a2, a3);
            }
        }
        pntrc_exit_trace(Category);
    }
    return Result;
}

/**
 * Trace implementation for 4 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_04 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 4) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 4, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 1, a4, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 5 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_05 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    
    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 5) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 5, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 2, a4, a5, 0);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}

/**
 * Trace implementation for 6 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_06 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    
    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 6) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 6, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 3, a4, a5, a6);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}


/**
 * Trace implementation for 7 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_07 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif
    
    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 7) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 7, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 4, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 1, a7, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}

/**
 * Trace implementation for 8 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_08 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 8) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 8, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 5, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 2, a7, a8, 0);
            }
        }
        pntrc_exit_trace(Category);
    }
    
    return Result;
}


/**
 * Trace implementation for 9 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_09 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracepointRef, 9) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 9, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 6, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 3, a7, a8, a9);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 10 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_10 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10
)
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;

    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 10) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 10, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 7, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 4, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 1, a10, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 11 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_11 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 11) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 11, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 8, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 5, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 2, a10, a11, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 12 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @param [in] a12              Parameter 12
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_12 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 12) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 12, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 9, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 6, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 3, a10, a11, a12);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 13 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @param [in] a12              Parameter 12
 * @param [in] a13              Parameter 13
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_13 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 13) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 13, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 10, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 7, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 4, a10, a11, a12);
                pntrc_tbb_WriteParTraceBlock(Category, 1, a13, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 14 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @param [in] a12              Parameter 12
 * @param [in] a13              Parameter 13
 * @param [in] a14              Parameter 14
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_14 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 14) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 14, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 11, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 8, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 5, a10, a11, a12);
                pntrc_tbb_WriteParTraceBlock(Category, 2, a13, a14, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * Trace implementation for 15 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @param [in] a12              Parameter 12
 * @param [in] a13              Parameter 13
 * @param [in] a14              Parameter 14
 * @param [in] a15              Parameter 15
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_15 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14,
    LSA_UINT32             a15
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 15) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 15, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 12, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 9, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 6, a10, a11, a12);
                pntrc_tbb_WriteParTraceBlock(Category, 3, a13, a14, a15);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * Trace implementation for 16 Parameters.
 * 
 * @param [in] SubSys           number of the subsystem
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] a1               Parameter 1
 * @param [in] a2               Parameter 2
 * @param [in] a3               Parameter 3
 * @param [in] a4               Parameter 4  
 * @param [in] a5               Parameter 5
 * @param [in] a6               Parameter 6
 * @param [in] a7               Parameter 7
 * @param [in] a8               Parameter 8
 * @param [in] a9               Parameter 9
 * @param [in] a10              Parameter 10
 * @param [in] a11              Parameter 11
 * @param [in] a12              Parameter 12
 * @param [in] a13              Parameter 13
 * @param [in] a14              Parameter 14
 * @param [in] a15              Parameter 15
 * @param [in] a16              Parameter 16
 * @return LSA_TRUE  trace entry is     printed
 * @return LSA_FALSE trace entry is not printed
 */
LSA_BOOL  pntrc_tbb_memory_16 (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3,
    LSA_UINT32             a4,
    LSA_UINT32             a5,
    LSA_UINT32             a6,
    LSA_UINT32             a7,
    LSA_UINT32             a8,
    LSA_UINT32             a9,
    LSA_UINT32             a10,
    LSA_UINT32             a11,
    LSA_UINT32             a12,
    LSA_UINT32             a13,
    LSA_UINT32             a14,
    LSA_UINT32             a15,
    LSA_UINT32             a16
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, Level, TracePointRef, 16) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 16, a1, a2, a3);
                pntrc_tbb_WriteParTraceBlock(Category, 13, a4, a5, a6);
                pntrc_tbb_WriteParTraceBlock(Category, 10, a7, a8, a9);
                pntrc_tbb_WriteParTraceBlock(Category, 7, a10, a11, a12);
                pntrc_tbb_WriteParTraceBlock(Category, 4, a13, a14, a15);
                pntrc_tbb_WriteParTraceBlock(Category, 1, a16, 0, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

static LSA_UINT16 pntrc_IntMin(LSA_UINT16 a, LSA_UINT16 b)
{
    if (a<b) return a;
    else return b;
}


/**
 * @brief Internal trace implementation for a byte array
 * 
 * Traces a whole byte array by splitting the array into 4x32 Byte parts and writing these parts into the trace buffer. 
 * Byte arrays with a length > PNTRC_MAX_BYTE_ARRAY_LENGTH are trunktated to PNTRC_MAX_BYTE_ARRAY_LENGTH.
 * 
 * @param [in] Category         Current category
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Flags            PNTRC_BYTE_ARRAY_MASK       
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] DataPtr          Basepointer of the array
 * @param [in] DataLen          Length of the array. Maximum length = PNTRC_MAX_BYTE_ARRAY_LENGTH
 * @return
 */
static LSA_VOID pntrc_tbb_trace_byte_array(LSA_UINT8 Category,
                  LTRC_SUBSYS_TYPE  SubSys,
                  PNTRC_LEVEL_TYPE  Level,
                  LSA_UINT16        Flags,
                  LSA_UINT32        TracePointRef,
                  const LSA_UINT8   *DataPtr,
                  LSA_UINT16        DataLen)
{
    LSA_UINT16 BytesWritten=0;
    LSA_UINT8 RemainingParams;
    LSA_UINT8 RemainingParamsStore;
    LSA_UINT16 ReadPos=0;
    LSA_UINT16 FetchCount;
    LSA_UINT32 a[3];
    LSA_UINT16 i;
    LSA_UINT16 BytesToWrite;
    LSA_UINT8 ParamCount;

    if (DataLen == 0)
    {
        ParamCount=0;
    }
    else
    {
        if( DataLen > PNTRC_MAX_BYTE_ARRAY_LENGTH )
        {
            DataLen = PNTRC_MAX_BYTE_ARRAY_LENGTH; //because ParCount is a byte (255 values) in the trace header
        }

        ParamCount = (LSA_UINT8)((DataLen-1) / 4) + 1;  /*number of parameters from dataparameter*/
    }

    RemainingParams = ParamCount;
    if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, (PNTRC_LEVEL_TYPE)(Level | Flags), TracePointRef, ParamCount) == LSA_RET_OK)
    {
        while (BytesWritten<DataLen)
        {
            RemainingParamsStore=RemainingParams;
            BytesToWrite=pntrc_IntMin(12,DataLen-BytesWritten);
            for (i = 0; i < 3; i++)
            {
                a[i] = 0;
            }
            i=0;
            while (BytesToWrite > 0)
            {
                FetchCount=pntrc_IntMin(4,DataLen-BytesWritten);
                PNTRC_MEMCPY_LOCAL_MEM(&a[i], &(DataPtr[ReadPos]), (LSA_UINT32)FetchCount);
                i=i+1;
                ReadPos=ReadPos+FetchCount;
                BytesWritten=BytesWritten+FetchCount;
                RemainingParams=RemainingParams-1;
                BytesToWrite=BytesToWrite-FetchCount;
            }
            pntrc_tbb_WriteParTraceBlock(Category, RemainingParamsStore, a[0], a[1], a[2]);
        }
    }
}

/**
 * @brief Trace implementation for a byte array
 * 
 * @see pntrc_tbb_trace_byte_array
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] DataPtr          Basepointer of the array
 * @param [in] DataLen          Length of the array. Maximum length = PNTRC_MAX_BYTE_ARRAY_LENGTH
 * @return
 */
LSA_BOOL  pntrc_tbb_memory_bytearray (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    const LSA_UINT8 *      DataPtr,
    LSA_UINT16             DataLen
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            pntrc_tbb_trace_byte_array(Category, SubSys,Level,PNTRC_BYTE_ARRAY_MASK,TracePointRef,DataPtr,DataLen);
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * @brief String trace implementation
 * 
 * PNTRC uses the macro PNTRC_STRLEN to determine the length of the string. The string must be terminated by the string terminator \0.
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] st               String that should be traced
 * @return LSA_TRUE             String was traced
 * @return LSA_FALSE            String was not traced
 */
LSA_BOOL  pntrc_tbb_memory_string (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracePointRef,
    const LSA_CHAR*        st
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    LSA_UINT16 ByteCount;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        { 
            ByteCount = (LSA_UINT16)PNTRC_STRLEN(st) + 1; // no miss of '\0'
            pntrc_tbb_trace_byte_array(Category,SubSys,Level,PNTRC_STRING_MASK,TracePointRef,(LSA_UINT8*)st,ByteCount);
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * @brief Writes a trace entry for the sync time (Legacy mode)
 * 
 * Writes a trace header with the bit PNTRC_SYNC_TIME_MASK and the timestamp stored in p1 and p2 into the trace buffer. 
 * The converter uses this information to synchronize the time of different CPUs in the logfile.
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] p1               Lower part of the timestamp
 * @param [in] p2               Upper part of the timestamp
 * @return LSA_TRUE             Writing trace was successfull
 * @return LSA_FALSE            Writing trace was not successfull
 */
LSA_BOOL  pntrc_tbb_memory_synctime (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, (PNTRC_LEVEL_TYPE) PNTRC_SYNC_TIME_MASK, TracepointRef, 2) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * @brief Writes a trace entry for the very first trace entry (PNTRC_CFG_NEW_TIMESYNC_MODE)
 * 
 * Writes a trace header with the bits PNTRC_SYNC_TIME_MASK | PNTRC_START_MASK and the timestamp stored in p1 and p2 into the trace buffer. 
 * The converter uses this information to synchronize the time of different CPUs in the logfile. 
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] p1               Lower part of the timestamp
 * @param [in] p2               Upper part of the timestamp
 * @return LSA_TRUE             Writing trace was successfull
 * @return LSA_FALSE            Writing trace was not successfull
 */
LSA_BOOL  pntrc_tbb_memory_synctime_start (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, (PNTRC_LEVEL_TYPE)(PNTRC_SYNC_TIME_MASK | PNTRC_START_MASK), TracepointRef, 2) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}


/**
 * @brief Writes a trace entry for the current local time (PNTRC_CFG_NEW_TIMESYNC_MODE)
 * 
 * Writes a trace header with the bits PNTRC_SYNC_TIME_MASK | PNTRC_LOCAL_MASK and the timestamp stored in p1 and p2 into the trace buffer. 
 * The converter uses this information to synchronize the time of different CPUs in the logfile.
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] p1               Lower part of the timestamp
 * @param [in] p2               Upper part of the timestamp
 * @return LSA_TRUE             Writing trace was successfull
 * @return LSA_FALSE            Writing trace was not successfull
 */
LSA_BOOL  pntrc_tbb_memory_synctime_local (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, (PNTRC_LEVEL_TYPE) (PNTRC_SYNC_TIME_MASK | PNTRC_LOCAL_MASK), TracepointRef, 2) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * @brief Writes a trace entry for the local time of a lower CPU (PNTRC_CFG_NEW_TIMESYNC_MODE)
 * 
 * Writes a trace header with the bits PNTRC_SYNC_TIME_MASK | PNTRC_LOWER_MASK and the timestamp stored in p1 and p2 into the trace buffer. 
 * The converter uses this information to synchronize the time of different CPUs in the logfile.
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] p1               Lower part of the timestamp
 * @param [in] p2               Upper part of the timestamp
 * @return LSA_TRUE             Writing trace was successfull
 * @return LSA_FALSE            Writing trace was not successfull
 */
LSA_BOOL  pntrc_tbb_memory_synctime_lower (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2,
    LSA_UINT32             a3
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys,(PNTRC_LEVEL_TYPE) (PNTRC_SYNC_TIME_MASK | PNTRC_LOWER_MASK), TracepointRef, 3) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 3, a1, a2, a3);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}

/**
 * @brief Writes a trace entry for the scale factor of the local timestamp (PNTRC_CFG_NEW_TIMESYNC_MODE)
 * 
 * Writes a trace header with the bits PNTRC_SYNC_TIME_MASK | PNTRC_SCALER_MASK and the timestamp stored in p1 and p2 into the trace buffer. 
 * The converter uses this information to synchronize the time of different CPUs in the logfile.
 * 
 * @param [in] SubSys           Subsystem of the trace entry
 * @param [in] Level            log level of the trace entry
 * @param [in] Idx              Index within the subsystem
 * @param [in] ModuleId         ModuleId of the file (not used since TracepointRef is used)
 * @param [in] File             String with the filename (not used for binary traces)
 * @param [in] Line             Line of Code (not used since TracepointRef is used)
 * @param [in] Msg              String of the trace entry (not used for binary trace)
 * @param [in] TracepointRef    Combined ModuleId and Linenumber
 * @param [in] p1               Lower part of the timestamp
 * @param [in] p2               Upper part of the timestamp
 * @return LSA_TRUE             Writing trace was successfull
 * @return LSA_FALSE            Writing trace was not successfull
 */
LSA_BOOL  pntrc_tbb_memory_synctime_scaler (
    LTRC_SUBSYS_TYPE       SubSys,
    PNTRC_LEVEL_TYPE       Level,
    LSA_UINT32             Idx,
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UINT32             ModuleId,
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_CHAR*              File,
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UINT32             Line,
    #endif
    LSA_UINT32             TracepointRef,
    LSA_UINT32             a1,
    LSA_UINT32             a2
    )
{
    LSA_BOOL Result = LSA_FALSE;
    LSA_UINT8 Category;
    
    LSA_UNUSED_ARG(Level);
    LSA_UNUSED_ARG(Idx);
    #if (PNTRC_CFG_COMPILE_MODULE_ID == 1)
    LSA_UNUSED_ARG(ModuleId);
    #endif
    #if (PNTRC_CFG_COMPILE_FILE == 1)
    LSA_UNUSED_ARG(File);
    #endif
    #if (PNTRC_CFG_COMPILE_LINE == 1)
    LSA_UNUSED_ARG(Line);
    #endif

    Category=PNTRC_GET_CATEGORY(g_pPntrcData->hSysDev);
    if (Category<PNTRC_MAX_CATEGORY_COUNT)
    {
        if (pntrc_enter_trace(Category) == 0)
        {
            if (pntrc_tbb_CheckAndWriteHeader(Category, SubSys, (PNTRC_LEVEL_TYPE)(PNTRC_SYNC_TIME_MASK | PNTRC_SCALER_MASK), TracepointRef, 2) == LSA_RET_OK)
            {
                pntrc_tbb_WriteParTraceBlock(Category, 2, a1, a2, 0);
            }
        }
        pntrc_exit_trace(Category);
    }

    return Result;
}
/*****************************************************************************/
/*  end of file pntrc_tbb.c                                                   */
/*****************************************************************************/

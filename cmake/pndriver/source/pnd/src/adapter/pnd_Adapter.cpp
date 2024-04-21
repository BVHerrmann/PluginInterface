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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_Adapter.cpp                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Adapter class for free functions                                         */
/*                                                                           */
/*****************************************************************************/

#include "pnd_Adapter.h"
#include "pnd_pnstack.h"
#include "pnd_pnio_user_core.h"

//#include "eps_mempool.h"

#if (PSI_CFG_USE_EDDI == 1)
#include "eddi_cfg.h"
#include "eddi_usr.h"
#include "eddi_sys.h"
#endif

#include "psi_int.h"
//#include "psi_hd.h"

PNIO_VOID CPndAdapter::requestStart(LSA_UINT16 mailBoxId, PND_REQUEST_FCT requestFunction, PNIO_VOID_PTR_TYPE pRqb)
{
    pnd_request_start(mailBoxId, requestFunction, pRqb);
}

PNIO_VOID_PTR_TYPE CPndAdapter::ohaAllocRqb(LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PND_HANDLE_PTR_TYPE pnd_sys)
{
    return pnd_oha_alloc_rqb(opcode, handle, pnd_sys);
}

PNIO_VOID CPndAdapter::pnioUserRequestIobcDone(LSA_VOID_PTR_TYPE pRqb)
{
    pnd_pnio_user_request_iobc_done(pRqb);
}

PNIO_VOID CPndAdapter::epsUserRequestLower(LSA_VOID_PTR_TYPE pRqb, LSA_VOID_PTR_TYPE pInst, EPS_LOWER_DONE_FCT pFct)
{
    eps_user_request_lower(pRqb, pInst, pFct);
}

PNIO_VOID_PTR_TYPE CPndAdapter::ohaAllocUpperMem(LSA_UINT16 len, PND_HANDLE_PTR_TYPE pnd_sys)
{
    return pnd_oha_alloc_upper_mem(len, pnd_sys);
}

PNIO_VOID CPndAdapter::epsUserOpenChannelLower(LSA_VOID_PTR_TYPE pRqb, LSA_VOID_PTR_TYPE pInst, EPS_LOWER_DONE_FCT pFct)
{
    eps_user_open_channel_lower(pRqb, pInst, pFct);
}

PNIO_VOID CPndAdapter::ohaFreeRqb(LSA_VOID_PTR_TYPE pRqb, PND_HANDLE_PTR_TYPE pPndSys)
{
    pnd_oha_free_rqb(pRqb, pPndSys);
}

PNIO_VOID CPndAdapter::memFree(LSA_VOID_PTR_TYPE pBuffer)
{
    pnd_mem_free(pBuffer);
}

PNIO_VOID_PTR_TYPE CPndAdapter::memAlloc(LSA_UINT32 nSize)
{
    return pnd_mem_alloc(nSize);
}

LSA_RESPONSE_TYPE CPndAdapter::allocCriticalSection(LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive)
{
	return pnd_alloc_critical_section(lock_id_ptr, isPtreadMutexRecursive);
}

LSA_RESPONSE_TYPE CPndAdapter::freeCriticalSection(LSA_UINT16 lock_id_ptr)
{
	return pnd_free_critical_section(lock_id_ptr);
}

PNIO_VOID CPndAdapter::enterCriticalSection(LSA_UINT16 lockId)
{
    pnd_enter_critical_section(lockId);
}

PNIO_VOID CPndAdapter::exitCriticalSection(LSA_UINT16 lockId)
{
    pnd_exit_critical_section(lockId);
}

LSA_VOID CPndAdapter::pndPnioUserRequestLocalDone(PNIO_VOID_PTR_TYPE pRqb)
{
    pnd_pnio_user_request_local_done(pRqb);
}
#if (PSI_CFG_USE_EDDI == 1)

LSA_RESPONSE_TYPE CPndAdapter::createMemPool(LSA_UINT16* pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_fp mem_set)
{
    return eps_mempool_create(pool_id, pBase, uSize, uAlign, mem_set);
}

LSA_RESPONSE_TYPE CPndAdapter::deleteMemPool(LSA_UINT16 pool_id)
{
    return eps_mempool_delete(pool_id);
}

LSA_VOID_PTR_TYPE CPndAdapter::poolAllocMem(LSA_UINT16 pool_id, LSA_UINT32 size)
{
    return eps_mempool_alloc(pool_id, size);    
}

LSA_UINT16 CPndAdapter::poolFreeMem(LSA_UINT16 pool_id, LSA_VOID_PTR_TYPE ptr)
{
    return eps_mempool_free(pool_id, ptr);    
}

PNIO_VOID CPndAdapter::epsTasksTriggerEventThread(LSA_UINT32 hThread)
{
	eps_tasks_trigger_event_thread(hThread);
}

LSA_UINT32 CPndAdapter::epsTasksStartEventThread(LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs, EPS_THREAD_LOGICAL_CPU_ENUM_TYPE elogicalCPU)
{
    return eps_tasks_start_event_thread(pName, Prio, eSchedType, pCbf, uParam, pArgs, elogicalCPU);
}
LSA_VOID CPndAdapter::epsTasksStopEventThread(LSA_UINT32 hThread)
{
	return eps_tasks_stop_event_thread(hThread);
}
PSI_DETAIL_STORE_PTR_TYPE CPndAdapter::psiGetDetailStore(LSA_UINT16 hd_id)
{
    return psi_get_detail_store(hd_id);
}

LSA_RESULT CPndAdapter::epsEnableIsoInterrupt(PSI_SYS_HANDLE hSysDev, LSA_VOID(*func_ptr)(LSA_INT param, LSA_VOID* args), LSA_INT32 param, LSA_VOID* args)
{
    return eps_enable_iso_interrupt(hSysDev, func_ptr, param, args);
}
LSA_RESULT CPndAdapter::epsDisableIsoInterrupt(PSI_SYS_HANDLE hSysDev)
{
	return eps_disable_iso_interrupt(hSysDev);
}
LSA_RESULT CPndAdapter::eddiBufferRequestSetup(LSA_UINT32    InstanceHandle,
    LSA_UINT32 TraceIdx,
    LSA_VOID* pKRAM,
    LSA_VOID* pIRTE,
    LSA_VOID* pGSharedRAM,
    LSA_VOID* pPAEARAM,
    LSA_UINT32 offset_ProcessImageEnd,
    LSA_UINT32 PollTimeOut_ns,
    LSA_UINT16 MaxNrProviders)
{
    LSA_UINT16 retval;

    retval = eddi_SysRed_BufferRequest_Setup(InstanceHandle,        /* InstanceHandle*/
            TraceIdx,                                        /* TraceID*/
            pKRAM,                                           /* pKRAM: in SOC: IOCC->IRTE->KRAM*/
            pIRTE,                                           /* pIRTE: in SOC: IOCC->IRTE*/
            pGSharedRAM,                                     /* pGSharedRAM*/
            pPAEARAM,                                        /* pPAEARAM*/
            offset_ProcessImageEnd /*dont't care on SOC*/,   /* offset_ProcessImageEnd*/
            PollTimeOut_ns /*dont't care on SOC*/,           /* PollTimeOut_ns*/
            MaxNrProviders);                                 /* MaxNrProviders, Don´t care, allocated statically*/


    return retval;
}

LSA_RESULT CPndAdapter::eddiProviderBufferRequestRemove(LSA_UINT32 InstanceHandle, LSA_UINT16 ApplProviderID)
{
    return eddi_SysRed_Provider_Remove(InstanceHandle, ApplProviderID);
}

LSA_VOID CPndAdapter::eddiApplSyncInit()
{
	return eddi_ApplSync_Init();
}

LSA_RESULT CPndAdapter::eddiApplSyncParam(EDDI_HANDLE const  hDDB, EDD_APPL_SYNC_PARAM_TYPE* const pParam)
{
    return eddi_ApplSync_Param(hDDB, pParam); 
}

LSA_RESULT CPndAdapter::eddiApplSyncCheckTracks(LSA_UINT16 const Handle, LSA_UINT8 * const pTracks, LSA_BOOL const bStoreCycle)
{
    return eddi_ApplSync_CheckTracks(Handle, pTracks, bStoreCycle);
}

LSA_UINT32 CPndAdapter::eddiGetCycleCounterLow(EDDI_HANDLE const hDDB)
{
    return eddi_GetCycleCounterLow(hDDB);  
}

LSA_RESULT CPndAdapter::eddiGetTimeStamp(EDDI_HANDLE const hDDB, LSA_UINT32 * const pTimeStamp)
{
    return eddi_GetTimeStamp(hDDB, pTimeStamp);  
}

LSA_RESULT CPndAdapter::eddiApplSyncGetApplTimeOffset(LSA_UINT16 const Handle, LSA_UINT32 * const pApplTimeOffset_ns)
{
    return eddi_ApplSync_GetApplTimeOffset(Handle, pApplTimeOffset_ns);  
}

LSA_RESULT CPndAdapter::eddiApplSyncGetTimeValues(LSA_UINT16 const Handle, EDD_APPL_SYNC_TIME_VALUES_TYPE * const pTimeValues)
{
	return eddi_ApplSync_GetTimeValues(Handle, pTimeValues);
}

LSA_RESULT CPndAdapter::eddiIOCCSetup(EDDI_IOCC_LOWER_HANDLE_TYPE* pIOCCHandle,
    EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle, 
    LSA_UINT32 const InstanceHandle, 
    LSA_UINT8 const IOCCChannel, 
    LSA_VOID* const pIOCC, 
    LSA_UINT8* const pDestBase,
    LSA_UINT32 const DestBasePhysAddr, 
    LSA_UINT8* const pExtLinkListBase,
    LSA_UINT32 const ExtLinkListBasePhysAddr,
    LSA_UINT8** const ppIntLinkListMem,
    LSA_UINT32* const pIntLinkListMemSize,
	LSA_UINT8** const ppWB,
    LSA_UINT32 const MbxOffset, 
    EDDI_IOCC_ERROR_TYPE* const pErrorInfo)
{
    return eddi_IOCC_Setup(pIOCCHandle, 
        UpperHandle, 
        InstanceHandle, 
        IOCCChannel, 
        pIOCC, 
        pDestBase, 
        DestBasePhysAddr, 
        pExtLinkListBase, 
        ExtLinkListBasePhysAddr, 
        ppIntLinkListMem, 
        pIntLinkListMemSize,
		ppWB,
        MbxOffset,
        pErrorInfo);
}

LSA_RESULT CPndAdapter::eddiIOCCSingleRead(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, LSA_UINT32 DataLength, LSA_UINT32 PAEARAMOffset, LSA_UINT8* pDest)
{
    return eddi_IOCC_SingleRead(IOCCHandle, DataLength, PAEARAMOffset, pDest);
}

LSA_RESULT CPndAdapter::eddiIOCCSingleWrite(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, LSA_UINT32 DataLength, LSA_UINT32 PAEARAMOffset, LSA_UINT8* pSrc)
{
    return eddi_IOCC_SingleWrite(IOCCHandle, DataLength, PAEARAMOffset, pSrc);
}

PSI_HD_STORE_PTR_TYPE CPndAdapter::psiHdGetHdStore(LSA_UINT16 const hd_nr)
{
    return psi_hd_get_hd_store(hd_nr);
}

LSA_RESULT CPndAdapter::eddiIOCCCreateLinkList(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT8 LinkListType, LSA_UINT8 LinkListMode, LSA_UINT16 NrOfLines)
{
    return eddi_IOCC_CreateLinkList(IOCCHandle, pLLHandle, LinkListType, LinkListMode, NrOfLines);
}

LSA_RESULT CPndAdapter::eddiIOCCAppendToLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle, LSA_UINT32 PAEARAMOffset, LSA_UINT16 DataLength, LSA_UINT8* pMem, LSA_VOID* UserIOHandle)
{
    return eddi_IOCC_AppendToLinkList(LLHandle, PAEARAMOffset, DataLength, pMem, UserIOHandle);
}

LSA_RESULT CPndAdapter::eddiIOCCRemoveFromLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle, LSA_VOID* pUserIOHandle)
{
    return eddi_IOCC_RemoveFromLinkList(LLHandle, pUserIOHandle);
}

LSA_RESULT CPndAdapter::eddiIOCCMultipleRead(EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT32 NrOfLinkLists)
{
    return eddi_IOCC_MultipleRead(pLLHandle, NrOfLinkLists);
}

LSA_RESULT CPndAdapter::eddiIOCCMultipleWrite(EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT32 NrOfLinkLists)
{
    return eddi_IOCC_MultipleWrite(pLLHandle, NrOfLinkLists);
}

LSA_RESULT CPndAdapter::eddiIOCCDeleteLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle)
{
    return eddi_IOCC_DeleteLinkList(LLHandle);
}

LSA_RESULT CPndAdapter::eddiIOCCShutdown(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle)
{
    return eddi_IOCC_Shutdown(IOCCHandle);
}
PNIO_VOID CPndAdapter::pndEddiAllocIoccCh1Lock()
{
	pnd_eddi_alloc_iocc_ch1_lock();
}
PNIO_VOID CPndAdapter::pndEddiAllocIoccCh2Lock()
{
	pnd_eddi_alloc_iocc_ch2_lock();
}
PNIO_VOID CPndAdapter::pndEddiFreeIoccCh1Lock()
{
	pnd_eddi_free_iocc_ch1_lock();
}
PNIO_VOID CPndAdapter::pndEddiFreeIoccCh2Lock()
{
	pnd_eddi_free_iocc_ch2_lock();
}
PNIO_BOOL CPndAdapter::isPndEddiIoccCh1LockAllocated()
{
	return is_pnd_eddi_iocc_ch1_lock_allocated();
}
PNIO_BOOL CPndAdapter::isPndEddiIoccCh2LockAllocated()
{
	return is_pnd_eddi_iocc_ch2_lock_allocated();
}
#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

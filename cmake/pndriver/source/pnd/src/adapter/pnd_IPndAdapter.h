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
/*  F i l e               &F: pnd_IPndAdapter.h                         :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Interface class for CPndAdapter class                                    */
/*                                                                           */
/*****************************************************************************/

#ifndef IPNDADAPTER_H
#define IPNDADAPTER_H

#include "pnd_int.h"
#include "pnd_sys.h"

#include "eps_cp_hw.h"
#include "eps_mempool.h"
#include "eps_tasks.h"
#include "psi_int.h"
#include "psi_hd.h"

#if (PSI_CFG_USE_EDDI == 1)
#include "eddi_usr.h"
#include "eddi_iocc_cfg.h"
#include "eddi_iocc.h"
#endif

class IPndAdapter
{
public:
    virtual ~IPndAdapter() {}
    virtual PNIO_VOID requestStart(LSA_UINT16 mailBoxId, PND_REQUEST_FCT requestFunction, PNIO_VOID_PTR_TYPE pRqb) = 0;
    virtual PNIO_VOID pnioUserRequestIobcDone(LSA_VOID_PTR_TYPE pRqb) = 0;
    virtual PNIO_VOID epsUserRequestLower(LSA_VOID_PTR_TYPE  pRqb,  LSA_VOID_PTR_TYPE  pInst, EPS_LOWER_DONE_FCT  pFct) = 0;
    virtual PNIO_VOID_PTR_TYPE ohaAllocRqb( LSA_OPCODE_TYPE opcode, LSA_HANDLE_TYPE handle, PND_HANDLE_PTR_TYPE pndSys ) = 0;
    virtual PNIO_VOID_PTR_TYPE ohaAllocUpperMem(LSA_UINT16 len, PND_HANDLE_PTR_TYPE pnd_sys) = 0;
    virtual PNIO_VOID epsUserOpenChannelLower(LSA_VOID_PTR_TYPE pRqb, LSA_VOID_PTR_TYPE pInst, EPS_LOWER_DONE_FCT pFct) = 0;
    virtual PNIO_VOID ohaFreeRqb(LSA_VOID_PTR_TYPE pRqb, PND_HANDLE_PTR_TYPE pPndSys) = 0;
    virtual PNIO_VOID memFree(LSA_VOID_PTR_TYPE pBuffer) = 0;
    virtual PNIO_VOID_PTR_TYPE memAlloc(LSA_UINT32 nSize) = 0;
    virtual LSA_RESPONSE_TYPE allocCriticalSection(LSA_UINT16* lock_id_ptr, LSA_BOOL isPtreadMutexRecursive) = 0;
    virtual LSA_RESPONSE_TYPE freeCriticalSection(LSA_UINT16 lock_id_ptr) = 0;
    virtual PNIO_VOID enterCriticalSection(LSA_UINT16 lockId) = 0;
    virtual PNIO_VOID exitCriticalSection(LSA_UINT16 lockId) = 0;
    virtual PNIO_VOID pndPnioUserRequestLocalDone(PNIO_VOID_PTR_TYPE pRqb) = 0;
	#if (PSI_CFG_USE_EDDI == 1)
    virtual LSA_RESPONSE_TYPE createMemPool(LSA_UINT16* pool_id, LSA_UINT32* pBase, LSA_UINT32 uSize, LSA_UINT32 uAlign, pMemSet_fp mem_set) = 0;
    virtual LSA_RESPONSE_TYPE deleteMemPool(LSA_UINT16 pool_id) = 0;
    virtual LSA_VOID_PTR_TYPE poolAllocMem(LSA_UINT16 pool_id, LSA_UINT32 size) = 0;
    virtual LSA_UINT16 poolFreeMem(LSA_UINT16 pool_id, LSA_VOID_PTR_TYPE ptr) = 0;
    virtual PSI_DETAIL_STORE_PTR_TYPE psiGetDetailStore(LSA_UINT16 hd_id) = 0;
    virtual PSI_HD_STORE_PTR_TYPE psiHdGetHdStore( LSA_UINT16 hd_nr ) = 0;
    virtual PNIO_VOID epsTasksTriggerEventThread(LSA_UINT32 hThread) = 0;
    virtual LSA_UINT32 epsTasksStartEventThread(LSA_CHAR* pName, LSA_INT Prio, EPS_THREAD_SCHEDULING_TYPE eSchedType, EPS_TASKS_THREAD_PROC_TYPE pCbf, LSA_UINT32 uParam, LSA_VOID* pArgs, EPS_THREAD_LOGICAL_CPU_ENUM_TYPE elogicalCPU) = 0;
	virtual LSA_VOID epsTasksStopEventThread(LSA_UINT32 hThread) = 0;
	virtual LSA_RESULT epsEnableIsoInterrupt(PSI_SYS_HANDLE hSysDev, LSA_VOID(*func_ptr)(LSA_INT param, LSA_VOID* args), LSA_INT32 param, LSA_VOID* args) = 0;
	virtual LSA_RESULT epsDisableIsoInterrupt(PSI_SYS_HANDLE hSysDev) = 0;
    virtual LSA_RESULT eddiBufferRequestSetup(LSA_UINT32    InstanceHandle,
                                              LSA_UINT32    TraceIdx,
                                              LSA_VOID    * pKRAM,
                                              LSA_VOID    * pIRTE,
                                              LSA_VOID    * pGSharedRAM,
                                              LSA_VOID    * pPAEARAM,
                                              LSA_UINT32    offset_ProcessImageEnd,
                                              LSA_UINT32    PollTimeOut_ns,
                                              LSA_UINT16    MaxNrProviders) = 0;
    virtual LSA_RESULT eddiProviderBufferRequestRemove(LSA_UINT32 InstanceHandle, LSA_UINT16 ApplProviderID) = 0;    
	virtual LSA_VOID   eddiApplSyncInit() = 0;
    virtual LSA_RESULT eddiApplSyncParam(EDDI_HANDLE hDDB, EDD_APPL_SYNC_PARAM_TYPE* pParam) = 0;
    virtual LSA_RESULT eddiApplSyncCheckTracks(LSA_UINT16 Handle, LSA_UINT8 * pTracks, LSA_BOOL bStoreCycle) = 0; 
    virtual LSA_UINT32 eddiGetCycleCounterLow(EDDI_HANDLE hDDB) = 0;
    virtual LSA_RESULT eddiGetTimeStamp(EDDI_HANDLE hDDB, LSA_UINT32* pTimeStamp) = 0;
    virtual LSA_RESULT eddiApplSyncGetApplTimeOffset(LSA_UINT16 Handle, LSA_UINT32* pApplTimeOffset_ns) = 0;
	virtual LSA_RESULT eddiApplSyncGetTimeValues(LSA_UINT16 Handle, EDD_APPL_SYNC_TIME_VALUES_TYPE * pTimeValues) = 0;
    virtual LSA_RESULT eddiIOCCSetup(EDDI_IOCC_LOWER_HANDLE_TYPE* pIOCCHandle, 
                                    EDDI_IOCC_UPPER_HANDLE_TYPE UpperHandle, 
                                    LSA_UINT32 InstanceHandle, 
                                    LSA_UINT8 IOCCChannel, 
                                    LSA_VOID* pIOCC, 
                                    LSA_UINT8* pDestBase, 
                                    LSA_UINT32 DestBasePhysAddr, 
                                    LSA_UINT8* pExtLinkListBase,
                                    LSA_UINT32 ExtLinkListBasePhysAddr, 
                                    LSA_UINT8** ppIntLinkListMem, 
                                    LSA_UINT32* pIntLinkListMemSize,
                                    LSA_UINT8** ppWB,
                                    LSA_UINT32 MbxOffset, 
                                    EDDI_IOCC_ERROR_TYPE* pErrorInfo) = 0;
    virtual LSA_RESULT eddiIOCCSingleRead(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, LSA_UINT32 DataLength, LSA_UINT32 PAEARAMOffset, LSA_UINT8* pDest) = 0;
    virtual LSA_RESULT eddiIOCCSingleWrite(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, LSA_UINT32 DataLength, LSA_UINT32 PAEARAMOffset, LSA_UINT8* pSrc) = 0;
    virtual LSA_RESULT eddiIOCCCreateLinkList(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle, EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT8 LinkListType, LSA_UINT8 LinkListMode, LSA_UINT16 NrOfLines) = 0;
    virtual LSA_RESULT eddiIOCCAppendToLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle, LSA_UINT32 PAEARAMOffset, LSA_UINT16 DataLength, LSA_UINT8* pMem, LSA_VOID* UserIOHandle) = 0;
    virtual LSA_RESULT eddiIOCCRemoveFromLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle, LSA_VOID* pUserIOHandle) = 0;
    virtual LSA_RESULT eddiIOCCMultipleRead(EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT32 NrOfLinkLists) = 0;
    virtual LSA_RESULT eddiIOCCMultipleWrite(EDDI_IOCC_LL_HANDLE_TYPE* pLLHandle, LSA_UINT32 NrOfLinkLists) = 0;
    virtual LSA_RESULT eddiIOCCDeleteLinkList(EDDI_IOCC_LL_HANDLE_TYPE LLHandle) = 0;
    virtual LSA_RESULT eddiIOCCShutdown(EDDI_IOCC_LOWER_HANDLE_TYPE IOCCHandle) = 0;
	virtual PNIO_VOID pndEddiAllocIoccCh1Lock() = 0;
	virtual PNIO_VOID pndEddiAllocIoccCh2Lock() = 0;
	virtual PNIO_VOID pndEddiFreeIoccCh1Lock() = 0;
	virtual PNIO_VOID pndEddiFreeIoccCh2Lock() = 0;
	virtual PNIO_BOOL isPndEddiIoccCh1LockAllocated() = 0;
	virtual PNIO_BOOL isPndEddiIoccCh2LockAllocated() = 0;
	#endif
};


#endif

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

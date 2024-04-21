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
/*  C o m p o n e n t     &C: PnDriver                                  :C&  */
/*                                                                           */
/*  F i l e               &F: pnd_ISOUser.cpp                           :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*  Implementation of CISOUser class                                         */
/*                                                                           */
/*****************************************************************************/

#define LTRC_ACT_MODUL_ID   3404
#define PND_MODULE_ID       3404

#include "pnd_ISOUser.h"
#include "pnd_Controller.h"
#include "pnd_Adapter.h"
#include "pnd_ValueHelper.h"
//#include "pnd_pnio_user_core.h"
#include "pnd_sys.h"
//#include "pnd_pnstack.h"
#include "pnd_trc.h"
#include "edd_usr.h"
#include "psi_int.h"
//#include "eps_isr.h"
//#include "eps_statistics.h"
#include "eps_pn_drv_if.h"



#ifdef PND_CFG_BASIC_VARIANT
#define ISO_USER_SOC1_MASK_IRT_IRQ1_HP_REG_OFFSET  0x1700C
#define ISO_USER_SOC1_ACK_IRT_HP_REG_OFFSET  0x17410
#define ISO_USER_SOC1_IRR_IRT_HP_REG_OFFSET  0x17418
#define ISO_USER_SOC1_HP_IRT_EOI_REG_OFFSET  0x17424

#define ISO_USER_SOC1_MASK_IRT_IRQ1_REG_OFFSET  ISO_USER_SOC1_MASK_IRT_IRQ1_HP_REG_OFFSET
#define ISO_USER_SOC1_ACK_IRT_REG_OFFSET  ISO_USER_SOC1_ACK_IRT_HP_REG_OFFSET
#define ISO_USER_SOC1_IRR_IRT_REG_OFFSET  ISO_USER_SOC1_IRR_IRT_HP_REG_OFFSET
#define ISO_USER_SOC1_IRT_EOI_REG_OFFSET  ISO_USER_SOC1_HP_IRT_EOI_REG_OFFSET

#else
#define ISO_USER_SOC1_MASK_IRT_IRQ1_SP_REG_OFFSET  0x17020
#define ISO_USER_SOC1_ACK_IRT_SP_REG_OFFSET  0x17438
#define ISO_USER_SOC1_IRR_IRT_SP_REG_OFFSET  0x17440
#define ISO_USER_SOC1_SP_IRT_EOI_REG_OFFSET 0x1744C

#define ISO_USER_SOC1_MASK_IRT_IRQ1_REG_OFFSET  ISO_USER_SOC1_MASK_IRT_IRQ1_SP_REG_OFFSET
#define ISO_USER_SOC1_ACK_IRT_REG_OFFSET  ISO_USER_SOC1_ACK_IRT_SP_REG_OFFSET
#define ISO_USER_SOC1_IRR_IRT_REG_OFFSET  ISO_USER_SOC1_IRR_IRT_SP_REG_OFFSET
#define ISO_USER_SOC1_IRT_EOI_REG_OFFSET  ISO_USER_SOC1_SP_IRT_EOI_REG_OFFSET
#endif

#if (EPS_PLF == EPS_PLF_LINUX_SOC1)
#ifdef PND_CFG_BASIC_VARIANT
#define ISO_USER_CONTEXT_SWITCH_TIME_NS 5000
#define ISO_USER_VALID_EARLY_CHECK_COUNT 2
#else
#define ISO_USER_CONTEXT_SWITCH_TIME_NS 110000
#define ISO_USER_VALID_EARLY_CHECK_COUNT 5
#endif
#else
#define ISO_USER_CONTEXT_SWITCH_TIME_NS 0
#define ISO_USER_VALID_EARLY_CHECK_COUNT 1
#endif

#define ISO_USER_SOC1_IRT_NEWCYCLE_MASK      0x00000001
#define ISO_USER_SOC1_IRR_IRT_EOI_REG_VALUE  0xF

CISOUser* CISOUser::m_pInstances[PSI_CFG_MAX_HD_CNT] = {0};

CISOUser::CISOUser( PND_HANDLE_PTR_TYPE pHandle, CUserNode *pParent ) : IISOUser(pHandle, pParent),
m_responseRQB(PNIO_NULL),
m_syncHandle(0),
m_initialized(PNIO_FALSE),
m_pIoduEddiInst(PNIO_NULL),
m_obLoss(PNIO_FALSE),
m_syncStarted(PNIO_FALSE),
m_pIrteBasePtr(PNIO_NULL),
m_currentSyncState(INIT),
m_rxLocalTransferEndNs(INVALID_RX_LOCAL_TRANSFER_END_NS),
m_pPndAdapter(PNIO_NULL),
m_handleTm(PNIO_FALSE),
m_validEarlyCount(ISO_USER_VALID_EARLY_CHECK_COUNT)
{
    PND_ASSERT(m_pndHandle.cp_nr < PSI_CFG_MAX_HD_CNT);
    m_pInstances[m_pndHandle.cp_nr] = this;
}

CISOUser::~CISOUser()
{
    m_pInstances[m_pndHandle.cp_nr] = 0;

	m_responseRQB = PNIO_NULL;
	m_pIoduEddiInst = PNIO_NULL;
	m_pIrteBasePtr = PNIO_NULL;

    deletePndAdapter();
}

CISOUser* CISOUser::getInstance( PNIO_UINT32 CpIndex )
{
    if (CpIndex < PSI_CFG_MAX_HD_CNT)
    {
        return m_pInstances[CpIndex];
    }
    else
    {
        return PNIO_NULL;
    }
}

PNIO_VOID CISOUser::handleTmInterrupt( PNIO_UINT32 /*param*/, PNIO_VOID_PTR_TYPE pArgs )
{
    PNIO_UINT32 updateRes = 0;
    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::handleTmInterrupt()");

    PNIO_CYCLE_INFO cycleInfo;
    CISOUser* isoInstance = static_cast<CISOUser*>(pArgs);

    PND_ASSERT(isoInstance != PNIO_NULL);
    PND_ASSERT(isoInstance->m_initialized == PNIO_TRUE);

	if (!isoInstance->m_handleTm)
	{
		isoInstance->m_handleTm = PNIO_TRUE;
		setNewCycle(isoInstance->m_pIrteBasePtr,PNIO_FALSE);
	}

    CController* cntInst = CController::get_instance(isoInstance->m_pndHandle.cp_nr);
    PND_ASSERT(cntInst != PNIO_NULL);

    if(isoInstance->m_obLoss == PNIO_TRUE)  //TODO: check if this way of handling is correct
    {
        PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::handleTmInterrupt(): OB Loss");
        isoInstance->getCycleInfo(&cycleInfo);
        cntInst->opFault(&cycleInfo);
    }
    else
    {
        if(isoInstance->checkIfInTime() == PNIO_TRUE)
        {
            if(isoInstance->m_isoParam.CACF > 1)
            {
                //AEV model
                updateRes = isoInstance->m_pIoduEddiInst->updateIsoOutputs();
                PND_ASSERT(updateRes == PNIO_OK);

                updateRes = isoInstance->m_pIoduEddiInst->updateIsoInputs();
                PND_ASSERT(updateRes == PNIO_OK);
            }
            else
            {
                //EVA model
                updateRes = isoInstance->m_pIoduEddiInst->updateIsoInputs();
                PND_ASSERT(updateRes == PNIO_OK);
            }
            isoInstance->getCycleInfo(&cycleInfo);
            cntInst->startOp(&cycleInfo);
        }
        else
        {
            PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::handleTmInterrupt(): Not in time");
            isoInstance->getCycleInfo(&cycleInfo);
            cntInst->opFault(&cycleInfo);
        }
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::handleTmInterrupt()");
}

PNIO_VOID CISOUser::setNewCycle(PNIO_VOID *basePtr, PNIO_BOOL enable)
{
	PNIO_UINT32 regval = PND_REG32_READ((PNIO_UINT32)(basePtr)+ISO_USER_SOC1_MASK_IRT_IRQ1_REG_OFFSET);
	if (enable)
	{
		regval |= ISO_USER_SOC1_IRT_NEWCYCLE_MASK;
	}
	else
	{
		regval &= ~ISO_USER_SOC1_IRT_NEWCYCLE_MASK;
	}
	PND_REG32_WRITE((PNIO_UINT32)(basePtr)+ISO_USER_SOC1_MASK_IRT_IRQ1_REG_OFFSET, regval);
}

PNIO_UINT32 CISOUser::setOpDone(PNIO_CYCLE_INFO* pCycleInfo)
{
    PNIO_UINT32 result = PNIO_OK;
    PNIO_CYCLE_INFO cycleInfo;
    PNIO_UINT32 cycleCounter = 0;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::setOpDone()");

    PND_ASSERT(m_initialized == PNIO_TRUE);

    CController* inst = CController::get_instance(m_pndHandle.cp_nr);
    PND_ASSERT(inst != PNIO_NULL);

    //Update outputs here if EVA model is used
    if(m_isoParam.CACF == 1)
    {
        if(checkIfInTime() == PNIO_TRUE)
        {
            getCycleInfo(&cycleInfo);
            cycleCounter = cycleInfo.CycleCount;

            result = m_pIoduEddiInst->updateIsoOutputs();
            PND_ASSERT(result == PNIO_OK);

            getCycleInfo(&cycleInfo);

            if (result == PNIO_OK)
            {                
                if (cycleCounter != cycleInfo.CycleCount)
                {
                    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "CISOUser::setOpDone(): overrun at output update");

                    inst->opFault(&cycleInfo);
                    getCycleInfo(&cycleInfo);
                }                
            }
        }
        else
        {
            PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_WARN, "CISOUser::setOpDone(): Not in time");

            getCycleInfo(&cycleInfo);
            inst->opFault(&cycleInfo);

            getCycleInfo(&cycleInfo);
        }

        if (pCycleInfo != PNIO_NULL)
        {
            *pCycleInfo = cycleInfo;
        }
    }
    else if(pCycleInfo != PNIO_NULL)
    {
        getCycleInfo(pCycleInfo);
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::setOpDone()");

    return result;
}

PNIO_BOOL CISOUser::checkIfInTime()
{
    PNIO_BOOL result = PNIO_FALSE;
    PNIO_UINT8 tracks = 0;
	PNIO_UINT8 earlyCheckCount = 0;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::checkIfInTime()");

	while (earlyCheckCount < m_validEarlyCount)
	{
		tracks = 0;
		LSA_RESULT eddiResult = m_pPndAdapter->eddiApplSyncCheckTracks(m_syncHandle, &tracks, PNIO_FALSE);
		PND_ASSERT(eddiResult == EDD_STS_OK);

		if(tracks == EDD_APPLSYNC_TRACK_MASK_INTIME)
		{
			result = PNIO_TRUE;
			break;
		}
		else if (tracks & EDD_APPLSYNC_TRACK_MASK_EARLY)
		{
			earlyCheckCount++;
		}
		else
		{
			result = PNIO_FALSE;
			break;
		}
	}

	if (result == PNIO_FALSE)
	{
		PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, ">>CISOUser::checkIfInTime: tracks[%#x]", tracks);
	}

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::checkIfInTime()");

    return result;
}

PNIO_VOID CISOUser::Create(PND_PARAM_ISO_HD_IF_PTR_TYPE params)
{
    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CISOUser::Create()");

    m_isoParam = *params;
    m_initialized = PNIO_TRUE;

    m_pPndAdapter = createPndAdapter();
    PND_ASSERT(m_pPndAdapter != PNIO_NULL);

	m_pIrteBasePtr = m_isoParam.pBase;

    CController* cntInst = CController::get_instance(m_pndHandle.cp_nr);
    PND_ASSERT(cntInst != PNIO_NULL);

    m_pIoduEddiInst = dynamic_cast<IIODU_EDDI*>(cntInst->get_IODU(m_pndHandle.cp_nr));
    PND_ASSERT(m_pIoduEddiInst != PNIO_NULL);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CISOUser::Create()");
}

PNIO_VOID CISOUser::Startup(PNIO_VOID *pRqb)
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(pRqb);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CISOUser::Startup()");

    PND_ASSERT(m_initialized == PNIO_TRUE);
	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStartup  = PNIO_TRUE;
	m_isShutdown = PNIO_FALSE;

	m_responseRQB = pRQB;

    startupDone(PNIO_OK);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CISOUser::Startup()");
}

PNIO_VOID CISOUser::startupDone(PNIO_UINT32 result)
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CISOUser::startupDone()");

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStarted   = PNIO_TRUE;
	m_isStartup   = PNIO_FALSE;
	m_responseRQB = 0;

	pRQB->args.pnuc_internal.pnio_err = result;

    m_pPndAdapter->pndPnioUserRequestLocalDone(pRQB);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CISOUser::startupDone()");
}

PNIO_VOID CISOUser::Shutdown(PNIO_VOID* pRqb)
{
	PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(pRqb);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CISOUser::Shutdown()");

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( m_responseRQB == 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isShutdown = PNIO_TRUE;
	m_isStartup  = PNIO_FALSE;
	m_responseRQB = pRQB;

    shutdownDone(PNIO_OK);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CISOUser::Shutdown()");
}

PNIO_VOID CISOUser::shutdownDone(PNIO_UINT32 pnioErr)
{
	PND_RQB_PTR_TYPE pRQB = m_responseRQB;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CISOUser::shutdownDone()");

	PND_ASSERT( pRQB != 0);
	PND_ASSERT( LSA_RQB_GET_OPCODE(pRQB) == PND_OPC_PNUC_INTERNAL_REQ );

	m_isStarted   = PNIO_FALSE;
	m_isStartup   = PNIO_FALSE;
	m_isShutdown  = PNIO_FALSE;
	m_responseRQB = 0;

	// Signal shutdown complete
	pRQB->args.pnuc_internal.pnio_err = pnioErr;

	m_pPndAdapter->pndPnioUserRequestLocalDone(pRQB);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<CISOUser::shutdownDone()");
}

PNIO_VOID CISOUser::isoIrqIsr(LSA_INT /*param*/, PNIO_VOID_PTR_TYPE args)
{
    PNIO_UINT32 regVal = 0;
    CISOUser* isoInstance = (CISOUser*)args;
    PND_ASSERT(isoInstance != PNIO_NULL);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::isoIrqIsr()");

    regVal = PND_REG32_READ((PNIO_UINT32)(isoInstance->m_pIrteBasePtr) + ISO_USER_SOC1_IRR_IRT_REG_OFFSET);
	eddi_ApplSync_Interrupt(isoInstance->m_syncHandle, regVal);

	if( (regVal & ISO_USER_SOC1_IRT_NEWCYCLE_MASK) == ISO_USER_SOC1_IRT_NEWCYCLE_MASK )
	{
		PND_REG32_WRITE((PNIO_UINT32)(isoInstance->m_pIrteBasePtr) + ISO_USER_SOC1_ACK_IRT_REG_OFFSET,
			ISO_USER_SOC1_IRT_NEWCYCLE_MASK);
	}

	// set the EOI for the ISO interrupt
	PND_REG32_WRITE((PNIO_UINT32)(isoInstance->m_pIrteBasePtr) + ISO_USER_SOC1_IRT_EOI_REG_OFFSET, ISO_USER_SOC1_IRR_IRT_EOI_REG_VALUE);

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::isoIrqIsr()");
}

PNIO_VOID CISOUser::eddiTmIntReceiver(LSA_USER_ID_TYPE UserID, PNIO_BOOL bOBLoss)
{
	STATE_TYPE checkState = STARTED;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::eddiTmIntReceiver()");

    CISOUser* isoInstance = m_pInstances[UserID.uvar16]; //get instance by user id which is cp index
    PND_ASSERT(isoInstance != PNIO_NULL);

    if (isoInstance->m_currentSyncState == checkState)
    {
        //first call for "eddiApplSyncCheckTracks" for this cycle
        LSA_RESULT result = eddi_ApplSync_CheckTracks(isoInstance->m_syncHandle, 0, PNIO_TRUE);
        PND_ASSERT(result == EDD_STS_OK);

        //store ob loss info
        isoInstance->m_obLoss = bOBLoss;
		handleTmInterrupt(0, isoInstance);
    }
	else
	{
		PND_ISO_TRACE_02(0, LSA_TRACE_LEVEL_CHAT, "CISOUser::eddiTmIntReceiver: Tm interrupt has been recevied while the syncstate(%d), is not %d",
			isoInstance->m_currentSyncState, checkState);
	}

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::eddiTmIntReceiver()");
}

PNIO_VOID CISOUser::getCycleInfo(PNIO_CYCLE_INFO* cycleInfo)
{
	LSA_RESULT result;

	PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::getCycleInfo()");
	EDD_APPL_SYNC_TIME_VALUES_TYPE eddCycleInfo;

	result = m_pPndAdapter->eddiApplSyncGetTimeValues(m_syncHandle, &eddCycleInfo);

	if (result != EDD_STS_OK)
	{
		PND_ISO_TRACE_01(m_pndHandle.pnio_if_nr, LSA_TRACE_LEVEL_ERROR,
			"CISOUser::getCycleInfo(): Error(%u) while getting time values", result);
	}

	cycleInfo->CycleCount = eddCycleInfo.CycleCounterLow;
	cycleInfo->ClockCount = eddCycleInfo.TimeStamp_ns / cycleInfoDivider;
	cycleInfo->CountSinceCycleStart = eddCycleInfo.ApplTimeOffset_ns / cycleInfoDivider;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::getCycleInfo()");
}

PNIO_UINT32 CISOUser::syncStateChanged(PNIO_VOID *pRqb)
{
    PND_RQB_PTR_TYPE pRQB = static_cast<PND_RQB_PTR_TYPE>(pRqb);
    PND_SYNC_STATE_TYPE syncState = PND_SYNC_STATE_NOT_CONFIGURED;
	PNIO_UINT32 result = PNIO_OK;

    PND_ASSERT(pRQB != 0);

    syncState = pRQB->args.sync_change.syncState;

    PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>CISOUser::syncStateChanged(), syncState=%d", syncState);

    if (syncState == PND_SYNC_STATE_NOT_SYNC)
    {
        result = changeSyncState(FREERUN);
    }
    else if (syncState == PND_SYNC_STATE_SYNC)
    {
        result = changeSyncState(RESTART);
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "<<CISOUser::syncStateChanged()");
	return result;
}

PNIO_VOID CISOUser::setRxLocalTransferEndNs(PNIO_UINT32 rxLocalTransferEndNs)
{
    PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, ">>CISOUser::setRxLocalTransferEndNs(), rxLocalTransferEndNs=%u", rxLocalTransferEndNs);

    m_rxLocalTransferEndNs = rxLocalTransferEndNs;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, "<<CISOUser::setRxLocalTransferEndNs()");
}

PNIO_UINT32 CISOUser::changeSyncState(PND_SYNC_EVENT_TYPE eventType)
{
    PNIO_UINT32 result = PNIO_OK;

    PND_ISO_TRACE_02(0, LSA_TRACE_LEVEL_NOTE, ">>CISOUser::changeSyncState(), currentSyncState=%d eventType=%d", m_currentSyncState, eventType);

    switch (m_currentSyncState)
    {
    case INIT:
        if (eventType == START)
        {
            // set eddi sync state to start //
            result = startSync();
            m_currentSyncState = STARTED;
        }
        else
        {
            PND_ISO_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "CISOUser::changeSyncState(), wrong event sequence, eventType=%d, currentSyncState=%d", eventType, m_currentSyncState);
        }
        break;
    case STARTED:
        if (eventType == FREERUN)
        {
            // set eddi sync state to freerun //
            result = freerunSync();
            m_currentSyncState = FREERUNNING;
        }
        else if (eventType == STOP)
        {
            // set eddi sync state to stop //
            result = stopSync();
            m_currentSyncState = INIT;
        }
        else
        {
            PND_ISO_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "CISOUser::changeSyncState(), wrong event sequence, eventType=%d, currentSyncState=%d", eventType, m_currentSyncState);
        }
        break;
    case FREERUNNING:
        if (eventType == RESTART)
        {
            // set eddi sync state to restart //
            result = restartSync();
            m_currentSyncState = STARTED;
        }
        else if (eventType == STOP)
        {
            // set eddi sync state to stop //
            result = stopSync();
            m_currentSyncState = INIT;
        }
        else
        {
            PND_ISO_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_LOW, "CISOUser::changeSyncState(), wrong event sequence, eventType=%d, currentSyncState=%d", eventType, m_currentSyncState);
        }
        break;
    default:
        PND_FATAL("Unknown sync state!");
        break;
    }

    PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<<CISOUser::changeSyncState(), newSyncState=%d", m_currentSyncState);

    return result;
}

PNIO_UINT32 CISOUser::setMode(PNIO_MODE_TYPE mode)
{
    PNIO_UINT32 syncResult = PNIO_OK;

    PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_NOTE, "<<CISOUser::setMode(), mode=%d", mode);
    switch(mode)
    {
        case PNIO_MODE_OPERATE:
            syncResult = changeSyncState(START);
            break;
        case PNIO_MODE_OFFLINE:
        case PNIO_MODE_CLEAR:
            syncResult = changeSyncState(STOP);
            break;
        default:
            PND_FATAL( "invalid internal mode" );
            break;
    }

    if(syncResult != PNIO_OK)
    {
        PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, ">>CISOUser::setMode() Failed. Error Code: %x", syncResult);
    }

    return syncResult;
}

PNIO_UINT32 CISOUser::startSync()
{
    PNIO_UINT32 result = PNIO_OK;
    PNIO_UINT32 temp = 0;
    LSA_RESULT retVal = 0;

	PSI_SYS_TYPE hsysDevHandle;
	CValueHelper valObj;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_NOTE, ">>CISOUser::startSync()");
	m_pPndAdapter->eddiApplSyncInit();

    EDD_APPL_SYNC_PARAM_TYPE param;
    param.Cmd = EDD_APPLSYNC_CMD_START_SYNC;
    param.Param.Start.pHandle = &m_syncHandle;
    param.Param.Start.ApplNr = 0;
    param.Param.Start.If = 0;
#ifndef PND_CFG_BASIC_VARIANT
    param.Param.Start.IntSrc = EDDI_APPLSYNC_INTSRC_SP1;
#else
	param.Param.Start.IntSrc = EDDI_APPLSYNC_INTSRC_HP1;
#endif
    param.Param.Start.pIRTE_PNIP = m_pIrteBasePtr;
    param.Param.Start.pCBF = eddiTmIntReceiver;
    param.Param.Start.UserID.uvar16 = m_pndHandle.cp_nr;
    param.Param.Start.CACF = m_isoParam.CACF;
    param.Param.Start.ReductionRatio = m_isoParam.ReductionRatio;
    param.Param.Start.Tk.SeparateTracks.Tki_ns = m_isoParam.Tk.SeparateTracks.Tki_ns;
    param.Param.Start.Tk.SeparateTracks.Tko_ns = m_isoParam.Tk.SeparateTracks.Tko_ns;
    param.Param.Start.pXPLLOUT_CycleLen_ns = &temp;

    PND_ASSERT(m_rxLocalTransferEndNs != INVALID_RX_LOCAL_TRANSFER_END_NS);

    if (m_isoParam.Te_ns == m_isoParam.Tm_ns)
    {
        PND_ISO_TRACE_03(0, LSA_TRACE_LEVEL_NOTE, "CISOUser::startSync(): Te_Engineering(%u) and Tm_Engineering(%u) are the same. RxLocalTransferEndNs(%u) is assigned to Te and Tm.",
            m_isoParam.Te_ns, m_isoParam.Tm_ns, m_rxLocalTransferEndNs);

        param.Param.Start.Te_ns = m_rxLocalTransferEndNs;
        param.Param.Start.Tm_ns = (m_rxLocalTransferEndNs < ISO_USER_CONTEXT_SWITCH_TIME_NS) ? m_rxLocalTransferEndNs : (m_rxLocalTransferEndNs - ISO_USER_CONTEXT_SWITCH_TIME_NS);
    }
    else
    {
        PND_ISO_TRACE_04(0, LSA_TRACE_LEVEL_NOTE, "CISOUser::startSync(): Te_Engineering(%u) and Tm_Engineering(%u) are different. RxLocalTransferEndNs(%u) is assigned to Te and Tm_Engineering(%u) is assigned to Tm.",
            m_isoParam.Te_ns, m_isoParam.Tm_ns, m_rxLocalTransferEndNs, m_isoParam.Tm_ns);

        param.Param.Start.Te_ns = m_rxLocalTransferEndNs;
        param.Param.Start.Tm_ns = m_isoParam.Tm_ns;
		m_validEarlyCount = 1;
    }

    if(m_syncStarted == PNIO_FALSE)
    {
        retVal = m_pPndAdapter->eddiApplSyncParam(0, &param);

        if( (retVal == EDD_STS_OK) || (retVal == EDD_STS_OK_PENDING) )
        {
            m_syncHandle = *(param.Param.Start.pHandle);
			
			setNewCycle( m_pIrteBasePtr, PNIO_TRUE);

			valObj.PndHandle2SysPtrPSI(&m_pndHandle, &hsysDevHandle);
#ifdef PND_CFG_BASIC_VARIANT
			//In basic variant, host side hd_nr getting from index 0 that is the ld index
			hsysDevHandle.hd_nr = 0;//ld
#endif
            retVal = m_pPndAdapter->epsEnableIsoInterrupt(&hsysDevHandle, isoIrqIsr, 0, this);
            PND_ASSERT(retVal == EPS_PNDEV_RET_OK);

            PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "CISOUser::startSync(): SYNC started");

            m_syncStarted = PNIO_TRUE;
            result = PNIO_OK;
        }
        else
        {
            PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::startSync(): Error(%u) while starting SYNC", retVal);
            result = PNIO_ERR_INTERNAL;
        }
    }
    else
    {
        result = PNIO_OK;
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::startSync()");

    return result;
}

PNIO_UINT32 CISOUser::stopSync()
{
    PNIO_UINT32 result = PNIO_OK;
    EDD_APPL_SYNC_PARAM_TYPE param;
    LSA_RESULT retVal = 0;

	PSI_SYS_TYPE hsysDevHandle;
	CValueHelper valObj;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::stopSync()");

    if(m_syncStarted == PNIO_TRUE)
    {
        param.Cmd = EDD_APPLSYNC_CMD_STOP;
        param.Param.Stop.Handle = m_syncHandle;
        retVal = m_pPndAdapter->eddiApplSyncParam(0, &param);

        if( (retVal == EDD_STS_OK) || (retVal == EDD_STS_OK_PENDING) )
        {
			valObj.PndHandle2SysPtrPSI(&m_pndHandle, &hsysDevHandle);
#ifdef PND_CFG_BASIC_VARIANT
			hsysDevHandle.hd_nr = 0;//ld
#endif
            retVal = m_pPndAdapter->epsDisableIsoInterrupt(&hsysDevHandle);
            PND_ASSERT(retVal == EPS_PNDEV_RET_OK);

			m_syncStarted = PNIO_FALSE;
        }
        else
        {
            PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::stopSync(): Error(%u) while stopping SYNC", retVal);
            result = PNIO_ERR_INTERNAL;
        }
    }
    else
    {
        result = PNIO_OK;
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::stopSync()");

    return result;
}

PNIO_UINT32 CISOUser::restartSync()
{
    PNIO_UINT32 result = PNIO_OK;
    EDD_APPL_SYNC_PARAM_TYPE param;
    LSA_RESULT retVal = EDD_STS_OK;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::restartSync()");

    if (m_syncStarted == PNIO_TRUE)
    {
		//Activate new cycle mask register
		setNewCycle(m_pIrteBasePtr, PNIO_TRUE);

        param.Cmd = EDD_APPLSYNC_CMD_RESTART;
        param.Param.Restart.Handle = m_syncHandle;
        retVal = m_pPndAdapter->eddiApplSyncParam(0, &param);

        if (!((retVal == EDD_STS_OK) || (retVal == EDD_STS_OK_PENDING)))
        {
            PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::restartSync(): Error(%u) while restarting SYNC", retVal);
            result = PNIO_ERR_INTERNAL;
        }
    }
    else
    {
        result = PNIO_OK;
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::restartSync()");

    return result;
}

PNIO_UINT32 CISOUser::freerunSync()
{
    PNIO_UINT32 result = PNIO_OK;
    EDD_APPL_SYNC_PARAM_TYPE param;
    LSA_RESULT retVal = EDD_STS_OK;

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>CISOUser::freerunSync()");

    if (m_syncStarted == PNIO_TRUE)
    {
        param.Cmd = EDD_APPLSYNC_CMD_FREERUN;
        param.Param.FreeRun.Handle = m_syncHandle;
        retVal = m_pPndAdapter->eddiApplSyncParam(0, &param);

        if ( !((retVal == EDD_STS_OK) || (retVal == EDD_STS_OK_PENDING)) )
        {
            PND_ISO_TRACE_01(0, LSA_TRACE_LEVEL_ERROR, "CISOUser::freerunSync(): Error(%u) while freerunning SYNC", retVal);
            result = PNIO_ERR_INTERNAL;
        }
    }
    else
    {
        result = PNIO_OK;
    }

    PND_ISO_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<CISOUser::freerunSync()");

    return result;
}

string CISOUser::GetKey()
{
	CValueHelper valObj;

	string result = valObj.UINT16ToString( m_pndHandle.pnio_if_nr );

	return ( result );
}

LSA_UINT CISOUser::GetNumber()
{
	return ( m_pndHandle.pnio_if_nr );
}

IPndAdapter * CISOUser::createPndAdapter()
{
    return new CPndAdapter();
}

void CISOUser::deletePndAdapter()
{
    if (m_pPndAdapter != PNIO_NULL)
    {
        delete m_pPndAdapter;
        m_pPndAdapter = PNIO_NULL;
    }
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/

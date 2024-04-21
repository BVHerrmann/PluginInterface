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
/*  F i l e               &F: pnd_IOSystemReconfig.cpp                  :F&  */
/*                                                                           */
/*  V e r s i o n         &V: BC_PNDRIVER_P02.01.03.00_00.01.00.01      :V&  */
/*                                                                           */
/*  D a t e  (YYYY-MM-DD) &D: 2019-09-24                                :D&  */
/*                                                                           */
/*****************************************************************************/
/*                                                                           */
/*  D e s c r i p t i o n :                                                  */
/*                                                                           */
/*                                                                           */
/*****************************************************************************/
#define LTRC_ACT_MODUL_ID   1124
#define PND_MODULE_ID       1124

#include "pnd_sys.h"
#include "pnd_trc.h"
#include "pnd_IOSystemReconfig.h"
#include "pnd_Controller.h"

PNIO_VOID CIOSystemReconfig::setControllerInstance(CController *pController)
{
    m_pController = pController;
}

CIOSystemReconfig::SM_PROCESS_RESULT CIOSystemReconfig::processStateMachine( PND_RQB_PTR_TYPE pRqb )
{
    PNIO_UINT32 result;
    PNIO_ADDR laddr;
    PND_RQB_PTR_TYPE rqb = (PND_RQB_PTR_TYPE)pRqb;

    switch (m_state)
    {
    case IDLE:
        if (m_mode == PNIO_IOS_RECONFIG_MODE_DEACT)
        {
            m_state = DEACT;
            prepareDeactPendingList();

            m_pPndAdapter->memFree(rqb);

            result = PNIO_TRUE;
            while (result)
            {
				PND_ASSERT(m_pController != PNIO_NULL);
                result = m_pController->IsInProgressListNotFull();
                if (result)
                {
                    result = getNextDeviceFromPendingList(&laddr);
                    if (result)
                    {
                        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to de-activate : %d, activationType : %d",
                            laddr.u.Addr,
                            PNIO_DA_FALSE);

                        PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)m_pPndAdapter->memAlloc(sizeof(*pRQB));
                        pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

                        prepareDeactReq(laddr, PNIO_DA_FALSE, pRQB);
                        m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
                    }
                }
            }

        }
        else if (m_mode == PNIO_IOS_RECONFIG_MODE_TAILOR)
        {
            m_state = TAILOR;
			PND_ASSERT(m_pController != PNIO_NULL);
            laddr.u.Addr = m_pController->GetLogAddrOfInterfaceSubmodule();
            prepareReadReq(laddr, rqb);
            m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)rqb);
        }
        break;

    case DEACT:
        if (rqb->args.iob_dev_act.pnio_err != PNIO_OK)
        {
            m_pnioErrorCodeOfLastFailedDevice = rqb->args.iob_dev_act.pnio_err;
            m_laddrOfLastFailedDevice = rqb->args.iob_dev_act.Addr;
        }
		PND_ASSERT(m_pController != PNIO_NULL);
        m_pController->RemoveDeviceFromInProgressList(rqb->args.iob_dev_act.Addr);

        if (m_pController->isInProgressActivationListEmpty() && m_pController->isPendingActivationListEmpty())
        {
            m_state = IDLE;

            rqb->args.iosys_recon.Mode = rqb->args.iob_dev_act.DevActMode;
            rqb->args.iosys_recon.Addr = m_laddrOfLastFailedDevice;
            rqb->args.iosys_recon.pnio_err = m_pnioErrorCodeOfLastFailedDevice;

            m_pnioErrorCodeOfLastFailedDevice = PNIO_OK;
            pnd_memset(&m_laddrOfLastFailedDevice, 0, sizeof(PNIO_ADDR));

            return FINISHED;
        }

        result = getNextDeviceFromPendingList(&laddr);
        if (result)
        {
            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to de-activate : %d, activationType : %d",
                laddr.u.Addr,
                PNIO_DA_FALSE);

            prepareDeactReq(laddr, PNIO_DA_FALSE, rqb);
            m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)rqb);
        }
        else
        {
            m_pPndAdapter->memFree(rqb);
        }
        break;

    case TAILOR:
        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PDIOSystemInfo was read! Pnio Error : %d",
            rqb->args.rec_read.pnio_err);

        result = evaluateReadReq(rqb);

        if (result)
        {
            m_state = TAILOR_WRITE;
            laddr.u.Addr = m_pController->GetLogAddrOfInterfaceSubmodule();
            prepareWriteReq(laddr, rqb);
            m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)rqb);
        }
        else
        {
            m_state = IDLE;
            freeUserParams();

            m_pnioErrorCodeOfLastFailedDevice = PNIO_OK;
            pnd_memset(&m_laddrOfLastFailedDevice, 0, sizeof(PNIO_ADDR));

            return FINISHED;
        }
        break;

    case TAILOR_WRITE:

        PND_IOBC_TRACE_01(0, LSA_TRACE_LEVEL_NOTE_HIGH, "PDInstanceTailorData was written! Pnio Error : %d",
            rqb->args.rec_write.pnio_err);

        result = evaluateWriteReq(rqb);

        if (result)
        {
            m_state = TAILOR_ACT;

            prepareActPendingList();

            m_pPndAdapter->memFree(rqb);

            result = PNIO_TRUE;
            while (result)
            {
                result = m_pController->IsInProgressListNotFull();
                if (result)
                {
                    result = getNextDeviceFromPendingList(&laddr);
                    if (result)
                    {
                        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to activate : %d, activationType : %d",
                            laddr.u.Addr,
                            PNIO_DA_TRUE);

                        PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)m_pPndAdapter->memAlloc(sizeof(*pRQB));
                        pnd_memset(pRQB, 0, sizeof(PND_RQB_TYPE));

                        prepareDeactReq(laddr, PNIO_DA_TRUE, pRQB);
                        m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
                    }
                }
            }
        }
        else
        {
            m_state = IDLE;
            freeUserParams();

            m_pnioErrorCodeOfLastFailedDevice = PNIO_OK;
            pnd_memset(&m_laddrOfLastFailedDevice, 0, sizeof(PNIO_ADDR));

            return FINISHED;
        }
        break;

    case TAILOR_ACT:
        if (rqb->args.iob_dev_act.pnio_err != PNIO_OK)
        {
            m_pnioErrorCodeOfLastFailedDevice = rqb->args.iob_dev_act.pnio_err;
            m_laddrOfLastFailedDevice = rqb->args.iob_dev_act.Addr;
        }
		PND_ASSERT(m_pController != PNIO_NULL);
        m_pController->RemoveDeviceFromInProgressList(rqb->args.iob_dev_act.Addr);

        if (m_pController->isInProgressActivationListEmpty() && m_pController->isPendingActivationListEmpty())
        {
            m_state = IDLE;
            freeUserParams();

            rqb->args.iosys_recon.Mode = rqb->args.iob_dev_act.DevActMode;
            rqb->args.iosys_recon.Addr = m_laddrOfLastFailedDevice;
            rqb->args.iosys_recon.pnio_err = m_pnioErrorCodeOfLastFailedDevice;

            m_pnioErrorCodeOfLastFailedDevice = PNIO_OK;
            pnd_memset(&m_laddrOfLastFailedDevice, 0, sizeof(PNIO_ADDR));

            return FINISHED;
        }

        result = getNextDeviceFromPendingList(&laddr);
        if (result)
        {
            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "Next Device Address to activate : %d, activationType : %d",
                laddr.u.Addr,
                PNIO_DA_TRUE);

            prepareDeactReq(laddr, PNIO_DA_TRUE, rqb);
            m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)rqb);
        }
        else
        {
            m_pPndAdapter->memFree(rqb);
        }
        break;
    }

    return ONGOING;
}

PNIO_IOS_RECONFIG_MODE CIOSystemReconfig::getMode() const
{
    return m_mode;
}

PNIO_BOOL CIOSystemReconfig::validateReconfigParams() const
{
	PND_ASSERT(m_reconfigParams != PNIO_NULL)
    if (m_reconfigParams->DeviceCount > 0)
    {
        if (m_reconfigParams->DeviceList == PNIO_NULL)
            return PNIO_FALSE;
    }

    if (m_reconfigParams->PortInterconnectionCnt > 0)
    {
        if (m_reconfigParams->PortInterconnectionList == PNIO_NULL)
            return PNIO_FALSE;
    }

    return PNIO_TRUE;
}
/*lint -e{818} Pointer parameter 'portInterconnectionList' could  be declared as pointing to const*/
CIOSystemReconfig::CIOSystemReconfig(PNIO_IOS_RECONFIG_MODE mode, PNIO_UINT32 deviceCount, const PNIO_ADDR *deviceList, 
                                     PNIO_UINT32 portInterconnectionCount, PNIO_ADDR *portInterconnectionList, 
                                     IPndAdapter *pPndAdapter) :
    m_state(IDLE),
    m_pPndAdapter(pPndAdapter),
    m_pController(PNIO_NULL),
    m_mode(mode),
    m_pnioErrorCodeOfLastFailedDevice(0)
{
    pnd_memset(&m_laddrOfLastFailedDevice, 0, sizeof(m_laddrOfLastFailedDevice));

    m_reconfigParams = (PNIO_IOS_RECONFIG_PTR_TYPE)m_pPndAdapter->memAlloc(sizeof(PNIO_IOS_RECONFIG_TYPE));
    PND_ASSERT(m_reconfigParams);

    m_reconfigParams->DeviceList = PNIO_NULL;
    m_reconfigParams->PortInterconnectionList = PNIO_NULL;

    m_reconfigParams->DeviceCount = deviceCount;

    if (m_reconfigParams->DeviceCount != 0 && deviceList != PNIO_NULL)
    {
        m_reconfigParams->DeviceList = (PNIO_ADDR *)m_pPndAdapter->memAlloc(m_reconfigParams->DeviceCount * sizeof(PNIO_ADDR));
        PND_ASSERT(m_reconfigParams->DeviceList);
        pnd_memcpy(m_reconfigParams->DeviceList, deviceList, deviceCount * sizeof(PNIO_ADDR));
    }

    m_reconfigParams->PortInterconnectionCnt = portInterconnectionCount;

    if (m_reconfigParams->PortInterconnectionCnt != 0 && portInterconnectionList != PNIO_NULL)
    {
        m_reconfigParams->PortInterconnectionList = (PNIO_ADDR *)m_pPndAdapter->memAlloc(2 * m_reconfigParams->PortInterconnectionCnt * sizeof(PNIO_ADDR));
        PND_ASSERT(m_reconfigParams->PortInterconnectionList);
        pnd_memcpy(m_reconfigParams->PortInterconnectionList, portInterconnectionList, 2 * m_reconfigParams->PortInterconnectionCnt * sizeof(PNIO_ADDR));
    }
}

CIOSystemReconfig::~CIOSystemReconfig()
{
    freeUserParams();

    if(m_pPndAdapter != PNIO_NULL)
    {
        delete m_pPndAdapter;
        m_pPndAdapter = PNIO_NULL;
    }
}

PNIO_VOID CIOSystemReconfig::prepareDeactPendingList()
{
    PNIO_ADDR tmpAddr;
    tmpAddr.AddrType = PNIO_ADDR_LOG;
    tmpAddr.IODataType = PNIO_IO_IN;

	PND_ASSERT(m_pController != PNIO_NULL);
    m_pController->lockSharedList();

    for (PNIO_UINT32 idx = 0; idx < PND_LADDR_MAX; idx++)
    {
        if (m_pController->isIOCDeviceLogAddr(idx))
        {
            tmpAddr.u.Addr = idx;
            m_pController->pendingListPushBack(tmpAddr.u.Addr);
        }
    }

    m_pController->unlockSharedList();
}

PNIO_VOID CIOSystemReconfig::prepareDeactReq( const PNIO_ADDR &laddr, PNIO_DEV_ACT_TYPE type, PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)rqb;
    PND_ASSERT(rqb_ptr);
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

    // set opcode for device de/activate
    LSA_RQB_SET_OPCODE(rqb_ptr, PND_OPC_DEVICE_ACTIVATE);
    LSA_RQB_SET_HANDLE(rqb_ptr, (PNIO_UINT8) CIOBaseCoreCommon::get_handle(m_pController));
    PND_RQB_SET_USER_INST_PTR(rqb_ptr, m_pController);
    PND_RQB_SET_USER_CBF_PTR(rqb_ptr, CController::iosystem_reconfig_done);

    PND_DEV_ACTIVATE_TYPE *pActivate = &rqb_ptr->args.iob_dev_act;

    pnd_memset(pActivate, 0, sizeof(PND_DEV_ACTIVATE_TYPE));

    pActivate->Addr.u.Addr = laddr.u.Addr;
    pActivate->Addr.AddrType = PNIO_ADDR_LOG;
    pActivate->Addr.IODataType = PNIO_IO_IN;
    pActivate->DevActMode = type;
    pActivate->pnd_handle.cp_nr = (PNIO_UINT16) CIOBaseCoreCommon::get_handle(m_pController);
}

PNIO_VOID CIOSystemReconfig::prepareReadReq( const PNIO_ADDR &laddr, PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)rqb;
    PND_ASSERT(rqb_ptr);
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

    // set opcode for record read
    LSA_RQB_SET_OPCODE(rqb_ptr, PND_OPC_REC_READ);
    LSA_RQB_SET_HANDLE(rqb_ptr, (PNIO_UINT8)CIOBaseCoreCommon::get_handle(m_pController));
    PND_RQB_SET_USER_INST_PTR(rqb_ptr, m_pController);
    PND_RQB_SET_USER_CBF_PTR(rqb_ptr, CController::iosystem_reconfig_done);

    // RecodRead Rqb
    PND_RECORD_READ_WRITE_TYPE *pRecRead = &rqb_ptr->args.rec_read;

    pnd_memset(pRecRead, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecRead->UserRef = 0;
    pRecRead->pnd_handle.cp_nr = (PNIO_UINT16) CIOBaseCoreCommon::get_handle(m_pController);
    pRecRead->Addr.AddrType = PNIO_ADDR_LOG;
    pRecRead->Addr.IODataType = PNIO_IO_IN;
    pRecRead->Addr.u.Addr = laddr.u.Addr;
    pRecRead->RecordIndex = 0x0000B081; // CM: PDIOSystemInfo
    pRecRead->Length = 10 + (PSI_MAX_CL_DEVICES * 4);
    pRecRead->pBuffer = (PNIO_UINT8 *)m_pPndAdapter->memAlloc(pRecRead->Length);

    PND_ASSERT(pRecRead->pBuffer);

    pnd_memset(pRecRead->pBuffer, 0, pRecRead->Length);
}

PNIO_UINT32 CIOSystemReconfig::evaluateReadReq(PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)rqb;
    PNIO_UINT32 result = PNIO_TRUE;

    if (rqb_ptr->args.rec_read.pnio_err != PNIO_OK)
    {
        rqb_ptr->args.iosys_recon.pnio_err = mapCmErrorToPnioError(rqb_ptr->args.rec_read.err);

        result = PNIO_FALSE;
    }
    else
    {
        PNIO_VOID *pPDIOSytemInfoBuffer = rqb_ptr->args.rec_read.pBuffer;

        // Create Mandatory and Optional Device List
        createMandatoryAndOptionalDeviceList(pPDIOSytemInfoBuffer);
    }

    m_pPndAdapter->memFree(rqb_ptr->args.rec_read.pBuffer);
    rqb_ptr->args.rec_read.pBuffer = PNIO_NULL;

    return result;
}

PNIO_VOID CIOSystemReconfig::prepareWriteReq( const PNIO_ADDR &laddr, PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)rqb;
    PND_ASSERT(rqb_ptr);
    pnd_memset(rqb_ptr, 0, sizeof(PND_RQB_TYPE));

    PNIO_VOID_PTR_TYPE  pRecWriteBuffer = PNIO_NULL;
    PNIO_UINT32        recWriteLength = 0;

    // Creating PDInstanceTailorData for RecordWrite
    createPDInstanceTailorData(&pRecWriteBuffer, &recWriteLength);

    LSA_RQB_SET_OPCODE(rqb_ptr, PND_OPC_REC_WRITE);
    PND_RQB_SET_USER_CBF_PTR(rqb_ptr, CController::iosystem_reconfig_done);
    LSA_RQB_SET_HANDLE(rqb_ptr, (PNIO_UINT8) CIOBaseCoreCommon::get_handle(m_pController));
    PND_RQB_SET_USER_INST_PTR(rqb_ptr, m_pController);

    PND_RECORD_READ_WRITE_PTR_TYPE pRecWrite = &rqb_ptr->args.rec_write;
    pnd_memset(pRecWrite, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecWrite->UserRef = 0;
    pRecWrite->pnd_handle.cp_nr = (PNIO_UINT16) CIOBaseCoreCommon::get_handle(m_pController);
    pRecWrite->Addr.AddrType = PNIO_ADDR_LOG;
    pRecWrite->Addr.IODataType = PNIO_IO_IN;
    pRecWrite->Addr.u.Addr = laddr.u.Addr;
    pRecWrite->RecordIndex = 0x0000B080; // PDInstanceTailorData
    pRecWrite->Length = recWriteLength;
    pRecWrite->pBuffer = pRecWriteBuffer;
}

PNIO_UINT32 CIOSystemReconfig::evaluateWriteReq(PNIO_VOID *rqb )
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)rqb;
    PNIO_UINT32 result = PNIO_TRUE;

    if (rqb_ptr->args.rec_write.pnio_err != PNIO_OK)
    {
        // break the operation with callback
        rqb_ptr->args.iosys_recon.pnio_err = mapCmErrorToPnioError(rqb_ptr->args.rec_write.err);

        result = PNIO_FALSE;
    }

    m_pPndAdapter->memFree(rqb_ptr->args.rec_write.pBuffer);
    rqb_ptr->args.rec_write.pBuffer = PNIO_NULL;

    return result;
}

PNIO_VOID CIOSystemReconfig::prepareActPendingList()
{
    PNIO_UINT16 size = (PNIO_UINT16) m_mandatoryDeviceList.size();
	PND_ASSERT(m_pController != PNIO_NULL);

    m_pController->lockSharedList();

    for (PNIO_UINT16 idx = 0; idx < size; idx++)
    {
        m_pController->pendingListPushBack(m_mandatoryDeviceList.at(idx));
    }

	PND_ASSERT(m_reconfigParams);
    for (PNIO_UINT16 idx = 0; idx < m_reconfigParams->DeviceCount; idx++)
    {
        m_pController->pendingListPushBack(m_reconfigParams->DeviceList[idx].u.Addr);
    }

    m_pController->unlockSharedList();
}

PNIO_VOID CIOSystemReconfig::freeUserParams()
{
	//PND_ASSERT(m_reconfigParams != PNIO_NULL)
    if (m_reconfigParams)
    {
        if (m_reconfigParams->DeviceList)
        {
            m_pPndAdapter->memFree(m_reconfigParams->DeviceList);
            m_reconfigParams->DeviceList = PNIO_NULL;
        }

        if (m_reconfigParams->PortInterconnectionList)
        {
            m_pPndAdapter->memFree(m_reconfigParams->PortInterconnectionList);
            m_reconfigParams->PortInterconnectionList = PNIO_NULL;
        }

        m_pPndAdapter->memFree(m_reconfigParams);
        m_reconfigParams = PNIO_NULL;
    }
}

PNIO_UINT32 CIOSystemReconfig::mapCmErrorToPnioError( const PNIO_ERR_STAT &error ) const
{
    // cmErr can start by 0xDE while reading operation. 0xDE.. --> read, 0xDF.. --> write
    PNIO_UINT32 cmErr;
	
    cmErr = (PNIO_UINT32)(error.ErrCode << 24) + (PNIO_UINT32)(error.ErrDecode << 16) + (PNIO_UINT32)(error.ErrCode1 << 8) + (PNIO_UINT32)(error.ErrCode2);

    switch (cmErr)
    {
    case 0:
        return PNIO_OK;

    case 0xDF80AA00:
        return PNIO_ERR_CORRUPTED_DATA;

    case 0xDF80AA02:
        return PNIO_ERR_INVALID_STATION;

    case 0xDF80AA03:
        return PNIO_ERR_INVALID_PORT;

    case 0xDF80B602:
        return PNIO_ERR_ACCESS_DENIED;

    default: // 0xDFxxxxxx & 0xXXXXXXXX
        return PNIO_ERR_INTERNAL;
    }
}

PNIO_VOID CIOSystemReconfig::createMandatoryAndOptionalDeviceList(PNIO_VOID *record )
{
    // ### PDIOSystemInfo Record ###
    // ----------------------------------------------------------------------------------------------
    // | Block Header | IOSystemProperties | Number of Devices | Station Number | Device Properties |
    // ----------------------------------------------------------------------------------------------
    //
    // ----------------------------------------
    // Block Header ( Block Type(PNIO_UINT16) + Block Version High(PNIO_UINT8)
    //              + Block Version Low(PNIO_UINT8) + Block Length(PNIO_UINT16)
    //                Total: 6 bytes
    // ----------------------------------------
    // IOSystemProperties (PNIO_UINT16): 
    //      0x00: No optional device
    //      0x01: At least one optional device
    // ----------------------------------------
    // Number Of Devices (PNIO_UINT16):
    //      It contains the number of devices projected
    // ----------------------------------------
    // Station Number (PNIO_UINT16): 
    //      Device's station number
    // ----------------------------------------
    // Device Properties (PNIO_UINT16):
    //      Bit 0: IsPartOfAddressTailoring
    //             0x00: Device is not part of address tailoring
    //             0x01: Device is part of address tailoring
    //      Bit 1: IsOptional
    //             0x00: Device is mandatory
    //             0x01: Device is optional
    //      Bit 2: HasFixedPortToOptionalNeighbor
    //             0x00: None of device's port is fixed peer to optional neighbor
    //             0x01: At least one of the device's port is fixed to peer to optional neighbor
    //      Bit 3: HasProgrammablePeer
    //             0x00: None of device's port is programmable peer
    //             0x01: At least one of the device's ports is programmable peer
    //      Bit 4-15: reserved
    // ----------------------------------------

    m_mandatoryDeviceList.clear();
    m_optionalDeviceList.clear();

    PNIO_UINT8 *pPDIoSystemInfoRec = (PNIO_UINT8 *)record;

    // IOSystemProperties shows the properties of RTC3 devices
    // PNIO_UINT16 IOSystemProperties = pnd_hton16( *(PNIO_UINT16*) (pPDIoSystemInfoRec + 6) ); // not used.

    PNIO_UINT16 NumberOfDevices = pnd_hton16(*(PNIO_UINT16*)(pPDIoSystemInfoRec + 8));

    for (PNIO_UINT16 i = 0; i < NumberOfDevices; i++)
    {
        PNIO_UINT16 StationNumber = pnd_hton16(*(PNIO_UINT16*)(pPDIoSystemInfoRec + 10 + 4 * i)); // StationNumber + DeviceProperties = 4 bytes
        PNIO_UINT16 DeviceProperties = pnd_hton16(*(PNIO_UINT16*)(pPDIoSystemInfoRec + 12 + 4 * i)); // StationNumber + DeviceProperties = 4 bytes
        PNIO_ADDR tmpAddr;

        if (StationNumber != 0) // In case of controller's logical address and station number
        {
			PND_ASSERT(m_pController != PNIO_NULL);
            tmpAddr.AddrType = PNIO_ADDR_LOG;
            tmpAddr.IODataType = PNIO_IO_IN;
            tmpAddr.u.Addr = m_pController->ConvertGeoToLogAddr(StationNumber, 0, 0);

            // Bit 1: IsOptional
            //     0x00: Device is mandatory
            //     0x01: Device is optional
            if ((DeviceProperties & 0x02) == 0x00)
            {
                // Mandatory device
                m_mandatoryDeviceList.push_back(tmpAddr.u.Addr);
            }
            else
            {
                // Optional device
                m_optionalDeviceList.push_back(tmpAddr.u.Addr);
            }

        }

    }
}

PNIO_VOID CIOSystemReconfig::createPDInstanceTailorData(PNIO_VOID **ppPDInstTailorDataBuff, PNIO_UINT32 *pPDInstTailorDataBufLen )
{
	PND_ASSERT(m_reconfigParams != PNIO_NULL)
    // calculating PDInstanceTailorData block length
    PNIO_UINT16 BlockLength = (PNIO_UINT16)(8 + (m_reconfigParams->DeviceCount * 4));

    if (m_reconfigParams->PortInterconnectionCnt != 0)
        BlockLength += (PNIO_UINT16)(4 + m_reconfigParams->PortInterconnectionCnt * 16);


    PNIO_UINT8 *pTailorDataBuffer;
    PNIO_UINT32 *pTailorDataBufferLen = (PNIO_UINT32*)pPDInstTailorDataBufLen;

    *pTailorDataBufferLen = BlockLength + 4;

    // allocating memory for PDInstanceTailorData
    pTailorDataBuffer = (PNIO_UINT8*) m_pPndAdapter->memAlloc((*pTailorDataBufferLen) * sizeof(pTailorDataBuffer));
    PND_ASSERT(pTailorDataBuffer);

    *(PNIO_UINT8**)ppPDInstTailorDataBuff = pTailorDataBuffer;

    PNIO_UINT8 *pTailorDataBufferHead = pTailorDataBuffer;
    PNIO_UINT8 *pTailorDataBufferTail = pTailorDataBufferHead;

    // Adding PDInstanceTailorData Block Header
    *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(0x7080));       // BlockType
    pTailorDataBufferTail += 2;
    *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(BlockLength));  // Block Length
    pTailorDataBufferTail += 2;
    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x01);                       // Block Version High
    pTailorDataBufferTail += 1;
    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);                       // Block Version Low
    pTailorDataBufferTail += 1;

    // Adding PDInstanceTailorData Block Header

    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);         // Padding
    pTailorDataBufferTail += 1;
    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);         // Padding
    pTailorDataBufferTail += 1;

    // Device Instance Tailor Data
    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);         // Padding
    pTailorDataBufferTail += 1;
    *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);         // Padding
    pTailorDataBufferTail += 1;
    *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(m_reconfigParams->DeviceCount));  // Number Of Devices
    pTailorDataBufferTail += 2;

    for (PNIO_UINT16 i = 0; i < (PNIO_UINT16)m_reconfigParams->DeviceCount; i++)
    {
        PNIO_UINT16 tmpDeviceNumber;
		PND_ASSERT(m_reconfigParams != PNIO_NULL);
		PND_ASSERT(m_pController != PNIO_NULL);
        tmpDeviceNumber = m_pController->getDeviceNumber(m_reconfigParams->DeviceList[i].u.Addr);

        *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(tmpDeviceNumber));  // Station Number
        pTailorDataBufferTail += 2;
        *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);                           // Padding
        pTailorDataBufferTail += 1;
        *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);                           // Padding
        pTailorDataBufferTail += 1;
    }

    // Checking if there is any programmable peers
    if (m_reconfigParams->PortInterconnectionCnt != 0)
    {
        // Port Instance Tailor Data
        *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);   // Padding
        pTailorDataBufferTail += 1;
        *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);   // Padding
        pTailorDataBufferTail += 1;
        *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(m_reconfigParams->PortInterconnectionCnt));  // Number Of Interconnections
        pTailorDataBufferTail += 2;

        for (PNIO_UINT16 i = 0; i < (PNIO_UINT16)m_reconfigParams->PortInterconnectionCnt * 2; i++)
        {
			PND_ASSERT(m_pController != PNIO_NULL);
            PNIO_UINT16 tmpDeviceNumber = m_pController->getDeviceNumber(m_reconfigParams->PortInterconnectionList[i].u.Addr);
            PNIO_UINT16 tmpSlotNumber = m_pController->getSlotNumber(m_reconfigParams->PortInterconnectionList[i].u.Addr);
            PNIO_UINT16 tmpSubslotNumber = m_pController->getSubslotNumber(m_reconfigParams->PortInterconnectionList[i].u.Addr);

            *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(tmpDeviceNumber));   // Station Number
            pTailorDataBufferTail += 2;
            *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(tmpSlotNumber));     // Slot Number
            pTailorDataBufferTail += 2;
            *(PNIO_UINT16 *)pTailorDataBufferTail = pnd_hton16((PNIO_UINT16)(tmpSubslotNumber));  // Subslot Number
            pTailorDataBufferTail += 2;
            *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);                            // Padding
            pTailorDataBufferTail += 1;
            *(PNIO_UINT8 *)pTailorDataBufferTail = (PNIO_UINT8)(0x00);                            // Padding
            pTailorDataBufferTail += 1;
        }
    }
}

PNIO_UINT32 CIOSystemReconfig::getNextDeviceFromPendingList( PNIO_ADDR *laddr )
{
    PNIO_UINT8 result;

    PND_ASSERT(laddr);
    pnd_memset(laddr, 0, sizeof(PNIO_ADDR));

    // 3 return types
    // -> OK
    // -> pInProgressActivationList is full
    // -> pPendingActivationList is empty
	PND_ASSERT(m_pController);
    m_pController->lockSharedList();

    // PendingActivationList is empty
    if (m_pController->isPendingActivationListEmpty())
    {
        result = PNIO_FALSE;
    }
    else
    {
        laddr->AddrType = PNIO_ADDR_LOG;
        laddr->IODataType = PNIO_IO_IN;

        laddr->u.Addr = m_pController->pendingListGetLast();

        m_pController->inProgressListPushBack(laddr->u.Addr);

        m_pController->pendingListPopBack();

        result = PNIO_TRUE;
    }

    m_pController->unlockSharedList();

    return result;
}


/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/
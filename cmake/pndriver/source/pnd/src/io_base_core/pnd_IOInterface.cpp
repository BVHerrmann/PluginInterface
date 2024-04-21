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
/*  F i l e               &F: pnd_IOInterface.cpp                       :F&  */
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
#define LTRC_ACT_MODUL_ID   1123
#define PND_MODULE_ID       1123

#include "pnd_IOInterface.h"
#include "pnd_sys.h"
#include "pnd_trc.h"


CIOInterface::CIOInterface(PNIO_UINT32 UserHandle, IPndAdapter* pPndAdapter) : CIOBaseCoreCommon(PNIO_UINT16(UserHandle), PND_CLOSED, 0, 0, pPndAdapter),
    m_pUserCbf_Alarm(PNIO_NULL),
    m_pUserCbf_DR_Read(PNIO_NULL),
    m_pUserCbf_SetIpAndNos(PNIO_NULL),
    m_pUserCbf_RemaRead(PNIO_NULL),
    m_set_ip_and_nos_running(PNIO_FALSE),
    m_nr_of_alarm_res(0),
    m_nr_of_rema_res(0)
{
    pnd_memset(s_PndLogAddr.logAddrList, 0, sizeof(s_PndLogAddr.logAddrList));
    s_PndLogAddr.isInitialized = PNIO_FALSE;

    pnd_ifc_datarecord_read_rqb_list.size = PND_PNIO_MAX_RECORD_READ_WRITE_RESOURCES;
    pnd_alloc_rqb_list(pnd_ifc_datarecord_read_rqb_list);
}

CIOInterface::~CIOInterface(void)
{
    pnd_free_rqb_list(pnd_ifc_datarecord_read_rqb_list);
}

PND_RQB_LIST_TYPE* CIOInterface::getRQBList(LSA_OPCODE_TYPE opcode)
{
    PND_RQB_LIST_TYPE* pRqbList = PNIO_NULL;
    if (opcode == PND_OPC_REC_READ)
    {
        pRqbList = &pnd_ifc_datarecord_read_rqb_list;
    }
    return pRqbList;
}

CIOInterface* CIOInterface::get_instance(PNIO_UINT32 CpIndex)
{
    if (CpIndex < PND_MAX_INSTANCES)
    {
        if (s_PndUserCoreCpList[CpIndex].interface_instance_pointer != PNIO_NULL)
            return s_PndUserCoreCpList[CpIndex].interface_instance_pointer;
        else
            return PNIO_NULL;
    }
    else
        return PNIO_NULL;
}

PNIO_VOID CIOInterface::remove_instance(PNIO_UINT16 UserHandle)
{
    delete s_PndUserCoreCpList[UserHandle].interface_instance_pointer;
}

PNIO_UINT32 CIOInterface::register_cbf(PNIO_CBE_TYPE CbeType, PNIO_CBF cbf)
{
    PNIO_UINT32 Ret = PNIO_OK;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>CIOInterface register_cbf");

    if (m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!cbf)
        return PNIO_ERR_PRM_CALLBACK;

    switch (CbeType) {

    case PNIO_CBE_IFC_SET_ADDR_CONF:
        if (m_pUserCbf_SetIpAndNos) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_SetIpAndNos = cbf;
            Ret = PNIO_OK;
        }
        break;

    case PNIO_CBE_REMA_READ_CONF:
        if (m_pUserCbf_RemaRead) {
            Ret = PNIO_ERR_ALREADY_DONE;
        }
        else {
            m_pUserCbf_RemaRead = cbf;
            Ret = PNIO_OK;
        }
        break;

    default:
        Ret = PNIO_ERR_PRM_TYPE;
        break;
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<< CIOInterface register_cbf");

    return Ret;
}

PNIO_VOID CIOInterface::de_register_all_cbf()
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>de_register_cbf");

    m_pUserCbf_Alarm = 0;
    m_pUserCbf_SetIpAndNos = 0;
    m_pUserCbf_RemaRead = 0;
    m_pUserCbf_DR_Read = 0;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<de_register_cbf");
}

PNIO_UINT32 CIOInterface::interface_open(PNIO_UINT32 CpIndex,
    PNIO_CBF cbf_RecReadConf,
    PNIO_CBF cbf_AlarmInd,
    PNIO_UINT32 * pApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    CIOInterface *pThis = PNIO_NULL; //CIOInterface

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>interface_open");

    if (!pApplHandle)
        return PNIO_ERR_PRM_HND;

    *pApplHandle = CIOInterface::get_handle_by_index(CpIndex);
    if (*pApplHandle == PND_INVALID_HANDLE)
        return PNIO_ERR_PRM_CP_ID;

    pThis = CIOInterface::get_instance(CpIndex);
    if (pThis == PNIO_NULL)
        return PNIO_ERR_CREATE_INSTANCE;

    if (pThis->m_Status != PND_CLOSED)
        return PNIO_ERR_WRONG_HND;

    pThis->m_CpIndex = CpIndex;
    pThis->m_pUserCbf_Alarm = cbf_AlarmInd;
    pThis->m_pUserCbf_DR_Read = cbf_RecReadConf;

    // Wait on callback, after request 
    /*******/

    PND_RQB_PTR_TYPE               pRQB;
    PND_IOB_CORE_OPEN_PTR_TYPE     pOpen;

    pRQB = (PND_RQB_PTR_TYPE) pThis->m_pPndAdapter->memAlloc( sizeof(*pRQB));

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    if (pRQB == 0)
        PND_FATAL("mem alloc failed");

    pOpen = &pRQB->args.iob_open;

    pnd_memset(pOpen, 0, sizeof(PND_IOB_CORE_OPEN_TYPE));

    pOpen->pnd_handle.cp_nr = (PNIO_UINT16)CpIndex;

    if (pThis->s_PndLogAddr.isInitialized != PNIO_TRUE)
    {
        pOpen->laddr_list_ptr = pThis->m_pPndAdapter->memAlloc(sizeof(s_PndLogAddr.logAddrList));
    }

    PND_ASSERT(pRQB);

    result = pnd_alloc_event(&pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_reset_event(pThis->m_PndWaitEvent);

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_INTERFACE_OPEN);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CIOInterface::interface_open_done);

    pThis->m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);    

    pnd_wait_event(pThis->m_PndWaitEvent);

    result = pnd_free_event(pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    if (pThis->s_PndLogAddr.isInitialized != PNIO_TRUE)
    {
        pnd_memcpy(pThis->s_PndLogAddr.logAddrList, pOpen->laddr_list_ptr, sizeof(pThis->s_PndLogAddr.logAddrList));
        pThis->s_PndLogAddr.isInitialized = PNIO_TRUE;

        pThis->m_pPndAdapter->memFree( pOpen->laddr_list_ptr);
    }

    pThis->m_nr_of_alarm_res = pOpen->nr_of_alarm_res;
    pThis->m_nr_of_rema_res = 1;  // 1 resource is provided to OHA

    result = pRQB->_response;
    if (result != PNIO_RET_OK)
        return PNIO_ERR_INTERNAL;
    else
        result = PNIO_OK;

    /******/
    result = pOpen->pnio_err;
    if (result != PNIO_OK)
        return result;

    pThis->m_pPndAdapter->memFree(pRQB);

    result = pThis->alarm_provide(pThis->m_nr_of_alarm_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;

    result = pThis->rema_provide(pThis, pThis->m_nr_of_rema_res);
    if (result != PNIO_OK)
        return  PNIO_ERR_NO_RESOURCE;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<interface_open");

    pThis->m_set_status(PND_OPENED);


    result = pnd_alloc_event(&pThis->pnd_ifc_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_set_event(pThis->pnd_ifc_datarecord_read_rqb_list.resource_free_event);

    return PNIO_OK;
}

PNIO_UINT32 CIOInterface::interface_close(PNIO_UINT32 ApplHandle)
{
    PNIO_UINT32 result = PNIO_OK;
    CIOInterface *pThis = PNIO_NULL;
    PND_RQB_PTR_TYPE                pRQB;
    PND_IOB_CORE_CLOSE_PTR_TYPE     pClose;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>interface_close");

    pThis = CIOInterface::get_instance(ApplHandle);

    PND_ASSERT(pThis != PNIO_NULL);

    if (!pThis)
        return PNIO_ERR_WRONG_HND;

    if (PND_CLOSED == pThis->m_get_status())
    {
        return PNIO_ERR_WRONG_HND;
    }

    pThis->de_register_all_cbf();

    pnd_wait_event(pThis->pnd_ifc_datarecord_read_rqb_list.resource_free_event);

        pRQB = (PND_RQB_PTR_TYPE) pThis->m_pPndAdapter->memAlloc( sizeof(*pRQB));

    if (!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    pClose = &pRQB->args.iob_close;

    pnd_memset(pClose, 0, sizeof(PND_IOB_CORE_CLOSE_TYPE));

    pClose->pnd_handle.cp_nr = (PNIO_UINT16)pThis->m_CpIndex;

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_INTERFACE_CLOSE);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CIOInterface::interface_close_done);

    result = pnd_alloc_event(&pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    pnd_reset_event(pThis->m_PndWaitEvent);

    pThis->m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnd_iob_core_request, (PNIO_VOID_PTR_TYPE)pRQB);

    pnd_wait_event(pThis->m_PndWaitEvent);

    result = pClose->pnio_err;

    pThis->m_pPndAdapter->memFree( pRQB);

    result = pnd_free_event(pThis->m_PndWaitEvent);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }


    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<interface_close");

    pThis->m_set_status(PND_CLOSED);

    result = pnd_free_event(pThis->pnd_ifc_datarecord_read_rqb_list.resource_free_event);
    if (result != PNIO_RET_OK)
    {
        PND_FATAL("event alloc failed");
    }

    return PNIO_OK;
}

PNIO_UINT32 CIOInterface::set_ip_and_nos(PNIO_UINT32 handle,
    PNIO_SET_IP_NOS_MODE_TYPE Mode,
    const PNIO_IPv4 &IpV4,
    const PNIO_NOS &NoS)
{
    PND_RQB_PTR_TYPE         pRQB = PNIO_NULL;
    PND_SET_IP_AND_NOS_TYPE* pSetIpAndNos;

    if (this->m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (Mode != PNIO_SET_IP_MODE &&
        Mode != PNIO_SET_NOS_MODE &&
        Mode != (PNIO_SET_NOS_MODE | PNIO_SET_IP_MODE))
    {
        return PNIO_ERR_MODE_VALUE;
    }

    if (handle != get_handle(this))
        return PNIO_ERR_PRM_HND;

    if (m_set_ip_and_nos_running == PNIO_TRUE)
        return PNIO_ERR_NOT_REENTERABLE;

    if (((Mode & PNIO_SET_NOS_MODE) == PNIO_SET_NOS_MODE) && (NoS.Length > OHA_MAX_STATION_NAME_LEN))
    {
        return PNIO_ERR_PRM_NOS_LEN;
    }

    m_set_ip_and_nos_running = PNIO_TRUE;

    pRQB = (PND_RQB_PTR_TYPE) m_pPndAdapter->memAlloc(sizeof(*pRQB));

    if (!pRQB)
    {
        return PNIO_ERR_INTERNAL;
    }

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_SET_ADDRESS);
    LSA_RQB_SET_HANDLE(pRQB, (PNIO_UINT8)handle);
    PND_RQB_SET_USER_INST_PTR(pRQB, this);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CIOInterface::set_ip_and_nos_done);

    pSetIpAndNos = &pRQB->args.set_ip_and_nos;

    pSetIpAndNos->pnd_handle.cp_nr = (PNIO_UINT16)m_CpIndex;

    pSetIpAndNos->mode = Mode;
    pnd_memcpy(&pSetIpAndNos->IpV4, &IpV4, sizeof(PNIO_IPv4));
    pnd_memcpy(&pSetIpAndNos->NoS, &NoS, sizeof(PNIO_NOS));

    m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    return PNIO_OK;
}
/*lint -e{818} Pointer parameter 'pAddr' could be declared as pointing to const */
PNIO_UINT32 CIOInterface::record_read(PNIO_ADDR * pAddr, PNIO_REF ReqRef, PNIO_UINT32 RecordIndex, PNIO_UINT32 Length)
{
    PNIO_UINT32 result = PNIO_OK;
    PND_RQB_PTR_TYPE                pRQB = PNIO_NULL;
    PND_RECORD_READ_WRITE_PTR_TYPE  pRecRead = PNIO_NULL;
    PNIO_UINT8 * pRecBuffer = PNIO_NULL;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>record_read");

    if (this->m_Status != PND_OPENED)
        return PNIO_ERR_WRONG_HND;

    if (!pAddr ||
        pAddr->AddrType != PNIO_ADDR_LOG ||
        (pAddr->IODataType != PNIO_IO_IN && pAddr->IODataType != PNIO_IO_OUT))
        return PNIO_ERR_PRM_ADD;

    // sanity addres bound
    if (pAddr->u.Addr > 0x7FFF)
        return PNIO_ERR_PRM_ADD;

    result = check_log_adress(pAddr);
    if (result != PNIO_OK)
    {
        return result;
    }

    // check if interface address
    if (s_PndLogAddr.logAddrList[pAddr->u.Addr] != PDSUBSLOTINTERFACE &&
        s_PndLogAddr.logAddrList[pAddr->u.Addr] != PDSUBSLOTPORT)
    {
        return PNIO_ERR_PRM_ADD;
    }

    if (Length == 0 || Length > PNIOI_DREC_MAX_SIZE)
        return PNIO_ERR_VALUE_LEN;

    if (RecordIndex > 0xFFFF)
        return PNIO_ERR_PRM_REC_INDEX;

    pRQB = pnd_lower_rqb_alloc(PND_OPC_REC_READ, 0, this, CIOInterface::record_read_done);

    if (!pRQB)
    {
        return PNIO_ERR_NO_RESOURCE;
    }

    pRecBuffer = (PNIO_UINT8 *)m_pPndAdapter->memAlloc(Length);
    if (pRecBuffer == 0)
        PND_FATAL("mem alloc failed");

    pnd_memset(pRecBuffer, 0, Length);

    pRecRead = &pRQB->args.rec_read;

    pnd_memset(pRecRead, 0, sizeof(PND_RECORD_READ_WRITE_TYPE));

    pRecRead->Addr.AddrType = pAddr->AddrType;
    pRecRead->Addr.IODataType = pAddr->IODataType;
    pRecRead->Addr.u.Addr = pAddr->u.Addr;

    pRecRead->UserRef = ReqRef;

    pRecRead->RecordIndex = RecordIndex;
    pRecRead->Length = Length;
    pRecRead->pBuffer = pRecBuffer;

    pRecRead->pnd_handle.cp_nr = this->m_hInstanceHandle;

    m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<record_read");

    return result;
}

PNIO_UINT32 CIOInterface::rema_provide(PNIO_VOID_PTR_TYPE pInst, PNIO_UINT16 nrOfRemaResource)
{
    CIOInterface *pThis = (CIOInterface *)pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>rema_provide");

    PND_RQB_PTR_TYPE       pRQB = 0;
    PND_REMA_DATA_TYPE*    pRema;
    PNIO_UINT16            i = 0;

    for (i = 0; i< nrOfRemaResource; i++)
    {
        pRQB = (PND_RQB_PTR_TYPE)pThis->m_pPndAdapter->memAlloc( sizeof(*pRQB));

        if (!pRQB)
        {
            return PNIO_ERR_INTERNAL;
        }

        pRema = &pRQB->args.rema_data;

        pnd_memset(pRema, 0, sizeof(PND_REMA_DATA_TYPE));

        pRema->pnd_handle.cp_nr = pThis->m_hInstanceHandle;

        LSA_RQB_SET_OPCODE(pRQB, PND_OPC_REMA_PROVIDE);
        LSA_RQB_SET_HANDLE(pRQB, 0);
        PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
        PND_RQB_SET_USER_CBF_PTR(pRQB, CIOInterface::ifc_rema_done);

        pThis->m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);
    }

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<rema_provide");

    return PNIO_OK;
}

PNIO_VOID CIOInterface::rema_response(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT(pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>rema_response");

    CIOInterface *pThis = (CIOInterface *)pInst;

    LSA_RQB_SET_OPCODE(pRQB, PND_OPC_REMA_RESPONSE);
    LSA_RQB_SET_HANDLE(pRQB, 0);
    PND_RQB_SET_USER_INST_PTR(pRQB, pThis);
    PND_RQB_SET_USER_CBF_PTR(pRQB, CIOInterface::ifc_rema_done);

    pThis->m_pPndAdapter->requestStart(PND_MBX_ID_IO_BASE_CORE, (PND_REQUEST_FCT)pnio_user_request_start, (PNIO_VOID_PTR_TYPE)pRQB);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<rema_response");
}

PNIO_VOID CIOInterface::alarm_indication(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CIOInterface *pThis = (CIOInterface *)pInst;
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    PND_ALARM_PTR_TYPE     pAlarm;

    pAlarm = &rqb_ptr->args.iob_alarm;

    /* fill in alarm data */

    if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
    {
        // pThis->pnd_lower_rqb_free(rqb_ptr);
        pThis->m_pPndAdapter->memFree(rqb_ptr);
    }
    else
    {

        if (pThis)
        {
            PNIO_ADDR Addr;
            PNIO_CBE_PRM cbf_prm;
            PNIO_CTRL_ALARM_DATA AlarmData;

            pnd_memset(&AlarmData, 0, sizeof(AlarmData));

            cbf_prm.Handle = pThis->get_handle(pThis); // = CPIndex
            cbf_prm.CbeType = PNIO_CBE_IFC_ALARM_IND;

            cbf_prm.u.AlarmInd.IndRef = pAlarm->ind_ref;

            switch (pAlarm->alarm.AlarmType)
            {
            case PNIO_ALARM_REDUNDANCY:
            case PNIO_ALARM_PORT_DATA_CHANGED:
            case PNIO_ALARM_SYNC_DATA_CHANGED:
            case PNIO_ALARM_NETWORK_COMPONENT_PROBLEM:
            case PNIO_ALARM_TIME_DATA_CHANGED:
            {
                AlarmData.AlarmType = pAlarm->alarm.AlarmType;
                AlarmData.AlarmPriority = pAlarm->alarm.AlarmPriority;
                AlarmData.DeviceNum = pAlarm->alarm.DeviceNum;
                AlarmData.SlotNum = pAlarm->alarm.SlotNum;
                AlarmData.SubslotNum = pAlarm->alarm.SubslotNum;
                AlarmData.CpuAlarmPriority = pAlarm->alarm.CpuAlarmPriority;
                AlarmData.PnioCompatModtype = pAlarm->alarm.PnioCompatModtype;
                AlarmData.AlarmTinfo.CompatDevGeoaddr = pAlarm->alarm.AlarmTinfo.CompatDevGeoaddr;
                AlarmData.AlarmTinfo.ProfileType = pAlarm->alarm.AlarmTinfo.ProfileType;
                AlarmData.AlarmTinfo.AinfoType = pAlarm->alarm.AlarmTinfo.AinfoType;
                AlarmData.AlarmTinfo.ControllerFlags = pAlarm->alarm.AlarmTinfo.ControllerFlags;
                AlarmData.AlarmTinfo.DeviceFlag = pAlarm->alarm.AlarmTinfo.DeviceFlag;
                AlarmData.AlarmTinfo.PnioVendorIdent = pAlarm->alarm.AlarmTinfo.PnioVendorIdent;
                AlarmData.AlarmTinfo.PnioDevIdent = pAlarm->alarm.AlarmTinfo.PnioDevIdent;
                AlarmData.AlarmTinfo.PnioDevInstance = pAlarm->alarm.AlarmTinfo.PnioDevInstance;

                pnd_memcpy(AlarmData.DiagDs, pAlarm->alarm.DiagDs, sizeof(AlarmData.DiagDs));
                pnd_memcpy(AlarmData.PrAlarmInfo, pAlarm->alarm.PrAlarmInfo, sizeof(AlarmData.PrAlarmInfo));

                AlarmData.AlarmAinfo.BlockType = pAlarm->alarm.AlarmAinfo.BlockType;
                AlarmData.AlarmAinfo.BlockVersion = pAlarm->alarm.AlarmAinfo.BlockVersion;
                AlarmData.AlarmAinfo.Api = pAlarm->alarm.AlarmAinfo.Api;
                AlarmData.AlarmAinfo.AlarmSpecifier = pAlarm->alarm.AlarmAinfo.AlarmSpecifier;
                AlarmData.AlarmAinfo.ModIdent = pAlarm->alarm.AlarmAinfo.ModIdent;
                AlarmData.AlarmAinfo.SubIdent = pAlarm->alarm.AlarmAinfo.SubIdent;
                AlarmData.AlarmAinfo.UserStrucIdent = pAlarm->alarm.AlarmAinfo.UserStrucIdent;
                AlarmData.AlarmAinfo.UserAlarmDataLen = pAlarm->alarm.AlarmAinfo.UserAlarmDataLen;

#ifdef PNIO_ALARM_OLD_STRUC
                pnd_memcpy(AlarmData.AlarmAinfo.UserAlarmData, pAlarm->alarm.AlarmAinfo.UserAlarmData,
                    AlarmData.AlarmAinfo.UserAlarmDataLen);
#else
                if (pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData)
                {
                    pnd_memcpy(AlarmData.AlarmAinfo.UAData.UserAlarmData, pAlarm->alarm.AlarmAinfo.UAData.UserAlarmData,
                        AlarmData.AlarmAinfo.UserAlarmDataLen);
                }
#endif
                Addr.AddrType = pAlarm->addr.AddrType;
                Addr.IODataType = pAlarm->addr.IODataType;
                Addr.u.Addr = pAlarm->addr.u.Addr;
                cbf_prm.u.AlarmInd.pAddr = &Addr;
                break;
            }
            default:
            {
                // PND_FATAL("unknowen alarm type");
                break;
            }
            } // end switch

              // alarm callback 

            cbf_prm.u.AlarmInd.pAlarmData = &AlarmData;

            if (pThis->m_pUserCbf_Alarm)
            {
                pThis->m_pUserCbf_Alarm(&cbf_prm);
            }
            else
            {
                PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF_ALARM_IND user callback() defined");
            }

            // reprovide alarm resource
            pThis->alarm_response(rqb_ptr);

        } // end if 
        else
        {
            PND_FATAL("wrong  instance ");
        }

    } // end else
}

PNIO_VOID CIOInterface::alarm_response(PNIO_VOID_PTR_TYPE  rqb_ptr)
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_response");

    PND_RQB_PTR_TYPE pRQB = (PND_RQB_PTR_TYPE)rqb_ptr;
    PND_ASSERT(pRQB);

    CIOBaseCoreCommon::alarm_response(pRQB, PNIO_REF_IFC, CIOInterface::alarm_indication);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_response");
}

PNIO_UINT32 CIOInterface::alarm_provide(PNIO_UINT16 nrOfAlarmResource)
{
    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, ">>alarm_provide");
    
    CIOBaseCoreCommon::alarm_provide(nrOfAlarmResource, PNIO_REF_IFC, CIOInterface::alarm_indication);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_CHAT, "<<alarm_provide");

    return PNIO_OK;
}



/* callbacks */
PNIO_VOID CIOInterface::record_read_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CIOInterface *pThis = (CIOInterface *)pInst;

    PNIO_ADDR Addr;
    PNIO_CBE_PRM cbf_prm;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>read_write_record_done");

    switch (LSA_RQB_GET_OPCODE(rqb_ptr))
    {
    case PND_OPC_REC_READ:
    {
        PND_RECORD_READ_WRITE_PTR_TYPE pRecRead = &rqb_ptr->args.rec_read;

        cbf_prm.CbeType = PNIO_CBE_IFC_REC_READ_CONF;
        cbf_prm.Handle = get_handle(pThis);

        Addr.AddrType = (PNIO_ADDR_TYPE)pRecRead->Addr.AddrType;
        Addr.IODataType = (PNIO_IO_TYPE)pRecRead->Addr.IODataType;
        Addr.u.Addr = pRecRead->Addr.u.Addr;
        cbf_prm.u.RecReadConf.pAddr = &Addr;

        cbf_prm.u.RecReadConf.RecordIndex = pRecRead->RecordIndex;

        cbf_prm.u.RecReadConf.ReqRef = pRecRead->UserRef;
        cbf_prm.u.RecReadConf.pBuffer = (PNIO_UINT8 *)pRecRead->pBuffer;
        cbf_prm.u.RecReadConf.Length = pRecRead->Length;

        cbf_prm.u.RecReadConf.Err.ErrCode = pRecRead->err.ErrCode;
        cbf_prm.u.RecReadConf.Err.ErrDecode = pRecRead->err.ErrDecode;
        cbf_prm.u.RecReadConf.Err.ErrCode1 = pRecRead->err.ErrCode1;
        cbf_prm.u.RecReadConf.Err.ErrCode2 = pRecRead->err.ErrCode2;
        cbf_prm.u.RecReadConf.Err.AddValue1 = (PNIO_UINT16)pRecRead->pnio_err;
        cbf_prm.u.RecReadConf.Err.AddValue2 = (PNIO_UINT16)pRecRead->pnio_err;

        if (pThis->m_pUserCbf_DR_Read)
        {
            pThis->m_pUserCbf_DR_Read(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "NO CBF for Data Record Read/Write defined. ");
        }

        pThis->m_pPndAdapter->memFree(pRecRead->pBuffer); // free record read Buffer allocated by record_read
        break;
    }
    default:
        PND_FATAL("wrong OPCODE");
    }

    pThis->pnd_lower_rqb_free(pRqb);

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<read_write_record_done");
}

PNIO_VOID CIOInterface::set_ip_and_nos_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CIOInterface *pThis = (CIOInterface *)pInst;

    PNIO_CBE_PRM cbf_prm;
    cbf_prm.Handle = pThis->get_handle(pThis);
    cbf_prm.CbeType = PNIO_CBE_IFC_SET_ADDR_CONF;

    if (pThis->m_pUserCbf_SetIpAndNos)
    {
        cbf_prm.u.SetAddrConf.pStationName = rqb_ptr->args.set_ip_and_nos.NoS.Nos;
        cbf_prm.u.SetAddrConf.StationNameLen = rqb_ptr->args.set_ip_and_nos.NoS.Length;
        pnd_memcpy(cbf_prm.u.SetAddrConf.LocalIPAddress, rqb_ptr->args.set_ip_and_nos.IpV4.IpAddress, 4);
        pnd_memcpy(cbf_prm.u.SetAddrConf.LocalSubnetMask, rqb_ptr->args.set_ip_and_nos.IpV4.NetMask, 4);
        pnd_memcpy(cbf_prm.u.SetAddrConf.DefaultRouterAddr, rqb_ptr->args.set_ip_and_nos.IpV4.Gateway, 4);
        cbf_prm.u.SetAddrConf.Options = rqb_ptr->args.set_ip_and_nos.Options;
        cbf_prm.u.SetAddrConf.Err = rqb_ptr->args.set_ip_and_nos.pnio_err;
        cbf_prm.u.SetAddrConf.Mode = rqb_ptr->args.set_ip_and_nos.mode;

        pThis->m_pUserCbf_SetIpAndNos(&cbf_prm);
    }
    else
    {
        PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Set IP and NoS user callback() defined. ApplHandle= %d Mode= %d", cbf_prm.Handle, pThis->m_pUserCbf_SetIpAndNos);
    }

    pThis->m_set_ip_and_nos_running = PNIO_FALSE;

    pThis->m_pPndAdapter->memFree(pRqb);
}

PNIO_VOID CIOInterface::ifc_rema_done(PNIO_VOID_PTR_TYPE  pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    PND_RQB_PTR_TYPE rqb_ptr = (PND_RQB_PTR_TYPE)pRqb;
    CIOInterface *pThis = (CIOInterface *)pInst;

    PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, ">>ifc_rema_done");

    if (LSA_RQB_GET_RESPONSE(rqb_ptr) == PNIO_OK_CANCEL)
    {
        pThis->m_pPndAdapter->memFree(rqb_ptr);
    }
    else
    {

        PNIO_CBE_PRM cbf_prm;
        cbf_prm.Handle = pThis->get_handle(pThis);
        cbf_prm.CbeType = PNIO_CBE_REMA_READ_CONF;

        if (pThis->m_pUserCbf_RemaRead)
        {
            cbf_prm.u.RemaReadConf.Err = rqb_ptr->args.rema_data.pnio_err;
            cbf_prm.u.RemaReadConf.RemaXMLBuffer = rqb_ptr->args.rema_data.remaXMLbuffer;
            cbf_prm.u.RemaReadConf.RemaXMLBufferLength = rqb_ptr->args.rema_data.remaXMLbufferLength;

            pThis->m_pUserCbf_RemaRead(&cbf_prm);
        }
        else
        {
            PND_IOBC_TRACE_02(0, LSA_TRACE_LEVEL_NOTE_HIGH, "No CBF for Rema Read user callback() defined. ApplHandle = %d Cbf = %d", cbf_prm.Handle, pThis->m_pUserCbf_RemaRead);
        }

        if(rqb_ptr->args.rema_data.data != PNIO_NULL)
        {        
            pThis->m_pPndAdapter->memFree(rqb_ptr->args.rema_data.data);
        }
		pThis->m_pPndAdapter->memFree(rqb_ptr->args.rema_data.remaXMLbuffer);

        PND_IOBC_TRACE_00(0, LSA_TRACE_LEVEL_NOTE_HIGH, "<<ifc_rema_done");

        // reprovide rema resource
        rema_response(pThis, rqb_ptr);
    }

}

PNIO_VOID CIOInterface::interface_open_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CIOInterface *pThis = (CIOInterface *)pInst;

    LSA_UNUSED_ARG(pRqb);

    pnd_set_event(pThis->m_PndWaitEvent);
}

PNIO_VOID CIOInterface::interface_close_done(PNIO_VOID_PTR_TYPE pInst, PNIO_VOID_PTR_TYPE  pRqb)
{
    CIOInterface *pThis = (CIOInterface *)pInst;

    LSA_UNUSED_ARG(pRqb);

    pnd_set_event(pThis->m_PndWaitEvent);
}

/*****************************************************************************/
/*  Copyright (C) 2019 Siemens Aktiengesellschaft. All rights reserved.      */
/*****************************************************************************/